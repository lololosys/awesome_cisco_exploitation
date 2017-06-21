# $Id: makefile,v 3.2.58.4 1996/05/08 11:57:52 mdb Exp $
# $Source: /release/112/cvs/Xsys/rommon/obj-4k-c7100/makefile,v $
#------------------------------------------------------------------
# makefile -- Predator Rom Monitor makefile
#
# July 1995, Michael Beesley
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.2.58.4  1996/05/08  11:57:52  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.2.58.3  1996/04/30  17:25:05  szhang
# CSCdi55355:  fault tolerant boot strategy causes boot cycle
# Branch: California_branch
#
# Revision 3.2.58.2  1996/04/09  23:09:09  szhang
# CSCdi53689:  Open() failed in PCMCIA monlib
# Branch: California_branch
# Monitor shouldn't reset the PCMCIA controller after the MONLIB
# is launched.
#
# Revision 3.2.58.1  1996/03/21  23:25:25  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.2.2.7  1996/02/16  21:41:42  szhang
# CSCdi49173:  Separate the startup and exception code
# Branch: ELC_branch
# Separate the startup and exception code into platform
# dependent and platform independent files.
#
# Revision 3.2.2.6  1996/02/12  19:54:33  mbeesley
# Branch: ELC_branch
# Delete line to copy srecord file to tftpboot.
#
# Revision 3.2.2.5  1996/01/16  22:50:04  szhang
# Branch: ELC_branch
# Use the MONLIB drivers with file rsp_rawio.c.
#
# Revision 3.2.2.4  1995/12/05  23:12:09  mbeesley
# Branch: ELC_branch
# Add complete support for discontigous NVRAM.
#
# Revision 3.2.2.3  1995/12/01  05:26:32  mbeesley
# Branch: ELC_branch
# Reorganize Rom monitor include files to allow easier porting.
#
# Revision 3.2.2.2  1995/11/23  23:09:23  szhang
# Branch: ELC_branch
# Added back the utility commands to convert the .ELF file into SREC
# file.
#
# Revision 3.2.2.1  1995/11/23  19:15:08  szhang
# Branch: ELC_branch
# Initial source code commit to ELC branch for Predator (C7100) ROM
# monitor.
#
# Revision 3.2  1995/11/17  18:40:25  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  13:04:11  shaker
# Bump version numbers from 2.x to 3.x.
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

SHELL =		/bin/sh
OPTIONS =
IROOT = 	../../..
INCDIR = 	$(SYSROOT)/rommon/src

#
# The platform specific source directory
#
PREDDIR =	$(SYSROOT)/rommon/src-4k-c7100
RSPDIR =	$(SYSROOT)/rommon/src-4k-rsp

#
# The processor specific source directory
#
PROCSRCDIR =	$(SYSROOT)/rommon/src-4k

#
# The common source directory
#
COMSRCDIR = 	$(SYSROOT)/rommon/src
FILESYSDIR = 	$(SYSROOT)/filesys
UTIL_DIR = 	$(SYSROOT)/rommon/utils

#
# Do not change the order of the includes - things will break
#
INC = 	-I$(INCDIR) -I$(PROCSRCDIR) \
	-I$(PREDDIR) \
	-I$(SYSROOT)/h \
	-I$(SYSROOT)/src-4k \
	-I$(IROOT)/boot/src-c4000 -I$(FILESYSDIR) -I$(SYSROOT)/machine \
	-I$(SYSROOT)/lib/cisco/include

DEFS = 	-DPREDATOR -DC7100_ROM -DFSLIB_NO_DEBUG -DOBJECT_4K -DTARGET_CISCO
CPPOPTS=-nostdinc $(INC) ${DEFS}

CCOPTS =	-g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp32 -mips2
ASOPTS = 	-g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp64 -mips3
CFLAGS = 	-O ${OPTIONS} ${CPPOPTS} ${CCOPTS}
ASFLAGS = 	-O ${OPTIONS} ${CPPOPTS} ${ASOPTS}

