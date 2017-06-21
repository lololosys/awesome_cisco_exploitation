/* $Id: ncia.c,v 3.1.2.15 1996/08/28 12:39:39 thille Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia.c,v $
 *------------------------------------------------------------------
 * ncia.c - Native Client Interface Architecture Support
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia.c,v $
 * Revision 3.1.2.15  1996/08/28  12:39:39  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.14  1996/08/26  15:04:35  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.1.2.13  1996/07/26  06:23:55  schiang
 * CSCdi64000:  remove unused variable ncia_cisco_debug
 * Branch: California_branch
 *
 * Revision 3.1.2.12  1996/07/23  13:20:48  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.2.11  1996/07/16  09:38:45  jolee
 * CSCdi63075:  Error in configure ncia client, two client share same mac
 * addr
 * Branch: California_branch
 *
 * Revision 3.1.2.10  1996/07/11  21:13:43  jolee
 * CSCdi62553:  Move externs from .c to .h that was lost in CSCdi61974
 * Branch: California_branch
 *
 * Revision 3.1.2.9  1996/07/09  05:48:26  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1.2.8  1996/07/03  06:30:29  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/06/18  19:01:15  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.6  1996/06/12  06:51:41  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/06/04  06:00:19  schiang
 * CSCdi59337:  ncia server sends extra data in ICANREACH & DL_STARTED
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/05/29  08:57:28  hampton
 * Grrrrrrrrrrr.  Remove testing define so that the gs7-j-m image will
 * compile.  [CSCdi58914]
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/05/29  05:12:14  schiang
 * CSCdi58914:  spelling errors in ncia.c
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/05/28  22:33:46  schiang
 * CSCdi58852:  router crashes when ncia server is un-configured
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:21  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.6  1996/05/14  18:42:08  schiang
 * CSCdi57622:  ncia start/stop doesnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.5  1996/05/14  04:31:48  jolee
 * CSCdi57570:  Added clear client support, removed backup mac suppport
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.4  1996/05/03  22:06:17  schiang
 * CSCdi56629:  ncia server code needs to clean up
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.3  1996/04/26  15:16:40  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.2  1996/04/24  06:28:24  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.1.4.1  1996/04/05  06:05:39  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:13:25  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "subsys.h"
#include "sys_registry.h"
#include <ciscolib.h>
#include "../os/signal.h"
#include "interface_private.h"
#include "../srt/srt_registry.h"
#include "../cls/dlc_registry.h"
#include "packet.h"
#include "../os/buffers.h"
#include "mgd_timers.h"
#include "config.h"
#include "parser.h"
#include "../h/pak_api.h"
#include "../ip/ip_registry.h"
#include "ieee.h"
#include "address.h" /* Defines hwaddrtype for clsi_msg.h    */
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../tcp/tcpdriver.h"
#include "../if/if_vidb.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc.h"
#include "../cls/cls_assert.h"
#include "../h/ieee.h"
#include "../if/rif.h"
#include "../if/network.h"

#include "../cls/ncia.h"
#include "../cls/ncia_client.h"
#include "../cls/ncia_server_fsm.h"
#include "../cls/ncia_debug_flags.h"
#include "../cls/ncia_debug.h"



/*************************************************************************/
/* local function declarations                                           */
/*************************************************************************/

static void ncia_init(subsystype *subsys);
static void ncia_start(void) ;
static void ncia_peer_background_teardown(int signal, int dummy1, 
                                           void *dummy2, char *dummy3);
static process ncia_peer_background(void);
static void ncia_config_server(parseinfo *csb);
static void ncia_unconfig_server(parseinfo *csb);
static void ncia_config_client(parseinfo *csb);
static void ncia_unconfig_client(parseinfo *csb);
static void ncia_config_rsrb(parseinfo *csb);
static void ncia_unconfig_rsrb(parseinfo *csb);
static ncia_rsrb_t *ncia_get_rsrb_by_index(int rsrb_ring);
static ushort ncia_cls_get_rsrb_ring(ushort rsrb_ring);
static uchar ncia_cls_get_rsrb_bridge(ushort rsrb_ring);
static TCLSIMsg *ncia_get_act_ring_msg(idbtype *swidb);
static void ncia_set_enable_req(EnableReq_t* enableReq);
static void rsrb_set_enable_req(EnableReq_t* enableReq);
static paktype *ncia_get_ndlc_buffer(paktype *pak, ndlc_msg_type_t ndlc);
static void ncia_fill_ndlc(NciaCepT *ncia_ptr, paktype *pak, 
                           ndlc_msg_type_t ndlc);
static void ncia_do_unconfig_rsrb(ncia_rsrb_t *ncia_rsrb);
static void ncia_do_unconfig_client(ncia_servertype *ncia_server, 
                                    ipaddrtype ipaddr, uchar *mac);



#ifdef NCIA_DEBUG_LOOPBACAK
static nciapeertype *ncia_fake_msg(nciapeertype *ncia_client, paktype *pak, 
                                   ndlc_msg_type_t ndlc);
#endif



/*************************************************************************/
/* Global Variables                                                      */
/*************************************************************************/

queuetype  ncia_group;
static queuetype  ncia_rsrb_group;

mgd_timer  ncia_master_timer;

int        ncia_peer_pid;
boolean    ncia_running;

void      *ncia_next_item;	/* used by macro */
int        ncia_hash_index;	/* used by macro */


/*
 * Native Client Interface Architecture subsystem header
 */
#define NCIA_MAJVERSION 1
#define NCIA_MINVERSION 0
#define NCIA_EDITVERSION  1

SUBSYS_HEADER(ncia, NCIA_MAJVERSION, NCIA_MINVERSION, NCIA_EDITVERSION,
	      ncia_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: cls", "req: cls");

static
void ncia_init (subsystype *subsys)
{
    NciaServerDebugAll("\nNCIA: ncia_init");

    if (system_loading)
	return;

    ncia_peer_pid   = 0;
    ncia_running    = FALSE;

    queue_init(&ncia_group, 0);
    queue_init(&ncia_rsrb_group, 0);

    ncia_debug_init();

    reg_add_dlc_wput(CLS_NCIA, ncia_wput, "ncia_wput");
    reg_add_cls_get_rsrb_ring(ncia_cls_get_rsrb_ring, "ncia_cls_get_rsrb_ring");
    reg_add_cls_get_rsrb_bridge(ncia_cls_get_rsrb_bridge, 
                                "ncia_cls_get_rsrb_bridge");
    reg_add_dlsw_get_ring_req(CLS_NCIA, 
			      ncia_get_act_ring_msg, "ncia_get_act_ring_msg");

    reg_add_dlsw_set_enable_req(CLS_NCIA,
                              ncia_set_enable_req, "ncia_set_enable_req");
}

 


/*
 * This routine starts the NCIA protocol processes.  These processes are
 * only ever started once.  They will continue to run until the router is
 * rebooted.  The gate on starting these processes is ncia_running
 * forks the required processes for message processing.
 */
static
void ncia_start (void) 
{
    NciaServerDebugAll("\nNCIA: ncia_start");

    if (!ncia_peer_pid) {
	ncia_peer_pid = process_create((process_t *) &ncia_peer_background, 
                                       "NCIA background",
                                       NORMAL_STACK, PRIO_NORMAL);
	/* Using default stack size will give stack low warning */
	if (ncia_peer_pid == NO_PROCESS) {
	    NciaServerDebugAll(": fork fails");
	    ncia_peer_pid = 0;
	    return;
	}
    }

    ncia_running = TRUE;

    /*
     * Initialize all the timer chains
     */
    mgd_timer_init_parent(&ncia_master_timer, NULL);

    /* 
     * setup transport 
     */
    reg_add_ncia_running(ncia_client_active,"ncia_client_active");
    reg_add_tcp_listen(NCIA2_PORT, ncia_tcpd_listen, "ncia_tcpd_listen");
    reg_add_tcp_access_check(NCIA2_PORT, ncia_client_active,
                             "ncia_client_active");

    reg_invoke_ip_tcpdriver_start();
}





 
static 
void ncia_peer_background_teardown (int signal, int dummy1, void *dummy2, 
                                    char *dummy3)
{
    NciaServerDebugAll("\nNCIA: ncia_peer_background_teardown");

    process_watch_mgd_timer(&ncia_master_timer, DISABLE);
    ncia_peer_pid = 0;
}
 

 

/*
 * Process messages from NDLC main fsm.
 */
static
process ncia_peer_background (void)
{
    ulong        major, minor;
    mgd_timer    *expired_timer;
    NciaCepT     *circuit;
    nciapeertype *ncia_client;
 
    NciaServerDebugAll("\nNCIA: start ncia_peer_background");

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, ncia_peer_background_teardown);
    /* the following timer can't have parent timer */
    process_watch_mgd_timer(&ncia_master_timer, ENABLE);
 
    while (TRUE) {
        process_wait_for_event();

        while (process_get_wakeup(&major, &minor)) {
            switch (major) {
            case TIMER_EVENT:
                 while (mgd_timer_expired(&ncia_master_timer)) {
                     expired_timer =mgd_timer_first_expired(&ncia_master_timer);

                     switch (mgd_timer_type(expired_timer)) {
                     case NCIA_CIRCUIT_TIMER:
                          NciaCircuitDebugState("\nNCIA: get NCIA_CKT_TIMER");
                          /*
                           * Circuit timer
                           */
                          circuit = mgd_timer_context(expired_timer);
                          if (!circuit) 
                              continue;
                          mgd_timer_stop(&circuit->fTimer);
                          ncia_server_fsm(circuit, nciama_timer, 
                                          nciami_timer_expired);
                          if (circuit->fNciaCepFlags & NciaFreeCkt)
                              nciaClearCircuit(circuit->fNciaClient, circuit);
                          break;
 
                     case NCIA_CLIENT_TIMER:
                          NciaServerDebugAll("\nNCIA: get NCIA_CLIENT_TIMER");
                          /*
                           * peer timer
                           */
                          ncia_client = mgd_timer_context(expired_timer);
                          if (!ncia_client)
                              continue;
                          mgd_timer_stop(&ncia_client->timer);
                          ncia_client_fsm(ncia_client, TIME_OUT);
                          break;
                     }
                 }
                 break;

            default:
                 errmsg(&msgsym(UNEXPECTEDEVENT, SCHED), major, minor);
                 break;
            }
        }
    }
}



