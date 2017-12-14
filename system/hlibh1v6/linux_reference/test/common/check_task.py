#!/usr/bin/python

#
# Check test cases in a task list
#

import sys
import os
import fnmatch
import filecmp
import difflib
import datetime
import shutil
import re
import subprocess

import create_tasklist

################################################################################
# global variables
cases = []
comparecomment=""
errbyte=0
casenum=0
recheck=0
deleteok=0
tasklist=""
failedlist="task/failed"
caseok=0
casefailed=0
caseerror=0
casesfailed=0
usedb=0
timestamp=datetime.datetime.today().strftime("%d%m%y_%H%M")

# These checks can be disabled
checkyuv=1
checkscaled=1
checkregs=1
checkmvout=1

# How many failed cases to save in FAILDIR
savefails=50

################################################################################
# Usage
def Usage():
    print \
"""
Usage: check_task.py [task/list | all | casenum | first-last] [--recheck] [--delete] [--db]

        task/list   - tasklist containing case numbers and order
        all         - all tested cases with dirs case_xxxx
        casenum     - number of test case to check
        first-last  - a range of test cases

        --recheck   - re-check all cases that have been checked earlier
        --delete    - delete test files for OK cases
        --db        - add test results to test data base

"""

################################################################################
# Parse parameters
def Parse(params):
    global recheck, deleteok, tasklist, usedb

    for par in params:
        if os.access(par, os.R_OK):
            if not os.access(par, os.R_OK):
                print "Can't access "+par
                sys.exit(1)
            fin=open(par,'r')
            tasklist=par
            for case in fin:
                cases.append(int(case))
            fin.close()
        elif par == "--recheck":
            recheck=1
        elif par == "--delete":
            deleteok=1
        elif par == "--db":
            usedb=1
        elif par == "all":
            expr=re.compile("case_(?P<case>[0-9]+)_[0-9]+")
            for rounddir in os.listdir("."):
                m=expr.match(rounddir)
                if not m is None: cases.append(int(m.group('case')))
        elif '-' in par:
            m=re.match("(?P<first>[0-9]+)-(?P<last>[0-9]+)", par)
            if not m is None:
                for i in range(int(m.group('first')), int(m.group('last'))):
                    cases.append(i)
        else:
            cases.append(int(par))

################################################################################
# ParseCommonConfig reads the tag from commonconfig.sh script
def ParseCommonConfig(varkey):
    if not os.access("commonconfig.sh", os.R_OK):
        return ""

    fcfg=open("commonconfig.sh","r")
    expr=re.compile(varkey+'=(?P<value>["a-z0-9_ ]+)')
    for line in fcfg:
        m=expr.search(line)
        if not m is None: return m.group('value').strip().strip("\"")

    return ""

################################################################################
# Read HW config file and add limits accordingly
def HwConfigLimits():
    global checkscaled

    if not os.access('hwconfig', os.R_OK):
        return

    fconfig=open('hwconfig', 'r')
    print "Reading hwconfig"
    for line in fconfig:
        par = line.split()[0].strip()
        val = line.split()[1].strip()
        if par == 'scalingEnabled' and val == '0':
            checkscaled=0
    fconfig.close()


################################################################################
# GetCasePar reads the case parameters from run script.
# Returns the parameter value in the case, 0 for invalid case/parameter.
def GetCasePar(casenum, param):
    # Read case parameters from run.sh into dict
    casepar={}
    if not os.access('task/run_'+str(casenum)+'.sh', os.R_OK):
        return 0

    fin=open('task/run_'+str(casenum)+'.sh', 'r')
    params = fin.readline()
    fin.close()
    for par in params.split():
        if '=' in par:
            casepar[par.split('=')[0].lstrip('-')] = par.split('=')[1]

    # Check that parameters are valid
    for par in ('width', 'height', 'input'):
        if par not in casepar:
            print "Error in "+par+" parameter for case "+str(casenum)
            return 0

    # If unspecified width or height parse from filename
    if casepar['width'] == '-255' or casepar['height'] == '-255':
        casepar['width'] = create_tasklist.GetResolutionFromFilename(casepar['input'], 0)
        casepar['height'] = create_tasklist.GetResolutionFromFilename(casepar['input'], 1)

    # Return parameter value for case
    if param not in casepar:
        print "Error getting parameter "+param+" for case: "+str(casenum)
        return 0

    return int(casepar[param])

