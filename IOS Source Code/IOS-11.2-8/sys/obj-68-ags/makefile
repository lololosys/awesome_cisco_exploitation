# $Id: makefile,v 3.14.4.7 1996/09/05 22:40:08 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-68-ags/makefile,v $
#------------------------------------------------------------------
# Makefile for AGS (CGS, MGS, and AGS+) and ASM class systems
#
# Copyright (c) 1985-1997 by cisco Systems, Inc.
# All rights reserved.
#
# Assumes GNU make and GNU cc
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.14.4.7  1996/09/05  22:40:08  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.14.4.6  1996/07/14  07:13:11  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.14.4.5  1996/07/02  01:12:05  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-68-*)
# Branch: California_branch
#
# Revision 3.14.4.4  1996/06/24  21:25:44  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.14.4.3  1996/06/10  20:03:55  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.14.4.2  1996/05/17  11:33:33  ppearce
# Merge IbuMod into Calif
#
# Revision 3.11.2.3  1996/05/02  22:05:58  hampton
# Remove support for old Multibus token ring cards.
# Branch: IbuMod_Calif_branch
#
# Revision 3.11.2.2  1996/04/03  20:11:13  ppearce
# Sync to IbuMod_Calif_baseline_960402
#
# Revision 3.11.2.1  1996/03/17  18:12:13  ppearce
# Sync IbuMod_Calif_branch to V111_1_3
#
# Revision 3.14.4.1  1996/03/18  21:15:05  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.8.12.4  1996/03/16  07:12:36  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.8.12.3  1996/03/09  05:07:43  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.8.12.2  1996/03/07  10:16:34  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.8.12.1  1996/02/20  10:43:05  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.14  1996/03/09  00:48:59  nitin
# CSCdi50416:  Move the non supported images to oddball
# Removed AGS+ images from production rule as not
# supported for 11.1.
#
# Revision 3.13  1996/03/07  21:50:42  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.12  1996/03/07  20:50:20  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.11  1996/02/26  21:50:52  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.10  1996/02/17  09:02:36  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.9  1996/01/25  11:20:55  smackie
# Registry code gets some much-needed hot-oil massaging, free liposuction
# and a makeover.  (CSCdi47101)
#
#    o Compress registry structures. Saves over 120K for all platforms
#    o Add registry subsystems and remove feature registry initializion
#      from registry.c to fix woeful initialization scaling problems
#    o Remove unused and seemingly unloved registry debugging code
#    o Add registry memory statistics to shame people into action
#
# Revision 3.8  1996/01/06  05:16:46  hampton
# Split timer specific routines into multiple files.  Move the processor
# specific routines into the src-xx-xxx directories.  [CSCdi46482]
#
# Revision 3.7  1995/12/11  23:19:37  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.6  1995/12/01  19:03:58  sdurham
# CSCdi44362:  envmon is non-modular w.r.t envmon mib.
#   Remove direct calls from envmon code to mib code. Use service points.
#
# Revision 3.5  1995/11/22  23:42:47  sdurham
# CSCdi35282:  Need command to configure envmon trap enables
#      add individual trap options in separate parser-mib file
#      envmon_mib_chain.
#
# Revision 3.4  1995/11/22  05:42:09  eschaffe
# CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
# option.
# Obsolete PROTOCONVERT flag in makefiles.  The control for defining
# protocolconversion for an image is now platform_specific.
#
# Revision 3.3  1995/11/17  17:52:26  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:09:46  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 2.9  1995/09/23  23:43:29  tli
# CSCdi40952:  tli needs a diet
# Remove more unnecessary code.
#
# Revision 2.8  1995/08/22  16:57:50  jjohnson
# CSCdi39028:  minor makefile cleanup
#
# Revision 2.7  1995/08/12  00:33:37  richl
# CSCdi38678:  ipmfast_ags.o got lost in the big makefile commit.
#
# Revision 2.6  1995/08/12  00:19:01  dino
# CSCdi38678:  ipmfast_ags.o got lost in the big makefile commit.
#
# Revision 2.5  1995/08/07  05:40:06  richl
# CSCdi38315:  makefile rework
#
# Revision 2.4  1995/07/17  23:27:40  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.3  1995/06/28  03:07:14  rmontino
# CSCdi36503:  AGS zipped images
#
# Revision 2.2  1995/06/22  17:15:42  sberl
# CSCdi34459:  make production target should build official images
# Add appn sub/superset images to trunkd.test target.
#
# Revision 2.1  1995/06/07  21:51:48  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: gs3-k-mz

