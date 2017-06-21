/* $Id: ccbfit.c,v 3.3.42.1 1996/04/30 23:06:47 dclare Exp $ 
 * $Source: /release/112/cvs/Xsys/isdn/ccbfit.c,v $
 *------------------------------------------------------------------
 * ccbfit.c  Call Control Terminal Initialization Procedures 
 *
 * February  1993, Dennis Clare 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: ccbfit.c,v $
 * Revision 3.3.42.1  1996/04/30  23:06:47  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/04  22:11:16  rbeach
 * CSCdi44727:  ISDN MBRI running 5ESS with spids will always use
 * int bri 0 spid value. There was a hardcoded value for the 5ESS
 * switchtype only. It is now dynamic. Also fixed a debug message
 * in l2.c.
 *
 * Revision 3.2  1995/11/17  17:39:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:12:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/28  15:24:25  dclare
 * CSCdi37836:  ISDN BRI switches DMS-100 and NI1 may need only one TEI.
 * One spid configured, use one TEI. Two spids configured, use two TEIs.
 *
 * Revision 2.1  1995/06/07  21:13:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       ccbfit.c                                                 
DESCRIPTION:  Call Control Terminal Initialization Module         
VERSION:      3.32

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

Terminal Initialization functions for North American Basic Rate 
ISDN (SPID/TID/EID) handling are kept here. 5ESS (5e7 Generic Program ), DMS-100,
and National ISDN-1 are supported.  

*************************************************************************
*************************************************************************
*/


#define CCBFIT_C


/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

/* Standard C library functions/definitions
*/
#include    "master.h"
#include    "../ui/debug.h"
#include    "../wan/isdn_debug.h"

#include "flags.h"
#include "tune.h"
#include "gendef.h"
#include "lif.h"
#include "ccie.h"
#include "gp.h"
#include "ccbri.h"
#include "cc_chan.h"
#include "logger.h"
#include "msg_isdn.c"       /* Not a typo, see logger.h */

/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/


/************************************************************************
*                     D A T A   A L L O C A T I O N                     *
*************************************************************************
*/



/************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S              *
*************************************************************************
*/




/************************************************************************
*                   P R I V A T E    F U N C T I O N S                  *
*************************************************************************
*/

PRIVATE void CCBFIT_SendSpid (int, int);


/************************************************************************
*                   P U B L I C    F U N C T I O N S                    *
*************************************************************************
*/
/*************************
* CCBFIT_AcceptEndpointId *
**************************
*
* Find and validate the ENDPOINT ID info element in the incoming message.
*
*/
PUBLIC int CCBFIT_AcceptEndpointId (usid, tid, interpreter)

int *usid;
int *tid;
int *interpreter;

{
   int cause = NOERR;

  switch ( SwitchType ) 
     { 
#ifdef BRI_5ESS
     case BRI_5ESS_STYPE:
        IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
           ENDPOINT_ID_IE, CODESET_6, 1 );
        /* there is no error checking for this info element.  It may be
        *  added later if there is a need for it.
        */
        if ( IE )
           {
           *usid = IE[2];
           *tid  = IE[3] & 0x7F;
           *interpreter = ( ( IE[3] & 0x80 ) == 0x80 ) ? 1 : 0;
           }
         else
            {
            cause = MANDATORY_IE_MISSING;                  /* Cause 96 */
            }
         break;
#endif 
     default:
        IE = CCIE_Get_IE( InPkt->Info, InPkt->InfoLen,
                         EID_IE, CODESET_0, 1 );
        /*
         * There is no error checking for this info element.  It may be
         * added later if there is a need for it.
         */
        if ( IE ) {
           *usid = IE[2] & 0x7F;
           *tid  = IE[3] & 0x3F;
           /*
            * check the length of the eid, allow calls with no tid 20nov
            */
           if (IE[1] >= 2) {
               *interpreter = ( ( IE[3] & 0x40 ) == 0x40 ) ? 1 : 0;
           } else {
               /*
                * Have to override the interpreter bit.
                */
               *interpreter = 1;
           }
         } else {
            cause = MANDATORY_IE_MISSING;                  /* Cause 96 */
         }
        break;
     }

   return ( cause );
}
/*****************
* CCBFIT_GetEpid *
******************
*
* For 5ESS and NI1 FIT Terminals return EPID which is stored in Host Cb
*/
PUBLIC void CCBFIT_GetEpid (usid, tid, epsf, ix)
int *usid;
int *tid;
int *epsf;
int ix;                 /* index of  eid */
{
   H_ID  hostcb;

   hostcb =  &Host_CB [Dsl_ID][ix];

   *usid = hostcb->epid.usid;
   *tid = hostcb->epid.tid;
   *epsf = hostcb->epid.epsf;
}