################################################################################
# RemoveBaseAddress
# Removes non-matching lines from register trace
def RemoveBaseAddress(regtrc, regtrcfiltered, suf):
    if not os.access(regtrc, os.R_OK):
       return

    freg=open(regtrc, 'r')
    fgrep=open(regtrcfiltered, 'w')
    for line in freg:
        # mb= not printed by system model
        if "mb=" in line: continue
        # ASIC ID not present on system model
        if " 00000000/" in line: continue
        # Swap bits don't match
        if " 00000008/" in line: continue
        if " 0000000c/" in line: continue
        # Base addresses
        if " 00000014/" in line: continue
        if " 00000018/" in line: continue
        if " 0000001c/" in line: continue
        if " 00000020/" in line: continue
        if " 00000024/" in line: continue
        if " 00000028/" in line: continue
        if " 0000002c/" in line: continue
        if " 00000030/" in line: continue
        if " 00000034/" in line: continue
        if " 0000009c/" in line: continue
        if " 000000cc/" in line: continue
        if " 000000d0/" in line: continue
        if " 0000011c/" in line: continue
        if " 0000039c/" in line: continue
        # Stream buffer size
        if " 00000060/" in line: continue
        # Config register
        if " 000000fc/" in line: continue
        # VP8 bases
        if suf == "ivf":
            if " 00000040/" in line: continue
            if " 00000044/" in line: continue
            if " 00000068/" in line: continue
            if " 000000e8/" in line: continue
            if " 000000ec/" in line: continue
            if " 000003d0/" in line: continue
            if " 000003d4/" in line: continue
            # RLC word count doesn't match in VP8
            if " 00000094/" in line: continue
            # Stream header remainder doesn't match in VP8
            if " 00000058/" in line: continue
        # H264 ref2 bases
        if suf == "h264":
            if " 00000070/" in line: continue
            if " 00000074/" in line: continue
        # Deadzone registers, only written when enabled
        if " 0000029" in line: continue
        if " 000002a" in line: continue
        if " 000002b" in line: continue
        if " 000002c" in line: continue
        if " 000002d" in line: continue
        if " 000002e" in line: continue
        if " 000002f" in line: continue
        if " 0000030" in line: continue
        if " 0000031" in line: continue
        if " 0000032" in line: continue
        if " 0000033" in line: continue
        if " 0000034" in line: continue
        if " 0000035" in line: continue
        if " 0000036" in line: continue
        if " 0000037" in line: continue
        if " 0000038" in line: continue
        if " 0000039" in line: continue
        # JPEG quant tables can't be read
        # VP8/H264 only registers are not written for JPEG
        if suf == "jpg":
            if " 000001" in line: continue
            if " 000002" in line: continue
            if " 000003" in line: continue
            if " 000004" in line: continue
        # If scaler not supported the regs won't match
        if not checkscaled:
            if " 000003a0" in line: continue
            if " 000003a4" in line: continue

        # Otherwise write the line
        fgrep.write(line)

    freg.close()
    fgrep.close()

################################################################################
# SaveFileToDir
def SaveFileToDir(src, dstdir, dstfile):
    if casesfailed > savefails and not ('run' in src):
        return
    if not os.access(src, os.R_OK):
        return
    if not os.access(dstdir, os.R_OK):
        os.mkdir(dstdir)
    if not os.access(dstdir, os.R_OK):
        print "Failed to create "+dstdir
        return
    shutil.copyfile(src, dstdir+"/"+dstfile)