void ncia_command (parseinfo *csb)
{
    ncia_servertype *ncia_server;
    ncia_rsrb_t     *ncia_rsrb;
    nciapeertype    *ncia_client;

    if (csb->nvgen) {
        ForEachNciaServer(ncia_group, ncia_server) {
            if (ncia_server && ncia_server->ncia_swidb && 
                ncia_server->ncia_swidb->hwptr &&
                ncia_server->ncia_swidb->hwptr->state == IDBS_UP) {
                nv_write(TRUE, "ncia server %d %i %e %e %d", ncia_server->index,
                         ncia_server->ipaddr, ncia_server->server_mac,
                    ncia_server->vmac_start, ncia_server->vmac_range);
                if (ncia_server->flags & NCIA_INBOUND_ONLY)
                    nv_add(TRUE, " inbound-only");
                if (ncia_server->ndlc_keepalive_int != 
                    NCIA_DEFAULT_NDLC_KEEPALIVE * ONESEC)
                    nv_add(TRUE, " keepalive %d", 
                           ncia_server->ndlc_keepalive_int/ONESEC);

                if (ncia_server->tcp_keepalive_int !=     
                    NCIA_DEFAULT_TCP_KEEPALIVE * ONEMIN)  
                    nv_add(TRUE, " tcp_keepalive %d",    
                           ncia_server->tcp_keepalive_int/ONEMIN); 

                ForEachNciaClient(ncia_server, ncia_client) {
                    if (ncia_client->NciaFlags & NCIA_CLIENT_CONFIGED) {
                        nv_write(TRUE, "ncia client %d %i %e",
                                 ncia_server->index, ncia_client->ipaddr, 
                                 ncia_client->Vmac);
                        if (ncia_client->NciaFlags & NCIA_CLIENT_NETBIOS)
                            nv_add(TRUE, " all");
                    }
                }
            }
        }

        for (ncia_rsrb = ncia_rsrb_group.qhead; ncia_rsrb;
            ncia_rsrb = ncia_rsrb->fnext) {
                if (ncia_rsrb->dlsw_ring)
                    nv_write(TRUE, "ncia rsrb %d %d %d %d %d %e",
                        ncia_rsrb->rsrb_target_ring, ncia_rsrb->rsrb_bridge,
                        ncia_rsrb->rsrb_local_ring, ncia_rsrb->dlsw_bridge,
                        ncia_rsrb->dlsw_ring, ncia_rsrb->rsrb_mac);
            }
        return;
    }

    switch (csb->which) {
      case CONFIG_NCIA_SERVER:
        if (csb->sense)
            ncia_config_server(csb);
        else
            ncia_unconfig_server(csb);

	break;

      case CONFIG_NCIA_CLIENT:
        if (csb->sense)
            ncia_config_client(csb);
        else
            ncia_unconfig_client(csb);

	break;

      case NCIA_RSRB:
        if (csb->sense)
            ncia_config_rsrb(csb);
        else
            ncia_unconfig_rsrb(csb);

	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


static
ncia_rsrb_t *ncia_get_rsrb_by_index (int rsrb_ring)
{
    ncia_rsrb_t  *ncia_rsrb;

    NciaServerDebugAll("\nNCIA: ncia_get_rsrb_by_index (ring %d)", rsrb_ring);

    for (ncia_rsrb = ncia_rsrb_group.qhead; ncia_rsrb;
        ncia_rsrb = ncia_rsrb->fnext) {
        /* serach for the existing server control block */
        if (ncia_rsrb->rsrb_local_ring == rsrb_ring)
            return(ncia_rsrb);
    }
    NciaServerDebugAll(", nothing found");
    return(NULL);
}



ncia_servertype *ncia_get_server_by_index (int server_num)
{
    ncia_servertype  *ncia_server;

    ForEachNciaServer(ncia_group, ncia_server)
        /* serach for the existing server control block */
        if (ncia_server->index == server_num)
            return(ncia_server);
 
    NciaServerDebugAll(", nothing found");
    return(NULL);
}

/* 
 * ncia_config_server
 *
 * Parse the configuration options,
 */
static
void ncia_config_server (parseinfo *csb)
{
    idbtype          *swidb;
    hwidbtype        *idb;
    int              i, j, server_num;
    int              inbound_only;
    ncia_servertype  *ncia_server;
    srb_triplet_t    srb_triplet;

    NciaServerDebugAll("\nNCIA: ncia_config_server");

    server_num = GETOBJ(int,1);

    ncia_server = ncia_get_server_by_index(server_num);
    if (!ncia_server) {
        ncia_server = malloc(sizeof(ncia_servertype));
        if (!ncia_server){
            printf(nomemory);
            return;
        }
        memset(ncia_server, 0, sizeof(ncia_servertype));
        enqueue(&ncia_group, ncia_server);
    } else {
        if (ncia_server->flags & NCIA_SERVER_ACTIVE)
            ncia_unconfig_server(csb);
    }
 
    ncia_server->index = server_num;
    ncia_server->ipaddr = GETOBJ(paddr,1)->ip_addr;
    snpa_copy_to_buffer( GETOBJ(hwaddr,1), ncia_server->server_mac);
    snpa_copy_to_buffer( GETOBJ(hwaddr,2), ncia_server->vmac_start);
    ncia_server->vmac_range = GETOBJ(int,2);
    inbound_only = GETOBJ(int,3);
    ncia_server->ndlc_keepalive_int = GETOBJ(int,4) * ONESEC;
    ncia_server->tcp_keepalive_int  = GETOBJ(int,5) * ONEMIN;

    ncia_server->vmac_list = malloc(ncia_server->vmac_range * 
                                 sizeof(ushort));
    if (!ncia_server->vmac_list){
        printf(nomemory);
        free(ncia_server);
        return;
    }

    for (i = 0, j = 1; i < ncia_server->vmac_range; i++, j++)
         ncia_server->vmac_list[i] = j;
    ncia_server->ncia_free_client_index = 0;   

    for (i = 0; i < NCIA_HASH_SIZE; i++) {   /* init ncia client hash queue */
         queue_init(&ncia_server->hash_tbl[i], 0);
         ncia_server->hash_tbl_element[i] = 0;
    }                                         

    /* get a virtual idb */
    if (!ncia_server->ncia_swidb) {
        if ((swidb = vidb_malloc()) == NULL) {
            printf(nomemory);
            free(ncia_server->vmac_list);
            free(ncia_server);
            return;
        }

        ncia_server->ncia_swidb   = swidb;
    } else
        swidb = ncia_server->ncia_swidb;

    idb = swidb->hwptr;

    sprintf(ncia_server->name, "NCIA%d", server_num);

    idb->hw_namestring = idb->hw_short_namestring 
                       = swidb->namestring
                       = ncia_server->name;
    idb->state         = IDBS_UP;
    idb->status        = IDB_VIRTUAL;
    idb->maxmtu        = NCIA_MTU; /* TR_MTU */
    idb->enctype       = idb->enctype_def 
                       = idb->enctype_cfg 
                       = swidb->ip_enctype 
                       = ET_NDLC;

    idb->unit          = server_num;
    idb->slot          = 0;
    swidb->sub_number  = 0;

    ieee_copy(ncia_server->vmac_start, idb->hardware);

    /*
     * Do some things to make our virtual interface look
     * sufficiently real
     */
    idb->parse_packet = tring_parse_packet;
    idb->extract_hwaddr = ieee_extract_hwaddr;
    idb->vidb_link = (struct hwidbtype_ *) RSRB_INVALID_ADDRESS;

    srb_triplet.localRing  = NCIA_LOCAL_RING;
    srb_triplet.bridgeNum  = NCIA_BRIDGE;
    srb_triplet.remoteRing = NCIA_TARGET_RING;
    srbV.srb_sb_set_triplet(swidb, &srb_triplet);

    if (!ncia_running) 
        ncia_start();

    ncia_server->flags |= NCIA_SERVER_CONFIGURED;
    if (inbound_only)
        ncia_server->flags |= NCIA_INBOUND_ONLY;

    reg_invoke_srt_set_srb_params(idb, csb->sense);
}


static
void ncia_unconfig_server (parseinfo *csb)
{
    nciapeertype    *ncia_client;
    ncia_servertype *ncia_server;
    int             server_num;

    NciaServerDebugAll("\nNCIA: ncia_unconfig_server");

    if (!ncia_running)
        return;

    server_num = GETOBJ(int,1);

    ncia_server = ncia_get_server_by_index(server_num);

    if (!ncia_server) {
        printf("\nNCIA: server is not configured.");
        return;
    }

    if (!(ncia_server->flags & NCIA_SERVER_CONFIGURED)) {
        printf("\nNCIA: server is not configured.");
        return;
    }

    ForEachNciaClient(ncia_server, ncia_client) {
        NciaServerDebugAll("\nNCIA: ncia_do_unconfig_client %i", 
                           ncia_client->ipaddr);
        ncia_do_unconfig_client(ncia_server, ncia_client->ipaddr, 
                                 &ncia_client->Vmac[0]);
    }

    ncia_server->ncia_swidb->hwptr->state = IDBS_DOWN;

    reg_invoke_srt_set_srb_params(ncia_server->ncia_swidb->hwptr, csb->sense);

    ncia_server->flags &= 
        ~(NCIA_SERVER_CONFIGURED|NCIA_SERVER_STOP|NCIA_INBOUND_ONLY);
    ncia_server->num_netbios_stn = 0;
}



/* 
 * ncia_config_client
 * ncia client <server_num> <ipaddress> <vmac>
 *
 * Parse the configuration options,
 */
static
void ncia_config_client (parseinfo *csb)
{
    ncia_servertype *ncia_server;
    nciapeertype    *client_ip, *new_client;
    nciapeertype    *client_mac;
    int             server_num;
    boolean         new;
    int             support;

    NciaServerDebugAll("\nNCIA: ncia_config_client");

    server_num = GETOBJ(int,1);
    support    = GETOBJ(int,2);

    ncia_server = ncia_get_server_by_index(server_num);

    if (!ncia_server || !(ncia_server->flags & NCIA_SERVER_CONFIGURED)) {
        printf("\nNCIA: ncia server is not configured!");
        return;
    }
    
    new = FALSE; 
    new_client = (nciapeertype *) NULL;

    /* validate the duplicated client */
    client_ip = ncia_find_client_by_ipaddr(ncia_server, GETOBJ(paddr,1)->ip_addr);
    if (client_ip) {
        if (client_ip->client_state != NCIA_CLOSED) {
            printf("\nNCIA: client %i busy", GETOBJ(paddr,1)->ip_addr);
            return;
	}
        else {
            client_mac = ncia_find_client_by_mac(ncia_server->ncia_swidb,
                                                 GETOBJ(hwaddr,1)->addr);
            if (client_mac) {
                if (client_mac->ipaddr != client_ip->ipaddr) {
                    printf("\nNCIA: Error, mac addr %e being used by another client %i",
                              GETOBJ(hwaddr,1)->addr, client_mac->ipaddr);
                    return;
		}
                else
                    return;   /* redundant, same client with same mac addr */
	    }
            else {
                printf("\nNCIA: Reconfig client's %i mac addr from %e to %e",
                  GETOBJ(paddr,1)->ip_addr, client_ip->Vmac, GETOBJ(hwaddr,1)->addr);
                new_client = client_ip;
	    }
	}
    }
    else {
        client_mac = ncia_find_client_by_mac(ncia_server->ncia_swidb,
                                                 GETOBJ(hwaddr,1)->addr);
        if (client_mac) {
            if (client_mac->client_state != NCIA_CLOSED) {
                printf("\nNCIA: client %e busy", GETOBJ(hwaddr,1)->addr);
                return;
	    }
            else {
                printf("\nNCIA: Reconfig client's %e ip address from %i to %i",
                         GETOBJ(hwaddr,1)->addr, client_mac->ipaddr,
                         GETOBJ(paddr,1)->ip_addr);
                ncia_unchain_client_from_hash_tbl(client_mac);
                new_client = client_mac;     /* to be reconfigured */
                new = TRUE;                  /* to be add back to hash table */
            }
	}
        else {
            new = TRUE;
            new_client = malloc (sizeof(nciapeertype));
            if (!new_client) {
            printf(nomemory);
            return;
	    }
        }
    }

    if (!new) { /* client registered, free index if its mac in assigned range */
        if (ncia_client_mac_in_range(new_client)) 
            ncia_add_to_free_mac_index_chain(new_client);
    }

    memset(new_client, 0, sizeof(nciapeertype));
    new_client->ipaddr = GETOBJ(paddr,1)->ip_addr;
    snpa_copy_to_buffer(GETOBJ(hwaddr,1), new_client->Vmac);
    new_client->ncia_server = ncia_server;
    new_client->NciaFlags |= NCIA_CLIENT_CONFIGED;

    if (support == NCIA_SNA_NETBIOS) {
        new_client->NciaFlags |= NCIA_CLIENT_NETBIOS;
        ncia_server->num_netbios_stn++;
    }

#ifdef NCIA_TEST_ROUTER_TO_ROUTER
    {
    extern char host_addr[IEEEBYTES];
    snpa_copy_to_buffer(GETOBJ(hwaddr,1), host_addr);
    }
#endif

    if (ncia_client_mac_in_range(new_client)) {
        if (ncia_mac_in_range_but_taken(new_client)) {
            printf("\nNCIA: client %i, mac address %e in use", 
                   new_client->ipaddr, new_client->Vmac); 
            if (!new)
                ncia_unchain_client_from_hash_tbl(new_client);
            free (new_client);
            return;
                   /* the above should not occur, new mac addrwas checked above */
	}
        else {  
            new_client->VmacIndex = ncia_subtract_macaddr_for_vmacindex(
                                    new_client->Vmac, ncia_server->vmac_start,
                                    ncia_server->vmac_range);
            if (ncia_server->ncia_free_client_index >= ncia_server->vmac_range) {
                NciaClientDebugAll(
                    "\nNCIA: mac %e for client %i mac range used up", 
                    new_client->Vmac, new_client->ipaddr);
                if (!new)   
                    ncia_unchain_client_from_hash_tbl(new_client);
                free (new_client);
                return;
            }
            else 
                ncia_remove_from_free_mac_index_chain(new_client);    
	}
    }
    else
        new_client->VmacIndex = NCIA_OUT_MAC_RANGE;

    if (ncia_client_debug)
        NciaSetClientDebug(new_client, ncia_client_debug);

    queue_init(&new_client->OutputQ, 0);
    queue_init(&new_client->NciaCircuit, 0);

    new_client->client_state = NCIA_CLOSED;
    new_client->rd_state = RD_INIT;
    new_client->cur_cap_xchg_count = 0;

    ncia_init_tcp_value_in_client_cb(new_client);

    if (new)
        ncia_chain_client_to_hash_tbl(new_client);
}


static
void ncia_unconfig_client (parseinfo *csb)
{
    ncia_servertype *ncia_server;
    int             server_num;
    ipaddrtype      ipaddr;
    uchar           mac[IEEEBYTES];

    NciaServerDebugAll("\nNCIA: ncia_unconfig_client");

    server_num = GETOBJ(int,1);

    ncia_server = ncia_get_server_by_index(server_num);

    if (!ncia_server || !(ncia_server->flags & NCIA_SERVER_CONFIGURED)) {
        printf("\nNCIA: ncia server is not configured!");
        return;
    }

    ipaddr = GETOBJ(paddr,1)->ip_addr;
    snpa_copy_to_buffer(GETOBJ(hwaddr,1), mac);

    ncia_do_unconfig_client(ncia_server, ipaddr, &mac[0]);
}


static
void ncia_do_unconfig_client (ncia_servertype *ncia_server, ipaddrtype ipaddr, 
                              uchar *mac)
{
    boolean         found;
    nciapeertype    *ncia_client;
    NciaCepT        *ncia_circuit;

    found = FALSE;
    /* find the clinet control block */
    ForEachNciaClient(ncia_server, ncia_client)  
        if (ieee_equal(ncia_client->Vmac, &mac[0]) && ncia_client->ipaddr == 
            ipaddr){
            found = TRUE;
            goto END_OF_CLIENT_LOOP;
        }
    END_OF_CLIENT_LOOP:
    if (!found){
        NciaClientDebugAll(
            "\nncia_unconfig_client can't find the specified client %i %e",
            ipaddr, mac);
        return;
    }
    else {
        if (ncia_client->NciaFlags & NCIA_CLIENT_NETBIOS) {
            ncia_client->NciaFlags &= ~NCIA_CLIENT_NETBIOS;
            ncia_server->num_netbios_stn--;
        }
        ncia_client->NciaFlags &= ~NCIA_CLIENT_CONFIGED;
                           /* make sure client ctl block will be deleted */
        ncia_client->NciaFlags &= ~NCIA_CLIENT_ACTIVE;
        ncia_tcpd_close(ncia_client->t, ncia_client->out_pak);
                                          /* close tcp session if exists */
        ForEachNciaCircuit(ncia_client, ncia_circuit) {
            ncia_server_fsm(ncia_circuit, nciama_admin, nciami_admin_stop);
            if (ncia_circuit->fNciaCepFlags & NciaFreeCkt)
                nciaClearCircuitOnly(ncia_circuit->fNciaClient, ncia_circuit);
            }

        ncia_client_destroy(ncia_client); /* free client ctl blk if no circuit*/
    }
}


static
ushort ncia_cls_get_rsrb_ring (ushort rsrb_ring)
{
    ncia_rsrb_t *ncia_rsrb;

    NciaServerDebugAll("\nNCIA: ncia_cls_get_rsrb_ring (rsrb ring %d)", 
                       rsrb_ring);

    ncia_rsrb = ncia_get_rsrb_by_index(rsrb_ring);
    if (!ncia_rsrb) {
        NciaServerDebugAll(
            "\nNCIA RSRB with rsrb target ring %d is not configured.", 
            rsrb_ring);
        return(0);
    }
    return(ncia_rsrb->dlsw_ring);
}



static
uchar ncia_cls_get_rsrb_bridge (ushort rsrb_ring)
{
    ncia_rsrb_t *ncia_rsrb;

    NciaServerDebugAll("\nNCIA: ncia_cls_get_rsrb_bridge (rsrb ring %d)", 
                       rsrb_ring);

    ncia_rsrb = ncia_get_rsrb_by_index(rsrb_ring);
    if (!ncia_rsrb) {
        NciaServerDebugAll(
            "\nNCIA RSRB with rsrb target ring %d is not configured.", 
            rsrb_ring);
        return(0);
    }
    return (ncia_rsrb->dlsw_bridge);
}



/* ncia_config_rsrb
 *
 * ncia rsrb <rsrb_ring> <rsrb_bridge> <local_ring> <dlsw_bridge> 
 *           <dlsw_ring> <virtual_macaddr>
 *
 * Parse the configuration options,
 * build a CLSI message, and put it on the Act Ring Pending
 */
static
void ncia_config_rsrb (parseinfo *csb)
{
    hwidbtype       *idb;
    idbtype         *swidb;
    uchar           macAddr[IEEEBYTES];
    uchar           zeroMacAddr[IEEEBYTES];  /* Zero MAC address is invalid  */
    hword           local_ring;              /* Local ring number            */
    byte            dlsw_bridge;             /* NCIA Bridge number           */
    hword           dlsw_ring;               /* NCIA ring number             */
    hword           rsrb_ring;
    byte            rsrb_bridge;
    ncia_rsrb_t     *ncia_rsrb;
    boolean         server_configured = FALSE;
    ncia_servertype *ncia_server;
    srb_triplet_t   srb_triplet;

    NciaServerDebugAll("\nNCIA: ncia_config_rsrb");

    /* at least one server is configured */
    ForEachNciaServer(ncia_group, ncia_server) {
        if (ncia_server && ncia_server->ncia_swidb && 
            ncia_server->ncia_swidb->hwptr &&
            ncia_server->ncia_swidb->hwptr->state == IDBS_UP) {
            server_configured = TRUE;
            break;
        }
    }
    if (!server_configured)
        return;

    local_ring = GETOBJ(int,3);
    ncia_rsrb = ncia_get_rsrb_by_index(local_ring);
    if (!ncia_rsrb){
        ncia_rsrb = malloc(sizeof(ncia_rsrb_t));
        if (!ncia_rsrb){
            printf(nomemory);
            return;
        }
        memset(ncia_rsrb, 0, sizeof(ncia_rsrb));
        enqueue(&ncia_rsrb_group, ncia_rsrb);
    } else {
        if (ncia_rsrb->flag & NCIA_RSRB_UP)
            ncia_do_unconfig_rsrb(ncia_rsrb);
    }

    memset(&zeroMacAddr, 0, IEEEBYTES);

    ieee_copy(GETOBJ(hwaddr,1)->addr, macAddr);
    rsrb_ring   = GETOBJ(int,1);
    rsrb_bridge = GETOBJ(int,2);
    local_ring  = GETOBJ(int,3);
    dlsw_bridge = GETOBJ(int,4);
    dlsw_ring   = GETOBJ(int,5);
 
    if (dlsw_ring == local_ring)
        printf("\nlocal ring equals to ncia ring (%d)", dlsw_ring);

    else if (local_ring == rsrb_ring)
        printf("\nlocal ring equals to rsrb ring (%d)", rsrb_ring);

    else if (vrfind_ringgroup(local_ring) != 0)  /* local_ring avail?  */
        printf("\nlocal ring %d has been configured as a ring group", 
               local_ring);
 
    else if (vrfind_ringgroup(rsrb_ring) == 0) /* rsrb ring active?   */
        printf("\nring group %d hasn't been configured yet", rsrb_ring);
 
    else if (vrfind_ringgroup(dlsw_ring) == 0) /* ncia ring active?   */
        printf("\nring group %d hasn't been configured yet", dlsw_ring);
 
    else if (bcmp(macAddr, (char *) &zeroMacAddr, IEEEBYTES) == 0)
        printf("\ninvalid virtual mac address %e", macAddr);
 
    else {
        /*
         * Since I don't have a vidb for RSRB, I have to create one
         * now to pass the parameters to NCIA.  It will be freed in
         * CreateEnableReq() before the ENABLE.REQ gets sent to CLS.
         */
        if ((swidb = vidb_malloc()) == NULL) {
            printf(nomemory);
            free(ncia_rsrb);
            return;
        }
        idb = swidb->hwptr;
        idb->status                  = IDB_VIRTUAL | IDB_TR;
#if 0
        idb->srb_thisring            = local_ring;
        idb->srb_targetring          = rsrb_ring;
        idb->srb_bridge_num          = rsrb_bridge;
#endif

        srb_triplet.localRing  = local_ring;
        srb_triplet.bridgeNum  = rsrb_bridge;
        srb_triplet.remoteRing = rsrb_ring;
        srbV.srb_sb_set_triplet(swidb, &srb_triplet);

        ieee_copy(macAddr, idb->hardware);

        idb->hw_namestring           = malloc(16);
        if (!idb->hw_namestring) {
            printf(nomemory);
            free(ncia_rsrb);
            vidb_free(swidb);
            return;
        }
        sprintf(idb->hw_namestring, "NCIA_RSRB");

        ncia_rsrb->flag             |= NCIA_RSRB_UP;
        ncia_rsrb->rsrb_target_ring  = rsrb_ring;
        ncia_rsrb->rsrb_bridge       = rsrb_bridge;
        ncia_rsrb->rsrb_local_ring   = local_ring;
        ncia_rsrb->dlsw_bridge       = dlsw_bridge;
        ncia_rsrb->dlsw_ring         = dlsw_ring;
        ieee_copy(macAddr, ncia_rsrb->rsrb_mac);

        reg_add_dlsw_set_enable_req(CLS_RSRB,
                              rsrb_set_enable_req, "rsrb_set_enable_req");

        reg_invoke_srt_set_srb_params(idb, csb->sense);

        /*
         * Free the vidb now.
         */
        vidb_free(swidb);
    }
}



/*
 * unconfigure RSRB from DLSw+
 */
static
void ncia_unconfig_rsrb (parseinfo *csb)
{
    ncia_rsrb_t   *ncia_rsrb;
    hword         local_ring;              /* Local ring number            */
    byte          dlsw_bridge;             /* NCIA Bridge number           */
    hword         dlsw_ring;               /* NCIA ring number             */
    hword         rsrb_ring;
    byte          rsrb_bridge;
    uchar         macAddr[IEEEBYTES];

    ieee_copy(GETOBJ(hwaddr,1)->addr, macAddr);
    rsrb_ring   = GETOBJ(int,1);
    rsrb_bridge = GETOBJ(int,2);
    local_ring  = GETOBJ(int,3);
    dlsw_bridge = GETOBJ(int,4);
    dlsw_ring   = GETOBJ(int,5);

    NciaServerDebugAll("\nNCIA: ncia_unconfig_rsrb local ring %d", local_ring);

    ncia_rsrb = ncia_get_rsrb_by_index(local_ring);

    if (!ncia_rsrb) {
        printf("\nNCIA: can't find the given local ring num: %d", local_ring);
        return;
    }

    if (ncia_rsrb->rsrb_target_ring != rsrb_ring) {
        printf("\nNCIA: unrecognized RSRB ring num: %d", rsrb_ring);
    } else
    if (ncia_rsrb->rsrb_bridge != rsrb_bridge){
        printf("\nNCIA: unrecognized RSRB bridge num: %d", rsrb_bridge);
    } else
    if (ncia_rsrb->dlsw_bridge != dlsw_bridge){
        printf("\nNCIA: unrecognized NCIA bridge num: %d", dlsw_bridge);
    } else
    if (ncia_rsrb->dlsw_ring != dlsw_ring){
        printf("\nNCIA: unrecognized NCIA ring num: %d", dlsw_ring);
    } else
    if (!ieee_equal(macAddr, ncia_rsrb->rsrb_mac)){
        printf("\nNCIA: unrecognized RSRB Mac Addr: %e", macAddr);
    } else
        ncia_do_unconfig_rsrb(ncia_rsrb);
}



static
void ncia_do_unconfig_rsrb (ncia_rsrb_t *ncia_rsrb)
{
    tr_vgrouptype *vrg;
    tr_ventrytype *vre;
    hwidbtype     *idb = NULL;

    ncia_rsrb->flag &= ~NCIA_RSRB_UP;

    vrg = vrfind_ringgroup(ncia_rsrb->rsrb_target_ring);
    if (!vrg) {
        printf("\nNCIA: can't find vrg for ring %d", 
               ncia_rsrb->rsrb_target_ring);
        return;
    }

    for (vre = (tr_ventrytype *) vrg->rings.qhead; vre; vre = vre->next) {
        if (vre->rn == ncia_rsrb->rsrb_local_ring && 
            ieee_equal(ncia_rsrb->rsrb_mac, vre->macaddr))
            idb = vre->virtual_idb;
    }

    if (!idb) {
        printf("\nNCIA: can't find idb for ring %d", 
               ncia_rsrb->rsrb_target_ring);
        return;
    }

    reg_invoke_srt_set_srb_params(idb, FALSE);

    unqueue(&ncia_rsrb_group, ncia_rsrb);

    free(ncia_rsrb);
}



nciapeertype *ncia_find_client_by_mac (idbtype *swidb, uchar *addr)
{
    nciapeertype *ncia_client;
    ncia_servertype *ncia_server;

    ncia_server = ncia_idb_to_server(swidb);
 
    /* find the clinet control block */
    ForEachNciaClient(ncia_server, ncia_client)   
        if (ieee_equal(ncia_client->Vmac, addr))
            return (ncia_client);

    return(NULL);
}


NciaCepT *ncia_find_circuit (nciapeertype *ncia_client, uchar *HostAddr, 
                             uchar HostSap, uchar ClientSap)
{
    NciaCepT *NciaCep;

    /* serach for the existing circuit control block */
    ForEachNciaCircuit(ncia_client, NciaCep)
        if (ieee_equal(NciaCep->fTargetAddr, HostAddr) &&
           (HostSap == NCIA_DONT_CARE || NciaCep->fTargetSap == HostSap) &&
           /* Client is zero when CAN_U_REACH is received */
           (NciaCep->fClientSap == ClientSap)) {
                return(NciaCep);
        }

    return(NULL);
}



/*
 * all new circuit control blocks are inserted in the circuit queue in client 
 * control block.
 */
NciaCepT *ncia_circuit_create (nciapeertype *ncia_client, uchar *SrcAddr, 
                               uchar SrcSap, uchar DestSap, NciaSapT *NciaSap)
{
    NciaCepT *NciaCep;

    NciaCircuitDebugEvent(
        "\nNCIA: create circuit: saddr %e, ssap %d, daddr %e, dsap %d", 
        SrcAddr, SrcSap, ncia_client->Vmac, DestSap);

    NciaCep = malloc(sizeof (NciaCepT));
    if (!NciaCep) {
        printf(nomemory);
        return(NULL);
    }
    
    memset(NciaCep, 0, sizeof(NciaCepT));
    ieee_copy(SrcAddr, NciaCep->fTargetAddr);
    NciaCep->fTargetSap = SrcSap;
    NciaCep->fClientSap = DestSap;
    NciaCep->fNciaClient = (void *) ncia_client;
    NciaCep->fState = nciast_closed;
    NciaCep->fPSap = NciaSap;
    NciaCep->fPCepId = (word) NciaCep;
    NciaCep->fLFSize = IEEE_LF1470;

    mgd_timer_init_leaf(&NciaCep->fTimer, &ncia_master_timer,
                        NCIA_CIRCUIT_TIMER, NciaCep, FALSE);
    queue_init(&NciaCep->fInputQ, 0);

    enqueue(&ncia_client->NciaCircuit, NciaCep);

    NciaCep->fClientInitWindowSize = NCIA_INIT_WINDOW_SIZE;

    NciaCircuitDebugEvent(" sid: %x", NciaCep);
    return(NciaCep);
}



boolean ncia_check_client_sap (nciapeertype *NciaClient, uchar Sap)
{
    int index;

    /* all SAPs supported */
    if (NciaClient->cap_xchg_ctl_vec.sap_list[0] == 0)
        return(TRUE);

    if (Sap) {
        for (index = 0; index < NCIA_MAX_SAPS; index++)
            if (NciaClient->cap_xchg_ctl_vec.sap_list[index] == Sap)
                return(TRUE);
        return(FALSE);
    }
    /*
     * always return true for sap 0
     */
    return(TRUE);
}




/* 
 * ncia_cep_find_opened
 * Check if the given uCepId is duplicated.
 */
boolean ncia_cep_find_opened (NciaSapT *NciaSap, ClsUCepIdT uCepId)
{
    NciaCepT *cep;

    for (cep = (NciaCepT *)NciaSap->fNciaCep; cep; cep = cep->fNextCep)
        if (cep->fUCepId == (word) uCepId)
            return (TRUE);

    return (FALSE);
}



NciaCepT *ncia_cep_search_by_ucepid (ClsUCepIdT uCepId)
{
    ncia_servertype *ncia_server;
    NciaCepT        *NciaCep;
    nciapeertype    *ncia_client;
 
    ForEachNciaServer(ncia_group, ncia_server) {
        ForEachNciaClient(ncia_server, ncia_client) {   
            /* serach for the existing circuit control block */
            ForEachNciaCircuit(ncia_client, NciaCep)
                if (NciaCep->fUCepId == (word) uCepId)
                    return(NciaCep);
        }
    }

    return(NULL);
}



ncia_servertype *ncia_idb_to_server (idbtype *swidb)
{
    ncia_servertype *ncia_server;

    ForEachNciaServer(ncia_group, ncia_server) {
        /* serach for the existing server control block */
        if (ncia_server->ncia_swidb == swidb)
            return(ncia_server);
    }
    return(NULL);
}



/* 
 * ncia_get_act_ring_msg
 */
static
TCLSIMsg *ncia_get_act_ring_msg (idbtype *swidb)
{
    TCLSIMsg		*theMsg;
    ActRngNCIAParms_t	*NciaActRng;

    /* build CLSI Message */
    theMsg =  CLSIMsgCreateHeader(CLS_ACTIVATE_RING | CLS_REQUEST,
  					   offsetof(ActRngReq_t,fParms) -
					   CLS_ACT_RING_REQ_HDR_SZ +
					   sizeof(ActRngNCIAParms_t),
					   CLS_LLC_LAYER,
					   CLS_ENTITY_MGR,
					   0);
    if (theMsg == NULL) {
	printf("\nNCIA: allocation for ActivateRing.Req fail");
	return(NULL);
    }

    NciaActRng = (ActRngNCIAParms_t *)(((ActRngReq_t*)theMsg)->fParms);

    return(theMsg);
}



static
void ncia_set_enable_req (EnableReq_t* enableReq)
{
    idbtype           *idb;
    EnableNCIAParms_t *nciaParms;
    ncia_servertype   *ncia_server;

    idb = enableReq->fPortId.fTypeInfo.swidb;
    ncia_server = ncia_idb_to_server(idb);

    nciaParms = (EnableNCIAParms_t*) &(enableReq->fParms);
    nciaParms->fVersion = CLSI_VERSION;
    nciaParms->fIpAddr = ncia_server->ipaddr;
    ieee_copy(ncia_server->vmac_start, nciaParms->fVMacAddr);
    ieee_copy(ncia_server->server_mac, nciaParms->fServerMac);
    nciaParms->fVMacRange = ncia_server->vmac_range;
    nciaParms->fServerNum = ncia_server->index;
}



static
void rsrb_set_enable_req (EnableReq_t* enableReq)
{
    idbtype           *idb;
    hwidbtype         *hwptr;
    EnableRSRBParms_t *rsrbParms;
    srb_triplet_t     srb_triplet;
 
    idb = enableReq->fPortId.fTypeInfo.swidb;
    hwptr = hwidb_or_null(idb);
    rsrbParms = (EnableRSRBParms_t*) &(enableReq->fParms);

    rsrbParms->fVersion = 1;
    snpa_init(&rsrbParms->fVMacAddr, STATION_IEEE48, TARGET_UNICAST,
      (uchar *) &(hwptr->hardware));
    rsrbParms->fMaxIFieldLength = 0;

    srbV.srb_sb_get_triplet(idb, &srb_triplet);
    rsrbParms->fLocalRing  = srb_triplet.localRing;
    rsrbParms->fTargetRing = srb_triplet.remoteRing;

    rsrbParms->fRsrbOptionFlags = 0;
 
    enableReq->fPortId.fTypeInfo.swidb = NULL;
    free(hwptr->hw_namestring);
    hwptr->status = 0;
}




/*
 * it's very important that the datagramsize is set to the beginning of
 * the data.
 */
static
paktype *ncia_get_ndlc_buffer (paktype *pak, ndlc_msg_type_t ndlc)
{
    int     size = ncia_get_ndlc_size(ndlc);

    if (size == 0) {
        if (pak)
            datagram_done(pak); 
        return(NULL);
    }

    /*
     * info, data, and dgrm must come with pak.
     */
    if ((ndlc == NDLC_INFO_FRAME || ndlc == NDLC_DGRM_FRAME || 
         ndlc == NDLC_DATA_FRAME)
         && !pak)
        return(NULL);
 
    if (!pak) {
        pak = getbuffer(size);
        if (!pak) {
            return(NULL);
        }
        pak->datagramsize = size;
    } else {
        /* simply adjust it */
        pak_delta_adjust_start(pak, -1 * size);

        if (size != pak->datagramsize &&
            ndlc != NDLC_DGRM_FRAME && ndlc != NDLC_DATA_FRAME &&
            ndlc != NDLC_XID_FRAME && ndlc != NDLC_INFO_FRAME) {
            NciaCircuitDebugError("\nNCIA: incorrect pkt size %d -> %d for %s,
                                  correct size",
                     size, pak->datagramsize, ncia_print_ndlc(ndlc));
            pak->datagramsize = size;
        }
    }

    return(pak);
}



static
void ncia_fill_ndlc (NciaCepT *ncia_ptr, paktype *pak, ndlc_msg_type_t ndlc)
{
    ndlc_packet_t *ndlc_ptr;

    ndlc_ptr = (ndlc_packet_t *) pak->datagramstart;

    ndlc_ptr->ndlc_header.proto_ver_id = NCIA_PROTOCOL_ID | NCIA_VERSION_NO;
    ndlc_ptr->ndlc_header.message_type = ndlc;
    ndlc_ptr->ndlc_header.packet_length = ncia_get_ndlc_size(ndlc);

    if (ndlc_ptr->ndlc_header.packet_length != pak->datagramsize &&
        ndlc != NDLC_XID_FRAME && ndlc != NDLC_INFO_FRAME) {
        NciaCircuitDebugError(
             "\nNCIA: (ncia_fill_ndlc) incorrect packet size, %d -> %d for %s",
                     ndlc_ptr->ndlc_header.packet_length, pak->datagramsize,
                     ncia_print_ndlc(ndlc));
    }

    switch(ndlc) {
    case NDLC_I_CAN_REACH:
    case NDLC_I_CANNOT_REACH:
         ieee_copy(ncia_ptr->fTargetAddr,
                   ndlc_ptr->ndlc_data.i_can_reach.target_mac);
         ndlc_ptr->ndlc_data.i_can_reach.client_sap = ncia_ptr->fClientSap;
         break;

    case NDLC_START_DL:
    case NDLC_DL_STARTED:
    case NDLC_START_DL_FAILED:
         ieee_copy(ncia_ptr->fTargetAddr,
                   ndlc_ptr->ndlc_data.start_dl.target_mac);
         ndlc_ptr->ndlc_data.start_dl.target_sap = ncia_ptr->fTargetSap;
         ndlc_ptr->ndlc_data.start_dl.client_sap = ncia_ptr->fClientSap;
         ndlc_ptr->ndlc_data.start_dl.window_size = 
                                               ncia_ptr->fClientInitWindowSize;

         ncia_ptr->fClientCurrentWindow = ncia_ptr->fClientInitWindowSize;

         ndlc_ptr->ndlc_data.start_dl.largest_frame_size = ncia_ptr->fLFSize;
         ndlc_ptr->ndlc_data.start_dl.window_size = NCIA_INIT_WINDOW_SIZE;
         if (ndlc == NDLC_START_DL) {
             ndlc_ptr->ndlc_data.start_dl.origin_ckt_id = ncia_ptr->fPCepId;
             ndlc_ptr->ndlc_data.start_dl.target_ckt_id = 0;
         } else {
             ndlc_ptr->ndlc_data.start_dl.origin_ckt_id =ncia_ptr->fClientCktId;
             ndlc_ptr->ndlc_data.start_dl.target_ckt_id = ncia_ptr->fPCepId;
         }
         break;

    case NDLC_HALT_DL:
    case NDLC_HALT_DL_NOACK:
         ndlc_ptr->ndlc_data.halt_dl.sender_ckt_id = ncia_ptr->fPCepId;
         ndlc_ptr->ndlc_data.halt_dl.receiver_ckt_id = ncia_ptr->fClientCktId;
         break;

    case NDLC_DL_HALTED:
         ndlc_ptr->ndlc_data.halt_dl.sender_ckt_id = ncia_ptr->fClientCktId;
         ndlc_ptr->ndlc_data.halt_dl.receiver_ckt_id = ncia_ptr->fPCepId;
         break;

    case NDLC_CONTACT_STN:
    case NDLC_STN_CONTACTED:
    case NDLC_FCM_FRAME:
         ndlc_ptr->ndlc_data.xid_frame.session_id = ncia_ptr->fClientCktId;
         break;

    case NDLC_XID_FRAME:
    case NDLC_INFO_FRAME:
    case NDLC_DGRM_FRAME:
         ndlc_ptr->ndlc_header.packet_length = pak->datagramsize;
         ndlc_ptr->ndlc_data.xid_frame.session_id = ncia_ptr->fClientCktId;
         break;

    case NDLC_DATA_FRAME:
         ndlc_ptr->ndlc_header.packet_length = pak->datagramsize;
         ieee_copy(ncia_ptr->fTargetAddr,
                   ndlc_ptr->ndlc_data.data_frame.target_mac);
         ndlc_ptr->ndlc_data.data_frame.target_sap = ncia_ptr->fTargetSap;
         ndlc_ptr->ndlc_data.data_frame.client_sap = ncia_ptr->fClientSap;
         ndlc_ptr->ndlc_data.data_frame.broadcast_type = 0;
         break;

    default:
         return;
    }
}




#ifdef NCIA_DEBUG_LOOPBACK
/*
4000.3000.0003 client --- fake1 1000.0000.0001 circuit1
4000.1060.1000 host  ---- fake2 1000.0000.0002 circuit2

1000.5ACC.80CB client --- fake1 1000.0000.0001 circuit11
1000.5ACC.A599 host  ---- fake2 1000.0000.0002 circuit22
*/

/* we fake the client here */
static
nciapeertype *ncia_fake_msg(nciapeertype *ncia_client, paktype *pak, ndlc_msg_type_t ndlc)
{
    ndlc_packet_t   *ndlc_ptr;
    char host_addr1[IEEEBYTES]   = {0x10, 0x00, 0x5a, 0xcc, 0xa5, 0x99};
    char client_addr1[IEEEBYTES] = {0x10, 0x00, 0x5a, 0xcc, 0x80, 0xcb};
    char host_addr[IEEEBYTES]    = {0x40, 0x00, 0x10, 0x60, 0x10, 0x00};
    char client_addr[IEEEBYTES]  = {0x40, 0x00, 0x30, 0x00, 0x00, 0x03};
    char fake1[IEEEBYTES]        = {0x10, 0x00, 0x00, 0x00, 0x00, 0x01};
    char fake2[IEEEBYTES]        = {0x10, 0x00, 0x00, 0x00, 0x00, 0x02};
    ndlc_can_u_reach_t      *can_u_reach;
    ndlc_start_dl_t         *start_dl;
    ndlc_xid_t              *xid_frame;
    static NciaCepT *circuit1;
    static NciaCepT *circuit11;
    static NciaCepT *circuit2;
    static NciaCepT *circuit22;

    ndlc_ptr = (ndlc_packet_t *) pak->datagramstart;

    switch(ndlc) {
    case NDLC_CAN_U_REACH:
         can_u_reach = (ndlc_can_u_reach_t *) &ndlc_ptr->ndlc_data.can_u_reach;
         if (ieee_equal(&can_u_reach->target_mac[0], &client_addr[0])) {
             ieee_copy(&host_addr[0], &can_u_reach->target_mac[0]);
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake2[0]));
         } else if (ieee_equal(&can_u_reach->target_mac[0], &client_addr1[0])) {
             ieee_copy(&host_addr1[0], &can_u_reach->target_mac[0]);
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake2[0]));
         } else {
             buginf("\nNCIA: unknown MAC (%e) in %s",
                            can_u_reach->target_mac, ncia_print_ndlc(ndlc));
             datagram_done(pak);
             return(NULL);
         }
         break;

    case NDLC_START_DL:
         start_dl = (ndlc_start_dl_t *) &ndlc_ptr->ndlc_data.start_dl;
         if (ieee_equal(&start_dl->target_mac[0], &client_addr[0])) {
             ieee_copy(&host_addr[0], &start_dl->target_mac[0]);
             circuit1 = (NciaCepT *) start_dl->origin_ckt_id;
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake2[0]));
         } else if (ieee_equal(&start_dl->target_mac[0], &client_addr1[0])) {
             ieee_copy(&host_addr1[0], &start_dl->target_mac[0]);
             circuit11 = (NciaCepT *) start_dl->origin_ckt_id;
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake2[0]));
         } else {
             buginf("\nNCIA: unknown MAC (%e) in %s",
                            start_dl->target_mac, ncia_print_ndlc(ndlc));
             datagram_done(pak);
             return(NULL);
         }
         break;

    case NDLC_I_CAN_REACH:
    case NDLC_I_CANNOT_REACH:
         can_u_reach = (ndlc_can_u_reach_t *) &ndlc_ptr->ndlc_data.can_u_reach;
         if (ieee_equal(&can_u_reach->target_mac[0], &host_addr[0])) {
             ieee_copy(&client_addr[0], &can_u_reach->target_mac[0]);
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake1[0]));
         } else if (ieee_equal(&can_u_reach->target_mac[0], &host_addr1[0])) {
             ieee_copy(&client_addr1[0], &can_u_reach->target_mac[0]);
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake1[0]));
         } else {
             buginf("\nNCIA: unknown MAC (%e) in %s",
                            can_u_reach->target_mac, ncia_print_ndlc(ndlc));
             datagram_done(pak);
             return(NULL);
         }
         break;

    case NDLC_DL_STARTED:
    case NDLC_START_DL_FAILED:
         start_dl = (ndlc_start_dl_t *) &ndlc_ptr->ndlc_data.start_dl;
         if (ieee_equal(&start_dl->target_mac[0], &host_addr[0])) {
             ieee_copy(&client_addr[0], &start_dl->target_mac[0]);
             circuit2 = (NciaCepT *) start_dl->origin_ckt_id;
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake1[0]));
         } else if (ieee_equal(&start_dl->target_mac[0], &host_addr1[0])) {
             ieee_copy(&client_addr1[0], &start_dl->target_mac[0]);
             circuit22 = (NciaCepT *) start_dl->origin_ckt_id;
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake1[0]));
         } else {
             buginf("\nNCIA: unknown MAC (%e) in %s",
                            start_dl->target_mac, ncia_print_ndlc(ndlc));
             datagram_done(pak);
             return(NULL);
         }
         break;

    case NDLC_DATA_FRAME:
         buginf("\nNCIA: do NOT support primitive DATA_FRAME in debug mode");
         datagram_done(pak);
         return(NULL);

    case NDLC_HALT_DL:
    case NDLC_HALT_DL_NOACK:
    case NDLC_DL_HALTED:
         /* not implemented yet! */
         break;

    case NDLC_XID_FRAME:
    case NDLC_CONTACT_STN:
    case NDLC_STN_CONTACTED:
    case NDLC_INFO_FRAME:
    case NDLC_FCM_FRAME:
    case NDLC_DGRM_FRAME:
         xid_frame = (ndlc_xid_t *) &ndlc_ptr->ndlc_data.xid_frame;
         if (xid_frame->session_id == (ulong) circuit1
            || xid_frame->session_id == (ulong) circuit11)
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake1[0]));
         else
             return(ncia_find_client_by_mac(
                        ncia_client->ncia_server->ncia_swidb, &fake2[0]));
         break;

    default:
         buginf("\nNCIA: Invalid primitive %s", ncia_print_ndlc(ndlc));
         datagram_done(pak);
         return(NULL);
    }
}
#endif