/*******************
* ccbfit_validspid *
********************
*
* Returns indication if the spid was sent out and valid. Needed for DMS100 bri
* calls, cannot place outgoing calls and should ignoreincoming calls if the spid
* is not valid. This routine should only be called for custom dms100 switches or
* national isdn.  
*/
BOOLEAN ccbfit_validspid (int dsl, int ces)
{
    int  hcb;              /* host control block, 0-1 */
    boolean return_value;

    hcb = ces - 1;
    return_value = TRUE;
    /*
     * For DMS100 custom, a call cannot be sent out on either tei if the
     * spid is not valid. National ISDN allows the terminal to act as a NIT if
     * the spid fails, but a NIT can only have 1 TEI. So for NI, if using the
     * 2nd ces and the spid is not valid, block the call.
     */
    if (Host_CB[dsl][hcb].epid.valid) {
        return_value = TRUE;
    } else {
        if (np_fit[dsl][hcb].spid_sent) {
            if (SwitchType == BRI_NI1_STYPE) {
                if ((ces == SIGNALING_CES) && (!Host_CB[dsl][1].epid.valid)) {
                    /* allow a NIT call */
                    return_value = TRUE;
                } else {
                    /* don't allow any calls */
                    return_value = FALSE;
                }
            } else {
                /* must be a dms custom, don't allow the call */
                return_value = FALSE;
            }
        }
    }
    return(return_value);
}