PLATFORM_TIMER   = timer_generic.o timer_i8254.o
PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.68
include ../makefile

#
# Platform specific definitions
#
PLATFORM		= CSC2
PLATFORM_FLAGS		= -D$(PLATFORM)
PLATFORM_IMAGE_GROUP	= GS
PLATFORM_IMAGE_PREFIX	= GS3
HWFLAGS			= gs

PLATFORM_SUPPORT = sub_ags.o

ASM =		sub_asm.o
TEST =		sub_test.o

ENVMIB =	sub_envmib.o sub_old_envmib.o

ROM_LDFLAGS = -e __start -N -nostdlib -Ttext 1010000 -Tdata 1000
RAM_LDFLAGS = -e __start -N -nostdlib -Ttext 1000

#
# Non-modular stuff required to make the "modular" images link
#
PLATFORM_NONMODULAR = $(ATM) $(FDDI_SMT)

#
# Define drivers supported by this platform
#
DRIVER_MIBS =	$(ETHERMIB) $(TRINGMIB) $(FDDIMIB) 

DRIVERS = 	$(FLASH) $(SERIAL) $(ETHER) $(TRING) $(LEX) $(FDDI_SMT) $(ATM)

DRIVERS_CS = 	$(FLASH) $(SERIAL) $(ETHER) $(TRING) $(LEX) $(FDDI_SMT) \
	        $(ATM) $(ASM)

NAS_DRIVERS=	$(DRIVERS)

# 
# Define platform specific mibs
#
MIBS := $(MIBS) 
PLATFORM_MIBS = $(ENVMIB)
PLATFORM_PARTS = $(PLATFORM_MIBS)

#
# This platform has additional comserver support, depending on the image.
#
NAS_P_OPTIONS= 

#
# Define platform fastswitching modules
#
FAST_MEDIA      = fr_fast_mci.o atm_dxi_fast_mci.o 
FAST_ATALK      = at_fast_mci.o 
FAST_CLNS       = clnsfast_mci.o
FAST_IBM        = fastsrb_cbus.o fastdlsw_cbus.o
FAST_DECNET     = dnfast_mci.o
FAST_IP         = ipfast_mci.o ipfast_cbus.o ipasm.o ipfast_ags.o
FAST_IPMCAST	= ipmfast_ags.o 
FAST_IPX        = novfast_mci.o novfast_cbus.o
FAST_TBRIDGE    = bridge_cbus.o tbridge.o tbridge_hes.o
FAST_VINES      = vinesfast_mci.o
FAST_XNS        = xnsfast_mci.o
FAST_VLANS	= ieee_mci_vlan.o isl_mci_vlan.o

###########################################################################
#
# Subsystems specific to the AGS and ASM platforms
#
###########################################################################

sub_ags =	csc2_console.o asm_hes.o if_mci.o mci_chain.o \
		if_controller.o if_controller_chain.o hes_controller.o \
                if_ags_c7000_cmd.o \
		ags_snmp.o platform_ags.o \
		nv_68k.o cbus_debug.o if_sbe16.o if_hes_ether.o \
		if_ags_c7000.o diag.o diag_chain.o diag_68.o \
		ec.o envmon_chain.o if_hes_common.o \
		mcdvr.o flash.o dev.o \
		if_hes_serial.o if_hes_serial_parser.o cbus_chain.o ags_fci.o \
		msg_cbus.o msg_mci.o msg_csc2.o \
		msg_ec.o msg_oir.o msg_dbus.o msg_fddi.o msg_ucode.o \
		if_msg_controller.o msg_flash.o \
		hes_fddi.o fddi.o fddi_chain.o fddi_debug.o hes_xdi.o \
		fddi_parser.o \
		fddi_ags.o if_ags_c7000_trip.o if_trip.o \
		ags_c7000_fddi.o if_ags_c7000_serial.o \
		cbus_registry.o

sub_asm =	ttystp.o tty2681.o stptest.o

sub_envmib =	sr_envmib.o sr_envmib_ags.o sr_envmiboid.o \
		envmon_mib_chain.o envmib_registry.o

sub_lex_platform = lex_sub_hes.o lex_fs_mci.o

sub_old_envmib = sr_old_lenvmib.o sr_old_lenvmiboid.o

sub_test =	hes_test.o cbus_test.o trip_test.o \
		transmit_test.o cbus_test_chain.o

