/* $Id: isdnintrfc.c,v 3.18.4.25 1996/09/10 02:43:42 suresh Exp $
 * $Source: /release/112/cvs/Xsys/isdn/isdnintrfc.c,v $
 *------------------------------------------------------------------
 * isdnintrfc.c  --  interface between router and ISDN code
 *  
 * November 1991, Ronnie Kon
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: isdnintrfc.c,v $
 * Revision 3.18.4.25  1996/09/10  02:43:42  suresh
 * CSCdi68129:  dialer pool-member 1 min-link 1 doesnt reserve the link
 * Branch: California_branch
 * Completely rewritten code for sharing of ISDN interfaces
 *
 * Revision 3.18.4.24  1996/09/09  18:58:48  bdas
 * CSCdi67753:  incorrect object value for demandNbrCallOrigin in isdn mib
 * and trap
 * Branch: California_branch
 *
 * Revision 3.18.4.23  1996/09/02  23:21:09  jchlin
 * CSCdi66886:  ISDN cause sent back to CO doesnt make CO to rehunt next
 * PRI group
 * Branch: California_branch
 * o       Make modem msg queue bigger.
 * o       Use the error cause returned by CSM to reject an incoming
 *         call.
 *
 * Revision 3.18.4.22  1996/08/26  23:08:21  wmay
 * CSCdi66907:  ISDN incoming calls getting rejected - enforce call ranges
 * for modem and dialer type calls
 * Branch: California_branch
 *
 * Revision 3.18.4.21  1996/08/16  18:08:11  rbeach
 * CSCdi64603:  Voice bearer calls reported incorrectly as 256 kb/sec.
 * This is a cosmetic bug only. Correctly display 64 kb/sec.
 * Branch: California_branch
 *
 * Revision 3.18.4.20  1996/08/16  01:27:11  billw
 * CSCdi65786:  Bus Error, attempted to free unassigned memory
 * Branch: California_branch
 *
 * Revision 3.18.4.19  1996/08/07  09:01:05  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.18.4.18  1996/07/24  21:24:34  wmay
 * CSCdi63927:  BRI double allocates remote number - remove 2nd calling
 * number extract.
 * Branch: California_branch
 *
 * Revision 3.18.4.17  1996/07/22  17:07:10  wmay
 * CSCdi63663:  ISDN code goes on a diet - save about 2K in UserIdle
 * Branch: California_branch
 *
 * Revision 3.18.4.16  1996/07/19  17:44:23  wmay
 * CSCdi63486:  pri switch-type with no pri-group causes heavy cpu
 * utilization
 * Branch: California_branch
 *
 * Revision 3.18.4.15  1996/06/29  00:56:49  thille
 * CSCdi61613:  Trim a little fat from isdn too.
 * Branch: California_branch
 * Implement second code review comments.  Thanks Sandra.
 *
 * Revision 3.18.4.14  1996/06/28  23:19:59  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.18.4.13  1996/06/28  23:13:17  thille
 * CSCdi61613:  Trim a little fat from isdn too.
 * Save 208 bytes by cleaning up literals.
 *
 * Revision 3.18.4.12  1996/06/16  21:13:25  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.18.4.11  1996/06/12  16:48:31  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - no more special init for b-channel interfaces
 * Branch: California_branch
 *
 * Revision 3.18.4.10  1996/05/21  06:33:41  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.18.4.9  1996/05/19  03:09:32  hrobison
 * CSCdi56813:  CSCdi46413 breaks platform independent file modularity
 * Branch: California_branch
 * With the excuse of changes required by the new compiler, take the
 * opportunity to remove platform dependancies, improve code
 * modularity and do some general code clean-up.
 *
 * Revision 3.18.4.8  1996/05/07  04:13:25  hrobison
 * CSCdi56472:  CSCdi46413 broke modularity if ISDN not included
 * Branch: California_branch
 * Quick fix: Change external reference to structure in ../isdn to
 * a registry call.  Code is scheduled for clean-up in CSCdi56813.
 *
 * Revision 3.18.4.7  1996/05/01  14:51:22  hrobison
 * CSCdi46413:  MIP when used as PRI must be in slot 0-4
 * Branch: California_branch
 * Change PRI initialization: for the 7xxx platform the
 * dsl id is now assigned independently of the MIP card
 * slot/subunit location.  The number of PRI interfaces
 * per router is still limited to 10.  The dsl assignment
 * for the 4xxx platform is based on the slot location
 * and corrected to allow only 1 dsl id per slot.
 *
 * Read bug release notes for details on how the ISDN
 * debug output and ISDN show commands have changed.
 * The debug output now uses the hw_short_namestring
 * rather than PRI<dsl #> or BRI<dsl #>.  The show commands
 * now take the interface name or the dsl id as input.
 *
 * Revision 3.18.4.6  1996/04/30  23:07:28  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.18.4.5  1996/04/27  05:54:38  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.18.4.4  1996/04/20  17:59:12  lol
 * CSCdi46005:  ISDN CLID not propagated to rem_addr field of T+ packet
 * Branch: California_branch
 *
 * Revision 3.9.2.4  1996/04/26  01:57:32  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.9.2.3  1996/04/23  00:32:48  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.9.2.2  1996/04/17  03:04:54  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.9.2.1  1996/02/23  20:24:41  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.18.4.3  1996/04/13  03:54:25  rbeach
 * CSCdi50619:  Dialer tries to open third channel on same BRI
 * interface. This is caused if an orphaned call leaves the
 * wait for carrier timer running. When the timer fires num_avail_b
 * gets incremented to 3. The wait for carrier timer is now
 * stopped in this case.
 * Branch: California_branch
 *
 * Revision 3.18.4.2  1996/03/29  17:51:33  rbeach
 * CSCdi24243:  Invalid Malloc at interrupt level called by
 * isdn_fromrouter. This is caused when doing OIR testing.
 * All elements are now allocated only once and put in a save
 * queue.
 * Branch: California_branch
 *
 * Revision 3.18.4.1  1996/03/18  20:31:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.11.2.4  1996/03/16  07:00:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.11.2.3  1996/03/13  01:40:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.11.2.2  1996/03/07  09:51:44  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.11.2.1  1996/02/20  01:05:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.18  1996/03/07  19:45:33  dclare
 * CSCdi50927:  Primary Rate ISDN IDD called address in wrong format for
 * primary-ts014.
 *
 * Revision 3.17  1996/03/06  16:10:56  dclare
 * CSCdi47302:  segV exception in ClearTimer/LIF_StopTimer. Increase the
 * number of blocks and reflect change in show isdn memory.
 *
 * Revision 3.16  1996/02/28  17:48:42  bdas
 * CSCdi49599:  ISDN MIB needs to update some entries upon a clear int
 *
 * Revision 3.15  1996/02/15  18:02:11  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.14  1996/02/15  17:27:32  rbeach
 * CSCdi47018:  sh dialer and sh isdn stat do not agree on MBRI.
 * The global Dsl_ID was being used when not initialized. Cleaned
 * up all the BRI shutdown routines and now pass the dsl as
 * a parameter. Removed dead code from all the shutdown routines.
 *
 * Revision 3.13  1996/02/12  22:23:27  rbeach
 * CSCdi48606:  ISDN incoming voice calls assume 56kb incorrectly.
 * Get the correct rate from the user_rate in the bearer capability.
 *
 * Revision 3.12  1996/02/08  03:06:29  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.11  1996/01/22  06:35:14  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10  1996/01/18  23:33:37  gtaylor
 * CSCdi47050:  bad bearercap debug message on PRI prints corrupt
 * information string
 *
 * Revision 3.9  1996/01/12  16:45:47  rbeach
 * CSCdi46101:  ISDN 4ESS Pri doesn't correctly handle outbound NSF.
 * The correct network specific facility values are now configurable.
 * We support outbound SDN, MEGACOMM and ACCUNET.
 *
 * Revision 3.8  1996/01/09  17:57:42  rbeach
 * CSCdi42595:  ISDN voice calls are screened when isdn caller is
 * configured. This is not the correct behavior. Moved the call
 * screening code after the voice call check.
 *
 * Revision 3.7  1995/11/28  17:59:11  rbeach
 * CSCdi44496:  PRI voice call bearer capability invalid. The bearer
 * rate is invalid on outgoing PRI voice calls.
 *
 * Revision 3.6  1995/11/22  18:13:13  rbeach
 * CSCdi36058:  ISDN code doesnt check malloc return from
 * extract_calling_num. If the malloc fails low memory can
 * be written on.
 *
 * Revision 3.5  1995/11/21  04:51:59  dclare
 * CSCdi42123:  isdn layer 2 gets hung in ESTABLISH_AWAITING_TEI. The PBX
 * can give the router a B-channel already in use. Make sure it is not in
 * use, if it is in use, clear the call.
 *
 * Revision 3.4  1995/11/20  22:42:02  wmay
 * CSCdi42512:  Bad V.120 ISDN LLC will crash box - after disconnecting,
 * break out of the main switch statement.
 *
 * Revision 3.3  1995/11/17  17:41:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:49:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:13:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.17  1995/11/08  21:07:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.16  1995/10/21  06:35:12  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.15  1995/10/19  15:11:02  dclare
 * CSCdi42248:  PRI doesnt support international called party number with
 * Accunet. For a 4ESS switchtype only, if a number starts with 011 or
 * x011, then mark it as international.
 *
 * Revision 2.14  1995/10/17  22:14:40  rbeach
 * CSCdi36915:  voice calls are not handled correctly over ISDN.
 * Allow configuration for data over voice on both outgoing and
 * incoming calls. For outgoing there is a new option for the
 * dialer map command called class. For incoming there is a new
 * interface option "isdn incoming-voice data". This will allow
 * the isdn code to answer incoming voice calls.
 *
 * Revision 2.13  1995/10/16  17:54:58  dclare
 * CSCdi40762:  PRI connected to DMS100 incorrectly puts B channels out of
 * service. A Restart and a shutdown of the interface will put channels
 * back into in-service.
 *
 * Revision 2.12  1995/09/26  23:42:56  dclare
 * CSCdi39627:  Problem establishing ISDN calls to 4500 MBRI port from 2
 * locations. Problem handling clear int bri 0, with calls up or incoming
 * calls with Layer 2 not activated. Calls are accepted, but CCBs get lost
 * and the CONNECT message does not get sent out correctly to the network.
 *
 * Revision 2.11  1995/08/30  16:46:32  rbeach
 * CSCdi37341:  Router locks up when removing Cx1 controller
 * configuration.  If there is a call connected and the controller
 * configuration is removed the router will crash. This is caused
 * by the isdn_info structure being free'ed from the de-configuration.
 * A check for a NULL isdn_info structure has been added.
 *
 * Revision 2.10  1995/08/28  21:20:04  bdas
 * CSCdi37630:  1TR6 semi-permanent : spc facility missing from connect
 *
 * Revision 2.9  1995/08/25  11:44:14  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.8  1995/07/28 16:29:26  dclare
 * CSCdi37107:  Speed up handling of ISDN BRI Layer 1 primitives.
 *
 * Revision 2.7  1995/07/28  15:24:34  dclare
 * CSCdi37836:  ISDN BRI switches DMS-100 and NI1 may need only one TEI.
 * One spid configured, use one TEI. Two spids configured, use two TEIs.
 *
 * Revision 2.6  1995/06/30  17:35:36  rbeach
 * CSCdi36633:  Australia ISDN PRI messes up the time slots for other
 * switchtypes. If the switchtype was not Australia the B channel
 * numbering was off by one.
 *
 * Revision 2.5  1995/06/26  18:02:27  dclare
 * CSCdi36415:  Addition of Calling number for Australia ISDN PRI (TS014).
 *
 * Revision 2.4  1995/06/23  15:14:17  dclare
 * CSCdi36325:  Australian ISDN PRI implementation for the 7000. Also get
 * rid of some homologation defines that became too awkward to use. These
 * are the final changes found during homologation.
 *
 * Revision 2.3  1995/06/15  23:10:18  bdas
 * CSCdi33538:  incomplete/wrong object instances returned; ISDN MIB,
 * Call History Mib.
 *
 * Revision 2.2  1995/06/09  20:59:13  rbeach
 * CSCdi32791:  BRI/MBRI interfaces dont allow more than 64 isdn caller
 * entries. Make the isdn caller entries dynamically allocated. Dialer
 * and ISDN call screening now share the same mechanism and code.
 *
 * Revision 2.1  1995/06/07  21:15:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Include the requisite cisco headers */
#include	"master.h"
#include	<string.h>
#include	<ciscolib.h>
#include	"packet.h"
#include	"interface_private.h"
#include        "../if/network.h"
#include        "../h/snmp_interface.h"
#include        "../snmp/ifmib_registry.h"
#include	"address.h"
#include	"../les/isdn_bri.h"
#include	"../les/if_les.h"
#include        "logger.h"

/*
 * Temporary hack until all the BRI drivers are combined
 */
#ifdef SAPPHIRE
#include	"../les/if_les_bri_c1000.h"
#else
#include	"../les/if_les_bri.h"
#endif

#include    	"../ui/debug.h"
#include    	"../wan/isdn_debug.h"
#include    	"../wan/dialer.h"
#include    	"../wan/dialer_private.h"
#include        "../wan/dialer_debug.h"
#include    	"../wan/isdn.h"
#include	"media_registry.h"
#include	"isdn_registry.h"
#include	"../wan/dialer_registry.h"
#include        "../h/cs_registry.regh"
/*
 *	We define PRIVATE to something other than static.  This is wrong
 *  enough to fix here, rather than mess with Telenetwork's headers.
 */
#undef PRIVATE

/*
 * Telenetworks defines NO_ERROR in an enum, while Snmp Research #define's it
 * (the joys of integrating 3rd party code...)
 */
#undef NO_ERROR

/* And now the Telenetworks headers */
#include	"gendef.h"
#include	"flags.h"
#include	"exec.h"
#include	"tune.h"
#include	"lif.h"
#include	"gp.h"
#include	"ccbri.h"
#include	"ccie.h"
#include	"exec.h"
#include	"execuser.h"
#include	"switch_def.h"
#include 	"l2.h"
#include	"cc_chan.h"

#ifdef GLOBAL_DEBUG
#	define	assert(pred)	{ if(!(pred)) { buginf("ASSERTION FAILED: file %s, line %d\n", __FILE__, __LINE__); } }
#else
#	define	assert(pred)
#endif

/*
 *      numbers that vary by switch type
 */
ulong DEFAULT_T200, DEFAULT_T203, DEFAULT_K, DEFAULT_N200;

/*
 *	declare my variables
 */
static const char isdn_received_msg[] = "\nISDN %s: received %s";
static queuetype q_from_router_sw, q_from_driver, q_from_modem, q_of_msgs, q_of_rmsgs;
#define FROM_ROUTERSW	&q_from_router_sw
                            /* the ISDN control module.  This is a queue
			       of struct br_msg's */
#define FROM_DRIVER	&q_from_driver
#define FROM_MODEM	&q_from_modem
#define FOR_MODEM_MSGS	&q_of_msgs
#define FOR_ROUTER_MSGS  &q_of_rmsgs

#define FROM_PHYSICAL_LAYER  IsdnL1Q
#define FIRST_DSL 0 /* First interface number */
#define SUBADDR_SEPARATOR ':'
#define ZERO_OFFSET 0  /* Used by subaddress match routine */
#define AFI_BYTE 0x50  /* Authority and Format Identifier : This is
                       present in the 4th octet of the subaddress */
#define SUBADDR_SEP_LEN 1
#define NON_SPC_CALL  FALSE
#define FAILED_CALL 1
#define ACCEPTED_CALL 2
#define REFUSED_CALL  3
#define DISC_OTHER_CAUSE            1
#define NORMAL_DISC_SENT            2
#define NORMAL_DISC_RECEIVED        3 
#define DISC_NETWORK_OUT_OF_ORDER   4 
#define DISC_CALL_REJECTED          5 
#define DISC_USER_BUSY              6 
#define NO_CHANNEL_AVAILABLE        7
#define INTERWORKING_ERROR          8 
/* D-channel idb to DSL_ID mapping. For Basic Rate, there are 
 * now multiple idbs. Check the tune.h file for exact values.
 */
hwidbtype *isdn_idblist[MAX_DSL];
static sys_timestamp next_tick, last_act_attempt, isdn_p1_expiration_time;
static uchar line_status[MAX_DSL]; /* Status table for all BRI interfaces */
PRIVATE char *spc_number[MAX_DSL]; /* Semi permanent connection number */
ulong L2_Discards[MAX_DSL]; /* Number of L2 DiscardQs taken place */
ulong L2D_Task_Counter[MAX_DSL]; /* Number of L2 DiscardQs at L2D Task */
BOOLEAN is_pri = FALSE;
static boolean core_init = FALSE;

static boolean pri_phys_ind = FALSE;
boolean Network_side = FALSE; /* flag to run PRI back-to-back */

/*
 * global defines
 */
queuetype *IsdnL1Q = NULL;		/* Q for passing prims from
					   L1 to isdn */
queuetype *IsdnL1PrimPoolQ = NULL;	/* Q which contains pre-allocated
					   L1 Q elements */

/*
 *	Stuff for keeping track of statistics
 */
enum frametypes {
	/*
	 *	Note that the show_statistics() routine relies on the order
	 *  given.  If you change anything, you must also change it there.
	 */

	/* the layer 2 stuff */
	Q921_INFO = 0,
	Q921_RR,
	Q921_RNR,
	Q921_REJ,
	Q921_SABME,
	Q921_DM,
	Q921_UI,
	Q921_DISC,
	Q921_UA,
	Q921_FRMR,
	Q921_XID,

	/* the layer 3 stuff */
	Q931_ALERTING,		/* must be 1st layer 3 entry */
	Q931_CALL_PROC,
	Q931_CONNECT,
	Q931_CONNECT_ACK,
	Q931_PROGRESS,
	Q931_SETUP,
	Q931_SETUP_ACK,
	Q931_RESUME,
	Q931_RESUME_ACK,
	Q931_RESUME_REJ,
	Q931_SUSPEND,
	Q931_SUSPEND_ACK,
	Q931_SUSPEND_REJ,
	Q931_USER_INFO,
	Q931_DISCONNECT,
	Q931_RELEASE,
	Q931_RELEASE_COMP,
	Q931_RESTART,
	Q931_RESTART_ACK,
	Q931_SEGMENT,
	Q931_CONG_CONT,
	Q931_INFORMATION,
	Q931_FACILITY,
	Q931_NOTIFY,
	Q931_STATUS,
	Q931_STATUS_ENQ,
	Q931_USETUP,

	NUM_STAT_ENTRIES		/* must be last */
};
enum direction { TX = 0, RX, NUM_DIR_ENTRIES };
static int statistics[NUM_STAT_ENTRIES][NUM_DIR_ENTRIES];

/* the internal message structure we use. */
struct br_msg {
	struct br_msg	*next;
	enum BriMsgType	message;
	enum EncodeType encoding;
	ushort 		call_id;
	char		*number;
	uint		speed;
	int			ces;
	struct hwidbtype_	*idb;
        ushort           bchan; /* used for modem support */
        boolean          spc; /* Semi permanent connection */
        boolean          voice_call; /* outgoing voice support */
        boolean          do_nsf;     /* outgoing nsf support */
        ushort           nsf_type;   /* outgoing nsf value */
};


extern void LM_PersDeact(int);
extern void show_nlcbs(int);
/*
 *	declare my procedures
 */

/* procedures with external linkage declared in ../les/isdn_bri.h */

/* local procedures */
static BYTE *my_Get_IE(PKG_ID, BYTE);
BYTE *my_Get_IE_pkt(PKT_ID, BYTE);
static boolean	isdn_p(void);
static boolean	isdn_p2(void);
static char	*extract_calling_num(PKG_ID);
static enum Chan extract_bchan(PKG_ID);
static int	parse_progress(PKG_ID);
static int      parse_bearercap(PKG_ID, boolean, boolean, boolean *);
static int	parse_network_spec_facil(PKG_ID);
static void	parse_llc(PKG_ID, llc_parse_type *);
static void	display_llc(int dsl, uchar type, llc_parse_type *llc);
static int	parse_hlc(PKG_ID);
static int	parse_service(PKG_ID);
static int      process_bri_call(ushort, char *, uint, int, boolean, boolean);
static int      process_pri_call(struct br_msg *, boolean);
static int      process_disconnect(UINT16, int, int, int, int, boolean);
static int	process_rejectcall(ushort, int, int);
static int	process_ignore(UINT16, int, int);
static int	process_simple(BYTE, UINT16, enum Chan, int, int);
static int	process_pri_simple(BYTE, UINT16, enum Chan, int);
static int	process_modem_callproc(BYTE, UINT16, enum Chan, int);
static int	process_modem_conn(BYTE, UINT16, enum Chan, int);
static int	process_disc_ack(UINT16, int, int, int, int);
static void	badframe(uchar *, int);
static void	badpacket(uchar *, int);
static void	decode_ie(uchar *, int, int);
static void	decode_info(uchar *, int, int, int);
static void	do_statistics(paktype *, enum direction, int);
static void	isdn_buginf_flush(void);
static void	isdn_display(paktype *, enum direction, int);
static void	isdn_report(int, enum BriMsgType, UINT16, enum Chan, char *, enum rem_src, int);
static void	process_rxdata(paktype *);
static void	process_rxstate(void);
static void	process_modem_command(struct br_msg *);
static void	show_data(uchar *, int, boolean, uchar *);
static void	show_cause(uchar *);
static void	show_progress(uchar *);
static void	show_signal(uchar *);

static char *extract_called_number(PKG_ID);
static char *extract_spc_number(PKG_ID);
static char *extract_sa(PKG_ID);
static boolean verify_answer_number(PKG_ID);
static boolean compare_num(char *, char *, char *);
static boolean match_num(char *, char *, int, int, int);
static void isdn_mib_extract_cause(PKG_ID, enum direction, isdn_bri_t *);
static void isdn_mib_cause_init(int, enum direction, isdn_bri_t *);
static hwidbtype *isdn_mib_find_idb(UINT16, hwidbtype *);
static ushort	isdn_frommodem(hwidbtype *, ushort, ushort, ushort, ushort, ushort, uchar *, ushort);

/* The dsl is now independant of slot/subunit position */
static hwidbtype *dsl_list[MAX_DSL];

/* Derive interface namestring from Dsl id */
inline char *DSL_TO_NAME (int dsl) {
   if (isdn_idblist[dsl] != NULL)
      return(isdn_idblist[dsl]->hw_short_namestring);
   else
      return("Error: Invalid Interface");
}

/* We probably will be able to remove these...*/
static inline hwidbtype *BCHAN_TO_IDB (int dsl, enum Chan bchan)
{
    return(isdn_idblist[dsl]->isdn_info->b_chan_idb[bchan]);
}

static inline hwidbtype *BCHAN_TO_IDB_WITH_CHECK (int dsl, enum Chan bchan)
{
    if (isdn_idblist[dsl] &&
	isdn_idblist[dsl]->isdn_info)
	return(BCHAN_TO_IDB(dsl,bchan));
    else
	return(NULL);
}

static inline ulong BCHAN_TO_BITFIELD (enum Chan bchan)
{
    return(1 << (31 - (int)bchan));
}

/*
 * isdn_increment_bchan
 * increment the number of available B
 * channels for an interface
 */
inline void isdn_increment_bchan (isdn_bri_t *d_isdn_info)
{
    int max_bchannels;
    dialerdbtype *ddb;

    ddb = get_dialerdb(d_isdn_info->d_chan_idb);

    if (is_pri)
       max_bchannels = ddb->dialer_num_link - get_dialer_minsum(ddb);
    else
       max_bchannels = SERIALS_PER_BRI;

    /*
     * This limit check shouldn't be necessary, but due to the
     * complex interactions of virtual, dialer, and real serial interfaces
     * for the same call, some limit checking is necessary to keep
     * this number in bounds.
     */
    if (d_isdn_info->num_avail_b < max_bchannels)
       d_isdn_info->num_avail_b++;
}

/*
 * isdn_decrement_bchan
 * decrement the number of available b
 * channels for an interface
 */
inline void isdn_decrement_bchan (isdn_bri_t *d_isdn_info) 
{
    /*
     * This limit check shouldn't be necessary, but due to the
     * complex interactions of virtual, dialer, and real serial interfaces
     * for the same call, some limit checking is necessary to keep
     * this number in bounds.
     */
    if (d_isdn_info->num_avail_b > 0)
       d_isdn_info->num_avail_b--;
}

/*
 *		Because we do so many little buginf()s to print things out,
 *	we were running out of tty buffers.  So we collect and flush when
 *	we are done.
 */
static char isdn_buginf_msg[512];		/* long enough */

static void
isdn_buginf_flush()
{
    char c;

    if (*isdn_buginf_msg) {
	/* buginf only handles msgs 300 bytes long or so */
	if (strlen(isdn_buginf_msg) > 256) {
	    c = isdn_buginf_msg[256];
	    isdn_buginf_msg[256] = 0;
	    buginf(isdn_buginf_msg);
	    isdn_buginf_msg[256] = c;
	    buginf(&isdn_buginf_msg[256]);
	} else
	    buginf(isdn_buginf_msg);
	*isdn_buginf_msg = 0;
    }
}

/*
 * isdn_init_dsl_list
 */
void isdn_init_dsl_list(void) {

   int ix;

   /* intialize idb pointer in each element */
   for (ix = 0; ix < MAX_DSL; ix++)
      dsl_list[ix] = NULL;
 }

/*
 * isdn_verify_dsl
 *
 *    Verify that a free dsl is available and
 *    reserve it for this interface.
 */
int isdn_verify_dsl (void) {
int dsl;

    /* check to make sure a dsl is available
     * to assign to this interface
     */
    for (dsl = 0; dsl < MAX_DSL; dsl++) {
       if (dsl_list[dsl] == NULL) {
          dsl_list[dsl] = (hwidbtype *)ISDN_DSL_RESERVED;
          return(dsl);
       }
    }
    return(ISDN_NO_DSL);
}

/*
 * isdn_assign_dsl
 *
 *   Store idb in reserved dsl.
 */
void isdn_assign_dsl (hwidbtype *dchan_idb, int dsl) {

   if (dsl_list[dsl] == (hwidbtype *)ISDN_DSL_RESERVED)
      dsl_list[dsl] = dchan_idb;
   else
      buginf("\nISDN %s: Error: interface dsl is not "
             "reserved.", dchan_idb->hw_short_namestring);
}

/*
 * isdn_deassign_dsl
 *
 *   When a 'no pri-group' occurs,
 *   deassign the dsl.
 */
void isdn_deassign_dsl (int dsl) {

   dsl_list[dsl] = NULL;
   isdn_idblist[dsl] = NULL;
}

/* initialize everything */
void isdn_init_core (hwidbtype *idb_assn, int dchan_id)
{
    struct br_msg *msg;
    int ix;

    if (!core_init) {
	core_init = TRUE;

        /*
         * Make bootstrap rom use isdn tei-firstcall, will keep from
         * confusing the switch by asking for Layer 2 TEIs when netbooting
         * or reloading the router.
         */
        if (bootstrap_enable)
            isdn_pu_establish = FALSE;

        reg_add_isdn_frommodem(isdn_frommodem, "isdn_frommodem");
        queue_init(FROM_ROUTERSW, MAX_ROUTER_QSIZE);
        queue_init(FROM_DRIVER, MAX_DRIVER_QSIZE);
        queue_init(FROM_MODEM, MAX_MODEM_QSIZE);
        queue_init(FOR_MODEM_MSGS, MAX_MODEM_QSIZE);
        queue_init(FOR_ROUTER_MSGS, MAX_ROUTER_QSIZE);
        /*
         * Messages coming from the modem code may call us
         * from interrupt level. To avoid a malloc at interrupt
         * time the structures are allocated here.
         */
        for (ix=0; ix < MAX_MODEM_QSIZE; ix++) {
            msg = malloc(sizeof(*msg));
            if (msg == NULL)
                buginf ("\n isdn_init_core modem malloc br_msg failed");
            enqueue(FOR_MODEM_MSGS, msg);   
        }
        for (ix=0; ix < MAX_ROUTER_QSIZE; ix++) {
            msg = malloc(sizeof(*msg));
            if (msg == NULL)
                buginf ("\n isdn_init_core malloc br_msg failed");
            enqueue(FOR_ROUTER_MSGS, msg);
        }
	/*
	 * initialize protocol defaults
	 */
	switch(SwitchType) {
	case PRI_1TR6_STYPE:
	case PRI_4ESS_STYPE:
	case PRI_DMS100_STYPE:
	case PRI_NTT_STYPE:
	case PRI_TS014_STYPE:
	case PRI_NET5_STYPE:
	case PRI_5ESS_STYPE:
	    is_pri = TRUE;
	    break;

	case BRI_5ESS_STYPE:
	case BRI_NET3_STYPE:
	case BRI_DMS100_STYPE:
	case VN2_STYPE:
	case VN3_STYPE:
	case KDD_STYPE:
	case NTT_STYPE:
	case BRI_1TR6_STYPE:
	case BRI_NI1_STYPE:
	case BRI_TS013_STYPE:
	default:
	    is_pri = FALSE;
	    break;
	}
	/*
	 * Initialize the B-channels. Have to do this here because
	 * of a race condition with CCPRI_Init.
	 */
	if (is_pri)
	    CC_CHAN_InitTablepri( MIN_CHANNEL,
            MAX_CHANNELP,
            CHANNEL_RESERVED, /* Initial chan state */
            0, /* Protocol not defined */
            0, /* Initial Call_ID */
            IN_SERVICE ); /* Initial serv state */
    }
    isdn_idblist[dchan_id] = idb_assn;
    isdn_idblist[dchan_id]->iqueue = isdn_iqueue;
    line_status[dchan_id] = IF_DOWN;
    L2_Discards[dchan_id] = 0; 
    L2D_Task_Counter[dchan_id] = 0; 

    if (detail_debug) {
	if (is_pri)
	    buginf("\nInit PRI%d", dchan_id);
	else
	    buginf("\nInit BRI%d: idb is %x", dchan_id, idb_assn);
    }

    GET_TIMESTAMP(next_tick);
    TIMER_STOP(last_act_attempt);	/* for VN2 activation procedure */

}

/*
 * Show the values of the isdn L2 timers, for the current switchtype.
 * The Layer 3 timers are embedded further in the isdn code.
 */
