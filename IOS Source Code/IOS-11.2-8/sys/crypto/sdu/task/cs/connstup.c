/* $Id: connstup.c,v 1.1.68.6 1996/09/11 23:23:06 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/task/cs/connstup.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: connstup.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: connstup.c,v $
 * Revision 1.1.68.6  1996/09/11  23:23:06  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 1.1.68.5  1996/08/28  19:31:37  bew
 * CSCdi66328:  crypto connection set too early on CRM side
 * Branch: California_branch
 * Exchange the pending connection id with connection id on
 * the CRM side only after the CCM has been received and
 * verified.
 *
 * Revision 1.1.68.4  1996/07/06  19:45:32  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/06/17  22:25:02  bew
 * CSCdi60484:  error_proc multiply defined in APPN and Cylink crypto code
 * Branch: California_branch
 * Change error_proc() to Error_proc() in Cylink code.
 *
 * Revision 1.1.68.2  1996/06/05  23:12:07  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:55  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.6  1996/04/16  21:54:08  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 1.1.2.5  1996/03/25  18:44:02  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 1.1.2.4  1996/03/13  22:45:33  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 1.1.2.3  1996/01/16  20:21:02  bew
 * Branch: IosSec_branch
 * Simplified the session mgmt debug msgs & changed the way that duplicate
 * connection messages are handled.
 *
 * Revision 1.1.2.2  1995/12/12  23:12:27  bew
 * Branch: IosSec_branch
 * Better formatting of debug messages.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:58  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:24  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Task/CS/CONNSTUP.C 43    7/20/95 5:45p Tchi 
*
* DESCRIPTION:  Connection Setup Source Code
*
* NOTES: RTXC Task Level
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CONNSTUP.C 
* 
* *****************  Version 43  *****************
* User: Tchi         Date: 7/20/95    Time: 5:45p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed check_unmanaged_sdu
* 
* *****************  Version 42  *****************
* User: Tchi         Date: 7/19/95    Time: 5:49p
* Updated in $/SecureLAN/SDU/Task/CS
* alter the challenge number after signing the message
*******************************************************************************/

#define CONNSTUP_C

/*******************************************************************************
* INCLUDE FILES
*******************************************************************************/
#include "../crypto/sdu/include/cylink.h"

#ifndef CISCO
#include <time.h>
#include <mem.h>
#endif /* CISCO */

/* Generic include files */
#include "../crypto/sdu/include/iqueue.h"
#ifndef CISCO
#include "../crypto/sdu/include/rtxcapi.h"
#endif
#include "../crypto/sdu/include/rtxcinfo.h"
#include "../crypto/sdu/include/debug.h"

/* Other SDU module files */
#include "../crypto/sdu/include/pa/pactl.h"
#ifndef CISCO
#include "../crypto/sdu/include/phybufm.h"
#endif /* CISCO */
#include "../crypto/sdu/include/connstup.h"
#include "../crypto/sdu/include/csutil1.h"
#include "../crypto/sdu/include/csutil2.h"
#include "../crypto/sdu/include/csdata.h"
#include "../crypto/sdu/include/mgdata.h"
#include "../crypto/sdu/include/net.h"
#include "../crypto/sdu/include/auth/authext.h"
#include "../crypto/sdu/include/iomgr.h"

/*Crypto related */
#include "../crypto/sdu/include/cy_api.h"

#ifdef CISCO
#include <ciscolib.h>
/* BEW: This is only here because of the updates to the cisco field. Better
 *      would be to put this and the updates should be in a glue file.
 */
#include "../crypto/crypto_glue_sdu.h"
#include "../crypto/crypto_glue_structs.h"
cisco_FrameControlHeader *cisco;
#include "address.h"
#include "../crypto/crypto_map.h"
#endif /* CISCO */

/*******************************************************************************
* STATIC DECLARATIONS (prototypes)
*******************************************************************************/
#ifdef CISCO /* Need to call these from the IOS */
void 	send_nnc_req(Pa2EpFrameControl *);
void 	recv_nnc(Pa2EpFrameControl *);
#else
static void 	send_nnc_req(Pa2EpFrameControl *);
static void 	recv_nnc(Pa2EpFrameControl *);
#endif
static void 	recv_nnc_req(Pa2EpFrameControl *, uchar *);
static void 	recv_nnc_rpy(Pa2EpFrameControl *, uchar *);
static void 	recv_nnc_cfm(Pa2EpFrameControl *, uchar *);
static void 	handle_nnc_req(Pa2EpFrameControl *, uchar *, short, ushort);
static int		sdu_has_no_nw_addr(ushort);
#ifdef CISCO
static uchar	check_unmanaged_sdu(uchar **, uchar *, CIBptr);
#else
static uchar	check_unmanaged_sdu(uchar **, uchar *);
#endif

/*******************************************************************************
* STATIC GLOBAL DATA
*******************************************************************************/
typedef   void (* functype1)  (Pa2EpFrameControl *, uchar *);

static functype1 recv_nnc_func[] =
{
	recv_nnc_req,
	recv_nnc_rpy,
    recv_nnc_cfm
};

/*******************************************************************************
* PUBLIC DECLARATIONS (prototypes)
*******************************************************************************/
void connstup(void);
uchar cs_init(void);

#ifdef CISCO

#define LONGEST_STR_LENGTH			40
#define STR_AUTH_NNC_SIG_FAILED		"NNC signature failed"
#define STR_AUTH_SDU_SIG_FAILED		"Connection message verify failed"
#define STR_AUTH_CHALLENGE_FAILED	"Challenge failed"
#define STR_AUTH_UNKNOWN			"Unknown error"

/****************************************************************************
* NAME:             Print_status_error
*
* DESCRIPTION:      This function prints errors found in the connection
*					message field
****************************************************************************/
static void Print_status_error (uchar status)
{
	char str[LONGEST_STR_LENGTH];
	switch (status) {
	case AUTH_NNC_SIG_FAILED:
		sstrncpy(str, STR_AUTH_NNC_SIG_FAILED, LONGEST_STR_LENGTH);
		break;
	case AUTH_SDU_SIG_FAILED:
		sstrncpy(str, STR_AUTH_SDU_SIG_FAILED, LONGEST_STR_LENGTH);
		break;
	case AUTH_CHALLENGE_FAILED:
		sstrncpy(str, STR_AUTH_CHALLENGE_FAILED, LONGEST_STR_LENGTH);
		break;
	default:
		sstrncpy(str, STR_AUTH_UNKNOWN, LONGEST_STR_LENGTH);
		break;
	}
	DEBUG_WITH_ARG("Far end authentication error: %s", str);
	return;
}
#endif /* CISCO */

