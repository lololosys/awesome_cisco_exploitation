# $Id: makefile,v 3.2.58.2 1996/05/08 11:57:50 mdb Exp $
# $Source: /release/112/cvs/Xsys/rommon/obj-4k-c4000/makefile,v $
#------------------------------------------------------------------
# makefile
#
# March 1994, Rob Clevenger
#
# Copyright (c) 1994-1996 by cisco Systems, Inc.
# All rights reserved.
# 
# Makefile to build the ROM Monitor for Sierra
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.2.58.2  1996/05/08  11:57:50  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.2.58.1  1996/03/21  23:25:05  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.2.2.1  1995/12/01  05:26:26  mbeesley
# Branch: ELC_branch
# Reorganize Rom monitor include files to allow easier porting.
#
# Revision 3.2  1995/11/17  18:40:19  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:04:03  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.3  1995/07/17  23:32:04  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Clean up bogus tool includes
#
# Revision 2.2  1995/06/17  01:00:52  clev
# CSCdi35421:  Add cache tests to ROM monitor core board diagnostics
#
# Revision 2.1  1995/06/07  22:36:11  hampton
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
include ../../makefile.4k

# override evil defaults from system settings
GNUM = -G 0 

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
SRCDIR = $(SYSROOT)/rommon/src-4k-c4000
#The processor specific source directory
PROCSRCDIR = $(SYSROOT)/rommon/src-4k
# The common source directory
COMSRCDIR = $(SYSROOT)/rommon/src
UTIL_DIR = $(SYSROOT)/rommon/utils

# Do not change the order of the includes - things will break
INC = -I$(INCDIR) -I$(PROCSRCDIR) -I$(SYSROOT)/src-4k-c4000 \
	-I$(SYSROOT)/rommon/src-4k-c4000 \
	-I$(SYSROOT)/h \
	-I$(SYSROOT)/src-4k \
	-I$(IROOT)/boot/src-c4000

DEFS=-DSIERRA
CPPOPTS=-nostdinc $(INC) ${DEFS}

CCOPTS= -g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp32 -mips2
ASOPTS= -g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp64 -mips3
CFLAGS= -O ${OPTIONS} ${CPPOPTS} ${CCOPTS}
ASFLAGS= -O ${OPTIONS} ${CPPOPTS} ${ASOPTS}

# Uncompress defines
UNCOMPRESS = uncompress
UNCSRCS    = $(COMSRCDIR)/sa_uncompress.c $(PROCSRCDIR)/scrt0_mips_unc.S
UNCOBJS    = scrt0_mips_unc.o sa_uncompress.o
UNCLDFLAGS = -e start $(GNUM) -N -Tsierraram.link -nostdlib -static

# TARGET is the stripped ELF binary file, ELFFILE is unstripped
TARGET=   C4500_RM
SYMFILE=  $(TARGET).sym
ELFFILE=  $(TARGET).elf
SRECFILE= $(TARGET).srec





#OBJDUMP= objdump.94q2.mips64 -h

LDFLAGS=-e evec -N $(GNUM) -Tsierrarom.link -nostdlib
RAMLDFLAGS= -e __start -N -Tsierraram.link -nostdlib -static

# ROM Based Object files : start.o must be first !

ROBJS = start.o main.o init.o

# The object files needed to give the image the power to boot

BOOTOBJS = monfsio.o simpfsio.o monlib_iface.o boot.o loadprog.o \
	ismagic.o device.o devtbl.o rawio.o fsutils.o

# The remaining object files

OBJS = moncmds.o monitor.o parsetoken.o shcmd.o memops.o alias.o builtin.o cpu_mips.o \
	nvram.o console.o mon_exceptn.o menu.o diag.o testmem.o query.o \
	error.o signal.o berrtest.o sizemem.o auxtest.o dis_mips.o \
	tlb.o dnld.o auxport.o cache_mips.o test2444.o brk_mips.o cookie.o \
	stk_mips.o timer.o tcalibrate.o nim.o imagetest.o confreg.o \
	testleds.o parity.o flash.o flashdrvr.o diagtools.o sierra_cycles.o \
	emulate.o stk_bcopy.o stk_bzero.o second_cache.o pri_cache.o

# The modules that are usually in the library are no longer

LIBOBJS = atoh.o getline.o getopt.o puts.o strings.o \
	bcmp.o getnum.o printf.o setjmp_mips.o

