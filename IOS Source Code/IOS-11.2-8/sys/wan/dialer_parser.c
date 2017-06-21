/* $Id: dialer_parser.c,v 3.1.16.33 1996/09/10 07:32:39 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer_parser.c,v $
 *------------------------------------------------------------------
 * Dialer parser interface.
 *
 * February 1996 - Lionel Bustini
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer_parser.c,v $
 * Revision 3.1.16.33  1996/09/10  07:32:39  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix botched comment during previous commit of CSCdi62263
 *
 * Revision 3.1.16.32  1996/09/10  07:17:36  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix multilink and dialer profiles
 *
 * Revision 3.1.16.31  1996/09/10  02:43:21  suresh
 * CSCdi68129:  dialer pool-member 1 min-link 1 doesnt reserve the link
 * Branch: California_branch
 * Completely rewritten code for sharing of ISDN interfaces
 *
 * Revision 3.1.16.30  1996/08/31  01:49:25  sjackson
 * CSCdi38165:  show dialer spacing not correct
 * Corrected spacing and display of dynamic entries disallowed
 * Branch: California_branch
 *
 * Revision 3.1.16.29  1996/08/28  13:21:14  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.16.28  1996/08/23  04:25:28  fox
 * CSCdi66589:  ppp_periodic() dereferences null pointer - idb->lcpfsm
 * during init
 * Branch: California_branch
 * Disable interrupts in isdn_setencap(), dialer_setencap() and
 * async_setencap() before changing encapsulations.  Make sure that
 * lcpfsm pointer in idb is set before idb vectors (such as periodic)
 * are set.
 *
 * Revision 3.1.16.27  1996/08/20  18:08:59  wmay
 * CSCdi39825:  ISDN PRI, setting d-channel mtu size not reflected on
 * b-channels - set for entire dialer rotary group
 * Branch: California_branch
 *
 * Revision 3.1.16.26  1996/08/17  01:38:42  asastry
 * CSCdi63397:  CDP not respecting configurations on b-channels - add
 * comment, per code review.
 *
 * Branch: California_branch
 *
 * Revision 3.1.16.25  1996/08/17  01:29:29  asastry
 * CSCdi63397:  CDP not respecting configurations on B-channels
 * Branch: California_branch
 *
 * Revision 3.1.16.24  1996/08/14  14:19:05  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * The multilink half-fastswitching performance improvement
 *
 * Revision 3.1.16.23  1996/08/13  02:26:50  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.16.22  1996/08/07  09:04:28  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.16.21  1996/08/06  00:37:45  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.1.16.20  1996/08/04  08:09:39  fox
 * CSCdi62182:  %SCHED-3-PAGEZERO: Low memory modified by ISDN (0x74 =
 * 0x0).
 * Branch: California_branch
 * Convert ddb->dial_reason to an array of char from a char *.  Modify all
 * necessary references appropriately.
 *
 * Revision 3.1.16.19  1996/08/02  20:07:15  suresh
 * CSCdi63943:  Dialer Interface should allow only one pool
 * Branch: California_branch
 *
 * Revision 3.1.16.18  1996/07/26  18:49:19  suresh
 * CSCdi63924:  ISDN call setup with 1TR6 and new-style DDR fails
 * Branch: California_branch
 * ISDN SPC is false by default, changed the appropriate command.
 *
 * Revision 3.1.16.17  1996/07/25  21:26:42  suresh
 * CSCdi63943:  Dialer Interface should allow only one pool
 * Branch: California_branch
 * Dialer Profile should enforce only one dialer pool.
 *
 * Revision 3.1.16.16  1996/07/23  22:04:18  suresh
 * CSCdi62055:  Dialer profile error message misleading
 * Branch: California_branch
 * Code Cleanup for change from dialer user to dialer profile.
 *
 * Revision 3.1.16.15  1996/07/19  19:50:34  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.1.16.14  1996/07/19  01:17:45  suresh
 * CSCdi63165:  Code Cleanup: dialer pool
 * Branch: California_branch
 * Cleaned up the dialer pool code.
 *
 * Revision 3.1.16.13  1996/07/18  14:56:12  widmer
 * CSCdi63340:  dialer isdn command not nvgenerated correctly
 * Branch: California_branch
 *
 * Revision 3.1.16.12  1996/07/16  18:08:58  wmay
 * CSCdi61965:  sh dialer on ISDN interfaces doesnt display all B Channels
 * Set up the hunt groups of b-channels for PRI and BRI in interface order-
 * display the entire hunt group when doing a show dialer interface for
 * a hunt group master
 * Branch: California_branch
 *
 * Revision 3.1.16.11  1996/07/13  01:24:23  suresh
 * CSCdi62603:  no dialer pool-member crash on Dialer Profiles
 * Branch: California_branch
 * Fixed a problem on condition check used to search pools.
 *
 * Revision 3.1.16.10  1996/07/01  12:55:40  dblair
 * CSCdi61321:  crash in mlp_remove_link when chaning bri encap
 * Prevent encaps changes on a dialer/ISDN interface from changing any
 * VACCESS interfaces that may be a part of the rotary-group.
 * Branch: California_branch
 *
 * Revision 3.1.16.9  1996/06/19  20:38:08  fox
 * CSCdi59847:  Dynamic dialer maps are not cleared on async dialer ints
 * Branch: California_branch
 * Call reg_invoke_ipcp_cleanup_ip_peer() to remove the neighbor route,
 * the dynamic dialer map, and any other mechanisms tied to the current
 * peer address before clearing the peer address.
 *
 * Revision 3.1.16.8  1996/06/19  00:21:33  carvalho
 * CSCdi33844:  X25 over B-channel broken
 * Branch: California_branch
 *
 * Set new encapsulation type in idb before invoking encapsulation
 * handler.
 *
 * Revision 3.1.16.7  1996/06/17  22:57:53  irfan
 * CSCdi60537:  dialer list command nvgens wrongly
 * Branch: California_branch
 * read old/new; write new.
 *
 * Revision 3.1.16.6  1996/06/07  20:43:53  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 * interface
 *         save default setting and restore it when folks remove RED,
 *         Custom, or Priority Queuing
 * Branch: California_branch
 *
 * Revision 3.1.16.5  1996/05/17  12:16:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.1  1996/05/06  00:07:45  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.16.4  1996/05/10  19:40:54  suresh
 * CSCdi57032:  Spurious access in dialer_command
 * Branch: California_branch
 * Corrected illegal pointer accesses.
 *
 * Revision 3.1.16.3  1996/05/09  14:49:08  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.16.2  1996/05/01  04:38:43  dblair
 * CSCdi55760:  7xxx crash during unconfig of Dialer maps testing over
 * ISDN PRI
 * Branch: California_branch
 *
 * Revision 3.1.16.1  1996/04/27  06:36:14  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.6  1996/04/25  23:58:13  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.1.2.5  1996/04/23  00:24:20  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.1.2.4  1996/04/17  00:00:20  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.1.2.3  1996/04/03  19:53:24  dblair
 * Branch: Dial1_branch
 * Fixed no dialer in-band crash
 *
 * Revision 3.1.2.2  1996/03/08  17:30:30  dblair
 * Branch: Dial1_branch
 * Change for multilink virtual bundle interfaces to support ISDN BRI,
 * PRI, and Dialer interfaces
 *
 * Revision 3.1.2.1  1996/02/23  21:31:24  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.1  1996/02/23  19:29:48  lbustini
 * Placeholders for Dial1_branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "../snmp/ifmib_registry.h"
#include "packet.h"
#include "address.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "access.h"
#include "ieee.h"
#include "../if/network.h"
#include "../wan/parser_defs_dialer.h"
#include "../parser/parser_defs_parser.h"
#include "dialer.h"
#include "dialer_private.h"
#include "dialer_debug.h"
#include "dialer_registry.h"
#include "msg_dialer.c"			/* Not a typo, see logger.h */
#include "../if/priority_private.h"
#include "../if/static_map.h"
#include "../ui/parse_util.h"
#include "../util/regexp.h"
#include "../wan/isdn.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include <ciscolib.h>
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"		/* for tcp header compression */
#include "../crypto/crypto_public.h"
#include "../crypto/crypto_registry.h"

/*
 * Extern structures
 */
extern dialerlisttype darray[DIALER_MAXLIST];
extern dialergrouptype *dialergroup_free_list;
extern dialerpoolheadtype *dialer_pool_head;
extern dialermaptype *dialertable[DIALERHASH];
extern char *const dialer_keywords[];

/*
 * forward references
 */
static idbtype *dialer_create_master_idb(uint);

/*
 * dialer_print_proto_addr
 * Prints a protocol address.
 */
void dialer_print_proto_addr (idbtype *idb, addrtype *addr,
				     uint link, char *str)
{
    switch (link) {
	case LINK_BRIDGE:
	    sprintf(str, "bridge");
	    break;
	case LINK_CLNS:
	    sprintf(str, "clns %n", addr->clns_addr);
	    break;
	default:
	    print_proto_addr(idb, addr, link, str);
    }
}

/*
 * dialer_addr_enter
 */
static void dialer_addr_enter (dialermaptype *addr)
{
  leveltype level;
    int hash;

    hash = xnethash((uchar *)&addr->proto_addr.addr, addr->proto_addr.length)& 
		(DIALERHASH-1);

    level = raise_interrupt_level(NETS_DISABLE);

    addr->next = dialertable[hash];
    dialertable[hash] = addr;

    reset_interrupt_level(level);

    reg_invoke_dialer_map_change(addr->link, addr->ddb->ownidb,
				 &addr->proto_addr, FALSE);
}

/*
 * dialer_entry_enter
 */
static void dialer_entry_enter (dialermaptype *addr, dialerentrytype *entry)
{
    dialerentrytype *e;
    leveltype level;
 
    level = raise_interrupt_level(NETS_DISABLE);
 
    if (addr->entry) {
        /*
         * Run through all numbers to insert at the end
         */
        e = addr->entry;
        while (e->next) 
            e = e->next;

        e->next = entry;
    } else 
	addr->entry = entry;

    entry->head = addr;

    reset_interrupt_level(level);
}

/*
 * dialer_clear_group
 * Clear the "dialer_call_group" field if it matches the passed map pointer.
 * Clear it unconditionally if the pointer is NULL.
 */
void dialer_clear_group (dialerdbtype *ddb, dialermaptype *map)
{
    dialerdbtype *tempddb;
    dialerpooltype *pool;

    /*
     * First deal with the interface's own call group.
     */
    if ((ddb->dialer_call_group == map) || (map == NULL)) {
	ddb->dialer_call_group = NULL;
    }

    if (HUNTGROUP_LEADER(ddb)) {
        /*
         * Deal with rotary groups here, clear call group on each member.
         */
	FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	    reg_invoke_dialer_clear_group(tempddb->dialer, tempddb, map);
	}
        /*
         * Deal with dialer profile members here, clear the call group on
         * all the isdn members using the call group.
         */
        if (ddb->dialer_pool) {
	    FOR_ALL_MEMBERS_IN_POOL(ddb->dialer_pool->pool_head, pool) {
	        if (pool->ddb && is_isdn(pool->ddb->ownidb)) {
	           reg_invoke_dialer_clear_group(pool->ddb->dialer, 
                                                 pool->ddb, map);
	        }
	    }
	}
    }
}

/*
 * dialer_map_delete
 */
static void dialer_map_delete (dialermaptype *hp)
{
    uint hash;
    dialermaptype **map;
    leveltype level;
    dialerdbtype *ddb;
    
    /*
     * Clear call group for this idb, and possibly its members.
     */
    ddb = hp->ddb;
    reg_invoke_dialer_clear_group(ddb->dialer, ddb, hp);

    level = raise_interrupt_level(NETS_DISABLE);
    /*
     * Find and delink the address map from the address hash table.
     */
    hash = xnethash((uchar *)&hp->proto_addr.addr, hp->proto_addr.length) & 
            (DIALERHASH-1);
    map = &dialertable[hash];
    while (*map) {
	if (*map == hp) {
	    *map = hp->next;
	    break;
	}
	map = &((*map)->next);
    }		
    reset_interrupt_level(level);
    reg_invoke_dialer_map_change(hp->link, ddb->ownidb, &hp->proto_addr, TRUE); 
    free(hp);
}

/*
 * dialer_clear_entry
 */
static void dialer_clear_entry (dialerentrytype *entry)
{
    if (entry->dialer_string)
	free(entry->dialer_string);
 
    if (entry->hostname)
	free(entry->hostname);
 
    if (entry->modem_chat_regexp) {
        free(entry->modem_chat_regexp);
        free(entry->modem_chat_regcomp);
    }
 
    if (entry->system_chat_regexp) {
        free(entry->system_chat_regexp);
        free(entry->system_chat_regcomp);
    }
    free(entry);
}

/*
 * dialer_entry_delete
 */
static void dialer_entry_delete (dialermaptype *map, dialerentrytype *entry)
{
    dialerentrytype **e;
    dialerentrytype *prev;
    leveltype level;
 
    level = raise_interrupt_level(NETS_DISABLE);
    /*
     * Find and delink the entry from the entry link list.
     */
    prev = NULL;
    e = &(map->entry);
    while (*e) {
        if (*e == entry) {
            *e = entry->next;
            break;
        }
	prev = *e;
        e = &((*e)->next);
    }
    reset_interrupt_level(level);

    dialer_clear_entry(entry);

    if (!map->entry)
	dialer_map_delete(map);
    else if (map->cip == entry)
	map->cip = prev;
} 

/*
 * dialer_addr_delete
 */
static void dialer_addr_delete (dialermaptype *map)
{
    dialermaptype **m;
    dialerentrytype *e, *etemp;
    leveltype level;
    uint hash;
    dialerdbtype *ddb;
    
    /*
     * Clear call group for this idb, and possibly its members.
     */
    ddb = map->ddb;
    reg_invoke_dialer_clear_group(ddb->dialer, ddb, map);

    level = raise_interrupt_level(NETS_DISABLE);
    hash = xnethash((uchar *)&map->proto_addr.addr, map->proto_addr.length) &
            (DIALERHASH-1);
    /*
     * Find and delink the entry from the entry link list
     */
    m = &dialertable[hash];
    while (*m) {
        if (*m == map) {
            *m = map->next;
            break;
        }
        m = &((*m)->next);
    }

    /*
     * Delete all pairs (address/string) for given address
     */
    e = map->entry;
    while (e) {
        etemp = e->next;
        dialer_clear_entry(e);
        e = etemp;
    }

    reset_interrupt_level(level);
    reg_invoke_dialer_map_change(map->link, ddb->ownidb, &map->proto_addr,TRUE);
    free(map);
}

/*
 * dialer_clear_map
 * Re-initialize the dialer map
 */
 
void dialer_clear_map (dialerdbtype *ddb)
{
    dialermaptype **p, *ptemp;
    dialerentrytype *e, *etemp;
    leveltype level;
    int index;
 
    /*
     * Clear call group for this idb, and possibly its members.
     */
    reg_invoke_dialer_clear_group(ddb->dialer, ddb, NULL);

    if (ddb->dialer_pool != NULL)
	dialer_remove_pool(ddb, ddb->dialer_pool->pool_number);

    /*
     * For all dialer hash table entries
     *   For all address configured for this idb
     *     For all map entries
     *       Delete entry
     */
    for (index = 0; index < DIALERHASH; index++) {
        p = &dialertable[index];
        while (*p) {
            if ((*p)->ddb == ddb) {
    	        level = raise_interrupt_level(NETS_DISABLE);
		e = (*p)->entry;
		while (e) {
		    etemp = e->next;
		    dialer_clear_entry(e);
		    e = etemp;
		}
	        ptemp = *p;
	        *p = (*p)->next;
		free(ptemp);
    		reset_interrupt_level(level);
		continue;
	    }
	    p = &((*p)->next);
        }
    }
}

/*
 * dialer_retrieve_maps
 * Find all maps of the hwidb and linktype specified, calling
 * reg_invoke_dialer_map_change for each.
 */
void dialer_retrieve_maps (hwidbtype *inidb, ulong link)
{
    dialermaptype *l;
    hwidbtype *idb = inidb;
    dialerdbtype *ddb;
    int i;
   
    /*
     * If this is a hunt group member, look up the address in the master list.
     */
    ddb = get_dialerdb(idb);
    ddb = GET_NETDDB(ddb);

    for (i = 0; i < DIALERHASH; i++) {
	l = dialertable[i];
	while (l) {
	    if ((l->link == link) && (l->ddb == ddb)) {
		reg_invoke_dialer_map_change(link, inidb, &l->proto_addr,FALSE)
;
	    }
	    l = l->next;
	}
    }
    return;
}

/*
 * dialer_parser_caller
 * Parse a dialer caller string
 */

