/* $Id: ccpmsg.c,v 3.4.18.2 1996/04/30 23:07:08 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/ccpmsg.c,v $
 *------------------------------------------------------------------
 * ccpmsg.c  ISDN Primary Rate Interface Call Control Common Message Routines
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccpmsg.c,v $
 * Revision 3.4.18.2  1996/04/30  23:07:08  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/04/11  21:43:55  dclare
 * CSCdi52751:  Make DMS-100 ISDN PRI changes to conform to the
 * Specification
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/08  03:06:23  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.3  1995/11/28  22:51:53  dclare
 * CSCdi44526:  ISDN PRI NET5 changes for Italian homologation. Changes to
 * handle corner test cases. Restart handling changed to handle interfaces,
 * a channel, and multiple channels.
 *
 * Revision 3.2  1995/11/17  17:40:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/16  17:54:41  dclare
 * CSCdi40762:  PRI connected to DMS100 incorrectly puts B channels out of
 * service. A Restart and a shutdown of the interface will put channels
 * back into in-service.
 *
 * Revision 2.3  1995/09/12  00:11:44  rbeach
 * CSCdi40059:  ISDN PRI running NTT will not accept RESTART from a
 * Fujitsu switch. We should support all valid restart indicator
 * values when running PRI NTT switchtype.
 *
 * Revision 2.2  1995/06/26  18:02:21  dclare
 * CSCdi36415:  Addition of Calling number for Australia ISDN PRI (TS014).
 *
 * Revision 2.1  1995/06/07  21:14:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccpmsg.c
DESCRIPTION:  ISDN Primary Rate Interface Call Control Common Message
              Routines
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

This Call Control module implements common functions in the evaluation
of incoming messages and the generation of outgoing messages to and
from the Host and Layer 3 Protocol Control.  It is an extension of
CCPRI.C and the division of functions between these two modules is
somewhat arbitrary.  The module exists because CCPRI.C would be too
large to be handled by some compilers if all the functios of both
modules were merged into CCPRI.C alone.

ORGANIZATION OF FUNCTIONS

The functions in this module are arranged alphabetically.

*************************************************************************
*************************************************************************
*/
/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* Standard C library functions/definitions
*/
/* """
#include    <stdio.h>
""" */

#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include    "flags.h"
#include    "gendef.h"
#include    "tune.h"
#include    "lif.h"
#include    "gp.h"
#include    "ccbri.h"
#include    "cc_chan.h"
#include    "ccpri.h"
#include    "ccie.h"
#include    "l3.h"

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/
/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/
/************************************************************************
*                            M A C R O S                                *
*************************************************************************
*/
/************************************************************************
*               F U N C T I O N   D E C L A R A T I O N S               *
*************************************************************************
*/
PRIVATE void    near CCPMSG_ErredOptional_IE    (int, int);
PRIVATE void    near CCPMSG_RestartChan         (int, int, BYTE *);
PRIVATE void    near CCPMSG_SendPeerStatus      (BYTE);
PRIVATE void    near CCPMSG_VerifyResendRestart (void);

/************************************************************************
*                    D A T A   A L L O C A T I O N                      *
*************************************************************************
*/
/************************************************************************
*             C O M M O N   M E S S A G E   R O U T I N E S             *
*************************************************************************
*/
/***********************
* CCPMSG_AppendHostUUI *
************************
*
* Recover UUI from the HOST message and transport it, in converted
* format, into the outgoing message to be sent to L3.
*
*/
PUBLIC void CCPMSG_AppendHostUUI (void)
{
   if (( IE = CCPRI_GetPrivate( USER_USER_IE, CODESET_0 )) != NULL )
      {
      OutPkt->InfoLen +=
         CCIE_BuildUser_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                            USER_USER_IE,
                            IE[ 2 ],         /* Protocol descriminator */
                           &IE[ 3 ],                  /* Start of info */
                            IE[ 1 ] - 1 );           /* Length of info */
      }
#ifdef EXTENDED_HOST_MSG
   else
      {
      CCPRI_CopyPublic( USER_USER_IE, CODESET_0 );
      }
#endif

   return;
}

/********************
* CCPMSG_ChanStatus *
*********************
*
* Process a CHAN_STATUS message from the HOST.  This should be a
* CHAN_ENQUIRY, or a RESTART, or a MAINTENANCE SERVICE request.
*
* For the CHAN_ENQUIRY, the HOST requests our current view of the call
* state and maintenance state of a B-channel from our Channel Management
* Table.
*
* For CHAN_RESTART_REQ the HOST requests a single B-channel be restarted.
* Note that if the HOST wishes to restart an H0 channel, it should send
* this message once for each B-channel.
*
* The CHAN_STATUS_IE is constructed as follows:
*
*    0x30 = CHAN_STATUS_IE
*    0x07 = Length (Optionally 06)
*    0xXX = B_Channel
*    0xXX = Action
*    0xXX = Call State
*    0xXX = Service State
*    0xXX = User Call ID (MSB)
*    0xXX = User Call ID (LSB)
*    0xXX = Int ID              ...Present only if MAX_INT_IDP > 0
*
*/
PUBLIC void CCPMSG_ChanStatus()

{
   int new_state;                         /* Channel maintenance state */
   int chan;                                              /* B-channel */
   int int_id;                                         /* Interface ID */


   IE = CCPRI_GetPrivate( CHAN_STATUS_IE, CODESET_0 );
   if (( IE ) && VALID_CHANNELP( IE[ 2 ] ))
      {
      chan = IE[ 2 ];
      int_id = (( MultiInterface && ( IE[ 1 ] > 6 ))) ? IE[ 8 ] : 0;
      if ( IE[ 3 ] == CHAN_ENQUIRY )
         {
         CCPMSG_SendHostChanStat( int_id, chan, CHAN_RESULT );
         }
      else if ( IE[ 3 ] == CHAN_RESTART_REQ )
         {
         switch ( SwitchType )
            {
            case PRI_4ESS_STYPE:
            case PRI_5ESS_STYPE:
            case PRI_DMS100_STYPE:
            case PRI_NET5_STYPE:
            case PRI_NTT_STYPE:
            case PRI_TS014_STYPE:
               CCPMSG_RestartReq( int_id, chan, FALSE );
               break;
            default: /* Function not supported for default SwitchTypes */
               CCPRI_Error( CCPMSG_CHAN_STATUS, P_BAD_DATA_FROM_HOST,
                            CHAN_STATUS_IE, FALSE );
               CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
               break;
            }
         }
      else if ( IE[ 3 ] == CHAN_MAINT_REQ )
         {
         /* This function is only supported for PRI_DMS100, PRI_4ESS
         *  and PRI_5ESS!
         */
         new_state = IE[ 5 ];                /* Read maintenance state */
         if (( new_state == IN_SERVICE )      ||
             ( new_state == MAINTENANCE )     ||
             ( new_state == OUT_OF_SERVICE ))
            {
            switch ( SwitchType )
               {
               /* Should NTT and NET5 be here also, only homologation will tell */
               case PRI_4ESS_STYPE:
                  if ( !CC_SERV_ChangeRequest( int_id, chan, Dsl_Id,
                                               new_state, TRUE,
                                               T3M1_EXPIRY, CCPRI_4ESS_T3M1 ))
                     {
                     /* Report failure.  On success, a message exchange with
                     *  the network will occur.
                     */
                     CCPMSG_SendHostChanStat( int_id, chan, CHAN_MAINTENANCE );
                     }
                  /* Save requested maint state for reference on timeout.
                  */
                  CC_CHAN_SetProtocolpri( int_id, chan,
                                       B_CHANNEL_UNITS, new_state, Dsl_Id );
                  break;
               case PRI_5ESS_STYPE:
                  if ( !CC_SERV_ChangeRequest( int_id, chan, Dsl_Id,
                                               new_state, TRUE,
                                               T3M1_EXPIRY, CCPRI_5ESS_T3M1 ))
                     {
                     /* Report failure.  On success, a message exchange with
                     *  the network will occur.
                     */
                     CCPMSG_SendHostChanStat( int_id, chan, CHAN_MAINTENANCE );
                     }
                  /* Save requested maint state for reference on timeout.
                  */
                  CC_CHAN_SetProtocolpri( int_id, chan, B_CHANNEL_UNITS,
                                       new_state, Dsl_Id );
                  break;
               case PRI_DMS100_STYPE:
                  if ( !CC_SERV_ChangeRequest( int_id, chan, Dsl_Id,
                                               new_state, FALSE, 0, 0 ))
                     {
                     /* Report failure.  On success, a message exchange with
                     *  the network will occur.
                     */
                     CCPMSG_SendHostChanStat( int_id, chan, CHAN_MAINTENANCE );
                     }
                  break;
               default:/* Function not supported for default SwitchTypes */
                  CCPRI_Error( CCPMSG_CHAN_STATUS, P_BAD_DATA_FROM_HOST,
                               CHAN_STATUS_IE, FALSE );
                  CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
                  break;
               }
            }
         else
            {
            CCPRI_Error( CCPMSG_CHAN_STATUS, P_BAD_DATA_FROM_HOST,
                         CHAN_STATUS_IE, FALSE );
            CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
            }
         }
      else
         {
         CCPRI_Error( CCPMSG_CHAN_STATUS, P_BAD_DATA_FROM_HOST,
                      CHAN_STATUS_IE, FALSE );
         CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
         }
      }
   else
      {
      CCPRI_Error( CCPMSG_CHAN_STATUS, P_BAD_DATA_FROM_HOST, 0, FALSE );
      CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
      }

   return;
}

/******************
* CCPMSG_DiscHost *
*******************
*
* Standard actions for the call which received a peer DISCONNECT
* indication (CC_DISCONNECT_IND).  This is a more complex function than
* just sending CALL_DISC to the HOST (CCPMSG_SendHostDisc()) since
* CC_RELEASE_REQ must be initiated towards the ISDN peer here.
*
*/
PUBLIC void CCPMSG_DiscHost( recovered_cause )

int recovered_cause;      /* Cause recovered from the clearing message */

{
   int cause;           /* Cause for non-acceptance of recovered_cause */
   BYTE valid_ie;                    /* for Australian test #T3-10-112 */

   valid_ie = 0;
   /*
    * Have to check for unimplemented IEs in the message.
    */
   valid_ie = CCPCC_VerifyDiscIEs(InPkt->Info, InPkt->InfoLen);
   if (detail_debug)
       buginf ("\n ret from VerifyDisc valid_ie is %x", valid_ie);
   if (valid_ie != NOERR) {
      if (detail_debug)
          buginf ("\n ret from VerifyDisc setting BAD INFO");
      if (valid_ie & 0x80) {
          recovered_cause = BAD_INFO_ELEM;
          cause = BAD_INFO_ELEM;
      } else {
          recovered_cause = MANDATORY_IE_MISSING;
          cause = MANDATORY_IE_MISSING;
      }
   } else {
      /*
       * Figure out what cause values we will propagate.
       */
      if (detail_debug)
          buginf ("\n ret from VerifyDisc setting other");
      if ( !recovered_cause ) {
         /*
          * It may be that we haven't yet recovered the peer cause from the
          * CC_DISCONNECT_IND (L3 DISCONNECT) message.  Do so now.
          */
         cause = CCPRI_AcceptCause( &recovered_cause );
      } else
         cause = ACCEPTED;
   }

   /* We will be sending the recovered_cause to the HOST.  If we don't have
    * anything reasonable to offer, then we'd better dream up something so
    * the HOST doesn't choke.
    */
   if ( cause != ACCEPTED )
      recovered_cause =  NORMAL_CLEARING;

   if (detail_debug)
       buginf ("\n calling send host disc cause is %x", recovered_cause);
   CCPMSG_SendHostDisc( recovered_cause, TRUE );     /* Clear the HOST */

   /* There is a disagreement between various VN3 specs about how to handle
   *  this case.  That is, should RELEASE be sent back to the network
   *  with the error cause, or should it be RELEASE_COMP.  The following
   *  code offers the latter solution.  The former is implemented.
   * if (( SwitchType == VN2_STYPE ) &&
   *     (( cause == MANDATORY_IE_MISSING ) ||      
   *      ( cause == MANDATORY_IE_LEN_ERR )))      
   *    {
   *    CCPMSG_RejectResponse( cause, 0 );
   *    CCPRI_ReleaseCall();
   *    }
   */
   /* Continue clearing the peer.
   */
   if ( CCPRI_GetOutPkt( CC_RELEASE_REQ ))
      {
      /* Build Cause info element.  Here we report the cause for our
      *  displeasure at the Cause info element sent to us in the DISCONNECT
      *  which began the clearing process.
      */
      if ( cause != ACCEPTED )
         {
         Diag[ 0 ] = NO_DIAG;
         OutPkt->InfoLen +=
            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                cause,
                                AM_I_THE_NETWORK_SIDE,
                                Diag );
         }
      CCPRI_Mail( L3_ID, FALSE );
      }
   return;
}

/******************
* CCPMSG_DiscPeer *
*******************
*
* Standard actions for local (HOST) hangup.  Send disconnect request to the
* ISDN peer.
*
*/
PUBLIC BOOLEAN CCPMSG_DiscPeer( cause )

int cause;       /* Cause for Cause info element in DISCONNECT message */

{
   BOOLEAN error = FALSE;         /* Error encountered in this process */


   if ( CCPRI_GetOutPkt( CC_DISCONNECT_REQ ))
      {
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
         if ( cause )
            {
            Diag[ 0 ] = NO_DIAG;
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   cause,
                                   AM_I_THE_NETWORK_SIDE,
                                   Diag );
            }
         else
            {
            /* It may be that we haven't yet recovered the peer cause from
            *  the HOST's CALL_DISC message.  Do so now.
            */
            if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
               {
               Diag[ 0 ] = NO_DIAG;
               OutPkt->InfoLen +=
                  CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                      IE[ 2 ],
                                      AM_I_THE_NETWORK_SIDE,
                                      Diag );
               }
#ifdef EXTENDED_HOST_MSG
            else
               {
               CCPRI_CopyPublic( CAUSE_IE, CODESET_0 );
               }
#endif
            }
