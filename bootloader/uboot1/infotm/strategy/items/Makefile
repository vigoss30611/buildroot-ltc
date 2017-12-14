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

LIB	= $(obj)libitems.a

AOBJS	=

COBJS-y += items.o
COBJS-y += init_config.o

COBJS	:= $(sort $(COBJS-y))
SRCS	:= $(AOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(AOBJS) $(COBJS))

CPPFLAGS += -I.. -I./
CFLAGS += -I.. -I./

all:	$(LIB) $(AOBJS)

$(LIB): $(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#$(obj)env_embedded.o: $(src)env_embedded.c $(obj)../tools/envcrc
#	$(CC) $(AFLAGS) -Wa,--no-warn \
#		-DENV_CRC=$(shell $(obj)../tools/envcrc) \
#		-c -o $@ $(src)env_embedded.c

#$(obj)../tools/envcrc:
#	$(MAKE) -C ../tools

#########################################################################

include $(SRCTREE)/rules.mk

sinclude $(obj).depend
#########################################################################
