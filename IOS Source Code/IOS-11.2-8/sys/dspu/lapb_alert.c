/* $Id: lapb_alert.c,v 3.2.60.2 1996/05/17 10:48:36 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/lapb_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management LAPB alert handler
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lapb_alert.c,v $
 * Revision 3.2.60.2  1996/05/17  10:48:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:35:21  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:03:22  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:33:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:45:54  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:20  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:05:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_generic.h"

#define MaxAlertSize 200
#include "../snanm/lapb_alert1.alth"
#include "../snanm/lapb_alert2.alth"
#include "../snanm/lapb_alert3.alth"
#include "../snanm/lapb_alert4.alth"
#include "../snanm/lapb_alert5.alth"
#include "../snanm/lapb_alert6.alth"
#include "../snanm/lapb_alert7.alth"
#include "../snanm/lapb_alert8.alth"
#include "../snanm/lapb_alert9.alth"
#include "../snanm/lapb_alert10.alth"
#include "../snanm/lapb_alert11.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "../x25/x25.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"


static void SetLapbParam(byte* buf, hwidbtype *idb, const char* configParam)
{ 
   byte* sv;
   byte* sf;
   short svLen;
   short sfLen;
 
   ForAllSvInMV(((MV*)buf), sv, svLen){
        switch (sv[1]){

        case 0x95: /* Install Causes */
            ForAllSfInSV(sv,sf,sfLen) {
               if (sf[1] == 0x82 && sf[3] == 0x73)
                   ConvertAsciiToEbcdic(configParam, &sf[5], 2);
            }
            break; 

        case 0x96: /* Failure Causes */
            SetFailCause(idb, sv, "LAPB");
             break;

        case 0x52: /* LCS Configuration Data */
            ForAllSfInSV(sv, sf, sfLen){ 
                if (sf[1] == 0x07) { /* LCS Link Attribute */
                    sf[2] = 0x03;    /* packet switched */
                    sf[3] = 0x02;    /* full duplex */
                    sf[4] = 0x04;    /* lapb dlc protocol type */                 
                    sf[5] = 0x01;    /* point to point */          
		}
	    }
            break;

        case 0x05: /* Hierarchy Name List */
            SetHRname(idb, sv, "LAPB");
            break;              
        }  /* switch */                      
    }  /* ForAllSvInMV */
}


void Lapb_Alert(Lapb_AlertType alertType, hwidbtype *idb)
{
   byte buf[MaxAlertSize];   
   boolean createAlert = FALSE;
   const char* configParam = "";


   switch(alertType){

      case LapbAlert1: /* Local station sent FRMR Y */
#ifdef LAPBALERT1_H
         memcpy(buf, (char*)&LAPBALERT1, sizeof(struct LapbAlert1));
         createAlert = TRUE;
         configParam = "N1";
#endif
         break;

      case LapbAlert2: /* Local station sent FRMR Z */
#ifdef LAPBALERT2_H
         memcpy(buf, (char*)&LAPBALERT2, sizeof(struct LapbAlert2));
         createAlert = TRUE;
#endif
         break;

      case LapbAlert3: /* Retry count expired */
#ifdef LAPBALERT3_H
         memcpy(buf, (char*)&LAPBALERT3, sizeof(struct LapbAlert3));
         createAlert = TRUE;
         configParam = "N2";
#endif
         break;

      case LapbAlert4: /* DISC command received during connect */
#ifdef LAPBALERT4_H
         memcpy(buf, (char*)&LAPBALERT4, sizeof(struct LapbAlert4));
         createAlert = TRUE;
#endif
         break;

      case LapbAlert5: /* Local station sent SABME, received DM */
#ifdef LAPBALERT5_H
         memcpy(buf, (char*)&LAPBALERT5, sizeof(struct LapbAlert5));
         createAlert = TRUE;
#endif
         break;

      case LapbAlert6: /* FRMR W received */
#ifdef LAPBALERT6_H
         memcpy(buf, (char*)&LAPBALERT6, sizeof(struct LapbAlert6));
         createAlert = TRUE;
#endif
         break;

     case LapbAlert7: /* FRMR X received */
#ifdef LAPBALERT7_H
         memcpy(buf, (char*)&LAPBALERT7, sizeof(struct LapbAlert7));
         createAlert = TRUE;
#endif
         break;

      case LapbAlert8: /* FRMR Y received */
#ifdef LAPBALERT8_H
         memcpy(buf, (char*)&LAPBALERT8, sizeof(struct LapbAlert8));
         createAlert = TRUE;
         configParam = "N1";
#endif
         break;

      case LapbAlert9: /* FRMR Z received */
#ifdef LAPBALERT9_H
         memcpy(buf, (char*)&LAPBALERT9, sizeof(struct LapbAlert9));
         createAlert = TRUE;
#endif
         break;

      case LapbAlert10: /* Local station sent FRMR W */
#ifdef LAPBALERT10_H
         memcpy(buf, (char*)&LAPBALERT10, sizeof(struct LapbAlert10));
         createAlert = TRUE;
#endif
         break;

      case LapbAlert11: /* Local station sent FRMR X */
#ifdef LAPBALERT11_H
         memcpy(buf, (char*)&LAPBALERT11, sizeof(struct LapbAlert11));
         createAlert = TRUE;
#endif
         break;

     }

     if (createAlert){
         SetLapbParam(buf, idb, configParam);
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nLapb  ALERT Alert%d not supported",alertType + 1);
}
