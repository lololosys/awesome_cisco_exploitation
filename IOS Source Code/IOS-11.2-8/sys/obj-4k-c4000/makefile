# $Id: makefile,v 3.15.4.27 1996/09/12 00:52:00 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-4k-c4000/makefile,v $
#------------------------------------------------------------------
# makefile - Makefile for system software for the R4K based C4000 (Sierra)
#
# 11/93, Lele Nardin
#
# Copyright (c) 1993-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.15.4.27  1996/09/12  00:52:00  skalahas
# CSCdi68904:  Add c4500-boot-mz in production_images list
# Branch: California_branch
#
# Revision 3.15.4.26  1996/09/11  18:44:36  skalahas
# CSCdi68814:  Remove fast lane from 4500 FAST_MEDIA
# Branch: California_branch
# Add FAST_LANE to PLATFORM_PLUS
#
# Revision 3.15.4.25  1996/09/11  18:11:29  skalahas
# CSCdi68814:  Remove fast lane from 4500 FAST_MEDIA
# Branch: California_branch
#
# Revision 3.15.4.24  1996/09/09  22:36:39  ralls
# CSCdi68524:  Minor makefile image list changes
# Branch: California_branch
#
# Revision 3.15.4.23  1996/09/05  22:40:46  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.15.4.22  1996/09/05  00:37:35  skalahas
# CSCdi68125:  sub_4500 rule missing
# Branch: California_branch
#
# Revision 3.15.4.21  1996/09/03  15:52:10  richl
# CSCdi67850:  rename images
#
# Revision 3.15.4.20  1996/08/28  13:01:07  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.15.4.19  1996/08/06  18:18:31  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.15.4.18  1996/07/14  07:12:52  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.15.4.17  1996/07/02  01:07:17  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-4k-*)
# Branch: California_branch
#
# Revision 3.15.4.16  1996/06/26  01:07:39  sdatta
# CSCdi61318:  update makefile
# Branch: California_branch
#
# Revision 3.15.4.15  1996/06/24  21:25:32  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.15.4.14  1996/06/12  00:16:42  fbordona
# CSCdi60152:  DLSw merge of low-end and RSP fast switch code for
#              modularity.
# Branch: California_branch
#
# Revision 3.15.4.13  1996/06/11  23:09:48  ralls
# CSCdi60023:  Gaia update
# Branch: California_branch
#
# Revision 3.15.4.12  1996/06/10  20:05:14  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.15.4.11  1996/06/04  23:33:53  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.15.4.10  1996/05/10  01:35:01  wmay
# CPP and ppp half-bridge commit
# Branch: California_branch
#
# Revision 3.15.4.9  1996/05/09  14:34:39  rbadri
# Branch: California_branch
# LANE, UNI3.1 features
#
# Revision 3.15.4.8  1996/05/02  08:42:22  dwong
# Branch: California_branch
# Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
#
# Revision 3.15.4.7  1996/05/01  02:39:13  bew
# CSCdi56287:  Need to add crypto images into makefile targets
# Branch: California_branch
# Add crypto image to oddball list.
#
# Revision 3.15.4.6  1996/04/28  22:52:58  tkolar
# CSCdi55981:  c4500-boot-m does not compile
# Branch: California_branch
# Fix a small typo from the ATM commit.
#
# Revision 3.15.4.5.2.1  1996/04/27  07:16:24  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.15.4.5  1996/04/25  23:21:45  ronnie
# CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
# Feature commit for ATM and Frame Relay into 11.2
# Branch: California_branch
#
# Revision 3.15.4.4  1996/04/23  19:32:48  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.15.4.3  1996/04/16  19:07:59  rchandra
# CSCdi54830:  IP features commit
# Branch: California_branch
#
# Revision 3.15.4.2  1996/04/15  21:20:00  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.15.4.1.8.1  1996/04/16  18:52:14  cyoung
# Take VTP out of the production images and make it an oddball
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.15.4.1  1996/03/18  21:13:56  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.7.12.4  1996/03/16  07:11:14  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.7.12.3  1996/03/09  05:07:24  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.7.12.2  1996/03/07  10:15:53  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.7.12.1  1996/02/20  10:42:52  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.15  1996/03/08  23:37:44  ljiang
# CSCdi50986:  Add mailbox support for Microcom Terminator(c2511
# derivative)
#
# Revision 3.14  1996/03/07  21:50:27  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.13  1996/03/07  20:50:00  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.12  1996/02/26  21:50:30  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.11  1996/02/17  09:00:57  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.10  1996/02/16  23:53:23  hampton
# Don't bother trying to compile broken oddball images.  These broken
# images should be added back to the list once they have been fixed.
# [CSCdi42489]
#
# Revision 3.9  1996/02/15  18:02:41  gtaylor
# CSCdi45779:  Leased Line ISDN work.
#
# Revision 3.8  1996/01/29  21:18:40  mohsen
# CSCdi44191:  Enable the support for E3 and DS3 PLIMs on 4500 ATM NIM
# Initialize E3 and DS3 PLIMS on startup.
#
# Revision 3.7  1996/01/06  05:16:28  hampton
# Split timer specific routines into multiple files.  Move the processor
# specific routines into the src-xx-xxx directories.  [CSCdi46482]
#
# Revision 3.6  1995/12/11  23:19:24  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.5  1995/12/03  17:57:35  thille
# CSCdi44905:  ciscopro targets not in 11.1
# Put them in.
#
# Revision 3.4  1995/11/21  23:57:26  jbalestr
# CSCdi43597:  BSTUN MIB not linked in for some images
#
# Revision 3.3  1995/11/17  17:51:50  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:08:58  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 2.17  1995/10/18  21:45:10  hampton
# Need a space conserving way to test build all images.  Add targets for
# production.test, oddballs.test, modular.test, and everything.test.
# These delete each target after making it to limit disk space usage.
# [CSCdi42405]
#
# Revision 2.16  1995/10/18  14:40:15  dblair
# CSCdi41356:  Default images of make files are not compressed
#         Change default images to be compressed image
#
# Revision 2.15  1995/10/10  07:23:00  sroach
# CSCdi41879:  Add BSC test command support
#
# Revision 2.14  1995/09/14  01:52:21  biren
# CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
#
# Revision 2.13  1995/09/04  00:58:51  richl
# CSCdi39767:  Add BiSync to -r- images on low end
# Move bisync subsystems out of common code, into platforms on
# which it is supported.
#
# Revision 2.12  1995/08/22  16:57:41  jjohnson
# CSCdi39028:  minor makefile cleanup
#
# Revision 2.11  1995/08/09  00:12:25  richl
# CSCdi38450:  oddball target out of date
#
# Revision 2.10  1995/08/07  05:39:37  richl
# CSCdi38315:  makefile rework
#
# Revision 2.9  1995/07/31  08:50:39  arothwel
# CSCdi37920:  Fix broken BSC/BSTUN MIB s/ware. Fix platform make for
# same.
#
# Revision 2.8  1995/07/17  23:27:25  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.7  1995/07/12  22:25:12  gstovall
# CSCdi36994:  Partner code needs to be pulled out of core subsystem
# Move the rest of the partner code from c4000 subsys to the partner
# subsys.
#
# Revision 2.6  1995/06/29  22:07:33  cnee
# CSCdi36573:  Mailbox support missing from IP/IPX image
#
# Revision 2.5  1995/06/23  22:23:38  mbeesley
# CSCdi36360:  95q1 compiler fails to build czip_elf
# Work-arround the compiler bug by declaring variables static.
# This allows zipped images for c4500, rsp to ship in 11.0.
#
# Revision 2.4  1995/06/22  17:15:23  sberl
# CSCdi34459:  make production target should build official images
# Add appn sub/superset images to trunkd.test target.
#
# Revision 2.3  1995/06/21  03:02:21  gstovall
# CSCdi35731:  Cleanup unused cruft in idb
# Nuke some unused vectors and variables from the hwidb.
#
# Revision 2.2  1995/06/08  22:07:33  jliu
# CSCdi35359:  Rename files for new hardware changes
#
# Revision 2.1  1995/06/07  21:50:53  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c4500-j-mz

