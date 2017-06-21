/* $Id: ccb1tr6.h,v 3.2 1995/11/17 17:39:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/ccb1tr6.h,v $
 *------------------------------------------------------------------
 * ccb1tr6.h  ISDN 1 TR6 BRI Call Control include file
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ccb1tr6.h,v $
 * Revision 3.2  1995/11/17  17:39:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE: ccb1tr6.h
DESCRIPTION: ISDN 1 TR6 BRI Call Control include file
VERSION: 3.21

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************
****history**************************************************************
*************************************************************************

GENERAL NOTES

*************************************************************************
*************************************************************************
*/

#ifndef     CCB1TR6_H                     /* prevent nested inclusions */
#define     CCB1TR6_H                                   /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* 1 TR6 Cause Values 
*/
#define INVALID_CALL_REFERENCE_VALUE_1TR6       0x01
#define BEARER_SERVICE_NOT_IMPLEMENTED_1TR6 	   0x03
#define CALL_IDENTITY_DOES_NOT_EXIST_1TR6       0x07 
#define NO_CHANNEL_AVAILABLE_1TR6               0x0A
#define NO_CIRCUIT_AVAILABLE_1TR6               0x0C 

/* 1 TR6 Network Specific Facilities 
*/
#define ANZEIGE_UMGELEITETER_RUF                0x1A
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
**************************************************************************
*/

/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/************************************************************************
*                F U N C T I O N   D E F I N I T I O N S                *
*************************************************************************
*/


#endif

