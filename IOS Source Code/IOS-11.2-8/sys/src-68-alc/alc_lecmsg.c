/* $Id: alc_lecmsg.c,v 3.2.62.7 1996/08/03 23:36:16 fox Exp $
 * $Source: /release/112/cvs/Xsys/src-68-alc/alc_lecmsg.c,v $
 *------------------------------------------------------------------
 * alc_lecmsg.c -- Primitives to support messaging between the LANE
 *	and earl task running on the NMP.
 *
 * June 9, 1995.  Ratinder Ahuja.
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: alc_lecmsg.c,v $
 * Revision 3.2.62.7  1996/08/03  23:36:16  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.2.62.6  1996/07/16  21:43:28  sakumar
 * CSCdi62646:  LES-BUSes can prevent LECs from starting via VTP
 * Branch: California_branch
 *
 * Revision 3.2.62.5  1996/06/29  00:06:13  michellw
 * CSCdi61611:  lane_configure_client param list disagrees with ACP.
 * Branch: California_branch
 * Pass in 0 for vr_rn and srb_bn.
 *
 * Revision 3.2.62.4  1996/06/19  19:30:17  snyder
 * CSCdi60807:  spelllink errors
 * Branch: California_branch
 *
 *         unexpeced -> unexpected
 *
 * Revision 3.2.62.3  1996/06/18  16:43:50  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/06/06  21:47:01  sakumar
 * CSCdi59775:  ATM 3.1 VTP Helper deletes LES/BUS when only LEC to be
 * deleted
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/05/09  14:41:01  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:25:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.6  1996/04/26  01:25:21  sakumar
 * Code changes after review of VTP helper code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.5  1996/04/16  17:59:25  sakumar
 * If VTP message from NMP indicates delete VLAN, then check if swidb
 * exists for VLANID and if one exists for ELAN-NAME and delete both
 * if they are different.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.4  1996/04/15  21:21:40  sakumar
 * Support for VTP on NMP creating/destroying LECs on Cat5K ATM Module.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.3  1996/04/11  05:31:17  rlowe
 * Add LCP driver and upcall support for VTP messages (over SCP).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.2  1996/04/08  14:49:55  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:08:49  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.2  1996/03/22  23:40:09  rlowe
 * Give relative paths to wbu include files.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.1  1996/03/22  09:43:34  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.4  1996/03/20  10:48:46  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Also fold old Synergy atmtest.c content into vcdtest.c
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.3  1996/03/11  08:10:26  rlowe
 * More LANE merger fun and games.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.2  1996/03/08  01:14:05  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix Synergy/Router conflicts for LANE.
 *
 * Revision 3.2.52.1  1996/03/03  21:24:49  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:21:01  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:11  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:42:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:24:47  shaker
 * Bump version numbers from 2.x to 3.x.
 * Revision 1.1.68.1  1996/02/03  07:08:28  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 1.1.50.3.2.1  1996/01/19  21:57:26  sgarcia
 * CSCdi47203:  sync 11.1 catchup branch to 11.0 ATMFLASH_V11_951214 tag
 * of Synalc2_
 * Branch: Synalc_catchup_branch
 * Patch 11.0 Synalc2_LE2_V110_1_0_3_branch diffs between tags
 * Synalc2_LE2_V110_1_0_3_arizona_arkansas_split_951103 and
 * ATMFLASH_V11_951214
 * into 11.1 branch.
 *
 * Revision 1.1.50.5  1995/12/06  22:00:35  sakumar
 * When a LEC is brought down, the DESTROY PORT response message from the
 * Catalyst 5000 supervisor is now deallocated correctly.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 1.1.50.4  1995/11/08  02:48:13  rlowe
 * Change to use LEIPC_PORT_STATE_CHANGE_IND for NMP spanning tree
 * messages.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 1.1.50.3  1995/10/24  18:27:23  sakumar
 * Since the out-of-band Port BLOCK/UNBLOCK messages arrive in the LECs IPC
 * queue, we need to put a fake IBIPC header.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 1.1.50.2  1995/09/28  00:47:12  bbenson
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 * Remove lec_ipc_inputQ_enabled.
 *
 * Revision 1.1.50.1  1995/09/11  01:57:04  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 1.1.34.2  1995/09/02  01:56:44  bbenson
 * Branch: Synalc2_branch
 * Sync in LANE for the new improved Synalc branch.
 *
 * Revision 1.1.34.1  1995/08/22  17:51:50  lcheung
 * move file from Synergy_atmlc_branch to Synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 1.1.2.6  1995/07/26  03:22:37  rlowe
 * Add "leipc_portstate_ind" as upcall for LCP driver to inform LANE of
 * NMP spanning tree state changes.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.5  1995/07/14  23:39:24  sakumar
 * Do a VCD->VPC conversion for LEIPC_REMOVE_MACS_ON_VCD case.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.4  1995/07/13  21:57:02  bbenson
 * Branch: Synergy_ATMlc_branch
 * lec_ipc.h moved to syn_common.
 *
 * Revision 1.1.2.3  1995/07/05  22:41:54  sakumar
 * Set MAC message to the NMP now contains the Card Instance Number in
 * bits 13 .. 15.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1.2.2  1995/06/15  21:02:46  rahuja
 * Branch: Synergy_ATMlc_branch
 * Committing working version of Synergy LANE.
 *
 * Revision 1.1.2.1  1995/06/09  23:46:39  sakumar
 * Initial check-in of real, functional code.
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/06/09  23:39:30  sakumar
 * Initial check-in of stub files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*********************************************************************
 *
 *  FILE: alc_lecmsg.c
 *
 *  CONTENTS:
 *		This file provides primitves to the LEC for exchanging messges
 *	with the earl and bridge tasks
 *
 *  OWNERS:
 *
 *      Ratinder P. S. Ahuja
 */
 
 
