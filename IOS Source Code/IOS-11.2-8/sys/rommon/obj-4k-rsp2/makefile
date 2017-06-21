# $Id: makefile,v 3.4.20.3 1996/05/08 11:57:57 mdb Exp $
# $Source: /release/112/cvs/Xsys/rommon/obj-4k-rsp2/makefile,v $
#------------------------------------------------------------------
# makefile
#
# 05/95 Steve J. Zhang
#
# Copyright (c) 1995 - 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.4.20.3  1996/05/08  11:57:57  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.4.20.2  1996/03/21  23:26:01  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.4.20.1  1996/03/18  21:46:02  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.2.2.2  1996/02/08  08:57:08  gstovall
# Branch: ELC_branch
# Sync to ELC_baseline_960205.
#
# Revision 3.2.2.1  1995/12/01  05:26:36  mbeesley
# Branch: ELC_branch
# Reorganize Rom monitor include files to allow easier porting.
#
# Revision 3.4  1996/02/01  02:26:54  mansonw
# CSCdi47569:  HSA slave refuses to switch to master state
# Fixed the HSA slave to gain master during hot insertion/removal
#
# Revision 3.3  1996/01/17  23:27:19  mansonw
# CSCdi47058:  Fix the broken ROMMON build
#
# Revision 3.2  1995/11/17  18:40:35  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:04:21  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.2  1995/07/17  23:32:09  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Clean up bogus tool includes
#
# Revision 2.1  1995/06/07  22:36:25  hampton
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
RSPDIR = $(SYSROOT)/rommon/src-4k-rsp
RSP2DIR = $(SYSROOT)/rommon/src-4k-rsp2
#The processor specific source directory
PROCSRCDIR = $(SYSROOT)/rommon/src-4k
# The common source directory
COMSRCDIR = $(SYSROOT)/rommon/src
FILESYSDIR = $(SYSROOT)/filesys
UTIL_DIR = $(SYSROOT)/rommon/utils

# Do not change the order of the includes - things will break
INC = -I$(INCDIR) -I$(PROCSRCDIR) -I$(SYSROOT)/src-rsp \
	-I$(RSPDIR) -I$(RSP2DIR) \
	-I$(SYSROOT)/h \
	-I$(SYSROOT)/src-4k \
	-I$(IROOT)/boot/src-c4000 -I$(FILESYSDIR) -I$(SYSROOT)/machine \
	-I$(SYSROOT)/lib/cisco/include

DEFS= -DRSP -DRSP2 -DRSP_ROM -DFSLIB_NO_DEBUG -DOBJECT_4K -DTARGET_CISCO
CPPOPTS=-nostdinc $(INC) ${DEFS}

CCOPTS= -g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp32 -mips2
ASOPTS= -g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp64 -mips3
CFLAGS= -O ${OPTIONS} ${CPPOPTS} ${CCOPTS}
ASFLAGS= -O ${OPTIONS} ${CPPOPTS} ${ASOPTS}

# Uncompress defines
UNCOMPRESS = uncompress
UNCSRCS    = $(COMSRCDIR)/sa_uncompress.c $(PROCSRCDIR)/scrt0_mips_unc.S
UNCOBJS    = scrt0_mips_unc.o sa_uncompress.o
UNCLDFLAGS = -e start $(GNUM) -N -Trsp2ram.link -nostdlib -static

# dbus image defines
DBUS_IMAGE = dbus_image
DBUSLDFLAGS = -e start $(GNUM) -N -Trsp2dbus.link -nostdlib -static

# TARGET is the stripped ELF binary file, ELFFILE is unstripped
TARGET=   RSP2_RM
SYMFILE=  $(TARGET).sym
ELFFILE=  $(TARGET).elf
SRECFILE= $(TARGET).srec



#OBJDUMP= objdump.94q2.mips64 -h

LDFLAGS=-e evec -N $(GNUM) -Trsp2rom.link -nostdlib
RAMLDFLAGS= -e __start -N -Trsp2ram.link -nostdlib -static

# ROM Based Object files : start.o must be first !

ROBJS = rsp2_start.o main.o rsp2_init.o

# The object files needed to give the image the power to boot

BOOTOBJS = monfsio.o simpfsio.o monlib_iface.o rsp_boot.o loadprog.o \
	ismagic.o device.o devtbl.o rsp_rawio.o fsutils.o clpd6720.o \
	rsp_ralib.o mon_ralib.o