#
# Uncompress defines
#
UNCOMPRESS = 	uncompress
UNCSRCS = 	$(COMSRCDIR)/sa_uncompress.c $(PROCSRCDIR)/scrt0_mips_unc.S
UNCOBJS = 	scrt0_mips_unc.o sa_uncompress.o
UNCLDFLAGS = 	-e start $(GNUM) -N -Tpred_ram.link -nostdlib -static

#
# TARGET is the stripped ELF binary file, ELFFILE is unstripped
#
TARGET =   	C7200_RM
SYMFILE =  	$(TARGET).sym
ELFFILE = 	$(TARGET).elf
SRECFILE = 	$(TARGET).srec
SIMFILE =  	$(TARGET).sim

#
# Loader flags
#
LDFLAGS =	-e evec -N $(GNUM) -Tpred_rom.link -nostdlib
RAMLDFLAGS = 	-e __start -N -Tpred_ram.link -nostdlib -static

#
# ROM Based Object files : start.o must be first !
#
ROBJS = pred_start.o main.o pred_init.o utils_mips.o

# The object files needed to give the image the power to boot

BOOTOBJS = monfsio.o simpfsio.o monlib_iface.o pred_boot.o loadprog.o \
	ismagic.o device.o rsp_rawio.o fsutils.o clpd6729.o \
	c7100_ralib.o 

# The remaining object files

OBJS = 	pred_moncmds.o monitor.o parsetoken.o shcmd.o memops.o alias.o \
	builtin.o cpu_mips.o pred_nvram.o console.o exceptn_mips.o \
	pred_mon_exceptn.o testmem.o query.o \
	error.o signal.o berrtest.o sizemem.o auxtest.o dis_mips.o \
	tlb.o dnld.o auxport.o cache_mips.o  brk_mips.o \
	stk_mips.o imagetest.o confreg.o \
	flash.o diagtools.o pred_emulate.o \
	stk_bcopy.o stk_bzero.o pred_eeprom.o nmc93c46.o

# The modules that are usually in the library are no longer

LIBOBJS = atoh.o getline.o getopt.o puts.o strings.o \
	bcmp.o getnum.o printf.o setjmp_mips.o

CBASE=	$(COMSRCDIR)/main.c \
	$(COMSRCDIR)/monitor.c \
	$(COMSRCDIR)/parsetoken.c \
	$(COMSRCDIR)/shcmd.c \
	$(COMSRCDIR)/memops.c \
	$(COMSRCDIR)/confreg.c \
	$(COMSRCDIR)/alias.c \
	$(COMSRCDIR)/builtin.c \
	$(COMSRCDIR)/console.c \
	$(COMSRCDIR)/testmem.c \
	$(COMSRCDIR)/query.c \
	$(COMSRCDIR)/error.c \
	$(COMSRCDIR)/signal.c \
	$(COMSRCDIR)/berrtest.c \
	$(COMSRCDIR)/auxtest.c \
	$(COMSRCDIR)/imagetest.c \
	$(COMSRCDIR)/diagtools.c \
	$(COMSRCDIR)/dnld.c \
	$(COMSRCDIR)/auxport.c \
	$(COMSRCDIR)/flash.c \
	$(COMSRCDIR)/monfsio.c \
	$(COMSRCDIR)/simpfsio.c \
	$(COMSRCDIR)/loadprog.c \
	$(COMSRCDIR)/fsutils.c \
	$(COMSRCDIR)/monlib_iface.c \
	$(COMSRCDIR)/ismagic.c \
	$(COMSRCDIR)/device.c \
	$(COMSRCDIR)/atoh.c \
	$(COMSRCDIR)/getline.c \
	$(COMSRCDIR)/getopt.c \
	$(COMSRCDIR)/puts.c \
	$(COMSRCDIR)/strings.c \
	$(COMSRCDIR)/bcmp.c \
	$(COMSRCDIR)/getnum.c \
	$(COMSRCDIR)/printf.c \
	$(COMSRCDIR)/sa_uncompress.c \
	$(COMSRCDIR)/nmc93c46.c \
	$(PROCSRCDIR)/cache_mips.c \
	$(PROCSRCDIR)/brk_mips.c \
	$(PROCSRCDIR)/stk_mips.c \
	$(PROCSRCDIR)/cpu_mips.c \
	$(PROCSRCDIR)/tlb.c \
	$(PROCSRCDIR)/dis_mips.c \
	$(PREDDIR)/pred_nvram.c \
	$(PREDDIR)/pred_moncmds.c \
	$(PREDDIR)/pred_init.c \
	$(PREDDIR)/pred_emulate.c \
	$(PROCSRCDIR)/exceptn_mips.c \
	$(PREDDIR)/pred_mon_exceptn.c \
	$(PREDDIR)/sizemem.c \
	$(PREDDIR)/pred_eeprom.c \
	$(RSPDIR)/stk_bcopy.c \
	$(RSPDIR)/stk_bzero.c \
	$(RSPDIR)/rsp_rawio.c \
	$(PREDDIR)/pred_boot.c \
	$(FILESYSDIR)/clpd6729.c \
	$(FILESYSDIR)/c7100_ralib.c

