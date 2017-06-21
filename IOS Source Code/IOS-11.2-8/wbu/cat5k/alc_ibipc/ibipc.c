/* $Id: ibipc.c,v 1.1.4.1 1996/05/09 14:55:20 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_ibipc/ibipc.c,v $
 *------------------------------------------------------------------
 * ibipc.c
 *
 * March 1995, jjchen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibipc.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:20  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/02  06:47:27  rlowe
 * Clean up compilation warnings for bootrom-specific code.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:14:48  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:57  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:38  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:48  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:16  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.9.8.1  1996/02/29  07:00:01  rlowe
 * Changes needed for IOS-side sync to Arkansas V111_0_16
 * (Synalc3_Cal_V111_0_16_branch).
 *
 * Revision 1.9  1995/10/04  01:07:42  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.8  1995/09/19  21:53:17  vinod
 * Added call to checktimeout() before untimeout() per umesh
 *
 * Revision 1.7  1995/09/15  23:49:10  vinod
 * Fixed IPC for multiple card support, moved some debug prints to
 * trace prints.
 *
 * Revision 1.6  1995/08/31  12:02:39  rlowe
 * Changes to work with IOS 11.0 and higher.
 *
 * Revision 1.5  1995/08/30  18:00:00  vinod
 * First phase of Multiple cards support for IPC, mainly from the
 * NMPs perspective, more changes soon. Rx upcalls (on the NMP side)
 * pass the ModuleNumber.
 *
 * Revision 1.4  1995/08/01  03:01:09  rlowe
 * Remove dependency on IOS header file "sys_registry.h" and its
 * sub-dependencies.
 *
 * Revision 1.3  1995/06/13  04:55:20  rlowe
 * Changes to get BIGA driver running in the Boots for download use.
 *
 * Revision 1.2  1995/05/22  19:26:40  jjchen
 * Set to use vlan 0.
 *
 * Revision 1.1  1995/05/01  23:50:25  jjchen
 * Moved from syn_core/nmp/nmpflash/ibipc.
 *
 * Revision 1.1  1995/03/29  05:44:00  jjchen
 * Created.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

#if ((! defined IOS_BOOT) || (defined MAKE_DEPEND))
#include "../../../sys/h/master.h"
#include "../../../sys/h/subsys.h"
#include "../../../sys/h/interface_private.h"
#include "../../../sys/h/ieee.h"
#include "../../../sys/h/packet.h"
#include "../../../sys/ui/debug.h"
#include "../../../sys/if/network.h"
#include "../../../sys/h/config.h"
#include "../../../sys/os/old_timer_callbacks.h"
#include "../../../sys/dev/monitor1.h"
#endif

#include "utypes.h"

#if ((defined IOS_BOOT) || (defined MAKE_DEPEND))
#include "ipcboot.h"
#endif

#if ((! defined NO_VLAN) || (defined MAKE_DEPEND))
#include "synpacket.h"
#endif

#include "ibframe.h"
#include "ibipc.h"
#include "ib_externs.h"


#ifdef IPC_OVER_OB /* NMP */
#include "process.h"
#include "scp_defs.h"
#include "scp_msgs.h"
#include "scpsapif.h"

extern QHEADER if_scp_tx_hdr;
IBIPC_TXD_T *
ipc_ob_send(IBIPC_Table_T *, uInt16, uChar, uChar, uInt32, uChar   *);

#endif

/* external */

extern uChar * get_slot_addr(uChar);
extern uChar my_slot;
#ifdef CATALYST_OS
extern struct scdata sc_data[];
extern uInt32 inband_port_index;
#endif

/* local */

int check_ipc_hdr(uChar *, IBIPC_Table_T *);
static int ipc_txd_queue(IBIPC_Table_T *, IBIPC_TXD_T *);
static void Ipc_addq(IBIPC_Table_T *);
static IBIPC_Table_T * get_ibipc_entry(uInt16, uInt16, uInt16, uInt16, int *);
static IBIPC_Table_T * get_ibipc_entry4handle(uInt32, int *);
static int is_tos_valid(uChar);
static void mac_drvr_send(uChar *, uInt32, uChar);
static uChar * mac_drvr_buf_alloc (uInt32, uChar);
int ipc_tx (uInt32  handle);
#ifndef CATALYST_OS
boolean ck_ipc_tx_block(ulong);
void ipc_tx_to(ulong, ulong, ulong);
#endif
int GetMySlotNumber();

IBIPC_Table_T * ibipc_table_header = 0;

uChar ibipc_llcsnap[8] = {0xaa, 0xaa, 0x03, 0x00, 0x00, 0x0c, 0x38, 0x38};

