/* $Id: qllc_event.c,v 3.2.62.1 1996/05/17 12:13:20 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/qllc_event.c,v $
 *------------------------------------------------------------------
 * SNA Network Management QLLC Event Handler 
 *
 * Jan 95, Silvana Zdravkovski
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: qllc_event.c,v $
 * Revision 3.2.62.1  1996/05/17  12:13:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/26  07:58:25  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2  1995/11/17  18:55:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:01:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "address.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc_public.h"
#include "../util/avl.h"
#include "qllc.h"
#include "interface_generic.h"
#include "sys_registry.h"
#include "../dspu/snanm.h"
#include "../dspu/snanm_registry.regh"
#include "qllc_event.h"



void QllcEvent(qllctype *qllc, QLLC_EventType event)
{
    switch(event){
    case (QLLC_FRMR_Sent):
        if ((qllc->qllc_frmr->q_WXYZ & Q_Y) != 0)
            reg_invoke_Qllc_Alert(QllcAlert1, qllc);
        else if ((qllc->qllc_frmr->q_WXYZ & Q_X) != 0)
            reg_invoke_Qllc_Alert(QllcAlert9, qllc);
        else if ((qllc->qllc_frmr->q_WXYZ & Q_W) != 0)
            reg_invoke_Qllc_Alert(QllcAlert8, qllc); 
        break;

    case (N2_RetryCountExpired):
        reg_invoke_Qllc_Alert(QllcAlert3,qllc);
        break;

    case (QLLC_FRMR_Rcvd):
        if ((qllc->qllc_frmr->q_WXYZ & Q_Y) != 0)
            reg_invoke_Qllc_Alert(QllcAlert6, qllc);
        else if ((qllc->qllc_frmr->q_WXYZ & Q_X) != 0)
            reg_invoke_Qllc_Alert(QllcAlert5, qllc);
        else if ((qllc->qllc_frmr->q_WXYZ & Q_W) != 0)
            reg_invoke_Qllc_Alert(QllcAlert4, qllc); 
        break;
    }
}





