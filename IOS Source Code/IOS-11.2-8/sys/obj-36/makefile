# $Id: makefile,v 3.6.4.2 1996/07/14 07:12:14 hampton Exp $
# $Source: /release/112/cvs/Xsys/obj-36/makefile,v $
#------------------------------------------------------------------
# makefile - Makefile for common 68360 objects
#
# October 1995, John Ng
#
# Copyright (c) 1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.6.4.2  1996/07/14  07:12:14  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.6.4.1  1996/03/18  21:13:17  gstovall
# Branch: California_branch
# Elvis has left the building.  He headed out to California, and took the
# port ready changes with him.
#
# Revision 3.2.28.4  1996/03/16  07:10:02  gstovall
# Branch: DeadKingOnAThrone_branch
# Make the king aware of V111_1_3.
#
# Revision 3.2.28.3  1996/03/09  05:07:04  hampton
# Slim down the kernel of the router.
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.2.28.2  1996/03/07  10:15:20  mdb
# Branch: DeadKingOnAThrone_branch
# cisco and ANSI/POSIX libraries.
#
# Revision 3.2.28.1  1996/02/20  15:04:33  dstine
# Branch: DeadKingOnAThrone_branch
#
# Revision 3.6  1996/03/07  21:56:17  rchiao
# CSCdi50987:  take out the RCS magic word
# replace the magic word with "last comment".
#
# Revision 3.5  1996/03/07  20:49:46  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.4  1996/02/26  21:49:25  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.3  1996/02/17  08:59:17  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.2  1995/11/17  17:51:34  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  12:41:34  shaker
# Bump version numbers from 2.x to 3.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

include ../makefile.tools
include ../makefile.36
include ../makefile
include ../makesubsys
include ../makecflow

include dependancies.specific
include $(SYSROOT)/dependancies