sub_ags.o			: $(sub_ags) 
sub_asm.o			: $(sub_asm)
sub_envmib.o			: $(sub_envmib)
sub_lex_platform.o		: $(sub_lex_platform)
sub_old_envmib.o		: $(sub_old_envmib)
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

#
# The "official" production set of images supplied to manufacturing
# AGS+ is not supported for 11.1
production_images = 

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
oddball_notyet = gs3-tli-m gs3-tic-m gs3-tiv-m
oddball_images = gs3-tatalk-m gs3-tddr-m gs3-tip-m \
		 gs3-tjpgen-m gs3-tjmanu-m gs3-tjrem-m \
                 gs3-i-m gs3-ir-m gs3-in-m \
                 gs3-inr-m gs3-d-m gs3-dr-mz \
		 gs3-p-mz gs3-ak-mz gs3-k-mz

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

#       GAIA


demo-data-build:
	@echo "$@: No internal data for this platform"

internal-data-build: 
	@echo "$@: No internal data for this platform"

feature-data-build: 
	@echo "$@: No Generic feature data"

platform-data-build: 
	@echo "$@: No Platform feature data"

feature-images-build:
	@echo "$@: No Generic feature images"

platform-images-build:
	@echo "$@: No Platform feature images"

######################################################################
#                                                             
# Feature Based Image build targets                           
#                                                             
######################################################################
                                                              
#feature based image targets will eventually appear here      
                                                              
fbi_production_images =                                       
                                                              
fbi_production: $(fbi_production_images)

######################################################################
#
#       End of GAIA
#
######################################################################
#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Build all images 
#
everything: production oddballs modular
	
#
# Communications server and Protocol Translator special main_*.c rules
#
main-cs-c-m:
	$(REMAKE) SWFLAGS=lx HWFLAGS=ts OBJNAME=$@ main
	@HWSWNAME=cs-c-m COMP="$(CC) $(CFLAGS)" \
	flock $(SYSROOT)/version $(SYSROOT)/cisco.sh \
	"CS Software (CS3-C-M)" \
	$(SYSTEM_LABEL) $(PLATFORM) "$(CC) $(CFLAGS)" TRUE
 
main-pt-lx-m:
	$(REMAKE) SWFLAGS=xl HWFLAGS=pt OBJNAME=$@ main
	@HWSWNAME=pt-lx-m COMP="$(CC) $(CFLAGS)" \
	flock $(SYSROOT)/version $(SYSROOT)/cisco.sh \
	"X.25/TCP/LAT Protocol Translator (PT3-LX-M)" \
	$(SYSTEM_LABEL) $(PLATFORM) "$(CC) $(CFLAGS)" TRUE

#
# Generic Link rules
#
gs3-%-m.czsun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

gs3-%-m.sun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

gs3-%-r.sun: lock main-gs-%-r gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(ROM_LDFLAGS) -o gs-$*-r.temp \
	Main_main-gs-$*-r.o cisco-gs-$*-r.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-r.temp $@

cs3-%-m.czsun: lock main-cs-%-m cs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o cs-$*-m.temp \
	Main_main-cs-$*-m.o cisco-cs-$*-m.o $(image-cs-$*)
	rm -f test.lock
	mv cs-$*-m.temp $@

cs3-%-m.sun: lock main-cs-%-m cs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o cs-$*-m.temp \
	Main_main-cs-$*-m.o cisco-cs-$*-m.o $(image-cs-$*)
	rm -f test.lock
	mv cs-$*-m.temp $@

pt3-%-m.czsun: lock main-pt-%-m pt-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o pt-$*-m.temp \
	Main_main-pt-$*-m.o cisco-pt-$*-m.o $(image-pt-$*)
	rm -f test.lock
	mv pt-$*-m.temp $@

pt3-%-m.sun: lock main-pt-%-m pt-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o pt-$*-m.temp \
	Main_main-pt-$*-m.o cisco-pt-$*-m.o $(image-pt-$*)
	rm -f test.lock
	mv pt-$*-m.temp $@

.PRECIOUS: gs3-%-m.sun gs3-%-r.sun gs3-%.czsun cs3-%-m.sun cs3-%.czsun \
	pt3-%-m.sun pt3-%.czsun

#
# Image rules
#
gs3-%-r: gs3-%-r.rom
	@
gs3-%-m: gs3-%-m.tftp
	@
pt3-%-m: pt3-%-m.tftp
	@
cs3-%-m: cs3-%-m.tftp
	@

include ../makedist
############################################################################

include dependancies.specific
include $(SYSROOT)/dependancies

