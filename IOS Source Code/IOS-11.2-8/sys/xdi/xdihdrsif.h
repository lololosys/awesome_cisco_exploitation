/* $Id: xdihdrsif.h,v 3.2.58.1 1996/03/22 00:16:09 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/xdihdrsif.h,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: xdihdrsif.h,v $
 * Revision 3.2.58.1  1996/03/22  00:16:09  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/23  04:42:28  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.2  1995/11/17  19:20:04  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
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

#ifndef	__XDIHDRSIF_H__
#define	__XDIHDRSIF_H__


/*********************************************************************
	Environment Definitions
*********************************************************************/

#define		MULTIPLE_STATION

/*
*	Platform: Sierra (C4000)
*	Processor: R4000
*/
#ifdef	SIERRA
#include	"../src-4k-c4000/sierra_xdi.h"
#include	"../src-c4000/c4000_xdi.h"
#include	"../if/if_xdi.h"

/*
*	Platform: XX (C4000)
*	Processor: 68k
*/
#elif	XX
#include	"../src-68-c4000/xx_xdi.h"
#include	"../src-c4000/c4000_xdi.h"
#include	"../if/if_xdi.h"

/*
*	Platform: 7000 (C7000)
*	Processor: 68k
*/
#elif	C7000

#include	"../src-68-c7000/c7000_xdi.h"
#include	"../hes/hes_xdi.h"
#include	"../if/if_xdi.h"

/*
*	Platform: RSP (75xx)
*	Processor: R4000
*/
#elif	RSP

#include	"../src-rsp/rsp_xdi.h"
#include	"../hes/hes_xdi.h"
#include	"../if/if_xdi.h"

/*
*	Platform: RVIP/SVIP 
*	Processor: R4000
*/
#elif	defined(RVIP) || defined(SVIP)

#include	"../src-vip/vip_xdi.h"
#include	"../pas/mif68840_xdi.h"
#include	"../if/if_xdi.h"

/*
*	Platform: PREDATOR
*	Processor: R4000
*/
#elif	defined(PREDATOR)

#include	"../src-vip/vip_xdi.h"
#include	"../pas/mif68840_xdi.h"
#include	"../if/if_xdi.h"

/*
*	Platform: AGS+
*	Processor: 68k
*/
#elif	CSC2
#include	"../src-68-ags/ags_xdi.h"
#include	"../hes/hes_xdi.h"
#include	"../if/if_xdi.h"

/*
* 	Platform: IGS (C3000) and others without a FDDI
*	Processor: 68k
*/
#elif	PAN
#include	"../src-68-c3000/igs_xdi.h"

#endif

#endif __XDIHDRSIF_H__
