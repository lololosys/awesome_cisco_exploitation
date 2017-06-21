# $Id: makefile,v 1.1.68.15 1996/09/14 05:35:01 mvenugop Exp $
# $Source: /release/112/cvs/Xsys/obj-68-as5200/makefile,v $
#------------------------------------------------------------------
# Makefile for AS5200
#
# August 1995, Joe Turner
#
# Copyright (c) 1989-1997 by cisco Systems, Inc.
# All rights reserved.
#
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 1.1.68.15  1996/09/14  05:35:01  mvenugop
# CSCdi69188:  Updating Gaia changes
# Branch: California_branch
#
# Revision 1.1.68.14  1996/09/11  21:05:16  skalahas
# CSCdi68845:  Add CPP to DRIVERS
# Branch: California_branch
#
# Revision 1.1.68.13  1996/09/10  00:06:30  jliu
# CSCdi64205:  Unprovisioned T1 plugged into AS5200 causes box to hang
# Branch: California_branch
#    o The fix included display warning message to console port and used
#      SNMP notification to send the message to network management station
#
# Revision 1.1.68.12  1996/09/05  22:36:20  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 1.1.68.11  1996/09/03  15:52:55  richl
# CSCdi67850:  rename images
#
# Revision 1.1.68.10  1996/08/28  13:01:36  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 1.1.68.9  1996/08/06  18:19:32  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 1.1.68.8  1996/07/30  01:31:11  styang
# CSCdi64413:  support dropped for rfc1406 and modem mgmt mib
# Branch: California_branch
# Line "DRIVER_MIBS = $(ETHERMIB) $(CALLMIB) $(DS1MIB) $(ISDNMIB)
# $(MODEM_MGMT_MIB)" is deleted by accident. Added it back.
#
# Revision 1.1.68.7  1996/07/15  20:36:59  towu
# CSCdi62430:  CT1 RBS is not enabled
# Branch: California_branch
#         o Adding msg_dsx0.o in sub_csm subsystem.
#
# Revision 1.1.68.6  1996/07/14  18:31:49  elem
# CSCdi62889:  Line config is missing autoselect parameter in AS5200
# Branch: California_branch
# Restore the NAS options which causes the COMMSERVER to be included
# into the IP only images.
#
# Revision 1.1.68.5  1996/07/08  18:21:56  ralls
# CSCdi62184:  Fixing small problems with make.features
# Branch: California_branch
#
# Revision 1.1.68.4  1996/07/02  23:55:29  jturner
# CSCdi61176:  post-brasil-commit code cleanup in tdm, asm, src and
# makefile
# Branch: California_branch
#
# Revision 1.1.68.3  1996/07/02  01:12:17  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-68-*)
# Branch: California_branch
#
# Revision 1.1.68.2  1996/06/24  21:25:48  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 1.1.68.1  1996/06/16  21:17:24  vnguyen
# Finally committing Brasil to California_branch
#
# Revision 1.1  1995/12/03  21:23:18  jturner
# Placeholder for Brasil.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c5200-js-l

PLATFORM_TIMER   = timer_generic.o timer_c3000.o
PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.68
include ../makefile

#
# Platform specific definitions
#
PLATFORM              = BRASIL
PLATFORM_FLAGS        = -D$(PLATFORM)
PLATFORM_IMAGE_GROUP  = 5200
PLATFORM_IMAGE_PREFIX = C5200
HWFLAGS               = les

PLATFORM_SUPPORT =   sub_as5200.o
PLATFORM_REQ =   sub_as5200_req.o
PLATFORM_MIBS = $(FLASH_LES_MIB)

ROM_LDFLAGS    = -e __start -N -nostdlib -Ttext 01020000 -Tdata 5000
FLASH_LDFLAGS  = -e __start -N -nostdlib -Ttext 03000060 -Tdata 5000
RAM_LDFLAGS    = -e __start -N -nostdlib -Ttext 5000
RELOC_LDFLAGS  = -e __start -N -nostdlib -Ttext 0 -Tdata 0 -r
RELOC_SUNFLAGS = -e __start -N -nostdlib -Ttext 0 -Tdata 0

