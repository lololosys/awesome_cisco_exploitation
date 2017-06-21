# $Id: makefile,v 3.3.10.3 1996/05/27 20:58:45 mdb Exp $
# $Source: /release/112/cvs/Xsys/rommon/obj-36-c1000/makefile,v $
#------------------------------------------------------------------
# makefile
#
# October 1994, Rob Clevenger
#
# Copyright (c) 1995 - 1996 by cisco Systems, Inc.
# All rights reserved.
# 
# Makefile to build the ROM Monitor for Sapphire
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.3.10.3  1996/05/27  20:58:45  mdb
# CSCdi58771:  use -Wa,-R for 96q1 instead of -R on gcc command line
# Branch: California_branch
#
# Revision 3.3.10.2  1996/05/08  11:57:47  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.3.10.1  1996/03/21  23:24:48  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.2.2.2  1996/03/19  01:00:44  mbeesley
# Branch: ELC_branch
# Sync to V111_1_3
#
# Revision 3.2.2.1  1995/12/01  05:26:21  mbeesley
# Branch: ELC_branch
# Reorganize Rom monitor include files to allow easier porting.
#
# Revision 3.3  1996/02/23  17:15:32  cshen
# CSCdi48454:  Need to support on-board flash on c1005 derivative
#
# Revision 3.2  1995/11/17  18:40:15  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:03:59  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.4  1995/11/08  21:23:30  shaker
# Merge Arkansas_branch into 11.1 mainline.
#
# Revision 2.3  1995/09/22  04:36:09  enf
# CSCdi40869:  Add frankenstorm to 11.0
#
# Revision 2.2  1995/07/17  23:31:34  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Clean up bogus tool includes
#
# Revision 2.1  1995/06/07  22:36:06  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../../scripts/sysroot)
endif

default: all

include ../../makefile.tools
include ../../makefile.68

.PHONY: all clean clobber depend force-tools install

# Crash us if /bin/make is used:
ifeq ($(origin USER),undefined)
USER=Bozo
endif

SHELL=	/bin/sh
OPTIONS=
IROOT = ../../..
INCDIR= $(SYSROOT)/rommon/src
# The platform specific source directory
SRCDIR = $(SYSROOT)/rommon/src-36-c1000
# The processor specific source directory
PROCSRCDIR = $(SYSROOT)/rommon/src-68
# Another processor specific source directory
XPROCSRCDIR = $(SYSROOT)/rommon/src-36
# The common source directory
COMSRCDIR = $(SYSROOT)/rommon/src
# The common netboot directory
NETSRCDIR = $(SYSROOT)/rommon/src-netboot
UTIL_DIR = $(SYSROOT)/rommon/utils

# Do not change the order of the includes - things will break
INC = -I$(SRCDIR) -I$(XPROCSRCDIR) -I$(PROCSRCDIR) -I$(NETSRCDIR) \
	-I$(INCDIR) -I$(SYSROOT)/src-36-c1000 -I$(SYSROOT)/src-36 \
	-I$(SYSROOT)/h \
	-I$(SYSROOT)/src-68k \
	-I$(IROOT)/boot/src-68-c3000 -I$(SYSROOT)/src-4k

DEFS=-DSAPPHIRE -DMC68020 ${BSUN3}
CPPOPTS=-nostdinc $(INC) ${DEFS}
# below should have -W as well FIXME
CCOPTS= -Wa,-R -g -mnobitfield -fno-builtin
ASOPTS= -fno-builtin
CFLAGS= -O ${OPTIONS} ${CPPOPTS} ${CCOPTS}
ASFLAGS= -O ${OPTIONS} ${CPPOPTS} ${ASOPTS}


# Uncompress defines
UNCOMPRESS = uncompress
UNCOBJS    = scrt0_68_unc.o sa_uncompress.o
#UNCLDFLAGS = -e start -N -Ttext 2004000 -nostdlib -static
UNCLDFLAGS = -e start -N -Ttext 2004000 -nostdlib -static

# TARGET is the stripped a.out binary file, SUNFILE is unstripped
TARGET=   C1000_RM
SYMFILE=  $(TARGET).sym
SUNFILE=  $(TARGET).sun
SRECFILE= $(TARGET).srec

DEFS_P=$(DEFS) -DONBOARD_FLASH
CPPOPTS_P=-nostdinc $(INC) ${DEFS_P}
CFLAGS_P= -O ${OPTIONS} ${CPPOPTS_P} ${CCOPTS}

TARGET_P=   P1000_RM
SYMFILE_P=  $(TARGET_P).sym
SUNFILE_P=  $(TARGET_P).sun
SRECFILE_P= $(TARGET_P).srec