/*************************
* CCBFIT_TerminalInit    *
**************************
*
*  The several steps involved with SPID/EID exchange are handled here.
*  The parser uses the np_fit data structure, and the isdn code uses
*  the Host_CB data structure.  
*
*/
PUBLIC int CCBFIT_TerminalInit (event, ces, dsl)
int event;                                           /* the main event */
int ces;                       /* Could be from L3 ( ces ) or the Host */
int dsl;                                                        /* DSL */
{
    BYTE event_b; 
    H_ID  hostcb;
    PKT_ID  loc_pkt;    
    int specifier;
    UINT32 msecs;        /* length of FIT timer in msecs */

    /*
     * Normally the global dsl value would be set in the ccbri_go routine. But
     * to force the spid to got out before a setup, this routine may be called
     * from Layer 2. The problem is that Remove_CCB will use the global dsl
     * value instead of the one passed into ccbfit_terminalinit. Set the
     * global value here. A race condition was discovered on Mars platform.
     */
    Dsl_ID = dsl;

    /*
     * The ces range is 1-2 and the specifier range is 0-1, set the
     * variables up right now so can use them later. 
     */
    specifier = ces - 1;

    /*
     * Need to have two different spid timer values. Just do it here for now.
     */
    if (SwitchType == BRI_DMS100_STYPE)
        msecs = FIT_TIMEOUT_DEFAULT_DMS;
    else
        msecs = FIT_TIMEOUT_DEFAULT_NI1;

    hostcb = NULL;
    switch (event) {
        case NL_PU_EST_IND:       /* kicks off initialization at power up */
            hostcb = &Host_CB [dsl][specifier];
            Dsl_ID = dsl; 
            CCB = CCBRI_GetNewCCB(TERM_INITIALIZE, NO_CHANNEL,
                                  SIGNALING_SAPI, hostcb->Ces, 0, 0);
            if (CCB != NULL) {
                LIF_SendPkt(L2P_LAPD, CC_ID, 0, L3_ID, NL_EST_REQ, dsl, Q931_SAPI,
                            hostcb->Ces, 0, 1, 0, FALSE, NULL, NOW, REGULAR_MAIL);
                hostcb->Terminal_State = AWAIT_ESTABLISH;       
                CCB->Host_Specifier = specifier; 
            } else {
                CCB = CCBRI_GetDummyCcb(0 ,hostcb->Ces, SIGNALING_SAPI);
                CCBRI_RejectHostCall(TEMPORARY_FAILURE);
            }
            break;        

        case HOST_CALL_REQUEST:  /* Host Call - we must be initialized first */
            hostcb = &Host_CB [dsl][specifier];
            if (hostcb) {
                hostcb->HostMsg = LIF_GetBuffer(LRG_INFO_BTYPE, TRUE);
                if (hostcb->HostMsg != NULL) {
                    LIF_N_Cpy(hostcb->HostMsg, InPkgInfo, InPkgInfoLen);
                    hostcb->Msglen = InPkgInfoLen; 
                } else {
                    CCBRI_Error(CCBRI_GET_OUT_INFO, OUT_OF_INFO_BUFFERS, 0, 0, TRUE);
                }
                CCB = CCBRI_GetNewCCB(TERM_INITIALIZE, NO_CHANNEL, SIGNALING_SAPI,
                                         hostcb->Ces, 0, 0);
                if (CCB != NULL) {
                    LIF_SendPkt(L2P_LAPD, CC_ID, TERM_INITIALIZE, L3_ID, NL_EST_REQ, dsl,
                          Q931_SAPI, hostcb->Ces, 0,hostcb->Ces, 0, FALSE,
                          NULL, NOW, REGULAR_MAIL);
                    hostcb->Terminal_State = AWAIT_ESTABLISH;       
                    CCB->Host_Specifier = specifier; 
                } else {
                    CCB = CCBRI_GetDummyCcb(0, hostcb->Ces, SIGNALING_SAPI);
                    CCBRI_RejectHostCall(TEMPORARY_FAILURE);
                }
            }  
            break; 

        case HOST_TERM_REGISTER:  /* Host wishes to register with network */
            hostcb = &Host_CB [dsl][specifier];

            switch (hostcb->Terminal_State) {
                case ESTABLISHED:
                    LIF_UpdateTimer(hostcb->Timer_T415 , CC_ID, CC_ID, FIT_TIMEOUT,
                                    dsl, CCB->Sapi, hostcb->Ces, 0, 0, msecs);
                    LIF_StartTimer(hostcb->Timer_T415);
                    /*
                     * US requirements say to only send the spid once.
                     */
                    if (!hostcb->epid.valid)
                        CCBFIT_SendSpid(dsl, specifier);
                    break;
                case AWAIT_ESTABLISH:
                    break;
                case INIT:
                    CCB = CCBRI_GetNewCCB(TERM_INITIALIZE, NO_CHANNEL, SIGNALING_SAPI,
                                          hostcb->Ces, 0, 0);
                    if (CCB != NULL) {
                        LIF_SendPkt(L2P_LAPD, CC_ID, TERM_INITIALIZE, L3_ID, NL_EST_REQ,
                                    dsl, Q931_SAPI, hostcb->Ces, 0, hostcb->Ces, 0,
                                    FALSE, NULL, NOW, REGULAR_MAIL);
                        hostcb->Terminal_State = AWAIT_ESTABLISH;       
                        CCB->Host_Specifier = specifier; 
                    } else {
                        CCB = CCBRI_GetDummyCcb(0 ,hostcb->Ces, SIGNALING_SAPI);
                        CCBRI_RejectHostCall(TEMPORARY_FAILURE);
                    }
                    break; 
                case TERM_DOWN:
                    CCB = CCBRI_GetNewCCB(TERM_INITIALIZE, NO_CHANNEL, SIGNALING_SAPI,
                                          hostcb->Ces, 0, 0);
                    if (CCB != NULL) {
                        LIF_SendPkt(L2P_LAPD, CC_ID, TERM_INITIALIZE,
                                    L3_ID, NL_EST_REQ, dsl, Q931_SAPI,
                                    hostcb->Ces, 0, hostcb->Ces, 0, FALSE,
                                    NULL, NOW, REGULAR_MAIL);
                        hostcb->Terminal_State = AWAIT_ESTABLISH;       
                        CCB->Host_Specifier = specifier; 
                    } else {
                        CCB = CCBRI_GetDummyCcb(0 ,hostcb->Ces, SIGNALING_SAPI);
                        CCBRI_RejectHostCall(TEMPORARY_FAILURE);
                    }
                    break;
                default:
                    break;
            }                               /* End of Case statement */
            break; 
 
        case NL_EST_CONF:           /* L3 tells us of  Link Establishment */
        case MNL_EST_IND:		 /*  was NL_EST_IND */
            CCB = CCB_Head[ dsl ];
            while (CCB &&
                ((CCB->Call_ID != 0) ||
                 (CCB->Sapi != SIGNALING_SAPI) || (CCB->Ces != ces))) {
                CCB = CCB->Next;
            }
            hostcb = &Host_CB [dsl][specifier];
            hostcb->Terminal_State = ESTABLISHED; 

            if (!CCB) {
                CCB = CCBRI_GetNewCCB(TERM_INITIALIZE, NO_CHANNEL,
                            SIGNALING_SAPI, hostcb->Ces, 0, 0);
                if (CCB != NULL)
                    CCB->Host_Specifier = specifier;
            }

            if ((CCB) && (CCB->State == TERM_INITIALIZE)) {
                if (hostcb->Initializing_Terminal) {
                    /*
                     * If the spid is not valid, but it has been sent before, do
                     * not resend the spid every 20 seconds if unanswered.  Only
                     * send the spid once. But if spid has been sent and it failed,
                     * do not send it again, even if Layer 2 restarts.
                     */
                    if (!hostcb->epid.valid) {
                        if ((np_fit[dsl][specifier].spid [0] != '\0') &&
                            (!np_fit[dsl][specifier].spid_sent)) {
                            LIF_UpdateTimer(hostcb->Timer_T415 , CC_ID, CC_ID, 
                                     FIT_TIMEOUT, dsl, CCB->Sapi, hostcb->Ces, 0, 0, 
                                     msecs);
                            LIF_StartTimer(hostcb->Timer_T415);
                            /*
                             * Only send the spid once. US Requirements say so.
                             */
                            hostcb->First_T415 = TRUE;
                            CCBFIT_SendSpid(dsl, specifier);
                        } else {
                            CCBRI_RemoveCCB(CCB);
                        }
                    } else
                        CCBRI_RemoveCCB(CCB);
                } else {
                    if (hostcb->Msglen) {
                        /*
                         * Mail to CC as if its coming from the Host
                         */ 
                        if (LIF_SendBuf(hostcb->Host_ID, CC_ID, hostcb->Msglen,
                                    LRG_INFO_BTYPE, hostcb->HostMsg,
                                    hostcb->HostMsg[0], NOW, REGULAR_MAIL ) == ERR)
                            CCBRI_Error(CCBRI_MAIL_TO_HOST, SEND_BUF_FAILED,
                                    OutPkgInfoLen, 0, TRUE);
                        hostcb->HostMsg = NULL; 
                        hostcb->Msglen = 0;  
                    }
                    CCBRI_RemoveCCB(CCB);
                    hostcb->Terminal_State = INIT; 
                }
            }
            break; 

        /*
         * This INFO should contain the EID, which completes Terminal Initialization 
         * activities. The other possible is that the switch is requesting 
         * terminal initialization due to some error circumstance. 
         */ 
        case CC_INFO_IND:
            if (SwitchType != BRI_5ESS_STYPE) {
                CCB = CCB_Head[dsl];
                while (CCB && ((CCB->Call_ID != 0) ||
                      (CCB->Sapi != SIGNALING_SAPI) || (CCB->Ces != ces)))
                       CCB = CCB->Next;
                hostcb =  &Host_CB [dsl][specifier];

                if (CCB) {
                    if ((hostcb->Terminal_State == AWAIT_INIT) &&
                        (CCB->State == TERM_INITIALIZE)) {

                        LIF_StopTimer(hostcb->Timer_T415);
                        loc_pkt = InPkt;
                        IE = CCIE_Get_IE(loc_pkt->Info, loc_pkt->InfoLen,
                               CAUSE_IE, CODESET_0, 1);
                        /*
                         * The initialization failed if there's a Cause. Extract it and
                         * inform the Host.
                         */ 
                        if (IE) {
                            /*
                             * Inform the user that a spid has failed.
                             */
                            errmsg(&msgsym(INVALID_SPID, ISDN), DSL_TO_NAME(dsl), ces);
                            if(isdn_debug || isdn_debug_ok(q931_debug, dsl))
                                buginf("\nISDN %s: Invalid EID/SPID, Spid%d rejected",
                                       DSL_TO_NAME(dsl), ces);
                           /*
                            * Make sure the epid stuff is cleared out.
                            */
                           hostcb->Terminal_State = NOT_INIT; 
                           hostcb->epid.usid = EID_NOTASSIGNED;
                           hostcb->epid.tid = EID_NOTASSIGNED;
                           hostcb->epid.epsf = FALSE; 
                           hostcb->epid.valid = FALSE;
                        } else {
                           /*
                            * No cause IE, so should be a valid eid for the spid.
                            */
                           IE = CCIE_Get_IE(loc_pkt->Info, loc_pkt->InfoLen,
                                  EID_IE, CODESET_0, 1);
                           if ((IE) && (IE[1] == 2)) {
                               /*
                                * Got a valid EID IE, set the fields, ???
                                */
                               hostcb->Terminal_State = INIT; 
                               hostcb->epid.usid =  IE[2] & 0x7f;
                               hostcb->epid.tid = 0x5f & IE[3];
                               hostcb->epid.epsf = ((IE[3] & 0x40) != 0);
                               hostcb->epid.valid = TRUE;

                               if(isdn_debug || isdn_debug_ok(q931_debug, dsl))
                                   buginf("\nISDN %s: Received EndPoint ID",
                                          DSL_TO_NAME(dsl));
                           }

                           /*
                            * Got one TEI started, now see if have to start the
                            * other TEI as well, only do this after the first TEI
                            * is up, not when the 2nd one comes up.
                            *
                            * But for NI1 and DMS, only if the second spid
                            * is configured, so 1 spid gives 1 TEI, 2 spids gives
                            * 2 TEIs. Have to check the next spid, so specifier+1
                            */
                           if (ces == SIGNALING_CES) {
                               /*
                                * Only try to bring up the second tei if it is not
                                * already up.
                                */
                               if(Host_CB[dsl][specifier+1].Terminal_State == TERM_DOWN) {
                                   switch (SwitchType) {
                                       case BRI_NI1_STYPE:
                                       case BRI_DMS100_STYPE:
                                            if (np_fit[dsl][specifier+1].spid[0] != '\0')
                                                CCBFIT_TerminalInit(HOST_TERM_REGISTER,
                                                    SIGNALING_CES + 1, dsl);
                                            break;
                                       case BRI_5ESS_STYPE:
                                       default:
                                            break;
                                   }
                               }
                           }
                        }
                        if (hostcb->Msglen) {
                            /* Mail to CC as if its coming from the Host */ 
                            if (LIF_SendBuf(hostcb->Host_ID, CC_ID, hostcb->Msglen,
                                   LRG_INFO_BTYPE, hostcb->HostMsg, hostcb->HostMsg[0],
                                   NOW, REGULAR_MAIL) == ERR) {
                                CCBRI_Error(CCBRI_MAIL_TO_HOST, SEND_BUF_FAILED,
                                      OutPkgInfoLen, 0, TRUE);
                            }
                            hostcb->HostMsg = NULL; 
                            hostcb->Msglen = 0;  
                        }  
                        /*
                         * Get rid of the ccb, don't need it anymore.
                         */
                        CCBRI_RemoveCCB(CCB);
                   }
               } else {
                  /*
                   * The switch has requested initialization, have to send the spid.
                   * Or the switch has just sent an unsolicited EID, accept it.
                   */ 
                  loc_pkt = InPkt;
                  IE = CCIE_Get_IE(loc_pkt->Info, loc_pkt->InfoLen,
                                   INFO_REQ_IE, CODESET_0, 1);

                  if ((IE) &&          /* The Info Req IE should be present */
                      (SUB_BYTE( IE[2], 6, 1)) &&           /* Info Req Ind */
                      ((SUB_BYTE(IE[2], 0, 2)) == TERMINAL_ID)) {
                      if ((CCB = CCBRI_GetNewCCB(TERM_INITIALIZE, NO_CHANNEL,
                                                 SIGNALING_SAPI, hostcb->Ces, 
                                                 0, 0)) != NULL) {
                          LIF_UpdateTimer(hostcb->Timer_T415 , CC_ID, CC_ID, 
                                          FIT_TIMEOUT, dsl, CCB->Sapi, hostcb->Ces,
                                          0, 0, msecs);
                          LIF_StartTimer(hostcb->Timer_T415);
                          /*
                           * Only send the spid once, conformance testing 
                           */
                          hostcb->First_T415 = FALSE;
                          CCBFIT_SendSpid(dsl, specifier);
                      }
                  } else {
                      IE = CCIE_Get_IE(loc_pkt->Info, loc_pkt->InfoLen,
                                       EID_IE, CODESET_0, 1);
                      if ((IE) && (IE[1] == 2)) {
                          hostcb->Terminal_State = INIT;
                          hostcb->epid.usid =  IE[2] & 0x7f;
                          hostcb->epid.tid = 0x5f & IE[3];
                          hostcb->epid.epsf = ((IE[3] & 0x40) != 0);
                          hostcb->epid.valid = TRUE;
                      }
                  }
               }
            }                               /* Switch != BRI_5ESS_STYPE */
            break; 

#ifdef BRI_5ESS
        /*
         * NOTE: you need to link in module ccmim.c in order to process
         *        this message !
         */
        case  MNL_MIM_IND:                   /* AT&T TID sent, was NL_MIM_IND */
            CCB = CCB_Head[dsl];
            while (CCB &&
                  ((CCB->Call_ID != 0) ||
                  (CCB->Sapi != SIGNALING_SAPI) || (CCB->Ces != ces))) {
                CCB = CCB->Next;
            }

            if (CCB) {                    
                hostcb = &Host_CB [Dsl_ID][specifier];
                /*
                 * In this case the 5ESS has responded to the SPID 
                 * request with the USID/Epid 
                 */
                LIF_StopTimer(hostcb->Timer_T415);
                hostcb->Terminal_State = INIT; 
                hostcb->spid_valid = TRUE;

                if(isdn_debug || isdn_debug_ok(q931_debug, dsl))
                    buginf("\nISDN %s: Received EndPoint Identifier",
                           DSL_TO_NAME(dsl));
                CCBRI_RemoveCCB( CCB );
            }
           /*
            * ignore an unsolicited MAN request
            */
           break; 
#endif
        case  FIT_TIMEOUT:
            /*
             * Timed-out waiting for Terminal initialization activities
             * to complete. Re-transmit the INFO w/SPID until we get an answer,
             * this is for National ISDN.
             */ 
            CCB = CCB_Head[dsl];
            while (CCB &&
                  ((CCB->Call_ID != 0) ||
                   (CCB->Sapi != SIGNALING_SAPI) || (CCB->Ces != ces))) {
                CCB = CCB->Next;
            }

            hostcb = &Host_CB [dsl][specifier];
            if (hostcb->First_T415 == TRUE) {
                /*
                 * NI1 and custom 5ESS will continuously send the SPID until
                 * acknowledged, but the custom DMS will only send it out twice.
                 */
                if (SwitchType == BRI_DMS100_STYPE)
                    hostcb->First_T415 = FALSE;
                LIF_UpdateTimer(hostcb->Timer_T415 , CC_ID, CC_ID, 
                                FIT_TIMEOUT, dsl, CCB->Sapi, hostcb->Ces, 0, 0, 
                                msecs);
                LIF_StartTimer(hostcb->Timer_T415);
                CCBFIT_SendSpid(dsl, specifier);   
            } else {
                /*
                 * This path will only occur for custom dms100.
                 * We've retransmitted our SPID, yet timed-out again
                 * Cancel all initiation activities.
                 */
                if ((hostcb->Terminal_State != INIT) &&
                    (hostcb->Terminal_State != TERM_DOWN)) {
                    if(isdn_debug || isdn_debug_ok(q931_debug, dsl))
                        buginf("\nISDN %s: Invalid EID/SPID, Spid failure",
                               DSL_TO_NAME(dsl));
                }
                if (hostcb->HostMsg) {  
                    event_b = HOST_DISCONNECT_ACK;  
                    CCBRI_DisconnectHostCall(&event_b, TIMER_EXPIRY);
                    LIF_FreeBuffer(LRG_INFO_BTYPE, hostcb->HostMsg);
                    hostcb->HostMsg = NULL; 
                    hostcb->Msglen = 0;  
                }
                hostcb->spid_valid = FALSE;
                CCBRI_RemoveCCB(CCB);

                /*
                 * One TEI started, now see if have to start a second TEI.
                 * Only do this after the first TEI is up, not when the second
                 * one comes up. But only do it if the second spid is configured,
                 * so 1 spid gives 1 TEI, 2 spids gives 2 TEIs.  Have to check the
                 * next spid, so specifier+1
                 */
                if (ces == SIGNALING_CES) {
                    /*
                     * Only try to bring up the second tei if it is not already up.
                     */
                    if (Host_CB[dsl][specifier+1].Terminal_State == TERM_DOWN) {
                        if (np_fit[dsl][specifier+1].spid[0] != '\0') {
                            CCBFIT_TerminalInit(HOST_TERM_REGISTER,
                                                SIGNALING_CES + 1, dsl);
                        }
                    }
                }
            }
            break;

         default:
            break;
      }
  return(TRUE);    
}

