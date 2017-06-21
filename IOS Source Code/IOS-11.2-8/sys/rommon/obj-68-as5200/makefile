# $Id: makefile,v 1.1.68.1 1996/06/16 21:17:56 vnguyen Exp $
# $Source: /release/112/cvs/Xsys/rommon/obj-68-as5200/makefile,v $
#------------------------------------------------------------------
# makefile -- Makefile to build the ROM Monitor for Brasil
#
# October 1995, Joe Turner
#
# Copyright (c) 1995-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 1.1.68.1  1996/06/16  21:17:56  vnguyen
# Finally committing Brasil to California_branch
#
# Revision 1.1  1995/12/03  21:11:29  jturner
# Initial revision
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
INCDIR= $(IROOT)/sys/rommon/src
# The platform specific source directory
SRCDIR = $(IROOT)/sys/rommon/src-68-as5200
# The processor specific source directory
PROCSRCDIR = $(IROOT)/sys/rommon/src-68
# Another processor specific source directory
XPROCSRCDIR = $(IROOT)/sys/rommon/src-36
# The common source directory
COMSRCDIR = $(IROOT)/sys/rommon/src
# The common netboot directory
NETSRCDIR = $(IROOT)/sys/rommon/src-netboot
UTIL_DIR = $(IROOT)/sys/rommon/utils

# Do not change the order of the includes - things will break
INC = -I$(SRCDIR) -I$(PROCSRCDIR) -I$(NETSRCDIR) \
	-I$(INCDIR) -I$(IROOT)/sys/src-68-as5200 \
	-I$(IROOT)/boot -I$(IROOT)/sys/h \
	-I$(IROOT)/sys/src-68k -I$(IROOT)/sys/les -I$(IROOT)/sys/ts \
	-I$(IROOT)/boot/src-68-c3000 -I$(IROOT)/sys/src-4k

DEFS=-DBRASIL -DMC68020 ${BSUN3}
CPPOPTS=-nostdinc $(INC) ${DEFS}
# below should have -W as well FIXME
CCOPTS= -Wa,-R -g -mnobitfield -fno-builtin
ASOPTS= -fno-builtin
CFLAGS= -O ${OPTIONS} ${CPPOPTS} ${CCOPTS}
ASFLAGS= -O ${OPTIONS} ${CPPOPTS} ${ASOPTS}


# Uncompress defines
UNCOMPRESS = uncompress
UNCOBJS    = scrt0_68_unc.o sa_uncompress.o
UNCLDFLAGS = -e start -N -Ttext 5000 -nostdlib -static

# TARGET is the stripped a.out binary file, SUNFILE is unstripped
TARGET=   AS5200_RM
SYMFILE=  $(TARGET).sym
SUNFILE=  $(TARGET).sun
SRECFILE= $(TARGET).srec

T_RELOC = 1000000
D_RELOC = 400
LDFLAGS	=  -e __start -N -Ttext $(T_RELOC) -Tdata $(D_RELOC) -nostdlib -static

# ROM Based Object files : start.o must be first !
# Note the this monitor does not use main.o
# All initiallization is done in init() and the monitor
# is invoked from brkpt_isr()
ROBJS = start.o init.o

# The object files needed to give the image the power to boot

BOOTOBJS = monfsio.o simpfsio.o monlib_iface.o boot.o loadprog.o \
	ismagic.o device.o devtbl.o rawio.o fsutils.o

# The remaining object files

OBJS = moncmds.o monitor.o parsetoken.o shcmd.o memops.o alias.o builtin.o \
	nvram.o console.o mon_exception.o testmem.o query.o sizemem.o \
	error.o signal.o dnld.o test2444.o cookie.o emulate.o timer.o tcalibrate.o \
	confreg.o diagtools.o parity.o flashdrvr.o flash.o berrtest.o disasm.o \
	breakpt.o context.o stack.o menu.o diag.o xmodem_generic.o \
	as5200_xmodem.o 

SHAREDOBJS = init.o moncmds.o diag.o

# The modules that are usually in the library are no longer

LIBOBJS = atoh.o getline.o getopt.o puts.o strings.o \
	bcmp.o getnum.o printf.o setjmp.o

