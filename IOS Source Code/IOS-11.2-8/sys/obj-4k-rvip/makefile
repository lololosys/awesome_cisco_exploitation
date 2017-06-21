# $Id: makefile,v 3.13.4.18 1996/09/05 22:40:23 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-4k-rvip/makefile,v $
#------------------------------------------------------------------
# makefile - Checkers makefile
#
# January 1995, Dave Getchell and Michael Beesley
#
# Copyright (c) 1995-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.13.4.18  1996/09/05  22:40:23  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.13.4.17  1996/09/03  15:52:35  richl
# CSCdi67850:  rename images
#
# Revision 3.13.4.16  1996/08/29  17:40:58  drowell
# CSCdi46093:  bundling the ucode is way to slow
# Branch: California_branch
# Introduced new mkbndl2 script which bundles ucode by directly generating
# an ELF object file. Also allows selection of ucode by flags in the ucode
# list file.
#
# Revision 3.13.4.15  1996/08/16  08:02:17  dlobete
# CSCdi59971:  DACL needs modularity work
# Branch: California_branch
# For the sake of future maintenance of IP access-lists, share as much
# source code as possible between VIP and RSP.
#
# Revision 3.13.4.14  1996/07/14  07:13:03  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.13.4.13  1996/07/02  01:07:39  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-4k-*)
# Branch: California_branch
#
# Revision 3.13.4.12  1996/06/27  18:11:35  avaz
# CSCdi41715:  vip1 flash rom and vpld versions not right
# Branch: California_branch
#
# Revision 3.13.4.11  1996/06/24  21:25:40  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.13.4.10  1996/06/12  07:40:55  cheilek
# CSCdi58294:  vip build modifies files under CVS control
# Branch: California_branch
# Automatically build VIP and VIP2 microcode as needed for *-*v-* images.
#
# Revision 3.13.4.9  1996/06/04  23:34:00  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.13.4.8  1996/05/08  23:03:52  mdb
# Fix minor breakage from CSCdi57043 commit. This should force the
# needed tools to be properly built before they are referenced.
# Branch: California_branch
#
# Revision 3.13.4.7  1996/05/08  02:53:24  cheilek
# CSCdi55787:  Microcode directory not updated when vip image built
# Branch: California_branch
# Unless the NO_VIP_BUNDLE flag is set, copy the vip images to the
# ../../micro directory and update the (c7000|rsp)v_ucode.files file(s)
#
# Revision 3.13.4.6  1996/05/08  01:09:55  gchristy
# CSCdi56944:  Compilation errors from IP named access list commit
# Branch: California_branch
# - Modularize new ACL code for inclusion in VIP images.
#
# Revision 3.13.4.5  1996/04/23  19:33:00  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.13.4.4  1996/04/19  15:22:57  che
# CSCdi55138:  IosSec_branch feature commit
# Branch: California_branch
# IOS Security feature commit
#
# Revision 2.1.6.18  1996/04/17  21:28:04  xliu
# Branch: IosSec_branch
# Define option to make crypto image.
#
# Revision 2.1.6.17  1996/04/09  21:23:27  che
# Branch: IosSec_branch
# Sync to California_branch (960405)
#
# Revision 2.1.6.16  1996/04/02  10:05:50  carrel
# cvs fixes
#
# Revision 2.1.6.15  1996/03/30  05:53:13  xliu
# Branch: IosSec_branch
# Commit before CA split.
#
# Revision 2.1.6.14  1996/03/25  23:24:19  xliu
# Branch: IosSec_branch
# Add support of EPA.
#
# Revision 2.1.6.13  1996/03/19  08:23:19  che
# Branch: IosSec_branch
#
# Revision 2.1.6.12  1996/02/21  03:46:35  che
# Branch: IosSec_branch
# Sync to 11.1 Mainline (02/20/96)
#
# Revision 2.1.6.11  1996/01/31  04:49:50  xliu
# Branch: IosSec_branch
# Add crypto engine ipc module.
#
# Revision 2.1.6.10  1996/01/23  06:11:00  che
# Branch: IosSec_branch
# Sync with 11.1 mainline. (960122)
#
# Revision 2.1.6.9  1996/01/09  18:11:19  xliu
# Branch: IosSec_branch
# Add crypto_engine_vip_chain.o.
#
# Revision 2.1.6.8  1996/01/08  19:28:15  ptran
# Fixed sync problem when trying to build rvip image.
# Branch: IosSec_branch
#
# Revision 2.1.6.7  1995/12/17  02:00:41  che
# Branch: IosSec_branch
# Sync to Arkansas
#
# Revision 2.1.6.6  1995/12/10  09:43:54  xliu
# Branch: IosSec_branch
# Add crypto engine vip ipc code to CRYPTO_ENGINE_VIP. Add a pointer
# field (crypto_vectors) to interface_private.h.
#
# Revision 2.1.6.5  1995/12/06  21:55:09  rmontino
# Branch: IosSec_branch
#
# Revision 2.1.6.4  1995/12/02  17:55:27  xliu
# Branch: IosSec_branch
# Define sub_crypto_engine to link crypto_core compiled in common dir.
#
# Revision 2.1.6.3  1995/11/30  09:52:48  xliu
# Branch: IosSec_branch
#
# Modified name.
#
# Revision 2.1.6.2  1995/11/30  06:03:15  xliu
# Branch: IosSec_branch
#
# Add vip specific crypto engine code.
#
# Revision 2.1.6.1  1995/11/17  17:19:02  carrel
# Branch: IosSec_branch
# Sync to Arkansas
#
# Revision 3.13.4.3  1996/04/15  21:20:12  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.13.4.2  1996/03/21  22:58:24  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.13.4.1  1996/03/18  21:14:49  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.3.2.9  1996/03/18  23:16:57  mbeesley
# Branch: ELC_branch
# Sync to V111_1_3
#
# Revision 3.3.2.8  1996/02/21  21:47:09  mbeesley
# CSCdi49481:  VIPs have no need of pas_controller.c. Link it out.
# Branch: ELC_branch
#
# Revision 3.3.2.7  1996/02/17  05:54:16  fsunaval
# CSCdi49008:  Fddi output counters are not reported and other fixes.
# Branch: ELC_branch
# 1.Output counts not increasing.
# 2.Make FDDI on VIP memory-mapped.
# 3.Distributed switching support.
# 4.Reduce burst-size from 64 to 32.
#
# Revision 3.3.2.6  1996/02/10  00:06:40  mbeesley
# CSCdi48581:  Add VIP2 support to ELC_branch
# Branch: ELC_branch
#
# Revision 3.3.2.5  1996/02/08  08:24:17  gstovall
# Branch: ELC_branch
# Sync to ELC_baseline_960205.
#
# Revision 3.3.2.4  1996/01/31  01:36:59  sthormod
# Branch: ELC_branch
# Add Predator 4R support and fix damage from last sync.
#
# Revision 3.3.2.3  1996/01/16  00:34:04  dcarroll
# Branch: ELC_branch
# Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
#
# Revision 3.3.2.2  1995/12/23  04:40:50  fsunaval
# Branch: ELC_branch
# 1.First pass at FDDI port adapter.
#
# Revision 3.3.2.1  1995/12/11  19:03:09  mbeesley
# Branch: ELC_branch
# Sync ELC_branch to ELC_baseline_120995.
#
# Revision 3.13  1996/03/07  21:50:37  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.12  1996/03/07  20:50:12  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.11  1996/03/04  07:13:34  kao
# CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
# VIP; VPLD version number should be 21.0 not 20.1
#
# Revision 3.10  1996/02/26  21:50:44  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.9  1996/02/17  09:01:55  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.8  1996/02/05  21:14:25  jwelder
# CSCdi47929:  Makefile for RVIP doesnt make production images properly
#
# Revision 3.7  1996/01/06  05:16:43  hampton
# Split timer specific routines into multiple files.  Move the processor
# specific routines into the src-xx-xxx directories.  [CSCdi46482]
#
# Revision 3.6  1996/01/06  03:20:51  gwynne
# CSCdi46437:  obj-4k-rsp and obj-4k-rvip makefiles need to use $(COLON)
#
# Revision 3.5  1995/12/06  21:52:59  foster
# CSCdi44432:  rsp slave image dependenices are broken
#
# Revision 3.4  1995/11/28  03:20:53  getchell
# CSCdi43583:  rvip needs updated vpld image (21)
#
# Revision 3.3  1995/11/17  17:52:14  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:09:27  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
# Revision 1.1.2.50  1995/11/08  01:12:29  getchell
# CSCdi43583:  rvip needs updated vpld image (21)
# Branch: VIP_branch
#
# Revision 1.1.2.49  1995/10/18  14:12:53  dlobete
# CSCdi42344:  RP/RSP support for 4T port-adaptor
# Branch: VIP_branch
#
# Revision 2.1  1995/06/07  21:51:18  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: rvip-dw-m tftpcopy

production_basenames = rvip-dw-m
modular_basenames    =
oddballs_basename    =
gaia_basenames =  rvip-dw-m

PLATFORM_TIMER   = timer_generic.o timer_nevada.o asm_4k_timer.o 
PLATFORM_OTHER	 = rvip_vspace.o rvip_stubs.o asm_4k_rvip.o \
		   vip_ukernel.o rvip_ukernel.o
PLATFORM_MONITOR = 

include ../makefile.tools
include ../makefile.4k
include ../makefile

#--------------------------------------------------------
# Hardware Specific Definitions:
#--------------------------------------------------------
RVIP_TYPE	= 20
RVIP_HW_VERSION	= 2.0
RVIP_HARDWARE	= CHECKERS_HW_REV2
VPLD_VERSION	= 20.0
CODE_START	= 0x64208000

#-------------------------------
# Microcode destination
#-------------------------------
UCODE_DIR        = ../../micro
RSP_UCODE_FILES  = $(UCODE_DIR)/rsp_ucode.files
GS7_UCODE_FILES  = $(UCODE_DIR)/c7000v_ucode.files

#--------------------------------------------------------
# Version number extraction from rspv_ucode.files
#--------------------------------------------------------

EXTRACT_RVIP_VER_MAJ_CMD := $(shell perl -n -e 'print "$$1" if /micro\/vip(\d+)-(\d+)/' < $(RSP_UCODE_FILES))
EXTRACT_RVIP_VER_MIN_CMD := $(shell perl -n -e 'print "$$2" if /micro\/vip(\d+)-(\d+)/' < $(RSP_UCODE_FILES))

#--------------------------------------------------------
# Version numbers and strings
#--------------------------------------------------------
RVIP_SW_VER_MAJ	= $(EXTRACT_RVIP_VER_MAJ_CMD)
RVIP_SW_VER_MIN	= $(EXTRACT_RVIP_VER_MIN_CMD)
RVIP_SW_VER	= $(RVIP_SW_VER_MAJ)-$(RVIP_SW_VER_MIN)
RVIP_SW_NUM	= $(RVIP_SW_VER_MAJ).$(RVIP_SW_VER_MIN)
RVIP_IMAGE_STR	= vip$(RVIP_SW_VER)

RVIP_VPLD_VER_MAJ = 4
RVIP_VPLD_VER_MIN = 24
VPLD_VERSION	= $(RVIP_VPLD_VER_MAJ).$(RVIP_VPLD_VER_MIN)

RVIP_ROM_VER_MAJ = 160
RVIP_ROM_VER_MIN = 5
ROM_VERSION =  $(RVIP_ROM_VER_MAJ).$(RVIP_ROM_VER_MIN)


#-------------------------------
# Platform specific definitions
#-------------------------------
PLATFORM		= RVIP
PLATFORM_FLAGS		= -D$(PLATFORM) -D$(RVIP_HARDWARE)
PLATFORM_IMAGE_GROUP	= VIP
PLATFORM_IMAGE_PREFIX	= RVIP
# 
# FIXME: Is this correct ?
#
HWFLAGS			= les

ROM_LDFLAGS= -e __start -N $(GNUM) -Ttext $(CODE_START) -Trvip.link -nostdlib
RAM_LDFLAGS= -e __start -N $(GNUM) -Ttext $(CODE_START) -Trvip.link -nostdlib

PLATFORM_SUPPORT_DSW = sub_rvip.o sub_fastswitch_pas.o 

#--------------------------------------------------------------------
# Sched alloc is used on this platform - hence override this variable
# so that support for sched-interval is not pulled in
#--------------------------------------------------------------------
SCHED_INTERVAL =

#---------------------------------------------------- 
# crypto support
#----------------------------------------------------
CRYPTO_VIP = $(CRYPTO_CORE) $(CRYPTO_ENGINE_VIP) $(EPAD)

#----------------------------------------------------
# Platform specific drivers, and peripheral support.
#----------------------------------------------------

DRIVERS_DSW =	$(AMDP2) $(DEC21140) $(DEC21150) $(IBM2692) $(S4T68360) \
                $(MIF68840) 

FAST_DRIVER =	
FASTSWITCH = 	
AMDP2 =		sub_amdp2.o   
DEC21140 =	sub_dec21140.o
DEC21150 =	sub_dec21150.o
S4T68360 =	sub_s4t68360.o
TEST =		sub_test.o
IBM2692 =	sub_ibm2692.o
EPAD = 		sub_epad.o
MIF68840 =  sub_mif68840.o
FAST_IPBRIDGING = 
 
#-------------------------------------------------------
# Rules to build assembly language portions of the RVIP
#-------------------------------------------------------

RVIP_ASM = 	asm_4k.o asm_4k_timer.o asm_4k_rvip.o asm_4k_vip_cbus.o \
		asm_4k_vip_dec21140.o asm_4k_vip_amdp2.o asm_4k_vip_af.o \
		ipasm_4k.o

$(RVIP_ASM):
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)$(AS) -c $(ASWITCHES) $(WARN_ALL) $< \
	|| (rm -f $@ ; exit 1)


#-------------------------------------------------------
# RVIP subsystem definitions (note alphabetical order)
#-------------------------------------------------------

#
# sub_amdp2 - AMD Presidio-2 based 8E Ethernet port adapter
#
sub_amdp2 =	if_pas_amdp2.o if_vip_amdp2.o if_isr_amdp2.o \
		msg_amdp2.o asm_4k_vip_amdp2.o

CRYPTO_ENGINE_VIP = crypto_engine_vip_api.o \
                    crypto_engine_vip_init.o if_crypto_engine.o \
                    crypto_swepa_vip_output.o crypto_engine_vip_ipc.o \
                    crypto_engine_vip_chain.o crypto_engine_key_ipc.o \
                    crypto_hwepa_key.o crypto_hwepa_inline.o \
                    crypto_registry.o crypto_vip_map.o
#
# sub_crypto_engine
#
sub_crypto_engine = $(CRYPTO_ENGINE_CORE) $(TOOLKIT_LIB) $(CRYPTO_ENGINE_VIP)

#
#
# sub_dec21140 - Support for dec21140 based 100 Mbit Ethernet port adapter
#
sub_dec21140 =	if_pas_dec21140.o if_vip_dec21140.o msg_dec21140.o \
		if_isr_dec21140.o asm_4k_vip_dec21140.o

#
# sub_dec21150 - Support for dec pci to pci bridge
#
sub_dec21150 = 	dec_ppb.o

#
# sub_ibmtr.o - Support for ibm lan streamer token ring chip set
#
sub_ibm2692 =	if_pas_ibm2692.o if_vip_ibm2692.o if_isr_ibm2692.o \
		msg_ibm2692.o asm_4k_vip_ibm2692.o
#
# sub_epad - Support for MC68360 based Encryption port adapter
#
sub_epad=       if_pas_epad.o if_vip_epad.o if_isr_epad.o msg_epad.o \
                epad_fw.o asm_4k_vip_epad.o


sub_mif68840 = csp.o hw_pcm.o hw_pcode.o cspcfm.o cspproc.o cspecm.o \
 		csprmt.o cspglbl.o cspsig.o cspinit.o csptimer.o csplem.o \
 		cspmsg.o  \
 		fbmglbl.o fbmq.o fbminit.o fbmraf.o fbmmsg.o fbmrcv.o fbmnn.o \
 		fbmsrp.o fbmparam.o fbmsup.o fbm.o fbmpmf.o fbmtimer.o \
		fbmesf.o fbmpmfp.o fbmframe.o fbmproc.o \
 		mibget.o mibglbl.o mibinit.o mibmsg.o mibproc.o mibsup.o  \
 		mib.o mibtable.o mibchng.o mibxdi.o \
 		fddi.o fddi_chain.o fddi_debug.o msg_fddi.o fddi_parser.o \
 		mif68840_xdi.o if_vip_mif68840.o if_xdi.o if_isr_mif68840.o \
 		if_pas_mif68840.o msg_mif68840.o asm_4k_vip_mif68840.o
 
#
# sub_rvip - core rvip support
#
sub_rvip =	rvip_dma.o rvip.o \
		vip_console.o vip_dbus.o asm_4k_vip_cbus.o \
		rpsp_memd.o vip_memd.o rvip_memd.o rvip_dbus.o \
		rsp_memd.o asm_4k_vip_af.o \
		msg_pas.o if_pas.o \
		platform_rvip.o rvip_nv.o vip_chain.o \
		init_pas.o pas_chain.o monvar.o \
		msg_dma.o rvip_pci.o pci.o msg_vip.o \
		vip_debug.o util_4k.o \
		vip_af.o ipc_cbus_vip.o \
		msg_ipsecure.o 

#
# sub_fastswitch_pas - distributed fast switching routines resident on vip
#
sub_fastswitch_pas = ipfast_vip.o ipfast_vip_flow.o \
	      ipfast_vip_chain.o \
              ipfast_vip_ipc.o dfs_vip_idb.o \
              ipasm_4k.o ipfast_index.o ip_actions.o \
              ipaccess_common.o \
              ipaccess1.o ipaccess2.o ipaccess_chain.o \
              ipaccount.o ipaccount_chain.o ipaccess_vip.o

