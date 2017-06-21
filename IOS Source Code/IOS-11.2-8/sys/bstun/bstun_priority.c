/* $Id: bstun_priority.c,v 3.2.60.3 1996/09/10 08:48:44 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bstun_priority.c,v $
 *------------------------------------------------------------------
 * bstun_priority.c -- Priority Queueing Support for BSTUN
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bstun_priority.c,v $
 * Revision 3.2.60.3  1996/09/10  08:48:44  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/05/17  10:41:04  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.3  1996/04/26  07:32:37  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.2  1996/04/11  08:19:49  jbalestr
 * CSCdi51807:  need a test command to transmit raw bisync frames
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:50:42  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:01:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:39:14  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/01/24  21:18:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  08:49:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/02  05:15:00  sberl
 * CSCdi36581:  cls.h should be cls_private.h
 * Remove references to cls.h from everything that is really a DLU. Change
 * name of cls.h to cls_private.h.
 *
 * Revision 2.2  1995/06/18  21:20:45  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:14:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "address.h"
#include "interface.h"
#include "interface_generic.h"
#include "packet.h"
#include "config.h"

#include "bstun.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../if/network_debug.h"
#include "../if/priority.h"
#include "bstun_public.h"

/*
 * bstun_prioritization
 * BSC address prioritization
 */

boolean bstun_prioritization (plisttype *plist, paktype *pak, hwidbtype *output)
{
    bstunhdrtype *sth;
    unsigned addrlen;

    addrlen = bstun_pinfo[plist->bstun_group]->addrlength;
    sth = (bstunhdrtype *)(pak->network_start);
    if ((plist->bstun_group == sth->group) &&  
	(!bcmp((uchar *)&plist->bsc_addr[0], &sth->addr[0], addrlen))){
	if (priority_debug)
	    buginf("\nPQ: %s: %s -> %s", output->hw_namestring,
		   print_linktype(pak->linktype),
		   idb_queue2name(output, plist->priority));
	return(TRUE);
    }
    return(FALSE);
}

/*
 * bstun_addr_pri_tcp
 * handles bstun address prioritization on TCP transport
 */

int bstun_addr_pri_tcp (hwidbtype *output, paktype *pak, int *priority)
{
    plisttype *plist;
    pritype *p;
    bstunhdrtype *sth=NULL;
    unsigned addrlen=0;

    sth = (bstunhdrtype *) (pak->datagramstart);

    p = PRIORITY_ARRAY(output);
    if (!p)
	return(FALSE);
    for (plist = p->plist; plist; plist = plist->next) {
	if (plist->bstun_group) {
	  /* serial(bsc) address prioritization for bstun */
	  addrlen = bstun_pinfo[plist->bstun_group]->addrlength;
	  if ((plist->bstun_group == sth->group) &&  
	      (!bcmp((uchar *)&plist->bsc_addr[0], &sth->addr[0],
		     addrlen)))  {
              *priority = plist->priority;
	     return(TRUE);
          }
	  else
	    continue;
	}
    }
    return(FALSE);
}

/*
 * prilist_protocol_bstun
 * Parse protocol BSTUN specific actions.
 */

boolean prilist_protocol_bstun (parseinfo *csb, plisttype *ptmp)
{
    int priority;
    unsigned bstun_group;
    uchar bsc_addr[BSC_MAX_ADDR_LENGTH];

    /*
     * Init bsc_addr 
     */
    memset(bsc_addr, 0, sizeof(bsc_addr));
 
    /*
     * Get priority type
     */
    priority = GETOBJ(int,4);
 
    /*
     * Get BSTUN group and bsc address
     */
    bstun_group = 0;
    if (GETOBJ(int, 6)) {       /* We have a BSTUN group & addr */
        bstun_group = GETOBJ(int, 6);
        if (bstun_pinfo[bstun_group] == NULL) {
            printf("BSTUN group %d not defined", bstun_group);
            return(FALSE);
        }
        /* get BSC address */
        bcopy(GETOBJ(paddr,1)->bstun_addr, bsc_addr, ADDRLEN_BSTUN);
    }
 
    ptmp->linkcode = LINK_BSTUN;
    ptmp->bstun_group = bstun_group;
    if (bstun_group) {
        bcopy(&bsc_addr[0], &ptmp->bsc_addr[0],
              bstun_pinfo[bstun_group]->addrlength);
    }
    ptmp->priority = priority;
    return(TRUE);
}

/* 
 * priority_protocol_bstun_nv_add
 * Add BSTUN specifics to a priority list configuration.
 */
 
void priority_protocol_bstun_nv_add (plisttype *p)
{
    char buff[BSTUN_MAX_PEER_NAME];
 
    if (p->linkcode != LINK_BSTUN)
        return;
    if (!p->bstun_group)
        return;
    nv_add(TRUE, " address %d ", p->bstun_group);
    bstun_sprintf_addr(p->bsc_addr, 0, p->bstun_group, buff);
    nv_add(TRUE, buff);
}

/*
 * priority_protocol_bstun_compare
 * Compare two priority list nodes for their BSTUN config.  Return TRUE (in
 * the boolean) if they are equal.
 */
 
boolean priority_protocol_bstun_compare (plisttype *one, plisttype *two)
{
    if (one->linkcode != LINK_BSTUN)
        return(FALSE);
    if (two->linkcode != LINK_BSTUN)
        return(FALSE);
    if (one->bstun_group != two->bstun_group)
        return(FALSE);
    if (one->bstun_group && two->bstun_group) {
        if (bcmp(&one->bsc_addr[0], &two->bsc_addr[0],
                  bstun_pinfo[one->bstun_group]->addrlength))
            return(FALSE);
    }
    return(TRUE);
}


/*
 * prilist_uses_bstun_group
 * Searches the priority lists and searches for a a priority list using
 * the specified bstun_group. Returns TRUE if it finds a match, FALSE otherwise.
 * If the bstun_group passed to this function is zero, the function will search
 * for a priority-list with a lincode of LINK_BSTUN. This indicates that a
 * priority-list is using bstun.
 */
boolean prilist_uses_bstun_group(uchar bstun_group)
{
    plisttype *plist;
    unsigned i;

    if (bstun_group) {
       for (i=0; i<PRIORITY_MAXLIST; i++) {
          if (parray[i]) {
             for (plist = parray[i]->plist; plist; plist = plist->next) {
                 if (plist->bstun_group == bstun_group)
                     return(TRUE);
             }
	  }
       }
    } else {
       for (i=0; i<PRIORITY_MAXLIST; i++) {
          if (parray[i]) {
              for (plist = parray[i]->plist; plist; plist = plist->next) {
                  if (plist->linkcode == LINK_BSTUN)
                      return(TRUE);
              }
          }
       }
    }
    return(FALSE);
}

