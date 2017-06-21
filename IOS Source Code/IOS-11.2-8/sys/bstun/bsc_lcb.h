/* $Id: bsc_lcb.h,v 3.2.60.6 1996/09/10 08:48:18 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_lcb.h,v $
 *------------------------------------------------------------------
 * Protocol control block structure.
 *
 * February 1995. Andrew R. Rothwell.
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_lcb.h,v $
 * Revision 3.2.60.6  1996/09/10  08:48:18  jbalestr
 * CSCdi62395:  bsc/bstun debug for specific groups and addresses
 * Branch: California_branch
 *
 * Revision 3.2.60.5  1996/08/05  21:43:58  jbalestr
 * CSCdi60580:  bsc contention suppresses EOTs
 * Branch: California_branch
 *
 * Revision 3.2.60.4  1996/07/22  18:22:20  snyder
 * CSCdi63677:  fix a pointer problem, and move more text to data
 *              moves 2692 bytes to data, 16 out of image completely
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/06/17  08:29:55  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2.60.2  1996/05/17  10:40:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.4  1996/05/07  21:33:32  jbalestr
 * CSCdi52473:  bsc local-ack fails link for rx frames larger than 888
 * bytes at 2400
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.3  1996/04/26  07:32:32  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.2.48.2  1996/04/22  08:57:49  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.48.1  1996/04/03  13:50:18  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2.60.1  1996/03/18  19:01:15  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:39:01  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:38:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  08:48:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/03  01:04:03  arothwel
 * CSCdi39397:  Fix:
 *
 *         o Add specific poll support for local-ack pollee.
 *         o Remove superfluous fields from lcb.
 *         o Generalize protocol main handler.
 *         o Finish off replacing all deprecated process mgnt calls.
 *         o Multiple poll-adds causing hard-on polling.
 *         o Add safety code to ignore 'spurious' Tx completes.
 *
 * Revision 2.3  1995/08/11  08:21:51  arothwel
 * CSCdi38625:  Fix:
 *              1. Broken timer s/ware stopping Tx/Rx.
 *              2. Incorrect bsc-mib sub-system dependencies.
 *              3. Premature starting of bstun-event-process.
 *              4. Removal of timestamp info from debugging messages.
 *              5. Replace deprecated process mngmt s/ware w/ new s/ware.
 *
 * Revision 2.2  1995/06/16  06:24:39  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  20:14:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __LCB_H
#define __LCB_H

#include "master.h"
#include "types.h"
#include "interface.h"
#include "mgd_timers.h"

#include "bsc.h"
#include "bsc_hdx.h"

#define MAX_ADDRS           32
#define MAX_TIMER_TYPES     10
#define MAX_LCB_RETRY_CTRS   4
#define MAX_LCB_FRAME_CTRS   6
#define MAX_CUCB_RETRY_CTRS  6
#define MAX_CUCB_FRAME_CTRS  6

/*---- Forward typedef references. */

typedef struct cucb_t cucb_t;

/*---- Actual typedefs. */

/*---- Exported typedefs, enums & structures. */

typedef enum {
        CS_EBCDIC, CS_ASCII
        } char_set_e;

typedef enum {
        CU_ACTIVE, CU_NONRESPONDING, CU_INIT
        } cu_state_e;

typedef enum {
        CURS_OK, CURS_DELETE, CURS_RESTART, CURS_INACTIVE, CURS_ACTIVE
        } cu_reqstate_e;

typedef enum {
        LCB_IO_TIMER, LCB_PAUSE_TIMER, LCB_HOST_ACTIVITY_TIMER
        } timer_type_e;

typedef enum {
        LS_IDLE, LS_TX
        } line_state_e;

typedef enum {
        PR_UNKNOWN, PR_PRIMARY, PR_SECONDARY, PR_CONTENTION, PR_POLLER, PR_POLLEE
        } poll_role_e;

typedef enum {
        LCB_OBQ, CUCB_OBQ 
        } queue_origin_e;

typedef enum {
        WS_UNKNOWN, WS_WACK_ALL, WS_DO_WACK, WS_DONT_WACK
        } wack_status_e;

typedef struct {
        cucb_t* head;
        cucb_t* tail;
        int count;
        } cucb_list_t;

typedef struct {
        int maximum;
        int current;
        uchar list[MAX_ADDRS];
        } list_t;

typedef struct {
        ulong ctr[MAX_LCB_RETRY_CTRS];
        } lcb_retry_ctrs_t;

typedef struct {
        ulong ctr[MAX_LCB_FRAME_CTRS];
        } lcb_frame_ctrs_t;

typedef struct {
        ulong ctr[MAX_CUCB_RETRY_CTRS];
        } cucb_retry_ctrs_t;

typedef struct {
        ulong ctr[MAX_CUCB_FRAME_CTRS];
        } cucb_frame_ctrs_t;

typedef struct {
        ulong timer[MAX_TIMER_TYPES];
        } timer_table_t;

typedef void (*bcc_update_fn)( ushort, ushort* );

typedef void (*fsm_exec_t)(lcb_t* lcb, int event);

/*---- Exported structures. */

