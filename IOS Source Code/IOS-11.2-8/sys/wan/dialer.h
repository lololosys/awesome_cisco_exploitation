/* $Id: dialer.h,v 3.11.4.11 1996/09/10 07:32:36 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer.h,v $
 *------------------------------------------------------------------
 * dialer.h -- dialer definitions
 *
 * 23-Sept-91, William Miskovetz
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer.h,v $
 * Revision 3.11.4.11  1996/09/10  07:32:36  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix botched comment during previous commit of CSCdi62263
 *
 * Revision 3.11.4.10  1996/09/10  07:17:32  dblair
 * CSCdi62263:  Dialer profile with Multilink not working
 * Branch: California_branch
 * Fix multilink and dialer profiles
 *
 * Revision 3.11.4.9  1996/08/06  00:37:43  asb
 * CSCdi40543:  MBRI dialer rotary cannot detect down BRI
 * Branch: California_branch
 *
 * Revision 3.11.4.8  1996/08/04  08:09:34  fox
 * CSCdi62182:  %SCHED-3-PAGEZERO: Low memory modified by ISDN (0x74 =
 * 0x0).
 * Branch: California_branch
 * Convert ddb->dial_reason to an array of char from a char *.  Modify all
 * necessary references appropriately.
 *
 * Revision 3.11.4.7  1996/07/23  22:04:16  suresh
 * CSCdi62055:  Dialer profile error message misleading
 * Branch: California_branch
 * Code Cleanup for change from dialer user to dialer profile.
 *
 * Revision 3.11.4.6  1996/07/19  19:50:30  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly - do not allocate
 * holdq until needed, and pass the holdq correctly as calls and callbacks
 * are made.  Redo a little callback code to deal with the holdq better and
 * rename some of the structures.  Add a debug dialer holdq (hidden).
 * Branch: California_branch
 *
 * Revision 3.11.4.5  1996/07/14  06:34:02  dblair
 * CSCdi60572:  Router changes configuration on reload.
 * Branch: California_branch
 * Fix dialer load direction config
 *
 * Revision 3.11.4.4  1996/06/18  01:39:40  suresh
 * CSCdi46872:  Dialer load balancing/multilink create to groups when 2
 * numbers dial
 * Branch: California_branch
 *
 * Revamped the way dialer groups are created to do correct
 * dialer load balancin
 *
 * Revision 3.11.4.3  1996/05/10  01:44:42  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.11.4.2  1996/04/27  06:36:03  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.2.4  1996/04/25  23:58:10  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.2.3  1996/04/23  00:24:17  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.7.2.2  1996/02/23  21:31:19  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.2.1  1996/01/16  15:45:13  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 * Revision 3.11.4.1  1996/03/18  22:47:21  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.4.3  1996/03/16  07:55:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.8.4.2  1996/03/13  02:11:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.8.4.1  1996/02/20  21:50:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.11  1996/03/08  18:02:41  dblair
 * CSCdi48424:  Multilink PPP may discard fragments while processing input
 * fragments.  Do not drop fragments when unassigned threshold is reached
 * and allow lost fragments to be detected earlier.
 *
 * Revision 3.10  1996/02/15  18:03:07  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.9  1996/02/13  08:17:31  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.8  1996/01/15  19:24:20  misko
 * CSCdi43044:  ip address negotiation not working with DDR
 * When we assign an IP address, allow a dialer map to be dynamically
 * created so that we can map the assigned IP address to an authenticated
 * name.
 *
 * Revision 3.7  1996/01/12  16:48:05  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.6  1995/12/22  01:28:35  suresh
 * CSCdi29641:  Dialer dialer-group accepts 0 but does not work
 * Changed the dialer-group range to 1-10; cleaned code so that
 * dialer-list and dialer-group accesses are consistent
 *
 * Revision 3.5  1995/12/21  20:06:49  dblair
 * CSCdi43794:  Clearing a dialer Async interface prevents new calls from
 * being made
 *
 * Revision 3.4  1995/11/29  01:59:02  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.3  1995/11/17  18:01:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:49:29  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:48:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.13  1995/11/08  21:35:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.12  1995/10/21  00:40:29  dblair
 * CSCdi42505:  Invalid free memory with ppp reliable link
 *         Freed memory in protocol_up which runs from interrupt if
 *         reliable
 *         link is configure.  Added flag to data d_hql_memtype to tell
 *         dialer_onesec to free the memory
 *
 * Revision 2.11  1995/10/17  22:18:32  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.10  1995/09/30  05:06:16  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.9  1995/09/20  22:38:56  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.8  1995/09/18  19:39:42  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.7  1995/08/21  22:54:25  lbustini
 * CSCdi39089:  CLNS DDR causes router to reload
 * Ensure that dialing cause does not exceed string size.
 *
 * Revision 2.6  1995/08/08  16:50:22  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.5  1995/08/07 20:32:29  bdas
 * CSCdi38212:  Dialing reason may be incorrect in some call history mib
 * entries
 *
 * Revision 2.4  1995/08/05  08:39:52  lbustini
 * CSCdi38136:  Incorrect output interface selected when fastswitching
 * over rotary group.
 * When a ddr link comes up, use the authenticated name along with
 * the phone number to set up the route cache entry.
 *
 * Revision 2.3  1995/07/31  09:20:20  misko
 * CSCdi37919:  DDR needs to allow protocols additional control
 * Add additional controls on a per packet basis for upper layer
 * protocols.  This is to help solve CSCdi33368.  Add new flags
 * to allow the upper layers to override the dialer access lists, so
 * they can specify that a packet is interesting or uninteresting
 * regardless of what the access lists say.  Also, add a flag so that
 * the protocols can tell the dialer not to hold the packet in the holdq
 * if a call is in progress.  Lastly, have a flag to we can tell the
 * protocols if we are dropping the packet (failing encaps) because
 * a call is in progress but not connected.
 *
 * Revision 2.2  1995/06/09  20:59:42  rbeach
 * CSCdi32791:  BRI/MBRI interfaces dont allow more than 64 isdn caller
 * entries. Make the isdn caller entries dynamically allocated. Dialer
 * and ISDN call screening now share the same mechanism and code.
 *
 * Revision 2.1  1995/06/07  23:17:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DIALER_H__
#define __DIALER_H__

#define LINK_ALL LINK_MAXLINKTYPE
#define DIALER_MAX_P2P_CALLS  255    /* max calls on rot grp for p2p encap */
#define DIALER_MAX_X25_CALLS	1	/* max calls on rot grp for x25 encap */
#define DIALER_MAX_FR_CALLS	1	/* max calls on rot grp for fr encap */

