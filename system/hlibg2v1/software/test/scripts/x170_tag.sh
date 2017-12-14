
# Create one tag for X170 software

# Name of your tag here:
#X170_TAG=sw7190_0_1
X170_TAG=tt5
T7190=sw7190_0_9
# update here the new existing tags you want to be in the new tag:
MPEG4_TAG=sw7190m_0_5
H264_TAG=sw7190h_0_8
JPEG_TAG=sw7190j_0_3
VC1_TAG=sw7190v_0_10
PP_TAG=sw7190p_0_6

MAIL=tagmail.txt


cvs tag $X170_TAG x170_tag.sh

# vc-1
mkdir v
cd v
cvs co -r $VC1_TAG 7190_decoder/software
cvs tag $X170_TAG
cd -

# mpeg-4
mkdir m
cd m
cvs co -r $MPEG4_TAG 7190_decoder/software
cvs tag $X170_TAG
cd -

# pp
mkdir p
cd p
cvs co -r $PP_TAG 7190_decoder/software
cvs tag $X170_TAG
cd -

# h264
mkdir h
cd h
cvs co -r $H264_TAG 7190_decoder/software
cvs tag $X170_TAG
cd -


# jpeg
mkdir j
cd j
cvs co -r $JPEG_TAG 7190_decoder/software
cvs tag $X170_TAG
cd -

rm -rf m h j p v

cvs co -r $X170_TAG 7190_decoder/software
cd 7190_decoder/software/test/scripts/swhw/

echo "Compile report:" > $MAIL
echo >> $MAIL
./make_all.sh >> $MAIL
cd -
mv 7190_decoder/software/test/scripts/swhw/$MAIL .

echo >> $MAIL
echo Included tags: >> $MAIL

echo >> $MAIL
echo $MPEG4_TAG >> $MAIL
echo $H264_TAG >> $MAIL
echo $JPEG_TAG >> $MAIL
echo $VC1_TAG >> $MAIL
echo $PP_TAG >> $MAIL

rm -rf 7190_decoder
cvs co -r $X170_TAG 7190_decoder/software
cd 7190_decoder/software
cvs tag -d $X170_TAG
cvs tag $T7190
cd ../../

cvs co hw_tools
./hw_tools/email $MAIL mahe "Tag add: $T7190"

rm -r hw_tools
rm -r 7190_decoder