void dialer_parser_caller (parseinfo *csb, dialerdbtype *ddb)
{
    int idx;
    dialer_call_tab *tmpcall, *tmpcall1, *tmpcopy;
    boolean match_found;
    char callchars[DIALER_MAX_CALLER];

    /*
     * There are required values after the caller command.
     */
    if (strlen(GETOBJ(string,1)) == 0) {
        printf("\n%%caller command incomplete, number required .");
        return;
    }
    /*
     * we need to add a new caller string.
     */
    strcpy(callchars, GETOBJ(string,1));
    if (csb->sense) {

        /*
         * New entry is added to the top of the list.
         */
        tmpcall = ddb->dialer_call;

        /*
         * We have a new entry. Change "X" to
         * "x" for later compares.
         */
        for (idx = 0; idx <= DIALER_MAX_CALLER; idx++) {
                if (callchars[idx] == 'X')
                    callchars[idx] = 'x';
        }

        /*
         * We have a new entry let's see if it
         * is a duplicate entry.
         */
        while (tmpcall != NULL) {
            if (!strcmp (tmpcall->dialer_call_string, callchars)) {
                return;
            }
            tmpcall = tmpcall->next;
        }
        tmpcall = ddb->dialer_call;
        ddb->dialer_call = malloc(sizeof(dialer_call_tab));
        if (ddb->dialer_call == NULL) {
            ddb->dialer_call = tmpcall;
            printf(nomemory);
            return;
        }
        ddb->dialer_call->next = tmpcall;
        /*
         * tmpcall will be NULL if this is the first entry.
         */
        if (tmpcall != NULL)
            tmpcall->prev = ddb->dialer_call;
        strcpy(ddb->dialer_call->dialer_call_string ,callchars);
    } else {

        /*
         * someone just did a "no dialer caller xxxxx"
         */
        if (strlen(GETOBJ(string,1)) == 0) {
            printf("\n%%dialer caller incomplete command.");
            return;
        }
        match_found = FALSE;
        tmpcall = ddb->dialer_call;
        while (tmpcall != NULL) {
            if (!strcmp (tmpcall->dialer_call_string, callchars)) {
                match_found = TRUE;
                break;
            }
            tmpcall = tmpcall->next;
        }

        /*
         * We need to unthread this entry. tmpcall is
         * the entry being deleted and tmpcall1 is the
         * next entry. tmpcopy is the previous entry.
         */
        if (match_found) {
            tmpcall1 = tmpcall->next;         /* could be NULL */
            tmpcopy = tmpcall->prev;

            /*
             * If the prev pointer is null this is the first
             * entry so we need to thread to the next pointer.
             * If the prev pointer is non null we just unthread.
             */
            if (tmpcopy == NULL) {

                /*
                 * We are removing the first entry in the
                 * list. If this is the only entry we need
                 * to NULL out the idb pointer else we
                 * thread to the next entry.
                 */
                if (tmpcall->next == NULL)
                    ddb->dialer_call = NULL;
                else {
                    ddb->dialer_call = tmpcall1;
                    tmpcall1->prev = NULL;
                }
            } else {
                tmpcopy->next = tmpcall1;
		if (tmpcall1 != NULL)
		    tmpcall1->prev = tmpcopy;
            }
            free (tmpcall);
        }
    }
}

/*
 * dialer_parse_map
 * Parse a map command
 */

static void dialer_parse_map (parseinfo *csb, dialerdbtype *ddb)
{
    hwidbtype *idb;
    addrtype proto_addr;
    dialermaptype *addr;
    dialerentrytype *entry;
    char *hostname;
    char *class;
    uint speed;
    char *modem_script;
    regexp *modem_script_comp;
    char *system_script;
    regexp *system_script_comp;
    char *dialer_string;
    boolean failed;
    boolean no_param;
    boolean cmd_sense;
    boolean broadcast;
    boolean spc;
    int link;

    idb = ddb->ownidb;
    broadcast = GETOBJ(int,4);
    link = GETOBJ(int,1);
    address_copy(&proto_addr, GETOBJ(paddr,1));
    /* Get the object that specifies semi permanent connections */
    spc = GETOBJ(int,5);

    /*
     * The madness below is so we can get the entire line of
     * there is not a name given.  Check to see if the name option follows
     * the nexthop address.  If so, get the name and save it.  If not,
     * restore the pointer to the buffer, and get the entire phone string.
     */

    hostname = NULL;		/* Assume no host name */
    class = NULL;               /* Assume no class name */  
    modem_script = NULL;	/* Assume no modem chat script */
    modem_script_comp = NULL;	/* And no compiled chat script */
    system_script = NULL;	/* Assume no system chat script */
    system_script_comp = NULL;	/* Assume no compile system chat script */
    dialer_string = NULL;	/* Assume no dialer string */
    speed = 0;			/* Assume no speed specified */

    failed = FALSE;
    no_param = TRUE;
    cmd_sense = GETOBJ(flag,sense);

    /*  <dialer-string>  */
    if (GETOBJ(string,1)[0] != '\0') {
	dialer_string = strdup(GETOBJ(string,1));
	if (!dialer_string) {
	    printf(nomemory);
	    failed = TRUE;
	} else {
	    no_param = FALSE;
	}
    }

    /*  name <string>  */
    if (GETOBJ(string,2)[0] != '\0') {
	if ((hostname = malloc(sizeof(char) * 
			     (strlen(GETOBJ(string,2)) + 1))) == NULL) {
	    printf(nomemory);
	    failed = TRUE;
	} else {
	    strcpy(hostname, GETOBJ(string,2));
	    no_param = FALSE;
	}
    }

    /*  class <string>  */
    if (GETOBJ(string,5)[0] != '\0') {
        class = strdup(GETOBJ(string,5));
        if (!class) {
            printf(nomemory);
            failed = TRUE;
        } else {
            strcpy(class, GETOBJ(string,5));
            no_param = FALSE;
        }
    }

    /*  speed { 56 | 64 }  */
    speed = GETOBJ(int,2);
    if (speed)
	no_param = FALSE;

    /*  modem-script <script>  */
    if (GETOBJ(string,3)[0] != '\0') {
	modem_script_comp = regcomp(GETOBJ(string,3));
	if (!modem_script_comp) {
	    printf("\n%%Invalid modem script");
	    failed = TRUE;
	} else {
	    modem_script = strdup(GETOBJ(string,3));
	    if (!modem_script) {
		printf(nomemory);
		failed = TRUE;
	    } else {
	        no_param = FALSE;
	    }
	}
    }

    /*  system-script <script>  */
    if (GETOBJ(string,4)[0] != '\0') {
	/*
	 * Compile the chat script regexp
	 */

	system_script_comp = regcomp(GETOBJ(string,4));
	if (!system_script_comp) {
	    failed = TRUE;
	} else {
	    /*
	     * Save the uncompiled version of the chat script regexp
	     */

	    system_script = strdup(GETOBJ(string,4));
	    if (!system_script) {
		printf(nomemory);
		failed = TRUE;
	    } else {
	        no_param = FALSE;
	    }
	}
    }

    if (no_param && csb->sense) {
	printf("\n%%Incomplete command");
	return;
    }

    if (!failed) {

	addr = dialer_addr_lookup(ddb, &proto_addr, link);
	/*
	 * if (address already in the table)
	 *   if (command line includes string)
	 *     if (address/string already in table)
	 *       update/delete entry
	 *     else if (first entry doesn't have a string defined
	 *       update/delete entry
	 *     else
	 *       add entry to table/printf(Can't find address map for %)
	 *   else if (first entry has a string defined)
	 *     if (more than one entry in table)
	 *       printf(Ambiguous cmd, must specify string)/delete all entries
	 *     else
	 *       update/delete entry
	 *   else     
	 *     update/delete entry
	 * else 
	 *   add entry to table/printf(Can't find address map for %)
	 */
	if (addr != NULL) {
	    if (addr->dynamic) {
		char *str = string_getnext();
                dialer_print_proto_addr(csb->interface, &proto_addr, link, str);
		printf("\n%%Unable to add map, dynamic map currently exists"
		       " for %s", str);
		return;
	    }
	    entry = dialer_string_lookup(addr, dialer_string);
	    if ((dialer_string && (entry || !(addr->entry)->dialer_string)) ||
		(!dialer_string && 
		 !((addr->entry)->dialer_string && (addr->entry)->next))) {
                if (!entry)
                    entry = addr->entry;
		if (!cmd_sense) {
		    dialer_entry_delete(addr, entry);
		    reg_invoke_bump_cache_version(idb, NULL, FALSE);
		    return;
		}
	    } else if (dialer_string && 
			(!entry && (addr->entry)->dialer_string)) {
		if (cmd_sense) {
                    entry = malloc(sizeof(dialerentrytype));
                    if (entry) {
                        entry->next = NULL;
                        entry->hostname = hostname;
                        entry->class = class;
                        entry->speed = speed ? speed : ISDN_SPEED_64;
                        entry->dialer_string = dialer_string;
                        entry->modem_chat_regexp = modem_script;
                        entry->modem_chat_regcomp = modem_script_comp;
                        entry->system_chat_regexp = system_script;
                        entry->system_chat_regcomp = system_script_comp;
			entry->broadcast = broadcast;
			entry->spc = spc;
			dialer_entry_enter(addr, entry);
		    } else {
                        printf(nomemory);
                        free(addr);
		    }
                } else {
                    char *str = string_getnext();
                    dialer_print_proto_addr(csb->interface, &proto_addr, link, str);
                    printf("\n%%Can't find address map for %s - %s", str,
			   dialer_string);
                }
		return;
	    } else if (!dialer_string && 
		       ((addr->entry)->dialer_string && (addr->entry)->next)) {
		if (cmd_sense) {
		    printf("\n%%Ambiguous command, must specify string");
		} else {
		    dialer_addr_delete(addr);
		    reg_invoke_bump_cache_version(idb, NULL, FALSE);
		}
		return;
	    }
	} else {
            if (cmd_sense) {
                addr = malloc(sizeof(dialermaptype));
                if (addr) {
		    entry = malloc(sizeof(dialerentrytype));
		    if (entry) {
                	address_copy(&addr->proto_addr, &proto_addr);
                        addr->ddb = ddb;
                        addr->link = link;
                        entry->next = NULL;
                        entry->hostname = hostname;
                        entry->class = class;
                        entry->speed = speed ? speed : ISDN_SPEED_64;
                        entry->dialer_string = dialer_string;
                        entry->modem_chat_regexp = modem_script;
                        entry->modem_chat_regcomp = modem_script_comp;
                        entry->system_chat_regexp = system_script;
                        entry->system_chat_regcomp = system_script_comp;
			entry->broadcast = broadcast;
			entry->spc = spc;
                        dialer_addr_enter(addr);
			dialer_entry_enter(addr, entry);
		    } else {
                        printf(nomemory);
			free(addr);
		    }
                } else {
                    printf(nomemory);
		}
            } else {
                char *str = string_getnext();
                dialer_print_proto_addr(csb->interface, &proto_addr, link, str);
                printf("\n%%Can't find address map for %s", str);
            }
	    return;
	}

	entry->broadcast = broadcast;

	if (dialer_string) {
	    if (entry->dialer_string)
	        free(entry->dialer_string);
	    entry->dialer_string = dialer_string;
	}

	if (hostname) {
	    if (entry->hostname)
		free(entry->hostname);
	    entry->hostname = hostname;
	}


	if (class) {
	    if (entry->class)
		free(entry->class);
	    entry->class = class;
	}

	if (modem_script) {
	    if (entry->modem_chat_regexp) {
		free(entry->modem_chat_regexp);
	        free(entry->modem_chat_regcomp);
	    }
	    entry->modem_chat_regexp = modem_script;
	    entry->modem_chat_regcomp = modem_script_comp;
	}

	if (speed)
	    entry->speed = speed;

	if (spc)
	    entry->spc = spc;
        else
	    entry->spc = FALSE;
	
	if (system_script) {
	    if (entry->system_chat_regexp) {
	        free(entry->system_chat_regexp);
		free(entry->system_chat_regcomp);
	    }
	    entry->system_chat_regexp = system_script;
	    entry->system_chat_regcomp = system_script_comp;
	}

	return;
    }

    if (hostname)
      free(hostname);

    if (class)
      free(class);

    if (dialer_string)
      free(dialer_string);

    if (modem_script)
      free(modem_script);

    if (modem_script_comp)
      free(modem_script_comp);

    if (system_script)
      free(system_script);

    if (system_script_comp)
      free(system_script_comp);
}

/*
 * dialer_create_dynamic_map
 * Routine to create a dynamic dialer map if an interface is configured
 * for DDR, and a map to the given address doesn't exist.
 */
void dialer_create_dynamic_map (uint link, hwidbtype *hwidb, addrtype *addr)
{
    dialermaptype *map;
    dialerentrytype *entry;
    dialerdbtype *master_ddb;
    dialerdbtype *ddb;
    int callerID = FALSE;

    if (!is_ddr(hwidb))
        return;

    ddb = get_dialerdb(hwidb);
    master_ddb = get_dialerdb(get_netidb(hwidb));
    map = dialer_addr_lookup(master_ddb, addr, link);
    /*
     * We first check to see if there exists a dialer map for this 
     * link and address pair.  If so, we just bail out.  We only want
     * to create dialer maps if they don't already exist...
     * Or if there is a dialer string, bail out too, since with a
     * dialer string, we know where to send the packets to, see
     * dialer_connected().
     */

    if (map || (ddb && ddb->dialer_string
		&& (!ddb->dialer_string->proto_addr.type)))
	return;
    /*
     * if they didn't authenticate (no remote name), then try
     * to use the callerID (for incoming call number_called is
     * callerID). For outgoing call, there must be a static dialer 
     * map or a dialer string.
     */
    if (ddb->remote_name[0] == '\0') {
	if (ddb->number_called[0] != '\0')
	    callerID = TRUE;
	else
	    return;
    }

    map = malloc(sizeof(dialermaptype));
    if (map) {
	entry = malloc(sizeof(dialerentrytype));
	if (entry) {
	    bcopy(addr, (uchar *)&map->proto_addr, sizeof(addrtype));
	    map->ddb = master_ddb;
	    map->link = link;
	    map->dynamic = TRUE;
	    entry->next = NULL;
	    if (callerID)
		entry->hostname = malloc(sizeof(char) *
					 (strlen(ddb->number_called)+1));
	    else
		entry->hostname = malloc(sizeof(char) *
					 (strlen(ddb->remote_name)+1));

	    if (entry->hostname  == NULL) {
		printf(nomemory);
		entry->hostname = NULL;
	    } else {
		if (callerID)
		    strcpy(entry->hostname, ddb->number_called);
		else
		    strcpy(entry->hostname, ddb->remote_name);
	    }
	    entry->speed = ISDN_SPEED_64;
	    entry->broadcast = TRUE;
	    entry->dialer_string = NULL;
	    entry->modem_chat_regexp = NULL;
	    entry->modem_chat_regcomp = NULL;
	    entry->system_chat_regexp = NULL;
	    entry->system_chat_regcomp = NULL;
	    dialer_addr_enter(map);
	    dialer_entry_enter(map, entry);
	} else {
	    printf(nomemory);
	    free(map);
	}
    } else {
	printf(nomemory);
    }
    }

/*
 * dialer_delete_dynamic_map
 * Routine to delete a dynamic dialer map if it exists.
 */
void dialer_delete_dynamic_map (uint link, hwidbtype *hwidb, addrtype *addr)
{
    dialermaptype *map;
    dialerdbtype *master_ddb;
    dialerdbtype *ddb;

    if (!is_ddr(hwidb))
        return;

    ddb = get_dialerdb(hwidb);

    master_ddb = get_dialerdb(get_netidb(hwidb));
    map = dialer_addr_lookup(master_ddb, addr, link);
    /*
     * We first check to see if there exists a dialer map for this 
     * link and address pair.  If so, we just bail out.  We only want
     * to delete dialer maps that exist...
     */
    if ((map != NULL) && (map->dynamic)) {
	dialer_addr_delete(map);
	reg_invoke_bump_cache_version(hwidb, NULL, FALSE);
    }
}

/*
 * dialer_update_timer
 * Inline to update a timer.
 */
static void dialer_update_timer (sys_timestamp *timer, ulong ticks)
{
    sys_timestamp newtimer;

    TIMER_START(newtimer, ticks);
    if (TIMER_RUNNING(*timer) && TIMER_LATER(*timer, newtimer)) {
        *timer = newtimer;
    }
}

/*
 * dialer_clear_stats
 * Clear dialer statistics
 */
static void dialer_clear_stats (dialer_stat *ds)
{
    if (ds != NULL) {
	ds->num_successes = 0;
	ds->num_failures = 0;
	TIMER_STOP(ds->last_call_time);
	ds->last_call_success = FALSE;
    }
}

/*
 * remove_dialer_string
 * Free a dialer string on the ddb.
 */
static void remove_dialer_string (dialerentrytype *entry)
{
    if (entry) {
	if (entry->dialer_string) {
	    free(entry->dialer_string);
	}
	if (entry->class) {
	    free(entry->class);
	}
	free(entry);
    }
}

/*
 * set_dialer_string
 * Copy dialer string to ddb.
 */
static void set_dialer_string (dialerdbtype *ddb, 
			       char *dialer_string, char *class,
			       boolean add)
{
    dialermaptype *map;
    dialerentrytype *entry, *prev, **entryp;

    map = NULL;
    prev = NULL;
    entry = NULL;
    entryp = NULL;

    /*
     * Search for the dialer string
     */
    if (ddb->dialer_string) {
	entryp = &(ddb->dialer_string->entry);
    }

    while (entryp && (*entryp)) {
	if (!strcmp(dialer_string, (*entryp)->dialer_string)) {
	    break;
	}
	prev = *entryp;
	entryp = &((*entryp)->next);
    }

    if (entryp && (*entryp)) {
	/*
	 * Found the dialer string, add or delete as necessary
	 */
	if (add) {
	    setstring(&(*entryp)->class, class);
	} else {
	    entry = *entryp;
	    (*entryp) = entry->next;
	    if (ddb->dialer_string->entry == NULL) {
		free(ddb->dialer_string);
		ddb->dialer_string = NULL;
	    } else if (ddb->dialer_string->cip == entry) {
		ddb->dialer_string->cip = prev;
	    }
	    remove_dialer_string(entry);
	}
    } else if (add) {
	/*
	 * Failed to find the dialer string so create one
	 */
	entry = malloc_named(sizeof(dialerentrytype), "dialer entry");
	if (entry == NULL) {
	    return;
	}
	setstring(&entry->dialer_string, dialer_string);
	setstring(&entry->class, class);

	if ((entry->dialer_string == NULL) ||
	    (class && *class && entry->class == NULL)) {
	    remove_dialer_string(entry);
	} else {
	    entry->speed = ISDN_SPEED_64;
	    entry->broadcast = TRUE;
	    entry->spc = FALSE;

	    /*
	     * Check for the presence of an existing map, if there is
	     * none then create one.
	     */
	    if (ddb->dialer_string) {
		(*entryp) = entry;
	    } else {
		map = malloc_named(sizeof(dialermaptype), "dialer map");
		if (map == NULL) {
		    remove_dialer_string(entry);
		} else {
		    map->entry = entry;
		    map->ddb = ddb;
		    map->link = LINK_ALL;
		    ddb->dialer_string = map;
		}
	    }
	}
    }
}

/*
 * copy_priority_fields
 * This routine copies priority queuing specific fields from idb1 to idb2.  
 * This copies information which must be propogated to other idbs when
 * using hunt groups.  
 */
