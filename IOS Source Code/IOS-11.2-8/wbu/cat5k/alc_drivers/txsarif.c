/* $Id: txsarif.c,v 1.1.4.1 1996/05/09 14:55:10 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/txsarif.c,v $
 *------------------------------------------------------------------
 * txsarif.c -- TX SAR command posting
 *
 * December 1994, Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: txsarif.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:10  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:27  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:46  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:37  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:06  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.9  1995/10/04  01:30:48  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.8  1995/08/30  19:14:12  sakumar
 * (1) Support for new PMC PMD.
 * (2) vc_per_vp command.
 *
 * Revision 1.7  1995/05/03  04:21:42  rlowe
 * Replace "acp_memcpy" with "memcpy", which is available in
 * services/libc/libc.a and libc_ios.a.
 *
 * Revision 1.6  1995/05/02  01:07:00  rlowe
 * Re-structure code to handling polling (boot case) as well as interrupt
 * mode (kernel case).  Isolate IOS-isms (such as edisms) a bit more.
 * Provide polling mode APIs.
 *
 * Revision 1.5  1995/04/12  22:04:11  sakumar
 * Changed VCD to VPC and
 *         vcd to vpc.
 *
 * Revision 1.4  1995/03/18  01:45:14  sakumar
 * Merged atmdriver with IOS environment
 *
 * Revision 1.3  1995/02/06  19:02:45  rahuja
 * Changed OK and ERR to ACP_OK, ACP_ERR
 *
 * Revision 1.2  1994/12/16  00:29:15  rahuja
 * Integrated and tested atm driver on acp
 *
 * Revision 1.1  1994/12/09  22:09:38  rahuja
 * Integrated with txsar
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * txsarif.c -- TX SAR command posting 
 *
 *------------------------------------------------------------------
 *
 *  CONTENTS:
 *      Interface routines to post commands to the tx sar.
 *
 *  OWNER:
 *
 *      Ratinder P. S. Ahuja
 *
*/
 
/*
**
**  INCLUDES
**
*/
#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif
 
#include    "basic_types.h"
#include    "queue_if.h"
#include    "buffer_if.h"
#include    "sar_if.h"
#include	"atmdrv_if.h"
#include    "shram.h"
#include    "spram.h"
#include    "atmdvr_gdata.h"


/*
** 	Prototypes
*/

tpSARCMD txsar_fillcmd(UINT32  cmd);
void txsar_postcmd(tpSARCMD pCmd);


 
/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_fillcmd
*
*   PARAMETERS:
*           UINT32  cmd
*
*   RETURN:
*           tpSARCMD	pCmd
*
*   ALGORITHMS
*
*******************************************************************************
*/
tpSARCMD
txsar_fillcmd(UINT32  cmd)
{
tpSARCMD    pCmdBuf = NULLP;
                                        /* q descriptors in ACP space   */
tpQDESC     pCmdBufPool = acp_Txspram_info.txspram_pAcpCmdPool;


    /*
    **  Get command buffer
    */
    if( (pCmdBuf = (tpSARCMD)bufmgr_getbuf(pCmdBufPool)) == NULLP ) {
 
        printf("\n\rtxsar_fillcmd: bufmgr_getbuf failed from pool %x", pCmdBufPool);
        return NULLP;
    }
 
    /*
    **  The command buffer returned is in the TX spram address space,
    **  convert to acp space
    */
    pCmdBuf = (tpSARCMD) MAP_SP_TX_ACP(pCmdBuf);
 
    /*
    **  Build command
    */
    pCmdBuf->cmd_wakeup = 0;
    pCmdBuf->cmd_status = 0;
    pCmdBuf->cmd_code = 0xff & (cmd >> 8);

	return pCmdBuf;

}/* end txsar_fillcmd */


/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_postcmd
*
*   PARAMETERS:
*           tpSARCMD pCmd
*
*   RETURN:
*           NONE
*
*   ALGORITHMS
*
*******************************************************************************
*/
void
txsar_postcmd(tpSARCMD pCmd)
{
tpQDESC     pAcpTxCmdQ = acp_Txspram_info.txspram_pAcpCmdq;

    /*
    **  Enq command on command q.  Convert q element to sar's address space
    **  Interrupt txsar
    */
    qmgr_enq( pAcpTxCmdQ, (tpQELEM)(MAP_SP_ACP_TX(pCmd)) );
    atmdrv_interrupt_txsar();


    /* Give up the processor */
 
    ADVR_DismissOnTxSar ( (void *) atmdrv_BLOCK, pCmd);
 
/*
* Note: The SARs DO NOT change the wakeup byte. After the SAR puts the command
*       buffer on the Response queue, it interrupts the ACP. The interrupt
*       handler on the ACP dequeues the response, and sets the wakup byte to 1.
*       Then when the ACP scheduler runs the TEST function, it finds the wakeup
*       byte set to 1 and so the scheduler wakes up this process
*/
 
	return;
 
}/*end txsar_postcmd */




