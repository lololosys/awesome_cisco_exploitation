# $Id: makefile,v 3.11.4.20 1996/09/14 05:30:39 mvenugop Exp $
# $Source: /release/112/cvs/Xsys/obj-68-c4000/makefile,v $
#------------------------------------------------------------------
# makefile for c4000 (aka Dos-XX)
#
# 4/92, Marciano Pitargue
#
# Copyright (c) 1992-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.11.4.20  1996/09/14  05:30:39  mvenugop
# CSCdi69188:  Updating Gaia changes
# Branch: California_branch
#
# Revision 3.11.4.19  1996/09/09  22:16:29  ralls
# CSCdi68493:  Problems building c1000-boot image
# Branch: California_branch
#
# Revision 3.11.4.18  1996/09/05  22:35:36  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.11.4.17  1996/09/03  15:53:13  richl
# CSCdi67850:  rename images
#
# Revision 3.11.4.16  1996/08/28  13:01:46  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.11.4.15  1996/08/06  18:19:21  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.11.4.14  1996/07/14  07:13:24  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.11.4.13  1996/07/02  01:12:30  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-68-*)
# Branch: California_branch
#
# Revision 3.11.4.12  1996/06/26  01:00:47  sdatta
# CSCdi61316:  Update makefile
# Branch: California_branch
#
# Revision 3.11.4.11  1996/06/24  21:25:56  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.11.4.10  1996/06/12  00:16:59  fbordona
# CSCdi60152:  DLSw merge of low-end and RSP fast switch code for
#              modularity.
# Branch: California_branch
#
# Revision 3.11.4.9  1996/06/10  20:03:39  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.11.4.8  1996/06/04  23:28:55  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.11.4.7  1996/05/10  01:35:32  wmay
# CPP and ppp half-bridge commit
# Branch: California_branch
#
# Revision 3.11.4.6  1996/05/02  08:44:00  dwong
# Branch: California_branch
# Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
#
# Revision 3.11.4.5  1996/05/01  02:31:48  bew
# CSCdi56287:  Need to add crypto images into makefile targets
# Branch: California_branch
# Add crypto image to oddball list.
#
# Revision 3.11.4.4  1996/04/23  19:28:29  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.11.4.3  1996/04/16  19:08:10  rchandra
# CSCdi54830:  IP features commit
# Branch: California_branch
#
# Revision 3.11.4.2  1996/04/15  21:20:25  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.11.4.1  1996/03/18  21:15:39  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.6.12.3  1996/03/16  07:13:18  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.6.12.2  1996/03/09  05:07:57  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.6.12.1  1996/03/07  10:16:52  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.11  1996/03/07  21:50:53  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.10  1996/03/07  20:50:31  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.9  1996/02/26  21:51:07  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.8  1996/02/17  09:03:28  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.7  1996/02/15  18:02:52  gtaylor
# CSCdi45779:  Leased Line ISDN work.
#
# Revision 3.6  1996/01/06  05:16:54  hampton
# Split timer specific routines into multiple files.  Move the processor
# specific routines into the src-xx-xxx directories.  [CSCdi46482]
#
# Revision 3.5  1995/12/11  23:19:48  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.4  1995/11/21  23:58:21  jbalestr
# CSCdi43597:  BSTUN MIB not linked in for some images
#
# Revision 3.3  1995/11/17  17:52:41  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:10:18  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 2.13  1995/10/18  21:46:52  hampton
# Need a space conserving way to test build all images.  Add targets for
# production.test, oddballs.test, modular.test, and everything.test.
# These delete each target after making it to limit disk space usage.
# [CSCdi42405]
#
# Revision 2.12  1995/10/18  14:40:48  dblair
# CSCdi41356:  Default images of make files are not compressed
#         Change default images to be compressed image
#
# Revision 2.11  1995/10/10  07:24:28  sroach
# CSCdi41879:  Add BSC test command support
#
# Revision 2.10  1995/09/04  00:59:03  richl
# CSCdi39767:  Add BiSync to -r- images on low end
# Move bisync subsystems out of common code, into platforms on
# which it is supported.
#
# Revision 2.9  1995/08/22  18:55:32  jjohnson
# CSCdi39028:  minor makefile cleanup
# all the oddballs build cleanly
#
# Revision 2.8  1995/08/22  16:58:04  jjohnson
# CSCdi39028:  minor makefile cleanup
#
# Revision 2.7  1995/08/14  14:51:49  richl
# CSCdi38712:  dont build modular_notyet in everything target
#
# Revision 2.6  1995/08/07  05:40:29  richl
# CSCdi38315:  makefile rework
#
# Revision 2.5  1995/07/31  08:49:50  arothwel
# CSCdi37920:  Fix broken BSC/BSTUN MIB s/ware. Fix platform make for
# same.
#
# Revision 2.4  1995/07/17  23:27:49  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.3  1995/06/22  17:15:55  sberl
# CSCdi34459:  make production target should build official images
# Add appn sub/superset images to trunkd.test target.
#
# Revision 2.2  1995/06/08  22:07:24  jliu
# CSCdi35359:  Rename files for new hardware changes
#
# Revision 2.1  1995/06/07  21:52:13  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c4000-j-mz

