/* $Id: x25_proto.h,v 3.2.60.1 1996/05/23 07:38:51 myeung Exp $
 * $Source: /release/112/cvs/Xsys/x25/x25_proto.h,v $
 *------------------------------------------------------------------
 * x25_proto.h -- X25 Level 3 support for higher level protocols (IP, etc)
 *
 * February 1988, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: x25_proto.h,v $
 * Revision 3.2.60.1  1996/05/23  07:38:51  myeung
 * CSCdi58029:  OSPF crash in ospf_interface_up() when unconfiguring OSPF
 * Branch: California_branch
 * - Add boolean parameter to ip_address_change registry to indicate if
 *   the change is for secondary or primary address.
 *
 * Revision 3.2  1995/11/17  18:07:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:53:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/04  22:02:34  gglick
 * CSCdi37951:  X.25/BFE X.121 address generation incorrect
 *
 * Revision 2.1  1995/06/07  23:22:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *  define the maximum number of bytes required for protocol identification
 *  in Call User Data
 */
#define X25_MAX_CUD_PID	7	/* compression + SNAP PID */

#define	X25_MTU		1500 
#define	X25_DDN_MTU	576
#define	X25_BFE_MTU	896

#define	X25_DDN_DNIC	"0000"
#define	X25_DDN_PADDR	'0'
#define	X25_DDN_LADDR	'1'
#define	X25_DDN_MAXPACKET	1024

/*
 *  CSCdi04506 required that DDN precedence 0 be forwarded through the x25
 *  switch (default facilities are not normally forwarded).  Hence ->lci_prec
 *  is initially set to X25_UNASSIGNED_PREC (WJT 11/30/92).
 */
#define	X25_DEFAULT_PREC	0
#define	X25_UNASSIGNED_PREC  (uchar) -1 

#define octettype struct _octets

octettype {
    uchar n, h, l, i;
};

/*
 * DDN Facilities and arguments
 */

#define	X25_FACILITY_DDN_STANDARD	0x04
#define	X25_FACILITY_DDN_STANDARD_ON	0x01
#define	X25_FACILITY_DDN_CALL_PREC	0x08
#define	X25_FACILITY_DDN_CALL_PREC_MASK	0x03
#define	X25_FACILITY_DDN_BFE_ATIF	0xC1  /* BFE ATIF Data */

/*
 * X25 DOD extended diagnostics
 */

#define	X25_DIAG_IMP_UNAVAIL		128 /* PSN dropped ready line */
#define	X25_DIAG_LINK_BREAK		129 /* HDLC (L2) sent break */
#define	X25_DIAG_LINK_UP		130 /* HDLC (L2) came up */
#define	X25_DIAG_LINK_DOWN		131 /* HDLC (L2) went down */
#define	X25_DIAG_DTE_RESTART		132 /* DTE retarted */
#define	X25_DIAG_RESOURCE_UNAVAIL	133 /* local resource shortage during call setup */
#define	X25_DIAG_REMOTE_RESOURCE_UNAVAIL 134 /* remote resource shortage during call setup */
#define	X25_DIAG_REMOTE_CALL_COLLISION	135 /* remote call collision */
#define	X25_DIAG_REMOTE_HOST_DEAD	136 /* remote host dead */
#define	X25_DIAG_REMOTE_IMP_DEAD	137 /* remote PSN dead */
#define	X25_DIAG_SUBNET_ACCESS_BARRED	138 /* subnet access barred by administrative controls */
#define	X25_DIAG_REMOTE_CONNECTION_LOST	139 /* remote connection lost */
#define	X25_DIAG_REMOTE_RESPONSE_LOST	140 /* remote response lost */
#define	X25_DIAG_CALLING_ADDR_UNAUTH	141 /* invalid source address - 1822L name not effective or not auth.. */
#define	X25_DIAG_CALLING_ADDR_WRONG	142 /* invalid source address - 1822L name/port mismatch */
#define	X25_DIAG_CALLED_ADDR_UNAUTH	143 /* invalid destination address - 1822L name not authorized */
#define	X25_DIAG_CALLED_ADDR_DISABLED	144 /* invalid destination address - 1822L name not effective */
#define	X25_DIAG_CALLED_ADDR_NO_DTE	145 /* invalid destination address - 1822L name not found */
#define	X25_DIAG_LOGICAL_ADDR_INVALID	146 /* invalid address - logical addressing not in use */
#define	X25_DIAG_LOGICAL_ADDR_ENABLED	147 /* declaration - 1822L name now effective */
#define	X25_DIAG_LOGICAL_ADDR_REENABLED	148 /* declaration - 1822L name was and still is effective */
#define	X25_DIAG_LOGICAL_ADDR_DISABLED	149 /* declaration - 1822L name now disabled */
#define	X25_DIAG_LOGICAL_ADDR_REDISABLED 150 /* declaration - 1822L name was and still is disabled */
#define	X25_DIAG_INCOMING_CALLS_BARRED	151 /* incoming calls barred */
#define	X25_DIAG_OUTGOING_CALLS_BARRED	152 /* outgoing call barred */
#define	X25_DIAG_CAUSE_NONZERO		153 /* cause field non-zero */
#define	X25_DIAG_VC_IDLE_TIMEOUT	154 /* VC idle timeout (DDN Standard Sevice) */
#define	X25_DIAG_SERVICE_UNAVAIL	155 /* destination DTE service mismatch (Standard Only) */
#define	X25_DIAG_INVALID_PROTO_ID	156 /* invalid protocol ID (DDN Standard Service) */
#define	X25_DIAG_ERROR_WHEN_OPEN	157 /* Error while opening AHIP source of IOP connection */
#define	X25_DIAG_PVC_END_INCOMP		160 /* PVC endpoints incompatible */
#define	X25_DIAG_ACS_RES_COMP		161 /* ACS reselection completed */
#define	X25_DIAG_PVC_NO_RESP		162 /* PVC - no response by DTE on initialization */
#define	X25_DIAG_PVC_UP_AND_COMP	163 /* PVC - link up and restart complete */
#define	X25_DIAG_PVC_NETWORK_ERROR	164 /* PVC - error caused by network */
#define	X25_DIAG_CPS_DEADLOCK		165 /* CPS aggregation deadlock detected */
#define	X25_DIAG_INVALID_PACKET		166 /* invalid packet in state ST_REMOTEuw_either */
#define	X25_DIAG_CONN_CLOSED_NET_ERR	167 /* Connection closed due to network error */
#define	X25_DIAG_NAS_NO_FAST_SELECT	168 /* NAS cannot intercept fast select */
#define	X25_DIAG_NAS_NO_RPOA		169 /* NAS cannot intercept RPOA */
#define	X25_DIAG_NAS_NO_X75		170 /* NAS connot intercept X75 call */
#define	X25_DIAG_NAS_TOO_MUCH_DATA	171 /* too much data in intercepted call (NAS) */
#define	X25_DIAG_NAS_BAD_ADDR		172 /* bad NAS address */
#define	X25_DIAG_NAS_BAD_FACILITY	173 /* invalid facility for normal X.25 clear (NAS) */
#define	X25_DIAG_NAS_BAD_LOCAL_RESADDR	174 /* invalid reselect address, local (NAS) */
#define	X25_DIAG_NAS_BAD_REMOTE_RESADDR	175 /* invalid reselect address, remote (NAS) */
#define	X25_DIAG_NAS_BAD_FAST_SELECT	176 /* reselect request with fast select (NAS) */
#define	X25_DIAG_NAS_BAD_TOO_DATA	177 /* reselect request with too much data (NAS) */
#define	X25_DIAG_NAS_NUI_BAD		178 /* reselect request NUI not 10 digits (NAS) */
#define	X25_DIAG_NAS_FIXED_FACIL	179 /* attempt to renegotiate fixed facilities (NAS) */
#define	X25_DIAG_NAS_INVAL_PACKET	180 /* local packet invalid for ACS state (NAS) */
#define	X25_DIAG_NAS_REMOTE_RES		181 /* remote reselect, invalid state (NAS) */
#define	X25_DIAG_NAS_RES_CALL_TO	182 /* reselect call timeout (NAS) */
#define	X25_DIAG_NAS_BAD_RES_REQ	183 /* malformed reselect request (NAS) */
#define	X25_DIAG_LOCAL_HIGHER_PREC	192 /* call cleared due to local prememption */
#define	X25_DIAG_REMOTE_HIGHER_PREC	193 /* call cleared due to remote prememption */
#define	X25_DIAG_PREC_TOO_HIGH		194 /* Precedence not allowed (DDN Standard Service) */
#define	X25_DIAG_BLACK_ENT_EMERG	224 /* Blacker - entering emergency mode */
#define	X25_DIAG_BLACK_LEAVE_EMERG	225 /* Blacker - leaving emergency mode */
#define	X25_DIAG_BLACK_EMERG_WINDOW_OP	226 /* Blacker - emergency window open */
#define	X25_DIAG_BLACK_FAIL_ADDR_TRANS	227 /* Blacker - call failed:address translation info required */
#define	X25_DIAG_BLACK_FAIL_NO_WIND	228 /* Blacker - call failed:emergency window not open */
#define	X25_DIAG_ILLEGAL_PROTO		234 /* Invalid Protocol ID (DDN Standard Service) */

/*
 *  bcdlen_inline
 *  Determine the number of bytes used by a pair of X.25 addresses
 */

static inline int bcdlen_inline (uchar c)
{
    uchar len;

    len = ((c >> 4) & 0xf) + (c & 0xf);
    return((len + 1) >> 1);
}


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * x25_proto.c
 */
extern uint x25_encaps_maptype(uint pak_linktype);
extern void x25_encaps_form_cud_pid(lcitype *lci, uchar *cud, uchar *len);
extern boolean x25_encaps_form_data_pid(paktype *pak, uchar lci_pid_method);
extern uint x25_encaps_parse_cud_pid(lcitype *lci, uchar *cud, uint cud_len,
				     uint *pid_len, boolean *compress);
extern boolean x25_encaps_parse_data_pid(paktype *pak, lcitype *lci);
extern void x25_encaps_print_cud(lcitype *lci, uchar *cud, uint cud_len,
				 uint pdu);
extern void x25_encaps_print_data_pid(uchar *pid, uint len);
extern x25map *x25_ip_icall_setup(lcitype *lci);
extern x25map *x25_ip_ocall_setup(idbtype *, addrtype *);
extern int x25_ip_call_size(lcitype *);
extern int x25_ip_facil(lcitype *, uchar *);
extern lcitype *x25_encaps_find_lci(idbtype *idb, paktype *pak, x25map *map,
				uint service);
extern uchar x25_ip_prec(uchar tos);
extern void x25_ip_default(idbtype *, ipaddrtype, ipaddrtype, boolean, boolean);