/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_post_nopcmd
*
*   PARAMETERS:
*           NONE
*
*   RETURN:
*           tpSARCMD
*           NULLP
*
*   ALGORITHMS
*
*******************************************************************************
*/
tpSARCMD
txsar_post_nopcmd()
{
tpSARCMD	pCmdBuf = NULLP;


	if( (pCmdBuf = txsar_fillcmd(SAR_NOP_C)) == NULLP ) {
		return NULLP;
	}
    pCmdBuf->cmd_length = sizeof(tSARCMD);
 
    /*
    **  Post command with txsar
    */
	txsar_postcmd(pCmdBuf);

    return pCmdBuf;
 
}/*end txsar_post_nopcmd */



/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_post_vpccmd
*
*   PARAMETERS:
*           
*
*   RETURN:
*           tpSARCMD
*           NULLP
*
*   ALGORITHMS
*
*******************************************************************************
*/
tpSARCMD
txsar_post_vpccmd(UINT8  refnum, UINT32  cmd,  tVPC  vpcval, UINT16  vcstate)
{
tpSARCMD    pCmdBuf;
tpVPCPARM   pVpcp;
 
 
	if( (pCmdBuf = txsar_fillcmd(cmd)) == NULLP ) {
		return NULLP;
	}
 
    pCmdBuf->cmd_wakeup = 0;
    pCmdBuf->cmd_length = mCMDOFFSET + sizeof(tVPCPARM);

    pVpcp = (tpVPCPARM)pCmdBuf->cmd_parmv;
    pVpcp->vpcp_vpc = vpcval;
    pVpcp->vpcp_vcstate = vcstate;
 
    /*
    **  Post command with txsar
    */
	txsar_postcmd(pCmdBuf);

    return pCmdBuf;
 
}/* end txsar_post_vpcsetcmd */




 
/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_post_sendcmd
*
*   PARAMETERS:
*           
*
*   RETURN:
*           tpSARCMD
*           NULLP
*
*   ALGORITHMS
*
*******************************************************************************
*/
tpSARCMD
txsar_post_sendcmd(UINT8  refnum, tVPC  vpc, UINT32 len, tpBUFFER pBuf)
{
tpSARCMD    pCmdBuf;
tpTXSEND_REQ    pSreq;

 
	if( (pCmdBuf = txsar_fillcmd(TXSAR_SEND_REQUEST_C)) == NULLP ) {
		return NULLP;
	}
    pCmdBuf->cmd_wakeup = 0;
    pCmdBuf->cmd_length = mCMDOFFSET + sizeof(tTXSEND_REQ);

    pSreq = (tpTXSEND_REQ)pCmdBuf->cmd_parmv;
    pSreq->txsend_len = len;
    pSreq->txsend_vpc = vpc;
    pSreq->txsend_pbuf = (tpBUFFER)(MAP_SH_ACP_SAR(pBuf));  /* map to sar adr*/
 
    /*
    **  Post command with txsar
    */
	txsar_postcmd(pCmdBuf);

    return pCmdBuf;
 
}/*end  txsar_post_sendcmd */



/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_post_caminit
*
*   PARAMETERS:
*			NONE
*
*   RETURN:
*           tpSARCMD
*           NULLP
*
*   ALGORITHMS
*
*******************************************************************************
*/

tpSARCMD
txsar_post_caminit()
{
tpSARCMD    pCmdBuf = NULLP;
 
 
    if( (pCmdBuf = txsar_fillcmd(TXSAR_INIT_CAM_C)) == NULLP ) {
        return NULLP;
    }
    pCmdBuf->cmd_wakeup = 0;
    pCmdBuf->cmd_length = sizeof(tSARCMD);
 
    /*
    **  Post command with txsar.  txsar_postcmd blocks till the cmd is
	**	acted upon by the sar.
    */
    txsar_postcmd(pCmdBuf);
 
    return pCmdBuf;
 
}/*end txsar_post_caminit*/


/*
*******************************************************************************
*
*   FUNCTION:
*           txsar_post_camcmd
*
*   PARAMETERS:
*			UINT32		cmd
*           tpCAMPARM	pCamReq
*
*   RETURN:
*           tpSARCMD
*           NULLP
*
*   ALGORITHMS
*
*		Get a command bufffer from the command pool, fill command and
*	post to tx sar
*
*******************************************************************************
*/

tpSARCMD
txsar_post_camcmd(UINT32 cmd, tpCAMPARM pCamReq)
{
tpSARCMD    pCmdBuf;
tpCAMPARM	pCamp; 
 
	/*
	**	txsar_fillcmd allocates a command buffer and fill in the cmd 
	**	header fields
	*/

    if( (pCmdBuf = txsar_fillcmd(cmd)) == NULLP ) {
        return NULLP;
    }
    pCmdBuf->cmd_wakeup = 0;
    pCmdBuf->cmd_length = mCMDOFFSET + sizeof(tCAMPARM);
 
	/*
	**	Fill in cam command parameters
	*/

    pCamp = (tpCAMPARM)pCmdBuf->cmd_parmv;
	memcpy(pCamp, pCamReq, sizeof(tCAMPARM));
 
    /*
	**
    **  Post command with txsar.  This is a blocking operation.  On return
	**	the pCmdBuf has the completion code and results.
	**
    */

    txsar_postcmd(pCmdBuf);
 
    return pCmdBuf;

}/*end txsar_post_camcmd*/
