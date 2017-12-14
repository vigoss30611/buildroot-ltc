#!/usr/bin/python

#
# Create task list for H1 encoder testing
#

import sys
import os
import random
import re

################################################################################
# global variables
sizeorder=False
randomorder=False
failed=False
failedlist=""
limits = []
cases = []
casegroups = []
casembs = {}    # Dict of case -> number of MBs in case

# case groups, order for 'all'
groups = ['vp8', 'h264', 'jpeg', 'stab', 'random-vp8', 'random-h264', \
          'random-jpeg']
# case numbers for each of groups
groupcases = {}
groupcases['h264'] = [1000, 1999]
groupcases['jpeg'] = [2000, 2999]
groupcases['vp8']  = [3000, 3999]
groupcases['stab'] = [4000, 4999]
groupcases['random-vp8']  = [5000, 5999]
groupcases['random-h264'] = [6000, 6999]
groupcases['random-jpeg'] = [7000, 7999]

################################################################################
# Usage
def Usage():
    print \
"""
Create a list of test cases (== task list) based on task/run_xxxx.sh scripts.

Usage: create_tasklist.py task/list [cases] [size-order | random-order]
                                    [param<value] [param=value] [param>value]

    task/list       - task list file to be created

    cases is a list of any of the following:
        casenum     - number of test case
        first-last  - a range of test cases
        all         - all cases
        vp8         - all vp8 cases
        h264        - all H.264 cases
        jpeg        - all JPEG cases
        stab        - all video stabilization cases
        random-vp8  - all random parameter vp8 cases
        random-h264 - all random parameter h264 cases
        random-jpeg - all random parameter jpeg cases
        failed      - all cases failed on previous testing

    size-order      - order cases by size
    random-order    - order cases by random

    param=value     - only cases with parameter equal to given value
    param>value     - only cases with parameter greater than given value
    param<value     - only cases with parameter less than given value
                        look at task/run_xxxx.sh scripts for possible parameters

    hwconfig        - if the file is found it is used to select valid cases

"""

## TODO
## filter out cases with internal test flag
## --maxWidth=n
## --frames=n

################################################################################
# Parse parameters
def Parse(params):
    global sizeorder, randomorder, failed, failedlist, limits

    for par in params:
        if par == "all":
            for group in groups:
                casegroups.append(group)
        elif par == "size-order":
            sizeorder = True
        elif par == "random-order":
            randomorder = True
        elif "failed" in par:
            failed = True
            failedlist = par
        elif par in groups:
            casegroups.append(par)
        elif '-' in par:
            m=re.match("(?P<first>[0-9]+)-(?P<last>[0-9]+)", par)
            if not m is None:
                for i in range(int(m.group('first')), int(m.group('last'))+1):
                    cases.append(int(i))
        elif '<=' in par:
            limits.append((par.split('<')[0], '<=', par.split('=')[1]))
        elif '>=' in par:
            limits.append((par.split('>')[0], '>=', par.split('=')[1]))
        elif '=' in par:
            limits.append((par.split('=')[0], '=', par.split('=')[1]))
        else:
            cases.append(int(par))


################################################################################
# Read HW config file and add limits accordingly
def HwConfigLimits():
    global limits, casegroups

    if not os.access('hwconfig', os.R_OK):
        return

    fconfig=open('hwconfig', 'r')
    print "Reading hwconfig for case limits"
    for line in fconfig:
        par = line.split()[0].strip()
        val = line.split()[1].strip()
        if par == 'maxEncodedWidth':
            limits.append(('width', '<=', str(val)))
        elif par == 'vsEnabled' and val == '0':
            limits.append(('videoStab', '=', '0'))
        elif par == 'rgbEnabled' and val == '0':
            limits.append(('inputFormat', '<=', '4'))
        elif par == 'h264Enabled' and val == '0':
            if 'h264' in casegroups:        casegroups.remove('h264')
            if 'random-h264' in casegroups: casegroups.remove('random-h264')
        elif par == 'vp8Enabled' and val == '0':
            if 'vp8' in casegroups:         casegroups.remove('vp8')
            if 'random-vp8' in casegroups:  casegroups.remove('random-vp8')
        elif par == 'jpegEnabled' and val == '0':
            if 'jpeg' in casegroups:        casegroups.remove('jpeg')
            if 'random-jpeg' in casegroups: casegroups.remove('random-jpeg')
    fconfig.close()


