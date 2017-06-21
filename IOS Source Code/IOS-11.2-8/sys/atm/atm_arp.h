/* $Id: atm_arp.h,v 3.6.20.8 1996/08/16 02:13:10 lmercili Exp $
 * $Source: /release/112/cvs/Xsys/atm/atm_arp.h,v $
 *------------------------------------------------------------------
 * A T M _ A R P . H
 *
 * January 1995, Ricky Li Fo Sjoe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atm_arp.h,v $
 * Revision 3.6.20.8  1996/08/16  02:13:10  lmercili
 * CSCdi65496:  rfc1577 failed ip routing
 * Branch: California_branch
 *
 * Revision 3.6.20.7  1996/06/19  23:19:13  rzagst
 * CSCdi60596:  ATM ARP fix to CSCdi56362 arp init called w/o atm interface
 * Branch: California_branch
 *
 * Revision 3.6.20.6  1996/06/18  16:55:41  achopra
 * CSCdi60126:  ARP server builds duplicate table entries for same ATM
 * address
 * Branch: California_branch
 *
 * Revision 3.6.20.5  1996/05/14  06:28:48  achopra
 * CSCdi47186:  Implement clear atm arp command.
 * Branch: California_branch
 *
 * Revision 3.6.20.4  1996/05/07  04:32:02  rzagst
 * CSCdi56362:  ATM ARP init can be called when no atm interface is present
 * backing out fix since it kills atm arp.
 * Branch: California_branch
 *
 * Revision 3.6.20.3  1996/05/07  00:37:43  rzagst
 * CSCdi56362:  ATM ARP init can be called when no atm interface is present
 * Branch: California_branch
 *
 * Revision 3.6.20.2  1996/05/01  14:44:56  achopra
 * CSCdi52007:  shut down an ATM ARP client on subinterface doesnt clear
 * map entry
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/04/25  23:01:49  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.6.24.2  1996/04/13  01:28:20  rzagst
 * ATM Modularity: Place atm arp in a separate subsystem.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6.24.1  1996/03/30  03:33:59  rzagst
 * ATM Modularity: Clean up and modularize by platform the ATM parser
 * commands.
 * Branch: ATM_Cal_branch
 *
 * Revision 3.6  1996/02/03  22:16:44  ronnie
 * CSCdi46955:  ATM ARP Server table out of sync with map list
 *
 * Revision 3.5  1996/01/24  22:56:08  ronnie
 * CSCdi45540:  ATM interface causes memory corruption on 7k
 *
 * Revision 3.4  1996/01/06  02:55:16  ronnie
 * CSCdi46517:  Cant turn off multiple arp server feature
 *
 * Revision 3.3  1995/11/22  20:44:19  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.2  1995/11/17  08:45:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:59:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  20:51:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/10/21  09:19:19  ronnie
 * CSCdi39635:  Connections to ARP server torn down too quickly
 *
 * Revision 2.3  1995/09/09  00:51:00  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/08/11  20:58:20  ronnie
 * CSCdi38022:  ATM ARP client needs to check for valid NSAP before
 * creating map
 *
 * Revision 2.1  1995/06/07  20:11:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Set up ATM ARP/INARP data
 *
 * IP ARP over ATM
 * 	Per RFC1577
 */

#ifndef __HAVE_ATM_ARP_H__
#define __HAVE_ATM_ARP_H__

/*
 *  There are different ways of providing redundant ARP Servers, and we
 *  need to be able to distinguish which one we are supposed to use.  These
 *  values are stored in swidb->rfc1577_flavor.
 */
enum {
    RFC1577_EXT_NONE,	/* Vanilla 1577 */
    RFC1577_EXT_BFI,	/* Our private solution (Brute Force and Ignorance) */
};

#define	MAX_ATM_ARP_HASH	256	/* Hash table size */
typedef struct atm_arp_tbl {
    struct atm_arp_tbl *next;	/* Ptr. to next in HASH */
    idbtype 	*idb;		/* Interface in question */
    uint 	time; 		/* timestamp of last update */
    addrtype 	net_addr; 	/* Network address */
    hwaddrtype 	atm_addr;	/* ATM NSAP address(if applicable) */
    hwaddrtype 	hw_addr; 	/* Hardware address (VCD) */
} atm_arp_tbl_t;

/*
 * ATM ARP configuration information
 */
typedef struct atm_arpserv_entry_ {
    struct atm_arpserv_entry_ *next, *prev;
    hwaddrtype nsap;
    boolean nsap_valid;	     /* ILMI leaves us with invalid NSAPs sometimes */
} atm_arpserv_entry;

typedef struct atm_arp_config_ {
    unsigned arp_flags;			/* ATM ARP related flags */
#define ATM_ARP_SERVER          0x1     /* This is an ATM ARP server */
    struct {
	struct {
	    int timeout;		/* cache entry timeout time */
	    struct wavl_handle_ *handle;/* handle on our cache */
	    mgd_timer timer;		/* our master timer */
	} server;
	struct {
	    atm_arpserv_entry *arp_serv_list; /* NSAPs of ARP servers to use */
	} client;
    } as_data;		/* arp server data */
} atm_swint_config;

/*
 *  NB: This functionality is used in atm_arp_client_off() directly, because
 *  we will be free()'ing the entry at the bottom of the loop, so the
 *  entry = entry->next will be a reference to freed memory if we use this
 *  construct.
 */
#define	FOR_ALL_ARP_SERVERS(atm_swint, entry) \
	for (entry = atm_swint->as_data.client.arp_serv_list ; \
		 entry != NULL ; entry = entry->next)


/*
 * return TRUE if the passed idb is running ATM ARP
 */
static inline boolean
is_atm_arp (idbtype *idb)
{
    return idb->atm_arp != NULL;
}

/*
 * return TRUE if the passed idb is an ARP server
 */
static inline boolean
is_atm_arp_server (const idbtype *idb)
{
    return (idb->atm_arp != NULL && (idb->atm_arp->arp_flags & ATM_ARP_SERVER));
}

/*
 * return TRUE if the passed idb is an ARP client
 */
static inline boolean
is_atm_arp_client (const idbtype *idb)
{
    /*
     *  Because an ARP server is also a client, we don't check the
     *  atm_arp->arp_flags flag
     */
    return idb->atm_arp != NULL;
}


/* 
 * General defaults
 */
#define	ATMARP_CLIENTAGE 	(15 * ONEMIN)	/* time 'til client ages out */
#define	ATMARP_SERVERAGE 	(20 * ONEMIN)	/* time 'til server ages out */

#define	ATM_ARP_MAX_SIZE	256	/* Maximum ARP packet size */

/* 
 * ARP/InARP specific info.
 */
# define ATM_ARHRD              0x0013	
# define ATM_ARPPRO		0x0800 	/* IP protocol */
# define ATM_ARSPLN		4	/* For IP */
# define ATM_ARTPLN		4	/* For IP */

/*
 * ARP Opcode values 
 */
# define ATM_ARP_REQ		1
# define ATM_ARP_REPLY		2
# define ATM_INARP_REQ          8
# define ATM_INARP_REPLY        9
# define ATM_ARP_NAK		10

/*
 * Type & Length bit definitions for ARSHTL, ARSSTL, ARTHTL & ARTSTL
 */
#define	ATM_NSAPA_ADDR		0x00
#define	ATM_E164_ADDR		0x40
#define ATM_ADDR_LEN_MASK	0x3f	/* mask off the type bits */
#define	ATM_ADDRLEN(len)	((len) & ATM_ADDR_LEN_MASK)
#define	ATM_ADDRTYPE(len)	((len) & ATM_E164_ADDR)

/*
 * 16 bit fields declared as arrays of uchars to guarantee that the compiler
 * doesn't do any funky padding for us.
 */
typedef struct atm_rfc_arp_ {
    uchar  arhrd[2];			/* 16 bits of hardware type */
    uchar  arpro[2];			/* 16 bits of procotol type */
    uchar  arshtl;			/* Type & Length of src. ATM # (q) */
    uchar  arsstl;			/* Type & Length of src. ATM sub #(r)*/
    uchar  opcode[2];			/* 16 bits of opcode */
    uchar  arspln;			/* Length of source protocol addr.(s)*/
    uchar  arthtl;			/* Type & length of targ. ATM # (x) */
    uchar  artstl;			/* Type & length of targ. ATM sub#(y)*/
    uchar  artpln;			/* Length of target protocol addr.(z)*/
    uchar  vl_start[0];			/* start of variable length fields */
    /*
     * The following fields are variable length and are not in
     * this structure, but I show the length of octets each may
     * have.
     *
     * Source ATM address (8, 20)
     * Source ATM subaddress (0, 20)
     * Source protocol address (4 for IP)
     * Target ATM address (8, 20)
     * Target ATM subaddress (0, 20)
     * Target protocol address (4 for IP)
     */
} atm_rfc_arp;

/*
 * some inlines for dealing with the variable length fields
 */
static inline uchar *arsha (atm_rfc_arp *head)	/* source ATM number */
{							/* q octets long */
    return head->vl_start;
}
static inline uchar *arssa (atm_rfc_arp *head)	/* source ATM subaddress */
{							/* r octets long */
    return arsha(head) + (head->arshtl & ATM_ADDR_LEN_MASK);
}
static inline uchar *arspa (atm_rfc_arp *head)	/* source protocol address */
{							/* s octets long */
    return arssa(head) + (head->arsstl & ATM_ADDR_LEN_MASK);
}
static inline uchar *artha (atm_rfc_arp *head)	/* target ATM number */
{							/* x octets long */
    return arspa(head) + head->arspln;
}
static inline uchar *artsa (atm_rfc_arp *head)	/* target ATM subaddress */
{							/* y octets long */
    return artha(head) + (head->arthtl & ATM_ADDR_LEN_MASK);
}
static inline uchar *artpa (atm_rfc_arp *head)	/* target protocol address */
{							/* z octets long */
    return artsa(head) + (head->artstl & ATM_ADDR_LEN_MASK);
}

#define	ATM_ARP_HDRBYTES	(sizeof(atm_rfc_arp))

/* avoid ugly casts in a bunch of code by doing one here */
static inline ipaddrtype
extr_src_ipaddr (atm_rfc_arp *arp)
{
    return (ipaddrtype)GETLONG(arspa(arp));
}
static inline ipaddrtype
extr_dst_ipaddr (atm_rfc_arp *arp)
{
    return (ipaddrtype)GETLONG(artpa(arp));
}

/*
 *  Given an ATM ARP header, report back how big it is (including all the
 *  variable length elements present).
 */
static inline int atm_arp_datasize (atm_rfc_arp *head)
{
    return (ATM_ARP_HDRBYTES + ((head->arshtl & ATM_ADDR_LEN_MASK) +
				(head->arsstl & ATM_ADDR_LEN_MASK) +
				(head->arthtl & ATM_ADDR_LEN_MASK) +
				(head->artstl & ATM_ADDR_LEN_MASK) +
				head->arspln +
				head->artpln));
}

#define atm_rfc_arp_headstart(ptr) ((ptr)->network_start)

typedef struct atm_arp_canon {
    idbtype	*idb;			/* Interface in question */
    addrtype	s_naddr;		/* Source network address */
    hwaddrtype	s_aaddr;		/* Source ATM address */
    hwaddrtype	s_saddr;		/* Source ATM subaddress */
    addrtype	d_naddr;		/* Destination Network address */
    hwaddrtype	d_aaddr;		/* Dest. ATM address */
    hwaddrtype	d_saddr;		/* Dest. ATM subaddress */
    hwaddrtype 	vcd;			/* Incoming VCD */
} atm_arp_canon_t;

/*
 * EXTERNS
 */
extern void atm_arp_clear(parseinfo *);
extern void atm_arp_parser_init(void);
extern void atm_arp_setup(void);
extern void atm_arp_client_init_common(idbtype *, hwaddrtype *);
extern boolean atm_arp_glom_by_serv(idbtype *, uchar, uchar *, ipaddrtype);
extern void atm_send_inarp_req(vc_info_t *, uint, paktype *);
extern void atm_inarp_enable_timer(vc_info_t *);
extern void atm_inarp_disable_timer(vc_info_t *);
extern staticmap_type *atm_arp_build_maps(idbtype *, atm_arpserv_entry *);
extern atm_arpserv_entry *locate_arpserv_entry(idbtype *, hwaddrtype *);

extern void atm_arp_serv_connect(idbtype *, vcd_t);
extern void atm_arp_serv_connect_ack(idbtype *, vcd_t);
extern void atm_arp_serv_disconnect(idbtype *, vcd_t);
extern void atm_arp_serv_glommed_onto_vc(idbtype *, vcd_t, uint, addrtype *);

extern queuetype atm_arp_packetQ;       /* InARP packets */

extern void atmarp_rxpak(paktype *, vc_info_t *);

#endif /* __HAVE_ATM_ARP_H__ */
