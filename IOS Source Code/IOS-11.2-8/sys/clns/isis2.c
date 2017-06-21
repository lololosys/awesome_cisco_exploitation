/* $Id: isis2.c,v 3.3.60.1 1996/08/28 12:38:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/clns/isis2.c,v $
 *------------------------------------------------------------------
 * isis2.c -- Support routines for ISIS
 *
 * October 1991, Dino Farinacci
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isis2.c,v $
 * Revision 3.3.60.1  1996/08/28  12:38:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1995/11/17  08:53:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:13:05  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:09:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:52:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:18:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../clns/msg_clns.c"		/* Not a typo, see logger.h */
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "../os/chunk.h"
#include "../clns/clns.h"
#include "../clns/clns_route.h"
#include "../clns/isis.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_externs.h"
#include "../clns/isis_inlines.h"
#include "isis_nlsp_debug.h"
#include "../xns/ipx_registry.h"

/*
 * isis_dump_packet
 *
 * Dump a packet via buginf.  Dumps starting at pak->datagramstart.
 */
void isis_dump_packet (paktype *pak)
{
    uchar *debug_ptr;
    ulong debug_byte, debug_length;

    debug_length = pak->datagramsize;
    debug_ptr = pak->datagramstart;
    for (debug_byte = 0; debug_byte < debug_length; debug_byte++) {
	if ((debug_byte % 16) == 0) {
	    buginf("\n  %03x  ", debug_byte);
	}
	buginf(" %02x", *debug_ptr);
	debug_ptr++;
    }
}


/*
 * isis_encaps
 *
 *    Convert the snpa target type into a linktype, which the drivers 
 *    know about.
 *
 *    Called only by IS-IS;  NLSP uses other means.
 */
boolean isis_encaps (
    paktype *pak,
    int length,
    idbtype *idb,
    hwaddrtype *snpa)
{
    pak->if_output = idb;
    pak->acct_proto = ACCT_PROTO_CLNS;

    switch (snpa->target_type) {
    case TARGET_UNICAST:
	pak->linktype = LINK_CLNS;
	break;

    case TARGET_BROADCAST:
	pak->linktype = LINK_CLNS_BCAST;
	break;

    case TARGET_OSI_ALL_IS:
	pak->linktype = LINK_CLNS_ALL_IS;
	break;

    case TARGET_OSI_ALL_L1:
	pak->linktype = LINK_ISIS_ALL_L1_IS;
	break;

    case TARGET_OSI_ALL_L2:
	pak->linktype = LINK_ISIS_ALL_L2_IS;
	break;

    default:
	if (isis_adj_packet_debug || isis_update_packet_debug) {
	    buginf("\nISIS: invalid encap target type for if: %s", 
		   idb->namestring);
	}
	return(FALSE);
    }

    /*
     * Call interface encapsulator
     */
    pak->datagramstart = (uchar *)isisheadstart(pak);
    pak->datagramsize = length;
    return((*idb->hwptr->vencap)(pak, (long)snpa->addr));
}


/*
 * isis_union_areas
 *
 * Add a learned area to the set of active areas.
 *
 */
void isis_union_areas (clns_pdbtype *pdb, uchar *area)
{

    uchar *largest;
    int   i, index;

    /*
     * Area already in list of acceptable areas.
     */
    for (i = 0; i < pdb->num_active_areas; i++) {
	if (nsap_match (area, pdb->active_area_address [i])) return;
    }

    index = pdb->num_active_areas;

    /*
     * Go do tie breaking. Largest area address gets dropped from 
     * list.  Complain bitterly if this happens.
     */
    if (pdb->num_active_areas == MAX_AREA_ADDRESSES) {
	largest = area;
	index = -1;
	for (i = 0; i < pdb->num_active_areas; i++) {
	    if (compare_area_address(largest, 
				     pdb->active_area_address [i]) < 0) {
		largest = pdb->active_area_address [i];
		index = i;
	    }
	}

	/* The area address on this call is the largest, ignore it. */
	if (index == -1) return;

	/* Going to replace an entry. Keep number of entries the same. */
	pdb->num_active_areas--;
	errmsg(&msgsym(AREALOST, CLNS), 
	       nsap_string(pdb->active_area_address[index]));
    }

    nsap_copy (area, pdb->active_area_address [index]);
    pdb->num_active_areas++;

}

/*
 * isis_valid_packet
 *
 * Validate a packet and note the first occurrence of each TLV
 * code.
 */
