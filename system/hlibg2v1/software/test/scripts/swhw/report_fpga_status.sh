#!/bin/bash

if [ "$1" == "" ]
then
  echo
  echo "This script generates summary from FPGA test report."
  echo
  echo "Usage: ./report_fpga_status.sh <input file> optional:<output file>"
  echo
  echo "Default output file is '$outputFile'"
  echo
  exit
fi

inputFile=$1
if [ "$2" == "" ];then
  outputFile="fpga_report.txt"
else
  outputFile=$2
fi

# To generate testcase_list
../../../../common/scripts/test_data/database/testcase_list.pl g2
. ./testcase_list_g2
rm -f report.tmp report_*_details.tmp

export cases_in_suite=0
hevc_subcats="intra inter allegro conformance compatibility random"
vp9_subcats="intra inter conformance compatibility random"

for subcat in $hevc_subcats; do
  let total_hevc_${subcat}_failed=0
  let total_hevc_${subcat}_passed=0
  let total_hevc_${subcat}_cases=0
done
for subcat in $vp9_subcats; do
  let total_vp9_${subcat}_failed=0
  let total_vp9_${subcat}_passed=0
  let total_vp9_${subcat}_cases=0
done


GetCategoryTotalCount()
{
  # Find if given test case belongs to a category
  for category in $category_list
  do
    # list cases in category
    eval cases_in_category=\$$category
    no_cases_in_category=`echo $cases_in_category | tr " " "\n" | wc | awk '{print $1}'`
    let cases_in_suite=$cases_in_suite+$no_cases_in_category
  done
}

CheckCategory()
{
  # Find if given test case belongs to a category
  for category in $category_list
  do
    # list cases in category
    eval cases_in_category=\$$category

    # find given case in category
    if eval "echo $cases_in_category | grep -w '$1'>/dev/null"
    then
      echo $category
      break
    fi
  done
}

testCases=`grep -v "Test case" $inputFile | awk 'BEGIN {FS=";"}{print $1}' | uniq`
testCasesOK=""
testCasesFAIL=""

for testcase in $testCases
do
  if [ `grep $testcase $inputFile | grep -c "FAIL\|ERROR"` -eq 0 ];then
    testCasesOK="$testcase $testCasesOK"
  else
    testCasesFAIL="$testcase $testCasesFAIL"
  fi
done

# Check passes
for testcase in $testCasesOK
do
  category=`grep $testcase $inputFile | awk 'BEGIN {FS=";"}{print $2}' | uniq`
  if [ "$category" == "" ]; then
    category=`CheckCategory $testcase`
  fi
  printf "%-6s %-39s %-5s\n" $testcase $category "OK" \
        | tee -a report_ok_details.tmp
  export ${category}_ok
  let ${category}_ok++

  if eval "echo $category | grep hevc > /dev/null"; then
    let total_hevc_passed++
    let total_hevc_cases++
  fi
  if eval "echo $category | grep vp9 > /dev/null"; then
    let total_vp9_passed++
    let total_vp9_cases++
  fi
  for subcat in $hevc_subcats; do
    if eval "echo $category | grep hevc_$subcat > /dev/null"; then
      let total_hevc_${subcat}_passed++
      let total_hevc_${subcat}_cases++
    fi
  done
  for subcat in $vp9_subcats; do
    if eval "echo $category | grep vp9_$subcat > /dev/null"; then
      let total_vp9_${subcat}_passed++
      let total_vp9_${subcat}_cases++
    fi
  done  
  let total_cases++
  let total_passed++
done

# Check failures
for testcase in $testCasesFAIL
do
  category=`grep $testcase $inputFile | awk 'BEGIN {FS=";"}{print $2}' | uniq`
  if [ "$category" == "" ]; then
    category=`CheckCategory $testcase`
  fi
  # Skip OK cases and use first failed/error as fail_reason
  fail_reason=`grep $testcase $inputFile | grep -v "OK" | awk 'BEGIN {FS=";"}{print $3}' | head -n1`
  if [ "$fail_reason" == "" ]; then
    fail_reason=" INCONCLUSIVE (simulation not started)"
    # check if the case has been run later and has passed -> don't list as failure anymore.
  fi
  printf "%-6s %-38s %s \n" $testcase $category "$fail_reason" | tee -a report_fail_details.tmp
  export ${category}_failed
  let ${category}_failed++
  if eval "echo $category | grep hevc > /dev/null"; then
    let total_hevc_failed++
    let total_hevc_cases++
  fi
  if eval "echo $category | grep vp9 > /dev/null"; then
    let total_vp9_failed++
    let total_vp9_cases++
  fi
  for subcat in $hevc_subcats; do
    if eval "echo $category | grep hevc_$subcat > /dev/null"; then
      let total_hevc_${subcat}_failed++
      let total_hevc_${subcat}_cases++
    fi
  done
  for subcat in $vp9_subcats; do
    if eval "echo $category | grep vp9_$subcat > /dev/null"; then
      let total_vp9_${subcat}_failed++
      let total_vp9_${subcat}_cases++
    fi
  done  

  let total_cases++
  let total_failed++
