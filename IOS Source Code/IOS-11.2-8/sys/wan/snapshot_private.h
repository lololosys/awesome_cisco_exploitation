/* $Id: snapshot_private.h,v 3.3 1995/11/17 18:05:03 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/snapshot_private.h,v $
 *------------------------------------------------------------------
 * snapshot_private.h - Private definitions for snapshot routing
 *
 * June 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Private header definitions for snapshot routing -- used only by
 * snapshot code.
 *------------------------------------------------------------------
 * $Log: snapshot_private.h,v $
 * Revision 3.3  1995/11/17  18:05:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:46  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:51:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNAPSHOT_PRIVATE_H__
#define __SNAPSHOT_PRIVATE_H__
#include "../h/bitlogic.h"

/*
 * A Snapshot IDB can be on one of the following QUEUES:
 *
 *	SNAPSHOT_ACTIVE_Q = Interface UP for sending updates, and we
 *                                are currently doing this.
 *      SNAPSHOT_CLIENT_QUIET_Q = Interface isn't due for an update.
 *	SNAPSHOT_SERVER_POST_ACTIVE_Q = Interface is a server that just left
 *				     its active period. This short post-active
 *				     period is a 'dead time' when aging
 *				     won't be restarted, because a heavily
 *				     loaded client could send updates AFTER
 *				     the server went in 'quiet' time. If
 *				     this were done, it would reactivate
 *				     the server, (and start routes to aging
 *                                   again) with only a very short active time
 *                                   left on the client -- possibly before
 *                                   an update could finish. Instead, the
 *				     server waits a few minutes after the
 *				     last active interval, thereby guaranteeing
 *				     the client will not 'awaken' the
 *				     server at this bad time.
 *	SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q = Interface is a client,
 *				     which just left the active state, and
 *				     will reenter the quiet state later. This
 *				     time slot allows for there to be no
 *				     immediate reactivation after just going
 *				     post-active, so a interface statechange
 *				     doesn't bring us back up in the server's
 *				     post active window (and a flapping line
 *                                   coming back up doesn't do it either).
 *	SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q = Client was in the 'post
 *				     active next quiet' queue but something
 *				     happened to tell it to wake up. Therefore,
 *				     this new queue says 'go active again right
 *				     away.'
 */
#define SNAPSHOT_NO_Q 			(0)
#define SNAPSHOT_ACTIVE_Q 		(SNAPSHOT_NO_Q + 1)
#define SNAPSHOT_CLIENT_QUIET_Q		(SNAPSHOT_ACTIVE_Q + 1)
#define SNAPSHOT_SERVER_POST_ACTIVE_Q 	(SNAPSHOT_CLIENT_QUIET_Q + 1)
#define SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q \
					(SNAPSHOT_SERVER_POST_ACTIVE_Q + 1)
#define SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q \
				(SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q + 1)
#define SNAPSHOT_QUEUE_COUNT \
				(SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q + 1)

#define SNAPSHOT_IS_IN_THIS_QUEUE(sab, q) ((sab)->runtime_queue == (q)) 
#define SNAPSHOT_IS_IN_NO_Q(sab) \
  SNAPSHOT_IS_IN_THIS_QUEUE((sab), SNAPSHOT_NO_Q)
#define SNAPSHOT_IS_IN_ACTIVE_Q(sab) \
  SNAPSHOT_IS_IN_THIS_QUEUE((sab), SNAPSHOT_ACTIVE_Q)
#define SNAPSHOT_IS_IN_CLIENT_QUIET_Q(sab) \
  SNAPSHOT_IS_IN_THIS_QUEUE((sab), SNAPSHOT_CLIENT_QUIET_Q)
#define SNAPSHOT_IS_IN_SERVER_POST_ACTIVE_Q(sab) \
  SNAPSHOT_IS_IN_THIS_QUEUE((sab), SNAPSHOT_SERVER_POST_ACTIVE_Q)
#define SNAPSHOT_IS_IN_CLIENT_POST_ACTIVE_NEXT_QUIET_Q(sab) \
  SNAPSHOT_IS_IN_THIS_QUEUE((sab), SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q)
#define SNAPSHOT_IS_IN_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q(sab) \
  SNAPSHOT_IS_IN_THIS_QUEUE((sab), SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q)

/*
 * Support for multiple activity blocks for client (if multiple dialer maps)
 * or server (one for each protocol address)
 * Supported as a queue...
 */
