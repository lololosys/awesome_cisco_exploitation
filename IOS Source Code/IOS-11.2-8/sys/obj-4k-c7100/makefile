# $Id: makefile,v 3.2.58.37 1996/09/10 23:45:19 ssangiah Exp $
# $Source: /release/112/cvs/Xsys/obj-4k-c7100/makefile,v $
#------------------------------------------------------------------
# makefile - Makefile for Predator (c7100)
#
# July 1995, Michael Beesley
#
# Copyright (c) 1995-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.2.58.37  1996/09/10  23:45:19  ssangiah
# CSCdi67934: Some asm source files compiled with incorrect flags.
# Branch: California_branch
#
# Revision 3.2.58.36  1996/09/09  22:36:29  ralls
# CSCdi68524:  Minor makefile image list changes
# Branch: California_branch
#
# Revision 3.2.58.35  1996/09/05  22:40:37  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.2.58.34  1996/09/03  15:52:18  richl
# CSCdi67850:  rename images
#
# Revision 3.2.58.33  1996/08/28  13:01:14  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.2.58.32  1996/08/26  22:41:15  dujihara
# CSCdi65820:  Image load point too high by 32k
# Branch: California_branch
# Image was loaded in the wrong location, wasting 32K of memory.
#
# Revision 3.2.58.31  1996/08/26  19:07:48  ssangiah
# CSCdi58114: Missing support for fastswitching of GRE for some protocols
#             and on the c7200.
# Branch: California_branch
#
# Revision 3.2.58.30  1996/08/23  18:39:56  abaindur
# CSCdi66370:  C7200 IP fastswitch code is a loner
# Branch: California_branch
#
# Revision 3.2.58.29  1996/08/06  18:19:56  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.2.58.28  1996/07/23  05:28:21  sdatta
# CSCdi63755:  Deleted crypto images from 7200 makefile
# Branch: California_branch
#
# Revision 3.2.58.27  1996/07/14  07:12:55  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.2.58.26  1996/07/02  18:17:18  ralls
# CSCdi61900:  Update make.features
# Branch: California_branch
#
# Revision 3.2.58.25  1996/07/02  01:07:25  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-4k-*)
# Branch: California_branch
#
# Revision 3.2.58.24  1996/06/26  01:12:59  sdatta
# CSCdi61320:  update makefile
# Branch: California_branch
#
# Revision 3.2.58.23  1996/06/24  21:25:34  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.2.58.22  1996/06/22  01:17:45  ssangiah
# CSCdi61081:  No fddimib support in Predator
# Add the FDDI MIB agent to c7200 images.
# Branch: California_branch
#
# Revision 3.2.58.21  1996/06/17  22:25:04  gstovall
# CSCdi58553:  Need new images for new product
# Branch: California_branch
# Add new *-inu-* image type for the c7200.
#
# Revision 3.2.58.20  1996/06/10  20:05:07  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.2.58.19  1996/06/04  23:33:46  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.2.58.18  1996/05/21  19:57:21  dcarroll
# CSCdi57757:  Fix envmon mib sub block
# Branch: California_branch
# Separate envmon code from envmon_mib code in makefile.
#
# Revision 3.2.58.17  1996/05/19  00:24:42  rpratt
# CSCdi57306:  HTML config has problems in new platform
# Branch: California_branch
#
# Revision 3.2.58.16  1996/05/08  04:00:14  ralls
# CSCdi57036:  Update the gaia features
# Branch: California_branch
#
# Revision 3.2.58.15  1996/05/07  00:14:32  mbeesley
# CSCdi55679:  Add IP flow/optimum switching engine
# Branch: California_branch
#
# Revision 3.2.58.14  1996/05/02  08:43:02  dwong
# Branch: California_branch
# Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
#
# Revision 3.2.58.13  1996/04/30  20:14:44  ralls
# CSCdi56220:  Update Gaia features
# Branch: California_branch
#
# Revision 3.2.58.12  1996/04/30  16:37:21  gstovall
# CSCdi56093:  modular builds fail for new platform
# Branch: California_branch
# Make the modular builds work again.
#
# Revision 3.2.58.11  1996/04/24  00:06:56  ahh
# CSCdi55483:  NAT: code review comments, various bug fixes
# Fast-path fixup, cruft removal.
# Branch: California_branch
#
# Revision 3.2.58.10  1996/04/23  19:32:37  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.2.58.9  1996/04/22  18:58:31  ssangiah
# CSCdi54920:  Need to add support for the flash MIB on c7200.
# Branch: California_branch
#
# Revision 3.2.58.8  1996/04/20  01:01:44  mbeesley
# CSCdi53386:  11.2 modular-ukernel images do not compile
# Branch: California_branch
#
# Revision 3.2.58.7  1996/04/15  21:20:03  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.2.58.6  1996/04/12  23:11:49  mdb
# CSCdi54154:  Need to remove old files
# Branch: California_branch
# Remove last traces of fslib_string.c
#
# Revision 3.2.58.5  1996/04/12  17:00:04  mdb
# CSCdi54154:  Need to remove old files
# Branch: California_branch
# Remove references to gutted files in the makefiles and nuke the gutted
# files.
#
# Revision 3.2.58.4  1996/04/09  14:00:01  ssangiah
# CSCdi53822: Need to deal with product name change.
# Fix .cvsignore and update the default image name to have the
# c7200- prefix.
# Branch: California_branch
#
# Revision 3.2.58.3  1996/04/06  02:05:23  ssangiah
# CSCdi53822:  Need to deal with product name change.
# Make changes to deal with the change in the product name for Predator
# from c7100 to c7200.
# Branch: California_branch
#
# Revision 3.2.58.2  1996/03/29  00:44:53  hochan
# CSCdi52960:  Move -k- images to oddballs
# Branch: California_branch
#
# Revision 3.2.58.1  1996/03/21  22:57:45  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.2.2.20  1996/03/18  23:16:49  mbeesley
# Branch: ELC_branch
# Sync to V111_1_3
#
# Revision 3.2.2.19  1996/02/14  23:26:13  mbeesley
# CSCdi48870:  modular.test build fails for Predator. Move the
# PCI bridge subsystem into the platform subsystem and out of
# the driver subsystem list.
# Branch: ELC_branch
#
# Revision 3.2.2.18  1996/02/14  22:42:50  gstovall
# CSCdi49023:  make production.test doesnt work in obj-4k-c7100
# Branch: ELC_branch
# It does now.
#
# Revision 3.2.2.17  1996/02/12  22:03:08  mbeesley
# CSCdi48718:  Bootstrap image does not build. Add AMDP2 to boot image.
# Branch: ELC_branch
#
# Revision 3.2.2.16  1996/02/08  08:23:40  gstovall
# Branch: ELC_branch
# Sync to ELC_baseline_960205.
#
# Revision 3.2.2.15  1996/02/01  08:10:14  mbeesley
# Branch: ELC_branch
# Bring the joys of transparent bridging to the c7100.
#
# Revision 3.2.2.14  1996/01/31  01:36:26  sthormod
# Branch: ELC_branch
# Add Predator 4R support.
#
# Revision 3.2.2.13  1996/01/25  20:01:26  gstovall
# Branch: ELC_branch
# Sigh... FDDI commit removed the 4T from the image. Put it back.
#
# Revision 3.2.2.12  1996/01/25  12:09:54  fsunaval
# Branch: ELC_branch
# 1.Predator FDDI support.
# 2.Patch CSCdi46358 (PA management interrupt) changes into ELC_branch.
#
# Revision 3.2.2.11  1996/01/18  22:10:46  gstovall
# Branch: ELC_branch
# Preadator now knows the joy and splendor of serial interfaces.
#
# Revision 3.2.2.10  1996/01/15  23:55:19  dcarroll
# Branch: ELC_branch
# Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
#
# Revision 3.2.2.9  1996/01/03  05:58:19  wfried
# Branch: ELC_branch
# Include file system TFTP copy support file in Predator's subsystem.
#
# Revision 3.2.2.8  1995/12/22  20:56:22  dcarroll
# Branch: ELC_branch
# Implement particle based IPX fastswitching for Predator.
#
# Revision 3.2.2.7  1995/12/15  18:42:19  dcarroll
# Branch: ELC_branch
# Remove envmon code from boot image. Use common
# envmon parser code.
#
# Revision 3.2.2.6  1995/12/12  04:41:13  mbeesley
# Branch: ELC_branch
# Use slotted interface names. Remove the 4e device driver from
# the bootstrap image for the time being. Should allow netbooting
# over the IO card 1fe.
#
# Revision 3.2.2.5  1995/12/11  19:03:00  mbeesley
# Branch: ELC_branch
# Sync ELC_branch to ELC_baseline_120995.
#
# Revision 3.2.2.4  1995/12/06  23:15:17  mbeesley
# Branch: ELC_branch
# Add parser support on fastethernet for Predator.
#
# Revision 3.2.2.3  1995/12/05  22:03:56  mbeesley
# Branch: ELC_branch
# Implement particle based IP fastswitch for Predator.
#
# Revision 3.2.2.2  1995/11/30  01:25:33  dcarroll
# Branch: ELC_branch
#
# Revision 3.2.2.1  1995/11/22  20:31:15  mbeesley
# Initial commit of c7100 support.
# Branch: ELC_branch
#
# Revision 3.2  1995/11/17  17:51:55  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  12:41:55  shaker
# Bump version numbers from 2.x to 3.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c7200-js-mz
PLATFORM_TIMER	= timer_generic.o timer_nevada.o asm_4k_timer.o
PLATFORM_OTHER	= c7100_vspace.o asm_4k_c7100.o c7100_ukernel.o 
PLATFORM_MONITOR = asm_4k_monitor.o

include ../makefile.tools
include ../makefile.4k
include ../makefile

#-------------------------------------------------------
# Platform specific definitions
#-------------------------------------------------------
PLATFORM		= PREDATOR
PLATFORM_FLAGS		= -D$(PLATFORM)
PLATFORM_IMAGE_GROUP	= 7200
PLATFORM_IMAGE_PREFIX	= C7200
HWFLAGS			= les
 
ROM_LDFLAGS= -e __start -N $(GNUM) -Ttext 0x60008000 -Tpredator.link -nostdlib
RAM_LDFLAGS= -e __start -N $(GNUM) -Ttext 0x60008000 -Tpredator.link -nostdlib
ZIP_LDFLAGS= -e __start -N $(GNUM) -Ttext 0x80008000 -Tpredator.link -nostdlib

PLATFORM_HTTP = sub_c7100_http.o
PLATFORM_SUPPORT = sub_c7100.o 
NONMODULAR_PLATFORM_REQ = shr_tring.o $(SERIAL) $(FDDI_FEATURE) $(SRBCORE) $(PLATFORM_HTTP)

PGEN :=
 
#
# Sched alloc is used on this platform - hence override this variable
# so that support for sched-interval is not pulled in
#
SCHED_INTERVAL =

PLATFORM_MIBS = $(ENVMIB)

