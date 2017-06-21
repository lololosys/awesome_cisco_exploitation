/* $Id: lapb_event.c,v 3.2.62.1 1996/08/07 06:21:37 szdravko Exp $
 * $Source: /release/112/cvs/Xsys/x25/lapb_event.c,v $
 *------------------------------------------------------------------
 * SNA Network Management LAPB Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lapb_event.c,v $
 * Revision 3.2.62.1  1996/08/07  06:21:37  szdravko
 * CSCdi62268:  Wrong output in NetMaster console coming from NSP
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:06:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:21:08  hampton
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
#include "lapb_event.h"
#include "lapb.h"


void LAPBEvent(hwidbtype *idb, LAPB_EventType event)
{
    /* LAPB alerts are only sent if X.25 packet-level 
     * encapsulation is used. i.e. attaching to an X.25 network. 
     * These are the only LAPB alerts defined.  There are
     * no alerts defined for using LAPB as a serial encapsulation 
     * method or using LAPB as the reliable transport for
     * PPP encapsulation.
     */
    if (is_x25(idb)) {
        switch (event) {
        case LocalSentFRMR_W:
            reg_invoke_Lapb_Alert(LapbAlert10, idb);
            break;
        case LocalSentFRMR_X:
            reg_invoke_Lapb_Alert(LapbAlert11, idb);
            break;

        case LocalSentFRMR_Y:
            reg_invoke_Lapb_Alert(LapbAlert1, idb);
            break;

        case LocalSentFRMR_Z:
            reg_invoke_Lapb_Alert(LapbAlert2, idb);
            break;

        case LocalReceivedFRMR_X:
            reg_invoke_Lapb_Alert(LapbAlert7, idb);
            break;

        case LocalReceivedFRMR_W:
            reg_invoke_Lapb_Alert(LapbAlert6, idb);
            break;

        case LocalReceivedFRMR_Y:
            reg_invoke_Lapb_Alert(LapbAlert8, idb);  
            break;
 
        case LocalReceivedFRMR_Z:
            reg_invoke_Lapb_Alert(LapbAlert9, idb); 
            break;

        case RetryCountExpired:
            reg_invoke_Lapb_Alert(LapbAlert3, idb);
            break;

        case DISCReceived:
            reg_invoke_Lapb_Alert(LapbAlert4, idb);
            break;

        case LocalSentSABM_ReceivedDM: /* Not Supported by LAPB */
            reg_invoke_Lapb_Alert(LapbAlert5, idb); 
            break;
	}
    }
}
 
      
