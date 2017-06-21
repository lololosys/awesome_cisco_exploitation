# $Id: makefile,v 3.23.4.29 1996/09/11 23:24:13 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-68-c3000/makefile,v $
#------------------------------------------------------------------
# Makefile for C3000 (aka Pancake, IGS, and so forth)
#
# December 5, 1989, Chris Shaker
#
# Copyright (c) 1989-1997 by cisco Systems, Inc.
# All rights reserved.
# 
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.23.4.29  1996/09/11  23:24:13  skalahas
# CSCdi68845:  Remove CPP from DRIVERS_G
# Branch: California_branch
# CPP needs to be there in only -j- image.
#
# Revision 3.23.4.28  1996/09/11  21:05:28  skalahas
# CSCdi68845:  Add CPP to DRIVERS
# Branch: California_branch
#
# Revision 3.23.4.27  1996/09/09  22:16:14  ralls
# CSCdi68493:  Problems building c1000-boot image
# Branch: California_branch
#
# Revision 3.23.4.26  1996/09/05  22:35:11  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.23.4.25  1996/09/03  15:53:04  richl
# CSCdi67850:  rename images
#
# Revision 3.23.4.24  1996/08/06  18:19:04  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.23.4.23  1996/07/14  07:13:19  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.23.4.22  1996/07/02  18:17:08  ralls
# CSCdi61900:  Update make.features
# Branch: California_branch
#
# Revision 3.23.4.21  1996/07/02  01:12:23  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-68-*)
# Branch: California_branch
#
# Revision 3.23.4.20  1996/06/28  02:03:27  sdatta
# CSCdi61603:  Remove ISDN_LT_GAIA and ISDN_LT_FEATURE
# Branch: California_branch
#
# Revision 3.23.4.19  1996/06/26  00:57:37  sdatta
# CSCdi61314:  Update makefile
# Branch: California_branch
#
# Revision 3.23.4.18  1996/06/24  21:25:52  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.23.4.17  1996/06/18  23:41:39  hampton
# Don't include CPP in images that don't have ISDN.  [CSCdi60703]
# Branch: California_branch
#
# Revision 3.23.4.16  1996/06/17  08:34:16  jhernand
# Commit Modular Cobra to California_branch
#
# Revision 3.23.4.15  1996/06/13  19:08:39  etrehus
# Commit boa to California Branch
#
# Revision 3.23.4.14  1996/06/12  19:56:24  jng
# CSCdi60260:  Async catwoman commit
# Branch: California_branch
# Async Catwoman commit into California
#
# Revision 3.23.4.13  1996/06/12  00:16:52  fbordona
# CSCdi60152:  DLSw merge of low-end and RSP fast switch code for
#              modularity.
# Branch: California_branch
#
# Revision 3.23.4.12  1996/06/10  20:03:30  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.23.4.11  1996/06/04  23:28:46  ralls
# CSCdi57580:  Update gaiafeatures, onec more
# Branch: California_branch
#
# Revision 3.23.4.10  1996/05/17  11:33:39  ppearce
# Merge IbuMod into Calif
#
# Revision 3.19.2.4  1996/05/06  00:06:29  ppearce
# Sync to IbuMod_Calif_baseline_960504
#
# Revision 3.19.2.3  1996/04/11  15:45:23  fbordona
# CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
# Branch: IbuMod_Calif_branch
#
# Revision 3.19.2.2  1996/04/03  20:11:26  ppearce
# Sync to IbuMod_Calif_baseline_960402
#
# Revision 3.19.2.1  1996/03/17  18:12:17  ppearce
# Sync IbuMod_Calif_branch to V111_1_3
#
# Revision 3.23.4.9  1996/05/10  01:35:25  wmay
# CPP and ppp half-bridge commit
# Branch: California_branch
#
# Revision 3.23.4.8  1996/05/08  12:01:57  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.23.4.7  1996/05/02  08:43:42  dwong
# Branch: California_branch
# Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
#
# Revision 3.23.4.6  1996/05/01  02:36:51  bew
# CSCdi56287:  Need to add crypto images into makefile targets
# Branch: California_branch
# Add crypto image to oddball list.
#
# Revision 3.23.4.5  1996/04/30  20:11:45  ralls
# CSCdi56220:  Update Gaia features
# Branch: California_branch
#
# Revision 3.23.4.4  1996/04/23  19:28:08  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.23.4.3  1996/04/16  19:08:06  rchandra
# CSCdi54830:  IP features commit
# Branch: California_branch
#
# Revision 3.23.4.2  1996/04/15  21:20:19  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.23.4.1  1996/03/18  21:15:20  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.15.2.4  1996/03/16  07:12:58  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.15.2.3  1996/03/09  05:07:50  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.15.2.2  1996/03/07  10:16:44  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.15.2.1  1996/02/20  10:43:09  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.23  1996/03/08  23:38:09  ljiang
# CSCdi50986:  Add mailbox support for Microcom Terminator(c2511
# derivative)
#
# Revision 3.22  1996/03/07  21:50:48  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.21  1996/03/07  20:50:26  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.20  1996/03/07  19:34:36  abaindur
# CSCdi50941:  Dust off platform specific stuff from relocatable image
# tools
#
# Revision 3.19  1996/02/26  21:51:00  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.18  1996/02/17  09:03:05  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.17  1996/02/15  18:02:46  gtaylor
# CSCdi45779:  Leased Line ISDN work.
#
# Revision 3.16  1996/01/29  05:45:08  hampton
# Missed a change for one of the igs oddball images when renaming the
# timer files.  [CSCdi46482]
#
# Revision 3.15  1996/01/16  09:37:49  billw
# CSCdi46950:  igs-c-l too big.  Remove decnet and advanced routing
# protocols
#
# Revision 3.14  1996/01/11  19:31:51  samuell
# CSCdi46757:  Add RMON images
#
# Revision 3.13  1996/01/06  05:16:50  hampton
# Split timer specific routines into multiple files.  Move the processor
# specific routines into the src-xx-xxx directories.  [CSCdi46482]
#
# Revision 3.12  1996/01/04  00:36:14  gwynne
# CSCdi46366:  Media-specific images required for igs-in-l image
#              feature set
# Also:   - Change copyright end date to 1996
#         - Add save of uncompressed binary size of disk file for
#           self-decompressing images (-mz).
#
# Revision 3.11  1995/12/22  05:38:13  ralls
# CSCdi45995:  Make the Heisenberg and null images build
#
# Revision 3.10  1995/12/16  00:14:44  sdurham
# CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
# cli support.
#    moved all parser commands related to mib to appropriate mib
#    subsystem.
#
# Revision 3.9  1995/12/14  19:38:32  ralls
# CSCdi45537:  Add null and nonbooting images to makefile
#
# Revision 3.8  1995/12/12  01:01:09  thille
# CSCdi45397:  Marketing initially unclear about what images they want
# RMON in.
# Create the images they want this week.
#
# Revision 3.7  1995/12/11  23:19:43  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.6  1995/12/03  17:57:45  thille
# CSCdi44905:  ciscopro targets not in 11.1
# Put them in.
#
# Revision 3.5  1995/11/22  16:23:39  gstovall
# CSCdi44327:  c2500 oddball images dont build
# Make sure to include all the serial bits when we use serial.
#
# Revision 3.4  1995/11/22  05:42:14  eschaffe
# CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
# option.
# Obsolete PROTOCONVERT flag in makefiles.  The control for defining
# protocolconversion for an image is now platform_specific.
#
# Revision 3.3  1995/11/17  17:52:35  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:10:01  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 2.18  1995/10/23  20:21:17  jquan
# CSCdi40522:  Modularity: sub_c3000 should not require sub_brut
#
# Revision 2.17  1995/10/18  21:46:13  hampton
# Need a space conserving way to test build all images.  Add targets for
# production.test, oddballs.test, modular.test, and everything.test.
# These delete each target after making it to limit disk space usage.
# [CSCdi42405]
#
# Revision 2.16  1995/10/10  17:39:44  gwynne
# CSCdi41859:  Desktop image igs-d-l > 4MB. Need smaller desktop image.
# Add makefile variables required to subset a desktop image < 4MB. Add
# the image to the oddballs for nightly build and limited availability.
# (gwynne)
#
# Revision 2.15  1995/10/10  07:23:41  sroach
# CSCdi41879:  Add BSC test command support
#
# Revision 2.14  1995/09/04  00:58:57  richl
# CSCdi39767:  Add BiSync to -r- images on low end
# Move bisync subsystems out of common code, into platforms on
# which it is supported.
#
# Revision 2.13  1995/08/22  18:55:03  jjohnson
# CSCdi39028:  minor makefile cleanup
# remove duplicate oddball
#
# Revision 2.12  1995/08/22  16:57:56  jjohnson
# CSCdi39028:  minor makefile cleanup
#
# Revision 2.11  1995/08/08  02:02:29  richl
# CSCdi38367:  dependencies for sub_pcbus_lnm.o not created, add
# dependency
#
# Revision 2.10  1995/08/07  05:40:18  richl
# CSCdi38315:  makefile rework
#
# Revision 2.9  1995/07/31  08:50:15  arothwel
# CSCdi37920:  Fix broken BSC/BSTUN MIB s/ware. Fix platform make for
# same.
#
# Revision 2.8  1995/07/17  23:27:45  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.7  1995/07/13  19:55:36  jkuzma
# CSCdi36776:  Partner box needs IPIPX image
#
# Revision 2.6  1995/07/12  22:25:37  gstovall
# CSCdi36994:  Partner code needs to be pulled out of core subsystem
# Move the rest of the partner code from c3000 subsys to the partner
# subsys.
#
# Revision 2.5  1995/06/27  17:07:26  tkolar
# CSCdi35462:  DES code needs to be its own subsystem
# Make it easier to export the source base.
#
# Revision 2.4  1995/06/22  17:15:48  sberl
# CSCdi34459:  make production target should build official images
# Add appn sub/superset images to trunkd.test target.
#
# Revision 2.3  1995/06/21  03:02:32  gstovall
# CSCdi35731:  Cleanup unused cruft in idb
# Nuke some unused vectors and variables from the hwidb.
#
# Revision 2.2  1995/06/19  20:13:08  jkuzma
# CSCdi33135:  Partner subset removed from desktop and IP images
#              - Added PARTNER macro to SYSTEM_DT and SYSTEM_IP
#
# Revision 2.1  1995/06/07  21:52:03  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c2500-j-l