/*
 * send a packet to client.  If the TCP session is not up, send ACTIVE_OPEN
 * to clinet fsm to start TCP session. If TCP session is up then call client_fsm
 * to send the packet out. 
 * INFO and DGRM frames are queued in circuit output Q because they are subject
 * to flow control.  All other packets which are not subject to flow control are
 * queued in client output Q.
 */
void ncia_send_ndlc (NciaCepT *ncia_ptr, ndlc_msg_type_t ndlc)
{
    paktype      *pak;
    nciapeertype *ncia_client;

    NciaCircuitDebugEvent("\nNCIA: send %s to client %i for ckt: %x", 
              ncia_print_ndlc(ndlc), ncia_ptr->fNciaClient->ipaddr, ncia_ptr);

    if (!ncia_ptr) {
        NciaCircuitDebugError(
            "\nNCIA: ncia_send_ndlc: invalid circuit control block");
        return;
    }

    pak = ncia_ptr->fPacket;
    ncia_ptr->fPacket = NULL;
    ncia_client = ncia_ptr->fNciaClient;
    if (!ncia_client) {
        NciaCircuitDebugError(
            "\nNCIA: ncia_send_ndlc: invalid client control block");
        if (pak)
            datagram_done(pak);
        return;
    }

    pak = ncia_get_ndlc_buffer(pak, ndlc);
    if (!pak) {
        NciaCircuitDebugError(
            "\nNCIA: send_ndlc: fail to get buffer for ndlc primitive %s", 
            ncia_print_ndlc(ndlc));
        return;
    }
    ncia_fill_ndlc(ncia_ptr, pak, ndlc);

    switch(ncia_client->client_state) { 
    case NCIA_OPENED:
         if (ncia_client->out_pak) {
             if (ncia_client->NciaFlags & NciaClientDebugERROR)
                 buginf("\nNCIA:no outpak found");
             datagram_done(ncia_client->out_pak);
         }
         if (ncia_ptr->fNciaCepFlags & NciaClientBusy &&
             /* these packets are subject flow control */
             (ndlc == NDLC_INFO_FRAME || ndlc == NDLC_DGRM_FRAME)) {
             enqueue(&ncia_ptr->fOutputQ, pak);
         } else if (ncia_ptr->fNciaCepFlags & NciaTcpBusy)
             enqueue(&ncia_client->OutputQ, pak);
         else 
         if (ncia_flow_control_out(ncia_ptr, 
                 (ndlc_packet_t *) pak->datagramstart, ndlc))
             /* under flow control */
             enqueue(&ncia_ptr->fOutputQ, pak);
         else
         {
             if (ncia_circuit_debug & NciaCircuitDebugEVENT)
                 ncia_print_ndlc_header(ndlc, 
                     (ndlc_packet_t *) pak->datagramstart, "OUT");
#ifdef NCIA_DEBUG_LOOPBACAK
             if ((ncia_client = ncia_fake_msg(ncia_client, pak, ndlc))) {
                 ncia_client->out_pak = NULL;
                 ncia_client->in_pak = pak;
                 ncia_pre_server_fsm(ncia_client, NDLC_PRIMITIVES);
             }
#else
             ncia_client->out_pak = pak;
             ncia_pre_client_fsm (ncia_client, NDLC_FSM_INPUT);
#endif
         }
         break;

    case NCIA_CLOSED:
         ncia_pre_client_fsm(ncia_client, ACTIVE_OPEN);
         /* fall through */

    default:
         if (ndlc == NDLC_INFO_FRAME || ndlc == NDLC_DGRM_FRAME)
             enqueue(&ncia_ptr->fOutputQ, pak);
         else
             enqueue(&ncia_client->OutputQ, pak);
         break;
    }
}



