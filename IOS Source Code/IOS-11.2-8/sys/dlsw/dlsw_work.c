/* $Id: dlsw_work.c,v 3.4.52.2 1996/05/17 10:46:39 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dlsw/dlsw_work.c,v $
 *------------------------------------------------------------------
 * dlsw_work.c
 *
 * May 1995, Richard Livingston
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dlsw_work.c,v $
 * Revision 3.4.52.2  1996/05/17  10:46:39  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.40.8  1996/05/02  22:47:20  kchiu
 * CSCdi56517:  Image build error for c1005 images in IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.40.7  1996/05/02  15:51:37  sbales
 * CSCdi51340:  Memory Leak DLSw on activation of PU2.1
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.40.6  1996/04/26  15:17:19  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.40.5  1996/04/26  07:34:54  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.40.4  1996/04/15  00:08:48  kchiu
 * Branch: IbuMod_Calif_branch
 * Add dlsw+/fddi support
 *
 * Revision 3.4.40.3  1996/04/11  14:37:19  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.40.2  1996/04/03  14:02:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.40.1  1996/03/28  08:20:05  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.4.52.1  1996/03/18  19:32:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.2  1996/03/07  08:45:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  13:46:53  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/11/22  06:53:39  kmoberg
 * CSCdi42704:  DLSw : Routers crash in CLS for TCP/IP over serial.
 *
 * CSCdi42704:  DLSw : Routers crash in CLS for TCP/IP over serial.
 * Let's try to get this in one more time...
 *
 * Revision 3.3  1995/11/17  09:04:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:21  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:25:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:56:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/26  03:44:28  kmoberg
 * CSCdi42704:  DLSw : Routers crash in CLS for TCP/IP over serial.
 * Clean up pak_grow_realign() and dlsw_work().
 *
 * Revision 2.3  1995/10/13  22:25:56  rnaderi
 * CSCdi41309:  router crashes when dlsw configured over frame-relay
 * CLSI message header was too big. Have to shift the message if necessary.
 *
 * Revision 2.2  1995/09/15  02:45:14  richl
 * CSCdi39887:  dlsw_work.c out of date
 *
 * Revision 2.1  1995/08/07  05:10:56  richl
 * CSCdi38315:  makefile rework
 *              add new files for libraries
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "ieee.h"
#include "packet.h"
#include "../cls/cls_private.h"
#include "../cls/dlc_registry.h"
#include "../if/tring.h"
#include "fastswitch_registry.h"
#include "../llc2/llc2.h"
#include "../llc2/lltimers.h"

/*
 * This file does a lot of the CLS work that falls below the CLS/DLC
 * demarcation line.  This is why it's OK for it to include dlc.h
 */


/*
 * Original written by Ramin Naderi.  Moved here from srb.c by 
 * Richard Livingston.  Now linked into the DLSw subsystem. Header files
 * need to be pared down.  I just brought over what is in srb.c for immediate
 * sizing of image.
 */

/* 
 * dlsw_work
 * 
 * This routine will take care of all of the necessary work that needs to
 * be done for DLSW. 
 * 
 * It will send appropriate indications to the DLSW code to get things 
 * started in the case of context-less cases. In addition, it will go 
 * to the DLU through the llc code which in trun will generate the
 * appropriate indications or confirmations to the user.
 */

