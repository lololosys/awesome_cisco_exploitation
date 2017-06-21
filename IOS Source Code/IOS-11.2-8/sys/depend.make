# $Id: depend.make,v 3.3.12.9 1996/07/14 07:08:16 hampton Exp $
# $Source: /release/112/cvs/Xsys/depend.make,v $
#------------------------------------------------------------------
# Copyright (c) 1992-1996, 97 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: depend.make,v $
# Revision 3.3.12.9  1996/07/14  07:08:16  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.3.12.8  1996/06/16  21:24:28  vnguyen
# Finally committing Brasil to California_branch
#
# Revision 3.3.12.7  1996/05/23  03:08:13  mdb
# Make the sed expression more aggressive for finding the basename of the
# .lis filename being added to the .D dependancy file. [CSCdi58243]
# Branch: California_branch
#
# Revision 3.3.12.6  1996/05/17  10:26:23  ppearce
# Merge IbuMod into Calif
#
# Revision 3.3.2.4  1996/05/05  22:39:26  ppearce
# Sync to IbuMod_Calif_baseline_960504
# Revision 3.3.12.5  1996/05/10  23:24:32  gstovall
# CSCdi56792:  Need to remove unused files
# Branch: California_branch
# Nuke support for the Irix image, also stamp out the last of the CS500.
#
# Revision 3.3.2.3  1996/04/29  17:29:33  ppearce
# IBU modularity: correction for "make depend"
# Branch: IbuMod_Calif_branch
#
# Revision 3.3.2.2  1996/04/26  08:03:34  dstine
# Branch: IbuMod_Calif_branch
# - commit of IBU modularity work, phase 1.
#
# Revision 3.3.12.4  1996/05/09  14:09:38  rbadri
# Branch: California_branch
# LANE, UNI3.1 features
#
# Revision 3.3.12.3  1996/04/19  14:29:45  che
# CSCdi55138:  IosSec_branch feature commit
# Branch: California_branch
# IosSecurity feature commits
#
# Revision 2.1.6.7  1996/03/30  00:11:22  carrel
# cvs fixes
#
# Revision 2.1.6.3  1995/11/28  00:17:17  bew
# Branch: IosSec_branch
# Port of newest Cylink SDU code, plus CS cleanup.
#
# Revision 2.1.6.2  1995/11/17  16:21:01  carrel
# Branch: IosSec_branch
# Sync to Arkansas
#
# Revision 2.1.6.1  1995/10/03  18:54:34  xliu
# Branch: IosSec_branch
# Add two more levels search in the definiton of FILES (depend.make) and
# BFILES (tags-id.make) for the subdir added under crypto dir.
# Revision 3.3.12.2.6.1  1996/04/10  02:33:20  rlowe
# Add SYNALC to platforms for sys dependancies.  Also fix-up
# integration of wbu makefile with sys makefile for obj-68-alc.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.3.12.2  1996/03/21  22:09:35  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.3.12.1  1996/03/18  18:44:57  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.2.2.2  1996/03/19  00:19:36  mbeesley
# Branch: ELC_branch
# Sync to V111_1_3
#
# Revision 3.2.2.1  1995/11/22  20:24:24  mbeesley
# Intial commit of c7100 support.
# Branch: ELC_branch
#
# Revision 3.3  1996/02/17  09:06:44  rchiao
# CSCdi49271:  makefile changes for ClearCase
# adding sysroot definition for atria. For obj-* changes, please see
# obj-4k/makefile or obj-68/makefile for example.
#
# Revision 3.2  1995/11/17  08:32:02  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  10:48:00  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.3  1995/11/08  20:47:08  shaker
# Merge Arkansas_branch into 11.1 mainline.
#
# Revision 2.2  1995/07/17  23:26:38  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.1  1995/06/07  19:58:24  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

.PHONY:	dependancies.specific

#
# Cut down on search of suffixes
#
.SUFFIXES:

#
# The following rules cancel out the internal suffix rules for
# objects, SCCS and RCS files. This speeds up dependancy parsing.
#

%: %.o
%: %,v
%: RCS/%,v
%: s.%
%: SCCS/s.%

SHELL=/bin/sh
ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif
#
# Files that we are interested in, relative to a subdirectory
#
FILES   := $(filter-out $(SYSROOT)/%/cisco.c, \
		$(filter-out $(SYSROOT)/%/cisco-%.c, \
			 $(wildcard $(SYSROOT)/*.[cCS] $(SYSROOT)/*/*.[cCS] \
			 $(SYSROOT)/lib/cisco/src/*.[cCS] \
			 $(SYSROOT)/lib/cisco/asm-68/*.[cCS] \
			 $(SYSROOT)/lib/cisco/asm-4k/*.[cCS] \
			 $(SYSROOT)/lib/ansi/ctype/*.[cCS] \
			 $(SYSROOT)/lib/ansi/reent/*.[cCS] \
			 $(SYSROOT)/lib/ansi/stdio/*.[cCS] \
			 $(SYSROOT)/lib/ansi/stdlib/*.[cCS] \
			 $(SYSROOT)/lib/ansi/string/*.[cCS] \
			 $(SYSROOT)/kerberos/c5libs/*/*.[cCS] \
			 $(SYSROOT)/kerberos/c5libs/*/*/*.[cCS] \
			 $(SYSROOT)/kerberos/csflibs/*/*.[cCS] \
			 $(SYSROOT)/kerberos/csfhost/*/*.[cCS] \
			 $(SYSROOT)/crypto/*/*.[cCS] \
			 $(SYSROOT)/crypto/*/*/*.[cCS] \
			 $(SYSROOT)/crypto/*/*/*/*.[cCS] \
			 $(SYSROOT)/crypto/*/*/*/*/*.[cCS])))