#ifdef EXTENDED_HOST_MSG
         if ( SwitchType == PRI_DMS100_STYPE )
            /* This element is not supported here as a "private."
            */
            CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );
#endif

         switch ( SwitchType )                         /* Consider UUI */
            {
            case PRI_4ESS_STYPE:
            case PRI_5ESS_STYPE:
               /* Consider if UUI should be added from the HOST.
               */
               CCPMSG_AppendHostUUI();
               break;
            case VN2_STYPE:
               break;
            default:                                         /* No UUI */
               break;
            }
         }
      CCPRI_Mail( L3_ID, FALSE );
      }
   else
      {
      error = TRUE;
      }

   return ( error );
}

/**************************
* CCPMSG_ErredOptional_IE *
***************************
*
* Some switch types have unusually severe IE checking requirements,
* such as checking the coding of optional elements and possibly
* optional fields therein.  This function is called in such cases
* when detected to perform specific recovery.
*
*/
PRIVATE void near CCPMSG_ErredOptional_IE( ie_id, cause )
int ie_id;                                      /* Erred IE identifier */
int cause;                                            /* Problem found */
{
   switch ( SwitchType )
      {
#ifdef VN2
      case VN2_STYPE:
#ifdef FFS
         /* Various VN3 specifications conflict on the proper action to
         *  take for non-mandatory IE errors.  The following code will
         *  cause a CC_BROADCAST_RESP to be sent if it hasn't been sent
         *  already for the incoming call (this is a required first
         *  response by Call Control).
         *  Then a CC_STATUS_REQ is generated, which will result in a
         *  STATUS message to the network.  If this operation is required,
         *  this code may be activated by removing the FFS flags.
         *  It is unwise to define this flag as other "alternate" code
         *  throughout the ISDN stack will come to life as a result.
         */
         /* Whether the cause was INVALID_ELEM_CONTENTS or
         *  MANDATORY_IE_LEN_ERR, the recovery is to send STATUS
         *  with cause = INVALID_ELEM_CONTENTS.
         */
         if (( !CCB->Responded ) && ( CCPRI_GetOutPkt( CC_BROADCAST_RESP )))
            {
            CCPRI_Mail( L3_ID, FALSE );      /* Send CC_BROADCAST_RESP */
            CCB->Responded = TRUE;
            }
         if ( CCPRI_GetOutPkt( CC_STATUS_REQ ))
            {
            Diag[ 0 ] = ie_id;
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   INVALID_ELEM_CONTENTS,
                                   AM_I_THE_NETWORK_SIDE,
                                   Diag );
            OutPkt->InfoLen +=
               CCIE_BuildCallState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       SourceState );
            CCPRI_Mail( L3_ID, FALSE );      /* Mail the CC_STATUS_REQ */
            }
#endif
         break;
#endif
      default:
         Trashcan = ie_id;/* Prevent compiler warning for unused parms */
         Trashcan = cause;
         break;                         /* Ignore this class of errors */
      }
   return;
}

/******************
* CCPMSG_FacAckIn *
*******************
*
* Process the CC_FACILITY_ACK message received from the ISDN peer.
*
*/
PUBLIC void CCPMSG_FacAckIn()

{
   int cause1, cause2;            /* Causes from info element analysis */

   if ( CCPRI_GetOutInfo( CALL_FACILITY_ACK ))
      {
      switch( SwitchType )
         {
#ifdef PRI_1TR6
         case PRI_1TR6_STYPE:
            cause1 =  CCPRI_GetFacilities ( FALSE );
            CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            break;
#endif
         case VN2_STYPE:
            CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            break;
         default:
            /* Either the NetFac IE or the CallingNumber IE must be
            *  present, or both may be present.  Neither may be coded
            *  incorrectly.
            */
            cause1 = CCPRI_AcceptNetFac();
            cause2 = CCPRI_AcceptCrNumb();
            if (( cause1 == INVALID_ELEM_CONTENTS )    ||
                ( cause2 == INVALID_ELEM_CONTENTS )    ||
               (( cause1 == MANDATORY_IE_MISSING ) &&
               ( cause2 == MANDATORY_IE_MISSING )))
               {
               if ( cause1 == INVALID_ELEM_CONTENTS )
                  CCPMSG_RejectMsg( INVALID_ELEM_CONTENTS, NETWORK_FACILITY_IE );
               else if ( cause2 == INVALID_ELEM_CONTENTS )
                  CCPMSG_RejectMsg( INVALID_ELEM_CONTENTS, CALLER_NUMBER_IE );
               else if ( cause1 == MANDATORY_IE_MISSING )
                  CCPMSG_RejectMsg( MANDATORY_IE_MISSING, NETWORK_FACILITY_IE );
               else if ( cause2 == MANDATORY_IE_MISSING )
                  CCPMSG_RejectMsg( MANDATORY_IE_MISSING, CALLER_NUMBER_IE );
               CCPRI_FreePkgInfo();
               }
            else
               {
               CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
               }
         }
      }
   return;
}

/*******************
* CCPMSG_FacAckOut *
********************
*
* Process the CALL_FACILITY_ACK message from the HOST.
*
*/
PUBLIC void CCPMSG_FacAckOut()

{
   int   id_len;                                         /* Net ID len */
   int   id_type;                                           /* ID type */
   int   id_plan;                                           /* ID plan */
   BYTE *net_id;                                         /* Network ID */


   if ( CCPRI_GetOutPkt( CC_FAC_ACK_REQ ))
      {
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
#ifdef EXTENDED_HOST_MSG
#ifdef PRI_DMS100
         if ( SwitchType == PRI_DMS100_STYPE )
            /* This element is not supported here as a "private."
            */
            CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );
#endif
#endif
         if (( IE = CCPRI_GetPrivate( NETWORK_FACILITY_IE,
                                      CODESET_0 )) != NULL )
            {
            if ( IE[ 1 ] > 2 )                 /* Network ID provided? */
               {
               id_len  =  IE[ 4 ];
               id_type =  IE[ 5 ];
               id_plan =  IE[ 6 ];
               net_id  = &IE[ 7 ];
               }
            else
               {
               id_len = id_type = id_plan = 0;
               net_id = NULL;
               }
            OutPkt->InfoLen +=
               CCIE_BuildNetFac_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    id_len,              /* Net ID len */
                                    id_type,                /* ID type */
                                    id_plan,                /* ID plan */
                                    net_id,              /* Network ID */
                                    FALSE,         /* parameterized-no */
                                    TRUE,             /* expansion-yes */
                                    IE[ 2 ],           /* feat or serv */
                                    IE[ 3 ],           /* NetFac value */
                                    NULL,           /* Param field-N/A */
                                    0 );          /* Parameter len-N/A */
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( NETWORK_FACILITY_IE, CODESET_0 );
            }
         CCPRI_CopyPublic( CALLER_NUMBER_IE, CODESET_0 );
#endif
         }
      CCPRI_Mail( L3_ID, FALSE );
      }

   return;
}

/******************
* CCPMSG_FacRejIn *
*******************
*
* Process the CC_FACILITY_REJ message received from the ISDN peer.
*
*/
PUBLIC void CCPMSG_FacRejIn()

{
   int cause;                                /* Cause for bad CAUSE_IE */
   int ie_cause;                            /* Cause from the CAUSE_IE */

   if ( CCPRI_GetOutInfo( CALL_FACILITY_REJ ))
      {
      switch( SwitchType )
         {
#ifdef PRI_1TR6
         case PRI_1TR6_STYPE:
             if (( cause = CCPRI_AcceptCause( &ie_cause )) == ACCEPTED )
                {
                OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
                OutPkgInfo[ OutPkgInfoLen++ ] = 1;
                OutPkgInfo[ OutPkgInfoLen++ ] = ie_cause;
                }
             CCPRI_GetFacilities ( FALSE );
             CCPRI_Mail( HOST_ID, TRUE );/* Relay the result to the HOST */
            break;
#endif
         case VN2_STYPE:
            cause = CCPRI_AcceptCause( &ie_cause );
            if (( cause == MANDATORY_IE_MISSING ) ||
                ( cause == MANDATORY_IE_LEN_ERR ))
               {
               CCPMSG_RejectResponse( cause, 0 );
               CCPMSG_SendHostDisc( 0, FALSE );
               CCPRI_ReleaseCall();
               }
            else
               {
               OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = ie_cause;
               CCPRI_Mail( HOST_ID, TRUE );
               }
            break;
         default:
            if (( cause = CCPRI_AcceptCause( &ie_cause )) != ACCEPTED )
               {
               CCPMSG_RejectMsg( cause, CAUSE_IE );
               CCPRI_FreePkgInfo();
               }
            else
               {
               CCPRI_AcceptNetFac();
               OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = ie_cause;
               CCPRI_Mail( HOST_ID, TRUE );
               }
            break;
         }
      }
   return;
}

/*******************
* CCPMSG_FacRejOut *
********************
*
* Process the CALL_FACILITY_REJ message received from the HOST.
*
*/
PUBLIC void CCPMSG_FacRejOut()

{
   int   id_len;                                         /* Net ID len */
   int   id_type;                                           /* ID type */
   int   id_plan;                                           /* ID plan */
   BYTE *net_id;                                         /* Network ID */

   if ( CCPRI_GetOutPkt( CC_FAC_REJ_REQ ))
      {
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
         if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
            {
            Diag[ 0 ] = NO_DIAG;
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   IE[ 2 ],
                                   AM_I_THE_NETWORK_SIDE,
                                   Diag );
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CAUSE_IE, CODESET_0 );
            }
         if ( SwitchType == PRI_DMS100_STYPE )
            /* This element is not supported here as a "private."
            */
            CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );

#endif
         if (( IE = CCPRI_GetPrivate( NETWORK_FACILITY_IE, CODESET_0 )) != NULL )
            {
            if ( IE[ 1 ] > 2 )                 /* Network ID provided? */
               {
               id_len  =  IE[ 4 ];
               id_type =  IE[ 5 ];
               id_plan =  IE[ 6 ];
               net_id  = &IE[ 7 ];
               }
            else
               {
               id_len = id_type = id_plan = 0;
               net_id = NULL;
               }
            OutPkt->InfoLen +=
               CCIE_BuildNetFac_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                    id_len,              /* Net ID len */
                                    id_type,                /* ID type */
                                    id_plan,                /* ID plan */
                                    net_id,              /* Network ID */
                                    FALSE,         /* parameterized-no */
                                    TRUE,             /* expansion-yes */
                                    IE[ 2 ],           /* feat or serv */
                                    IE[ 3 ],           /* NetFac value */
                                    NULL,           /* Param field-N/A */
                                    0 );          /* Parameter len-N/A */
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( NETWORK_FACILITY_IE, CODESET_0 );
            }
#endif
         }
      CCPRI_Mail( L3_ID, FALSE );
      }

   return;
}

/******************
* CCPMSG_FacReqIn *
*******************
*
* Process the CC_FACILITY_IND for far-end FACILITY request.
*
*/
PUBLIC void CCPMSG_FacReqIn()

{
   int cause;                                /* Cause from IE analysis */


   if ( CCPRI_GetOutInfo( CALL_FACILITY ))
      {
      if (( cause = CCPRI_AcceptNetFac()) != ACCEPTED )
         {
         /* This implementation currently assumes no Codeset 6 facility.
         */
         CCPMSG_RejectMsg( cause, NETWORK_FACILITY_IE );
         CCPRI_FreePkgInfo();
         }
      else
         {
         CCPRI_Mail( HOST_ID, TRUE );
         }
      }

   return;
}

/*******************
* CCPMSG_FacReqOut *
********************
*
* Accept the CALL_FACILITY message from the HOST and prepare the
* CC_FACILITY_REQ message for the ISDN peer.
*
*/
PUBLIC void CCPMSG_FacReqOut()

{
   int   id_len;                                         /* Net ID len */
   int   id_type;                                           /* ID type */
   int   id_plan;                                           /* ID plan */
   BYTE *net_id;                                         /* Network ID */

   if ( CCPRI_GetOutPkt( CC_FACILITY_REQ ))
      {
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
#ifdef EXTENDED_HOST_MSG
         /* Info element not supported as a "private" element in this msg.
         */
         CCPRI_CopyPublic( CHANNEL_ID_IE, CODESET_0 );
         if ( SwitchType == PRI_DMS100_STYPE )
            /* This element is not supported here as a "private."
            */
            CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );

#endif
         if (( IE = CCPRI_GetPrivate( NETWORK_FACILITY_IE,
                                      CODESET_0 )) != NULL )
            {
            switch ( SwitchType )
               {
               case VN2_STYPE:
                  OutPkt->InfoLen +=
                     CCIE_BuildNatFac_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          &IE[ 5 ], IE[ 4 ] );
                  break;
               default:
                  if ( IE[ 1 ] > 2 )           /* Network ID provided? */
                     {
                     id_len  =  IE[ 4 ];
                     id_type =  IE[ 5 ];
                     id_plan =  IE[ 6 ];
                     net_id  = &IE[ 7 ];
                     }
                  else
                     {
                     id_len = id_type = id_plan = 0;
                     net_id = NULL;
                     }
                  OutPkt->InfoLen +=
                     CCIE_BuildNetFac_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          id_len,        /* Net ID len */
                                          id_type,          /* ID type */
                                          id_plan,          /* ID plan */
                                          net_id,        /* Network ID */
                                          FALSE,   /* parameterized-no */
                                          TRUE,       /* expansion-yes */
                                          IE[ 2 ],     /* feat or serv */
                                          IE[ 3 ],     /* NetFac value */
                                          NULL,     /* Param field-N/A */
                                          0 );    /* Parameter len-N/A */
                  break;
               }
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( NETWORK_FACILITY_IE, CODESET_0 );
            }
#endif
         }
      CCPRI_Mail( L3_ID, FALSE );
      }

   return;
}

/****************
* CCPMSG_InCall *
*****************
*
* Process the CC_SETUP_IND message, generating a CALL_INCOMING message
* to the HOST.
*
*/
PUBLIC BOOLEAN CCPMSG_InCall()

