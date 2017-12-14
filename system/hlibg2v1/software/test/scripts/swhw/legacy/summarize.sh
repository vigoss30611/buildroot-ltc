#!/bin/bash
#-------------------------------------------------------------------------------
#-                                                                            --
#-       This software is confidential and proprietary and may be used        --
#-        only as expressly authorized by a licensing agreement from          --
#-                                                                            --
#-                            Hantro Products Oy.                             --
#-                                                                            --
#-                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
#-                            ALL RIGHTS RESERVED                             --
#-                                                                            --
#-                 The entire notice above must be reproduced                 --
#-                  on all copies and should not be removed.                  --
#-                                                                            --
#-------------------------------------------------------------------------------
#-
#--   Abstract     : Script for summarizing test results from various         --
#--                  configuration reports                                    --
#--                                                                           --
#-------------------------------------------------------------------------------

#=====--------------------------------------------------------------------=====#
#. ./config.sh
. ./commonconfig.sh
. ./f_utils.sh "summary"

REPORTIME=`date +%d%m%y_%k%M | sed -e 's/ //'`
CSV_FILE="${CSV_FILE_PATH}integrationreport_${HWBASETAG}_${SWTAG}_${USER}_${REPORTIME}${CSV_FILE_NAME_SUFFIX}_summary.csv"
STATUS_FILE=${CSV_FILE_PATH}${PRODUCT}_status.csv

ALL_CONFIGURATIONS="\
integration_default \
integration_pbyp \
integration_sliceud \
integration_wholestrm \
integration_nal \
integration_memext \
integration_rlc \
integration_rndcfg \
integration_rndtc \
integration_rndtc_rlc \
integration_rlc_pbyp \
integration_rlc_nal \
integration_rlc_wholestrm"

# Not yet supported
#export ALL_CONFIGURATIONS="${ALL_CONFIGURATIONS} integration_mbuffer"

