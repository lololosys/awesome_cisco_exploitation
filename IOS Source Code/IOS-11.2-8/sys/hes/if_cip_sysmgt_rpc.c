/* $Id: if_cip_sysmgt_rpc.c,v 3.4.12.5 1996/08/28 12:44:47 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip_sysmgt_rpc.c,v $
 *------------------------------------------------------------------
 * if_cip_sysmgt_rpc.c - CIP system management RPC interface.
 *
 * April 1995, Mike Otto
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_cip_sysmgt_rpc.c,v $
 * Revision 3.4.12.5  1996/08/28  12:44:47  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.12.4  1996/08/07  08:58:58  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.12.3  1996/06/02  15:31:50  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.4.12.2  1996/05/17  11:17:55  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.2.2  1996/04/03  14:29:34  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.2.1  1996/03/21  06:08:00  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.4.12.1  1996/03/18  19:41:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.16.3  1996/03/16  06:50:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.16.2  1996/03/07  09:39:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.16.1  1996/01/24  21:46:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/16  13:25:20  banderse
 * CSCdi49063:  Sysmgt IPC communication should function when interface is
 * shutdown
 *
 * Revision 3.3  1995/12/05  14:53:04  motto
 * CSCdi43295:  Get msg at startup: IPC Master:NETMGT.TRAP locate port
 * failed ()
 *
 * Revision 3.2  1995/11/17  09:17:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:40:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:00:28  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/08/22  17:18:38  motto
 * CSCdi37937:  CIP sysmgt RPCs timeout when all interfaces shutdown
 *
 * Revision 2.4  1995/07/13  19:41:53  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.3  1995/06/28  09:24:37  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/09  13:04:38  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:41:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "subsys.h"
#include "sys_registry.h"
#include "cbus_registry.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "if_cip_sysmgt_rpc.h"
#include "if_cip.h"
#include "../if/channel_debug.h"
#include "channel_sb.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "msg_sysmgt.c"			/* Not a typo, see logger.h */

char *const netmgt_operations[NETMGT_MAX_OPERATIONS] = {"SET", "GET EXACT", 
                                                        "GET NEXT"};

#define CIP_SYSMGT_IPC_RPC_TIMOEUT      5

/*
 * Statistics
 */
static ulong sysmgt_request_cnt  = 0;
static ulong sysmgt_req_sent_cnt = 0;
static ulong sysmgt_rsp_rxd_cnt  = 0;
static ulong sysmgt_failed_cnt   = 0; 
static ulong sysmgt_events       = 0;

/*
 * SYSMGT IPC info
 */
typedef struct sysmgt_ipc_info_ {
    ipc_port_id nm_query_port_id;
    boolean     nm_request_semaphore[SYSMGT_MAX_CLASS];
} sysmgt_ipc_info_t;

static sysmgt_ipc_info_t *get_cip_sysmgt_info(hwidbtype *hwidb);
static boolean set_cip_sysmgt_info(hwidbtype *hwidb, sysmgt_ipc_info_t *info);
static boolean netmgt_synchronous_queryBLOCK(boolean *nm_request_semaphore);

/*
 * Event variables
 */
static ipc_port_id netmgt_trap_port_id = 0;
static forktype netmgt_event_handler(void);
static queuetype netmgt_event_queue;

#define NETMGT_TRAP_PORT_NAME   "IPC Master:NETMGT.TRAP"

/*
 * cip_sysmgt_rpc_init
 */
static void cip_sysmgt_rpc_init (subsystype *subsys) {
    
    cfork(netmgt_event_handler, 0L, 1000, "SYSMGT Events", 0);
}

/*
 * Process a NETMGT event message
 */
static void process_event (netmgt_header_t *nm_hdr) {
    if (nm_hdr->flags.control == NETMGT_TRAP) {
        
        if (channel_sysmgt_debug) {
            buginf("\nCIP NETMGT.TRAP, class=%d slot=%d type =%d length=%d\n",
              nm_hdr->class, nm_hdr->slot, nm_hdr->type, nm_hdr->data_length);
	}
        
        if (nm_hdr->class < SYSMGT_MAX_CLASS) {
            reg_invoke_cip_netmgt_event(nm_hdr->class, nm_hdr->slot, 
                                        nm_hdr->type, nm_hdr->data_length, 
                                        nm_hdr->data);
	}
        else {
            errmsg(&msgsym( NETMGT_EVENT, SYSMGT_RPC),"invalid class",
                   nm_hdr->class);
        }
    }
    else {
        errmsg(&msgsym( NETMGT_EVENT, SYSMGT_RPC),"invalid control",
               nm_hdr->flags.control);
    }
}
 