{
   int     cause = 0;                  /* Cause for unacceptable IE(s) */
   int     rate;                                        /* Bearer rate */
   BOOLEAN success = TRUE;                  /* Result of this function */
   int     prog_desc;                          /* Progress description */

   if(detail_debug)
      buginf("\n entering CCPMSG_InCall");

   /* Start off by allocating an info buffer in which the HOST's
   *  CALL_INCOMING will be constructed on-the-fly.
   */
   if ( !CCPRI_GetOutInfo( CALL_INCOMING ))
      {
      CCPMSG_RejectInCall( CALL_REJECTED, 0 );
      success = FALSE;
      goto in_call_failed;
      }

   /* Check each mandatory info element from the network for correctness
   *  and recover components of each element.  The relevant data from each
   *  IE will be moved into the pkg info buffer of the CALL_INCOMING message
   *  in each "accept" routine.
   */
   if (( cause = CCPRI_AcceptBearerCap( &rate )) != ACCEPTED )
      {
      CCPMSG_RejectInCall( cause, BEARER_CAPABILITY_IE );
      success = FALSE;
      goto in_call_failed;
      }

   /* Set the presumed channel type for the call based upon the
   *  provided bearer rate.
   */
   CCB->ChanType = CCPRI_RateToType( rate );

   /* For VN3, note that the channel offered by the network is most generally 
   *  expected as "exclusive", however, this is not manadatory for all VN3
   *  private network data applications, so it is difficult to enforce here.
   */ 
   if (( cause = CCPRI_AcceptChanId( CC_SETUP_IND, TRUE )) != ACCEPTED )
      {
      if ( cause == NO_CIRCUIT_AVAILABLE )
         {
         switch ( SwitchType )
            {
            case PRI_DMS100_STYPE:
               CCPMSG_RejectInCall( NO_CIRCUIT_AVAILABLE, 0 );  /* #44 */
               break;
            case VN2_STYPE:
               CCPMSG_RejectInCall( NO_CIRCUIT_AVAILABLE, 0 );  /* #44 */
               break;
            default:
               /* Select cause 34/44 based upon the "exclusive" option.
               */
               CCPMSG_RejectInCall(( CCB->Exclusive ) ? 
                                        REQ_CHANNEL_NOT_AVAIL : 
                                        NO_CIRCUIT_AVAILABLE,
                                     0 );
               break;
            }
         }
      else
         {
         CCPMSG_RejectInCall( cause, CHANNEL_ID_IE );
         }
      success = FALSE;
      goto in_call_failed;
      }

   if ( SwitchType == VN2_STYPE )
      {
      /* This element may be expected by some DP Equipment in the
      *  incoming SETUP message indicating interworking.
      */
      if (( cause = CCPRI_AcceptProgressInd( &prog_desc )) == ACCEPTED )
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = prog_desc;
         }
      else if ( cause != MANDATORY_IE_MISSING )
         {
         CCPMSG_ErredOptional_IE( NETWORK_FACILITY_IE, cause );
         }
      }
   cause = CCPRI_AcceptNetFac();                           /* Optional */
   if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
      CCPMSG_ErredOptional_IE( SIGNAL_IE, cause );

   cause = CCPRI_AcceptSignal();   /* Element is optional - get signal */
   if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
      CCPMSG_ErredOptional_IE( SIGNAL_IE, cause );

   cause = CCPRI_AcceptCrNumb(); /* This element is optional - get CPN */
   if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
      CCPMSG_ErredOptional_IE( CALLER_NUMBER_IE, cause );

   cause = CCPRI_AcceptCrSubAddr();  /* Element is optional - get CPSA */
   if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
      CCPMSG_ErredOptional_IE( CALLER_SUB_ADDR_IE, cause );

   cause = CCPRI_AcceptCdNumb();
   if (( cause != ACCEPTED ) && (cause != MANDATORY_IE_MISSING))
      {
      /* This treatment (mandatory IE error) is consistent with some
      *  applications which do not route the call on the CALLED_SUB_ADDR_IE.
      *  This treatment may be inappropriate for others.
      */
      CCPMSG_RejectInCall( cause, CALLED_NUMBER_IE );
      success = FALSE;
      goto in_call_failed;
      }

   cause = CCPRI_AcceptCdSubAddr();  /* Element is optional - get CPSA */
   if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
      CCPMSG_ErredOptional_IE( CALLED_SUB_ADDR_IE, cause );

   switch ( SwitchType )         /* Consider UUI based upon SwitchType */
      {
      case PRI_DMS100_STYPE:
         break;
#ifdef PRI_1TR6
      case PRI_1TR6_STYPE:
         break;
#endif
      case VN2_STYPE:
         break;
      default:
         cause = CCPRI_AcceptUserUser();    /* UUI element is optional */
         if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
            CCPMSG_ErredOptional_IE( USER_USER_IE, cause );
         break;
      }
   CCB->ClearHost = TRUE;

   CCPRI_Mail( HOST_ID, TRUE );                  /* Send CALL_INCOMING */

in_call_failed:
   if ( OutPkgInfo )                    /* We have a buffer to release */
      CCPRI_FreePkgInfo();

   return( success );
}

/******************
* CCPMSG_NotifyIn *
*******************
*
* Process the CC_NOTIFY_IND message from the peer.
*
*/
PUBLIC void CCPMSG_NotifyIn()

{
   int cause, recovered_cause;                       /* Various causes */
   int prog_desc;                              /* Progress description */
   int notify_desc;                              /* Notify description */


   if ( CCPRI_GetOutInfo( CALL_NOTIFY ))
      {
      if ( CCPRI_AcceptCause( &recovered_cause ) == ACCEPTED )
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = recovered_cause;
         }
      if (( cause = CCPRI_AcceptProgressInd( &prog_desc )) == ACCEPTED )
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = prog_desc;
         }
      if (( cause = CCPRI_AcceptNotify( &notify_desc )) == ACCEPTED )
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = NOTIFY_IND_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = notify_desc;
         }
      else                               /* This element was mandatory */
         {
         CCPMSG_RejectMsg( cause, NOTIFY_IND_IE );
         CCPRI_FreePkgInfo();
         goto bailout;
         }
      CCPRI_Mail( HOST_ID, TRUE );
      }
bailout:
   return;
}

/****************
* CCPMSG_InfoIn *
*****************
*
* Process the CC_INFO_IND message from the peer.
* Added for TS014 homologation.
*
*/
PUBLIC void CCPMSG_InfoIn ()
{
   int cause;

   if (CCPRI_GetOutInfo(CALL_INFO)) {
      cause = CCPRI_AcceptDisplay();
      if (cause != NOERR) {
         CCPMSG_RejectMsg(cause, DISPLAY_IE);
         CCPRI_FreePkgInfo();
      } else
         CCPRI_Mail(HOST_ID, TRUE);
   }
}

/*******************
* CCPMSG_NotifyOut *
********************
*
* Process the CALL_NOTIFY message from the HOST.
*
*/
PUBLIC void CCPMSG_NotifyOut()

{
   if ( CCPRI_GetOutPkt( CC_NOTIFY_REQ ))
      {
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
         if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
            {
            Diag[ 0 ] = 0;
            OutPkt->InfoLen +=                   /* Build the message. */
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   IE[ 2 ],
                                   AM_I_THE_NETWORK_SIDE,
                                   Diag );
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CAUSE_IE, CODESET_0 );
            }
#endif
         if (( IE = CCPRI_GetPrivate( PROGRESS_IND_IE,
                                      CODESET_0 )) != NULL )
            {
            OutPkt->InfoLen +=
               CCIE_BuildProgInd_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CCITT_STANDARD,
                                     MY_LOCATION,
                                     IE[ 2 ] );
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( PROGRESS_IND_IE, CODESET_0 );
            }
#endif
         if (( IE = CCPRI_GetPrivate( NOTIFY_IND_IE, CODESET_0 )) != NULL )
            {
            OutPkt->InfoLen +=
               CCIE_BuildNotifyInd_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       &IE[ 2 ],
                                       1 );
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( NOTIFY_IND_IE, CODESET_0 );
            }
#endif
         }
#ifdef EXTENDED_HOST_MSG
      /* Other elements not specifically supported in the HOST message.
      */
      CCPRI_CopyPublic( DISPLAY_IE, CODESET_0 );
      CCPRI_CopyPublic( INFO_REQ_IE, CODESET_0 );
      CCPRI_CopyPublic( CALLER_NUMBER_IE, CODESET_0 );
      /* Note that CONNECTED_NUMBER_IE is not considered here as the
      *  NTI spec puts it here, which is in error since it is not
      *  in correct ascending numerical order by ie_id.
      */
      CCPRI_CopyPublic( REDIRECTION_NUM_IE, CODESET_0 );
#endif
      CCPRI_Mail( L3_ID, FALSE );
      }

   return;
}

/*****************
* CCPMSG_OutCall *
******************
*
* Process the CALL_OUTGOING message from the HOST.
*
* Return TRUE if the process completed with success.
*
*/
PUBLIC BOOLEAN CCPMSG_OutCall()

