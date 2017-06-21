# $Id: makefile,v 1.1.4.5 1996/06/11 02:36:55 venkat Exp $
# $Source: /release/112/cvs/Xwbu/cat5k/obj-3k-sar/makefile,v $
#------------------------------------------------------------------
# makefile -- for Synergy ATM line card SAR code.
#
# March, 1996, Rick Lowe
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 1.1.4.5  1996/06/11  02:36:55  venkat
# CSCdi60077:  Build tools under wbu independent of the Host OS
# Branch: California_branch
# Built a wrapper function for buginf for the tools abl.c
# csumaout.c. Makefile changes to accomodate the whole tuple
# of uname(1) to figure out the host OS.
#
# Revision 1.1.4.4  1996/06/06  22:53:25  venkat
# CSCdi59790:  PATH to GNU toolset hardcoded in WBU makefiles
# Branch: California_branch
# Defined three sets of (CC, LD, AR etc) defines for host
# (sunos/solaris), cross-compiler for 68k, cross-compiler for mips.
# All these defines are moved to makefile.c5k.alc and the other
# makefiles (cat5k/obj-3k-sar/makefile cat5k/obj-68-alc/makefile
# cat5k/alc_sartools/Makefile) include this file.
#
# Revision 1.1.4.3  1996/06/01  00:04:41  venkat
# CSCdi59228:  Pull compiler usage out from pmcc tool into makefile.
# Branch: California_branch
#
# Revision 1.1.4.2  1996/05/31  01:03:24  venkat
# CSCdi59149:  Tactical fix of ATM image breakages from 96q1 tools.
# Branch: California_branch
# Assembly file and pmcc (cygnus.c) fixes to get builds to work with 96q1
# tool
# chain
#
# Revision 1.1.4.1  1996/05/09  15:01:23  rbadri
# Branch: California_branch
# Synergy features to the mainline
#
# Revision 1.1.40.1  1996/04/27  06:17:22  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 1.1.28.1  1996/04/08  01:42:47  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 1.1.16.1  1996/03/22  09:13:32  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 1.1.2.4  1996/03/21  06:47:32  rlowe
# Position pmon.static.srec as the checked-in S-record file for PMON.
# Change sarpud to sar_pod to be consistent.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1.2.3  1996/03/20  11:19:31  rlowe
# Create pmon.srec from sar_pod.srec until we figure out what to do about
# it.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1.2.2  1996/03/20  04:06:10  vkrishna
# Changes to support SAR firmware build as part of Cat5K ATM Module
# IOS build procedure.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1.2.1  1996/03/13  02:38:27  rlowe
# Add file content from Synergy repository.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 1.1  1996/03/11  07:44:43  rlowe
# Placeholders for California.
#
#
#------------------------------------------------------------------
# $Endlog$
#

include ../../makefile.c5k.alc

# Defines used in building SAR PUD, TXSAR, RXSAR runtime images

COMMONINC=../alc_cominc/
INCLDIR=../src-3k-sar/
LSIINCLDIR=../alc_sarinc/
SRCDIR=../src-3k-sar/
BINDIR=./
TESTDIR=../src-3k-sar/
PMCCFLAGS = -DDEBUG -DPMCC -DCYGNUS -DLANGUAGE_ASSEMBLY 
GNUCFLAGS = -nostdinc -mcpu=r3000 -mips1 -mfp32 -DMIPSEB -EB

CPPFLAGS = -E $(PMCCFLAGS) 
CFLAGS = $(GNUCFLAGS) $(PMCCFLAGS)
CPPFLAGS_NO_PMON = -E $(PMCCFLAGS) -DNO_PMON
CFLAGS_NO_PMON = $(GNUCFLAGS) $(PMCCFLAGS) -DNO_PMON
INCLFLAGS=-I$(COMMONINC) -I$(INCLDIR) -I$(LSIINCLDIR)

LDFLAGS = -N -M -oformat symbolsrec -e _start

RMF= rm -f

# Defines used in building SAR PUD run-time images 

OPTLEV = -g
FTEXT = a0c00000
FDATA = a0c07000
PROM =  -Ttext $(FTEXT) -Tdata $(FDATA)

