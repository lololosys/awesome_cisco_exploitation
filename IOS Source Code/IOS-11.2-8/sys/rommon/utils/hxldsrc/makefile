# $Id: makefile,v 3.2.62.1 1996/05/08 11:58:07 mdb Exp $
# $Source: /release/112/cvs/Xsys/rommon/utils/hxldsrc/makefile,v $
#------------------------------------------------------------------
# makefile
#
# June 1993, Rob Clevenger
#
# Copyright (c) 1994-1996 by cisco Systems, Inc.
# All rights reserved.
# 
# Makefile for hxld program.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.2.62.1  1996/05/08  11:58:07  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.2  1995/11/17  18:49:19  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:10:29  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.2  1995/07/17  23:32:15  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Clean up bogus tool includes
#
# Revision 2.1  1995/06/07  22:43:02  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../../../scripts/sysroot)
endif

default: all

include ../../../makefile.tools

CC= $(CC_NATIVE)

CFLAGS=	-O -DUNIX -I../../src
TARGET=	hxld-$(NATIVE_OSREV)

CBASE= 	main.c intelhex.c xtekhex.c binary.c \
	openstrm.c coff.c elf.c bout.c motsrec.c \
	image_file.c

all: $(TARGET)

hxld: $(TARGET)
	@

$(TARGET): $(CBASE) makefile
	$(CC) $(CFLAGS) -o $@ $(CBASE)

clean:
	rm -f *.o

clobber: clean
	@for prog in $(patsubst %-$(NATIVE_OSREV),%,$(TARGET)); do \
	    for file in $(SYSROOT)/rommon/utils/hxldsrc/$$prog-*; do \
		if [ -f $$file ]; then \
		    echo rm -f $$file; \
		    rm -f $$file; \
		fi; \
	    done; \
	done
	rm -f ID TAGS dependancies
	touch dependancies

#
# The include file dependencies
#
depend: dependancies $(CBASE)
	@echo "Computing dependancies in rommon/utils/hxldsrc"
	@$(CC) $(CFLAGS) -MM -MG $(CBASE) > dependancies
	@echo "Computing TAGS file in rommon/utils/hxldsrc"
	@etags -t $(CBASE)
	@echo "Computing ID file in rommon/utils/hxldsrc"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE)

dependancies:
	rm -f dependancies
	touch dependancies

include dependancies