struct cucb_t {                         /* Control unit control block. */
       cucb_t* next;                    /* Ptr to next control unit cblk. */
       cucb_t* prev;                    /* Ptr to previous control unit cblk. */
       uchar address;                   /* Address of control unit. */
       uchar sel_address;               /* Selection address of CU. */
       uint active_dev_addr;            /* Currently active device address. */
       cu_state_e state;                /* State of control unit. */
       cu_reqstate_e req_state;         /* Required state of CU. */
       int  gstatus_pending;            /* count of status to be sent on gpoll */
       uint gpoll_status[MAX_ADDRS];    /* pending status for gpoll */
       uint device_status[MAX_ADDRS];   /* status to give when spec polled */
       wack_status_e wack_status;       /* wack in relation to printer frames */
       list_t spec_poll_list;           /* Specific poll list. */
       queuetype outboundQ;             /* Outbound queue. */
       cucb_retry_ctrs_t cfg_retry_ct;  /* Configured retry counters. */
       cucb_retry_ctrs_t run_retry_ct;  /* Runtime retry counters. */
       cucb_frame_ctrs_t tx_frame_ct;   /* Tx Frame counters. */
       cucb_frame_ctrs_t rx_frame_ct;   /* Tx Frame counters. */
       sys_timestamp no_addr_add_until; /* time last AddrAdd sent through tunnel */
       boolean no_poll;                 /* Is host still actioning CU. */
       boolean wack_status_sent;        /* has do/dont wack been sent yet */
       };

/*---- This structure is forward ref'd in ../h/types.h. */

struct lcb_t_ {                         /* Protocol control block. */
       poll_role_e poll_role;            /* Poller or pollee. */
       fsm_exec_t fsm;                  /* Poller/pollee fsm entrypoint. */
       int state;                       /* State of polling fsm. */
       int line_state;                  /* State of line. */
       hwidbtype* idb;                  /* Ptr back to idb. Set on init. */
       paktype* timer_pak;              /* Timer pak ptr. */
       paktype* host_timer_pak;         /* Host Timer pak ptr. */
       paktype* cs_pak;                 /* Control sequence pak ptr. */
       paktype* tx_pak;                 /* Transmit pak ptr. */
       paktype* frame_builder_pak;      /* to build pak from receive chars */
       paktype* rx_pak;                 /* Receive pak ptr. */
       uchar tx_save[4];                /* Tx saved header from Txmit pak. */
       ulong servlim;                   /* Poll-list cycle ratio: */
                                        /* Active versus non-responding. */
       int poll_retries;                /* Poll retry count for CUs. */
       boolean flush;                   /* Flush frame until ETX/ABORT rcvd. */
       cucb_t* active_cu;               /* Currently active CU. */
       int active_poll_cycle_count;     /* */
       list_t active_cu_list;           /* Active control unit list.*/
       list_t non_responding_cu_list;   /* Control unit not responding list. */
       boolean spec_poll_done;          /* Done specific poll? */
       queuetype outboundQ;             /* Outbound queue. */
       boolean lack_handle_spec_poll;   /* Handle specific polls only. */
       cucb_list_t cucb_list;           /* Head of control unit list. */
       boolean pending_cu_change;       /* A cu on cucblist needs attention. */
       uchar tx_ack_next;               /* Next expected Tx ACK value. */
       uchar rx_ack_next;               /* Next expected Rx ACK value. */
       lcb_retry_ctrs_t cfg_retry_ct;   /* Configured retry counters. */
       lcb_retry_ctrs_t run_retry_ct;   /* Runtime retry counters. */
       lcb_frame_ctrs_t tx_frame_ct;    /* Tx frame counters. */
       lcb_frame_ctrs_t rx_frame_ct;    /* Rx frame counters. */
       queue_origin_e origin_queue;     /* Queue from which we Tx'd to line. */
       ulong unknownCUCBAddrRx;         /* Count of frames received from the serial
                                           interface with an unsupported CUCB address*/
        /*
        * fields to be used for bsc passthrough mode
        */
       uint            frame_seq_state; /* Frame sequencing FSM state. */
       uint            rx_state;        /* state of RBB FSM */
       uint            hdx_state;       /* state of HDX enforcement FSM */
       uchar           ns;              /* Current sequence number */
       uchar           secondary_address; /* Secondary addr used by station */
       uchar           input;           /* Current byte on data stream. */
       uchar           frame_type_rcvd; /* Frame type received from device. */
       bsc_hdx_frame_e txCompType;      /* frame type expected on tx comp */

      /*
       * fields dependant on char-set
       */
       char_set_e      char_set;        /* Char set to use. ASCII/EBCDIC. */
       const uchar*    bsc_cs_table;    /* Ptr to Control Sequence table. */
       const uchar*    char_table;      /* Ptr to char-set -> BSC xlate table.*/
       const uchar*    ps_table;        /* Poll->Select address lookup table */
       const uchar*    sp_table;        /* Select->Poll address lookup table. */
       const uchar*    dev_addr_table;  /* device address lookup table */
       uint            bcc_length;      /* number of bcc bytees */
       bcc_update_fn   bcc_fn;          /* accumulates block check character */
  
       /*
        * timer things
        */
       mgd_timer io_timer;              /* IO timer. */
       timer_table_t io_timer_table;    /* IO timer table (of timeouts). */
       ulong next_io_timeout;           /* Next IO timeout value to use. */
       mgd_timer pause_timer;           /* Pause timer. */
       ulong pause_timeout;             /* Pause timeout value. */
       mgd_timer host_activity_timer;   /* Host activity timer. */
       ulong host_activity_timeout;     /* Host activity timeout value. */
};


/*---- Exported inlines. */

static inline uchar bsc_map_sel_to_pol (lcb_t* lcb, uchar address)
{
   return lcb->ps_table[address];
}

static inline uchar bsc_map_pol_to_sel (lcb_t* lcb, uchar address)
{
   return lcb->sp_table[address];
}


/*---- Exported functions. */

extern lcb_t*
lcb_create (hwidbtype* idb,
	    mgd_timer* io_timer_parent);

extern void
lcb_delete (lcb_t* lcb);

extern void
lcb_partial_init (lcb_t* lcb);

#endif  /* __LCB_H. */
/*---- End include file: bsc_lcb.h. */
