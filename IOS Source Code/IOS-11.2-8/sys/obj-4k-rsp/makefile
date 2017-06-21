# $Id: makefile,v 3.33.4.45 1996/09/11 20:23:36 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-4k-rsp/makefile,v $
#------------------------------------------------------------------
# Makefile for RSP system software
#
# Chris Shaker
#
# Copyright (c) 1994-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.33.4.45  1996/09/11  20:23:36  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
# Corrected LANEMIBS to LANEMIB
#
# Revision 3.33.4.44  1996/09/10  03:00:14  cheilek
# CSCdi61961:  need to add crypto target to svip makefile
# Branch: California_branch
# Bundle crypto VIP microcode with crypto images.
#
# Revision 3.33.4.43  1996/09/05  22:40:31  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.33.4.42  1996/09/03  15:52:27  richl
# CSCdi67850:  rename images
#
# Revision 3.33.4.41  1996/08/29  17:40:54  drowell
# CSCdi46093:  bundling the ucode is way to slow
# Branch: California_branch
# Introduced new mkbndl2 script which bundles ucode by directly generating
# an ELF object file. Also allows selection of ucode by flags in the ucode
# list file.
#
# Revision 3.33.4.40  1996/08/28  13:01:22  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.33.4.39  1996/08/15  18:07:06  ptran
# Add ESA's idb to VIP and RSP hwidb/swidb chain.
# CSCdi64021:  show diag doesnt display EPA info when the next PA is empty
# Branch: California_branch
#
# Revision 3.33.4.38  1996/08/07  20:59:49  mleelani
# CSCdi65339:  Tunnel fastswitching support for RSP broken for multicast
# Branch: California_branch
# Support tunnel fastswitching for multicast packets.
#
# Revision 3.33.4.37  1996/08/06  18:18:52  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.33.4.36  1996/07/17  18:15:44  skufer
# CSCdi63216:  rsp-dw-m microcode appears as interim release, rev 200.0
# Branch: California_branch
#
# Revision 3.33.4.35  1996/07/15  02:15:08  bchan
# CSCdi50608:  makefile for mib incorrect
# Branch: California_branch
#
# Revision 3.33.4.34  1996/07/14  07:12:59  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.33.4.33  1996/07/12  22:08:19  cheilek
# CSCdi62623:  build fails for rsp-ajv40-mz, rsp-jv40-mz, ...
# Branch: California_branch
# Check for presence of a 'v' anywhere in the second field of the image
# name, not just at the end.
#
# Revision 3.33.4.32  1996/07/09  20:56:02  ralls
# CSCdi62324:  Remove PLATFORM_SUPPORT_V from 7000 and RSP
# Branch: California_branch
#
# Revision 3.33.4.31  1996/07/08  22:35:34  mwu
# CSCdi60922:  11.2 Core Features Commit
# Branch: California_branch
#
# Revision 3.33.4.30  1996/07/02  01:07:32  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-4k-*)
# Branch: California_branch
#
# Revision 3.33.4.29  1996/06/26  01:10:55  sdatta
# CSCdi61319:  update makefile
# Branch: California_branch
#
# Revision 3.33.4.28  1996/06/24  21:25:37  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.33.4.27  1996/06/21  07:44:04  gchristy
# CSCdi60987:  RSP: support fastswitching of GRE/IPIP tunnels
# Branch: California_branch
#
# Revision 3.33.4.26  1996/06/12  07:40:43  cheilek
# CSCdi58294:  vip build modifies files under CVS control
# Branch: California_branch
# Automatically build VIP and VIP2 microcode as needed for *-*v-* images.
# While we're here, fix *-*v-* images to again work for vip2.
#
# Revision 3.33.4.25  1996/06/11  23:09:35  ralls
# CSCdi60023:  Gaia update
# Branch: California_branch
#
# Revision 3.33.4.24  1996/06/10  20:05:00  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.33.4.23  1996/06/04  23:33:39  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.33.4.22  1996/06/04  00:42:21  ddecapit
# CSCdi59346:  Makefile changes reqd. to move TRLANE to non-production
# images
# Branch: California_branch
#
# Revision 3.33.4.21  1996/05/17  11:33:27  ppearce
# Merge IbuMod into Calif
#
# Revision 3.26.2.6  1996/05/13  07:03:43  bchan
# CSCdi50608:  makefile for mib incorrect
# Branch: IbuMod_Calif_branch
#
# Revision 3.26.2.5  1996/05/06  00:06:14  ppearce
# Sync to IbuMod_Calif_baseline_960504
#
# Revision 3.26.2.4  1996/04/03  20:10:56  ppearce
# Sync to IbuMod_Calif_baseline_960402
#
# Revision 3.26.2.3  1996/03/28  23:22:23  bchan
# CSCdi52724:  merge tn3280 server code
# Branch: IbuMod_Calif_branch
# move tn3270_server_alert.c from sub_snanm to sub_channel
#
# Revision 3.26.2.2  1996/03/28  11:29:45  bchan
# CSCdi52724:  merge tn3280 server code
# Branch: IbuMod_Calif_branch
#
# Revision 3.26.2.1  1996/03/17  18:12:04  ppearce
# Sync IbuMod_Calif_branch to V111_1_3
#
# Revision 3.33.4.20  1996/05/10  01:35:12  wmay
# CPP and ppp half-bridge commit
# Branch: California_branch
#
# Revision 3.33.4.19  1996/05/09  14:34:43  rbadri
# Branch: California_branch
# LANE, UNI3.1 features
#
# Revision 3.33.4.18  1996/05/08  12:01:48  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.33.4.17  1996/05/08  04:10:30  cheilek
# CSCdi55966:  tftpboot fails via VIP2-4E
# Branch: California_branch
# Fix dependency.
#
# Revision 3.33.4.16  1996/05/08  04:00:04  ralls
# CSCdi57036:  Update the gaia features
# Branch: California_branch
#
# Revision 3.33.4.15  1996/05/02  08:43:22  dwong
# Branch: California_branch
# Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
#
# Revision 3.33.4.14  1996/04/30  21:17:32  dkerr
# CSCdi43050:  IP lookup bottleneck for RSP in ISP test setup
# Branch: California_branch
#
# Revision 3.33.4.13  1996/04/30  20:14:37  ralls
# CSCdi56220:  Update Gaia features
# Branch: California_branch
#
# Revision 3.33.4.12.2.1  1996/04/27  07:16:28  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.33.4.12  1996/04/25  23:21:56  ronnie
# CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
# Feature commit for ATM and Frame Relay into 11.2
# Branch: California_branch
#
# Revision 3.33.4.11  1996/04/23  19:32:25  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.33.4.10  1996/04/22  18:58:38  ssangiah
# CSCdi54920:  Need to add support for the flash MIB on c7200.
# Branch: California_branch
#
# Revision 3.33.4.9  1996/04/19  15:22:43  che
# CSCdi55138:  IosSec_branch feature commit
# Branch: California_branch
# IOS Security feature commit
#
# Revision 3.33.4.8  1996/04/16  19:08:02  rchandra
# CSCdi54830:  IP features commit
# Branch: California_branch
#
# Revision 3.33.4.7  1996/04/15  21:20:08  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.33.4.6  1996/04/12  23:11:46  mdb
# CSCdi54154:  Need to remove old files
# Branch: California_branch
# Remove last traces of fslib_string.c
#
# Revision 3.33.4.5  1996/04/12  17:00:06  mdb
# CSCdi54154:  Need to remove old files
# Branch: California_branch
# Remove references to gutted files in the makefiles and nuke the gutted
# files.
#
# Revision 3.33.4.4  1996/04/11  19:11:01  jwelder
# CSCdi53423:  RSP cannot tftpboot via VIP
# Branch: California_branch
#
# Revision 3.33.4.3.4.1  1996/04/16  18:52:20  cyoung
# Take VTP out of the production images and make it an oddball
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.33.4.3  1996/03/29  00:42:16  hochan
# CSCdi52960:  Move -k- images to oddballs
# Branch: California_branch
#
# Revision 3.33.4.2  1996/03/21  22:58:07  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.33.4.1  1996/03/18  21:14:28  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.4.2.5  1996/03/18  23:16:54  mbeesley
# Branch: ELC_branch
# Sync to V111_1_3
#
# Revision 3.4.2.4  1996/02/10  00:06:31  mbeesley
# CSCdi48581:  Add VIP2 support to ELC_branch
# Branch: ELC_branch
#
# Revision 3.4.2.3  1996/02/08  08:23:56  gstovall
# Branch: ELC_branch
# Sync to ELC_baseline_960205.
#
# Revision 3.4.2.2  1996/01/15  23:55:24  dcarroll
# Branch: ELC_branch
# Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
#
# Revision 3.4.2.1  1995/12/11  19:03:05  mbeesley
# Branch: ELC_branch
# Sync ELC_branch to ELC_baseline_120995.
#
# Revision 3.33  1996/03/11  07:31:41  jwelder
#         Vip micro code is now only in images:
#         rsp-kv-mz, rsp-akv-mz, rsp-jv-mz, rsp-ajv-mz, rsp-pv-mz
#
#
# CSCdi49379:  must restrict VIP ucode to specific RSP subsets
#
# Revision 3.32  1996/03/09  00:48:15  nitin
# CSCdi50416:  Move the non supported images to oddball
# Changed the default image for rsp & gs7 to be -j-. Moved the -k- images
# to oddballs.
#
# Revision 3.31  1996/03/07  21:50:34  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.30  1996/03/07  20:50:08  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.29  1996/03/06  19:11:28  foster
# CSCdi49134:  HSA : OIR event causes router to crash
# makefile cleanup moving some objects used in both image to the COMMON
# list
#
# Revision 3.28  1996/03/04  18:58:56  dkerr
# CSCdi50623:  IP flow cache needs better accounting granularity
# Broke build in last commit.  Let's get everything this time...
#
# Revision 3.27  1996/03/01  02:07:57  jack
# Added the images that include the "v" stuff for VIP support.
# rsp-jv-mz rsp-ajv-mz rsp-pv-mz.
#
# CSCdi48618:  7500 makefiles need to be updated to make rspv-* images
#
# Revision 3.26  1996/02/26  21:50:40  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.25  1996/02/17  09:01:38  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.24  1996/01/25  11:20:49  smackie
# Registry code gets some much-needed hot-oil massaging, free liposuction
# and a makeover.  (CSCdi47101)
#
#    o Compress registry structures. Saves over 120K for all platforms
#    o Add registry subsystems and remove feature registry initializion
#      from registry.c to fix woeful initialization scaling problems
#    o Remove unused and seemingly unloved registry debugging code
#    o Add registry memory statistics to shame people into action
#
# Revision 3.23  1996/01/21  17:14:38  schaefer
# CSCdi45761: support RSP in new chassis
# oops, finish rework of master/slave object lists
#
# Revision 3.22  1996/01/15  18:04:45  schaefer
# CSCdi45761: support RSP in new chassis
# rework master/slave object lists
#
# Revision 3.21  1996/01/09  00:58:26  wmay
# CSCdi46498:  add -aj-mz version to production image
#
# Revision 3.20  1996/01/07  13:52:31  banderse
# CSCdi45351:  show microcode should display CIP flash default
# removed xxx_flash_ucode reference.
#
# Revision 3.19  1996/01/06  05:16:35  hampton
# Split timer specific routines into multiple files.  Move the processor
# specific routines into the src-xx-xxx directories.  [CSCdi46482]
#
# Revision 3.18  1996/01/06  03:20:37  gwynne
# CSCdi46437:  obj-4k-rsp and obj-4k-rvip makefiles need to use $(COLON)
#
# Revision 3.17  1996/01/05  20:52:43  mbeesley
# CSCdi46502:  Ucode bundle linked twice in ziped images
# Use seperate linker script for the self-decompressor.
#
# Revision 3.16  1995/12/28  08:20:30  foster
# CSCdi46157:  Dbus down-load needs to work on rsp2 slave down-load
# add more link scripts and change the load point.
#
# Revision 3.15  1995/12/23  02:25:36  foster
# CSCdi45893:  parallel builds are broken for rsp-k-m*
#
# Revision 3.14  1995/12/15  18:01:36  yanke
# Move ipc subsystem to platform to fix rsp modularity problem.
#
# CSCdi45442:  ipc breaks rsp modularity
#
# Revision 3.13  1995/12/12  09:08:12  dbath
# CSCdi45422:  HSA systems forget what time it is on failover
# Add calendar.o back into the slave image.
#
# Revision 3.12  1995/12/11  23:19:31  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.11  1995/12/07  20:07:57  foster
# CSCdi44432:  rsp slave image dependenices are broken
# changes from code review comments
#
# Revision 3.10  1995/12/06  21:52:22  foster
# CSCdi44432:  rsp slave image dependenices are broken
#
# Revision 3.9  1995/12/05  00:50:31  wmay
# CSCdi44967:  7000 and RSP need -j images for V.120 support
#
# Revision 3.8  1995/12/01  19:03:54  sdurham
# CSCdi44362:  envmon is non-modular w.r.t envmon mib.
#   Remove direct calls from envmon code to mib code. Use service points.
#
# Revision 3.7  1995/11/29  18:45:46  banderse
# CSCdi43438:  CIP needs default microcode flash image
# Added release specific default flash image; changes to ensure flash file
# system is initialized completely before IP download.
#
# Revision 3.6  1995/11/27  21:32:39  yanke
# Add cbus protection module to master/slave rsp images.
#
# CSCdi44478:  slave rsp crashes or resets during ips eoir events
#
# Revision 3.5  1995/11/22  23:42:43  sdurham
# CSCdi35282:  Need command to configure envmon trap enables
#      add individual trap options in separate parser-mib file
#      envmon_mib_chain.
#
# Revision 3.4  1995/11/20  02:35:07  shj
# CSCdi44018:  LANEMIB not included in rsp images
# Add LANMIB to DRIVER_MIBS.
#
# Revision 3.3  1995/11/17  17:52:07  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:09:14  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 2.20  1995/10/18  21:45:24  hampton
# Need a space conserving way to test build all images.  Add targets for
# production.test, oddballs.test, modular.test, and everything.test.
# These delete each target after making it to limit disk space usage.
# [CSCdi42405]
#
# Revision 2.19  1995/10/18  14:40:41  dblair
# CSCdi41356:  Default images of make files are not compressed
#         Change default images to be compressed image
#
# Revision 2.18  1995/09/26  21:29:10  motto
# Decouple IPC from RSP subsystem.
# CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
#
# Revision 2.17  1995/09/22  04:52:10  richl
# CSCdi40872:  apply CSCdi39018 to mainline
#
# Revision 2.16  1995/09/17  20:53:53  motto
# CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
#
# Revision 2.15  1995/09/14  01:52:17  biren
# CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
#
# Revision 2.14  1995/08/22  16:57:45  jjohnson
# CSCdi39028:  minor makefile cleanup
#
# Revision 2.13  1995/08/12  07:57:34  richl
# CSCdi38686:  more ipmulticast fastswitching rearrangement
#
# Revision 2.12  1995/08/09  00:21:14  kramling
# CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
# 10.3.
#
# Revision 2.11  1995/08/07  05:39:56  richl
# CSCdi38315:  makefile rework
#
# Revision 2.10  1995/07/27  22:18:02  schaefer
# CSCdi36819: sys/obj-68-c7000/makefile change
# apply to boot RSP microcode bundle
#
# Revision 2.9  1995/07/17  23:27:31  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.8  1995/07/17  08:21:39  schaefer
# CSCdi36181: more robust microcode management (on RSP)
#
# Revision 2.7  1995/07/12  15:57:34  csilva
# CSCdi36819:  sys/obj-68-c7000/makefile change
# sys/hes/rsp_ucode.bndl.c will only be remade, depending upon the actual
# microcode listed in the rsp_ucode.files.
#
# Revision 2.6  1995/07/02  06:53:53  shaker
# CSCdi36675:  Fix link problem from CSCdi36167
# Add missing msg_aip.o to obj-4k-rsp/makefile
#
# Revision 2.5  1995/06/23  23:05:43  tli
# CSCdi36020:  Add service provider subset for 7500
#
# Revision 2.4  1995/06/23  22:23:40  mbeesley
# CSCdi36360:  95q1 compiler fails to build czip_elf
# Work-arround the compiler bug by declaring variables static.
# This allows zipped images for c4500, rsp to ship in 11.0.
#
# Revision 2.3  1995/06/22  17:15:35  sberl
# CSCdi34459:  make production target should build official images
# Add appn sub/superset images to trunkd.test target.
#
# Revision 2.2  1995/06/10  01:56:38  smackie
# Add IPC to the SYSTEM_FOO declarations. (CSCdi35646)
#
# Revision 2.1  1995/06/07  21:51:08  hampton
# Bump version numbers from 1.x to 2.x.
#
#$Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: rsp-jv-mz

