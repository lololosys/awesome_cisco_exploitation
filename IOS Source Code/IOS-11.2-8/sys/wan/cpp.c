/* $Id: cpp.c,v 3.1.2.6 1996/08/28 13:20:56 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/cpp.c,v $
 *------------------------------------------------------------------
 * cpp.c -- routines for the cpp proprietary protocol.
 *
 * September, 1995, Bill May
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cpp.c,v $
 * Revision 3.1.2.6  1996/08/28  13:20:56  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.5  1996/06/27  09:44:31  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.1.2.4  1996/06/06  22:51:50  wmay
 * CSCdi58782:  down line stays stuck looped - clear inloopback field when
 * changing state
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/22  19:06:46  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * Connectted -> Connected
 *
 * Revision 3.1.2.2  1996/05/13  14:47:55  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/10  01:44:22  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/09  17:24:35  wmay
 * Placeholder for California branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include <ciscolib.h>
#include <string.h>
#include "globs.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "../wan/dialer_registry.h"
#include "../les/isdn_bri.h"
#include "../if/network.h"
#include "ttysrv.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "dialer.h"
#include "dialer_private.h"
#include "ieee.h"
#include "../if/ether.h"
#include "../if/arp.h"
#include "aaa.h"
#include "login.h"
#include "../ui/debug.h"
#include "sched.h"
#include "../os/signal.h"
#include "mgd_timers.h"
#include "../wan/isdn.h"
#include "../wan/cpp_debug.h"
#include "../wan/cpp.h"

/*
 * Local Storage
 */

static mgd_timer cpp_master_timer, rx_sequence_timer, cpp_neg_timer;
static watched_queue *CpppakQ;
static int cpp_fork;

/*
 * CPP special is the ether mac address for special cpp commands
 */
static const uchar CPP_SPECIAL[] = {0x00, 0x40, 0xf9, 0x00, 0x00, 0x01};

/*
 * cpp_proto_supported
 * Tells which cpp transmission types that we support
 */
static boolean cpp_proto_supported (enum cpp_protos proto)
{
    switch (proto) {
    case CPP_PROTO_COMP_ORDERED:	return(FALSE);
    case CPP_PROTO_ORDERED:		return(TRUE);
    case CPP_PROTO_HDLC:		return(TRUE);
    case CPP_PROTO_FRAG:		return(FALSE);
    case CPP_PROTO_COMP_FRAG:		return(FALSE);
    }
    return(FALSE);
}


static void dump_packet_len (paktype *pak, boolean whole)
{
    int temp, end;
    end = pak->datagramsize;
    if (whole == FALSE && end > 32) end = 32;
    
    for (temp = 0; temp < end; temp++) {
	if ((temp % 16) == 0)
	    buginf("\n");
	buginf("%02x ", pak->datagramstart[temp]);
    }
}

#define dump_packet(a) dump_packet_len(a, FALSE)
/*
 * cpp_enqueue
 * Interrupt level packet enqueue.  Put on the watched queue.
 */
static void cpp_enqueue (paktype *pak)
{
    process_enqueue_pak(CpppakQ, pak);
}


/*
 * cpp_getlink
 * get link type for an cpp datagram.
 */
static long cpp_getlink (paktype *pak)
{
    /*
     * If fr_flags is set, we have a cpp packet that hasn't
     * been decoded.  If not, decode it exactly like an ethernet packet
     */
    if (pak->fr_flags == 0) {
	return(ether_getlink(pak));
    }
    
    return(LINK_CPP);
}

/*
 * cpp_parse_packet
 *
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 * If we have a input type that we need to decode, we will set
 * fr_flags in the pak structure to 1.  This indicates to type2link
 * that we are a cpp packet, so will go on the cpp queue.
 */
static iqueue_t cpp_parse_packet (hwidbtype *idb, paktype *pak, uchar *data)
{
    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (idb->cpp_info == NULL)
	return(NULL);
    
    /*
     * Grab the encapsulation type, and figure out the encapsulation size.
     */
    pak->enctype    = ET_CPP;
    pak->rif_start  = NULL;
    pak->riflen     = 0;
    pak->info_start = NULL;
    pak->addr_start = NULL;
    pak->mac_start  = NULL;

    switch (idb->cpp_info->protocol) {
    case CPP_PROTO_HDLC:
	pak->network_start = pak->datagramstart;
	pak->encsize = 0;
	pak->fr_flags = 0;
	break;
    case CPP_PROTO_COMP_ORDERED:
    case CPP_PROTO_ORDERED:
	pak->network_start = pak->datagramstart + CPP_ORDERED_ENCAPBYTES;
	pak->encsize = CPP_ORDERED_ENCAPBYTES;
	pak->fr_flags = 1;
	break;
    case CPP_PROTO_FRAG:
    case CPP_PROTO_COMP_FRAG:
	/* I don't know what to do with this yet */
	return(NULL);
    }
    return(idb->iqueue);
} 

/*
 * The below routines reset the cpp info field that hangs off
 * the idb.  This info is about the current call.
 */
static void cpp_reset_remote_isdn (remote_isdn_type *r)
{
    if (r->number) {
	free(r->number);
	r->number = NULL;
    }
    r->subaddr_len = 0;
}

static void cpp_reset_info (cpp_info_type *cinfo)
{
    cpp_reset_remote_isdn(&cinfo->ports[0]);
    cpp_reset_remote_isdn(&cinfo->ports[1]);
    cinfo->neg_pak_rcvd = FALSE;
    cinfo->negotiation_complete = FALSE;
    cinfo->protocol = CPP_PROTO_HDLC;
    cinfo->remote_name[0] = '\0';
    cinfo->seen_keepalive = FALSE;
    cinfo->keep_periods_missed = 0;
    cinfo->remote_user_info = NULL;
}

/*********************************************************************
 *              CPP NEGOTIATION ROUTINES                             *
 *********************************************************************/
/*
 * cpp_neg_complete
 * This routine is called when the negotiation is completed and successful.
 * At this point, the line protocol is up.
 */
static void cpp_neg_complete (hwidbtype *hwidb,
			      cpp_info_type *cinfo)
{
    mgd_timer_stop(&cinfo->neg_timer);
    cinfo->negotiation_complete = TRUE;
    if (cinfo->remote_name[0] != '\0')
	reg_invoke_dialer_remote_name(hwidb, cinfo->remote_name);
    reg_invoke_media_protocol_up(hwidb, LINK_ALL);
}

/*
 * cpp_copy_subaddr.  This will copy the isdn subaddress part of
 * the number to the cpp negotiation pak.  The isdn address is in
 * the form <number>:<subaddr>
 * We copy it to the form <len><data> and return the len. (plus 1,
 * for the length of the field).
 */
static ushort cpp_copy_subaddr (uchar *number, uchar *fptr)
{
    ushort len;
    char *check;
    len = 0;
    if (number != NULL) {
	for (check = number; *check != ':' && *check != '\0'; check++);
	if (*check == ':') {
	    check++;
	    len = strlen(check);
	    strncpy(fptr + 1, check, len);
	}
    }
    *fptr = len;
    return(len + 1);
}

/*
 * create_and_send_cpp_inband_pak
 * This will create the inband packets that cpp needs to talk
 * back and forth with.
 * This includes keepalives and compression resets
 */