#-------------------------------------------------------
# Define environmental monitor mib
#-------------------------------------------------------
ENVMIB = sub_envmib.o

#-------------------------------------------------------
# Define drivers supported by this platform
#-------------------------------------------------------
AMDP2 =		sub_amdp2.o   
DEC21140 =	sub_dec21140.o
DEC21150 =	sub_dec21150.o
IBM2692 =	sub_ibm2692.o
IOCARD =	sub_iocard.o
S4T68360 =	sub_s4t68360.o
TEST =		sub_test.o
MIF68840 =	sub_mif68840.o

#
# Non-modular stuff required to make the "modular" images link
#
PLATFORM_NONMODULAR = $(MIF68840)

#-------------------------------------------------------
# Define MIBS
#-------------------------------------------------------
DRIVER_MIBS =	$(ETHERMIB) $(TRINGMIB) $(FDDIMIB)

DRIVERS =	$(ETHER) $(SERIAL) $(TRING) $(LEX) \
		$(DEC21140) $(AMDP2) $(IOCARD) $(S4T68360)\
		$(MIF68840) $(IBM2692)

DRIVERS_BOOT =	$(ETHER) $(SERIAL) $(TRING) $(LEX) \
		$(DEC21140) $(AMDP2) $(IOCARD) $(S4T68360) \
		$(MIF68840) $(IBM2692)

#
# Use new flash MIB
#
FLASHMIB = $(FLASH_LES_MIB)

#
#------------------------------------------------------- 
# Add in platform specific mibs and other components
#-------------------------------------------------------
MIBS := $(MIBS) $(ENVMIB)

#-------------------------------------------------------
# Defines for fast switching
#-------------------------------------------------------
FAST_MEDIA 	 = fr_fast_les.o lex_fs_les.o
FAST_FR 	 = fr_fast_les.o
FAST_LEX 	 = lex_fs_les.o
FAST_ATALK	 = at_fast_les.o
FAST_ATSMRP 	 = at_smrpfast_les.o at_smrpfast.o smrp_fast_les.o
FAST_CLNS	 = clnsfast_les.o
FAST_DECNET	 = dnfast_les.o
FAST_IBM	 = fastsrb.o c7100_srb.o fastdlsw.o
FAST_IBM_SRB_ONLY = fastsrb.o c7100_srb.o
FAST_IP		 = ipfast_les.o ipasm_4k.o ipfast_flow_pas.o ipfast_flow.o \
		   ipfast_rsp_mtrie.o
