# $Id: makefile,v 3.8.4.8 1996/09/14 07:53:44 dstine Exp $
# $Source: /release/112/cvs/Xsys/obj-sp-sunos/makefile,v $
#------------------------------------------------------------------
# Makefile for the SPARC SunOS native image
#
# Copyright (c) 1985-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.8.4.8  1996/09/14  07:53:44  dstine
# CSCdi69193:  Wrong image name used for default unix image build
# Branch: California_branch
#
# Revision 3.8.4.7  1996/09/13  04:14:55  smackie
# Fix some of the major unix image unhappiness. (CSCdi69048)
#
#    o Remove problematic unix NIT support
#    o Change default memory pool alignment to align 64 bit integers
#    o Fix message compilation errors
#
# Branch: California_branch
#
# Revision 3.8.4.6  1996/08/28  13:02:04  thille
# CSCdi67083: Message externs need a cleanup.
# Branch: California_branch
# Get rid of all extern msgsym()s in .c files, rework logger.h and
# msg_foo.c files so they are included and provide the externs.  Clean up
# other random externs in .c files.
#
# Revision 3.8.4.5  1996/07/14  07:13:35  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.8.4.4  1996/06/24  21:26:05  smackie
# Fix the assumption that the R4600 and 68K platforms always have a
# "classic" Rom Monitor. (CSCdi61149)
# Branch: California_branch
#
# Revision 3.8.4.3  1996/05/31  21:59:09  snijsure
# CSCdi58178:  HTTP problem with flash access
# Branch: California_branch
# Implement FS_open, FS_close, FS_read using open,close, read for UNIX
# image.
#
# Revision 3.8.4.2  1996/05/22  03:33:48  mordock
# CSCdi58327:  snmp modularity commit broke some builds
# Branch: California_branch
# the ifmib and chassismib need to be included in the MIBS definition.
# this functionality used to be part of the core os.
#
# Revision 3.8.4.1  1996/03/18  21:16:23  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.3.14.4  1996/03/16  07:14:08  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.3.14.3  1996/03/09  05:08:12  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.3.14.2  1996/03/07  10:17:14  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.3.14.1  1996/02/20  15:05:47  dstine
# Branch: DeadKingOnAThrone_branch
#         Sync from DeadKingOnAThrone_baseline_960122 to
#                   DeadKingOnAThrone_baseline_960213
#
# Revision 3.8  1996/03/07  21:51:05  rchiao
# CSCdi50987:  take out the RCS magic word
# Replace the magic word with "last comment".
#
# Revision 3.7  1996/03/07  20:50:43  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.6  1996/02/29  02:32:53  hampton
# CSCdi50303:  UNIX images dont compile
#
# Revision 3.5  1996/02/26  21:51:22  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.4  1996/02/17  09:04:29  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.3  1995/12/11  23:19:57  billw
# CSCdi42614:  RADIUS is in all images
#
# Revision 3.2  1995/11/17  17:53:05  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  12:43:11  shaker
# Bump version numbers from 2.x to 3.x.
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
include ../makefile.sp-sunos
include ../makefile

#
# Processor specific parameters
#
PLATFORM          = UNIX
PLATFORM_FLAGS    = -D$(PLATFORM)
PLATFORM_IMAGE_GROUP	= SunOS
PLATFORM_IMAGE_PREFIX	= UNIX
HWFLAGS           = unix

GDB_CORE = 	unix_exception.o msg_unix.o
GDB_UI =
UNIX =		sub_unix.o

LIBARY=		gcclib.o

DRIVER_MIBS = $(ETHERMIB) $(TRINGMIB)

DRIVERS =  $(SERIAL) $(ETHER) $(TRING) $(LEX) unix_fast.o

FASTSWITCH_CORE :=	$(FASTSWITCH_CORE) sub_sse.o
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

POSTLIBS =	-lc -lgcc

#
# Redefine some build parameters for our SPARC image
#
ASMDIR= ../unix
CSWITCHESUNIX= -I$(MACHINE) -I$(HFILES) $(GDB_FLAG) $(PLATFORM_FLAGS) \
		$(OBJECT_FLAGS) $(CFLOW_FLAGS)
RAM_LDFLAGS= -static -e __start -nostdlib -fno-builtin

###########################################################################
#
# Subsystems specific to the sunos platform
#
###########################################################################

sub_lex_platform = 	lex_sub_unix.o

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