void show_isdn_L2timers ()
{
    printf("\n\tISDN Layer 2 values:");
    printf("\n\t K    = %d   outstanding I-frames", DEFAULT_K);
    printf("\n\t N200 = %d   max number of retransmits", DEFAULT_N200);
    /*
     * make sure the millisecond values are displayed in seconds
     */
    printf("\n\t T200 = %d   seconds", DEFAULT_T200 / 1000);
    printf("\n\t T202 = %d   seconds", DEFAULT_T202 / 1000);
    printf("\n\t T203 = %d  seconds", DEFAULT_T203 / 1000);
    if (Network_side)
	printf("\n\t Network side configuration");
}

/*
 * Show the state and service of the isdn pri
 * if a -1 is passed, then all interfaces are
 * shown, otherwise only the requested interface.
 */
void show_isdn_pri (int dsl)
{
    CC_CHAN_Show_Status(dsl);
}

/*
 * Show the status of the ISDN Line, Layer 1, 2 and 3.
 * Will also display the switchtype currently active.
 */
void show_isdn_status (int dsl_id)
{
    CCB_ID ccb, ccb_next;
    int start;
    int range;
    int dsl;
    hwidbtype *current_isdnidb;
    int chan;
    dialerdbtype *ddb;

    if (SwitchType == DEFAULT_ISDN_SWITCH)
	printf("\n **** No ISDN Switchtype currently defined ****");
    else {
	printf("\nThe current ISDN Switchtype = ");
	printf(print_isdn_switch(SwitchType));
    }
    if (Network_side) {
        printf(", Network side configuration");
    }

    if ((dsl_id < 0) || (dsl_id >= MAX_DSL)) {
        start = 0;
        range = MAX_DSL;
    } else {
        start = dsl_id;
        range = dsl_id + 1;
    }
    automore_enable(NULL);
    for (dsl = start; dsl < range; dsl++) {
	current_isdnidb = isdn_idblist[dsl];
	/*
	 * Show the interfaces that are defined.
	 */
	if (current_isdnidb != NULL) {
            printf("\nISDN %s interface",
                   current_isdnidb->hw_namestring);
            printf("\n    Layer 1 Status:\n\t");
            switch (line_status[dsl]) {
                case 0x00:
                    printf("ACTIVATING");
                    break;
                case 0x01:
                    printf("ACTIVE");
                    break;
                case 0x02:
                    printf("ACTIVE_ErrorInd");
                    break;
                case 0x03:
                    printf("DEACTIVATED");
                    break;
                default:
                    printf("Unknown state");
                    break;
            }

            /*
             * Show Layer 2
             */
            show_dlcbs(dsl);

            /*
             * Show the spid info and related info in Host control block.
             */
            show_host_cb(dsl);

            /*
             * Show Layer 3
             */
            show_nlcbs(dsl);

	    /*
	     * Look at the CCBs
	     */
	    printf("\n    Activated dsl %d CCBs = %d", dsl, ActiveCCBs[dsl]);
	    ccb = CCB_Head[dsl];
	    while (ccb) {
		mem_lock(ccb);

	       /*
	        * Austrailia has B1 on TS0; the D channel assigned to TS15.
	        * So adjust the display for this switch type.
	        */
	        if ((SwitchType == PRI_TS014_STYPE) && (ccb->B_Chan <= 15))
	           chan = ccb->B_Chan - 1;
	        else
	           chan = ccb->B_Chan;
	        printf("\n\tCCB:callid=%x, sapi=%x, ces=%x, B-chan=%d",
	               ccb->Call_ID, ccb->Sapi, ccb->Ces, chan);
	        ccb_next = ccb->Next;
		free(ccb);
		ccb = ccb_next;
	    }
	    ddb = get_dialerdb(current_isdnidb);
            if (detail_debug) {
	       printf("\n    Number of active calls = %d",
	              ddb->dialer_num_call);
	       printf("\n    Number of available B-channels = %d",
	              current_isdnidb->isdn_info->num_avail_b);
            }

            if (L2_Discards[dsl] != L2D_Task_Counter[dsl]) {
	        printf("\n    Number of L2 Discards = %d"
	           "L2D_Task Discards = %d", L2_Discards[dsl], 
		   L2D_Task_Counter[dsl]);
            }

	    automore_conditional(0);
        }
    }
    printf("\n    Total Allocated ISDN CCBs = %d", AllocatedCCBs);
}

/* set the switch type */
void
isdn_switchtype (SWITCH_TYPE Switch)
{
    /*
     * NZ = New Zealand, NW = Norway
     * they are a subswitch of NET3
     */
    if (Switch == BRI_NZNET3_STYPE || Switch == BRI_NWNET3_STYPE) {
	SwitchType = BRI_NET3_STYPE;
	SubSwitchType = Switch;
    } else {
	SwitchType = Switch;
	SubSwitchType = NO_ISDN_SWITCH;
    }
}

/*
 * Get the switch type. The Norway and New Zealand are subswitch types of
 * the NET3 type.
 */
SWITCH_TYPE
isdn_getswitchtype ()
{
    if ((SwitchType == BRI_NET3_STYPE) &&
	(SubSwitchType == BRI_NZNET3_STYPE ||
	 SubSwitchType == BRI_NWNET3_STYPE))
	return SubSwitchType;
    else
	return SwitchType; 
}

/*
 *	This is the predicate for the schedular.  It returns FALSE if we need
 *  to be scheduled, TRUE otherwise.
 */
static boolean
isdn_p (void)
{
    if (!is_pri)
	return (SLEEPING(isdn_p1_expiration_time) && QUEUEEMPTY(FROM_ROUTERSW) &&
		QUEUEEMPTY(FROM_DRIVER) && QUEUEEMPTY(FROM_PHYSICAL_LAYER));
    else
        return (SLEEPING(isdn_p1_expiration_time) && QUEUEEMPTY(FROM_ROUTERSW) &&
                QUEUEEMPTY(FROM_DRIVER) && QUEUEEMPTY(FROM_MODEM) &&
                pri_phys_ind == FALSE);
}

/* like above, but does not check for timer expiry */
static boolean
isdn_p2 ()
{
    if (!is_pri)
	return QUEUEEMPTY(FROM_ROUTERSW) &&
	    QUEUEEMPTY(FROM_DRIVER) && 
	    (IsdnL1Q == NULL || QUEUEEMPTY(FROM_PHYSICAL_LAYER));
    else
	return QUEUEEMPTY(FROM_ROUTERSW) && QUEUEEMPTY(FROM_DRIVER) &&
               QUEUEEMPTY(FROM_MODEM) && pri_phys_ind == FALSE &&
               QUEUEEMPTY(FROM_PHYSICAL_LAYER);

}
#ifdef PAN
/*
 *		The predicate for waiting for activation for VN2.  Only waits
 *	for a change in line_status, or T3 expiry (which should have caused
 *	the change in any case--the alarm time is a safety catch).
 */
static boolean
isdn_p4 (void)
{
    return QUEUEEMPTY(FROM_PHYSICAL_LAYER);
}
#endif
/*
 *	The Line up routine
 */
#if 0
static void
isdn_lsd ()
{
    LIF_SendSbc(HDLC_GENERIC_ID, ME_ID, MY_DEF_LLD, REGULAR_MAIL);
}
#endif

/*
 *	The iqueue routine for isdn.
 */
void isdn_iqueue (hwidbtype *idb, paktype *p)
{

    if (SwitchType == NO_ISDN_SWITCH) {
#ifdef GLOBAL_DEBUG
	buginf("\nISDN Error: isdn_iqueue() called without a switch type");
#endif
	/* Not ready for it, toss it */
	retbuffer(p);
    } else {
	GET_TIMESTAMP(idb->lastinput); /* remember time of last input */
	if (pak_enqueue(FROM_DRIVER, p) == NULL)
	    datagram_done(p);
    }
}

/*
 * isdn_remove_v120
 * This should be called when a clear on the d channel is done.
 * It will stop all the v120 sessions on any of the D channels
 */
static void isdn_remove_v120 (int dsl)
{
    hwidbtype *d_idb, *b_idb;
    int channel;

    d_idb = isdn_idblist[dsl];
    if (d_idb == NULL)
	return;
    for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
	b_idb = d_idb->isdn_info->b_chan_idb[channel];
	if (b_idb) {
	    if (b_idb->isdn_info->v120info)
		reg_invoke_v120_stop_interface(b_idb);
	}
    }
}
    
/*
 *	This is called whenever we have nothing else to do.  It will check
 *  to see if there is anything new for us, and if not, will block.
 */
void UserIdle (void)
{
    paktype	*p;
    struct br_msg *b;
    BYTE donesomething;		/* TRUE if we need to let Tworks in again */
    MAILBOX_ID LIF_ID_To_MB(INT16);
    int ces;
    isdn_bri_t *isdn_info;
    extern TIMER_BLOCK *ActiveTimers; /* Head of active timer list */
    extern int isdn_pu_establish;
    enum Chan bchan;
    enum Chan channel;
    isdn_l1_primitive_t *layer1_msg;
    uchar dsl;
    hwidbtype *b_idb = NULL;

    donesomething = FALSE;
    ces = 0;
    layer1_msg = NULL;
    pri_phys_ind = FALSE;

    while (IsdnL1Q && !QUEUEEMPTY(FROM_PHYSICAL_LAYER)) {
	layer1_msg = p_dequeue(FROM_PHYSICAL_LAYER);
	if (layer1_msg != NULL) {
	    dsl = layer1_msg->dchan_num; 
	    if (detail_debug)
		buginf("\nISDN BRI Recvd L1 prim %d bri %d state was %d",
		       layer1_msg->prim, dsl, line_status[dsl]);
	    switch (layer1_msg->prim) {
	    case ISDN_PH_ACT_IND :
		switch (line_status[dsl]) {
		case IF_ACTIVATING :
		case IF_DOWN :
		    line_status[dsl] = IF_ACTIVE; 
		    isdn_phys_ind(isdn_idblist[dsl], TRUE, dsl);
		    donesomething = TRUE;
		    if (isdn_debug)
			buginf("\nISDN %s: L1 is IF_ACTIVE", 
                               DSL_TO_NAME(dsl));
		    /*
		     * Restart the code if powerup. 
		     * Sends a NL_EST_REQ to get it going.
		     */

                    if (is_pri)
                        Pri_Activate_L2(dsl);
                    else if (isdn_pu_establish)
			isdn_powerup(dsl);
		    break;
		case IF_ACTIVE :
		    if (isdn_debug)
			buginf("\nISDN %s: L1 ERR IF_ACTIVE", 
                               DSL_TO_NAME(dsl));
		    break;
		case IF_ACTIVE_EI1 :
		    line_status[dsl] = IF_ACTIVE;
		    donesomething = TRUE;
		    if (isdn_debug)
			buginf("\nISDN %s: L1 is IF_ACTIVE", 
                               DSL_TO_NAME(dsl));
		    break;
		default :
		    line_status[dsl] = IF_ACTIVE; 
		    isdn_phys_ind(isdn_idblist[dsl], TRUE, dsl);
		    donesomething = TRUE;
		    if (isdn_debug)
			buginf("\nISDN %s: L1 is IF_ACTIVE", 
                               DSL_TO_NAME(dsl));
		    /*
		     * Restart the code if powerup. 
		     * Sends a NL_EST_REQ to get it going.
		     */
                    if (is_pri)
                        Pri_Activate_L2(dsl);
                    else if (isdn_pu_establish)
			isdn_powerup(dsl);
		    break;
                }
		break;
	    case ISDN_PH_DEACT_IND :
		isdn_remove_v120(dsl);
		switch (line_status[dsl]) {
		case IF_ACTIVATING :
		case IF_ACTIVE :
		case IF_ACTIVE_EI1 :
		    line_status[dsl] = IF_DOWN;
		    isdn_phys_ind (isdn_idblist[dsl], FALSE, dsl);

                    if(is_pri){
                      CC_CHAN_set_status_inserv(dsl);
                      pri_phys_ind = TRUE;
                    }
                    else{
		      L2_ResetLink (dsl);
		      /*
		       * Adding the call to shutdown here. It is actually a 
                       * race condition and can cause Call Control Blocks 
                       * (CCBs) to get lost.  Especially true when clear 
                       * interface commands are done and Layer 2 may not come 
                       * up real fast, CCBs munged.
		       */
		      CCBRI_ShutDown(dsl);
                    }

		    donesomething = TRUE;
		    if (isdn_debug)
			buginf("\nISDN %s: Physical layer is IF_DOWN",
                               DSL_TO_NAME(dsl));
		    break;
		case IF_DOWN :	/* We were down already. */
		    if (isdn_debug)
			buginf("\nISDN %s: L1 persistent Deactivated",
			       DSL_TO_NAME(dsl));
		    if (SwitchType == VN2_STYPE || SwitchType == VN3_STYPE ||
			SwitchType == BRI_1TR6_STYPE || 
			SwitchType == BRI_NET3_STYPE) {
			LM_PersDeact(dsl);
			LIF_SendPkt (L2P_LAPD,ME_ID,0, L2_ID, PERSISTENT_DEACT,
				     dsl, 0, 1, 0,
				     0, 0, FALSE, NULL,
				     NOW, REGULAR_MAIL);
		    }
		    break; 
		default :
		    if (isdn_debug)
			buginf("\nISDN %s: L1 status error in line_status",
			       DSL_TO_NAME(dsl));
		    break;
                }
		break;
	    case ISDN_MPH_EI1_IND :
		switch (line_status[dsl]) {
		case IF_ACTIVATING :
		case IF_ACTIVE :
		    line_status[dsl] = IF_ACTIVE_EI1;
		    /* and fall thru */
		case IF_ACTIVE_EI1 :
		    if (isdn_debug)
			buginf("\nISDN %s: Recvd MPH_EI1_IND from L1",
			       DSL_TO_NAME(dsl));
		    break;
		case IF_DOWN :	/* EI1 is not meaningful when L1 is down */
		    break;
		default :
		    if (isdn_debug)
			buginf("\nISDN %s: L1 status error in line_status",
			       DSL_TO_NAME(dsl));
		    break;
                }
		donesomething = TRUE;
		break;
	    case ISDN_MPH_EI2_IND :
		switch (line_status[dsl]) {
		case IF_ACTIVATING :
		case IF_ACTIVE :
		case IF_ACTIVE_EI1 :
		    line_status[dsl] = IF_ACTIVE;
		    if (isdn_debug)
			buginf("\nISDN %s: Recvd MPH_EI2_IND from L1",
			       DSL_TO_NAME(dsl));
		    donesomething = TRUE;
		    break;
		case IF_DOWN :
		    if (isdn_debug)
			buginf("\nISDN %s, EI2 Received when IF is "
			       "DOWN - Error", DSL_TO_NAME(dsl));
		    break;
		default :
		    if (isdn_debug)
			buginf("\nISDN %s: L1 status error in line_status",
			       DSL_TO_NAME(dsl));
		    break;
                }
		break;
	    case ISDN_MPH_CONNECT_IND :
		if (isdn_debug)
		    buginf("\nISDN %s: Recvd MPH_IIC_IND from L1", 
                           DSL_TO_NAME(dsl));
		/*
		 * To be implemented Later, when the loop current
		 * detect is enabled at the driver.
		 */
		break;
	    case ISDN_MPH_DISCONNECT_IND :
		if (isdn_debug)
		    buginf("\nISDN %s: Recvd MPH_IID_IND from L1", 
                           DSL_TO_NAME(dsl));
		/*
		 * To be implemented Later, when the loop current
		 * detect is enabled at the driver.
		 */
		break;
	    case ISDN_MPH_ACT_IND :
		if (isdn_debug)
		    buginf("\nISDN %s: Recvd MPH_ACT_IND from L1",
                           DSL_TO_NAME(dsl));
		/*
		 * All actions have been taken when PH_ACT_IND
		 * was received. Hence don't do anything now.
		 */
		break; 
	    case ISDN_MPH_DEACT_IND :
		if (isdn_debug)
		    buginf("\nISDN %s: Recvd MPH_DEACT_IND from L1",
                           DSL_TO_NAME(dsl));
		/*
		 * All actions have been taken when PH_ACT_IND
		 * was received. Hence don't do anything now.
		 */
		break;
	    default :
		if (isdn_debug)
		    buginf("\nISDN %s: Recvd Unknown Prim %d from L1",
		           DSL_TO_NAME(dsl), layer1_msg->prim);
		break;
	    } /* case */	
	    p_enqueue(IsdnL1PrimPoolQ, layer1_msg);
	} /* if layer1_msg != NULL */
    }


    /* look for commands from the router */
    while ((b = dequeue(FROM_ROUTERSW)) != NULL) {
	if (!is_pri) {
	    for (dsl = 0; dsl < MAX_DSL; dsl++) {
		if ((isdn_idblist[dsl] != NULL) && ((isdn_idblist[dsl] == b->idb) ||
		    (isdn_idblist[dsl]->isdn_info->b1_chan_idb == b->idb) ||
		    (isdn_idblist[dsl]->isdn_info->b2_chan_idb == b->idb)))
		    break;
	    }
	    if (dsl == MAX_DSL) {
		buginf("\nISDN %s: Error: Recvd call/hangup from dialer on an "
		       "Illegal Interface", b->idb->hw_short_namestring);
                enqueue(FOR_ROUTER_MSGS, b);
		return;
	    }
	} else {
            /*
             * If there is any existing calls connected and the
             * controller is deconfigured we have a race condition
             * where the isdn_info structure might have been
             * pulled out from underneath us. This check here
             * avoids a crash. CSCdi37341.
             */
            if (b->idb->isdn_info == NULL) {
		buginf("\nISDN %s: Error: Recvd call/hangup from dialer and isdn_info is null.", b->idb->hw_short_namestring);
                enqueue(FOR_ROUTER_MSGS, b);
		return;
            }
	    dsl = b->idb->isdn_info->dsl;
	}

	switch(b->message) {
	case ISDN_CALL:
	    if (isdn_debug) {
	       buginf("\nISDN %s: Event: Call to %s at %d Kb/s",
		      DSL_TO_NAME(dsl), b->number, b->speed);
	    }
	    /*
	     * Don't process call if we're in an error condition.
	     * Put in for Bridging Measure.
	     */
	    if (line_status[dsl] != IF_ACTIVE_EI1) {
		/*
		 *  Activate now.
		 */
		if (line_status[dsl] != IF_ACTIVE) {
		    if (isdn_idblist[dsl]) {
			hwidbtype *temphwidb;
		        temphwidb = isdn_idblist[dsl];
			line_status[dsl] = IF_ACTIVATING;
			
			if (isdn_debug)
			    buginf("\nISDN %s: Activating", DSL_TO_NAME(dsl));
			if (temphwidb->isdn_info &&
			    temphwidb->isdn_info->activate_line)
			    (*temphwidb->isdn_info->activate_line)(temphwidb);
		    }
		}
		
		if (is_pri) {
		    if (process_pri_call(b, dsl))
			buginf("\nISDN %s: Error: Cannot process the call "
                               "to %s", DSL_TO_NAME(dsl), b->number);
		} else {
		    if (process_bri_call(b->call_id, b->number, b->speed, dsl,
					 b->spc, b->voice_call))
			buginf("\nISDN %s: Error: Cannot process the call "
                               "to %s", DSL_TO_NAME(dsl), b->number);
		}
		donesomething = TRUE;
	    }
	    break;
	    
	case ISDN_HANGUP:
	    if (!is_pri) {
		/*
		 * added to make the second tei work for dms and ni1, have
		 * to figure out the ces, just in case a call fails.
		 * Also, have to know which B-channel to disconnect.
		 */
		b_idb = isdn_idblist[dsl]->isdn_info->b1_chan_idb;
		isdn_info = b_idb->isdn_info;
		if (b->call_id == isdn_info->call_id) {
		    ces = isdn_info->ces;
		    isdn_info->ces = SIGNALING_CES;
		    bchan = B1;
		} else {
		    b_idb = isdn_idblist[dsl]->isdn_info->b2_chan_idb;
		    isdn_info = b_idb->isdn_info;
		    if (isdn_info->call_id == b->call_id) {
			ces = isdn_info->ces;
			isdn_info->ces = SIGNALING_CES;
			bchan = B2;
		    } else {
			b_idb = isdn_idblist[dsl];
			ces = SIGNALING_CES;
			bchan = ISDN_Dchan;
		    }
		}
	    } else {
		/* 
		 * We are a PRI interface and handle the B's differently.
		 * Let's see if this call_id matches the
		 * D-channel idb call_id.
		 */
		if (isdn_idblist[dsl] == NULL)
		    break;
		isdn_info = isdn_idblist[dsl]->isdn_info;
		if (isdn_info == NULL)
		    break;
		
		if (b->call_id == isdn_info->call_id) {
		    ces = isdn_info->ces;
		    isdn_info->ces = SIGNALING_CES;
		    isdn_info->in_use = FALSE;
		    bchan = ISDN_Dchan;
		}

		/*
		 * Now let's check all the B-channels.
		 */
		for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
		    b_idb = isdn_idblist[dsl]->isdn_info->b_chan_idb[channel];
		    if (b_idb == NULL)
			continue;
		    isdn_info = b_idb->isdn_info;
		    if (isdn_info == NULL)
			continue;
		    if (b->call_id == isdn_info->call_id) {
			ces = isdn_info->ces;
			isdn_info->ces = SIGNALING_CES;
			bchan = isdn_info->chantype;
			break;
		    }
		}
	    }

	    if ((SwitchType != BRI_DMS100_STYPE) &&
		(SwitchType != BRI_NI1_STYPE))
		ces = SIGNALING_CES; /* not dms or ni1 */

	    if (isdn_debug) {
		if ((SwitchType == BRI_DMS100_STYPE) ||
		    (SwitchType == BRI_NI1_STYPE)) 
		    buginf("\nISDN %s: Event: Hangup call to call id 0x%x "
		           " ces = %d", DSL_TO_NAME(dsl), b->call_id, ces);
		else
		    buginf("\nISDN %s: Event: Hangup call to call id 0x%x ",
		           DSL_TO_NAME(dsl), b->call_id);
	    }
	    if (!is_pri)
		if (b_idb->isdn_info && b_idb->isdn_info->disable_bri_channel)
		    (*b_idb->isdn_info->disable_bri_channel)(b_idb);
	    if (process_disconnect(b->call_id, NORMAL_CLEARING, 0x81, ces,dsl, b->spc))
		buginf("\nISDN %s: Error: Cannot process a disconnect "
		       "(call id = 0x%x)", DSL_TO_NAME(dsl), b->call_id);
	    donesomething = TRUE;
	    break;
			
	default:
	    buginf("\nISDN %s: ISDN Software Error: Unknown message from call"
	           "control (1, %d) ", DSL_TO_NAME(dsl), b->message);
	    break;
	}
            enqueue(FOR_ROUTER_MSGS, b);
    }

    /* look for frames from the driver */
    while ((p = pak_dequeue(FROM_DRIVER)) != NULL) {
	process_rxdata(p);
	donesomething = TRUE;
    }

    /* look for commands from the modem code */
    while ((b = dequeue(FROM_MODEM)) != NULL) {
            process_modem_command(b);
	    donesomething = TRUE;
    }

    /* check for expired timers */
    if (ActiveTimers != NULL && AWAKE(ActiveTimers->ExpireTime)) {
	TickHandler();
	donesomething = TRUE;
    }

    /*
     *	If we have done something, we must return to the T'works
     *	code in case they have to take an action based on ours.
     */
    if (!donesomething) {
	if (ActiveTimers) {
	    COPY_TIMESTAMP(ActiveTimers->ExpireTime, isdn_p1_expiration_time);
	    edisms((blockproc *)isdn_p, 0);
	} else {
	    edisms((blockproc *)isdn_p2, 0);
	}
    }
}
/*
 * We have just dequeued a message from the modem code.
 *
 */
static void process_modem_command (struct br_msg *b)
{
    int ces;
    isdn_bri_t *isdn_info, *d_isdn_info;
    ushort bchan, channel, cause, modem_err;
    int dsl;
    hwidbtype *b_idb, *d_chan_idb;
    dialerdbtype *ddb;
    boolean bchan_valid;
    cause = 0;
    modem_err = 0;

    if ((b->idb->isdn_info == NULL) || 
        (isdn_idblist[b->idb->isdn_info->dsl] == NULL)) {
        b->next = NULL;           /* just in case */
        b->message = 0;
        b->call_id = 0;
        b->number = NULL;
        b->ces = 0;
        b->idb = NULL;
        b->bchan = 0;
        enqueue(FOR_MODEM_MSGS, b);
        if (isdn_debug)          
           buginf("\nISDN: MODEM ERROR: isdn_info is NULL");
        return;
    }
    dsl = b->idb->isdn_info->dsl;
    isdn_info = NULL;
    ces = 0;
    channel = 0;
    /*
     * If the event is VOICE_ISDNCALL we don't have a B-channel
     * yet so don't use it.
     */
    bchan_valid = FALSE;
    bchan = b->bchan;
    if (bchan != 0xFFFF) {
        b_idb = isdn_idblist[dsl]->isdn_info->b_chan_idb[bchan];
        /*
         * b_idb could have been nulled out elsewhere - 
         * which was the case for CSCdi88556 - so check it.
         */
        if (!b_idb) {
            b->next = NULL;           /* just in case */
            b->message = 0;
            b->call_id = 0;
            b->number = NULL;
            b->ces = 0;
            b->idb = NULL;
            b->bchan = 0;
            enqueue(FOR_MODEM_MSGS, b);
            if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
               buginf("\nISDN %s: MODEM ERROR: b_idb is NULL",
                      DSL_TO_NAME(dsl));
            return;
        }
        isdn_info = b_idb->isdn_info;
        ces = isdn_info->ces;
        bchan_valid = TRUE;
    } else {
        b_idb = NULL;
    }
    switch(b->message) {

    case VOICE_IDLE:
        break;

    case VOICE_ANS:
        /*
         * Okay. This is pri so I already sent the modem
         * code the B channel. I can just select it from
         * the args passed in.
         */
        process_modem_conn(CALL_ACCEPT, b->call_id, bchan, dsl);
        isdn_info->in_use = TRUE;
        modem_err = reg_invoke_modem_report(isdn_idblist[dsl], b->call_id, 
                                            ces, bchan, DEV_CONNECTED, NULL, 
                                            NULL, cause);
        if (modem_err) {
           if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
              buginf("\nISDN %s: VOICE_ANS: MODEM ERROR %x: bchan %d,"
                     " call id %x",
                     DSL_TO_NAME(dsl), modem_err, bchan, b->call_id);
           process_rejectcall(b->call_id, CALL_REJECTED, dsl);
	}
        break;

    case VOICE_ISDNCALL:
        b->speed = ISDN_SPEED_64;
        b->voice_call = TRUE;
        if (process_pri_call(b, dsl)) {
           if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
              buginf("\nISDN %s: Error: Cannot process modem call to %s",
                     DSL_TO_NAME(dsl), b->number);
           break;
        } 
        d_chan_idb = isdn_idblist[dsl];
	ddb = get_dialerdb(d_chan_idb);
        d_isdn_info = d_chan_idb->isdn_info;
        isdn_decrement_bchan(d_isdn_info);   /* one less available */
        dialer_increment_call(ddb);     /* one more B chan. in use */
        break;

    case VOICE_DIGIT:
        break;

    case VOICE_HANGUP:
        /*
         * We are a PRI interface and handle the B's differently.
         * Let's see if this call_id matches the
         * D-channel idb call_id.
         */
        if (isdn_idblist[dsl] == NULL) {
           if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
              buginf ("\nISDN %s: MODEM ERROR: VOICE_HANGUP - no idb", 
                       DSL_TO_NAME(dsl));
           break;
        }
        /*
         * If the passed in b-channel is valid let's use it
         * instead of searching through all the idb's to find
         * the matching callid.
         */
        if (bchan_valid == TRUE) {
            if (b->call_id == isdn_info->call_id) {
                ces = isdn_info->ces;
                isdn_info->ces = SIGNALING_CES;
            }
        } else {
            /*
             * The passed in b-channel is not valid so we need to
             * match call_id's. This can occur if the modem goes
             * on hook before we completely connect.
             */
            for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
                b_idb = isdn_idblist[dsl]->isdn_info->b_chan_idb[channel];
                if (b_idb == NULL)
                    continue;
                isdn_info = b_idb->isdn_info;
                if (isdn_info == NULL)
                    continue;
                if (b->call_id == isdn_info->call_id) {
                    ces = isdn_info->ces;
                    isdn_info->ces = SIGNALING_CES;
                     bchan = isdn_info->chantype;
                    break;
                }
            }
        }
        if ((SwitchType != BRI_DMS100_STYPE) && (SwitchType != BRI_NI1_STYPE))
            ces = SIGNALING_CES; /* not dms or ni1 */
        if (isdn_debug || isdn_debug_ok(q931_debug, dsl)) {
            if ((SwitchType == BRI_DMS100_STYPE) ||
                (SwitchType == BRI_NI1_STYPE))
                 buginf("\nISDN %s: Event: Hangup call to call id 0x%x "
                        " ces = %d", DSL_TO_NAME(dsl), b->call_id, ces);
            else
                 buginf("\nISDN %s: Event: Hangup call to call id 0x%x ",
                        DSL_TO_NAME(dsl), b->call_id);
        }
        if (channel != ISDN_MAX_CHAN) {
            if (process_disconnect(b->call_id, NORMAL_CLEARING, 0x81, ces,dsl, b
 ->spc)) {
               if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
                  buginf("\nISDN %s: Error: Cannot process a disconnect "
                         "(call id = 0x%x)", DSL_TO_NAME(dsl), b->call_id);
               break;
            }
            /*
             * This is part of the code from isdn_call_disconnect.
             * We need to clean up a bunch of things here.
             */
            d_isdn_info = isdn_info->d_chan_idb->isdn_info;
            isdn_info->call_is_voice = FALSE;
            ddb = get_dialerdb(b_idb);
            ddb->dialer_spoofing_int = FALSE;
            ddb->dialer_spoofing_prot = FALSE;
            ddb->dialer_lineup = FALSE;
            TIMER_STOP(ddb->dialer_idle_timer);
            TIMER_STOP(ddb->dialer_fast_idle_timer);
            ddb->dialer_stats = NULL;
            dialer_set_number_called(ddb, NULL);
            bzero(ddb->remote_name, MAXSTR);
            bzero(ddb->dial_reason, MAXSTR);
            d_chan_idb = isdn_idblist[dsl];
            ddb = get_dialerdb(d_chan_idb);
            isdn_increment_bchan(d_isdn_info);   /* one more B available */
            dialer_decrement_call(ddb);
        } else {
           if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
              buginf ("\nISDN %s: MODEM Error: No B channel for disconnect ",
                      DSL_TO_NAME(dsl));
        }
        break;

    default:
        if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
           buginf("\nISDN %s: Software Error: Unknown message from modem"
                  "control (1, %d) ", DSL_TO_NAME(dsl), b->message);
        break;
    }

    b->next = NULL;           /* just in case */
    b->message = 0;
    b->call_id = 0;
    b->number = NULL;
    b->ces = 0;
    b->idb = NULL;
    b->bchan = 0;
    enqueue(FOR_MODEM_MSGS, b);
}

