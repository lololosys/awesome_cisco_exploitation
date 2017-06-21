/* $Id: sdlc_event.c,v 3.3 1995/11/17 09:23:55 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ibm/sdlc_event.c,v $
 *------------------------------------------------------------------
 * SNA Network Management SDLC Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sdlc_event.c,v $
 * Revision 3.3  1995/11/17  09:23:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:48:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "interface_private.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "sdlc.h"
#include "sdlc_event.h"
#include "sdlc_registry.h"
#include "../ibm/sdlc_registry.regc"


static void SetLastFailObjects(sdlc_data_block *sdb, sdlc_nm_event_hook_t eventType)
{
    /* last fail control in + out and last fail reply time out
       used by both sna netmgt and snmp trap */
    if ((eventType != SDLC_CarrierDown) &&
        (eventType != SDLC_RxDm) &&
        (eventType != SDLC_RxDmInErrorState) &&
        (eventType != SDLC_XidRetriesExhausted)) {

        GET_TIMESTAMP(sdb->sdlc_lastFailTime);
        sdb->sdlc_lastFailCtrlIn  = sdb->sdlc_lastCtrlIn;  /*SZ*/
        sdb->sdlc_lastFailCtrlOut = sdb->sdlc_lastCtrlOut;
        sdb->sdlc_lastFailReplyTO = sdb->sdlc_lastReplyTO;
    }
}

         

void SdlcNmEvent(sdlc_nm_event_hook_t eventType,
                 hwidbtype *idb, 
                 sdlc_data_block *sdb)
{

    SetLastFailObjects(sdb, eventType);
    
  /*
   * Decide which SNA Alerts to create for the Events
   */

   switch (eventType) {
       case SDLC_MaxIFieldExceeded:
         reg_invoke_Sdlc_Alert(SdlcAlert16, sdb);
         break;

       case SDLC_RxFrmrNoReason:
         reg_invoke_Sdlc_Alert(SdlcAlert14, sdb);
         break;

       case SDLC_RxUFrameWithIField:
         reg_invoke_Sdlc_Alert(SdlcAlert9, sdb);
         break;

       case SDLC_RxSnrmInNrm:
         reg_invoke_Sdlc_Alert(SdlcAlert3, sdb);
         break;

       case SDLC_RxFrmrWandX:
         reg_invoke_Sdlc_Alert(SdlcAlert5, sdb);
         break;

       case SDLC_RxFrmrW:
         reg_invoke_Sdlc_Alert(SdlcAlert4, sdb);
         break;

       case SDLC_RxFrmrY:
         reg_invoke_Sdlc_Alert(SdlcAlert7, sdb);
         break;

       case SDLC_RxFrmrZ:
         reg_invoke_Sdlc_Alert(SdlcAlert6, sdb);
         break;

       case SDLC_RxDm:
         reg_invoke_Sdlc_Alert(SdlcAlert2, sdb);
         break;

       case SDLC_RxInvalidUFrame:
         reg_invoke_Sdlc_Alert(SdlcAlert8, sdb);
         break;

       case SDLC_RxSFrameWithIField:
         reg_invoke_Sdlc_Alert(SdlcAlert9, sdb);
         break;

       case SDLC_RxInvalidSFrame:
         reg_invoke_Sdlc_Alert(SdlcAlert8, sdb);
         break;

       case SDLC_RxFrmrNoReasonInErrorState:
       case SDLC_RxUFrameWithIFieldInErrorState:
       case SDLC_RxFrmrInErrorState:
       case SDLC_RxDmInErrorState:
       case SDLC_RNRLimit:
       case SDLC_PollRetriesExhausted_IFrameReSent:
         break;

       case SDLC_RxInvalidNr:
         reg_invoke_Sdlc_Alert(SdlcAlert10, sdb);
         break;

       case SDLC_PollRetriesExhausted_NoResponse:
         reg_invoke_Sdlc_Alert(SdlcAlert1, sdb);
         break;

       case SDLC_InactivityTimerExpired:
         reg_invoke_Sdlc_Alert(SdlcAlert12, sdb);
         break;

       case SDLC_XidRetriesExhausted:
         reg_invoke_Sdlc_Alert(SdlcAlert13, sdb);
         break;
 
       case SDLC_CarrierDown:
         reg_invoke_Sdlc_Alert(SdlcAlert15, sdb);
         break;

   }

  /*
   * Decide which SNMP Traps to create for the Events
   */

   switch (eventType) {

       /* FRMR Transmitted */
       case SDLC_MaxIFieldExceeded:
       case SDLC_RxUFrameWithIField:
       case SDLC_RxInvalidUFrame:
       case SDLC_RxUFrameWithIFieldInErrorState:
       case SDLC_RxInvalidNr:
       case SDLC_RxSFrameWithIField:
         sdb->lastFailCause = TX_FRMR;
         reg_invoke_sdlc_ls_status_change(sdb);
         break;

       /* FRMR Received */
       case SDLC_RxFrmrNoReason:
       case SDLC_RxFrmrWandX:
       case SDLC_RxFrmrW:
       case SDLC_RxFrmrY:
       case SDLC_RxFrmrZ:
       case SDLC_RxFrmrNoReasonInErrorState:
       case SDLC_RxFrmrInErrorState:
         sdb->lastFailCause = RX_FRMR;
         reg_invoke_sdlc_ls_status_change(sdb);
         break;

       /* Protocol Error */
       case SDLC_RxSnrmInNrm:
       case SDLC_RxInvalidSFrame:
         sdb->lastFailCause = PROTOCOL_ERR;
         reg_invoke_sdlc_ls_status_change(sdb);
         break;

       /* No Response */
       case SDLC_PollRetriesExhausted_NoResponse:
         sdb->lastFailCause = NO_RESPONSE;
         reg_invoke_sdlc_ls_status_change(sdb);
         break;

       /* No Activity */
       case SDLC_InactivityTimerExpired:
         sdb->lastFailCause = NO_ACTIVITY;
         reg_invoke_sdlc_ls_status_change(sdb);
         break;

       /* RNR Limit */
       case SDLC_RNRLimit:
         sdb->lastFailCause = RNR_LIMIT;
         reg_invoke_sdlc_ls_status_change(sdb);
         break;

       /* Retries Expired */
       case SDLC_PollRetriesExhausted_IFrameReSent:
         sdb->lastFailCause = RETRIES_EXPIRED;
         reg_invoke_sdlc_ls_status_change(sdb);
         break;

       case SDLC_CarrierDown:
/*       if (sdb->sdlc_state != SDLC_DISCONNECT)
             reg_invoke_snmp_link_trap(LINK_DOWN_TRAP, idb); */

       /* Do not create Trap */
       case SDLC_RxDm:
       case SDLC_RxDmInErrorState:
       case SDLC_XidRetriesExhausted:
         break;
   }

}




      