T_RELOC = 000000
D_RELOC = 2000400
LDFLAGS	=  -e __start -N -Ttext $(T_RELOC) -Tdata $(D_RELOC) -nostdlib -static

# ROM Based Object files : start.o must be first !
# Note the Sapphire monitor does not use main.o
# All initiallization is done in init() and the monitor
# is invoked from brkpt_isr()
ROBJS = start.o init.o

# The object files needed to give the image the power to boot

BOOTOBJS = monfsio.o simpfsio.o monlib_iface.o boot.o loadprog.o \
	ismagic.o device.o devtbl.o rawio.o fsutils.o

# The remaining object files

OBJS = moncmds.o monitor.o parsetoken.o shcmd.o memops.o alias.o builtin.o \
	nvram.o console_36.o mon_exceptn.o testmem.o query.o sizemem.o \
	error.o signal.o dnld.o cookie.o emulate.o quicc.o timer.o tp3420.o \
	confreg.o diagtools.o parity.o berrtest.o disasm.o \
	nvcookie.o pcmcia.o clpd6720.o breakpt.o context.o stack.o spi.o \
	menu.o diag.o enet_sap.o quicc_bri.o quicc_serial.o

POBJS = moncmds.o monitor.o parsetoken.o shcmd.o memops.o alias.o builtin.o \
	nvram.o console_36.o mon_exceptn.o testmem.o query.o sizemem.o \
	error.o signal.o dnld.o cookie.o emulate.o quicc.o timer.o tp3420.o \
	confreg.o diagtools.o parity.o berrtest.o disasm.o \
	nvcookie.o breakpt.o context.o stack.o spi.o \
	menu.o diag.o enet_sap.o quicc_bri.o quicc_serial.o

PARTNEROBJS = flash_umi.o flashdrvr.o

SHAREDOBJS = init.o moncmds.o diag.o

# The modules that are usually in the library are no longer

LIBOBJS = atoh.o getline.o getopt.o puts.o strings.o \
	bcmp.o getnum.o printf.o setjmp.o

CBASE=	$(COMSRCDIR)/monitor.c $(COMSRCDIR)/parsetoken.c \
	$(COMSRCDIR)/shcmd.c $(COMSRCDIR)/memops.c $(COMSRCDIR)/confreg.c \
	$(COMSRCDIR)/alias.c $(COMSRCDIR)/builtin.c $(COMSRCDIR)/nvram.c \
	$(XPROCSRCDIR)/console_36.c $(COMSRCDIR)/cookie.c $(SRCDIR)/enet_sap.c \
	$(COMSRCDIR)/testmem.c $(COMSRCDIR)/query.c $(COMSRCDIR)/error.c \
	$(SRCDIR)/init.c $(XPROCSRCDIR)/quicc.c $(SRCDIR)/moncmds.c \
	$(SRCDIR)/mon_exceptn.c $(SRCDIR)/sizemem.c $(SRCDIR)/nvcookie.c \
	$(COMSRCDIR)/signal.c $(PROCSRCDIR)/berrtest.c $(PROCSRCDIR)/disasm.c \
	$(SRCDIR)/emulate.c $(SRCDIR)/parity.c $(SRCDIR)/timer.c \
	$(COMSRCDIR)/diagtools.c $(COMSRCDIR)/dnld.c $(XPROCSRCDIR)/breakpt.c \
	$(COMSRCDIR)/monfsio.c $(COMSRCDIR)/simpfsio.c $(COMSRCDIR)/rawio.c \
	$(COMSRCDIR)/loadprog.c $(COMSRCDIR)/fsutils.c $(PROCSRCDIR)/stack.c \
	$(COMSRCDIR)/monlib_iface.c $(COMSRCDIR)/boot.c $(COMSRCDIR)/ismagic.c \
	$(COMSRCDIR)/device.c $(SRCDIR)/devtbl.c $(SRCDIR)/diag.c \
	$(COMSRCDIR)/atoh.c $(COMSRCDIR)/getline.c $(COMSRCDIR)/getopt.c \
	$(COMSRCDIR)/puts.c $(COMSRCDIR)/strings.c $(PROCSRCDIR)/context.c \
	$(COMSRCDIR)/bcmp.c $(COMSRCDIR)/getnum.c $(COMSRCDIR)/printf.c \
	$(SRCDIR)/pcmcia.c $(SRCDIR)/clpd6720.c $(COMSRCDIR)/sa_uncompress.c \
	$(SRCDIR)/tp3420.c $(SRCDIR)/quicc_bri.c $(SRCDIR)/quicc_serial.c \
	$(COMSRCDIR)/menu.c $(SRCDIR)/spi.c $(SRCDIR)/flash_umi.c \
        $(SRCDIR)/flashdrvr.c