void copy_priority_fields (dialerdbtype *ddb1, dialerdbtype *ddb2)
{
    int i, queueno;
    hwidbtype *idb1, *idb2;
    dialerdbtype *tempddb;

    idb1 = ddb1->ownidb;
    idb2 = ddb2->ownidb;

     /*
      * propagate (potential) queue length change to new idb.
      * note that this is not strictly a priority queueing feature, but
      * it _certainly_ isn't a hunt group specific field (i.e.
      * copy_hunt_fields()), so put it here instead.
      */
     idb2->holdqueue_out = idb1->holdqueue_out;

    /*
     * priority queuing is unavailable when PPP uses reliable-link (LAPB)
     */
    if(idb2->ppp_lapb)
        return;

    /*
     * Don't use any macros with idb2 values - they might not be set yet
     * Use idb1 values instead.
     */
    queueno = DEFAULT_QUEUE(idb1);
    idb2->outputq[queueno].maximum = idb1->outputq[queueno].maximum;
    idb2->input_defqcount = idb1->input_defqcount;
    /*
     * Check whether we need to add, remove or change a WFQ.  Has
     * queuing algorithm changed, or if we're configured for WFQ, have
     * WFQ parameters changed?
     */
    if ((idb2->queue_algorithm != idb1->queue_algorithm) ||
	(((QUEUE_FAIR | QUEUE_FAIR_DEF) & idb2->queue_algorithm) &&
	!compare_fair_queue(idb1,idb2))) {
	delete_fair_queue(idb2);
	idb2->queue_algorithm = idb1->queue_algorithm;
	if ((QUEUE_FAIR | QUEUE_FAIR_DEF ) & idb2->queue_algorithm)
	    build_fair_queue_head(idb2, idb1->priority_list,
				  idb1->fair_queue_head->fq_hash_mask + 1,
				  idb1->fair_queue_head->fq_queue_count -
				  idb1->fair_queue_head->fq_reserved_queue_start); 
	else
	    
	    set_default_queueing(idb2);
	idb2->old_queue_algorithm = idb2->queue_algorithm;
    }
    idb2->priority_list = idb1->priority_list;
    /*
     *  Do not change the oqueue vector on a multilink bundle interface
     */
    if (!idb2->firstsw->is_bundle_interface)
	idb2->oqueue = idb1->oqueue;
    idb2->oqueue_dequeue = idb1->oqueue_dequeue;
    idb2->custom_queuenum = idb1->custom_queuenum;
    for (i = 0; i < PRIORITY_MAXLIST; i++)
	idb2->custqremain[i] = idb1->custqremain[i];
    if (HUNTGROUP_LEADER(ddb2)) {
       FOR_ALL_HUNTDDBS_ON_MASTER(ddb2, tempddb)
	 copy_priority_fields(ddb1, tempddb);
    }
}

/*
 * copy_hunt_fields
 * This routine copies hunt group specific fields from idb1 to idb2.  
 * This copies information which must be propagated to other idbs when
 * using hunt groups.  This combined with copy_dialer_fields should copy
 * all the necessary information.
 */
void copy_hunt_fields (dialerdbtype *ddb1, dialerdbtype *ddb2)
{
    hwidbtype *idb1, *idb2;
    dialerdbtype *tempddb;
    thcdbtype *thcdb;

    idb1 = ddb1->ownidb;
    idb2 = ddb2->ownidb;
    dialer_set_member_encap(NULL, idb2, idb1->enctype);
    if (!idb2->serial_pulseinterval)
        idb2->serial_pulseinterval = idb1->serial_pulseinterval;
    idb2->datarate_interval = idb1->datarate_interval;
    idb2->pak_backing_store = idb1->pak_backing_store;

    if (IS_ENCRYPTION(idb1)) {
        reg_invoke_crypto_dialer_propagate_map(idb1, idb2);
    }

    /*
     * Copy CDP 'enabled' flag to all members of the dialer
     * pool. This will prevent CDP from gratuitously keeping a channel
     * up, even though the  CDP 'enabled' flag has been turned off
     * at the "leader" interface.
     */
    idb2->firstsw->cdp_enabled = idb1->firstsw->cdp_enabled;

    if (idb2->maxmtu != idb1->maxmtu) {
	if_change_mtu(idb2->firstsw, idb1->maxmtu);
    }
    reg_invoke_media_copy_encap_fields(idb1->enctype, idb1, idb2);

    /*
     * Deal with TCP header compression...
     * While calling a registry may be more appropriate here, it wasn't
     * clear how to do this cleanly.  We need to copy the thcdb->flags
     * from one to another, and so we needed to know about thcdb. The
     * free() calls probably could have been a registry too.  
     * ip_header_compression_set is a good candidate, but it requires that
     * we know if we are passive or not.
     * Bottom line, do it here for now as frame relay does.
     */
    if (idb1->tcp_compress && !idb2->tcp_compress) {
	thcdb = malloc_named(sizeof(thcdbtype), "TCP thc");
	if (thcdb) {
	    reg_invoke_hdr_compress_init(LINK_IP, thcdb, DEFAULT_THC_STATES);
	    thcdb->flags = idb1->tcp_compress->flags;
	    idb2->tcp_compress = thcdb;
	}
    } else if (!idb1->tcp_compress && idb2->tcp_compress) {
	thcdb = idb2->tcp_compress;
	if (thcdb) {
	    idb2->tcp_compress = NULL;
	    if (thcdb->tstate) {
		free(thcdb->tstate);
	    }
	    if (thcdb->rstate) {
		free(thcdb->rstate);
	    }
	    free(thcdb);
	}
    }
    if (HUNTGROUP_LEADER(ddb2)) {
       FOR_ALL_HUNTDDBS_ON_MASTER(ddb2, tempddb)
	 copy_hunt_fields(ddb1, tempddb);
    }
}

/*
 * copy_dialer_fields
 * This routine copies dialer specific fields from idb1 to idb2.  
 */
void copy_dialer_fields (dialerdbtype *ddb1, dialerdbtype *ddb2)
{
    dialerdbtype *tempddb;

    if (ddb2->dialer == DIALER_NONE) {
	ddb2->dialer = (ddb2->ownidb->status & IDB_ASYNC) ? 
				DIALER_INBAND_ASYNC : DIALER_INBAND_SYNC;
    }

    /*
     * Copy over timers.
     */
    ddb2->dialer_idle_ticks = ddb1->dialer_idle_ticks;
    dialer_update_timer(&ddb2->dialer_idle_timer, ddb2->dialer_idle_ticks);
    ddb2->dialer_fast_idle_ticks = ddb1->dialer_fast_idle_ticks;
    dialer_update_timer(&ddb2->dialer_fast_idle_timer, 
			ddb2->dialer_fast_idle_ticks);
    ddb2->dialer_enable_ticks = ddb1->dialer_enable_ticks;
    ddb2->dialer_holdq_ticks = ddb1->dialer_holdq_ticks;
    ddb2->callback_secure = ddb1->callback_secure;
    dialer_update_timer(&ddb2->dialer_enable_timer, 
			ddb2->dialer_enable_ticks);
    ddb2->dialer_carrier_ticks = ddb1->dialer_carrier_ticks;
    dialer_update_timer(&ddb2->dialer_carrier_timer, 
			ddb2->dialer_carrier_ticks);
    if (ddb1->dialer_string != NULL)
      set_dialer_string(ddb2, ddb1->dialer_string->entry->dialer_string,
			NULL, 1);
    else
      ddb2->dialer_string = NULL;

    /*
     * Warning: the following test is an exception.  It should not be used
     * in this module, but alleviates here the need for a registry.
     * B channels can not be assigned a priority individually, so copy
     * the priority configured on the D channel.
     * Other dialers don't copy the priority from the rotary group leader
     * to the members, so they each can have a different priority.
     */
    if (ddb1->dialer == DIALER_ISDN) {
	ddb2->dialer_priority = ddb1->dialer_priority;
    }

    if (HUNTGROUP_LEADER(ddb2))
    	ddb2->dialer_load_threshold = ddb1->dialer_load_threshold;
    ddb2->thresh_direction = ddb1->thresh_direction;
    ddb2->dialer_holdmax = ddb1->dialer_holdmax;

    if (HUNTGROUP_LEADER(ddb2)) {
       FOR_ALL_HUNTDDBS_ON_MASTER(ddb2, tempddb)
	 copy_dialer_fields(ddb1, tempddb);
    }
}

/*
 * reset_dialer_fields
 */
void reset_dialer_fields (dialerdbtype *ddb)
{
    if (ddb->dialer_holdQ)
	dialer_holdq_discard_queue(&ddb->dialer_holdQ,"reset dialer fields");

    ddb->dialer = DIALER_NONE;
    ddb->dialer_parity = DIALER_INBAND_NOPARITY;
    ddb->dialer_idle_ticks = DIALER_IDLE_TIME;
    ddb->dialer_fast_idle_ticks = DIALER_FAST_IDLE_TIME;
    ddb->dialer_enable_ticks = DIALER_ENABLE_TIME;
    ddb->dialer_holdq_ticks = DIALER_HOLDQ_TIME + ddb->dialer_enable_ticks;;
    ddb->dialer_carrier_ticks = DIALER_CARRIER_TIME;
    ddb->callback_secure = FALSE;
    ddb->callback_type = CALLBACK_NONE;
    ddb->fastrollover_ticks = ISDN_FASTROLLOVER_DELAY_DEF;
    ddb->disconnect_reason = NO_DISCONNECT;
    ddb->conn_group = NULL;
    ddb->thresh_direction = DIALER_THRESH_OUTBOUND;
    TIMER_STOP(ddb->dialer_idle_timer);
    TIMER_STOP(ddb->dialer_fast_idle_timer);
    TIMER_STOP(ddb->dialer_enable_timer);
    TIMER_STOP(ddb->dialer_carrier_timer);
    TIMER_STOP(ddb->dialer_load_timer);
    set_dialer_state(ddb, DIALER_READY);
    ddb->dialer_placed_call = FALSE;
    ddb->dialer_list = 0;
    ddb->dialer_load_threshold = 0;
    ddb->dialer_priority = 0;
    ddb->dialer_call_group = NULL;
    ddb->dialer_holdmax = 0;
    ddb->dialer_stats = NULL;
    ddb->dialer_num_call = 0;
    ddb->dialer_call = NULL;
    ddb->dialer_max_call = DIALER_MAX_P2P_CALLS;
    dialer_clear_stats(&ddb->dialer_dtr_stats);
}

/*
 * default_set_dialer
 * Perform encaps specific actions when an interface is configured for DDR.
 */
void default_set_dialer (dialerdbtype *ddb, boolean setup)
{
    if (setup) {
	ddb->dialer_max_call = DIALER_MAX_P2P_CALLS;
    }
}

/*
 * dialer_set_huntidb
 * Set netcfg_idb vector to point to the rotary group leader.  Deal with
 * rotaries of rotaries.
 */
static void dialer_set_huntidb (
    dialerdbtype *ddb,
    dialerdbtype *netddb,
    boolean add)
{
    hwidbtype *idb;
    dialerdbtype *tempddb;

    ddb->netcfg_ddb = netddb;
    ddb->netcfg_idb = netddb->ownidb;
    idb = ddb->ownidb;
    reg_invoke_dialer_setup(idb->enctype, ddb, add);

    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	dialer_set_huntidb(tempddb, netddb, add);
    }
}

/*
 * get_pak_netidb
 * return dialer idb packet was originaly sent to.
 */
idbtype *get_pak_netidb (paktype *pak)
{
    if (is_ddr(pak->if_output->hwptr)) {
	return(pak->dialer_output);
    }
    return(pak->if_output);
}

/*
 * init_dialer_idb
 * Initialize the dialer specific fields of the given idb.
 */
dialerdbtype *init_dialer_idb (hwidbtype *idb)
{
    dialerdbtype *ddb;
    dialergrouptype *grptr;

    /*
     * First allocate dialer descriptor block structure.
     */
    ddb = malloc_named(sizeof(dialerdbtype), "Dialer DB");
    if (ddb == NULL) {
	return(NULL);
    }

    /*
     * Create one more dialergroup structure and add it to the free list.
     */

    grptr = malloc_named(sizeof(dialergrouptype), "Dialer bundle");
    if (grptr == NULL) {
	free(ddb);
	return(NULL);
    } else {
	grptr->next = dialergroup_free_list;
	dialergroup_free_list = grptr;
    }

    idb->dialerdb = ddb;
    ddb->ownidb = idb;

    idb->span_bridge_process_force |= SPAN_BR_PROC_DIALER;

    /*
     * Then reset all the fields.
     */
    reset_dialer_fields(ddb);
    ieee_copy(default_mac_addr, idb->hardware);
    ieee_copy(default_mac_addr, idb->bia);
    idb->mci_testptr = NULL;
    ddb->netcfg_ddb = ddb;
    ddb->netcfg_idb = idb;
    ddb->dialer_num_link = 1;
    ddb->dialer_encap_ptr = NULL;
    ddb->hunt_group_master = NULL;
    ddb->hunt_group_first = NULL;
    ddb->hunt_group_next = NULL;
    ddb->dialer_spoofing_int = FALSE;
    ddb->dialer_spoofing_prot = FALSE;
    ddb->hunt_group_lead = FALSE;
    set_dialer_state(ddb, DIALER_READY);
    ddb->dialer_fsm = dialer_fsm_idle;
    ddb->dialer_bound = FALSE;
    ddb->dialer_binding = FALSE;
    ddb->rotary_group = -1;

    return(ddb);
}

/*
 * dialer_free_struct
 * Free dialer structures.
 */
static void dialer_free_struct (dialerdbtype *ddb)
{
    dialer_call_tab *tmpcall, *tmpcall1;

    if (ddb->dialer_string != NULL) {
	free(ddb->dialer_string->entry->dialer_string);
	free(ddb->dialer_string->entry);
	free(ddb->dialer_string);
	ddb->dialer_string = NULL;
    }

    if (ddb->dialer_profile != NULL) {
	free(ddb->dialer_profile);
	ddb->dialer_profile = NULL;
	ddb->dialer = DIALER_NONE;
    }

    if (ddb->dialer_call != NULL) {
        tmpcall = ddb->dialer_call;
        while (tmpcall != NULL) {
            tmpcall1 = tmpcall->next;
            free(tmpcall);
            tmpcall = tmpcall1;
        }
	ddb->dialer_call = NULL;
    }
}

/*
 * dialer_release
 * Remove dialerdb structure on idb.
 */
static void dialer_release (hwidbtype *idb)
{
    dialergrouptype *grptr;

    free(idb->dialerdb);
    idb->dialerdb = NULL;

    /*
     * Remove one element from the free dialergroup list.
     */
    if (dialergroup_free_list != NULL) {
	grptr = dialergroup_free_list;
	dialergroup_free_list = grptr->next;
	free(grptr);
    }
}

/*
 * dialer_pri_shutdown
 * This PRI interface is going down. Time to cleanup.
 */
void dialer_pri_shutdown (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb == NULL)
	return;

    /*
     * If this PRI is a member of a rotary group, unlink it first.
     */
    if (HUNTGROUP_MEMBER(ddb)) {
	add_to_hunt_group(ddb, idb->unit, FALSE, NULL);
    }
    dialer_goingdown(idb);
    dialer_clear_map(ddb);
    dialer_member_adjust_encap_vectors(ddb, FALSE);
    dialer_free_struct(ddb);
    /*
     * Set the reset vector back to a deconfigured
     * value. This avoids a spurious access on
     * pri deconfig. CSCdi37866
     */
    idb->reset = (reset_t) return_nothing;
}

/*
 * dialer_member_adjust_encap_vectors
 * When adding or deleting a member from a hunt group, adjust the
 * idb vectors appropriately.
 */
void dialer_member_adjust_encap_vectors (dialerdbtype *ddb, boolean adding)
{
    hwidbtype *hwidb;

    hwidb = ddb->ownidb;

    if (adding) {
	/*
	 * If vectors have already been adjusted, exit now.
	 */
	if (hwidb->vencap == dialer_vencapsulate)
	    return;

	/*
	 * Save current values
	 */
	ddb->dialer_save_iqueue = hwidb->iqueue;
	ddb->dialer_save_vencap = hwidb->vencap;
	ddb->fs_type = hwidb->fast_switch_type;

	/*
	 * Set new functions for dialer use
	 */
	hwidb->vencap = dialer_vencapsulate;
	hwidb->iqueue = dialer_raw_enqueue;
	hwidb->fast_switch_type = FS_DIALER;

/* This may be needed for dialer profiles. 3/29/96
     if (hwidb->reset != dialer_reset) {
         ddb->dialer_save_reset = hwidb->reset;
         hwidb->reset = dialer_reset;
     }
*/
    } else {
	if (hwidb->vencap != dialer_vencapsulate)
	    return;
	/*
	 * Restore saved values
	 */
	hwidb->iqueue = ddb->dialer_save_iqueue;
	hwidb->vencap = ddb->dialer_save_vencap;
	hwidb->fast_switch_type = ddb->fs_type;
/* This may be needed for dialer profiles. 3/29/96
     if (hwidb->reset == dialer_reset) {
          hwidb->reset = ddb->dialer_save_reset;
      }
*/
    }
}

/*
 * dialer_adjust_encap_vectors
 * Adjust the idb vectors after the encapsulation changed.
 */
void dialer_adjust_encap_vectors (hwidbtype *hwidb, int enctype)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(hwidb);
    if (ddb != NULL) {
	dialer_member_adjust_encap_vectors(ddb, TRUE);
    }
}

/*
 * add_to_hunt_group
 * Add/delete an idb to/from a dialer hunt group
 */
void add_to_hunt_group (dialerdbtype *ddb, int group, boolean sense,
			hwidbtype *hunt_add_idb)
{
    hwidbtype *idb, *hunt_idb;
    dialerdbtype *hunt_ddb, *tempddb;
    idbtype *newidb;
    list_element *l_elt;
    int status;
    
    /*
     * If "no dialer rotary-group", delete it.
     * We haven't fixed up any idb vectors at this point.
     */
    if (!sense) {
        hunt_ddb = ddb->hunt_group_master; /* Get master pointer */
	if (!hunt_ddb)
	    return;
        if (hunt_ddb->hunt_group_first == ddb)
	    hunt_ddb->hunt_group_first = ddb->hunt_group_next;
	else {
	  FOR_ALL_HUNTDDBS_ON_MASTER(hunt_ddb, tempddb)
	    if (tempddb->hunt_group_next == ddb) { /* Found link to this? */
	      tempddb->hunt_group_next = ddb->hunt_group_next;
	      break;
	   }
        }
	ddb->hunt_group_master = NULL; /* Forget master pointer */
	ddb->hunt_group_next = NULL;   /* And next pointer */
        if (ddb->ownidb->type != IDBTYPE_VACCESS) {
	   hunt_ddb->dialer_num_call -= ddb->dialer_num_call;
           hunt_ddb->dialer_num_link -= ddb->dialer_num_link;
        }
	reg_invoke_dialer_clear_group(ddb->dialer, ddb, NULL);
	dialer_set_huntidb(ddb, ddb, FALSE);   /* set new network idb */
	/*
	 * dialer_disconnect_group might remove ddb and so ddb must not
 	 * be referenced after this call.
	 */
	dialer_disconnect_group(ddb);  /* disconnect int. or all members */
	return;
    }

    /*
     *  If we are configuring the same rotary-group as before
     *    return to avoid disconnecting unnecessarily
     */

    if (HUNTGROUP_MEMBER(ddb)) {
        if (ddb->hunt_group_master->ownidb->unit == group) {
            return;
        }
    }

    /*
     *  Check if huntgroup idb was passed in
     */

    if (!hunt_add_idb) {

	/*
	 * First, walk down the list of idbs looking for a dialer idb that
	 * has the same unit number as the hunt group desired.  If not found,
	 * then create it.
	 */
     FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_SERIAL, l_elt, hunt_idb) {
        if ((hunt_idb->unit == group) && (hunt_idb->type == IDBTYPE_DIALER))
            break;
     }
	if (!hunt_idb) {
	    newidb = dialer_create_master_idb(group);
	    hunt_idb = hwidb_or_null(newidb);
	    if (!hunt_idb)
		return;		/* Error creating the group */
	}
    } else {
	/*
	 *  huntgroup idb was passed in.  Make sure it is a hunt group leader
	 */
	if (HUNTGROUP_LEADER(hunt_add_idb->dialerdb)) {
	    hunt_idb = hunt_add_idb;
	} else {
	    return;
	}
    }

    hunt_ddb = get_dialerdb(hunt_idb);

    if (IS_PROFILE(hunt_ddb))
      return;

    /*
     * If this idb already is in a hunt group, remove it from that group
     * before adding it to another group.  If not just disconnect all
     * existing connections.
     */
    if (HUNTGROUP_MEMBER(ddb))
	add_to_hunt_group(ddb, ddb->hunt_group_master->ownidb->unit, FALSE,
			  NULL);
    else
	dialer_disconnect_group(ddb);

    /*
     * Now add this to the group
     */
    ddb->hunt_group_master = hunt_ddb; /* Set master pointer */
    ddb->hunt_group_next = hunt_ddb->hunt_group_first; /* Link list to entry */
    hunt_ddb->hunt_group_first = ddb;  /* And make this the head of the list */
    if (ddb->ownidb->type != IDBTYPE_VACCESS) {
       hunt_ddb->dialer_num_call += ddb->dialer_num_call;
       hunt_ddb->dialer_num_link += ddb->dialer_num_link;
    }

    /*
     * Set up the dialer fields
     */
    copy_dialer_fields(hunt_ddb, ddb);
    copy_hunt_fields(hunt_ddb, ddb);
    status = raise_interrupt_level(NETS_DISABLE);
    copy_priority_fields(hunt_ddb, ddb);
    reg_invoke_media_fix_serial_vectors(ET_LAPB, hunt_idb);
    reset_interrupt_level(status);
    dialer_set_huntidb(ddb, hunt_ddb, TRUE);

    /*
     * If a serial interface, turn spoofing off and change state to down.
     * Leave rotary group leads alone.
     */
    if (!HUNTGROUP_LEADER(ddb) && IS_SPOOFING(ddb)) {
		idb = ddb->ownidb;
	if (IS_SPOOFING_INT(ddb) && (idb->state == IDBS_UP)) {
	    idb->state = IDBS_DOWN;
	}
	ddb->dialer_spoofing_int = FALSE;
	ddb->dialer_spoofing_prot = FALSE;
	set_dialer_state(ddb, DIALER_READY);
    }
}

/*
 * dialer_remove - Set a line to not have an attached dialer
 */
void dialer_remove (hwidbtype *hwidb)
{
    dialerdbtype *ddb;

    /*
     * If we have never been initialized, just return
     */
    ddb = get_dialerdb(hwidb);
    if (ddb == NULL) 
        return;

    /*
     * Don't remove dialers on rotary group members.
     */
    if (HUNTGROUP_MEMBER(ddb)) {
	printf("\n%%Remove interface from rotary group first");
	return;
    }

    /*
     * DTR dialers need to bring the line up (raise DTR) before removal
     */
    if (DTR_DIALER(ddb)) {
	ddb->dialer = DIALER_NONE;		/* so that reset works */
	(*(hwidb->reset))(hwidb);
    }

    /*
     * Unlink members of a rotary group leader that is being removed.
     */
    if (HUNTGROUP_LEADER(ddb)) {
	while (ddb->hunt_group_first != NULL) {
	    add_to_hunt_group(ddb->hunt_group_first, hwidb->unit, FALSE, NULL);
	}
    }

    if (ddb->dialer_holdQ != NULL) {
	dialer_holdq_discard_queue(&ddb->dialer_holdQ, "Dialer removal");
    }
    ddb->dialer_holdmax = 0;

    /*
     * Erase dialer configuration.
     */
    dialer_clear_map(ddb);
    dialer_free_struct(ddb);
    reset_dialer_fields(ddb);
    hwidb->serial_pulseinterval = ddb->dialer_save_pulseinterval;
    
    /*
     * Dialer interfaces are done now.  Physical interfaces need to remove all
     * traces of DDR configuration.
     */
    if (hwidb->type != IDBTYPE_DIALER) {
	ddb->netcfg_ddb = NULL;
	ddb->netcfg_idb = NULL;
	hwidb->mci_testptr = hwidb->mci_regptr;
        hwidb->span_bridge_process_force &= ~SPAN_BR_PROC_DIALER;

	/*
	 * Fixup our idb vectors
	 */
	dialer_member_adjust_encap_vectors(ddb, FALSE);

	/*
	 * if we were spoofing, turn it off.  If we were spoofing and
	 * we were pretending that the interface was up, set it to down.
	 */
	if (IS_SPOOFING(ddb)) {
	    if (IS_SPOOFING_INT(ddb) && (hwidb->state == IDBS_UP))
		hwidb->state = IDBS_DOWN;
  
            ddb->dialer_spoofing_int = FALSE;
	    ddb->dialer_spoofing_prot = FALSE;
	    route_adjust(hwidb);
	}
	dialer_release(hwidb);		/* Remove dialerdb structure */
	reg_invoke_fast_setup(hwidb);	/* Restore fastswitching */
    }
}

/*
 * dialer_interface_command_ok
 *
 * apply any validity tests Dialer may need for interface command
 */
boolean dialer_interface_command_ok (parseinfo *csb)
{
    hwidbtype *hwidb;
    dialerdbtype *ddb;

    hwidb = hwidb_or_null(GETOBJ(idb,1));

    /*
     * the caller ensures hwidb is not NULL
     */

    /*
     * don't allow deletion if we have rotary group members
     */
    if ((!csb->nvgen) && (!csb->sense)) {
	ddb = get_dialerdb(hwidb);
	if (ddb->hunt_group_first) {
	    printf("\n%% Remove all dialer rotary group members first");
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * dialer_is_backup_interface
 * Check to see if this interface is backing up any other interface
 */
static boolean dialer_is_backup_interface (dialerdbtype *ddb)
{
    hwidbtype *hwidb, *d_hwidb;
    idbtype *swidb;
    list_element *l_elt;

    d_hwidb = ddb->ownidb;
    /*
     * Search through all interfaces to see if anyone 
     * is using us as a backup interface
     */
    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_SERIAL, l_elt, hwidb) {

	if (hwidb->secondary && 
	    /*
	     * We have to do this ugly hack because some commands
	     * completely change the hwidb for instance: physical layer async
	     */
           !strcmp(hwidb->secondary->hw_namestring, d_hwidb->hw_namestring)) {
	    hwidb->secondary = d_hwidb;
	    d_hwidb->standby = TRUE;
	    return TRUE;
	}

	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {

	    if (swidb->backup && 
	    /*
	     * We have to do this ugly hack because some commands
	     * completely change the hwidb for instance: physical layer async
	     */
	      !strcmp(swidb->backup->hw_namestring, d_hwidb->hw_namestring)) {
		swidb->backup = d_hwidb;
		d_hwidb->standby = TRUE;
		return TRUE;
	    }
	}
    }

    return FALSE;
}

/*
 * dialer_command_inband
 * modularize the dialer in-band code into one function so it
 *  can be called from outside the dialer.
 */

void dialer_command_inband (boolean sense, dialerdbtype *ddb, int parity)
{
    hwidbtype *idb;

    idb = ddb->ownidb;

    if (sense) {
	/*
	 *  If we have already done this, don't do it again
	 *    just because someone reconfigured the same thing.
	 */
	
	if ((ddb->dialer == DIALER_INBAND_ASYNC) ||
	    (ddb->dialer == DIALER_INBAND_SYNC))
	    return;

	/*
	 * We won't configure 'dialer in-band' if the interface has
	 * a dialer profile configured.
	 */
	if (ddb->dialer_profile != NULL) {
	    printf("\n%%Remove the dialer profile first");
	    return;
	}
	/*
	 * If a DTR dialer is configured, remove it (raise DTR) before
	 * configuring an in band dialer.
	 */
	if (DTR_DIALER(ddb)) {
	    ddb->dialer_spoofing_prot = FALSE;
	    ddb->dialer_spoofing_int = FALSE;
	    ddb->dialer = DIALER_NONE;	/* so that reset works */
	    (*(idb->reset))(idb);
	}
	
	ddb->number_called[0] = '\0'; /* Ensure no garbage */
	ddb->remote_name[0] = '\0';
	ddb->dialer = (idb->status & IDB_ASYNC) ? DIALER_INBAND_ASYNC :
	    DIALER_INBAND_SYNC;
	ddb->dialer_parity = parity;
	ddb->dialer_save_pulseinterval = idb->serial_pulseinterval;
	if (!idb->serial_pulseinterval) /* If no pulse timer */
	    idb->serial_pulseinterval = 1 * ONESEC; /* Set to one second */
	
	/*
	 * Fixup idb vectors for dialer use
	 */
	dialer_member_adjust_encap_vectors(ddb, TRUE);
	
	/*
	 * If a dialer is defined and the state is IDBS_DOWN, then we want
	 * to start spoofing.  This means to set the state to IDBS_UP, set
	 * the spoofing flag, and forge ahead.
	 */
	if (idb->state == IDBS_DOWN && !HUNTGROUP_MEMBER(ddb)) {
	    set_dialer_state(ddb, DIALER_WAITING);
	    /*
	     * HACK: Check at this point if any other interface is
	     * trying to use this interface as a backup if so
	     * setup both the Primary & Secondary for backup.
	     */
	    if (dialer_is_backup_interface(ddb)) {
		/*
		 * Shut the secondary interface, move it to standby mode
		 */
		shutdown_going_down(ddb->ownidb);
	    } else {
		TIMER_START(ddb->dialer_enable_timer, 
			    ddb->dialer_enable_ticks);
	    }
	}
	route_adjust(idb);
	reg_invoke_fast_setup(idb);
    } else {			/* shut down dialer */
	dialer_remove(idb);
	return;
    }
    
}


/*
 * get_dialer_minsum
 * Compute the sum of the minimum number of calls on all the pools a given
 * interface is a member of.
 */
ushort get_dialer_minsum (dialerdbtype *ddb)
{
    dialerpooltype *pool;
    ushort minsum;

    minsum = 0;

    FOR_ALL_POOLS_IN_INTERFACE(ddb, pool) {
	minsum += pool->min_link;
    }

    return(minsum);
}

/*
 * get_dialer_free
 * Compute the number of available B channels used 
 * exempting the reserved channels
 */
ushort get_dialer_used (dialerdbtype *ddb)
{
    dialerpooltype *pool;
    short used;

    used = 0;

    FOR_ALL_POOLS_IN_INTERFACE(ddb, pool) 
	used += (pool->dial_out > pool->min_link ? 
		 pool->dial_out - pool->min_link : 0) + pool->dial_in;

    return(used);
}

/*
 * dialer_group_async_add_pool
 * Add the pool on the Group Async without linking it any of the 
 * profile or member pools, just enough so that the "dialer pool-member"
 * shows up in the config since this will be applied in its entirety on
 * the Async Interfaces covered by this Group Async.
 */
static boolean dialer_group_async_add_pool (dialerdbtype *ddb, 
					    int number,
					    int priority, 
					    int min_link, 
					    int max_link,
					    boolean add) 
{
    dialerpooltype *pool, **prev_pool;

    /*
     * Is there a dialer pool configured ?
     */

    prev_pool = &ddb->dialer_pool; 
    
    FOR_ALL_POOLS_IN_INTERFACE(ddb, pool) { 
	if (pool->pool_number == number) 
	    break; 
	prev_pool = &(pool->next_in_interface); 
    }


    if (add) {				/* dialer add pool */

	if (pool != NULL) {		/* pool already present */

	    pool->pool_priority = priority;
	    pool->min_link = min_link;
	    pool->max_link = max_link;

	} else {			/* pool not present, create one */

	    pool = malloc_named(sizeof(dialerpooltype), "Dialer Pool Member");
	    if (pool == NULL)
		return FALSE;

	    pool->pool_number = number;
	    pool->pool_priority = priority;
	    pool->min_link = min_link;
	    pool->max_link = max_link;
	    pool->next_in_interface = *prev_pool;
	    *prev_pool = pool;
	}

    } else {				/* dialer remove pool */

	if (pool != NULL) {		/* pool present, delete it */
	    *prev_pool = pool->next_in_interface;
	    free(pool);

		/* pool not present, hence ignore this */

	}

    }

    return TRUE;

}

/*
 * dialer_remove_pool
 * Remove given interface from pool.
 */
void dialer_remove_pool (dialerdbtype *ddb, uchar number)
{
    dialerpoolheadtype *head, **prev_head;
    dialerpooltype *interface_pool, *head_pool, **prev_pool;
    dialerdbtype *profileddb, *memberddb;

    /*
     * Is this interface a Group Async ? 
     */
    if (ddb->ownidb->type == IDBTYPE_ASYNC_GROUP) {
	dialer_group_async_add_pool(ddb,
				    number,
				    0,
				    0,
				    0,
				    FALSE);
	return;
    }

    /*
     * Is there a dialer pool configured ?
     */

    prev_pool = &ddb->dialer_pool;
    FOR_ALL_POOLS_IN_INTERFACE(ddb, interface_pool) {
        if (interface_pool->pool_number == number)
            break;
        prev_pool = &(interface_pool->next_in_interface);
    }

    if (interface_pool != NULL) {	/* dialer pool present */
	/*
	 * Before removing the pool, disconnect all links owned by this pool
	 */
	if (HUNTGROUP_LEADER(ddb)) {
	    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, memberddb) {
		/*
		 * Which pool is this link associated with ?
		 */
		if (DIALER_UP(memberddb) &&
		    (profileddb = memberddb->netcfg_ddb) && 
		    (profileddb->dialer_pool->pool_number == number))
		    dialer_disconnect(memberddb->ownidb);
	    }
	} else {
	    /*
	     * Which pool is this link associated with ?
	     */		
	    if (DIALER_UP(ddb) &&
		(profileddb = ddb->netcfg_ddb) && 
		(profileddb->dialer_pool->pool_number == number))
		dialer_disconnect(ddb->ownidb);
	}
	/*
	 * Unlink the pool from interface list 
	 */
	*prev_pool = interface_pool->next_in_interface;

	/*
	 * Find the head 
	 */

	prev_head = &dialer_pool_head;
        FOR_ALL_HEAD_OF_POOLS(head) {
	    if (head->pool_number == number)
		break;
	    prev_head = &(head->next_head);
	}

	/*
	 * If head for this pool doesn't exist, we're in trouble.
	 */
	if (head == NULL) {
	    errmsg(&msgsym(NULLPTR, DIALER), "dialer_remove_pool");
	    return;
	}

	/*
	 * Set up correct pool.
	 */
	if (IS_PROFILE(ddb)) {
	    prev_pool = &head->first_profile_pool;
	    head->number_of_profile_pools--;
	} else {
	    prev_pool = &head->first_member_pool;
	    head->number_of_member_pools--;
	}

	/*
	 * Find dialer pool in this dialer pool list.
	 */
	for (head_pool = *prev_pool; head_pool; 
	     head_pool = head_pool->next_in_head) {

	    if (head_pool == interface_pool)
		break;
	    prev_pool = &(head_pool->next_in_head);
	}

	/*
	 * Unlink it from head list
	 */
	*prev_pool = head_pool->next_in_head;
	free(head_pool);
	if ((head->first_profile_pool == NULL) &&
	    (head->first_member_pool == NULL)) {
	    *prev_head = head->next_head;
	    free(head);
	}

	if (DIALPOOL_MEMBER(ddb))
	    if (ddb->dialer_pool == NULL)
		ddb->dialer_pool_member = FALSE;
    }

}

/*
 * dialer_add_pool
 * Add given interface to pool.
 */
boolean dialer_add_pool (dialerdbtype *ddb, int number, 
int priority, int min_link, int max_link)
{
    ushort minsum;
    dialerpoolheadtype *head;
    dialerpooltype *pool, **prev_pool;

    if ((number < DIALER_MINPOOL) || (number > DIALER_MAXPOOL))
	return(FALSE);

    /*
     * Is this interface a Group Async ? 
     */
    if (ddb->ownidb->type == IDBTYPE_ASYNC_GROUP) {
	if (dialer_group_async_add_pool(ddb,
					   number,
					   priority,
					   min_link,
					   max_link,
					   TRUE))
	    return TRUE;
	else 
	    return FALSE;
    }

    /*
     * Is there a dialer pool configured ?
     */

    prev_pool = &ddb->dialer_pool; 
    
    FOR_ALL_POOLS_IN_INTERFACE(ddb, pool) { 
	if (pool->pool_number == number) 
	    break; 
	prev_pool = &(pool->next_in_interface); 
    }


    if (pool == NULL) {				/* no dialer pool */
	
	/*
	 * If head of dialer pool doesn't exist, create it now and
	 * add it to the dialer pools head list.
	 */

	FOR_ALL_HEAD_OF_POOLS(head) {
	    if (head->pool_number == number)
		break;
	}

	if (head == NULL) {		/* no head, create it */
	    head = malloc_named(sizeof(dialerpoolheadtype), 
				"Dialer Pool Head");
	    if (head == NULL)
		return(FALSE);
	    head->pool_number = number;
	    head->next_head = dialer_pool_head;
	    dialer_pool_head = head;
	}
	
	/*
	 * Create the pool 
	 */

	pool = malloc_named(sizeof(dialerpooltype), "Dialer Pool Member");
	if (pool == NULL) {
	    /*
	     * couldn't create pool, so free a new head
	     */
	    if ((head->first_profile_pool == NULL) &&
		(head->first_member_pool == NULL)) {
		dialer_pool_head = head->next_head;
		free(head);
	    }
	    return(FALSE);
	}
	pool->pool_head = head;
	pool->ddb = ddb;
	pool->pool_number = number;
	pool->next_in_interface = *prev_pool;

	if (IS_PROFILE(ddb)) {		/* Dialer Profile */
	    pool->next_in_head = head->first_profile_pool;
	    head->first_profile_pool = pool;
	    head->number_of_profile_pools++;
	    if (ddb->dialer_pool != NULL) {
                dialer_remove_pool(ddb, ddb->dialer_pool->pool_number);
                ddb->dialer_pool = NULL;
            }
	    ddb->dialer_pool = pool;

	} else {			/* Physical Interface */
	    pool->next_in_head = head->first_member_pool;
	    head->first_member_pool = pool;
	    head->number_of_member_pools++;
	    *prev_pool = pool;
	}

    }

    /*
     * For dialer pool members, verify that the sum of the minimum will not
     * exceed the capacity of the ISDN link.
     */
    if (!IS_PROFILE(ddb) && ISDN_DIALER(ddb)) {
	
	minsum = get_dialer_minsum(ddb);

	/*
	 * This test should be at the very beginning before you even add
	 * the dialer pool in 
	 */

	if (minsum > ddb->dialer_num_link) {
	    printf("\n%%Minimum specified exceeds number of available links");
	    return(FALSE);
	}
    }

    if (!IS_PROFILE(ddb)) {		/* Physical Interface */
	pool->pool_priority = priority;
	pool->min_link = min_link;
	pool->max_link = max_link;
	ddb->dialer_pool_member = TRUE;
    }

    return(TRUE);

}

/*
 * is_ddr_configured
 * Check for even the slightest trace of Legacy DDR
 */
static boolean is_ddr_configured (dialerdbtype *ddb)
{
    int indx;
    dialermaptype *dmap;

    /*
     * Are there any Dialer Maps configured ?
     */
    for (indx = 0; indx < DIALERHASH; indx++)
	for (dmap = dialertable[indx]; dmap; dmap = dmap->next)
	    if (ddb == dmap->ddb)
		return FALSE;
    /*
     * Is there any Legacy DDR at all ?
     * Except "dialer in-band" and "dialer dtr"
     */
    if ((ddb->dialer_rotor_style != DIALER_ROTOR_PRIORITY) ||
	(ddb->dialer_string != NULL) ||
	(HUNTGROUP_MEMBER(ddb)) ||
	(ddb->dialer_load_threshold) ||
	(ddb->dialer_priority) ||
	(ddb->dialer_holdmax) ||
	(ddb->dialer_call != NULL) ||
	(ddb->dialer_list) ||
	(ddb->callback_secure))
	return FALSE;

    return TRUE;
}

/*
 * dialer_map_configured
 *
 * Is there a dialer map associated with this interface?
 */
static dialermaptype *dialer_map_configured (dialerdbtype *ddb)
{
    int index;
    dialermaptype *map;
    
    for (index = 0; index < DIALERHASH; index ++) {
	for (map = dialertable[index]; map; map = map->next) {
	    if ((ddb == map->ddb) && (!map->dynamic)) {
		return (map);
	    }
	}
    }
    return (NULL);
}

/*
 * dialer_parser_sanity
 *
 * This function does a whole bunch of checks to make sure Legacy DDR &
 * Dialer Profiles are not mixed up. Also, any other applicable checks
 * should be placed in here.
 */
static boolean dialer_parser_sanity (parseinfo *csb, dialerdbtype *ddb)
{
    const char *legacy_error = "Remove Dialer Profile Configuration first";
    const char *profile_error = "Remove Legacy DDR Configuration first";
    const char *r_member = "on a rotary group member";
    const char *dtr_dial = "on a DTR dialer";
    uint command;
    list_element *l_elt ;
    dialerdbtype *currentddb;
    hwidbtype *hwidb;
    char *remote_name;

    command = csb->which;

    /* 
     * Don't allow "dialer remote-name" if the remote-name is already
     * configured in another interface
     */
    if (command == DIALER_REMOTE_NAME) {
	remote_name = GETOBJ(string,1);
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_SERIAL, l_elt, hwidb) {
	    currentddb = get_dialerdb(hwidb);
	    if ((currentddb != NULL) && (IS_PROFILE(currentddb))) {
		if ((currentddb->dialer_profile != NULL) && 
		    (!strcmp(remote_name, currentddb->dialer_profile))) {
		    printf("\n%%Remote-name %s is already"
			   " configured on interface %s.", 
			   remote_name, hwidb->hw_namestring); 
		    return (FALSE);
		}
	    }
	}
    }

    if (command == DIALER_STRING) {
        if (HUNTGROUP_MEMBER(ddb) || DTR_DIALER(ddb) ||
	    dialer_map_configured(ddb)) {
            printf("\n%%Cannot change dialer string %s.",
		HUNTGROUP_MEMBER(ddb) ? r_member :
		DTR_DIALER(ddb) ? dtr_dial : "when dialer map present");
	    return (FALSE);
        }
    } else if (command == DIALER_DTR) {
	if (HUNTGROUP_MEMBER(ddb) || ddb->dialer_string ||
	    dialer_map_configured(ddb)) {
	    printf("\n%%DTR dialing not allowed ");
	    if (HUNTGROUP_MEMBER(ddb)) {
		printf("%s.", r_member);
	    } else {
		printf("with a dialer %s.",
		    ddb->dialer_string ? "string" : "map");
	    }
	    return (FALSE);
	}
    } else if (command == DIALER_MAP) {
        if (HUNTGROUP_MEMBER(ddb) || DTR_DIALER(ddb) || ddb->dialer_string) {
            printf("\n%%Cannot change dialer map %s.",
		HUNTGROUP_MEMBER(ddb) ? r_member :
		DTR_DIALER(ddb) ? dtr_dial : "when dialer string present");
	    return (FALSE);
        }
    }

    if (ddb->ownidb->status & IDB_DIALER) { /* This is a DIALER interface */
	/*
	 * Don't allow a Dialer Map with any trace of Dialer Profiles
	 */
	if ((command == DIALER_MAP) &&
	    /*
	     * Is there a "remote-name" configured ? 
	     */
	    ((IS_PROFILE(ddb) && (ddb->dialer_profile != NULL)) ||
	     /*
	      * Is there a "dialer pool" configured ? 
	      */
	     (ddb->dialer_pool != NULL) ||
	     /*
	      * Is there more than one "dialer string" configured ?
	      */
	     ((ddb->dialer_string) && (ddb->dialer_string->entry->next)))) {
	    printf("\n%%%s", legacy_error);
	    return FALSE;
	}
    
	/*
	 * Don't allow "dialer remote-name" or "dialer pool" 
	 * if there are any Dialer Maps under this interface
	 */
	if ((command == DIALER_REMOTE_NAME) ||
	    (command == DIALER_POOL)) {
	    /*
	     * Are there any Dialer Maps under this interface ?
	     */
	    if (dialer_map_configured(ddb)) {
		printf("\n%%%s", profile_error);
		return (FALSE);
	    }

	}
	
    } else {				/* This is NOT Dialer Interface */
	
	if (command == DIALER_POOL_MEMBER) {
	    /*
	     * This is ugly, but we have detect the slightest trace of
	     * Legacy DDR configuration and this is the only way I know
	     * of 
	     */
	    if (!is_ddr_configured(ddb)) {
		printf("\n%%%s", profile_error);
		return FALSE;
	    }

	} else if ((command != DIALER_INBAND) &&
		   (command != DIALER_DTR)) {
	    if (ddb->dialer_pool != NULL) {
		printf("\n%%%s", legacy_error);
		return FALSE;
	    }

	}

    }
    
    return TRUE;
}

/*
 * dialer_command
 * Used to configure a serial interface for dialer support.
 */
void dialer_command (parseinfo *csb)
{

    hwidbtype *idb;
    idbtype *swidb;
    dialerdbtype *ddb, *tempddb;
    dialer_call_tab *tmpcall;
    dialerpooltype *pool;
    dialermaptype *dmap;
    dialerentrytype *dentry;
    ulong ultemp;
    int parity;

    swidb = csb->interface;
    idb = hwidb_or_null(swidb);
    if (idb == NULL) {
	return;
    }

    if (idb->enctype == ET_NULL) {
	if (!csb->nvgen) {
	    printf("\n%%Encapsulation must be set before using dialer subcommands");
	}
        return;
    }

    ddb = get_dialerdb(idb);
    if (csb->nvgen) {
	if (ddb == NULL) {
	    return;
	}
	if (HUNTGROUP_MEMBER(ddb) &&
	    (csb->which != DIALER_INBAND) &&
	    (csb->which != DIALER_HUNT) &&
	    (csb->which != DIALER_PRIORITY)) { 
	    return;
	}

	switch (csb->which) {
	  case NOPREFIX_DIALER:
	    /* no NV generation */
	    break;

	  case DIALER_INBAND:
	    nv_write((V25BIS_DIALER(ddb) || ASYNC_DIALER(ddb)), csb->nv_command);
            nv_add((ddb->dialer_parity == DIALER_INBAND_ODDPARITY),
		   " odd-parity");
	    break;
	    
	  case DIALER_ROTOR:
	    if (ddb->dialer_rotor_style != DIALER_ROTOR_PRIORITY) {
		/*
		 * this will change if we ever add another command
		 */
		nv_write(TRUE, csb->nv_command);
		nv_add(TRUE, " best");
	    }
	    break;

	  case DIALER_IDLE:
            nv_write((!DIALPOOL_MEMBER(ddb) && 
		      ddb->dialer_idle_ticks != DIALER_IDLE_TIME),
		     "%s %u", csb->nv_command,
		     ddb->dialer_idle_ticks / ONESEC);
	    break;

	  case DIALER_FAST_IDLE:
          nv_write((!DIALPOOL_MEMBER(ddb) && 
                    ddb->dialer_fast_idle_ticks != DIALER_FAST_IDLE_TIME),
		     "%s %u", csb->nv_command,
		     ddb->dialer_fast_idle_ticks / ONESEC);
	    break;

	  case DIALER_REENABLE:
	  nv_write((!DIALPOOL_MEMBER(ddb) && 
		    ddb->dialer_enable_ticks != DIALER_ENABLE_TIME),
		     "%s %u", csb->nv_command,
		     ddb->dialer_enable_ticks / ONESEC);
	    break;

	  case DIALER_CARRIER:
          nv_write((!DIALPOOL_MEMBER(ddb) && 
                    ddb->dialer_carrier_ticks != DIALER_CARRIER_TIME),
		     "%s %u", csb->nv_command,
		     ddb->dialer_carrier_ticks / ONESEC);
	    break;

	  case DIALER_STRING:
	    dentry = NULL;
	    dmap = NULL;
	    dmap = ddb->dialer_string;
	    if (dmap)
	      dentry = dmap->entry;
	    while (dentry != NULL) {
		nv_write(TRUE, "%s %s", csb->nv_command, 
			 dentry->dialer_string);
		nv_add(dentry->class != NULL, " class %s", dentry->class);
		dentry = dentry->next;
	    }
	    break;

	  case DIALER_MAP:
	    nv_dialer_map(ddb);
	    break;

	  case DIALER_HUNT:
	    if (HUNTGROUP_MEMBER(ddb)) {
		nv_write(TRUE, "%s %d", csb->nv_command,
			 ddb->hunt_group_master->ownidb->unit);
	    } else if (idb->type == IDBTYPE_ASYNC_GROUP) {
		if (ddb->rotary_group >= 0)
		    nv_write(TRUE, "%s %d", csb->nv_command,
			     ddb->rotary_group);
	    }
	    break;

	  case DIALER_LOAD:
	    if (ddb->dialer_load_threshold) {
	      nv_write(ddb->dialer_load_threshold, "%s %u",
		       csb->nv_command,
		       ddb->dialer_load_threshold);
	      nv_add((ddb->thresh_direction == DIALER_THRESH_EITHER),
		     " either");
	      nv_add((ddb->thresh_direction == DIALER_THRESH_INBOUND),
		     " inbound");
	    }
	    break;

	  case DIALER_PRIORITY:
	    nv_write(ddb->dialer_priority, "%s %u",
		     csb->nv_command,
		     ddb->dialer_priority);
	    break;

	  case DIALER_DTR:
	    nv_write(DTR_DIALER(ddb), csb->nv_command);
	    break;

	  case DIALER_HOLDQ:
	    if (ddb->dialer_holdmax) {
		nv_write(ddb->dialer_holdmax, "%s %u",
			 csb->nv_command,
			 ddb->dialer_holdmax);
		ultemp = ddb->dialer_holdq_ticks - ddb->dialer_enable_ticks;
		nv_add((ultemp != DIALER_HOLDQ_TIME),
		       " timeout %u", ultemp / ONESEC);
	    }
	    break;

          case DIALER_CALLER:
            tmpcall = ddb->dialer_call;
            /*
             * The additional check here for the presence
             * of and isdn_info structure seperates the
             * dialer caller command from the isdn caller
             * command. They both use the same idb field.
             * The only difference is that dialer caller
             * is only allowed on dialer interfaces. 
             * dialer interfaces do not have an isdn_info
             * structure. Only D-channel interfaces do.
             */
            while (tmpcall != NULL && !idb->isdn_info) {
                nv_write(TRUE, "%s %s", 
                         csb->nv_command, 
                         tmpcall->dialer_call_string);
                tmpcall = tmpcall->next;
            }
            break;

	  case DIALER_CALLBACK_SECURE:
	    nv_write(ddb->callback_secure, "%s", csb->nv_command);
	    break;

	  case DIALER_REMOTE_NAME:
	    nv_write((IS_PROFILE(ddb) && (ddb->dialer_profile != NULL)),
		     "%s %s", csb->nv_command, ddb->dialer_profile);
	    break;

	  case DIALER_POOL:
	    if (ddb->dialer_pool != NULL)
	      nv_write(TRUE, "%s %u", csb->nv_command, 
		       ddb->dialer_pool->pool_number);
	    break;

	  case DIALER_POOL_MEMBER:
	    pool = ddb->dialer_pool;
	    while (pool != NULL) {
		nv_write(TRUE, "%s %d", csb->nv_command, pool->pool_number);
		nv_add(pool->pool_priority, " priority %d",pool->pool_priority);
		nv_add(pool->min_link, " min-link %d", pool->min_link);
		nv_add(pool->max_link, " max-link %d", pool->max_link);
		pool = pool->next_in_interface;
	    }
	    break;

	  case DIALER_MAX_CALL:
	    nv_write(ddb->dialer_max_call != DIALER_MAXCALL,
		     "%s %u", csb->nv_command, ddb->dialer_max_call);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    if ((ddb == NULL) || (ddb->dialer == DIALER_NONE)) {
	if (!csb->sense) {
	    /*
	     * If 'no dialer something' is entered, but DDR is not configured,
	     * silently return.
	     */
	    return;
	} else if (csb->which == DIALER_POOL) {
	    printf("\n%%Configure a \"dialer remote-name\" first");
	    return;
	} else if ((csb->which != DIALER_HUNT) &&
        	   (csb->which != DIALER_INBAND) &&
                   (csb->which != DIALER_DTR) &&
                   (csb->which != DIALER_REMOTE_NAME)) {
	    printf("\n%%Configure a \"dialer in-band\" first");
	    return;
	} 
	/*
	 * The dialer fields of this idb have not been initialized, so 
	 * initialize them now.
	 */
	if (ddb == NULL) {
	  ddb = init_dialer_idb(idb);
          if (ddb == NULL) {
	    printf(nomemory);
	    return;
	  }
	}
    }

    if (!dialer_parser_sanity(csb, ddb))
	/*
	 * Conflicting configurations, hence return
	 */
	return;

    switch (csb->which) {
      case NOPREFIX_DIALER:
        if (!csb->sense)		/* if "no dialer" */
	    dialer_remove(idb);
	return;

      case DIALER_INBAND:
	parity = GETOBJ(int,1);
	dialer_command_inband(csb->sense, ddb, parity);
	if (!csb->sense) {
	    /*
	     *  ddb has been freed by dialer_remove in
	     *    dialer_command_inband.  So return.
	     */
	    return;
	}

	break;

      case DIALER_ROTOR:
	ddb->dialer_rotor_style = GETOBJ(int, 1);
	break;

      case DIALER_IDLE:
        if (csb->sense) {
	    ddb->dialer_idle_ticks = (ulong) GETOBJ(int,1) * ONESEC;
	    dialer_update_timer(&ddb->dialer_idle_timer, 
				ddb->dialer_idle_ticks);
	} else {
	    ddb->dialer_idle_ticks = DIALER_IDLE_TIME;
	}
	break;

      case DIALER_FAST_IDLE:
        if (csb->sense) {
	    ddb->dialer_fast_idle_ticks = (ulong) GETOBJ(int,1) * ONESEC;
	    dialer_update_timer(&ddb->dialer_fast_idle_timer,
				ddb->dialer_fast_idle_ticks);
	} else {
	    ddb->dialer_fast_idle_ticks = DIALER_FAST_IDLE_TIME;
	}
	break;

      case DIALER_REENABLE:
	ultemp = ddb->dialer_holdq_ticks - ddb->dialer_enable_ticks;
        if (csb->sense) {
	    /*
	     * don't allow re-enabling before interface has completed
	     * resetting itself. causes problems/race-conditions.
	     */
	    if (idb->serial_pulseinterval >=
		((ulong) GETOBJ(int,1) * ONESEC)) {
		printf("\n%% Dialer re-enable time must be"
		       " greater than serial pulse time: %u",
		       (int) (idb->serial_pulseinterval/ONESEC));
		break;
	    }
	    ddb->dialer_enable_ticks = (ulong) GETOBJ(int,1) * ONESEC;
	    dialer_update_timer(&ddb->dialer_enable_timer,
				ddb->dialer_enable_ticks);
	} else {
	    ddb->dialer_enable_ticks = DIALER_ENABLE_TIME;
	}
	ddb->dialer_holdq_ticks = ddb->dialer_enable_ticks + ultemp;
	break;

      case DIALER_CARRIER:
        if (csb->sense) {
	    ddb->dialer_carrier_ticks = (ulong) GETOBJ(int,1) * ONESEC;
	    dialer_update_timer(&ddb->dialer_carrier_timer,
				ddb->dialer_carrier_ticks);
	} else {
	    ddb->dialer_carrier_ticks = DIALER_CARRIER_TIME;
	}
	break;

      case DIALER_STRING:
        if (csb->sense) {
	  set_dialer_string(ddb, GETOBJ(string,1), 
			    GETOBJ(string,2), csb->sense);
	} else {
	  reg_invoke_dialer_clear_group(ddb->dialer, 
					ddb, ddb->dialer_string);
	  set_dialer_string(ddb, GETOBJ(string,1), 
			    GETOBJ(string,2), csb->sense);
	}
	reg_invoke_bump_cache_version(idb, NULL, FALSE);
	break;

      case DIALER_MAP:
        dialer_parse_map(csb, ddb);
        break;

      case DIALER_HUNT:
	if (DTR_DIALER(ddb)) {
	    printf("\n%%Cannot add a DTR dialer to dialer rotary group.");
	    return;
	}
	if (idb->type == IDBTYPE_ASYNC_GROUP) {
	    /*
	     * Stash away the dialer group number 
	     */
	    if (csb->sense)
		ddb->rotary_group = GETOBJ(int,1);
	    else
		ddb->rotary_group = -1;
	    /* 
	     *  Silently fail to add a group master interface.  All of the
	     *	slave interfaces will add themselves.
	     */
	    return;
	}
        add_to_hunt_group(ddb, GETOBJ(int,1), csb->sense, NULL);
	break;

      case DIALER_LOAD:
	if (csb->sense) {
	    ddb->dialer_load_threshold = GETOBJ(int, 1);
	    /*
	     * If this is a rotary group member and the load threshold timer 
	     * is not already running, start it now.
	     */
	    if (HUNTGROUP_LEADER(ddb) && CONNECTED_GROUP(ddb) &&
		!TIMER_RUNNING(ddb->dialer_load_timer))
		TIMER_START(ddb->dialer_load_timer, DIALER_LOAD_TIME);
	    
	    /*
	     * Direction configuration
	     */
	    
	    ddb->thresh_direction = GETOBJ(int,2);

	} else {
            ddb->dialer_load_threshold = 0;
	    TIMER_STOP(ddb->dialer_load_timer);
	}
	break;

      case DIALER_PRIORITY:
        if (csb->sense)
	    ddb->dialer_priority = (uchar) GETOBJ(int, 1);
	else
            ddb->dialer_priority = 0;
	break;

      case DIALER_DTR:
	if (csb->sense) {
            /*
             * If an in-band dialer is configured, turn off spoofing flags
             * and clear DTR dialing statistics
             */
            if (V25BIS_DIALER(ddb)) {
                if (IS_SPOOFING(ddb)) {
                    ddb->dialer_spoofing_prot = FALSE;
                    ddb->dialer_spoofing_int = FALSE;
                    idb->state = IDBS_DOWN;
                }
                dialer_clear_stats(&ddb->dialer_dtr_stats);
            }

	    ddb->dialer = DIALER_DTR_SYNC;
	    ddb->dialer_save_pulseinterval = idb->serial_pulseinterval;
	    idb->serial_pulseinterval = 0;       /* DTR is not pulsed */
	    ddb->number_called[0] = '\0'; 	 /* Ensure no garbage */
	    ddb->remote_name[0] = '\0';
            /*
             * Fixup idb vectors for dialer use
             */
            dialer_member_adjust_encap_vectors(ddb, TRUE);

            /*
             * If a dialer is defined and the state is IDBS_DOWN, then we
             * want to start spoofing.  This means to set DTR low and start
             * the enable timer.  When it expires it sets the state to
             * IDBS_UP and sets the spoofing flags.
             */
            if (idb->state != IDBS_ADMINDOWN) {
                dialer_dtr_drop(idb);
		set_dialer_state(ddb, DIALER_WAITING);
                TIMER_START(ddb->dialer_enable_timer, ddb->dialer_enable_ticks);
            }
            route_adjust(idb);
	    reg_invoke_fast_setup(idb);
        } else {                  /* shut down dialer */
            dialer_remove(idb);
	    return;
	}
        break;

      case DIALER_HOLDQ:
        if (SLAVE_DIALER(ddb))
            return;

        if (csb->sense) {
	    ddb->dialer_holdmax = GETOBJ(int,1);
	    ultemp = (ulong) GETOBJ(int,2) * ONESEC;
	    if (ultemp)
	      ddb->dialer_holdq_ticks = ultemp + ddb->dialer_enable_ticks;
	    else
	      ddb->dialer_holdq_ticks = DIALER_HOLDQ_TIME +
		ddb->dialer_enable_ticks;
        } else {
            ddb->dialer_holdmax = 0;
	    dialer_holdq_discard_queue(&ddb->dialer_holdQ,
				       "Removing hold queue");
        }
        break;

      case DIALER_CALLER:
        if (HUNTGROUP_MEMBER(ddb)) {
            printf("\n%%Cannot change caller string on a rotary group member.");
            return;
        }

        /*
         * The additional check here for the presence
         * of and isdn_info structure seperates the
         * dialer caller command from the isdn caller
         * command. They both use the same idb field.
         * The only difference is that dialer caller
         * is only allowed on dialer interfaces.
         * dialer interfaces do not have an isdn_info
         * structure. Only D-channel interfaces do.
         */
        if (idb->isdn_info) {
            printf("\n%%This command is only supported on dialer interfaces.");
            return;
        }

        if (strlen(GETOBJ(string,1)) > DIALER_MAX_CALLER) {
            printf("\n%%Dialer caller string too long.");
            return;
        }

        dialer_parser_caller(csb, ddb);
        break;

      case DIALER_CALLBACK_SECURE:
	ddb->callback_secure = csb->sense;
	break;

      case DIALER_REMOTE_NAME:
	if (csb->sense) {
	    if (ddb->hunt_group_first != NULL) {
		printf("\n%%Unlink rotary group members first.");
		return;
	    }
	    setstring(&ddb->dialer_profile, GETOBJ(string, 1));
	    if (ddb->dialer_profile != NULL)
		ddb->dialer = DIALER_PROFILE;
	} else if (ddb->dialer_profile != NULL) {
	    pool = ddb->dialer_pool;
	    if (pool != NULL) {
		dialer_remove_pool(ddb, pool->pool_number);
	    }
	    free(ddb->dialer_profile);
	    ddb->dialer_profile = NULL;
	    ddb->dialer = DIALER_NONE;
	}
	return;

      case DIALER_POOL:
	if (csb->sense && !IS_PROFILE(ddb)) {
	    printf("\n%%Configure a dialer remote-name first");
	    return;
	}
	if (csb->sense) {
	    dialer_add_pool(ddb, GETOBJ(int, 1), GETOBJ(int, 2),
                            GETOBJ(int, 3), GETOBJ(int, 4));
	} else if (ddb->dialer_pool != NULL) {
	    dialer_remove_pool(ddb, GETOBJ(int, 1));
	}
	return;

      case DIALER_POOL_MEMBER:
	if (csb->sense) {
	    dialer_add_pool(ddb, GETOBJ(int, 1), GETOBJ(int, 2),
                            GETOBJ(int, 3), GETOBJ(int, 4));
	    ddb->dialer_holdmax = 0;
	    if (ddb->dialer_holdQ != NULL) {
		dialer_holdq_discard_queue(&ddb->dialer_holdQ,
					   "Removing hold queue");
	    }
	} else if (ddb->dialer_pool != NULL) {
	    dialer_remove_pool(ddb, GETOBJ(int, 1));
	}
	if (HUNTGROUP_LEADER(ddb)) {
	    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
		tempddb->dialer_pool_member = ddb->dialer_pool_member;
	    }
	}
	return;

      case DIALER_MAX_CALL:
	if (csb->sense) {
	    ddb->dialer_max_call = GETOBJ(int, 1);
	} else {
	    ddb->dialer_max_call = DIALER_MAXCALL;
	}
	return;

      default:
	bad_parser_subcommand(csb, csb->which);
	return;
    }

    if (HUNTGROUP_LEADER(ddb)) {
	FOR_ALL_HUNTDDBS_ON_MASTER(ddb, tempddb) {
	    copy_dialer_fields(ddb, tempddb);
	}
    }

}

/*
 * dialer_write_map
 * Write out a single map entry
 */
static void dialer_write_map (hwidbtype *idb, dialermaptype *addr)
{
    dialerentrytype *entry;
    char *buffer;

    entry = addr->entry;
    buffer = string_getnext();
    while (entry) {
	dialer_print_proto_addr(idb->firstsw, &addr->proto_addr,
				(uint)addr->link, buffer);
        nv_write(TRUE, "dialer map ");
	nv_add(TRUE, "%s", buffer);
        nv_add(entry->hostname != NULL, " name %s", entry->hostname);
        nv_add(entry->class != NULL, " class %s", entry->class);
        nv_add(entry->modem_chat_regexp != NULL, " modem-script %s",
	       entry->modem_chat_regexp);
        nv_add((entry->speed && (entry->speed != ISDN_SPEED_64)), " speed %u", 
		entry->speed);
        nv_add(entry->system_chat_regexp != NULL, " system-script %s",
       	       entry->system_chat_regexp);
	nv_add(entry->broadcast == TRUE, " broadcast");
	nv_add(entry->spc == TRUE, " spc");
        nv_add(entry->dialer_string != NULL, " %s", entry->dialer_string);

        entry = entry->next;
    }
}

void nv_dialer_map (dialerdbtype *ddb)
{
    int i;
    dialermaptype *p;
    
    for (i = 0; i < DIALERHASH; i++) {
	for (p = dialertable[i]; p; p = p->next) {
	    if ((ddb == p->ddb) && (!p->dynamic))
		dialer_write_map(ddb->ownidb, p);
	}
    }
}

/*
 * diallst_match_protocol
 * Inline to test whether the given user input matches a dialer list entry 
 * for deletion.
 */
static inline boolean diallst_match_protocol (
    parseinfo *csb,
    dlisttype *dlist, 
    int testtype,
    int testlink,
    union dllist testlist)
{ 
    /*
     * This replaces the following code :
     * if ((dlist->list_type != DIALLST_LIST) &&
     *     (dlist->linktype == testlink)) {
     *     if ((testtype == DIALLST_NONE) ||
     *        ((testtype == dlist->list_type) &&
     *         ((testtype != DIALLST_PROLIST) ||
     *         ((testtype == DIALLST_PROLIST) &&
     *          (((testlink == LINK_CLNS) &&
     *            (strcmp(dlist->name, GETOBJ(string,1)) == 0)) ||
     *           ((testlink != LINK_CLNS) &&
     *            (dlist->testlist == testlist))))))) {
     *	       return(TRUE);
     *     } else {
     *	       return(FALSE);
     * } else {
     *     return(FALSE);
     * }
     */

    /*
     * Command must not be "dialer-list <n> list <l>.
     */
    if (dlist->list_type == DIALLST_LIST)
	return(FALSE);
    /*
     * Entered protocol must match list entry protocol.
     */
    if (dlist->linktype != testlink)
	return(FALSE);
    /*
     * If "no dialer-list <n>" entered. accept it.
     */
    if (testtype == DIALLST_NONE)
	return(TRUE);
    /*
     * Entered list type must match list entry type.
     */
    if (testtype != dlist->list_type)
	return(FALSE);
    /*
     * If "no dialer-list <n> protocol <p> permit/deny" entered, accept it.
     */
    if (testtype != DIALLST_PROLIST)
	return(TRUE);
    /*
     * If "no dialer-list <n> protocol clns list <name>" entered, and <name>
     * matches list entry name, accept it.
     */
    if (testlink == LINK_CLNS) {
	if (strcmp(dlist->testlist.name, GETOBJ(string,1)) == 0)
	    return(TRUE);
    }
    /*
     * If "no dialer-list <n> protocol <p> list <l>" entered, and <l> matches
     * list entry number, accept it.
     */
    if ((testlink != LINK_CLNS) && (dlist->testlist.number == testlist.number))
	return(TRUE);
    /*
     * Anything else doesn't qualify.
     */
    return(FALSE);
}

/*
 * diallst_protocol
 * Process a dialer-list n protocol y {permit | deny | list <n> } command.
 */
static void diallst_protocol (parseinfo *csb, int number)
{
    dialerlisttype *diallst;
    dlisttype *dlist, *next, *prev;
    int testlink, testtype;
    union dllist testlist;

    /*
     * Get the protocol string.
     */
    testlink = GETOBJ(int,2);
    testtype = GETOBJ(int,3);
    testlist.number = GETOBJ(int,4);

    diallst = &darray[number];
    /*
     * Handle the "no" case first.  If "no dialer-list n protocol p" was
     * specified, remove all entries in the list for the given protocol.
     * If "no dialer-list n protocol p list l" was specified, remove only
     * that list.  Don't remove "dialer-list n list l" configuration.
     */
    if (!csb->sense) {
	dlist = diallst->dlist;
	prev = NULL;
	while (dlist) {
	    next = dlist->next;
	    if (diallst_match_protocol(csb, dlist, testtype, 
					testlink, testlist)) {
		reg_invoke_set_dialer_list(testlink, &diallst->plist, 0);
		if (prev == NULL) {
		    diallst->dlist = dlist->next;
		} else {
		    prev->next = dlist->next;
		}
		if ((testlink == LINK_CLNS) && 
		    (dlist->list_type == DIALLST_PROLIST)) {
		    free(dlist->testlist.name); 
		    dlist->testlist.name = NULL; 
		}
		free(dlist);
	    } else {
	        prev = dlist;
	    }
	    dlist = next;
	}
	return;
    }

    prev = NULL;
    for (dlist = diallst->dlist; dlist;
	 prev = dlist, dlist = dlist->next) {
	if (dlist->linktype == testlink) {
	    /*
	     * Set list number.
	     * DENY list is equal to 0,
	     * PERMIT list is equal to -1.
	     */
	    if (testtype == DIALLST_PERMIT) {
		testlist.number = -1;
	    } else if (testtype == DIALLST_DENY) {
		testlist.number = 0;
	    }

	    /*
	     * List already exists: if CLNS execute special case.
	     */
	    if (testlink == LINK_CLNS) {
		/*
		 * Free current filter name.
		 */
		if (dlist->list_type == DIALLST_PROLIST) {
		    free (dlist->testlist.name);
		    dlist->testlist.name = NULL;
		}
		/*
		 * Set new filter name.
		 */
		if (testtype == DIALLST_PROLIST) {
		    dlist->testlist.name = strdup(GETOBJ(string,1));
		    testlist = dlist->testlist;
		}
	    }

	    dlist->list_type = testtype;
	    dlist->testlist = testlist;

	    /*
	     * Finally set the fast dialerlist.
	     */
	    reg_invoke_set_dialer_list(testlink, &diallst->plist, testlist.number);
	    return;
	}
    }

    /*
     * We have a new list member, add it to the end of the list.
     */
    dlist = malloc(sizeof(dlisttype));
    if (dlist == NULL) {
	printf(nomemory);
	return;
    }

    /*
     * Set testlist for all types of dialer lists:
     * - for CLNS filters, testlist is a pointer to the filter string
     * - for access lists, testlist is equal to the access list
     * - for DENY lists, testlist is 0
     * - for PERMIT lists, testlists is -1
     */
    if (testlink == LINK_CLNS) {
	testlist.name = strdup(GETOBJ(string,1));
    } else {
	/*
	 * Update list type and list number.
	 * DENY list is equal to 0,
	 * PERMIT list is equal to -1.
	 */
	if (testtype == DIALLST_PERMIT) {
	    testlist.number = -1;
	} else if (testtype == DIALLST_DENY) {
	    testlist.number = 0;
	}
    }
    dlist->testlist = testlist;
    dlist->linktype = testlink;
    dlist->list_type = testtype;
    if (prev) {
	prev->next = dlist;
    } else {
	diallst->dlist = dlist;
    }
    reg_invoke_set_dialer_list(testlink, &diallst->plist, testlist.number);
}

/*
 * diallst_list
 * Process a dialer-list n list y command.
 */
static void diallst_list (parseinfo *csb, int number)
{
    dialerlisttype *diallst;
    dlisttype *dlist, *prev;
    int testlink, testtype;
    union dllist testlist;

    testtype = DIALLST_LIST;
    testlist.number = GETOBJ(int,2);
    testlink = diallst_getlink(testlist.number);
    prev = NULL;
    diallst = &darray[number];
    for (dlist = diallst->dlist; dlist; prev = dlist, dlist = dlist->next) {
        if (dlist->linktype == testlink) {
	    if (dlist->testlist.number == testlist.number) {
		/*
		 * If we match an existing list, and it was a "no"
		 * command, delete it and return.  Otherwise return.
		 */
		if (!csb->sense) {
		    reg_invoke_set_dialer_list(testlink, &diallst->plist, 0);
		    if (prev)
		        prev->next = dlist->next;
		    else
		        diallst->dlist = dlist->next;
		    free(dlist);
		}
	    } else {
		/*
		 * We matched the protocol, but not the list.  If it was
		 * a "no" command just ignore, else replace existing list 
		 * with the new one.
		 */
		if (csb->sense) {
		    dlist->testlist = testlist;
		    dlist->list_type = testtype;
		    reg_invoke_set_dialer_list(testlink, &diallst->plist, testlist.number);
		}
	    }
	    return;
        }
    }

    /*
     * If this is a "no" command, it is for something that didn't
     * exist, so just return
     */
    if (!csb->sense)
	return;

    /*
     * We have a new list member so add it to the end of the list
     */
    dlist = malloc(sizeof(dlisttype));
    if (!dlist) {
	printf(nomemory);
	return;
    }
    dlist->linktype = testlink;
    dlist->testlist = testlist;
    dlist->list_type = testtype;
    if (prev)
	prev->next = dlist;
    else
	diallst->dlist = dlist;
    reg_invoke_set_dialer_list(testlink, &diallst->plist, testlist.number);
}

/*
 * dialerlist_command
 * Configure an item into a dialer list.  The syntax is:
 *   dialer-list n protocol xxx keyword
 * where keyword is either permit, deny, or list y.
 */
void dialerlist_command (parseinfo *csb)
{
    dialerlisttype *diallst;
    dlisttype *dlist, *next;
    char *name;

    if (csb->nvgen) {
	int i;
        for (i = 0; i < DIALER_MAXLIST; i++) {
	    for (dlist = darray[i].dlist; dlist; dlist = dlist->next) {
		nv_write(TRUE, "%s %u", csb->nv_command, i);
		if (dlist->list_type == DIALLST_LIST) {
		    nv_add(TRUE, " protocol %s list %u",
			   print_linktype(dlist->linktype),
			   dlist->testlist.number);
		} else {
		    nv_add(TRUE, " protocol %s", 
			   print_linktype(dlist->linktype));
		    if (dlist->list_type == DIALLST_PERMIT) {
			nv_add(TRUE, " permit");
		    } else if (dlist->list_type == DIALLST_DENY) {
			nv_add(TRUE, " deny");
		    } else {	/* dlist->list_type == DIALLST_PROLIST */
			if (dlist->linktype == LINK_CLNS) {
			    name = (char *)dlist->testlist.name;
			    nv_add(TRUE, " list %s", name);
			} else {
			    nv_add(TRUE, " list %u", dlist->testlist.number);
			}
		    }
		}
	    }
	}
	return;
    }

    /*
     * get the next word.  If it was not specified,
     * then delete the entire list provided "no" was specified.
     */
    switch (csb->which) {
      case NO_DIALLST:
	diallst = &darray[GETOBJ(int,1)];
	dlist = diallst->dlist;
	diallst->dlist = NULL;
	while (dlist) {
	    reg_invoke_set_dialer_list(dlist->linktype, &diallst->plist, 0);
	    if (dlist->linktype == LINK_CLNS) {
		name = dlist->testlist.name;
		free(name);
	    }
	    next = dlist->next;
	    free(dlist);
	    dlist = next;
	}
	break;

      case DIALLST_PROTOCOL:
        diallst_protocol(csb, GETOBJ(int,1));
        break;

      case DIALLST_ACCESS:
        diallst_list(csb, GETOBJ(int,1));
        break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * dialergroup_command
 * Set up a dialer list for an interface.
 */

void dialergroup_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb == NULL) {
	if (!csb->nvgen && csb->sense)
	    printf("\n%%A dialer must first be specified.");
	return;
    }

    if (csb->nvgen) {
	nv_write(ddb->dialer_list,
		 "%s %u", csb->nv_command,
		 ddb->dialer_list);
	return;
    }
    /*
     * Don't allow a dialer-group command on a Physical Interface
     * configured for Dialer Profiles
     */
    if (!(ddb->ownidb->status & IDB_DIALER) && 
	(ddb->dialer_pool != NULL)) {
	printf("\n%%Remove Dialer Profile Configuration first");
	return;
    }

    if (csb->sense) {
	ddb->dialer_list = GETOBJ(int,1);
	ddb->dialer_plist = &darray[ddb->dialer_list];
    } else {
	ddb->dialer_list = 0;
	ddb->dialer_plist = NULL;
    }
}

/*
 * print_dialer_state
 */
static char *print_dialer_state (dialerdbtype *ddb)
{
    switch (ddb->dialer_state) {
	case DIALER_READY:
	    return("idle");
	case DIALER_PENDING:
	    return("call pending");
	case DIALER_CONNECTED:
	    return("physical layer up");
	case DIALER_OPERATIONAL:
	    return("data link layer up");
	case DIALER_DISCONNECTING:
	    return("call being disconnected");
	case DIALER_WAITING:
	    return("enabling");
	case DIALER_SHUTDOWN:
	    return("shutdown");
	default:
	    return("unknown");
    }
}

/*
 * show_dialer_state
 * Display current dialer state.
 */
void show_dialer_state (dialerdbtype *ddb)
{
    hwidbtype *idb;
    dialer_stat *ds;
    dialerentrytype *entry;

    if (ddb->dialer_load_threshold) {
	printf("\nLoad threshold for dialing additional calls is %u",
		 ddb->dialer_load_threshold);
    }

    printf("\nIdle timer (%u secs), Fast idle timer (%u secs)",
	        ddb->dialer_idle_ticks / ONESEC, 
		ddb->dialer_fast_idle_ticks / ONESEC);
    printf("\nWait for carrier (%u secs), Re-enable (%u secs)",
	        ddb->dialer_carrier_ticks / ONESEC, 
		ddb->dialer_enable_ticks / ONESEC);
    if (!HUNTGROUP_LEADER(ddb) || IS_PROFILE(ddb)) {
        printf("\nDialer state is %s", print_dialer_state(ddb));
	if (*ddb->dial_reason) {
	    printf("\nDial reason: %s", ddb->dial_reason);
	}
    }

    if (DIALPOOL_MEMBER(ddb) && ddb->dialer_bound) {
      printf("\nInterface bound to profile %s", 
	     ddb->netcfg_idb->hw_namestring);
    }

    if (TIMER_RUNNING(ddb->dialer_idle_timer))
	printf("\nTime until disconnect %u secs",  
		   TIME_LEFT_SLEEPING(ddb->dialer_idle_timer) / ONESEC);
    if (TIMER_RUNNING(ddb->dialer_fast_idle_timer))
	printf(", Time until fast disconnect %u secs", 
		   TIME_LEFT_SLEEPING(ddb->dialer_fast_idle_timer) / ONESEC);
    if (TIMER_RUNNING(ddb->dialer_enable_timer))
	printf("\nTime until interface enabled %u secs", 
		   TIME_LEFT_SLEEPING(ddb->dialer_enable_timer) / ONESEC);

    if (DIALER_CALLUP(ddb)) {
        /* 
         * Make sure that the dialer statistics are still valid since the 
         * dialer may have been reconfigured after the current connected call.
         *
         * Loop through all the entries in the dialer string and compare
         * the statistics structure addresses to verify that the dialer_stats
         * pointer still points to an existing entry statistics since deleting
         * a dial string via configuration can free up the dialer string 
         * structure of the current connected call, resulting in an invalid 
         * dialer_stats pointer. 
         */
        if (ddb->netcfg_ddb && ddb->netcfg_ddb->dialer_string) {
            for (ds = NULL, entry = ddb->netcfg_ddb->dialer_string->entry;
                 entry && !ds; entry = entry->next) {
                 if (ddb->dialer_stats == &(entry->stat)) {
	             ds = ddb->dialer_stats;
                 }
            }
	    if (ds) {
	        printf("\nCurrent call connected ");
	        print_dhms(ds->curr_call_time);
	    }
        }
        if (ddb->number_called[0] != '\0') {
	    printf("\nConnected to %s", ddb->number_called);
	}
        if (ddb->remote_name[0] != '\0') {
	    printf(" (%s)", ddb->remote_name);
	}
	idb = ddb->ownidb;
	if (is_isdn(idb))
	    reg_invoke_v120_show(idb);
 	if (dialer_detailed_debug && idb->dialerdb->conn_group) {
 	    printf("\nGroup number %s, name %s",
 		   idb->dialerdb->conn_group->number_called,
 		   idb->dialerdb->conn_group->remote_name);
 	}		
    }
    if ((ddb->dialer_holdQ) &&
	(!DIALER_HOLDQ_EMPTY(&ddb->dialer_holdQ->queue))) {
	printf("\n%d packets in the hold queue",
	       ddb->dialer_holdQ->queue.count);
    }
}

/*
 * Accumulate dialer map statistics
 *
 * WARNING: this routine mallocs accounting structures.  It is the
 * responsibility of the caller to call dialer_cum_free_stats() when
 * done with the information.
 */
static void
dialer_cum_stats (dialerentrytype **head, dialerentrytype *entry)
{
    dialerentrytype *e;

    for (e = *head; e; e = e->next) {
	if (!strcmp(e->dialer_string, entry->dialer_string)) {
	    e->stat.num_successes += entry->stat.num_successes;
	    e->stat.num_failures  += entry->stat.num_failures;
	    e->stat.last_call_success |= entry->stat.last_call_success;
	    if (!TIMER_RUNNING(e->stat.last_call_time) ||
		(TIMER_RUNNING(entry->stat.last_call_time) &&
		 TIMER_LATER(entry->stat.last_call_time,
			     e->stat.last_call_time)))
		 e->stat.last_call_time = entry->stat.last_call_time;
	    return;
	}
    }

    e = malloc(sizeof(dialerentrytype));
    if (e) {
	bcopy(entry, e, sizeof(dialerentrytype));

	e->next = *head;
	*head = e;
    }
}

/*
 * dialer_cum_free_stats
 * free up all the resources that dialer_cum_stats allocated
 *
 */
static void
dialer_cum_free_stats (dialerentrytype **head)
{
    dialerentrytype *e, *next;

    for (e = *head; e; e = next) {
	next = e->next;
	free(e);
    }
    *head = NULL;
}

/*
 * dialer_display_stats
 *
 * As the name suggests, print out the call stats
 */
static void dialer_display_stats (dialer_stat *dsp, char *dial_str, char *type)
{
    char dhms[12]; /* Enough space for the date-time string */

    if (dsp == NULL || dial_str == NULL) {
	return;
    }

    sprint_dhms(dhms, dsp->last_call_time);
    printf("\n%16s%-10u %-10u    %11s", dial_str, dsp->num_successes,
	dsp->num_failures, dhms);
    if (TIMER_RUNNING(dsp->last_call_time)) {
	printf("%-14s", dsp->last_call_success ? "successful" : "failed");
    } else {
	printf("%-14s", "-");
    }
    if (type) {
	printf("   %s", type);
    }
}

/*
 * show_dialer_stats
 * Display dialer statistics.
 */
void show_dialer_stats (dialerdbtype *ddb)
{
    dialermaptype *p;
    dialerentrytype *e, *dialer_cum;
    int i;

    printf("\n\nDial String      Successes   Failures    Last called   "
	   "Last status");

    /*
     * First display DTR dialer stats, if any.
     */
    if (DTR_DIALER(ddb)) {
	dialer_display_stats(&ddb->dialer_dtr_stats, "-", "DTR dialer");
    } else if (ddb->dialer_string != NULL) {
         /*
          * Else display default statistics if a dialer string is configured.
          */
	for (e = ddb->dialer_string->entry; e; e = e->next)
	    dialer_display_stats(&e->stat, e->dialer_string, "Default");
   }

     /*
      * Last go through all dialer maps.
      */
    dialer_cum = NULL;

    for (i = 0; i < DIALERHASH; i++)
	for (p = dialertable[i]; p; p = p->next)
	    if (ddb == p->ddb)
	    	for (e = p->entry; e; e = e->next)
		    dialer_cum_stats(&dialer_cum, e);

    for (e = dialer_cum; e; e = e->next) {
	dialer_display_stats(&e->stat, e->dialer_string, NULL);
    }
    dialer_cum_free_stats(&dialer_cum);
}

void show_dialer_info (dialerdbtype *ddb)
{
    hwidbtype *idb;

    idb = ddb->ownidb;
    printf("\n\n%s - dialer type = %s", idb->hw_namestring,
	   dialer_keywords[ddb->dialer]);
    if (V25BIS_DIALER(ddb) || ASYNC_DIALER(ddb)) {
	if (ddb->dialer_parity == DIALER_INBAND_ODDPARITY) {
	    printf(" ODD-PARITY");
	} else {
	    printf(" NO-PARITY");
	}
    }

    if (HUNTGROUP_MEMBER(ddb)) {
	printf("\nRotary group %d, priority %u",
		ddb->hunt_group_master->ownidb->unit, ddb->dialer_priority);
    } else if (DIALPOOL_MEMBER(ddb)) {
	printf("\nDialer pool %d, priority %u",
		ddb->dialer_pool->pool_number, ddb->dialer_pool->pool_priority);
    }
    show_dialer_state(ddb);

    /*
     * Display statistics only for rotary group leaders and direct serial
     * interfaces.
     */

    if (!SLAVE_DIALER(ddb)) {
      show_dialer_stats(ddb);
    }
}


/*
 * show_dialer_hunt_group
 * When we're doing a show dialer int <interface>, this routine
 * will show either just the interface (in the case of a group
 * member) or the entire hunt group (ie: all members of a pri group)
 */
static void show_dialer_hunt_group (dialerdbtype *ddb)
{
    dialerdbtype *member_ddb;

    if (ddb == NULL)
	return;
    
    reg_invoke_dialer_show_info(ddb->dialer, ddb);

    if (HUNTGROUP_LEADER(ddb)) {
	FOR_ALL_HUNTDDBS_ON_MASTER(ddb, member_ddb) {
	    if (member_ddb->ownidb->status & IDB_VIRTUAL) {
		continue;
	    }
	    show_dialer_hunt_group(member_ddb);
	}
    }
}


/*
 * show_dialer_map_entry_still_valid
 *
 * Check to make sure that a dialer map entry is still valid
 */
static boolean show_dialer_map_entry_still_valid (dialerentrytype *check)
{
    dialermaptype *map;
    dialerentrytype *entry;
    int i;
    
    for (i = 0; i < DIALERHASH; i++) {
	for (map = dialertable[i]; map; map = map->next) {
	    entry = map->entry;
	    while (entry) {
		if (entry == check)
		    return (TRUE);
		entry = entry->next;
	    }
	}
    }
    return (FALSE);
}

/*
 * show_dialer_map_print_by_name
 *
 * Print out all the dialer maps with the name passed
 */
static void show_dialer_map_print_by_name (char *name)
{
    dialermaptype *map;
    dialerentrytype *entry;
    dialerentrytype *next;
    char *address_buffer = string_getnext();
    char *entry_name;
    int i;

    if (!address_buffer)
	return;
        
    for (i = 0; i < DIALERHASH; i++) {
	for (map = dialertable[i]; map; map = map->next) {
	    entry = map->entry;
	    while (entry) {
		next = entry->next;
		if (entry->hostname)
		    entry_name = entry->hostname;
		else
		    entry_name = "";
		if (strcmp(name,entry_name) == 0) {
		    
		    address_buffer[0] = '\0';
		    dialer_print_proto_addr(map->ddb->ownidb->firstsw,
					    &map->proto_addr,map->link,
					    address_buffer);
		    printf ("\n%s dialer map %s %s %s (%s) on %s",
			    (map->dynamic ? "Dynamic" : "Static"),
			    address_buffer,
			    (entry->hostname ? "name" : common_str_empty_str),
			    entry_name,
			    (entry->dialer_string ? 
			     entry->dialer_string :
			     common_str_empty_str),
			    map->ddb->ownidb->hw_namestring);
		    if (!show_dialer_map_entry_still_valid(next))
			return;
		}
		entry = next;
	    }
	}
    }
}


static char *show_dialer_map_find_next_name (char *name)
{
    dialermaptype *map;
    dialerentrytype *entry;
    int i;
    char *ret_name = NULL;
    char *entry_name;
    
    for (i = 0; i < DIALERHASH; i++) {
	for (map = dialertable[i]; map; map = map->next) {
	    entry = map->entry;
	    while (entry) {
		if (entry->hostname)
		    entry_name = entry->hostname;
		else
		    entry_name = "";
		if ((strcmp(entry_name,name) > 0 ) &&
		    ((ret_name == NULL) ||
		     (strcmp(entry_name,ret_name) < 0))) {
		    if (ret_name == NULL)
			ret_name = string_getnext();
		    strcpy (ret_name,entry_name);
		}
		entry = entry->next;
	    }
	}
    } 
    return ret_name;
}


void show_dialer (parseinfo *csb)
{
    hwidbtype *idb;
    dialerdbtype *ddb;
    boolean onceflag;
    list_element *l_elt;
    char *name;
    
    
    
    switch (csb->which) {
    case SHOW_DIALER:
	automore_enable(NULL);
	if (GETOBJ(idb,1)) {
	    idb = (GETOBJ(idb,1)->hwptr);
	    ddb = get_dialerdb(idb);
	    if (ddb != NULL) {
		show_dialer_hunt_group(ddb);
	    } else {
		printf("\n%s - Dialing not enabled on this interface.", 
		       idb->hw_namestring);
	    }
	} else {
	    onceflag = FALSE;
	    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_SERIAL, l_elt, idb) {
		ddb = get_dialerdb(idb);
		if (ddb != NULL) {
		    if (onceflag) {
			automore_conditional(0);
		    } else {
			onceflag = TRUE;
		    }
		    reg_invoke_dialer_show_info(ddb->dialer, ddb);
		}
	    }
	}
	break;
    case SHOW_DIALER_MAP:
	automore_enable(NULL);
	name = "";
	show_dialer_map_print_by_name(name); 
	while ((name=show_dialer_map_find_next_name(name)) != NULL)
	    show_dialer_map_print_by_name(name);
	automore_disable();
	break;
    }
    
}