RELOC_LD_OPTIONS = -text 03000060 -data 5000
RELOC_MKSUM_OPTIONS =
RELOC_PIC = relocate

#
# Non-modular stuff required to make the "modular" images link
#
PLATFORM_NONMODULAR = 

NONMODULAR_PLATFORM_REQ = $(MCN) $(DSX1) $(MODEM_MGMT_POOL) $(MODEM_MGMT) \
			  $(TDM) $(CDM) $(TRING) \
			  $(SRBCORE) $(CSM) $(DRIVER_MIBS)
#
# Define drivers supported by this platform
#
CD2430 =    sub_cd2430.o
CSM =       sub_csm.o
DSX1 =      sub_dsx1.o
HD64570 =   sub_hd64570.o
LANCE =     sub_lance.o
LES_SERIAL = sub_les_serial.o
MCN =       sub_mcn.o
MODEM_MGMT_POOL = sub_modem_mgmt_pool.o
MODEM_MGMT = sub_modem_mgmt.o
MODEM_MGMT_MIB = sub_modem_mgmtmib.o
NONMOD = shr_tring.o
PRI =       sub_pri.o
TDM =	    sub_tdm.o

# VLANS needs to be stubbed out because the 5200 does not support it.
VLANS =

#
# Here is where you add features to the plus set which are platform specific
#
PLATFORM_PLUS_OOPS = $(MODEM_MGMT) $(MODEM_MGMT_POOL) $(MODEM_MGMT_MIB) \
		     $(RMON) $(V120) $(IPNAT) $(FAST_IPNAT)

#
# Non-modular hack because you can't remove the plus feature set from the
# c5200 images.  Not even the boot image...  When you fix the modularity
# of modem management, please remove _OOPS from PLATFORM_PLUS, and you can
# delete PLATFORM_PARTS for this platform.  This will move the plus features
# out of the boot image and leave them in the right images.
# You'll also need to remove $(SNMP) from $(DRIVERS_BOOT).
# 
PLATFORM_PARTS = $(PLATFORM_PLUS_OOPS)
#
# Group the DRIVERS options
#

DRIVER_MIBS = $(ETHERMIB) $(CALLMIB) $(DS1MIB) $(ISDNMIB) 

DRIVERS =       $(LANCE) $(HD64570) \
		$(ISDN) $(PRI) $(TDM) $(CSM) $(MCN) $(DSX1) \
		$(CD2430) $(CPP) $(ETHER) \
		$(SERIAL) $(TRING) $(LES_SERIAL) 
#
# ISP image does not support CPP or TRING
#
DRIVERS_ISP =	$(LANCE) $(HD64570) \
		$(ISDN) $(PRI) $(TDM) $(CSM) $(MCN) $(DSX1) \
		$(CD2430) $(ETHER) \
		$(SERIAL) $(LES_SERIAL) 

DRIVERS_BOOT =  $(DRIVERS) $(SNMP) $(MIBS) $(CDP) $(MODEMCAP)

#
# Define platform specific mibs
#
MIBS :=	$(MIBS) $(FLASH_LES_MIB) 

#
# This platform has additional comserver support, depending on the image.
#
NAS_J_OPTIONS=  $(ARAP) $(DES) $(LPD)
NAS_I_OPTIONS=  $(COMMSERVER) $(MENUS) $(LPD) \
                $(SYSLOGMIB)
NAS_P_OPTIONS=  $(COMMSERVER) $(MENUS) $(SYSLOGMIB) 
NAS_D_OPTIONS=  $(NAS_I_OPTIONS) $(ARAP) $(DES)

#
# Defines for fast switching
#
FAST_MEDIA 	 = fr_fast_les.o 
FAST_FR		 = fr_fast_les.o
FAST_ATALK	 = at_fast_les.o
FAST_ATSMRP	 = at_smrpfast_les.o at_smrpfast.o smrp_fast_les.o
FAST_TBRIDGE	 = tbridge.o tbridge_les.o
FAST_CLNS	 = clnsfast_les.o
FAST_DECNET	 = dnfast_les.o
FAST_IBM	 = fastsrb_les.o fastdlsw.o
FAST_IP		 = ipfast_les.o ipasm.o
FAST_IPMCAST	 = ipmfast_les.o platform_les_ipmfast.o
FAST_IPNAT	 = ipnat_les.o
FAST_TUNNEL	 = tfast_les.o
FAST_IPX	 = novfast_les.o
FAST_VINES	 = vinesfast_les.o
FAST_XNS	 = xnsfast_les.o