/*
 * Layer 1 has synchronized. Now it is time to send a SABME.
 *
 */
void
Pri_Activate_L2 (int Dsl_Id)
{
    ushort retcode;

    line_status[Dsl_Id] = IF_ACTIVE;
    pri_phys_ind = TRUE;
    if (SwitchType == PRI_DMS100_STYPE && !Network_side)
        return;
    retcode = LIF_SendPkt( L2P_LAPD, CC_ID, 0, L3_ID, NL_EST_REQ, Dsl_Id,
			  Q931_SAPI, 1, 0, 0xFFFF, Q931_DSL_CHAN_ID, TRUE,
			  NULL, NOW, REGULAR_MAIL );
}

/*
 *	This is the MTEX task which gets frames from the state table and
 *  informs the router.
 */
void
Host_Start ()
{
    for (;;)
	process_rxstate();
}

/*
 *	This is the MTEX task which gets frames from the Layer 2 and is
 *  supposed to send them to the driver.
 */
void
L2D_Srq_Task ()
{
    EXT_MEM gen_addr;		/* Generic address of mail */
    PRIM_ID prim;
    paktype *p;
    sys_timestamp svclock;
    hwidbtype	*current_isdnidb; /* keep a local copy, used a lot */
#ifdef PAN
    ulong delta;
    TIMER_BLOCK *timer;
    extern TIMER_BLOCK *ActiveTimers; /* Head of active timerlist */
#endif


    /*
     * we have to use the system clock to be within =-5% accuracy in order
     * to pass homologation testing.
     */
    GET_TIMESTAMP(svclock);
    /* main loop */
    for (;;) {
	MTEX_ReadMail(HDLC_D_MB, &gen_addr);
	prim = (PRIM_ID)gen_addr.Addr;
	if (detail_debug)
	    buginf("\nRecvd from L2 ISDN %s state %d", 
                   DSL_TO_NAME(prim->Dsl_ID), line_status[prim->Dsl_ID]); 
	if(prim == NULL) {
	    if(isdn_debug)
		buginf("\nISDN Error: Rx a NULL primitive in L2D_Srq");
	    continue;
	}
	if(prim->Pkt == NULL) {
	    if(isdn_debug)
		buginf("\nISDN Error: Rx a NULL packet in L2D_Srq");
	    continue;
	}
	if(prim->BufType != PRIM_BTYPE) {
	    if(isdn_debug)
		buginf("\nISDN Error: Rx buf type %d "
		       "when expected a primitive", prim->BufType);
	    LIF_Dispose(prim);
	    continue;
	}

	/*
	 * If the interface isn't activated, attempt to bring it up.
	 * Added interface isn't ACTIVATING for Cabrillo homologation.
	 * Save the current isdn idb pointer, since it is used a lot.
	 */

	current_isdnidb = isdn_idblist[prim->Dsl_ID];
        if (current_isdnidb == NULL)
          continue;

	if ( (line_status[prim->Dsl_ID] != IF_ACTIVE) 
	    && (line_status[prim->Dsl_ID] != IF_ACTIVE_EI1) 
	    && (line_status[prim->Dsl_ID] != IF_ACTIVATING) ) {

	    if (isdn_idblist[prim->Dsl_ID]) {
		line_status[prim->Dsl_ID] = IF_ACTIVATING;
		if(isdn_debug)
		      buginf("\nISDN %s: Activating ", 
                             DSL_TO_NAME(prim->Dsl_ID));
		if (current_isdnidb->isdn_info &&
		    current_isdnidb->isdn_info->activate_line)
		    (*current_isdnidb->isdn_info->activate_line)(current_isdnidb);
	    }
	    GET_TIMESTAMP(last_act_attempt); /* added for Siemens tester */
#ifdef PAN
	    /*
	     *			(*current_isdnidb->reset)(current_isdnidb);
	     */
	    /* there is a 2 ms delay between SID PUP and Act Req */
	    /*
	     *		For VN2, we are required to wait out T3 before giving
	     *	up on the frame.  We add a 15 second timer just in case
	     *	there is a bug, ISDN will not be wedged.
	     */
	    if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
		GET_TIMESTAMP(svclock);
		edisms((blockproc *) isdn_p4, 0);
		/*
		 *		Now timegash all the timers, so they don't
		 *	expire right away.  Never gash by more than 40 ms.
		 */
		delta = ELAPSED_TIME(svclock);
		if(SwitchType != VN3_STYPE)
		    delta = min(delta, 40);
		for (timer = ActiveTimers ; timer != NULL ; timer= timer->Next)
		    TIMER_UPDATE(timer->ExpireTime, delta);
	    } else {
		TIMER_START(isdn_p1_expiration_time, 15 * ONESEC);
		edisms((blockproc *) isdn_p, 0);
	    }
#else /*PAN*/
	    TIMER_START(isdn_p1_expiration_time, 15 * ONESEC);
	    edisms((blockproc *) isdn_p, 0);
#endif /*PAN*/
	    if ( (line_status[prim->Dsl_ID] != IF_ACTIVE) &&
		(line_status[prim->Dsl_ID] != IF_ACTIVE_EI1) ) {
		GET_TIMESTAMP(last_act_attempt);
		if(isdn_debug) 
		    buginf("\nISDN %s: Could not bring up interface",
			   DSL_TO_NAME(prim->Dsl_ID));
		if (isdn_idblist[prim->Dsl_ID]->isdn_info->call_tbl)
		   isdn_carrier_timeout(isdn_idblist[prim->Dsl_ID],
			isdn_idblist[prim->Dsl_ID]->isdn_info->call_tbl, 
			isdn_idblist[prim->Dsl_ID]->isdn_info->call_tbl);
		goto cleanup;
	    }
	}

        if (L2_Discards[prim->Dsl_ID] > L2D_Task_Counter[prim->Dsl_ID]) {
           if (isdn_debug || isdn_debug_ok(q931_debug, prim->Dsl_ID))
              buginf("\nISDN %s: Event: Syncing Discards: L2 Discards %d", 
                 "L2D_Task Counter %d", DSL_TO_NAME(prim->Dsl_ID),
                 L2_Discards[prim->Dsl_ID], L2D_Task_Counter[prim->Dsl_ID]);
           L2D_Task_Counter[prim->Dsl_ID]++;
           goto cleanup;
        }

	if((p = getbuffer(prim->Pkt->HeadLen + prim->Pkt->InfoLen))
	   == NULL) {
	    current_isdnidb->counters.output_nobuffers++;
	    goto cleanup;
	}
	p->datagramsize = prim->Pkt->HeadLen + prim->Pkt->InfoLen;
	p->datagramstart =  p->network_start;
	p->if_output = current_isdnidb->firstsw;
	bcopy(prim->Pkt->Header, p->datagramstart, p->datagramsize);
	do_statistics(p, TX, prim->Dsl_ID);

	/*
	 * If we are running over the MIP card we need to
	 * call their board_encap procedure before soutput.
	 */
	if (current_isdnidb->board_encap)
	    if (!(*current_isdnidb->board_encap)
		(p,current_isdnidb)) {
		if (detail_debug)
		    buginf("\n   %s:ISDN PRI board encap error returned",
			   current_isdnidb->hw_namestring);
		retbuffer(p);
		return;
	    }
    datagram_out(p);

    cleanup:
	if(prim->FreePkt) {
	    LIF_FreePkt(prim->Pkt);
	}
	LIF_FreePrim(prim);
    }
    /*NOTREACHED*/
}

/*
 *	Handle a command from the router software.
 *
 *	Return value: -1 in case of failure, 0
 */
int
isdn_fromrouter (enum BriMsgType command,
		 ushort CallID, char *number,
		 uint speed, int ces, hwidbtype *idb, boolean spc)
{
    struct br_msg *msg;

    assert(sizeof(UINT16) == sizeof(CallID));

    if(SwitchType == NO_ISDN_SWITCH) {
#ifdef GLOBAL_DEBUG
	buginf("\nBRI Error: isdn_fromrouter() called without a switch type");
#endif
	return -1;
    }

    if ((msg = dequeue(FOR_ROUTER_MSGS)) == NULL) {
        buginf("\nBRI Error: isdn_fromrouter() msg dequeue NULL");
        return -1;
    }

    msg->next = NULL;		/* just in case */

    msg->message = command;
    msg->call_id = CallID;
    msg->number = number;
    msg->speed = speed;
    msg->ces = ces;
    msg->idb = idb;
    msg->spc = spc;
    msg->voice_call = idb->isdn_info->voice_call;
    msg->do_nsf = idb->isdn_info->do_nsf;
    msg->nsf_type = idb->isdn_info->nsf_type;

    enqueue(FROM_ROUTERSW, msg);
    return 0;
}

/*
 *      Handle a command from the modem software.
 *
 *      Return value: is a cause value
 */
static ushort
isdn_frommodem (hwidbtype * dchan_idb, ushort CallID, ushort ces, ushort bchan, 
				  ushort event, ushort cause, uchar *number, ushort encoding)
{
    struct br_msg *msg;

    assert(sizeof(UINT16) == sizeof(CallID));

    if(SwitchType == NO_ISDN_SWITCH) {
        buginf("\nBRI Error: isdn_frommodem() called without a switch type");
        return -1;
    }

    if ((msg = dequeue(FOR_MODEM_MSGS)) == NULL) {
        buginf("\nBRI Error: isdn_frommodem() msg dequeue NULL");
        return -1;
    }

    msg->next = NULL;           /* just in case */

    msg->message = event;
    msg->call_id = CallID;
    msg->number = number;
    msg->speed = ISDN_SPEED_64;
    msg->ces = ces;
    msg->idb = dchan_idb;
    msg->bchan = bchan;
    msg->spc = FALSE;
    msg->voice_call = TRUE;
    msg->encoding = encoding;

    enqueue(FROM_MODEM, msg);
    return 0;
}

/*
 *	Call the passed number.  Return -1 on failure, 0 on success.
 */
static int
process_bri_call (ushort Call_ID, char *number, uint speed, int dsl, 
		  boolean semi_perm_call, boolean voice_call)
{
    int i;
    BYTE *p;
    char *ptr;
    isdn_bri_t *isdn_info;
    int count;
    int length_index;
    boolean do_subaddress;
    int used_ces = 0;
    int ces;
    boolean spid_okay;

    if ((isdn_debug || isdn_debug_ok(q931_debug, dsl)) && voice_call)
        buginf ("\n process_bri_call making a voice_call.");
    count = 0;
    length_index = 0;
    do_subaddress = FALSE;
    ces = SIGNALING_CES;  /* default ces to use */
    assert(sizeof(UINT16) == sizeof(Call_ID));

    /*
     * Attempt to make a call, but the custom DMS100 requires a valid spid
     * before an outgoing call can be made.
     *
     * The following check is needed to determine which ces/tei to use. The
     * DMS and NI will make one call per TEI, if two spids are configured. If
     * only 1 spid is configured, then only one TEI will be used. All other switches
     * work fine the way just using SIGNALING_CES. We have at least 1 free B-chan.
     */
    if((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
        if (np_fit[dsl][1].spid[0] == '\0') {
            used_ces = 0;    /* the 2nd spid is not defined */
        } else {
            isdn_info = isdn_idblist[dsl]->isdn_info->b1_chan_idb->isdn_info;

            if(isdn_info->in_use)       /* check b1 idb useage */
                used_ces = isdn_info->ces;
            else {                      /* check the b2 idb usage */
                isdn_info = isdn_idblist[dsl]->isdn_info->b2_chan_idb->isdn_info;
                if(isdn_info->in_use)
                    used_ces = isdn_info->ces;
            }
        }
        /*
         * used_ces 0 - no channels in use
         *          1 - SIGNALING_CES
         *          2 - SIGNALING_CES + 1
         * ces is already set to SIGNALING_CES
         */
        if(used_ces == SIGNALING_CES)
            ces = SIGNALING_CES + 1;
    }

    /*
     * Okay, have now figured out which ces to use, now figure out if
     * the spid is valid or if it has been NACKed.
     */
    if ((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
        spid_okay = ccbfit_validspid(dsl, ces);
        if (!spid_okay) {
            if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
                buginf("\nISDN %s: Event: Invalid SPID, ces %d, cannot send "
                       "Setup", DSL_TO_NAME(dsl), ces);
            return -1;
        }
    }

    if((p = LIF_GetBuffer(LRG_INFO_BTYPE, TRUE)) == NULL)
	return -1;

    p[0] = HOST_CALL_REQUEST;	/* message type */
    p[1] = dsl;			/* DSL */
    p[2] = ces;

    p[3] = (Call_ID >> 8) & 0xff; /* MSB */
    p[4] = Call_ID & 0xff;	/* LSB */
    /*	p[5] = << len of info elements--will be filled in later >> */

    /* now start stuffing the information elements, starting at offset 6 */
    i = 6;

    /*
     *	First do the Private Info Elements
     */

    /*
     * Rather than try to use Telenetworks private info elements, we
     * only use public info elements.  That means it is up to us to
     * make sure the mandatory elements are there and are correct, but
     * if you use Telenetworks private info elements, there is no way
     * to request any B channel, they only pay attention to preferred
     * or exclusive.
     */
    p[5] = i - 6;

    /*
     *	Now the Public Information Elements (ie., those which require
     *  fields not available through Telenetworks private IE mechanism)
     */

    /*
     *	The Bearer Capability.  (Must do here, as T'works put out a
     *  mu-law encoding octet, which is surprising for digital data).
     *
     *	Also, we must deal with 56Kb links
     */

    if(SwitchType != BRI_1TR6_STYPE) {
        if (voice_call) {
            p[i++] = BEARER_CAPABILITY_IE;  /* Oct: 1   Information Identifier */
            p[i++] = 3;                /* Oct: 2   length */
            p[i++] = 0x80;      /* Oct: 3   CCITT coding, SPEECH */
            p[i++] = 0x80 | BEAR_RATE_64KBPS; /* Oct: 4   no ext, circuit mode, 64Kb/s */
            p[i++] = 0x80 | 0x20 | 2;   /* Oct: 5   u-law */
        } else {
	    if(speed == ISDN_SPEED_56 ) {
                p[i++] = BEARER_CAPABILITY_IE;
	        p[i++] = 4;		/* length */
	        p[i++] = 0x88;	/* CCITT coding, unrest. digital info */
	        p[i++] = 0x80 | BEAR_RATE_64KBPS; /* no ext, circ mode, 64Kb/s */
	        p[i++] = 0x20 | 1;	/* V.110 protocol */
	        p[i++] = 0x80 | 0x0f; /* 56 Kb/s */
	    } else {
	        p[i++] = BEARER_CAPABILITY_IE;
	        p[i++] = 2;		/* length */
	        p[i++] = 0x88;	/* CCITT coding, unrest. digital info */
	        p[i++] = 0x80 | BEAR_RATE_64KBPS; /* no ext, circ mode, 64Kb/s */
	    }
        }
    }

    /* The Channel ID */
    p[i++] = CHANNEL_ID_IE;
    p[i++] = 1;			/* length */
    p[i++] = 0x80 | 3;		/* preferred, any channel */;

    /* Terminal Capability */
    if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
	p[i++] = CODESET_SHIFT_IE | CODESET_6 | NON_LOCKING_SHIFT_MASK;
	p[i++] = TERMINAL_CAP_IE;
	p[i++] = 1;
	p[i++] = 0x80;		/* only legal value */
    }
    if ( (SwitchType == BRI_1TR6_STYPE) && semi_perm_call) {
        p[i++] = NETWORK_FACILITY_IE;
        p[i++] = 0x04; /* length */
        p[i++] = 0x00;
        p[i++] = SEMI_PERMANENT_LINK;
        p[i++] = ALL_SERVICES;
        p[i++] = NO_ADDITIONAL_INFO;
        p[i++] = NETWORK_FACILITY_IE;
        p[i++] = 0x04; /* length */
        p[i++] = 0x00;
        p[i++] = ACTIVATE_USE_OF_LINK; /* activate link */
        p[i++] = ALL_SERVICES;
        p[i++] = NO_ADDITIONAL_INFO;
    }

    /*
     * Now stuff the number we wish to call.  This ought to
     * go into a Called Party Number information element, but
     * the 5ESS insists on a Keypad. The basic-5ess switchtype
     * will not support the use of subaddress number. Whatever
     * is contained in the string will be sent out in Keypad. Do the same
     * for NI1, there are too many restrictions and  problems making sure
     * the Called Party Number is correctly configured. Been told that we
     * will have problems connecting calls. Not sure about this one yet. ????
     */
    if (SwitchType == BRI_5ESS_STYPE || SwitchType == BRI_NI1_STYPE) {
	/*
         * Keypad and calling party number
         */
	p[i++] = KEYPAD_FACILITY_IE;
	p[i++] = strlen(number);
	for (ptr = number ; *ptr ; p[i++] = *ptr++)
	    ;

        /*
         * Put in the Calling Party Number if it has been configured. Required
         * since the 5ESS is not a real FIT.
         */
        if((SwitchType == BRI_5ESS_STYPE) &&
           (isdn_calling[dsl].number[0] != '\0')) {
            p[i++] = CALLER_NUMBER_IE;
            p[i++] = 1 + strlen(isdn_calling[dsl].number);
            p[i++] = 0xC1;          /* local number, isdn numbering plan */
            for (ptr = isdn_calling[dsl].number ; *ptr ; p[i++] = *ptr++)
                ;
        }
    } else {
        /*
         * Put in the Calling Party Number if it has been
         * configured. This is only for Australia useage here.
         */
        if((SwitchType == BRI_TS013_STYPE) &&
           (isdn_calling[dsl].number[0] != '\0')) {
            p[i++] = CALLER_NUMBER_IE;
            p[i++] = 1 + strlen(isdn_calling[dsl].number);
            p[i++] = 0xA1;          /* national number, isdn numbering plan */
            for (ptr = isdn_calling[dsl].number ; *ptr ; p[i++] = *ptr++)
                ;
        }

	/* The Called Party Number and Called Party Subaddress
	 * the subaddress is optional and is separated by a colon
	 * in the called number string, as shown below
	 * dialer map ip x.x.x.x 12345:6789
	 *                             ^subaddress digits
	 */
	p[i++] = CALLED_NUMBER_IE;
	length_index = i;	/* save the index in case a subaddr */
	p[i++] = 1 + strlen(number); /* type of number + number */
	if(SwitchType == BRI_TS013_STYPE)
	    p[i++] = 0x81;	/* ISDN number type and plan */
	else
	    p[i++] = 0x80;	/* unknown number type and plan */
	for (ptr = number ; *ptr ; p[i++] = *ptr++ ) {
	    if(number[count] == SUBADDR_SEPARATOR) {
		do_subaddress = TRUE; /* have to process subaddress */
		break;
	    }
	    count++;
	}
    }
    if(do_subaddress) {
	/* adjust CALLED Party number len, plus number type/plan IE */
	p[length_index] = count + 1;
	p[i++] = CALLED_SUB_ADDR_IE;

	/*
	 * Figure out the number for the subaddress length. It is the
	 * length of the number, minus the subaddress separator, minus
	 * the length of the Called Party Number, plus the Type of 
	 * subaddr IE, plus the AFI (Authority Format Id) IE
	 */

	p[i++] = (strlen(number) - SUBADDR_SEP_LEN - count) + 2;
	p[i++] = 0x80;	/* Type of subaddress is NSAP, even indicator */
	p[i++] = 0x50;	/* AFI code x50 indicates sub-address is IA5 chars */
	while (number[++count]) {
	    p[i++] = number[count];
	}
    }

    if(SwitchType == BRI_1TR6_STYPE) {
	/* shift to code set 6 */
	p[i++] = CODESET_SHIFT_IE + ADD_TRANSMIT_ATTR_IE + CODESET_6;

	/* service indicator */
	p[i++] = SERVICE_INDICATOR_IE;
	p[i++] = 02;		/* length */
	if (speed == ISDN_SPEED_56) {
	    p[i++] = 07;	/* 64k */
	    p[i++] = 0xa9;	/* additional info set for 56k */
	} else {
	    p[i++] = 07;	/* 64k */
	    p[i++] = 00;	/* additional info */
	}
    }

    if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
	/* this is where VN2 wants the speed */
	if(speed == ISDN_SPEED_56) {
	    p[i++] = LOW_LAYER_COMPAT_IE;
	    p[i++] = 4;
	    p[i++] = 0x88;	/* CCITT encoding, unrestricted digital info */
	    p[i++] = 0x90;	/* circuit mode, 64Kb/s */
	    p[i++] = 0x21;	/* V.110 protocol */
	    p[i++] = 0x8f;	/* 56 Kb/s */
	}

	/* High layer compatibility */
	p[i++] = HIGH_LAYER_COMPAT_IE;
	p[i++] = 2;
	p[i++] = 0xc1;
	p[i++] = 0xff;
    }

    /* added for Norway */
    if (SubSwitchType == BRI_NWNET3_STYPE) {
	/* High layer compatibility */
	p[i++] = HIGH_LAYER_COMPAT_IE;
	p[i++] = 2;
	p[i++] = 0xd1;
	p[i++] = 0xff;
    }

    /*
     * The SENDING_COMPLETE IE says that everything is
     * in this SETUP message. Added at the end so can still test
     * with the NTT switch simulator. Required for Taiwan
     * homologation.
     */
    if ((SwitchType == VN3_STYPE) ||
        (isdn_sys_constants[dsl].sending_complete))
	p[i++] = 0xA1;		/* Sending Complete */

    if(LIF_SendBuf(HOST_ID, CC_ID, i, LRG_INFO_BTYPE, p, HOST_CALL_REQUEST,
		   NOW, REGULAR_MAIL) != NOERR)
	return -1;
	
    return 0;
}

/*
 *	Call the passed number.  Return -1 on failure, 0 on success.
 *  For Primary Rate interfaces
 */
static int
process_pri_call (struct br_msg *b, int dsl)
{
    int i, ix;
    BYTE *p;
    char *ptr, *number;
    int count;
    int called_num_len_ie;
    int string_len;
    int numb_plan;
    int numb_type;
    char intl_number[DIALER_MAX_CALLER+1]; /* used for 4ess intl calls */
    ushort call_id;
    uint speed, nsf_type;
    boolean do_subaddress, voice_call, do_nsf;

    call_id = b->call_id;
    number = b->number;
    speed = b->speed;
    voice_call = b->voice_call;
    assert(sizeof(UINT16) == sizeof(call_id));

    if ((isdn_debug || isdn_debug_ok(q931_debug, dsl)) && voice_call)
        buginf ("\n process_pri_call making a voice_call.");

    do_nsf = b->do_nsf;
    nsf_type = 0; 
    if (do_nsf == TRUE)
        nsf_type = b->nsf_type;

    count = 0;
    called_num_len_ie = 0;
    do_subaddress = FALSE;

    /* attempt to make a call */
    if((p = LIF_GetBuffer(LRG_INFO_BTYPE, TRUE)) == NULL)
	return -1;

    p[0] = CALL_OUTGOING;	/* pri call request */
    p[1] = dsl;			/* DSL */
    p[2] = SIGNALING_CES;	/* all other switches */

    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;	/* LSB */
    /*	p[5] = << len of info elements--will be filled in later >> */

    /* now start stuffing the information elements, starting at offset 6 */
    i = 6;

    /*
     *	First do the Private Info Elements
     */

    /*
     * Rather than try to use Telenetworks private info elements, we
     * only use public info elements.  That means it is up to us to
     * make sure the mandatory elements are there and are correct, but
     * if you use Telenetworks private info elements, there is no way
     * to request any B channel, they only pay attention to preferred
     * or exclusive.
     */

    if (voice_call) {
        p[i++] = BEARER_CAPABILITY_IE;  /* Oct: 1   Information Identifier */
        p[i++] = 4;         /* Oct: 2   length */
        p[i++] = 0x80;      /* Oct: 3   CCITT coding, SPEECH */
        p[i++] = 0x00;		/* circuit mode */
        p[i++] = BEAR_RATE_64KBPS; /* 64Kb/s */
        if (b->encoding == U_LAW_ENCODING)
            p[i++] = 0x80 | 0x20 | 2;   /* Oct: 5   u-law */
		else
            p[i++] = 0x80 | 0x20 | 3;   /* Oct: 5   a-law */
    } else {
        if(speed == ISDN_SPEED_56) {
	    p[i++] = BEARER_CAPABILITY_IE;
	    p[i++] = 5;		/* length */
	    p[i++] = 0x08;		/* xfer */
	    p[i++] = 0x00;		/* circuit mode */
	    p[i++] = BEAR_RATE_64KBPS; /* 64Kb/s */
	    p[i++] = 0x20 | 1;      /* Layer 1, V.110 protocol */
	    p[i++] = 0x0f;		/* sync, 56 Kb/s */
        } else {
	    p[i++] = BEARER_CAPABILITY_IE;
	    p[i++] = 3;		/* length */
	    p[i++] = 0x08;		/* xfer */
	    p[i++] = 0x00;		/* circuit mode */
	    p[i++] = BEAR_RATE_64KBPS; /* 64Kb/s */
        }
    }

    /* The Channel ID for PRI */
    p[i++] = CHANNEL_ID_IE;
    p[i++] = 3;			/* length */
    p[i++] = 0x00;		/* preferred = 0, exclusive = 1 */
    p[i++] = B_CHANNEL_UNITS;	/* channel type */
    p[i++] = 0x00;		/* request channel 0, the code will assign */

    /*
     * If we are running 4ESS let's check on our NSF.
     */

    if ((SwitchType == PRI_4ESS_STYPE) && do_nsf) {
        p[i++] = NETWORK_FACILITY_IE;
        p[i++] = 2;
        p[i++] = 0x80;          /* Network ID 0 for now */
        p[i++] = nsf_type;      /* Facility coding SDN */
    }

    /*
     * Put in the Calling Party Number if it has been
     * configured. This is only for Australia useage.
     */
    if ((SwitchType == PRI_TS014_STYPE) &&
        (isdn_calling[dsl].number[0] != '\0')) {
            p[i++] = CALLER_NUMBER_IE;
            p[i++] = 2 + strlen(isdn_calling[dsl].number);
            p[i++] = 0x02;          /* national number */
            p[i++] = 0x01;          /* isdn numbering plan */
            for (ptr = isdn_calling[dsl].number ; *ptr ; p[i++] = *ptr++)
                ;
    }

    /* The Called Party Number and Called Party Subaddress
     * the subaddress is optional and is separated by a colon
     * in the called number string, as shown below
     * dialer map ip x.x.x.x 12345:6789
     *                             ^subaddress digits
     */
    p[i++] = CALLED_NUMBER_IE;
    called_num_len_ie = i;		/* save the index in case a subaddr */
    /*
     * number length + 1 + number type + number plan
     */
    string_len = strlen(number);
    ptr = intl_number;
    strcpy (ptr, number);
    if (SwitchType == PRI_NET5_STYPE || SwitchType == PRI_NTT_STYPE) {
        numb_type = 0x00;          /* unknown number */
        numb_plan = 0x00;          /* unknown numbering plan */
    } else {
 	/*
	 * For AT&T 4ESS have to check to see if it is a national number or an
	 * international number. Use the number of digits dialed and look for
	 * a 011 in the first few digits. Have to know if have a subaddress
	 * because that would make the length look longer than it really is.
	 * A national number max is 11 digits, 1+area code+7 digits
         *
	 * For the 4ESS we will not send the 011 but still set type to intl.
         *
	 * Australia (TS014) uses a 0011 prefix for international calls.
	 */
	numb_type = NAT_NUMBER;                 /* national number */
	numb_plan = ISDN_NUMB_PLAN;             /* isdn numbering plan */
        if (((SwitchType == PRI_4ESS_STYPE) || (SwitchType == PRI_TS014_STYPE)) 
            && (string_len >= 4)) {
            if ((number[0] == '0') && (number[1] == '1') && (number[2] == '1')) {

                /*
                 * Got a 011 change the number type to international.
                 */
                if (SwitchType == PRI_4ESS_STYPE) {
                    numb_type = INTL_NUMBER;
                    string_len -= 3;
                    for (ix = 0; ix < string_len; ix++) {
                        intl_number[ix] = intl_number[ix+3];
                    }
                } else {
                    numb_type = UNKNOWN_NUMBER;
                }
            } else if ((number[1] == '0') && (number[2] == '1') && (number[3] == '1')) {
                /*
                 * Got a x011, change the number type to international.
                 */
                if (SwitchType == PRI_4ESS_STYPE) {
                    numb_type = INTL_NUMBER;
                    string_len -= 3;
                    for (ix = 1; ix < string_len; ix++) {
                        intl_number[ix] = intl_number[ix+3];
                    }
                } else {
                    numb_type = UNKNOWN_NUMBER;
                }
            }
        }
    }
    p[i++] = string_len + 2;
    p[i++] = numb_type;              /* called party number type */
    p[i++] = numb_plan;             /* called party numbering plan */

    for (ptr = intl_number ; *ptr ; p[i++] = *ptr++ ) {
	if(number[count] == ':') {
	    do_subaddress = TRUE; /* have to process subaddress */
	    break;
	}
	count++;
    }

    if(do_subaddress) {
	p[called_num_len_ie] = count + 2; /* adjust CALLED Party number len */
	p[i++] = CALLED_SUB_ADDR_IE;
	called_num_len_ie = i;     /* save the value */
	p[i++] = strlen(number) - count + 1; /* subaddr length */
	p[i++] = 0x80;	/* NSAP, even */
	p[i++] = 0x50;	/* AFI code x50 indicates sub-address is IA5 chars */
	while (number[++count]) {
	    p[i++] = number[count];
	}
    }

    /* end of privates, now can fill in the length */
    p[5] = i - 6;

    /*
     *	Now the Public Information Elements (ie., those which require
     *  fields not available through Telenetworks private IE mechanism)
     */

    /*
     *	The Bearer Capability.  (Must do here, as T'works put out a
     *  mu-law encoding octet, which is surprising for digital data).
     *
     *	Also, we must deal with 56Kb links
     */

    p[i++] = BEARER_CAPABILITY_IE;
    p[i++] = 3;			/* length */
    p[i++] = 0x08;		/* xfer */
    p[i++] = 0x00;		/* circuit mode */
    p[i++] = BEAR_RATE_64KBPS;	/* 64Kb/s */

    /* The Channel ID for PRI */
    p[i++] = CHANNEL_ID_IE;
    p[i++] = 3;			/* length */
    p[i++] = 0x00;		/* preferred = 0, exclusive = 1 */
    p[i++] = B_CHANNEL_UNITS;	/* channel type */
    p[i++] = 0x00;		/* request channel 0, the code will assign */

    /*
     * If we are running 4ESS let's check on our NSF.
     */

    if ((SwitchType == PRI_4ESS_STYPE) && do_nsf) {
        p[i++] = NETWORK_FACILITY_IE;
        p[i++] = 2;
        p[i++] = 0x80;          /* Network ID 0 for now */
        p[i++] = nsf_type;      /* Facility coding SDN     */
    }

    /* The Called Party Number and Called Party Subaddress
     * the subaddress is optional and is separated by a colon
     * in the called number string, as shown below
     * dialer map ip x.x.x.x 12345:6789
     *                             ^subaddress digits
     */
    do_subaddress = FALSE;
    count = 0;
    p[i++] = CALLED_NUMBER_IE;
    called_num_len_ie = i;		/* save the index in case a subaddr */
    p[i++] = 1 + strlen(number);
    p[i++] = 0x80;		/* unknown number type and plan */
    for (ptr = number ; *ptr ; p[i++] = *ptr++ ) {
	if(number[count] == ':') {
	    do_subaddress = TRUE; /* have to process subaddress */
	    break;
	}
	count++;
    }

    if(do_subaddress) {
	p[called_num_len_ie] = count + 2; /* adjust CALLED Party number len */
	p[i++] = CALLED_SUB_ADDR_IE;
	called_num_len_ie = i;     /* save the value */
	p[i++] = strlen(number) - count + 1; /* subaddr length */
	p[i++] = 0x80;	/* NSAP, even */
	p[i++] = 0x50;	/* AFI code x50 indicates sub-address is IA5 chars */
	while (number[++count]) {
	    p[i++] = number[count];
	}
    }

    /*
     * The SENDING_COMPLETE IE says that everything is
     * in this SETUP message. Added for Taiwan homologation.
     */
    if (isdn_sys_constants[dsl].sending_complete)
        p[i++] = 0xA1;   /* Sending Complete */

    if(LIF_SendBuf(HOST_ID, CC_ID, i, LRG_INFO_BTYPE, p, CALL_OUTGOING,
		   NOW, REGULAR_MAIL) != NOERR)
	return -1;
	
    return 0;
}

