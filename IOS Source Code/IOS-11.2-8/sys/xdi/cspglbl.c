/* $Id: cspglbl.c,v 3.2 1995/11/17 18:08:50 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/cspglbl.c,v $
 *------------------------------------------------------------------
 * XDI source file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cspglbl.c,v $
 * Revision 3.2  1995/11/17  18:08:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:54:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:23:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifdef USE_WHAT_STRING
static char      xdi_id[] = "@(#) xm7_cspglbl.c xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994 Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED";
#endif
/*********************************************************************
		Connection Services Process Module
		
		Global Variables Module
		
		File:			xm7_cspglbl.c
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This module defines the global variables used throughout the
		CSP.

		Modification History:
        *** SMT 7.3 ***
*********************************************************************/

#include        "xdihdrsif.h"

#include	"smterror.h"
#include	"fddihdr.h"
#include	"csphdr.h"

#ifdef MULTIPLE_STATION
/* Array holding all CSP structures for each station. */
struct CSPAllInfo CSPInfo[MAX_STATION_COUNT];

#else							/* MULTIPLE_STATION */

/* Station Data */
CSPStationInfoType stationData;

/* MAC Data */
CSPMACInfoType   macData[MAX_MAC_COUNT];

/*
 * PORT Data One extra entry is used as a place holder for non-existant
 * attachment ports. Some code assumes that when there is an A port, there
 * will a B port, and vice versa. In flexible configurations, this may not be
 * the case. The extra element is a dummy to provide some place for accessing
 * information.
 */
CSPPORTInfoType  portData[CSP_PORT_COUNT];

/* PATH Data */
CSPPATHInfoType  pathData[MAX_PATH_COUNT];

#endif							/* MULTIPLE_STATION */

/*
* The SMT-defined addresses are global variables because all
* compilers do not handle string constants in the same way.
* Making the addresses constants gives them a type definition
* and provides a portable method for initializing the address
* values.
*/
FDDILongAddressType cspBroadcastAddr = SMT_Broadcast_INIT;
FDDILongAddressType cspNullAddr = SMT_NULL_INIT;
FDDILongAddressType cspUnknownAddr = SMT_Unknown_INIT;
FDDILongAddressType cspDirBcnDA = SMT_Directed_Beacon_INIT;
FDDILongAddressType cspSrfDA = SMT_SRF_INIT;
FDDILongAddressType cspSbaDA = SMT_SBA_INIT;
