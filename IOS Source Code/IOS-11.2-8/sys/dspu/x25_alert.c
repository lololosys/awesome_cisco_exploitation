/* $Id: x25_alert.c,v 3.2.60.1 1996/03/18 19:34:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dspu/x25_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management X25 Alert handler
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_alert.c,v $
 * Revision 3.2.60.1  1996/03/18  19:34:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:23  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:06:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:31:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "interface_private.h"

#include "../snanm/x25_alert1.alth"
#include "../snanm/x25_alert2.alth"
#include "../snanm/x25_alert3.alth"
#include "../snanm/x25_alert4.alth"
#include "../snanm/x25_alert5.alth"
#include "../snanm/x25_alert6.alth"
#include "../snanm/x25_alert7.alth"
#include "../snanm/x25_alert8.alth"
#include "../snanm/x25_alert9.alth"
#include "../snanm/x25_alert10.alth"
#include "../snanm/x25_alert11.alth"
#include "../snanm/x25_alert17.alth"
#include "../snanm/x25_alert20.alth"
#include "../snanm/x25_alert21.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "../x25/x25.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"
#include "../x25/x25_event.h"

typedef enum {X25_T0, X25_T1, X25_T2, X25_T3} X25TimerType;


static void SetX25Param(byte* buf, 
                 lcitype* lci, 
                 hwidbtype *idb, 
                 X25TimerType timer,
                 X25_EventData *eventData)
{ 
   byte* sv;
   byte* sf;
   short svLen;
   short sfLen;
   char  timerName[4];
 
   ForAllSvInMV(((MV*)buf), sv, svLen){
        switch (sv[1]){

        case 0x96: /* Failure Cause */
            ForAllSfInSV(sv,sf,sfLen) {
                if (sf[1] == 0x82) { 
                    switch (sf[3]){
                    case 0x1A: /* CLEAR request/indication    */
                         sf[3] += eventData->alertType;   /* -> /clear/restart/reset */
                    case 0x1B: /* RESET request/indication sent */
                    case 0x1C: /* RESTART request/indication sent   */
                        
                        sf[5] = eventData->cause;  /* cause code */
                        break;
		    case 0x1D:  /* diagnostic code */
                        sf[5] = eventData->diag;
                        break;
         	    case 0x1E: /* diagnostic explanation */
                        sf[5] = eventData->gfiLcg;
                        sf[6] = eventData->lcn;
                        sf[7] = eventData->pti;
                        break;
	            case 0xD8:   /* (timer) expired */
                        timerName[0] = 'T';
                        timerName[1] = '2';
                        timerName[2] = timer + '0';
                        ConvertAsciiToEbcdic(timerName, &sf[5], 3);
                        break;
	            case 0xB6:  /* (retry count) */
                        switch (timer){
			 case X25_T0:
                            sf[5] = idb->x25_t0_count;
                            break;
			 case X25_T1:
                            sf[5] = idb->x25_t1_count;
                            break;
			 case X25_T2:
                            sf[5] = idb->x25_t2_count;
                            break;
		      	 case X25_T3:
                            sf[5] = idb->x25_t3_count;
                            break;
			 }
                        break;
		    case 0xB7:  /* (timer setting) */
                        switch (timer){
			 case X25_T0:
                            sf[5] = idb->x25_t0;
                            break;
			 case X25_T1:
                            sf[5] = idb->x25_t1;
                            break;
			 case X25_T2:
                            sf[5] = idb->x25_t2;
                            break;
		      	 case X25_T3:
                            sf[5] = idb->x25_t3;
                            break;
			 }
                        break;
	            case 0x56:  /* DTE address called */
                        if (lci)
                           SetX25Address(&sf[5], &lci->lci_dst_addr);
                         break;
	            case 0x57:  /* DTE address calling */
                        if (lci)
                           SetX25Address(&sf[5], &lci->lci_src_addr);
                        break;
		    case 0x58:  /* local DTE address */
                        SetX25Address(&sf[5], idb->x25_address);
                        break;
		    case 0x60:  /* port Number */
                        sf[5] = idb->unit;
                        break;
		    case 0x61:  /* adapter Number */
                        sf[5] = idb->hw_if_index;
                        break;      
	            case 0x67:  /* default = locally initiated logical channel */
                        if (lci) {
                           sf[5] = lci->lci_lci >> 8;
                           sf[6] = lci->lci_lci;
                           if (!lci->lci_outgoing)
                               sf[3] = 0x68;   /* change subfield to remotely initiated */
			}
                        break;
		    }
		}
	    }
            break;

        case 0x52: /* LCS Configuration Data */
            ForAllSfInSV(sv, sf, sfLen){ 
                if (sf[1] == 0x07) { /* LCS Link Attribute */
                    sf[2] = 0x03;    /* packet switched */
                    sf[3] = 0x02;    /* full duplex */
                    sf[4] = 0x04;    /* lapb dlc protocol type */                 
                    sf[5] = 0x01;    /* point to point  */          
		}
	    }
            break;
        case 0x05 : /* Hierarchy Name List */
            SetHRname(idb, sv, "X25");
            break;              
        }                        
    }
}