PLATFORM_TIMER   = timer_generic.o timer_nevada.o asm_4k_timer.o
PLATFORM_OTHER   = sierra_ukernel.o sierra_vspace.o asm_4k_c4000.o
PLATFORM_MONITOR = asm_4k_monitor.o

include ../makefile.tools
include ../makefile.4k
include ../makefile

#
# Platform specific definitions
#
PLATFORM		= SIERRA
PLATFORM_FLAGS		= -D$(PLATFORM)
PLATFORM_IMAGE_GROUP	= 4500
PLATFORM_IMAGE_PREFIX	= C4500

HWFLAGS			= les

ROM_LDFLAGS= -e __start -N $(GNUM) -Ttext 0x60008000 -Tsierra.link -nostdlib
RAM_LDFLAGS= -e __start -N $(GNUM) -Ttext 0x60008000 -Tsierra.link -nostdlib
ZIP_LDFLAGS= -e __start -N $(GNUM) -Ttext 0x80008000 -Tsierra.link -nostdlib

PLATFORM_SUPPORT = sub_c4500.o 

PGEN :=	$(PGEN) sub_pgen_4500.o

#
# Non-modular stuff required to make the "modular" images link
#
#
# Define drivers supported by this platform
#
ATMZR =		sub_atmzr.o
BRI =		sub_bri.o
BSI =		sub_bsi.o
C4000_FDDI =	sub_c4000_fddi.o $(FDDI_SMT) $(BSI)
DSX1 =          sub_dsx1.o
C4000_PRI = 	sub_c4000_pri.o
HD64570 =	sub_hd64570.o
LANCE =		sub_lance.o
LES_SERIAL =	sub_les_serial.o
MCN =		sub_mcn.o
MK5025 =	sub_mk5025.o
TEST=		sub_test.o
TMS380 =	sub_tms380.o
PARTNER =	sub_partner.o
PCI =           sub_pci_nim.o
AM79970 =       sub_am79970.o
NONMOD = shr_tring.o