HFILES=	$(SRCDIR)/nvmonvars.h $(INCDIR)/setjmp.h $(INCDIR)/mon_elf.h \
	$(INCDIR)/queryflags.h $(INCDIR)/sh.h $(INCDIR)/menu.h \
	$(INCDIR)/testmem.h $(INCDIR)/bitdef.h $(SRCDIR)/mon_plat_defs.h\
	$(INCDIR)/mon_defs.h $(INCDIR)/dnld.h $(XPROCSRCDIR)/console_36.h \
	$(INCDIR)/mon_boot.h $(PROCSRCDIR)/exception.h \
	$(SYSROOT)/os/signal.h $(IROOT)/boot/flh.h $(SYSROOT)/h/elf.h \
	$(INCDIR)/dev.h $(INCDIR)/dnld.h $(PROCSRCDIR)/context.h \
	$(INCDIR)/proto.h $(INCDIR)/eeprom.h $(INCDIR)/error.h \
	$(INCDIR)/strings.h $(INCDIR)/simpfsio.h $(PROCSRCDIR)/stack.h \
	$(INCDIR)/monitor.h $(INCDIR)/monfsio.h $(INCDIR)/fsio.h \
	$(SYSROOT)/src-36-c1000/pcmap.h $(SYSROOT)/src-4k/signal_4k.h \
	$(IROOT)/boot/src-68-c3000/cookie.h $(SYSROOT)/src-36/quicc.h \
	$(IROOT)/boot/types.h $(IROOT)/boot/cpu.h $(SRCDIR)/quicc_bri.h  \
	$(SRCDIR)/quicc_serial.h  \
	$(INCDIR)/confreg.h $(INCDIR)/extern.h $(IROOT)/boot/m68vectors.h \
	$(IROOT)/boot/emt.h $(IROOT)/boot/reason.h \
	$(SRCDIR)/clpd6720.h $(SRCDIR)/clpd6720_internals.h \
	$(SYSROOT)/les/flash_fs_les.h $(INCDIR)/stdarg.h \
	$(SRCDIR)/enet_sap.h $(SRCDIR)/tp3420.h

ASM=	$(SRCDIR)/start.S $(PROCSRCDIR)/setjmp.S $(PROCSRCDIR)/scrt0_68_unc.S

UTOOLS=  epromsum-$(NATIVE_OSREV) mkffsi-$(NATIVE_OSREV)
HTOOLS=  hxld-$(NATIVE_OSREV)
TOOLS=  $(patsubst %,$(UTIL_DIR)/%,$(UTOOLS)) $(patsubst %,$(UTIL_DIR)/hxldsrc/%,$(HTOOLS))

vpath %.c	$(SRCDIR):$(XPROCSRCDIR):$(PROCSRCDIR):$(COMSRCDIR):$(NETSRCDIR)
vpath %.h	$(COMSRCDIR):$(SRCDIR):$(PROCSRCDIR):$(SYSROOT)/h:\
	$(SYSROOT)/src-68k:$(IROOT)/boot/src-c4000:\
	$(IROOT)/boot/src-68-c4000:$(NETSRCDIR)
vpath %.S	$(COMSRCDIR):$(SRCDIR):$(PROCSRCDIR):$(XPROCSRCDIR):$(NETSRCDIR)

# Override RCS stuff

#%: %,v

#%: RCS/%,v

.SUFFIXES:
.SUFFIXES: .o .lis .S .c .s

%.o : %.S
	$(CC) -c $(ASFLAGS) $<
%.o : %.c
	$(CC) -c $(CFLAGS) $<
%.lis : %.c
	lister3 CC="$(CC) -c $(CFLAGS)" asm=1 targ=m68k $< >$*.lis

all: force-tools tools $(TARGET) $(SRECFILE) $(UNCOMPRESS) $(TARGET_P) $(SRECFILE_P)

prom: all
	sh mkpromsrec
	sh mkpromsrec -P

partner: force-tools tools $(TARGET_P) $(SRECFILE_P) $(UNCOMPRESS)
	sh mkpromsrec -P

install: all install.sh
	sh install.sh $(SRECFILE)
	touch install.sh

onboard_flash:
	$(CC) -c $(CFLAGS_P) $(SRCDIR)/init.c
	$(CC) -c $(CFLAGS_P) $(SRCDIR)/moncmds.c
	$(CC) -c $(CFLAGS_P) $(SRCDIR)/diag.c

clean:
	rm -f $(OBJS) $(ROBJS) $(BOOTOBJS) $(LIBOBJS) $(UNCOBJS) banner.* \
	tools $(PARTNEROBJS)

