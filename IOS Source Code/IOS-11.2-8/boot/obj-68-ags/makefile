# $Id: makefile,v 3.1.58.1 1996/05/27 21:18:14 mdb Exp $
# $Source: /release/112/cvs/Xboot/obj-68-ags/makefile,v $
#------------------------------------------------------------------
# Makefile for CSC2 (68020) bootstrap software
#
# Copyright (c) 1986-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.1.58.1  1996/05/27  21:18:14  mdb
# CSCdi58771:  use -Wa,-R for 96q1 instead of -R on gcc command line
# Branch: California_branch
#
# Revision 3.1  1995/11/09  09:07:53  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1  1995/06/07  19:14:00  hampton
# Bump version numbers from 1.x to 2.x.
#
# Revision 1.2  1993/10/07 17:45:41  hampton
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
# Revision 1.1  1993/09/24  16:32:21  widmer
# Rename boot obj directories to include CPU type and product family
#
# Revision 1.6  1993/07/01  23:10:37  hampton
# Bring modular system to Aries 9.21(0.12) patch level.
#
# CSCdi11649:  rom monitor worm test does not work and is not necessary
# 
# remove the worm test code.
#
# Revision 1.5  1993/03/30  00:28:14  mdb
# Use ASMCC for compilation of .S files.
#
# Revision 1.4  1993/03/26  08:56:47  mdb
# Add support for the kinder gentler makefile output as is used in the
# sys directories by setting the environment variable called "QUIET".
#
# Use include ../Makefile.68k to get compiler variables set.
#
# Revision 1.3  1993/02/28  22:59:40  schaefer
# Chassis MIB
#
# Revision 1.2  1993/01/30  01:11:08  whp4
# Add RP1 support.
#
# Revision 1.1  1992/03/07  23:56:13  hampton
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
DEFS=-DCSC2 -DMC68020 ${BSUN3}
CPPOPTS=-nostdinc -I. -I.. -I../diag/h ${DEFS}
CCOPTS=-W -Wa,-R -g
CFLAGS=-O ${OPTIONS} ${CPPOPTS} ${CCOPTS}

TRELOC= 1000000
BRELOC=	120
START=	evect

LDFLAGS= -e $(START) -N -Tdata $(BRELOC) -Ttext $(TRELOC) ${BSUN3} -nostdlib -static

#	Objects for ROM monitor image (asm.o must be first):
OBJS=	asm.o breakp.o confreg.o console.o cpu.o ctype.o \
	decodebe.o deex.o dinstr.o emulate.o extern.o help.o \
	init.o ioprobetest.o locore.o meminit.o memtest.o monitor.o \
	printf.o prom2.o refresh.o save_stack.o stack.o stdio.o \
	strlen.o strncmp.o sum.o test.o uncompress.o util.o

#	Objects for SYSTEM TEST ROM monitor image:
SOBJS=	locore.o extern.o asm.o init.o refresh.o meminit.o \
	monitor.o emulate.o prom2.o testSYSTEST.o ioprobetest.o memtest.o \
	console.o help.o breakp.o stdio.o confreg.o save_stack.o \
	stack.o decodebe.o cpu.o uncompress.o sum.o \
	ctype.o strlen.o strncmp.o util.o applique.o printf.o \
	dinstr.o deex.o mec_test.o mcitest.o mci_err.o xmem.o \
	ds3-ultra_test.o dci_test.o

CBASE=	../breakp.c ../confreg.c ../cpu.c ../ctype.c ../deex.c \
	../emulate.c ../extern.c ../help.c ../meminit.c \
	../memtest.c ../monitor.c ../printf.c ../prom2.c \
	../save_stack.c ../stack.c ../stdio.c ../strlen.c ../strncmp.c \
	../sum.c ../test.c ../uncompress.c ../util.c \
	../diag/mci/mcitest.c ../diag/mci/mci_err.c  \
	../diag/mec/mec_test.c ../diag/mci/xmem.c ../diag/mci/applique.c \
	../diag/mec/dci_test.c ../diag/mec/ds3-ultra_test.c


HFILES=	../caps.h ../cpu.h ../ctype.h ../defs.h ../emt.h ../extern.h \
	../m68vectors.h ../prom2.h ../reason.h ../save_stack.h \
	../statreg.h ../stdio.h ../test.h \
	../src-68-ags/buserr.h ../src-68-ags/confreg.h \
	../src-68-ags/console.h ../src-68-ags/defs_csc2.h \
	../i8254.h ../src-68-ags/opcodes.h \
	../src-68-ags/pcmap.h	../diag/h/master.h \
	../diag/h/interface.h ../diag/h/mci_err.h \
	../diag/h/ttysrv.h ../diag/h/ether.h ../diag/h/if_mci.h \
	../diag/h/if_fci.h ../diag/h/std_dci.h ../diag/h/dci_diag_defs.h


ASM=	../src-68-ags/locore.S ../asm.S

CCSC2=	../src-68-ags/console.c \
	../src-68-ags/decodebe.c ../src-68-ags/dinstr.c \
	../src-68-ags/init.c ../src-68-ags/ioprobetest.c \
	../src-68-ags/refresh.c ../src-68-ags/rom_tests.c

VPATH= ../src-68-ags:..:../diag/mci:../diag/mec

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
	rm -f csc2
	$(LD) $(LDFLAGS) -o csc2 ${OBJS} cisco.o
	@$(SIZE) csc2
	@sh ../symbols.sh csc2 csc2.symbols CSC/2
	@cp csc2 csc2.sun
	@$(STRIP) csc2
	@convert csc2 csc2.hpux
	@mv csc2.hpux csc2
	#dl68 -I -T 0 -s19 -Q1 -o csc2.u48 csc2
	#dl68 -I -T 0 -s19 -Q2 -o csc2.u46 csc2
	#dl68 -I -T 0 -s19 -Q3 -o csc2.u44 csc2
	#dl68 -I -T 0 -s19 -Q4 -o csc2.u42 csc2

sboot: makefile ${SOBJS}
	@sh ../cisco.sh TRUE
	$(QUIET_COM)echo "Compiling cisco.c to cisco.o"
	$(QUIET_CC)$(CC) $(CFLAGS) -c cisco.c
	$(LD) $(LDFLAGS) -o scsc2 ${SOBJS} cisco.o
	@$(SIZE) scsc2
	@sh ../symbols.sh "$(NM)" scsc2 scsc2.symbols CSC/2
	@cp scsc2 scsc2.sun
	@$(STRIP) scsc2
	@convert scsc2 scsc2.hpux
	@mv scsc2.hpux scsc2
#	dl68 -I -T 0 -s19 -Q1 -o scsc2.u48 scsc2
#	dl68 -I -T 0 -s19 -Q2 -o scsc2.u46 scsc2
#	dl68 -I -T 0 -s19 -Q3 -o scsc2.u44 scsc2
#	dl68 -I -T 0 -s19 -Q4 -o scsc2.u42 scsc2

# Monitor.o cannot be optimized. The optimizer removes the code modifying
# the input argument register values. This breaks the single step and cache
# control commands:
monitor.o: monitor.c
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c ${OPTIONS} ${CPPOPTS} ${CCOPTS} ../monitor.c
monitor.lis: ../monitor.c
	lister ../monitor.c ${OPTIONS} ${CPPOPTS} ${CCOPTS} | grep -v '.stab' > monitor.lis

# Rule for making SYSTESTtest image:
testSYSTEST.o:	../test.c
	$(QUIET_COM)echo "Compiling $< to $@"
	$(QUIET_CC)$(CC) -c $(CFLAGS) -DSYSTEST -o testSYSTEST.o ../test.c

lint:
	@echo Linting...
	lint -hxnzCc ${CPPOPTS} -DDEBUG -Dinline= $(CBASE) $(CCSC2) >lint1.log
	grep -v alignment lint1.log | grep -v llib-lc > LINT
	rm lint1.log
	@echo Done. Output in LINT

clean:
	/bin/rm -f $(OBJS) $(DOBJS) csc2* cisco.o cisco.s *.lis LINT

#
# The include file dependancies
#
depend:
	$(QUIET_COM)echo "Making dependancies"
	$(QUIET_CC)$(CC) $(CPPOPTS) -M $(CBASE) $(CCSC2) $(ASM) > dependancies
	@echo "Computing TAGS file"
	@etags -t $(CBASE) $(HFILES) $(ASM) $(CCSC2)
	@echo "Computing ID file"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM) $(CCSC2)

include dependancies