{
   int      cause;                       /* Cause for unacceptable IEs */
   BYTE     bear_cap[ 4 ];/* Local bearer capability construction buff */
   int      bear_rate;                /* Bearer rate implying ChanType */
   int      bear_cap_len;   /* Length of constructed bearer capability */
   int      chan;                                       /* B/H-channel */
   int      int_id;                                    /* Interface ID */
   BOOLEAN  exclusive;                           /* Channel exclusive? */
   BOOLEAN  private_bear_cap;         /* HOST sent bear cap as priv ie */
   BOOLEAN  private_chan_id;        /* HOST send channel id as priv ie */
   int      id_len;                                      /* Net ID len */
   int      id_type;                                        /* ID type */
   int      id_plan;                                        /* ID plan */
   BYTE    *net_id;                                      /* Network ID */
   BOOLEAN  success;                       /* Result for this function */
   BOOLEAN  any_chan;

   bear_cap_len = 0;
   any_chan = FALSE;                    /* user supplied or any */
   cause = 0;                           /* Cause for unacceptable IEs */
   bear_rate = 0;                    /* Bearer rate implying ChanType */
   private_bear_cap = FALSE;         /* HOST sent bear cap as priv ie */
   private_chan_id = FALSE;        /* HOST send channel id as priv ie */
   success = TRUE;                        /* Result for this function */

   CCB->ClearHost = TRUE;       /* The call will be cleared regardless */
   /* We start off getting the OutPkt since we will be building the
   *  CC_SETUP_REQ message as we go through the IEs of the HOST's message.
   */
   if ( CCPRI_GetOutPkt( CC_SETUP_REQ ))
      {
      /* ***************** Consider BEARER_CAPABILITY *******************
      */
      if (( IE = CCPRI_GetPrivate(BEARER_CAPABILITY_IE, CODESET_0)) != NULL )
         {
         private_bear_cap = TRUE;
         /* Recover and prepare the XFER capability, XfER mode, XFER rate
         *  and Layer 1 info.
         */
         bear_cap[ 0 ] = 0x80 | IE[2];                     /* Xfer cap */
         bear_cap[ 1 ] = 0x80 | (IE[3] << 5) | IE[4];     /* Mode/rate */
         bear_rate = IE[4];  /* Save off bearer rate implying ChanType */
         /* Consider user info Layer 1 protocol and user rate.
         *  NOTE: If more extensive user rate data is required than a
         *  single octet, this code must be extended accordingly.
         */
         if ( IE[ 1 ] > 3 )       /* Bearer Cap octet 5 info provided? */
            {
            bear_cap[ 2 ] = 0x20 | IE[ 5 ];/* Layer ID & User L1 proto */
            if ( IE[ 1 ] == 4 )
               {
               bear_cap[ 2 ] |= 0x80;                 /* Extension bit */
               }
            else if ( IE[ 1 ] == 5 )         /* User rate is provided? */
               {
               bear_cap[ 3 ] = 0x80 | IE[ 6 ];            /* User rate */
               }
            else
               {
               if (detail_debug)
                   buginf("\n outgoing call, bad private BC");
               CCPRI_Error( CCPMSG_OUT_CALL, P_BAD_DATA_FROM_HOST,
                            BEARER_CAPABILITY_IE, FALSE );
               CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
               cause = INVALID_ELEM_CONTENTS;
               goto out_call_failed;
               }
            }
         bear_cap_len = IE[ 1 ] - 1;
         }
#ifdef EXTENDED_HOST_MSG
      else if (( IE = CCPRI_GetPublic( BEARER_CAPABILITY_IE,
                                       CODESET_0 )) != NULL )
         {
         IE_Fields = CCIE_ExpandBearCap_IE( IE, &Trashcan );
         bear_rate = IE_Fields[ 4 ];
         }
#endif
      else
         {
         if(isdn_debug)
            buginf("\n Outgoing call using a bad Bearer Capability IE");

         CCPRI_Error( CCPMSG_OUT_CALL, P_BAD_DATA_FROM_HOST,
                      BEARER_CAPABILITY_IE, FALSE );
         CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
         cause = MANDATORY_IE_MISSING;
         goto out_call_failed;
         }

      /* Set the presumed channel type for the call based upon the
      *  provided bearer rate.
      */
      if (( CCB->ChanType = CCPRI_RateToType(bear_rate)) == NO_CHANNEL_UNITS )
         { 
         CCPRI_Error( CCPMSG_OUT_CALL, P_BAD_DATA_FROM_HOST,
                      BEARER_CAPABILITY_IE, FALSE );
         CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
         cause = INVALID_ELEM_CONTENTS;
         goto out_call_failed;
         }

      /* ******************** Consider CHANNEL ID *************************
      */
      /* One of these must be provided.
      *  We must determine the channel situation before we proceed since
      *  the selected channel is stored away in the CCB and the Channel
      *  Management Table.  This element will have to be reconsidered in
      *  its message position when the CC_SETUP_REQ message is being built.
      */
      if (( IE = CCPRI_GetPrivate( CHANNEL_ID_IE, CODESET_0 )) != NULL )
         {
         private_chan_id = TRUE;
         chan      = IE[ 4 ];
         /*
          * If the channel requested is 0, then the isdn code is to
          * determine which channel to use for the call. Can be any channel.
          */
         if(chan == 0)
            any_chan = TRUE;
         exclusive = IE[ 2 ];
         int_id    = ( MultiInterface ) ? IE[ 5 ] : 0;
         IE_Fields = NULL;
         }
#ifdef EXTENDED_HOST_MSG
      else if (( IE = CCPRI_GetPublic( CHANNEL_ID_IE, CODESET_0 )) != NULL )
         {
         IE_Fields = CCIE_ExpandChanId_IE( IE, &Trashcan );
         chan      = IE_Fields[ 9 ];
         exclusive = IE_Fields[ 2 ];
         int_id    =
            (IE_Fields[0] && MultiInterface) ? IE_Fields[IE_Fields[8] ] : 0;
         }
#endif
      else
         {
         CCPRI_Error( CCPMSG_OUT_CALL, P_BAD_DATA_FROM_HOST,
                      CHANNEL_ID_IE, FALSE );
         CCPMSG_SendHostProgress( MESSAGE_ERROR, FALSE, FALSE );
         cause = MANDATORY_IE_MISSING;
         goto out_call_failed;
         }
      /* Check if the HOST is requesting an UNavailable channel or is
       * browsing for a channel.
	   */
      if ( any_chan )
         {
         exclusive = FALSE;
         }
      /* Now determine if we will need to go find a free channel.
      */
      if (( SwitchType == VN2_STYPE ) && ( any_chan ))
         {
         ;    /* Do nothing for now.  Let the network pick the channel */
         }
      else

      if (( any_chan )                                       ||
          ( CC_CHAN_GetStatepri( int_id, chan,
                              CCB->ChanType, Dsl_Id ) != CHANNEL_IDLE ) ||
          ( CC_CHAN_GetServState( int_id, chan,
                                  CCB->ChanType, Dsl_Id ) != IN_SERVICE ))
         {
         if ( exclusive )                        /* Exclusive request? */
            {
            /* A note to DMS100 users:
            *
            *  The DMS-100 documentation requires that the channel selection
            *  to the network be made exclusively (Call Control Procedures
            *  (5.1.1.1 c. of NIS A211-1, Ver 2. 12/89).  The machinery to
            *  make this request exclusive is provided to the HOST in the
            *  CALL_OUTGOING msg.  Since the HOST may not care which channel
            *  is used and it cannot resolve all race conditions with the
            *  network for channel usage, we assume that if the HOST makes a
            *  non-exclusive request, CCPMSG will select any idle channel as
            *  an alternate, but will request IT exclusively to the network.
            */
            /* A note to VN3 users:
            *
            *  The VN3 documentation requires that the channel selection
            *  to the network be made exclusively (Call Control Procedures
            *  (5.1.1.2 a. of ST/LAA/RSM/144.  The machinery to
            *  make this request exclusive is provided to the HOST in the
            *  CALL_OUTGOING msg.  Since the HOST may not care which channel
            *  is used and it cannot resolve all race conditions with the
            *  network for channel usage, we assume that if the HOST makes a
            *  non-exclusive request, CCPMSG will select any idle channel as
            *  an alternate, but will request IT exclusively to the network.
            */
            cause = REQ_CHANNEL_NOT_AVAIL;                 /* Cause 44 */
            goto out_call_failed;
            }
         /* Select another channel for proposal to the ISDN peer.
         *  Reset the int_id to 0.
         *  For H11 channels, the interface containing the D-channel should
         *  be initialized in CCPRI_Init() such that the D-channel is
         *  designated as RESERVED.   Thus this interface will never appear
         *  as available for the H11 call to CC_CHAN.
         */
         int_id = 0;
         switch ( SwitchType )
            {
            case PRI_DMS100_STYPE:
               while (( !CCB->B_Chan ) && ( int_id <= MAX_INT_IDP ))
                  {
                  /* We ASSUME here - Only 23 chans in int_id = 0
                   * if defined as a User then look from high channel to
                   * low channel (i.e. 23 to 1), if Network, low to 
                   * high (i.e. 1 to 23). ===
                   */
                  if (Network_side) {
                      if ((CCB->B_Chan = CC_CHAN_GetIdleChanpri(int_id,
                           MIN_CHANNEL, MAX_CHANNELP, CCB->ChanType,
                           Dsl_Id)) == 0)
                           int_id++;
                  } else {
                      if ((CCB->B_Chan = CC_CHAN_GetIdleDMSChan( int_id,
                           MIN_CHANNEL, MAX_CHANNELP, Dsl_Id)) == 0)
                           int_id++;
                  }
                  }
               break;
            default:
               while (( !CCB->B_Chan ) && ( int_id <= MAX_INT_IDP ))
                  {
                  /*
                   * User side (default) gets channels high to low,
                   * Network side gets channels low to high (1 to 23)
                   */
                  if (Network_side) {
                      if ((CCB->B_Chan = CC_CHAN_GetIdleChanpri(int_id,
                           MIN_CHANNEL, MAX_CHANNELP, CCB->ChanType,
                           Dsl_Id)) == 0)
                           int_id++;
                  } else {
                      if ((CCB->B_Chan = CC_CHAN_GetIdleDMSChan( int_id,
                           MIN_CHANNEL, MAX_CHANNELP, Dsl_Id)) == 0)
                           int_id++;
                  }
                  }
               break;
            }
         if ( CCB->B_Chan == 0 )
            {
            cause = NO_CIRCUIT_AVAILABLE;                  /* Cause 34 */
            goto out_call_failed;
            }
         }
      else
         {
         CCB->B_Chan = chan;       /* Save the original channel number */
         }
      CCB->Int_Id[ 0 ] = int_id;              /* Save the interface ID */
      switch ( SwitchType )
         {
         case PRI_DMS100_STYPE:
            exclusive = TRUE;  /* Selection is always made exclusively */
            break;
         case VN2_STYPE:
            exclusive = TRUE;  /* Selection is always made exclusively */
            break;
         default:
            break;                                              /* NOP */
         }
         /*
          * Set channel parameters now.
          */

         CC_CHAN_SetUserpri( CCB->Int_Id[ 0 ], CCB->B_Chan,
                          CCB->ChanType, CCB->Call_ID, Dsl_Id );
         CC_CHAN_SetProtocolpri( CCB->Int_Id[ 0 ], CCB->B_Chan,
                              CCB->ChanType, L2P_LAPD, Dsl_Id );
         CC_CHAN_SetStatepri( CCB->Int_Id[ 0 ], CCB->B_Chan,/* Negotiable */
                           CCB->ChanType, CHANNEL_PROPOSED, Dsl_Id );
      CCB->Exclusive = exclusive;      /* Save the exclusive indicator */

      /* Now that the channel study is over, we may proceed.
      */
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )                 /* No private elements? */
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
#ifdef EXTENDED_HOST_MSG
        /*  ***************** Consider CALL IDENT ************************
         *  This element is not supported as a "private."
         */
         CCPRI_CopyPublic( CALL_IDENT_IE, CODESET_0 );
#endif
         /*
          * Get the Sending Complete IE for NET5.
          */
         CCPRI_CopyPublic(SENDING_COMPLETE_IE, CODESET_0);

         /* ************* Consider BEARER_CAPABILITY (again) *************
         */
         if ( private_bear_cap )
            {
            OutPkt->InfoLen +=
               CCIE_BuildBearerCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       bear_cap, bear_cap_len );
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( BEARER_CAPABILITY_IE, CODESET_0 );
            }
#endif
         /* ***************** Consider CHANNEL ID (again) **************/
         if ( private_chan_id )    /* Priv CHANNEL_ID_IE version used? */
            {
            if ( !any_chan )
               {
               CCB->B_Chan = 0;    /* Do not include the CHANNEL_ID_IE */
               }
            else                             /* Add channel ID element */
               {
               if(detail_debug)
                  buginf("\n building outgoing channel id for call");
               OutPkt->InfoLen +=
                  CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       CHANNELS_LISTED,
                                       CCB->B_Chan,
                                       CCB->ChanType,
                                       ( CCB->ChanType == H0_CHANNEL_UNITS ),
                                       CCB->Exclusive,    /* Exclusive */
                                       CCB->Int_Id_Len,/* Intrface ID len */
                                       CCB->Int_Id );/* Intrface ID ptr */
               }
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CHANNEL_ID_IE, CODESET_0 );
            }

         if ( SwitchType == PRI_DMS100_STYPE )
            /* ***************** Consider FACILITY ********************
            */
            /* This element is not supported here as a "private."
            */
            CCPRI_CopyPublic( FACILITY_IE, CODESET_0 );

         /* ***************** Consider PROGRESS_IND ********************
         */
         /* This element is not supported here as a "private."
         */
         CCPRI_CopyPublic( PROGRESS_IND_IE, CODESET_0 );
#endif
         /* ************** Consider NETWORK FACILITIES *****************
         */
         if (( IE = CCPRI_GetPrivate( NETWORK_FACILITY_IE,
                                      CODESET_0 )) != NULL )
            {
            switch ( SwitchType )
               {
               case VN2_STYPE:
                  OutPkt->InfoLen +=           /* NATIONAL_FACILITY_IE */
                     CCIE_BuildNatFac_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          &IE[3],   /* Facility string */
                                          IE[2] );  /* Facility length */
                  break;
               default:
                  if ( IE[ 1 ] > 2 )           /* Network ID provided? */
                     {
                     id_len  =  IE[ 4 ];
                     id_type =  IE[ 5 ];
                     id_plan =  IE[ 6 ];
                     net_id  = &IE[ 7 ];
                     }
                  else
                     {
                     id_len = id_type = id_plan = 0;
                     net_id = NULL;
                     }
                  OutPkt->InfoLen +=
                     CCIE_BuildNetFac_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          id_len,        /* Net ID len */
                                          id_type,          /* ID type */
                                          id_plan,          /* ID plan */
                                          net_id,        /* Network ID */
                                          FALSE,   /* parameterized-no */
                                          TRUE,       /* expansion-yes */
                                          IE[ 2 ],        /* feat-serv */
                                          IE[ 3 ],     /* NetFac value */
                                          NULL,     /* Param field-N/A */
                                          0 );    /* Parameter len-N/A */
                  break;
               }
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( NETWORK_FACILITY_IE, CODESET_0 );
            }
#endif
         if ( SwitchType == VN2_STYPE )
            {
            /* Add TERMINAL_CAPABILITY_IE non-locking in Codeset 6.
            */
            OutPkt->InfoLen +=
               CCIE_BuildCodesetShift_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                          CODESET_6, FALSE );
            OutPkt->InfoLen +=
               CCIE_BuildTermCap_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     CEPT_NATIONAL_STANDARD,
                                      NATIONAL_FUNCTION );
            /* We will ignore the KEYPAD_FACILITY_IE for now since the
            *  network doesn't look at it and it is optional.
            */
            }
         /* ************* Consider CALLING PARTY ID *****************
         */
         if (( IE = CCPRI_GetPrivate( CALLER_NUMBER_IE, CODESET_0 )) != NULL )
            {
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[OutPkt->InfoLen],
                                       CALLER_NUMBER_IE,
                                       IE[ 2 ],         /* Number type */
                                       IE[ 3 ],         /* Number plan */
                                      &IE[ 4 ],              /* digits */
                                       IE[ 1 ] - 2,   /* Number length */
                                       TRUE );             /* In ASCII */
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CALLER_NUMBER_IE, CODESET_0 );
            }
#endif
         /* *********** Consider CALLING PARTY SUB-ADDRESS ***************
         */
         if (( IE = CCPRI_GetPrivate( CALLER_SUB_ADDR_IE, CODESET_0 )) != NULL )
            {
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[OutPkt->InfoLen],
                                       CALLER_SUB_ADDR_IE,
                                       IE[ 2 ],         /* Number type */
                                       IE[ 3 ],         /* Number plan */
                                      &IE[ 6 ],      /* Number pointer */
                                       IE[ 1 ] - 4,   /* Number length */
                                       TRUE );             /* In ASCII */
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CALLER_SUB_ADDR_IE, CODESET_0 );
            }
#endif
         /* *************** Consider CALLED PARTY ID ********************
         */
         if (( IE = CCPRI_GetPrivate( CALLED_NUMBER_IE, CODESET_0 )) != NULL )
            {
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[OutPkt->InfoLen],
                                       CALLED_NUMBER_IE,
                                       IE[ 2 ],         /* Number type */
                                       IE[ 3 ],         /* Number plan */
                                      &IE[ 4 ],      /* Number pointer */
                                       IE[ 1 ] - 2,   /* Number length */
                                       TRUE );             /* In ASCII */
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CALLED_NUMBER_IE, CODESET_0 );
            }
#endif
         /* ************** Consider CALLED PARTY SUB ADDR *****************
         */
         if (( IE = CCPRI_GetPrivate( CALLED_SUB_ADDR_IE,
                                      CODESET_0 )) != NULL )
            {
            OutPkt->InfoLen +=
               CCIE_BuildPartyNumb_IE( &OutPkt->Info[OutPkt->InfoLen],
                                       CALLED_SUB_ADDR_IE,
                                       IE[ 2 ],         /* Number type */
                                       IE[ 3 ],         /* Number plan */
                                      &IE[ 4 ],      /* Number pointer */
                                       IE[ 1 ] - 2,   /* Number length */
                                       TRUE );             /* In ASCII */
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CALLED_SUB_ADDR_IE, CODESET_0 );
            }
#ifdef VN2
         if ( SwitchType == VN2_STYPE )
            /* ************** Consider USER_USER_FAC_IE ****************
            */
            /* The USER-USER facilities is not currently supported
            *  as a "private" IE and must be presented by the Host in
            *  "public" presentation.
            */
            CCPRI_CopyPublicShifted( USER_USER_FAC_IE, CODESET_6, FALSE );
