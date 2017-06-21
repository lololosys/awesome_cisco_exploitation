/* $Id: fr_arp.h,v 3.1.32.1 1996/04/25 23:23:13 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_arp.h,v $
 *------------------------------------------------------------------
 * fr_arp.h -- Definitions for Frame Relay ARP Protocol
 *
 * Feburary 1996, Charles Poe
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_arp.h,v $
 * Revision 3.1.32.1  1996/04/25  23:23:13  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.38.1  1996/04/24  04:11:15  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1.2.2  1996/03/19  17:54:17  ccpoe
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.1  1996/02/19  08:16:32  ccpoe
 * Frame Relay ARP subsystem
 * Branch: FR_SVC_branch
 *
 * Revision 3.1  1996/02/10  00:42:27  ccpoe
 * Frame Relay ARP subsystem
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "address.h"

/*
 * Packet format and definitions for Frame Relay ARP
 */

enum FR_ARPTYPE {
    FR_CISCO_ARP,
    FR_INARP,
    FR_RFC_ARP,
    MAX_FR_ARPTYPE_COUNT
};

#define FR_PR_NONE 0
#define FR_PR_IP PR_IP
#define FR_PR_IP_INHERIT_COMPR        FR_PR_IP+1
#define FR_PR_IP_NP_COMPR             FR_PR_IP_INHERIT_COMPR + 1
#define FR_PR_IP_P_COMPR              FR_PR_IP_NP_COMPR + 1
#define FR_PR_TCP_COMP                TYPE_COMPRESSED_TCP
#define FR_PR_NP_TCP_COMPR            FR_PR_TCP_COMP + 1
#define FR_PR_P_TCP_COMPR             FR_PR_NP_TCP_COMPR + 1
#define FR_PR_DECNET 0x809
#define FR_PR_APOLLO PR_APOLLO		/* protocol is 10MB Apollo */
#define FR_PR_XNS    0x80a
#define FR_PR_NOVELL 0x80b
#define FR_PR_APPLETALK 0x80c
#define FR_PR_VINES 0x80e
#define FR_PR_CLNS 0x80f
#define FR_ARPBYTES 12+2*sizeof(union addresses)
#define FR_OP_REQ 0
#define FR_OP_REP 1
#define FR_SERIAL 3
#define FR_PL 2

/*
 * Defines for inverse ARP
*/
# define FR_IP_IN_ARP_ENABLED    0x01
# define FR_IPX_IN_ARP_ENABLED   0x02
# define FR_AT_IN_ARP_ENABLED    0x04
# define FR_XNS_IN_ARP_ENABLED   0x08
# define FR_VINES_IN_ARP_ENABLED 0x10
# define FR_DNET_IN_ARP_ENABLED  0x20
# define FR_ALL_IN_ARP_ENABLED (FR_IP_IN_ARP_ENABLED | FR_IPX_IN_ARP_ENABLED |\
				FR_AT_IN_ARP_ENABLED | FR_XNS_IN_ARP_ENABLED |\
				FR_DNET_IN_ARP_ENABLED |FR_VINES_IN_ARP_ENABLED)

#define FR_INARP_DLCI_GET 0
#define FR_INARP_DLCI_SET 1
#define FR_INARP_DLCI_GETORSET 2
#define FR_ARP_INT_GET 0
#define FR_ARP_INT_SET 1
#define FR_ARP_INT_GETORSET 2
#define FR_ARP_INTV 1

struct fr_idb_arp {
    boolean extended_inarp;
    boolean skip_inarp;				   /* lmi send inarp req */
    ulong fr_inarp_interval;
    mgd_timer *fr_inarp_timer;
    ushort inarp_status[MAXDLCI_VALUES];           /* inverse arp per PVC */
};

/*
 * The definition of a "Frame Relay Arp".  Note that this is *not* the
 * same as the definition of a RFC structure ARP on frame relay as the
 * opcode field is in the wrong place.
 */
typedef struct fr_arptype_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    ushort opcode;			/* 16 bits of opcode */
    uchar arhln;			/* 8 bits of length of hdw address */
    uchar arpln;			/* 8 bits of length of protocol add */
    ushort shdw;			/* 16 bits of source hdw address */
    union addresses PACKED(spro);	/* 32 bits of source protocol */
    ushort PACKED(thdw);		/* 16 bits of target hdw address */
    union addresses PACKED(tpro);	/* 32 bits of target protocol */
} fr_arptype;


/*
 * Set up Frame Relay ARP/INARP data
 */
#define DEFAULT_INARP_INTERVAL 60
#define AT_INARPBYTES  20
#define XNS_INARPBYTES 36
#define VINES_INARPBYTES 24
#define DNET_INARPBYTES  16

/*
 * IP ARP over frame relay
 */

# define FR_ARHRD              0xF
# define FR_ARHLN              2
# define FR_INARP_REQ          8
# define FR_INARP_REPLY        9
#define RFBYTES               20

typedef struct fr_rfc_arp_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar  arhln;			/* 8 bits of length of hdw address */
    uchar  arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    ushort ipshdw;			/* 10 bits of source hdw address */
    ipaddrtype PACKED(ipspro);		/* 32 bits of source protocol */
    ushort PACKED(ipthdw);		/* 10 bits of target hdw address */
    ipaddrtype PACKED(iptpro);		/* 32 bits of target protocol */
} fr_rfc_arp;

#define fr_rfc_arp_headstart(ptr) ((ptr)->network_start)

typedef struct fr_xns_inarptype_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar  arhln;			/* 8 bits of length of hdw address */
    uchar  arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    ushort xnsshdw;			/* 10 bits of source hdw address */
    struct xns_addrtype PACKED(xnsspro);
    ushort PACKED(xnsthdw);		/* 10 bits of target hdw address */
    struct xns_addrtype PACKED(xnstpro);
} fr_xns_inarptype;

typedef struct fr_novell_inarptype_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar arhln;			/* 8 bits of length of hdw address */
    uchar arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    ushort novshdw;			/* 10 bits of source hdw address */
    struct nov_addrtype PACKED(novspro);
    ushort PACKED(novthdw);		/* 10 bits of target hdw address */
    struct nov_addrtype PACKED(novtpro);
} fr_nov_inarptype;

typedef struct fr_at_inarptype_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar  arhln;			/* 8 bits of length of hdw address */
    uchar  arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    ushort atshdw;			/* 10 bits of source hdw address */
    struct apple_addrtype atspro;
    ushort atthdw;			/* 10 bits of target hdw address */
    struct apple_addrtype attpro;
} fr_at_inarptype;

typedef struct fr_decnet_inarptype_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar arhln;			/* 8 bits of length of hdw address */
    uchar arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    ushort decnetshdw;		/* 10 bits of source hdw address */
    ushort dnspro;
    ushort decnetthdw;		/* 10 bits of target hdw address */
    ushort dntpro;
} fr_decnet_inarptype;

typedef struct fr_vines_inarptype_ {
    ushort arhrd;			/* 16 bits of hardware type */
    ushort arpro;			/* 16 bits of procotol type */
    uchar  arhln;			/* 8 bits of length of hdw address */
    uchar  arpln;			/* 8 bits of length of protocol add */
    ushort opcode;			/* 16 bits of opcode */
    ushort vnshdw;			/* 10 bits of source hdw address */
    struct vines_addrtype PACKED(vnspro);
    ushort PACKED(vnthdw);		/* 10 bits of target hdw address */
    struct vines_addrtype PACKED(vntpro);
} fr_vines_inarptype;

typedef union fr_arp_sizer_ {
    union {
	fr_rfc_arp ip;
	fr_xns_inarptype xns;
	fr_nov_inarptype novell;
	fr_at_inarptype at;
	fr_decnet_inarptype dec;
	fr_vines_inarptype vines;
    } u;
} fr_arp_sizer;

#define FR_ARP_MAX_SIZE sizeof(fr_arp_sizer)

/*
 * fr_link_inarp_timer
 *	Link fr_idb with inverse ARP timer
 */
static inline void fr_link_inarp_timer(hwidbtype *hwidb,
				       mgd_timer *inarp_timer)
{
   hwidb->frame_relay_stuff->fr_arp->fr_inarp_timer = inarp_timer;
}

/*
 * fr_get_inarp_timer
 *      Get inverse ARP timer belonging to a hwidb
 */
static inline mgd_timer *fr_get_inarp_timer(hwidbtype *hwidb)
{
    return (hwidb->frame_relay_stuff->fr_arp->fr_inarp_timer);
}
    

extern void fr_arp_enqueue(paktype *);
extern void fr_arp_init(subsystype *);
extern void fr_dlci_inarp_init(idbtype *, ushort, uint, boolean);
extern void fr_send_inarp_req(idbtype *, hwidbtype *, uint, ushort);
extern void print_fr_arp(fr_arptype *);
extern void clear_fr_inarp (parseinfo *);
extern void fr_arp_commands(parseinfo *);
extern void fr_arp_parser_init (void);
extern forktype fr_arp_background (void);

