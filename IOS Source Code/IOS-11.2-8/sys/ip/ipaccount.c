/* $Id: ipaccount.c,v 3.4.58.4 1996/06/29 00:11:16 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccount.c,v $
 *------------------------------------------------------------------
 * ipaccount.c -- accumulate "accounting" data on ip packets.
 *
 * Copyright (c) 1989-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccount.c,v $
 * Revision 3.4.58.4  1996/06/29  00:11:16  snyder
 * CSCdi61694:  change some char * to char [] in ip
 * Branch: California_branch
 *     save 116 bytes
 *
 * Revision 3.4.58.3  1996/06/28  23:17:35  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.58.2  1996/05/04  01:03:11  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - ARP, IP protocol, fast-switching and smf support for the Bridge-group
 *   Virtual Interface.
 * - Set up bridge/route indicators based on CRB/IRB is enabled or not.
 *
 * Constrained Multicast Flooding
 * - Convert tbridge flood array to a linked list of tbifd's.
 *
 * Name Access List
 *
 * Revision 3.4.58.1  1996/04/29  05:19:13  pst
 * CSCdi54786:  IP violation accounting goes wrong
 * Branch: California_branch
 *
 * Revision 3.4  1995/11/17  23:30:35  thille
 * CSCdi43342:  IP Violations Accounting does not reset threshhold
 * exceeded counter
 * Initialize and reset counter.  Duh!
 *
 * Revision 3.3  1995/11/17  09:34:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:40:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:56:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/11  23:49:17  hampton
 * Convert IP host and IP routing to fully use the passive timers macros
 * instead of referencing the system clock directly.  [CSCdi40085]
 *
 * Revision 2.2  1995/08/07 20:11:53  raj
 * CSCdi37551:  show ip accounting not scheduler friendly if output aborted
 * Just needed a automore_quit at the right place.
 *
 * Revision 2.1  1995/06/07  20:58:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdlib.h>
#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "packet.h"
#include "ip.h"
#include "ipaccount.h"
#include "access.h"
#include "ipaccess.h"
#include "logger.h"
#include "../os/free.h"

/*
 * Local storage
 */
ip_acctg_info_type ip_acctg_info;	/* accounting data structure */
ip_acctg_acc_type *ip_acctg_acc;	/* access information for networks */
queuetype ipacctQ;			/* local queue of memory */
queuetype ipmemQ;			/* REAL memory obtained */
boolean ip_acctg_on;                    /* whether we do accounting */

/*
 * ip_acct_init
 * Initialize variables for IP accounting
 */

void ip_acct_init (void)
{
    ip_acctg_info.generation = 1;
    ip_acctg_info.threshold = IPACCTGDEFAULTLIMIT;
    ip_acctg_acc = malloc(sizeof(ip_acctg_acc_type));
    if (ip_acctg_acc == NULL) {
	printf(nomemory);
    } else {
	/* unneeded, since malloc() returns zeroed memory */
	/* ip_acctg_acc->count = 0; */
	/* ip_acctg_acc->transits_def = 0; */
	/* ip_acctg_acc->transits = 0; */
    }
    ip_acctg_on = FALSE;

    /*	
     * Initialize IP accounting parser support
     */
    ipaccount_parser_init();
}

/*
 * ip_accumulate_acctg
 * Called from ipsendnet() when IP accounting is enabled on an interface.
 */

void ip_accumulate_acctg (iphdrtype *ip, acl_headertype *violation)
{
    ip_acctg_entry_type *ipae;
    long slot;
    ip_acctg_mem_type *ipam;
    uint i, aclnum;
    ipaddrtype srcadr, dstadr, mask;
    ip_acctg_acc_entry_type *acc_entry;
    leveltype SR;
    
    if (violation) {
	switch (violation->type) {
	case ACL_IP_SIMPLE:
	case ACL_IP_EXTENDED:
	    aclnum = atoi(violation->name);
	    break;

	default:
	    return;		/* We only support old ACLs */
	}
    } else
	aclnum = NO_VIOLATION;
    
    if (ip_acctg_acc != NULL) {
	srcadr = ip->srcadr;
	dstadr = ip->dstadr;
	for (i = 0, acc_entry = &ip_acctg_acc->list[0]; 
	     i < ip_acctg_acc->count; acc_entry++, i++) {
	    mask = acc_entry->mask;
	    if (((srcadr & mask) == acc_entry->addr) ||
		((dstadr & mask) == acc_entry->addr)) {
		goto hit;
	    }
	}
	if (ip_acctg_acc->count == 0)
	    goto hit;
	if (ip_acctg_acc->transits == 0)
	    return;
	ip_acctg_acc->transits--;
    }
hit:
    slot = ip->srcadr;
    slot ^= ip->dstadr;
    slot ^= (slot >> 16);
    slot ^= (slot >>8);
    slot &= 0xFF;
    SR = raise_interrupt_level(NETS_DISABLE);
    ipae = ip_acctg_info.current->store[slot];
    while (ipae) {
	if ((ipae->srcadr == ip->srcadr) &&
	    (ipae->dstadr == ip->dstadr) &&
	    (ipae->access_violation == aclnum)) {
	    ipae->packets++;
	    ipae->bytes += ip->tl;
            ipae->access_violation = aclnum;
	    reset_interrupt_level(SR);
	    return;
	}
	ipae = ipae->next;
    }
    reset_interrupt_level(SR);
    
    /*
     * See if count exceeds the configured threshold.
     */
    if (ip_acctg_info.count < ip_acctg_info.threshold) {
	SR = raise_interrupt_level(NETS_DISABLE);	/* lock out fastswitch accumulate */
	ipae = dequeue(&ipacctQ);
	reset_interrupt_level(SR);
	if (ipae == NULL) {
	    /*
	     * None on queue -- see if any memory blocks are *empty*
	     */
	    for (ipam = (ip_acctg_mem_type *) ipmemQ.qhead; ipam;
		 ipam = ipam->next) {
		if (ipam->fragments == IPACCTGENTRIESPERBLOCK) {
		    ipam->fragments = 0;
		    break;
		}
	    }
	    if (!ipam && !mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {
		/*
		 * No empty memory blocks found -- get more memory.
		 * Give back memory if we are running the system too low.
		 */
		ipam = malloc(sizeof(ip_acctg_mem_type));
		if (ipam)
		    enqueue(&ipmemQ, ipam);
	    }
	    if (ipam) {
		ipae = &ipam->entry[0];
		for (i = 0; i <IPACCTGENTRIESPERBLOCK; i++, ipae++) {
		    ipae->memory = (ulong *) ipam;
		    SR = raise_interrupt_level(NETS_DISABLE);
		    enqueue(&ipacctQ, ipae);
		    reset_interrupt_level(SR);
		}
	    }
	    SR = raise_interrupt_level(NETS_DISABLE);
	    ipae = dequeue(&ipacctQ);
	    reset_interrupt_level(SR);
	}
	if (ipae) {
	    ipae->srcadr = ip->srcadr;
	    ipae->dstadr = ip->dstadr;
	    ipae->bytes = ip->tl;
	    ipae->packets = 1;
            ipae->access_violation = aclnum;
	    SR = raise_interrupt_level(NETS_DISABLE);
	    ipae->next = ip_acctg_info.current->store[slot];
	    ip_acctg_info.current->store[slot] = ipae;
	    reset_interrupt_level(SR);
	    ip_acctg_info.count++;
	    return;
	}
    }
    
    /*
     * If we get here -- we where unable to save the information.
     */
    if (aclnum == NO_VIOLATION) {
	ip_acctg_info.current->packets_tossed++;
	ip_acctg_info.current->bytes_tossed += ip->tl;
    } else 
        ip_acctg_info.current->violations_tossed++;
}

/*
 * ip_acctlist
 * Handle the [no] ip accounting-list ipaddress mask command
 */

void ip_acctlist (parseinfo *csb)
{
    int i;
    ipaddrtype addr, mask;

    if (ip_acctg_acc == NULL)
	return;

    if (csb->nvgen) {
	for (i = 0; i < ip_acctg_acc->count; i++) {
	    nv_write(TRUE, "%s %i %i", csb->nv_command,
		     ip_acctg_acc->list[i].addr, ~ip_acctg_acc->list[i].mask);
	}
	return;
    }

    if (GETOBJ(int,1) == FALSE) {	/* FALSE if no ip address & mask were given */
	if (!csb->sense) {
	    free(ip_acctg_acc);
	    ip_acctg_acc = malloc(sizeof(*ip_acctg_acc));
	    if (ip_acctg_acc == NULL) {
		printf(nomemory);
	    } else {
		/* unneeded, since malloc() returns zeroed memory */
		/* ip_acctg_acc->count = 0; */
		/* ip_acctg_acc->transits_def = 0; */
		/* ip_acctg_acc->transits = 0; */
	    }
	}
	return;
    }
    addr = GETOBJ(paddr,1)->ip_addr;
    mask = GETOBJ(paddr,2)->ip_addr;
    addr = (addr & ~mask);
    if (!csb->sense) {			/* no ip accounting-list ... */
	u_short ix;

	for (ix = 0; ix < ip_acctg_acc->count; ++ix)
	    if (ip_acctg_acc->list[ix].addr == addr &&
		ip_acctg_acc->list[ix].mask == ~mask)
		goto hit;
	printf("\nCannot find %i %i in list", addr, mask);
	return;
hit:
	ip_acctg_acc->count--;
	for (; ix < ip_acctg_acc->count; ++ix) {
	    ip_acctg_acc->list[ix].addr = ip_acctg_acc->list[1 + ix].addr;
	    ip_acctg_acc->list[ix].mask = ip_acctg_acc->list[1 + ix].mask;
	}
    } else {
	u_short size, ix;
	ip_acctg_acc_type* newacc;

	for (ix = 0; ix < ip_acctg_acc->count; ++ix) {
	    if (ip_acctg_acc->list[ix].addr == addr) {
		ip_acctg_acc->list[ix].mask = ~mask;
		return;
	    }
	}
	size = sizeof(ip_acctg_acc_type) +
	    (sizeof(ip_acctg_acc_entry_type) * (1 + ip_acctg_acc->count));
	newacc = malloc(size);
	if (newacc == NULL) {
	    printf(nomemory);
	} else {
	    ip_acctg_acc_type* oldacc;

	    bcopy((u_char*) ip_acctg_acc, (u_char*) newacc,
		  size - sizeof(ip_acctg_acc_entry_type));
	    newacc->list[ip_acctg_acc->count].addr = addr;
	    newacc->list[ip_acctg_acc->count].mask = ~mask;
	    ++newacc->count;
	    oldacc = ip_acctg_acc;
	    ip_acctg_acc = newacc;
	    free(oldacc);
	}
    }
    return;
}

/*
 * ip_acctthreshold
 * "ip accounting-threshold" command
 */
void ip_acctthreshold (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write(ip_acctg_info.threshold != IPACCTGDEFAULTLIMIT,
		 "%s %u", csb->nv_command, ip_acctg_info.threshold);
	return;
    }
    ip_acctg_info.threshold = csb->sense ? GETOBJ(int,1) : IPACCTGDEFAULTLIMIT;
}