HDRS=   $(INCLDIR)dma_test.h   $(INCLDIR)errors.h     $(INCLDIR)sar_hw.h

CFILES=

ASFILES=    $(SRCDIR)sar_pod.s

OBJFILE=    $(BINDIR)sar_pod.o


# Defines used in building RXSAR run-time images 

RX_A_SEG = -Ttext a0c1e000 -Tdata a0c1fe00
NO_PMON_SEG = -Ttext a0c00000 -Tdata a0c07000

RX_HDRS=$(INCLDIR)rxsar.h		\
		$(COMMONINC)memmap.h		\
		$(COMMONINC)biga.h			\
		$(COMMONINC)cell.h			\
		$(COMMONINC)basic_types.h	\
		$(COMMONINC)buffer_if.h		\
		$(COMMONINC)cam_if.h		\
		$(COMMONINC)ivcimap.h		\
		$(COMMONINC)ovcimap.h		\
		$(COMMONINC)queue_if.h		\
		$(COMMONINC)sar_if.h		\
		$(COMMONINC)shram.h			\
		$(COMMONINC)param.h			\
		$(COMMONINC)spram.h


ASFILES=	$(SRCDIR)rxsar.s  $(SRCDIR)rxoffdiag.s  

OBJFILE=	$(BINDIR)rxsar.o		\
			$(BINDIR)rxsar_no_pmon.o \
			$(BINDIR)rxoffdiag.o

# Defines used in building TXSAR run-time images 

DRVLIB=../../acp/drivers/atmdriver.a
H_SEG = -Ttext a0c1e000 -Tdata a0c1f800 
TX_A_SEG = -Ttext a0c1e000 -Tdata a0c1f800
SAR_SEG= -Ttext a0c00000 -Tdata a0c07000

TX_HDRS=$(INCLDIR)txsar.h			\
		$(COMMONINC)cell.h			\
		$(COMMONINC)basic_types.h	\
		$(COMMONINC)buffer_if.h		\
		$(COMMONINC)cam_if.h		\
		$(COMMONINC)ivcimap.h		\
		$(COMMONINC)ovcimap.h		\
		$(COMMONINC)queue_if.h		\
		$(COMMONINC)sar_if.h		\
		$(COMMONINC)shram.h			\
		$(COMMONINC)spram.h


ASFILES=	$(SRCDIR)tx.s $(TESTDIR)test.s 	$(SRCDIR)txoffdiag.s 

OBJFILE=	$(BINDIR)txinit.o		\
			$(BINDIR)tx.o			\
			$(BINDIR)txnopmon.o		\
			$(BINDIR)txoffdiag.o

TESTFILES=	$(TESTDIR)txtest.c 		\
			$(TESTDIR)enqdeq.c 		\
			$(TESTDIR)vcdtest.c		\
			$(TESTDIR)datatest.c


TESTOBJS=	$(BINDIR)txtest.o 	\
			$(BINDIR)test.o 	\
			$(BINDIR)enqdeq.o 	\
			$(BINDIR)vcdtest.o	\
			$(BINDIR)datatest.o

SARBINS=pmon.static.bin
SARTOOLS= ../alc_sartools
SARBINS2=txnopmon.bin rxsar_no_pmon.bin txoffdiag.bin rxoffdiag.bin sar_pod.bin
SARSRECS=txnopmon.srec rxsar_no_pmon.srec txoffdiag.srec rxoffdiag.srec \
		 sar_pod.srec
SARSYMSRECS=$(SARSRECS:.srec=.symsrec)

all:	$(SARTOOLS) $(BINDIR)txnopmon.bin $(BINDIR)rxsar_no_pmon.bin \
		$(BINDIR)txoffdiag.bin $(BINDIR)rxoffdiag.bin $(BINDIR)sar_pod.bin \
		$(SARBINS)

clean:	
		$(RMF) *.map *.o *.bin $(SARSRECS) $(SARSYMSRECS)

FRC:
	
