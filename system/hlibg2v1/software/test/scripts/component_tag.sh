#!/bin/sh


PRODUCT=8190
COMPONENT=`echo "$1" | tr '[:lower:]' '[:upper:]'` # has to be one of: H264, MPEG4, JPEG or PP

TAG_PREFIX=sw$PRODUCT

case $COMPONENT in
     "H264") PRODUCT=8170 ;
             TAG_PREFIX=sw$PRODUCT ;     
             TAG_PREFIX=${TAG_PREFIX}h264 ;
             VERSION_SOURCE="../../source/h264/h264decapi.c" ;
             DECAPI="H264DEC" ;;
     "H264HIGH")
             TAG_PREFIX=${TAG_PREFIX}h264 ;
             VERSION_SOURCE="../../source/h264high/h264decapi.c" ;
             DECAPI="H264DEC" ;;             
     "MPEG4") TAG_PREFIX=${TAG_PREFIX}mpeg4 ;
             VERSION_SOURCE="../../source/mpeg4/mp4decapi.c" ;
             DECAPI="MP4DEC" ;;
     "MPEG2") TAG_PREFIX=${TAG_PREFIX}mpeg2 ;
             VERSION_SOURCE="../../source/mpeg2/mpeg2decapi.c" ;
             DECAPI="MPEG2DEC" ;;
      "VC1") TAG_PREFIX=${TAG_PREFIX}vc1 ;
             VERSION_SOURCE="../../source/vc1/vc1decapi.c" ;
             DECAPI="VC1DEC" ;;
      "JPEG") TAG_PREFIX=${TAG_PREFIX}jpeg ;
             VERSION_SOURCE="../../source/jpeg/jpegdecapi.c" ;
             DECAPI="JPG" ;;
      "RV")  TAG_PREFIX=sw$PRODUCT ;     
             TAG_PREFIX=${TAG_PREFIX}rv ;
             VERSION_SOURCE="../../source/rv/rvdecapi.c" ;
             DECAPI="RVDEC" ;;             
      "VP6") PRODUCT=9190 ;
             TAG_PREFIX=sw$PRODUCT ;     
             TAG_PREFIX=${TAG_PREFIX}vp6 ;
             VERSION_SOURCE="../../source/vp6/vp6hwd_api.c" ;
             DECAPI="VP6DEC" ;;             
      "VP8") PRODUCT=9190 ;
             TAG_PREFIX=sw$PRODUCT ;     
             TAG_PREFIX=${TAG_PREFIX}vp8 ;
             VERSION_SOURCE="../../source/vp8/vp8decapi.c" ;
             DECAPI="VP8DEC" ;;             
      "PP") TAG_PREFIX=${TAG_PREFIX}pp ;
             VERSION_SOURCE="../../source/pp/ppapi.c" ;
             DECAPI="PP" ;;
      "AVS") PRODUCT=9190 ;
             TAG_PREFIX=sw$PRODUCT ;     
             TAG_PREFIX=${TAG_PREFIX}avs ;
             VERSION_SOURCE="../../source/avs/avsdecapi.c" ;
             DECAPI="AVSDEC" ;;
          *) echo -e "Unknown component \"$COMPONENT\"."
             echo -e "Has to be one of H264, H264HIGH, MPEG4, JPEG, MPEG2, VC1,
	     RV, VP6, VP8, AVS or PP\nQuitting..."
             exit 1 ;;     
esac


isdigit ()    # Tests whether *entire string* is numerical.
{             # In other words, tests for integer variable.
  [ $# -eq 1 ] || return 1

  case $1 in
  *[!0-9]*|"") return 1;;
            *) return 0;;
  esac
}

# check and list if there are any modified files in the project
res=`cvs diff --brief ../.. 2>&1`

if [ $? != 0 ]
then
    echo -e "WARNING! Found locally modified files!"
    echo "$(echo -e "$res" | grep "Index:" | awk '{print $2}')"
    echo -n "Continue? [y/n] "
    read ans
    if [ "$ans" != "y" ]
    then
        echo -e "\tNO, check the differences then!\nQuitting..."
        exit 1
    else
        echo -e "\tYES, but you have been warned!"
    fi   