/*
 * This function is called by the application to register for In-band IPC.
 *
 * 'sid' and 'did' are a unique pair of IDs to indicate a communication session.
 * App_IpcTxAck() and App_IpcTxNak() are used for connectionless-acknowledge mode.
 * App_IpcConfirm() is no use right now.
 * However, App_IpcRxInd() is provided by the application for IPC to call it as
 * receiving an IPC packet.
 *
 */
int Ipc_AppRegLink (uInt16  sid,		/* source ID */
		    uInt16  did,		/* destination ID */
		    uChar   slot,		/* destination card slot */
		    void (* App_IpcTxAck)(),	/* Tx ACK callback */
		    void (* App_IpcTxNak)(),	/* Tx NAK callback */
		    void (* App_IpcRxInd)(),	/* Rx callback */
		    void (* App_IpcConfirm)(), 	/* pass-through callback */
		    uChar   qflag,		/* indicates (fast/slow) MAC to use */
		    uInt32  * phandle,
		    uInt32  * pMTUSize)		/* ptr return MTU size */
{
	IBIPC_Table_T * ibp;

	*phandle = 0;

	/* valid sid, did and slot?
	 */
	if (sid < MIN_IB_IPC_ID || sid > MAX_IB_IPC_ID)
	    return(IBIPC_RC_ILLEGAL_SID);

	/* 'did' and 'slot' are no use for now.
	if (did < MIN_IB_IPC_ID || did > MAX_IB_IPC_ID)
	    return(IBIPC_RC_ILLEGAL_DID);

	if (slot < MIN_SLOT_ID || slot > MAX_SLOT_ID)
	    return(IBIPC_RC_ILLEGAL_SLOT);
	*/

	/* sid being used ?
	 */
	for (ibp = ibipc_table_header; ibp; ibp = ibp->next) {
	     if ((sid == ibp->sid) &&
		 (did == ibp->did) &&
		 (slot == ibp->slot))
		return(IBIPC_RC_REGISTERED);
	}

	/* allocate a new In-band IPC entry and set it up.
	 */
	if ((ibp = (IBIPC_Table_T *)malloc(sizeof(IBIPC_Table_T))) == NULL)
	    return(IBIPC_RC_NOMEMORY);

	ibp->sid = sid;
	ibp->did = did;
	ibp->slot = slot;
	ibp->SourceSlot = GetMySlotNumber();
	ibp->qflag = qflag;
	ibp->App_IpcTxAck = App_IpcTxAck;
	ibp->App_IpcTxNak = App_IpcTxNak;
	ibp->App_IpcRxInd = App_IpcRxInd;
	ibp->App_IpcConfirm = App_IpcConfirm;
	ibp->txd = 0;
	*phandle = (uInt32)ibp;
	ibp->handle = (uInt32)ibp;
#ifdef  IPC_OVER_OB
	if (qflag == IPC_IN_BAND)
	    *pMTUSize = IPC_IB_APP_DATA_SIZE;
	else {
	    *pMTUSize = IPC_OB_APP_DATA_SIZE;
	}
#else   /* assumed in-band only */
	*pMTUSize = IPC_IB_APP_DATA_SIZE;
#endif

	Ipc_addq(ibp);

	return(IBIPC_RC_OK);
}

/*
 *
 */
int Ipc_AppDeRegister (uInt32 handle)		/* IPC registered handle */
{
	IBIPC_Table_T * ibp, * previous_ibp;
	IBIPC_TXD_T   * txd, * next_txd = 0;
	int s;

	/* Find an Ib-band IPC entry with sid and delete it.
	 */
	previous_ibp = (IBIPC_Table_T *)&ibipc_table_header;

	for (ibp = ibipc_table_header; ibp; ibp = ibp->next) {
	     if (handle == ibp->handle) {
		if (previous_ibp == (IBIPC_Table_T *)&ibipc_table_header)
		    ibipc_table_header = ibp->next;
		else
		    previous_ibp->next = ibp->next;

		for(txd = ibp->txd; txd; txd = next_txd) { /* release resources */
		    next_txd = txd->next;
#ifdef CATALYST_OS
		    /*
		     * Cancel timer if it has not already expired
		     */
		    s = spl7();
		    if (checktimeout(ipc_tx, ibp, txd->timer_index) == TRUE) {
		      untimeout(txd->timer_index); 
		    } else {
		      trace(IPC_TRACE, 1, "IPC: Timer Already expired\n");
		    }
		    splx(s);
#else /* Cisco IOS */
		    destroy_timer((timer_t *)txd->timer_index);
#endif
		    if (ibp->App_IpcTxNak)
			ibp->App_IpcTxNak(((IBIPC_PKT_T *)(txd->pkt))->data);
		    else 
			free(txd); /* dummy client */
		}

		free(ibp);

		return(IBIPC_RC_OK);
	     }
	     previous_ibp = ibp;
	}

	return(IBIPC_RC_ILLEGAL_SID);
}

