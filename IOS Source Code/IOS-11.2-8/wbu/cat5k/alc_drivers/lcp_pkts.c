/* $Id: lcp_pkts.c,v 1.1.4.1 1996/05/09 14:54:57 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/lcp_pkts.c,v $
 *------------------------------------------------------------------
 * lcp_pkts.c -- Synergy ATM line-card LCP driver message support.
 *
 * March 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcp_pkts.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:57  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:11  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:29  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:24  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:53  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3  1995/06/09  08:18:01  rlowe
 * Integrate to latest LCP driver (message model, startup & HW handshakes).
 * Set local default MAC as 0:2:3:4:5:6 for testing.
 *
 * Revision 1.2  1995/03/29  08:16:47  rlowe
 * Archive current LCP driver as integration continues.
 *
 * Revision 1.1  1995/03/24  06:28:07  rlowe
 * For LCP driver init to sync on Reset Ack message.  Allow mechanism for
 * deferred/late sync.  Provide APIs to request slot number/MAC address
 * from LCP driver, as reported by LCP at init time.  Provide utility
 * routine to format LCP message headers.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "acp_basic_types.h"
#include "syn_ser_eeprom.h"
#include "scp_defs.h"
#include "scp_msgs.h"
#include "lcp_if.h"
#include "lcp_defs.h"


/* --------------------------------------------------------------------	*/
/*	This routine formats a LCP/SCP message header.			*/
/*	Messages can be sent to either the local LCP, or the NMP.	*/
/*	If the given header pointer is null, a buffer with the proper	*/
/*	length is allocated (if possible).  NULL may be returned in	*/
/*	the worst case, if no buffer is available or given.		*/
/* --------------------------------------------------------------------	*/
tLCP_MsgBufU * LCPP_FmtHeader (tLCP_MsgBufU *	pBuf,
				u_int8		fLocal,
				u_int8		MsgLen,
				u_int8		DstSap,
				u_int8		SrcSap,
				u_int8		MsgOpCode)
{
	register tLCP_MsgHdr *	pHdr ;


		/* Message (payload) size limited by max physical MTU size. */
	if (MsgLen > LCP_MAX_DATA_SIZ)
		MsgLen = LCP_MAX_DATA_SIZ ;

	if (pBuf == NULL)
	{
		pBuf = NULL ;	/* RKL:  need to fix this !! */
	}

	if (pBuf != NULL)
	{
		pHdr = (tLCP_MsgHdr *) & pBuf->msg.addr ;

			/* Src is always our slot, from startup LCP Ready. */
			/* Dst could be local LCP or NMP.                  */
		pHdr->src_addr	= LCP_SLOTNUM (LCP_PhysSlotNbr ()) ;
		pHdr->dest_addr	= (fLocal ? pHdr->src_addr :
					    (LCP_SLOTNUM (LCP_NMP_PHYS_SLOT))) ;
		pHdr->length	= MsgLen ;
		pHdr->dsap	= DstSap ;
#ifdef	ACP_REV0_LCP
			/* Src SAP always flagged to indicate the ACP. */
		pHdr->ssap	= (SrcSap | (LCP_ACP_SAP_FLAG)) ;
#else
		pHdr->ssap	= SrcSap ;
#endif
		pHdr->opcode	= MsgOpCode ;
	}

	return (pBuf) ;

}	/* LCPP_FmtHeader */