/*
 * This function provides the process level blocking
 * for sysmgt event notification messages from the CIP. 
 * It keeps the process blocked while there is no event
 * on the sysmgt_event_queue.
 */
static boolean netmgt_event_block (ulong context) {
  return (!netmgt_event_queue.count);
}

/*
 * System management event handler task (SYSMGT)
 */
static forktype netmgt_event_handler (void) {
    leveltype level;
    ipc_message *ipc_trap_msg = NULL;
    boolean netmgt_ev_running = TRUE;
    ipc_error_code code;
    queuetype *netmgt_ev_q_p = NULL;
    netmgt_header_t *nm_hdr;

    queue_init(&netmgt_event_queue, 0);
    netmgt_ev_q_p = &netmgt_event_queue;

    process_sleep_for(ONESEC*1);  /* give time for IPC to settle */
    if (channel_sysmgt_debug)
        buginf("CREATING %s PORT\n", NETMGT_TRAP_PORT_NAME);
    code = ipc_create_named_port(NETMGT_TRAP_PORT_NAME, &netmgt_trap_port_id,
                                 IPC_PORT_OOB);
    if (code != IPC_OK) {
        errmsg(&msgsym( IPC_ERROR, SYSMGT_RPC),
               NETMGT_TRAP_PORT_NAME, 
               "port creation",
               ipc_decode_error(code));
        return;
    }

    code = IPC_NO_SUCH_PORT;
    while (code != IPC_OK) {
        if (channel_sysmgt_debug)
            buginf("REGISTERING %s PORT\n", NETMGT_TRAP_PORT_NAME);
        code = reg_invoke_ipc_register_port( netmgt_trap_port_id);
        if (code == IPC_NO_MEMORY) {
            errmsg(&msgsym( IPC_ERROR, SYSMGT_RPC), 
                   NETMGT_TRAP_PORT_NAME, 
                   "port registration",
                   ipc_decode_error(code));
            return;
        }
        if (code != IPC_OK) {
            if (channel_sysmgt_debug)
                buginf("PORT %s REGISTRATION FAILURE(%s)\n", 
                       NETMGT_TRAP_PORT_NAME, ipc_decode_error(code));
            process_sleep_for(ONESEC*10);
	}
    }

    if (channel_sysmgt_debug)
        buginf("SET RECEIVE QUEUE %s PORT\n", NETMGT_TRAP_PORT_NAME);
    code = ipc_set_receive_queue( netmgt_trap_port_id, netmgt_ev_q_p);
    if (code != IPC_OK) {
        errmsg(&msgsym( IPC_ERROR, SYSMGT_RPC),  
               NETMGT_TRAP_PORT_NAME, 
               "set receive queue",
               ipc_decode_error(code));
        netmgt_ev_running = FALSE;
    } else {
        if (channel_sysmgt_debug)
            buginf("PORT %s(%u) INITIALIZED\n", NETMGT_TRAP_PORT_NAME,
                                                  netmgt_trap_port_id);
    }
    
    while (netmgt_ev_running) {
        if (netmgt_ev_q_p->count) {
            ipc_trap_msg = dequeue(netmgt_ev_q_p);
            if (ipc_trap_msg) {
                nm_hdr = (netmgt_header_t *)ipc_trap_msg->data;
                nm_hdr->data = (uchar *)ipc_trap_msg->data + NETMGT_HDR_SIZE;
                process_event(nm_hdr);
                sysmgt_events++;
                reg_invoke_ipc_return_message( ipc_trap_msg);
            }
        }
        else {
	    edisms((blockproc*)netmgt_event_block, 0L);
        }
    }

    /*
     * close port
     */
    code = reg_invoke_ipc_close_port( netmgt_trap_port_id);
    if (code != IPC_OK) {
        errmsg(&msgsym( IPC_ERROR, SYSMGT_RPC), 
               NETMGT_TRAP_PORT_NAME,
               "close port", 
               ipc_decode_error(code));
    }

    /*
     * Cleanup queue
     */
    level = raise_interrupt_level(NETS_DISABLE);
    while (netmgt_ev_q_p->count)
	reg_invoke_ipc_return_message(dequeue(netmgt_ev_q_p));
    netmgt_ev_q_p = NULL;
    reset_interrupt_level(level);
    process_kill(THIS_PROCESS);
}

