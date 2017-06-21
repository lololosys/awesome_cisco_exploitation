/* $Id: debug.h,v 3.1 1995/11/09 09:10:03 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/debug.h,v $
 *------------------------------------------------------------------
 * Support for RP1 specific debugging flags
 *
 * April 1995, Stig Thormodsrud
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: debug.h,v $
 * Revision 3.1  1995/11/09  09:10:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:16:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:23  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:58  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define NUM_DEBUG_FLAGS  3

#define DEBUG_BOOT    "boot"
#define DEBUG_MONLIB  "monlib"
#define DEBUG_ELF     "elf"


extern boolean debug(char *);
extern char    *debug_value(char *);
extern void    debug_set(char *);
extern void    debug_unset(char *);
