/* $Id: monlib_iface.c,v 3.2.58.1 1996/03/21 23:27:39 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/rommon/src/monlib_iface.c,v $
 *------------------------------------------------------------------
 * monlib_iface.h
 *
 * December 1993, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Support routines and structures for the Monitor's
 * interface to monlib.
 *------------------------------------------------------------------
 * $Log: monlib_iface.c,v $
 * Revision 3.2.58.1  1996/03/21  23:27:39  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:20:43  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:42:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** Support routines and structures for the Monitor's
** interface to monlib.
*/
#include "monitor.h"
#include "signal_4k.h"
#include "getdents.h"
#include "fsio.h"
#include "mon_boot.h"
#include "fstat.h"
#include "dev.h"
#include "simpfsio.h"
#include "monlib.h"
#include "proto.h"
#include "sh.h"
#include "console.h"

/* define the structures in RAM */
struct monlib_iface ml_iface;
struct mon_iface m_iface;

extern char *banner_string;
extern int null_routine();
extern long getfreememstart();
extern int launch(), bzero(), strlen(), strcpy(), strchr(), strcmp();

/*
** The monlib interface structures are initialized to point to
** default routines in EPROM (in this, the Monitor code).
** When a monlib is loaded it alters these structures to
** "connect up" to the Monitor code.
*/
void
init_ml_iface(void)
{
    struct monlib_iface *mlifptr = &ml_iface;
    struct mon_iface *mifptr = &m_iface;

    /* initialize the monitor to monlib interface */
    mlifptr->monlib_vers = "";
    mlifptr->mon_iface = mifptr;
    mlifptr->init  = null_routine;
    mlifptr->open  = sfs_open;
    mlifptr->close = sfs_close;
    mlifptr->read  = sfs_read;
    mlifptr->write = sfs_write;
    mlifptr->lseek = sfs_lseek;
    mlifptr->fstat = sfs_fstat;
    mlifptr->getdents = sfs_getdents;
    mlifptr->delete = null_routine;
    mlifptr->squeeze = null_routine;
    mlifptr->istat = null_routine;
    mlifptr->fsstat = null_routine;
    mlifptr->settype = null_routine;
    mlifptr->reserved = null_routine;
    mlifptr->boot = boot;
    mlifptr->bcopy = bcopy;

    /* and the monlib to monitor interface */
    mifptr->mon_vers = banner_string;
    mifptr->mlifp = mlifptr;
    mifptr->ra_read = raw_read;
    mifptr->ra_write = raw_write;
    mifptr->ra_status = raw_stat;
    mifptr->ra_control = raw_ctl;
    mifptr->ra_yield = raw_yield;
    mifptr->ra_time = raw_time;
    mifptr->open = open;
    mifptr->close = close;
    mifptr->read = read;
    mifptr->write = write;
    mifptr->lseek = lseek;
    mifptr->stat = stat;
    mifptr->fstat = fstat;
    mifptr->getdents = getdents;
    mifptr->getline = getline;
    mifptr->printf = (int (*)())printf;
    mifptr->sprintf = (int (*)())sprintf;
    mifptr->puts = puts;
    mifptr->putchar = (void (*)())putchar;
    mifptr->getchar = getchar;
    mifptr->shcmdline = shcmdline;
    mifptr->getfreememstart = getfreememstart;
    mifptr->launch = launch;
    mifptr->conin = conin;
    mifptr->bzero = bzero;
    mifptr->bcopy = bcopy;
    mifptr->strlen = strlen;
    mifptr->strcpy = strcpy;
    mifptr->getnum = getnum;
    mifptr->strcmp = strcmp;
    mifptr->strchr = strchr;
    mifptr->signal = signal;
    /* initialize our global pointer */
    mifp = mifptr;
}

int
null_routine(void)
{
    return(-1);
}

int
initfs(int argc, char *argv[])
{
    initdevtbl();
    init_ml_iface();  /* init the monlib interface */
    return(0);
}
