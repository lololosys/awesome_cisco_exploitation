/* $Id: bsc_lack_support.h,v 3.3.54.1 1996/05/17 10:40:37 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_lack_support.h,v $
 *------------------------------------------------------------------
 * Support code header file for BSC local-ack.
 *
 * February 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_lack_support.h,v $
 * Revision 3.3.54.1  1996/05/17  10:40:37  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.42.2  1996/04/22  08:57:47  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.42.1  1996/04/03  13:50:16  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.3  1995/11/21  08:52:20  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.2  1995/11/17  08:48:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/03  01:04:02  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.2  1995/06/16  06:24:38  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_LACK_SUPPORT_H
#define __BSC_LACK_SUPPORT_H

/*---- Exported constants. */

#define OUTBOUND_QUEUE_DEPTH       10   /* entries. */

#define DEFAULT_IO_TIMEOUT        (30 * ONE_SECOND)
#define LINE_CHECK_TIMEOUT        (10 * ONE_SECOND)
#define LINE_IDLE_START_TIMEOUT   1
#define ACK_TIMEOUT               ( 3 * ONE_SECOND)
#define ENQ_TIMEOUT               ( 3 * ONE_SECOND)
#define POLL_TIMEOUT              ( 3 * ONE_SECOND)
#define TTD_DELAY_TIMEOUT         ( 2 * ONE_SECOND)
#define POLL_ADDR_ADD_TIMEOUT     (30 * ONE_SECOND)

/*---- Config defaults. */

#define DEFAULT_SERVLIM            3
#define DEFAULT_POLL_RETRIES       6
#define POLLER_PAUSE_TIMEOUT       (ulong) (.3 * ONE_SECOND)
#define DEFAULT_HOST_ACTIVITY_TIMEOUT  (60 * ONE_SECOND)

/*---- Exported typedef'd enums and structures. */

typedef enum {
        SS_OK, SS_DE, SS_DB, SS_OTHER
        } sense_status_e;

/*---- Device Status' for BSC devices ----*/  
#define BSC_DEVICE_BUSY     0xC840
#define BSC_DEVICE_BUSY_BIT 0x0800
#define BSC_DEVICE_END      0xC240
#define BSC_DEVICE_END_BIT  0x0200
#define BSC_DEVICE_OK       0x0000
#define BSC_DEVICE_ALL      0x0E3D

/*---- Exported prototypes. */

#define GET_CUADDR_FROM_PAK(pak) (uchar) pak->datagramstart[0]
#define GET_CUCMD_FROM_PAK(pak) (uchar) pak->datagramstart[1]

void
bsc_lack_hardware_handler(paktype* pak, hwidbtype* idb);

void
bsc_lack_network_handler(paktype* pak, hwidbtype* idb);

void
bsc_lack_timer_handler(paktype* pak, hwidbtype* idb);

void
bsc_poller_fsm_exec(lcb_t* lcb, int event);

void
bsc_pollee_fsm_exec(lcb_t* lcb, int event);

void
bsc_lack_poll_list_delete_all(lcb_t* lcb);

void
bsc_poll_list_init(list_t* this);

void
bsc_poll_list_add(list_t* this, uchar address);

void
bsc_poll_list_delete(list_t* this, uchar address);

void
bsc_poll_list_move(list_t* from, list_t* to, uchar address);

cucb_t*
bsc_cucb_create(lcb_t* lcb, uchar address);

void
bsc_cucb_delete(lcb_t* lcb, cucb_t* this);

void
bsc_cucb_inactivate(cucb_t* cucb);

cucb_t*
bsc_cucb_find(lcb_t* lcb, uchar address);

void
bsc_cucb_delete_all(lcb_t* lcb);

void
bsc_cucb_list_init(cucb_list_t* cucb_list);

boolean
bsc_send_peer_alert(lcb_t* lcb, uchar cu_addr, uchar cu_cmd, uchar cu_devaddr);

void
bsc_timer_table_init(timer_table_t* this);

void
bsc_set_lcb_cfg_retry_counters(lcb_t* lcb);

void
bsc_reset_lcb_retry_counters(lcb_t* lcb);

void
bsc_reset_cucb_retry_counters(cucb_t* cucb);

void
bsc_format_enq_frame(lcb_t* lcb, paktype* pak, uchar cu_addr, uchar dev_addr);

void
bsc_format_device_status(lcb_t* lcb, paktype* pak, uchar cu_addr, uchar dev_addr, uint sts );

boolean
bsc_query_printer_ack(lcb_t* lcb, paktype* pak);

uchar
bsc_set_next_ack(lcb_t* lcb, uchar ack_rcvd);

boolean
bsc_flush_queue(queuetype* qp, uchar cu_cmd);

void
bsc_flush_all_queue(queuetype *qp);

void
bsc_cucb_process_poll_updates(lcb_t* lcb);

int
bsc_poll_sched(lcb_t* lcb);

int
bsc_find_next_poll_work(lcb_t * lcb);

void
bsc_update_pollee_device_status(lcb_t* lcb, paktype* pak );

void
bsc_update_poller_device_status(lcb_t* lcb, paktype* pak );

sense_status_e
bsc_device_clear_for_frame(lcb_t* lcb, cucb_t* cucb);

uint
bsc_get_unit_number(lcb_t* lcb, uint dev_addr );

#endif  /* __BSC_LACK_SUPPORT_H. */
/*---- end include file: bsc_lack_support.h. */
