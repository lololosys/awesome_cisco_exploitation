# $Id: makefile,v 1.1.8.1 1996/07/08 22:42:19 mwu Exp $
# $Source: /release/112/cvs/Xsys/rommon/obj-4k-svip/makefile,v $
#------------------------------------------------------------------
# makefile
#
# Mar, 96. Paul Greenfield
#
# Copyright (c) 1995-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 1.1.8.1  1996/07/08  22:42:19  mwu
# CSCdi60922:  11.2 Core Features Commit
# Branch: California_branch
#
# Revision 1.1  1996/03/23  01:14:01  pgreenfi
# Placeholder files for obj-4k-svip
#
#------------------------------------------------------------------
# $Endlog$
#

DIS	= /router/bin/objdump.94q2.mips64 -d -j .text -f -l -r

default: all

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
INCDIR= $(IROOT)/sys/rommon/src
# The platform specific source directory
SVIPDIR = $(IROOT)/sys/rommon/src-4k-svip
#The processor specific source directory
PROCSRCDIR = $(IROOT)/sys/rommon/src-4k
# The common source directory
COMSRCDIR = $(IROOT)/sys/rommon/src
FILESYSDIR = $(IROOT)/sys/filesys
UTIL_DIR = $(IROOT)/sys/rommon/utils

# Do not change the order of the includes - things will break
INC = -I$(INCDIR) -I$(PROCSRCDIR) -I$(IROOT)/sys/src-svip \
	-I$(SVIPDIR) -I$(IROOT)/boot \
	-I$(IROOT)/sys/h \
	-I$(IROOT)/sys/src-4k -I$(IROOT)/sys/les -I$(IROOT)/sys/ts \
	-I$(IROOT)/boot/src-c4000 -I$(FILESYSDIR) -I$(IROOT)/sys/machine

DEFS= -DSVIP -DNOECHO -DFSLIB_NO_DEBUG -DOBJECT_4K -DTARGET_CISCO
CPPOPTS=-nostdinc $(INC) ${DEFS}

CCOPTS= -g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp32 -mips2
ASOPTS= -g $(GNUM) -fsigned-char -fno-builtin -mcpu=r4000 -mfp64 -mips3
CFLAGS= -O ${OPTIONS} ${CPPOPTS} ${CCOPTS}
ASFLAGS= -O ${OPTIONS} ${CPPOPTS} ${ASOPTS}

# Uncompress defines
UNCOMPRESS = uncompress
UNCSRCS    = $(COMSRCDIR)/sa_uncompress.c $(PROCSRCDIR)/scrt0_mips_unc.S
UNCOBJS    = scrt0_mips_unc.o sa_uncompress.o
UNCLDFLAGS = -e start $(GNUM) -N -Tsvip_ram.link -nostdlib -static

# dbus image defines
DBUS_IMAGE = dbus_image
DBUSLDFLAGS = -e start $(GNUM) -N -Tsvipdbus.link -nostdlib -static


# expimage defines
EXPIMAGE = expimage
EXPSRCS    = $(COMSRCDIR)/expimage.c $(PROCSRCDIR)/scrt0_mips_unc.S
EXPOBJS    = scrt0_mips_unc.o expimage.o
EXPLDFLAGS = -e start $(GNUM) -N -Tsvip_ram.link -nostdlib -static


# TARGET is the stripped ELF binary file, ELFFILE is unstripped
TARGET=   SVIP_RM
SYMFILE=  $(TARGET).sym
ELFFILE=  $(TARGET).elf
SRECFILE= $(TARGET).srec
SIMFILE=  $(TARGET).sim


#OBJDUMP= objdump.94q2.mips64 -h

LDFLAGS=-e evec -N $(GNUM) -Tsvip_rom.link -nostdlib
RAMLDFLAGS= -e __start -N -Tsvip_ram.link -nostdlib -static

# ROM Based Object files : start.o must be first !

ROBJS = svip_start.o main.o svip_init.o

# The object files needed to give the image the power to boot

BOOTOBJS = monfsio.o simpfsio.o monlib_iface.o loadprog.o \
	ismagic.o device.o svip_devtbl.o rawio.o fsutils.o boot.o \

# The object files needed to support DBUS download and virtual console

