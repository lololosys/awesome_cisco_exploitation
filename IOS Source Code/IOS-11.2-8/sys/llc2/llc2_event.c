/* $Id: llc2_event.c,v 3.2.62.1 1996/07/31 06:48:37 gcox Exp $
 * $Source: /release/112/cvs/Xsys/llc2/llc2_event.c,v $
 *------------------------------------------------------------------
 * SNA Network Management LLC2 Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: llc2_event.c,v $
 * Revision 3.2.62.1  1996/07/31  06:48:37  gcox
 * CSCdi64685:  Convert LLC2 to interface_generic.h (except llc2.c)
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:44:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:35:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:42:05  hampton
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
#include "interface_generic.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "llc2.h"
#include "llc2_event.h"


       
void LLC2Event(llctype *llc, LLC2_EventType event)
{

    switch(event){
    case AckTimerExpired:
        reg_invoke_Lanllc_Alert(LanllcAlert1, llc);
        break;

    case DMReceived:
        reg_invoke_Lanllc_Alert(LanllcAlert2, llc);
        break;

    case SABMEReceivedWhileConnected:
        reg_invoke_Lanllc_Alert(LanllcAlert3, llc);  
        break;

    case FRMRreceived:
        if (llc->frmrchars[FRMR_WXYZ] == FRMR_WBIT)
            reg_invoke_Lanllc_Alert(LanllcAlert4, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == (FRMR_WBIT+FRMR_XBIT))
            reg_invoke_Lanllc_Alert( LanllcAlert5, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == (FRMR_ZBIT))
            reg_invoke_Lanllc_Alert(LanllcAlert6, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == FRMR_YBIT)
            reg_invoke_Lanllc_Alert(LanllcAlert7, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == FRMR_VBIT+FRMR_WBIT)
            reg_invoke_Lanllc_Alert(LanllcAlert4, llc);
        break;
 
    case FRMRsent:
        if (llc->frmrchars[FRMR_WXYZ] == FRMR_WBIT) 
            reg_invoke_Lanllc_Alert(LanllcAlert8, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == (FRMR_WBIT+FRMR_XBIT))
            reg_invoke_Lanllc_Alert(LanllcAlert9, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == FRMR_ZBIT)
            reg_invoke_Lanllc_Alert(LanllcAlert10, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == FRMR_YBIT)
            reg_invoke_Lanllc_Alert(LanllcAlert11, llc);
        else if (llc->frmrchars[FRMR_WXYZ] == FRMR_VBIT+FRMR_WBIT)
            reg_invoke_Lanllc_Alert(LanllcAlert8, llc);
        break;
     }
}


      