void ncia_send_ndlc_no_ckt (nciapeertype *ncia_client, 
                            ClsDlcParamBlockT *clsBlock, ClsOpcodeT opCode)
{
    ndlc_msg_type_t ndlc;
    paktype         *pak;
    ndlc_packet_t   *ndlc_ptr;

    if (opCode == DLC_TEST_STN_RSP)
        ndlc = NDLC_I_CAN_REACH;
    else if (opCode == DLC_UDATA_STN_REQ)
        ndlc = DLC_UDATA_STN_REQ;
    else {
        datagram_done(clsBlock->fPakPtr);
        clsBlock->fPakPtr = NULL;
        return;
    }

    pak = ncia_get_ndlc_buffer(clsBlock->fPakPtr, ndlc);
    if (!pak) {
        NciaCircuitDebugError(
            "\nNCIA: send_ndlc: fail to get buffer for ndlc primitive %s", 
            ncia_print_ndlc(ndlc));
        return;
    }

    ndlc_ptr = (ndlc_packet_t *) pak->datagramstart;

    ndlc_ptr->ndlc_header.proto_ver_id = NCIA_PROTOCOL_ID | NCIA_VERSION_NO;
    ndlc_ptr->ndlc_header.message_type = ndlc;

    NciaCircuitDebugEvent("\nNCIA: send %s to client %i for mac %e",
                          ncia_print_ndlc(ndlc), ncia_client->ipaddr, 
                          ncia_client->Vmac);

    switch (opCode) {
    case DLC_TEST_STN_RSP:
         {
         DlcTestStnT  *dlcTestStn;
         dlcTestStn = &clsBlock->fParameters.fDlcTestStn;
         ieee_copy(dlcTestStn->fLMac,
                   ndlc_ptr->ndlc_data.i_can_reach.target_mac);
         ndlc_ptr->ndlc_data.i_can_reach.client_sap = dlcTestStn->fRSap;
         ndlc_ptr->ndlc_header.packet_length = sizeof(ndlc_i_can_reach_t) + 
                                               NDLC_HDR_SZ;
         }
         break;

    case DLC_UDATA_STN_REQ:
         {
         DlcUdataStnT *dlcUdataStn;
         dlcUdataStn = &clsBlock->fParameters.fDlcUdataStn;
         ieee_copy(dlcUdataStn->fLMac,
                   ndlc_ptr->ndlc_data.data_frame.target_mac);
         ndlc_ptr->ndlc_data.data_frame.client_sap = dlcUdataStn->fRSap;
         ndlc_ptr->ndlc_data.data_frame.target_sap = dlcUdataStn->fLSap;
         ndlc_ptr->ndlc_data.data_frame.broadcast_type = 0;
         ndlc_ptr->ndlc_header.packet_length = pak->datagramsize;
         }
         break;

    default:
         datagram_done(clsBlock->fPakPtr);
         clsBlock->fPakPtr = NULL;
         return;
    }

    switch(ncia_client->client_state) {
    case NCIA_OPENED:
         if (ncia_client->out_pak) {
             if (ncia_client->NciaFlags & NciaClientDebugERROR)
                 buginf("\nNCIA: outpak found");
             datagram_done(ncia_client->out_pak);
         } if (ncia_client->client_state == BUSY_XPORT)
             enqueue(&ncia_client->OutputQ, pak);
         else {
             ncia_client->out_pak = pak;
             ncia_pre_client_fsm(ncia_client, NDLC_FSM_INPUT);
         }
         break;

    case NCIA_CLOSED:
         ncia_pre_client_fsm(ncia_client, ACTIVE_OPEN);
         /* fall through */

    default:
         enqueue(&ncia_client->OutputQ, pak);
         break;
    }
}