static void create_and_send_cpp_inband_pak (hwidbtype *hwidb,
					    ulong fields,
					    uchar disconnect_reason)
{
    paktype *pak;
    cpp_info_type *cinfo;
    ether_hdr *ether;
    ushort *lenptr, len;
    uchar *fptr;
    hwidbtype *config_hwidb;
    uchar *number1, *number2;

    config_hwidb = get_netidb(hwidb);
    
    cinfo = hwidb->cpp_info;
    pak = getbuffer(255);
    if (pak == NULL)
	return;
    pak->if_output = hwidb->firstsw;
    ether =
	(ether_hdr *)pak->datagramstart =
	    pak->network_start - sizeof(ether_hdr);

    /*
     * create ethernet header.  Address it correctly if we have the
     * remote address - otherwise us the cpp special.
     */
    if (cinfo->neg_pak_rcvd || cinfo->negotiation_complete) {
	ieee_copy(cinfo->remote_addr, ether->daddr);
    } else {
	ieee_copy(CPP_SPECIAL, ether->daddr);
    }
    /* Get source address - always use default_mac_addr */
    ieee_copy(default_mac_addr, ether->saddr);
    ether->type_or_len = TYPE_CPP;
    /*
     * Add cpp header
     */
    PUTCPPSHORT(&ether->data[CPP_NEG_CPROTO], CPP_CPROTO);
    ether->data[CPP_NEG_VERSION] = 2;
    lenptr = (ushort *)&ether->data[CPP_NEG_LENGTH];
    fptr = &ether->data[CPP_NEG_FIELDS];

    if (fields & (CPP_SEND_PORTS)) {
	number1 = isdn_get_number(hwidb, 0);
	number2 = isdn_get_number(hwidb, 1);
    } else {
	number1 = NULL;
	number2 = NULL;
    }
    /*
     * Now put on the fields requested in the fields bitfield
     */
    len = 0;
    if (fields & CPP_SEND_PROTOCOL) {
	*fptr++ = CPP_NEG_PROTOCOL;
	*fptr++ = 1;
	if (cinfo->neg_pak_rcvd)
	    *fptr++ = cinfo->protocol;
	else
	    *fptr++ = CPP_SUPPORTED_PROTO;
	len += 3;
    }
    if (fields & CPP_SEND_PROTO_LIST) {
	*fptr++ = CPP_NEG_PROTO_LIST;
	*fptr++ = 2;
	*fptr++ = CPP_PROTO_ORDERED;
	*fptr++ = CPP_PROTO_HDLC;
	len += 4;
    }
    if (fields & CPP_SEND_PASSWORD) {
	uchar templen;
	
	fptr[0] = CPP_NEG_PASSWORD;
	if (find_passwd(hostname, &fptr[2])) {
	    fptr[1] = templen = strlen(&fptr[2]);
	} else {
	    fptr[1] = templen = 0;
	}
	len += 2 + templen;
	fptr += 2 + templen;
    }

    if (fields & CPP_SEND_ETHERNET_ID) {
	*fptr++ = CPP_NEG_ETHERNET_ID;
	*fptr++ = IEEEBYTES;
	ieee_copy(default_mac_addr, fptr);
	fptr += IEEEBYTES;
	len += IEEEBYTES + 2;
    }

    if (fields & CPP_SEND_PORT_1_NUMBER) {
	ushort temp;
	uchar *check;
	*fptr++ = CPP_NEG_PORT_1_NUMBER;
	if (number1 && number1[0] != '\0') {
	    for (check = number1; *check != ':' && *check != '\0'; check++);
	    temp = check - number1;
	    strncpy(fptr + 1, number1, temp);
	} else
	    temp = 0;
	*fptr++ = temp;
	fptr += temp;
	len += temp + 2;
    }
    if (fields & CPP_SEND_PORT_2_NUMBER) {
	ushort temp;
	uchar *check;
	*fptr++ = CPP_NEG_PORT_2_NUMBER;
	if (number2 && number2[0] != '\0') {
	    for (check = number2; *check != ':' && *check != '\0'; check++);
	    temp = check - number2;
	    strncpy(fptr + 1, number2, temp);
	} else
	    temp = 0;
	*fptr++ = temp;
	fptr += temp;
	len += temp + 2;
    }

    if (fields & CPP_SEND_ORIG_PORT) {
	*fptr++ = CPP_NEG_ORIG_PORT;
	*fptr++ = 1;
	if (hwidb->isdn_info)
	    *fptr++ = hwidb->isdn_info->chantype + 1;
	else
	    *fptr++ = 1;
	len += 3;
    }

    if (fields & CPP_SEND_DISCONNECT) {
	*fptr++ = CPP_NEG_DISCONNECT;
	*fptr++ = 1;
	*fptr++ = disconnect_reason;
	len += 3;
    }
    if (fields & CPP_SEND_COMPLETE) {
	*fptr++ = CPP_NEG_COMPLETE;
	*fptr++ = 0;
	len += 2;
    }
    if (fields & CPP_SEND_LINE_INTEGRITY) {
	*fptr++ = CPP_NEG_LINE_INTEGRITY;
	*fptr++ = 1;
	*fptr++ = hwidb->keep_period;
	len += 3;
    }
    if (fields & CPP_SEND_RESET_COMP) {
	*fptr++ = CPP_NEG_RESET_COMP;
	*fptr++ = 1;
	*fptr++ = disconnect_reason;
	len += 3;
    }
    if (fields & CPP_SEND_IP_ADDRESS) {
	*fptr++ = CPP_NEG_IP_ADDRESS;
	*fptr++ = sizeof(ulong);
	if (config_hwidb->firstsw->ip_unnumbered)
	    PUTCPPLONG(fptr,
		       config_hwidb->firstsw->ip_unnumbered->ip_address);
	else
	    PUTCPPLONG(fptr, config_hwidb->firstsw->ip_address);
	fptr += sizeof(ulong);
	len += sizeof(ulong) + 2;
    }

    if (fields & CPP_SEND_PORT_1_SUBADDR) {
	ushort temp;
	*fptr++ = CPP_NEG_PORT_1_SUBADDR;
	temp = cpp_copy_subaddr(number1, fptr);
	fptr += temp;
	len += temp + 1;
    }
    
    if (fields & CPP_SEND_PORT_2_SUBADDR) {
	ushort temp;
	*fptr++ = CPP_NEG_PORT_2_SUBADDR;
	temp = cpp_copy_subaddr(number2, fptr);
	fptr += temp;
	len += temp + 1;
    }
    
    if (fields & CPP_SEND_REMOTE_NAME) {
	int str_len;
	*fptr++ = CPP_NEG_REMOTE_NAME;
	str_len = strlen(hostname);
	if (str_len >= 8)
	    str_len = 7;
	*fptr++ = str_len;
	fptr = strncpy(fptr, hostname, str_len);
	len += str_len + 2;
    }
    /*
     * Put length of fields into cpp header
     * set datagramsize
     */
    PUTCPPSHORT(&ether->data[CPP_NEG_LENGTH], len);
    pak->datagramsize = len + sizeof(ether_hdr) + CPP_NEGBYTES;
    if (cinfo->negotiation_complete) {
	/* add header if we are protocol up  */
        if (cinfo->protocol != CPP_PROTO_HDLC) {
	    pak->datagramstart -= 2;
	    pak->datagramsize += 2;
	    PUTCPPSHORT(pak->datagramstart, CPP_INBAND_SEQ_NO);
	}
    }

    if (cpp_packet_debug) {
	buginf("\n%s:sending packet - len %d", hwidb->hw_short_namestring,
	       pak->datagramsize);
	dump_packet_len(pak, TRUE);
    }

    /*
     * add the board encap (if needed), and output packet
     */
    if (hwidb->board_encap) {
	(*hwidb->board_encap)(pak, hwidb);
    }
    datagram_out(pak);
}