/*************************
* CCBFIT_TerminalParms    *
**************************
*
*  Host may update terminal paramaters via a Host Registration message
*
*/
PUBLIC void  CCBFIT_TerminalParms( dsl, host_specifier, msg )

int dsl;                                    /* digital subscriber loop */
int host_specifier;                     /* index to host control block */
BYTE * msg;                               /* host registration message */

{
  int IE_index; 
  int j; 
  H_ID  hostcb;

  hostcb =  &Host_CB [dsl] [ host_specifier  ];

  /*  First we determine if its a Fit type terminal
  */ 
  if ( CCBRI_GetHost_IE ( msg , INITIALIZING_TERM_IE ) )
     {
     hostcb->Initializing_Terminal = TRUE; 
     }
  else
     {
     hostcb->Initializing_Terminal = TRUE; 
     }
  /*  Are we doing DN routing? 
  */ 

  if ( CCBRI_GetHost_IE ( msg , DN_ROUTING_IE ) )
     {
     hostcb->DN_Routing = TRUE; 
     }
  else
     {
     hostcb->DN_Routing = FALSE; 
     }

  /*  DN available 
  */ 
  if ( ( IE =  CCBRI_GetHost_IE ( msg , DN_IE )) != NULL )
     {
     IE_index = 2;
     j = 0;
     while (IE_index < ( IE[1] + 2 ) )
        hostcb->DN [j++]  = IE[IE_index++ ];
     hostcb->DN [j++]     = 0x00;
     }
  else
     {
     hostcb->DN[ 0 ] = 0x00; 
     }

  /*  Are we doing Bearer Routing?
  */ 
  if ( CCBRI_GetHost_IE ( msg , BEARER_ROUTING_IE ) )
     {
     hostcb->Bearer_Routing = TRUE; 
     }
  else
     {
     hostcb->Bearer_Routing = FALSE; 
     }

  /*  Pick up Bearer Type - which may be Voice 0x01, Circuit 0x02, or
  *   Packet 0x04
  */ 
  if ( ( IE =  CCBRI_GetHost_IE ( msg , BEARER_TYPE_IE )) != NULL )
     {
     hostcb->Bearer_Type = IE[2]; 
     }
  else
     {
     hostcb->Bearer_Type = 0x00; 
     }
     
  /*  Then the SPID
  */ 
  if ( ( IE =  CCBRI_GetHost_IE ( msg , SPID_IE )) != NULL )
     {
     IE_index = 2;
     j = 0;
     while (IE_index < ( IE[1] + 2 ) )
        hostcb->SPID [j++]  = IE[IE_index++ ];
     hostcb->SPID [j++]  = 0x00;
     hostcb->Initializing_Terminal = TRUE; 
     hostcb->Terminal_State = TERM_DOWN; 
     }
  else
     {
     hostcb->SPID[ 0 ] = 0x00; 
     }

  /*  The CES may be passed
  */ 
  if ( ( IE =  CCBRI_GetHost_IE ( msg , CES_IE )) != NULL )
     {
     hostcb->Ces = IE[2]; 
     }
  else
     {
     hostcb->Ces  = ( InPkgInfo[ 2 ] ) ? InPkgInfo[ 2 ] : SIGNALING_CES;
     }
}

