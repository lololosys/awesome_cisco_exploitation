/* $Id: g_dev.c,v 3.2.60.3 1996/09/07 02:07:26 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/g_dev.c,v $
 *------------------------------------------------------------------
 * pan_dev.c -- Generic device driver routines
 *
 * 27-June-1991, Darren Kerr
 * 12-Mar-1992, Dit Morse
 **    changed all calls to pass ptr to device block
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: g_dev.c,v $
 * Revision 3.2.60.3  1996/09/07  02:07:26  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/08/02  22:53:00  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  19:12:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:43:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:46:03  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:01:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:25:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "ttysrv.h"
#include "stacks.h"
#include "g_dev.h"

boolean devBlockOnOpen(flash_dev_t *);

/*
 * dev_init
 *
 * Call initialize routine for a device.
 */
int dev_init(flash_dev_t *dp)
{
    if (!dp)
	return DEVERR_TBLOVER;
    if (dp->dev_ocnt)                /* load initialized????? */
	return DEVERR_STILL_OPEN;
    if (dp->dev_fun->df_intr) {
	if (dp->dev_level > LEVEL_MAX)
	    return(DEVERR_LEVEL);
	if (dp->dev_level > 0)
	    /* This guy really should return status... */
	    createlevel(dp->dev_level, dp->dev_fun->df_intr, dp->dev_name);
    }
    if (!dp->dev_fun->df_init)
	return(DEVERR_NOINIT);
    if ((*dp->dev_fun->df_init)() != DEV_OK)
	return(DEVERR_INIT);
    dp->dev_state = DEV_STATE_INIT;
    return(DEV_OK);
}

/*
 * !!flash!! return truth (is open?)
 * Return an actual index into the dev structure.
 * Because there is no user space or user processes,
 * this doesn't return an indirect reference (as in
 * some other operating systems).
 * Maybe in the future, the file handle could be referenced
 * through the proc struct.
 */

int dev_open (flash_dev_t *dp, int flags)
{
    int   sts;
    int   level;
    
    if (!dp)
	return DEVERR_NODEV;

    if (dp->dev_state < DEV_STATE_INIT)
	return DEVERR_INIT;

    if (!dp->dev_fun->df_open)
	return DEVERR_OPEN_STS;
    
    level = raise_interrupt_level(ALL_DISABLE);   /* disable interrupts */
    if (dp->dev_ocnt &&
	((dp->dev_flags & DEV_FLAGS_EXCL) || flags & DEV_FLAGS_EXCL)) {
	reset_interrupt_level(level); /* re-enable interrupts */
	/* OK, kph? */
	if ( (flags & DEV_FLAGS_QUIET) == 0 )
	    printf("\nUser on tty%t is already accessing %s",
		   dp->dev_tty, dp->dev_name);
	if (flags & DEV_FLAGS_NDLY)
	    return DEVERR_EXCL;
	edisms((blockproc *)devBlockOnOpen, (int) dp);
    } else {
	dp->dev_ocnt++;
	dp->dev_tty = stdio->ttynum;
	reset_interrupt_level(level);
    }
    if ((sts = (*dp->dev_fun->df_open)(flags)) != DEV_OK) {
	level = raise_interrupt_level(ALL_DISABLE);
	dp->dev_ocnt--;
	reset_interrupt_level(level);
	return(sts);
    }
    dp->dev_flags = flags;
	return(sts);
}


/*
 * External entrypoint to generic drivers' close routines.
 */
int dev_close (flash_dev_t *dp)
{
    int   level = raise_interrupt_level(ALL_DISABLE);   /* disable interrupts */

    if (dp && dp->dev_ocnt) {
	(*dp->dev_fun->df_close)(dp);
	dp->dev_ocnt--;
	reset_interrupt_level(level);                /* re-enable interrupts */
	return(0);
    }
    reset_interrupt_level(level); /* re-enable interrupts */
    return -1;
}

/*
 * External entrypoint to generic drivers' ioctl routines.
 */
int dev_ioctl (flash_dev_t *dp, int cmd, void *buf)
{
    if (dp && dp->dev_ocnt)
       return((*dp->dev_fun->df_ioctl)(dp, cmd, buf));
    return(-1);

}

/*
 * External entrypoint to generic drivers' read routines.
 */
ulong dev_read (flash_dev_t *dp, uchar *buf, uchar *addr, ulong len)
{
    if (dp && dp->dev_ocnt)
       return((*dp->dev_fun->df_read)(dp, buf, addr, len));
    return(-1);
}

/*
 * External entrypoint to generic drivers' write routines.
 */
ulong dev_write (flash_dev_t *dp, uchar *buf, uchar *addr, ulong len)
{
    if (dp && dp->dev_ocnt)
	return((*dp->dev_fun->df_write)(dp, buf, addr, len));
    return(-1);
}

boolean devBlockOnOpen(flash_dev_t *dp)
{
    if (dp->dev_ocnt == 0) {
	dp->dev_ocnt++;
	return(FALSE); /* wakeup */
    }
    return(TRUE); /* keep sleeping */
}

#ifdef DEV_NAME_STRING
/*
 * Various error messages printed from dev_install & dev_open.
 * NOTE: these are indexed through error codes in dev.h
 */
char *const dev_errmsg[] = {
    "install OK",                 /* DEV_OK                         */
    "devtable overflow",          /* DEV_TBLOVER                    */
    "no initialize routine",      /* DEVERR_NOINIT                  */
    "initialize error",           /* DEVERR_INIT                    */
    "bad name",                   /* DEVERR_BADNAME                 */
    "dev still open",             /* DEVERR_STILL_OPEN              */
    "device not found",           /* DEVERR_NODEV                   */
    "already exclusively opened", /* DEVERR_EXCL                    */
    "unknown open status",        /* DEVERR_OPEN_STS                */
    "failed alloc",               /* DEVERR_ALLOC                   */
    "bad intr level",             /* DEVERR_LEVEL                   */
    "controller error",           /* DEVERR_CTLR                    */
    "unknown",                    /* DEVERR_LAST                    */
};
#define DEVERR_LAST (sizeof(dev_errmsg) / sizeof(char *))

#endif /* DEV_NAME_STRING */









