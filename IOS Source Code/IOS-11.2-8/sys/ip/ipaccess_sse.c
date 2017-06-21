/* $Id: ipaccess_sse.c,v 3.4.26.6 1996/08/28 12:49:38 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess_sse.c,v $
 *------------------------------------------------------------------
 * ipaccess_sse.c - Simple IP access lists for the SSE
 *
 * June 1993 Tony Li
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess_sse.c,v $
 * Revision 3.4.26.6  1996/08/28  12:49:38  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.26.5  1996/07/06  05:53:24  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.4.26.4  1996/05/13  06:30:45  gchristy
 * CSCdi50886:  SSE: reload in _sse_tree_walk
 * Branch: California_branch
 * - Fix dynamic ACL generation bugs
 * - Deal with the unconditional match case correctly.
 *
 * Revision 3.4.26.3  1996/05/04  01:03:04  wilber
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
 * Revision 3.4.26.2  1996/04/29  09:20:43  gchristy
 * CSCdi50875:  SSE: handle low memory conditions more gracefully
 * Branch: California_branch
 *
 * Revision 3.4.26.1  1996/03/18  20:16:28  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.1  1996/03/07  09:46:07  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4  1996/01/19  00:57:21  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/06  05:54:23  gchristy
 * CSCdi44414:  Router crashes once the sse is enabled for IP on the EIP
 * interfaces.
 *   - Always set patchpoint to deny packets before freeing trees.
 *   - Use sse_slow_free_tree() instead of ipaccess_sse_gc_tree() to clean
 *     up after we run out of SSE memory.
 *   - Deal with possible error conditions in ipaccess_sse_code_gen().
 *
 * Revision 3.2  1995/11/17  09:34:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:04:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/09/25  08:31:58  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.8  1995/08/28  22:01:17  gchristy
 * CSCdi39439:  SSE: null dereference in IP access-lists
 *
 * Revision 2.7  1995/08/28  21:01:30  gchristy
 * CSCdi39419:  SSE: memory leak when parsing long access-lists
 *  - Free partially constructed trees before bailing.
 *  - Add an extra out-of-sync check before rebuilding to reduce churning
 *    while a long access-list is being parsed.
 *
 * Revision 2.6  1995/06/28  09:26:45  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.5  1995/06/26  01:35:42  gchristy
 * CSCdi36406:  SSE: NULL dereference with simple IP ACLs
 *
 * Revision 2.4  1995/06/19  01:06:28  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.3  1995/06/15  18:38:29  gchristy
 * CSCdi26854:  SSE access-lists arent enabled with SSE switching
 *  - add a new registry function which causes SSE access lists to be
 *    rebuilt when SSE switching is enabled or disabled on an interface
 *    that has an access-list defined
 *
 * Revision 2.2  1995/06/09  13:05:28  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:58:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include <stdlib.h>
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "registry.h"
#include "ip_registry.h"
#include "logger.h"
#include "../hes/msg_sse.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "../util/range.h"
#include "../srt/span.h"
#include "../hes/if_mci.h"
#include "access.h"
#include "ipaccess_private.h"
#include "ipaccess.h"
#include "../ip/ip.h"
#include "../parser/parser_defs_access.h"

#include "../parser/parser_defs_sse.h"
#include "../hes/sse_public.h"
#include "../hes/sse.h"
#include "../hes/sse_private.h"
#include "../hes/sse_registry.h"
#include "../ip/ipfast_sse.h"
#include "../hes/ssesym.h"
#include "ipaccess_sse_private.h"


/*
 * sse_access_group_callback
 * This routine gets called back when something changed and we need to know
 * about it.
 */

void sse_access_group_callback (idbtype *idb, boolean groupin,
				acl_headertype *acl)
{
    hwidbtype *hwidb;
    mcitype *cbus;
    ssetype *sse;
    sse_ipaccessinfo *ipaccinfo;

    hwidb = idb->hwptr;
    if (!hwidb || !(hwidb->status & IDB_CBUS))
	return;

    /*
     * We don't care about output access-group changes on interfaces
     * that aren't SSE-switching
     */
    if (!groupin && !(hwidb->ip_routecache & IP_SSE_ROUTECACHE))
	return;

    /*
     * If we got this far, we either have an input access-group
     * change, or an output access-group change on an interface which
     * has SSE-switching enabled.
     */

    cbus = hwidb->devctl;
    if (!cbus)
	return;

    sse = cbus->sseptr;
    if (!sse)
	return;

    ipaccinfo = sse->protoinfo.ipaccessinfo;
    if (!ipaccinfo)
	return;
    if (sse_debug)
	buginf("\nSSE: IP access group change detected");

    ipaccinfo->version++;
    return;
}

/*
 * sse_access_check
 * This routine is called when something has changed on an interface
 * that may require a rebuild of the access trees.
 */

void sse_access_check (idbtype *idb)
{
    hwidbtype *hwidb;
    mcitype *cbus;
    ssetype *sse;
    sse_ipaccessinfo *ipaccinfo;

    hwidb = idb->hwptr;
    if (!hwidb || !(hwidb->status & IDB_CBUS))
	return;

    cbus = hwidb->devctl;
    if (!cbus)
	return;

    sse = cbus->sseptr;
    if (!sse)
	return;

    ipaccinfo = sse->protoinfo.ipaccessinfo;
    if (!ipaccinfo)
	return;

    if (idb->ip_output_accesslist) 
	ipaccinfo->version++;

}

/*
 * sse_access_list_callback
 * This routine gets called back when something changed and we need to know
 * about it.
 */

void sse_access_list_callback (acl_headertype *acl)
{
    mcitype *cbus;
    ssetype *sse;
    sse_ipaccessinfo *ipaccinfo;
    int aclnum;

    if (!acl)
	return;

    switch (acl->type) {
    case ACL_IP_SIMPLE:		/* We only support numbered ACLs (for now) */
    case ACL_IP_EXTENDED:
	break;

    default:
	return;
    }

    aclnum = atoi(acl->name);

    for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	sse = cbus->sseptr;
	if (!sse)
	    continue;
	ipaccinfo = sse->protoinfo.ipaccessinfo;
	if (!ipaccinfo)
	    continue;

 	/*
 	 * Only force an update if the list is one of the ones we
 	 * are using.
 	 */
 	if (ipaccinfo->ipaccess_cache[aclnum])
	    ipaccinfo->version++;
    }
}

/*
 * ipaccess_sse_outofsync
 * Return TRUE if we're out of sync.
 */

static inline boolean ipaccess_sse_outofsync (sse_ipaccessinfo *ipaccinfo)
{
    return(ipaccinfo->version != ipaccinfo->generated);
}

/*
 * ipaccess_sse_dispatch
 * Return TRUE if we're out of sync.
 */

static boolean ipaccess_sse_dispatch (sse_protoinfo *protoinfo)
{
    sse_ipaccessinfo *ipaccinfo;

    ipaccinfo = protoinfo->ipaccessinfo;
    if (!ipaccinfo)
	return(FALSE);
    return(ipaccess_sse_outofsync(ipaccinfo));
}

/*
 * ipaccess_sse_deny
 * Backpatch an ACL to always deny.
 */

void ipaccess_sse_deny (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
			int access_list) 
{
    shadowwordtype sw;

    sse_init_sw(&sw, 0, SSE_INST_NOP, SSE_GLOBAL_DENY);
    sse_poke_treemem(sse,
		     ipaccinfo->ipaccess_cache[access_list]->patchpoint, &sw); 
}

/*
 * ipaccess_sse_iacl_deny
 * Set input ACL patchpoint to deny all packets.
 */
static void ipaccess_sse_iacl_deny(ssetype *sse, sse_ipaccessinfo *ipaccinfo)
{
    shadowwordtype sw;

    sse_init_sw(&sw, 0, SSE_INST_NOP, SSE_GLOBAL_DENY);
    sse_poke_treemem(sse, SSE_GLOBAL_IPIACL, &sw);
}

/*
 * ipaccess_sse_backpatch
 * Backpatch a new tree into place.
 */

void ipaccess_sse_backpatch (ssetype *sse, sse_ipaccessinfo *ipaccessinfo,
			     int list, spintype *newtree)
{
    shadowwordtype sw;

    if (!ipaccessinfo->ipaccess_cache[list])
	return;
    if (!newtree) {
	ipaccess_sse_deny(sse, ipaccessinfo, list);
	return;
    }
    sse_init_sw(&sw, 0, SSE_INST_NOP, newtree->physaddr);
    sse_poke_treemem(sse, ipaccessinfo->ipaccess_cache[list]->patchpoint,
		     &sw);
}

/*
 * ipaccess_sse_cache_setup
 * Hang SSE information off the access_cache.  Set up the backpatch point.  
 */

