/* $Id: tr_alert.c,v 3.2.60.2 1996/05/17 10:49:58 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/tr_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Token Ring Alert handler
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tr_alert.c,v $
 * Revision 3.2.60.2  1996/05/17  10:49:58  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:35:31  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:04:09  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:34:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:06:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:31:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_generic.h"
#include "../if/tring_common.h"

/* Both 9 and 10 are beaconing with one shorter than hard-error.
 * We cannot distinguish the difference.
#include "../snanm/tr_alert9.alth"
*/
#include "../snanm/tr_alert1.alth"
#include "../snanm/tr_alert2.alth"
#include "../snanm/tr_alert3.alth"
#include "../snanm/tr_alert4.alth"
#include "../snanm/tr_alert5.alth"
#include "../snanm/tr_alert6.alth"
#include "../snanm/tr_alert7.alth"
#include "../snanm/tr_alert8.alth"
#include "../snanm/tr_alert10.alth"
#include "../snanm/tr_alert14.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"


void TR_Alert(TR_AlertType alertType, hwidbtype* idb)
{
   byte buf[MaxAlertSize];
   boolean createAlert = FALSE;

   if (idb->failCause == idb->lastFailCause)
        return;                             /* same erro, dont log it */

  switch(alertType){
      case TrAlert1:    /* lobe test failure during insertion process */
#ifdef TRALERT1_H
          memcpy(buf, (char*)&TRALERT1, sizeof(struct TrAlert1));
          createAlert = TRUE;
#endif
          break;

      case TrAlert2:   /* open phase, ring beacon */
#ifdef TRALERT2_H
          memcpy(buf, (char*)&TRALERT2, sizeof(struct TrAlert2));
          createAlert = TRUE;
#endif
         break;

      case TrAlert3:   /* open phase, duplicate address */
#ifdef TRALERT3_H
          memcpy(buf, (char*)&TRALERT3, sizeof(struct TrAlert3));
          createAlert = TRUE;
#endif
          break;

      case TrAlert4:   /* open phase, remove adaptor cmd received */
#ifdef TRALERT4_H
          memcpy(buf, (char*)&TRALERT4, sizeof(struct TrAlert4));
          createAlert = TRUE;
#endif
         break;

      case TrAlert5:   /* open phase, not 1,2,3 or 4 */
#ifdef TRALERT5_H
          memcpy(buf, (char*)&TRALERT5, sizeof(struct TrAlert5));
          createAlert = TRUE;
#endif
          break;

      case TrAlert6:   /* wire fault */
#ifdef TRALERT6_H
          memcpy(buf, (char*)&TRALERT6, sizeof(struct TrAlert6));
          createAlert = TRUE;
#endif
         break;

     case TrAlert7:    /* remote removal */
#ifdef TRALERT7_H
          memcpy(buf, (char*)&TRALERT7, sizeof(struct TrAlert7));
          createAlert = TRUE;        
#endif
          break;

      case TrAlert8:   /* remove adaptor received, shutdown */
#ifdef TRALERT8_H
          memcpy(buf, (char*)&TRALERT8, sizeof(struct TrAlert8));
          createAlert = TRUE; 
#endif
         break;

      case TrAlert9:   /* ring beacon, not supported */
#ifdef TRALERT9_H
          memcpy(buf, (char*)&TRALERT9, sizeof(struct TrAlert9));
          createAlert = TRUE; 
#endif
         break;

      case TrAlert10:  /* ring beacon */
#ifdef TRALERT10_H
          memcpy(buf, (char*)&TRALERT10, sizeof(struct TrAlert10));
          createAlert = TRUE;
#endif
         break;

      case TrAlert14:   /* keep alive failure */
#ifdef TRALERT14_H
          memcpy(buf, (char*)&TRALERT14, sizeof(struct TrAlert14));
          createAlert = TRUE;
#endif
         break;
      default:
          break;
      }
     
     if (createAlert){
         SetLanParam(buf, idb, "TR");
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nToken-Ring LAN Alert%d not supported",alertType + 1);

}