/*
 * This function provides the process level blocking
 * for netmgt RPC queries from the CIP. 
 * It blocks the process to syncrhonize request based on
 * CIP and class of information request.
 */
static boolean netmgt_synchronous_queryBLOCK (boolean *nm_request_semaphore) {
  return *nm_request_semaphore;
}

/*
 * Get CIP sysmgt IPC info
 */
static sysmgt_ipc_info_t *get_cip_sysmgt_info (hwidbtype *hwidb) {
    channel_sb_t *chan_sb;

    chan_sb = channel_get_sb(hwidb);
    if (chan_sb && chan_sb->cip_info) {
        return chan_sb->cip_info->sysmgt_ipc_info;
    } else {
        return NULL;
    }
}

/*
 * Set CIP sysmgt IPC info
 */
static boolean set_cip_sysmgt_info (hwidbtype *hwidb, sysmgt_ipc_info_t *info) {
    channel_sb_t *chan_sb;

    chan_sb = channel_get_sb(hwidb);
    if (chan_sb && chan_sb->cip_info) {
        chan_sb->cip_info->sysmgt_ipc_info = info;
        return TRUE;
    }
    return FALSE;
}

/*
 * Reset sysmgt IPC info
 */
void cip_sysmgt_info_reset (hwidbtype *hwidb) {
    sysmgt_ipc_info_t *info = NULL;
    channel_sb_t *chan_sb;

    chan_sb = channel_get_sb(hwidb);
    if (chan_sb && chan_sb->cip_info) {
        info = chan_sb->cip_info->sysmgt_ipc_info;
        if (info) {
            info->nm_query_port_id = 0;
	}
    }
}

/*
 * Gets netmgt query info if valid interface
 * and IPC port can be located.
 */
static sysmgt_ipc_info_t *get_netmgt_query_info (hwidbtype *hwidb,
						 sysmgt_class_t class) {
    sysmgt_ipc_info_t *info = NULL;
    char ipc_name[32];
    int i;
    
    if (class < SYSMGT_MAX_CLASS) {
        if ((hwidb) && 
            (!(hwidb->status & IDB_NOHARDWARE)) &&
            (hwidb->type == IDBTYPE_CIP)) {
            if ((info = get_cip_sysmgt_info(hwidb)) == NULL) {
                if ((info = malloc( sizeof(sysmgt_ipc_info_t))) != NULL) {
                    info->nm_query_port_id = 0;
                    for(i=0;i<SYSMGT_MAX_CLASS;i++) {
                        info->nm_request_semaphore[i] = FALSE;
                    }
                    if (!set_cip_sysmgt_info(hwidb, info)) {
                        free(info);
                        return NULL;
                    }
                }
    	    }
            if (info && (info->nm_query_port_id == 0)) {
                sprintf( ipc_name, "SLOT%1d:NETMGT.QUERY", hwidb->slot);
                info->nm_query_port_id = reg_invoke_ipc_locate_port( ipc_name);
                if (channel_sysmgt_debug) {
                    buginf("CIP locate %s port id=%u\n", ipc_name, 
                                                         info->nm_query_port_id);
                }
                if (info->nm_query_port_id) {
                    edisms( (blockproc *)netmgt_synchronous_queryBLOCK, 
                            (long)&info->nm_request_semaphore[class]);
                }
            }
        }
    }
    return info;
}

/*
 * This is the sysmgt RPC interface function. Call this 
 * routine to query information from the CIP card. The 
 * process should not pass in any non-static or stack
 * allocated buffers. They could get lost if the process
 * is killed while it is blocked here.
 *  INPUT: CIP hwidb
 *         nm_hdr structure
 *         pointer to data
 * OUTPUT: sm_data if successfull RPC  or NULL if RPC failed.
 */
