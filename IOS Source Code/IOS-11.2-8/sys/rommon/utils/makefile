# $Id: makefile,v 3.2.62.2 1996/05/08 11:58:03 mdb Exp $
# $Source: /release/112/cvs/Xsys/rommon/utils/makefile,v $
#------------------------------------------------------------------
# makefile
#
# August 1993, Rob Clevenger
#
# Copyright (c) 1994-1996 by cisco Systems, Inc.
# All rights reserved.
# 
# Makefile for the Monitor utility programs.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.2.62.2  1996/05/08  11:58:03  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.2.62.1  1996/03/19  03:53:27  rchiao
# Branch: California_branch
# Added ClearCase build changes.
#
# Revision 3.2  1995/11/17  18:48:39  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:10:07  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.2  1995/07/17  23:32:12  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Clean up bogus tool includes
#
# Revision 2.1  1995/06/07  22:42:33  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../../scripts/sysroot)
endif

default: all

include ../../makefile.tools
CC= $(CC_NATIVE)

CBASE= eprom_elf.c mkffsi.c squish_elf.c epromsum.c
CPPOPTS = -DUNIX -I../src -I/usr/include -I../../h -I../../les

EPROM_ELF  = eprom_elf-$(NATIVE_OSREV)
MKFFSI     = mkffsi-$(NATIVE_OSREV)
SQUISH_ELF = squish_elf-$(NATIVE_OSREV)
EPROMSUM   = epromsum-$(NATIVE_OSREV)

UTILS      = $(EPROM_ELF) $(MKFFSI) $(SQUISH_ELF) $(EPROMSUM)
UTILS_ROOT = $(patsubst %-$(NATIVE_OSREV),%,$(UTILS))

all: $(UTILS)

eprom_elf:  $(EPROM_ELF)
	@
mkffsi:     $(MKFFSI) 
	@
squish_elf: $(SQUISH_ELF)
	@
epromsum:   $(EPROMSUM)
	@

$(EPROMSUM): epromsum.c makefile
	$(CC) $(CPPOPTS) -o $@ epromsum.c

$(EPROM_ELF): eprom_elf.c makefile
	$(CC) $(CPPOPTS) -o $@ eprom_elf.c

$(MKFFSI): mkffsi.c makefile
	$(CC) $(CPPOPTS) -o $@ mkffsi.c

$(SQUISH_ELF): squish_elf.c makefile
	$(CC) $(CPPOPTS) -o $@ squish_elf.c

clean:
	@for prog in $(UTILS_ROOT); do \
	    for file in $(SYSROOT)/rommon/utils/$$prog-*; do \
		if [ -f $$file ]; then \
			echo rm -f $$file; \
	    		rm -f $$file*; \
		fi; \
	    done; \
	done

clobber: clean
	rm -f ID TAGS

VIEW := $(shell viewname)
STAMP = $(shell datestamp)

SYSREVA= $(shell $(SYSROOT)/scripts/getversion SYS_MAJORVERSION < $(SYSROOT)/h/master.h)
SYSREVB= $(shell $(SYSROOT)/scripts/getversion SYS_MINORVERSION < $(SYSROOT)/h/master.h)
SYSREV= $(SYSREVA).$(SYSREVB)\($(STAMP)\)

ROMREVA= $(shell $(SYSROOT)/scripts/getversion ROM_MAJORVERSION < $(SYSROOT)/rommon/src/mon_defs.h)
ROMREVB= $(shell $(SYSROOT)/scripts/getversion ROM_MINORVERSION < $(SYSROOT)/rommon/src/mon_defs.h)
ROMREV= $(ROMREVA.$(ROMREVB)\($(STAMP)\)"

banner.sh : banner.SH ../src/mon_defs.h
	./banner.SH $(STAMP) $(ROMREV) $(SYSREV) $(VIEW)

symbols_elf.sh : symbols_elf.SH ../src/mon_defs.h
	./symbols_elf.SH $(ROMREV) $(VIEW)

../version:
	@echo 100 > ../version

#
# The include file dependencies
#
depend: dependancies $(CBASE)
	@echo "Computing dependancies in rommon/utils"
	@$(CC) $(CPPOPTS) -MM -MG $(CBASE) > dependancies
	@echo "Computing TAGS file in rommon/utils"
	@etags -t $(CBASE)
	@echo "Computing ID file in rommon/utils"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE)

dependancies: banner.sh symbols_elf.sh ../version
	rm -f dependancies
	touch dependancies

include dependancies

