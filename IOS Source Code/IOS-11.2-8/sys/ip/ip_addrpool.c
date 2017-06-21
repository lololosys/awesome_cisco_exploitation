/* $Id: ip_addrpool.c,v 3.10.6.8 1996/09/12 13:31:27 fox Exp $
 * $Source: /release/112/cvs/Xsys/ip/ip_addrpool.c,v $
 *------------------------------------------------------------------
 * ip_addrpool.c - IP Peer Address Pooling source file
 *
 * Sept 10, 1995, Craig Fox
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ip_addrpool.c,v $
 * Revision 3.10.6.8  1996/09/12  13:31:27  fox
 * CSCdi68759:  AAA per-user routes are not removed when connection drops
 * Branch: California_branch
 * Separate parallel connection support for per-user routes from the
 * support for neighbor routes and dynamic dialer maps.  Transfer
 * ownership of the static route to the last idb to add it (applies only
 * to per-user routes).
 *
 * Revision 3.10.6.7  1996/09/10  06:16:16  fox
 * CSCdi68456:  Parallel connection information lost if one link
 * disconnects
 * Branch: California_branch
 * Expand check for parallel connections to protect IP dynamic dialer
 * maps.
 *
 * Revision 3.10.6.6  1996/07/02  22:19:13  tkolar
 * CSCdi61856:  peer default ip address is immune to default keyword
 * Branch: California_branch
 *
 * Revision 3.10.6.5  1996/06/23  21:32:32  fox
 * CSCdi61073:  Invalid Memory Free at Interrupt Level
 * Branch: California_branch
 * Don't clear IP peer address from net_cstate().  Remove redundant
 * support for clearing IP peer address on a IP routing topology change
 * as well.
 *
 * Revision 3.10.6.4  1996/06/19  20:35:21  fox
 * CSCdi59847:  Dynamic dialer maps are not cleared on async dialer ints
 * Branch: California_branch
 * Call reg_invoke_ipcp_cleanup_ip_peer() to remove the neighbor route,
 * the dynamic dialer map, and any other mechanisms tied to the current
 * peer address before clearing the peer address.
 *
 * Revision 3.10.6.3  1996/06/17  23:32:06  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/05/21  09:51:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.10.6.1  1996/03/18  20:16:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.3  1996/03/16  06:57:32  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.14.2  1996/03/13  01:37:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.14.1  1996/02/20  00:58:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/05  07:07:27  fox
 * CSCdi50489:  unnecessary neighbor route installed, backup line never
 * drops
 * Add '[no] peer neighbor-route' command
 *
 * Revision 3.9  1996/02/22  14:34:25  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.8  1996/02/01  06:04:14  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/30  16:03:16  fox
 * CSCdi47721:  Peer IP address supported on non-PPP/SLIP interfaces
 * Check for enctype of PPP or SLIP instead of for point-to-point link
 *
 * Revision 3.6  1996/01/29  07:28:30  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.5  1996/01/26  04:34:52  fox
 * CSCdi47583:  AAA authorization frees but uses IP addresses from a pool
 * Don't let a pool or DHCP supplied address be marked any other way
 *
 * Revision 3.4  1995/12/17  18:29:26  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/19  03:33:42  fox
 * CSCdi43677:  IPCP rejects peer address if address pooling not configured
 * Accept peer address if no address pooling or peer address enabled.
 * Allow TACACS+ to override local address pooling configuration.
 *
 * Revision 3.2  1995/11/18  17:41:36  fox
 * CSCdi43045:  ppp and slip exec-level commands are broken
 * Don't reserve peer address if interface is down.
 * If address pooling is disabled, allow peer address.
 *
 * Revision 3.1  1995/11/09  11:55:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/10/25  16:16:54  vandys
 * CSCdi42183:  DHCP can not be used as pooling mechanism
 * Special handling when "default" pool is chosen
 *
 * Revision 2.3  1995/10/11  05:51:26  fox
 * CSCdi40958:  PPP address negotiation doesnt work with dialer interfaces
 * Add ip_peer_addr() function as it was assumed to be
 * Make ip_pooling_available() work for a incomplete interface (no IDB)
 * Reset the peer address at net_cstate and ip_route_adjust time
 * Hook ip_peer_addr() to reg_invoke_ip_peer_addr() registry stub
 * Hook clear_ip_peer_addr() to reg_clear_invoke_ip_peer_addr()
 *
 * Revision 2.2  1995/10/04  04:30:10  irfan
 * CSCdi38219:  Async def ip addr should not be the same as int ip addr
 * Shortening of available rope. When configuring peer default
 * ip addr ensure that it's different from any interfaces's
 * address (numbered/unnumbered).
 *
 * Revision 2.1  1995/09/18  19:35:34  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "interface_private.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "logger.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "ttysrv.h"
#include "ieee.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../parser/parser_defs_tcpip.h"
#include "../ui/debug.h"
#include "../ip/ip_localpool.h"
#include "../wan/dialer.h"
#include "../ip/ipaddress.h"
#include "../ip/ip_addrpool.h"
#include "../ip/ip_debug.h"
#include "../wan/ppp_registry.h"

/*
 * Function prototypes for functions in this module
 */

