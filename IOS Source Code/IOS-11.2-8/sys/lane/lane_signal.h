/* $Id: lane_signal.h,v 3.3.32.5 1996/07/21 00:56:37 cakyol Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_signal.h,v $
 *------------------------------------------------------------------
 * lane_signal.h: LAN Emulation ATM signalling defitions
 *
 * December 1995, Norm Finn
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_signal.h,v $
 * Revision 3.3.32.5  1996/07/21  00:56:37  cakyol
 * CSCdi63405:  LANE LES: lec gets stuck in state T
 * Branch: California_branch
 *
 * Revision 3.3.32.4  1996/06/26  08:49:36  cakyol
 * CSCdi61253:  pvc cleanup in LANE left some stuff around (lane_call
 * (...))
 * Branch: California_branch
 *
 * Revision 3.3.32.3  1996/05/31  01:08:34  shj
 * CSCdi46584:  LANE signaling needs to be smarter about ADD_PARTY
 * Branch: California_branch
 * Queue & retry ADD_PARTY's that bounce with "too many ADD_PARTY's.
 *
 * CSCdi55191: LANE can leave <stranded> p2mp svc - does DROP instead of
 * RELEASE
 * Add count of outstanding DROP's, so we can tell for sure when a leaf is
 * the last one.
 *
 * CSCdi55975: LANE: %LANE-3-LANE_SIG_INV_MSG: invalid msg received
 * (3 BD3FA4 8)
 * Make sure we zero out vcd of p2mp vc when last leaf is gone, so we
 * don't later think we already have an established vc.
 *
 * Revision 3.3.32.2  1996/05/09  14:32:38  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.32.1.2.1  1996/04/27  07:06:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.32.1  1996/04/25  23:14:34  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.52.2  1996/04/16  02:44:28  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.52.1  1996/04/08  14:51:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.50.1  1996/04/08  08:06:16  ddecapit
 * Sync to LE_Syn_Cal_V111_1_3_branch
 * Branch: TRlane_LE_Syn_1_3_branch
 *
 * Revision 3.3.48.1  1996/04/06  21:19:48  ddecapit
 * Sync to LE_Cal_V111_1_0_3_branch
 * Branch: TRlane_branch_1_0_3
 *
 * Revision 3.3  1996/01/12  23:54:08  mdb
 * CSCdi46841:  sloppy initialization macros defined
 *
 * Revision 3.2.16.2  1996/03/23  21:12:47  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.2.16.1  1996/03/05  04:47:00  ddecapit
 * Branch: TRlane_branch
 * Token Ring B-LLI for LAN Emulation
 * Fix copyright
 *
 * Revision 3.2  1995/11/17  17:50:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/19  21:57:21  shj
 * CSCdi37300:  LANE signaling debug msgs need more & better info
 *
 * Add interface to messages, show called/calling atm-addresses on
 * SETUP & ADD_PARTY, show sent & recvd blli's.
 *
 * Revision 2.2  1995/07/07  01:37:54  shj
 * CSCdi36794:  LANE confusion caused by multiple pending signaling actions
 *
 * Handle RELEASE/DROP_PARTY/CANCEL while previous SETUP/ADD_PARTY/LISTEN
 * still in progress.
 *
 * Revision 2.1  1995/06/07  21:19:46  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LANE_SIGNAL_H_
#define _LANE_SIGNAL_H_

/* Macros for empty ATM NSAP hwaddrtype */

#define LANE_NO_NSAP (hwaddrtype){\
	type:STATION_ILLEGAL, length:0, target_type:TARGET_UNICAST}
#define LANE_EMPTY_NSAP (hwaddrtype){\
	type:STATION_ATMNSAP, length:STATIONLEN_ATMNSAP,\
	target_type:TARGET_UNICAST}
#define LANE_ALL_ONES_ADDR (uchar[]){ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
	0xff, 0xff, 0xff}
#define LANE_ALL_ONES_NSAP (hwaddrtype){\
	type:STATION_ATMNSAP, length:STATIONLEN_ATMNSAP,\
	target_type:TARGET_UNICAST, addr:LANE_ALL_ONES_ADDR}
