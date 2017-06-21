/* $Id: vines_access.c,v 3.4.12.1 1996/03/18 22:31:35 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/vines/vines_access.c,v $
 *------------------------------------------------------------------
 * vines_access.c -- Banyan Vines access list support routines
 *
 * February 1990, David R. Hampton
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_access.c,v $
 * Revision 3.4.12.1  1996/03/18  22:31:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.3  1996/03/16  07:52:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.16.2  1996/03/07  11:04:17  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/02/20  21:46:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/16  02:50:40  ehk
 * CSCdi48679:  Vines extended access-lists disappearing after a reload
 *
 * due to sign extension of source and destination port masks,
 * parser rejects user-configured access lists. this fix will work
 * for any newly configured access lists, but won't retrofit previously
 * configured VINES extended access lists. so user has to reconfigure
 * access lists.
 *
 * Revision 3.3  1995/12/04  20:32:04  slin
 * CSCdi44873:  show vines access can halt system
 *
 * Revision 3.2  1995/11/17  17:57:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:14:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "vines_private.h"
#include "vines_ipc.h"

vinesacctype *vines_access_lists[VINESMAXACC+1];


/*
 * vines_get_old_access_list
 * Parse vines access list command.
 *
 * Normal access lists are:
 *      vines access-list <number> <permit | deny> <IP | ARP | ICP | RTP>
 *	<source network:addr> <source mask>
 *	<destination network:addr> <destination mask>
 *
 * Extended access lists are:
 *      vines access-list <number> <permit | deny> <IPC | SPP>
 *      <source network:addr> <source mask> <source port>
 *      <destination network:addr> <destination mask> <destination port>
 *
 * New extended access lists are:
 *      vines access-list <number> <permit | deny> <IPC | SPP>
 *      <source addr> <source mask> <source port> <source port mask>
 *      <dest addr> <dest mask> <dest port> <dest port mask>
 *
 */

void vines_access_command (parseinfo *csb)
{
    vinesacctype *item, *wrk_item, *entry;
    int number;
    boolean grant, port_numbers;
    
    /*
     * get the access list number
     */
    number = GETOBJ(int,1);

    if (csb->sense == FALSE) {	
	/*
	 * No access-list command.  Delink entries and free memory.
	 */
	item = (vinesacctype *) vines_access_lists[number];
	vines_access_lists[number] = NULL;
	while (item) {
	    /*
	     * Temporary patch to prevent router crash due to memory
	     * corruption (CSCdi49737). Get rid of validmem() once
	     * the cause is identified.
	     */
	    if (!validmem(item))
		break;
	    wrk_item = item->next;
	    free(item);
	    item = wrk_item;
	}
	return;
    }
    
    /*
     * Get the access type, either "permit" or "deny".
     * Skip this if deleting an access list.
     */
    grant = GETOBJ(int,2);

    entry = malloc(sizeof(vinesacctype));
    if (!entry)
	return;
    entry->grant = grant;

    /*
     * get the protocol type -- if the protocol is anything other than
     * "ip" then this becomes an extended list.
     */
    entry->protocol = GETOBJ(int,3);
    port_numbers = ((entry->protocol == VINES_IPC) ||
		    (entry->protocol == VINES_SPP));    

    /*
     * get source address and socket number
     */
    entry->srcnet = GETOBJ(paddr,1)->vines_addr.net;
    entry->srchost = GETOBJ(paddr,1)->vines_addr.host;
    entry->smasknet = ~GETOBJ(paddr,2)->vines_addr.net;
    entry->smaskhost = ~GETOBJ(paddr,2)->vines_addr.host;

    if (port_numbers) {
	entry->srcsocket = GETOBJ(int,4);
	if ((number >= VINESMINEACC) && (number <= VINESMAXEACC))
	    entry->smasksock = ~GETOBJ(int,5);
	else
	    entry->smasksock = ~0;
    }

    /*
     * get destination address and socket number
     */
    entry->dstnet = GETOBJ(paddr,3)->vines_addr.net;
    entry->dsthost = GETOBJ(paddr,3)->vines_addr.host;
    entry->dmasknet = ~GETOBJ(paddr,4)->vines_addr.net;
    entry->dmaskhost = ~GETOBJ(paddr,4)->vines_addr.host;

    if (port_numbers) {
	entry->dstsocket = GETOBJ(int,6);
	if ((number >= VINESMINEACC) && (number <= VINESMAXEACC))
	    entry->dmasksock = ~GETOBJ(int,7);
	else
	    entry->dmasksock = ~0;
    }

    /*
     * fix up the entries so we don't have impossible matches in the list.
     */
    entry->srcnet    &= entry->smasknet;
    entry->srchost   &= entry->smaskhost;
    entry->srcsocket &= entry->smasksock;
    entry->dstnet    &= entry->dmasknet;
    entry->dsthost   &= entry->dmaskhost;
    entry->dstsocket &= entry->dmasksock;

    /*
     * Check to ensure that we don't add a duplicate entry.
     */
    wrk_item = vines_access_lists[number];
    for (item = wrk_item; item; wrk_item = item, item = item->next) {
	/*
	 * Temporary patch to prevent router crash due to memory
	 * corruption (CSCdi49737). Get rid of validmem() once
	 * the cause is identified.
	 */
	if (!validmem(wrk_item) ||
	    (bcmp(&entry->grant, (uchar *)&wrk_item->grant,
		  sizeof(vinesacctype) - sizeof(item->next)) == 0)) {
	    free(entry);
	    return;
	}
    }
    
    /*
     * Finally add the new item to the list.
     */
    if (wrk_item == NULL)
	vines_access_lists[number] =  entry;
    else
	wrk_item->next = entry;	    
}

/*
 * vines_access_check
 *
 * Determine if we are allowed to forward the passed datagram, based on the
 * VINES access pointer.  Return FALSE if output access is denied.  We
 * do not have to worry about fragmentation issues because we will
 * never handle fragmented VINES packets.
 */
boolean vines_access_check (
    paktype *pak,
    vinesacctype *item)
{
    vinesiptype *vip;
    vinesipctype *ipc;
    
    vip = (vinesiptype *)pak->network_start;
    for ( ; item; item = item->next) {
	/*
	 * Temporary patch to prevent router crash due to memory
	 * corruption (CSCdi49737). Get rid of validmem() once
	 * the cause is identified.
	 */
	if (!validmem(item))
	    break;
	/*
	 * First check the source network.
	 */
	if ((GET2WD(vip->ssrcnet) & item->smasknet) != item->srcnet)
	    continue;
	if ((vip->srchost & item->smaskhost) != item->srchost)
	    continue;

	/*
	 * Then check the destination network.
	 */
	if ((GET2WD(vip->ddstnet) & item->dmasknet) != item->dstnet)
	    continue;
	if ((vip->dsthost & item->dmaskhost) != item->dsthost)
	    continue;

	/*
	 * last check any of the extended stuff.
	 */
	if (item->protocol) {
	    if (vip->protocol != item->protocol)
		continue;
	    if ((item->protocol == VINES_IPC) || (item->protocol == VINES_SPP)) {
		ipc = (vinesipctype *) vip->data;
		if ((ipc->srcport & item->smasksock) != item->srcsocket)
		    continue;
		if ((ipc->dstport & item->dmasksock) != item->dstsocket)
		    continue;
	    }
	}
	return(item->grant);
    }
    return(FALSE);
}

/*
 * vines_accesscheck
 *
 * Determine if we are allowed to forward the passed datagram, based on the
 * VINES access pointer.  Return FALSE if output access is denied.  We
 * do not have to worry about fragmentation issues because we will
 * never handle fragmented VINES packets.
 */
boolean vines_accesscheck (paktype *pak, int accesslist)
{
    vinesacctype *item;
    
    if (accesslist > VINESMAXACC)
	return(TRUE);
    item = (vinesacctype *) vines_access_lists[accesslist];
    if (item == NULL)
	return TRUE;

    return(vines_access_check(pak, item));
}
    
/*
 * vines_short_access_check
 *
 * Determine if we are allowed to take a given action.  Return FALSE if
 * the action should be denied.
 */
boolean vines_short_access_check (
    ulong net,
    ushort host,
    int accesslist)
{
    vinesacctype *item;

    if (!accesslist)
	return(TRUE);
    if ((accesslist < VINESMINSACC) || (accesslist >  VINESMAXSACC))
	return(FALSE);
    item = (vinesacctype *) vines_access_lists[accesslist];
    if (item == NULL)
	return(FALSE);

    for ( ; item; item = item->next) {
	/*
	 * Temporary patch to prevent router crash due to memory
	 * corruption (CSCdi49737). Get rid of validmem() once
	 * the cause is identified.
	 */
	if (!validmem(item))
	    break;
	/*
	 * First check the source network.
	 */
	if ((net & item->smasknet) != item->srcnet)
	    continue;
	if ((host & item->smaskhost) != item->srchost)
	    continue;
	return(item->grant);
    }
    return(FALSE);
}

/*
 * vines_access_str
 *
 * Write an access list item into a buffer.  This insures consistency
 * between writing "write terminal" and "show access"
 */
#define vines_4char_host(hhhh) ((hhhh) & 0xffff)

char *vines_access_str (char *buff, int listno, vinesacctype *item)
{
    if (!buff)
	return(NULL);

    sprintf(buff, "%s", (item->grant ? "permit" : "deny  "));

    /*
     * Short
     */
    if ((listno >= VINESMINSACC) && (listno <= VINESMAXSACC)) {
	sprintf(buff+strlen(buff), " %16z %16z",
		item->srcnet, vines_4char_host(item->srchost),
		~item->smasknet, vines_4char_host(~item->smaskhost));
	return(buff);
    }

    /*
     * Normal and extended
     */
    if (item->protocol < NVINESIPTYPES)
	sprintf(buff+strlen(buff), " %3s", vines_ip_types[item->protocol]);
    else
	sprintf(buff+strlen(buff), " %3d", item->protocol);
    
    if ((item->protocol != VINES_IPC) && (item->protocol != VINES_SPP)) {
	sprintf(buff+strlen(buff), " %16z %16z %16z %16z",
		item->srcnet, vines_4char_host(item->srchost),
		~item->smasknet, vines_4char_host(~item->smaskhost),
		item->dstnet, vines_4char_host(item->dsthost),
		~item->dmasknet, vines_4char_host(~item->dmaskhost));
    } else {
	if ((listno >= VINESMINOACC) && (listno <= VINESMAXOACC)) {
	    sprintf(buff+strlen(buff),
		    " %16z %16z %04x %16z %16z %04x",
		    item->srcnet, vines_4char_host(item->srchost),
		    ~item->smasknet, vines_4char_host(~item->smaskhost),
		     item->srcsocket, item->dstnet,
		    vines_4char_host(item->dsthost), ~item->dmasknet,
		    vines_4char_host(~item->dmaskhost), item->dstsocket);
	} else if ((listno >= VINESMINEACC) && (listno <= VINESMAXEACC)) {
	    sprintf(buff+strlen(buff),
		    " %16z %16z %04x %04x %16z %16z %04x %04x",
		    item->srcnet, vines_4char_host(item->srchost),
		    ~item->smasknet, vines_4char_host(~item->smaskhost),
		    item->srcsocket, ((~item->smasksock) & 0xffff),
		    item->dstnet, vines_4char_host(item->dsthost),
		    ~item->dmasknet, vines_4char_host(~item->dmaskhost),
		    item->dstsocket, ((~item->dmasksock) & 0xffff));
	} else {
	    sprintf(buff+strlen(buff), " oops. listno is %d, item is %x.",
		    listno, item);
	}
    }
    return(buff);
}

/*
 * vines_access_display
 *
 * Function to display the VINES access lists.
 */
void vines_access_display (int list)
{
    int i;
    vinesacctype *p, *pred;
    char buffer[110];
    uint DisplayBufSize;
    char *displaybuf;
    
    displaybuf = malloc(VINESBUFLEN);
    if (displaybuf == NULL)
	return;
    DisplayBufSize = VINESBUFLEN;
    
    /*
     * The user asked for a particular access list.
     */
    if (list != -1) {
	if ((list <= 0) || (list > VINESMAXACC)) {
	    printf("bad list number");
	    free(displaybuf);
	    return;
	}
	safe_sprintf(&displaybuf, &DisplayBufSize,
		     "\nVINES access list %d", list);
	pred = (vinesacctype *)&vines_access_lists[list];
	for (p = pred->next ; p; pred = p, p = pred->next) {
	    /*
	     * Temporary patch to prevent router crash due to memory
	     * corruption (CSCdi49737). Get rid of validmem() once
	     * the cause is identified.
	     */
	    if (validmem(p)) {
		safe_sprintf(&displaybuf, &DisplayBufSize, "\n    %s", 
			     vines_access_str(buffer, list, p));
	    } else {
		safe_sprintf(&displaybuf, &DisplayBufSize,
			     "\n    bad list %d(0x%08x) at 0x%08x",
			     list, p, pred);
		break;
	    }
	}	    
	automore_enable(NULL);
	printf("%s", displaybuf);
	automore_disable();
	free(displaybuf);
	return;
    }

    /*
     * Print all of the access lists.
     */
    for (i = 1 ; i <= VINESMAXACC; i++) {
	pred = (vinesacctype *)&vines_access_lists[i];
	p = pred->next;
	if (p) {
	    safe_sprintf(&displaybuf, &DisplayBufSize,
			 "\nVINES access list %d", i);
	    for ( ; p; pred = p, p = pred->next) {
		/*
		 * Temporary patch to prevent router crash due to memory
		 * corruption (CSCdi49737). Get rid of validmem() once
		 * the cause is identified.
		 */
		if (validmem(p)) {
		    safe_sprintf(&displaybuf, &DisplayBufSize, "\n    %s",
				 vines_access_str(buffer, i, p));
		} else {
		    safe_sprintf(&displaybuf, &DisplayBufSize,
				 "\n    bad list %d(0x%08x) at 0x%08x",
				 i, p, pred);
		    break;
		}
	    }	    
	}
    }
    automore_enable(NULL);
    printf("%s", displaybuf);
    automore_disable();
    free(displaybuf);
}
