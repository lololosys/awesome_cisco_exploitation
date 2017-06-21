/* $Id: rommon_fslib_iface.c,v 3.2 1995/11/17 09:08:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/rommon_fslib_iface.c,v $
 *------------------------------------------------------------------
 * Interface translator for the ROM MONITOR to call the FSLIB
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rommon_fslib_iface.c,v $
 * Revision 3.2  1995/11/17  09:08:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:57  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "types.h"
#include "../src-4k/signal_4k.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib.h"
#include "fslib_internals.h"
#include "fslib_print.h"
#include "../rommon/src/mon_boot.h"
#include "../rommon/src/monlib.h"

int (*printf_ptr)();


/******************************************************************************
 * Name: open_translator
 *
 * Description: changes the parameters from the Unix open() to what
 *   the ROM MONITOR expects
 *
 * Input: 
 *   file_name -- a file name of the file (device ID and file name)
 *     (see the open routine in the FSLIB for more information)
 *         
 *   mode -- the flags parameter -- should always be O_RDONLY
 *  
 *   devnum -- the device number of the device to be opened.
 *     (not used)
 *
 *   offset -- the offset into the device at which to start
 *     (not used)
 * 
 *   size -- the size (in bytes) of the device
 *     (not used)
 *
 * Return Value:
 *   fd -- file descriptor for the file that was opened.
 *     (see the open routine in the FSLIB for more information)
 *
 *******************************************************************************/
static int open_translator (char *file_id, ulong mode, int devnum, int offset, long size)
{
    int fd;

    fd = FSLIB_open(file_id,mode,0);
    return(fd);
}

#ifdef notdef
/*
 * The following functions are no longer needed because 
 * ralib is moved into fslib
 */

/******************************************************************************
 * Name: ralib_device_first_ok_dummy
 *
 * Description: This function just returns RALIB_DEVICE_FIRST_OK
 *
 * Input: Any
 *
 * Return Value:
 *   Always returns the error code FSLIB_DEVICE_FIRST_OK
 *
 *******************************************************************************/
static int ralib_device_first_ok_dummy (void)
{
    return(RALIB_DEVICE_FIRST_OK);
}


/******************************************************************************
 * Name: ralib_error_dummy
 *
 * Description: This function just returns RALIB_ERROR
 *
 * Input: Any
 *
 * Return Value:
 *   Always returns the error code FSLIB_ERROR
 *
 *******************************************************************************/
static int ralib_error_dummy (void)
{
    return(RALIB_ERROR);
}
#endif /* #ifdef notdef */


/******************************************************************************
 * Name: main
 *
 * Description: main entry point for the FSLIB
 *   This code is executed when FSLIB is loaded.  It does the
 *   FSLIB initilization and returns.
 *
 * Input: 
 *   code -- Ignored
 *   plbp -- pointer to the Process Launch Block
 *   pibp -- pointer to the Process Interface Block
 *
 * Return Value:
 *   boolean -- TRUE = success, FALSE = failure of some kind
 *
 *******************************************************************************/
int main (int code, struct plb *plbp, struct pib *pibp)
{
#ifdef notdef
    static ralib_vector_table_t ralib_vector_table;
#endif /* #ifdef notdef */
    ralib_vector_table_t *ralib;
    fslib_vector_table_t *fslib;
    struct monlib_iface *mlifp;
    struct mon_iface *mifp;

    mifp = pibp->mifp;
    mlifp = mifp->mlifp;
    /*
     * The mon_iface structure contains vectors to all the raw access
     * library (RALIB) functions as well as a pointer to the
     * monlib_iface structure that is where we are to put the file
     * system interface (FSLIB) function vectors.
     */

#ifdef notdef
    /* 
     * Fill out the RALIB vector table based on what is in the
     * mon_iface structure
     */
    ralib = &ralib_vector_table;

    ralib->read         = mifp->ra_read;
    ralib->write        = mifp->ra_write;
    ralib->status       = mifp->ra_status;
    ralib->control      = mifp->ra_control;
    ralib->yield        = mifp->ra_yield;
    ralib->time         = mifp->ra_time;
    ralib->device_init  = (void *)ralib_device_first_ok_dummy;
    ralib->get_dev_num  = mifp->ra_getdevidx;
    ralib->attrib_read  = (void *)ralib_error_dummy;
    ralib->attrib_write = (void *)ralib_error_dummy;

    /*
     * Call the fslib init function to tell the FSLIB where the RALIB 
     * functions are, and get the table of the FSLIB vectors.
     */
    fslib = FSLIB_init(ralib);
#endif /* #ifdef notdef */

    ralib = ralib_init();
    fslib = FSLIB_init(ralib);

    printf_ptr = mifp->printf;

    /*
     * Now copy the necessary vectors from the FSLIB vector table to
     * the appropriate spot in the mon_iface structure
     * XXX jack, figure out how to get the version right
     */
    mlifp->monlib_vers = "Flash File System Version %1.1";
    mlifp->init        = (void *)fslib->init;
    mlifp->open        = open_translator; /* special function to make paramters match */
    mlifp->close       = fslib->close;
    mlifp->read        = fslib->read;
    mlifp->write       = fslib->write;
    mlifp->lseek       = fslib->lseek;
    mlifp->fstat       = fslib->fstat;
    mlifp->getdents    = fslib->getdents;
    mlifp->delete      = fslib->delete;
    mlifp->squeeze     = fslib->squeeze;
    mlifp->istat       = fslib->istat;
    mlifp->fsstat      = fslib->fsstat;
    mlifp->settype     = fslib->settype;
    
    /*
     * set up the global print paramters to some reasonable values for
     * debugging 
     */
    global_print_level = 8;
    global_print_mask  = 0x409f;
    global_print = TRUE;
    
    return(TRUE);
}


/*
 *	Make compiler/linker happy when build monlib
 */
void flush (void)
{
}