static NciaCepT *ncia_get_cep_for_halt(nciapeertype *ncia_client, 
                                ndlc_packet_t *ndlc_packet)
{
    NciaCepT      *ncia_ptr;
    int            ndlc_type;
    int            my_id;
    int            remote_id;
 
    ndlc_type = ndlc_packet->ndlc_header.message_type;

    if (ndlc_type == NDLC_HALT_DL || ndlc_type == NDLC_HALT_DL_NOACK) {
        my_id = NDLC_DATA.halt_dl.receiver_ckt_id;
        remote_id = NDLC_DATA.halt_dl.sender_ckt_id;
    } else {
        remote_id = NDLC_DATA.halt_dl.receiver_ckt_id;
        my_id = NDLC_DATA.halt_dl.sender_ckt_id;
    }

    ForEachNciaCircuit(ncia_client, ncia_ptr) {
        /* if it's connect out, client might not have our id yet if client
           sends us HALT_DL before we send DL_STARTED. */
        if (ncia_ptr->fPCepId == my_id)
            return(ncia_ptr);
        if (NDLC_DATA.halt_dl.receiver_ckt_id == 0 &&
            ncia_ptr->fClientCktId == remote_id)
            return(ncia_ptr);
    }

    NciaServerDebugAll("\nNCIA: Warning, circuit id not found!");
    return(NULL);
}