#endif                                                          /* VN2 */
         if ( SwitchType == PRI_DMS100_STYPE )
            /* ************ Consider ORIGINAL_CALLED_NUM_IE *************
            */
            /* This element is not supported as a "private."
            */
            CCPRI_CopyPublic( ORIGINAL_CALLED_NUM_IE, CODESET_0 );

         /* *************** Consider REDIRECTING_NUM_IE *****************
         */
         /* This element is not supported as a "private."
         */
         CCPRI_CopyPublic( REDIRECTING_NUM_IE, CODESET_0 );

         /* ************* Consider TRANSIT_NETWORK_SEL_IE ***************
         */
         /* This element is not supported as a "private."
         */
         CCPRI_CopyPublic( TRANSIT_NETWORK_SEL_IE, CODESET_0 );

         /* ************** Consider LOW_LAYER_COMPAT_IE *****************
         */
         /* This element is not supported as a "private."
         */
         CCPRI_CopyPublic( LOW_LAYER_COMPAT_IE, CODESET_0 );
#endif
         /* ************** Consider HIGH_LAYER_COMPAT_IE ****************
         */
         if (( IE = CCPRI_GetPrivate( HIGH_LAYER_COMPAT_IE,
                                      CODESET_0 )) != NULL )
            {
            OutPkt->InfoLen +=
               CCIE_BuildHighLayer_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       IE[ 2 ],            /* Standard */
                                       IE[ 3 ],      /* Interpretation */
                                       IE[ 4 ],        /* Presentation */
                                       IE[ 5 ] );   /* Service or char */

            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( HIGH_LAYER_COMPAT_IE, CODESET_0 );
            }
#endif        /* EXTENDED_HOST_MSG */

         /* *********** Consider USER_USER_IE (optional) ****************
         */
         switch ( SwitchType )
            {
            case PRI_DMS100_STYPE:
               break;                                           /* NOP */
#ifdef PRI_1TR6
            case PRI_1TR6_STYPE:
               break;                                           /* NOP */
#endif
            case VN2_STYPE:
               break;
            default:                    /* AT&T is the default for now */
               CCPMSG_AppendHostUUI();
               break;
            }
         }
#ifdef EXTENDED_HOST_MSG
      if ( SwitchType == PRI_5ESS_STYPE )
         {
         CCPRI_CopyPublic( CODESET_SHIFT_IE, CODESET_0 );  /* To set 6 */
         CCPRI_CopyPublic( USER_CODE_IE, CODESET_6 );
         CCPRI_CopyPublic( TRAVEL_CLASSMARK_IE, CODESET_6 );
         }
#endif

      /* Support for additional Codeset 6 & 7 elements should be added here
      *  if required.
      */
      CCPRI_Mail( L3_ID, FALSE );             /* Send the CC_SETUP_REQ */

out_call_failed:
      if ( cause )  /* A cause of zero means everything went correctly */
         {
         if ( OutPkt )           /* A half-built CC_SETUP_REQ message? */
            {
            LIF_FreePkt( OutPkt );
            OutPkt = NULL;
            }
         CCPMSG_RejectOutCall( cause, FALSE ); /* Send failure to HOST */
         success = FALSE;
         }
      }
   return( success );
}
#ifdef PRI_1TR6
/************************
* CCPMSG_GetHostChannel *
*************************
*
* Extract and validate the Host Channel selection
*
*/
PUBLIC int  CCPMSG_GetHostChannel()
{
   int   chan;                                         /* Host channel */
   int   int_id;
   int   cause = NOERR;

   if (( IE = CCPRI_GetPrivate( CHANNEL_ID_IE, CODESET_0 )) != NULL )
      {
      IE_Fields = NULL;
      chan      = IE[ 4 ];
      int_id    = ( MultiInterface ) ? IE[ 5 ] : 0;
      CCB->Exclusive = IE[ 2 ];
      }
#ifdef EXTENDED_HOST_MSG
   else if (( IE = CCPRI_GetPublic( CHANNEL_ID_IE, CODESET_0 )) != NULL )
      {
      IE_Fields = CCIE_ExpandChanId_IE( IE, &Trashcan );
      chan      = IE_Fields[ 9 ];
      int_id    = ( IE_Fields[ 0 ] && MultiInterface ) ?
                     IE_Fields[ IE_Fields[ 8 ] ] : 0;
      CCB->Exclusive = IE_Fields[ 2 ];
      }
#endif
   else
      {
      cause = MANDATORY_IE_MISSING;
      }
   /* Check for different channel numb or same channel in a different
   *  interface.
   */
   if (( CCB->B_Chan != chan ) || ( CCB->Int_Id[ 0 ] != int_id ))
      {
      /* We are required to use the network's channel offering.
      */
      cause = REQ_CHANNEL_NOT_AVAIL;
      }
   else
      {
      CCB->B_Chan      = chan;                            /* Save chan */
      CCB->Int_Id[ 0 ] = int_id;                       /* Interface ID */
      }

   return cause;
}

#endif
/**********************
* CCPMSG_RejectInCall *
***********************
*
* Reject an incoming call in its setup due to flaws or incompatibilities in
* the associated message received from the peer.  This sequence constitutes
* the first response to the CC_SETUP_IND, thus the CC_BROADCAST_RESP must
* be sent first (USER side only).  There may or may not be a CALL_REJECTION
* message driving this function.  If cause is non-NULL, no CALL_REJECTION
* message is present.
*
*/
PUBLIC void CCPMSG_RejectInCall( cause, ie_id )

int cause;                                     /* Reason for rejection */
int ie_id;            /* Information element type causing call reject. */
{

   if ( CCPRI_GetOutPkt( CC_BROADCAST_RESP ))
      {
      CCPRI_Mail( L3_ID, FALSE );               /* Send BROADCAST_RESP */
      }
   if ( CCPRI_GetOutPkt( CC_SETUP_REJ_REQ )) /* Instigate RELEASE_COMP */
      {
      if ( cause )
         {
         Diag[ 0 ] = ie_id;
         OutPkt->InfoLen +=                      /* Build the message. */
            CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                cause,
                                AM_I_THE_NETWORK_SIDE,
                                Diag );
         }
#ifdef EXTENDED_HOST_MSG
      else if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
#endif
      else
         {
         if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
            {
            Diag[ 0 ] = ie_id;
            OutPkt->InfoLen +=                   /* Build the message. */
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   IE[ 2 ],
                                   AM_I_THE_NETWORK_SIDE,
                                   Diag );
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CAUSE_IE, CODESET_0 );
            }
         CCPRI_CopyPublic( USER_USER_IE, CODESET_0 );
#endif
         }
      CCPRI_Mail( L3_ID, FALSE );             /* Send CC_SETUP_REJ_REQ */
      }

   return;
}

/*******************
* CCPMSG_RejectMsg *
********************
*
* Reject a message received from Layer 3 due to flaws in its contents.
* The Protocol Control machine may have advanced its state incorrectly
* because the information elements were not rigorously examined at that
* level.  This function will cause Protocol Control to return to its
* previous L3 state as specified in the CC_CHNG_STATE_REQ message and to
* send a STATUS message to the Network with the info elements as supplied
* in the CC_STATUS_REQ message generated here if a non-zero cause value
* is supplied to this function.
*
*/
PUBLIC void CCPMSG_RejectMsg( cause, ie_id )

int     cause;                                 /* Reason for rejection */
int     ie_id;         /* Information element type causing msg reject. */

{
   if ( CCPRI_GetOutPkt( CC_CHNG_STATE_REQ ))
      {
      /* Protocol Control is issued a message to correct its state
      *  in the event it has changed states inappropriately.  Its new
      *  state should be set to the state it reported with this msg.
      */
      OutPkt->Info[ OutPkt->InfoLen++ ] = SourceState;
      /* Mail the change state msg
      */
      CCPRI_Mail( L3_ID, FALSE );
      switch ( SwitchType )
         {
#ifdef PRI_1TR6
         case PRI_1TR6_STYPE:
            if ( CCPRI_GetOutPkt( CC_RELEASE_REQ ))
               {
               OutPkt->InfoLen +=
                   CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                     cause,
                                     AM_I_THE_NETWORK_SIDE,
                                     0 );
                  /* Mail the RELEASE_REQ
                   */
               CCPRI_Mail( L3_ID, FALSE );
               }
            break;
#endif
         default:    /* Function not supported for default SwitchTypes */
            if ( cause )
               {
               if ( CCPRI_GetOutPkt( CC_STATUS_REQ ))
                  {
                  Diag[ 0 ] = ie_id;
                  OutPkt->InfoLen +=
                    CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                      cause,
                                      AM_I_THE_NETWORK_SIDE,
                                      Diag );
                  OutPkt->InfoLen +=
                       CCIE_BuildCallState_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                              SourceState );
                  /* Mail the CC_STATUS_REQ
                   */
                  CCPRI_Mail( L3_ID, FALSE );
                  }
               }
            break;
         }
      }
   return;
}

/***********************
* CCPMSG_RejectOutCall *
************************
*
* Build and send the CALL_REJECTION message to the HOST.
*
*/
PUBLIC void CCPMSG_RejectOutCall( cause, look_for_uui )

int     cause;                                 /* Reason for rejection */
BOOLEAN look_for_uui;     /* Look in the InPkt for UUI to send to HOST */

{
   if ( !cause )
      {
      if ( CCPRI_AcceptCause( &cause ) != ACCEPTED )
         cause = NORMAL_CLEARING;
      }

   if ( CCPRI_GetOutInfo( CALL_REJECTION ) )
      {
      OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;      /* Build CAUSE_IE */
      OutPkgInfo[ OutPkgInfoLen++ ] = 1;
      OutPkgInfo[ OutPkgInfoLen++ ] = cause;
      switch ( SwitchType )
         {
         case PRI_4ESS_STYPE:
         case PRI_5ESS_STYPE:
            if ( look_for_uui )
               CCPRI_AcceptUserUser();
            break;
         default:
            Trashcan = look_for_uui;      /* Prevent compiler warning. */
            break;
         }
      CCPRI_Mail( HOST_ID, TRUE );  /* Send CALL_REJECTION to the HOST */
      }

   return;
}

/************************
* CCPMSG_RejectResponse *
*************************
*
* Reject a message when the appropriate response is to abandon the call
* by initiating RELEASE_COMP.
*
*/
PUBLIC void CCPMSG_RejectResponse( cause, diagnostic )

int cause;                                        /* Reason for reject */
int diagnostic;                           /* Supporting data for cause */
{
   if ( CCPRI_GetOutPkt( CC_REL_COMP_REQ ))
      {
      if ( !cause )
         cause = NORMAL_CLEARING;
      Diag[ 0 ] = diagnostic;
      Diag[ 1 ] = 0;
      OutPkt->InfoLen +=
         CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                             cause,
                             AM_I_THE_NETWORK_SIDE,
                             Diag );
      CCPRI_Mail( L3_ID, FALSE );
      }

   return;
}

/********************
* CCPMSG_ReleaseInd *
*********************
*
* Special switch-specific handling for the CC_RELEASE_IND message
* (RELESAE received at Layer 3).
*
* For most switch types, it is sufficient to simply inform the HOST and to
* clear the call.
*
* In the DMS-100 world, the arrival of CC_RELEASE_IND requires that Call 
* Control return a CC_REL_COMP_REQ to Layer 3, which is expecting it in 
* State 19, to motivate the sending of RELEASE_COMP.  Then Call Control 
* must inform the HOST and clear the call.
*
*/
PUBLIC void CCPMSG_ReleaseInd()
{

   if ( SwitchType == PRI_DMS100_STYPE )
      {
      if ( CCPRI_GetOutPkt( CC_REL_COMP_REQ ))
         CCPRI_Mail( L3_ID, FALSE );
      }
   CCPMSG_SendHostDisc( 0, TRUE );
   CCPRI_ReleaseCall();

   return;
}

/*****************
* CCPMSG_Restart *
******************
*
* Process the CC_RESTART_IND and CC_RESTART_CONF messages.
* This function is not uniformly supported by the various switch types.
*
*/
PUBLIC void CCPMSG_Restart (event,dsl)

