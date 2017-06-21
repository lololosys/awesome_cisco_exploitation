/* $Id: lanmgr.c,v 3.17.8.19 1996/09/10 03:14:04 ioakley Exp $
 * $Source: /release/112/cvs/Xsys/ibm/lanmgr.c,v $
 *------------------------------------------------------------------
 * lanmgr.c -- Token Ring Lan Manager Support Vectors
 *
 * January 1991, David R. Hampton
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lanmgr.c,v $
 * Revision 3.17.8.19  1996/09/10  03:14:04  ioakley
 * CSCdi60079:  Correct clsi compliant LNM dlu clean-up problems
 *              that result in LNM clsi related error messages when
 *              "no source-bridge"/"source-bridge" commands are
 *              entered on token ring interfaces.
 * Branch: California_branch
 *
 * Revision 3.17.8.18  1996/08/30  18:17:54  bmonsen
 * CSCdi67726:  lnm subsys header did not include cls and llc2
 * Branch: California_branch
 * Added required subsystem to the subsys header
 *
 * Revision 3.17.8.17  1996/08/28  12:46:45  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.17.8.16  1996/08/26  15:08:14  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.17.8.15  1996/08/16  07:04:18  ioakley
 * CSCdi63385:  Prevent router crash caused by CLS Connect_Req failure
 *              and resolve LNM conn pending/reporting link issues.
 * Branch: California_branch
 *
 * Revision 3.17.8.14  1996/08/07  08:59:32  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.17.8.13  1996/08/02  20:44:34  ioakley
 * CSCdi59890:  Correct CRS regression introduced by CSCdi42298
 *              for concurrent LNM/AST port configurations -
 *              resolves LNM resync/segment-test command failures.
 * Branch: California_branch
 *
 * Revision 3.17.8.12  1996/07/25  14:13:02  fbordona
 * CSCdi64009:  %LNMC-3-BADCLSIRET: bogus Invalid ret code (0x7004)
 *              Adjust LNM for SRB sub-block changes.
 * Branch: California_branch
 *
 * Revision 3.17.8.11  1996/07/25  05:46:55  kchiu
 * CSCdi35303:  Token ring shut down with different segment number
 * Branch: California_branch
 *
 * Revision 3.17.8.10  1996/07/09  05:58:06  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.17.8.9  1996/07/02  23:10:56  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.17.8.8  1996/06/28  23:06:36  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.17.8.7  1996/06/21  21:49:32  ioakley
 * CSCdi58444:  Add check in display_station_by_idb to prevent
 *              crash in lnm watchdog code during lnm "show ring"
 *              command processing.
 * Branch: California_branch
 *
 * Revision 3.17.8.6  1996/06/17  23:30:44  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.17.8.5  1996/06/01  01:31:34  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.17.8.4  1996/05/17  11:20:26  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.16.2.7  1996/05/05  23:08:18  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.16.2.6  1996/05/03  18:08:09  ioakley
 * CSCdi54742:  Remove redundant start_timer call in lanmgr_ringstat
 *              since rem_beacon_postmortem will always return when
 *              ring->beaconing == PERMANENT.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.16.2.5  1996/04/26  15:19:47  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.16.2.4  1996/04/26  07:54:10  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.16.2.3  1996/04/03  14:33:52  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.16.2.2  1996/03/29  19:36:51  ioakley
 * CSCdi52877:  VDLC merge into IbuMod_Calif - Resolve various LNM/VDLC
 *              inconsistencies & include LNM/VDLC disable port updates.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.16.2.1  1996/03/17  17:50:21  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.17.8.3  1996/04/11  23:20:26  monical
 * CSCdi47927:  show lnm stations only returns info for one interface
 * Branch: California_branch
 *
 * Revision 3.17.8.2  1996/03/21  22:41:33  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.17.8.1  1996/03/18  20:11:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.5  1996/03/18  23:21:19  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.3.2.4  1996/03/06  19:47:51  sthormod
 * CSCdi50859:  add srb switching path for predator 4R
 * Branch: ELC_branch
 * Use GETSHORT on mac frame to avoid alignment problems.
 *
 * Revision 3.3.2.3  1996/02/08  07:24:17  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3.2.2  1996/01/15  22:54:42  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.3.2.1  1995/12/11  18:48:25  mbeesley
 * Branch: ELC_branch
 * Sync ELC_branch to ELC_baseline_120995.
 *
 * Revision 3.17  1996/02/27  19:30:00  jrosen
 * CSCdi47898:  no lnm disabled crashes router if CIP internal lan exists
 * - Make sure that set_extra_addrs member of the hardware IDB is
 *   non-NULL before calling the function it points to.
 *
 * Revision 3.16  1996/02/15  00:12:45  hwang
 * CSCdi45890:  sho lnm int truncate the interface name
 * Replace hw_namestring with hw_short_namestring.
 *
 * Revision 3.15  1996/02/09  03:59:25  ioakley
 * CSCdi42298: Don't invoke crs_input or vring_mac_adapter
 *             on the basis of transparently bridged input.
 *
 * Revision 3.14  1996/02/08  17:57:24  ioakley
 * CSCdi47561: Resolve potential network congestion problems by adding
 *             a global configuration command to disable the automatic
 *             Report Path Trace function and also correct problems with
 *             Report Path Trace frames returned to originating stations.
 *
 * Revision 3.13  1996/02/07  16:11:48  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.12  1996/02/01  06:03:02  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.11  1996/01/30  17:14:29  monical
 * CSCdi39483:  Crash in _crashdump after sh lnm station command entered
 *
 * Revision 3.10  1996/01/30  01:20:20  asethi
 * CSCdi46995:  7513 gives allignment error when pinging XNS, IPX packets
 *
 * The allignment error was occuring because we were doing a word read on
 * this field,
 * and since this was on a odd byte in the machine, an allignment error
 * occured.
 *
 * This has been rectified by changing the access to the structure by a
 * byte pointer.
 *
 * Revision 3.9  1996/01/29  07:27:57  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.8  1996/01/19  20:08:26  psandova
 * CSCdi46764:  Spurious memory acces on 4500 on configr no source-bridge
 * on tokring
 *
 * Check for valid output interface in packet and cleanup if nessessary.
 * Also, remove bogus attempt to get virtual-ring-group using null idb.
 *
 * Revision 3.7  1996/01/18  22:51:42  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/18  03:23:01  kchiu
 * CSCdi43616:  Router needs to set express buffer bit on RPS response
 *
 * Revision 3.5  1995/12/18  21:36:35  ioakley
 * CSCdi45683:  Correct %DLC-3-WPUTERR during startup.
 *
 * Revision 3.4  1995/12/07  02:25:14  pleung
 * CSCdi34018:  LNM: password 1 command cannot be removed
 *
 * Revision 3.3  1995/11/17  09:22:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:35:33  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:45:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  21:01:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/07/13  20:33:05  jacobt
 * CSCdi36703:  replace malloc() by malloc_named()
 *
 * Revision 2.4  1995/06/28  09:25:30  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/18  23:36:40  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/15  22:00:48  hampton
 * Fix 24.5 day bugs in Lan Manager support.  While there, eliminate
 * direct references to msclock.  [CSCdi35914]
 *
 * Revision 2.1  1995/06/07 20:47:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_generic.h"
#include "packet.h"
#include "logger.h"
#include "../ibm/msg_lanmgr.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "../if/network.h"
#include "../if/tring.h"
#include "../if/tring_common.h"
#include "../if/rif_util.h"
#include "../cls/clsi_msg.h"
#include "../cls/dlc_public.h"
#include "../cls/dlc.h"
#include "lanmgr.h"
#include "ibmnm.h"
#include "../if/network.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_lanmgr.h"
#include "../llc2/llc2.h"
#include "../srt/rsrb.h"
#include "../srt/srb_core.h"
#include "../srt/span.h"
#include "../srt/srt_registry.h"
#include "../cls/dlc_registry.h"
#include "../ibm/lnm_dlu.h"
#include "../partner/partner_registry.h"
#include "../h/fastswitch_registry.regh"
#include "../util/random.h"
#include "ttysrv.h"
#include "subsys.h"
#include "../ibm/lanmgr_registry.regh"

/*
 * Forwards
 */
static void lnm_send_activate_sap(TCLSIMsg *clsi_msg, boolean port_flag);

/*
 * Global Storage
 */
int lanmgr_pid;
boolean lanmgr_global_enable;
boolean lanmgr_pathtrace_enable_lnm;
boolean lanmgr_pathtrace_enable_orig;
queuetype lanmgrQ;
queuetype ringQ;
queuetype lnm_clsiQ;
queuetype lnm_ctlQ;
station_info *station_table[NETHASHLEN];
ulong crs_correlator;
sys_timestamp lanmgr_timer;	/* One Second background timer */
sys_timestamp rem_timer;	/* Five Second background timer */
ulong rps_correlator;
static boolean lnm_keepalive_flag;
static lnm_t   *lnm_hash_table[NLNM_HASH_SIZE];

/*
 * Local Storage
 */
/*
 *   The address below is used for the special LRM_MGMT_SERVERS_PRESENT
 *   command that is intended to simulate the look and feel of an
 *   IBM 8209 Bridge.
 */
static const uchar lnm_local_admin_addr[IEEEBYTES] = 
                                        {0x40, 0x00, 0x00, 0x00, 0x20, 0x00};

/*
 * This table helps to identify good clsi ops
 * from bad ones.
 * Comment an entry here before adding it to the
 * CLSI processing logic to keep them in sync.
 * sync with clsi_msg.h to catch the latest changes..
 */

static const ushort lnm_bad_clsi_op_tbl[] = 
{
      /*
       * all the indications
       */
	CLS_ACTIVATE_SAP | 		CLS_INDICATION,
	CLS_CLOSE_STN | 		CLS_INDICATION,	
/*	CLS_CONNECT | 			CLS_INDICATION,	*/
/*	CLS_CONNECTED | 		CLS_INDICATION,	*/
/*	CLS_DATA | 			CLS_INDICATION,	*/
	CLS_DEACTIVATE_SAP | 		CLS_INDICATION,	
/*	CLS_DISCONNECT | 		CLS_INDICATION,	*/
	CLS_ENABLE | 			CLS_INDICATION,		
/*	CLS_FLOW | 			CLS_INDICATION,	*/
/*	CLS_ID | 			CLS_INDICATION,	*/
/*	CLS_ID_STN | 			CLS_INDICATION,	*/
	CLS_OPEN_STN | 			CLS_INDICATION,	
	CLS_REQ_OPNSTN | 		CLS_INDICATION,	
	CLS_SIGNAL_STN | 		CLS_INDICATION,	
	CLS_DISABLE | 			CLS_INDICATION,	
/*	CLS_UDATA | 			CLS_INDICATION, */
	CLS_MODIFY_SAP | 		CLS_INDICATION,     
/*	CLS_TEST | 			CLS_INDICATION, */
	CLS_ACTIVATE_RING | 		CLS_INDICATION,  
	CLS_DEACTIVATE_RING | 		CLS_INDICATION,
/*	CLS_TEST_STN | 			CLS_INDICATION, */
/*	CLS_UDATA_STN | 		CLS_INDICATION, */
/*	CLS_CONNECT_STN | 		CLS_INDICATION, */
      /*
       * all the confirms
       */
/*	CLS_ACTIVATE_SAP | 		CLS_CONFIRM, */
/*	CLS_CLOSE_STN | 		CLS_CONFIRM, */
/*	CLS_CONNECT | 			CLS_CONFIRM, */
/*	CLS_CONNECTED | 		CLS_CONFIRM, */
/*	CLS_DATA | 			CLS_CONFIRM, */
/*	CLS_DEACTIVATE_SAP | 		CLS_CONFIRM, */
/*	CLS_DISCONNECT | 		CLS_CONFIRM, */
/*	CLS_ENABLE | 			CLS_CONFIRM, */
/*	CLS_FLOW | 			CLS_CONFIRM, */
/*	CLS_ID | 			CLS_CONFIRM, */
	CLS_ID_STN | 			CLS_CONFIRM, 
/*	CLS_OPEN_STN | 			CLS_CONFIRM, */
/*	CLS_REQ_OPNSTN | 		CLS_CONFIRM, */
/*	CLS_SIGNAL_STN | 		CLS_CONFIRM, */
/*	CLS_DISABLE | 			CLS_CONFIRM, */
	CLS_UDATA | 			CLS_CONFIRM,          
	CLS_MODIFY_SAP | 		CLS_CONFIRM,     
/*	CLS_TEST | 			CLS_CONFIRM, */
/*	CLS_ACTIVATE_RING | 		CLS_CONFIRM, */
/*	CLS_DEACTIVATE_RING | 		CLS_CONFIRM, */
	CLS_TEST_STN | 			CLS_CONFIRM,
	CLS_UDATA_STN | 		CLS_CONFIRM
};       

static const ushort lnm_cs_clsi_op_tbl[] =
{
	CLS_TEST_STN 		| CLS_INDICATION,
	CLS_ID_STN 		| CLS_INDICATION,
	CLS_UDATA_STN 		| CLS_INDICATION,
	CLS_CONNECT_STN 	| CLS_INDICATION,
};

static const ushort cs2lnm_clsi_op_tbl[] = 
{
#ifdef NOCSTM
	CLS_TEST_STN 		| CLS_INDICATION,
#endif
	CLS_ID_STN 		| CLS_INDICATION,
	CLS_CONNECT_STN 	| CLS_INDICATION,
};

static const ushort lnm_init_clsi_op_tbl[] =
{
        CLS_ACTIVATE_SAP        | CLS_CONFIRM,
	CLS_DEACTIVATE_SAP      | CLS_CONFIRM,
	CLS_ENABLE  		| CLS_CONFIRM, 
	CLS_DISABLE  		| CLS_CONFIRM
};


/*
 * CLSI ops for which the data length has to be valid
 * and resonable. Ignore the data length ofr other types.
 * These ops are the only ones allowed to carry the
 * end systems payload.
 */

static const ushort lnm_data_req_clsi_op_tbl[] = 
{
      /*
       * all the indications
       */
	CLS_DATA | 			CLS_INDICATION,
	CLS_ID | 			CLS_INDICATION,
/*	CLS_ID_STN | 			CLS_INDICATION,  
	NULL XIDs have zero length
*/
	CLS_UDATA | 			CLS_INDICATION,
	CLS_UDATA_STN | 		CLS_INDICATION,
      /*
       * all the confirms
       */
	CLS_ID | 			CLS_CONFIRM,
	CLS_ID_STN | 			CLS_CONFIRM,
	CLS_UDATA | 			CLS_CONFIRM,          
	CLS_UDATA_STN | 		CLS_CONFIRM
};       


static const ushort lnm_ctxt_req_clsi_op_tbl[] = 
{
	CLS_CONNECTED | 		CLS_INDICATION,	
	CLS_DATA | 			CLS_INDICATION,	
	CLS_DISCONNECT | 		CLS_INDICATION,	
	CLS_FLOW | 			CLS_INDICATION,	
	CLS_ID | 			CLS_INDICATION,	
	CLS_UDATA | 			CLS_INDICATION, 
	CLS_TEST | 			CLS_INDICATION, 
       
	CLS_CLOSE_STN | 		CLS_CONFIRM, 
	CLS_CONNECT | 			CLS_CONFIRM, 
	CLS_CONNECTED | 		CLS_CONFIRM, 
	CLS_DATA | 			CLS_CONFIRM, 
	CLS_DISCONNECT | 		CLS_CONFIRM, 
	CLS_FLOW | 			CLS_CONFIRM, 
	CLS_ID | 			CLS_CONFIRM, 
	CLS_OPEN_STN | 			CLS_CONFIRM, 
	CLS_UDATA | 			CLS_CONFIRM,          
	CLS_TEST | 			CLS_CONFIRM, 
};       


#define NLNM_BAD_CLSI_OP sizeof(lnm_bad_clsi_op_tbl)/sizeof(ushort)
#define NLNM_DATAREQ_CLSI_OP sizeof(lnm_data_req_clsi_op_tbl)/sizeof(ushort)
#define NLNM_CONTEXTREQ_CLSI_OP sizeof(lnm_ctxt_req_clsi_op_tbl)/sizeof(ushort)
#define NLNM_INIT_CLSI_OP sizeof(lnm_init_clsi_op_tbl)/sizeof(ushort)
#define NLNM_CS_CLSI_OP sizeof(lnm_cs_clsi_op_tbl)/sizeof(ushort)
#define NLNM_CS2LNM_CLSI_OP sizeof(cs2lnm_clsi_op_tbl)/sizeof(ushort)


/**********************************************************************
 *
 *		     Play with the process timers
 *
 **********************************************************************/
static inline void start_rem_timer (void)
{
    if (!TIMER_RUNNING(rem_timer))
	TIMER_START(rem_timer, REM_WORK_INTERVAL);
}

static inline void bump_rem_timer (void)
{
    TIMER_UPDATE(rem_timer, REM_WORK_INTERVAL);
}

static inline void stop_rem_timer (void)
{
    TIMER_STOP(rem_timer);
}


/**********************************************************************
 *
 *	      Inline functions to assign common values.
 *
 **********************************************************************/

static inline subvector_t *assign_mac_ring (
    subvector_t *sv,
    hwidbtype *idb)
{
    sv->length = 4;
    sv->command = SVID_LOCALRING;
    sv->data.d.sword[0] = srbV.srb_sb_get_thisring(idb->firstsw);
    return((subvector_t *)((uchar *)sv + 4));
}

static inline subvector_t *assign_mac_location (
    subvector_t *sv,
    station_info *info)
{
    ring_info *ring;

    if (info->phys_location == 0) {
	ring = idb_to_ring(info->idb);
	info->phys_location = ring->location_count++;
    }
    
    sv->length = 6;
    sv->command = SVID_ASSGN_PHYS_LOC;
    sv->data.d.lword = info->phys_location;
    return((subvector_t *)((uchar *)sv + 6));
}

static inline subvector_t *assign_mac_softerr_timer (
    subvector_t *sv,
    ring_info *ring)
{
    sv->length = 4;
    sv->command = SVID_SOFTERRTIMER;
    sv->data.d.sword[0] = ring->softerr_timer;
    return((subvector_t *)((uchar *)sv + 4));
}

static inline subvector_t *assign_mac_correlator (
    subvector_t *sv,
    ushort correlator)
{
    sv->length = 4;
    sv->command = SVID_CORRELATOR;
    sv->data.d.sword[0] = correlator;
    return((subvector_t *)((uchar *)sv + 4));
}

static inline subvector_t *assign_mac_short (
    subvector_t *sv,
    uchar command,
    ushort value)
{
    sv->length = 4;
    sv->command = command;
    sv->data.d.sword[0] = value;
    return((subvector_t *)((uchar *)sv + 4));
}

static inline subvector_t *assign_mac_long (
    subvector_t *sv,
    uchar command,
    ulong value)
{
    sv->length = 6;
    sv->command = command;
    sv->data.d.lword = value;
    return((subvector_t *)((uchar *)sv + 6));
}

static inline subvector_t *assign_mac_bytes (
    subvector_t *sv,
    uchar command,
    uchar const *address,
    int length)
{
    sv->length = length + 2;
    sv->command = command;
    bcopy(address, sv->data.d.byte, length);
    return((subvector_t *)((uchar *)sv + length + 2));
}

static inline subvector_t *assign_mac_address (
    subvector_t *sv,
    uchar command,
    uchar const *address)
{
    return(assign_mac_bytes(sv, command, address, IEEEBYTES));
}

static void finish_mvid (
    paktype *pak,
    subvector_t *sv)
{
    major_vector *major;

    major = (major_vector *)major_vector_headstart(pak);

    major->length = (uchar *)sv - (uchar *)major;
}


/**********************************************************************
 *
 *
 *
 **********************************************************************/

/*
 * idb_to_ring
 * address_to_ring
 * ringno_to_ring
 *
 * Three ways to find a ring structure in the table.
 */
hwidbtype *ring_to_idb (ring_info *ring)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	if (idb->ring_info == ring)
	    return(idb);
    }
    return(NULL);
}

ring_info *idb_to_ring (hwidbtype *idb)
{
    return(idb ? idb->ring_info : NULL);
}

hwidbtype *address_to_idb (uchar *address)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	if (ieee_equal_sans_rii(idb->hardware, address))
	    return(idb);
    }
    return(NULL);
}

#ifdef NOBODY_CALLS_ME
ring_info *address_to_ring (uchar *address)
{
    hwidbtype *idb;

    FOR_ALL_HWIDBS(idb) {
	if (ieee_equal_sans_rii(idb->hardware, address))
	    return(idb->ring_info);
    }
    return(NULL);
}
#endif /* NOBODY_CALLS_ME */

ring_info *ringno_to_ring (int ringno)
{
    ring_info *ring;

    for (ring = ringQ.qhead; ring; ring = ring->next) {
	if (ring->ringno == ringno)
	    return(ring);
    }
    return(NULL);
}

/*
 *
 */
void lanmgr_enqueue (paktype *pak)
{
    if (lanmgr_pid) {
	p_enqueue(&lanmgrQ, pak);
    } else {
	protocol_discard(pak, TRUE);
    }
}

/*
 *
 */
station_info *find_station (uchar *address, hwidbtype *idb)
{
    uchar hash;
    station_info *info, *match;
    
    /*
     * Find or create an entry for this station.
     */
    match = NULL;
    hash = address[4] ^ address[5];
    for (info = station_table[hash]; info; info = info->next)
	if (ieee_equal(&info->address[0], address)) {
	    if (idb && (info->idb == idb))
		return(info);
	    match = info;
	}
    return(match);
}

station_info *find_or_build_station (
    uchar *address,
    hwidbtype *idb,
    boolean temp)
{
    uchar hash;
    station_info *info;
    leveltype level;

    /*
     * Find or create an entry for this station.
     */
    hash = address[4] ^ address[5];
    for (info = station_table[hash]; info; info = info->next)
	if (ieee_equal(info->address, address) && (info->idb == idb))
	    break;
    if (!info) {
	info = malloc_named(sizeof(station_info), lnm_cb);
	if (!info) {
	    return(NULL);
	}
	ieee_copy(address, info->address);
	info->nexterror = NULL;
	info->error_xmt = FALSE;
	info->idb = idb;
	if (temp) {
	    TIMER_START(info->temp, IBMNM_TEMP_RETAIN);
	} else {
	    TIMER_STOP(info->temp);
	}
	level = raise_interrupt_level(NETS_DISABLE);
	info->next = station_table[hash];
	station_table[hash] = info;
	reset_interrupt_level(level);
	info->request_data = NULL;
    }
    return(info);
}

