/* $Id: tring_private.h,v 3.1.42.2 1996/08/07 09:00:06 snyder Exp $
 * $Source: /release/112/cvs/Xsys/if/tring_private.h,v $
 *- -----------------------------------------------------------------
 * tring_private.h -- 802.5 Token Ring private definitions.
 * 
 * December 1995, Greg Stovall
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tring_private.h,v $
 * Revision 3.1.42.2  1996/08/07  09:00:06  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.1.42.1  1996/03/18  20:15:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.2  1996/03/14  01:05:28  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.1.2.1  1996/01/24  22:19:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.1  1996/01/23  21:45:05  gstovall
 * Placeholders for the king.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TRING_PRIVATE_H__
#define __TRING_PRIVATE_H__

/*
 * TR_BUGINF_INTERVAL determines a window in which we won't display the
 * same failure message.
 */
#define TR_BUGINF_INTERVAL (4*60*ONESEC)
#define TR_KEEP_COUNT 8	   /* Number of keepalives to miss before
			   the protocol down */

/*
 * 4M Parameters
 */
#define VISIBLE_TR_BANDWIDTH	4000
#define TR_DELAY	250		/* not sure about this */
#define TR_HOLDQ	40		/* max packets queued for output */

/*
 * 16M Parameters
 */
#define VISIBLE_TR16_BANDWIDTH	16000
#define TR16_DELAY	63		/* actually 62.5 */
#define TR16_HOLDQ	40		/* max packets queued for output */

/*
 * Token ring subblock definition
 */
typedef struct tokensb_ {
    sys_timestamp tr_status_time;	/* time it last changed */

    /*
     * tr_lineup is a counter that keeps track of keepalives
     * we can miss before we take 'er down.  When it hits zero down
     * it goes.  When accessed it is looked at as a boolean.
     */
    int tr_lineup;			/* times we can miss <> 0 is up */
    sys_timestamp tr_lineuptimer;	/* when the last keepalive came in */
    ulong tr_group;			/* group multicast for interface */
    ulong tr_functional;		/* functional address for if !*/
    ushort tr_ring_status;		/* last ring status we heard about !*/
    timer_t *mbus_tr_timer;		/* transmitter delay via timers.c */
    sys_timestamp tr_test_timer;	/* interface test timer */

    /*
     * TR Debugging and crash information
     */    
    ulong tr_last_panic;
    sys_timestamp tr_last_panic_time;
    ushort tr_fail_count;		/* how many failures were there?  !*/
    ushort tr_last_fail;		/* last interface failure         !*/
    ulong tr_last_fail_code;		/* last associated failure code   !*/
    sys_timestamp tr_last_fail_time;	/* when it failed.                !*/
    sys_timestamp tr_next_buginf_time;	/* next time to display a failure */

    /*
     * The dead man timer in Spyglass is disabled in E-OIR.
     * The following fields are used to inplement a similar
     * timer function at the system level
     */
    short tr_polling_count;		/* polling timer */
    short tr_polling_failure;		/* number of continuous failures */
    int   tr_polling_state;		/* state of the polling */

    /*
     * Support for draft bridge mib.  Moved here from board devctl
     * blocks to generalize code.
     */
    ulong signal_loss;			/* ring error - loss of signal    */
    ulong hard_errs;			/* ring error - unrecoverable     */
    ulong soft_errs;			/* ring error - recoverable error */
    ulong tx_beacon;			/* ring error - beacons on ring   */
    ulong wire_fault;			/* ring error - fault in cable    */
    ulong rmt_removal;			/* ring error - kicked off ring   */
    ulong single_station;		/* ring error - only station      */
    ulong ring_recovery;		/* ring error - recovery mode     */

    ulong prev_signal_loss;
    ulong prev_hard_errs;
    ulong prev_soft_errs;
    ulong prev_tx_beacon;
    ulong prev_wire_fault;
    ulong prev_rmt_removal;
    ulong prev_single_station;
    ulong prev_ring_recovery;
    ulong prev_bad_acbits;

    ulong prev_err_line;
    ulong prev_err_internal;
    ulong prev_err_burst;
    ulong prev_err_arifci;
    ulong prev_err_abort;
    ulong prev_err_lostframe;
    ulong prev_err_rcvcongestion;
    ulong prev_err_copy;
    ulong prev_err_freq;
    ulong prev_err_token;
    uchar naun[IEEEBYTES];	      /* nearest active upstream neighbor */
    uchar beacon_naun[IEEEBYTES];     /* nearest beaconing upstream neighbor */
    ushort last_beacon_type;

    ulong ringOpenStatus;
    /*
     * Vector to hold pre-existing state/config vector which was
     * in place before we initialized an TokenRing IDB as an TokenRing IDB.
     * If the TokenRing state/config vector is not able to handle one
     * or more items, that itemlist should be passed to the "super"
     * function.
     */
    hwidb_state_config_t	super_state_config;
} tokensb;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
#define TR_OPN_PHASES 6
#define TR_OPN_ERRORS 11
#define TR_RSTAT_BUF_SIZE 256


extern char *const tr_panic_types[];
extern char ring_status_text[];

/*
 * return non-zero if srb explorer - ASSUMES rif is present
 */
static inline boolean isExplorerTrh(trrif_t *trh)
{
	return((trh->rc_blth & (RC_BROADBIT|RC_SPANBIT|RC_TRACE_ORIG)));
}

boolean tr_ringstat_common(hwidbtype *, ushort, boolean);
void tr_send_loop(hwidbtype *, uchar *);
void TR_OpenError(hwidbtype *idb, ushort error, ushort phase, boolean display);
void tring_update_counters(tokensb *tsb);
void print_ring_status(hwidbtype *hwidb);
void new_tring_speed(hwidbtype* idb, int speed, boolean initial);
boolean tr_test_idbupBLOCK(hwidbtype *);
void decode_ring_status(char *, ushort);
void tring_idb_init(hwidbtype *, int);


extern void tring_parser_init(void);

#endif /* __TRING_PRIVATE_H__ */
