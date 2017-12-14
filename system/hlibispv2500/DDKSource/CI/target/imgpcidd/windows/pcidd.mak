#################################################################################
#
#	Name		: Makefile for WinNT PCI Debug Kernel Driver
#	Author		: John Metcalfe
#	Created	 	: 17 Sept 2002
#
# Copyright     : 1997 - 2002 by Imagination Technologies Limited. All rights reserved.
#                 No part of this software, either material or conceptual, may
#                 be copied, distributed, transmitted, transcribed, stored in a
#                 retrieval system or translated into any human or computer
#                 language in any form by and means, electronic, mechanical,
#                 manual or otherwise, or disclosed to third parties without the
#                 express written permission of VideoLogic Limited, Unit 8,
#                 HomePark Industrial Estate, King's Langley, Hertfordshire,
#                 WD4 8LZ, U.K.
#
#
################################################################################

!CMDSWITCHES +S
DRVNAME  = imgpcidd

################################################### Display building banner

!IFNDEF IMG_DEBUG_FLAG
BUILDTYPE=RELEASE
!MESSAGE This Driver is a $(BUILDTYPE) Build.
!ELSE
BUILDTYPE=DEVELOPMENT
!MESSAGE This Driver is a $(BUILDTYPE) Build. For development use only.
!ENDIF

#Guess the working directory if it hasn't been set
CURDIR = $(MAKEDIR)
!IFNDEF WORKROOT
WORKROOT=$(CURDIR)\..\..\..\..\..
!ENDIF

!MESSAGE Work root set to: $(WORKROOT)
!MESSAGE .

##########################################################################
# Path macros that are dependant on build type
##########################################################################

!ifdef IMG_DEBUG_FLAG
!MESSAGE  WinNT PCI Driver DEBUG BUILD
BIN   = $(CURDIR)\bin\debug
!ELSE
!MESSAGE  WinNT PCI Driver NON-DEBUG BUILD
BIN  = $(CURDIR)\bin\release
!ENDIF

################## create directories

SRC		= $(CURDIR)\code
INC		= $(SRC);

#################################################################################
#			Default settings
#################################################################################

TOOLSROOT		= $(WORKROOT)\tools
EXTERNTOOLSROOT	= $(TOOLSROOT)\external
DDKROOT 	= $(EXTERNTOOLSROOT)\REL\ddk\winxp

INCLUDETYPE 	= $(WORKROOT)\include\REL\1.0\include\win32
INCLUDECOMMON 	= $(CURDIR)\..\..\include

############################################################################
# Define the tools  
############################################################################

LNK=$(DDKROOT)\bin\link
CC32=$(DDKROOT)\bin\cl
MASM=$(DDKROOT)\bin\ml
LIB32=$(DDKROOT)\bin\lib
DDKLIB=$(DDKROOT)\lib\i386

INCLUDES = $(DDKROOT)\inc;$(INC);$(INCLUDETYPE);$(INCLUDECOMMON)

LIB		= $(DDKROOT)\lib;
                                             

############################################################################
# Define global C compiler and linker options.
############################################################################

CFLAGS = -D_X86_=1 -Di386=1 -DSTD_CALL -DCONDITION_HANDLING=1 -DNT_INST=0 -DWIN32=100 
CFLAGS = $(CFLAGS) -D_NT1X_=100 -DWINNT=1 -D_WIN32_WINNT=0x0501 /DWINVER=0x0501 
CFLAGS = $(CFLAGS) -D_WIN32_IE=0x0600 -DWIN32_LEAN_AND_MEAN=1 -DWINXP -DMSVC_IA32
CFLAGS = $(CFLAGS) -DDEVL=1 -D__BUILDMACHINE__=WinDDK -DNDEBUG
CFLAGS = $(CFLAGS) -nologo /c /Zel /Zp8 /Gy -cbstring /W4 /Gz /G6 /Gi- /Gm- /GX- /GR- /GF
CFLAGS = $(CFLAGS) -Z7 /QIfdiv- /QIf /QI0f /Zvc6