#define LANE_LECS_FIXED_NSAP (hwaddrtype){\
	type:STATION_ATMNSAP, length:STATIONLEN_ATMNSAP,\
	target_type:TARGET_UNICAST, addr:\
    {0x47, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
	   0x00, 0x00, 0x00, 0x00, \
    0x00, 0xa0, 0x3e, 0x00, 0x00, 0x01, \
    0x00} }


/*
 * Codes for LAN Emulation B-LLI types
 */

typedef enum lane_blli_t_ {
    LANE_BLLI_UNDEFINED,
    LANE_BLLI_CONTROL,
    LANE_BLLI_ETHER_DATA,
    LANE_BLLI_TR_DATA,
    LANE_BLLI_ETHER_MULTI,
    LANE_BLLI_TR_MULTI
} lane_blli_t;

/*
 * Assume all the valid LANE pid's are sequential.
 */
#define LANE_BLLI_MIN_PID       LANE_BLLI_CONTROL
#define LANE_BLLI_MAX_PID       LANE_BLLI_TR_MULTI


/*
 * Codes for reason for callback
 */

typedef enum lane_callback_code_t_ {
    LANE_CALL_CALL_COMPLETE,
    LANE_CALL_CALL_FAILED,
    LANE_CALL_ADD_COMPLETE,
    LANE_CALL_ADD_FAILED,
    LANE_CALL_RELEASE_COMPLETE,
    LANE_CALL_RELEASE_RECEIVED,
    LANE_CALL_DROP_COMPLETE,
    LANE_CALL_DROP_FAILED,
    LANE_CALL_SETUP_RECEIVED,
    LANE_CALL_LISTEN_COMPLETE,
    LANE_CALL_LISTEN_FAILED,
    LANE_CALL_CANCEL_COMPLETE
} lane_callback_code_t;

/*
 * Codes for state of VCC
 */

typedef enum lane_vcc_state_t_ {
    LANE_VCC_INITIALIZED,
    LANE_VCC_LISTEN_SENT,
    LANE_VCC_LISTENING,
    LANE_VCC_CANCEL_SENT,
    LANE_VCC_SETUP_SENT,
    LANE_VCC_SETUP_RECEIVED,
    LANE_VCC_CONNECT_SENT,
    LANE_VCC_CONNECTED,
    LANE_VCC_ADD_SENT,
    LANE_VCC_DROPPING,
    LANE_VCC_RELEASING,
    LANE_VCC_NOT_FOUND
} lane_vcc_state_t;

/*
 * Return codes from user-called functions
 */

typedef enum lane_signal_return_t_ {
    LANE_SIG_SUCCESS,		/* It worked.  All done. */
    LANE_SIG_FAILURE,		/* It failed.  All done. */
    LANE_SIG_CALLBACK		/* Will callback when I know what happened. */
} lane_signal_return_t;


/*
 * Structure to hold info from "lane register" command.  Chained
 * to sig_pvc structure.
 */ 

typedef struct lane_sig_register_t_ {
    struct lane_sig_register_t_ *next;
    lane_dest_t                 lan_dest;       /* mac */
    hwaddrtype                  atm_addr;       /* atm address */
} lane_sig_register_t;


/*
 * Structure identifying one connection or one leaf
 */

