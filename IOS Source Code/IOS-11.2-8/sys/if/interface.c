/* $Id: interface.c,v 3.18.10.21 1996/08/15 02:32:29 hampton Exp $
 * $Source: /release/112/cvs/Xsys/if/interface.c,v $
 *------------------------------------------------------------------
 * interface.c -- functions for manipulating swidb and hwidb data structures
 *
 * 27-May-1993, Kirk Lougheed
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interface.c,v $
 * Revision 3.18.10.21  1996/08/15  02:32:29  hampton
 * Change some function arguments to be 'const'.  [CSCdi66102]
 * Branch: California_branch
 *
 * Revision 3.18.10.20  1996/08/13  14:55:52  tylin
 * CSCdi61338:  ATM LEC sub-interf missing from PIM-MIB
 *    Set snmp_if_index of main swidb to be the same as hwidb.
 * Branch: California_branch
 *
 * Revision 3.18.10.19  1996/07/18  22:01:56  snyder
 * CSCdi63378:  remove calculated pluralization from show hardware
 *              make rxtype table const
 *                   656 out of image
 *                   188 out of data section
 * Branch: California_branch
 *
 * Revision 3.18.10.18  1996/07/08  22:31:06  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.18.10.17  1996/07/03  17:39:17  rbadri
 * CSCdi62015:  DECnet fast switching over LANE doesnt work
 * Branch: California_branch
 * In idb_create_subif(), increment the newly allocated swidb's
 * cache version as opposed to the passed in IDB.
 *
 * Revision 3.18.10.16  1996/07/02  23:11:08  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.18.10.15  1996/06/27  21:41:47  rbadri
 * CSCdi61523:  Can not configure MTU on a sub-if greater than hwidb (for
 * ATM)
 * Branch: California_branch
 * Apply the mtu command on the major interface to .0 interface.
 * sub-interfaces will have an MTU based on the configuration in them or
 * the default from hwidb.
 *
 * Revision 3.18.10.14  1996/06/17  22:53:31  widmer
 * CSCdi58627:  Parser allows configuration of invalid subinterface: 0.-1
 * Branch: California_branch
 * Change %d to %u
 *
 * Revision 3.18.10.13  1996/06/17  08:31:56  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.18.10.12  1996/06/11  09:30:28  dstine
 * CSCdi57115:  Counts not cleared with clear counters command
 * Branch: California_branch
 *
 * Revision 3.18.10.11  1996/06/07  20:46:07  fred
 * CSCdi58655:  turning RED off on an interface turns WFQ off on that
 *              interface. Save default in a variable and restore that
 *              when folks turn off RED, Priority, or Custom Queuing.
 * Branch: California_branch
 *
 * Revision 3.18.10.10  1996/05/28  03:29:55  dstine
 * CSCdi58776:  Inverted interface ordering seen in SNMP and
 *              configuration display
 * Branch: California_branch
 *
 * Revision 3.18.10.9  1996/05/22  20:54:53  raj
 * CSCdi56721:  Debug ip mpacket doesnt show ttl and other important fields
 * Branch: California_branch
 * Added fields for "detail", and specifying acl as well as group.
 *
 * Revision 3.18.10.8  1996/05/21  09:49:54  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.18.10.7  1996/05/21  06:31:16  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.18.10.6  1996/05/17  11:22:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.18.2.4  1996/05/03  04:04:25  bchan
 * CSCdi56609:  dynamic idb list creation crashes the system
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.18.2.3  1996/05/02  22:04:30  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.18.2.2  1996/04/26  07:55:10  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.18.2.1  1996/04/03  14:37:54  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.18.10.5  1996/05/09  14:30:00  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.18.10.4.14.1  1996/04/27  07:03:49  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.18.10.4.4.3  1996/04/25  20:04:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Incorporate gstovall's comments:
 * - filesys/flash_config.c
 *         - Don't include globs.h, master.h already includes it.
 * - h/interface.h
 *         - Extra "," after IDBTYPE_SYNALC_BIGA.
 * - if/interface.c
 *         - Why did you include network.h?
 *
 * Revision 3.18.10.4.4.2  1996/04/11  00:04:02  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Track manual configuration of MTU on swidb basis
 * Save sub-interface MTU to NVRAM
 *
 * Revision 3.18.10.4.4.1  1996/04/08  01:54:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.18.10.4  1996/03/27  22:28:05  wmay
 * CSCdi51648:  No channel-group, channel-group command causes crash
 * Branch: California_branch
 *
 * Revision 3.18.10.3  1996/03/23  01:32:23  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.18.10.2  1996/03/21  22:48:57  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.18.10.1  1996/03/18  20:13:58  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.18.12.2  1996/03/28  17:25:27  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.18.12.1  1996/03/22  09:37:16  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.18  1996/02/23  20:11:04  feisal
 * CSCdi49543:  Subinterface commands over 20 characters crash the router
 *
 * Revision 3.17  1996/02/13  08:12:59  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.16  1996/02/13  02:53:52  hampton
 * Stop the carrier timer before clearing the IDB.  [CSCdi46506]
 *
 * Revision 3.15.6.1  1996/03/05  06:13:50  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.15.2.1  1996/02/27  21:08:20  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.15  1996/02/11  02:36:08  mwu
 * CSCdi47972:  one more NULL check in idb_add_swidb_to_list.
 *
 * Revision 3.14  1996/02/09  03:08:38  mwu
 * CSCdi47972:  chunk_malloc return was not checked for NULL and caused
 * a SegV exception. The size of IDB List Element Chunks is resized to
 * fit supported interfaces because chunk can not be grown dynamically
 * from interrupt level.
 *
 * Revision 3.13  1996/02/08  23:09:16  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.12  1996/01/18  23:00:55  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.11  1996/01/11  03:50:48  kao
 * CSCdi43878:  IS_VIP() macro should be used
 *
 * Revision 3.10.2.1  1996/02/15  19:13:47  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.10  1996/01/05  07:46:47  hampton
 * Even though 'commit' comes alphabetically before 'compile', they should
 * be performed the other way round when programming.  Fix the 11.1 tree
 * so that it compiles again.  [CSCdi45120]
 *
 * Revision 3.9  1996/01/05  00:15:06  fred
 * CSCdi45120:  No fair-queue command does not take effect on async
 * interface
 *         fair queuing was being re-enabled during link reset. changed
 *         logic to
 *         only disable state automatically, not enable it
 *
 * Revision 3.8  1995/12/15  08:01:50  rramacha
 * CSCdi42101:  sh ver does not recognize CX-SMIP or CX-SSIP cards
 *
 * Revision 3.7  1995/12/07  22:41:55  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.6  1995/12/06  18:29:52  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.5  1995/11/24  19:38:13  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state. It
 * is not normally altered unless the protocols controlling the
 * sub-interface wish to do so. An interface routine
 * subif_line_statechange() is provided to change swidb's line state.
 *
 * Revision 3.4  1995/11/24  16:39:02  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.3  1995/11/17  09:29:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:38:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/11/08  21:03:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.10  1995/10/18  17:18:38  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 * Sort high end idb's by slot/slotunit/vc, low end by unit/vc.
 *
 * Default slot and slotunit are changed to MAXUINT.  Virtual interfaces
 * (tunnel, loop, async) shouldn't change them, as parser is now smart
 * enough to use unit on high end.
 *
 * Sort swidbs by subinterface number, rather than alphabetically
 * (CSCdi35897).
 *
 * Revision 2.9  1995/09/19  18:36:48  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.8  1995/09/12  21:10:13  pst
 * CSCdi39958:  ip ethernet 802.3 packets bridged when they should be
 * routed
 *
 * Revision 2.7  1995/08/11  18:34:00  wmay
 * CSCdi38645:  Invalid media message received while bridging on
 * subinterfaces - propogate tbridge_media type to subinterfaces - also
 * add point to point subints to bridge flood list
 *
 * Revision 2.6  1995/06/30  06:41:55  dstine
 * CSCdi36619:  IDB nits
 *         - tidy up some nits
 *         - fix error in dynamic hwsb usage check
 *
 * Revision 2.5  1995/06/29  23:29:35  jchlin
 * Currently, c4000 and c7000 are reusing hwidbs and swidbs because
 * of dynamic changes of interface configuration. Due to the lack of
 * way to clean up the idbs when they are freed, both platforms will
 * try to release as many known buffer pointers of idb as possible
 * before they are put in the private queue for reuse.
 * When reusing idbs, idb init routines called inside idb_create()
 * are called to reinitialized idbs. In 11.0, there is a new
 * function call added in idb_create() which is not picked up by
 * both platform init_idb(). And therefore, it caused a crash
 * because a null pointer is not initialized. This fix is trying to
 * separate the malloc of idbs and initialization of idbs into
 * two different functions so that the initialization of
 * both hwidb and swidb can be done in that new rourtine as well
 * as new addition function calls in the future. Then we can avoid
 * another addition of function calls in the idb_create() to crash
 * both platform again.
 * CSCdi36076:  Configuring PRI on E1 crashes box w/ALIGN-1-FATAL
 *
 * Revision 2.4  1995/06/21  09:08:48  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.3  1995/06/14  00:44:40  dstine
 * CSCdi35572:  cBus analysis deletes IDBs, causing list manager crash
 *
 * Revision 2.2  1995/06/08  20:52:34  wmay
 * CSCdi35517: Extra field in swidb-bridge_flood_first ==> tbridge_circuitQ
 *
 * Revision 2.1  1995/06/07  20:53:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "../snmp/ifmib_registry.h"
#include "network.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_priority.h"
#include "../if/loopback.h"
#include "../if/if_vidb.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../os/chunk.h"
#include "mgd_timers.h"
#include "../iprouting/igrp.h"
#include "address.h"
#include "packet.h"

/*
 * Storage
 */
int nnets;			/* total non-virtual interfaces */
int nethernets;			/* number of Ethernet/IEEE 802.3 interfaces */
int nfethernets;		/* number of 100 Mbit Ethernet interfaces */
int nserialnets;		/* number of serial interfaces */
int nas_serialnets;		/* number of sync/async serial interfaces */
int nTRs;			/* number of Token Ring interfaces */
int nbrinets;                   /* number of Basic Rate interfaces */
int nprinets;                   /* number of Primary Rate interfaces */
int is_pri_supp;               	/* pri supported in this image flag */
int nfddinets;                  /* number of fddi interfaces */
int nhssinets;                  /* number of HSSI serial interfaces */
int nposnets;
int natmnets;			/* Number of ATM interfaces */
int npcbusnets;                 /* number of PC bus interfaces */
int nchannelnets;               /* number of IBM channel interfaces */
int nfethernets;                /* number of Fast Ethernet interfaces */
int netherhubports;             /* number of ethernet hub ports */
int ne1ports;                   /* number of Channelized E1 ports */
int nt1ports;                   /* number of Channelized T1 ports */
int nse1ports;                  /* number of Channelized E1 ports in SMIP */
int nst1ports;                  /* number of Channelized T1 ports in SMIP */

