# $Id: makefile,v 3.19.4.31 1996/09/05 22:35:59 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-68-c7000/makefile,v $
#------------------------------------------------------------------
# Makefile for system software for the c7000
#
# Copyright (c) 1985-1997 by cisco Systems, Inc.
# All rights reserved.
#
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.19.4.31  1996/09/05  22:35:59  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.19.4.30  1996/09/03  15:53:23  richl
# CSCdi67850:  rename images
#
# Revision 3.19.4.29  1996/08/28  13:01:56  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.19.4.28  1996/08/19  21:26:01  bew
# CSCdi66466:  take crypto targets out of production in
# obj-68-c7000/makefile
# Branch: California_branch
# Move crypto images from production to oddballs. 7000 is not yet a
# supported box.
#
# Revision 3.19.4.27  1996/08/06  18:19:45  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.19.4.26  1996/07/15  06:08:57  bchan
# CSCdi50608:  makefile for mib incorrect
# Branch: California_branch
#
# Revision 3.19.4.25  1996/07/15  02:14:49  bchan
# CSCdi50608:  makefile for mib incorrect
# Branch: California_branch
#
# Revision 3.19.4.24  1996/07/14  07:13:29  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.19.4.23  1996/07/09  20:56:12  ralls
# CSCdi62324:  Remove PLATFORM_SUPPORT_V from 7000 and RSP
# Branch: California_branch
#
# Revision 3.19.4.22  1996/07/02  01:12:36  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-68-*)
# Branch: California_branch
#
# Revision 3.19.4.21  1996/06/26  01:04:12  sdatta
# CSCdi61317:  Update makefile
# Branch: California_branch
#
# Revision 3.19.4.20  1996/06/24  21:25:59  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.19.4.19  1996/06/12  07:41:09  cheilek
# CSCdi58294:  vip build modifies files under CVS control
# Branch: California_branch
# Automatically build VIP and VIP2 microcode as needed for *-*v-* images.
#
# Revision 3.19.4.18  1996/06/11  23:08:30  ralls
# CSCdi60023:  Gaia update
# Branch: California_branch
#
# Revision 3.19.4.17  1996/06/10  20:03:47  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.19.4.16  1996/06/04  23:29:03  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.19.4.15  1996/05/17  11:33:45  ppearce
# Merge IbuMod into Calif
#
# Revision 3.15.2.6  1996/05/13  07:03:34  bchan
# CSCdi50608:  makefile for mib incorrect
# Branch: IbuMod_Calif_branch
#
# Revision 3.15.2.5  1996/05/06  00:06:39  ppearce
# Sync to IbuMod_Calif_baseline_960504
#
# Revision 3.15.2.4  1996/04/03  20:11:38  ppearce
# Sync to IbuMod_Calif_baseline_960402
#
# Revision 3.15.2.3  1996/03/28  23:22:34  bchan
# CSCdi52724:  merge tn3280 server code
# Branch: IbuMod_Calif_branch
# move tn3270_server_alert.c from sub_snanm to sub_channel
#
# Revision 3.15.2.2  1996/03/28  11:29:58  bchan
# CSCdi52724:  merge tn3280 server code
# Branch: IbuMod_Calif_branch
#
# Revision 3.15.2.1  1996/03/17  18:12:27  ppearce
# Sync IbuMod_Calif_branch to V111_1_3
#
# Revision 3.19.4.14  1996/05/10  01:35:41  wmay
# CPP and ppp half-bridge commit
# Branch: California_branch
#
# Revision 3.19.4.13  1996/05/09  14:34:52  rbadri
# Branch: California_branch
# LANE, UNI3.1 features
#
# Revision 3.19.4.12  1996/05/08  12:02:03  mdb
# Implement a method of naming locally built tools based on the type of
# the Machine, the name and the revision of the Operating System.
# Also make provide for all locally built tools be to removed when a
# 'make superclean' is performed in the sys directory. [CSCdi57043]
# Branch: California_branch
#
# Revision 3.19.4.11  1996/05/08  03:58:43  ralls
# CSCdi57036:  Ole man river he just keeps rollin along
# Branch: California_branch
#
# Revision 3.19.4.10  1996/05/02  08:44:20  dwong
# Branch: California_branch
# Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
#
# Revision 3.19.4.9  1996/04/30  20:11:54  ralls
# CSCdi56220:  Update Gaia features
# Branch: California_branch
#
# Revision 3.19.4.8.2.1  1996/04/27  07:16:39  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.19.4.8  1996/04/25  23:22:05  ronnie
# CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
# Feature commit for ATM and Frame Relay into 11.2
# Branch: California_branch
#
# Revision 3.19.4.7  1996/04/23  19:28:44  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.19.4.6  1996/04/15  21:20:30  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.19.4.5  1996/04/12  23:11:29  mdb
# CSCdi54154:  Need to remove old files
# Branch: California_branch
# Remove last traces of fslib_string.c
#
# Revision 3.19.4.4  1996/04/12  17:00:01  mdb
# CSCdi54154:  Need to remove old files
# Branch: California_branch
# Remove references to gutted files in the makefiles and nuke the gutted
# files.
#
# Revision 3.19.4.3.4.1  1996/04/16  18:52:26  cyoung
# Take VTP out of the production images and make it an oddball
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.19.4.3  1996/03/29  00:44:43  hochan
# CSCdi52960:  Move -k- images to oddballs
# Branch: California_branch
#
# Revision 3.19.4.2  1996/03/21  22:59:06  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.19.4.1  1996/03/18  21:15:54  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.12.8.4  1996/03/16  07:13:36  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.12.8.3  1996/03/09  05:08:03  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.12.8.2  1996/03/07  10:17:02  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.12.8.1  1996/02/20  10:43:13  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.19  1996/03/11  07:27:07  jwelder
#         Pull the VIP microcode out of normal c7000 images, add the
#         following images: gs7-kv-mz, gs7-akv-mz, gs7-jv-mz, gs7-ajv-mz,
#         gs7-pv-mz
#
# CSCdi48845:  must restrict VIP ucode to specific GS7 subsets
#
# Revision 3.18  1996/03/09  00:48:31  nitin
# CSCdi50416:  Move the non supported images to oddball
# Changed the default image for rsp & gs7 to be -j-. Moved the -k- images
# to oddballs.
#
# Revision 3.17  1996/03/07  21:50:59  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.16  1996/03/07  20:50:36  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.15  1996/02/26  21:51:14  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.14  1996/02/17  09:03:51  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.13  1996/01/25  11:21:01  smackie
# Registry code gets some much-needed hot-oil massaging, free liposuction
# and a makeover.  (CSCdi47101)
#
#    o Compress registry structures. Saves over 120K for all platforms
#    o Add registry subsystems and remove feature registry initializion
#      from registry.c to fix woeful initialization scaling problems
#    o Remove unused and seemingly unloved registry debugging code
#    o Add registry memory statistics to shame people into action
#
# Revision 3.12  1996/01/09  00:58:35  wmay
# CSCdi46498:  add -aj-mz version to production image
#
# Revision 3.11  1996/01/07  13:52:19  banderse
# CSCdi45351:  show microcode should display CIP flash default
# removed xxx_flash_ucode reference.
#
# Revision 3.10  1996/01/06  05:16:58  hampton
# Split timer specific routines into multiple files.  Move the processor
# specific routines into the src-xx-xxx directories.  [CSCdi46482]
#
# Revision 3.9  1995/12/22  23:09:25  yanke
# temp solution to fix ipc modularity problem on c7000.
#
# CSCdi46065:  ipc breaks modularity on c7000
#
# Revision 3.8  1995/12/11  23:19:53  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.7  1995/12/05  00:50:10  wmay
# CSCdi44967:  7000 and RSP need -j images for V.120 support
#
# Revision 3.6  1995/12/01  19:04:03  sdurham
# CSCdi44362:  envmon is non-modular w.r.t envmon mib.
#   Remove direct calls from envmon code to mib code. Use service points.
#
# Revision 3.5  1995/11/29  18:45:42  banderse
# CSCdi43438:  CIP needs default microcode flash image
# Added release specific default flash image; changes to ensure flash file
# system is initialized completely before IP download.
#
# Revision 3.4  1995/11/22  23:42:52  sdurham
# CSCdi35282:  Need command to configure envmon trap enables
#      add individual trap options in separate parser-mib file
#      envmon_mib_chain.
#
# Revision 3.3  1995/11/17  17:52:47  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:10:37  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 2.16  1995/10/18  21:47:21  hampton
# Need a space conserving way to test build all images.  Add targets for
# production.test, oddballs.test, modular.test, and everything.test.
# These delete each target after making it to limit disk space usage.
# [CSCdi42405]
#
# Revision 2.15  1995/10/18  14:40:53  dblair
# CSCdi41356:  Default images of make files are not compressed
#         Change default images to be compressed image
#
# Revision 2.14  1995/08/22  17:19:54  motto
# CSCdi37937:  CIP sysmgt RPCs timeout when all interfaces shutdown
#
# Revision 2.13  1995/08/22  16:58:08  jjohnson
# CSCdi39028:  minor makefile cleanup
#
# Revision 2.12  1995/08/14  14:51:54  richl
# CSCdi38712:  dont build modular_notyet in everything target
#
# Revision 2.11  1995/08/12  07:57:42  richl
# CSCdi38686:  more ipmulticast fastswitching rearrangement
#
# Revision 2.10  1995/08/09  00:21:19  kramling
# CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
# 10.3.
#
# Revision 2.9  1995/08/08  23:52:13  richl
# CSCdi38448:  show hardware yields srswitch info when it shouldnt
#
# Revision 2.8  1995/08/07  13:55:45  richl
# CSCdi38315:  makefile rework
#              .o accidentally deleted from sub_channel.  put it back.
#
# Revision 2.7  1995/08/07  05:40:39  richl
# CSCdi38315:  makefile rework
#
# Revision 2.6  1995/07/17  23:27:54  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.5  1995/07/12  15:52:48  csilva
# CSCdi36819:  sys/obj-68-c7000/makefile change
# sys/hes/c7000_ucode_bndl.c or srs_ucode_bndl.c will only be remade,
# depending upon the actual microcode listed in the c7000_ucode.files
# or the srs_ucode.files.
#
# Revision 2.4  1995/07/01  05:58:48  awu
# CSCdi36167:  ATM calls printf from interrupt level.
#
# Revision 2.3  1995/06/22  17:16:00  sberl
# CSCdi34459:  make production target should build official images
# Add appn sub/superset images to trunkd.test target.
#
# Revision 2.2  1995/06/10  01:56:19  smackie
# Add IPC to the SYSTEM_FOO declarations. Move LANE includes to prevent
# dependancy problems on ILMI. (CSCdi35646)
#
# Revision 2.1  1995/06/07  21:52:21  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c7000-js-mz