/****************************************************************************
* NAME:             cs_init
*
* DESCRIPTION:      This function is the initilization routine for CS.
*                   It should be called by Startup Manager.
*
* INPUTS, OUTPUTS:
*       (1) Initialize global data such as: error code, MIB variables, ...
*       (2) Set up pending connection free list
*
* RETURNS:
*       0 : if it initializes succesfully. Otherwise error code is returned
****************************************************************************/
uchar cs_init(void)
{
	short       i;
	uchar		*p;		/*point to the first byte of open-close flag in NNC */
	uchar		open;

	/********************************************
	* Global data initialization
	********************************************/
	cs_err = CS_NO_ERROR;
	/*the error entities are meaningless unless the correspoinding bit */
	/*position in cs_err is set. */

	sdu_conn_built = 0L;
	sdu_pconns = 0L;
#ifndef CISCO
	cs_unique_conn_id = 0L;
#endif

	/*a special NNC used to exchange system's open-close flag: */
    memset(nnc_sys_oc, 0, MAX_NNC_SIZE);
	/*set the open-close flag */
	p = nnc_sys_oc + NNC_FIX_GRP_OFFSET + dssPkeyLen + NNC_GRP_OC_OFFSET;
	*p = 0;
	*(p + 1) = 0;
	open = OPEN_NODE >> 8;
	if (ep_ctrl & OPEN_SDU)
	{
		(*p) |= open; 		/*bypass traffic */
	}
	else
	{
		(*p) &= (~open); 	/*block traffic */
	}

	/*set the number of groups so this sepcial NNC has a fixed size */
	*(p - 1) = 1;

	/********************************************
	* Pending connection list initialization
	********************************************/
	pconn_used = NULL_INDEX;
	pconn_free = 0;
	for (i = pconn_free; i < PCONN_ENTRIES; i++)
	{
		pconn_list[i].next = i + 1;
		pconn_list[i].prev = i - 1;
	}
    pconn_list[pconn_free].prev = NULL_INDEX;
	pconn_list[PCONN_ENTRIES - 1].next = NULL_INDEX;

	/**********************************************************************
	* Check config table is missing. Do not return error if it is missing.
	* Otherwise, system will not start up. Just stay in OFFLINE mode until
	* config tables have been downloaded from SM
	***********************************************************************/
#ifndef CISCO
	if (csAvail.bitMap != CS_CONFIG_TB_AVAIL)
		post2IOmanager(CONNSTUP, CS_EVT_CONFIG_TB_MISSING, 0);
#endif 

	return(CS_NO_ERROR);
}

#ifndef CISCO
/*******************************************************************************
* NAME:             connstup
*
* DESCRIPTION:      This task is invoked by Entity Protection
*
* INPUTS:
*       Entries in "iq_cs_frm"
*
* OUTPUT:
*       Depending on if the frame is NNC Echo Request, Reply, or Confirm,
*       Ether frames are generated to the network or CIB is filled.
*
*******************************************************************************/
void connstup(void)
{
	Pa2EpFrameControl   *entry;

	KS_defslice(SELFTASK, (TICKS) 0);   /*Turn off time slicing */
	for (;;)
	{
		/*Wait until EP invokes CS, or itself wakes up every 800 msec */
		/*to administrate the pending connection list */
		KS_waitt(CS_FRM_SEMA, 800/CLKTICK);

		/***************************************************************
		* Process all queue entries
		***************************************************************/
		IQ_GET(iq_cs_frm, entry);
		while (entry != NULL)
		{
        	/*so we won't get partially updated config data if SNMP SetReq is preempted */
			KS_lockw(CS_CONFIG_RES);
			if (entry->frameMode == ConnectInit)
			{	/*EP requests to build a new connection. */
				send_nnc_req(entry);
			}
			else   /*EP posts the received NNC Req; */
			{      /*PA posts the received NNC Reply and Confirm. */
				recv_nnc(entry);
			}
			KS_unlock(CS_CONFIG_RES);

			taskReleaseBuffer(entry->descriptorLink);  /*release frame control */
			IQ_GET(iq_cs_frm, entry);	/*process next entry */
		}

		/****************************************************************
		*   Check pending connection list to see any timed out entries
		*****************************************************************/
		pconn_mgmt();
	}
}
#endif /* !CISCO */

/*******************************************************************************
* NAME:             send_nnc_req
*
* DESCRIPTION:      These function sends an Echo Request frame embedded
*                   with NNC. Different protocol has different Echo frame.
*
* INPUTS:           frm_ctrl - the frame control associated with the data frame
*                              that incurs NNC exchange.
*                   NOTICE THAT the CIB pointer in frm_ctrl is non-null.
*
* OUTPUT:           A NNC Echo Request frame is sent out depending on the
*                   protocol.
*
*******************************************************************************/
#ifdef CISCO
void send_nnc_req(Pa2EpFrameControl *fc)
#else
static void send_nnc_req(Pa2EpFrameControl *fc)
#endif /* !CISCO */
{
	uchar               *tx_buf;    /*transmit buffer. point to layer 2. */
#ifndef CISCO
	uchar               *l3;        /*point to layer 3 in tx_buf */
	uchar               *l4;        /*point to layer 4 in tx_buf */
#endif /* !CISCO */
	uchar               *l5;        /*point to connection setup msg in tx_buf */
	short               index;      /*pending connecdtion list index */
	ushort              pid;        /*protocol ID */
	uchar               *end_l5;    /*pointer to the byte after the frame */
	uchar               *nnc_ptr;
	Cy2TxFrameControl   *fch;       /*frame control header */
	pconn_entry         *pconn;
	CIBptr              cib_ptr;
	uchar				auth;
	int					use_new_old;

#ifdef INTGR_DEBUG	/*TEST ONLY */
	ep_post_cs_count++;
#endif

	/******************************************************************
	* Initialization
	*******************************************************************/
	cib_ptr = fc->epEntry.info.cs.cib;				/*non-null */
	pid = fc->epEntry.pe.pid;
	auth = CS_NO_ERROR;
	/* Get a valid NNC or the dummy NNC with system open-close flag. */
	nnc_ptr = get_nnc(fc, &auth, &use_new_old); 	/*non-null */

	/******************************************************************
	* Since NNC Reply and Confirm use SDU's network address,
	* we need to make sure SDU has a network address for this protocol.
	*******************************************************************/
	if (sdu_has_no_nw_addr(pid))
	{
		Error_proc(cib_ptr, nnc_ptr + NNC_FIX_GRP_OFFSET + dssPkeyLen, 0L, 0, 0, 0);
		return;
	}

	/******************************************************************
	* Send NNC request frame
	******************************************************************/
	fch = taskAcquireBuffer();
	if (fch)
	{
		tx_buf = fch->frameP;
		index = alloc_pconn(fc);
		if (index != NULL_INDEX)    /*entry allocated successfully */
		{
			/********************************************************
			* get different layer pointers
			********************************************************/
			pconn = pconn_list + index;
#ifdef CISCO
			/*
			 * Record in this pending connection entry that we sent
			 * the NNC Req.
			 */
			pconn->cisco.i_sent_cim = TRUE;
			/*
			 * Bypass usage of l2, l3 & l4.
			 */
			l5 = tx_buf;
			/*
			 * The connection ID is a circular number based on an
			 * crypto engine restriction. Save the connid in the CIB for 
			 * the IOS.
			 */
			fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id = 
  crypto_glue_set_cs_unique_conn_id(fc->epEntry.info.cs.cib->crypto.cisco.slot);
			/********************************************************
			* build the frame
			********************************************************/
			build_cs_msg_req(fc, l5, index, &end_l5, nnc_ptr, auth,
			    fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id);
#else
			l3 = tx_buf + (int)(fc->networkLink - fc->frameLink);
			l4 = l3 + (int)(fc->transportLink - fc->networkLink);
			l5 = l4 + l4_header_size[pid - INTERNET_IP];
			cs_unique_conn_id++;
			/********************************************************
			* build the frame
			********************************************************/
			build_cs_msg_req(fc, l5, index, &end_l5, nnc_ptr, auth);
			memcpy(tx_buf, fc->frameLink, (int)(l4 - tx_buf));  /*copy L2,L3 */
			if (fc->receivedPath == UPE_SIDE)
			{
				swap_l2_da_sa(tx_buf, fc->macType);
				swap_l3_da_sa(l3, pid);
			}

			(*init_l3_l4_header[pid - INTERNET_IP]) (l3, l4, end_l5, INIT_ICMP_ECHO_REQ);

			if (fc->macType == IEEE8023) ADJ_LLC_LENGTH(tx_buf, end_l5);
#endif /* !CISCO */

			/***********************************************************************
			* Set up the pending connection list.
			*
			* Partially save the frame transmitted for the sake of retransmission
			* Copy L2, L3, L4, and L5 except NNC and signature.
			***********************************************************************/
			pconn->nnc = nnc_ptr;	/*non-null */
			pconn->l5_offset = (ushort)(l5 - tx_buf);
			pconn->tx_count = 1;
			pconn->retx_fsize = (ushort)(end_l5 - tx_buf);
			memcpy(pconn->retx_buf,
				   tx_buf,
				   ((ushort)(l5 - tx_buf)) + PDU_FIX_NNC_OFFSET + dhPublicLen);

			pconn->retx_port = fc->fch.txPortP;
			pconn->retx_action = (fc->receivedPath == UPE_SIDE) ?
								  FRAME_REFLECT : FRAME_FORWARD;
			pconn->cib = cib_ptr;  /*non-null */
			pconn->pe = fc->epEntry.pe;
			pconn->upe = fc->epEntry.upe;

			/*undefined if "STAT_SYS_OC_FLAG bit" is set in "auth" */
			pconn->sm_dss_pub_new_old = use_new_old;

			GetRTClock((ulong *)(&pconn->timestamp));
#ifdef CISCO
			/*
			 * Fill in src & dst to both pconn struct (for retransmission) and
			 * fch, for this transmission. The fch copy is done via the struct
			 * copy below.
			 */
		    pconn->cisco.src = fc->epEntry.cisco.src;
		    pconn->cisco.dst = fc->epEntry.cisco.dst;
			/*
			 * Indicate that this is NOT a re-transmitted frame & copy
			 * the epEntry structure.
			 */
			cisco = (cisco_FrameControlHeader *)fch->otherP;
			cisco->retransmit = FALSE;
			cisco->epEntry = fc->epEntry; /* struct copy */
#endif /* CISCO */

			/*********************************************************
			* Send out the frame
			*********************************************************/
			cs_transmit_frm(fch,
							pconn->retx_fsize,
							&(fc->epEntry.pe),
							&(fc->epEntry.upe),
							pconn->retx_port,
							pconn->retx_action);
			/*no error checking since if the transmission fails, a retransmission */
			/*will occur when it times out */

			DEBUG(cs200);
		}
		else
		{
			taskReleaseBuffer(fch->descLogP);
			Error_proc(fc->epEntry.info.cs.cib,
					   nnc_ptr + NNC_FIX_GRP_OFFSET + dssPkeyLen,
					   CS_ERR_NO_PCONN_ENTRY,
					   &cs_err_no_pconn_entry,
					   &(fc->epEntry.pe),
					   &(fc->epEntry.upe));
		}
	}
	else
	{
		Error_proc(fc->epEntry.info.cs.cib,
				   nnc_ptr + NNC_FIX_GRP_OFFSET + dssPkeyLen,
				   CS_ERR_NO_TX_BUF,
				   &cs_err_no_tx_buf,
				   &(fc->epEntry.pe),
				   &(fc->epEntry.upe));
	}
}

/*******************************************************************************
* NAME:             recv_nnc
*
* DESCRIPTION:      These functions process the received NNC Echo frames
*                   depending on the protocol. It needs to further distinguish
*                   if the recevied frame is NNC Echo Request, Reply, or Confirm.
*
* INPUTS:           fc - the frame control associated with the data frame
*                              that incurs NNC exchange.
*
* OUTPUT:           Different actions for NNC Echo Request, Reply, or Confirm.
*                   See the each individual function description.
*
*******************************************************************************/
#ifdef CISCO
void recv_nnc(Pa2EpFrameControl *fc)
#else
static void recv_nnc(Pa2EpFrameControl *fc)
#endif
{
	uchar * pdu;     /*point to where connection setup message in the frame */

	pdu = (fc->transportLink) +
		  l4_header_size[fc->epEntry.pe.pid - INTERNET_IP];

	if (pdu[PDU_PS_OFFSET] == PS_SDU)   /*NNC from SDU */
	{
		if (fc->receivedPath == UPE_SIDE)   /*NNCs always arrive at this port */
		{
			/*index into the table to call different functions. */
			(*recv_nnc_func[*(pdu + PDU_OP_OFFSET)]) (fc, pdu);
		}
		else
		{
			/*else: invalid configuration of cascaded SDUs. discard the frame. */
			Error_proc(0, 0, CS_ERR_CASCADED_CONFIG, 0, 0, 0);
		}
	}
	else
	{
#ifdef INTGR_DEBUG	/*TEST ONLY */
		cs_debug |= CS_TEST_SFU_SNIC;
#endif
#ifndef CISCO
		/*NNC from SFU, SNE, or SNIC. bypass the frame. */
		cs_transmit_frm((Cy2TxFrameControl *)fc,
						fc->frameLength,
						0,
						0,
						fc->fch.txPortP,
						FRAME_FORWARD);
#endif
	}
}

/*******************************************************************************
* NAME:             recv_nnc_req
*
* DESCRIPTION:      When PA receives a NNC Echo Request frame, A frame control
*                   will be placed in queue to invoke CS. CS calls this function.
*
* INPUTS:           fc  - frame control
*                   pdu - pointer to the connection setup message layer 5
*
* OUTPUT:           An NNC Echo Reply frame will be sent back; a pending
*                   connection entry will be added to the list.
*
*******************************************************************************/
static void recv_nnc_req(Pa2EpFrameControl *fc, uchar *pdu)
{
	pconn_entry         *pconn;
	short   			race_index;         /*pending connecdtion list index */
	short   			i;
	ushort				recv_nnc_size;
#ifdef CISCO
	ulong				my_serial;
#endif

	/*************************************************************************
	* Sanity check: if bad size, or there is no far SDU but the far PE echoes
	*				back, discard this frame.
	************************************************************************/
#ifndef CISCO
	if (bad_nnc_frm_size(fc, pdu, ECHO_REQUEST, &recv_nnc_size) ||
		((*((ulong *)(pdu + PDU_SERIAL_NUM_OFFSET))) ==
		 (*((ulong *)mg_sdu_serial_num))))
		return;
#else
        if (bad_nnc_frm_size(fc, pdu, ECHO_REQUEST, &recv_nnc_size) ||
          crypto_glue_is_my_snum((*((ulong *)(pdu + PDU_SERIAL_NUM_OFFSET))))){
                return;
        }
#endif

	/*************************************************************
	* Search pending connection list for any race condition.
	*
	* A race condition occurs when two SDUs send NNC Init msg
	* to each other at the same time
	*************************************************************/
	race_index = NULL_INDEX;    /*initialization */
	i = pconn_used;
	while ((i != NULL_INDEX) && (race_index == NULL_INDEX)) /*search entity IDs */
	{
		if ( !memcmp(&(pconn_list[i].pe),  &(fc->epEntry.pe),  6) &&
			 !memcmp(&(pconn_list[i].upe), &(fc->epEntry.upe), 6) )
		{
			race_index = i;
		}

		i = pconn_list[i].next;
	}

	/********************************************************************
	* Take different actions depending on if there is any race condition
	*********************************************************************/
	if (race_index == NULL_INDEX)
	{
		/*no race condition */
		handle_nnc_req(fc, pdu, race_index, recv_nnc_size);
	}
	else
	{
		pconn = pconn_list + race_index;
		if ((*(pconn->retx_buf + pconn->l5_offset + PDU_OP_OFFSET)) ==
			 ECHO_REPLY)	/*frame sent is NNC Reply */
		{
			/*no race condition. */
			/*this is a retransmission of NNC Req from other SDU. */
#ifdef CISCO
			/*this retransmission came AFTER we sent them an NNC Reply*/
#endif /* CISCO */
			/*handle_nnc_req(fc, pdu, race_index, recv_nnc_size); */
			pconn_retransmit(race_index);

#ifdef INTGR_DEBUG	/*TEST ONLY */
			cs_debug |= CS_TEST_RETX;
#endif
		}
		else	/*frame sent is NNC Request */
		{
			/*race condition found. */

#ifdef INTGR_DEBUG	/*TEST ONLY */
			race_count++;
#endif
			DEBUG("CS: race condition found");
#ifndef CISCO
			/* If non-cisco code check sdu_serial_number */   
			if (memcmp(pdu + PDU_SERIAL_NUM_OFFSET,
					   mg_sdu_serial_num,
					   SERIAL_NUM_SIZE) > 0)
#else
			/* 
			 * If cisco code, find the serial number for the crypto engine
			 * associated with the slot associated with this connection msg. 
			 * in my public key linked list. Compare that serial number with 
			 * the one in the connection message to see who wins. 
			 * (Largest s/n wins.)
			 */
			my_serial = crypto_glue_get_sn_by_slot(
								   fc->epEntry.info.cs.cib->crypto.cisco.slot);
			if (memcmp(pdu + PDU_SERIAL_NUM_OFFSET,
					   &my_serial,
					   SERIAL_NUM_SIZE) > 0)
#endif

			{
				/*race condition found and I lose */
#ifdef CISCO
				/*
				 * Because some routers are slow, we may have the
				 * case where this transmission came BEFORE we got around to
				 * sending them an NNC Reply. If so, we want to drop this
				 * duplicate NNC Req. We recognise this because we recorded
				 * the fact in the COT entry when we got the first copy of
				 * the NNC Req.
				 *
				 * Otherwise we have a real race condition which we lost.
				 */
				if (pconn_list[race_index].cisco.i_sent_cim == TRUE) {
					handle_nnc_req(fc, pdu, race_index, recv_nnc_size);
				} else {
					DEBUG("Dropping a CIM we already received");
				}
#else /* !CISCO */
				handle_nnc_req(fc, pdu, race_index, recv_nnc_size);
#endif /* CISCO */
			}
			/*else: race condition found and I win. ignore this frame. */

#ifdef INTGR_DEBUG	/*TEST ONLY */
			else
			{
				cs_debug |= CS_TEST_RACE_DROP_FRAME;
			}
#endif
		}
	}
}