PLATFORM_TIMER   = timer_generic.o timer_i8254.o
PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.68
include ../makefile

# need to stub out VLANS because it should not exist on this platform
VLANS =

#
# Platform specific definitions
#
PLATFORM		= XX
PLATFORM_FLAGS		= -D$(PLATFORM)
PLATFORM_IMAGE_GROUP	= 4000
PLATFORM_IMAGE_PREFIX	= C4000
HWFLAGS			= les

ROM_LDFLAGS =   -e __start -N -nostdlib -Ttext 0100d000 -Tdata 20000
FLASH_LDFLAGS = -e __start -N -nostdlib -Ttext 03000060 -Tdata 20000
RAM_LDFLAGS =   -e __start -N -nostdlib -Ttext 12000

PLATFORM_SUPPORT = sub_c4000.o 

#
# Non-modular stuff required to make the "modular" images link
#
PLATFORM_NONMODULAR = $(LANCE) $(MCN) $(DSX1)
NONMODULAR_PLATFORM_REQ = $(LANCE) $(MCN) $(SERIAL) $(SRBCORE) $(DSX1)
PLATFORM_MIBS = $(FLASH_LES_MIB) 

#
# Define drivers supported by this platform
#
BRI =		sub_bri.o
BSI =		sub_bsi.o
C4000_FDDI =	sub_c4000_fddi.o $(FDDI_SMT) $(BSI)
TEST =		sub_test.o
LANCE =		sub_lance.o
MCN =		sub_mcn.o
MK5025 =	sub_mk5025.o
TMS380 =	sub_tms380.o
HD64570 =	sub_hd64570.o
LES_SERIAL =	sub_les_serial.o
DSX1    =       sub_dsx1.o 
C4000_PRI = 	sub_c4000_pri.o
NONMOD = shr_tring.o

#
# These components, like drivers, are not supported by all platforms. 
# They are distinct from drivers for the purpose of gcc link ordering.
#
PLATFORM_PARTS = $(ISDN) $(V120) $(C4000_FDDI) $(C4000_PRI) $(CPP)

DRIVER_MIBS =	$(ETHERMIB) $(TRINGMIB) $(FDDIMIB) $(DS1MIB) $(CALLMIB) \
	        $(ISDNMIB) 

DRIVERS =	$(LANCE) $(MK5025) $(TMS380) $(HD64570) $(MCN) \
		$(BRI) $(DSX1) $(ETHER) $(SERIAL) $(TRING) $(LEX) \
		$(LES_SERIAL)

DRIVERS_BOOT =  $(DRIVERS)

# 
# Define platform specific mibs
# 
MIBS := $(MIBS) $(FLASH_LES_MIB) 

#
# This platform has additional comserver support, depending on the image.
#
NAS_P_OPTIONS= 

#
# Defines for fast switching
#
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
FAST_MEDIA 	 = fr_fast_les.o
FAST_FR 	 = fr_fast_les.o
FAST_TBRIDGE	 = tbridge.o  tbridge_les.o
FAST_VINES	 = vinesfast_les.o
FAST_XNS	 = xnsfast_les.o
FAST_VLANS	 = ieee_vlan.o isl_vlan.o vlan_les.o

PLATFORM_PLUS = $(FAST_IPNAT) $(IPNAT)

#
# c4000 subsystem definitions (note alphabetical order)
#

#
#sub_bri - The National TP3420 and the Munich 32 support for BRI 
#
sub_bri =       if_c4000_bri.o if_c4000_mb.o if_les_bri.o \
		if_les_bri_chain.o if_les_leased_bri.o bri_debug.o \
		if_c4000_bri_chain.o msg_bri.o
#
# sub_bsi - National Semiconductor BSI/BMAC support for FIM
#
sub_bsi = 	asm_c4000_bsi.o c4000_bsi.o cam.o bsi_xilinx.o \
		c4000_fddi_chain.o msg_les_fddi.o

#
# sub_c4000 - non speed critical components of the core c4000 support
#
sub_c4000 =	asm_c4000.o diag.o diag_68.o diag_chain.o if_c4000_chain.o \
		msg_nim.o xx.o xx_nim.o \
		platform_xx.o c4000_nim.o c4000_nim_snmp.o xx_snmp.o \
		if_les.o les_chain.o les_console.o \
		nv_les.o xx_nv.o file_io_msgs.o \
            	flash_dvr_les.o dev_io.o file_io.o flash_fs_les.o \
 		xx_flash.o flash_partition.o flash_cmnds.o flash_services.o \
 		boot_flash_cmnds.o flash_services_les.o snmp_flash_cmnds.o 

