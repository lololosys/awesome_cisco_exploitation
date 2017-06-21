# $Id: makefile,v 3.1.58.1 1996/05/27 21:18:18 mdb Exp $
# $Source: /release/112/cvs/Xboot/obj-68-c3000/makefile,v $
#------------------------------------------------------------------
# Makefile for Pancake (68020) bootstrap software
#
# Copyright (c) 1989-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.1.58.1  1996/05/27  21:18:18  mdb
# CSCdi58771:  use -Wa,-R for 96q1 instead of -R on gcc command line
# Branch: California_branch
#
# Revision 3.1  1995/11/09  09:08:00  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.2  1995/07/25  15:06:18  ssangiah
# CSCdi36833:  Copan with async daughtercard crashes in rommon on b f
# Change BSS start address from 120 to 140 to make space for the eight
# additional interrupt vectors for the two CD24xx controllers on the
# Copan daughter card.
#
# Revision 2.1  1995/06/07  19:14:11  hampton
# Bump version numbers from 1.x to 2.x.
#
# Revision 1.4  1994/10/30 21:58:39  abaindur
# CSCdi25240:  Need to support dfb on low end systems
# Flash Load Helper and Dual Flash Bank port.
#
# Revision 1.3.30.1  1994/10/07  23:30:25  enf
# Branch: Dfb_branch
# flh support for ROMMON
#
# Revision 1.3  1993/12/21  06:25:38  enf
# Cancun boot support
#
# Revision 1.2  1993/10/07  17:45:43  hampton
# Bring modular system to Aries 9.21(0.26) patch level.
#
# CSCdi13362:  Fix makefiles so that NM and CC can be multiple word
# strings.  Enclose '$(NM)' in double quotes before passing it to
# 'symbols.sh' so that NM can be a string composed of multiple words.
#
# CSCdi12795:  Can't build 9.21 68K system images on SGI boxes. 
# Use new SIZE, NM, and STRIP strings. Pass the correct version of 'nm'
# to 'symbols.sh'. Also, expand is now done by 'symbols.sh', as in the
# system tree.
#
# Revision 1.1  1993/09/24  16:32:25  widmer
# Rename boot obj directories to include CPU type and product family
#
# Revision 1.14  1993/07/01  23:10:53  hampton
# Bring modular system to Aries 9.21(0.12) patch level.
#
# CSCdi11649:  rom monitor worm test does not work and is not necessary
# 
# remove the worm test code.
#
# Revision 1.13  1993/07/01  22:27:55  whp4
# Wrong comment character used in previous edit.
#
# Revision 1.12  1993/06/25  05:08:56  smackie
# Bring modular system to Aries 9.21(0.10) patch level.
#
# CSCdi11223:  merge 9.14 low end code into aries
#
# Revision 1.11  1993/03/30  00:28:27  mdb
# Use ASMCC for compilation of .S files.
#
# Revision 1.10  1993/03/26  08:57:12  mdb
# Add support for the kinder gentler makefile output as is used in the
# sys directories by setting the environment variable called "QUIET".
#
# Use include ../Makefile.68k to get compiler variables set.
#
# Revision 1.9  1993/02/28  22:59:47  schaefer
# Chassis MIB
#
# Revision 1.8  1993/02/03  04:21:33  gstovall
# Removed support for the cristal low-end platform.
#
# Revision 1.7  1993/01/30  01:11:19  whp4
# Add RP1 support.
#
# Revision 1.6  1992/09/26  00:41:05  gstovall
# Added support for the serial NVRAM cookie used on cristal.  Copied kph's
# driver down to the rom monitor level.
#
# Revision 1.5  1992/09/23  17:12:48  clev
# Add improved cache command for pan.
# Add support for Bass.
#
# Revision 1.4  1992/08/31  20:37:54  dcoli
# Changes for Brut.
#
# Revision 1.3  1992/05/16  00:26:02  clev
# Removed flash test routines from Monitor
#
# Revision 1.2  1992/05/12  22:20:42  clev
# - Eliminate support for the physical configuration register on Pan.
#
# - Add support for the monitor command "o/r" for Pan and XX.
#
# - Break enable during boot on Pan.
#
# - Flash support added for Pan.
#
# Revision 1.1  1992/03/07  23:57:28  hampton
# Initial conversion from RCS to CVS.
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

include ../Makefile.68k