NciaCepT *ncia_get_cep_from_ndlc (nciapeertype *ncia_client, paktype *pak)
{
    ndlc_packet_t *ndlc_packet;
    int ndlc_type;

    ndlc_packet = (ndlc_packet_t *) pak->datagramstart;
    ndlc_type = ndlc_packet->ndlc_header.message_type;

    switch (ndlc_type) {
    case NDLC_CAN_U_REACH:
         /* don't check here */
         return(NULL);

    case NDLC_DATA_FRAME:
         return(ncia_find_circuit(ncia_client, NDLC_DATA.data_frame.target_mac,
                                  NDLC_DATA.data_frame.target_sap, 
                                  NDLC_DATA.data_frame.client_sap));

    case NDLC_START_DL:
         /* the ndlc packet doesn't have out ckt id yet */
         return(ncia_find_circuit(ncia_client, NDLC_DATA.start_dl.target_mac,
                NDLC_DATA.start_dl.target_sap, NDLC_DATA.start_dl.client_sap));

    case NDLC_DL_STARTED:
         return((NciaCepT *) (NDLC_DATA.dl_started.origin_ckt_id));

    case NDLC_START_DL_FAILED:
         return((NciaCepT *) (NDLC_DATA.start_dl_failed.origin_ckt_id));

    case NDLC_XID_FRAME:
         return((NciaCepT *) (NDLC_DATA.xid_frame.session_id));

    case NDLC_CONTACT_STN:
         return((NciaCepT *) (NDLC_DATA.contact_stn.session_id));

    case NDLC_STN_CONTACTED:
         return((NciaCepT *) (NDLC_DATA.stn_contacted.session_id));

    case NDLC_INFO_FRAME:
         return((NciaCepT *) (NDLC_DATA.info_frame.session_id));

    case NDLC_HALT_DL:
    case NDLC_HALT_DL_NOACK:
    case NDLC_DL_HALTED:
         return(ncia_get_cep_for_halt(ncia_client, ndlc_packet));

    case NDLC_FCM_FRAME:
         return((NciaCepT *) (NDLC_DATA.fcm_frame.session_id));

    case NDLC_DGRM_FRAME:
         return((NciaCepT *) (NDLC_DATA.dgrm_frame.session_id));

    default:
         NciaCircuitDebugAll("\nNCIA: invalid message type");
         return(NULL);
    }
}



