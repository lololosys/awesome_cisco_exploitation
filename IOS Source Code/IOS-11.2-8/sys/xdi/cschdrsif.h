/* $Id: cschdrsif.h,v 3.4.44.2 1996/03/22 00:15:52 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/cschdrsif.h,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cschdrsif.h,v $
 * Revision 3.4.44.2  1996/03/22  00:15:52  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.4.44.1  1996/03/18  22:51:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.2  1996/01/16  04:54:05  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/23  04:42:19  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.4  1996/01/05  04:57:27  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/11/17  18:08:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:52:39  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:54:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*********************************************************************
	Cisco environment (Platform, Processor) Header File
	
	File:			xdihdrsif.h
	Created:		12/03/93

	Version:		
	Last Modified:
	
	This header file includes the XDI header files which are
	platform and processor dependent.

	Modification History:
*********************************************************************/

#ifndef	__CSCHDRSIF_H__
#define	__CSCHDRSIF_H__


/*********************************************************************
	Environment Definitions
*********************************************************************/

#include        "master.h"
#include	<string.h>
#include	<ciscolib.h>
#include        "logger.h"
#include        "ttysrv.h"
#include	"sys_registry.h"
#include	"interface_private.h"
#include        "packet.h"
#include        "../if/fddi.h"
#include	"../if/fddi_private.h"
/*
*	Platform: XX (C4000)
*	Processor: 68k
*/
#ifdef	XX
#include	"../les/if_les.h"
#include	"../../boot/src-68-c4000/defs_xx.h"
#include	"../src-68-c4000/xx_fddi_public.h"
#include        "../src-c4000/c4000_bsi.h"
#endif

/*
*	Platform: Sierra (C4000)
*	Processor: R4000
*/
#ifdef	SIERRA
#include	"../les/if_les.h"
#include	"../src-4k-c4000/sierra_fddi_public.h"
#include        "../src-c4000/c4000_bsi.h"
#endif

/*
*	Platform: AGS+
*	Processor: 68k
*/
#ifdef	CSC2
#include	"../../boot/src-68-ags/defs_csc2.h"
#include	"../hes/if_mci.h"
#include	"../hes/hes_fddi_public.h"
#endif

/*
*	Platform: 7000 (C7000)
*	Processor: 68k
*/
#ifdef	C7000
#include	"../../boot/src-68-c7000/defs_rp1.h"
#include	"../hes/if_mci.h"
#include	"../hes/hes_fddi_public.h"
#endif

/*
*	Platform: 7500 (RSP)
*	Processor: R4000
*/
#ifdef	RSP
#include	"../hes/hes_fddi_public.h"
#endif


#endif