PLATFORM_TIMER   = timer_generic.o timer_c3000.o
PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.68
include ../makefile

# need to stub out VLANS because it should not exist on this platform
VLANS = 

#
# Platform specific definitions
#
PLATFORM              = PAN
PLATFORM_FLAGS        = -D$(PLATFORM)
PLATFORM_IMAGE_GROUP  = 2500
PLATFORM_IMAGE_PREFIX = C2500
HWFLAGS = les

PLATFORM_SUPPORT =   sub_c2500.o
PLATFORM_SUPPORT_LITE =   sub_c2500_lite.o 
PLATFORM_REQ =   sub_c2500_req.o 
#
# PLATFORM_MIBS is a GAIA variable only
#
PLATFORM_MIBS = $(FLASH_LES_MIB)

ROM_LDFLAGS    = -e __start -N -nostdlib -Ttext 01020000 -Tdata 1000
FLASH_LDFLAGS  = -e __start -N -nostdlib -Ttext 03000060 -Tdata 1000
RAM_LDFLAGS    = -e __start -N -nostdlib -Ttext 1000
RELOC_LDFLAGS  = -e __start -N -nostdlib -Ttext 0 -Tdata 0 -r
RELOC_SUNFLAGS = -e __start -N -nostdlib -Ttext 0 -Tdata 0

