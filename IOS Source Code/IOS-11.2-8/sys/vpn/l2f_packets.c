/* $Id: l2f_packets.c,v 1.2.40.14 1996/09/12 04:02:12 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/l2f_packets.c,v $
 *------------------------------------------------------------------
 * l2f_packets.c   --  routines for creating and receiving l2f packets
 *
 * January 1996, Tim Kolar
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: l2f_packets.c,v $
 * Revision 1.2.40.14  1996/09/12  04:02:12  tkolar
 * CSCdi62277:  L2F tunnel authentication over Radius fails
 * Branch: California_branch
 * Allow cleartext passwords to be passed from Radius
 *
 * Revision 1.2.40.13  1996/09/04  16:14:40  tkolar
 * CSCdi67999:  L2F_CLOSE_STR is processed incorrectly
 * Branch: California_branch
 * Fix processing of L2F_CLOSE_STR, and add our own temporary
 * outgoing L2F_CLOSE_STR so people can test against us.
 *
 * Revision 1.2.40.12  1996/08/16  22:25:55  tkolar
 * CSCdi66320:  L2F LCP0 message not handled
 * Branch: California_branch
 * Interpret L2F_REQ_LCP0 correctly, and change two
 * other defines to match the L2F draft.
 *
 * Revision 1.2.40.11  1996/08/16  22:05:16  tkolar
 * CSCdi66351:  L2F tunnel authentication should allow arbitrary length
 * challenges
 * Branch: California_branch
 * Allow them, and change the default challenge to 16 bytes.
 *
 * Revision 1.2.40.10  1996/07/26  00:01:56  syiu
 * CSCdi64115:  Stackability, L2F projections fails on second user when
 * tunnel exist
 * Branch: California_branch
 *
 * Revision 1.2.40.9  1996/06/21  16:37:23  tkolar
 * CSCdi54166:  %ALIGN-3-CORRECT: Alignment correction while using VPN
 * Branch: California_branch
 * Use the offset bit of L2F to provide correct alignment, and while we're
 * here hide a buginf behind a debug flag where it belongs.
 *
 * Revision 1.2.40.8  1996/06/18  21:03:21  tkolar
 * CSCdi60718:  L2F_KEY is ignored
 * Branch: California_branch
 * Treat keys as per the RFC.
 *
 * Revision 1.2.40.7  1996/05/22  20:58:04  syiu
 * CSCdi58451:  prefix more L2F debug messages
 * Branch: California_branch
 *
 * Revision 1.2.40.6  1996/05/22  18:11:43  snyder
 * CSCdi58423:  spellink errors
 * Branch: California_branch
 *
 * suceeded -> succeeded
 * non-existant -> non-existent
 *
 * Revision 1.2.40.5  1996/05/12  21:24:20  tkolar
 * CSCdi57473:  L2F implementation does not match RFC
 * Branch: California_branch
 * Assuage those poor fools who believe that just because we
 * wrote the RFC, our implementation should somehow interoperate
 * with it.
 *
 * Revision 1.2.40.4  1996/05/12  18:08:34  tkolar
 * CSCdi57470:  ipwrite should still not be called at interrupt
 * Branch: California_branch
 * Missed a file...
 *
 * Revision 1.2.40.3  1996/04/29  09:44:01  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 1.2.40.2  1996/04/27  06:34:19  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.18  1996/04/25  23:56:43  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 1.2.2.17  1996/04/20  20:59:40  tkolar
 * Branch: Dial1_branch
 * Bring the L2F flags into agreement with the RFC.
 *
 * Revision 1.2.2.16  1996/04/16  23:58:21  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 1.2.2.15  1996/03/25  05:32:13  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.2.2.14  1996/03/24  01:44:25  tkolar
 * Branch: Dial1_branch
 * Enable AAA forwarding of VPN sessions, and complete
 * authentication of MIDs.  Also update a few parts of the
 * protocol to match changes in the RFC.
 *
 * Revision 1.2.2.13  1996/03/22  23:12:04  tkolar
 * Branch: Dial1_branch
 * Use malloc'd memory for stack name and tunnel info.
 *
 * Revision 1.2.2.12  1996/03/22  05:34:19  tkolar
 * Branch: Dial1_branch
 * Reshape the way MLPVT interacts with VPN.
 *
 * Revision 1.2.2.11  1996/03/21  23:51:44  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 1.2.2.10  1996/03/19  03:36:54  tkolar
 * Branch: Dial1_branch
 * Clean up after tunnels when they close connections, and after
 * developers when they fat-finger commits.
 *
 * Revision 1.2.2.9  1996/03/12  04:55:32  tkolar
 * Branch: Dial1_branch
 * Vastly expand L2F comments, add two new debugging variables, and make
 * wraparound sequence numbers fully understood.
 *
 * Revision 1.2.2.8  1996/03/11  22:58:35  tkolar
 * Branch: Dial1_branch
 * Clean up a small collection of nits.
 *
 * Revision 1.2.2.7  1996/03/11  05:57:06  tkolar
 * Branch: Dial1_branch
 * Provide L2F timeouts.
 *
 * Revision 1.2.2.6  1996/03/11  02:07:37  tkolar
 * Branch: Dial1_branch
 * Enable L2F management packet reliable delivery.
 *
 * Revision 1.2.2.5  1996/02/25  05:18:56  tkolar
 * Branch: Dial1_branch
 *
 * Revision 1.2.2.4  1996/02/17  22:59:02  tkolar
 * Branch: Dial1_branch
 * Checkpoint a pingable VPN.
 *
 * Revision 1.2.2.3  1996/02/13  23:07:48  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN.
 *
 * Revision 1.2.2.2  1996/02/09  19:11:23  tkolar
 * Branch: Dial1_branch
 * Fix compile error.  Whups...
 *
 * Revision 1.2.2.1  1996/02/08  23:20:03  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN integration
 *
 * Revision 1.2.40.1  1996/03/18  22:46:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 1.2.14.1  1996/03/07  11:06:13  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 1.2  1996/01/31  18:38:37  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:10  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "types.h"
#include <ctype.h>
#include "ttysrv.h"
#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../ip/ip.h"
#include "../wan/dialer.h"
#include "../wan/ppp.h"
#include "../util/md5.h"
#include "../wan/ppp_auth.h"
#include "interface_private.h"
#include "../mlpvt/mlpvt_registry.regh"
#include "../wan/ppp_registry.h"
#include "../util/md5.h"
#include "l2f.h"
#include "vpn.h"
#include <ciscolib.h>


/*
 * l2f_get_management_header
 *
 * Build most of the header for an L2F management packet
 */