static void ipaccess_sse_cache_setup (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
			       int list)  
{

    ipaccinfo->ipaccess_cache[list] = malloc(sizeof(cacheentrytype)); 

    if (!ipaccinfo->ipaccess_cache[list]) {
	errmsg(&msgsym(NOMEMORY, SSE), "access information"); 
	return;
    }
    if (!ipaccinfo->ipaccess_cache[list]->patchpoint)
	ipaccinfo->ipaccess_cache[list]->patchpoint =
	    sse_malloc_treeaddr(sse, &ipaccinfo->ipaccess_stat);
    if (!ipaccinfo->ipaccess_cache[list]->patchpoint) {
	free(ipaccinfo->ipaccess_cache[list]);
	ipaccinfo->ipaccess_cache[list] = NULL;
	return;
    }
    ipaccess_sse_deny(sse, ipaccinfo, list);
}

/*
 * ipaccess_sse_deletetree
 * Free an ACL tree
 */

void ipaccess_sse_deletetree (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
			      int list)
{
    if (ipaccinfo->ipaccess_cache[list]->ip_access_tree) {
	sse_slow_free_tree(sse,
		   ipaccinfo->ipaccess_cache[list]->ip_access_tree,
		   &ipaccinfo->ipaccess_stat);
    }	

    ipaccinfo->ipaccess_cache[list]->ip_access_tree = NULL;
	
}    

/*
 * ipaccess_sse_clean
 * Free all ACL trees and patch to send all packets to the RP
 */

static void ipaccess_sse_clean (ssetype *sse, sse_protoinfo *protoinfo)
{
    sse_ipaccessinfo *ipaccinfo;
    int i;

    ipaccinfo = protoinfo->ipaccessinfo;
    if (!ipaccinfo)
	return;
   
    /*
     * Send all packets to the RP and free the input ACL tree
     */

    if (ipaccinfo->iacl_tree) {
	ipaccess_sse_iacl_deny(sse, ipaccinfo);
	sse_free_tree(sse, ipaccinfo->iacl_tree, &ipaccinfo->ipaccess_stat);
	ipaccinfo->iacl_tree = NULL;
    }

    for (i = MINFASTACC; i <= MAXSLOWACC; i++)
	if (ipaccinfo->ipaccess_cache[i]) {
	    ipaccess_sse_deny(sse, ipaccinfo, i);
	    ipaccess_sse_deletetree(sse, ipaccinfo, i);
	}
}    

/*
 * ipaccess_sse_get_patchpoint
 * return the treeaddr of the patchpoint for a given ACL
 */

static ulong ipaccess_sse_get_patchpoint (ssetype *sse, acl_headertype *acl)
{
    uint list;
    sse_ipaccessinfo *ipaccessinfo;

    if (!acl)
	return(0);

    switch (acl->type) {
    case ACL_IP_SIMPLE:
    case ACL_IP_EXTENDED:
	break;
	
    default:
	return(0);
    }

    ipaccessinfo = sse->protoinfo.ipaccessinfo;

    if (!ipaccessinfo)
	return(0);

    list = atoi(acl->name);

    if (ipaccessinfo->ipaccess_cache[list]) 
	return((ulong) ipaccessinfo->ipaccess_cache[list]->patchpoint);
    else 
	return(0);
}    

/*
 * ipaccess_sse_acl_exists
 * return TRUE if we have generated a tree for a given acl
 */

static boolean ipaccess_sse_acl_exists (ssetype *sse, acl_headertype *acl)
{

    sse_ipaccessinfo *ipaccessinfo;
    int aclnum;

    ipaccessinfo = sse->protoinfo.ipaccessinfo;

    if (!ipaccessinfo)
	return(FALSE);

    if (!acl)
	return(TRUE);

    switch (acl->type) {
    case ACL_IP_SIMPLE:
    case ACL_IP_EXTENDED:
	break;

    default:
	return(FALSE);

    }

    aclnum = atoi(acl->name);

    if (ipaccessinfo->ipaccess_cache[aclnum] &&
	ipaccessinfo->ipaccess_cache[aclnum]->ip_access_tree)
	    return(TRUE);

    return(FALSE);
}    

/*
 * ipaccess_sse_iacltree_add_node
 * Add a node to the iacl tree.
 * Code shamelessly lifted from sse_icbtree_add_node.
 */

static boolean ipaccess_sse_iacltree_add_node (ssetype *sse,
					sse_ipaccessinfo *ipaccinfo,
					hwidbtype *hwidb, int list,
					treeaddrtype patchpoint)  
{
    spintype **pred, *spin;
    uchar byte;
    ssestattype *stat;
    
    /*
     * Add the address to the tree.
     */
    stat = &ipaccinfo->ipaccess_stat;
    pred = &ipaccinfo->iacl_tree;
    spin = *pred;
    byte = (hwidb->cbus_icb_address >> 1) & 0xff;
    while (spin) {
	switch (spin->type) {
	case SPIN_COMPARE:
	    if (spin->value == byte) 
		return(TRUE);		/* duplicate icbs -- legal! */
	    else if (byte > spin->value) 
		pred = &spin->GTADDR;
	    else if (byte < spin->value) 
		pred = &spin->LTADDR;
	    spin = *pred;
	    break;
	default:
	    if (sse_debug)
		buginf("\nSSE: ipaccess_sse_iacltree_add_node unknown type %d", 
		       spin->type); 
	    return(FALSE);
	}
    }
    
    /*
     * The icb is not in the tree.
     */
    spin = sse_malloc_spin(0, stat);
    if (!spin) 
	return(FALSE);
    sse_init_ucond_spin(spin, byte, SSE_INST_NOP);
    *pred = spin;
    pred = &spin->EQADDR;
    
    /*
     * Add the match node. 
     */
    spin = sse_malloc_spin(0, stat);
    if (!spin) 
	return(FALSE);
    spin->type = SPIN_MATCH;
    spin->EXTRACOUNT = 0;
    spin->protoinfo = patchpoint;
    spin->nodeinfo.match.protoinfo2 = list;
    *pred = spin;
    return(TRUE);
}

/*
 * ipaccess_sse_iacltree_code_gen
 * Generate (and install!) code for the input ACL tree.
 * This is slightly modified version of sse_icbtree_code_gen.
 */

static int ipaccess_sse_iacltree_code_gen (ssetype *sse, spintype *spin,
				    ssestattype *stat, queuetype *queue)
{
    shadowwordtype tmp;
    
    switch (spin->type) {
    case SPIN_COMPARE:
	tmp.lt.inst = spin->nodeinfo.compare.lt.inst;
	tmp.eq.inst = spin->nodeinfo.compare.eq.inst;
	tmp.gt.inst = spin->nodeinfo.compare.gt.inst;
	if (spin->LTADDR) {
	    tmp.lt.ptr = spin->LTADDR->physaddr;
	    tmp.lt.value = spin->LTADDR->value;
	} else
	    tmp.lt.ptr = SSE_GLOBAL_RETURN;
	if (spin->EQADDR) 
	    tmp.eq.ptr = spin->EQADDR->physaddr;
	else {
	    errmsg(&msgsym(SWFAILURE, SSE), "ipiacl spin node without match",
		   spin);
	    crashdump(10);
	    tmp.eq.ptr = SSE_GLOBAL_RETURN;
	}
	if (spin->GTADDR) {
	    tmp.gt.ptr = spin->GTADDR->physaddr;
	    tmp.gt.value = spin->GTADDR->value;
	} else
	    tmp.gt.ptr = SSE_GLOBAL_RETURN;
	sse_poke_treemem(sse, spin->physaddr, &tmp);
	break;
    case SPIN_MATCH:
	sse_init_sw(&tmp, 0, SSE_INST_NOP, spin->protoinfo);
	sse_poke_treemem(sse, spin->physaddr, &tmp);
	break;
    default:
	break;
    }
    return(OPCODE_WALK);
}

/*
 * ipaccess_sse_link_nodes
 * Link two spin nodes together for unconditional execution.
 */

static inline void ipaccess_sse_link_nodes (spintype **lt, spintype **eq,
					    spintype **gt, spintype *new)
{
    *eq = new;
    if (lt) {
	*lt = new;
	sse_spinref_plus1(new);
    }
    if (gt) {
	*gt = new;
	sse_spinref_plus1(new);
    }
}

#define SSE_SPIN(new, val, inst) \
	new = sse_malloc_spin(SSE_PROTOCOL_IP, &ipaccinfo->ipaccess_stat); \
	if (!new) { \
	    sse_free_tree(sse, term, &ipaccinfo->ipaccess_stat); \
	    return(NULL); \
	} \
	sse_init_ucond_spin(new, val, inst);