queuetype swinQ;		/* queue of idb's */
queuetype hwinQ;		/* queue of hardware idb's */
queuetype *swidbQ = & swinQ;	/* pointer to idb queue */
queuetype *hwidbQ = & hwinQ;	/* pointer to hardware idb queue */


static int next_swidb_index;	/* next swidb number */
static int next_hwidb_index;	/* next hwidb number */

static chunk_type	*idb_sb_chunk;	/* IDB Sub-Block registration chunks */
static chunk_type	*idb_list_hdr_chunk; /* IDB list header chunks */
static chunk_type	*idb_list_elt_chunk; /* IDB list element chunks */
static chunk_type	*idb_notify_chunk;   /* IDB deletion notify chunks */

/*
 * More platforms are now planning to support hotswap
 */
boolean hotstall_flag;

static char *hwidb_default_typestring = "Unknown";
static const char *hardware_idb = "*Hardware IDB*";  /* Hardware IDB ident */
static const char *software_idb = "*Software IDB*";  /* Software IDB ident */

static idb_si_counters_t idb_si_counter_zeros;

/*
 * There are two types of IDB (that we care about, anyway) -- SW idb's
 * and HW idb's. It is very useful to know when we pick up a pointer
 * to a "idb" whether it is a SW or HW idb.
 */
enum {
    IDBTYPE_HW=1,
    IDBTYPE_SW
};

/*
 * Master IDB headers for private IDB list functions.
 * The private IDB list functions are used to allow clients of the IDB's
 * (either/both HW and SW idb's) to keep their own private lists
 * of idb's, so that they need not walk every idb in the box with
 * macros like FOR_ALL_SWIDBS() or FOR_ALL_HWIDBS().
 */

static struct {
    list_header	fixed_list[IDBLIST_MAX];
    ulong	dynamic_id;
    list_header	dynamic_list;
} idblist_headers;

typedef struct dynamic_idblist_hdr_ {
    list_element	link;		/* Must be first in struct */
    ulong		type;		/* typecode  */
    list_header		header;
} dyn_idblist_hdr_t;

/*
 * Usage of the idblist element:
 *
 * The 'link' element is a normal list element.
 * We put the pointer to the idb (either hardware or software idb)
 * in the link.data field of the link struct element. This allows
 * the user of the IDBlist facility to loop across their
 * lists using the list.h macros FOR_ALL_DATA_IN_LIST (if they
 * get the head of the list themselves) or using the
 * FOR_ALL_SWIDBS_IN_LIST() or FOR_ALL_HWIDBS_IN_LIST() macros
 * to be completely transparent.
 *
 * For internal management of the idblist elements, we use
 * the list manager FOR_ALL_ELEMENTS_IN_LIST() and typecasting so that
 * we are looking at the dyn_idblist_elt_t struct, rather than just
 * the list_element struct in the front.
 */
typedef struct dynamic_idblist_elt_ {
    list_element	link;		/* Must be first in struct */
    ushort		type;		/* IDBLIST_* typecode */
    ushort		idb_type;	/* SW idb or HW idb */
    void		*idb;		/* could be sw or hw idb */
} dyn_idblist_elt_t;

/*
 * Text names for IDB lists.
 */
static char *const idblist_names[IDBLIST_MAX] =
{
    "HW IDB Apollo",
    "HW IDB AppleTalk",
    "HW IDB Asynch",
    "HW IDB ATM",
    "HW IDB BOOTP",
    "HW IDB BRI",

    "HW IDB BSI FDDI",
    "HW IDB CBUS",
    "HW IDB CDP",
    "HW IDB CIP",
    "HW IDB CLNS",
    "HW IDB DECnet",
    "HW IDB Ethernet",
    "HW IDB FDDI",
    "HW IDB HSSI",
    "HW IDB IP",
    "HW IDB IP ARP",
    "HW IDB IP HOST",
    "HW IDB IP Multicast",
    "HW IDB LANCE",
    "HW IDB LAN Manager",
    "HW IDB LAT",
    "HW IDB MCI",
    "HW IDB MIP",
    "HW IDB MOP",
    "HW IDB Novell",
    "HW IDB NTP",
    "HW IDB Old TR",
    "HW IDB RSRB",
    "HW IDB SDLC",
    "HW IDB Serial",
    "HW IDB SPAN",
    "HW IDB SRB",
    "HW IDB TMS380",
    "HW IDB XNS",
    "HW IDB Throttled",
    "HW IDB Tunnel",


    "SW IDB Apollo",
    "SW IDB AppleTalk",
    "SW IDB BOOTP",
    "SW IDB CDP",
    "SW IDB CLNS",
    "SW IDB DECnet",
    "SW IDB IP",
    "SW IDB IP ARP",
    "SW IDB IP HOST",
    "SW IDB IP Multicast",
    "SW IDB LAT",
    "SW IDB Novell",
    "SW IDB NTP",
    "SW IDB RSRB",
    "SW IDB SPAN",
    "SW IDB XNS"
};

/*
 * Text names for known HW IDB sub-block types.
 * Add only names for the fixed sub-block types and add them in
 * an order which corresponds to their order in the HWIDB_SB_*
 * enum list in h/interface.h
 *
 * Do not add names for sub-blocks which will be allocated using
 * HWIDB_SB_DYNAMIC.
 */
static char *const hwidb_sb_names[HWIDB_SB_MAX] =
{
    "HWIDB SB ATM",
    "HWIDB SB CMNS",
    "HWIDB SB Dial Backup",
    "HWIDB SB Dial on Demand",
    "HWIDB SB FDDI",
    "HWIDB SB FR",
    "HWIDB SB HDLC",
    "HWIDB SB LAPB",
    "HWIDB SB MOP",
    "HWIDB SB NETBIOS",
    "HWIDB SB PPP",
    "HWIDB SB SDLC",
    "HWIDB SB Serial",
    "HWIDB SB SMDS",
    "HWIDB SB STUN",
    "HWIDB SB Token",
    "HWIDB SB X25"
};

/*
 * Text names for known HW IDB sub-block types.
 * Add only names for the fixed sub-block types and add them in
 * an order which corresponds to their order in the HWIDB_SB_*
 * enum list in h/interface.h
 *
 * Do not add names for sub-blocks which will be allocated using
 * SWIDB_SB_DYNAMIC.
 */
static char *const swidb_sb_names[SWIDB_SB_MAX] =
{
    "SWIDB SB Apollo",
    "SWIDB SB AppleTalk",
    "SWIDB SB ARP",
    "SWIDB SB Bridging",
    "SWIDB SB CDP",
    "SWIDB SB CLNS",
    "SWIDB SB DECnet",
    "SWIDB SB Hot Standby"
    "SWIDB SB IP",
    "SWIDB SB IPHost",
    "SWIDB SB IPRouting",
    "SWIDB SB IP Multicast",
    "SWIDB SB ISIS",
    "SWIDB SB ISO-IGRP",
    "SWIDB SB LAT",
    "SWIDB SB LEX",
    "SWIDB SB LLC2",
    "SWIDB SB Novell",
    "SWIDB SB NTP",
    "SWIDB SB OSPF",
    "SWIDB SB SMRP",
    "SWIDB SB SPAN",
    "SWIDB SB SSE",
    "SWIDB SB Vines",
    "SWIDB SB XNS"
};


/*
 * IDB List info function.
 * Called by the List Manager to print neat stuff about a list of IDB's
 * during the "show list" command.
 */
static boolean
idb_list_info (list_header *l_hdr, list_element *element, char *buffer)
{
    dyn_idblist_elt_t	*l_elt = (dyn_idblist_elt_t *)element;
    idbtype		*idb = NULL;
    hwidbtype		*hwidb = NULL;

    if (!(l_hdr && l_elt && buffer))
	return (FALSE);

    if (l_elt->idb_type == IDBTYPE_SW) {
	idb = l_elt->idb;
	sprintf(buffer, "SWIDB %s", idb->namestring);
	return (TRUE);
    } else if (l_elt->idb_type == IDBTYPE_HW) {
	hwidb = l_elt->idb;
	sprintf(buffer, "HWIDB %s", hwidb->hw_namestring);
	return (TRUE);
    }
    return (FALSE);
}

/*
 * idb_sb_info
 *
 * Called by the List Manager to print basic info about dynamic sub-block
 * entries. This is called for ONLY dynamic sub-block entries.
 */
static boolean
idb_dyn_sb_info (list_header *l_hdr, list_element *element, char *buffer)
{
    idb_sb_dyn_t	*dyn_sb = (idb_sb_dyn_t *)element;
    idbtype		*idb = NULL;
    hwidbtype		*hwidb = NULL;

    if (dyn_sb == NULL)
	return (FALSE);
    if (dyn_sb->sb.idb_type == IDBTYPE_SW) {
	idb = (idbtype *)dyn_sb->sb.idb_ptr;
	sprintf(buffer, "swidb %s, dyn SB at %08x, type %d",
		idb->namestring, dyn_sb->sb.user_ptr, dyn_sb->sb.type);
	return (TRUE);
    } else if (dyn_sb->sb.idb_type == IDBTYPE_HW) {
	hwidb = (hwidbtype *)dyn_sb->sb.idb_ptr;
	sprintf(buffer, "hwidb %s, dyn SB at %08x, type %d",
		hwidb->hw_namestring, dyn_sb->sb.user_ptr, dyn_sb->sb.type);
	return (TRUE);
    }
    return (FALSE);
}
/*
 * get_next_swidb_index
 * Get the next interface number
 */

static int get_next_swidb_index (void)
{
    return (next_swidb_index++);
}

/*
 * idb_get_next_hw_index
 * Get the next hardware interface index
 * Used for SNMP numbering - Also used by the cdbs
 */

int
idb_get_next_hw_index (void)
{
    return (next_hwidb_index++);
}

int
idb_get_current_hw_index (void)
{
    /* since this is primed to deliver the next index to the requestor
       subtract one */

    return (next_hwidb_index-1);
}

/*
 * idb_can_create_more
 * Find out whether any more interfaces can be created
 */

boolean
idb_can_create_more (void)
{
    return (next_swidb_index < MAX_INTERFACES);
}

/*
 * swidb_init_common
 *
 * Common initialization for software IDBs
 */