/*
**
**  INCLUDES
**
*/
 
 
#ifndef LANGUAGE_C
#define LANGUAGE_C
#endif
#include	"master.h"
#include	"mgd_timers.h"
#include 	"sys_registry.h"
#include	"interface_private.h"
#include	"types.h"
#include	"globs.h"
#include	"packet.h"
#include	"address.h"
#include	"config.h"

#include	"../if/atm.h"
#include	"../if/ether.h"
#include	"../if/tring.h"
#include	"../if/network.h"
#include	"../atm/atmsig_api.h"
#include	"../atm/ilmi_api.h"

#include	"../lane/lane.h"
#include	"../lane/lane_private.h"
#include	"../lane/lec_private.h"
#include	"../lane/lane_registry.h"

#include 	"../../wbu/cat5k/alc_cominc/basic_types.h"
#include	"../../wbu/cat5k/alc_cominc/sar_if.h"
#include	"../../wbu/cat5k/sysif/lec_ipc.h"
#include	"../../wbu/cat5k/alc_cominc/atmdrv_if.h"
#include	"../../wbu/cat5k/sysif/scp_msgs.h"

#include	"ios_atmdriver.h"


/*
**
**  EXTERNS
**
*/


#define	mLEIPC_PRINTF	buginf

extern  UINT16	LCP_CardInstanceNbr(void);
extern  hwidbtype * atmSyn_hwidb;

/*
**
**  GLOBALS
**
*/

#define	NMP_SLOTID		1		/* destination card slot    */

void    *pQ = NULLP;
UINT32 leipc_handle;
UINT32 leipc_mtu;
UINT32 src_tid;
UINT32 dst_tid;
UINT32 dst_slot;

static pid_t    Syn_vtp_helper_pid;
static forktype Syn_vtp_helper_process(void);
watched_queue * Syn_vtpHelperInputQ;
boolean         Syn_vtpHelperInputQ_enabled = FALSE;



/*
**
**  STATIC
**
*/
 
 

/*
**
**  PROTOTYPES
**
*/

void	leipc_rx_handler(void	*pMsg);
UINT32 	leipc_sendmsg(tpLEIPCHDR    pLeipcMsg, UINT32 msglen);
boolean alc_vtp_lec_create_or_destroy (VLAN_PARAMS * pVTPMsg);
void alc_lec_remove (idbtype* swidb);




void
leipc_txdone_ack(char *pbuf)
{

    Ipc_AppMsgFree((uChar *)pbuf);

}/*end leipc_txdone_ack*/

void
leipc_txdone_nack(char *pbuf)
{

    Ipc_AppMsgFree((uChar *)pbuf);

}/*end leipc_txdone_nack*/