#
#   Build pmcc and other MIPS tools for making SAR. Always force the make
#   so that there is no problem even if we alternate the build between
#   SUNOS and Solaris machines - Venkat 03/11
#

$(SARTOOLS): FRC            # For TX and RX SARs
	cd $@ ; $(MAKE) all

#
#   Post-process any SAR s-record images into flat binary.
#   The SAR runtime codes are built by $(SARBINS2), but PMON is checked in 
#	as an s-record (until we get good makefile for it).
#

#
#   For now, hack the PMON file as a duplicate of the powerup diags file.
#

$(SARBINS) : %.bin : %.srec
	-rm -f $(@F).t $@
	sed -e "/^S./!d" $< > $(@F).t
	$(OC_MIPS) -I srec -O binary -S -v --gap-fill=0 $(@F).t $@
	-rm -f $(@F).t

$(BINDIR)sar_pod.bin: $(BINDIR)sar_pod
	-rm -f $(@F).t $@
	sed -e "/^S4/d" $<.srec >$(@F).t
	$(OC_MIPS) -I srec -O binary -S -v --gap-fill=0 $(@F).t $@
	-rm -f $(@F).t

$(BINDIR)txnopmon.bin: $(BINDIR)txnopmon
	-rm -f $(@F).t $@
	sed -e "/^S4/d" $<.srec >$(@F).t
	$(OC_MIPS) -I srec -O binary -S -v --gap-fill=0 $(@F).t $@
	-rm -f $(@F).t

$(BINDIR)rxsar_no_pmon.bin: $(BINDIR)rxsar_no_pmon
	-rm -f $(@F).t $@
	sed -e "/^S4/d" $<.srec >$(@F).t
	$(OC_MIPS) -I srec -O binary -S -v --gap-fill=0 $(@F).t $@
	-rm -f $(@F).t

$(BINDIR)txoffdiag.bin: $(BINDIR)txoffdiag
	-rm -f $(@F).t $@
	sed -e "/^S4/d" $<.srec >$(@F).t
	$(OC_MIPS) -I srec -O binary -S -v --gap-fill=0 $(@F).t $@
	-rm -f $(@F).t

$(BINDIR)rxoffdiag.bin: $(BINDIR)rxoffdiag
	-rm -f $(@F).t $@
	sed -e "/^S4/d" $<.srec >$(@F).t
	$(OC_MIPS) -I srec -O binary -S -v --gap-fill=0 $(@F).t $@
	-rm -f $(@F).t

#
# Make rules relating to SAR PUD
#

$(BINDIR)sar_pod: $(BINDIR)sar_pod.o $(HDRS)
	$(LD_MIPS) $(LDFLAGS) $(PROM) -o $(BINDIR)sar_pod.symsrec \
	$(BINDIR)sar_pod.o > $(BINDIR)sar_pod.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)sar_pod.symsrec > $(BINDIR)sar_pod.srec

$(BINDIR)sar_pod.o:   $(SRCDIR)sar_pod.s $(HDRS)
	$(RMF) $(BINDIR)sar_pod.t2 $(BINDIR)sar_pod.S
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)sar_pod.s >$(BINDIR)sar_pod.S
	$(CC_MIPS) $(CPPFLAGS_NO_PMON) $(INCLFLAGS) -o $(BINDIR)sar_pod.t2 $(BINDIR)sar_pod.S 
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)sar_pod.t2 > $(BINDIR)sar_pod.S
	$(CC_MIPS) $(CFLAGS_NO_PMON) -c $(OPTLEV) $(INCLFLAGS) -o $@ $(BINDIR)sar_pod.S
	$(RMF) $(BINDIR)sar_pod.t2 $(BINDIR)sar_pod.S

#
# Make rules relating to RXSAR 
#

$(BINDIR)rxsar: $(BINDIR)rxsar.o $(RX_HDRS)
	$(LD_MIPS) $(LDFLAGS) $(RX_A_SEG) -o $(BINDIR)rxsar.symsrec \
	$(BINDIR)rxsar.o  > $(BINDIR)rxsar.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)rxsar.symsrec > $(BINDIR)rxsar.srec