static void swidb_init_common (idbtype *swidb)
{
    /* Make sure IGRP doesn't divide by zero! */

    swidb->visible_bandwidth = swidb->visible_bandwidth_def = BANDWIDTH_SCALE;

    /*
     * Point each dynamically created sub-interface at the global
     * route/bridge indicators, or allocate a specific instance
     * initialized to bridge all rxtypes.
     */
    if (global_crb_enable) {

	ushort *rxtype_flags;

	rxtype_flags = malloc_named(SIZEOF_RXTYPE_FLAGS, "CRB rxtypes");
	if (rxtype_flags)
	    swidb->rxtype_flags = rxtype_flags;

    } else {
	swidb->rxtype_flags = global_rxtype_flags;
    }
}

/*
 * idb_init_all
 * Initialize the default contents of both hwidb and swidb.
 * Please add any idb content initialization code in this
 * function so that it will not break any code which relies 
 * on idb_init_all() to intialize default contents of both 
 * idbs.
 */
void idb_init_all (hwidbtype* hwidb, idbtype *swidb)
{
    idb_link_hw_sw(hwidb, swidb);
    idb_init_common(hwidb);
    swidb_init_common(swidb);
}

/*
 * idb_create
 * Create an IDB (hardware plus master software).
 * Puts the IDB in SRAM, if available.
 */
hwidbtype *
idb_create (void)
{
    hwidbtype *hwidb;
    idbtype *idb;

    if (!idb_can_create_more())
	return(NULL);
    hwidb = malloc_named_fast(sizeof(hwidbtype), hardware_idb);
    if (!hwidb) 
        return(NULL);

    idb = malloc_named(sizeof(idbtype), software_idb);
    if (!idb) {
	free(hwidb);
	return(NULL);
    }
    
    /*
     * After the successful memory allocation of both hwidb 
     * swidb, the function idb_init_all() should be called to
     * initialize the default content of both idbs. 
     * Please add any idb content initialization code in the
     * function idb_init_all() so that it will not break 
     * any code which relies on idb_init_all() to intialize
     * default contents of both idbs.
     */
    idb_init_all(hwidb, idb);
    return (hwidb);
}


/* link a hwidb and a swidb */
void
idb_link_hw_sw (hwidbtype *hwidb, idbtype *idb)
{
    idb->hwptr = hwidb;
    hwidb->firstsw = idb;
    hwidb->fast_rcvidb = idb;
    hwidb->output_if_hw = hwidb;
    idb->output_if = idb;
}

/*
 * idb_init_common
 * Initialize the things common to all hardware idbs, including base
 * assumptions in the first swidb allocated on a hwidb.
 */
void
idb_init_common (hwidbtype *hwidb)
{
    ushort	i;
    idbtype	*idb;
    list_header	*l_hdr;

    idb = hwidb->firstsw;
    hwidb->state_config = hwidb_generic_state_config;
    hwidb->fast_switch_type = FS_UNKNOWN;
    hwidb->input_qcount = 0;
    hwidb->input_defqcount = INPUTQ_DEFAULT;
    hwidb->holdqueue_out = 0;	/* no configured value */
    hwidb->typestring = hwidb_default_typestring;
    hwidb->carrierdelay = hwidb->carrierdelay_def = (2 * ONESEC);
    idb->if_number = get_next_swidb_index();
    idb->subif_state = SUBIF_STATE_UP;
    idb->subif_line_state = SUBIF_STATE_UP;
    idb->cache_version++;
    hwidb->datarate_interval = DATARATE_INTERVAL_DEF;
    hwidb->hub = NULL;
    hwidb->slot = MAXUINT;
    hwidb->slotunit = MAXUINT;
    hwidb->vc = NO_VC;
    hwidb->pak_backing_store = TRUE;	/* Enable by default */
    hwidb->queue_algorithm = QUEUE_FAIR_DEF;
    hwidb->old_queue_algorithm = QUEUE_FAIR_DEF;
    hwidb->si_counters = NULL;
    mgd_timer_init_leaf(&hwidb->carriertimer, &if_cstate_timer, 0, hwidb,
			TRUE);
    for (i = 0; i < HWIDB_SB_MAX; ++i) {
	hwidb->subblock.sb[i].user_ptr = NULL;
	hwidb->subblock.sb[i].type     = 0;
	hwidb->subblock.sb[i].idb_type = IDBTYPE_HW;
	hwidb->subblock.sb[i].usage    = 0;
    }
    for (i = 0; i < SWIDB_SB_MAX; ++i) {
	idb->subblock.sb[i].user_ptr = NULL;
	idb->subblock.sb[i].type     = 0;
	idb->subblock.sb[i].idb_type = IDBTYPE_SW;
	idb->subblock.sb[i].usage    = 0;
    }
    l_hdr = list_create(&hwidb->subblock.sb_dynamic, 0, "HW idb sb hdr",
			LIST_FLAGS_INTERRUPT_SAFE);
    if (l_hdr) {
	list_set_info(l_hdr, idb_dyn_sb_info);
	hwidb->subblock.sb_dynamic_id = HWIDB_SB_DYNAMIC + 1;
    }
    l_hdr = list_create(&idb->subblock.sb_dynamic, 0, "SW idb sb hdr",
			LIST_FLAGS_INTERRUPT_SAFE);
    if (l_hdr) {
	list_set_info(l_hdr, idb_dyn_sb_info);
	idb->subblock.sb_dynamic_id = SWIDB_SB_DYNAMIC + 1;
    }
}

/*
 * idb_create_subif
 * Create a sub-idb given a master IDB, and link it in.
 * This will link into both the swidbQ and the list for the IDB.
 */

idbtype *
idb_create_subif (idbtype *idb, int subidbnum)
{
    idbtype *subidb;
    idbtype *scan, *scanlast;
    char buffer[32];
    ushort	i;
    list_header	*l_hdr;
    leveltype level;

    if (idb == NULL)
	return (NULL);
    if (subidbnum == 0)			/* Illegal! */
	return (NULL);
    if (!idb_can_create_more())		/* Just too darned many */
	return (NULL);

    /* Allocate the memory. */

    subidb = malloc_named(sizeof(idbtype), software_idb);
    if (!subidb)
	return (NULL);

    /* Set up the basic fields. */

    subidb->hwptr = idb->hwptr;
    subidb->if_number = get_next_swidb_index();
    subidb->sub_number = subidbnum;
    subidb->subif_state = SUBIF_STATE_UP;
    subidb->subif_line_state = SUBIF_STATE_UP;
    subidb->cache_version++;
    subidb->output_if = subidb;
    (void) sprintf(buffer, "%s.%u", idb->namestring, subidbnum);
    setstring(&subidb->namestring, buffer);
    (void) sprintf(buffer, "%s.%u", idb->short_namestring, subidbnum);
    setstring(&subidb->short_namestring, buffer);
    subidb->delay = idb->delay;
    subidb->delay_def = idb->delay_def;
    subidb->manual_bandwidth = FALSE;
    subidb->manual_delay = FALSE;
    subidb->visible_bandwidth = idb->visible_bandwidth;
    subidb->visible_bandwidth_def = idb->visible_bandwidth_def;
    subidb->ip_routecache_same_def = idb->ip_routecache_same_def;
    subidb->tbridge_media = idb->tbridge_media;
    for (i = 0; i < SWIDB_SB_MAX; ++i) {
	subidb->subblock.sb[i].user_ptr = NULL;
	subidb->subblock.sb[i].type     = 0;
	subidb->subblock.sb[i].idb_type = IDBTYPE_SW;
	subidb->subblock.sb[i].usage    = 0;
    }
    l_hdr = list_create(&subidb->subblock.sb_dynamic, 0, idb->namestring,
			LIST_FLAGS_INTERRUPT_SAFE);
    if (l_hdr) {
	list_set_info(l_hdr, idb_dyn_sb_info);
	subidb->subblock.sb_dynamic_id = SWIDB_SB_DYNAMIC + 1;
    }
    /*
     * Point each dynamically created sub-interface at the global
     * route/bridge indicators, or allocate a specific instance
     * initialized to bridge all rxtypes.
     */
    if (global_crb_enable) {
	ushort *rxtype_flags;

	rxtype_flags = malloc_named(SIZEOF_RXTYPE_FLAGS, "CRB rxtypes");
	if (rxtype_flags)
	    subidb->rxtype_flags = rxtype_flags;
    } else {
	subidb->rxtype_flags = global_rxtype_flags;
    }

    /*
     * For the ATM interface, it is possible sub-interfaces could have
     * different mtu sizes depending on the application running on them.
     * To efficiently utilize the memory, for a newly created atm interface,
     * the mtu size is set to the default value, if the max mtu is
     * greater to the default one.
     */
    if (is_atm(subidb->hwptr)) {
	if (subidb->hwptr->maxmtu > subidb->hwptr->maxmtu_def) {
	    subidb->sub_mtu   = subidb->hwptr->maxmtu_def;
	} else {
	    subidb->sub_mtu   = subidb->hwptr->maxmtu;
	}
    } else {
	subidb->sub_mtu = subidb->hwptr->maxmtu;
    }

    reg_invoke_sw_idb_init(subidb, FALSE);
    reg_invoke_sw_idb_encaps_init(idb->hwptr->enctype, subidb, TRUE);
    queue_init(&subidb->tbridge_circuitQ, 0);


    /* Link it in. (in sub_number order) - firstsw doesn't have
     * a sub_number, so start with that one.
     */
    level = raise_interrupt_level(ALL_DISABLE);
    scanlast = idb;
    scan = idb->nextsub;
    while (scan && scan->sub_number < subidbnum) {
	scanlast = scan;
	scan = scan->nextsub;
    }
    scanlast->nextsub = subidb;
    subidb->nextsub = scan;

    /*
     * Insert it into the global list also - same place - after scanlast
     */
    insqueue(swidbQ, subidb, scanlast);

    reset_interrupt_level(level);
    return (subidb);
}

/*
 * idb_unlist
 *
 * This function is used to unthread the specified list header.
 * In the future, this function will likely grow into the function which
 * unthreads an IDB from all linked structures in preparation for hiding
 * or deletion.
 */
static void
idb_unlist (list_header *lhdr)
{
    if (lhdr) {
	if (LIST_VALID(lhdr)) {
	    while (!LIST_EMPTY(lhdr)) {
		(void)list_dequeue(lhdr);
	    }
	    list_destroy(lhdr);
	}
    }
}

/*
 * idb_free
 * Free an IDB pair.
 *
 * Assumes that there is no more than one SW idb attached to the 
 * specified HW idb. Checks to see if the HW/SW idb pair specified
 * are linked into idb lists or contain headers of active idb lists.
 */

