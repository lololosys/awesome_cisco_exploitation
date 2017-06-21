/* $Id: lsv_private.h,v 3.3.58.2 1996/08/12 20:05:25 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/lane/lsv_private.h,v $
 *------------------------------------------------------------------
 * lsv_private.h: LAN Emulation Service definitions
 *
 * December 1994, Norm Finn
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lsv_private.h,v $
 * Revision 3.3.58.2  1996/08/12  20:05:25  cyoung
 * Add support for more than one LES per ELAN.
 * CSCdi62514:  LES for same ELAN on different physical interfaces missing
 * from MIB
 * Branch: California_branch
 *
 * Revision 3.3.58.1  1996/05/09  14:33:25  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.86.1  1996/04/27  07:07:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.76.4  1996/04/16  02:45:21  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.76.3  1996/04/09  02:17:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.3.76.2  1996/04/08  14:52:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.76.1  1996/04/08  01:57:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.64.1  1996/03/22  22:55:43  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.48.3  1996/03/08  01:10:35  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * Fix router/Synergy conflicts.
 *
 * Revision 3.3.48.2  1996/03/05  06:43:47  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.48.1  1996/03/03  21:23:44  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.36.1  1996/02/27  21:11:04  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.14.5  1996/02/22  21:08:34  cakyol
 * new scheduling paradigm expects a process to watch only
 * on a single managed timer.  So, convert multiple timers
 * to work under one master timer.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.4  1996/02/22  09:05:46  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.3  1996/02/06  22:19:48  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.2  1996/01/26  02:02:58  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.1.2.1  1996/03/23  21:14:14  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 * 
 * Revision 3.3.14.1  1996/01/11  01:47:00  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:07:14  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:51:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2.40.1  1996/02/03  07:07:22  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.2.22.1  1995/10/16  23:45:57  sakumar
 * Added support for Topology Change and LES, BUS, LECS implemented
 * on the Catalyst 5000 ATM Module with RSAR->TSAR firmware
 * implementation of BUS.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.2  1995/07/25  21:54:01  nfinn
 * CSCdi37659:  LANE server hangs waiting to get LECS addr from switch
 *
 * Revision 2.1  1995/06/07  21:20:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LSV_PRIVATE_H_
#define _LSV_PRIVATE_H_

#include "generic_objects.h"
#include "lecs_finder.h"

/* forward typedefs */
 
typedef struct lsv_info_t_ lsv_info_t;
typedef struct lsv_reg_table_t_ lsv_reg_table_t;
typedef struct clientValidation_t_ ClientValidation_t;
 

/* externs */

/* LES/BUS process past NVRAM processing state */
extern boolean lsv_running;

/* chain of all LESs */
extern lsv_info_t *lsv_all_lesbus;

/* transaction ID for ILMI address reg/dereg */
extern ulong lane_ilmi_transaction_id;

/* root for LES/BUS managed timers */
extern mgd_timer lsv_root_timer;

/* service constants */

#define	S4_MINIMUM		150	/* LES Control Time-out */
#define S4_DEFAULT		150
#define S4_MAXIMUM		  0
#define S5_MINIMUM		  1	/* Maximum BUS Frame Age */
#define S5_DEFAULT		  1
#define S5_MAXIMUM		  4
#define LSV_PROC_START_TIME	  3	/* delay on startup */
#define LSV_RECYCLE_TIME	30	/* recycle time */

/* LES/BUS timer types */

#define	LSV_TIMER_RECYCLE	1	/* recycle timer */
#define LSV_TIMER_CLIENT	2	/* client timer */
#define LSV_TIMER_LECS_ACCESS	3	/* LECS reconnect time */
#define LSV_LEC_VALIDATION	4	/* LEC validation timer */

/* LES/BUS status */