$(BINDIR)rxsar.o:	$(SRCDIR)rxsar.s $(RX_HDRS)
	$(RMF) $(BINDIR)rxsar.t2 $(BINDIR)rxsar.S
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)rxsar.s > $(BINDIR)rxsar.S
	$(CC_MIPS) $(CPPFLAGS) $(INCLFLAGS) -o $(BINDIR)rxsar.t2 $(BINDIR)rxsar.S
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)rxsar.t2 > $(BINDIR)rxsar.S
	$(CC_MIPS) $(CFLAGS) -c $(INCLFLAGS) -o $@ $(BINDIR)rxsar.S
	$(RMF) $(BINDIR)rxsar.t2 $(BINDIR)rxsar.S

$(BINDIR)rxsar_no_pmon: $(BINDIR)rxsar_no_pmon.o $(RX_HDRS)
	$(LD_MIPS) $(LDFLAGS) $(NO_PMON_SEG) -o $(BINDIR)rxsar_no_pmon.symsrec $(BINDIR)rxsar_no_pmon.o > $(BINDIR)rxsar_no_pmon.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)rxsar_no_pmon.symsrec > $(BINDIR)rxsar_no_pmon.srec
 
$(BINDIR)rxsar_no_pmon.o:   $(SRCDIR)rxsar.s $(RX_HDRS)
	$(RMF) $(BINDIR)rxsar_no_pmon.t2 $(BINDIR)rxsar_no_pmon.S
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)rxsar.s > $(BINDIR)rxsar_no_pmon.S
	$(CC_MIPS) $(CPPFLAGS_NO_PMON) $(INCLFLAGS) -o $(BINDIR)rxsar_no_pmon.t2 $(BINDIR)rxsar_no_pmon.S
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)rxsar_no_pmon.t2 > $(BINDIR)rxsar_no_pmon.S
	$(CC_MIPS) $(CFLAGS_NO_PMON) -c $(INCLFLAGS) -o $@ $(BINDIR)rxsar_no_pmon.S
	$(RMF) $(BINDIR)rxsar_no_pmon.t2 $(BINDIR)rxsar_no_pmon.S


$(BINDIR)rxoffdiag: $(BINDIR)rxoffdiag.o $(RX_HDRS)
	$(LD_MIPS) $(LDFLAGS) $(NO_PMON_SEG) -o $(BINDIR)rxoffdiag.symsrec $(BINDIR)rxoffdiag.o > $(BINDIR)rxoffdiag.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)rxoffdiag.symsrec > $(BINDIR)rxoffdiag.srec
 
$(BINDIR)rxoffdiag.o:   $(SRCDIR)rxoffdiag.s $(RX_HDRS)
	$(RMF) $(BINDIR)rxoffdiag.t2 $(BINDIR)rxoffdiag.S
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)rxoffdiag.s > $(BINDIR)rxoffdiag.S
	$(CC_MIPS) $(CPPFLAGS_NO_PMON) $(INCLFLAGS) -o $(BINDIR)rxoffdiag.t2 $(BINDIR)rxoffdiag.S
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)rxoffdiag.t2 > $(BINDIR)rxoffdiag.S
	$(CC_MIPS) $(CFLAGS_NO_PMON) -c $(INCLFLAGS) -o $@ $(BINDIR)rxoffdiag.S
	$(RMF) $(BINDIR)rxoffdiag.t2 $(BINDIR)rxoffdiag.S


#
# Make rules relating to TXSAR
#

nopmon:	$(BINDIR)txnopmon 

$(BINDIR)txsar: $(BINDIR)tx.o $(TX_HDRS)
	$(LD_MIPS) $(LDFLAGS) $(TX_A_SEG) -o $(BINDIR)txsar.symsrec  \
	$(BINDIR)tx.o >  $(BINDIR)txsar.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)txsar.symsrec > $(BINDIR)txsar.srec

$(BINDIR)txnopmon: $(BINDIR)txnopmon.o $(TX_HDRS)
	$(LD_MIPS) $(LDFLAGS) $(SAR_SEG) -o $(BINDIR)txnopmon.symsrec \
	$(BINDIR)txnopmon.o > $(BINDIR)txnopmon.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)txnopmon.symsrec > $(BINDIR)txnopmon.srec