isis_TLV_parse *isis_valid_packet (paktype *pak, void *isis_ptr,
				    clns_pdbtype *pdb)
{
    isis_TLV_parse *tlv_parse;
    int header_length, bytesleft;
    int pdu_length;
    int wire_length;
    isishdrtype *isis;
    isis_pt_IIH_hdrtype *ser_IIH;
    isis_lan_IIH_hdrtype *lan_IIH;
    isis_LSP_hdrtype *lsp;
    isis_CSNP_hdrtype *csnp;
    isis_PSNP_hdrtype *psnp;
    isis_info_type *opt_ptr;
    boolean ok_for_nlsp;
    char *packet_type;
    char error_type[80];	/* Plenty long, but be careful! */
    char error_info[80];	/* Ditto. */
    hwaddrtype snpa;
    ulong circ_type;
    
    isis = isis_ptr;
    lsp = isis_ptr;
    packet_type = "unknown";
    error_type[0] = '\0';
    error_info[0] = '\0';
    tlv_parse = malloc(sizeof(isis_TLV_parse));
    if (tlv_parse == NULL)
	return(NULL);			/* No memory */

    /*
     * First check fields that are invariant across PDU types.
     * We make an initial length check just to make sure we're not
     * walking off of the end of the packet;  we refine the length
     * check later.
     */

    wire_length = isis_pak_length(pak, pdb);
    if (wire_length < ISIS_MINIMAL_HEADER_SIZE) {
	sprintf(error_type, "header size %d too small", wire_length);
 	goto bad_packet;
    }

    /* PDU version number must be right. */

    if (isis->version_pdu != ISIS_VERSION_LSP) {
	sprintf(error_type, "bad version %d", isis->version_pdu);
 	goto bad_packet;
    }

    /* Fields ignored by NLSP... */

    if (pdb->proc_type == PROC_TYPE_ISIS) {

	/* Protocol version number. */

	if (isis->version_pid != ISIS_VERSION) {
	    sprintf(error_type, "bad version_pid %d", isis->version_pid);
	    goto bad_packet;
	}

	/* ID length must be right. */

	if (isis->id_length != 0 && isis->id_length != STATION_LENGTH) {
	    sprintf(error_type, "bad id_length %d", isis->id_length);
	    goto bad_packet;
	}

	/* Max area addresses must be right. */

	if (isis->max_area_addresses != 0 && 
	    isis->max_area_addresses != MAX_AREA_ADDRESSES) {
	    sprintf(error_type, "bad max_area_addresses %d",
		    isis->max_area_addresses);
	    goto bad_packet;
	}
    }

    /* Check out the type-specific stuff. */

    ok_for_nlsp = TRUE;

    switch (isis->type) {

    case ISIS_LAN_L2_HELLO:
	ok_for_nlsp = FALSE;
	/* Fall through */
    case ISIS_LAN_L1_HELLO:

	/* LAN Hellos:  Validate circuit type.  NLSP is L1 only. */

	if (isis->type == ISIS_LAN_L1_HELLO) {
	    packet_type = "LAN L1 hello";
	} else {
	    packet_type = "LAN L2 hello";
	}
	lan_IIH = isis_ptr;
	circ_type = lan_IIH->circuit_type;
	if (circ_type == 0) {
	  bad_circtype:
	    sprintf(error_type, "bad circuit type %d", circ_type);
	    goto bad_packet;
	}
	if (pdb->proc_type == PROC_TYPE_NLSP && 
	    lan_IIH->circuit_type == ISIS_CIRCUIT_L2)
	    goto bad_circtype;
	header_length = ISIS_LAN_HELLO_HEADER_SIZE;
	pdu_length = GETSHORT(lan_IIH->pdu_length);
	break;

    case ISIS_PT_HELLO:
	/*
	 * WAN Hellos:  Validate circuit type.  NLSP special casing is
	 * done in NLSP adjacency processing, since we need to tear
	 * down an existing adjacency if an L2 circuit value is seen.
	 */
	packet_type = "P2P hello";
	ser_IIH = isis_ptr;
	circ_type = ser_IIH->circuit_type;
	if (circ_type == 0)
	    goto bad_circtype;
	header_length = ISIS_PT_HELLO_HEADER_SIZE;
	pdu_length = GETSHORT(ser_IIH->pdu_length);
	break;

    case ISIS_L2_LSP:
	ok_for_nlsp = FALSE;
	/* Fall through */
    case ISIS_L1_LSP:
	/*
	 * LSPs.  Validate the IS type.  We only check for NLSP, because
	 * this is broken in our deployed ISIS!
	 */
	if (isis->type == ISIS_L1_LSP) {
	    packet_type = "L1 LSP";
	} else {
	    packet_type = "L2 LSP";
	}
	if (pdb->proc_type == PROC_TYPE_NLSP) {
	    if (lsp->is_type != ISIS_CIRCUIT_L1 && 
		lsp->is_type != ISIS_CIRCUIT_L1L2) {
		sprintf(error_type, "Bad IS type %d", lsp->is_type);
		goto bad_packet;
	    }
	}
	header_length = ISIS_LSP_HEADER_SIZE;
	pdu_length = GETSHORT(lsp->pdu_length);
	break;

    case ISIS_L2_CSNP:
	ok_for_nlsp = FALSE;
    case ISIS_L1_CSNP:
	csnp = isis_ptr;
	header_length = ISIS_CSNP_HEADER_SIZE;
	pdu_length = GETSHORT(csnp->pdu_length);
	break;

    case ISIS_L2_PSNP:
	ok_for_nlsp = FALSE;
    case ISIS_L1_PSNP:
	psnp = isis_ptr;
	header_length = ISIS_PSNP_HEADER_SIZE;
	pdu_length = GETSHORT(psnp->pdu_length);
	break;

    default:
	sprintf(error_type, "unknown packet type %d", isis->type);
	goto bad_packet;
    }

    /* Bite off if packet type isn't valid for NLSP. */

    if (pdb->proc_type == PROC_TYPE_NLSP && (!ok_for_nlsp)) {
	sprintf(error_type, "unknown packet type %d", isis->type);
	goto bad_packet;
    }

    /* Header length must be right. */

    if (isis->hlen != header_length) {
	sprintf(error_type, "Bad header length %d, should be %d",
		isis->hlen, header_length);
	goto bad_packet;
    }

    /* 
     * PDU length must be >= header length and fit within datagram.  For
     * NLSP, the "wire length" (really the IPX length) must exactly match
     * the NLSP length.
     */

    if (pdu_length < header_length) {
	sprintf(error_type, "packet length %d less than header length %d",
		pdu_length, header_length);
 	goto bad_packet;
    }
    if (pdb->proc_type == PROC_TYPE_NLSP) {
	if (pdu_length != wire_length) {
	    sprintf(error_type, "packet (%d) or wire (%d) length invalid",
		    pdu_length, wire_length);
	    goto bad_packet;
	}
    } else {
	if (pdu_length > wire_length) {
	    sprintf(error_type, "packet (%d) or wire (%d) length invalid",
		    pdu_length, wire_length);
	    goto bad_packet;
	}
    }

    bytesleft = pdu_length - header_length;


    opt_ptr = (isis_info_type *)(((uchar *)isis) + header_length);
    
    /* Run down the options to make sure they're kosher. */
    
    while (bytesleft >= ISIS_INFO_TYPE_OVERHEAD) {
	if (tlv_parse->TLV_array[opt_ptr->code] == NULL)
	    tlv_parse->TLV_array[opt_ptr->code] = opt_ptr;
 	if (opt_ptr->length + ISIS_INFO_TYPE_OVERHEAD > bytesleft) {
	    sprintf(error_type, "option %x length %d > remaining bytes (%d)",
		    opt_ptr->code, opt_ptr->length + ISIS_INFO_TYPE_OVERHEAD,
		    bytesleft);
 	    goto bad_packet;
	}

	/* Now do type-specific validation. */

	switch (opt_ptr->code) {


	    /* SNPs have to be multiples of the entry length. */

	  case ISIS_SNP:
	    if (opt_ptr->length % ISIS_SNP_ENTRY_LENGTH) {
		sprintf(error_type, "Bad SNP option length %d",
			opt_ptr->length);
		goto bad_packet;
	    }
	    break;


	    /* Other types are handled by protocol-specific code. */

	  default:
	    if (pdb->proc_type == PROC_TYPE_NLSP) {
		if (!reg_invoke_ipx_nlsp_validate_tlv(pdb, opt_ptr, isis_ptr,
						 error_type)) {
		    goto bad_packet;
		}
	    }
	}

	/* Move on to the next one. */

 	bytesleft -= opt_ptr->length + ISIS_INFO_TYPE_OVERHEAD;
	ADVANCE_ITEM(opt_ptr, TRUE);
    }
    
    if (bytesleft != 0) {		/* Partial TLV at end */
	sprintf(error_type, "Incomplete option, remaining length %d, "
		"first byte %x", bytesleft, *((uchar *) opt_ptr));
 	goto bad_packet;
    }
    
    return(tlv_parse);			/* Success! */


    bad_packet:
    if (isis->type == ISIS_L1_LSP || isis->type == ISIS_L2_LSP) {
	sprintf(error_info, ", ID %s, seq %d, ht %d",
		lspid_string(&lsp->lspid), GETLONG(&lsp->sequence_number),
		GETSHORT(lsp->ht));
    }
    (*pak->if_input->hwptr->extract_snpa)(pak, &snpa, SOURCE);
    errmsg(&msgsym(BADPACKET, CLNS), pdb->errmsg_name, packet_type, error_type,
	   error_info, snpa_string(&snpa), pak->if_input->namestring);
    if (isis_nlsp_proto_error_debug[pdb->proc_type]) {
	buginf("\n%s: %s, %s%s from %s (%s)", pdb->errmsg_name, packet_type,
	       error_type, error_info, snpa_string(&snpa),
	       pak->if_input->namestring);
	isis_dump_packet(pak);
    }
    free(tlv_parse);
    return(NULL);
}
