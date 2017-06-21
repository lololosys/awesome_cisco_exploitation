/* $Id: ospf_hello.c,v 3.2.58.5 1996/08/28 12:53:19 thille Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/ospf_hello.c,v $
 *------------------------------------------------------------------
 * ospf_hello.c -- Supports OSPF hello protocol between neighbors
 *
 * October 4 1990, Phanindra Jujjavarapu
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ospf_hello.c,v $
 * Revision 3.2.58.5  1996/08/28  12:53:19  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.58.4  1996/07/24  07:45:31  myeung
 * CSCdi63337:  Unable to bring up adjacencies on p2mp
 * Branch: California_branch
 *
 * Revision 3.2.58.3  1996/07/01  07:44:45  myeung
 * CSCdi56125:  Router links information incorrect for an internal stub
 * area router
 * Branch: California_branch
 * - Schedule SPF when we flush the old DR network LSA
 *
 * Revision 3.2.58.2  1996/04/16  19:01:48  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.2.58.1  1996/03/18  20:28:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  09:49:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  14:30:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  17:36:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:09:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/24  12:33:38  myeung
 * CSCdi38999:  ospf point-to-multipoint broken with 4 routers on same
 * subnet
 * - Except for virtual link, do not use the routing table for sending
 *   OSPF packet to neighbor
 *
 * Revision 2.1  1995/06/07  21:09:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Routines exported from this mode
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "../ui/debug.h"
#include "route.h"
#include "logger.h"
#include "../iprouting/msg_ospf.c"	/* Not a typo, see logger.h */
#include "iprouting_debug.h"
#include "../ip/ip.h"
#include "../if/network.h"
#include "ospf_pak.h"
#include "ospf.h"
#include "../util/md5.h"
#include "../os/old_timer_callbacks.h"

/*
 * ospf_send_hello
 * builds a hello packet for an
 * interface and sends it out to
 * a multicast address.
 */
