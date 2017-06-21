/* $Id: csutil1.c,v 1.1.68.7 1996/09/11 23:23:09 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/task/cs/csutil1.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: csutil1.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: csutil1.c,v $
 * Revision 1.1.68.7  1996/09/11  23:23:09  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 1.1.68.6  1996/08/16  05:58:15  bew
 * CSCdi64252:  4500 crashes randomly after a long time during connection
 * setup
 * Branch: California_branch
 * Re-implement manner of finding pending crypto connections using
 * existing SDU structures.
 *
 * Revision 1.1.68.5  1996/07/06  19:45:34  xliu
 * CSCdi61999:  decryption packet lost after connection already establish
 * Branch: California_branch
 *
 * Revision 1.1.68.4  1996/07/01  09:07:12  xliu
 * CSCdi61744:  fail to select correct crypto maps when multiple are
 * created
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/06/17  22:25:06  bew
 * CSCdi60484:  error_proc multiply defined in APPN and Cylink crypto code
 * Branch: California_branch
 * Change error_proc() to Error_proc() in Cylink code.
 *
 * Revision 1.1.68.2  1996/06/07  00:27:41  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Cleanup to let compile with -Wall.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:58  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.7  1996/04/16  21:54:11  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 1.1.2.6  1996/03/25  18:44:03  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 1.1.2.5  1996/03/13  22:45:35  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 1.1.2.4  1996/01/18  23:23:41  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 1.1.2.3  1995/12/20  23:32:07  bew
 * Branch: IosSec_branch
 * Context is now freed after a sign operation. Before the callback is
 * called a pointer to the pak is taken so it can be used after the
 * callback returns.
 *
 * Revision 1.1.2.2  1995/12/12  23:12:28  bew
 * Branch: IosSec_branch
 * Better formatting of debug messages.
 *
 * Revision 1.1.2.1  1995/11/28  01:50:01  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:27  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Task/CS/CSUTIL1.C 39    7/19/95 5:49p Tchi 
*
* DESCRIPTION:	This file has all utility functions used by the Connection
*               setup module.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: CSUTIL1.C 
* 
* *****************  Version 39  *****************
* User: Tchi         Date: 7/19/95    Time: 5:49p
* Updated in $/SecureLAN/SDU/Task/CS
* alter the challenge number after signing the message
* 
* *****************  Version 38  *****************
* User: Tchi         Date: 6/12/95    Time: 12:09p
* Updated in $/SecureLAN/SDU/Task/CS
* added the dummy function umSduGetDssPublic for now
* 
* *****************  Version 37  *****************
* User: Tchi         Date: 5/26/95    Time: 9:59a
* Updated in $/SecureLAN/SDU/Task/CS
* changed BLK_READ | BLK_WRITE to BLK_NOACCESS
* 
* *****************  Version 36  *****************
* User: Sduhost      Date: 5/14/95    Time: 3:01p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed so that get_nnc_size is called only once on the received NNC
* 
* *****************  Version 35  *****************
* User: Sduhost      Date: 5/01/95    Time: 7:10p
* Updated in $/SecureLAN/SDU/Task/CS
* added more debugging information
* 
* *****************  Version 34  *****************
* User: Tchi         Date: 4/25/95    Time: 7:26p
* Updated in $/SecureLAN/SDU/Task/CS
* check far_end_auth in function "handle_nnc_req_revisions"
* 
* *****************  Version 33  *****************
* User: Tchi         Date: 4/20/95    Time: 7:48p
* Updated in $/SecureLAN/SDU/Task/CS
* config data resouce locking; csAvail to check if CS can start
* 
* *****************  Version 32  *****************
* User: Sduhost      Date: 4/17/95    Time: 6:23p
* Updated in $/SecureLAN/SDU/Task/CS
* added "adj_active_conn_count"
* 
* *****************  Version 31  *****************
* User: Tchi         Date: 4/17/95    Time: 5:22p
* Updated in $/SecureLAN/SDU/Task/CS
* CS_ERR_AUTH_API is not used anymore
* 
* *****************  Version 30  *****************
* User: Sduhost      Date: 4/17/95    Time: 12:18p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed a bug in Error_proc.
* 
* *****************  Version 29  *****************
* User: Sduhost      Date: 4/12/95    Time: 5:37p
* Updated in $/SecureLAN/SDU/Task/CS
* "total length" field should include IP header length
* 
* *****************  Version 28  *****************
* User: Sduhost      Date: 4/07/95    Time: 3:57p
* Updated in $/SecureLAN/SDU/Task/CS
* clear XCHG_KEY bit when returning a CIB
* 
* *****************  Version 27  *****************
* User: Tchi         Date: 3/27/95    Time: 4:54p
* Updated in $/SecureLAN/SDU/Task/CS
* replace KS_inqtime with GetRTClock; the logic to check error code
* returned by the latter will be done later
* 
* *****************  Version 26  *****************
* User: Tchi         Date: 3/22/95    Time: 3:04p
* Updated in $/SecureLAN/SDU/Task/CS
* NNC exchange will not start if SDU's network address is not available
* for the current protocol
* 
* *****************  Version 25  *****************
* User: Tchi         Date: 3/16/95    Time: 11:42a
* Updated in $/SecureLAN/SDU/Task/CS
* IO manager has the responsibility to watch out NOT to be flooded with
* events.
* 
* *****************  Version 24  *****************
* User: Tchi         Date: 2/23/95    Time: 2:03p
* Updated in $/SecureLAN/SDU/Task/CS
* fix the NNC frame signature signing and verification
* 
* *****************  Version 23  *****************
* User: Sduhost      Date: 2/23/95    Time: 11:11a
* Updated in $/SecureLAN/SDU/Task/CS
* bug fix
* 
* *****************  Version 22  *****************
* User: Tchi         Date: 2/16/95    Time: 6:58p
* Updated in $/SecureLAN/SDU/Task/CS
* changed the addresses of the last two NNC msg
* 
* *****************  Version 21  *****************
* User: Tchi         Date: 2/13/95    Time: 6:58p
* Updated in $/SecureLAN/SDU/Task/CS
* error handling for IO MGR
* 
* *****************  Version 20  *****************
* User: Tchi         Date: 2/13/95    Time: 9:06a
* Updated in $/SecureLAN/SDU/Task/CS
* error event IDs start from 1
* 
* *****************  Version 19  *****************
* User: Tchi         Date: 2/12/95    Time: 5:50p
* Updated in $/SecureLAN/SDU/Task/CS
* error handling for IO Mgr
* 
* *****************  Version 18  *****************
* User: Bald_eagle   Date: 2/09/95    Time: 5:05p
* Updated in $/SecureLAN/SDU/Task/CS
* intgr2a
* 
* *****************  Version 17  *****************
* User: Tchi         Date: 2/07/95    Time: 6:57p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed a problem that when CIB is reused, the open-close flag in the
* status field is not cleared to zero.
* 
* *****************  Version 16  *****************
* User: Tchi         Date: 2/04/95    Time: 9:16a
* Updated in $/SecureLAN/SDU/Task/CS
* CS to Authentication interface
* 
* *****************  Version 15  *****************
* User: Tchi         Date: 1/20/95    Time: 5:40p
* Updated in $/SecureLAN/SDU/Task/CS
* intgra 2a
* 
* *****************  Version 14  *****************
* User: Tchi         Date: 1/19/95    Time: 6:39p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed csTransmit
* 
* *****************  Version 13  *****************
* User: Tchi         Date: 1/13/95    Time: 10:39a
* Updated in $/SecureLAN/SDU/Task/CS
* changed to ep_frm.h
* 
* *****************  Version 12  *****************
* User: Tchi         Date: 12/19/94   Time: 7:24p
* Updated in $/SecureLAN/SDU/Task/CS
* MIB changes
* 
* *****************  Version 11  *****************
* User: Sduhost      Date: 3/16/94    Time: 3:23p
* Updated in $/SecureLAN/SDU/Task/CS
* Final Update for Integration 1C+.
* 
*
* *****************  Version 10  *****************
* User: Tchi         Date: 3/15/94    Time: 5:31p
* Updated in $/SecureLAN/SDU/Task/CS
* previous version was wrong
*
* *****************  Version 8  *****************
* User: Tchi         Date: 11/11/94   Time: 4:25p
* Updated in $/SecureLAN/SDU/Task/CS
* retransmission, bypass NNC frame from different product,...
* 
* *****************  Version 7  *****************
* User: Tchi         Date: 11/08/94   Time: 6:08p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed IP checksum (reversed); fixed TTL to use max.
* 
* *****************  Version 6  *****************
* User: Tchi         Date: 11/08/94   Time: 4:09p
* Updated in $/SecureLAN/SDU/Task/CS
* added DSS
* 
* *****************  Version 5  *****************
* User: Tchi         Date: 10/31/94   Time: 6:46p
* Updated in $/SecureLAN/SDU/Task/CS
* fixed free_pconn
* 
* *****************  Version 4  *****************
* User: Tchi         Date: 10/31/94   Time: 9:34a
* Updated in $/SecureLAN/SDU/Task/CS
* use macro SET_PDU_USHORT
*
* *****************  Version 3  *****************
* User: Tchi         Date: 10/28/94   Time: 5:32p
* Updated in $/SecureLAN/SDU/Task/CS
* Integration 1.a bug fix
*
* *****************  Version 2  *****************
* User: Tchi         Date: 10/27/94   Time: 6:48p
* Updated in $/SecureLAN/SDU/Task/CS
* 
* *****************  Version 1  *****************
* User: Tchi         Date: 10/25/94   Time: 8:50p
* Created in $/SecureLAN/SDU/Task/CS
* created
*******************************************************************************/