/*
 * clear_dialer
 * Clears 'show dialer' information
 */
void clear_dialer (parseinfo *csb)
{
    hwidbtype *idb;
    dialermaptype *p;
    dialerentrytype *e;
    dialerdbtype *ddb;
    int i;
    list_element *l_elt;

    idb = hwidb_or_null(GETOBJ(idb,1));
    if (idb != NULL) {
	ddb = get_dialerdb(idb);
    	if (ddb == NULL) {
	    printf("\n%s - ", idb->hw_namestring);
	    printf("Dialing not enabled on this interface.");
	    return;
    	}

       /*
        * Clear statistics for DTR dialers.
        */
    	dialer_clear_stats(&ddb->dialer_dtr_stats);

       /*
        * Clear default statistics.
        */
	if (ddb->dialer_string != NULL)
    	    dialer_clear_stats(&ddb->dialer_string->entry->stat);

       /*
        * Do the same for map strings
     	*/
	for (i = 0; i < DIALERHASH; i++) {
	    for (p = dialertable[i]; p; p = p->next) {
		if (ddb == p->ddb) {
		    for (e = p->entry; e; e = e->next) {
			dialer_clear_stats(&e->stat);
		    }
		}
	    }
	}
    } else {
	FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_SERIAL, l_elt, idb) {
	    ddb = get_dialerdb(idb);
	    if (ddb != NULL) {
		dialer_clear_stats(&ddb->dialer_dtr_stats);
		if (ddb->dialer_string != NULL)
    		    dialer_clear_stats(&ddb->dialer_string->entry->stat);
		for (i = 0; i < DIALERHASH; i++) {
		    for (p = dialertable[i]; p; p = p->next) {
			if (ddb == p->ddb) {
			    for (e = p->entry; e; e = e->next) {
				dialer_clear_stats(&e->stat);
			    }
			}
		    }
		}
	    }
	}
    }
    automore_disable();
}

/*
 * dialer_set_leader_encap
 * Set the encapsulation of the dialer group master
 */
void dialer_set_leader_encap (parseinfo *csb, hwidbtype *idb,
			      encapstype enctype)
{
    dialerdbtype *ddb;

    /*
     * call encapsulation setup routine, and setup dialer/encap specific fields.
     */
    idb->enctype = enctype;
    reg_invoke_media_serial_setup(enctype, csb, idb);
    ddb = get_dialerdb(idb);
    idb->enctype_cfg = idb->enctype;
    ddb->dialer_encap_ptr = NULL;
    reg_invoke_dialer_setup(idb->enctype, ddb, TRUE);

    /*
     * Rotary group leaders may have to change their subinterface type
     * if X.25 or FR was configured.
     */
    set_subif_link_type(idb->firstsw);

    /*
     * Make sure idb vectors are setup properly.
     * Don't execute this code for PRI. The
     * vectors are setup in mip_encapsulation.
     * Don't execute this when ppp is in a state of using lapb
     * as an extra layer of encapsulation. Vectors point to lapb code.
     */
    if ((idb->type !=  IDBTYPE_T1) &&
        (idb->type != IDBTYPE_E1) &&
        !idb->ppp_lapb)
        dialer_member_adjust_encap_vectors(ddb, TRUE);
}

/*
 * dialer_set_member_encap - Set the encapsulation of a dialer group
 * member.
 */

void dialer_set_member_encap (parseinfo *csb, hwidbtype *idb,
			      encapstype enctype)
{
    dialerdbtype *ddb;
    parse_packet_t save_parse_packet;

    /*
     *  VACCESS interface may be part of the dialer huntgroup
     *    because multilink ppp code put it there when created
     *  Do not change encaps of a VACCESS interface.  Encaps
     *   for VACCESS int. can only be changed by configuring the appropriate
     *   VTEMPLATE int.
     */
    if (idb->type == IDBTYPE_VACCESS) {
	return;
    }

    ddb = get_dialerdb(idb);

    if ((enctype != idb->enctype) || 
	 (enctype == ET_X25) || 
	  (enctype == ET_LAPB)) {
	/*
	 * Set new encapsulation if it changed.  If X.25 or LAPB, encapsulation 
	 * may have changed from DTE to DCE, so do it anyway.
	 */
	save_parse_packet = idb->parse_packet;
	reg_invoke_dialer_setup(idb->enctype, ddb, FALSE);
	if (idb->deencap)
	    (*idb->deencap)(idb);	/* Shut down current encapsulation */

	/*
	 * Call encapsulation specific setup routine.
	 */
	idb->enctype = enctype;
	reg_invoke_media_serial_setup(enctype, csb, idb);
	idb->enctype_cfg = idb->enctype; /* Save new configured default */
	reg_invoke_dialer_setup(idb->enctype, ddb, TRUE);

	/*
	 * Rotary group leaders need to keep their own parse_packet vector,
	 * so that ISDN dialers can still bring up layer 2 on the D channel
	 * when member of a rotary group.
	 */
	if (HUNTGROUP_LEADER(ddb)) {
	    idb->parse_packet = save_parse_packet;
	}
    }

    /*
     * Rotary group leaders may have to change their subinterface type
     * if X.25 or FR was configured.
     */
    set_subif_link_type(idb->firstsw);

    /*
     * Make sure idb vectors are setup properly.
     * Don't execute this code for PRI. The
     * vectors are setup in mip_encapsulation.
     * Don't execute this when ppp is in a state of using lapb
     * as an extra layer of encapsulation. Vectors point to lapb code.
     */
    if (idb->type !=  IDBTYPE_T1 &&
        idb->type != IDBTYPE_E1 &&
        !idb->ppp_lapb)
    	dialer_member_adjust_encap_vectors(ddb, TRUE);
}

/*
 * dialer_setencap - Set the encapsulation on a dialer interface
 *
 * Set the encapsulation on a dialer interface
 */

static void dialer_setencap (parseinfo *csb)
{
    hwidbtype *master_idb;
    hwidbtype *tempidb;
    hwidbtype *saveidb;
    dialerdbtype *master_ddb, *tempddb;
    encapstype enctype;
    int call_flag;
    leveltype level;

    master_idb = csb->interface->hwptr;	/* Get interface pointer */
    master_ddb = get_dialerdb(master_idb);
    enctype = GETOBJ(int, 1);
    call_flag = GETOBJ(int, 10);

    /*
     * If we're a member of a hunt group, do not allow encapsulation changes
     */
    if (HUNTGROUP_MEMBER(master_ddb) && !call_flag) {
         printf("\n%% Cannot change encapsulation of hunt group member");
        return;
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * The master idb doesn't call dialer_set_member_encap(), because a few
     * things have already been done in encapsulation_command(), so just set
     * the new encapsulation here.  Dialer interfaces part of a rotary group
     * are handled like any rotary group member.
     */
    if (!HUNTGROUP_MEMBER(master_ddb)) {
	dialer_set_leader_encap(csb, master_idb, enctype);
    } else {
	dialer_set_member_encap(csb, master_idb, enctype);
    }

    saveidb = csb->interface->hwptr;
    FOR_ALL_HUNTDDBS_ON_MASTER(master_ddb, tempddb) {
	tempidb = tempddb->ownidb;
	csb->interface->hwptr = tempidb;
	if (HUNTGROUP_LEADER(tempddb)) {
    	    /* 
	     * This code has been placed here mainly to accomodate
	     * PRI. mip_encapsulation needs to adjust the idb
	     * vectors in a specific sequence. BRI interfaces will
	     * call isdn_setencap and dialer interfaces will call
	     * dialer_setencap.
	     */
            SETOBJ(int, 10) = TRUE;
	    (*tempidb->setencap)(csb);
	} else {
            dialer_set_member_encap(csb, tempidb, enctype);
    	}
    }
    csb->interface->hwptr = saveidb;
    master_idb->iqueue = (iqueue_t) return_nothing;
    set_default_queueing(master_idb);
    master_idb->periodic =  NULL;
    master_idb->parse_packet = (parse_packet_t) return_nothing;
    master_idb->hdlc_lineup = TRUE;
    master_idb->ppp_lineup = TRUE;

    reset_interrupt_level(level);
}

/*
 * dialer_set_maxdgram
 * To let encapsulations set idb->maxdgram.
 */
static boolean dialer_set_maxdgram (hwidbtype *idb, int buffersiz, int maxdgram)
{
    if_maxdgram_adjust(idb, maxdgram);

    return(TRUE);
}

/*
 * dialer_create_master_idb - Actually create a master IDB
 *
 */
static idbtype *dialer_create_master_idb (uint unit)
{
    hwidbtype *idb;
    idbtype *swidb;
    dialerdbtype *ddb;

    idb = idb_create();
    if (!idb) {
        printf(nomemory);
	return(NULL);
    }

    /*
     * This wants to be a serial interface, make sure it has a serial
     * subblock.
     */
    create_serial_sb(idb);

    ddb = init_dialer_idb(idb);
    if (ddb == NULL) {
        printf(nomemory);
        return(NULL);
    }
    swidb = idb->firstsw;
    idb->setencap = dialer_setencap;
    idb->soutput = (soutput_t) return_nothing;
    idb->reset = (reset_t) return_nothing;
    idb->iqueue = (iqueue_t) return_nothing;
    idb->set_maxdgram = dialer_set_maxdgram;
    idb->broadcast = (broadcast_t) return_fuz_false;
    idb->maxmtu = idb->maxmtu_def = MAXSERIALSIZE;
    idb->input_defqcount = INPUTQ_DEFAULT;
    idb->reliability = 255;
    idb->load = 1;
    idb->rxload = 1;
    idb->type = IDBTYPE_DIALER;
    /* take a guess on the bandwidth... */
    set_default_interface_bandwidth(idb->firstsw, VISIBLE_MCI_SERIAL_BANDSLOW);
    set_default_interface_delay(idb->firstsw, MCI_SERIAL_DELAY); /* same for pan */
    idb->keep_period = KEEPALIVE_DEFAULT;
    idb->name = IFNAME_DIALER;
    idb->unit = unit;
    idb_init_names(swidb, TRUE);
    reg_invoke_sw_idb_init(swidb, TRUE);
    holdq_init(idb, HDLC_HOLDQ);
    hdlc_setup(NULL, idb);
    idb->fast_switch_type = FS_DIALER;
    idb->vencap = dialer_vencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->parse_packet = (parse_packet_t) return_nothing;
    idb->serial_pulseinterval = DIALER_DEFAULT_PULSEINTERVAL;
    idb->serial_pulseinterval_def = DIALER_DEFAULT_PULSEINTERVAL;
    idb->periodic = NULL;
    idb->hdlc_lineup = TRUE;
    idb->status |= IDB_SERIAL | IDB_HEARSELF | IDB_DIALER | IDB_BRIDGE;
    idb->state = IDBS_UP;
    idb->hw_if_index = idb_get_next_hw_index();
    idb->snmp_if_index = swidb->snmp_if_index = reg_invoke_ifmib_register_hwidb(idb);

    ddb->hunt_group_lead = TRUE;
    ddb->dialer_spoofing_int = TRUE;
    ddb->dialer_spoofing_prot = TRUE;
    set_dialer_state(ddb, DIALER_READY);
    serial_idb_enqueue(idb);
    idb_enqueue(idb);
    route_adjust(idb);
    reg_invoke_fast_setup(idb);
    return(swidb);
}

/*
 * dialer_createidb
 * Create a virtual idb for a dialer hunt group - Parser interface
 */
static idbtype *dialer_createidb (uint unit)
{
    return(dialer_create_master_idb(unit));
}

boolean dialer_parser_createidb (uint64 *ifcode, parseinfo *csb)
{
    if (*ifcode == IFTYPE_DIALER) {
	csb->createidb = dialer_createidb;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * dialer_deleteidb
 * "delete" a dialer idb: reset fields and unlink group members
 */
void dialer_deleteidb (idbtype *idb)
{
    hwidbtype *hwidb;

    hwidb = hwidb_or_null(idb);

    if ((hwidb == NULL) || (hwidb->type != IDBTYPE_DIALER))
	return;

    /*
     * Clean up the dialer fields, but keep the dialerdb structure.
     */
    dialer_remove(hwidb);
}

/*
 * propagate_hunt_changes
 * Propagate changes to the hunt group idb's.  
 */
void propagate_hunt_changes (
    hwidbtype *idb)
{
    dialerdbtype *ddb, *hunt_ddb;

    if (!if_is_ddr(idb))
        return;

    ddb = get_dialerdb(idb);
    if (!HUNTGROUP_LEADER(ddb))
	return;

    FOR_ALL_HUNTDDBS_ON_MASTER(ddb, hunt_ddb) {
	copy_hunt_fields(ddb, hunt_ddb);       
	copy_priority_fields(ddb, hunt_ddb);
	reg_invoke_media_fix_serial_vectors(ET_LAPB, hunt_ddb->ownidb);
    }
}

/*
 * dialer_params_init
 * Initialize DIALER static map parameters
 */
static void dialer_params_init (dialer_params_type *params)
{
    params->callback_server_user = FALSE;
    params->callback_server_dial = FALSE;
    params->voice_call = FALSE;
    params->do_nsf = FALSE;
    params->nsf_type = 0;
    params->idle_ticks = 0;
    params->fast_idle_ticks = 0;
    params->enable_ticks = 0;
    params->carrier_ticks = 0;
    params->speed = ISDN_SPEED_64;
    params->spc   = FALSE;
}

/*
 * dialer_mapclass_command
 * Parse DIALER mapclass commands
 */
void dialer_mapclass_command (parseinfo *csb)
{
  mapclass_type *class_ptr = csb->mapclass;
  dialer_params_type *params = class_ptr->params;

  if (class_ptr->encap != ET_DIALER) {
    printf("\n%% Can't configure dialer parameters in non dialer map class");
    return;
  }

  /*
   * if no parameters configured yet, initialiaze parameter list
   */
  if (!params) {
    params = malloc(sizeof(dialer_params_type));
    if (!params) {
      printf(nomemory);
      return;
    }
    dialer_params_init(params);
    class_ptr->params = params;
  }
  switch (csb->which) {
  case DIALER_MAP_CALLBACK_SERVER:
    if (csb->sense) {
      params->callback_server_user = GETOBJ(int,1);
      params->callback_server_dial = GETOBJ(int,2);
      if ((!params->callback_server_user) && 
	  (!params->callback_server_dial))
	params->callback_server_user = TRUE;
    } else {
      params->callback_server_user = FALSE;
      params->callback_server_dial = FALSE;
    }
    break;
  case DIALER_MAP_VOICE_CALL:
    if (csb->sense) {
      params->voice_call = TRUE;
    } else {
      params->voice_call = FALSE;
    }
    break;
  case DIALER_MAP_SDN_CALL:
    if (csb->sense) {
      params->do_nsf = TRUE;
      params->nsf_type = 0xe1;
    } else {
      params->do_nsf = FALSE;
      params->nsf_type = 0;
    }
    break;
  case DIALER_MAP_MEGA_CALL:
    if (csb->sense) {
      params->do_nsf = TRUE;
      params->nsf_type = 0xe3;
    } else {
      params->do_nsf = FALSE;
      params->nsf_type = 0;
    }
    break;
  case DIALER_MAP_ACCU_CALL:
    if (csb->sense) {
      params->do_nsf = TRUE;
      params->nsf_type = 0xe6;
    } else {
      params->do_nsf = FALSE;
      params->nsf_type = 0;
    }
    break;
  case DIALER_MAP_IDLE:
    if (csb->sense)
      params->idle_ticks = (ulong) GETOBJ(int,1) * ONESEC;
    else
      params->idle_ticks = 0;	      
    break;

  case DIALER_MAP_FAST_IDLE:
    if (csb->sense)
      params->fast_idle_ticks = (ulong) GETOBJ(int,1) * ONESEC;
    else
      params->fast_idle_ticks = 0;	      
    break;

  case DIALER_MAP_REENABLE:
    if (csb->sense)
      params->enable_ticks = (ulong) GETOBJ(int,1) * ONESEC;
    else
      params->enable_ticks = 0;	      
    break;

  case DIALER_MAP_CARRIER:
    if (csb->sense)
      params->carrier_ticks = (ulong) GETOBJ(int,1) * ONESEC;
    else
      params->carrier_ticks = 0;	      
    break;

  case DIALER_MAP_ISDN:
    if (csb->sense) {

      if (GETOBJ(int, 3)) 
	params->speed = ISDN_SPEED_56;

      if (GETOBJ(int, 4))
	params->spc = TRUE;

    } else {

      if (GETOBJ(int, 3))
	  params->speed = ISDN_SPEED_64;

      if (GETOBJ(int, 4))
	params->spc = FALSE;
    }
    break;

  default:
    bad_parser_subcommand(csb, csb->which);
    break;
  }
}
/*
 * dialer_mapclass_nvgen
 * Does map-class nvgen
 */
void dialer_mapclass_nvgen (mapclass_type *class_ptr)
{
  dialer_params_type *params = class_ptr->params;

  if (params) {

    if 	((params->callback_server_user || params->callback_server_dial)) {
      nv_write(TRUE,"dialer callback-server");
      nv_add(params->callback_server_user, " username");
    }

    if (params->voice_call) {
      nv_write(TRUE,"dialer voice-call");
    }

    if (params->do_nsf) {
      if (params->nsf_type == 0xe1)
	nv_write(TRUE,"dialer outgoing sdn");
      if (params->nsf_type == 0xe3)
	nv_write(TRUE,"dialer outgoing mega");
      if (params->nsf_type == 0xe6)
	nv_write(TRUE,"dialer outgoing accu");
    }

    if (params->idle_ticks)
      nv_write(params->idle_ticks != DIALER_IDLE_TIME,
	       "dialer idle-timeout %u", params->idle_ticks / ONESEC);

    if (params->fast_idle_ticks)
      nv_write(params->fast_idle_ticks != DIALER_FAST_IDLE_TIME,
	       "dialer fast-idle %u", params->fast_idle_ticks / ONESEC);

    if (params->enable_ticks) 
      nv_write(params->enable_ticks != DIALER_ENABLE_TIME,
	       "dialer enable-timeout %u", params->enable_ticks / ONESEC);

    if (params->carrier_ticks)
      nv_write(params->carrier_ticks != DIALER_CARRIER_TIME,
	       "dialer wait-for-carrier-time %u",
	       params->carrier_ticks / ONESEC);

    if ((params->speed == ISDN_SPEED_56) ||
	(params->spc)) {
	nv_write(TRUE, "dialer isdn");
	nv_add((params->speed == ISDN_SPEED_56), " speed 56");
	nv_add(params->spc, " spc");
    }
  }
}

/*
 * dtr_dialer
 * return TRUE if the idb is configured with a DTR dialer
 */
boolean dtr_dialer (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(DTR_DIALER(ddb));
    }
    return(FALSE);
}
