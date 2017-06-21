/* $Id: ccbntkd.h,v 3.2 1995/11/17 17:39:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/ccbntkd.h,v $
 *------------------------------------------------------------------
 * ccbntkd.h  ISDN NTT & KDD Basic Rate Call Control Include File 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ccbntkd.h,v $
 * Revision 3.2  1995/11/17  17:39:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccbntkd.h                                               
DESCRIPTION:  ISDN NTT & KDD Basic Rate Call Control Include File     
VERSION:      3.21                                                    

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

#ifndef     CCBNTKD_H                       /* prevent nested inclusions */
#define     CCBNTKD_H                                /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* Definitions for CCB Call States
*
*
*/
enum CCB_State
   {
   NTKD_STATE_LOW_LIMIT,                              /* Sizing element */
   NTKD_IDLE,                                               /* CCB idle */
   NTKD_RINGING,                                       /* Being alerted */
   NTKD_CONNECTED,                                      /* Conversation */
   NTKD_CONNECTING,                                /* User sent Connect */  
   NTKD_LOCKOUT,                           /* Disconnected but off hook */
   NTKD_OFFHOOK_RELEASING,  /* Waiting for release confirmation on hook */
   NTKD_ONHOOK_RELEASING,  /* Waiting for release confirmation off hook */
   NTKD_OFFERED,                          /* In call offered to Host    */                
   NTKD_ORIGINATE,                    /* Await response to CC_SETUP_REQ */
   NTKD_ROUTINGIN,   /* Engaged but neither dialing nor in conversation */
   NTKD_ROUTINGOUT,  /* Engaged but neither dialing nor in conversation */
   NTKD_DIGIT_COLLECT,  /* SBC digit collection for non overlap dialing */     
   NTKD_DISCONNECTING_HOST, /* Disconnect send to Host, waiting an ACK  */ 
   NTKD_STATE_HIGH_LIMIT                              /* Sizing element */
   };

/* ------------------------------------------------------------------- */
/*                      Miscellaneous Constants                        */
/* ------------------------------------------------------------------- */

/* Various constants.
*/
#define DIAL_KEY              0xEF          /* Request to dial on-hook */
#define EVENT_PENDING            1           /* Internal event pending */
#define VOICE_BEARER_DATA_LEN    3         /* Bearer capability length */
#define NO_ACTIVE_CALLS  MAX_VOICE_CALLS            /* No active calls */
#define NO_STATE_MATCH   MAX_VOICE_CALLS /* No calls of state specified */
#define NO_CALL          MAX_VOICE_CALLS        /* No calls identified */
#define NO_SERVICE_PENDING       0     /* CCB service function default */
#define INTERNAL_RECONNECT       1                   /* Internal event */


/* ------------------------------------------------------------------- */
/*                          Module Error Codes                         */
/* ------------------------------------------------------------------- */

#define INVALID_STATE           -1
#define UNEXPECTED_MSG          -2
#define MISSING_MANDATORY_IE    -3
#define INVALID_INFO_ELEMENT    -4
#define UNACCEPTABLE_CHANNEL    -5
#define UNKNOWN_MSG_SOURCE      -6
#define NO_CALL_SLOT            -7
#define OUT_OF_CCBS             -8
#define BAD_CALLED_ADDR         -9
#define BAD_BEARER_CAP         -10
#define OUT_OF_PKTS            -11
#define MAND_IE_LEN_ERR        -12
#define DIGIT_BUFFER_OVERFLOW  -13

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
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