# The object files needed to support DBUS download and virtual console

DBUSOBJS = dbus_start.o dbus_main.o handler.o command.o control.o \
	   dbus_output.o dbus_io.o compress_lzw.o dbus_exceptn.o 

# The remaining object files

OBJS = rsp2_moncmds.o monitor.o parsetoken.o shcmd.o memops.o alias.o builtin.o \
	cpu_mips.o nvram.o console.o rsp2_mon_exceptn.o testmem.o query.o \
	error.o signal.o berrtest.o sizemem.o auxtest.o dis_mips.o \
	tlb.o dnld.o auxport.o cache_mips.o  brk_mips.o \
	stk_mips.o timer.o tcalibrate.o imagetest.o confreg.o \
	testleds.o flash.o flashdrvr.o diagtools.o  \
	emulate.o stk_bcopy.o stk_bzero.o ideeprom.o rsp_macaddr.o rsp2_ms.o \
	vconsole.o

# The modules that are usually in the library are no longer

LIBOBJS = atoh.o getline.o getopt.o puts.o strings.o \
	bcmp.o getnum.o printf.o setjmp_mips.o

CBASE=	$(COMSRCDIR)/main.c $(COMSRCDIR)/monitor.c $(COMSRCDIR)/parsetoken.c \
	$(COMSRCDIR)/shcmd.c $(COMSRCDIR)/memops.c $(COMSRCDIR)/confreg.c \
	$(COMSRCDIR)/alias.c $(COMSRCDIR)/builtin.c $(COMSRCDIR)/nvram.c \
	$(COMSRCDIR)/console.c \
	$(COMSRCDIR)/testmem.c $(COMSRCDIR)/query.c $(COMSRCDIR)/error.c \
	$(COMSRCDIR)/signal.c $(COMSRCDIR)/berrtest.c $(RSP2DIR)/rsp2_moncmds.c \
	$(COMSRCDIR)/auxtest.c $(PROCSRCDIR)/dis_mips.c $(COMSRCDIR)/imagetest.c \
	$(PROCSRCDIR)/cpu_mips.c $(PROCSRCDIR)/tlb.c \
	$(COMSRCDIR)/diagtools.c $(COMSRCDIR)/dnld.c \
	$(COMSRCDIR)/auxport.c $(PROCSRCDIR)/cache_mips.c \
	$(PROCSRCDIR)/brk_mips.c $(PROCSRCDIR)/stk_mips.c $(COMSRCDIR)/flash.c \
	$(RSPDIR)/tcalibrate.c  $(RSP2DIR)/rsp2_init.c \
	$(RSP2DIR)/rsp2_mon_exceptn.c  $(RSPDIR)/sizemem.c \
	$(RSPDIR)/timer.c $(RSPDIR)/testleds.c $(RSPDIR)/flashdrvr.c \
	$(RSPDIR)/emulate.c $(RSPDIR)/stk_bcopy.c $(RSPDIR)/stk_bzero.c \
	$(COMSRCDIR)/monfsio.c $(COMSRCDIR)/simpfsio.c $(RSPDIR)/rsp_rawio.c \
	$(COMSRCDIR)/loadprog.c $(COMSRCDIR)/fsutils.c \
	$(COMSRCDIR)/monlib_iface.c $(RSPDIR)/rsp_boot.c $(COMSRCDIR)/ismagic.c \
	$(COMSRCDIR)/device.c \
	$(RSPDIR)/devtbl.c $(RSPDIR)/ideeprom.c $(RSPDIR)/rsp_macaddr.c \
	$(COMSRCDIR)/atoh.c $(COMSRCDIR)/getline.c $(COMSRCDIR)/getopt.c \
	$(COMSRCDIR)/puts.c $(COMSRCDIR)/strings.c \
	$(COMSRCDIR)/bcmp.c $(COMSRCDIR)/getnum.c $(COMSRCDIR)/printf.c \
	$(COMSRCDIR)/sa_uncompress.c $(FILESYSDIR)/clpd6720.c \
	$(FILESYSDIR)/rsp_ralib.c $(RSPDIR)/mon_ralib.c $(RSP2DIR)/rsp2_ms.c \
	$(RSP2DIR)/vconsole.c $(COMSRCDIR)/dbus_io.c \
        $(COMSRCDIR)/handler.c $(COMSRCDIR)/command.c $(COMSRCDIR)/control.c \
	$(COMSRCDIR)/compress_lzw.c $(COMSRCDIR)/dbus_output.c $(COMSRCDIR)/dbus_main.c \
        $(COMSRCDIR)/dbus_exceptn.c