/******************
* CCBFIT_SendSpid  *
*******************
*
* Send the SPID to register a Fully Initializing Terminal
*
*/
PRIVATE void CCBFIT_SendSpid (dsl, specifier)
int dsl;
int specifier;  
{
  H_ID hostcb;
  int  k =0;
  int  crlen;
  int  spidLen;

  crlen = 0;                           /* SPID sent with null call ref */
  hostcb = &Host_CB [dsl] [ specifier ];

  switch ( SwitchType ) 
     { 
#ifdef BRI_5ESS  
     case BRI_5ESS_STYPE:
        ME_got_mnl_est_conf (dsl, hostcb->Ces, CCB->Call_ID);
        hostcb->Terminal_State = AWAIT_INIT;
        np_fit[dsl][specifier].spid_sent = TRUE;
        break; 
#endif 
     /* Assume here that DMS100 and NI1 have the same SPID format
     */
     default:  
        /* Assume here that DMS100 and NI1 have the same SPID format
        */
        if ( np_fit[dsl][specifier].spid[0] != '\0')
           {   
           OutPkt = LIF_GetPkt( LRG_INFO_BTYPE, DATA_OFFSET,  FALSE );
           if  ( OutPkt )
              {
              OutPkt->HeadLen  = DATA_OFFSET;                     /* 4 */
              OutPkt->InfoType = LRG_INFO_BTYPE;            /* default */
              OutPkt->Info[ 0 ] = Qdot931;                 /* protocol */
              OutPkt->Info[ 1 ] = crlen;              /* null call ref */
              OutPkt->InfoLen = crlen +  3;   /* leave space for l3hdr */
                                       
              OutPkt->Info[ crlen + 2 ] = CC_INFO_REQ;   /* message ID */
              OutPkt->Info[ OutPkt->InfoLen++ ]  = SPID_IE;
              for (spidLen = 0; np_fit[dsl][specifier].spid[spidLen]; ++spidLen)
			;

              OutPkt->Info[ OutPkt->InfoLen++ ]  = spidLen;
              while (k < spidLen)
                 OutPkt->Info[OutPkt->InfoLen++] = np_fit[dsl][specifier].spid[k++];

              /*
               * Bump up the priority of the mail for spids.
               * Don't really want to add a parameter to ccbri_mailtol3()
               */
              Mail_priority = EXPRESS_MAIL;

              CCBRI_MailtoL3();
              hostcb->Terminal_State = AWAIT_INIT; 
              np_fit[dsl][specifier].spid_sent = TRUE;
              }
           else
              {
              CCBRI_Error( CCBRI_GET_OUT_PKT, CC_INFO_REQ, CCB->State, OUT_OF_PKTS, TRUE );
              CCBRI_RemoveCCB(CCB);
              }
           } 
        else
           {
           /*
           *  May not be an error - the Host hasn't sent a Registration Message
           *  for instance.
           */ 
           ;
           } 
        break; 
     }                                        /* End of Case statement */
}

