# $Id: Makefile,v 1.10 1996/11/18 21:45:17 jchlin Exp $
# $Source: /cscka/t1_neat/src/t1/Makefile,v $
#------------------------------------------------------------------
# Makefile for T1 Neat
#
# October, 1995, Joe Liu
#
# Copyright (c) 1995-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: Makefile,v $
# Revision 1.10  1996/11/18 21:45:17  jchlin
# o       Change a common filename from ct1_rbs.c to cas_rbs.c
#
# Revision 1.9  1996/10/23 23:29:32  jchlin
# o       Move the dependancy file include from Makefile to make.rule.
#
# Revision 1.8  1996/10/23  03:38:39  jchlin
# o       Restructure the respository so that T1 and E1 codes can share
#         common code.
# o       Restructure the makefile so that it adapts new tree structures
#         and also rewrite the makefile for automatically generating the
#         dependancies. The old one doesn't work properly.
#
# Revision 1.7  1996/10/01  06:12:29  jchlin
# Make srec2c when it's not uptodate.
#
# Revision 1.6  1996/09/08  02:16:41  jchlin
# o       Make as5200_t1_words.c depending on neat.ram.srec and srec2c.
#
# Revision 1.5  1996/05/09  21:25:47  towu
# Added new feature Robbed bit signals.
#
# Revision 1.4  1996/04/11  01:16:29  jliu
#     o add -mnobitfield option for 68360 processor compiler, so it won't
#       create assembly instruction like bfextu since 68360 treat it like
#       illegal instruction
#
# Revision 1.3  1996/04/04  01:05:31  towu
# o Removed proto_auto.h.
# o Modified Makefile to build as5200_t1_words.c
#
# Revision 1.2  1996/03/02  03:51:15  towu
# The basecode to support both mailbox and message queue for ipc.
#
# Revision 1.1  1996/02/14  02:18:03  towu
# Merge bin to src
#
# Revision 1.1.1.1  1996/02/12  18:59:19  towu
# Created
#
#------------------------------------------------------------------
# $Endlog$
#
#####################################################################
# T1 specifics
#
SHELL=/bin/sh

COMMON_DIR      := ../common
SRC_DIR         := ../t1
OBJ_DIR         := ../../as/t1
SYS_FIRMWARE    := ../../../firmware
SRC_FILES       := t1_c68k.S t1_init.c t1_mib.c t1_framer.c t1_rcv.c \
                   t1_interrupt.c t1_message.c t1_fdl.c t1_main.c 
COMMON_SRC      := as_memory.c as_utils.c as_neat_ipc.c as_cas_rbs.c as_timer.c\
                   as_list.c as_robbed_bit.c

FIRMWARE        = $(SYS_FIRMWARE)/as5200_t1_firmware.c

NEAT		= $(SRC_DIR)/t1_neat
ANEAT		= $(SRC_DIR)/t1_neat_aout
VERSION		= 1
REVISION	= 0
PRODUCTION	= 

BOARDTYPE	= -DT1_FIRMWARE
SREC_TO_C	= t1_mpu2neat_words.c

#####################################################################
# include common files
#
include $(COMMON_DIR)/make.68k
include $(COMMON_DIR)/make.rule