static void ip_free_pool (ipaddrtype, idbtype *);

static void null_function(void);
static int null_get_function(ipaddrtype *, idbtype *,
			     uchar *, uchar *, ulong);
static void null_free_function(ipaddrtype, idbtype *);
static void null_chown_function(ipaddrtype, idbtype *);

/*
 * Available IP pooling mechanisms
 */
static ip_addresspool_type ippool_none = \
       {PEER_ADDR_DEFAULT, "None", null_function, null_function, null_function,
        null_get_function, null_free_function, null_chown_function};

static ip_addresspool_type *default_ip_pool = &ippool_none;

/*
 * This pointer table is used to record supported pooling mechanisms.  It
 * is globally readable but settable only through a registry call.  It
 * should be initialized to pointers to the None type so that print
 * calls etc are safe.
 */
ip_addresspool_type *ippools[MAX_POOL_TYPES] = { &ippool_none };

static list_header ippeer_addrpool_list;

/*
 * ip_add_addrpool_type
 *
 * Add a new pooling mechanism to the supported set.
 *
 * There are three error conditions that should be reported as error
 * messages.
 */
static void ip_add_addrpool_type (ip_addresspool_type *ippool)
{
    /* sanity checks */
    if (!ippool)
	return;

    if (ippool->pool_type >= MAX_POOL_TYPES)
	return;

    /*
     * Valid pointer (well at least non-zero :-)) and valid type,
     * just install it, creaming anything that might already be there.
     */
     ippools[ippool->pool_type] = ippool;
}


/*
 * ip_address_pool_command
 *
 * configure one of ip address-pool { dhcp-proxy-client |
 *                                    local
 *                                    tacacs }
 */
void ip_address_pool_command (parseinfo *csb)
{
    if (csb->nvgen) {
	/*
	 * Single nvgen parse node for each option.  Pick out which
	 * type of pool we're running and print out the config text
	 * for the selected option.
	 */
	nv_write(default_ip_pool->pool_type == PEER_ADDR_DHCP,
	    "%s dhcp-proxy-client", csb->nv_command);
	nv_write(default_ip_pool->pool_type == PEER_ADDR_POOL,
	    "%s local", csb->nv_command);
	return;
    }
    
    if (csb->sense) {

        switch (csb->which) {

        case PEER_ADDR_DHCP:
	    default_ip_pool = ippools[PEER_ADDR_DHCP];
	    break;

        case PEER_ADDR_POOL:
	    default_ip_pool = ippools[PEER_ADDR_POOL];
	    break;
        }

        POOL_STARTUP(default_ip_pool);

    } else

	default_ip_pool = &ippool_none;
}