################################################################################
# BinDiff
def BinDiff(file1, file2):
    global errbyte

    chunk=1000000
    errcnt=0
    errbyte=0
    diffbytes=0
    len1=os.stat(file1).st_size
    len2=os.stat(file2).st_size
    f1=open(file1, 'r')
    f2=open(file2, 'r')
    bytes=max(len1, len2)
    comm="diff "
    for i in range(bytes/chunk+1):
        data1=f1.read(chunk)
        data2=f2.read(chunk)
        cmpbytes=min(len(data1), len(data2))
        for j in range(cmpbytes):
            if data1[j] != data2[j]:
                errbyte=i*chunk + j
                comm+="@"+str(errbyte)+":"+str(ord(data1[j]))
                comm+="/"+str(ord(data2[j]))
                errcnt+=1
                if errcnt < 3:
                    comm+=" "
                else:
                    comm+="."
                    return comm
        diffbytes += cmpbytes
        if len(data1) < len(data2):
            errbyte=i*chunk+len(data1)
            return "EOF @"+str(errbyte)+"."
        elif len(data1) > len(data2):
            errbyte=i*chunk+len(data2)
            return "EOF ref @"+str(errbyte)+"."
    return "No diff in "+str(diffbytes)+" bytes."

################################################################################
# TxtDiff
def TxtDiff(file1, file2):
    file1=open(file1, 'r')
    file2=open(file2, 'r')
    expr=re.compile("pic=*")
    linenum=0
    picstr="pic=0"
    for line1 in file1:
        match=expr.match(line1)
        if match != None:
            picstr=match.string.strip()
        line2=file2.readline()
        if line1 != line2:
            first=len(line2)
            for i in range(len(line1)):
                if len(line2) > i and line1[i] != line2[i]:
                    first=i
                    break
            diff = first
            last = first + min(12, len(line1)-first, len(line2)-first)
            first = first - min(12, first)
            return("diff on line:"+str(linenum)+" char:"+str(diff)+", "+picstr+
                   ": "+line1[first:last].strip()+"##"+line2[first:last].strip()+".")
        linenum+=1
    return "No diff in "+str(linenum)+" lines."

################################################################################
# YuvDiff
def YuvDiff(file1, file2, width, height, bpp):
    len1=os.stat(file1).st_size
    len2=os.stat(file2).st_size
    f1=open(file1, 'r')
    f2=open(file2, 'r')
    picsize=width*height*bpp/8
    mbw=(width+15)/16
    comm="w="+str(width)+" h="+str(height)+" "
    if not picsize:
        return "zero size."
    if len1 != len2:
        f1pics=len1/picsize
        f2pics=len2/picsize
        comm+="pics decoded "+str(f1pics)+"/"+str(f2pics)+", "
    bytes=max(len1, len2)
    # Compare YUV's picture by picture
    for i in range(bytes/picsize+1):
        if len1 <= i*picsize:
            return "EOF."
        if len2 <= i*picsize:
            return "EOF ref."
        data1=f1.read(picsize)
        data2=f2.read(picsize)
        for j in range(min(len(data1),len(data2))):
            if data1[j] != data2[j]:
                comm+="diff @"+str(i*picsize+j)+":"+str(ord(data1[j]))
                comm+="/"+str(ord(data2[j]))
                errpic=i
                comm+=" pic="+str(errpic)+" byte="+str(j)
                if bpp == 12:
                    if j < width*height:
                        erry=j/width
                        errx=j-erry*width
                        errmb=erry/16*mbw + errx/16
                        comm+=" luma mb="+str(errmb)
                    elif j < width*height*5/4:
                        erry=(j-width*height)/(width/2)
                        errx=(j-width*height)-erry*width/2
                        errmb=erry/8*mbw + errx/8
                        comm+=" cb mb="+str(errmb)
                    else:
                        erry=(j-width*height*5/4)/(width/2)
                        errx=(j-width*height*5/4)-erry*width/2
                        errmb=erry/8*mbw + errx/8
                        comm+=" cr mb="+str(errmb)

                comm+="."
                return comm

    return comm

