# $Id: makefile,v 3.13.4.23 1996/09/06 23:10:37 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-36-c1005/makefile,v $
#------------------------------------------------------------------
# Makefile for C1005 platform (aka Catwoman)
#
# October 1995, John Ng
#
# Copyright (c) 1996-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.13.4.23  1996/09/06  23:10:37  skalahas
# CSCdi68384:  misnaming of crypto images
# Branch: California_branch
#
# Revision 3.13.4.22  1996/09/05  22:34:47  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.13.4.21  1996/09/03  15:52:02  richl
# CSCdi67850:  rename images
#
# Revision 3.13.4.20  1996/08/06  18:18:21  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.13.4.19  1996/07/14  07:12:46  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.13.4.18  1996/07/02  18:17:25  ralls
# CSCdi61900:  Update make.features
# Branch: California_branch
#
# Revision 3.13.4.17  1996/07/02  00:59:15  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-36-*)
# Branch: California_branch
#
# Revision 3.13.4.16  1996/06/26  04:52:35  sdatta
# CSCdi61331: Add FRAME_RELAY here since c1005 has FRAME_RELAY but not
# the c1000
# Branch: California_branch
#
# Revision 3.13.4.15  1996/06/26  00:50:13  sdatta
# CSCdi61313:  Update makefile
# Branch: California_branch
#
# Revision 3.13.4.14  1996/06/24  21:25:29  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.13.4.13  1996/06/12  23:06:50  pbopardi
# CSCdi60241:  Async HTTP 11.2 commit
# CSCdi60245:  Frame Relay HTTP EzSetup 11.2 commit
# Branch: California_branch
#
# Revision 3.13.4.12  1996/06/12  19:56:20  jng
# CSCdi60260:  Async catwoman commit
# Branch: California_branch
# Async Catwoman commit into California
#
# Revision 3.13.4.11  1996/06/04  23:29:11  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.13.4.10  1996/05/24  06:01:53  fowler
# CSCdi58030:  c1005 images dont build
#                         Fix subsystem naming
# Branch: California_branch
#
# Revision 3.13.4.9  1996/05/23  06:47:55  fowler
# CSCdi58030:  c1005 images don't build
# Branch: California_branch
#
# Revision 3.13.4.8  1996/05/13  23:21:30  ahh
# CSCdi56426:  NAT: EFT bugs and feedback
# Fix hash algorithm, fix address block coalescing, reduce up-front
# memory allocation, drop untranslatable packets, clean up debugs.
# Branch: California_branch
#
# Revision 3.13.4.7  1996/05/01  14:30:45  fox
# CSCdi55358:  PPP subsystem needs to be more modular
# Branch: California_branch
# Make PPP NCPs into real subsystems.  Add them to all images where
# PPP and the related network protocol are located.  NCPs are dependant
# on PPP and the related network protocol subsystem but not vice versa.
#
# Revision 3.13.4.6  1996/04/23  19:29:58  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.13.4.5  1996/04/23  00:09:12  dcoli
# CSCdi44581:  Warn user when copying enterprise image to ciscoPro router
# flash
# Branch: California_branch
# Stamp a.out header with a bit identifying the image as a ciscoPro
# image.
#
# Revision 3.13.4.4  1996/04/15  21:19:55  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.13.4.3  1996/04/13  01:18:35  sgannu
# CSCdi52380:  New OSPF/PIM image for catwoman
# Branch: California_branch
# Create a new image c1005-xy2-mz which has Catwoman IP only + OSPF +PIM.
#
# Revision 3.13.4.2  1996/03/29  07:58:12  dcoli
# CSCdi44879: Create new subset images for 1000s and change some image
# names
# Branch: California_branch
# CiscoPro images need to be sub-setted for 11.1/11.2
#
# Revision 3.13.4.1  1996/03/18  21:13:39  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.9.12.4  1996/03/16  07:10:34  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.9.12.3  1996/03/09  05:07:19  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.9.12.2  1996/03/07  10:15:32  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.9.12.1  1996/02/20  15:05:00  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.13  1996/03/07  21:50:21  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.12  1996/03/07  20:49:52  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.11  1996/02/26  21:50:12  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.10  1996/02/17  09:00:05  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.9  1995/12/28  04:19:26  rchiao
# CSCdi42749:  Remove p image from the production build of c100X images
# convert makefile for 11.1. Moving c1005-p image to odd-ball.
#
# Revision 3.8  1995/12/11  23:19:20  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.7  1995/12/04  05:41:58  sgannu
# CSCdi44879:   Create new subset images for 1000s and change some old
# 1000 image names according to the new conventions.
#
# Revision 3.6  1995/12/03  17:57:31  thille
# CSCdi44905:  ciscopro targets not in 11.1
# Put them in.
#
# Revision 3.5  1995/11/29  18:29:14  dblair
# CSCdi40974:  PPP Multi Link breaks modularity
# include ppp_debug.h to resolve references to multilink_debug
#
# Revision 3.4  1995/11/28  11:01:06  tli
# CSCdi43711:  SP subset dropped X.25
# Add SP support
#
# Revision 3.3  1995/11/17  17:51:42  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:08:05  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c1005-bny-mz

