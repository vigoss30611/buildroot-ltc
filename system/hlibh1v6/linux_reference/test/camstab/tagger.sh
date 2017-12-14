#!/bin/bash
#create tag


cvs tag $1 EncGetOption.c
cvs tag $1 EncGetOption.h
cvs tag $1 Makefile
cvs tag $1 standalone.c

cvs tag $1 README
#cvs tag $1 tagger.sh
cvs tag $1 test_vs.sh
cvs tag $1 checkcase_vs.sh
cvs tag $1 checkall_vs.sh

cvs tag $1 ../../../source/inc/basetype.h \
           ../../../source/inc/vidstbapi.h \
           ../../../source/inc/ewl.h 

cvs tag $1 ../../../source/camstab 

cvs tag $1  ../../Makefile
cvs tag $1  ../../ewl
cvs tag $1  ../../kernel_module
cvs tag $1  ../../memalloc

cvs tag $1  ../../debug_trace/vidstabdebug.h
 