#
# Options for 
#
PLATFORM_PLUS = $(FAST_IPNAT) $(IPNAT) $(LANEMIB) $(LANE) $(FAST_LANE) $(VLANS)

#
# Non-modular stuff required to make the "modular" images link
#
PLATFORM_NONMODULAR = $(MCN) $(LANCE) $(DSX1)
NONMODULAR_PLATFORM_REQ = $(LANCE) $(MCN) $(DSX1) $(SERIAL) $(SRBCORE)
PLATFORM_MIBS = $(FLASH_LES_MIB) $(DS1MIB) $(ATOMMIB_ES)

DRIVER_MIBS =	$(ETHERMIB) $(TRINGMIB) $(FDDIMIB) $(DS1MIB) $(ISDNMIB) \
		$(CALLMIB) $(ATMMIB) $(ATOMMIB_ES) 

DRIVERS =	$(MK5025) $(TMS380) $(HD64570) $(MCN) $(BRI) $(LANCE) \
                $(ATMZR) $(DSX1) $(C4000_PRI) $(ETHER) $(SERIAL) $(TRING) \
		$(ATM) $(ATM_ARP) $(C4000_FDDI) $(ISDN) $(LEX) $(PARTNER) \
		$(PCI) $(AM79970) $(LES_SERIAL) $(V120) $(CPP)

DRIVERS_BOOT =	$(MK5025) $(TMS380) $(HD64570) $(MCN) $(BRI) $(LANCE) \
                $(ATMZR) $(DSX1) $(C4000_PRI) $(ETHER) $(SERIAL) $(TRING) \
		$(ATM) $(ATM_ARP) $(C4000_FDDI) $(ISDN) $(LEX) $(PARTNER) \
		$(PCI) $(AM79970) $(LES_SERIAL)

#
# For the ISDN images (not production on this platform, test only)
#
DRIVERS_G = $(LANCE) $(MK5025) $(HD64570) $(MCN) $(BRI) \
            $(ATMZR) $(DSX1) $(C4000_PRI) $(ETHER) $(SERIAL) $(LES_SERIAL)

# 
# Add in platform specific mibs and other components
#
MIBS := $(MIBS) $(FLASH_LES_MIB)

#
# This platform has additional comserver support, depending on the image.
#
NAS_P_OPTIONS= 

#
# Defines for fast switching
#
FAST_MEDIA 	 = fr_fast_les.o 
FAST_FR 	 = fr_fast_les.o
FAST_LANE 	 = lanefast_les.o
FAST_ATALK	 = at_fast_les.o
FAST_ATSMRP	 = at_smrpfast_les.o at_smrpfast.o smrp_fast_les.o
FAST_CLNS	 = clnsfast_les.o
FAST_DECNET	 = dnfast_les.o
FAST_IBM	 = fastsrb_les.o fastdlsw.o
FAST_IP		 = ipfast_les.o ipasm_4k.o
FAST_IPMCAST	 = ipmfast_les.o platform_les_ipmfast.o
FAST_IPNAT	 = ipnat_les.o
FAST_TUNNEL	 = tfast_les.o
FAST_IPX	 = novfast_les.o
FAST_TBRIDGE	 = tbridge.o tbridge_les.o 
FAST_VINES	 = vinesfast_les.o
FAST_XNS	 = xnsfast_les.o
FAST_VLANS	 = ieee_vlan.o isl_vlan.o vlan_les.o


