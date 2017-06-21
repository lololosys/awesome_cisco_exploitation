/* $Id: smrp_globals.c,v 3.3.20.1 1996/05/02 08:44:47 dwong Exp $
 * $Source: /release/112/cvs/Xsys/smrp/smrp_globals.c,v $
 *------------------------------------------------------------------
 * Globals for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: smrp_globals.c,v $
 * Revision 3.3.20.1  1996/05/02  08:44:47  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.3  1996/01/25  11:22:38  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.2  1995/11/17  18:53:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:13:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:25:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/11  19:02:13  mleelani
 * CSCdi41471:  Add support for pruning duplicate paths
 * o Check if we are the right parent when we receive member
 * responses.
 * o Assign PortParentaddresses on other ports correctly, when
 * better routes are received.
 * o Negotiate portparents using PortParentUnreachable Vector,
 * when a new port comes up.
 *
 * Revision 2.8  1995/09/15  22:06:28  mleelani
 * CSCdi39756:  2 forwarding paths exist when join to group across routers.
 * o Update port parents for directly connected networks also.
 * o Start PortParent negotiation whenever the port parent address
 *   has to be set to its own port address.
 * o Reduce the PortParent timer for faster convergence.
 *
 * Revision 2.7  1995/08/30  22:43:29  mleelani
 * CSCdi38436:  SMRP: Duplicate paths when the previous parent poisons the
 * route.
 * Wrong Port Parent addresses get assigned during race conditions; these
 * lead to multiple data paths.
 *
 * Revision 2.6  1995/07/24  22:30:49  hampton
 * Further work to parts of Appletalk to make it use the passive timer
 * macros for all its timers.  It no longer have any direct references to
 * the system clock.  [CSCdi37581]
 *
 * Revision 2.5  1995/06/28 09:30:28  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.4  1995/06/23  18:49:12  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.3  1995/06/20  07:17:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/14  22:54:58  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Convert SMRP processes to use new scheduler.
 * - Access SMRP fields from idb sub-block.
 * - Include interface.h instead of interface_private.h whereever
 *   possible.
 * - Misc SMRP bugfixes.
 *
 * Revision 2.1  1995/06/07  22:45:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface.h"
#include "packet.h"
#include "subsys.h"
#include "logger.h"
#include "address.h"
#include "config.h"
#include "smrp_private.h"
#include "smrp_registry.h"
#include "../smrp/smrp_registry.regc"

/*
 *==================================================================
 * SMRP Global Data
 *
 * Most SMRP process specific data is private and only declared in
 * the module that is responsible for it.  Data defined here only
 * when it must be shared between modules (such as queues) or
 * when it must be modified by the user interface.
 *==================================================================
 */ 

uchar const SMRPVersion = 1;

tinybool smrp_running;		/* SMRP is running now */
tinybool smrp_test_running = FALSE;	/* SMRP testing is running now */
long smrp_regcode;		/* SMRP private registery */

ulong *smrp_stat;		/* SMRP statistics */

ulong smrp_requestRetries;	/* SMRP protocol parameters */
ulong smrp_requestInterval;
ulong smrp_responseInterval;
ulong smrp_creatorInterval;
ulong smrp_memberInterval;
ulong smrp_helloInterval;
ulong smrp_nodeAgeInterval;
ulong smrp_purgeRouteInterval;

pid_t smrp_input_pid;		/* Process ID for input process */
pid_t smrp_bg_pid;		/* Process ID for background process */
pid_t smrp_route_pid;		/* Process ID for route process */
pid_t smrp_maint_pid;		/* Process ID for maint process */

watched_queue *smrpbgQ;		/* Watched Queue for background process */
watched_queue *smrpRouteQ;	/* Watched Queue for routing process */
watched_queue *smrpNodeQ;	/* Watched Queue for maint process */

static void smrp_init(subsystype *);
extern void smrp_parser_init(void);


/*
 * SMRP subsystem header
 */

#define SMRP_MAJVERSION 1
#define SMRP_MINVERSION 0
#define SMRP_EDITVERSION 1