#define CSUTIL1_C

/*******************************************************************************
* INCLUDE FILES
*******************************************************************************/
#include "../crypto/sdu/include/cylink.h"
#ifndef CISCO
#include <mem.h>
#endif
#include "../crypto/sdu/include/connstup.h"
#include "../crypto/sdu/include/csutil1.h"
#include "../crypto/sdu/include/csutil2.h"
#include "../crypto/sdu/include/ep_frm.h"
#include "../crypto/sdu/include/pa/pactl.h"
#ifndef CISCO
#include "../crypto/sdu/include/rtxcapi.h"
#endif
#include "../crypto/sdu/include/csdata.h"
#ifndef CISCO
#include "../crypto/sdu/include/phybufm.h"
#endif
#include "../crypto/sdu/include/net.h"
#include "../crypto/sdu/include/mgdata.h"
#include "../crypto/sdu/include/cy_api.h"
#include "../crypto/sdu/include/auth/authext.h"
#include "../crypto/sdu/include/iomgr.h"
#include "../crypto/sdu/include/rtxcinfo.h"

#ifdef CISCO
#include "address.h"
#include "../crypto/crypto_glue_sdu.h"
#include "../crypto/sdu/include/debug.h"
#include "../crypto/crypto_map.h"
#include "../crypto/crypto_sesmgmt.h"
#include "../os/free.h"
#endif

/*******************************************************************************
* PUBLIC DECLARATIONS (prototypes)
*******************************************************************************/
void 	build_ip_header(uchar *, uchar *, ushort, uchar);
void    swap_l2_da_sa(uchar *, ulong);
void    swap_l3_da_sa(uchar *, ushort);
short   alloc_pconn(Pa2EpFrameControl *);
void    free_pconn(short);
void    Error_proc(CIBptr, uchar *, ulong, entity_pair *, EntityID *, EntityID *);
short   srch_pconn_index(ulong, ushort);
ushort  get_ip_checksum(ushort *, int);
#ifdef CISCO
void    build_cs_msg_req(Pa2EpFrameControl *, uchar *, short, uchar **,
						 uchar *, uchar, ulong);
void    build_cs_msg_rpy(Pa2EpFrameControl *, uchar *, uchar *,
						 uchar *, uchar *, uchar **, short, ushort, ulong);
void    build_cs_msg_cfm(uchar *, uchar *, uchar *, uchar **, short, uchar *,
						 Pa2EpFrameControl *);
#else
void    build_cs_msg_req(Pa2EpFrameControl *, uchar *, short, uchar **,
						 uchar *, uchar);
void    build_cs_msg_rpy(Pa2EpFrameControl *, uchar *, uchar *,
void    build_cs_msg_cfm(uchar *, uchar *, uchar *, uchar **, short, uchar *);
						 uchar *, uchar *, uchar **, short, ushort);
#endif
uchar   handle_nnc_req_revisions(uchar *, Pa2EpFrameControl *,
								 int *, uchar **, uchar);
void    init_l3_l4_ip(uchar *, uchar *, uchar *, uchar);
void    init_l3_l4_idp_ipx(uchar *, uchar *, uchar *, uchar);
void    init_l3_l4_ddp(uchar *, uchar *, uchar *, uchar);
int     bad_nnc_frm_size(Pa2EpFrameControl *, uchar *, int, ushort *);
void    cs_transmit_frm(Cy2TxFrameControl *, ushort, EntityID *, EntityID *,
						void *, int);
ushort	get_nnc_size(uchar *);
void    post_err_iomgr(ulong,ulong *,ushort);
void 	build_cs_frm_l2_addr(uchar *, ulong);
void 	build_cs_frm_l3_addr(uchar *, ushort, uchar *);
void 	genDHshareXorChallenge(Pa2EpFrameControl *,	uchar *, uchar *,
							   short, uchar, uchar);
void 	xor_challenge(uchar *);
#ifdef CISCO
uchar   *umSduGetDssPublic(uchar *, uchar *, CIBptr);
#else
uchar   *umSduGetDssPublic(uchar *, uchar *);
#endif

#ifndef CISCO
/*******************************************************************************
* STATIC DECLARATIONS (prototypes)
*******************************************************************************/
static void check_to_swap_llc(uchar *, ulong);
static void set_cs_frm_nw_addr(uchar *, ushort);

#endif /* !CISCO */
/*******************************************************************************
* PUBLIC GLOBALS
*******************************************************************************/
ushort l4_header_size[] =
{
	ICMP_HEADER_SIZE,
    ECHO_HEADER_SIZE,
    ECHO_HEADER_SIZE,
    AEP_HEADER_SIZE
};

#ifndef CISCO
functype2 init_l3_l4_header[] =
{
	init_l3_l4_ip,
	init_l3_l4_idp_ipx,
    init_l3_l4_idp_ipx,
	init_l3_l4_ddp
};

/*******************************************************************************
* STATIC GLOBALS
*******************************************************************************/
static uchar ip_init1_partial[] = {     0x00, 0x00,     /*identification */
										0x40, 0x00 };   /*flags, frag offset */

static uchar ip_init2_partial[] = {     0xFF, 0x01,     /*TTL=max, prtcl=ICMP */
										0x00, 0x00 };   /*clear checksum */

static uchar icmp_init_echo_req[] = {   0x08,           /*type=ICMP Echo Req */
                                        IP_NNC_CODE,    /*code point */
                                        0x00, 0x00,     /*clear checksum */
										0x00, 0x01,     /*identification */
										0x00, 0x01 };   /*sequence number */

static uchar icmp_init_echo_rpy[] = {   0x00,           /*type=ICMP Echo Reply */
										IP_NNC_CODE,    /*code point */
                                        0x00, 0x00,     /*clear checksum */
                                        0x00, 0x01,     /*identification */
                                        0x00, 0x01 };   /*sequence number */
#endif /* !CISCO */
static uchar nnc_req_3b[] = { SECURE_LAN_PRTCL, PS_SDU, 0x00 };

static uchar nnc_rpy_3b[] = { SECURE_LAN_PRTCL, PS_SDU, 0x01 };

static uchar nnc_cfm_3b[] = { SECURE_LAN_PRTCL, PS_SDU, 0x02 };
#ifndef CISCO
/*******************************************************************************
* NAME:             swap_l2_da_sa
*
* DESCRIPTION:		This function swap the layer 2 address in the buffer.
*
* INPUTS:           mac_type - ETHERNET or IEEE802.3
*                   buf - point to MAC.
*
* OUTPUT:			MAC DA and SA are swapped; LLC DSAP and SSAP are swapped.
*
* NOTE:             This function currently only deals with ETHERNET and
*                   IEEE802.3. It needs to be modified in the future releases
*                   for different LANs.
*
*******************************************************************************/
void swap_l2_da_sa(uchar *buf, ulong mac_type)
{
	uchar da[MAC_ADDR_LEN];

	memcpy(da, buf, MAC_ADDR_LEN);	/*save DA */
	memcpy(buf, buf + MAC_ADDR_LEN, MAC_ADDR_LEN);	/*change DA */
	memcpy(buf + MAC_ADDR_LEN, da, MAC_ADDR_LEN);	/*change SA */

	check_to_swap_llc(buf, mac_type);
}

/*******************************************************************************
* NAME:             build_cs_frm_l2_addr
*
* DESCRIPTION:		This function changes the layer 2 DA to be the current SA
*					and uses SDU's address as the SA. It also swaps LLC DSAP
*					and SSAP if present.
*
* INPUTS:           mac_type - ETHERNET or IEEE802.3
*                   buf - point to MAC.
*
* OUTPUT:			see descryption above.
*
*******************************************************************************/
void build_cs_frm_l2_addr(uchar *buf, ulong mac_type)
{
	memcpy(buf, buf + MAC_ADDR_LEN, MAC_ADDR_LEN);		/*change DA */
	memcpy(buf + MAC_ADDR_LEN, systemMacAddress_g, MAC_ADDR_LEN); /*change SA */
	check_to_swap_llc(buf, mac_type);
}

/******************************************************************************
* NAME:			check_to_swap_llc
*
* DESCRYPTION:  This function checks for IEEE802.3. If it is present,
*				it swaps LLC DSAP and SSAP.
*
* INPUTS:           mac_type - ETHERNET or IEEE802.3
*                   buf - point to MAC.
*
* OUTPUT:			see descryption above.
*
*******************************************************************************/
static void check_to_swap_llc(uchar *buf, ulong mac_type)
{
	uchar dsap;

	if ((mac_type == IEEE8023) &&
		(buf[MAC_TO_DSAP_LEN] != 0xFF) && (buf[MAC_TO_SSAP_LEN] != 0xFF))
	{
		/*********************************
		* Swap LLC dsap and ssap
		*********************************/
		dsap = buf[MAC_TO_DSAP_LEN];
		buf[MAC_TO_DSAP_LEN] = buf[MAC_TO_SSAP_LEN];
		buf[MAC_TO_SSAP_LEN] = dsap;
	}
}