void
idb_free (hwidbtype *hwidb)
{
    idbtype	*idb;

    if (!hwidb)
	return;
    idb = hwidb->firstsw;

    /*
     * When the IDB was initialized, two list header(s) were created
     * for the dynamic sub-block lists. This inserts an element into
     * the "list of all lists." We have to remove any elements on the
     * sub-block lists and then remove the dynamic sub-block list header
     * from the list of lists.
     */
    idb_unlist(&idb->subblock.sb_dynamic);
    idb_unlist(&hwidb->subblock.sb_dynamic);
    if (idb)
	free(idb);
    if (hwidb)
	free(hwidb);
}

/*
 * idb_default_ordering
 *
 * This is the default IDB lexical ordering function, used by all
 * "non-slotted" platforms. Slotted platforms will need to implement
 * a platform-specific IDB ordering.
 */
boolean idb_default_ordering (hwidbtype *new_idb, hwidbtype *old_idb)
{
    short	lex;

    if (!(new_idb && old_idb))
	return (FALSE);

    lex = strcmp(old_idb->name, new_idb->name);
    if (lex > 0)
	return (TRUE);
    if (lex == 0) {
	/*
	 * Name is the same.  We want to put the ones with VCs after
	 * the ones without.  That way we don't get 0 0:0 0:1 1
	 */
	if (old_idb->vc == NO_VC) {
	    if (new_idb->vc == NO_VC) {
		if (old_idb->unit > new_idb->unit)
		    return (TRUE);
	    }
	} else {
	    /*
	     * old_idb has a VC, new one doesn't.
	     * new_idb goes before new one
	     */
	    if (new_idb->vc == NO_VC)
		return (TRUE);
	    /*
	     * both have VC.  Compare unit, then VC number
	     */
	    if (old_idb->unit > new_idb->unit)
		return (TRUE);
	    if (old_idb->unit == new_idb->unit)
		if (old_idb->vc > new_idb->vc)
		    return (TRUE);
	}
    }
    return (FALSE);
}

/*
 * idb_slotted_ordering
 *
 * This is the IDB lexical ordering function for platforms with interface NICs
 * which plug into "slots"; or are daughtercards on the main IP's.
 *
 * If a particular slotted platform needs an inteface ordering function
 * which is especially perverse, then override the function pointer
 * assignment in your particular "src-<cpu>-<platform>/platform_<name>.c
 * file, NOT HERE.
 */
boolean idb_slotted_ordering (hwidbtype *new_idb, hwidbtype *old_idb)
{
    short	lex;

    /*
     * High end routine.
     * Compare slot, applique, vc, then name and unit number
     */
    if (old_idb->slot > new_idb->slot)
	return (TRUE);
    if (old_idb->slot == new_idb->slot) {
	if (old_idb->slotunit > new_idb->slotunit)
	    return (TRUE);
	if (old_idb->slotunit == new_idb->slotunit) {
	    if (old_idb->vc > new_idb->vc)
		return (TRUE);
	    if (old_idb->vc == new_idb->vc) {
		lex = strcmp(old_idb->name, new_idb->name);
		if (lex > 0)
		    return (TRUE);
		if (lex == 0) {
		    if (old_idb->unit > new_idb->unit)
			return (TRUE);
		}
	    }
	}
    }
    return (FALSE);
}

/*
 * idb_enqueue
 * Do the initial enqueue of a basic IDB onto the swidbQ and hwidbQ.
 *
 * idb_init_name should be called before this routine.
 *
 * For virtual interfaces, the slot and slotunit field should not be
 * set - they are initialized to MAXUINT in idb_init_common.  This
 * assures that for the high end, they will appear at the end of the
 * interface list.
 */
void
idb_enqueue (hwidbtype *new_idb)
{
    idbtype *idb, *last_idb;
    hwidbtype *thwidb, *last_hwidb;
    leveltype level;

    last_idb = NULL;
    last_hwidb = NULL;
    level = raise_interrupt_level(ALL_DISABLE);

    FOR_ALL_SWIDBS(idb) {
	if (platform_interface_ordering(new_idb, idb->hwptr))
	    break;
	last_idb = idb;
    }

    FOR_ALL_HWIDBS(thwidb) {
	if (platform_interface_ordering(new_idb, thwidb))
	    break;
	last_hwidb = thwidb;
    }
    /*
     * Insert onto queues.  If last_idb or last_hwidb are NULL, we
     * will go on to the beginning of the queue
     */
    insqueue(swidbQ, new_idb->firstsw, last_idb);
    insqueue(hwidbQ, new_idb, last_hwidb);

    reset_interrupt_level(level);
}

/*
 * idb_unlink
 * Unlink and IDB pair from all relevant lists.  Does not free the
 * space, because it's never safe to free an IDB pointer.
 */
void idb_unlink (hwidbtype *hwidb)
{
    idbtype	*swidb;
    
    p_unqueue(hwidbQ, hwidb);
    FOR_ALL_SWIDBS_ON_HW (hwidb, swidb)
	p_unqueue(swidbQ, swidb);
}

#define SLOT_PORT_DELIMITER '/'
#define	PORT_VC_DELIMITER ':'

/*
 * idb_name_format:
 * Return the format that idb names should be in for this platform
 */
int idb_name_format (void)
{
    switch (cpu_type) {
    
    case CPU_RP1:
    case CPU_RSP:
	return(EXTENDED_SLOTTED_IDB_NAMES);
    case CPU_VIP:
    case CPU_C7100:
	return(SLOTTED_IDB_NAMES);
    default:
	return(NONSLOTTED_IDB_NAMES);
    }
}

/*
 * idb_slotted_names:
 * Create idb name strings for boxes that are slotted
 */
static void idb_slotted_names (int fmt, char *buffer, idbtype *swidb)
{
    hwidbtype *hwidb = swidb->hwptr;
    int pa_bay, pa_unit;
    char nulls[2] = { 0x00, 0x00 };
    char *name;

    if (hwidb->name)
	name = hwidb->name;
    else 
	name = nulls;

    switch (fmt) {
    case SLOTTED_IDB_NAMES:
	if (hwidb->vc != NO_VC) {
	    sprintf(buffer, "%s%u%c%u%c%d", name,
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit, PORT_VC_DELIMITER,
		    hwidb->vc);
	} else {
	    sprintf(buffer, "%s%u%c%u", name,
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit);
	}
	setstring(&swidb->namestring, buffer);
	hwidb->hw_namestring = swidb->namestring;
	if (hwidb->vc != NO_VC) {
	    sprintf(buffer, "%c%c%u%c%u%c%d",
		    name[0], name[1],
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit, PORT_VC_DELIMITER,
		    hwidb->vc);
	} else {
	    sprintf(buffer, "%c%c%u%c%u", name[0], name[1],
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit);
	}
	setstring(&swidb->short_namestring, buffer);
	hwidb->hw_short_namestring = swidb->short_namestring;
	break;

    case EXTENDED_SLOTTED_IDB_NAMES:
	if (is_vip(hwidb)) {

	    /*
	     * Gotta play with the pa-bay to compute the right slotunit.
	     */
	    if (hwidb->slotunit >= PA_VIRTUAL_SLOT_BOUNDARY) {
	    	pa_bay = PA_BAY_1;
	        pa_unit = hwidb->slotunit - PA_VIRTUAL_SLOT_BOUNDARY;
	    } else {
	    	pa_bay = PA_BAY_0;
		pa_unit = hwidb->slotunit;
	    }
	    

            if (hwidb->vc != NO_VC) {
                sprintf(buffer, "%s%u%c%d%c%d%c%d", name,
                    hwidb->slot, SLOT_PORT_DELIMITER,
                    pa_bay, SLOT_PORT_DELIMITER,
                    pa_unit, PORT_VC_DELIMITER,
                    hwidb->vc);
            } else {
                sprintf(buffer, "%s%u%c%d%c%d", name,
                    hwidb->slot, SLOT_PORT_DELIMITER,
                    pa_bay, SLOT_PORT_DELIMITER,
                    pa_unit);
            }
            setstring(&swidb->namestring, buffer);
            hwidb->hw_namestring = swidb->namestring;
            if (hwidb->vc != NO_VC) {
                sprintf(buffer, "%c%c%u%c%d%c%d%c%d",
                    name[0], name[1],
                    hwidb->slot, SLOT_PORT_DELIMITER,
                    pa_bay, SLOT_PORT_DELIMITER,
                    pa_unit, PORT_VC_DELIMITER,
                    hwidb->vc);
            } else {
                 sprintf(buffer, "%c%c%u%c%d%c%d", name[0], name[1],
                    hwidb->slot, SLOT_PORT_DELIMITER,
		    pa_bay, SLOT_PORT_DELIMITER,
                    pa_unit);
            }
            setstring(&swidb->short_namestring, buffer);
            hwidb->hw_short_namestring = swidb->short_namestring;

	} else {

	    /*
	     * Classical SLOTTED scheme for backward compatibility.
	     */

	    if (hwidb->vc != NO_VC) {
	    	sprintf(buffer, "%s%u%c%u%c%d", name,
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit, PORT_VC_DELIMITER,
		    hwidb->vc);
	    } else {
	    	sprintf(buffer, "%s%u%c%u", name,
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit);
	    }
	    setstring(&swidb->namestring, buffer);
	    hwidb->hw_namestring = swidb->namestring;
	    if (hwidb->vc != NO_VC) {
	    	sprintf(buffer, "%c%c%u%c%u%c%d",
		    name[0], name[1],
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit, PORT_VC_DELIMITER,
		    hwidb->vc);
	    } else {
	   	 sprintf(buffer, "%c%c%u%c%u", name[0], name[1],
		    hwidb->slot, SLOT_PORT_DELIMITER,
		    hwidb->slotunit);
	    }
	    setstring(&swidb->short_namestring, buffer);
	    hwidb->hw_short_namestring = swidb->short_namestring;
	}
	break;
    }
}
    
/*
 * idb_init_names
 * Set up interface name strings.
 */
void idb_init_names (idbtype *swidb, boolean always_short)
{
    char buffer[128];
    hwidbtype *hwidb = swidb->hwptr;
    int name_format;
    char nulls[2] = { 0x00, 0x00 };
    char *name;

    if (swidb->namestring)
	return;

    if (hwidb->name)
	name = hwidb->name;
    else 
	name = nulls;

    name_format = idb_name_format();
    if (name_format != NONSLOTTED_IDB_NAMES && !always_short) {
	idb_slotted_names(name_format, buffer, swidb);
	return;
    }
    if (hwidb->vc != NO_VC) {
	sprintf(buffer, "%s%d%c%d", name, hwidb->unit,
		PORT_VC_DELIMITER, hwidb->vc);
    } else
        sprintf(buffer, "%s%d", name, hwidb->unit);
    setstring(&swidb->namestring, buffer);
    hwidb->hw_namestring = swidb->namestring;
    
    if (hwidb->vc != NO_VC) {
	sprintf(buffer, "%c%c%d%c%d", name[0], name[1],
		hwidb->unit,
		PORT_VC_DELIMITER, hwidb->vc);
    } else
        sprintf(buffer, "%c%c%d", name[0], name[1], hwidb->unit);
    setstring(&swidb->short_namestring, buffer);
    hwidb->hw_short_namestring = swidb->short_namestring;
}

