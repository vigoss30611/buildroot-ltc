#!/bin/bash
update_items() {
	newer=`find output/product/items.itm -newer output/station/items.itm.tmp`
	tmp=output/station/items.itm.tmp.1
	if [ "$newer" = "output/product/items.itm" ];then
		cp output/product/items.itm $tmp
		sed -i '$d' $tmp
		sed -n '/#ddk.items.start/,/#ddk.items.end/p' output/station/items.itm.tmp >> $tmp
		echo "items.end" >> $tmp
		mv $tmp output/images/items.itm
	else
		cp output/station/items.itm.tmp output/images/items.itm
	fi
}
echo "update items file"
update_items
echo "update items over"