/*
 * ip_accttransits
 * "ip accounting-transits" command
 */
void ip_accttransits (parseinfo *csb)
{
    if (csb->nvgen) {
	nv_write((ip_acctg_acc->count != 0) &&
		 (ip_acctg_acc->transits_def != 0),
		 "%s %d", csb->nv_command, ip_acctg_acc->transits_def);
	return;
    }
    if (ip_acctg_acc == NULL) {
	return;
    }
    ip_acctg_acc->transits_def = csb->sense ? GETOBJ(int,1) : 0;
    ip_acctg_acc->transits     = csb->sense ? GETOBJ(int,1) : 0;
}

/*
 * ip_acct_fixup
 * Called when the IP accounting configuration is altered.
 */

void ip_acct_fixup (boolean running)
{
    idbtype *idb;
    ip_acctg_mem_type *ipam;
    int i;

    if (!running) {
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ip_output_acctg || idb->ip_violation_acctg) {
		running = TRUE;
		break;
	    }
	}
    }
    if (!running && ip_acctg_on) {
	++ip_acctg_info.generation;
	while ((ipam = dequeue(&ipmemQ))) {
	    free(ipam);
	}
	ip_acctg_on = FALSE;
	if (ip_acctg_acc != NULL)
	    ip_acctg_acc->transits = ip_acctg_acc->transits_def;
    } else if (running && !ip_acctg_on) {
	queue_init(&ipacctQ, 0);
	queue_init(&ipmemQ, 0);
	for (i = 0; i < IPACCTGARRAYSIZE; i++) {
	    ip_acctg_info.data[0].store[i] = 0;
	    ip_acctg_info.data[1].store[i] = 0;
	}
	ip_acctg_info.current = &ip_acctg_info.data[0];
	ip_acctg_info.backup = 0;
	ip_acctg_info.count = 0;
	ip_acctg_info.data[0].packets_tossed = 0;
	ip_acctg_info.data[0].bytes_tossed = 0;    
	ip_acctg_info.data[0].violations_tossed = 0;
	GET_TIMESTAMP(ip_acctg_info.data[0].when);
	ip_acctg_on = TRUE;
    }
}

/*
 * show_ipaccounting
 * Triggered by the command SHOW IP ACCOUNTING [<checkpoint>]
 * The optional parameter, [<checkpoint>], indicates whether
 * the totals to be displayed are the current totals or the
 * data saved when a clear ip accounting command is entered.
 */

static const char ipacctghdr[] = "   Source           Destination              Packets               Bytes";
static const char ipviolationshdr[] = "   Source           Destination              Packets               Bytes   ACL";
static const char detail[] = "\n %16i %16i %-17u %-19u";
static const char violations_detail[] = "\n %16i %16i %-17u %-19u %-5u";

void show_ipaccounting (parseinfo *csb)
{
    ip_acctg_entry_type *ipae;
    ip_acctg_data_type *ipad;
    boolean printing_violations = GETOBJ(int,2);
    int i;
    
    if (!ip_acctg_on)
	return;
    
    /*
     * The default i.e. no args passed, print current contents.
     */
    if (!GETOBJ(int,1)) {
	ipad = ip_acctg_info.current;
    } else {
	ipad = ip_acctg_info.backup;
    }

    /*
     * Print out the data.
     */
    if (ipad) {
	ulong generation;

	if (printing_violations) {
	    automore_enable(ipviolationshdr);
	} else {
	    automore_enable(ipacctghdr);
	}
	generation = ip_acctg_info.generation;
	for (i = 0; i < IPACCTGARRAYSIZE; i++) {
	    if(automore_quit())
		goto punt;
	    ipae = ipad->store[i];
	    while (ipae) {
		if (ip_acctg_info.generation != generation) {
		    printf("\n\n%% IP accounting data cleared "
			   "or checkpointed\n");
		    return;
		}
		if (!printing_violations) {
		    if (!ipae->access_violation) 
			printf(detail, ipae->srcadr, ipae->dstadr, 
			       ipae->packets, ipae->bytes);
		} else {
		    if (ipae->access_violation) 
			printf(violations_detail, ipae->srcadr, 
			       ipae->dstadr, ipae->packets, ipae->bytes, 
			       ipae->access_violation);
		}
		ipae = ipae->next;
	    }
            process_may_suspend();
	}
	
	/*
	 * Print out the number of packets and bytes discarded -- if any.
	 */
	printf("\n\nAccounting data age is %#TE", ipad->when);
	if (!printing_violations) {
	    if (ipad->packets_tossed)
		printf("\nAccounting threshold exceeded "
		       "for %u packets and %u bytes",
		       ipad->packets_tossed, ipad->bytes_tossed);
	} else {
	    if (ipad->violations_tossed)
		printf("\nAccounting threshold exceeded "
		       "for %u packets violating access list(s)",
		       ipad->violations_tossed);
	}
    punt:
	automore_disable();
    }
}

/*
 * clear_ip_accounting_func
 * Triggered by the command clear ip accounting <checkpoint> and snmp
 * checkpoint sets.
 *
 * The backup totals are always cleared.  If the optional parameter,
 * CHECKPOINT, is set, then the backup totals are cleared with no
 * further action.  If the CHECKPOINT parameter is not set, then the 
 * backup totals are cleared, then swapped with the current totals,
 * effectively saving the current totals, then clearing them.
 */
void clear_ip_accounting_func (boolean checkpoint)
{
    ushort i;
    ip_acctg_entry_type *ipae;
    ip_acctg_data_type *ipad;
    ip_acctg_mem_type *ipam, *next;
    
    if (!ip_acctg_on)			/* a little self-protection */
	return;
    
    ++ip_acctg_info.generation;

    /*
     * If a backup entry is present -- re-assemble fragment count.
     */
    if (ip_acctg_info.backup != 0) {
	ipad = ip_acctg_info.backup;
	for (i = 0; i < IPACCTGARRAYSIZE; i++) {
	    ipae = ipad->store[i];
	    while (ipae) {
		ipam = (ip_acctg_mem_type *) ipae->memory;
		ipam->fragments++;
		ipae = ipae->next;
	    }
	    ipad->store[i] = 0;
	}
	
	/*
	 * See if we can *prune* ipmemQ.
	 */
	ipam = (ip_acctg_mem_type *) ipmemQ.qhead;
	while (ipam) {
	    next = ipam->next;
	    if (ipam->fragments == IPACCTGENTRIESPERBLOCK) {
		unqueue (&ipmemQ, ipam);
		free (ipam);
	    }
	    ipam = next;
	}
	ip_acctg_info.backup = 0;
	
    }
    if (! checkpoint) {
	/*
	 * Just swap the accounting buffers, the next time we'll flush
	 * the actual data.
	 */
	ip_acctg_info.backup = ip_acctg_info.current;
	ipad = &ip_acctg_info.data[0];
	if (ipad == ip_acctg_info.current)
	    ipad = &ip_acctg_info.data[1];
	ipad->packets_tossed = 0;
	ipad->bytes_tossed = 0;
	ipad->violations_tossed = 0;
	GET_TIMESTAMP(ipad->when);
	ip_acctg_info.current = ipad;
	ip_acctg_info.count = 0;
	if (ip_acctg_acc != NULL)
	    ip_acctg_acc->transits = ip_acctg_acc->transits_def;
    }
}

/*
 * clear_ip_accounting
 * Triggered by the command clear ip accounting <checkpoint>
 *
 * The backup totals are always cleared.  If the optional parameter,
 * CHECKPOINT, is set, then the backup totals are cleared with no
 * further action.  If the CHECKPOINT parameter is not set, then the 
 * backup totals are cleared, then swapped with the current totals,
 * effectively saving the current totals, then clearing them.
 */

void clear_ip_accounting (parseinfo *csb)
{
    clear_ip_accounting_func((boolean) GETOBJ(int,1));
}
