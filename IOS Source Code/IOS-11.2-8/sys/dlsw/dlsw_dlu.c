/* $Id: dlsw_dlu.c,v 3.4.44.2 1996/05/17 10:45:45 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_dlu.c,v $
 *------------------------------------------------------------------
 * This file contains the necessary code for DLSW to act as a DLU.
 * DLSw subsystem 
 *
 * Fri July 29, 1994 G. Pandian
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * May-1 1994  Ramin Naderi 
 *
 * 
 * For Incoming traffic, messages are first categorized.  The Control
 * messages are placed on the dlsw_ctlQ. The information messages are
 * dispatched directly to the CORE module using the disp_to_fsm()
 * function call.
 * 
 * For Outgoing traffic, messages are queued directly to CLS.
 *
 * The global DLSw/CLS queue creation/initialization/maintenance
 * are also located within this module.
 *
 *------------------------------------------------------------------
 * $Log: dlsw_dlu.c,v $
 * Revision 3.4.44.2  1996/05/17  10:45:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.32.3  1996/04/26  07:34:21  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.32.2  1996/04/25  04:28:04  ioakley
 * CSCdi55673:  Reduce number of calls on DLSw LAN side and remove
 *              functions that are no longer useful or necessary.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.32.1  1996/04/03  14:01:35  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.44.1  1996/03/18  19:31:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/03/07  08:44:34  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.14.1  1996/02/20  13:46:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/15  17:18:24  fbordona
 * CSCdi45661:  DLSw+ peers show incorrect circuit count.
 *              Added checking for enable and disable of ports.
 *              Consolidate fsm to disp *stn structures.
 *              Consolidate fsm_to_disp() and cstm_to_disp().
 *              Improve local switching debug.
 *
 * Revision 3.3  1995/11/17  09:03:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:21:56  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:23:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:55:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/13  22:25:54  rnaderi
 * CSCdi41309:  router crashes when dlsw configured over frame-relay
 * CLSI message header was too big. Have to shift the message if necessary.
 *
 * Revision 2.4  1995/09/07  04:37:54  kpatel
 * CSCdi39871:  DLSw+ does not interoperate with remote window size
 *              of one
 *
 * Revision 2.3  1995/07/19  21:00:54  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Add registry service for callback to get QLLC's Activate Ring CLSI
 * message.
 * Move code around for default Activate ring, so that it doesn't fall in
 * the qllc path.
 *
 * Revision 2.2  1995/06/09  13:03:50  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:27:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../if/rif.h"
#include "../ui/debug.h"
#include "../cls/dlc_registry.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../dlsw/dlsw_dlu.h"

TCLSIQueue* gDLSW2CLSWriteQ;

static TCLSIQInit* gDLSWQInit;
static TCLSIQueue* gCLS2DLSWReadQ;
/*
static boolean gDLURunning;
static word gDLUPSapId;
static word gDLUPCepId;
*/

extern boolean dlsw_core_debug_msgs;

/*
 * Global Data
 */
queuetype dlsw_ctlQ;

/*
 * This routine will process the message coming from the CLS 
 * Data is handled in the fast path.
 * All the control information is handled thorugh the
 * slow path, that is, the dlsw_ctlQ.
 */
