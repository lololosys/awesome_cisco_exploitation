/* $Id: nim.h,v 3.2 1995/11/17 18:45:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-4k-c4000/nim.h,v $
 *------------------------------------------------------------------
 * nim.h
 *
 * August 1993, Yvette Yuen
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the NIM polling routines.
 *------------------------------------------------------------------
 * $Log: nim.h,v $
 * Revision 3.2  1995/11/17  18:45:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:08:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:40:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Ported from DosXX support file.
**
** List all the possible basic module types and type code for Sierra.
** Define module information structure.
*/
 
#define MAXSLOTS    0x03       /* Sierra supports up to three NIMs */ 
#define SLOT_OFFSET 0x0100000  /* Address space between slots */
#define VACCODE     0xff       /* slot is vacant */

struct slotdata {
    unsigned short slotnum;    /* slot number */
    unsigned short id;         /* module id   */ 
    unsigned short *base;      /* base address of module */
    int (*nimfunc)();          /* the function invoked for this nim*/
    int *nimfparam;            /* optional function parameter */
    char *type;                /* module type string */
};

