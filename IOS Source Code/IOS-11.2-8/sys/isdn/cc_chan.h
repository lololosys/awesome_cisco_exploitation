/* $Id: cc_chan.h,v 3.2.60.1 1996/04/30 23:06:36 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/cc_chan.h,v $
 *------------------------------------------------------------------
 * cc_chan.h  Layer 3 Call Control Channel Management include file.
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cc_chan.h,v $
 * Revision 3.2.60.1  1996/04/30  23:06:36  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:38:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/16  17:54:40  dclare
 * CSCdi40762:  PRI connected to DMS100 incorrectly puts B channels out of
 * service. A Restart and a shutdown of the interface will put channels
 * back into in-service.
 *
 * Revision 2.1  1995/06/07  21:12:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       cc_chan.h                                               
DESCRIPTION:  Layer 3 Call Control Channel Management include file.   
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

This include file accompanies cc_chan.c which defines and accesses the
channel management table. Note that some of the defines will be similar,
but will with a "P" or a "B". The P, means that it is for PRI and the
B, means that it is for BRI interfaces. 


*************************************************************************
*************************************************************************
*/

#ifndef     CC_CHAN_H                     /* prevent nested inclusions */
#define     CC_CHAN_H                                   /* module name */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/* B-Channel call-related states.
*/
#define CHANNEL_IDLE          0             /* Channel available state */
#define CHANNEL_PROPOSED      1            /* Channel being negotiated */
#define CHANNEL_BUSY          2           /* Channel unavailable state */
#define CHANNEL_RESERVED      3           /* Channel has been reserved */
#define CHANNEL_RESTART_PEND  4  /* Channel restart has been initiated */
#define CHANNEL_MAINT_PEND    5 /* Local channel maint. change pending */
#ifdef BRI_1TR6
#define CHANNEL_PROPOSED_1TR6 6   /* Net may resend SETUP, change Chan */ 
#endif 

/* B-Channel service states.
*/
#define IN_SERVICE            0                /* B-Channel in service */
#define MAINTENANCE           1       /* B-Channel in maintenance busy */
#define OUT_OF_SERVICE        2            /* B-Channel out of service */

#define MIN_CHANNEL           1             /* Lowest B-channel number */
#define MAX_INT_ID_LEN        1 /* Maximum allowed interface ID length */

#define MAX_CHANNELB           2      /* Maximum B-channel number - BRI */
#define MIN_INT_IDB            0     /* Interface ID, not a BRI element */
#define MAX_INT_IDB            0     /* Interface ID, not a BRI element */

/* CUSTOMIZE HERE:
*  Note that the interface ID is defined in AT&T 41449 to legally assume
*  the range 0 - 31.  It defaults here at a lesser value to conserve 
*  channel table memory.  MAX_INT_ID may be resized up to 31 inclusive as
*  required.
*/
#define MAX_CHANNELP          32      /* Maximum B-channel number - PRI */
/*
 * These values are going to have to be rechecked when multiple interfaces
 * get supported with a single D-channel. Note the use of VALID_INT_IDP
 * with these values. ===
 */
#define MIN_INT_IDP            0                        /* Interface ID */
#define MAX_INT_IDP            0                        /* Interface ID */

#define ANY_INTERFACE      0xFF           /* Refer to any interface ID */

#define CHANS_PER_B           1                         /* 1 B-channel */
#define CHANS_PER_H0          6                        /* 6 B-channels */
#define CHANS_PER_H11        24                       /* 24 B-channels */
#define NOT_A_HYPERCHANNEL   -1  /* Possible result for CC_CHAN_GetXXX */
#define NOT_TIMING           -1              /* Default Timer ID value */

/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/************************************************************************
*                              M A C R O S                              *
*************************************************************************
*/
#define VALID_CHANNELB(c)   (((c)>=MIN_CHANNEL) && ((c)<=MAX_CHANNELB))
#define VALID_CHANNELP(c)   (((c)>=MIN_CHANNEL) && ((c)<=MAX_CHANNELP))