typedef enum lsv_server_state_t_ 
{
    LSV_LESBUS_INITIALIZED,	/* control blocks done */
    LSV_LESBUS_WAITING_ILMI,	/* waiting for ILMI prefix */
    LSV_LESBUS_WAITING_LISTEN,	/* setting up listens */
    LSV_LESBUS_PROVISIONAL,	/* Up, but NSAP not registered */
    LSV_LESBUS_OPERATIONAL,	/* properly up */
    LSV_LESBUS_TERMINATING	/* on the way down */

} lsv_server_state_t;

/* LES/BUS administrative status */

typedef enum lsv_server_admin_stat_t_ 
{
    LSV_LESBUS_ADMIN_UP,
    LSV_LESBUS_ADMIN_DOWN,
    LSV_LESBUS_ADMIN_DESTROY

} lsv_server_admin_stat_t;

/* LES Interface Statistics */

typedef struct lsv_stats_t_ 
{
    ulong			lsvInJoinReqs;
    ulong			lsvOutJoinResps;
    ulong			lsvOutJoinFails;
    ulong			lsvLastJoinFailCause;
    hwaddrtype			lsvLastJoinFailLec;
    ulong			lsvOutConfigReqs;
    ulong			lsvInConfigResps;
    ulong			lsvInConfigFails;
    ulong			lsvInRegReqs;
    ulong			lsvOutRegResps;
    ulong			lsvOutRegFails;
    ulong			lsvLastRegFailCause;
    hwaddrtype			lsvLastRegFailLec;
    ulong			lsvInUnregReqs;
    ulong			lsvOutUnregResps;
    ulong			lsvOutUnregFails;
    ulong			lsvInArpReqs;
    ulong                       lsvFastFwdArpReqs;
    ulong                       lsvFastFwdArpResps;
    ulong			lsvOutArpResps;
    ulong			lsvInFlushReqs;
    ulong			lsvOutFlushResps;
    ulong			lsvInTopReqs;
    ulong			lsvOutTopReqs;
    ulong			lsvInNarpReqs;
    ulong			lsvOutNarpReqs;
    ulong                       lsvFastFwdFlushResps;
    ulong			lsvUnknownFrames;
    ulong                       lsvFastUnknownFrames;
    ulong			lsvOutOtherFails;

} lsv_stats_t;

#define LSV_STATS_EMPTY (lsv_stats_t) {0}

/* BUS Interface Statistics */

typedef struct lsv_bus_stats_t_ 
{
    ulong			busInFrames;
    ulong			busOutFrames;
    ulong			busOutDiscards;
    ulong                       busUnicastForwards;
    ulong                       busBytes;

} lsv_bus_stats_t;

#define LSV_BUS_STATS_EMPTY (lsv_bus_stats_t) {0}

/* state of address registration with ATM switch */

typedef enum lane_addr_reg_state_t_ 
{
    LANE_ADREG_INITIALIZED,
    LANE_ADREG_REGISTERING,
    LANE_ADREG_REGISTERED,
    LANE_ADREG_UNREGISTERING

} lane_addr_reg_state_t;

/*
** data structure that maintains all the LECS access
** stuff for verifying a client from an LES.
*/
typedef struct lecsAccess_t
{
    void *callId;
    boolean shouldBeConnected;
    boolean connected;
    ListPtr validationRequests;
    mgd_timer lecsReconnectTimer;

} LecsAccess;

/*
** how often to try and connect to the master
** LECS, if I have not yet been successfull.
*/
#define LECS_RECONNECT_TIME	3

/*
 * LES/BUS info struct
 *
 * This hangs off of the lane_info struct 
 * which ties all LANE components to the swidb.
 */