/*
 * Send a Release Complete to reject the incoming call.
 * This is for PRI calls.
 */
static int
process_rejectcall (UINT16 call_id, int cause, int dsl)
{
    BYTE *p;
    BYTE msg;
    int i;
    hwidbtype *b_idb;

    b_idb = isdn_mib_find_idb(call_id, isdn_idblist[dsl]);
    if (b_idb != NULL) {
        isdn_mib_cause_init(cause | 0x80,  TX, b_idb->isdn_info);
        b_idb->isdn_info->call_id = call_id;
        isdn_mib_update(b_idb, NULL, FALSE);
    }
    if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
	return -1;

    msg = CALL_REJECTION;

    p[0] = msg;
    p[1] = dsl;			/* DSL */
    p[2] = SIGNALING_CES;
    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;	/* LSB */
    p[5] = 0;			/* length of privates, fill in later */

    /*
     * Make the Cause a private ie
     */
    i = 6;
    p[i++] = CAUSE_IE;
    p[i++] = 1;			/* length */
    p[i++] = 0x80 | cause;
    p[5] = 3;			/* private info length */

    /*
     * Add the real Cause IE (public)
     */
    p[i++] = CAUSE_IE;
    p[i++] = 2;
    p[i++] = 0x87;
    p[i++] = 0x80 | cause;

    if(LIF_SendBuf(HOST_ID, CC_ID, i, SML_INFO_BTYPE,
		   p, msg, NOW, REGULAR_MAIL) != NOERR)
	return -1;
    

    return 0;
}