/*
 * idb_first_init
 * Call this to set up data structures *before* we starting looking
 * around for actual interface hardware.
 */

void
idb_first_init (void)
{
    ushort	i;
    list_header	*l_hdr;


    nnets = 0;
    nethernets = 0;			/* no ethernet interfaces yet */
    nserialnets = 0;			/* no serial interfaces yet */
    nTRs = 0;				/* no tokenring interfaces yet */
    nbrinets = 0;                       /* no bri interfaces yet */
    nprinets = 0;                       /* no pri interfaces yet */
    is_pri_supp = 0;                    /* don't support pri yet */
    nfddinets = 0;                      /* no fddi interfaces yet */
    nhssinets = 0;                      /* no HSSI interfaces yet */
    nposnets = 0;
    npcbusnets = 0;                     /* no PCbus interfaces yet */
    nfethernets = 0;                    /* no FastEthernet interfaces yet */
    netherhubports = 0;                 /* no ethernet hub ports yet */
    memset(&idb_si_counter_zeros, 0, sizeof(idb_si_counter_zeros));
    mgd_timer_init_parent(&if_cstate_timer, NULL);

    next_swidb_index = 1;
    next_hwidb_index = 1;
    queue_init(swidbQ, 0);
    queue_init(hwidbQ, 0);

    idb_sb_chunk = chunk_create(sizeof(idb_sb_dyn_t), 200, 0,
				NULL, 0,
				"IDB SB Chunks");
    idb_list_hdr_chunk = chunk_create(sizeof(dyn_idblist_hdr_t), 200, 0,
				      NULL, 0,
				      "IDB List Header Chunks");

    /* 
     * The number of elements allocated for the following chunk could
     * be scaled better if the backplane information were available.
     * idb_first_init is invoked in system_init before subsystem
     * initialization. The size should support a fair large number
     * interfaces. If the chunk could grow from interrupt
     * level, then the chunk creation would give CHUNK_FLAGS_DYNAMIC
     * flag instead.
     * If the chunk runs out at run-time, the size has to be increased!!
     */
    idb_list_elt_chunk = chunk_create(sizeof(dyn_idblist_elt_t), 
				      MAX_INTERFACES * 3, 0,
				      NULL, 0,
				      "IDB List Element Chunks");
    idb_notify_chunk = chunk_create(sizeof(idb_notify_t), 200, 0,
				    NULL, 0,
				    "IDB Notification Chunks");

    for (i = 0; i < IDBLIST_MAX; ++i) {
	l_hdr = list_create(&idblist_headers.fixed_list[i], 0,
			    idblist_names[i], LIST_FLAGS_INTERRUPT_SAFE);
	if (l_hdr)
	    list_set_info(l_hdr, idb_list_info);
    }
    /*
     * Create the list header for the list of list headers for
     * dynamic IDB lists.
     */
    l_hdr = list_create(&idblist_headers.dynamic_list, 0,
			"IDB List Dynamic Header", LIST_FLAGS_INTERRUPT_SAFE);
    if (l_hdr)
	list_set_info(l_hdr, idb_list_info);
    idblist_headers.dynamic_id = IDBLIST_DYNAMIC + 1;
    vidb_init();		/* init the virtual interfaces */
    nullidb_init();		/* initialize the null idb */
}

/*
 * idb_final_hw_init
 * Common interface initialization done after all idb's have been created
 */

void
idb_final_hw_init (hwidbtype *hwidb)
{
    hwidb->keep_period = KEEPALIVE_DEFAULT;
    hwidb->keep_count = 0;
    if (!hwidb->hw_if_index)
	hwidb->hw_if_index = idb_get_next_hw_index();
    if (!hwidb->snmp_if_index)
	hwidb->snmp_if_index = reg_invoke_ifmib_register_hwidb(hwidb);
    hwidb->newproto = -1;    /* line state is neither TRUE nor FALSE */
}

void
idb_final_sw_init (idbtype *idb)
{
    if (!idb->namestring) {
	idb_init_names(idb, FALSE);
    }

    /*
     * Set snmp index value of main swidb to be the same as hwidb.
     * This needs to be changed once supporting swidb registered w/ snmp
     * and wants to differenciate between main swidb and hwidb for snmp.
     */
    if (idb->hwptr->firstsw == idb)
	idb->snmp_if_index = idb->hwptr->snmp_if_index;
}

void
idb_final_init (void)
{
    hwidbtype *hwidb;
    idbtype *idb;

    /*
     * Add in the new number of physical interfaces
     */
    nnets += nethernets + nserialnets + nTRs + nfddinets + 
	      nhssinets + nbrinets + natmnets + npcbusnets + nposnets;

    FOR_ALL_HWIDBS(hwidb) {
	idb_final_hw_init(hwidb);
    }

    FOR_ALL_SWIDBS(idb) {
	idb_final_sw_init(idb);
    }

    /*
     * Execute final initialization requests
     */
    reg_invoke_if_final_init();

    /*
     * Register some functions
     */
    reg_add_create_idb(loopback_parser_createidb, "loopback_parser_createidb");
}

/*
 * interface_summary
 *
 * Print summary of interface types
 */

void
idb_print_summary (void)
{
    ulong tmp_count;

    if ((tmp_count = (nethernets - nfethernets)))
        printf("%u %s interface(s)\n", tmp_count, "Ethernet/IEEE 802.3");
    if (nfethernets)
        printf("%u %s interface(s)\n", nfethernets, "FastEthernet/IEEE 802.3"); 
    if (netherhubports)
        printf("%u %s repeater port(s)\n", netherhubports, "Ethernet/IEEE 802.3");
    if (nTRs)
        printf("%u %s interface(s)\n", nTRs, "Token Ring/IEEE 802.5");
    if ((tmp_count = (nserialnets - nas_serialnets)))
        printf("%u %s network interface(s)\n", tmp_count, "Serial");
    if (nas_serialnets)
        printf("%u %s network interface(s)\n", nas_serialnets, 
                                    "Low-speed serial(sync/async)");
    if (nhssinets)
        printf("%u %s network interface(s)\n", nhssinets, "HSSI");
    if (nbrinets)
        printf("%u %s interface(s)\n", nbrinets, "ISDN Basic Rate");
    if (nfddinets)
        printf("%u %s network interface(s)\n", nfddinets, "FDDI");
    if (natmnets)
        printf("%u %s network interface(s)\n", natmnets, "ATM");
    if (nchannelnets)
        printf("%u %s interface(s)\n", nchannelnets, "IBM channel");
    if (npcbusnets)
        printf("%u %s interface(s)\n", npcbusnets, "PCbus");
    if ((tmp_count = (nttylines -  nas_serialnets - ndummyttylines)))
        printf("%u terminal line(s)\n", tmp_count);
    if ((tmp_count = (ne1ports + nse1ports)))
        printf("%u Channelized %c1/PRI port(s)\n", tmp_count, 'E');
    if ((tmp_count = (nt1ports + nst1ports)))
        printf("%u Channelized %c1/PRI port(s)\n", tmp_count, 'T');
    if (nposnets)
        printf("%u %s network interface(s)\n", nposnets, "Packet over Sonet");
}

/*
 * idb_clear_dangerous
 *
 * This totally bogus code was located two places in the router and was
 * turning the set of hwidb carrier timers into a circular list.  It has
 * been centralized here and this one problem fixed, but this is likely
 * to be causing may other problems and memory leaks.  This routine
 * should NEVER, EVER, EVER, EVER be referenced from any place other than
 * the two existing calls.  Calling memset to zero on an idb is a heinous
 * practice and should be eliminated as soon as is humanly possible.
 *
 */
void idb_clear_dangerous (hwidbtype *hwidb)
{
    idbtype *swidb;

    swidb = hwidb->firstsw;
    mgd_timer_stop(&hwidb->carriertimer);
    if (hwidb->fair_queue_head) {
	free(hwidb->fair_queue_head);
    }   
    memset(hwidb, 0, sizeof(*hwidb));
    memset(swidb, 0, sizeof(*swidb));
    hwidb->firstsw = swidb;
}
/***************************************************************************
 *
 *		       IDB Sub-block functions.
 *
 ***************************************************************************/

/* 
 * media_idb_enqueue
 * enqueue idb to the media queue, also takes care of subif idbs
 * (important for oir applications)
 */
void media_idb_enqueue (hwidbtype *hwidb)
{
  if (hwidb) {
    idbtype *swidb;
    reg_invoke_media_hwidb_enqueue_private_list(hwidb);
    
    idb_enqueue(hwidb);

    swidb = hwidb->firstsw;
    while(swidb->nextsub) {
      insqueue(swidbQ, swidb->nextsub, swidb);
      swidb = swidb->nextsub;
    }
  }
}
/*
 * idb_fill_sb
 *
 * A little inline to fill in the idb sub-block.
 */
static inline void
idb_fill_sb (idb_sb_t *sb, void *ptr, void *idb, ushort itype, ushort type)
{
    sb->user_ptr = ptr;
    sb->idb_ptr  = idb;
    sb->idb_type = itype;
    sb->type     = type;
}

/*
 * idb_sb_order
 *
 * list ordering function called by List Manager while inserting
 * dynamically allocated IDB sub-blocks into their lists. Used for both
 * HW and SW idb dynamic sub-blocks.
 */
static int 
idb_sb_order (void *runner_arg, void *sb_arg)
{
    idb_sb_dyn_t	*runner = runner_arg;
    idb_sb_dyn_t	*new_sb = sb_arg;

    if (runner == NULL) {
	return(0);
    }
    return (runner->sb.type - new_sb->sb.type);
}

/*
 * idb_add_swsb		-- add a SW idb sub-block pointer.
 *
 * Adds a sub-block pointer passed by client and set usage to 0.
 *
 */