int event;       /* CC_RESTART_IND or CC_RESTART_CONF */
int dsl;         /* interface */
{
   BOOLEAN  do_all;                                   /* All channels? */
   BYTE    *b_chan;             /* Intermediate B-channel list pointer */
   int      restart_class;                /* Type of restart requested */
   int      int_id;         /* Interface ID of channel being restarted */
   int      b_channel;           /* Individual channel being restarted */
   BYTE    *count;         /* Pointer to call counter octet in OutInfo */
   BYTE     cause;                    /* Cause for bad restart message */
   BYTE     valid_ie;
   int      bchan_state;           /* used to see if B-channel defined */
   int      ix;
   int      bcount;                      /* count how many channel IEs */
   BOOLEAN  struct_err;                         /* IE structural error */
   BOOLEAN  restart_err;                  /* NET5, error in restart ie */

   /* Look for the Restart Ind info element.
   */

   do_all = FALSE;
   int_id = 0;
   count = 0;
   cause = 0;
   struct_err = FALSE;
   ix = 0;
   bcount = 0;
   restart_err = FALSE;

   if(detail_debug)
      buginf("\nreceived a RESTART, in CCPMSG_Restart routine");
   if ((IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
                         RESTART_IND_IE, CODESET_0, 1)) == NULL) {
      if (detail_debug)
          buginf("\n CCPMSG_Restart restart_failed man ie miss");
      /*
       * for Australia PRI ignore the Restart with missing
       * Restart indicator ie.
       */
      if (SwitchType != PRI_TS014_STYPE) {
          cause = MANDATORY_IE_MISSING;                        /* Cause 96 */
          Diag[ 0 ] = RESTART_IND_IE;
          Diag[ 1 ] = 0;
      }
      goto restart_failed;
   } else if (( SwitchType == VN2_STYPE ) && ( IE[ 1 ] != 1 )) {
      cause = MANDATORY_IE_LEN_ERR;                       /* Cause 103 */
      Diag[ 0 ] = RESTART_IND_IE;
      Diag[ 1 ] = 0;
      goto restart_failed;
   } else {
      restart_class = IE[ 2 ] & 0x07;         /* Recover restart class */
      if (SwitchType == PRI_NET5_STYPE) {
          /*
           * For Italy have to check the extend bit.
           */
          if ((IE[2] & 0xF8) != 0x80)
              restart_err = TRUE;
      }
      if (detail_debug)
          buginf("\n CCPMSG_Restart restart_class is %x, error=%x",
                 restart_class, restart_err);
   }

   IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen,
                    CHANNEL_ID_IE, CODESET_0, 1);
   if (detail_debug)
       buginf("\n CCPMSG_Restart getting ie");
   if (IE) {                                  /* Channel ID present? */
      if (detail_debug)
          buginf("\n CCPMSG_Restart start to expand");
      bcount = 1;   /* at least 1 Channel ie */
      IE_Fields = CCIE_ExpandChanId_IE( IE, &struct_err );
      if (IE_Fields[0]) {                 /* Interface ID present? */
         /* A single octet interface ID is assumed here as per AT&T.
         */
         int_id = IE_Fields[ IE_Fields[ 8 ] ];
         if ((struct_err) || (!VALID_INT_IDP(int_id))) { /* IntID OK? */
            cause = INVALID_ELEM_CONTENTS;                /* Cause 100 */
            Diag[0] = CHANNEL_ID_IE;
            Diag[1] = 0;
            goto restart_failed;
         }
      }
   }

   switch (SwitchType) {
      case PRI_4ESS_STYPE:
      case PRI_5ESS_STYPE:
         /* Note that PRI_5ESS is ambiguous.  Only one restart class
         *  is defined (Indicated B-Channel), yet the absence of
         *  the CHANNEL_ID_IE clearly implies RESTART_ALL.  But in
         *  this case the restart class appears to be meaningless, at
         *  best, or contradictory at worst.
         */
         if (restart_class != RESTART_SINGLE) {
            /* This is the only legal value defined for this field, thus
             * any other value must be treated as an element coding error.
             */
            cause = INVALID_ELEM_CONTENTS;                /* Cause 100 */
            Diag[0] = RESTART_IND_IE;
            Diag[1] = 0;
            goto restart_failed;
         } else if (!IE)
            /* In the presence of CHANNEL_ID_IE, we assume that some subset
             * of the channels will be restarted.
             */
            do_all = TRUE;
         break;
      case PRI_NTT_STYPE:
      case PRI_NET5_STYPE:
         /*
          * Verify the restart class as well as the extend bit for the class,
          * if not set, send a STATUS. The extend bit is tested in Italy.
          */ 
         if (((restart_class != RESTART_SINGLE) &&
             (restart_class != RESTART_SINGLE_INTERFACE) &&
             (restart_class != RESTART_ALL)) || (restart_err)) {
            /*  This is the only legal value defined for this field, thus
             *  any other value must be treated as an element coding error.
             */
            if (detail_debug)
                buginf("\n CCPMSG_Restart not restart single");
            cause = INVALID_ELEM_CONTENTS;                /* Cause 100 */
            Diag[0] = RESTART_IND_IE;
            Diag[1] = 0;
            goto restart_failed;
         } else if (!IE) {
            /* In the presence of CHANNEL_ID_IE, we assume that some subset
             * of the channels will be restarted.
             */
            if (detail_debug)
                buginf("\n CCPMSG_Restart setting do_all true");
            do_all = TRUE;
         }
         break;
      case PRI_TS014_STYPE:
         if ((restart_class != RESTART_SINGLE) &&
             (restart_class != RESTART_ALL)) {
            /*  This is the only legal value defined for this field, thus
             *  any other value must be treated as an element coding error.
             */
            if (detail_debug)
                buginf("\n CCPMSG_Restart not restart single");
            /*
             * Just ignore the Restart if invalid class.
             */
            goto restart_failed;
         } else {
            if (!IE) {
            /* In the presence of CHANNEL_ID_IE, we assume that some subset
             *  of the channels will be restarted.
             */
            if (detail_debug)
                buginf("\n CCPMSG_Restart setting do_all true");
            do_all = TRUE;
            }
         }
         break;
      case PRI_DMS100_STYPE:
         if (restart_class == RESTART_ALL)
             do_all = TRUE;
         else if (restart_class != RESTART_SINGLE) {
            /* Coding error.  Only two choices.
             */
            cause = INVALID_ELEM_CONTENTS;                /* Cause 100 */
            Diag[0] = RESTART_IND_IE;
            Diag[1] = 0;
            goto restart_failed;
         }

         else if (!IE) {
            /* restart_class = RESTART_SINGLE.  The channel is to be
            *  indicated through the CHANNEL_ID_IE, which is missing.
            *  Its absence is a fatal error.
            */
            cause = MANDATORY_IE_MISSING;                  /* Cause 96 */
            Diag[0] = CHANNEL_ID_IE;
            Diag[1] = 0;
            goto restart_failed;
         }
         break;
      case VN2_STYPE:      /* Restart supported for DP Equipment only. */
         if ( struct_err ) {          /* Length error in CHANNEL_ID_IE */
            cause = MANDATORY_IE_LEN_ERR;                 /* Cause 103 */
            Diag[0] = CHANNEL_ID_IE;
            Diag[1] = 0;
            goto restart_failed;
         } else if ((restart_class == RESTART_ALL) ||
             (restart_class == RESTART_SINGLE_INTERFACE)) {
            do_all = TRUE;
         } else if (restart_class != RESTART_SINGLE) {
            /* Coding error.
            */
            cause = INVALID_ELEM_CONTENTS;                /* Cause 100 */
            Diag[0] = RESTART_IND_IE;
            Diag[1] = 0;
            goto restart_failed;
         } else if (!IE) {
            /* restart_class = RESTART_SINGLE.  The channel is to be
            *  indicated through the CHANNEL_ID_IE, which is missing.
            *  Its absence is a fatal error.
            */
            cause = MANDATORY_IE_MISSING;                  /* Cause 96 */
            Diag[0] = CHANNEL_ID_IE;
            Diag[1] = 0;
            goto restart_failed;
         }
         break;
#ifdef PRI_1TR6                                                /* 1TR6 */
      case PRI_1TR6_STYPE:
            /* Prevents "unreachable code" compiler warning
             */
         break;
#endif
      default:
         if (detail_debug)
             buginf("\n CCPMSG_Restart func_not_supported");
         goto func_not_supported;
   }

   if (detail_debug)
       buginf("\n CCPMSG_Restart is in rest ind");
   if (event == CC_RESTART_IND ) {
       /*
        * Italy homologation has forces some rework of the Restart stuff. Keeping it
        * separate because it could adversely affect other switchtypes.
        */
       if (SwitchType == PRI_NET5_STYPE) {
           if (detail_debug)
               buginf("\n CCPMSG_Restart in check IEs");
            /*
             * Have to check for unimplemented IEs in the message.
             * This is for Italy homologation. Also count the number of Channel
             * information elements.
             */
            valid_ie = CCPCC_VerifyRestartIEs(InPkt->Info, InPkt->InfoLen, &bcount);
            if (detail_debug)
                buginf ("\n ret from VerifyRestart valid_ie is %x", valid_ie);
            if (valid_ie != NOERR) {
                Diag[0] = 0;
                if (valid_ie & 0x80) {
                    /*
                     * Something wrong with the incoming message, but send the status
                     * and continue since it it not a mandatory ie
                     */
                    cause = BAD_INFO_ELEM;        /* cause 99 */
                    Diag[0] = valid_ie;
                    Diag[1] = 0;
                    CCPMSG_SendPeerStatus(cause);
                    cause = NOERR;           /* reset cause, so not sent out later */
               } else {
                   cause = MANDATORY_IE_MISSING;  /* cause 96 */
                   goto restart_failed;
               }
            }
            if (detail_debug)
                buginf ("\n CCPMSG_Restart checking net5 RES ACK REQ");
            /*
             * For NET5 just reuse the incoming Restart message. But must
             * verify the IEs and change message type before sending it back out.
             */
            CCPMSG_VerifyResendRestart();

            count  = &OutPkt->Info[ OutPkt->InfoLen++ ];
            *count = 0;           /* Initialize */
      } else {
      /*
       * not NET5, The far end sent RESTART.  Prepare to send RESTART_ACK.
       */
      if (( OutPkt = LIF_GetPkt( LRG_INFO_BTYPE, DATA_OFFSET, TRUE )) != NULL ) {
         LIF_CpyPkt( OutPkt, InPkt );/* Reuse PD, Call Ref & other IEs */
         CCIE_Change_Msg_Type( OutPkt->Info, CC_RESTART_ACK_REQ );
         OutPkt->InfoType = LRG_INFO_BTYPE;
         OutPkt->Info[ OutPkt->InfoLen++ ] = FALSE;
         count  = &OutPkt->Info[ OutPkt->InfoLen++ ];
         *count = 0;                                     /* Initialize */
      } else {
         CCPRI_Error( CCPMSG_RESTART, P_OUT_OF_PKTS, 0, TRUE );
      }
      }
   } else {
      count = NULL;
   }
   /*
    * only clear out every thing if we get a RESTART,  not on an ACK. Just
    * accept the RESTART_ACK and don't do much else. Was causing existing calls
    * to be dropped.
    */
   if (event == CC_RESTART_IND) {
       if (do_all) {
          for (b_channel = MIN_CHANNEL; b_channel <= MAX_CHANNELP; b_channel++) {
             CCPMSG_RestartChan(int_id, b_channel, NULL);
             /*
              * now clear out the nlcb
              */
             L3_Restart_nlcb(dsl);
          }
          if(detail_debug)
              buginf("\nCCPMSG_Restart...ALL channels restarted");
       } else {
          /*
           * NET5 will put multiple Channel_IEs in the Restart, most other switchtypes
           * don't do this yet. The CCPCC_VerifyRestartIEs routine will count them
           * for NET5. The others are checked when a channel ie is looked for.
           */ 
          if (bcount > 0) {
              for (ix = 1; ix <= bcount; ix++) {
                  /*
                   * This will find the ix'th Channel_ie. NET5 likes to put multiple
                   * Channel_ies in the Restart.
                   */
                  IE = CCIE_Get_IE(InPkt->Info, InPkt->InfoLen, CHANNEL_ID_IE,
                                   CODESET_0, ix);
                  if (IE) {                                  /* Channel ID present? */
                     if (detail_debug)
                         buginf("\n CCPMSG_Restart start to expand, ix=x", ix);
                     IE_Fields = CCIE_ExpandChanId_IE( IE, &struct_err );
                     if (IE_Fields[0]) {                 /* Interface ID present? */
                        /* A single octet interface ID is assumed here as per AT&T.  */
                        int_id = IE_Fields[ IE_Fields[ 8 ] ];
                        if ((struct_err) || (!VALID_INT_IDP(int_id))) { /* IntID OK? */
                            cause = INVALID_ELEM_CONTENTS;              /* Cause 100 */
                            Diag[0] = CHANNEL_ID_IE;
                            Diag[1] = 0;
                            goto restart_failed;
                        }
                     }
                  }
                  /*  Restart individual channel(s).
                   *  Test each condition which will cause us to abort the restart
                   *  from within the CHANNEL_ID_IE.  If any condition is TRUE, restart
                   *  is aborted.
                   */
                  if ((IE_Fields[1] == BASIC_RATE_INTERFACE) ||

                     /* D-Channel identified - not currently supported. */
                      (IE_Fields[3] == 1) ||

                         /* Read info channel selection - determine if the channel(s)
                          * are not indicated in following octets.
                          */
                      (IE_Fields[4] != 1) ||

                         /* Non-CCITT coding standard not currently supported. */
                      (IE_Fields[5] != 0) ||

                         /*
                          * Channel type. H0 and H11 channels not supported. While
                          * the code supports H0 channel use, we only expect restart
                          * to reference B_CHANNEL_UNITS (see 41449 Sec 5.5.2).
                          */
                      (IE_Fields[7] != 3)) {
                         cause = INVALID_ELEM_CONTENTS;               /* Cause 100 */
                         Diag[0] = CHANNEL_ID_IE;
                         Diag[1] = 0;
                         goto restart_failed;         /* Don't perform the restart */
                  }

                  b_chan = &IE_Fields[9];
                  /*
                   * Have to check the range of channels allowed. Do not allow channel
                   * 25 if only 24 are defined. AT&T found this problem after E1
                   */
                  while (*b_chan) {               /* Could be a list of channels */
                      if (detail_debug)
                          buginf("\nRestarting channel 0x%x", *b_chan);
                      bchan_state = CC_CHAN_GetStatepri(int_id, *b_chan, B_CHANNEL_UNITS,
                                                        Dsl_Id);
                      if ((!VALID_CHANNELP(*b_chan)) ||
                          (bchan_state == CHANNEL_RESERVED)) {
                          cause = INVALID_ELEM_CONTENTS;                /* Cause 100 */
                          Diag[0] = CHANNEL_ID_IE;
                          Diag[1] = *b_chan;
                          /*
                           * Some NET5 switches will send an invalid B-channel, the
                           * other B-channels will not get restarted.
                           */
                          b_chan++;
                      } else if (event == CC_RESTART_CONF) {
                          /* Test if we really initiated the restart. */
                          if(CC_CHAN_GetTimerID(int_id, *b_chan, Dsl_Id) != NOT_TIMING) {
                              CCPMSG_RestartChan(int_id, *b_chan, count);
                              b_chan++;
                          } else
                             b_chan++;                                  /* Next chan */
                      } else {
                          CCPMSG_RestartChan(int_id, *b_chan, count);
                          b_chan++;
                      }
                  }
              }
          }
       }
   }

   if (detail_debug)
       buginf("\n CCPMSG_Restart OutPkt is %x", OutPkt);
   if (OutPkt) {
      CCPRI_Mail( L3_ID, FALSE );           /* Mail CC_RESTART_ACK_REQ */
   }