/*
*******************************************************************************
*
*   FUNCTION:
*           leipc_init
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           OK or ERR
*
*   ALGORITHMS:
*
*           Register sap with IPC
*           Record the recv queue descriptor
*
*******************************************************************************
*/
 
UINT32 leipc_init(void)
{
    UINT32 rc;
 
    /*
    **  Register to use blocking calls
    */

    rc = Ipc_AppRegLink ( IBIPC_ALC_LANE_ID,	/* source ID                */
                        IBIPC_NMP_LANE_ID,      /* destination ID           */
                        NMP_SLOTID,             /* destination card slot    */
                        NULLP,			/*leipc_txdone_ack Tx ACK callback  */
                        NULLP,			/*leipc_txdone_nack Tx NAK callback */
                        leipc_rx_handler,       /* Rx upcalls               */
                        NULLP,                  /* pass-through callback    */
                        IPC_IN_BAND,            /* use inband channel       */
                        (uInt32 *)&leipc_handle,/* handle returned by ipc   */
                        (uInt32 *)&leipc_mtu);  /* ptr return MTU size      */
   
    return rc;
 
}/* end leipc_init */
 
 


/*
*******************************************************************************
*
*   FUNCTION:
*           leipc_term
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           OK or ERR
*
*   ALGORITHMS:
*
*           Un register with IPC
*
*******************************************************************************
*/
 
void
leipc_term()
{
UINT32 rc;
 
    rc = Ipc_AppDeRegister(leipc_handle);
 
    if (rc)
        mLEIPC_PRINTF("IPC de-registration failure : %d\n", rc);
 
 
}/* end leipc_term */
 
 

/*
*******************************************************************************
*
*   FUNCTION:
*           leipc_rx_handler
*
*   PARAMETERS:
*           char *pBuf
*
*   RETURN:
*           none
*
*   ALGORITHMS:
*
*           Call back from IPC entity.  Enq message on service users
*   ipc message queue and return
*
*******************************************************************************
*/
 
void
leipc_rx_handler(void *pBuf)
{
    paktype *pak = pBuf;

    if(lec_inputQ_enabled) {
       	process_enqueue_pak(lec_ipc_inputQ, pak);
    } else {
        datagram_done(pak);
    }
 
}/*end leipc_rx_handler*/




/*
*******************************************************************************
*
*   FUNCTION:
*           leipc_sendmsg
*
*   PARAMETERS:
*           tpLEIPCHDR  pLeipcMsg       message to be sent to peer
*
*   RETURN:
*           none
*
*   ALGORITHMS:
*
*           Used to send response message and async events over ipc to the
*   peer task.  Returns the error code.
*
*   NOTE::  The buffer is freed in this routine
*
*******************************************************************************
*/
UINT32
leipc_sendmsg(tpLEIPCHDR    pLeipcMsg, UINT32 msglen)
{
    UINT32 rc;
 
    rc = Ipc_AppSend (leipc_handle,     	/* registered handle        */
		      IBIPC_NMP_LANE_ID,	/* destination ID           */
		      (uChar)NMP_SLOTID,	/* destination slot         */
		      /* type of service          */
		      (uChar)(IPC_DATAGRAM),
		      /*(uChar)(IPC_TYPE3 |IPC_NON_BLOCK_MASK)*/
		      (uInt32)msglen,         /* message length after     */
		      /* application header       */
		      (uChar *) pLeipcMsg     /* message buffer ie.       */
                    );				/*  application hdr)        */

    Ipc_AppMsgFree((uChar *)pLeipcMsg);

    return rc;
 
}/*end leipc_sendmsg*/
 



/*
*******************************************************************************
*
*   FUNCTION:	leipc_send_earl_req
*
*   PARAMETERS:	tREQID 
*		UINT32		req_type	get, set, clear
*		tpMACADDR
*		tCOLOR
*		tVPCD	
*
*   RETURN:
*           
*
*   ALGORITHMS:	Allocate an IPC message buffer, fill in earl parameters and
*		request.  The request is one of : 
*
*				LEIPC_SET_MAC_REQ
*				LEIPC_GET_MAC_REQ
*				LEIPC_CLEAR_MAC_REQ
*
*   NOTE::  The buffer is freed in this routine
*
*******************************************************************************
*/
 