/* send a disconnect */
static int
process_disconnect (UINT16 call_id, int cause, int diagnostic,
		    int ces, int dsl,  boolean semi_perm_conn)
{
    BYTE *p;
    BYTE msg;
    int i;
    hwidbtype *b_idb;
    
    b_idb = isdn_mib_find_idb(call_id, isdn_idblist[dsl]);
    /*
     * if the dialer has asked to disconnect a call that's already been
     * disconnected, then we need not update the mib. So, let's update
     * the mib tables only for genuine call disconnections.
     */
    if (b_idb != NULL) {
       if  (b_idb->isdn_info->in_use) {
          isdn_mib_cause_init(cause | 0x80,  TX, b_idb->isdn_info);
          isdn_mib_update(b_idb, NULL, FALSE);
       }
       b_idb->isdn_info->in_use = FALSE;
       b_idb->isdn_info->call_is_voice = FALSE;
    }
    if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
	return -1;

    if (is_pri)
	msg = CALL_DISC;
    else
	msg = HOST_DISCONNECT;	/* message type */

    p[0] = msg;
    p[1] = dsl;			/* DSL */

    /* added for dms and ni1 support of second tei usage */
    if((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
	if ((ces == SIGNALING_CES) || (ces == 0))
	    p[2] = SIGNALING_CES; /* main ces */
	else
	    p[2] = SIGNALING_CES + 1; /* 2nd ces */
    }
    else
	p[2] = SIGNALING_CES;	/* not dms or ni1, only 1 tei */

    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;	/* LSB */

    /* 
     * Make the cause a private IE, privates start at p[6].
     */
    i = 6;
    p[i++] = CAUSE_IE;
    p[i++] = 1;		/* length */
    p[i++] = 0x80 | cause;
    p[5] = 3;    /* length of private IEs */

    /*
     *	Cause
     * VN2 diagnostics only on STATUS msg
     */
    if((SwitchType == VN2_STYPE) || (SwitchType == BRI_5ESS_STYPE) ||
       (SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE) ||
       (SwitchType == VN3_STYPE) || (SwitchType == BRI_TS013_STYPE) ||
       (SwitchType == BRI_NET3_STYPE))
	diagnostic = -1;

    if (SwitchType != BRI_1TR6_STYPE) {
	p[i++] = CAUSE_IE;
	/*
	 * Determine if a Diagnostic byte is to be included with the Cause.
	 */
	p[i++] = diagnostic != -1 ? 3 : 2; /* length of Cause, +1 with diag */
	if ((SwitchType == VN2_STYPE) || (SwitchType == VN3_STYPE))
	    p[i++] = 0x87;	/* ccitt, international location */
	else
	    p[i++] = 0x80;	/* ccitt, user location */
	p[i] = 0x80 | cause;
	if(diagnostic != -1)
	    p[++i] = diagnostic;
    } else {
	if (semi_perm_conn) {
            p[i++] = NETWORK_FACILITY_IE;
            p[i++] = 0x04; /* length */
            p[i++] = 0x00;
            p[i++] = SEMI_PERMANENT_LINK;
            p[i++] = ALL_SERVICES;
            p[i++] = NO_ADDITIONAL_INFO;
	}
	p[i++] = CAUSE_IE;
	p[i] = 0;	/* length = 0, 1tr6 normal call clearing, for certif */
    }

    if(LIF_SendBuf(HOST_ID, CC_ID, ++i, SML_INFO_BTYPE, p, msg,
		   NOW, REGULAR_MAIL) != NOERR)
	return -1;
	
    return 0;
}

/* send a disconnect */
static int
process_ignore (UINT16 call_id, int ces, int dsl)
{
	BYTE *p;

	if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
		return -1;

	p[0] = HOST_DISCONNECT_ACK;		/* message type */
	p[1] = dsl;				/* DSL */

	/* added for dms and ni1 support of second tei usage */
	if((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) {
		if (ces == SIGNALING_CES)
			p[2] = SIGNALING_CES;	/* main ces */
		else
			p[2] = SIGNALING_CES + 1;	/* 2nd ces */
	}
	else
		p[2] = SIGNALING_CES;	/* not dms or ni1, only 1 tei */

	p[3] = (call_id >> 8) & 0xff;	/* MSB */
	p[4] = call_id & 0xff;			/* LSB */
	p[5] = 0;

	if(LIF_SendBuf(HOST_ID, CC_ID, 6, SML_INFO_BTYPE,
		       p, HOST_DISCONNECT_ACK, NOW, REGULAR_MAIL) != NOERR)
		return -1;
	
	return 0;
}

/* send a simple command to the call control.  return 0 on success */
static int
process_simple (BYTE msg, UINT16 call_id, enum Chan bchan, int ces, int dsl)
{
    BYTE *p;
    int i;
    isdn_bri_t *isdn_info;
    char *ptr;

    if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
	return -1;

    p[0] = msg;			/* message type */
    p[1] = dsl;			/* DSL */

    /* The dms and ni1 fix to use 2 ces/tei values, the other switches
     * work fine with 1 ces/tei.
     */
    if((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE))
	p[2] = ces;
    else
	p[2] = SIGNALING_CES;

    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;	/* LSB */
    p[5] = 0;			/* private info length, filled in later */
    i = 6;			/* start filling in here */

    /* We have to add the Channel_ID ie in a CONNECT for the dms switch
     * and the ni1 switch. The other switches don't need it.
     */
    if((msg == HOST_CONNECT) && ((SwitchType == BRI_DMS100_STYPE) ||
				 (SwitchType == BRI_NI1_STYPE))) {
	p[i++] = CHANNEL_ID_IE;
	p[i++] = 1;		/* length */
	if(bchan == B1)
	    p[i++] = 0x89;	/* exclusive + channel B1 */
	else
	    p[i++] = 0x8A;	/* exclusive + channel B2 */
    }

    if ((msg == HOST_CONNECT) && (SwitchType == BRI_1TR6_STYPE)) {
          if (bchan == B1)
	      isdn_info = isdn_idblist[dsl]->isdn_info->b1_chan_idb->isdn_info;
          else
	      isdn_info = isdn_idblist[dsl]->isdn_info->b2_chan_idb->isdn_info;
          if (isdn_info->spc) {
		p[i++] = CONNECTED_ADDRESS_IE;
		p[i++] = strlen(spc_number[dsl]);
		for (ptr = spc_number[dsl] ; *ptr ; p[i++] = *ptr++)
			;
		p[i++] = NETWORK_FACILITY_IE;
	        p[i++] = 0x04; /* length */
		p[i++] = 0x00;
		p[i++] = SEMI_PERMANENT_LINK;
		p[i++] = ALL_SERVICES; 
		p[i++] = NO_ADDITIONAL_INFO;
		p[i++] = NETWORK_FACILITY_IE;
	        p[i++] = 0x04; /* length */
		p[i++] = 0x00;
		p[i++] = ACTIVATE_USE_OF_LINK; /* activate link */
		p[i++] = ALL_SERVICES;
		p[i++] = NO_ADDITIONAL_INFO;
          }
    }

    if(LIF_SendBuf(HOST_ID, CC_ID, i, SML_INFO_BTYPE, p, msg, NOW,
		   REGULAR_MAIL) != NOERR)
	return -1;
	
    return 0;
}

/*
 * send a simple command to the PRI call control.
 * return 0 on success
 */
static int
process_pri_simple (BYTE msg, UINT16 call_id, enum Chan bchan, int dsl)
{
    BYTE *p, *p2;
    int i;

    if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
        return -1;

    p[0] = msg;			/* message type */
    p[1] = dsl;			/* DSL */

    p[2] = SIGNALING_CES;

    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;	/* LSB */
    p[5] = 0;			/* length of privates, to be filled in later */

    /* We have to add the Channel_ID ie in a CONNECT for the dms switch
     * and the ni1 switch. The other switches don't need it.
     */
    i = 6;		/* start putting the information elements here */
    if(msg == CALL_ACCEPT) {
        p[i++] = CHANNEL_ID_IE;
        p[i++] = 4;		/* length */
        p[i++] = 1;		/* exclusive = 1, preferred = 0 */
        p[i++] = B_CHANNEL_UNITS; /* channel type */

        if (SwitchType == PRI_TS014_STYPE) {
            if (bchan <= 15)
                p[i++] = (bchan + 1) & 0x1f;  /* exclusive + B channel */
            else   
                p[i++] = bchan & 0x1f; /* exclusive + B channel */
        } else {
            p[i++] = (bchan + 1) & 0x1f;  /* exclusive + B channel */
        }

        p[i++] = 0x00;		/* any interface = 0 */

        /*
         * stuff in the number of Private ies
         */
        p[5] = i-5;
    }
    if(LIF_SendBuf(HOST_ID, CC_ID, i, SML_INFO_BTYPE, p, msg, NOW,
		   REGULAR_MAIL) != NOERR)
        return -1;

    /*
     * now force a CONNECT after the above CALL_PROC
     */
    if(msg == CALL_ACCEPT) {
        if((p2 = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
            return -1;

        p2[0] = msg;		/* message type */
        p2[1] = dsl;		/* DSL */

        p2[2] = SIGNALING_CES;

        p2[3] = (call_id >> 8) & 0xff; /* MSB */
        p2[4] = call_id & 0xff;	/* LSB */
        p2[5] = 0;

        /* We have to add the Channel_ID ie in a CONNECT for the dms switch
	 * and the ni1 switch. The other switches don't need it.
	 */
	i = 6;
        p2[i++] = PROGRESS_IND_IE;
        p2[i++] = 1;		/* length */
        p2[i++] = 0x82;		/* some value - called party answer */


        /*
         * stuff in the number of Private ies
         */
        p2[5] = i-5;
	if(LIF_SendBuf(HOST_ID, CC_ID, i, SML_INFO_BTYPE, p2, msg, NOW,
		       REGULAR_MAIL) != NOERR)
	    return -1;
    }

    return 0;
}

/*
 * send a call proc command to the PRI call control.
 * this is done after an incoming SETUP and a successful
 * DEV_INCALL to the modem code.
 * return 0 on success
 */
static int
process_modem_callproc (BYTE msg, UINT16 call_id, enum Chan bchan, int dsl)
{
    BYTE *p;
    int i;

    if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
        return -1;

    p[0] = msg;                 /* message type */
    p[1] = dsl;                 /* DSL */

    p[2] = SIGNALING_CES;

    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;      /* LSB */
    p[5] = 0;                   /* length of privates, to be filled in later */

    /* We have to add the Channel_ID ie in a CONNECT for the dms switch
     * and the ni1 switch. The other switches don't need it.
     */
    i = 6;              /* start putting the information elements here */
    if(msg == CALL_ACCEPT) {
        p[i++] = CHANNEL_ID_IE;
        p[i++] = 4;             /* length */
        p[i++] = 1;             /* exclusive = 1, preferred = 0 */
        p[i++] = B_CHANNEL_UNITS; /* channel type */

        if (SwitchType == PRI_TS014_STYPE) {
            if (bchan <= 15)
                p[i++] = (bchan + 1) & 0x1f;  /* exclusive + B channel */
            else
                p[i++] = bchan & 0x1f; /* exclusive + B channel */
        } else {
            p[i++] = (bchan + 1) & 0x1f;  /* exclusive + B channel */
        }

        p[i++] = 0x00;          /* any interface = 0 */

        /*
         * stuff in the number of Private ies
         */
        p[5] = i-5;
    }
    if(LIF_SendBuf(HOST_ID, CC_ID, i, SML_INFO_BTYPE, p, msg, NOW,
                   REGULAR_MAIL) != NOERR)
        return -1;

    /*
     * Now force out an Alerting Message. This is required by 
     * various networks around the world when handling an 
     * incoming speech call. CSCdi74913
     */
    if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
        return -1;

    p[0] = CALL_PROGRESS;
    p[1] = dsl;                 /* DSL */

    p[2] = SIGNALING_CES;

    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;      /* LSB */
    p[5] = 0;                   /* length of privates, to be filled in later */

    /*
     * This is what forces the Alerting to go out.
     */
    i = 6;
    p[i++] = PROGRESS_IND_IE;
    p[i++] = 1;            /* length */
    p[i++] = 0x81;         /* CALLED_ALERTING, for alerting to go out*/


    /*
     * stuff in the number of Private ies
     */
    p[5] = i-5;
    if(LIF_SendBuf(HOST_ID, CC_ID, i, SML_INFO_BTYPE, p, CALL_PROGRESS, NOW,
                   REGULAR_MAIL) != NOERR)
        return -1;

    return 0;
}

/*
 * send a connecte command to the PRI call control.
 * this is done after receiving a VOICE_ANS from the
 * modem code.
 * return 0 on success
 */
static int
process_modem_conn (BYTE msg, UINT16 call_id, enum Chan bchan, int dsl)
{
    BYTE *p2;
    int i;
    if((p2 = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
        return -1;

    p2[0] = msg;            /* message type */
    p2[1] = dsl;            /* DSL */

    p2[2] = SIGNALING_CES;

    p2[3] = (call_id >> 8) & 0xff; /* MSB */
    p2[4] = call_id & 0xff; /* LSB */
    p2[5] = 0;

    /* We have to add the Channel_ID ie in a CONNECT for the dms switch
     * and the ni1 switch. The other switches don't need it.
     */
    i = 6;
    p2[i++] = PROGRESS_IND_IE;
    p2[i++] = 1;            /* length */
    p2[i++] = 0x82;         /* some value - called party answer */


    /*
     * stuff in the number of Private ies
     */
    p2[5] = i-5;
    if(LIF_SendBuf(HOST_ID, CC_ID, i, SML_INFO_BTYPE, p2, msg, NOW,
                   REGULAR_MAIL) != NOERR)
        return -1;

    return 0;
}
/*
 * send a disconnect ack command to the call control.
 * return 0 on success
 */
static int
process_disc_ack (UINT16 call_id, int cause, int diagnostic, int ces, int dsl)
{
    BYTE *p;
    int i;

    if((p = LIF_GetBuffer(SML_INFO_BTYPE, TRUE)) == NULL)
	return -1;

    p[0] = HOST_DISCONNECT_ACK;	/* message type */
    p[1] = dsl;			/* DSL */

    /* added for dms and ni1 support of second tei usage */
    if((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)){
	if (ces == 0)
	    p[2] = SIGNALING_CES; /* main ces */
	else
	    p[2] = SIGNALING_CES + 1; /* 2nd ces */
    }
    else
	p[2] = SIGNALING_CES;	/* not dms or ni1, only 1 tei */

    p[3] = (call_id >> 8) & 0xff; /* MSB */
    p[4] = call_id & 0xff;	/* LSB */
    p[5] = 0;
    p[6] = CAUSE_IE;
    p[7] = 1;
    p[i = 8] = cause;
    if(SwitchType != VN2_STYPE && SwitchType != VN3_STYPE && diagnostic != -1)
	p[i = 9] = diagnostic;

    if(LIF_SendBuf(HOST_ID, CC_ID, i + 1, SML_INFO_BTYPE, p,
		   HOST_DISCONNECT_ACK, NOW, REGULAR_MAIL) != NOERR)
	return -1;
	
    return 0;
}

/*
 *  Deal with a change of state in a physical layer.
 */
void
isdn_phys_ind (hwidbtype *idb, boolean is_up, int dsl)
{
	isdn_bri_t *isdn_info;
	hwidbtype *b1_idb;
	call_id_string_t *call_tbl, *tmp;
	hwidbtype *b2_idb;

     if(idb){
	/*
	 * Leased line acts differently from normal ISDN interfaces.  B channels
	 * need to be notified independantly of state change at layer 1
	 */
	if(ISBRILEASED(idb)){
		isdn_info = idb->isdn_info;
		b1_idb = isdn_info->b1_chan_idb;
		b2_idb = isdn_info->b2_chan_idb;

		if (is_up) {
		    if (idb->state != IDBS_ADMINDOWN)
	            net_cstate(idb, IDBS_UP);
		    if (b1_idb->state != IDBS_ADMINDOWN)
	            (b1_idb->isdn_info->cstate_bri_leased)(b1_idb, IDBS_UP);
		    if (b2_idb->state != IDBS_ADMINDOWN)
	            (b2_idb->isdn_info->cstate_bri_leased)(b2_idb, IDBS_UP);
		} else {
		    if (idb->state != IDBS_ADMINDOWN)
	            net_cstate(idb, IDBS_DOWN);
		    if (b1_idb->state != IDBS_ADMINDOWN)
	            (b1_idb->isdn_info->cstate_bri_leased)(b1_idb, IDBS_DOWN);
		    if (b2_idb->state != IDBS_ADMINDOWN)
	            (b2_idb->isdn_info->cstate_bri_leased)(b2_idb, IDBS_DOWN);			
		}
		idb->counters.transitions++;
		b1_idb->counters.transitions++;
		b2_idb->counters.transitions++;
		return;
	}

    /*
     * If the interface came up, inform the cstate routine so that it
     * can stop spoofing the interface.  While this is not visible to
     * the user currently, it may be helpful later.  If we were
     * deactivated, again inform the cstate routine as it may want
     * to tear down any calls that were up.
     *
     * Clear the in_use flag just for safety.  It's overkill...
     *
     * The idb here better be the D channel idb...
     *
     * NOTE: We don't do this for PRI because PRI gets here via the
     * cstate routine.  All PRI really wants to do is send the DEACT
     * indication...
     */
     if (!is_pri) {
        if (idb->isdn_info->b1_chan_idb->isdn_info->in_use)
             isdn_mib_update(idb->isdn_info->b1_chan_idb, NULL, FALSE);
        if (idb->isdn_info->b2_chan_idb->isdn_info->in_use)
             isdn_mib_update(idb->isdn_info->b2_chan_idb, NULL, FALSE);
        idb->isdn_info->b1_chan_idb->isdn_info->in_use = FALSE;
        idb->isdn_info->b2_chan_idb->isdn_info->in_use = FALSE;

        /*
         * We should ignore state changes if we are admin down.
         * This is evil as we shouldn't have even gotten them,
         * but there is a race condition.
         */
        if (is_up) {
            if (idb->state != IDBS_ADMINDOWN)
                net_cstate(idb, IDBS_UP);
        } else {
            call_tbl = idb->isdn_info->call_tbl;
            while (call_tbl) {
                tmp = call_tbl->next;
                isdn_carrier_timeout(idb, call_tbl, call_tbl);
                call_tbl = tmp;
            }
            if (idb->state != IDBS_ADMINDOWN)
            net_cstate(idb, IDBS_DOWN);
        }
        idb->counters.transitions++;
      }
     }

    if (detail_debug)
	buginf("\nISDN %s: Sending data to L2", DSL_TO_NAME(dsl));

    /* send the PH_DEACT_IND (we never send a PH_ACT_IND) */
    if(!is_up) {
	if(LIF_SendPkt(	L2P_LAPD, /* protocol */
		       HDLC_GENERIC_ID,	/* source layer id */
		       0,	/* sender's state */
		       L2_ID,	/* destination layer id */
		       PH_DEACT_IND, /* event */
		       dsl,	/* digital subscriber loop */
		       0,	/* SAPI (filled in by layer 2) */
		       0,	/* CES (filled in by layer 2) */
		       0,	/* logical link id (filled in by L2) */
		       0,	/* call id (filled in by layer 2) */
		       0,	/* channel ID (0 = D channel) */
		       FALSE,	/* free packet? */
		       NULL,	/* address of packet */
		       NOW,	/* delay */
		       REGULAR_MAIL /* mail priority (0 through 255) */
		       ) != NOERR)
	    buginf("\nISDN Buffer Underflow: Cannot send packet to "
		   "report change in ISDN physical layer");
    }

    /* send the MPH_ACT_IND or MPH_DEACT_IND */
    if(LIF_SendPkt(	L2P_LAPD, /* protocol */
		   HDLC_GENERIC_ID, /* source layer id */
		   0,		/* sender's state */
		   ME_ID,	/* destination layer id */
		   is_up ? MPH_ACT_IND : MPH_DEACT_IND,	/* event */
		   dsl,		/* digital subscriber loop */
		   ME_SAPI,	/* SAPI */
		   0,		/* CES (filled in by layer 2) */
		   0,		/* logical link id (filled in by L2) */
		   0,		/* call id (filled in by layer 2) */
		   0,		/* channel ID (0 = D channel) */
		   FALSE,	/* free packet? */
		   NULL,	/* address of packet */
		   NOW,		/* delay */
		   REGULAR_MAIL	/* mail priority (0 through 255) */
		   ) != NOERR)
	buginf("\nSoftware Error: Cannot send packet to report change \
in ISDN physical layer");

}

/*
 *	Process a frame we received.
 */
static void
process_rxdata (paktype *p)
{
    PKT_ID pkt;
    uchar dsl;
    hwidbtype *hwidb;
    
    /* Extract the dsl here */
    /* Beware: can't use hwidb->isdn_info->dsl 
     * to extract dsl since BRI doesn't store
     * in that structure.
     */
    hwidb = (hwidbtype *) p->if_input->hwptr;
    for (dsl = 0; dsl < MAX_DSL; dsl++)
        if (isdn_idblist[dsl] == hwidb)
	    break;

    if (dsl == MAX_DSL) {
	buginf("\nISDN Error: Illegal Interface %s", hwidb->hw_short_namestring);
	retbuffer(p);
	return;
    }
 
    /*
     *		Because of the limitations of the Idacom protocol analyzer,
     *	it is very useful to be able to send ALERTs and convert them into
     *	whatever you wanted it to send you.  So I'm leaving this code here.
     *	Change it as you like, but leave the rough outline for the next
     *	poor hacker who has to deal with this.
     */
#if 0
    if(p->datagramsize > 6 &&
       p->datagramstart[6 + p->datagramstart[5]] == ALERTING) {
	int i;
	static tog = 0;

	i = 6;
	p->datagramstart[i++] = fake_call_ref;
	switch(tog++ % 3) {
	case 0:
	    p->datagramstart[i++] = CALL_PROC;
	    p->datagramstart[i++] = CHANNEL_ID_IE;
	    p->datagramstart[i++] = 1;
	    p->datagramstart[i++] = 0x89;
	    break;

	case 1:
	    p->datagramstart[i++] = CONNECT;
	    break;
			
	case 2:
	    p->datagramstart[i++] = RELEASE;
	    p->datagramstart[i++] = CAUSE_IE;
	    p->datagramstart[i++] = 2;
	    p->datagramstart[i++] = 0x87;
	    p->datagramstart[i++] = 0x90;

	    p->datagramstart[i++] = 0xc0;
	    break;
	}

	p->datagramsize = i;
    }
#endif

    do_statistics(p, RX, dsl);

    /* check for a packet too large for us to handle */
    if(p->datagramsize > LRG_INFO_LEN)
	p->datagramsize = LRG_INFO_LEN;	/* truncate to fit */

    if((pkt = (PKT_ID)LIF_GetPkt(LRG_INFO_BTYPE, DATA_OFFSET, TRUE))
       == NULL) {
	if (isdn_debug)
	    buginf("\nSoftware Error: LIF_GetPkt failed (%s:%d)\n",
		   __FILE__, __LINE__);
	isdn_idblist[dsl]->counters.output_nobuffers++;
	retbuffer(p);		/* might as well just throw it on the floor */
	return;
    }

    /*
     *	We will simply copy the data.  Yes, it's a waste of time,
     *  but this never really happens.
     */
    bcopy(p->datagramstart, pkt->Header, p->datagramsize);
    pkt->HeadLen = p->datagramsize >= 4 ? 4 : 3;
    if(pkt->HeadLen >= 4)
	pkt->InfoLen = p->datagramsize - pkt->HeadLen;
    else
	pkt->InfoLen = 0;
    pkt->Info = (pkt->InfoLen > 0) ? &pkt->Header[pkt->HeadLen] : NULL;

    retbuffer(p); 
    if (detail_debug)
	buginf("\nISDN %s:  Frame sent to L2", DSL_TO_NAME(dsl));
    if(LIF_SendPkt(	L2P_LAPD, /* protocol */
		   HDLC_GENERIC_ID, /* source layer id */
		   0,		/* sender's state */
		   L2_ID,	/* destination layer id */
		   PH_DATA_IND,	/* event to mail */
		   dsl,		/* digital subscriber loop */
		   0,		/* SAPI (filled in by layer 2) */
		   0,		/* CES (filled in by layer 2) */
		   0,		/* logical link id (filled in by L2) */
		   0,		/* call id (filled in by layer 2) */
		   0,		/* channel ID (0 = D channel) */
		   TRUE,	/* free packet? */
		   pkt,		/* address of packet */
		   NOW,		/* delay */
		   REGULAR_MAIL	/* mail priority (0 through 255) */
		   ) != NOERR) {
	/* what should we do to show the error? */
	buginf("\nISDN Error: Cannot send packet to protocol stack");
    }
}
/*
 * Given the Call ID, Find the call table
 */
static call_id_string_t *isdn_find_cid_table( hwidbtype *idb, UINT16 call_id)
{
    call_id_string_t *call_entry;

    call_entry = idb->isdn_info->call_tbl;
    while (call_entry) {
	  if (call_entry->call_id == call_id) 
	     return(call_entry);
          else
	      call_entry = call_entry->next;
    }
    return(NULL);
}

static char *display_remote_number (uchar *remote, isdn_bri_t *isdn_info)
{
    if (remote == NULL) {
	if (isdn_info->remote_number == NULL)
	    return("<unknown>");
	else
	    return(isdn_info->remote_number);
    }
    return(remote);
}

/*
 *	Process some information from the Layer 3 Call Control
 */
static void
process_rxstate ()
{
    EXT_MEM gen_addr;		/* Generic address of mail */
    BYTE *ie;
    BOOLEAN	struct_err;
    PKG_ID pkg;
    isdn_bri_t *isdn_info, *d_isdn_info;
    UINT16 call_id;
    ushort modem_err, cause;
    enum Chan bchan;
    int dsl;
    int ces;
    char *remote_number, *local_number;
    enum rem_src remote_src;
    hwidbtype *tempidb, *b_idb, *d_chan_idb;
    MAILBOX_ID LIF_ID_To_MB(INT16);
    int channel;
    int nsf;			/* Network Specific Facilities */
    int i;			/* needed for debugging */
    call_id_string_t *call_tbl;
    char *rem_number;
    llc_parse_type llc;
    dialerdbtype *ddb, *netddb;
    boolean voice_answer;
    short minsum, max_available, used;
    boolean is_voice_bearer;

    voice_answer = FALSE;
    modem_err = 0;
    cause = 0;

    remote_src = ISDN_SRC_UNKNOWN;
    i = 0;
    rem_number = NULL;
    MTEX_ReadMail(LIF_ID_To_MB(HOST_ID), &gen_addr);
    pkg = (PKG_ID)gen_addr.Addr;
    dsl = pkg->PkgInfo[1];
    ces = pkg->PkgInfo[2];
    bchan = ISDN_Dchan;		/* init to an invalid value */
    call_id = (pkg->PkgInfo[3] << 8) | pkg->PkgInfo[4];

    d_chan_idb = isdn_idblist[dsl];
    if (d_chan_idb == NULL) {
	if(isdn_debug)			
	    buginf("\nISDN Event: got a null dsl");
	return;
    }
    d_isdn_info = d_chan_idb->isdn_info;
    if (d_isdn_info == NULL) {
	if (isdn_debug)
	    buginf("\nISDN Event: %s no isdn info",
		   d_chan_idb->hw_namestring);
	return;
    }

    if(detail_debug) {
	buginf("\n  entering process_rxstate, call-id = %x", call_id);
	buginf("\n   info [0-25]= ");
	for (i=0; i<25; i++) {
	    buginf("%x ", pkg->PkgInfo[i]);
	}
    }

    /*********************************
     *
     * These are the Basic Rate Events
     *
     ********************************/
    /*
     * The dms100 and ni1 can use ces 1 or 2. Only display if it is not
     * the SIGNALING_CES (1)
     */
    if((isdn_debug || isdn_debug_ok(q931_debug, dsl)) &&
       (ces != SIGNALING_CES))
	buginf("\nISDN %s: Event: incoming ces value = %d",
	       d_chan_idb->hw_short_namestring, ces);
    switch(pkg->MsgType) {
    case HOST_INCOMING_CALL:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_INCOMING_CALL");
	    isdn_buginf_flush();
	}
	if((bchan = extract_bchan(pkg)) == ISDN_Dchan) {
	    if(isdn_debug)
		buginf("\nRemote BRI Protocol Error: "
		       "No B channel assigned by switch (call id = %d)",
		       call_id);
	    process_ignore(call_id, ces, dsl);
	    break;
	}

        /*
         * Added for DMS100 testing, have to ignore incoming calls if the
         * spid is not valid. Will allow an NIT but for only one TEI.
         */
        if (((SwitchType == BRI_DMS100_STYPE) || (SwitchType == BRI_NI1_STYPE)) &&
             (!ccbfit_validspid(dsl, ces))) {
            if (isdn_debug || isdn_debug_ok(q931_debug, dsl)) {
                buginf("\nISDN %s: Event: Invalid SPID, ces %d, Incoming call "
                       "ignored", d_chan_idb->hw_short_namestring, ces);
            }
            process_ignore(call_id, ces, dsl);
            break;
        }

	remote_number = extract_calling_num(pkg);
	ddb = get_dialerdb(d_chan_idb);
	netddb = GET_NETDDB(ddb);

	minsum = get_dialer_minsum(ddb);
	max_available = ddb->dialer_num_link - minsum;
	used = get_dialer_used(ddb);
	/*
	 * This IS_CALL_ALLOWED() at this point is bogus since Dialer
	 * Profile binding hasn't still happened and the netddb we have
	 * in hand is the D Channel's and not the Dialer Profile above.
	 */

	if (DIALPOOL_MEMBER(ddb)) {
	    if (!(IS_CALL_ALLOWED(netddb) &&
		  (used < max_available))) {
		process_disconnect(call_id, CALL_REJECTED, 0xe, ces,
				   dsl, NON_SPC_CALL);
		if (isdn_debug || dialer_debug) {
		    buginf("\nISDN %s: Incoming call rejected, "
			   "exceeded max calls",
			   d_chan_idb->hw_short_namestring);
		}
		
		if (remote_number)
		    free(remote_number);
		break;
	    }
	}
	b_idb = d_isdn_info->b_chan_idb[bchan];
	isdn_info = b_idb->isdn_info;
	remote_src = ISDN_SRC_ISDN;
        tempidb = get_netidb(d_chan_idb);
        /* 4000/4500 homologation in France : Verify  BC */
        if (d_isdn_info->voice_answer_data || d_isdn_info->voice_answer_modem)
            voice_answer = TRUE;
        isdn_info->speed = parse_bearercap(pkg,
					   d_isdn_info->voice_answer_data,
					   d_isdn_info->voice_answer_modem,
                                           &is_voice_bearer);
        if ((!(isdn_info->speed)) &&
	    (SwitchType != BRI_1TR6_STYPE)){
            /* the bearercap was bad.  probably a voice call */
            if(isdn_debug)
                buginf("\nISDN %s: Event: "
                       "Received a call with a bad bearercap from %s on %s",
                       d_chan_idb->hw_short_namestring,
		       display_remote_number(remote_number, isdn_info),
                       isdn_info->chantype == B1 ? "B1" :
                       (isdn_info->chantype == B2 ? "B2" : "<???>"));
            /*
             *  We take no action--this is probably a call to another
             *  device on the S bus.
             */
            process_ignore(call_id, ces, dsl);
            free(remote_number);
            break;
        }
        /*
         * If we are a member of a rotary group we can still do
         * caller id screening but it is done by the dialer code.
         * idb->hunt_group_master == NULL if we are NOT part of
         * a rotary group.
         */
        if (!dialer_calling_num (remote_number, netddb)) {
            process_disconnect(call_id, CALL_REJECTED, 0xe,
			       ces, dsl, NON_SPC_CALL);
            reg_invoke_isdn_stats_update(d_chan_idb,
					 REFUSED_CALL,
					 remote_number);
            if (remote_number != NULL)
                free(remote_number);
            d_isdn_info->num_screens++;
            if (isdn_debug)
                buginf("\nISDN %s: Incoming call rejected due to screening.",
                       d_chan_idb->hw_short_namestring);
           break;
        }
	if (! verify_answer_number(pkg))  {
	    if(isdn_debug) {
		buginf("\nISDN %s: Event: "
		 "Received a call from %s on %s at %d Kb/s", 
                  d_chan_idb->hw_short_namestring,
		  display_remote_number(remote_number, isdn_info),
		  isdn_info->chantype == B1 ? "B1" :
		  (isdn_info->chantype == B2 ? "B2" : "<???>"),
                  isdn_info->speed == ISDN_SPEED_VOICE ? 64 : isdn_info->speed);
		buginf("\nISDN %s: Ignoring call : Received called/subaddr "
		       "does not match configured numbers",
		       d_chan_idb->hw_short_namestring);
	    }
	    process_ignore(call_id, ces, dsl);
	    break;
	}
	/*
	 * We will now parse the llc.  We will use it for
	 * v.120 below
	 */
	parse_llc(pkg, &llc);
	if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
	    display_llc(dsl, 'B', &llc);
	if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
	    if (llc.llc_valid) {
		isdn_info->speed = llc.speed;
	    } else {
		if(isdn_debug) {
		    buginf("\nISDN %s Event: Received a call "
			   "with a bad Low Layer Compatibility from %s on %s",
			   d_chan_idb->hw_short_namestring,
			   display_remote_number(remote_number, isdn_info),
			   isdn_info->chantype == B1 ? "B1" :
			   (isdn_info->chantype == B2 ? "B2" : "<???>"));
		    buginf("\n---Assuming 64Kb/s");
		}

		/*
		 *  assume 64Kb, so as not to change the sematics of
		 *  homologated code.
		 */
		isdn_info->speed = ISDN_SPEED_64;
	    }
	} 
	/*
	 * User defineable option:
	 * We check if the call was not end to end and if so set the speed
	 * to what the user has pre-chosen.
	 */
	if (isdn_sys_constants[dsl].nend_to_end){
	    if(isdn_debug)
		buginf("\nISDN %s: Event: Checking if call is not ISDN "
                "end to end", d_chan_idb->hw_short_namestring);
	    if (parse_progress(pkg) & CALL_NOT_END_TO_END_ISDN){
		isdn_info->speed = isdn_sys_constants[dsl].nend_speed;
		if(isdn_debug)
		    buginf("\nISDN %s: Event: Adjusting speed to user "
                           "selection %dK", d_chan_idb->hw_short_namestring, 
                           isdn_info->speed);
	    }
	}
        /*
         * User defineable option:
         * We need to see if the user wants to override the incoming speed
         * if this is a voice call.
         */
        if ((is_voice_bearer) &&
            (isdn_sys_constants[dsl].voice_rate) && (voice_answer)) {
                isdn_info->speed = isdn_sys_constants[dsl].voice_speed;
                if(isdn_debug)
                    buginf("\nISDN %s: Event: Speech call adjusting speed
                           to user " "selection %dK", DSL_TO_NAME(dsl),
                           isdn_info->speed);
        }
	if ((SwitchType == BRI_1TR6_STYPE) &&
	    (!(isdn_info->speed = parse_service(pkg)))) {
	    /* 1TR6 Service IE was bad */
	    if (isdn_debug)
		buginf("\nISDN %s: Event: "
		       "Received a call with a bad Service ie from %s on %s",
		       d_chan_idb->hw_short_namestring,
		       display_remote_number(remote_number, isdn_info),
		       isdn_info->chantype == B1 ? "B1" :
		       (isdn_info->chantype == B2 ? "B2" : "<???>"));
	    process_ignore(call_id, ces, dsl);
	    free(remote_number); /* ignore with bad Serive IE */	
	    break;
	}
	if(parse_hlc(pkg)) {
	    /* The High Layer Compatability was bad.  Ignore. */
	    if(isdn_debug)
		buginf("\nISDN %s: Event: "
		       "Rx call w/ bad High Layer Compat from %s on %s", 
                       d_chan_idb->hw_short_namestring,
		       display_remote_number(remote_number, isdn_info),
		       isdn_info->chantype == B1 ? "B1" :
		       (isdn_info->chantype == B2 ? "B2" : "<???>"));
	    /*
	     *	We take no action--this is probably a call to another
	     *	device on the S bus.
	     */
	    process_ignore(call_id, ces, dsl);
	    free(remote_number);
	    break;
	}

	/*
	 * If we are answering as V120 - make sure that we can set up a
	 * v120 call
	 */
	if (llc.v120 || d_isdn_info->answer_all_as_v120) {
	    if (llc.v120) {
		if ((llc.llc_valid == FALSE) ||
		    (!(reg_invoke_v120_start_interface(b_idb,
						       TRUE,
						       &llc.v120_info)))) {
		    /* Can't get a V.120 session - ignore the call */
		    process_ignore(call_id, ces, dsl);
		    if (remote_number)
			free(remote_number);
		    break;
		}
	    } else {
		/* Don't have an llc with V.120 */
		if (!(reg_invoke_v120_start_interface(b_idb, FALSE, NULL))) {
		    /* Couldn't start V.120 - ignore call */
		    process_ignore(call_id, ces, dsl);
		    if (remote_number)
			free(remote_number);
		    break;
		}
	    }
	}

	if(isdn_debug)
	    buginf("\nISDN %s: Event: Received a call from %s on %s at "
                  "%d Kb/s", d_chan_idb->hw_short_namestring,
		  display_remote_number(remote_number, isdn_info),
		  isdn_info->chantype == B1 ? "B1" :
		  (isdn_info->chantype == B2 ? "B2" : "<???>"),
		  isdn_info->speed == ISDN_SPEED_VOICE ? 64 : isdn_info->speed);
	if(isdn_info->in_use) {
	    /* We are using the B channel already */
	    if(isdn_debug || isdn_debug_ok(q931_debug, dsl)) {
		buginf("\nISDN %s: Event: Not accepting call, "
		       "B-channel in use", d_chan_idb->hw_short_namestring);
            }
	    free(remote_number);
	    if(SwitchType != BRI_1TR6_STYPE) 
		process_disconnect(call_id, REQ_CHANNEL_NOT_AVAIL, 0xe,
				   ces, dsl, NON_SPC_CALL);
	    break;
	}

	/*
	 * If this BRI is a dialer pool member, attempt to bind to a dialer
	 * profile now using pool configuration or CLI.  If we can't bind
	 * and PPP or PAP/CHAP are not configured, reject the call.
	 */
	if (DIALPOOL_MEMBER(ddb)) {
	  ddb = get_dialerdb(b_idb);
	  if (!(dialer_match_pool_or_caller_id(ddb, remote_number) || 
		dialer_may_match_name(b_idb))) {
	    process_disconnect(call_id, CALL_REJECTED, 0xe, ces, dsl, 
			       NON_SPC_CALL);
	    if (remote_number)
	      free(remote_number);
	    if (isdn_debug || dialer_debug) {
	      buginf("\nISDN %s: Incoming call rejected, unbindable", 
                     d_chan_idb->hw_short_namestring);
	    }
	    break;
          }
	}
	isdn_info->in_use = TRUE;

	if(isdn_debug)
	    buginf("\nISDN %s: Event: Accepting the call",
		   d_chan_idb->hw_short_namestring);
	if (isdn_info->set_bri_bandwidth)
	    (*isdn_info->set_bri_bandwidth)(b_idb, isdn_info->speed);
	isdn_report(dsl, ISDN_INCOMING, call_id, bchan, remote_number,
		    remote_src, ces);

	/*
	 *		The VN2 requires ALERTING messages if the Facilite
	 *	Nationale is not "*40#".  But it's hard to tell, being
	 *	that the spec is in French.  See the layer 3 spec, Aug
	 *	1990, section E.0.2.4 (p. 180).
	 */
	if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE) {
	    BYTE *ie;

#define	VN2_FACILITY_NATIONALE_IE	0x20
	    if((ie = my_Get_IE(pkg, VN2_FACILITY_NATIONALE_IE)) != NULL &&
	       ie[1] == 4 && ie[2] == '*' && ie[3] == '3' &&
	       ie[4] == '9' && ie[5] == '#')
		process_simple(HOST_ALERTING, call_id, bchan, ces, dsl);
	}
        if ((SwitchType == BRI_1TR6_STYPE) &&
	   ((ie = my_Get_IE(pkg, NETWORK_FACILITY_IE)) != NULL)){
	   if ((ie[3] == SEMI_PERMANENT_LINK) ||
	       (ie[3] == ACTIVATE_USE_OF_LINK))
	        isdn_info->spc = TRUE;
	   if (spc_number[dsl])
	        free(spc_number[dsl]);
	   spc_number[dsl] = extract_spc_number(pkg);
	}

	process_simple(HOST_CONNECT, call_id, bchan, ces, dsl);
	break;
		
    case HOST_CONNECT:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg,
		    d_chan_idb->hw_short_namestring,
		    "HOST_CONNECT");
	    isdn_buginf_flush();
	}
	if((bchan = extract_bchan(pkg)) != ISDN_Dchan) {
	    b_idb = d_isdn_info->b_chan_idb[bchan];
	    isdn_info = b_idb->isdn_info;
	} else {
	    /* search for the call id */
	    b_idb = d_isdn_info->b1_chan_idb;
	    isdn_info = b_idb->isdn_info;
	    if(isdn_info->call_id != call_id) {
		b_idb = d_isdn_info->b2_chan_idb;
		isdn_info = b_idb->isdn_info;
		if(isdn_info->call_id != call_id) {
		    buginf("\nISDN %s: Error: Unexpected Connect", 
                           d_chan_idb->hw_short_namestring);
		    process_disconnect(call_id, NORMAL_CLEARING, 0x81,
				       ces, dsl, NON_SPC_CALL);
		    isdn_info->in_use = FALSE;
		    break;
		}
		bchan = B2;
	    } else
		bchan = B1;
	}
	remote_number = NULL;
	isdn_report(dsl, ISDN_CONNECT, call_id, bchan, NULL, ISDN_SRC_UNKNOWN,
		    ces);
	if (isdn_info->set_bri_bandwidth) {
	    (*isdn_info->set_bri_bandwidth)(b_idb, isdn_info->speed);
	}
	if(isdn_debug)
	    buginf("\nISDN %s: Event: Connected to %s on %s at %d Kb/s",
		  d_chan_idb->hw_short_namestring,
		  display_remote_number(remote_number, isdn_info),
		  isdn_info->chantype == B1 ? "B1" :
		  (isdn_info->chantype == B2 ? "B2" : "<???>"),
	          isdn_info->speed == ISDN_SPEED_VOICE ? 64 : isdn_info->speed);
	break;
		
    case HOST_DISCONNECT:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_DISCONNECT");
	    isdn_buginf_flush();
	}
	b_idb = d_isdn_info->b1_chan_idb;
	isdn_info = b_idb->isdn_info;
	if (isdn_info->call_id != call_id) {
 	    b_idb = d_isdn_info->b2_chan_idb;
	    isdn_info = b_idb->isdn_info;
	    if(isdn_info->call_id != call_id) {
		buginf("\nISDN %s: Error: Unexpected Disconnect - callid %x", 
		       d_chan_idb->hw_short_namestring, call_id);
		break;
	    } else
		bchan = B2;
        } else
	    bchan = B1;
	if(isdn_debug) {
           call_tbl = isdn_find_cid_table(d_chan_idb, call_id);
           if (call_tbl)
               rem_number = strdup(call_tbl->called_num);
            buginf("\nISDN %s: Event:  Call to %s was hung up.", 
                   d_chan_idb->hw_short_namestring,
		   rem_number == NULL ? "<unknown>" : rem_number);
      }
	if ((ie = my_Get_IE(pkg, CAUSE_IE)) == NULL) {
	    process_disc_ack(call_id, MANDATORY_IE_MISSING, CAUSE_IE,
			     ces, dsl);
	} else {
	    CCIE_ExpandCause_IE(ie, &struct_err);
	    process_disc_ack(call_id, struct_err, CAUSE_IE, ces, dsl);
	}
	isdn_mib_extract_cause(pkg, RX, isdn_info);
        call_tbl = isdn_find_cid_table(d_chan_idb, call_id);
        b_idb->isdn_info->call_id = call_id;
        if (call_tbl) {
	    isdn_mib_update(b_idb, call_tbl->called_num, FALSE);
	    isdn_carrier_timeout(d_chan_idb, call_tbl, call_tbl);
	} else
	    isdn_mib_update(b_idb, NULL, FALSE);
	isdn_report(dsl, ISDN_DISCON, call_id, 0, NULL, ISDN_SRC_UNKNOWN, ces);
	break;

    case HOST_DISCONNECT_ACK:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_DISCONNECT_ACK"); 
	    isdn_buginf_flush();
	}
	b_idb = d_isdn_info->b1_chan_idb;
	isdn_info = b_idb->isdn_info;
        if (isdn_info->call_id != call_id) {
	    b_idb = d_isdn_info->b2_chan_idb;
	    isdn_info = b_idb->isdn_info;
        }
        isdn_mib_extract_cause(pkg, RX, isdn_info);

	call_tbl = isdn_find_cid_table(d_chan_idb, call_id);
        if (call_tbl) {
               b_idb->isdn_info->call_id = call_id;
               isdn_mib_update(b_idb, call_tbl->called_num, FALSE); 
	       isdn_carrier_timeout(d_chan_idb, call_tbl, call_tbl);
        }	
	isdn_report(dsl, ISDN_DISCON, call_id, 0, NULL, ISDN_SRC_UNKNOWN, ces);
	break;
		
    case HOST_PROCEEDING:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_PROCEEDING");
	    isdn_buginf_flush();
	}
	/* we must extract the assigned B channel, if there is one
	 * the 5ess will get channel usually get channel in HOST_INFO
	 * (SetupAck). Other switches seem to be sending the SetupAck.
	 */
	if((bchan = extract_bchan(pkg)) == ISDN_Dchan) {
	    if(isdn_debug) {
		if ((SwitchType != NTT_STYPE) &&
		    (SwitchType != VN2_STYPE) &&
		    (SwitchType != VN3_STYPE))
		    buginf("\nB-channel assigned in previous message "
			   "(call id = %d)", call_id);
		else
		    buginf("\nISDN %s: Remote BRI Protocol Error: No B "
                           "channel assigned by switch (call id = %d)", 
                           d_chan_idb->hw_short_namestring, call_id);
	    }
	    break;
	}
	b_idb = d_isdn_info->b_chan_idb[bchan];
	isdn_info = b_idb->isdn_info;
 	/*
	 * Make sure the PBX or switch is not delivering another call on
	 * a B-channel that is already in use.
	 */
	if ((isdn_info->in_use) && (isdn_info->call_id != call_id)){
	    buginf("\nISDN %s: Event: Not accepting call, B-chan in use", 
                   d_chan_idb->hw_short_namestring);
	    process_disconnect(call_id, CHANNEL_UNACCEPTABLE, 0xe, ces,
			       dsl, NON_SPC_CALL);
	    isdn_info->in_use = FALSE;
	} else {
	    isdn_info->call_id = call_id;
	    isdn_info->in_use = TRUE;
	    isdn_info->ces = ces;

	    /* Must also set the ces in the D-channel idb. In case
	     * the call fails, the proper ces is used for call release.
	     */
	    d_isdn_info->ces = ces;
	}
	break;

    case HOST_ALERTING:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_ALERTING");
	    isdn_buginf_flush();
	}
	break;
    case HOST_QUERY_RESPONSE:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_QUERY_RESPONSE"); 
	    isdn_buginf_flush();
	}
	break;

    case HOST_INFORMATION:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_INFORMATION");
	    isdn_buginf_flush();
	}
	/* AT&T will send the channel id in the SETUP_ACK message, this is
	 * processed as HOST_INFORMATION, we have to look for the channel id.
	 * Looks like other switches are having problems as well.
	 */
	if ((SwitchType != NTT_STYPE) &&
	    (SwitchType != VN2_STYPE) &&
	    (SwitchType != VN3_STYPE)) {
	    enum Chan bchan_tmp;
	    /* we must extract the assigned B channel, if there is one */
	    if((bchan_tmp = extract_bchan(pkg)) != ISDN_Dchan) {
		bchan = bchan_tmp; /* keep it valid */
		if(isdn_debug)
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "\nISDN Event: B channel assigned by switch");

		b_idb = d_isdn_info->b_chan_idb[bchan];
		isdn_info = b_idb->isdn_info;
		/*
		 * Is the B channel in use already? The PBX seems to
		 * occasionally use a B-channel that is already being used.
		 */
		if(isdn_info->in_use) {
		    if(isdn_debug || isdn_debug_ok(q931_debug, dsl))
		        buginf("\nISDN %s: Event: Not accepting call, "
                               "B-channel in use",
			       d_chan_idb->hw_short_namestring);
		    process_disconnect(call_id, CHANNEL_UNACCEPTABLE, 0xe,
				       ces, dsl, NON_SPC_CALL);
		} else {
		    isdn_info->call_id = call_id;
		    isdn_info->in_use = TRUE;
		    isdn_info->ces = ces;

		   /* Must also set the ces in the D-channel idb. In case
		    * the call fails, the proper ces is used for call release.
		    */
		   d_isdn_info->ces = ces;
                }
	    }
	}
	break;

    case HOST_PROGRESS:
	if(isdn_debug){
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "HOST_PROGRESS");
	    isdn_buginf_flush();
	}
	break;

	/************************************
	 *
	 * These are the Primary Rate Events
	 *
	 ************************************/
    case CALL_ACCEPT:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "CALL_ACCEPT");
	    isdn_buginf_flush();
	}
	bchan = extract_bchan(pkg);
	if (bchan == ISDN_Dchan) {
	    if(isdn_debug)
		buginf("\nISDN %s: Remote PRI Protocol Error: No B channel " 
		       "assigned by switch (call id = %d)", 
                       d_chan_idb->hw_short_namestring, call_id);
	    process_simple(CALL_REJECTION, call_id, bchan, ces, dsl);
	} else {
	    b_idb = d_isdn_info->b_chan_idb[bchan];
 	    if (b_idb && b_idb->isdn_info) {
		isdn_info = b_idb->isdn_info;
		isdn_info->call_id = call_id;
		isdn_info->in_use = TRUE;
		if(isdn_debug)
		    buginf("\nISDN %s: PRI Event: CALL_ACCEPT, bchan = %d", 
			   d_chan_idb->hw_short_namestring, bchan);
                /*
                 * There are two ways to detect if this is a modem voice
                 * call or not. If it was an outgoing voice call than
                 * the call_id will be 0xA000 or greater. If it was an
                 * incoming voice call the isdn_info->call_is_voice will
                 * be TRUE. In this case we generated the call so we
                 * can just use the call_id.
                 */
                if (call_id >= ISDN_MODEM_CALL_ID_MIN) {
                    isdn_info->call_is_voice = TRUE;
                    modem_err = reg_invoke_modem_report(d_chan_idb, call_id,
							ces, bchan,
							DEV_CALL_PROC,
							NULL, NULL, cause);
                   if (modem_err) {
                      if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
                         buginf("\nISDN %s: CALL_ACCEPT: MODEM ERROR %x: "
                                "bchan %d, call id %x",
                                DSL_TO_NAME(dsl), modem_err, bchan, call_id);
                   }

                }
	    } else {
		if (isdn_debug)
                buginf("\nISDN %s: Call Accept, but no idb, bchan = %d", 
                       d_chan_idb->hw_short_namestring, bchan);
	    }
	}
	break;

    case CALL_DISC:
	if(isdn_debug)
	    buginf ("\nISDN %s: entering process_rxstate, CALL_DISC", 
                    d_chan_idb->hw_short_namestring);

	/*
	 * Now let's check all the B-channels.
	 */
	for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
	    /*
	     * If someone just removed the pri-group
	     * we might not have a d-channel anymore.
	     */
	    b_idb = d_isdn_info->b_chan_idb[channel];
	    if (b_idb == NULL)
		continue;
	    isdn_info = b_idb->isdn_info;
	    if (isdn_info == NULL)
		continue;
	    if (isdn_info->call_id == call_id) {
		ces = isdn_info->ces;
		isdn_info->ces = SIGNALING_CES;
		isdn_info->in_use = FALSE;
		bchan = isdn_info->chantype;
	        isdn_mib_extract_cause(pkg, RX, isdn_info);
                call_tbl = isdn_find_cid_table(d_chan_idb, call_id);
                b_idb->isdn_info->call_id = call_id;
                if (call_tbl)
                     isdn_mib_update(b_idb, call_tbl->called_num, FALSE);
                else
                     isdn_mib_update(b_idb, NULL, FALSE);
                /*
                 * There are two ways to detect if this is a modem voice
                 * call  or not. If it was an outgoing voice call than
                 * the call_id will be 0xA000 or greater. If it was an
                 * incoming voice call the isdn_info->call_is_voice will
                 * be TRUE.
                 */
                if ((isdn_info->call_is_voice == FALSE) && 
                    (call_id < ISDN_MODEM_CALL_ID_MIN)) {
                    isdn_report(dsl, ISDN_DISCON, call_id, bchan, NULL, 
                                ISDN_SRC_UNKNOWN, ces);
                } else {
                    modem_err = reg_invoke_modem_report(d_chan_idb,
							call_id, ces, bchan,
							DEV_IDLE, NULL, NULL,
							cause);
                    if (modem_err) {
                       if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
                          buginf("\nISDN %s: CALL_DISC: MODEM ERROR %x: "
                                 "bchan %d, call id %x",
                                 DSL_TO_NAME(dsl), modem_err, bchan, call_id);
		    }
                    /*
                     * This is part of the code from isdn_call_disconnect.
                     * We need to clean up a bunch of things here.
                     */
                    isdn_info->call_is_voice = FALSE;
                    ddb = get_dialerdb(b_idb);
                    ddb->dialer_spoofing_int = FALSE;
                    ddb->dialer_spoofing_prot = FALSE;
                    ddb->dialer_lineup = FALSE;
                    TIMER_STOP(ddb->dialer_idle_timer);
                    TIMER_STOP(ddb->dialer_fast_idle_timer);
                    ddb->dialer_stats = NULL;
                    dialer_set_number_called(ddb, NULL);
                    bzero(ddb->remote_name, MAXSTR);
                    bzero(ddb->dial_reason, MAXSTR);
                    ddb = get_dialerdb(d_chan_idb);
                    /*
                     * if there is a modem error, most likely it is
                     * due to a call clearing collision.  Let VOICE_HANGUP
                     * in process_modem_command() increment the number of
                     * of available B channels.
                     */
                    if (!modem_err) {
                       isdn_increment_bchan(d_isdn_info); /* one more B */
                       dialer_decrement_call(ddb);   
		    }
                }
		break;
	    }
	}

	break;    

    case CALL_INCOMING:
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "CALL_INCOMING ");
	    isdn_buginf_flush();

	}
	bchan = extract_bchan(pkg);
	if (bchan == ISDN_Dchan) {
	    if(isdn_debug)
		buginf("\nISDN %s: Remote PRI Protocol Error: No B channel "
		       "assigned by switch (call id = %d)",
		       d_chan_idb->hw_short_namestring, 
                       call_id);
	    /*
	     * Cause 34, no channel available
	     */
	    process_rejectcall(call_id, NO_CIRCUIT_AVAILABLE, dsl);
	    break;
	}
	b_idb = d_isdn_info->b_chan_idb[bchan];
	if (b_idb == NULL)
	    break;
	isdn_info = b_idb->isdn_info;
	if (isdn_info == NULL)
	    break;
	remote_number = extract_calling_num(pkg);

	ddb = get_dialerdb(d_chan_idb);
	netddb = GET_NETDDB(ddb);

	minsum = get_dialer_minsum(ddb);
	max_available = ddb->dialer_num_link - minsum;
	used = get_dialer_used(ddb);
	/*
	 * This IS_CALL_ALLOWED() at this point is bogus since Dialer
	 * Profile binding hasn't still happened and the netddb we have
	 * in hand is the D Channel's and not the Dialer Profile above.
	 */

	if (DIALPOOL_MEMBER(ddb)) {
	    if (!(IS_CALL_ALLOWED(netddb) &&
		  (used < max_available))) {
		process_rejectcall(call_id, CALL_REJECTED, dsl);
		reg_invoke_isdn_stats_update(d_chan_idb,
					     REFUSED_CALL, remote_number);
		if (remote_number)
		    free(remote_number);
		if (isdn_debug || dialer_debug) {
		    buginf("\nISDN %s: Incoming call rejected, "
			   "exceeded max calls",
			   d_chan_idb->hw_short_namestring);
		}
		break;
	    
	    }
	}

        /*
         * If we are a member of a rotary group we can still do
         * caller id screening but it is done by the dialer code.
         * idb->hunt_group_master == NULL if we are NOT part of
         * a rotary group.
         */
        if(!dialer_calling_num(remote_number, netddb)) {
            process_rejectcall(call_id, CALL_REJECTED, dsl);
            reg_invoke_isdn_stats_update(d_chan_idb, REFUSED_CALL,
					 remote_number);
            if (remote_number != NULL)
                free(remote_number);
            d_isdn_info->num_screens++;
            if (isdn_debug)
                buginf("\nISDN %s: Incoming call rejected due to screening",
		       d_chan_idb->hw_short_namestring);
             break;
        }
	if(!verify_answer_number(pkg)) {
	    if(isdn_debug) {
		buginf("\nISDN %s: Event: Received a call from %s on B%d",
		       d_chan_idb->hw_short_namestring,
		       display_remote_number(remote_number, isdn_info),
		       (1+isdn_info->chantype));

		buginf("\n\t\tReject call, Called number does not "
		       "match config answer number");
	    }
	    process_rejectcall(call_id, CALL_REJECTED, dsl);
            reg_invoke_isdn_stats_update(d_chan_idb, REFUSED_CALL,
					 remote_number);
	    free(remote_number);
	    break;
	}

        if (d_isdn_info->voice_answer_data || d_isdn_info->voice_answer_modem)
            voice_answer = TRUE;
 	isdn_info->speed = parse_bearercap(pkg,
					   d_isdn_info->voice_answer_data, 
					   d_isdn_info->voice_answer_modem,
                                           &is_voice_bearer);
 	if (!(isdn_info->speed )) {
	    /* the bearercap was bad.  probably a voice call */
	    if(isdn_debug)
		buginf("\nISDN %s: Event: Received a call with a bad "
		       "bearercap from %s on B%d",
		       d_chan_idb->hw_short_namestring,
		       display_remote_number(remote_number, isdn_info),
		       (1 + isdn_info->chantype));
            /*
             * This is to pass NET5 pri homologation. I am
             * supposed to reject BCs requesting speech.
             * with a cause of 88. Video screening 
             * is handled in ccpri.c
             */
            if (SwitchType == PRI_NET5_STYPE) {
                /*
                 * Cause 88, Bearer Capability incompatible
                 */
                process_rejectcall(call_id, INCOMPATIBLE_DEST, dsl);
                reg_invoke_isdn_stats_update(d_chan_idb, REFUSED_CALL,
					     remote_number);
                free(remote_number);
                break;
            } else {
	        /*
	         * Cause 58, Bearer Capability not presently available
	         */
	        process_rejectcall(call_id, BEAR_CAP_NOT_AVAIL, dsl);
                reg_invoke_isdn_stats_update(d_chan_idb, REFUSED_CALL,
					     remote_number);
	        free(remote_number);
	        break;
            }
	}
	if((nsf = parse_network_spec_facil(pkg))) {
	    if(isdn_debug)
		buginf("\nISDN %s: Event: Received a call with an NSF",
		       d_chan_idb->hw_short_namestring);
	}

	/*
	 * If we are answering as V120 - make sure that we can set up a
	 * v120 call
	 */
	parse_llc(pkg, &llc);
	if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
	    display_llc(dsl, 'P', &llc);
	if ((d_isdn_info->answer_all_as_v120) || (llc.v120)) {
	    boolean good_v120;

	    good_v120 = TRUE;
	    if (llc.v120) {
		if ((llc.llc_valid == FALSE) ||
		    (!(reg_invoke_v120_start_interface(b_idb,
						       TRUE,
						       &llc.v120_info)))) {
		    good_v120 = FALSE;
		}
	    } else {
		if (!(reg_invoke_v120_start_interface(b_idb, FALSE, NULL))) {
		    good_v120 = FALSE;
		}
	    }
	    if (good_v120 == FALSE) {
		process_rejectcall(call_id, CALL_REJECTED, dsl);
		if (remote_number)
		    free(remote_number);
		break;
	    }
	}

       /*
        * If this PRI is a dialer pool member, attempt to bind to a dialer
        * profile now using pool configuration or CLI.  If we can't bind
        * and PPP or PAP/CHAP are not configured, reject the call.
        */
       if (DIALPOOL_MEMBER(ddb)) {
	   ddb = get_dialerdb(b_idb);
           if (!(dialer_match_pool_or_caller_id(ddb, remote_number) ||
                 dialer_may_match_name(b_idb))) {
               process_rejectcall(call_id, CALL_REJECTED, dsl);
               reg_invoke_isdn_stats_update(d_chan_idb, REFUSED_CALL, 
                                            remote_number);
               if (remote_number)
                   free(remote_number);
                 if (isdn_debug || dialer_debug) {
                     buginf("\nISDN %s: Incoming call rejected, unbindable",
                            d_chan_idb->hw_short_namestring);
                 }
                 break;
             }
       } 
	isdn_info->in_use = TRUE;
        /*
         * User defineable option:
         * We need to see if the user wants to override the incoming speed
         * if this is a voice call.
         */
        if ((is_voice_bearer) &&
            (isdn_sys_constants[dsl].voice_rate) && (voice_answer)) {
                isdn_info->speed = isdn_sys_constants[dsl].voice_speed;
                if(isdn_debug)
                    buginf("\nISDN %s: Event: Speech call adjusting speed
                           to user " "selection %dK", DSL_TO_NAME(dsl),
                           isdn_info->speed);
        }
	if(isdn_debug)
	    buginf("\nISDN %s: Event: Received a call from %s on "
		  "B%d at %d Kb/s", d_chan_idb->hw_short_namestring,
		  display_remote_number(remote_number, isdn_info),
		  (1+isdn_info->chantype), 
                  isdn_info->speed == ISDN_SPEED_VOICE ? 64 : isdn_info->speed);
        /*
         * For outgoing voice calls we know that the call_id
         * is 0xA000 or greater. For incoming calls the network
         * assigns the call_id so we cannot use it as a flag,
         * therefore I need to set a flag in the B channel
         * isdn_info structure.
         */
        isdn_info->call_is_voice = FALSE;
        if ((voice_answer == TRUE) && (isdn_info->speed == ISDN_SPEED_VOICE)) {          
            isdn_info->call_is_voice = TRUE;
            isdn_info->speed = ISDN_SPEED_64;
            isdn_info->call_id = call_id;
            isdn_info->ces = ces;
            if (isdn_info->remote_number != NULL)
                free(isdn_info->remote_number);
            isdn_info->remote_number = remote_number;
            local_number = extract_called_number(pkg);
            modem_err = reg_invoke_modem_report(d_chan_idb, call_id, ces,
						bchan, DEV_INCALL,
						remote_number, local_number,
						cause);
            if (modem_err) {
               if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
                  buginf("\nISDN %s: CALL_INCOMING: MODEM ERROR %x: "
                         "bchan %d, call id %x",
                         DSL_TO_NAME(dsl), modem_err, bchan, call_id);
	    }
             
            /*
             * Check to ensure that free B channels are
             * available before accepting voice call.
             * If not, then reject call.
             */
            d_chan_idb = isdn_idblist[dsl];
            d_isdn_info = d_chan_idb->isdn_info;
            if (d_isdn_info->num_avail_b <= 0) {
               modem_err = NO_CIRCUIT_AVAILABLE;  /* cause code 34 */
               if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
                  buginf("\nISDN %s: CALL_INCOMING, Voice: ERROR. No free channels",
                         DSL_TO_NAME(dsl));
            }

            if (modem_err) {
                process_rejectcall(call_id, modem_err, dsl);
                reg_invoke_isdn_stats_update(d_chan_idb, REFUSED_CALL, 
                                             remote_number);
                free(remote_number);
		/*
		 * Well, this is a problem.  We've just assigned 
		 * isdn_info->remote_number to remote_number, but
		 * now we free it.  Tsk tsk.  Remember to also clear
		 * isdn_info, or we'll free it again...
		 */
		isdn_info->remote_number = NULL;
                break;
            }

            process_modem_callproc(CALL_ACCEPT, call_id, bchan, dsl);

            if (local_number != NULL)
                free(local_number);
            /*
             * must be an incoming call...
             * Need to keep track of incoming call statistics.
             */
            ddb = get_dialerdb(d_chan_idb);
            isdn_decrement_bchan(d_isdn_info);   /* one less available */
            dialer_increment_call(ddb);     /* one more B chan. in use */
        } else {
            isdn_report(dsl, ISDN_INCOMING, call_id, bchan, remote_number, 
                        ISDN_SRC_ISDN, ces);
            process_pri_simple(CALL_ACCEPT, call_id, bchan, dsl);
        }
	break;

    case CALL_CONNECT:
	/*
	 * Received a Connect message, have to cut thru to the B-Channel
	 */
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "CALL_CONNECT ");
	    isdn_buginf_flush();
	}
	/*
	 * Check all the B-channel idb's to match the call_id for the
	 * channel received in the Call Proceeding message.
	 */
	for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
	    b_idb = d_isdn_info->b_chan_idb[channel];
	    if (b_idb == NULL)
		continue;
	    isdn_info = b_idb->isdn_info;
	    if (isdn_info == NULL)
		continue;
	    if (call_id == isdn_info->call_id) {
		bchan = isdn_info->chantype;
		if (detail_debug)
		    buginf("\nISDN %s: Event: found callid for chan %d", 
                           d_chan_idb->hw_short_namestring, bchan);
		isdn_info->in_use = TRUE;	    
		/*
		 * There are two ways to detect if this is a modem voice
		 * call or not. If it was an outgoing voice call than
		 * the call_id will be 0xA000 or greater. If it was an
		 * incoming voice call the isdn_info->call_is_voice will
		 * be TRUE.
		 */
		if ((isdn_info->call_is_voice == FALSE) &&
                    (call_id < ISDN_MODEM_CALL_ID_MIN)) {
		    isdn_report(dsl, ISDN_CONNECT, call_id, bchan, NULL, 
				ISDN_SRC_UNKNOWN, ces);
		} else {
		    modem_err = reg_invoke_modem_report(d_chan_idb, call_id,
							ces, bchan,
							DEV_CONNECTED,
							NULL, NULL, cause);
		    if (modem_err) {
                       if (isdn_debug || isdn_debug_ok(q931_debug, dsl))
                          buginf("\nISDN %s: CALL_CONNECTED: MODEM ERROR %x: "
                                 "bchan %d, call id %x",
                                 DSL_TO_NAME(dsl), modem_err, bchan, call_id);
		   } 
		}
		break;
	    }
	}
	/*
	 * Make sure that we found the call_id for this call. If
	 * the channel made it to 23, then call_id couldn't be found.
	 */
	if(channel == ISDN_MAX_CHAN) {
	    if(isdn_debug)
		buginf("\nISDN %s: PRI Error: call_id not found", 
                       d_chan_idb->hw_short_namestring);
	    process_simple(CALL_REJECTION, call_id, bchan, ces, dsl);
	}
	break;

    case CALL_PROGRESS:
	/*
	 * Received a Progressing message. This includes a Connect,
	 * Connect Ack, Progress, and Alerting. 
	 */
	if(isdn_debug) {
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    isdn_received_msg, d_chan_idb->hw_short_namestring,
		    "CALL_PROGRESSing");
	    isdn_buginf_flush();
	}
	if(detail_debug) {
	    buginf("\nChecking the channel in the CALL_PROGRESSing\n");
	    for(i=0; i <= pkg->InfoLen; i++ ) {
		buginf("%x ", pkg->PkgInfo[i]);
	    }
	}
	break;
    case CALL_OUTGOING:
    case CALL_REJECTION:
	if(isdn_debug)
	    buginf("\nISDN %s: received message %x",
		   d_chan_idb->hw_short_namestring, 
                   pkg->MsgType);
        call_tbl = isdn_find_cid_table(d_chan_idb, call_id);
        if (call_tbl) 
	    isdn_carrier_timeout(d_chan_idb, call_tbl, call_tbl);
	break;
    case CHAN_STATUS:
        /*
         * PkgInfo[6]=CHANNEL_STATUS_IE (0x30)
         *         7 = length
         *         8 = B-channel
         *         9 = action
	 * by the time the Restart gets here, the channel has already been
	 * cleared, just report that the Restart has occured.
         */
        if(isdn_debug)
	    buginf("\nISDN %s: CHAN_STATUS, ",
		   d_chan_idb->hw_short_namestring);
        if (pkg->PkgInfo[6] == CHAN_STATUS_IE) {
            if (pkg->PkgInfo[9] == CHAN_RESTARTED) {
                if(isdn_debug)
                    buginf("B-channel %d Restarted, action=%d",
                           pkg->PkgInfo[8], pkg->PkgInfo[9]);
            }
        }
        break;

	/*
	 * These cases really aren't handled yet, but keep the entry
	 * for them just in case.
	 */
    case CALL_UUI:
    case CALL_FACILITY:
    case CALL_FACILITY_ACK:
    case CALL_FACILITY_REJ:
    case CALL_CONGESTION:
    case CALL_NOTIFY:
    case CALL_INFO:
    case CALL_FACILITY_REGISTER:
    case CALL_REGISTER_ACK:
    case CALL_REGISTER_REJ:
    case CALL_FACILITY_CANCEL:
    case CALL_CANCEL_ACK:
    case CALL_CANCEL_REJ:
    case CALL_RELEASE:
	if(isdn_debug)
	    buginf("\nISDN %s: received message %x",
		   d_chan_idb->hw_short_namestring, 
                   pkg->MsgType);
	break;

    case CALL_CLEARED:
	if(isdn_debug)
	    buginf("\nISDN %s: entering process_rxstate, CALL_CLEARED", 
                   d_chan_idb->hw_short_namestring);
	/*
	 * Now let's check all the B-channels for the call id.
	 * First make sure that the isdn_info is there and hasn't
	 * been removed with the no pri-group command.
	 */
	for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
	    b_idb = d_isdn_info->b_chan_idb[channel];
	    if (b_idb == NULL)
		continue;
	    isdn_info = b_idb->isdn_info;
	    if (isdn_info == NULL)
		continue;
	    if (call_id == isdn_info->call_id) {
		ces = isdn_info->ces;
		isdn_info->ces = SIGNALING_CES;
		isdn_info->in_use = FALSE;
		bchan = isdn_info->chantype;
		break;
	    }
	}
	/*
	 * Clear the call. If a channel wasn't found then send a ZERO for 
	 * the channel and check to see if the dialer wait for carrier 
	 * should be removed as well.
	 */
	if(channel < ISDN_MAX_CHAN) {
	    /* Found the B-channel, so disconnect it. */               
	    call_tbl = isdn_find_cid_table(d_chan_idb, call_id);
	    if (call_tbl)
		isdn_carrier_timeout(d_chan_idb, call_tbl, call_tbl);
	    isdn_report(dsl, ISDN_DISCON, call_id, bchan, NULL, 
			ISDN_SRC_UNKNOWN, ces);
	} else {
	    /*
	     * No B-channel found, must be an outstanding call, have to
	     * stop the wait for carrier as well.
	     */
	    isdn_report(dsl, ISDN_DISCON, call_id, NO_CHANNEL, NULL, 
			ISDN_SRC_UNKNOWN, ces);
	}
	break;

    default:
	if(isdn_debug)
	    buginf("\nISDN %s: received unknown message: %d ",
		   d_chan_idb->hw_short_namestring, pkg->MsgType);
	if(pkg->MsgType == PRIM_BTYPE) {
	    /* They sent me a primitive?  What will I do with that? */
	    if(isdn_debug)
		buginf("\nISDN %s: Error: Rx buf type PRIM_BTYPE when "
                       "expected a package", d_chan_idb->hw_short_namestring);
	    LIF_Dispose(pkg);
	    return;
	} else {
	    /* Some unexpected package */
	    if(isdn_debug)
		buginf("\nISDN %s: Software Error: Unknown message "
		       "from call control (2, %d)",
		       d_chan_idb->hw_short_namestring,
		       pkg->MsgType);
	    break;
	}
    }

    LIF_FreePkg(pkg);
}