/*******************************************************************************
* NAME:             swap_l3_da_sa
*
* DESCRIPTION:		This function swaps network addresses
*
* INPUTS:           layer 3 PDU and layer 3 protocol ID
*
* OUTPUT:           modified layer 3 PDU
*
*******************************************************************************/
void swap_l3_da_sa(uchar *pdu, ushort pid)
{
	uchar   temp1, temp2;
    uchar   temp[12];

	switch (pid)
	{
        case INTERNET_IP:
			/* swap source IP addr and destination IP addr */
            memcpy(temp, pdu + 12, IP_ADDR_LEN);
            memcpy(pdu + 12, pdu + 16, IP_ADDR_LEN);
            memcpy(pdu + 16, temp, IP_ADDR_LEN);
			break;

        case XEROX_IDP:
		case NOVELL_IPX:
            /* swap two blocks of network, node, and socket addresses */
            memcpy(temp, pdu + 6, 12);
            memcpy(pdu + 6, pdu + 18, 12);
            memcpy(pdu + 18, temp, 12);
			break;

        case APPLE_DDP:
            /* swap network numbers */
			temp1 = pdu[4];
            temp2 = pdu[5];
            pdu[4] = pdu[6];
			pdu[5] = pdu[7];
			pdu[6] = temp1;
            pdu[7] = temp2;

            /* swap node addresses */
            temp1 = pdu[8];
            pdu[8] = pdu[9];
			pdu[9] = temp1;

			/* swap socket numbers */
			temp1 = pdu[10];
            pdu[10] = pdu[11];
            pdu[11] = temp1;
            break;

		default:
			break;
	}
}


/*******************************************************************************
* NAME:             build_cs_frm_l3_addr
*
* DESCRIPTION:		This function changes the layer 3 DA to be the current SA
*					and uses SDU's address as the SA.
*
* INPUTS:           buf - layer 3 buffer to change
*					pid - layer 3 protocol ID
* 					pdu - the CS layer in the received frame.
*
* OUTPUT:           modified layer 3 PDU
*
*******************************************************************************/
void build_cs_frm_l3_addr(uchar *buf, ushort pid, uchar *pdu)
{
	uchar	*p_nw_addr;

	switch (pid)
	{
		case INTERNET_IP:
			memcpy(buf + 16, pdu + PDU_NW_ADDR_OFFSET, IP_ADDR_LEN); /*change DA */
			memcpy(buf + 12, sys_sdu_nw_addr_ip, IP_ADDR_LEN);	/*change SA */
			break;

		case XEROX_IDP:
		case NOVELL_IPX:
			p_nw_addr = (pid == XEROX_IDP) ? sys_sdu_nw_addr_idp :
											 sys_sdu_nw_addr_ipx ;
			/*change dest. network, and node. Ignore socket. */
			memcpy(buf + 6, pdu + PDU_NW_ADDR_OFFSET, 12);

			memcpy(buf + 18, p_nw_addr, 4); /*change src. network */
			memcpy(buf + 22, systemMacAddress_g, MAC_ADDR_LEN); /*change src node */
			*((ushort *)(buf + 28)) = 0;	/*change src socket */
			break;

		case APPLE_DDP:
			p_nw_addr = pdu + PDU_NW_ADDR_OFFSET; /*point to other SDU's nw. addr */
			buf[4] = p_nw_addr[0]; /*change dest. network number */
			buf[5] = p_nw_addr[1];
			buf[8] = p_nw_addr[2];	/*change dest. node ID */
			buf[10] = 0;  			/*ignore dest. socket number */

			buf[6] = sys_sdu_nw_addr_ddp[0]; /*change src. network number */
			buf[7] = sys_sdu_nw_addr_ddp[1];
			buf[9] = sys_sdu_ddp_node_id;    /*change src. node ID */
			buf[11] = 0;               		 /*ignore src. socket number */
			break;

		default:
			break;
	}
}
#endif /* !CISCO */
/*******************************************************************************
* NAME:             alloc_pconn
*
* DESCRIPTION:		It allocates a pending connection list entry and returns
*                   the index to this entry.
*
* INPUTS:           pconn_free; current frame control
*
* OUTPUT:           pconn_list, pconn_used, pconn_free
*
* NOTE:             The free list is a singly linked list while the used list
*                   is a doubly linked list
*
*******************************************************************************/
short alloc_pconn(Pa2EpFrameControl *fc)
{
    short   index;

    index = pconn_free;
    if (pconn_free != NULL_INDEX)
    {
#ifndef CISCO
        /*********************************************************
        * Initialize the DH secret number hidden memory index
        *********************************************************/
        if (!hMemAlloc(&(pconn_list[index].hi_dh_secret),
                       dhPublicLen,
                       BLK_NOACCESS))
        {
            if (!hMemAlloc(&(pconn_list[index].hi_dh_shared),
                           dhPublicLen,
                           BLK_NOACCESS))
            {
#endif /* !CISCO */
                /**********************************************
                * Adjust the free list
                **********************************************/
                pconn_free = pconn_list[index].next;
    
                if (pconn_free != NULL_INDEX)
                    pconn_list[pconn_free].prev = NULL_INDEX;
    
                /**********************************************
                * Adjust the used list
                **********************************************/
                pconn_list[index].prev = NULL_INDEX;
                pconn_list[index].next = pconn_used;
                if (pconn_used != NULL_INDEX)
                {
                    pconn_list[pconn_used].prev = index;
                }
                pconn_used = index;
    
                pconn_list[index].tx_count = 0;
                sdu_pconns++;
#ifndef CISCO
            }
            else
            {
                index = NULL_INDEX;
                Error_proc( 0, 0, CS_ERR_ALLOC_HM, &cs_err_alloc_hm,
                            &(fc->epEntry.pe), &(fc->epEntry.upe));
                if (hMemFree(pconn_list[index].hi_dh_secret))
                {
                    /*Crypto failure. Generate Trap here. */
                    Error_proc( 0, 0, CS_ERR_RELEASE_HM, &cs_err_release_hm,
                                &pconn_list[index].pe, &pconn_list[index].upe);
                }
            }
        }
        else
        {
            /*Crypto failure. Generate Trap here. */
            index = NULL_INDEX;
            Error_proc( 0, 0, CS_ERR_ALLOC_HM, &cs_err_alloc_hm,
                        &(fc->epEntry.pe), &(fc->epEntry.upe));
        }
#endif /* !CISCO */
    }

#ifdef CISCO
	/*
	 * Clear out the nnc so we don't try to free garbage later.
	 */
	pconn_list[index].nnc = NULL;
#endif

    return(index);
}


/*******************************************************************************
* NAME:             free_pconn
*
* DESCRIPTION:		It releases an pending connection list entry back to
*                   the free pool.
*
* INPUTS:           The index of the entry to be released.
*
* OUTPUT:           pconn_list, pconn_used, pconn_free
*
* NOTE:             The free list is a singly linked list while the used list
*                   is a doubly linked list
*
*******************************************************************************/
void free_pconn(short index)
{
	short   head;
	short   tail;

	if (index > NULL_INDEX)             /*correct index is non-negative */
	{
#ifdef CISCO
	/*
	 * Free the previously malloc()ed NNC in the unmanaged mode.
	 * However in a low memory situation the nnc pointer could not have been
	 * malloc()ed so a pointer to a static location was returned. (Too much
	 * SDU code assumes that the nnc pointer is non-NULL.)
	 */
    if (pconn_list[index].nnc != NULL) {
		if (mem_lockable(pconn_list[index].nnc)) {
			free(pconn_list[index].nnc);
		}
		pconn_list[index].nnc = NULL; /* For safety */
	}
#else /* !CISCO */
	    /***********************************************************
		* Release the hidden memory for DH secret and share number
		***********************************************************/
		if (hMemFree(pconn_list[index].hi_dh_secret) ||
			hMemFree(pconn_list[index].hi_dh_shared))
		{
			/*Crypto failure. Generate Trap here. */
			Error_proc(0,
					   0,
					   CS_ERR_RELEASE_HM,
					   &cs_err_release_hm,
					   &pconn_list[index].pe,
					   &pconn_list[index].upe);
		}
#endif

		/**********************************************
		* Adjust the used list
		**********************************************/
		head = pconn_list[index].prev;
		tail = pconn_list[index].next;

		if (head != NULL_INDEX)
		{
			pconn_list[head].next = tail;
		}

		if (tail != NULL_INDEX)
		{
			pconn_list[tail].prev = head;
		}

		if (index == pconn_used)    /*this entry is the first one */
		{
			pconn_used = tail;
		}

		/**********************************************
		* Adjust the free list
		**********************************************/
		pconn_list[index].timestamp = 0L;	/*indicating a free entry */
		pconn_list[index].next = pconn_free;

		pconn_list[index].prev = NULL_INDEX;
		pconn_list[pconn_free].prev = index;

		pconn_free = index;

		if (sdu_pconns >= 1) sdu_pconns--;
	}

#ifdef INTGR_DEBUG	/*TEST ONLY */
	else
	{
		cs_debug |= CS_TEST_PCONN_FREE_NULL;
	}
#endif
}