#define SSE_SPINC(new, val, inst1, inst2, inst3) \
	new = sse_malloc_spin(SSE_PROTOCOL_IP, &ipaccinfo->ipaccess_stat); \
	if (!new) { \
	    sse_free_tree(sse, term, &ipaccinfo->ipaccess_stat); \
	    return(NULL); \
	} \
	sse_init_spin(new, val, inst1, inst2, inst3);
/* 
 * If this item has logging enabled or may time out, punt it up to the
 * RP if we match 
 */
#define ITEM_GRANT ((item->log || item->timeout) ? FALSE : item->grant)

/*
 * ipaccess_sse_term
 * Build tree for a single term.  Return FALSE if there are problems.
 */

spintype *ipaccess_sse_term (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
			     boolean grant, ipaddrtype address, ipaddrtype
			     mask)
{
    treecmptype value;
    int byte;
    uchar addrbytes[sizeof(ipaddrtype)];
    uchar maskbytes[sizeof(ipaddrtype)];
    spintype *term, **pred_eq, **pred_lt, **pred_gt, *new, *nop;
    boolean first;

    term = NULL;
    pred_eq = &term;
    pred_lt = NULL;
    pred_gt = NULL;
    value = 0;
    first = TRUE;
    bcopy(&address, addrbytes, sizeof(ipaddrtype));
    bcopy(&mask, maskbytes, sizeof(ipaddrtype));
    for (byte = 0; byte < sizeof(ipaddrtype); byte++) {
	if (maskbytes[byte] == 0xff) 
	    continue;

	/*
	 * The first node for the byte is for RD_SREG.
	 */
	SSE_SPIN(new, value, SSE_INST_RD_IPSRC + byte);

	/*
	 * The next node is NOP while we delay for the RD_SREG to complete.
	 * The mask for this byte is loaded here.  After promotion, it will
	 * be the data for the RD_SREG instruction.
	 */
	SSE_SPIN(nop, (~maskbytes[byte] & 0xff), SSE_INST_NOP);
	if (first) {
	    first = FALSE;
	    new->LTADDR = nop;
	    new->GTADDR = nop;
	    sse_spinref_plus2(nop);
	}
	new->EQADDR = nop;
	value = addrbytes[byte] & ~maskbytes[byte];
	ipaccess_sse_link_nodes(pred_lt, pred_eq, pred_gt, new);
	pred_eq = &nop->EQADDR;
	pred_lt = &nop->LTADDR;
	pred_gt = &nop->GTADDR;
    }

    /*
     * Ok, add a final dummy node on so that we either get the last
     * compare or just jump to the next term.
     */
    SSE_SPIN(new, value, SSE_INST_NOP);
    new->EQGRANT = grant;
    if (first) {
	/*
	 * Access list term was a NOOP.  Treat entire term as an unconditional
	 * match.
	 */
	new->LTGRANT = new->GTGRANT = grant;
	new->type = SPIN_COMPARE_FINAL_TERM;
    } else {
	new->LTGRANT = new->GTGRANT = !grant;
    }

    ipaccess_sse_link_nodes(pred_lt, pred_eq, pred_gt, new);

    return(term);
}

/*
 * ipaccess_sse_link_terms_subr
 * Link the current term into failure nodes of the previous term.
 * For COMPARE nodes, replace dangling links with current term.  If all 3
 * links are dangling, only revise 2 out of 3.
 * For COMPARE_NXT_TERM nodes, unconditionally link in the current term.
 * For COMPARE_FINAL_TERM nodes, avoid revising link regardless.
 */

static int ipaccess_sse_link_terms_subr (ssetype *sse, spintype *spin,
				  ssestattype *stat, queuetype *queue)
{
    spintype *curr;
    boolean grant;

    curr = (spintype *) stat;
    grant = (boolean) queue;
    if (spin == curr)
	return(OPCODE_PARENT);
    switch (spin->type) {
    case SPIN_COMPARE:
	if (!spin->LTADDR && !spin->EQADDR && !spin->GTADDR) {
	    /*
	     * Figure out which branch was success branch.  Link into
	     * all others.  Note: 'grant' is the grant sense from the
	     * term we are linking from, not the term we are linking to.
	     */
	    if (spin->LTGRANT == grant) {
		spin->EQADDR = spin->GTADDR = curr;
	    } else if (spin->EQGRANT == grant) {
		spin->LTADDR = spin->GTADDR = curr;
	    } else {
		spin->LTADDR = spin->EQADDR = curr;
	    }
	    sse_spinref_plus2(curr);
	    /*
	     * Previous term node should now be considered terminal, to
	     * prevent anything from linking into its 'grant' branch.
	     */
	    spin->type = SPIN_COMPARE_FINAL_TERM;
	} else {
	    /*
	     * Conditional link to next term
	     */
	    if (!spin->LTADDR) {
		sse_spinref_plus1(curr);
		spin->LTADDR = curr;
	    }
	    if (!spin->EQADDR) {
		sse_spinref_plus1(curr);
		spin->EQADDR = curr;
	    }
	    if (!spin->GTADDR) {
		sse_spinref_plus1(curr);
		spin->GTADDR = curr;
	    }
	}
	break;
    case SPIN_COMPARE_NXT_TERM:
	/*
	 * Unconditional link to next term
	 */
	spin->LTADDR = spin->EQADDR = spin->GTADDR = curr;
	sse_spinref_plus2(curr);
	sse_spinref_plus1(curr);
	return(OPCODE_PARENT);
	break;
    case SPIN_COMPARE_FINAL_TERM:
    default:
	/*
	 * No links
	 */
	break;
    }
    return(OPCODE_WALK);
}

/*
 * ipaccess_sse_link_cmp_subr
 * Link the current comparison operator to previous comparison operator.
 * For COMPARE nodes, replace links to address 'match' with current term.
 * For COMPARE_NXT_TERM and COMPARE_FINAL_TERM nodes,
 * avoid revising link regardless.
 */

static int ipaccess_sse_link_cmp_subr (ssetype *sse, spintype *spin,
				ssestattype *stat,  queuetype *queue)
{
    spintype *curr, *match;

    curr = (spintype *) stat;
    match = (spintype *) queue;
    if (spin == curr)
	return(OPCODE_PARENT);
    switch (spin->type) {
    case SPIN_COMPARE:
	/*
	 * Conditional link to next operator
	 */
	if (spin->LTADDR == match) {
	    sse_spinref_plus1(curr);
	    spin->LTADDR = curr;
	}
	if (spin->EQADDR == match) {
	    sse_spinref_plus1(curr);
	    spin->EQADDR = curr;
	}
	if (spin->GTADDR == match) {
	    sse_spinref_plus1(curr);
	    spin->GTADDR = curr;
	}
	break;
    case SPIN_COMPARE_NXT_TERM:
    case SPIN_COMPARE_FINAL_TERM:
    default:
	/*
	 * No links
	 */
	break;
    }
    return(OPCODE_WALK);
}

/*
 * ipaccess_sse_link_terms
 * (See ipaccess_sse_link_terms_subr)
 */

static void ipaccess_sse_link_terms (spintype *pred, spintype *curr, boolean grant)
{
    pred->walk = NULL;
    sse_tree_walk(NULL, pred, ipaccess_sse_link_terms_subr, NULL,
		  (ssestattype *) curr, (queuetype *) grant);
}

/*
 * ipaccess_sse_link_cmp
 * (See ipaccess_sse_link_cmp_subr)
 */

static void ipaccess_sse_link_cmp (spintype *pred, spintype *curr, spintype *match)
{
    pred->walk = NULL;
    sse_tree_walk(NULL, pred, ipaccess_sse_link_cmp_subr, NULL,
		  (ssestattype *) curr, (queuetype *) match);
}

/*
 * ipaccess_sse_check_udptcp_subr
 * Check either the source or destination port of the UDP/TCP header.
 */