CBASE=	$(COMSRCDIR)/monitor.c $(COMSRCDIR)/parsetoken.c \
	$(COMSRCDIR)/shcmd.c $(COMSRCDIR)/memops.c $(COMSRCDIR)/confreg.c \
	$(COMSRCDIR)/alias.c $(COMSRCDIR)/builtin.c $(COMSRCDIR)/nvram.c \
	$(COMSRCDIR)/console.c $(COMSRCDIR)/cookie.c \
	$(COMSRCDIR)/testmem.c $(COMSRCDIR)/query.c $(COMSRCDIR)/error.c \
	$(SRCDIR)/init.c $(COMSRCDIR)/xmodem_generic.c \
	$(SRCDIR)/moncmds.c $(SRCDIR)/as5200_xmodem.c \
	$(INCDIR)/flash.c $(SRCDIR)/tcalibrate.c $(SRCDIR)/flashdrvr.c \
	$(SRCDIR)/mon_exception.c $(SRCDIR)/sizemem.c $(SRCDIR)/test2444.c \
	$(COMSRCDIR)/signal.c $(PROCSRCDIR)/berrtest.c $(PROCSRCDIR)/disasm.c \
	$(SRCDIR)/emulate.c $(SRCDIR)/parity.c $(SRCDIR)/timer.c \
	$(COMSRCDIR)/diagtools.c $(COMSRCDIR)/dnld.c $(PROCSRCDIR)/breakpt.c \
	$(COMSRCDIR)/monfsio.c $(COMSRCDIR)/simpfsio.c $(COMSRCDIR)/rawio.c \
	$(COMSRCDIR)/loadprog.c $(COMSRCDIR)/fsutils.c $(PROCSRCDIR)/stack.c \
	$(COMSRCDIR)/monlib_iface.c $(COMSRCDIR)/boot.c $(COMSRCDIR)/ismagic.c \
	$(COMSRCDIR)/device.c $(SRCDIR)/devtbl.c $(SRCDIR)/diag.c \
	$(COMSRCDIR)/atoh.c $(COMSRCDIR)/getline.c $(COMSRCDIR)/getopt.c \
	$(COMSRCDIR)/puts.c $(COMSRCDIR)/strings.c $(PROCSRCDIR)/context.c \
	$(COMSRCDIR)/bcmp.c $(COMSRCDIR)/getnum.c $(COMSRCDIR)/printf.c \
	$(COMSRCDIR)/sa_uncompress.c $(COMSRCDIR)/menu.c

HFILES=	$(SRCDIR)/nvmonvars.h $(INCDIR)/setjmp.h $(INCDIR)/mon_elf.h \
	$(INCDIR)/queryflags.h $(INCDIR)/sh.h $(INCDIR)/menu.h \
	$(INCDIR)/testmem.h $(INCDIR)/bitdef.h $(SRCDIR)/mon_plat_defs.h\
	$(INCDIR)/mon_defs.h $(INCDIR)/nonvol_x2444.h \
	$(INCDIR)/dnld.h $(COMSRCDIR)/console.h \
	$(INCDIR)/mon_boot.h $(PROCSRCDIR)/exception.h $(INCDIR)/flash.h \
	$(IROOT)/sys/os/signal.h $(IROOT)/boot/flh.h $(IROOT)/sys/h/elf.h \
	$(INCDIR)/dev.h $(INCDIR)/dnld.h $(PROCSRCDIR)/context.h \
	$(INCDIR)/proto.h $(INCDIR)/eeprom.h $(INCDIR)/error.h \
	$(INCDIR)/strings.h $(INCDIR)/simpfsio.h $(PROCSRCDIR)/stack.h \
	$(INCDIR)/monitor.h $(INCDIR)/monfsio.h $(INCDIR)/fsio.h \
	$(IROOT)/boot/src-68-c3000/pcmap.h $(COMSRCDIR)/xmodem_generic.h \
	$(IROOT)/sys/src-68-as5200/as5200_pcmap.h \
	$(IROOT)/sys/src-4k/signal_4k.h $(IROOT)/boot/src-68-c3000/cookie.h \
	$(IROOT)/boot/types.h $(IROOT)/boot/cpu.h \
	$(INCDIR)/confreg.h $(INCDIR)/extern.h $(IROOT)/boot/m68vectors.h \
	$(IROOT)/boot/emt.h $(IROOT)/boot/reason.h \
	$(IROOT)/sys/les/flash_fs_les.h $(INCDIR)/stdarg.h \
	$(IROOT)/sys/ts/s2681.h