################################################################################
# CompareFiles
def CompareFiles(file1, file2, filetype):
    global comparecomment

    file1ok=file2ok=0
    if os.access(file1, os.R_OK): file1ok=1
    if os.access(file2, os.R_OK): file2ok=1
    # File of zero size is treated as no file (error)
    if file1ok and file2ok:
        if os.stat(file1).st_size != os.stat(file2).st_size:
            if os.stat(file1).st_size == 0: file1ok=0
            if os.stat(file2).st_size == 0: file2ok=0

    if not file1ok and not file2ok: comparecomment="No files."
    elif not file1ok:               comparecomment="No file."
    elif not file2ok:               comparecomment="No ref file."
    else:
        if filecmp.cmp(file1, file2):
            comparecomment="OK."
            return 0    # Compare success
        else:
            if filetype == 0:   # Binary files to compare
                comparecomment=BinDiff(file1, file2)
            if filetype == 1:   # Txt files to compare
                comparecomment=TxtDiff(file1, file2)
            if filetype == 2:   # YUV files to compare
                comparecomment=YuvDiff(file1, file2, GetCasePar(casenum, 'width'),
                                        GetCasePar(casenum, 'height'), 12)
            if filetype == 3:   # YUYV files to compare
                comparecomment=YuvDiff(file1, file2, GetCasePar(casenum, 'scaledWidth'),
                                        GetCasePar(casenum, 'scaledHeight'), 16)
            return 1    # Compare fails

    return 2    # Not possible to compare

################################################################################
# ParsePicBytes
def ParsePicBytes(log, suf):
    global comparecomment

    comparecomment=""
    picendbyte=[]
    if not os.access(log, os.R_OK):
        comparecomment=" No log file."
        return picendbyte

    # Log file format for VP8/H264, column for pic number and byte counter
    if suf == "ivf":
        piccol = 1
        typecol = 3
        bytecol = 9
    elif suf == "h264":
        piccol = 1
        typecol = 3
        bytecol = 9
    else:
        return picendbyte

    i=0
    bytes=0
    flog=open(log,"r")
    for line in flog:
        linesplit = line.split()
        if len(linesplit) > bytecol and linesplit[5] == '|':
            pic=linesplit[piccol]
            if pic == str(i):
                bytes=linesplit[bytecol]
                picendbyte.append(int(bytes))
                i+=1

    flog.close()
    return picendbyte

################################################################################
# Decode
def Decode(suf, strm, yuv):
    if suf == "ivf":
        #args = ("./vp8x170dec_pclinux", "-P", "-O"+yuv, strm)
        args = ("./vpxdec", "--i420", "-o", yuv, strm)
    elif suf == "h264":
        #args = ("./hx170dec_pclinux", "-P", "-O"+yuv, strm)
        args = ("./ldecod.exe", "-o", yuv, "-i", strm)
    elif suf == "jpg":
        args = ("./jx170dec_pclinux", "-P", strm)
    else:
        return

    fout=open(yuv+".log",'w')
    if os.access(args[0], os.R_OK):
        popen = subprocess.Popen(args, stdout=fout, stderr=subprocess.STDOUT)
        popen.wait()
    fout.close()
    if suf == "jpg" and os.access("out.yuv", os.W_OK):
        os.rename("out.yuv", yuv)

################################################################################
# WriteResult
def WriteResult(rounddir, freport, result, comment):
    global timestamp, hwtag, systag, swtag, cfgcomment, testdeviceip

    exectime=0
    if os.access(rounddir+"/case.time", os.R_OK):
        ftime=open(rounddir+"/case.time", "r")
        start=end=0
        for line in ftime:
            if line.split(":")[0] == "START":
                start = end = line.split(":")[1]
            if line.split(":")[0] == "END":
                end = line.split(":")[1]
        exectime=int(end)-int(start)

    freport.write(rounddir+";Integration;;;;"+timestamp+";"+str(exectime)+";")
    freport.write(result+";"+hwtag+";"+systag+";"+swtag+";"+os.getenv("USER")+";")
    freport.write(cfgcomment+" "+comment+";;;;;"+testdeviceip+";;\n")

    if usedb:
        subprocess.call(["./f_testdb.sh", rounddir, timestamp, str(exectime), result, cfgcomment+" "+comment])