LFLAGS = -nologo -MERGE:_PAGE=PAGE -MERGE:_TEXT=.text -SECTION:INIT,d -OPT:REF -OPT:ICF 
LFLAGS = $(LFLAGS) -IGNORE:4001,4037,4039,4065,4070,4078,4087,4089,4198,4221 -machine:i386
LFLAGS = $(LFLAGS) -INCREMENTAL:NO -FULLBUILD /release -NODEFAULTLIB /WX
LFLAGS = $(LFLAGS) -version:5.1 -osversion:5.1 /opt:nowin98 -STACK:262144,4096
LFLAGS = $(LFLAGS) -dll -driver -align:0x20 -subsystem:native,5.1 -base:0x10000 
LFLAGS = $(LFLAGS) -debugtype:vc6 /MAP /MAPINFO:LINES

!IFDEF IMG_DEBUG_FLAG
OBJ    = $(BIN)\tmpdbg
CFLAGS = $(CFLAGS) -DIMG_DEBUG_FLAG -DDBG=1 -DRDRDBG -DSRVDBG /nologo /Fo$(OBJ)\ -Z7 /Od /Oi /Oy- 
LFLAGS = $(LFLAGS) -debug:FULL
MFLAGS   = /c /W3 /Zi /Cx /coff /DBLD_COFF /DIS_32 /DMASM6 /DNEWSTRUCTS /DWINNT /nologo
MAPFILE   = /map:$(PROJ).map

!ELSE
OBJ    = $(BIN)\tmp
CFLAGS = $(CFLAGS) /nologo /Fo$(OBJ)\ /WX -Z7 /Oxs /Oy
LFLAGS = $(LFLAGS)
MFLAGS    = /c /W3  /Cx /coff /DBLD_COFF /DIS_32 /DMASM6 /DNEWSTRUCTS /DWINNT /nologo

!ENDIF

############################################################################
# Initialise Global defines
############################################################################

!IFDEF LEGACY
CFLAGS = $(CFLAGS) -DUSE_LEGACY_STRUCTURES
!MESSAGE This Driver will use legacy support structures!
!endif

#
# Create destination directories if they do not exist
#
!if !EXISTS($(OBJ))
!if [md $(OBJ)]
!endif
!endif
!if !EXISTS($(BIN))
!if [md $(BIN)]
!endif
!endif

################################################################################
#
#	Object files
#
################################################################################

DRVOBJS=$(OBJ)\MAIN.OBJ \
		$(OBJ)\PCIDD.OBJ \
		$(OBJ)\PCIUTILS.OBJ \
		$(OBJ)\hostfunc.OBJ \
		$(OBJ)\registry.OBJ 


################################################################################
#
#	Dependencies & compilation
#
################################################################################

all: setupvars $(BIN)\$(DRVNAME).sys

clean:
	@del $(BIN)\$(DRVNAME).sys
	@del /q $(OBJ)\*

setupvars:
    @set include=$(INCLUDES)


################################################################################
#
#	Compile dependencies
#
################################################################################

$(OBJ)\main.obj: $(SRC)\main.c
    $(CC32) $(CFLAGS) $(SRC)\main.c 

$(OBJ)\hostfunc.obj: $(SRC)\hostfunc.c
    $(CC32) $(CFLAGS) $(SRC)\hostfunc.c

$(OBJ)\pcidd.obj: $(SRC)\pcidd.c
    $(CC32) $(CFLAGS) $(SRC)\pcidd.c

$(OBJ)\pciutils.obj: $(SRC)\pciutils.c
    $(CC32) $(CFLAGS) $(SRC)\pciutils.c

$(OBJ)\registry.obj: $(SRC)\registry.c
    $(CC32) $(CFLAGS) $(SRC)\registry.c

#################################################################################
#
#	Link etc.
#
#################################################################################

#
# Link
#
$(BIN)\$(DRVNAME).sys: $(DRVOBJS)
    $(LNK) @<<
	-entry:DriverEntry@8
	-MAP:$*.map
	-OUT:$(BIN)\$(DRVNAME).sys
	$(LFLAGS)
	$(DRVOBJS)
	$(DDKLIB)\ntoskrnl.lib	
	$(DDKLIB)\hal.lib	
<<

	
#################################################################################
#	End Of File
#################################################################################