#
# sub_s4t68360 - Support for MC68360 based 4T Serial port adapter
#
sub_s4t68360 =	if_pas_s4t68360.o if_vip_s4t68360.o if_isr_s4t68360.o \
                msg_s4t68360.o asm_4k_vip_s4t68360.o

#
# sub_test - System test and diagnostics
#
sub_test =	diag_chain.o vip_test.o diag.o diag_4k.o \
		transmit_test.o pas_test.o

#
# Subsystems
#
sub_amdp2.o   			: $(sub_amdp2)   
sub_dec21140.o			: $(sub_dec21140)
sub_dec21150.o			: $(sub_dec21150)
sub_ibm2692.o			: $(sub_ibm2692)
sub_mif68840.o			: $(sub_mif68840)
sub_rvip.o			: $(sub_rvip)
sub_s4t68360.o			: $(sub_s4t68360)
sub_epad.o                      : $(sub_epad)
sub_fastswitch_pas.o		: $(sub_fastswitch_pas)
sub_test.o			: $(sub_test)
sub_crypto_engine.o		: $(sub_crypto_engine)

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
# Image Suffix's
#
production_images = $(addsuffix .ucode,$(production_basenames))
production_test_images = $(addsuffix .sun,$(production_basenames))
modular_images = $(addsuffix .ucode,$(modular_basenames))
oddball_images = $(addsuffix .ucode,$(oddball_basenames))
platform_feature_images = $(addsuffix .ucode,$(gaia_basenames))

#
# Build production images (Default)
# 
production: $(production_images) tftpcopy

production.test: $(production_test_images)
	rm -f $(production_test_images)

#
# Build bundled microcode files
#

$(UCODE_DIR)/$(RVIP_IMAGE_STR): rvip-dw-m.ucode
	cp $? $@

#
# Copy to tftpboot directory
#

tftpcopy: $(production_images)
	$(COPY) $? $(TFTPHOST)$(COLON)$(TFTPDIR)/$(RVIP_IMAGE_STR)$(BIN_SUFFIX)
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST):$(TFTPDIR)/$(RVIP_IMAGE_STR)$(BIN_SUFFIX)"

.PHONY: tftpcopy

#
# Build modular images
#
modular: $(modular_images)

#
# Given the set of object files in the modular image definitions,
# determine whether there are any modularity breaks without actually
# building the images.
#
modularity_check: $(MODULARITY_CHECK)

include $(SYSROOT)/obj-4k-rvip/make.gaia

#
# Build oddball images
#
oddballs: $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)

#--------------------------------------------------------------------------
#
# TRUNKD images:
#
# Build the suite of modular images, then delete them.
#
trunkd.test: rvip-dw-m.ucode

#
# Generic Link rules
#

rvipflex: rvipflex.o
	$(CC) -nostdlib -o rvipflex rvipflex.o

%.ucode: %.bin rvipflex $(IOS2UCODE_ELF_PROG)
	$(IOS2UCODE_ELF_PROG) -i $< -f rvipflex -o $@ -v $(RVIP_SW_NUM) \
		-V $(VPLD_VERSION) -t $(RVIP_TYPE) -m $(RVIP_HW_VERSION) \
		-h $(RVIP_HARDWARE)
	rm -rf $@.bundle*
	cat $(SYSROOT)/version
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

rvip-%-m.sun: lock $(SYS_START_FILE) main-gs-%-m gs-%.lego vip-version.o
	$(QUIET_COM)echo Linking $@
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*) \
	vip-version.o
	rm -f test.lock
	rm -f vip-version.*
	mv gs-$*-m.temp $@

vip-version.c :
	@echo 'unsigned short vip_sw_vint = \
		($(RVIP_SW_VER_MAJ) * 256) + \
		$(RVIP_SW_VER_MIN);' > vip-version.c
	@echo 'unsigned short vip_vpld_vint = \
		($(RVIP_VPLD_VER_MAJ) * 256) + \
		$(RVIP_VPLD_VER_MIN);' >> vip-version.c
	@echo 'unsigned short vip_rom_vint = \
		($(RVIP_ROM_VER_MAJ) * 256) + \
		$(RVIP_ROM_VER_MIN);' >> vip-version.c

vip-version.o : vip-version.c

.PRECIOUS: rvip-%-m.sun
.PHONY: vip-version.c

rvip-%-m: rvip-%-m.ucode
	@

include ../makedist

############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies










