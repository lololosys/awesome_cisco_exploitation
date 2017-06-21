/* $Id: cspgrace.h,v 3.2 1995/11/17 18:08:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/cspgrace.h,v $
 *------------------------------------------------------------------
 *
 *
 * Mar 1994, Walter Friedrich
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cspgrace.h,v $
 * Revision 3.2  1995/11/17  18:08:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:37  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_cspgrace.h xm7_v3.1.1:cset.0166:7:7 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Connection Services Process Header File
		
		CSP XLNT Entry(tm) Placeholder Header File
		
		File:			xm7_cspgrace.h
		Created:		10/08/92

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This header file contains blank definitions for macros used
		by the XLNT Entry(tm) software. This software provides
		scheduled and graceful insertion functions as well as local
		MAC management. Implementors of FDDI concentrators interested
		in these capabilities should contact XLNT Designs, Inc. for
		more information about the XLNT Entry product.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/


#define MCANCEL_SCHED_INS(stnID, portPtr)
#define MSCHEDULED_INS(stnID,portPtr,reqPaths)
#define MGRACEFUL_INS(stnID,portPtr,reqPaths)
#define MSCHED_TIMEOUT_INSERT(stnID,portPtr)
#define MINIT_SCHED_INS(stnID)						0
 /* XLNT Entry support */
#define MINIT_GI_INS(stnID)							0

 /* XLNT Entry support */
#define MCANCEL_GRACE_INS(stnID, portPtr)
#define MSIGNAL_LMM(stnID, sigType, sigEntity, sigData)

/* XLNT Entry support */
#define MCONFIGURE_M_PORT(stnID, portPtr, sigType, sigData)\
			ConfigureMasterMulti (stnID, portPtr, portPtr->cmState);\
			if (portPtr->isConnecting && portPtr->CF_Join)\
				SendSignalMulti (stnID, SIG_EC_PCM_RETRY,\
					portPtr->ID, (uInt32) 0);