FILES_D := $(join $(dir $(FILES)), $(addprefix .D., $(notdir $(FILES))))
FILES_REG  := $(wildcard ../*/*.reg)
FILES_RPC  := $(wildcard ../*/*.rpc)

#
# Defining all the CPU types is a hack, but it gets more include files.
# The symbol DEPEND is used to let modules that include files based on
# other symbols defined (like SMALLASM) that they should include all the
# possibilities.
#
CFLAGS  := -w -DTARGET_CISCO -DTARGET_UNIX \
	      -DOBJECT_68 -DOBJECT_4K -DOBJECT_36 \
              -DCSC2 -DPAN -DBRASIL -DRP1 -DUNIX -DXX -DSIERRA -DRSP \
	      -DSAPPHIRE -DRVIP -DSVIP -DPREDATOR -DSYNALC -DDEPEND

ifeq ($(origin INCLUDE_FILE), undefined)
include ../makefile.68
else
include $(INCLUDE_FILE)
endif

PROCESS :=	$(CC) $(CFLAGS) -I../h -I../machine -MM -MG
FLATTEN :=	../scripts/flatten_dependancies.perl
DEPEND	:=	$(SYSROOT)/dependancies
SPECIAL	:=	../depend.specials
DEPEND_SEED :=  dependancies.seed

all: depend 

dependancies.global : ../scripts/dummy_depend.c
	@$(PROCESS) $< | $(FLATTEN) -p > ../$@

dependancies.specific : 
	@echo "include .DD.*" > dependancies.specific

.D.%.c : %.c
	@$(PROCESS) $< | $(FLATTEN) -l >$@

.D.%.C : %.C
	@$(PROCESS) $< | $(FLATTEN) -l >$@

.D.%.S : %.S
	@$(PROCESS) $< | $(FLATTEN) >$@

introdepend:
	@echo "Computing dependancies"

depend_seed:
	@echo ${FILES} > $(SYSROOT)/$(DEPEND_SEED)$$
	@perl -ne 'foreach $$i (split(/\s+/)) {$$f=$$i; \
	     $$f =~ s!$(SYSROOT)!\.\.!g; $$i =~ s/.*\///; \
	     $$i =~ s/(.*)\.[cS]/\1\.o \1\.lis/; print "$$i : $$f\n";}' \
	     $(SYSROOT)/$(DEPEND_SEED)$$ > $(SYSROOT)/$(DEPEND_SEED);
	@\rm -f $(SYSROOT)/$(DEPEND_SEED)$$

alldepend: $(SYSROOT)/h/interface_types.h depend_seed
	@cat $(SPECIAL) > $(DEPEND).temp
	@echo "" >> $(DEPEND).temp
	@echo "# start of dependancies generated by ./depend.make" >> $(DEPEND).temp
	@echo "" >> $(DEPEND).temp
	@(for i in ${FILES_REG} ; do \
	  echo "$$i : $$i " | \
	  sed -e 's#^[^ ]*/##' -e 's/\.reg/.regc/' >> $(DEPEND).temp; \
	  echo "$$i : $$i " |  \
	  sed -e 's#^[^ ]*/##' -e 's/\.reg/.regh/' >> $(DEPEND).temp; \
	  done)
	@(for i in ${FILES_RPC} ; do \
	  echo "$$i : $$i " | \
	  sed -e 's#^[^ ]*/##' -e 's/\.rpc/.rpcc/' >> $(DEPEND).temp; \
	  echo "$$i : $$i " |  \
	  sed -e 's#^[^ ]*/##' -e 's/\.rpc/.rpch/' >> $(DEPEND).temp; \
	  done)
	@echo "" >> $(DEPEND).temp
	@echo "# end of dependancies generated by ./depend.make" >> $(DEPEND).temp
	@echo "" >> $(DEPEND).temp
	@echo "include \$$(SYSROOT)/$(DEPEND_SEED)" >> $(DEPEND).temp
	@mv $(DEPEND).temp $(DEPEND)

depend: introdepend alldepend

.PHONY: depend all alldepend

%: %,v

%: RCS/%,v

../h/interface_types.h : ../h/interface_private.h
	../scripts/gen_if_types