/***********************
* CCBFIT_TerminalDown  *
************************
*
* Send the SPID to register a Fully Initializing Terminal,
* this is only used for 5ess, dms-100 and ni1 switchtypes.
*
*/
PUBLIC void CCBFIT_TerminalDown (dsl, ces)
BYTE dsl;
int  ces;  
{
    H_ID hostcb;
    int  k;
    int  max_hosts;

    k = 0;
    max_hosts = MAX_HOSTS;
    hostcb = &Host_CB [dsl] [k];
    while ((hostcb->Ces != ces) && (k < max_hosts)) {
       hostcb = &Host_CB [dsl] [k++];
    } 
    hostcb->Terminal_State = TERM_DOWN;
    hostcb->epid.valid = FALSE;
    hostcb->epid.usid = EID_NOTASSIGNED;
    hostcb->epid.tid = EID_NOTASSIGNED;
    np_fit[dsl][ces-1].spid_sent = FALSE;
    /*
     * Make sure that the CCB gets removed. Can cause CCBs to
     * get lost if the link doesn't come up correctly.
     */
    CCB = CCB_Head[dsl];
    while (CCB &&
          ((CCB->Call_ID != 0) ||
           (CCB->Sapi    != SIGNALING_SAPI) ||
           (CCB->Ces     != ces)))
              CCB = CCB->Next;
    if (CCB)
        CCBRI_RemoveCCB(CCB);
}