RELOC_LD_OPTIONS = -text 3000060 -data 1000
RELOC_MKSUM_OPTIONS =
RELOC_PIC = relocate

#
# Platform-specific plus option set features
#
PLATFORM_PLUS= $(RMON) $(FAST_IPNAT) $(IPNAT)

#
# Define additional subsystems for manufacturing image
#
MANU =    sub_manu.o

#
# Define drivers supported by this platform
#
BRUT =      sub_brut.o
BRI =       sub_bri.o
CSU_DSU =   sub_csu_dsu.o
DS1_C2500_MIB = sub_ds1c2500mib.o
LANCE =     sub_lance.o
HD64570 =   sub_hd64570.o
ICSUDSU_MIB = sub_icsudsumib.o
MK5025 =    sub_mk5025.o
TMS380 =    sub_tms380.o
PARTNER =   sub_partner.o
CD2430 =    sub_cd2430.o
PCBUS =     sub_pcbus.o
HUB =       sub_hub.o 
HUB_MIB =   sub_rptrmib.o
PCBUS_LNM = sub_pcbus_lnm.o
LES_SERIAL = sub_les_serial.o

DRIVERS_F_MIBS = $(ETHERMIB) $(TRINGMIB)
DRIVERS_FIN_MIBS = $(ETHERMIB) $(TRINGMIB)
DRIVERS_G_MIBS = $(ETHERMIB) $(HUB_MIB) $(ISDNMIB) $(TRINGMIB) $(CALLMIB)
DRIVERS_NAS_MIBS= $(ETHERMIB) $(TRINGMIB)