PLATFORM_TIMER   = timer_generic.o timer_nevada.o asm_4k_timer.o
PLATFORM_OTHER   = rsp_ukernel.o rsp_vspace.o asm_4k_rsp.o
PLATFORM_MONITOR = asm_4k_monitor.o

include ../makefile.tools
include ../makefile.4k
include ../makefile

#
# Platform specific definitions
#
PLATFORM		= RSP
PLATFORM_FLAGS		= -D$(PLATFORM)
PLATFORM_IMAGE_GROUP	= RSP
PLATFORM_IMAGE_PREFIX	= RSP
HWFLAGS			= gs

#
# this section is used to build the ucode header for the slave image.
#

UCODE_VERSION	= 20.00
RSP2_TYPE	= 15		# this is FCI_RSP2_CONTROLLER from hes/if_fci.h
RSP_HW_VERSION	= 1.0		# what rev are we? XXX
RSP_HARDWARE	= RSP2_HW

CODE_START	= 0x60010000
ZCODE_START	= 0x80010000

START	    = __start

#
# Ucode definitions
#

UCODE_DIR        = ../../micro
RSP_UCODE_FILES  = $(UCODE_DIR)/rsp_ucode.files

#
# These flags indicate what ucode goes into the three flavors of bundle.
#
UCODE_FLAGS =
VUCODE_FLAGS = +VIP1 +VIP2 +HSA +POSIP
CVUCODE_FLAGS = +VIP1 +VIP2 +CRYPTO +HSA +POSIP
BOOT_UCODE_FLAGS = +VIP1 +VIP2 +BOOT +POSIP