#define MAXSTR			120	/* max characters in a name/string */

/*
 * V25bis dialer frame headers and definitions
 */
typedef struct dialer_hdr_ {
    ushort flags;
} dialer_hdr;

#define DIALER_ENCAPBYTES (sizeof(dialer_hdr))
#define V25BIS_FLAGS       0xff13

/*
 * Dialer types.
 */
enum DIALERS {
    DIALER_NONE,
    DIALER_ISDN,
    DIALER_INBAND_SYNC,
    DIALER_INBAND_ASYNC,
    DIALER_DTR_SYNC,
    DIALER_PROFILE,
    DIALER_MAX
};

/*
 * Dialer states.
 */
enum DIALER_STATE {
    DIALER_READY,		/* dialer is ready to take/place a call */
    DIALER_PENDING,		/* a call is being placed */
    DIALER_CONNECTED,		/* physical layer is up */
    DIALER_OPERATIONAL,		/* data link layer is up */
    DIALER_DISCONNECTING,	/* dialer is waiting for the complete disc. */
    DIALER_WAITING,		/* dialer is waiting for the enable timer */
    DIALER_SHUTDOWN		/* dialer is shut down */
};

enum DIALER_THRESHOLD_DIRECTION {
    DIALER_THRESH_OUTBOUND = 0,	/* default */
    DIALER_THRESH_EITHER,
    DIALER_THRESH_INBOUND
}; 

enum CALLBACK_TYPE {
    CALLBACK_NONE,
    CALLBACK_CLIENT,
    CALLBACK_SERVER
};

enum DIALER_DISCONNECT_REASON {
    NO_DISCONNECT,
    DISCONNECT_IDLE
};

typedef dialerdbtype * (dialer_state_t) (dialerdbtype *, dialerpaktype *);

/*
 * Dialer lists by protocol.  Used when fastswitching.
 */
struct dialerplisttype_ {
    short ip_dialerlist;
    short ipx_dialerlist;
};

/*
 * Structure to gather statistics about calls.
 */
struct dialer_stat_ {
    ulong num_successes;
    ulong num_failures;
    boolean last_call_success;
    sys_timestamp last_call_time;
    sys_timestamp curr_call_time;
};

/*
 * Dialer descriptor block.  Please keep all the fields 32-bit aligned.
 */
struct dialerdbtype_ {
    hwidbtype *ownidb;		/* "owner" idb */
    enum DIALERS dialer;	/* dialer type */
    char *dialer_profile;       /* dialer profile name */
    uchar dialer_parity;	/* none or odd, for V.25bis */
    uchar dialer_list;		/* dialer list for packet classification */
    uchar dialer_priority;	/* dialer priority for placing calls */
    uchar dialer_load_threshold;/* load threshold to place another call */
    enum DIALER_THRESHOLD_DIRECTION thresh_direction;
    enum DIALER_STATE dialer_state;	/* state of the interface */
    dialer_state_t *dialer_fsm;       /* state machine for dialer profiles */
    dialerdbtype *dialing_ddb;        /* interface that originated the call */
    dialerdbtype *profile_dialing_ddb; /* interface profile used to dial */
    hwidbtype *netcfg_idb;	/* idb that has network config */
    dialerdbtype *netcfg_ddb;	/* above idb's dialerdb structure */
    dialerdbtype *save_netcfg_ddb;   /* copy of above, used by pool member */
    dialerdbtype *hunt_group_master; /* master IDB for hunt group */
    dialerdbtype *hunt_group_first;  /* link to first member in hunt group */
    dialerdbtype *hunt_group_next;   /* link to next member in hunt group */
    dialerlisttype *dialer_plist;    /* pointer to current dialer_list */
    tinybool hunt_group_lead;	     /* are we the hunt group leader */
    tinybool dialer_placed_call;     /* flag to indicate if we placed a call */
    tinybool dialer_spoofing_int; /* flag to indicate if spoofing interface */
    tinybool dialer_spoofing_prot;/* flag to indicate if spoofing line prot */
    tinybool dialer_in_use;	     /* flag to indicate if in use */
    tinybool dialer_lineup;	     /* flag to indicate if int up & running */
    tinybool callback_secure;
    tinybool dialer_pool_member;     /* interface is in a dialer pool */
    tinybool dialer_bound;           /* dialer is bound to a profile */
    tinybool dialer_binding;         /* TRUE when interface is being bound */
    ulong dialer_save_pulseinterval; /* copy of original idb vector */
    iqueue_t dialer_save_iqueue;     /* copy of original idb vector */
    vencap_t dialer_save_vencap;     /* copy of original idb vector */
    reset_t dialer_save_reset;       /* copy of original idb vector */
    encapstype dialer_save_encap;    /* original encapsulation */
    enum FSTYPE fs_type;	     /* copy of idb->fast_switch_type */
    void *dialer_encap_ptr;	     /* used by encapsulation */
    short dialer_num_call;    /* cur number of calls on this rotary grp */ 
    uchar dialer_max_call;    /* max number of calls on this rotary grp */
    uchar dialer_num_link;    /* number of links in rotary group */
    ulong dialer_idle_ticks;  /* how long to wait before disconnect */
    ulong dialer_fast_idle_ticks;    /* how long to wait before fast disc. */
    ulong dialer_enable_ticks;	     /* how long to wait after disconnect */
    ulong dialer_carrier_ticks;	     /* how long to wait for carrier */
    ulong dialer_holdq_ticks;	     /* how long to keep packets in holdq */
    ushort fastrollover_ticks;   /* time between fastrollover dialing */
    sys_timestamp dialer_idle_timer;	/* idle disconnect timer */
    sys_timestamp dialer_fast_idle_timer;/* fast idle disconnect timer */
    sys_timestamp dialer_enable_timer;	/* reenable timer */
    sys_timestamp dialer_carrier_timer;	/* wait for carrier timer */
    sys_timestamp dialer_load_timer;	/* load computation timer */
    dialermaptype *dialer_string;	/* dialer default number */
    dialermaptype *dialer_call_group;	/* first in call group */
    dialergrouptype *conn_group;     /* pointer to connected group structure */
    dialerdbtype *next_conn;	/* next ddr idb in connected group */
    dialerdbtype *prev_conn;	/* previous ddr idb in connected group */
    dialerpooltype *dialer_pool;/* pointer to dialer pool descriptor */
    ulong dialer_holdmax;	/* maximum dialer output hold queue size */
    dialer_holdq_type *dialer_holdQ;
    enum CALLBACK_TYPE callback_type;
    enum DIALER_DISCONNECT_REASON disconnect_reason;
    dialer_stat *dialer_stats;	/* stats for current connection */
    dialer_stat dialer_dtr_stats;/* DTR dialer statistics */
    int dialer_rotor_style;	/* outbound line choice: priority or best */
    sys_timestamp last_getdialer;   /* last time we tried to dial on this line */
    sys_timestamp mru_callsuccess;  /* last successful dialout attempt */
    sys_timestamp mru_lastfailure;  /* last failed dialout attempt */
    dialer_call_tab *dialer_call;/* table of numbers for callid scr*/
    char remote_name[MAXSTR];	/* remote router's name */
    char number_called[MAXSTR];	/* number called */
    char dial_reason[MAXSTR];	/* reason why we dialed */
    int  rotary_group;          /* used only for Group Async interfaces */
    ulong dummy;
};

/*
 * A few useful macros.
 */
#define SET_PAK_DIALER_INPUT(pak)  ((pak)->dialer_input = 	\
				    hwidb_or_null((pak)->if_input))
#define GET_NETIDB(ddb)	       ((ddb)->netcfg_idb)
#define HUNTGROUP_MEMBER(ddb)  ((ddb)->hunt_group_master != NULL)
#define HUNTGROUP_LEADER(ddb)  ((ddb)->hunt_group_lead)
#define IN_HUNTGROUP(ddb)      (HUNTGROUP_LEADER(ddb) || HUNTGROUP_MEMBER(ddb))
#define DIALER_UP(ddb)	       ((ddb)->dialer_state == DIALER_OPERATIONAL)
#define DIALER_CALLUP(ddb)     (DIALER_UP(ddb) ||			    \
				((ddb)->dialer_state == DIALER_CONNECTED))
#define DIALER_CALLING(ddb)    ((ddb)->dialer_state == DIALER_PENDING)
#define DIALER_INPROGRESS(ddb) (DIALER_CALLUP(ddb) || DIALER_CALLING(ddb))
#define DIALER_SHUT(ddb)       ((ddb)->dialer_state == DIALER_SHUTDOWN)
#define DIALER_FREE(ddb)       ((ddb)->dialer_state == DIALER_READY)
#define DIALER_ENABLING(ddb)   ((ddb)->dialer_state == DIALER_WAITING)
#define DIALPOOL_MEMBER(ddb)   ((ddb)->dialer_pool_member)

/*
 * A few useful inlines.
 */

static inline dialerdbtype *get_dialerdb(hwidbtype *idb)
{
    return(idb->dialerdb);
}

static inline dialergrouptype *get_dialergroup (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(ddb->conn_group);
    }
    return(NULL);
}
 
static inline enum FSTYPE get_dialerfstype (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(ddb->fs_type);
    }
    return(FS_UNKNOWN);
}
 
/*
 * dialer_encap
 * Test whether encapsulation supports DDR.
 */
static inline boolean dialer_encap (encapstype enctype)
{
    return((enctype == ET_HDLC) ||
           (enctype == ET_PPP)  ||
           (enctype == ET_SLIP) ||
           (enctype == ET_LAPB) ||
           (enctype == ET_X25)  ||
           (enctype == ET_FRAME_RELAY) ||
	   (enctype == ET_CPP));
}
  
static inline boolean in_huntgroup (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
        return(IN_HUNTGROUP(ddb));
    }
    return(FALSE);
}
 
/*
 * Inline to return master swidb of hwidb holding the net configuration
 * Net configuration is typically held in a rotary group master, for example
 *
 * Return original swidb if desired swidb not found
 */
static inline idbtype *get_netcfg_idb (idbtype *idb)
{
    dialerdbtype *ddb;

    if (idb && idb->hwptr) {
	ddb = get_dialerdb(idb->hwptr);
	if (ddb && ddb->netcfg_idb && ddb->netcfg_idb->firstsw) {
	    return (ddb->netcfg_idb->firstsw);
	}
    }
    return(idb);
}
 
/*
 * get_netidb
 * Get idb that has all the network configuration.
 */
static inline hwidbtype *get_netidb (hwidbtype *idb)
{
    dialerdbtype *ddb;

    ddb = get_dialerdb(idb);
    if (ddb != NULL) {
	return(GET_NETIDB(ddb));
    }
    return(idb);
}
 
/*
 * dialer.c
 */
extern void dialer_disconnect(hwidbtype *);
extern void dialer_unlink_member(dialerdbtype *);
extern boolean dialer_async_classify(hwidbtype *, paktype *);
extern boolean huntgroup_leader(hwidbtype *);
extern boolean huntgroup_member(hwidbtype *);

/*
 * dialer_config.c
 */
extern hwidbtype *get_netidb(hwidbtype *);
extern idbtype *get_pak_netidb(paktype *);


#endif __DIALER_H__