HFILES=	$(RSPDIR)/nvmonvars.h $(INCDIR)/setjmp.h $(INCDIR)/mon_elf.h \
	$(INCDIR)/queryflags.h $(INCDIR)/sh.h \
	$(INCDIR)/testmem.h $(INCDIR)/bitdef.h $(PROCSRCDIR)/cpu_mips.h \
	$(INCDIR)/mon_defs.h $(INCDIR)/dnld.h $(RSPDIR)/mon_plat_defs.h \
	$(INCDIR)/nonvol_x2444.h $(PROCSRCDIR)/dis_mips.h $(INCDIR)/mon_boot.h\
	$(SYSROOT)/src-4k/signal_4k.h $(INCDIR)/dev.h $(INCDIR)/dnld.h \
	$(INCDIR)/proto.h $(INCDIR)/eeprom.h $(INCDIR)/error.h \
	$(INCDIR)/strings.h $(INCDIR)/flash.h $(INCDIR)/simpfsio.h \
	$(INCDIR)/monitor.h $(INCDIR)/monfsio.h $(INCDIR)/fsio.h \
	$(SYSROOT)/src-rsp/rsp_pcmap.h $(RSPDIR)/pcmap.h \
	$(INCDIR)/stdarg.h $(SYSROOT)/h/elf.h $(SYSROOT)/ts/s2681.h \
	$(INCDIR)/console.h $(SYSROOT)/h/types.h $(IROOT)/boot/cpu.h \
	$(INCDIR)/confreg.h $(INCDIR)/extern.h $(IROOT)/boot/buff.h \
	$(IROOT)/boot/emt.h $(IROOT)/boot/reason.h $(SYSROOT)/les/flash_fs_les.h \
	$(RSPDIR)/dclasic.h $(RSPDIR)/memdasic.h \
	$(RSPDIR)/regfpga.h $(SYSROOT)/hes/dgbus.h \
	$(FILESYSDIR)/clpd6720.h \
	$(FILESYSDIR)/clpd6720_internals.h \
	$(FILESYSDIR)/filesys.h $(FILESYSDIR)/fslib.h \
	$(FILESYSDIR)/fslib_internals.h \
	$(FILESYSDIR)/fslib_print.h $(FILESYSDIR)/ralib.h \
	$(FILESYSDIR)/ralib_internals.h \
	$(IROOT)/boot/src-c4000/cookie.h $(RSPDIR)/mon_ralib.h \
	$(COMSRCDIR)/command.h $(COMSRCDIR)/dbstatus.h \
	$(COMSRCDIR)/queue.h $(COMSRCDIR)/compress_lzw.h $(COMSRCDIR)/dbus.h


ASM=	$(RSP2DIR)/rsp2_start.S $(PROCSRCDIR)/setjmp_mips.S $(PROCSRCDIR)/scrt0_mips_unc.S \
	$(COMSRCDIR)/dbus_start.S

UTOOLS=  eprom_elf squish_elf mkffsi
HTOOLS=  hxld
TOOLS=   $(patsubst %,$(UTIL_DIR)/%,$(UTOOLS)) $(patsubst %,$(UTIL_DIR)/hxldsrc/%,$(HTOOLS))

vpath %.c	$(COMSRCDIR):$(RSPDIR):$(RSP2DIR):$(PROCSRCDIR):$(FILESYSDIR)
vpath %.h	$(COMSRCDIR):$(RSPDIR):(RSP2DIR):$(PROCSRCDIR):$(SYSROOT)/h:\
	$(SYSROOT)/src-4k:$(IROOT)/boot/src-c4000:\
	$(IROOT)/boot/src-68-c4000 $(FILESYSDIR)
vpath %.S	$(COMSRCDIR):$(RSPDIR):$(RSP2DIR):$(PROCSRCDIR)

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
#	lister3 CC=$(CC) AS=$(AS) $< $(CFLAGS) | grep -v '.stab' >$*.lis
	lister3 CC=$(CC) AS=$(AS) $< $(CFLAGS) > $@