ASM=	$(SRCDIR)/start.S $(PROCSRCDIR)/setjmp.S $(PROCSRCDIR)/scrt0_68_unc.S

UTOOLS=  epromsum-$(NATIVE_OSREV) mkffsi-$(NATIVE_OSREV)
HTOOLS=  hxld-$(NATIVE_OSREV)
TOOLS=  $(patsubst %,$(UTIL_DIR)/%,$(UTOOLS)) $(patsubst %,$(UTIL_DIR)/hxldsrc/%,$(HTOOLS))

vpath %.c	$(SRCDIR):$(PROCSRCDIR):$(COMSRCDIR):$(NETSRCDIR)
vpath %.h	$(COMSRCDIR):$(SRCDIR):$(PROCSRCDIR):$(SYSROOT)/h:$(SYSROOT)/ts:\
	$(SYSROOT)/src-68k:$(SYSROOT)/les:$(IROOT)/boot/src-c4000:\
	$(IROOT)/boot/src-68-c4000:$(NETSRCDIR)
vpath %.S	$(COMSRCDIR):$(SRCDIR):$(PROCSRCDIR):$(NETSRCDIR)

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

all: force-tools tools $(TARGET) $(SRECFILE) $(UNCOMPRESS)

prom: all
	sh mkpromsrec
	sh mkpromsrec -P

install: all install.sh
	sh install.sh $(SRECFILE)
	touch install.sh

clean:
	rm -f $(OBJS) $(ROBJS) $(BOOTOBJS) $(LIBOBJS) $(UNCOBJS) banner.* \
	tools

clean_shared: 
	rm -f $(SHAREDOBJS)

clobber: clean
	rm -f $(TARGET)* $(UNCOMPRESS)* moncode* pfsi syms* 

superclean: clobber
	$(MAKE) --no-print-directory -C $(UTIL_DIR) clean
	$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc clean

moncode: $(TARGET) tools
	$(UTIL_DIR)/hxldsrc/hxld-$(NATIVE_OSREV) -bSr24 -f moncode $(TARGET)

force-tools:
	rm -f tools

tools:
	@$(MAKE) --no-print-directory -C $(UTIL_DIR) depend $(UTOOLS)
	@$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc depend $(HTOOLS)
	touch tools


$(TARGET): $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) makefile $(UTIL_DIR)/banner.sh \
	tools $(UTIL_DIR)/symbols_elf.sh
	@sh $(UTIL_DIR)/banner.sh AS5200
	$(CC) $(CFLAGS) -c banner.c
	$(LD) $(LDFLAGS) -o $(SUNFILE) $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) banner.o
	$(SIZE) $(SUNFILE)
	@$(NM) -p $(SUNFILE) | egrep -v "   U" > syms
	$(UTIL_DIR)/epromsum-$(NATIVE_OSREV) syms $(SUNFILE)
	@rm -f syms
	sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(SUNFILE) $(TARGET).symbols $(SUNFILE)
	cp $(SUNFILE) $(TARGET)
	$(STRIP) $(TARGET)

$(UNCOMPRESS): $(UNCOBJS) makefile $(UTIL_DIR)/symbols_elf.sh \
		tools 
	$(LD) $(UNCLDFLAGS) -o $(UNCOMPRESS) $(UNCOBJS)
	@$(SIZE) $(UNCOMPRESS)
	@$(NM) $(UNCOMPRESS) >$(UNCOMPRESS).symbols
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(UNCOMPRESS) $(UNCOMPRESS).symbols $(UNCOMPRESS)
	@$(STRIP) $(UNCOMPRESS)

$(SRECFILE): $(TARGET) moncode $(UNCOMPRESS) tools $(UTIL_DIR)/combpcode.sh
	$(UTIL_DIR)/mkffsi-$(NATIVE_OSREV) $(UNCOMPRESS) pfsi
	sh $(UTIL_DIR)/combpcode.sh -b moncode pfsi $(SRECFILE) >/dev/null
 
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
	@echo "Computing dependancies in rommon/obj-68-as5200"
	@$(CC) $(CPPOPTS) -M $(CBASE) $(ASM) > dependancies
	@echo "Computing TAGS file in rommon/obj-68-as5200"
	@etags -t $(CBASE) $(HFILES) $(ASM)
	@echo "Computing ID file in rommon/obj-68-as5200"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM)

dependancies:
	rm -f dependancies
	touch dependancies

include dependancies