PLATFORM_TIMER   = timer_generic.o timer_i8254.o
PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.68
include ../makefile

IP7000 =		$(IPC_UDP) sub_dbus_io.o
FASTSWITCH_7000 =	sub_sse.o

#
# Platform specific definitions
#
PLATFORM		= RP1
PLATFORM_FLAGS		= -D$(PLATFORM) -DC7000
PLATFORM_IMAGE_GROUP	= 7000
PLATFORM_IMAGE_PREFIX	= C7000
HWFLAGS 		= gs

ROM_LDFLAGS = 	-e __start -N -nostdlib -Ttext 1010000 -Tdata 1000
RAM_LDFLAGS = 	-e __start -N -nostdlib -Ttext 1000

#
# Redefinition of common code to include C7000 extensions
#
IPSERVICES :=		$(IPSERVICES) $(IPC_UDP) sub_dbus_io.o
FASTSWITCH_CORE :=	$(FASTSWITCH_CORE) sub_sse.o

ENVMIB =	sub_envmib.o sub_old_envmib.o

TEST = 		sub_test.o
CHANNEL =	sub_channel.o
TN3270S =	sub_tn3270s.o
TN3270SMIB =    sub_tn3270smib.o
IPC_CBUS_RP =	sub_ipc_cbus_rp.o