/*
 *	Report a call event to the router
 */
static void
isdn_report (int dsl, enum BriMsgType msg, UINT16 call_id, enum Chan bchan,
	     char *remote_number, enum rem_src remote_src, int ces)
{
    hwidbtype *idb;
    hwidbtype *tmpidb;
    isdn_bri_t *isdn_info;
    int channel, mask, speed;
    call_id_string_t *call_tbl;

    speed = 0;
    if(detail_debug)
       buginf("\nISDN %s: isdn report  bchan %d, callid 0x%x", 
	      DSL_TO_NAME(dsl), bchan, call_id);
    switch(msg) {
    case ISDN_CONNECT:
    case ISDN_INCOMING:
	if(!is_pri) {
	    idb = (bchan == B1) ?
		isdn_idblist[dsl]->isdn_info->b1_chan_idb : 
		    isdn_idblist[dsl]->isdn_info->b2_chan_idb;
	    isdn_info = idb->isdn_info;
	    isdn_info->call_id = call_id;
	    isdn_info->ces = ces;
	    if (isdn_info->enable_bri_channel)
		(*isdn_info->enable_bri_channel)(idb);
	} else {
	    /* 
	     * We are a PRI interface.
	     */
	    idb = BCHAN_TO_IDB(dsl, bchan);
	    isdn_info = idb->isdn_info;
	    isdn_info->call_id = call_id;
	    isdn_info->ces = ces;
	    /*
	     * If we are running Primary Rate I know that all my timeslots
	     * might not be active yet so let's set the speed.
	     */
            mask = BCHAN_TO_BITFIELD(idb->isdn_info->chantype);
            /* 
             * If this is an incoming call then the speed is
             * stored in isdn_info->speed. If this is an outgoing
             * call then the speed is in the dialer call_tbl struct.
             */
            if (msg == ISDN_CONNECT) {
                call_tbl = isdn_find_cid_table(isdn_idblist[dsl], call_id);
		if (call_tbl)
                    speed = call_tbl->speed;
            } else {
                speed = isdn_info->speed;
            }
            reg_invoke_isdn_map_channel(idb->type, idb, mask, speed);
	    set_default_interface_bandwidth(idb->firstsw,
					    (speed == ISDN_SPEED_56) ?
					    VISIBLE_BRI_BANDWIDTH_56 :
					    VISIBLE_BRI_BANDWIDTH_64);
            reg_invoke_digital_call_report(idb, bchan, DEV_CONNECTED);
	}
	if(msg == ISDN_INCOMING) {
	    if(isdn_info->remote_number != NULL)
		free(isdn_info->remote_number);
	    isdn_info->remote_number = remote_number;
	    isdn_info->remote_src = remote_src;
	}
	isdn_call_connect(idb);
	idb->counters.transitions++;
	break;
		
    case ISDN_DISCON:
	/*
	 *	First, find the B channel idb.  Yes, it's ugly,
	 *  but it's really not that bad.  We only know the D
	 *  channel idb.  We have to dereference it to get the
	 *  isdn_info, which points to the B channel idb.  It's
	 *  all the casting that's ugly.
	 */
	if(!is_pri) {
	    if(isdn_idblist[dsl]->isdn_info->b1_chan_idb->isdn_info->call_id
	       == call_id) {
		idb = isdn_idblist[dsl]->isdn_info->b1_chan_idb;
		bchan = B1;
	    } else if(isdn_idblist[dsl]->isdn_info->b2_chan_idb->isdn_info->call_id == call_id) {
		idb = isdn_idblist[dsl]->isdn_info->b2_chan_idb;
		bchan = B2;
		if (idb->isdn_info && idb->isdn_info->disable_bri_channel)
		    (*idb->isdn_info->disable_bri_channel)(idb);
	    } else {
		if(isdn_debug) {
		    buginf("\nISDN %s: Got a disconnect on a non-existent "
                           "call (call id = 0x%x).", DSL_TO_NAME(dsl), 
                           call_id);
		    buginf("\nThis probably is a call that we placed that "
			   "never got answered.");
		}
		return;		/* no such call */
	    }
	    if (idb->isdn_info && idb->isdn_info->disable_bri_channel)
		(*idb->isdn_info->disable_bri_channel)(idb);
	    idb->isdn_info->in_use = FALSE;
	    idb->isdn_info->ces = SIGNALING_CES;
	    isdn_call_disconnect(idb);
	    idb->counters.transitions++;
	} else {
	    /*
	     * We are a PRI interface lets find the idb.
	     */
	    bchan = -1;
	    if (isdn_idblist[dsl] && isdn_idblist[dsl]->isdn_info) {
		for(channel = 0; channel < ISDN_MAX_CHAN; channel++) {
		    if (isdn_idblist[dsl]->isdn_info) {
			tmpidb =
			    isdn_idblist[dsl]->isdn_info->b_chan_idb[channel];
			/* 
			 * Need to make sure that this is a configured 
			 * timeslot and it's PRI.
			 */
			if(tmpidb && tmpidb->isdn_info) {
			    if(tmpidb->isdn_info->call_id == call_id) {
				bchan = tmpidb->isdn_info->chantype;
				break;
			    }
			}
		    }
		}
	    }
	    /* 
	     * This can occur if we placed a call and T303 has expired
	     * twice. The call never connected so we never cut through.
	     * Therefore we have no B-channel.
	     */
	    if(bchan == -1) {
		if(isdn_debug) {
		    buginf("\nISDN %s: Got a disconnect on a non-existent "
                           "call (call id = 0x%x).", DSL_TO_NAME(dsl), 
                           call_id);
		    buginf("\nThis probably is a call that we placed "
			   "that never got answered.");
		}
		/*
		 * Make sure the dialer wait-for-carrier timer is disabled,
		 * just in case the call was not successfull/incomplete.
		 */
                call_tbl = isdn_find_cid_table(isdn_idblist[dsl], call_id);
                if (call_tbl) {
                    isdn_carrier_timeout(isdn_idblist[dsl], call_tbl, call_tbl);
                }
		return; 	/* no such call */
	    }
	    idb = isdn_idblist[dsl]->isdn_info->b_chan_idb[bchan];

            /*
             * NET5 homologation fix for the 30 chan teardown.
             */
            if (idb == NULL)
                break;
            if (idb->isdn_info == NULL)
                break;
	    idb->isdn_info->in_use = FALSE;
	    idb->isdn_info->ces = SIGNALING_CES;

	    /*
	     * If we are running Primary Rate we know that all timeslots
	     * are initialized to 64kb. In case this was not a 64 kb call 
	     * make the timeslot 64 kb.
	     */
	    if (IS_PRI(idb)) {
		ulong mask;
		mask = BCHAN_TO_BITFIELD(idb->isdn_info->chantype);
		reg_invoke_isdn_map_channel(idb->type, idb, mask,
					    ISDN_SPEED_64);
		set_default_interface_bandwidth(idb->firstsw,
						VISIBLE_BRI_BANDWIDTH_64);
                reg_invoke_digital_call_report(idb, bchan, DEV_IDLE);
	    }

	    isdn_call_disconnect(idb);
	    idb->counters.transitions++;
	}
	break;
		
    default:
	buginf("\nISDN %s: Software Error: non-existent case %d in "
               "isdn_report.", DSL_TO_NAME(dsl),msg);
	break;
    }
}

/*
 * Return 1 if the HLC shows this call is not for us, 0 if it is
 */
static int
parse_hlc (PKG_ID pkg)
{
    BYTE *ie;
    int i, b1, b2;

    if((ie = my_Get_IE(pkg, HIGH_LAYER_COMPAT_IE)) == NULL)
	return 0;
	
    if(ie[1] < 2)		/* test the length */
	return 0;
	
    /*
     *		The HLC is mandatory in the VN2 world, so we must check
     *	for a compatible one (0xc1ff and 0xd180).  In all other cases
     *	a HLC is bad, as none of them mean "router" so whatever it says,
     *	it isn't for us.
     */
    switch(SwitchType) {
    case VN2_STYPE:
    case VN3_STYPE:
	b1 = ie[2] & 0x7f;
	for(i = 2 ; (i < (2 + ie[1])) && !(ie[i] & 0x80); i++)
	    ;
	if(++i >= 2 + ie[1])
	    return 1;
	b2 = ie[i];
	b1 |= 0x80;
	b2 |= 0x80;
	return !((b1 == 0xc1 && b2 == 0xff) ||
		 (b1 == 0xd1 && b2 == 0x80) ||
		 (b1 == 0xc0 && b2 == 0xc1)); /* for ACT/U6/AU6-CN1 */

    case BRI_NET3_STYPE:
	/*
	 * Ignore it, because we send it out for Norway, we can
	 * get it back in.
	 */
	return 0;
			
    case NTT_STYPE:
    case BRI_NI1_STYPE:		/*=== len error legal for NI1 */
	return 0;

    default:
	return 1;
    }
}

/*
 *  parse_llc
 *  Parse the low layer capability, and return the speed of the connection.
 *  return codes are contained in the llc structure pointed to.  If the
 *  llc_valid field is set, we've been able to parse the LLC.  If there
 *  is no LLC, we return ISDN_SPEED_64 in the speed field, and no V.120
 *  info in v120 field.
 */
static void parse_llc (PKG_ID pkg, llc_parse_type *llc)
{
    BYTE *ie;
    int ix, llc_len;

    llc->llc_valid = TRUE;
    llc->v120 = FALSE;
    llc->speed = ISDN_SPEED_64;
    llc->llc_present = TRUE;
    if((ie = my_Get_IE(pkg, LOW_LAYER_COMPAT_IE)) == NULL) {
	/* No llc - we indicate default speed and a valid llc */
	llc->llc_present = FALSE;
	return;
    }

    llc_len = ie[1];
    if(llc_len < 2) {		/* test the length */
	llc->llc_valid = FALSE;
	return;
    }

    llc_len += 2;	    /* use the length as a buffer index comparison */
	
    /* verify the information transfer capability */
    switch(ie[2] & 0x1f) {
    case 0x00:			/* speech */
    case 0x10:			/* 3.1KHz audio */
    case 0x11:			/* 7KHz audio */
    case 0x18:			/* Video */
    default:
	llc->llc_valid = FALSE;
	return;
		
    case 0x08:			/* unrestricted digital information */
    case 0x09:			/* restricted digital information */
	break;
    }

    /* advance to octet 4 */
    for(ix = 2 ; !(ie[ix] & 0x80) && (llc_len > ix) ; ix++)
	;
    ix++;
    if(llc_len <= ix) {
	llc->llc_valid = FALSE;
	return;
    }

    /* verify the information transfer rate */
    if((ie[ix] & 0x1f) != 0x10) {	/* 64Kb/s */
	llc->llc_valid = FALSE;
	return;
    }
	
    /* advance to octet 5 */
    for( ; !(ie[ix] & 0x80) && (llc_len > ix) ; ix++)
	;

    ix++;
    if ((llc_len <= ix) ||
	(ie[ix] & 0x80)) {
	/* no extension to user rate */
	/* we set the speed and the fact that the llc was valid above */
	if (llc->v120)
	    llc->llc_valid = FALSE;
	return;
    }

    /* check layer 1 protocol */
    if ((ie[ix] & 0x1f) == 0x08) {
	llc->v120 = TRUE;
    }
	
    if(llc_len <= ++ix) {
	llc->llc_valid = FALSE;
	return;		/* bad length */
    }

    /* octet 5a */
	
    /* examine the user rate */
    switch(ie[ix] & 0x1f) {
    case 0x0f:			/* 56Kb/s */
	llc->speed = ISDN_SPEED_56;
	return;
    default:
	llc->v120_info.speed = ie[ix] & 0x1f;
	if (llc->v120)
	    break;
	llc->llc_valid = FALSE;
	return;		/* we don't do any of the others */
    }
    /* Now processing V.120 information */
    llc->v120_info.sync = (ie[ix] & 0x40) ? FALSE : TRUE;
    if(llc_len <= ++ix) {
	llc->llc_valid = FALSE;
	return;		/* bad length */
    }
    /* octet 5b */
    llc->v120_info.header = (ie[ix] & 0x40) ? TRUE : FALSE;
    llc->v120_info.multiframe = (ie[ix] & 0x20) ? TRUE : FALSE;
    llc->v120_info.mode = (ie[ix] & 0x10) ? TRUE : FALSE;
    llc->v120_info.lli_negotiation = (ie[ix] & 0x08) ? TRUE : FALSE;
    llc->v120_info.assign = (ie[ix] & 0x04) ? TRUE : FALSE;
    if(llc_len <= ++ix) {
	llc->llc_valid = FALSE;
	return;		/* bad length */
    }
    /* octet 5c */
    llc->v120_info.stopbits = (ie[ix] >> 5) & 0x03;
    llc->v120_info.databits = (ie[ix] >> 3) & 0x03;
    llc->v120_info.parity = ie[ix] & 0x07;
}

static uchar YORN (boolean type)
{
    if (type) return('Y');
    return('N');
}

static void display_llc (int dsl, uchar type, llc_parse_type *llc)
{
    if (llc->llc_present == FALSE)
	return;
    if (llc->llc_valid == FALSE) {
	buginf("\nISDN %s: llc is invalid", type, DSL_TO_NAME(dsl));
	return;
    }
    buginf("\nISDN %s: llc valid, speed %d", type, DSL_TO_NAME(dsl), 
           llc->speed);

    if (llc->v120) {
	buginf(" call type is v.120");
	buginf("\nspeed:%d sync:%c header:%c multiframe:%c",
	       llc->v120_info.speed, YORN(llc->v120_info.sync),
	       YORN(llc->v120_info.header), YORN(llc->v120_info.multiframe));
	buginf("\nmode %d stop %d data %d parity %d",
	       llc->v120_info.mode, llc->v120_info.stopbits,
	       llc->v120_info.databits, llc->v120_info.parity);
    }
}
/*
 *  Parse the bearer capability, and return the speed of the connection.
 *  Return 0 on error.
 */