int l2f_get_management_header(l2f_tunneltype *tunnel, uchar *data, 
					boolean use_key)
{
    l2f_headertype *header;
    int size;

    size = sizeof(l2f_headertype) - sizeof(long);  /* there may not be a key */

    header = (l2f_headertype *) data;
    header->version_and_flags = tunnel->version_and_flags;
    header->protocol = L2F_PROTO;
    header->mid = 0;

    if (tunnel->instigator) {
	PUTSHORT(&header->clid, tunnel->nas_clid);
    } else {
	PUTSHORT(&header->clid, tunnel->gw_clid);
    }

    if (tunnel->offset_exists) {
	header->version_and_flags |= L2F_OFFSET;
	PUTSHORT(&header->offset, 0);
    }

    if (use_key) {
	size += sizeof(long);
	header->version_and_flags |= L2F_KEY;
	if (tunnel->instigator) {
	    PUTLONG(&header->key, tunnel->nas_key);
	} else {
	    PUTLONG(&header->key, tunnel->gw_key);
	}
    }

    return(size);
}

/*
 * l2f_set_header_extras
 *
 *  fill in the rest of a header for a L2F management packet
 */
static void l2f_set_header_extras(uchar *data, int length, int mid)
{
    l2f_headertype *header = (l2f_headertype *) data;

    header->length = length;
    header->mid = mid;
}


/*
 * l2f_nas2gw_tunnel0
 *
 *  Create an initial L2F_CONF packet to be sent from the NAS to the GW,
 *   to start the tunnel negotiation.
 */
int l2f_nas2gw_tunnel0(l2f_tunneltype *tunnel, uchar *data, int maxdatalen)
{
    int len;
    uchar *dataptr;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, FALSE);
    dataptr += len;

    /*
     *  Message type
     */

    *dataptr++ = L2F_CONF;

    /*
     *  Tunnel name
     */
    *dataptr++ = L2F_CONF_NAME;
    *dataptr++ = (uchar) strlen(tunnel->nas_name);
    bcopy(tunnel->nas_name, dataptr, strlen(tunnel->nas_name));
    dataptr += strlen(tunnel->nas_name);

    /* 
     *  Tunnel challenge;
     */
    *dataptr++ = L2F_CONF_CHAL;
    *dataptr++ = tunnel->nas_challenge_length;
    bcopy(tunnel->nas_challenge, dataptr, tunnel->nas_challenge_length);
    dataptr += tunnel->nas_challenge_length;

    /* 
     *  CLID for other side;
     */
    *dataptr++ = L2F_CONF_CLID;
    PUTLONG(dataptr, tunnel->gw_clid);
    dataptr += sizeof(long);

    l2f_set_header_extras(data, dataptr-data, L2F_MANAGEMENT_MID);

    return(dataptr-data);

}

/*
 * l2f_gw2nas_tunnel0
 *
 *  Create a response L2F_CONF packet, to be sent from the GW to the NAS,
 *   as the first response in a tunnel negotiation.
 */
int l2f_gw2nas_tunnel0(l2f_tunneltype *tunnel, uchar *data, int maxdatalen)
{
    int len;
    uchar *dataptr;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, FALSE);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_CONF;

    /*
     *  Tunnel name
     */
    *dataptr++ = L2F_CONF_NAME;
    *dataptr++ = (uchar) strlen(tunnel->gw_name);
    bcopy(tunnel->gw_name, dataptr, strlen(tunnel->gw_name));
    dataptr += strlen(tunnel->gw_name);

    /* 
     *  Tunnel challenge;
     */
    *dataptr++ = L2F_CONF_CHAL;
    *dataptr++ = tunnel->gw_challenge_length;
    bcopy(tunnel->gw_challenge, dataptr, tunnel->gw_challenge_length);
    dataptr += tunnel->gw_challenge_length;

    /* 
     *  CLID for other side;
     */
    *dataptr++ = L2F_CONF_CLID;
    PUTLONG(dataptr, tunnel->nas_clid);
    dataptr += sizeof(long);

    l2f_set_header_extras(data, dataptr-data, L2F_MANAGEMENT_MID);

    return(dataptr-data);
}

