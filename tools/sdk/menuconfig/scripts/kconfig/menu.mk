# =====================================================================
# Kernel configuration targets
# These targets are used from top-level makefile
#
# SPDX-License-Identifier:  GPL-2.0
#
# Merge from u-boot-2016.09
#

PHONY += menuconfig nconfig config

ifdef KBUILD_KCONFIG
Kconfig := $(CURDIR)/configs/$(KBUILD_KCONFIG)
else
Kconfig := $(CURDIR)/Kconfig
endif

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	else echo sh; fi ; fi)

HOSTCC       = cc
HOSTCFLAGS	 = -Wall -Wstrict-prototypes -O2 -fomit-frame-pointer
obj:= $(CURDIR)/tools/scripts/kconfig
src:= $(CURDIR)/tools/scripts/kconfig

menuconfig: $(obj)/mconf
	$< $(Kconfig)

nconfig: $(obj)/nconf
	$< $(Kconfig)

config: $(obj)/conf
	$< $(Kconfig)

silentoldconfig: $(obj)/conf
	@mkdir -p include/config include/generated
	$< --$@ $(Kconfig)

%_defconfig: $(obj)/conf
	$< --defconfig=$(DEFCONFIG_DIR)/$@ $(Kconfig)

# Help text used by make help
help:
	@echo  'Configure targets:'
	@echo  '  config	- Update current config utilising a line-oriented program'
	@echo  '  nconfig	- Update current config utilising a ncurses menu based program'
	@echo  '  menuconfig	- Update current config utilising a menu based program'

clean:
	rm -f $(obj)/*.o
	rm -f $(obj)/$(hostprogs-y)
	rm -f $(obj)/.d*
	rm -f $(obj)/lxdialog/*.o
	rm -rf $(SRCTREE)/include/config $(SRCTREE)/include/generated

# lxdialog stuff
check-lxdialog  := $(obj)/lxdialog/check-lxdialog.sh

HOST_EXTRACFLAGS += $(shell $(CONFIG_SHELL) $(check-lxdialog) -ccflags) \
									-DLOCALE

# ===========================================================================
# Shared Makefile for the various kconfig executables:
# conf:   Used for defconfig, oldconfig and related targets
# nconf:  Used for the nconfig target.
#         Utilizes ncurses
# mconf:  Used for the menuconfig target
#         Utilizes the lxdialog package

lxdialog := lxdialog/checklist.o lxdialog/util.o lxdialog/inputbox.o
lxdialog += lxdialog/textbox.o lxdialog/yesno.o lxdialog/menubox.o

conf-objs := conf.o zconf.tab.o
mconf-objs := mconf.o zconf.tab.o $(lxdialog)
nconf-objs := nconf.o zconf.tab.o nconf.gui.o

$(obj)/zconf.tab.o: $(obj)/zconf.lex.c $(obj)/zconf.hash.c

hostprogs-y = conf mconf nconf

# Check that we have the required ncurses stuff installed for lxdialog (menuconfig)
PHONY += $(obj)/dochecklxdialog
$(addprefix $(obj)/,$(lxdialog)): $(obj)/dochecklxdialog
$(obj)/dochecklxdialog:
	$(CONFIG_SHELL) $(check-lxdialog) -check $(HOSTCC) $(HOST_EXTRACFLAGS) $(HOSTLOADLIBES_mconf)

HOSTLOADLIBES_mconf = $(shell $(CONFIG_SHELL) $(check-lxdialog) -ldflags $(HOSTCC))
HOSTLOADLIBES_nconf = $(shell \
										pkg-config --libs menuw panelw ncursesw 2>/dev/null \
										|| pkg-config --libs menu panel ncurses 2>/dev/null \
										|| echo "-lmenu -lpanel -lncurses"  )

include tools/scripts/Makefile.host