################################################################################
# CheckResult
def CheckResult(rounddir, flog, freport):
    global casefailed, caseerror, caseok

    if os.access(rounddir+"/result", os.R_OK):
        fresult=open(rounddir+"/result", "r")
        result=fresult.readline().strip()
        fresult.close()
        fcheck=open(rounddir+"/check", "r")
        comment=fcheck.readline().strip()
        fcheck.close()
        WriteResult(rounddir, freport, result, comment)
        flog.write(rounddir+";"+result+";"+comment+"\n")
        flog.flush()
        if result == "FAILED":
            casefailed+=1
        elif result == "ERROR":
            caseerror+=1
        elif result == "OK":
            caseok+=1

################################################################################
# Main program
def main():
    global casenum
    global casefailed, caseerror, caseok
    global casesfailed
    global timestamp, hwtag, systag, swtag, cfgcomment, testdeviceip

    hwtag=ParseCommonConfig('hwtag')
    systag=ParseCommonConfig('systag')
    swtag=ParseCommonConfig('swtag')
    cfgcomment=ParseCommonConfig('comment')
    testdeviceip=ParseCommonConfig('testdeviceip')
    logfile="check_"+hwtag+"_"+timestamp+".log"
    reportfile="integrationreport_"+hwtag+"_"+timestamp+".csv"
    FAILDIR="/export/tmp/h1/"+hwtag+"_"+timestamp+"/"
    debugfile=FAILDIR+"checkdebug.sh"
    firstcase=1
    caseamount=0
    casecount=0
    failedcases=0
    casesok=0
    caseserror=0
    ffailed=0

    if len(sys.argv) < 2:
        Usage()
        sys.exit(1)

    # Parse arguments into list of cases
    Parse(sys.argv[1:])
    HwConfigLimits()

    caseamount=len(cases)
    flog=open(logfile,'w')
    freport=open(reportfile,'w')

    if recheck:
        print "Re-checking",caseamount,"cases, log file",logfile
        flog.write("Re-checking "+str(caseamount)+" cases, log file "+logfile+"\n")
    else:
        print "Checking",caseamount,"cases, log file",logfile
        flog.write("Checking "+str(caseamount)+" cases, log file "+logfile+"\n")

    print "Writing report file "+reportfile 
    freport.write("TestCase;TestPhase;Category;TestedPictures;Language;StatusDate;ExecutionTime;Status;HWTag;EncSystemTag;SWTag;TestPerson;Comments;Performance;KernelVersion;RootfsVersion;CompilerVersion;TestDeviceIP;TestDeviceVersion")

    # Create link check.log
    if os.access("check.log", os.F_OK):
        os.remove("check.log")
    os.symlink(logfile, "check.log")

    print "Copying failed cases to", FAILDIR
    print "For checking: scp -r hlabws5:"+FAILDIR+" /tmp"
    print "For checking: scp -r hlabws5:"+os.getcwd()+"/"+logfile+" /tmp"
    if not os.access(FAILDIR, os.W_OK):
        os.mkdir(FAILDIR)
    fdebug=open(debugfile,'w')

    if not tasklist == failedlist: ffailed=open(failedlist, "w")

    # Get the HW config and write in report
    if os.access("hwconfig", os.R_OK):
	fcfg=open("hwconfig", "r")
	freport.write(fcfg.read())
	fcfg.close()

    for casenum in cases:
        case = str(casenum)
        casecount += 1

        refdir="ref/case_"+case

        caseok=0
        casefailed=0
        caseerror=0
        for rounddir in sorted(os.listdir(".")):
            if fnmatch.fnmatch(rounddir, 'case_'+case+'_*'):
                print "CHK [",casecount,"/",caseamount,"]", casesok, "OK ", casesfailed, "FAILED ",caseserror,"error  ",rounddir,"      \r",
                sys.stdout.flush()

                fail=0
                error=0
                checklog=rounddir+"/check"

                if os.access(checklog, os.R_OK) and os.access(rounddir+"/result", os.R_OK):
                    # Round has been checked before, always recheck error results
                    fresult=open(rounddir+"/result", "r")
                    if fresult.readline().strip() != "ERROR" and not recheck:
                        # Round was OK/FAILED, just write log
                        CheckResult(rounddir, flog, freport)
                        continue

                if not os.access(rounddir, os.R_OK) or not os.access(rounddir+"/status", os.R_OK):
                    # Seems that round is incomplete, don't check
                    continue

                fcheck=open(checklog, "w")
                if not os.access(refdir, os.R_OK):
                    fcheck.write("No reference data!")
                    error=1

                if not error:
                    suf=""
                    if   os.access(refdir+"/stream.ivf",  os.R_OK): suf="ivf"
                    elif os.access(refdir+"/stream.h264", os.R_OK): suf="h264"
                    elif os.access(refdir+"/stream.jpg",  os.R_OK): suf="jpg"

                    if suf == "":
                        fcheck.write("No reference stream! ")
                        error=1

                if not error:
                    # Check run status
                    run_status=open(rounddir+"/status", "r").readline().strip()
                    if run_status == "0":
                        fcheck.write("Run OK.")
                    elif run_status == "3":
                        fcheck.write("Not supported.")
                        error=1
                    elif run_status == "4":
                        fcheck.write("Out of memory.")
                        error=1
                    elif run_status == "6":
                        fcheck.write("Out of memory.")
                        error=1
                    else:
                        fcheck.write("Run FAILED!")
                        fail=1

                if not error:
                    # Check stream
                    teststrm=rounddir+"/stream."+suf
                    refstrm=refdir+"/stream."+suf
                    testlog=rounddir+"/log"

                    cmpfail=CompareFiles(teststrm, refstrm, 0)
                    fcheck.write(" Stream "+comparecomment)
                    if cmpfail == 2:
                        error=1
                    elif cmpfail == 1:
                        fail=1
                        SaveFileToDir(teststrm, FAILDIR, "case_"+case+"."+suf)
                        SaveFileToDir(refstrm, FAILDIR, "case_"+case+"."+suf+".ref")
                        SaveFileToDir(testlog, FAILDIR, "case_"+case+".log")
                        SaveFileToDir(refdir+"/encoder.log", FAILDIR, "case_"+case+".log.ref")

                        # Find pic for error byte position
                        picbytes=ParsePicBytes(testlog, suf)
                        picstart=0
                        for i in range(len(picbytes)):
                            if picbytes[i] > errbyte:
                                fcheck.write(" Pic="+str(i)+" start:"+str(picstart)+" end:"+str(picbytes[i])+".")
                                break
                            picstart=picbytes[i]

                        # Decode streams and compare decoded YUVs
                        if checkyuv:
                            testyuv=rounddir+"/case_"+case+".yuv"
                            refyuv=rounddir+"/case_"+case+".yuv.ref"
                            Decode(suf, teststrm, testyuv)
                            Decode(suf, refstrm, refyuv)

                            cmpfail=CompareFiles(testyuv, refyuv, 2)
                            fcheck.write(" YUV "+comparecomment)
                            if cmpfail == 1:
                                SaveFileToDir(testyuv, FAILDIR, "case_"+case+".yuv")
                                SaveFileToDir(refyuv, FAILDIR, "case_"+case+".yuv.ref")
                                if os.access(testyuv, os.R_OK): os.remove(testyuv)
                                if os.access(refyuv, os.R_OK): os.remove(refyuv)

                    # Check log
                    if os.access(testlog, os.R_OK):
                        fenclog=open(testlog,"r")
                        expr=re.compile("ERROR*")
                        for line in fenclog:
                            match=expr.match(line)
                            if match != None:
                                fcheck.write(" Output sizes "+match.string.strip())
                                fail=1
                        fenclog.close()

                    # Check MV output
                    if checkmvout and os.access(rounddir+"/mv.txt", os.R_OK):
                        cmpfail=CompareFiles(rounddir+"/mv.txt", refdir+"/mv.txt", 1)
                        fcheck.write(" MV "+comparecomment)
                        if cmpfail == 2:
                            error=1
                        elif cmpfail == 1:
                            fail=1
                            SaveFileToDir(rounddir+"/mv.txt", FAILDIR, "case_"+case+"_mv.txt")
                            SaveFileToDir(refdir+"/mv.txt", FAILDIR, "case_"+case+"_mv.txt.ref")

                    # Check SW registers
                    if checkregs and os.access(rounddir+"/sw_reg.trc", os.R_OK):
                        RemoveBaseAddress(rounddir+"/sw_reg.trc", rounddir+"/sw_reg.trc.cmp", suf)
                        RemoveBaseAddress(refdir+"/sw_reg.trc", refdir+"/sw_reg.trc.cmp", suf)

                        cmpfail=CompareFiles(rounddir+"/sw_reg.trc.cmp", refdir+"/sw_reg.trc.cmp", 1)
                        fcheck.write(" Reg "+comparecomment)
                        if cmpfail == 1:
                            fail=1
                            SaveFileToDir(rounddir+"/sw_reg.trc.cmp", FAILDIR, "case_"+case+"_sw_reg.trc")
                            SaveFileToDir(refdir+"/sw_reg.trc.cmp", FAILDIR, "case_"+case+"_sw_reg.trc.ref")

                    # Check scaled yuv
                    if checkscaled and os.access(rounddir+"/scaled.yuyv", os.R_OK):
                        cmpfail=CompareFiles(rounddir+"/scaled.yuyv", refdir+"/scaled.yuyv", 3)
                        fcheck.write(" Scaled "+comparecomment)
                        if cmpfail == 1:
                            fail=1
                            SaveFileToDir(rounddir+"/scaled.yuyv", FAILDIR, "case_"+case+"_scaled.yuyv")
                            SaveFileToDir(refdir+"/scaled.yuyv", FAILDIR, "case_"+case+"_scaled.yuyv.ref")


                # Result for this round, fail is dominant
                fcheck.close()
                fresult=open(rounddir+"/result", "w")
                if fail:
                    SaveFileToDir("task/run_"+case+".sh", FAILDIR, "run_"+case+".sh")
                    fresult.write("FAILED")
                elif error:
                    fresult.write("ERROR")
                else:
                    fresult.write("OK")
                    if deleteok:
                        if os.access(rounddir+"/mv.txt", os.R_OK): os.remove(rounddir+"/mv.txt")
                        if os.access(rounddir+"/ewl.trc", os.R_OK): os.remove(rounddir+"/ewl.trc")
                        if os.access(rounddir+"/sw_reg.trc", os.R_OK): os.remove(rounddir+"/sw_reg.trc")
                        if os.access(rounddir+"/sw_reg.trc.cmp", os.R_OK): os.remove(rounddir+"/sw_reg.trc.cmp")
                        if os.access(rounddir+"/scaled.yuyv", os.R_OK): os.remove(rounddir+"/scaled.yuyv")
                fresult.close()

                # Create log entry
                CheckResult(rounddir, flog, freport)

        # Result for this case, fail is dominant, single success overrides errors
        if casefailed:
            casesfailed+=1
            if ffailed: ffailed.write(case+"\n")
            # Create script for debugging failed cases
            fdebug.write("if [ -e case_"+case+".log ] ; then gvim -d case_"+case+".log* ; fi\n")
            fdebug.write("if [ -e case_"+case+".yuv ] ; then viewyuv -c"+
                         " -w"+str(GetCasePar(case, 'width'))+
                         " -h"+str(GetCasePar(case, 'height'))+
                         " case_"+case+".yuv* ; fi\n")
        elif caseok:
            casesok+=1
        elif caseerror:
            caseserror+=1

    if ffailed: ffailed.close()
    print
    print "Total of", casecount, "cases:  ", casesok, "OK ", casesfailed, "FAILED ",caseserror,"error  "
    if casesfailed and ffailed:
        print "Created failed cases tasklist: "+failedlist
    flog.write("Total of "+str(casecount)+" cases:  "+str(casesok)+" OK  "+str(casesfailed)+" FAILED  "+str(caseserror)+" error   ")
    flog.close()
    freport.close()
    fdebug.close()


################################################################################
if __name__ == '__main__':
    main()