all: force-tools tools $(SRECFILE)

dbus: $(DBUS_IMAGE)

install: $(SRECFILE) install.sh
	sh install.sh $(SRECFILE)
	touch install.sh

clean:
	rm -f $(OBJS) $(ROBJS) $(BOOTOBJS) $(LIBOBJS) $(UNCOBJS) $(DBUSOBJS) banner.* \
	tools

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
	@$(MAKE) --no-print-directory -C $(SYSROOT)/obj-4k-rsp $(MKSUM_ELF_PROG)
	@$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc depend $(HTOOLS)
	touch tools


$(TARGET): $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) makefile $(UTIL_DIR)/banner.sh \
		rsp2rom.link $(UTIL_DIR)/sym_elf.sh tools \
		$(UTIL_DIR)/symbols_elf.sh
	@sh $(UTIL_DIR)/banner.sh RSP2
	$(CC) $(CFLAGS) -c banner.c
	$(LD) $(LDFLAGS) -o $(ELFFILE) $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) banner.o
	cp $(TARGET).elf $(TARGET).nm.src
	sh $(UTIL_DIR)/sym_elf.sh $(ELFFILE) >syms
	$(UTIL_DIR)/eprom_elf-$(NATIVE_OSREV) syms $(ELFFILE)
	@$(SIZE) $(ELFFILE)
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(ELFFILE) $(TARGET).symbols $(ELFFILE)
	@cp $(ELFFILE) $(TARGET)
	@$(STRIP) $(TARGET)

$(UNCOMPRESS): $(UNCOBJS) makefile rsp2ram.link $(UTIL_DIR)/symbols_elf.sh \
		tools 
	$(LD) $(UNCLDFLAGS) -o $(UNCOMPRESS) $(UNCOBJS)
	@$(SIZE) $(UNCOMPRESS)
	@$(NM) $(UNCOMPRESS) >$(UNCOMPRESS).symbols
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(UNCOMPRESS) $(UNCOMPRESS).symbols $(UNCOMPRESS)
	@$(STRIP) $(UNCOMPRESS)
	# Place RSP machine id in ELF header
	$(SYSROOT)/obj-4k-rsp/$(MKSUM_ELF_PROG) $(UNCOMPRESS) -machine
	$(UTIL_DIR)/squish_elf-$(NATIVE_OSREV) $(UNCOMPRESS) temp
	@mv temp $(UNCOMPRESS)

$(DBUS_IMAGE): $(DBUSOBJS) makefile rsp2ram.link $(UTIL_DIR)/symbols_elf.sh tools
	$(LD) $(DBUSLDFLAGS) -o $(DBUS_IMAGE) $(DBUSOBJS)
	@$(SIZE) $(DBUS_IMAGE)
	cp $(DBUS_IMAGE) $(DBUS_IMAGE).nm.src
	@$(NM) $(DBUS_IMAGE) >$(DBUS_IMAGE).symbols
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(DBUS_IMAGE) $(DBUS_IMAGE).symbols $(DBUS_IMAGE)
	@$(STRIP) $(DBUS_IMAGE)
	# Place RSP machine id in ELF header
	$(SYSROOT)/obj-4k-rsp/$(MKSUM_ELF_PROG) $(DBUS_IMAGE) -machine
	$(UTIL_DIR)/squish_elf-$(NATIVE_OSREV) $(DBUS_IMAGE) temp
	@mv temp $(DBUS_IMAGE)

$(SRECFILE): $(UNCOMPRESS) $(DBUS_IMAGE) moncode tools $(UTIL_DIR)/combpcode.sh
	$(UTIL_DIR)/mkffsi-$(NATIVE_OSREV) $(UNCOMPRESS) $(DBUS_IMAGE) pfsi
	sh $(UTIL_DIR)/combpcode.sh moncode pfsi $(SRECFILE) >/dev/null
	cp $(SRECFILE) /tftpboot/$(USER)/$(TARGET)

#
# The include file dependencies
#
depend: dependancies $(CBASE) $(HFILES) $(ASM)
	$(CC) $(CPPOPTS) -M $(CBASE) $(ASM) > dependancies
	etags -t $(CBASE) $(HFILES) $(ASM)
	mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) $(ASM)

dependancies:
	rm -f dependancies
	touch dependancies

include dependancies






