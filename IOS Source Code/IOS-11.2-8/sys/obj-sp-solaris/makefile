# $Id: makefile,v 3.7.4.9 1996/09/14 07:53:37 dstine Exp $
# $Source: /release/112/cvs/Xsys/obj-sp-solaris/makefile,v $
#------------------------------------------------------------------
# Makefile for system software for the SPARC processor on Solaris
#
# Copyright (c) 1994-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.7.4.9  1996/09/14  07:53:37  dstine
# CSCdi69193:  Wrong image name used for default unix image build
# Branch: California_branch
#
# Revision 3.7.4.8  1996/09/13  04:14:43  smackie
# Fix some of the major unix image unhappiness. (CSCdi69048)
#
#    o Remove problematic unix NIT support
#    o Change default memory pool alignment to align 64 bit integers
#    o Fix message compilation errors
#
# Branch: California_branch
#
# Revision 3.7.4.7  1996/08/28  13:02:01  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.7.4.6  1996/07/14  07:13:33  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.7.4.5  1996/06/24  21:26:03  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.7.4.4  1996/05/31  21:59:15  snijsure
# CSCdi58178:  HTTP problem with flash access
# Branch: California_branch
# Implement FS_open, FS_close, FS_read using open,close, read for UNIX
# image.
#
# Revision 3.7.4.3  1996/05/22  03:33:36  mordock
# CSCdi58327:  snmp modularity commit broke some builds
# Branch: California_branch
# the ifmib and chassismib need to be included in the MIBS definition.
# this functionality used to be part of the core os.
#
# Revision 3.7.4.2  1996/03/21  22:59:25  gstovall
# Branch: California_branch
# The ELC_branch now knows the joy of California dreaming.
#
# Revision 3.7.4.1  1996/03/18  21:16:13  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.2.2.2  1996/03/18  23:24:26  mbeesley
# Branch: ELC_branch
# Sync to V111_1_3
#
# Revision 3.2.2.1  1996/02/15  19:23:29  mbeesley
# CSCdi48867:  Unix image does not compile
# Branch: ELC_branch
#
# Revision 3.7  1996/03/07  21:51:03  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.6  1996/03/07  20:50:41  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.5  1996/02/29  02:32:41  hampton
# CSCdi50303:  UNIX images dont compile
#
# Revision 3.4  1996/02/26  21:51:19  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.3  1996/02/17  09:04:14  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.2  1995/11/17  17:53:02  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  12:43:08  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.4  1995/10/18  21:47:39  hampton
# Need a space conserving way to test build all images.  Add targets for
# production.test, oddballs.test, modular.test, and everything.test.
# These delete each target after making it to limit disk space usage.
# [CSCdi42405]
#
# Revision 2.3  1995/08/07  05:40:48  richl
# CSCdi38315:  makefile rework
#
# Revision 2.2  1995/07/17  23:28:00  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.1  1995/06/07  21:52:38  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

default: unix-i-m

PLATFORM_TIMER   = unix_timer.o
PLATFORM_OTHER   = unix.o unix_ukernel.o 
PLATFORM_MONITOR = unix_monitor.o

include ../makefile.tools
include ../makefile.sp-solaris
include ../makefile

#
# Processor specific parameters
#
PLATFORM          = UNIX
PLATFORM_FLAGS    = -D$(PLATFORM)
PLATFORM_IMAGE_GROUP	= Solaris
PLATFORM_IMAGE_PREFIX	= UNIX
HWFLAGS           = unix

GDB_CORE = 	unix_exception.o msg_unix.o
GDB_UI =
UNIX =		sub_unix.o

LIBARY =	gcclib.o

DRIVER_MIBS = $(ETHERMIB) $(TRINGMIB)

DRIVERS =  $(SERIAL) $(ETHER) $(TRING) $(LEX) unix_fast.o