DBUSOBJS = dbus_start.o svip_dbus_main.o handler.o \
           command.o svip_control.o dbus_output.o \
           dbus_io.o svip_compress_lzw.o


# The remaining object files

OBJS = svip_moncmds.o monitor.o parsetoken.o shcmd.o memops.o alias.o builtin.o cpu_mips.o \
	nvram.o console.o svip_mon_exceptn.o testmem.o query.o \
	error.o signal.o berrtest.o svip_sizemem.o auxtest.o dis_mips.o \
	tlb.o auxport.o cache_mips.o  brk_mips.o \
	stk_mips.o imagetest.o confreg.o \
	diagtools.o  \
	svip_emulate.o svip_bzero.o stubs.o

# The modules that are usually in the library are no longer

LIBOBJS = atoh.o getline.o getopt.o puts.o strings.o \
	bcmp.o getnum.o printf.o setjmp_mips.o

CBASE=	$(COMSRCDIR)/main.c $(COMSRCDIR)/monitor.c $(COMSRCDIR)/parsetoken.c \
	$(COMSRCDIR)/shcmd.c $(COMSRCDIR)/memops.c $(COMSRCDIR)/confreg.c \
	$(COMSRCDIR)/alias.c $(COMSRCDIR)/builtin.c $(COMSRCDIR)/nvram.c \
	$(COMSRCDIR)/console.c \
	$(COMSRCDIR)/testmem.c $(COMSRCDIR)/query.c $(COMSRCDIR)/error.c \
	$(COMSRCDIR)/signal.c $(COMSRCDIR)/berrtest.c $(SVIPDIR)/svip_moncmds.c \
	$(COMSRCDIR)/auxtest.c $(PROCSRCDIR)/dis_mips.c $(COMSRCDIR)/imagetest.c \
	$(PROCSRCDIR)/cpu_mips.c $(PROCSRCDIR)/tlb.c \
	$(COMSRCDIR)/diagtools.c \
	$(COMSRCDIR)/auxport.c $(PROCSRCDIR)/cache_mips.c \
	$(PROCSRCDIR)/brk_mips.c $(PROCSRCDIR)/stk_mips.c $(COMSRCDIR)/flash.c \
	$(SVIPDIR)/svip_init.c \
	$(SVIPDIR)/svip_mon_exceptn.c  $(SVIPDIR)/svip_sizemem.c \
	$(SVIPDIR)/svip_emulate.c $(SVIPDIR)/svip_bzero.c \
	$(COMSRCDIR)/monfsio.c $(COMSRCDIR)/simpfsio.c $(COMSRCDIR)/rawio.c \
	$(COMSRCDIR)/loadprog.c $(COMSRCDIR)/fsutils.c \
	$(COMSRCDIR)/monlib_iface.c $(COMSRCDIR)/ismagic.c \
	$(COMSRCDIR)/device.c \
	$(SVIPDIR)/svip_devtbl.c \
	$(COMSRCDIR)/atoh.c $(COMSRCDIR)/getline.c $(COMSRCDIR)/getopt.c \
	$(COMSRCDIR)/puts.c $(COMSRCDIR)/strings.c \
	$(COMSRCDIR)/bcmp.c $(COMSRCDIR)/getnum.c $(COMSRCDIR)/printf.c \
	$(COMSRCDIR)/sa_uncompress.c \
	$(COMSRCDIR)/command.c $(SVIPDIR)/svip_compress_lzw.c $(SVIPDIR)/svip_control.c \
	$(COMSRCDIR)/dbus_io.c $(SVIPDIR)/svip_dbus_main.c $(COMSRCDIR)/dbus_output.c \
	$(COMSRCDIR)/handler.c $(SVIPDIR)/stubs.c $(COMSRCDIR)/boot.c