restart_failed:
   if (OutPkt) {
      LIF_FreePkt( OutPkt );           /* Release the incoming packet. */
      OutPkt = NULL;
   }
   if (cause) {
      /* Something wrong with the incoming message */
      CCB = CCPRI_GetDummyCcb( 0 );
      CCB->Call_ID = 0;
      CCB->PriState = NULL;
      CCB->CallRefLen = InPkt->Info[ 1 ];
      if (CCB->CallRefLen == 1)
         CCB->CallRef = InPkt->Info[ 2 ];
      else                                      /* 2 octet global cref */
         CCB->CallRef = (InPkt->Info[ 2 ] << 8) + InPkt->Info[ 3 ];

      if (CCPRI_GetOutPkt(CC_STATUS_REQ)) {
         OutPkt->InfoLen +=
            CCIE_BuildCause_IE(&OutPkt->Info[ OutPkt->InfoLen ],
                                cause, AM_I_THE_NETWORK_SIDE, Diag);
         OutPkt->InfoLen +=
            CCIE_BuildCallState_IE(&OutPkt->Info[ OutPkt->InfoLen ], 0);
         CCPRI_Mail(L3_ID, FALSE);
      }
   }

func_not_supported:
   return;
}

/*********************
* CCPMSG_RestartChan *
**********************
*
* Restart an individual B-Channel.
* This will also put a channel back into "in-service".
*
*/
PRIVATE void near CCPMSG_RestartChan( int_id, b_chan, count )

int   int_id;               /* Interface ID of channel to be restarted */
int   b_chan;                               /* Channel to be restarted */
BYTE *count;                  /* Count of calls cleared by the RESTART */
{
   int     timer_id;               /* Service message timer identifier */
   CCB_ID  ccb;                    /* Service message timer identifier */

   if(detail_debug)
      buginf("\nentering CCPMSG_RestartChan, restarting channel %d", b_chan);

   /* The D-channel is distinguished in the table as a RESERVED channel.
   */
   if ( CC_CHAN_GetStatepri( int_id, b_chan,
                          B_CHANNEL_UNITS, Dsl_Id ) == CHANNEL_RESERVED )
      {
      goto not_the_d_chan;
      }

   /* Search for a CCB associated with this channel.
   */
   CCB = CCB_Head[ Dsl_Id ];
   while ( CCB )
      {
      ccb = CCB->Next;                                    /* Save next */
      if ( CCB->Int_Id[ 0 ] == int_id )
         {
         /* Test if the B_channel being restarted is a B-channel which is
          * in direct use. This will probably have to change when multirate
          * calls are supported.
          */
         if ((CCB->B_Chan) && (b_chan == CCB->B_Chan))
            {
            /* Assemble impacted call_ids list at the end of the
            *  CC_RESTART_ACK_REQ.  This list will be consulted by L3 so
            *  it may release NLCBs.  If count == NULL, this indicates that
            *  the CC_RESTART_ACK_REQ will not be sent because the restart
            *  was initiated locally and we are processing peer RESTART_ACK.
            *  In this case, L3 has already cleared the call context(s).
            */
            if ( count )
               {
               *count += 1;
               OutPkt->Info[ OutPkt->InfoLen++ ] = 
                  (BYTE)( CCB->Call_ID >> 8 );
               OutPkt->Info[ OutPkt->InfoLen++ ] = 
                  (BYTE)  CCB->Call_ID;/* Call_ID LSB */
               }
            CCPMSG_SendHostDisc( TEMPORARY_FAILURE, FALSE );
            CCPRI_ReleaseCall();
            }
         }
      CCB = ccb;                                /* Advance to next CCB */
      }

   /*
    * Remove any assignment and put the channel back into service.
    */
   CC_CHAN_ReleaseChanpri(int_id, b_chan, B_CHANNEL_UNITS, Dsl_Id);
   CC_CHAN_SetServState(int_id, b_chan, B_CHANNEL_UNITS, IN_SERVICE, Dsl_Id);

   /* Go ahead and clear the channel timer.
   */
   if (( timer_id = CC_CHAN_GetTimerID( int_id,
                                        b_chan,
                                        Dsl_Id )) != NOT_TIMING )
      {
      LIF_StopTimer( timer_id );
      LIF_RemoveTimer( timer_id );
      CC_CHAN_SetTimerID( int_id, b_chan, NOT_TIMING, Dsl_Id );
      }

   CCPMSG_SendHostChanStat( int_id, b_chan, CHAN_RESTARTED );/* Tell HOST */
   if(detail_debug)
   	  buginf("\nCCPMSG_RestartChan...channel %d restarted", b_chan );

not_the_d_chan:
   return;
}

/********************
* CCPMSG_RestartReq *
*********************
*
* Initiate B-channel restart because of a second timeout of T308 at
* Protocol Control, a HOST restart request or T316 Expiry.
*
* It is assumed by this function that CCB->B_Chan is valid and that
* CCB->ChanType is in B_CHANNEL_UNITS (i.e. a single B-channel).
*
*/
PUBLIC void CCPMSG_RestartReq( int_id, b_chan, retry )

BYTE int_id;                    /* Interface ID for channel to restart */
int  b_chan;                                     /* Channel to restart */
int retry;
{
   CCB_ID  ccb;                                    /* Intermediate ccb */
   int     calls;                 /* Offset for impacted calls counter */
   INT16   timer_id;                                /* T316 identifier */


   timer_id = CC_CHAN_GetTimerID( int_id, b_chan, Dsl_Id );
   if ( retry )
      {
      if ( CC_CHAN_GetProtocolpri( int_id, b_chan,
                                B_CHANNEL_UNITS, Dsl_Id ) & 0x80 )
         {
         /* Report the failure to the HOST...give up on further retry.
         *  Note that CCPMSG_SendHostChanStat() will reuse the Dummy CCB.
         */
         if ( timer_id != NOT_TIMING )
            LIF_RemoveTimer( timer_id );
         CC_CHAN_SetTimerID( int_id, b_chan, NOT_TIMING, Dsl_Id );
         CC_CHAN_ReleaseChanpri( int_id, b_chan, B_CHANNEL_UNITS, Dsl_Id );
         CCPMSG_SendHostChanStat( int_id, b_chan, CHAN_RESTART_ERR );
         goto restart_failed;
         }
      else
         {
         /* Flag that one timeout has already occurred.
         */
         CC_CHAN_SetProtocolpri( int_id, b_chan, B_CHANNEL_UNITS,
                              ( CC_CHAN_GetProtocolpri( int_id,
                                                     b_chan,
                                                     B_CHANNEL_UNITS,
                                                     Dsl_Id ) | 0x80 ),
                              Dsl_Id );
         }
      }

   CCB = CCPRI_GetDummyCcb( 1 );         /* Use the duumy to send mail */
   CCB->PriState   = NULL;                                 /* No state */
   CCB->B_Chan     = b_chan;                          /* Channel parms */
   CCB->ChanType   = B_CHANNEL_UNITS;                 /* Channel parms */
   CCB->CallRef    = 0;                       /* Global call reference */
   CCB->CallRefLen = 1;                       /* Call reference length */

   if ( CCPRI_GetOutPkt( CC_RESTART_REQ ) )
      {
      OutPkt->InfoLen +=
         CCIE_BuildChanId_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                              CHANNELS_LISTED,
                              b_chan,
                              B_CHANNEL_UNITS,
                              CHANNEL_NUMBERS,
                              TRUE,                       /* Exclusive */
                              ( MultiInterface ) ? 1 : 0,
                              ( MultiInterface ) ? &int_id : NULL );/* Interface ident */
      OutPkt->InfoLen +=
         CCIE_BuildRestartInd_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                  RESTART_SINGLE );
      OutPkt->Info[ OutPkt->InfoLen++ ] = FALSE;             /* Do_all */
      calls = OutPkt->InfoLen++;  /* Save offset to current end of msg */
      OutPkt->Info[ calls ] = 0;                         /* Initialize */
      ccb = CCB_Head[ Dsl_Id ];  /* Search CCB list for impacted calls */
      while ( ccb )
         {
         /* Prepare call_id list at the end of the message for L3.
         */
         if ( ccb->B_Chan  == b_chan )
            {
            OutPkt->Info[ OutPkt->InfoLen++ ] = ccb->Call_ID >> 8;
            OutPkt->Info[ OutPkt->InfoLen++ ] = ccb->Call_ID;
            OutPkt->Info[ calls ]++;   /* Inc number of impacted calls */
            }
         ccb = ccb->Next;
         }
      CCPRI_Mail( L3_ID, FALSE );               /* Send CC_RESTART_REQ */

      CC_CHAN_SetStatepri( int_id, b_chan, B_CHANNEL_UNITS,
                        CHANNEL_RESTART_PEND, Dsl_Id );
      CC_CHAN_SetUserpri( int_id, b_chan, B_CHANNEL_UNITS, 0, Dsl_Id );

      /* Prepare the channel restart timer.
      */
      if ( timer_id != NOT_TIMING )
         {
         LIF_StopTimer( timer_id );
         if ( LIF_UpdateTimer( timer_id, CC_ID, CC_ID,
                               T316_EXPIRY, Dsl_Id, 0,
                               int_id, b_chan, 0, T316_TimeOut ) != ERR )
            {
            LIF_StartTimer( timer_id );
            }
         else
            {
            CCPRI_Error( CCPMSG_RESTART_REQ, P_UPDATE_TIMER_FAILED, 0, FALSE );
            goto restart_failed;
            }
         }
      else if (( timer_id = LIF_AddTimer( CC_ID, CC_ID, T316_EXPIRY,
                                          Dsl_Id, 0, 0, b_chan,
                                          0, T316_TimeOut )) != ERR )
         {
         CC_CHAN_SetTimerID( int_id, b_chan, timer_id, Dsl_Id );
         LIF_StartTimer( timer_id );
         }
      else
         {
         CCPRI_Error( CCPMSG_RESTART_REQ, P_ADD_TIMER_FAILED, 0, FALSE );
         goto restart_failed;
         }
      }
restart_failed:

   return;
}

/**************************
* CCPMSG_SendHostChanStat *
***************************
*
* Build and send the HOST a CHAN_STATUS message.
* If "dumb" channel management (CHANNEL_TABLE not defined) is active,
* the status will be a little numbed out as well.
*
*/
PUBLIC void CCPMSG_SendHostChanStat( int_id, chan, action )

int int_id;                                        /* The interface ID */
int chan;                                   /* The channel in question */
int action;                           /* Channel action being reported */

{
   int user_call_id;          /* Call ID associated with channel usage */


   /* Use DUMMY CCB since a CCB is needed to send mail and this message
   *  is not necessarily associated with any call.
   */
   CCB = CCPRI_GetDummyCcb( 1 );
   CCB->Call_ID = 0;                                 /* Global Call ID */
   if ( CCPRI_GetOutInfo( CHAN_STATUS ))
      {
      user_call_id = CC_CHAN_GetUserpri( int_id, chan, B_CHANNEL_UNITS, Dsl_Id );
      OutPkgInfo[ OutPkgInfoLen++ ] = CHAN_STATUS_IE;         /* IE ID */
      OutPkgInfo[ OutPkgInfoLen++ ] = (MultiInterface) ? 7 : 6; /* Len */
      OutPkgInfo[ OutPkgInfoLen++ ] = chan;          /* Channel number */
      OutPkgInfo[ OutPkgInfoLen++ ] = action;   /* Reason for this msg */
      OutPkgInfo[ OutPkgInfoLen++ ] = CC_CHAN_GetStatepri( int_id, chan,
                                                        B_CHANNEL_UNITS,
                                                        Dsl_Id );
      OutPkgInfo[ OutPkgInfoLen++ ] = CC_CHAN_GetServState( int_id, chan,
                                                            B_CHANNEL_UNITS,
                                                            Dsl_Id );
      OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) ( user_call_id >> 8 );
      OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) user_call_id;
      if ( MultiInterface )
         OutPkgInfo[ OutPkgInfoLen++ ] = (BYTE) int_id;
      CCPRI_Mail( HOST_ID, FALSE );
      }
   CCB = NULL;

   return;
}

/*************************
* CCPMSG_SendHostCongest *
**************************
*
* Send a CALL_CONGESTION message to the HOST after checking things out.
*
*/
PUBLIC void CCPMSG_SendHostCongest()

{
   int cause;                       /* Cause for unacceptable CAUSE_IE */
   int ie_cause;                      /* Cause recovered from CAUSE_IE */


   IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                     CONGEST_LEVEL_IE, CODESET_0, 1 );
   if ( IE )
      {
      if (( IE[ 0 ] == 0xBF ) || ( IE[ 0 ] == 0xB0 )) /* Legal values? */
         {
         if ( CCPRI_GetOutInfo( CALL_CONGESTION ))
            {
            /* Build CONGESTION_IE with congestion state flagged.
            */
            OutPkgInfo[ OutPkgInfoLen++ ] = CONGESTION_IE;
            OutPkgInfo[ OutPkgInfoLen++ ] = 1;               /* Length */
            OutPkgInfo[ OutPkgInfoLen ] = ( IE[ 0 ] == 0xBF ) ? TRUE : FALSE;
            /* Save congested status.
            */
            CCB->Congested = (BOOLEAN) OutPkgInfo[ OutPkgInfoLen++ ];

            if (( cause = CCPRI_AcceptCause( &ie_cause )) == ACCEPTED )
               {
               OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;            /* Length */
               OutPkgInfo[ OutPkgInfoLen++ ] = ie_cause;
               CCPRI_Mail( HOST_ID, TRUE );
               }
            else
               {
               CCPMSG_RejectMsg( cause, CAUSE_IE );
               CCPRI_FreePkgInfo();
               }
            }
         }
      else
         {
         CCPMSG_RejectMsg( INVALID_ELEM_CONTENTS, CONGEST_LEVEL_IE );
         }
      }
   else
      {
      CCPMSG_RejectMsg( MANDATORY_IE_MISSING, CONGEST_LEVEL_IE );
      }

   return;
}

