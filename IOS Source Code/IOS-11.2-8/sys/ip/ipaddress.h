/* $Id: ipaddress.h,v 3.3.60.2 1996/09/05 23:59:40 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaddress.h,v $
 *------------------------------------------------------------------
 * Prototypes for ipaddress.c
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ipaddress.h,v $
 * Revision 3.3.60.2  1996/09/05  23:59:40  rbadri
 * CSCdi64212:  martian table bogusly externed in a .c file.
 * Branch: California_branch
 * Use registry to adjust the martian table. Created a new boolean
 * to mark the validity of the entry.
 *
 * Revision 3.3.60.1  1996/03/18  20:16:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/14  01:06:37  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.3.26.1  1996/02/20  14:25:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  09:34:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:30  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:56:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/17  14:15:02  widmer
 * CSCdi41543:  IP tunnel address not wholly erased
 * Missed file in previous commit.
 *
 * Revision 2.4  1995/09/18  19:35:57  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.3  1995/08/15  00:20:04  bcole
 * CSCdi38170:  Ping to HSRP IP address causes ICMP echo request storm
 *
 * Add notion of dynamic IP aliases, so that HSRP virtual IP address is
 * now pingable.  Eliminate packet forwarding problem for this virtual
 * IP address as a side effect.
 *
 * Revision 2.2  1995/06/16  21:05:37  shankar
 * CSCdi35615:  FR autoinstall of Accesspro unsuccesful. 2500 works okay.
 * Credit Ashwin with this fix.
 *
 * Revision 2.1  1995/06/07  20:58:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IPADDRESS_H__
#define __IPADDRESS_H__

# define MAXTRIES 	7
# define PAUSE_TIME 	15

inline static boolean valid_autoinstall_if (hwidbtype *idb)
{
    if (idb->status & IDB_PCBUS)
    	return FALSE;
    if ((idb->status & IDB_SERIAL) == 0)
	return FALSE;
    if (idb->status & IDB_ASYNC)
	return FALSE;
    if (idb->status & IDB_ISDN)
	return FALSE;
    return(TRUE);
}

typedef enum ipaddr_type {
    IPADDR_UNKNOWN,
    IPADDR_INTERFACE,
    IPADDR_ALIAS,
    IPADDR_DYN_ALIAS
} ipaddress_type;

/*
 * The data structure into which we stick all aliases and interface
 * address for quick lookups
 */
typedef struct ipaddr_tabletype_ {
    struct ipaddr_tabletype_ *next;
    ipaddrtype address;
    idbtype *swidb;
    ulong port;
    void *data;
    ipaddress_type type;
    ushort addrflags;
} ipaddr_tabletype;

#define IPADDRFLAG_NO_ARP_RELOAD	0x0001	/* Don't reload ARP table */

extern ipaddr_tabletype *ipaddr_hash_table[];
#define FOR_IPADDR_HASH_CHAIN(address, ptr)\
    for (ptr = ipaddr_hash_table[nethash(address)]; ptr; ptr = ptr->next)

/*
 * We allocate ipaddr_tabletype elements using the chunk manager.
 * We allocate 50 addresses per chunk.
 */
#define IPADDR_TABLE_ENTRIES  50

extern ipaddrtype default_gateway;

extern void ip_interface_erase(idbtype *);
extern void fixup_ipsockets(void);
extern boolean ip_stay_shutdown(idbtype *);
extern void ip_fixsocket_address(idbtype *, hwidbtype *);
extern void resolve_ipaddr(ipaddrtype *, ipaddrtype *, ipaddrtype,
			   ipaddrtype *, ipaddrtype *);
ipaddrtype ip_ouraddr(hwidbtype *);
extern void ip_get_netsmask(ipaddrtype, ipaddrtype *);
extern void attempt_autoinstall(void);
extern forktype autoinstall_over_serial(void); 
extern void resolve_interface(void);
extern void ipaddr_table_init(void);
extern boolean ipaddr_table_insert(ipaddrtype, idbtype *, enum ipaddr_type,
				   ushort, ulong, void *);
extern ipaddress_type ip_ouraddress_type(ipaddrtype);
extern char *ipaddresstype_string (ipaddress_type);
extern boolean ipaddr_table_delete(ipaddrtype, idbtype *);
extern idbtype *ipaddr_findidb(ipaddrtype);
extern ipaddrtype ip_defaultgw(void);
extern boolean ip_staticaliasaddr(ipaddrtype);
extern boolean ip_adjust_martian(ipaddrtype, ipaddrtype, boolean);


#endif __IPADDRESS_H__