void X25_Alert(X25_AlertType alertType, lcitype* lci, hwidbtype *idb, uchar* eventData)
{
   byte buf[MaxAlertSize];
   boolean createAlert = FALSE;
   X25TimerType timer = X25_T0;

   /* Dspu almost guarantee a buf available,
    * this should not happen
    */

   switch(alertType){

      case X25Alert1: /* CLEAR/RESET IND received by DTE */
#ifdef X25ALERT1_H
         memcpy(buf, (char*)&X25ALERT1, sizeof(struct X25Alert1));
         createAlert = TRUE;
#endif
         break;

      case X25Alert2: /* RESTART IND received by DTE */
#ifdef X25ALERT2_H
         memcpy(buf, (char*)&X25ALERT2, sizeof(struct X25Alert2));
         createAlert = TRUE;
#endif
         break;

      case X25Alert3: /* RESET REQ sent by DTE */
#ifdef X25ALERT3_H
         memcpy(buf, (char*)&X25ALERT3, sizeof(struct X25Alert3));
         createAlert = TRUE;
#endif
         break;

      case X25Alert4: /* CLEAR sent by DTE */
#ifdef X25ALERT4_H
         memcpy(buf, (char*)&X25ALERT4, sizeof(struct X25Alert4));
         createAlert = TRUE;
#endif
         break;

      case X25Alert5: /* RESTART sent by DTE */
#ifdef X25ALERT5_H
         memcpy(buf, (char*)&X25ALERT5, sizeof(struct X25Alert5));
         createAlert = TRUE;
#endif
         break;

      case X25Alert6: /* T20 (Restart) expired */
#ifdef X25ALERT6_H
         memcpy(buf, (char*)&X25ALERT6, sizeof(struct X25Alert6));
         createAlert = TRUE;
         timer = X25_T0;
#endif
         break;

     case X25Alert7: /* T22 (Reset) expired */
#ifdef X25ALERT7_H
         memcpy(buf, (char*)&X25ALERT7, sizeof(struct X25Alert7));
         createAlert = TRUE;
         timer = X25_T2;
#endif
         break;

      case X25Alert8: /* T21 (Call) expired */
#ifdef X25ALERT8_H
         memcpy(buf, (char*)&X25ALERT8, sizeof(struct X25Alert8));
         createAlert = TRUE;
         timer = X25_T1;
#endif
         break;

      case X25Alert9: /* T23 (Clear) expired */ 
#ifdef X25ALERT9_H
         memcpy(buf, (char*)&X25ALERT9, sizeof(struct X25Alert9));
         createAlert = TRUE;
         timer = X25_T3;
#endif
         break;

      case X25Alert10: /* (DTE) Potocol violation by remote */
#ifdef X25ALERT10_H
         memcpy(buf, (char*)&X25ALERT10, sizeof(struct X25Alert10));
         createAlert = TRUE;
#endif
         break;

      case X25Alert11: /* Diagnostic packet received by DTE */
#ifdef X25ALERT11_H
         memcpy(buf, (char*)&X25ALERT11, sizeof(struct X25Alert11));
         createAlert = TRUE;
#endif
         break;

      case X25Alert17: /* EIA signal lost */
#ifdef X25ALERT17_H
         if (idb->failCause != idb->lastFailCause)
             return;                             /* same error, don't log it */
         memcpy(buf, (char*)&X25ALERT17, sizeof(struct X25Alert17));
         createAlert = TRUE;
#endif
         break;

      case X25Alert20: /* (DCE)Cleanup Indication Sent {Restart/Clear/Reset} */
#ifdef X25ALERT20_H
         memcpy(buf, (char*)&X25ALERT20, sizeof(struct X25Alert20));
         createAlert = TRUE;
#endif
         break;

      case X25Alert21: /* (DCE)Cleanup Request Received {Restart/Clear/Reset} */
#ifdef X25ALERT21_H
         memcpy(buf, (char*)&X25ALERT21, sizeof(struct X25Alert21));
         createAlert = TRUE;
#endif
         break;
     }

     if (createAlert){
         SetX25Param(buf, lci, idb, timer, (X25_EventData*)eventData);
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nX25  ALERT Alert%d not supported",alertType + 1);
}


