/* $Id: ccb5ess.h,v 3.2.60.1 1996/04/30 23:06:43 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccb5ess.h,v $
 *------------------------------------------------------------------
 * ccb5ess.h  ATT's 5ESS Basic Rate Call Control Include File
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ccb5ess.h,v $
 * Revision 3.2.60.1  1996/04/30  23:06:43  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:39:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:13:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccb5ess.h                                               
DESCRIPTION:  ATT's 5ESS Basic Rate Call Control Include File         
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

#ifndef     CCB5ESS_H                       /* prevent nested inclusions */
#define     CCB5ESS_H                                /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
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
#define SPECIFIER_0              0            /* specifier for host cb */

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

PRIVATE     BOOLEAN near CCB5E_DisconnectingHost      (int *);
PRIVATE     int     near CCB5E_CallOffered            (int *);
PRIVATE     BOOLEAN near CCB5E_DisconnectInd          (void);
/* PRIVATE     int     near CCB5E_PassiveHangup          (CCB_ID); */
PRIVATE     void    near CCB5E_RestartChan            (int, BYTE);
PUBLIC      void         CCB5E_ReleaseCall            (void);
#ifdef ICLID
#ifdef TASK_ICLID_DRV
PRIVATE     void    near CCB5E_SendICLID              (BYTE *, BYTE);
#endif
#endif
PRIVATE     int     near CCB5E_SetupInd               (void);
/* PRIVATE     void    near CCB5E_StdVoiceHangup         (int); */
PRIVATE     int     near CCB5E_VoiceConnected         (int *);
PRIVATE     int     near CCB5E_VoiceDialing           (int *);
PRIVATE     int     near CCB5E_VoiceIdle              (int *);
PRIVATE     int     near CCB5E_VoiceReleasing         (int *);
PRIVATE     int     near CCB5E_VoiceRinging           (int *);
PRIVATE     int     near CCB5E_VoiceTone              (int *);


#endif

