/* $Id: ipcios.c,v 1.1.4.1 1996/05/09 14:55:23 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_ibipc/ipcios.c,v $
 *------------------------------------------------------------------
 * ipcios.c
 *
 * March 1995, jjchen
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipcios.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:23  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:51  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:29  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:43  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.3  1996/03/23  04:48:10  rlowe
 * Fix include file usage and some makefile rules.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.2  1996/03/22  10:19:02  rlowe
 * Clean up include file usage.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:59  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:39  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:51  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:19  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.6  1995/10/04  01:07:45  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.5  1995/08/31  12:02:40  rlowe
 * Changes to work with IOS 11.0 and higher.
 *
 * Revision 1.4  1995/08/01  05:17:41  rlowe
 * Resurrect IOS header "sys_registry.h".  Ugh !
 *
 * Revision 1.3  1995/08/01  03:01:10  rlowe
 * Remove dependency on IOS header file "sys_registry.h" and its
 * sub-dependencies.
 *
 * Revision 1.2  1995/05/05  16:35:46  jjchen
 * Added MAC address learning for IPC.
 *
 * Revision 1.1  1995/05/01  23:53:10  jjchen
 * Moved from syn_core/nmp/nmpflash/ibipc.
 *
 * Revision 1.1  1995/03/29  05:45:18  jjchen
 * Created.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../../../sys/h/master.h"
#include "../../../sys/h/subsys.h"
#include "../../../sys/h/sys_registry.h"

/*
 * Global variables
 */

queuetype ipcQ;                        /* Queue of CDP packets */
boolean ipc_running;                   /* True if CDP running */
int ipcfork;                           /* Fork of CDP process */

/*
 * Function declarations
 */
void ipc_init (subsystype *);
forktype ipc_process (void);
void ipc_clean_up(void);
void ipc_add_registry_calls (void);
void ipc_start_process (void);

/*
 * IPC subsystem header
 */
#define LINK_IPC 63 /* !!! move this into linktype.h, duplicated one in ibipc.c */

#define IPC_MAJVERSION 1
#define IPC_MINVERSION 0
#define IPC_EDITVERSION  1

SUBSYS_HEADER(ipc, IPC_MAJVERSION, IPC_MINVERSION, IPC_EDITVERSION,
	      ipc_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);

/*
 * ipc_init
 *
 * Initialize all IPC-related stuff.
 */
void ipc_init (subsystype * pSubSys)
{
    int i;
    idbtype *idb;

/*   ipc_media_registry();*/
/*    ipc_add_registry_calls();*/
/*   ipc_debug_init();*/
/*   ipc_parser_init();*/
    queue_init(&ipcQ, 0);

/* ! may be needed
   FOR_ALL_SWIDBS(idb) {
	idb->ipc_enabled = FALSE;
    }
*/
    ipc_cfg_address();
/*    ipc_start_process();*/
}

/*
 * ipc_running_test
 *
 * Return TRUE if IPC is running on this system.
 */
static boolean ipc_running_test (void) 
{
    return(ipc_running);
}

/*
 * ipc_on_idb 
 *
 * Return TRUE if IPC is running on the particular interface.
 */
static boolean ipc_on_idb (idbtype *idb)
{
/* ! may be needed
    if ((!ipc_running) || (!idb->ipc_enabled)) {
	return(FALSE);
    } else {
	return(TRUE);
    }
*/
    return(FALSE);   
}

/*
 * ipc_start_process
 *
 * Start the IPC process
 */
void ipc_start_process (void)
{
    if (!ipcfork)
	ipcfork = cfork((forkproc *)ipc_process, 0L, 0, "IPC Process", 0);
}

/*
 * ipc_enqueue
 * Called from interrupt level to put a packet on the IPC input queue
 */

void ipc_enqueue (paktype *pak)
{
/*
    if (pak->if_input->ipc_enabled) {
	net_bridge_copy_broadcast(pak);
*/
 	p_enqueue(&ipcQ, (void *)pak);
/*
    } else
	netinput_enqueue(pak);
*/
}

/*
 * ipcBLOCK
 *
 * Block waiting for something for the ipc process to do.
 */

boolean ipcBLOCK (queuetype *q)
{
    if (q->qhead)
	return(FALSE);			/* input available */

    return(TRUE);
}

/*
 * ipc_process
 * 
 * Main IPC (WBU Synergy Inter-Process Communication) process
 */

forktype ipc_process (void)
{
    int count;
    paktype *pak;

    edisms((blockproc *)systeminitBLOCK, (ulong) (NULL));

    ipc_running = TRUE;
    
    while (ipc_running) {
	edisms((blockproc *)ipcBLOCK, (ulong)&ipcQ);

	/* Process a limited number of packets */
	count = 1;

	while (count-- > 0) {	
	    pak = (paktype *) p_dequeue(&ipcQ); 
	    if (!pak)
		break;
/* ! may be needed
	    pak->if_input->hwptr->rx_frames[ACCT_PROTO_IPC]++;
	    pak->if_input->hwptr->rx_bytes[ACCT_PROTO_IPC] += pak->datagramsize;
*/

	    Ipc_rx(pak);

/*	    datagram_done(pak);*/
	}
    }

    /* Clean up ... */
    ipc_clean_up();

    /* Die ... */
    ipcfork = 0;
    process_kill (THIS_PROCESS);
}

/*
 * ipc_clean_up
 *
 */
void ipc_clean_up(void)
{

/*    reg_invoke_bridge_entry_global_remove(multicast); */
}

/*
 * ipc_reload_notify
 *
 * Notify all interfaces that we are going down by sending out a
 * packet with TTL 0.
 */
void ipc_reload_notify (void)
{
}

/*
 * ipc_if_down_notify
 *
 */
void ipc_if_down_notify (idbtype *idb)
{
}

/*
 * ipc_if_up_notify
 *
 */
void ipc_if_up_notify (idbtype *idb)
{
}

/*
 * ipc_add_registry_calls
 *
 * Do all registry-related stuff here.
 */
void ipc_add_registry_calls (void)
{
    reg_add_raw_enqueue(LINK_IPC, ipc_enqueue, "ipc_enqueue");
    reg_add_proto_running(LINK_IPC, ipc_running_test, "ipc_running_test");
    reg_add_proto_on_swidb(LINK_IPC, ipc_on_idb, "ipc_on_idb");
    reg_add_will_reload(ipc_reload_notify, "ipc_reload_notify");
    reg_add_swif_goingdown(ipc_if_down_notify, "ipc_if_down_notify");
    reg_add_swif_comingup(ipc_if_up_notify, "ipc_if_up_notify");
}
