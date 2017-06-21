# $Id: tags-id.make,v 3.3.42.1 1996/03/18 18:45:38 gstovall Exp $
# $Source: /release/111/cvs/Xsys/tags-id.make,v $
#------------------------------------------------------------------
# Copyright (c) 1992-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: tags-id.make,v $
# Revision 3.3.42.1  1996/03/18  18:45:38  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.3.12.2  1996/03/07  08:27:14  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.3.12.1  1996/02/20  13:04:15  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.3  1995/12/24  09:29:57  che
# CSCdi46019:  Store Kerberos credentials in Kerberos library cred cache
# Make sure tags are made for files in the nested Kerberos and Crypto
# subdirectories.
#
# Revision 3.2  1995/11/17  08:32:50  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  10:48:40  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.2  1995/10/18  21:44:00  hampton
# Need a space conserving way to test build all images.  Add the top
# level targets production.testall, oddballs.testall, modular.testall,
# and everything.testall.  These delete each target after making it to
# limit disk space usage.  Also add a way to make a vi ctags file.
# [CSCdi42405]
#
# Revision 2.1  1995/06/07  19:59:10  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell scripts/sysroot)
endif


#
# Files that we are interested in, relative to the base of the trunk.
#
BFILES   := $(wildcard *.[cChS] */*.[cChS] */*/*.[cChS] */*/*/*.[cChS] \
	    */*/*/*/*.[cChS] *.regh */*.regh *.regc */*.regc */*.rpcc */*.rpch)

BFILES_T := $(join $(dir $(BFILES)), $(addprefix .T., $(notdir $(BFILES))))

# makefile invokes tags-id.make with empty target list, or "TAGS" or "id".
# if TAGS_TYPE is not defined, continue to make emacs-style tags; if it is
# defined (expected values are "ctags", "etags", or "ctags etags" for both),
# use it to determine which tags files get created.
ifeq ($(origin TAGS_TYPE), undefined)
TAGS_TYPE=etags
endif

all: TAGS id

TAGS:	$(TAGS_TYPE)

id:
	@echo "Computing ID file"
	@/bin/ls $(BFILES) | \
	nice mkid -S.S=asm -Sasm-c\| -Sasm+u -Sasm+p -S.C=c -S.regh=c -S.regc=c \
	  -S.rpch=c -S.rpcc=c -fID.temp -	
	@mv ID.temp $(SYSROOT)/ID

ctags:
	@echo "Computing tags file for vi"
	@ctags -t -f tags.temp $(BFILES) > tags.log 2>&1
	@mv tags.temp tags

etagsintro:
	@echo "Computing TAGS file for emacs"

.T.%.c : %.c
	@etags -t -f $@ $<

.T.%.C : %.C
	@etags -t -f $@ $<

.T.%.S : %.S
	@etags -f $@ $<

.T.%.h : %.h
	@etags -t -f $@ $<

.T.%.regh : %.regh
	@etags -t -f $@ $<

.T.%.regc : %.regc
	@etags -t -f $@ $<

.T.%.rpcc : %.rpcc
	@etags -t -f $@ $<

.T.%.rpch : %.rpch
	@etags -t -f $@ $<

etags: etagsintro $(BFILES_T)
	@fixetags.perl $(BFILES_T) > $(SYSROOT)/TAGS.temp
	@mv $(SYSROOT)/TAGS.temp $(SYSROOT)/TAGS

.PHONY: all id

%: %,v

%: RCS/%,v