UINT32  
leipc_send_earl_req(UINT32 		reqid, 
		    UINT32		req_type,
		    tpMACADDR	pMac, 
		    tCOLOR		color,
		    UINT16          vcd)
{
    tpLEIPCHDR		pHdr;
    tpEARL_PARAM	pEparm;
    UINT32		rc = OK;
    UINT16		cardid;

	
    if ( (pHdr = (tpLEIPCHDR)Ipc_AppMsgMalloc(mLEIPC_HSZ + sizeof(tEARL_PARAM)))
	== NULLP ) {
	mLEIPC_PRINTF("\n\rleipc_getmac_req: Ipc_AppMsgMalloc failed");
	return !OK;
    }

    /*
     **	Fill in header
     */

    pHdr->leipc_msgtype = req_type;
    pHdr->leipc_reqid = reqid;
    pEparm = (tpEARL_PARAM)pHdr->leipc_msgparam;

    /*
     **	Fill in paramenters
     */

    pEparm->color = color;
    if( (req_type == LEIPC_SET_MAC_REQ) ||
       (req_type == LEIPC_REMOVE_MACS_ON_VCD) ) { 
	if ( (pEparm->vpci = get_vpc_from_vcd(0, vcd)) == VC_INVALID ) {
	    return !OK;
	} 
    }
    cardid = (UINT16)LCP_CardInstanceNbr();
    cardid <<= 13;
    pEparm->vpci |= cardid;
    memcpy(pEparm->macaddr, pMac, sizeof(tMACADDR));
	
    rc = leipc_sendmsg(pHdr, (mLEIPC_HSZ + sizeof(tEARL_PARAM)));

    return rc;

}/* end leipc_send_earl_req */




/*
*******************************************************************************
*
*   FUNCTION:
*           leipc_send_spt_req
*
*   PARAMETERS:
*               tREQID
*               UINT32      req_type    create, destroy
*              	tCOLOR		lecid
*
*   RETURN:
*           none
*
*   ALGORITHMS:
*
*           Allocate an IPC message buffer, fill in spanning tree
*	 parameters and request.  The request is one of :
*
*		LEIPC_PORT_CREATE_REQ
*		LEIPC_PORT_DESTROY_REQ
*
*   NOTE::  The buffer is freed in this routine
*
*******************************************************************************
*/
 
UINT32
leipc_send_spt_req(UINT32      reqid,
                   UINT32      req_type,
				   tCOLOR		lecid)
{

    tpLEIPCHDR      pHdr;
    tpSPT_PARAM     pParam;
    UINT32          rc = OK;
 
    if ( (pHdr = (tpLEIPCHDR)Ipc_AppMsgMalloc(mLEIPC_HSZ + sizeof(tSPT_PARAM)))
	== NULLP ) {
        mLEIPC_PRINTF("\n\rleipc_getmac_req: Ipc_AppMsgMalloc failed");
        return !OK;
    }
 
    /*
    **  Fill in header
    */
 
    pHdr->leipc_msgtype = req_type;			/* requested operation			*/
    pHdr->leipc_reqid = reqid;
    pParam = (tpSPT_PARAM)pHdr->leipc_msgparam;
 
    /*
    **  Fill in paramenters
    */
 
    pParam->port_color = lecid;
 
    rc = leipc_sendmsg(pHdr, (mLEIPC_HSZ + sizeof(tSPT_PARAM)));
 
    return rc;

}/*end leipc_send_spt_req */


/*
*******************************************************************************
*
*   FUNCTION:
*           leipc_portstate_ind
*
*   PARAMETERS:
*               
*				UINT32		color
*				UINT32		portstate
*				UINT32		portnum
*
*   RETURN:
*           	0 on success
*
*   ALGORITHMS:
*
*           Allocate an paktype buffer, fill in the port state
*    parameters and indication.  The ind is one of :
*
*       LEIPC_PORT_BLOCK_IND
*       LEIPC_PORT_DESTROY_REQ
*
*   NOTE::  The buffer is freed in this routine
*
*******************************************************************************
*/
 