#
# as5200 subsystem definitions (note alphabetical order)
#
sub_as5200 =	asm_as5200.o diag.o diag_68.o diag_chain.o \
		platform_as5200.o if_les.o \
		les_chain.o les_console.o nv_les.o as5200_nv.o \
		file_io_msgs.o \
           	flash_dvr_les.o dev_io.o file_io.o flash_fs_les.o \
		as5200_flash.o flash_partition.o flash_cmnds.o \
		flash_services.o flash_services_les.o snmp_flash_cmnds.o  \
		nv_config_eeprom.o as5200_nim.o  

sub_as5200_req =  asm_as5200.o nv_config_eeprom.o \
                les_console.o platform_as5200.o as5200_nv.o nv_les.o

sub_cd2430 =	asm_as5200_cd2430.o cd2402img.o tty_as5200_cd2430.o \
                tty_cd2430_chain.o msg_cd2430.o

sub_csm =       if_as5200_csm.o if_as5200_dummy.o modem_mgmt_chain.o \
                msg_dsx0.o msg_csm.o

sub_dsx1 =   if_controller.o if_controller_chain.o if_msg_controller.o \
             dsx1.o dsx1_chain.o msg_dsx1.o \
             les_controller.o if_c4000_dsx1.o if_as5200_t1.o if_as5200_e1.o \
             if_as5200_dsx1_framer.o as5200_t1_firmware.o as5200_busytone.o \
             as5200_dialtone.o if_as5200_dsx1_ipc.o

sub_hd64570 =	asm_c3000_hd64570.o if_les_hd64570.o if_c3000_hd64570.o \
		msg_hd64570.o

sub_lance  =	asm_c3000_lance.o if_les_lance.o if_c3000_lance.o \
		msg_lance.o if_les_lance_chain.o if_les_ether.o

sub_les_serial = if_les_serial.o

sub_mcn    =    if_as5200_mcn.o if_c4000_m32.o \
                if_les_m32.o as5200_mcn.o  

sub_modem_mgmt_pool = if_as5200_modem.o modem_async_if.o \
                      modem_chain.o modem_debug.o modem_parser.o msg_modem.o \
                      as5200_nim_snmp.o as5200_snmp.o

sub_modem_mgmt = modem_mgmt.o 

sub_modem_mgmtmib = sr_modem_mgmtmib.o sr_modem_mgmtmiboid.o 

sub_partner =   mailbox_debug.o msg_mailbox.o chipcom_mbox_chain.o \
		chipcom_mbox.o chipcom_mbox_c3000.o

sub_pri =       ccpri.o ccpmsg.o ccpcc.o cc_serv.o

sub_tdm =       if_as5200_tdm.o tdm_chain.o tdm_parser.o msg_tdm.o \
		tdm_debug.o

sub_test =	les_test.o transmit_test.o

#
# as5200 subsystem build information
#
sub_as5200.o			: $(sub_as5200)
sub_cd2430.o			: $(sub_cd2430)
sub_csm.o                       : $(sub_csm)
sub_dsx1.o                      : $(sub_dsx1)
sub_hd64570.o			: $(sub_hd64570)
sub_lance.o			: $(sub_lance)
sub_les_serial.o		: $(sub_les_serial)
sub_mcn.o                       : $(sub_mcn)
sub_modem_mgmt_pool.o           : $(sub_modem_mgmt_pool)
sub_modem_mgmt.o		: $(sub_modem_mgmt)
sub_modem_mgmtmib.o		: $(sub_modem_mgmtmib)
sub_partner.o			: $(sub_partner)
sub_pri.o                       : $(sub_pri)
sub_tdm.o			: $(sub_tdm)
sub_test.o			: $(sub_test)

#
# If Common Object support is disabled, include the rest of the subsystem
# definitions here
#

ifeq ($(OBJECTS),)
include ../makesubsys
endif

#
# Include code flow and coverage definitions
#
include ../makecflow

#
# Include the definitions for final targets. This *must* come after
# the local definitions of subsystems
#

include ../makeimages
include ../make.features

#
# The "official" production set of images supplied to manufacturing
#
production_images = c5200-js-l c5200-ds-l c5200-is-l \
		    c5200-i-l c5200-d-l c5200-j-l c5200-p3-l

# The as5200 boot image is removed because of a modularity break
#                 c5200-boot-l

production:	$(production_images)

production_test_images = $(addsuffix .test,$(production_images))

production.test: $(production_test_images)

#
# Crypto production available only to the crypto gang and release meisters
#
crypto_images = 

crypto: $(crypto_images)

crypto_test_images = $(addsuffix .test,$(crypto_images))
crypto.test: $(crypto_test_images)


#
# The "unofficial" engineering specials
#

oddball_notyet = 
oddball_images = c5200-tjpgen-m c5200-tjtcl-m

oddballs: $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)


# Build the suite of modular images, then delete them.
#
modular: $(MODULAR)
	rm -f $(MODULAR)

#
# Given the set of object files in the modular image definitions,
# determine whether there are any modularity breaks without actually
# building the images.
#
modularity_check: $(MODULARITY_CHECK)

include $(SYSROOT)/obj-68-as5200/make.gaia

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Build all images 
#
everything: production modular oddballs

#
# Generic Link rules.
#

# reloc_ld processes a .sun file with relocation info and creates a
# cisco relocatable image that has a special header, a position
# independent relocator, and relocation tables sandwiched between the
# a.out header and the text segment.
# reloc_ld also creates the final .sun file from which the .symbols
# file is generated.
# reloc_mksum processes the relocatable image and inserts checksums.
#
c5200-%-l.relocbin: c5200-%-l.sun
	./$(RELOC_MKSUM_PROG) $(RELOC_MKSUM_OPTIONS) $@
	ls -l $@

c5200-%-l.sun: lock main-gs-%-l gs-%.lego \
	$(RELOC_LD_PROG) $(RELOC_MKSUM_PROG) $(RELOC_PIC) 
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RELOC_LDFLAGS) -o gs-$*-l.reloctemp \
	Main_main-gs-$*-l.o cisco-gs-$*-l.o $(image-gs-$*)
	mv gs-$*-l.reloctemp c5200-$*-l.reloctemp
	./$(RELOC_LD_PROG) $(RELOC_LD_OPTIONS) c5200-$*-l.reloctemp \
	$(RELOC_PIC) c5200-$*-l.relocbin c5200-$*-l.temp \
	|| (rm -f c5200-$*-l.reloctemp ; exit 1)
	rm -f c5200-$*-l.reloctemp
	mv c5200-$*-l.temp $@
	rm -f test.lock

c5200-%-m.sun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c5200-%-f.sun: lock main-gs-%-f gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(FLASH_LDFLAGS) -o gs-$*-f.temp \
	Main_main-gs-$*-f.o cisco-gs-$*-f.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-f.temp $@

c5200-%-r.sun: lock main-gs-%-r gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(ROM_LDFLAGS) -o gs-$*-r.temp \
	Main_main-gs-$*-r.o cisco-gs-$*-r.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-r.temp $@

#
# Image rules
#

as5200_t1_firmware.o: ../firmware/as5200_t1_firmware.c

c5200-%-l: make_firmware c5200-%-l.reloc
	@
c5200-%-r: c5200-%-r.rom
	@
c5200-%-m: c5200-%-m.tftp
	@
c5200-%-f: c5200-%-f.tftp
	@

.PRECIOUS: c5200-%-l.sun c5200-%-m.sun c5200-%-f.sun c5200-%-r.sun

include ../makedist
############################################################################

include $(SYSROOT)/dependancies

make_firmware:
	$(MAKE) -C ../firmware/as/t1;

