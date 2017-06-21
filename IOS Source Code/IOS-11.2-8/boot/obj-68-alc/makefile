#
# $Id: makefile,v 3.1.58.3 1996/06/04 05:20:28 rlowe Exp $
# $Source: /release/112/cvs/Xboot/obj-68-alc/makefile,v $
#------------------------------------------------------------------
# Makefile for Synergy ATM Line Card (ALC) IOS bootstrap software
# (based on Makefile for Pancake (68020) bootstrap software)
#
# April 13, 1995, Rick Lowe
#
# Copyright (c) 1995-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.1.58.3  1996/06/04  05:20:28  rlowe
# Fixed wbu/cat5k/obj-68-alc and boot/obj-68-alc makefiles.
# CSCdi59410:  Cant drive C5K ATM card ios boot build without preexisting
# WBU libs
# Branch: California_branch
#
# Revision 3.1.58.2  1996/05/27  21:18:16  mdb
# CSCdi58771:  use -Wa,-R for 96q1 instead of -R on gcc command line
# Branch: California_branch
#
# Revision 3.1.58.1  1996/05/09  13:54:45  rbadri
# Branch: California_branch
# Synergy features to the mainline.
#
# Revision 3.1.88.2  1996/05/01  05:58:25  rlowe
# Add obj-68-alc to list of boot object directories.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.1.88.1  1996/04/27  06:13:11  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.1.78.1  1996/04/08  01:34:41  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 3.1.64.2  1996/04/02  04:43:45  rlowe
# Add default target.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.1.64.1  1996/03/22  09:07:08  rlowe
# Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.1.50.2  1996/03/20  10:41:34  rlowe
# Integrate with ios/wbu sub-tree and makefiles.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 3.1.50.1  1996/03/03  21:19:07  rlowe
# Apply SYNALC content to merge branch.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 3.1.48.1  1996/03/02  01:07:04  rlowe
# Non-sync sync to mainline tag V111_1_0_3.
# Branch: Synalc3_Cal_V111_1_0_3_branch
#
# Revision 3.1.32.1  1996/02/29  07:42:05  rlowe
# First pass non-sync sync for Synergy ATM line card to V111_0_16.
# Branch: Synalc3_Cal_V111_0_16_branch
#
# Revision 3.1  1995/11/09  09:07:57  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1.62.1  1996/02/03  06:11:46  rlowe
# Non-sync sync of Synalc_catchup_branch to V111_0_5.
# Branch: Synalc_catchup_pretal_branch
#
# Revision 2.1.44.4  1995/10/23  23:22:53  rlowe
# Add -DGCC68K to allow for PAcKed attribute in SCP messages with NMP.
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.3  1995/10/13  05:24:16  rlowe
# Enable and use BCS checksums in PROM Boot.
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.2  1995/10/03  07:04:47  rlowe
# Re-port IOS Boot for Synergy ATM line card from 10.3 to 11.0.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.1  1995/09/11  01:39:42  rlowe
# Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
# yielding Synalc2_LE2_V110_1_0_3_branch.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.28.2  1995/09/02  00:17:56  rlowe
# Have makefile provide search paths for include files, instead of
# hardcoding them.
#
# Branch: Synalc2_branch
#
# Revision 2.1.28.1  1995/08/22  21:23:37  lcheung
# move files from Synergy_atmlc_branch to Synalc2_branch
# Branch: Synalc2_branch
#
# Revision 1.1.2.8  1995/07/27  08:44:18  rlowe
# Change DATA section base address to new values for Rev-2 ALC HW.
# (See atm_repository/acp/include/acp_image_map.h for high-ram carving.)
# Change linker output format from symbolsrec to a.out, for use with
# ALC combined image tools.
#
# Branch: Synergy_ATMlc_branch
#
# Revision 1.1.2.7  1995/06/15  09:36:55  rlowe
# Move boot download functionality to boot-1.  Condition PMON to SARs
# on MFG jumper instead of DBG jumper.  Condition boot download on
# SPARE jumper until checksums are done.  Cleanup makefiles.
# Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
# Explicitly hold/release reset on BIGA.  Force LCP reset whenever
# LCP driver re-initialized.
#
# Branch: Synergy_ATMlc_branch
#
# Revision 1.1.2.6  1995/06/13  06:19:28  rlowe
# Fix makefile bug where a stale init.o was being linked in.
# Branch: Synergy_ATMlc_branch
#
# Revision 1.1.2.5  1995/06/13  04:52:28  rlowe
# Changes to get BIGA driver running in the Boots for download use.
# Branch: Synergy_ATMlc_branch
#
# Revision 2.1  1995/06/07  19:14:05  hampton
# Bump version numbers from 1.x to 2.x.
#
# Revision 1.1.2.4  1995/06/09  09:20:16  rlowe
# Port and integrate NMP (CBI) flash driver (Intel, AMD flash simms) to
# ACP.
# Branch: Synergy_ATMlc_branch
#
# Revision 1.1.2.3  1995/06/02  00:40:18  rlowe
# Add in-band IPC, download, and burn-to-flash code.
# Branch: Synergy_ATMlc_branch
#
# Revision 1.1.2.2  1995/05/17  00:27:20  rlowe
# Support downloading to Rev 2 ATM line cards.  Move downloaded code base
# from $40001000 to $40004000.
# Branch: Synergy_ATMlc_branch
#
# Revision 1.1.2.1  1995/04/19  02:31:56  rlowe
# Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
#
# Branch: Synergy_ATMlc_branch
#
# Revision 1.1  1995/04/14 05:16:02  rlowe
# Placeholders for IOS port to Synergy ATM line card.
#
#------------------------------------------------------------------
# Revision 1.1.1.1  1995/02/21  19:33:43  kchristi
# creating new ios directory
#
# Revision 1.4  1995/02/18  12:05:36  rlowe
# Have IOS IOMEM be in ACP DRAM area, between processor ram chunk and
# NTC DMA area.  Keep ACP Frame Buffer area hidden from IOS.
#
# Revision 1.3  1995/02/15  02:46:42  rlowe
# BCS is now in services/bcs, not services/boot2.
#
# Revision 1.2  1995/02/01  12:02:48  rlowe
# Changes to get ACP IOS-Boot up.  Hack makefile to get to some ACP includes
# and libraries (not portable to general users, though).
#
# Revision 1.1  1995/02/01  10:59:53  rlowe
# Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
#
# Revision 1.4  1995/01/27  03:15:01  rlowe
# Add Boot Code Service (BCS) library.
#
# Revision 1.3  1995/01/26  04:56:30  rlowe
# Since IOS Boot now is burned into flash, do not rcp to tftp server.
#
# Revision 1.2  1995/01/26  04:53:04  rlowe
# Change TEXT and DATA section addresses to match new ACP (flash) image map.
#
# Revision 1.1  1995/01/11  22:06:19  rlowe
# Initial port of IOS Boot to ACP.  Got Monitor up.
#
# Revision 1.1.1.1  1994/08/31  22:16:33  rahuja
# IPS ver 10.2 snapshot
#
#------------------------------------------------------------------
# $Endlog$
#

