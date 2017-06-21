/* $Id: x25_proto.c,v 3.4.10.3 1996/06/01 07:19:53 tylin Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_proto.c,v $
 *------------------------------------------------------------------
 * x25_proto.c -- X25 support for higher level protocols, IP, etc.
 *
 * February 1988, Greg Satz
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_proto.c,v $
 * Revision 3.4.10.3  1996/06/01  07:19:53  tylin
 * CSCdi53715:  CDP is not working over Fr-Rel, X25 and SMDS interfaces
 *    Add new CUD (0xDA) for CDP
 * Branch: California_branch
 *
 * Revision 3.4.10.2  1996/05/23  07:38:49  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.4.10.1  1996/03/18  22:50:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.4  1996/03/16  07:57:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.3  1996/03/13  02:13:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  11:19:51  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:56:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/27  05:15:44  carvalho
 * CSCdi45673:  Bus Error in x25_sendcall - trying to use DDN X25.
 * Test lci->lci_map for NULL before dereferencing it.
 *
 * Revision 3.3  1995/11/17  18:07:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:52:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:53:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/04  22:02:32  gglick
 * CSCdi37951:  X.25/BFE X.121 address generation incorrect
 *
 * Revision 2.1  1995/06/07  23:22:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "../ui/debug.h"
#include "address.h"
#include "packet.h"
#include "config.h"
#include "../wan/serial.h"
#include "../if/network.h"
#include "ieee.h"
#include "../ip/ip.h"
#include "../iprouting/route.h"
#include "x25.h"
#include "x25_address.h"
#include "x25_proto.h"
#include "x25_bfe.h"
#include "x25_debug.h"
#include "x25_pbp.h"

/*
 *  Protocol Identification encodings
 *
 *  PIDs are encoded:
 *  - in Call User Data (CUD), indicating the proposed datagram protocol
 *    to be encapsulated on the VC, and
 *  - for VCs that encapsulate arbitrary protocols, a PID is encoded at
 *    the beginning of each Complete Packet Sequence (CPS); the PID
 *    identifies the protocol carried in remainder of the CPS.
 *
 *  Various authorities have defined PID encodings that Cisco supports,
 *  particularly RFC 877, RFC 1356 (which obsoletes RFC 877), the X.29
 *  PAD Recommendation and ISO/IEC 8473.  There is also a Draft
 *  Recommendation from the ITU-T referred to as Draft X.mpe (for the
 *  X series Draft Recommendation for Multi PDU Encapsulation) which
 *  is *not* supported, but is noted for evaluation and tracking as it
 *  evolves.
 *
 *  The IP CUD value is consistently defined in RFC 877, the DDN and BFE
 *  specifications, RFC 1356 and the Draft X.mpe.
 *
 *  The CLNP familiy of protocols are encapsulated using the CUD
 *  values specified in ISO/IEC 8473.
 *
 *  The PAD CUD value is defined by X.29.
 *
 *  Cisco defines proprietary CUD values for proposing encapsulation of
 *  Apollo, Appletalk, Bridging (Cisco-defined, IEEE Spanning Tree and
 *  DEC Spanning Tree), Compressed TCP, DECnet, Novell and XNS datagrams.
 *
 *  Banyan defines a proprietary CUD value for proposing encapsulation
 *  of VINES datagrams.
 *
 *  RFC 1356 defines standard CUD values for proposing encapsulation of
 *  most (if not all) forms of ethernet traffic.  The Draft X.mpe echos
 *  this usage.
 *
 *  RFC 1356 also defines a standard CUD value for proposing a VC that
 *  can carry arbitrary protocols; each Complete Packet Sequence carried
 *  over this type of VC has a leading PID encoded in a fixed-length
 *  version of the CUD PID encodings.  The Draft X.mpe echos this usage.
 *
 *  The Draft X.mpe has some unique features.  Cisco is tracking the
 *  devlopment of this standard, but has not implemented it.  One
 *  feature is a mechanism that would allow multiple datagram PDUs to be
 *  represented in a single CPS of a multi-protocol VC.  Another feature
 *  defines a method of carrying Frame Relay traffic.
 */

/*
 *  CUD PID constants
 */

/*
 *  The 1988 CCITT Recommendation X.244 assigns four value ranges to
 *  the first byte of Call User Data and Called User Data (defined by
 *  the value of the two high order bits) according to the authorites
 *  that may assign meaning to the values of a particular range.
 */
#define	X25_CUD_AUTHORITY_M		0xC0
#define	X25_CUD_AUTHORITY_CCITT		0x00	/* CCITT usage */
#define	X25_CUD_AUTHORITY_ADMIN		0x40	/* Administrations */
#define	X25_CUD_AUTHORITY_IUB		0x80	/* International user bodies */
#define	X25_CUD_AUTHORITY_ANY		0xC0	/* no constraints */

/*
 *  CCITT-defined CUD protocol identification values
 */
#define	X25_MULTI_ENC_PROTOCOL_ID	0x00	/* RFC 1356 & X.mpe Draft */
#define	X25_PAD_PROTOCOL_ID		0x01	/* defined by X.29 */
#define	X25_TP_PROTOCOL_ID		0x03	/* defined by X.224 */
#define	X25_FR_PROTOCOL_ID		0x08	/* defined by X.mpe Draft */

/*
 *  International user body CUD protocol identification values
 */
#define	X25_SNAP_PROTOCOL_ID		0x80	/* RFC 1356 & X.mpe Draft */
#define	X25_CLNP_PROTOCOL_ID		0x81	/* defined by ISO/IEC 8473 */
#define	X25_ESIS_PROTOCOL_ID		0x82	/* defined by RFC-1356 */
#define	X25_ISIS_PROTOCOL_ID		0x83	/* defined by X.mpe Draft */

/*
 *  other CUD protocol identification values
 */