DRIVER_MIBS =	$(ETHERMIB) $(TRINGMIB) $(HUB_MIB) $(CALLMIB) $(ISDNMIB) \
		$(DS1_C2500_MIB) $(ICSUDSU_MIB)

DRIVERS_F =	$(ETHER) $(TRING) $(SERIAL) $(BRUT) $(HD64570) \
		$(PCBUS) $(LES_SERIAL) $(CD2430)
DRIVERS_FIN =   $(LANCE) $(HD64570) $(MK5025) $(TMS380) $(CD2430) $(ETHER) \
	        $(SERIAL) $(TRING) $(LEX) $(BRUT) $(PCBUS) $(LES_SERIAL)
DRIVERS_FINO =  $(LANCE) $(HD64570) $(MK5025) $(TMS380) $(CD2430) $(ETHER) \
	        $(SERIAL) $(TRING) $(LEX) $(BRUT) $(LES_SERIAL)
DRIVERS_G =     $(ETHER) $(TRING) $(SERIAL) $(BRUT) $(BRI) \
		$(LANCE) $(TMS380) $(HUB) $(ISDN) $(PCBUS) $(LES_SERIAL)
DRIVERS_NAS=	$(LANCE) $(HD64570) $(TMS380) $(CD2430) \
		$(ETHER) $(SERIAL) $(TRING) $(BRUT) $(LES_SERIAL)
DRIVERS = 	$(LANCE) $(HD64570) $(CSU_DSU) $(MK5025) $(TMS380) $(PCBUS) \
		$(CD2430) $(ISDN) $(ETHER) $(SERIAL) $(TRING) $(BRI) $(BRUT) \
		$(HUB) $(PARTNER) $(LEX) $(LES_SERIAL) $(V120) $(CPP)

DRIVERS_BOOT =  $(LANCE) $(HD64570) $(CSU_DSU) $(TMS380) $(ISDN) \
		$(ETHER) $(SERIAL) $(TRING) $(BRI) $(LES_SERIAL)

#
# Define platform specific mibs
#
MIBS :=	$(MIBS) $(FLASH_LES_MIB) 

#
# This platform has additional comserver support, depending on the image.
#
NAS_J_OPTIONS=  $(ARAP) $(DES) $(LPD)
NAS_I_OPTIONS= 	$(COMMSERVER) $(MENUS) $(LPD) 
NAS_P_OPTIONS=  $(NAS_I_OPTIONS)
NAS_D_OPTIONS= 	$(NAS_I_OPTIONS) $(ARAP) $(DES)

#
# Defines for fast switching
#
FAST_MEDIA 	 = fr_fast_les.o 
FAST_FR 	 = fr_fast_les.o 
FAST_ATALK	 = at_fast_les.o
FAST_ATSMRP	 = at_smrpfast_les.o at_smrpfast.o smrp_fast_les.o
FAST_CLNS	 = clnsfast_les.o
FAST_DECNET	 = dnfast_les.o
FAST_IBM	 = fastsrb_les.o fastdlsw.o
FAST_IP		 = ipfast_les.o ipasm.o
FAST_IPMCAST	 = ipmfast_les.o platform_les_ipmfast.o
FAST_IPNAT	 = ipnat_les.o
FAST_TUNNEL	 = tfast_les.o
FAST_IPX	 = novfast_les.o
FAST_TBRIDGE	 = tbridge.o tbridge_les.o 
FAST_VINES	 = vinesfast_les.o
FAST_XNS	 = xnsfast_les.o
#
# c2500 subsystem definitions (note alphabetical order)
#

