/* $Id: hdlc_alert.c,v 3.2.60.2 1996/05/17 10:48:31 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/hdlc_alert.c,v $
 *------------------------------------------------------------------
 * SNA Network Management HDLC Alert Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hdlc_alert.c,v $
 * Revision 3.2.60.2  1996/05/17  10:48:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:35:18  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:03:18  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:33:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:45:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:05:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "interface_generic.h"
#include "../snanm/hdlc_keepalive_alert.alth"
#include "../snanm/hdlc_signallost_alert.alth"
#include "mv_sr.h"
#include "snautil.h"
#include "registry.h"
#include "../dspu/snanm_registry.regh"
#include "lan_alert.h"


void Dlc_Alert(Dlc_AlertType alertType, hwidbtype *idb)
{
    byte buf[MaxAlertSize];
    boolean createAlert = FALSE;

   /* Dspu almost guarantee a buf available,
    * this should not happen
    */

   if (idb->failCause == idb->lastFailCause)
        return;                             /* same error, don't log it */

    switch(alertType){

      case Hdlc_keepAliveAlert:
      case Lapb_keepAliveAlert:
#ifdef HDLCKEEPALIVEALERT_H
         memcpy(buf, (char*)&HDLCKEEPALIVEALERT, sizeof(struct HdlcKeepAliveAlert));
         createAlert = TRUE;
#endif
         break;

      case Hdlc_signalLostAlert:
#ifdef HDLCSIGNALLOSTALERT_H
         memcpy(buf, (char*)&HDLCSIGNALLOSTALERT, sizeof(struct HdlcSignalLostAlert));
         createAlert = TRUE;
#endif
         break;
    }

    if (createAlert) {
        SetLanParam(buf, idb, alertType == Lapb_keepAliveAlert? "LAPB": "HDLC");
        MV_SendAlert((MV*)buf);
    }
    else 
        printf("\nHDLC  ALERT Alert%d not supported",alertType + 1);
}

