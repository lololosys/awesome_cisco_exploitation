# $Id: depend.alc.make,v 3.1.64.1 1996/05/09 14:34:46 rbadri Exp $
# $Source: /release/112/cvs/Xsys/obj-68-alc/depend.alc.make,v $
#------------------------------------------------------------------
# depend.alc.make -- GNU-Makefile dependencies wrapper for Synergy
#			ATM line card.  Used to generate dependencies
#			given that some ALC code is in the Synergy
#			repository, not IOS.  Adjusts the standard
#			depend.make to work with additional include
#			paths and compile flags.
# (based on makefile for C3000 (aka Pancake, IGS, and so forth)).
#
# August 31, 1995, Rick Lowe
#
# Copyright (c) 1995-1996 by cisco Systems, Inc.
# All rights reserved.
# 
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: depend.alc.make,v $
# Revision 3.1.64.1  1996/05/09  14:34:46  rbadri
# Branch: California_branch
# LANE, UNI3.1 features
#
# Revision 3.1.92.1  1996/04/27  07:16:32  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.1.82.2  1996/04/11  05:55:04  rlowe
# Note this file as being obsolete.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.1.82.1  1996/04/08  01:59:28  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 3.1.70.1  1996/03/22  09:40:57  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.1.54.2  1996/03/20  10:48:37  rlowe
# Integrate with ios/wbu sub-tree and makefiles.
# Also fold old Synergy atmtest.c content into vcdtest.c
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 3.1.54.1  1996/03/03  21:24:19  rlowe
# Apply SYNALC content to merge branch.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 3.1.52.1  1996/03/02  01:20:26  rlowe
# Non-sync sync to mainline tag V111_1_0_3.
# Branch: Synalc3_Cal_V111_1_0_3_branch
#
# Revision 3.1.36.1  1996/02/29  07:49:41  rlowe
# First pass non-sync sync for Synergy ATM line card to V111_0_16.
# Branch: Synalc3_Cal_V111_0_16_branch
#
# Revision 3.1  1995/11/09  12:42:35  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1.32.1  1996/02/03  07:07:57  rlowe
# Non-sync sync of Synalc_catchup_branch to V111_0_5.
# Branch: Synalc_catchup_pretal_branch
#
# Revision 2.1.2.1.2.2.2.1  1996/01/19  21:57:10  sgarcia
# CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
# of Synalc2_
# Branch: Synalc_catchup_branch
# Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
# Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
# ATMFLASH_V11_951214
# into 11.1 branch.
#
# Revision 2.1.2.1.2.3  1995/11/08  02:29:40  rlowe
# Allow SYNALC_ROOT to take its value from user environment.
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.2.1.2.2  1995/09/12  10:31:24  rlowe
# Ensure that variable pointing to Synergy repository is passed
# to recursive makes.
# Cleanup makefile to follow along lines of c3000 makefile.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.2.1.2.1  1995/09/12  01:50:27  rlowe
# Fix for make's "simply expanded" (non-recursive) macros.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.2.1  1995/08/31  14:35:26  rlowe
# Carryover useful code from old Synergy_ATMlc_branch (10.3) to new
# Synalc2_branch (11.0).  Provide wrappers to depend.make for ALC
# having code in both Synergy and IOS repositories.
#
# Branch: Synalc2_branch
#
# Revision 2.1  1995/08/31  14:26:19  rlowe
# Placeholder for Synalc2_branch.
#
#
#------------------------------------------------------------------
# $Endlog$
#

#
#       This sub-makefile is now obsolete.  Cat5k ATM card
#       dependancies are now driven by the usual ios/sys "make depend".
#
