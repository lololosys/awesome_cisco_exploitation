/* $Id: x25_event.c,v 3.2 1995/11/17 18:07:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/x25/x25_event.c,v $
 *------------------------------------------------------------------
 * SNA Network Management X25 Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_event.c,v $
 * Revision 3.2  1995/11/17  18:07:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/05  08:41:58  bchan
 * CSCdi38837:  Spurious memory access in x25Event
 *
 * Revision 2.2  1995/08/17  03:56:52  bchan
 * CSCdi38837:  Spurious memory access in x25Event
 *
 * Revision 2.1  1995/06/07  23:22:19  hampton
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
#include "x25_event.h"
#include "../dspu/snanm_registry.regh"
#include "x25_registry.h"
#include "packet.h"


void X25EventRx(hwidbtype* idb, lcitype* lci, packettype* p)
{
   X25_EventData eventData;
   eventData.diag = *((uchar *)p + X25HEADERBYTES);
   eventData.cause = *((uchar *)p + X25HEADERBYTES + 1);

   switch (p->x25_pti){
      case X25_RESTART:
         X25Event(lci, idb, &eventData, X25Restart_Rcvd);
         break;
      case X25_CLEAR:
         X25Event(lci, idb, &eventData, X25Clear_Rcvd);
         break;
      case X25_RESET:
         X25Event(lci, idb, &eventData, X25Reset_Rcvd);
         break;
       case X25_DIAGNOSTIC:
         eventData.gfiLcg = ((x25paktype*)p)->x25GfiLcg;
         eventData.lcn = ((x25paktype*)p)->x25Lcn;
         eventData.pti = ((x25paktype*)p)->x25Pti;
         X25Event(lci, idb, &eventData, DiagnosticPakSentOrRcvd);
         break;

   }
}

void X25Event(lcitype   *lci, 
              hwidbtype *idb,
              X25_EventData* eventData,
              X25_EventType event) {

    boolean isDte = ISDTE(idb, get_lci_considb(lci));
    X25_AlertType alert = -1;

    eventData->alertType = AlertClear;   /* default is clear  */
    switch(event) {

    case X25Clear_Sent:
        /* A clearRequest or clear inidcation is sent */

       if (lci->lci_state == X25_P6 && eventData->diag == X25_DIAG_TE_CLEAR_INDICATION){
             /* T23 expired */
             alert = X25Alert9;
       }
       else if (lci->lci_state == X25_P2 &&eventData->diag ==  X25_DIAG_TE_INCOMING_CALL){
             /* T21 expired */
             alert = X25Alert8;
       }
       else if ((eventData->cause & 0x7f) != 0){  /* cause set */
           if (isDte)
                alert = X25Alert4;
           else {
                eventData->alertType = AlertClear;
                alert = X25Alert20;
           }
       }
       break;

    case X25Reset_Sent:
        /* A reset request or indication is sent */

       if ( !ISIEEEMSK(idb)) {
           reg_invoke_x25Reset_event(lci);
       }

       if (lci->lci_state == X25_D2 && 
                            eventData->diag == X25_DIAG_TE_RESET_INDICATION){
             /* T22 expired */
             alert = X25Alert7;
       }
       else if (isDte)

           alert =  X25Alert3;
       else {
           eventData->alertType = AlertReset;
          alert = X25Alert20;
       }
       break;

    case X25Restart_Sent:

       /* first to send a trap */
       if (idb->x25_upstate && idb->x25_state == X25_R1) {
            reg_invoke_x25Restart_event(idb);
	    idb->x25_snmp_restart_tx = TRUE;
       }
       /* fall down to send the alert */

    case CMNS_Restart_Sent:

       if (eventData->diag == X25_DIAG_TE_RESTART_INDICATION){
             /* T20 expired */
             alert = X25Alert6;
       }
       else if (isDte)
           alert =  X25Alert5;
       else {
           eventData->alertType = AlertRestart;
           alert = X25Alert20;
	}
        break;


    case X25Clear_Rcvd:
        if (isDte){
            /* All CLEAR received by DTE is ERROR
             */
            eventData->alertType = AlertClear;
            alert =  X25Alert1;
        }
        else 
            /* Only send alert for CLEAR (received)
             * which is in ERROR, filter out NORMAL CLEAR
             */
            if ((eventData->cause & 0x7f) != 0){
                eventData->alertType = AlertClear;
                alert =  X25Alert21;
	    }
        break;   

    case X25Reset_Rcvd:
        eventData->alertType = AlertReset;
        alert = isDte? X25Alert1 : X25Alert21;
        break; 

    case X25Restart_Rcvd:
        eventData->alertType = AlertRestart;
        alert =  isDte? X25Alert2: X25Alert21;
        break; 

    case ProtocolViolation:
        alert = X25Alert10;
        break;

    case DiagnosticPakSentOrRcvd:
        alert = X25Alert11;
        break;

    }
    if (alert != -1)
         reg_invoke_X25_Alert(alert, lci, idb, (uchar*)eventData);
}








      