static int
parse_bearercap (PKG_ID pkg, boolean voice_data, boolean voice_modem,
                 boolean * is_voice_bearer)
{
    BYTE *ie;
    int i;

    if((ie = my_Get_IE(pkg, BEARER_CAPABILITY_IE)) == NULL)
	return 0;
	
    if(ie[1] < 2)		/* test the length */
	return 0;

    *is_voice_bearer = FALSE;
	
    /* verify the information transfer capability */
    switch(ie[2] & 0x1f) {
    case 0x00:                  /* speech */
    case 0x10:                  /* 3.1KHz audio */
    case 0x11:                  /* 7KHz audio */
        *is_voice_bearer = TRUE;
        if (voice_data)
            break;
        if (voice_modem)
            return ISDN_SPEED_VOICE;
        else
            return 0;

    case 0x18:			/* Video */
	return 0;
		
    case 0x08:			/* unrestricted digital information */
    case 0x09:			/* restricted digital information */
	break;
		
    default:
	return 0;
    }

    /* advance to octet 4 */
    for(i = 2 ; !(ie[i] & 0x80) && (ie[1] + 2 > i) ; i++)
	;
    i++;
    if(ie[1] + 2 <= i)
	return 0;

    /* verify the information transfer rate */
    if((ie[i] & 0x1f) != 0x10)	/* 64Kb/s */
	return 0;
	
    /* advance to octet 5 */
    for( ; !(ie[i] & 0x80) && (ie[1] + 2 > i) ; i++)
	;
    i++;
    if(ie[1] + 2 <= i)
	return ISDN_SPEED_64;
	
    if(ie[i] & 0x80)
	return ISDN_SPEED_64;	/* no extension to user rate */
	
    if(ie[1] + 2 <= ++i)
	return 0;		/* bad length */
	
    /* examine the user rate */
    switch(ie[i] & 0x1f) {
    case 0x0f:			/* 56Kb/s */
	return ISDN_SPEED_56;
		
    default:
	return 0;		/* we don't do any of the others */
    }
}

/*
 * Parse the Network Specific Facilities, and return true if found or a
 * false if not found
 *  Return 0 on error.
 */
static int
parse_network_spec_facil (PKG_ID pkg)
{
    BYTE *ie;

    if((ie = my_Get_IE(pkg, NETWORK_FACILITY_IE)) == NULL)
	return 0;
    else
	return 1;
}

/*
 *    Parse the progress IE and return the information field in Octet 4
 *  Return 0 if none present. Return 0 if an error is found.
 */
static int
parse_progress (pkg)
register PKG_ID pkg;
{
    BYTE *ie;
 
    if((ie = my_Get_IE(pkg, PROGRESS_IND_IE)) == NULL)
	return 0;

    if(ie[1] < 2)           /* validate the length */
	return 0;

    return (ie[3]);
}
 
/*
 *  Parse the 1TR6 Service IE, and return the speed of the connection.
 *  Return 0 on error.
 */
static int
parse_service (PKG_ID pkg)
{
    BYTE *ie;
    int speed = 0;

    if((ie = my_Get_IE(pkg, SERVICE_INDICATOR_IE)) == NULL) {
	return 0;
    }
	
    if(ie[1] < 2){		/* test the length */
	return 0;
    }

    /* verify the service ie, only allow a 64k call. beware!!! also 
       checked in ccbri.c B1tr6_Service_Ind */

    switch(ie[2] & 0x1f) {
	/* only allow the 64 kbps ie */
    case 0x07:			/* Datenvebertraagung (64 kbps) */

	/* the additionl info is checked, only 2 cases allowed. */
	if ((ie[3] & 0xff) == 0){
	    speed = ISDN_SPEED_64;
	}
	else if ((ie[3] & 0xff) == 0xA9) {
	    speed = ISDN_SPEED_56;
	}
	break;

	/* don't allow any of the following for 1tr6 */
	/* case 0x01:	Service Fernsprechen, 3.1 kHz */
	/* case 0x02:	a/b services */
	/* case 0x03:	x21 services */
	/* case 0x04:	Group 4 telefax */
	/* case 0x05:	Interactive videotex  */
	/* case 0x08:	 x25 services, not supported */		
	/* case 0x09:	Teletex 64 */
	/* case 0x0a:	Mixed Mode */
	/* case 0x0d:	Telecontrol */
	/* case 0x0e:	Graphic telephone services */
	/* case 0x0f:	Interactive videotex (new std) */
	/* case 0x10:	Videophone */

    default:
	speed = 0;
	break;
    }
    return speed;
}

/*
 *  Extract the B channel from the passed package.  Return the D channel
 *  on error.
 */
static enum Chan
extract_bchan (PKG_ID pkg)
{
    BYTE *ie;
    int ix;
    int bcount;
    uchar bmask;
    enum Chan bchan;

    bcount = 0;
    bmask = 1;
    if((ie = my_Get_IE(pkg, CHANNEL_ID_IE)) == NULL)
	return ISDN_Dchan;

    if(detail_debug) {
	buginf("\n  extracted channel ie[0-8]= ");
	for (ix=0; ix<=8; ix++)
	    buginf("%x ", ie[ix]);
	buginf("\n");
    }

    /*
     * For BRI, the channel is in ie[2], the lowest 2 bits.
     * ie[2] checked to see if it is PRI or BRI. A 0x20 indicates PRI
     * ie[3] - interface, ie[4] - type of B, ie[5] - channel/slotmap
     */
    if (ie[2] & PRI_INTERFACE) {
        /*
         * We know that we are PRI. If we are running NTT
         * PRI we also know that there is no octet 3.1 and
         * this is an incoming call so we need to use a
         * slotmap to get the b-channel. SLOT_MAP is 0x10.
         */
        if ((ie[3] & SLOT_MAP) &&  (SwitchType == PRI_NTT_STYPE)) {
            /* 
             * Let's start with the lowest B first. ie[6]
             */
            for (ix = 0; ix < 8; ix++) {
                if (ie[6] & bmask)
                    return bcount;
                bcount++;
                bmask = bmask << 1;
            }
            bmask = 1;
            for (ix = 0; ix < 8; ix++) {
                if (ie[5] & bmask)
                    return bcount;
                bcount++;
                bmask = bmask << 1;
            }
            bmask = 1;
            for (ix = 0; ix < 8; ix++) {
                if (ie[4] & bmask)
                    return bcount;
                bcount++;
                bmask = bmask << 1;
            }
        }

        /*
         * if interface defined, int in ie[3], channel in ie[4]
         * no interface defined, channel in ie[3]
         */
        if (ie[2] & 0x40)
            bchan = (ie[5] & 0x1f); /* chan w interface */
        else
            bchan = (ie[4] & 0x1f); /* chan wo interface */

	/*
	 * Australia is different and causes problems. Actually uses
	 * B1-B30, NET5 skips B16 and uses B1-B15, B17-B31.
	 */
	if (SwitchType == PRI_TS014_STYPE) {
	    if (bchan <= 15)
	        bchan -= 1;
	} else {
	    bchan -= 1;
	}
        return (bchan);
    } else {
	switch(ie[2] & 3) {
	case 1:
	    return B1;
	case 2:
	    return B2;
	default:
	    return ISDN_Dchan;
	}
    }
}
/*
 *	Extract the calling number.  Return NULL if non-existent.
 */
static char *
extract_calling_num (PKG_ID pkg)
{
    BYTE *ie;
    char *retval;
    int offset;

    if((ie = my_Get_IE(pkg, CALLER_NUMBER_IE)) == NULL)
	return NULL;

    /*
     * check to see if byte 3A is there.  If so, everything is one byte
     * further than we expect.  Also, just because the IE is there,
     * doesn't mean a number is there, so check the length also.  It
     * may be the case that presentation is restricted.
     *
     * element 3 is either byte 3A or the first digit of the calling
     * number.
     */

    if(ie[3] & 0x80)
	offset = 2;
    else
	offset = 1;

    if((ie[1] - offset) == 0)
	return NULL;

    /*
     * This is the fix for CSCdi28158. We have already referenced
     * IE[3] above but the IE may not be that long. The bug
     * that would cause the crash was a malloc (0) and then a
     * free of that block at a later time. If the IE len is less
     * than 2 IE[3] is not valid and return NULL.
     */
    if(ie[1] < 2)
        return NULL;

    retval = malloc(ie[1] - offset + 1); /* +1 for the ending 0 */
    if (retval != NULL) {
        bcopy(&ie[2 + offset], retval, ie[1] - offset);
        retval[ie[1] - offset] = '\0';
    }
    return retval;
}

/* extract an information element from a package */
static BYTE *
my_Get_IE (pkg, target)
    PKG_ID pkg;
    BYTE target;
{
    static BYTE retary[64];	/* big enough */
    int i;

    for(i = 6 + pkg->PkgInfo[5] ; i < pkg->InfoLen ; ) {
	if(pkg->PkgInfo[i] == target) {
	    assert(2 + pkg->PkgInfo[i + 1] <= sizeof(retary));
	    assert(i + pkg->PkgInfo[i + 1] < pkg->InfoLen);

	    bcopy(&pkg->PkgInfo[i], retary,
		  min(sizeof(retary), 2 + pkg->PkgInfo[i + 1]));
	    return retary;
	} else {
	    /*
	     * this is where the Sending Complete IE messes us up, because
	     * it is a single IE.
	     */
	    if((pkg->PkgInfo[i] & 0x80) == 0x80)
		i += 1;		/* single octet */
	    else
		i += 2 + pkg->PkgInfo[i + 1]; /* variable length */
	}
    }
    return NULL;
}

/* extract an information element from a package */
BYTE *
my_Get_IE_pkt (pkt, target)
    PKT_ID pkt;
    BYTE target;
{
    static BYTE retary[64];	/* big enough */
    int i;

    for(i = 3 + pkt->Info[1] ; i < pkt->InfoLen ; ) {
	if(pkt->Info[i] == target) {
	    assert(2 + pkt->Info[i + 1] <= sizeof(retary));
	    assert(i + pkt->Info[i + 1] < pkt->InfoLen);

	    bcopy(&pkt->Info[i], retary,
		  min(sizeof(retary), 2 + pkt->Info[i + 1]));
	    return retary;
	} else {
	    if (SwitchType != BRI_1TR6_STYPE) {
		i += 2 + pkt->Info[i + 1];
	    } else /* adjust for CodeSet IEs and single octet IEs */ {
		if((pkt->Info[i] & 0x80) == 0x80)
		    i += 1;	/* single octet */
		else
		    i += 2 + pkt->Info[i + 1]; /* variable length */
	    }
	}
    }
    return NULL;
}

PUBLIC void itoa (int val, char *str, int base)
{
    PRIVATE void inttoasc(unsigned, char *, int);

    (void) inttoasc(val, str, base);
}

/* value to be converted to a string, 
 * string to write to,
 * base to convert in
 */
static void inttoasc (unsigned val, char *str, int base)
{
    char tmp[11];
    char sign = ' ';
    char *right;

    tmp[10] = '\0';
    right = &tmp[9];

    if (base < 0) {
	if ((int) val < 0) {
	    val = - ((int) val);
	    sign = '-';
	}
	base = -base;
    }

    do {
	*right-- = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" [val % base];
    } while ((val /= base) != 0);

    *str++ = sign;
    right++;
    while (*right)
	{
	    *str++ = *right++;
	}
    *str = '\0';
}

void
isdn_exit (int x)
{
    buginf("\n ISDN Software Error: call to isdn_exit(%d)", x);

    /*
     * We used to reload the system.
     * Don't do that.  Bad Idea.  Try to keep running
     */
}

/*
 *	Compiles traffic statistics 
 */
static void
do_statistics (paktype *p, enum direction dir, int dsl)
{
    /*
     *	This probably ought to be in the main thread of the code,
     *  not part of the statistics subroutine.  On the other hand,
     *  the two will always be called together, so what the hey?
     */
    if (isdn_debug_ok(q921_debug || q931_debug, dsl))
	isdn_display(p, dir, dsl);

    if(p->datagramstart[2] & 1) {
	switch(p->datagramstart[2] & 0xef) {
	case 0x01: statistics[Q921_RR][dir]++;		break;

	case 0x03:		/* UIs are special--they can be USETUP msgs */
	    statistics[Q921_UI][dir]++;
	    if((p->datagramstart[0] & 0xfc) == 0) /* check for SAPI == 0 */
		if(p->datagramstart[5 + p->datagramstart[4]] == 5)
		    statistics[Q931_USETUP][dir]++;
	    break;

	case 0x05: statistics[Q921_RNR][dir]++;		break;
	case 0x09: statistics[Q921_REJ][dir]++;		break;
	case 0x0f: statistics[Q921_DM][dir]++;		break;
	case 0x43: statistics[Q921_DISC][dir]++;	break;
	case 0x63: statistics[Q921_UA][dir]++;		break;
	case 0x6f: statistics[Q921_SABME][dir]++;	break;
	case 0x87: statistics[Q921_FRMR][dir]++;	break;
	case 0xaf: statistics[Q921_XID][dir]++;		break;
	default:	break;
	}
    } else {
	/* it's an INFO frame */
	statistics[Q921_INFO][dir]++;
	switch(p->datagramstart[6 + p->datagramstart[5]]) {
	case 0x01: statistics[Q931_ALERTING][dir]++;	break;
	case 0x02: statistics[Q931_CALL_PROC][dir]++;	break;
	case 0x07: statistics[Q931_CONNECT][dir]++;	break;
	case 0x0f: statistics[Q931_CONNECT_ACK][dir]++;	break;
	case 0x03: statistics[Q931_PROGRESS][dir]++;	break;
	case 0x05: statistics[Q931_SETUP][dir]++;	break;
	case 0x0d: statistics[Q931_SETUP_ACK][dir]++;	break;

	case 0x26: statistics[Q931_RESUME][dir]++;	break;
	case 0x2e: statistics[Q931_RESUME_ACK][dir]++;	break;
	case 0x22: statistics[Q931_RESUME_REJ][dir]++;	break;
	case 0x25: statistics[Q931_SUSPEND][dir]++;	break;
	case 0x2b: statistics[Q931_SUSPEND_ACK][dir]++;	break;
	case 0x21: statistics[Q931_SUSPEND_REJ][dir]++;	break;
	case 0x20: statistics[Q931_USER_INFO][dir]++;	break;

	case 0x45: statistics[Q931_DISCONNECT][dir]++;	break;
	case 0x4d: statistics[Q931_RELEASE][dir]++;	break;
	case 0x5a: statistics[Q931_RELEASE_COMP][dir]++;	break;
	case 0x46: statistics[Q931_RESTART][dir]++;	break;
	case 0x4e: statistics[Q931_RESTART_ACK][dir]++;	break;

	case 0x60: statistics[Q931_SEGMENT][dir]++;	break;
	case 0x79: statistics[Q931_CONG_CONT][dir]++;	break;
	case 0x7b: statistics[Q931_INFORMATION][dir]++;	break;
	case 0x62: statistics[Q931_FACILITY][dir]++;	break;
	case 0x6e: statistics[Q931_NOTIFY][dir]++;	break;
	case 0x7d: statistics[Q931_STATUS][dir]++;	break;
	case 0x75: statistics[Q931_STATUS_ENQ][dir]++;	break;
	default:	break;
	}
    }
}

/*
 *	Show line statistics for BRI
 */
void
show_statistics ()
{
    static char *const frames[NUM_STAT_ENTRIES] = {
	/* these must be in the exact order that their values are in */
	"INFO", "RR", "RNR", "REJ", "SABME", "DM", "UI", "DISC", "UA",
	"FRMR", "XID", "ALERTING", "CALL_PROC", "CONNECT",
	"CONNECT_ACK", "PROGRESS", "SETUP", "SETUP_ACK", "RESUME",
	"RESUME_ACK", "RESUME_REJ", "SUSPEND", "SUSPEND_ACK",
	"SUSPEND_REJ", "USER_INFO", "DISCONNECT", "RELEASE",
	"RELEASE_COMP", "RESTART", "RESTART_ACK", "SEGMENT",
	"CONG_CONT", "INFORMATION", "FACILITY", "NOTIFY",
	"STATUS", "STATUS_ENQ"
	};
    int i;

    buginf("\nLayer 2 Frames                   TX             RX\n");
    for(i = 0 ; i < NUM_STAT_ENTRIES ; i++) {
	if(i == Q931_ALERTING)
	    buginf("\n\nLayer 3 Frames                   TX             RX\n");
	buginf("\n%-20s%-15d%-15d", frames[i], statistics[i][TX],
	       statistics[i][RX]);
    }
}

/*
 *  Display the frame, for debugging purposes.  Do full decoding.
 */
static void
isdn_display (paktype *p, enum direction dir, int dsl)
{
    int i;			/* needed for debugging */
    uchar *frame = p->datagramstart;
    int length = p->datagramsize;

    i = 0;			/*for debugs*/
    if(detail_debug) {
	buginf("\nframe = ");
	for(i=0; i<8; i++) {
	    buginf("%x ", frame[i]); 
	}
    }	
    /* Verify the Layer 2 */
    if((frame[0] & 1) || !(frame[1] & 1) || length < 3) {
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"\nISDN %s: %s  ", DSL_TO_NAME(dsl),
		dir == TX ? "TX ->" : "RX <-");
	badframe(frame, length);
	isdn_buginf_flush();
	return;
    }

#define is_usetup(frame)	\
    ((frame[0] & 0xfc) == 0 &&  \
     (frame[2] & 0xef) == 3 && frame[5 + frame[4]] == 5)

    if(!!(frame[0] & 2) == (dir == RX)) {
	/* a command */
	if(frame[2] & 1) {
	    /* not an info frame */
	    if(!isdn_debug_ok(q921_debug, dsl)) {
		/* check for a USETUP */
		if(is_usetup(frame)) {
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "\nISDN %s: %s  ", DSL_TO_NAME(dsl),
			    dir == TX ? "TX ->" : "RX <-");
		    decode_info(frame, 3, length, dsl);
		    isdn_buginf_flush();
		}
		return;
	}

            sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                "\nISDN %s: %s  ", DSL_TO_NAME(dsl), 
		 dir == TX ? "TX ->" : "RX <-");

	    switch(frame[2]) {
	    case 1:		/* a RRc */
		if(length != 4) {
		    badframe(frame, length);
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"RR%c sapi = %d  tei = %d nr = %d ",
			 frame[3] & 1 ? 'p' : 'c',
			frame[0] >> 2, frame[1] >> 1, frame[3] >> 1);
		isdn_buginf_flush();
		return;
				
	    case 5:		/* a RNRc */
		if(length != 4) {
		    badframe(frame, length);
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"RNR%c sapi = %d  tei = %d  nr = %d",
			frame[3] & 1 ? 'p' : 'c',
			frame[0] >> 2, frame[1] >> 1, frame[3] >> 1);
		isdn_buginf_flush();
		return;
				
	    case 9:		/* a REJc */
		if(length != 4) {
		    badframe(frame, length);
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"REJ%  sapi = %d  tei = %d  nr = %d",
			frame[3] & 1 ? 'p' : 'c',
			frame[0] >> 2, frame[1] >> 1, frame[3] >> 1);
		isdn_buginf_flush();
		return;
				
	    case 0x6f:
	    case 0x7f:
		if(length != 3) {
		    badframe(frame, length);
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"SABME%s sapi = %d  tei = %d",
			frame[2] & 0x10 ? "p" : "",
			frame[0] >> 2, frame[1] >> 1);
		isdn_buginf_flush();
		return;
				
	    case 0x03:
	    case 0x13:
		if(length == 8 && (frame[0] >> 2) == 63 &&
		   (frame[1] >> 1) == 127) {
		    ushort ai, ri;
		    static char *const frms[7] = {
			"IDREQ", "IDASSN", "IDDENY", "IDCKRQ", 
			"IDCKRP", "IDREM", "IDVER"
			};

		    /* TEI assignment */
		    ai = frame[7] >> 1;
		    ri = (frame[4] << 8) | frame[5];

		    if(frame[6] < 1 || frame[6] > 7 || frame[3] != 0x0f ||
		       (frame[6] == 0 && ai != 127) ||
		       !(frame[7] & 1)) {
			badframe(frame, length);
			isdn_buginf_flush();
			return;
		    }
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "%s  ri = %d  ai = %d ", frms[frame[6]-1], ri, ai);
		    isdn_buginf_flush();
		} else {
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "UI%s sapi = %d  tei = %d",
			    frame[2] & 0x10 ? "p" : "", frame[0] >> 2,
			    frame[1] >> 1);
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    " i = ");
		    show_data(&frame[3], length - 3,
			      FALSE, &frame[length - 1]);
		    if(isdn_debug_ok(q931_debug, dsl) && is_usetup(frame)) {
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"\n    ");
			decode_info(frame, 3, length, dsl);
		    }
		    isdn_buginf_flush();
		}
		return;

	    case 0x43:
	    case 0x53:
		if(length != 3) {
		    badframe(frame, length);
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"DISC%s sapi = %d  tei = %d",
			frame[2] & 0x10 ? "p" : "", frame[0] >> 2,
			frame[1] >> 1);
		isdn_buginf_flush();
		return;
				
	    case 0xaf:
	    case 0xbf:
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"XID%s sapi = %d  tei = %d  i = ",
			 frame[2] & 0x10 ? "p" : "c", 
			 frame[0] >> 2, frame[0] >> 1);
		show_data(&frame[3], length - 3, FALSE, &frame[length - 1]);
		isdn_buginf_flush();
		return;
		/*
		 * These are added for when acting as a Network side of
		 * the connection. Now the decodes will work properly. 
		 */
	    case 0x63:
	    case 0x73:
		if(Network_side) {
		    if(length != 3) {
			badframe(frame, length);
			isdn_buginf_flush();
			return;
		    }
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "UA%s sapi = %d  tei = %d",
			    frame[2] & 0x10 ? "f" : "",
			    frame[0] >> 2, frame[1] >> 1);
		} else
		    badframe(frame, length);
		isdn_buginf_flush();
		return;
		/*
		 * These were added for when acting as a Network side of
		 * the connection. The decodes will work properly. 
		 */
	    case 0x87:
	    case 0x97:
		if(Network_side) {
		    if(length != 8) {
			badframe(frame, length);
			isdn_buginf_flush();
			return;
		    }
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "FRMR%c sapi = %d  tei = %d  i = ",
			    frame[2] & 0x10 ? "f" : "",
			    frame[0] >> 2, frame[1] >> 1);
		    show_data(&frame[3], length - 3,
			      FALSE, &frame[length - 1]);
		} else
		    badframe(frame, length);
		isdn_buginf_flush();
		return;

	    default:
		badframe(frame, length);
		isdn_buginf_flush();
		return;
	    }
	} else {
	    /* an info frame */
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "\nISDN %s: %s  ", DSL_TO_NAME(dsl),
		    dir == TX ? "TX ->" : "RX <-");

	    if(length < 4) {	/* note that null info frames */
		badframe(frame, length); /* may be used by the switch */
		isdn_buginf_flush();
		return;		/* as heartbeats */
	    }
	    if(isdn_debug_ok(q921_debug, dsl)) {
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"INFO%c sapi = %d  tei = %d  ns = %d  nr = %d", 
			frame[3] & 1 ? 'p' : 'c',
			frame[0] >> 2, frame[1] >> 1,
			frame[2] >> 1, frame[3] >> 1);
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "  i = ");
		show_data(&frame[4], length - 4, FALSE, &frame[length - 1]);
		if (!q931_debug) {
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "\n    ");
	    }

	    decode_info(frame, 4, length, dsl);	/* decode the layer 3 */
	    isdn_buginf_flush();
	}
    } else {
	/* a response */

        if(!isdn_debug_ok(q921_debug, dsl)) {
            /*
             * See if we have an I frame to decode when acting
             * as a network side of the connection.
             */
                if ((Network_side) && (frame[2] & 0x01) == 0) {
                    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                        "\nISDN %s: %s  ", DSL_TO_NAME(dsl),
			dir == TX ? "TX ->" : "RX <-");
                    decode_info(frame, 4, length, dsl);
                    isdn_buginf_flush();
                }
            return;
         }

	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"\nISDN %s: %s  ", DSL_TO_NAME(dsl), 
		dir == TX ? "TX ->" : "RX <-");
	
	switch(frame[2]) {
	case 1:			/* a RRr */
	    if(length != 4) {
		badframe(frame, length);
		isdn_buginf_flush();
		return;
	    }
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "RR%c sapi = %d  tei = %d  nr = %d",
		    frame[3] & 1 ? 'f' : 'r', 
		    frame[0] >> 2, frame[1] >> 1, frame[3] >> 1);
	    isdn_buginf_flush();
	    return;
			
	case 5:			/* a RNRc */
	    if(length != 4) {
		badframe(frame, length);
		isdn_buginf_flush();
		return;
	    }
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "RNR%c sapi = %d  tei = %d  nr = %d",
		    frame[3] & 1 ? 'f' : 'r', 
		    frame[0] >> 2, frame[1] >> 1, frame[3] >> 1);
	    isdn_buginf_flush();
	    return;
			
	case 9:			/* a REJc */
	    if(length != 4) {
		badframe(frame, length);
		isdn_buginf_flush();
		return;
	    }
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "REJ%c sapi = %d  tei = %d  nr = %d",
		    frame[3] & 1 ? 'f' : 'r',
		    frame[0] >> 2, frame[1] >> 1,frame[3] >> 1);
	    isdn_buginf_flush();
	    return;
			
	case 0x0f:
	case 0x1f:
	    if(length != 3) {
		badframe(frame, length);
		isdn_buginf_flush();
		return;
	    }
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "DM%s  sapi = %d  tei = %d",
		    frame[2] & 0x10 ? "f" : "", 
		    frame[0] >> 2, frame[1] >> 1);
	    isdn_buginf_flush();
	    return;
			
	case 0x63:
	case 0x73:
	    if(length != 3) {
		badframe(frame, length);
		isdn_buginf_flush();
		return;
	    }
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "UA%s sapi = %d  tei = %d",
		    frame[2] & 0x10 ? "f" : "",
		    frame[0] >> 2, frame[1] >> 1);
	    isdn_buginf_flush();
	    return;

	case 0x87:
	case 0x97:
	    if(length != 8) {
		badframe(frame, length);
		isdn_buginf_flush();
		return;
	    }
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "FRMR%c  sapi = %d  tei = %d  i = ",
		    frame[2] & 0x10 ? "f" : "", 
		    frame[0] >> 2, frame[1] >> 1);
	    show_data(&frame[3], length - 3, FALSE, &frame[length - 1]);
	    isdn_buginf_flush();
	    return;

	case 0xaf:
	case 0xbf:
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "XID%s  sapi = %d  tei = %d  i = ",
		    frame[2] & 0x10 ? "f" : "r",
		    frame[0] >> 2, frame[0] >> 1);
	    show_data(&frame[3], length - 3, FALSE, &frame[length - 1]);
	    isdn_buginf_flush();
	    return;

	    /*
	     * These are added for when acting as a Network side of
	     * the connection. Now the decodes work properly. 
	     */
	case 0x6f:
	case 0x7f:
	    if(Network_side) {
		if(length != 3) {
		    badframe(frame, length);
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"SABME%s  sapi = %d  tei = %d",
			frame[2] & 0x10 ? "p" : "", frame[0] >> 2,
			frame[1] >> 1);
	    } else
		badframe(frame, length);
	    isdn_buginf_flush();
	    return;
	    /*
	     * These are added for when acting as a Network side of
	     * the connection. Now the decodes work properly. 
	     */
	case 0x43:
	case 0x53:
	    if(Network_side) {
		if(length != 3) {
		    badframe(frame, length);
		    isdn_buginf_flush();
		    return;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"DISC%s sapi = %d  tei = %d",
			frame[2] & 0x10 ? "p" : "", 
			frame[0] >> 2, frame[1] >> 1);
	    } else
		badframe(frame, length);
	    isdn_buginf_flush();
	    return;
	    /*
	     * These are added for when acting as a Network side of
	     * the connection.
	     */
	case 0x03:
	case 0x13:
	    if(Network_side) {
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"UI%s "," sapi = %d  tei = %d",
			frame[2] & 0x10 ? "p" : "", 
			dsl, frame[0] >> 2, frame[1] >> 1);
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), " i = ");
		show_data(&frame[3], length - 3, FALSE, &frame[length -1]);
		if(isdn_debug_ok(q931_debug, dsl) && is_usetup(frame)) {
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "\n    ");
		    decode_info(frame, 3, length, dsl);
		}
	    } else
		badframe(frame, length);
	    isdn_buginf_flush();
	    return;
			
	default:
	    /*
	     * Added to decode when acting as the network side
	     */
	    if(Network_side) {
		/* an info frame */
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"\nISDN %s: %s  ", DSL_TO_NAME(dsl),
			dir == TX ? "TX ->" : "RX <-");

		if(length < 4) { /* note that null info frames */
		    badframe(frame, length); /* may be used by the switch */
		    isdn_buginf_flush();
		    return;	/* as heartbeats */
		}
		if(isdn_debug_ok(q921_debug, dsl)) {
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "INFO%c "
			    "sapi = %d  "
			    "tei = %d  ns = %d  nr = %d",
			    frame[3] & 1 ? 'p' : 'c',  frame[0] >> 2,
			    frame[1] >> 1, frame[2] >> 1, frame[3] >> 1);
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "  i = ");
		    show_data(&frame[4], length - 4,
			      FALSE, &frame[length - 1]);
		    if (!q931_debug) {
			isdn_buginf_flush();
			return;
		    }
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "\n    ");
		}
		decode_info(frame, 4, length, dsl); /* decode the layer 3 */
	    } else {
		badframe(frame, length);
	    }
	    isdn_buginf_flush();
	    return;
	}
    }
}