/*
 * peer_config_command
 *
 * on point to point interfaces, the pool feature is
 * available as default once ip pooling is turned on. This
 * command can disable it or enable it on a per interface basis.
 */
void peer_config_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;      /* Get software IDB */
    ippeerinfotype *ip_peer;	/* local variable to reduce dereferencing */
    ipaddrtype ippeer_addr_def = 0;
    ipaddrtype *temp_ipaddr;
    char *ippeer_poolname_def = NULL;
    enum PEER_POOL_TYPES ippeer_pool_type = PEER_ADDR_DEFAULT;
    list_element *el;

    if (!idb || !is_ppp_or_slip(idb->hwptr))
	return;
    
    if (!idb->ip_peer && !create_ippeerinfo(idb))
	return;

    ip_peer = idb->ip_peer;

    if (csb->nvgen) {     /* Default is POOL */

	switch (csb->which) {
	  case PEER_ADDR_NONE:
	    /* There are several variants of this command. */
	    switch (ip_peer->ippeer_pool_type) {
	      case PEER_ADDR_POOL:
		nv_write(TRUE, "%s pool %s", csb->nv_command,
			 ip_peer->ippeer_poolname_def);
		break;
	      case PEER_ADDR_DHCP:
		nv_write(TRUE, "%s dhcp", csb->nv_command);
		break;
	      case PEER_ADDR_CONFIG:
		nv_write(TRUE, "%s %i", csb->nv_command,
			 ip_peer->ippeer_addr_def);
		break;
	      case PEER_ADDR_NONE:
		nv_write(TRUE, "no %s", csb->nv_command);
		break;
	      case PEER_ADDR_DEFAULT:
	      default:
		break;
	    }

	  case PEER_NEIGHBOR_ROUTE:
	    nv_write(ip_peer->no_neighbor_route, "no %s", csb->nv_command);

	  default:
	    break;
	}
	return;
    }

    if (csb->which == PEER_NEIGHBOR_ROUTE) {
	/*
	 * Default is FALSE meaning create neighbor routes
	 */
	ip_peer->no_neighbor_route = !(csb->sense || csb->set_to_default);
	return;
    }

    if (csb->sense) {

	switch (csb->which) {

	case PEER_ADDR_CONFIG:
	    /*
	     * The address has been checked out by the parser, and is
	     * zero for the NO form, so simply save it. When configuring
	     * a new address, ensure that it's different from
	     * any peer address that might be configured on other interfaces
	     * AND any interfaces's address (numbered/unnumbered).
	     */

	    ippeer_pool_type = PEER_ADDR_CONFIG;
	    ippeer_addr_def = GETOBJ(paddr,1)->ip_addr;

	    if (csb->sense) {
		hwidbtype *temp_hwidb;

		FOR_ALL_DATA_IN_LIST(&ippeer_addrpool_list, el, temp_ipaddr) {
		    if (*temp_ipaddr == ippeer_addr_def) {
			printf("\n%%Peer IP address %i in use.",
			       ippeer_addr_def);
			return;
		    }
		}

		FOR_ALL_HWIDBS(temp_hwidb) {
		    if (ip_ouraddr(temp_hwidb) ==
			GETOBJ(paddr,1)->ip_addr) {
			printf("\n%% Address %i in use on interface %s",
			       GETOBJ(paddr,1)->ip_addr,
			       temp_hwidb->hw_namestring);
			return;
		    }
		}
	    }

	    /*
	     * Update the current peer address iff it won't override another.
	     * Rotaries and ISDN cannot not be configured with a specific
	     * address so we should never get here unless idb == net_idb!!!!
	     */
	    if (ip_peer->ippeer_addr_source == PEER_ADDR_FROM_CONFIG) {
		ip_peer->ippeer_addr = ippeer_addr_def;

		/*
		 * Put this IP Peer address on the master list.
		 */
		list_enqueue(&ippeer_addrpool_list, NULL, 
			     &(ip_peer->ippeer_addr));
	    }
	    break;

	case PEER_ADDR_DHCP:
	    ippeer_pool_type = PEER_ADDR_DHCP;
            POOL_STARTUP(ippools[PEER_ADDR_DHCP]);
	    break;

	case PEER_ADDR_POOL:
	    /*
	     * First make sure we have a valid pool name
	     */
	    setstring(&ippeer_poolname_def, GETOBJ(string, 1));
	    if (!ippeer_poolname_def) {
		printf(nomemory);
		return;		/* don't change the configuration! */
	    }
	    ippeer_pool_type = PEER_ADDR_POOL;
            POOL_STARTUP(ippools[PEER_ADDR_POOL]);
	    break;

	default:
	    break;

	} /*switch */

    } else if (csb->set_to_default) {
	ippeer_pool_type = PEER_ADDR_DEFAULT;
    }  else {
	ippeer_pool_type = PEER_ADDR_NONE; /* No address flag */
	list_remove(&ippeer_addrpool_list, NULL, &(ip_peer->ippeer_addr));
    }

    /*
     * Release old pool string, even if same name we will have a new string
     */
    if (ip_peer->ippeer_poolname_def)
        free(ip_peer->ippeer_poolname_def);

    /*
     * Then install new values
     * We do this in this order so that we can sanity check the input
     * before changing the configuration.
     */
    ip_peer->ippeer_pool_type = ippeer_pool_type;
    ip_peer->ippeer_addr_def = ippeer_addr_def;
    ip_peer->ippeer_poolname_def = ippeer_poolname_def;
}