fi


# check if API implementation is up-to-date; must be!
echo -e "\nChecking CVS status of \"$VERSION_SOURCE\""
cvs_stat=$(cvs stat $VERSION_SOURCE | grep Status:)

echo "$cvs_stat"

if [ "$(echo -e "$cvs_stat" | awk '{print $4}')" != "Up-to-date" ]
then
    echo -e "Please check in all changes first!\nQuitting..."
    exit 1
fi

prev_MAJOR=`sed -n -e 's/\(#define '$DECAPI'_BUILD_MAJOR \)\([0-9]*\)/\2/p' $VERSION_SOURCE`
prev_MINOR=`sed -n -e 's/\(#define '$DECAPI'_BUILD_MINOR \)\([0-9]*\)/\2/p' $VERSION_SOURCE`

# ask for new major and minor in command line
echo -e "\nCreating TAG for $PRODUCT $COMPONENT software."
echo -e "TAG will look like: ${TAG_PREFIX}_MAJOR_MINOR_b \n"

echo -e "Previous build number was: \t${prev_MAJOR}.$prev_MINOR"
echo -e "Please give new MAJOR and MINOR build numbers next.\n"

echo -n "MAJOR build number: "
read MAJOR

if ! isdigit "$MAJOR"
then
  echo -e "\"$MAJOR\" has at least one non-digit character.\nQuitting..."
  exit 1
fi

echo -en "\nMINOR build number: "
read MINOR

if ! isdigit "$MINOR"
then
  echo -e "\"$MINOR\" has at least one non-digit character.\nQuitting..."
  exit 1
fi

TAGNAME=${TAG_PREFIX}_${MAJOR}_${MINOR}_b #now we have our new TAG

#major=`echo $tagname | sed -n 's/\(sw6280h\)\(_\)\([0-9]*\)\(_\)\([0-9]*\)/\3/p'`
#minor=`echo $tagname | sed -n 's/\(sw6280h\)\(_\)\([0-9]*\)\(_\)\([0-9]*\)/\5/p'`

# chck validity of new build version
if [ $(expr $prev_MAJOR \* 1000 + $prev_MINOR) -ge $(expr $MAJOR \* 1000 + $MINOR) ]
then
    echo "New version: ${MAJOR}.$MINOR NOT OK. Previous one: ${prev_MAJOR}.$prev_MINOR"
    exit 1 
fi

echo -en "\nTAG to be created \"$TAGNAME\". Is this OK? [y/n]: "
read ans

if [ "$ans" != "y" ]
then
    echo -e "TAG name was NOT confirmed to be OK! Type 'y' next time!\nQuitting..."
    exit 1
fi

# update new version number to CVS
echo -e "\nUpdating \"$VERSION_SOURCE\" with new build number..."

sed -e 's/\(#define '$DECAPI'_BUILD_MAJOR \)\([0-9]*\)/\1'$MAJOR'/' \
    -e 's/\(#define '$DECAPI'_BUILD_MINOR \)\([0-9]*\)/\1'$MINOR'/' $VERSION_SOURCE > foo.c

echo "Previous version saved as: ${VERSION_SOURCE}.old"
mv $VERSION_SOURCE ${VERSION_SOURCE}.old
mv foo.c $VERSION_SOURCE

echo -en "cvs commit $VERSION_SOURCE\t"
if (cvs commit -m "build number update for tag $TAGNAME" $VERSION_SOURCE &> /dev/null)
then echo "OK"
else 
    echo "FAILED"
    exit 1
fi

# and in the end do the tagging
tagger_dir="$(echo "$COMPONENT" | tr '[:upper:]' '[:lower:]')"
#tagger_dir=.
current_dir=$(pwd)

echo -e "\nTagging component using \"${tagger_dir}/tagger.sh\""
cd ../$tagger_dir
./tagger.sh $TAGNAME $2
cd $current_dir

echo -e "\nAll Done!\n"
exit 0
