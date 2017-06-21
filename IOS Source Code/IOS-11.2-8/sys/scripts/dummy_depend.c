/* $Id: dummy_depend.c,v 3.1.2.1 1996/07/14 07:13:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/scripts/dummy_depend.c,v $
 *------------------------------------------------------------------
 * Dummy file used to find platform mandated include files.
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dummy_depend.c,v $
 * Revision 3.1.2.1  1996/07/14  07:13:37  hampton
 * Eliminate all redundant dependencies for any given file.  Split out
 * platform specific dependencies and put them in the appropriate obj-xxx
 * directory.  [CSCdi62810]
 * Branch: California_branch
 *
 * Revision 3.1  1996/07/12  21:22:01  hampton
 * New file to flatten paths in dependency lists, and to strip duplicate
 * dependencies.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __GNUC__
#define	volatile
#endif

#ifdef lint
#define inline
#endif

/*
 * Target platform definitions
 */

#ifdef TARGET_CISCO
#include "target_cisco.h"
#endif

#ifdef TARGET_UNIX
#include "target_unix.h"
#endif
