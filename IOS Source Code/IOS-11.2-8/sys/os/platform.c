/* $Id: platform.c,v 3.2.62.2 1996/06/17 08:34:50 jhernand Exp $
 * $Source: /release/112/cvs/Xsys/os/platform.c,v $
 *------------------------------------------------------------------
 * platform.c - Platform specific indirected variables
 *
 * December 1994, Scott Mackie
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: platform.c,v $
 * Revision 3.2.62.2  1996/06/17  08:34:50  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2.62.1  1996/03/18  21:30:43  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/09  05:10:04  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.2.26.1  1996/02/20  16:41:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:50:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/15  18:54:22  eschaffe
 * CSCdi40294:  Increase the number of Protocol Translation sessions
 *
 * Revision 2.1  1995/06/07  21:57:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "config.h"
#include "../../boot/cpu.h"
#include "ttysrv.h"

/*
 * Interrupt level variables
 */

const uint all_disable  = PLATFORM_ALL_DISABLE;
const uint tty_disable  = PLATFORM_TTY_DISABLE;
const uint nets_disable = PLATFORM_NETS_DISABLE;
const uint all_enable   = PLATFORM_ALL_ENABLE;

/*
 * System variables
 */

const uint famtype = PLATFORM_FAMTYPE;
const uint maxlines = PLATFORM_MAXLINES;

int nttylines = 0;		/* total number of async terminal lines */
int nconlines = 0;		/* total number of console lines (0 or 1) */
int nauxlines = 0;		/* number of Auxiliary lines */
int ndummyttylines = 0;         /* number of dummy ttylines */

tt_soc *MODEMS[PLATFORM_MAXLINES];	/* pointers to tty data structure */

/* Initialize platform default for line speed */
 
int platformdef_asyncspeed[] = {
    0,
    ASYNC_DEFSPEED,           /* console default speed */
    ASYNC_DEFSPEED,           /* async tty default speed */
    ASYNC_DEFSPEED,           /* vty default speed */
    0,
    ASYNC_DEFSPEED,           /* aux port default speed */            
};
 
/* Initialize platform defaults for stopbits */
 
int platformdef_stopbits[] = {
    0,                   /* bad line default */         
    STOPBITS2,           /* console default stopbits */
    STOPBITS2,           /* async tty default stopbits */
    STOPBITS2,           /* vty default stopbits */
    0,                   /* string line default */
    STOPBITS2,           /* aux port default stopbits */            
};
 
/* Initialize platform line default capabilities */
 
ulong platformdef_capabilities[] = {
    0L,
    0L,           /* console default speed */
    0L,           /* async tty default speed */
    0L,           /* vty default speed */
    0L,
    0L,           /* aux port default speed */            
};