HFILES=	$(INCDIR)/monitor.h \
	$(INCDIR)/setjmp.h \
	$(INCDIR)/mon_elf.h \
	$(INCDIR)/queryflags.h \
	$(INCDIR)/sh.h \
	$(INCDIR)/testmem.h \
	$(INCDIR)/bitdef.h \
	$(INCDIR)/mon_defs.h \
	$(INCDIR)/dnld.h \
	$(INCDIR)/mon_boot.h \
	$(INCDIR)/dev.h \
	$(INCDIR)/proto.h \
	$(INCDIR)/eeprom.h \
	$(INCDIR)/error.h \
	$(INCDIR)/strings.h \
	$(INCDIR)/flash.h \
	$(INCDIR)/simpfsio.h \
	$(INCDIR)/monfsio.h \
	$(INCDIR)/fsio.h \
	$(INCDIR)/stdarg.h \
	$(INCDIR)/console.h \
	$(INCDIR)/confreg.h \
	$(INCDIR)/extern.h \
	$(PROCSRCDIR)/cpu_mips.h \
	$(PROCSRCDIR)/dis_mips.h \
	$(SYSROOT)/h/elf.h \
	$(SYSROOT)/h/types.h \
	$(SYSROOT)/les/flash_fs_les.h \
	$(SYSROOT)/hes/dgbus.h \
	$(SYSROOT)/src-4k/signal_4k.h \
	$(SYSROOT)/src-4k/vspace_4k.h \
	$(SYSROOT)/ts/s2681.h \
	$(IROOT)/boot/cpu.h \
	$(IROOT)/boot/buff.h \
	$(IROOT)/boot/emt.h \
	$(IROOT)/boot/reason.h \
	$(IROOT)/boot/types.h \
	$(IROOT)/boot/src-c4000/cookie.h \
	$(PREDDIR)/pred_plat_defs.h \
	$(PREDDIR)/pred_nvmonvars.h \
	$(FILESYSDIR)/clpd6720.h \
	$(FILESYSDIR)/clpd6729.h \
	$(FILESYSDIR)/clpd6720_internals.h \
	$(FILESYSDIR)/filesys.h \
	$(FILESYSDIR)/fslib.h \
	$(FILESYSDIR)/fslib_internals.h \
	$(FILESYSDIR)/fslib_print.h \
	$(FILESYSDIR)/ralib.h \
	$(FILESYSDIR)/ralib_internals.h

ASM =	$(PREDDIR)/pred_start.S $(PROCSRCDIR)/utils_mips.S \
	$(PROCSRCDIR)/setjmp_mips.S $(PROCSRCDIR)/scrt0_mips_unc.S

UTOOLS =	eprom_elf-$(NATIVE_OSREV) squish_elf-$(NATIVE_OSREV) \
		mkffsi-$(NATIVE_OSREV)
HTOOLS =  	hxld-$(NATIVE_OSREV)
TOOLS =   	$(patsubst %,$(UTIL_DIR)/%,$(UTOOLS)) $(patsubst \
		%,$(UTIL_DIR)/hxldsrc/%,$(HTOOLS))

vpath %.c	$(COMSRCDIR):$(PREDDIR):$(RSPDIR):$(PROCSRCDIR):$(FILESYSDIR)