#
# Version number extraction from rsp_ucode.files
#

EXTRACT_SVIP_VER_MAJ_CMD := $(shell perl -n -e 'print "$$1" if /micro\/vip2_(\d+)-(\d+)/' < $(RSP_UCODE_FILES))
EXTRACT_SVIP_VER_MIN_CMD := $(shell perl -n -e 'print "$$2" if /micro\/vip2_(\d+)-(\d+)/' < $(RSP_UCODE_FILES))

SVIP_IMAGE = $(UCODE_DIR)/vip2_$(EXTRACT_SVIP_VER_MAJ_CMD)-$(EXTRACT_SVIP_VER_MIN_CMD)

EXTRACT_CRYPTO_SVIP_VER_MAJ_CMD := $(shell perl -n -e 'print "$$1" if /micro\/vip2c_(\d+)-(\d+)/' < $(RSP_UCODE_FILES))
EXTRACT_CRYPTO_SVIP_VER_MIN_CMD := $(shell perl -n -e 'print "$$2" if /micro\/vip2c_(\d+)-(\d+)/' < $(RSP_UCODE_FILES))

CRYPTO_SVIP_IMAGE = $(UCODE_DIR)/vip2c_$(EXTRACT_CRYPTO_SVIP_VER_MAJ_CMD)-$(EXTRACT_CRYPTO_SVIP_VER_MIN_CMD)