# Crash us if /bin/make is used:
ifeq ($(origin USER),undefined)
USER=Bozo
endif

ifeq ($(origin QUIET), undefined)
QUIET_COM=@\#
QUIET_CC=
else
QUIET_COM=@
QUIET_CC=@
endif

default all : boot

include ../Makefile.68k

#
#	Identify if this make is being driven by the ios/wbu/cat5k/obj-68-alc
#	makefile, or not.  If so, do NOT dispatch submakes back into that
#	place !!
#

ifeq ($(strip $(WBU_DRIVE)),)
ALC_MKWBU_DEP = @echo "Checking WBU dependancies ..." ; \
		$(MAKE) -C ../../wbu/cat5k/obj-68-alc "IOSBOOT=yes" null
ALC_MKWBU_LIB = @echo "Driving WBU libs ..." ; \
		$(MAKE) -C ../../wbu/cat5k/obj-68-alc "IOSBOOT=yes" for_iosboot
else
ALC_MKWBU_DEP = @echo "Suppressing WBU dependancies check"
ALC_MKWBU_LIB = @echo "Suppressing WBU libs generation"
endif


#
# Provide a default value if not already given by user, and ensure
# that it is available to all sub-makes.
#
SYNALC_ROOT = ../../wbu/cat5k
export SYNALC_ROOT

SYNALC_PATH = $(SYNALC_ROOT)/alc_include $(SYNALC_ROOT)/alc_cominc $(SYNALC_ROOT)/sysif

SHELL=	/bin/sh
OPTIONS=
DEFS=-DSYNALC -DGCC68K -DMC68020 ${BSUN3}
CPPOPTS=-dD -nostdinc -I. -I../src-68-alc ${SYNALC_PATH:%=-I%} ${DEFS}
CCOPTS=-W -Wa,-R -g -Wa,-al > $(@:.o=.lst)
CFLAGS=-O ${OPTIONS} ${CPPOPTS} ${CCOPTS}

TRELOC= 0x90040000
BRELOC= 0x40000180
START=	evect

LDFLAGS= -e $(START) -N -Tdata $(BRELOC) -Ttext $(TRELOC) ${BSUN3} -nostdlib -static -T image.ld # -T ../../ldscripts.68k/m68kaout.xbn

#	Objects for ROM monitor image (asm.o must be first):
OBJS=	asm.o breakp.o confreg.o console.o cpu.o ctype.o \
	decodebe.o deex.o dinstr.o emulate.o extern.o flh_log.o \
	help.o init.o locore.o \
	meminit.o memtest.o monitor.o printf.o prom2.o refresh.o \
	save_stack.o stack.o stdio.o strlen.o strncmp.o sum.o test.o \
	uncompress.o util.o setcache.o nonvol_x2444.o \