UINT32 leipc_portstate_ind (UINT32      color,
			    UINT32      portstate,
			    UINT32	portnum)
{

    paktype *pak = NULL;
    tpLEC_IPCMSG	pMsg;
 
    if(!lec_inputQ_enabled) {
	return 0;
    }

    /*
     **	Allocate a paktype,  cast the datagram start to tpLEC_IPCMSG
     */
    if ( !(pak = getbuffer(sizeof(tLEC_IPCMSG) + sizeof(IBIPC_PKT_HDR_T))) ) {
	return 0;
    }

    pak->datagramsize = sizeof(tLEC_IPCMSG) + sizeof(IBIPC_PKT_HDR_T);
    pMsg = (tpLEC_IPCMSG)(pak->datagramstart + sizeof(IBIPC_PKT_HDR_T));

    pMsg->leipc_msgtype = LEIPC_PORT_STATE_CHANGE_IND ;
    pMsg->leipc_reqid = 0;

    pMsg->param.spanning_msg.port_color = color;
    pMsg->param.spanning_msg.port_state = portstate ;
    pMsg->param.spanning_msg.errcode = SPTIPC_OK;

    process_enqueue_pak(lec_ipc_inputQ, pak);

    return 0;

}/*end leipc_portstate_ind*/

/*
*******************************************************************************
*
*   FUNCTION:
*           vtpipc_vlanparams_cmd
*
*   PARAMETERS:
*				UINT8		* pVPC
*
*   RETURN:
*           	FALSE on failure
*           	TRUE  on success
*
*   ALGORITHMS:
*           Allocate a paktype buffer, fill in the Vlan Parameters, and
*           enqueue to VTP Helper input queue.
* 
*
*   NOTE::  The LCP buffer is NOT released in this routine, which is
*           called by the LCP driver at interrupt-level.
*
*           Also, a MODIFY message is converted to ADD or DELETE
*
*******************************************************************************
*/
 
UINT32 vtpipc_vlanparams_cmd (UINT8 * pVPC)
{
    paktype *pak = NULL;
    VLAN_PARAMS    * pVTPMsg;
    unsigned char action;
    UINT32        retcode = TRUE;
 
    if(!Syn_vtpHelperInputQ_enabled) {
	return (FALSE);
    }

    pVTPMsg = (VLAN_PARAMS *) pVPC;

    if (pVTPMsg->vlan_num > 1000) {
        return (FALSE);
    }
    action = pVTPMsg->vlan_info_mode;

    /* If this VLAN setup to run over PVC, return TRUE */
    if (alc_is_vlan_on_PVCs(pVTPMsg->vlan_num)) {
        return (TRUE);
    }

    if (pVTPMsg->type != VTP_VLAN_ETH_TYPE) {
        idbtype*      swidb  = NULL;
        idbtype*      swidb2 = NULL;
        /* 
         * This is a tricky situation. Currently we only do LANE for Ethernet 
         * and since this command is asking for an operation other than
         * for Ethernet, we will delete any Ethernet LECs relating
         * to the command (i.e. with the same VLANID or ELAN NAME 
         * and return FALSE.
         */
        swidb = reg_invoke_find_lec_by_name_on_interface(pVTPMsg->name,
                                  pVTPMsg->name_len, atmSyn_hwidb->firstsw);

        swidb2 = reg_invoke_find_lec_by_vlanid_on_interface(pVTPMsg->vlan_num,
                                  atmSyn_hwidb->firstsw);

        if (swidb == NULL && swidb2 == NULL) {
            return (FALSE);
        } else {
            action = VTP_DELETE;
            retcode = FALSE;
        }
    }

    /* At this point, convert MODIFY to either ADD or DELETE */
    if (action == VTP_MODIFY) {
        /* Convert this to add or delete */
        switch (pVTPMsg->vlan_state) {
            case VTP_VLAN_OPERATIONAL_STATE:
                action = VTP_ADD;
                break;
            case VTP_VLAN_SUSPENDED_STATE:
                action = VTP_DELETE;
                break;
            default:
                return (FALSE);
        }
    }

    /*
     * Allocate a paktype,  cast the datagram start to VLAN_PARAMS
     */
    if ( !(pak = getbuffer(sizeof(VLAN_PARAMS))) ) {
	return (FALSE);
    }

    pak->datagramsize = sizeof(VLAN_PARAMS);

    /* Copy VTP Parms to paktype */
    memcpy(pak->datagramstart, pVTPMsg, sizeof(VLAN_PARAMS));

    pVTPMsg = (VLAN_PARAMS *) pak->datagramstart;
    pVTPMsg->vlan_info_mode = action;

    process_enqueue_pak(Syn_vtpHelperInputQ, pak);
		
    return (retcode);

}/*end vtpipc_vlanparams_cmd*/

