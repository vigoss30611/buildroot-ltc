#
# (C) Copyright 2006
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
include $(TOPDIR)/include/config.mk
LIB	:= $(obj)libflash.a

COBJS-y += infotm_flash.o

ifeq ($(SOC), q3f)
#COBJS-y += flash_q3f_fpga.o
COBJS-y += flash_q3f.o
endif

ifeq ($(SOC), apollo3)
#COBJS-y += flash_q3f_fpga.o
COBJS-y += flash_q3f.o
endif

ifeq ($(SOC), imapx800)
#COBJS-y += flash_x800a.o
COBJS-y += flash_q3.o
endif
#COBJS-y += flash_imapx800.o

COBJS	:= $(COBJS-y)
SRCS	:= $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))

all:	$(LIB)

$(LIB): $(obj).depend $(OBJS)
	@$(AR) $(ARFLAGS) $@ $(OBJS)
	@echo "AR    $(subst $(OBJTREE)/,,$@)"

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