void ospf_send_hello (areatype *area, idbtype *idb, nbrtype *rep_nbr)
{
    paktype *pak;
    hellotype *hello;
    ulong *temp;
    ushort length;
    nbrtype *nbr;
    ipaddrtype dest_addr = 0;
    ospf_idb_info *ospf_info = idb->ospf_info;

    /* Don't send hello if we are loading up
     */

    if (system_loading)
	return;

    if (!ospf_info->oi_myself) {
	errmsg(&msgsym(NOSELF, OSPF), idb->namestring, ospf_info->if_state);
	return;
    }
    /*
     * First check if this interface
     * is passive for this router.
     */
    if (idb->static_routing ||
	(idb->ip_passive_router & (area->ar_pdb_ptr->mask))) {
	return;
    }

    /*
     * If the OSPF interface state is DOWN but the real interface
     * state is UP, then there is something wrong.   Set a flag
     * and let the main thread check and correct.
     * Ignore the test if we are running as demand circuit on p2p link on
     * which the I_UP event has been performed 
     * (oi_myself->nbr_state != NBR_DOWN). 
     */
    if ((!ospf_info->oi_demand_circuit || 
	 (ospf_info->if_type != IF_POINT_TO_POINT) || 
	 (ospf_info->oi_myself->nbr_state == NBR_DOWN)) &&
	(ospf_info->if_state == IFS_DOWN)) {
	if (interface_up(idb) && (ospf_info->if_type != IF_VIRTUAL_LINK)) {
	    area->ar_pdb_ptr->ospf_flag |= OSPF_IF_JUMPSTART;
	    errmsg(&msgsym(OSPFINTDOWN, OSPF), idb->namestring);
	}
	return;
    }

    pak = getbuffer(IPHEADERBYTES(NOPT) + OSPF_HDR_SIZE +
		    HELLO_HDR_SIZE + ospf_info->nbrcnt * NBR_ID_SZ + MD5_LEN);
    if (!pak)
	return;

    hello = (hellotype *)((ipheadstart(pak) + IPHEADERBYTES(NOPT)) +
			  OSPF_HDR_SIZE);
    hello->h_netmask = idb->ip_nets_mask;
    hello->h_deadint = ospf_info->dead_interval;
    hello->h_interval = ospf_info->hello_interval;
    hello->h_rtr_options = area->ar_options;
    /*
     * If we run the over p2p, virtual link or p2mp DC, set the DC bit.
     */
    if (OSPF_DC_IF_TYPE(ospf_info) && ospf_info->oi_demand_circuit)
	hello->h_rtr_options |= OPTION_DC_BIT;
    hello->h_rtr_priority = ospf_info->rtr_pri;
    hello->h_deg_rtr = ospf_info->dr ? ospf_info->dr->nbr_ipaddr : 0;
    hello->h_bdeg_rtr = ospf_info->bdr ? ospf_info->bdr->nbr_ipaddr :0;
    length = HELLO_HDR_SIZE;
    /* build the neighbor entries */
    temp = &(hello->h_nbrs);
    nbr = ospf_info->oi_nbr;
    /*
     * check if the interface is POINT-TO-POINT or
     * VIRTUAL in which case we have only one neighbour
     */
    if ((ospf_info->if_type == IF_POINT_TO_POINT) ||
	(ospf_info->if_type == IF_VIRTUAL_LINK)) {
	if (nbr) {
	    if ((nbr->nbr_state == NBR_FULL) && nbr->nbr_suppress_hello) {
		datagram_done(pak);
		return;
	    }
	    if (nbr->nbr_state > NBR_ATTEMPT) {
		*temp = nbr->nbr_id;
		length += NBR_ID_SZ;
		temp++;
	    }
	}
    } else { /* NBMA or BROADCAST or P2MP */
	if ((ospf_info->if_type == IF_POINT_TO_M_POINT) &&
	    (ospf_info->nbrcnt == ospf_info->oi_suppress_hello_count) &&
	    (ospf_info->nbrcnt == ospf_info->nbr_fcnt) &&
	    (ospf_info->oi_suppress_hello_count > 0)) {
		datagram_done(pak);
		return;
	    }
	for(; nbr != NULL; nbr = nbr->nbr_next) {
	    if (nbr->nbr_state > NBR_ATTEMPT) {
		*temp = nbr->nbr_id;
		length += NBR_ID_SZ;
		temp++;
	    }
	    /* also Dr and backup cases */
	}
    }
    /* find type of address to send
     * to depending upon type of network
     */
    switch (ospf_info->if_type) {
      case IF_BROADCAST:
      case IF_POINT_TO_POINT:
      case IF_POINT_TO_M_POINT:
	dest_addr =  ALLSPFROUTERS;
	break;
      case IF_VIRTUAL_LINK:
	dest_addr = ALL_VIRTUAL_NBRS;
	break;
      case IF_NONBROADCAST:
	/* check to see if this was a poll timer or response */
	if (rep_nbr) 
	    dest_addr = rep_nbr->nbr_ipaddr;
	else { 
	    switch (ospf_info->if_state){
	      case IFS_WAITING:
	      case IFS_OTHER: 
		if (ospf_info->rtr_pri)
		    dest_addr = ALL_ELIG_NBRS;
		else
		    dest_addr = DR_BDR_NBRS;
		break;
	      case IFS_DR:
	      case IFS_BACKUP:
		dest_addr = ALL_UP_NBRS;
		break;
	    }
	    break;
	}
    }
    ospf_send_packet(pak, idb, OSPF_HELLO, length, dest_addr, FALSE);
} /* end of send_hello_pak */	


/*
 * ospf_rcv_hello
 * receives hello from one of its neighbour.
 */