/*
 * cpp_add_remote_number
 * returns pointer after we are done.
 * Takes the isdn from a cpp negotiation packet, and stores it
 */
static uchar *cpp_add_remote_number (remote_isdn_type *rptr, uchar *fptr)
{
    int len;

    len = *fptr++;
    if (len == 0)
	return(fptr);
    
    if (rptr->number) {
	if (strncmp(rptr->number, fptr, len) == 0)
	    return(fptr + len);		/* same string - no copy*/
	free(rptr->number); 		/* else get rid of old numbers */
    }
    rptr->number = malloc(len + 1);
    memcpy(rptr->number, fptr, len);
    rptr->number[len] = '\0';
    
    return(fptr + len);
}

/*
 * cpp_add_remote_subaddr
 * Takes the isdn sub address from a negotiation packet
 */
static uchar *cpp_add_remote_subaddr (remote_isdn_type *rptr, uchar *fptr)
{
    int len;
    uchar *end;

    len = *fptr++;
    if (*fptr <= 10) {
       end = memcpy(rptr->subaddress, fptr, len);
       *end++ = '\0';
       rptr->subaddr_len = len;
   }
    
    return(fptr + len);
}

/*
 * cpp_check_password
 * We check the hostname, then will check the ethernet address as
 * possible hostnames.  Hopefully, someone will make up a registry
 * for us to call here.
 */
static boolean cpp_check_password (uchar *password,
				   cpp_info_type *cinfo)
{
    usernametype *info;
    enum password_check_type check_it;

    check_it = check_local_password(cinfo->remote_name, password, &info);
    if (check_it == PASSWORD_CORRECT ) {
	if (cpp_negotiation_debug)
	    buginf("\n   password is correct");
	/* save this for callback futures */
	cinfo->remote_user_info = info;
	return(FALSE);
    }
    if (cpp_negotiation_debug) {
	switch (check_it) {
	case PASSWORD_NO_USER:
	    buginf("\n   no username %s", cinfo->remote_name);
	    break;
	case PASSWORD_NO_PASSWORD:
	    buginf("\n   no password for user %s", cinfo->remote_name);
	    break;
	case PASSWORD_NOT_CORRECT:
	    buginf("\n   incorrect password for user %s", cinfo->remote_name);
	    break;
	default:
	    break;
	}
    }
		
    return(TRUE);
}

static const char *cpp_decode_disconnect (uchar type)
{
    switch (type) {
    case CPP_ERR_PASSWORD_BAD:   return("bad password");
    case CPP_ERR_NO_CALLBACK_NO: return("No callback number");
    case CPP_ERR_BAD_ETHER:	 return("bad ethernet address");
    case CPP_ERR_CALL_YOU_BACK:  return("call you back");
    case CPP_ERR_BAD_PACKET:	 return("bad packet");
    case CPP_ERR_NO_PROTOS:	 return("no protocols in common");
    case CPP_ERR_TIMEOUT:	 return("inband timed out");
    case CPP_ERR_LIV:		 return("line integrity violation");
    case CPP_ERR_UNSPECIFIED:	 return("unspecified");
    }
    return("Unknown");
}

/*
 * cpp_negotiate
 * This routine will handle incoming cpp negotiation packets
 */