#
# sub_c4000_fddi - non speed critical components for c4000 FDDI
#
sub_c4000_fddi = c4000_fddi.o fddi.o fddi_debug.o msg_fddi.o csppcm.o \
                 csppcode.o xx_fddi.o fddi_chain.o c4000_xdi.o \
		 fddi_parser.o

#
# sub_dsx1 - C4000 dsx1 subsystem for CE1/T1
#
sub_dsx1 = if_controller.o if_controller_chain.o if_msg_controller.o \
           dsx1.o dsx1_chain.o msg_dsx1.o \
           les_controller.o if_c4000_dsx1.o if_c4000_ce1.o \
           if_c4000_bt8360.o if_c4000_dsx1_framer.o c4000_e1_firmware.o

#
#PRI support on 4000
#
sub_c4000_pri = ccpri.o ccpmsg.o ccpcc.o cc_serv.o

#
# sub_lance - AMD Lance support for EIM and DEIM
#
sub_lance  =	asm_c4000_lance.o if_les_lance.o if_c4000_lance.o \
	        xx_lance.o msg_lance.o if_les_lance_chain.o \
		if_les_ether.o if_c4000_lance_parser.o

#
# sub_les_serial.o - Generic LES serial support
#
sub_les_serial = if_les_serial.o

#
# sub_lex_platform - Lex les support.
#
sub_lex_platform = lex_sub_les.o lex_fs_les.o

#
# sub_mcn - The Munich 32 and the MCN subsystem
#
sub_mcn    =	asm_c4000_m32.o if_c4000_mcn.o if_c4000_m32.o \
		if_les_m32.o xx_mcn.o

#
# sub_mk5025 - SGS/Thomson MK5025 support for DSIM
#
sub_mk5025 =	asm_c4000_mk5025.o if_les_mk5025.o if_c4000_mk5025.o \
		xx_mk5025.o msg_mk5025.o

sub_test =      les_test.o transmit_test.o

#
# sub_tms380 - Texas TMS380 support for TRIM
#
sub_tms380 =	asm_c4000_tms380.o if_les_tms380.o if_c4000_tms380.o \
		xx_tms380.o if_les_madge.o asm_les_madge.o

#
# sub_hd64570 - Hitachi Hd64570 support for QSIM
#
sub_hd64570 =   asm_c4000_hd64570.o if_les_hd64570.o if_c4000_hd64570.o \
                xx_hd64570.o msg_hd64570.o


sub_bri.o			: $(sub_bri)
sub_bsi.o			: $(sub_bsi)
sub_c4000.o			: $(sub_c4000)
sub_c4000_fddi.o		: $(sub_c4000_fddi)
sub_dsx1.o                      : $(sub_dsx1)
sub_c4000_pri.o			: $(sub_c4000_pri)
sub_hd64570.o			: $(sub_hd64570)
sub_lance.o			: $(sub_lance)
sub_les_serial.o		: $(sub_les_serial)
sub_lex_platform.o		: $(sub_lex_platform)
sub_mcn.o			: $(sub_mcn)
sub_mk5025.o			: $(sub_mk5025)
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
production_images = c4000-ajs-mz c4000-i-mz c4000-is-mz \
		    c4000-d-mz c4000-ds-mz c4000-j-mz c4000-js-mz \
		    c4000-ainr-mz c4000-p-mz c4000-boot-r

production: $(production_images)

production_test_images = $(addsuffix .test,$(production_images))
production.test: $(production_test_images)

#
# Crypto production available only to the crypto gang and release meisters
#
crypto_images = c4000-ajs40-mz c4000-ajs56-mz c4000-is40-mz c4000-is56-mz \
	        c4000-ds40-mz  c4000-ds56-mz  c4000-js40-mz c4000-js56-mz 

crypto: $(crypto_images)

crypto_test_images = $(addsuffix .test,$(crypto_images))
crypto.test: $(crypto_test_images)

#
# The "unofficial" engineering specials
#
oddball_notyet = 
oddball_images = c4000-tatalk-m c4000-tddr-m c4000-tic-m c4000-tip-m \
	         c4000-tiv-m c4000-tjmanu-m c4000-tjpgen-m c4000-tjrem-m \
		 c4000-tjtcl-m c4000-tli-m 

oddballs: $(oddball_images)
	rm -f $(oddball_images) 

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

include $(SYSROOT)/obj-68-c4000/make.gaia

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Build all images 
#
everything: production modular oddballs 
	
#
# Generic Link rules
#
c4000-%-m.czsun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c4000-%-m.sun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c4000-%-r.sun: lock main-gs-%-r gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(ROM_LDFLAGS) -o gs-$*-r.temp \
	Main_main-gs-$*-r.o cisco-gs-$*-r.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-r.temp $@

.PRECIOUS: c4000-%-m.sun c4000-%-r.sun c4000-%.czsun

#
# Image rules
#
c4000-%-r: c4000-%-r.rom
	@
c4000-%-m: c4000-%-m.tftp
	@

include ../makedist
############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies
