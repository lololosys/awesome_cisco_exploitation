/* $Id: x25_address.h,v 3.2.62.1 1996/06/01 07:19:48 tylin Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_address.h,v $
 *------------------------------------------------------------------
 * x25_address.h -- X25 Level 3 address mapping support
 *
 * February 1988, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_address.h,v $
 * Revision 3.2.62.1  1996/06/01  07:19:48  tylin
 * CSCdi53715:  CDP is not working over Fr-Rel, X25 and SMDS interfaces
 *    Add new CUD (0xDA) for CDP
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  18:07:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:22:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __X25_ADDRESS_H__
#define __X25_ADDRESS_H__

/*
 *  X.25 address mapping utilities
 *
 *  The X.25 address mapping utilities fall into two broad functional
 *  categories, those that support the encapsulation of datagram
 *  protocols and those that support connection-oriented functions.
 *
 *  Datagram Encapsulation Services
 *    Generally the X.25 encapsulation service deals with datagrams of a
 *    particular type for transmission and reception across an X.25 cloud.
 *    A protocol source/destination is mapped to the X.121 address needed
 *    for X.25 Call establishment; the X.25 encapsulation service manages
 *    the establishment and tear-down of VCs as required by the datagram
 *    traffic and the options configured.
 *
 *    Two methods are typically available for establishing datagram
 *    connections, Cisco's historical (and proprietary) method for
 *    establishing a connection to carry a given type of traffic, and a
 *    standardized method the most recent of which is defined in the IETF's
 *    RFC 1356.  RFC 1356 includes the capability of creating a "null"
 *    encapsulation VC; such a VC has a protocol identification preamble
 *    at the start of every Complete Packet Sequence carried, so it can
 *    transport more than one type of traffic.
 *
 *	Datagram Service	non-X.121 Address	notes
 *	-----------------------	-----------------	-----
 *	Apollo			ADDR_APOLLO
 *	Appletalk		ADDR_APPLE
 *	Bridging		-none-			123
 *	CDP			-none-			1
 *	CLNS			-none-			12
 *	Compressed TCP		ADDR_IP			 2 45
 *	DecNET			ADDR_DECNET		 2
 *	IP			ADDR_IP
 *	Novell			ADDR_NOVELL
 *	Vines			ADDR_VINES
 *	XNS			ADDR_XNS
 *
 *	notes:
 *	1. These services do not map a datagram address; all bridging
 *	   traffic is sent to all bridging hosts reachable via X.25, and
 *	   CLNS engages in routing exchanges across X.25 to determine what
 *	   datagrams go to what destinations.
 *	2. These services accept a multitiude of datagram identifications
 *	   for transport.  Bridging support LINK_BRIDGE, LINK_IEEE_SPANING
 *	   and LINK_DEC_SPANING.  CLNS supports LINK_CLNS, LINK_CLNS_ALL_ES,
 *	   LINK_CLNS_ALL_IS, LINK_ISIS_ALL_L1_IS, LINK_ISIS_ALL_L2_IS and
 *	   LINK_CLNS_BCAST.  Compressed TCP supports LINK_COMPRESSED_TCP
 *	   and LINK_UNCOMPRESSED_TCP.  DecNET supports LINK_DECNET,
 *	   LINK_DECNET_ROUTER, LINK_DECNET_NODE and LINK_DECNET_PRIME_ROUTER.
 *	3. Cisco's bridging support is proprietary, so RFC 1356 bridging is
 *	   not supported.
 *	4. The compressed TCP service using RFC 1356 operation requires
 *	   a multi-protocol VC because the two datagram types (compressed
 *	   and uncompressed) must be distinguished (Cisco's proprietary
 *	   method uses the Q-bit to make this distinction).
 *	5. The compressed TCP service may only use a single VC to any
 *	   given destination (multiple VCs would allow mis-ordered
 *	   delivery of datagrams which defeats the purpose of the service).
 *
 *    When a datagram encapsulation map is defined, an X.121 address must
 *    be supplied (the "primary" address, i.e. the address that appears
 *    in an X.25 packet) along with the mapped datagram addresses needed
 *    to support the requested map services (between zero and seven
 *    "secondary" addresses--note that if two services use the same
 *    address type, e.g. IP and compressed TCP, that only one address
 *    of that type can be mapped).
 *
 *  Connection-Oriented Services
 *
 *    Three address mapping services are provided for connection-
 *    oriented protocols, PAD,QLLC, and CMNS mapping.  X.25 does not manage 
 *    the creation or deletion of VCs for these services, as this is left
 *    up to those protocol implementations, nor are the VCs that use the
 *    map tracked (as is necessary when sending a datagram over one of a 
 *    set of possible encapsulation VCs).
 *
 *    PAD maps are defined to allow configuration of facilities to encode
 *    on particular outgoing PAD Calls.  A single X.121 address is
 *    defined.
 *
 *    CMNS maps are defined to allow NSAP and NSAP-preamble addresses
 *    to be associated with an 802.x MAC address (which defines a route
 *    for received CMNS Calls out 802.x media) or an (optional) X.121
 *    address (which defines a route for received CMNS Calls out an
 *    X.25 interface to an optional X.121 destination).  The NSAP
 *    address is the primary address and the MAC or any X.121 address
 *    are the sole secondary address.  Note that this is more accurately
 *    a routing usage than a mapping usage.
 *
 *    QLLC maps are define to allow a mapping from an X.121 address to a 
 *    virtual MAC address.  With 10.4 it's possible to use the X.121 
 *    address in native format.  Accordingly there are not ONE but TWO
 *    map service entries, one for mapped connections, and one for native
 *    connections.  There is only one QLLC connection per virtual circuit.   
 */

/*
 *  With IETF/RFC-1356 operation, a map can support up to nine protocols
 *  using seven addresses (IP and THC share and address, and CLNS doesn't
 *  map an address)
 */
#define X25_MAX_ADDRESSES_MAPPED	7

/*
 *  map services
 */
#define X25_MAPS_APOLLO		0x0001	/* datagram encapsulation services */
#define X25_MAPS_APPLE		0x0002
#define X25_MAPS_BRIDGE		0x0004
#define X25_MAPS_CLNS		0x0008
#define X25_MAPS_COMP_TCP	0x0010
#define X25_MAPS_DECNET		0x0020
#define X25_MAPS_IP		0x0040
#define X25_MAPS_NOVELL		0x0080
#define X25_MAPS_VINES		0x0100
#define X25_MAPS_XNS		0x0200
#define X25_MAPS_PAD		0x0400	/* connection-oriented services */
#define X25_MAPS_CMNS		0x0800
#define X25_MAPS_QLLC		0x1000	/* qllc maps to Virtual Mac address */
#define X25_MAPS_QLLC_NATIVE	0x2000	/* qllc uses unmappped x121 address */
#define X25_MAPS_CDP		0x4000	/* cisco discovery protocol */
#define X25_MAPS_ALL_BITS	0xEFFF	/* all map service bits */

#define X25_MAPS_ALL_QLLC	(X25_MAPS_QLLC | X25_MAPS_QLLC_NATIVE)

#define X25_MAPS_ENCAPS		(X25_MAPS_APOLLO | X25_MAPS_APPLE |	 \
				 X25_MAPS_BRIDGE | X25_MAPS_CLNS |	 \
				 X25_MAPS_COMP_TCP | X25_MAPS_DECNET |	 \
				 X25_MAPS_IP | X25_MAPS_NOVELL |	 \
				 X25_MAPS_VINES | X25_MAPS_XNS | X25_MAPS_CDP)
#define X25_MAPS_CON_ORIENTED	(X25_MAPS_ALL_QLLC | X25_MAPS_PAD |	 \
				 X25_MAPS_CMNS)

/*
 *  define macros for the exceptional encapsulation cases noted (above)
 */
#define X25_MAPE_NO_SVC_ADDRESS		(X25_MAPS_BRIDGE |		\
					 X25_MAPS_CLNS | X25_MAPS_PAD | \
					 X25_MAPS_QLLC_NATIVE | X25_MAPS_CDP)
#define X25_MAPE_MULTI_LINKTYPES	(X25_MAPS_CLNS |		\
					 X25_MAPS_COMP_TCP | X25_MAPS_DECNET)
#define X25_MAPE_CISCO_ONLY		(X25_MAPS_BRIDGE)
#define X25_MAPE_FORCE_MULTI_ENC	(X25_MAPS_COMP_TCP)
#define X25_MAPE_SINGLE_VC		(X25_MAPS_COMP_TCP | X25_MAPS_QLLC)
#define X25_MAPE_NO_MAP_TO_PVC		(X25_MAPS_QLLC)

/*
 *  map status values
 */
#define	X25_MAP_PERM	 0		/* permanent address map */
#define	X25_MAP_PVC	 1		/* PVC address map */
#define	X25_MAP_CONSTR	 2		/* dynamic address map (e.g. DDN) */
#define	X25_MAP_TEMP	 3		/* temporary address map (e.g. PAD) */
#define X25_MAP_CONFIGURED_M 0x02	/* configured vs. transient mask */
#define X25_MAP_CONFIGURED_V 0x00	/* configured vs. transient value */

/*
 *  configured flags and operational flags
 *
 *  An IP address can support IP traffic, TCP header compression traffic, or
 *  both, so creation of an X.121/IP address map is (unfortunately) ambiguous.
 *
 *  CMNS commonly needs to determine what type of address a CMNS NSAP maps
 *  to, so three flags are defined to simplify these tests.
 */
#define X25_MAPF_DEFAULT	0x0000	/* default behavior (no config. flags)*/
#define X25_MAPF_REV_OK		0x0001	/* accept incoming reverse charged */
#define X25_MAPF_BCAST		0x0002	/* send broadcasts */
#define X25_MAPF_PASSIVE	0x0004	/* passive TCP header compression */
#define X25_MAPF_BAR_IN		0x0010	/* incoming CALLs barred */
#define X25_MAPF_BAR_OUT	0x0020	/* outgoing CALLs barred */
#define	X25_MAPF_PBP		0x0040	/* packet-by-packet compression */
#define	X25_MAPF_NOPBP		0x0080	/* disable comression */

#define	X25_MAP_DEFAULT_IDLE	0xffff	/* no idle timer specified for map */

/*
 *  match options
 */
#define X25_MATCH_EXACT		0x00	/* exact address match */
#define X25_MATCH_NSAP_BESTFIT	0x01	/* find an NSAP best fit */
#define X25_MATCH_CONFIGURED	0x02	/* find a configured (perm/pvc) map */
#define X25_MATCH_INCOMING	0x04	/* match for an incoming Call */
#define X25_MATCH_OUTGOING	0x08	/* match for an outgoing Call */

/*
 *  define an address map entry
 */
STRUCTURE_REF(x25_addr_entry);
struct x25_addr_entry_ {
    x25_addr_entry *next;	/* next link of address hash chain */
    x25map *map;		/* containing map */
    addrtype addr;		/* protocol address */
};

/*
 *  define the facilities that may be configured for originated CALLs
 */
STRUCTURE_REF(x25_fac_conf);
struct x25_fac_conf_ {
    uchar win;			/* incoming window size */
    uchar wout;			/* outgoing window size */
    uchar pin;			/* incoming packet size */
    uchar pout;			/* outgoing packet size */
    uchar cug;			/* closed user group (basic) */
    uchar rev;			/* reverse charging */
    uchar tput_in;		/* incoming throughput */
    uchar tput_out;		/* outgoing throughput */
    ushort tdelay;		/* transit delay */

    /* cisco or user format NUID */
    uchar cisco_nuid;		/* cisco format? */
    uchar nuid_len;		/* NUID length */
    char *nuid;			/* Network User Identification data */

    rpoatype *rpoa;		/* Recognized Private Operating Authority */
};

/*
 *  define the information used to carry protocols over X.25; this
 *  information is generated using "x25 map ..." configuration commands
 */
struct x25map_ {
    ushort map_services;		/* mapped service bits */
    uchar map_service_cnt;		/* # of services mapped */
    uchar map_status;			/* permanent, constructed, etc. */
    ushort map_linktype;		/* mapped type (LINK_...) */

    ushort map_flags;			/* configured behavior flags */
    uchar map_maxnvc;			/* maximum number of VCs to allow */
    uchar map_cud_pid_method;		/* encaps. method for originated CALL */
    ushort map_idle;			/* map-specific idle timer */

    idbtype *map_idb;			/* idb associated with map */
    uchar map_nvc;			/* number of existing VCs */
    uchar map_index;			/* index into lci pointer array */
    ushort map_;			/* align */
    lcitype *map_lci[X25_MAXNVC];	/* array of pointers to active VCs */

    x25_fac_conf map_fac;		/* configured facilities */

    llctype *map_llc2;			/* pointer to llc2 context */
    bfetype *map_bfe;			/* pointer to BFE context */
    thcdbtype *map_thc;			/* pointer to TCP compression context */

    /*
     *  the end of this structure has one or more addresses, the first
     *  of which is the primary address (e.g. the address that appears
     *  in X.25 packets, either X.121 or NSAP) which is mapped to N
     *  secondary addresses where 0 <= N <= X25_MAX_ADDRESSES_MAPPED
     *
     *  All of these addresses, primary and secondary, are hashed into
     *  the mapped address hash table (x25_addr_hash_table).
     */
    uint map_addr_count;		/* number of mapped addresses */
    x25_addr_entry map_address[1];	/* set of addresses mapped */
};

/*
 *  define a *KLUDGE* to represent a MAC address for CONS maps (NSAP <-> MAC)
 *  do *not* let this representation escape the X.25 code!
 */
#define ADDR_MAC_KLUDGE	ADDR_MAX
#define ADDRLEN_MAC_KLUDGE IEEEBYTES
#define mac_kludge_addr	addr.x121_address

/*
 *  define the useful indices for the map_address array
 */
#define X25_MAP_PRIMARY_ADDR_INX	0
#define X25_MAP_SECONDARY_ADDR_INX	1

/*
 *  IMPORTANT NOTE: xnethash() is an external routine that will be
 *  exposed to our ADDR_MAC_KLUDGE value.  It does not validate
 *  the passed address, but if this changes we're in trouble.
 */
#define	X25_ADDR_HASH_SIZE	128
#define X25_ADDR_OVERHEAD	(sizeof(addrtype) - sizeof(union addresses))
#define	X25_ADDR_HASH(address)					\
	(xnethash((uchar *)address,				\
		  (address)->length + X25_ADDR_OVERHEAD) &	\
	 (X25_ADDR_HASH_SIZE - 1))

/*
 *  define an array to use when converting from an ADDR_<type> to a LINK_<type>
 */
extern ushort addr_linktype[ADDR_MAX];

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern uchar *x25_printfacility(uchar *);

/*
 * x25_address.c
 */
extern uint x25_map_service2link(uint service);
extern uint x25_map_link2service(uint linktype);
extern void x25_map_hash_init(void);
extern void x25_map_hash_restart(hwidbtype *idb, llctype *llc2);
extern x25map *x25_map_create(idbtype *idb, ushort map_services,
		uint addr_cnt,
		addrtype *addr, ... /* any secondary addresses */);
extern boolean x25_map_match_service(x25map *map, ushort service);
extern x25map *x25_map_find(hwidbtype *hwidb, addrtype *addr, uint match,
			    uint services);
extern void x25_map_delete(x25map *map, boolean clear);
extern void x25_map_delete_subint(idbtype *idb);
extern boolean x25_map_addr_match(x25map *map, addrtype *addr);
extern boolean x25_map_add_lci(lcitype *lci, x25map *map);
extern void x25_map_remove_lci(lcitype *lci);
extern void x25_map_update_nvc(x25map *map, int new);
extern void x25_map_update_if_nvc(hwidbtype *idb, int new);
extern void x25_map_flush_hold(x25map *map);
extern void x25_map_flush_rpoa(rpoatype *rpoa);
extern x25map *x25_map_type_check(hwidbtype *hwidb, idbtype *idb,
				  uint link_type);
extern x25map *x25_broadcast(idbtype *idb, paktype *pak);
extern char *x25_nuid(x25map *map, boolean show);
extern char *x25_cud_pid_method_str(uint method);
extern void x25_map_entry_nvgen(x25map *map, void *context);
extern void x25_map_nvgen(parseinfo *csb);
extern void show_x25_maps(void);
extern uchar *x25_prx25_addr(uchar *c);

#endif					/* __X25_ADDRESS_H__ */