################################################################################
# GetResolutionFromFilename reads the case parameters from run script.
# Returns either the width or height
def GetResolutionFromFilename(filename, height):
    filename = filename.lower()
    if '/' in filename:
        filename = filename.rsplit('/',1)[1]

    if 'subqcif' in filename: return (128,96)[height]
    if 'sqcif'   in filename: return (128,96)[height]
    if 'qcif'    in filename: return (176,144)[height]
    if '4cif'    in filename: return (704,576)[height]
    if 'cif'     in filename: return (352,288)[height]
    if 'qqvga'   in filename: return (160,120)[height]
    if 'qvga'    in filename: return (320,240)[height]
    if 'vga'     in filename: return (640,480)[height]
    if '720p'    in filename: return (1280,720)[height]
    if '1080p'   in filename: return (1920,1080)[height]

    m=re.search("w(?P<width>[0-9]+)h(?P<height>[0-9]+)", filename)
    if not m is None:
        return (m.group('width'),m.group('height'))[height]

    m=re.search("(?P<width>[0-9]+)x(?P<height>[0-9]+)", filename)
    if not m is None:
        return (m.group('width'),m.group('height'))[height]

    return 0


################################################################################
# GetCaseParams reads the case parameters from run script.
# Returns the number of mbs in the case, 0 for invalid case.
def GetCaseParams(casenum, limits):
    ## TODO create task/run_casenum.sh
    # This is now done with testcaselist.sh
    # Read case definition from external parameter file (TBD database)
    if not os.access('task/run_'+str(casenum)+'.sh', os.R_OK):
        return 0

    # Read case parameters from run.sh into dict
    casepar={}
    fin=open('task/run_'+str(casenum)+'.sh', 'r')
    params = fin.readline()
    for par in params.split():
        if '=' in par:
            casepar[par.split('=')[0].lstrip('-')] = par.split('=')[1]
    fin.close()

    # Check that parameters are valid
    if 'width' not in casepar or 'height' not in casepar:
        print "Error in width/height parameters for case: ", casenum
        return 0

    if 'input' not in casepar:
        print "Error in input parameter for case: ", casenum
        return 0

    # If unspecified width or height parse from filename
    if casepar['width'] == '-255' or casepar['height'] == '-255':
        casepar['width'] = GetResolutionFromFilename(casepar['input'], 0)
        casepar['height'] = GetResolutionFromFilename(casepar['input'], 1)

    # Check parameter limits
    for limit in limits:
        if limit[0] not in casepar:
            print "Warning for parameter limit: "+limit[0]+" not in case", casenum
            continue

        # Limit format: param, operand '<=' '=' '>=', value
        if limit[1] == '<=':
            if int(casepar[limit[0]]) > int(limit[2]): return 0
        if limit[1] == '=':
            if int(casepar[limit[0]]) != int(limit[2]): return 0
        if limit[1] == '>=':
            if int(casepar[limit[0]]) < int(limit[2]): return 0

    # Return mbs for case
    mbs = int(casepar['width'])/16*int(casepar['height'])/16
    mbs *= (int(casepar['lastPic'])-int(casepar['firstPic'])+1)
#    print casenum, casepar['width'], casepar['height'], mbs
    return mbs


################################################################################
# GetMbs
def GetMbs(i):
    return int(casembs[i])


################################################################################
# Main program
def main():

    if len(sys.argv) < 3:
        Usage()
        sys.exit(1)

    tasklist=sys.argv[1]

    print 'Creating tasklist <'+tasklist+'>'

    # Store command in history
    fcmd=open("create_tasklist_history", 'a')
    for par in sys.argv:
        fcmd.write("%s " % par)
    fcmd.write("\n")
    fcmd.close()

    Parse(sys.argv[2:])

    if failed:
        print "Adding previously failed cases from list '"+failedlist+"'"
        fin=open(failedlist, 'r')
        for case in fin:
            cases.append(case)
        fin.close()

    HwConfigLimits()

    print "Limits: ", limits

    # Check that cases are valid
    for i in cases[:]:
        mbs = GetCaseParams(i, limits)
        if mbs == 0:
            cases.remove(i)
        else:
            casembs[i] = mbs

    print "Groups: ", casegroups

    # Expand casegroups into list of cases
    for group in casegroups:
        for i in range(groupcases[group][0], groupcases[group][1]):
            # Read case definitions
            mbs = GetCaseParams(i, limits)
            # Check if i is a valid case
            if mbs:
                cases.append(i);
                # Create dict of case number -> total mbs
                casembs[i] = mbs

    orderedcases = []
    if sizeorder:
        print "Ordering cases by size..."
        for case in sorted(casembs.keys(), key=GetMbs):
            orderedcases.append(case)
    elif randomorder:
        print "Ordering cases by random..."
        numcases = len(cases)
        for unused_i in range(numcases):
            case = random.choice(cases)
            orderedcases.append(case)
            cases.remove(case)
    else:
        orderedcases = cases

    print "Total of "+str(len(orderedcases))+" cases: ", orderedcases

    # Write task list
    if not os.access("task",os.R_OK): os.mkdir("task")

    fout=open(tasklist,'w')
    for case in orderedcases:
        fout.write("%s\n" % str(case))

    fout.close()


################################################################################
if __name__ == '__main__':
  main()

