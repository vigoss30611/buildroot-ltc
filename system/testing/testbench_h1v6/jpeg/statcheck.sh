#!/bin/bash
#Check status of files

echo "/test/jpeg"
cvs stat armscript.txt | grep atus
cvs stat checkall_jpeg.sh | grep atus
cvs stat checkcase_jpeg.sh | grep atus
cvs stat EncGetOption.c | grep atus
cvs stat EncGetOption.h | grep atus
cvs stat GetNextOpt.c | grep atus
cvs stat GetNextOpt.h | grep atus
cvs stat JpegTestBench.c | grep atus
cvs stat JpegTestBench.h | grep atus
cvs stat Makefile | grep atus
cvs stat README | grep atus
cvs stat statcheck.sh | grep atus
cvs stat tagger.sh | grep atus
cvs stat TestBenchInternals.c | grep atus
cvs stat test_jpeg.sh | grep atus

echo "/common"
cvs stat ../../../source/common/ | grep atus 

echo "/jpeg"
cvs stat ../../../source/jpeg/ | grep atus 

echo "/inc"
cvs stat ../../../source/inc/basetype.h | grep atus 
cvs stat ../../../source/inc/jpegencapi.h | grep atus 
cvs stat ../../../source/inc/ewl.h | grep atus 

echo "/ewl"
cvs stat  ../../ewl/ | grep atus

echo "/linux_reference"
cvs stat  ../../Makefile | grep atus

echo "/linux_reference/debug_trace"
cvs stat  ../../debug_trace/ | grep atus

echo "/test_jpeg"
cvs stat -v Makefile