/*
 *
 */
int ipc_tx (uInt32  handle)		/* registered handle */
{
	IBIPC_Table_T * ibp;
	int rc;
#ifdef CATALYST_OS
	IBIPC_PKT_T   * mbp;
#else
	paktype * pak;
#endif
	IBIPC_TXD_T   * next_txd;


	ibp = get_ibipc_entry4handle(handle, &rc);
	if (rc)
	    goto ipc_tx_rtn; /* not register yet */

	if (!ibp->txd)
	    return(IBIPC_RC_NOTXDPENDING);

	ibp->txd->timer_index = -1;

	if (ibp->txd->tos == IPC_TYPE3) {
	if (ibp->txd->retry) { /* try again */
#ifdef CATALYST_OS
	    ibp->txd->timer_index = timeout(ipc_tx, handle, ibp->txd->timer);
#else /* Cisco IOS */
	    ibp->txd->timer_index = (int)create_timer(ipc_tx_to, (void *)handle, NULL, NULL, "IPC tx", TMT_ONCE);
	    set_timer((timer_t  *)ibp->txd->timer_index, ibp->txd->timer);
#endif
	}

	if (!ibp->txd->retry || ibp->txd->timer_index <= 0) { /* give up */
	    next_txd = ibp->txd->next;

	    if (ibp->App_IpcTxNak)
		ibp->App_IpcTxNak(((IBIPC_PKT_T *)(ibp->txd->pkt))->data);
	    else 
		free(ibp->txd); /* dummy client */

	    ibp->txd = next_txd;
#ifdef CATALYST_OS
	    wakeup(handle); /* if someone was blocked earlier. */
#else /* Cisco IOS */
#endif
	    return(IBIPC_RC_NOTXDPENDING);
	}

	ibp->txd->retry--;
	} /* if (ibp->txd->tos == IPC_TYPE3) */

#ifdef CATALYST_OS
	/* allocate MAC driver buffer */
	if (!(mbp = (IBIPC_PKT_T *)mac_drvr_buf_alloc(ibp->txd->pkt_len, ibp->qflag))) {
	    rc = IBIPC_RC_NODRVRBUF;
	    goto ipc_tx_rtn;
	}

	/* copy whole packet to the MAC driver buffer
	 */
#ifdef	IPC_OVER_OB /* NMP */
	if (ibp->qflag != IPC_IN_BAND)
	    bcopy((uChar *) ibp->txd->pkt, ((IF_SCP_BUF_PTR)mbp)->ifscpbuf, ibp->txd->pkt_len);
	else
#endif
	bcopy((uChar *) ibp->txd->pkt, mbp, ibp->txd->pkt_len);
	
	/* Have the driver send the packet
	 */
	mac_drvr_send((uChar *)mbp, ibp->txd->pkt_len, ibp->qflag);
#else /* Cisco IOS */
	/* allocate MAC driver buffer */
	if (!(pak = (paktype *)mac_drvr_buf_alloc(ibp->txd->pkt_len, ibp->qflag))) {
	    rc = IBIPC_RC_NODRVRBUF;
	    goto ipc_tx_rtn;
	}

	/* copy whole packet to the MAC driver buffer
	 */
	bcopy((uChar *) ibp->txd->pkt, pak->datagramstart, ibp->txd->pkt_len); /* VLAN ??? */

	/* Have the driver send the packet
	 */
	mac_drvr_send((uChar *)pak, ibp->txd->pkt_len, ibp->qflag);
#endif
	rc = IBIPC_RC_OK;

ipc_tx_rtn:

	/* remove from queue if a datagram packet
	 */
	if (ibp->txd->tos == IPC_DATAGRAM) {
	    ibp->txd = ibp->txd->next;
	}

	return(rc);
}

/*
 *
 */