#define SNAPSHOT_MAX_DIALER_CONNECTED_HWIDBS (100)
#define SNAPSHOT_ACTIVITY_BLOCK_DATA_INTEGRITY (0x42696f6e)
typedef struct _snapshot_activity_block {
    struct _snapshot_activity_block *next;
    struct _snapshot_activity_block *queue_prev;
    struct _snapshot_activity_block *queue_next;
    struct snapshot_interface_info_ *sii;
    ulong data_integrity_check;
    int index;
    int runtime_queue;
    int time_left_in_this_queue;
    int active_exchange_period;	/* Minutes updates were received for.. */
    BITMASK_DEFINITION(protocols_started_to_age, LINK_MAXLINKTYPE);
    uchar address;		/* Snapshot dialer 'address' -- 0 if none */
    int count_dialer_connected_hwidbs;
    hwidbtype *dialer_connected_hwidbs[SNAPSHOT_MAX_DIALER_CONNECTED_HWIDBS];
    addrtype protocol_address;	/* Protocol address used on servers */
} snapshot_activity_block;

#define SNAPSHOT_CLEAR_PROTOCOL_UPDATES_EXCHANGED(sab) \
  BITMASK_CLEAR_ALL((sab)->protocols_started_to_age, LINK_MAXLINKTYPE)
#define SNAPSHOT_CHECK_ANY_PROTOCOL_UPDATES_EXCHANGED(sab) \
  BITMASK_ANY_SET((sab)->protocols_started_to_age, LINK_MAXLINKTYPE)
#define SNAPSHOT_CHECK_PROTOCOL_UPDATES_EXCHANGED(sab, proto) \
  BITMASK_TEST((sab)->protocols_started_to_age, proto)
#define SNAPSHOT_SET_PROTOCOL_UPDATES_EXCHANGED(sab, proto) \
  BITMASK_SET((sab)->protocols_started_to_age, proto)

/*
 * The following is a per-hardware IDB structure which defines Snapshot
 * info for interfaces configured with them. Note that we keep track of our
 * OWN structure instead of attaching it off of the idb queue...
 */
enum SNAPSHOT_FLAGS {
    SNAPSHOT_FLAGS_SERVER = 0,
    SNAPSHOT_FLAGS_DIALER = 1,
    SNAPSHOT_FLAGS_SUPPRESS_STATECHANGE_UPDATES = 2,
    SNAPSHOT_FLAGS_IDB_UP = 3,
    SNAPSHOT_FLAGS_MAXFLAG = 4
  };

#define SNAPSHOT_INTERFACE_INFO_DATA_INTEGRITY (0x4a6f656c)
typedef struct snapshot_interface_info_ {
    struct snapshot_interface_info_ *next;
    ulong data_integrity_check;
    int index;
    idbtype *idb;
    int	active_time;
    int quiet_time;
    snapshot_activity_block *activity_blocks;
    int activity_index;
    BITMASK_DEFINITION(flags, SNAPSHOT_FLAGS_MAXFLAG);
} snapshot_interface_info;

#define SNAPSHOT_IS_SERVER(sii) \
  (BITMASK_TEST((sii)->flags, SNAPSHOT_FLAGS_SERVER))
#define SNAPSHOT_IS_CLIENT(sii) \
  (!SNAPSHOT_IS_SERVER((sii)))
#define SNAPSHOT_IS_DIALER(sii) \
  (BITMASK_TEST((sii)->flags, SNAPSHOT_FLAGS_DIALER))
#define SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii) \
  (BITMASK_TEST((sii)->flags, SNAPSHOT_FLAGS_SUPPRESS_STATECHANGE_UPDATES))
#define SNAPSHOT_IS_IDB_UP(sii) \
  (BITMASK_TEST((sii)->flags, SNAPSHOT_FLAGS_IDB_UP))
#define SNAPSHOT_CLEAR_FLAGS(sii) \
  BITMASK_CLEAR_ALL((sii)->flags, SNAPSHOT_FLAGS_MAXFLAG)
#define SNAPSHOT_CLEAR_FLAG(sii, bit) \
  BITMASK_CLEAR((sii)->flags, bit)
#define SNAPSHOT_SET_FLAG(sii, bit) \
  BITMASK_SET((sii)->flags, bit)
#define SNAPSHOT_SET_SERVER(sii) \
  SNAPSHOT_SET_FLAG((sii), SNAPSHOT_FLAGS_SERVER)
