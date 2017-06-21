/* $Id: ipxaccount.c,v 3.5.12.3 1996/08/07 09:05:03 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xns/ipxaccount.c,v $
 *------------------------------------------------------------------
 * ipxaccount.c -- accumulate "accounting" data on ipx packets.
 *
 * September, 1993 Joel P. Bion
 *
 * Code brazenly lifted from ipaccount.c 
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipxaccount.c,v $
 * Revision 3.5.12.3  1996/08/07  09:05:03  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.12.2  1996/06/28  23:33:19  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.5.12.1  1996/03/18  22:52:41  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.22.2  1996/03/13  02:14:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.22.1  1996/02/20  21:59:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/12  22:21:37  dko
 * CSCdi48651:  no ipx routing doesnt remove ipx accounting cmd.
 * Insert generic check for if novell is running or shutting down.
 * Clean up a leftover debugging bugtrace.
 *
 * Revision 3.4  1995/11/20  23:53:26  mschaef
 * CSCdi43322:  Bus Error Reboot in _nov_acct_and_access_check_mci
 *
 * Revision 3.3  1995/11/17  19:21:59  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:58:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:38:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/01  03:38:33  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.1  1995/06/07 23:27:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../h/config.h"
#include "novell.h"
#include "novfast.h"
#include "logger.h"
#include "../os/free.h"

/*
 * Local storage
 */
ipx_acctg_info_type ipx_acctg_info;	/* accounting data structure */
ipx_acctg_acc_type *ipx_acctg_acc;	/* access information for networks */
queuetype ipxacctQ;			/* local queue of memory */
queuetype ipxmemQ;			/* REAL memory obtained */
boolean ipx_acctg_on;                   /* whether we do accounting */

/*
 * ipx_acct_init
 * Initialize variables for IP accounting
 */

void ipx_acct_init (void)
{
    ipx_acctg_info.generation = 1;
    ipx_acctg_info.threshold = IPXACCTGDEFAULTLIMIT;
    ipx_acctg_acc = malloc(sizeof(ipx_acctg_acc_type));
    if (ipx_acctg_acc == NULL) {
	printf(nomemory);
    } else {
	/* unneeded, since malloc() returns zeroed memory */
	/* ipx_acctg_acc->count = 0; */
	/* ipx_acctg_acc->transits_def = 0; */
	/* ipx_acctg_acc->transits = 0; */
    }
    ipx_acctg_on = FALSE;
}

/*
 * ipx_accumulate_acctg
 * Called from ipxsendnet() when IPX accounting is enabled on an interface.
 */
