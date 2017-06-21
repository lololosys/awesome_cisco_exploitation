# $Id: makefile,v 3.2.60.22 1996/09/10 03:00:19 cheilek Exp $
# $Source: /release/112/cvs/Xsys/obj-4k-svip/makefile,v $
#------------------------------------------------------------------
# makefile - VIPER makefile
#
# January 1995, Michael Beesley
#
# Copyright (c) 1995-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.2.60.22  1996/09/10  03:00:19  cheilek
# CSCdi61961:  need to add crypto target to svip makefile
# Branch: California_branch
# Bundle crypto VIP microcode with crypto images.
#
# Revision 3.2.60.21  1996/09/05  22:40:15  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.2.60.20  1996/09/03  15:52:42  richl
# CSCdi67850:  rename images
#
# Revision 3.2.60.19  1996/08/29  17:41:01  drowell
# CSCdi46093:  bundling the ucode is way to slow
# Branch: California_branch
# Introduced new mkbndl2 script which bundles ucode by directly generating
# an ELF object file. Also allows selection of ucode by flags in the ucode
# list file.
#
# Revision 3.2.60.18  1996/08/16  08:02:20  dlobete
# CSCdi59971:  DACL needs modularity work
# Branch: California_branch
# For the sake of future maintenance of IP access-lists, share as much
# source code as possible between VIP and RSP.
#
# Revision 3.2.60.17  1996/08/15  22:31:47  ptran
# CSCdi66137:  Add oddball image for ESA diag.
# Branch: California_branch
#
# Revision 3.2.60.16  1996/07/14  07:13:05  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.2.60.15  1996/07/02  01:07:45  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-4k-*)
# Branch: California_branch
#
# Revision 3.2.60.14  1996/07/01  23:31:33  xliu
# CSCdi61833:  vip2 crypto target break the make.
# Branch: California_branch
#
# Revision 3.2.60.13  1996/07/01  16:23:31  xliu
# CSCdi61767:  add vip2 crypto image name to the production
# Branch: California_branch
#
# Revision 3.2.60.12  1996/06/26  03:42:23  ondiaye
# CSCdi61326:  add svip_cyatest.c to California Branch
# Branch: California_branch
#
# Revision 3.2.60.11  1996/06/24  21:25:41  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.2.60.10  1996/06/12  07:41:04  cheilek
# CSCdi58294:  vip build modifies files under CVS control
# Branch: California_branch
# Automatically build VIP and VIP2 microcode as needed for *-*v-* images.
#
# Revision 3.2.60.9  1996/06/04  23:34:12  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.2.60.8  1996/05/08  23:03:55  mdb
# Fix minor breakage from CSCdi57043 commit. This should force the
# needed tools to be properly built before they are referenced.
# Branch: California_branch
#
# Revision 3.2.60.7  1996/05/08  02:54:04  cheilek
# CSCdi55787:  Microcode directory not updated when vip image built
# Branch: California_branch
# Unless the NO_VIP_BUNDLE flag is set, copy the vip image to the
# ../../micro directory and update rspv_ucode.files
#
# Revision 3.2.60.6  1996/05/08  01:10:00  gchristy
# CSCdi56944:  Compilation errors from IP named access list commit
# Branch: California_branch
# - Modularize new ACL code for inclusion in VIP images.
#
# Revision 3.2.60.5  1996/04/23  23:43:26  ptran
# CSCdi55475:  need to include crypto engine in svip image.
# Branch: California_branch
# Include crypto engine in svip image.
#
# Revision 3.2.60.4  1996/04/23  19:33:12  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.2.60.3  1996/04/15  21:20:15  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.2.60.2  1996/03/29  23:04:29  hampton
# Bring the SVIP makefile into the 90's.  [CSCdi52864]
# Branch: California_branch
#
# Revision 3.2.60.1  1996/03/21  22:58:39  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.2.2.7  1996/03/18  23:17:00  mbeesley
# Branch: ELC_branch
# Sync to V111_1_3
#
# Revision 3.2.2.6  1996/03/13  23:52:58  fsunaval
# CSCdi50071:  Addition to the SVIP family
# Branch: ELC_branch
#
# Revision 3.2.2.5  1996/03/07  17:28:41  getchell
# CSCdi50953:  viper dfs mods
# Branch: ELC_branch
#
# Revision 3.2.2.4  1996/02/21  21:47:29  mbeesley
# CSCdi49481:  VIPs have no need of pas_controller.c. Link it out.
# Branch: ELC_branch
#
# Revision 3.2.2.3  1996/02/21  20:35:45  getchell
# CSCdi49474:  Time to turn on IPC/DFS on viper
# Bump version number
# Branch: ELC_branch
#
# Revision 3.2.2.2  1996/02/16  22:32:19  getchell
# CSCdi49237:  svip kernel and asm files out of whack
# Modify CODE_START to reclaim long lost DRAM (bringup kruft)
# Branch: ELC_branch
#
# Revision 3.2.2.1  1996/02/10  00:06:49  mbeesley
# CSCdi48581:  Add VIP2 support to ELC_branch
# Branch: ELC_branch
#
# Revision 3.2  1995/11/17  17:52:18  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  12:42:20  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1  1995/06/07  21:51:26  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: svip-dw-m.ucode tftpcopy