FASTSWITCH_CORE :=	$(FASTSWITCH_CORE) sub_sse.o
FAST_VLANS =		ieee_vlan.o isl_vlan.o vlan_les.o
FAST_IP =		sub_sse_ip.o
FAST_TBRIDGE =		sub_sse_bridge_t.o
FAST_SRB = 		sub_sse_bridge_sr.o
FAST_IPX = 		sub_sse_ipx.o
FAST_VLANS =		vlan_unix.o
UTIL :=			$(UTIL) comp_stac.o

#
# Sigh, have to redefine MIBS because of FLASHMIB
#
MIBS = $(CDPMIB) $(IPROUTE_STATIC_MIBS) $(IFMIB) $(CHASSISMIB)

PLATFORM_PARTS = $(UNIX)

POSTLIBS=	-lsocket -lnsl -ldl -lc -lgcc

#
# Redefine some build parameters for our SPARC image
#
ASMDIR= ../unix
CSWITCHESUNIX= -I$(MACHINE) -I$(HFILES) $(GDB_FLAG) $(PLATFORM_FLAGS) \
		$(OBJECT_FLAGS) $(CFLOW_FLAGS)
RAM_LDFLAGS= -e _start -nostdlib

###########################################################################
#
# Subsystems specific to the Solaris platform
#
###########################################################################

sub_lex_platform =	lex_sub_unix.o

sub_sse =		sse.o unix_sse.o sse_chain.o \
			sse_debug.o msg_sse.o unix_sse_chain.o

sub_sse_bridge_t =	bridge_sse.o bridge_sse_chain.o

sub_sse_bridge_sr =	srb_sse.o srb_sse_chain.o

sub_sse_ip =		ipfast_sse.o ipaccess_sse.o unix_ipsse.o \
			ip_sse_chain.o ip_sse.o

sub_sse_ipx =		novfast_sse.o unix_novsse.o novell_sse_chain.o	

sub_unix =		unix_io.o unix_init.o platform_unix.o unix_nv.o \
			unix_snmp.o unix_console.o unix_file_io.o

sub_unix.o 		: $(sub_unix)
sub_lex_platform.o 	: $(sub_lex_platform)
sub_sse.o		: $(sub_sse)
sub_sse_ip.o		: $(sub_sse_ip)
sub_sse_ipx.o		: $(sub_sse_ipx)
sub_sse_bridge_t.o	: $(sub_sse_bridge_t)
sub_sse_bridge_sr.o	: $(sub_sse_bridge_sr)
sub_test.o 		: $(sub_test)

include ../makesubsys
include ../makecflow
include ../makeimages

#
# Build all images 
#
everything: oddballs
	
#
# The "unofficial" engineering specials
#
oddball_images = unix-k-m unix-i-m unix-ir-m unix-d-m unix-dr-m unix-in-m \
		 unix-inr-m 
 
oddballs: $(oddball_images)

oddball_test_images = $(addsuffix .test,$(oddball_images))

oddballs.test: $(oddball_test_images)

#
###########################################################################
#                                                                         #
#                Rules for building the UNIX image                        #
#                                                                         #
###########################################################################
#
# Link rules. Note that the main.c file must be first in the list of
# objects passed to the linker. The entry point address is expected to point
# to _start(), which will call main():
###########################################################################

unix-%-m.sun: lock $(SYS_START_FILE) main-gs-%-m gs-%.lego
	$(QUIET_COM)echo Linking $@...
	$(QUIET_CC)$(CC) $(RAM_LDFLAGS) -o gs-$*-m.temp \
	$(SYS_START_FILE) \
	Main_main-gs-$*-m.o cisco-gs-$*-m.o $(image-gs-$*) $(POSTLIBS)
	rm -f test.lock
	mv gs-$*-m.temp $@

unix-%-m: unix-%-m.sun
	mv $< $@

#
# unix_io.o is compiled using standard UNIX search path for include files
#
unix_io.o: ../unix/unix_io.c
	$(CC) $(CSWITCHESUNIX) -O -c $<

unix_ukernel.o: ../unix/unix_ukernel.c
	$(CC) $(CSWITCHESUNIX) -O -c $<

asm.o vinesasm.o xnsasm.o:
	$(CC) -c ../unix/unix_null.c -o $@

include dependancies.specific
include $(SYSROOT)/dependancies