HFILES=	$(SVIPDIR)/svip_nvmonvars.h $(INCDIR)/setjmp.h $(INCDIR)/mon_elf.h \
	$(INCDIR)/queryflags.h $(INCDIR)/sh.h \
	$(INCDIR)/testmem.h $(INCDIR)/bitdef.h $(PROCSRCDIR)/cpu_mips.h \
	$(INCDIR)/mon_defs.h $(SVIPDIR)/svip_mon_plat_defs.h \
	$(INCDIR)/nonvol_x2444.h $(PROCSRCDIR)/dis_mips.h $(INCDIR)/mon_boot.h\
	$(IROOT)/sys/src-4k/signal_4k.h $(INCDIR)/dev.h \
	$(INCDIR)/proto.h $(INCDIR)/eeprom.h $(INCDIR)/error.h \
	$(INCDIR)/strings.h $(INCDIR)/flash.h $(INCDIR)/simpfsio.h \
	$(INCDIR)/monitor.h $(INCDIR)/monfsio.h $(INCDIR)/fsio.h \
	$(IROOT)/sys/src-svip/svip_pcmap.h \
	$(INCDIR)/stdarg.h $(IROOT)/sys/h/elf.h $(IROOT)/sys/ts/s2681.h \
	$(INCDIR)/console.h $(IROOT)/sys/h/types.h $(IROOT)/boot/cpu.h \
	$(INCDIR)/confreg.h $(INCDIR)/extern.h $(IROOT)/boot/buff.h \
	$(IROOT)/boot/emt.h $(IROOT)/boot/reason.h $(IROOT)/sys/les/flash_fs_les.h \
	$(SVIPDIR)/svip_vfpga.h $(IROOT)/sys/hes/dgbus.h \
	$(FILESYSDIR)/clpd6720.h \
	$(FILESYSDIR)/clpd6720_internals.h \
	$(FILESYSDIR)/filesys.h $(FILESYSDIR)/fslib.h \
	$(FILESYSDIR)/fslib_internals.h \
	$(FILESYSDIR)/fslib_print.h $(FILESYSDIR)/ralib.h \
	$(FILESYSDIR)/ralib_internals.h \
	$(SVIPDIR)/svip_vfpga.h $(COMSRCDIR)/command.h \
	$(COMSRCDIR)/compress_lzw.h $(COMSRCDIR)/dbstatus.h \
	$(COMSRCDIR)/dbus.h $(COMSRCDIR)/queue.h $(SVIPDIR)/diagcode.h



ASM=	$(SVIPDIR)/svip_start.S $(PROCSRCDIR)/setjmp_mips.S $(PROCSRCDIR)/scrt0_mips_unc.S \
	$(COMSRCDIR)/dbus_start.S $(SVIPDIR)/svip_dbus_addr.S

UTOOLS=  eprom_elf squish_elf mkffsi
HTOOLS=  hxld
TOOLS=   $(patsubst %,$(UTIL_DIR)/%,$(UTOOLS)) $(patsubst %,$(UTIL_DIR)/hxldsrc/%,$(HTOOLS))

vpath %.c	$(COMSRCDIR):$(SVIPDIR):$(PROCSRCDIR):$(FILESYSDIR)
vpath %.h	$(COMSRCDIR):(SVIPDIR):$(PROCSRCDIR):$(IROOT)/sys/h:\
	$(IROOT)/sys/ts:\
	$(IROOT)/sys/src-4k:$(IROOT)/les:$(IROOT)/boot:$(IROOT)/boot/src-c4000:\
	$(IROOT)/boot/src-68-c4000 $(FILESYSDIR)
vpath %.S	$(COMSRCDIR):$(SVIPDIR):$(PROCSRCDIR)

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
	rm -f $(OBJS) $(ROBJS) $(BOOTOBJS) $(LIBOBJS) $(UNCOBJS) banner.* \
	tools

clobber: clean
	rm -f $(TARGET)* $(UNCOMPRESS)* moncode* pfsi syms*

superclean: clobber
	$(MAKE) --no-print-directory -C $(UTIL_DIR) clean
	$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc clean

moncode: $(TARGET) tools
	$(UTIL_DIR)/hxldsrc/hxld -r 32 -a 0  -f moncode $(TARGET)

force-tools:
	rm -f tools

tools:
	@$(MAKE) --no-print-directory -C $(UTIL_DIR) depend $(UTOOLS)
	@$(MAKE) --no-print-directory -C $(IROOT)/sys/obj-4k-svip mksum_elf
	@$(MAKE) --no-print-directory -C $(UTIL_DIR)/hxldsrc depend $(HTOOLS)
	touch tools


