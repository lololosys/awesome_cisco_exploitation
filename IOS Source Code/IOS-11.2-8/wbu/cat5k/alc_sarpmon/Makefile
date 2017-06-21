# $Id: Makefile,v 1.1.4.1 1996/05/09 15:00:14 rbadri Exp $
# $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/Makefile,v $
#------------------------------------------------------------------
# Makefile -- for LSI Logic ATMizer PMON.
#
# 
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: Makefile,v $
# Revision 1.1.4.1  1996/05/09  15:00:14  rbadri
# Branch: California_branch
# Synergy features to the mainline
#
# Revision 1.1.40.1  1996/04/27  06:16:11  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.28.1  1996/04/08  01:41:24  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 1.1.16.1  1996/03/22  09:12:20  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.2.1  1996/03/13  02:37:55  rlowe
# Add file content from Synergy repository.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1  1996/03/11  07:43:51  rlowe
# Placeholders for California.
#
#
#------------------------------------------------------------------
# $Endlog$
#

include $(LSIPKG)/defines.mk
include files.mk
SHELL = /bin/sh

all: date.c vers.c tools.c rules.mk $(PMON_TARG)
	@echo "pmon is now up to date"

clean: 
	@$(MAKE) clean_b "CMD=clean"
	@$(RMF) rules.mk

tape:
	@mkreadme README.sh $(REV) > README
	@mklst pmon $(CFILES) $(ASFILES) $(OTHERS) >> ../tape.lst

clean_b: $(PMON_TARG)

BG:
	@echo "###### pmon/bg"
#@cd bg;$(MAKE) -f ../target.mk $(CMD) ENDIAN=bg TFLAG="-EB -G 0"
	@cd bg;$(MAKE) -f ../target.mk $(CMD) ENDIAN=bg TFLAG="-EB "

LG:
	@echo "###### pmon/lg"
#@cd lg;$(MAKE) -f ../target.mk $(CMD) ENDIAN=lg TFLAG="-EL -G 0"
	@cd lg;$(MAKE) -f ../target.mk $(CMD) ENDIAN=lg TFLAG="-EL "

# make a new date file if anthing has been changed
# this should also detect changes in either library, but doesn't
date.c : $(CFILES) $(ASFILES)
	mkmsg date -date > date.c

vers.c : ../defines.mk
	mkmsg vers $(REV) > vers.c

tools.c : ../defines.mk
	mkmsg tools TOOLS=$(TOOLSET) > tools.c

rules.mk : files.mk
	@$(RMF) rules.mk
	mklist -r -o rules.mk files.mk .. ASFILES CFILES date.c vers.c tools.c devinfo.c

