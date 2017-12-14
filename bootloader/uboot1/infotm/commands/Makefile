#
# (C) Copyright 2004-2006
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

include $(TOPDIR)/config.mk

LIB	= $(obj)libicmd.a

AOBJS	=

COBJS-y += cmd_vs.o tt_lib.o  \
		   cmd_vstest.o tt_blk.o tt_crypto.o   \
		   tt_wire.o cmd_bootl.o   \
		   cmd_load.o cmd_items.o cmd_skeleton.o cmd_net.o
ifneq ($(MODE),simple)
COBJS-y += cmd_nand.o tt_ff.o tt_nand.o
endif
#COBJS-$(CONFIG_FASTBOOT) += cmd_fastboot.o

COBJS	:= $(sort $(COBJS-y))
SRCS	:= $(AOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(AOBJS) $(COBJS))

CPPFLAGS += -I..

all:	$(LIB) $(AOBJS)

$(LIB): $(obj).depend $(OBJS)
	@$(AR) $(ARFLAGS) $@ $(OBJS)
	@echo "AR    $(subst $(OBJTREE)/,,$@)"

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
