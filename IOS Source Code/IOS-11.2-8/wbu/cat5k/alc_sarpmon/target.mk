# $Id: target.mk,v 1.1.4.1 1996/05/09 15:00:35 rbadri Exp $
# $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/target.mk,v $
#------------------------------------------------------------------
# target.mk -- target sub-makefile for LSI Logic ATMizer PMON.
#
# 
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: target.mk,v $
# Revision 1.1.4.1  1996/05/09  15:00:35  rbadri
# Branch: California_branch
# Synergy features to the mainline
#
# Revision 1.1.40.1  1996/04/27  06:16:32  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.28.1  1996/04/08  01:41:49  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 1.1.16.1  1996/03/22  09:12:44  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.2.1  1996/03/13  02:38:18  rlowe
# Add file content from Synergy repository.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1  1996/03/11  07:44:13  rlowe
# Placeholders for California.
#
#
#------------------------------------------------------------------
# $Endlog$
#

include $(LSIPKG)/defines.mk
include ../files.mk

OBJS = $(ASFILES:.s=.o) $(CFILES:.c=.o) date.o vers.o tools.o devinfo.o
PROM = -prom -crt0 -syms -T $(FTEXT) -D $(FDATA)
FLIB = -L$(LSIPKG)/$(FPEM_DIR)/$(ENDIAN)

all : pmon.srec
	@echo pmon/$(ENDIAN) is now up to date

pmon.srec: ../rules.mk $(OBJS) $(LSIPKG)/lib/$(ENDIAN)/lib$(NMOD)c.a $(FPEM_DEP)
	pmcc $(PROM) $(OPTLEV) $(TFLAG) $(FLIB) -o pmon.srec $(OBJS) $(LIBF)

clean:
	@$(RMF) $(OBJS) pmon pmon.rec pmon.map

#$(LSIPKG)/lib/$(ENDIAN)/lib$(NMOD)c.a:
#	cd $(LSIPKG)/lib;make

main.o mips.o regs.o set.o regdefs.o : $(LSIPKG)/defines.mk
main.o mips.o regs.o set.o regdefs.o load.o devinfo.o : $(LSIPKG)/include/defines.h

cmdtable.o commands.o debug.o demo.o devinfo.o dis.o go.o hist.o \
load.o machine.o main.o memtst.o more.o regdefs.o regs.o sbrk.o \
set.o stty.o sym.o mips.o : ../pmon.h

include ../rules.mk