/*
 * l2f_nas2gw_tunnel1
 *
 * Create an L2F_OPEN packet, to be sent from the NAS to the GW, to 
 *  agree to open a tunnel.
 */
int l2f_nas2gw_tunnel1(l2f_tunneltype *tunnel, uchar *data, int maxdatalen)
{
    int len;
    uchar *dataptr;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_OPEN;

    /*
     *  Open CHAP response
     */
    *dataptr++ = L2F_OPEN_RESP;
    *dataptr++ = MD5_LEN;
    bcopy(tunnel->nas_response, dataptr, MD5_LEN);
    dataptr += MD5_LEN;

    l2f_set_header_extras(data, dataptr-data, L2F_MANAGEMENT_MID);

    return(dataptr-data);
}

/*
 * l2f_gw2nas_tunnel1
 *
 * Create an L2F_OPEN packet, to be sent from the GW to the NAS, to 
 *  agree to open a tunnel.
 */
int l2f_gw2nas_tunnel1(l2f_tunneltype *tunnel, uchar *data, int maxdatalen)
{
    int len;
    uchar *dataptr;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_OPEN;

    /*
     *  Open CHAP response
     */
    *dataptr++ = L2F_OPEN_RESP;
    *dataptr++ = MD5_LEN;
    bcopy(tunnel->gw_response, dataptr, MD5_LEN);
    dataptr += MD5_LEN;

    l2f_set_header_extras(data, dataptr-data, L2F_MANAGEMENT_MID);

    return(dataptr-data);
}

/*
 * l2f_nas2gw_mid0
 *
 *  Create an L2F_OPEN packet to request the opening of a MID.  Sent from
 *   the NAS to GW.
 */
int l2f_nas2gw_mid0(l2f_midtype *mid, uchar *data, int maxdatalen)
{
    int len;
    uchar *dataptr;
    l2f_tunneltype *tunnel;

    if (l2f_event_debug)
	buginf("\nL2F building nas2gw_mid0");

    tunnel = mid->owner;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_OPEN;

    /* 
     *  Authen type 
     */
    *dataptr++ = L2F_OPEN_TYPE;
    *dataptr++ = mid->authtype;

    /*
     *  Client name
     */
    *dataptr++ = L2F_OPEN_NAME;
    *dataptr++ = (uchar) strlen(mid->name);
    bcopy(mid->name, dataptr, strlen(mid->name));
    dataptr += strlen(mid->name);

    /*
     *  Open challenge
     */

    *dataptr++ = L2F_OPEN_CHAL;
    *dataptr++ = mid->challenge_len;
    bcopy(mid->challenge, dataptr, mid->challenge_len);
    dataptr += mid->challenge_len;

    /* 
     *  CHAP identification 
     */
    *dataptr++ = L2F_OPEN_ID;
    *dataptr++ = mid->chap_id;

    /*
     *  Open response
     */
    *dataptr++ = L2F_OPEN_RESP;
    *dataptr++ = mid->response_len;
    bcopy(mid->response, dataptr, mid->response_len);
    dataptr += mid->response_len;

    /*
     *  Open LCP accepted from client
     */
    *dataptr++ = L2F_ACK_LCP1;
    PUTSHORT(dataptr, mid->rcv_ack_len);
    dataptr += sizeof(short);
    bcopy(mid->rcv_ack, dataptr, mid->rcv_ack_len);
    dataptr += mid->rcv_ack_len;

    /*
     *  Open LCP sent to client
     */
    *dataptr++ = L2F_ACK_LCP2;
    PUTSHORT(dataptr, mid->snt_ack_len);
    dataptr += sizeof(short);
    bcopy(mid->snt_ack, dataptr, mid->snt_ack_len);
    dataptr += mid->snt_ack_len;

    l2f_set_header_extras(data, dataptr-data, mid->mid);

    return(dataptr-data);
}

/*
 * l2f_gw2nas_mid0
 *
 *  Create an L2F_OPEN packet, sent from the GW to the NAS to confirm 
 *  that a MID should be opened.
 */
int l2f_gw2nas_mid0(l2f_midtype *mid, uchar *data, int maxdatalen)
{
    int len;
    uchar *dataptr;
    l2f_tunneltype *tunnel;

    tunnel = mid->owner;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_OPEN;

    l2f_set_header_extras(data, dataptr-data, mid->mid);

    return(dataptr-data);
}

/*
 * l2f_echo
 *  
 *  Create an L2F_ECHO packet.
 */
int l2f_echo(l2f_tunneltype *tunnel, uchar *data, int maxdatalen) 
{
    int len;
    uchar *dataptr;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_ECHO;
    l2f_set_header_extras(data, dataptr-data, L2F_MANAGEMENT_MID);

    return(dataptr-data);
}

/*
 * l2f_echo_resp
 *  
 *  Create an L2F_ECHO_RESP packet.
 */
int l2f_echo_resp(l2f_tunneltype *tunnel, uchar *data, int maxdatalen) 
{
    int len;
    uchar *dataptr;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_ECHO_RESP;

    l2f_set_header_extras(data, dataptr-data, L2F_MANAGEMENT_MID);

    return(dataptr-data);
}

/*
 * l2f_pkt_close_mid
 *
 *  Create an L2F_CLOSE packet for a particular MID.
 */
int l2f_pkt_close_mid(l2f_midtype *mid, uchar *data, int maxdatalen, int why)
{
    int len;
    uchar *dataptr;
    l2f_tunneltype *tunnel;

    tunnel = mid->owner;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_CLOSE;

    /* 
     *  Why we're closing.
     */
    *dataptr++ = L2F_CLOSE_WHY;
    PUTLONG(dataptr, why);
    dataptr += sizeof(long);

    *dataptr++ = L2F_CLOSE_STR;
    PUTSHORT(dataptr, strlen(L2F_FAREWELL));
    dataptr += sizeof(short);
    bcopy(L2F_FAREWELL, dataptr, strlen(L2F_FAREWELL));
    dataptr += strlen(L2F_FAREWELL);

    l2f_set_header_extras(data, dataptr-data, mid->mid);

    return(dataptr-data);

}

/*
 * l2f_pkt_close_tunnel
 *
 *  Create an L2F_CLOSE packet for a tunnel.
 */
int l2f_pkt_close_tunnel(l2f_tunneltype *tunnel, uchar *data, int maxdatalen)
{
    int len;
    uchar *dataptr;

    dataptr = data;

    len = l2f_get_management_header(tunnel, dataptr, tunnel->key_exists);
    dataptr += len;

    /*
     *  Message type
     */
    *dataptr++ = L2F_CLOSE;

    l2f_set_header_extras(data, dataptr-data, L2F_MANAGEMENT_MID);

    return(dataptr-data);

}


/*
 * l2f_receive_l2f_conf
 *
 *  Handle an L2F_CONF packet received from the other side.
 */
void l2f_receive_l2f_conf(l2f_headertype *header, uchar *payload, int payload_size, ipaddrtype srcaddr)
{
    int count, len;
    uchar challenge[MAX_CHALLENGE_LEN];
    uchar *buf, *name;
    l2f_tunneltype *tunnel;
    l2f_tunnel_infotype *info;
    int clid = -1, challenge_len = 0;
    char *stackname;
    addrtype daddr, saddr;
    ipaddrtype source_ip;

    buf = malloc(MAX_L2FMANAGE_SIZE);
    if (!buf) {
	return;
    }

    name = malloc(MAX_VPN_SMALL_PARM);
    if (!name) {
	free(buf);
	return;
    }

    for (count = 1; count < payload_size; count++) {
	switch(payload[count]) {
	    case L2F_CONF_NAME:
		if (l2f_packet_debug)
		    buginf("\nL2F_CONF_NAME");
		count++;
		len = payload[count];
		bcopy(&payload[count+1], name, len);
		name[len] = 0;
		count += len;
		break;
	    case L2F_CONF_CHAL:
		count++;
		len = payload[count];
		challenge_len = len;
		if (len > MAX_CHALLENGE_LEN) {
		    challenge_len = MAX_CHALLENGE_LEN;
		    if (l2f_error_debug) {
		       buginf("\nL2F_CONF_CHAL with length > %d received, "
			"authentication will probably fail.", 
			MAX_CHALLENGE_LEN);
		    }
		}
		bcopy(&payload[count+1], challenge, challenge_len);
		if (l2f_packet_debug) {
		    buginf("\nL2F_CONF_CHAL (%d bytes)", len);
		}
		count += len;
		break;
	    case L2F_CONF_CLID:
		count++;
		clid = GETLONG(&payload[count]);
		if (l2f_packet_debug) {
		    buginf("\nL2F_CONF_CLID");
		    buginf("\nclid: %d", clid);
		}
		count += sizeof(long) -1;
		break;
	    default:
		if (l2f_error_debug)
		    buginf("\nL2F Bogus L2F_CONF suboption");
	}
    }

    if (GETSHORT(&header->clid)) {
	tunnel = l2f_find_tunnel_by_clid(GETSHORT(&header->clid));
	if (!tunnel || !tunnel->instigator 
		    || (tunnel->state != L2FTUNNEL_OPENING)) {
	    if (l2f_error_debug) {
		buginf("\nL2F Received L2F_CONF with clid set, but we didn't"
			" instigate a connection.");
	    }
	    free(name);
	    free(buf);
	    return;
	}
    } else {
	stackname = malloc(MAXSTR+1);
	if (!stackname) {
	    free(name);
	    free(buf);
	    return;
	}
	bzero(stackname, MAXSTR+1);
	reg_invoke_mlpvt_find_stackname(stackname, MAXSTR, &source_ip);
	info = l2f_find_tunnel_by_name(name);
	if (info != NULL) {
	    if (l2f_event_debug)
		buginf("\nL2F Creating new tunnel for %s", name);
	    tunnel = l2f_make_tunnel_structure(info);
	} else if (stackname[0] && (!strcmp(name, stackname))) {
	    /*
	     * this is a stackability connection.  If there is already
	     *  one with this IP address, then things messed up...
	     */
             tunnel = l2f_find_active_tunnel_by_address(srcaddr);
             if (tunnel && !tunnel->instigator) {
		if (l2f_error_debug) {
		   buginf("\nL2F Received a stack OPEN request from %i, but "
			  "we already have a tunnel there", srcaddr);
		}
		free(stackname);
		free(name);
		free(buf);
		return;
	     } else {

		info = malloc(sizeof(l2f_tunnel_infotype));
                if (!info) {
		   free(stackname);
		   free(name);
		   free(buf);
		   return;
                }

		info->virt_temp = reg_invoke_mlp_get_vtemplate_num();
		strcpy(info->nas_name, stackname);
		strcpy(info->gw_name, stackname);
		info->direction = L2F_INCOMING;
		info->type = VPDN_MULTICHASSIS;
		if (!source_ip)
		    info->source_ip = -1;
		else
		    info->source_ip = source_ip;
		tunnel = l2f_make_tunnel_structure(info);
		free(info);
	     }
	} else {
	    if (l2f_error_debug) {
		buginf("\nL2F Couldn't find tunnel named %s", name);
	    }
	    free(stackname);
	    free(name);
	    free(buf);
	    return;
	}
	free(stackname);
    }

    if (tunnel == NULL) {
	free(name);
	free(buf);
	return;
    }

    if (tunnel->instigator) {
	/*
	 * First, clean out our CONF from the resend queue
	 */
	l2f_clean_resendQ(tunnel, L2F_CONF, L2F_MANAGEMENT_MID);

	strcpy(tunnel->gw_name, name);
	bcopy(challenge, tunnel->gw_challenge, challenge_len);
	tunnel->gw_challenge_length = challenge_len;

	if (tunnel->nas_password[0]) {
	    chap_hash(tunnel->nas_password, clid %256, 
		    tunnel->gw_challenge, 
		    tunnel->gw_challenge_length, tunnel->nas_response);
	} else {
	    vpn_get_md5_digest(tunnel->nas_name, 
		    clid %256, (char *) tunnel->gw_challenge, 
		    tunnel->gw_challenge_length, tunnel->nas_response);
	}
	tunnel->nas_key = vpn_make_tunnel_key(tunnel->nas_response);

	if (tunnel->gw_password[0]) {
	    chap_hash(tunnel->gw_password, tunnel->gw_clid %256, 
		    tunnel->nas_challenge, 
		    tunnel->nas_challenge_length, tunnel->gw_response);
	} else {
	    vpn_get_md5_digest(tunnel->gw_name, tunnel->gw_clid %256, 
			(char *) tunnel->nas_challenge,
			tunnel->nas_challenge_length, tunnel->gw_response);
	}
	tunnel->gw_key = vpn_make_tunnel_key(tunnel->gw_response);

	tunnel->key_exists = TRUE;

	tunnel->nas_clid = clid;
	len = l2f_nas2gw_tunnel1(tunnel, buf, MAX_L2FMANAGE_SIZE);
	if (len > 0)  {
	    l2f_send(tunnel, buf, len, INITIAL_SEND); 
	    l2f_tunnel_move_to_state(tunnel, L2FTUNNEL_OPEN);
	}
	free(name);
	free(buf);
	return;
    } else {
	if (l2f_event_debug)
	    buginf("\nL2F Got a tunnel named %s, responding", name);
	tunnel->packets_in++;
	tunnel->bytes_in += header->length;
	strcpy(tunnel->nas_name, name);
	bcopy(challenge, tunnel->nas_challenge, challenge_len);
	tunnel->nas_challenge_length = challenge_len;
	if (tunnel->gw_password[0]) {
	    chap_hash(tunnel->gw_password, tunnel->gw_clid %256, 
		    tunnel->nas_challenge, 
		    tunnel->nas_challenge_length, tunnel->gw_response);
	} else {
	    vpn_get_md5_digest(tunnel->gw_name, clid %256, (char *) 
			tunnel->nas_challenge, tunnel->nas_challenge_length,
			tunnel->gw_response);
	}
	tunnel->gw_key = vpn_make_tunnel_key(tunnel->gw_response);

	if (tunnel->nas_password[0]) {
	    chap_hash(tunnel->nas_password, clid %256, 
		    tunnel->gw_challenge, 
		    tunnel->gw_challenge_length, tunnel->nas_response);
	} else {
	    vpn_get_md5_digest(tunnel->nas_name, tunnel->nas_clid, 
			tunnel->gw_challenge, tunnel->gw_challenge_length, 
			tunnel->nas_response);
	}
	tunnel->nas_key = vpn_make_tunnel_key(tunnel->nas_response);

	tunnel->key_exists = TRUE;

	tunnel->gw_clid = clid;
	tunnel->ip_address = srcaddr;
        daddr.ip_addr = srcaddr;
	saddr.ip_addr = 0;
	if (tunnel->source_ip) { 
	    if (tunnel->source_ip != -1)
		saddr.ip_addr = tunnel->source_ip;
	}
	else
	    saddr.ip_addr = vpn_source_ip_address;
	
	if (saddr.ip_addr) {
	    tunnel->sock = (ipsocktype *) open_ipsocket(UDP_PROT, &saddr,
		&daddr, L2F_PORT, L2F_PORT, SOCK_ANYSRC | SOCK_SRC_SPECIFIED);
	} else {
	    tunnel->sock = (ipsocktype *) open_ipsocket(UDP_PROT, 0,
		    &daddr, L2F_PORT, L2F_PORT, SOCK_ANYSRC | SOCK_SRC_DYN);
	}

	if (tunnel->sock == NULL) {
	     if (l2f_error_debug)
		 buginf("\nL2F Failed to open socket to %i", tunnel->ip_address);
	     free(name);
	     free(buf);
	     return;
	}

	if (l2f_event_debug) {
	    if (saddr.ip_addr)
		buginf("\nL2F Open UDP socket from %i to %i",
		       tunnel->sock->laddress.ip_addr, 
		       tunnel->sock->faddress.ip_addr);
	    else
		buginf("\nL2F Open UDP socket to %i",
		       tunnel->sock->faddress.ip_addr);
	}

	len = l2f_gw2nas_tunnel0(tunnel, buf, MAX_L2FMANAGE_SIZE);
	if (len > 0) {
	    l2f_tunnel_move_to_state(tunnel, L2FTUNNEL_OPENING);
	    l2f_send(tunnel, buf, len, INITIAL_SEND); 
	}
    }
    free(name);
    free(buf);
}

/*
 * l2f_receive_l2f_open
 *
 *  Handle an L2F_OPEN packet received from the other side.
 */
void l2f_receive_l2f_open(l2f_headertype *header, uchar *payload, int payload_size) 
{
    int count;
    uchar *buf;
    int len = -1;
    uchar chapnamelen = 0, chapchallen = 0, chapresplen = 0;
    ushort rcv_ack_len = 0;
    ushort snt_ack_len = 0;
    ushort lcp0_len = 0;
    uchar auth_type = 0;
    uchar chap_id = 0;
    l2f_tunneltype *tunnel;
    l2f_midtype *mid;
    int status;
    struct open_info *info;
    boolean passed_auth = FALSE;
    uchar outhash[MD5_LEN];

    info = malloc(sizeof(struct open_info));

    if (!info) {
	return;
    }

    buf = malloc(MAX_L2FMANAGE_SIZE);
    if (!buf) {
	free(info);
	return;
    }


    tunnel = l2f_find_tunnel_by_clid(GETSHORT(&header->clid));
    if (tunnel == NULL) {
	free(info);
	free(buf);
        return;
    }

    for (count = 1; count < payload_size; count++) {
	switch(payload[count]) {
	    case L2F_OPEN_TYPE:
		if (l2f_packet_debug)
		    buginf("\nL2F_OPEN_TYPE");
		count++;
		auth_type = payload[count]; 
		break;
	    case L2F_OPEN_NAME:
		if (l2f_packet_debug)
		    buginf("\nL2F_OPEN_CHAP");
		count++;
		chapnamelen = payload[count];
		bcopy(&payload[count+1], info->chapname, chapnamelen);
		info->chapname[chapnamelen] = 0;
		count += chapnamelen;
		break;
	    case L2F_OPEN_ID:
		if (l2f_packet_debug)
		    buginf("\nL2F_OPEN_ID");
		count++;
		chap_id = payload[count]; 
		break;
	    case L2F_OPEN_CHAL:
		if (l2f_packet_debug)
		    buginf("\nL2F_OPEN_CHAL");
		count++;
		chapchallen = payload[count];
		bcopy(&payload[count+1], info->chapchal, chapchallen);
		count += chapchallen;
		break;
	    case L2F_OPEN_RESP:
		if (l2f_packet_debug)
		    buginf("\nL2F_OPEN_RESP");
		count++;
		chapresplen = payload[count];
		bcopy(&payload[count+1], info->chapresp, chapresplen);
		count += chapresplen;
		break;
	    case L2F_ACK_LCP1:
		if (l2f_packet_debug)
		    buginf("\nL2F_ACK_LCP1");
		count++;
		rcv_ack_len = GETSHORT(&payload[count]);
		bcopy(&payload[count+2], info->rcv_ack, rcv_ack_len);
		count += rcv_ack_len+1;  /* +1 to account for the */
					    /*	 second length byte  */
		break;
	    case L2F_ACK_LCP2:
		if (l2f_packet_debug)
		    buginf("\nL2F_ACK_LCP2");
		count++;
		snt_ack_len = GETSHORT(&payload[count]);
		bcopy(&payload[count+2], info->snt_ack, snt_ack_len);
		count += snt_ack_len+1;  /* +1 to account for the */
					    /*	 second length byte  */
		break;
	    case L2F_REQ_LCP0:
		if (l2f_packet_debug)
		    buginf("\nL2F_REQ_LCP0");
		count++;
		lcp0_len = GETSHORT(&payload[count]);
		if (l2f_error_debug) {
		    buginf("\nL2F_REQ_LCP0 received and ignored.");
		}
		count += lcp0_len+1;  /* +1 to account for the */
					    /*	 second length byte  */
		break;
	    default:
		if (l2f_error_debug)
		    buginf("\nL2F Bogus L2F_OPEN suboption");
	}
    }

    if (header->mid == L2F_MANAGEMENT_MID) {
	if (tunnel->instigator) {
	    if (tunnel->state != L2FTUNNEL_OPEN) {
		buginf("\nL2F NAS received tunnel OPEN before sending one");
	    }

	    /*
	     * Clean out our OPEN from the resend queue
	     */
	    l2f_clean_resendQ(tunnel, L2F_OPEN, L2F_MANAGEMENT_MID);

            /*
             * Start the first MID open 
             */
            l2f_activate_waiting_mids(tunnel);

	    if (tunnel->gw_password[0]) {
		chap_hash(tunnel->gw_password, tunnel->gw_clid %256, 
			tunnel->nas_challenge, 
			tunnel->nas_challenge_length, outhash);

		if (bcmp(outhash, info->chapresp, MD5_LEN) == 0) {
		    passed_auth = TRUE;
		}
	    } else {
	        passed_auth = vpn_chapstyle_auth(tunnel->gw_name, 
			tunnel->nas_challenge, tunnel->nas_challenge_length, 
			info->chapresp, tunnel->gw_clid %256);
	    }


	    if (!passed_auth) {
		/*
		 * Tunnel fails authentication
		 */
		if (l2f_error_debug) {
		    buginf("\nL2F Tunnel authentication failed for %s",
			tunnel->gw_name);
		}
	        l2f_initiate_tunnel_shutdown(tunnel);
		l2f_clear_tunnel(tunnel);
	    } else {
		/*
		 * Tunnel passes authentication 
		 */
		if (l2f_error_debug) {
		    buginf("\nL2F Tunnel authentication succeeded for %s",
			tunnel->gw_name);
		}
	    }

	} else {
	    if (tunnel->state != L2FTUNNEL_OPENING) {
		if (l2f_error_debug) {
		  buginf("\nL2F Gateway received tunnel OPEN while in state %s",
			l2f_tunnel_state_string(tunnel->state));
		}
	    } else {
		/*
		 * Clean out our CONF from the resend queue
		 */
		l2f_clean_resendQ(tunnel, L2F_CONF, L2F_MANAGEMENT_MID);

		if (tunnel->nas_password[0]) {
		    chap_hash(tunnel->nas_password, tunnel->nas_clid %256, 
			    tunnel->gw_challenge, 
			    tunnel->gw_challenge_length, outhash);
		    if (bcmp(outhash, info->chapresp, MD5_LEN) == 0) {
			passed_auth = TRUE;
		    }
		} else {
		    passed_auth = vpn_chapstyle_auth(tunnel->nas_name, 
			tunnel->gw_challenge, tunnel->gw_challenge_length, 
			info->chapresp, tunnel->nas_clid%256);

		}

		if (!passed_auth) {
		    /*
		     * Tunnel fails authentication
		     */
		    if (l2f_error_debug) {
			buginf("\nTunnel authentication failed for %s",
			    tunnel->nas_name);
		    }
	            l2f_initiate_tunnel_shutdown(tunnel);
		    l2f_clear_tunnel(tunnel);
		} else {
		    /*
		     * Tunnel passes authentication 
		     */
		    len = l2f_gw2nas_tunnel1(tunnel, buf, MAX_L2FMANAGE_SIZE);
		    if (len > 0)  {
			l2f_tunnel_move_to_state(tunnel, L2FTUNNEL_OPEN);
			l2f_send(tunnel, buf, len, INITIAL_SEND);
		    }
		    if (l2f_error_debug) {
			buginf("\nL2F Tunnel authentication succeeded for %s",
			    tunnel->nas_name);
		    }
		} 
	    }
	}
	free(info);
	free(buf);
	return;
    }

    if (l2f_event_debug)
	buginf("\nL2F Got a MID management packet");

    if (tunnel->state != L2FTUNNEL_OPEN) {
	if (l2f_event_debug)
	    buginf("\nL2F ...but discarded it because the tunnel wasn't open.");
	free(info);
	free(buf);
	return;
    }

    mid = l2f_get_mid(GETSHORT(&header->clid), header->mid);

    if (!tunnel->instigator) {

        /*
         * If there is a MID already in the OPEN state, then 
         * the previous OPEN to the NAS was lost or has not been 
         * received by the NAS. Resend the L2F_OPEN 
         */
        if (mid) {
	   l2f_clean_resendQ(tunnel, L2F_OPEN, mid->mid);
	   l2f_mid_move_to_state(mid, L2FMID_OPEN);
	   len = l2f_gw2nas_mid0(mid, buf, MAX_L2FMANAGE_SIZE);
           l2f_send(tunnel, buf, len, INITIAL_SEND);
	   free(info);
	   free(buf);
	   return;
        }

	/*
	 * Clean out our tunnel OPEN from the resend queue
	 */
	l2f_clean_resendQ(tunnel, L2F_OPEN, L2F_MANAGEMENT_MID);

	/*
	 *  get new mid and populate it
	 */
	mid = l2f_make_mid_structure(tunnel);
	mid->packets_in++;
	mid->bytes_in += header->length;
	mid->mid = header->mid;
	setstring(&mid->name, info->chapname);
	l2f_store_mid(tunnel->nas_clid, mid->mid, mid);
	if (vpn_get_mid_interface(
			&status, 
			mid->name,
			auth_type,
			chap_id,
			chapchallen,
			info->chapchal,
			chapresplen,
			info->chapresp,
			100,
			rcv_ack_len,
			info->rcv_ack,
			snt_ack_len,
			info->snt_ack,
			mid)) {
	    /* 
	     * we got a MID interface, keep this connection
	     */
	    len = l2f_gw2nas_mid0(mid, buf, MAX_L2FMANAGE_SIZE);
	    if (len > 0)  {
		l2f_mid_move_to_state(mid, L2FMID_OPEN);
		l2f_send(tunnel, buf, len, INITIAL_SEND);
	    }
	} else {
	    /*
	     * we failed to get a MID interface, blow away the MID
	     */
	    len = l2f_pkt_close_mid(mid, buf, MAX_L2FMANAGE_SIZE, status);
	    if (len > 0)  {
		l2f_send(tunnel, buf, len, INITIAL_SEND);
		l2f_mid_move_to_state(mid, L2FMID_CLOSING);
	    }
	    l2f_destroy_mid_structure(mid, TRUE);
	}
    } else {
	/* 
         *  We are the instigator. Remove the resend queue.
         *  Complete the end-to-end PPP connection state 
	 *  Note that if  we don't recognize this MID, we dump the packet.
	 */
        if (mid) {
           if (l2f_clean_resendQ(tunnel, L2F_OPEN, mid->mid)) {
	      l2f_mid_move_to_state(mid, L2FMID_OPEN);

              if (l2f_event_debug)
                 buginf("\nL2F MID synced NAS/HG Clid=%d/%d Mid=%d on %s",
                     mid->owner->nas_clid, mid->owner->gw_clid, 
                     mid->mid, mid->idb->hw_namestring);
 
              /*
               * The PPP link has now been completely forwarded
               */
             reg_invoke_ppp_set_lcpstate(mid->idb, FORWARDED);
           } else if (l2f_error_debug) {
	      /*
               * The L2F_OPEN arrived but the resend Q has no match. 
               * This is a stray packet. Since the Home-Gateway *always* 
               * sends successive retries (no ACK from the NAS to shut it up),
               * we'll just ignore it. 
               */
              buginf("\nL2F MID already synced NAS/HG Clid=%d/%d Mid=%d on %s",
                     mid->owner->nas_clid, mid->owner->gw_clid, 
                     mid->mid, mid->idb->hw_namestring);
	   }
        }
    }
    free(info);
    free(buf);
    return;

}

/*
 * l2f_receive_l2f_close
 *
 *  Handle an L2F_CLOSE packet received from the other side.
 */
void l2f_receive_l2f_close(l2f_headertype *header, uchar *payload, int payload_size) 
{
    int count, len;
    uchar *buf;
    l2f_midtype *mid;
    l2f_tunneltype *tunnel;

    buf = malloc(MAX_L2FMANAGE_SIZE);
    if (!buf)
	return;

    for (count = 1; count < payload_size; count++) {
	switch(payload[count]) {
	    case L2F_CLOSE_WHY:
		if (l2f_packet_debug)
		    buginf("\nL2F_CLOSE_WHY");
		count += sizeof(long);
		break;
	    case L2F_CLOSE_STR:
		if (l2f_packet_debug)
		    buginf("\nL2F_CLOSE_STR (receiving and ignoring)");
		count++;
		count += GETSHORT(&payload[count])+1;  
					/* +1 to account for the */
				        /*	 second length byte  */
		break;
	    default:
		if (l2f_error_debug)
		    buginf("\nL2F Bogus L2F_CLOSE suboption");
	}
    }

    if (header->mid == L2F_MANAGEMENT_MID) {
	if (l2f_event_debug)
	    buginf("\nL2F Got closing for tunnel ");
	tunnel = l2f_find_tunnel_by_clid(GETSHORT(&header->clid));
	if (tunnel == NULL) {
	    free(buf);
	    return;
	}

        if (tunnel->state == L2FTUNNEL_CLOSED) {
           return;
	} else if (tunnel->state == L2FTUNNEL_CLOSING) {
	    /* We instigated this and are getting confirmation */
	} else {
	    /* this is an external request.  Answer it */
	    len = l2f_pkt_close_tunnel(tunnel, buf, MAX_L2FMANAGE_SIZE);
	    if (len > 0)  {
		l2f_send(tunnel, buf, len, INITIAL_SEND);
	    }
	}

	l2f_clear_tunnel(tunnel);

	free(buf);
	return;
    } 

    mid = l2f_get_mid(GETSHORT(&header->clid), header->mid);
    if (mid == NULL) {
	if (l2f_error_debug)
	    buginf("\nL2F Received a close request for a non-existent mid");
	free(buf);
	return;
    }

    if (mid->state == L2FMID_CLOSING) {
	/* we instigated the closing and are getting confirmation */
    } else {
	/* this is a request to destroy mid, answer them */
	len = l2f_pkt_close_mid(mid, buf, MAX_L2FMANAGE_SIZE, VPN_SUCCESS);
	if (len > 0)  {
	    l2f_send(mid->owner, buf, len, INITIAL_SEND);
	}
    }
    if (l2f_event_debug)
	buginf("\nL2F Destroying mid");
    l2f_destroy_mid_structure(mid, TRUE);
    free(buf);
}

/*
 * l2f_receive_l2f_echo
 *
 *  Handle an L2F_ECHO packet received from the other side.
 */
void l2f_receive_l2f_echo(l2f_headertype *header, uchar *payload, int payload_size)
{
    int length;
    uchar *buf;
    l2f_tunneltype *tunnel;


    tunnel = l2f_find_tunnel_by_clid(GETSHORT(&header->clid));
    if (tunnel == NULL) {
        return;
    }

    buf = malloc(MAX_L2FMANAGE_SIZE);
    if (!buf)
	return;
 
    if (tunnel) {
	length = l2f_echo_resp(tunnel, buf, MAX_L2FMANAGE_SIZE);
	if (length > 0) {
	    l2f_send(tunnel, buf, length, INITIAL_SEND);
	}
    }

    free(buf);
}