int Ipc_AppSend (uInt32  handle,		/* registered handle */
		 uInt16  did,			/* destination ID */
		 uChar   slot,			/* destination slot */
		 uChar   tos,			/* type of service */
		 uInt32  mlen,			/* message length after application header */
		 uChar   * mptr)		/* message buffer (ie. application hdr) */
{
	IBIPC_Table_T * ibp;
	int rc;
	IBIPC_HDR_T     * ipc_hdr;
	struct a8023hdr * a8023_hdr;
	IBIPC_PKT_T     * mbp;
	IBIPC_TXD_T	* txd;
#ifndef NO_VLAN
	PSYNERGY_FRAME sp;
#endif

	ibp = get_ibipc_entry4handle(handle, &rc);
	if (rc)
	    return(rc); /* not register yet */

	if (!is_tos_valid(tos & ~IPC_NON_BLOCK_MASK))
	    return(IBIPC_RC_TOS_ERROR);

	if (ibp->txd) {
	   if (tos & IPC_NON_BLOCK_MASK) 
		return(IBIPC_RC_TOOMANYTX);
	   else
#ifdef CATALYST_OS
		sleep(handle, 27); /* blocking - one outstanding TX cmd at a time */
#else /* Cisco IOS */
#ifndef IOS_BOOT
		edisms((blockproc *)ck_ipc_tx_block, handle);
#else
		return(IBIPC_RC_TOOMANYTX);
#endif
#endif
	}

#ifdef	IPC_OVER_OB /* NMP */
	if (ibp->qflag != IPC_IN_BAND) {
	    txd = ipc_ob_send(ibp, did, slot, tos, mlen, mptr);
	    goto Ipc_AppSend_tx;
	}
#endif

	mbp = (IBIPC_PKT_T *)(mptr - sizeof(IBIPC_PKT_HDR_T));
	txd = (IBIPC_TXD_T *)(mptr - sizeof(IBIPC_PKT_HDR_T) - sizeof(IBIPC_TXD_T));

	ipc_hdr = &mbp->ipc_hdr;
	a8023_hdr = (struct a8023hdr *)&mbp->a8023;

	/* IPC header.
	 */
	ipc_hdr->len = mlen;
	ipc_hdr->did = did;
	ipc_hdr->sid = ibp->sid;

	ipc_hdr->DestSlot = ibp->slot;
	ipc_hdr->SrcSlot = ibp->SourceSlot; /*?????????????????????*/
	if (tos == IPC_DATAGRAM)
	    ipc_hdr->ctl = IPC_CTL_DATAGRAM;
	else
	    ipc_hdr->ctl = IPC_CTL_TYPE3;

#ifdef OLD
	ipc_hdr->rsv0 = 0;
	ipc_hdr->rsv1 = 0;
	ipc_hdr->rsv2 = 0;	
#endif
	ipc_hdr->Revision = 0;

	/* LLC/SNAP headers
	 */
	bcopy(ibipc_llcsnap, (uChar *)&a8023_hdr->llchdr, sizeof(LLCSNAP));

	/* Destination and source MAC addresses and length field
	 */
	bcopy((uChar *) (get_slot_addr(slot)), (uChar *)&a8023_hdr->da, sizeof(MAC_ADDRESS));
	bcopy((uChar *) (get_my_addr()), (uChar *)&a8023_hdr->sa, sizeof(MAC_ADDRESS));
	a8023_hdr->len = mlen + IB_IPC_APP_HDR_SIZE + sizeof(LLCSNAP);

#ifndef NO_VLAN
	/* set up VLAN header
	 */

	sp = (PSYNERGY_FRAME)mbp->vlan;

	memset((uChar *)sp, 0, SYNHDR_SIZE);

	sp->SynHdrUnion.SageTxHeader.TypeUser.Type = 0;
	sp->SynHdrUnion.SageTxHeader.TypeUser.UserDefined = 0;

	sp->SynHdrUnion.SageTxHeader.PacketControl.GenerateCRC = 1;
	sp->SynHdrUnion.SageTxHeader.PacketControl.NativeAddress = 1;
	sp->SynHdrUnion.SageTxHeader.VLanIndex.Index = inband_port_index;
	sp->SynHdrUnion.SageTxHeader.VLanIndex.VLan = 0;
#endif

	/*
	 */
	txd->next = 0;
	txd->pkt  = (uChar *)mbp;
#ifndef NO_VLAN
	txd->pkt_len  = a8023_hdr->len + sizeof(struct etherhdr) + SYNHDR_SIZE;
#else
	txd->pkt_len  = a8023_hdr->len + sizeof(struct etherhdr);
#endif

	txd->timer_index = -1;
	txd->timer = IPC_ACK_TIMER;
	txd->retry = IPC_TX_RETRY;
	txd->tos = tos;
	txd->did = did;
	txd->slot = slot;

	/* Have the driver send the packet
	 */
Ipc_AppSend_tx:

	if (ipc_txd_queue(ibp, txd))
	    rc = ipc_tx(handle);

	return(rc);
	
}

/*
 * 
 */
