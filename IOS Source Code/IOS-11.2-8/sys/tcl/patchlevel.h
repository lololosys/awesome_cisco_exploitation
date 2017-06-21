/* $Id: patchlevel.h,v 3.2 1995/11/17 18:46:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/tcl/patchlevel.h,v $
 *------------------------------------------------------------------
 * patchlevel.h - This file does nothing except define a
 *                "patch level" for Tcl.
 * April 1994, Brad Benson
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: patchlevel.h,v $
 * Revision 3.2  1995/11/17  18:46:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:03:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * patchlevel.h --
 *
 * This file does nothing except define a "patch level" for Tcl.
 * The patch level is an integer that increments with each new
 * release or patch release.  It's used to make sure that Tcl
 * patches are applied in the correct order and only to appropriate
 * sources.
 */

#define TCL_PATCH_LEVEL 104