#ifdef CISCO

#define LONGEST_STR_LENGTH		60
#define CS_STR_ERR_NO_TX_BUF		"No transmission buffer"
#define CS_STR_ERR_NO_PCONN_ENTRY	"No pending connection entry"
#define CS_STR_ERR_PCONN_TIMEOUT	"No response after retransmissions"
#define CS_STR_ERR_NNC_EXP			"NNC exceeds the expiration time"
#define CS_STR_ERR_NNC_SIG			"Invalid signature in NNC"
#define CS_STR_ERR_SU_SIG			"Invalid signature in connection message"
#define CS_STR_ERR_PCONN_INDEX_LOST	"Lost pending connection index"
#define CS_STR_ERR_CHALLENGE_ODD	"Invalid challenge number in the CRM"
#define CS_STR_ERR_CHALLENGE_CMP	"Invalid challenge number in the CCM"
#define CS_STR_ERR_NNC_GRPS			"Inconsistent groups in the NNCs compared"
#define CS_STR_ERR_GEN_CHL			"Failed to generate challenge"
#define CS_STR_ERR_GEN_DH			"Failed to generate Diffie-Hellman pair"
#define CS_STR_ERR_GEN_SIG			"Failed to generate signature"
#define CS_STR_ERR_SOK_MASK_RNG		"Inconsistent socket mask tbl and range tbl"
#define CS_STR_ERR_VERIFY_SIG		"Failed in SHA when verifying signature"
#define CS_STR_ERR_NNC_FRM_SIZE		"Invalid connection message frame size"
#define CS_STR_ERR_TRANSMIT			"Connection message not transmitted"
#define CS_STR_ERR_NNC_SIZE			"Invalid NNC size"
#define CS_STR_ERR_UNKNOWN			"Unknown error"