#-------------------------------------------------------
# Rules to build assembly language portions of the 4500
#-------------------------------------------------------


C4500_ASM = 	asm_4k.o asm_4k_c4000.o asm_4k_c4000_bsi.o \
		asm_4k_c4000_lance.o asm_4k_c4000_mk5035.o \
		asm_4k_c4000_tms380.o asm_4k_c4000_m32.o e_r4k.o \
		asm_4k_timer.o ipasm_4k.o vinesasm_4k.o xnsasm_4k.o \
		asm_4k_c4000_atmzr.o

$(C4500_ASM) $(CZIPASM):
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)$(AS) -c $(ASWITCHES) $(WARN_ALL) $< \
	|| (rm -f $@ ; exit 1)

#-------------------------------------------------------
# C4500 subsystem definitions (note alphabetical order)
#-------------------------------------------------------

#
# sub_am79970 - Support for Am79c970 based six-ethernet NIMs on sierra.
#
sub_am79970  =   if_c4000_am79970.o sierra_am79970.o asm_4k_c4000_am79970.o 

#
# sub_atmzr - LSI Logic's ATMizer support for ATM NIM
#
sub_atmzr =	c4000_atmzr.o c4000_atmzr_sunilite.o \
	 	atmzr_firmware.o atmzr_xilinx.o sierra_atmzr.o \
		asm_4k_c4000_atmzr.o c4000_atm_chain.o \
		c4000_atmzr_sunipdh.o

#
# sub_bri - The National TP3420 and the Munich 32 support for Multi BRI NIM
#
sub_bri =       if_c4000_bri.o if_c4000_mb.o if_les_bri.o \
		if_les_bri_chain.o if_les_leased_bri.o bri_debug.o \
		if_c4000_bri_chain.o msg_bri.o

#
# sub_bsi - National Semiconductor BSI/BMAC support for FDDI NIM
#
sub_bsi = 	asm_4k_c4000_bsi.o c4000_bsi.o cam.o bsi_xilinx.o \
		c4000_fddi_chain.o msg_les_fddi.o

#
# sub_c4500 - non speed critical components of the core C4500 support
#
sub_c4500 =	platform_sierra.o c4000_nim.o c4000_nim_snmp.o \
		sierra_snmp.o \
		if_c4000_chain.o msg_nim.o sierra.o sierra_nim.o \
		if_les.o les_chain.o monvar.o \
		les_console.o nv_les.o sierra_nv.o util_4k.o \
		calendar.o \
            	flash_dvr_les.o sierra_flash.o flash_partition.o dev_io.o \
		file_io.o flash_fs_les.o flash_cmnds.o boot_flash_cmnds.o \
		file_io_msgs.o \
		flash_services.o flash_services_les.o snmp_flash_cmnds.o 

#
# sub_c4000_fddi - non speed critical components for C4500 FDDI
#
sub_c4000_fddi = c4000_fddi.o fddi.o fddi_debug.o msg_fddi.o csppcm.o \
                 csppcode.o sierra_fddi.o fddi_chain.o c4000_xdi.o \
		 fddi_parser.o
#
# sub_dsx1 - C4000 dsx1 subsystem for CE1/T1
#
sub_dsx1 = if_controller.o if_controller_chain.o les_controller.o \
           if_msg_controller.o dsx1.o dsx1_chain.o \
           if_c4000_dsx1.o if_c4000_ce1.o \
           if_c4000_bt8360.o if_c4000_dsx1_framer.o msg_dsx1.o \
           c4000_e1_firmware.o

#
#PRI support for 4500.
#
sub_c4000_pri = ccpri.o ccpmsg.o ccpcc.o cc_serv.o

#
# sub_hd64570 - Hitachi Hd64570 support for QSIM
#
sub_hd64570 = asm_4k_c4000_hd64570.o if_les_hd64570.o if_c4000_hd64570.o \
              sierra_hd64570.o msg_hd64570.o

#
# sub_lance - AMD Lance support for EIM and DEIM
#
sub_lance  =	asm_4k_c4000_lance.o if_les_lance.o if_c4000_lance.o \
	        sierra_lance.o msg_lance.o if_les_lance_chain.o \
		if_les_ether.o if_c4000_lance_parser.o