production_basenames = svip-dw-m  
modular_basenames    =
oddballs_basename    =
gaia_basenames =  svip-dw-m 

PLATFORM_TIMER   = timer_generic.o timer_nevada.o asm_4k_timer.o 
PLATFORM_OTHER	 = svip_vspace.o svip_stubs.o asm_4k_svip.o \
		   vip_ukernel.o svip_ukernel.o
PLATFORM_MONITOR = 

include ../makefile.tools
include ../makefile.4k
include ../makefile

#--------------------------------------------------------
# Hardware Specific Definitions:
#--------------------------------------------------------
SVIP_TYPE	= 21
SVIP_HW_VERSION	= 2.0
SVIP_HARDWARE	= VIPER_HW_REV1
CODE_START	= 0x60010000

#-------------------------------
# Microcode destination
#-------------------------------
UCODE_DIR        = ../../micro
RSP_UCODE_FILES  = $(UCODE_DIR)/rsp_ucode.files

#--------------------------------------------------------
# Version number extraction from rspv_ucode.files
#--------------------------------------------------------

EXTRACT_SVIP_VER_MAJ_CMD := $(shell perl -n -e 'print "$$1" if /micro\/vip2_(\d+)-(\d+)/' < $(RSP_UCODE_FILES))
EXTRACT_SVIP_VER_MIN_CMD := $(shell perl -n -e 'print "$$2" if /micro\/vip2_(\d+)-(\d+)/' < $(RSP_UCODE_FILES))

#--------------------------------------------------------
# Version numbers and strings
#--------------------------------------------------------
SVIP_SW_VER_MAJ	= $(EXTRACT_SVIP_VER_MAJ_CMD)
SVIP_SW_VER_MIN	= $(EXTRACT_SVIP_VER_MIN_CMD)
SVIP_SW_VER	= $(SVIP_SW_VER_MAJ)-$(SVIP_SW_VER_MIN)
SVIP_SW_NUM	= $(SVIP_SW_VER_MAJ).$(SVIP_SW_VER_MIN)
SVIP_IMAGE_STR	= vip2_$(SVIP_SW_VER)

#-------------------------------
# Platform specific definitions
#-------------------------------
PLATFORM		= SVIP
PLATFORM_FLAGS          = -DBH_POSTED_READS -DPCI_POSTED_READS -D$(PLATFORM) \
			  -DPOSTED_READ -DACC_POSTED_READ_SUPPORT
PLATFORM_IMAGE_GROUP	= VIP
PLATFORM_IMAGE_PREFIX	= SVIP
# 
# FIXME: Is this correct ?
#
HWFLAGS			= les

ROM_LDFLAGS= -e __start -N $(GNUM) -Ttext $(CODE_START) -Tsvip.link -nostdlib
RAM_LDFLAGS= -e __start -N $(GNUM) -Ttext $(CODE_START) -Tsvip.link -nostdlib

PLATFORM_SUPPORT_DSW = sub_svip.o sub_fastswitch_pas.o

#--------------------------------------------------------------------
# Sched alloc is used on this platform - hence override this variable
# so that support for sched-interval is not pulled in
#--------------------------------------------------------------------
SCHED_INTERVAL =

#----------------------------------------------------
# crypto support
#----------------------------------------------------
CRYPTO_VIP = $(CRYPTO_CORE) $(CRYPTO_ENGINE_VIP) 

#----------------------------------------------------
# Platform specific drivers, and peripheral support.
#----------------------------------------------------

DRIVERS_DSW =	$(AMDP2) $(DEC21140) $(DEC21150) $(MIF68840) \
		$(IBM2692) $(S4T68360)  
FAST_DRIVER =	
FASTSWITCH = 	
AMDP2 =		sub_amdp2.o   
DEC21140 =	sub_dec21140.o
DEC21150 =	sub_dec21150.o
MIF68840 =  	sub_mif68840.o
S4T68360 =	sub_s4t68360.o
TEST =		sub_test.o
IBM2692 =	sub_ibm2692.o
FAST_IPBRIDGING = 
EPAD =          sub_epad.o
PAS_DIAG =      sub_pas_diag.o
 
#-------------------------------------------------------
# Rules to build assembly language portions of the SVIP
#-------------------------------------------------------

SVIP_ASM = 	asm_4k.o asm_4k_timer.o asm_4k_svip.o asm_4k_vip_cbus.o \
		asm_4k_vip_dec21140.o asm_4k_vip_amdp2.o asm_4k_vip_af.o \
		asm_4k_vip_ibm2692.o asm_4k_vip_mif68840.o \
		asm_4k_vip_s4t68360.o ipasm_4k.o