clean_shared: 
	rm -f $(SHAREDOBJS)

clobber: clean
	rm -f $(TARGET)* $(TARGET_P)* $(UNCOMPRESS)* pfsi syms* 

superclean: clobber
	$(MAKE) --no-print-directory -C $(UTIL_DIR) clean
	$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc clean

$(SRECFILE): $(TARGET) tools
	$(UTIL_DIR)/hxldsrc/hxld-$(NATIVE_OSREV) -r 32 -a 0  -f $@ $(TARGET)

$(SRECFILE_P): $(TARGET_P) tools
	$(UTIL_DIR)/hxldsrc/hxld-$(NATIVE_OSREV) -r 32 -a 0  -f $@ $(TARGET_P)

force-tools:
	rm -f tools

tools:
	@$(MAKE) --no-print-directory -C $(UTIL_DIR) depend $(UTOOLS)
	@$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc depend $(HTOOLS)
	touch tools


$(TARGET): clean_shared $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) makefile $(UTIL_DIR)/banner.sh \
	tools $(UTIL_DIR)/symbols_elf.sh
	@sh $(UTIL_DIR)/banner.sh C1000
	$(CC) $(CFLAGS) -c banner.c
	$(LD) $(LDFLAGS) -o $(SUNFILE) $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) banner.o
	$(SIZE) $(SUNFILE)
	@$(NM) -p $(SUNFILE) | egrep -v "   U" > syms
	$(UTIL_DIR)/epromsum-$(NATIVE_OSREV) syms $(SUNFILE)
	@rm -f syms
	sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(SUNFILE) $(TARGET).symbols $(SUNFILE)
	cp $(SUNFILE) $(TARGET)
	$(STRIP) $(TARGET)

$(TARGET_P): clean_shared onboard_flash $(ROBJS) $(POBJS) $(PARTNEROBJS) $(BOOTOBJS) $(LIBOBJS) makefile $(UTIL_DIR)/banner.sh \
	tools $(UTIL_DIR)/symbols_elf.sh
	@sh $(UTIL_DIR)/banner.sh $(TARGET_P)
	$(CC) $(CFLAGS) -c banner.c
	$(LD) $(LDFLAGS) -o $(SUNFILE_P) $(ROBJS) $(POBJS) $(PARTNEROBJS) $(BOOTOBJS) $(LIBOBJS) banner.o
	$(SIZE) $(SUNFILE_P)
	@$(NM) -p $(SUNFILE_P) | egrep -v "   U" > syms
	$(UTIL_DIR)/epromsum-$(NATIVE_OSREV) syms $(SUNFILE_P)
	@rm -f syms
	sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(SUNFILE_P) $(TARGET_P).symbols $(SUNFILE_P)
	cp $(SUNFILE_P) $(TARGET_P)
	$(STRIP) $(TARGET_P)


$(UNCOMPRESS): $(UNCOBJS) makefile $(UTIL_DIR)/symbols_elf.sh \
		tools 
	$(LD) $(UNCLDFLAGS) -o $(UNCOMPRESS) $(UNCOBJS)
	@$(SIZE) $(UNCOMPRESS)
	@$(NM) $(UNCOMPRESS) >$(UNCOMPRESS).symbols
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(UNCOMPRESS) $(UNCOMPRESS).symbols $(UNCOMPRESS)
	@$(STRIP) $(UNCOMPRESS)

# Below is an example of how to construct a ROM image in srecord format
# for downloading to a PROM programmer.  Note the "-a 18000" option to
# combpcode.sh.  This is vital as it is the link address of the rboot image.
romimage.srec: $(SRECFILE) tools $(UTIL_DIR)/combpcode.sh $(UNCOMPRESS) rboot
	# rboot program must be first in file system
	$(UTIL_DIR)/mkffsi-$(NATIVE_OSREV) rboot $(UNCOMPRESS) pfsi
	sh $(UTIL_DIR)/combpcode.sh -a 18000 $(SRECFILE) pfsi romimage.srec >/dev/null
	@rm -f pfsi

#
# The include file dependencies
#
depend: dependancies $(CBASE) $(HFILES) $(ASM)
	@echo "Computing dependancies in rommon/obj-36-c1000"
	@$(CC) $(CPPOPTS) -M $(CBASE) $(ASM) > dependancies
	@echo "Computing TAGS file in rommon/obj-36-c1000"
	@etags -t $(CBASE) $(HFILES) $(ASM)
	@echo "Computing ID file in rommon/obj-36-c1000"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM)

dependancies:
	rm -f dependancies
	touch dependancies

include dependancies
