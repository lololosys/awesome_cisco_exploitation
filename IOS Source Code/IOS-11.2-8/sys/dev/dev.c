/* $Id: dev.c,v 3.2.60.5 1996/09/07 02:07:25 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/dev.c,v $
 *------------------------------------------------------------------
 * dev.c -- Generic device driver routines
 *
 * 27-June-1991, Darren Kerr
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dev.c,v $
 * Revision 3.2.60.5  1996/09/07  02:07:25  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/08/02  22:52:44  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/05/31  21:20:14  hampton
 * Back out accidental changes to this file.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/30  23:43:07  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/05/17  10:44:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/05/02  22:01:25  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:59:04  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/11/17  08:59:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:18:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:23:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "dev.h"
#include "ttysrv.h"
#include "stacks.h"

boolean devBlockOnOpen(flash_dev_t *);
#ifndef RP1 /* XXXMERGEFIX */
extern devfun_t mci_flash_fun;
extern devfun_t env_flash_fun;
#endif

int dev_null_int(void)
{
    return -1;
}

void dev_null_void(void)
{
    return;
}

devfun_t dev_null_fun =
{
    (int (*)(void))dev_null_int,
    (int (*)(int))dev_null_int,
    (int (*)(void))dev_null_int,
    (void (*)(void))dev_null_void,
    (int (*)(ulong, int, void *))dev_null_int,
    (int (*)(ulong, char *, int))dev_null_int,
    (int (*)(ulong, char *, int))dev_null_int,
};

/* Global device table */
flash_dev_t devtable[DEV_CNT] = {
    /*     Name   Cntrlr  State Ocnt Flags  Vector  Tty  Functions */
    {"null-dev",  "NULL",     0,   0,    0,     0,   -1, &dev_null_fun},  /* DEV_NUL */
    {"env-flash", "ENVM",     0,   0,    0, LEVEL1,  -1, &env_flash_fun}, /* DEV_ENV */
    {"mci-flash",  "MCI",     0,   0,    0,     0,   -1, &mci_flash_fun}, /* DEV_MCI */
};

/*
 * dev_init
 *
 * Call initialize routine for a device.
 */
int dev_init(int dev)
{
    flash_dev_t *dp;

    if (dev > DEV_MAX && dev < 0)
	return DEVERR_TBLOVER;
    dp = &devtable[dev];
    if (dp->dev_ocnt)
	return DEVERR_STILL_OPEN;
    if (dp->dev_fun->df_intr) {
	if (dp->dev_level > LEVEL_MAX)
	    return DEVERR_LEVEL;
	if (dp->dev_level > 0)
	    /* This guy really should return status... */
	    createlevel(dp->dev_level, dp->dev_fun->df_intr, dp->dev_name);
    }
    if (!dp->dev_fun->df_init)
	return DEVERR_NOINIT;
    if ((*dp->dev_fun->df_init)() != DEV_OK)
	return DEVERR_INIT;
    dp->dev_state = DEV_STATE_INIT;
    return DEV_OK;
}

/*
 * Return an actual index into the dev structure.
 * Because there is no user space or user processes,
 * this doesn't return an indirect reference (as in
 * some other operating systems).
 * Maybe in the future, the file handle could be referenced
 * through the proc struct.
 */

int dev_open (ulong dev, int flags)
{
    flash_dev_t *dp;
    int   sts;
    int   level;
    
    if (dev > DEV_MAX)
	return DEVERR_NODEV;
    dp = &devtable[dev];
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
	return sts;
    }
    dp->dev_flags = flags;
    return dp - &devtable[0];  /* index of device */
}

/*
 * External entrypoint to generic drivers' close routines.
 */
int dev_close (ulong ind)
{
    int   level;

    level = raise_interrupt_level(ALL_DISABLE);   /* disable interrupts */
    if (ind < DEV_CNT && devtable[ind].dev_ocnt) {
	(*devtable[ind].dev_fun->df_close)();
	devtable[ind].dev_ocnt--;
	reset_interrupt_level(level); /* re-enable interrupts */
	return 0;
    }
    reset_interrupt_level(level); /* re-enable interrupts */
    return -1;
}

/*
 * External entrypoint to generic drivers' ioctl routines.
 */
int dev_ioctl (ulong dev, int cmd, void *buf)
{
    if (dev < DEV_CNT && devtable[dev].dev_ocnt)
	return (*devtable[dev].dev_fun->df_ioctl)(dev,cmd,buf);
    return -1;
}

/*
 * External entrypoint to generic drivers' read routines.
 */
int dev_read (ulong dev, char *buf, int len)
{
    if (dev < DEV_CNT && devtable[dev].dev_ocnt)
	return (*devtable[dev].dev_fun->df_read)(dev,buf,len);
    return -1;
}

/*
 * External entrypoint to generic drivers' write routines.
 */
int dev_write (ulong dev, char *buf, int len)
{
    if (dev < DEV_CNT && devtable[dev].dev_ocnt)
	return (*devtable[dev].dev_fun->df_write)(dev,buf,len);
    return -1;
}

boolean devBlockOnOpen(flash_dev_t *dp)
{
    if (dp->dev_ocnt == 0) {
	dp->dev_ocnt++;
	return FALSE; /* wakeup */
    }
    return TRUE; /* keep sleeping */
}

#ifdef DEV_NAME_STRING
/*
 * Find already allocated device, or next unallocated
 * device from the global device table.
 */
flash_dev_t *dev_next(char *devname)
{
    flash_dev_t *dp, *dev_find();
    
    if ((dp = dev_find(devname)) != NULL)
	return dp;
    if (devTabIndex < DEV_MAXDRIVERS)
	return &devtable[devTabIndex];
    return NULL;
}

/*
 * Return pointer to device whose name matches passed param.
 */
flash_dev_t *dev_find(char *devname)
{
    register int i;
    
    for (i=0; i<devTabIndex; i++)
	if (devtable[i].dev_state &&
	    strcmp(devname, devtable[i].dev_name) == 0)
	    return &devtable[i];
    return NULL;
}

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
