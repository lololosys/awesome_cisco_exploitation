# $Id: makefile,v 3.14.4.19 1996/09/12 00:50:26 skalahas Exp $
# $Source: /release/112/cvs/Xsys/obj-36-c1000/makefile,v $
#------------------------------------------------------------------
# Makefile for system software port to the M68EN360 (Sapphire)
#
# November 1994, David Coli
#
# Copyright (c) 1994-1997 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.14.4.19  1996/09/12  00:50:26  skalahas
# CSCdi68904:  Add c1000-rboot-r in production_images list
# Branch: California_branch
#
# Revision 3.14.4.18  1996/09/05  22:34:21  skalahas
# CSCdi68227:  miscellaneous makefile cleanup
# Branch: California_branch
#
# Revision 3.14.4.17  1996/09/03  18:48:06  richl
# CSCdi67863:  commit gaia stuff, and crypto top-level target
#
# Revision 3.14.4.16  1996/09/03  15:51:55  richl
# CSCdi67850:  rename images
#
# Revision 3.14.4.15  1996/08/06  18:18:04  skalahas
# CSCdi65242:  GAIA makefile enhancements
# Branch: California_branch
#
# Revision 3.14.4.14  1996/07/14  07:12:43  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.14.4.13  1996/07/02  00:58:42  jpfeifer
# CSCdi61850:  adding support for gaia based production images (obj-36-*)
# Branch: California_branch
#
# Revision 3.14.4.12  1996/06/28  02:00:31  sdatta
# CSCdi61603:  Remove IPX_C1000 from SUBSET_IPATALK_ISDN
# Branch: California_branch
#
# Revision 3.14.4.11  1996/06/26  00:46:41  sdatta
# CSCdi61311:  makefile
# Branch: California_branch
#
# Revision 3.14.4.10  1996/06/24  21:25:27  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.14.4.9  1996/06/10  20:04:08  ralls
# CSCdi60025:  Gaia, small fixes...
# Branch: California_branch
#
# Revision 3.14.4.8  1996/06/04  23:29:18  ralls
# CSCdi57580:  Update gaia features, onec more
# Branch: California_branch
#
# Revision 3.14.4.7  1996/05/22  04:05:44  fox
# CSCdi58392:  Appletalk Control Protocol not included in Appletalk image
# Branch: California_branch
# Uhhhh, ATCP needs to be most places that ATALK is.
#
# Revision 3.14.4.6  1996/05/01  14:30:32  fox
# CSCdi55358:  PPP subsystem needs to be more modular
# Branch: California_branch
# Make PPP NCPs into real subsystems.  Add them to all images where
# PPP and the related network protocol are located.  NCPs are dependant
# on PPP and the related network protocol subsystem but not vice versa.
# Add $(SERIAL) to DRIVERS_RBOOT in order fix missing 'serial_setencap'.
#
# Revision 3.14.4.5  1996/04/23  19:30:10  ralls
# CSCdi55428:  Update the gaia packaging data
# Branch: California_branch
#
# Revision 3.14.4.4  1996/04/23  00:09:09  dcoli
# CSCdi44581:  Warn user when copying enterprise image to ciscoPro router
# flash
# Branch: California_branch
# Stamp a.out header with a bit identifying the image as a ciscoPro
# image.
#
# Revision 3.14.4.3  1996/04/15  21:16:04  bcole
# CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
# commit
# Branch: California_branch
#
# Revision 3.14.4.2  1996/03/29  07:58:10  dcoli
# CSCdi44879: Create new subset images for 1000s and change some image
# names
# Branch: California_branch
# CiscoPro images need to be sub-setted for 11.1/11.2
#
# Revision 3.14.4.1  1996/03/18  21:13:29  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.8.12.4  1996/03/16  07:10:17  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.8.12.3  1996/03/09  05:07:15  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.8.12.2  1996/03/07  10:15:27  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.8.12.1  1996/02/20  15:04:42  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.14  1996/03/09  22:56:21  widmer
# CSCdi47890:  HTTP server doesnt work
# Add HTTP server
#
# Revision 3.13  1996/03/07  21:50:18  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.12  1996/03/07  20:49:50  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.11  1996/02/26  21:50:02  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.10  1996/02/17  08:59:48  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.9  1996/02/15  18:02:36  gtaylor
# CSCdi45779:  Leased Line ISDN work.
#
# Revision 3.8  1995/12/28  04:15:25  rchiao
# CSCdi42749:  Remove p image from the production build of c100X images
# coverting makefile for 11.1.  Move c1000-p image to odd-ball.
#
# Revision 3.7  1995/12/11  23:19:17  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.6  1995/12/04  05:41:56  sgannu
# CSCdi44879:   Create new subset images for 1000s and change some old
# 1000 image names according to the new conventions.
#
# Revision 3.5  1995/12/03  17:57:29  thille
# CSCdi44905:  ciscopro targets not in 11.1
# Put them in.
#
# Revision 3.4  1995/11/28  11:00:58  tli
# CSCdi43711:  SP subset dropped X.25
# Update SP support
#
# Revision 3.3  1995/11/17  17:51:39  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.2  1995/11/17  00:07:52  gstovall
# Ladies and gentlemen, I introduce to you, the port ready commit.
#
# Revision 2.11  1995/09/23  05:57:31  enf
# CSCdi39189:  100X need insurance against loading incorrect image
#
# Revision 2.10  1995/09/04  00:58:45  richl
# CSCdi39767:  Add BiSync to -r- images on low end
# Move bisync subsystems out of common code, into platforms on
# which it is supported.
#
# Revision 2.9  1995/08/22  16:57:35  jjohnson
# CSCdi39028:  minor makefile cleanup
#
# Revision 2.8  1995/08/09  19:32:43  sgannu
# CSCdi38309:  catwoman msg causes modular build failure
#
# Revision 2.7  1995/08/07  05:39:27  richl
# CSCdi38315:  makefile rework
#
# Revision 2.6  1995/07/17  23:27:19  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.5  1995/07/17  18:45:07  abaindur
# CSCdi35639:  Change production images for c1000 to cziped ones
#
# Revision 2.4  1995/07/07  16:26:58  abaindur
# CSCdi36780:  .sun file for c1000 rboot-r images not preserved
# Add to .PRECIOUS list.
#
# Revision 2.3  1995/06/20  03:01:33  speakman
# CSCdi36088:  undefined tbridge references making trunkd.test
# Remove tbridge_smf_... functions from tbridge.c to bridge.c
# in the interest of the lite images that build with only
# process-level transparent bridging.
#
# Revision 2.2  1995/06/19  10:02:39  tli
# CSCdi36023:  Optimize service provider image for 1003.
# It's too big for default flash.  Compress the image.
#
# Revision 2.1  1995/06/07  21:46:27  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: c1000-bny-mz

PLATFORM_OTHER   = c1000_ukernel.o
PLATFORM_MONITOR = asm_68_monitor.o

include ../makefile.tools
include ../makefile.36
include ../makefile
include c1000_make_common_defs

#
# Platform specific definitions
#
PLATFORM_IMAGE_PREFIX = C1000

# INTERFACE is derived from ../les/if_table.h
INTERFACE = 0x09

BRI =		sub_bri.o
DRIVERS = 	$(QUICC_ETHER) $(ETHER) $(SERIAL) 
PLATFORM_PARTS = $(ISDN) $(ISDN_HTTP) $(ISDNMIB) $(CALLMIB) $(BRI)
PLATFORM_PLUS = $(OSPF) $(FAST_IPMCAST) $(IPMULTICAST) $(PIM_OSPF_MIBS) \
		$(IPNAT) $(FAST_IPNAT) $(SMRP) $(ATALK_IP) $(ATALK_AURP) \
		$(RSVP) $(IPX_NLSP) $(ISIS_NLSP_CORE) $(IPX_NLSP_MIBS)

sub_bri =	if_c1000_bri.o if_les_bri_c1000.o if_bri_quicc.o \
		if_les_bri_chain.o bri_debug.o msg_bri.o asm_c1000_bri.o \
		if_les_leased_bri.o

#
# c1003 susbsytem build information
#
sub_bri.o			: $(sub_bri)

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
production_images = c1000-y-mz c1000-by-mz c1000-ny-mz c1000-bny-mz \
		    c1000-bnsy-mz  c1000-rboot-r

#
# Please see richl about the rboot image.
#
#c1000-rboot-r 
 
production: $(production_images)

production_test_images = $(addsuffix .test,$(production_images))

production.test: $(production_test_images)

#
# Crypto production available only to the crypto gang and release meisters
#
crypto_images = c1000-bnsy40-mz c1000-bnsy56-mz

crypto: $(crypto_images)

crypto_test_images = $(addsuffix .test,$(crypto_images))
crypto.test: $(crypto_test_images)


#
# The "unofficial" engineering specials
#
oddball_notyet = c1000-tddr-m c1000-tiv-m
oddball_images = c1000-tatalk-m c1000-tic-m c1000-tip-m c1000-tli-m \
	         c1000-p-mz

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

include $(SYSROOT)/obj-36-c1000/make.gaia

#
# These images need a little modularity work
#
modular_notyet: $(MODULAR_NOTYET) $(oddball_notyet)

#
# Build all images 
#
everything: production oddballs modular
	
#
# Generic Link rules.
#

c1000-%-m.czsun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c1000-%-m.sun: lock main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-m.temp $@

c1000-%-r.sun: lock main-gs-%-r gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(LD) $(ROM_LDFLAGS) -o gs-$*-r.temp \
	Main_main-gs-$*-r.o cisco-gs-$*-r.o $(image-gs-$*)
	rm -f test.lock
	mv gs-$*-r.temp $@

#
# Image rules
#
c1000-%-r: c1000-%-r.rom
	@
c1000-%-m: c1000-%-m.tftp
	@
c1000-%-mz: c1000-%-mz.tftp
	@

.PRECIOUS: c1000-%-m.sun c1000-%-m.czsun c1000-%-r.sun 

#
############################################################################
#
# 			Libraries and the Like
#
############################################################################
include ../makedist

include dependancies.specific
include $(SYSROOT)/dependancies




