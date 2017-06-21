/* $Id: stun_priority.c,v 3.4.30.2 1996/05/17 11:21:56 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/stun_priority.c,v $
 *------------------------------------------------------------------
 * stun_priority.c -- Priority Queueing Support for STUN
 *
 * 19-July-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: stun_priority.c,v $
 * Revision 3.4.30.2  1996/05/17  11:21:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.18.2  1996/04/26  07:54:51  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.18.1  1996/04/03  14:35:09  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.30.1  1996/03/18  20:12:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.6.1  1996/03/07  09:42:52  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/11  16:44:10  vrawat
 * CSCdi46408:  unable to remove queue-list command
 *
 * Revision 3.3  1995/11/17  09:24:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:36:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:46:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:49:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "packet.h"
#include "config.h"
#include "logger.h"
#include "../ui/debug.h"
#include "../ibm/ibm.h"
#include "../ibm/ibm_public.h"
#include "../if/priority.h"
#include "../if/network.h"
#include "../if/network_debug.h"

/*
 * stun_prioritization
 * SDLC address prioritization
 */

boolean stun_prioritization (plisttype *plist, paktype *pak, hwidbtype *output)
{
    stunhdrtype *sth;
    unsigned addrlen;

    addrlen = stun_pinfo[plist->stun_group]->addrlength;
    sth = (stunhdrtype *)(pak->network_start);
    if ((plist->stun_group == sth->group) &&  
	(!bcmp((uchar *)&plist->sdlc_addr[0], &sth->addr[0], addrlen))){
	if (priority_debug)
	    buginf("\nPQ: %s: %s -> %s", output->hw_namestring,
		   print_linktype(pak->linktype),
		   idb_queue2name(output, plist->priority));
	return(TRUE);
    }
    return(FALSE);
}

/*
 * stun_addr_pri_tcp
 * handles stun address prioritization on TCP transport
 */

int stun_addr_pri_tcp (hwidbtype *output, paktype *pak, int *priority)
{
    plisttype *plist;
    pritype *p;
    stunhdrtype *sth=NULL;
    unsigned addrlen=0;

    sth = (stunhdrtype *) (pak->datagramstart);

    p = PRIORITY_ARRAY(output);
    if (!p)
	return(FALSE);
    for (plist = p->plist; plist; plist = plist->next) {
	if (plist->stun_group) {
	  /* serial(sdlc) address prioritization for stun */
	  addrlen = stun_pinfo[plist->stun_group]->addrlength;
	  if ((plist->stun_group == sth->group) &&  
	      (!bcmp((uchar *)&plist->sdlc_addr[0], &sth->addr[0],
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
 * prilist_protocol_stun
 * Parse protocol stun specific actions.
 */

boolean prilist_protocol_stun (parseinfo *csb, plisttype *ptmp) 
{
    int priority;
    unsigned stun_group;
    uchar sdlc_addr[SDLC_MAX_ADDR_LENGTH];

    /*
     * Get priority type
     */
    priority = GETOBJ(int,4);

    /*
     * Get STUN group and sdlc address
     */
    stun_group = 0;
    if (GETOBJ(int, 6)) {	/* We have a stun group & addr */
	stun_group = GETOBJ(int, 6);
        if (stun_pinfo[stun_group] == NULL) {
	    printf("STUN group %d not defined", stun_group);
	    return(FALSE);
	}
	/* get sdlc address */
	bcopy(GETOBJ(paddr,1)->stun_addr, sdlc_addr, ADDRLEN_STUN);
    }

    ptmp->linkcode = LINK_STUN;
    ptmp->stun_group = stun_group;
    bcopy(&sdlc_addr[0], &ptmp->sdlc_addr[0],
	  stun_pinfo[stun_group]->addrlength);
    ptmp->priority = priority;
    return(TRUE);
}

/*
 * priority_protocol_stun_nv_add
 * Add STUN specifics to a priority list configuration.
 */

void priority_protocol_stun_nv_add (plisttype *p)
{
    char buff[MAX_PEER_NAME];
    
    if (p->linkcode != LINK_STUN)
	return;
    if (!p->stun_group)
	return;
    nv_add(TRUE, " address %d ", p->stun_group);
    stun_sprintf_addr(p->sdlc_addr, 0, p->stun_group, buff);
    nv_add(TRUE, buff);
}

/*
 * priority_protocol_stun_compare
 * Compare two priority list nodes for their stun config.  Return TRUE 
 * if they are equal.
 */

boolean priority_protocol_stun_compare (plisttype *one,
				     plisttype *two)
{
    if (one->linkcode != LINK_STUN)
	return(FALSE);
    if (two->linkcode != LINK_STUN)
	return(FALSE);

    /* two are equal if stun_group is 0 (address not configured) */
    if ((one->stun_group == 0) &&
        (two->stun_group == 0))
        return TRUE;

    if (one->stun_group != two->stun_group)
	return(FALSE);
    if (bcmp(&one->sdlc_addr[0], &two->sdlc_addr[0],
	      stun_pinfo[one->stun_group]->addrlength))
	return(FALSE);
    return(TRUE);
}

/*
 * prilist_uses_stun_group
 * Searches the priority lists and searches for a a priority list using
 * the specified stun_group. Returns TRUE if it finds a match, FALSE otherwise.
 * If the stun_group passed to this function is zero, the function will search
 * for a priority-list with a lincode of LINK_STUN. This indicates that a
 * priority-list is using stun.
 */
boolean prilist_uses_stun_group(uchar stun_group)
{
    plisttype *plist;
    unsigned i;

    if (stun_group) {
       for (i=0; i<PRIORITY_MAXLIST; i++) {
          if (parray[i]) {
             for (plist = parray[i]->plist; plist; plist = plist->next) {
                 if (plist->stun_group == stun_group)
                     return(TRUE);
             }
	  }
       }
    } else {
       for (i=0; i<PRIORITY_MAXLIST; i++) {
          if (parray[i]) {
              for (plist = parray[i]->plist; plist; plist = plist->next) {
                  if (plist->linkcode == LINK_STUN)
                      return(TRUE);
              }
          }
       }
    }
    return(FALSE);
}