SHELL=	/bin/sh
OPTIONS=
DEFS=-DPAN -DMC68020 ${BSUN3}
CPPOPTS=-nostdinc -I. -I../src-68-c3000 ${DEFS}
CCOPTS=-W -Wa,-R -g
CFLAGS=-O ${OPTIONS} ${CPPOPTS} ${CCOPTS}

TRELOC= 1000000
BRELOC=	140
START=	evect

LDFLAGS= -e $(START) -N -Tdata $(BRELOC) -Ttext $(TRELOC) ${BSUN3} -nostdlib -static

#	Objects for ROM monitor image (asm.o must be first):
OBJS=	asm.o init.o cpu.o flh_log.o emulate.o monitor.o \
	extern.o decodebe.o prom2.o confreg.o locore.o \
	save_stack.o stack.o nonvol_x2444.o breakp.o dinstr.o \
	setcache.o memtest.o console.o refresh.o ctype.o \
	strlen.o stdio.o printf.o test.o sum.o dnld.o meminit.o \
	deex.o strncmp.o util.o uncompress.o help.o

# Objects for extended tests (-D XTESTS)
XTOBJS=	rom_tests.o fldvr.o 

CBASE=	../breakp.c ../confreg.c ../cpu.c ../ctype.c ../deex.c ../dnld.c \
	../emulate.c ../extern.c ../flh_log.c ../help.c  ../meminit.c \
	../memtest.c ../monitor.c ../printf.c ../prom2.c \
	../save_stack.c ../stack.c ../stdio.c ../strlen.c ../strncmp.c \
	../sum.c ../test.c ../uncompress.c ../util.c

HFILES=	../caps.h ../cpu.h ../ctype.h ../defs.h ../dnld.h ../emt.h \
	../extern.h ../flh_log.h ../flh.h ../m68vectors.h ../prom2.h \
	../reason.h ../save_stack.h \
	../statreg.h ../stdio.h ../test.h \
	../src-68-ags/buserr.h ../src-68-c3000/confreg.h \
	../src-68-c3000/console.h ../src-68-c3000/defs_pan.h \
	../i8254.h ../src-68-ags/opcodes.h ../src-68-c3000/pcmap.h \
	../src-68-c3000/nonvol_x2444.h

ASM=	../src-68-c3000/locore.S ../asm.S

CPAN=	../src-68-c3000/console.c \
	../src-68-c3000/decodebe.c ../src-68-ags/dinstr.c \
	../src-68-c3000/init.c ../src-68-c3000/refresh.c \
	../src-68-c3000/setcache.c ../src-68-c3000/rom_tests.c \
	../src-68-c3000/fldvr.c ../src-68-c3000/nonvol_x2444.c

vpath %.c	../src-68-c3000:..:../src-68-ags
vpath %.h	../src-68-c3000:..:../src-68-ags
vpath %.S	../src-68-c3000:..:../src-68-ags

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

boot: makefile ${OBJS}
	@sh ../cisco.sh TRUE
	$(QUIET_COM)echo "Compiling cisco.c to cisco.o"
	$(QUIET_CC)$(CC) $(CFLAGS) -c cisco.c
	rm -f pan
	$(LD) $(LDFLAGS) -o pan ${OBJS} cisco.o
	@$(SIZE) pan
	@sh ../symbols.sh pan pan.sym Pancake
	@cp pan pan.sun
	@$(STRIP) pan
	@convert pan pan.hpux
	@mv pan.hpux pan
	#dl68 -I -T 0 -s19 -Q1 -o pan.u48 pan
	#dl68 -I -T 0 -s19 -Q2 -o pan.u46 pan
	#dl68 -I -T 0 -s19 -Q3 -o pan.u44 pan
	#dl68 -I -T 0 -s19 -Q4 -o pan.u42 pan

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
	@lint -hxnzCc ${CPPOPTS} -Dinline= $(CBASE) $(CPAN) >lint1.log
	@grep -v alignment lint1.log | grep -v llib-lc > LINT
	@rm lint1.log
	@echo Done. Output in LINT

clean:
	/bin/rm -f $(OBJS) pan* cisco.o cisco.s *.lis LINT

#
# The include file dependancies
#
depend:
	$(QUIET_COM)echo "Making dependancies"
	$(QUIET_CC)$(CC) $(CPPOPTS) -M $(CBASE) $(CPAN) $(ASM) > dependancies
	@echo "Computing TAGS file"
	@etags -t $(CBASE) $(HFILES) $(ASM) $(CPAN)
	@echo "Computing ID file"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM) $(CPAN)

include dependancies