void ipx_accumulate_acctg (ipxhdrtype *ipx)
{
    ipx_acctg_entry_type *ipxae;
    long slot;
    ipx_acctg_mem_type *ipxam;
    uint i;
    ulong srcnet, dstnet;
    mac_address_type srchost, dsthost;
    ipx_acctg_acc_entry_type *acc_entry;
    leveltype SR;
    
    srcnet = GET2WD(ipx->ssnet);
    dstnet = GET2WD(ipx->ddnet);
    ieee_copy(ipx->shost, (uchar *) &srchost);
    ieee_copy(ipx->dhost, (uchar *) &dsthost);
    if (ipx_acctg_acc != NULL) {
	for (i = 0, acc_entry = &ipx_acctg_acc->list[0]; 
	     i < ipx_acctg_acc->count; acc_entry++, i++) {
	    /*
	     * This complex expression simply applies an IPX net.host access
	     * list first to the source address, then the destination address.
	     * If either pass the access list entry, a 'hit' is found.
	     */
	    if ((((srcnet & ~acc_entry->mask_net) == acc_entry->addr_net) &&
		 ((srchost.u.lword[0] & ~acc_entry->mask_host.u.lword[0]) ==
		  acc_entry->addr_host.u.lword[0]) &&
		 ((srchost.u.sword[2] & ~acc_entry->mask_host.u.sword[2]) ==
		  acc_entry->addr_host.u.sword[2])) ||
		(((dstnet & ~acc_entry->mask_net) == acc_entry->addr_net) &&
		 ((dsthost.u.lword[0] & ~acc_entry->mask_host.u.lword[0]) ==
		  acc_entry->addr_host.u.lword[0]) &&
		 ((dsthost.u.sword[2] & ~acc_entry->mask_host.u.sword[2]) ==
		  acc_entry->addr_host.u.sword[2]))) {
		goto hit;
	    }
	}
	if (ipx_acctg_acc->count == 0)
	    goto hit;
	if (ipx_acctg_acc->transits == 0)
	    return;
	ipx_acctg_acc->transits--;
    }
hit:
    slot = srcnet;
    slot ^= dstnet;
    slot ^= (slot >> 16);
    slot ^= (slot >>8);
    slot &= 0xFF;
    SR = raise_interrupt_level(NETS_DISABLE);
    ipxae = ipx_acctg_info.current->store[slot];
    while (ipxae) {
	if ((ipxae->srcnet == srcnet) && (ipxae->dstnet == dstnet) &&
	    (ieee_equal(MAC_ADDRESS_TYPE_TO_UCHAR(srchost), ipxae->srchost)) &&
	    (ieee_equal(MAC_ADDRESS_TYPE_TO_UCHAR(dsthost), ipxae->dsthost))) {
	    ipxae->packets++;
	    ipxae->bytes += GETSHORT(&ipx->len);
	    reset_interrupt_level(SR);
	    return;
	}
	ipxae = ipxae->next;
    }
    reset_interrupt_level(SR);
    
    /*
     * See if count exceeds the configured threshold.
     */
    if (ipx_acctg_info.count < ipx_acctg_info.threshold) {
	SR = raise_interrupt_level(NETS_DISABLE);	/* lock out fastswitch accumulate */
	ipxae = dequeue(&ipxacctQ);
	reset_interrupt_level(SR);
	if (ipxae == NULL) {
	    /*
	     * None on queue -- see if any memory blocks are *empty*
	     */
	    for (ipxam = (ipx_acctg_mem_type *) ipxmemQ.qhead; ipxam;
		 ipxam = ipxam->next) {
		if (ipxam->fragments == IPXACCTGENTRIESPERBLOCK) {
		    ipxam->fragments = 0;
		    break;
		}
	    }
	    if (!ipxam && !mempool_is_empty(MEMPOOL_CLASS_LOCAL)) {
		/*
		 * No empty memory blocks found -- get more memory.
		 * Give back memory if we are running the system too low.
		 */
		ipxam = malloc(sizeof(ipx_acctg_mem_type));
		if (ipxam)
		    enqueue(&ipxmemQ, ipxam);
	    }
	    if (ipxam) {
		ipxae = &ipxam->entry[0];
		for (i = 0; i <IPXACCTGENTRIESPERBLOCK; i++, ipxae++) {
		    ipxae->memory = (ulong *) ipxam;
		    SR = raise_interrupt_level(NETS_DISABLE);
		    enqueue(&ipxacctQ, ipxae);
		    reset_interrupt_level(SR);
		}
	    }
	    SR = raise_interrupt_level(NETS_DISABLE);
	    ipxae = dequeue(&ipxacctQ);
	    reset_interrupt_level(SR);
	}
	if (ipxae) {
	    ipxae->srcnet = srcnet;
	    ipxae->dstnet = dstnet;
	    ieee_copy(MAC_ADDRESS_TYPE_TO_UCHAR(srchost), ipxae->srchost);
	    ieee_copy(MAC_ADDRESS_TYPE_TO_UCHAR(dsthost), ipxae->dsthost);
	    ipxae->bytes = GETSHORT(&ipx->len);
	    ipxae->packets = 1;
	    SR = raise_interrupt_level(NETS_DISABLE);
	    ipxae->next = ipx_acctg_info.current->store[slot];
	    ipx_acctg_info.current->store[slot] = ipxae;
	    reset_interrupt_level(SR);
	    ipx_acctg_info.count++;
	    return;
	}
    }
    
    /*
     * If we get here -- we where unable to save the information.
     */
    ipx_acctg_info.current->packets_tossed++;
    ipx_acctg_info.current->bytes_tossed += GETSHORT(&ipx->len);
}

/*
 * ipx_acctlist
 * Handle the [no] ipx accounting-list ipxaddress mask command
 */
void ipx_acctlist (parseinfo *csb)
{
    int i;
    ulong addrnet, masknet;
    mac_address_type addrhost, maskhost;

    if (ipx_acctg_acc == NULL)
	return;

    if (csb->nvgen) {
	for (i = 0; i < ipx_acctg_acc->count; i++) {
	    nv_write(TRUE, "%s %x.%e %x.%e", csb->nv_command,
		     ipx_acctg_acc->list[i].addr_net,
		     MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[i].addr_host),
		     ipx_acctg_acc->list[i].mask_net,
		     MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[i].mask_host));
	}
	return;
    }

    /* FALSE if no ipx address & mask were given */
    if (GETOBJ(int,1) == FALSE) {
	if (!csb->sense) {
	    free(ipx_acctg_acc);
	    ipx_acctg_acc = (ipx_acctg_acc_type*)
	      malloc(sizeof(*ipx_acctg_acc));
	    if (ipx_acctg_acc == NULL) {
		printf(nomemory);
	    } else {
		/* unneeded, since malloc() returns zeroed memory */
		/* ipx_acctg_acc->count = 0; */
		/* ipx_acctg_acc->transits_def = 0; */
		/* ipx_acctg_acc->transits = 0; */
	    }
	}
	return;
    }
    addrnet = GETOBJ(paddr,1)->xns_addr.net;
    ieee_copy((uchar *)GETOBJ(paddr,1)->xns_addr.host, (uchar *)&addrhost);
    masknet = GETOBJ(paddr,2)->xns_addr.net;
    ieee_copy((uchar *)GETOBJ(paddr,2)->xns_addr.host, (uchar *)&maskhost);
    if (!csb->sense) {			/* no ipx accounting-list ... */
	u_short ix;

	for (ix = 0; ix < ipx_acctg_acc->count; ++ix)
	    if ((ipx_acctg_acc->list[ix].addr_net == addrnet) &&
		ieee_equal(MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[ix].addr_host),
			   MAC_ADDRESS_TYPE_TO_UCHAR(addrhost)) &&
		(ipx_acctg_acc->list[ix].mask_net == masknet) &&
		ieee_equal(MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[ix].mask_host),
			   MAC_ADDRESS_TYPE_TO_UCHAR(maskhost)))
		goto hit;
	printf("\nCannot find %x.%e %x.%e in list", addrnet, addrhost, masknet,
	       maskhost);
	return;
hit:
	ipx_acctg_acc->count--;
	for (; ix < ipx_acctg_acc->count; ++ix) {
	    ipx_acctg_acc->list[ix].addr_net =
	      ipx_acctg_acc->list[1 + ix].addr_net;
	    ieee_copy(MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[1 + ix].addr_host),
		      MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[ix].addr_host));
	    ipx_acctg_acc->list[ix].mask_net =
	      ipx_acctg_acc->list[1 + ix].mask_net;
	    ieee_copy(MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[1 + ix].mask_host),
		      MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[ix].mask_host));
	}
    } else {
	u_short size, ix;
	ipx_acctg_acc_type* newacc;

	for (ix = 0; ix < ipx_acctg_acc->count; ++ix) {
	    if ((ipx_acctg_acc->list[ix].addr_net == addrnet) &&
		ieee_equal(MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[ix].addr_host),
			   MAC_ADDRESS_TYPE_TO_UCHAR(addrhost))) {
		ipx_acctg_acc->list[ix].mask_net = masknet;
		ieee_copy(MAC_ADDRESS_TYPE_TO_UCHAR(maskhost),
			  MAC_ADDRESS_TYPE_TO_UCHAR(ipx_acctg_acc->list[ix].mask_host));
		return;
	    }
	}
	size = sizeof(ipx_acctg_acc_type) +
	    (sizeof(ipx_acctg_acc_entry_type) * (1 + ipx_acctg_acc->count));
	newacc = malloc(size);
	if (newacc == NULL) {
	    printf(nomemory);
	} else {
	    ipx_acctg_acc_type* oldacc;

	    bcopy((u_char*) ipx_acctg_acc, (u_char*) newacc,
		  size - sizeof(ipx_acctg_acc_entry_type));
	    newacc->list[ipx_acctg_acc->count].addr_net = addrnet;
	    ieee_copy(MAC_ADDRESS_TYPE_TO_UCHAR(addrhost),
		      MAC_ADDRESS_TYPE_TO_UCHAR(newacc->list[ipx_acctg_acc->count].addr_host));
	    newacc->list[ipx_acctg_acc->count].mask_net = masknet;
	    ieee_copy(MAC_ADDRESS_TYPE_TO_UCHAR(maskhost),
		      MAC_ADDRESS_TYPE_TO_UCHAR(newacc->list[ipx_acctg_acc->count].mask_host));
	    ++newacc->count;
	    oldacc = ipx_acctg_acc;
	    ipx_acctg_acc = newacc;
	    free(oldacc);
	}
    }
    return;
}