SUBSYS_HEADER(smrp, SMRP_MAJVERSION, SMRP_MINVERSION, SMRP_EDITVERSION,
	      smrp_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: atalk", NULL);

/*
 * smrp_register_thyself
 */
static void 
smrp_register_thyself (void)
{
    smrp_parse_Initialize();

    reg_add_media_interesting_addresses(smrp_addresses, "smrp_addresses");
    
    reg_add_reset_if(smrp_ResetIFNotify, "smrp_ResetIFNotify");
    reg_add_route_adjust(smrp_route_adjust, "smrp_route_adjust");
    reg_add_showproto(smrp_ShowProto, "smrp_ShowProto");
#if NOT_USED
    reg_add_encap_fixup(smrp_IFFixupNotify, "smrp_IFFixupNotify"); 
    reg_add_clear_counters(atalkif_ClearCounters, "atalkif_ClearCounters");
    reg_add_access_show(show_smrpaccess, "show_smrpaccess");
    reg_add_setup_global(smrp_SetupGlobal, "smrp_SetupGlobal");
#endif

}

/* 
 * smrp_init
 * Initialize the SMRP protocol machine
 */
static void
smrp_init (subsystype *subsys)
{
    ushort	i;

    smrp_register_thyself();

    /* Set up debugging... */
    smrp_debug_init();
    
    smrp_stat = malloc(sizeof(ulong) * SMRP_MAXTRAFFIC);
    if (smrp_stat == NULL) {
        return;
    }
    for (i = 0; i < SMRP_MAXTRAFFIC; i++)
	smrp_stat[i] = 0;
    
    smrp_running = FALSE;

    smrp_requestRetries = MaxRetries;
    smrp_requestInterval = RequestInterval;
    smrp_responseInterval = ResponseInterval;
    smrp_creatorInterval = CreatorInterval;
    smrp_memberInterval = MemberInterval;
    smrp_helloInterval = HelloInterval;
    smrp_nodeAgeInterval = NodeAgeInterval;
    smrp_purgeRouteInterval = PurgeRouteInterval;

    smrp_group_Initialize();		/* initialize group module */
    smrp_input_Initialize();		/* initialize input module */
    smrp_maint_Initialize();		/* initialize maintenance module */
    smrp_node_Initialize();		/* initialize node module */
    smrp_port_Initialize();		/* initialize port module */
    smrp_parse_Initialize();		/* initialize parse module */
    smrp_route_Initialize();		/* initialize routing module */
    smrp_trans_Initialize();		/* initialize transaction module */
    smrp_util_Initialize();		/* initialize utility module */
    smrpfast_Initialize();		/* initialize fast switching module */
    smrp_parser_init();		        /* initialize parse chain module */
    
    
    /* Set up SMRP private function registry */
    smrp_regcode = registry_create(REG_UNKNOWN, SERVICE_SMRP_MAX, "SMRP");
    if (smrp_regcode != REG_ILLEGAL)
	create_registry_smrp();
}

/*
 * smrp_start
 * start smrp processing
 */
void
smrp_start (void)
{
    /*
     * anything beyond here should only be started once
     */
    if (smrp_running)
	return;
    
    if (smrp_input_pid || smrp_bg_pid || smrp_route_pid || smrp_maint_pid) {
	printf("\n%%Previously running SMRP processes have not yet stopped. "
	       "\nPlease wait a few seconds and reissue this command");
	return;
    }
    
    smrp_running = TRUE;

    smrp_input_pid = process_create(smrp_input, "SMRP Input",
				    NORMAL_STACK, PRIO_HIGH);
    smrp_bg_pid = process_create(smrp_background, "SMRP Background",
				 NORMAL_STACK, PRIO_NORMAL);
    smrp_route_pid = process_create(smrp_route, "SMRP Routing",
				    NORMAL_STACK, PRIO_NORMAL);
    smrp_maint_pid = process_create(smrp_maint, "SMRP Maint",
				    NORMAL_STACK, PRIO_NORMAL);
}

/*
 * smrp_stop
 * stop SMRP routing.
 *
 * Because there are many dynamically allocated structures in SMRP,
 * we have to schedule the processes to run to deallocate memory.
 */
void
smrp_stop (void)
{
    smrp_running = FALSE;
    atalk_SMRP_running = FALSE;

    smrp_DeleteMatchingForwards(0, 0, FALSE, FALSE);
    smrp_CloseAllPorts();
    smrp_DeleteAllNodes();

    /* XXX Need to GC all memory */

    if (smrp_input_pid)
	process_kill(smrp_input_pid);
    if (smrp_maint_pid)
	process_kill(smrp_maint_pid);
    if (smrp_route_pid)
	process_kill(smrp_route_pid);
    if (smrp_bg_pid)
	process_kill(smrp_bg_pid);
}
