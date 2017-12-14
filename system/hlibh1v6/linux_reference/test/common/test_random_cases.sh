#!/bin/bash

#imports
#.   ./f_run.sh

# Output file when running all cases
resultfile=results_random

rm -f random_log_done random.log

# Create random_run signaling that these cases are stored in random_cases
touch random_run
mkdir random_cases
chmod 777 random_cases


randomizeCases()
{
    for ((i=1000; i<=4000; i++))
    do
        let 'j=i-1000' 
        cases[$j]=$i
    done

    let 'number_of_cases=3000'
    let 'i=0'
    
    while [ $number_of_cases != 0 ]
    do

	random_value=$RANDOM; let "random_value %= number_of_cases"

        random_cases_arr[$i]=${cases[$random_value]}

        echo ${cases[$random_value]} >> random.log 

	unset cases[$random_value]
	cases=(${cases[*]})
		
	let 'number_of_cases=number_of_cases-1'
	let 'i=i+1'
    done

    export random_cases_arr
}

echo -n "Creating random case order in random.log... "

randomizeCases

echo "Done"
touch random_log_done

echo "Running test cases in random order"
echo "This will take several minutes"
echo "Output is stored in $resultfile.log"
rm -f $resultfile.log


for ((i=0; i<=2999; i++))
do
    echo -en "\rCase ${random_cases_arr[$i]}\t"
    
    if [ 1000 -le ${random_cases_arr[$i]} ] && [ ${random_cases_arr[$i]} -lt 1990 ]
    then
        ./test_h264.sh ${random_cases_arr[$i]} >> $resultfile.log
    
        if [ 1750 -le ${random_cases_arr[$i]} ] && [ ${random_cases_arr[$i]} -lt 1800 ]
        then
            ./test_vs.sh ${random_cases_arr[$i]} >> $resultfile.log
        fi
    
    elif [ 2000 -le ${random_cases_arr[$i]} ] && [ ${random_cases_arr[$i]} -lt 3000 ]
    then
        ./test_jpeg.sh ${random_cases_arr[$i]} >> $resultfile.log
    elif [ 3000 -le ${random_cases_arr[$i]} ] && [ ${random_cases_arr[$i]} -lt 4000 ]
    then
        ./test_vp8.sh ${random_cases_arr[$i]} >> $resultfile.log
    fi
done