typedef struct lane_vcc_info_t_ {
/* Who can write it?  S = Signaling side, U = user side, M = SNMP side */
    struct lane_vcc_info_t_	*next_m;	/* M */
    lane_sig_register_t         *reg_table;     /* S */
    struct lane_vcc_info_t_	*next_s;        /* S */
    struct lane_vcc_info_t_     *next_mc;       /* S */
    struct lane_signal_mcgroup_t_ *mc_head;     /* S */

    lec_id_t			 lecid;		/* S */
    boolean                      in_sig_chain:1;   /* S */
    boolean                      is_multicast:1;   /* S */
    boolean                      no_svc_timeout:1; /* U */
    idbtype			*swidb;         /* U */
    hwaddrtype			 calling_addr;	/* SU */
    hwaddrtype			 called_addr;	/* SU */
    boolean		         svc;	        /* SU */
    lane_vcc_state_t		 vcc_state;	/* S */
    struct lane_vcc_info_t_	*(*callback)(struct lane_vcc_info_t_ *,
					     lane_callback_code_t);	/* U */
    lane_vc_type_t		 vc_type;	/* U */
    void			*parm;		/* U For user's use */
    vcd_t			 vcd;		/* S */
    void			*call_id;	/* S */
						/* U More to come for user */
						/* M More to come for SNMP */
						/* S More to come for signals */
} lane_vcc_info_t;

/* Macro for empty lane_vcc_info_t */

#define LANE_VCC_INFO_EMPTY (lane_vcc_info_t){ \
    NULL, NULL, NULL, NULL, NULL, \
    0, FALSE, FALSE, FALSE, NULL, LANE_EMPTY_NSAP, LANE_NO_NSAP, \
    FALSE, LANE_VCC_INITIALIZED, NULL, \
    LANE_LEC_DIRECT, NULL, 0, NULL}

/*
 * Structure defining a multicast group (used only at root end)
 */

typedef struct lane_signal_mcgroup_t_ {
    hwaddrtype			 calling_addr;	/* U */
    struct lane_vcc_info_t_	*root_chain;	/* S chain of customers */
    lane_info_t                 *lane_info;     /* S */
    lane_vcc_state_t		 mcgroup_state;	/* US */
    lane_vc_type_t		 vc_type;	/* U */
    boolean		 	 svc;		/* S */
    vcd_t			 vcd;		/* S */
    int                          n_members;     /* S */
    int                          adds_out;      /* S */
    int                          drops_out;     /* S */
    void                        *call_id;       /* S */
} lane_signal_mcgroup_t;

/* Macro for empty lane_signal_mcgroup_t */

#define LANE_MCGROUP_EMPTY (lane_signal_mcgroup_t){ \
    LANE_EMPTY_NSAP, NULL, NULL, LANE_VCC_INITIALIZED, \
    LANE_LES_DISTRIBUTE, FALSE, 0, 0, 0, 0, NULL}

/* macros for chaining through lane_vcc_info's */

#define FOR_ALL_LANE_SIG_VCCS(lv, info) \
  for ((lv) = (info)->signal_chain; (lv); (lv) = (lv)->next_s)

#define FOR_ALL_LANE_MCGROUP_VCCS(lv, mcg) \
  for ((lv) = (mcg)->root_chain; (lv); (lv) = (lv)->next_mc)

#define FOR_ALL_LANE_SIG_REGS(r, p) \
  for ((r) = (p)->reg; (r); (r) = (r)->next)


/*
 * Extern function definitions.
 *
 * User always creates lane_vcc_info_t and lane_signal_mcgroup_t with
 * vcc_state == LANE_VCC_INITIALIZED.  User always deletes these
 * structures, but may do so only when vcc_state == LANE_VCC_INITIALIZED.
 */

extern lane_signal_return_t	  lane_no_listen(lane_vcc_info_t *);
extern lane_signal_return_t	     lane_listen(lane_vcc_info_t *);
extern lane_signal_return_t	    lane_release(lane_vcc_info_t *);
extern lane_signal_return_t  lane_create_mcgroup(lane_signal_mcgroup_t *,
                                                 lane_info_t *);
extern lane_signal_return_t  lane_delete_mcgroup(lane_signal_mcgroup_t *);
extern lane_signal_return_t		lane_add(lane_signal_mcgroup_t *,
						 lane_vcc_info_t *);
extern lane_signal_return_t	       lane_drop(lane_signal_mcgroup_t *,
						 lane_vcc_info_t *);

extern void    lane_sig_make_blli(blli_type *, lane_vc_type_t);
extern boolean lane_sig_blli_match(int, blli_type *, lane_vc_type_t);

#endif _LANE_SIGNAL_H_