#define SNAPSHOT_SET_CLIENT(sii) \
  SNAPSHOT_CLEAR_FLAG((sii), SNAPSHOT_FLAGS_SERVER)
#define SNAPSHOT_SET_DIALER(sii) \
  SNAPSHOT_SET_FLAG((sii), SNAPSHOT_FLAGS_DIALER)
#define SNAPSHOT_SET_SUPPRESS_STATECHANGE_UPDATES(sii) \
  SNAPSHOT_SET_FLAG((sii), SNAPSHOT_FLAGS_SUPPRESS_STATECHANGE_UPDATES)
#define SNAPSHOT_SET_IDB_UP(sii) \
  SNAPSHOT_SET_FLAG((sii), SNAPSHOT_FLAGS_IDB_UP)
#define SNAPSHOT_SET_IDB_DOWN(sii) \
  SNAPSHOT_CLEAR_FLAG((sii), SNAPSHOT_FLAGS_IDB_UP)

/* Quiet period between active cycles */
#define SNAPSHOT_GUARANTEED_QUIET_MINUTES (2)

/*
 * Retry period formula. Accounts for horribly out of synch client
 * and server. A server 'hooks in' at the last second a client is
 * active. It stays active for ACTIVE period minutes, and then tries
 * to stay 'post active' for SNAPSHOT_GUARANTEED_QUIET_MINUTES. Then
 * there is one minute of 'synch' error possible. This leads to a
 * retry interval that must be at least period of time to let the
 * client 'clear'. This is also the minimum quiet time
 */
#define SNAPSHOT_RETRY_INTERVAL(ap) \
    ((ap) + SNAPSHOT_GUARANTEED_QUIET_MINUTES + 1)

/*
 * Minimum time required to allow updates to be exchanged for a
 * 'successfull update cycle'
 */
#define SNAPSHOT_SUCCESSFULL_EXCHANGE_MINUTES(sii) \
    (MAX(((sii)->active_time / 2), 3))
 
/* Extra time to allow the dialer interfaces to synch up to each other */
#define SNAPSHOT_DIALER_EXTRA_BREATHING_ROOM_MINUTES (3)

extern queuetype snapshot_interfaces;

/*
 * One per hardware dialer interface that is currently active and
 * needing to be 'kept up' for Dialer purposes.
 */
typedef struct snapshot_active_dialer_hardware_block_ {
    struct snapshot_active_dialer_hardware_block_ *next;
    struct snapshot_active_dialer_hardware_block_ *prev;
    hwidbtype *hwidb;
    int dialer_startup_remaining;
} snapshot_active_dialer_hardware_block;
 
/* Various looping mechanisms */
#define SNAPSHOT_FOR_ALL_SII(x) \
 for ((x) = (snapshot_interface_info *) snapshot_interfaces.qhead; \
      (x) != NULL; (x) = (x)->next)

#define SNAPSHOT_FOR_ALL_SAB_ON_SII(x, y) \
 for ((x) = (y)->activity_blocks; (x) != NULL; (x) = (x)->next)

#define SNAPSHOT_FOR_ALL_SAB_ON_SII_SAVE_NEXT(x, y, n) \
 for ((x) = (y)->activity_blocks, \
      (n) = ((x) == NULL) ? NULL : (x)->next; (x) != NULL; \
      (x) = (n), (n) = (((x) == NULL) ? NULL : (x)->next))

#define SNAPSHOT_FOR_ALL_SAB_IN_QUEUE(x, q) \
 for ((x) = snapshot_run_queues[(q)]; (x) != NULL; (x) = (x)->queue_next)
    
#define SNAPSHOT_FOR_ALL_SAB_IN_QUEUE_SAVE_NEXT(x, q, n) \
 for ((x) = snapshot_run_queues[(q)], \
      (n) = ((x) == NULL) ? NULL : (x)->queue_next; (x) != NULL; \
      (x) = (n), (n) = ((x) == NULL ? NULL : (x)->queue_next))

#define SNAPSHOT_FOR_ALL_SADHB(x) \
  for ((x) = snapshot_active_dialer_hwidbs; (x) != NULL; (x) = (x)->next)

#define SNAPSHOT_FOR_ALL_SADHB_SAVE_NEXT(x, n) \
 for ((x) = snapshot_active_dialer_hwidbs, \
      (n) = ((x) == NULL) ? NULL : (x)->next; (x) != NULL; \
      (x) = (n), (n) = (((x) == NULL) ? NULL : (x)->next))
  

#endif