CBASE=	$(COMSRCDIR)/main.c $(COMSRCDIR)/monitor.c $(COMSRCDIR)/parsetoken.c \
	$(COMSRCDIR)/shcmd.c $(COMSRCDIR)/memops.c $(COMSRCDIR)/confreg.c \
	$(COMSRCDIR)/alias.c $(COMSRCDIR)/builtin.c $(COMSRCDIR)/nvram.c \
	$(COMSRCDIR)/console.c  $(COMSRCDIR)/menu.c $(COMSRCDIR)/cookie.c \
	$(COMSRCDIR)/testmem.c $(COMSRCDIR)/query.c $(COMSRCDIR)/error.c \
	$(COMSRCDIR)/signal.c $(COMSRCDIR)/berrtest.c $(SRCDIR)/moncmds.c \
	$(COMSRCDIR)/auxtest.c $(PROCSRCDIR)/dis_mips.c $(COMSRCDIR)/imagetest.c \
	$(PROCSRCDIR)/cpu_mips.c $(PROCSRCDIR)/tlb.c \
	$(COMSRCDIR)/diagtools.c $(COMSRCDIR)/dnld.c $(SRCDIR)/pri_cache.c \
	$(COMSRCDIR)/auxport.c $(PROCSRCDIR)/cache_mips.c $(SRCDIR)/second_cache.c \
	$(PROCSRCDIR)/brk_mips.c $(PROCSRCDIR)/stk_mips.c $(COMSRCDIR)/flash.c \
	$(SRCDIR)/tcalibrate.c $(SRCDIR)/nim.c $(SRCDIR)/init.c \
	$(SRCDIR)/mon_exceptn.c  $(SRCDIR)/diag.c $(SRCDIR)/sizemem.c \
	$(SRCDIR)/parity.c $(SRCDIR)/test2444.c $(SRCDIR)/timer.c \
	$(SRCDIR)/testleds.c $(SRCDIR)/flashdrvr.c \
	$(SRCDIR)/sierra_cycles.c $(SRCDIR)/emulate.c $(SRCDIR)/stk_bcopy.c \
	$(SRCDIR)/stk_bzero.c \
	$(COMSRCDIR)/monfsio.c $(COMSRCDIR)/simpfsio.c $(COMSRCDIR)/rawio.c \
	$(COMSRCDIR)/loadprog.c $(COMSRCDIR)/fsutils.c \
	$(COMSRCDIR)/monlib_iface.c $(COMSRCDIR)/boot.c $(COMSRCDIR)/ismagic.c \
	$(COMSRCDIR)/device.c \
	$(SRCDIR)/devtbl.c \
	$(COMSRCDIR)/atoh.c $(COMSRCDIR)/getline.c $(COMSRCDIR)/getopt.c \
	$(COMSRCDIR)/puts.c $(COMSRCDIR)/strings.c \
	$(COMSRCDIR)/bcmp.c $(COMSRCDIR)/getnum.c $(COMSRCDIR)/printf.c \
	$(COMSRCDIR)/sa_uncompress.c

HFILES=	$(SRCDIR)/nvmonvars.h $(INCDIR)/setjmp.h $(INCDIR)/mon_elf.h \
	$(INCDIR)/menu.h $(INCDIR)/queryflags.h $(INCDIR)/sh.h \
	$(INCDIR)/testmem.h $(INCDIR)/bitdef.h $(PROCSRCDIR)/cpu_mips.h \
	$(INCDIR)/mon_defs.h $(INCDIR)/dnld.h $(SRCDIR)/mon_plat_defs.h \
	$(INCDIR)/nonvol_x2444.h $(PROCSRCDIR)/dis_mips.h $(INCDIR)/mon_boot.h\
	$(SYSROOT)/src-4k/signal_4k.h $(INCDIR)/dev.h $(INCDIR)/dnld.h \
	$(INCDIR)/proto.h $(INCDIR)/eeprom.h $(INCDIR)/error.h \
	$(INCDIR)/strings.h $(INCDIR)/flash.h $(INCDIR)/simpfsio.h \
	$(INCDIR)/monitor.h $(INCDIR)/monfsio.h $(INCDIR)/fsio.h \
	$(SYSROOT)/src-4k-c4000/pcmap.h $(SRCDIR)/nim.h \
	$(IROOT)/boot/src-c4000/cookie.h $(SYSROOT)/src-4k/nevasic.h \
	$(INCDIR)/stdarg.h $(SYSROOT)/h/elf.h $(SYSROOT)/ts/s2681.h \
	$(INCDIR)/console.h $(SYSROOT)/h/types.h $(IROOT)/boot/cpu.h \
	$(INCDIR)/confreg.h $(INCDIR)/extern.h $(IROOT)/boot/buff.h \
	$(IROOT)/boot/emt.h $(IROOT)/boot/reason.h $(SYSROOT)/les/flash_fs_les.h

ASM=	$(SRCDIR)/start.S $(PROCSRCDIR)/setjmp_mips.S $(PROCSRCDIR)/scrt0_mips_unc.S