EXTRACT_RVIP_VER_MAJ_CMD := $(shell perl -n -e 'print "$$1" if /micro\/vip(\d+)-(\d+)/' < $(RSP_UCODE_FILES))
EXTRACT_RVIP_VER_MIN_CMD := $(shell perl -n -e 'print "$$2" if /micro\/vip(\d+)-(\d+)/' < $(RSP_UCODE_FILES))

RVIP_IMAGE = $(UCODE_DIR)/vip$(EXTRACT_RVIP_VER_MAJ_CMD)-$(EXTRACT_RVIP_VER_MIN_CMD)

#
# This is a little nuts, but there are 4 different *.link files for rsp images.
# Each file differs by one statement:
#
# rsp-k-m* (and all master router images) uses rsp.link and has the added line:
#				rsp_ucode_bndl.o(.rodata)
# this places the ucode bundle in ro data which is a Good Thing, and allows a 
# non-compressed 4k images to be in the bundle (for debugging).
#
# rsp-*v*-m* (all VIP/HSA images) use rspv.link and has the added line:
#				rspv_ucode_bndl.o(.rodata)
# 
# rsp-dw-* (which is a slave image) uses rspdw.link and has no added lines.
#
# rsp-boot-* (all boot images) uses rspboot.link and has the added line:
#  				rspboot_ucode_bndl.o(.rodata)
# this places the boot ucode (different then system ucode) in ro data.
# note that LINK_ and FILE_ never both eval for the same target


define LINK_
rsp$(findstring dw,$@)
endef

# To see if this is a CRYPTO image, check to see if there's a '40' 
# or '56' between the two '-'s.

_CRYPTO_ = $(shell echo $(patsubst fbi-%,%,$@) | perl -n -e 'print "c\n" if /^[^-]+-[^45-]*(40|56)[^-]*-[^-]+$$/;')

# To see if this is a VIP/HSA image, check to see if there's a 'v'
# between the two '-'s.  Simply looking for a 'v' is insufficient, 
# since modular-vax contains that character.

_VIP_ = $(shell echo $(patsubst fbi-%,%,$@) | perl -n -e 'print "v\n" if /^[^-]+-[^v-]*v[^-]*-[^-]+$$/;')

define _FILE
$(findstring boot,$@).link
endef



RAM_LDFLAGS = -e $(START) -N $(GNUM) -Ttext $(CODE_START)  \
		-T$(LINK_)$(_CRYPTO_)$(_VIP_)$(_FILE)  -nostdlib
ZIP_LDFLAGS = -e $(START) -N $(GNUM) -Ttext $(ZCODE_START) \
		-Trspczip.link  -nostdlib

ENVMIB = sub_envmib.o
RSPASM = asm_4k.o asm_4k_rsp.o asm_4k_timer.o
RSPFASTASM = ipasm_4k.o

#
# Redefinition of common code to include C7000 extensions
#
IPSERVICES :=		$(IPSERVICES) sub_dbus_io.o
PGEN := 		$(PGEN) sub_pgen_rsp.o