# All cases
integration_default_cases='"h264 mpeg4 h263 sorenson vc1 mpeg2 mpeg1 jpeg pp"'
integration_pbyp_cases='"h264 mpeg4 h263 sorenson vc1_adv vc1_pp_multi_inst mpeg2 mpeg1"'
#integration_pbyp_cases='"\
# 2220 2221 2222 2223 2224 2225 2226 2227 2228 2229 2230 2231 2232 2233 2234 2235 2236 2237 2239 2241 2243 2602 2609 2610 2614 2616 2652 2655 2656 2674 4018 417 418 419 425 5050 5080 5081 5135 5136 5137 5138 5139 5140 5141 5142 5143 5144 5145 5146 5147 5148 5149 5150 5151 5152 5153 5154 5155 5156 5157 5158 5159 5160 5165 5166 5167 5168 5169 5170 5171 5172 5173 5174 5175 5176 5177 5178 5179 5180 5181 5182 6000 6001 6002 6003 6004 6005 6006 6007 6008 6009 6010 6011 6012 6013 6014 6015 6016 6017 6018 6019 6020 6021 6022 6023 6024 6025 6028 6029 6030 6031 6032 6033 6034 6035 6036 6037 6042 6043 6044 6045 6046 6047 6048 6049 6050 6051 6052 6053 6054 6055 6056 6057 6058 6059 6060 6061 6062 6063 6064 6065 6068 6069 6070 6071 6072 6073 6074 6075 6076 6077 6079 6080 6081 6082 6083 6084 6086 6087 6088 6089 6090 6091 6092 6093 6094 6095 6096 6097 6098 6099 6100 6101 6102 6103 6104 6105 6106 6107 6108 6109 611 6110 6111 6112 6113 6114 6115 6116 6117 6118 6119 6120 6121 6122 6123 6124 6125 6126 6127 6128 6129 613 6130 6131 6132 6133 6134 6135 6136 6137 6138 6139 614 6140 6141 6142 6143 6144 6145 6146 6147 6148 6149 615 6150 6151 6152 6153 6154 6155 6156 6157 6158 6159 6160 6161 6162 6163 6164 6165 6166 6167 6168 6169 6170 6171 6172 6173 6174 6175 6176 6177 6178 6179 6180 6181 6182 6183 6184 6185 6186 6187 6188 6189 6190 6191 6192 6193 6194 6195 6196 6197 6198 6199 6200 6201 6202 6203 6204 6205 6206 6207 6208 6209 6210 6211 6212 6213 6214 6215 6216 6217 6218 6219 6220 6221 6222 6223 6224 6225 6226 6227 6228 6229 6230 6231 6232 6233 6234 6235 6236 6237 6238 6239 6240 6241 6242 6243 6244 6245 6246 6247 6248 6249 6250 6251 6252 6253 6254 6255 6256 6257 6258 6259 6260 6261 6262 6263 6264 6265 6266 6267 6268 6269 6270 6271 6272 6273 6274 6275 6276 6277 6278 6279 6280 6281 6282 6283 6284 6285 6286 6287 6288 6289 6290 6291 6292 6293 6294 6295 6296 6297 6298 6299 6300 6301 6302 6303 6304 6305 6306 6307 6308 6309 6310 6311 6312 6313 6314 6315 6316 6317 6318 6319 6320 6321 6322 6323 6324 6325 6326 6327 6328 6329 6330 6331 6332 6333 6334 6335 6336 6337 6338 6339 6340 6341 6342 6343 6344 6345 6346 6347 6348 6349 6350 6351 6352 6353 6354 6355 6356 6357 6358 6359 6360 6361 6362 6363 6364 6365 6366 6367 6368 6369 6370 6371 6372 6373 6374 6375 6376 6377 6378 6379 6380 6381 6382 6383 6384 6385 6386 6387 6388 6389 6390 6391 6392 6393 6394 6395 6396 6397 6398 6399 6400 6401 6402 6403 6404 6405 6406 6407 6408 6409 6410 6411 6412 6413 6414 6415 6416 6417 6418 6419 6420 6421 6422 6423 6424 6425 6426 6427 6428 6429 6430 6431 6432 6433 6434 6435 6436 6437 700 7010 7013 7014 7019 7022 7031 7034 7035 7036 704 7044 7045 7049 7052 7053 7054 7055 7058 7059 706 7060 7063 7066 7067 7068 708 711 7114 7116 7117 7119 712 7120 7122 7123 7124 7126 7130 7133 7138 7139 714 7140 7141 7143 7144 7146 7147 7149 7151 7152 7154 7155 7156 7157 7158 7160 7161 7162 7163 7166 7167 7168 717 7170 7174 7175 7176 7177 7178 718 719 7209 721 7212 7213 7214 722 7221 7225 723 7232 7233 7235 7238 724 7240 7241 7242 7243 7244 7249 725 7250 7252 7253 7254 7255 7256 726 7261 727 728 729 7307 7312 7317 7327 7332 7333 7334 7335 7336 7337 7338 7339 7341 7345 7346 7348 7349 7350 7352 7353 7355 7357 7358 7359 7360 7361 7362 7364 7365 7366 7367 7368 7369 7370 7372 7373 745 746 747 748 749 750 751 7516 752 753 754 755 756 757 758 759 760 761 762 763 764 765 766 767 768 769 770 771 772 773 774 775 776 777 778 802 809 810 812 816 822 836 837 838 900 901 961\
# 100 101 102 103 104 105 106 107 108 109 11 110 113 114 115 116 117 118 119 13 131 132 15 16 168 169 170 171 172 173 174 175 176 177 178 179 21 2104 2118 2127 222 223 224 225 226 227 228 229 23 230 231 235 236 24 243 244 246 247 249 2534 2535 2536 294 34 40 41 43 44 4506 4508 4510 4514 46 47 50 51 52 53 54 56 5611 5615 5616 5618 5619 5623 5626 5627 5628 5629 5630 5637 5642 5643 5651 5657 5665 5666 5668 5671 5673 5674 5675 5676 5677 5684 5686 5688 5695 5696 5697 5698 5699 57 5700 5701 5702 5704 5705 5706 5707 5709 5710 58 59 60 61 62 66 67 68 69 70 73 74 75 76 77 78 79 80 81 84 85 86 87 88 89 90 9003 91 92 93 94\
# vc1_adv vc1_pp_multi_inst\
# 2401 2403 2404 2405 2406 2407 2408 2409 2410 2411 2412 2413 2414 2415 2416 2417 2418 2419 2420 2421 2422 2423 2424 2425 2426 2427 2428 2429 2430 2431 2432 2433 2434 2435 2436 2437 2438 2439 2440 2441 2442 2443 2444 2445 2446 2447 3000 3001 3002 3003 3004 3005 3006 3007 3008 3009 3010 3011 3012 3013 3014 3015 4013 4014 4100 4101 4102 4103 4104 4105 4106 4107 4108 4109 4110 4111 4112 4113 4114 4115 4116 4117 4118 4119 4120 4121 4122 4123 4124 4125 4126 4127 4128 4129 4130 4131 4132 4133 4134 4135 4136 4137 4138 4140 4141 4142 4143 4144 4145 4146 4147 4148 4149 4150 4151 4152 5900 5901 5902 9000 9001 900"'
integration_wholestrm_cases='"h264 mpeg4 h263 sorenson mpeg2 mpeg1"'
integration_nal_cases='"h264"'
#integration_nal_cases='"\
# 2220 2221 2222 2223 2224 2225 2226 2227 2228 2229 2230 2231 2232 2233 2234 2235 2236 2237 2239 2241 2243 2602 2609 2610 2614 2616 2652 2655 2656 2674 4018 417 418 419 425 5050 5080 5081 5135 5136 5137 5138 5139 5140 5141 5142 5143 5144 5145 5146 5147 5148 5149 5150 5151 5152 5153 5154 5155 5156 5157 5158 5159 5160 5165 5166 5167 5168 5169 5170 5171 5172 5173 5174 5175 5176 5177 5178 5179 5180 5181 5182 6000 6001 6002 6003 6004 6005 6006 6007 6008 6009 6010 6011 6012 6013 6014 6015 6016 6017 6018 6019 6020 6021 6022 6023 6024 6025 6028 6029 6030 6031 6032 6033 6034 6035 6036 6037 6042 6043 6044 6045 6046 6047 6048 6049 6050 6051 6052 6053 6054 6055 6056 6057 6058 6059 6060 6061 6062 6063 6064 6065 6068 6069 6070 6071 6072 6073 6074 6075 6076 6077 6079 6080 6081 6082 6083 6084 6086 6087 6088 6089 6090 6091 6092 6093 6094 6095 6096 6097 6098 6099 6100 6101 6102 6103 6104 6105 6106 6107 6108 6109 611 6110 6111 6112 6113 6114 6115 6116 6117 6118 6119 6120 6121 6122 6123 6124 6125 6126 6127 6128 6129 613 6130 6131 6132 6133 6134 6135 6136 6137 6138 6139 614 6140 6141 6142 6143 6144 6145 6146 6147 6148 6149 615 6150 6151 6152 6153 6154 6155 6156 6157 6158 6159 6160 6161 6162 6163 6164 6165 6166 6167 6168 6169 6170 6171 6172 6173 6174 6175 6176 6177 6178 6179 6180 6181 6182 6183 6184 6185 6186 6187 6188 6189 6190 6191 6192 6193 6194 6195 6196 6197 6198 6199 6200 6201 6202 6203 6204 6205 6206 6207 6208 6209 6210 6211 6212 6213 6214 6215 6216 6217 6218 6219 6220 6221 6222 6223 6224 6225 6226 6227 6228 6229 6230 6231 6232 6233 6234 6235 6236 6237 6238 6239 6240 6241 6242 6243 6244 6245 6246 6247 6248 6249 6250 6251 6252 6253 6254 6255 6256 6257 6258 6259 6260 6261 6262 6263 6264 6265 6266 6267 6268 6269 6270 6271 6272 6273 6274 6275 6276 6277 6278 6279 6280 6281 6282 6283 6284 6285 6286 6287 6288 6289 6290 6291 6292 6293 6294 6295 6296 6297 6298 6299 6300 6301 6302 6303 6304 6305 6306 6307 6308 6309 6310 6311 6312 6313 6314 6315 6316 6317 6318 6319 6320 6321 6322 6323 6324 6325 6326 6327 6328 6329 6330 6331 6332 6333 6334 6335 6336 6337 6338 6339 6340 6341 6342 6343 6344 6345 6346 6347 6348 6349 6350 6351 6352 6353 6354 6355 6356 6357 6358 6359 6360 6361 6362 6363 6364 6365 6366 6367 6368 6369 6370 6371 6372 6373 6374 6375 6376 6377 6378 6379 6380 6381 6382 6383 6384 6385 6386 6387 6388 6389 6390 6391 6392 6393 6394 6395 6396 6397 6398 6399 6400 6401 6402 6403 6404 6405 6406 6407 6408 6409 6410 6411 6412 6413 6414 6415 6416 6417 6418 6419 6420 6421 6422 6423 6424 6425 6426 6427 6428 6429 6430 6431 6432 6433 6434 6435 6436 6437 700 7010 7013 7014 7019 7022 7031 7034 7035 7036 704 7044 7045 7049 7052 7053 7054 7055 7058 7059 706 7060 7063 7066 7067 7068 708 711 7114 7116 7117 7119 712 7120 7122 7123 7124 7126 7130 7133 7138 7139 714 7140 7141 7143 7144 7146 7147 7149 7151 7152 7154 7155 7156 7157 7158 7160 7161 7162 7163 7166 7167 7168 717 7170 7174 7175 7176 7177 7178 718 719 7209 721 7212 7213 7214 722 7221 7225 723 7232 7233 7235 7238 724 7240 7241 7242 7243 7244 7249 725 7250 7252 7253 7254 7255 7256 726 7261 727 728 729 7307 7312 7317 7327 7332 7333 7334 7335 7336 7337 7338 7339 7341 7345 7346 7348 7349 7350 7352 7353 7355 7357 7358 7359 7360 7361 7362 7364 7365 7366 7367 7368 7369 7370 7372 7373 745 746 747 748 749 750 751 7516 752 753 754 755 756 757 758 759 760 761 762 763 764 765 766 767 768 769 770 771 772 773 774 775 776 777 778 802 809 810 812 816 822 836 837 838 900 901 961"'
integration_vc1ud_cases='"vc1_adv"'
# integration_vc1ud_cases='"2839 4346 4400 4401 4422 5802"'
integration_memext_cases='"jpeg"'
integration_rlc_cases='"h264 mpeg4 h263" -n "_hp_ _mp_ _high_ _main_ _cabac_ cabac_ asp mpeg4_error"'
integration_rndcfg_cases='"h264 mpeg4 h263 sorenson vc1 mpeg2 mpeg1 jpeg pp"'
integration_rndtc_cases='"h264 h263 mpeg4 sorenson mpeg2 mpeg1 jpeg vc1 pp"'
integration_rndtc_rlc_cases='"h264 mpeg4 h263" -n "_hp_ _mp_ _high_ _main_ _cabac_ cabac_ asp mpeg4_error"'
integration_rlc_pbyp_cases='"h264 mpeg4 h263" -n "_hp_ _mp_ _high_ _main_ _cabac_ cabac_ asp mpeg4_error"'
# integration_rlc_pbyp_cases='"\
# 100 101 102 103 104 105 106 107 108 109 11 110 113 114 115 116 117 118 119 13 131 132 15 168 169 170 171 172 173 174 175 176 177 178 179 21 222 223 224 225 226 227 228 229 23 230 231 235 236 24 243 244 246 247 249 294 34 40 41 46 50 51 5611 5615 5616 5618 5619 5623 5626 5627 5628 5629 5630 5637 5642 5643 5651 5657 5665 5666 5668 5671 5673 5674 5675 5676 5677 5684 5686 5688 5695 5696 5697 5698 5699 57 5700 5701 5702 5704 5705 5706 5707 5709 5710 58 59 60 61 62 66 67 68 69 70 73 74 75 76 77 78 79 80 9003 92\
# 2220 2221 2226 2227 2236 2237 2239 4018 417 418 419 425 5050 5135 5136 5137 5138 5139 5140 5141 5142 5143 5144 5145 5146 5147 5148 5149 5150 5151 5152 5153 5154 6000 6001 6002 6003 6004 6005 6006 6007 6008 6009 6010 6011 6012 6013 6014 6015 6016 6017 6018 6019 6020 6021 6022 6023 6024 6025 6028 6029 6030 6031 6032 6033 6034 6035 6036 6037 6042 6043 6044 6045 6046 6047 6048 6049 6050 6051 6052 6053 6054 6055 6056 6057 6058 6059 6060 6061 6062 6063 6064 6065 6068 6069 6070 6071 6072 6073 6074 6075 6076 6077 6079 6080 6081 6082 6083 6084 6086 6087 6088 6089 611 613 614 615 700 704 706 708 711 712 714 717 718 719 721 722 723 724 725 726 727 728 729 745 746 747 748 749 750 751 752 753 754 755 756 757 758 759 760 761 762 763 764 765 766 767 768 769 770 771 772 773 774 775 776 777 778 802 809 810 812 816 822 836 837 838 900 901"'
integration_rlc_nal_cases='"h264" -n "_hp_ _mp_ _high_ _main_ _cabac_ cabac_ "'
#integration_rlc_nal_cases='"
# 2220 2221 2226 2227 2236 2237 2239 4018 417 418 419 425 5050 5135 5136 5137 5138 5139 5140 5141 5142 5143 5144 5145 5146 5147 5148 5149 5150 5151 5152 5153 5154 6000 6001 6002 6003 6004 6005 6006 6007 6008 6009 6010 6011 6012 6013 6014 6015 6016 6017 6018 6019 6020 6021 6022 6023 6024 6025 6028 6029 6030 6031 6032 6033 6034 6035 6036 6037 6042 6043 6044 6045 6046 6047 6048 6049 6050 6051 6052 6053 6054 6055 6056 6057 6058 6059 6060 6061 6062 6063 6064 6065 6068 6069 6070 6071 6072 6073 6074 6075 6076 6077 6079 6080 6081 6082 6083 6084 6086 6087 6088 6089 611 613 614 615 700 704 706 708 711 712 714 717 718 719 721 722 723 724 725 726 727 728 729 745 746 747 748 749 750 751 752 753 754 755 756 757 758 759 760 761 762 763 764 765 766 767 768 769 770 771 772 773 774 775 776 777 778 802 809 810 812 816 822 836 837 838 900 901"'
integration_rlc_wholestrm_cases='"h264 mpeg4 h263" -n "_hp_ _mp_ _high_ _main_ _cabac_ cabac_ asp mpeg4_error"'
integration_mbuffer_cases='"_pp_" -n "jpeg"'