$(TARGET): $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) makefile $(UTIL_DIR)/banner.sh \
		svip_rom.link $(UTIL_DIR)/sym_elf.sh tools \
		$(UTIL_DIR)/symbols_elf.sh
	@sh $(UTIL_DIR)/banner.sh VIP
	$(CC) $(CFLAGS) -c banner.c
	$(LD) $(LDFLAGS) -o $(ELFFILE) $(ROBJS) $(OBJS) $(BOOTOBJS) $(LIBOBJS) banner.o  
	cp $(TARGET).elf $(TARGET).nm.src
	sh $(UTIL_DIR)/sym_elf.sh $(ELFFILE) >syms
	$(UTIL_DIR)/eprom_elf syms $(ELFFILE)
	$(SIZE) $(ELFFILE)
	sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(ELFFILE) $(TARGET).symbols $(ELFFILE)
	cp $(ELFFILE) $(TARGET)
	$(STRIP) $(TARGET)

$(UNCOMPRESS): $(UNCOBJS) makefile svip_ram.link $(UTIL_DIR)/symbols_elf.sh \
		tools 
	$(LD) $(UNCLDFLAGS) -o $(UNCOMPRESS) $(UNCOBJS)
	$(SIZE) $(UNCOMPRESS)
	$(NM) $(UNCOMPRESS) >$(UNCOMPRESS).symbols
	sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(UNCOMPRESS) $(UNCOMPRESS).symbols $(UNCOMPRESS)
	$(STRIP) $(UNCOMPRESS)
	# Place SVIP machine id in ELF header
	$(IROOT)/sys/obj-4k-svip/mksum_elf $(UNCOMPRESS) -machine
	$(UTIL_DIR)/squish_elf $(UNCOMPRESS) temp
	mv temp $(UNCOMPRESS)


$(DBUS_IMAGE): $(DBUSOBJS) makefile svipdbus.link $(UTIL_DIR)/symbols_elf.sh tools
	$(LD) $(DBUSLDFLAGS) -o $(DBUS_IMAGE)   $(DBUSOBJS) 
	$(SIZE) $(DBUS_IMAGE)
	cp $(DBUS_IMAGE) $(DBUS_IMAGE).nm.src
	$(NM)  $(DBUS_IMAGE) >$(DBUS_IMAGE).symbols
	$(NM) -n  $(DBUS_IMAGE) >$(DBUS_IMAGE).nm
	sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(DBUS_IMAGE) $(DBUS_IMAGE).symbols $(DBUS_IMAGE)
	$(STRIP) $(DBUS_IMAGE)
	# Place SVIP machine id in ELF header
	$(IROOT)/sys/obj-4k-svip/mksum_elf $(DBUS_IMAGE) -machine
	$(UTIL_DIR)/squish_elf $(DBUS_IMAGE) temp
	mv temp $(DBUS_IMAGE)


$(EXPIMAGE): $(EXPOBJS) makefile svip_ram.link $(UTIL_DIR)/symbols_elf.sh \
		tools 
	$(LD) $(EXPLDFLAGS) -o $(EXPIMAGE) $(EXPOBJS)
	@$(SIZE) $(EXPIMAGE)
	@$(NM) $(EXPIMAGE) >$(EXPIMAGE).symbols
	@sh $(UTIL_DIR)/symbols_elf.sh $(NM) $(EXPIMAGE) $(EXPIMAGE).symbols $(EXPIMAGE)
	@$(STRIP) $(EXPIMAGE)
	# Place SVIP machine id in ELF header
	$(IROOT)/sys/obj-4k-svip/mksum_elf $(EXPIMAGE) -machine
	$(UTIL_DIR)/squish_elf $(EXPIMAGE) temp
	@mv temp $(EXPIMAGE)

$(SIMFILE): $(TARGET)
	exe2sim -I ELF -O LAI -B 1 -i $(TARGET) -o $(SIMFILE) -A bfc00000 -S 00040000
	cp $(SIMFILE).dat0 sim.rom.dat0
	$(UTIL_DIR)/split_rom < $(TARGET).sim.dat0

$(SRECFILE): $(UNCOMPRESS) $(DBUS_IMAGE)  moncode tools $(UTIL_DIR)/combpcode.sh
	$(UTIL_DIR)/mkffsi $(UNCOMPRESS) $(DBUS_IMAGE) pfsi
	sh $(UTIL_DIR)/combpcode.sh moncode pfsi $(SRECFILE) >/dev/null
	cp $(SRECFILE) /tftpboot/$(TARGET).new

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