vpath %.h	$(COMSRCDIR):(PREDDIR):$(PROCSRCDIR):$(SYSROOT)/h:\
		$(SYSROOT)/src-4k:\
		$(IROOT)/boot/src-c4000:\
		$(FILESYSDIR)

vpath %.S	$(PREDDIR):$(PROCSRCDIR)

#
# Override RCS stuff
#

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
	$(UTIL_DIR)/hxldsrc/hxld-$(NATIVE_OSREV) -r 32 -a 0 -f $@ $(TARGET)

force-tools:
	rm -f tools

tools:
	@$(MAKE) --no-print-directory -C $(UTIL_DIR) depend $(UTOOLS)
	@$(MAKE) --no-print-directory -C $(SYSROOT)/obj-4k-c7100 $(MKSUM_ELF_PROG)
	@$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc depend $(HTOOLS)
	touch tools

$(TARGET): $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) makefile \
		$(UTIL_DIR)/banner.sh \
		pred_rom.link $(UTIL_DIR)/sym_elf.sh tools \
		$(UTIL_DIR)/symbols_elf.sh
	@sh $(UTIL_DIR)/banner.sh C7200
	$(CC) $(CFLAGS) -c banner.c
	$(LD) $(LDFLAGS) -o $(ELFFILE) $(ROBJS) $(OBJS) $(BOOTOBJS) \
		$(LIBOBJS) banner.o
	cp $(TARGET).elf $(TARGET).nm.src
	sh $(UTIL_DIR)/sym_elf.sh $(ELFFILE) >syms
	$(UTIL_DIR)/eprom_elf-$(NATIVE_OSREV) syms $(ELFFILE)
	@$(SIZE) $(ELFFILE)
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(ELFFILE) $(TARGET).symbols \
		$(ELFFILE)
	@cp $(ELFFILE) $(TARGET)
	@$(STRIP) $(TARGET)

$(UNCOMPRESS): $(UNCOBJS) makefile pred_ram.link $(UTIL_DIR)/symbols_elf.sh \
		tools 
	$(LD) $(UNCLDFLAGS) -o $(UNCOMPRESS) $(UNCOBJS)
	@$(SIZE) $(UNCOMPRESS)
	@$(NM) $(UNCOMPRESS) >$(UNCOMPRESS).symbols
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(UNCOMPRESS) \
		$(UNCOMPRESS).symbols $(UNCOMPRESS)
	@$(STRIP) $(UNCOMPRESS)
	# Place C7100 machine id in ELF header
	$(SYSROOT)/obj-4k-c7100/$(MKSUM_ELF_PROG) $(UNCOMPRESS) -machine
	$(UTIL_DIR)/squish_elf-$(NATIVE_OSREV) $(UNCOMPRESS) temp
	@mv temp $(UNCOMPRESS)

$(SIMFILE): $(TARGET)
	exe2sim -I ELF -O LAI -B 1 -i $(TARGET) -o $(SIMFILE) -A bfc00000 \
		-S 00040000
	$(UTIL_DIR)/split_rom-$(NATIVE_OSREV) < $(TARGET).sim.dat0

$(SRECFILE): $(UNCOMPRESS) moncode tools $(UTIL_DIR)/combpcode.sh
	$(UTIL_DIR)/mkffsi-$(NATIVE_OSREV) $(UNCOMPRESS) pfsi
	sh $(UTIL_DIR)/combpcode.sh moncode pfsi $(SRECFILE) >/dev/null

#
# The include file dependencies
#
depend: dependancies $(CBASE) $(HFILES) $(ASM)
	@echo "Computing dependancies in rommon/obj-4k-c7100"
	@$(CC) $(CPPOPTS) -M $(CBASE) $(ASM) > dependancies
	@echo "Computing TAGS file in rommon/obj-4k-c7100"
	@etags -t $(CBASE) $(HFILES) $(ASM)
	@echo "Computing ID file in rommon/obj-4k-c7100"
	@mkid '-S.S=asm' '-Sasm-c|' '-Sasm+u' '-Sasm+p' $(CBASE) $(HFILES) \
		$(ASM)

dependancies:
	rm -f dependancies
	touch dependancies

include dependancies


