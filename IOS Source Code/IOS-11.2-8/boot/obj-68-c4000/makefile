# $Id: makefile,v 3.1.58.1 1996/05/27 21:18:20 mdb Exp $
# $Source: /release/112/cvs/Xboot/obj-68-c4000/makefile,v $
#------------------------------------------------------------------
# Makefile for XX (68020) bootstrap software
#
# March 1992, Rob Clevenger
#
# Copyright (c) 1992-1996 by cisco Systems, Inc.
# All rights reserved.
# 
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.1.58.1  1996/05/27  21:18:20  mdb
# CSCdi58771:  use -Wa,-R for 96q1 instead of -R on gcc command line
# Branch: California_branch
#
# Revision 3.1  1995/11/09  09:08:03  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1  1995/06/07  19:14:15  hampton
# Bump version numbers from 1.x to 2.x.
#
# Revision 1.3  1994/03/19 02:01:56  clev
# CSCdi18266:  Add console download code for manufacturing for XX
#
# Revision 1.2  1993/10/07  17:45:47  hampton
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
# Revision 1.1  1993/09/24  16:32:28  widmer
# Rename boot obj directories to include CPU type and product family
#
# Revision 1.8  1993/07/01  23:11:21  hampton
# Bring modular system to Aries 9.21(0.12) patch level.
#
# CSCdi11649:  rom monitor worm test does not work and is not necessary
# 
# remove the worm test code.
#
# Revision 1.7  1993/03/30  00:28:31  mdb
# Use ASMCC for compilation of .S files.
#
# Revision 1.6  1993/03/26  08:57:15  mdb
# Add support for the kinder gentler makefile output as is used in the
# sys directories by setting the environment variable called "QUIET".
#
# Use include ../Makefile.68k to get compiler variables set.
#
# Revision 1.5  1993/02/28  22:59:59  schaefer
# Chassis MIB
#
# Revision 1.4  1993/01/30  01:12:13  whp4
# Add RP1 support.
#
# Revision 1.3  1992/05/16  00:26:46  clev
# Removed flash test routines from Monitor
#
# Revision 1.2  1992/04/17  23:49:19  schaefer
# fix typo
# ROM monitor changes to read image capability bits from cookie PROM
#
# Revision 1.1  1992/04/15  02:11:26  clev
# Initial checkin for XX
#
# Revision 1.1  1992/03/09  08:38:14  hampton
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
DEFS=-DXX -DMC68020 ${BSUN3}
CPPOPTS=-nostdinc -I. -I../src-68-c4000 ${DEFS}
CCOPTS=-W -Wa,-R -g
CFLAGS= -O ${OPTIONS} ${CPPOPTS} ${CCOPTS}

TRELOC= 1000000
BRELOC=	200
START=	evect

LDFLAGS= -e $(START) -N -Tdata $(BRELOC) -Ttext $(TRELOC) ${BSUN3} -nostdlib -static

#	Objects for ROM monitor image (asm.o must be first):
OBJS=	asm.o breakp.o confreg.o console.o cpu.o ctype.o dnld.o \
	locore.o decodebe.o deex.o dinstr.o emulate.o extern.o help.o \
	init.o memtest.o monitor.o prom2.o printf.o refresh.o setcache.o\
	save_stack.o stack.o stdio.o strlen.o strncmp.o sum.o test.o \
	uncompress.o util.o parity.o \
	memops.o

# Object files for extended tests (-D XTESTS)
XTOBJS=	rom_tests.o fldvr.o

CBASE=	../breakp.c ../confreg.c ../cpu.c ../ctype.c ../deex.c \
	../emulate.c ../extern.c ../help.c \
	../memtest.c ../monitor.c ../prom2.c ../dnld.c \
	../save_stack.c ../stack.c ../stdio.c ../strlen.c ../strncmp.c \
	../sum.c ../test.c ../uncompress.c ../util.c

HFILES=	../caps.h ../cpu.h ../ctype.h ../defs.h ../emt.h ../extern.h \
	../m68vectors.h ../prom2.h ../reason.h ../save_stack.h \
	../statreg.h ../stdio.h ../test.h ../dnld.h \
	../src-68-ags/buserr.h ../src-68-c4000/confreg.h \
	../src-68-c4000/console.h ../src-68-c4000/defs_xx.h \
	../i8254.h ../src-68-ags/opcodes.h ../src-68-c4000/pcmap.h

ASM=	../src-68-c4000/locore.S ../asm.S

CXX=	../src-68-c4000/console.c \
	../src-68-c4000/decodebe.c ../src-68-c4000/dinstr.c \
	../src-68-c4000/init.c ../src-68-c4000/refresh.c \
	../src-68-c4000/rom_tests.c ../src-68-c4000/fldvr.c \
	../src-68-c4000/parity.c ../src-68-c4000/printf.c \
	../src-68-c4000/setcache.c ../src-68-c4000/memops.c

vpath %.c	../src-68-c4000:..
vpath %.h	../src-68-c4000:..:../src-68-ags
vpath %.S	../src-68-c4000:..

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
	@rm -f xx
	@$(LD) $(LDFLAGS) -o xx ${OBJS} cisco.o
	@$(SIZE) xx
	@sh ../symbols.sh xx xx.sym dosxx
	cp xx xx.sun
	$(STRIP) xx
	convert xx xx.hpux
	mv xx.hpux xx

locore.o:  ../defs.h locore.S

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
	@lint -hxnzCc ${CPPOPTS} -Dinline= $(CBASE) $(CXX) >lint1.log
	@grep -v alignment lint1.log | grep -v llib-lc > LINT
	@rm lint1.log
	@echo Done. Output in LINT

clean:
	/bin/rm -f $(OBJS) xx* cisco.o cisco.s *.lis LINT

#
# The include file dependancies
#
depend:
	$(QUIET_COM)echo "Making dependancies"
	$(QUIET_CC)$(CC) $(CPPOPTS) -M $(CBASE) $(CXX) $(ASM) > dependancies
	@echo "Computing TAGS file"
	@etags -t $(CBASE) $(HFILES) $(ASM) $(CXX)
	@echo "Computing ID file"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM) $(CXX)

include dependancies

