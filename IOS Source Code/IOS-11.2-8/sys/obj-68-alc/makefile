# $Id: makefile,v 3.2.62.11 1996/09/05 22:39:59 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-68-alc/makefile,v $
#------------------------------------------------------------------
# Makefile for Synergy ATM line card IOS platform software
# (based on makefile for C3000 (aka Pancake, IGS, and so forth)).
#
# August 16, 1995, Rick Lowe
#
# Copyright (c) 1995-1997 by cisco Systems, Inc.
# All rights reserved.
# 
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.2.62.11  1996/09/05  22:39:59  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.2.62.10  1996/09/03  15:52:49  richl
# CSCdi67850:  rename images
#
# Revision 3.2.62.9  1996/07/14  07:13:15  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.2.62.8  1996/07/09  06:03:43  ppearce
# CSCdi59527: inlines waste code space
# Branch: California_branch
#   Just say "no" to inline abuse - SRB subblock inlines removed
#
# Revision 3.2.62.7  1996/07/02  01:12:12  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-68-*)
# Branch: California_branch
#
# Revision 3.2.62.6  1996/06/24  21:25:47  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.2.62.5  1996/06/22  05:25:57  ralls
# CSCdi61107:  Update make.features
# Branch: California_branch
#
# Revision 3.2.62.4  1996/06/10  20:04:01  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.2.62.3  1996/06/04  23:29:24  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.2.62.2  1996/05/24  21:54:18  jjohnson
# CSCdi58698:  unable to build alc image
# Gotta include the chassis mib in all images that have snmp
#
# Revision 3.2.62.1  1996/05/09  14:34:47  rbadri
# Branch: California_branch
# LANE, UNI3.1 features
#
# Revision 3.2.92.5  1996/05/04  02:42:55  rlowe
# Add obj-68-alc to list of production image directories.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.2.92.4  1996/05/02  17:58:22  sakumar
# Added some (more) IP subsytems to make telnet work.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.2.92.3  1996/05/02  01:46:27  rlowe
# When dispatching kernel submake from wbu-side, suppress submakes
# dispatched from sys-side.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.2.92.2  1996/04/29  22:21:41  sakumar
# Changes to Cat5K ATM HW specific code for IOS to link after latest sync.
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.2.92.1  1996/04/27  07:16:33  cakyol
# non sync sync from V112_0_2 to ....
# Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
#
# Revision 3.2.82.4  1996/04/26  05:51:11  rlowe
# Treat compiler warnings as errors.  Eliminate such warnings from
# current ACP source.  Revise LCP handshake pulse width to be closer
# to 1.0 usec.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.2.82.3  1996/04/25  05:46:10  rlowe
# Clean out obsolete files and code.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.2.82.2  1996/04/10  02:33:26  rlowe
# Add SYNALC to platforms for sys dependancies.  Also fix-up
# integration of wbu makefile with sys makefile for obj-68-alc.
# Branch: LE_Cal_V112_0_2_branch
#
# Revision 3.2.82.1  1996/04/08  01:59:30  bbenson
# Branch: LE_Cal_V112_0_2_branch
# Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
#
# Revision 3.2.68.4  1996/04/06  03:34:13  rlowe
# Allow single location (sys or wbu) to drive entire cat5k ATM image
# make and dependencies.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.2.68.3  1996/04/02  04:59:47  rlowe
# Fix up more include file paths.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.2.68.2  1996/03/23  00:06:48  rlowe
# Make use of SYSROOT, as with other obj-*-* makefiles.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.2.68.1  1996/03/22  09:40:58  rlowe
# Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
# yielding LE_Syn_Cal_V111_1_3_branch.
# Branch: LE_Syn_Cal_V111_1_3_branch
#
# Revision 3.2.52.2  1996/03/20  10:48:38  rlowe
# Integrate with ios/wbu sub-tree and makefiles.
# Also fold old Synergy atmtest.c content into vcdtest.c
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 3.2.52.1  1996/03/03  21:24:20  rlowe
# Apply SYNALC content to merge branch.
# Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
#
# Revision 3.2.50.1  1996/03/02  01:20:27  rlowe
# Non-sync sync to mainline tag V111_1_0_3.
# Branch: Synalc3_Cal_V111_1_0_3_branch
#
# Revision 3.2.34.1  1996/02/29  07:49:42  rlowe
# First pass non-sync sync for Synergy ATM line card to V111_0_16.
# Branch: Synalc3_Cal_V111_0_16_branch
#
# Revision 3.2  1995/11/17  17:52:29  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  12:42:36  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.1.62.1  1996/02/03  07:07:58  rlowe
# Non-sync sync of Synalc_catchup_branch to V111_0_5.
# Branch: Synalc_catchup_pretal_branch
#
# Revision 2.1.44.9  1995/10/23  23:24:28  rlowe
# Fix to allow for PAcKed attribute in SCP messages with NMP.
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.8  1995/09/29  18:46:01  sakumar
# Added support for vc-per-vp command for the Catalyst 5000 ATM module..
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.7  1995/09/25  21:00:50  fxu
# Branch: Synalc2_LE2_V110_1_0_3_branch
# modified emanate.o path
#
# Revision 2.1.44.6  1995/09/20  00:07:47  fxu
# Branch: Synalc2_LE2_V110_1_0_3_branch
# put Emanate subagent back.
#
# Revision 2.1.44.5  1995/09/13  03:25:13  rlowe
# Ensure that path to Synergy repository is given whether by default,
# command-line, or user environment.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.4  1995/09/12  10:59:45  rlowe
# Add back some subsystems that need to be referenced differently
# after the makefile cleanup.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.3  1995/09/12  10:31:25  rlowe
# Ensure that variable pointing to Synergy repository is passed
# to recursive makes.
# Cleanup makefile to follow along lines of c3000 makefile.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.2  1995/09/12  02:42:16  rlowe
# Allow Synergy ATM line card-specific dependencies to be generated
# from the obj-68-alc makefile.  This includes dependencies to files
# in the Synergy (non-IOS) repository.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1.44.1  1995/09/11  01:56:45  rlowe
# Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
# yielding Synalc2_LE2_V110_1_0_3_branch.
#
# Branch: Synalc2_LE2_V110_1_0_3_branch
#
# Revision 2.1  1995/06/07  21:51:55  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default alc atmiponly: alc-a-m # alc-lab-m

PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.68
include ../makefile

#
# Platform specific definitions
#
PLATFORM		= SYNALC
PLATFORM_FLAGS		= -D$(PLATFORM) -DGCC68K
PLATFORM_IMAGE_GROUP	= C5kATM
PLATFORM_IMAGE_PREFIX	= ALC
HWFLAGS			= synalc

#
# Definitions very specific to SYNALC; disallow -pipe mode during
# compilations IF we are generating assembly listings.
#
ifeq ($(strip $(DOLIST)),)              # Command-line to enable listings.
SYNALC_LST =
else
SYNALC_LST = -Wa,-al -dD > $(@:.o=).lst
COPTIONS =
endif

#
# Want to know about wbu module of IOS.
#
SYNALC_ROOT  = ../../wbu/cat5k
export SYNALC_ROOT

override ASWITCHES += $(SYNALC_LST)
override CSWITCHES += $(SYNALC_LST)

PLATFORM_SUPPORT = sub_alc.o

ROM_LDFLAGS	=
FLASH_LDFLAGS	=
RAM_LDFLAGS	= -e __start -N -nostdlib -Ttext 40010000 \
			-Wl,-defsym,_puts=_buginf
RELOC_LDFLAGS	= -e __start -N -nostdlib -Ttext 40000000 -Tdata 40000000 -r \
			-Wl,-defsym,_puts=_buginf
RELOC_SUNFLAGS	=

RELOC_LD_OPTIONS =
RELOC_MKSUM_OPTIONS =
RELOC_PIC	= relocate
RELOC_MKSUM	= reloc_mksum
RELOC_LD	= reloc_ld

#
# Define additional subsystems for manufacturing image
#
MANU =

#
# Non-modular stuff required to make the "modular" images link
#
PLATFORM_NONMODULAR =

#
# Define drivers supported by this platform
#
BIGA		= sub_biga.o
HUB		=
PARTNER		=

DRIVERS_NAS	=
DRIVERS		= $(BIGA)
DRIVERS_BOOT	=

#
# Define platform specific mibs
#
MIBS :=	$(IFMIB) $(CHASSISMIB) # $(MIBS)

#
# Defines for fast switching
#
FAST_MEDIA	=
FAST_ATALK	=
FAST_CLNS	=
FAST_DECNET	=
FAST_IBM	=
FAST_IP		= ipfast_les.o ipasm.o
FAST_IPMCAST	= ipmfast_les.o
FAST_IPX	=
FAST_TBRIDGE	=
FAST_VINES	=
FAST_XNS	=