station_info *newrold_station (paktype *pak)
{
    uchar address[IEEEBYTES];
    
    find_802addr(pak, address, GET_SOURCE);
    return(find_or_build_station(address, hwidb_or_null(pak->if_input), FALSE));
}

/**********************************************************************
 *
 *        Translations from code to text string.
 *
 **********************************************************************/
static const char unknown[] = "unknown";

static char *const mvid_names[] = {
    "response", "unknown", "beacon", "claim token", "ring purge", "AMP", "SMP",
    "duplicate address test", "lobe test", "transmit forward", "unknown",
    "remove", "change parameters", "initialize station", "request address",
    "request state", "request attachments", "unknown", "unknown", "unknown",
    "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
    "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
    "request initialization", "unknown", "report address", "report state",
    "report attachments", "report new monitor", "report NAUN change",
    "report next neighbor incomp", "report active monitor error",
    "report soft error", "report transmit forward"};

static const int mvr_bits[] = {MVR_RESPONSE, 0, MVR_BEACON, MVR_CLAIM,
    MVR_RINGPURGE, MVR_AMP, MVR_SMP, MVR_DUPTEST, MVR_LOBETEST,
    MVR_TRANSMITFWD, 0, MVR_REMOVE, MVR_CHANGEPARAM, MVR_INITIALIZE,
    MVR_REQADDRESS, MVR_REQSTATE, MVR_REQATTACH, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, MVR_REQINIT, 0, MVR_RPTADDRESS,
    MVR_RPTSTATE, MVR_RPTATTACH, MVR_RPTNEWMON, MVR_RPTNAUN,
    MVR_NNINCOMP, MVR_RPTMONERR, MVR_SOFTERR, MVR_RPTTXMTFWD};

static const int mvo_bits[] = {MVO_RESPONSE, 0, MVO_BEACON, MVO_CLAIM,
    MVO_RINGPURGE, MVO_AMP, MVO_SMP, MVO_DUPTEST, MVO_LOBETEST,
    MVO_TRANSMITFWD, 0, MVO_REMOVE, MVO_CHANGEPARAM, MVO_INITIALIZE,
    MVO_REQADDRESS, MVO_REQSTATE, MVO_REQATTACH, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, MVO_REQINIT, 0, MVO_RPTADDRESS,
    MVO_RPTSTATE, MVO_RPTATTACH, MVO_RPTNEWMON, MVO_RPTNAUN,
    MVO_NNINCOMP, MVO_RPTMONERR, MVO_SOFTERR, MVO_RPTTXMTFWD};

static char *const svid_names[] = {
    "unknown", "beacon type", "naun", "local ring", "assign physical location",
    "soft error timer", "enabled class", "allowed priority", "unknown",
    "correlator", "last address", "physical location", "unknown", "unknown",
    "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
    "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
    "unknown", "unknown", "unknown", "unknown", "unknown", "unknown",
    "response", "address modifier", "product id", "ucode level", "unknown",
    "unknown", "wrap data", "frame forward", "station id", "status subvector",
    "transmit status", "group address", "functional address",
    "isolating counts", "nonisolating counts", "unknown", "error"};   

static const int svid_bits[] = {0, SV_BEACONTYPE, SV_NAUN, SV_LOCALRING,
    SV_ASSGN_PHYS_LOC, SV_SOFTERRTIMER, SV_ENABLEDCLASS, SV_ALLOWEDPRIORITY,
    0, SV_CORRELATOR, SV_LASTADDRESS, SV_PHYS_LOC, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, SV_RESPONSE, SV_ADDRESS_MOD,
    SV_PRODUCTID, SV_UCODELEVEL, 0, 0, SV_WRAPDATA, SV_FRAMEFORWARD,
    SV_STATIONID, SV_STATUSSUBVEC, SV_XMITSTATUS, SV_GROUPADDRESS,
    SV_FUNCTIONALADDRESS, SV_ISOLATINGCOUNTS, SV_NONISOLATINGCOUNTS, 0,
    SV_ERROR};

static char *const beacon_names[] = {
    "none", "recovery mode set", "signal loss error",
    "streaming, not Claim Token", "streaming, Claim Token"};

static char *const error_names[] = {
    "none", "monitor error", "dup monitor", "dup address"};

static char *const response_string[] = {
    "0", "Missing MVID", "MVID length", "Unrecognized MVID", 
    "Inappropriate source class", "SVID length", "Transmit Forward invalid",
    "Missing required SVID", "Unrecognized required SVID", 
    "MAC frame too large", "Function disabled"};

static inline boolean valid_mvid (
    int command)
{
    return(command <= MVID_LAST);
}

static inline boolean valid_svid (
    int command)
{
    return(command <= SVID_LAST);
}

char *mvid_to_str (int command)
{
    return(valid_mvid(command) ? mvid_names[command] : "unknown");
}

static inline char *svid_to_string (
    int command)
{
    return(valid_svid(command) ? svid_names[command] : "unknown");
}

static inline char *sv_beacon_to_str (
    int beacontype)
{
    return((beacontype <= 4) ? beacon_names[beacontype] : "unknown");
}

static inline char *sv_errcode_to_str (
    int error)
{
    return((error <= 3) ? error_names[error] : "unknown");
}

/*
 *
 */