done

# Generate report.
for category in $category_list
do
  eval current_ok=\$${category}_ok
  eval current_failed=\$${category}_failed

  # This category contains at least one executed case
  if [ "$current_ok" != "" -o "$current_failed" != "" ];then
    eval cases_in_category=\$$category
    cases_in_category=`echo $cases_in_category | tr " " "\n" | wc | awk '{print $1}'`

    if [ "$current_ok" == "" ];then
      current_ok=0
    fi
    if [ "$current_failed" == "" ];then
      current_failed=0
    fi

    let total=$current_ok+$current_failed
    let pass_rate=$current_ok*100/$total

    printf "%-46s %-8s %s \n" $category ${pass_rate}% "($total / $cases_in_category cases run)" >> report.tmp
  fi
done 
echo

# Get total test suite size.
GetCategoryTotalCount

# Write out report.

let total_vp9_intra_pass_rate=$total_vp9_intra_passed*100/$total_vp9_intra_cases                      2> /dev/null
let total_vp9_inter_pass_rate=$total_vp9_inter_passed*100/$total_vp9_inter_cases                      2> /dev/null
let total_vp9_conformance_pass_rate=$total_vp9_conformance_passed*100/$total_vp9_conformance_cases    2> /dev/null
let total_vp9_compatibility_pass_rate=$total_vp9_compatibility_passed*100/$total_vp9_compatibility_cases    2> /dev/null
let total_vp9_pass_rate=$total_vp9_passed*100/$total_vp9_cases                                        2> /dev/null
let total_vp9_random_pass_rate=$total_vp9_random_passed*100/$total_vp9_random_cases             2> /dev/null
let total_hevc_intra_pass_rate=$total_hevc_intra_passed*100/$total_hevc_intra_cases                   2> /dev/null
let total_hevc_inter_pass_rate=$total_hevc_inter_passed*100/$total_hevc_inter_cases                   2> /dev/null
let total_hevc_conformance_pass_rate=$total_hevc_conformance_passed*100/$total_hevc_conformance_cases 2> /dev/null
let total_hevc_compatibility_pass_rate=$total_hevc_compatibility_passed*100/$total_hevc_compatibility_cases    2> /dev/null
let total_hevc_allegro_pass_rate=$total_hevc_allegro_passed*100/$total_hevc_allegro_cases             2> /dev/null
let total_hevc_random_pass_rate=$total_hevc_random_passed*100/$total_hevc_random_cases             2> /dev/null
let total_hevc_pass_rate=$total_hevc_passed*100/$total_hevc_cases                                     2> /dev/null
let total_pass_rate=$total_passed*100/$total_cases                                                    2> /dev/null

reportdate=`date`
echo                                                                             | tee $outputFile
echo "G2 FPGA testing report. $total_cases / $cases_in_suite tests executed, $total_passed passed, $total_failed failed. " | tee -a $outputFile
echo "hevc_intra         : $total_hevc_intra_pass_rate% of executed tests passed.         " | tee -a $outputFile
echo "hevc_inter         : $total_hevc_inter_pass_rate% of executed tests passed.         " | tee -a $outputFile
echo "hevc_conformance   : $total_hevc_conformance_pass_rate% of executed tests passed.   " | tee -a $outputFile
echo "hevc_compatibility : $total_hevc_compatibility_pass_rate% of executed tests passed.           " | tee -a $outputFile
echo "hevc_allegro       : $total_hevc_allegro_pass_rate% of executed tests passed.       " | tee -a $outputFile
echo "hevc_random        : $total_hevc_random_pass_rate% of executed tests passed.       " | tee -a $outputFile
echo "vp9_intra          : $total_vp9_intra_pass_rate% of executed tests passed.          " | tee -a $outputFile
echo "vp9_inter          : $total_vp9_inter_pass_rate% of executed tests passed.          " | tee -a $outputFile
echo "vp9_conformance    : $total_vp9_conformance_pass_rate% of executed tests passed.    " | tee -a $outputFile
echo "vp9_compatibility  : $total_vp9_compatibility_pass_rate% of executed tests passed.            " | tee -a $outputFile
echo "vp9_random         : $total_vp9_random_pass_rate% of executed tests passed.       " | tee -a $outputFile
echo "                   HEVC TOTAL   : $total_hevc_pass_rate% of executed tests passed.  " | tee -a $outputFile
echo "                   VP9 TOTAL    : $total_vp9_pass_rate% of executed tests passed.   " | tee -a $outputFile
echo "                   TOTAL SCORE  : $total_pass_rate% of executed tests passed.       " | tee -a $outputFile
echo "-------------------------------------------------------------------------" | tee -a $outputFile
echo " Report generated by $USER@$HOSTNAME on $reportdate"                       | tee -a $outputFile
echo
echo $PWD/$outputFile created.
echo

rm -f report.tmp report_*_details.tmp