/*******************************************************************************
* NAME:             handle_nnc_req
*
* DESCRIPTION:      This function acts on the received NNC Request frame.
*
* INPUTS:           fc  - the frame control associated with the received NNC
*                         Request frame
*                   pdu - point to where the received connection message starts
*                   race_index - indicating if there is a race condition
*
* OUTPUT:           An NNC Echo Reply frame will be sent back; a pending
*                   connection entry will be added to the list.
*
*******************************************************************************/
static void handle_nnc_req(	Pa2EpFrameControl 	*fc,
							uchar 				*pdu,
							short 				race_index,
							ushort 				recv_nnc_size)
{
	uchar               *tx_buf;        /*transmit buffer. point to layer 2. */
#ifndef CISCO
	uchar               *l3;            /*point to layer 3 in tx_buf */
	uchar               *l4;            /*point to layer 4 in tx_buf */
#endif
	uchar               *l5;            /*point to connection setup msg in tx_buf */
	uchar               *end_l5;        /*pointer to the byte after the frame */
	CIBptr              cib_ptr;
	uchar               *nnc_ptr;   	/*to be transmitted back to initiator */
										/*and used to create CIB */
	Cy2TxFrameControl   *fch;           /*frame control header */
	pconn_entry         *pconn;
	uchar				*nnc_grp;
	uchar				*other_sdu_dss_pub;
	int					use_new_old;
	short               index;          /*allocated pending connecdtion list index */
	ushort              pid;            /*protocol ID */
	ushort				retx_l5_offset;
	uchar               auth;
	uchar				far_end_auth;
	uchar				um_other_sdu;
#ifdef CISCO
	long				*pdu_long_ptr;
#endif

#ifdef INTGR_DEBUG	/*TEST ONLY */
	ep_post_cs_count++;
#endif

	fch = taskAcquireBuffer();
	if (fch)
	{
		tx_buf = fch->frameP;
		index = (race_index == NULL_INDEX) ? alloc_pconn(fc) : race_index;
#ifdef CISCO
		/*
		 * Either this is the first NNC Req we've gotten, or we're overwriting
		 * the value if we lost a race condition.
		 */
		pconn_list[index].cisco.i_sent_cim = FALSE;
#endif /* CISCO */
		if (index != NULL_INDEX)
		{
			/********************************************************
			* Authentication check
			********************************************************/
			/*initialization */
			auth = CS_NO_ERROR;
			pconn_list[index].pe = fc->epEntry.pe;  /*for the ease of error process */
			pconn_list[index].upe = fc->epEntry.upe;
			cib_ptr = fc->epEntry.info.cs.cib;
			far_end_auth = pdu[PDU_STAT_OFFSET];
#ifdef CISCO
		    /*
		     * Setup the cib with Cisco-specific stuff.
		     */
	    	crypto_glue_setup_cib(cib_ptr, fc->epEntry.cisco.ptr);
			/* 
			 * Set the cib ptr here reather than later.
			 * Cisco needs it during sig. verif. 
			 */
		    pconn_list[index].cib = cib_ptr; 
			/*
			 * This is to aid in debugging
			 */
			if ((far_end_auth & STAT_AUTH_MASK) != CS_NO_ERROR) {
				Print_status_error(far_end_auth);
			}
#endif /* CISCO */
			um_other_sdu = check_unmanaged_sdu(&other_sdu_dss_pub, pdu,
											   cib_ptr);
#ifdef CISCO
			/*
			 * If we couldn't get a public number, we'd better let the other
			 * side know.
			 */
			if ((um_other_sdu == 1) && (other_sdu_dss_pub == NULL)) {	
				auth |= AUTH_SDU_SIG_FAILED;
				DEBUG("Peer public key not found");
			}
#endif /* CISCO */

			/*if message signature has been generated and DSS exists */
			if (!(far_end_auth & STAT_GEN_SIG_FAILED) && other_sdu_dss_pub)
			{
				auth |= check_frame_sig(index, pdu, other_sdu_dss_pub, recv_nnc_size);
			}

			/*verify NNC revisions */
			auth |= handle_nnc_req_revisions(pdu, fc, &use_new_old,
							&nnc_ptr, far_end_auth); /*"nnc_ptr" non-null */

			/******************************************************************
			* Since NNC Reply and Confirm use SDU's network address,
			* we need to make sure SDU has a network address for this protocol.
			*******************************************************************/
			if (sdu_has_no_nw_addr(fc->epEntry.pe.pid))
			{
				Error_proc(	cib_ptr,
							nnc_ptr + NNC_FIX_GRP_OFFSET + dssPkeyLen,
							0L, 0, 0, 0);
				free_pconn(index);
				taskReleaseBuffer(fch->descLogP);
				return;
			}

			/*if the far end is not using the dummy NNC with system flag and */
			/*variable "use_new_old" has been previously defined */
			if (!(far_end_auth & STAT_SYS_OC_FLAG) &&
				!(auth & STAT_SYS_OC_FLAG) &&
				!um_other_sdu)
			{
				auth |= check_nnc_sig( pdu,
									   index,
									   use_new_old,
									   recv_nnc_size,
									   *(ulong *)(nnc_ptr + NNC_CUS_ID_OFFSET) );
			}

			if (((auth & STAT_AUTH_MASK)!= CS_NO_ERROR) || (!cib_ptr))
			{
				/************************************************************
				* Failed authentication:
				*   A CIB will be created later to block the traffic.
				*   Therefore no pending conneciton entry is needed.
				************************************************************/

			   /************************************************************
				* Null CIB:
				*   A far end PE sends a NNC frame to a local UPE, or we are
				*   out of PET entry (in both cases EP processes data
				*   frames according to a system flag). For both cases, EP
				*   passes a null CIB and UPE NNC. The UPE NNC has a zeroed
				*	membership therefore a open/close connection will be
				*   built.
				*
				*   No need to keep an entry in pending connection list (nor
				*   to create a CIB) for either case
				************************************************************/

				free_pconn(index);
				index = NULL_INDEX;		/*so far end won't send back message */
			}

			/********************************************************
			* Get different layer pointers
			********************************************************/
			pid = fc->epEntry.pe.pid;
#ifdef CISCO
			/*
			 * Bypass usage of l2, l3 & l4.
			 */
			l5 = tx_buf;

			/*
			 * Copy the peer network address into fc->epEntry.cisco.dst 
			 * for use during building the CRM.
		 	 */
			pdu_long_ptr = (long *) (pdu + PDU_NW_ADDR_OFFSET);
			fc->epEntry.cisco.dst = *pdu_long_ptr;

			/*
			 * The connection ID is a circular number based on an
			 * crypto engine restriction.
			 */
			if ((index != NULL_INDEX) && (race_index == NULL_INDEX))
			{
				/*
				 * Generate the connection id & stuff it in the cib
				 * while we're at it. It also has to be stored in the
				 * fc so we can pass it onto the EPA to bind the  
				 * connection in the EPA. (cib_ptr should be the same as
				 * the fc cib_ptr.)
				 */
				cib_ptr->crypto.cisco.new_connection_id = 
  crypto_glue_set_cs_unique_conn_id(fc->epEntry.info.cs.cib->crypto.cisco.slot);

			}
			/********************************************************
			* Build the frame
			********************************************************/
			retx_l5_offset = (ushort)(l5 - tx_buf);
			/*DH share number is created by this function */
			build_cs_msg_rpy(fc, l5, &auth, pdu, nnc_ptr, /*nnc: non-null */
							 &end_l5, index, retx_l5_offset,
							 cib_ptr->crypto.cisco.new_connection_id);
#else
			l3 = tx_buf + (int)(fc->networkLink - fc->frameLink);
			l4 = l3 + (int)(fc->transportLink - fc->networkLink);
			l5 = l4 + l4_header_size[pid - INTERNET_IP];

			if ((index != NULL_INDEX) && (race_index == NULL_INDEX))
			{
				cs_unique_conn_id++;
			}

			/********************************************************
			* Build the frame
			********************************************************/
			retx_l5_offset = (ushort)(l5 - tx_buf);
			/*DH share number is created by this function */
			build_cs_msg_rpy(fc, l5, &auth, pdu, nnc_ptr, /*nnc: non-null */
							 &end_l5, index, retx_l5_offset);
			memcpy(tx_buf, fc->frameLink, (int)(l4 - tx_buf));   /*copy L2,L3 */
			build_cs_frm_l2_addr(tx_buf, fc->macType);
			build_cs_frm_l3_addr(l3, pid, pdu);

			(*init_l3_l4_header[pid - INTERNET_IP]) (l3, l4, end_l5,
													 INIT_ICMP_ECHO_RPY);

			if (fc->macType == IEEE8023) ADJ_LLC_LENGTH(tx_buf, end_l5);
#endif

			/***********************************************************************
			* Set up pending connection list entry.  *
			* Partially save the frame transmitted for the sake of retransmission
			* Copy L2, L3, L4, and L5 except NNC and signature.
			* "original" challenge number has already been copied into buffer.
			***********************************************************************/
			if (index != NULL_INDEX)
			{
				pconn = pconn_list + index;
				pconn->nnc = nnc_ptr;
				pconn->l5_offset = retx_l5_offset;
				pconn->tx_count = 1;
				pconn->retx_fsize = (ushort)(end_l5 - tx_buf);

				/*copy L2, L3, L4, and L5 before challenge number */
				memcpy(pconn->retx_buf,
					   tx_buf,
					   retx_l5_offset + PDU_CHAL_OFFSET);
				/*copy DH public number */
				memcpy(pconn->retx_buf + retx_l5_offset + PDU_FIX_NNC_OFFSET,
					   tx_buf + retx_l5_offset + PDU_FIX_NNC_OFFSET,
					   dhPublicLen);

				pconn->retx_port = fc->fch.txPortP;
				pconn->retx_action = FRAME_REFLECT;
#ifndef CISCO   /* Done before signature verification at Cisco */
				pconn->cib = cib_ptr;
#endif

				/*Becomes undefined if "auth" in the frame sent out has */
				/*"STAT_SYS_OC_FLAG" bit set. */
				pconn->sm_dss_pub_new_old = use_new_old;

				memcpy(pconn->other_dss_pub, other_sdu_dss_pub, dssPkeyLen);
#ifdef CISCO
				/*
				 * Fill in src & dst to both pconn struct (for retransmission) 
				 * and fch, for this transmission. The fch copy is done via the
				 * struct copy below.
				 */
		    	pconn->cisco.src = fc->epEntry.cisco.src;
		    	pconn->cisco.dst = fc->epEntry.cisco.dst;
				if (um_other_sdu) {
					/*
					 * The public key pointer points to malloc'ed memory.
					 */
					free(other_sdu_dss_pub);
				}
#endif /* CISCO */

				GetRTClock((ulong *)(&pconn->timestamp));
			}
#ifdef CISCO
			/*
			 * Copy the peer network address previously saved into the pconn
			 * struct for re-transmission's sake. Mark the packet we're about 
			 * to send as being sent the first time (that is, not
			 * retransmitted) Copy the epEntry structure so we have
			 * all the info to get the packet out of the IOS.
			 */
	        cisco = (cisco_FrameControlHeader *)fch->otherP;
			cisco->retransmit = FALSE;
			cisco->epEntry = fc->epEntry; /* struct copy */
#endif /* CISCO */

			/**********************************************************
			* Send back the frame.
			***********************************************************/
			cs_transmit_frm(fch,
							(ushort)(end_l5 - tx_buf),
							&(fc->epEntry.pe),
							&(fc->epEntry.upe),
							fc->fch.txPortP,
							FRAME_REFLECT);
			/*no error checking since if the transmission fails, a retransmission */
			/*will occur when it times out */

			DEBUG(cs300);

			/*********************************************************
			* Create CIB
			*********************************************************/
			if (cib_ptr)
			{
				if ((auth & STAT_AUTH_MASK) == CS_NO_ERROR)
				{
					/**********************************************************
					* create CIB according to both NNCs. Do not submit it until
					* the challenge number is checked in the Confirm message
					***********************************************************/
					create_cib(index, pdu, auth, far_end_auth, um_other_sdu);
				}
				else
				{
					/**********************************************************
					* Authentication failed. Create a CIB to block the traffic.
					* Pending connection entry has already been released.
					* Far end will not send back message.
					***********************************************************/
					cib_ptr->pvector = 0;
					(cib_ptr->status) &= (~OPEN_NODE); /*block traffic */
					time_conn_fast(cib_ptr);
					release_pend_cib(cib_ptr);
				}
			}
		}
		else
		{
			taskReleaseBuffer(fch->descLogP);
			nnc_grp = get_nnc(fc, &auth, &use_new_old) +	/*non-null */
					  NNC_FIX_GRP_OFFSET + dssPkeyLen;
			Error_proc(fc->epEntry.info.cs.cib,
					   nnc_grp,
					   CS_ERR_NO_PCONN_ENTRY,
					   &cs_err_no_pconn_entry,
					   &(fc->epEntry.pe),
					   &(fc->epEntry.upe));
		}
	}
	else
	{
		nnc_grp = get_nnc(fc, &auth, &use_new_old) +		/*non-null */
				  NNC_FIX_GRP_OFFSET + dssPkeyLen;
		Error_proc(fc->epEntry.info.cs.cib,
				   nnc_grp,
				   CS_ERR_NO_TX_BUF,
				   &cs_err_no_tx_buf,
				   &(fc->epEntry.pe),
				   &(fc->epEntry.upe));
	}
}

/*******************************************************************************
* NAME:             recv_nnc_rpy
*
* DESCRIPTION:      This functions processes the received NNC Echo Reply
*                   message. It also sends back the NNC Echo Confirm message.
*                   It posts errors if there is any
*
* INPUTS:           fc  - frame control
*                   pdu - point to the received connection setup msg layer 5
*
* OUTPUT:           A NNC Echo Confirm message will be sent back. The pending
*                   connection list entry will be released.
*
*******************************************************************************/
static void recv_nnc_rpy(Pa2EpFrameControl *fc, uchar *pdu)
{
	uchar	recv_challenge[CHALLENGE_SIZE];
	uchar * tx_buf;         /*transmit buffer. point to layer 2. */
#ifndef CISCO
	uchar * l3;             /*point to layer 3 in tx_buf */
	uchar * l4;             /*point to layer 4 in tx_buf */
#endif
	uchar * l5;             /*point to connection setup msg in tx_buf */
	uchar * other_sdu_dss_pub;
	uchar * end_l5;         /*pointer to the byte after the frame */
	CIBptr  cib_ptr;
	Cy2TxFrameControl *fch; /*frame control header */
	ulong	recv_conn_id;
	ushort	recv_nnc_size;
	ushort  pid;            /*protocol ID */
	short   index;          /*my pconn index carried in frame */
#ifdef CISCO
	short   far_end_index = 0;	/*far end's pconn index carried in frame */
#else
	short   far_end_index;	/*far end's pconn index carried in frame */
#endif
	uchar   auth;
	uchar   far_end_auth;
	uchar	auth_sent;
	uchar	um_other_sdu;
#ifdef CISCO
	long				*pdu_long_ptr;
#endif

#ifdef INTGR_DEBUG	 /*TEST ONLY */
	pa_post_cs_count++;
#endif

	/*************************************************************************
	* Sanity check: if bad size, or there is no far SDU but the far PE echoes
	*				back, discard this frame.
	************************************************************************/
#ifndef CISCO
	if (bad_nnc_frm_size(fc, pdu, ECHO_REPLY, &recv_nnc_size) ||
		((*((ulong *)(pdu + PDU_SERIAL_NUM_OFFSET))) ==
		 (*((ulong *)mg_sdu_serial_num))))
		return;
#else
       if (bad_nnc_frm_size(fc, pdu, ECHO_REPLY, &recv_nnc_size) ||
               crypto_glue_is_my_snum((*((ulong *)(pdu + PDU_SERIAL_NUM_OFFSET)
)) ) )
               return;
#endif

	/***********************************************************************
	* Initialization: not granteed a good one since signature not verified
	***********************************************************************/
	GET_PDU_SHORT(index, pdu + PDU_INIT_PCONN_OFFSET);
#ifdef CISCO
	/*
	 * Validate that the index is within valid range for reliability.
	 */
	if (index > PCONN_ENTRIES) {
		DEBUG("Index in connection message too large!");
		return;
    	}
#endif
	recv_conn_id = *((ulong *)(pdu + PDU_INIT_CONNID_OFFSET)); /*comparison only */

	/*pending connection entry could have been released and */
	/*a retransmitted reply message is received */
	if (!pconn_exist(index, recv_conn_id, PDU_INIT_CONNID_OFFSET)) {
		DEBUG("No pending connection found for connection message");
		return;
	}

	auth_sent = (pconn_list[index].retx_buf)
				[(pconn_list[index].l5_offset) + PDU_STAT_OFFSET];
	far_end_auth = pdu[PDU_STAT_OFFSET];

#ifdef CISCO
	/*
	 * This is to aid in debugging
	 */
	if ((far_end_auth & STAT_AUTH_MASK) != CS_NO_ERROR) {
		Print_status_error(far_end_auth);
	}

	/*
	 * If we don't catch the far end authentication error here, it seems that
	 * a missing public key failure reported by the peer is ignored and we 
	 * continue setting up a connection assuming that they are participating 
	 * when in fact they can't authenticate us. 
	 *
	 * It seems pointless to continue since he'll continue to not be able to
	 * authenticate us. 
	 */
	if (far_end_auth & AUTH_SDU_SIG_FAILED)
	{
		free_pconn(index);
		return;
	}
#endif /* CISCO */

	auth = CS_NO_ERROR;
	um_other_sdu = check_unmanaged_sdu(&other_sdu_dss_pub, pdu,
									   pconn_list[index].cib);

#ifdef CISCO
	/*
	 * If we couldn't get a public number, we'd better let the other
	 * side know.
	 */
	if ((um_other_sdu == 1) && (other_sdu_dss_pub == NULL)) {	
		auth |= AUTH_SDU_SIG_FAILED;
		DEBUG("Peer public key not found");
	}
#endif

	/********************************************************
	* Authentication check
	********************************************************/
	/*save the XOR'ed challenge number so we can sent it back */
	memcpy(recv_challenge, pdu + PDU_CHAL_OFFSET, CHALLENGE_SIZE);

	/*generate DH share number and restore the XOR'ed challenge number */
	genDHshareXorChallenge(fc, pdu, &auth, index, auth_sent, far_end_auth);
	/*if the message signature has been generated */
	if (!(far_end_auth & STAT_GEN_SIG_FAILED) && other_sdu_dss_pub)
	{
		auth |= check_frame_sig(index,
								pdu,
								other_sdu_dss_pub,
								recv_nnc_size);
	}

	/*if the far end is not using the dummy NNC with system flag and */
	/*"SM_DSS_PUBLIC_NEW or OLD" has been previously defined and saved */
	/*in pending connection entry */
	if (!(far_end_auth & STAT_SYS_OC_FLAG) &&
		!(auth_sent & STAT_SYS_OC_FLAG) &&
		!um_other_sdu)
	{
		auth |= check_nnc_sig(	pdu,
								index,
								pconn_list[index].sm_dss_pub_new_old,
								recv_nnc_size,
								*(ulong *)(pconn_list[index].nnc + NNC_CUS_ID_OFFSET));
	}

	/*verify challenge number */
	if ((auth & STAT_AUTH_MASK) == CS_NO_ERROR)
	{
		if (good_challenge_odd(pdu, index))
		{   /*we can trust all data carried in frame */
			GET_PDU_SHORT(far_end_index, pdu + PDU_RESP_PCONN_OFFSET);
		}
		else
		{
			/*challenge failed! */
			auth |= AUTH_CHALLENGE_FAILED;
#ifdef CISCO
			DEBUG("Challenge verification failed!");
#endif
		}
	}

	/*If authentication fails, search pending connection list to re-compute */
	/*some previously used variables(which were not garanteed then). */
	if ((auth & STAT_AUTH_MASK) != CS_NO_ERROR)
	{
		index = srch_pconn_index(recv_conn_id, PDU_INIT_CONNID_OFFSET);
		far_end_index = UNDEF_INDEX;
		if (index != NULL_INDEX)
		{
			auth_sent = (pconn_list[index].retx_buf)
						[(pconn_list[index].l5_offset) + PDU_STAT_OFFSET];
		}
	}

	/********************************************
	* Build Confirm frame and send back
	*********************************************/
#ifdef CISCO
	/*
	 * Save the peer network address in dst for use in sending out the packet.
	 * (Don't need to save it in the pconn struct since CCM is never
	 * re-transmitted.) Also generate the src while we're at it ... isn't done
	 * when building the CCM.
	 */
	pdu_long_ptr = (long *) (pdu + PDU_NW_ADDR_OFFSET);
	fc->epEntry.cisco.dst = *pdu_long_ptr;
	crypto_glue_get_network_address(&fc->epEntry.cisco.src,
									fc->epEntry.cisco.dst);
#endif
	if (index != NULL_INDEX)
	{
		cib_ptr = pconn_list[index].cib;    /*same as in frame control, non-null */

		if (far_end_index != NULL_INDEX)    /*far end has a pending connection entry */
		{
			fch = taskAcquireBuffer();
			if (fch)
			{
#ifdef CISCO
				/*
			 	 * Bypass usage of l2, l3 & l4.
			 	 */
				tx_buf = fch->frameP;
				l5 = tx_buf;
				pid = fc->epEntry.pe.pid;
#else
				/* Get different layer pointers */
				tx_buf = fch->frameP;
				pid = fc->epEntry.pe.pid;
				l3 = tx_buf + (int)(fc->networkLink - fc->frameLink);
				l4 = l3 + (int)(fc->transportLink - fc->networkLink);
				l5 = l4 + l4_header_size[pid - INTERNET_IP];
#endif

				/* Build the frame and send it */
				build_cs_msg_cfm(l5, &auth, pdu, &end_l5, index, recv_challenge,
								 fc);
#ifndef CISCO
				build_cs_msg_cfm(l5, &auth, pdu, &end_l5, index, recv_challenge);
				memcpy(tx_buf, fc->frameLink, (int)(l4 - tx_buf));   /*copy L2,L3 */
				build_cs_frm_l2_addr(tx_buf, fc->macType);
				build_cs_frm_l3_addr(l3, pid, pdu);

				(*init_l3_l4_header[pid - INTERNET_IP]) (l3, l4, end_l5,
														 INIT_ICMP_ECHO_REQ);

				if (fc->macType == IEEE8023) ADJ_LLC_LENGTH(tx_buf, end_l5);
#else /* CISCO */
				/*
				 * Mark the packet we're about 
				 * to send as being sent the first time (that is, not
				 * retransmitted). Copy the epEntry stucture so we that the
				 * needed info for the IOS to send out the message.
				 */
	        	cisco = (cisco_FrameControlHeader *)fch->otherP;
				cisco->retransmit = FALSE;
				cisco->epEntry = fc->epEntry; /* struct copy */
#endif

				cs_transmit_frm(fch,
								(ushort)(end_l5 - tx_buf),
								&pconn_list[index].pe,
								&pconn_list[index].upe,
								fc->fch.txPortP,
								FRAME_REFLECT);

				DEBUG(cs400);
			}
			else
			{
				Error_proc(0,
						   0,
						   CS_ERR_NO_TX_BUF,
						   &cs_err_no_tx_buf,
						   &pconn_list[index].pe,
						   &pconn_list[index].upe);
			}
		}

		/***********************************************************
		* Create CIB
		***********************************************************/
		if (((auth & STAT_AUTH_MASK) == CS_NO_ERROR) &&
			((far_end_auth & STAT_AUTH_MASK) == CS_NO_ERROR))
		{
			/* create CIB according to both NNCs. */
			create_cib(index, pdu, auth_sent | auth, far_end_auth, um_other_sdu);
#ifdef CISCO
			crypto_glue_xchg_connection_ids(cib_ptr, FALSE, 0);
#endif /* CISCO */
		}
		else
		{
			/* Authentication failed. Create a CIB to block the traffic. */
			cib_ptr->pvector = 0;
			(cib_ptr->status) &= (~OPEN_NODE); /*block traffic */
			time_conn_fast(cib_ptr);
		}

		free_pconn(index);
		release_pend_cib(cib_ptr);
	}
	else
	{
		/*either an error occurs due to the lost pending connection entry */
		/*or the Reply frame is a retransmission and its entry has been released */
		Error_proc(0,0,CS_ERR_PCONN_INDEX_LOST,0,0,0);	/*a possible error */
	}
}

/*******************************************************************************
* NAME:             recv_nnc_cfm
*
* DESCRIPTION:      This functions processes the received NNC Echo Confirm
*                   message. It posts errors if there is any, otherwise it
*                   performs the NNC matching algorithm to create CIB.
*
* INPUTS:           fc  - frame control
*                   pdu - pointer to the connection setup message layer 5
*
* OUTPUT:           error messages or CIB created.
*
*******************************************************************************/
static void recv_nnc_cfm(Pa2EpFrameControl *fc, uchar *pdu)
{
	CIBptr  cib_ptr;
	ulong	recv_conn_id;
	ushort	recv_nnc_size;
	short   index;          /*pending connection list index carried in frame */
	uchar   auth;
	uchar   far_end_auth;
	uchar	auth_sent;

#ifdef INTGR_DEBUG	/*TEST ONLY */
	pa_post_cs_count++;
#endif

	if (bad_nnc_frm_size(fc, pdu, ECHO_CONFIRM, &recv_nnc_size))
		return; /*discard this frame */

	/*********************************************************************
	* Init - not garanteed a good one since signature is not checked yet
	*********************************************************************/
	GET_PDU_SHORT(index, pdu + PDU_CFM_RPC_OFFSET);
#ifdef CISCO
	/*
	 * Validate that the index is within valid range for reliability.
	 */
	if (index > PCONN_ENTRIES) {
		DEBUG("Index in connection message too large!");
		return;
    	}
#endif
	recv_conn_id = *((ulong *)(pdu + PDU_CFM_RP_CID_OFFSET)); /*comparison only */

	/*pending connection entry could be released and */
	/*the received frame is a retransmitted confirm message. */
	if (!pconn_exist(index, recv_conn_id, PDU_RESP_CONNID_OFFSET))
		return;

	far_end_auth = pdu[PDU_STAT_OFFSET];
	auth = CS_NO_ERROR;
	auth_sent = (pconn_list[index].retx_buf)
				[(pconn_list[index].l5_offset) + PDU_STAT_OFFSET];
#ifdef CISCO
	/*
	 * For debugging.
	 */
	if ((far_end_auth & STAT_AUTH_MASK) != CS_NO_ERROR) {
		Print_status_error(far_end_auth);
	}
	/*
	 * If we don't catch the far end authentication error here, it seems that
	 * a missing public key failure reported by the peer is ignored and we 
	 * continue setting up a connection assuming that they are participating
	 * when in fact they can't authenticate us. 
	 */
	if (far_end_auth & AUTH_SDU_SIG_FAILED)
	{
		free_pconn(index);
		return;
	}
#endif /* CISCO */

	/********************************************************
	* Authentication check
	********************************************************/
	/*Restore the XOR'ed challenge number. */
	/*Since far SDU only copies the challenge number from Reply to Confirm, */
	/*	whether challenge has been XOR'ed depends on if I have XOR'ed the */
	/*	challenge in Reply */
	if (!(auth_sent & STAT_GEN_DH_FAILED))
	{
#ifndef CISCO
		if (!hMemSyndrome(pconn_list[index].hi_dh_shared,
						  SYNDROME_HMA_OFFSET,
						  dhPublicLen,
						  CHALLENGE_SIZE,
						  syndrome))
#endif
		if (crypto_glue_generate_syndrome(
			fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id,
			fc->epEntry.info.cs.cib->crypto.cisco.slot,
			SYNDROME_HMA_OFFSET,
			CHALLENGE_SIZE,
			syndrome))
		{
			/*I have XOR'ed it in Reply. Restore the challenge number */
			xor_challenge(pdu + PDU_CFM_CHAL_OFFSET);
		}
		else
		{
			Error_proc(	0,
						0,
						CS_ERR_GEN_DH,
						&cs_err_gen_dh,
						&(fc->epEntry.pe),
						&(fc->epEntry.upe));
			auth |= STAT_GEN_DH_FAILED;
		}
  }
	/*if the message signature has been generated AND DSS public exists */
	if (!(far_end_auth & STAT_GEN_SIG_FAILED) &&
		memcmp(	pconn_list[index].other_dss_pub,
				nnc_sys_oc + NNC_FIX_GRP_OFFSET,
				dssPkeyLen))
	{
		auth |= check_frame_sig(index,
					pdu,
					pconn_list[index].other_dss_pub,   /*Other SDU's DSS public num */
					recv_nnc_size);
	}

	/*verify challenge number */
	if ((auth & STAT_AUTH_MASK) == CS_NO_ERROR)
	{
		if (!good_challenge_cmp(pdu, index))
		{
			auth |= AUTH_CHALLENGE_FAILED;
#ifdef CISCO
			DEBUG("Challenge verification failed!");
#endif
		}
	}

	if (((auth & STAT_AUTH_MASK) == CS_NO_ERROR) &&
		((far_end_auth & STAT_AUTH_MASK)== CS_NO_ERROR))
	{
		/***************************************************************
		* Submit the CIB (created previously at receiving Req msg) by
		* clearing the pending bit. Or modify CIB.
		***************************************************************/
		cib_ptr = pconn_list[index].cib;
		if (far_end_auth & STAT_CYOC_MASK)
		{
			/*Modify CIB in case of something wrong at processing Reply */
			cib_ptr->pvector = 0;   /*EP will fall back to use cib_ptr->status */
		}

		free_pconn(index);
		release_pend_cib(cib_ptr);
#ifdef CISCO
		crypto_glue_xchg_connection_ids(cib_ptr, FALSE, 0);
#endif /* CISCO */
	}
	else
	{
		/*****************************************************************
		* Authentication failed. Search for the pending connection index
		* and create a blocking CIB.
		*****************************************************************/
		index = srch_pconn_index(recv_conn_id, PDU_RESP_CONNID_OFFSET);
		if (index != NULL_INDEX)
		{
			cib_ptr = pconn_list[index].cib;
			cib_ptr->pvector = 0;              	/*block traffic */
			(cib_ptr->status) &= (~OPEN_NODE);
			time_conn_fast(cib_ptr);

			free_pconn(index);
			release_pend_cib(cib_ptr);
		}
		else
		{
			/*either an error occurs due to the lost pending connection entry */
			/*or the Reply frame is a retransmission and its entry has been released */
			Error_proc(0,0,CS_ERR_PCONN_INDEX_LOST, 0, 0, 0); /*a possible error */
		}
	}
}

/**************************************************************************
* NAME:			sdu_has_no_nw_addr
*
* DESCRYPTION:	This function checks if SDU has a network address for the
*				current protocol that it is processing.
*
* INPUTS:		pid - current protocol's id
*
* RETURNS:		1 - if SDU has no network address; 0 - if it has one
**************************************************************************/
static int sdu_has_no_nw_addr(ushort pid)
{

	switch (pid)
	{
		case INTERNET_IP:
#ifdef CISCO
			/*
			 * We don't want to set sys_sdu_nw_addr_ip to a fake IP address
			 * or pick a real IP address from one interface, because this
			 * variable is used elsewhere in the SDU code and there may be
			 * side affects later. What we really want to know here is
			 * whether IP is enabled or not.
			 *
			 */
			 return  0;
#else
			if ((*((ulong *)sys_sdu_nw_addr_ip)) != 0L)		return 0;
#endif /* !CISCO */
			break;

		case XEROX_IDP:
			if ((*((ulong *)sys_sdu_nw_addr_idp)) != 0L)	return 0;
			break;

		case NOVELL_IPX:
			if ((*((ulong *)sys_sdu_nw_addr_ipx)) != 0L)	return 0;
			break;

		case APPLE_DDP:
			if ((*((ushort *)sys_sdu_nw_addr_ddp)) != 0)	return 0;
			break;

		default:
			break;
	}

#ifdef INTGR_DEBUG	/*TEST ONLY */
	cs_debug |= CS_TEST_NO_NW_ADDR;
#endif
	return 1;
}

/***************************************************************************
* NAME:			check_unmanaged_sdu
*
* DESCRYPTION:	This function checks if the received NNC is from a unmanaged
*				SDU.
*
* INPUTS:		pdu - pointer to the received NNC Req or Reply message
*
* OUTPUTS:		other_sdu_dss_pub - other SDU's DSS public number
*
* RETURN:		1 if the received NNC is from a unmanaged SDU. 0 otherwise.
*
***************************************************************************/
static uchar check_unmanaged_sdu(uchar **other_sdu_dss_pub, uchar *pdu,
								 CIBptr cib)
{
	uchar	*p_dss_public;

	if (!(pdu[PDU_STAT_OFFSET] & STAT_SYS_OC_FLAG)) /*far end does not use the */
	{                                               /*dummy system NNC */
		p_dss_public = pdu + PDU_FIX_NNC_OFFSET + dhPublicLen + NNC_FIX_GRP_OFFSET;
		if (memcmp(p_dss_public, nnc_sys_oc + NNC_FIX_GRP_OFFSET, dssPkeyLen))
		{
			*other_sdu_dss_pub = p_dss_public;	/*non-zero */
			return 0;
		}
		else
		{	/*If other SDU has a zeroed DSS public number, it is a unmanaged SDU */
			*other_sdu_dss_pub = umSduGetDssPublic(pdu + PDU_NW_ADDR_OFFSET,
									pdu + PDU_SERIAL_NUM_OFFSET, cib);
			return 1;
		}
	}
	else
	{
		*other_sdu_dss_pub = NULL;
		return 0;
	}
}

/*------------------------- END OF FILE ------------------------------ */
