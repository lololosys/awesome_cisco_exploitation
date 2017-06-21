# $Id: Makefile,v 1.1.4.5 1996/06/11 02:36:53 venkat Exp $
# $Source: /release/112/cvs/Xwbu/cat5k/alc_sartools/Makefile,v $
#------------------------------------------------------------------
# Makefile -- for LSI Logic ATMizer tools.
#
# 
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: Makefile,v $
# Revision 1.1.4.5  1996/06/11  02:36:53  venkat
# CSCdi60077:  Build tools under wbu independent of the Host OS
# Branch: California_branch
# Built a wrapper function for buginf for the tools abl.c
# csumaout.c. Makefile changes to accomodate the whole tuple
# of uname(1) to figure out the host OS.
#
# Revision 1.1.4.4  1996/06/06  22:53:23  venkat
# CSCdi59790:  PATH to GNU toolset hardcoded in WBU makefiles
# Branch: California_branch
# Defined three sets of (CC, LD, AR etc) defines for host
# (sunos/solaris), cross-compiler for 68k, cross-compiler for mips.
# All these defines are moved to makefile.c5k.alc and the other
# makefiles (cat5k/obj-3k-sar/makefile cat5k/obj-68-alc/makefile
# cat5k/alc_sartools/Makefile) include this file.
#
# Revision 1.1.4.3  1996/06/01  00:04:50  venkat
# CSCdi59228:  Pull compiler usage out from pmcc tool into makefile.
# Branch: California_branch
#
# Revision 1.1.4.2  1996/05/31  01:03:21  venkat
# CSCdi59149:  Tactical fix of ATM image breakages from 96q1 tools.
# Branch: California_branch
# Assembly file and pmcc (cygnus.c) fixes to get builds to work with 96q1
# tool
# chain
#
# Revision 1.1.4.1  1996/05/09  15:00:38  rbadri
# Branch: California_branch
# Synergy features to the mainline
#
# Revision 1.1.40.1  1996/04/27  06:16:35  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.28.1  1996/04/08  01:41:53  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 1.1.16.1  1996/03/22  09:12:47  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.2.2  1996/03/20  04:06:05  vkrishna
# Changes to support SAR firmware build as part of Cat5K ATM Module
# IOS build procedure.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1.2.1  1996/03/13  02:38:21  rlowe
# Add file content from Synergy repository.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1  1996/03/11  07:44:28  rlowe
# Placeholders for California.
#
#
#------------------------------------------------------------------
# $Endlog$
#


include ../../makefile.c5k.alc
include ./defines.mk


CFILES = rdfrec.c rdsrec.c gens4rec.c sym2s4rec.c prfp.c rm-f.c \
	 mklst.c mkmsg.c mkreadme.c mkfiles.c mklist.c gensrec.c \
	 genfrec.c gencoff.c edown.c

OTHERS = Makefile mipscoff.h mips.c epi.c bso.c promize.c ghs.c man.c \
	 cygnus.c algo.c

CFLAGS = -DLANGUAGE_C $(PKGSW) $(LSIPMCC)
OBJS = $(CFILES:.c=) $(SPCLTOOLS) pmcc$(EXE) sar_filter-* sym2s4rec-*

all : $(SAR_FILTER) $(SYM2S4REC)

.c.o$O:
	$(CC_HOST) -c $(CFLAGS) $*.c

tools : $(OBJS)
	@echo "tools are now up to date"

clean : 
	@$(RMF) $(OBJS)

tape:
	@mklst tools $(CFILES) $(OTHERS) >> ../tape.lst

pmcc$(EXE) : $(PMCC) ../alc_sarinc/defines.h
	$(CC_HOST) $(CFLAGS) -DPMCC_DRIVER -I../alc_sarinc -o pmcc$(EXE) $(PMCC)

$(SYM2S4REC) : sym2s4rec.c 
	$(CC_HOST) $(CFLAGS) -o $@ $^

$(SAR_FILTER) : $(PMCC) ../alc_sarinc/defines.h
	$(CC_HOST) $(CFLAGS) -I../alc_sarinc -o $@ $(PMCC)

rdsrec$(EXE): rdsrec.o$O machine.o$O
	$(CC_HOST) $(CFLAGS) -o rdsrec$(EXE) rdsrec.o$O machine.o$O

rdfrec$(EXE): rdfrec.o$O machine.o$O
	$(CC_HOST) $(CFLAGS) -o rdfrec$(EXE) rdfrec.o$O machine.o$O

machine.o$O: ../alc_sarpmon/machine.c
	$(CC_HOST) -c $(CFLAGS) -DMIPSEB -I. -I../alc_sarinc  \
	../alc_sarpmon/machine.c