int Ipc_AppRxDone (uInt32 handle,		/* registered handle */
		   uChar   * mptr)		/* ptr message buffer (ie. application hdr) */
{
	IBIPC_Table_T * ibp;
	int rc;

	ibp = get_ibipc_entry4handle(handle, &rc);

	/* release the packet buffer */

#ifdef CATALYST_OS
#ifdef IPC_OVER_OB
	if (rc) return(rc); /* panic !!! */
	if (ibp->qflag != IPC_IN_BAND)
	    rel_scpbuf((IF_SCP_BUF_PTR)mptr);
	else
#endif
	m_freem(dtom(mptr));
#else /* Cisco IOS */
	datagram_done((paktype *)mptr); 
#endif

	return(IBIPC_RC_OK);
}

/*
 * Dummy !
 */
int Ipc_AppRequest (uChar * pReq)		/* ptr message buffer (ie. DL header) */
{
	return(IBIPC_RC_DUMMY_FUNCTION);
}

/*
 *
 */
int Ipc_AppGetStats (uInt32 handle,			/* registered handle, 0xffffffff for all */
		     IBIPC_Table_T ** ibpp)		/* ptr stats record array */
{
	IBIPC_Table_T * ibp = 0;
	int rc = IBIPC_RC_OK;

	if (handle == IBIPC_HANDLE_WILD_CARD)
	    ibp = ibipc_table_header;
	else
	    ibp = get_ibipc_entry4handle(handle, &rc);

	*ibpp = ibp;

	return(rc);
}

/*
 * ! It is application's responsibility to release the memory buffer by calling
 *   Ipc_AppMsgFree().
 *
 */
uChar * Ipc_AppMsgMalloc (uInt32 mlen)		/* number of bytes of application data */
{
	uChar * bp;

	bp = (uChar *)malloc(mlen + sizeof(IBIPC_TXD_T) + sizeof(IBIPC_PKT_HDR_T));
	if (bp)
	    return(bp + sizeof(IBIPC_TXD_T) + sizeof(IBIPC_PKT_HDR_T));

	return(0); /* no buffer */
}

/*
 */
int Ipc_AppMsgFree (uChar * mbp)		/* ptr message buffer */
{
	if (mbp)
	    free(mbp - sizeof(IBIPC_TXD_T) - sizeof(IBIPC_PKT_HDR_T));

	return(0);
}


/*
 * The MAC driver will call this routine and pass the packet with the IPC SAP number.
 */
int Ipc_rx (uChar * mptr)		/* ptr to the packet */
{
	IBIPC_Table_T * ibp;
	IBIPC_HDR_T *  ipc_hdr;
	int rc;
	uInt16 sid, did, SourceSlot, DestSlot;

	/* "destination id" in the packet is our "source id" and 
	 * "source id" in the packet is our "destination id".
	 */
#ifdef CATALYST_OS
#ifndef NO_VLAN
	mptr -= SYNHDR_SIZE;
#endif
	ipc_hdr = &((IBIPC_PKT_T *)mptr)->ipc_hdr;
#else /* Cisco IOS */
	ipc_hdr = &((IBIPC_PKT_T *)((paktype *)mptr)->datagramstart)->ipc_hdr; /* VLAN ??? */
#endif

	sid = ipc_hdr->did;
	did = ipc_hdr->sid;
	SourceSlot = ipc_hdr->DestSlot;
	DestSlot = ipc_hdr->SrcSlot;

#ifdef CATALYST_OS
#ifdef DEBUG
	trace(IPC_TRACE, 10, "IPC Rx from Slot %d\n", DestSlot);
#endif
#endif

	/*ibp = get_ibipc_entry(sid, IBIPC_ID_WILD_CARD, &rc);*/
	ibp = get_ibipc_entry(sid, did, SourceSlot, DestSlot, &rc);
	if (rc) {
	    /* drop the packet */
	    /* ??? */
#ifdef CATALYST_OS
	    trace(IPC_TRACE, 1, "no IPC entry found, sid = %x, did = %x\n", sid, did);
	    m_freem(dtom(mptr));
#else /* Cisco IOS */
	    printf("no IPC entry found, sid = %x, did = %x\n", sid, did);
	    datagram_done((paktype *)mptr);
#endif
	    return(rc);
	}

	if (check_ipc_hdr(mptr, ibp)) {
#ifdef CATALYST_OS
	    m_freem(dtom(mptr));
#else /* Cisco IOS */
	    datagram_done((paktype *)mptr);
#endif
	    return(0);
	}

	/* Packet received indication to the application.
	 * Packet ownership will be passed to its application.
	 * Later, the application can call Ipc_AppRxDone() to free up the packet.
	 */
	if (ibp->App_IpcRxInd)
#ifdef CATALYST_OS
	   (* ibp->App_IpcRxInd)((uInt32)ibp->slot, mptr);
	else
	   m_freem(dtom(mptr));
#else /* Cisco IOS */
	   (* ibp->App_IpcRxInd)(mptr);
	else
	   datagram_done((paktype *)mptr);
#endif

	return(0);
}