/*
*******************************************************************************
*
*   FUNCTION:
*           Syn_vtp_helper_process
*
*   PARAMETERS: NONE
*                 
*
*   RETURN: NONE
*
*   ALGORITHMS:
*           Dequeue buffer, call alc_vtp_lec_create_or_destroy()  
*
*******************************************************************************
*/
forktype Syn_vtp_helper_process(void)
{
    ulong major, minor;
    paktype *pak;
    VLAN_PARAMS *pVTPMsg;

    /* wait for system to initialize */
    process_wait_on_system_init();

    /*
     * Send message to ATM Helper task on NMP that we are ready to receive
     * VTP messages
     */
    leipc_send_earl_req(0, LEIPC_VTP_READY_IND, 0, 0, 0);


    process_watch_queue(Syn_vtpHelperInputQ, ENABLE, RECURRING);
    while (1) {
        process_wait_for_event();
        while (process_get_wakeup(&major, &minor)) {
            switch(major) {
              case QUEUE_EVENT:
                while ( (pak = process_dequeue(Syn_vtpHelperInputQ) ) != NULL) {

                    pVTPMsg = (VLAN_PARAMS *)pak->datagramstart; 

                    if (FALSE == alc_vtp_lec_create_or_destroy (pVTPMsg) ) {
                        buginf("\n\rVTP: LEC Config Failed VLAN: %d", 
				pVTPMsg->vlan_num);
                    }
                    datagram_done(pak);
                }
                break;
              default:
                buginf("\n\rSyn_vtp_helper_process: unexpected event!");
                break;
            }
        }
    }
}

/*
*******************************************************************************
*
*   FUNCTION:	Syn_vtp_helper_init
*
*   PARAMETERS: NONE
*
*   RETURN: 	NONE
*
*   ALGORITHMS:
*           Create Input Q; start VTP Helper process
*
*******************************************************************************
*/

void Syn_vtp_helper_init(void)
{
    Syn_vtpHelperInputQ = create_watched_queue("C5K ATM VTP Helper Input", 0,0);
    Syn_vtp_helper_pid = process_create(Syn_vtp_helper_process, 
                                "Cat5K VTP HELPER", NORMAL_STACK, PRIO_NORMAL);
    Syn_vtpHelperInputQ_enabled = TRUE;
}