void *sysmgt_rpc ( hwidbtype *hwidb, netmgt_header_t *nm_hdr, void *nm_data) {
    void *result_p = NULL;
    char *buf;
    ipc_error_code err;
    ipc_message *ipc_msg;
    sysmgt_ipc_info_t *info;
    
    sysmgt_request_cnt++;
    nm_hdr->flags.control = NETMGT_REQUEST;  
    nm_hdr->flags.ret_code  = NETMGT_NO_RESOURCES;
    nm_hdr->flags.reserved2 = 0;
    info = get_netmgt_query_info( hwidb, nm_hdr->class);
    if (info && (info->nm_query_port_id != 0)) {
        ipc_msg = reg_invoke_ipc_get_message( NETMGT_HDR_SIZE+nm_hdr->index_length, 
                                   info->nm_query_port_id, 0);
        if (ipc_msg) {
            buf = (char *)ipc_msg->data;  
	    bcopy(nm_hdr, ipc_msg->data, NETMGT_HDR_SIZE);
            if (nm_hdr->index_length)
	        bcopy(nm_data, &buf[NETMGT_HDR_SIZE], nm_hdr->index_length);
            info->nm_request_semaphore[nm_hdr->class] = TRUE;
            sysmgt_req_sent_cnt++;
            reg_invoke_ipc_set_rpc_timeout(ipc_msg, CIP_SYSMGT_IPC_RPC_TIMOEUT);
            ipc_msg = reg_invoke_ipc_send_rpc(ipc_msg, &err);
            info->nm_request_semaphore[nm_hdr->class] = FALSE;
            if ((err == IPC_OK) && (ipc_msg != NULL)) {
                sysmgt_rsp_rxd_cnt++;
                result_p = nm_data;
                bcopy(ipc_msg->data, nm_hdr, NETMGT_HDR_SIZE);
                switch (nm_hdr->flags.ret_code) {
	          case NETMGT_FOUND:
                    buf = (char *)ipc_msg->data;
                    bcopy(&buf[NETMGT_HDR_SIZE], nm_data, nm_hdr->data_length);
                    break;
                  case NETMGT_NOT_FOUND:
                    result_p = NULL;
                    break;
                  default:
                    if (channel_sysmgt_debug)
                        buginf("%s: SYSMGT - Invalid response code (%d) - class (%d)\n",
                               hwidb->hw_namestring, nm_hdr->flags.ret_code, 
                               nm_hdr->class);
                    result_p = NULL;
                    break;
                }
            }
            else {
                sysmgt_failed_cnt++;
                cip_sysmgt_info_reset(hwidb);
                if (channel_sysmgt_debug)
                    buginf("%s: SYSMGT - IPC RPC request failed (%s)\n",
                            hwidb->hw_namestring, ipc_decode_error(err));
            }
            if (ipc_msg)
                reg_invoke_ipc_return_message( ipc_msg);
        }
        else {
            sysmgt_failed_cnt++;
            if (channel_sysmgt_debug)
                buginf("%s: SYSMGT - Out of buffers for IPC RPC request\n",
                        hwidb->hw_namestring);
        }   
    }
    else {
        sysmgt_failed_cnt++;
        if (channel_sysmgt_debug)
	    buginf("%s: SYSMGT - No IPC port id\n", hwidb->hw_namestring);
    }
    return result_p;
}

/*
 * Display the System Management RPC statistics/state
 */
void show_sysmgt_rpc (parseinfo *csb) {
    automore_enable(NULL);
    printf("\nSystem Management RPC Statistics");
    printf("\n   Requests      Req Sent       Rsp Rxd      Failures        Events");
    printf("\n %-10u    %-10u    %-10u    %-10u    %-10u\n", sysmgt_request_cnt,
              sysmgt_req_sent_cnt, sysmgt_rsp_rxd_cnt, 
              sysmgt_failed_cnt, sysmgt_events);
    automore_disable();
}

/*
 * SYSMGT RPC subsystem header
 */

#define CIP_SYSMGT_RPC_MAJVERSION   1
#define CIP_SYSMGT_RPC_MINVERSION   0
#define CIP_SYSMGT_RPC_EDITVERSION  0

SUBSYS_HEADER(cip_sysmgt_rpc, CIP_SYSMGT_RPC_MAJVERSION,
	      CIP_SYSMGT_RPC_MINVERSION, 
              CIP_SYSMGT_RPC_EDITVERSION, cip_sysmgt_rpc_init,
	      SUBSYS_CLASS_MANAGEMENT,
	      "req: ipc, cip", NULL);