boolean
idb_add_swsb (struct idbtype_ *idb, swidb_sb_t *type_i, void *userval)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    swidb_sb_t		type_o = 0;	/* the type we are going to ass'n */

    if ((idb == NULL) ||  (*type_i > SWIDB_SB_DYNAMIC))
	return (FALSE);
    if (*type_i < SWIDB_SB_MAX) {
	type_o = *type_i;
	idb_fill_sb(&idb->subblock.sb[type_o],userval,idb,IDBTYPE_SW,type_o);
	idb->subblock.sb[type_o].usage = 0;
    } else if (*type_i == SWIDB_SB_DYNAMIC) {
	type_o = idb->subblock.sb_dynamic_id++;
	idb_sb = chunk_malloc(idb_sb_chunk);
	if (idb_sb == NULL)
	    return (FALSE);
	idb_fill_sb(&idb_sb->sb, userval, idb, IDBTYPE_SW, type_o);
	idb_sb->sb.usage = 0;
	if (!list_insert(&idb->subblock.sb_dynamic,
			 (list_element *)idb_sb, (void *)type_o,
			 idb_sb_order)) {
	    return (FALSE);
	}
    } else {
	return (FALSE);
    }
    *type_i = type_o;			/* return the assigned type */
    return (TRUE);
}

/*
 * idb_add_hwsb		-- add a HW idb sub-block pointer.
 *
 * Adds a sub-block pointer passed by client and set usage to 0.
 *
 */
boolean
idb_add_hwsb (struct hwidbtype_ *idb, hwidb_sb_t *type_i, void *userval)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    hwidb_sb_t		type_o = 0;	/* the type we are going to ass'n */

    if ((idb == NULL) ||  (*type_i > HWIDB_SB_DYNAMIC))
	return (FALSE);
    if (*type_i < HWIDB_SB_MAX) {
	type_o = *type_i;
	idb_fill_sb(&idb->subblock.sb[type_o],userval,idb,IDBTYPE_HW,type_o);
	idb->subblock.sb[type_o].usage = 0;
    } else if (*type_i == HWIDB_SB_DYNAMIC) {
	type_o = idb->subblock.sb_dynamic_id++;
	idb_sb = chunk_malloc(idb_sb_chunk);
	if (idb_sb == NULL)
	    return (FALSE);
	idb_fill_sb(&idb_sb->sb, userval, idb, IDBTYPE_HW, type_o);
	idb_sb->sb.usage = 0;
	if (!list_insert(&idb->subblock.sb_dynamic,
			 (list_element *)idb_sb, (void*)type_o,
			 idb_sb_order)) {
	    return (FALSE);
	}
    } else {
	return (FALSE);
    }
    *type_i = type_o;			/* return the assigned type */
    return (TRUE);
}

/*
 * idb_delete_swsb
 *
 * Delete a sub-block from a software IDB.
 *
 * If the usage count for the sub-block is non-zero, then return FALSE.
 */
boolean
idb_delete_swsb (struct idbtype_ *idb, swidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */

    if (idb == NULL)
	return (FALSE);
    if (type < SWIDB_SB_MAX) {
	if (idb->subblock.sb[type].usage > 0)
	    return (FALSE);
	idb->subblock.sb[type].user_ptr = NULL;
	idb->subblock.sb[type].type = 0;
	idb->subblock.sb[type].usage = 0;
	return (TRUE);
    } else if (type > SWIDB_SB_DYNAMIC) {
	idb_sb = list_remove(&idb->subblock.sb_dynamic, NULL, (void*)type);
	if (idb_sb != NULL) {
	    chunk_free(idb_sb_chunk, idb_sb);
	} else {
	    return (FALSE);
	}
    }
    return (TRUE);
}

/*
 * idb_delete_hwsb
 *
 * Delete a sub-block from a hardware IDB.
 *
 * If the usage count for the sub-block is non-zero, then return FALSE.
 */
boolean
idb_delete_hwsb (struct hwidbtype_ *idb, hwidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */

    if (idb == NULL)
	return (FALSE);
    if (type < HWIDB_SB_MAX) {
	if (idb->subblock.sb[type].usage > 0)
	    return (FALSE);
	idb->subblock.sb[type].user_ptr = NULL;
	idb->subblock.sb[type].type = 0;
	idb->subblock.sb[type].usage = 0;
	return (TRUE);
    } else if (type > HWIDB_SB_DYNAMIC) {
	idb_sb = list_remove(&idb->subblock.sb_dynamic, NULL, (void*)type);
	if (idb_sb != NULL) {
	    chunk_free(idb_sb_chunk, idb_sb);
	} else {
	    return (FALSE);
	}
    }
    return (TRUE);
}

/* 
 * idb_get_swsb
 *
 * Obtain a pointer to the sub-block without incrementing usage count.
 */
void *
idb_get_swsb (struct idbtype_ const *idb, swidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    list_element	*runner;

    if (idb == NULL)
	return (NULL);
    if (type < SWIDB_SB_MAX) {
	return (idb->subblock.sb[type].user_ptr);
    } else if (type > SWIDB_SB_DYNAMIC) {
	FOR_ALL_ELEMENTS_IN_LIST(&idb->subblock.sb_dynamic, runner){
	    idb_sb = (idb_sb_dyn_t *)runner;
	    if (idb_sb->sb.type == type)
		return (idb_sb->sb.user_ptr);
	}
    }
    return (NULL);
}

/* 
 * idb_get_hwsb
 *
 * Obtain a pointer to the sub-block without incrementing usage count.
 */
void *
idb_get_hwsb (struct hwidbtype_ *idb, hwidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    list_element	*runner;

    if (idb == NULL)
	return (NULL);
    if (type < HWIDB_SB_MAX) {
	return (idb->subblock.sb[type].user_ptr);
    } else if (type > HWIDB_SB_DYNAMIC) {
	FOR_ALL_ELEMENTS_IN_LIST(&idb->subblock.sb_dynamic, runner){
	    idb_sb = (idb_sb_dyn_t *)runner;
	    if (idb_sb->sb.type == type)
		return (idb_sb->sb.user_ptr);
	}
    }
    return (NULL);
}

/*
 * idb_use_swsb
 *
 * Obtain a pointer to a sub-block, incrementing the usage count.
 * (Later, the usage count might be a semaphore.)
 */
void *
idb_use_swsb (struct idbtype_ *idb, swidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    list_element	*runner;

    if (idb == NULL)
	return (NULL);
    if (type < SWIDB_SB_MAX) {
	idb->subblock.sb[type].usage++;
	return (idb->subblock.sb[type].user_ptr);
    } else if (type > SWIDB_SB_DYNAMIC) {
	FOR_ALL_ELEMENTS_IN_LIST(&idb->subblock.sb_dynamic, runner){
	    idb_sb = (idb_sb_dyn_t *)runner;
	    if (idb_sb->sb.type == type) {
		idb_sb->sb.usage++;
		return (idb_sb->sb.user_ptr);
	    }
	}
    }
    return (NULL);
}

/*
 * idb_use_hwsb
 *
 * Obtain a pointer to a sub-block, incrementing the usage count.
 * (Later, the usage count might be a semaphore.)
 */
void *
idb_use_hwsb (struct hwidbtype_ *idb, hwidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    list_element	*runner;

    if (idb == NULL)
	return (NULL);
    if (type < HWIDB_SB_MAX) {
	idb->subblock.sb[type].usage++;
	return (idb->subblock.sb[type].user_ptr);
    } else if (type > HWIDB_SB_DYNAMIC) {
	FOR_ALL_ELEMENTS_IN_LIST(&idb->subblock.sb_dynamic, runner){
	    idb_sb = (idb_sb_dyn_t *)runner;
	    if (idb_sb->sb.type == type) {
		idb_sb->sb.usage++;
		return (idb_sb->sb.user_ptr);
	    }
	}
    }
    return (NULL);
}

/*
 * idb_release_swsb
 *
 * Decrement the usage counter on a idb sub-block. 
 * (Later, the usage count might be a semaphore.)
 */
boolean
idb_release_swsb (struct idbtype_ *idb, swidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    list_element	*runner;

    if (idb == NULL)
	return (FALSE);
    if (type < SWIDB_SB_MAX) {
	if (idb->subblock.sb[type].usage >= 1) {
	    idb->subblock.sb[type].usage--;
	    return (TRUE);
	} else {
	    return (FALSE);
	}
    } else if (type > SWIDB_SB_DYNAMIC) {
	FOR_ALL_ELEMENTS_IN_LIST(&idb->subblock.sb_dynamic, runner) {
	    idb_sb = (idb_sb_dyn_t *) runner;
	    if (idb_sb->sb.type == type) {
		if (idb_sb->sb.usage >= 1) {
		    idb_sb->sb.usage--;
		    return (TRUE);
		} else {
		    return (FALSE);
		}
	    }
	}
    }
    return (FALSE);
}

/*
 * idb_release_hwsb
 *
 * Decrement the usage counter on a idb sub-block. 
 * (Later, the usage count might be a semaphore.)
 */
boolean
idb_release_hwsb (struct hwidbtype_ *idb, hwidb_sb_t type)
{
    idb_sb_dyn_t	*idb_sb;	/* dynamic IDB sub-block */
    list_element	*runner;

    if (idb == NULL)
	return (FALSE);
    if (type < HWIDB_SB_MAX) {
	if (idb->subblock.sb[type].usage >= 1) {
	    idb->subblock.sb[type].usage--;
	    return (TRUE);
	} else {
	    return (FALSE);
	}
    } else if (type > HWIDB_SB_DYNAMIC) {
	FOR_ALL_ELEMENTS_IN_LIST(&idb->subblock.sb_dynamic, runner) {
	    idb_sb = (idb_sb_dyn_t *) runner;
	    if (idb_sb->sb.type == type) {
		if (idb_sb->sb.usage >= 1) {
		    idb_sb->sb.usage--;
		    return (TRUE);
		} else {
		    return (FALSE);
		}
	    }
	}
    }
    return (FALSE);
}

/*
 * IDB List functions.
 *
 * IDB lists are created by software which previously would walk
 * the list of "all SW idb's" or "all HW idb's" and then check
 * some field on the IDB to see if the software functionality in
 * question was enabled on that IDB.
 *
 * Obviously, this becomes tedious and time consuming as we add more
 * and more interfaces to the router. So the following routines are
 * used to manage private lists of interface descriptors.
 *
 * By using these functions, two things can be eliminated:
 * 1. The "idb->foobar_enabled" flag, which is typically checked
 *    in the middle of a loop like thus:
 *
 *	FOR_ALL_SWIDBS(idb) {
 *		if (idb->foobar_enabled) {
 *			[ Do something here ]
 *		}
 *	}
 *
 * 2. The excess CPU overhead to find the half-dozen interfaces out
 *    of several dozen interfaces in the box which have your protocol
 *    enabled. This is especially useful on a router with lots of 
 *    sub-interfaces or lots of chanellized interfaces on the box.
 */


/*
 * get_list_header
 *
 * Grungy little inline to obtain a IDB list header given the IDB list ID.
 */