printUsage()
{
    echo "./summarize.sh [options]"
    echo "Options:"
    echo "\"csv\"             Generate CSV report and update the status file, optional"
}

generate_csv_report=0

# get command line arguments, show help if wrong number, otherwise parse options 
if [ $# -gt 1 ]
then
    printUsage
    exit 1
    
elif [ $# -eq 1 ]
then
    if [ $1 == "csv" ]
    then
        generate_csv_report=1
    else
        printUsage
        exit 1   
    fi
fi

echo "Test Case;Category;Pictures;Decoder Picture Width;PP Picture Width;Status;HW Tag;SW Tag;FAILED configuration;" > "summary_report.csv"

# write configuration cases
rm -rf category_cases
for configuration in $ALL_CONFIGURATIONS
do
    eval configuration_cases=\$${configuration}_all_cases
    #echo "configuration_cases: $configuration_cases"
    case_number=`echo "$configuration_cases" | awk -F -n '{print $1}'`
    case_number=`echo $case_number | tr -d \"`
    #echo "case_number: $case_number"
    case_filter=`echo "$configuration_cases" | awk -F -n '{print $2}'`
    case_filter=`echo $case_filter | tr -d \"`
    #echo "case_filter: $case_filter"
    #echo "$configuration categories:"
    getCases "$category_list_swhw" "$case_number" "$case_filter" "0" >> /dev/null 2>&1
    echo "${configuration}=\"${test_cases}\"" >> category_cases
    #echo
done

# get all cases
case_filter=""
#echo "all categories:"
getCases "$category_list_swhw" "$CHECK_FORMATS" "$case_filter" "0" >> /dev/null 2>&1
#getCases "$category_list_swhw" "$CHECK_FORMATS" "$case_filter" "0"
all_cases=$test_cases
#for test_case in $all_cases
#do
#    echo "$test_case" >> dadaa.txt
#done
#exit 

# loop through all cases and configurations
let 'ok_cases=0'
let 'failed_cases=0'
let 'not_executed_cases=0'
for test_case in $all_cases
do
    test_status="NOT_EXECUTED"
    previous_test_status="$test_status"
    failed_configurations=""
    printCase "$test_case" "DUMMY" "DUMMY"
    for configuration in $ALL_CONFIGURATIONS
    do
        if [ ! -d $configuration ]
        then
            # test if really not executed, i.e., test is not executed although it should
            configuration_cases=`grep "$configuration" ./category_cases | awk -F = '{print $2}' | tr -d \"`
            for configuration_case in $configuration_cases
            do
                if [ "$configuration_case" == "$test_case" ]
                then
                    let 'not_executed_cases+=1'
                    break
                fi
            done
            
        else
            cd $configuration
            if [ ! -e report.csv ]
            then
                echo "Cannot find report.csv in ${configuration} -> skipping ${configuration}"
                
            else
                test_status=`cat report.csv | grep -w "case ${test_case}" | awk -F \; '{print $8}'`
                if [ "$test_status" == "NOT_FAILED" ] || [ "$test_status" == "OK" ]
                then
                    let 'ok_cases+=1'
                
                elif  [ "$test_status" == "FAILED" ] || [ "$test_status" == "NOT_OK" ]
                then
                    let 'failed_cases+=1'
            
                # test if really not executed, i.e., test is not executed although it should
                elif [ "$test_status" == "NOT_EXECUTED" ]
                then
                    #echo $configuration > ../jep.jep
                    configuration_cases=`grep "$configuration" ../category_cases | awk -F = '{print $2}' | tr -d \"`
                    #echo $configuration_cases >> ../jep.jep
                    #exit
                    for configuration_case in $configuration_cases
                    do
                        if [ "$configuration_case" == "$test_case" ]
                        then
                            let 'not_executed_cases+=1'
                            break
                        fi
                    done
                fi
                #echo "#$test_status#"
                if [ ! -z "$test_status" ]
                then
                    # NOT_EXECUTED->NOT_FAILED->OK->NOT_OK->FAILED
                    if [ "$test_status" == "NOT_EXECUTED" ] && [[ "$previous_test_status" != "NOT_FAILED" && "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
                    then
                        previous_test_status=$test_status
                        #let 'not_executed_cases+=1'
                    fi
                    if [ "$test_status" == "NOT_FAILED" ] && [[ "$previous_test_status" != "OK" && "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
                    then
                        previous_test_status=$test_status
                        #let 'ok_cases+=1'
                    fi
                    if [ "$test_status" == "OK" ] && [[ "$previous_test_status" != "NOT_OK" && "$previous_test_status" != "FAILED" ]]
                    then
                        previous_test_status=$test_status
                        #let 'ok_cases+=1'
                    fi
                    if [ "$test_status" == "NOT_OK" ] && [[ "$previous_test_status" != "FAILED" ]]
                    then
                        previous_test_status=$test_status
                        failed_configurations="${failed_configurations} ${configuration}"
                        #let 'failed_cases+=1'
                    fi
                    if [ "$test_status" == "FAILED" ]
                    then
                        previous_test_status=$test_status
                        failed_configurations="${failed_configurations} ${configuration}"
                        #let 'failed_cases+=1'
                    fi
                fi
            fi
            cd - >> /dev/null
        fi
    done
    getCategory "$test_case"
    if [ $? != 0 ]
    then
        category="UNKNOWN"
    fi
    getPpCaseWidth "$test_case"
    if [ $pp_case_width == 0 ]
    then
        pp_case_width="N/A"
    fi
    getDecCaseWidthWithAwk "$test_case"
    if [ $dec_case_width == 0 ]
    then
        dec_case_width="N/A"
    fi
    
    echo "$previous_test_status"
    echo "case $test_case;$category;$TB_MAX_NUM_PICS;$dec_case_width;$pp_case_width;$previous_test_status;$HWCONFIGTAG;$SWTAG;$failed_configurations;" >> "summary_report.csv"
done
    
# if csv reporting is enabled, copy the report to destination file
if [ "$generate_csv_report" == 1 ]
then
    cp summary_report.csv "${CSV_FILE}"
    if [ ! -e "$STATUS_FILE" ]
    then
        echo "HW Tag;SW Tag;Reporter;Date;OK cases;FAILED cases;NOT_EXECUTED cases;TOTAL cases" > "$STATUS_FILE"
    fi
    let 'total_cases=ok_cases+failed_cases+not_executed_cases'
    echo "${HWCONFIGTAG};${SWTAG};${USER};${REPORTIME};${ok_cases};${failed_cases};${not_executed_cases};${total_cases}" >> "$STATUS_FILE"
fi
exit 0
