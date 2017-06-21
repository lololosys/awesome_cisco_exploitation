# $Id: files.mk,v 1.1.4.1 1996/05/09 15:00:20 rbadri Exp $
# $Source: /release/112/cvs/Xwbu/cat5k/alc_sarpmon/files.mk,v $
#------------------------------------------------------------------
# files.mk -- file list sub-makefile for LSI Logic ATMizer PMON.
#
# 
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: files.mk,v $
# Revision 1.1.4.1  1996/05/09  15:00:20  rbadri
# Branch: California_branch
# Synergy features to the mainline
#
# Revision 1.1.40.1  1996/04/27  06:16:17  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.28.1  1996/04/08  01:41:32  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 1.1.16.1  1996/03/22  09:12:27  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.2.1  1996/03/13  02:38:02  rlowe
# Add file content from Synergy repository.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1  1996/03/11  07:43:58  rlowe
# Placeholders for California.
#
#
#------------------------------------------------------------------
# $Endlog$
#

####################################################################
#     New modules should be added the appropriate section below    #
####################################################################

CFILES = main.c commands.c dis.c hist.c machine.c regs.c sym.c \
	 set.c stty.c more.c memtst.c load.c go.c debug.c \
	 regdefs.c cmdtable.c sbrk.c demo.c

ASFILES = mips.s 

OTHERS = Makefile README files.mk target.mk set.h pmon.h vers.c