/*******************************************************************************
* NAME:		   		Print_auth_error
*
* DESCRIPTION:		Print out in DEBUG the meaning of an authentication error.
*
*******************************************************************************/
static void Print_auth_error (ulong err_bit)
{
	char str[LONGEST_STR_LENGTH];
	switch (err_bit) {
	case CS_ERR_NO_TX_BUF:
		sstrncpy(str, CS_STR_ERR_NO_TX_BUF, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_NO_PCONN_ENTRY:
		sstrncpy(str, CS_STR_ERR_NO_PCONN_ENTRY, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_PCONN_TIMEOUT:
		sstrncpy(str, CS_STR_ERR_PCONN_TIMEOUT, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_NNC_EXP:
		sstrncpy(str, CS_STR_ERR_NNC_EXP, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_NNC_SIG:
		sstrncpy(str, CS_STR_ERR_NNC_SIG, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_SU_SIG:
		sstrncpy(str, CS_STR_ERR_SU_SIG, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_PCONN_INDEX_LOST:
		sstrncpy(str, CS_STR_ERR_PCONN_INDEX_LOST, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_CHALLENGE_ODD:
		sstrncpy(str, CS_STR_ERR_CHALLENGE_ODD, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_CHALLENGE_CMP:
		sstrncpy(str, CS_STR_ERR_CHALLENGE_CMP, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_NNC_GRPS:
		sstrncpy(str, CS_STR_ERR_NNC_GRPS, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_GEN_CHL:
		sstrncpy(str, CS_STR_ERR_GEN_CHL, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_GEN_DH:
		sstrncpy(str, CS_STR_ERR_GEN_DH, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_GEN_SIG:
		sstrncpy(str, CS_STR_ERR_GEN_SIG, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_SOK_MASK_RNG:
		sstrncpy(str, CS_STR_ERR_SOK_MASK_RNG, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_VERIFY_SIG:
		sstrncpy(str, CS_STR_ERR_VERIFY_SIG, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_NNC_FRM_SIZE:
		sstrncpy(str, CS_STR_ERR_NNC_FRM_SIZE, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_TRANSMIT:
		sstrncpy(str, CS_STR_ERR_TRANSMIT, LONGEST_STR_LENGTH);
		break;
	case CS_ERR_NNC_SIZE:
		sstrncpy(str, CS_STR_ERR_NNC_SIZE, LONGEST_STR_LENGTH);
		break;
	default:
		sstrncpy(str, CS_STR_ERR_UNKNOWN, LONGEST_STR_LENGTH);
		break;
	}
	DEBUG_WITH_ARG("Authentication Error: %s", str);
	return;

}
#endif /* CISCO */


/*******************************************************************************
* NAME:             Error_proc
*
* DESCRIPTION:		This function fills the CIB to pass or reject frames,
*                   sets an bit in the error flag, and record the involved
*                   PE-UPE pair.
*                   NOTICE THAT it does not release pending connection entry.
*
* INPUTS:           open/close flag for the PE, error bit, involved entities,
*
*
* OUTPUT:           CIB, error flag, and recorded entities.
*
*******************************************************************************/
void Error_proc( CIBptr          cib,
				 uchar *         nnc_grp,
                 ulong           err_bit,
				 entity_pair *   err_entities,
				 EntityID *      pe,
				 EntityID *      upe)
{
    bits16  oc_flag;

	/***********************************************************************
	* Fill CIB to either pass or reject (according to open-close
	* flag) if it is not NULL.
	*
	* WARNING: THE CALLING FUNCTION needs to make sure that no functions
	*		   will be called to create CIB after if this function creates
	*		   one.
	**********************************************************************/
	if (cib)
	{
		cib->pvector = 0;

		if (nnc_grp)
		{
			GET_PDU_USHORT(oc_flag, nnc_grp + NNC_GRP_OC_OFFSET);
			if (oc_flag & OPEN_SDU)
			{
				(cib->status) |= OPEN_NODE; 	/*bypass the traffic */
			}
			else
			{
				(cib->status) &= (~OPEN_NODE); 	/*block traffic */
			}
		}
		else /*set status according to system flag (should not happen. just in case) */
		{
			if (ep_ctrl & OPEN_SDU)
			{
				(cib->status) |= OPEN_NODE; 	/*bypass the traffic */
			}
			else
			{
				(cib->status) &= (~OPEN_NODE); 	/*block traffic */
			}
		}

		time_conn_fast(cib);
		release_pend_cib(cib);
	}

	/**********************************************************
	* Set CS error flag and record involved entities
	**********************************************************/
	if (err_bit)
	{
#ifndef CISCO
		post_err_iomgr(err_bit,&cs_err,CONNSTUP);
#else
		Print_auth_error(err_bit);
#endif
	}

	if (err_entities && pe && upe)
	{
		err_entities->pe = *pe;         /*for SM to keep track */
		err_entities->upe = *upe;
	}
	else if (err_entities)	/*could not record the entity pair for this error */
	{
		memset(err_entities, 0, sizeof(entity_pair));
	}
}
#ifndef CISCO
/*******************************************************************************
* NAME:             init_l3_l4_ip
*
* DESCRIPTION:      This function initializes the layer 3 header and build
*                   layer 4 as the Echo frame using a special code point
*                   to carry NNC in the echo data.
*
* INPUTS:           pointers to layer 3 and 4
*
* OUTPUT:           modified layer 3 and 4
*
* NOTE:             The length field in the header will be set only after
*                   the connection message has been built.
*
*******************************************************************************/
void init_l3_l4_ip(uchar * l3, uchar * l4, uchar * end_l5, uchar flag)
{
	ushort      checksum;

	/*********************************************************
	* Build L4 as Echo request or reply
    **********************************************************/
    if (flag == INIT_ICMP_ECHO_REQ)
    {
		memcpy(l4, icmp_init_echo_req, sizeof(icmp_init_echo_req));
    }
	else
    {
        memcpy(l4, icmp_init_echo_rpy, sizeof(icmp_init_echo_rpy));
    }

    /***************************************************************
	*  Compute ICMP checksum: ICMP header + data
    ****************************************************************/
    checksum = get_ip_checksum((ushort *)l4, (int) ((end_l5 - l4) >> 1));
    /*SET_PDU_USHORT(checksum, l4 + 2);  //Venkat said checksum is reversed. */
	*((ushort *)(l4 + 2)) = checksum;

	/******************************************************
	* Build layer 3
	*******************************************************/
	build_ip_header(l3, l4, (ushort)(end_l5 - l4), 1);	/*1=ICMP protocol ID */
}

/***************************************************************************
* NAME:			build_ip_header
*
* DESCRYPTION:	This function initializes various fields in IP header
*
* INPUTS:		l3 - point to IP layer
*				l4 - point to the next layer above IP
*				len - total length of layer 4 and above in bytes
*
* OUTPUTS:		IP header gets built
*
**************************************************************************/
void build_ip_header(uchar *l3, uchar *l4, ushort len, uchar prtcl)
{
	ushort      checksum;

	len += ((ushort)(l4 - l3));	/*"Total Length" field includes IP header */

	/*change the total length field */
	SET_PDU_USHORT(len, l3 + 2);

	/*set "Identification", "Flags", and "Flagment offset". */
	memcpy(l3 + 4, ip_init1_partial, sizeof(ip_init1_partial));

	/*preserve "TTL", set "Protocol" and clear "Header Checksum" */
	memcpy(l3 + 8, ip_init2_partial, sizeof(ip_init2_partial));
    l3[9] = prtcl;

	/* Checksum covers IP header only */
	checksum = get_ip_checksum((ushort *)l3, (int) ((l4 - l3) >> 1));
	/*SET_PDU_USHORT(checksum, l3 + 10);   //Venkat said checksum is reversed. */
	*((ushort *)(l3 + 10)) = checksum;
}

/*******************************************************************************
* NAME:             init_l3_l4_idp_ipx
*
* DESCRIPTION:      see "init_l3_l4_ip"
*
*******************************************************************************/
#pragma argsused
void init_l3_l4_idp_ipx(uchar * l3, uchar * l4, uchar * end_l5, uchar flag)
{
	ushort  len;

    /*********************************************************
    * Build L4 as Echo
    **********************************************************/
	*l4 = 0;
    *(l4 + 1) = IPX_NNC_CODE;

	/*********************************************************
	* Initialize layer 3
    **********************************************************/
    *l3 = 0xFF;     /*turn off checksum */
    *(l3 + 1) = 0xFF;

    /*change the length field */
    len = (ushort)(end_l5 - l3);
    SET_PDU_USHORT(len, l3 + 2);

	*(l3 + 4) = 0;  /*initialize hop count */
    *(l3 + 5) = 2;  /*set pkt type to echo */
}


/*******************************************************************************
* NAME:             init_l3_l4_ddp
*
* DESCRIPTION:      see "init_l3_l4_ip"
*
*******************************************************************************/
#pragma argsused
void init_l3_l4_ddp(uchar * l3, uchar * l4, uchar * end_l5, uchar flag)
{
    ushort  len;

    /*********************************************************
    * Build L4 as Echo
    *********************************************************/
    *l4 = DDP_NNC_CODE;

    /*********************************************************
    * Initialize layer 3
    **********************************************************/
    /*change the length field */
    len = (ushort)(end_l5 - l3);
    SET_PDU_USHORT(len, l3);
    (*l3) &= 0x03;              /*zero MSB 6 bits */

    /*turn off check sum */
    *(l3 + 2) = 0;
    *(l3 + 3) = 0;

    /*change the ddp type to Echo */
    *(l4 - 1) = 4;
}
#endif /* !CISCO */

/*******************************************************************************
* NAME:             build_cs_msg_req
*
* DESCRIPTION:		This function builds the Connectioon Setup Request message.
*
* INPUTS:           frame control:      to get Entity ID
*                   layer 5 pointer:    where the message starts
*                   pconn list index:   to use the challenge number
*
* OUTPUT:           l5_end is set to point to one byte after the frame,
*                   buffer pointed to by l5 is filled with the message
*
*******************************************************************************/
#ifdef CISCO
void build_cs_msg_req(Pa2EpFrameControl *fc, uchar *l5, short index,
					  uchar ** l5_end, uchar *nnc_ptr, uchar auth, 
					  ulong cs_unique_conn_id)
#else
void build_cs_msg_req(Pa2EpFrameControl *fc, uchar *l5, short index,
					  uchar ** l5_end, uchar *nnc_ptr, uchar auth)
#endif
{
	uchar *     buf;
	uchar *     p_status;
	ushort		nnc_size;
        ulong		serial_num;
	/**********************************************************
	* build the message
	***********************************************************/
	buf = l5;

	/*copy the first 3 bytes */
	memcpy(buf, nnc_req_3b, sizeof(nnc_req_3b));
	buf += sizeof(nnc_req_3b);

	/*set the 1-byte status field */
	p_status = buf;
	*buf = auth;
	buf++;

	/*set the 4-byte serial number */
#ifndef CISCO
	memcpy(buf, mg_sdu_serial_num, SERIAL_NUM_SIZE);
#else
        serial_num = crypto_glue_get_sn_by_slot(fc->epEntry.info.cs.cib->crypto.cisco.slot);
	memcpy(buf, &serial_num, SERIAL_NUM_SIZE);
#endif
	buf = buf + SERIAL_NUM_SIZE;

	/*set the 16-byte network address field */
#ifndef CISCO 
	set_cs_frm_nw_addr(buf, fc->epEntry.pe.pid);
#else
	crypto_glue_get_network_address((ulong *)buf, fc->epEntry.cisco.dst);
#endif
	buf += PDU_MAX_NW_ADDR_SIZE;

	/*set the 4-byte initiator's unique connection ID. */
	SET_PDU_ULONG(cs_unique_conn_id, buf);
	buf += sizeof(cs_unique_conn_id);

	/*zero the 4-byte respondor's unique connection ID. */
	*((ulong *)buf) = 0;
	buf += sizeof(cs_unique_conn_id);

	/*set the 2-byte initiator's pending entry index. */
	SET_PDU_USHORT(index, buf);
	buf += PCONN_INDEX_SIZE;

	/*zero the 2-byte respondor's pending entry index. */
	*((ushort *)buf) = 0;
	buf += PCONN_INDEX_SIZE;

	/*generate challenge number */
	gen_challenge_num(buf, NULL, p_status, fc); /*zeros if failed to generate */
	buf += CHALLENGE_SIZE;

	/*generate the DH number: WARNING: THE CALLER HAS TO MAKE SURE index is not */
	/*											  NULL_INDEX */
	gen_DH_pub_secret(buf, pconn_list[index].hi_dh_secret, p_status, fc);
	buf += dhPublicLen;

	/*plug in the NNC */
	nnc_size = get_nnc_size(nnc_ptr);
	memcpy(buf, nnc_ptr, nnc_size);		/*nnc_ptr: non-null */
#ifdef CISCO
	crypto_glue_insert_node_id(buf, &fc->epEntry.pe);
#endif
	buf += nnc_size;

	/*generate the signature */
	gen_signature(buf, p_status, &(fc->epEntry.pe), &(fc->epEntry.upe),
				  l5, auth,
				  nnc_ptr + NNC_FIX_GRP_OFFSET, /*SDU DSS public num */
				  fc->epEntry.info.cs.cib->crypto.cisco.slot);
	buf += DEF_SIG_SIZE;

	/* set the end of layer 5 pointer */
	*l5_end = buf;
}

/*******************************************************************************
* NAME:             build_cs_msg_rpy
*
* DESCRIPTION:		This function builds the connection setup reply message
*
* INPUTS:           frame control:      to get Entity ID
*                   layer 5 pointer:    where the message starts
*                   pconn list index:   to use the challenge number
*                   pdu:                point to where the received Request msg
*                   nnc_to_send:        point to the NNC to be imbeded in frame
*                   authentication flag:
*
* OUTPUT:           l5_end is set to point to one byte after the frame,
*                   buffer pointed to by l5 is filled with the message
*
*******************************************************************************/
#ifdef CISCO
void build_cs_msg_rpy(Pa2EpFrameControl *fc, uchar *l5, uchar *auth,
						 uchar *pdu, uchar *nnc_to_send,
						 uchar ** l5_end, short index, ushort retx_l5_offset,
						 ulong cs_unique_conn_id)
#else
void build_cs_msg_rpy(Pa2EpFrameControl *fc, uchar *l5, uchar *auth,
						 uchar *pdu, uchar *nnc_to_send,
						 uchar ** l5_end, short index, ushort retx_l5_offset)
#endif
{
	uchar *     buf;
	uchar *     p_status;
	uchar *		p_challenge;
	ushort		nnc_size;
	short		hi_dh_secret = 0;
	ulong		serial_num;
#ifndef CISCO
	short		hi_dh_shared = 0;
#endif
#ifdef CISCO
	long *		buf_long;
#endif

	/**********************************************************
	* build the message
	***********************************************************/
	buf = l5;

	/*copy the first 3 bytes */
	memcpy(buf, nnc_rpy_3b, sizeof(nnc_rpy_3b));
	buf += sizeof(nnc_rpy_3b);

	/*set the 1-byte status field */
	p_status = buf;
	*buf = *auth;
	buf++;

	/*set the 4-byte serial number */
#ifndef CISCO
	memcpy(buf, mg_sdu_serial_num, SERIAL_NUM_SIZE);
#else
        serial_num = crypto_glue_get_sn_by_slot(fc->epEntry.info.cs.cib->crypto.cisco.slot);
	memcpy(buf, &serial_num, SERIAL_NUM_SIZE);
#endif
	buf = buf + SERIAL_NUM_SIZE;

#ifndef CISCO 
	set_cs_frm_nw_addr(buf, fc->epEntry.pe.pid);
#else
	crypto_glue_get_network_address((ulong *)buf, fc->epEntry.cisco.dst);
	/* Copy the src into the fc struct for use of sending the packet out */
	buf_long = (ulong *)buf;
	fc->epEntry.cisco.src = *buf_long;
#endif
	buf += PDU_MAX_NW_ADDR_SIZE;

	/*copy the 4-byte initiator's unique connection ID from the received frame */
	memcpy(buf, pdu + PDU_INIT_CONNID_OFFSET, sizeof(cs_unique_conn_id));

	buf += sizeof(cs_unique_conn_id);

	/*set the  4-byte respondor's unique connection ID. */
	SET_PDU_ULONG(cs_unique_conn_id, buf);
	buf += sizeof(cs_unique_conn_id);

	/*copy the 2-byte initiator's pending entry index from the received frame */
	buf[0] = pdu[PDU_INIT_PCONN_OFFSET];
	buf[1] = pdu[PDU_INIT_PCONN_OFFSET + 1];
	buf += PCONN_INDEX_SIZE;

	/*set the 2-byte respondor's pending entry index. */
	SET_PDU_USHORT(index, buf);
	buf += PCONN_INDEX_SIZE;

	/*generate the challenge number */
	gen_challenge_num(buf, pdu + PDU_CHAL_OFFSET,
					  p_status, fc); /*zeros if failed to generate */
	p_challenge = buf;
	buf += CHALLENGE_SIZE;

#ifndef CISCO
	/*get the hidden memory index for secret number and share number */
	if (index != NULL_INDEX)
	{
		hi_dh_secret = pconn_list[index].hi_dh_secret;
		hi_dh_shared = pconn_list[index].hi_dh_shared;
	}
	else
	{
		if (hMemAlloc(&hi_dh_secret,
					   dhPublicLen,
					   BLK_NOACCESS) ||
			hMemAlloc(&hi_dh_shared,
					   dhPublicLen,
					   BLK_NOACCESS))
		{
			Error_proc(0,
					   0,
					   CS_ERR_ALLOC_HM,
					   &cs_err_alloc_hm,
					   &(fc->epEntry.pe),
					   &(fc->epEntry.upe));
			(*p_status) |=  STAT_GEN_DH_FAILED;
		}
	}
#else
	/*
	 * Ensure gen_DH_pub_secret() gets called
	 */
	hi_dh_secret = 1; /* non-zero */
#endif

	/*generate the DH public and secret numbers */
	if (hi_dh_secret) gen_DH_pub_secret(buf, hi_dh_secret, p_status, fc);
	buf += dhPublicLen;

	/*generate the DH share number to apply to signature */
	if (!(*p_status & STAT_GEN_DH_FAILED))
	{
#ifndef CISCO
		if (hGetDHsharedNumber(dhPublicLen,
								hi_dh_secret,	/*my secret number */
								pdu + PDU_FIX_NNC_OFFSET, /*far SDU's DH public */
								hi_dh_shared,   	/*output: DH shared number */
								dhModulus) ||
			hMemSyndrome(hi_dh_shared,
						 SYNDROME_HMA_OFFSET,
						 dhPublicLen,
						 CHALLENGE_SIZE,
						 syndrome))
#else /* CISCO */
		/*
		 * Generate the shared number and syndrome.
		 */
		if (!crypto_glue_generate_DH_shared_number(
				pdu + PDU_FIX_NNC_OFFSET,	/*far SDU's DH public */
				fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id,
				fc->epEntry.info.cs.cib->crypto.cisco.slot) ||
			!crypto_glue_generate_syndrome(
				fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id,
				fc->epEntry.info.cs.cib->crypto.cisco.slot,
				SYNDROME_HMA_OFFSET,
				CHALLENGE_SIZE,
				syndrome))
#endif /* CISCO */
		{
			Error_proc(	0,
						0,
						CS_ERR_GEN_DH,
						&cs_err_gen_dh,
						&(fc->epEntry.pe),
						&(fc->epEntry.upe));
			(*p_status) |= STAT_GEN_DH_FAILED;
		}
	}

	/*plug in the NNC - non null pointer */
	nnc_size = get_nnc_size(nnc_to_send);
	memcpy(buf, nnc_to_send, nnc_size);
#ifdef CISCO
	crypto_glue_insert_node_id(buf, &fc->epEntry.pe);
#endif
	buf += nnc_size;

	/*generate the signature */
	gen_signature(buf, p_status, &(fc->epEntry.pe), &(fc->epEntry.upe),
				  l5, *auth,
				  nnc_to_send + NNC_FIX_GRP_OFFSET, /*SDU DSS public num */
				  fc->epEntry.info.cs.cib->crypto.cisco.slot);
	buf += DEF_SIG_SIZE;

	/*alter the challenge number */
	if (!(*p_status & STAT_GEN_DH_FAILED))
	{
		/*copy the real challenge number to retx buffer before altering it */
		if (index != NULL_INDEX)
		{
			memcpy(	pconn_list[index].retx_buf + retx_l5_offset + PDU_CHAL_OFFSET,
					p_challenge,
					CHALLENGE_SIZE);
		}
		xor_challenge(p_challenge);
	}

	/* set the end of layer 5 pointer */
	*l5_end = buf;
	/*change auth if anything goes wrong */
	*auth = *p_status;

#ifndef CISCO
	/*free previously allocated hidden memory */
	if (index == NULL_INDEX)
	{
		if (hi_dh_secret)	/*hidden memory was allocated successfully */
		{
			if (hMemFree(hi_dh_secret))
			{
				Error_proc(	0,
							0,
							CS_ERR_RELEASE_HM,
							&cs_err_release_hm,
							&(fc->epEntry.pe),
							&(fc->epEntry.upe));
			}
		}

		if (hi_dh_shared)   /*hidden memory was allocated successfully */
		{
			if (hMemFree(hi_dh_shared))
			{
				Error_proc(	0,
							0,
							CS_ERR_RELEASE_HM,
							&cs_err_release_hm,
							&(fc->epEntry.pe),
							&(fc->epEntry.upe));
			}
		}
	}
#endif /* !CISCO */
}


/*******************************************************************************
* NAME:             build_cs_msg_cfm
*
* DESCRIPTION:		This function builds the connection setup confirm message
*
* INPUTS:			index - the pending connection list entry built previously
*                   l5 - where the output confirm msg starts
*                   auth - the authentication result
*                   pdu - point to where the received Reply msg
*
* OUTPUT:           l5_end is set to point to one byte after the output reply
*                   msg.
*                   buffer pointed to by l5 is filled with the message
*
*******************************************************************************/
#ifdef CISCO
void build_cs_msg_cfm(uchar *l5, uchar *auth, uchar *pdu, uchar ** l5_end,
					  short index, uchar *recv_challenge, Pa2EpFrameControl *fc)
#else
void build_cs_msg_cfm(uchar *l5, uchar *auth, uchar *pdu, uchar ** l5_end,
					  short index, uchar *recv_challenge)
#endif
{
	uchar 		*buf;
	uchar		*p_status;
	pconn_entry	*pconn;
	ushort      size;

	buf = l5;

	/*copy the first 3 bytes */
	memcpy(buf, nnc_cfm_3b, sizeof(nnc_cfm_3b));
	buf += sizeof(nnc_cfm_3b);

	/*set the 1-byte status field */
	*buf = *auth;
	p_status = buf;
	buf++;

#ifdef CISCO
	/*copy the 4-byte responder's unique connection ID field */
	/*warning! note the hack of defining the conn_id to be a ulong*/
	memcpy(buf, pdu + PDU_RESP_CONNID_OFFSET, sizeof(ulong));
	buf += sizeof(ulong);
#else
	/*copy the 4-byte responder's unique connection ID field */
	memcpy(buf, pdu + PDU_RESP_CONNID_OFFSET, sizeof(cs_unique_conn_id));
	buf += sizeof(cs_unique_conn_id);
#endif /* !CISCO */

	/*copy initiator's and responder's pending indexes */
	size = (PCONN_INDEX_SIZE << 1);
	memcpy(buf, pdu + PDU_INIT_PCONN_OFFSET, size);
	buf += size;

	/*copy the "original(not XOR'ed)" complete challenge number */
	memcpy(buf, pdu + PDU_CHAL_OFFSET, CHALLENGE_SIZE);
	buf += CHALLENGE_SIZE;

	/*generate the signature: WARNING: THE CALLER HAS TO MAKE SURE index is not */
	/*											  NULL_INDEX */
	pconn = &pconn_list[index];
	gen_signature(buf, p_status,
				  &(pconn->pe), &(pconn->upe), l5,
				  pconn->retx_buf[pconn->l5_offset + PDU_STAT_OFFSET],
				  pconn->nnc + NNC_FIX_GRP_OFFSET,	/*SDU DSS public num */
				  fc->epEntry.info.cs.cib->crypto.cisco.slot);
	buf += DEF_SIG_SIZE;

	/*alter the challenge number */
	memcpy(l5 + PDU_CFM_CHAL_OFFSET, recv_challenge, CHALLENGE_SIZE);

	/* set the end of layer 5 pointer */
	*l5_end = buf;
	/*change auth if anything goes wrong */
	*auth = *p_status;
}


/*******************************************************************************
* NAME:             srch_pconn_index
*
* DESCRIPTION:		This function searches the pending connection list
*                   for the connection identified by PE and UPE pair.
*
* INPUTS:           frame control and pending connection list
*
* OUTPUT:           index if found; NULL_INDEX otherwise
*
*******************************************************************************/
short srch_pconn_index(ulong recv_conn_id, ushort retx_connid_ofs)
{
	pconn_entry     *pconn;
	ulong           saved_conn_id;
	short 			i;

	i = pconn_used;
	while (i != NULL_INDEX)
	{
		pconn = pconn_list + i;
		saved_conn_id = *((ulong *)(pconn->retx_buf +
									pconn->l5_offset + retx_connid_ofs));
		if (recv_conn_id == saved_conn_id) return i;

		i = pconn->next;
	}

	return(NULL_INDEX);
}
#ifndef CISCO
/*******************************************************************************
* NAME:             get_ip_checksum
*
* DESCRIPTION:		This function computes the IP header checksum
*                   It is copied from the book "Internetworking with
*                   TCP/IP vol II" by Douglas Comer. (page 70)
*
* INPUTS:           buf - pointer to the first 16-bit word in the header
*                   nwords - the number of 16-bit words in the header
*
* OUTPUT:           computed checksum
*
*******************************************************************************/
ushort get_ip_checksum(ushort *buf, int nwords)
{
	ulong       sum;

	for (sum = 0; nwords > 0; nwords--)
	{
		sum += *buf++;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);     /*add in carry */
	sum += (sum >> 16);                     /*maybe one more */
	return((ushort)(~sum));
}
#endif /* !CISCO */
/*******************************************************************************
* NAME:             handle_nnc_req_revisions
*
* DESCRIPTION:		This function verifies the NNC revision
*
* INPUTS:           fc  - the frame control associated with the received NNC
*                         Request frame
*
*                   pdu - point to where the received connection message starts
*
* OUTPUTS:          nnc_to_send - point to the NNC to send back
*                   nnc_to_use  - point to the NNC to be used  in the matching
*                                 algorithm to create CIB
*
* RETURN:           1 if NNC version expired; 0 if it is up to date
*
*******************************************************************************/
uchar handle_nnc_req_revisions(uchar *pdu, Pa2EpFrameControl *fc,
				  int *use_new_old, uchar **nnc_ptr, uchar far_end_auth)
{
	ushort      	far_end_nnc_rev;
	uchar *     	rev_ptr;
	ulong       	now;
	uchar *     	local_new_nnc;
	uchar *     	local_old_nnc;
	ushort      	local_new_nnc_rev;
	ushort      	local_old_nnc_rev;
	ulong			base_time;
	AuthNetControl	*p_tm;
#ifdef CISCO
	uchar			dummy_auth;
#endif

	/*config tables have been downloaded, and there is NC in system. */
	if ((csAvail.bitMap == CS_CONFIG_TB_AVAIL) &&
		((p_tm = getAuthTimers()) != NULL) &&
		((base_time = getNCtimeStamp()) != 0))
	{
		/*****************************************************
		* get the far end's nnc revision
		******************************************************/
		rev_ptr = pdu + PDU_FIX_NNC_REV_OFFSET + dhPublicLen;
		GET_PDU_USHORT(far_end_nnc_rev, rev_ptr);

		/*****************************************************
		* get this end's nnc revisions - both new and old
		******************************************************/
		*nnc_ptr = NULL;	/*initialization */
		local_old_nnc = nnc_old_tb + (fc->epEntry.info.cs.nnc);
		local_new_nnc = nnc_new_tb + (fc->epEntry.info.cs.nnc);
		GET_PDU_USHORT(local_old_nnc_rev, local_old_nnc + NNC_REV_OFFSET);
		GET_PDU_USHORT(local_new_nnc_rev, local_new_nnc + NNC_REV_OFFSET);

		/******************************************************
		* verify revisions according to different time marks
		*******************************************************/
		GetRTClock(&now);
		if (now < (base_time + p_tm->aTime))     		/*now < A_time */
		{
			if ((far_end_nnc_rev == local_old_nnc_rev) || /*receive old version */
				(far_end_auth & STAT_SYS_OC_FLAG))
			{
				*nnc_ptr = local_old_nnc;
				*use_new_old = SM_DSS_PUBLIC_OLD;
			}
		}
		else if (now < (base_time + p_tm->sTime))    	/* A_time <= now < S_time */
		{
			if (far_end_nnc_rev == local_new_nnc_rev) 	  /*receive new version */
			{
				*nnc_ptr = local_new_nnc;
				*use_new_old = SM_DSS_PUBLIC_NEW;
			}
			else if ((far_end_nnc_rev == local_old_nnc_rev) || /*receive old version */
					 (far_end_auth & STAT_SYS_OC_FLAG))
			{
				*nnc_ptr = local_old_nnc;
				*use_new_old = SM_DSS_PUBLIC_OLD;
			}
		}
		else if (now < (base_time + p_tm->eTime))       /* S_time <= now < E_time */
		{
			if ((far_end_nnc_rev == local_new_nnc_rev) || /*receive new version */
				(far_end_auth & STAT_SYS_OC_FLAG))
			{
				*nnc_ptr = local_new_nnc;
				*use_new_old = SM_DSS_PUBLIC_NEW;
			}
		}
		/*else											// now > E_time */

		if (*nnc_ptr)
		{
    		if ((*(ushort *)(*nnc_ptr + NNC_REV_OFFSET)) != 0)
    		{	/*the NNC revision is zero when epPEdef has been updated but not NNC table */
    			return(CS_NO_ERROR);
    		}
    	}
    	else
    	{
    		/*NNC expired. */
    		Error_proc(	0,
    					0,
    					CS_ERR_NNC_EXP,
    					&cs_err_nnc_exp,
    					&(fc->epEntry.pe),
    					&(fc->epEntry.upe));
    	}
	}

#ifdef CISCO
	*nnc_ptr = get_nnc(fc, &dummy_auth, use_new_old);
	return(CS_NO_ERROR);
#else /* !CISCO */
	*nnc_ptr = nnc_sys_oc;
	return(STAT_SYS_OC_FLAG);	/**use_new_old becomes undefined */
#endif /* !CISCO */
}

/****************************************************************************
* NAME:         bad_nnc_frm_size
*
* DESCRIPTION:  This function checks if the received connection message
*               has proper length.
*
* INPUTS:       fc - frame control of the received frame
*               pdu - connection message pdu in the received frame
*               frm - indicates if Request, Reply, or Confirm.
*
* OUTPUTS:      return received NNC size
*				return 1 if the length is bad, 0 otherwise.
*
****************************************************************************/
int bad_nnc_frm_size(	Pa2EpFrameControl	*fc,
						uchar 				*pdu,
						int 				frm_type,
						ushort 				*recv_nnc_size)
{
	uchar	*nnc;
	uchar   *computed_end;

	/**************************************
	* Get the correct PDU size
	***************************************/
	switch (frm_type)
	{
		case ECHO_REQUEST:
		case ECHO_REPLY:
			nnc = pdu + PDU_FIX_NNC_OFFSET + dhPublicLen;
			*recv_nnc_size = get_nnc_size(nnc);
			computed_end = nnc + (*recv_nnc_size) + DEF_SIG_SIZE;
			break;

		case ECHO_CONFIRM:
			*recv_nnc_size = 0;
			computed_end = pdu + PDU_CFM_SIG_OFFSET + DEF_SIG_SIZE;
			break;

		default:
#ifdef INTGR_DEBUG	/*TEST ONLY */
			cs_debug |= CS_TEST_BAD_NNC_FRM_TYPE;
#endif

			return (1);		/*impossible case */
	}

	/*********************************************************************
	* Check PDU size: computed size should be bound by the physical size
	*********************************************************************/
	if (computed_end <=	(((uchar *)fc->frameLink) + fc->frameLength))
	{
		return(0);      /*frame length is ok */
	}
	else
	{
		if (frm_type == ECHO_REQUEST) /*only the first frame carries entity IDs */
		{
			Error_proc(0,
					   0,
					   CS_ERR_NNC_FRM_SIZE,
					   &cs_err_nnc_frm_size,
					   &(fc->epEntry.pe),
					   &(fc->epEntry.upe));
		}
		else
		{
			Error_proc(0,
					   0,
					   CS_ERR_NNC_FRM_SIZE,
					   0,
					   0,
					   0);
		}
		return(1);      /*frame length is bad */
	}
}


/***************************************************************************
* NAME:         cs_transmit_frm
*
* DESCRIPTION:  This function transmits or bypass NNC frames.
*
* INPUTS:       fch     - the frame control header.
*               size    - frame length
*
* OUTPUTS:      frame is sent. if there is any error, buffer will be released
*
****************************************************************************/
void  cs_transmit_frm(Cy2TxFrameControl *fch, ushort size,
					  EntityID *pe, EntityID *upe,
					  void *port, int action)
{
	int error;

	fch->txPortP = port;
	error = taskTransmitReq(fch, size, action);
	if (error == REJECT_TXREQ)
	{
		taskReleaseBuffer(fch->descLogP);
		Error_proc(0,
				   0,
				   CS_ERR_TRANSMIT,
				   &cs_err_transmit,
				   pe,
				   upe);
	}

#ifdef CISCO
	else {
		/*
	 	 * Return the fch buffer here.
	 	 */
		taskReleaseBuffer(fch->descLogP);
	}
#endif

#ifdef INTGR_DEBUG	/*TEST ONLY */
	else
	{
		cs_transmit_count++;
	}
#endif
}


/**************************************************************************
* NAME:			get_nnc_size
*
* DESCRIPTION:	This function calculates the NNC size from the Groups field
*				in NNC.
*
* INPUTS:		nnc - pointer to the  certificate
*
* RETURNS:		the length of NNC in bytes
*
**************************************************************************/
ushort	get_nnc_size(uchar *nnc)
{
	ushort 	groups;
	ushort 	nnc_size;
	uchar   *grp_ptr;

	grp_ptr = nnc+ NNC_FIX_GRP_OFFSET + dssPkeyLen;
	GET_PDU_USHORT(groups, grp_ptr);
	nnc_size = (((groups + 31) / 32) * NNC_GRP_BLK_SIZE) + /*number of blocks * 12 bytes */
				dssPkeyLen +  NNC_FIX_SIZE;

	/*sanity check on NNC size */
	if (nnc_size > MAX_NNC_SIZE)
	{
		Error_proc(0, 0, CS_ERR_NNC_SIZE, 0, 0, 0);
		nnc_size = MAX_NNC_SIZE;
	}

	return nnc_size;
}

#ifndef CISCO
/**************************************************************************************
* NAME:         post_err_iomgr
*
* DESCRIPTION:  This function posts an error record to IO manager.
*
* INPUTS:       err_bit - the error bit to be reported
*				err_flag - the 32-bit error big flag
*               task_id - task number
*
* OUTPUTS:      IO Manager API is called.
*
* NOTE: 		It is IOMgr's responsibility to watch out not to be flooded
*				with events.
*
**************************************************************************************/
void post_err_iomgr(ulong err_bit, ulong *err_flag, ushort task_id)
{
	uchar event = 1;
	ulong org_bit;

	org_bit = err_bit;
	/*Since the event number is a 8-bit ID number instead of a 64-bit error flag, */
	/*conversion needs to be done here. */
	while (!(err_bit & 1))
	{
		err_bit >>= 1;
		event++;
	}

	/*Call IO Mgr API */
	post2IOmanager(task_id, event, 0);	/*the last param is not used */

	/*set the bit in error flag: THIS HAS TO BE THE LAST. */
	(*err_flag) |= org_bit;
}


/****************************************************************************
* NAME:			set_cs_frm_nw_addr
*
* DESCRYPTION:	This function sets the network address field in the
*				Connection Setup protocol PDU.
*
* INPUTS:		buf - pointer to the destination
*				pid - the protocol ID
*
* OUTPUTS:		see DESCRYPTION.
*
****************************************************************************/
static void set_cs_frm_nw_addr(uchar *buf, ushort pid)
{
	uchar	*paddr;

	memset(buf, 0, PDU_MAX_NW_ADDR_SIZE);	/*clear the field first */
	switch (pid)
	{
		case INTERNET_IP:
			memcpy(buf, sys_sdu_nw_addr_ip, PDU_NW_ADDR_SIZE);
			break;

		case XEROX_IDP:
		case NOVELL_IPX:
			paddr = (pid == XEROX_IDP) ? sys_sdu_nw_addr_idp :
										 sys_sdu_nw_addr_ipx ;
			/*set the SDU's network addr */
			memcpy(buf, paddr, PDU_NW_ADDR_SIZE);
			/*set the SDU's MAC addr */
			memcpy(buf + PDU_NW_ADDR_SIZE, systemMacAddress_g, MAC_ADDR_LEN);
			break;

		case APPLE_DDP:
			buf[0] = sys_sdu_nw_addr_ddp[0];
			buf[1] = sys_sdu_nw_addr_ddp[1];
			buf[2] = sys_sdu_ddp_node_id;
			break;

		default:	/*invalid case */
			break;
	}
}
#endif /* !CISCO */

/**********************************************************************
*
*
*
*
***********************************************************************/
void genDHshareXorChallenge(Pa2EpFrameControl	*fc,
							uchar 				*pdu,
							uchar 				*auth,
							short 				index,	/*non NULL_INDEX */
							uchar 				auth_sent,
							uchar				far_end_auth)
{

	if (!(auth_sent & STAT_GEN_DH_FAILED))
	{
#ifndef CISCO
		/*DH public and secret numbers have been genereated when sending Init */
		if (!hGetDHsharedNumber(dhPublicLen,
								pconn_list[index].hi_dh_secret, /*my secret number */
								pdu + PDU_FIX_NNC_OFFSET, /*far SDU's DH public */
								pconn_list[index].hi_dh_shared, /*output: DH shared number */
								dhModulus))
#else /* CISCO */
		if (crypto_glue_generate_DH_shared_number(
				pdu + PDU_FIX_NNC_OFFSET,	/*far SDU's DH public */
				fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id,
				fc->epEntry.info.cs.cib->crypto.cisco.slot))
#endif /* CISCO */
		{
			if (!(far_end_auth & STAT_GEN_DH_FAILED))  /*far end has XORed it */
			{
#ifndef CISCO
				if (!hMemSyndrome(pconn_list[index].hi_dh_shared,
								  SYNDROME_HMA_OFFSET,
								  dhPublicLen,
								  CHALLENGE_SIZE,
								  syndrome))
#else /* CISCO */
				if (crypto_glue_generate_syndrome(
					  fc->epEntry.info.cs.cib->crypto.cisco.new_connection_id,
					  fc->epEntry.info.cs.cib->crypto.cisco.slot,
					  SYNDROME_HMA_OFFSET,
					  CHALLENGE_SIZE,
					  syndrome))
#endif /* CISCO */
				{
					xor_challenge(pdu + PDU_CHAL_OFFSET);	/*restore the challenge number */
				}
				else
				{
					Error_proc(	0,
								0,
								CS_ERR_GEN_DH,
								&cs_err_gen_dh,
								&(fc->epEntry.pe),
								&(fc->epEntry.upe));
					(*auth) |= STAT_GEN_DH_FAILED;
				}
			}
		}
		else
		{
			Error_proc(	0,
						0,
						CS_ERR_GEN_DH,
						&cs_err_gen_dh,
						&(fc->epEntry.pe),
						&(fc->epEntry.upe));
			(*auth) |= STAT_GEN_DH_FAILED;
		}
	}
}

/**********************************************************************
* NAME: 		xor_challenge
*
* DESCRYPTION:	This function uses the DH share number "syndrome"
*				(generated before this function is called) to XOR the
*				even bits in challenge number.
*
* INPUTS:		pChallenge - pointer to the challenge number
*				hiDHshared - the hidden memory index to the DH share number
*
* OUTPUTS:		Challenge number is changed. If failed, the *pStatus will
*				have the bit STAT_GEN_DH_FAILED set.
*
***********************************************************************/
void xor_challenge(uchar *challenge)
{
	short i;

	for (i = 0; i < CHALLENGE_SIZE; i++)
	{
		challenge[i] = ((challenge[i] & CHAL_EVEN_MASK) ^
						(syndrome[i] & CHAL_EVEN_MASK)) |
					   (challenge[i] & CHAL_ODD_MASK);
	}

/*
	#define HALF_CHAL_SIZE			(CHALLENGE_SIZE / 2)
	#define XOR_SYNDROME(chal, synd)					\
	{													\
		*chal = ((*(chal) & CHAL_EVEN_MASK) ^ synd) |	\
				(*(chal) & CHAL_ODD_MASK);				\
	}

	uchar	syndrome[HALF_CHAL_SIZE];
	uchar	*challenge1;
	uchar	*challenge2;
	short	i;
	uchar	syndrome1;
	uchar	syndrome2;

	for (i = 0; i < HALF_CHAL_SIZE; i++)
	{
		syndrome1 = syndrome[i] & CHAL_EVEN_MASK;
		syndrome2 =  (syndrome[i] & CHAL_ODD_MASK) >> 1;
		challenge1 = challenge + (2 * i);
		challenge2 = challenge1 + 1;
		XOR_SYNDROME(challenge1, syndrome1);
		XOR_SYNDROME(challenge2, syndrome2);
	}
*/
}

/**********************************************************************
* NAME:			umSduGetDssPublic
*
* DESCRYPTION:	This function resolves the "Cisco Unmanaged SDU" issue.
*				It searches a table downloaded from SM for the SDU's
*				DSS public number.
*
* INPUTS:       SDU's network address and serial number
*
* OUTPUTS:		None.
*
* RETURN:		A pointer to the SDU's DSS public number; NULL if it
*				is not in the table.
*
***********************************************************************/
#pragma argsused
uchar *umSduGetDssPublic(uchar *sduNetAddr, uchar *sduSerialNum, CIBptr cib)
{
#ifdef CISCO
	return crypto_glue_umSduGetDssPublic(sduNetAddr, sduSerialNum, cib);
#else
	return NULL;
#endif /* !CISCO */
}

/*---------------------------- END OF FILE -------------------------------- */