#
# sub_les_serial - Generic LES serial support
#
sub_les_serial =	if_les_serial.o

#
# sub_lex_platfrom - Lex les support.
#
sub_lex_platform = lex_sub_les.o lex_fs_les.o

#
# sub_mcn - The Munich 32 and the MCN subsystem
#
sub_mcn    =	asm_4k_c4000_m32.o if_c4000_mcn.o if_c4000_m32.o \
		if_les_m32.o sierra_mcn.o

#
# sub_mk5025 - SGS/Thomson MK5025 support for DSIM
#
sub_mk5025 =	asm_4k_c4000_mk5025.o if_les_mk5025.o if_c4000_mk5025.o \
		sierra_mk5025.o msg_mk5025.o

#
# sub_pci_nim - Support for PCI bus based NIMs on sierra.
#
sub_pci_nim  =   pci.o if_c4000_pci.o c4000_pci_xilinx.o

#
# sub_test - System test and diagnostics
#
sub_test =	diag_chain.o les_test.o diag.o diag_4k.o \
		transmit_test.o

#
# sub_tms380 - Texas TMS380 support for TRIM
#
sub_tms380 =	asm_4k_c4000_tms380.o if_les_tms380.o if_c4000_tms380.o \
		sierra_tms380.o if_les_madge.o asm_les_madge.o

#
# sub_partner - Partner support
#
sub_partner = 	mailbox_debug.o msg_mailbox.o chipcom_mbox_chain.o \
		partner_subsys.o partner_c4000.o \
		chipcom_mbox.o chipcom_mbox_4k_c4000.o
#
# sub_pgen_4500 - Pagent 4500 support
#
sub_pgen_4500 =	pgen_4500.o

sub_am79970.o                   : $(sub_am79970)
sub_atmzr.o			: $(sub_atmzr)
sub_bri.o			: $(sub_bri)
sub_bsi.o			: $(sub_bsi)
sub_c4500.o			: $(sub_c4500)
sub_c4000_fddi.o		: $(sub_c4000_fddi)
sub_dsx1.o                      : $(sub_dsx1)
sub_c4000_pri.o			: $(sub_c4000_pri)
sub_lance.o			: $(sub_lance)
sub_les_serial.o		: $(sub_les_serial)
sub_lex_platform.o		: $(sub_lex_platform)
sub_mcn.o			: $(sub_mcn)
sub_mk5025.o			: $(sub_mk5025)
sub_pci_nim.o                   : $(sub_pci_nim)
sub_tms380.o			: $(sub_tms380)
sub_partner.o			: $(sub_partner)
sub_hd64570.o			: $(sub_hd64570)
sub_test.o			: $(sub_test)
sub_pgen_4500.o			: $(sub_pgen_4500)

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
production_images = c4500-ajs-mz c4500-i-mz c4500-is-mz \
		    c4500-d-mz c4500-ds-mz c4500-j-mz c4500-js-mz \
		    c4500-ainr-mz c4500-p-mz c4500-boot-mz

production: $(production_images)

production_test_images = $(addsuffix .test,$(production_images))
production.test: $(production_test_images)

#
# Crypto production available only to the crypto gang and release meisters
#
crypto_images = c4500-ajs40-mz c4500-ajs56-mz c4500-is40-mz c4500-is56-mz \
	        c4500-ds40-mz c4500-ds56-mz c4500-js40-mz c4500-js56-mz 

crypto: $(crypto_images)

crypto_test_images = $(addsuffix .test,$(crypto_images))
crypto.test: $(crypto_test_images)

#
# The "unofficial" engineering specials.  Sorry, all images must be of the
# pattern c4500-t*
#
oddball_notyet = c4500-tic-m c4500-tli-m 
oddball_images = c4500-tatalk-m c4500-tddr-m c4500-tiv-m \
	         c4500-tjvtp-m c4500-tjpgen-m c4500-tjtcl-m c4500-tjmanu-m \
		 c4500-tjrem-m 

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
include $(SYSROOT)/obj-4k-c4000/make.gaia

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Build all images 
#

everything: production oddballs modular

#
# Generic Link rules
#
c4500-%-m.czsun: lock $(SYS_START_FILE) main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c4500-%-m.sun: lock $(SYS_START_FILE) main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

.PRECIOUS: c4500-%-m.sun c4500-%.czsun 

#
# Image rules
#
c4500-%-m: c4500-%-m.tftp
	@

include ../makedist

############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies
