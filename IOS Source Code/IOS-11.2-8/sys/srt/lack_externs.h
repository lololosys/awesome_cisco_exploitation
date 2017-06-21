/* $Id: lack_externs.h,v 3.2.62.3 1996/07/19 22:46:38 snyder Exp $
 * $Source: /release/112/cvs/Xsys/srt/lack_externs.h,v $
 *------------------------------------------------------------------
 * lack_externs.h -- Local Acknowledgement Externals
 *
 * March 4, 1993, Claude Cartee and Eric B. Decker
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lack_externs.h,v $
 * Revision 3.2.62.3  1996/07/19  22:46:38  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2.62.2  1996/07/18  04:26:06  ppearce
 * CSCdi56398:  Router (sdlc-secondary) fails to respond to SNRM inputs
 * Branch: California_branch
 *   Ensure SDLLC process is re-started when SDLLC is configured
 *
 * Revision 3.2.62.1  1996/05/17  12:12:53  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/04/11  14:38:15  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  18:54:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/08  16:49:17  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.1  1995/06/07 23:00:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */


#ifndef __LACK_EXTERNS_H__
#define __LACK_EXTERNS_H__


#include "../util/fsm.h"


#define LAK2_MAX_FSM_LEVELS 3

/*
 * DLC definitions
 */

typedef enum {
    DLC_NONE = 0,
    DLC_SDLC,
    DLC_LLC2,
    DLC_QLLC,
    DLC_CLS,
    DLC_T_MAX
} dlc_t;


typedef struct {
    dlc_t type;
    union {
	void		*control_block;
	sdlc_data_block *sdb;
	llctype 	*llc;
	lnxtype		*lnx;
    } ptr;
} dlc_db_t;


/*
 * Return codes from dlc vectors
 */

typedef enum {
    DLC_RS_FAILED = 0,			/* to preserve llc2_close symantic */
    DLC_RS_OK = 1,			/* ditto */
    DLC_RS_PENDING
} dlc_rtn_t;


/*
 * DLC functions when presented with no object behave as
 * follows:
 *
 * primative	object exists
 *
 * 		(yes)		(no)
 * open		under construction Pending/Ok
 *
 * close	Pending/Ok	Fail
 *
 * abort	Ok		Ok
 *
 * busy		Ok		Fail
 */

struct dlc_handle_t_ {
    dlc_t type;
    dlc_rtn_t (*dlc_open)(dlc_db_t *, lak_t *);
    dlc_rtn_t (*dlc_close)(dlc_db_t*, lak_t *);
    dlc_rtn_t (*dlc_busy)(dlc_db_t *, boolean);
    dlc_rtn_t (*dlc_abort)(dlc_db_t *);
} ;

    

typedef struct {
    int		len;			/* rif length */
    uchar	data[SRB_MAXRIF];	/* actual rifs */
} lak_rif_t;


/*
 * return values from lack_passthru
 */

typedef enum {
    LAK2_TERMINATE	= 0,		/* packet should be lacked */
    LAK2_PASSTHRU	= 1,		/* sended it on */
    LAK2_XID		= 2		/* special processing */
} lak_pass_t;



/*
 * lack opcode classification
 */

typedef enum {
    LAK_OP_NONE		= 0,
    LAK_OP_NOT_LAK	= 1,
    LAK_OP_VERS2	= 2,		/* Vers 2 opcodes */
    LAK_OP_VERS3	= 3,		/* Vers 3 opcodes */
} lak_type_t;

#define LACK_HASH_SIZE           256
#define LACK_HASH_MASK           (LACK_HASH_SIZE-1)
#define LACK_MAX_PASSTHRU_RINGS  100 
#define LACK_HASHQ_SIZE          200

/*
 * lack entity structure.
 *
 *   *next
 *     Lack entries are chained together in hash buckets.  This is the pointer
 *     to the next one.
 *
 *   *llc;
 *     A pointer to a llc2 connection block.
 *
 *   *vrp
 *     A pointer to the associated remote peer structure associated with our
 *     remote end.
 *
 *   local_busy;
 *     TRUE if we be quenched.  Flow controlled.  Send RNRs locally.
 *
 *   timer_start;
 *   *timer_queue_next;
 *     Two cells used in timer operations.  This contains the time we will
 *     next trip or something like that.  A linked list of elements are
 *     maintained for lack entities that have a timer running.
 *
 *   llc2_open_ok;
 *   *llc2_open_pak;
 *     The above two cells are used in the delayed open that we have to do if
 *     lack is causing LLC2 to open up rather than the host did it.
 *
 *   lak2_state;
 *     The state this lack entity is in.  If non-zero denotes that this entity
 *     is being controlled by the new Lak2 FSM.  See lack2fsm.h for state
 *     definitions.
 *
 *   *piggie_pak;
 *     A packet that can be used by the Lak2 state machine to send messages
 *     over to the remote end.  It is a packet that has entered the system
 *     from the local side destined for the remote.
 *
 *   retry_count;
 *     A retry count used in conjuntion with the timer cells to implement
 *     multiple retries for critical protocol messages.
 *
 *   lack_state;
 *     The state cell used by the original lack code.  If this is a Lak2
 *     entity this will be set to LACK_LAK2.
 *
 *   address[14];
 *
 *     Gag.  The identifier for this lack entry.  It actually consists of the
 *     following fields:
 *
 * 	Local_Addr	address[0]	da, local host mac address
 * 	Rem_Addr	address[6]	sa, remote host mac address
 * 	local_sap	address[12]	dsap, local sap
 * 	rem_sap		address[13]	ssap, remote sap
 *
 *     If a packet were to be constructed as [da sa dsap ssap] and we put this
 *     packet out on the local ring it would be pointed at our local host.  In
 *     other words this is how we get to local host from here.  Note this is
 *     how the RIF entry is stored too.  This nonsense of address[14] will
 *     eventually get cleaned up and replaced with a structure.  Hard coded
 *     offsets into address is bull shit.
 */

struct lak_t_ {
    lak_t             *dummy_next;      /* for p_enqueue purpose */
    lak_t 	      *next;
    dlc_db_t	       dlc_db;		/* dlc instance */
    dlc_handle_t      *dlc_handle;	/* pointer to dlc calling vectors */
    const fsm_t	      *fsm_ptr;		/* which fsm to use */
    llctype           *llc;		/* should go away eventually */
    tr_vpeertype      *vrp;
    int		       local_ring;
    int		       remote_ring;
    boolean            local_busy;
    sys_timestamp      timer_start;	/* timer stuff */
    lak_t 	      *timer_queue_next; /* linked list of timer elements */
    boolean	       llc2_open_ok;
    paktype           *llc2_open_pak;
    int		       lak2_state;	/* lak2 fsm state */
    int		       lak2_fsm_level;	/* how many times the fsm has been called */
    paktype	      *piggie_pak;	/* lak2 piggie back packet */
    int		       retry_count;	/* lak2 ack retry count */
    uchar              address[14];	/* connection descriptor */

#define LACK_LOCAL_ADDR	address[0]
#define LACK_REM_ADDR	address[6]
#define LACK_LOCAL_SAP	address[12]
#define LACK_REM_SAP	address[13]

    lak_rif_t	       rif;		/* rif towards local host */
};

/* Per-cent of full queue to make RNR at (allow at least one entry in queue */
#define LACK_TCPQ_THRESHOLD 50

struct lak_addr_fields {
    uchar		locaddr[IEEEBYTES]; /* local or virtual device */
    uchar	 	remaddr[IEEEBYTES]; /* device on the peer */
    uchar		locsap;
    uchar		remsap;
} ;

typedef union lak_addr_t_ {
    uchar			lak_addr[14];	/* one big field */
    struct lak_addr_fields	lak_addr_fields;
} lak_addr_t;

#define LAK2_P(lackptr)	(!(lackptr) ? FALSE : ((lackptr)->lack_state == LACK_LAK2))

/*
 * Local Acknowledgement and Media Translation (SDLLC) external variables
 */
extern boolean    lack_running;
extern int        sdllc_pid;
extern int        sdllc_vr;
extern boolean    lack_debug_packet;
extern boolean	  lack_debug_state;
extern boolean	  lack_debug_err;
extern boolean    sdllc_debug;
extern ulong      lack_connect_timeout;
extern lak_t	 *lack_hash_freelist;
extern boolean    sdllc_lak_enable;
extern boolean    rsrb_cos_enable;
extern lak_t     *lack_timer_queue;
extern queuetype  lack_linkupQ;
extern queuetype  lack_inputQ;


extern	void	lak_forwardpak(uchar *da, uchar *sa, int dsap, int ssap, hwidbtype *idb,
				int type, boolean toggle_dbit);
extern	void	lack_data(paktype *pak, int length, tr_ventrytype *vre);
extern forktype lack_linkup_proc(void);

extern	void	lack_setstate(lak_t *lackie, int newstate);
extern	boolean	lack_find_passthru_ring(paktype *pak, tr_vgrouptype *vrg);
extern	lak_pass_t lack_passthru(tr_ventrytype *vre, tr_vpeertype *vrp, paktype *pak);
extern	boolean	lak2_local_term(paktype *pak, uchar *type, tr_vpeertype *vrp,
				int local_ring, int rem_ring);
extern	void	lack_enter_passthru_ring(int ring_num);
extern	void	lack_delete_passthru_ring(int ring_num);
extern	void	lack_sdllc_register(llctype *cookie);
extern	lak_t	*lack_create_entry(uchar *addr, tr_vpeertype *vrp, int local_ring, int rem_ring);
extern	boolean	lack_addr_in_use(uchar *addr);
extern	lak_t	*lack_by_machdr(uchar *addr);
extern	void	lack_cleanup_llc(llctype *llc);
extern	void	lack_fix_llc(llctype *llc, paktype **pak);
extern	void	lack_remove_hash(lak_t *target);
extern	int	calc_lack_hash(uchar *);
extern	char	*lack_prstate(lak_t *lackie, int state);
extern	char	*lack_prconch(uint);
extern	void	show_lack(void);
extern	void	lack_toss_connections(tr_vpeertype *vrp);

extern	forktype sdllc_proc(void);
boolean dlc_local_ack_this(paktype *pak, hwidbtype *idb, tr_ventrytype *vre);
extern	boolean sdllc_local_ack_this(paktype *, hwidbtype *, tr_ventrytype *);

/*
 * xid was originally defined as a packet "looking structure.
 * use packet for now.  re-visit later and re-implement after compile
 */
extern	void	sdllc_datagram_out(paktype *pak, uint type);	/* hampton patch */
extern	boolean	sdllc_remove_vring(hwidbtype *idb, int rsrb_et);
extern	void	sdllc_encaps(parseinfo *csb);
extern	void	sdllcif_command(parseinfo *csb);
extern	void	sdllc_net_ind_disconnect(hwidbtype *idb, sdlc_data_block *sdb);
extern	void	sdllc_net_ind_connect(hwidbtype *idb, sdlc_data_block *sdb);
extern	void	sdllc_cleanup_llc(llctype *llc);
extern	void	sdllc_fix_llc(llctype *llc, paktype **ppak);
extern	boolean	sdllc_fix_llc_action(llctype *llc, paktype *pak);
extern	void	sdllc_vrforward_data(paktype  *pak, hwidbtype *idb);
extern	hwidbtype *sdllc_new_vidb(hwidbtype *inidb, int thisring, int bridge_num, int targetring);
extern	boolean rsrb_sdllc_oqueue(hwidbtype *idb, paktype *pak, enum HEADTAIL dummy);
extern  boolean sdllc_get_sdb(paktype *pak, hwidbtype **sdlc_idb, sdlc_data_block **sdb);
extern   boolean sdllc_fix_llc_action(llctype *llc, paktype *pak);

#define	MAX_LACK_NAME	40
extern 	char	*lack_id(lak_t *lackie);

#define TRMAC2LAK_FROM_NET	1
#define	TRMAC2LAK_FROM_MEDIA	0
extern	lak_t	*trmac2lak(paktype *pak, uchar * entity, boolean fromnet);
extern	lak_t	*sdb2lak(hwidbtype *idb, sdlc_data_block *sdb);
extern  void    lack2_pak2addr(paktype *pak, uchar *entity, boolean fromnet);
extern  void lack2_rsdllc_initiate_rem_conn(paktype *pak, sdlc_data_block *sdb,      
			   tr_ventrytype *vre);
extern  lak_t   *lack2_build_reverse_entry(paktype *pak, tr_ventrytype *vre,
        void *control_block, dlc_t dlc_type);
extern  hwidbtype *dlc_new_vidb(hwidbtype *inidb, char *name,
                        int thisring, int bridgenum, int targetring);
extern  void remove_vre(tr_vgrouptype *vrg, tr_ventrytype *vre, int rsrb_et);

#endif __LACK_EXTERNS_H__