void ospf_rcv_hello (paktype *pak, ospftype *ospf, areatype *area)
{
    iphdrtype   *ip;
    hellotype	*hello;
    nbrtype	*nbr;
    ulong	*nbr_id;
    ushort	nbr_count;
    idbtype	*idb;
    boolean	check_mask, dc_bit;
    ospf_idb_info *ospf_info;

    ip = (iphdrtype *)ipheadstart(pak);
    hello = (hellotype *)(ospf->ospf_data);
    idb = pak->if_input;
    ospf_info = idb->ospf_info;

    /* 
     * check that interface up event is detected before
     * accepting any hellos on this interface and we are
     * not seeing our own hello packets.
     */
    if (!ospf_info->oi_myself) {
	errmsg(&msgsym(NOSELF, OSPF), idb->namestring, ospf_info->if_state);
	return;
    }
    if ((ospf_info->oi_myself->nbr_state == NBR_DOWN) || 
	(ip_up_ouraddress(ip->srcadr))) {
	return;
    }
    /* 
     * check Netmask, HelloInt and DeadInt for inconsistencey
     * with configured values for interface.
     * Don't check the mask in case of unnumbered, P2P, P2MP, or virtual links.
     */
    check_mask = (!idb->ip_unnumbered &&
		  (ospf_info->if_type != IF_POINT_TO_POINT) &&
		  (ospf_info->if_type != IF_POINT_TO_M_POINT) &&
		  (ospf_info->if_type != IF_VIRTUAL_LINK));

    if ((check_mask && (hello->h_netmask != idb->ip_nets_mask))
	|| (hello->h_interval != ospf_info->hello_interval)
	|| (hello->h_deadint != ospf_info->dead_interval)) {
	ospf_adj_buginf("\nOSPF: Mismatched hello parameters from %i",
			ip->srcadr);
	if (check_mask)
	   ospf_adj_buginf("\nDead R %d C %d, Hello R %d C %d  Mask R %i C %i",
			   hello->h_deadint, ospf_info->dead_interval,
			   hello->h_interval, ospf_info->hello_interval,
			   hello->h_netmask, idb->ip_nets_mask);
	else
	   ospf_adj_buginf("\nDead R %d C %d, Hello R %d C %d",
			   hello->h_deadint, ospf_info->dead_interval,
			   hello->h_interval, ospf_info->hello_interval);

	return;
    }

    /* 
     * check NSSA indication bit N in options field match with 
     * what we support.
     */

    if (GET_N_P_BIT(hello->h_rtr_options) 
	!= GET_N_P_BIT(area->ar_options)) {
	ospf_adj_buginf("\nOSPF: Hello from %i with mismatched "
			"NSSA option bit", ip->srcadr);
	return;
    }

    /* 
     * check external routing capability indication bit E in options
     * field match with what we support.
     */

    if (GET_E_BIT(hello->h_rtr_options) != GET_E_BIT(area->ar_options)){
	ospf_adj_buginf("\nOSPF: Hello from %i with mismatched "
			"Stub/Transit area option bit", ip->srcadr);
	return;
    }

    
    /* find the neighbor(create one if not there) */
    nbr = ospf_find_nbr(idb, ospf->ospf_rtr_id, ip->srcadr, TRUE);
    if (nbr == NULL) {
	errmsg(&msgsym(NONEIGHBOR, OSPF), "Hello pkt",
	       ospf->ospf_rtr_id);
        return;
    }

    dc_bit = (GET_DC_BIT(hello->h_rtr_options) == OPTION_DC_BIT);
    if (OSPF_DC_IF_TYPE(ospf_info) && dc_bit) {
	if (!nbr->nbr_suppress_hello) {
	    nbr->nbr_suppress_hello = TRUE;
	    ospf_info->oi_suppress_hello_count++;
	}
	ospf_info->oi_demand_circuit = TRUE;
    }
    /*
     * If the neighbor is new, copy the message key id it used
     */
    if (nbr->nbr_flags & NBRF_NEW) {
        nbr->nbr_flags &= ~NBRF_NEW;
        nbr->nbr_message_digest_keyid = ospf->ospf_auth.auth2.keyid;
    }
    if (nbr->nbr_id == 0)
	nbr->nbr_id = ospf->ospf_rtr_id;

    /* events that drive neighbour state machine */

    /*
     * now found the neighbour use the finate state
     * machine to drive the neighbor while processing the
     * hello packet .
     */
    (*(nbr_state_machine[NBR_EVENT_HELLO_RCV][nbr->nbr_state]))
	                                 (idb, nbr);
    /*
     * now check if we have 2way or 1way communication with neighbour
     * check if our router id is in list of neighbours in hello packet
     */
    nbr_count = ospf->ospf_length - (OSPF_HDR_SIZE + HELLO_HDR_SIZE);
    nbr_count = nbr_count/NBR_ID_SZ;
    nbr_id = &(hello->h_nbrs);
    for (; nbr_count; nbr_count--, nbr_id++) {
	if (*nbr_id == area->ar_pdb_ptr->ospf_rtr_id) {
	    if (nbr->nbr_state < NBR_2WAY) {
		nbr->nbr_dr = hello->h_deg_rtr;
		nbr->nbr_bdr = hello->h_bdeg_rtr;
		nbr->nbr_pri = hello->h_rtr_priority;
		(*(nbr_state_machine[NBR_EVENT_2WAY][nbr->nbr_state]))
		                                              (idb, nbr);
		nbr->nbr_options = hello->h_rtr_options;
	    }
	    if (OSPF_DC_IF_TYPE(ospf_info) && !dc_bit) {
		if (nbr->nbr_suppress_hello) {
		    nbr->nbr_suppress_hello = FALSE;
		    ospf_info->oi_suppress_hello_count--;
		}
		ospf_info->oi_demand_circuit = 
		    ospf_info->oi_cfg_demand_circuit;
	    }
	    break;
	}
    }

    /* Didn't find our ID in the other guy's hello. */

    if (nbr_count == 0) {
	nbr->nbr_dr = hello->h_deg_rtr;
	nbr->nbr_bdr = hello->h_bdeg_rtr;
	nbr->nbr_pri = hello->h_rtr_priority;
	if (nbr->nbr_state > NBR_INIT) {
	    /* only one way between these neighbours */
	    if (OSPF_DC_IF_TYPE(ospf_info)) {
		if (nbr->nbr_suppress_hello != dc_bit) {
		    if (nbr->nbr_suppress_hello)
			ospf_info->oi_suppress_hello_count--;
		    else
			ospf_info->oi_suppress_hello_count++;
		    nbr->nbr_suppress_hello = dc_bit;
		    ospf_info->oi_demand_circuit = 
			ospf_info->oi_cfg_demand_circuit;
		}
	    }
	    (*(nbr_state_machine[NBR_EVENT_1WAY][nbr->nbr_state]))(idb, nbr);
	}
	goto check_response;
    }

    /* Events that drive interface state machine. */
    
    /* now check the priority */
    if (hello->h_rtr_priority != nbr->nbr_pri) {
	nbr->nbr_dr = hello->h_deg_rtr;
	nbr->nbr_bdr = hello->h_bdeg_rtr;
	nbr->nbr_pri = hello->h_rtr_priority;
	if (ospf_info->if_state > IFS_POINT_TO_POINT) {
	    (*(if_state_machine[IFE_NBRCHANGE][ospf_info->if_state]))
		                                   (idb, nbr);
	    goto check_response;
	}
    }
    /* now check designated router part */
    if (ospf_info->if_state == IFS_WAITING) {
	nbr->nbr_dr = hello->h_deg_rtr;
	if ((hello->h_deg_rtr == ip->srcadr) 
	    && (hello->h_bdeg_rtr == 0)) {
	    nbr->nbr_bdr = 0;
	    (*(if_state_machine[IFE_BACKUP_SEEN][ospf_info->if_state]))
		                                      (idb, nbr);
	}
    } else /* check if deg_rtr is same as before */
	if (hello->h_deg_rtr != nbr->nbr_dr) {
	    nbr->nbr_dr = hello->h_deg_rtr;
	    nbr->nbr_bdr = hello->h_bdeg_rtr;
	    (*(if_state_machine[IFE_NBRCHANGE][ospf_info->if_state]))
		                                      (idb, nbr);
	}
    /* now check backup designated router */
    if (ospf_info->if_state == IFS_WAITING) {
	if (hello->h_bdeg_rtr == ip->srcadr) {
	    nbr->nbr_bdr = hello->h_bdeg_rtr;
	    (*(if_state_machine[IFE_BACKUP_SEEN][ospf_info->if_state]))
		                                      (idb, nbr);
	}
    } else /* check if bdeg_rtr is same as before */
	if (hello->h_bdeg_rtr != nbr->nbr_bdr) {
	    nbr->nbr_bdr = hello->h_bdeg_rtr;
	    (*(if_state_machine[IFE_NBRCHANGE][ospf_info->if_state]))
		                                      (idb, nbr);
	}
  check_response:
    /* 
     * send hello for NOBBROADCAST
     * network if the hello is from 
     * eligible non DR and BDR and this
     * this router is not eligible for
     * DR or BDR.
     */
    if (ospf_info->if_type == IF_NONBROADCAST) {
	if ((ospf_info->bdr != nbr)
	    && (ospf_info->dr != nbr)
	    && nbr->nbr_pri
	    && (ospf_info->rtr_pri == 0))
	    ospf_send_hello(area, idb, nbr);

    }
} /* end of ospf_rcv_hello */