#define	X25_VINES_PROTOCOL_ID		0xC0	/* -partial PID- Banyan */
#define	X25_QLLC_PROTOCOL_ID		0xC3	/* IBM's QLLC */
#define	X25_IP_PROTOCOL_ID		0xCC	/* RFC 877 & 1356 & X.mpe */
#define	X25_OBSOLETE_PUP_PROTOCOL_ID	0xCE	/* OBSOLETE Cisco proprietary */
#define	X25_OBSOLETE_CHAOS_PROTOCOL_ID	0xCF	/* OBSOLETE Cisco proprietary */
#define	X25_DECNET_PROTOCOL_ID		0xD0	/* Cisco proprietary */
#define	X25_XNS_PROTOCOL_ID		0xD1	/* Cisco proprietary */
#define X25_APPLETALK_PROTOCOL_ID	0xD2	/* Cisco proprietary */
#define	X25_NOVELL_PROTOCOL_ID		0xD3	/* Cisco proprietary */
#define	X25_APOLLO_PROTOCOL_ID		0xD4	/* Cisco proprietary */
#define X25_BRIDGE_PROTOCOL_ID          0xD5	/* Cisco proprietary */
#define X25_OBSOLETE_IEEE_PROTOCOL_ID   0xD6	/* OBSOLETE Cisco proprietary*/
#define X25_OBSOLETE_DEC_PROTOCOL_ID    0xD7	/* OBSOLETE Cisco proprietary*/
#define X25_THC_PROTOCOL_ID		0xD8	/* Cisco proprietary */
/* note: X25_PBP_PROTOCOL_ID		0xD9	-- see x25_pbp.h */
#define X25_CDP_PROTOCOL_ID		0xDA	/* Cisco proprietary */

/*
 *  Protocol Identification constants
 */

/*
 *  a Protocol Identifier may specify the protocol in as few as 1 byte
 */
#define X25_PID_LEN		1	/* Protocol ID length */
#define X25_PID_OFFSET		0	/* Protocol ID offset */

/*
 *  ISO/IEC 8473-defined CUD values
 *  The first VC encoding CLNP has a single CUD byte identifying the protocol.
 *  Subsequent VCs need to encode, in addition to the PID, a Length
 *  Indication, SNCR Version and SNCD Value.
 */
#define X25_CLNP_LI_LEN		1
#define X25_CLNP_LI_OFFSET	X25_PID_LEN
#define X25_CLNP_LI		4
#define X25_CLNP_VER_LEN	1
#define X25_CLNP_VER_OFFSET	(X25_PID_LEN + X25_CLNP_LI_LEN)
#define X25_CLNP_VER		2
#define X25_CLNP_SNCR_LEN	2
#define X25_CLNP_SNCR_OFFSET	(X25_PID_LEN + X25_CLNP_LI_LEN +	\
				 X25_CLNP_VER_LEN)

/*
 *  IETF/RFC 1356 SNAP encoding values
 *  After the SNAP PID, an Organizationally Unique Identifier and
 *  the organization's Protcol Type are encoded
 */
#define X25_SNAP_OUI_LEN	3
#define X25_SNAP_OUI_OFFSET	X25_PID_LEN
#define X25_SNAP_TYPE_LEN	2
#define X25_SNAP_TYPE_OFFSET	(X25_PID_LEN + X25_SNAP_OUI_LEN)

/*
 *  X.29's PAD PID
 */
#define X25_PAD_PADING_LEN	3
#define X25_PAD_PADING_OFFSET	X25_PID_LEN

/*
 *  Banyan VINES' propretary PID
 *  requires the PID followed by a zero byte (significance unknown)
 *  followed by their two-byte ethertype ID
 */
#define X25_VINES_PADING_LEN	1
#define X25_VINES_PADING_OFFSET	X25_PID_LEN
#define X25_VINES_TYPE_LEN	2
#define X25_VINES_TYPE_OFFSET	(X25_PID_LEN + X25_VINES_PADING_LEN)

/*
 *  the minimum CUD length encoding a PID is set to that required
 *  by X.29 unless a particular application explicitly requires a
 *  smaller size
 */
#define X25_CUD_MIN_LEN		4		/* from X.29 */
#define X25_BFE_CUD_LEN		X25_PID_LEN	/* BFE is an exception */
#define X25_CLNP_CUD_LEN	X25_PID_LEN	/* ISO 8473, first VC */
#define X25_CLNP_SNDCF_CUD_LEN	(X25_PID_LEN + X25_CLNP_LI_LEN +	\
				 X25_CLNP_VER_LEN + X25_CLNP_SNCR_LEN)
#define X25_SNAP_CUD_LEN	(X25_PID_LEN + X25_SNAP_OUI_LEN +	\
				 X25_SNAP_TYPE_LEN)
#define X25_VINES_CUD_LEN	(X25_PID_LEN + X25_VINES_PADING_LEN +	\
				 X25_VINES_TYPE_LEN)

#define X25_MAX_CUD_LEN		X25_SNAP_CUD_LEN

/*
 *  x25_encaps_maptype
 *  Determine the map type to search for from a given link type
 */
uint x25_encaps_maptype (uint pak_linktype)
{
    switch (pak_linktype) {
      /*
       *  X.25 can encapsulate the following protocols ...
       */
      case LINK_APOLLO:
      case LINK_APPLETALK:
      case LINK_BRIDGE:
      case LINK_CDP:
      case LINK_CLNS:
      case LINK_COMPRESSED_TCP:
      case LINK_DECNET:
      case LINK_IP:
      case LINK_NOVELL:
      case LINK_VINES:
      case LINK_XNS:
      case LINK_X25_MULTI_ENC:
      case LINK_PAD:
      case LINK_QLLC:
	return(pak_linktype);
      /*
       *  ... and the following link types as aliased to a supported protocol
       */
      case LINK_DECNET_ROUTER_L1:
      case LINK_DECNET_ROUTER_L2:
      case LINK_DECNET_NODE:
      case LINK_DECNET_PRIME_ROUTER:
	return(LINK_DECNET);
      case LINK_DEC_SPANNING:
      case LINK_IEEE_SPANNING:
	return(LINK_BRIDGE);
      case LINK_CLNS_ALL_ES:
      case LINK_CLNS_ALL_IS:
      case LINK_ISIS_ALL_L1_IS:
      case LINK_ISIS_ALL_L2_IS:
      case LINK_CLNS_BCAST:
	return(LINK_CLNS);
      case LINK_UNCOMPRESSED_TCP:
	return(LINK_COMPRESSED_TCP);
    }
    return(LINK_ILLEGAL);
}

/*
 *  x25_encaps_form_cud_pid
 *  Generate the Call User Data to encode the particular encapsulation
 *  protocol identifier required by a given map.
 */
void x25_encaps_form_cud_pid (
    lcitype *lci,
    uchar *cud,
    uchar *len)
{
    x25map *map;
    uint map_type;
    ushort method;
    ushort snap_id;
    uint inx;

    map = lci->lci_map;
    map_type = x25_encaps_maptype(lci->lci_linktype);

    if (map && (map->map_services & X25_MAPE_CISCO_ONLY)) {
	method = X25_CUD_PID_CISCO;
    } else if ((map == NULL) ||
	       (map->map_cud_pid_method == X25_CUD_PID_DEFAULT)) {
	method = lci->lci_idb->hwptr->x25_cud_pid_def;
    } else {
	method = map->map_cud_pid_method;
    }

    if ((map_type == LINK_ILLEGAL) ||
	(method == X25_CUD_PID_NONE)) {
	*len = 0;
	return;
    }

    /*
     *  set the length to the most common value
     */
    *len = X25_CUD_MIN_LEN;
    memset(cud, 0, X25_CUD_MIN_LEN);

    if ((map_type == LINK_X25_MULTI_ENC) ||
	(method == X25_CUD_PID_MULTI) ||
	((method != X25_CUD_PID_CISCO) &&
	 map && (map->map_services & X25_MAPE_FORCE_MULTI_ENC))) {
	/*
	 *  Some non-Cisco encapsulations require a multiprotocol VC
	 */
	cud[X25_PID_OFFSET] = X25_MULTI_ENC_PROTOCOL_ID;
    } else if ((map_type == LINK_IP) && (method != X25_CUD_PID_SNAP)) {
	cud[X25_PID_OFFSET] = X25_IP_PROTOCOL_ID;

	/*
	 *  BFE requires a single byte of IP CUD encoding
	 */
	if (ISBFE(lci->lci_idb->hwptr))
	    *len = X25_BFE_CUD_LEN;
    } else if ((map_type == LINK_CLNS) && (method != X25_CUD_PID_SNAP)) {
	cud[X25_PID_OFFSET] = X25_CLNP_PROTOCOL_ID;

	/*
	 *  ISO/IEC 8473 requires an SNCR encoding if more than one VC
	 *  is opened
	 */
	if ((map == NULL) || (map->map_maxnvc == 1)) {
	    *len = X25_CLNP_CUD_LEN;
	} else {
	    *len = X25_CLNP_SNDCF_CUD_LEN;
	    cud[X25_CLNP_LI_OFFSET] = X25_CLNP_LI;
	    cud[X25_CLNP_VER_OFFSET] = X25_CLNP_VER;
	    cud[X25_CLNP_SNCR_OFFSET] = (map->map_nvc - 1) & 0xff;
	    cud[X25_CLNP_SNCR_OFFSET + 1] = ((map->map_nvc - 1) >> 8) & 0xff;
	}
    } else if (map_type == LINK_PAD) {
	cud[X25_PID_OFFSET] = X25_PAD_PROTOCOL_ID;
    } else if (map_type == LINK_QLLC) {
	cud[X25_PID_OFFSET] = X25_QLLC_PROTOCOL_ID;
    } else if (method == X25_CUD_PID_CISCO) {
	/*
	 *  encode a Cisco-proprietary (i.e. backwards compatible) CUD field
	 */
	switch (map_type) {
	  case LINK_APOLLO:
	    cud[X25_PID_OFFSET] = X25_APOLLO_PROTOCOL_ID;
	    break;
	  case LINK_APPLETALK:
	    cud[X25_PID_OFFSET] = X25_APPLETALK_PROTOCOL_ID;
	    break;
	  case LINK_BRIDGE:
	    cud[X25_PID_OFFSET] = X25_BRIDGE_PROTOCOL_ID;
	    break;
	  case LINK_CDP:
	    cud[X25_PID_OFFSET] = X25_CDP_PROTOCOL_ID;
	    break;
	  case LINK_COMPRESSED_TCP:
	    cud[X25_PID_OFFSET] = X25_THC_PROTOCOL_ID;
	    break;
	  case LINK_DECNET:
	    cud[X25_PID_OFFSET] = X25_DECNET_PROTOCOL_ID;
	    break;
	  case LINK_NOVELL:
	    cud[X25_PID_OFFSET] = X25_NOVELL_PROTOCOL_ID;
	    break;
	  case LINK_VINES:
	    *len = X25_VINES_CUD_LEN;
	    cud[X25_PID_OFFSET] = X25_VINES_PROTOCOL_ID;
	    cud[X25_VINES_PADING_OFFSET] = 0;
	    cud[X25_VINES_TYPE_OFFSET] = (TYPE_VINES2 >> 8) & 0xff;
	    cud[X25_VINES_TYPE_OFFSET + 1] = TYPE_VINES2 & 0xff;
	    break;
	  case LINK_XNS:
	    cud[X25_PID_OFFSET] = X25_XNS_PROTOCOL_ID;
	    break;
	  default:
	    *len = 0;
	}
    } else {
	/*
	 *  encode an IETF/RFC-1356 CUD using SNAP
	 */
	if ((snap_id = link2serial(lci->lci_linktype)) == 0) {
	    /*
	     *  bad news--unable to encode
	     */
	    *len = 0;
	} else {
	    *len = X25_SNAP_CUD_LEN;

	    cud[X25_PID_OFFSET] = X25_SNAP_PROTOCOL_ID;

	    ieee_copy_oui(zerosnap, &cud[X25_SNAP_OUI_OFFSET]);

	    cud[X25_SNAP_TYPE_OFFSET] = (snap_id >> 8) & 0xff;
	    cud[X25_SNAP_TYPE_OFFSET + 1] = snap_id & 0xff;
	}
    }

    /*
     * insert a leading packet compression protocol ID if specified
     * (prohibited for PAD maps)
     */
    if ((map_type != LINK_PAD) &&
	(map != NULL) && (map->map_flags & X25_MAPF_PBP)) {
	/*
	 *  insert the compression PID byte in front of the
	 *  constructed PID
	 */
	for (inx = *len; inx; inx--)
	    cud[inx] = cud[inx - 1];
	cud[X25_PID_OFFSET] = X25_PBP_PROTOCOL_ID;
	*len += 1;
    }
}

/*
 *  x25_encaps_form_data_pid
 *  Attach the protocol identification preamble to an outgoing datagram.
 *  Returns FALSE if encoding failed.
 */
boolean x25_encaps_form_data_pid (
    paktype *pak,
    uchar data_pid_method)
{
    ushort snap_id;
    ushort link_type;
    ushort map_type;

    if (data_pid_method == X25_DATA_PID_NONE)
	return(TRUE);

    link_type = pak->linktype;

    /*
     *  over-ride the packet linktype if we've identified a THC datagram
     */
    if ((link_type == LINK_IP) && (pak->x25_flags & PAK_X25_ENC))
	link_type = LINK_UNCOMPRESSED_TCP;

    if ((map_type = x25_encaps_maptype(link_type)) == LINK_ILLEGAL)
	return(FALSE);

    if ((map_type == LINK_IP) &&
	(data_pid_method == X25_DATA_PID_MULTI)) {
	/*
	 *  use the short IP identifier
	 */
	pak->encsize += X25_PID_LEN;
	pak->datagramstart -= X25_PID_LEN;
	pak->datagramsize += X25_PID_LEN;
	pak->datagramstart[X25_PID_OFFSET] = X25_IP_PROTOCOL_ID;
    } else if ((map_type == LINK_CLNS) &&
	       (data_pid_method == X25_DATA_PID_MULTI)) {
	/*
	 *  use the short CLNP identifier--
	 *  Note that the RFC-1356 ESIS PID is not used (nor the X.mpe ISIS)
	 */
	pak->encsize += X25_PID_LEN;
	pak->datagramstart -= X25_PID_LEN;
	pak->datagramsize += X25_PID_LEN;
	pak->datagramstart[X25_PID_OFFSET] = X25_CLNP_PROTOCOL_ID;
    } else {
	/*
	 *  datagram must be identified using SNAP
	 */
	if ((snap_id = link2serial(link_type)) == 0)
	    return(FALSE);	/* unable to encode */

	pak->encsize += X25_SNAP_CUD_LEN;
	pak->datagramstart -= X25_SNAP_CUD_LEN;
	pak->datagramsize += X25_SNAP_CUD_LEN;

	pak->datagramstart[X25_PID_OFFSET] = X25_SNAP_PROTOCOL_ID;
	ieee_copy_oui(zerosnap, &pak->datagramstart[X25_SNAP_OUI_OFFSET]);
	pak->datagramstart[X25_SNAP_TYPE_OFFSET] = (snap_id >> 8) & 0xff;
	pak->datagramstart[X25_SNAP_TYPE_OFFSET + 1] = snap_id & 0xff;
    }

    return(TRUE);
}

/*
 *  x25_encaps_parse_cud_pid
 *  Determine the protocol identified by a Call User Data (CUD) field.
 *  Returns LINK_ILLEGAL if decode failed.
 *
 *  Side effects: sets *pid_len to the length of the protocol encoding,
 *  sets *compress to indicate compression usage, and (if *lci isn't NULL)
 *  sets lci->lci_cud_pid_method to the protocol identification method.
 */
uint x25_encaps_parse_cud_pid (
    lcitype *lci,
    uchar *cud,
    uint cud_len,
    uint *pid_len,
    boolean *compress)
{
    uint call_type;
    uchar cud_pid_method;

    /*
     *  initialize the PID length and compression flag
     */
    *pid_len = 0;
    *compress = (cud_len >= X25_PID_LEN) &&
		(cud[X25_PID_OFFSET] == X25_PBP_PROTOCOL_ID);

    if (*compress) {
	/*
	 *  compression is a preamble to a PID
	 */
	cud++;
	cud_len--;
    }

    /*
     *  set a default CUD PID method
     */
    cud_pid_method = X25_CUD_PID_CISCO;

    /*
     *  decode the PID
     */
    if (cud_len < X25_PID_LEN) {
	call_type = LINK_ILLEGAL;
    } else {
	switch (cud[X25_PID_OFFSET]) {
	  /*
	   *  IETF/RFC-1356 multiprotocol encapsulation PID
	   */
	  case X25_MULTI_ENC_PROTOCOL_ID:
	    call_type = LINK_X25_MULTI_ENC;
	    cud_pid_method = X25_CUD_PID_MULTI;
	    break;

	  /*
	   *  standardized single-protocol encapsulation PIDs
	   */
	  case X25_IP_PROTOCOL_ID:
	    call_type = LINK_IP;
	    if (lci &&
		(lci->lci_idb->hwptr->x25_cud_pid_def != X25_CUD_PID_CISCO))
		cud_pid_method = X25_CUD_PID_IETF;
	    break;
	  case X25_CLNP_PROTOCOL_ID:
	    /*
	     *  don't validate any encoded SNCR, just determine its length
	     */
	    if ((cud_len >= X25_CLNP_SNDCF_CUD_LEN) &&
		(cud[X25_CLNP_LI_OFFSET] == X25_CLNP_LI) &&
		(cud[X25_CLNP_VER_OFFSET] == X25_CLNP_VER))
		*pid_len = X25_CLNP_SNDCF_CUD_LEN;
	    else
		*pid_len = X25_PID_LEN;
	    /* FALL THROUGH */
	  case X25_ESIS_PROTOCOL_ID:
	    call_type = LINK_CLNS;
	    if (lci &&
		(lci->lci_idb->hwptr->x25_cud_pid_def != X25_CUD_PID_CISCO))
		cud_pid_method = X25_CUD_PID_IETF;
	    break;
	  case X25_PAD_PROTOCOL_ID:
	    if ((cud_len >= X25_CUD_MIN_LEN) &&
		(cud[X25_PAD_PADING_OFFSET] == 0) &&
		(cud[X25_PAD_PADING_OFFSET + 1] == 0) &&
		(cud[X25_PAD_PADING_OFFSET + 2] == 0)) {
		call_type = LINK_PAD;
		if (lci &&
		    (lci->lci_idb->hwptr->x25_cud_pid_def != X25_CUD_PID_CISCO))
		    cud_pid_method = X25_CUD_PID_IETF;
	    } else {
		call_type = LINK_ILLEGAL;
	    }
	    break;

	  /*
	   *  standardized single-protocol SNAP PID
	   */
	  case X25_SNAP_PROTOCOL_ID:
	    call_type = LINK_ILLEGAL;	/* default */

	    if (cud_len >= X25_SNAP_CUD_LEN) {
		*pid_len = X25_SNAP_CUD_LEN;

		if ((cud[X25_SNAP_OUI_OFFSET] == 0) ||
		    (cud[X25_SNAP_OUI_OFFSET + 1] == 0) ||
		    (cud[X25_SNAP_OUI_OFFSET + 2] == 0)) {
		    call_type = serial2link(
				((cud[X25_SNAP_TYPE_OFFSET] << 8) |
				 cud[X25_SNAP_TYPE_OFFSET + 1]) & 0xffff);

		    /*
		     *  disallow if a multiprotocol VC is required or
		     *  RFC 1356 encapsulation isn't supported
		     */
		    if ((call_type != LINK_ILLEGAL) &&
			(x25_map_link2service(call_type) &
			 (X25_MAPE_FORCE_MULTI_ENC | X25_MAPE_CISCO_ONLY)))
			call_type = LINK_ILLEGAL;
		}
	    }

	    cud_pid_method = X25_CUD_PID_SNAP;
	    break;

	  /*
	   *  Cisco proprietary CUD PIDs
	   */
	  case X25_DECNET_PROTOCOL_ID:
	    call_type = LINK_DECNET;
	    break;
	  case X25_XNS_PROTOCOL_ID:
	    call_type = LINK_XNS;
	    break;
	  case X25_APPLETALK_PROTOCOL_ID:
	    call_type = LINK_APPLETALK;
	    break;
	  case X25_NOVELL_PROTOCOL_ID:
	    call_type = LINK_NOVELL;
	    break;
	  case X25_APOLLO_PROTOCOL_ID:
	    call_type = LINK_APOLLO;
	    break;
	  case X25_BRIDGE_PROTOCOL_ID:
	    call_type = LINK_BRIDGE;
	    break;
	  case X25_THC_PROTOCOL_ID:
	    call_type = LINK_COMPRESSED_TCP;
	    break;

	  /*
	   *  other proprietary CUD PIDs
	   */
	  case X25_VINES_PROTOCOL_ID:
	    /*
	     *  validate the Banyan-specified VINES PID
	     */
	    if ((cud_len >= X25_VINES_CUD_LEN) &&
		(cud[X25_VINES_PADING_OFFSET] == 0) &&
		(cud[X25_VINES_TYPE_OFFSET] == ((TYPE_VINES2 >> 8) & 0xff)) &&
		(cud[X25_VINES_TYPE_OFFSET + 1] == (TYPE_VINES2 & 0xff))) {
		call_type = LINK_VINES;
	    } else {
		call_type = LINK_ILLEGAL;
	    }
	    break;

	  case X25_QLLC_PROTOCOL_ID:
	    call_type = LINK_QLLC;
	    break;

	  case X25_CDP_PROTOCOL_ID:
	    call_type = LINK_CDP;
	    break;

	  /*
	   *  unidentifiable CUD PID
	   */
	  default:
	    call_type = LINK_ILLEGAL;
	    break;
	}
    }

    /*
     *  make the final protocol identification and PID length determination
     */
    if (call_type == LINK_ILLEGAL) {
	cud_pid_method = X25_CUD_PID_NONE;
	*pid_len = 0;

	/*
	 *  assign the default protocol to the VC if the compression
	 *  preamble isn't specified
	 */
	if ((*compress) || (lci == NULL)) {
	    *compress = FALSE;
	} else {
	    call_type = lci->lci_idb->hwptr->x25_default;
	}
    } else {
	/*
	 *  if defaulted, determine the PID length
	 */
	if (*pid_len == 0) {
	    *pid_len = (cud_len <= X25_CUD_MIN_LEN) ? cud_len : X25_CUD_MIN_LEN;
	}

	if (*compress) {
	    *pid_len += 1;
	}
    }

    if (lci)
	lci->lci_cud_pid_method = cud_pid_method;

    return(call_type);
}

/*
 *  x25_encaps_parse_data_pid
 *  Parse and remove the PID encoded in the first data packet of a datagram's
 *  Complete Packet Sequence.  Returns LINK_ILLEGAL if the decode failed.
 */
boolean x25_encaps_parse_data_pid (
    paktype *pak,
    lcitype *lci)
{
    ushort size;
    ushort service;

    /*
     *  handle a null identification
     */
    if (pak->datagramsize < X25_PID_LEN) {
	pak->linktype = LINK_ILLEGAL;
	return(FALSE);
    }

    switch (pak->datagramstart[X25_PID_OFFSET]) {
      /*
       *  standardized non-SNAP PIDs
       */
      case X25_IP_PROTOCOL_ID:
	size = X25_PID_LEN;
	pak->linktype = LINK_IP;
	break;
      case X25_CLNP_PROTOCOL_ID:
      case X25_ESIS_PROTOCOL_ID:
	size = X25_PID_LEN;
	pak->linktype = LINK_CLNS;
	break;

      /*
       *  standardized SNAP PID
       */
      case X25_SNAP_PROTOCOL_ID:
	size = X25_SNAP_CUD_LEN;
	if ((pak->datagramsize >= X25_SNAP_CUD_LEN) &&
	    (pak->datagramstart[X25_SNAP_OUI_OFFSET] == 0) &&
	    (pak->datagramstart[X25_SNAP_OUI_OFFSET + 1] == 0) &&
	    (pak->datagramstart[X25_SNAP_OUI_OFFSET + 2] == 0))
	    pak->linktype = serial2link(
			(pak->datagramstart[X25_SNAP_TYPE_OFFSET] << 8) |
			 pak->datagramstart[X25_SNAP_TYPE_OFFSET + 1]);
	else
	    pak->linktype = LINK_ILLEGAL;
	break;

      /*
       *  unidentifiable CPS PID
       */
      default:
	size = 0;
	pak->linktype = LINK_ILLEGAL;
    }

    if ((pak->linktype != LINK_ILLEGAL) &&
	(((service = x25_map_link2service(pak->linktype)) &
	  lci->lci_map->map_services) == 0)) {
	x25map *map;

	/*
	 *  we have a valid protocol ID, but the VC's map doesn't encode
	 *  the service; the PDU is illegal *uless* there is another map
	 *  defined on the subinterface for the identified service/destination
	 */
	if (((map = x25_map_find(lci->lci_idb->hwptr,
		&lci->lci_map->map_address[X25_MAP_PRIMARY_ADDR_INX].addr,
		X25_MATCH_EXACT, service)) == NULL) ||
	    (map->map_idb != lci->lci_idb))
	    pak->linktype = LINK_ILLEGAL;
    }

    if (pak->linktype == LINK_ILLEGAL)
	return(FALSE);

    pak->network_start += size;
    pak->encsize += size;
    return(TRUE);
}

/*
 *  x25_encaps_print_cud
 *  Print the protocol identified by a Call User Data (CUD) field.
 */
void x25_encaps_print_cud (
    lcitype *lci,
    uchar *cud,
    uint cud_len,
    uint pdu)
{
    uint call_type;
    uint pid_len;
    boolean compression;
    uint i;

    if (cud_len == 0) {
	return;
    }

    /*
     *  determine the PID encoding, number of PID bytes and compression use
     */
    call_type = x25_encaps_parse_cud_pid(NULL, cud, cud_len,
					 &pid_len, &compression);

    /*
     *  print the bytes that encode the PID or, if PID encoding is unknown,
     *  print the minimum number of bytes
     */

    if (pid_len == 0) {
	pid_len = (cud_len > X25_CUD_MIN_LEN) ? X25_CUD_MIN_LEN : cud_len;
    }

    buginf("\n  %s User Data (%d): 0x",
	(pdu == X25_CALL_REQUEST) ?
		"Call" : ((pdu == X25_CALL_CONNECTED) ? "Called" : "Clear"),
	cud_len);

    for (i = 0; i < pid_len; i++) {
	buginf("%-02x", *cud++);
    }

    /*
     *  interpret the PID
     */
    if (call_type == LINK_ILLEGAL) {
	buginf(" (unknown)");
    } else {
	buginf(" (%s%s)", compression ? "Compressed ": "",
		print_linktype(call_type));
    }
}

/*
 *  x25_encaps_print_data_pid
 *  Print the PID encoded in the first data packet of a datagram's Complete
 *  Packet Sequence.
 */
void x25_encaps_print_data_pid (
    uchar *pid,
    uint data_len)
{
    uint data_type;
    uint pid_len;
    char *preamble;

    /*
     *  default number of PID bytes to print
     */
    pid_len = X25_PID_LEN;

    if (data_len < X25_PID_LEN) {
	/*
	 *  handle a null identification
	 */
	data_type = LINK_ILLEGAL;
    } else {
	switch (pid[X25_PID_OFFSET]) {
	  /*
	   *  standardized non-SNAP PIDs
	   */
	  case X25_IP_PROTOCOL_ID:
	    data_type = LINK_IP;
	    break;
	  case X25_CLNP_PROTOCOL_ID:
	  case X25_ESIS_PROTOCOL_ID:
	    data_type = LINK_CLNS;
	    break;

	  /*
	   *  standardized SNAP PID
	   */
	  case X25_SNAP_PROTOCOL_ID:
	    pid_len = X25_SNAP_CUD_LEN;
	    if ((data_len >= X25_SNAP_CUD_LEN) &&
		(pid[X25_SNAP_OUI_OFFSET] == 0) &&
		(pid[X25_SNAP_OUI_OFFSET + 1] == 0) &&
		(pid[X25_SNAP_OUI_OFFSET + 2] == 0))
		data_type = serial2link(
			(pid[X25_SNAP_TYPE_OFFSET] << 8) |
			 pid[X25_SNAP_TYPE_OFFSET + 1]);
	    else
		data_type = LINK_ILLEGAL;
	    break;

	  /*
	   *  unidentifiable CPS PID
	   */
	  default:
	    data_type = LINK_ILLEGAL;
	}
    }

    preamble = " (0x";

    while (pid_len-- && data_len--) {
	buginf("%s%-02x", preamble, *pid++);
	preamble = " ";
    }

    buginf(", %s)", print_linktype(data_type));
}

/*
 *  x25_ip_prec
 *  Determine the precedence of an IP TOS field (from the DDN Specification)
 */

uchar x25_ip_prec (register uchar tos)
{
    switch (tos & IP_TOS_PRECEDENCE) {
      case TOS_PREC_ROUTINE:
	return(0);
      case TOS_PREC_PRIORITY:
	return(1);
      case TOS_PREC_IMMEDIATE:
	return(2);
/*
 *    case TOS_PREC_FLASH:
 *    case TOS_PREC_FLASH_OVER:
 *    case TOS_PREC_CRIT_ECP:
 *    case TOS_PREC_INET_CONTROL:
 *    case TOS_PREC_NET_CONTROL:
 */
      default:
	return(3);
    }
}

/*
 *  x25_encaps_find_lci
 *  Given an upper level address, find the best existing LCI over which a
 *  packet may be sent.  From most to least preferred:
 *	0) an LCI in state D1 with an open window and RR
 *	1) an LCI in state D1 with space in its queue, a closed window and RR
 *	2) an LCI with space in its queue and:
 *		in state D1 and RNR, or
 *		in state {D2, D3}
 *	3) an LCI with space in its queue and in state {P2, P3, P5}
 *
 *  If no existing LCI can be used AND
 *    the map allows outgoing Calls AND
 *    the maxnvc limit has not been reached AND
 *    no existing LCI is in state RNR,
 *  we will attempt to open a new LCI.
 */

#define CANT_USE	5

lcitype *x25_encaps_find_lci (
    idbtype *swidb,
    paktype *pak,
    x25map *map,
    uint service)
{
    register lcitype *lci;
    hwidbtype *hwidb = swidb->hwptr;
    uint linktype;
    uchar prec;
    uint inx;
    char pref;
    char best_fit = CANT_USE;
    uchar best_fit_inx = 0;
    uchar no_new_vcs;
    leveltype level;

    linktype = x25_map_service2link(service);

    /*
     *  determine the VC precedence to use, if any
     */
    prec = X25_UNASSIGNED_PREC;
    if ((service == X25_MAPS_IP) && (hwidb->x25_flags & X25_IPPREC)) {
	iphdrtype *ip;

	ip = (iphdrtype *)pak;
	prec = x25_ip_prec(ip->tos);
	if ((prec != X25_UNASSIGNED_PREC) && (x25_debug || x25event_debug))
	    buginf("\n%s: X25 using X25 precedence %d, src %i, dst %i",
		swidb->namestring, prec, ip->srcadr, ip->dstadr);
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     *	determine if this map is at its maxnvc limit or
     *  outgoing CALLs are prohibited
     */
    no_new_vcs = (map->map_nvc >= map->map_maxnvc) ||
		 (map->map_flags & X25_MAPF_BAR_OUT);

    /*
     *  search the mapped VCs
     */
    for (inx = 0; (inx < map->map_nvc) && (lci = map->map_lci[inx]); inx++) {
	/*
	 *  prevent new VCs if a VCs in hold-down is found
	 */
	if (lci->lci_state == X25_X1) {
	    no_new_vcs = TRUE;
	    continue;
	}

	/*
	 *  skip a VC that can't transport the datagram
	 */

	/*
	 *  an IP datagram must match the VC precedence (may be UNASSIGNED)
	 */
	if ((service == X25_MAPS_IP) && (lci->lci_prec != prec))
	    continue;

	/*
	 *  multiprotocol maps may accept incoming single protocol VCs,
	 *  so check the encapsulation protocol
	 */
	if ((lci->lci_linktype != LINK_X25_MULTI_ENC) &&
	    (lci->lci_linktype != linktype))
	    continue;

	/*
	 *  determine the preference for this VC
	 */
	switch (lci->lci_state) {
	  case X25_D1:
	  case X25_P4:
	    if (!lci->lci_busy) {
		if (((lci->lci_ps - lci->lci_rpr + hwidb->x25_modulo) %
		     hwidb->x25_modulo) < lci->lci_wout) {
		    /*
		     *  this VC is ready to go--look no further!
		     */
		    map->map_index = inx;
		    reset_interrupt_level(level);
		    return(lci);
		}

		pref = QUEUEFULL(&lci->lci_holdQ) ? CANT_USE : 1;
		break;
	    }

	    /*
	     *  the VC is in RNR
	     */
	    no_new_vcs = TRUE;

	    /* FALLTHROUGH */
	  case X25_D2:
	  case X25_D3:
	    pref = (QUEUEFULL(&lci->lci_holdQ)) ? CANT_USE : 2;
	    break;

	  case X25_P2:
	  case X25_P3:
	  case X25_P5:
	    pref = (QUEUEFULL(&lci->lci_holdQ)) ? CANT_USE : 3;
	    break;

	  default:
	    pref = CANT_USE;
	}

	if (pref < best_fit) {
	    best_fit = pref;
	    best_fit_inx = inx;
	}

	/*
	 *  compressed TCP must use the first VC that can carry THC
	 */
	if (service & X25_MAPE_SINGLE_VC) {
	    no_new_vcs = TRUE;
	    break;
	}
    }

    if (best_fit != CANT_USE) {
	map->map_index = best_fit_inx;
	lci = map->map_lci[best_fit_inx];
    } else {
	lci = NULL;
    }

    reset_interrupt_level(level);

    /*
     *	return a VC capable of taking a packet, if any;
     *	NULL if an existing VC is RNR or map is at its VC limit
     */
    if (lci || no_new_vcs)
	return(lci);

    /*
     *	try to establish a new VC of the appropriate type
     */
    linktype = x25_map_service2link(map->map_services);
    return(x25_call_setup(swidb, map, NULL, NULL, linktype,
			  prec, NULL, 0, FALSE));
}

/*
 * x25_ip_ocall_setup
 * Provide special service for outgoing IP calls
 */

x25map *x25_ip_ocall_setup (
    idbtype *swidb,
    addrtype *proto_addr)
{
    x25map *map;
    addrtype x121;
    octettype *octets;
    rvntype rvn;
    uchar imp, host;
    hwidbtype *hwidb = swidb->hwptr;

    map = NULL;
    memset(&x121, 0, sizeof(addrtype));
    x121.type = ADDR_X121;

    if (ISBFE(hwidb)) {
	if (bfe_ip_parse(proto_addr->ip_addr, &rvn)) {
	    bfe_x121_form(&rvn, &x121);
	    if ((map = x25_map_create(swidb, X25_MAPS_IP, 2,
				      &x121, proto_addr))) {
		map->map_status = X25_MAP_CONSTR;
	    }
	}
    } else if (ISDDN(hwidb)) {
	octets = (octettype *)&proto_addr->ip_addr;
	imp = 0;
	host = 0;

	if ((proto_addr->ip_addr & CLASS_A_BIT) == 0) {
	    imp = octets->i;
	    host = octets->h;
	} else if ((proto_addr->ip_addr & CLASS_B_BIT) == 0) {
	    imp = octets->i;
	    host = octets->l;
	} else if ((proto_addr->ip_addr & CLASS_C_BIT) == 0) {
	    imp = octets->i & 0xf;		/* lower four bits */
	    host = (octets->i & 0xf0) >> 4;	/* upper four bits */
	}

	x121.length = X25_DDN_X121_ADDR_LEN;
	if (host < 64)
	    (void) sprintf(x121.x121_addr, "%4s%c%-03d%-02d00",
			   X25_DDN_DNIC, X25_DDN_PADDR, imp, host);
	else
	    (void) sprintf(x121.x121_addr, "%4s%c%-05d00", X25_DDN_DNIC,
			   X25_DDN_LADDR, host * 256 + imp);

	if ((map = x25_map_create(swidb, X25_MAPS_IP, 2, &x121, proto_addr)))
	    map->map_status = X25_MAP_CONSTR;
    }

    return(map);
}

/*
 * x25_ip_icall_setup
 * Provide special service for incoming IP calls
 */

x25map *x25_ip_icall_setup (
    lcitype *lci)
{
    x25map *map;
    idbtype *idb = lci->lci_idb;
    hwidbtype *hwidb = idb->hwptr;
    octettype octets;
    rvntype rvn;
    addrtype proto_addr;
    int temp;
    uchar imp, host;

    map = NULL;
    memset(&proto_addr, 0, sizeof(addrtype));
    proto_addr.type = ADDR_IP;
    proto_addr.length = ADDRLEN_IP;

    if (ISBFE(hwidb)) {
	if (bfe_x121_parse(&lci->lci_src_addr, &rvn, idb->ip_address)) {
	    bfe_ip_form(&rvn, &proto_addr.ip_addr);
	    if ((map = x25_map_create(idb, X25_MAPS_IP, 2,
					&lci->lci_src_addr, &proto_addr))) {
		map->map_status = X25_MAP_CONSTR;
	    }
	}
    } else if (ISDDN(hwidb)) {
	if (lci->lci_src_addr.x121_addr[4] == '0') {
	    imp = 100*(lci->lci_src_addr.x121_addr[5] - '0') +
		  10*(lci->lci_src_addr.x121_addr[6] - '0') +
		  (lci->lci_src_addr.x121_addr[7] - '0');
	    host = 10*(lci->lci_src_addr.x121_addr[8] - '0') +
		   (lci->lci_src_addr.x121_addr[9] - '0');
	} else {
	    temp = 10000*(lci->lci_src_addr.x121_addr[5] - '0') +
		   1000*(lci->lci_src_addr.x121_addr[6] - '0') +
		   100*(lci->lci_src_addr.x121_addr[7] - '0') +
		   10*(lci->lci_src_addr.x121_addr[8] - '0') +
		   (lci->lci_src_addr.x121_addr[9] - '0');
	    host = temp / 256;
	    imp = temp - (256 * host);
	}

	if ((idb->ip_majornet & CLASS_A_BIT) == 0) {
	    octets.n = idb->ip_majornet >> CLASS_A_SHIFT;
	    octets.h = host;
	    octets.l = 0;
	    octets.i = imp;
	} else if ((idb->ip_majornet & CLASS_B_BIT) == 0) {
	    octets.n = idb->ip_majornet >> CLASS_A_SHIFT;
	    octets.h = (idb->ip_majornet >> CLASS_B_SHIFT) & 0xff;
	    octets.l = host;
	    octets.i = imp;
	} else if ((idb->ip_majornet & CLASS_C_BIT) == 0) {
	    octets.n = (idb->ip_majornet >> CLASS_A_SHIFT) & 0xff;
	    octets.h = (idb->ip_majornet >> CLASS_B_SHIFT) & 0xff;
	    octets.l = (idb->ip_majornet >> CLASS_C_SHIFT) & 0xff;
	    octets.i = (host << 4) | imp;
	}

	proto_addr.ip_addr = *(ipaddrtype *)&octets;
	if ((map = x25_map_create(idb, X25_MAPS_IP,
				2, &lci->lci_src_addr, &proto_addr)))
	    map->map_status = X25_MAP_CONSTR;
    }

    return(map);
}

/*
 * x25_ip_call_size
 * Return number of extra bytes IP over X25 requires in CALL SETUP
 */

int x25_ip_call_size (register lcitype *lci)
{
    register int bytes;
    x25map *map;
    hwidbtype *hwidb;

    hwidb = lci->lci_idb->hwptr;

    bytes = 0;
    if (ISDDN(hwidb)) {
	bytes += (2 + 2);	/* local marker, DDN standard */
	if (lci->lci_prec != X25_UNASSIGNED_PREC)
	    bytes += 2;		/* precedence */
	map = lci->lci_map;
	if (map && map->map_bfe) {
	    if (!(hwidb->x25_bfe_status & BFE_EMERG_MODE))
	        bytes += (BFE_FACIL_SIZE * 2);
	    else 
		bytes += BFE_FACIL_SIZE;
	}
    }
    return(bytes);
}

/*
 * x25_ip_facil
 * Include necessary facilities for IP
 */

int x25_ip_facil (register lcitype *lci, register uchar *c)
{
    uchar *save = c;
    x25map *map;
    hwidbtype *hwidb;
    ipaddrtype ip;
    leveltype level;

    hwidb = lci->lci_idb->hwptr;

    if (ISDDN(hwidb)) {
	*c++ = X25_FACILITY_MARKER;
	*c++ = X25_FACILITY_MARKER_LOCAL;
	*c++ = X25_FACILITY_DDN_STANDARD;
	*c++ = X25_FACILITY_DDN_STANDARD_ON;
	if (lci->lci_prec != X25_UNASSIGNED_PREC) {
	    *c++ = X25_FACILITY_DDN_CALL_PREC;
	    *c++ = lci->lci_prec;
	}
	map = lci->lci_map;
	if (map != NULL) {
            bfetype *bfe = map->map_bfe;

	    if (bfe != NULL) {
		if (!(hwidb->x25_bfe_status & BFE_EMERG_MODE)) {
		    /* needs Enter */
		    *c++ = X25_FACILITY_DDN_BFE_ATIF;
		    *c++ = BFE_FACIL_LEN;
		    memset(c, 0, BFE_FACIL_LEN);
		    c += BFE_FACIL_LEN; /* 4 == sizeof(rvntype) */

		    level = raise_interrupt_level(NETS_DISABLE);
		    hwidb->x25_bfe_status |= BFE_EMERG_MODE;
		    reset_interrupt_level(level);
		 }
		 /* needs ATIF */

		 *c++ = X25_FACILITY_DDN_BFE_ATIF;
		 *c++ = BFE_FACIL_LEN;
		 bfe_ip_form(&bfe->rbfe_rvn, &ip);
		 bcopy(&ip, c, BFE_FACIL_LEN);
		 c += BFE_FACIL_LEN;
	    }
	}
    }
    return (c - save);
}

/*
 * x25_ip_default
 * Create a default X.25 address using DDN or BFE mapping
 * Called by SERVICE_IP_ADDRESS_CHANGE
 */

void x25_ip_default (idbtype *idb, ipaddrtype address, ipaddrtype mask,
		     boolean secondary, boolean adding)
{
    octettype *octets;
    rvntype rvn;
    uchar imp, host;
    hwidbtype *hwidb = idb->hwptr;

    /*
     * Only default the first subinterface.
     */
    if (!is_first_swidb(idb))
	return;

    if (ISBFE(hwidb)) {
	if (! idb->ip_enabled ||
	    ! bfe_ip_parse(idb->ip_address, &rvn)) {
	    printf("\nWarning: BFE X.25 will only work with a valid BFE "
		   "Class A IP address");
	    hwidb->x25_address->type = ADDR_X121;
	    hwidb->x25_address->length = 0;
	    hwidb->x25_address->x121_addr[0] = '\0';
	} else {
	    bfe_x121_form(&rvn, hwidb->x25_address);
	}
    } else if (ISDDN(hwidb) && idb->ip_enabled) {
	octets = (octettype *)&idb->ip_address;
	imp = 0;
	host = 0;

	if ((idb->ip_address & CLASS_A_BIT) == 0) {
	    imp = octets->i;
	    host = octets->h;
	} else if ((idb->ip_address & CLASS_B_BIT) == 0) {
	    imp = octets->i;
	    host = octets->l;
	} else if ((idb->ip_address & CLASS_C_BIT) == 0) {
	    imp = octets->i & 0xf;		/* lower four bits */
	    host = (octets->i & 0xf0) >> 4;	/* upper four bits */
	}

	hwidb->x25_address->type = ADDR_X121;
	hwidb->x25_address->length = X25_DDN_X121_ADDR_LEN;
	if (host < 64)
	    (void) sprintf(hwidb->x25_address->x121_addr, "%4s%c%-03d%-02d00",
			   X25_DDN_DNIC, X25_DDN_PADDR, imp, host);
	else
	    (void) sprintf(hwidb->x25_address->x121_addr, "%4s%c%-05d00",
			   X25_DDN_DNIC, X25_DDN_LADDR, host * 256 + imp);
    }
}