IPRSP =		$(IPC_UDP) sub_dbus_io.o
IP_FAST_RSP = ipfast_les.o ipfast_rsp.o ipasm_4k.o ipfast_rsp_ipc.o ipfast_flow.o

#
# Sched alloc is used on this platform - hence override this variable
# so that support for sched-interval is not pulled in
#
SCHED_INTERVAL =

#
# Non-modular stuff required to make the "modular" images link
#
PLATFORM_NONMODULAR = $(ATM) $(ATM_ARP) $(FDDI_SMT) \
                      $(IPC_CBUS_MRSP) $(IPC_CBUS_RSP)

NONMODULAR_PLATFORM_REQ = $(ATM) $(FDDI_SMT) $(TRING) $(SERIAL) \
$(IPC_CBUS_RSP) $(IPC_CBUS_MRSP) $(ISDN_FEATURE) $(SRBCORE)

PLATFORM_MIBS = $(ENVMIB) $(FSIPMIB) $(DS1MIB) $(FLASHMIB) $(ATOMMIB_ES)


#
# Define drivers supported by this platform
#

NONMOD = shr_tring.o

PLATFORM_SUPPORT = sub_rsp.o $(IPC)
ISDN_RSP = 	sub_rsp_pri.o
IPC_CBUS_RSP =	sub_ipc_cbus_rsp.o 
IPC_CBUS_MRSP = sub_ipc_cbus_mrsp.o
CHANNEL =	sub_channel.o
TN3270S =	sub_tn3270s.o
TN3270SMIB =    sub_tn3270smib.o
UCODE = 	rsp_ucode_bndl.o
UCODE_TOC =     rsp_ucode_toc.o
UCODE_TOC_SRC = ../hes/rsp_ucode_toc.c
UCODE_DATA =    rsp_ucode_data.o
UCODEV =        rspv_ucode_bndl.o
UCODEV_TOC =    rspv_ucode_toc.o
UCODEV_TOC_SRC = ../hes/rspv_ucode_toc.c
UCODEV_DATA =   rspv_ucode_data.o
UCODECV =       rspcv_ucode_bndl.o
UCODECV_TOC =   rspcv_ucode_toc.o
UCODECV_TOC_SRC = ../hes/rspcv_ucode_toc.c
UCODECV_DATA =  rspcv_ucode_data.o
UCODE_BOOT = 	rspboot_ucode_bndl.o 
UCODE_BOOT_TOC = rspboot_ucode_toc.o 
UCODE_BOOT_TOC_SRC = ../hes/rspboot_ucode_toc.c
UCODE_BOOT_DATA = rspboot_ucode_data.o 

CRYPTO_ENGINE_RSP = crypto_swepa_rsp.o crypto_engine_rsp_ipc.o
#need to stub out APPNMIB_SHARE so appn images will not pick the share image again
APPNMIB_SHARE=

DRIVER_MIBS =	$(ETHERMIB) $(TRINGMIB) $(DS1MIB) $(FDDIMIB) \
                $(CHANNELMIB) $(TN3270SMIB) $(APPNMIB_TN3270S) \
	        $(FSIPMIB) $(ISDNMIB) $(CALLMIB) $(ATMMIB) $(ATOMMIB_ES) 

DRIVERS	=	$(ATM) $(ATM_ARP) $(FDDI_SMT) $(ETHER) $(SERIAL) $(TRING) \
		$(ISDN) $(LEX) $(ISDN_RSP) $(IPC_UDP) \
                $(CHANNEL)  $(TN3270S)                          \
		$(IPC_CBUS_RSP) $(IPC_CBUS_MRSP) $(V120) $(CPP)

DRIVERS_BOOT =  $(ATM) $(ATM_ARP) $(FDDI_SMT) $(ETHER) $(SERIAL) $(TRING) \
		$(ISDN) $(LEX) $(ISDN_RSP)  $(IPC_UDP) \
		$(IPC_CBUS_RSP) $(IPC_CBUS_MRSP)
#
# Defines for slave support
#
PLATFORM_SUPPORT_DSW = sub_srsp.o
DRIVERS_DSW =


#
#
# old flash MIB not supported, use new one instead
#
FLASHMIB=$(FLASH_LES_MIB)

# 
# Add in platform specific mibs
#
MIBS := $(MIBS) $(ENVMIB) 

#
# This platform has additional comserver support, depending on the image.
#
NAS_P_OPTIONS= 

#
# This defines the option which will be compiled on for the crypto image.
#
CRYPTO_RSP_OPTION= $(CRYPTO_ENGINE_RSP)

#
# Defines for fast switching
#
FAST_MEDIA = fr_fast_les.o lex_fs_les.o lanefast_les.o
FAST_FR = fr_fast_les.o
FAST_LEX = lex_fs_les.o
FAST_LANE = lanefast_les.o
FAST_ATALK = at_fast_les.o
FAST_ATSMRP = at_smrpfast_les.o at_smrpfast.o smrp_fast_les.o
FAST_CLNS = clnsfast_les.o
FAST_DECNET = dnfast_les.o
FAST_IBM = fastsrb.o rsp_srb.o fastdlsw.o
FAST_IP = ipfast_les.o ipfast_rsp.o ipasm_4k.o ipfast_rsp_ipc.o \
	ipfast_flow.o ipfast_rsp_mtrie.o udp_flood_fs.o
FAST_IPMCAST = ipmfast_les.o
FAST_TUNNEL  = tfast_les.o
FAST_IPNAT = ipnat_les.o
FAST_IPX = novfast_les.o
FAST_TBRIDGE = tbridge.o tbridge_rsp.o 
FAST_VINES = vinesfast_les.o
FAST_XNS = xnsfast_les.o
RSPFASTOBJ = ipfast_les.o 
FAST_VLANS = ieee_vlan.o isl_vlan.o vlan_rsp.o