struct lsv_info_t_ 
{
    lsv_info_t			*next;
    lane_info_t			*lane_info;
    lsv_server_admin_stat_t	 admin_state;
    lsv_server_state_t		 server_state;
    ulong			 S5_max_frame_age;
    ulong			 S4_control_timeout;
    hwaddrtype			 lsv_lecs_nsap;
    hwaddrtype			 lsv_nsap;
    hwaddrtype			 bus_nsap;
    lsv_client_t		*client_table;
    IndexPtr			 clientList;
    lsv_client_t		*beingValidated;
    lsv_reg_table_t		*reg_table;
    sys_timestamp		 start_time;
    boolean			 dont_recurse;
    lane_addr_reg_state_t	 lsv_reg_state;
    lane_addr_reg_state_t	 bus_reg_state;
    ulong			 lsv_reg_transid;
    ulong			 bus_reg_transid;
    lane_signal_mcgroup_t	 distribute_vcc;
    lane_signal_mcgroup_t	 forward_vcc;
    lane_vcc_info_t		 lsv_listen_vcc;
    lane_vcc_info_t		 bus_listen_vcc;
    LecsAccess			 lecsAccess;
    lsv_stats_t			 lsvs;
    lsv_bus_stats_t		 buss;
    mgd_timer			 timer_node;
    mgd_timer			 recycle_timer;

    /* add for TR LANE */
    ushort			seg_id;

    /* added for MIB support */
    ulong                       lesIndex;

};

/* Macro for empty lsv_info_t */

#define LSV_INFO_EMPTY (lsv_info_t) { \
    NULL, NULL, LSV_LESBUS_ADMIN_DOWN, LSV_LESBUS_INITIALIZED, \
    S5_DEFAULT, S4_DEFAULT, LANE_NO_NSAP, \
    LANE_EMPTY_NSAP, LANE_EMPTY_NSAP, \
    NULL, NULL, NULL, NULL, timer_no_time, FALSE, \
    LANE_ADREG_INITIALIZED,  LANE_ADREG_INITIALIZED, 0, 0, \
    LANE_MCGROUP_EMPTY, LANE_MCGROUP_EMPTY, \
    LANE_VCC_INFO_EMPTY, LANE_VCC_INFO_EMPTY, \
    { NULL, FALSE, FALSE, NULL, {} }, \
    LSV_STATS_EMPTY, LSV_BUS_STATS_EMPTY, {}, {}, 0, 0}

/* LES Client states */

typedef enum lsv_client_state_t_ 
{
    LSV_CLIENT_INITIALIZED,	/* control block created only */
    LSV_CLIENT_CONNECTED,	/* control direct connected */
    LSV_CLIENT_JOIN_RECEIVED,	/* join req received */
    LSV_CLIENT_ADD_SENT,	/* control distribute setup sent */
    LSV_CLIENT_JOINED,		/* control distribute done */
    LSV_CLIENT_OPERATIONAL,	/* BUS connections complete */
    LSV_CLIENT_REJECTED,	/* Join failed, awaiting LEC exit */
    LSV_CLIENT_TERMINATING	/* termination in progress */

} lsv_client_state_t;

#define LSV_CLIENT_STATE_NAMES {"init", "conn", "join_rcv", \
	"add_sent", "joined", "oper", "reject", "term"}

/* LES address registration table */

struct lsv_reg_table_t_ 
{
    lsv_reg_table_t		*next;
    lsv_client_t		*client;
    hwaddrtype			 nsap;
    lane_dest_t			 lan_dest;
    boolean			 manual_entry:1;

};


/* LES Client table */

struct lsv_client_t_ 
{
    lsv_client_t		*next;
    lsv_info_t			*lsv_info;
    ushort			 lecid;
    boolean			 is_proxy;
    boolean			 dont_recurse;
    lane_join_pdu_t		 first_join;
    ClientValidation_t		*validation;
    lane_vcc_info_t		 direct_vcc;
    lane_vcc_info_t		 distribute_vcc;
    lane_vcc_info_t		 send_vcc;
    lane_vcc_info_t		 forward_vcc;
    lsv_client_state_t		 client_state;
    mgd_timer			 S4_timer;

};