/**********************
* CCPMSG_SendHostDisc *
***********************
*
* Send the host a CALL_DISC message.
*
*/
PUBLIC void CCPMSG_SendHostDisc( cause, look_for_uui )

int     cause;                                     /* Disconnect cause */
BOOLEAN look_for_uui;     /* Look in the InPkt for UUI to send to HOST */

{
   if ( CCPRI_GetOutInfo( CALL_DISC ))
      {
      OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
      if ( !cause )
         {
         if ( CCPRI_AcceptCause( &cause ) != ACCEPTED )
            cause = NORMAL_CLEARING;                       /* Cause 16 */
         }
      switch ( SwitchType )
         {
         case PRI_4ESS_STYPE:
         case PRI_5ESS_STYPE:
            OutPkgInfo[ OutPkgInfoLen++ ] = 1;
            OutPkgInfo[ OutPkgInfoLen++ ] = cause;
            if ( look_for_uui )
               CCPRI_AcceptUserUser();
            break;
         case VN2_STYPE:
            OutPkgInfo[ OutPkgInfoLen++ ] = 1;
            OutPkgInfo[ OutPkgInfoLen++ ] = cause;
            cause = CCPRI_AcceptNetFac();                   /* Charges */
            if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
               {
               CCPMSG_ErredOptional_IE( NETWORK_FACILITY_IE, cause );
               }
            break;
#ifdef PRI_1TR6
         case PRI_1TR6_STYPE:
               /* LEER_ELEMENT is an arbitrary value that means
               *  normal call clearing in the 1TR6 world ( len = 0 )
               */
            if ( cause == LEER_ELEMENT )
               {
               OutPkgInfo[ OutPkgInfoLen++ ] = 0;
               }
            else
               {
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = cause;
               }
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                              NETWORK_FACILITY_IE, CODESET_0, 1);
            if (IE )
               {
               OutPkgInfo[ OutPkgInfoLen++ ] = NETWORK_FACILITY_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 2;
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[3];
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[4];
               }

            /* Charges are an option.
            */
            CCPRI_GetCharges();

            /* Date is mandatory.
            */
            CCPRI_GetDate();
            break;
#endif
         default:
            Trashcan = look_for_uui;      /* Prevent compiler warning. */
            break;
         }
      CCPRI_Mail( HOST_ID, TRUE );
      }

   return;
}

/**********************
* CCPMSG_SendHostInfo *
***********************
*
* Relay contents of the INFO message to the Host.
*
*/
PUBLIC void CCPMSG_SendHostInfo()
{
   int cause;

   switch ( SwitchType )
      {
      case VN2_STYPE:
         if ( CCPRI_GetOutInfo( CALL_INFO ))
            {
            cause = CCPRI_AcceptNetFac();
            if (( cause != ACCEPTED ) && ( cause != MANDATORY_IE_MISSING ))
               {
               CCPMSG_ErredOptional_IE( NETWORK_FACILITY_IE,
                                        INVALID_ELEM_CONTENTS );
               }
            else
               {
               CCPRI_Mail( HOST_ID, TRUE );          /* Send CALL_INFO */
               }
            }
         break;
      default:
         break;
      }
   return;
}

/**************************
* CCPMSG_SendHostProgress *
***************************
*
* Compose and send the HOST a CALL_PROGRESS message.
*
*/
PUBLIC void CCPMSG_SendHostProgress( progress_desc, look_for_cause,
                                     look_for_uui )

int     progress_desc;                  /* Indicator of the call event */
BOOLEAN look_for_cause; /* Look in the InPkt for CAUSE to send to HOST */
BOOLEAN look_for_uui;     /* Look in the InPkt for UUI to send to HOST */

{
   int cause;                                            /* Peer cause */


   if ( CCPRI_GetOutInfo( CALL_PROGRESS ))
      {
      if (( look_for_cause ) && ( CCPRI_AcceptCause( &cause ) == ACCEPTED ))
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = CAUSE_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = cause;
         }
      if ( progress_desc )
         {
         OutPkgInfo[ OutPkgInfoLen++ ] = PROGRESS_IND_IE;
         OutPkgInfo[ OutPkgInfoLen++ ] = 1;
         OutPkgInfo[ OutPkgInfoLen++ ] = progress_desc;
         }
      switch ( SwitchType )
         {
         case PRI_4ESS_STYPE:
         case PRI_5ESS_STYPE:
            if ( look_for_uui )
               CCPRI_AcceptUserUser();
            break;
#ifdef PRI_1TR6
         case PRI_1TR6_STYPE:
            IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                STATUS_GERUF_TLN_IE, CODESET_6, 1 );
            if ( (IE) && (( IE[2] == KEINE_ANGABE ) || ( IE[2] == TLN_WIRD_GERUFEN )) )
               {
               /* total length of private IE's = 3
               */
               OutPkgInfo[ OutPkgInfoLen++ ] = 3;
               OutPkgInfo[ OutPkgInfoLen++ ] = STATUS_GERUF_TLN_IE;
               OutPkgInfo[ OutPkgInfoLen++ ] = 1;
               OutPkgInfo[ OutPkgInfoLen++ ] = IE[2];
               }
            CCPRI_GetDate();
            break;
#endif
         case VN2_STYPE:
            /* Note that User-User Facilities IE is not explicitly supported,
            *  but the Host will receive this IE among the "public" IEs if
            *  it was sent by the network and if EXTENDED_HOST_MSG has been 
            *  defined in FLAGS.H.
            */
            break;
         default:
            Trashcan = look_for_uui;      /* Prevent compiler warning. */
            break;
         }
      CCPRI_Mail( HOST_ID, TRUE );               /* Send CALL_PROGRESS */
      }

   return;
}

/*********************
* CCPMSG_SendHostUUI *
**********************
*
* Receive peer UUI and relay it to the HOST after integrity checks
* have been passed.
*
*/
PUBLIC void CCPMSG_SendHostUUI()

{
   int cause;                   /* Cause for unacceptable USER_USER_IE */


   if ( !CCB->Congested )       /* The message is discarded otherwise. */
      {
      if ( CCPRI_GetOutInfo( CALL_UUI ))
         {
         switch ( SwitchType )
            {
            case VN2_STYPE:
               if ( OutPkgInfo != NULL )
                  CCPRI_FreePkgInfo();
               break;
            default:
               if (( cause = CCPRI_AcceptUserUser()) != ACCEPTED )
                  {
                  CCPMSG_RejectMsg( cause, USER_USER_IE );
                  CCPRI_FreePkgInfo();
                  }
               else
                  {
                  CCPRI_Mail( HOST_ID, TRUE );
                  }
               break;
            }
         }
      }
   return;
}

/*************************
* CCPMSG_SendPeerCongest *
**************************
*
* Send a CC_CONG_CON_REQ message.
*
*/
PUBLIC void CCPMSG_SendPeerCongest()

{
   if ( CCPRI_GetOutPkt( CC_CONG_CON_REQ ))
      {
#ifdef EXTENDED_HOST_MSG
      if ( Host_IEs_Len == 0 )
         {
         CCPRI_CopyPublicAll();
         }
      else
#endif
         {
         if (( IE = CCPRI_GetPrivate( CONGESTION_IE, CODESET_0 )) != NULL )
            {
            CCB->Congested = IE[ 2 ];
            OutPkt->InfoLen +=
               CCIE_BuildCongLevel_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                       ( CCB->Congested ) ? CONGESTED :
                                                            NOT_CONGESTED );
            }
#ifdef EXTENDED_HOST_MSG
         else if (( IE = CCPRI_GetPublic( CONGESTION_IE, CODESET_0 )) != NULL )
            {
            CCB->Congested = ( IE[ 0 ] == 0xBF ) ? TRUE : FALSE;
            CCPRI_CopyPublic( CONGESTION_IE, CODESET_0 );
            }
#endif
         if (( IE = CCPRI_GetPrivate( CAUSE_IE, CODESET_0 )) != NULL )
            {
            Diag[ 0 ] = NO_DIAG;
            OutPkt->InfoLen +=
               CCIE_BuildCause_IE( &OutPkt->Info[ OutPkt->InfoLen ],
                                   IE[ 2 ],
                                   AM_I_THE_NETWORK_SIDE,
                                   Diag );
            }
#ifdef EXTENDED_HOST_MSG
         else
            {
            CCPRI_CopyPublic( CAUSE_IE, CODESET_0 );
            }
#endif
         }
      CCPRI_Mail( L3_ID, FALSE );
      }

   return;
}

/*********************
* CCPMSG_SendPeerUUI *
**********************
*
*  Send CC_USER_INFO_REQ to the peer.
*
*/
PUBLIC void CCPMSG_SendPeerUUI()

{
   if ( !CCB->Congested )
      {
      if ( CCPRI_GetOutPkt( CC_USER_INFO_REQ ))
         {
#ifdef EXTENDED_HOST_MSG
         if ( Host_IEs_Len == 0 )
            {
            CCPRI_CopyPublicAll();
            }
         else
#endif
            {
#ifdef EXTENDED_HOST_MSG
#ifdef VN2
            /* The USER-USER facilities is not currently supported
            *  as a "private" IE and must be presented by the Host in
            *  "public" presentation.
            */
            CCPRI_CopyPublicShifted( USER_USER_FAC_IE, CODESET_6, FALSE );
#endif                                                          /* VN2 */
            /* Not supported as a private IE
            */
            CCPRI_CopyPublic( MORE_DATA_IE, CODESET_0 );
#endif
            CCPMSG_AppendHostUUI();
            }
         CCPRI_Mail( L3_ID, FALSE );
         }
      }
   return;
}

/************************
* CCPMSG_SendPeerStatus *
*************************
*
* Send a Status message to other side, NET5 useage, received an
* invalid IE in an incoming Restart message.
*
*/
PRIVATE void CCPMSG_SendPeerStatus (cause)
BYTE cause;
{
    CCB = CCPRI_GetDummyCcb( 0 );
    CCB->Call_ID = 0;
    CCB->PriState = NULL;
    CCB->CallRefLen = InPkt->Info[ 1 ];
    if (CCB->CallRefLen == 1)
        CCB->CallRef = InPkt->Info[ 2 ];
    else  /* 2 octet global cref */
        CCB->CallRef = (InPkt->Info[ 2 ] << 8) + InPkt->Info[ 3 ];

    if (CCPRI_GetOutPkt(CC_STATUS_REQ)) {
        OutPkt->InfoLen += CCIE_BuildCause_IE(&OutPkt->Info[OutPkt->InfoLen],
                                              cause, AM_I_THE_NETWORK_SIDE, Diag);
        OutPkt->InfoLen += CCIE_BuildCallState_IE(&OutPkt->Info[OutPkt->InfoLen], 0);
        CCPRI_Mail(L3_ID, FALSE);
    }
}

/*****************************
* CCPMSG_VerifyResendRestart *
******************************
*
* Take the incoming Restart message and use that message for the outgoing
* Restart Acknowledge. First have to verify that the IEs are allowed, keep
* only those that are, get rid of the others (i.e. Display). Needed for
* Italy NET5 homologation, they really test the Restart message.
*/
PRIVATE void CCPMSG_VerifyResendRestart ()
{
    int  ix;
    int  index;
    int  current_offset;
    BYTE current_ie;
    int  length;

    index = 0;
    current_offset = 0;
    length = 0;
    if (CCPRI_GetOutPkt(CC_RESTART_ACK_REQ)) {
        /*
         * For NET5 just reuse the incoming Restart message. But must
         * verify the IEs and change message type before sending it back out.
         */
        OutPkt->HeadLen = InPkt->HeadLen;
        OutPkt->InfoLen = InPkt->InfoLen;
        OutPkt->InfoType = InPkt->InfoType;
        if (InPkt->Header)
            LIF_N_Cpy (OutPkt->Header, InPkt->Header, InPkt->HeadLen);
        OutPkt->InfoType = CC_RESTART_ACK_REQ;
        OutPkt->Info[index++] = Qdot931;      /* Protocol discriminator 0x08 */
        /*
         * call reference
         */
        OutPkt->Info[1] = InPkt->Info[1];
        if (InPkt->Info[1] == 1) {
            OutPkt->Info[2] = InPkt->Info[2] | 0x80;
            OutPkt->Info[3] = CC_RESTART_ACK_REQ;
            OutPkt->InfoLen = 4;  /* Leave space for L3 header */
        } else {
            OutPkt->Info[2] = InPkt->Info[2] | 0x80;
            OutPkt->Info[3] = InPkt->Info[3];
            OutPkt->Info[4] = LRG_INFO_BTYPE;
            OutPkt->InfoLen = 5;/* Leave space for L3 header */
        }
        /*
         * now copy body, but verify before copying,
         * uses InPkt as the source and Output as the destination.
         */
        if (InPkt->Info) {
            index = OutPkt->InfoLen;
            current_offset = CCIE_LenOf_L3_Msg_Header(InPkt->Info);
            while (current_offset < InPkt->InfoLen) {
                current_ie = InPkt->Info[current_offset];
                switch (current_ie) {
                    case CHANNEL_ID_IE:
                    case RESTART_IND_IE:
                        OutPkt->Info[index++] = InPkt->Info[current_offset++]; /*ie*/
                        length = InPkt->Info[current_offset++];
                        OutPkt->Info[index++] = length;
                        for (ix = 0; ix < length; ix++) {
                            OutPkt->Info[index++] = InPkt->Info[current_offset++];
                        }
                        break;
                    default:
                        /*
                         * Invalid IEs for the Restart message, just skip past.
                         * single octet case
                         */
                        if (current_ie & 0x80) {
                            current_offset++;
                        } else {
                            /*
                             * variable length octet
                             */
                            current_offset++;
                            current_offset += InPkt->Info[current_offset] + 1;
                        }
                        break;
                }
            }
            OutPkt->InfoLen = index;
        }
        CCIE_Change_Msg_Type(OutPkt->Info, CC_RESTART_ACK_REQ);
        OutPkt->InfoType = LRG_INFO_BTYPE;
        OutPkt->Info[OutPkt->InfoLen++] = FALSE;
    }
}
/* End of module. */