FAST_IPMCAST	 = ipmfast_les.o platform_les_ipmfast.o
FAST_TUNNEL      = tfast_les.o
FAST_IPNAT	 = ipnat_les.o
FAST_IPX	 = novfast_pas.o
FAST_TBRIDGE	 = tbridge.o tbridge_pas.o
FAST_VINES	 = vinesfast_les.o
FAST_XNS	 = xnsfast_les.o
FAST_VLANS 	 = ieee_vlan.o isl_vlan.o vlan_les.o
NONMOD = shr_tring.o

#-------------------------------------------------------
# Definitions for the PLUS images
#-------------------------------------------------------
PLATFORM_PLUS = $(FAST_IPNAT) $(IPNAT) $(VLANS)

#-------------------------------------------------------
# Rules to build assembly language portions of Predator
#-------------------------------------------------------

C7100_ASM = 	asm_4k.o asm_4k_timer.o asm_4k_c7100.o \
		asm_4k_vip_dec21140.o asm_4k_vip_amdp2.o \
		ipasm_4k.o vinesasm_4k.o xnsasm_4k.o \
		asm_4k_vip_ibm2692.o asm_4k_vip_mif68840.o \
		asm_4k_vip_s4t68360.o

$(C7100_ASM) $(CZIPASM):
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)$(AS) -c $(ASWITCHES) $(WARN_ALL) $< \
	|| (rm -f $@ ; exit 1)

#-------------------------------------------------------
# C7100 subsystem definitions (note alphabetical order)
#-------------------------------------------------------

#
# sub_amdp2 - AMD Presidio-2 based 8E Ethernet port adapter
#
sub_amdp2 =	if_pas_amdp2.o if_c7100_amdp2.o if_isr_amdp2.o \
		msg_amdp2.o asm_4k_vip_amdp2.o

#
# sub_c7100 - core C7100 support
#
sub_c7100 =	c7100.o c7100_chain.o c7100_init.o \
		c7100_macaddr.o msg_c7100.o platform_c7100.o monvar.o \
		les_console.o \
		c7100_nv.o nv_les.o \
		c7100_snmp.o msg_oir.o \
		util_4k.o \
		calendar.o \
		if_controller.o if_controller_chain.o pas_controller.o \
		c7100_flashmib.o file_io_msgs.o \
		flash_cmnds_hes.o flash_services.o flash_services_hes.o \
		flash_command.o flash_config.o tftp_rcp_filesys.o \
		fs_rec.o nv_filesys.o flash_filesys.o fslib.o \
		fslib_dev_rom.o fslib_print.o c7100_ralib.o \
		clpd6729.o c7100_flash.o filesys.o rsp_fslib_dev.o \
		fslib_dev_ram.o fslib_dev_intel_series_2plus.o \
		fslib_dev_rsp_internal_flash.o rsp_fslib_test.o \
		filesys_test_cmd.o filesys_test_chain.o filesys_debug.o \
		c7100_oir.o oir_debug.o oir_chain.o \
		if_pas.o init_pas.o msg_pas.o pas_chain.o pci.o \
		c7100_pci.o gt64010.o msg_gt64010.o \
		c7100_monlib_bndl.o \
		c7100_envm.o envmon_chain.o \
		c7100_test.o \
		$(DEC21150)

#
# sub_c7100_http - Support for c7100 specific http services
#
sub_c7100_http = c7100_http.o

sub_envmib =	sr_envmib.o sr_envmib_c7100.o sr_envmib_7000_rsp.o \
		sr_envmiboid.o envmon_mib_chain.o envmib_registry.o

#
# sub_dec21140 - Support for dec21140 based 100 Mbit Ethernet port adapter
#
sub_dec21140 =	if_pas_dec21140.o if_c7100_dec21140.o msg_dec21140.o \
		if_isr_dec21140.o asm_4k_vip_dec21140.o \
		if_pas_dec21140_chain.o fast_ethernet_debug.o \
		fast_ethernet_chain.o

#
# sub_dec21150 - Support for dec pci to pci bridge
#
sub_dec21150 = 	dec_ppb.o

#
# sub_ibm2692.o - Support for ibm lan streamer token ring chip set
#
sub_ibm2692 =	if_pas_ibm2692.o if_c7100_ibm2692.o if_isr_ibm2692.o \
		msg_ibm2692.o asm_4k_vip_ibm2692.o

#
# sub_iocard - Support for the IO card based media
#
sub_iocard = 	if_c7100_iocard.o