static int dlsw_dlu_rput(TCLSIQueue* dluReadQ, TCLSIMsg* theMsg)
{
hword primitive;
struct strbuf ctlbuf, databuf;
int flags;
struct str_core_rcvctl rcvctlbuf;
word retCode;
    
 /* Take care of the filtering. */
 if( !input_filter_check(theMsg))
  CLSIMsgDestroy(theMsg);

 primitive = CLSIMsgGetPrimitiveCode(theMsg);
 retCode = CLSIMsgGetReturnCode(theMsg);

 switch(primitive)
  {
   /* we will handle the data through the fast path */
   case CLS_DATA         | CLS_INDICATION:
   case CLS_DATA         | CLS_CONFIRM:
   case CLS_FLOW         | CLS_INDICATION:
    rcvctlbuf.prim_type  = DLSW_LAN_FSM;
    rcvctlbuf.len        = sizeof(struct str_core_rcvctl);
    ctlbuf.buf         = (char *) &rcvctlbuf;
    databuf.len        = 0;
    databuf.buf        = (char *) theMsg;
    flags              = 0;
    disp_to_fsm(rcvctlbuf.prim_type, (TCLSIMsg *)theMsg);
    break;

   /* The following will include all of the control cases */
   /* Control cases are handled through the dlsw_ctlQ     */
   /* 
    * CSCdi31293 - memory leak for NB UI frames and data corruption 
    * It is ideal for UDATA to go in the fast path if
    * it is NOT NB like NOVEL IPX...
    */
   case CLS_UDATA        | CLS_INDICATION:
   case CLS_UDATA        | CLS_CONFIRM:
   case CLS_ACTIVATE_RING  | CLS_CONFIRM:
   case CLS_DEACTIVATE_RING  | CLS_CONFIRM:
   case CLS_ENABLE         | CLS_CONFIRM:
   case CLS_DISABLE        | CLS_CONFIRM:
   case CLS_ACTIVATE_SAP   | CLS_CONFIRM:
   case CLS_DEACTIVATE_SAP | CLS_CONFIRM:
   case CLS_MODIFY_SAP     | CLS_CONFIRM:
   case CLS_REQ_OPNSTN     | CLS_CONFIRM:
   case CLS_CLOSE_STN      | CLS_CONFIRM:
   case CLS_ID_STN         | CLS_INDICATION:
   case CLS_ID             | CLS_CONFIRM:
   case CLS_ID             | CLS_INDICATION:
   case CLS_TEST           | CLS_INDICATION:
   case CLS_TEST_STN       | CLS_INDICATION:
   case CLS_TEST           | CLS_CONFIRM:
   case CLS_CONNECT_STN    | CLS_INDICATION:
   case CLS_UDATA_STN      | CLS_INDICATION:
   case CLS_UDATA_STN      | CLS_CONFIRM:
   case CLS_CONNECT        | CLS_INDICATION:
   case CLS_CONNECT        | CLS_CONFIRM:
   case CLS_CONNECTED      | CLS_INDICATION:
   case CLS_DISCONNECT     | CLS_INDICATION:
   case CLS_DISCONNECT     | CLS_CONFIRM:
    p_enqueue(&dlsw_ctlQ, theMsg);
    break;

   default: 
    buginf("\ndlu_rput: Unknown primitive %d.",primitive);
    CLSIMsgDestroy(theMsg);
    break;
  }
  return(CLS_OK);
}

int dlsw_dlu_init()
{
    int fail_flag;

    fail_flag = FALSE;  /* start fresh */

    queue_init(&dlsw_ctlQ, DLSW_DEF_EXPLQ_DEPTH);

    gDLSWQInit = CLSIQInitCreate("DLSWDLU", dlsw_dlu_rput);
    if (gDLSWQInit == NULL)
        return FALSE;

    gCLS2DLSWReadQ = CLSIQueueCreate(gDLSWQInit, NULL);
    if (gCLS2DLSWReadQ == NULL)
        return FALSE;

    gDLSW2CLSWriteQ = CLSOpen(gCLS2DLSWReadQ);
    if (gDLSW2CLSWriteQ == NULL)
        return FALSE;

    reg_add_ucep_cantput((uint)gCLS2DLSWReadQ, dlsw_cantput, "dlsw_cantput");
    return TRUE;
}


int dlsw_dlu_clean ()
{
    /*
     * Lets do house cleaning on the queues
     */
    CLSClose(gDLSW2CLSWriteQ);
    CLSIQueueDestroy(gCLS2DLSWReadQ);
    CLSIQInitDestroy(gDLSWQInit);
    process_kill(THIS_PROCESS);

    return TRUE;
}

/*
 * dlsw_queue_cls_message
 *
 * DLSw service to pass a message into CLSI
 */

void dlsw_queue_cls_message (TCLSIMsg* clsi_msg)
{
    if (clsi_msg != NULL) {
        if (gDLSW2CLSWriteQ == NULL) {
            CLSIMsgDestroy(clsi_msg);
        } else {
            CLSIQueuePut(gDLSW2CLSWriteQ, clsi_msg);
        }
    }
}

/*
 * Check for the filters in effect. 
 * TRUE  : Keep it.                      
 * FALSE : Dump it.                 
 */
int input_filter_check(TCLSIMsg *theMsg)
{
    return(TRUE);
}