/*
*******************************************************************************
*
*   FUNCTION:	alc_vtp_lec_create_or_destroy
*
*   PARAMETERS: VLAN_PARAMS * pVTPMsg
*
*   RETURN:
*               FALSE on failure
*               TRUE  on success
*
*   ALGORITHMS:
*           if ( create LEC) 
*               Find a sub-interface number not in use; call function to 
*               create LEC.
*           else
*               delete sub-interface, thereby removing LEC.
*
*******************************************************************************
*/
boolean
alc_vtp_lec_create_or_destroy (VLAN_PARAMS * pVTPMsg) {

    idbtype* swidb = NULL;
    idbtype* swidb2 = NULL;
    int subifnum, last;

    last  = 0;

    /* Find out if any LEC with this name already exists */
    swidb = reg_invoke_find_lec_by_name_on_interface(pVTPMsg->name,
                                  pVTPMsg->name_len, atmSyn_hwidb->firstsw);

    /* Find out if any LEC with this vlanid already exists */
    swidb2 = reg_invoke_find_lec_by_vlanid_on_interface(pVTPMsg->vlan_num,
                                  atmSyn_hwidb->firstsw);

    if (pVTPMsg->vlan_info_mode == VTP_DELETE) {

        if (swidb) {
            alc_lec_remove(swidb);
        }
        if (swidb2 && (swidb != swidb2)) {
            alc_lec_remove(swidb2);
        }

    } else { /* can only be VTP_ADD */

        if ( swidb && (swidb == swidb2)) {
            /* LEC already exists */
            return TRUE;
        }
        
        /* 
         * Delete any LECs that conflict with current VTP request, since
         * VTP over-rides user config
         */
        if (swidb) {
            alc_lec_remove(swidb);
        }
        if (swidb2) {
            alc_lec_remove(swidb2);
        }

        /* First see if a swidb contains a LES matching the ELAN name */
        if ( (swidb2 = reg_invoke_lsv_find_by_name_on_interface(pVTPMsg->name,
                       pVTPMsg->name_len, atmSyn_hwidb)) != NULL) {
            if (!reg_invoke_lane_configure_client(swidb2, TRUE,
                                                  LANE_TYPE_802_3,
                                                  pVTPMsg->name,
                                                  pVTPMsg->name_len,
                                                  pVTPMsg->vlan_num,
                                                  0, 0)) {
                    alc_lec_remove(swidb);
                    return FALSE;
            }
            return TRUE;
        }

        /* 
         * Next see if any existing swidb has no LANE components
         * configured. If so, then use it.
         */
        FOR_ALL_SWIDBS_ON_HW(atmSyn_hwidb, swidb) {
            if ((!lec_enabled(swidb) && !swidb->lane_info->lsv_info) ||
                swidb->subif_state == SUBIF_STATE_DELETED) {
                if (swidb->subif_state == SUBIF_STATE_DELETED) {
                    shutdown_subif(swidb, SUBIF_STATE_UP);
                }
                /* No LANE components configured, so config an LEC */
                if (!reg_invoke_lane_configure_client(swidb, TRUE,
                                                      LANE_TYPE_802_3,
                                                      pVTPMsg->name,
                                                      pVTPMsg->name_len,
                                                      pVTPMsg->vlan_num,
						      0, 0)) {
                        alc_lec_remove(swidb);
                        return FALSE;
                }
                return TRUE;
            }
        }

        /* 
         * Cannot use any existing sub-interface, so create a new one.
         * Fist need to find a sub-interface number not currently in use.
         */
        subifnum = 1; /* first non-major interface number */
        if (idb_can_create_more()) {
            FOR_ALL_SWIDBS_ON_HW(atmSyn_hwidb, swidb) {
                if (swidb->sub_number < subifnum) {/* should only happen for 
                                                  firstsw */
                    continue;
                }
                if (swidb->sub_number == subifnum) { /* a match try next one */
                    last = swidb->sub_number;
                    subifnum++;
                    continue;
                }
                break; /* aha!  A hole.  Use this number */
            }
            if (!swidb) { /* match was not found */
                subifnum = last + 1;
            }
        
            swidb = idb_create_subif(atmSyn_hwidb->firstsw, subifnum); 

            if (!swidb)  { /* could not create swidb */
               return FALSE;
            }
        } else {
            return FALSE;
        }
 
        /* configure the sub-interface */
        if (!reg_invoke_lane_configure_client(swidb, TRUE,
                                              LANE_TYPE_802_3,
                                              pVTPMsg->name,
                                              pVTPMsg->name_len,
                                              pVTPMsg->vlan_num,
					      0, 0)) {
                alc_lec_remove(swidb);
                return FALSE;
        }
    } 

    return TRUE;
}
int
alc_is_pvc_on_swidb(idbtype* swidb)
{
    hwidbtype *hwidb;
    vcd_t    vcnum;
    vc_info_t *vc;
    atm_db_t *atm;

    if (swidb) {
	hwidb = swidb->hwptr;
    } else {
	printf("\nerror: source idb not set");
	return FALSE;
    }
    atm = hwidb->atm_db;
    for (vcnum = 1; vcnum < atm->config.max_intf_vcs; vcnum++) {
      if ((vc = atm_getvc_all(atm, vcnum))) {
	if(IS_ATMVC_SVC(vc) || (vc->flags & VC_FLAG_TEMP))
	  continue;
	if (vc->swidb != swidb)	/* Only do this SWIDB */
	  continue;
	else
	  return TRUE;
      }
    }
    return FALSE;
}

/*
 * Remove the LEC. Also if the swidb is unused, remove the swidb itself
 */
void
alc_lec_remove (idbtype* swidb)
{
    /* If LES/BUS or LECS configured, then remove the LEC */
    if ( (alc_is_pvc_on_swidb(swidb)) || 
         ((swidb->lane_info != NULL) &&
         ( (swidb->lane_info->lsv_info) ||
           (swidb->lane_info->lecs_info))) ) {
        lec_delete_client(swidb, LANE_CALLER_SNMP);
        return;
    }
 
    /* Otherwise just delete the swidb */
    delete_interface(swidb);
    return;
}
 