void dlsw_work (paktype *newpak, hwidbtype *idb, int riflen)
{
    llctype             *llc;
    sap_hdr             *llc1;
    trrif_t *trh;
    snap_hdr *snap;
    ClsUSapIdT		uSapId;
    boolean             isResponse;
    
    ClsDlcParamBlockT   clsBlock;
    DlcIdT              *dlcId;
    
    DlcTestT            *dlcTest;
    DlcConnectStnT      *dlcConnectStn;
    LlcConnectStnT      *llcConnectStn;
    
    DlcUdataStnT        *dlcUdataStn;
    LlcUdataStnT        *llcUdataStn;
    paktype             *repak;
    
    /* 
     * compute the correct sap to send message to
     *
     * This is different for VDLC than for LLCs which are more closely
     * modelled on the Token Ring
     */

    uSapId = reg_invoke_dlsw_get_sap(get_cls_type(idb), idb);
    
    if (uSapId == NULL) {
	if (srb_debug || srbevent_debug)
	    buginf("\nDLSw: failure - sap entry is not valid");
	datagram_done(newpak);
	return;
    }

    llc1      = (sap_hdr *)   newpak->info_start;
    isResponse = (llc1->ssap & SAP_RESPONSE);
    trh = (trrif_t *) newpak->datagramstart;
    snap = (snap_hdr *)((uchar *)&trh->rc_blth + riflen);
    
    newpak->enc_flags |= ENC_DLSW;
    llc = (llctype *) LLcByPak(idb->firstsw, newpak);
    if (llc)
        llc->enc_flags = newpak->enc_flags;
    
    if ( ((snap->control & ~LLC1_P) == LLC1_XID) ) {
	if (llc) {
	    if (llc->llc_xid_pak) {
		datagram_done(llc->llc_xid_pak);
		llc->llc_xid_pak = NULL;
                llc2_stoptimer(llc, LLC1_TIMER_M);
	    }

	    if (llc->u_cep_id == (void*) CLS_NO_CEP) {
		datagram_done(newpak);
		return;
	    }

	    dlcId = &clsBlock.fParameters.fDlcId;
	    clsBlock.fResultError    = CLS_OK;
	    clsBlock.fPakPtr         = newpak;
	    dlcId->fXidT             = isResponse ? XID_RESPONSE: XID_COMMAND;
	    dlcId->fDlcPCepId     = (DlcPCepIdT)llc;
	    dlcId->fXIDLength = newpak->datagramsize -
		(newpak->info_start + SAP_HDRBYTES - newpak->datagramstart);

	    /*
	     * CLS will append a clsi message header onto newpak->network_start.
	     * The start of the clsi message must be word aligned otherwise
	     * queueing functions in buffers.c will spew out alignment
	     * warnings. The long term solution is to use the fReserveLength
	     * field in the clsi message to put "dead space" between the clsi
	     * message and the frame data so the clsi message header can be word 
	     * aligned. At this point, CLS and DLSw do not honor the fReserveLength
	     * field and it is too risky to make a big change now (12-2-94).
	     * The use of the fReserveLength field should be incorporated in
	     * the first maintenance release.
	     *
	     * For now, shift the XID frame so newpak->network_start is word
	     * aligned. This will guarantee that when the clsi message is
	     * appended, it will also be word aligned.
	     *
	     * Set network_start to point at start of xid data.
	     */
	    bcopy(newpak->info_start, newpak->datagramstart - SAP_HDRBYTES, 
		  newpak->datagramsize - (newpak->info_start - newpak->datagramstart));
	    newpak->network_start = newpak->datagramstart;
	    newpak->datagramstart -= SAP_HDRBYTES;
	    newpak->info_start = newpak->datagramstart;
	    dlcId->fXIDPtr = newpak->network_start;
	    
	    reg_invoke_cls_rput(llc->u_cep_id, CLS_ID_IND, &clsBlock);
	}
	else {
	    /*
	     * ID Station Ind
	     *
	     * Code to for CLS block and send it to CLS - moved to dlc.c
	     * PHIL
	     */
	    CLSASSERT_FATAL(idb != NULL);
	    newpak->riflen = riflen;
            repak = pak_grow_realign(newpak, newpak->datagramsize,
                               (sizeof(IdStnInd_t) + sizeof(long)));
            if (repak == NULL) {
		 datagram_done(newpak);
		 return;
	    }

            repak->enc_flags = newpak->enc_flags;
	    DlcSendUControlStnInd(&clsBlock, repak, LLC1_XID, isResponse,
			      uSapId, (DlcPCepIdT)llc);
	}

	return;
    }
    else if ( ((snap->control & ~LLC1_P) == LLC1_TEST) ) {
	if(llc)	{
	    if (llc->u_cep_id == (void*) CLS_NO_CEP) {
		datagram_done(newpak);
		return;
	    }
	    dlcTest = &clsBlock.fParameters.fDlcTest;
	    clsBlock.fResultError    = CLS_OK;
	    clsBlock.fPakPtr         = newpak;
	    dlcTest->fTestT       = isResponse ? TEST_RESPONSE: TEST_COMMAND;
	    dlcTest->fDlcPCepId   = (DlcPCepIdT)llc;
	    
	    /* 
	     * Set network_start to point at start of xid data 
	     */
	    newpak->network_start = newpak->info_start + SAP_HDRBYTES;
	    dlcTest->fTESTLength = newpak->datagramsize - (newpak->network_start - newpak->datagramstart);
	    dlcTest->fTESTPtr = newpak->network_start;
	    reg_invoke_cls_rput(llc->u_cep_id, CLS_TEST_IND, &clsBlock);
	}
	else {
	    /*
	     * TEST STATION IND
	     *
	     * Code to form clsBlock and send it to CLS - moved to dlc.c
	     * PHIL
	     */
	    CLSASSERT_FATAL(idb != NULL);
	    newpak->riflen = riflen;
            repak = pak_grow_realign(newpak, newpak->datagramsize,
                               (sizeof(TestStnInd_t) + sizeof(long)));
            if (repak == NULL) {
		 datagram_done(newpak);
		 return;
	    }

            repak->enc_flags = newpak->enc_flags;
	    DlcSendUControlStnInd(&clsBlock, repak, LLC1_TEST, isResponse,
			      uSapId, (DlcPCepIdT)llc);
	}
	return;
    }
    else if ( ((snap->control & ~LLC1_P) == SABME) && !llc) {
	hwaddrtype macAddr;

        repak = pak_grow_realign(newpak, newpak->datagramsize,
                            (sizeof(ConnectStnInd_t) + sizeof(long)));
	if (repak == NULL) {
	     datagram_done(newpak);
	     return;
	}

        repak->enc_flags = newpak->enc_flags;
	dlcConnectStn = &clsBlock.fParameters.fDlcConnectStn;
	llcConnectStn = &dlcConnectStn->fDlcConnect.fLlcConnectStn;
	
        (*idb->extract_snpa)(repak, &macAddr, SOURCE);
	
	llcConnectStn->fMacType = STATION_IEEE48;
	llcConnectStn->fLMacLength = IEEEBYTES;
	llcConnectStn->fLMacPtr = repak->addr_start;
	llcConnectStn->fRMacLength = IEEEBYTES;
	llcConnectStn->fRMacPtr = repak->addr_start + IEEEBYTES;
	llcConnectStn->fRoutingLength = riflen;
	llcConnectStn->fRoutingPtr = repak->rif_start;
	
	llc1 = (sap_hdr *) repak->info_start;
	llcConnectStn->fRSap = llc1->ssap & ~SAP_RESPONSE;
	llcConnectStn->fLSap = llc1->dsap;
	
	/*
	 * Set network_start to point at start of xid data 
	 */
	repak->network_start = repak->info_start + SAP_HDRBYTES;
 
	dlcConnectStn->fCONNECTLength = repak->datagramsize -(repak->network_start - repak->datagramstart);
	
	dlcConnectStn->fCONNECTPtr = repak->network_start;
	
	dlcConnectStn->fClsUSapId = uSapId;
 
	clsBlock.fResultError    = CLS_OK;
	clsBlock.fPakPtr         = repak;
	reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, CLS_CONNECT_STN_IND, &clsBlock);
	return;
    }
    else if ( ((snap->control & ~LLC1_P) == LLC1_UI) ) {
	hwaddrtype macAddr;
	if(llc) {
	    if (llc->u_cep_id == (void*) CLS_NO_CEP) {
		datagram_done(newpak);
		return;
	    }

	    clsBlock.fResultError    = CLS_OK;
	    clsBlock.fPakPtr         = newpak;
	    /* 
	     * Set network_start to point at start of udata 
	     */
	    newpak->network_start = newpak->info_start + SAP_HDRBYTES;
	    clsBlock.fLoadLength = newpak->datagramsize -(newpak->network_start - newpak->datagramstart);
	    reg_invoke_cls_rput(llc->u_cep_id, CLS_UDATA_IND, &clsBlock);
	}
	else {
            repak = pak_grow_realign(newpak, newpak->datagramsize,
                              (sizeof(UDataStnInd_t) + sizeof(long)));
	    if (repak == NULL) {
		 datagram_done(newpak);
		 return;
	    }

            repak->enc_flags = newpak->enc_flags;
	    clsBlock.fResultError    = CLS_OK;
	    clsBlock.fPakPtr         = repak;
	    dlcUdataStn = &clsBlock.fParameters.fDlcUdataStn;
	    llcUdataStn = &dlcUdataStn->fDlcUdata.fLlcUdataStn;
	    
	    (*idb->extract_snpa)(repak, &macAddr, SOURCE);
 
	    llcUdataStn->fMacType = STATION_IEEE48;
	    llcUdataStn->fLMacLength = IEEEBYTES;
	    llcUdataStn->fLMacPtr = repak->addr_start;
	    llcUdataStn->fRMacLength = IEEEBYTES;
	    llcUdataStn->fRMacPtr = repak->addr_start + IEEEBYTES;
	    llcUdataStn->fRoutingLength = riflen;
	    llcUdataStn->fRoutingPtr = repak->rif_start;
	    
	    llc1 = (sap_hdr *) repak->info_start;
	    llcUdataStn->fRSap = llc1->ssap & ~SAP_RESPONSE;
	    llcUdataStn->fLSap = llc1->dsap;
	    
	    dlcUdataStn->fClsUSapId   = uSapId; 
	    /* 
	     * Set network_start to point at start of udata 
	     */
	    repak->network_start = repak->info_start + SAP_HDRBYTES;
	    dlcUdataStn->fUDataLength = repak->datagramsize -(repak->network_start - repak->datagramstart);
	    dlcUdataStn->fUDataPtr = repak->network_start;
	    
	    reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, CLS_UDATA_STN_IND, &clsBlock);
	}

	return;
    }
    else {
	reg_invoke_llc2_input(newpak);  /* with UCEP_ID */
	return;
    }
}