sub_bri =       if_c3000_bri.o if_les_bri.o asm_c3000_bri.o \
		if_les_bri_chain.o bri_debug.o msg_bri.o if_les_leased_bri.o

sub_brut =	brut.o brut_chain.o brut_debug.o

sub_c2500 =	asm_c3000.o diag.o diag_68.o diag_chain.o \
		platform_c3000.o if_les.o c3000_snmp.o \
		les_chain.o les_console.o nv_les.o c3000_nv.o \
		file_io_msgs.o \
           	flash_dvr_les.o dev_io.o file_io.o flash_fs_les.o \
		c3000_flash.o flash_partition.o flash_cmnds.o \
		flash_services.o flash_services_les.o snmp_flash_cmnds.o  \
		flh_chain.o flh.o nv_config_eeprom.o

sub_c2500_req =  asm_c3000.o nv_config_eeprom.o \
                les_console.o platform_c3000.o c3000_nv.o nv_les.o

sub_c2500_lite = asm_c3000.o diag.o diag_68.o diag_chain.o \
               platform_c3000.o if_les.o c3000_snmp.o \
               les_chain.o les_console.o nv_les.o c3000_nv.o \
               file_io_msgs.o \
               flash_dvr_les.o dev_io.o file_io.o flash_fs_les.o \
               c3000_flash.o flash_partition.o flash_cmnds.o \
               flash_services.o flash_services_les.o snmp_flash_cmnds.o  \
               nv_config_eeprom.o 

sub_cd2430 =    if_les_common_cd2430.o if_c3000_cd2430.o \
                if_les_sync_cd2430.o if_c3000_sync_cd2430.o cd2430_sync_debug.o \
                if_les_async_cd2430.o if_c3000_async_cd2430.o tty_cd2430_chain.o \
                asm_cd2430.o cd2402img.o msg_cd2430.o

sub_csu_dsu =	asm_c3000_csu_dsu.o if_c3000_csu_dsu.o \
                if_c3000_t1_csu_dsu.o if_c3000_sw56_csu_dsu.o msg_csu_dsu.o \
                if_c3000_csu_dsu_parser.o csu_dsu_debug.o csu_dsu_chain.o

sub_ds1c2500mib = sr_ds1c3000mib.o sr_ds1c3000miboid.o

sub_hd64570 =	asm_c3000_hd64570.o if_les_hd64570.o if_c3000_hd64570.o \
		msg_hd64570.o

sub_hub =       hub.o hub_chain.o hub_parser.o \
                hub_ether.o hub_show_ether.o msg_hub.o \
                hub_debug.o

sub_icsudsumib = sr_icsudsumib.o sr_icsudsumiboid.o

sub_lance  =	asm_c3000_lance.o if_les_lance.o if_c3000_lance.o \
		msg_lance.o if_les_lance_chain.o if_les_ether.o

sub_les_serial = if_les_serial.o

sub_lex_platform = lex_sub_les.o lex_fs_les.o

sub_manu =      manu_les.o nonvol_x2444.o

sub_mk5025 =	asm_c3000_mk5025.o if_les_mk5025.o if_c3000_mk5025.o \
		msg_mk5025.o

sub_partner =   mailbox_debug.o msg_mailbox.o chipcom_mbox_chain.o \
                partner_subsys.o partner_c3000.o \
		chipcom_mbox.o chipcom_mbox_c3000.o \
                microcom_mbox_c3000.o

sub_pcbus_lnm = if_c3000_local_lnm.o local_lnm_chain.o

sub_pcbus =	if_c3000_pcbus.o pcbus_debug.o pcbus_chain.o

sub_rptrmib =	sr_rptrmib.o sr_rptrmiboid.o hub_mib_chain.o

sub_test =	les_test.o transmit_test.o

sub_tms380 =	asm_c3000_tms380.o if_les_tms380.o if_c3000_tms380.o \
		if_les_madge.o asm_les_madge.o

#
# c3000 susbsytem build information
#

