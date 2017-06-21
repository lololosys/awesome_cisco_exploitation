# $Id: Makefile,v 3.2.58.3 1996/05/10 23:16:37 gstovall Exp $
# $Source: /release/112/cvs/Xboot/Makefile,v $
#------------------------------------------------------------------
# Makefile to compile all versions of Bootstrap ROM
#
# Copyright (c) 1989-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: Makefile,v $
# Revision 3.2.58.3  1996/05/10  23:16:37  gstovall
# CSCdi56792:  Need to remove unused files
# Branch: California_branch
# The beast still exists, finish killing off the CS500, and while I am in
# there, nuke the even crustier CSC1.
#
# Revision 3.2.58.2  1996/05/09  13:59:29  rbadri
# Branch: California_branch
# Synergy features
#
# Revision 3.2.58.1.18.1  1996/05/01  05:58:19  rlowe
# Add obj-68-alc to list of boot object directories.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.2.58.1  1996/03/19  03:55:00  rchiao
# Branch: California_branch
# Added ClearCase build changes.
#
# Revision 3.2  1995/11/17  07:40:45  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  09:03:52  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1  1995/06/07  19:11:19  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

# Crash us if /bin/make is used:
ifeq ($(origin USER),undefined)
USER=Bozo
endif

SHELL=	/bin/sh
OPTIONS=
DIRS=	 obj-68-ags obj-68-c3000 obj-68-c4000 obj-68-c7000 obj-68-alc

ifeq ($(origin QUIET), undefined)
QUIET_COM=@\#
QUIET_CC=
else
QUIET_COM=@
QUIET_CC=@
endif

VIEW := $(shell viewname)
STAMP = $(shell datestamp)
BOOTREVA= $(shell ../sys/scripts/getversion MAJORVERSION < defs.h)
BOOTREVB= $(shell ../sys/scripts/getversion MINORVERSION < defs.h)
BOOTREV= $(BOOTREVA).$(BOOTREVB)\\\($(STAMP)\\\)

default: boot

all:	clean depend boot

symbols.sh: symbols.SH defs.h
	./symbols.SH $(STAMP) $(BOOTREV) $(VIEW)

cisco.sh: cisco.SH defs.h
	./cisco.SH $(STAMP) $(BOOTREV) $(VIEW)

boot:
	$(QUIET_COM)echo "Making $@ in directories: $(DIRS)"
	$(QUIET_CC)for dir in $(DIRS) ; do \
		(cd $$dir ; \
		echo "EXECUTING 'make boot' in `pwd`" ; \
		$(MAKE) boot) ; \
	done

lint:
	$(QUIET_COM)echo "Making $@ in directories: $(DIRS)"
	$(QUIET_CC)for dir in $(DIRS) ; do \
		(cd $$dir ; \
		echo "EXECUTING 'make lint' in `pwd`" ; \
		$(MAKE) lint) ; \
	done

clean:  cisco.sh symbols.sh
	$(QUIET_COM)echo "Making $@ in directories: $(DIRS)"
	$(QUIET_CC)for dir in $(DIRS) ; do \
		(cd $$dir ; \
		touch dependancies ; \
		echo "EXECUTING 'make clean' in `pwd`" ; \
		$(MAKE) clean) ; \
	done

depend:
	$(QUIET_COM)echo "Making $@ in directories: $(DIRS)"
	$(QUIET_CC)for dir in $(DIRS) ; do \
		(cd $$dir ; \
		touch dependancies ; \
		echo "EXECUTING 'make depend' in `pwd`" ; \
		$(MAKE) depend) ; \
	done
	@echo "Computing TAGS file"
	@etags -t *.[Shc] */*.[Shc]
	@echo "Computing ID file"	
	@mkid -S.S=asm -Sasm-c\| -Sasm+u -Sasm+p *.[Shc] */*.[Shc]