#
# Define features for the PLUS feature set
#
PLATFORM_PLUS = $(IPNAT) $(FAST_IPNAT) $(LANE) $(LANEMIB) $(VLANS)

$(CPUASM) $(RSPASM) $(RSPFASTASM) $(CZIPASM):
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)$(AS) -c $(ASWITCHES) $(WARN_ALL) $< \
	|| (rm -f $@ ; exit 1)

###########################################################################
#
# Platform-specific subsystems
#
###########################################################################

# objects common to master & slave images
RSP_COMMON =	rsp_init.o platform_rsp.o \
		rsp_cbus_monitor.o \
		rsp_ipc_cbus.o \
		nv_les.o rsp_nv.o les_console.o \
		util_4k.o monvar.o \
		diag.o diag_4k.o diag_chain.o \
		calendar.o \
		if_qa.o rsp_chain.o \
		msg_chasint.o \
		msg_rsp.o \
		hes_test.o \
		cbus_debug.o \
		transmit_test.o \
		qa_mgr.o \
		rsp_monlib_bndl.o \
		remote_filesys_server.o \
		remote_tty_server.o remote_tty_client.o \
		nv_filesys.o fs_rec.o \
		filesys.o flash_filesys.o \
		fslib.o clpd6720.o \
		rsp_fslib_dev.o rsp_ralib.o \
		fslib_dev_intel_series_2plus.o fslib_dev_rsp_internal_flash.o \
		fslib_print.o fslib_dev_rom.o fslib_dev_ram.o rsp_flash.o \
		filesys_debug.o flash_config.o flash_command.o \
		cbus_registry.o 

# objects specific to the master image
sub_rsp =	$(RSP_COMMON) \
		rsp_if.o \
		if_controller.o if_controller_chain.o if_msg_controller.o \
		hes_controller.o \
		rsp_flashmib.o file_io_msgs.o \
		flash_cmnds_hes.o flash_services.o flash_services_hes.o \
		ucode.o msg_ucode.o ucode_compress.o \
		msg_dbus.o dbus.o dbus_chain.o \
		cbus_chain.o hes_chain.o \
		cbus_test_chain.o \
		msg_cbus.o \
		rsp_oir.o msg_oir.o oir_chain.o oir_debug.o \
		rsp_test.o \
		qa_util.o \
		msg_qa.o \
		if_hes_common.o \
		rsp_common_if.o \
		rsp_cmd.o \
		rsp_cbus.o rsp_dbus.o \
		rsp_commands.o \
		aip_parse_support.o \
		atm_aip_chain.o \
		if_hes_ether.o \
		if_hes_serial.o if_hes_serial_parser.o rsp_serial.o \
		rsp_eip.o \
		rsp_aip.o \
		rsp_trip.o \
		rsp_mip.o \
		if_aip.o msg_aip.o \
		if_trip.o \
		rsp_fip.o hes_fddi.o hes_xdi.o fddi.o fddi_chain.o \
		fddi_parser.o fddi_debug.o msg_fddi.o \
		c7000_rsp_fip.o \
		dsx1_chain.o dsx1.o if_mip.o \
		rsp_macaddr.o \
		rsp_chasint.o envmon_chain.o \
		rsp_tmpstubs.o \
		tftp_rcp_filesys.o \
		remote_filesys_client.o \
		rsp_fslib_test.o \
		filesys_test_cmd.o \
		filesys_test_chain.o \
		rsp_rvip.o rsp_vip.o if_rvip.o if_vip.o if_vip_4t.o \
		rvip_debug.o rsp_svip.o if_svip.o svip_debug.o \
		if_feip.o rsp_feip.o \
		fast_ethernet_chain.o fast_ethernet_debug.o \
		posip_chain.o \
		if_posip.o rsp_posip.o \
		slave_chain.o master_xchng.o if_vip_4r.o rsp_vip_4r.o rsp_esa.o

# objects for the slave image
sub_srsp =	$(RSP_COMMON) \
		srsp_ipc_cbus.o \
		srsp_stubs.o \
		slave_xchng.o \
		srsp_flashmib.o


sub_channel = 	if_cip.o rsp_cip.o \
		if_cip_sysmgt_rpc.o msg_sysmgt.o \
		if_cip_tcpip.o if_cip_sna.o if_cip_vlan.o \
        	channel_chain.o channel_debug.o channel.o 

sub_tn3270s = 	if_cip_tn3270s.o if_tn3270s.o tnlogscat.o tn3270s_alert.o 


sub_dbus_io =	dbus_io.o
sub_envmib =	sr_envmib.o sr_envmib_rsp.o sr_envmib_7000_rsp.o \
		sr_envmiboid.o  envmon_mib_chain.o envmib_registry.o

sub_lex_platform = lex_sub_hes.o
sub_pgen_rsp =	pgen_rsp.o
sub_rsp_pri =	ccpri.o ccpmsg.o ccpcc.o cc_serv.o
sub_ipc_cbus_rsp =	ipc_slave_apps.o
sub_ipc_cbus_mrsp =	mrsp_ipc_cbus.o

sub_dbus_io.o		: $(sub_dbus_io)
sub_channel.o		: $(sub_channel)
sub_envmib.o		: $(sub_envmib)
sub_ipc_cbus_rsp.o	: $(sub_ipc_cbus_rsp)
sub_ipc_cbus_mrsp.o     : $(sub_ipc_cbus_mrsp)
sub_lex_platform.o	: $(sub_lex_platform)
sub_pgen_rsp.o		: $(sub_pgen_rsp)
sub_rsp.o		: $(sub_rsp)
sub_srsp.o		: $(sub_srsp)
sub_tn3270s.o           : $(sub_tn3270s)
sub_rsp_pri.o		: $(sub_rsp_pri)

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