/*
 * ipx_acct_fixup
 * Called when the IPX accounting configuration is altered.
 */
void ipx_acct_fixup (boolean running)
{
    idbtype *idb;
    ipx_acctg_mem_type *ipxam;
    int i;

    if (!running) {
	FOR_ALL_SWIDBS(idb) {
	    if (idb->ipx_accounting) {
		running = TRUE;
		break;
	    }
	}
    }
    if (!running && ipx_acctg_on) {
	++ipx_acctg_info.generation;
	while ((ipxam = dequeue(&ipxmemQ)))
	    free(ipxam);
	ipx_acctg_on = FALSE;
	if (ipx_acctg_acc != NULL)
	    ipx_acctg_acc->transits = ipx_acctg_acc->transits_def;
    } else if (running && !ipx_acctg_on) {
	queue_init(&ipxacctQ, 0);
	queue_init(&ipxmemQ, 0);
	for (i = 0; i < IPXACCTGARRAYSIZE; i++) {
	    ipx_acctg_info.data[0].store[i] = 0;
	    ipx_acctg_info.data[1].store[i] = 0;
	}
	ipx_acctg_info.current = &ipx_acctg_info.data[0];
	ipx_acctg_info.backup = 0;
	ipx_acctg_info.count = 0;
	ipx_acctg_info.data[0].packets_tossed = 0;
	ipx_acctg_info.data[0].bytes_tossed = 0;    
	GET_TIMESTAMP(ipx_acctg_info.data[0].when);
	ipx_acctg_on = TRUE;
    }
}

/*
 * show_ipxaccounting
 * Triggered by the command SHOW IPX ACCOUNTING [<checkpoint>]
 * The optional parameter, [<checkpoint>], indicates whether
 * the totals to be displayed are the current totals or the
 * data saved when a clear ipx accounting command is entered.
 */
static const char ipxacctghdr[] = " Source                  Destination                Packets           Bytes";
static const char detail[] = "\n %08x.%e %08x.%e %-10u %-15u";

void show_ipxaccounting (parseinfo *csb)
{
    ipx_acctg_entry_type *ipxae;
    ipx_acctg_data_type *ipxad;
    int i;
    
    if (!ipx_acctg_on)
	return;
    
    /*
     * The default i.e. no args passed, print current contents.
     */
    if (!GETOBJ(int,1)) {
	ipxad = ipx_acctg_info.current;
    } else {
	ipxad = ipx_acctg_info.backup;
    }

    /*
     * Print out the data.
     */
    if (ipxad) {
	ulong generation;

	automore_enable(ipxacctghdr);
	generation = ipx_acctg_info.generation;
	for (i = 0; i < IPXACCTGARRAYSIZE; i++) {
	    ipxae = ipxad->store[i];
	    while (ipxae) {
		if (ipx_acctg_info.generation != generation) {
		    printf("\n\n%% IPX accounting data cleared "
			   "or checkpointed\n");
		    return;
		}
		printf(detail,
		       ipxae->srcnet, ipxae->srchost, ipxae->dstnet,
		       ipxae->dsthost, ipxae->packets, ipxae->bytes);
		ipxae = ipxae->next;
	    }
	    /*
	     * Make sure fastswitch acl flags are valid when restarting:
	     */
	    nov_update_global_acl_flags();
	}
	
	/*
	 * Print out the number of packets and bytes discarded -- if any.
	 */
	printf("\n\nAccounting data age is %#TE", ipxad->when);
	if (ipxad->packets_tossed)
	    printf("\nAccounting threshold exceeded "
		   "for %u packets and %u bytes",
		   ipxad->packets_tossed, ipxad->bytes_tossed);
	automore_disable();
    }
}

void clear_ipx_accounting_func (boolean checkpoint)
{
    ushort i;
    ipx_acctg_entry_type *ipxae;
    ipx_acctg_data_type *ipxad;
    ipx_acctg_mem_type *ipxam, *next;
    
    if (!ipx_acctg_on)			/* a little self-protection */
	return;
    
    ++ipx_acctg_info.generation;

    /*
     * If a backup entry is present -- re-assemble fragment count.
     */
    if (ipx_acctg_info.backup != 0) {
	ipxad = ipx_acctg_info.backup;
	for (i = 0; i < IPXACCTGARRAYSIZE; i++) {
	    ipxae = ipxad->store[i];
	    while (ipxae) {
		ipxam = (ipx_acctg_mem_type *) ipxae->memory;
		ipxam->fragments++;
		ipxae = ipxae->next;
	    }
	    ipxad->store[i] = 0;
	}
	
	/*
	 * See if we can *prune* ipxmemQ.
	 */
	ipxam = (ipx_acctg_mem_type *) ipxmemQ.qhead;
	while (ipxam) {
	    next = ipxam->next;
	    if (ipxam->fragments == IPXACCTGENTRIESPERBLOCK) {
		unqueue (&ipxmemQ, ipxam);
		free (ipxam);
	    }
	    ipxam = next;
	}
	ipx_acctg_info.backup = 0;
	
    }
    if (! checkpoint) {
	/*
	 * Just swap the accounting buffers, the next time we'll flush
	 * the actual data.
	 */
	ipx_acctg_info.backup = ipx_acctg_info.current;
	ipxad = &ipx_acctg_info.data[0];
	if (ipxad == ipx_acctg_info.current)
	    ipxad = &ipx_acctg_info.data[1];
	ipxad->packets_tossed = 0;
	ipxad->bytes_tossed = 0;
	GET_TIMESTAMP(ipxad->when);
	ipx_acctg_info.current = ipxad;
	ipx_acctg_info.count = 0;
	if (ipx_acctg_acc != NULL)
	    ipx_acctg_acc->transits = ipx_acctg_acc->transits_def;
    }
}

/*
 * clear_ipx_accounting
 * Triggered by the command clear ipx accounting <checkpoint>
 *
 * The backup totals are always cleared.  If the optional parameter,
 * CHECKPOINT, is set, then the backup totals are cleared with no
 * further action.  If the CHECKPOINT parameter is not set, then the 
 * backup totals are cleared, then swapped with the current totals,
 * effectively saving the current totals, then clearing them.
 */
void clear_ipx_accounting (parseinfo *csb)
{
    clear_ipx_accounting_func((boolean) GETOBJ(int,1));
}


/*
 * ipx accounting
 */
void ipx_accounting_command (parseinfo *csb)
{
    idbtype *idb = csb->interface;
    hwidbtype *hwidb;

    if (!novell_running || novell_shutting_down)
	return;

    if (csb->nvgen) {
	nv_write(idb->ipx_accounting, csb->nv_command);
	return;
    }
    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL) {
	printf("\nIPX: Cannot find hardware IDB for interface. Accounting status not changed.");
	return;
    }
    if (hwidb->novell_routecache & NOVELL_RCACHE_CBUS) {
	printf("\n%%IPX autonomous switching must be disabled "
	       "on %s", hwidb->hw_namestring);
	return;
    }
    /*
     * Be careful about how and when we actually enable/disable accounting
     * on the interface. When going from OFF to ON, enable it *after* data
     * structure initialization. When going from ON to OFF, disable it
     * *before* data structure deallocation. Failure to follow these rules
     * may result in interrupt level code attempting to access data
     * structures being initialized/deallocated at process level by
     * ipx_acct_fixup().
     */
    
    if (!csb->sense)
 	idb->ipx_accounting = csb->sense;
    
    ipx_acct_fixup(csb->sense);
    
    idb->ipx_accounting = csb->sense;
    nov_update_global_acl_flags(); /* Always update fastswitch flags */
}
