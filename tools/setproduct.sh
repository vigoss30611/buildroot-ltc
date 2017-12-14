#!/bin/bash

curd=`pwd`
product_num=10000
count=0
sensor_num1=-1
sensor_num2=-1
path_json_num=-1

for arg in $@
do
        case $arg in
             -p*)
              	product_num=`echo $arg|cut -c 3-`
		;;

             -s0*)
                sensor_num1=`echo $arg|cut -c 4-`
		            if [ -z "${sensor_num1}" ]; then sensor_num1=-1;fi
		;;
		
		         -s1*)
                sensor_num2=`echo $arg|cut -c 4-`
		            if [ -z "${sensor_num2}" ]; then sensor_num1=-1;fi
		;;

             -j*)
                path_json_num=`echo $arg|cut -c 3-`
		;;

             ?) 
            	echo "unkonw argument"
        	exit 1

        esac
done
#echo "product:$product_num json:$path_json_num,sensor_num1:$sensor_num1,sensor_num2:$sensor_num2"
products=(`ls -F products/ | grep "/$" | awk -F/ '{print $1}'`)

if [ "$product_num" -ge 0 ] 2>/dev/null;
then
	prod=${products[$((product_num))]}
else
	prod=$product_num
fi

if [ -z $prod ]; then
	echo -e "\n# please choose a product from list below:\n"
	for i in ${products[@]}; do
		if [ -d "products/$i" ]; then
			#echo "$(($count))  $i"
			printf "%-4d%s\n" ${count} "$i"
			let count++
		fi
	done
	printf "\n# your choice: "
	read product_num
	if [ "$product_num" -ge 0 ] 2>/dev/null;
	then
		prod=${products[$((product_num))]}
	else
		prod=$product_num
	fi
fi

if test -d products/$prod; then
	mkdir -p output/images
    mkdir -p output/station
	rm -f output/product
	ln -s $curd/products/$prod output/product
	make qsdk_defconfig
	echo -e "\n# product successfully set to $prod"
else
	echo $prod is not a valid product
	exit
fi

python ./tools/select_sensor_config.py	/products/$prod/ $sensor_num1 $sensor_num2 $path_json_num