static void cpp_negotiate (hwidbtype *hwidb, paktype *pak)
{
    cpp_info_type *cinfo;
    ether_hdr *ether;
    short field_len, password_len;
    uchar *fptr, f_type;
    uchar password[8];
    boolean check_proto_list, have_error, i_answered;
    uchar error_type;
    ulong send_to_remote;
    dialerdbtype *ddb;

    if (cpp_event_debug || cpp_packet_debug)
	buginf("\n%s:(I) NEG packet - len %d", hwidb->hw_short_namestring,
	       pak->datagramsize);
    if (cpp_packet_debug) {
	dump_packet_len(pak, TRUE);
    }
    cinfo = hwidb->cpp_info;

    /*
     * check some relevant data, like the incoming mac address,
     * that the types are correct, etc
     */
    ether = (ether_hdr *)pak->datagramstart;
    if ((GETSHORT(&ether->type_or_len) !=  TYPE_CPP) ||
	(GETCPPSHORT(&ether->data[CPP_NEG_CPROTO]) != CPP_CPROTO)) {
	create_and_send_cpp_inband_pak(hwidb, CPP_SEND_DISCONNECT,
				       CPP_ERR_BAD_PACKET);
	datagram_done(pak);
	return;
    }

    if (!ieee_equal(ether->daddr, CPP_SPECIAL) &&
	!ieee_equal(ether->daddr, default_mac_addr)) {
	create_and_send_cpp_inband_pak(hwidb, CPP_SEND_DISCONNECT,
				       CPP_ERR_BAD_ETHER);
	datagram_done(pak);
	return;
    }

    /*
     * Okay - begin getting the relevant data
     */
    cinfo->cpp_neg_version = ether->data[CPP_NEG_VERSION];
    ieee_copy(ether->saddr, cinfo->remote_addr);
    field_len = GETCPPSHORT(&ether->data[CPP_NEG_LENGTH]);
    
    password[0] = '\0';
    password_len = 0;
    check_proto_list = FALSE;
    have_error = FALSE;
    error_type = CPP_ERR_BAD_PACKET;
    send_to_remote = CPP_SEND_PROTOCOL | CPP_SEND_ETHERNET_ID |
	CPP_SEND_IP_ADDRESS | CPP_SEND_REMOTE_NAME;
    
    fptr = &ether->data[CPP_NEG_FIELDS];
    while ((have_error == FALSE) && (field_len > 0)) {
	f_type = *fptr++;
	field_len--;	/* for field type */
	switch (f_type) {
	case CPP_NEG_PORT_1_NUMBER:
	    field_len -= *fptr + 1;
	    if (field_len >= 0)
		fptr = cpp_add_remote_number(&cinfo->ports[0], fptr);
	    else 
		have_error = TRUE;
	    send_to_remote |= CPP_SEND_PORT_1_NUMBER;
	    if (cpp_negotiation_debug)
		buginf("\n   port 1 number:%s", cinfo->ports[0].number);
	    break;
	case CPP_NEG_PORT_2_NUMBER:
	    field_len -= *fptr + 1;
	    if (field_len >= 0)
		fptr = cpp_add_remote_number(&cinfo->ports[1], fptr);
	    else
		have_error = TRUE;
	    send_to_remote |= CPP_SEND_PORT_2_NUMBER;
	    if (cpp_negotiation_debug)
		buginf("\n   port 2 number:%s", cinfo->ports[0].number);
	    break;
	case CPP_NEG_ORIG_PORT:
	    if (*fptr++ != 1) {
		have_error = TRUE;
	    } else {
		cinfo->originating_port = *fptr++;
	    }
	    field_len -= 2;
	    if (cpp_negotiation_debug)
		buginf("\n   origination port:%d", cinfo->originating_port);
	    break;
	case CPP_NEG_PROTOCOL:
	    if (*fptr++ == 1) {
		enum cpp_protos protos;
		protos = *fptr++;
		if (cpp_proto_supported(protos))
		    cinfo->protocol = protos;
		else
		    check_proto_list = TRUE;
		field_len -= 2;
		if (cpp_negotiation_debug)
		    buginf("\n   attempting proto:%d", protos);
	    } else
		have_error = TRUE;
	    break;
	case CPP_NEG_PROTO_LIST: {
	    ushort ix, len;
	    len = *fptr++;
	    field_len--;
	    for (ix = 0; ix < len; ix++, field_len--, fptr++) {
		if ((check_proto_list) &&
		    (cpp_proto_supported((enum cpp_protos) *fptr))) {
		    cinfo->protocol = (enum cpp_protos)*fptr;
		    check_proto_list = FALSE;
		}
	    }
	    if (check_proto_list) {
		have_error = TRUE;
		error_type = CPP_ERR_NO_PROTOS;
	    }
	}
	    break;
	case CPP_NEG_PASSWORD:
	    password_len = *fptr++;
	    field_len--;
	    if (password_len) {
		if (password_len < 8) {
		    sstrncpy(password, fptr, password_len + 1);
		}
		fptr += password_len;
		field_len -= password_len;
	    }
	    break;
	case CPP_NEG_ETHERNET_ID:
	    if (*fptr++ == IEEEBYTES) {
		field_len -= (IEEEBYTES + 1);
		ieee_copy(fptr, cinfo->remote_addr);
		fptr += IEEEBYTES;
	    } else
		have_error = TRUE;
	    if (cpp_negotiation_debug)
		buginf("\n   ether id:%e", cinfo->remote_addr);
	    break;
	case CPP_NEG_DISCONNECT:
	    if (*fptr++ == 1) {
		if (cpp_negotiation_debug) {
		    buginf("\n   disconnect %s received",
			   cpp_decode_disconnect(*fptr));
		}
		/* Disconnect here ? */
		if (*fptr++ != CPP_ERR_CALL_YOU_BACK) {
		    datagram_done(pak);
		    return;
		}
	    }
	    break;
	case CPP_NEG_COMPLETE:
	    field_len -= 2;
	    if ((field_len > 0) ||
		(cinfo->neg_pak_rcvd == FALSE)) {
		cpp_reset_info(cinfo);
		datagram_done(pak);
		return;
	    }
	    cpp_neg_complete(hwidb, cinfo);
	    datagram_done(pak);
	    /* check if call back */
	    if (cinfo->accept_callback) {
		if (cpp_event_debug || cpp_negotiation_debug)
		    buginf("\n%s - disconnecting for callback",
			   hwidb->hw_short_namestring);
		reg_invoke_dialer_callback_start(hwidb,
						 cinfo->remote_user_info,
						 TRUE,
						 NULL);
	    }
	    return;
	case CPP_NEG_LINE_INTEGRITY:
	    if (*fptr++ != 1) {
		have_error = TRUE;
	    } else {
#if 0
		cinfo->remote_keeptime = *fptr++;
#else
		fptr++;
#endif
		send_to_remote |= CPP_SEND_LINE_INTEGRITY;
	    }
	    field_len -= 2;
	    break;
	case CPP_NEG_IP_ADDRESS:
	    if (*fptr++ != sizeof(ulong)) {
		have_error = TRUE;
	    } else {
		cinfo->remote_ip_address = (ipaddrtype)GETCPPLONG(fptr);
		field_len -= sizeof(uchar) + sizeof(ulong);
		fptr += sizeof(ulong);
	    }
	    if (cpp_negotiation_debug)
		buginf("\n   ip address:%i", cinfo->remote_ip_address);
	    break;
	case CPP_NEG_PORT_1_SUBADDR:
	    field_len -= *fptr + 1;
	    if (field_len >= 0)
		fptr = cpp_add_remote_subaddr(&cinfo->ports[0], fptr);
	    else
		have_error = TRUE;
	    send_to_remote |= CPP_SEND_PORT_2_NUMBER;
	    break;
	case CPP_NEG_PORT_2_SUBADDR:
	    field_len -= *fptr + 1;
	    if (field_len >= 0)
		fptr = cpp_add_remote_subaddr(&cinfo->ports[1], fptr);
	    else
		have_error = TRUE;
	    send_to_remote |= CPP_SEND_PORT_2_NUMBER;
	    break;
	case CPP_NEG_REMOTE_NAME:
	    if (*fptr >= 8) {
		have_error = TRUE;
	    } else {
		uchar len;
		len = *fptr++;
		field_len --;
		sstrncpy(cinfo->remote_name, fptr,len + 1);
		fptr += len;
		field_len -= len;
		if (cpp_negotiation_debug)
		    buginf("\n   remote name:%s", cinfo->remote_name);
	    }
	    break;
	default:
	    if (cpp_negotiation_debug) {
		buginf("\n   Illegal type %d", f_type);
	    }
	    have_error = TRUE;
	    break;
	}
    }

    ddb = get_dialerdb(hwidb);
    i_answered = reg_invoke_dialer_is_answer(ddb->dialer, ddb);
    
    if (have_error == FALSE) {
	/*
	 * Check the password
	 */
	if (password[0] != '\0') {
	    error_type = CPP_ERR_PASSWORD_BAD;
	    have_error = cpp_check_password(password, cinfo);
	} else {
	    /* if I'm the called party, and they need a password,
	     *  reject here
	     */
	    if (i_answered && cinfo->auth_required) {
		have_error = TRUE;
		error_type = CPP_ERR_PASSWORD_BAD;
		if (cpp_negotiation_debug) {
		    buginf("\n%s:Password required - none received",
			   hwidb->hw_short_namestring);
		}
	    }
	}
    } 
    
    if (have_error) {
	/* send disconnect */
	if (cpp_negotiation_debug)
	    buginf("\n%s-sending disconnect packet %x",
		   hwidb->hw_short_namestring, error_type);
	create_and_send_cpp_inband_pak(hwidb, CPP_SEND_DISCONNECT, error_type);
    } else {
	cinfo->neg_pak_rcvd = TRUE;
	if (i_answered) {
	    /* We answered - send response */
	    if (cinfo->accept_callback) {
		/*
		 * This is really gross, but we'll do it anyway.
		 * We won't send our ethernet address to the remote
		 * side.  This will cause full re-negotiation when
		 * we call them back
		 */
		send_to_remote |= CPP_SEND_DISCONNECT;
		send_to_remote &= ~(CPP_SEND_ETHERNET_ID);
		error_type = CPP_ERR_CALL_YOU_BACK;
	    } else
		error_type = CPP_ERR_NO_ERR;
	    create_and_send_cpp_inband_pak(hwidb, send_to_remote, error_type);
	} else {
	    /*
	     * We called them - send startup message and bring link up
	     */
	    if (cpp_negotiation_debug)
		buginf("\n%s:negotiation complete",hwidb->hw_short_namestring);
	    create_and_send_cpp_inband_pak(hwidb, CPP_SEND_COMPLETE, 0);
	    cpp_neg_complete (hwidb, cinfo);
	}
    }
}

/*
 * cpp_inband
 * process the inband control messages after negotiation
 * has completed.
 */
static void cpp_inband (hwidbtype *hwidb, paktype *pak)
{
    cpp_info_type *cinfo;
    ether_hdr *ether;
    short field_len;
    uchar *fptr, f_type;
    boolean have_error;
    uchar error_type;

    if (cpp_packet_debug) {
	buginf("\n%s:inband packet - len %d", hwidb->hw_short_namestring,
	       pak->datagramsize);
	dump_packet(pak);
    }
    cinfo = hwidb->cpp_info;

    ether = (ether_hdr *)pak->datagramstart;
    if ((GETSHORT(&ether->type_or_len) !=  TYPE_CPP) ||
	(GETCPPSHORT(&ether->data[CPP_NEG_CPROTO]) != CPP_CPROTO)) {
	create_and_send_cpp_inband_pak(hwidb, CPP_SEND_DISCONNECT,
				       CPP_ERR_BAD_PACKET);
	datagram_done(pak);
	return;
    }

    if (!ieee_equal(ether->daddr, CPP_SPECIAL) &&
	!ieee_equal(ether->daddr, default_mac_addr)) {
	create_and_send_cpp_inband_pak(hwidb, CPP_SEND_DISCONNECT,
				       CPP_ERR_BAD_ETHER);
	datagram_done(pak);
	return;
    }

    /*
     * Okay - begin getting the relevant data
     */
    field_len = GETCPPSHORT(&ether->data[CPP_NEG_LENGTH]);
    
    have_error = FALSE;
    error_type = CPP_ERR_BAD_PACKET;

    fptr = &ether->data[CPP_NEG_FIELDS];
    while ((have_error == FALSE) && (field_len > 0)) {
	f_type = *fptr++;
	field_len--;	/* for field type */
	switch (f_type) {
	case CPP_NEG_LINE_INTEGRITY:
	    if (*fptr++ != 1) {
		have_error = TRUE;
	    } else {
		cinfo->seen_keepalive = TRUE;
	    }
	    field_len -= 2;
	    break;
	case CPP_NEG_RESET_COMP:
	    /* asdf  - received compression history reset */
	    break;
	default:
	    have_error = TRUE;
	    break;
	}
    }
    if (have_error) {
	if (cpp_event_debug)
	    buginf("\n%s:Weird inband packet", hwidb->hw_short_namestring);
	create_and_send_cpp_inband_pak(hwidb, CPP_SEND_DISCONNECT,
				       CPP_ERR_BAD_PACKET);
    }
    datagram_done(pak);
}

static void cpp_send_negotiation (hwidbtype *hwidb, cpp_info_type *cinfo)
{
    ulong to_send;
    cinfo->negotiation_times++;
    
    if (cinfo->negotiation_times > CPP_MAX_NEGOTIATION_SENT) {
	if (cpp_negotiation_debug || cpp_event_debug)
	    buginf("\n%s negotiation timed out", hwidb->hw_short_namestring);

	mgd_timer_stop(&cinfo->neg_timer);
	return;
    }
    if (cpp_negotiation_debug || cpp_event_debug)
	buginf("\n%s sending negotiation packet", hwidb->hw_short_namestring);
    to_send = CPP_SEND_ON_UP;
    if (cinfo->auth_required)
	to_send |= CPP_SEND_PASSWORD;
    create_and_send_cpp_inband_pak(hwidb, to_send, 0);
    mgd_timer_start(&cinfo->neg_timer, CPP_NEG_DELAY);
}
/*
 * cpp_cstate
 * If the line has gone down, bring the protocol down.
 * If the line is up, bring up the protocol and start negotiation.
 */
static void cpp_cstate (hwidbtype *hwidb)
{
    cpp_info_type *cinfo;
    dialerdbtype *ddb;

    hwidb->inloopback = FALSE;
    cinfo = hwidb->cpp_info;
    if (hwidb->state == IDBS_UP) {
	cpp_reset_info(cinfo);
	cinfo->negotiation_times = 0;
	ddb = get_dialerdb(hwidb);
	if (!(reg_invoke_dialer_is_answer(ddb->dialer, ddb))) {
	    cpp_send_negotiation(hwidb, cinfo);
	}	
    } else {
	/* clear the negotiation flag so we do it next time line comes up */
	cinfo->negotiation_complete = FALSE;
	mgd_timer_stop(&cinfo->neg_timer);
    }
}

/*
 * cpp_linestate
 * Returns true if the protocol is up
 */
static boolean cpp_linestate (hwidbtype *hwidb)
{
    if (reg_invoke_dialer_spoofing_prot(hwidb))
	return(TRUE);
    else
	return(hwidb->cpp_info &&
	       hwidb->cpp_info->negotiation_complete);
}

/***************************************************************************
 *  Cpp input packet processing
 ***************************************************************************/
/*
 * process_cpp_pak
 * This routine will process a cpp packet that has its headers
 * stripped and has been decompressed and the like.
 */
static void process_cpp_pak (paktype *pak, hwidbtype *hwidb)
{
    ulong link;

    if (cpp_packet_debug) {
	buginf("\n%s:input packet - len %d",
	       hwidb->hw_short_namestring, pak->datagramsize);
	dump_packet(pak);
    }
    /*
     * Parse the packet like it was an ethernet packet
     */
    ether_parse_packet(hwidb, pak, NULL);

    /*
     * Get the link type - we want to set the TBRIDGE flag
     * here in case we are bridging IP or IPX 
     *
     * asdf - Since we already have the linktype and seem to have
     * gone through raw_enqueue already, we could probably just
     * set pak->linktype and reg_invoke_raw_enqueue here.
     */
    link = cpp_getlink(pak);
    switch (link) {
    case LINK_ARP:
    case LINK_IP:
	if (BRIDGING_IF(pak->if_input, LINK_IP))
	    pak->flags |= PAK_TBRIDGE_IN;
	raw_enqueue(hwidb, pak);
	break;
    case LINK_NOVELL:
	/*
	 * Novell packet - I'm not sure if ipx wants to see packets
	 * that I'm bridging, so we go right to raw_enqueue
	 */
	if (BRIDGING_IF(pak->if_input, LINK_NOVELL))
	    pak->flags |= PAK_TBRIDGE_IN;
	raw_enqueue(hwidb, pak);
	break;
    case LINK_APPLETALK:
    case LINK_AARP:
    case LINK_ATALK_SHORT:
	/*
	 * Appletalk varieties.  If we're bridging appletalk, send it
	 * right to net_bridge_enqueue.  Otherwise, raw_enqueue it
	 */
	if (BRIDGING_IF(pak->if_input, LINK_APPLETALK)) {
	    pak->flags |= PAK_TBRIDGE_IN;
	    net_bridge_enqueue(pak);
	} else {
	    raw_enqueue(hwidb, pak);
	}
	break;
    case LINK_CPP:
	/* packet for us - probably a keepalive or a compression restart*/
	cpp_inband(hwidb, pak);
	break;
    default:
	/*
	 * Everything else is a bridging packet
	 */
	pak->flags |= PAK_TBRIDGE_IN;
	net_bridge_enqueue(pak);
	break;
    }
}

/*
 * cpp_pak_insert_before
 * routine that compares packet sequence numbers to put it on the
 * rx_sequence number queue in order.
 */
static boolean cpp_pak_insert_before (paktype *old, void *ptr)
{
    paktype *new;
    ulong old_seq, new_seq;
    hwidbtype *hwidb;
    dialergrouptype *dgptr;
    cpp_group_type *cgptr;
    
    new = (paktype *)ptr;
    hwidb = old->if_input->hwptr;
    dgptr = get_dialergroup(hwidb);
    cgptr = dgptr->cgroup;

    old_seq = GETCPPSHORT(old->datagramstart) & CPP_SEQ_NO_MASK;
    new_seq = GETCPPSHORT(new->datagramstart) & CPP_SEQ_NO_MASK;
    if (new_seq < cgptr->rx_next_sequence) {
	if (old_seq > cgptr->rx_next_sequence)
	    return(FALSE);
    }
    if (old_seq < cgptr->rx_next_sequence) {
	if (new_seq > cgptr->rx_next_sequence)
	    return(TRUE);
    }
    
    if (new_seq < old_seq)
	return(TRUE);
    return(FALSE);
    
}

/*
 * process_cpp_ordered_pak
 * Process a packet with a sequence number.  If we don't have the
 * next sequence number, put it on a queue, and start a timer
 *
 * This routine can handle compressed ordered packets, once I
 * figure out the correct calls to decompress.
 */
static void process_cpp_ordered_pak (paktype *pak, hwidbtype *hwidb)
{
    ushort seq, max_seq;
    dialergrouptype *dgptr;
    cpp_group_type *cgptr;
    boolean do_decompress;

    seq = GETCPPSHORT(pak->datagramstart);
    if (seq == CPP_INBAND_SEQ_NO) {
	/* local packet */
	pak->datagramstart += sizeof(ushort);
	pak->datagramsize -= sizeof(ushort);
	cpp_inband(hwidb, pak);
	return;
    }
    do_decompress = seq & CPP_FLAG_MASK;
    seq &= CPP_SEQ_NO_MASK;
    /*
     * Here we should process the sequence number - get the connected
     * group pointer, and the cpp group pointer
     */
    dgptr = get_dialergroup(hwidb);
    if (dgptr == NULL) {
	datagram_done(pak);
	return;
    }
    cgptr = dgptr->cgroup;
    if (cgptr == NULL) {
	datagram_done(pak);
	return;
    }

    /*
     * If we don't have a sequence number, or we got the next
     * sequence number, we will process this packet
     */
    if (seq == cgptr->rx_next_sequence ||
	cgptr->rx_sequence_valid == FALSE) {
	/*
	 * Increment the sequence number
	 */
	if (cgptr->rx_sequence_valid)  {
	    cgptr->rx_next_sequence =
		INC_CPP_SEQ(cgptr->rx_next_sequence, 1);
	} else {
	    cgptr->rx_next_sequence = INC_CPP_SEQ(seq, 1);
	    cgptr->rx_sequence_valid = TRUE;
	}
	/*
	 * Strip off the sequence number, and decompress
	 */
	pak->datagramstart += sizeof(ushort);
	pak->datagramsize  -= sizeof(ushort);
	if (do_decompress) {
	    /* here's where the key call would be */
	}
	/*
	 * Clear the fr_flags (indicates we have a plain ether packet
	 * for type2link), then process the packet
	 */
	pak->fr_flags = 0;
	process_cpp_pak(pak, hwidb);
	/*
	 * Now check if anything on pak queue.  We do pretty
	 * much the same procedure.
	 */
	for (pak = pak_dequeue(&cgptr->rxQ);
	     pak != NULL;
	     pak = pak_dequeue(&cgptr->rxQ)) {
	    seq = GETCPPSHORT(pak->datagramstart);
	    do_decompress = seq & CPP_FLAG_MASK;
	    seq &= CPP_SEQ_NO_MASK;
	    if (seq != cgptr->rx_next_sequence) {
		mgd_timer_start(&cgptr->rx_sequence_timer, (ONESEC/4));
		pak_requeue(&cgptr->rxQ, pak);
		return;
	    }
	    
	    cgptr->rx_next_sequence = INC_CPP_SEQ(cgptr->rx_next_sequence, 1);
	    pak->datagramstart += sizeof(ushort);
	    pak->datagramsize  -= sizeof(ushort);
	    pak->fr_flags = 0;
	    if (do_decompress) {
	    }
	    process_cpp_pak(pak, hwidb);
	}
	/*
	 * If we got this far, we've stripped the queue.
	 * We stop the sequence timer
	 */
	mgd_timer_stop(&cgptr->rx_sequence_timer);
    } else {
	/*
	 * This packet is not the next packet.
	 * See if we are within a range of sequence numbers.  If so,
	 * place it in the rx_sequence number queue, and start the timer
	 */
	max_seq = INC_CPP_SEQ(cgptr->rx_next_sequence, MAX_CPP_RX_PAK);

	if (((max_seq < cgptr->rx_next_sequence) &&
	     ((max_seq < seq) && (seq < cgptr->rx_next_sequence))) ||
	    ((max_seq > cgptr->rx_next_sequence) &&
	     ((seq < cgptr->rx_next_sequence) || (seq > max_seq)))) {
	    if (cpp_event_debug) {
		buginf("\n%s:out of sequence packet - got %d, range %d %d",
		       hwidb->hw_short_namestring, seq,
		       cgptr->rx_next_sequence, max_seq);
		dump_packet(pak);
	    }
	    datagram_done(pak);
	    return;
	}

	if (!(data_insertlist(&cgptr->rxQ, pak, cpp_pak_insert_before))) {
	    datagram_done(pak);
	    return;
	}

	/* start the timer, if it is not already running */
	if (!(mgd_timer_running(&cgptr->rx_sequence_timer)))
	    mgd_timer_start(&cgptr->rx_sequence_timer, (ONESEC/4));
    }
}

/*
 * cpp_rx_sequence_timer_expired
 * What happens if the sequence number timer expires ?
 */
static void cpp_rx_sequence_timer_expired (cpp_group_type *cgroup)
{
    paktype *pak;
    ushort old_seq;
    /*
     * Timer for this group expired - increment the sequence number and
     * process the first packet on the rxQ.
     */
    pak = pak_dequeue(&cgroup->rxQ);
    if (pak == NULL) {
	if (cpp_event_debug)
	    buginf("\nCpp timer expired - no pak on queue");
	mgd_timer_stop(&cgroup->rx_sequence_timer);
	return;
    }
    old_seq = cgroup->rx_next_sequence;
    cgroup->rx_next_sequence =
	INC_CPP_SEQ(cgroup->rx_next_sequence, 1);
    if (cpp_event_debug) {
	buginf("\n%s: Sequence timer expired - Lost %d Trying sequence %d",
	       pak->dialer_input->hw_short_namestring,
	       old_seq,
	       cgroup->rx_next_sequence);
    }
    /*
     * this will handle if we have 2 bad packets - it will put
     * it back on the queue.
     */
    process_cpp_ordered_pak(pak, pak->dialer_input);
}

/*
 * cpp_input
 * This is the basic input routine. It pulls packets off its
 * queue till it's all done...  It also handles any timers
 */

static void cpp_input (void)
{
    hwidbtype *hwidb;
    cpp_info_type *cinfo;
    paktype *pak;
    int pak_count;
    ulong major, minor;
    mgd_timer *expired;
    cpp_group_type *cgroup;

    process_watch_queue(CpppakQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&cpp_master_timer, ENABLE);
    
    while (TRUE) { 
	process_wait_for_event();
	pak_count = PAK_SWITCHCOUNT;
	while (process_get_wakeup(&major, &minor)) {
	    if (major == QUEUE_EVENT) {
		/*
		 * Have a packet on my queue
		 */
		if (--pak_count == 0)
		    break;

		pak = process_dequeue(CpppakQ);
		if (pak == NULL)
		    break;

		/*
		 * We will use the dialer_input, because that will be
		 * the b-channel.
		 */
		if (pak->dialer_input != NULL)
		    hwidb = pak->dialer_input;
		else
		    hwidb = pak->if_input->hwptr;
		
		if ((hwidb == NULL) ||
		    ((cinfo = hwidb->cpp_info) == NULL)) {
		    if (hwidb) {
			buginf("%s:no cinfo", hwidb->hw_short_namestring);
		    }
		    datagram_done(pak);
		    continue;
		}
		if (cinfo->negotiation_complete == FALSE) {
		    cpp_negotiate(hwidb, pak);
		    continue;
		}

		/* Real, live packet.  Do processing */
		switch (cinfo->protocol) {
		case CPP_PROTO_ORDERED:
		case CPP_PROTO_COMP_ORDERED:
		    process_cpp_ordered_pak(pak, hwidb);
		    break;
		case CPP_PROTO_HDLC:
		    process_cpp_pak(pak, hwidb);
		    break;
		case CPP_PROTO_FRAG:
		case CPP_PROTO_COMP_FRAG:
		    datagram_done(pak);
		    break;
		}
	    } else if (major == TIMER_EVENT) {
		/* process timers */
		while (mgd_timer_expired(&cpp_master_timer)) {
		    expired = mgd_timer_first_expired(&cpp_master_timer);
		    switch (mgd_timer_type(expired)) {
		    case CPP_GROUP_RX_SEQUENCE:
			cgroup = mgd_timer_context(expired);
			cpp_rx_sequence_timer_expired(cgroup);
			break;
		    case CPP_INTERFACE_NEG_TIMER:
			hwidb = mgd_timer_context(expired);
			cpp_send_negotiation(hwidb, hwidb->cpp_info);
			break;
		    default:
			mgd_timer_stop(expired);
			break;
		    }
		}
	    } else {
		errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
	    }
	}  /* end while process_get_wakeup */
    } /* end while TRUE */
}


/*
 * cpp_vencapsulate
 * Encapsulate a datagram.  
 */
static boolean cpp_vencapsulate (paktype *pak, long address)
{
    hwidbtype *hwidb;
    cpp_info_type *cinfo;
    dialergrouptype *dgptr;
    cpp_group_type *cgptr;

    hwidb = pak->if_output->hwptr;
    if ((hwidb == NULL) ||
	((cinfo = hwidb->cpp_info) == NULL))
	return(FALSE);

    /*
     * before getting CPP type. check if we have to do some ugly things
     * for bridging
     */
    pak->encsize = 0;

    if (pak->linktype != LINK_BRIDGE) {
	if (pak->linktype == LINK_NOVELL)
	    pak->enctype = ET_NOVELL_ETHER;
	else
	    pak->enctype = ET_ARPA;
	if (ether_vencap(pak, address)) {
#if 0
	    /*
	     * an ugly kludge to get NCP state for bridge
	     */
	    pak->linktype = LINK_BRIDGE;
	    pak->enctype = ET_BRIDGE_ENCAPS;
#endif
	} else {
	    if (cpp_packet_debug)
		buginf("\nether vencap failed - linktype %d, address %x",
		       pak->linktype, address);
	    return(FALSE);
	}
	if (pak->datagramsize < MINETHERSIZE)
	    pak->datagramsize = MINETHERSIZE;
    }

    /* The packet should now have the ether header on it */

    if (cinfo->protocol != CPP_PROTO_HDLC) {
	ushort seq_num;
	
	dgptr = get_dialergroup(hwidb);
	if (dgptr == NULL)
	    return(FALSE);
	cgptr = dgptr->cgroup;
	if (cgptr == NULL)
	    return(FALSE);
	
	seq_num = cgptr->tx_sequence_number;
	cgptr->tx_sequence_number =
	    INC_CPP_SEQ(cgptr->tx_sequence_number,1);
	/*
	 * We should compress at this point, before adding the sequence #
	 */
	pak->datagramstart -= CPP_ORDERED_ENCAPBYTES;
	pak->datagramsize += CPP_ORDERED_ENCAPBYTES;
	PUTCPPSHORT(pak->datagramstart, seq_num);
	pak->encsize += 2;
    }

    /*
     * Check if we're compressing protocol datagrams and if we are
     * allowed to compress THIS datagram.  Skip compression if any
     * other code (like poor TCP, for example) is holding onto this
     * packet.
    if (pak->refcount < 2) 
        ccp_compress_outgoing_pkt(idb, pak);
     */

    pak->mac_start  = pak->datagramstart;
    pak->addr_start = NULL;
    pak->rif_start  = NULL;
    pak->riflen     = 0;
    pak->info_start = NULL;
    pak->enctype    = ET_CPP;

    if (cpp_packet_debug) {
	buginf("\n%s output packet - len %d", hwidb->hw_short_namestring,
	       pak->datagramsize);
	dump_packet(pak);
    }
    if (hwidb->board_encap) {
	if (!(*hwidb->board_encap)(pak,hwidb)) {
	    return(FALSE);
	}
    }
    /*
     * Ah, we won't be doing this for a while...
    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
     */
    
    return(TRUE);
}

/*
 * cpp_deencap
 * Take down the protocol.
 * Called whenever a new encap type is selected.
 */
static void cpp_deencap (hwidbtype *idb)
{
    idbtype *swidb;
    idb->show_support = NULL;

    idb->enctype = ET_NULL;	/* No encapsulation set now */
    if (idb->comp_db) {
        free(idb->comp_db);
	idb->comp_db = NULL;
    }
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	swidb->arp_arpa = idb->cpp_info->save_arp;
	swidb->arp_arpa_def = idb->cpp_info->save_arp_def;
	swidb->arp_timeout  = idb->cpp_info->save_arp_timeout;
    }
    if (idb->cpp_info) {
	free(idb->cpp_info);
	idb->cpp_info = NULL;
    }
}


/*
 * cpp_periodic
 * Send keep alives and keep track of keepalives we have received
 * No keeps received for 3 periods, dump the call
 */
static void cpp_periodic (hwidbtype *hwidb)
{
    cpp_info_type *cinfo;

    cinfo = hwidb->cpp_info;
    if (cinfo == NULL)
	return;
    if ((cinfo->negotiation_complete) &&
	(hwidb->nokeepalive == FALSE)) {
	/*
	 * Check if keepalive was seen in this period
	 */
	if (cinfo->seen_keepalive) {
	    cinfo->keep_periods_missed = 0;
	    cinfo->seen_keepalive = FALSE;
	} else {
	    cinfo->keep_periods_missed++;
	    if (cinfo->keep_periods_missed > CPP_MAX_KEEP_PERIODS_MISSED) {
		if (cpp_event_debug)
		    buginf("\n%s:Line Integrity Violation",
			   hwidb->hw_short_namestring);
		dialer_disconnect(hwidb);
		return;
	    }
	}
	/*
	 * Send our own keepalive to the other side
	 */
	create_and_send_cpp_inband_pak(hwidb, CPP_SEND_LINE_INTEGRITY, 0);
    }
}

/*
 * cpp_setup
 * Set up a serial interface to use cpp
 */
static boolean cpp_setup (parseinfo *csb, hwidbtype *idb)
{
    idbtype *swidb;
    cpp_info_type *cinfo;

    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_UNKNOWN;
    cinfo = idb->cpp_info = malloc_named(sizeof(cpp_info_type), "CPP info");
    if (cinfo == NULL)
	return(FALSE);
    mgd_timer_init_leaf(&cinfo->neg_timer, &cpp_neg_timer,
			CPP_INTERFACE_NEG_TIMER, idb, FALSE);
    cpp_reset_info(cinfo);

    idb->vencap = cpp_vencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->deencap = cpp_deencap;
    idb->getlink = cpp_getlink;
    set_default_queueing(idb);
    idb->iqueue = raw_enqueue;
    idb->parse_packet = cpp_parse_packet;
    idb->periodic = cpp_periodic;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = ether_extract_addr;
    idb->broadcast = (broadcast_t) return_fuz_false;    /* just return FALSE */
    idb->show_support = NULL; /* cpp_interface; */
    idb->lineaction = cpp_cstate;
    idb->linestate = cpp_linestate;
    idb->enctype = ET_CPP;
    idb->encsize = 0; /* unordered transmission has 0 */

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	cinfo->save_arp = swidb->arp_arpa;
	cinfo->save_arp_def = swidb->arp_arpa_def;
	cinfo->save_arp_timeout = swidb->arp_timeout;
	swidb->ip_enctype = ET_ARPA;
	swidb->arp_arpa = swidb->arp_arpa_def = TRUE;
	swidb->arp_timeout = ARP_MAXAGE;
	reg_invoke_media_enctype_setup(swidb, ET_CPP);
	swidb->tbridge_media = TBR_CPP_TRANSIT;
	swidb->ip_routecache_same = swidb->ip_routecache_same_def = TRUE;
    }
    idb->inloopback = FALSE;

    /*
     * If these are ZERO then it's during system init. and we con't call
     * set_maxdgram()!
     */
    if (supports_maxdgram(idb)) {
	(*idb->set_maxdgram)(idb, idb->maxmtu + ETHER_BAGGAGE, 
			     idb->maxmtu+ETHER_BAGGAGE+CPP_ENCAPBYTES);
	(*(idb->reset))(idb);
    }
    idb->span_encapsize = idb->encsize;


    if (!cpp_fork) {
	cpp_fork = cfork(cpp_input, 0L, 1000,
			 "CPP manager", CONSOLE_LINENUM);
    }

    idb->status &= ~IDB_BCAST;
    return(TRUE);
}

/**************************************************************************
 *   CPP command processing
 **************************************************************************/
void cpp_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    cpp_info_type *cinfo;
    hwidb = csb->interface->hwptr;
    if (HUNTGROUP_MEMBER(get_dialerdb(hwidb))) {
	if (!csb->nvgen) {
	    printf("\nCannot change cpp values of hunt group member");
	}
	return;
    }
    if (hwidb->enctype != ET_CPP) {
	if (!csb->nvgen) {
	    printf("\nMust set encapsulation to CPP before"
		   " using CPP subcommand");
	}
	return;
    }

    cinfo = hwidb->cpp_info;
    if (csb->nvgen) {
	switch (csb->which) {
	case CPP_AUTH:
	    nv_write(cinfo->auth_required, "%s", csb->nv_command);
	    break;
	case CPP_CALLBACK:
	    nv_write(cinfo->accept_callback, "%s", csb->nv_command);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case CPP_AUTH:
	if (csb->sense)
	    cinfo->auth_required = TRUE;
	else
	    cinfo->auth_required = FALSE;
	break;
    case CPP_CALLBACK:
	cinfo->accept_callback = csb->sense;
	break;
	
    }
    reg_invoke_interface_cfg_modified(hwidb);
}

/*
 * cpp_copy_fields
 * copy cpp fields from one idb to another.  Used to propagate info from
 * dialer hunt groups to interfaces.
 */
static void copy_cpp_fields (hwidbtype *idb1, hwidbtype *idb2)
{
    cpp_info_type *cinfo1, *cinfo2;

    cinfo1 = idb1->cpp_info;
    if (cinfo1 == NULL)
	return;

    if (idb2->cpp_info == NULL) {
	idb2->cpp_info = malloc_named(sizeof(cpp_info_type), "CPP info");
	cpp_reset_info(idb2->cpp_info);
    }
    cinfo2 = idb2->cpp_info;

    cinfo2->accept_callback = cinfo1->accept_callback;
    cinfo2->auth_required = cinfo1->auth_required;
    /*
     * deal with compression
     * If the primary has compression enabled, allocate the space on
     * on the secondary if it doesn't exist.  If the primary doesn't
     * have it, then if the secondary does, free it.
    if (idb1->comp_db) {
        if (!idb2->comp_db)
	    idb2->comp_db = malloc_named(sizeof(struct compress_db_),
					 "CPPINE Comp DB");
	if (idb2->comp_db)
	    idb2->comp_db->comp_configed = idb1->comp_db->comp_configed;
    } else {
        if (idb2->comp_db) {
	    free(idb2->comp_db);
	    idb2->comp_db = NULL;
	}
    }
     */
}

/*************************************************************************
 *  CPP GROUP PROCESSING
 *
 * The below routines are processing for the cpp group extensions
 * off of the dialergrouptype structures.
 *************************************************************************/
/*
 * cpp_group_init
 * Initialize the cpp group structure for a dialer group
 * This will keep track of all the fields and timers that we
 * need to handle for a cpp extension
 */
static void cpp_group_init (hwidbtype *hwidb, dialergrouptype *gptr)
{
    cpp_group_type *cgroup;
    
    if (hwidb->enctype != ET_CPP)
	return;

    gptr->cgroup = malloc_named(sizeof(cpp_group_type), "CPP group");
    cgroup = gptr->cgroup;
    if (cgroup) {
	queue_init(&cgroup->rxQ, 0);
	cgroup->rx_sequence_valid = FALSE;
	mgd_timer_init_leaf(&cgroup->rx_sequence_timer, &rx_sequence_timer,
			    CPP_GROUP_RX_SEQUENCE, cgroup, FALSE);
	if (cpp_event_debug)
	    buginf("\n%s: created cpp group %s",
		   hwidb->hw_short_namestring, gptr->remote_name);
    }
}

/*
 * cpp_group_destroy
 * Stop the group processing, and free the memory associated with it
 */
static void cpp_group_destroy (dialergrouptype *gptr)
{
    paktype *pak;
    cpp_group_type *cgroup;

    cgroup = gptr->cgroup;
    if (cgroup == NULL)
	return;

    while ((pak = p_dequeue(&cgroup->rxQ)) != NULL) {
	datagram_done(pak);
    }
    mgd_timer_stop(&cgroup->rx_sequence_timer);
    free(cgroup);
    gptr->cgroup = NULL;
    if (cpp_event_debug)
	buginf("\ndestroyed cpp group %s", gptr->remote_name);
}


/*
 * cpp_init
 * Initialization of CPP subsystem
 */
static void cpp_init (subsystype *subsys)
{
    /*
     * Do CPP proper
     */
    if (CpppakQ == NULL)
	CpppakQ = create_watched_queue("cpp input", 0, 0);

    mgd_timer_init_parent(&cpp_master_timer, NULL);
    mgd_timer_init_parent(&rx_sequence_timer, &cpp_master_timer);
    mgd_timer_init_parent(&cpp_neg_timer, &cpp_master_timer);
    
    cpp_fork = 0;

    cpp_parser_init();
    cpp_debug_init();
    reg_add_media_serial_setup(ET_CPP, cpp_setup, "cpp_setup");
    reg_add_media_copy_encap_fields(ET_CPP, copy_cpp_fields,
				    "copy_cpp_fields");
    reg_add_dialer_group_init(cpp_group_init, "cpp group init");
    reg_add_dialer_group_destroy(cpp_group_destroy,
				 "cpp group destroy");
    reg_add_raw_enqueue(LINK_CPP, cpp_enqueue, "cpp_enqueue");
}

#define CPP_MAJVERSION 1
#define CPP_MINVERSION 0
#define CPP_EDITVERSION  1

SUBSYS_HEADER(cpp, CPP_MAJVERSION, CPP_MINVERSION, CPP_EDITVERSION,
	      cpp_init, SUBSYS_CLASS_PROTOCOL, NULL,
	      "req: dialer compress");
