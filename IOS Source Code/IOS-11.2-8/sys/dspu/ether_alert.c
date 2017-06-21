/* $Id: ether_alert.c,v 3.2.60.2 1996/05/17 10:48:29 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/ether_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Ethernet Alert Handler
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ether_alert.c,v $
 * Revision 3.2.60.2  1996/05/17  10:48:29  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:35:17  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:03:14  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:33:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:45:49  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:05:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_generic.h"
/*   Open phase and continuous carrier not supported 
#include "../snanm/ether_alert1.alth"   open, no carrier 
#include "../snanm/ether_alert2.alth"   open, duplicate address
#include "../snanm/ether_alert3.alth"   open, remove station
#include "../snanm/ether_alert4.alth"   open, not 1,2 or 3
#include "../snanm/ether_alert6.alth"   continuous carrier, sender not the cause
#include "../snanm/ether_alert8.alth"   continuous carrier, sender the cause
#include "../snanm/ether_alert9.alth"   continuous carrier, cause unknown
#include "../snanm/ether_alert11.alth"  continuous carrier, cause remote
*/
#include "../snanm/ether_alert5.alth"  
#include "../snanm/ether_alert6.alth"
#include "../snanm/ether_alert7.alth"
#include "../snanm/ether_alert10.alth"
#include "../snanm/ether_alert12.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"


void Ether_Alert(Ether_AlertType alertType, hwidbtype* idb)
{
   byte buf[MaxAlertSize];
   boolean createAlert = FALSE;

   if (idb->failCause == idb->lastFailCause)
        return;                             /* same erro, dont log it */

   switch(alertType){
      case Ether_Alert1: /* Open failure, insertion process failed */
#ifdef ETHER1_H
         memcpy(buf, (char*)&ETHER1, sizeof(struct Ether1));
         createAlert = TRUE;
#endif
        break;

      case Ether_Alert2: /* Open failure, duplicate address */
#ifdef ETHER2_H
         memcpy(buf, (char*)&ETHER2, sizeof(struct Ether2));
         createAlert = TRUE;        
#endif
         break;

      case Ether_Alert3:
#ifdef ETHER3_H
         memcpy(buf, (char*)&ETHER3, sizeof(struct Ether3));
         createAlert = TRUE;
#endif
         break;

      case Ether_Alert4:
#ifdef ETHER4_H
         memcpy(buf, (char*)&ETHER4, sizeof(struct Ether4));
         createAlert = TRUE;
#endif
         break;

      case Ether_Alert5:   /* admin shutdown */
#ifdef ETHER5_H
         memcpy(buf, (char*)&ETHER5, sizeof(struct Ether5));
         createAlert = TRUE;
#endif
         break;

      case Ether_Alert6:  /* continuous carrier */
#ifdef ETHER6_H
         memcpy(buf, (char*)&ETHER6, sizeof(struct Ether6));
         createAlert = TRUE;
#endif
         break;

     case Ether_Alert7:   /* lost carrier */
#ifdef ETHER7_H
         memcpy(buf, (char*)&ETHER7, sizeof(struct Ether7));
         createAlert = TRUE;
#endif
         break;

      case Ether_Alert8:  /* continuous carrier, isolated to local */
#ifdef ETHER8_H
         memcpy(buf, (char*)&ETHER8, sizeof(struct Ether8));
         createAlert = TRUE;
#endif
         break;

      case Ether_Alert9: /* continuous carrier, source not isolated */
#ifdef  ETHER9_H
         memcpy(buf, (char*)&ETHER9, sizeof(struct Ether9)); 
         createAlert = TRUE;
#endif
         break;

      case Ether_Alert10:   /* excessive collision */
#ifdef ETHER10_H
         memcpy(buf, (char*)&ETHER10, sizeof(struct Ether10));
         createAlert = TRUE;
#endif
         break;

      case Ether_Alert11:  /* continuous carrier */
#ifdef ETHER11_H
         memcpy(buf, (char*)&ETHER11, sizeof(struct Ether11));
         createAlert = TRUE;
#endif
         break;
     case Ether_Alert12:   /* linkdown, fatal error, late collision */
#ifdef ETHER12_H
         memcpy(buf, (char*)&ETHER12, sizeof(struct Ether12));
         createAlert = TRUE;
#endif
         break;
     }
     
     if (createAlert){
         SetLanParam(buf,idb, "ETHER");         
         MV_SendAlert((MV*)buf);
     }
     else
        printf("\nEther_Alert%d not supported",alertType + 1);

}