PLATFORM_SUPPORT  = sub_c7000.o $(IPC) 


#
# Platform-specific plus option set features
#
PLATFORM_PLUS = $(VLANS) $(LANE) $(LANEMIB)

#
# VIP definitions
#
UCODE = c7000_ucode_bndl.o
UCODEV = c7000v_ucode_bndl.o
UCODE_DIR        = ../../micro
C7000V_UCODE_FILES  = $(UCODE_DIR)/c7000v_ucode.files

#
# Version number extraction from c7000v_ucode.files
#

EXTRACT_RVIP_VER_MAJ_CMD := $(shell perl -n -e 'print "$$1" if /micro\/vip(\d+)-(\d+)/' < $(C7000V_UCODE_FILES))
EXTRACT_RVIP_VER_MIN_CMD := $(shell perl -n -e 'print "$$2" if /micro\/vip(\d+)-(\d+)/' < $(C7000V_UCODE_FILES))

RVIP_IMAGE = $(UCODE_DIR)/vip$(EXTRACT_RVIP_VER_MAJ_CMD)-$(EXTRACT_RVIP_VER_MIN_CMD)

#
# Non-modular pieces required to make the "modular" images link
# see also, common_c7000's inclusion of CIP and IPC
#
PLATFORM_NONMODULAR = $(ATM) $(ATM_ARP) $(FDDI_SMT) $(FLASH)
NONMODULAR_PLATFORM_REQ = $(ATM) $(FDDI_SMT) $(FLASH) \
$(TRING) $(TUNNEL_FEATURE) $(ISDN_FEATURE) $(SRBCORE)