#
# SynALC subsystem definitions (note alphabetical order)
#

sub_alc =	asm_alc.o diag.o diag_68.o diag_chain.o $(GDB)	\
		platform_synalc.o if_les.o synalc_snmp.o	\
		les_chain.o les_console_alc.o 			\
		timer_generic.o timer_alc.o			\
		alc_lec.o alc_lecmsg.o				\
		nv_les.o synalc_nv.o alc_cookie.o synalc_atm_chain.o \
		ios_atmdriver.o Syn_ipc.o ip_post_config.o

sub_biga =	if_les_biga.o if_synergy_biga.o

sub_hub =

sub_manu =

sub_partner =

sub_test =

#
# SynALC subsystem build information
#

sub_alc.o			: $(sub_alc)
sub_biga.o			: $(sub_biga)
sub_hub.o			: $(sub_hub)
sub_manu.o			: $(sub_manu)
sub_partner.o			: $(sub_partner)
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
production_images = 

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
oddball_notyet	=
oddball_images	= alc-a-m

oddballs: $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)

#
# Build the suite of modular images, then delete them.
# Currently unsupported for SYNALC.  How much router code do we run ?
#
#modular: $(MODULAR)
#	rm -f $(MODULAR)
modular:

#
# Given the set of object files in the modular image definitions,
# determine whether there are any modularity breaks without actually
# building the images. Currently unsupported for SYNALC.
#
#modularity_check: $(MODULARITY_CHECK)
modularity_check:

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Build all images
#
everything: production modular oddballs

#
#	Identify if this make is being driven by the ios/wbu/cat5k/obj-68-alc
#	makefile, or not.  If so, do NOT dispatch submakes back into that
#	place !!
#

ifeq ($(strip $(WBU_DRIVE)),)
ALC_MKWBU_DEP = @echo "Checking WBU dependancies ..." ; \
		$(REMAKE) -C ../../wbu/cat5k/obj-68-alc "IOSKERN=yes" \
			"IOSDEFS=$(PLATFORM_FLAGS) $(OBJECT_FLAGS)" null
ALC_MKWBU_LIB = @echo "Driving WBU libs ..." ; \
		$(REMAKE) -C ../../wbu/cat5k/obj-68-alc "IOSKERN=yes" \
			"IOSDEFS=$(PLATFORM_FLAGS) $(OBJECT_FLAGS)" for_kernel
ALC_MKWBU_BBI = @echo "Binding BBI image ..." ; \
		$(REMAKE) -C ../../wbu/cat5k/obj-68-alc "IOSKERN=yes" \
			"IOSDEFS=$(PLATFORM_FLAGS) $(OBJECT_FLAGS)" \
			"FVERSION=$(FVERSION)" "PVERSION=$(PVERSION)" fast
else
ALC_MKWBU_DEP = @echo "Suppressing WBU dependancies check"
ALC_MKWBU_LIB = @echo "Suppressing WBU libs generation"
ALC_MKWBU_BBI = @echo "Suppressing WBU BBI binding"
endif

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
# We drive WBU sub-makes twice here -- once for automagic dependencies,
# next for actual code libraries.
#
alc-%-l.relocbin: alc-%-l.sun
	./$(RELOC_MKSUM) $(RELOC_MKSUM_OPTIONS) $@
	ls -l $@

alc-%-l.sun: lock main-alc-%-l alc-%.lego \
	$(RELOC_LD) $(RELOC_MKSUM) $(RELOC_PIC) 
	$(ALC_MKWBU_DEP)
	$(ALC_MKWBU_LIB)
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RELOC_LDFLAGS) -o alc-$*-l.reloctemp \
	Main_main-alc-$*-l.o cisco-alc-$*-l.o $(image-alc-$*)
	mv alc-$*-l.reloctemp alc-$*-l.reloctemp
	./$(RELOC_LD) $(RELOC_LD_OPTIONS) alc-$*-l.reloctemp \
	$(RELOC_PIC) alc-$*-l.relocbin alc-$*-l.temp
	rm -f alc-$*-l.reloctemp
	mv alc-$*-l.temp $@
	rm -f test.lock