/*
 * ospf_md_authenticate
 * Generate digest for the message with the input key.
 * Digest is append to the end of message. The length of digest is
 * returned.
 */
static ulong ospf_md_authenticate (areatype *area, ospftype *ospf, 
				   ospf_md_key *md_key)
{     
    uchar *ospf_trailer;
    MD5_CTX mdcontext;

    ospf->ospf_checksum = 0;
    ospf->ospf_auth.auth2.digest_offset = ospf->ospf_length;
    ospf->ospf_auth.auth2.digest_len = MD5_LEN;
    ospf->ospf_auth.auth2.seq_num = area->ar_message_digest_seq++; 
    ospf_trailer = (uchar *) ospf + ospf->ospf_length;
    if (md_key) {
        ospf->ospf_auth.auth2.keyid = md_key->id;
        bcopy(md_key->key, ospf_trailer, MD5_LEN);
    } else {
        ospf->ospf_auth.auth2.keyid = 0;
        memset(ospf_trailer, 0, MD5_LEN);
    }
    MD5Init(&mdcontext);
    MD5Update(&mdcontext, (uchar *) ospf, ospf->ospf_length + MD5_LEN);
    MD5Final(ospf_trailer, &mdcontext);
    return MD5_LEN;
}

/*
 * ospf_write
 * Send an OSPF packet to a group or individual neighbor.
 */
