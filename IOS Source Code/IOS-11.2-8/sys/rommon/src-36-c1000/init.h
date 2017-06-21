/* $Id: init.h,v 3.2 1995/11/17 18:44:02 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-36-c1000/init.h,v $
 *------------------------------------------------------------------
 * Header file for init.c
 *
 * April 1995, Eric Fronberg
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init.h,v $
 * Revision 3.2  1995/11/17  18:44:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:39:15  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef struct quicc_pin_init {
    /*
     * Parallel Port A pins
     */
    volatile ushort	padir;	/* Port A Data Direction Register */
    volatile ushort	papar;	/* Port A Pin Assignment Register */
    volatile ushort	paodr;	/* Port A Open Drain Register */
    volatile ushort	padat;	/* Port A Data Register */

    /* now for pip pins */
    volatile ulong      pbdir;	/* Port B Data Direction Register */
    volatile ulong      pbpar;	/* Port B Pin Assignment Register */
    volatile ushort     pbodr;	/* Port B Open Drain Register */
    volatile ulong      pbdat;	/* Port B Data Register */


    /* port c pins */

    volatile ushort	pcdir;	/* Port C Data Direction Register */
    volatile ushort	pcpar;	/* Port C Pin Assignment Register */
    volatile ushort	pcso;	/* Port C Special Options */
    volatile ushort	pcdat;	/* Port C Data Register */
} quicc_pin_init_t;

#define CONSOLE_PIN_INIT	0 /* index into pin init array */
#define SAPPHIRE_HARDWARE	1
#define CATWOMAN_HARDWARE	2 