/*
 * packets in client output queue are not subject flow control.
 */
void ncia_drain_client_outputq (nciapeertype *ncia_client)
{
    paktype *pak;

    if (ncia_client->NciaFlags & NciaClientDebugEVENT)
        buginf("\nNCIA: drain client output queue (ip:%i mac:%e)",
                                ncia_client->ipaddr, ncia_client->Vmac);

    if (QUEUEEMPTY(&ncia_client->OutputQ)) {
        return;
    }

    while ((pak = dequeue(&ncia_client->OutputQ))) {
        if (ncia_client->out_pak) {
            if (ncia_client->NciaFlags & NciaClientDebugERROR)
                buginf("\nNCIA: outpak found");
            datagram_done(ncia_client->out_pak);
        }
        NciaCircuitDebugAll("\n      get one packet %x", pak);
        ncia_client->out_pak = pak;
        ncia_pre_client_fsm (ncia_client, NDLC_FSM_INPUT);
        if (ncia_client->client_state == BUSY_XPORT)
            return;
    }
}



/*
 * packets in circuit output queue are subject flow control.
 */
void ncia_drain_circuit_outQ (NciaCepT *ncia_ptr)
{
    paktype *pak;
    nciapeertype *ncia_client;

    NciaServerDebugAll("\nNCIA: ncia_drain_circuit_outQ");

    if (QUEUEEMPTY(&(ncia_ptr->fOutputQ)))
        return;

    ncia_client = ncia_ptr->fNciaClient;

    while ((pak = dequeue(&ncia_ptr->fOutputQ))) {
        /* subject flow control */
        if (ncia_ptr->fNciaCepFlags & NciaClientBusy || 
            ncia_ptr->fGrantPacket == 0) {
            requeue(&ncia_ptr->fOutputQ, pak);
            return;
        } else {
            if (ncia_client->out_pak) {
                if (ncia_client->NciaFlags & NciaClientDebugERROR)
                    buginf("\nNCIA:no outpak found");
                datagram_done(ncia_client->out_pak);
            }
            ncia_client->out_pak = pak;
            ncia_pre_client_fsm(ncia_client, NDLC_FSM_INPUT);
            if (ncia_client->client_state == BUSY_XPORT)
                return;
        }
    }
}