$(BINDIR)txhost: $(TESTOBJS) $(DRVLIB) $(TX_HDRS)
	$(LD_MIPS) $(LDFLAGS) $(H_SEG) -o $(BINDIR)txhost.symsrec $(TESTOBJS) \
	$(DRVLIB) > $(BINDIR)txhost.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)txhost.symsrec > $(BINDIR)txhost.srec

$(BINDIR)txinit.o:	$(SRCDIR)txinit.c $(TX_HDRS)
	$(RMF) $(BINDIR)txinit.t2 $(BINDIR)txinit.C
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)txinit.c > $(BINDIR)txinit.C
	$(CC_MIPS) $(CPPFLAGS) $(INCLFLAGS) -o $(BINDIR)txinit.t2 $(BINDIR)txinit.C
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)txinit.t2 > $(BINDIR)txinit.C
	$(CC_MIPS) $(CFLAGS) -c $(INCLFLAGS) -o $@ $(BINDIR)txinit.C
	$(RMF) $(BINDIR)txinit.t2 $(BINDIR)txinit.C

$(BINDIR)tx.o:	$(SRCDIR)tx.s $(TX_HDRS)
	$(RMF) $(BINDIR)tx.t2 $(BINDIR)tx.S
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)tx.s > $(BINDIR)tx.S
	$(CC_MIPS) $(CPPFLAGS) $(INCLFLAGS) -o $(BINDIR)tx.t2 $(BINDIR)tx.S
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)tx.t2 > $(BINDIR)tx.S
	$(CC_MIPS) $(CFLAGS) -c $(INCLFLAGS) -o $@ $(BINDIR)tx.S
	$(RMF) $(BINDIR)tx.t2 $(BINDIR)tx.S

$(BINDIR)txnopmon.o:	$(SRCDIR)tx.s $(TX_HDRS)
	$(RMF) $(BINDIR)txnopmon.t2 $(BINDIR)txnopmon.S
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)tx.s > $(BINDIR)txnopmon.S
	$(CC_MIPS) $(CPPFLAGS_NO_PMON) $(INCLFLAGS) -o $(BINDIR)txnopmon.t2 $(BINDIR)txnopmon.S
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)txnopmon.t2 > $(BINDIR)txnopmon.S
	$(CC_MIPS) $(CFLAGS_NO_PMON) -c $(INCLFLAGS) -o $@ $(BINDIR)txnopmon.S
	$(RMF) $(BINDIR)txnopmon.t2 $(BINDIR)txnopmon.S

$(BINDIR)txoffdiag: $(BINDIR)txoffdiag.o $(TX_HDRS)
	$(LD_MIPS) $(LDFLAGS) $(SAR_SEG) -o $(BINDIR)txoffdiag.symsrec \
	$(BINDIR)txoffdiag.o > $(BINDIR)txoffdiag.symsrec.map
	../alc_sartools/$(SYM2S4REC) $(BINDIR)txoffdiag.symsrec > $(BINDIR)txoffdiag.srec

$(BINDIR)txoffdiag.o:	$(SRCDIR)txoffdiag.s $(TX_HDRS)
	$(RMF) $(BINDIR)txoffdiag.t2 $(BINDIR)txoffdiag.S
	../alc_sartools/$(SAR_FILTER) -h $(SRCDIR)txoffdiag.s > $(BINDIR)txoffdiag.S
	$(CC_MIPS) $(CPPFLAGS_NO_PMON) $(INCLFLAGS) -o $(BINDIR)txoffdiag.t2 $(BINDIR)txoffdiag.S
	../alc_sartools/$(SAR_FILTER) -s $(BINDIR)txoffdiag.t2 > $(BINDIR)txoffdiag.S
	$(CC_MIPS) $(CFLAGS_NO_PMON) -c $(INCLFLAGS) -o $@ $(BINDIR)txoffdiag.S
	$(RMF) $(BINDIR)txoffdiag.t2 $(BINDIR)txoffdiag.S