static void ospf_write (
    paktype *pak,			/* the packet to send */
    ulong length,			/* 0 if internal, else total len */
    ipaddrtype src_addr, 			
    ipaddrtype dest_addr,		/* destination ip address */
    idbtype *idb,			/* xfc to use if broadcast */
    ipparamtype *param)			/* default parameters if internal */
{
    nbrtype *nbr;
    paktype *pak_copy;
    ospf_idb_info *ospf_info = idb->ospf_info;

    if (dest_addr >= ALL_UP_NBRS && dest_addr <= DR_BDR_NBRS) {
	nbr = ospf_info->oi_nbr;
	switch (dest_addr) {
	  case ALL_UP_NBRS:
	    for(; nbr != NULL; nbr = nbr->nbr_next) {
		if (nbr->nbr_state != NBR_DOWN) {
		    pak_copy = pak_duplicate(pak);
		    if (!pak_copy)
			break;
		    ipwrite(pak_copy, length, OSPF_PROT, src_addr,
			    nbr->nbr_ipaddr, idb, NULL, param);
		}
	    }
	    datagram_done(pak);
	    break;
	  case ALL_ELIG_NBRS:
	    for(; nbr != NULL; nbr = nbr->nbr_next) {
		if (nbr->nbr_pri) {
		    pak_copy = pak_duplicate(pak);
		    if (!pak_copy)
			break;
		    ipwrite(pak_copy, length, OSPF_PROT, src_addr,
			    nbr->nbr_ipaddr, idb, NULL, param);
		}
	    }
	    datagram_done(pak);
	    break;
	  case ALL_EXCH_NBRS:
	    for(; nbr != NULL; nbr = nbr->nbr_next) {
		if (nbr->nbr_state >= NBR_EXCHANGE) {
		    pak_copy = pak_duplicate(pak);
		    if (!pak_copy)
			break;
		    ipwrite(pak_copy, length, OSPF_PROT, src_addr,
			    nbr->nbr_ipaddr, idb, NULL, param);
		}
	    }
	    datagram_done(pak);
	    break;
	  case DR_BDR_NBRS:
	    if (ospf_info->dr) {
		pak_copy = pak_duplicate(pak);
		if (pak_copy)
		    ipwrite(pak_copy, length, OSPF_PROT, src_addr,
			    ospf_info->dr->nbr_ipaddr, idb, NULL, param);
	    }
	    if (ospf_info->bdr) {
		pak_copy = pak_duplicate(pak);
		if (pak_copy)
		    ipwrite(pak_copy, length, OSPF_PROT, src_addr,
			    ospf_info->bdr->nbr_ipaddr, idb, NULL, param);
	    }
	    datagram_done(pak);
	    break;
	  case ALL_VIRTUAL_NBRS:
            /* Send packet with IP TTL = 255 (default) on virtual link */
            param->ttl = TTL_DEFAULT;     
            ipwrite(pak, length, OSPF_PROT, src_addr,
		    ospf_info->vnbr_ipaddr, idb, NULL, param);
	    break;
	}
    } else {
	/*
	 * If it is not virtual link, set the output interface
	 * so we will not use the routing table to get the next hop.
	 * The next hop should be the neighbor address.
	 * In other words, the neighbor should be reachable
	 * in single hop.
	 * This is done because P2MP will cause adjacency
	 * failure otherwise because the host route it generates
	 * into the routing table will mislead us to send unicast
	 * OSPF packet over an old path rather than directly to
	 * the neighbor.
	 */
	if (ospf_info->if_type != IF_VIRTUAL_LINK)
	    pak->if_output = idb->output_if;
	ipwrite(pak, length, OSPF_PROT, src_addr,
		dest_addr, idb->output_if, NULL, param);
    }
}