$(SVIP_ASM):
	$(QUIET_COM) echo "Compiling $< to $@"
	$(QUIET_CC)$(AS) -c $(ASWITCHES) $(WARN_ALL) $< \
	|| (rm -f $@ ; exit 1)


#-------------------------------------------------------
# SVIP subsystem definitions (note alphabetical order)
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
# sub_crypto_engine: esa driver code is removed untill new ESA firmware is
# delivered.
#
sub_crypto_engine = $(CRYPTO_ENGINE_CORE) $(TOOLKIT_LIB) $(CRYPTO_ENGINE_VIP)
 

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
# sub_epad - Support for MC68360 based Encryption port adapter
#
sub_epad=       if_pas_epad.o if_vip_epad.o if_isr_epad.o msg_epad.o \
                epad_fw.o asm_4k_vip_epad.o
 
#
# sub_ibmtr.o - Support for ibm lan streamer token ring chip set
#
sub_ibm2692 =	if_pas_ibm2692.o if_vip_ibm2692.o if_isr_ibm2692.o \
		msg_ibm2692.o asm_4k_vip_ibm2692.o

#
# sub_mif68840.o - Support for Motorola IFDDI chip set
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
 		mif68840_xdi.o if_vip_mif68840.o if_xdi.o if_isr_mif68840.o \
 		if_pas_mif68840.o msg_mif68840.o asm_4k_vip_mif68840.o

#
# sub_svip - core svip support
#
sub_svip =	svip_dma.o svip.o \
		vip_console.o vip_dbus.o asm_4k_vip_cbus.o \
		rpsp_memd.o vip_memd.o svip_memd.o svip_dbus.o \
		rsp_memd.o asm_4k_vip_af.o \
		msg_pas.o if_pas.o \
		platform_svip.o vip_nv.o vip_chain.o \
		init_pas.o pas_chain.o monvar.o \
		svip_msg_dma.o svip_pci.o pci.o msg_vip.o \
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
		transmit_test.o pas_test.o svip_cyatest.o
sub_pas_diag = pas_diags.o epa_diags.o

#
# Subsystems
#
sub_amdp2.o   		: $(sub_amdp2)   
sub_dec21140.o		: $(sub_dec21140)
sub_dec21150.o		: $(sub_dec21150)
sub_ibm2692.o		: $(sub_ibm2692)
sub_mif68840.o      	: $(sub_mif68840)
sub_svip.o		: $(sub_svip)
sub_s4t68360.o		: $(sub_s4t68360)
sub_fastswitch_pas.o	: $(sub_fastswitch_pas)
sub_test.o		: $(sub_test)
sub_crypto_engine.o             : $(sub_crypto_engine)
sub_epad.o                      : $(sub_epad)
sub_pas_diag.o                  : $(sub_pas_diag)

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

$(UCODE_DIR)/vip2_%: svip-dw-m.ucode
	cp $? $@

$(UCODE_DIR)/vip2c_%: svip-cdw-m.ucode
	cp $? $@

#
# Copy to tftpboot directory
#

tftpcopy: $(production_images)
	$(COPY) $? $(TFTPHOST)$(COLON)$(TFTPDIR)/$(SVIP_IMAGE_STR)$(BIN_SUFFIX)
	@-echo ""
	@-echo "TFTP image: $(TFTPHOST):$(TFTPDIR)/$(SVIP_IMAGE_STR)$(BIN_SUFFIX)"

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

include $(SYSROOT)/obj-4k-svip/make.gaia

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
trunkd.test: svip-dw-m.ucode

#
# Generic Link rules
#

%.ucode: %.bin $(IOS2UCODE_ELF_PROG)
	$(IOS2UCODE_ELF_PROG) -i $< -o $@ -v $(SVIP_SW_NUM) \
		-t $(SVIP_TYPE) -m $(SVIP_HW_VERSION) -h $(SVIP_HARDWARE)
	rm -rf $@.bundle*
	cat $(SYSROOT)/version
	sh $(SYSROOT)/symbols.sh "$(NM)" $*.sun $*.symbols $(PLATFORM) $*

svip-%-m.sun: lock $(SYS_START_FILE) main-gs-%-m gs-%.lego vip-version.o
	$(QUIET_COM)echo Linking $@
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*) \
	vip-version.o
	rm -f test.lock
	rm -f vip-version.*
	mv gs-$*-m.temp $@

vip-version.c :
	@echo 'unsigned short vip_sw_vint = \
		($(SVIP_SW_VER_MAJ) * 256) + \
		$(SVIP_SW_VER_MIN);' > vip-version.c

vip-version.o : vip-version.c

.PRECIOUS: svip-%-m.sun
.PHONY: vip-version.c

svip-%-m: svip-%-m.ucode
	@

include ../makedist

############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies
