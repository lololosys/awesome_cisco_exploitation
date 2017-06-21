/* $Id: main_pt.c,v 3.5.48.1 1996/03/18 21:30:20 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/os/main_pt.c,v $
 *------------------------------------------------------------------
 * main_pt.c -- Protocol Translator
 *
 * 8-January-1989, Kirk Lougheed
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: main_pt.c,v $
 * Revision 3.5.48.1  1996/03/18  21:30:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.16.2  1996/03/14  01:21:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.5.16.1  1996/02/20  16:40:30  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1995/12/10  21:02:23  smackie
 * Next on a very special Geraldo. Unused global booleans and the files
 * that loved them. (CSCdi45340)
 *
 * Revision 3.4  1995/12/03  17:58:19  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/22  05:43:15  eschaffe
 * CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
 * option.
 * Obsolete PROTOCONVERT flag in makefiles.  The control for defining
 * protocolconversion for an image is now platform_specific.
 *
 * Revision 3.2  1995/11/17  18:49:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:56:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../dev/monitor1.h"
#include "interface_private.h"
#include "../os/gdb.h"
#include "boxtypes.h"
#include "exception.h"


/*
 * Fundamental configuration variables
 */
/*************************************************************/
/* WARNING! Do not add any initialized variables above       */
/* data_start in this file. The code below relies on the     */
/* fact that data_start is the first initialized variable    */
/* in the system image!                                      */
/*************************************************************/
int data_start = 0;		/* Start of initialized data */


boolean romcode = ROM_ENABLE;		/* running out of RAM */
boolean router_enable = FALSE;		/* cannot route packets */
boolean x25_enable = TRUE;		/* commercial X.25 */
boolean bridge_enable = FALSE;		/* bridging */
boolean srswitch_enable = SRSWITCH_ENABLE; /* srswitch */
boolean lat_enable = LAT_ENABLE;	/* LAT protocol */
boolean bootstrap_enable = FALSE;	/* we are not a bootstrap */
int boxtype = BOXTYPE_PT;
int ciscopro_image = CISCOPRO_IMAGE;	/* image is runnable on a
					   CiscoPro router */

/*
 * _start
 * For M68k based platforms, the world begins here and this must be 
 * the first code in the text segment.
 * For R4k based platforms, some postion independant code executes to setup
 * the correct virtual address space, and then it calls _start .
 */

#define	BOOTLOAD	2	/* run code */
#define	VERSION		3	/* get code version */
#define	DEBUG		4	/* Run code in debug mode */
#define	SETUP_DEBUG	5	/* Just set up debugger and return */

boolean _start (
    int code,
    boolean arg)
{
    long int ptrdiff;

    /*
     * Copy the initialized data section from ROM to RAM:
     * For images linked to run from DRAM,
     *  for 68K platfroms, data_start will follow etext within an
     *  alignment fudge factor not more than a double long.
     *  for the MIPS platforms, _fdata will follow etext within
     *  8K bytes.
     * So we'll play safe and assume the data region will start
     * within 32K bytes for run-from-RAM images.
     * For Run-from-ROM or run-from-FLASH images, data_start will be
     * in DRAM and will not be anywhere near etext (atleast 16M away).
     * This is the case where we want to move the data section.
     */
    ptrdiff = (long)DATA_START - (long)TEXT_END;
    if ((ptrdiff < 0) || (ptrdiff > (1024*32))) {
        /* Not running from DRAM. Move data section. */
        bcopy (TEXT_END, DATA_START, ((long)DATA_END - (long)DATA_START));
    }

    switch(code) {
      case BOOTLOAD:
      case DEBUG:
      case SETUP_DEBUG:
	init_exception_handler();	/* Initialize exception handler */
	if (code != BOOTLOAD)		/* Enable kernel debugging */
	  gdb_kernel_enable();

	breakpoint();			/* Take an initial breakpoint */

	if (code == SETUP_DEBUG)	/* Get out if we're supposed to */
	  break;

	return(main(arg));		/* Start the system */

      case VERSION:
	return(SYS_MAJORVERSION*256+SYS_MINORVERSION);
      default:
	return(FALSE);
    }
    return(TRUE);
}