alc-%-m.sun: lock main-alc-%-m alc-%.lego
	$(ALC_MKWBU_DEP)
	$(ALC_MKWBU_LIB)
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o alc-$*-m.temp \
	Main_main-alc-$*-m.o cisco-alc-$*-m.o $(image-alc-$*)
	rm -f test.lock
	mv alc-$*-m.temp $@

#
# Special post-processing to strip a.out file for input to BBI tool.
# Now we also drive the final BBI image from here.
#
alc-%-m.ios: alc-%-m.sun
	$(NM) -n $< > $<.nmap
	objcopy.$(TOOLS_VER).68k -S -v -F "a.out-zero-big" $< $@
	rm -f atmiponly
	ln $< atmiponly
	$(ALC_MKWBU_BBI)

#
# Image rules
#

#alc-%-l: alc-%-l.reloc
	@
#alc-%-r: alc-%-r.rom
	@
alc-%-m: alc-%-m.ios # alc-%-m.tftp
	@
#alc-%-f: alc-%-f.tftp
	@

.PRECIOUS: alc-%-l.sun alc-%-m.sun alc-%-f.sun alc-%-r.sun alc-%-m.ios

.PHONY: depend.alc

include ../makedist
############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies

############################################################################

#
# Extensions to "makeimages":
# (SYNALC_LIBS files are under ios/wbu or ios/snmp-em.)
#
SYNALC_LIB0 =	lib_atmdvr_common.a lib_ipc_kernel.a lib_acpdvrs_common.a \
		lib_btglue_flash.a lib_btglue_common.a lib_flashdvr.a	\
		lib_flashcsum.a lib_leddvr_kernel.a lib_lcp_led_dvrs.a	\
		libmfp.a liblow_util.a

SYNALC_LIB1 =	../../snmp-em/snmp/emanate.o			\
		$(SYNALC_ROOT)/obj-68-alc/libc_acp_kernel.a	\
		$(SYNALC_ROOT)/obj-68-alc/liblow_util.a

SYNALC_LIBS =	$(SYNALC_LIB0:%=$(SYNALC_ROOT)/obj-68-alc/%)

$(SYNALC_LIBS) : ;

ALC_NONMODULAR=	$(ARP) $(ETHER) $(SERIAL)
ALC_OPTIONS =	$(IPCORE) $(IPUDPTCP) $(IPSERVICES) $(TACACS) $(IPDIAG) \
                $(IPDNS) $(IPACCOUNT) $(IPALIAS) $(ATM) $(ATMMIB) $(SNMP) \
                $(LANE) # $(TCL)
SUBSET_ALC =	$(KERNEL) $(ALC_OPTIONS) $(DRIVERS) $(PLATFORM_SUPPORT) $(ALC_NONMODULAR) $(CORE_IBM)

image-alc-a =	$(SUBSET_ALC)		$(SYNALC_LIBS) $(UTIL)		$(MIBS)
image-alc-lab =	$(SUBSET_ALC)		$(SYNALC_LIBS) $(UTIL)		$(MIBS)

include $(SYSROOT)/obj-68-alc/make.gaia

#############################################################################
#
# RAM Images (netbootable):
#
#############################################################################

main-alc-a-m:
	$(REMAKE) SWFLAGS=NONE HWFLAGS=$(HWFLAGS) OBJNAME=$@ main
	@HWSWNAME=alc-a-m COMP="$(CC) $(CFLAGS)" \
	flock $(SYSROOT)/version $(SYSROOT)/cisco.sh \
	"$(PLATFORM_IMAGE_GROUP) Software ($(PLATFORM_IMAGE_PREFIX)-A-M)" \
	$(SYSTEM_LABEL) $(PLATFORM) "$(CC) $(CFLAGS)" TRUE

#############################################################################
#
# Engineering special images
#
# "Unofficial", curious and oddball images
#
#############################################################################

main-alc-lab-m:
	$(REMAKE) SWFLAGS=NONE HWFLAGS=$(HWFLAGS) OBJNAME=$@ main
	@HWSWNAME=alc-lab-m COMP="$(CC) $(CFLAGS)" \
	flock $(SYSROOT)/version $(SYSROOT)/cisco.sh \
	"$(PLATFORM_IMAGE_GROUP) Software ($(PLATFORM_IMAGE_PREFIX)-LAB-M)" \
	$(SYSTEM_LABEL) $(PLATFORM) "$(CC) $(CFLAGS)" TRUE

FRC:

