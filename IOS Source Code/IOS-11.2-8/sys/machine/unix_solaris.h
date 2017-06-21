/* $Id: unix_solaris.h,v 3.2 1995/11/17 17:50:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/machine/unix_solaris.h,v $
 *------------------------------------------------------------------
 * unix_solaris.h - Definitions for Solaris images
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_solaris.h,v $
 * Revision 3.2  1995/11/17  17:50:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:40:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:43:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



/*
 * Solaris uses ELF images
 */

#include "image_elf.h"


/*
 * These are only used by unix_bootstrap.c and unix_init.c so they
 * belong here.
 */

#define OS_STRING	"Solaris"
#define PROCESSOR_NAME	"Sparc"