/*
 * verify a given circuit pointer is valid or not.
 */
boolean ncia_verify_circuit (ulong circuit)
{
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
    NciaCepT        *ncia_circuit;

    ForEachNciaServer(ncia_group, ncia_server)
        ForEachNciaClient(ncia_server, ncia_client)  
            ForEachNciaCircuit(ncia_client, ncia_circuit)
                if (circuit == (ulong) ncia_circuit)
                    return(TRUE);

    return(FALSE);
}


nciapeertype *ncia_verify_client (ipaddrtype client_ipaddr)
{
    ncia_servertype *ncia_server;
    nciapeertype    *ncia_client;
 
    ForEachNciaServer(ncia_group, ncia_server)
        ForEachNciaClient(ncia_server, ncia_client)  
            if (client_ipaddr == (ipaddrtype) ncia_client->ipaddr)
                    return(ncia_client);
    return((nciapeertype *) NULL);
}


/*
 * insert a Cep to the SAP's Cep List 
 * The circuit control block must be in the client control block's CEP list.
 */
void ncia_insert_cep (NciaCepT *ncia_circuit)
{
    NciaSapT *sap;
    NciaCepT *cep;

    sap = ncia_circuit->fPSap;

    if (!sap)
        return;

    for (cep = (NciaCepT *) sap->fNciaCep; cep; cep = cep->fNextCep)
        if (cep == ncia_circuit)
            break;

    if (!cep) {
        ncia_circuit->fNextCep = sap->fNciaCep;
        sap->fNciaCep = ncia_circuit;
        ncia_circuit->fNciaCepFlags |= NciaUsedBySAP;
    }
}



/*
 * remove a Cep from the SAP's Cep List.  Cep is not freed after removed.
 */
void ncia_remove_cep (NciaCepT *ncia_circuit)
{
    NciaSapT *sap;
    NciaCepT *cep;

    sap = ncia_circuit->fPSap;

    if (!sap)
        return;
    
    if (sap->fNciaCep == ncia_circuit) {
        /* first cep in cep list */
        sap->fNciaCep = ncia_circuit->fNextCep;
        return;
    }

    for (cep = (NciaCepT *) sap->fNciaCep; cep; cep = cep->fNextCep) {
        if (cep->fNextCep == ncia_circuit) {
            cep->fNextCep = ncia_circuit->fNextCep;
            ncia_circuit->fNciaCepFlags &= ~NciaUsedBySAP;
            return;
        }
    }
}