/*
 * ospf_send_packet
 * build ospf header in the packet
 * send it out.
 */
void ospf_send_packet (
    paktype *ori_pak,
    idbtype *idb,
    ushort pak_type,
    ushort length,
    ipaddrtype dest_addr,
    boolean copy)
{
    ipaddrtype src_addr;
    ospftype *ospf;
    areatype *area;
    paktype *pak;
    paktype *pak_copy;
    ipparamtype params;		/* routing paramaters for ipwrite */
    ospf_idb_info *ospf_info = idb->ospf_info;
    ospf_md_key *md_key;
    ulong md_len;

    if (copy)
	pak = pak_duplicate(ori_pak);
    else
	pak = ori_pak;
    if (pak == NULL)
	return;

    params = ipdefaultparams;
    if (ospf_info->if_type == IF_VIRTUAL_LINK)
	params.ttl = TTL_DEFAULT;
    else
	params.ttl = 1;
    /*
     * Give priority to OSPF packet
     */
    params.tos = TOS_PREC_INET_CONTROL;
    pak->flags |= PAK_PRIORITY;
    area = ospf_info->area_ptr;
    length += OSPF_HDR_SIZE;
    ospf = (ospftype *)(ipheadstart(pak) + IPHEADERBYTES(NOPT));
    ospf->ospf_version = OSPF_VERSION;
    ospf->ospf_type = pak_type;
    ospf->ospf_rtr_id = area->ar_pdb_ptr->ospf_rtr_id;
    ospf->ospf_area_id = area->ar_id;
    ospf->ospf_length = length;
    ospf->ospf_checksum = 0;
    memset(ospf->ospf_auth.auth, 0, 8);
    ospf->ospf_autype = area->ar_autype;
    ospf->ospf_checksum = ipcrc((ushort *)ospf, length);
    length += IPHEADERBYTES(NOPT);

    /*
     * On a POINT-TO-POINT network always send
     * packets to ALLSPFROUTERS multicast address
     */
    if (ospf_info->if_type == IF_POINT_TO_POINT)
	dest_addr = ALLSPFROUTERS; 
    /*
     * we know where to send this packet except in the
     * case of MULTI-ACCESS NONBROADCAST Network.
     */
    src_addr = idb->ip_address;
    if ((src_addr == 0L) && (idb->ip_unnumbered))
	pak->if_output = idb;

    /*
     * Handle authentication
     */
    switch (ospf->ospf_autype) {
        case OSPF_AUTH_NONE:
            ospf_write(pak, length, src_addr, dest_addr, idb, &params);
	    break;
        case OSPF_AUTH_CT: 
	    bcopy(ospf_info->auth_key, ospf->ospf_auth.auth, 8);
	    ospf_write(pak, length, src_addr, dest_addr, idb, &params);
	    break;
	case OSPF_AUTH_MD:
	    if (!ospf_info->old_md_nbr_count) {
	        /*
		 * Neighbor agree on the youngest key
		 */
	        md_len = ospf_md_authenticate(area, ospf, 
					      ospf_info->youngest_md_key);
		ospf_adj_buginf("\nOSPF: Send with youngest Key %u", 
				ospf_info->youngest_md_key ? 
				ospf_info->youngest_md_key->id : 0);
		ospf_write(pak, length + md_len, src_addr, dest_addr,
			   idb, &params);
	    } else {
	        /*
		 * Some neighbor using obsolete key. Perform rollover
		 */
	        md_key = (ospf_md_key *) ospf_info->md_key_q.head;
	        while (md_key) {
		    ospf_adj_buginf("\nOSPF: Send with key %u", md_key->id);
		    pak_copy = pak_duplicate(pak);
		    if (!pak_copy)
		        return;
		    ospf = (ospftype *)(ipheadstart(pak_copy) + 
					IPHEADERBYTES(NOPT));
		    md_len = ospf_md_authenticate(area, ospf, md_key);
		    ospf_write(pak_copy, length + md_len, src_addr, dest_addr, 
			       idb, &params);
		    md_key = (ospf_md_key *) md_key->link.next;
	        }
		datagram_done(pak);
	    }
	    break;
    }
}


/* end of ospf_hello.c */