/*
 * pool vector functions
 */

static void null_function (void)
{}

static int null_get_function (ipaddrtype *a, idbtype *b,
			     uchar *c, uchar *d, ulong e)

{
    return POOL_RET_FAIL;
}

static void null_free_function (ipaddrtype a, idbtype *b)

{ }

static void null_chown_function (ipaddrtype a, idbtype *b)

{ }

/*
 * Return the ip_peer->addr field in the IDB.
 * Attempt in one spot to get all of this stuff right.
 */

static ipaddrtype ip_peer_addr (idbtype *swidb)
{
    /* sanity checks */

    if (!swidb || !swidb->ip_peer) {
	return 0L;	/* if no ip_peer struct then we have no peer addr */
    }

    if (!interface_up_simple(swidb)) {
	return 0L;	/* if interface is down, address is not in user */
    }

    return (swidb->ip_peer->ippeer_addr);
}


/*
 * Set the ip_peer->addr field in the IDB, setting the source of the peer
 * address at the same time, and freeing (if necessary) any previous peer
 * address.  Attempt in one spot to get all of this stuff right.
 */

static void set_ip_peer_addr (idbtype *swidb, ipaddrtype ippeer_addr,
			      enum PEER_ADDR_SOURCES ippeer_source)
{
    ippeerinfotype *ip_peer;	/* local variable to reduce dereferencing */

    /* sanity checks */

    if (!swidb)
	return;

    if (!swidb->ip_peer && !create_ippeerinfo(swidb))
	return;

    ip_peer = swidb->ip_peer;

    if (peer_debug)
	buginf("\nset_ip_peer_addr: %s: address = %i (%d)",
	       swidb->short_namestring, ippeer_addr, (uint)ippeer_source);

    /* Remove any previous address */

    switch (ip_peer->ippeer_addr_source) {

    case PEER_ADDR_FROM_DHCP:
    case PEER_ADDR_FROM_POOL:
	/*
	 * If this is the same address, someone is attempting to claim a pool
	 * address as their own.  Nope, no way!  This does not catch all such
	 * failures, but you have to start somewhere.
	 */
        if (ip_peer->ippeer_addr == ippeer_addr) {
	    if (peer_debug)
		buginf(" is redundant");
	    return;
	}
	ip_free_pool(ip_peer->ippeer_addr, swidb);
	break;

    case PEER_ADDR_FROM_AAA:
    case PEER_ADDR_FROM_EXEC:
    case PEER_ADDR_FROM_VTY:
    case PEER_ADDR_FROM_MAP:
    case PEER_ADDR_FROM_CHAT:
    case PEER_ADDR_FROM_PEER:
    case PEER_ADDR_FROM_CONFIG:
	break;
    }

    /* If we are deallocating addresses drop back to any default */

    if (!ippeer_addr && (ippeer_source == PEER_ADDR_FROM_CONFIG))
	ippeer_addr = ip_peer->ippeer_addr_def;

    /* Record the new values */

    ip_peer->ippeer_addr = ippeer_addr;
    ip_peer->ippeer_addr_source = ippeer_source;
}


/*
 * Clear IP peer address, ie reset to configured value
 */
static void clear_ip_peer_addr (idbtype *idb)
{
    /*
     * We don't want a special peer address so if no peer address
     * structure has been created yet, don't bother doing it now.
     * This is not a special check since we had to make the validity
     * checks anyway.
     */
    if (idb && idb->ip_peer) {
	/*
	 * Clearing the IP peer address means that neighbor routes and
	 * dialer maps are no longer valid.  Might as well clear them
	 * now.
	 */
	reg_invoke_ipcp_cleanup_ip_peer(idb);
	/*
	 * Check to make sure that this we have an address to clear.
	 * This function is called from a multitude of places to ensure
	 * that we don't inadvertently hold on to a pool address.  Thus
	 * there may be no need to actually call set_ip_peer_addr().
	 */
	if (idb->ip_peer->ippeer_addr_source != PEER_ADDR_FROM_CONFIG) {
	    set_ip_peer_addr(idb, 0, PEER_ADDR_FROM_CONFIG);
	}
    }
}


static void move_ip_peer_addr (tt_soc *src_tty, tt_soc *dst_tty)
{
    idbtype *src, *dst;

    /*
     * First level sanity checks (don't reference NULLs)
     */
    if (!src_tty || !src_tty->tty_idb || !src_tty->tty_idb->firstsw ||
	!dst_tty || !dst_tty->tty_idb || !dst_tty->tty_idb->firstsw)
	return;

    src = src_tty->tty_idb->firstsw;
    dst = dst_tty->tty_idb->firstsw;

    /*
     * Just return if the source does not have an ip_peer address structure
     * because that means that no peer address exists to be moved.  This is
     * unlikely occurrence but a necessary sanity check.
     */
    if (!src->ip_peer)
	return;

    if (!dst->ip_peer && !create_ippeerinfo(dst))
	return;

    /*
     * Second level sanity checks (don't waste any more time!)
     */
    if (!src->ip_peer->ippeer_addr ||
	(src->ip_peer->ippeer_addr_source == PEER_ADDR_FROM_CONFIG))
	return;

    /*
     * Make space for the address in its new spot
     */
    clear_ip_peer_addr(dst);

    /*
     * Transfer the address, its source and its ownership (if from a pool)
     */
    set_ip_peer_addr(dst, src->ip_peer->ippeer_addr,
		     src->ip_peer->ippeer_addr_source);

    switch (src->ip_peer->ippeer_addr_source) {

    case PEER_ADDR_FROM_DHCP:
	POOL_CHOWN(ippools[PEER_ADDR_DHCP])(dst->ip_peer->ippeer_addr, dst);
	break;

    case PEER_ADDR_FROM_POOL:
	POOL_CHOWN(ippools[PEER_ADDR_POOL])(dst->ip_peer->ippeer_addr, dst);
	break;

    default:
	break;
    }

    /*
     * Moving the IP peer address means that neighbor routes and dynamic
     * dialer maps are no longer valid.  Might as well clear them now.
     */
    reg_invoke_ipcp_cleanup_ip_peer(src);

    /*
     * Now clear out from the source
     * Don't use set_ip_peer_addr as it will attempt to free an address
     * from DHCP or the local address pool(s).
     */
    src->ip_peer->ippeer_addr = src->ip_peer->ippeer_addr_def;
    src->ip_peer->ippeer_addr_source = PEER_ADDR_FROM_CONFIG;

    if (peer_debug)
	buginf("\nmove_ip_peer_addr: %s to %s: address = %i (%d)",
	       src->short_namestring, dst->short_namestring,
	       dst->ip_peer->ippeer_addr,
	       (uint)dst->ip_peer->ippeer_addr_source);
}


boolean ip_pooling_available (idbtype *idb)
{
    boolean default_flag = (default_ip_pool != &ippool_none);
    idbtype *cfg_idb = get_netcfg_idb(idb); /* get idb with the configuration */

    /*
     * Sanity check the input
     *
     * If no IDB is assigned yet, return the default flag with the assumption
     * that a new IDB will be a default IDB.  This is crude but incoming VTYs
     * do not have an IDB until you get to slip_exec_command() and we use
     * this function to dynamically modify the parse chain before we are that
     * far.
     */
    if (!idb || !cfg_idb)
	return default_flag;

    /*
     * Unlike the tests above, these sanity checks verify that we have the
     * memory to store an ip peer info struct.  Thus a failure here means
     * that pooling is NOT available.
     */
    if (!idb->ip_peer && !create_ippeerinfo(idb))
	return FALSE;
    if (!cfg_idb->ip_peer && !create_ippeerinfo(cfg_idb))
	return FALSE;

    switch (cfg_idb->ip_peer->ippeer_pool_type) {

    case PEER_ADDR_DEFAULT:
	/*
	 * IP pooling is available on a defaulted interface if the
	 * global default is enabled.
	 */
	return default_flag;

    case PEER_ADDR_POOL:
    case PEER_ADDR_CONFIG:
    case PEER_ADDR_DHCP:
	return TRUE;

    case PEER_ADDR_NONE:
    default:
	return FALSE;
    }
}


/*
 * ip_get_pool()
 *	Get an IP address from the appropriate pool
 */
static int
ip_get_pool (ipaddrtype *new_addr, idbtype *idb,
		        uchar *ustring, uchar *poolname, ulong wait_ticks)
{
    ip_addresspool_type *ip_pool;
    boolean named_pool;
    ippeerinfotype *ip_peer;
    idbtype *cfg_idb;
    enum PEER_POOL_TYPES ippeer_pool_type;
    int ret;

    /*
     * Figure out if it's an explicitly named pool.  The default
     * doesn't count as such.
     */
    named_pool = poolname && *poolname;
    if (named_pool) {
	if (!strcmp(poolname, DEFAULT_LOCAL_POOL_NAME)) {
	    named_pool = FALSE;
	}
    }

    /*
     * Test routines go directly to the core allocation functions.  All
     * others MUST have an IDB.
     */
    if (!idb || !new_addr) {
	return POOL_RET_FAIL;
    }

    /*
     * Get idb with the configuration
     */
    cfg_idb = get_netcfg_idb(idb);

    /*
     * Either use the cached IP peer information, or build it now.
     * If we don't have this, pooling can't happen.
     */
    if (!idb->ip_peer && !create_ippeerinfo(idb)) {
	return POOL_RET_FAIL;
    }
    ip_peer = idb->ip_peer;
    if (!cfg_idb->ip_peer && !create_ippeerinfo(cfg_idb)) {
	return POOL_RET_FAIL;
    }

    if (peer_debug) {
	if (*new_addr) {
	    buginf("\nip_get_pool: %s: validate address = %i",
	           idb->short_namestring, *new_addr);
	}
        if (poolname && *poolname) {
	    buginf("\nip_get_pool: %s: using pool %s",
		   idb->short_namestring, poolname);
	}
    }

    /*
     * The interface configuration can be overridden by a TACACS+/AAA
     * authorization request.  In general, if a poolname is passed in
     * either local address pooling is configured on an interface or
     * TACACS+/AAA has provided a poolname and expects that an address
     * lookup will be done from this pool.  So if a poolname is provided,
     * override the interface's address pooling configuration.
     */
    if (named_pool) {
	ippeer_pool_type = PEER_ADDR_POOL;
    } else {
	ippeer_pool_type = cfg_idb->ip_peer->ippeer_pool_type;
    }

    /*
     * Process 'get pool addr' request appropriate to interface config
     */
    switch (ippeer_pool_type) {

    case PEER_ADDR_DEFAULT:
	/*
	 * We will use the Global Default Mechanism but that can pose a
	 * problem if the GDM is not configured (no ip address-pool).
	 * In that case, we can not provide an address but we surely
	 * should accept any that is offered!
	 */
	if (default_ip_pool != &ippool_none) {
	    ip_pool = default_ip_pool;	/* use Global Default Mechanism */
	    break;
	} else {
	    if (*new_addr) {
	        return POOL_RET_OK;	/* let the peer have this address */
	    } else {
	        return POOL_RET_FAIL;	/* Nobody has an address to use */
	    }
	}

    case PEER_ADDR_NONE:
	/*
	 * Address pooling disabled so let the peer have his address if
	 * he presented one or else return failure, no address available.
	 */
	if (*new_addr) {
	    return POOL_RET_OK;		/* let the peer have this address */
	} else {
	    return POOL_RET_FAIL;	/* Nobody has an address to use */
	}

    case PEER_ADDR_CONFIG:
	/*
	 * Use configured address unless the configured address is obviously
	 * invalid.
	 */
	if (!ip_peer->ippeer_addr_def) {
	    return POOL_RET_FAIL;
	}
	/*
	 * Fail if peer want an address other than the one configured.
	 */
	if (*new_addr && (*new_addr != ip_peer->ippeer_addr_def)) {
	    return POOL_RET_FAIL;
	}
	/*
	 * If we have a different address from a pool, return it first
	 */
	if (ip_peer->ippeer_addr_source != PEER_ADDR_FROM_CONFIG) {
	    ip_free_pool(ip_peer->ippeer_addr, idb);
	}
	*new_addr = ip_peer->ippeer_addr_def;
	return POOL_RET_OK;

    case PEER_ADDR_POOL:
	ip_pool = ippools[PEER_ADDR_POOL];
	break;

    case PEER_ADDR_DHCP:
	ip_pool = ippools[PEER_ADDR_DHCP];
	break;

    default:
	return POOL_RET_FAIL;		/* fail any unknown methods */
    }

    /*
     * If we have a different address from a pool, return it first unless
     * there is a chance that we may want it back if the allocation fails
     */
    if (ip_peer->ippeer_addr_source != PEER_ADDR_FROM_CONFIG) {
	if ((ip_peer->ippeer_addr_source != PEER_ADDR_FROM_POOL) ||
		(ip_pool != ippools[PEER_ADDR_POOL])) {
	    ip_free_pool(ip_peer->ippeer_addr, idb);
	}
    }

    /*
     * Break out to the actual pool mechanism selected
     */
    ret = (POOL_GET(ip_pool)(new_addr, idb, ustring, poolname, wait_ticks));

    if (peer_debug) {
	if (ret) {
	    buginf("\nip_get_pool: %s: returning address = %i",
	           idb->short_namestring, *new_addr);
	} else if (*new_addr) {
	    buginf("\nip_get_pool: %s: invalid address = %i",
	           idb->short_namestring, *new_addr);
	} else {
	    buginf("\nip_get_pool: %s: no address available",
		   idb->short_namestring);
	}
    }

    return ret;
}

/*
 * ip_free_pool
 *
 * see if we got this address leased from somebody.
 * release and purge from our queue.
 *
 * set to use configured address if any.  note that configured address will
 * never be set in any idb of a rotary group.
 */
static void ip_free_pool (ipaddrtype addr, idbtype *idb)
{
    ip_addresspool_type *ip_pool;
    ippeerinfotype *ip_peer;	/* local variable to reduce dereferencing */

    /*
     * Just return if IDB does not have a peer address structure because
     * this implies that he has no address either.
     */
    if (!idb->ip_peer)
	return;

    ip_peer = idb->ip_peer;

    switch (ip_peer->ippeer_addr_source) {

    case PEER_ADDR_FROM_POOL:
	ip_pool = ippools[PEER_ADDR_POOL];
	break;

    case PEER_ADDR_FROM_DHCP:
	ip_pool = ippools[PEER_ADDR_DHCP];
	break;

    default:
	return;			/* address didn't come from us */
    }

    if (peer_debug)
	buginf("\nip_free_pool: %s: address = %i (%d)",
	       idb->short_namestring, addr, (uint)ip_peer->ippeer_addr_source);

    POOL_FREE(ip_pool)(addr, idb);

    ip_peer->ippeer_addr = ip_peer->ippeer_addr_def;
    ip_peer->ippeer_addr_source = PEER_ADDR_FROM_CONFIG;
}

/*
 * Parser chains for addrpool scripts.
 * Include these in menus.c to simplify makefile changes necessary to
 * add dhcp to an image.
 */

#include "../parser/parser_actions.h"
#include "../parser/macros.h"

/*
 * Parser chains and definitions for DHCP client subsystem
 */

/*
 * Parse chains for IP DHCP subcommand
 */
#define	ALTERNATE	NONE
#include "cfg_addrpool.h"
LINK_POINT(ip_addrpool_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for PEER DEFAULT commands
 */
#define	ALTERNATE	NONE
#include "cfg_int_peer.h"
LINK_POINT(peer_interface_commands, ALTERNATE);
#undef	ALTERNATE


/*
 * Parse chain registration array for IP address pooling UI
 */
static parser_extension_request ip_addrpool_chain_init_table[] = {
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ip_addrpool_commands) },
    { PARSE_ADD_CFG_INT_CMD, &pname(peer_interface_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * ip_addrpool_init
 * Initialize the ip_addrpool system. Includes setting up data
 * structures, registering parse chains and hooking into some
 * ip registeries.
 */
static void ip_addrpool_init (subsystype *subsys)
{
    parser_add_command_list(ip_addrpool_chain_init_table, "Dhcp Scripts");

    peer_debug = FALSE;
    reg_add_ip_get_pool_addr(ip_get_pool, "ip_get_pool");
    reg_add_ip_add_pooling_mechanism(ip_add_addrpool_type,
				     "ip_add_addrpool_type");
    reg_add_clear_ip_peer_addr(clear_ip_peer_addr, "clear_ip_peer_addr");
    reg_add_set_ip_peer_addr(set_ip_peer_addr, "set_ip_peer_addr");
    reg_add_move_ip_peer_addr(move_ip_peer_addr, "move_ip_peer_addr");
    reg_add_ip_peer_addr(ip_peer_addr, "ip_peer_addr");

    /*
     * Initialize a batch of well-known mechanism pointers to prevent
     * vectoring thru low addresses.  This is done in case the initializer
     * earlier in the file did not work.
     */
    ippools[PEER_ADDR_CONFIG] = &ippool_none;
    ippools[PEER_ADDR_NONE] = &ippool_none;
    ippools[PEER_ADDR_POOL] = &ippool_none;
    ippools[PEER_ADDR_DHCP] = &ippool_none;

    list_create(&ippeer_addrpool_list, 0, "IP Peer Address Pool",
		 LIST_FLAGS_AUTOMATIC);
}

/*
 * IP Address Pool subsystem header
 */
#define IP_ADDRPOOL_MAJVERSION 1
#define IP_ADDRPOOL_MINVERSION 0
#define IP_ADDRPOOL_EDITVERSION  1

SUBSYS_HEADER(ip_addrpool_sys,
	      IP_ADDRPOOL_MAJVERSION, IP_ADDRPOOL_MINVERSION,
	      IP_ADDRPOOL_EDITVERSION,
	      ip_addrpool_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      NULL);
