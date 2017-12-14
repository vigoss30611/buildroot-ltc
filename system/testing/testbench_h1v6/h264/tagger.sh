#!/bin/bash
#create tag


cvs tag $1 EncGetOption.c
cvs tag $1 EncGetOption.h
cvs tag $1 Makefile
cvs tag $1 H264TestBench.c
cvs tag $1 H264TestBench.h
cvs tag $1 TestBenchInternals.c
cvs tag $1 testid.c
cvs tag $1 README
#cvs tag $1 tagger.sh
cvs tag $1 test_h264.sh
cvs tag $1 checkcase_h264.sh
cvs tag $1 checkall_h264.sh

cvs tag $1 ../../../source/inc/basetype.h \
           ../../../source/inc/h264encapi.h \
           ../../../source/inc/h264encapi_ext.h \
           ../../../source/inc/ewl.h 

cvs tag $1 ../../../source/common/encasiccontroller.c \
            ../../../source/common/encasiccontroller.h \
            ../../../source/common/encasiccontroller_v2.c \
            ../../../source/common/enccfg.h \
            ../../../source/common/encpreprocess.c \
            ../../../source/common/encpreprocess.h \
            ../../../source/common/enccommon.h
 
cvs tag $1 ../../../source/h264

cvs tag $1 ../../../source/camstab/vidstabalg.c \
           ../../../source/camstab/vidstabcommon.c \
           ../../../source/camstab/vidstabcommon.h

cvs tag $1  ../../Makefile
cvs tag $1  ../../ewl
cvs tag $1  ../../kernel_module
cvs tag $1  ../../memalloc

cvs tag $1  ../../debug_trace/encdebug.h \
		../../debug_trace/enctracestream.h \
		../../debug_trace/enctrace.c \
		../../debug_trace/enctrace.h \
 		../../debug_trace/vidstabdebug.h       
 