static void
decode_info (uchar *frame, int headlen, int length, int dsl)
{
    if(frame[headlen] != 0x08) {
	/*
	 * The AT&T Service messages use a protocol discrim of 3, NI-1 will use
	 * a pd of 43, Allow this for 4ESS, 5ESS, and DMS-100.
	 */
	if ((frame[headlen] == 0x03) && is_pri) {
	    switch(frame[headlen + 2 + frame[headlen + 1]]) {
	    case 0x07:
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"SERVICE ACKNOWLEDGE ");
		break;
	    case 0x0f:
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "SERVICE ");
		break;
	    default:
		badpacket(frame, length);
		break;
	    }
	    goto showdata;
	}
	if (SwitchType != BRI_1TR6_STYPE) {
	    badpacket(frame, length);
	    return;
	}
    }

    if(headlen + 1 >= length || headlen + 2 + frame[headlen + 1] >= length) {
	badpacket(frame, length);
	return;
    }

    isdn_buginf_flush();

    switch(frame[headlen + 2 + frame[headlen + 1]]) {
    case 0x00:
	if(SwitchType == BRI_5ESS_STYPE) {
	    if((frame[headlen + 2 + frame[headlen + 1] + 1]) == 0xf7)
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"MANAGEMENT_INFO ");
	    else
		badpacket(frame, length);
	}
	else
	    badpacket(frame, length);
	break;
    case 0x01:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "ALERTING ");
	break;
    case 0x02:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "CALL_PROC ");
	break;
    case 0x07:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "CONNECT ");
	break;
    case 0x0f:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "CONNECT_ACK ");
	break;
    case 0x03:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "PROGRESS ");
	break;
    case 0x05:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "SETUP ");
	break;
    case 0x0d:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "SETUP_ACK ");
	break;

    case 0x26:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "RESUME ");
	break;
    case 0x2e:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "RESUME_ACK ");
	break;
    case 0x22:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "RESUME_REJ ");
	break;
    case 0x25:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "SUSPEND ");
	break;
    case 0x2b:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "SUSPEND_ACK ");
	break;
    case 0x21:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "SUSPEND_REJ ");
	break;
    case 0x20:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "USER_INFO ");
	break;

    case 0x45:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "DISCONNECT ");
	break;
    case 0x4d:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "RELEASE ");
	break;
    case 0x5a:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "RELEASE_COMP ");
	break;
    case 0x46:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "RESTART ");
	break;
    case 0x4e:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "RESTART_ACK ");
	break;

    case 0x60:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "SEGMENT ");
	break;
    case 0x64:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"REGISTER/1TR6 STATUS ACK ");
	break;
    case 0x67:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "1TR6 INFO ACK ");
	break;
    case 0x79:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "CONG_CONT ");
	break;
    case 0x7b:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "INFORMATION ");
	break;
    case 0x62:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "FACILITY ");
	break;
    case 0x6e:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "NOTIFY ");
	break;
    case 0x7d:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "STATUS ");
	break;
    case 0x75:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "STATUS_ENQ ");
	break;
    case 0x77:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "1TR6 CLOSE_ACK ");
	break;

    case 0x6c:
	if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "REGISTER_ACC ");
	else
	    badpacket(frame, length);
	break;

    case 0x74:
	if(SwitchType == VN2_STYPE || SwitchType == VN3_STYPE)
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "REGISTER_REF ");
	else
	    badpacket(frame, length);
	break;

    case 0xf7: 
	if(SwitchType == BRI_5ESS_STYPE)
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "MANAGEMENT_INFO ");
	else
	    badpacket(frame, length);
	break;

    default:
	badpacket(frame, length);
	break;
    }
 showdata:
    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
	    "pd = %d  callref = ",  frame[headlen]);
    show_data(&frame[headlen + 2], frame[headlen + 1],
	      FALSE, &frame[length-1]);

    /* now show the information elements */
    decode_ie(frame, headlen + 3 + frame[headlen + 1], length);
}

static void
decode_ie (uchar *frame, int start, int length)
{
    uchar codeset, cur_codeset, last_ie;
    BYTE doascii;
    boolean len_err;
    boolean docause, doprogress; /* flags for extended information */
    boolean dosignal;
    int i;

    len_err = FALSE;
    last_ie = 0;
    codeset = cur_codeset = 0;
    for(i = start ; i < length ; ) {
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "\n        ");
	if(frame[i] & 0x80) {
	    /* single octet information */
	    if((frame[i] & 0x70) == 0x10) {
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"%sShift to Codeset %d",
			frame[i] & 0x08 ? "" : "Locking ",
			frame[i] & 0x07);
		if(!(frame[i] & 0x08)) {
		    if((frame[i] & 0x07) <= codeset && 
		       SwitchType != VN2_STYPE && SwitchType != VN3_STYPE)
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"   Decode Error: shift from codeset %d to %d",
				codeset, frame[i] & 0x07);
		    cur_codeset = codeset = frame[i] & 0x07;
		} else
		    cur_codeset = frame[i] & 0x07;
	    } else {
		if(cur_codeset == 0) {
		    if(frame[i] == 0xa0)
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"More Data");
		    else if(frame[i] == 0xa1)
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"Sending Complete");
		    else if((frame[i] & 0x70) == 0x00)
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"RESERVED = %d (0x%02x)", frame[i] & 0xf,
				frame[i]);
		    else if((frame[i] & 0x70) == 0x30) {
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"Congestion Level = 0x%x", frame[i]&0xf);
			if((frame[i] & 0xf) == 0)
			    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				    " (receiver ready)");
			else if((frame[i] & 0xf) == 0xf)
			    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				    " (receiver not ready)");
			else
			    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				    " (Error: reserved value %02x)",
				    frame[i]);
		    } else if((frame[i] & 0x70) == 0x30) {
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"Repeat Indicator = 0x%x", frame[i]&0xf);
			if((frame[i] & 0xf) == 0x2)
			    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				    " (Prioritized list for selecting "
				    "one possibility)");
			else
			    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				    " (Error: reserved value)");
		    } else
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"IE 0x%x", frame[i]);
		} else
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Codeset %d IE 0x%x", frame[i] & 0x07, frame[i]);
		cur_codeset = codeset;
	    }
	    i++;
	} else {
	    /* variable length information */
	    if(cur_codeset != 0) {
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"Codeset %d IE 0x%x  i = ", cur_codeset, frame[i]);
		/*
		 * AT&T testing sent a bad CodeSet 6 IE that messed us up,
		 * verify the length of the remainer. 
		 */
		if(frame[i+1] >= (length-i)) {
		    show_data(&frame[i+2], length-2-i, TRUE, &frame[length]);
		    len_err = TRUE;
		} else
		    show_data(&frame[i+2], frame[i+1], TRUE, &frame[length]);
	    } else {
		if(frame[i] < last_ie) {
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "IE out of order or end of 'private' IEs --");
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "\n        ");
		}
		last_ie = frame[i];
		doascii = FALSE;
		docause = FALSE;
		doprogress = FALSE;
		dosignal = FALSE;
		isdn_buginf_flush();
		switch(frame[i]) {
		case 0x00:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Segmented Message");
		    break;
		case 0x01:
		    if(SwitchType == BRI_1TR6_STYPE) {
			sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				"Service Indicator");
		    } else {
			if((SwitchType == PRI_4ESS_STYPE) ||
			   (SwitchType == PRI_5ESS_STYPE) ||
			   (SwitchType == PRI_DMS100_STYPE))
			    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				    "Change Status");
			else
			    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
				    "UNRECOGNIZED VARIABLE (0x%x%x) ",
				    frame[i] >> 4, frame[i] & 0xf);
		    }
		    break;
		case 0x03:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "1TR6 Time/Date");
		    break;
		case 0x04:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Bearer Capability");
		    break;
		case 0x07:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Status des geruf Tln");
		    break;
		case 0x08:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Cause");
		    docause = TRUE;
		    break;
		case 0x10:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Call Identity");
		    break;
		case 0x14:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Call State");
		    break;
		case 0x18:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Channel ID");
		    break;
		case 0x1c:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Facility");
		    break;
		case 0x1e:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Progress Ind");
		    doprogress = TRUE;
		    break;
		case 0x20:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Net Specific Fac");
		    break;
		case 0x27:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Notification Ind");
		    break;
		case 0x28:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Display");
		    doascii = TRUE;
		    break;
		case 0x29:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Date/Time");
		    break;
		case 0x2c:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Keypad Facility");
                    doascii = TRUE;
		    break;
		case 0x32:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Info Request");
		    break;
		case 0x34:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Signal");
		    dosignal = TRUE;
		    break;
		case 0x36:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Switchhook");
		    break;
		case 0x38:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Feature Activate");
		    break;
		case 0x39:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Feature Indicate");
		    break;
		case 0x3a:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "SPID Information");
                    doascii = TRUE;
		    break;
		case 0x3b:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "ENDPOINT IDent");
		    break;
		case 0x40:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Information Rate");
		    break;
		case 0x42:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "End to End Delay");
		    break;
		case 0x43:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Delay Sel & Ind");
		    break;
		case 0x44:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Packet Layer Binary Params");
		    break;
		case 0x45:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Packet Layer Window Size");
		    break;
		case 0x46:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Packet Size");
		    break;
		case 0x4c:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Connected Number");
		    break;
		case 0x6c:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Calling Party Number");
		    doascii = TRUE;
		    break;
		case 0x6d:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Calling Party SubAddr");
		    doascii = TRUE;
		    break;
		case 0x70:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Called Party Number");
		    doascii = TRUE;
		    break;
		case 0x71:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Called Party SubAddr");
		    doascii = TRUE;
		    break;
                case 0x73:
                    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                            "Original Called Number");
                    doascii = TRUE;
                    break;
		case 0x74:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Redirecting Number");
		    doascii = TRUE;
		    break;
		case 0x78:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Transit Net Select");
		    doascii = TRUE;
		    break;
		case 0x79:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Restart Indicator");
		    break;
		case 0x7c:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Low Layer Compat");
		    break;
		case 0x7d:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "High Layer Compat");
		    break;
		case 0x7e:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "User-User");
		    doascii = TRUE;
		    break;
		case 0x0c:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "Connected Address");
		    doascii = TRUE;
		    break;
		default:
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			    "UNRECOGNIZED VARIABLE (0x%x%x) ",
			    frame[i] >> 4, frame[i] & 0xf);
		    break;
		}
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), " i = ");
		/*
		 * Australia testing sent a nasty ie that would cause the
		 * code to spin in a loop forever. Now check the length.
		 */
		if(frame[i+1] >= (length-i)) {
		    show_data(&frame[i+2], length-2-i,
			      doascii, &frame[length]);
		    len_err = TRUE;
		} else
		    show_data(&frame[i+2], frame[i+1],
			      doascii, &frame[length]);

		/*
		 * show the Cause data in text form
		 */
		if (docause) {
	            sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		            " - ");
		    /* check octet 3 extend bit */
		    if((frame[i+2] & 0x80) == 0x80)
			show_cause(&frame[i+3]); /* no octet 3 extend bit */
		    else
			if((frame[i+3]) == 0x00)
			    /* octet 3 extend, rec Q931*/
			    show_cause(&frame[i+4]);
		    docause = FALSE;
		}
		/*
		 * show the Progress info in text form
		 */
		if (doprogress) {
		    show_progress(&frame[i+3]); /* no octet 3 extend bit */
		    doprogress = FALSE;
		}
		if (dosignal) {
		    show_signal(&frame[i+2]); /* octet 3 */
		    dosignal = FALSE;
		}
	    }
	    /*
	     * Found a length error, enables us to send the error
	     * message and get out.
	     */
	    if(len_err) {
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
			"\n        Decode Error: IE Length Error (0x%x) ",
			frame[i+1]);
		i = length;	/* ie len error, ignore the rest */
	    }
	    else
		i += 2 + frame[i + 1];
	    cur_codeset = codeset;
	}
	isdn_buginf_flush();
    }
}

/* display a bad frame */
static void
badframe (uchar *frame, int length)
{
    isdn_buginf_flush();
    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "BAD FRAME(");
    show_data(frame, length, FALSE, &frame[length - 1]);
    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), ")");
    isdn_buginf_flush();
}

/* display a bad packet */
static void
badpacket (uchar *frame, int length)
{
    isdn_buginf_flush();

    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "BAD PACKET(");
    show_data(frame, length, FALSE, &frame[length - 1]);
    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), ")");
    isdn_buginf_flush();
}

/* display some portion of a frame */
static void
show_data (uchar *frame, int len, boolean ascii, uchar *lastbyte)
{
    boolean instring = FALSE;
    boolean first = TRUE;
    int savelen = len;

    if(!len) {
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "(null)");	
	return;
    }

    isdn_buginf_flush();

    /*
     * CSCdi30369
     * This piece of code protects against receiving a garbage message
     * of greater than 512 bytes. The debug array is only 512 bytes
     * long so we should return an error if this length is exceeded.
     */
    if(len > sizeof(isdn_buginf_msg)) {
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Decode Error: len %d overflows frame", savelen);
        isdn_buginf_flush();
        return;
    }

    while (len-- && frame <= lastbyte) {
#define isprintc(c)	((c) >= ' ' && (c) <= '~')
	if(isprintc(*frame) && ascii) {
	    if(!instring) {
		instring = TRUE;
		if(first)
		    first = FALSE;
		else
		    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), ", ");
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "'");
	    }
	    if(*frame == '\'')
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "\\'");
	    else if(*frame == '\\')
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "\\\\");
	    else
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "%c",
			*frame);	
	} else {
	    if(instring) {
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "', 0x");
		instring = FALSE;
	    } else if(first) {
		sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "0x");
		first = FALSE;
	    }
	    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		    "%x%x", (*frame >> 4) & 0xf, *frame & 0xf);
	}
	frame++;
    }
    if(instring)
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), "'");	

    if(len >= 0)
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Decode Error: len %d overflows frame", savelen);

    isdn_buginf_flush();
}

/* display the real meaning of Cause value */
static void
show_cause (uchar *frame)
{
    switch (*frame) {
    case 0x81:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Unallocated/unassigned number ");
	break;
    case 0x82:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"No route to specified network ");
	break;
    case 0x83:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"No route to destination ");
	break;
    case 0x84:
        sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                "Vacant code or prefix digit not dialed ");
        break;
    case 0x86:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Channel unacceptable ");
	break;
    case 0x87:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Call awarded and delivered in established channel ");
	break;
    case 0x88:
        sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                "Call is proceeding or prefix 0 dialed in error ");
        break;
    case 0x89:
        sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                "Prefix 1 dialed in error ");
        break;
    case 0x8a:
        sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                "Prefix 1 not dialed ");
        break;
    case 0x8b:
        sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                "Excessive digits received, call is proceeding ");
    case 0x8d:
        sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
                "Service denied ");
    case 0x90:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Normal call clearing ");
	break;
    case 0x91:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"User busy ");
	break;
    case 0x92:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"No user responding ");
	break;
    case 0x93:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"No answer from user ");
	break;
    case 0x95:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Call rejected ");
	break;
    case 0x96:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Number changed ");
	break;
    case 0x9a:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Non-selected user clearing ");
	break;
    case 0x9b:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Destination out of order ");
	break;
    case 0x9c:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Invalid number format or Special Intercept ");
	break;
    case 0x9d:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Facility rejected or Special intercept, code undefined ");
	break;
    case 0x9e:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Response to STATUS ENQUIRY or number unassigned ");
	break;
    case 0x9f:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Normal, unspecified or Special intercept, call blocked "
                "group restriction ");
	break;
    case 0xa2:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"No channel available ");
	break;
    case 0xa6:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Network out of order ");
	break;
    case 0xa9:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Temporary failure ");
	break;
    case 0xaa:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Switching equipment congestion ");
	break;
    case 0xab:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Access information discarded ");
	break;
    case 0xac:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Requested channel not available ");
	break;
    case 0xaf:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Resources unavailable, unspecified ");
	break;
    case 0xb1:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Quality of service unavailable ");
	break;
    case 0xb2:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Requested facility not subscribed ");
	break;
    case 0xb3:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Bearer capability incompatible with service request ");
	break;
    case 0xb5:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Service operation violated ");
	break;
    case 0xb9:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Bearer capability not authorized ");
	break;
    case 0xba:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Bearer capability not presently available ");
	break;
    case 0xbf:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Service/option not available, unspecified ");
	break;
    case 0xc1:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Bearer capability not implemented ");
	break;
    case 0xc2:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Channel type not implemented ");
	break;
    case 0xc5:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Requested facility not implemented ");
	break;
    case 0xc6:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Only restricted digital info bearer capability available ");
	break;
    case 0xcf:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Service/option not implemented, unspecified ");
	break;
    case 0xd1:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Invalid call reference value ");
	break;
    case 0xd2:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Channel does not exist ");
	break;
    case 0xd3:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Suspended call exists, but call id does not ");
	break;
    case 0xd4:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Call id in use "); break;
    case 0xd5:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"No call suspended "); break;
    case 0xd6:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Call with requested call id has been cleared ");
	break;
    case 0xd8:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Incompatible destination ");
	break;
    case 0xda:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Segmentation error ");
	break;
    case 0xdb:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Invalid transit network selection or Reassembly error ");
	break;
    case 0xdf:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Invalid message, unspecified ");
	break;
    case 0xe0:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Mandatory IE missing ");
	break;
    case 0xe1:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Message type not implemented ");
	break;
    case 0xe2:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Message not compatible with call state or not implemented ");
	break;
    case 0xe3:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"IE not implemented ");
	break;
    case 0xe4:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Invalid IE contents ");
	break;
    case 0xe5:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Message not compatible with call state or protocol error, "
                "threshold exceeded ");
	break;
    case 0xe6:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Recovery on timer expiry ");
	break;
    case 0xef:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Protocol error, unspecified ");
	break;
    case 0xff:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Interworking, unspecified ");
	break;

    default:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		"Unknown Cause value");
	break;
    }
}

/* display the real meaning of Progress value */
static void
show_progress (uchar *frame)
{
    switch (*frame) {
    case 0x81:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		" - Call not end-to-end ISDN, may have in-band info ");
	break;
    case 0x82:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		" - Destination address is non-ISDN ");
	break;
    case 0x83:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		" - Origination address is non-ISDN ");
	break;
    case 0x84:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		" - Call has returned to the ISDN ");
	break;
    case 0x88:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		" - In-band info or appropriate now available ");
	break;
    case 0x8a:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		" - Delay in response at called interface ");
	break;
    default:
	sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg),
		" - Reserved value ");
	break;
    }
}

/*
 * Display the real meaning of Signal value.
 */
static void
show_signal (uchar *frame)
{
    char buffer[40];

    switch (*frame) {
    case 0x00:
	sprintf(buffer, " - Dial tone on ");
        break;
    case 0x01:
        sprintf(buffer, " - Ring back tone on ");
        break;
    case 0x02:
        sprintf(buffer, " - Intercept tone on ");
        break;
    case 0x03:
        sprintf(buffer, " - Network congestion tone on ");
        break;
    case 0x04:
        sprintf(buffer, " - Busy tone on ");
        break;
    case 0x05:
        sprintf(buffer, " - Confirm tone on ");
        break;
    case 0x06:
        sprintf(buffer, " - Answer tone on ");
        break;
    case 0x07:
        sprintf(buffer, " - Call waiting tone on ");
        break;
    case 0x08:
        sprintf(buffer, " - Off-hook warning tone on ");
        break;
    case 0x3f:
        sprintf(buffer, " - Tones off ");
        break;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x47:
        sprintf(buffer, " - Alerting on - pattern %d ", (*frame & 0x0f));
        break;
    case 0x4f:
        sprintf(buffer, " - Alerting off ");
        break;
    default:
        sprintf(buffer, " - Reserved value ");
        break;
    }
    sprintf(isdn_buginf_msg + strlen(isdn_buginf_msg), buffer);
}

/* 
 * Get the called party Number, Return NULL if not present 
 */
static char * extract_called_number (PKG_ID pkg)
{
    BYTE *info_element;
    char *ans_num;

    if ( (info_element = my_Get_IE (pkg, CALLED_NUMBER_IE) ) == NULL)
	return (NULL);
    if (info_element[1] == 0)	/* contains the length of the I.E. */
	return (NULL);
    ans_num = malloc (info_element[1] );
    if (ans_num != NULL) {
	/* The next stmt will copy info_element[1] -1  bytes. That's ok 
	   because we don't need to copy the TON/NPI byte from the IE */
	sstrncpy(ans_num, &info_element[3], (int) info_element[1]);
    }
    return ans_num;
}

/* 
 * This is used only for German 1tr6 bri semi permanent connections.
 * Get the spc Number, Return NULL if not present 
 */
static char * extract_spc_number (PKG_ID pkg)
{
	BYTE *info_element;
	char *number;

	if ( (info_element = my_Get_IE (pkg, CALLED_NUMBER_IE) ) == NULL)
		return (NULL);
	if (info_element[1] == 0) /* contains the length of the I.E. */
		return (NULL);
	number = malloc (info_element[1] + 1 );
	if (number != NULL) {
	    sstrncpy(number, &info_element[2], (int) info_element[1]+1);
	}
	return number;
}
/* 
 * Get the called party Subaddress, Return NULL if not present 
 */
static char * extract_sa (PKG_ID pkg)
{
    BYTE *info_element;
    char *sa;

    if ( (info_element = my_Get_IE (pkg, CALLED_SUB_ADDR_IE) ) == NULL)
	return (NULL);
    if (info_element[1] == 0)
	return (NULL);
    if (info_element[3] != AFI_BYTE)
	return(NULL);
    sa = malloc (info_element[1] );
    if (sa != NULL) {
	/* The next stmt will copy info_element[1] - 2 bytes. That's ok 
	   because we don't need to copy the TOS & AFI bytes from the IE */
	sstrncpy(sa, &info_element[4], (int) info_element[1]-1);
    }
    return sa;
}

/* 
 * Returns FALSE if the received #/subaddr does not match the configured
 */
static boolean verify_answer_number (PKG_ID pkg)
{
    boolean result;
    char *rcvd_number, *subaddress;
    BYTE dsl;

    result = FALSE;
    dsl = pkg->PkgInfo[1];
    /* If nothing is configured, then accept the call */
    if ( ( isdn_answer[dsl].answer1[0] == '\0') &&
	( isdn_answer[dsl].answer2[0] == '\0') ) {
	result = TRUE; 
    } else {
	/* Something is configured. Now we'll have to compare */
	rcvd_number = extract_called_number(pkg);
	subaddress = extract_sa(pkg);
	if ( isdn_answer[dsl].answer1[0] != '\0') 
	    result = compare_num(rcvd_number, subaddress,
				 isdn_answer[dsl].answer1);
	/* Comparison with the first config'ed # failed. Try the next one */
	if ( (!result) && (isdn_answer[dsl].answer2[0] != '\0') ) 
	    result = compare_num(rcvd_number, subaddress,
				 isdn_answer[dsl].answer2);
	if (rcvd_number != NULL) 
	    free(rcvd_number);
	if (subaddress != NULL) 
	    free(subaddress);
    }		
    return(result);
}

/* 
 * Returns FALSE if the called #/subaddress comparisons fail 
 */
static boolean
compare_num (char *rcvd_number, char *subaddress, char *ans_num)
{
    /* Here we first get the called # and the subaddress lengths in the
       received # and the configured #. Then we compare the called party
       #s first. Next the subaddresses are compared. */
    
    int cfg_len; /* length of the configured called # and subaddress */
    int cfg_cd_len; /* length of the configured called party # */
    int k, cfg_sa_len; /* length of the configured subaddress */
    int rcvd_cd_len; /* length of the received called party # */
    int rcvd_sa_len; /* length of the received called party subaddr */
    boolean result, cd_result, sa_result;

    result = FALSE; /* result of all comaprisons */
    cd_result = FALSE; /* result of called # comparisons only */
    sa_result = FALSE; /* result of called subaddr comparison */

    rcvd_cd_len = strlen (rcvd_number);
    cfg_len = strlen ( ans_num);
    rcvd_sa_len = strlen(subaddress);
    
    /* If nothing has been delivered, ignore the call */
    if ((!rcvd_cd_len) && (!rcvd_sa_len))
	return(FALSE);
    
    for ( k=0; k < cfg_len; k++) {
	if ( ans_num[k] == SUBADDR_SEPARATOR)
	    break;
    }
    cfg_cd_len = k;
    if ( ans_num[k] == SUBADDR_SEPARATOR)
	cfg_sa_len = cfg_len - k -1; /* 1 for the subaddr separator */
    else
	cfg_sa_len = cfg_len - k; 

    /* Compare the called party #s only if both config'ed and delivered */
    if (rcvd_cd_len &&  cfg_cd_len) 
	cd_result = match_num(rcvd_number, ans_num,
			      rcvd_cd_len, cfg_cd_len, ZERO_OFFSET);

    /* Compare the subaddresses  only if both config'ed and delivered */
    if (rcvd_sa_len && cfg_sa_len) 
	sa_result = match_num(subaddress, ans_num, rcvd_sa_len,
			      cfg_sa_len, cfg_cd_len+1);
    /* +1 is for the subaddress separator character */	
    
    /* when both (called # and subaddress) are configured , then both must 
       compare ok with the delivered #s */
    if ( cfg_cd_len && cfg_sa_len )
	result = cd_result && sa_result;
    else
	result = cd_result || sa_result;
    return(result);
}

/* 
 * returns FALSE if the strings do not match 
 */
boolean
match_num (char *src, char *dest, int r_len, int c_len, int offset)
{
    boolean result;
    int iter; /* Number of iterations to be performed */

    result = TRUE;
    if ( r_len < c_len ) /* received # is smaller */
	iter = r_len;
    else 
	iter = c_len;
    /* 
     * Now compare the numbers. Note that arrays are zero-based. 
     */
    while (iter > 0) {
	if  ((dest[c_len+offset-1] != 'x') && (dest[c_len+offset-1] != 'X')) {
	    /*
	     * It's not a don't care digit. It must match with received digit.
	     */
	    if (dest[c_len+offset-1] != src[r_len-1]) {
		result = FALSE;
		break;
	    }
	}
	c_len--; 
	r_len--; 
	iter--;
    }    
    return(result);
}

/*
 * isdn_mib_update
 * Carry out all the updates to the mib structures.
 */
void isdn_mib_update (hwidbtype *hwidb, char *called_num, boolean conn)
{
    if (hwidb && called_num && *called_num) {
       if (hwidb->isdn_info->remote_number)
           free(hwidb->isdn_info->remote_number);
       hwidb->isdn_info->remote_number = strdup(called_num);
       hwidb->isdn_info->remote_src = ISDN_SRC_UNKNOWN;
    }
    hwidb->isdn_info->call_connected = conn;
    reg_invoke_isdn_neighbor_update(hwidb);
    reg_invoke_isdn_history_update(hwidb);
}

/*
 * Extract the disconnect cause from pkg. This will be used by the mib.
 */
void isdn_mib_extract_cause (PKG_ID pkg, enum direction dir, isdn_bri_t *isdn_info)
{
  int cause, pkg_index, cause_length;

   if (pkg && isdn_info) {
    for (pkg_index =  pkg->InfoLen - 1; pkg_index > 0 ; pkg_index--) {
	if(pkg->PkgInfo[pkg_index] == CAUSE_IE) 
	  break;
    }
    if (pkg_index > 0) {
       cause_length = pkg->PkgInfo[++pkg_index];
       switch (cause_length) {
	  case 0x01:
	  case 0x02:
	  case 0x03:
	     cause = pkg->PkgInfo[pkg_index + cause_length];
	     if (cause_length == 0x01)
		cause = cause | 0x80;
	     break;
          default: 
	     cause = DISC_OTHER_CAUSE;
	     break;
       }
    } else
       cause = DISC_OTHER_CAUSE;
    isdn_mib_cause_init(cause,  dir, isdn_info);
  }

}

/*
 * Initialize the clear_reason and clear_code based on the value of the 
 * disconnect cause. Also, get the exact cause value into 
 * disconnect_cause. The latter will be used by the call history mib.
 */
void isdn_mib_cause_init (int cause, enum direction dir, isdn_bri_t *isdn_info)
{
    char cause_array[6];
    uchar cause_octet;

    *isdn_buginf_msg = 0;
    *cause_array = 0;
    sprintf(cause_array + strlen(cause_array), "0x");
    cause_octet = (uchar) cause;
    sprintf(cause_array + strlen(cause_array), "%x", cause_octet & 0x7f);
    if (isdn_info->clear_reason)
       free(isdn_info->clear_reason);
    if (isdn_info->clear_code)
       free(isdn_info->clear_code);
    show_cause( &cause_octet);
    isdn_info->clear_reason = strdup(isdn_buginf_msg);
    isdn_info->clear_code =  strdup(cause_array);
    switch ( cause ) {
        case 0x90:
	      if (dir == TX)
		isdn_info->disconnect_cause = NORMAL_DISC_SENT;
              else
		isdn_info->disconnect_cause = NORMAL_DISC_RECEIVED;
	      break;
        case 0x91:
		isdn_info->disconnect_cause = DISC_USER_BUSY;
	      break;
        case 0x95:
		isdn_info->disconnect_cause = DISC_CALL_REJECTED;
	      break;
        case 0xa6:
		isdn_info->disconnect_cause = DISC_NETWORK_OUT_OF_ORDER;
	      break;
        case 0xa2:
		isdn_info->disconnect_cause = NO_CHANNEL_AVAILABLE;
	      break;
        case 0xff:
		isdn_info->disconnect_cause = INTERWORKING_ERROR;
	      break;
        default:
		isdn_info->disconnect_cause = DISC_OTHER_CAUSE;
	      break;
    }
    *isdn_buginf_msg = 0;
}
/*
 * Find the B channel idb for mib update, based on call_id.
 * If not able to find, then return the D channel idb of the ISDN BRI/PRI.
 */
hwidbtype *isdn_mib_find_idb (UINT16 call_id, hwidbtype *idb)
{
  hwidbtype *b_idb;
  isdn_bri_t *isdn_info;
  int channel;

  b_idb = NULL;
  if (idb == NULL)
    return (NULL);
  if (!is_pri) {
    if (idb->isdn_info->b1_chan_idb->isdn_info->call_id == call_id)
      return(idb->isdn_info->b1_chan_idb);
    else if (idb->isdn_info->b2_chan_idb->isdn_info->call_id == call_id)
      return(idb->isdn_info->b2_chan_idb);
  } else {
      if ((idb != NULL) && (idb->isdn_info != NULL))
        for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
          b_idb = idb->isdn_info->b_chan_idb[channel];
          if (b_idb == NULL)
              continue;
          isdn_info = b_idb->isdn_info;
          if (isdn_info == NULL)
              continue;
          if (isdn_info->call_id == call_id) {
             idb = b_idb;
             break;
            }
          }
  }
  return(idb);
}