# Objects for extended tests (-D XTESTS)
XTOBJS=	rom_tests.o fldvr.o 

CBASE=	../breakp.c ../confreg.c ../cpu.c ../ctype.c ../deex.c \
	../emulate.c ../extern.c ../flh_log.c ../help.c  \
	../memtest.c ../monitor.c ../printf.c ../prom2.c \
	../save_stack.c ../stack.c ../stdio.c ../strlen.c ../strncmp.c \
	../sum.c ../test.c ../uncompress.c ../util.c

HFILES=	../caps.h ../cpu.h ../ctype.h ../defs.h ../emt.h \
	../extern.h ../flh_log.h ../flh.h ../m68vectors.h ../prom2.h \
	../reason.h ../save_stack.h \
	../statreg.h ../stdio.h ../test.h \
	../src-68-ags/buserr.h ../src-68-alc/confreg.h \
	../src-68-alc/console.h ../src-68-alc/defs_alc.h \
	../i8254.h ../src-68-ags/opcodes.h ../src-68-alc/pcmap.h \
	../src-68-alc/nonvol_x2444.h

ASM=	../src-68-alc/locore.S ../asm.S

CALC=	../src-68-alc/console.c \
	../src-68-alc/decodebe.c ../src-68-ags/dinstr.c \
	../src-68-alc/init.c ../src-68-alc/refresh.c \
	../src-68-alc/setcache.c ../src-68-alc/rom_tests.c \
	../src-68-alc/fldvr.c ../src-68-alc/nonvol_x2444.c

ALCLIBS	= libbcsclnt.a libbcsutil.a lib_flashcsum.a libmfp.a \
	  libc_acp_boots.a libc_acp_kernel.a liblow_util.a

LIBLIST	= $(ALCLIBS:%=$(SYNALC_ROOT)/obj-68-alc/%)

vpath %.c	../src-68-alc:..:../src-68-ags
vpath %.h	../src-68-alc:..:../src-68-ags
vpath %.S	../src-68-alc:..:../src-68-ags

# Override RCS stuff

%: %,v

%: RCS/%,v

.SUFFIXES:
.SUFFIXES: .o .lis .S .c

%.o : %.S
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(ASMCC) -c $(CFLAGS) $<
%.o : %.c
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c $(CFLAGS) $<
%.lis : %.c
	lister $< $(CFLAGS) | grep -v '.stab' >$*.lis

init.o : init.c
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -S $(CFLAGS) $<
	grep -v '#.*APP' $(@:.o=.s) > $(@:.o=.foo)
	mv -f $(@:.o=.foo) $(@:.o=.s)
	$(QUIET_CC)$(ASMCC) -c $(CFLAGS) $(@:.o=.s)

wbu_libs:
	$(ALC_MKWBU_DEP)
	$(ALC_MKWBU_LIB)

boot: makefile ${OBJS} wbu_libs
	@sh ../cisco.sh TRUE
	$(QUIET_COM)echo "Compiling cisco.c to cisco.o"
	$(QUIET_CC)$(CC) $(CFLAGS) -c cisco.c
	mv -f $@ cisco.lst
	rm -f alc*
	$(LD) $(LDFLAGS) -o alc.aout \
		-Wl,"-Map,alc.map" \
		-Wl,-defsym,_buginf=_printf \
		${OBJS} cisco.o $(LIBLIST)
	$(SIZE) alc.aout
	@sh ../symbols.sh alc.aout alc.sym ATM-LC
	$(NM) -n alc.aout > alc.nmap
	$(STRIP) -s -v alc.aout

# Monitor.o cannot be optimized. The optimizer removes the code modifying
# the input argument register values. This breaks the single step and cache
# control commands:
monitor.o: monitor.c
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c ${OPTIONS} ${CPPOPTS} ${CCOPTS} ../monitor.c
monitor.lis: ../monitor.c
	lister ../monitor.c ${OPTIONS} ${CPPOPTS} ${CCOPTS} | grep -v '.stab' > monitor.lis

lint:
	@echo Linting...
	@lint -hxnzCc ${CPPOPTS} -Dinline= $(CBASE) $(CALC) >lint1.log
	@grep -v alignment lint1.log | grep -v llib-lc > LINT
	@rm lint1.log
	@echo Done. Output in LINT

clean:
	/bin/rm -f $(OBJS) alc* cisco.o cisco.s *.lis LINT

#
# The include file dependancies
#
depend:
	$(QUIET_COM)echo "Making dependancies"
	$(QUIET_CC)$(CC) $(CPPOPTS) -M $(CBASE) $(CALC) $(ASM) > dependancies
	@echo "Computing TAGS file"
	@etags -t $(CBASE) $(HFILES) $(ASM) $(CALC)
	@echo "Computing ID file"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM) $(CALC)

include dependancies