production_images = rsp-p-mz rsp-pv-mz rsp-dw-mz rsp-boot-mz \
		    rsp-dsv-mz rsp-adsv-mz rsp-isv-mz rsp-jsv-mz rsp-ajsv-mz

production: $(production_images)

production_test_images = $(addsuffix .test,$(production_images))

production.test: $(production_test_images)

#
# Crypto production available only to the crypto gang and release meisters
#
crypto_images = rsp-ajsv40-mz rsp-ajsv56-mz rsp-dsv40-mz rsp-dsv56-mz \
	        rsp-isv40-mz rsp-isv56-mz rsp-jsv40-mz rsp-jsv56-mz 

crypto: $(crypto_images)

crypto_test_images = $(addsuffix .test,$(crypto_images))
crypto.test: $(crypto_test_images)

#
# The "unofficial" engineering specials
#
oddball_notyet = rsp-tic-m rsp-tiv-m rsp-tli-m rsp-s-m
oddball_images = rsp-monlib-m rsp-i-m rsp-ir-m rsp-in-m rsp-inr-m rsp-d-m \
		 rsp-dr-m rsp-tatalk-m rsp-tddr-m rsp-tjvtp-m \
                 rsp-tjpgen-m rsp-tjtcl-m rsp-tjmanu-m rsp-tjrem-m \
		 rsp-k-mz rsp-ak-mz rsp-kv-mz rsp-akv-mz \
                 rsp-crypto-m rsp-crypto-mz rsp-jtrlane-m rsp-jtrlane-mz

oddballs: $(oddball_images)
	rm -f $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)

#
# Build all images 
#
everything: production modular oddballs

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

include $(SYSROOT)/obj-4k-rsp/make.gaia

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Generic Link rules.
#
rsp-%-m.sun rsp-%-m.czsun: lock $(SYS_START_FILE) \
			main-gs-%-m gs-%.lego 
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*) 
	rm -f test.lock
	mv gs-$*-m.temp $@

rsp-boot-m.sun rsp-boot-m.czsun: lock $(SYS_START_FILE)  \
			main-gs-boot-m gs-boot.lego $(UCODE_BOOT)	 
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-boot-m.temp \
	$(SYS_START_FILE) \
	Main_main-gs-boot-m.o cisco-gs-boot-m.o $(image-gs-boot) $(UCODE_BOOT)
	rm -f test.lock
	mv gs-boot-m.temp $@

#
# rules for the slave
#

%.ucode: %.bin $(IOS2UCODE_ELF_PROG)
	$(QUIET_COM)echo Generating ucode bundle $@...
	$(QUIET_CC)$(IOS2UCODE_ELF_PROG) -i $< -o $@ -v $(UCODE_VERSION) \
		-t $(RSP2_TYPE) -m $(RSP_HW_VERSION) \
		-h $(RSP_HARDWARE) -C unix
	rm -rf $@.bundle*
	$(COPY) $@ $(TFTPHOST)$(COLON)$(TFTPDIR)/$@$(BIN_SUFFIX)
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST)$(COLON)$(TFTPDIR)/$@$(BIN_SUFFIX)"
	cat $(SYSROOT)/version
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

rsp-dw-m.sun rsp-dw-m.czsun: $(SYS_START_FILE) main-gs-dw-m gs-dw.lego
	../lock rsp-dw.lock
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-dw-m.temp \
	$(SYS_START_FILE) \
	Main_main-gs-dw-m.o cisco-gs-dw-m.o $(image-gs-dw)
	rm -f rsp-dw.lock
	mv gs-dw-m.temp $@

.PRECIOUS: rsp-%-m.sun rsp-%-m.czsun

#
# Image rules
#
rsp-%-m: rsp-%-m.tftp
	@

############################################################################
#
# 			Libraries and the Like
#
############################################################################
#

#
# create microcode bundle from files listed in ucode.files
#

UCDEPEND=$(shell ../scripts/mkbndl2 -depend $(UCODE_FLAGS) $(RSP_UCODE_FILES))

$(UCODE): $(RSP_UCODE_FILES) $(UCDEPEND)
	../scripts/mkbndl2 $(UCODE_FLAGS) $(RSP_UCODE_FILES) $(UCODE_DATA) \
	                   $(UCODE_TOC_SRC) \
	    || (rm -f $(UCODE_DATA) $(UCODE_TOC_SRC); exit 1)
	$(QUIET_CC)$(CC) -c $(CFLAGS) $(WARN_ALL) \
	    $(patsubst %,-g,$(filter $(<D), $(GDB_REQUIRED))) \
	    $(UCODE_TOC_SRC) -o $(UCODE_TOC) || (rm -f $(UCODE_TOC); exit 1)
	$(QUIET_CC)$(LD) $(LOPTIONS) -r -o $(UCODE) $(UCODE_TOC) \
	    $(UCODE_DATA) || (rm -f $(UCODE); exit 1)

#
# Vip microcode bundle
#    Insure fresh vip ucode is available - it's not in CVS
#    Developers who want to save time can assert NO_VIP_UPDATE=1 but
#    must be sure they have both vip and vip2 images in ../../micro
#

ifndef NO_VIP_UPDATE
VIP_DEPEND = vip_ucode_once
ifndef VIP_UCODE_NEEDED_TOUCHED
DUMMY := $(shell touch vip_ucode_needed)
export VIP_UCODE_NEEDED_TOUCHED=1
endif
endif

#
# Need dependencies for VIP microcode to handle case where they don't
# exist yet.  They're actually built by the vip_ucode_once rule.
#

$(SVIP_IMAGE):
$(RVIP_IMAGE):
  
vip_ucode_once: vip_ucode_needed
	$(MAKE) $(MAKEFLAG-J) -C ../obj-4k-rvip $(RVIP_IMAGE)
	$(MAKE) $(MAKEFLAG-J) -C ../obj-4k-svip $(SVIP_IMAGE)
	touch vip_ucode_once