static inline list_header *
get_list_header (idblist_t type)
{
    list_header		*l_hdr = NULL;
    dyn_idblist_hdr_t	*dyn_header;
    list_element	*runner;

    if (type < IDBLIST_MAX) {
	l_hdr = &idblist_headers.fixed_list[type];
    } else {
	FOR_ALL_DATA_IN_LIST(&idblist_headers.dynamic_list,runner,dyn_header) {
	    if (dyn_header->type == type) {
		l_hdr = &dyn_header->header;
		break;
	    }
	}
    }
    return (l_hdr);
}

/*
 * get_list_header_link
 *
 * Grungy little inline to obtain a dynamic IDB list header link
 * given the IDB list ID.
 */
static inline dyn_idblist_hdr_t *
get_list_header_link (idblist_t type)
{
    dyn_idblist_hdr_t	*dyn_header;
    list_element	*runner;

    if (type < IDBLIST_MAX)
	return(NULL);

    FOR_ALL_DATA_IN_LIST(&idblist_headers.dynamic_list,runner,dyn_header) {
	if (dyn_header->type == type) {
	    break;
	}
    }
    return (dyn_header);
}

/*
 * idb_create_list
 *
 * Create a private IDB list. (for either SW or HW idb's)
 *
 * typein -- 	the type ID for the user of the list. If
 *		your application isn't listed in interface.h,
 *		then you should specify IDBLIST_DYNAMIC and an ID
 *		will be assigned to you.
 *
 * typeout --	the type ID assigned to you and which you should then
 *		pass into all future idb_ functions.
 */
boolean
idb_create_list (idblist_t typein, ulong *typeout)
{
    dyn_idblist_hdr_t *dynlisthdr;

    if (typeout == NULL || typein > IDBLIST_DYNAMIC)
	return (FALSE);

    /*
     * If the type specified is one of the statically defined ones,
     * we already have an in-place list header which has been previously
     * initialized.
     */
    if (typein < IDBLIST_MAX) {
        *typeout = typein;
        return (TRUE);
    }
    *typeout = 0;
    if (typein == IDBLIST_DYNAMIC) {
	/*
	 * The caller wants us to create a dynamic list.
	 * Get a list header
	 */
	dynlisthdr = chunk_malloc(idb_list_hdr_chunk);
	if (dynlisthdr == NULL)
	    return (FALSE);
        memset(dynlisthdr, 0, sizeof(dyn_idblist_hdr_t));
        dynlisthdr->type = idblist_headers.dynamic_id;
	dynlisthdr->link.data = dynlisthdr;
	if (list_create(&dynlisthdr->header, 0, "Dyn. IDB List Hdr",
			LIST_FLAGS_INTERRUPT_SAFE) == NULL) {
            chunk_free(idb_list_hdr_chunk, dynlisthdr);
	    return (FALSE);
	}
	if (!list_enqueue(&idblist_headers.dynamic_list,
			  &dynlisthdr->link, dynlisthdr)) {
	    buginf("\nCREATE_LIST: list_insert failed");
            chunk_free(idb_list_hdr_chunk, dynlisthdr);
	    return (FALSE);
	}
	*typeout = idblist_headers.dynamic_id++;
    }
    return (TRUE);
}
/*
 * idb_report_create_error
 */
void idb_report_create_error (char *str)
{
    buginf("%%Can't create %s idb list\n", str);
}

/*
 * idb_destroy_list
 *
 * Destroy an IDB list. If there are elements still on the list,
 * they're history. Toast. Gone.
 * 
 * If the list was dynamically created, then we remove the header
 * from the list of dynamically created list headers as well.
 */
boolean
idb_destroy_list (idblist_t typein)
{
    list_element	*runner;
    list_header		*l_hdr = NULL;

    if (typein < IDBLIST_MAX) {
	l_hdr = &idblist_headers.fixed_list[typein];
    } else if (typein > IDBLIST_DYNAMIC) {
	l_hdr = get_list_header(typein);
    }

    if (l_hdr == NULL)
	return (FALSE);

    /*
     * Check and empty the list first.
     */
    if (!LIST_EMPTY(l_hdr)) {
	FOR_ALL_ELEMENTS_IN_LIST(l_hdr, runner) {
	    list_remove(l_hdr, runner, NULL);
	    chunk_free(idb_list_elt_chunk, runner);
	}
    }

    /*
     * Fixed list headers are pre-allocated and initialized in
     * idb_first_init, and must not be freed or re-initialized.
     * So we free only dynamic list headers in this function.
     */
    if (typein > IDBLIST_DYNAMIC) {

	dyn_idblist_hdr_t *d_hdr;

	d_hdr = get_list_header_link(typein);
	if (NULL == d_hdr)
	    return(FALSE);
	if (list_remove(&idblist_headers.dynamic_list, &d_hdr->link, NULL) !=
	    NULL) {
	    chunk_free(idb_list_hdr_chunk, d_hdr);
	} else {
	    return(FALSE);
	}
    }

    return (TRUE);
}

/*
 * idb_get_listhead
 *
 * Return a pointer to the listhead for the specified list type.
 * If no such list type exists, return NULL. This is function is
 * for public consumption, if required.
 */
list_header *
idb_get_listhead (idblist_t type)
{
    return (get_list_header(type));
}


/*
 * swidb_list_order
 *
 * This function is called by the List Manager to determine at what
 * position to insert an element into a list.
 *
 * This function will keep the ordering of sw IDB's in the list
 * by if_number, preserving the ordering one would expect to see
 * from FOR_ALL_SWIDBS().
 */
static int 
swidb_list_order (void *runner_arg, void *element_arg)
{
    idbtype	*runner  = runner_arg;
    idbtype	*element = element_arg;

    if (runner == NULL){
	return (0);
    }
    return (runner->if_number - element->if_number);
}

/*
 * Add a SW IDB to a previously established IDB list.
 *
 * In order to not break any assumptions that higher-level software
 * might make about lists of IDB's, we must insert IDB's into the list
 * in unit-number order.
 */
boolean
idb_add_swidb_to_list (struct idbtype_ *idb, idblist_t typein)
{
    list_header		*l_hdr;
    dyn_idblist_elt_t	*idb_list_elt;
    boolean		ret;

    if (typein == IDBLIST_MAX || typein < SWIDBLIST_MIN)
	return (FALSE);

    l_hdr = get_list_header(typein);
    if (l_hdr == NULL)
	return (FALSE);
    /*
     * We now have the correct list header. We now walk the list
     * to make sure that we insert the IDB pointer at the correct point
     * in the list.
     */
    idb_list_elt = chunk_malloc(idb_list_elt_chunk);
    if (idb_list_elt == NULL)
        return (FALSE);

    idb_list_elt->type     = typein;
    idb_list_elt->idb_type = IDBTYPE_SW;
    idb_list_elt->idb      = idb;
    ret = list_insert(l_hdr, (list_element *)idb_list_elt, idb,
		      swidb_list_order);
    return (ret);
}

/*
 * hwidb_list_order
 *
 * This function is called by the List Manager to determine at what
 * position to insert an element into a list.
 *
 * This function will keep the ordering of hw IDB's in the list
 * by if_number, preserving the ordering one would expect to see
 * from FOR_ALL_HWIDBS().
 */
static int 
hwidb_list_order (void *runner_arg, void *element_arg)
{
    hwidbtype	*runner  = runner_arg;
    hwidbtype	*element = element_arg;

    if (runner == NULL) {
	return (0);
    }
    
    if (runner == element) {
	return (0);
    }

    if (platform_interface_ordering(element, runner)) {
	return (1);
    } else {
	return (-1);
    }
}

boolean 
idb_add_hwidb_to_list (struct hwidbtype_ *hwidb, idblist_t type)
{
    list_header		*l_hdr;
    dyn_idblist_elt_t	*idb_list_elt;
    boolean		ret;

    if (   type == IDBLIST_MAX
	|| (type > HWIDBLIST_MAX && type < SWIDBLIST_MAX))
	return (FALSE);

    l_hdr = get_list_header(type);
    if (l_hdr == NULL)
	return (FALSE);
    /*
     * We now have the correct list header. We now walk the list
     * to make sure that we insert the IDB pointer at the correct point
     * in the list.
     */
    idb_list_elt = chunk_malloc(idb_list_elt_chunk);
    if (idb_list_elt == NULL)
	return (FALSE);

    idb_list_elt->type      = type;
    idb_list_elt->idb_type  = IDBTYPE_HW;
    idb_list_elt->idb       = hwidb;
    ret = list_insert(l_hdr, (list_element *)idb_list_elt, hwidb,
		      hwidb_list_order);
    return (ret);
}
/*
 * idb_report_add_error
 */
void idb_report_add_error (char *str)
{
    buginf("Error adding idb to %s idb list\n", str);
}

/*
 * idb_remove_from_list
 *
 * Remove an IDB (either hardware or software) from an IDB list.
 * We simply traverse the list ID in question, looking for an entry
 * with the IDB memory address in question and we excise this entry
 * from the middle of the linked list.
 *
 * Returns TRUE if IDB pointer was found and removed, FALSE if not found.
 */
boolean
idb_remove_from_list (void *idb, idblist_t type)
{
    list_header		*l_hdr;
    list_element	*runner;
    void		*idb_elt;

    if (idb == NULL || (type == IDBLIST_MAX) || (type == IDBLIST_DYNAMIC))
	return (FALSE);

    l_hdr = get_list_header(type);
    if (l_hdr == NULL)
	return (FALSE);
    FOR_ALL_DATA_IN_LIST(l_hdr, runner, idb_elt) {
	if (idb_elt != idb)
	    continue;
	if (list_remove(l_hdr, runner, NULL) != NULL) {
	    chunk_free(idb_list_elt_chunk, runner);
	    return(TRUE);
	}
	return(FALSE);
    }
    return (FALSE);
}

/*
 * idb_report_removal_error
 */
void idb_report_removal_error (char *str)
{
     buginf("Error removing idb from %s idb list\n", str);
}

/*
 * swidb_for_all_on_list
 *
 * This function will perform the equivalent of the Lisp "apply"
 * function -- ie, apply a function to every idb in the list.
 */
boolean
idb_for_all_on_swlist (idblist_t type, swidbfunc_t idbfunc, void *arg)
{
    list_header		*l_hdr;
    list_element	*list_elt;
    void		*idb_elt;

    if ((type == IDBLIST_MAX) || (type == IDBLIST_DYNAMIC))
	return (FALSE);

    l_hdr = get_list_header(type);
    if (l_hdr == NULL)
	return (FALSE);
    list_elt = NULL;
    FOR_ALL_DATA_IN_LIST(l_hdr, list_elt, idb_elt) {
	(*idbfunc)((struct idbtype_ *)idb_elt, arg);
    }
    return (TRUE);
}

/*
 * hwidb_for_all_on_list
 *
 * This function will perform the equivalent of the Lisp "apply"
 * function -- ie, apply a function to every idb in the list.
 */
