import sys

import os

#list product support sensors

#choice sensor
sn = []
sn_path = ''
sn_items = []
selection = "output/station/selection_config.txt"

def remove_sensor_items():
    #first copy items.itm to items.itm.tmp
    tg_itm = os.getcwd() + '/output/product/' + 'items.itm'
    tmp_itm = os.getcwd() + '/output/station/' + 'items.itm.tmp'
    os.system("cp -f " + tg_itm + ' ' +  tmp_itm)
    #print "tg_itm = " + tg_itm
    #print "file = " + file
    os.system("sed -i /\#ddk.items.start/,/#ddk.items.end/d" + ' ' + tmp_itm)

def remove_exist_config():
    tmp = os.getcwd() + '/output/' + '/system/root/'
    target = tmp + '.ispddk'
    for (dir, subdir, file_name) in os.walk(target):
        for file_elem in file_name:
            #print "in here::::" + file_elem
            #name_list = file_elem.split('-')
            os.system("rm -f " + target + '/' + file_elem)
            #print name_list[0]

def do_copy_sensor_items(file):
    fp = open(file, 'r')
    for line in fp.readlines():
        sn_items.append(line)
def add_to_main_item():
    tg_itm = os.getcwd() + '/output/station/' + 'items.itm.tmp'
    tmp = []
    end = ''
    os.system("sed -i '/items.end/,$d'"+ ' ' + tg_itm)
    fp = open(tg_itm, 'r')
    tmp = fp.readlines()
    #print tmp
    fp.close()
    fp = open(tg_itm, 'w')

    sn_items.insert(0, "#ddk.items.start\n")
    sn_items.append("#ddk.items.end\n")
    sn_items.append("items.end")
    tmp.extend(sn_items)
    fp.writelines(tmp)
    fp.close()

def check_items_exist(path):
    i = 0
    file_list = []
    for(dir, subdir, file_name) in os.walk(path):
        for file_elem in file_name:
            file_list.append(file_elem)
    for el in file_list:
        if (os.path.splitext(el)[1] != ".itm"):
            i=i+1
        else:
            break
    if(i == len(file_list)):
        print "no items file ,please add, exit!"
        return 0
    else:
        #print "there is a item file"
        return 1

def do_copy_configure(line):
    tmp = os.getcwd() + "/output/" + '/system/root/'
    target = tmp + '.ispddk'
    #print "target = " + target

    if (not os.path.exists(target)):
        os.system("mkdir " + target)
    #else:

    ln_list = line.split(' ')
    src = ln_list[0] + '/' + ln_list[1]
    #print "src = " +src
    if (0 == check_items_exist(src)):
        exit(1)

    for (dir, subdir, file_name) in os.walk(src):
        for file_elem in file_name:
            if (os.path.splitext(file_elem)[1] != ".itm"):
               #print file_elem
               os.system("cp -f "  + src + '/' + file_elem + ' ' +  target )
            elif (os.path.splitext(file_elem)[1] == ".itm"):
                do_copy_sensor_items(src + '/' + file_elem)

def do_copy_json(line):
    tmp = os.getcwd() + '/output/' + '/system/root/'
    target = tmp + '.videobox'
    #print "target = " + target

    if (not os.path.exists(target)):
        os.system("mkdir " + target)
    else:
        os.system("rm -f "  + target +  '/*.json')
    #frist used the select json to change default json
    ln_list = line.split(' ')
    src = ln_list[0] + '/' + ln_list[1]
    #print "src = " +src
    os.system("cp -f "  + src +  ' ' +  target + '/' + 'path.json' )
    #second copy all json to target directory
    src = ln_list[0] + '/'
    ldir = os.listdir(src)
    for elem in ldir:
        if (elem != "path.json"):
            os.system("cp -f "  + src + elem  + ' ' +  target)

if __name__ == "__main__":   

    fp = open(selection, 'r')
    f = fp.readlines()
    #first remove exist sensor config and sensor items
    remove_exist_config()
    remove_sensor_items()
    for line in f:
        #print "++ line : " + line
        if (line.find('json') != -1 ):
            do_copy_json(line)
        else:
            do_copy_configure(line)

    add_to_main_item()

    fp.close()