boolean parse_mac_subvectors (
    subvector_t *sv,
    ulong length,
    uchar mvid,
    mac_data_ptrs *result,
    int *error)
{
    ulong bit, present;
    uchar i;

    present = 0;
    bzero ((uchar *)result, sizeof(mac_data_ptrs));
    for (i=0; length > 0 && i <= SVID_LAST; length -= sv->length, i++,
	    sv = (subvector_t *)((int)sv + sv->length)) {
	if (!valid_svid(sv->command)) {
	    lnmmacbuginf("\nLANMGR: illegal subvector (%#x) received.",
			 sv->command);
	    *error = RESP_UNKN_SVID;
	    return(FALSE);
	}
	bit = svid_bits[sv->command];
	if (((mvr_bits[mvid] | mvo_bits[mvid]) & bit) == 0) {
	    lnmmacbuginf("\nLANMGR: unexpected %s subvector received.",
		     svid_to_string(sv->command));
	    *error = RESP_UNKN_SVID;
	    return(FALSE);
	}

	present |= bit;
	switch (sv->command) {
	case SVID_BEACONTYPE:
            if (result->beacontypeDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->beacontype = (ushort *)&sv->data;
            result->beacontypeDone = TRUE;
	    break;
	    
	case SVID_NAUN:
            if (result->naunDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->naun = (uchar *)&sv->data;
            result->naunDone = TRUE;
	    break;
	    
	case SVID_LOCALRING:
            if (result->localringDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->localring = (ushort *)&sv->data;
            result->localringDone = TRUE;
	    break;
	    
	case SVID_ASSGN_PHYS_LOC:
            if (result->physAssgnLocDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->phys_location = (ulong *)&sv->data;
            result->physAssgnLocDone = TRUE;
	    break;
	    
	case SVID_SOFTERRTIMER:
            if (result->softerrTimerDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->soft_err_timer = (ushort *)&sv->data;
            result->softerrTimerDone = TRUE;
	    break;
	    
	case SVID_ENABLEDCLASS:
            if (result->enabledclassDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->enabledclass = (ushort *)&sv->data;
            result->enabledclassDone = TRUE;
	    break;
	    
	case SVID_ALLOWEDPRIORITY:
            if (result->allowedPriDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->allowedpriority = (ushort *)&sv->data;
            result->allowedPriDone = TRUE;
	    break;
	    
	case SVID_CORRELATOR:
            if (result->corrDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->correlator = (ushort *)&sv->data;
            result->corrDone = TRUE;
	    break;
	    
	case SVID_LASTADDRESS:
            if (result->lastaddrDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->lastaddress = (uchar *)&sv->data;
            result->lastaddrDone = TRUE;
	    break;
	    
	case SVID_PHYS_LOC:
            if (result->phyLocDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->phys_loc = (ulong *)&sv->data;
            result->phyLocDone = TRUE;
	    break;
	    
	case SVID_RESPONSE:
            if (result->respDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->response = (response_t *)&sv->data;
            result->respDone = TRUE;
	    break;
	    
	case SVID_ADDRESS_MOD:
            if (result->addrModDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->address_mod = (ushort *)&sv->data;
            result->addrModDone = TRUE;
	    break;
	    
	case SVID_PRODUCTID:
            if (result->prodIdDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->productid = (uchar *)&sv->data;
            result->prodIdDone = TRUE;
	    break;
	    
	case SVID_UCODELEVEL:
            if (result->ucodeLvlDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->ucodelevel = (uchar *)&sv->data;
            result->ucodeLvlDone = TRUE;
	    break;
	    
	case SVID_FRAMEFORWARD:
            if (result->forDataLenDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->forward_data = (uchar *)&sv->data;
	    result->forward_len = sv->length - SVID_HDR_LEN;
            result->forDataLenDone = TRUE;
	    break;
	    
	case SVID_STATIONID:
            if (result->statIdDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->stationid = (uchar *)&sv->data;
            result->statIdDone = TRUE;
	    break;
	    
	case SVID_STATUSSUBVEC:
            if (result->statSubvDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->statussubvec = (uchar *)&sv->data;
            result->statSubvDone = TRUE;
	    break;
	    
	case SVID_XMITSTATUS:
            if (result->xmitstatDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->xmitstatus = (ushort *)&sv->data;
            result->xmitstatDone = TRUE;
	    break;
	    
	case SVID_GROUPADDRESS:
            if (result->groupDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->group = (ulong *)&sv->data;
            result->groupDone = TRUE;
	    break;
	    
	case SVID_FUNCTIONALADDRESS:
            if (result->funcDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->functional = (ulong *)&sv->data;
            result->funcDone = TRUE;
	    break;
	    
	case SVID_ISOLATINGCOUNTS:
            if (result->isoDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->isolating = (isolating_counts_t *)&sv->data;
            result->isoDone = TRUE;
	    break;
	    
	case SVID_NONISOLATINGCOUNTS:
            if (result->nonisoDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->noniso = (nonisolating_counts_t *)&sv->data;
            result->nonisoDone = TRUE;
	    break;
	    
	case SVID_ERROR:
            if (result->errDone) {
                lnmmacbuginf("\nLANMGR: repeated %s subvector received.",
                    svid_to_string(sv->command));
                return(FALSE);
            }
	    result->error = (ushort *)&sv->data;
            result->errDone = TRUE;
	    break;
	    
	default:
	    /* Shouldn't ever happen.  Should be caught above. */
	    lnmmacbuginf("\nLANMGR: unknown subvector (%#2x) received.", 
			sv->command);
	}
    }
    
    if (i >= SVID_LAST) {
       lnmmacbuginf("nLANMGR: mac packet is corrupted.. major vector has more than %d subvectors.", i);
       return(FALSE);
    }
    if (mvr_bits[mvid] & ~present) {
	lnmmacbuginf("\nLANMGR: missing required subvector(s) %x.",
		 mvr_bits[mvid] & ~present);
	*error = RESP_NO_REQ_SVID;
	return(FALSE);
    }
    *error = RESP_ACK;
    return(TRUE);
}

/*
 *
 */
static paktype *setup_macpacket2 (
    int class,
    hwidbtype *idb,
    uchar *address,
    int mvid)
{
    trrif_t *trh;
    paktype *pak;
    major_vector *major;

    pak = getbuffer(MAC_MAXSIZE);
    if (!pak)
	return(NULL);

    major = (major_vector *)major_vector_headstart(pak);
    pak->linktype = LINK_MAC;
    pak->acct_proto = ACCT_PROTO_LANMAN;
    pak->if_output = idb->firstsw;
    pak->datagramstart = (uchar *)major - TRING_ENCAPBYTES;
    trh = (trrif_t *)pak->datagramstart;
    ieee_copy(address,       trh->daddr);
    ieee_copy(idb->hardware, trh->saddr);
    major->dst_class = CLASS_RINGSTATION;
    major->src_class = class;
    major->command = mvid;
    return(pak);
}

paktype *setup_macpacket (int class, station_info *info, int mvid)
{
    return(setup_macpacket2(class, info->idb, info->address, mvid));
}

paktype *setup_macreply (paktype *pak, uchar mvid, subvector_t **sv)
{
    paktype *newpak;
    trrif_t *old, *new;
    major_vector *omajor, *nmajor;

    newpak = pak_duplicate(pak);
    if (newpak) {
	newpak->if_output = pak->if_input;
	pak->acct_proto = ACCT_PROTO_LANMAN;

	new = (trrif_t *)newpak->datagramstart;
	old = (trrif_t *)pak->datagramstart;
	ieee_copy(old->daddr, new->saddr);
	ieee_copy(old->saddr, new->daddr);
	new->daddr[0] &= ~TR_RII;
	if (old->saddr[0] & TR_RII) {
	    new->saddr[0] |= TR_RII;
	    new->rc_blth &= RC_RTMASK;
	    new->rc_dlf ^= RC_D;
	    nmajor = (major_vector *)(&new->rc_blth + (new->rc_blth & RC_LTHMASK));
	    omajor = (major_vector *)(&old->rc_blth + (old->rc_blth & RC_LTHMASK));
	} else {
	    nmajor = (major_vector *)&new->rc_blth;
	    omajor = (major_vector *)&old->rc_blth;
	}
	nmajor->dst_class = omajor->src_class;
	nmajor->src_class = omajor->dst_class;
	nmajor->command = mvid;
	*sv = (subvector_t *)&nmajor->data;
    }
    return(newpak);
}

/**********************************************************************
 *
 *			Ring Station Functions
 *
 **********************************************************************/
static const char rsrcvbad[] = "\nLANMGR%d: RS bad %s from %e.";
static const char rsrcvd[]   = "\nLANMGR%d: RS received %s from %e.";
static const char rsrcvinv[] = "\nLANMGR%d: RS received invalid command (%d) from %e.";
static const char rscant[]   = "\nLANMGR%d: RS cannot send %s to %e.";
static const char rssent[]   = "\nLANMGR%d: RS sending %s to %e.";
static const char rsringpoll[] = "\nLANMGR%d: RS %s watching ring poll.";

/*
 * rs_init
 *
 * Initialize Ring Station Driver values for this ring structure.  This
 * routine is called when the initial ring data structures (one per
 * interface) are created at boot time.  It is also called for each
 * structure created for a virtual ring.
 */
static void rs_init (
    ring_info *ring)
{
    ring->watch_poll = FALSE;
}

/*
 * watch_ring_poll
 *
 * When start is TRUE, set up the interface to begin watching the normal
 * ring polling.  This is called whenever a "Report NAUN Change" frame is
 * received, indicating a station has entered or left the ring, or
 * whenever the ring recovers from a beaconing state.
 *
 * When start is FALSE, an Active Monitor Present frame has just been
 * received.  Count it and see if it is time to stop watching the ring
 * polling.  Two full polls of the ring are watched, and then the
 * watching is disabled.  This allows the router to maintain an accurate
 * picture of who is present on the ring, without constantly chewing up
 * the processor watching ring polls on a stable ring.
 */
void watch_ring_poll (hwidbtype *idb, boolean start)
{
    ring_info *ring;
    
    ring = idb->ring_info;
    if (start && lanmgr_pid) {
	lnmmacebuginf(rsringpoll, idb->unit, "start");
	idb->tr_ring_mode |= RINGMODE_RS;
	ring->watch_poll = 2;
	if (idb->set_extra_addrs != NULL)
	    (void)(*idb->set_extra_addrs)(idb);
	return;
    }

    ring->watch_poll--;
    if (ring->watch_poll == 0) {
	lnmmacebuginf(rsringpoll, idb->unit, "stop");
	idb->tr_ring_mode &= ~RINGMODE_RS;
	if (idb->set_extra_addrs != NULL)
	    (void)(*idb->set_extra_addrs)(idb);
    }
}

void lanmgr_ringstat (
    hwidbtype *idb,
    ushort ringstat)
{
    ring_info *ring;
    bridge_info *bridge;
    station_info *info;
    int unit;

    if (!lanmgr_global_enable)
	return;

    ring = idb_to_ring(idb);
    if (!ring)
	return;
    unit = idb->unit;

    if (ringstat & RNG_WIRE_FAULT) {
	/*
	 * Whoops.  The cable got pulled.  Shut down the lan manager
	 * connections, since that's what a true blue bridge does.  Kind
	 * of silly for us, but...
	 */
	if ((bridge = ring_to_bridge(ring)))
	    lrm_terminating(bridge, LRM_TERM_RING_ERR);
	return;
    }

    if (ringstat & RNG_SIGNAL_LOSS) {
	/*
	 * Whoops.  Lost the interface.
	 */
	ring->rem_state = RS_PRM_BCN;
	ring->beaconing = BCN_PERMANENT;
	ring->last_beacon_type = BEACON_SIGNALLOSS;
	rem_temp_beaconing(ring, NULL, NULL);
	start_rem_timer();
	/*
	 * This will be reset by a "UP" ring status.
	 */
	return;
    }

    if (ringstat & RNG_HARD_ERROR) {
	/*
	 * beaconing on the ring
	 */
	if (ring->beaconing == BCN_NONE) {
	    ring->beaconing = BCN_BEACONING;
	    GET_TIMESTAMP(ring->last_beacon_time);
	}
	if (ringstat & RNG_BEACON) {
	    /*
	     * I'm transmitting
	     */
	    lnmmacebuginf("\nLANMGR%d: This station is beaconing.", unit);
	    ring->last_beacon_type =  BEACON_STRM_N_CLAIM;
	    ieee_copy(idb->hardware, ring->last_beacon_address);
	    info = find_station(idb->hardware, idb);
	    if (info) {
		ieee_copy(info->naun, ring->last_beacon_naun);
	    } else {
		ieee_zero(ring->last_beacon_naun);
	    }
	} else {
	    /*
	     * I'm receiving
	     */
	    lnmmacebuginf("\nLANMGR%d: Beaconing is present on the ring.", unit);
	}
	start_rem_timer();
    }

    if (!(ringstat & RNG_UNUSEABLE)) {
	/*
	 * Ring has recovered
	 */
	if ((ring->beaconing == BCN_BEACONING) ||
	    (ring->beaconing == BCN_PERMANENT)) {
	    ring->beaconing = BCN_POSTMORTEM;
	    lnmmacebuginf("\nLANMGR%d: Ring is no longer beaconing.", unit);
	}
	start_rem_timer();
	watch_ring_poll(idb, TRUE);
    }
}

/*
 * ringstation_input
 * 
 * Process a packet that was sent to the Ring Station MAC Class.  This
 * routine only handles those MAC packets that are part of the ring
 * maintenance process.  This allows the router to keep track of the
 * current state of the ring, and which stations are currently present on
 * the ring.  The only frames received by this routine should be AMP,
 * SMP, Beacon, Ring Purge, and Claim Token frames.
 */
void ringstation_input (
    major_vector *major,
    station_info *info,
    hwidbtype *idb)
{
    subvector_t *sv;
    ulong length;
    ring_info *ring;
    mac_data_ptrs results;
    int unit;
    int dummy;
    uchar command;

    /*
     * Setup
     */
    memset(&results, 0, sizeof(mac_data_ptrs));
    ring = idb->ring_info;
    unit = idb->unit;

    /*
     * Parse the received frame.
     */
    length = GETSHORT(&major->length) - MAC_MVID_LEN;
    command = major->command;
    lnmmacbuginf(rsrcvd, unit, mvid_names[command], info->address);
    sv = (subvector_t *)&major->data;
    if (!parse_mac_subvectors(sv, length, command, &results, &dummy)) {
	lnmmacbuginf(rsrcvbad, unit, mvid_names[command], info->address);
	return;
    }

    /*
     * Now process it.
     */
    switch (command) {
      case MVID_BEACON:
	if (ring->beaconing == BCN_NONE) {
	    ring->beaconing = BCN_BEACONING;
	    GET_TIMESTAMP(ring->last_beacon_time);
	}
	ring->last_beacon_type = GETSHORT(results.beacontype);
	ieee_copy(info->address, ring->last_beacon_address);
	ieee_copy(results.naun, ring->last_beacon_naun);
	if (results.phys_loc)
	    info->phys_location = GETLONG(results.phys_loc);
	start_rem_timer();
	break;

      case MVID_CLAIM:
	GET_TIMESTAMP(ring->last_claim_time);
	ieee_copy(info->address, ring->last_claim_address);
	ieee_copy(results.naun, info->naun);
	if (results.phys_loc)
	    info->phys_location = GETLONG(results.phys_loc);
        reg_invoke_inc_local_lnm_claim_token_counter(idb);
	break;

      case MVID_RINGPURGE:
	GET_TIMESTAMP(ring->last_purge_time);
	ieee_copy(info->address, ring->last_purge_address);
	ieee_copy(results.naun, info->naun);
	if (results.phys_loc)
	    info->phys_location = GETLONG(results.phys_loc);
        reg_invoke_inc_local_lnm_purge_token_counter(idb);
	break;

      case MVID_AMP:
        if (!ieee_equal(info->address, ring->amp)) {
            ieee_copy(info->address, ring->amp);
            ring->new_amp = TRUE;
        }
	ieee_copy(results.naun, info->naun);
	if (results.phys_loc)
	    info->phys_location = GETLONG(results.phys_loc);
	watch_ring_poll(idb, FALSE);
	break;

      case MVID_SMP:
	ieee_copy(results.naun, info->naun);
	if (results.phys_loc)
	    info->phys_location = GETLONG(results.phys_loc);
	break;

      default:
	break;
    }
}

/*
 */

static uchar const dummy_ucodelevel[UCODELEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uchar const dummy_statusvec[STATUSLEN] = {0, 0, 0, 0, 0, 0};
static uchar const dummy_productid[IDLEN] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static void vring_mac_adapter (
    major_vector *major,
    station_info *source_info,
    hwidbtype *idb,
    paktype *pak,
    uchar *daddr)
{
    subvector_t *sv;
    ulong length;
    ring_info *ring;
    mac_data_ptrs results;
    int unit;
    paktype *reply;
    uchar *naun, *temp;
    ulong resp;
    uchar command, response;
    int errorcode;
    tring_hdr *trh;
    major_vector *fwd_frame, *reply_major;
    station_info *info;
    uchar *reply_address;
    ushort srb_thisring;

    /*
     * Setup
     */
    memset(&results, 0, sizeof(mac_data_ptrs));
    ring = idb->ring_info;
    unit = idb->unit;
    reply_address = source_info->address;

    srb_thisring = srbV.srb_sb_get_thisring(idb->firstsw);

    /*
     * Look up the destination address.  This routine must respond as if
     * was a token ring adapter with that address.
     */
    if (ieee_equal(daddr, trbaddr2)) {
	temp = vr_first_virtual(srb_thisring);
	if (!temp)
	    return;
	ieee_copy(temp, daddr);
    }
    info = find_or_build_station(daddr, idb, FALSE);
    
    /*
     * Parse the received frame.
     */
    length = major->length - MAC_MVID_LEN;
    if ((length < MAC_MVID_LEN) || (length > MAX_TR_FRAMESIZE)) {
       lnmmacbuginf("\nLANMGR: bad packet length");
       return;
    }
    command = major->command;
    lnmmacbuginf(rsrcvd, unit, mvid_names[command], source_info->address);
    sv = (subvector_t *)&major->data;
    if (!parse_mac_subvectors(sv, length, command, &results, &errorcode))
	lnmmacbuginf(rsrcvbad, unit, mvid_names[command], source_info->address);
    
    /*
     * Now build a response.
     */
    response = MVID_RESPONSE;
    reply = setup_macreply(pak, MVID_RESPONSE, &sv);
    reply_major = (major_vector *)major_vector_headstart(reply);
    if (results.correlator)
	sv = assign_mac_correlator(sv, GETSHORT(results.correlator));

    switch (major->command) {
      case MVID_TRANSMITFWD:
	trh = (tring_hdr *)results.forward_data;
	if (trh->saddr[0] & TR_RII)
	    fwd_frame = (major_vector *)(trh->data + (trh->data[0] & RC_LTHMASK));
	else
	    fwd_frame = (major_vector *)trh->data;

	/*
	 * Contents of a 'Frame Forward' SVID must be a MAC Transmit
	 * Forward frame.
	 */
	if (fwd_frame->command != MVID_TRANSMITFWD) {
	    resp = (RESP_BAD_FWD << 16) | (major->dst_class << 12) |
		(major->src_class << 8) | major->command;
	    sv = assign_mac_long(sv, SVID_RESPONSE, resp);
	    break;
	}
	retbuffer(reply);
	
	/*
	 * Create the 'Report Transmit Forward' frame.  This frame will
	 * be transmitted at the end of this subroutine.
	 */
	response = MVID_RPTTXMTFWD;
	reply_address = tr_crs;
	reply = setup_macpacket2(CLASS_RINGSTATION, idb, tr_crs, MVID_RPTTXMTFWD);

	/*
	 * Freshen the major_vector pointer if required
	 */
	reply_major = (major_vector *)major_vector_headstart(reply);

	reply_major->dst_class = CLASS_CRS;
	if (reply) {
	    sv = assign_mac_short(sv, SVID_XMITSTATUS, 0x01CC);
    	}

	/*
	 * Extract the new frame from the 'Frame Forward' subvector, and
	 * transmit it.
	 */
	pak->if_output = idb->firstsw;
	clear_if_input(pak);
	pak->datagramstart = (uchar *)results.forward_data;
	pak->datagramsize = results.forward_len;
	(void)pak_lock(pak);
	datagram_out(pak);
	break;

      case MVID_CHANGEPARAM:
      case MVID_INITIALIZE:
	resp = (errorcode << 16) | (major->dst_class << 12) |
	    (major->src_class << 8) | major->command;
	sv = assign_mac_long(sv, SVID_RESPONSE, resp);
	break;

      case MVID_REQADDRESS:
	response = MVID_RPTADDRESS;
	reply_major->command = MVID_RPTADDRESS;
	naun = vr_next_virtual(srb_thisring, info->address);
	sv = assign_mac_address(sv, SVID_NAUN, naun ? naun : zeromac);
	sv = assign_mac_long(sv, SVID_PHYS_LOC, 0);
	sv = assign_mac_long(sv, SVID_GROUPADDRESS, 0);
	sv = assign_mac_long(sv, SVID_FUNCTIONALADDRESS, TRFUNC_BRIDGE);
	break;

      case MVID_REQSTATE:
	response = MVID_RPTSTATE;
	reply_major->command = MVID_RPTSTATE;
	sv = assign_mac_bytes(sv, SVID_UCODELEVEL, dummy_ucodelevel, UCODELEN);
	sv = assign_mac_address(sv, SVID_STATIONID, zeromac);
	sv = assign_mac_bytes(sv, SVID_STATUSSUBVEC, dummy_statusvec, STATUSLEN);
	break;

      case MVID_REQATTACH:
	response = MVID_RPTATTACH;
	reply_major->command = MVID_RPTATTACH;
	sv = assign_mac_short(sv, SVID_ENABLEDCLASS, 0x7FFF);
	sv = assign_mac_short(sv, SVID_ALLOWEDPRIORITY, 0x0006);
	sv = assign_mac_bytes(sv, SVID_PRODUCTID, dummy_productid, IDLEN);
	sv = assign_mac_long(sv, SVID_FUNCTIONALADDRESS, TRFUNC_BRIDGE);
	break;

      case MVID_REMOVE:
      default:
	/*
	 * Ignore it.
	 */
	retbuffer(reply);
	return;
    }

    /*
     * Send it.
     */
    if (!reply)
	return;

    finish_mvid(reply, sv);
    if ((idb->state == IDBS_UP) &&
	pak_encap(idb, reply,
		  ((major_vector *)major_vector_headstart(reply))->length,
		  (long)reply_address)) {
	lnmmacbuginf(rssent, idb->unit, mvid_names[response], reply_address);
	datagram_out(reply);
    } else {
	lnmmacbuginf(rscant, idb->unit, mvid_names[response], reply_address);
	datagram_done(reply);
    }
}

/**********************************************************************
 *
 *                 Ring Parameter Server Functions
 *
 **********************************************************************/

static const char rpscant[]   = "\nLANMGR%d: RPS cannot send %s to %e.";
static const char rpscant2[]  = "\nLANMGR%d: RPS cannot send %s to %s.";
static const char rpsretry[]  = "\nLANMGR%d: RPS retry count exceeded sending %s to %e.";
static const char rpscorr[]   = "\nLANMGR%d: RPS correlator mismatch, rcvd %d, expected %d";
static const char rpsrcvbad[] = "\nLANMGR%d: RPS received bad %s from %e.";
static const char rpsrcvd[]   = "\nLANMGR%d: RPS received %s from %e.";
static const char rpsrcvinv[] = "\nLANMGR%d: RPS received invalid command (%s) from %e.";
static const char rpssent[]   = "\nLANMGR%d: RPS sending %s to %e.";
static const char rpssent2[]  = "\nLANMGR%d: RPS sending %s to %s.";

void free_rps_pak (station_info *info)
{
    if (info->rps_pak)
	datagram_done((paktype *)info->rps_pak);
    info->rps_correlator = 0;
    info->rps_count = 0;
    TIMER_STOP(info->rps_time);
    info->rps_pak = NULL;
}

static boolean send_rps_pak (
    paktype *pak,
    hwidbtype *idb,
    station_info *info,
    boolean save,
    ushort correlator)
{
    major_vector *major;
    tring_hdr *trh;

    major = (major_vector *)major_vector_headstart(pak);

    if ((idb->state == IDBS_UP) && (!info->rps_pak) &&
	pak_encap(idb, pak, major->length, (long)info->address)) {
	lnmmacbuginf(rpssent, idb->unit, mvid_names[major->command],
		     info->address);
	if (save) {
	    info->rps_correlator = correlator;
	    info->rps_count = RPS_MAX_TRIES;
	    TIMER_START(info->rps_time, ONESEC);
	    info->rps_pak = pak;
	    pak_lock(pak);
	    start_lanmgr_timer();
	}
	if ((major->command == MVID_INITIALIZE) &&
            (pak->if_output->hwptr->mac_use_exp_buf == TRUE)) {
            trh = (tring_hdr *)pak->datagramstart;
            trh->fc = FC_TYPE_MAC_EXP;
        }
	datagram_out(pak);
 	return(TRUE);
   } else {
	lnmmacbuginf(rpscant, idb->unit, mvid_names[major->command],
		     info->address);
	datagram_done(pak);
	return(FALSE);
    }
}

void resend_rps_pak (station_info *info)
{
    paktype *pak, *pak1;
    major_vector *major;

    pak = info->rps_pak;

    /* Make sure the output interface is set (CSCdi46764). */
    if ((!pak->if_output) || (!pak->if_output->hwptr)) {
        lnmmacebuginf("\nLANMGR: RPS no output interface.  Aborting resend.");
        /* Inform lanmgr of failure. */
        rps_report_station(info);
        free_rps_pak(info);
        return;
    }

    major = (major_vector *)major_vector_headstart(pak);

    if ((!info->rps_count--) || (info->idb->state != IDBS_UP)) {
	lnmmacebuginf(rpsretry, pak->if_output->hwptr->unit,
		      mvid_names[major->command], info->address);
	/* Inform lanmgr of failure. */
	rps_report_station(info);
	free_rps_pak(info);
	return;
    }

    /*
     * packet is already encalsulated from the first transmit
     */
    lnmmacbuginf("\nLANMGR%d: RPS resending to station %e.",
	      pak->if_output->hwptr->unit, info->address);
    if ((pak1 = pak_duplicate(pak))) {
        pak1->if_output = pak->if_output;
        datagram_out(pak1);
    }
    TIMER_UPDATE(info->rps_time, ONESEC);
    start_lanmgr_timer();
}

/*
 * Process a packet that was sent to the Ring Parameter MAC Class.  This
 * should be either a Initialization Request or a Response.
 */
void rps_input (
    major_vector *major,
    station_info *info,
    hwidbtype *idb)
{
    subvector_t *sv;
    ulong length;
    paktype *newpak;
    major_vector *newmajor;
    mac_data_ptrs results;
    int unit, dummy;
    ushort new_correlator;
    uchar command;

    /*
     * Setup
     */
    memset(&results, 0, sizeof(mac_data_ptrs));
    unit = idb->unit;

    /*
     * Parse the received frame.
     */
    length = major->length - MAC_MVID_LEN;
    command = major->command;
    lnmmacbuginf(rpsrcvd, unit, mvid_names[command], info->address);
    sv = (subvector_t *)&major->data;
    if (!parse_mac_subvectors(sv, length, command, &results, &dummy)) {
	lnmmacbuginf(rpsrcvbad, unit, mvid_names[command], info->address);
	return;
    }

    switch (command) {
      case MVID_RESPONSE:
	if (!info->rps_correlator) {
	    /* true for initial broadcast initialize */
	    break;
	}
	if (results.correlator && (info->rps_correlator != *results.correlator)) {
	    lnmmacbuginf(rpscorr, unit, GETSHORT(results.correlator),
			 info->rps_correlator);
	    break;
	}
	if (results.response->code & RESP_ERROR) {
	    lnmmacbuginf("\nLANMGR%d: RPS response error '%s' from %e.", unit,
		     response_string[results.response->code & ~RESP_ERROR],
		     info->address);
	    info->attach_status = RPS_STAT_NACK;
	    resend_rps_pak(info);
	    break;
	}

	lnmmacebuginf("\nLANMGR%d: RPS initialized station %e.",unit,info->address);
	info->attach_status = RPS_STAT_OK;
	rps_report_station(info);
	free_rps_pak(info);
	break;
	    

      case MVID_REQINIT:
	if (info->rps_correlator) {
	    lnmmacbuginf("\nLANMGR%d: RPS rcvd REQ INIT from %e, message already outstanding.",
		      unit, info->address);
	    /* We have an outstanding request with this guy... */
	    break;
	}

	/*
	 * Copy in the information present
	 */
	ieee_copy(results.naun, info->naun);
	bcopy(results.productid, info->product_id, IDLEN);
	bcopy(results.ucodelevel, info->ucode, UCODELEN);
	if (results.address_mod)
	    info->address_modifier = *results.address_mod;
	
	/*
	 * Build and send a initialize command.
	 */
	newpak = setup_macpacket(CLASS_RPS, info, MVID_INITIALIZE);
	if (!newpak) {
	    lnmmacbuginf(rpscant,unit,mvid_names[MVID_INITIALIZE],info->address);
	    break;
	}
	newmajor = (major_vector *)major_vector_headstart(newpak);
 	new_correlator = rps_correlator++;
	info->attach_status = RPS_STAT_NO_RESP;
	sv = (subvector_t *)&newmajor->data;
	sv = assign_mac_correlator(sv, new_correlator);
	sv = assign_mac_ring(sv, idb);
	sv = assign_mac_location(sv, info);
	sv = assign_mac_softerr_timer(sv, idb->ring_info);
	finish_mvid(newpak, sv);
	send_rps_pak(newpak, idb, info, TRUE, new_correlator);
	break;

      default:
	break;
    }
}

/*
 * Start up the Ring Parameter Server on each interface where
 * requested.  This involved serveral steps.
 *
 *     1) Check to see if there is a RPS already active on the ring.
 *     2) If not, send a broadcast initialize packet and go into active
 *        mode as the RPS.
 *     3) If so, compare parameters.
 *        3a) If different, go into passive mode.
 *        3b) If the same, go into active mode (without the broadcast).
 *
 * This function is called by the token ring driver when it has finished
 * initializing and inserted onto the ring.  It passes in the ring number
 * (if any) assigned during the ring insertion process.
 */
void rps_startup (hwidbtype *idb, int assigned)
{
    paktype *pak;
    major_vector *major;
    subvector_t *sv;
    ring_info *ring;
    ushort srb_thisring;

    /*
     * Don't do something if this interface does not support LAN MANAGER
     */
     if (!lanmgr_global_enable)
	return;

    srb_thisring = srbV.srb_sb_get_thisring(idb->firstsw);
    ring = idb_to_ring(idb);
    if (!ring->num_mismatch_allowed) {
        if (srb_thisring && assigned && (srb_thisring != assigned)) {
            errmsg(&msgsym(BADRNGNUM, LANMGR), idb->hw_namestring);
            shutdown_interface(idb->firstsw, TRUE);
            return;
        }
    }
    /*
     * Check here to see if we should be active.  First, we need to wait
     * until the interface is up before we can manke any decision.  This
     * is required so that we can see if there is another RPS present on
     * the ring.  If there is, 'assigned' contains the ring number
     * assigned the other ring parameter server.  Also make sure that we
     * only ever execute this routine once.
     */
    if (idb->state != IDBS_UP)
	return;
    if (!(idb->tr_ring_mode & RINGMODE_RPS) || !srb_thisring) {
	ring->rps_state = RPS_STATE_OFF;
	return;
    }
    if (ring->rps_state != RPS_STATE_DETERMINE)
	return;
    if (assigned == -1) {
	errmsg(&msgsym(OLDTRAMON, LANMGR), idb->hw_namestring);
	idb->tr_ring_mode &= ~RINGMODE_RPS;
	ring->rps_state = RPS_STATE_OFF;
	(void)(*idb->reset)(idb);
	return;
    }

    /*
     * Send initialize packet to the ring.
     */
    ring->rps_state = RPS_STATE_ACTIVE;
    pak = setup_macpacket2(CLASS_RPS, idb, trbaddr2, MVID_INITIALIZE);
    if (pak) {
	major = (major_vector *)major_vector_headstart(pak);
	sv = (subvector_t *)&major->data;
	sv = assign_mac_ring(sv, idb);
	sv = assign_mac_softerr_timer(sv, ring);
	finish_mvid(pak, sv);
	if ((idb->state == IDBS_UP) &&
	    pak_encap(idb, pak, major->length, (long)trbaddr2)) {
	    lnmmacbuginf(rpssent2, idb->unit, mvid_names[MVID_INITIALIZE],
		     idb->hw_namestring);
	    datagram_out(pak);
	} else {
	    datagram_done(pak);
	}
    } else {
	lnmmacbuginf(rpscant2, idb->unit, mvid_names[MVID_INITIALIZE],
		     idb->hw_namestring);
    }
}

/*
 * rps_init
 *
 * Initialize Ring Parameter Driver values for this ring structure.
 */
void rps_init (ring_info *ring)
{
    ring->softerr_timer = RPS_SOFTERR_DEFAULT;
    ring->rps_state = RPS_STATE_OFF;
    ring->location_count = 1;
    ring->num_mismatch_allowed = FALSE;
}


/**********************************************************************
 *
 *		Configuration Report Server Functions
 *
 **********************************************************************/

static const char crscant[]  = "\nLANMGR%d: CRS cannot send %s to %e.";
static const char crsretry[] = "\nLANMGR%d: CRS retry count exceeded sending %s to %e.";
static const char crscorr[]  = "\nLANMGR%d: CRS correlator mismatch, rcvd %d, expected %d";
static const char crsrcvbad[]= "\nLANMGR%d: CRS received bad %s from %e.";
static const char crsrcvd[]  = "\nLANMGR%d: CRS received %s from %e.";
static const char crsrcvinv[]= "\nLANMGR%d: CRS received invalid command (%s) from %e.";
static const char crssent[]  = "\nLANMGR%d: CRS sending %s to %e.";

void free_crs_pak (station_info *info)
{
    if (info->crs_pak)
	datagram_done((paktype *)info->crs_pak);
    info->crs_correlator = 0;
    info->crs_count = 0;
    TIMER_STOP(info->crs_time);
    info->crs_pak = NULL;
}

static boolean send_crs_pak (
    paktype *pak,
    hwidbtype *idb,
    station_info *info,
    boolean save,
    ushort correlator)
{
    major_vector *major;

    major = (major_vector *)major_vector_headstart(pak);

    if ((idb->state == IDBS_UP) && (!info->crs_pak) && (!is_fddi(idb)) &&
	pak_encap(idb, pak, major->length,  (long)info->address)) {
	lnmmacbuginf(crssent, idb->unit, mvid_names[major->command],
		     info->address);
	if (save) {
	    info->crs_correlator = correlator;
	    info->crs_count = CRS_MAX_TRIES;
	    TIMER_START(info->crs_time, ONESEC);
	    info->crs_pak = pak;
	    pak_lock(pak);
	    start_lanmgr_timer();
	}
	datagram_out(pak);
 	return(TRUE);
   } else {
	lnmmacbuginf(crscant, idb->unit, mvid_names[major->command],
		     info->address);
	datagram_done(pak);
	if (is_fddi(idb))
	    return(TRUE);
	return(FALSE);
    }
}

void resend_crs_pak (station_info *info)
{
    paktype *pak, *pak1;
    major_vector *major;
    int unit;
    uchar mvcmd;

    pak = info->crs_pak;
    if(!pak) {
        lnmmacbuginf("\nLANMGR: resend_crs_pak - called with NULL pak");
        bugtrace();
        return;
    }
    major = (major_vector *)major_vector_headstart(pak);
    if( (!pak->if_output) || (!pak->if_output->hwptr)) {
        return;
    }
    unit = pak->if_output->hwptr->unit;
    if ((!info->crs_count--) || (info->idb->state != IDBS_UP)) {
	mvcmd = major->command;
	lnmmacebuginf(crsretry,unit,mvid_names[major->command],
		      info->address);
	/* Free the packet first */
	free_crs_pak(info);

	/* Inform lanmgr of failure. This may generate a new packet. */
	crs_success_fail(info, mvcmd, FALSE, 0);
	return;
    }

    /*
     * packet is already encalsulated from the first transmit
     */
    lnmmacbuginf("\nLANMGR%d: CRS resending to station %e.", unit,
		 info->address);

    TIMER_START(info->crs_time, ONESEC);
    if ((pak1 = pak_duplicate(pak))) {
        pak1->if_output = pak->if_output;
        datagram_out(pak1);
    }
    start_lanmgr_timer();
}

/*
 * send_crs_general
 *
 * Build and send a the requested CRS mac frame.  This will be one of 1)
 * Request Station Address, 2) Request Station State, or 3) Request
 * Station Attachments. This routine is invoked because a LAN Manager
 * somewhere requested this information.
 */
boolean send_crs_general (
    station_info *info,
    uchar command,
    int attempts)
{
    paktype *pak;
    major_vector *major;
    subvector_t *sv;
    hwidbtype *idb;
    ushort new_correlator;

    idb = info->idb;
    pak = setup_macpacket(CLASS_CRS, info, command);
    if (!pak) {
	lnmmacbuginf(crscant, idb->unit, mvid_names[command], info->address);
	return(FALSE);
    }

    new_correlator = crs_correlator++;
    major = (major_vector *)major_vector_headstart(pak);
    sv = (subvector_t *)&major->data;
    sv = assign_mac_correlator(sv, new_correlator);
    finish_mvid(pak, sv);
    return(send_crs_pak(pak, idb, info, TRUE, new_correlator));
}

/*
 * send_crs_remove
 *
 * Build and send a CRS death packet.  This is a "Remove Station" frame.
 */
boolean send_crs_remove (station_info *info)
{
    paktype *pak;
    major_vector *major;
    hwidbtype *idb;

    idb = info->idb;
    pak = setup_macpacket(CLASS_CRS, info, MVID_REMOVE);
    if (!pak) {
	lnmmacbuginf(crscant, idb->unit, mvid_names[MVID_REMOVE], info->address);
	return(FALSE);
    }

    major = (major_vector *)major_vector_headstart(pak);
    major->length = 4;
    return(send_crs_pak(pak, idb, info, FALSE, 0));
}

/*
 * send_crs_set_sta
 */
boolean send_crs_set_sta (
    station_info *info,
    ushort *localring,
    ulong *location,
    ushort *softerr,
    ulong *enabled_func,
    ushort *allow_prio)
{
    paktype *pak;
    major_vector *major;
    subvector_t *sv;
    hwidbtype *idb;
    ushort new_correlator;

    idb = info->idb;
    pak = setup_macpacket(CLASS_CRS, info, MVID_CHANGEPARAM);
    if (!pak) {
	lnmmacbuginf(crscant, idb->unit, mvid_names[MVID_CHANGEPARAM], info->address);
	return(FALSE);
    }

    major = (major_vector *)major_vector_headstart(pak);
    new_correlator = crs_correlator++;
    sv = (subvector_t *)&major->data;
    sv = assign_mac_correlator(sv, new_correlator);
    if (localring)    sv = assign_mac_short(sv, SVID_LOCALRING, *localring);
    if (location)     sv = assign_mac_location(sv, info);
    if (softerr)      sv = assign_mac_short(sv, SVID_SOFTERRTIMER, *softerr);
    if (enabled_func) sv = assign_mac_short(sv, SVID_ENABLEDCLASS, *enabled_func);
    if (allow_prio)   sv = assign_mac_short(sv, SVID_ALLOWEDPRIORITY, *allow_prio);
    finish_mvid(pak, sv);
    return(send_crs_pak(pak, idb, info, TRUE, new_correlator));
}

/*
 *
 */
void crs_input (major_vector *major, station_info *info, hwidbtype *idb)
{
    subvector_t *sv;
    ulong length;
    station_info *info2;
    mac_data_ptrs args;
    int unit, dummy;
    ushort code;
    uchar command;

    /*
     * Setup
     */
    memset(&args, 0, sizeof(mac_data_ptrs));
    unit = idb->unit;
    info2 = NULL;

    /*
     * Parse the received frame.
     */
    length = major->length - MAC_MVID_LEN;
    command = major->command;
    lnmmacbuginf(crsrcvd, unit, mvid_names[command], info->address);
    sv = (subvector_t *)&major->data;
    if (!parse_mac_subvectors(sv, length, command, &args, &dummy)) {
	lnmmacbuginf(crsrcvbad, unit, mvid_names[command], info->address);
	return;
    }

    switch (command) {
      case MVID_RESPONSE:
	if (!info->crs_correlator) {
	    break;
	}
	if (args.correlator && 
	    (info->crs_correlator != GETSHORT(args.correlator))) {
	    lnmmacbuginf(crscorr,unit,GETSHORT(args.correlator),
			 info->crs_correlator);
	    break;
	}
	if (args.response->code & RESP_ERROR) {
	    code = args.response->code & ~RESP_ERROR;
	    lnmmacbuginf("\nLANMGR%d: CRS response error '%s' from %e.",
		     unit, response_string[code], info->address);
	    break;
	}
	crs_success_fail(info, args.response->mvid, TRUE, major->data.d.lword);
	free_crs_pak(info);
	break;
	
      case MVID_RPTADDRESS:
	if (args.correlator && 
	    (info->crs_correlator != GETSHORT(args.correlator))) {
	    info2 = find_station(tr_amp, idb);
	    if (!info2 || 
		(info2->crs_correlator != GETSHORT(args.correlator))) {
		lnmmacbuginf(crscorr,unit,GETSHORT(args.correlator),
			     info->crs_correlator);
		break;
	    }
	    info->request_data = info2->request_data;
	    info2->request_data = NULL;
	}
	ieee_copy(args.naun, info->naun);
	info->phys_location = GETLONG(args.phys_loc);
	info->functional = GETLONG(args.functional);
	info->group = GETLONG(args.group);
	if (args.address_mod)
	    info->address_modifier = GETSHORT(args.address_mod);
	free_crs_pak(info2 ? info2 : info);
	crs_got_info(info, MVID_RPTADDRESS);
	break;

      case MVID_RPTSTATE:
	if (args.correlator && 
	    (info->crs_correlator != GETSHORT(args.correlator))) {
	    lnmmacbuginf(crscorr,unit,GETSHORT(args.correlator),
			 info->crs_correlator);
	    break;
	}
	bcopy(args.ucodelevel, info->ucode, UCODELEN);
	bcopy(args.statussubvec, info->status_vector, STATUSLEN);
	if (args.stationid)
	    ieee_copy(args.stationid, info->station_id);
	free_crs_pak(info);
	crs_got_info(info, MVID_RPTSTATE);
	break;

      case MVID_RPTATTACH:
	if (args.correlator && 
	    (info->crs_correlator != GETSHORT(args.correlator))) {
	    lnmmacbuginf(crscorr,unit,GETSHORT(args.correlator),
			 info->crs_correlator);
	    break;
	}
	info->enabled_functions = GETSHORT(args.enabledclass);
	info->allowed_priority = GETSHORT(args.allowedpriority);
	bcopy(args.productid, info->product_id, IDLEN);
	info->functional = GETLONG(args.functional);
	free_crs_pak(info);
	crs_got_info(info, MVID_RPTATTACH);
	break;
	    
      case MVID_RPTNEWMON:
	ieee_copy(args.naun, info->naun);
	info->phys_location = GETLONG(args.phys_loc);
	bcopy(args.productid, info->product_id, IDLEN);
	crs_report_new_monitor(info);
	break;
	    
      case MVID_RPTNAUN:
	ieee_copy(args.naun, info->naun);
	info->phys_location = GETLONG(args.phys_loc);
	crs_report_naun_change(info);
	break;

      case MVID_RPTTXMTFWD:
	info->transmit_status = GETSHORT(args.xmitstatus);
	crs_report_transmit_fwd(info);
	break;

      default:
	break;
    }
}

void crs_init (ring_info *ring)
{
    /* do nothing */
}

/**********************************************************************
 *
 *		     Ring Error Monitor Functions
 *
 **********************************************************************/

static const char remrcvbad[]= "\nLANMGR%d: REM bad %s from %e.";
static const char remrcvd[]  = "\nLANMGR%d: REM received %s from %e.";
static const char remrcvinv[]= "\nLANMGR%d: rem received invalid command (%s) from %e.";

static boolean rem_report_test (
    mac_data_ptrs *r,
    ushort mask)
{
    if (!mask)
	return(FALSE);
    if (((mask & REM_MM_I_LINE)      && r->isolating->line)            ||
	((mask & REM_MM_I_INTERNAL)  && r->isolating->internal)        ||
	((mask & REM_MM_I_BURST)     && r->isolating->burst)           ||
	((mask & REM_MM_I_AC)        && r->isolating->ac)              ||
	((mask & REM_MM_I_ABORTS)    && r->isolating->abort)           ||
	((mask & REM_MM_N_LOSTFRAME) && r->noniso->lost_frame)   ||
	((mask & REM_MM_N_RCVRCONG)  && r->noniso->congestion)   ||
	((mask & REM_MM_N_FC)        && r->noniso->frame_copied) ||
	((mask & REM_MM_N_FREQ)      && r->noniso->frequency)    ||
	((mask & REM_MM_N_TOKEN)     && r->noniso->token))
	    return(TRUE);
    return(FALSE);
}

static void rem_process_softerr (
    mac_data_ptrs *results,
    station_info *info,
    ring_info *ring)
{
    boolean report_weight, report_preweight, report_noniso;
    boolean report_congest, fwd_frame;
    station_info *naun;
    uchar *address = info->address;
    int ringno = srbV.srb_sb_get_thisring(firstsw_or_null(info->idb));

    report_weight = report_preweight = report_noniso = FALSE;
    report_congest = fwd_frame = FALSE;

    if (!info->error_state) {
	lnmebuginf("\nIBMNM%d: Adding %e to error list", ringno, address);
	info->nexterror = ring->station_errors;
	info->error_state = ERR_EXIST;
	ring->station_errors = info;
	if (ring->pass <= 0)
	    ring->pass = REM_DECREMENT_INTERVAL / REM_WORK_INTERVAL;
	start_rem_timer();
    }
    if (!(naun = find_station(info->naun, info->idb)))
	return;

    if (!naun->error_state) {
	naun->nexterror = ring->station_errors;
	naun->error_state = ERR_EXIST;
	ring->station_errors = naun;
    }

    /*
     * Count errors
     */
    info->isolating.line     += results->isolating->line;
    info->isolating.internal += results->isolating->internal;
    info->isolating.burst    += results->isolating->burst;
    info->isolating.ac       += results->isolating->ac;
    info->isolating.abort    += results->isolating->abort;
    
    ring->noniso.lost_frame   += results->noniso->lost_frame;
    ring->noniso.congestion   += results->noniso->congestion;
    ring->noniso.frame_copied += results->noniso->frame_copied;
    ring->noniso.frequency    += results->noniso->frequency;
    ring->noniso.token        += results->noniso->token;

    /*
     * Bump weights.
     */
    info->weight += 2 * results->isolating->line;
    info->weight += 4 * results->isolating->internal;
    info->weight += 2 * results->isolating->burst;
    info->weight += 4 * results->isolating->abort;
    info->weight = min(info->weight, REM_WEIGHT_LIMIT);

    naun->weight += 2 * results->isolating->line;
    naun->weight += 2 * results->isolating->burst;
    naun->weight += 5 * results->isolating->ac;
    naun->weight = min(naun->weight, REM_WEIGHT_LIMIT);

    /*
     * Check Thresholds and report if newly exceeded.
     */
    if (!(info->error_state & ERR_ME_PREWEIGHT)) {
	if (info->weight >= REM_THRESH_PREWEIGHT) {
	    info->error_state |= ERR_ME_PREWEIGHT;
	    report_preweight = TRUE;
	    lnmebuginf("\nIBMNM%d: Station %e going into preweight condition",
		      ringno, address);
	}
    } else if (!(info->error_state & ERR_ME_WEIGHT)) {
	if (info->weight >= REM_THRESH_WEIGHT) {
	    info->error_state |= ERR_ME_WEIGHT;
	    report_weight = TRUE;
	    lnmebuginf("\nIBMNM%d: Station %e going into weight condition",
		      ringno, address);
	}
    }
    if (!(naun->error_state & ERR_ME_PREWEIGHT)) {
	if (naun->weight >= REM_THRESH_PREWEIGHT) {
	    naun->error_state |= ERR_ME_PREWEIGHT;
	    lnmebuginf("\nIBMNM%d: Station %e going into preweight condition",
		      ringno, naun->address);
	}
    } else if (!(naun->error_state & ERR_ME_WEIGHT)) {
	if (naun->weight >= REM_THRESH_WEIGHT) {
	    naun->error_state |= ERR_ME_WEIGHT;
	    lnmebuginf("\nIBMNM%d: Station %e going into weight condition",
		      ringno, naun->address);
	}
    }
    if ((ring->noniso.lost_frame   >= REM_NONISO_LIMIT) ||
	(ring->noniso.congestion   >= REM_NONISO_LIMIT) ||
	(ring->noniso.frame_copied >= REM_NONISO_LIMIT) ||
	(ring->noniso.frequency    >= REM_NONISO_LIMIT) ||
	(ring->noniso.token        >= REM_NONISO_LIMIT)) {
	report_noniso = TRUE;
	lnmebuginf("\nIBMNM%d: Non-isolating error threshold exceeded.", ring->ringno);
    }

    /*
     * Keep track of congestion notification
     */
    if (results->noniso->congestion) {
	info->congestion_count++;
	TIMER_START(info->congestion_time, REM_CONGEST_TIME);
	info->error_state |= ERR_ME_CONGEST;
	if (info->congestion_count > ring->congestion_thresh) {
	    report_congest = TRUE;
	    lnmebuginf("\nIBMNM%d: Adapters experiencing congestion.", ring->ringno);
	}
    }

    /*
     * Does the lan manager want to see individual frame reports.
     */
    fwd_frame = FALSE;
    if (ring->notif_flags & ENA_INT_RING) 
	fwd_frame = rem_report_test(results, ring->ring_inten_mask);
    else if ((ring->notif_flags & ENA_INT_AUTO) && (info->error_state & ERR_FAULT))
	fwd_frame = rem_report_test(results, ring->auto_inten_mask);

    /*
     * Send any messages to the lan manager.
     */
    if (fwd_frame)
	rem_forward_mac_frame(info, MVID_SOFTERR, (uchar *)results->isolating,
			      (uchar *)results->noniso);
    if (report_preweight)
	rem_xxx_notification(REM_PREWEIGH_EXC, info);
    else if (report_weight)
	rem_xxx_notification(REM_WEIGHT_EXC, info);
    if (report_noniso)
	rem_noniso_thresh(info->idb);
    if (report_congest)
	rem_congestion_xxx(info, REM_RCVR_CNG);

    /*
     * Schedule wakeup.
     */
    start_rem_timer();
}

/*
 * rem_purge_softerr
 *
 * Erase all soft errors.  This is invoked my Lan Manager sending us a
 * 'Reset' command.  There is no interlocking because the setting and
 * clearing of these counters happens from the same process.
 */
void rem_purge_softerr (ring_info *ring)
{
    station_info *info, *next;

    /*
     * Clear non-isolating errors.
     */
    ring->noniso.lost_frame   = 0;
    ring->noniso.congestion   = 0;
    ring->noniso.frame_copied = 0;
    ring->noniso.frequency    = 0;
    ring->noniso.token        = 0;

    /*
     *  Clear isolating errors.
     */
    info = ring->station_errors;
    ring->station_errors = NULL;

    for ( ; info; ) {
	info->isolating.line       = 0;
	info->isolating.internal   = 0;
	info->isolating.burst      = 0;
	info->isolating.ac         = 0;
	info->isolating.abort      = 0;
	info->weight	           = 0;
	info->error_xmt	           = FALSE;

	info->congestion_count     = 0;
	TIMER_STOP(info->congestion_time);

	info->error_state	   = 0;

	next = info->nexterror;
	info->nexterror = NULL;

	info = next;
    }
}


void rem_input (
    major_vector *major,
    station_info *info,
    hwidbtype *idb)
{
    subvector_t *sv;
    ulong length;
    ring_info *ring;
    mac_data_ptrs results;
    int unit, dummy;
    uchar command;

    /*
     * Setup
     */
    memset(&results, 0, sizeof(mac_data_ptrs));
    ring = idb->ring_info;
    unit = idb->unit;

    /*
     * Parse the received frame.
     */
    length = major->length - MAC_MVID_LEN;
    command = major->command;
    lnmmacbuginf(remrcvd, unit, mvid_names[command], info->address);
    sv = (subvector_t *)&major->data;
    if (!parse_mac_subvectors(sv, length, command, &results, &dummy)) {
	lnmmacbuginf(remrcvbad, unit, mvid_names[command], info->address);
	return;
    }

    switch (command) {
      case MVID_NNINCOMP:
	ieee_copy(results.lastaddress, ring->last_nnin_addr);
	if (ring->notif_flags & ENA_NOT_FORWARD)
	    rem_forward_mac_frame(info, MVID_NNINCOMP, NULL, NULL);
	break;
	    
      case MVID_RPTMONERR:
	ieee_copy(results.naun, info->naun);
	info->phys_location = GETLONG(results.phys_loc);
	ring->last_monerr_code = GETSHORT(results.error);
	GET_TIMESTAMP(ring->last_monerr_time);
	ieee_copy(info->naun, ring->last_monerr_address);
	if (ring->notif_flags & ENA_NOT_FORWARD)
	    rem_forward_mac_frame(info, MVID_RPTMONERR, NULL, NULL);
	break;
	    
      case MVID_SOFTERR:
	ieee_copy(results.naun, info->naun);
	info->phys_location = GETLONG(results.phys_loc);
	rem_process_softerr(&results, info, ring);
	break;
	    
      default:
	break;
    }
}

void rem_init (ring_info *ring)
{
    ring->rem_state          = RS_NORMAL;

    ring->decrement          = REM_DECREMENT;
    ring->station_errors     = NULL;

    ring->notif_flags        = REM_NOTIF_DEF;
    ring->ring_inten_mask    = REM_RINGIN_DEF;
    ring->auto_inten_mask    = REM_AUTOIN_DEF;
    
    ring->congestion_thresh = REM_CONGEST_INIT;
}

/*
 * Background functions related to the Ring Error Monitor.  This function
 * returns TRUE if it needs to be run again at the next interval period,
 * i.e. there are still errored stations.
 */
boolean rem_background (ring_info *ring)
{
    station_info *info, *pred;
    idbtype      *swidb;

    /*
     * Find out what caused a beaconing condition.
     */
    if (ring->beaconing != BCN_NONE) {
	return(rem_beacon_postmortem(ring));
    }

    /*
     * For each station with errors.  Because we are using the nexterror
     * pointer which is at an offset of four instead of an offset of
     * zero, we must back up pred by four when we initially set it.
     * This is effectively setting the value of the info variable to
     * '(station_info *)&ring->num_entries', which is what we actually
     * want.
     */
    ring->pass--;
    pred = (station_info *)&ring->num_entries;
    info = pred->nexterror;
    while (info) {
	/*
	 * Is it still congested?  If not notify lan manager, but only it
	 * we originally notified that it was congested in the first
	 * place.
	 */
	if (!SLEEPING(info->congestion_time)) {
	    if (info->congestion_count > ring->congestion_thresh)
		rem_congestion_xxx(info, REM_RCVR_CNG_END);
	    info->congestion_count = 0;
	}

	if (ring->pass == 0) {
	    /*
	     * Decrement error counts.
	     */
	    info->isolating.line     = max(info->isolating.line - 1, 0);
	    info->isolating.internal = max(info->isolating.internal - 1, 0);
	    info->isolating.burst    = max(info->isolating.burst - 1, 0);
	    info->isolating.ac       = max(info->isolating.ac - 1, 0);
	    info->isolating.abort    = max(info->isolating.abort - 1, 0);
	    info->weight             = max(info->weight - 1, 0);

	    ring->noniso.lost_frame   = max(ring->noniso.lost_frame - 1, 0);
	    ring->noniso.congestion   = max(ring->noniso.congestion - 1, 0);
	    ring->noniso.frame_copied = max(ring->noniso.frame_copied - 1, 0);
	    ring->noniso.frequency    = max(ring->noniso.frequency - 1, 0);
	    ring->noniso.token        = max(ring->noniso.token - 1, 0);
	}

	/*
	 * Below the threshold?
	 */
	if (info->error_state & ERR_ME_WEIGHT) {
	    if (info->weight > REM_WEIGHT_DECREASING) {
		rem_xxx_notification(REM_WEIGHT_EXC, info);
	    } else if (info->weight == REM_WEIGHT_DECREASING) {
		info->decay_count++;
		rem_xxx_notification(REM_ERROR_DECAY, info);
	    } else {
		info->error_state &= ~(ERR_ME_WEIGHT);
            }
        } else if (info->error_state & ERR_ME_PREWEIGHT) {
            if (info->weight < REM_THRESH_PREWEIGHT) {
		info->isolating.line = 0;
		info->isolating.internal = 0;
		info->isolating.burst = 0;
		info->isolating.ac = 0;
		info->isolating.abort = 0;
		info->decay_count = 0;
		info->error_state &= ~(ERR_ME_PREWEIGHT);
	    }
	}

	/*
	 * Any errors left?
	 */
	if (!info->isolating.line      && 
	    !info->isolating.internal  &&
	    !info->isolating.burst     &&
	    !info->isolating.ac        &&
	    !info->isolating.abort     &&
	    !info->weight              &&
	    !info->congestion_count      ) {
            swidb = firstsw_or_null(info->idb);
	    lnmebuginf("\nIBMNM%d: Removing %e from error list", 
		 srbV.srb_sb_get_thisring(swidb), info->address);
	    info->error_state = 0;
	    pred->nexterror = info->nexterror;
	    info->nexterror = NULL;
	    info = pred;
	}
	
	/*
	 * Next
	 */
	pred = info;
	info = pred->nexterror;
    }

    if (ring->pass <= 0)
	ring->pass = REM_DECREMENT_INTERVAL / REM_WORK_INTERVAL;

   /*
     * Indicate if we need to be run again.
     */
    if (ring->station_errors)
	return(TRUE);
    return(FALSE);
}

/**********************************************************************
 *
 *                 Top Level Functions
 *
 **********************************************************************/

/*
 * lanmgr_BLOCK
 * Block lanmgr_background process if no work available.
 */

boolean lanmgr_BLOCK (void)
{
    if (lanmgrQ.qhead)
	return(FALSE);
    if (lnm_ctlQ.qhead)
	return(FALSE);
    if (ibmnmQ.qhead)
        return(FALSE);
    if (TIMER_RUNNING_AND_AWAKE(lanmgr_timer))
	return(FALSE);
    if (TIMER_RUNNING_AND_AWAKE(rem_timer))
	return(FALSE);
    return(TRUE);
}

/*
 * lanmgr_background
 *
 * Perform periodic processing.  This function returns TRUE if it needs
 * to be run again at the next interval period, i.e. there are still
 * work to do.
 */
boolean lanmgr_background (void)
{
    station_info *info, *pred, *error_pred;
    ring_info *ring;
    int bucket;
    boolean wakeup;
    leveltype level;

    wakeup = FALSE;

    if (!lanmgr_global_enable)
	return FALSE;

    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	if (!lanmgr_global_enable)
	    return FALSE;

	pred = (station_info *)&station_table[bucket];
	info = pred->next;
	while (info) {
	    if (!lanmgr_global_enable)
		return FALSE;

	    /*
	     * Kill this one if it is a temporary.
	     */
	    if (TIMER_RUNNING(info->temp)) {
		if (!SLEEPING(info->temp)) {
		    /*
		     * Check to see if this station structure is still in
		     * use anywhere.  It is easier to abort and catch it
		     * on the next pass than it is to try and unlink it
		     * from all possible places it might be in use.
		     */
		    ring = idb_to_ring(info->idb);
		    if (ring) {
			if ((ring->beacon_info == info) ||
			    (ring->beacon_naun == info))
			    goto ageloop;
			for (error_pred = ring->station_errors;
			     error_pred;
			     error_pred = error_pred->nexterror) {
			    if (error_pred == info)
				break;
			}
			if (error_pred)
			    goto ageloop;
		    }

		    /*
		     * Not in use anywhere.  Kill it.
		     */
		    level = raise_interrupt_level(NETS_DISABLE);
		    pred->next = info->next;
		    if (info->request_data)
			free(info->request_data);
		    info->next = info->nexterror = NULL;
		    free_crs_pak(info);
		    free_rps_pak(info);
		    info->request_data = NULL;
		    reset_interrupt_level(level);
		    free(info);
		    info = pred;
		    goto ageloop;
		} else {
		    wakeup = TRUE;
		}
    	    }

	    /*
	     * Resends?
	     */
	    if (TIMER_RUNNING(info->rps_time)) {
		if (!SLEEPING(info->rps_time))
		    resend_rps_pak(info);
		wakeup = TRUE;
	    }
	    if (TIMER_RUNNING(info->crs_time)) {
		if (!SLEEPING(info->crs_time))
		    resend_crs_pak(info);
		wakeup = TRUE;
	    }

ageloop:
	    pred = info;
	    info = pred->next;
	}
    }
    return(wakeup);
}

/*
 * lanmgr_input
 */
void lanmgr_input (void)
{
    paktype *pak;
    hwidbtype *idb;
    major_vector *major;
    station_info *info;
    tring_hdr *trh;
    uchar *dst_class_ptr;
    uchar d_class;
    uchar s_class;
    uchar i = 0;

    while ((i++ < MAX_MAC_PAK) && (pak = p_dequeue(&lanmgrQ))) {
         if (!lanmgr_global_enable) {
            datagram_done(pak);
            continue;
         }

	idb = pak->if_input->hwptr;

	 idb_increment_rx_proto_counts(pak, ACCT_PROTO_LANMAN);
	pak->acct_proto = ACCT_PROTO_LANMAN;

	info = newrold_station(pak);

	if (!info) {
	    datagram_done(pak);
	    continue;
	}
	trh = (tring_hdr *)pak->datagramstart;

	/*
	 * Parse the received frame.
	 */
	if (trh->saddr[0] & TR_RII)
	    major = (major_vector *)(trh->data + (trh->data[0] & RC_LTHMASK));
	else
	    major = (major_vector *)trh->data;
	if (!valid_mvid(major->command)) {
	    lnmmacbuginf("\nLANMGR%d: Received invalid MVID (%d) from %e.",
			 idb->unit, major->command, trh->saddr);
	    datagram_done(pak);
	    continue;
	}

        dst_class_ptr = (uchar *)major; 
        dst_class_ptr += sizeof(ushort);
        d_class = *dst_class_ptr >> 4; 
	switch (d_class) {
	  case CLASS_CRS:
	    if (idb->tr_ring_mode & RINGMODE_NETMGR)
                if (!RUNNING_IF(LINK_BRIDGE, idb->firstsw) ||
		   (idb->firstsw->span_ptr->protocol == SPAN_PROTO_IBM)) {
		    crs_input(major, info, idb);
                }
	    break;
	    
	  case CLASS_RPS:
	    if (idb->tr_ring_mode & RINGMODE_RPS)
		rps_input(major, info, idb);
	    break;
	    
	  case CLASS_REM:
	    if (idb->tr_ring_mode & RINGMODE_REM)
		rem_input(major, info, idb);
	    break;
	    
	  case CLASS_RINGSTATION:
            s_class = *dst_class_ptr & 0x0f;
	    if (s_class == CLASS_RINGSTATION)
		ringstation_input(major, info, idb);
	    else {
		if (!RUNNING_IF(LINK_BRIDGE, idb->firstsw)) {
		    vring_mac_adapter(major, info, idb, pak, trh->daddr);
                }
	    }
	    break;
		    
	  default:
	    lnmmacbuginf("\nLANMGR%d: Packet for unknown MAC class (%d).",
		     idb->unit, major->dst_class);
	    break;
	}
	datagram_done(pak);
    }
}

/*
 * lanmgr_process
 */
forktype lanmgr_process (void)
{
    ring_info *ring;
    boolean rem_wakeup;
    sys_timestamp wakeuptime;
    TCLSIMsg* clsi_msg;
    llctype *cookie;
    lnm_t *lnm_tmp;
    paktype *pak;
    ushort primitive;
    boolean suspend_flag = FALSE;

    lnm_tmp = NULL;

    edisms(systeminitBLOCK, 0);

    while (TRUE) {
	edisms((blockproc *)lanmgr_BLOCK, 0);

	/*
	 * Process any new frames.  This must come first so that the
	 * background task doesn't resend a frame that has already been
	 * answered, just not processed yet.
	 */
	lanmgr_input();
	ibmnm_leg_input();
	while ( TRUE ) {
	    clsi_msg = p_dequeue(&lnm_ctlQ);
	    if (clsi_msg == NULL) {
		break;
            }
	    lnm_tmp = lnm_pre_proc_clsi(clsi_msg);
	    if (lnm_tmp == NULL)
		break;
	    if (lnm_tmp->payload_ptr == NULL)
		break;
	    cookie = (llctype *) lnm_tmp->p_cep_id;
	    pak = CLSIMsgGetPak(clsi_msg);
	    lnm_tmp->clsi_msg = clsi_msg;
	    primitive = CLSIMsgGetPrimitiveCode(clsi_msg);
	    if (primitive == (CLS_DATA | CLS_INDICATION))
		cookie = (llctype *) lnm_tmp->u_cep_id;
	    ibmnm_input(cookie, pak, lnm_tmp);
	    suspend_flag = TRUE;
	    CLSIMsgDestroy(clsi_msg);
	}
	if (clsi_msg)
	    CLSIMsgDestroy(clsi_msg);
	/*
	 * Do retransmissions and other maintenance work.
	 */
	if (TIMER_RUNNING_AND_AWAKE(lanmgr_timer)) {
	    if (lanmgr_background()) {
		bump_lanmgr_timer();
	    } else {
		stop_lanmgr_timer();
	    }
	    wakeuptime = ibmnm_background();
	    if (TIMER_RUNNING(wakeuptime))
		set_lanmgr_timer(wakeuptime);
	}

	/*
	 * Ring Error Monitor events happen every ten seconds.
	 */
	if (TIMER_RUNNING_AND_AWAKE(rem_timer)) {
	    rem_wakeup = FALSE;
	    for (ring = ringQ.qhead; ring; ring = ring->next)
		rem_wakeup |= rem_background(ring);
	    if (rem_wakeup)
		bump_rem_timer();
	    else
		stop_rem_timer();
	}
	if (suspend_flag == TRUE)
	    process_may_suspend();
    }
}

/*
 *
 */
void lanmgr_destroy_ring (hwidbtype *idb)
{
    ring_info *ring;

    /*
     * Does the ring structure already exist?
     */
    ring = idb_to_ring(idb);
    if (!ring)
	return;

    /*
     * Yes, see if anyone else is using it.  If so, don't delete it.
     */
    idb->ring_info = NULL;
    FOR_ALL_HWIDBS(idb)
	if (idb->ring_info == ring)
	    return;

    unqueue(&ringQ, ring);
    free(ring);
}

/*
 *
 */
ring_info *lanmgr_create_ring (hwidbtype *idb)
{
    ring_info *ring;
    ushort srb_thisring = srbV.srb_sb_get_thisring(idb->firstsw);

    if (!lanmgr_global_enable)
	    return(NULL);;

    /*
     * Does the ring structure already exist?
     */
    ring = idb_to_ring(idb);
    if (ring) {
	if (ring->ringno == srb_thisring)
	    return(ring);
	else if ((ring->ringno == 0) && (srb_thisring != 0)) {
	    ring->ringno = srb_thisring;
	    return(ring);
	} else 
	    lanmgr_destroy_ring(idb);
    }

    /*
     * Nope, need to create a new one.
     */
    ring = malloc_named(sizeof(ring_info), lnm_cb);
    if (!ring) {
	return(NULL);
    }

    rs_init(ring);
    rps_init(ring);
    crs_init(ring);
    rem_init(ring);

    ring->ringno = srb_thisring;
    idb->ring_info = ring;
    enqueue(&ringQ, ring);
    return(ring);
}

void lanmgr_init (subsystype *subsys)
{
    hwidbtype *idb;
    int bucket;

    lanmgr_global_enable = TRUE;
    lanmgr_pathtrace_enable_lnm = TRUE;
    lanmgr_pathtrace_enable_orig = TRUE;
    crs_correlator = 1;
    rps_correlator = 1;
    for (bucket = 0; bucket < NETHASHLEN; bucket++) 
        station_table[bucket] = NULL;

    ibmnm_init();
    FOR_ALL_HWIDBS(idb) {
	if (!is_srb_hw(idb))
	    continue;
	(void)lanmgr_create_ring(idb);
    }
    stop_lanmgr_timer();			/* nothing to do yet */
    stop_rem_timer();				/* nothing to do yet */

    /*
     * Register some functions
     */
    reg_add_raw_enqueue(LINK_MAC, lanmgr_enqueue, "lanmgr_enqueue");
    reg_add_media_tring_ring_status(lanmgr_ringstat, "lanmgr_ringstat");
    reg_add_media_rps_startup(rps_startup, "rps_startup");

    /* Register lan manager functions */

    reg_add_set_ibmnm_interfaces( set_ibmnm_interfaces,
 				 "set_ibmnm_interfaces");
    reg_add_send_trace_report   ( send_trace_report,
				 "send_trace_report");
    reg_add_lbs_srte_bcast      ( lbs_srte_bcast,
				 "lbs_srte_bcast");
    reg_add_lanmgr_create_ring  (lanmgr_create_ring,
				 "lanmgr_create_ring");
    reg_add_lanmgr_destroy_ring ( lanmgr_destroy_ring,
 				 "lanmgr_destroy_ring");

    /*
     * Note that the parser will be initialized by lanmgr_chain.o if
     * it's been linked in.
     */
}

/*
 * lnm_enqueue
 */
static void lnm_enqueue (paktype *newpak)
{
    llctype             *llc;
    sap_hdr             *llc1;
    trrif_t *trh;
    snap_hdr *snap;
    sap_entry_t         *sap_entry;
    boolean             isResponse;
    idbtype             *swidb;
    hwidbtype           *idb; 
    int                 riflen = newpak->riflen;
    ClsDlcParamBlockT   clsBlock;    
    DlcUdataStnT        *dlcUdataStn;
    LlcUdataStnT        *llcUdataStn;
    paktype             *repak;

/*
 * This definition part of code to make RSRB work
 * with CLSI compliant LNM
 */
    tr_vgrouptype *vrg;

    swidb = (idbtype *)newpak->if_input;
    idb = (hwidbtype *)swidb->hwptr;
    /*
     * Code for continuing legacy RSRB support
     */
    vrg = vrfind_ringgroup(srbV.srb_sb_get_thisring(swidb));
    if (vrg) {
	if (idb->vidb_link) {
	    /*
	     * This is the RSRB Case
             */
	    /*  sap_entry = sap_get_entry(idb->firstsw, SAP_IBMNM); */
	    ibmnm_enqueue(newpak);
	    return;
	}
	else {
	    /*
	     * This is the NON-RSRB Case where the two ports on
	     * the LNM bridge are defined as ring-group members.
	     */
	    sap_entry = sap_get_entry(idb->firstsw, SAP_IBMNM);
	}	
    }	
    else {
        /* This is the local case where the two ports on the
	 * LNM bridge are defined as source-bridge X Y Z &
	 * source-bridge Z Y X on their respective interfaces
	 */
	sap_entry = sap_get_entry(idb->firstsw, SAP_IBMNM);  
    }
    if (!sap_entry) { 
	lnmllcbuginf("\nLANMGR: lnm_enqueue - sap entry %d is not valid", sap_entry);
        datagram_done(newpak);
	return;	
    }
    if (!sap_entry->usapid) {
	lnmllcbuginf("\nLANMGR: lnm enqueue - no usapid present in sap_entry %d", sap_entry);
	datagram_done(newpak);
	return;
    }	  
    llc1      = (sap_hdr *)   newpak->info_start;
    isResponse = (llc1->ssap & SAP_RESPONSE);
    trh = (trrif_t *) newpak->datagramstart;
    snap = (snap_hdr *)((uchar *)&trh->rc_blth + riflen);
    
    llc = (llctype *) LLcByPak(idb->firstsw, newpak);

    if ( ((snap->control & ~LLC1_P) == LLC1_UI) ) {
	hwaddrtype macAddr;
	if(llc) {
	    if (llc->u_cep_id == (void*) CLS_NO_CEP) {
		datagram_done(newpak);
		return;
	    }

	    clsBlock.fResultError    = CLS_OK;
	    clsBlock.fPakPtr         = newpak;
	    /* 
	     * Set network_start to point at start of udata 
	     */
	    newpak->network_start = newpak->info_start + SAP_HDRBYTES;
	    clsBlock.fLoadLength = newpak->datagramsize -(newpak->network_start - newpak->datagramstart);
	    reg_invoke_cls_rput(llc->u_cep_id, CLS_UDATA_IND, &clsBlock);
	}
	else {
            repak = pak_grow_realign(newpak, newpak->datagramsize,
                              (sizeof(UDataStnInd_t) + sizeof(long)));
	    if (repak == NULL) {
		 datagram_done(newpak);
		 return;
	    }

	    clsBlock.fResultError    = CLS_OK;
	    clsBlock.fPakPtr         = repak;
	    dlcUdataStn = &clsBlock.fParameters.fDlcUdataStn;
	    llcUdataStn = &dlcUdataStn->fDlcUdata.fLlcUdataStn;
	    
	    (*idb->extract_snpa)(repak, &macAddr, SOURCE);
 
	    llcUdataStn->fMacType = STATION_IEEE48;
	    llcUdataStn->fLMacLength = IEEEBYTES;
	    llcUdataStn->fLMacPtr = repak->addr_start;
	    llcUdataStn->fRMacLength = IEEEBYTES;
	    llcUdataStn->fRMacPtr = repak->addr_start + IEEEBYTES;
	    llcUdataStn->fRoutingLength = riflen;
	    llcUdataStn->fRoutingPtr = repak->rif_start;
	    
	    llc1 = (sap_hdr *) repak->info_start;
	    llcUdataStn->fRSap = llc1->ssap & ~SAP_RESPONSE;
	    llcUdataStn->fLSap = llc1->dsap;
	    
	    dlcUdataStn->fClsUSapId   = (ClsUSapIdT) sap_entry->usapid; 
	    dlcUdataStn->fUDataLength = repak->datagramsize -(repak->network_start - repak->datagramstart);
	    dlcUdataStn->fUDataPtr = repak->network_start;
	    reg_invoke_cls_rput((ClsUCepIdT) CLS_NO_CEP, CLS_UDATA_STN_IND, &clsBlock);
	}
	return;
    }
    else {
	reg_invoke_llc2_input(newpak);  /* with UCEP_ID */
	return;
    }
}


/**********************************************************************
 *
 *                      Configuration Support
 *
 **********************************************************************/

void lanmgr_start (void)
{
    union L_primitives lp;
    if (lanmgr_pid)
	return;

    reg_add_srt_set_srb_params(lnm_set_srb_params,
			       "lnm_set_srb_params");
    reg_add_raw_enqueue(LINK_IBMNM, lnm_enqueue, "lnm_enqueue");
    lanmgr_pid = cfork(lanmgr_process, 0L, 1500, "LanNetMgr Supt", 0);
    queue_init(&lnm_clsiQ, 0);
    /* Interface between LAN and LNM */
    lnm_keepalive_flag = TRUE;

    lp.type = I_START;
    init_to_ldisp(&lp);
}

void lnm_set_srb_params(hwidbtype *idb, boolean sense)
{
    CLSPortID port_id;

    port_id.type = CLS_P_TYPE_SWIDB;
    port_id.fTypeInfo.swidb = firstsw_or_null(idb);
    if (is_srb_hw(idb)) {
        if (sense)
	    lnm_enable_port(&port_id);
        else
	    lnm_disable_port(&port_id);
    }
}

void lnm_enable_port(CLSPortID *port_id)
{
    union L_primitives lp;
    tr_vgrouptype *vrg;
    lnm_clsi_t *lnm_clsi_tmp;
    hwidbtype *idb;
    word vdlc_corr;

    ushort port_number = CLSIPortIdGetPortNumber(port_id);
    ushort slot_number = CLSIPortIdGetSlotNumber(port_id);
    ushort port_type   = CLSIPortIdGetPortType(port_id);
    lnm_clsi_tmp = lnm_get_clsi(slot_number, port_number, port_type);
    vdlc_corr = random_gen();

    /* Enable for the traditional LNM local bridge port */
    if (!lnm_clsi_tmp) {
        lp.type = L_ENABLE_REQ;
        lp.ldisp_enable_req.port_type = CLSIPortIdGetPortType(port_id);
        lp.ldisp_enable_req.swidb     = CLSIPortIdGetSwIdb(port_id);
	lp.ldisp_enable_req.corr      = vdlc_corr;
        lfsm_to_ldisp(&lp);
    }
    else {
	if (lnm_clsi_tmp->enabled == FALSE) {
	    lp.type = L_ENABLE_REQ;
            lp.ldisp_enable_req.port_type = CLSIPortIdGetPortType(port_id);
            lp.ldisp_enable_req.swidb     = CLSIPortIdGetSwIdb(port_id);
	    lp.ldisp_enable_req.corr      = vdlc_corr;
            lfsm_to_ldisp(&lp);
	}
    }
    idb = ((CLSIPortIdGetSwIdb(port_id))->hwptr);
    vrg = vrfind_ringgroup(srbV.srb_sb_get_targetring(idb->firstsw));
    if (!vrg) {
	return;
    }
    else {
        /* Enable for the LNM for DLSw VDLC port */
	lnm_clsi_tmp = lnm_get_clsi_swidb(idb->firstsw);
        if (!lnm_clsi_tmp) {
	    lp.type = L_ENABLE_REQ;
	    lp.ldisp_enable_req.port_type = CLS_VDLC;
	    lp.ldisp_enable_req.hwidb = idb;
	    lp.ldisp_enable_req.swidb = idb->firstsw;
	    lfsm_to_ldisp(&lp);
        }
	else {
	    if (lnm_clsi_tmp->enabled == FALSE) {
	        lp.type = L_ENABLE_REQ;
	        lp.ldisp_enable_req.port_type = CLS_VDLC;
	        lp.ldisp_enable_req.hwidb = idb;
	        lp.ldisp_enable_req.swidb = idb->firstsw;
	        lfsm_to_ldisp(&lp);
            }
        }  
    }
}

void lnm_disable_port(CLSPortID *port_id)
{
    union L_primitives lp;
    lnm_clsi_t *lnm_clsi_tmp;
    tr_vgrouptype *vrg;
    word correlator = 0;
    
    ushort port_number = CLSIPortIdGetPortNumber(port_id);
    ushort slot_number = CLSIPortIdGetSlotNumber(port_id);
    ushort port_type   = CLSIPortIdGetPortType(port_id);

    lnm_clsi_tmp = lnm_get_clsi(slot_number, port_number, port_type);
    vrg = vrfind_ringgroup(lnm_clsi_tmp->ring_group);
    if (lnm_clsi_tmp->enabled) {
	if (lnm_clsi_tmp->sap_enabled) {
	    correlator = lnm_clsi_tmp->correlator;
	    lp.type = L_DEACTIVATESAP_REQ;
            lp.ldisp_deactivatesap_req.sap_value = SAP_IBMNM;
            lp.ldisp_deactivatesap_req.u_sap_id = (uint)lnm_clsi_tmp;
            lp.ldisp_deactivatesap_req.p_sap_id = lnm_clsi_tmp->p_sap_id;
	    lp.ldisp_deactivatesap_req.swidb    = lnm_clsi_tmp->swidb;
            lfsm_to_ldisp(&lp);
	}
    }
    if (!vrg) {
	return;
    }
    else {
        /* Deactivate Sap for the CLS VDLC port */
	lnm_clsi_tmp = lnm_get_clsi_vdlc(correlator);
	if (!lnm_clsi_tmp)
	    return;
	if (lnm_clsi_tmp->enabled) {
	    if (lnm_clsi_tmp->sap_enabled) {
	        lp.type = L_DEACTIVATESAP_REQ;
	        lp.ldisp_deactivatesap_req.u_sap_id = (uint)lnm_clsi_tmp;
	        lp.ldisp_deactivatesap_req.p_sap_id = lnm_clsi_tmp->p_sap_id;
	        lp.ldisp_deactivatesap_req.swidb        = lnm_clsi_tmp->swidb;
	        lp.ldisp_deactivatesap_req.sap_value    = SAP_IBMNM;
	        lfsm_to_ldisp(&lp);
	    }
	}

    }	
}

lnm_clsi_t *lnm_get_clsi(hword slot_number, hword port_number, hword port_type)
{
  lnm_clsi_t *lnm_clsi_tmp;

  for ( lnm_clsi_tmp = (lnm_clsi_t *)lnm_clsiQ.qhead; 
	lnm_clsi_tmp;
	lnm_clsi_tmp = lnm_clsi_tmp->next ) {
        if ( (lnm_clsi_tmp->slot_number == slot_number) &&
	     (lnm_clsi_tmp->port_number == port_number) &&
	     (lnm_clsi_tmp->port_type   == port_type) )
	   return lnm_clsi_tmp;
  }
  return NULL;
}

lnm_clsi_t *lnm_get_clsi_swidb(idbtype *swidb)
{
    lnm_clsi_t *lnm_clsi_tmp;
    for ( lnm_clsi_tmp = (lnm_clsi_t *)lnm_clsiQ.qhead; 
	lnm_clsi_tmp;
	lnm_clsi_tmp = lnm_clsi_tmp->next ) {
        if (lnm_clsi_tmp->swidb == swidb)
	    return lnm_clsi_tmp;
    }
    return NULL;
}

lnm_clsi_t *lnm_get_clsi_vdlc(word corr)
{
    lnm_clsi_t *lnm_clsi_tmp;
    for ( lnm_clsi_tmp = (lnm_clsi_t *)lnm_clsiQ.qhead; 
	lnm_clsi_tmp;
	lnm_clsi_tmp = lnm_clsi_tmp->next ) {
        if ((!lnm_clsi_tmp->vswidb) && (corr == lnm_clsi_tmp->correlator))
	    return lnm_clsi_tmp;
    }
    return NULL;
}

void lnm_clsi_cleanup(TCLSIMsg *clsi_msg)
{
  lnm_clsi_t 	*lnm_clsi_tmp, *lnm_vdlc_tmp;
  word          correlator;
  ushort 	port_number, slot_number, sub_number, port_type;
  CLSPortID     port_id;
  hwidbtype     *hw_idb;
  idbtype	*sw_idb;
  char   *str;

  str = string_getnext();
	
  port_id   = ((DisableCnf_t *)clsi_msg)->fPortId;

  slot_number = CLSIPortIdGetSlotNumber(&port_id);
  port_number = CLSIPortIdGetPortNumber(&port_id);
  sub_number  = CLSIPortIdGetSubNumber(&port_id);
  port_type   = CLSIPortIdGetPortType(&port_id);

  sw_idb = CLSIPortIdGetSwIdb(&port_id);
  if (sw_idb == NULL ) {
      errmsg(LNMC_ERR_BADCLSI, 
          CLSIMsgSprintfShort(clsi_msg, str),
	  "Sending Activate Sap", "sw_idb is NULL"); 
      return;
  }
  hw_idb = sw_idb->hwptr;
  if (hw_idb == NULL ) {
      errmsg(LNMC_ERR_BADCLSI, 
          CLSIMsgSprintfShort(clsi_msg, str),
	  "lnm_clsi_cleanup", "hw_idb is NULL ");
      return;
  }
  lnm_clsi_tmp = lnm_get_clsi(slot_number, port_number, port_type);
  if (!lnm_clsi_tmp) {
      lnm_clsi_tmp = lnm_get_clsi_swidb(sw_idb);
  }	
  if (!lnm_clsi_tmp)
      return;
  correlator = lnm_clsi_tmp->correlator;
  lnm_vdlc_tmp = (lnm_clsi_t *)lnm_get_clsi_vdlc(correlator);
  if (lnm_vdlc_tmp) {
      unqueue(&lnm_clsiQ, lnm_vdlc_tmp);
      free(lnm_vdlc_tmp);
  }
  unqueue(&lnm_clsiQ, lnm_clsi_tmp);
  free(lnm_clsi_tmp);
}

lnm_t *lnm_pre_proc_clsi(TCLSIMsg* clsi_msg)
{
    char 		*str;
    lnm_clsi_t         *lnm_clsi_tmp;
    lnm_t              *lnm_ptr = NULL;
    str = string_getnext();
    lnm_clsi_tmp = NULL;
    
    if (!lnm_is_bad_clsi(clsi_msg)) {
	lnm_clsi_tmp = lnm_usapid2dgra(clsi_msg->fIdentifier);
	lnm_ptr = lnm_proc_clsi(clsi_msg, lnm_clsi_tmp); 
    }
    return(lnm_ptr);
}

lnm_t *lnm_proc_clsi(TCLSIMsg* clsi_msg, lnm_clsi_t *port_id)
{
    ushort 		primitive = CLSIMsgGetPrimitiveCode(clsi_msg);
    lnm_dlc_id_t 	hash_dlc_id;
    lnm_t		*lnm_ptr = NULL;
    union L_primitives 	lp;
    ReqOpnStnCnf_t      *reqOpnCnf;
    UDataStnInd_t       *udata_stn;
    UDataInd_t 		*udata_ind;
    DataInd_t           *data_ind;    
    ulong               clsi_cnf_ret_code;
    char 		*str;
    uchar           	rif[SRB_MAXRIF];

    str = string_getnext();
    if (lnm_is_init_clsi_op(primitive))  {
	lnm_init_clsi_op_proc(clsi_msg);
	return(lnm_ptr);
    } 
    if ((lnm_ptr == NULL ) && 
	(lnm_is_context_valid(clsi_msg)))
	/* try from the message */
	lnm_ptr = ucepid2lnm(clsi_msg->fIdentifier);
    if (lnm_ptr == NULL ) {   
 	lnm_get_rif_from_clsi(clsi_msg, rif); 
 	rif[1] ^= RC_D; /* make the direction forward */
	rif[0] &= ~RC_RTMASK; /* clear broadcast bits */

	lnm_ptr = lnm_add_entry(&hash_dlc_id, port_id, rif);
	if (lnm_ptr == NULL ) {
	    lnmllcbuginf("\nLANMGR - proc_clsi unable to add lnm_entry.");     
	    return(lnm_ptr);
	}
	lnm_ptr->my_ckt = TRUE;	
    }
   
    /* decode and process incoming messages */

    if ( (primitive & CLS_TYPE_MASK) == CLS_INDICATION ){
	lnm_ptr->payload_len = 0;
	switch(primitive)
	    {
	    case CLS_TEST_STN | CLS_INDICATION :         
	    case CLS_TEST | CLS_INDICATION:
	    case CLS_ID_STN | CLS_INDICATION:
	    case CLS_ID | CLS_INDICATION:
	    case CLS_CONNECT_STN | CLS_INDICATION:
	    case CLS_CONNECT | CLS_INDICATION:
	    case CLS_CONNECTED | CLS_INDICATION:
		lnmllcbuginf("\nLANMGR: Invalid CLSI Indication Received on lnm_ctlQ %s ", CLSIMsgSprintfShort(clsi_msg, str));
		lnm_ptr->payload_len = 0;
		lnm_ptr->payload_ptr = NULL;
		break;
	    case CLS_UDATA_STN | CLS_INDICATION :              
		if (lnm_ptr->conn_pend == TRUE) {
		     unqueue(&lnm_clsiQ, lnm_ptr);
                     free(lnm_ptr);
	             lnmllcbuginf("\nLANMGR: CLS UDataStnInd for Set Reporting Point received in conn_pend state.");
		     lnm_ptr = NULL;
		     return(lnm_ptr);
		}
		udata_stn = (UDataStnInd_t *)clsi_msg;
		lnm_ptr->payload_len = udata_stn->fUDataLength;
		if (lnm_ptr->payload_len > 0 )
		    lnm_ptr->payload_ptr = (uchar *) clsi_msg + 
			((UDataStnInd_t *)clsi_msg)->fUDataOffset;
		else
		    lnm_ptr->payload_ptr = NULL;
		break;
	    case CLS_DISCONNECT | CLS_INDICATION :        
                /*
                 * We may get disc.ind after sending close_stn.req
                 * but before receiving the close_stn.cfm.
                 */
		lp.type = L_DISCONNECT_RSP;
		lp.ldisp_disconnect_rsp.pCepId = lnm_ptr->p_cep_id;
		lfsm_to_ldisp(&lp);		
		lnm_ptr->payload_len = 0;
		lnm_ptr->payload_ptr = NULL;			    
		lnm_proc_disc(lnm_ptr);
		break;
	    case CLS_UDATA | CLS_INDICATION :
	        udata_ind = (UDataInd_t *) clsi_msg;
		lnm_ptr->payload_len = udata_ind->fUDataLength;
		if (lnm_ptr->payload_len > 0 ) 
		  lnm_ptr->payload_ptr = udata_ind->fUDataPtr;
		else
		  lnm_ptr->payload_ptr = NULL;
		break;
	    case CLS_DATA | CLS_INDICATION :              
	        data_ind = (DataInd_t *) clsi_msg;
		lnm_ptr->payload_len = data_ind->fDataLength;
		if (lnm_ptr->payload_len > 0 ) 
		  lnm_ptr->payload_ptr = data_ind->fData +
		                         data_ind->fReserveLength;
		else
		  lnm_ptr->payload_ptr = NULL;
		break;
	      default :
	        lnmllcbuginf("\nLANMGR: Unrecognized CLSI Indication Primitive
Received on lnm_ctlQ.");      
		break;
	      }
    }
    else if ( (primitive & CLS_TYPE_MASK) == CLS_CONFIRM) {
	/*  
	 * all the confirms 
	 */
	lnm_ptr->payload_len = 0;
	clsi_cnf_ret_code = CLSIMsgGetReturnCode(clsi_msg);
	/* 
	 * Process each error code
	 */
	if (clsi_cnf_ret_code != CLS_OK) { 
	    if (primitive == (CLS_REQ_OPNSTN | CLS_CONFIRM) ) {
		/* 
		 * Any error code for ROS is serious enough 
		 * to close down the circuit.
		 * Usually it is a case of CLS_NO_REM_STN | CLS_INTERFACE_DOWN
		 */
		lnm_proc_openreq_cnf(lnm_ptr, FALSE);
		lnm_delete_entry(lnm_ptr);
		lnm_ptr = NULL;
		return(lnm_ptr);
	    } 
            if ((primitive == (CLS_CONNECT | CLS_CONFIRM)) &&
                (clsi_cnf_ret_code == CLS_TIMEOUT)) {
                /*
                * CLS keeps LLC alive if Connect.Req failed due to timeout
                * send Close.Req to destroy llc structure.
                */
                lnm_proc_openreq_cnf(lnm_ptr, FALSE);
                lnm_send_close_stn(lnm_ptr);
                return(lnm_ptr);
            }
	    switch (clsi_cnf_ret_code) {
	    case CLS_UNKNOWN_CEP :
		/* 
		 * CLSI decided to clean up the CEP
		 * for some reason. accept the error code.
		 */
		if (primitive == (CLS_CLOSE_STN | CLS_CONFIRM) )
		    break;
		else {
		   /* Treat the same as a bad open req confirmation
                    * and delete the entry
                    */
		    lnm_proc_openreq_cnf(lnm_ptr, FALSE);
		    lnm_delete_entry(lnm_ptr);
		    lnm_ptr = NULL;
		    return(lnm_ptr);
		}
		break;
	    case CLS_DISC_ERROR :
	    case CLS_TIMEOUT :
		/*
		 * Probably the station is not 
		 * connected to the media anymore. 
		 * treat like DLC_OK
		 */
		lnm_proc_openreq_cnf(lnm_ptr, FALSE);
		lnm_delete_entry(lnm_ptr);
		lnm_ptr = NULL;		
		return(lnm_ptr);
		break;
	    case CLS_NO_XID_RSP :
		lnm_ptr = NULL;
		return(lnm_ptr);
		break;
            default :
		/*
		 * Ideally this default never should get executed
		 * if we could handle all the error conditions.
		 */
		break;
	    } 
	}
	switch(primitive)
	    {
	    case CLS_REQ_OPNSTN | CLS_CONFIRM :
 	        reqOpnCnf = (ReqOpnStnCnf_t*) clsi_msg;
	        lnm_ptr->p_cep_id = reqOpnCnf->fCLSIHdr.fPCepId;
		lnm_proc_openreq_cnf(lnm_ptr, TRUE);
		break;
	    case CLS_CLOSE_STN | CLS_CONFIRM :
		lnm_delete_entry(lnm_ptr);
		lnm_ptr = NULL;
		break; 
	    case CLS_CONNECT | CLS_CONFIRM :           
		lnm_ptr->clsi_state = lnmst_connected;
		lnm_ptr->clsi_msg = clsi_msg;
		lnm_send_signalstn_req(lnm_ptr);
		ibmnm_lrm_conn_estab(lnm_ptr);
		lnm_ptr->conn_pend = FALSE;
		break;
	    case CLS_CONNECTED | CLS_CONFIRM :         
		lnm_ptr->clsi_state = lnmst_connected;
		break;
	    case CLS_DISCONNECT | CLS_CONFIRM :        
		lnm_ptr->clsi_state = lnmst_disconnect;
		break;
	    case CLS_FLOW | CLS_CONFIRM :              
		lnmllcbuginf("\n LNM-FLOW_CONTROL : Received ctlQ %s ",
		       CLSIMsgSprintfShort(clsi_msg, str));
		break;
	    case CLS_TEST_STN | CLS_CONFIRM :
	    case CLS_TEST | CLS_CONFIRM:
	    case CLS_ID | CLS_CONFIRM:
	    case CLS_ID_STN | CLS_CONFIRM :
	    case CLS_UDATA | CLS_CONFIRM :
	    case CLS_DATA | CLS_CONFIRM :
	    case CLS_OPEN_STN | CLS_CONFIRM :
	    case CLS_SIGNAL_STN | CLS_CONFIRM :
		lnmllcbuginf("\nLANMGR: Invalid CLSI Confirmation Primitive Received on lnm_ctlQ %s ",CLSIMsgSprintfShort(clsi_msg, str));
		lnm_ptr->payload_len = 0;
		lnm_ptr->payload_ptr = NULL;
		break;
	    default :
		lnmllcbuginf("\nLANMGR: Unrecognized CLSI Confirmation Primitive Received on lnm_ctlQ");
		lnm_ptr->payload_len = 0;
		lnm_ptr->payload_ptr = NULL;
		break;
	    }
            return(lnm_ptr);
    }
    return(lnm_ptr);
}

static lnm_t *id2lnm(uint id)
{
   int n;
   lnm_t *lnm_ptr;
   
   if (!validmem((lnm_t *)id)) {  /* Its really a magic cookie */
       return NULL;
   }
   
   if ((lnm_t *)id == ((lnm_t *)id)->id)
     return (lnm_t *)id;

   for ( n = 0 ; n < NLNM_HASH_SIZE; n++) {
       lnm_ptr = lnm_hash_table[n];
       while (lnm_ptr) {
	   if ( lnm_ptr == (lnm_t *)id )
	       return(lnm_ptr);
	   lnm_ptr = lnm_ptr->next;
       }
   }

   return(NULL);
}



lnm_t *ucepid2lnm(uint u_cep_id)
{
    lnm_t *lnm_ptr;
    
    lnm_ptr = id2lnm(u_cep_id);
    if ((lnm_ptr) && ( lnm_ptr->u_cep_id == u_cep_id ))
	return(lnm_ptr);
    return(NULL);
}

lnm_clsi_t *lnm_usapid2dgra(uint u_sap_id)
{
  lnm_clsi_t *lnm_clsi_tmp;

  for (lnm_clsi_tmp = (lnm_clsi_t *)lnm_clsiQ.qhead;
       lnm_clsi_tmp;
       lnm_clsi_tmp = lnm_clsi_tmp->next)  {
    if (lnm_clsi_tmp == (lnm_clsi_t *) u_sap_id) 
      return lnm_clsi_tmp;
  }
  return(NULL);
}

/*
 * Get the pointer to the rif from clsi message
 * Change this later to use the access functions provided by clsi
 */
void lnm_get_rif_from_clsi (TCLSIMsg *clsi_msg, uchar *new_rif)
{

    ushort       msg_type;
    ushort       ring_group;
    ushort       ring_number;
    ushort       bridge_number;
    srbroute_t   *bridged_rif;
    uchar	 *rif;
    uchar       temp_rif[SRB_MAXRIF];
    srbrd_t 	*rd;
    uchar	ii;
    uchar       rif_len;
    lnm_clsi_t  *lnm_clsi_tmp;
    long        port_mtu_size;
    uchar       rc_code;

    msg_type = CLSIMsgGetPrimitiveCode(clsi_msg);
    switch (msg_type) {
      case (CLS_TEST_STN | CLS_INDICATION):     /* Test Frame      */
	rif_len = ((TestStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((TestStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      case (CLS_ID_STN | CLS_INDICATION):       /* Contextless XID */
	rif_len = ((IdStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((IdStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      case (CLS_UDATA_STN | CLS_INDICATION):    /* UIDATA          */
	rif_len = ((UDataStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((UDataStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      case (CLS_CONNECT_STN | CLS_INDICATION):  /* Contextless SABME */
	rif_len = ((ConnectStnInd_t *)clsi_msg)->fRoutingLength;
	if (rif_len != 0 ) 
	    rif = (uchar *) clsi_msg + 
		((ConnectStnInd_t *)clsi_msg)->fRoutingOffset;
	else
	    rif = NULL;
	break;

      default:
	lnmllcbuginf("\nLANMGR: lnm_get_rif_from_clsi - Invalid CLSI primitive type %x", msg_type);
	rif_len = 0;
	rif = NULL;
	break;
    }

    /*
     * The last hop bridge number does not get initialized to 0,
     * when make_forward rif is called.
     * So set the new rif to zeros, here.
     */
    memset(new_rif, 0, SRB_MAXRIF);
    if ( (rif_len == 0 ) || (rif == NULL)) {
	return;
    }
    else
	bcopy((uchar *)rif, (uchar *)temp_rif, SRB_MAXRIF);

    bridged_rif = (srbroute_t *)temp_rif;

    if (RC_BROADBIT & rif[0]) {
        /* If only it is broad cast */
        lnm_clsi_tmp =    lnm_usapid2dgra(clsi_msg->fIdentifier);

        ring_group = lnm_clsi_tmp->ring_group;
        bridge_number = lnm_clsi_tmp->bridge_number;
        ring_number = lnm_clsi_tmp->ring_number;
        port_mtu_size = lnm_clsi_tmp->mtu_size;
	if (lnm_clsi_tmp->mtu_size > 0)
	    port_mtu_size = lnm_clsi_tmp->mtu_size;
	else
	    port_mtu_size = RC_LF17800_VAL;
      
        ii = ((RIF_LENGTH(rif[0])-2)/2);

        if (RIF_LENGTH(rif[0]) == 2 ) {
	    /* virgin rif - add ring-bridge-ring-bridge(0) 4 bytes */
            rd = (srbrd_t *) (&(bridged_rif->rd[ii]));
            PUTSHORT(rd, (((ring_number << 4) & 0xfff0) | (bridge_number & 0x000f)));
            PUTSHORT(++rd, ((ring_group << 4) & 0xfff0));
            bridged_rif->rc_blth = ((bridged_rif->rc_blth & RC_RTMASK) | 6);
        } else {
	    /* rif already exists - add bridge-ring 2 bytes */
            ii = RIF_LENGTH(rif[0]) - 2;
            rd = (srbrd_t *) &temp_rif[ii];
            PUTSHORT(rd, ((GETSHORT(rd) & 0xfff0) | (bridge_number & 0x000f)));
            PUTSHORT(++rd, ((ring_group << 4) & 0xfff0));
            bridged_rif->rc_blth =  ((bridged_rif->rc_blth & RC_RTMASK) | 
  				 ((RIF_LENGTH(rif[0]) + 2) & RC_LTHMASK) );
        }
        /* now, fix the lf size */
        if (((temp_rif[1] & RC_LFMASK) == RC_LFINITIAL) || 
               ((rc_code_to_bytes(temp_rif[1] & RC_LFMASK)) > port_mtu_size)) { 
            rc_code = bytes_to_rc_code(port_mtu_size);
            temp_rif[1] = (temp_rif[1] & ~RC_LFMASK) | rc_code;
        }
    }

    make_forward_rif ((srbroute_t *) temp_rif, (srbroute_t *) new_rif);
    return;
}

void lnm_proc_disc(lnm_t *lnm_ptr)
{
    llctype *lrm_cookie;
    
    lrm_cookie = (llctype *)lnm_ptr->u_cep_id;
    if (lrm_cookie != NULL) {
        ibmnm_link_state_change(lrm_cookie, LC_DISC_LAN);
    }
    lnm_send_close_stn(lnm_ptr);
    return;
}

void lnm_proc_openreq_cnf(lnm_t *lnm_ptr, boolean status)
{
    if (status == TRUE)
	lnm_send_connect_req(lnm_ptr);
    else
	ibmnm_lrm_conn_fail(lnm_ptr);  
    return;
}


/*
 * To compute an hash value < 255 so we could slide this entry
 * into aan hash table.
 */

static uint calc_lnm_hash (uchar *string)
{
   uint i, n = 0;
   /* We are creating the circuits even before the saps are nailed down */
   for (i = 0; i < ((sizeof(lnm_dlc_id_t))); i++)
      n += *(string + i);
   n &= NLNM_HASH_MASK;
   return (n);
}

/*
 * lnm _ a d d _ e n t r y
 *
 * Create a new hashtable entry, filling in all needed parameters,
 * borrow an entry from the free list 
 * and then placing it into the hash table
 */

lnm_t *lnm_add_entry (lnm_dlc_id_t *lnm_hash_dlc_id, lnm_clsi_t *port_id, 
	                                                     uchar *rif )
{
    lnm_t	*lnm_ptr;
    uint	n;
    uint        rif_len;

    lnm_ptr = malloc_named(sizeof(lnm_t), "LNM-CIRCUIT-DYNAMIC");
    if (!lnm_ptr)
        return(NULL);

    lnm_init_fields(lnm_ptr);
    bcopy((uchar *)lnm_hash_dlc_id, (uchar *)&(lnm_ptr->dlc_id), 
	  sizeof(lnm_dlc_id_t));
    n				= calc_lnm_hash((uchar *)(&(lnm_ptr->dlc_id)));
    lnm_ptr->next 		= lnm_hash_table[n];
    lnm_hash_table[n] 		= lnm_ptr;
    /*
     * Create an index so a user could refer to it while
     * deleting an entry???
     * Assume a uniform hash distribution and a 
     * max of 256 entries for a hash hit
     */
    lnm_ptr->lnm_index.entry.hash = n;
    if (lnm_ptr->next == NULL ) 
      lnm_ptr->lnm_index.entry.list = 0;
    else
      lnm_ptr->lnm_index.entry.list = 
	lnm_ptr->next->lnm_index.entry.list++;

    lnm_ptr->lnm_port_id_dgra = port_id; 
    /*
     * This is done so the remote partner gets our slot/port/sub/type
     * info and could print on his console. Should help to identify
     * the sw_idbs of both ends. xchg of sw_idb info happens thru ssp.
     */
    lnm_ptr->our_port_id = 	(uchar)port_id->slot_number | 
    				(uchar)port_id->port_number | 
    				(uchar)port_id->sub_number | 
    				(uchar)port_id->port_type;

    rif_len = RIF_LENGTH(rif[0]);

    if ( (rif) && (rif_len <= SRB_MAXRIF) ){
	bcopy(rif, lnm_ptr->local_rif, rif_len);
	memset(lnm_ptr->local_rif+rif_len, 0, (SRB_MAXRIF - rif_len));
    }
    else
	memset(lnm_ptr->local_rif, 0, SRB_MAXRIF);
    return(lnm_ptr);
}

void lnm_init_fields(lnm_t *lnm_ptr)
{
    lnm_ptr->id = lnm_ptr;

    lnm_ptr->lnm_state	        = lnmst_disconnect;
    lnm_ptr->clsi_state 	= clnmst_disconnect;
    lnm_ptr->lnm_port_id_dgra   = NULL;
    lnm_ptr->next             	= (struct _lnm_hash *)NULL;
    lnm_ptr->pak        	= (paktype *)NULL;
    lnm_ptr->u_cep_id           = (int)lnm_ptr; 
    lnm_ptr->p_cep_id           = 0;
    
    lnm_ptr->payload_len        = 0;
    lnm_ptr->fc_llc_flowlevel   |= DLC_CONGESTION_LOW;
}

void lnm_delete_entry (lnm_t *lnm_ptr)
{
   int n;
   lnm_t *curr, *prev;
   leveltype status;

   curr = NULL;
   n = calc_lnm_hash((uchar *)&(lnm_ptr->dlc_id));
   curr = lnm_hash_table[n];
   prev = NULL;

   while (curr) {
      if (curr == lnm_ptr) {
          lnm_ptr->id = NULL;
          status = raise_interrupt_level(NETS_DISABLE);
          if (prev)
              prev->next = curr->next;
          else
              lnm_hash_table[n] = curr->next;
          free(lnm_ptr);
          reset_interrupt_level(status);
          return;
      }
      prev = curr;
      curr = curr->next;
   }
}


/*
 * lnm  _ i d
 *
 * return a string describing this lnm reporting link.
 */

char *lnm_id (lnm_t *lnm_ptr, char *str)
{
    if (!lnm_ptr)
	sprintf(str, "bogus");
    else
	sprintf(str, "(%e %02x)=>(%e %02x)", 
		&lnm_ptr->dlc_id.server_mac_address,
		lnm_ptr->dlc_id.server_link_sap, 
		&lnm_ptr->dlc_id.client_mac_address, 
		lnm_ptr->dlc_id.client_link_sap);
    return(str);
}


/*
 * Validates the following fields of the CLSI message.
 * 1. iftype
 * 2. type mask
 * 3. valid primitive for lnm
 * 4. data size for primitives containing end-station payload
 */

boolean lnm_is_bad_clsi(TCLSIMsg *clsi_msg)
{
    int 	i;
    boolean 	error_code = FALSE;
    ushort     	primitive = CLSIMsgGetPrimitiveCode(clsi_msg);
    char 	*str;

    str = string_getnext();


    if (clsi_msg->fHeaderLength < sizeof(TCLSIMsg))  {
	errmsg(LNMC_ERR_BADCLSIHDRLEN,  CLSIMsgSprintfShort(clsi_msg, str),
	       clsi_msg->fHeaderLength,
	       "is_bad_clsi_op");
    	error_code = TRUE;
    }
    switch (clsi_msg->fIdentifierType) {
    case CLS_U_CEP_ID:
    case CLS_U_SAP_ID:
    case CLS_ENTITY_MGR:
	break;
    default:
	errmsg(LNMC_ERR_BADCLSIIDTYPE, 
	       CLSIMsgSprintfShort(clsi_msg, str), 
	       clsi_msg->fIdentifierType,
	       "is_bad_clsi_op");
    	error_code = TRUE;
	break;
    }
    switch ( (primitive & CLS_TYPE_MASK)) {
    case CLS_INDICATION:
    case CLS_CONFIRM :
	break;
    default:
	errmsg(LNMC_ERR_BADCLSIPRIMTYPE,  CLSIMsgSprintfShort(clsi_msg, str),
	       (primitive & CLS_TYPE_MASK),
	       "is_bad_clsi_op");
    	error_code = TRUE;
	break;
    }
    for ( i=0; i < NLNM_BAD_CLSI_OP; i++) {
	if (primitive == lnm_bad_clsi_op_tbl[i]) {
	    errmsg(LNMC_ERR_BADCLSI, 
		   CLSIMsgSprintfShort(clsi_msg, str),
		   "is_bad_clsi_op", "");
	    error_code = TRUE;
	}
    }
    for ( i=0; i < NLNM_DATAREQ_CLSI_OP; i++) {
	if (primitive == lnm_data_req_clsi_op_tbl[i]) {
	    if (clsi_msg->fDataSize == 0 ) {
		errmsg(LNMC_ERR_BADCLSIDATALEN,
		       CLSIMsgSprintfShort(clsi_msg, str),
		       clsi_msg->fDataSize,
		       "is_bad_clsi_op");
		error_code = TRUE;
		break;
	    }
	}
    }
    if (lnm_is_context_valid(clsi_msg))
	error_code = FALSE;
    else
	error_code = TRUE;
    return(error_code);
}


boolean lnm_is_context_valid(TCLSIMsg *clsi_msg)
{
    ushort primitive;
    lnm_t *lnm_ptr;
    int i;
    char 	*str;

    str = string_getnext();

    primitive = CLSIMsgGetPrimitiveCode(clsi_msg);

    for ( i=0; i < NLNM_CONTEXTREQ_CLSI_OP; i++) {
	if (primitive == lnm_ctxt_req_clsi_op_tbl[i]) {
	    if ((clsi_msg->fIdentifierType != CLS_U_CEP_ID) ||
		(clsi_msg->fIdentifier == CLS_NO_CEP) ) {
		errmsg(LNMC_ERR_BADCLSI,
		       CLSIMsgSprintfShort(clsi_msg, str),
		       "bad identifier type; without context", "");
		return FALSE;
		break;
	    }
	    lnm_ptr = ucepid2lnm(clsi_msg->fIdentifier);
	    if (lnm_ptr == NULL ) {
		errmsg(LNMC_ERR_BADCLSI, 
		       CLSIMsgSprintfShort(clsi_msg, str),
		       "without context", "");
		return FALSE;
		break;
	    }
	}
    }
    return TRUE;
}

void lnm_send_info_to_disp(lnm_t *lnm_ptr, paktype *pak)
{
    union L_primitives lp;
    lp.type = L_DATA_REQ;
    lp.ldisp_data_req.pCepId = lnm_ptr->p_cep_id;
    lp.ldisp_data_req.theData = pak->datagramstart;
    lp.ldisp_data_req.dataLength = pak->datagramsize;
    lp.ldisp_data_req.pak = pak;
    lfsm_to_ldisp(&lp);
}

void lnm_send_udata_to_disp(lnm_t *lnm_ptr, paktype *pak, boolean local_address, ushort length)
{
    union L_primitives lp;
    lp.type = L_UDATASTN_REQ;
    lp.ldisp_udatastn_req.p_sap_id = lnm_ptr->lnm_port_id_dgra->p_sap_id;
    lp.ldisp_udatastn_req.theUData = pak->network_start;
    lp.ldisp_udatastn_req.udataLength = length;
    if (!local_address) {
	ieee_copy(lnm_ptr->dlc_id.client_mac_address, 
	     lp.ldisp_udatastn_req.remoteAddr);
    }
    else {
	ieee_copy(lnm_local_admin_addr,
	     lp.ldisp_udatastn_req.remoteAddr);
    }
    ieee_copy(lnm_ptr->dlc_id.server_mac_address, 
	     lp.ldisp_udatastn_req.localAddr);
    lp.ldisp_udatastn_req.lSap = SAP_IBMNM;
    lp.ldisp_udatastn_req.rSap = SAP_IBMNM;
    lp.ldisp_udatastn_req.options = EO_RIF_SPECIFIED;
    lp.ldisp_udatastn_req.rifLength = RIF_LENGTH(lnm_ptr->local_rif[0]);
    lp.ldisp_udatastn_req.rif = lnm_ptr->local_rif;
    lfsm_to_ldisp(&lp);
}    


void lnm_send_req_open_stn (lnm_t *lnm_ptr)
{
    union L_primitives 	lp;
    lp.type = L_REQ_OPNSTN_REQ;
    lp.ldisp_reqopenstn_req.uCepId = (uint)lnm_ptr;
    ieee_copy(lnm_ptr->dlc_id.client_mac_address, 
	      lp.ldisp_reqopenstn_req.remoteAddr);
    ieee_copy(lnm_ptr->dlc_id.server_mac_address, 
	      lp.ldisp_reqopenstn_req.localAddr);
    lp.ldisp_reqopenstn_req.lSap = SAP_IBMNM;
    lp.ldisp_reqopenstn_req.rSap = SAP_IBMNM;
    lp.ldisp_reqopenstn_req.rifLength = RIF_LENGTH(lnm_ptr->local_rif[0]);
    lp.ldisp_reqopenstn_req.rif = lnm_ptr->local_rif;
    lp.ldisp_reqopenstn_req.options = EO_RIF_SPECIFIED;
    lp.ldisp_reqopenstn_req.p_sap_id = lnm_ptr->lnm_port_id_dgra->p_sap_id;
    lfsm_to_ldisp(&lp);
}


void lnm_send_close_stn (lnm_t *lnm_ptr)
{
    union L_primitives lp;
    if (lnm_ptr->clsi_state == clnmst_closed )
      return;
    lp.type = L_CLOSESTN_REQ;
    lp.ldisp_closestn_req.pCepId = lnm_ptr->p_cep_id;
    lnm_ptr->clsi_state = clnmst_closed;
    /* 
     * so, we don't queue any more messages to CLSI.
     */
    lfsm_to_ldisp(&lp);
}


void lnm_send_connect_req (lnm_t *lnm_ptr)
{
    union L_primitives 	lp;
    lp.type = L_CONNECT_REQ;
    lp.ldisp_connect_req.pCepId = lnm_ptr->p_cep_id;
    lnm_ptr->clsi_state = lnmst_contact_pending;
    lfsm_to_ldisp(&lp);
}


void lnm_send_signalstn_req (lnm_t *lnm_ptr)
{
    union L_primitives 	lp;
    lp.type = L_SIGNALSTN_REQ;
    lp.ldisp_signalstn_req.pCepId = lnm_ptr->p_cep_id;
    lfsm_to_ldisp(&lp);
}
    


boolean	lnm_is_init_clsi_op(ushort primitive)
{
int i;
    for ( i=0; i < NLNM_INIT_CLSI_OP; i++) {
	if (primitive == lnm_init_clsi_op_tbl[i]) 
	    return TRUE;
    }
     return FALSE;
}


void lnm_init_clsi_op_proc(TCLSIMsg* clsi_msg)
{
    lnm_t	        *lnm_ptr = NULL;
    ushort 		primitive = CLSIMsgGetPrimitiveCode(clsi_msg);
    ulong 		clsi_cnf_ret_code = CLSIMsgGetReturnCode(clsi_msg);
    char 		*str;
    union L_primitives 	lp;
    CLSDLCType_e        dlc_type;
    boolean             port_flag = TRUE;
    lnm_clsi_t          *lnm_clsi_tmp = NULL;

    str = string_getnext();
    if ((primitive & CLS_TYPE_MASK) != CLS_CONFIRM)
	return;
    if (clsi_cnf_ret_code != CLS_OK) {
      switch (clsi_cnf_ret_code) {
      case CLS_OUT_OF_MEMORY:
      case CLS_SAP_BUSY :
      case CLS_UNKNOWN_SAP:
      case CLS_ALREADY_ENABLED:
      case CLS_NO_SAP_REGISTRY:
      case CLS_INVALID_TARGET_RING:
      case CLS_CONFIG_CONFLICT:
      default:
	errmsg(LNMC_ERR_BADCLSIRET, 
	       CLSIMsgSprintfShort(clsi_msg, str),
	       clsi_cnf_ret_code,
	       "init_clsi_op_proc", lnm_id(lnm_ptr, str));
	return;
      }
    }
    switch(primitive)
	{
	    case CLS_ENABLE  | CLS_CONFIRM :
		dlc_type   = ((EnableCnf_t *)clsi_msg)->fDLCType;
		if (dlc_type == CLS_VDLC)
		    port_flag = FALSE;
		else
		    port_flag = TRUE;
		lnm_send_activate_sap(clsi_msg, port_flag);
		break;
	    case CLS_DISABLE  | CLS_CONFIRM :
		dlc_type = ((DisableCnf_t *)clsi_msg)->fDLCType;
		if (dlc_type != CLS_VDLC)
		    lnm_clsi_cleanup(clsi_msg);
		break;    
	    case CLS_ACTIVATE_SAP | CLS_CONFIRM :
		lnm_clsi_tmp = lnm_usapid2dgra(clsi_msg->fIdentifier);
	        if (lnm_clsi_tmp) {
		  lnm_clsi_tmp->p_sap_id =
		      ((ActRngCnf_t *)clsi_msg)->fCLSIHdr.fPSapId;
 		  lnm_clsi_tmp->sap_enabled = TRUE; /* successfully enabled */
		}
		else {
		  errmsg(LNMC_ERR_BADCLSISAP,
			 CLSIMsgSprintfShort(clsi_msg, str),
			 clsi_msg->fIdentifier,
			 "lnm_pre_proc_clsi - ActSapCnf");
	        }
		break;
	    case CLS_DEACTIVATE_SAP | CLS_CONFIRM :
		lnm_clsi_tmp = lnm_usapid2dgra(clsi_msg->fIdentifier);
		if (lnm_clsi_tmp == NULL) {
		  errmsg(LNMC_ERR_BADCLSISAP,
			 CLSIMsgSprintfShort(clsi_msg, str),
			 clsi_msg->fIdentifier,
			 "lnm_pre_proc_clsi- DeActSapCnf");
		  return;
		}
		if (!lnm_clsi_tmp->vswidb)
		    lp.ldisp_disable_req.port_type = CLS_VDLC;
		else
 		    lp.ldisp_disable_req.port_type = lnm_clsi_tmp->port_type;
		lnm_clsi_tmp->sap_enabled = FALSE;
 		lp.ldisp_disable_req.swidb = lnm_clsi_tmp->swidb;
 		lp.type = L_DISABLE_REQ;
 		lfsm_to_ldisp(&lp);
		lnm_clsi_tmp->enabled = FALSE;
		break;
	    default :
		errmsg(LNMC_ERR_BADCLSICNF, 
		    CLSIMsgSprintfShort(clsi_msg, str),
		    "init_clsi_op_proc - default", lnm_id(lnm_ptr, str));
		break;
	    }
}


static void lnm_send_activate_sap(TCLSIMsg *clsi_msg, boolean port_flag)
{
  union 	L_primitives lp;
  lnm_clsi_t 	*lnm_clsi_tmp;

  ushort 	port_number, slot_number, sub_number, port_type;
  ushort        trans_bridge_group;
  CLSPortID     port_id;
  hwidbtype     *hw_idb;
  idbtype	*sw_idb;
  char          *str;
  srb_triplet_t srb_triplet;

  str = string_getnext();

  port_id   = ((EnableCnf_t *)clsi_msg)->fPortId;

  slot_number = CLSIPortIdGetSlotNumber(&port_id);
  port_number = CLSIPortIdGetPortNumber(&port_id);
  sub_number  = CLSIPortIdGetSubNumber(&port_id);
  port_type   = CLSIPortIdGetPortType(&port_id);

  sw_idb = CLSIPortIdGetSwIdb(&port_id);
  if (sw_idb == NULL ) {
      errmsg(LNMC_ERR_BADCLSI, 
          CLSIMsgSprintfShort(clsi_msg, str),
	  "Sending Activate Sap", "sw_idb is NULL"); 
      return;
  }
  trans_bridge_group = 0;
  hw_idb = sw_idb->hwptr;
  if (hw_idb == NULL ) {
      errmsg(LNMC_ERR_BADCLSI, 
          CLSIMsgSprintfShort(clsi_msg, str),
	  "Sending Activate Sap", "hw_idb is NULL ");
      return;
  }
  lnm_clsi_tmp = lnm_get_clsi(slot_number, port_number, port_type);
  if (!lnm_clsi_tmp) {
      lnm_clsi_tmp = lnm_get_clsi_swidb(sw_idb);
  }	
  if (!lnm_clsi_tmp) {
    lnm_clsi_tmp = malloc_named(sizeof(lnm_clsi_t), "LNM-SAP");
    if (!lnm_clsi_tmp) {
      return;
    }
    enqueue(&lnm_clsiQ, lnm_clsi_tmp);
  }
  srbV.srb_sb_get_triplet(sw_idb, &srb_triplet);
  lnm_clsi_tmp->ring_number   = srb_triplet.localRing;
  lnm_clsi_tmp->bridge_number = srb_triplet.bridgeNum;
  lnm_clsi_tmp->ring_group    = srb_triplet.remoteRing;

  lnm_clsi_tmp->slot_number = slot_number;
  lnm_clsi_tmp->port_number = port_number;
  lnm_clsi_tmp->sub_number  = sub_number;
  lnm_clsi_tmp->port_type   = port_type;
  lnm_clsi_tmp->trans_bridge_group = trans_bridge_group;
  lnm_clsi_tmp->mtu_size  = llc_mtu(hw_idb);
  lnm_clsi_tmp->enabled = TRUE;      /* cls port enabled status         */
  lnm_clsi_tmp->sap_enabled = FALSE; /* set to TRUE when cnf comes back */
  lnm_clsi_tmp->swidb = sw_idb;
  lnm_clsi_tmp->correlator = ((EnableCnf_t *)clsi_msg)->fCorrelator;
  if (port_flag)
      lnm_clsi_tmp->vswidb = sw_idb;
  else
      lnm_clsi_tmp->vswidb = (idbtype *)NULL;
  lnm_clsi_tmp->port_id = (uchar *)&port_id;
  lp.type = L_ACTIVATESAP_REQ;
  lp.ldisp_activatesap_req.u_sap_id = (uint)lnm_clsi_tmp;
  lp.ldisp_activatesap_req.sap_value     = SAP_IBMNM;
  lp.ldisp_activatesap_req.port_type     = lnm_clsi_tmp->port_type;
  lp.ldisp_activatesap_req.swidb         = lnm_clsi_tmp->swidb;
  lp.ldisp_activatesap_req.p_sap_id      = lnm_clsi_tmp->p_sap_id;
  lfsm_to_ldisp(&lp);
}

void ldisp_to_lfsm(int msg_type,  TCLSIMsg *clsi_msg)
{
    ushort 	primitive = CLSIMsgGetPrimitiveCode(clsi_msg);
    lnm_t  	*lnm_ptr;
    char 	*str;

    UDataInd_t	        *udata_ind;

    str = string_getnext();

    if (lnm_is_bad_clsi(clsi_msg)) {
	CLSIMsgDestroy(clsi_msg);
	return;
    }

    lnm_ptr = ucepid2lnm(clsi_msg->fIdentifier);
    
    if (lnm_ptr == NULL )
	lnmllcbuginf("\nLANMGR: ldisp_to_lfsm - null lnm_ptr from ucepid2lnm");
    else {
	switch(primitive) {
	case CLS_UDATA        | CLS_INDICATION:
            udata_ind = (UDataInd_t *) clsi_msg;
	    lnm_proc_info(clsi_msg, lnm_ptr);
	    /* the above function reuses the clsi pak */
	    break;
	case CLS_DATA        | CLS_INDICATION:
	    if ( lnm_ptr->lnm_state != lnmst_connected) {
		  errmsg(LNMC_ERR_BADCLSIIND, 
		  CLSIMsgSprintfShort(clsi_msg, str),
		  "disp_to_fsm; data/udata ", lnm_id(lnm_ptr, str));
		CLSIMsgDestroy(clsi_msg);	
	    }
	    else
		lnm_proc_info(clsi_msg, lnm_ptr);
	    /* the above function reuses the clsi pak */
	    break;
	case CLS_FLOW         | CLS_INDICATION:
	    lnm_ptr->fc_llc_flowlevel =
		((FlowIndh_t *) clsi_msg)->fFlowLevel;
	    CLSIMsgDestroy(clsi_msg);	
	    break;
	    default :
		errmsg(LNMC_ERR_BADCLSIIND, 
		       CLSIMsgSprintfShort(clsi_msg, str),
		       "disp_to_fsm", lnm_id(lnm_ptr, str));
	    CLSIMsgDestroy(clsi_msg);		    
	    break;
	}
    }
}

/*
 * stubs for lnm_disp.c
 */
void ldisp_to_init(int msg_type)
{
    return;
}

void lnm_proc_info(TCLSIMsg* clsi_msg, lnm_t *lnm_ptr)
{
    return;
}


void lanmgr_command (parseinfo *csb)
{
    switch (csb->which) {
      case LM_SNMP_ONLY:
	if (csb->nvgen) {
	    nv_write(ibmnm_no_changes, "!\n%s", csb->nv_command);
	    return;
	}
	ibmnm_no_changes = csb->sense;
	break;

      case LM_DISABLED:
	if (csb->nvgen) {
	    nv_write(!lanmgr_global_enable, "!\n%s", csb->nv_command);
	    return;
	}

	if (csb->sense) { 			/* disabled 		*/
	    if (!lanmgr_global_enable) {
		printf("Lanmgr has been terminated\n");
		return;
	    }
	    lanmgr_terminate();
	} else { 				/* restart lanmgr	*/
	    if (lanmgr_global_enable) {
		printf("Lanmgr has been started\n");
		return;
	    }
	    lanmgr_restart();
	}
	break;
	
      case LM_PATHTRACE_DISABLED:
	if (csb->nvgen) {
	    if (!lanmgr_pathtrace_enable_lnm &&
		!lanmgr_pathtrace_enable_orig) {
		nv_write(TRUE, "%s %s", csb->nv_command,
			 "all");
	    }
	    else {		
		nv_write(!lanmgr_pathtrace_enable_lnm, "%s",
		         csb->nv_command);
	        nv_write(!lanmgr_pathtrace_enable_orig, "%s %s",
                         csb->nv_command, "origin");
	    }
	    return;
	}
        if (GETOBJ(int,1) == LM_PATHTRACE_DISABLE_ORIGIN) {
            if (csb->sense)
                lanmgr_pathtrace_enable_orig = FALSE;    
            else
                lanmgr_pathtrace_enable_orig = TRUE;
        }
        else if (GETOBJ(int,1) == LM_PATHTRACE_DISABLE_ALL) {
            if (csb->sense) {
                lanmgr_pathtrace_enable_lnm  = FALSE;
                lanmgr_pathtrace_enable_orig = FALSE;
            }
            else {
                lanmgr_pathtrace_enable_lnm  = TRUE;
                lanmgr_pathtrace_enable_orig = TRUE;
            }
        }
        else {
	    if (csb->sense)
		lanmgr_pathtrace_enable_lnm = FALSE;
	    else
		lanmgr_pathtrace_enable_lnm = TRUE;
        }
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


void lanmgrif_command (parseinfo *csb)
{
    idbtype   *swidb = csb->interface;
    hwidbtype *idb = hwidb_or_null(csb->interface);
    ring_info *ring;
    bridge_info *bridge;
    srb_triplet_t srb_triplet;
    int link;
    char *buff;
    void lnm_fill_key(char *, char *), lnm_get_key(char *, char *);

    /* Do not continue if it is part of a PCbus virtual bridge */
    if (reg_invoke_local_lnm_vr_bridge (idb))
        return;

    if (csb->nvgen) {
	if (!(ring = idb_to_ring(idb)) ||
	    !(bridge = idb_to_bridge(idb)))
	    return;
	/* Fall through to the command switch below */

    } else {
	if (!is_srb_hw(idb)) {
	    printf("LNM functions are only supported on token rings \
and FDDI.");
	    return;
	}
	
	ring = idb->ring_info;
	if (!ring) {
	    printf("LANMGR: missing ring data structure.\n");
	    return;
	    }
	bridge = idb_to_bridge(idb);
    }
 
    srbV.srb_sb_get_triplet(swidb, &srb_triplet);

    switch (csb->which) {
      case LM_ALTERNATE:	    /* lanmgr alternate */
	if (!bridge) {
	    printf("You must configure the bridge before issuing the command.\n");
	    return;
	}
	if (csb->nvgen) {
	    nv_write(bridge->num_alt_lanmgrs != LM_DEF_ALT_LMS,
		     "%s %d", csb->nv_command,
		     bridge->num_alt_lanmgrs);
	    return;
	}
	if (csb->sense) {
	    bridge->num_alt_lanmgrs = GETOBJ(int,1);
	} else {
	    bridge->num_alt_lanmgrs = LM_DEF_ALT_LMS;
	}
	break;

      case LM_CRS:		    /* lanmgr crs */
	if (csb->nvgen) {
	    if (idb->tr_ring_mode & RINGMODE_NETMGR) {
		/* nada */
	    } else {
		nv_write(TRUE, "no %s", csb->nv_command);
	    }
	    return;
	}
	if (csb->set_to_default || csb->sense) {
	    idb->tr_ring_mode |= RINGMODE_NETMGR;
	} else {
	    idb->tr_ring_mode &= ~RINGMODE_NETMGR;
	}
	if (idb->set_extra_addrs != NULL)
	    (void)(*idb->set_extra_addrs)(idb);
	break;

      case LM_LOSS:		/* lanmgr loss-threshold */
	if (!bridge) {
	    printf("You must configure the bridge before issuing the command.\n");
	    return;
	}
	if (csb->nvgen) {
	    nv_write(bridge->frame_lost_thresh != LM_DEF_LOSS_THRESH,
		     "%s %d", csb->nv_command, bridge->frame_lost_thresh);
	    return;
	}
	if (csb->sense) {
	    bridge->frame_lost_thresh = GETOBJ(int,1);
	} else {
	    bridge->frame_lost_thresh = LM_DEF_LOSS_THRESH;
	}
	break;

#ifdef DEBUG
      case LM_NOTIF:		/* lanmgr notification-interval */
	if (csb->nvgen) {
	    nv_write(bridge->notification_interval != LM_DEF_NOTIF_INT,
		     "%s %d", csb->nv_command, 
		     bridge->notification_interval / ONEMIN);
	    return;
	}
	if (csb->sense) {
	    bridge->notification_interval = GETOBJ(int,1) * ONEMIN;
	} else {
	    bridge->notification_interval = LM_DEF_NOTIF_INT;
	}
	break;
#endif

      case LM_PASSWORD:		/* lanmgr password */
	if (!bridge) {
	    printf("You must configure the bridge before issuing the command.\n");
	    return;
	}
	if (csb->nvgen) {
	    for (link = 0; link <= LM_MAX_ALT_LMS; link++) {
		nv_write(bcmp("00000000", bridge->links[link].key, LMKEYLEN) != 0,
			 "%s %d %*s", csb->nv_command, link, LMKEYLEN,
			 bridge->links[link].key);
	    }
	    return;
	}
	link = GETOBJ(int,1);
	if (csb->sense) {
	    buff = GETOBJ(string, 1);
	    if (*buff == '"') {
		if (strlen(buff) > LMKEYLEN + 2) {
		    printf("Length of Password must be less than 8 chars\n");
		    return;
		}
		lnm_get_key(buff, bridge->links[link].key);
	    } else {
		if (strlen(buff) > LMKEYLEN) {
		    printf("Length of Password must be less than 8 chars\n");
		    return;
		}
		lnm_fill_key(buff, bridge->links[link].key);
	    }

	    bcopy(bridge->links[link].key, bridge->links[link].newkey, LMKEYLEN);

	} else {
	    for (link = 0; link <= LM_MAX_ALT_LMS; link++) {
		bcopy("00000000", bridge->links[link].key, LMKEYLEN);
		bcopy("00000000", bridge->links[link].newkey,LMKEYLEN);
	    }
	}
	break;

      case LM_REM:		/* lanmgr rem */
	if (csb->nvgen) {
	    nv_write(!(idb->tr_ring_mode & RINGMODE_REM),
		     "no %s", csb->nv_command);
	    return;
	}
	if (csb->sense) {
	    idb->tr_ring_mode |= RINGMODE_REM;
	} else {
	    idb->tr_ring_mode &= ~RINGMODE_REM;
	}
	if (idb->set_extra_addrs != NULL)
	    (void)(*idb->set_extra_addrs)(idb);
	break;

#ifdef DEBUG
      case LM_RING:	    /* lanmgr ring */
	if (csb->nvgen) {
	    nv_write((srb_triplet.bridgeNum > SRB_MAX_BRIDGE_NUM) && srb_triplet.localRing,
		     "%s %d", csb->nv_command, srb_triplet.localRing);
	    return;
	}
	if (csb->sense) {
	    if (srb_triplet.localRing && (GETOBJ(int,1) != srb_triplet.localRing)) {
		printf("Ring number does not match SRB parameters\n");
		return;
	    }
	    srbV.srb_sb_set_thisring(swidb, GETOBJ(int,1));
	} else {
	    if (srb_triplet.bridgeNum) {
		printf("SRB is active on this interface.\n");
		return;
	    }
	    srbV.srb_sb_set_thisring(swidb, 0);
	}
	break;
#endif

      case LM_RPS:	    /* lanmgr rps */
	if (csb->nvgen) {
	    nv_write(!(idb->tr_ring_mode & RINGMODE_RPS),
		     "no %s", csb->nv_command);
	    return;
	}
	if (csb->sense) {
	    idb->tr_ring_mode |= RINGMODE_RPS;
	    /*
	     * We cannot simply turn on the RPS.  We must make sure that
	     * all RPSs on the ring are using the same ring number.
	     * Tr_get_smt_stats will cause the right thing to happen when
	     * it reads the actual ring numner from the chipset.
	     */
	    if (GETOBJ(int,1) == TRUE) {
		ring->rps_state = RPS_STATE_ACTIVE;
		if (idb->set_extra_addrs != NULL)
		    (void)(*idb->set_extra_addrs)(idb);
		rps_startup(idb, srb_triplet.localRing);
	    } else {
		ring->rps_state = RPS_STATE_DETERMINE;
		srbV.srb_sb_set_actualring(swidb, 0);
		reg_invoke_media_tring_get_smt_stats(idb->type, idb);
	    }
	} else {
	    idb->tr_ring_mode &= ~RINGMODE_RPS;
	    ring->rps_state = RPS_STATE_OFF;
	    if (idb->set_extra_addrs != NULL)
		(void)(*idb->set_extra_addrs)(idb);
	    idb->mac_use_exp_buf = FALSE;
	}
	break;

#ifdef DEBUG
      case LM_RS:	    /* lanmgr station */
	if (csb->nvgen) {
	    /* No NV generation for this debugging command according to Hampton */
	  return;
        }
	if (csb->sense) {
	    idb->tr_ring_mode |= RINGMODE_RS;
	} else {
	    idb->tr_ring_mode &= ~RINGMODE_RS;
	}
	if (idb->set_extra_addrs != NULL)
	    (void)(*idb->set_extra_addrs)(idb);
	break;
#endif

      case LM_SOFTERR:	    /* lanmgr softerr */
	if (csb->nvgen) {
	    if (idb->tr_ring_mode & RINGMODE_RPS)
	    nv_write(ring->softerr_timer != RPS_SOFTERR_DEFAULT,
		     "%s %d", csb->nv_command, ring->softerr_timer);
	    return;
	}
	if (!csb->sense) {
	    ring->softerr_timer = RPS_SOFTERR_DEFAULT;
	    return;
	}

	ring->softerr_timer = GETOBJ(int,1);
	break;

      case LM_RINGNUM_MISMATCH_ALLOWED:
        if (csb->nvgen) {
            nv_write(ring->num_mismatch_allowed, "%s", csb->nv_command);
            return;
        }
        ring->num_mismatch_allowed = (csb->sense ? TRUE : FALSE);
        break;

      case LM_MAC_USE_EXP_BUF:	    /* express buffer */
	if (csb->nvgen) {
            nv_write(idb->mac_use_exp_buf, "%s", csb->nv_command);
            return;
        }
        if (csb->sense) {
            if (!(idb->tr_ring_mode & RINGMODE_RPS)) {
                printf("\n Cannot use express buffer! RPS is disabled!");
                return;
            }
            idb->mac_use_exp_buf = TRUE;
        }
        else 
            idb->mac_use_exp_buf = FALSE;
        break;
            
      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    lanmgr_start();
}


/**********************************************************************
 *
 *			 show command support
 *
 **********************************************************************/
    
static int display_one_station (
    station_info *info,
    boolean header,
    boolean all)
{
    hwidbtype *idb;
    int line = 0;
    char flag;

    if (header) {
	printf("\n                                              isolating error counts");
	printf("\n    station      int    ring  loc.   weight   line  inter burst  ac   abort");
/*      printf("\nxxxx.xxxx.xxxxT   xx  xxxx  xxxx   xx - x   xxxxx..."); */
	line += 3;
    }

    idb = info->idb;
    if (info->error_state & ERR_ME_WEIGHT)
	flag = 'W';
    else if (info->error_state & ERR_ME_PREWEIGHT)
	flag = 'P';
    else 
	flag = 'N';
    printf("\n%e%c  %7s%04x  %04x   %02x - %c   %05x %05x %05x %05x %05x",
	   info->address, TIMER_RUNNING(info->temp) ? 'T' : ' ',
	   idb->hw_short_namestring, srbV.srb_sb_get_thisring(idb->firstsw), 
           info->phys_location, info->weight, flag, info->isolating.line,
	   info->isolating.internal, info->isolating.burst,
	   info->isolating.ac, info->isolating.abort);
    line += 1;
    
    if (all) {	
	printf("\n\nUnique ID:  %e          NAUN: %e", info->station_id,
	       info->naun);
	printf("\nFunctional: C000.%04x.%04x         Group: C000.%04x.%04x",
	       info->functional >> 16, info->functional & 0xffff,
	       info->group >> 16, info->group & 0xffff);
	printf("\nPhysical Location:   %05d        Enabled Classes:  %04x",
	       info->phys_location, info->enabled_functions);
	printf("\nAllowed Priority:    %05d        Address Modifier: %04x",
	       info->allowed_priority, info->address_modifier);
	printf("\nProduct ID:     %08x.%08x.%08x.%08x.%04x",
                GETLONG(&info->product_id[ 0]),
                GETLONG(&info->product_id[ 4]),
                GETLONG(&info->product_id[ 8]),
                GETLONG(&info->product_id[12]),
                GETSHORT(&info->product_id[16]));
	printf("\nUcode Level:    %08x.%08x.%04x",
	       *((ulong  *)&info->ucode[0]),
	       *((ulong  *)&info->ucode[4]),
	       *((ushort *)&info->ucode[8]));
	printf("\nStation Status: %08x.%04x",
	       *((ulong  *)&info->status_vector[0]),
	       *((ushort *)&info->status_vector[4]));
	printf("\nLast transmit status: %02x", info->transmit_status);
	line += 9;
    }
    return(line);
}

static void display_station_by_idb (
    hwidbtype *idb)
{
    station_info *info, *amp;
    ring_info *ring;
    int line = 0;
    boolean header = TRUE;
    leveltype status;

    if (idb) {
	ring = idb_to_ring(idb);
	if (!ring) return;

	info = amp = find_station(ring->amp, idb);
        if (info)
            mem_lock(info);
        ring->new_amp = FALSE;
	if (!info) return;
	do {
	    automore_conditional(12);

            if (ring->new_amp == TRUE) {
                printf("\nNew active monitor present! Re-enter the command for latest station information");
                free(info);
                return;
            }
	    line += display_one_station(info, header, FALSE);
            free(info);
	    header = FALSE;
            status = raise_interrupt_level(ALL_DISABLE);
	    info = find_station(info->naun, idb);
            if (info)
                mem_lock(info);
            reset_interrupt_level(status);
            if ((!info) || (ieee_equal(info->address, amp->address))) {
                if (info)
                    free(info);
                break;
            }
	} while (info != amp);
    }
}

static void display_station_by_address (
    uchar *address)
{
    station_info *info;
    int bucket, line = 0;

    for (bucket = 0; bucket < NETHASHLEN; bucket++) {
	for (info = station_table[bucket]; info; info = info->next) {
	    if (!ieee_equal_sans_rii(info->address, address))
		continue;
            mem_lock(info);
	    automore_conditional(12);
	    line += display_one_station(info, TRUE, TRUE);
            free(info);
	}
    }
}

static void display_all_stations (
    boolean verbose)
{
    station_info *info, *amp;
    ring_info *ring;
    int line = 0;
    boolean header = TRUE;
    hwidbtype *idb;
    leveltype status;

    for (ring = ringQ.qhead; ring; ring = ring->next) {
	idb = ring_to_idb(ring);
        if ((!idb) || (idb->state != IDBS_UP)) continue;

	info = amp = find_station(ring->amp, idb);
        if (info)
            mem_lock(info);
        ring->new_amp = FALSE;
	if (!info) continue;
	do {
	    automore_conditional(12);

            if (ring->new_amp == TRUE) {
                printf("\nNew active monitor present! Re-enter the command for latest station information");
                free(info);
                return;
            }
	    line += display_one_station(info, header | verbose, verbose);
            free(info);
	    header = FALSE;
            status = raise_interrupt_level(ALL_DISABLE);
	    info = find_station(info->naun, idb);
            if (info)
                mem_lock(info);
            reset_interrupt_level(status);
            if ((!info) || (ieee_equal(info->address, amp->address))) {
                if (info)
                    free(info);
                break;
            }
	} while (info != amp);
    }
}

/*
 *
 */
void display_ring_info (ring_info *target)
{
    ring_info *ring;
    boolean onceflag = FALSE;
    int line = 2;
    char buffer[20];
    hwidbtype *idb;

    for (ring = ringQ.qhead; ring; ring = ring->next) {
	if (target && (target != ring))
	    continue;
	idb = ring_to_idb(ring);
	if (!idb)
	    continue;
	if (onceflag) {
	    if (target != NULL)
		return;
	} else {
	    printf("\n                                             nonisolating error counts");
	    printf("\ninterface     ring   Active Monitor   SET    dec  lost  cong.  fc   freq. token\n");
/*          printf("\n1234567890  xxxxc  xxxx.xxxx.xxxx  xxxxx  xxxxx  xxxxx xxxxx xxxxx..");  */
	    onceflag = TRUE;
	}

	printf("\n%12s  %04d%c  %e  %05d  %05d %05d %05d %05d %05d %05d",
	       idb->hw_short_namestring, srbV.srb_sb_get_thisring(idb->firstsw),
	       ring->station_errors?'*':' ', ring->amp, ring->softerr_timer,
	       ring->decrement, ring->noniso.lost_frame,
	       ring->noniso.congestion, ring->noniso.frame_copied,
	       ring->noniso.frequency, ring->noniso.token);
	line++;
    }

    /*
     * Give more information if a specific ring was asked for.
     */
    if (!target)
	return;

    ring = target;
    idb = ring_to_idb(ring);

    /*
     * More info
     */
    printf("\nNotification flags: %02x, Ring Intensive: %02x, Auto Intensive: %02x",
	   ring->notif_flags, ring->ring_inten_mask, ring->auto_inten_mask);
    printf("\nActive Servers: LRM LBS");
    if (idb->tr_ring_mode & RINGMODE_REM)
	printf(" REM");
    if (idb->tr_ring_mode & RINGMODE_RPS)
	printf(" RPS");
    if (idb->tr_ring_mode & RINGMODE_NETMGR)
	printf(" CRS");
    printf("\n");

    /*
     * Print 'Lasts'.
     */
    sprint_dhms(buffer, ring->last_nnin_time);
    printf("\nLast NNIN:   %s, from %e.", buffer, ring->last_nnin_addr);

    sprint_dhms(buffer, ring->last_claim_time);
    printf("\nLast Claim:  %s, from %e.", buffer, ring->last_claim_address);

    sprint_dhms(buffer, ring->last_purge_time);
    printf("\nLast Purge:  %s, from %e.", buffer, ring->last_purge_address);

    sprint_dhms(buffer, ring->last_beacon_time);
    printf("\nLast Beacon: %s, '%s' from %e.", buffer, 
	   sv_beacon_to_str(ring->last_beacon_type), ring->last_beacon_address);

    sprint_dhms(buffer, ring->last_monerr_time);
    printf("\nLast MonErr: %s, '%s' from %e.", buffer,
	   sv_errcode_to_str(ring->last_monerr_code), ring->last_monerr_address);

    automore_conditional(0);

    display_station_by_idb(idb);
}

/*
 *
 */
void show_lanmgr (parseinfo *csb)
{
    int ring_no;
    ring_info *ring=NULL;
    hwidbtype *idb;
    uchar address[IEEEBYTES];
    uint pathtrace_display = PATHTRACE_NONE;

    if (!lanmgr_global_enable) {
	printf("Lanmgr is disabled\n");
	return;
    }

    idb = hwidb_or_null(GETOBJ(idb,1));
    switch (csb->which) {
      case SHOW_LANMGR_BRIDGE:
	ibmnm_display_bridge(idb, "");
	pathtrace_display = PATHTRACE_BRIDGE;
	break;

      case SHOW_LANMGR_CONFIG:
	ibmnm_display_config();
	pathtrace_display = PATHTRACE_CONFIG;
	break;

      case SHOW_LANMGR_INTERFACE:
	automore_enable(NULL);
	if (idb) {
		ring = idb_to_ring(idb);
		display_ring_info(ring);
	}
	else 
		display_ring_info(ring);
	break;
	automore_disable();
	break;

      case SHOW_LANMGR_RING:
	automore_enable(NULL);
	if ((ring_no = GETOBJ(int,1)))
	{
		ring = ringno_to_ring(ring_no);
		if (!ring) {
	    	printf("No such ring - %d.", ring_no);
		} else {
	    	display_ring_info(ring);
		}
	}
	else
		display_ring_info(ring);
	automore_disable();
	break;

      case SHOW_LANMGR_STATION:
	automore_enable(NULL);
	if (!GETOBJ(int,1) && !GETOBJ(int,2))
		display_all_stations(FALSE);
	else if (GETOBJ(int,2))
		display_all_stations(TRUE);
	else {
		ieee_copy(GETOBJ(hwaddr,1)->addr, address);
		display_station_by_address(address);
	}
	automore_disable();
	pathtrace_display = PATHTRACE_STATION;
	break;

#ifdef IBMNM_DEBUG
      case SHOW_LANMGR_ZEBRA:
	ibmnm_test("");
	break;
#endif

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    if (pathtrace_display) {
      if ((!lanmgr_pathtrace_enable_lnm) ||
          (!lanmgr_pathtrace_enable_orig)) {
        switch (pathtrace_display) {
          case PATHTRACE_BRIDGE:
            printf("\nBridge Path Trace Reporting Summary:\n");
            break;
          case PATHTRACE_CONFIG:
            printf("\nConfig Path Trace Reporting Summary:\n");
            break;
          case PATHTRACE_STATION:
            printf("\nStation Path Trace Reporting Summary:\n");
            break;
          default:
            break;
        }
        if (!lanmgr_pathtrace_enable_lnm) {
          printf("Path Trace Reporting to LANMGR stations currently disabled\n");
        }
        if (!lanmgr_pathtrace_enable_orig) {
          printf("Path Trace Reporting to Originating stations currently disabled\n");
        }
      }
    }
}
/*
 *
 *
 */

void lnm_get_key (
char *src, char *dst)
{
     int i;

     for (i = 0, src++; *src != '"' && *src != 0 && i < LMKEYLEN; i++)
	*dst++ = *src++;

     while (i++ < LMKEYLEN)
	*dst++ = ' ';

}

void lnm_fill_key (
char *src, char *dst)
{
    int i;

    i = strlen(src);
     
    bcopy(src, dst, i);
    while (i < LMKEYLEN)
	dst[i++] = ' ';
}

/*
 *  Terminate Lanmgr on the fly
 */
void lanmgr_terminate (void)
{
    hwidbtype *idb;
    paktype *pak;
    bridge_info *b, *next;
    station_info *info, *tmpinfo;
    ring_info *ring;
    int i;
    TCLSIMsg* clsi_msg;

    ibmnm_reload_message();


    /* Destroy the ring and free the message		*/
    FOR_ALL_HWIDBS(idb) {
     	if (!is_srb_hw(idb))
		continue;

        if ((ring = idb_to_ring(idb))) {
            idb->tr_ring_mode &= ~RINGMODE_RPS;
            ring->rps_state = RPS_STATE_OFF;
            if (idb->set_extra_addrs != NULL)
                (void)(*idb->set_extra_addrs)(idb);
            idb->mac_use_exp_buf = FALSE;
        }

	lanmgr_destroy_ring(idb);
    }

    lanmgr_global_enable = FALSE;
    lanmgr_pathtrace_enable_lnm = FALSE;
    lanmgr_pathtrace_enable_orig = FALSE;						
    while ((pak = p_dequeue(&lanmgrQ)) ) {
	datagram_done(pak);
    }

    while ((clsi_msg = p_dequeue(&lnm_ctlQ))) {
        CLSIMsgDestroy(clsi_msg);
    }	  
    while ((pak = p_dequeue(&ibmnmQ))) {
	datagram_done(pak);
    }
    /* Free up the bridge structure				*/
    for (b = (bridge_info *)lm_bridgeQ.qhead; b; ) {
	next = b->next;
	p_unqueue(&lm_bridgeQ, b);
    	free(b);
	b = next;
    }

    /* Free the link station structure				*/
    for (i = 0; i < NETHASHLEN; i++) {
        info = station_table[i];
	while (info) {
	    if (info->rps_pak)
		free_rps_pak(info);
	    if (info->crs_pak)
		free_crs_pak(info);
	    tmpinfo = info;
	    info = info->next;
	    tmpinfo->next = NULL;
	    free(tmpinfo);
	}
	station_table[i] = NULL;
    }

    stop_lanmgr_timer();    
    stop_rem_timer();                          
}

void lanmgr_restart (void)
{

    hwidbtype *idb;

    lanmgr_global_enable = TRUE;
    lanmgr_pathtrace_enable_lnm = TRUE;
    lanmgr_pathtrace_enable_orig = TRUE;				       
    crs_correlator = 1;
    rps_correlator = 1;

    ibmnm_init();
    FOR_ALL_HWIDBS(idb) {
    	if (!is_srb_hw(idb))
	    continue;

	idb->tr_ring_mode |= (RINGMODE_REM | RINGMODE_RPS | RINGMODE_NETMGR);
	if (idb->set_extra_addrs != NULL)
	    (void)(*idb->set_extra_addrs)(idb);

    	(void)lanmgr_create_ring(idb);
    }

    stop_lanmgr_timer();
    stop_rem_timer();
    set_ibmnm_interfaces(NULL, FALSE);
}

/*
 * LNM subsystem header
 */

#define LNM_MAJVERSION 1
#define LNM_MINVERSION 0
#define LNM_EDITVERSION  1

SUBSYS_HEADER(lanmgr, LNM_MAJVERSION, LNM_MINVERSION, LNM_EDITVERSION,
	      lanmgr_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, netbios_acl, srb",
	      "req: netbios_acl, llc2, cls, srb, netbios_name");

