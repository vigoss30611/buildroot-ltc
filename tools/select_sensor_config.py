import sys

import os

#list product support sensors

#choice sensor
sn = []
sn_path = ''
sn_items = []
selection = "/output/station/selection_config.txt"

def save_selection_to_file(fp, path, index):
    #print fp
    #print "your select: " + sn[index]
    #print "====" + path
    fp.write(path + ' ' + sn[index] + ' \n')

def list_json_select(fp, path, itm):
    i = 0
    #print "\n# " + itm + "\n"
    if (os.path.isdir(path + '/' + itm)):
        ldir = os.listdir(path + '/' + itm)
        for elem in ldir:
            sn.append(elem)
            print "%d   %s" %(i, elem)
            i +=1
        print "%c   %s" %('x', "default")
        #wait ui input the number and copy the configure file to fixed directory.
        if(sys.argv[4] != "-1"):
            input = sys.argv[4]
            print "\n# your choise:%s\n" % input
        else:
            input = raw_input("\n# your choice: ")
            
        if (input == 'x'):
            del sn[0:]
            i = 0
        elif (input.isdigit()):
            save_selection_to_file(fp, path + '/' + itm, int(input))
        elif (sn.index(input)):
            save_selection_to_file(fp, path + '/' + itm, sn.index(input))		

        del sn[0:]

def list_menu(fp, path):
    i = 0
    sn_list = os.listdir(path)
    sn_list.reverse()
    if (0 == len(sn_list)):
        print "Error, the product directory have no configure file, please add!"
        return
    for itm in sn_list:
        if (itm[0:-1] == "sensor"):
            print "\n# please choose %s configuration:\n" % itm
            #print '\n# ' + itm + '\n'
            if (os.path.isdir(path + '/' + itm)):
                ldir = os.listdir(path + '/' + itm)
                for elem in ldir:
                    sn.append(elem)
                    print "%d   %s" %(i, elem)
                    i +=1
                print "%c   %s" %('x', "none")
                #wait ui input the number and copy the configure file to fixed directory.
               	
            if((itm == "sensor0") and (sys.argv[2] != "-1")):
                input = sys.argv[2]
                print "\n# your choise:%s\n" % input
            elif((itm == "sensor1") and (sys.argv[3] != "-1")):
                input = sys.argv[3]
                print "\n# your choise:%s\n" % input
            else:
                input = raw_input("\n# your choice: ")
                if (input == 'x'):
                    del sn[0:]
                    i = 0
                    #remove_exist_config(itm)
                    #print "# choice none %s"%(itm)
                    continue
                elif (input.isdigit()):
                    save_selection_to_file(fp, path + '/' + itm, int(input))
                elif (sn.index(input)):
                    save_selection_to_file(fp, path + '/' + itm, sn.index(input))

            del sn[0:]
            i = 0
        elif (itm == "json"):
            print "\n# please choose product json configration:\n"
            list_json_select(fp, path, itm)

if __name__ == "__main__":   
    sn_path = os.getcwd() + sys.argv[1] + "isp"
    #print sn_path
    fp = open(os.getcwd() + selection, 'w')
    list_menu(fp, sn_path)
    fp.close()
    #remove_sensor_items()
    #add_to_main_item()
    prod_list=(sys.argv[1]).split('/')
    #print prod_list
    print "\n# choose configuration successfully to " + prod_list[-2]

