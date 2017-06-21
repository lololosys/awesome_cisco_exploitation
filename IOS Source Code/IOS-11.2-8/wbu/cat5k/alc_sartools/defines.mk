# $Id: defines.mk,v 1.1.4.1 1996/05/09 15:00:40 rbadri Exp $
# $Source: /release/112/cvs/Xwbu/cat5k/alc_sartools/defines.mk,v $
#------------------------------------------------------------------
# defines.mk -- definitions for ATMizer makefiles.
#
# 
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: defines.mk,v $
# Revision 1.1.4.1  1996/05/09  15:00:40  rbadri
# Branch: California_branch
# Synergy features to the mainline
#
# Revision 1.1.40.1  1996/04/27  06:16:37  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.28.2  1996/04/26  02:17:50  venkat
# Branch: LE_Cal_V112_0_2_branch
# Fixed Warnings while compiling the sartools (pmcc)
#
# Revision 1.1.28.1  1996/04/08  01:41:56  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 1.1.16.1  1996/03/22  09:12:49  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.2.1  1996/03/13  02:38:23  rlowe
# Add file content from Synergy repository.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1  1996/03/11  07:44:30  rlowe
# Placeholders for California.
#
#
#------------------------------------------------------------------
# $Endlog$
#

######################################################################
#     This file contains information about your configuration.       #
######################################################################

CFLAGS = $(OPTLEV) $(TFLAG)
ASFLAGS = $(OPTLEV) $(TFLAG)
ARFLAGS = -rcv

.SUFFIXES: .o .s
BUILD_DIRS = TOOLS LIB PMON
CLEAN_DIRS =  LIB PMON
PMON_TARG =  BG
LIB_TARG =  BG BO
FPEM_DIR = lib
REV = 4.1_Beta
FTEXT = a0c00000
FDATA = a0d00000
RMF = rm -f
# tools type CYGSUN
TOOLSET = CYGSUN
#AR = mips-idt-ecoff-ar cr	changed Ratinder P. S. Ahuja
AR = mips64-elf-ar $(ARFLAGS)
PMCC = cygnus.c
SPCLTOOLS = edown
OPTLEV = -O -g 
NMOD = x
# fp support via cp1

