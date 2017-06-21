/* $Id: qllc_alert.c,v 3.2.60.2 1996/05/17 10:49:06 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/qllc_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management QLLC Alert Handler 
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc_alert.c,v $
 * Revision 3.2.60.2  1996/05/17  10:49:06  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:35:24  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:03:48  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:34:07  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:04  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:44  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:06:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/20  17:03:33  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.2  1995/07/02 05:15:51  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.1  1995/06/07  20:30:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include "interface_generic.h"
#include "packet.h"
#include "registry.h"

#include "../snanm/qllc_alert1.alth"

/* Q_Z invalid NR not supported 
#include "../snanm/qllc_alert2.alth"
*/

#include "../snanm/qllc_alert3.alth"
#include "../snanm/qllc_alert4.alth"
#include "../snanm/qllc_alert5.alth"
#include "../snanm/qllc_alert6.alth"
/* Q_Z invalid NR not supported 
#include "../snanm/qllc_alert7.alth"
*/
#include "../snanm/qllc_alert8.alth"
#include "../snanm/qllc_alert9.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "../x25/x25.h"
#include "../util/avl.h"
#include "../cls/dlc_public.h"
#include "../cls/dlc.h"
#include "../cls/cls_assert.h"
#include "../cls/dlc_registry.regh"
#include "../srt/qllc.h"
#include "../srt/qllc_private.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "dspu_fp.h"
#include "lan_alert.h"


static void SetQllcParam(byte* buf, lcitype* lci, const char* configParam) 
{
    byte* sv;
    byte* sf;
    short svLen;
    short sfLen;

    hwidbtype* idb = lci->lci_idb->hwptr;

    ForAllSvInMV(((MV*)buf), sv, svLen){
        switch (sv[1]){
 
        case 0x95: /* Install Cause */
            ForAllSfInSV(sv,sf,sfLen) {
               if (sf[1] == 0x82 && sf[3] == 0x73)
                   ConvertAsciiToEbcdic(configParam, &sf[5], 2);
            }
            break; 

        case 0x96: /* Failure Cause */
            SetFailCause(idb, sv, "QLLC");   /* set common fail cause */
            ForAllSfInSV(sv,sf,sfLen) {
                if (sf[1] == 0x82) {
                    switch (sf[3]) {
                    case 0x56: /* DTE address called */
                        SetX25Address(&sf[5], &lci->lci_dst_addr);
                        break;
                    case 0x57: /* DTE address calling */
                        SetX25Address(&sf[5], &lci->lci_src_addr);
                        break;
                    case 0x67: /* default = locally initiated logical channel  */
                        sf[5] = lci->lci_lci >> 8;
                        sf[6] = lci->lci_lci;
                        if (!lci->lci_outgoing)
                            sf[3] = 0x68;   /* change subfield to remotely initiated */
                        break;
	           }
	       }   
	    }
            break;

        case 0x05: /* Hierarchy/Resource List */
            SetHRname(idb, sv, "QLLC");
            break;
        }
    }            
}                            


void Qllc_Alert(Qllc_AlertType alertType, qllctype* qllc)
{
   byte buf[MaxAlertSize];
   boolean createAlert = FALSE;
   const char* configParam = "";
   lcitype* lci = qllc->lci;

   if (lci == 0)
      return;

   switch(alertType){

       case QllcAlert1: /* Local station sent FRMR Y */
#ifdef QLLCALERT1_H
           memcpy(buf, (char*)&QLLCALERT1, sizeof(struct QllcAlert1)); 
           createAlert = TRUE;   
           configParam = "N1";
#endif
           break;

      case QllcAlert2: /* Local station sent FRMR Z */
#ifdef QLLCALERT2_H
           memcpy(buf, (char*)&QLLCALERT2, sizeof(struct QllcAlert2)); 
           createAlert = TRUE;  
#endif
           break;

      case QllcAlert3: /* Retry count expired */
#ifdef QLLCALERT3_H
           memcpy(buf, (char*)&QLLCALERT3, sizeof(struct QllcAlert3)); 
           createAlert = TRUE;  
           configParam = "N2";
#endif
           break;

      case QllcAlert4: /* FRMR W received */
#ifdef QLLCALERT4_H 
           memcpy(buf, (char*)&QLLCALERT4, sizeof(struct QllcAlert4)); 
           createAlert = TRUE;  
#endif
           break;

      case QllcAlert5: /* FRMR X received */
#ifdef QLLCALERT5_H
           memcpy(buf, (char*)&QLLCALERT5, sizeof(struct QllcAlert5)); 
           createAlert = TRUE;  
#endif
           break;

      case QllcAlert6: /* FRMR Y received */
#ifdef QLLCALERT6_H
           memcpy(buf, (char*)&QLLCALERT6, sizeof(struct QllcAlert6)); 
           createAlert = TRUE;  
           configParam = "N1";
#endif
           break;

     case QllcAlert7: /* FRMR Z received */
#ifdef QLLCALERT7_H
           memcpy(buf, (char*)&QLLCALERT7, sizeof(struct QllcAlert7)); 
           createAlert = TRUE;  
#endif
           break;

      case QllcAlert8: /* Local station sent FRMR W */
#ifdef QLLCALERT8_H
           memcpy(buf, (char*)&QLLCALERT8, sizeof(struct QllcAlert8)); 
           createAlert = TRUE;  
#endif
           break;

      case QllcAlert9: /* Local station sent FRMR X */
#ifdef QLLCALERT9_H
           memcpy(buf, (char*)&QLLCALERT9, sizeof(struct QllcAlert9)); 
           createAlert = TRUE;  
#endif
           break;
     }

     if (createAlert) {
         SetQllcParam(buf, lci, configParam);
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nQLLC ALERT Alert%d not supported",alertType + 1);
}
