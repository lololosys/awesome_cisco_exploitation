/* $Id: cc.h,v 3.2 1995/11/17 17:38:46 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/isdn/cc.h,v $
 *------------------------------------------------------------------
 * cc.h  ISDN Call Control include file 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cc.h,v $
 * Revision 3.2  1995/11/17  17:38:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:11:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:12:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       cc.h                                                    
DESCRIPTION:  ISDN Call Control include file                          
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


#ifndef     CC_H                          /* prevent nested inclusions */
#define     CC_H                                        /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/

/* Call Control States
*/
#define         IDLE              1
#define         DT_WAIT           2
#define         DIALING           3
#define         RINGBACK          4
#define         COLLECT_DIGITS    5
#define         RINGING           6
#define         CONN_WAIT         7
#define         CONVERSATION      8
#define         IDLE_WAIT         9
#define         OFF_HOOK_IDLE    10

/* Information Element Definitions
*/
#define         SIG            0x34                    /* Signaling IE */
#define         BCAP           0x04            /* Bearer Capability IE */

/* Offsets into Information Field
*/
#define         PROTO_DISC_INDEX  0          /* Protocol Discriminator */
#define         CALL_REF0_INDEX   1           /* Call Reference Length */
#define         CALL_REF1_INDEX   2                  /* Call Reference */
#define         MESSAGE_INDEX     3          /* Message Type IE Offset */
#define         BCAP_INDEX        4        /* Bearer Capability Offset */
#define         CHID_INDEX        8            /* Channel ID IE Offset */
#define         CPNO_INDEX        8   /* Called Party Number IE Offset */
#define         CAUSE_INDEX       4                 /* Cause IE Offset */
#define         KPAD_INDEX        4                /* Keypad IE Offset */
#define         SHOOK_INDEX       4           /* Switch-Hook IE Offset */

/* Miscellaneous constants
*/
#define         CES_VALUE         1               /* 1 Valid CES Value */
#define         BAD_CES         126                     /* Illegal CES */
#define         BAD_MESSAGE     127                 /* Illegal Message */

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/

struct CallControlBlock
   {
   BYTE    State;                               /* network link state */
   BYTE    Substate;           /* substate of the CCB if State = TONE */
   int     B_Chan_Id;                     /* B channel identification */
   int     Call_Id;                            /* call identification */
   BYTE    Call_Ref;      /* call reference - note: 1 byte (BRI) only */
   int     Call_Appearance;                        /* call appearance */
   BYTE    Ces;                         /* connection endpoint suffix */
   BOOLEAN First_Digit;                         /* First dialed digit */
   BYTE    Hcb_Index;                   /* Hcb Ccb_In_Use Array Index */
   struct  CallControlBlock *Next;                      /* next block */
   };
typedef struct CallControlBlock *CCB_ID;

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

