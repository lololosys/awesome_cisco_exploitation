/* $Id: main_unix.c,v 3.5.46.2 1996/05/10 23:53:10 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/os/main_unix.c,v $
 *------------------------------------------------------------------
 * main_unix.c -- cisco running under UNIX
 *
 * 18-December-1991, Kirk Lougheed
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: main_unix.c,v $
 * Revision 3.5.46.2  1996/05/10  23:53:10  gstovall
 * CSCdi56792:  Need to remove unused files
 * Branch: California_branch
 * Nuke support for the Irix image, also stamp out the last of the CS500.
 *
 * Revision 3.5.46.1  1996/03/18  21:30:22  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.14.2  1996/03/09  05:09:56  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.14.1  1996/02/20  16:40:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1995/12/10  21:02:26  smackie
 * Next on a very special Geraldo. Unused global booleans and the files
 * that loved them. (CSCdi45340)
 *
 * Revision 3.4  1995/12/03  17:58:22  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.3  1995/11/22  05:43:22  eschaffe
 * CSCdi42241:  PROTOCONVERT is TRUE for images that don't include the PT
 * option.
 * Obsolete PROTOCONVERT flag in makefiles.  The control for defining
 * protocolconversion for an image is now platform_specific.
 *
 * Revision 3.2  1995/11/17  18:49:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:46:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/01  06:55:05  hampton
 * Make the unix images compile again.  [CSCdi39708]
 *
 * Revision 2.1  1995/06/07 21:56:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "boxtypes.h"

/*
 * External declarations
 */
extern void unix_exit(int);
extern void setup_signal_handlers(void);

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
boolean router_enable = TRUE;		/* cannot route packets */
boolean x25_enable = X25_ENABLE;	/* commercial X.25 */
boolean bridge_enable = BRIDGE_ENABLE;  /* bridging */
boolean srswitch_enable = SRSWITCH_ENABLE; /* srswitch */
boolean lat_enable = LAT_ENABLE;	/* LAT protocol */
boolean bootstrap_enable = FALSE;	/* we are not a bootstrap */
int boxtype = BOXTYPE_GS;		/* Natively a router */
int ciscopro_image = CISCOPRO_IMAGE;	/* image is runnable on a
					   CiscoPro router */


/*
 * _start
 * The world begins here.
 * This must be the first code in the text segment
 * We are called by UNIX, not a cisco bootstrap, so we don't need to
 * initialize gdb support.  With some minor work we could put in
 * some command line support to allow a netbooting startup.
 */

boolean _start (int code, boolean arg)
{
    setup_signal_handlers();
    main(NULL);		/* start the system at normal entry point*/
    unix_exit(0);
    return(TRUE);
}