sub_bri.o			: $(sub_bri)
sub_brut.o			: $(sub_brut)
sub_c2500.o			: $(sub_c2500)
sub_c2500_lite.o		: $(sub_c2500_lite)
sub_c2500_req.o			: $(sub_c2500_req)
sub_cd2430.o			: $(sub_cd2430)
sub_csu_dsu.o			: $(sub_csu_dsu)
sub_ds1c2500mib.o		: $(sub_ds1c2500mib)
sub_hd64570.o			: $(sub_hd64570)
sub_hub.o			: $(sub_hub)
sub_icsudsumib.o		: $(sub_icsudsumib)
sub_lance.o			: $(sub_lance)
sub_les_serial.o		: $(sub_les_serial)
sub_lex_platform.o		: $(sub_lex_platform)
sub_manu.o			: $(sub_manu)
sub_mk5025.o			: $(sub_mk5025)
sub_partner.o			: $(sub_partner)
sub_pcbus.o			: $(sub_pcbus)
sub_pcbus_lnm.o			: $(sub_pcbus_lnm)
sub_rptrmib.o			: $(sub_rptrmib)
sub_test.o			: $(sub_test)
sub_tms380.o			: $(sub_tms380)

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
production_images = c2500-ajs-l c2500-i-l c2500-is-l \
		    c2500-d-l c2500-ds-l c2500-j-l c2500-js-l  \
		    c2500-c-l c2500-f-l c2500-fin-l c2500-f2in-l \
		    c2500-g-l c2500-p-l c2500-ainr-l c2500-boot-r

production: $(production_images) 

production_test_images = $(addsuffix .test,$(production_images))
production.test: $(production_test_images)

#
# Crypto production available only to the crypto gang and release meisters
#
crypto_images=  c2500-ajs40-l c2500-ajs56-l c2500-is40-l c2500-is56-l \
		c2500-ds40-l  c2500-ds56-l  c2500-js40-l c2500-js56-l 

crypto: $(crypto_images)

crypto_test_images = $(addsuffix .test,$(crypto_images))
crypto.test: $(crypto_test_images)


#
# The "unofficial" engineering specials
#
oddball_notyet = 
oddball_images = c2500-tatalk-m c2500-tddr-m c2500-tic-m c2500-tip-m \
	         c2500-tjmanu-m c2500-tjpgen-m c2500-tjrem-m c2500-tjtcl-m \
		 c2500-tli-m c2500-td4-m c2500-td4-l \
	         c2500-tiv-m c2500-tnull-m c2500-tmin-m 

oddballs: $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)

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

#
# Some gaia stuff was here
#
include $(SYSROOT)/obj-68-c3000/make.gaia

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
c2500-%-l.sun: lock main-gs-%-l gs-%.lego \
	$(RELOC_LD_PROG) $(RELOC_MKSUM_PROG) $(RELOC_PIC) 
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RELOC_LDFLAGS) -o gs-$*-l.reloctemp \
	Main_main-gs-$*-l.o cisco-gs-$*-l.o $(image-gs-$*)
	mv gs-$*-l.reloctemp c2500-$*-l.reloctemp
	$(RELOC_LD_PROG) $(RELOC_LD_OPTIONS) c2500-$*-l.reloctemp \
	$(RELOC_PIC) c2500-$*-l.relocbin c2500-$*-l.temp \
	|| (rm -f c2500-$*-l.reloctemp ; exit 1)
	rm -f c2500-$*-l.reloctemp
	mv c2500-$*-l.temp $@
	rm -f test.lock

c2500-%-m.czsun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c2500-%-m.sun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c2500-%-f.sun: lock main-gs-%-f gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(FLASH_LDFLAGS) -o gs-$*-f.temp \
	Main_main-gs-$*-f.o cisco-gs-$*-f.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-f.temp $@

c2500-%-r.sun: lock main-gs-%-r gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(ROM_LDFLAGS) -o gs-$*-r.temp \
	Main_main-gs-$*-r.o cisco-gs-$*-r.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-r.temp $@

c2500-%-l.relocbin: c2500-%-l.sun
	$(RELOC_MKSUM_PROG) $(RELOC_MKSUM_OPTIONS) $@
	ls -l $@

#
# Image rules
#

c2500-%-l: c2500-%-l.reloc
	@
c2500-%-r: c2500-%-r.rom
	@
c2500-%-m: c2500-%-m.tftp
	@
c2500-%-f: c2500-%-f.tftp
	@

.PRECIOUS: c2500-%-l.sun c2500-%-m.sun c2500-%-f.sun c2500-%-r.sun 

include ../makedist
############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies
