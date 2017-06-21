/* $Id: ip_addrpool.h,v 3.3.6.2 1996/09/10 06:16:18 fox Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_addrpool.h,v $
 *------------------------------------------------------------------
 * ip_addrpool.h - IP Peer Address Pooling header include file
 *
 * Sept 10, 1995, Craig Fox
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_addrpool.h,v $
 * Revision 3.3.6.2  1996/09/10  06:16:18  fox
 * CSCdi68456:  Parallel connection information lost if one link
 * disconnects
 * Branch: California_branch
 * Expand check for parallel connections to protect IP dynamic dialer
 * maps.
 *
 * Revision 3.3.6.1  1996/06/19  20:35:23  fox
 * CSCdi59847:  Dynamic dialer maps are not cleared on async dialer ints
 * Branch: California_branch
 * Call reg_invoke_ipcp_cleanup_ip_peer() to remove the neighbor route,
 * the dynamic dialer map, and any other mechanisms tied to the current
 * peer address before clearing the peer address.
 *
 * Revision 3.3  1996/03/05  07:07:29  fox
 * CSCdi50489:  unnecessary neighbor route installed, backup line never
 * drops
 * Add '[no] peer neighbor-route' command
 *
 * Revision 3.2  1995/11/18  17:41:38  fox
 * CSCdi43045:  ppp and slip exec-level commands are broken
 * Don't reserve peer address if interface is down.
 * If address pooling is disabled, allow peer address.
 *
 * Revision 3.1  1995/11/09  11:55:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/23  20:56:54  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.1  1995/09/18  19:35:35  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IP_ADDRPOOL_H__
#define __IP_ADDRPOOL_H__

/*
 * String used for name of pool to use if local pooling is enabled
 * globally (ip address-pool local) and the interface is in a default
 * state (peer default ip address pool)
 */

#define DEFAULT_LOCAL_POOL_NAME "default"

/*
 * ip_peer_pool_type byte in idb structure has these values
 */
enum PEER_POOL_TYPES {		/* Configured source of ip_peer_addr */
    PEER_ADDR_DEFAULT,
    PEER_ADDR_POOL,
    PEER_ADDR_DHCP,
    PEER_ADDR_CONFIG,
    PEER_ADDR_NONE,
    MAX_POOL_TYPES
};

/*
 * ensure we don't pick a valid pool type
 */
#define PEER_NEIGHBOR_ROUTE MAX_POOL_TYPES

/*
 * ip_peer_addr_source byte in idb structure has these values
 */
enum PEER_ADDR_SOURCES {	/* Actual source of ip_peer_addr */
    PEER_ADDR_FROM_CONFIG,  /* addr is configured peer address (if any) */
    PEER_ADDR_FROM_DHCP,    /* addr is from dchp pool */
    PEER_ADDR_FROM_POOL,    /* addr is from local pool */
    PEER_ADDR_FROM_PEER,    /* addr is from peer (thru ipcp) */
    PEER_ADDR_FROM_AAA,     /* addr is from TACACS */
    PEER_ADDR_FROM_EXEC,    /* addr is from exec command */
    PEER_ADDR_FROM_VTY,     /* addr is from VTY pt command */
    PEER_ADDR_FROM_MAP,     /* addr is from dialer map */
    PEER_ADDR_FROM_CHAT     /* addr is from chat script */
};


/*
 * Structure to hold all peer config and operational parameters and avoid
 * IDB growth (well sort of)
 *
 * For some IDBs, the operational values (addr_source, addr, pool, etc) are
 * kept in the IDB's copy of this structure and the configured values are
 * placed in the netcfg_idb's copy of this structure (ie the D-channel or
 * the hunt_group_master).  Note that the 'addr_def' value never applies
 * to hunt (rotary) groups so that no remapping needs to be done to access
 * it.  The remapping is done by peer_config_idb (which could be a general
 * function someday).
 */
struct ippeerinfotype_ {
    ipaddrtype ippeer_addr_def;	/* Configured peer IP addr for P2P links */
    char *ippeer_poolname_def;	/* Configured Pool to allocate peer addr */
    ipaddrtype ippeer_addr;	/* Current peer IP addr for P2P links */
    ipaddrtype neighbor;	/* Same as ippeer_addr iff a route added */
    char ippeer_pool_type;	/* Configured pool mechanism */
    char ippeer_addr_source;	/* Actual source of address */
    tinybool no_neighbor_route;	/* Disable auto creation of neighbor route */
};


/*
 *  the ip address pool mechanism structure
 */
struct ip_addresspool_type_ {
    enum PEER_POOL_TYPES pool_type;         /* the pool mechanism type */
    char *pool_desc_string;                 /* string describing mech */
    void (*vpool_startup)(void);            /* startup function */
    void (*vpool_shutdown)(void);           /* shutdown function */
    void (*vpool_show)(void);               /* display function */
    int (*vpool_get)(ipaddrtype *,
		     idbtype *, uchar *,
		     uchar *, ulong);       /* allocate function */
    void (*vpool_free)(ipaddrtype,
		       idbtype *);          /* release function */
    void (*vpool_chown)(ipaddrtype,
			idbtype *);	    /* change owner (IDB) function */
};


/*
 * some vector abbreviations
 */
#define POOL_STARTUP(pooltype_ptr) ((*((pooltype_ptr)->vpool_startup)) ())
#define POOL_SHUTDOWN(pooltype_ptr) ((*((pooltype_ptr)->vpool_shutdown)) ())
#define POOL_SHOW(pooltype_ptr) ((*((pooltype_ptr)->vpool_show)) ())
#define POOL_STRING(pooltype_ptr) ((pooltype_ptr)->pool_desc_string)
#define POOL_GET(pooltype_ptr) (*((pooltype_ptr)->vpool_get))
#define POOL_FREE(pooltype_ptr) (*((pooltype_ptr)->vpool_free))
#define POOL_CHOWN(pooltype_ptr) (*((pooltype_ptr)->vpool_chown))


/*
 * IP pooling result codes for request & release
 */
#define POOL_RET_FAIL     0
#define POOL_RET_OK       1
#define POOL_RET_PENDING -1


/*
 * Create ip_peer info area if it doesn't already exist and initialize to
 * zeros.
 *
 * NOTE:  This memory is never freed.  It is allocated in this manner because
 * no other method could be found to ensure that it is only allocated if
 * necessary.
 */

static inline ippeerinfotype *create_ippeerinfo(idbtype *idb)
{
    if (!idb->ip_peer)
	idb->ip_peer = malloc_named(sizeof(ippeerinfotype), "ippeerinfotype");
    return idb->ip_peer;
}


/*
 *  some data values used by address_pool
 */

extern ip_addresspool_type *ippools[MAX_POOL_TYPES];


/*
 * addr_pool.c
 */
extern void ip_address_pool_command(parseinfo *);
extern void peer_config_command(parseinfo *);
extern boolean ip_pooling_available(idbtype *);


#endif __IP_ADDRPOOL_H__