PLATFORM_MIBS = $(ENVMIB) $(FSIPMIB) $(DS1MIB) $(ATOMMIB_ES)
NONMOD = shr_tring.o
#need to stub out APPNMIB_SHARE so appn images will not pick the share image again
APPNMIB_SHARE=

#
# Define drivers supported by this platform
#
ISDN7000 =	sub_c7000_pri.o

DRIVERS =	$(SERIAL) $(ETHER) $(TRING) $(FLASH) $(LEX) \
		$(IPC_CBUS_RP) $(ISDN) $(ISDN7000) \
                $(CHANNEL)  $(TN3270S)             \
		$(FDDI_SMT) $(ATM) $(ATM_ARP) $(V120) $(CPP)

DRIVER_MIBS =	$(ETHERMIB) $(TRINGMIB) $(DS1MIB) $(FDDIMIB) \
                $(CHANNELMIB) $(TN3270SMIB) $(APPNMIB_TN3270S) \
	        $(FSIPMIB) $(ATOMMIB_ES) $(ATMMIB) $(CALLMIB) $(ISDNMIB)



# 
# Add in platform specific mibs
#
MIBS := $(MIBS) $(ENVMIB)

#
# This platform has additional comserver support, depending on the image.
#
NAS_P_OPTIONS= 

#
# Define platform fastswitching modules
#
FAST_MEDIA   = fr_fast_mci.o atm_dxi_fast_mci.o 
FAST_FR   = fr_fast_mci.o 
FAST_ATMDXI = atm_dxi_fast_mci.o 
FAST_ATALK   = at_fast_mci.o
FAST_ATSMRP  = at_smrpfast_mci.o at_smrpfast.o smrp_fast_mci.o
FAST_CLNS    = clnsfast_mci.o sub_sse_clns.o
FAST_IBM     = fastdlsw_cbus.o fastsrb_cbus.o sub_sse_bridge_sr.o
FAST_DECNET  = dnfast_mci.o
FAST_IP      = ipfast_mci.o ipfast_cbus.o ipasm.o ipfast_c7000.o \
	       sub_sse_ip.o
FAST_IPMCAST = ipmfast_c7000.o
FAST_IPX     = novfast_mci.o novfast_cbus.o sub_sse_ipx.o
FAST_TBRIDGE = bridge_cbus.o tbridge.o tbridge_hes.o \
               sub_sse_bridge_t.o
FAST_VINES   = vinesfast_mci.o
FAST_XNS     = xnsfast_mci.o
FAST_VLANS   = ieee_mci_vlan.o isl_mci_vlan.o

###########################################################################
#
# Subsystems specific to the c7000 router platform
#
###########################################################################