VUCDEPEND=$(shell ../scripts/mkbndl2 $(VUCODE_FLAGS) -depend \
	              $(RSP_UCODE_FILES))

$(UCODEV): $(RSP_UCODE_FILES) $(VUCDEPEND) $(VIP_DEPEND)
	../scripts/mkbndl2 $(VUCODE_FLAGS) $(RSP_UCODE_FILES) \
			   $(UCODEV_DATA) $(UCODEV_TOC_SRC) \
	    || (rm -f $(UCODEV_DATA) $(UCODEV_TOC_SRC); exit 1)
	$(QUIET_CC)$(CC) -c $(CFLAGS) $(WARN_ALL) \
	    $(patsubst %,-g,$(filter $(<D), $(GDB_REQUIRED))) \
	    $(UCODEV_TOC_SRC) -o $(UCODEV_TOC) \
	    || (rm -f $(UCODEV_TOC); exit 1)
	$(QUIET_CC)$(LD) $(LOPTIONS) -r -o $(UCODEV) $(UCODEV_TOC) \
	    $(UCODEV_DATA) || (rm -f $(UCODEV); exit 1)

#
# Crypto vip microcode bundle
#    Insure fresh vip ucode is available - it's not in CVS
#    Developers who want to save time can assert NO_VIP_UPDATE=1 but
#    must be sure they have both vip and vip2 images in ../../micro
#

ifndef NO_VIP_UPDATE
CRYPTO_VIP_DEPEND = crypto_vip_ucode_once
ifndef CRYPTO_VIP_UCODE_NEEDED_TOUCHED
DUMMY := $(shell touch crypto_vip_ucode_needed)
export CRYPTO_VIP_UCODE_NEEDED_TOUCHED=1
endif
endif

#
# Need dependencies for VIP microcode to handle case where they don't
# exist yet.  They're actually built by the vip_ucode_once rule.
#

$(CRYPTO_SVIP_IMAGE):
  
crypto_vip_ucode_once: crypto_vip_ucode_needed vip_ucode_once
	$(MAKE) $(MAKEFLAG-J) -C ../obj-4k-svip $(CRYPTO_SVIP_IMAGE)
	touch crypto_vip_ucode_once

CVUCDEPEND=$(shell ../scripts/mkbndl2 $(CVUCODE_FLAGS) -depend \
	              $(RSP_UCODE_FILES))

$(UCODECV): $(RSP_UCODE_FILES) $(CVUCDEPEND) $(CRYPTO_VIP_DEPEND)
	../scripts/mkbndl2 $(CVUCODE_FLAGS) $(RSP_UCODE_FILES) \
			   $(UCODECV_DATA) $(UCODECV_TOC_SRC) \
	    || (rm -f $(UCODECV_DATA) $(UCODECV_TOC_SRC); exit 1)
	$(QUIET_CC)$(CC) -c $(CFLAGS) $(WARN_ALL) \
	    $(patsubst %,-g,$(filter $(<D), $(GDB_REQUIRED))) \
	    $(UCODECV_TOC_SRC) -o $(UCODECV_TOC) \
	    || (rm -f $(UCODECV_TOC); exit 1)
	$(QUIET_CC)$(LD) $(LOPTIONS) -r -o $(UCODECV) $(UCODECV_TOC) \
	    $(UCODECV_DATA) || (rm -f $(UCODECV); exit 1)

#
# boot image ucode bundle shouldn't include all microcode (e.g. CIP)
#
BOOT_UCDEPEND=$(shell ../scripts/mkbndl2 $(BOOT_UCODE_FLAGS) -depend \
                  $(RSP_UCODE_FILES))

$(UCODE_BOOT): $(RSP_UCODE_FILES) $(BOOT_UCDEPEND) $(VIP_DEPEND)
	../scripts/mkbndl2 $(BOOT_UCODE_FLAGS) $(RSP_UCODE_FILES) \
			   $(UCODE_BOOT_DATA) $(UCODE_BOOT_TOC_SRC) \
	    || (rm -f $(UCODE_BOOT_DATA) $(UCODE_BOOT_TOC_SRC); exit 1)
	$(QUIET_CC)$(CC) -c $(CFLAGS) $(WARN_ALL) \
	    $(patsubst %,-g,$(filter $(<D), $(GDB_REQUIRED))) \
	    $(UCODE_BOOT_TOC_SRC) -o $(UCODE_BOOT_TOC) \
	    || (rm -f $(UCODE_BOOT_TOC); exit 1)
	$(QUIET_CC)$(LD) $(LOPTIONS) -r -o $(UCODE_BOOT) $(UCODE_BOOT_TOC) \
	    $(UCODE_BOOT_DATA) \
	    || (rm -f $(UCODE_BOOT); exit 1)

#
# create monlib file from flash file system .c source file
#
RSP_MONLIB_SRCFILES = ../filesys/scrt0_mips_fslib.S \
	../filesys/rommon_fslib_iface.c \
	../filesys/rommon_fslib_dev.c \
	../filesys/fslib.c \
	../filesys/fslib_dev_rom.c \
	../filesys/fslib_print.c \
	../filesys/rsp_ralib.c \
	../filesys/clpd6720.c

monlib_depend:
	$(MAKE) -f makefile.monlib depend

rsp-monlib-m: $(RSP_MONLIB_SRCFILES) makefile.monlib
	$(MAKE) -f makefile.monlib

../filesys/rsp_monlib_bndl.c: rsp-monlib-m ../scripts/make_rsp_monlib_bndl
	../scripts/make_rsp_monlib_bndl rsp-monlib-m $@

rsp_monlib_bndl.o: ../filesys/rsp_monlib_bndl.c

include ../makedist

#
#
include dependancies.specific
include $(SYSROOT)/dependancies