UTOOLS=  eprom_elf-$(NATIVE_OSREV) squish_elf-$(NATIVE_OSREV) \
	 mkffsi-$(NATIVE_OSREV)
HTOOLS=  hxld-$(NATIVE_OSREV)
TOOLS=   $(patsubst %,$(UTIL_DIR)/%,$(UTOOLS)) $(patsubst %,$(UTIL_DIR)/hxldsrc/%,$(HTOOLS))

vpath %.c	$(COMSRCDIR):$(SRCDIR):$(PROCSRCDIR)
vpath %.h	$(COMSRCDIR):$(SRCDIR):$(PROCSRCDIR):$(SYSROOT)/h:\
	$(SYSROOT)/src-4k:$(IROOT)/boot/src-c4000:\
	$(IROOT)/boot/src-68-c4000
vpath %.S	$(COMSRCDIR):$(SRCDIR):$(PROCSRCDIR)

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
	lister3 CC=$(CC) AS=$(AS) $< $(CFLAGS) | grep -v '.stab' >$*.lis

all: force-tools tools $(SRECFILE)

install: $(SRECFILE) install.sh
	sh install.sh $(SRECFILE)
	touch install.sh

clean:
	rm -f *.o banner.* tools

clobber: clean
	rm -f $(TARGET)* $(UNCOMPRESS)* moncode* pfsi syms*

superclean: clobber
	$(MAKE) --no-print-directory -C $(UTIL_DIR) clean
	$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc clean

moncode: $(TARGET) tools
	$(UTIL_DIR)/hxldsrc/hxld-$(NATIVE_OSREV) -r 32 -a 0  -f $@ $(TARGET)

force-tools:
	rm -f tools

tools:
	@$(MAKE) --no-print-directory -C $(UTIL_DIR) depend $(UTOOLS)
	@$(MAKE) --no-print-directory -C $(SYSROOT)/obj-4k-c4000 $(MKSUM_ELF_PROG)
	@$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc depend $(HTOOLS)
	touch tools


$(TARGET): $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) makefile $(UTIL_DIR)/banner.sh \
		sierrarom.link $(UTIL_DIR)/sym_elf.sh tools \
		$(UTIL_DIR)/symbols_elf.sh
	@sh $(UTIL_DIR)/banner.sh C4500
	$(CC) $(CFLAGS) -c banner.c
	$(LD) $(LDFLAGS) -o $(ELFFILE) $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) banner.o
	sh $(UTIL_DIR)/sym_elf.sh $(ELFFILE) >syms
	$(UTIL_DIR)/eprom_elf-$(NATIVE_OSREV) syms $(ELFFILE)
	@$(SIZE) $(ELFFILE)
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(ELFFILE) $(TARGET).symbols $(ELFFILE)
	@cp $(ELFFILE) $(TARGET)
	@$(STRIP) $(TARGET)

$(UNCOMPRESS): $(UNCOBJS) makefile sierraram.link $(UTIL_DIR)/symbols_elf.sh \
		tools 
	$(LD) $(UNCLDFLAGS) -o $(UNCOMPRESS) $(UNCOBJS)
	@$(SIZE) $(UNCOMPRESS)
	@$(NM) $(UNCOMPRESS) >$(UNCOMPRESS).symbols
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(UNCOMPRESS) $(UNCOMPRESS).symbols $(UNCOMPRESS)
	@$(STRIP) $(UNCOMPRESS)
	# Place SIERRA machine id in ELF header
	$(SYSROOT)/obj-4k-c4000/$(MKSUM_ELF_PROG) $(UNCOMPRESS) -machine
	$(UTIL_DIR)/squish_elf-$(NATIVE_OSREV) $(UNCOMPRESS) temp
	@mv temp $(UNCOMPRESS)

$(SRECFILE): $(UNCOMPRESS) moncode tools $(UTIL_DIR)/combpcode.sh
	$(UTIL_DIR)/mkffsi-$(NATIVE_OSREV) $(UNCOMPRESS) pfsi
	sh $(UTIL_DIR)/combpcode.sh moncode pfsi $(SRECFILE) >/dev/null

#
# The include file dependencies
#
depend: dependancies $(CBASE) $(HFILES) $(ASM)
	@echo "Computing dependancies in rommon/obj-4k-c4000"
	@$(CC) $(CPPOPTS) -M $(CBASE) $(ASM) > dependancies
	@echo "Computing TAGS file in rommon/obj-4k-c4000"
	@etags -t $(CBASE) $(HFILES) $(ASM)
	@echo "Computing ID file in rommon/obj-4k-c4000"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM)

dependancies:
	rm -f dependancies
	touch dependancies

include dependancies