boolean
idb_for_all_on_hwlist (idblist_t type, hwidbfunc_t idbfunc, void *arg)
{
    list_header		*l_hdr;
    list_element	*list_elt;
    void		*idb_elt;


    if (   (type > HWIDBLIST_MAX && type < IDBLIST_MAX)
	|| (type == IDBLIST_DYNAMIC))
	return (FALSE);

    l_hdr = get_list_header(type);
    if (l_hdr == NULL)
	return (FALSE);
    list_elt = NULL;
    FOR_ALL_DATA_IN_LIST(l_hdr, list_elt, idb_elt) {
	(*idbfunc)((struct hwidbtype_ *)idb_elt, arg);
    }
    return (TRUE);
}


/*
 * rxtype_string
 * Return rxtype as a string
 */

char *rxtype_string (rxtype_t rxtype)
{
    static char *const rxtype_string_array[RXTYPE_LAST] = {
	"UNKNOWN",
	"DODIP",
	"DECNET",
	"XNS",
	"CLNS",
	"APPLE",
	"NOVELL1",
	"NOVELL2",
	"SAP_IP",
	"APOLLO",
	"10_UNUSED",
	"VINES",
	"TRANSIT",
	"SRB",
	"ISL_vLAN",
	"IEEE_802dot10_vLAN",
	"SNAP_UNK",
	"SNAP_DODIP",
	"SNAP_DECNET",
	"SNAP_XNS",
	"20_UNUSED",
	"SNAP_APPLE",
	"SNAP_NOV1",
	"23_UNUSED",
	"24_UNUSED",
	"SNAP_APOLLO",
	"26_UNUSED",
	"SNAP_VINES",
	"28_UNUSED",
	"LEX",
	"JUMBO_CLNP",
	"JUMBO_IP",
	"SAP_UNKN",
	"33_UNUSED",
	"IPUNC",
	"IP_TUNNEL",
	"SAP_CLNS",
	"37_UNUSED",
	"SAP_NOVELL",
	"39_UNUSED",
	"40_UNUSED",
	"LANE_BUS",
	"LANE_CONTROL",
	"SAP_VINES",
	"44_UNUSED",
	"RFC826_ARP",
	"DLSW",
	"REVERSE_ARP",
    };

    if (rxtype < MAX_RXTYPES) {
	return (rxtype_string_array[rxtype]);
    } else {
	return ("\nInvalid RXTYPE passed to rxtype_string()");
    }
}

boolean
idb_hw_state_config (hwidbtype *hwidb, ulong opcode, struct itemlist_ *list)
{
    if (!(hwidb && list))
	return (FALSE);

    return ((*hwidb->state_config)(hwidb, opcode, list));
}

#ifdef NOBODY_CALLS_ME
boolean
idb_sw_state_config (idbtype *idb, ulong opcode, struct itemlist_ *list)
{
    if (!(idb && list))
	return (FALSE);

    return ((*idb->state_config)(idb->hwptr, opcode, list));
}
#endif /* NOBODY_CALLS_ME */

/*
 * idb_inputs
 *
 * Return cumulative counts for both slow, fast, and automonous switch path 
 * processing of input packets (This was split apart to make per-interface 
 * accounting not cause a performance hit).
 */

ulong idb_inputs (hwidbtype *idb)
{
    ulong temp = idb->counters.inputs;
    int i;
    idbtype *swidb;
    
    /*
     * Now if we are doing any Cbus or SSE Bridging then first poll the
     * SP autonomous counters so as to incorporate them into the overall
     * count 
     */
    if (idb->cbus_bridge_enable || idb->sse_bridge_forwarding) {
        FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
            reg_invoke_bridge_poll_interface_stats(swidb);
        }
    }
    
    for (i = 0; i < ACCT_PROTO_LAST; i++) {
	temp += idb->counters.inputs_fast[i];
	temp += idb->counters.inputs_auto[i];
    }
    return (temp);
}

/*
 * idb_increment_rx_proto_counts
 * increment the per protocol counters for the receive interface
 * If we are dealing with a dialer interface, we need to increment
 * the counters for the actual interface.
 */
void idb_increment_rx_proto_counts (paktype *pak, int proto)
{
    hwidbtype *hwidb;

    hwidb = (pak->dialer_input != NULL) ? pak->dialer_input :
	pak->if_input->hwptr;

    if (hwidb) {
	hwidb->counters.rx_frames[proto]++;
	hwidb->counters.rx_bytes[proto] += pak->datagramsize;
    }
}


ulong idb_fast_in (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.inputs_fast[i];
    return (temp);
}

ulong idb_auto_in (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.inputs_auto[i];
    return (temp);
}

ulong idb_rx_cumbytes (hwidbtype *idb)
{
    ulong temp = idb->counters.rx_cumbytes;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++) {
      temp += idb->counters.rx_cumbytes_fast[i];
      temp += idb->counters.rx_cumbytes_auto[i];
    }
    return (temp);
}

ulong idb_rx_fast_cumbytes (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.rx_cumbytes_fast[i];
    return (temp);
}

ulong idb_rx_auto_cumbytes (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.rx_cumbytes_auto[i];
    return (temp);
}

ulong idb_outputs (hwidbtype *idb)
{
    ulong temp = idb->counters.outputs;
    int i;
    idbtype *swidb;

    /*
     * Now if we are doing any Cbus Bridging or SSE bridging then firstly
     * poll the SP or SSP for autonomous counters so as to incorporate them
     * into the overall count.
     */

    if (idb->cbus_bridge_enable || idb->sse_bridge_forwarding) {
        FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
            reg_invoke_bridge_poll_interface_stats(swidb);
        }
    }

    for (i = 0; i < ACCT_PROTO_LAST; i++) {
      temp += idb->counters.outputs_fast[i];
      temp += idb->counters.outputs_auto[i];
    }
    return (temp);
}

ulong idb_fast_out (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.outputs_fast[i];
    return (temp);
}

ulong idb_auto_out (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.outputs_auto[i];
    return (temp);
}

ulong idb_tx_cumbytes (hwidbtype *idb)
{
    ulong temp = idb->counters.tx_cumbytes;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++) {
      temp += idb->counters.tx_cumbytes_fast[i];
      temp += idb->counters.tx_cumbytes_auto[i];
    }
    return (temp);
}

ulong idb_tx_fast_cumbytes (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.tx_cumbytes_fast[i];
    return (temp);
}

ulong idb_tx_auto_cumbytes (hwidbtype *idb)
{
    ulong temp = 0;
    int i;

    for (i = 0; i < ACCT_PROTO_LAST; i++)
      temp += idb->counters.tx_cumbytes_auto[i];
    return (temp);
}

/*
 * idb_subif_allowed_here
 *
 * Returns TRUE if subinterfaces are allowed on a particular hardware IDB
 *
 * Subinterfaces now also allowed on HDLC interfaces to support 802.10
 * SDE encapsulation over serial links.
 */
boolean
idb_subif_allowed_here (hwidbtype *idb)
{
    if (idb == NULL)
	return (FALSE);
    /*
     * Currently, sub-interfaces are not supported for lex interfaces. This 
     * check is necessary because the status word on lex interfaces have 
     * IDB_IEEEMASK set.  When sub-interfaces are supported on LAN media,
     * this decision will have to be re-evaluated.
     */
    if (is_lex(idb)) 		/* check for IDB_LEX before IDB_IEEEMASK */
	return (FALSE);
    if (idb->status & IDB_SERIAL)
        if (is_frame_relay(idb) || is_x25(idb) || is_smds(idb) ||
	    is_hdlc(idb) || is_atm_dxi(idb))
	    return (TRUE);		/* Allow frame relay and X.25 */
    if (idb->status & IDB_IEEEMASK)
	return (TRUE);			/* For Novell encaps & vLAN switching */
    if (idb->status & IDB_ATM) {
	return (TRUE);
    }
    return (FALSE);
}

/*
 * idb_get_counter_info
 *
 * Return IDB counter information.
 */
void *
idb_get_counter_info (hwidbtype *hwidb, enum IDB_COUNTER_INFO which)
{
    void 	*ret;

    if (hwidb == NULL)
	return (NULL);
    switch (which) {
    case IDB_COUNTER_SI_CNT_PTR:
	ret = hwidb->si_counters ? hwidb->si_counters : &idb_si_counter_zeros;
	break;

    case IDB_COUNTER_CNT_PTR:
	ret = &hwidb->counters;
	break;

    case IDB_COUNTER_SI_CNT_ACTIVE:
	ret = hwidb->si_counters;
	break;

    default:
	ret = NULL;
	break;
    }
    return (ret);
}

/*
 * print_snpa_string
 *
 * Convert an SNPA to a string in standard form. The function puts the string
 * into the buffer passed in by caller, and returns the pointer to the
 * buffer.
 */
char *print_snpa_string (char *buffer, hwaddrtype *snpa)
{

    if (!snpa) {
	sprintf(buffer, "<Null address>");
	return(buffer);
    }
    switch (snpa->type) {
      case STATION_IEEE48:
	  sprintf(buffer, "%e", snpa->addr);
	  break;
      case STATION_SMDS64:
	  sprintf(buffer, "%E", snpa->addr);
	  break;
      case STATION_X121:
	  sprintf(buffer, "%*s", snpa->length, snpa->addr);
	  break;
      case STATION_FR10:
	  sprintf(buffer, "DLCI %d", *(ushort *)snpa->addr);
	  break;
      case STATION_CISCO_HDLC:
	  sprintf(buffer, "*HDLC*");
	  break;
      case STATION_CISCO_MLAPB:
	  sprintf(buffer, "*m-LAPB*");
	  break;
      case STATION_LAPB:
	  sprintf(buffer, "*LAPB*");
	  break;
      case STATION_CISCO_MSDLC:
	  sprintf(buffer, "*m-SDLC*");
	  break;
      case STATION_PPP:
 	  sprintf(buffer, "*PPP*");
	  break;
      case STATION_CISCO_TUNNEL:
	  sprintf(buffer, "*Tunnel*");
	  break;
      case STATION_ATMVC:
	  sprintf(buffer, "VC %d", *(ushort *)snpa->addr);
	  break;
      case STATION_ATMNSAP:
	  sprintf(buffer, "*ATM NSAP*");
	  break;
      default:
	  sprintf(buffer, "**Unknown SNPA type**");
	  break;
    }

    return(buffer);
}

/* this accessor should move to a subblock module eventually? gcox */
boolean is_stun_sdlc_encap (const hwidbtype *hwidb) {
  return ( (hwidb->stun_sdlc_encap == ET_SDLCP)
           || (hwidb->stun_sdlc_encap == ET_SDLCS));
}