/*
 * return : 1 - do not give the IPC packet data to the user.
 */
int check_ipc_hdr(mptr, ibp)
uChar *  mptr;
IBIPC_Table_T * ibp;
{
#ifdef CATALYST_OS
	IBIPC_HDR_T	 * ipc_hdr = &((IBIPC_PKT_T *)mptr)->ipc_hdr;
	struct a8023hdr	 * a8023_hdr = (struct a8023hdr *)&(((IBIPC_PKT_T *)mptr)->a8023);
	int s;
#else /* Cisco IOS */
	IBIPC_HDR_T	 * ipc_hdr = &((IBIPC_PKT_T *)((paktype *)mptr)->datagramstart)->ipc_hdr; /* VLAN ??? */
	struct a8023hdr	 * a8023_hdr = (struct a8023hdr *)&(((IBIPC_PKT_T *)((paktype *)mptr)->datagramstart)->a8023); /* VLAN ??? */
	paktype	* pak;
#endif

	IBIPC_PKT_HDR_T  * bp;
	IBIPC_TXD_T      * next_txd;
	int rc = 1;
#ifndef NO_VLAN
	PSYNERGY_FRAME sp;
#endif

	switch(ipc_hdr->ctl) {
	    case IPC_CTL_ACK:
		if (ibp->txd) {
		    if (ibp->txd->did == ipc_hdr->sid) {
			/* completed "TX request" and return it to its original */
#ifdef CATALYST_OS
		    /*
		     * Cancel timer if it has not already expired
		     */
		    s = spl7();
		    if (checktimeout(ipc_tx, ibp, ibp->txd->timer_index) == TRUE) {
		      untimeout(ibp->txd->timer_index);
		    } else {
		      trace(IPC_TRACE, 1, "IPC: Timer already expired\n");
		    }
		    splx(s);
#else /* Cisco IOS */
		    	destroy_timer((timer_t *)ibp->txd->timer_index);
#endif
			next_txd = ibp->txd->next;

			if (ibp->App_IpcTxAck)
			    ibp->App_IpcTxAck(((IBIPC_PKT_T *)(ibp->txd->pkt))->data);
			else
			    free(ibp->txd); /* dummy client */

#ifdef CATALYST_OS
		    	wakeup(ibp->handle); /* if someone was blocked earlier. */
#else /* Cisco IOS */
#endif
			ibp->txd = next_txd;
		    }
		}
		    
		break;

	    case IPC_CTL_DATAGRAM:
		rc = 0;
		break;

	    case IPC_CTL_TYPE3:
		/* send acknowledgement
		 */
#ifdef CATALYST_OS
		if (bp=(IBIPC_PKT_HDR_T *)mac_drvr_buf_alloc(sizeof(IBIPC_PKT_HDR_T), ibp->qflag)) {
#else /* Cisco IOS */
		if (pak=(paktype *)mac_drvr_buf_alloc(sizeof(IBIPC_PKT_HDR_T), ibp->qflag)) {
		    bp = (IBIPC_PKT_HDR_T  *)pak->datagramstart; /* VLAN ??? */
#endif

		    bcopy((uChar *)&a8023_hdr->sa, &bp->a8023.da, sizeof(MAC_ADDRESS));
		    bcopy((uChar *)&a8023_hdr->da, &bp->a8023.sa, sizeof(MAC_ADDRESS));
		    bp->a8023.etype = sizeof(struct llcsnap) + sizeof(struct ibipc_hdr);

		    bcopy(ibipc_llcsnap, &bp->llcsnap, sizeof(struct llcsnap));

		    bp->ipc_hdr.len = 0;
		    bp->ipc_hdr.sid = ipc_hdr->did;
		    bp->ipc_hdr.did = ipc_hdr->sid;
		    bp->ipc_hdr.ctl = IPC_CTL_ACK;
#ifdef OLD
		    bp->ipc_hdr.rsv0 = 0;
		    bp->ipc_hdr.rsv1 = 0;
		    bp->ipc_hdr.rsv2 = 0;
#endif
		    bp->ipc_hdr.Revision = 0;

#ifndef NO_VLAN
		    /* set up VLAN header
	 	    */

		    sp = (PSYNERGY_FRAME)bp->vlan;

		    memset((uChar *)sp, 0, SYNHDR_SIZE);

		    sp->SynHdrUnion.SageTxHeader.TypeUser.Type = 0;
		    sp->SynHdrUnion.SageTxHeader.TypeUser.UserDefined = 0;

		    sp->SynHdrUnion.SageTxHeader.PacketControl.GenerateCRC = 1;
		    sp->SynHdrUnion.SageTxHeader.PacketControl.NativeAddress = 1;
		    sp->SynHdrUnion.SageTxHeader.VLanIndex.Index = inband_port_index;
		    sp->SynHdrUnion.SageTxHeader.VLanIndex.VLan = 0;
#endif

#ifdef CATALYST_OS
		    mac_drvr_send((uChar *)bp, sizeof(IBIPC_PKT_HDR_T), ibp->qflag);
#else
		    mac_drvr_send((uChar *)pak, sizeof(IBIPC_PKT_HDR_T), ibp->qflag);
#endif
		}	
		rc = 0;
		break;

	    default:
		break;
	}

	return(rc);
}

/*
 *
 */

static IBIPC_Table_T * get_ibipc_entry(
				       uInt16 sid, 
				       uInt16 did, 
				       uInt16 SrcSlot, 
				       uInt16 DestSlot,
				       int * rc)
{
  IBIPC_Table_T * ibp;

  for (ibp = ibipc_table_header; ibp; ibp = ibp->next) {
    if ((sid == ibp->sid) && 
	(did == ibp->did || did == IBIPC_ID_WILD_CARD) &&
	(SrcSlot == ibp->SourceSlot) &&
	(DestSlot == ibp->slot)) {

      *rc = IBIPC_RC_OK;
      return(ibp);
    }
  }

  *rc = IBIPC_RC_ILLEGAL_SID;
  return(0);
}

static IBIPC_Table_T * get_ibipc_entry4handle(uInt32 handle, int * rc)
{
	IBIPC_Table_T * ibp;

	for (ibp = ibipc_table_header; ibp; ibp = ibp->next) {
	     if (handle == ibp->handle) {
		 *rc = IBIPC_RC_OK;
		 return(ibp);
	     }
	}

	*rc = IBIPC_RC_ILLEGAL_HANDLE;
	return(0);
}

static int ipc_txd_queue(IBIPC_Table_T * ibp, IBIPC_TXD_T * txd)
{
	IBIPC_TXD_T * last_txd;
	int rc = 0;

	if (ibp->txd) {
	    for (last_txd = ibp->txd; (uInt32)(last_txd->next) != 0; last_txd = last_txd->next);
	    last_txd->next = txd;
	}
	else {
	    ibp->txd = txd;
	    rc = 1; /* indicate first in the queue */
	}

	txd->next = 0;
	return(rc);
}

/*
 *
 */

static void Ipc_addq(IBIPC_Table_T * ibp)
{
	ibp->next = ibipc_table_header;
	ibipc_table_header = ibp;
}

static
uChar * mac_drvr_buf_alloc (uInt32 mlen, uChar qflag)
{
	uChar * bp = 0;
#ifdef CATALYST_OS
	struct mbuf * m;
	
	/*
	 * allocate the memory buffer for the whole packet and
	 * return the pointer to the application data area.
	 */
    if (qflag == IPC_IN_BAND) {
	if (m = m_get(M_DONTWAIT, MT_DATA)) {
		mclget(m);
		if (m->m_len == MLEN)
			m_free(m);
		else {
			bp = mtod(m, uChar *);
		}
	}
    }
#ifdef IPC_OVER_OB
    else {
	bp = (uChar *) get_scpbuf();
    }
#endif
#else /* Cisco IOS */
	paktype *pak;
	
        if (pak = (paktype *) getbuffer(mlen)) {
	    bp = (uChar *)pak;
	}
#endif

	return(bp);
}

static
void mac_drvr_send(uChar * mbp, uInt32 len, uChar qflag)
{
#ifdef CATALYST_OS
	struct mbuf    *m;

    if (qflag == IPC_IN_BAND) {
	m = dtom(mbp);
	m->m_len = len;
	sonic_send(&sc_data[0], m);
    }
#ifdef IPC_OVER_OB
    else {
	enq_scpif(&if_scp_tx_hdr, mbp);
    }
#endif
#else /* Cisco IOS */
	#define LINK_IPC 63 /* !!! move this into linktype.h */
	#define ACCT_PROTO_IPC 100 /* !!! move this to interface.h */
	paktype *pak = (paktype *)mbp;
	idbtype *idb; /* what ??? */
	
	pak->if_output = idb;
	pak->linktype = LINK_IPC;
	pak->acct_proto = ACCT_PROTO_IPC;
	pak->datagramsize = len;

/*	datagram_out(pak);*/
	if (pak->datagramsize < 60) pak->datagramsize = 60;
#ifdef	IOS_BOOT
	{
		long	RC ;

		RC = BT_nmp_send(0, pak->datagramsize, pak->datagramstart);
		if (RC < 0)
			buginf ("\nmac_drvr_send:  BT_nmp_send RC %d\n", RC) ;
	}
#else
	aalsap_nmp_send(0, pak->datagramsize, pak->datagramstart);
#endif
	datagram_done(pak);
#endif
}

static
int is_tos_valid(uChar tos)
{
	if ((tos == IPC_DATAGRAM) || (tos == IPC_TYPE3))
	    return(1);
	else
	    return(0);
}

#ifndef CATALYST_OS

/* The followings are supportive functions needed by Cisco IOS.
 *
 * edisms() in IOS is used to replace sleep() and wakeup() from CATALYST_OS.
 */

/*
 * This routine is used with edisms() as wake up condition check for IPC TX
 * being blocked. 
 */
boolean ck_ipc_tx_block(ulong handle)
{
	IBIPC_Table_T * ibp;
	int rc;

	ibp = get_ibipc_entry4handle(handle, &rc);
	if (rc)
	    return(rc); /* panic !!! not register yet, just return 'pseudo TRUE' */

	if (ibp->txd)
	   return(TRUE);  /* still blocking */
	else
	   return(FALSE);
}

void ipc_tx_to(ulong arg1, ulong arg2, ulong arg3)
{
	ipc_tx(arg1);
}

#endif

#ifdef IPC_OVER_OB

IF_SCP_BUF_PTR ipc_ob_rx()
{
	IBIPC_Table_T * ibp;
	IF_SCP_BUF_PTR   bp;
	SCPHDR *       hdrp;
	int rc;
	uInt16 sid, did, SrcSlot, DestSlot;

	/* "destination id" in the packet is our "source id" and 
	 * "source id" in the packet is our "destination id".
	 */

	if ((bp = (IF_SCP_BUF_PTR) deq_scpif(&(_proc->if_scp_rx_hdr))) == NULL)
	    return(bp); /* nothing in the queue */

	hdrp = (SCPHDR *)&bp->ifscpbuf[0]; 

	sid = hdrp->dsap;
	did = hdrp->ssap;
	SrcSlot = hdrp->dest_addr + 1; /* Slot is 0 based in SCP */
	DestSlot = hdrp->src_addr + 1; /* Slot is 0 based in SCP */

	/*ibp = get_ibipc_entry(sid, IBIPC_ID_WILD_CARD, &rc);*/
	ibp = get_ibipc_entry(sid, did, SrcSlot, DestSlot, &rc);
	if (rc) {
	    /* drop the packet */
	    /* ??? */
	    trace(IPC_TRACE, 1, "no IPC entry found, sid = %x, did = %x\n", sid, did);
	    rel_scpbuf(bp);
	    return(0);
	}

	return(bp);
}

IBIPC_TXD_T * 
ipc_ob_send(IBIPC_Table_T * ibp,	/* IPC control table */
	    uInt16  did,		/* destination ID */
	    uChar   slot,		/* destination slot */
	    uChar   tos,		/* type of service */
	    uInt32  mlen,		/* message length after application header */
	    uChar   * mptr)		/* message buffer (ie. application hdr) */
{
	SCPHDR		* hdrp;
	IBIPC_TXD_T	* txd;


	(uChar *)hdrp = mptr - SCP_HEADER_LENGTH;
	txd = (IBIPC_TXD_T *)(mptr - sizeof(IBIPC_PKT_HDR_T) - sizeof(IBIPC_TXD_T));

	/* Set up SCP header
	 */

	hdrp->src_addr  = my_slot - 1;
	hdrp->dest_addr = my_slot - 1;
	hdrp->length = mlen;
	hdrp->dsap = did;
	hdrp->ssap = ibp->sid;
	hdrp->opcode = 0x11; /* ??? */
	
	/* Set up TX descriptor
	 */

	txd->next = 0;
	txd->pkt  = (uChar *)hdrp;
	txd->pkt_len  = mlen + SCP_HEADER_LENGTH;

	txd->timer_index = -1;
	txd->timer = IPC_ACK_TIMER;
	txd->retry = IPC_TX_RETRY;
	txd->tos = tos;
	txd->did = did;
	txd->slot = slot;

	return(txd);
}
#endif


int
GetMySlotNumber()
{
#ifdef CATALYST_OS
  return(1);
#else
  return(LCP_PhysSlotNbr());
#endif
}