#
#sub_mif68840 - Support for the Motorola MC68840 Fddi port adapter
#
sub_mif68840 = csp.o hw_pcm.o hw_pcode.o cspcfm.o cspproc.o cspecm.o \
		csprmt.o cspglbl.o cspsig.o cspinit.o csptimer.o csplem.o \
		cspmsg.o  \
		fbmglbl.o fbmq.o fbminit.o fbmraf.o fbmmsg.o fbmrcv.o fbmnn.o \
		fbmsrp.o fbmparam.o fbmsup.o fbm.o fbmpmf.o fbmtimer.o \
		fbmesf.o fbmpmfp.o fbmframe.o fbmproc.o \
		mibget.o mibglbl.o mibinit.o mibmsg.o mibproc.o mibsup.o  \
		mib.o mibtable.o mibchng.o mibxdi.o \
		fddi.o fddi_chain.o fddi_debug.o msg_fddi.o fddi_parser.o \
		mif68840_xdi.o if_c7100_mif68840.o if_xdi.o if_isr_mif68840.o \
		if_pas_mif68840.o msg_mif68840.o asm_4k_vip_mif68840.o

#
# sub_lex_platfrom - Lex support
#
sub_lex_platform = lex_sub_les.o

#
# sub_s4t68360 - Support for the MC68360 based 4T serial card
#
sub_s4t68360 =	if_pas_s4t68360.o if_c7100_s4t68360.o if_isr_s4t68360.o \
		msg_s4t68360.o asm_4k_vip_s4t68360.o if_c7100_s4t68360_parser.o

#
# sub_test - System test and diagnostics
#
sub_test =	diag_chain.o diag.o diag_4k.o transmit_test.o pas_test.o

#
# Subsystems
#
sub_amdp2.o   			: $(sub_amdp2)   
sub_c7100.o			: $(sub_c7100)
sub_c7100_http.o		: $(sub_c7100_http)
sub_envmib.o			: $(sub_envmib)
sub_dec21140.o			: $(sub_dec21140)
sub_dec21150.o			: $(sub_dec21150)
sub_ibm2692.o			: $(sub_ibm2692)
sub_iocard.o			: $(sub_iocard)
sub_lex_platform.o		: $(sub_lex_platform)
sub_s4t68360.o			: $(sub_s4t68360)
sub_test.o			: $(sub_test)
sub_mif68840.o          	: $(sub_mif68840)

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
production_images = c7200-ajs-mz c7200-boot-mz c7200-ds-mz c7200-inu-mz \
		    c7200-is-mz c7200-i-mz c7200-js-mz c7200-p-mz c7200-ads-mz

production: $(production_images)

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
oddball_images = c7200-monlib-m c7200-tatalk-m c7200-tddr-m c7200-tic-m \
		 c7200-tiv-m c7200-tjtcl-m c7200-tjmanu-m \
		 c7200-tjrem-m c7200-tli-m c7200-k-mz c7200-ak-mz

oddballs: $(oddball_images)
	rm -f $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)

#
# Predator is unable to build the modular-ukernel image. Add a special rule
#
modular-ukernel:
	@echo "modular-ukernel not supported at this time"

#
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

include $(SYSROOT)/obj-4k-c7100/make.gaia

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Build all images 
#
everything: production oddballs modular

#
# Generic Link rules.
#
c7200-%-m.czsun: lock $(SYS_START_FILE) main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c7200-%-m.sun: lock $(SYS_START_FILE) main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

.PRECIOUS: c7200-%-m.sun c7200-%-m.czsun

#
# Image rules
#
c7200-%-m: c7200-%-m.tftp
	@

#-------------------------------------------------------
# Libraries and the like
#-------------------------------------------------------

#
# create monlib file from flash file system .c source file
#
C7100_MONLIB_SRCFILES = ../filesys/scrt0_mips_fslib.S \
	../filesys/rommon_fslib_iface.c \
	../filesys/rommon_fslib_dev.c \
	../filesys/fslib.c \
	../filesys/fslib_dev_rom.c \
	../filesys/fslib_print.c \
	../filesys/c7100_ralib.c \
	../filesys/clpd6729.c

monlib_depend:
	$(MAKE) -f makefile.monlib depend

c7200-monlib-m: $(C7100_MONLIB_SRCFILES) makefile.monlib
	$(MAKE) -f makefile.monlib

../filesys/c7100_monlib_bndl.c: c7200-monlib-m \
	../scripts/make_c7100_monlib_bndl
	../scripts/make_c7100_monlib_bndl c7200-monlib-m $@

c7100_monlib_bndl.o: ../filesys/c7100_monlib_bndl.c

include ../makedist

############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies

