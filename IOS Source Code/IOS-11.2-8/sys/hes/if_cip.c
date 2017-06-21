/* $Id: if_cip.c,v 3.13.4.24 1996/09/13 17:26:50 kmitchel Exp $
 * $Source: /release/112/cvs/Xsys/hes/if_cip.c,v $
 *------------------------------------------------------------------
 * if_cip.c:  Stuff for the Channel Interface Processor
 *
 * February 1994, Stig Thormodsrud & William H. Palmer
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_cip.c,v $
 * Revision 3.13.4.24  1996/09/13  17:26:50  kmitchel
 * CSCdi59440:  Escon CIP becomes Boxed when router Reloaded
 * Branch: California_branch
 *
 * Revision 3.13.4.23  1996/09/06  08:00:08  mkirk
 * CSCdi59855:  TN3270 Server timing-mark command has minor blemishes
 * Branch: California_branch
 *
 * Revision 3.13.4.22  1996/09/05  05:47:06  mkirk
 * CSCdi59855:  TN3270 Server timing-mark command has minor blemishes
 * Branch: California_branch
 *
 * Revision 3.13.4.21  1996/08/28  12:44:40  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.4.20  1996/08/26  18:58:20  kmitchel
 * CSCdi66420:  CBUS-3-CIPRSET 8010 on interface disable; IOS loops
 * Branch: California_branch
 *
 * Revision 3.13.4.19  1996/08/26  18:24:40  kmitchel
 * CSCdi66467:  CIP love letter stats not displayed after 49 days of uptime
 * Branch: California_branch
 *
 * Revision 3.13.4.18  1996/08/26  15:07:06  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.13.4.17  1996/08/13  02:17:37  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.13.4.16  1996/08/07  18:47:37  lmenditt
 * CSCdi64003:  show ext channel x/0 <path> stat gives wrong output
 * Branch: California_branch
 *
 * Revision 3.13.4.15  1996/08/07  08:58:53  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.4.14  1996/08/01  16:54:47  lmenditt
 * CSCdi64147:  sh ext ch x/2 tcp-s ipAddr cant find match
 * Branch: California_branch
 *
 * Revision 3.13.4.13  1996/07/15  17:38:24  kmitchel
 * CSCdi62273:  IP MTU xxxx on CIP Physical Interface periodically gets
 * disappered
 * Branch: California_branch
 *
 * Revision 3.13.4.12  1996/07/09  05:51:09  ppearce
 * CSCdi59527:  inlines wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.13.4.11  1996/06/27  09:28:22  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.13.4.10  1996/06/21  20:16:29  kmitchel
 * CSCdi31487:  CIP: Static route for CLAW connection should be more
 * dynamic
 * Branch: California_branch
 *
 * Revision 3.13.4.9  1996/06/06  18:17:27  motto
 * CSCdi58517:  CSNA Virtual Port x/2 hung in down/down state after
 * noShut/micro rel
 * Branch: California_branch
 *
 * Revision 3.13.4.8  1996/06/02  15:31:45  motto
 * CSCdi50208:  Issuing csna, llc or tcp CIP show cmd immed after CIP
 * reset fails
 * Branch: California_branch
 *
 * Revision 3.13.4.7  1996/05/29  21:46:26  motto
 * CSCdi57396:  %ALIGN-3-CORRECT error at 0x600ABF6C (channel_et_srb)
 * Branch: California_branch
 *
 * Revision 3.13.4.6  1996/05/29  18:25:41  motto
 * CSCdi46832:  vencap failed debug messages displayed for non cip
 * explorers
 * Branch: California_branch
 *
 * Revision 3.13.4.5  1996/05/17  11:17:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.12.2.10  1996/05/12  08:10:01  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.9  1996/05/05  23:07:05  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.12.2.8  1996/05/01  08:01:13  bchan
 * CSCdi56320:  client connection didnt work
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.7  1996/04/26  15:18:42  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.6  1996/04/03  14:29:23  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.12.2.5  1996/03/28  11:28:34  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.12.2.4  1996/03/21  06:07:45  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.12.2.3  1996/03/17  17:49:13  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.12.2.2  1996/03/15  05:55:32  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Code review comments
 *
 * Revision 3.12.2.1  1996/03/01  16:30:53  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.13.4.4  1996/05/09  14:28:47  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.4.3.2.1  1996/04/27  07:02:18  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.4.3  1996/04/25  23:12:12  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.13.4.2.6.1  1996/04/08  01:46:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.4.2  1996/03/23  01:31:00  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.13.4.1  1996/03/18  19:41:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.8.2  1996/03/28  17:24:47  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.13.8.1  1996/03/22  09:36:25  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/03/07  19:38:46  kmitchel
 * CSCdi31487:  CIP: Static route for CLAW connection should be more
 *              dynamic
 *
 * Revision 3.12  1996/02/13  01:33:45  motto
 * Spontaneous cbus complex restarts running CIP CSNA and
 * also out stuck messages.
 * CSCdi47536:  CIP failure in 7513 - CY-Bus problems
 *
 * Revision 3.11.2.1  1996/01/24  21:46:10  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.11.16.1  1996/03/05  06:11:17  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.11.12.1  1996/02/27  20:42:00  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.11  1996/01/18  22:47:15  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.10.2.1  1996/02/15  19:13:00  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.10  1996/01/07  13:51:20  banderse
 * CSCdi45351:  show microcode should display CIP flash default
 * if_cip.c - cleanup up printout.
 * ucode.c,
 * ucode.h  - handle IP flash default from mkbndl FLASH keyword.
 *
 * Revision 3.9  1995/12/13  16:14:15  motto
 * CSCdi44542:  CIP internal MAC adapters do not get ackd after mic rel
 *
 * Revision 3.8  1995/12/12  23:39:28  ppearce
 * CSCdi40831:  hold-queue config cmd missing on CIP i/f after micro reload
 *   Invoke holdq_init() only if output holdq values have not been init'd
 *     otherwise invoke holdq_reset() instead
 *
 * Revision 3.7  1995/12/05  23:08:08  motto
 * Fix compile error - making functions static.
 * CSCdi30997:  snmp sub-agent for IBM channel claw cfg returns too many
 * instances
 *
 * Revision 3.6  1995/12/05  15:12:44  motto
 * CSCdi30997:  snmp sub-agent for IBM channel claw cfg returns too many
 * instances
 *
 * Revision 3.5  1995/12/05  14:48:45  motto
 * CSCdi41938:  SNMP gets of the CIP daughter brd rets NULL if I/F not
 * cfgd w/devic
 *
 * Revision 3.4  1995/11/24  16:38:31  jjohnson
 * CSCdi42637:  platform-specific code should be moved out of sub_snmp
 * Install an API for accessing platform-specific SNMP objects
 *
 * Revision 3.3  1995/11/17  09:17:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:27:26  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.28  1995/11/08  21:00:23  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.27  1995/10/05  17:33:49  motto
 * Close window on BADVCN error message.
 * Fix SNMP query of CIP daughter boards. Was returning the virtual
 * interface as a daughter board. Remove 'debug channel trap' from
 * parser. This was not implemented to do anything.
 * CSCdi39794:  %MEMD-3-BADVCN error messages after CIP mic reload
 *
 * Revision 2.26  1995/09/19  18:36:22  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.25  1995/09/15  05:57:02  ppearce
 * CSCdi40438:  Cannot config LLC parms for RSRB/LAK on CSNA virtual i/f
 *
 * Revision 2.24  1995/09/11  10:23:44  kmitchel
 * CSCdi25909:  Erroneous CIP configuration commands are still added to
 *              config file
 *
 * Revision 2.23  1995/09/09  00:51:35  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.22  1995/09/07  22:26:54  ppearce
 * CSCdi39901:  Crash in RSP when source-bridge configured on CIP ILAN
 *   Set "slot_ptr" in CIP internal LAN's virtual idb
 *
 * Revision 2.21  1995/08/29  23:54:10  motto
 * CSCdi39129:  Make the source-bridge config cmd on CIP ILANs work like
 * TRIP/FIP
 *
 * Revision 2.20  1995/08/28  23:03:10  ppearce
 * CSCdi39437:  RP throttles CIP interface by sending disable interface cmd
 *   Initialize CIP virtual interface to indicate IDB_CBUS status
 *     so that RP throttles CIP interface correctly
 *
 * Revision 2.19  1995/08/27  17:16:40  motto
 * CSCdi39079:  LF field in RIF for CIP ILANs is never greater than 2052
 *
 * Revision 2.18  1995/08/27  15:37:43  motto
 * CSCdi38395:  show ex ch <slot>/<port> stat doesnt show remaining config
 *
 * Revision 2.17  1995/08/22  17:18:32  motto
 * CSCdi37937:  CIP sysmgt RPCs timeout when all interfaces shutdown
 *
 * Revision 2.16  1995/08/12  07:08:51  smackie
 * Repair various buffer and memory display foibles. (CSCdi37152)
 *
 *   o Unify memory dump code to one simple general purpose routine
 *   o Enhance buffer debugging by extending buffer display commands
 *   o Show all private buffer pools (so that BRI pools show up)
 *
 * Revision 2.15  1995/08/09  21:33:20  ppearce
 * CSCdi38300:  Add CSNA packet switching to 75xx (RSP)
 *
 * Revision 2.14  1995/08/09  00:19:41  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.13  1995/08/08  16:46:52  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.12  1995/08/04 14:10:09  motto
 * Changed CIP LAN interface into SRB to only have one entry per LAN
 * instead of per adapter.
 * CSCdi37535:  CIP LAN SRB explorers multiplying by # of adapters defined
 * per LAN
 *
 * Revision 2.11  1995/07/29  01:43:51  snyder
 * CSCdi37882:  currekt missed-spelled wurds
 *              foward and nofoward misspelled, but responsible parties
 *              wanted new words (yes and no) instead of fixed spelling
 *              since it saves 9 bytes, I'll change words instead.  ;-)
 *
 * Revision 2.10  1995/07/23  16:25:52  motto
 * Modify CIP SRB fastswitch to allow CIPxLANa-2-CIPyLANa
 * and CIPxLANa-2-CIPxLANb.
 * CSCdi37365:  CSNA does not work with 2 CIPs in a C7000
 *
 * Revision 2.9  1995/07/19  23:37:30  motto
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.8  1995/07/18  21:46:25  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.7  1995/07/13  19:41:48  motto
 * Fixed all channel tcp/ip show commands to be consistent and work
 * properly.
 * CSCdi36252:  show ex ch <slot>/<port> tcp-connections needs parms to be
 * optional
 *
 * Revision 2.6  1995/06/25  21:26:27  kmitchel
 * CSCdi35580:  CIP-SNA Virtual Lan: verbiage change - virtual to internal
 *              also includes fix for CSCdi35538 - display all CLAW lnks
 *              for offload statistics.
 *
 * Revision 2.5  1995/06/22  15:33:48  motto
 * CSCdi36240:  SRB fastswitching and process switching broken for CIP
 * v-interface
 *
 * Revision 2.4  1995/06/21  20:28:06  motto
 * Fix 'show extended channel x/2 <llc2|csna|connection-map|
 * max-llc2-sessions> ....' commands for SNA support on the CIP card.
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.3  1995/06/18  23:17:08  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/16  04:27:35  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:40:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "media_registry.h"
#include "cbus_registry.h"
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "if_mci.h"
#include "../if/fddi.h"
#include "../hes/if_hes_common.h"
#include "if_cip.h"
#include "../os/chunk.h"
#include "if_cip_vlan.h"
#include "if_cip_tn3270s.h"
#include "../if/channel.h"
#include "channel_sb.h"
#include "logger.h"
#include "../os/free.h"
#include "../if/network.h"
#include "config.h"
#include "../parser/parser_defs_channel.h"
#include "../if/channel_debug.h"
#include "../iprouting/route.h"
#include "../ip/ip_registry.h"
#include "../ip/ip.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_platform_api.h"
#include "../snmp/sr_old_lchassismib.h"
#include "../if/static_map.h"
#include "if_cmd.h"
#include "../if/tring.h"
#include "if_cip_tcpip.h"
#include "if_cip_sna.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../llc2/llc2_sb.h"
#include "msg_cbus.c"			/* Not a typo, see logger.h */

#include "if_cip_sysmgt_rpc.h"
#include "ucode.h"
#include "ctrlr_cmds.h"
#include "../dev/flash_dvr_spec.h"
#include "slot.h"
#include "../h/types.h"
#include "sysmgt_channel.h"
#include "sr_channelmib.h"

/*
 * Structures for holding the information retrieved
 * from the CIP via cbus attn commands.
 * Using these commands allows this information to
 * be displayed, even if all the CIP interfaces are
 * administratively shutdown.  [CSCdi26192]
 */

typedef struct cip_adptr_info_ {
    ulong type;            /* Channel adapter type, eg ECA or PCA */
    ulong hw_ver;          /* Channel adpater hw version          */
    ulong ucode_ver;       /* Channel adapter microcode version   */
} cip_adptr_info;

typedef struct cip_mem_info_ {
    ulong sram_avail;
    ulong sram_total;
    ulong dram_avail;
    ulong dram_total;
} cip_mem_info;

#define CIP_METRIC_NOTAVAIL MAXULONG

/*
 * procedure function declaration for checking vc by type 
 */
typedef boolean (vc_check_proc)(cip_vctype *);

static void channel_love_raw_pak(paktype *); 

static void cip_plevel_init(hwidbtype *, cip_idb_infotype *);
static void cip_purge_queued_events(hwidbtype *, cip_idb_infotype *, boolean);

static void cip_eoir_reset(hwidbtype *);
static void cip_reset_queues_and_buffs(hwidbtype *);
static boolean cip_disable(hwidbtype *);
static void cip_reset_cfg(hwidbtype *, cip_idb_infotype *);
static boolean cip_enable(hwidbtype *);
static void cip_defer_enable(hwidbtype *, cip_idb_infotype *);

static void cip_cfg_acknowledge(hwidbtype *, ushort, ulong, uchar *, ulong);
static void cip_unexpected_cfg_ack(idbtype *, char *, uchar *, ulong);
static void cip_nack_report(hwidbtype *, boolean, ushort, void *);

static void cip_vc_del_accepted(hwidbtype *, ushort, ushort, cip_vctype *);
static void cip_vc_del_rejected(hwidbtype *, ushort, ushort, cip_vctype *);
static void cip_vc_cfg_accepted(hwidbtype *, ushort, ushort, cip_vctype *);
static void cip_vc_cfg_rejected(hwidbtype *, ushort, ushort, cip_vctype *);


static boolean csna_config_vc_check(cip_vctype *);
static cip_vctype *cip_findNextPathDevice(cip_idb_infotype *, 
		    long, long, int, vc_check_proc (*vc_validate));
static boolean channel_et_srb(paktype *pak, int linktype,
			      ushort flags, int size, ushort vc);

static void cip_plevel_adjust_preQ_size(hwidbtype *, cip_idb_infotype *);
static boolean cip_plevel_element_put(cip_idb_infotype *, plevel_t *,
				      plevent_loc_t);
static boolean cip_plevel_chunk_to_preQ(cip_idb_infotype *);
static void cip_plevel_preQ_to_chunk(cip_idb_infotype *);
static void cip_plevel_element_dispose(cip_idb_infotype *, plevel_t *,
				       dispose_t);
static void cip_parse_love_letter(hwidbtype *, uchar *, ulong);
static void chan_netmgt_event_handler(ushort slot, ushort code, 
                                      ushort length, void *ev_data );
static boolean get_from_cip_appl_version (hwidbtype *, cip_appl_version_t *);
static boolean len_exceeded(uchar *, uint);
static cip_vctype *cip_find_vc_with_ipaddr(cip_idb_infotype *, 
					   ipaddrtype);


/*
 * Compile time debug control
 * Process Level acknowledgement queue operations
 *   Debugging messages printed only if compiled TRUE and
 *   if channel_debug is TRUE at execution time
 */
#define CHANNEL_DEBUG_DEED FALSE

/*
 * One chunk is used for all CIP interfaces
 */
static chunk_type *cip_plevel_chnk;      /* Chunk of plevel elements */

const char if_shutdown[] = "\n\n...Data unavailable - selected interface is shutdown...\n";

static char *const cip_daughter_types[CIP_NUM_INTERFACE_TYPE+1] = {
    "None",
    "PCA",
    "ECA",
    "Virtual Interface"
};

/*
 * The pca data rates that we support.
 */
static const rate_type pca_rates[CIP_NUM_PCA_RATES] = {
    {"S",  "3",   CIP_PCA_RATE_3},
    {"S4", "4.5", CIP_PCA_RATE_4_5}
};

/* 
 * The operating modes available, although so far
 * only CLAW, IVORIE, and DP are supported by the
 * cip microcode.
 */
static char *const cip_modes[CHANNEL_MAXMODE] = {
    "",
    "CLAW",
    "CSNA",
    "LCS",
    "OFFLOAD",
    "IVORIE",
    "DP"
};
static char *const cip_cfg_names[CHANNEL_MAX_CFG_CMD+1] = {
    "NODEID",
    "CLAW",
    "CSNA",
    "LCS",
    "OFFLOAD",
    "IVORIE",
    "DP",
    "UNDEFINED",
    "CTA-TEST-TOOL",
    "MAX-LLC2-SESS",
    "ADAPTER",
    "ADAPTER UPDATE",
/* =========================== TN3270 =========================== */
    "TN3270S_SVR",             /* CIP_TN3270S_SVR          */
    "TN3270S_SVR_LU_DNS",      /* CIP_TN3270S_SVR_LU_DNS   */
    "TN3270S_SVR_MAX_LUS",     /* CIP_TN3270S_SVR_MAX_LUS  */ 
    "TN3270S_SVR_SIFTDOWN",    /* CIP_TN3270S_SVR_SIFTDOWN */
    "TN3270S_PU_NEW",          /* CIP_TN3270S_PU_NEW       */
    "TN3270S_PU_UPD",          /* CIP_TN3270S_PU_UPD       */
    "TN3270S_DLUR_PU_NEW",     /* CIP_TN3270S_DLUR_PU_NEW  */
    "TN3270S_DLUR_PU_UPD",     /* CIP_TN3270S_DLUR_PU_UPD  */
    "TN3270S_DLUR_DLUR",       /* CIP_TN3270S_DLUR         */
    "TN3270S_DLUR_DLUR_DLUS",  /* CIP_TN3270S_DLUR_DLUS    */
    "TN3270S_DLUR_NN_SVR",     /* CIP_TN3270S_DLUR_NN_SVR  */
    "TN3270S_DLUR_SAP",        /* CIP_TN3270S_DLUR_SAP     */
    "TN3270S_DLUR_SAP_VRN",    /* CIP_TN3270S_DLUR_SAP_VRN */
    "TN3270S_DLUR_SAP_LNK",    /* CIP_TN3270S_DLUR_SAP_LNK */
    "TN3270S_TIMING_MARK",     /* CIP_TN3270S_SVR_TIMING_MARK */
/* =========================== TN3270 =========================== */
    "UNKNOWN"
};

/* 
 * Link Incident Report types
 */
static char *const lir_strings[] = {
    "implicit",
    "bit-error",
    "link failed",
    "NOS",
    "sequence timeout",
    "invalid sequence"
};

static cip_nodeidtype cip_nodeid;    /* only one for all CIPs */
ushort cip_stats_interval;      /* rate at which CIP sends stat love-letters */

/*
 * Default number of attempts to make 
 */
#define CIP_DEFAULT_WRITE_MEM_ATTEMPTS 4  


/*
 * cip_init
 */
static void cip_init (subsystype *subsys)
{
    cip_platform_init(subsys);

    channel_debug_init();
    channel_parser_init();
    
    cip_stats_interval = CIP_DEFAULT_STATS_INTERVAL;

    /* 
     * setup the local node descriptor 
     */
    memset(&cip_nodeid, ' ', sizeof(cip_nodeidtype));
    cip_nodeid.dev_type = CHANNEL_NODEID;
    switch (snmp_platform_get_value(SNMP_PLATFORM_CHASSIS_TYPE)) {
      case D_chassisType_c7000:
	strncpy(cip_nodeid.type_number, "C7000", 5);
	strncpy(cip_nodeid.model_number, "0", 1);
	break;

      case D_chassisType_c7010:
	strncpy(cip_nodeid.type_number, "C7000", 5);
	strncpy(cip_nodeid.model_number, "1", 1);
	break;

      case D_chassisType_c7505:
	strncpy(cip_nodeid.type_number, "C7500", 5);
	strncpy(cip_nodeid.model_number, "0", 1);
	break;

      case D_chassisType_c7506:
	strncpy(cip_nodeid.type_number, "C7500", 5);
	strncpy(cip_nodeid.model_number, "1", 1);
	break;

      case D_chassisType_c7507:
	strncpy(cip_nodeid.type_number, "C7500", 5);
	strncpy(cip_nodeid.model_number, "2", 1);
	break;

      case D_chassisType_c7513:
	strncpy(cip_nodeid.type_number, "C7500", 5);
	strncpy(cip_nodeid.model_number, "3", 1);
	break;

      default:
	strncpy(cip_nodeid.type_number, "Cxx00", 5);
	strncpy(cip_nodeid.model_number, "X", 1);
	break;
    }
    strncpy(cip_nodeid.plant, "A", 1);

    reg_add_hwidb_setup_routine(IDBTYPE_CIP, channel_idb_init,
				"channel_idb_init");
    reg_add_show_controller_specific(FCI_CIP_CONTROLLER, cip_show_cbus, 
				     "cip_show_cbus");
    reg_add_media_channel_macwrite(ET_UNUSED1, channel_et_srb,
				   "channel_et_srb");
    reg_add_cip_netmgt_event( SYSMGT_CHAN, chan_netmgt_event_handler, 
                              "chan_netmgt_event_handler");
    cip_chassis_serial_init(&cip_nodeid);
     
    /* create the chunk of plevel elements */
    cip_plevel_chnk = chunk_create(CIP_PLEVENT_ELEMENT_SIZE, CIP_PLEVENT_COUNT,
				   CHUNK_FLAGS_DYNAMIC,
				   NULL, 0,
				   CIP_PLEVENT_CHUNK_NAME);

}
/*
 * The love letter timestamp sent by the CIP and stored in the
 * cip info block is 64 bits.  Since the timestamp is updated
 * when a love letter is processed and since this occurs at
 * interrupt level, any process-level accesses must be interrupt
 * protected.
 */
ulonglong
cip_get_love_letter_timestamp (cip_idb_infotype *cip)
{
    ulonglong love_time;
    leveltype level;
    
    love_time = 0;
    
    if (cip) {
	level = raise_interrupt_level(NETS_DISABLE);
	love_time = cip->cip_timestamp;
	reset_interrupt_level(level);
    }
    return love_time;
}
void
cip_set_love_letter_timestamp (cip_idb_infotype *cip, ulonglong love_time)
{
    leveltype level;
    
    if (cip) {
	level = raise_interrupt_level(NETS_DISABLE);
	cip->cip_timestamp = love_time;
	reset_interrupt_level(level);
    }
}


/*
 * cip_cfg_name_str
 */
char *cip_cfg_name_str (ushort cfg_cmd)
{
    if ((cfg_cmd & ~CIP_TEARDOWN_VC) < CHANNEL_MAX_CFG_CMD) 
	return cip_cfg_names[cfg_cmd & ~CIP_TEARDOWN_VC];
    
    return cip_cfg_names[CHANNEL_MAX_CFG_CMD];	
}

/*
 * cip_mode_to_str
 */
static inline char *cip_mode_to_str_inline (uint dev_type)
{
    if (dev_type > CHANNEL_MAXMODE)
	return NULL;
    
    return cip_modes[dev_type];
}

/*
 * cip_daughter_string
 */
static char *cip_daughter_string (int type)
{
    if (type < 0 || type > CIP_NUM_INTERFACE_TYPE) {
	return "Unknown";
    } else {
	return cip_daughter_types[type];
    }
}

/*
 * cip_daughter_type
 * 
 * 0 CIP_NONE
 * 1 CIP_PCA_DAUGHTER
 * 2 CIP_ECA_DAUGHTER
 * 3 CIP_VIRTUAL_INTERFACE
 */
static int cip_daughter_type (hwidbtype *hwidb)
{
    leveltype level;
    ushort type;
    
    level = raise_interrupt_level(NETS_DISABLE);

    select_if(hwidb);
    if (send_cip_daughter_card_type_cmd(hwidb, &type)) {
	reset_interrupt_level(level);
	return -1;
    } else {
	reset_interrupt_level(level);
	return (int) type;
    }
}
static boolean cip_is_tn3270s_cmd (ushort cmd) 
{
    switch (cmd) {
    case CIP_TN3270S_SVR          :
    case CIP_TN3270S_SVR_LU_DNS   :
    case CIP_TN3270S_SVR_MAX_LU   :
    case CIP_TN3270S_SVR_SIFTDOWN :
    case CIP_TN3270S_PU_NEW       :
    case CIP_TN3270S_PU_UPD       :
    case CIP_TN3270S_DLUR_PU_NEW  :
    case CIP_TN3270S_DLUR_PU_UPD  :
    case CIP_TN3270S_DLUR         :
    case CIP_TN3270S_DLUR_DLUS    :
    case CIP_TN3270S_DLUR_NN_SVR  :
    case CIP_TN3270S_DLUR_SAP     :
    case CIP_TN3270S_DLUR_SAP_VRN :
    case CIP_TN3270S_DLUR_SAP_LNK : 
    case CIP_TN3270S_SVR_TIMING_MARK:
	return TRUE;
    default:	    
	return FALSE;
    }    
}
/*
 * cip_is_vinterface_cmd
 *
 * return whether this is a virtual interface
 * command
 */
static inline boolean cip_is_vinterface_cmd (
    ushort cmd_type)
{
    switch (cmd_type) {
    case CHANNEL_CFG_MAX_LLC2:
    case CHANNEL_CFG_VADAPTER:
    case CHANNEL_CFG_VADAPTER_UPD:
	return TRUE;
    default:	    
	if (cip_is_tn3270s_cmd(cmd_type))
	    return TRUE;
	return FALSE;
    }    
}

/*
 * cip_copy_idb_id_info()
 *   Copy ID info fields from src_hwidb into dst_hwidb
 *     (i.e. name, slot, unit, slotunit)
 *
 */
void cip_copy_idb_id_info (hwidbtype *src_hwidb, hwidbtype *dst_hwidb)
{

    if (src_hwidb && dst_hwidb) {
        dst_hwidb->name = src_hwidb->name;
        dst_hwidb->slot = src_hwidb->slot;
        dst_hwidb->unit = src_hwidb->unit;
        dst_hwidb->slotunit = src_hwidb->slotunit;
        dst_hwidb->slot_ptr = src_hwidb->slot_ptr;
    }

} /* End cip_copy_idb_id_info() */

/*
 * cip_copy_idb_info()
 *   Copy relevant info from src_hwidb into dst_hwidb
 *
 */
void cip_copy_idb_info (hwidbtype *src_hwidb, hwidbtype *dst_hwidb)
{

    channel_sb_t *src_chan_sb;
    channel_sb_t *dst_chan_sb;

        
    if (src_hwidb && dst_hwidb) {

        dst_hwidb->status |= src_hwidb->status;

        dst_hwidb->fci_type = src_hwidb->fci_type;

        ieee_copy(src_hwidb->bia, dst_hwidb->bia);
        ieee_copy(src_hwidb->hardware, dst_hwidb->hardware);

        dst_hwidb->mci_index = src_hwidb->mci_index;
        dst_hwidb->mci_regptr = src_hwidb->mci_regptr;
        dst_hwidb->devctl = src_hwidb->devctl;
        dst_hwidb->cbus_ctrlr_index = src_hwidb->cbus_ctrlr_index;

        dst_hwidb->maxmtu_def = src_hwidb->maxmtu_def;
	dst_hwidb->firstsw->sub_mtu = src_hwidb->maxmtu_def;

        dst_hwidb->maxmtu = src_hwidb->maxmtu;
        dst_hwidb->max_buffer_size = src_hwidb->max_buffer_size;
	dst_hwidb->max_pak_size = src_hwidb->max_pak_size;
	dst_hwidb->max_pak_size_def = src_hwidb->max_pak_size_def;
        dst_hwidb->max_buffers = src_hwidb->max_buffers;

        src_chan_sb = channel_get_sb(src_hwidb);
        dst_chan_sb = channel_get_or_create_sb(dst_hwidb);
        if (src_chan_sb && dst_chan_sb) {      
            dst_chan_sb->cip_info = src_chan_sb->cip_info;
        }

        dst_hwidb->tx_q_ptr = src_hwidb->tx_q_ptr;
        dst_hwidb->tx_acc_ptr = src_hwidb->tx_acc_ptr;

        dst_hwidb->cbus_burst_count = src_hwidb->cbus_burst_count;
        if (idb_is_throttled(src_hwidb))
	    idb_mark_throttled(dst_hwidb);

        /*
         * Copy LLC2 parms for use by RSRB TCP/local-ack
         *
         */
        llc_sb_copy(src_hwidb->firstsw, dst_hwidb->firstsw);

    }

} /* End cip_copy_idb_info() */

/*
 * cip_interface_is_virtual
 *
 * return whether this is a CIP
 * virtual interface
 */
boolean cip_interface_is_virtual (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;
    
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return FALSE;
    }
    
    switch (cip->daughter_type) {
    case CIP_NONE:
    case CIP_PCA_DAUGHTER:
    case CIP_ECA_DAUGHTER:
	return FALSE;
	/* Virtual Interface */
    case CIP_VIRTUAL_INTERFACE:
    default:
	return TRUE;
    }
}


/*
 * cip_cmd_valid_on_interface
 *
 * return whether this is a valid command for the
 * current interface type
 * Optionally prints message via printf
 */
boolean cip_cmd_valid_on_interface ( 
    hwidbtype *hwidb, 
    ushort     cmd_type,
    boolean    message)
{
    boolean matching;
    
    /* Check first for existance of the cip info block */
    if (channel_sb_cip_info_is_null(hwidb)) {
	if (message)
	    printf("\n%% CIP data block not allocated for this interface: %s; cannot configure.",
		   hwidb->hw_namestring);
	return FALSE;
    }
    
    /* Return TRUE if both return the same result */
    matching = (cip_interface_is_virtual(hwidb) == 
		cip_is_vinterface_cmd(cmd_type));
    if ((!matching) && (message)) {
	printf("\n%% Command is not valid on current interface: %s.",
	       hwidb->hw_namestring);
    }
    return (matching);
}

/*
 * ---------------------------------------------------------------
 * cip_ipaddr_duplicate_chk
 *    Determines whether this ipaddress is already configured on
 *    - any CLAW or OFFLOAD device
 *    - any non-shut interface
 *    - this interface
 *
 * Note: it does not check ip addresses already configured on 
 *       TN3270Server PUs.
 * 
 *    If so, displays a message and returns FALSE
 * ---------------------------------------------------------------
 */
static boolean cip_ipaddr_duplicate_chk (idbtype    *swidb,
				  ushort      cfg_which, 
				  ipaddrtype  ipaddr)
{
    hwidbtype *hwidb;
    cip_vctype *vc;
    
    switch (cfg_which) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
    case CHANNEL_DP:
    case CHANNEL_IVORIE:
    case TN3270S_CFG_PU         :
    case TN3270S_CFG_DLUR_PU    :
	FOR_ALL_HWIDBS (hwidb) {
	    if (is_cip(hwidb)) {
                cip_idb_infotype *cip;
                cip = channel_sb_get_cip_info(hwidb);
                if (cip) {
		    vc = cip_find_vc_with_ipaddr(cip, ipaddr);
		    if (vc) {
			printf("\n%%%s device on %s is using IP address %i", 
			       cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
			       hwidb->hw_namestring,
			       ipaddr);
			return FALSE;
		    }
                }
	    }
	}
	/* Check interfaces, including subnets */
	if (reg_invoke_ip_address_duplicate(ipaddr,
					    MASK_HOST, swidb, TRUE))
	    return FALSE;
	
	/* Finally, check to make sure it's not the ip address of
	   the current interface */
	if (swidb->ip_address == ipaddr) {	    
	    printf("\n%%%s is already using IP address %i",
		   swidb->namestring, ipaddr);
	    return FALSE;
	}
	break;
    default:
	break;
    }	
    return TRUE;
}
/*
 * ---------------------------------------------------------------
 * cip_validate_ipaddr
 *    Determines whether the ipaddress can be configured on
 *    cfg_which and returns TRUE if it can
 *    Displays a message otherwise and returns FALSE
 * ---------------------------------------------------------------
 */
boolean cip_validate_ipaddr (idbtype    *swidb,
			     ushort      cfg_which, 
			     ipaddrtype  ipaddr)
{
    boolean valid;
    
    valid = TRUE;
    switch (cfg_which) {
    case TN3270S_CFG_PU         :
    case TN3270S_CFG_DLUR_PU    :
	/*
	 * The ipaddress cannot match that of:
	 *  - any other non-shut interface
	 *  - any CLAW device on any CIP card
	 *  - any OFFLOAD device on any CIP card
	 */
        valid = cip_ipaddr_duplicate_chk(swidb, cfg_which,
					 ipaddr);
	break;
    default:
	break;
    }
    return valid;
}
/*
 * ---------------------------------------------------------------
 * cip_len_exceeded
 *   Determines whether the string length exceeds the
 *   specified maxlen.  
 *   If so, displays a message and returns TRUE,
 *   otherwise returns FALSE.
 * ---------------------------------------------------------------
 */

boolean cip_len_exceeded (uchar *s,
			  uint maxlen) 
{
    return len_exceeded(s, maxlen);
}


/*
 * cip_get_cfg_block
 *
 * Ask the cip for the address that we should write 
 * configuration blocks to.
 */
static ulong cip_get_cfg_block (hwidbtype *hwidb)
{
    leveltype level;
    ulong mem_addr;

    mem_addr = 0;

    level = raise_interrupt_level(NETS_DISABLE); 
    select_if(hwidb);
    get_cip_cfg_block_cmd(hwidb, &mem_addr);
    reset_interrupt_level(level);

    return mem_addr;
}

/* 
 * cip_write_mem
 */
static int cip_write_mem (hwidbtype *hwidb, ulong mem, ushort *buf, int len)
{
    /* 
     * assume that caller has disabled interrupts and the ctrlr has been
     * selected 
     */
    if (!mem) {
	return(FALSE);
    }

    if (send_cbus_cip_mema_select_cmd(hwidb, mem))
	return FALSE;

    for ( ; len > 0; buf++, len -= sizeof(ushort)) 
	if (send_cbus_cip_mema_write_cmd(hwidb, *buf))
	    return FALSE;

    return(TRUE);
}

    
static void cip_cfg_block_dump ( 
   hwidbtype *hwidb,
   ulong      len,
   void      *pBuf)
{
    uchar   *p;
    uchar    cfg_cmd;
    boolean  teardown;
    char     errbuf[32];

    p = pBuf;
    cfg_cmd = p[0];
    teardown = cfg_cmd & CIP_TEARDOWN_VC;
    cfg_cmd &= ~CIP_TEARDOWN_VC;
    
    switch (cfg_cmd) {
    case CHANNEL_NODEID:
	errbuf[0] = 0;
	break;
    case CHANNEL_CTA_TEST_TOOL:
    {
	ushort command, offset;
	
        command = GETSHORT(&p[2]);
        offset  = GETSHORT(&p[4]);
	
	sprintf(errbuf, "command %-04X, offset %-3d",
		command, offset);
	break;
    }
    case CHANNEL_CLAW:
    case CHANNEL_CSNA:
    case CHANNEL_LCS:
    case CHANNEL_OFFLOAD:
    case CHANNEL_IVORIE:
    case CHANNEL_DP:
    {
	ushort vcnum;
	uchar  device;
	ushort path;
	
        path  = GETSHORT(&p[2]);
        vcnum = GETSHORT(&p[4]);
	device = p[1];
	sprintf(errbuf, "for vc %d, path %04X device %02X",
		vcnum, path, device);
	break;
    }
    default:
	if (cip_is_tn3270s_cmd(cfg_cmd))
	    cip_tn3270s_cfg_str(cfg_cmd, errbuf, pBuf, len);
	else
	    if (cip_is_vinterface_cmd(cfg_cmd))
		errbuf[0] = 0;
	    else
		sprintf(errbuf, " cmd %d",
			cfg_cmd);
	break;
    }
    buginf("\n%s: sending %s%s command %s",
	   hwidb->hw_namestring,
	   teardown ? "no " : "",
	   cip_cfg_name_str(cfg_cmd),
	   errbuf);

    if (channel_debug_blocks)
	memory_dump(p, &p[len-1],
		    FALSE, MEMORY_DUMP_DATA_TYPE_ASCII, buginf);
}

/*
 * cip_send_cfg_cmd - Send config buffer to CIP
 */
static boolean cip_send_cfg_cmd (hwidbtype *hwidb, 
				 cip_idb_infotype *cip,
				 ushort preQ_elements,
				 ushort *buf,
				 ulong len) {
    leveltype level;
    ushort attempts;
    int   i, j;
    boolean success;
    
    if (!cip->cfg_block) {
	cip->cfg_block = cip_get_cfg_block(hwidb);
        if (!cip->cfg_block) 
	    return FALSE;
    }
    
    if (channel_debug)
	cip_cfg_block_dump(hwidb, len, buf);


    /* Process level support
     * Assure an element available for the response
     *  and for any asynchronous responses
     */ 
    for (i = 0; i < preQ_elements; i++) {
	if (!cip_plevel_chunk_to_preQ(cip)) {
	    for (j = i; j > 0; j--)
		cip_plevel_preQ_to_chunk(cip);
	    errmsg(&msgsym(CIPNOPREQ, CBUS), 
		   hwidb->hw_namestring);
	    return FALSE;	
	}
    }
     
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    for (attempts = 0; attempts < CIP_DEFAULT_WRITE_MEM_ATTEMPTS; attempts++) {
	if (cip_write_mem(hwidb, cip->cfg_block, (ushort *)buf, len))
	    break;
    }
    
    if (attempts == CIP_DEFAULT_WRITE_MEM_ATTEMPTS)
	success = FALSE;
    else
	success = (send_cip_parse_blk_cmd(hwidb) == CMD_RSP_OKAY);
    reset_interrupt_level(level);
    
    if (!success) {
	/* Remove the pre-queued elements */
	for (j = preQ_elements; j > 0; j--)
	    cip_plevel_preQ_to_chunk(cip);
    }
    return (success);
}

/*
 * cip_send_or_ack
 *
 *   If the interface is not administratively down (shut),
 *   send the configuration buffer to the CIP.  If this
 *   fails, call the acknowledgement routine with a
 *   CIP_CFG_ACK_LOCAL, CIP_CFG_NACK so that cleanup 
 *   can be done.
 *
 *   Otherwise, if the CIP is administratively down (shut)
 *   call the acknowledgement routine with a
 *   CIP_CFG_ACK_LOCAL, CIP_CFG_ACK
 *
 */
boolean cip_send_or_ack (
   hwidbtype        *hwidb, 
   cip_idb_infotype *cip,
   ushort            num_preQ,
   uchar            *buf,
   ulong             len)
{
    if (!(hwidb->state == IDBS_ADMINDOWN)) {
	if (cip_send_cfg_cmd(hwidb, cip, num_preQ, (ushort *)buf, len)) {
	    return TRUE;
	} else {
	    /* Failed; locally nack the pending configuration */ 
	    cip_cfg_acknowledge(hwidb, CIP_CFG_ACK_LOCAL, CIP_CFG_NACK,
				buf, len);
	    return FALSE;
	}
    } else {
	/* Locally (positive) acknowledge pending configuration */
	cip_cfg_acknowledge(hwidb, CIP_CFG_ACK_LOCAL, CIP_CFG_ACK,
			    buf, len);
	return TRUE;
    }
}

/*
 * cip_send_nodeid
 */
static int cip_send_nodeid (
    hwidbtype *hwidb, 
    cip_idb_infotype *cip, 
    cip_nodeidtype *nodeid, 
    int len)
{
    ushort num_preQ;
    uchar  buf[sizeof(cip_nodeidtype)+sizeof(cip->reset_correlator)];
    
    num_preQ = 1;
    /*
     * CIP nodeid structure is global. We can not put
     * reset correlator in this structure. So, we are
     * adding it to end in this function. The CIP 
     * ucode should be aware that the nodeid structure
     * in memd has a 2 byte correlator at the end.
     * Current code on CIP does not check nodeid length
     * so this should work. 
     * Correlator is being added so that the CIP_CFG_XXX
     * requests can be checked for valid correlator and
     * processed appropriately.  - motto (04Jun96)
     */
    bcopy(nodeid, buf, len);
    PUTSHORT(&buf[len], cip->reset_correlator);
    len += sizeof(cip->reset_correlator);

    return cip_send_or_ack (hwidb, cip, num_preQ, buf, len);
}

static void
cip_init_metrics (cip_metrics *cip_load)
{
    cip_load->one_min = CIP_METRIC_NOTAVAIL;
    cip_load->five_min = CIP_METRIC_NOTAVAIL;
    cip_load->sixty_min = CIP_METRIC_NOTAVAIL;
}

static void
cip_init_load_metric_values (cip_idb_infotype *cip)
{
    cip_init_metrics(&cip->cip_cpu_load);
    cip_init_metrics(&cip->cip_dma_load);
    cip_init_metrics(&cip->cip_channel_load);
}

/*
 * cip_create_info
 */
static cip_idb_infotype *cip_create_info (hwidbtype *hwidb)
{

    cip_idb_infotype *cip;
    channel_sb_t *chan_sb;
    nd *nd_ptr;

    chan_sb = channel_get_or_create_sb(hwidb);
    if (!chan_sb) {
      return(NULL);
    }

    cip = malloc(sizeof(cip_idb_infotype));
    if (!cip) {
	return NULL;
    }

    chan_sb->cip_info = cip;

    queue_init(&cip->cip_vcQ, 0);

    /* Assign vc base for this interface */
    cip->vc_base = (hwidb->slotunit * (CIP_PER_INT_MAX_VC+1));
    cip->next_vc = cip->vc_base;
    
    cip->daughter_type = cip_daughter_type(hwidb); 
    cip->data_rate = CIP_PCA_DEFAULT_RATE;  /* data rate only used on PCA */
    cip->cfg_block = 0;
    cip->subchannels = 0;
    cip_init_load_metric_values(cip);
    cip_set_love_letter_timestamp(cip, 0);
    TIMER_STOP(cip->last_love_letter);
    GET_TIMESTAMP(cip->timeOfReset);

    nd_ptr = (nd *)&cip->node_id[0];
    nd_ptr->flags.validity = ND_NOT_VALID;
    memset(nd_ptr->type_number, '.', ND_TYPE_LEN);
    memset(nd_ptr->model_number, '.', ND_MODEL_LEN);
    memset(nd_ptr->manufacturer, '.', ND_MANUF_LEN);
    memset(nd_ptr->plant, '.', ND_PLANT_LEN);
    memset(nd_ptr->sequence, '.', ND_SERIAL_LEN);
    nd_ptr->tag = 0;

    /* Process Level Event support */
    cip_plevel_init(hwidb, cip);

    cip_vlan_create(hwidb, cip);
    cip_tn3270s_create(hwidb->firstsw, cip);

    queue_init(&cip->cip_cta_ttQ, 0);

    cip->sysmgt_ipc_info = NULL;     /* CIP SYSMGT IPC information */
    return cip;
}

/*
 * cip_get_create_info()
 *   Return existing cip_info ptr from Channel subblock
 *     if no cip_info exists, then try to create
 *
 */
static cip_idb_infotype *cip_get_or_create_info (hwidbtype *hwidb)
{
    cip_idb_infotype *cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        cip = cip_create_info(hwidb);
    }
    return(cip);
}

/*
 * cip_adjacent_devices
 */
static inline boolean cip_adjacent_devices (int dev_type)
{
    switch (dev_type) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
	return TRUE;
    case CHANNEL_CSNA:
    case CHANNEL_DP:
    case CHANNEL_IVORIE:
    default:
	return FALSE;
    }
}

/*
 * cip_find_vc
 */
static cip_vctype *cip_find_vc (
    cip_idb_infotype *cip, 
    uint dev_type, 
    uint path, 
    uint device)
{
    cip_vctype *vc;

    if (!cip)
	return NULL;
    
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if ((vc->flags & CIP_VC_INUSE) && 
	    (dev_type == (vc->dev_type & ~CIP_TEARDOWN_VC)) &&
	    (path == vc->path) && (device == vc->device)) {
	    return vc;
	}
    }
    return NULL;
}

/*
 * cip_find_vc_with_vcnum
 */
static cip_vctype *cip_find_vc_with_vcnum (
    cip_idb_infotype *cip, 
    ushort vcnum)
{
    cip_vctype *vc;

    if (!cip)
	return NULL;

    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if ((vc->flags & CIP_VC_INUSE) && ((ushort) vc->vc_num == vcnum)) {
	    /* Return the even vc */
	    if ((vc->flags & CIP_ADJ_DEV) && (vc->device%2))
		vc = vc->claw_pair;
	    return vc;
	}
    }
    return NULL;
}

/*
 * cip_find_vc_with_ipaddr
 */
static cip_vctype *cip_find_vc_with_ipaddr (
    cip_idb_infotype *cip, 
    ipaddrtype ipaddr)
{
    cip_vctype *vc;

    if (!cip)
	return NULL;

    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if ((vc->flags & CIP_VC_INUSE) && (vc->ipaddr == ipaddr)) {
	    return vc;
	}
    }
    return NULL;
}

/*
 * cip_can_make_vc
 */
static boolean cip_can_make_vc (cip_idb_infotype *cip) 
{
    if ((cip->next_vc+1 >= cip->vc_base) && 
	(cip->next_vc+1 <= cip->vc_base+CIP_PER_INT_MAX_VC)) {
	return TRUE;
    } else {
	return FALSE;
    }
}

/*
 * cip_create_vc
 */
static int cip_create_vc (cip_idb_infotype *cip)
{
    ushort vc;

    if (!cip_can_make_vc(cip))
	return CIP_INVALID_VC;

    vc = cip->next_vc;
    cip->next_vc++;
    
    return vc;
}

/*
 * cip_add_vc
 */
static cip_vctype *cip_add_vc (
    cip_idb_infotype *cip, 
    uint dev_type, 
    uint path, 
    uint device,
    short vc_num)
{
    cip_vctype *vc;
    boolean reuse = FALSE;
    short new_vc_num;

    /*
     * See if we can reuse one
     */
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if (!(vc->flags & CIP_VC_INUSE)) {
	    /* 
	     * If the parameter vc_num is >= 0, then it is
             * telling us to use that vc # instead of generating 
	     * one or using an existing one.  So don't reuse 
	     * a vc that has a valid vc #.
	     */
	    if (vc_num >= 0 && vc->vc_num != CIP_INVALID_VC)
		continue;
	    reuse = TRUE;
	    /*
	     * reset other fields
	     */
	    vc->ipaddr = vc->ipaddr2 = 0;
	    vc->hostname[0] = '\0';
	    vc->cipname[0] = '\0';
	    vc->hostappl[0] = '\0';
	    vc->cipappl[0] = '\0';
	    vc->hostappl2[0] = '\0';
	    vc->cipappl2[0] = '\0';
	    memset(&vc->stats, 0, sizeof(statistics));
	    memset(&vc->si_stats, 0, sizeof(statistics));
	    break;
	}
    }
    
    /* 
     * Decide what vc # we will be assigning.  Could be any
     * of the following conditions:
     *   1. a previously assigned number that we are reusing
     *   2. a new number
     *   3. a number that is passed as the vc_num parameter
     */
    new_vc_num = CIP_INVALID_VC;
    if (vc_num == CIP_NEW_VC) {
	if (vc && vc->vc_num >= 0) {
	    new_vc_num = vc->vc_num; 	      /* condition #1 */
	} else {
	    new_vc_num = cip_create_vc(cip);  /* condition #2 */
	}
    } else {
	new_vc_num = vc_num; 	              /* condition #3 */
    }

    if (new_vc_num < 0)
	return NULL;

    /* 
     * If we didn't find an unused one, make one.
     */
    if (!vc) {
	vc = malloc(sizeof(cip_vctype));
	if (vc == NULL) {
 	    return NULL;
	}
        /*
         * Initialize fields 
         */
        vc->ipaddr = vc->ipaddr2 = 0;
	vc->vc_num = CIP_INVALID_VC;
    }

    vc->vc_num = new_vc_num;
    vc->claw_pair = NULL;
    vc->flags = CIP_VC_INUSE;
    vc->dev_type = dev_type;
    if (cip_adjacent_devices(dev_type))
	vc->flags |= CIP_ADJ_DEV;
    vc->path = path;
    vc->device = device;

    cip->subchannels++;

    if (!reuse)
	enqueue(&cip->cip_vcQ, vc);
    
    return vc;
}
/* 
 * cip_default_pca_rate 
 */
static void cip_default_pca_rate (
    idbtype *swidb,
    cip_idb_infotype *cip)
{	
    cip->data_rate = CIP_PCA_DEFAULT_RATE;
    set_default_interface_bandwidth(swidb, VISIBLE_CIP_PCA_BANDWIDTH_3);
    set_default_interface_delay(swidb, CIP_PCA_DELAY_3);
}

static void cip_add_static_route (
    idbtype    *swidb,
    cip_vctype *vc)
{
    addrtype proto_addr;

    switch (vc->dev_type) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
    case CHANNEL_DP:
    case CHANNEL_IVORIE:
	if (!(vc->flags & CIP_VC_SROUTE)) {
	    if (channel_debug)
		buginf("\nAdding STATIC ROUTE for vc: %s %04X %02X",
		       cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
		       vc->path, vc->device);
	    memset(&proto_addr, 0, sizeof(addrtype));
	    proto_addr.type = ADDR_IP;
	    proto_addr.length = ADDRLEN_IP;
	    proto_addr.ip_addr = vc->ipaddr;
	    reg_invoke_ip_add_static_route(vc->ipaddr, MASK_HOST, 0,
					   swidb, 0, NET_ATTACHED|NET_INTERNAL,
					   STATIC_DISTANCE, NULL);
	    channel_ipaddr_insert(vc->vc_num, &proto_addr);
	    
	    vc->flags |= CIP_VC_SROUTE;
	}
    default:
	break;
    }
}

static void cip_delete_static_route (
    idbtype    *swidb,
    cip_vctype *vc)
{
    if (vc->flags & CIP_VC_SROUTE) {
	if (channel_debug)
	    buginf("\nDeleting STATIC ROUTE for vc: %s %04X %02X",
		   cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
		   vc->path, vc->device);
	reg_invoke_ip_delete_static_route(vc->ipaddr, MASK_HOST, swidb);
	channel_ipaddr_delete(vc->ipaddr);
	vc->flags &= ~CIP_VC_SROUTE;
    }
}

/* 
 * cip_teardown_vc 
 */
static boolean cip_teardown_vc (
    idbtype *swidb,
    cip_idb_infotype *cip,
    cip_vctype *vc)
{
    cip_vctype *vc2;
    
    if ((vc->flags & CIP_ADJ_DEV) && (vc->device%2))
	return FALSE;

    switch (vc->dev_type & ~CIP_TEARDOWN_VC) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
	vc2 = vc->claw_pair;
	vc2->dev_type |= CIP_TEARDOWN_VC;
	vc2->flags &= ~CIP_VC_INUSE;
	vc2->vc_num = CIP_INVALID_VC;
	vc2->ack_flags = 0;
	vc2->state_flags = 0;
	cip->subchannels--;
	break;
    case CHANNEL_DP:
    case CHANNEL_IVORIE:
    case CHANNEL_CSNA:
    default:
	break;
    }
    vc->dev_type |= CIP_TEARDOWN_VC;
    vc->flags &= ~CIP_VC_INUSE;
    vc->ack_flags = 0;
    vc->state_flags = 0;
    cip->subchannels--;
    return TRUE;
}

/*
 * cip_valid_path_device
 */
static boolean cip_valid_path_device (
    cip_idb_infotype *cip, 
    uint dev_type, 
    uint path, 
    uint device)
{
    cip_vctype *vc;
    cip_vctype *duplicate;
    boolean adj_dev;

    if (!cip)
	return FALSE;

    /*
     * Both claw & offload use the adjacent odd device and
     * since this routine is only called on the first device,
     * we must also check device+1.
     */
    if (cip_adjacent_devices(dev_type)) {
	/* 
	 * The 1st device of a claw pair must be even.
	 */
	if (device%2) {
	    printf("\n%%%s must have an even device\n",
		   cip_mode_to_str_inline(dev_type));
	    return FALSE;
	}
	adj_dev = TRUE;
    } else {
	adj_dev = FALSE;
    }

    if (path > CIP_MAX_PATH) {
	printf("\n%%Path must be 4 hex digits\n");
	return FALSE;
    }
    if (device > CIP_MAX_DEVICE) {
	printf("\n%%Device must be 2 hex digits\n");
	return FALSE;
    }
    if (adj_dev) {
	if (device+1 > CIP_MAX_DEVICE) {
	    printf("\n%%Device must be 2 hex digits\n");
	    return FALSE;
	}
    }

    duplicate = NULL;
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if (!(vc->flags & CIP_VC_INUSE))
	    continue;
	if (vc->path == path) {
	    if (vc->device == device) {
		duplicate = vc;
		break;
	    }
	    if (adj_dev) {
		if (vc->device == device+1) {
		    duplicate = vc;
		    break;
		}
	    }
	}
    }
    if (duplicate) {
	printf("\n%%PATH/DEVICE conflicts with path %04X %02X\n",
	       vc->path, vc->device);
	return FALSE;
    }

    return TRUE;
}
/* 
 * len_exceeded - print message if string is longer than max
 */
static boolean len_exceeded ( uchar *s, uint maxlen )
{
    if (strlen(s) > maxlen) { 
	printf("\n%% '%s' exceeds max length of %d characters", s, maxlen);
	return TRUE;
    }	
    return FALSE;    
}
/*
 * cip_validate_parameters
 */
static boolean cip_validate_parameters (
    parseinfo *csb,
    cip_idb_infotype *cip, 
    uint dev_type)
{
    hwidbtype *hwidb;
    cip_vctype *vc;
        
    switch (dev_type) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
    case CHANNEL_DP:
    case CHANNEL_IVORIE:
	FOR_ALL_HWIDBS (hwidb) {
	    if (is_cip(hwidb)) {
                cip = channel_sb_get_cip_info(hwidb);
                if (cip) {
		    vc = cip_find_vc_with_ipaddr(cip, GETOBJ(paddr,1)->ip_addr);
    		    if (vc) {
		        printf("\n%%%s device on %s is using IP address %i", 
		               cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
		               hwidb->hw_namestring,
		               GETOBJ(paddr,1)->ip_addr);
		        return FALSE;
                    }
		}
	    }
	}
	/* Check interfaces, including subnets */
	if (reg_invoke_ip_address_duplicate(GETOBJ(paddr,1)->ip_addr,
					    MASK_HOST, csb->interface, TRUE))
	    return FALSE;

	/* Finally, check to make sure it's not the ip address of
	   the current interface */
	if (csb->interface->ip_address == GETOBJ(paddr,1)->ip_addr) {	    
	    printf("\n%%%s is already using IP address %i",
		   csb->interface->namestring, GETOBJ(paddr,1)->ip_addr);
	    return FALSE;
	}
	break;
    default:
	break;
    }
    switch (dev_type) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
	if ((len_exceeded(GETOBJ(string,1), CIP_NAME_LEN-1)) ||
	    (len_exceeded(GETOBJ(string,2), CIP_NAME_LEN-1)) ||
	    (len_exceeded(GETOBJ(string,3), CIP_NAME_LEN-1)) ||
	    (len_exceeded(GETOBJ(string,4), CIP_NAME_LEN-1))) 
	    return FALSE;

	/* Validate the two additional Offload strings */
	if (dev_type == CHANNEL_OFFLOAD) {
	    if ((len_exceeded(GETOBJ(string,5), CIP_NAME_LEN-1)) ||
		(len_exceeded(GETOBJ(string,6), CIP_NAME_LEN-1))) 
		return FALSE;
	    
	    /* For Offload, the IP link name pair cannot match the API link
	     * name pair
	     */
	    if (!strcasecmp(GETOBJ(string,3), GETOBJ(string,5)) && 
	        !strcasecmp(GETOBJ(string,4), GETOBJ(string,6))) {
		printf("\n%% The IP link names and the API link names are the same (%s %s).",
		       GETOBJ(string,3),
		       GETOBJ(string,4));
		return FALSE;
	    }
	}
	break;
    default:
	break;
    }
    return TRUE;
}    
/* 
 * cip_vc2buf
 *
 * Format of MEMD/DRAM structure for configuration commannds for CIP
 * This structure is defined in the cip repository ../dma/memd.h 
 * and is included here for reference
 *
 * Format of control block used between RP and CIP for
 *   passing configuration commands
 * typedef struct cip_memd_cmd_blk_ {
 *    uchar  devtype;           // connection type (CLAW, etc)
 *    uchar  device;            // device descriptor
 *    uchar  path[2];           // path descriptor
 *    uchar  vcnum[2];          // proposed vc # for this connection
 * *OR*
 *    uchar  ipaddr[4];         // host ip address
 *    uchar  hostname[8];       // host name
 *    uchar  hostappl[8];       // host application name
 *    uchar  cipname[8];	// IP name
 *    uchar  cipappl[8];	// IP application name
 *    uchar  ipaddr2[4];        // second host ip addr for data pusher
 *    uchar  hostappl2[8];      // second host application name for offload 
 *    uchar  cipappl2[8];	// second cip application name for offload
 * *OR*
 *    uchar  maxpiu[2];             // CSNA max piu 
 *    uchar  blocking_time_d[2];    // CSNA blocking time delay 
 *    uchar  blocking_length_d[2];  // CSNA blocking length delay
 * } cip_memd_cmd_blk;
 *
 * Format of MEMD/DRAM structure for configuration commannds for CIP
 * Format of buf:
 *     field      length  CLAW OFFLOAD DP IVORIE
 *  0  vc->dev_type [1]
 *  1  vc->device   [1]     
 *  2  vc->path     [2]     
 *  4  vc->vc_num   [2]     
 *  6  vc->ipaddr   [4]     
 * 10  vc->hostname [8]     
 * 18  vc->hostappl [8]     
 * 26  vc->cipname  [8]     
 * 34  vc->cipappl  [8]     
 *
 * 42  correlator   [2]   XX    XX
 *
 * 42  vc->ipaddr2  [4]               XX   XX
 * 46  correlator   [2]               XX   XX
 *
 * ---------------------------------------------------------------------
 */
/* Define field offsets that are needed in by cfg_acknowledge */
#define OFFSET_CFG_CMD 0
#define OFFSET_CFG_VCNUM 4

static int cip_vc2buf (cip_vctype *vc, uchar *buf, ushort correlator)
{
    int indx;

    /* 
     * Convert stucture elements into a buffer so that we don't 
     * have to worry about the compiler adding padding.
     */

    indx = 0;
    buf[indx++] = (uchar)vc->dev_type;
    buf[indx++] = (uchar)vc->device;
    PUTSHORT(&buf[indx], (ushort)vc->path);
    indx += sizeof(ushort);
    PUTSHORT(&buf[indx], (ushort)vc->vc_num);
    indx += sizeof(ushort);

    switch (vc->dev_type & ~CIP_TEARDOWN_VC) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
    case CHANNEL_DP:      /* host & appl names not used by dp, copy garbage */
    case CHANNEL_IVORIE:
	PUTLONG(&buf[indx], (ulong)vc->ipaddr);
	indx += sizeof(ulong);
	/* 
	 * The strings are not NULL terminated, they are space padded.
	 */
	memset(&buf[indx], ' ',
	       CIP_NUM_CLAW_NAMES * (CIP_NAME_LEN-1));
	strncpy(&buf[indx], vc->hostname, strlen(vc->hostname));
	indx += CIP_NAME_LEN - 1;
	strncpy(&buf[indx], vc->hostappl, strlen(vc->hostappl));
	indx += CIP_NAME_LEN - 1;
	strncpy(&buf[indx], vc->cipname, strlen(vc->cipname));
	indx += CIP_NAME_LEN - 1;
	strncpy(&buf[indx], vc->cipappl, strlen(vc->cipappl));
	indx += CIP_NAME_LEN - 1;

	/*
	 * The second ip address is only used by DP and IVORIE, but is sent
	 * anyway
	 */
	PUTLONG(&buf[indx], (ulong)vc->ipaddr2);
	indx += sizeof(ulong);

	/* API link names are only used by OFFLOAD, but are sent anyway */
	memset(&buf[indx], ' ',
	       (CIP_NUM_OFFLOAD_NAMES - CIP_NUM_CLAW_NAMES) *
	       (CIP_NAME_LEN-1));
	strncpy(&buf[indx], vc->hostappl2, strlen(vc->hostappl2));
	indx += CIP_NAME_LEN - 1;
	strncpy(&buf[indx], vc->cipappl2, strlen(vc->cipappl2));
	indx += CIP_NAME_LEN - 1;
	break;
    case CHANNEL_CSNA:
	PUTSHORT(&buf[indx], vc->maxpiu);
	indx += sizeof(ushort);
	PUTSHORT(&buf[indx], vc->blocking_time_delay);
	indx += sizeof(ushort);
	PUTSHORT(&buf[indx], vc->blocking_length_delay);
	indx += sizeof(ushort);
	break;
    default:
	return 0;
    }
     
    /* Add the correlator */
    PUTSHORT(&buf[indx], correlator);
    indx += sizeof(ushort);
   
    return indx;
}

/*
 * cip_send_subchannel
 */
static int cip_send_subchannel (
    hwidbtype *hwidb, 
    cip_idb_infotype *cip, 
    cip_vctype *vc)
{
    idbtype *swidb;
    uchar buf[CIP_CFG_SIZE];
    int indx;
    ushort num_preQ;

    /* 
     * We only send config of the even device when in
     * claw or offload mode.
     */
    if ((vc->flags & CIP_ADJ_DEV) && (vc->device%2))
	return(FALSE);

    swidb = hwidb->firstsw;
    memset(&buf[0], 0, CIP_CFG_SIZE);
    indx = cip_vc2buf(vc, buf, cip->reset_correlator);
    
    /* Mark the configuration as PENDING acknowledgment from CIP */    
    vc->ack_flags |= CIP_CFG_PNDING;
 
    if (vc->ack_flags & CIP_CFG_CREATE)
	num_preQ = 2;
    else
	num_preQ = 1;

    return cip_send_or_ack (hwidb, cip, num_preQ, buf, indx);
    
}
/*
 * cip_send_cta_tt_command
 * Format of MEMD/DRAM structure for configuration commannds for CIP
 *
 *  typedef struct {
 *     uchar  cfg_cmd_code;     //  cfg command code
 *     uchar  pad_align1;       //  pad to align on even boundary
 *     uint16 command;		//  cta tt command
 *     uint16 offset;		//  cta tt offset 
 *     uchar  value;		//  replacement string
 *  } cip_memd_cmd_blk;
 */
static int cip_send_cta_tt_command (hwidbtype *hwidb, 
                                    cip_idb_infotype *cip, 
                                    cta_tt_cmdtype *cta_tt,
                                    int cfg_cmd) {
    idbtype *swidb;
    uchar buf[CIP_CFG_SIZE];
    int indx = 0;
    ushort num_preQ;
 
    swidb = hwidb->firstsw;
    memset(&buf[0], 0, CIP_CFG_SIZE);
    
    buf[indx] = (uchar)cfg_cmd;
    indx += 2;
    PUTSHORT(&buf[indx], (ushort)cta_tt->command);
    indx += sizeof(ushort);
    PUTSHORT(&buf[indx], (ushort)cta_tt->offset);
    indx += sizeof(ushort);
    PUTSHORT(&buf[indx], (ushort)cta_tt->length);
    indx += sizeof(ushort);
    bcopy(cta_tt->value, &buf[indx], cta_tt->length);
    indx += cta_tt->length;
    
    /* Add the correlator */
    PUTSHORT(&buf[indx], cip->reset_correlator);
    indx += sizeof(ushort);

    /* Mark the configuration as PENDING acknowledgment from CIP */    
    cta_tt->ack_flags |= CIP_CFG_PNDING;

    num_preQ = 1;

    return cip_send_or_ack (hwidb, cip, num_preQ, buf, indx);
}

/*
 * cip_send_rate
 */
static boolean cip_send_rate (
   hwidbtype *hwidb, 
   cip_idb_infotype *cip,
   int rate)
{
    leveltype level;

    if (channel_debug)
	buginf("\n%s: sending data rate %s", 
	       hwidb->hw_namestring, rate);

    level = raise_interrupt_level(NETS_DISABLE); 
    select_if(hwidb);

    if (send_cip_data_rate_cmd(hwidb, rate)) {
	reset_interrupt_level(level);
	return FALSE;
    } else {
	reset_interrupt_level(level);
	return TRUE;
    }
}

/*
 * cip_send_stats_interval
 */
static boolean cip_send_stats_interval (
    hwidbtype *hwidb, 
    int        interval)
{
    leveltype level;
    boolean   success;
    
    level = raise_interrupt_level(NETS_DISABLE); 
    select_if(hwidb);
    
    success = send_cip_stats_int_cmd(hwidb, interval);
    
    reset_interrupt_level(level);
    
    return (success);
}
    
static boolean cip_vc_pending (
   hwidbtype *hwidb,
   cip_vctype *vc)
{
    if (!vc)
	return FALSE;
    
    if (vc->ack_flags & CIP_CFG_PNDING) {
	printf("\n%%Configuration of %s %04X %02X is pending...",
	       cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
	       vc->path, vc->device);
	return TRUE;
    }
    return FALSE;
}   

static boolean update_csna_block_option (ushort *pBlkOption, ushort newValue)
{
    boolean option_changed;
    
    option_changed = *pBlkOption != newValue;
    *pBlkOption = newValue;
    return option_changed;
}

/*
 * channel_interface_csna_command
 */
/******************************************************************
 *      csna <path> <device> [{maxpiu <size> | time-delay <time> | length-delay <piu-length>}]
 * [no] csna <path> <device>
 *
 * OBJ(int,8) = length-delay
 * OBJ(int,6) = time-delay
 * OBJ(int,4) = maxpiu
 *
 * OBJ(int,7) = (FALSE) | TRUE  length-delay
 * OBJ(int,5) = (FALSE) | TRUE  time-delay
 * OBJ(int,3) = (FALSE) | TRUE  maxpiu
 *
 * OBJ(int,2) = <device>
 * OBJ(int,1) = <path>
 * OBJ(idb,1) = <interface>
 */  

/*
 * Constraints ...
 * length-delay specified, but time-delay is 0 
 *  (effect: length-delay ignored, no message; implemented on the CIP)
 * length-delay greater than maxpiu 
 *  (effect: adjusted to maxpiu, no message; implemented on the CIP)
 * specifying maxpiu after the csna device command has been sent to CIP 
 *  (warning: not effective until shut; no shut)
 */

void channel_interface_csna_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    uint path;
    uint device;
    cip_vctype *vc;
    cip_idb_infotype *cip;
    int dev_type;
    uint opt_maxpiu, opt_time_d, opt_length_d;
    boolean csna_device_created;
    boolean csna_blk_opts_specified;
    boolean csna_blk_opts_modified;
    ushort time_d, length_d;

    swidb = csb->interface;
    hwidb = swidb->hwptr;

    cip = cip_get_or_create_info(hwidb);
    if (!cip) {
        if (!csb->nvgen) {
          printf("\n%%Couldn't create info\n");
        }
        return;
    }

    /* Process any queued events */
    cip_process_queued_events(hwidb, cip);

    dev_type = csb->which;

    if (!cip_cmd_valid_on_interface(hwidb, csb->which, !csb->nvgen))
	return;
    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	    if ((vc->dev_type != dev_type) || !(vc->flags & CIP_VC_INUSE))
		continue;
	    nv_write(TRUE, "%s %04X %02X", 
		     csb->nv_command, vc->path, vc->device);
	    nv_add(vc->maxpiu != CSNA_DEVICE_DEF_MAXPIU, " %s %d", 
		   CSNA_DEVICE_KEY_MAXPIU,
		   vc->maxpiu);
	    nv_add(vc->blocking_time_delay != CSNA_DEVICE_DEF_TIME_D, " %s %d",
		   CSNA_DEVICE_KEY_TIME_D,
		   vc->blocking_time_delay);
	    nv_add(vc->blocking_length_delay != CSNA_DEVICE_DEF_LENGTH_D,
		   " %s %d", 
		   CSNA_DEVICE_KEY_LENGTH_D,
		   vc->blocking_length_delay);
	}
	return;
    }
    
    path = GETOBJ(int,1);
    device = GETOBJ(int,2);

    csna_device_created = FALSE;
    
    /* 
     * find VC with matching path/device (if it exists)
     */
    vc = cip_find_vc(cip, dev_type, path, device);
    
    /* See if anything PNDING on this vc */    
    if (vc && cip_vc_pending(hwidb, vc))
	return;
     
    if (!vc) {
	/*
	 * no match found
	 */
	if (!csb->sense) {
	    printf("\n%% Subchannel not found.\n");
	    return;
	}
	
	if (!cip_valid_path_device(cip, dev_type, path, device)) {
	    return;
	}
	/*
	 * validate parameters
	 */
	if (!cip_validate_parameters(csb, cip, dev_type)) {
	    return;
	}    
	vc = cip_add_vc(cip, dev_type, path, device, CIP_NEW_VC);
	if (!vc) 
 	    return;
	
	csna_device_created = TRUE;
	/*
	 * CSNA blocking parameters
	 */
	vc->maxpiu = CSNA_DEVICE_DEF_MAXPIU;
	vc->blocking_time_delay = CSNA_DEVICE_DEF_TIME_D;
	vc->blocking_length_delay = CSNA_DEVICE_DEF_LENGTH_D;
    }

    
    if (!csb->sense) {  
	/*
	 * a no command - tear down the subchannel
	 */
	vc->dev_type |= CIP_TEARDOWN_VC;
	vc->ack_flags |= CIP_CFG_DELETE;
	cip_send_subchannel(hwidb, cip, vc);
	return;	
    }
    
    /* Either adding a new or modifying an existing csna device command */
    
    opt_maxpiu = GETOBJ(int,3);
    opt_time_d = GETOBJ(int,5);
    opt_length_d = GETOBJ(int,7);
    
    csna_blk_opts_specified = opt_maxpiu || opt_time_d || opt_length_d;
    csna_blk_opts_modified = FALSE;
    
    if (csna_blk_opts_specified) {
	/* Validate and process blocking options */
	if (opt_maxpiu) {
	    if ((!csna_device_created) &&
		interface_up(swidb)) {
		/* The subchannel is already active */
		printf("\n%% Subchannel %s %-04X %-02X currently active",
		       cip_mode_to_str_inline(vc->dev_type),
		       vc->path, vc->device);
		printf("\n%s value will become effective at next interface reset",
		       CSNA_DEVICE_KEY_MAXPIU);
	    }
	    update_csna_block_option(&vc->maxpiu, GETOBJ(int,4));
	}
	
	if (opt_time_d) {
	    time_d = GETOBJ(int,6);
	    csna_blk_opts_modified |= 
		update_csna_block_option(&vc->blocking_time_delay, time_d);
	}
	
	if (opt_length_d) {
	    length_d = GETOBJ(int,8);
	    csna_blk_opts_modified |= 
		update_csna_block_option(&vc->blocking_length_delay, length_d);
	}
    }
    
    /* If new device */
    if (csna_device_created) {
	vc->ack_flags |= CIP_CFG_CREATE;
	cip_send_subchannel(hwidb, cip, vc);
	return;	
    }
    /* Otherwise if blocking values have changed */
    if (csna_blk_opts_modified) {
	vc->ack_flags |= CIP_CFG_UPDATE;
	cip_send_subchannel(hwidb, cip, vc);
	return;
    }
    /* 
     * Print message if no blocking overrides specified
     * and the CSNA subchannel already exists.
     */
    if (!csna_blk_opts_specified) {
	printf("\n%% Subchannel not added: %s 0x%-04X 0x%-02X; it already exists.",
	       cip_mode_to_str_inline(vc->dev_type),
	       vc->path, vc->device);
    }
    return;    
}

/*
 * channel_interface_command
 *
 * path     = OBJ(int,1)
 * device   = OBJ(int,2)
 * ipaddr   = OBJ(paddr,1)
 * ipaddr2  = OBJ(paddr,2)   
 * hostname = OBJ(string,1)
 * cipname  = OBJ(string,2)
 * hostappl = OBJ(string,3)
 * cipappl  = OBJ(string,4)
 * hostappl2= OBJ(string,5)
 * cipappl2 = OBJ(string,6)
 */
void channel_interface_command (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    uint path;
    uint device;
    cip_vctype *vc, *vc2;
    cip_idb_infotype *cip;
    int dev_type;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
 
    cip = cip_get_or_create_info(hwidb);
    if (!cip) {
        if (!csb->nvgen) {
          printf("\n%%Couldn't create info\n");
        }
        return;
    }

    /* Process any queued events */
    cip_process_queued_events(hwidb, cip);

    dev_type = csb->which;

    if (!cip_cmd_valid_on_interface(hwidb, csb->which, !csb->nvgen))
	return;
    
    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
	FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	    if ((vc->dev_type != dev_type) || !(vc->flags & CIP_VC_INUSE))
		continue;
	    if ((vc->flags & CIP_ADJ_DEV) && (vc->device%2))
		continue; /* don't save claw pair device */
	    switch (dev_type) {
	    case CHANNEL_CLAW:
	    case CHANNEL_OFFLOAD:
		nv_write(TRUE, "%s %04X %02X %i %s %s %s %s",
			 csb->nv_command, vc->path, vc->device,
			 vc->ipaddr, vc->hostname, vc->cipname,
			 vc->hostappl, vc->cipappl);
		nv_add(dev_type == CHANNEL_OFFLOAD, " %s %s",
			 vc->hostappl2, vc->cipappl2);
		nv_add(vc->flags & CIP_VC_BRDCST, " broadcast");
		break;
	    case CHANNEL_DP:
	    case CHANNEL_IVORIE:
		nv_write(TRUE, "%s %04X %02X %i %i", 
			 csb->nv_command, vc->path, vc->device,
			 vc->ipaddr, vc->ipaddr2);
		break;
	    default:
	    }
	}
	return;
    }

    path = GETOBJ(int,1);
    device = GETOBJ(int,2);

    /* 
     * find swidb with matching path/device (if it exists)
     */
    vc = cip_find_vc(cip, dev_type, path, device);
     
    /* See if anything PNDING on this vc */    
    if (vc && cip_vc_pending(hwidb, vc))
	return;
     
    if (!vc) {
	/*
	 * no match found
	 */
	if (!csb->sense) {
	    printf("\n%%Subchannel not found.\n");
	    return;
	}

	if (!cip_valid_path_device(cip, dev_type, path, device)) {
	    return;
	}
	/*
	 * validate parameters
	 */
	if (!cip_validate_parameters(csb, cip, dev_type)) {
	    return;
	}    
	vc = cip_add_vc(cip, dev_type, path, device, CIP_NEW_VC);
	if (!vc) 
 	    return;
	if (cip_adjacent_devices(dev_type)) {
	    vc2 = cip_add_vc(cip, dev_type, path, device + 1, vc->vc_num);
	    if (!vc2) {
		vc->flags &= ~CIP_VC_INUSE;
		return;
	    }
	    vc->claw_pair = vc2;
	    vc2->claw_pair = vc;
	}
    } else {
	/*
	 * match found
	 */
	if (csb->sense) {
	   printf("\n%%Duplicate.  Not added.\n");
	   return;
	} else {
	    if (cip_adjacent_devices(dev_type) && (vc->device%2)) {
		printf("\n%%%s must have an even device\n",
		       cip_mode_to_str_inline(csb->which));
		return;
	    }
	}
    }

    if (!csb->sense) {  
	/*
	 * a no command 
	 */
	vc->dev_type |= CIP_TEARDOWN_VC;

        /* Suppress traffic flow and broadcasts */
	switch (dev_type) {
	case CHANNEL_CLAW:
	case CHANNEL_OFFLOAD:
	case CHANNEL_DP:
	case CHANNEL_IVORIE:
	    cip_delete_static_route(swidb, vc);
	    break;
	default:
	    break;
	}
	vc->ack_flags |= CIP_CFG_DELETE;
	cip_send_subchannel(hwidb, cip, vc);
    } else {
	switch (dev_type) {
	case CHANNEL_CLAW:
	case CHANNEL_OFFLOAD:
	    vc->dev_type = dev_type;
	    vc->ipaddr = GETOBJ(paddr,1)->ip_addr;
	    sstrncpy(vc->hostname, GETOBJ(string,1), CIP_NAME_LEN);
	    uppercase(vc->hostname);
	    sstrncpy(vc->cipname, GETOBJ(string,2), CIP_NAME_LEN);
	    uppercase(vc->cipname);
	    sstrncpy(vc->hostappl, GETOBJ(string,3), CIP_NAME_LEN);
	    uppercase(vc->hostappl);
	    sstrncpy(vc->cipappl, GETOBJ(string,4), CIP_NAME_LEN);
	    uppercase(vc->cipappl);

	    /* Two additional names for Offload device */
	    if (dev_type == CHANNEL_OFFLOAD) {
		sstrncpy(vc->hostappl2, GETOBJ(string,5), CIP_NAME_LEN);
		uppercase(vc->hostappl2);
		sstrncpy(vc->cipappl2, GETOBJ(string,6), CIP_NAME_LEN);
		uppercase(vc->cipappl2);
	    }
 	    /*
 	     * set broadcast flag if desired for claw or offload subchannel
 	     */
 	    if (GETOBJ(int,3))
 		vc->flags |= CIP_VC_BRDCST;
	    break;
	case CHANNEL_DP:
	case CHANNEL_IVORIE:	    
	    vc->dev_type = dev_type;
	    vc->ipaddr = GETOBJ(paddr,1)->ip_addr;
	    vc->ipaddr2 = GETOBJ(paddr,2)->ip_addr;
	    break;
	default:
	    break;
	}
	vc->ack_flags |= CIP_CFG_CREATE;
	cip_send_subchannel(hwidb, cip, vc);
    }
}
static cta_tt_cmdtype *cip_find_cta_tt (
    cip_idb_infotype *cip, 
    uint              command,
    uint              offset)
{
    cta_tt_cmdtype *cta_tt_p;

    FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO(cip, cta_tt_p) {
	if ((cta_tt_p->command == command) && 
	    (cta_tt_p->offset == offset)) {
	    break;
	}
    }
    return (cta_tt_p);
}
static void cip_cta_tt_remove (
    cip_idb_infotype *cip, 
    uint              command,
    uint              offset)
{
    cta_tt_cmdtype *cta_tt_p, *cta_tt_prev;

    cta_tt_prev = NULL;
	    
    FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO(cip, cta_tt_p) {
	if ((cta_tt_p->command == command) && 
	    (cta_tt_p->offset == offset)) {
	    remqueue( &cip->cip_cta_ttQ, cta_tt_p, cta_tt_prev);
	    free(cta_tt_p);
	    break;
	}	
	cta_tt_prev = cta_tt_p;
    }
}
static void cip_cta_tt_add (
    cip_idb_infotype *cip, 
    cta_tt_cmdtype   *cta_tt_p)
{
    enqueue(&cip->cip_cta_ttQ, cta_tt_p);
}

/*
 * channel_interface_cta_tt_cmd
 *
 * command  = OBJ(int,1)
 * offset   = OBJ(int,2)
 * length   = OBJ(int,3)
 * value    = OBJ(string,1)
 */
void channel_interface_cta_tt_cmd (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    cta_tt_cmdtype *cta_tt_p;
    cip_idb_infotype *cip;
    uint     command, offset, length;
    int i;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
   
    cip = cip_get_or_create_info(hwidb);
    if (!cip) {
        if (!csb->nvgen) {
          printf("\n%%Couldn't create info\n");
        }
        return;
    }

    /* Process any queued events */
    cip_process_queued_events(hwidb, cip);

    /*
     * generate config commands for nvram 
     */
    if (csb->nvgen) {
        FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO(cip, cta_tt_p) {
	    nv_write(TRUE, "%s %04x %d ",
	             csb->nv_command, cta_tt_p->command,
                     cta_tt_p->offset);
            for(i=0;i<cta_tt_p->length;i++)
                nv_add(TRUE, "%02x", cta_tt_p->value[i]);
	}
	return;
    }
    
    command = GETOBJ(int,1);
    offset  = GETOBJ(int,2);
    
    cta_tt_p = cip_find_cta_tt(cip, command, offset);
   
    /* See if anything PNDING on this */    
    if (cta_tt_p && (cta_tt_p->ack_flags & CIP_CFG_PNDING)) {
	printf("\n%%Configuration of %s %04X %d is pending...",
	       cip_cfg_name_str(csb->which),
	       command, offset);
	return;
    }
    
    if (!csb->sense) {
	if (cta_tt_p) {
	    cta_tt_p->ack_flags |= CIP_CFG_DELETE;
	    cip_send_cta_tt_command(hwidb, cip, cta_tt_p,
				    csb->which | CIP_TEARDOWN_VC);
	} else {
            printf("\n%%Not found. \'no\' command failed.\n");
	}
	return;
    }
    
    if (cta_tt_p) {
	printf("\n%%Duplicate.  Not added.\n");
	return;
    }
    
    length = GETOBJ(int,3);
    cta_tt_p = malloc(sizeof(cta_tt_cmdtype));
    if (cta_tt_p) {
	cta_tt_p->command = command;
	cta_tt_p->offset  = offset;
	cta_tt_p->length  = length;
	
	bcopy(GETOBJ(string,1), cta_tt_p->value, length);
	cip_cta_tt_add(cip, cta_tt_p);
	cta_tt_p->ack_flags |= CIP_CFG_CREATE;
        cip_send_cta_tt_command( hwidb, cip, cta_tt_p, csb->which);
    } else {
	printf(nomemory);
    }
}

/*
 * channel_rate_command
 *
 * rate = OBJ(string,1)
 */
void channel_rate_command (parseinfo *csb)
{
    idbtype   *swidb;
    hwidbtype *hwidb;
    char *value;
    int rate, i;
    cip_idb_infotype *cip;

    swidb = csb->interface;
    hwidb = swidb->hwptr;
    
    cip = cip_get_or_create_info(hwidb);
    if (!cip) {
        if (!csb->nvgen) {
          printf("\n%%Couldn't create info\n");
        }
        return;
    }

    if (cip->daughter_type != CIP_PCA_DAUGHTER) {
	if (!csb->nvgen)
	    printf("\n%%Command only valid for PCA adapter cards");
	return;
    }
	
    if (csb->nvgen) {
	if (cip->data_rate != CIP_PCA_DEFAULT_RATE) {
	    nv_write(TRUE, "%s %s", csb->nv_command,
		     pca_rates[cip->data_rate].name);
	}
	return;
    }

    rate = CIP_PCA_DEFAULT_RATE;    
    if (csb->sense) {
	value = GETOBJ(string, 1);
 	uppercase(value);
	rate = -1;
	for (i=0; i < CIP_NUM_PCA_RATES; i++) {
	    if (strcmp(value, pca_rates[i].name) == 0) {
		rate = i;
		break;
	    }
	}
	if (rate < 0) {
	    printf("%%invalid rate %s\n",value);
	    return;
	}
    }
    /* 
     * send it down
     */
    if (!(swidb->hwptr->state == IDBS_ADMINDOWN))
	if (!cip_send_rate(swidb->hwptr, cip, 
			   pca_rates[rate].rate_code)) {
	    printf("\n%%Channel data transfer rate not changed.");
	    return;
	}
    cip->data_rate = rate;
    switch (pca_rates[rate].rate_code) {	
    case CIP_PCA_RATE_3:
	set_default_interface_bandwidth(swidb, VISIBLE_CIP_PCA_BANDWIDTH_3);
	set_default_interface_delay(swidb, CIP_PCA_DELAY_3);
	break;
    case CIP_PCA_RATE_4_5:
	set_default_interface_bandwidth(swidb, VISIBLE_CIP_PCA_BANDWIDTH_4_5);
	set_default_interface_delay(swidb, CIP_PCA_DELAY_4_5);
	break;
    }
}

/*
 * cip_send_cfg
 *
 * send all the config command for an interface
 */
static void cip_send_cfg (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;
    cip_vctype *vc;
    cta_tt_cmdtype *cta_tt_p;

    if (channel_debug)
	buginf("\n%s: sending entire config", hwidb->hw_namestring);

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return; /* no config to send */
    }
    
    if (!cip->cfg_block)
	cip->cfg_block = cip_get_cfg_block(hwidb);
    cip_send_nodeid(hwidb, cip, &cip_nodeid, CIP_NODEID_SIZE);
    
    if (cip->daughter_type == CIP_PCA_DAUGHTER) { 
	if (!cip_send_rate(hwidb, cip, 
			   pca_rates[cip->data_rate].rate_code)) {
	    errmsg(&msgsym(CIPDTRFAIL, CBUS), 
		   hwidb->hw_namestring, pca_rates[cip->data_rate].name);
	    cip_default_pca_rate(hwidb->firstsw, cip);
	}
    }
    
    /* 
     * Periodically the CIP sends statistics (via a love-letter). 
     * Here we set the interval (in seconds). 
     */
    if (cip_stats_interval < CIP_MIN_STATS_INTERVAL) 
	cip_stats_interval = CIP_DEFAULT_STATS_INTERVAL;

    cip_send_stats_interval(hwidb, cip_stats_interval);

    /* Send all the configuration commands */
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if (vc->flags & CIP_VC_INUSE) 
	    if ((!vc->claw_pair) || (!(vc->device%2 == 1))) {
		vc->ack_flags |= CIP_CFG_CREATE;
		cip_send_subchannel(hwidb, cip, vc);
	    }
    }
    
    FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO(cip, cta_tt_p) {
	cta_tt_p->ack_flags |= CIP_CFG_CREATE;
        cip_send_cta_tt_command(hwidb, cip, cta_tt_p,
				CHANNEL_CTA_TEST_TOOL);
    }
    cip_vlan_send_vlan_cfg(hwidb->firstsw);

    cip_tn3270s_send_cfg(hwidb->firstsw);

}


/*
 * cip_clear_counters
 */
static void cip_clear_counters (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;
    cip_vctype *vc;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }

    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) 
	bcopy(&vc->stats, &vc->si_stats, sizeof(statistics));
}

/*
 * get_cip_appl_version - Retrieve CIP application version 
 * 
 * Input: search = SEARCH_NEXT or SEARCH_MATCH
 *          slot = pointer to slot index
 *       cip_appl_version = pointer to data retrieved
 * Output: Truth Value 
 */
boolean get_cip_appl_version ( int search, long *slot, 
			      cip_appl_version_t *cip_appl_version) {
    hwidbtype *hwidb;

    if (search == SEARCH_MATCH) {
	FOR_ALL_HWIDBS ( hwidb) {
	    if (is_cip(hwidb) && (*slot == hwidb->slot) && 
		(hwidb->state != IDBS_ADMINDOWN)) {
		if (get_from_cip_appl_version(hwidb, cip_appl_version)) {
		    *slot = hwidb->slot;
		    return(TRUE);
		}
		else {
		    /* 
		     * can't allow failure of an exact match ...
		     */
		    return(FALSE);
		}
	    }
	}
	/*
	 * No matching IDB found
	 */
	return(FALSE);
    }
    else {
	/* get next */
	FOR_ALL_HWIDBS ( hwidb) {
	    if (is_cip(hwidb) && (*slot <= hwidb->slot) &&
		(hwidb->state != IDBS_ADMINDOWN)) {
		if (get_from_cip_appl_version(hwidb, cip_appl_version)) {
		    *slot = hwidb->slot;
		    return(TRUE);
		}
		else {
		    /* 
		     * A get_next search failed - Must be at the end of the 
		     * application table.
		     * Bump the slot, yank the appl index to 0 and keep trying
		     */
		    *slot +=1;
		    cip_appl_version->name[0] = '\0';		    
		}
	    }
	}
	/*
	 * No matching IDB found.
	 */
	return(FALSE);
    }

    return(FALSE);
}

/*
 * get_from_cip_appl_version - Retrieve application version info from
 *                               specified CIP card for specified index.
 * Input: hwidb  = pointer to interface 
 *        cip_version_p = pointer to data structure to return version info in.
 * Output: TRUE - if found, FALSE if not.
 */
static boolean get_from_cip_appl_version (hwidbtype *hwidb, 
					  cip_appl_version_t *cip_appl_version_p) {
    netmgt_header_t nm_hdr;

    if (cip_appl_version_p) {
        nm_hdr.class           = SYSMGT_APPL_VERSION;
        nm_hdr.type            = 0;
        nm_hdr.index_length    = 32;
        nm_hdr.data_length     = CIP_APPL_VERSION_SIZE;
        nm_hdr.slot            = hwidb->slot;
        nm_hdr.flags.operation = NETMGT_GET_NEXT;

        if (channel_sysmgt_debug) {
            buginf("\n%s CIP APPL VERSION (%d) (%s)", 
                   netmgt_operations[nm_hdr.flags.operation],
                   nm_hdr.slot,
		   cip_appl_version_p->name);
        }

        if (sysmgt_rpc(hwidb, &nm_hdr, cip_appl_version_p)) 
	    return(TRUE);
    }
    return(FALSE);
}

/*
 * There are platform differences in the ability to use IPC when
 * all interfaces on the card are administratively shutdown.
 *
 * Return TRUE if
 *   - IPC is available regardless of interface state, or
 *   - at least one interface on this card is not administratively shutdown
 */    
static boolean
cip_ipc_capable (ushort slot_index)
{
    hwidbtype *idb;
    ushort port;
    
    if (ipc_avail_when_interfaces_shut()) {
	return (TRUE);
    }

    for (port = 0; port <= CIP_VIRTUAL_PORT; port++) {
	idb = hwidb_lookup(slot_index, port);
	if ((idb) && (idb->state != IDBS_ADMINDOWN)) {
	    return (TRUE);
	}
    }   
    return (FALSE);
}

static void
cip_print_loader_segments (ushort slot_index, hwidbtype *hwidb)
{
    cip_appl_version_t cip_appl_version;
    
    if (!hwidb) {
	return;
    }
    /* 
     * Query the loader segment information via IPC.
     */
    if (cip_ipc_capable(slot_index)) {
	cip_appl_version.name[0] = '\0';
	while (get_from_cip_appl_version(hwidb, &cip_appl_version)) {
	    printf("\n%sLoaded:%15s Rev. %4d %43s",
		   cip_show_util_indentation,	   
		   cip_appl_version.name,
		   cip_appl_version.revision, cip_appl_version.compile_info); 
	}
    }
}

static void
cip_print_pld_rom_ver (hwidbtype *hwidb)
{
    leveltype level;
    ushort vpld_ver, rom_ver;
    
    if (!hwidb) {
	return;
    }
    
    vpld_ver = rom_ver = 0;
    
    level = raise_interrupt_level(NETS_DISABLE);
    
    select_if(hwidb);
    send_pld_version_cmd(hwidb, &vpld_ver);
    send_rom_version_cmd(hwidb, &rom_ver);
    
    reset_interrupt_level(level);

    printf("\n%sEPROM version %d.%d, VPLD version %d.%d", 
	   cip_show_util_indentation,	   
	   rom_ver  >> 8, rom_ver & 0xff,
	   vpld_ver >> 8, vpld_ver & 0xff);
}

static ushort
adapter_info_desc (ushort info_type, ushort port, ushort offset) 
{
    return (offset << 8 | port << 4 | info_type); 
}

static boolean
cip_query_adapter_info (hwidbtype *hwidb, ushort port,
			cip_adptr_info *adapter_info)
{
    leveltype level;
    
    if (!hwidb) {
	return (FALSE);
    }
    
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    if (send_specific_cmd_1arg_lret(hwidb, FCI_CMD_CIP_ADPTR_INFO,
				    adapter_info_desc(FCI_ADPTR_TYPE, port, 0),
				    &adapter_info->type)) {
	reset_interrupt_level(level);
	return (FALSE);
    }

    if (adapter_info->type == CIP_NONE) {
	reset_interrupt_level(level);
	return (FALSE);
    }

    if (send_specific_cmd_1arg_lret(hwidb, FCI_CMD_CIP_ADPTR_INFO,
				    adapter_info_desc(FCI_ADPTR_HWVER, port, 0),
				    &adapter_info->hw_ver)) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    if (send_specific_cmd_1arg_lret(hwidb, FCI_CMD_CIP_ADPTR_INFO,
				    adapter_info_desc(FCI_ADPTR_UCODE, port, 0),
				    &adapter_info->ucode_ver)) {
	reset_interrupt_level(level);
	return (FALSE);
    }

    reset_interrupt_level(level);
    return (TRUE);
}

static void
cip_print_adapter_info (hwidbtype *hwidb)
{
    ushort port;
    cip_adptr_info adapter_info;
    
    /* On the 7000 platform, the microcode version is not available when
     * each of the interfaces is administratively shutdown.  This is because
     * the CIP cannot use IPC to load the segment which contains the channel 
     * adapter microcode.
     * In such a case, the microcode version is returned as 0x00000000.
     */
    for (port = 0; port < CIP_VIRTUAL_PORT; port++) {
	if (cip_query_adapter_info(hwidb, port, &adapter_info)) {
	    printf("\n%s%3s%d: hw version %02d, microcode version ",
		   cip_show_util_indentation,
		   cip_daughter_string(adapter_info.type),
		   port,
		   adapter_info.hw_ver);
	    if (adapter_info.ucode_ver) {
		printf("%08X",
		       adapter_info.ucode_ver);
	    } else {
		printf("n/a");
	    }
	}
    }
}

static boolean
cip_query_memory_info (hwidbtype *hwidb,
		       cip_mem_info *memory_info)
{
    leveltype level;

    if (!hwidb) {
	return (FALSE);
    }

    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    if (send_specific_cmd_1arg_lret(hwidb, FCI_CMD_CIP_CARD_INFO,
				    FCI_SRAM_TOTAL,
				    &memory_info->sram_total)) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    if (memory_info->sram_total) {
	if (send_specific_cmd_1arg_lret(hwidb, FCI_CMD_CIP_CARD_INFO,
					FCI_SRAM_AVAIL,
					&memory_info->sram_avail)) {
	    reset_interrupt_level(level);
	    return (FALSE);
	}
    } else {
	memory_info->sram_avail = 0;
    }
    
    if (send_specific_cmd_1arg_lret(hwidb, FCI_CMD_CIP_CARD_INFO,
				    FCI_DRAM_TOTAL,
				    &memory_info->dram_total)) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    if (send_specific_cmd_1arg_lret(hwidb, FCI_CMD_CIP_CARD_INFO,
				    FCI_DRAM_AVAIL,
				    &memory_info->dram_avail)) {
	reset_interrupt_level(level);
	return (FALSE);
    }
    reset_interrupt_level(level);
    return (TRUE);
}

static void
cip_print_memory_stats (hwidbtype *hwidb, cip_idb_infotype *cip)
{
    cip_mem_info memory_info;
    
    printf("\n%s  Memory    ",
	   cip_show_util_indentation);
    /*
     * Query the CIP for the current memory statistics.
     */   
    if (!cip_query_memory_info(hwidb, &memory_info)) {
	
	/* If unable to retrieve memory statistics from the CIP,
	 * use the statistics that are in a recently received love letter.
	 * If there is no such love letter, indicate this information
	 * is not available.
	 */
	if (cip && cip_get_love_letter_timestamp(cip)) {
	    memory_info.sram_avail = cip->sram_free;
	    memory_info.sram_total = cip->sram_size;
	    memory_info.dram_avail = cip->dram_free;
	    memory_info.dram_total = cip->dram_size;
	} else {
	    printf("n/a");
	    return;
	}
    }
    if (memory_info.sram_total) {
	printf("sram %lu/%dK, ",
	       memory_info.sram_avail,
	       memory_info.sram_total/ONE_K);
    }
    printf("dram %lu/%dM",
	   memory_info.dram_avail,
	   memory_info.dram_total/ONE_MEG);
}

static const char *
cip_percentage_str (ulong value)
{
    static char percentage_string[16];
    
    if (value > 100)
	return(" n/a");
    sprintf(percentage_string,"%-3d%%",
	    value);
    return(percentage_string);
}

static void
cip_print_metrics (const char  *pLabel,
		   cip_metrics *cip_load_metric)
{
    printf("\n%s  %10s",
	   cip_show_util_indentation,
	   pLabel);
    
    printf("1m %s, ",
	   cip_percentage_str(cip_load_metric->one_min));
    printf("5m %s, ",
	   cip_percentage_str(cip_load_metric->five_min));
    printf("60m %s",
	   cip_percentage_str(cip_load_metric->sixty_min));
}

static void
cip_print_channel_metrics (hwidbtype *hwidb)
{
    ushort slot;
    ushort port;
    hwidbtype *idb;
    cip_idb_infotype *cip;
    char adapter_string[8];
    
    slot = hwidb->slot;
    
    for (port = 0; port < CIP_VIRTUAL_PORT; port++) {
	idb = hwidb_lookup(slot, port);
	if (idb) {
	    cip = channel_sb_get_cip_info(idb);
	    sprintf(adapter_string, "%3s%d",
		    cip_daughter_string(cip->daughter_type),
		    port);
	    cip_print_metrics(adapter_string, &cip->cip_channel_load);
	}
    }
}

static cip_idb_infotype *
cip_most_recent_loveletter (ushort slot_index) 
{
    cip_idb_infotype *cip;
    hwidbtype *hwidb;
    cip_idb_infotype *this_cip;
    ushort port;
    ulonglong last_love;
    ulonglong love_time;
    
    cip = NULL;
    last_love = 0;
    
    /*
     * Determine which nonshut interface has the most recent love letter.
     */
    for (port = 0; port <= CIP_VIRTUAL_PORT; port++) {
	hwidb = hwidb_lookup(slot_index, port);
	if ((hwidb) && (hwidb->state != IDBS_ADMINDOWN)) {
            this_cip = channel_sb_get_cip_info(hwidb);
	    if (this_cip) {
		love_time = cip_get_love_letter_timestamp(this_cip);
		if (love_time > last_love) {
		    last_love = love_time;
		    cip = this_cip;
		}
	    }
	}
    }
    return cip;
}

/* 
 * cip_show_cbus
 */
void cip_show_cbus (ushort slot_index)
{
    slottype *slotp;
    hwidbtype *hwidb;
    cip_idb_infotype *cip;
    
    slotp = &slots[slot_index];
    /* 
     * If the card is disabled, we have no relevant
     * information to report.  Also, we will be unable to
     * communicate with it using the cbus.
     */
    if (slotp->flags & BOARD_DISABLED)
	return;

    /* 
     * Display information about the CIP.  This information will come from:
     *   - cbus attn commands that are sent to the CIP (requires a hwidb)
     *   - a recently received love letter from one of the CIP interfaces.
     */
    hwidb = hwidb_lookup(slot_index, CIP_VIRTUAL_PORT);
    cip = cip_most_recent_loveletter(slot_index);
    if (!cip)
        cip = channel_sb_get_cip_info(hwidb);

    cip_print_loader_segments(slot_index, hwidb);
    cip_print_pld_rom_ver(hwidb);
    cip_print_adapter_info(hwidb);
    
    printf("\n%sLoad metrics:",
	   cip_show_util_indentation);

    cip_print_memory_stats(hwidb, cip);
    cip_print_metrics("CPU", &cip->cip_cpu_load);
    cip_print_metrics("DMA", &cip->cip_dma_load);
    cip_print_channel_metrics(hwidb);
}

/*
 * cip_interface
 */ 
static void cip_show_interface (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }

    switch (cip->daughter_type) {
    case CIP_NONE:
	printf("\n  No adapter card installed");
	return;
    case CIP_PCA_DAUGHTER:
	printf("\n  PCA adapter card");
	printf("\n  Data transfer rate %s Mbytes",
	       pca_rates[cip->data_rate].value);
	break;
    case CIP_ECA_DAUGHTER:
	printf("\n  ECA adapter card");
	printf("\n  Data transfer rate %s Mbytes","12");
	break;
    case CIP_VIRTUAL_INTERFACE:
	printf("\n  Virtual interface");
	return;
    default:
	printf("\n  Unknown adapter card");
	return;
    }
    printf(", number of subchannels %d",cip->subchannels);
}

/*
 * show_last_love_time
 */
static void show_last_love_time (cip_idb_infotype *cip)
{
    long last_love;
    long next_love;

    if (!TIMER_RUNNING(cip->last_love_letter)) {
	printf("\n\n  No statistics received since last reset");
	return;    
    }

    last_love = ELAPSED_TIME(cip->last_love_letter) / ONESEC;
    next_love = cip_stats_interval - last_love;
    printf("\n\n  Last statistics ");
    if (next_love > 0) {
	printf("%d seconds old, next in %d seconds", 
	       last_love, next_love > 0 ? next_love : 0);
    } else {
	print_dhms(cip->last_love_letter);
	printf(" old");
    }
}

/* 
 * cip_display_vc 
 */
static void cip_display_vc (cip_vctype *vc)
{
    int type;
    char buf[51];
    ulong last_sense;

    if (!vc)
	return;

    type = vc->dev_type & ~CIP_TEARDOWN_VC;
    if (type <= 0 || type >= CHANNEL_MAXMODE)
	return;

    if (!(vc->flags & CIP_VC_INUSE)) {
	return;
    }
    
    printf("\n  %8s %04X   %02X  ", cip_mode_to_str_inline(type), vc->path,
	   vc->device); 
    last_sense = vc->last_sense;  /* so we don't lose it below */
    switch (type) {
    case CHANNEL_CLAW:
    case CHANNEL_OFFLOAD:
	if (vc->device%2) 
	    vc = vc->claw_pair;  /* bait & switch */
	sprintf(buf,"%15i %8s %8s %8s %8s", vc->ipaddr, vc->hostname,
		       vc->cipname, vc->hostappl, vc->cipappl); 
	break;
    case CHANNEL_CSNA:
	sprintf(buf,"%s %-5d %s %-3d %s %-5d", 
		CSNA_DEVICE_KEY_MAXPIU,   vc->maxpiu,
		CSNA_DEVICE_KEY_TIME_D,   vc->blocking_time_delay,
		CSNA_DEVICE_KEY_LENGTH_D, vc->blocking_length_delay);
	break;
    case CHANNEL_DP:
    case CHANNEL_IVORIE:
	sprintf(buf,"%15i %15i", vc->ipaddr, vc->ipaddr2);
	break;
    }
    printf(" %51s ", buf);
    printf("%04X", last_sense);

    /* Display the Offload API link names */
    if (type == CHANNEL_OFFLOAD) {
	printf("\n%38s %8s %8s"," ",
	       vc->hostappl2,
	       vc->cipappl2); 
    }
}

/* 
 * cip_display_vc_list
 * VC  MODE     Path Device  Pair  Flags
 *  0  CLAW     C700     50     0  IN_USE ADJDEV TEARDN BROADC SROUTE PNDING CUD 
 */

#define BLANKS "      "
static void cip_display_vc_list (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;
    cip_vctype *vc, *vc2;
    int type;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }
    if (cip_interface_is_virtual(hwidb)) {
	cip_tn3270s_display_vc_list(hwidb->firstsw);
	return;
    }
    
    printf("\n\n  VC  MODE     Path Device  Pair  Flags");
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	type = vc->dev_type & ~CIP_TEARDOWN_VC;
	if (type <= 0 || type >= CHANNEL_MAXMODE)
	    printf("\n  Out of range dev_type (%d)", type);
	printf("\n %-3d  %8s %04X     %02X  ", vc->vc_num,
	       cip_mode_to_str_inline(type), vc->path, vc->device); 
	if (vc->claw_pair) {
	    vc2 = vc->claw_pair;
	    printf(" %-3d  ", vc2->vc_num);
	} else {
	    printf("      ");
	}
	printf("%s %s %s %s %s %s",
	       (vc->flags & CIP_VC_INUSE)       ? "IN_USE" : BLANKS, 
	       (vc->flags & CIP_ADJ_DEV)        ? "ADJDEV" : BLANKS,
	       (vc->dev_type & CIP_TEARDOWN_VC) ? "TEARDN" : BLANKS,
	       (vc->flags & CIP_VC_BRDCST)      ? "BROADC" : BLANKS,
	       (vc->flags & CIP_VC_SROUTE)      ? "SROUTE" : BLANKS,
	       (vc->ack_flags & CIP_CFG_PNDING) ? "PNDING" : BLANKS);
	printf(" %c%c%c",
	       (vc->ack_flags & CIP_CFG_CREATE) ? 'C' : ' ',
	       (vc->ack_flags & CIP_CFG_UPDATE) ? 'U' : ' ',
	       (vc->ack_flags & CIP_CFG_DELETE) ? 'D' : ' ');
    }
}

/* 
 * cip_display_cta_test_tool
 */
static void cip_display_cta_test_tool (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;
    cta_tt_cmdtype *cta_tt_p;
    int i;

    printf("\nCommand   Offset   Len   Value");    

    cip = channel_sb_get_cip_info(hwidb);
    if (cip) {
        FOR_ALL_CTA_TT_CMDS_ON_CIP_IDB_INFO(cip, cta_tt_p) {
            printf("\n0x%04x     %-3d      %1d    0x", cta_tt_p->command,
                   cta_tt_p->offset, cta_tt_p->length);
            for(i=0;i<cta_tt_p->length;i++) {
                printf("%02x", cta_tt_p->value[i]);
	    }
        }
    }
    printf("\n");
}

static void cip_display_nd (char *description, nd *nd_ptr)
{
    printf("\n  %s - ", description);

    switch (nd_ptr->flags.validity) {
    case ND_VALID:  
	printf("VALID");
	break;
    case ND_VAL_NOT_CURRENT:
	printf("NOT CURRENT");
	break;
    case ND_NOT_VALID:
	printf("NOT VALID");
	break;
    default:
	break;
    }

    printf("\n    Class: ");
    switch(nd_ptr->class) {
    case 0:
 	printf("%15s","Unspecified");
  	break;
    case CLASS_DIRECT:
	printf("%15s","Channel Path");
	break;
    case CLASS_CTCA:
	printf("%15s","CTCA-standalone");
	break;
    case CLASS_SWITCH:
	printf("%15s","Switch");
	break;
    default:
	printf("unknown(%02d)    ", nd_ptr->class);
	break;
    }

    printf(" Type Number : %6s        Tag: %x",
	   nd_ptr->type_number, nd_ptr->tag);
    printf("\n    Model: %3s             Manufacturer: %3s",
	   nd_ptr->model_number, nd_ptr->manufacturer); 
    printf("\n    Plant: %2s              Sequence    : %12s", 
	   nd_ptr->plant, nd_ptr->sequence); 
}

/*
 * find_next_path
 */
static cip_vctype *find_next_path (cip_idb_infotype *cip, int path,
				   int search_type)
{
    cip_vctype *vc, *save_vc;
    int save_path;

    save_vc = NULL;
    save_path = -1;
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	switch (search_type) {
	case SEARCH_NEXT:
	    /* 
	     * look for the next path
	     */ 
	    if ((int)vc->path > path) {
		if (save_path == -1 || vc->path < save_path) {
		    save_vc = vc;
		    save_path = vc->path;
		}
	    }
	    break;
	case SEARCH_MATCH:
	    /*
	     * look for an exact match
	     */
	    if (vc->path == path)
		return vc;
	    break;
	default:
	}

    }
    return save_vc;
}

/*
 * find_next_path_device
 */
static cip_vctype *find_next_path_device (cip_idb_infotype *cip, int
					  path, int device, int search_type) 
{
    cip_vctype *vc, *save_vc;
    int save_device;

    save_vc = NULL;
    save_device = -1;
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	switch (search_type) {
	case SEARCH_NEXT:
	    if (vc->path == path && (int)vc->device > device) {
		if (save_device == -1 || vc->device < save_device) {
		    save_vc = vc;
		    save_device = vc->device;
		}
	    }
	    break;
	case SEARCH_MATCH:
	    if (vc->path == path && vc->device == device)
		return vc;
	    break;
	default:
	    return NULL;
	}
    }
    return save_vc;
}

/* 
 * cip_display_subchannel
 */
static void cip_display_subchannel (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;
    cip_vctype *vc, *vc_path;
    nd local_nd;
    int i, path, device;
    int slot, slotunit;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }

    printf("\n%s: state %s", hwidb->hw_namestring, print_hwidbstate(hwidb)); 
    printf("\n  Flags:");
    if (cip->db_flags & DB_VALID)
	printf(" VALID");
    else 
	printf(" INVALID");
    if (cip->db_flags & DB_ESCON)
	printf(" ESCON");
    if (cip_daughterBoardStatus(cip))
	printf(" LOADED");
    if (cip->db_flags & DB_END_LOAD)
	printf(" END_LOAD");
    if (cip_daughterBoardOffline(cip))
	printf(" OFFLINE");
    if (cip->db_flags & DB_GO_OFFLINE)
	printf(" GO_OFF");
    if (cip->db_flags & DB_RQC_PENDING)
	printf(" RQC_PEND");
    if (cip->pacb_flags & PA_ENABLED)
	printf(" MEMD_ENABLED");
    if (cip_daughterBoardSignal(cip))
	printf(" SIGNAL");
    if (cip->pacb_flags & PA_SEND_LOVE)
	printf(" LOVE");
    if (cip->db_flags & DB_LOAD_PENDING)
	printf(" LOAD_STARTED");
    printf("\n  Link: %02X, ", cip->link_addr);
    printf("Buffers %lu, ", cip->no_buffers);
    printf("CRC errors %lu, ", cip->crc_errors);
    printf("Load count %lu", cip->load);
    printf("\n  Link Incident Reports\n    ");
    for (i=0; i < MAX_LIR_TYPES; i++) {
	if (i != 0)
	    printf(", ");
	if (i == 3)
	    printf("\n    ");
	printf("%s %lu",lir_strings[i], cip->link_incident[i]);
    }

    cip_display_nd("Neighbor Node", (nd *)&cip->node_id[0]);
    
    memset(&local_nd, 0, sizeof(nd));

    local_nd.flags.validity = ND_VALID;
    local_nd.class = CLASS_CTCA;
    cbus_unit2slotunit(hwidb->mci_index, &slot, &slotunit);
    local_nd.tag = slot;
    local_nd.tag <<= 4;
    local_nd.tag += slotunit;
    bcopy(cip_nodeid.type_number, local_nd.type_number, 6);
    bcopy(cip_nodeid.model_number, local_nd.model_number, 3);
    bcopy("CSC", local_nd.manufacturer, 3);
    bcopy(cip_nodeid.plant, local_nd.plant, 2);
    bcopy(cip_nodeid.sequence, local_nd.sequence, 12);
    cip_display_nd("Local Node", &local_nd);

    /*
     * apply more brute force(tm) to print vc's sorted by path, device
     */
    if (cip->subchannels) {
	printf("\n%-79s\n  Mode     Path Device %50s Sense","Last"," ");	
	path = -1;
	while ((vc_path = find_next_path(cip, path, SEARCH_NEXT)) != NULL) {
	    path = vc_path->path;
	    device = -1;
	    while ((vc = find_next_path_device(cip, path, device, SEARCH_NEXT)) != NULL) {
		device = vc->device;
		cip_display_vc(vc);
	    }
	    if (automore_quit())
		break;
	}
    } else {
	printf("\n\n  No subchannels configured");
    }
}

/*
 * print_long_count
 */
static char *print_long_count (ulonglong *nump, char *p)
{
    ulonglong num = *nump;
    ulonglong one_billion = 1000000000L;

    if (num < one_billion)
        sprintf(p,"%-9u",(ulong)num);
    else if ((num >> 10) < one_billion)
        sprintf(p, "%-8uK", (ulong)(num >> 10));
    else if ((num >> 20) < one_billion)
        sprintf(p, "%-8uM", (ulong)(num >> 20));
    else
        sprintf(p, "%-8uG", (ulong)(num >> 30));
    return(p);
}

/*
 * cip_zero_totals
 */
static inline void cip_zero_totals (cip_totals *total)
{
    memset(total, 0, sizeof(cip_totals));
}

/* 
 * cip_add_totals
 */
static void cip_add_totals (cip_totals *src_tot, cip_totals *dst_tot)
{
    dst_tot->blk_read += src_tot->blk_read;
    dst_tot->blk_write += src_tot->blk_write;
    dst_tot->byte_read += src_tot->byte_read;
    dst_tot->byte_write += src_tot->byte_write;
    dst_tot->drop_read += src_tot->drop_read;
    dst_tot->drop_write += src_tot->drop_write;
    dst_tot->failed_memd += src_tot->failed_memd;
}

/* 
 * cip_print_totals
 */
static void cip_print_totals (char *string, cip_totals *total)
{
    char printbuf1[81], printbuf2[81];

    print_long_count(&total->byte_read, printbuf1);
    print_long_count(&total->byte_write, printbuf2);
    printf("\n%s %-10u %-10u %9s %9s %-10u %-10u %-5d",
	   string, total->blk_read, total->blk_write, printbuf1,
	   printbuf2, total->drop_read, total->drop_write, total->failed_memd);
}

/*
 * cip_num_mode_links
 *
 * returns the number of links that a mode uses
 */ 
static int cip_num_mode_links (int mode)
{
    switch (mode) {
    case CHANNEL_OFFLOAD:
	return 3;
    case CHANNEL_CLAW:
	return 2;
    case CHANNEL_CSNA:
    case CHANNEL_LCS:
    case CHANNEL_IVORIE:
    case CHANNEL_DP:
	return 1;
    default:
	return 0;
    }

}


/* 
 * cip_nonzero_data_stats
 *
 *    This function checks statistics.data_stats[link] to determine if it
 *      contains any nonzero data.
 *
 */
static boolean cip_nonzero_data_stats (cip_vctype *vc, long link)
{
    statistics *stats;
    data_stats *data;
    
    if (link < CLAW_MAX_LINKS && link >= 0) {      
	stats = &vc->stats;
	data = &stats->data[link];
	if (data->connected ||          
	    data->blocks_read != 0 ||
	    data->blocks_written != 0 ||
	    data->failed_memd_buf != 0 ) {
	    
	    return TRUE;
	} else {
	    return FALSE;
	}
    } else {
	return FALSE; 
    }    
}                   


#define si_val(variable) (stats-> ## variable - si_stats-> ## variable)
#define si_dval(variable) (data-> ## variable - si_data-> ## variable)


/* 
 * cip_print_data_stats
 */
static void cip_print_data_stats (
				  long link, 
				  data_stats *data, 
				  data_stats *si_data, 
				  cip_vctype *vc, 
				  cip_totals *tot_ptr)
{
    ulonglong bytes_read, bytes_written;
    ulonglong si_bytes_read, si_bytes_written;
    ulonglong temp_value;
    uchar printbuf1[16], printbuf2[16];
    leveltype level;
    
    /*      
     * Net interrupts are disabled while copying to longlong so we 
     * don't get clobbered by a new love letter.
     */
    level = raise_interrupt_level(NETS_DISABLE); 
    bytes_read = data->bytes_read_high;
    bytes_read <<= 32;
    bytes_read += data->bytes_read_low;
    bytes_written = data->bytes_written_high;
    bytes_written <<= 32;
    bytes_written += data->bytes_written_low;
    reset_interrupt_level(level);
    
    si_bytes_read = si_data->bytes_read_high;
    si_bytes_read <<= 32;
    si_bytes_read += si_data->bytes_read_low;
    si_bytes_written = si_data->bytes_written_high;
    si_bytes_written <<= 32;
    si_bytes_written += si_data->bytes_written_low;
    
    temp_value = bytes_read - si_bytes_read;
    print_long_count(&temp_value, printbuf1);
    temp_value = bytes_written - si_bytes_written;
    print_long_count(&temp_value, printbuf2);
    printf("\n %02x-%02x %-10u %-10u %9s %9s %-10u %-10u %-5d %-3s",
	   vc->device, 
	   link, 
	   si_dval(blocks_read),
	   si_dval(blocks_written),
	   printbuf1,
	   printbuf2,
	   si_dval(read_blocks_dropped),
	   si_dval(write_blocks_dropped), 
	   si_dval(failed_memd_buf),
	   data->connected ? "Y" : "N");
    tot_ptr->blk_read += si_dval(blocks_read);
    tot_ptr->blk_write += si_dval(blocks_written);
    tot_ptr->byte_read += bytes_read - si_bytes_read;
    tot_ptr->byte_write += bytes_written - si_bytes_written;
    tot_ptr->drop_read += si_dval(read_blocks_dropped);
    tot_ptr->drop_write +=  si_dval(write_blocks_dropped);
    tot_ptr->failed_memd += si_dval(failed_memd_buf);
}


/* 
 * cip_display_stats
 */
static void cip_display_stats (hwidbtype *hwidb, int path, int device,
			       int search_type)
{
    cip_idb_infotype *cip;
    cip_vctype *vc, *vc_path;
    statistics *stats, *si_stats;
    data_stats *data, *si_data;
    uchar printbuf1[24];  
    long i;
    int num_devices, num_paths;
    cip_totals total[NUM_TOTALS], *tot_ptr;
    int dev_search;
    boolean totals_printed, print_path_header;
    
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }
    
    if (cip->subchannels == 0) {
	printf("\n\n  No subchannels configured");
	return;
    }
    
    cip_zero_totals(&total[TOT_DB]);
    
    /*
     * Since there are normally not more that 8 devices, we will use a
     * dreaded n^2 algorthm to print devices sorted by path, device.
     */
    if (device == -1)
	dev_search = SEARCH_NEXT;
    else
	dev_search = search_type;
    num_paths = 0;
    totals_printed = FALSE;
    while ((vc_path = find_next_path(cip, path, search_type)) != NULL) {
	path = vc_path->path;
	cip_zero_totals(&total[TOT_PATH]);
	if (dev_search != SEARCH_MATCH)
	    device = -1;
        print_path_header = TRUE;
	while ((vc = find_next_path_device(cip, path, device, dev_search)) != NULL) {
	    device = vc->device;
	    if (!(vc->flags & CIP_VC_INUSE)) {
	        if (dev_search == SEARCH_MATCH) {
		    break;
	        } else {
                    continue;
	        }
	    }
            if (print_path_header) {
                print_path_header = FALSE;
	        /* pause after every path, except 1st */
	        if (num_paths && (search_type != SEARCH_MATCH))
	            automore_conditional(0);
	        num_paths++;
	        printf("\n\nPath: %04X  -- %s", 
	               path, (vc->path_flags & PATH_ESTABLISHED) 
		       ? "ESTABLISHED" : "NOT ESTABLISHED");
	        printf("\n                  Command             Selective     System"
	               "     Device        CU");
	        printf("\nDev   Connects    Retries    Cancels      Reset      Reset"
	               "     Errors       Busy");
	    }
	    stats = &vc->stats;
	    si_stats = &vc->si_stats;
	    printf("\n %02x %-10u %-10u %-10u %-10u %-10u %-10u %-10u",
		   vc->device, 
		   si_val(connections), si_val(cmd_retries), 
		   si_val(cancels), si_val(sel_resets),
		   si_val(system_resets), si_val(dev_errors),
		   si_val(cu_busy));
	    if (dev_search == SEARCH_MATCH)
		break;  
	}
	if (dev_search != SEARCH_MATCH)
	    device = -1;
	num_devices = 0;
        print_path_header = TRUE;
	while ((vc = find_next_path_device(cip, path, device, dev_search)) != NULL) {
	    device = vc->device;
	    if (!(vc->flags & CIP_VC_INUSE)) {
	        if (dev_search == SEARCH_MATCH) {
		    break;
	        } else {
                    continue;
	        }
	    }
	    tot_ptr = &total[TOT_DEV];
	    cip_zero_totals(tot_ptr);
            if (print_path_header) {
                print_path_header = FALSE;
	        printf("\n\n                 Blocks               Bytes              "
	               "Dropped Blk    Memd");
	        printf("\nDev-Lnk      Read      Write      Read     Write       "
	               "Read      Write  wait Con");
	    }  else if ((vc->claw_pair != NULL) & !totals_printed) {
		/* need to print a blank line if we are starting a claw pair and
		   were not preceeded by a total or header */
		printf("\n");
            }     
            num_devices++;
	    if (totals_printed) {           
  	        printf("\n");  
		totals_printed = FALSE;
	    }
	    stats = &vc->stats;
	    si_stats = &vc->si_stats;
	    /*
	     * Need to search for valid stat data.  It should not be      
	     * assumed that the first n elements of data[] are the valid  
	     * ones.  For example, Offload may use the 3rd & 4th instead. 
	     * Also, some features (eg: IP Datagram Checksum Offload) may 
	     * use additional elements.                                   
	     */
	    for (i = 0; i < CLAW_MAX_LINKS; i++) {      /* for all possible stats */
		/*
		 * Print the first n data_stats as well as any nonzero ones. 
		 */
		if (i < cip_num_mode_links(vc->dev_type) ||
		    cip_nonzero_data_stats(vc, i)) { 
		    data = &stats->data[i];
		    si_data = &si_stats->data[i];        
		    cip_print_data_stats(i, data, si_data, vc, tot_ptr);
		}
	    }
	    cip_add_totals(tot_ptr, &total[TOT_PATH]);
	    /* don't print device totals for just 1 device */
	    if (vc->flags & CIP_ADJ_DEV) {
		cip_print_totals("Total:", tot_ptr);
		totals_printed = TRUE;
	    }
	    if (dev_search == SEARCH_MATCH)
		break;
	}
	cip_add_totals(&total[TOT_PATH], &total[TOT_DB]);
	if (num_devices > 1) {
	    sprintf(printbuf1, "\nPath %04X\nTotal:", path);
	    cip_print_totals(printbuf1, &total[TOT_PATH]);
	}
	if (search_type == SEARCH_MATCH)
	    break;   
	if (automore_quit())
	    break;
    }
    if (num_paths > 1)
	cip_print_totals("\nAdapter Card\nTotal:", &total[TOT_DB]);
}


/*
 * cip_clear_si_stats
 * 
 * after a interface reset, the stats need to be zeroed
 */
static void cip_clear_si_stats (cip_idb_infotype *cip)
{
    cip_vctype *vc;

    if (!cip)
	return;

    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	memset(&vc->stats, 0, sizeof(statistics));
	memset(&vc->si_stats, 0, sizeof(statistics));
    }
}
/* 
 * show_channel_commands
 */
void show_extended_channel_commands (parseinfo *csb)
{
    idbtype   *swidb;
    hwidbtype *hwidb;
    cip_idb_infotype *cip;
    int stat_type;
    int path, device;
    int search_type;
    
    swidb = GETOBJ(idb,1);
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	printf("\n  No subchannels configured");
	return;
    }

    automore_enable(NULL);
    switch (csb->which) {
    case CHANNEL_SUBCHANNEL:
	cip_display_subchannel(hwidb);
	break;
    case CHANNEL_STATISTICS:
	path = device = -1;
	stat_type = GETOBJ(int,1);
	search_type = SEARCH_NEXT;
	switch (stat_type) {
	case CHANNEL_STATS_DEFAULT:
	    search_type = SEARCH_NEXT;
	    break;
	case CHANNEL_STATS_PATH:
	    search_type = SEARCH_MATCH;
	    path = GETOBJ(int,2);
	    break;
	case CHANNEL_STATS_DEVICE:
	    search_type = SEARCH_MATCH;
	    path = GETOBJ(int,2);
	    device = GETOBJ(int,3);
	    break;
	default:
	    search_type = SEARCH_NEXT;
	}
	cip_display_stats(hwidb, path, device, search_type);
	break;
    case CHANNEL_VC:
	cip_display_vc_list(hwidb);
	break;
    case CHANNEL_CTA_TT:
	cip_display_cta_test_tool(hwidb);
	break;
    default:
	break;
    }

    show_last_love_time(cip);
    automore_disable();
}


static void cip_display_tcp (cip_tcp_t *tcp_p)
{
    printf("\n\nTCP Statistics for IP Address %15i", tcp_p->index.cipIpAddress);


    printf("\n  RtoAlgorithm: ");
    switch(tcp_p->cipTcpRtoAlgorithm) {
    case 1: printf("%10s", "other");
            break;
    case 2: printf("%10s", "constant");
            break;
    case 3: printf("%10s", "rsre");
            break;
    case 4: printf("%10s", "vanj");
            break;
    default: printf("%10s", "unknown");
             break;
    }

    printf("   RtoMin      : %10d   RtoMax      : %10d",
	    tcp_p->cipTcpRtoMin, tcp_p->cipTcpRtoMax);

    printf("\n  MaxConn     : %10d   ActiveOpens : %10d   PassiveOpens: %10d",
	   tcp_p->cipTcpMaxConn, tcp_p->cipTcpActiveOpens, tcp_p->cipTcpPassiveOpens);

    printf("\n  AttemptFails: %10d   EstabResets : %10d   CurrEstab   : %10d",
	   tcp_p->cipTcpAttemptFails, tcp_p->cipTcpEstabResets, tcp_p->cipTcpCurrEstab);

    printf("\n  InSegs      : %10d   OutSegs     : %10d   RetransSegs : %10d",
	   tcp_p->cipTcpInSegs, tcp_p->cipTcpOutSegs, tcp_p->cipTcpRetransSegs);

    printf("\n  InErrs      : %10d   OutRsts     : %10d",
	   tcp_p->cipTcpInErrs, tcp_p->cipTcpOutRsts);

} /* cip_display_tcp  */


/*
 * TCP/IP Connection states
 */
static char *const tcpConnState[] = {
    "unknown",
    "closed",
    "listen",
    "synSent",
    "synReceive",
    "establish",
    "finWait1",
    "finWait2",
    "closeWait",
    "lastAck",
    "closing",
    "timeWait",
    "deleteTCB"
    };
#define MAX_TCPCONN_STATES sizeof(tcpConnState)/sizeof(char *)

static void cip_display_tcpconn (cip_tcp_conn_t *tcp_conn_p)
{
    char in_bytes_count_buf[20], out_bytes_count_buf[20];
    ulonglong  in_bytes, out_bytes;
    
    /*
     * Convert TCP connection byte counts from two 32bit
     * values to a 64bit value.
     */
    in_bytes = ((ulonglong)tcp_conn_p->cipTcpConnInHCBytes_msw << 32) + 
                tcp_conn_p->cipTcpConnInHCBytes_lsw;
    out_bytes = ((ulonglong)tcp_conn_p->cipTcpConnOutHCBytes_msw << 32) + 
                 tcp_conn_p->cipTcpConnOutHCBytes_lsw;
    
    /*
     * Validate TCP Connection state value
     */
    if ((tcp_conn_p->cipTcpConnState < 1) || 
        (tcp_conn_p->cipTcpConnState >= MAX_TCPCONN_STATES))
        tcp_conn_p->cipTcpConnState = 0;

    /*
     * Print TCP Connection information
     */
    printf("\n%15i %5d %15i %5d  %10s  %s   %s",
	    tcp_conn_p->index.cipIpAddress, 
            tcp_conn_p->index.cipTcpConnLocalPort,
            tcp_conn_p->index.cipTcpConnRemAddress, 
            tcp_conn_p->index.cipTcpConnRemPort,
            tcpConnState[tcp_conn_p->cipTcpConnState],
            print_long_count(&in_bytes, in_bytes_count_buf),
            print_long_count(&out_bytes, out_bytes_count_buf));
} /* cip_display_tcpconn  */


static void cip_display_ip (cip_ip_t *ip_p)
{
    printf("\n\nIP Statistics for IP Address %15i", ip_p->index.cipIpAddress);


    printf("\n  Forwarding     : ");
    switch(ip_p->cipIpForwarding) {
    case 1: printf("%10s", "yes");
            break;
    case 2: printf("%10s", "no");
            break;
    default: printf("%10s", "unknown");
             break;
    }
   
    printf("   DefaultTTL     : %10d   InReceives   : %d",
	   ip_p->cipIpDefaultTTL, ip_p->cipIpInReceives);

    printf("\n  InHdrErrors    : %10d   InAddrErrors   : %10d   ForwDatagrams: %10d",
	    ip_p->cipIpInHdrErrors, ip_p->cipIpInAddrErrors, ip_p->cipIpForwDatagrams);

    printf("\n  InUnknownProtos: %10d   InDiscards     : %10d   InDelivers   : %10d",
	   ip_p->cipIpInUnknownProtos, ip_p->cipIpInDiscards);

    printf("\n  OutRequests    : %10d   OutDiscards    : %10d   OutNoRoutes  : %10d",
	   ip_p->cipIpOutRequests, ip_p->cipIpOutDiscards, ip_p->cipIpOutNoRoutes);

    printf("\n  ReasmTimeout   : %10d   ReasmReqds     : %10d   ReasmOKs     : %10d",
	   ip_p->cipIpReasmTimeout, ip_p->cipIpReasmReqds, ip_p->cipIpReasmOKs);

    printf("\n  ReasmFails     : %10d   FragOKs        : %10d   FragFails    : %10d",
	   ip_p->cipIpReasmFails, ip_p->cipIpFragOKs, ip_p->cipIpFragFails);

    printf("\n  FragCreates    : %10d   RoutingDiscards: %10d",
	   ip_p->cipIpFragCreates, ip_p->cipIpRoutingDiscards);
} /* cip_display_ip  */


static void cip_display_udp (cip_udp_t *udp_p)
{
    printf("\n\nUDP Statistics for IP Address %15i", udp_p->index.cipIpAddress);


    printf("\n  InDatagrams : %10d   NoPorts     : %10d",
	   udp_p->cipUdpInDatagrams, udp_p->cipUdpNoPorts);

    printf("\n  InErrors    : %10d   OutDatagrams: %10d",
	   udp_p->cipUdpInErrors, udp_p->cipUdpOutDatagrams);

 } /* cip_display_udp  */


static void cip_display_udp_listeners (cip_udp_listeners_t *udp_listener_p)
{
    printf("\nUDP Listener: IP Address %15i    LocalPort %10d", 
            udp_listener_p->index.cipIpAddress, 
            udp_listener_p->index.cipUdpLocalPort);

} /* cip_display_udp_listeners  */


static void cip_display_icmp (cip_icmp_t *icmp_p)
{
    printf("\n\nICMP Statistics for IP Address %15i", icmp_p->index.cipIpAddress);

    printf("\n  InMsgs         : %10d   InErrors        : %10d   InDestUnreachs: %10d",
	   icmp_p->cipIcmpInMsgs, icmp_p->cipIcmpInErrors, icmp_p->cipIcmpInDestUnreachs);

    printf("\n  InTimeExcds    : %10d   InParmProbs     : %10d   InSrcQuenchs  : %10d",
	   icmp_p->cipIcmpInTimeExcds, icmp_p->cipIcmpInParmProbs, icmp_p->cipIcmpInSrcQuenchs);

    printf("\n  InRedirects    : %10d   InEchos         : %10d   OutEchoReps   : %10d",
	   icmp_p->cipIcmpInRedirects, icmp_p->cipIcmpInEchos, icmp_p->cipIcmpOutEchoReps);

    printf("\n  OutTimestamps  : %10d   OutTimestampReps: %10d   OutAddrMasks  : %10d",
	   icmp_p->cipIcmpOutTimestamps, icmp_p->cipIcmpOutTimestampReps, icmp_p->cipIcmpOutAddrMasks);

    printf("\n  OutAddrMaskReps: %10d",
	   icmp_p->cipIcmpOutAddrMaskReps);
} /* cip_display_icmp  */

/* 
 * show_extended_channel_tcpconns_command
 */
void show_extended_channel_tcpconns_command (parseinfo *csb)
{
    idbtype *swidb_p;
    hwidbtype *hwidb_p;
    cip_idb_infotype *cip;
    int search_type, index_type, display_type;
    cip_tcp_conn_t tcp_conn, *tcp_conn_p;
    ulong local_ipaddr, remote_ipaddr;
    ulong wc_local_ipaddr;
    long local_port, remote_port;
    uint count = 0;
    ulonglong in_bytes  = 0;
    ulonglong out_bytes = 0;
    char in_bytes_count_buf[20], out_bytes_count_buf[20];
    boolean wc_lport_flag, found;
    boolean quit = FALSE;
    
    swidb_p = GETOBJ(idb,1);
    hwidb_p = swidb_p->hwptr;

    cip = channel_sb_get_cip_info(hwidb_p);
    if (!cip) {
	printf("\n  No subchannels configured");
	return;
    }

    /*
     * Initialize variables
     */
    display_type  = GETOBJ(int,4);    /* summary or detail display */
    index_type    = GETOBJ(int,1);    /* level of index selected   */
    search_type   = SEARCH_NEXT;
    wc_lport_flag = GETOBJ(int,5);    /* local port wild card flag */
    local_ipaddr  = 0;
    local_port    = 0;
    remote_ipaddr = 0;
    remote_port   = 0;
    
    /*
     * .........WARNING WARNING WARNING........
     * Get index parameters. This switch statement
     * intentionally falls through to the default.
     */
    switch (index_type) {
      case CHANNEL_SHOW_TCPCONNS_FOREIGN_PORT:
        search_type = SEARCH_MATCH;
        remote_port = (long) GETOBJ(int,3);
        /* NOTICE!!! This should fall through to the next case */
      case CHANNEL_SHOW_TCPCONNS_FOREIGN_ADDR:
        remote_ipaddr = GETOBJ(paddr,2)->ip_addr;
        /* NOTICE!!! This should fall through to the next case */
      case CHANNEL_SHOW_TCPCONNS_LOCAL_PORT:
        if (wc_lport_flag)
            search_type = SEARCH_NEXT;
        else
            local_port  = (long) GETOBJ(int,2);
        /* NOTICE!!! This should fall through to the next case */
      case CHANNEL_SHOW_TCPCONNS_LOCAL_ADDR:
        local_ipaddr = GETOBJ(paddr,1)->ip_addr;
      default:
        break;
    }
    wc_local_ipaddr = local_ipaddr;     /* ge the local port wild card value */
    
    /*
     * Make sure the IDB is up for querying from the CIP
     */
    if (hwidb_p->state == IDBS_ADMINDOWN) {
        printf("%s", if_shutdown);
        return;
    }
    
    automore_enable(NULL);
    
    /*
     * Query TCP connections from CIP card until:
     *  - no more tcp connections found.
     *  - user specified tcp conn index does not
     *    match returned record.
     */
    while ((tcp_conn_p = get_cip_tcp_conn(search_type, hwidb_p, local_ipaddr, 
                                          local_port, remote_ipaddr, remote_port, 
                                          &tcp_conn)) != NULL) {
        found = FALSE;
        /*
         * Validate TCP Conn index values specified
         */
        switch (index_type) {
          case CHANNEL_SHOW_TCPCONNS_FOREIGN_PORT:
              if (remote_port != tcp_conn_p->index.cipTcpConnRemPort)
                  break;
          case CHANNEL_SHOW_TCPCONNS_FOREIGN_ADDR:
              if (remote_ipaddr != tcp_conn_p->index.cipTcpConnRemAddress)
                  break;
          case CHANNEL_SHOW_TCPCONNS_LOCAL_PORT:
              if ((!wc_lport_flag) &&
                  (local_port != tcp_conn_p->index.cipTcpConnLocalPort))
                  break;
          case CHANNEL_SHOW_TCPCONNS_LOCAL_ADDR:
              if (local_ipaddr != tcp_conn_p->index.cipIpAddress)
                  break;
          default:
              /*
               * Display TCP connection record or summarize record
               */
              if (display_type == CHANNEL_SHOW_TCPCONNS_DETAIL) {
                  if (!count) {
                      printf("\nLocal IP Addr   Port  Remote IP Addr  Port   "  \
                             "State        In Bytes   Out Bytes");
                  }
                  cip_display_tcpconn(tcp_conn_p);
              } else {
                  in_bytes  += ((ulonglong)tcp_conn_p->cipTcpConnInHCBytes_msw << 32) + 
                                tcp_conn_p->cipTcpConnInHCBytes_lsw;
                  out_bytes += ((ulonglong)tcp_conn_p->cipTcpConnOutHCBytes_msw << 32) + 
                                tcp_conn_p->cipTcpConnOutHCBytes_lsw;
              }
              found = TRUE;
              count++;
              break;
        }   /* switch (index_type) */

        /*
         * Stop query if record does not match specified index and
         * (wild card local port not specified or
         * wild card local port was specified and local ip address
         * does not match last retrieved TCP conn record).
         */
        if (!found && (!wc_lport_flag || 
            (wc_local_ipaddr != tcp_conn_p->index.cipIpAddress)))
            break;
        
        /*
         * Update indices for next query
         * if wild card specified and either a remote ipaddr
         * or both remote ipaddr and remote port are specified,
         * then only update the local and/or remote port.
         * else update all parameters.
         */
        if (search_type != SEARCH_MATCH) {
            if (wc_lport_flag && (index_type > CHANNEL_SHOW_TCPCONNS_LOCAL_PORT)) {
                if (index_type == CHANNEL_SHOW_TCPCONNS_FOREIGN_ADDR) {
                    local_port    = tcp_conn_p->index.cipTcpConnLocalPort;
                    remote_port   = tcp_conn_p->index.cipTcpConnRemPort;
                    if (!++remote_port) 
                        if (!++local_port)
                            break;
                } else if (index_type == CHANNEL_SHOW_TCPCONNS_FOREIGN_PORT) {
                    local_port    = tcp_conn_p->index.cipTcpConnLocalPort;
                    if (!++local_port)
                        break;
                }
                else {
                }
            }
            else {
                remote_ipaddr = tcp_conn_p->index.cipTcpConnRemAddress;
                local_port    = tcp_conn_p->index.cipTcpConnLocalPort;
                local_ipaddr  = tcp_conn_p->index.cipIpAddress;
                remote_port   = tcp_conn_p->index.cipTcpConnRemPort;
                if (!++remote_port)
                    if (!++remote_ipaddr)
                        if (!++local_port)
                            if (!++local_ipaddr)
                                break;
	    }
        } else {
            break;
        }

        quit = automore_quit();
        if (quit)
            break;
    }

    if (!quit) {
        if (!count) {
               printf("\n\n...No TCP Connection Statistics Found...\n");
	} else if (display_type == CHANNEL_SHOW_TCPCONNS_SUMMARY) {
            printf("\nTCP Connections=%d  Input Bytes=%s  Output Bytes=%s",
                   count,
	           print_long_count(&in_bytes, in_bytes_count_buf),
	           print_long_count(&out_bytes, out_bytes_count_buf));
        }
    }
        
    automore_disable();
} /* show_extended_channel_tcp_conns_command */


/* 
 * show_ext_channel_tcp
 */
static int show_ext_channel_tcp ( hwidbtype *hwidb_p, ulong local_ipaddr, 
                                  int search_type) {
    cip_tcp_t cip_tcp, *cip_tcp_p;
    int count = 0;
    
    while ((cip_tcp_p = get_cip_tcp(search_type, hwidb_p, local_ipaddr, 
                                    &cip_tcp)) != NULL) {
        cip_display_tcp(cip_tcp_p);
        count++; 
        local_ipaddr = ++cip_tcp_p->index.cipIpAddress;
	if ((search_type == SEARCH_MATCH) ||
            automore_quit() || !local_ipaddr) {
	    break;
	}
    }
    return count;
} /* show_ext_channel_tcp */

/* 
 * show_extended_channel_ip_command
 */
static int show_ext_channel_ip ( hwidbtype *hwidb_p, ulong local_ipaddr, 
                                 int search_type) {
    cip_ip_t cip_ip, *cip_ip_p;
    int count = 0;
    
    while ((cip_ip_p = get_cip_ip(search_type, hwidb_p, local_ipaddr, 
                                  &cip_ip)) != NULL) {
        cip_display_ip(cip_ip_p);
        count++; 
        local_ipaddr = ++cip_ip_p->index.cipIpAddress;
	if ((search_type == SEARCH_MATCH) ||
            automore_quit() || !local_ipaddr) {
	    break;
        }
    }
    return count;
} /* show_ext_channel_ip */

/* 
 * show_extended_channel_udp_listeners
 */
static int show_ext_channel_udp_listeners (hwidbtype *hwidb_p,
					   ulong local_ipaddr, 
					   int search_type) {
    cip_udp_listeners_t cip_udp_listener, *cip_udp_listener_p;
    long local_port = 0;
    int count = 0;
    
    while ((cip_udp_listener_p = get_cip_udp_listeners(SEARCH_NEXT, hwidb_p, 
                                                       local_ipaddr, local_port, 
                                                       &cip_udp_listener)) != NULL) {
        if ((search_type == SEARCH_MATCH) && 
            (local_ipaddr != cip_udp_listener_p->index.cipIpAddress)) {
	    break;
        }
        if (automore_quit())
            break;
        cip_display_udp_listeners(cip_udp_listener_p);
        count++; 
        local_port = ++cip_udp_listener_p->index.cipUdpLocalPort;
        if (!local_port) {
            local_ipaddr = ++cip_udp_listener_p->index.cipIpAddress;
            if (!local_ipaddr)
                break;
        }
        else {
            local_ipaddr = cip_udp_listener_p->index.cipIpAddress;
        }
    }
    return count;
} /* show_ext_channel_udp_listeners */

/* 
 * show_ext_channel_udp
 */
static int show_ext_channel_udp (hwidbtype *hwidb_p, ulong local_ipaddr, 
                                 int search_type) {
    cip_udp_t cip_udp, *cip_udp_p;
    int count = 0;
    
    while ((cip_udp_p = get_cip_udp(search_type, hwidb_p, local_ipaddr, 
                                  &cip_udp)) != NULL) {
        cip_display_udp(cip_udp_p);
        count++; 
        local_ipaddr = ++cip_udp_p->index.cipIpAddress;
	if ((search_type == SEARCH_MATCH) ||
            automore_quit() || !local_ipaddr) {
	    break;
        }
    }
    return count;

} /* show_ext_channel_udp */

/* 
 * show_ext_channel_icmp
 */
static int show_ext_channel_icmp (hwidbtype *hwidb_p, ulong local_ipaddr, 
                                  int search_type) {
    cip_icmp_t cip_icmp, *cip_icmp_p;
    int count = 0;
    
    while ((cip_icmp_p = get_cip_icmp(search_type, hwidb_p, local_ipaddr, 
                                      &cip_icmp)) != NULL) {
        cip_display_icmp(cip_icmp_p);
        count++; 
        local_ipaddr = ++cip_icmp_p->index.cipIpAddress;
	if ((search_type == SEARCH_MATCH) ||
            automore_quit() || !local_ipaddr) {
	    break;
        }
    }
    return count;
} /* show_ext_channel_icmp */

/* 
 * show_extended_channel_tcpip_command
 */
void show_extended_channel_tcpip_command (parseinfo *csb)
{
    idbtype *swidb_p;
    hwidbtype *hwidb_p;
    cip_idb_infotype *cip;
    int search_type, stat_type;
    ulong local_ipaddr;
    char *type = NULL;
    int count = -1;
    
    swidb_p = GETOBJ(idb,1);
    hwidb_p = swidb_p->hwptr;

    cip = channel_sb_get_cip_info(hwidb_p);
    if (!cip) {
	printf("\n  No subchannels configured");
	return;
    }

    search_type  = SEARCH_NEXT;
    stat_type    = GETOBJ(int,1);
    local_ipaddr = 0;

    if (stat_type == CHANNEL_SHOW_IPADDR_MATCH) {
	search_type = SEARCH_MATCH;
	local_ipaddr = GETOBJ(paddr,1)->ip_addr;
    } 

    /*
     * Make sure the IDB is up for querying from the CIP
     */
    if (hwidb_p->state == IDBS_ADMINDOWN) {
        printf("%s", if_shutdown);
        return;
    }
    
    automore_enable(NULL);
    
    switch (csb->which) {
        case CHANNEL_SHOW_IP:
            type = "IP";
            count = show_ext_channel_ip( hwidb_p, local_ipaddr, search_type);
            break;
        case CHANNEL_SHOW_TCP:
            type = "TCP/IP";
            count = show_ext_channel_tcp( hwidb_p, local_ipaddr, search_type);
            break;
        case CHANNEL_SHOW_ICMP:
            type = "ICMP";
            count = show_ext_channel_icmp( hwidb_p, local_ipaddr, search_type);
            break;
        case CHANNEL_SHOW_UDP:
            type = "UDP";
            count = show_ext_channel_udp( hwidb_p, local_ipaddr, search_type);
            break;
        case CHANNEL_SHOW_UDP_LISTENERS:
            type = "UDP Listeners";
            count = show_ext_channel_udp_listeners( hwidb_p, local_ipaddr, search_type);
            break;
        default:
            printf("\n\n...Invalid display type...\n");
            break;
    }

    if (!count)   
        printf("\n\n...No %s statistics found\n", type);    

    automore_disable();
} /* show_extended_channel_tcpip_command */


/* 
 * show_ext_channel_csna_command
 * show extended channel [slot/port] csna {admin | oper | stats} [path [dev]]
 *
 * OBJ(int,4) = <dev>
 * OBJ(int,3) = <path>
 *
 * OBJ(int,2) = CHANNEL_STATS_PATH    | 
 *              CHANNEL_STATS_DEVICE  | 
 *              CHANNEL_STATS_DEFAULT
 *
 * OBJ(int,1) = CSNA_SHO_EXT_TYPE_ADMN | 
 *              CSNA_SHO_EXT_TYPE_OPER | 
 *              CSNA_SHO_EXT_TYPE_STAT |
 *              CSNA_SHO_EXT_TYPE_DFLT 
 * OBJ(idb,1) = <interface>
 */
void show_ext_channel_csna_command (parseinfo *csb)
{
    idbtype          *swidb;
    hwidbtype        *hwidb;
    cip_idb_infotype *cip;
    cip_vctype       *vc;
    uint              show_type, stat_type;
    int               search_type;    
    int               path, device; 
    long              search_path, search_device;
    csna_dev_t        csna_dev;
    char              printbuf1[20], printbuf2[20];
    ulonglong         val;
    boolean           found = FALSE;
    boolean           aborted = FALSE;
    boolean           quit = FALSE;

    
    swidb = GETOBJ(idb,1);
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	printf("\n  No subchannels configured");
	return;
    }

    automore_enable(NULL);
    switch (csb->which) {
    case CHANNEL_CSNA_DEVICE:
	path = device = -1;
	stat_type = GETOBJ(int,2);
	search_type = SEARCH_NEXT;
	switch (stat_type) {
	default:
	case CHANNEL_STATS_DEFAULT:
	    break;
	case CHANNEL_STATS_PATH:
	    path = GETOBJ(int,3);
	    break;
	case CHANNEL_STATS_DEVICE:
	    path = GETOBJ(int,3);
	    device = GETOBJ(int,4);
	    break;
	}
	show_type = GETOBJ(int,1);
        search_path = (path != -1) ? path : 0;
        search_device = (device != -1) ? device : 0;
        if ((path != -1) && (device != -1))
            search_type = SEARCH_MATCH;
	switch (show_type) {
	default:
	case CSNA_SHO_EXT_TYPE_DFLT:
	case CSNA_SHO_EXT_TYPE_ADMN:
            while ((vc = cip_findNextPathDevice(cip, search_path, 
                                                search_device,
                                                search_type, 
                                                csna_config_vc_check)) != NULL) {
                if ((path != -1) && (path != vc->path))
                    break;
                if (!found) {
                    found = TRUE;
                    printf("\n     Path Dv  %12s %12s %12s",
                           CSNA_DEVICE_KEY_MAXPIU,
                           CSNA_DEVICE_KEY_TIME_D,
                           CSNA_DEVICE_KEY_LENGTH_D); 
		}
                printf("\n%4s %04X %02X  %5d        %5d        %5d",
                        cip_mode_to_str_inline(CHANNEL_CSNA),
                        vc->path, vc->device, vc->maxpiu, 
                        vc->blocking_time_delay, vc->blocking_length_delay);  
                if (search_type == SEARCH_MATCH)
                    break;
                search_path = vc->path;
                search_device = vc->device + 1;
                if (!search_device) {
                    search_path++;
                    if (!search_path)
                        break;
		}
                quit = automore_quit();
                if (quit)
                    break;
            }
            break;
	case CSNA_SHO_EXT_TYPE_OPER:
            /*
             * Make sure the IDB is up for querying from the CIP
             */
            if (hwidb->state == IDBS_ADMINDOWN) {
                aborted = TRUE;
                break;
            }
            while ((vc = cip_findNextPathDevice(cip, search_path, 
                                                search_device,
                                                search_type, 
                                                csna_config_vc_check)) != NULL) {
                if ((path != -1) && (path != vc->path)) {
                    break;
                }
            
                if (csna_dev_rpc(hwidb, vc, &csna_dev) == NULL) {
                    break;
                }
                if (!found) {
                    found = TRUE;
                    printf("\n     Path Dv Status       SlowDown  %12s %12s %12s",
                           CSNA_DEVICE_KEY_MAXPIU,
                           CSNA_DEVICE_KEY_TIME_D,
                           CSNA_DEVICE_KEY_LENGTH_D); 
		}
                printf("\n%4s %04X %02X %12s %8s  %5d        %5d        %5d",
                        cip_mode_to_str_inline(CHANNEL_CSNA),
                        vc->path, vc->device,
                        get_csna_status_text(&csna_dev),
                        get_csna_slowdown_text(&csna_dev),
                        csna_dev.cipCardCsnaOperMaxBlockLength,
                        csna_dev.cipCardCsnaOperBlockDelayTime,
                        csna_dev.cipCardCsnaOperBlockDelayLength);  
                if (search_type == SEARCH_MATCH)
                    break;
                search_path = vc->path;
                search_device = vc->device + 1;
                if (!search_device) {
                    search_path++;
                    if (!search_path)
                        break;
		}
                quit = automore_quit();
                if (quit)
                    break;
            }
	    break;
	case CSNA_SHO_EXT_TYPE_STAT:
            /*
             * Make sure the IDB is up for querying from the CIP
             */
            if (hwidb->state == IDBS_ADMINDOWN) {
                aborted = TRUE;
                break;
            }
            while ((vc = cip_findNextPathDevice(cip, search_path, 
                                                search_device,
                                                search_type, 
                                                csna_config_vc_check)) != NULL) {
                if ((path != -1) && (path != vc->path)) {
                    break;
                }
                if (csna_dev_rpc(hwidb, vc, &csna_dev) == NULL) {
                    break;
                }
                found = TRUE;
                printf("\n%8s %04X %02X",
                        cip_mode_to_str_inline(CHANNEL_CSNA),
                        vc->path, vc->device);
                printf("\n  Blocks Transmitted = %-10u  Received = %-10u",
                        csna_dev.cipCardCsnaStatsBlocksTxd,
                        csna_dev.cipCardCsnaStatsBlocksRxd);
                val = ((ulonglong)csna_dev.cipCardCsnaStatsBytesTxd_msw << 32) + 
                      csna_dev.cipCardCsnaStatsBytesTxd_lsw;
                print_long_count(&val, printbuf1);
                val = ((ulonglong)csna_dev.cipCardCsnaStatsBytesRxd_msw << 32) + 
                      csna_dev.cipCardCsnaStatsBytesRxd_lsw;
                print_long_count(&val, printbuf2);
                printf("\n  Bytes  Transmitted =  %9s  Received =  %9s",
                       printbuf1, printbuf2);
                printf("\n  Slow downs Sent = %-10u  Received = %-10u",
                        csna_dev.cipCardCsnaStatsSlowDownsSent,
                        csna_dev.cipCardCsnaStatsSlowDownsReceived);
                val = ((ulonglong)csna_dev.cipCardCsnaStatsBytesTxByMaxBlockLength_msw << 32) +
                       csna_dev.cipCardCsnaStatsBytesTxByMaxBlockLength_lsw;
                print_long_count(&val, printbuf1);
                printf("\n  Txd by %12s: Blocks = %-10u   Bytes = %9s",
                        CSNA_DEVICE_KEY_MAXPIU,
                        csna_dev.cipCardCsnaStatsBlocksTxByMaxBlockLength,
                        printbuf1);
                val = ((ulonglong)csna_dev.cipCardCsnaStatsBytesTxByBlockDelayTime_msw << 32) +
                       csna_dev.cipCardCsnaStatsBytesTxByBlockDelayTime_lsw;
                print_long_count(&val, printbuf1);
                printf("\n  Txd by %12s: Blocks = %-10u   Bytes = %9s",
                        CSNA_DEVICE_KEY_TIME_D,
                        csna_dev.cipCardCsnaStatsBlocksTxByBlockDelayTime,
                        printbuf1);
                val = ((ulonglong)csna_dev.cipCardCsnaStatsBytesTxByBlockDelayLength_msw << 32) +
                       csna_dev.cipCardCsnaStatsBytesTxByBlockDelayLength_lsw;
                print_long_count(&val, printbuf1);
                printf("\n  Txd by %12s: Blocks = %-10u   Bytes = %9s",
                        CSNA_DEVICE_KEY_LENGTH_D,
                        csna_dev.cipCardCsnaStatsBlocksTxByBlockDelayLength,
                        printbuf1);
                if (search_type == SEARCH_MATCH)
                    break;
                search_path = vc->path;
                search_device = vc->device + 1;
                if (!search_device) {
                    search_path++;
                    if (!search_path)
                        break;
		}
                quit = automore_quit();
                if (quit)
                    break;
            }
	    break;
	}    
	break;
    default:
	break;
    }
    
    if (!quit) {
        if (aborted)
            printf("%s", if_shutdown);
        else if (!found)
            printf("\n\n...No CSNA instances found for this interface...\n");
    }
    automore_disable();
}



/*
 * link2channel
 * Given a link type, return the channel type code
 */
static long link2channel (long link)
{
    ulong value;

    switch (link) {
	case LINK_IP:            return(TYPE_IP10MB);
	default:
	  value = reg_invoke_media_link_to_type(link);
	  return(value ? value : 0L);
    }
}


/*
 * channel_getlink
 * Return generic link code for a channel datagram
 */
static long channel_getlink (paktype *pak)
{
    channel_hdr *channel;

    channel = (channel_hdr *)pak->datagramstart;
    return(type2link(GETSHORT(&channel->type)));
}

/*
 * channel_pkt_dump
 *
 * Print out a packet inbound or outbound for a channel interface
 */
static void channel_pkt_dump (paktype* pak)
{
    int size;
    channel_hdr *hdr;
    iphdrtype *ip;
    
    size = pak->datagramsize;
    hdr = (channel_hdr *)pak->datagramstart;
    buginf("size = %d, vc = %04X, type = %04X", size, hdr->vc, hdr->type);
    switch (hdr->type) {
    case TYPE_IP10MB:
	ip = (iphdrtype *)&pak->datagramstart[CHANNEL_ENCAPBYTES];
	buginf(", src %i, dst %i", ip->srcadr, ip->dstadr);
	break;
    default:
    }
}

/*
 * channel_et_srb
 * scribble header for virtual lan emulation on channel
 */
static boolean channel_et_srb (paktype *pak, int linktype,
			       ushort flags, int size, ushort vc)
{
    ushort *sp;
    hwidbtype *hwidb = pak->if_output->hwptr;
    channel_sb_t *chan_sb = channel_get_sb(hwidb);

    if (!chan_sb) {
        if (channel_debug_ilan) {
	    buginf("\n%s: channel subblock is undefined for this interface", 
                     hwidb->hw_namestring);
        }
        return(FALSE);
    }

    pak->datagramsize += CHANNEL_TOKEN_ENCAPBYTES;
    pak->datagramstart -= CHANNEL_TOKEN_ENCAPBYTES;
    sp = (ushort *)((uchar *)pak->datagramstart);
    PUTSHORT(sp, vc);
    sp++;
    PUTSHORT(sp, 0x2001);
    sp++;

    if (chan_sb->emulated_media == CIP_VLAN_TOKEN) {
	PUTSHORT(sp, 0x0001);	/* token ring */
        sp++;
        PUTSHORT(sp, (AC_PRI4<<8) | FC_TYPE_LLC);
    } else if (chan_sb->emulated_media == CIP_VLAN_FDDI) {
	PUTSHORT(sp, 0x0002);	/* fddi */
        sp++;
	PUTSHORT(sp, FDDI_LLC_FC_BYTE); /* add lc byte */
    } else {
        if (channel_debug_ilan)
	    buginf("\n%s - vc(%d): channel SRB encapsulation media type invalid(%d)", 
                   hwidb->hw_namestring, vc, chan_sb->emulated_media);
        return(FALSE);
    }
    return(TRUE);
}


/*
 * channel_encapsulate
 * worker routine for channel_vencap and channel_broadcast
 */
static boolean channel_encapsulate (paktype *pak, short vc, int enctype)
{
    channel_hdr *channelhdr;
    hwidbtype *hwidb;
    ushort flags = 0;

    switch (enctype) {
      case ET_CHANNEL:
	pak->datagramsize += CHANNEL_ENCAPBYTES;
	pak->datagramstart -= CHANNEL_ENCAPBYTES;
	if (pak->datagramstart < pak->data_area)
	    return(FALSE);
	channelhdr = (channel_hdr *)pak->datagramstart;
	channelhdr->vc = vc;
	PUTSHORT(&channelhdr->type, link2channel(pak->linktype));

	pak->mac_start  = pak->datagramstart;
	pak->addr_start = NULL;
	pak->rif_start  = NULL;
	pak->riflen 	= 0;
	pak->info_start = NULL;
	pak->enctype    = ET_CHANNEL;
	pak->encsize   += CHANNEL_ENCAPBYTES;

	if (channel_pkt_debug) {
	    hwidb = pak->if_output->hwptr;
	    buginf("\nCE:(%s)-out ", hwidb->hw_namestring);
	    channel_pkt_dump(pak);
	}
	break;
      default:
	if (!reg_invoke_media_channel_macwrite(enctype, pak, pak->linktype,
					       flags, pak->datagramsize, vc)) {
	    errmsg(&msgsym(BADENCAP, LINK), pak->if_output->namestring,
		   enctype);
	    return(FALSE);
	}
	break;
    }
    return(TRUE);
}

/*
 * channel_broadcast
 *
 * Support IP broadcast over the channel by flooding packet
 * down each of the vc's.
 *
 * Suppress broadcast on any vc that does not
 *   - have the CIP_VC_BRDCST flag on (not configured for broadcast)
 *   - have the CIP_VC_SROUTE flag on (not currently in the static route table)
 *
 * An INUSE vc that doesn't have the CIP_VC_SROUTE flag on may indicate
 *   - the CIP has not yet acknowledged the configuration for that device, or,
 *   - the CLAW link for that device has gone down.  In either case, bypass it.
 */

#define CIP_BROADCAST (CIP_VC_INUSE|CIP_VC_BRDCST|CIP_VC_SROUTE)

static boolean channel_broadcast (idbtype *swidb, paktype *pak)
{
    cip_idb_infotype *cip;
    cip_vctype *vc, *firstvc;
    hwidbtype *hwidb;
    paktype *newpak;
#if 0
    if (channel_pkt_debug)
	buginf("\n%s: flooding broadcast to all hosts", swidb->namestring);
#endif
    cip = channel_sb_get_cip_info(swidb->hwptr);
    if (!cip) {
	return (FALSE);
    }

    hwidb = pak->if_output->hwptr;    
    firstvc = NULL;

    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if (!((vc->flags & CIP_BROADCAST) == CIP_BROADCAST))
	    continue;

	if (firstvc == NULL) {
	    firstvc = vc;
	    continue;
	}
	newpak = pak_duplicate(pak);
	if (!newpak)
	    break;
	if (!channel_encapsulate(newpak, vc->vc_num, ET_CHANNEL)) {
	    datagram_done(newpak);
	    return (FALSE);
	}
	datagram_out(newpak);
    }
    if (firstvc) 
	return (channel_encapsulate(pak, firstvc->vc_num, ET_CHANNEL));
    return (FALSE);
}


/*
 * channel_vencap
 * 
 * Encapsulate a datagram.  Makes no assumptions about data alignment.
 * datagramstart and datagramsize must be properly set before calling.
 */
boolean channel_vencap (paktype *pak, long address)
{
    ushort flags;
    int enctype;
    boolean broadcast;
    staticmap_type *map;
    ushort vc;

    if (!pak->datagramstart)
	return(FALSE);		/* bad caller */

    pak->enctype = ET_CHANNEL;   
    enctype = pak->enctype;

    /*
     * Do broadcast_determination.  We want an encapsulation method, and
     * the appropriate hdlc header flags.  The encapsulation method is
     * only used as an index to the mac rewrite routine, or to indicate
     * that the packet has already been completely processed.
     */
    flags = 0; 

    if (!reg_invoke_media_channel_macaddr(pak->linktype, pak, address,
				       &enctype, &broadcast, &map)) {
	if (!reg_used_media_channel_macaddr(pak->linktype))
	    errmsg(&msgsym(BADMACREG, LINK), pak->if_output->namestring,
		   pak->linktype);
	return(FALSE);
    }

    if (enctype == ET_NULL)
	return(TRUE);

    if (broadcast) {
	return(channel_broadcast(pak->if_output, pak));
    }

    if (pak->linktype == LINK_SRB) {
        channel_sb_t *chan_sb = channel_get_sb(pak->if_output->hwptr);
        if (chan_sb) {
	    vc = chan_sb->get_vc_from_macaddr(pak->if_output->hwptr,
                                              pak->datagramstart+TR_DA_OFFSET,
                                              pak->datagramstart+TR_SA_OFFSET);
            if (vc == NO_CHANNEL_VC) {
	        return(FALSE);
	    }
        } else {
            return(FALSE);
        }
	enctype = ET_UNUSED1;	/*  */
	
    } else {
	bcopy(map->hwconf.addr, &vc, STATIONLEN_ATMVC); 
    }
    if (!channel_encapsulate(pak, vc, enctype))
	return(FALSE);

    /*
     * Make sure routing cache is populated by transit traffic.
     */
    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);

    return(TRUE);
}



/*
 * channel_input
 * Generic input routine for channel, mostly for debugging
 */
static void channel_input (hwidbtype *hwidb, paktype *pak)
{
    if (channel_pkt_debug) {
	buginf("\n(%s)-in  ", hwidb->hw_namestring);
	channel_pkt_dump(pak);
    }
    raw_enqueue(hwidb, pak);
}


/*
 * channel_parse_packet
 *
 * Set up a packet buffer so that the network level protocol fields
 * are correctly initialized
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 *
 * WARNING: It it up to the calling routine to insure that the
 * (potential) snap header is in a location that it *byte* accessible.
 * For MCI/Cbus devices, this means reading this information off of the
 * controller and into local memory.  THe same rules apply for all other
 * drivers.
 */
static iqueue_t channel_parse_packet (
    hwidbtype *hwidb,
    paktype *pak,
    uchar *data)
{
    ushort type_or_len;
    int info_size;
    charlong firstlong;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	firstlong.d.lword = GETLONG(data);
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	firstlong.d.lword = GETLONG(pak->datagramstart);
    }

    type_or_len = firstlong.d.sword[1];

    pak->enctype = ET_CHANNEL;

    if (type_or_len == CHANNEL_ENCAPS_VLAN)
	info_size = CHANNEL_ENCAPBYTES;
    else 
	info_size = 0;
    /*
     * If we're building a virgin frame, we have to set datagramstart
     */
    if (data) {
	pak->datagramstart = pak_center(pak) -
	    CHANNEL_ENCAPBYTES - info_size;
    }
    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = pak->mac_start;
    pak->rif_start     = NULL;
    pak->riflen        = 0;
    pak->info_start    = NULL;
    pak->network_start = pak->mac_start + CHANNEL_ENCAPBYTES + info_size;
    
    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;
    return(raw_enqueue);
}

/*
 * ---------------------------------------------------------------------
 * cip_reset - reset a CIP interface
 *
 *    This is called under a number of circumstances, including:
 *      an EOIR event of another card
 *      an interface state change
 *         - [no] shut command
 *         - clear interface command
 *         - mic reload
 *      an interrupt-level request to reset the interface,
 *         due to some resource constraint, eg. buffer overuse
 *
 *   o If we are here on an EOIR event, the interface queues and
 *     buffers are reinitialized, and the CIP is sent the UNQUIESCE
 *     command.
 *   
 *   o Otherwise, we are here to DISABLE the interface, reinitialize
 *     the interface queues and buffers, and then re-ENABLE it if it is
 *     not administratively shut.  
 *
 *  After the CIP is sent a DISABLE/ENABLE sequence, the entire
 *  configuration must be resent.  Prior to resending the configuration,
 *  the current configuration must be reset.  This includes deleting 
 *  static route entries and resetting configuration PNDING states.
 *  Some of the work which must be done to reset the configuration
 *  cannot be done at interrupt level.
 *    
 *  In such a case, the interface state is changed to IDBS_DOWN and a 
 *  process level event is enqueued to reset the configuration and enable 
 *  the interface. The interface state will be brought back to IDBS_UP
 *  after it is enabled and the CIP sends the signal indicator.
 *
 *  Bringing the state to IDBS_DOWN will prevent usage of these still
 *  present static routes.  The deferred_enable() routine (which runs at 
 *  process level) will delete the static routes before enabling the CIP.
 *
 * ---------------------------------------------------------------------
 */
void cip_reset (hwidbtype *hwidb)
{
    ushort sresult;
    leveltype level;
    boolean oninterrupt;
    cip_idb_infotype *cip;

    if (channel_debug)
	buginf("\n%s: cip_reset(), state %s", hwidb->hw_namestring,
	       print_hwidbstate(hwidb));

    if (hwidb->status & IDB_NOHARDWARE) {
	errmsg(&msgsym(RESETNXI, CBUS), hwidb, hwidb->hw_namestring);
	return;
    }

    hwidb->counters.resets++;
    hwidb->status &= ~IDB_DISABLED;

    cip_sysmgt_info_reset(hwidb);

    if (hwidb->oir_flags & EOIR_RESET) {
	cip_eoir_reset(hwidb);
	return;
    }
    /* Are we here on an interrupt ? */
    oninterrupt = (onintstack() != 0);
    
    /* Disable interrupts and select the interface */
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    reg_invoke_bump_cache_version(hwidb, NULL, TRUE);

    /*
     * Send the DISABLE comand
     */
    sresult = cip_disable(hwidb);
    if (sresult) {
	reset_interrupt_level(level);
	return;
    }
    /*
     * Flush hold queue: Does this get done for EOIR ?
     */
    holdq_reset(hwidb);
    /*
     * Also reset all idb on the cip virtual interface.
     */
    if (is_cip_virtual(hwidb)) {
        hwidbtype *ilan_hwidb = hwidb->next_idb_on_physint;
        while (ilan_hwidb) {
            if (is_cip_lan(ilan_hwidb)) {
                (*ilan_hwidb->reset)(ilan_hwidb);
            }
            ilan_hwidb = ilan_hwidb->next_idb_on_physint;
        }
    }

    /*
     * Set mac address, if not already set
     */
    if (is_ieee_zero(hwidb->bia)) {
	reg_invoke_assign_mac_addr(hwidb);
	ieee_copy(hwidb->bia, hwidb->hardware);
    }

    /*
     * Readjust Queues and Buffers
     */
    cip_reset_queues_and_buffs (hwidb);
    
    cip = channel_sb_get_cip_info(hwidb);
    if (cip) {
	/* Increment the interface correlator */    
	cip->reset_correlator++;
	
	/* Purge elements from the Process Level Queue */
	cip_purge_queued_events(hwidb, cip, oninterrupt);
	
	cip_init_load_metric_values(cip);
    }
    /*
     * Don't enable the CIP interface if:
     *    - the interface is administratively shutdown
     *    - the bootldr (rather than the system) image is running
     */
    if ((hwidb->state == IDBS_ADMINDOWN) ||
	(system_loading)) {
        reset_interrupt_level(level);
        return;
    }

    /*
     * Defer the ENABLE for this CIP interace if it has
     * already been configured.
     */
    if (cip) {
        /*
         * Report the interface as down, and enqueue an event to enable it.
         */
	net_cstate(hwidb, IDBS_DOWN);
	cip_defer_enable(hwidb, cip);
    } else {
        /*
         * Send the ENABLE comand to unconfigured CIP interfaces
         */
        cip_enable(hwidb);
    }
    
    reset_interrupt_level(level);
    return;
}

/*
 * ---------------------------------------------------------------------
 * cip_eoir_reset
 *   A minimal amount of work is done on a reset issued at an EOIR event
 *   Buffers and queues are readjusted
 *   The interface processor is given a command to resume 
 * ---------------------------------------------------------------------
 */
static void cip_eoir_reset (hwidbtype *hwidb)
{
    leveltype level;
    
    if (channel_debug)
	buginf("\n%s: cip_eoir_reset",
	       hwidb->hw_namestring);
    
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);

    cip_reset_queues_and_buffs (hwidb);
    send_status_cmd(hwidb, FCI_STATUS_UNQUIESCE);

    reset_interrupt_level(level);
    return;
}
/* 
 * ---------------------------------------------------------------------
 * cip_reset_queues_and_buffs
 *   This is called by cip_reset to readjust
 *   buffer and queue limits
 *
 * Assumptions:
 *    interrupts are disabled
 *    the interface has been selected
 * ---------------------------------------------------------------------
 */
static void cip_reset_queues_and_buffs (hwidbtype *hwidb)
{

    /*
     * Call a platform specific routine to pass down platform
     * specific information for this interface (for example,
     * buffer pools on the c7000/ags and hardware queues on 
     * the RSP.
     */

    cip_restart_init(hwidb);

     /* Set tx output queue limit */
    cbus_mci_txlimit(hwidb);
}

/* 
 * ---------------------------------------------------------------------
 * cip_disable
 *   Issue a DISABLE command to the CIP
 *
 * Assumptions:
 *    interrupts are disabled
 *    the interface has been selected
 * ---------------------------------------------------------------------
 */
static boolean cip_disable (hwidbtype *hwidb)
{
    ushort sresult;
    
    if (channel_debug)
	buginf("\n%s: cip_reset(), %s", hwidb->hw_namestring,
	       "sending DISABLE");
    
    sresult = send_if_disable_cmd_delay(hwidb, IF_DISABLE_TIME);
    if (sresult) {
	errmsg(&msgsym(CIPRSET, CBUS), hwidb->hw_namestring, sresult, 
	       "disable");
    }
    return (sresult);
}
/* 
 * ---------------------------------------------------------------------
 * cip_enable
 *   Issue an ENABLE command to the CIP
 *
 * Assumptions:
 *    interrupts are disabled
 *    the interface has been selected
 * ---------------------------------------------------------------------
 */
static boolean cip_enable (hwidbtype *hwidb)
{
    ushort sresult;

    if (channel_debug)
	buginf("\n%s: cip_reset(), %s", hwidb->hw_namestring,
	       "sending ENABLE");
    
    sresult = send_if_enable_cmd(hwidb);
    if (sresult) {
	errmsg(&msgsym(CIPRSET, CBUS), hwidb->hw_namestring, sresult, 
	       "enable");
    }
    return (sresult);
}

/*
 * ---------------------------------------------------------------------
 * cip_reset_cfg
 *    Reset the current configuration states
 *
 *    This is called at process level.  
 *      - at cip_reset, after issuing a DISABLE to the CIP
 *      - in the event that cip_reset is called at interrupt
 *        level, this is called at process level
 *        (at deferred enable), prior to issuing the ENABLE
 *
 *    o Delete any static routes associated with CIP configuration
 *      commands. 
 *
 *    o If the configuration is still PNDING, either because it
 *      got purged rather than processed, or because the CIP did
 *      not acknowledge it, then process it here and take it out of
 *      PNDING state
 *
 *   When the CIP is DISABLED, all the static routes must be 
 *   deleted from the static route table.  Enabling the CIP 
 *   can set up race condition where traffic is forwarded before 
 *   the configuration command has been fully processed.
 *
 * ---------------------------------------------------------------------
 */
static void cip_reset_cfg (
    hwidbtype        *hwidb,
    cip_idb_infotype *cip)
{
    cip_vctype *vc;
    idbtype *swidb;

    if (!cip)
	return;

    if (channel_debug)
	buginf("\n%s: reset_cfg",
	       hwidb->hw_namestring);
    
    swidb = hwidb->firstsw;
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if (vc->flags & CIP_VC_INUSE) {
	    
	    /* Clear any asynchronous state change pending conditions */
	    vc->state_flags = 0;
	    
	    /* If a static route is defined on this vc, delete it */
	    cip_delete_static_route(swidb, vc);
	    
	    if (vc->ack_flags & CIP_CFG_PNDING) {
		if (vc->ack_flags & CIP_CFG_DELETE) {
		    if (channel_debug)
			buginf("\ntearing down PNDING vc: %s %04X %02X",
			       cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
			       vc->path, vc->device);
		    cip_teardown_vc(swidb, cip, vc);
		} else {
		    if (channel_debug)
			buginf("\nResetting ack flags for PNDING vc: %s %04X %02X",
			       cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
			       vc->path, vc->device);
		    vc->ack_flags = 0;
		}
	    }
	}
    }
    /* If this is the Virtual interface, call its reset routine */
    if (cip_interface_is_virtual(hwidb)) {
	cip_vlan_reset_cfg(hwidb, cip);
	cip_tn3270s_reset_cfg(hwidb, cip);
    }
}

/* 
 * ---------------------------------------------------------------------
 * cip_send_configuration
 *   The CIP has just been ENABLED.
 *    - Reinitialize the love statistics,
 *    - Readjust the size of the preQ
 *    - Send the full configuration
 * ---------------------------------------------------------------------
 */
static void cip_send_configuration (
    hwidbtype        *hwidb,
    cip_idb_infotype *cip)
{	
    GET_TIMESTAMP(cip->timeOfReset);
    cip->cfg_block = 0;  /* new ucode may use a different address */
    cip_set_love_letter_timestamp(cip, 0);
    TIMER_STOP(cip->last_love_letter);
    cip_clear_si_stats(cip);
    
    /* Adjust the size of the preQ */
    cip_plevel_adjust_preQ_size(hwidb, cip);
    
    /* send nodeid, the stats interval and all config commands */
    cip_send_cfg(hwidb);
}

/* 
 * ---------------------------------------------------------------------
 * cip_defer_enable
 *   cip_reset has been called at interrupt level.
 *   - enqueue a process level event to send the ENABLE and the 
 *     configuration 
 *
 * Format of buf:
 *                       PLEVENT_ENABLE
 * Offset field  length  Description
 *   0 correlator[2]     correlator 
 * ---------------------------------------------------------------------
 */
#define OFFSET_ENABLE_CORR 0  /* Correlator in a ENABLE requeust */

static void cip_defer_enable (
    hwidbtype        *hwidb,
    cip_idb_infotype *cip)
{
    plevel_t *plevel_element;
    
    if (!cip)
	return;
    
    /* We are called from interrupt level - enqueue a PLEVENT event */
    if (channel_debug)
	buginf("\n%s: reset from interrupt level -- defer enable",
	       hwidb->hw_namestring);
    
    plevel_element = cip_plevel_element_get(cip, PLEVEL_PREQ);
    if (!plevel_element) {
	errmsg(&msgsym(CIPENABLE, CBUS), hwidb->hw_namestring);
	return;
    }
      
    plevel_element->pl_event = PLEVENT_ENABLE;
    plevel_element->pl_size  = sizeof(ushort);
    PUTSHORT(&plevel_element->pl_data[OFFSET_ENABLE_CORR], cip->reset_correlator);

    cip_plevel_element_put(cip, plevel_element, PLEVEL_RDYQ);
}

/* 
 * ---------------------------------------------------------------------
 * cip_deferred_enable
 *   We are here at process level.
 *    - The tasks that have been deferred because the reset request
 *      came at interrupt level can be done now:
 *      - reset the configuration
 *      - Send the ENABLE to the CIP.
 *      - Send entire configuration set
 * ---------------------------------------------------------------------
 */
static void cip_deferred_enable (
    hwidbtype        *hwidb,
    cip_idb_infotype *cip,
    uchar            *buf,
    ushort            length)
{	
    leveltype level;
    ushort    sresult;
    ushort    correlator;

    correlator = GETSHORT(&buf[OFFSET_ENABLE_CORR]);
    
    if (channel_debug)
	buginf("\n%s:(%04X,%04X) Processing deferred enable",
	       hwidb->hw_namestring, cip->reset_correlator, correlator);
    
    if (correlator != cip->reset_correlator) {
	if (channel_debug)
	    buginf("\n%s:(%04X,%04X) Deferred enable - bad correlator",
		   hwidb->hw_namestring, cip->reset_correlator, correlator);
	return;	
    }
    
    /* Reset the current configuration states */
    cip_reset_cfg(hwidb, cip);
    
    level = raise_interrupt_level(NETS_DISABLE);
    select_if(hwidb);
    
    sresult = cip_enable(hwidb);
    if (sresult) {
	reset_interrupt_level(level);
	return;
    }
    cip_send_configuration(hwidb, cip);
    
    reset_interrupt_level(level);
    return;
}

/*
 * ---------------------------------------------------------------------
 * cip_plevel_init
 *   Create and initialize the queues used for process level event support
 *
 * ---------------------------------------------------------------------
 */
static void cip_plevel_init (
    hwidbtype        *hwidb,
    cip_idb_infotype *cip)
{

    queue_init(&cip->plevel_preQ, 0);
    queue_init(&cip->plevel_rdyQ, 0);

    cip->reset_correlator = 0;
    cip->plevel_preQ_min  = CIP_PLEVEL_PREQ_MIN;

    /* Populate the preQ */
    cip_plevel_adjust_preQ_size(hwidb, cip);

    cip->cip_encaps = cip_encaps_deed;
}
/*
 * ---------------------------------------------------------------------
 * cip_plevel_adjust_preQ_size
 *   Add or remove elements from the preQ
 *
 * ---------------------------------------------------------------------
 */
static void cip_plevel_adjust_preQ_size (
    hwidbtype *hwidb,
    cip_idb_infotype *cip)
{
    ushort count, i;
    leveltype level;
    
    level = raise_interrupt_level(NETS_DISABLE);
    count = cip->plevel_preQ.count;

    if (channel_debug)
	buginf("\n%s: adjusting preQ, current size = %d",
	       hwidb->hw_namestring,	       
	       count);
    
    if (count < cip->plevel_preQ_min) {
	for (i = count; i < cip->plevel_preQ_min; i++)
	    cip_plevel_chunk_to_preQ(cip);
	reset_interrupt_level(level);
	return;
	
    } 	
    if (count > cip->plevel_preQ_min) {
	for (i = count; i > cip->plevel_preQ_min; i--)
	    cip_plevel_preQ_to_chunk(cip);
	reset_interrupt_level(level);
	return;
	
    }
    reset_interrupt_level(level);
    return;
}

/*
 * ---------------------------------------------------------------------
 * cip_plevel_element_get
 *    Get an element from the chunk, preQ or rdyQ
 *
 * ---------------------------------------------------------------------
 */
plevel_t *cip_plevel_element_get (
   cip_idb_infotype *cip,
   plevent_loc_t location)
{
    plevel_t *plevel_element;
    
    switch (location) {
    case PLEVEL_CHNK:   /* the chunk */
	plevel_element = chunk_malloc(cip_plevel_chnk);
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf("\nCHNK--> %08X",
		   plevel_element);
	break;
    case PLEVEL_PREQ:   /* the preQ */    
	plevel_element = p_dequeue(&cip->plevel_preQ);
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf("\n                preQ--> %08X",
		   plevel_element);
	break;
    case PLEVEL_RDYQ:   /* the rdyQ */
	plevel_element = p_dequeue(&cip->plevel_rdyQ);
	if (CHANNEL_DEBUG_DEED && channel_debug && plevel_element)
	    buginf("\n                                      rdyQ--> %08X(%-3d)",
		   plevel_element, plevel_element->pl_size);
	break;
    default:
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf("\n????--> NULL");
	plevel_element = NULL;
	break;
    }
    return plevel_element;
}

/*
 * ---------------------------------------------------------------------
 * cip_plevel_element_put
 *    Put an element in the chunk or on the preQ or rdyQ
 *
 * Returns FALSE if the incoming element is NULL
 * ---------------------------------------------------------------------
 */
static boolean cip_plevel_element_put (
   cip_idb_infotype *cip,
   plevel_t *plevel_element,
   plevent_loc_t location)
{
    
    if (plevel_element == NULL)
	return FALSE;
    
    switch (location) {
    case PLEVEL_CHNK:   /* the chunk */
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf("\nCHNK<-- %08X",
		   plevel_element);
	chunk_free(cip_plevel_chnk, plevel_element);
	break;
    case PLEVEL_PREQ:   /* the preQ */    
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf("\n        %08X -->preQ",
		   plevel_element);
	p_enqueue(&cip->plevel_preQ, plevel_element);
	break;
    case PLEVEL_RDYQ:   /* the rdyQ */
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf("\n                        %08X(%-3d) -->rdyQ",
		   plevel_element, plevel_element->pl_size);
	p_enqueue(&cip->plevel_rdyQ, plevel_element);
	break;
    default:
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf("\n????<-- %08X",
		   plevel_element);
	break;
    }
    return TRUE;
}

/*
 * ---------------------------------------------------------------------
 * cip_plevel_chunk_to_preQ
 *    Get an element from the chunk and add it to the preQ
 *
 * ---------------------------------------------------------------------
 */
static boolean cip_plevel_chunk_to_preQ (
    cip_idb_infotype *cip)
{
    plevel_t *plevel_element;

    plevel_element = cip_plevel_element_get(cip, PLEVEL_CHNK);
    if (!plevel_element)
	return FALSE;

    cip_plevel_element_put(cip, plevel_element, PLEVEL_PREQ);
    return TRUE;
}
/*
 * ---------------------------------------------------------------------
 * cip_plevel_preQ_to_chunk
 *    Return an element from the preQ to the chunk
 *
 * ---------------------------------------------------------------------
 */
static void cip_plevel_preQ_to_chunk (
    cip_idb_infotype *cip)
{
    plevel_t *plevel_element;

    plevel_element = cip_plevel_element_get(cip, PLEVEL_PREQ);
    if (!plevel_element)
	return;

    cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_CHUNK);
}

/*
 * ---------------------------------------------------------------------
 * cip_plevel_element_dispose
 *     Return a plevel_element to either the preQ or to the chunk
 *
 * ---------------------------------------------------------------------
 */
static void cip_plevel_element_dispose (
    cip_idb_infotype *cip,
    plevel_t *plevel_element,
    dispose_t destination)
{
    /* Determine if this is to be returned to the preQ or to the chunk */
    switch (destination) {
    case DISPOSE_TO_PREQ:
	cip_plevel_element_put(cip, plevel_element, PLEVEL_PREQ);
	break;
    case DISPOSE_TO_CHUNK:
	cip_plevel_element_put(cip, plevel_element, PLEVEL_CHNK);
	break;
    }
}

/*
 * ---------------------------------------------------------------------
 * cip_purge_queued_events
 *     An interface reset has occured.
 *     Remove all outstanding events from the process_level work queue
 *
 *     If here on interrupt level, all elements go back to the preQ 
 *      - cannot chunk_free at interrupt level
 *      - must guarantee availability of preQ elements during the 
 *        deferred enable period.
 *      - a pkt in transit from the CIP prior to the reset may arrive
 *        in the deferred enable interim. (In such a case, the element 
 *        will be tossed because of correlator mismatch).
 *      - The preQ size will be readjusted when the deferred enable occurs.
 *     Otherwise, they go back to the chunk
 * ---------------------------------------------------------------------
 */
static void cip_purge_queued_events (
    hwidbtype *hwidb,
    cip_idb_infotype *cip,
    boolean oninterrupt)
{
    plevel_t *plevel_element;
    dispose_t destination;
    
    
    /* Determine if this is to be returned to the preQ or to the chunk */
    if (oninterrupt) {
	destination = DISPOSE_TO_PREQ;
    } else {
	destination = DISPOSE_TO_CHUNK;
    }
    
    while ((plevel_element = cip_plevel_element_get(cip, PLEVEL_RDYQ))
	   != NULL) {
	if (CHANNEL_DEBUG_DEED && channel_debug)
	    buginf(" PURGED");
	cip_plevel_element_dispose(cip, plevel_element, destination);
    }
}

/*
 * ---------------------------------------------------------------------
 * cip_plevel_enqueue
 *     Enqueue the received element onto the rdyQ
 * This is called at interrupt level
 *
 * These are either:
 *  CIP configuration acknowledgments (PLEVENT_CFGACK)
 *  CIP asynchronous state change requests (PLEVENT_ASYREQ)
 *
 * Format of buf:        PLEVENT_ASYREQ
 * Offset field  length  Description
 *   0 code      [4]     CIP_CFG_SR_ADD 2
 *                       CIP_CFG_SR_DEL 3
 *   4 correlator[2]     correlator 
 *   6 vc        [2]     VC
 * ---------------------------------------------------------------------
 */
#define OFFSET_CODE       0   /* Asynchronous request code */
#define OFFSET_CFG_CODE   4   /* CIP_CFG_XXX config command code */
#define OFFSET_ASYNC_CORR 4   /* Correlator */
#define OFFSET_ASYNC_VC   6   /* VC */

void cip_plevel_enqueue (
    hwidbtype *hwidb,
    cip_idb_infotype *cip,
    plevel_t *plevel_element,
    ushort bytes)
{
    ulong       code;
    uchar      *buf;
    ushort      vcnum;
    cip_vctype *vc;
    ushort      correlator;
 
    plevel_element->pl_size  = bytes;
    buf = plevel_element->pl_data;
    code = GETLONG(&buf[OFFSET_CODE]);

    switch (code) {
    case CIP_CFG_NACK:
    case CIP_CFG_ACK:
        correlator = GETSHORT(&buf[bytes-sizeof(correlator)]);
	if (correlator != cip->reset_correlator) {
	    if (channel_debug)
		buginf("\n%s:(%04X,%04X) Bad correlator on %s",
		       hwidb->hw_namestring, cip->reset_correlator, correlator,
		       (code == CIP_CFG_ACK) ? "CIP_CFG_ACK" : "CIP_CFG_NACK");
	    cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_PREQ);
	    return;
	}
	plevel_element->pl_event = PLEVENT_CFGACK;
	break;
    case CIP_CFG_SR_DEL:
    case CIP_CFG_SR_ADD:
        correlator = GETSHORT(&buf[OFFSET_ASYNC_CORR]);
	if (correlator != cip->reset_correlator) {
	    if (channel_debug)
		buginf("\n%s:(%04X,%04X) Bad correlator on route %s request",
		       hwidb->hw_namestring, cip->reset_correlator, correlator,
		       (code == CIP_CFG_SR_ADD) ? "UP" : "DOWN");
	    cip_plevel_element_dispose(cip, plevel_element,
				       DISPOSE_TO_PREQ);
	    return;	
	}
        vcnum = GETSHORT(&buf[OFFSET_ASYNC_VC]);
	vc = cip_find_vc_with_vcnum(cip, vcnum);
	if (!vc) {
	    if (channel_debug)	    
		buginf("\n%s:(%04X,%04X) Bad vc (%d) on route %s request",
		       hwidb->hw_namestring, cip->reset_correlator, correlator,
		       vcnum,
		       (code == CIP_CFG_SR_ADD) ? "UP" : "DOWN");
	    cip_plevel_element_dispose(cip, plevel_element,
				       DISPOSE_TO_PREQ);
	    return;	
	}
	if (channel_debug)
	    buginf("\n%s:(%04X,%04X) Received route %s for %s (%d) %04X %02X",
		   hwidb->hw_namestring, cip->reset_correlator, correlator,
		   (code == CIP_CFG_SR_ADD) ? "UP" : "DOWN",
		   cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
		   vcnum,
		   vc->path, vc->device);
	
	/* Set the current desired state */
	vc->state_flags &= ~(CIP_CHG_UP | CIP_CHG_DOWN);
	vc->state_flags |= (code == CIP_CFG_SR_ADD) ?
	    CIP_CHG_UP : CIP_CHG_DOWN;
	
	/* Is an asynchronous state change already pending? */
	if (vc->state_flags & CIP_CHG_PNDING) {
	    /* Put the element back on the preQ for reuse */
	    cip_plevel_element_dispose(cip, plevel_element,
				       DISPOSE_TO_PREQ);
	    return;	
	}
	
	/* Set flag to indicate a pending asynchronous state change */
	vc->state_flags |= CIP_CHG_PNDING;

	/* And  the plevel element as State Change requeust */
	plevel_element->pl_event = PLEVENT_ASYREQ;
	break;
    }	
    cip_plevel_element_put(cip, plevel_element, PLEVEL_RDYQ);
}

/*
 * ---------------------------------------------------------------------
 * cip_cfg_state_change
 * Format of buf:
 *   As described in cip_plevel_enqueue.
 * ---------------------------------------------------------------------
 */
static void cip_cfg_state_change (
    hwidbtype *hwidb,
    cip_idb_infotype *cip,
    plevel_t *plevel_element,
    uchar    *buf,
    ushort    length)
{
    ushort      vcnum;
    cip_vctype *vc;
    ushort      correlator;
    leveltype   level;
    ushort      state_flags;
    
    correlator = GETSHORT(&buf[OFFSET_ASYNC_CORR]);
    vcnum = GETSHORT(&buf[OFFSET_ASYNC_VC]);

    if (correlator != cip->reset_correlator) {
	if (channel_debug)
	    buginf("\n%s:(%04X,%04X) Processing route change: rejected - bad correlator",
		   hwidb->hw_namestring, cip->reset_correlator, correlator);
	cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_PREQ);
	return;	
    }
    vc = cip_find_vc_with_vcnum(cip, vcnum);
    if (!vc) {
	if (channel_debug)	
	    buginf("\n%s:(%04X,%04X) Processing route change: rejected - bad vc (%d)",
		   hwidb->hw_namestring, cip->reset_correlator, correlator,
		   vcnum);
	cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_PREQ);
	return;	
    }
    
    level = raise_interrupt_level(NETS_DISABLE);
    /* Save desired state transition */
    state_flags = vc->state_flags;
    
    /* Turn off pending asynchronous state change indication*/
    vc->state_flags &= ~CIP_CHG_PNDING;
    
    /*
     * Dispose of the element, to assure availability for next asynchronous
     * event
     */
    cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_PREQ);
    
    /* Reenable interrupts; safe to get another state change now */
    reset_interrupt_level(level);
    if (channel_debug)
	buginf("\n%s:(%04X,%04X) Processing route %s for %s (%d) %04X %02X",
	       hwidb->hw_namestring, cip->reset_correlator, correlator,
	       (state_flags & CIP_CHG_UP) ? "UP" : "DOWN",
	       cip_modes[(vc->dev_type & ~CIP_TEARDOWN_VC)],
	       vcnum,
	       vc->path, vc->device);
    
    /* Process the state change */   
    if (state_flags & CIP_CHG_DOWN) {
	cip_delete_static_route(hwidb->firstsw, vc);
	return;
    }
    if (state_flags & CIP_CHG_UP) {
	/* If not PNDING DELETE */
	if ((vc->ack_flags & (CIP_CFG_PNDING | CIP_CFG_DELETE)) != 
	    (CIP_CFG_PNDING | CIP_CFG_DELETE))
	    cip_add_static_route(hwidb->firstsw, vc);
	return;
    }
}

/*
 * ---------------------------------------------------------------------
 * cip_process_queued_events
 *     Process events that have been queued for process-level completion
 *
 * Format of buf:
 *                       PLEVENT_ENABLE
 * Offset field  length  Description
 *   0 correlator[2]     correlator 
 * - - - - - - - - - - - - - - - - - - - - - - - - - 
 *                       PLEVENT_CFGACK
 * Offset field  length  Description
 *   0 code      [4]     CIP_CFG_NACK 0
 *                       CIP_CFG_ACK  1
 *               [ ]     Remaining fields cfg dependent
 * - - - - - - - - - - - - - - - - - - - - - - - - - 
 *                       PLEVENT_ASYREQ
 * Offset field  length  Description
 *   0 code      [4]     CIP_CFG_SR_ADD 2
 *                       CIP_CFG_SR_DEL 3
 *   4 correlator[2]     correlator 
 *   6 vc        [2]     VC
 * ---------------------------------------------------------------------
 */
void cip_process_queued_events (
    hwidbtype *hwidb,
    cip_idb_infotype *cip)
{
    plevel_t *plevel_element;
    ulong     code;
    uchar    *buf;
    ushort    length;

    while ((plevel_element = cip_plevel_element_get(cip, PLEVEL_RDYQ))
	   != NULL) {
	buf = plevel_element->pl_data;
	length = plevel_element->pl_size;
	switch (plevel_element->pl_event) {
	case PLEVENT_ENABLE:           /* Send entire configuration */
	    if (CHANNEL_DEBUG_DEED && channel_debug)
		buginf(" PLEVENT_ENABLE");
	    cip_deferred_enable(hwidb, cip, buf, length);
	    cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_PREQ);
	    break;
	case PLEVENT_CFGACK:           /* Configuration acknowledgement */
	    if (CHANNEL_DEBUG_DEED && channel_debug)
		buginf(" PLEVENT_CFGACK");
            code = GETLONG(&buf[OFFSET_CODE]);
	    buf += sizeof(code);
	    length -= sizeof(code);
	    cip_cfg_acknowledge(hwidb, CIP_CFG_ACK_CIP, code, buf, length);
	    cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_CHUNK);
	    break;
	case PLEVENT_ASYREQ:          /* Asynchronous cfg state change */
	    if (CHANNEL_DEBUG_DEED && channel_debug)
		buginf(" PLEVENT_ASYREQ");
	    cip_cfg_state_change(hwidb, cip, plevel_element, buf, length);
	    /* element has been disposed */
	    break;
	default:
	    if (CHANNEL_DEBUG_DEED && channel_debug)
		buginf(" PLEVENT_HUH");
	    cip_plevel_element_dispose(cip, plevel_element, DISPOSE_TO_CHUNK);
	    break;
	}
    }
}

/*
 * ---------------------------------------------------------------------
 * channel_onesec 
 *     Called every second to do work at process level
 *
 * ---------------------------------------------------------------------
 */

static void channel_onesec (hwidbtype *hwidb)
{
    cip_idb_infotype *cip;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }
    cip_process_queued_events(hwidb, cip);
}

/*
 * cip_info_reset
 */
static void cip_info_reset (idbtype *swidb)
{
    hwidbtype *hwidb;
    cip_idb_infotype *cip;
    cip_vctype *vc;
    
    hwidb = swidb->hwptr;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }
    cip->daughter_type = cip_daughter_type(hwidb);
    cip->data_rate = CIP_PCA_DEFAULT_RATE;  /* data rate only used on PCA */
    cip->cfg_block = 0;  
    cip_init_load_metric_values(cip);
    cip_set_love_letter_timestamp(cip, 0);
    TIMER_STOP(cip->last_love_letter);
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if (vc->flags & CIP_VC_INUSE) {
	    if ((!vc->claw_pair) || (!(vc->device%2 == 1))) {
		/* Delete the static route if one exists */
		cip_delete_static_route(swidb, vc);
		cip_teardown_vc(swidb, cip, vc);
	    }
	}
	memset(&vc->stats, 0, sizeof(statistics));
	memset(&vc->si_stats, 0, sizeof(statistics));
    }
    cip->subchannels = 0;
}

ushort return_NO_CHANNEL_VC (void *dummy, ...)
{
    return(NO_CHANNEL_VC);
}


/*
 * channel_idb_init
 */
void channel_idb_init (hwidbtype *hwidb)
{
    idbtype *swidb = hwidb->firstsw;
    channel_sb_t *chan_sb;
    cip_idb_infotype *cip = NULL;
    char type;
    boolean type_change;

    /*
     * Set the fastswitch flavor
     */
    hwidb->fast_switch_type = FS_CHANNEL;

    /*
     * Do the remainder of the IDB
     */

    chan_sb = channel_get_or_create_sb(hwidb);
    if (!chan_sb) {
        printf(nomemory);
        return;
    }

    /* Do SRB stuff */ 

    chan_sb->get_hwidb_from_vc = (channel_ilan_hwidb_from_vc_t)return_null;
    chan_sb->get_vc_from_macaddr 
                 = (channel_ilan_vc_from_macaddr_t)return_NO_CHANNEL_VC;

    if (is_cip_virtual(hwidb)) {
        hwidb->srb_routecache = TRUE;
        hwidb->maxmtu_def = hwidb->maxmtu = CIP_VIRTUAL_MTU;
	hwidb->firstsw->sub_mtu = hwidb->maxmtu_def;
        chan_sb->get_hwidb_from_vc = cip_ilan_hwidb_from_vc;
    }
    hwidb->enctype = ET_CHANNEL;	 
    hwidb->vencap = channel_vencap;
    hwidb->getlink = channel_getlink;
    hwidb->iqueue = channel_input;

    hwidb->extract_hwaddr = invalidate_hwaddr; /* invalid concept on channel */
    hwidb->extract_snpa = invalidate_snpa; /* invalid concept on channel */
    hwidb->broadcast = (broadcast_t)return_fuz_false; /* no physical broadcast on channel */
    hwidb->encsize = CHANNEL_ENCAPBYTES;
    hwidb->parse_packet = channel_parse_packet;
    hwidb->buffersize = FDDI_BUFFERSIZE_WORDS << 1; /* CHANNELTODO */
    hwidb->nokeepalive = TRUE;

    hwidb->show_support = cip_show_interface;
    hwidb->clear_support = cip_clear_counters;

    /*
     * Determine if output holdQ values are initialized
     *   by checking if "output_defqcount" has been set
     *
     * If output holdQ values not init'd
     *   invoke holdq_init() for first time init
     *
     * Otherwise, keep holdQ values as they are already defined
     *   and just invoke holdq_reset()
     *
     */
    if (hwidb->output_defqcount[DEFAULT_QUEUE(hwidb)] != CHANNEL_HOLDQ) {
        holdq_init(hwidb, CHANNEL_HOLDQ);
    } else {
        holdq_reset(hwidb);
    }

    hwidb->periodic = channel_periodic;
    hwidb->reset = cip_reset;
    
    reg_add_if_onesec(channel_onesec, "channel_onesec");
    reg_add_raw_enqueue(LINK_LOVE, channel_love_raw_pak, "channel_pak_love");

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (is_first_swidb(swidb)) {
	    swidb->ip_split_disabled = 
	      swidb->ip_split_disabled_def = TRUE;	/* CHANNELTODO */
	}
	reg_invoke_media_enctype_setup(swidb, ET_CHANNEL);
	swidb->ip_enctype = ET_CHANNEL; 
	if (swidb->tbridge_on_swidb) {	/* CHANNELTODO */
	    hwidb->bridge_transit = TRUE;
	    swidb->tbridge_media = TBR_ATM_TRANSIT;
	}

	type = cip_daughter_type(hwidb);
	type_change = FALSE;
        cip = chan_sb->cip_info;
	if (cip) {
	    /* 
	     * If this routine is called as a via "micro reload", then
	     * cip_info may already exist.
	     */ 
	    if (cip->daughter_type != type) {
		/*
		 * Daughter type may have changed due to OIR
		 */
		type_change = TRUE;
		errmsg(&msgsym(CIPADPTCHNG, CBUS),
		       hwidb->hw_namestring,
		       cip_daughter_string(cip->daughter_type),
		       cip_daughter_string(type));
		cip_info_reset(swidb);
	    }
	} else {
	    cip_create_info(hwidb);
	} 
	switch (type) {
	case CIP_PCA_DAUGHTER:
	    if (!cip  || type_change) {
		/* default to the smaller value */
		set_default_interface_bandwidth(swidb,
						VISIBLE_CIP_PCA_BANDWIDTH_3);
		set_default_interface_delay(swidb, CIP_PCA_DELAY_3);
	    }
	    break;
	case CIP_ECA_DAUGHTER:
	    set_default_interface_bandwidth(swidb,
					    VISIBLE_CIP_ECA_BANDWIDTH);
 	    set_default_interface_delay(swidb, CIP_ECA_DELAY);
	    break;
	    /* Here  is where the virtual interface will be seen */
	case CIP_VIRTUAL_INTERFACE:
	    /* fall through */
	case CIP_NONE:
	default:			/* Good enough values... */
	    set_default_interface_bandwidth(swidb,
					    VISIBLE_CIP_ECA_BANDWIDTH);
 	    set_default_interface_delay(swidb, CIP_ECA_DELAY);
	    break;
	}
    }
}
/*
 * ---------------------------------------------------------------------
 * Handle data coming from the CIP 
 *  - love letters 
 *    processed as they are received (at interrupt level)
 * ---------------------------------------------------------------------
 */
static void channel_love_raw_pak (paktype *pak) 
{
    cip_parse_love_letter(pak->if_input->hwptr, pak->datagramstart,
			  pak->datagramsize);
    datagram_done(pak);
}

/* 
 * comon routine to handle love letter, both c7000 and rsp use same love 
 * letter format
 * 
 * This is called from interrupt level
 */

static void cip_parse_love_letter (
    hwidbtype *hwidb,
    uchar *love_ptr, 
    ulong length) 
{
    cip_idb_infotype *cip;
    cip_vctype *vc;
    cip_ll *ll;
    cip_ll_card *card;
    cip_ll_db *db;
    cip_ll_dev *dev;
    cip_ll_pa *pa;
    ulong indx;
    ulonglong *cip_timestamp_p;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	if (channel_debug_love)
	    buginf("\ncip_love_letter: received ll, but no cip_info");
	return;
    }

    if (channel_debug_love) {
	buginf("\n%s: Love letter received, bytes %d",
	       hwidb->hw_namestring,
	       length);
    }

    if (length < sizeof(cip->cip_timestamp)) {
	return;
    }
    
    GET_TIMESTAMP(cip->last_love_letter);
    cip_timestamp_p = (ulonglong *)love_ptr;
    cip->cip_timestamp = *cip_timestamp_p;
    
    indx = ltob(LL_TIMESTAMP_SIZE);   
    ll = (cip_ll *)(love_ptr + indx);   
    
    while (indx < length) {
	switch (ll->type) {
	case CIP_MIB_CARD: 
	    card = (cip_ll_card *)&ll->data[0];
	    cip->sram_size = card->sram_size;
	    cip->sram_free = card->sram_free;
	    cip->dram_size = card->dram_size;
	    cip->dram_free = card->dram_free;
	    cip->cpu_util = card->cpu_util;
	    if (ltob(ll->len - LL_HEADER_SIZE) > 
		(offsetof(cip_ll_card, cpu_util) +
		 sizeof(card->cpu_util))) {
		cip->cip_cpu_load.one_min = card->cpu_load_one_min;
		cip->cip_cpu_load.five_min = card->cpu_load_five_min;
		cip->cip_cpu_load.sixty_min = card->cpu_load_sixty_min;
		cip->cip_dma_load.one_min = card->dma_load_one_min;
		cip->cip_dma_load.five_min = card->dma_load_five_min;
		cip->cip_dma_load.sixty_min = card->dma_load_sixty_min;
	    }
 	    break;
	case CIP_MIB_DB: 
	    db = (cip_ll_db *)&ll->data[0];
	    cip->db_flags = db->flags;
	    cip->link_addr = db->link_addr;
	    bcopy(&db->node_id[0], &cip->node_id[0], ltob(LIR_ND_SIZE));
	    cip->devices = db->devices;
	    hwidb->counters.input_resource = db->no_buffers; 
	    hwidb->counters.input_crc
		= hwidb->counters.input_error = db->crc_errors;
	    cip->no_buffers = db->no_buffers;
	    cip->crc_errors = db->crc_errors;
	    cip->load = db->load;
	    bcopy(&db->link_incident[0], &cip->link_incident[0],
		  ltob(MAX_LIR_TYPES));
	    cip->pacb_flags = db->pacb_flags;
	    if (ltob(ll->len - LL_HEADER_SIZE) > 
		(offsetof(cip_ll_db, pacb_flags) +
		 sizeof(db->pacb_flags))) {
		cip->cip_channel_load.one_min = db->channel_load_one_min;
		cip->cip_channel_load.five_min = db->channel_load_five_min;
		cip->cip_channel_load.sixty_min = db->channel_load_sixty_min;
	    }
	    break;
	case CIP_MIB_DEV:
	    dev = (cip_ll_dev *)&ll->data[0];
	    vc = cip_find_vc(cip, (uint)dev->dev_type, (uint)dev->path,
			     (uint)dev->dev_address);
	    if (!vc) {
		if (channel_debug_love) {
		    buginf("\ncip_love_letter(): no vc found for (%d, "
			   "%04x, %02x)", 
			   dev->dev_type, dev->path, dev->dev_address);
		}
		break;
	    }
	    vc->path_flags = dev->path_flags;
	    vc->dev_flags = dev->dev_flags;
	    vc->last_sense = dev->last_sense;
	    vc->last_sense_time = dev->last_sense_time[0];
	    vc->last_sense_time <<= 32;
	    vc->last_sense_time += dev->last_sense_time[1];
	    bcopy(&dev->stats, &vc->stats, sizeof(statistics));
	    break;
	case CIP_MIB_PA: 
	    pa = (cip_ll_pa *)&ll->data[0];
	    hwidb->counters.input_resource = pa->no_buffers; 
	    hwidb->counters.input_crc
		= hwidb->counters.input_error = pa->crc_errors;
	    cip->no_buffers = pa->no_buffers;
	    cip->crc_errors = pa->crc_errors;
	    cip->pacb_flags = pa->pacb_flags;
	    break;
	default:
	    if (channel_debug_love)
		buginf("\ncip_love_letter(): unknown code type %d", ll->type);
	    break;
	}
	if (ll->len) {
	    indx += ltob(ll->len);  /* convert length from words to bytes */
	} else {
	    /*
	     * A zero length is invalid, so skip over this length 
             * field and try again.
	     */
	    indx += ltob(LL_LEN_SIZE); /* Bad length - increment by minimum amount */
	    if (channel_debug_love)
		buginf("\ncip_love_letter() corrupt, section length 0");
	}
	if (indx > length) {
	    if (channel_debug_love) {
		buginf("\ncip_love_letter() corrupt, indx %d, length %d",
		       indx, length);
	    }
	    return;
	}
	ll = (cip_ll *)&ll->data[ll->len - LL_HEADER_SIZE];  /* subtract for header */
    }
    return;
}


/*
 * ---------------------------------------------------------------
 * A configuration has been acknowledged by the CIP 
 * --- this is called at process level ---
 *
 * NOTE: Also handle the messages (via errmsg) for all failures:
 *       if not positive ack, and locally called, then the send 
 *       failed.
 *       If not positive ack, and came from CIP , the CIP also
 *       (probably) sent up a logger message explaining why
 *
 * Values of ack_flags
 *        CIP_CFG_ACK_CIP      0x0001   CIP   acknowledgement
 *        CIP_CFG_ACK_LOCAL    0x0002   local acknowledgement
 *
 * Values of code:
 *        CIP_CFG_NACK    -- Negative response to a cfg command
 *        CIP_CFG_ACK     -- Positive response to a cfg command
 *
 * ---------------------------------------------------------------
 */

static void cip_cfg_acknowledge (
    hwidbtype *hwidb,
    ushort     ack_flags,
    ulong      code,
    uchar     *buf,
    ulong      len)
{
    idbtype          *swidb;
    cip_idb_infotype *cip;
    uchar             cfg_cmd;
    boolean           teardown;
    boolean           positive_ack;
    ushort            vcnum;
    cip_vctype       *vc;
    ushort            correlator;

    swidb = hwidb->firstsw;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	cip_unexpected_cfg_ack(swidb,"No CIP data block",
			       buf, len);
	return;
    }
    
    cfg_cmd = buf[OFFSET_CFG_CMD];
    
    if (cip_is_tn3270s_cmd(cfg_cmd & ~CIP_TEARDOWN_VC)) {
	cip_tn3270s_cfg_acknowledge(hwidb, ack_flags, code, buf, len);
	return;
    }

    if (cip_is_vinterface_cmd(cfg_cmd & ~CIP_TEARDOWN_VC)) {
	cip_vlan_cfg_acknowledge(hwidb, ack_flags, code, buf, len);
	return;
    }

    if (cfg_cmd & CIP_TEARDOWN_VC)
	teardown = TRUE;
    else
	teardown = FALSE;
    
    if (code == CIP_CFG_ACK)
	positive_ack = TRUE;
    else
	positive_ack = FALSE;
    
    switch (cfg_cmd & ~CIP_TEARDOWN_VC) {
    case CHANNEL_NODEID:
	/* currently no state info kept on this beast           */
        /* No action for either positive or negative acknowledgment */
	return;
    case CHANNEL_CTA_TEST_TOOL:
    {
	ushort command, offset;
	cta_tt_cmdtype *cta_tt_p;
	
        correlator = GETSHORT(&buf[len-sizeof(correlator)]);
        command = GETSHORT(&buf[2]);
        offset = GETSHORT(&buf[4]);
	
	cta_tt_p = cip_find_cta_tt(cip, command, offset);
	if (!cta_tt_p) {
	    cip_unexpected_cfg_ack(swidb,"No such cta tt",
				   buf, len);
	    return;
	}
	if (channel_debug) 
	    buginf("\n%s:(%04X,%04X) Received %s ack for %s%s %04X %d",
		   hwidb->hw_namestring, cip->reset_correlator, correlator,
		   positive_ack ? "POSITIVE" : "NEGATIVE",
		   (teardown) ? "no " : "",
		   cip_cfg_name_str(cfg_cmd),
		   command, offset);
	
	if (cip->reset_correlator != correlator) {
	    cip_unexpected_cfg_ack(swidb,"Correlator mismatch", 
				   buf, len);
	    return;
	}
	
	/* Turn off the PNDING status bit */
	if (!(cta_tt_p->ack_flags & CIP_CFG_PNDING)) {
	    cip_unexpected_cfg_ack(swidb,"PNDING",
				   buf, len);
	    return;
	}
	cta_tt_p->ack_flags &= ~CIP_CFG_PNDING;
	
	if (teardown) {
	    if (!(cta_tt_p->ack_flags & CIP_CFG_DELETE)) {
		cip_unexpected_cfg_ack(swidb,"DELETE",
				       buf, len);
		return;
	    }
	    cta_tt_p->ack_flags &= ~CIP_CFG_DELETE;
	    
	    if (!positive_ack) {
		/* Hmmm... Nacking a delete */
		/* Report our surprise for those who care */
		cip_unexpected_cfg_ack(swidb,"DELETE NACKED",
				       buf, len);
		cip_nack_report(hwidb, teardown,
				(cfg_cmd & ~CIP_TEARDOWN_VC), cta_tt_p);
	    }
	    /* Nuke it in either case */
	    cip_cta_tt_remove(cip, command, offset);
	    return;
	}
	
        /* Otherwise new                       */
	if (!(cta_tt_p->ack_flags & CIP_CFG_CREATE)) {
	    cip_unexpected_cfg_ack(swidb,"CREATE",
				   buf, len);
	    return;
	}       
	cta_tt_p->ack_flags &= ~CIP_CFG_CREATE;
	if (!positive_ack) {
	    cip_nack_report(hwidb, teardown,
			    (cfg_cmd & ~CIP_TEARDOWN_VC), cta_tt_p);
	    cip_cta_tt_remove(cip, command, offset);
	}
	return;
	break;
    }
    case CHANNEL_CLAW:
    case CHANNEL_CSNA:
    case CHANNEL_LCS:
    case CHANNEL_OFFLOAD:
    case CHANNEL_IVORIE:
    case CHANNEL_DP:
        correlator = GETSHORT(&buf[len-sizeof(correlator)]);
        vcnum = GETSHORT(&buf[OFFSET_CFG_VCNUM]);

	vc = cip_find_vc_with_vcnum(cip, vcnum);
	if (!vc) {
	    cip_unexpected_cfg_ack(swidb,"No such vcnum",
				   buf, len);
	    return;
	}
	if (channel_debug) 
	    buginf("\n%s:(%04X,%04X) Received %s ack for %s%s %04X %02X",
		   hwidb->hw_namestring, cip->reset_correlator, correlator,
		   positive_ack ? "POSITIVE" : "NEGATIVE",
		   (teardown) ? "no " : "",
		   cip_cfg_name_str(cfg_cmd),
		   vc->path, vc->device);
		
	if (cip->reset_correlator != correlator) {
	    cip_unexpected_cfg_ack(swidb,"Correlator mismatch", 
				   buf, len);
	    return;
	}
	
	/* Turn off the PNDING status bit */
	if (!(vc->ack_flags & CIP_CFG_PNDING)) {
	    cip_unexpected_cfg_ack(swidb,"PNDING",
				   buf, len);
	    return;
	}
	vc->ack_flags &= ~CIP_CFG_PNDING;
	
	if (teardown) {
	    if (!(vc->ack_flags & CIP_CFG_DELETE)) {
		cip_unexpected_cfg_ack(swidb,"DELETE",
				       buf, len);
		return;
	    }
	    vc->ack_flags &= ~CIP_CFG_DELETE;
	    
	    if (positive_ack) {
		cip_vc_del_accepted(hwidb, ack_flags,
				    cfg_cmd, vc);
	    } else {
		cip_vc_del_rejected(hwidb, ack_flags,
				    cfg_cmd, vc);
	    }
	    return;
	}
	
        /* Otherwise check if update (only CSNA) */
	if (vc->ack_flags & CIP_CFG_UPDATE) {
	    vc->ack_flags &= ~CIP_CFG_UPDATE;

	    /* Nothing else to do */

	    return;
	}
	
        /* Otherwise new                       */
	if (!(vc->ack_flags & CIP_CFG_CREATE)) {
	    cip_unexpected_cfg_ack(swidb,"CREATE",
				   buf, len);
	    return;
	}       
	vc->ack_flags &= ~CIP_CFG_CREATE;

	if (positive_ack) {
	    cip_vc_cfg_accepted(hwidb, ack_flags,
				cfg_cmd, vc);
	} else {
	    cip_vc_cfg_rejected(hwidb, ack_flags,
				cfg_cmd, vc);
	}
	return;
	break;
    default:
	cip_unexpected_cfg_ack(swidb,"Unexpected command code",
			       buf, len);
	return;
    }
}