common_c7000 =	csc2_console.o asm_hes.o if_mci.o mci_chain.o \
		if_controller.o if_controller_chain.o hes_controller.o  \
                nv_rp1.o \
		platform_c7000.o c7000_snmp.o \
		c7000_fci.o if_ags_c7000_aip.o if_aip.o aip_parse_support.o \
		if_hes_ether.o atm_aip_chain.o \
		if_ags_c7000_cmd.o hes_fddi.o hes_xdi.o \
		diag.o diag_68.o diag_chain.o \
		cbus_chain.o fddi.o fddi_chain.o fddi_debug.o fddi_parser.o \
		c7000_rsp_fip.o calendar.o envm.o envmon_chain.o \
		ags_c7000_fddi.o \
		flash_fmgr.o flash_dvr.o flash_rp1.o if_hes_serial.o \
		if_hes_serial_parser.o hotswap.o oir_debug.o oir_chain.o \
		if_hes_common.o dbus.o dbus_chain.o  c7000_dbus.o \
		if_ags_c7000.o if_trip.o if_ags_c7000_trip.o \
		if_ags_c7000_serial.o \
		ucode.o g_dev.o cbus_debug.o \
		msg_cbus.o msg_mci.o msg_csc2.o msg_envm.o msg_dbus.o \
		msg_ucode.o msg_oir.o msg_fddi.o msg_aip.o \
		if_msg_controller.o \
		dsx1_chain.o dsx1.o if_mip.o c7000_mip.o \
                ucode_compress.o msg_flash.o \
		if_feip.o fast_ethernet_chain.o fast_ethernet_debug.o \
		if_ags_c7000_feip.o \
		if_vip.o if_rvip.o rvip_debug.o c7000_vip.o c7000_rvip.o \
		filesys.o fslib.o \
		remote_filesys_server.o \
		remote_tty_server.o remote_tty_client.o \
		fslib_print.o \
		fslib_dev_intel_series_2plus.o \
		filesys_test_cmd.o filesys_test_chain.o \
		filesys_debug.o flash_command.o flash_config.o \
		fs_rec.o nv_filesys.o \
		tftp_rcp_filesys.o flash_filesys.o \
		rp_fslib_dev.o rp_fslib_test.o \
		rp_ralib.o rp_flash_filesys.o rp_fc.o rp_flash.o \
		if_vip_4t.o \
		c7000_monlib_bndl.o \
		monvar.o if_vip_4r.o c7000_vip_4r.o \
		cbus_registry.o

sub_channel = 	if_cip.o if_ags_c7000_cip.o \
                if_cip_sysmgt_rpc.o msg_sysmgt.o \
                if_cip_tcpip.o if_cip_sna.o if_cip_vlan.o \
		channel_chain.o channel_debug.o channel.o 

sub_tn3270s = 	if_cip_tn3270s.o if_tn3270s.o tnlogscat.o tn3270s_alert.o 


sub_ipc_cbus_rp =  ipc_cbus_rp.o ipc_slave_apps.o

sub_c7000 =	$(common_c7000)

sub_c7000_pri = ccpri.o ccpmsg.o ccpcc.o cc_serv.o

sub_dbus_io =	dbus_io.o

sub_envmib =	sr_envmib.o sr_envmib_7000.o sr_envmiboid.o \
		sr_envmib_7000_rsp.o envmon_mib_chain.o envmib_registry.o

sub_lex_platform = lex_sub_hes.o lex_fs_mci.o
sub_c7000_common.o		: $(sub_c7000_common)

sub_old_envmib = sr_old_lenvmib_7000.o sr_old_lenvmib_7000oid.o

sub_sse =	sse_ssp.o sse_chain.o sse_debug.o sse.o msg_sse.o \
		sse_registry.o

sub_sse_clns = 	clnsfast_sse.o

sub_sse_ip =	ipfast_sse.o ipaccess_sse.o ip_sse_chain.o ip_sse.o 

sub_sse_bridge_t =	bridge_sse.o bridge_sse_chain.o

sub_sse_bridge_sr =	srb_sse.o srb_sse_chain.o

sub_sse_ipx =    novfast_sse.o novell_sse_chain.o

sub_test =	cbus_test.o trip_test.o hes_test.o \
		transmit_test.o cbus_test_chain.o sse_test.o \
		cip_test.o

sub_c7000.o			: $(sub_c7000)
sub_c7000v.o		        : $(sub_c7000v)
sub_c7000_pri.o			: $(sub_c7000_pri)
sub_channel.o			: $(sub_channel)
sub_dbus_io.o			: $(sub_dbus_io)
sub_envmib.o			: $(sub_envmib)
sub_ipc_cbus_rp.o		: $(sub_ipc_cbus_rp)
sub_lex_platform.o		: $(sub_lex_platform)
sub_old_envmib.o		: $(sub_old_envmib)
sub_sse.o			: $(sub_sse)
sub_sse_clns.o			: $(sub_sse_clns)
sub_sse_ip.o			: $(sub_sse_ip)
sub_sse_ipx.o			: $(sub_sse_ipx)
sub_sse_bridge_t.o		: $(sub_sse_bridge_t)
sub_sse_bridge_sr.o		: $(sub_sse_bridge_sr)
sub_test.o			: $(sub_test)
sub_tn3270s.o			: $(sub_tn3270s)

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

# The indirect assignments below are for the benefit of build analysis tools
#
# The "official" production set of images supplied to manufacturing
#