#define LSV_CLIENT_EMPTY (lsv_client_t){ \
    NULL, NULL, 0, FALSE, FALSE, {}, \
    NULL, \
    LANE_VCC_INFO_EMPTY, LANE_VCC_INFO_EMPTY, \
    LANE_VCC_INFO_EMPTY, LANE_VCC_INFO_EMPTY, \
    LSV_CLIENT_INITIALIZED, {}}


/*********************************************************
**********************************************************
** 		LES to LECS validation subsystem
**********************************************************
*********************************************************/

/*
** this record represents the transaction information
** of the validation request made by the LES to the LECS
** on behalf of a client who wants to perform a join.
*/
struct clientValidation_t_
{
    ListEntry links;
    lsv_info_t *lsv_info;
    lsv_client_t *client;
    mgd_timer timer;

};

/*********************************************************
**********************************************************
**	    External variables and functions
**********************************************************
*********************************************************/

extern boolean	valid_lesbus(lsv_info_t*);
extern void	lsv_initValidationSubsystem(void);
extern void	cleanupLecsAccess(lsv_info_t *lsv_info, 
		    boolean disconnect,
		    boolean discard,
		    boolean destroyList);
extern void	lsv_connectToMasterLecs(lsv_info_t *lsv_info);
extern void	lsv_start_lecs_connection_procedures(lsv_info_t *lsv_info);
extern void	lsv_client_validation_timeout(ClientValidation_t *cv);
extern void	lsv_process_config_resp(paktype*, lsv_info_t*);
extern void	lsv_answer_request(lsv_client_t *, paktype *,
		    ulong status);
extern void	lsv_big_error(lsv_info_t*, int seconds_to_recycle);
extern boolean	lsv_check_lan_dest(lane_dest_t *);
extern void	lsv_client_timed_out(lsv_client_t *);
extern void	lsv_crash(lsv_info_t *, char *msg);
extern lsv_client_t *lsv_find_client_primary(lsv_info_t *, 
		    hwaddrtype *);
extern paktype *lsv_getbuffer(lsv_info_t *, int size);
extern lsv_info_t *lsv_init_idb(idbtype *, int elan_type);
extern void	lsv_kill_client(lsv_info_t *, lsv_client_t *);
extern void	lsv_kill_lesbus(lsv_info_t *);
extern boolean	lsv_lesbus_alive(lsv_info_t *);
extern void	lsv_pack_nsap(uchar [], hwaddrtype *);
extern boolean	lsv_primary_nsap_conflict(lsv_info_t *, 
		    lsv_client_t *, hwaddrtype *);
extern void	lsv_process_join(paktype *, lsv_info_t *);
extern void	lsv_process_recycle(lsv_info_t *);
extern void	lsv_register_nsap(lsv_info_t *, hwaddrtype *,
		    lane_addr_reg_state_t *, ulong *transid);
extern boolean	lsv_unregister_nsap(lsv_info_t *, hwaddrtype *,
		    lane_addr_reg_state_t *, ulong *transid);
extern ushort	lsv_register_lan_dest_nsap(lsv_info_t *, lsv_client_t *,
		    lane_dest_t *, hwaddrtype *, boolean);
extern void	lsv_server_new_state(lsv_info_t*, 
		    lsv_server_state_t);
extern lane_vcc_info_t *lsv_sigback_listen_direct(lane_vcc_info_t*,
		    lane_callback_code_t);
extern lane_vcc_info_t *lsv_sigback_listen_send(lane_vcc_info_t*,
		    lane_callback_code_t);
extern void	lsv_start(void);
extern boolean	lsv_start_lesbus(lsv_info_t *);
extern void	lsv_start_pended_actions(void);
extern void	lsv_unregister_all_dest_nsap(lsv_info_t*, 
		    lsv_client_t*);
extern char *   lsv_pr_server_state(lsv_client_state_t);

#endif _LSV_PRIVATE_H_

