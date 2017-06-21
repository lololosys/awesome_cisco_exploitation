/* $Id: lnm_dlu.c,v 3.2.60.2 1996/05/17 11:20:40 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/lnm_dlu.c,v $
 *------------------------------------------------------------------
 * lnm_dlu.c -- CLSI Lan Manager Data Link User
 *
 * August 1995, Ivan P. Oakley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lnm_dlu.c,v $
 * Revision 3.2.60.2  1996/05/17  11:20:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.2  1996/04/26  07:54:17  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.1  1996/04/03  14:34:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  20:11:38  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:42:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  14:21:11  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:23:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:45:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/24  02:37:01  ioakley
 * Placeholder for new file.
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
#include "../if/tring.h"
#include "../ui/debug.h"
#include "../cls/clsi_msg.h"
#include "../cls/clsi_queue.h"
#include "../dlsw/dlsw_csm_externs.h"
#include "../ibm/lnm_dlu.h"
#include "../ibm/lanmgr.h"
#include "../ibm/ibmnm.h"

static TCLSIQInit* gLNMQInit;
static TCLSIQueue* gCLS2LNMReadQ;
static TCLSIQueue* gLNM2CLSWriteQ;

/* extern boolean lnm_cls_debug_msgs; */

queuetype lnm_ctlQ;

/*
 * Routine to process messages inbound from the CLS:
 * Data is processed in the fast path.
 * Control information is handled by means of the lnm_ctlQ.
 */
static int lnm_dlu_rput(TCLSIQueue* dluReadQ, TCLSIMsg* theMsg)
{
hword primitive;
struct str_lnm_buf ctlbuf, databuf;
int flags;
struct str_lnm_rcvctl rcvctlbuf;

    primitive = CLSIMsgGetPrimitiveCode(theMsg);
    
    switch(primitive) {
      case CLS_FLOW    |    CLS_INDICATION:
	  rcvctlbuf.prim_type   =    LNM_LAN_FSM;
	  rcvctlbuf.len         =    sizeof(struct str_lnm_rcvctl);
	  ctlbuf.buf            =    (char *) &rcvctlbuf;
	  databuf.len           =    0;
	  databuf.buf           =    (char *) theMsg;
	  flags                 =    0;
	  Lan_to_Lnm(&ctlbuf, &databuf, &flags);
	  break;
      case CLS_DATA               |    CLS_INDICATION:
      case CLS_DATA               |    CLS_CONFIRM:
      case CLS_UDATA              |    CLS_INDICATION:
      case CLS_UDATA              |    CLS_CONFIRM:
      case CLS_ACTIVATE_RING      |    CLS_CONFIRM:
      case CLS_DEACTIVATE_RING    |    CLS_CONFIRM:
      case CLS_ENABLE             |    CLS_CONFIRM:
      case CLS_DISABLE            |    CLS_CONFIRM:
      case CLS_ACTIVATE_SAP       |    CLS_CONFIRM:
      case CLS_DEACTIVATE_SAP     |    CLS_CONFIRM:
      case CLS_REQ_OPNSTN         |    CLS_CONFIRM:
      case CLS_CLOSE_STN          |    CLS_CONFIRM:
      case CLS_UDATA_STN          |    CLS_INDICATION:
      case CLS_UDATA_STN          |    CLS_CONFIRM:
      case CLS_CONNECT            |    CLS_INDICATION:
      case CLS_CONNECT            |    CLS_CONFIRM:
      case CLS_CONNECTED          |    CLS_INDICATION:
      case CLS_DISCONNECT         |    CLS_INDICATION:
      case CLS_DISCONNECT         |    CLS_CONFIRM:
	  p_enqueue(&lnm_ctlQ, theMsg);
	  break;

      default:
	  lnmllcbuginf("\nLNM_DLU: dlu_rput - CLSI message not recognized/supported.");
	  CLSIMsgDestroy(theMsg);
	  break;
      }
      return(CLS_OK);
}

/*
 * Process messages coming from LNM.
 */
void Lnm_to_Lan(ctlbuf, databuf, flags)
struct str_lnm_buf *ctlbuf;
struct str_lnm_buf *databuf;
int *flags;
{
int lresult_flag = FALSE;

struct str_lnm_sndctl *sndctlbuf;

struct str_lnm_rcvctl rcvctlbuf;
struct str_lnm_buf lctlbuf;
struct str_lnm_buf ldatabuf;
int    lflags;
char   *lstr;
lstr   = string_getnext();

    sndctlbuf = (struct str_lnm_sndctl *) ctlbuf -> buf;

    switch(sndctlbuf -> prim_type) {

      case LNM_INIT_LAN:
          if (sndctlbuf -> msg_type == I_STOP)
              lresult_flag = lnm_dlu_clean();
          else if (sndctlbuf -> msg_type == I_START)
              lresult_flag = lnm_dlu_init();
          else
              lresult_flag = FALSE;

	  if (lresult_flag == TRUE)
              rcvctlbuf.result = LFAIL;
	  else
              rcvctlbuf.result = LPASS;
	  
	  rcvctlbuf.prim_type = LNM_LAN_INIT;
	  lctlbuf.len         = sizeof(struct str_lnm_rcvctl);
	  lctlbuf.buf         = (char *) &rcvctlbuf;
	  ldatabuf.len        = 0;
	  ldatabuf.buf        = (char *) NULL;
	  lflags              = 0;
	  Lan_to_Lnm(&lctlbuf, &ldatabuf, &lflags);
	  break;

      case LNM_FSM_LAN:
	  CLSIQueuePut(gLNM2CLSWriteQ, (TCLSIMsg*) databuf -> buf);
	  break;

      default:
	  lnmllcbuginf("LNM_DLU: Lnm_to_Lan received unrecognized message\n");
	  break;
    }
}

int lnm_dlu_init()
{
int lfail_flag;

    lfail_flag = FALSE;

    queue_init(&lnm_ctlQ, LNM_DEF_Q_DEPTH);

    gLNMQInit = CLSIQInitCreate("LNMDLU", lnm_dlu_rput);
    if (gLNMQInit == NULL)
	return FALSE;

    gCLS2LNMReadQ = CLSIQueueCreate(gLNMQInit, NULL);
    if (gCLS2LNMReadQ == NULL)
        return FALSE;

    gLNM2CLSWriteQ = CLSOpen(gCLS2LNMReadQ);
    if (gLNM2CLSWriteQ == NULL)
	return FALSE;

    return(TRUE);
}

int lnm_dlu_clean()
{
    CLSClose(gLNM2CLSWriteQ);
    CLSIQueueDestroy(gCLS2LNMReadQ);
    CLSIQInitDestroy(gLNMQInit);
    process_kill(THIS_PROCESS);
    
    return(TRUE);

}