/* ---------------------------------------------------------------
 */
static void cip_unexpected_cfg_ack (
    idbtype          *swidb,
    char             *pDescription,
    uchar            *buf,
    ulong             len)
{
    if (channel_debug) {
	buginf("\nUnexpected acknowledgment: %s, %s",
	       swidb->namestring, pDescription);
    }
}
static void cip_nack_report (
    hwidbtype *hwidb,
    boolean    teardown,
    ushort     cfg_cmd,
    void      *cfginfo)
{
    cip_idb_infotype *cip;
    cip_vctype *vc;
    cta_tt_cmdtype *cta_tt_p;
    char errbuf[32];
    
    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
	return;
    }
    
    /* We are here because either the CIP rejected the cfg command,
     * or becuase the cfg command could not be sent to the CIP 
     * due to some 'unusual' reason, eg. the CIP cbus timeout 
     */
    
    switch (cfg_cmd) {
    case CHANNEL_NODEID:
	errbuf[0] = 0;
	break;
    case CHANNEL_CTA_TEST_TOOL:
	cta_tt_p = cfginfo;
	sprintf(errbuf, "%s %-04X %-3d",
		(teardown ? "(remove)" : " "),
		cta_tt_p->command,
		cta_tt_p->offset);
	break;
    case CHANNEL_CLAW:
    case CHANNEL_CSNA:
    case CHANNEL_LCS:
    case CHANNEL_OFFLOAD:
    case CHANNEL_IVORIE:
    case CHANNEL_DP:
	vc = cfginfo;
	sprintf(errbuf, "%s %04X %02X",
		(teardown ? "(remove)" : " "),
		vc->path,
		vc->device);
	break;
    default:
	sprintf(errbuf, " cmd %d",
		cfg_cmd);
	break;
    }
    errmsg(&msgsym(CIPCFGFAIL, CBUS), 
	   hwidb->hw_namestring,
	   cip_cfg_name_str(cfg_cmd), 
	   errbuf);
    return;
}

/*
 * ---------------------------------------------------------------------
 * cip_vc_del_accepted
 *    Positive acknowledgement of a subchannel cfg delete
 *
 *    o Finish the teardown by calling cip_teardown_vc
 *    o Remove the asynchronous state change PREQ element
 * ---------------------------------------------------------------------
 */
static void cip_vc_del_accepted (
    hwidbtype  *hwidb,
    ushort      ack_flags,
    ushort      cfg_cmd,
    cip_vctype *vc)
{
    idbtype *swidb;
    cip_idb_infotype *cip;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return;
    }

    swidb = hwidb->firstsw;
    cip_teardown_vc(swidb, cip, vc);

    /* Remove the asynchronous PLEVEL element from the PREQ */
    if (ack_flags & CIP_CFG_ACK_CIP)
	cip_plevel_preQ_to_chunk(cip);
}
/*
 * ---------------------------------------------------------------------
 * cip_vc_del_rejected
 *    Negative acknowledgement of a subchannel cfg delete
 *    This may be called if the delete request could not be sent to 
 *    the CIP
 *
 *    o Undo the initial teardown attempt:
 *      Reset the CIP_TEARDOWN_VC bit.
 *      Note: static route will already have been removed.
 * ---------------------------------------------------------------------
 */
static void cip_vc_del_rejected (
    hwidbtype  *hwidb,
    ushort      ack_flags,
    ushort      cfg_cmd,
    cip_vctype *vc)
{
    /* Hmmm... Nacking a delete */
    cip_nack_report(hwidb, (cfg_cmd & CIP_TEARDOWN_VC),
		    (cfg_cmd & ~CIP_TEARDOWN_VC), vc);
    
    vc->dev_type &= ~CIP_TEARDOWN_VC;
}

/*
 * ---------------------------------------------------------------------
 * cip_vc_cfg_accepted
 *    Positive acknowledgement of a subchannel cfg
 *
 *    o Add static route if IVORIE or DP,
 *      CLAW and OFFLOAD static routes will be added
 *      explicitly by CIP request
 * ---------------------------------------------------------------------
 */
static void cip_vc_cfg_accepted (
    hwidbtype  *hwidb,
    ushort      ack_flags,
    ushort      cfg_cmd,
    cip_vctype *vc)
{
    switch (cfg_cmd) {
    case CHANNEL_IVORIE:
    case CHANNEL_DP:
	/* Data Pusher and Ivorie have static routes
	 * added at cfg acceptance
	 */
	if (ack_flags & CIP_CFG_ACK_CIP)
	    cip_add_static_route(hwidb->firstsw, vc);
	break;
    default:
	break;
    }
}
/*
 * ---------------------------------------------------------------------
 * cip_vc_cfg_rejected
 *    Negative acknowledgement of a subchannel cfg
 * ---------------------------------------------------------------------
 */
static void cip_vc_cfg_rejected (
    hwidbtype  *hwidb,
    ushort      ack_flags,
    ushort      cfg_cmd,
    cip_vctype *vc)
{
    idbtype *swidb;
    cip_idb_infotype *cip;

    swidb = hwidb->firstsw;

    cip = channel_sb_get_cip_info(hwidb);
    if (!cip) {
        return;
    }

    cip_nack_report(hwidb, (cfg_cmd & CIP_TEARDOWN_VC),
		    (cfg_cmd & ~CIP_TEARDOWN_VC), vc);
    cip_teardown_vc(swidb, cip, vc);
    
    /* Remove the asynchronous PLEVEL element from the PREQ */
    if (ack_flags & CIP_CFG_ACK_CIP)
	cip_plevel_preQ_to_chunk(cip);
}

/* **************  Start of SNMP related routines *************** */

#if 0
/* The following is the schema used for the SNMP get routines below */
data * cip_somethingGet (indices, int searchType)
{
  best = NULL;

  FOR_ALL_HWIDBS (hwidb) {
    /* if we exactly match, we found what we want regardless of
       whether it is a get or getnext */
    if (hwidb->fields == indices) {
      best = hwidb;
      break;
    }
    /* We didn't exactly match. If this is a get next, see if the hwidb
       is a better next candidate than the best so far. */
    if (searchType == SEARCH_NEXT) {
      /* this is a get next, see if current hwidb is a next candidate */
      if (hwidb->fields >= indices) {
	/* current record is > requested, is it the best? */
	if ((best == NULL) || (best->fields > hwidb->fields)) {
	  /* new record is the best match so far */
	  best = hwidb;
	}
      }
    }
  }
  
  if (searchType == SEARCH_NEXT && best) {
    indices = best->fields;
  }

  return best;
}
#endif


cip_idb_infotype * cip_cardInfoGet (long *slotP, int searchType)
{
  /* This routine scans the hwidbs looking for the cip_idb_infotype
     which belongs to slot and has the most up-to-date information
     from the CIP ie the most recent love-letter.
  */

  hwidbtype *hwidb;
  cip_idb_infotype *best = NULL;
  int daughter_boards_left = CIP_MAX_DAUGHTERS;
  ulonglong last_love;
  ulonglong love_time;
  long best_slot = 0;
  boolean possible_best = FALSE;
  
  if (!slotP) return NULL;
  
  last_love = 0;
  FOR_ALL_HWIDBS (hwidb) {
      if (is_cip(hwidb)) {
	  /* if we exactly match, we found what we want regardless of
	     whether it is a get or getnext */
	  if (hwidb->slot == *slotP) {
	      
	      if (daughter_boards_left < 0) {
 		  /* 
 		   * There are no daughter_boards_left on this interface
 		   * (including one for the virtual interface).
 		   * If an exact match was requested, there is no use in
 		   * searching any further.  If it was a GetNextRequest, 
 		   * keep searching for other interfaces on other slots.
 		   */
 		  if (searchType == SEARCH_MATCH) {
 		      break;
 		  }
 	      } else {
 		  /* 
 		   * Have not exhausted all daughter boards.  This may
 		   * be the best.
 		   */
		  possible_best = TRUE;
	      }
	      daughter_boards_left--;
	      
	  } else {
	      
	      /* We didn't exactly match. If this is a get next, see if the hwidb
		 is a better next candidate than the best so far. */
	      if (searchType == SEARCH_NEXT) {
		  
		  /* this is a get next, see if current hwidb is a candidate */
		  if (hwidb->slot > *slotP) {
		      /* possible candidate */
		      
		      /* current record is > requested, is it the best? */
		      if ((best == NULL) || 
			  (best_slot >= hwidb->slot)) {
			  possible_best = TRUE;
		      }
		  }
	      }
	  }
    
	  if (possible_best) {
	      cip_idb_infotype *this_cip;
	      possible_best = FALSE;
	      /* select best as the one with the most recent stats */
	      this_cip = channel_sb_get_cip_info(hwidb);
	      if (this_cip) {
		  if (hwidb->slot != best_slot) {
 		      /* 
 		       * First time picking this slot as best, so love  
 		       * timestamp we saved was for a different interface
 		       * and needs to be reset.
 		       */
 		      last_love = 0;
 		  }
 		  
		  love_time = cip_get_love_letter_timestamp(this_cip);
		  if ((love_time > last_love) || (!best)) {
		      /* 
 		       * If the love letter timestamp for the best and the 
 		       * current interface are equal, we will stick with 
 		       * the first one we found.  However, if we don't 
 		       * have a best, go with the new one we just found.
 		       */
		      last_love = love_time;
		      best = this_cip;
		      best_slot = hwidb->slot;
		  }
	      }
	  }
      }
  }
  
  if (searchType == SEARCH_NEXT) {
     *slotP = best_slot;
 }
  
  return best;
}

cip_idb_infotype * cip_interfaceInfoGet (long *slotP, long *portP, 
					 int searchType)
{
  /* This routine scans the hwidbs looking for the cip_idb_infotype
     which corresponds to slot and port. If an exact match is found, 
     the  cip_idb_infotype is returned otherwise the next lexographical
     cip_idb_infotype is returned when searchType == SEARCH_NEXT. 
     If none found, NULL is returned.
  */
  hwidbtype *hwidb;
  cip_idb_infotype *best = NULL;
  long best_mci_index = 0;
  int unit;
  
  if (!slotP || !portP) return NULL;

  unit = cbus_slotunit2unit(*slotP, *portP);

  FOR_ALL_HWIDBS (hwidb) {
    cip_idb_infotype *cip = channel_sb_get_cip_info(hwidb);
    /* 
    * if we exactly match, we found what we want regardless of
    *   whether it is an SNMP GetRequest or GetNextRequest
    */
    if ((hwidb->mci_index == unit) && 
        is_cip(hwidb) && !is_cip_virtual(hwidb) && cip) {
      best = cip;
      best_mci_index = hwidb->mci_index;
      break;
    } else {

      /* We didn't exactly match. If this is a get next, see if the hwidb
	 is a better next candidate than the best so far. */
      if (searchType == SEARCH_NEXT) {
	/* this is a get next, see if current hwidb is a candidate */
	if ((hwidb->mci_index > unit)  && 
	     is_cip(hwidb) && !is_cip_virtual(hwidb) && cip) {
	  /* current record is > requested, is it the best? */
	  if ((best == NULL) || (best_mci_index > hwidb->mci_index)) {
	    /* new record is the best match so far */
            best = cip;
	    best_mci_index = hwidb->mci_index;
	  }
	}
      }
    }
  }
  
  if (searchType == SEARCH_NEXT && best) {
      cbus_unit2slotunit(best_mci_index, (int *)slotP, (int *)portP);
  }

  return best;
}


static long cip_find_next_nonzero_link (cip_vctype *vc, long clawLink)
{
    long i, link;
    boolean found_nonzero_stats;
    
    found_nonzero_stats = FALSE;
    link = clawLink;
    
    for (i = link; i < CLAW_MAX_LINKS; i++) {
	if (cip_nonzero_data_stats(vc, i)) {
	    link = i;
	    found_nonzero_stats = TRUE;
	    break;
	}
    }
    if (!found_nonzero_stats) {
	link = 0;
    }
    
    return link;
}


static cip_vctype * cip_findNextPathDevice(cip_idb_infotype *cip, 
		    long path, long device, int searchType,
                    vc_check_proc (*vc_validate))
{
    /* 
    * This routine scans the cip looking for a For a cip_vctype which
    *  corresponds to the path and device. If an exact match is found, 
    *  the  cip_vctype is returned otherwise the next lexographical
    *  cip_vctype is returned when searchType == SEARCH_NEXT. 
    *  If none found, NULL is returned.
    *  
    * On a SEARCH_NEXT request, the VC returned can represent the path and 
    * device that were requested. 
    *
    * On a NEXT_BEST request, an exact match is not allowed.
    */
    
    cip_vctype *vc = NULL, *best = NULL;
    
    if (!cip) return NULL;
    
    FOR_ALL_VCS_ON_CIP_IDB_INFO(cip, vc) {
	if (vc->flags & CIP_VC_INUSE) {       /* make sure this vc is inuse */
	    if (vc->path == path && vc->device == device && 
		(vc_validate == NULL || (*vc_validate)(vc))  &&
		searchType != NEXT_BEST) {
		/* 
		* If we found the same VC as before (ie: same path, 
		* device), consider it a match.
		*/
		best = vc;
		break;
	    }
	    /* We didn't exactly match. For a get, we try the next vc */
	    if (searchType == SEARCH_NEXT || searchType == NEXT_BEST) {
		/* Its a get next. */
		if ((vc->path > path || (vc->path == path && (vc->device > device))) && 
		    ((vc_validate == NULL) || (*vc_validate)(vc))) {
		    /* 
		    * The candidate VC's path & device are lexicographically greater
		    * than the path and device that were passed to this routine.
		    *
		    * Check that it is better than the best we have found so far.  That 
		    * is, check that its path and device are lexicographically closer 
		    * to (ie: lower than) the path and device of the current best.
		    */
		    if (!best || best->path > vc->path || 
			(best->path == vc->path && best->device >= vc->device)) {
			/* found a better vc */
			best = vc;
		    }
		}
	    }
	} 
    }
    return best;
}

static cip_vctype * cip_getSubchannelInfo (long *slotP, long *portP, 
					   long *pathP,
					   long *deviceP, long *clawP1,
					   hwidbtype **vc_hwidb, int searchType,
                                           vc_check_proc (*vc_validate))
{
    /* This routine scans the hwidbs looking for the cip_vctype
       which corresponds to slot, port, path, device, optional 
       clawLnk and optional vc_check_proc. If an exact match is found, 
       the cip_vctype is returned otherwise the next lexographical 
       cip_vctype is returned for searchType == SEARCH_NEXT. If none 
       found, NULL is returned.
       */
    hwidbtype *hwidb;
    cip_vctype * best = NULL;
    long best_mci_index = 0, clawLink = 0;
    int unit;
    cip_vctype *vc = NULL;
    long       next_link = 0;
    
    if (!slotP || !portP || !pathP || !deviceP)
	return NULL;
    
    if (clawP1) {
	clawLink = *clawP1;
    }
    
    unit = cbus_slotunit2unit(*slotP, *portP);
    
    FOR_ALL_HWIDBS (hwidb) {  
	cip_idb_infotype *cip = channel_sb_get_cip_info(hwidb);
	/* 
	* if we exactly match, we found what we want regardless of
	* whether it is a get or getnext 
	*/
	if ((hwidb->mci_index == unit)  && 
	    is_cip(hwidb) && !is_cip_virtual(hwidb) && cip) {
	    next_link = 0;
	    
	    vc = cip_findNextPathDevice(cip, *pathP, *deviceP, 
					SEARCH_MATCH, vc_validate);
	    if (vc) {
		/* 
		* The VC is an exact match.  Use it if the clawLink is within
		* the basic set or has nonzero data_stats.
		*/
		if ((clawLink < cip_num_mode_links(vc->dev_type)) || 
 		    (cip_nonzero_data_stats(vc, clawLink)))  {
		    best = vc;
		    best_mci_index = hwidb->mci_index;
		    if (vc_hwidb)
			*vc_hwidb = hwidb;
		    break;
		} else {
 		    /*
		    * Remember the next nonzero link, if any, for use in case this 
		    * is a GetNextRequest 
		    */
 		    next_link = cip_find_next_nonzero_link(vc, clawLink);
 		}
	    }
	}
	
	/*
	* HWIDB has no VC exactly matching the requested slot/port/path/device/link.
	* If this is a GetNextRequest, see if the hwidb has a 'next best' VC that 
	* is better than the best so far.  That is, the HWIDB is equal or higher in 
        * lexicographical order (determined by mci_index) and has a VC that is next 
        * best in lexicographical order.
	*/
	if (searchType == SEARCH_NEXT) {
	    /* this is a get next, see if the vc is a candidate */
	    if ((hwidb->mci_index >= unit)  && 
		is_cip(hwidb) && !is_cip_virtual(hwidb) && cip) {
		if (!best || (best_mci_index >= hwidb->mci_index)) {
		    if (hwidb->mci_index > unit) {
			/* 
			* If working with a new (lexicographically higher) unit,  
			* start searching VCs from the start of the address range.
			*/
			vc = cip_findNextPathDevice(cip, 0, 0, SEARCH_NEXT,
						    vc_validate);
		    } else {
			/* 
			* Same unit.  If the matching VC was found and it has
			* non-zero stats for clawLinks beyond the requested one,
			*  use it along with the next non-zero link.
			* Otherwise, we've exhausted all links on the current VC, 
			* so search for a VC that is lexicographically higher. 
			*/
 			if (next_link != 0) {             
 			    /* 
			    * We have a matching unit/path/device 
			    * and it has additional nonzero links 
			    */
 			    clawLink = next_link;
 			} else { 
			    /* 
			    * We've exhausted all links on the current VC, 
			    * or didn't have a matching VC, so search
			    * for a VC that is lexicographically higher.
			    */
			    vc = cip_findNextPathDevice(cip, *pathP, *deviceP, 
							NEXT_BEST, vc_validate);
			}
		    }
		    if (vc) {
			/* new record is the best match so far */
			best = vc;
			best_mci_index = hwidb->mci_index;
			if (vc_hwidb) {
			    *vc_hwidb = hwidb;
			}
		    }
		}
	    }
	}
    }
  
    /* 
    * For a GetNextRequest, update the slot/port, path, device, and clawLink. 
    */
    if (searchType == SEARCH_NEXT && best) {
	
	/* Copy the correct clawLink to the caller's area           */
 	if (clawP1) {
 	    
 	    /* if the unit, path, or device has changed, reset the clawLink */
 	    if (best_mci_index != unit || 
 		best->path != *pathP || 
 		best->device != *deviceP) {
 		*clawP1 = 0;
 		
 	    } else { 
 		*clawP1 = clawLink;
 	    }
 	}
	
	cbus_unit2slotunit(best_mci_index, (int *)slotP, (int *)portP);
	*pathP = best->path;
	*deviceP = best->device;
    }
    return best;
}

cip_vctype * cip_getSubchannel (long *slotP, long *portP, long *pathP,
				long *deviceP, int searchType)
{
  /* Go get the cip_vctype base on the indices. */
  cip_vctype *vc = cip_getSubchannelInfo(slotP, portP, pathP, deviceP, 
					 NULL, /* claw index not present */
					 NULL, searchType, NULL);
  return vc;
};

static boolean claw_vc_check ( cip_vctype *vc) {
   return ((vc->dev_type == CHANNEL_CLAW) || (vc->dev_type == CHANNEL_OFFLOAD));
}
 
cip_vctype * cip_getSubchannelClaw (long *slotP, long *portP, long *pathP,
				    long *deviceP, long *clawP,
				    int searchType)
{
  /* This routine returns the cip_vctype based on the indices. */
  cip_vctype *vc = NULL;
  if (clawP) {
    vc = cip_getSubchannelInfo(slotP, portP, pathP, deviceP, clawP, 
			       NULL, searchType, claw_vc_check);
  };
  return vc;
};

static boolean claw_config_vc_check ( cip_vctype *vc) {
   return ((vc->dev_type == CHANNEL_CLAW) &&
           (!(vc->flags & CIP_ADJ_DEV) || ((vc->device%2) != 1)));
}
 
cip_vctype * cip_getSubchannelClawConfig (long *slotP, long *portP, long *pathP,
				          long *deviceP, long *clawP,
				          int search)
{
   return cip_getSubchannelInfo(slotP, portP, pathP, deviceP, 
                                clawP, NULL, search, claw_config_vc_check);
};

static boolean offload_config_vc_check ( cip_vctype *vc) {
  return ((vc->dev_type == CHANNEL_OFFLOAD) &&
          (!(vc->flags & CIP_ADJ_DEV) || ((vc->device%2) != 1)));
}
 
cip_vctype *cip_getSubchannelOffloadConfig (long *slotP, long *portP,
                                            long *pathP,long *deviceP,
                                            int search) {
  return cip_getSubchannelInfo( slotP, portP, pathP, deviceP, 
                                NULL, NULL, search, offload_config_vc_check);
}

static boolean csna_config_vc_check ( cip_vctype *vc) {
  return (vc->dev_type == CHANNEL_CSNA);
}
 
cip_vctype *cip_getSubchannelCsnaConfig (long *slotP, long *portP,
                                         long *pathP,long *deviceP,
                                         hwidbtype **hwidb, int search) {
  return cip_getSubchannelInfo( slotP, portP, pathP, deviceP, 
                                NULL, hwidb, search, csna_config_vc_check);
}

data_stats * cip_getDataStats (cip_vctype *vc, long claw_link)
{
  data_stats *dataStatsP = NULL;
  /*
     * return pointer to data_stats if we have a valid vc & claw_link.
     */
    if (vc && 
	(claw_link >= 0) &&
	(claw_link < CLAW_MAX_LINKS))  {

	dataStatsP = &vc->stats.data[claw_link];
  }
  return dataStatsP;
}

ulong cip_subchannelWriteBlocksDropped (cip_vctype *vc)
{
  /* This routine sums the write blocks dropped for each claw link in
      the the vc */

  int i;
  ulong result = 0;
  
  if (vc) {
    for (i=0; i< cip_num_mode_links(vc->dev_type); i++) {
      result += vc->stats.data[i].write_blocks_dropped;
    }
  }
  return result;
}

ulong cip_timeSinceLastReset (cip_idb_infotype * cip)
{
  /* This routine returns the number of seconds since the cip has neen reset. */

  ulong result = 0;
  if (cip && TIMER_RUNNING(cip->timeOfReset))
      result = ELAPSED_TIME(cip->timeOfReset) / ONESEC;
  return result;
};

boolean cip_daughterBoardStatus (cip_idb_infotype *cip)
{
   /* This returns whether or not the code has been loaded on the daughter 
      board and running */
   return (cip && ((cip->db_flags & DB_LOADED) == DB_LOADED)) ? TRUE : FALSE;
}

boolean cip_daughterBoardSignal (cip_idb_infotype * cip)
{
  /* This returns whether or not signal has been seen on the daughter board */
  return (cip && ((cip->pacb_flags & PA_SIGNAL) == PA_SIGNAL)) ? TRUE : FALSE;
}

boolean cip_daughterBoardOffline (cip_idb_infotype * cip)
{
  /* This returns whether or not the board is off line */
  return (!cip || ((cip->db_flags & DB_OFFLINE) == DB_OFFLINE) ||
                  ((cip->db_flags & DB_VALID) != DB_VALID)) ? TRUE : FALSE;
}

void
get_cip_version (int index, cip_version_t *cip_version_p) {
	
    ushort hw_version;
    ushort sw_version;
    
    if (cip_version_info(index, &hw_version, &sw_version)) {
	cip_version_p->hw_version_major = hw_version >> 8;
	cip_version_p->hw_version_minor = hw_version & 0xFF;
	cip_version_p->sw_version_major = sw_version >> 8;
	cip_version_p->sw_version_minor = sw_version & 0xFF;
    }
}

static uchar link_incident_cause_convert (uchar channel_cause) {
    switch(channel_cause){
        case CHANNEL_IMPL_INCIDENT:
            return D_linkIncidentTrapCause_liImplicitIncidents; 
        case  CHANNEL_BER_EXCEEDED: 
            return D_linkIncidentTrapCause_liBERthreshold;
        case  CHANNEL_LINK_FAILED:   
            return D_linkIncidentTrapCause_liSignalOrSyncLoss;
        case  CHANNEL_NOS:         
            return D_linkIncidentTrapCause_liNotOperationalSequence; 
        case  CHANNEL_SEQ_TO:     
            return D_linkIncidentTrapCause_liSequenceTimeouts;
        case  CHANNEL_INVALID_SEQ:   
            return D_linkIncidentTrapCause_liInvalidSequences;
        default:
            return  D_linkIncidentTrapCause_liOther;
    }
}

/*
 * setup_link_event - 
 */
static boolean setup_link_event (cip_linktype *ciplink, chan_link_event_t *clf_ev, ushort slot) {
    ciplink->slot               = slot;
    ciplink->which              = clf_ev->which;
    ciplink->dtr_brd_status     = ((clf_ev->dbcb_flags & DB_LOADED) ?  
    D_cipCardDtrBrdStatus_true : D_cipCardDtrBrdStatus_false);
    ciplink->dtr_brd_signal     = ((clf_ev->pacb_flags & PA_SIGNAL) ?
    D_cipCardDtrBrdSignal_true : D_cipCardDtrBrdSignal_false);
    ciplink->link_incident_cause= link_incident_cause_convert(clf_ev->link_incident_cause);
    return TRUE;
}
/*
 * chan_netmgt_event_handler - registered function that is called
 *                             whenever a channel  event is recevied
 *                             from the CIP.
 */
static void chan_netmgt_event_handler (ushort slot, ushort code, 
                                ushort length, void *ev_data ) {

    cip_linktype cip_link;
    chan_link_event_t *cc_link;
    cc_link = (chan_link_event_t *) ev_data;
    switch (code){
        case CARD_LINK_FAILURE_EVENT:
         /* add a boolean function to setup this data struct */
            if (!setup_link_event( &cip_link, cc_link, slot))
                return;
            break;
        default:
            buginf ("\nInvalid event code (channel class trap)"); 
            break;    
    } 
  reg_invoke_cip_cipchan_trap(code, &cip_link);    
}
  
/* **************  End of SNMP related routines *************** */

/*
 * CIP subsystem header
 */

#define CIP_MAJVERSION   1
#define CIP_MINVERSION   0
#define CIP_EDITVERSION  1

/*---------------------------------------------------------------------------
 * Subsystem Header for CIP Driver
 *---------------------------------------------------------------------------*/
SUBSYS_HEADER(cip,                              /* name */
	      CIP_MAJVERSION,			/* major version */ 
	      CIP_MINVERSION,			/* minor version */ 
	      CIP_EDITVERSION,			/* edit version */
	      cip_init, 	 		/* init function */
	      SUBSYS_CLASS_DRIVER,	 	/* subsystem class */
	      "seq: dbus, cbus_mci",    	/* sequence list */
	      "req: dbus, cbus_mci"   	        /* prerequisite list */
	      );
      