production_images = c7000-ajs-mz c7000-js-mz c7000-p-mz \
		    c7000-ajsv-mz c7000-jsv-mz c7000-pv-mz 

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
# The "unofficial" engineering specials.  Note that some of these are
# actually in production on other platforms, so they don't have "t" in 
# the image name. 
#
oddball_notyet = c7000-tli-m c7000-tic-m c7000-tiv-m 
oddball_images = c7000-tjvtp-m c7000-tjpgen-m c7000-tjmanu-m c7000-tjrem-m \
                 c7000-tjtcl-m c7000-tatalk-m c7000-tddr-m c7000-tip-m 

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

include $(SYSROOT)/obj-68-c7000/make.gaia

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
c7000-%-m.czsun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c7000-%-m.sun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c7000-%-r.sun: lock main-gs-%-r gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(ROM_LDFLAGS) -o gs-$*-r.temp \
	Main_main-gs-$*-r.o cisco-gs-$*-r.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-r.temp $@

.PRECIOUS: c7000-%-m.sun c7000-%-r.sun c7000-%.czsun

#
# Image rules
#
c7000-%-r: c7000-%-r.rom
	@
c7000-%-m: c7000-%-m.tftp
	@

############################################################################
#
# 			Libraries and the Like
#
############################################################################
#
#
# create microcode bundle from files listed in c7000_ucode.files
#

C7000 MICROCODE BUNDLE DEPENDANCIES=$(shell expand ../../micro/c7000_ucode.files | sed -e 's/ *\#.*//' -e 's/^ *\([^ ][^ ]*\).*/\1/')
C7000V MICROCODE BUNDLE DEPENDANCIES=$(shell expand ../../micro/c7000v_ucode.files | sed -e 's/ *\#.*//' -e 's/^ *\([^ ][^ ]*\).*/\1/')

../hes/c7000_ucode_bndl.c: $(C7000 MICROCODE BUNDLE DEPENDANCIES) ../../micro/c7000_ucode.files
	mkbndl -f ../hes/c7000_ucode_bndl.c ../../micro/c7000_ucode.files

c7000_ucode_bndl.o: ../hes/c7000_ucode_bndl.c

#
# Vip microcode bundle
#    Insure fresh vip ucode is available - it's not in CVS
#    Developers who want to save time can assert NO_VIP_UPDATE=1 but
#    must be sure they have a vip image in ../../micro
#

ifndef NO_VIP_UPDATE
VIP_DEPEND = vip_ucode_once
ifndef VIP_UCODE_NEEDED_TOUCHED
DUMMY := $(shell touch vip_ucode_needed)
export VIP_UCODE_NEEDED_TOUCHED=1
endif
endif

#
# Need dependency for VIP microcode to handle case where it doesn't
# exist yet.  It's actually built by the vip_ucode_once rule.
#

$(RVIP_IMAGE):

vip_ucode_once: vip_ucode_needed
	$(MAKE) $(MAKEFLAG-J) -C ../obj-4k-rvip $(RVIP_IMAGE)
	touch vip_ucode_once

../hes/c7000v_ucode_bndl.c: $(C7000V MICROCODE BUNDLE DEPENDANCIES) ../../micro/c7000v_ucode.files $(VIP_DEPEND)
	mkbndl -f ../hes/c7000v_ucode_bndl.c ../../micro/c7000v_ucode.files
 
c7000v_ucode_bndl.o: ../hes/c7000v_ucode_bndl.c
 
#
# create MONLIB source file from flash file system source files
#
C7000_FSLIB_FILES = ../filesys/c7000_monlib_main.c \
		../filesys/rommon_fslib_dev.c \
		../filesys/rommon_fslib_iface.c \
		../filesys/fslib.c \
		../filesys/fslib_dev_rom.c \
		../filesys/fslib_print.c \
		../filesys/rp_ralib.c

gs7-fslib-m: $(C7000_FSLIB_FILES) makefile.monlib
	$(MAKE) -f makefile.monlib depend
	$(MAKE) -f makefile.monlib $@

gs7-monlib-m: gs7-fslib-m $(MKFFSI_PROG)
	$(MKFFSI_PROG) gs7-fslib-m $@

../filesys/c7000_monlib_bndl.S: gs7-monlib-m ../scripts/make_7000_monlib_bndl
	../scripts/make_7000_monlib_bndl gs7-monlib-m $@

c7000_monlib_bndl.o: ../filesys/c7000_monlib_bndl.S

include ../makedist
#
#
include dependancies.specific
include $(SYSROOT)/dependancies