static spintype *ipaccess_sse_check_udptcp_subr (ssetype *sse,
					  sse_ipaccessinfo *ipaccinfo,
					  slowtype *item, spintype *new,
					  spintype *term, boolean source,
					  spintype *fail, spintype *success,
					  spin_cell *link_pt)
{
    spintype *nop, *new1, *new2;
    ushort port, port2, opr;
    treeinsttype inst1, inst2;

    if (source) {
	inst1 = SSE_INST_RD_IPSPORT_MSB;
	inst2 = SSE_INST_RD_IPSPORT_LSB;
	port = item->sport;
	port2 = item->sport2;
	opr = item->srcopr;
    } else {
	inst1 = SSE_INST_RD_IPDPORT_MSB;
	inst2 = SSE_INST_RD_IPDPORT_LSB;
	port = item->dport;
	port2 = item->dport2;
	opr = item->dstopr;
    }
    /*
     * Check MSB IP port
     */
    SSE_SPIN(nop, 0xff, SSE_INST_NOP);
    if (!source) {
	/*
	 * Want unconditional execution as we've just switched scratchpad
	 * pages.
	 */
	sse_init_spin_links(new, nop);
	new->LTINST = new->EQINST = new->GTINST = inst1;
    } else {
	link_pt->inst = inst1;
	link_pt->addr = nop;
    }
    SSE_SPINC(new, ((port >> 8) & 0xff), SSE_INST_LD_SPAGE, inst2,
	      SSE_INST_LD_SPAGE);
    sse_init_spin_links(nop, new);
    switch (opr) {
    case OPC_LT:
	SSE_SPIN(nop, 0xff, SSE_INST_NOP);
	new->EQADDR = nop;
	new->LTADDR = success;
	sse_spinref_plus1(success);
	new->GTADDR = fail;
	sse_spinref_plus1(fail);
	SSE_SPIN(new, (port & 0xff), SSE_INST_LD_SPAGE);
	sse_init_spin_links(nop, new);
	new->EQADDR = new->GTADDR = fail;
	sse_spinref_plus2(fail);
	new->LTADDR = success;
	sse_spinref_plus1(success);
	break;
    case OPC_GT:
	SSE_SPIN(nop, 0xff, SSE_INST_NOP);
	new->EQADDR = nop;
	new->LTADDR = fail;
	sse_spinref_plus1(fail);
	new->GTADDR = success;
	sse_spinref_plus1(success);
	SSE_SPIN(new, (port & 0xff), SSE_INST_LD_SPAGE);
	sse_init_spin_links(nop, new);
	new->LTADDR = new->EQADDR = fail;
	sse_spinref_plus2(fail);
	new->GTADDR = success;
	sse_spinref_plus1(success);
	break;
    case OPC_EQ:
	SSE_SPIN(nop, 0xff, SSE_INST_NOP);
	new->EQADDR = nop;
	new->LTADDR = new->GTADDR = fail;
	sse_spinref_plus2(fail);
	SSE_SPIN(new, (port & 0xff), SSE_INST_LD_SPAGE);
	sse_init_spin_links(nop, new);
	new->LTADDR = new->GTADDR = fail;
	sse_spinref_plus2(fail);
	new->EQADDR = success;
	sse_spinref_plus1(success);
	break;
    case OPC_NEQ:
	SSE_SPIN(nop, 0xff, SSE_INST_NOP);
	new->EQADDR = nop;
	new->LTADDR = new->GTADDR = success;
	sse_spinref_plus2(success);
	SSE_SPIN(new, (port & 0xff), SSE_INST_LD_SPAGE);
	sse_init_spin_links(nop, new);
	new->LTADDR = new->GTADDR = success;
	sse_spinref_plus2(success);
	new->EQADDR = fail;
	sse_spinref_plus1(fail);
	break;
    case OPC_RANGE:
	new->EQINST = new->GTINST = SSE_INST_NOP;
	SSE_SPINC(new1, ((port2 >> 8) & 0xff), inst2, inst2,
		  SSE_INST_LD_SPAGE);
	new->EQADDR = new1;
	new->LTADDR = fail;
	sse_spinref_plus1(fail);
	SSE_SPIN(nop, 0xff, SSE_INST_NOP);
	new1->LTADDR = nop;
	SSE_SPIN(new2, (port & 0xff), SSE_INST_LD_SPAGE);
	sse_init_spin_links(nop, new2);
	new2->LTADDR = fail;
	sse_spinref_plus1(fail);
	new2->EQADDR = new2->GTADDR = success;
	sse_spinref_plus2(success);
	SSE_SPIN(nop, 0xff, SSE_INST_NOP);
	new1->EQADDR = nop;
	SSE_SPINC(new2, (port & 0xff), SSE_INST_LD_SPAGE,
		  SSE_INST_LD_SPAGE, SSE_INST_NOP);
	sse_init_spin_links(nop, new2);
	new2->LTADDR = fail;
	sse_spinref_plus1(fail);
	SSE_SPIN(new1, (port2 & 0xff), SSE_INST_LD_SPAGE);
	new2->EQADDR = new2->GTADDR = new1;
	sse_spinref_plus1(new1);
	new1->GTADDR = fail;
	sse_spinref_plus1(fail);
	new1->LTADDR = new1->EQADDR = success;
	sse_spinref_plus2(success);
	SSE_SPINC(new2, ((port2 >> 8) & 0xff), SSE_INST_LD_SPAGE,
		  inst2, SSE_INST_LD_SPAGE);
	new->GTADDR = new2;
	new2->LTADDR = success;
	sse_spinref_plus1(success);
	new2->GTADDR = fail;
	sse_spinref_plus1(fail);
	SSE_SPIN(nop, 0xff, SSE_INST_NOP);
	new2->EQADDR = nop;
	sse_init_spin_links(nop, new1);
	sse_spinref_plus1(new1);
	new = new1;
	break;
    default:
	break;
    }
    return(new);
}

/*
 * ipaccess_sse_check_udptcp
 * Perform TCP/UDP port checking.
 */

static spintype *ipaccess_sse_check_udptcp (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				     slowtype *item, spintype *new,
				     spintype *term, spin_cell *link_pt)
{
    spintype *nop, *fail, *success, *src_tree;

    src_tree = NULL;
    SSE_SPIN(fail, 0x80, SSE_INST_NOP);
    /*
     * All branches should lead us to next access list term
     */
    fail->type = SPIN_COMPARE_NXT_TERM;
    SSE_SPIN(success, 0x80, SSE_INST_NOP);
    success->LTGRANT = success->EQGRANT = success->GTGRANT = ITEM_GRANT;
    success->type = SPIN_COMPARE_FINAL_TERM;
    if (item->srcopr != OPC_NONE) {
	src_tree = new;
	new = ipaccess_sse_check_udptcp_subr(sse, ipaccinfo, item, new, term,
					     TRUE, fail, success, link_pt);
	if (!new) {
	    if (mem_refcount(fail) == 1)
		free(fail);
	    if (mem_refcount(success) == 1)
		free(success);
	    return(NULL);
	}
    }
    if (item->dstopr != OPC_NONE) {
	/*
	 * First switch scratchpad pages
	 */
	SSE_SPIN(nop, 0x81, SSE_INST_NOP);
	if (src_tree) {
	    /*
	     * Need to link into success nodes from src port comparison tree.
	     */
	    ipaccess_sse_link_cmp(src_tree, nop, success);
	} else {
	    link_pt->inst = SSE_INST_LD_SPAGE;
	    link_pt->addr = nop;
	}
	SSE_SPIN(new, 0, SSE_INST_NOP);
	sse_init_spin_links(nop, new);
	new = ipaccess_sse_check_udptcp_subr(sse, ipaccinfo, item, new, term,
					     FALSE, fail, success, NULL);
	if (!new) {
	    if (mem_refcount(fail) == 1)
		free(fail);
	    if (mem_refcount(success) == 1)
		free(success);
	    return(NULL);
	}
    }
    /*
     * Adjust refcounts
     */
    if (mem_refcount(fail) > 1)
	sse_spinref_minus1(fail);
    if (mem_refcount(success) > 1)
	sse_spinref_minus1(success);
    return(success);
}

/*
 * ipaccess_sse_check_fo
 * Check to see if this is a fragmented IP packet.  If so, accept it,
 * regardless of item->grant.
 */

static spintype *ipaccess_sse_check_fo (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				 spintype *new, spintype *term)
{
    spintype *nop;

    /*
     * Check MSB IP FO
     */
    SSE_SPIN(nop, 0x1f, SSE_INST_NOP);
    new->EQINST = SSE_INST_RD_IPFO_MSB;
    new->EQADDR = nop;
    SSE_SPINC(new, 0, SSE_INST_NOP, SSE_INST_RD_IPFO_LSB, SSE_INST_NOP);
    /*
     * If FO check fails, return grant = TRUE
     */
    new->LTGRANT = new->EQGRANT = new->GTGRANT = TRUE;
    new->type = SPIN_COMPARE_FINAL_TERM;
    sse_init_spin_links(nop, new);
    /*
     * Check LSB IP FO.
     */
    SSE_SPIN(nop, 0xff, SSE_INST_NOP);
    new->EQADDR = nop;
    SSE_SPIN(new, 1, SSE_INST_NOP);
    /*
     * If fragment offset is:
     *     1, return grant = FALSE (fragment is a security problem)
     *     0, continue with access checks
     * otherwise, return grant = TRUE.
     */
    new->LTGRANT = new->EQGRANT = FALSE;
    new->GTGRANT = TRUE;
    new->type = SPIN_COMPARE_FINAL_TERM;
    sse_init_spin_links(nop, new);
    return(new);
}

/*
 * ipaccess_sse_check_tos
 * Check precedence and tos subfields of TOS byte from IP header.
 */

static spintype *ipaccess_sse_check_tos (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				  slowtype *item, spintype *new,
				  spintype *term, boolean first)
{
    spintype *nop;
    uchar mask;
    uchar tos;

    mask = 0;
    tos = 0;
    if (item->tos) {
	mask |= IP_TOS_MASK;
	tos |= item->tos_value;
    }
    if (item->precedence) {
	mask |= IP_TOS_PRECEDENCE;
	tos |= item->precedence_value;
    }
    SSE_SPIN(nop, mask, SSE_INST_NOP);
    new->EQADDR = nop;
    new->EQINST = SSE_INST_RD_IPTOS;
    if (first) {
	new->LTINST = SSE_INST_RD_IPTOS;
	new->GTINST = SSE_INST_RD_IPTOS;
	new->LTADDR = nop;
	new->GTADDR = nop;
	sse_spinref_plus2(nop);
    }
    SSE_SPIN(new, tos, SSE_INST_NOP);
    sse_init_spin_links(nop, new);
    return(new);
}

/*
 * ipaccess_sse_check_pro
 * Compare IP protocol number of packet against extended access list.
 */

static spintype *ipaccess_sse_check_pro (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				  slowtype *item, spintype *new,
				  spintype *term, boolean first)
{
    spintype *nop;

    SSE_SPIN(nop, 0xff, SSE_INST_NOP);
    new->EQADDR = nop;
    new->EQINST = SSE_INST_RD_IPPRO;
    if (first) {
	new->LTINST = SSE_INST_RD_IPPRO;
	new->GTINST = SSE_INST_RD_IPPRO;
	new->LTADDR = nop;
	new->GTADDR = nop;
	sse_spinref_plus2(nop);
    }
    SSE_SPIN(new, item->protocol, SSE_INST_NOP);
    sse_init_spin_links(nop, new);
    return(new);
}

/*
 * ipaccess_sse_check_established
 * Check TCP ACK and RST bits.
 */

static spintype *ipaccess_sse_check_established (ssetype *sse,
					  sse_ipaccessinfo *ipaccinfo,
					  spintype *new, spintype *term)
{
    spintype *nop;

    SSE_SPIN(nop, 0x14, SSE_INST_NOP);
    new->LTINST = SSE_INST_RD_IPTCPFLAGS;
    new->LTADDR = nop;
    SSE_SPIN(new, 0, SSE_INST_NOP);
    sse_init_spin_links(nop, new);
    return(new);
}

/*
 * ipaccess_sse_get_tp_hdr
 * Unconditionally read enough transport layer header information for extended
 * access list checks.  We could defer reading the transport layer header until
 * it is actually needed, but then each extended access list entry term would
 * have to check to see if it is already read or not.
 */

static spintype *ipaccess_sse_get_tp_hdr (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				   spintype **final)
{
    spintype *old, *new, *term;
    int i;

    term = NULL;
    SSE_SPIN(old, 0, SSE_INST_RD_BYTE|SSE_INST_LD_IPSPORT_MSB);
    term = old;
    SSE_SPIN(new, 0, SSE_INST_RD_BYTE|SSE_INST_LD_IPSPORT_LSB);
    sse_init_spin_links(old, new);
    SSE_SPIN(old, 0, SSE_INST_RD_BYTE|SSE_INST_LD_IPDPORT_MSB);
    sse_init_spin_links(new, old);
    SSE_SPIN(new, 0, SSE_INST_RD_BYTE|SSE_INST_LD_IPDPORT_LSB);
    sse_init_spin_links(old, new);
    /*
     * Skip uninteresting bytes of transport layer header
     */
    for (i = 0; i < 9; i++) {
	old = new;
	SSE_SPIN(new, 0, SSE_INST_RD_BYTE);
	sse_init_spin_links(old, new);
    }
    /*
     * Switch back to page 0 of relocation registers before returning
     */
    SSE_SPIN(old, 0, SSE_INST_LD_SPAGE);
    sse_init_spin_links(new, old);
    SSE_SPIN(new, 0x80, SSE_INST_RD_BYTE|SSE_INST_LD_IPTCPFLAGS);
    sse_init_spin_links(old, new);
    *final = new;
    return(term);
}

/*
 * ipaccess_sse_slow_term
 * Generate spin nodes for a particular entry in an extended IP access list.
 */

static spintype *ipaccess_sse_slow_term (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				  slowtype *item)
{
    treecmptype value;
    int byte;
    uchar addrbytes[2*sizeof(ipaddrtype)];
    uchar maskbytes[2*sizeof(ipaddrtype)];
    spintype *term, **pred_eq, **pred_lt, **pred_gt, *new, *nop;
    boolean first;
    spin_cell *link_pt;

    link_pt = NULL;
    term = NULL;
    pred_eq = &term;
    pred_lt = NULL;
    pred_gt = NULL;
    value = 0;
    first = TRUE;
    bcopy(&item->source, addrbytes, sizeof(ipaddrtype));
    bcopy(&item->destination, &addrbytes[sizeof(ipaddrtype)],
	  sizeof(ipaddrtype));
    bcopy(&item->srcmask, maskbytes, sizeof(ipaddrtype));
    bcopy(&item->dstmask, &maskbytes[sizeof(ipaddrtype)],
	  sizeof(ipaddrtype));
    for (byte = 0; byte < 2*sizeof(ipaddrtype); byte++) {
	if (maskbytes[byte] == 0xff) 
	    continue;

	/*
	 * The first node for the byte is for RD_SREG.
	 */
	SSE_SPIN(new, value, SSE_INST_RD_IPSRC + byte);

	/*
	 * The next node is NOP while we delay for the RD_SREG to complete.
	 * The mask for this byte is loaded here.  After promotion, it will
	 * be the data for the RD_SREG instruction.
	 */
	SSE_SPIN(nop, (~maskbytes[byte] & 0xff), SSE_INST_NOP);
	if (first) {
	    first = FALSE;
	    new->LTADDR = nop;
	    new->GTADDR = nop;
	    sse_spinref_plus2(nop);
	}
	new->EQADDR = nop;
	value = addrbytes[byte] & ~maskbytes[byte];
	ipaccess_sse_link_nodes(pred_lt, pred_eq, pred_gt, new);
	pred_eq = &nop->EQADDR;
	pred_lt = &nop->LTADDR;
	pred_gt = &nop->GTADDR;
    }

    /*
     * Ok, add a final dummy node on so that we either get the last
     * compare or just jump to the next term.
     */
    SSE_SPIN(new, value, SSE_INST_NOP);
    ipaccess_sse_link_nodes(pred_lt, pred_eq, pred_gt, new);
    if ((item->protocol == IP_PROT) && !item->fancy) {
	/*
	 * Don't care which IP protocol the packet is, or about the TCP/UDP
	 * port checks, or about tos/precedence.
	 */
    } else {
	if (item->protocol != IP_PROT) {
	    new = ipaccess_sse_check_pro(sse, ipaccinfo, item, new, term,
					 first);
	    first = FALSE;
	    if (!new)
		return(NULL);
	}
	if (item->tos || item->precedence) {
	    new = ipaccess_sse_check_tos(sse, ipaccinfo, item, new, term,
					 first);
	    first = FALSE;
	    if (!new)
		return(NULL);
	}
	if ((item->srcopr == OPC_NONE) && (item->dstopr == OPC_NONE) &&
	    !item->established) {
	    /*
	     * Don't care about transport layer header or port checks.
	     */
	} else {
	    new = ipaccess_sse_check_fo(sse, ipaccinfo, new, term);
	    if (!new)
		return(NULL);
	    if (item->established) {
		new = ipaccess_sse_check_established(sse, ipaccinfo,
						     new, term);
		if (!new)
		    return(NULL);
		link_pt = &new->nodeinfo.compare.gt;
	    } else {
		link_pt = &new->nodeinfo.compare.lt;
	    }
	    if ((item->srcopr != OPC_NONE) || (item->dstopr != OPC_NONE)) {
		switch (item->protocol) {
		case TCP_PROT:
		case UDP_PROT:
		    new = ipaccess_sse_check_udptcp(sse, ipaccinfo, item,
						    new, term, link_pt);
		    if (!new)
			return(NULL);
		    link_pt = &new->nodeinfo.compare.eq;
		    break;
		case ICMP_PROT:
		case IGMP_PROT:
		    /*
		     * These packets are too much of a pain (and not
		     * important enough) to SSE classify.  Punt to slower
		     * code path...
		     */
		    new->LTGRANT = new->EQGRANT = new->GTGRANT = FALSE;
		    new->type = SPIN_COMPARE_FINAL_TERM;
		    return(term);
		    break;
	        default:
		    break;
		}
	    }
	}
    }
    if (first) {
	/*
	 * Access list term was a NOOP.  Treat entire term as an unconditional
	 * match.
	 */
	new->LTGRANT = new->EQGRANT = new->GTGRANT = ITEM_GRANT;
	new->type = SPIN_COMPARE_FINAL_TERM;
    } else {
	if (new->type == SPIN_COMPARE) {
	    /*
	     * Figure out which branch is success branch.
	     */
	    if (link_pt == NULL)
		link_pt = &new->nodeinfo.compare.eq;
	    new->LTGRANT = new->EQGRANT = new->GTGRANT = !ITEM_GRANT;
	    link_pt->grant = ITEM_GRANT;
	}
    }

    return(term);
}

/*
 * ipaccess_sse_walk_acl
 *	Called to walk an Access-List and traverse nested lists where
 *	necessary.
 */
static spintype *ipaccess_sse_walk_acl (ssetype *sse,
					sse_ipaccessinfo *ipaccinfo, 
					slowtype *item, int list, 
					boolean *prev_grant, spintype **pred)
{
    spintype *curr, *tree;
    
    curr = NULL;
    tree = NULL;

    for (; item; item = item->next) {
	process_may_suspend();
	if (ipaccess_sse_outofsync(ipaccinfo)) {
	    return(NULL);
	}
	if (item->dynamic) { 	
	    dynlisttype *nestacl;
	    /*
	     * nested item.  Get nest list & include it here
	     * Save current item then fetch next item
	     */
	    nestacl = item->dynacl_p;
	    if (nestacl && nestacl->item) {
		curr = ipaccess_sse_walk_acl(sse, ipaccinfo, 
					     nestacl->item, list,
					     prev_grant, pred); 
		
	    } else {
		continue;
	    }
	    if (!curr) 
		return(NULL);

	    if (!tree) 
		tree = curr;
	    /*
	     * Stop if we have an unconditional match as the tree
	     * terminates here and the rest of the items are effectively
	     * no-ops anyhow. 
	     */
	    if (curr->type == SPIN_COMPARE_FINAL_TERM)
		return(tree);

	    *pred = curr;
	    curr = NULL;
	    continue;
	} else {
	    curr = ipaccess_sse_slow_term(sse, ipaccinfo, item);
	}
	if (!curr) 
	    return(NULL);
	if (*pred)
	    ipaccess_sse_link_terms(*pred, curr, *prev_grant);
	if (!tree) {
	    tree = curr;
	} else {
	    /*
	     * Fixup refcount because ipaccess_sse_link_terms assumes
	     * all links represent references beyond some initial
	     * reference.
	     */
	    sse_spinref_minus1(curr);
	}

	/*
	 * Stop if we have an unconditional match as the tree
	 * terminates here and the rest of the items are effectively
	 * no-ops anyhow. 
	 */
	if (curr->type == SPIN_COMPARE_FINAL_TERM)
	    return(tree);

	*pred = curr;
	curr = NULL;
	*prev_grant = ITEM_GRANT;
    }
    return(tree);
}

/*
 * ipaccess_sse_buildslow
 * Construct a tree for an extended access list.
 */

static spintype *ipaccess_sse_buildslow (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				  slowtype *item, int list)
{
    spintype *tp_final, *tree, *pred, *curr;
    boolean prev_grant;

    prev_grant = FALSE;
    tree = NULL;
    pred = NULL;
    curr = NULL;
    tp_final = NULL;

    if (item && acl_array[list]->tp_header_required) {
	tree = ipaccess_sse_get_tp_hdr(sse, ipaccinfo, &tp_final);
	if (!tree)
	    return(NULL);
    }
    curr = ipaccess_sse_walk_acl(sse, ipaccinfo, item, list,
				 &prev_grant, &pred);
    if (!curr) {
	sse_free_tree(sse, tree, &ipaccinfo->ipaccess_stat);
	return(NULL);
    }
    if (tp_final) {
	/*
	 * Unconditional link transport layer header read to beginning
	 */
	sse_init_spin_links(tp_final, curr);
	tp_final = NULL;
    }
    if (!tree) 
	tree = curr;
    return(tree);
}

/*
 * ipaccess_sse_buildtree
 * Construct a tree for an access list.
 */

static spintype *ipaccess_sse_buildtree (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				  fasttype *grp, int list) 
{
    int j;
    fasthash *acc;
    fastitem *item;
    spintype *tree, *pred, *curr;
    boolean prev_grant;

    tree = NULL;
    pred = NULL;
    curr = NULL;
    prev_grant = FALSE;
    /*
     * Scan the hash table of single addresses
     */
    for (j = 0; j < NETHASHLEN; j++) {
	for (acc = grp->hashlist[j]; acc; acc = acc->next) {
	    curr = ipaccess_sse_term(sse, ipaccinfo, acc->grant,
				     acc->address, 0); 
	    if (!curr) {
		sse_free_tree(sse, tree, &ipaccinfo->ipaccess_stat);
		return(NULL);
	    }
	    if (pred)
		ipaccess_sse_link_terms(pred, curr, prev_grant);
	    pred = curr;
	    if (!tree) {
		tree = curr;
	    } else {
		/*
		 * Fixup refcount because ipaccess_sse_link_terms assumes
		 * all links represent references beyond some initial
		 * reference.
		 */
		sse_spinref_minus1(curr);
	    }
	    curr = NULL;
	    prev_grant = acc->grant;
	}
    }
    /*
     * Scan the table of wildcarded entries
     */
    for (item = grp->items; item; item = item->next)  {
	process_may_suspend();
        if (ipaccess_sse_outofsync(ipaccinfo)) {
	    sse_free_tree(sse, tree, &ipaccinfo->ipaccess_stat);
	    return(NULL);
	}
	curr = ipaccess_sse_term(sse, ipaccinfo, item->grant,
				 item->permitmask, item->wildmask);
	if (!curr) {
	    sse_free_tree(sse, tree, &ipaccinfo->ipaccess_stat);
	    return(NULL);
	}
	ipaccess_sse_link_terms(pred, curr, prev_grant);
	pred = curr;
	if (!tree)
	    tree = curr;
	else {
	    if (mem_refcount(curr) == 1) {

		/*
		 * No new links implies that the previous tree was a universal
		 * match.  Don't bother evaluating anymore.
		 */
		sse_free_tree(sse, curr, &ipaccinfo->ipaccess_stat);
		return(tree);
	    }
	    sse_spinref_minus1(curr);
	}
	curr = NULL;
	prev_grant = item->grant;
    }
    return(tree);
}

/*
 * ipaccess_sse_code_gen
 * Generate code for a tree.  It's already mapped, so just push it down.
 */

int ipaccess_sse_code_gen (ssetype *sse, spintype *spin, ssestattype *stat,
			   queuetype *queue)
{
    shadowwordtype sw;

    if (spin->physaddr == SSE_UNMAPPED) {
	errmsg(&msgsym(SWFAILURE, SSE), "tried to enqueue unmapped node",
	       spin);
	return(OPCODE_ABORT);
    }
    switch (spin->type) {
    case SPIN_COMPARE_FINAL_TERM:
    case SPIN_COMPARE_NXT_TERM:
    case SPIN_COMPARE:
        if (spin->LTADDR) {
   	    sw.lt.ptr = spin->LTADDR->physaddr;
	    sw.lt.value = spin->LTADDR->value;
	} else {
	    if (spin->LTGRANT) 
		sw.lt.ptr = SSE_GLOBAL_RETURN;
	    else
		sw.lt.ptr = SSE_GLOBAL_DENY;
	    sw.lt.value = 0;
	}
        if (spin->EQADDR) {
   	    sw.eq.ptr = spin->EQADDR->physaddr;
	    sw.eq.value = spin->EQADDR->value;
	} else {

	    /*
	     * Generate a deny or accept.
	     */
	    if (spin->EQGRANT)
		sw.eq.ptr = SSE_GLOBAL_RETURN;
	    else
		sw.eq.ptr = SSE_GLOBAL_DENY;
	    sw.eq.value = 0;
	}
        if (spin->GTADDR) {
   	    sw.gt.ptr = spin->GTADDR->physaddr;
	    sw.gt.value = spin->GTADDR->value;
	} else {
	    if (spin->GTGRANT)
		sw.gt.ptr = SSE_GLOBAL_RETURN;
	    else
		sw.gt.ptr = SSE_GLOBAL_DENY;
	    sw.gt.value = 0;
	}
	break;   
    default:
	if (sse_debug)
	    buginf("\nSSE: ipaccess_sse_code_gen unexpected type %d",
		   spin->type);
	return(OPCODE_ABORT);
    }
    sw.lt.inst = spin->LTINST;	
    sw.eq.inst = spin->EQINST;
    sw.gt.inst = spin->GTINST;
    sse_poke_treemem(sse, spin->physaddr, &sw);
    return(OPCODE_WALK);
}


/*
 * ipaccess_sse_newtree
 * Calculate a new tree for an idb.
 */

spintype *ipaccess_sse_newtree (ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				int list)
{
    spintype *tree;
    int retval;
    fasttype *grp;
    slowtype *item;

    if ((list > MAXSLOWACC) || (list < MINFASTACC))
	return(NULL);
    if (list < MINSLOWACC) {
	grp = (fasttype *) ip_getacl(list);
	if (!grp)
	    return(NULL);
	tree = ipaccess_sse_buildtree(sse, ipaccinfo, grp, list);
    } else {
	item = (slowtype *) ip_getacl(list);
	if (!item)
	    return(NULL);
	tree = ipaccess_sse_buildslow(sse, ipaccinfo, item, list);
    }
    retval = sse_tree_walk(sse, tree, sse_address_node, NULL,
			   &ipaccinfo->ipaccess_stat, NULL);
    if (OPCODE_ABORT == retval) {
      sse_slow_free_tree(sse, tree, &ipaccinfo->ipaccess_stat);
      return(NULL);
    }

    retval = sse_tree_walk(sse, tree, ipaccess_sse_code_gen,
                         NULL, &ipaccinfo->ipaccess_stat, NULL);

    if (OPCODE_ABORT == retval) {
      sse_slow_free_tree(sse, tree, &ipaccinfo->ipaccess_stat);
      return(NULL);
    }

    return(tree);
}

/*
 * ipaccess_sse_add_acl
 * If a cache entry for the given ACL doesn't already exist, create one.
 */
static boolean ipaccess_sse_add_acl(ssetype *sse, sse_ipaccessinfo *ipaccinfo,
			     int list)
{
    spintype *newtree;

    if ((list < MINFASTACC) || (list > MAXSLOWACC))
	return(FALSE);

    if (!ipaccinfo->ipaccess_cache[list]) {
	ipaccess_sse_cache_setup(sse, ipaccinfo, list);
	if (!ipaccinfo->ipaccess_cache[list])
	    return(FALSE);
    }

    /*
     * Create new tree.
     */
    if (!ipaccinfo->ipaccess_cache[list]->ip_access_tree) {

	if (sse_debug)
	    buginf("\nSSE: Building IP access list %d", list);

	newtree = ipaccess_sse_newtree(sse, ipaccinfo, list);
	if (!newtree) {
	    ipaccess_sse_deny(sse, ipaccinfo, list);
	    ipaccess_sse_deletetree(sse, ipaccinfo, list);
	    return(FALSE);
	} else {
	    ipaccess_sse_backpatch(sse, ipaccinfo, list, newtree);
	    ipaccinfo->ipaccess_cache[list]->ip_access_tree
		= newtree;
	}
    }

    return(TRUE);
}

/*
 * ipaccess_sse_add_input_acl
 * Add icb to input ACL tree.
 */

static void ipaccess_sse_add_input_acl(ssetype *sse, sse_ipaccessinfo *ipaccinfo,
				hwidbtype *hwidb, uint input_list) 
{
    idbtype *idb;
    acl_headertype *first_list;
    boolean multiple_iacls;

    /*
     * If more than one input access list is defined for the swidbs
     * associated with this hwidb, then punt the packets up to the RP
     * and let it sort things out.
     */

    multiple_iacls = FALSE;
    first_list = acl_array[input_list];
	
    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	if (idb->ip_input_accesslist) {
	    if (!first_list) {
		first_list = idb->ip_input_accesslist;
		continue;
	    } else if (first_list != idb->ip_input_accesslist) {
		multiple_iacls = TRUE;
		break;
	    }
	}
    }
	

    if (multiple_iacls) {
	
	if (!ipaccess_sse_iacltree_add_node(sse, ipaccinfo, hwidb, input_list,
					    SSE_GLOBAL_DENY)) {  
	    /*
	     * If an add failed, clean up the tree.
	     */
	    sse_slow_free_tree(sse, ipaccinfo->iacl_tree,
			       &ipaccinfo->ipaccess_stat);
	}
	return;
    }

    /*
     * Make sure ACL is in the cache.
     */

    if (!ipaccess_sse_add_acl(sse, ipaccinfo, input_list))
	return;

    if (!ipaccess_sse_iacltree_add_node(sse, ipaccinfo, hwidb, input_list,
			 ipaccinfo->ipaccess_cache[input_list]->patchpoint)) { 
	/*
	 * If an add failed, clean up the tree.
	 */
	sse_slow_free_tree(sse, ipaccinfo->iacl_tree,
			   &ipaccinfo->ipaccess_stat);
	
	return;
    }
}

/*
 * ipaccess_sse_update
 * Update all of the access lists for interfaces on this sse.
 */

void ipaccess_sse_update (ssetype *sse, sse_protoinfo *protoinfo)
{
    idbtype *idb;
    hwidbtype *hwidb;
    sse_ipaccessinfo *ipaccinfo = protoinfo->ipaccessinfo;
    int retval;
    shadowwordtype sw;
    uint inlist = 0, outlist = 0;

    if (!ipaccinfo)
	return;
    if (!ipaccess_sse_outofsync(ipaccinfo))
	return;
    ipaccinfo->generated = ipaccinfo->version;

    /*
     * Before we start, suspend to see if the world is still changing.
     */

    process_suspend();

    if (ipaccess_sse_outofsync(ipaccinfo))
	return;
    /*
     * Force recalculation of routing trees
     */
    ipfast_clear_sse(protoinfo);	
    
    /*
     * Clean out existing ACL trees
     */
    ipaccess_sse_clean(sse, protoinfo);

    FOR_ALL_SWIDBS(idb) {
	
	/*
	 * Suspend just in case this is long
	 */
	process_may_suspend();			
        if (ipaccess_sse_outofsync(ipaccinfo)) {
	    /*
	     * ACLs are changing out from under us.
	     */
	    return;
        }

	/*
	 * First, make sure we want to build a tree.
	 */
	hwidb = idb->hwptr;
	if (sse->real) {
	    if (!hwidb)
		continue;
	    if (!(hwidb->status & IDB_CBUS))
		continue;
	    if (hwidb->devctl != sse->cbus)
		continue;
	}
	
	/*
	 * Add all idbs with input ACLs to the tree, even
	 * if they don't have SSE-switching enabled.
	 */

	if (idb->ip_input_accesslist) {
	    switch (idb->ip_input_accesslist->type) {
	    case ACL_IP_SIMPLE:
	    case ACL_IP_EXTENDED:
		inlist = atoi(idb->ip_input_accesslist->name);
		break;
	    default:
		continue;
	    }
	    ipaccess_sse_add_input_acl(sse, ipaccinfo, hwidb, inlist);
	}

	/*
	 * Add cache entries for output idbs with SSE-switching enabled.
	 */

	if (sse->real && !(hwidb->ip_routecache & IP_SSE_ROUTECACHE))
		continue;

	if (idb->ip_output_accesslist) {
	    switch (idb->ip_output_accesslist->type) {
	    case ACL_IP_SIMPLE:
	    case ACL_IP_EXTENDED:
		outlist = atoi(idb->ip_output_accesslist->name);
		break;
	    default:
		continue;
	    }

	    if (!ipaccess_sse_add_acl(sse, ipaccinfo, outlist))
		continue;
	}
    }
    /*
     * If we have an input ACL tree at this point, install it, else
     * let packets go on their way.
     */
	
    if (ipaccinfo->iacl_tree) {

	retval = sse_tree_walk(sse, ipaccinfo->iacl_tree,
			       sse_address_node, NULL,
			       &ipaccinfo->ipaccess_stat, NULL); 
	if (OPCODE_ABORT == retval) {
	/*
	 * Something is very wrong. 
	 */
	    ipaccess_sse_iacl_deny(sse, ipaccinfo);
	    sse_free_tree(sse, ipaccinfo->iacl_tree,
			  &ipaccinfo->ipaccess_stat); 
	    ipaccinfo->iacl_tree = NULL;
	} else {
	    /*
	     * Generate the tree and backpatch.
	     */
	    sse_tree_walk(sse, ipaccinfo->iacl_tree,
			  ipaccess_sse_iacltree_code_gen, 
			  NULL, NULL, NULL);
	    sse_init_sw(&sw, ipaccinfo->iacl_tree->value,
			SSE_INST_NOP, ipaccinfo->iacl_tree->physaddr);
	    sse_poke_treemem(sse, SSE_GLOBAL_IPIACL, &sw);
	}
    } else {
	sse_init_sw(&sw, 0, SSE_INST_NOP, SSE_GLOBAL_RETURN);
	sse_poke_treemem(sse, SSE_GLOBAL_IPIACL, &sw);
    }
}

/*
 * ipaccess_sse_show_spin_inst
 * Show a particular instruction of a single spin node of an IP access list.
 */

static void ipaccess_sse_show_spin_inst (spin_cell *c)
{
    if (c->addr)
	printf("%8x %2x ", c->addr, c->inst);
    else
	printf("%s   %2x ", c->grant ? "PERMIT" : "DENY  ", c->inst);
}

/*
 * ipaccess_sse_show_spin
 * Display a single spin node of an IP access list.
 */

static int ipaccess_sse_show_spin (ssetype *sse, spintype *spin, ssestattype *stat,
			  queuetype *queue)
{
    int base = (int) stat;		/* Tacky kludge */

    printf("\n%8x %c    %5d %8x ", spin, sse_node_type(spin->type),
	   spin->depth, spin->physaddr);
    switch(spin->type) {
    case SPIN_COMPARE_FINAL_TERM:
    case SPIN_COMPARE_NXT_TERM:
    case SPIN_COMPARE:
	switch (base) {
	case BASE_DECIMAL:
	    printf("%4d", spin->value);
	    break;
	case BASE_HEX:
	default:
	    printf("%4x", spin->value);
	    break;
	}
	ipaccess_sse_show_spin_inst(&spin->nodeinfo.compare.lt);
	ipaccess_sse_show_spin_inst(&spin->nodeinfo.compare.eq);
	ipaccess_sse_show_spin_inst(&spin->nodeinfo.compare.gt);
	printf("%d", spin->parents);
	break;
    default:
	break;
    }
    if (automore_quit())
	return(OPCODE_ABORT);
    return(OPCODE_WALK);
}

/*
 * ipaccess_sse_show_command
 * Show a tree for an interface.
 */

void ipaccess_sse_show_command (parseinfo *csb)
{
    int command, list;
    mcitype *cbus;
    ssetype *sse;
    sse_ipaccessinfo *ipaccinfo;

    command = GETOBJ(int, 1);

    switch (command) {

    case SSE_CMD_ACCESS_INPUT:
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    if (!cbus->sseptr)
		continue;
	    sse = cbus->sseptr;
	    if (!sse)
		continue;
	    ipaccinfo = sse->protoinfo.ipaccessinfo;
	    if (!ipaccinfo)
		continue;
	    if (!ipaccinfo->iacl_tree)
		continue;
    	    show_sse_internals(ipaccinfo->iacl_tree, BASE_HEX, sse_show_spin);
	}
	break;
    case SSE_CMD_ACCESS_LIST:
	list = GETOBJ(int, 2);
	if (list < MINFASTACC || list > MAXSLOWACC)
	return;
	for (cbus = (mcitype *) cbusQ.qhead; cbus; cbus = cbus->next) {
	    sse = cbus->sseptr;
	    if (!sse)
		continue;
	    ipaccinfo = sse->protoinfo.ipaccessinfo;
	    if (!ipaccinfo)
		continue;
	    if (!ipaccinfo->ipaccess_cache[list])
		continue;
	    if (!ipaccinfo->ipaccess_cache[list]->ip_access_tree)
		continue;
	    show_sse_internals(ipaccinfo->ipaccess_cache[list]->ip_access_tree,
			       BASE_DECIMAL, ipaccess_sse_show_spin);
	}
	break;
    default:
	printf("%%Unknown command");
	return;
}

}

/*
 * ipaccess_sse_init
 * Initialize information for an SSE structure.
 */

void ipaccess_sse_init (sse_protoinfo *protoinfo)
{
    sse_ipaccessinfo *ipaccinfo;

    ipaccinfo = malloc(sizeof(sse_ipaccessinfo));
    if (ipaccinfo) {
	ipaccinfo->generated = ipaccinfo->version = 0;
	protoinfo->ipaccessinfo = ipaccinfo;
    }
}

/*
 * ipaccess_sse_reinit
 * Free all data structures as something is horribly wrong.
 */

static void ipaccess_sse_reinit (ssetype *sse, sse_protoinfo *protoinfo)
{
    sse_ipaccessinfo *ipaccinfo;
    int i;

	ipaccinfo = protoinfo->ipaccessinfo;

    if (!ipaccinfo)
	return;

    if (ipaccinfo->iacl_tree) {
	ipaccess_sse_iacl_deny(sse, ipaccinfo);
	sse_free_tree(sse, ipaccinfo->iacl_tree, &ipaccinfo->ipaccess_stat);
	ipaccinfo->iacl_tree = NULL;
    }

    for (i = MINFASTACC; i <= MAXSLOWACC; i++) {
	if (!ipaccinfo->ipaccess_cache[i])
	    continue;
	ipaccess_sse_deny(sse, ipaccinfo, i);
	ipaccess_sse_deletetree(sse, ipaccinfo, i);
	sse_free_treeaddr(sse, &ipaccinfo->ipaccess_stat,
			  ipaccinfo->ipaccess_cache[i]->patchpoint);
	free(ipaccinfo->ipaccess_cache[i]);
	ipaccinfo->ipaccess_cache[i] = NULL;
    }
}

/*
 * ipaccess_sse_print_stats
 * Print stats for this sse.
 */

void ipaccess_sse_print_stats (sse_protoinfo *protoinfo)
{
    sse_ipaccessinfo *ipaccinfo = protoinfo->ipaccessinfo;

    if (!ipaccinfo)
	return;
    printf("\nIP access lists");
    printf(SSE_STAT_STRING SSE_INTERN_STRING,
	   ipaccinfo->ipaccess_stat.words_used, 
	   ipaccinfo->ipaccess_stat.rwbytes_used,
	   ipaccinfo->ipaccess_stat.spin_used);  
}

/*
 * ipaccess_clear_sse
 * Reset the world when a reset happens.
 */

static void ipaccess_clear_sse (sse_protoinfo *protoinfo)
{
    protoinfo->ipaccessinfo->version++;
    ipfast_clear_sse(protoinfo);
}

/*
 * ipaccess_sse_register_thyself
 * Register this file.
 */

static void ipaccess_sse_register_thyself (void)
{
    reg_add_sse_update(ipaccess_sse_update, "ipaccess_sse_update");
    reg_add_sse_dispatch(ipaccess_sse_dispatch, "ipaccess_sse_dispatch");
    reg_add_sse_data_init(ipaccess_sse_init, "ipaccess_sse_init");
    reg_add_sse_protocols_invalid(ipaccess_clear_sse, "ipaccess_clear_sse");
    reg_add_sse_reinit(ipaccess_sse_reinit, "ipaccess_sse_reinit");
    reg_add_sse_protocol_reset(SSE_PROTOCOL_IP, ipfast_clear_sse,
			       "ipfast_clear_sse");
    reg_add_sse_print_stats(ipaccess_sse_print_stats,
			    "ipacces_sse_print_stats");
    reg_add_sse_access_check(sse_access_check,
			    "sse_access_check");  
    reg_add_ip_access_group(sse_access_group_callback,
			    "sse_access_group_callback");  
    reg_add_ip_access_list(sse_access_list_callback,
			   "sse_access_list_callback");
    reg_add_sse_access_get_patchpoint(ipaccess_sse_get_patchpoint,
			       "ipaccess_sse_get_patchpoint");
    reg_add_sse_acl_exists(ipaccess_sse_acl_exists,
			       "ipaccess_sse_acl_exists");
}

/*
 * ip_sse_init
 * Initialize IP for the SSE.
 */

static void ip_sse_init (subsystype *subsys)
{
    /*
     * Warning: this is very order dependent.  It is vital that the access
     * lists are registered first as this cause them to be updated BEFORE
     * the routing table.  Do it the other way 'round and you have a major
     * gaping security hole.
     */
     ipaccess_sse_register_thyself();
     ipfast_sse_register_thyself();

     reg_add_ip_change_mtu(sse_set_mtu, "sse_set_mtu");
     ip_sse_chain_init();
}

/* 
 * ip_sse subsystem header
 */

#define SSE_MAJVERSION 1
#define SSE_MINVERSION 0
#define SSE_EDITVERSION 1

SUBSYS_HEADER(ip_sse, SSE_MAJVERSION, SSE_MINVERSION, SSE_EDITVERSION,
	      ip_sse_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      "req: sse, iphost");