/* Test the channel ID interface ID for its range.
*  i = interface ID, t = channel type (type 8 = H11_CHANNEL_UNITS).
*/

/*
 * define VALID_INT_IDP(i)  (((i)>=MIN_INT_IDP) && ((i)<=MAX_INT_IDP))
 *
 * this may have to be added back in when a single D-channel supports
 * multiple interfaces. Used in conjunction with MIN_INT_IDP and
 * MAX_INT_IDP. ===
 */
#define VALID_INT_IDP(i)  ((i)==0)
#define VALID_INT_IDB(i)  ((i)==0)

/* Test the channel ID channel type for its value.
*/
/* Return the number of "extra" channels making up a channel group for
*  a specific channel type (t), or ERR for an unrecognized type.
*/
#define GET_CHAN_SPANP(t) ((t == B_CHANNEL_UNITS) ? CHANS_PER_B :\
                          (t == H0_CHANNEL_UNITS) ? CHANS_PER_H0  :\
                          (t == H11_CHANNEL_UNITS) ? CHANS_PER_H11 : ERR) 
#define GET_CHAN_SPANB(t) (((t)==B_CHANNEL_UNITS) ? 0 : ERR) 

#define LEGAL_CHAN_TYPEP(t) (GET_CHAN_SPANP((t))!=ERR)
#define LEGAL_CHAN_TYPEB(t) (GET_CHAN_SPANB((t))!=ERR)

/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/
/************************************************************************
*                F U N C T I O N   D E C L A R A T I O N S              *
*************************************************************************
*/

PUBLIC void    CC_CHAN_Set_Chan_State  (int, int, BOOLEAN, int);
PUBLIC void    CC_CHAN_Show_Status     (int);
PUBLIC void    CC_CHAN_set_status_inserv  (int);
PUBLIC int     CC_CHAN_GetIdleChanpri  (BYTE, int, int, int, int);
PUBLIC int     CC_CHAN_GetIdleDMSChan  (BYTE, int, int, int);
PUBLIC int     CC_CHAN_GetProtocolpri  (BYTE, int, int, int);
PUBLIC int     CC_CHAN_GetServState    (BYTE, int, int, int);
PUBLIC int     CC_CHAN_GetStatepri     (BYTE, int, int, int);
PUBLIC int     CC_CHAN_GetTimerID      (BYTE, int, int);
PUBLIC int     CC_CHAN_GetUserpri      (BYTE, int, int, int);
PUBLIC BOOLEAN CC_CHAN_InitTablepri    (int, int, int, int, int, int);
PUBLIC void    CC_CHAN_ReleaseChanpri  (BYTE, int, int, int);
PUBLIC void    CC_CHAN_SetProtocolpri  (BYTE, int, int, int, int);
PUBLIC void    CC_CHAN_SetServState    (BYTE, int, int, int, int);
PUBLIC void    CC_CHAN_SetStatepri     (BYTE, int, int, int, int);
PUBLIC void    CC_CHAN_SetTimerID      (BYTE, int, int, int);
PUBLIC void    CC_CHAN_SetUserpri      (BYTE, int, int, int, int);


PUBLIC int     CC_CHAN_GetIdleChanbri  (int, int, int, int);
PUBLIC int     CC_CHAN_GetProtocolbri  (int, int, int);
PUBLIC int     CC_CHAN_GetStatebri     (int, int, int);
PUBLIC int     CC_CHAN_GetUserbri      (int, int, int);
PUBLIC BOOLEAN CC_CHAN_InitTablebri    (int, int, int, int, int);
PUBLIC void    CC_CHAN_ReleaseChanbri  (int, int, int);
PUBLIC void    CC_CHAN_SetProtocolbri  (int, int, int, int);
PUBLIC void    CC_CHAN_SetStatebri     (int, int, int, int);
PUBLIC void    CC_CHAN_SetUserbri      (int, int, int, int);

#endif