PLATFORM_OTHER   = c1000_ukernel.o
PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.36
include ../makefile
include ../obj-36-c1000/c1000_make_common_defs

# INTERFACE is derived from ../les/if_table.h
INTERFACE = 0x03

PLATFORM_IMAGE_GROUP = 1005
PLATFORM_IMAGE_PREFIX = C1005

QUICC_ASYNC =	sub_quicc_async.o
QUICC_SERIAL =	sub_quicc_serial.o

DRIVERS = $(QUICC_ETHER) $(QUICC_SERIAL) $(ETHER) $(SERIAL)
DRIVER_MIBS = $(ETHERMIB) 
DRIVERS_ASYNC = $(QUICC_ETHER) $(QUICC_ASYNC) $(ETHER) $(SERIAL)

PLATFORM_PARTS = $(FRAME_RELAY) $(FRAME_RELAY_ARP) $(FRAME_RELAY_TRAFFIC)\
	         $(FRAME_RELAY_MIB) $(FAST_FR) $(SMDS) $(FRAME_RELAY_HTTP)
PLATFORM_PLUS = $(OSPF) $(FAST_IPMCAST) $(IPMULTICAST) $(PIM_OSPF_MIBS) \
		$(IPNAT) $(FAST_IPNAT) $(SMRP) $(FRAME_RELAY_SVC) $(ATALK_IP) \
		$(ATALK_AURP) $(RSVP)  $(IPX_NLSP) $(ISIS_NLSP_CORE) \
		$(IPX_NLSP_MIBS)


# Desktop Subset Definitions
# Catwoman IP/IPX/ATALK subset with only X25.

sub_quicc_serial =	if_c1000_serial.o if_serial_quicc.o if_les_serial.o \
			asm_c1000_serial.o cpai.o msg_serial_quicc.o \
			quicc_serial_common.o

sub_quicc_async  =	asm_c1000_serial.o cpai.o msg_serial_quicc.o \
			if_async_quicc.o quicc_serial_common.o \
			if_les_async_chain.o


#
# c1005 subsystem build information
#
sub_quicc_serial.o		: $(sub_quicc_serial)

sub_quicc_async.o		: $(sub_quicc_async)

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
production_images = c1005-y-mz c1005-by-mz c1005-ny-mz c1005-bny-mz \
		    c1005-bnsy-mz c1005-qy-mz c1005-nqy-mz c1005-y2-mz

production: $(production_images)

production_test_images = $(addsuffix .test,$(production_images))

production.test: $(production_test_images)

#
# Crypto production available only to the crypto gang and release meisters
#
crypto_images = c1005-bnsy40-mz c1005-bnsy56-mz

crypto: $(crypto_images)

crypto_test_images = $(addsuffix .test,$(crypto_images))
crypto.test: $(crypto_test_images)

#
# The "unofficial" engineering specials
#
oddball_notyet = c1005-tddr-m c1005-tiv-m
oddball_images = c1005-tatalk-m c1005-tic-m c1005-tip-m c1005-tli-m \
		 c1005-p-mz

oddballs: $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)

#
# Build suite of modular images, then delete them.
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
# Build all images 
#
everything: production modular

include $(SYSROOT)/obj-36-c1005/make.gaia

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) 

#
# Build all images 
#
everything: production modular
	
#
# Generic Link rules.
#

c1005-%-m.czsun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c1005-%-m.sun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c1005-%-r.sun: lock main-gs-%-r gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(ROM_LDFLAGS) -o gs-$*-r.temp \
	Main_main-gs-$*-r.o cisco-gs-$*-r.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-r.temp $@

#
# Image rules
#
c1005-%-r: c1005-%-r.rom
	@
c1005-%-m: c1005-%-m.tftp
	@
c1005-%-mz: c1005-%-mz.tftp
	@

.PRECIOUS: c1005-%-m.sun c1005-%-m.czsun c1005-%-r.sun 

#
############################################################################
#
# 			Libraries and the Like
#
############################################################################
include ../makedist

include dependancies.specific
include $(SYSROOT)/dependancies
