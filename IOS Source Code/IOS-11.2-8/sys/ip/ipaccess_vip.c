/* $Id: ipaccess_vip.c,v 3.7.6.11 1996/08/28 12:49:41 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipaccess_vip.c,v $
 *------------------------------------------------------------------
 * DFS routines for manipulating IP access lists.
 *
 * September 1995, Don Lobete
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipaccess_vip.c,v $
 * Revision 3.7.6.11  1996/08/28  12:49:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.6.10  1996/08/16  08:02:03  dlobete
 * CSCdi59971:  DACL needs modularity work
 * Branch: California_branch
 * For the sake of future maintenance of IP access-lists, share as much
 * source code as possible between VIP and RSP.
 *
 * Revision 3.7.6.9  1996/08/12  06:12:13  dlobete
 * CSCdi59677:  distributed switching broken if MIP queuing fixed
 * Branch: California_branch
 * Eliminate mci_index from DFS scope and replace with the more
 * suitable hw_if_index. This not only fixes DFS to MIP channel-groups
 * but also paves the way for input from the new channelized port-adaptors
 * into DFS.
 *
 * Revision 3.7.6.8  1996/07/26  00:54:01  hampton
 * Fix the automore code in show access-list.  [CSCdi61084]
 * Branch: California_branch
 *
 * Revision 3.7.6.7  1996/07/20  01:13:19  bew
 * CSCdi63411:  ACL encrypted/decrypted flags are not useful
 * Branch: California_branch
 * Remove encrypted/decrypted flags introduced early in 11.2
 * from ACL code.
 *
 * Revision 3.7.6.6  1996/06/06  09:24:46  dlobete
 * CSCdi57425:  VIP2 max cache entries wt. 32MB & 64MB are the same
 * Branch: California_branch
 *
 * Revision 3.7.6.5  1996/05/28  16:13:35  xliu
 * CSCdi58788:  code changes for vip named access list and encryption perf
 * Branch: California_branch
 *
 * Revision 3.7.6.4  1996/05/21  09:51:16  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.6.3  1996/05/08  01:09:47  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.7.6.2  1996/04/29  04:46:35  dlobete
 * CSCdi54163:  Distributed-switched packets discarded on ATM interface
 * Branch: California_branch
 *
 * Revision 3.7.6.1  1996/04/10  03:38:19  dlobete
 * CSCdi53121:  Distributed IP Access Lists need to run at interrupt-level
 * Branch: California_branch
 *
 * Revision 3.7  1996/03/04  07:12:37  kao
 * CSCdi49674:  While VIP is cofigured with DFS, OIR event will crash the
 * VIP;
 *
 * Revision 3.6  1996/02/01  06:04:25  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/19  03:02:42  dlobete
 * CSCdi44954:  Local-switching for DFS required
 * But that's not all! Also part of this jumbo Distributed IP
 * Switching commit:
 * - CSCdi43302: Illegal Access to Low Address crashes RVIP/4E (RSP2)
 * - DFS config which mirrors RSP route-cache setting
 *         (ie configurable distributed flow or optimal switching)
 * - Interrupt level IPC updates and invalidates for DFS cache operations
 * - Stay in closer synch with RSP cache for prefix invalidations
 *
 * Revision 3.4  1996/01/19  00:57:25  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/17  18:29:38  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:34:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:56:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:04:29  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  05:26:26  dlobete
 * Placeholder for VIP_branch development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include <stdlib.h>
#include "../ui/common_strings.h"
#include "registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "access.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "stdarg.h"
#include "../parser/parser_defs_access.h"
#include "../ipmulticast/parser_defs_igmp.h"
#include "parser_defs_ip.h"
#include "../if/ether.h"
#include "../if/fddi.h"
#include "config.h"
#include "../wan/serial.h"
#include "../ip/ip.h"
#include "../ipmulticast/igmp.h"
#include "../ip/ipaccess_private.h"
#include "../ip/ipaccess.h"
#include "../ip/ipaccount.h"
#include "../ip/ipfast_index.h"
#include "../if/tring.h"
#include "../les/if_les.h"
#include "../wan/frame_relay.h"
#include "../wan/atm_dxi.h"
#include "../wan/smds.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../os/chunk.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

#include "../src-rsp/rsp_bufhdr.h"
#include "../src-rsp/rsp_if.h"
#include "../src-rsp/rsp_qasic.h"
#include "../src-rsp/qa_hw.h"
#include "../src-rsp/rsp_fast.h"
#include "../src-rsp/rsp_pcmap.h"
#include "../src-rsp/rsp_cache.h"

#include "sys_registry.h"
#include "../ip/ip_registry.h"

#include "../parser/parser_actions.h"
#include "../ip/ip_actions.h"

#include "../src-vip/dfs_vip_idb.h"
#include "../ip/ipfast_ipc.h"
#include "../ip/ipfast_vip.h"
#include "../ip/ipfast_vip_ipc.h"
#include "../ip/ipaccess_vip.h"

static chunk_type *ipaccess_chunk;
static ulong 	  ipaccess_mem_in_use;
static parseinfo ipaccess_csb_struct;


watched_queue *DACL_inputQ;


void acl_intface_process (void *acl_int)
{
    ipc_ipfast_acl_ena_msg *acl_intface = (ipc_ipfast_acl_ena_msg *)acl_int;
    ipc_ipfast_acl_inout *intface;
    dfsidbtype *hwidb_dfs, *dfsidb;
    ushort i;

    /*
     * Load up the data we got from the message.
     */
    intface = (ipc_ipfast_acl_inout *) &acl_intface->acl_data;
    for(i=0; i < acl_intface->num_interfaces; i++) {
	hwidb_dfs = dfs_vip_get_hwidb(intface->hw_if_index);
	if (hwidb_dfs == NULL) {
	    /* keep trying then */
	    intface++;
	    continue;
	}

	/*
	 * Only care about input acls that apply to this VIP. And we
	 * care about all output acls.
	 */
	if (intface->in_not_out) {
	    if (hwidb_dfs->hwidb)
		hwidb_dfs->hwidb->fast_rcvidb->ip_input_accesslist =
		    access_find_or_create_acl(intface->ip_in_accesslist);
	    /*
	     * Now do some checking so we can set the access-list flags
	     * correctly (ala the system code). If we have a
	     * non-zero input ACL set all dfsidbs ip_fast_flags. If
	     * a 0 input, ACL check if there are none left.
	     */
	    if (intface->ip_in_accesslist) {
		FOR_ALL_DFSIDBS(dfsidb)
		    dfsidb->ip_fast_flags |= IP_FAST_INPUTACCESSLISTS;
	    } else {
		if (intface->no_inacls) {
                    FOR_ALL_DFSIDBS(dfsidb)
                        dfsidb->ip_fast_flags &= ~IP_FAST_INPUTACCESSLISTS;
		}
	    }
	} else {
	    hwidb_dfs->ip_output_accesslist =
		access_find_or_create_acl(intface->ip_out_accesslist);
	}
	intface++;
    }
}

void show_acl_intface_settings (acl_headertype *acl)
{
    dfsidbtype *hwidb_dfs;
    hwidbtype *hwidb;
    int i;
    int input_acl, output_acl;

    printf("\n\nInterface Access List Configuration");
    for(i=0; i < (MAX_INTERFACES); i++) {
        hwidb_dfs = hwidb_array[i];
        if (hwidb_dfs == NULL)
            continue;
	hwidb = hwidb_dfs->hwidb;
        if ((hwidb && hwidb->fast_rcvidb &&
             hwidb->fast_rcvidb->ip_input_accesslist)
            || hwidb_dfs->ip_output_accesslist) {

            printf("\n  hw_if_index %d: ip_fast_flags 0x%x",
                hwidb_dfs->hw_if_index, hwidb_dfs->ip_fast_flags);

	    if (hwidb && hwidb->fast_rcvidb &&
		hwidb->fast_rcvidb->ip_input_accesslist)
		input_acl = 
		    atoi(hwidb->fast_rcvidb->ip_input_accesslist->name);
	    else
		input_acl = 0;

	    if (hwidb_dfs->ip_output_accesslist)
		output_acl = atoi(hwidb_dfs->ip_output_accesslist->name);
	    else
		output_acl = 0;

            printf(" input acl %d, output acl %d.", input_acl, output_acl);
        }
    }
    printf("\n");
    printf("\nDistributed IP Access Lists memory use: %d bytes\n",
           ipaccess_mem_in_use);
}

/*
 * Copy a fast-message for IP DACL into a DRAM pak and queue
 * for later processing at process-level.
 */
void ipaccess_vip_queue_to_pl (void *message)
{
    ipc_ipfast_msg_hdr *msg = message;
    DACL_inputQ_elt *input_queue_elt;

    input_queue_elt = chunk_malloc(ipaccess_chunk);
    if (!input_queue_elt) {
	return;
    }

    input_queue_elt->next = NULL;
    if (msg->msg_size > IPACCESS_DATA_SIZE_MAX) {
	chunk_free(ipaccess_chunk, input_queue_elt);
	return;
    }
    
    bcopy(message, &input_queue_elt->data[0], msg->msg_size);
    process_enqueue(DACL_inputQ, input_queue_elt);
}

/*
 * Consume from the DACL input queue, prepare a csb structure
 * and call the common IP access routines.
 */
static void ipaccess_vip_process_input (void)
{
    DACL_inputQ_elt *queue_elt;
    ipc_ipfast_msg_hdr *msg;
    ipc_ipfast_acc_fasthash *acl_fh;
    ipc_ipfast_acc_fastitem *acl_fi;
    ipc_ipfast_acc_slowtype *acl_si;
    ipc_ipfast_acl_ena_msg *acl_intface;
    parseinfo *csb = &ipaccess_csb_struct;
    leveltype level;

    while ((queue_elt = process_dequeue(DACL_inputQ))) {
	msg = (ipc_ipfast_msg_hdr *)&queue_elt->data[0];
	switch (msg->ipfast_type) {
	  case IPFAST_ACCESS_ADD_FASTHASH:
	  case IPFAST_ACCESS_DEL_FASTHASH:
	    if (msg->ipfast_type == IPFAST_ACCESS_ADD_FASTHASH)
		csb->sense = TRUE;
	    else
		csb->sense = FALSE;
	    csb->nvgen = FALSE;

	    acl_fh = (ipc_ipfast_acc_fasthash *) msg;
	    SETOBJ(int, 1) = acl_fh->list_number;
	    SETOBJ(int, 2) = acl_fh->grant;
	    SETOBJ(paddr, 1).ip_addr = acl_fh->address;
	    SETOBJ(paddr, 2).ip_addr = 0;

	    level = raise_interrupt_level(NETS_DISABLE);
	    ip1access_command(csb);
	    reset_interrupt_level(level);
	    break;
 
	  case IPFAST_ACCESS_ADD_FASTITEM:
	  case IPFAST_ACCESS_DEL_FASTITEM:
	    if (msg->ipfast_type == IPFAST_ACCESS_ADD_FASTITEM)
		csb->sense = TRUE;
	    else
		csb->sense = FALSE;
	    csb->nvgen = FALSE;

	    acl_fi = (ipc_ipfast_acc_fastitem *) msg;
            SETOBJ(int, 1) = acl_fi->list_number;
            SETOBJ(int, 2) = acl_fi->grant;
            SETOBJ(paddr, 1).ip_addr = acl_fi->permitmask;
            SETOBJ(paddr, 2).ip_addr = acl_fi->wildmask;

            level = raise_interrupt_level(NETS_DISABLE);
            ip1access_command(csb);
	    reset_interrupt_level(level);
	    break;
 
	  case IPFAST_ACCESS_ADD_SLOWITEM:
	  case IPFAST_ACCESS_DEL_SLOWITEM:
	    if (msg->ipfast_type == IPFAST_ACCESS_ADD_SLOWITEM)
		csb->sense = TRUE;
	    else
		 csb->sense = FALSE;
	    csb->nvgen = FALSE;

	    acl_si = (ipc_ipfast_acc_slowtype *) msg;
            SETOBJ(int, 1) = acl_si->list_number;
            SETOBJ(int, 2) = acl_si->grant;
	    SETOBJ(int, 3)  = acl_si->protocol;
	    SETOBJ(int, 4) = acl_si->dstopr;
	    SETOBJ(int, 5) = acl_si->dport;
	    SETOBJ(int, 6) = acl_si->dport2;
	    SETOBJ(int, 7) = acl_si->established;
	    SETOBJ(int, 8) = acl_si->srcopr;
	    SETOBJ(int, 9) = acl_si->sport;
	    SETOBJ(int, 10) = acl_si->sport2;
	    SETOBJ(int, 11) = acl_si->precedence;
	    SETOBJ(int, 12) = acl_si->precedence_value;
	    SETOBJ(int, 13) = acl_si->tos;
	    SETOBJ(int, 14) = acl_si->tos_value;
	    SETOBJ(int, 15) = acl_si->log;
	    SETOBJ(int, 16) = FALSE; 		/* dynamic */
	    SETOBJ(int, 17) = 0;	    	/* dynamic-timeout */
	    SETOBJ(paddr, 1).ip_addr = acl_si->source;
	    SETOBJ(paddr, 2).ip_addr = acl_si->srcmask;
	    SETOBJ(paddr, 3).ip_addr = acl_si->destination;
	    SETOBJ(paddr, 4).ip_addr = acl_si->dstmask;

	    level = raise_interrupt_level(NETS_DISABLE);
	    ip2access_command(csb);
	    reset_interrupt_level(level);
	    break;
 
	  case IPFAST_IP_ACL_INTFACE:
	    acl_intface = (ipc_ipfast_acl_ena_msg *) msg;
	    level = raise_interrupt_level(NETS_DISABLE);
	    acl_intface_process(acl_intface);
	    reset_interrupt_level(level);
	    break;
	}
	chunk_free(ipaccess_chunk, queue_elt);
    }
}

process ipaccess_vip_input (void)
{
    ulong major, minor;

    process_watch_queue(DACL_inputQ, ENABLE, RECURRING);

    while (TRUE) {
        process_wait_for_event();
        while (process_get_wakeup(&major, &minor)) {
            switch (major) {
              case QUEUE_EVENT:
                ipaccess_vip_process_input();
                break;
 
              default:
                errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
                break;
            }
        }
    }
}

/*
 * ip_printf
 * I/i prints a dotted Internet address.  Takes an integer argument.
 */
int ip_printf (const char **fmt, char **cp, tt_soc *tty, va_list *argp,
	       boolean rjust, int width, int size, int hash, int precision)
{
    ulong n;
    int length, d, charcount;
    uchar digits[16];

    charcount = 0;
    n = va_arg(*argp, ipaddrtype);
    length = sprintf(digits, "%d.%d.%d.%d",
		     (n >> 24), ((n & 077600000) >> 16),
		     ((n & 0177400) >> 8), (n & 0377));

    if (rjust && width != -1 && (length < width))
	for (d = 0; d < (width - length); d++) {
	    doprintc(cp, tty, ' '); /* Leading fill - spaces */
	    charcount++;
	}

    if (*cp) {
	strcpy(*cp, digits);
	*cp += length;
    } else
	_ttyprintf(tty, "%s", digits);
    charcount += length;
    if (!rjust && width != -1 && (length < width)) {
	for (d = 0; d < (width-length); d++) {
	    doprintc(cp,tty,' '); /* trailing fill - spaces*/
	    charcount++;
	}
    }
    return(charcount);
}

void ipaccess_vip_init (ulong mem_size)
{
    ulong dacl_entries = 0;

    /*
     * Set up the chunk required for use in our process-level
     * DACL queue.
     */
    switch (mem_size) {
      case DFS_64MB_SIZE:
	dacl_entries = IPACCESS_BUFFS_MAX_HUGE;
	break;
      case DFS_32MB_SIZE:
	dacl_entries = IPACCESS_BUFFS_MAX_LARGE;
	break;
      case DFS_16MB_SIZE:
	dacl_entries = IPACCESS_BUFFS_MAX_MEDIUM;
	break;
      case DFS_8MB_SIZE:
      default:
	dacl_entries = IPACCESS_BUFFS_MAX_SMALL;
	break;
    }
    ipaccess_chunk = chunk_create(IPACCESS_BUFF_SIZE_MAX,
                                        dacl_entries,
					CHUNK_FLAGS_NONE, NULL,
					0, "DACL Queue Elements");
    ipaccess_mem_in_use += (IPACCESS_BUFF_SIZE_MAX+sizeof(DACL_inputQ_elt)) * 
	                          dacl_entries;

    /*
     * Queue and process initialization.
     */
    DACL_inputQ = create_watched_queue("DACL messages", 0, 0);
    process_create(ipaccess_vip_input, "DACL Input", LARGE_STACK, PRIO_HIGH);
    
    /*
     * IP Printf initialization.
     */
    reg_add_printf((long)'I', ip_printf, "ip_printf");
    reg_add_printf((long)'i', ip_printf, "ip_printf");
}
