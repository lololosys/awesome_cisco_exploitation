/* $Id: monlib.h,v 3.1 1995/11/09 09:10:23 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/monlib.h,v $
 *------------------------------------------------------------------
 * Header file for monlib
 *
 * December 1994, Szewei Ju
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: monlib.h,v $
 * Revision 3.1  1995/11/09  09:10:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:15  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:36  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:41  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * redefine SIG_PF here so we don't have to include ../../sys/os/gdb.h 
 */
typedef void (*SIG_PF)(void);   

#include "../../sys/rommon/src/monlib.h"

