# $Id: makefile,v 3.6.4.1 1996/07/14 07:13:08 hampton Exp $
# $Source: /release/112/cvs/Xsys/obj-68/makefile,v $
#------------------------------------------------------------------
# makefile - Makefile for common 68K objects
#
# December 1994, Scott Mackie
#
# Copyright (c) 1994-1996 by cisco Systems, Inc.
# All rights reserved.
#------------------------------------------------------------------
# $Log: makefile,v $
# Revision 3.6.4.1  1996/07/14  07:13:08  hampton
# Eliminate all redundant dependencies for any given file.  Split out
# platform specific dependencies and put them in the appropriate obj-xxx
# directory.  [CSCdi62810]
# Branch: California_branch
#
# Revision 3.6  1996/03/07  21:56:30  rchiao
# CSCdi50987:  take out the RCS magic word
# replace the magic word with "last comment".
#
# Revision 3.5  1996/03/07  20:50:15  rchiao
# CSCdi50983:  need an empty line after last comment in makfiles
# add an empty line in makefiles right after the last comment.
#
# Revision 3.4  1996/02/26  21:50:48  rchiao
# CSCdi49917:  makefile changes for ClearCase, part II
# Please see 'Description' for a more detailed explanation.
# The derived elements such as 'version' and 'dependancies' now needs
# absolute path in order to access.
#
# Revision 3.3  1996/02/17  09:02:15  rchiao
# CSCdi49271:  makefile changes for ClearCase
#
# Revision 3.2  1995/11/17  17:52:21  hampton
# Remove old entries from the RCS header logs.
#
# Revision 3.1  1995/11/09  12:42:25  shaker
# Bump version numbers from 2.x to 3.x.
#
# Revision 2.2  1995/07/17  23:27:37  smackie
# Tidy up various makefile semantics (CSCdi37193)
#
#   o Split out native tool support from cross-compilation tools
#   o Rename errant makefiles
#   o Make native object directories more explicitly named
#   o Clean up bogus tool includes
#
# Revision 2.1  1995/06/07  21:51:41  hampton
# Bump version numbers from 1.x to 2.x.
#
#------------------------------------------------------------------
# $Endlog$
#

ifeq ($(origin SYSROOT), undefined)
SYSROOT :=       $(shell ../scripts/sysroot)
endif

include ../makefile.tools
include ../makefile.68
include ../makefile
include ../makesubsys
include ../makecflow

include dependancies.specific
include $(SYSROOT)/dependancies
