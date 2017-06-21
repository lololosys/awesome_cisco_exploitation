/* $Id: vtp_dep.c,v 1.2.58.12 1996/08/21 20:37:42 hampton Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_dep.c,v $
 *------------------------------------------------------------------
 * vtp_dep.c - VTP IOS dependent module.           
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_dep.c,v $
 * Revision 1.2.58.12  1996/08/21  20:37:42  hampton
 * Further isolate VLAN code from IPX internals.  [CSCdi66726]
 * Branch: California_branch
 *
 * Revision 1.2.58.11  1996/08/13  02:26:21  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 1.2.58.10  1996/06/29  01:56:14  cyoung
 * Don't write to a free'd memory block (duh).  Make the stack for the VTP
 * process a more reasonable size.
 * CSCdi61006:  Systems crash, 7k & 4500 simultaneously, while deleting
 * vlan.
 * Branch: California_branch
 *
 * Revision 1.2.58.9  1996/06/28  23:31:51  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.2.58.8  1996/06/21  20:39:31  cyoung
 * Create subinterfaces only for Ethernet and TR type VLANs on LANE trunk
 * CSCdi60973:  extra sub interfaces got created with [vtp trunk domain
 * xxx]
 * Branch: California_branch
 *
 * Revision 1.2.58.7  1996/06/21  18:00:03  cyoung
 * Must clear out old client before creating a new one.
 * CSCdi60867:  lane client ethernet name does not change with vlan name
 * Branch: California_branch
 *
 * Revision 1.2.58.6  1996/06/20  19:50:23  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.2.58.5  1996/06/18  00:51:04  cyoung
 * CSCdi60069:  router crashes when writing vtp info to tftp file
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * CSCdi59933:  RSP crashes for creating-deleting-recreating vtp-domains
 * CSCdi60044:  7000 image crash
 * CSCdi60394:  [sh vtp trunk] crashes 7000
 * CSCdi60528:  rsp can't change vtp database until someone else does
 * CSCdi60541:  Attempt to delete a vlan crashes image
 * CSCdi60632:  cannot set password for vtp management-domain
 * CSCdi60384:  configuration timestamp is not displayed correctly
 * Plus some Cat5k bug fixes rolled in as well as dependant code clean up.
 * Branch: California_branch
 *
 * Revision 1.2.58.4  1996/05/21  22:29:27  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.3  1996/05/21  21:39:25  mordock
 * CSCdi58327:  snmp modularity commit broke some builds
 * Branch: California_branch
 * need to use new ifmib registry
 *
 * Revision 1.2.58.2  1996/05/21  10:08:56  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.2.58.1  1996/05/09  14:48:31  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.8  1996/04/18  21:26:51  percyk
 * added consistency checks for translational vlans via snmp.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.7  1996/04/17  21:57:00  cyoung
 * CSCdi53840:  error message should pop up once the error occurs
 * Use errmsg to provide timely VTP error logging.
 *
 * CORE - retrieve domain/vlan functions modify input parameters instead
 *        of affecting global structure.
 *        Convert macros to functions (change_to_client,
 *        change_to_transparent).
 *        domain/vlan notifications now pass before and after data so that
 *        transistion actions can be done.
 *        rip out duplicate add/changes from the verify/validate
 *        routines.
 *        vtp_vlan_change_notification is called without checking for
 *        changes.
 *        do not change to client as a result of failed vlan change
 *        notification of domain revision number change notification.
 *  DEP - support new function declarations, fix bug where trying to
 *        configure swidb when no more can be created.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.6  1996/04/16  18:58:59  cyoung
 * Using 'exit','end' or '^Z' as an implicit apply breaks when the parser
 * needs to pop out of the sub-command scope to check against the global
 * parse chain.  Make 'apply' an explicit command to avoid this blunder.
 * CSCdi54003:  write to tftp file when it shouldnt, doesnt when it should
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.5  1996/04/11  20:50:45  cyoung
 * CSCdi53657:  missing TR default VLAN 1003
 * CSCdi53488:  4500 crashes when configed with a wrong vtp config file
 * MIB - Fix bug with SR VLAN info not showing up
 * CORE - Add syntax checking for domain info in vtp_download_info and
 *        vtp_add_domain, add syntax checking for VLANs in
 *        vtp_download_info
 * DEP - Make TFTP loading silent, zero length loaded file still okay but
 *       runt TFTP file now flagged as error, prevent loading of database
 *       from a revision number change
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.4  1996/04/10  01:41:30  cyoung
 * Do not process any VTP IPC messages until config has been read in.
 * Trying to load TFTP file before the interfaces have been configured
 * IP addresses is problematic.
 * CSCdi53479:  7000 does not load vtp config file at reboot
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.3  1996/04/09  17:09:45  percyk
 * vlan config consistency between snmp and cli.
 * advt interval and passwd needs to be accepted from download data.
 * cannot change vlan name length once it's set.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.2  1996/04/09  04:24:02  cyoung
 * Report TFTP load failure back to VTP process.
 * CSCdi53758:  vtp server does not turn into client when fail to read
 * tftp file
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.10  1996/04/06  03:53:40  cyoung
 * Add trunk creation/deletion capability to VTP MIB code.
 * Beautify some code formatting.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.9  1996/04/06  02:07:05  cyoung
 * CSCdi53693:  vlan is not built when it should be
 * don't abort the vtp-database command if nv_valid false
 * And the following fixes while we're at it:
 *  don't load database when writing into it
 *  set the revision number to zero when loading from NVRAM (to avoid
 *   conflict with TFTP).
 *  use vtp_rev_less in vtp_download_info
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.8  1996/04/05  22:56:15  cyoung
 * CSCdi53688:  bridge group not removed when it should be
 * While the bonnet is up, fix another potential memory leak
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.7  1996/04/05  19:07:31  cyoung
 * CSCdi53470:  wrong behaviour while deleting vtp-created vlans
 * CSCdi53667:  Spurious memory access found on 4500
 * Also fix possible memory leak on LEC configuration failure
 *
 * core - set the vlan type to pass checks in vtp_delete_two_tlb_entries
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.6  1996/04/04  18:24:36  percyk
 * more error codes when configuring vlans.
 * more error checking for translational vlans.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.5  1996/04/03  22:36:58  cyoung
 * New objects for TFTP support, ETR vlan type and trunk table creation
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MIB - TFTP support, ETR type allowed, still need work on Trunk creation
 * dep - use temp buf to determine add/mod mode instead of real database
 * core - add tooBig hook for trap generation
 *
 * Revision 1.2.64.4  1996/03/30  04:14:16  cyoung
 * vtp-domain and vtp-database modes exit cleanly with ^Z or end
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.3  1996/03/27  23:41:32  cyoung
 * MIB - implement new trap formats (correct varbinds)
 * core - vtp_delete_domain crashes with trunk defined
 * depend - TFTP read needs its own process, implement VLAN state
 *          modifications, some cleanup
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.2  1996/03/25  02:21:49  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:45:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.12  1996/03/23  02:20:58  cyoung
 * Quick fix for database exit command.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.11  1996/03/22  23:11:03  cyoung
 * Quick fix for empty tftp file.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.10  1996/03/22  16:22:18  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.9  1996/03/19  19:10:00  percyk
 * remove unused function
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.8  1996/03/19  00:21:20  percyk
 * vtp support on isl links.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.6  1996/03/13  17:27:59  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.5  1996/03/08  18:19:00  cyoung
 * Remove 'vtp network' command.
 * Add tftp server and file name parameters to 'server' keyword
 *  and support.
 * Check managementDomainIndex range in MIB code.
 * Change to client on reception of any error (not just NVRAM error).
 * Set the updater when configured from the CLI.
 * Set the appropriate state of a VLAN in vtp_edit_vlan
 * Remove unneeded function declarations in vtp_dep.h
 * Fix memory corruption in vtp_upload_info.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.4  1996/03/07  02:35:10  cyoung
 * Added vtp network command.
 * Cleanup database subcommand looping.
 * Don't require ring keyword for fddi type vlans.
 * Choose better values for default VLAN fields.
 * Assign domain index before inserting a new domain into the list.
 * Assign correct state when going through vtp_edit_vlan.
 * Add support for removing configuration as a result of VLAN deletion.
 * Rename spanning tree defines to match the MIB.
 * nv_gen support for all VTP commands.
 * set the parent_valid field correctly.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.3  1996/03/05  01:55:54  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.2  1996/03/04  23:48:43  percyk
 * define new platform dependent routine on rcving no response to req
 * packet.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:27  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.17  1996/02/26  18:07:29  percyk
 * more debug messages.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.16  1996/02/23  23:05:50  cyoung
 * Independant code put in for checking native vlan range.
 * Do not increment the domain index when adding the 'null domain'.
 * remove duplicate function declarations from vtp_interface.h
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.15  1996/02/22  04:42:54  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.14  1996/02/22  03:10:00  cyoung
 * Move include files around to make code more easily ported to Cat5000.
 * TruthValue false is 2 and not 0.
 * Roll function declarations from vtp_agent into vtp_interface and
 * vtp_externs (cut down on number of include files).
 * Add IOS_SPECIFIC compile flag to mark non-portable code.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.13  1996/02/21  04:12:03  cyoung
 * bridge numbers start at 1. Fix spurious memory in the vtp_receive_xxx
 * functions.  VTP multicast address added to SMF.  Make sure the firstsw
 * is passed into the the vtp_receive_packet function instead of the
 * subinterface.  Set revision number to zero if modification is made
 * to vtp database when in other than server mode
 *
 * VTP now works over LANE.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.12  1996/02/16  19:32:15  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup, add more vtp debugging.
 *
 * Revision 1.2.4.11  1996/02/15  02:30:32  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * move platform dependent stuff into interface files.
 *
 * Revision 1.2.4.10  1996/02/13  04:03:07  cyoung
 * move notification_enabled to vtp_dep.c, minor bug fixes, add code to
 * check for native vlan range and existance of trunk before allowing sets.
 * Also some code shuffle as a result of resolving conflicts.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.9  1996/02/12  21:32:35  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.2.4.8  1996/02/09  01:10:01  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.2.4.7  1996/02/08  04:12:33  cyoung
 * Set default time interval when creating or modifying management domain.
 * Add support for setting vlan filter on trunks and the native vlan.
 * Add support for transmitting adverts over LANE.
 * Add code for removing trunks from idblist.
 * Add interface for core code to notify dependant code when a mod has
 *  been made to the trunk list.
 * More code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.6  1996/02/01  19:17:35  cyoung
 * Start work on LANE encaps for VTP adverts.  Reset timer to zero when
 * freed.  Fix 'imaginative' cast for SAID value.  More basic cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.5  1996/01/31  02:34:03  cyoung
 * Provide function to allow variance in managementDomainEntry row
 * creation.
 * Remove redundant consistency checking.  Add hooks to correctly report
 * result of 'apply' operation.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.4  1996/01/31  00:44:05  cyoung
 * MIB fixes and beginning code for sub-interface creation
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.3  1996/01/24  01:37:54  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.2  1996/01/23  00:47:18  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.2.4.1  1996/01/11  01:47:50  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.5  1996/01/06  00:37:41  dcheng
 * Update the VTP packet format to include ring number and parent VLAN for
 * FDDI/TR VLAN, bridge number and STP type for FDDI/TR net VLAN, and
 * translational bridged VLAN id (0, 1 or 2). Update the database and
 * user interface for these as well.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.4  1996/01/04  00:38:07  dcheng
 * Delete the boolean flag "vtp_enabled" in the idb structure, clean up the
 * related code. Fix some bugs in the CLI for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.3  1996/01/03  01:49:21  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.2  1995/12/23  03:51:27  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  01:48:49  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  01:47:25  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "mgd_timers.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface.h"
#include "interface_private.h"
#include "ieee.h"
#include "packet.h"
#include "logger.h"
#include "address.h"
#include "file.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "../if/rif.h"
#include "../if/ether.h"
#include "../smf/smf_public.h"
#include "config.h"
#include "parser.h"
#include "name.h"
#include "../os/signal.h"
#include "../os/clock.h"
#include "../util/random.h"
#include "../wan/serial.h"
#include "../if/static_map.h"
#include "../util/md5.h"
#include "../snmp/snmp_api.h"
#include "vtp_externs.h"
#include "vtp_interface.h"
#include "vtp_debug.h"
#include "vtp_pub.h"
#include "../lane/lane_registry.h"
#include "../lane/lane.h"
#include "../srt/srt_registry.h"
#include "../srt/span.h"
#include "../vlan/vlan.h"
#include "../tbridge/tbridge_sde.h" /* said_table */
#include "vtp_dep.h"
#include "vtp_registry.h" /* trap functions */
#include "../snmp/ifmib_registry.h" /* ifmib_snmpidb_get */



#define LANE_TYPE_802_3 0x01 /* from lane_private.h */
#define LANE_TYPE_802_5 0x02

pid_t vtp_process_id;
pid_t vtp_tftp_id;

/* Global variables */

boolean vtp_running;                     /* TRUE if VTP running */
boolean vtp_explicitly_disarmed;         /* TRUE if VTP was explicitly
					    off */

watched_queue *vtp_packet_q;

unsigned char vtp_multicast[]    = { 0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc };  
unsigned char vtp_tr_func_addr[] = { 0xC0, 0x00, 0x08, 0x00, 0x00, 0x00 };

long vtp_notification_enabled;            /* TRUE if enabled */
vtp_timer vtp_root_timer;

static char vtp_default_elan_name[7] = { 'd', 'e', 'f', 'a', 'u', 'l', 't' };

#define VTP_DEFAULT_ELAN_NAME_LENGTH 7


/* forward declarations (don't want to include monster sr_vtpmib.h) */
static idbtype* vtp_add_vlan_to_trunk(int bg, vtp_domain_info* d_info,
				      vtp_vlan_info* v_info,
				      vtp_trunk_info* t_info);
static int vtp_get_bridge_group(vtp_domain_info* d_info,
				vtp_vlan_info* v_info);
void vtp_macaddrs_update(idbtype *idb);
static idbtype* vtp_get_subif(vtp_trunk_info* t_info, vtp_vlan_info*
			      v_info); 
static idbtype* vtp_configure_subif(boolean create, 
				    vtp_trunk_info* t_info,
				    vtp_vlan_info* v_info);
unsigned long vtp_dep_get_local_ipaddr(void);
void vtp_dep_add_trap_functions(void);
long vtp_dep_get_notifications_enabled(void);
boolean vtp_post_message(vtp_ipc_message *pointer);

/* VTP subsystem header */
#define VTP_MAJVERSION   1
#define VTP_MINVERSION   0
#define VTP_EDITVERSION  1


static void vtp_tftp_report_failure(unsigned char* domain_name, 
				    unsigned char dname_length) {
    vtp_ipc_tftp_load_failure *failure;

    failure = malloc(sizeof(vtp_ipc_tftp_load_failure));
    if (!failure)
	return;

    failure->message = VTP_TFTP_LOAD_FAILURE;
    memcpy(failure->domain_name, domain_name, dname_length);
    failure->dname_length = dname_length;
		
    vtp_post_message((vtp_ipc_message*)failure);
}

void vtp_tftp_main (void) {
    vtp_domain_info d_info;
    unsigned char* buf;
    ulong size;
    unsigned long message;
    void *major;  
    vtp_ipc_load_tftp *pointer;
    unsigned long count;
    filetype file; 
    unsigned int error;
    
    while (TRUE) {
	
	process_wait_for_event();
	process_get_message(&message, &major, &count);

        pointer = (vtp_ipc_load_tftp *)major;

	d_info.domain_name_length = pointer->dname_length;
	memcpy(d_info.domain_name, pointer->domain_name,
	       pointer->dname_length); 
	if (vtp_get_domain_info(&d_info, VTP_EXACT)) {
	    vtp_tftp_report_failure(pointer->domain_name,
				    pointer->dname_length);
	    continue;
	}

	/* Don't be alarmed (much).  The download function actually
	   frees the buffer. */
	
	size = (VTP_MAX_VLANS * sizeof(vtp_vlan_info)) +
	    sizeof(vtp_domain_info); 
	buf = malloc(size);
	if (!buf) {
	    vtp_tftp_report_failure(pointer->domain_name,
				    pointer->dname_length);
	    continue;
	}
		    
	file.filename = d_info.tftp_file_name;
	file.ps.tuba.boothost.ip_addr = d_info.tftp_server;
	file.proto = FILE_ACCESS_TFTP;
	file.resetsubr = NULL;
	file.ps.tuba.boothost.type = ADDR_IP;
	file.ps.tuba.boothost.length = ADDRLEN_IP;
	file.ps.tuba.username = NULL;
	file.ps.tuba.display = FALSE;
	
	error = reg_invoke_file_read(file.proto, &file, buf, &size, FALSE,
				     NULL, NULL);
	    
	if (error == ERROR_OK) {
	    /* zero length file OK.  Used for new domains */
	    if (size == 0) {
		free(buf);
		goto tftp_discard;
	    }

	    /* check for runt file */
	    if (size < ((VTP_MIN_VLANS * sizeof(vtp_vlan_info)) +
			sizeof(vtp_domain_info))) { 
		vtp_tftp_report_failure(pointer->domain_name,
					pointer->dname_length);
		free(buf);
		goto tftp_discard;
	    }
	    vtp_post_download_config(d_info.domain_name,
				     d_info.domain_name_length, FALSE, FALSE,
				     buf);
	} else {
	    vtp_tftp_report_failure(pointer->domain_name,
				    pointer->dname_length);
	    free(buf);
	}

    tftp_discard:
	free(pointer);
	
    }

}

unsigned char vtp_load_database (vtp_domain_info* d_info_p) {
    vtp_ipc_load_tftp *pointer;
    unsigned long id;

    pointer = malloc( sizeof(vtp_ipc_load_tftp) );

    if (!pointer)
       return VTP_GENERAL_ERR;

    pointer->message = VTP_LOAD_TFTP;

    memcpy(pointer->domain_name, d_info_p->domain_name,
	   d_info_p->domain_name_length); 
    pointer->dname_length = d_info_p->domain_name_length;

    id = (unsigned long)(pointer->message);
    process_send_message(vtp_tftp_id, id, (void *)pointer, 0);
    return VTP_NOTIFICATION_OK;
}



/* clean up any droppings left over after a 'no int' command */
void vtp_swif_erase (idbtype* swidb) 
{
    vtp_subif_subidb* subif_sb;

    subif_sb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);
    if (!subif_sb)
	return;

    if (!idb_delete_swsb(swidb, SWIDB_SB_VTP_SUB))
	return;
    free(subif_sb);
}


void vtp_swif_going_down (idbtype* swidb) 
{
    vtp_trunk_info t_info;

    t_info.trunk_id = swidb;
    if (vtp_get_trunk_info(&t_info, VTP_EXACT) != VTP_OK)
	return;

    vtp_core_change_trunk_link_state(t_info.dname_length,
				     t_info.domain_name,
				     t_info.trunk_id,
				     TRUNK_LINK_DOWN);
}

void vtp_swif_coming_up (idbtype* swidb) 
{
    vtp_trunk_info t_info;

    t_info.trunk_id = swidb;
    if (vtp_get_trunk_info(&t_info, VTP_EXACT) != VTP_OK)
	return;

    vtp_post_change_trunk_link_state(t_info.dname_length,
				     t_info.domain_name,
				     t_info.trunk_id,
				     TRUNK_LINK_UP);
}

void vtp_srt_port_state_change (idbtype* port, int status) {
    vtp_trunk_info t_info;
    unsigned short vlan_id;
    vtp_vlan_info v_info;

    if (port->hwptr)
	t_info.trunk_id = port->hwptr->firstsw;
    
    if (vtp_get_trunk_info(&t_info, VTP_EXACT) != VTP_OK)
	return;

    vlan_id = reg_invoke_vtp_find_vlan_by_bridge_group(port->span_index);
    if (!vlan_id)
	return;

    v_info.isl_vlan_id = vlan_id;
    if (vtp_get_vlan_info(t_info.dname_length, t_info.domain_name,
			  &v_info, VTP_EXACT) != VTP_OK)
	return;

    vtp_post_change_trunk_vlan_stp_state(t_info.dname_length,
					 t_info.domain_name, vlan_id,
					 t_info.trunk_id, status ==
					 PORT_FORWARDING ?
					 VLAN_SPT_STATE_FWD :
					 VLAN_SPT_STATE_NON_FWD); 
}

static char bridge_group_ref[SPAN_MAXINDEX];

void vtp_srt_add_local_port (idbtype* port, int bg) {
    unsigned short vlan_id;
    vtp_domain_info d_info;

    /* If this is a sub-interface it does not count as a local port */
    if ((port->hwptr) && (port->hwptr->firstsw != port))
	return;

    bridge_group_ref[bg]++;

    if (!reg_invoke_vtp_find_domain_by_bridge_group(bg,
						    d_info.domain_name, 
						    &d_info.domain_name_length))
	return;

    vlan_id = reg_invoke_vtp_find_vlan_by_bridge_group(bg);
    if (!vlan_id)
	return;

    /* If this port starts interest in an otherwise unused VLAN, add
     * it to the 'local ports' list.  If it is just an additional
     * instance of a bridge group already tracked then no need to
     * notify the core code again.
     */
    if (bridge_group_ref[bg] == 1) {
	vtp_post_change_vlan_local_assoc(d_info.domain_name_length, 
					 d_info.domain_name, vlan_id,
					 VLAN_LOCAL_ASSOC); 
    }
}

void vtp_srt_remove_local_port (idbtype* port, int bg) {
    unsigned short vlan_id;
    vtp_domain_info d_info;

    /* If this is a sub-interface it does not count as a local port */
    if ((port->hwptr) && (port->hwptr->firstsw != port))
	return;

    bridge_group_ref[bg]--;

    if (!reg_invoke_vtp_find_domain_by_bridge_group(bg,
						    d_info.domain_name, 
						    &d_info.domain_name_length))
	return;

    vlan_id = reg_invoke_vtp_find_vlan_by_bridge_group(bg);
    if (!vlan_id)
	return;

    /* If there are no tracked ports in the bridge group then remove it from 
     * the local association list in the core code.
     */
    if (bridge_group_ref[bg] <= 0) {
	vtp_post_change_vlan_local_assoc(d_info.domain_name_length, 
					 d_info.domain_name, vlan_id,
					 VLAN_LOCAL_ASSOC_NONE); 
	bridge_group_ref[bg] = 0;
    }
}

boolean vtp_dep_is_pruned (void* subif_sb) {

  vtp_subif_subidb* subif_ptr;

  subif_ptr = subif_sb;
  if ((subif_ptr) && (subif_ptr->vtp_enabled) &&
      (subif_ptr->is_pruned)) 
    return TRUE;

  return FALSE;
}

/*
 * Initialize all vtp related stuff.
 */
void vtp_sys_init (subsystype *subsys)
{
    
    vtp_process_id = 0;
    vtp_packet_q = NULL;
    vtp_media_registry();
    vtp_debug_init(); 
    vtp_parser_init();
    memset(bridge_group_ref, 0, SPAN_MAXINDEX);
    
    /* 
     * This one must go direct as the message queue hasn't been set up
     * yet.  This, however does prevent there from being any conflicts 
     */
    
    vtp_explicitly_disarmed = FALSE;
    vtp_notification_enabled = 2; /* false for TruthValue in SNMP */
    
    reg_add_swif_erase(vtp_swif_erase, "vtp_swif_erase");
    reg_add_swif_goingdown(vtp_swif_going_down,
			    "vtp_swif_going_down");
    reg_add_swif_comingup(vtp_swif_coming_up, "vtp_swif_coming_up");
    reg_add_srt_report_port_state(vtp_srt_port_state_change,
				  "vtp_srt_state_change");
    reg_add_srt_report_new_port(vtp_srt_add_local_port,
				"vtp_srt_add_local_port");
    reg_add_srt_report_port_removal(vtp_srt_remove_local_port,
				    "vtp_srt_remove_local_port"); 
    reg_add_vtp_port_is_pruned(vtp_dep_is_pruned, "vtp_dep_is_pruned");
}



/*
 * The VTP main routine.
 */
void vtp_process_main (void)
{
    
   unsigned long major, minor;
 

   /* initialize VTP system resourses */
   if (!system_loading)
      process_watch_mgd_timer(&(vtp_root_timer.the_timer), ENABLE);

   vtp_packet_q = create_watched_queue("VTP packets", 0, 0);

   process_watch_queue(vtp_packet_q, ENABLE, RECURRING);

   reg_add_raw_enqueue(LINK_VTP, vtp_enqueue, "vtp_queue");
 
   /* initialize vtp private data structures */
   vtp_init();

   /* don't process CLI commands from NVRAM until add interfaces 
      have been processed, otherwise trunks can't be created and
      the server cannot load the TFTP file */

   process_wait_on_system_init();
 
   while (vtp_running) {

      process_wait_for_event();

      while (process_get_wakeup(&major, &minor)) {
         switch (major) {

            case QUEUE_EVENT:
               vtp_process_packet_queue();
               break;

            case TIMER_EVENT:
               vtp_process_timer();
               break;
 
            case MESSAGE_EVENT:
               vtp_process_message();
               break;

            default:
               break;

         }
      }
   }
 
   /* shut down the process */
   vtp_process_teardown();
}

 
 
/*
 * vtp process initialization function.
 */
void vtp_start_process (void)
{
    if (!vtp_process_id)  {
       vtp_root_timer_init();
       vtp_process_id = process_create(vtp_process_main, "VTP Process", 
				       LARGE_STACK,
				       PRIO_NORMAL);
       vtp_tftp_id = process_create(vtp_tftp_main, "VTP TFTP Process", 
				    NORMAL_STACK, PRIO_NORMAL); 
       vtp_running = TRUE;
       vtp_explicitly_disarmed = FALSE;
    }
}



/*
 * Re-schedule vtp process for fair sharing of cpu.
 */
void vtp_process_reschedule (void)
{

    process_may_suspend();

}



/*
 * vtp shutdown routine.
 */
void vtp_process_teardown (void)
{

    paktype *pak;
 

    /* release system resouces */
    reg_delete_raw_enqueue(LINK_VTP);

    process_watch_queue(vtp_packet_q, DISABLE, RECURRING);

    while ( (pak = process_dequeue(vtp_packet_q) ) != NULL )
       retbuffer(pak);

    delete_watched_queue(&vtp_packet_q);

    process_watch_mgd_timer(&(vtp_root_timer.the_timer), DISABLE);
 
    /* un-register notification functions */
    reg_invoke_bridge_entry_global_remove(vtp_multicast);
 
    /* clean up the private resouces */
    vtp_terminate();
 
    /* kill the process */
    vtp_process_id = 0;
   process_kill(THIS_PROCESS);

}

 

/************************************************************************
 *
 * Data Buffer Service.
 *
 ************************************************************************/
/*
 * Function to allocate a data buffer.               
 */
void* vtp_get_buffer (unsigned long size)
{
   return(getbuffer(size));
}



/*
 * Function to duplicate a data buffer.               
 */
void* vtp_buffer_duplicate (void *buff_p) {
   paktype *pak;

   pak = pak_duplicate(buff_p);
   return pak;
}



/*
 * Function to free a data buffer.               
 */
void vtp_free_buffer (void *buff_p)
{
   datagram_done(buff_p);
}



/*
 * Function to locate the first byte of VTP packet.  
 */
void* vtp_buffer_payload (void *trunk_id, void *buff_p)
{
    return ((paktype *)buff_p)->network_start;
}


/* printable character checking */

boolean vtp_is_print (unsigned char* string, unsigned char len) {
    uchar t;

    t = 0;
    while (t < len) {
	if (!isprint(string[t])) 
	    return FALSE;
	t++;
    }
    return TRUE;
}

/*
 * Timer Service
 */

void vtp_root_timer_init (void)
{
    vtp_root_timer.next = &vtp_root_timer; 
    vtp_root_timer.prev = &vtp_root_timer; 
    mgd_timer_init_parent(&(vtp_root_timer.the_timer), NULL);
}



/*
 * Function to search a timer in a sorted timer list.
 */
vtp_timer *vtp_search_timer (mgd_timer *mgd_timer_p)
{
    vtp_timer *timer_p;

    for (timer_p = vtp_root_timer.next; 
         timer_p != &vtp_root_timer;
         timer_p = timer_p->next) {

        if (mgd_timer_p == &(timer_p->the_timer))
            return(timer_p);
        else
        if (mgd_timer_p < &(timer_p->the_timer))
            break;
    }
    return(NULL);
}



/*
 * Function to create a timer.
 */
void* vtp_create_timer (void (*exp_func), void *context) {
    vtp_timer *timer_p;
    vtp_timer *current_timer_p;

    if ((timer_p = (vtp_timer *)(malloc(sizeof(vtp_timer)))) != NULL) {

        /* link the new entry to the timer list, sorted in ascending
           order of the address of mgd_timer */
        for (current_timer_p = vtp_root_timer.next; 
             current_timer_p != &vtp_root_timer;
             current_timer_p = current_timer_p->next) {

            if (&(timer_p->the_timer) < &(current_timer_p->the_timer))
                break;   
        }
        (current_timer_p->prev)->next = timer_p;
        timer_p->next = current_timer_p;
        timer_p->prev = current_timer_p->prev;
        current_timer_p->prev = timer_p;

        timer_p->exp_func = exp_func;
        timer_p->context = context;

        mgd_timer_init_leaf(&(timer_p->the_timer),
			    &(vtp_root_timer.the_timer), 0/*type*/,
			    context, FALSE); 
    }
    return timer_p;
}



/*
 * Function to delete a timer.
 */
void vtp_delete_timer (void *timer) 
{
    vtp_timer *timer_p = (vtp_timer *)timer;

    (timer_p->next)->prev = timer_p->prev; 
    (timer_p->prev)->next = timer_p->next; 
    /* Do we need and how to cancel the mgd timer ? */
    free(timer);
}



/*
 * Function to start a timer.
 */
void vtp_start_timer (void *timer_p, unsigned long value) 
{
    mgd_timer_start(&(((vtp_timer *)timer_p)->the_timer), value);
}



/*
 * Function to stop a timer.
 */
void vtp_stop_timer (void *timer_p) 
{
    mgd_timer_stop(&(((vtp_timer *)timer_p)->the_timer));
}



/*
 * Function to test the status of a timer.
 */
boolean vtp_timer_active (void *timer_p) {
    return(mgd_timer_running(&(((vtp_timer *)timer_p)->the_timer)));
}


unsigned long vtp_timer_time_left (void* timer_p) {
    return(mgd_timer_left_sleeping(&(((vtp_timer*)timer_p)->the_timer)));
}

/*
 * Function to process expired timers.
 */
void vtp_process_timer (void)
{
    vtp_timer *timer_p;
    mgd_timer *expired_timer;
    while (mgd_timer_expired(&(vtp_root_timer.the_timer)))  {
         expired_timer = mgd_timer_first_expired(&(vtp_root_timer.the_timer));

         /* search the timer chain for  */
        if ((timer_p = vtp_search_timer(expired_timer)) != NULL) {
            (*timer_p->exp_func)(timer_p->context);
        }
    }
}



/*
 * Function to get system up time in milliseconds.
 */
unsigned long vtp_get_system_uptime (void)
{
    return(snmp_sysUpTime());
}



/*
 * Function to get date/time timestamp.                       
 */
void vtp_get_timestamp (unsigned char *timestamp)
{
    clock_epoch ce;
    timeval tv;
    unsigned char *byte = timestamp;
 
    clock_get_time(&ce);
    clock_epoch_to_timeval(&ce, &tv, 0); 

    *byte++ = (tv.year % 100) / 10 + '0';
    *byte++ = (tv.year % 10) + '0';
    *byte++ = (tv.month / 10) + '0';
    *byte++ = (tv.month % 10) + '0';
    *byte++ = (tv.day / 10) + '0';
    *byte++ = (tv.day % 10) + '0';
    *byte++ = (tv.hour / 10) + '0';
    *byte++ = (tv.hour % 10) + '0';
    *byte++ = (tv.minute / 10) + '0';
    *byte++ = (tv.minute % 10) + '0';
    *byte++ = (tv.second / 10) + '0';
    *byte++ = (tv.second % 10) + '0';
};

 
/*
 * This function posts a message to the VTP process.         
 */
boolean vtp_post_message (vtp_ipc_message *pointer)
{
    unsigned long id;
  
    if (vtp_running) {
        id = (unsigned long)(pointer->message);
        if (process_send_message(vtp_process_id, id, (void *)pointer, 0) == FALSE) {
	    free(pointer);
	    return FALSE;
	}
    } else {
        free(pointer);
	return FALSE;
    }

    return TRUE;
}


 
/*
 * This function posts a add/modify domain message to the VTP process.
 */
void vtp_post_configure_domain (unsigned char mode, vtp_domain_info*
				d_info_p) {

    vtp_ipc_config_domain *pointer;

    pointer = malloc( sizeof(vtp_ipc_config_domain) );

    if (!pointer)
       return;

    pointer->message = VTP_CONFIGURE_DOMAIN;
    pointer->operation = mode;

    memcpy(&pointer->d_info, d_info_p, sizeof(vtp_domain_info));
    vtp_post_message( (vtp_ipc_message *)pointer );

}

void vtp_post_change_vlan_local_assoc(unsigned char dname_length, 
				      unsigned char* domain_name, 
				      unsigned short vlan_id, 
				      unsigned char flag) {
    vtp_ipc_prune_local *ptr;

    ptr = malloc(sizeof(vtp_ipc_prune_local));
    if (!ptr)
	return;

    ptr->message = VTP_PRUNING_LOCAL;
    ptr->dname_length = dname_length;
    memcpy(ptr->domain_name, domain_name, dname_length);
    ptr->vlan_id = vlan_id;
    ptr->flag = flag;
    vtp_post_message((vtp_ipc_message*)ptr);
}

void vtp_post_change_domain_name (unsigned char  old_length, 
				  unsigned char* old_name, 
				  unsigned char  new_length,
				  unsigned char* new_name) {
    vtp_ipc_change_domain_name *pointer;

    pointer = malloc(sizeof(vtp_ipc_change_domain_name));
    if (!pointer)
	return;

    pointer->message = VTP_CHANGE_DOMAIN_NAME;
    pointer->old_name_length = old_length;
    pointer->new_name_length = new_length;
    memcpy(pointer->new_name, new_name, new_length);
    memcpy(pointer->old_name, old_name, old_length);
    vtp_post_message((vtp_ipc_message *)pointer);
}

/*
 * Enable VTP on interface. If VTP process is not running and VTP has
 * not been explicitly turned off (by the command 'no vtp run'), start
 * the process.
 */
unsigned char vtp_intf_enable (vtp_trunk_info* t_info) {
    vtp_domain_info d_info;
    vtp_vlan_info v_info;
    idbtype* swidb;
    int bg;
    unsigned char error;

    swidb = NULL;

    /* create default vlan on trunk */
    d_info.domain_name_length = t_info->dname_length;
    memcpy(d_info.domain_name, t_info->domain_name,
	   t_info->dname_length);

    for (error = vtp_get_vlan_info(t_info->dname_length,
				   t_info->domain_name, &v_info,
				   VTP_FIRST); !error; error =
	 vtp_get_vlan_info(t_info->dname_length, t_info->domain_name,
			   &v_info, VTP_NEXT)) {
	bg = vtp_get_bridge_group(&d_info, &v_info);
	swidb = vtp_add_vlan_to_trunk(bg, &d_info, &v_info, t_info);
	if (swidb) 
	    vtp_macaddrs_update(swidb);
    }
    
    return (1);
    
}



unsigned char vtp_intf_disable (vtp_trunk_info* t_info) {
    vtp_subif_subidb *sub_sb;
    idbtype* swidb;

    FOR_ALL_SWIDBS_ON_HW(((idbtype*)t_info->trunk_id)->hwptr, swidb) {
	sub_sb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);
	if ((!sub_sb) || (!sub_sb->vtp_enabled))
	    continue; /* not a vtp sub-interface */

	delete_interface(swidb);
    }

    return 1;

}



/*
 * This function posts a add trunk link message to the VTP process.
 */
void vtp_post_configure_trunk (unsigned char mode, vtp_trunk_info*
			       t_info) {

    vtp_ipc_config_trunk *ptr;

    ptr = malloc( sizeof(vtp_ipc_config_trunk) );
    if (!ptr)
       return;

    ptr->message = VTP_CONFIGURE_TRUNK;
    ptr->operation = mode;

    memcpy(&ptr->t_info, t_info, sizeof(vtp_trunk_info));
 
    vtp_post_message( (vtp_ipc_message *)ptr );
}


/*
 * This function posts a add trunk link message to the VTP process.
 */
void vtp_post_change_trunk_link_state (unsigned char dname_length,
				       unsigned char* domain_name,
				       void* trunk_id, 
				       unsigned char up_flag) {
    vtp_ipc_prune_trunk_state* ptr;

    ptr = malloc( sizeof(vtp_ipc_prune_trunk_state) );
    if (!ptr)
       return;

    ptr->message = VTP_PRUNING_TRUNK_UPDOWN;
    ptr->dname_length = dname_length;
    memcpy(ptr->domain_name, domain_name, dname_length);
    ptr->trunk_id = trunk_id;
    ptr->up_flag = up_flag;
    vtp_post_message( (vtp_ipc_message *)ptr );
}

void 
vtp_post_change_trunk_vlan_stp_state (unsigned char dname_length, 
				      unsigned char* domain_name,
				      unsigned short isl_vlan_id,
				      void* trunk_id, 
				      unsigned short new_stp_state) {
    vtp_ipc_prune_stp_state* ptr;

    ptr = malloc(sizeof(vtp_ipc_prune_stp_state));
    if (!ptr)
	return;

    ptr->message = VTP_PRUNING_STP_CHANGE;
    ptr->dname_length = dname_length;
    memcpy(ptr->domain_name, domain_name, dname_length);
    ptr->vlan_id = isl_vlan_id;
    ptr->trunk_id = trunk_id;
    ptr->span_state = new_stp_state;
    vtp_post_message( (vtp_ipc_message *)ptr );
}    

void
vtp_post_change_vlan_pruning_eligible (void* trunk_id, 
				       unsigned char* eligibility) {
    vtp_ipc_prune_eligible *ptr;

    ptr = malloc(sizeof(vtp_ipc_prune_eligible));
    if (!ptr)
	return;

    ptr->message = VTP_PRUNING_ELIGIBLE;
    ptr->trunk_id = trunk_id;
    memcpy(ptr->elig_mask, eligibility, VLANS_BITMASK_SIZE);
    vtp_post_message( (vtp_ipc_message *)ptr);
}

void
vtp_post_set_vlan_eligible_on_trunk (void* trunk_id, 
				     unsigned short vlan_id,
				     boolean is_eligible) {
    vtp_ipc_prune_vlan_elig *ptr;

    ptr = malloc(sizeof(vtp_ipc_prune_vlan_elig));
    if (!ptr)
	return;

    ptr->message = VTP_PRUNING_VLAN_ELIG;
    ptr->trunk_id = trunk_id;
    ptr->vlan_id = vlan_id;
    ptr->is_eligible = is_eligible;
    vtp_post_message( (vtp_ipc_message *)ptr);
}

/*
 * This function posts a add VLAN message to the VTP process.
 */
void vtp_post_configure_vlan (unsigned char dname_length, 
			      unsigned char* domain_name, 
			      unsigned char mode, 
			      vtp_vlan_info* v_info) {
    vtp_ipc_config_vlan *pointer;

    pointer = malloc( sizeof(vtp_ipc_config_vlan) );

    if (!pointer)
       return;

    pointer->message = VTP_CONFIGURE_VLAN;
    pointer->dname_length = dname_length;
    memcpy(pointer->domain_name, domain_name, dname_length);
    pointer->operation = mode;

    memcpy(&pointer->v_info, v_info, sizeof(vtp_vlan_info));

    vtp_post_message((vtp_ipc_message *)pointer);
}


/*
 * This function posts a download config message to the VTP process.
 */
void vtp_post_download_config (unsigned char* domain_name, 
			       unsigned char dname_length, 
			       boolean save_config, 
			       unsigned char need_swap, 
			       unsigned char *file) {

    vtp_ipc_download_config *pointer;


    pointer = malloc( sizeof(vtp_ipc_download_config) );

    if (!pointer)
       return;

    pointer->message = VTP_DOWNLOAD_CONFIG;

    memcpy(pointer->domain_name, domain_name, dname_length);
    pointer->dname_length = dname_length;
    pointer->save_config = save_config;
    pointer->file = file;

    if (vtp_post_message((vtp_ipc_message *)pointer) == FALSE) {
	free(file);
    }
}



/*
 * This function posts a shutdown message to the VTP process.
 */
void vtp_post_shutdown (void)
{
    vtp_ipc_shutdown *pointer;

    pointer = malloc( sizeof(vtp_ipc_shutdown) );

    if (!pointer)
       return;

    pointer->message = VTP_SHUTDOWN;  

    vtp_post_message((vtp_ipc_message *)pointer);

}



/*
 * This function posts a clear statistics message to the VTP process.
 */
void vtp_post_clear_statistics (unsigned char clear_all, unsigned char
				dname_length, unsigned char *domain_name)
{

    vtp_ipc_clear_statistics *ptr;
    unsigned long ix;


    ptr = malloc(sizeof(vtp_ipc_clear_statistics));

    if (!ptr)
       return;

    ptr->message = VTP_CLEAR_STATISTICS;  

    ptr->dname_length = dname_length;
    for (ix = 0; ix < dname_length; ix++)
        ptr->domain_name[ix] = domain_name[ix];

    vtp_post_message((vtp_ipc_message *)ptr);

}



/*
 * This function posts a clear statistics message to the VTP process.
 */
void vtp_post_delete_database (unsigned char dname_length, unsigned
			       char *domain_name) {

    vtp_ipc_delete_database *ptr;
    unsigned long ix;

    ptr = malloc( sizeof(vtp_ipc_delete_database) );

    if (!ptr)
       return;

    ptr->message = VTP_DELETE_DATABASE;   

    ptr->dname_length = dname_length;
    for (ix = 0; ix < dname_length; ix++)
        ptr->domain_name[ix] = domain_name[ix];

    vtp_post_message((vtp_ipc_message *)ptr);

}


/*
 * This function processes message sent via IPC.        
 */
void vtp_process_message (void)
{
    vtp_message_t message;
    void *major;  
    vtp_ipc_message *pointer;
    unsigned long count;
 
    while ( process_get_message((ulong*)&message, &major, &count) ) {

        pointer = (vtp_ipc_message *)major;

        switch (message) {
	    
	case VTP_CONFIGURE_DOMAIN: 
	    vtp_configure_domain(pointer->config_domain.operation,
				 &pointer->config_domain.d_info);
	    break;

	case VTP_CHANGE_DOMAIN_NAME:
	    vtp_change_domain_name(pointer->dname_change.old_name_length,
				   pointer->dname_change.old_name,
				   pointer->dname_change.new_name_length,
				   pointer->dname_change.new_name); 
	    break;

	case VTP_CONFIGURE_TRUNK:
	    vtp_configure_trunk(pointer->config_trunk.operation,
				&pointer->config_trunk.t_info);
	    break;
	    
	case VTP_CONFIGURE_VLAN:	                  
	    vtp_configure_vlan(pointer->config_vlan.dname_length,
			       pointer->config_vlan.domain_name,
			       pointer->config_vlan.operation,
			       &pointer->config_vlan.v_info);
	    break;
	    
	case VTP_DOWNLOAD_CONFIG:
	    vtp_download_info(pointer->download_config.save_config, 
			      FALSE,
			      pointer->download_config.file); 
	    break;
	    
	case VTP_SHUTDOWN:       
	    vtp_running = FALSE;
	    vtp_explicitly_disarmed = TRUE;  
	    break;
	    
	case VTP_CLEAR_STATISTICS:
	    vtp_clear_statistics(pointer->clear_statistics.clear_all,  
				 pointer->clear_statistics.dname_length,
				 pointer->clear_statistics.domain_name);
	    break;
	    
	case VTP_DELETE_DATABASE:  
	    vtp_delete_database(pointer->delete_database.dname_length,
				pointer->delete_database.domain_name);
	    break;

	case VTP_TFTP_LOAD_FAILURE:
	    vtp_report_load_failure(pointer->load_failure.domain_name,
				    pointer->load_failure.dname_length);
	    break;
	    
	case VTP_PRUNING_TRUNK_UPDOWN:
	    vtp_core_change_trunk_link_state(pointer->prune_trunk_state.dname_length,
					     pointer->prune_trunk_state.domain_name,
					     pointer->prune_trunk_state.trunk_id,
					     pointer->prune_trunk_state.up_flag); 
	    break;
	case VTP_PRUNING_STP_CHANGE:
	    vtp_core_change_trunk_vlan_spt_state(pointer->prune_span_state.dname_length,
						 pointer->prune_span_state.domain_name,
						 pointer->prune_span_state.vlan_id,
						 pointer->prune_span_state.trunk_id,
						 pointer->prune_span_state.span_state); 
	    break;
	case VTP_PRUNING_ELIGIBLE:
	    vtp_core_change_vlan_pruning_eligible(pointer->prune_eligible.trunk_id,
						  pointer->prune_eligible.elig_mask);
	    break;
	case VTP_PRUNING_VLAN_ELIG:
	    vtp_core_set_vlan_eligible_on_trunk(pointer->vlan_elig.trunk_id, 
						pointer->vlan_elig.vlan_id,
						pointer->vlan_elig.is_eligible);
	    break;
	case VTP_PRUNING_LOCAL:
	    vtp_core_change_vlan_local_assoc(pointer->prune_local.dname_length, 
					     pointer->prune_local.domain_name,
					     pointer->prune_local.vlan_id,
					     pointer->prune_local.flag);
	    break;
	default:
	    break;

        }

        free(pointer);

    }
}



/************************************************************************
 *
 * Packet Queue Service.
 *
 ************************************************************************/

/*
 * Function to put a packet on the VTP input queue.
 */
void vtp_enqueue (paktype *pak)
{
    if (vtp_running) {
        process_enqueue_pak(vtp_packet_q, pak);
    }
    else
        protocol_discard(pak, FALSE);
}



/*
 * Process incoming vtp packet queue.
 */
void vtp_process_packet_queue (void)
{
    paktype *pak;
    idbtype* firstsw;
    int count;

    firstsw = NULL;

    /* process a limited number of packets */
    count = PAK_SWITCHCOUNT;

    while (count-- > 0) {

       pak = process_dequeue(vtp_packet_q);

       if (!pak)
          break;
     
       if ( !interface_up(pak->if_input) ) {
          datagram_done(pak);
          continue;
       }

       if (!pak->if_input->hwptr) {
	   datagram_done(pak);
	   continue;
       }

       firstsw = pak->if_input->hwptr->firstsw;
       if (!firstsw) {
	   datagram_done(pak);
	   continue;
       }

       VTP_LOG_RUNTIME("\nvtp pkt rcvd on %s, length %d", 
                     pak->if_input->namestring, pak->datagramsize);

       /* hexdump of incoming packet */

       VTP_LOG_PACKET( pak->datagramstart, pak->datagramsize, "VTP(I)" );

       vtp_receive_packet(firstsw, pak,
            pak->datagramsize - (pak->network_start - pak->datagramstart),
            pak->mac_start + 6);

    }

}



/************************************************************************
 *
 * Network Media Interface Support.
 *
 ************************************************************************/
/*
 * vtp_ether_macaddr
 *
 * Turn a VTP address into an encapsulation type and a MAC address.
 * Called by MEDIA_ETHER_MACADDR, indexed by LINK_VTP.
 */
boolean vtp_ether_macaddr (paktype *pak, long address,
                                  int *enctype, unsigned char **daddr,
                                  unsigned char const **other_info)
{
    *daddr = vtp_multicast;
    *enctype = ET_SNAP;
    *other_info = cisco_snap;
    return(TRUE);
}



/*
 * vtp_fddi_macaddr
 *
 * Turn a VTP address into an encapsulation type and a MAC address
 * Called by MEDIA_FDDI_MACADDR, indexed by LINK_VTP.
 */
boolean vtp_fddi_macaddr (paktype *pak, long address,
			  int *enctype, unsigned char **daddr,
			  unsigned char const **other_info,
			  unsigned long *rif_flags)
{
    *daddr = vtp_multicast;
    *enctype = ET_SNAP;
    *other_info = cisco_snap;
    *rif_flags = RIF_NOT_PRESENT;
    return(TRUE);
}


/*
 * 
 */
void vtp_macaddrs_update (idbtype *idb)
{
    unsigned char *vtp_mcast;
    unsigned char vtp_multicast_swap[IEEEBYTES];
 
    if (is_tokenring(idb->hwptr)) {
        ieee_swap(vtp_tr_func_addr, vtp_multicast_swap);
        vtp_mcast = vtp_multicast_swap;
    } else {
        vtp_mcast = vtp_multicast;
    }
 
    if (RUNNING_IF(LINK_VTP, idb)) {
        reg_invoke_tbridge_smf_update(vtp_mcast,
                                      idb,
                                      SMF_MACTYP_VTP_MCAST |
                                      SMF_ACTION_RECEIVE);
        if (idb->hwptr && idb->hwptr->listen)
            if_multi_add(idb->hwptr, idb, vtp_mcast);
    } else {
        reg_invoke_tbridge_smf_delete(vtp_mcast,
                                      idb,
                                      SMF_MACTYP_VTP_MCAST |
                                      SMF_ACTION_RECEIVE);
    }
}

/*
 * Register media dependent code for VTP
 */
void vtp_media_registry (void)
{
    reg_add_media_ether_macaddr(LINK_VTP, vtp_ether_macaddr,
                                "vtp_ether_macaddr");
    reg_add_media_fddi_macaddr(LINK_VTP, vtp_fddi_macaddr,
                               "vtp_fddi_macaddr");
    reg_add_media_interesting_addresses(vtp_macaddrs_update,
                                        "vtp_macaddrs_update");
}

/************************************************************************
 *
 * Datagram Transmit Support.
 *
 ************************************************************************/
int vtp_transmit_packet (void *trunk_id, void *buff_p, unsigned long length)
{
    vtp_trunk_info t_info;
    unsigned long said;
    idbtype* trunk_idb;
    paktype *pak;

    t_info.trunk_id = trunk_id;
    trunk_idb = (idbtype*)trunk_id;

    if (!trunk_idb)
       return (FALSE);

    if (vtp_get_trunk_info(&t_info, VTP_EXACT))
       return (FALSE);

    pak             = (paktype *)buff_p;
    pak->linktype   = LINK_VTP;
    pak->acct_proto = ACCT_PROTO_VTP; 

    switch (t_info.encaps_type) {

       case VTP_TRUNK_ENCAPS_LANE:
	   pak->if_output =
	      reg_invoke_find_lec_by_name_on_interface(vtp_default_elan_name,  
                          VTP_DEFAULT_ELAN_NAME_LENGTH, trunk_idb);
	   break;

       case VTP_TRUNK_ENCAPS_DOT10:
	   said =
	       VTP_VLAN_DEFAULT_SAID(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL); 
	   pak->if_output =
	       vtp_find_sde_on_idb(said, trunk_idb);
          break;

       case VTP_TRUNK_ENCAPS_ISL:
          pak->if_output =
	      vtp_find_idb_for_isl(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL, 
				   trunk_idb);
	  break;

       default:
          break;

    }

    if (!pak->if_output) {
	datagram_done(pak);
	VTP_LOG_RUNTIME("\nvtp trunk - no pak->if_output");
	return (FALSE);
    }

    if (pak_encap(trunk_idb->hwptr, pak, length, 0)) {
       VTP_LOG_PACKET( pak->datagramstart, pak->datagramsize, "VTP(O)" );
       datagram_out(pak);
       return (TRUE);
    }
    else {
       VTP_LOG_RUNTIME("\nVTP: Encapsulation on interface %s failed",
		       pak->if_output->namestring); 
        datagram_done(pak);
        return (FALSE);
    }
}



/************************************************************************
 *
 * MD5 Digest Checksum Facility Support.
 *
 ************************************************************************/

/*
 * Function to get the 16-byte secret value.
 * Note - the main code section taken from VTP arch spec's appendix.
 */
void vtp_get_secret_value (unsigned char passwordlen, unsigned char *password, 
                                                      unsigned char *secret)
{

    unsigned int ix;
    MD5_CTX ctx;
    unsigned char *cp, password_buf[64];
    unsigned int password_index = 0;
    unsigned int count = 0, i;


    if (passwordlen == 0)  {
        for (ix = 0; ix < 16; ix++)
             secret[ix] = 0;
    } else {
        MD5Init(&ctx);   
        /* loop until we've done 1 Megabyte */
        while (count < 100032) {
            cp = password_buf;
            for (i = 0; i < 64; i++) {
                /*
                 * Take the next byte of the password, wrapping to the
                 * beginning of the password as necessary.
                 */
                *cp++ = password[password_index++ % passwordlen];
            }
            process_may_suspend();
            MD5Update(&ctx, password_buf, 64);
            count += 64;
        }
        MD5Final(secret, &ctx);              
    }
}



/*
 * Function to get MD5 digest.
 */
void vtp_get_md5_digest (unsigned char *data, 
			 unsigned long length,
			 unsigned char *digest)
{
    MD5_CTX ctx;

    MD5Init(&ctx);
    MD5Update(&ctx, data, length);
    MD5Final(digest, &ctx);
}



/************************************************************************
 *
 * Random Number Support.
 *
 ************************************************************************/

/*
 * Function to get a 32-bit pesudo random number.
 */
unsigned long vtp_get_random_number (void)
{
    return random_gen();
}



/************************************************************************
 *
 * VLAN Management.      
 *
 ************************************************************************/
/*
 *
 */
Vtp_Notif_Err_t vtp_store_domain_info (unsigned char dname_length, 
				       unsigned char* domain_name, 
				       void* info_buf, 
				       unsigned long info_size) {
    filetype file;
    vtp_domain_info d_info;
    vtp_vlan_info* v_info_p;
    unsigned short vlan_id;

    v_info_p = NULL;
    vlan_id = 0;

    d_info.domain_name_length = dname_length;
    memcpy(d_info.domain_name, domain_name, dname_length);
    if (vtp_get_domain_info(&d_info, VTP_EXACT))
	return VTP_GENERAL_ERR;

    if (d_info.mode != VTP_SERVER)
	return VTP_NOTIFICATION_OK;

    file.filename = d_info.tftp_file_name;
    file.proto = FILE_ACCESS_TFTP;
    file.resetsubr = NULL;
    file.ps.tuba.boothost.type = ADDR_IP;
    file.ps.tuba.boothost.length = ADDRLEN_IP;
    file.ps.tuba.boothost.ip_addr = d_info.tftp_server;
    file.ps.tuba.username = NULL;
    file.ps.tuba.display = FALSE;
	
    if (!reg_invoke_file_write(file.proto, &file, info_buf, info_size)) 
	return VTP_NOTIFICATION_NVR_ERR;

    return VTP_NOTIFICATION_OK;
}


static 
void vtp_set_pruning_state_on_all_trunks (vtp_domain_info* d_info) {
    vtp_trunk_info t_info;
    idbtype *firstsw, *swidb;
    vtp_subif_subidb *sub_sb;
    boolean pruning_enabled;
    unsigned char pruning_status[VLANS_BITMASK_SIZE];
    unsigned long vlan_id;

    pruning_enabled = (d_info->pruning_mode == VTP_PRUNING_ENABLE);
    
    if (vtp_get_trunk_by_domain(d_info->domain_name_length,
				d_info->domain_name, &t_info,
				VTP_FIRST)) 
	return;

    do {
	firstsw = t_info.trunk_id;
	if (pruning_enabled) {
	    if (!vtp_core_get_trunk_pruning_status(d_info->domain_name_length,
						   d_info->domain_name,
						   t_info.trunk_id,
						   pruning_status)) 
		continue;
	}
	FOR_ALL_SWIDBS_ON_HW(firstsw->hwptr, swidb) {
	    sub_sb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);	    
	    if ((sub_sb) && (sub_sb->vtp_enabled)) {
		if (pruning_enabled) {
		    if (swidb->tbridge_on_swidb) {
			vlan_id = reg_invoke_vtp_find_vlan_by_bridge_group(swidb->span_index);
			if (pruning_status[vlan_bit_offset(vlan_id, 0)] &
			    vlan_bit_mask(vlan_id, 0)) {
			    sub_sb->is_pruned = FALSE;
			} else {
			    sub_sb->is_pruned = TRUE;
			}
		    }
		} else {
		    sub_sb->is_pruned = pruning_enabled;
		}
	    }
	}
    } while(vtp_get_trunk_by_domain(d_info->domain_name_length,
				    d_info->domain_name, &t_info,
				    VTP_NEXT) == VTP_OK);
}

Vtp_Notif_Err_t vtp_domain_change_notification (Vtp_Op_t mode,
						vtp_domain_info *old_info,
						vtp_domain_info *new_info) {
    unsigned char error;

    error = VTP_NOTIFICATION_OK;

    switch(mode) {
    case VTP_ADD:
	if (new_info->domain_name_length == 0)
	    return VTP_NOTIFICATION_OK;

	if (new_info->mode == VTP_SERVER)
	    error = vtp_load_database(new_info);
	else 
	    error = VTP_NOTIFICATION_OK;
	break;
    case VTP_MODIFY:
	if (new_info->domain_name_length == 0)
	    return VTP_NOTIFICATION_OK;

	/* If the domain name has changed then we must store the
	 * domain info if a VTP server.
	 */

	if ((new_info->mode == VTP_SERVER) &&
	    (((new_info->domain_name_length != 
	       old_info->domain_name_length)) ||
	     (memcmp(new_info->domain_name, old_info->domain_name,
		     new_info->domain_name_length)))) {
	    unsigned long buf_size;
	    void* buffer;
	    
	    if (vtp_upload_info(new_info->domain_name_length,
				new_info->domain_name, &buffer,
				&buf_size) != VTP_OK)
		return VTP_NOTIFICATION_NVR_ERR;
	    
	    if (vtp_store_domain_info(new_info->domain_name_length,
				      new_info->domain_name, buffer,
				      buf_size) != VTP_NOTIFICATION_OK)
	    { 
		free(buffer);
		return VTP_NOTIFICATION_NVR_ERR;
	    }
	    free(buffer);
	}

	if ((new_info->mode == VTP_SERVER) &&
	    (strcmp(old_info->tftp_file_name, 
		    new_info->tftp_file_name) ||
	     (old_info->tftp_server != new_info->tftp_server) ||
	     (old_info->mode == VTP_CLIENT))) {
	    error = vtp_load_database(new_info);
	} else {
	    error = VTP_NOTIFICATION_OK;
	}

	if (new_info->pruning_mode != old_info->pruning_mode) {
	    vtp_set_pruning_state_on_all_trunks(new_info);
	}

	break;
    case VTP_DELETE:
      error = VTP_NOTIFICATION_OK;
      break;
    }
    
    return error;
}


static idbtype* vtp_configure_subif (boolean create, 
				     vtp_trunk_info* t_info,
				     vtp_vlan_info* v_info) {
  idbtype* swidb, *trunk_idb;
  int subifnum, last;
  vtp_subif_subidb* sub_idb;
  swidb_sb_t sb_type;
  
  sb_type = SWIDB_SB_VTP_SUB;
  swidb = NULL;
  sub_idb = NULL;
  last  = 0;
  trunk_idb = t_info->trunk_id;

  swidb = vtp_get_subif(t_info, v_info);
  if (!create && !swidb)
    return NULL;
  
  if (create) {
    subifnum = 1; /* first non-major interface number */
    if (!swidb) {
      if (!idb_can_create_more())
	return NULL;
      FOR_ALL_SWIDBS_ON_HW(trunk_idb->hwptr, swidb) {
	if (swidb->sub_number < subifnum) /* should only happen
					     for firstsw */
	  continue;
	if (swidb->sub_number == subifnum) { /* a match try next
						one */
	  last = swidb->sub_number;
	  subifnum++;
	  continue;
	}
	break; /* aha!  A hole.  Use this number */
      }
      if (!swidb) /* match was not found */
	subifnum = last + 1;
      
      swidb = idb_create_subif(trunk_idb, subifnum); 
      
      if (!swidb) /* match was not found */
	return NULL;
      
      sub_idb = malloc(sizeof(vtp_subif_subidb));
      if (!sub_idb) {
	delete_interface(swidb);
	return NULL;
      }
      
      sub_idb->vtp_enabled = TRUE;
      sub_idb->is_pruned = TRUE;
      if (!idb_add_swsb(swidb, &sb_type, sub_idb)) {
	free(sub_idb);
	delete_interface(swidb);
	return NULL;
      }
    }
    
    sub_idb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);
    if ((!sub_idb) || (!sub_idb->vtp_enabled))
      return swidb;
    
    /* configure the sub-interface */
    switch (t_info->encaps_type) {
    case VTP_TRUNK_ENCAPS_ISL:
      if (!vtp_configure_isl_on_idb(swidb, v_info)) {
	idb_delete_swsb(swidb, SWIDB_SB_VTP_SUB);
	if (sub_idb)
	  free(sub_idb);
	delete_interface(swidb);
	return (NULL);
      }
      break;
      
    case VTP_TRUNK_ENCAPS_DOT10:
      vtp_configure_sde_on_idb(swidb, v_info);
      break;
      
    case VTP_TRUNK_ENCAPS_LANE: {
      int type, vr_rn, srb_bn;
      type = 0;
      
      if (v_info->type == VLAN_ETH_TYPE)
	type = LANE_TYPE_802_3;
      else if (v_info->type == VLAN_TR_TYPE)
	type = LANE_TYPE_802_5;
      else {
	idb_delete_swsb(swidb, SWIDB_SB_VTP_SUB);
		  if (sub_idb)
		    free(sub_idb);
		  delete_interface(swidb);
		  return NULL;
      }
      
      vr_rn = v_info->ring_number;
      srb_bn = v_info->bridge_number;
      
      /* start from a clean slate */
      reg_invoke_lane_configure_client(swidb, FALSE, type,
				       v_info->vlan_name,
				       v_info->vlan_name_length,
				       v_info->isl_vlan_id,
				       vr_rn, srb_bn);
      if (!reg_invoke_lane_configure_client(swidb, TRUE,
					    type,
					    v_info->vlan_name, 
					    v_info->vlan_name_length,
					    v_info->isl_vlan_id,
					    vr_rn, srb_bn))
	{
	  idb_delete_swsb(swidb, SWIDB_SB_VTP_SUB);
	  if (sub_idb)
	    free(sub_idb);
	  delete_interface(swidb);
	  return NULL;
	}
    }
    }
  } else {
    sub_idb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);
    if (!sub_idb)
      return NULL;
    if (sub_idb->vtp_enabled) {
      if (!idb_delete_swsb(swidb, SWIDB_SB_VTP_SUB))
	return NULL;
      free(sub_idb);
      delete_interface(swidb);
    }
    return NULL;
  }
  
  return swidb;
}

static idbtype* vtp_get_subif (vtp_trunk_info* t_info, 
			       vtp_vlan_info* v_info) {
    idbtype* swidb;

    switch (t_info->encaps_type) {
	
    case VTP_TRUNK_ENCAPS_ISL:
	if (v_info->type != VLAN_ETH_TYPE)
	    return NULL;
	swidb = vtp_find_idb_for_isl(v_info->isl_vlan_id,
				     t_info->trunk_id); 
	break;
	
    case VTP_TRUNK_ENCAPS_DOT10:
	if (v_info->type != VLAN_FDDI_TYPE)
	    return NULL;
	swidb = vtp_find_sde_on_idb(v_info->said, t_info->trunk_id);
	break;
	
    case VTP_TRUNK_ENCAPS_LANE:
	swidb =
	    reg_invoke_find_lec_by_name_on_interface(v_info->vlan_name, 
						     v_info->vlan_name_length, 
						     t_info->trunk_id);
	break;

    default:
	return (NULL);
    }

    return swidb;
}



static idbtype* vtp_add_vlan_to_trunk (int bg, 
				       vtp_domain_info* d_info, 
				       vtp_vlan_info* v_info, 
				       vtp_trunk_info* t_info) {
    vtp_subif_subidb *sub_sb;
    idbtype* swidb;
    
    if ((t_info->dname_length != d_info->domain_name_length) ||
	( memcmp(t_info->domain_name, d_info->domain_name,
		 t_info->dname_length)))
	return (NULL); /* This trunk does not belong to the changed domain */

    swidb = vtp_configure_subif(TRUE, t_info, v_info);
    if (!swidb)
      return NULL;

    sub_sb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);

    if ((sub_sb) && (d_info->pruning_mode == VTP_PRUNING_DISABLE)) {
	sub_sb->is_pruned = FALSE;
    }

    /* Is this sub-interface already in a bridge group ? */
    if (swidb->tbridge_on_swidb) {
	if (reg_invoke_vtp_find_vlan_by_bridge_group(swidb->span_index) == 
	    v_info->isl_vlan_id) {
	    vtp_post_change_trunk_vlan_stp_state(d_info->domain_name_length, 
						 d_info->domain_name,
						 v_info->isl_vlan_id, 
						 t_info->trunk_id,
						 swidb->port_state ==
						 PORT_FORWARDING ? 
						 VLAN_SPT_STATE_FWD :
						 VLAN_SPT_STATE_NON_FWD); 
	    return swidb; /* we are done, subif is in right bridge
			   * group */
	}
    }

    if ((!sub_sb) || (!sub_sb->vtp_enabled))
	return NULL; /* We have been told 'hands off' by the operator */
    
    /* See if the sub-if can't be assigned to the right bridge
       group or try to add sub-if to new bridge group */
    reg_invoke_vtp_config_subif_in_bridge_group(TRUE, bg, swidb);
    return swidb;
}

static idbtype* vtp_modify_vlan_in_trunk (int bg, 
					  vtp_domain_info* d_info, 
					  vtp_vlan_info* old_info,
					  vtp_vlan_info* new_info,
					  vtp_trunk_info* t_info) { 
    vtp_subif_subidb* sub_sb;
    idbtype* swidb;
    boolean change;

    change = FALSE;
    swidb = NULL;

    if ((t_info->dname_length != d_info->domain_name_length) ||
	(memcmp(t_info->domain_name, d_info->domain_name,
		t_info->dname_length))) 
	return NULL; /* This trunk does not belong to the changed
			domain */
    switch(t_info->encaps_type) {
    case VTP_TRUNK_ENCAPS_ISL:
    case VTP_TRUNK_ENCAPS_DOT10:
	break;
    case VTP_TRUNK_ENCAPS_LANE:
	swidb =
	    reg_invoke_find_lec_by_name_on_interface(old_info->vlan_name,
						     old_info->vlan_name_length, 
						     t_info->trunk_id);
	if (!swidb)
	    return NULL;

	sub_sb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);

	/* check if we have access */
	if ((!sub_sb) || (!sub_sb->vtp_enabled))
	    return NULL;

	if (old_info->vlan_name_length != new_info->vlan_name_length)
	    change = TRUE;
	else if (memcmp(old_info->vlan_name, new_info->vlan_name,
			old_info->vlan_name_length))
	    change = TRUE;
	change |= old_info->type != new_info->type;
	change |= old_info->ring_number != new_info->ring_number;
	change |= old_info->bridge_number != new_info->bridge_number;

	if (change) {
	    int type, vr_rn, srb_bn;

	    type = 0;
	    if (new_info->type == VLAN_ETH_TYPE)
		type = LANE_TYPE_802_3;
	    else if (new_info->type == VLAN_TR_TYPE)
		type = LANE_TYPE_802_5;
	    else
		return NULL;
	      
	    vr_rn = new_info->ring_number;
	    srb_bn = new_info->bridge_number;
	    
	    reg_invoke_lane_configure_client(swidb, FALSE, 0,
					     old_info->vlan_name,
					     old_info->vlan_name_length, 
					     old_info->isl_vlan_id,
					     old_info->ring_number,
					     old_info->bridge_number);

	    reg_invoke_lane_configure_client(swidb, TRUE, type,
					     new_info->vlan_name,
					     new_info->vlan_name_length, 
					     new_info->isl_vlan_id,
					     new_info->ring_number,
					     new_info->bridge_number);
	}
    }

    if ((new_info->state == VLAN_SUSPENDED_STATE) && (swidb->subif_state ==
						    SUBIF_STATE_UP)) 
	shutdown_subif(swidb, SUBIF_STATE_ADMINDOWN);

    return swidb;
}



static void vtp_delete_vlan_from_trunk (vtp_domain_info* d_info,
					vtp_vlan_info* v_info, 
					vtp_trunk_info* t_info) { 

    if ((t_info->dname_length != d_info->domain_name_length) ||
	(memcmp(t_info->domain_name, d_info->domain_name,
		t_info->dname_length)))
	return; /* This trunk does not belong to the changed domain */

    /* remove any encapsulations and delete the sub-interface */
    vtp_configure_subif(FALSE, t_info, v_info);
}



static int vtp_get_bridge_group (vtp_domain_info* d_info,
				 vtp_vlan_info* v_info) {
    int bg;
    
    bg = reg_invoke_vtp_find_bridge_group_by_vlan(d_info->domain_name,
                                                  d_info->domain_name_length,
                                                  v_info->isl_vlan_id);
    if (bg != -1)
       return (bg);

    /* Try to create one */
    bg = reg_invoke_vtp_configure_bridge_group(TRUE, d_info->domain_name,
					       d_info->domain_name_length,
					       v_info->isl_vlan_id); 

    return (bg);
}



/*
 *
 */
Vtp_Notif_Err_t vtp_vlan_change_notification (Vtp_Op_t mode,
					      unsigned char dname_length,
					      unsigned char *domain_name,
					      vtp_vlan_info *old_info,
					      vtp_vlan_info *new_info)
{
    vtp_domain_info d_info;
    vtp_trunk_info t_info;
    unsigned char error;
    unsigned short vlan_id;
    int bg;

    bg = 0;
    error = VTP_NOTIFICATION_OK;

    /* Can't have a trunk if in the no-domain state, so don't bother
       trying to set up bridge groups and sub-interfaces. */

    if (dname_length == 0)
	return VTP_NOTIFICATION_OK;

    memcpy(d_info.domain_name, domain_name, dname_length);
    d_info.domain_name_length = dname_length;
    if (vtp_get_domain_info(&d_info, VTP_EXACT))
	return VTP_GENERAL_ERR; 
    
    switch (mode) {
    case VTP_ADD:
	bg = vtp_get_bridge_group(&d_info, new_info);

	vlan_id = reg_invoke_vtp_find_vlan_by_bridge_group(bg);
	if (!vlan_id)
	    return VTP_GENERAL_ERR;

	if (bridge_group_ref[bg] > 0) {
	    vtp_post_change_vlan_local_assoc(d_info.domain_name_length, 
					     d_info.domain_name, vlan_id,
					     VLAN_LOCAL_ASSOC); 
	}

	error = vtp_get_trunk_info(&t_info, VTP_FIRST);
	while (!error) {
	    vtp_add_vlan_to_trunk(bg, &d_info, new_info, &t_info);
	    error = vtp_get_trunk_info(&t_info, VTP_NEXT);
	}
	break;
    case VTP_DELETE:
	/* delete the bridge group */
	bg = reg_invoke_vtp_configure_bridge_group(FALSE, domain_name,
						   dname_length,
						   old_info->isl_vlan_id);
	bridge_group_ref[bg] = 0;
	error = vtp_get_trunk_info(&t_info, VTP_FIRST);
	while (!error) {
	    vtp_delete_vlan_from_trunk(&d_info, old_info, &t_info);
	    error = vtp_get_trunk_info(&t_info, VTP_NEXT);
	}
	break;
    case VTP_MODIFY:
	error = vtp_get_trunk_info(&t_info, VTP_FIRST);

	while (!error) {
	    vtp_modify_vlan_in_trunk(bg, &d_info, old_info, new_info,
				     &t_info);
	    error = vtp_get_trunk_info(&t_info, VTP_NEXT);
	}
	break;
    }

    return VTP_NOTIFICATION_OK;
}



/*
 *
 */
void vtp_error_notification (unsigned char dname_length, 
			     unsigned char *domain_name,
			     Vtp_Err_t type)
{
    return;
}



Vtp_Notif_Err_t vtp_trunk_change_notification (Vtp_Op_t mode, 
					       vtp_trunk_info* old_info,
					       vtp_trunk_info* new_info) {
    switch(mode) {
    case VTP_ADD:
	if (!vtp_intf_enable(new_info))
	    return VTP_NOTIFICATION_OK;
	break;
    case VTP_DELETE:
	vtp_intf_disable(old_info);
	return VTP_NOTIFICATION_OK;
    case VTP_MODIFY:
    default:
	break;
    }
    
    return VTP_NOTIFICATION_OK;
}



/************************************************************************
 *
 * Machine Dependent Data Access.
 *
 ************************************************************************/
/*
 * Return as is on a big-edian machine, otherwise swap the bytes.
 */
unsigned long vtp_format_4_bytes (unsigned long data) 
{
    return(data);
    /****************************************************************
    for little endian machine, use :
    return((data >> 24) | ((data & 0x00ff0000) >> 8) | 
      ((data & 0x0000ff00) << 8) | (data << 24));                      
    ****************************************************************/
}



/*
 * Return as is on a big-edian machine, otherwise swap the bytes.
 */
unsigned short vtp_format_2_bytes (unsigned short data) 
{
    return(data);
    /****************************************************************
    for little endian machine, use :
    return((unsigned short)((data >> 8) | (data << 8)));        
    ****************************************************************/
}



/************************************************************************
 *
 * CLI database add/mod/delete support.
 *
 ************************************************************************/

vtp_vlan_info_chain *vlan_cli_entry_head = NULL;
vtp_vlan_info_chain *vlan_cli_entry_tail = NULL;
static unsigned short vlan_cli_num_vlans = 0;

boolean vtp_cli_database_find (vtp_vlan_info* info, unsigned short
			   vlan_id) { 
    vtp_vlan_info_chain *vlan_p; 

    vlan_p = vlan_cli_entry_head;

    while (vlan_p && vlan_p->info.isl_vlan_id < vlan_id) 
	vlan_p = vlan_p->next;
    
    if ((!vlan_p) || (vlan_p->info.isl_vlan_id != vlan_id))
	return FALSE;    

    memcpy(info, &vlan_p->info, sizeof(vtp_vlan_info));
    return TRUE;
}

int vtp_cli_database_editing (unsigned char mode,
			      vtp_vlan_info* vlan_info) {
    vtp_vlan_info_chain *vlan_p, *prev, *new_vlan;

    prev = NULL;
    vlan_p = vlan_cli_entry_head;

    switch (mode) {
    case VTP_ADD:
	new_vlan = malloc(sizeof(vtp_vlan_info_chain));
	if (!new_vlan)
	    return -1;
        new_vlan->next = NULL;    
	memcpy(&new_vlan->info, vlan_info, sizeof(vtp_vlan_info));

	while (vlan_p && vlan_p->info.isl_vlan_id < vlan_info->isl_vlan_id) {
	    prev = vlan_p;
	    vlan_p = vlan_p->next;
	}

	if ((vlan_p) && (vlan_p->info.isl_vlan_id == vlan_info->isl_vlan_id))
	    return -1;                            /* entry already
						     exists */

	if (!prev) {                              /* adding to head */
	    new_vlan->next = vlan_cli_entry_head;
	    vlan_cli_entry_head = new_vlan;
	} else if (!vlan_p) {                     /* adding to tail */
	    prev->next = new_vlan;
	    vlan_cli_entry_tail = new_vlan;
	} else {                                  /* adding to center */
	    prev->next = new_vlan;
	    new_vlan->next = vlan_p;
	}
	vlan_cli_num_vlans++;
	break;
    case VTP_MODIFY:
	while (vlan_p && vlan_p->info.isl_vlan_id < vlan_info->isl_vlan_id) 
	    vlan_p = vlan_p->next;

	if ((!vlan_p) || (vlan_p->info.isl_vlan_id != vlan_info->isl_vlan_id))
	    return -1;

	memcpy(&vlan_p->info, vlan_info, sizeof(vtp_vlan_info));
	break;
    case VTP_DELETE:
	while (vlan_p && vlan_p->info.isl_vlan_id < vlan_info->isl_vlan_id) {
	    prev = vlan_p;
	    vlan_p = vlan_p->next;
	}

	if ((!vlan_p) || (vlan_p->info.isl_vlan_id !=
			  vlan_info->isl_vlan_id)) 
	    return 0;

	if (!prev)                             /* removing head */
	    vlan_cli_entry_head = vlan_p->next;
	else if (!vlan_p->next) {              /* removing tail */
	    vlan_cli_entry_tail = prev;
	    prev->next = NULL;
	} else                                 /* removing middle */
	    prev->next = vlan_p->next;
	vlan_cli_num_vlans--;
	free(vlan_p);
    }
    return 0;
}


int vtp_cli_database_init (vtp_domain_info* d_info) {
    vtp_vlan_info_chain *ptr;
    vtp_vlan_info vlan;
    unsigned char error;

    while ((ptr = vlan_cli_entry_head) != NULL) {
        vlan_cli_entry_head = ptr->next;
        free(ptr);
    }

    vlan_cli_num_vlans = 0;

    error = vtp_get_vlan_info(d_info->domain_name_length,
			      d_info->domain_name, &vlan, VTP_FIRST); 

    while (!error) {
	ptr = malloc(sizeof(vtp_vlan_info_chain));

	/* we're out of memory, clean up and return */
	if (!ptr) { 
	    vtp_cli_database_abort();
	    return -1; 
	}

	ptr->next = NULL;
	memcpy(&ptr->info, &vlan, sizeof(vtp_vlan_info));
        if (vlan_cli_entry_head == NULL) {
            vlan_cli_entry_head = ptr;
            vlan_cli_entry_tail = ptr;
        } else {
            vlan_cli_entry_tail->next = ptr;
            vlan_cli_entry_tail = ptr;
        }
	vlan_cli_num_vlans++;
	error = vtp_get_vlan_info(d_info->domain_name_length,
				  d_info->domain_name, &vlan,
				  VTP_NEXT); 
    }

    return 0;
}

/*
 * Abort the CLI editing buffer.
 */
void vtp_cli_database_abort (void)
{
    vtp_vlan_info_chain *ptr;

    while ((ptr = vlan_cli_entry_head) != NULL) {
        vlan_cli_entry_head = ptr->next;
        free(ptr);
    }

    vlan_cli_num_vlans = 0;
}



/*
 * Apply the CLI editing buffer to the database.
 */
void vtp_cli_database_apply (vtp_domain_info* d_info, int res_method) 
{
    vtp_vlan_info_chain *vlan_ptr;
    unsigned char* buffer, *ptr;

    if (!d_info)
	return;

    vlan_ptr = NULL;

    buffer = malloc(sizeof(vtp_domain_info) + 
		    (vlan_cli_num_vlans * sizeof(vtp_vlan_info))); 
    if (!buffer) {
	vtp_cli_database_abort();
	return; /*out of memory */
    }

    ptr = buffer;
    d_info->vlan_count = vlan_cli_num_vlans;
    d_info->updater = vtp_dep_get_local_ipaddr();
    vtp_get_timestamp(d_info->timestamp);

    /* If this config is being read in from NVRAM then it gets a
       revision number of zero, meaning any proceeding information
       will get precedence over it. */

    if (res_method == RES_MANUAL)
	d_info->revision++;
    else
	d_info->revision = 0;
    memcpy(ptr, d_info, sizeof(vtp_domain_info));
    ptr += sizeof(vtp_domain_info);

    vlan_ptr = vlan_cli_entry_head;

    while (vlan_ptr) {
	memcpy(ptr, &vlan_ptr->info, sizeof(vtp_vlan_info));
	ptr += sizeof(vtp_vlan_info);
	vlan_ptr = vlan_ptr->next;
    }

    /* use of nv_valid prevents a config being read in from NVRAM
       being written out to the TFTP server */
    vtp_post_download_config(d_info->domain_name,
			     d_info->domain_name_length, res_method ==
			     RES_MANUAL, FALSE, buffer);

    /* clear the temporary memory */
    vtp_cli_database_abort();

    /* null the list header */
    vlan_cli_entry_head = NULL;
    vlan_cli_entry_tail = NULL;
    
    return;
}


/* vtp_dep_add_trap_functions
 *
 * Called by the system-dependant VTP MIB code to register the
 * necessary functions to support VTP traps.
 */
void vtp_dep_add_trap_functions () {

    reg_add_vtp_revnum_error_detected(vtp_revnum_error_detected,
				      "vtp_revnum_error_detected");
    reg_add_vtp_digest_error_detected(vtp_digest_error_detected,
				      "vtp_digest_error_detected");
    reg_add_vtp_storage_exceeded(vtp_storage_exceeded,
				 "vtp_storage_exceeded");
    reg_add_vtp_mtu_too_big(vtp_mtu_too_big, "vtp_mtu_too_big");
    reg_add_vtp_report_apply_status(vtp_report_apply_status,
				    "vtp_report_apply_status"); 

}


/* vtp_dep_get_trunk_from_ifIndex
 *
 * Called by the system-dependant VTP MIB code to verify that a VTP
 * trunk exists for the given ifIndex (used during test and set
 * operation).  The routine returns the trunk-id (normally a void
 * pointer to a platform-dependant interface structure) or NULL if no
 * trunk is found for the given interface.
 */

void* vtp_dep_get_trunk_from_ifIndex (long vlanTrunkPortIfIndex) {
    snmpidbtype* snmpidb;
    
    snmpidb = reg_invoke_ifmib_snmpidb_get(NULL, -1, EXACT, 
                                           vlanTrunkPortIfIndex,
					   NULL, NULL); 
    if ((!snmpidb) || (!snmpidb->snmp_if_ptr.ifptr.hwidb))
	return NULL; 

    return snmpidb->snmp_if_ptr.ifptr.hwidb->firstsw;
}


/* vtp_dep_get_ifIndex_from_trunk
 *
 * Called by the system-dependant VTP MIB code to retrieve the ifIndex
 * associated with a VTP trunk-id.  The routine returns the ifIndex
 * associated with a VTP trunk-id, zero otherwise.
 */

long vtp_dep_get_ifIndex_from_trunk (void* trunk_id) {
   idbtype* swidb;

   if (!trunk_id)
       return 0;

   swidb = (idbtype*)trunk_id;
   if (!swidb->hwptr)
       return 0;

   return swidb->hwptr->snmp_if_index;
}


/* vtp_dep_get_local_ipaddr
 *
 * Called by the system-dependant VTP MIB code when a new domain is
 * created or when the VTP VLAN database is updated.  Also called by
 * the VTP parser code when a new domain is created or the database
 * updated.   The routine returns the first found IP address on the
 * device.  If no IP address is found then zero is returned.
 */

unsigned long vtp_dep_get_local_ipaddr () {
    idbtype* idb;

    FOR_ALL_SWIDBS(idb) {
	if (idb->ip_enabled) 
	    return idb->ip_address;
    }
    return 0;
}


/* vtp_dep_validate_managementDomainRowStatus
 *
 * Called by the VTP MIB system-dependant code to verify that the
 * RowStatus requested via SNMP is legal for the platform.  Generally
 * the router allows the creation and deletion of managementDomains
 * while the LAN switches do not
 */

boolean vtp_dep_validate_managementDomainRowStatus (long value) {
    if ((value == VTP_CREATE_AND_WAIT) || (value == VTP_NOT_IN_SERVICE))
	return FALSE;

    return TRUE;
}

/* vtp_dep_validate_vlanTrunkPortRowStatus
 *
 * Called by the VTP MIB system-dependant code to verify that the
 * RowStatus requested via SNMP is legal for the platform.  Platforms
 * that do not support explicit trunk creation via SNMP would not
 * allow creation or deletion
 */

boolean vtp_dep_validate_vlanTrunkPortRowStatus (long value) {
    if ((value == VTP_CREATE_AND_WAIT) || (value == VTP_NOT_IN_SERVICE))
	return FALSE;

    return TRUE;
}

/* vtp_dep_validate_vlanTrunkPortEncaps
 *
 * Called by the system-dependant VTP MIB code to insure that the
 * encapsulation specified via SNMP makes sense for the port specified
 * (LANE for ATM, 802.10 for FDDI, ISL for FastEthernet).  If there is
 * an inconsistentcy the function returns INCONSISTENT_VALUE_ERROR,
 * otherwise it returns NO_ERROR.
 */

long vtp_dep_validate_vlanTrunkPortEncaps (long ifIndex, long encaps) {
    idbtype* trunk;

    trunk = vtp_dep_get_trunk_from_ifIndex(ifIndex);

    if (!trunk)
	return INCONSISTENT_VALUE_ERROR;
    
    if ((idb_is_fastethernet(trunk)) && (encaps != VTP_TRUNK_ENCAPS_ISL))
	return INCONSISTENT_VALUE_ERROR;

    if ((idb_is_fddi(trunk)) && (encaps != VTP_TRUNK_ENCAPS_DOT10))
	return INCONSISTENT_VALUE_ERROR;

    if ((idb_is_atm(trunk)) && (encaps != VTP_TRUNK_ENCAPS_LANE))
	return INCONSISTENT_VALUE_ERROR;

    return NO_ERROR;
}

/* vtp_dep_get_notifications_enabled
 *
 * Called by the VTP MIB system-dependant code to determine whether
 * notifications are enabled or disabled.  The routine returns the
 * TruthValue for the notificiation status (true(1), false(2). 
 */

long vtp_dep_get_notifications_enabled () { 
    return vtp_notification_enabled;
}

/* vtp_dep_notification_enabled
 *
 * Called by the VTP MIB system-dependant code to set the value of the
 * VTP notifications enabled variable.  Acceptable values are true(1)
 * and false(2). 
 */

void vtp_dep_notification_enabled (long new_value) {
    vtp_notification_enabled = new_value;
}




/*
 * no response to vtp request packet - take corrective action.
 * Prostack requires this. 
 */
void vtp_no_request_response (void)
{
 
 
 
}

unsigned char 
vtp_dep_vlan_state_change_notification (unsigned char dname_length,
					unsigned char *domain_name, 
					void *trunk_id, 
					unsigned short num_changed_vlan,
					vtp_vlan_status *info) {
  vtp_trunk_info t_info;
  vtp_vlan_info v_info;
  vtp_vlan_status* v_sts;
  vtp_subif_subidb* subif_sb;
  idbtype* swidb;
  int t;
  
  t_info.trunk_id = trunk_id;
  if (vtp_get_trunk_info(&t_info, VTP_EXACT) != VTP_OK)
    return VTP_GENERAL_ERR;
  
  for (t = 0, v_sts = info; t < num_changed_vlan; t++, v_sts++) {
    v_info.isl_vlan_id = v_sts->isl_vlan_id;
    if (vtp_get_vlan_info(dname_length, domain_name, &v_info,
			  VTP_EXACT) != VTP_OK) 
      continue;

    swidb = vtp_get_subif(&t_info, &v_info);
    if (!swidb)
      continue;
    
    subif_sb = idb_get_swsb(swidb, SWIDB_SB_VTP_SUB);
    if ((!subif_sb) || (!subif_sb->vtp_enabled))
      continue;

    subif_sb->is_pruned = !v_sts->isl_vlan_status;
  }

  return VTP_NOTIFICATION_OK;
}

boolean vtp_configure_isl_on_idb (idbtype *trunk_idb, vtp_vlan_info *info)
{

   hwidbtype *hwidb;
   idbtype *swidb;
   isl_vlan_descriptor *isl_vlan_index;
   unsigned char index;
   int subif_index;


   hwidb = trunk_idb->hwptr;
 
   /*
    * Only one subinterface per hardware IDB can be
    * configured to be within the same Virtual LAN.
    */
   FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
      if ( (swidb->isl_vlan) &&
           ( (swidb->isl_vlan->colour == info->isl_vlan_id)
                                          && (swidb != trunk_idb) ) ) {

         printf("\n\nConfiguration of multiple subinterfaces of the same"
                " main interface\nwith the same ISL vLAN colour (%d) is"
                " not permitted."
                "\n\nThis ISL colour is already configured on interface"
                " %s.\n", info->isl_vlan_id, swidb->namestring);
         return (FALSE);
      }
   } 
 
   /*
    * Walk the linked list of ISL vLAN Descriptors, typically there'll only
    * be one.
    *
    * Note we can do one of three things here:
    *
    * 1.  Add a new ISL structure to the hash table. Also add it to
    *     the subinterface software IDB.
    *
    * 2.  Link a new ISL structure to an existing chain.
    *
    * 3.  Add a subinterface to an existing ISL entry. i.e. another
    *     subinterface of a different physical interface supporting this
    *     ISL colour. Adding multiple subinterfaces to a virtual LAN is
    *     done via using the hardware IDBs interface index as a subscript
    *     into an array of MAX interfaces.
    */
 
   /* obtain hash index from isl vlan id (i.e. Colour) */
   index  = info->isl_vlan_id >> 8;
   index ^= info->isl_vlan_id;
 
   if (isl_vlan_table[index] == NULL) {
 
      isl_vlan_table[index] = (isl_vlan_descriptor *)
                                malloc(sizeof(isl_vlan_descriptor));
 
      if (isl_vlan_table[index] == NULL)
         return (FALSE);
 
      trunk_idb->isl_vlan = isl_vlan_table[index];
   }

   for (isl_vlan_index = isl_vlan_table[index]; isl_vlan_index != NULL;
                       isl_vlan_index = isl_vlan_index->next_isl_vlan_db) {
 
    /* If first entry or we've found an ISL colour matching that entered */
 
        if ( (isl_vlan_index->colour == 0) ||
             (isl_vlan_index->colour == info->isl_vlan_id) ) {
 
            isl_vlan_index->colour = info->isl_vlan_id;
            subif_index = trunk_idb->hwptr->hw_if_index;
            isl_vlan_index->isl_vlan_subinterface[subif_index] = trunk_idb;
            trunk_idb->isl_vlan = isl_vlan_table[index];
            isl_vlan_index->number_subif_in_isl_vlan++;
 
            /*
             * Having configured an interface for ISL vLAN encapsulation now
             * program the FEIP address filter (currently supporting 32 entries
             * to absorb ISL multicast packets received on that interface, if
             * we haven't already. This check avoids dulpicates ...
             */
            if (hwidb->isl_vlan_on_hwidb != TRUE) {
 
                if_multi_add(hwidb, trunk_idb, isl_vlan_multicast);
                hwidb->isl_vlan_on_hwidb = TRUE;
            }
 
            return (TRUE);
        }
    }
 
    /* Else add a new ISL vLAN (colour) descriptor to the linked list */
 
    isl_vlan_index->next_isl_vlan_db = isl_vlan_index
          = malloc( sizeof(isl_vlan_descriptor) );
 
    isl_vlan_index->colour = info->isl_vlan_id;
    subif_index = trunk_idb->hwptr->hw_if_index;
    isl_vlan_index->isl_vlan_subinterface[subif_index] = trunk_idb;
    isl_vlan_index->number_subif_in_isl_vlan++;
 
    if (hwidb->isl_vlan_on_hwidb != TRUE) {
        if_multi_add(hwidb, trunk_idb, isl_vlan_multicast);
        hwidb->isl_vlan_on_hwidb = TRUE;
    }

    return (TRUE);

}



void vtp_configure_sde_on_idb (idbtype *swidb, vtp_vlan_info *info)
{
    int index, subif_index;
    said_descriptor *said_index;

    if_maxdgram_adjust(swidb->hwptr,
		       swidb->hwptr->max_buffer_size + SDE_ENCAPBYTES);

    /*
     * VLAN's can have larger than media size frames, take that into account.
     */
    if (swidb->hwptr->max_buffer_size > swidb->hwptr->max_pak_size) {
	swidb->hwptr->max_pak_size =
	    swidb->hwptr->max_pak_size_def + SDE_ENCAPBYTES;
    }

    swidb->ip_routecache_same = swidb->ip_routecache_same_def = TRUE;

    /*
     * IPX can have subinterfaces independent of vLAN bridging/routing, so
     * if configured with a Novell network number then override any default
     * encapsulation.
     */
    reg_invoke_set_protocol_encaps(LINK_NOVELL, swidb, ET_SDE);
 
    /*
     * Write the SDE Security Association ID value into the SDE structure and
     * fill in the back pointer to the subinterface IDB it's configured on.
     *
     * Note we maintain an array, indexed by hardware interface number, of
     * the software IDBs associated with this SAID value, so that when we
     * match a received 802.10 packet's SAID with those configured on the
     * received interface, we can then index the corresponding subinterface
     * IDB - which ultimately let's us get the packet to the correct bridge
     * group!
     *
     * If SAID not already present then malloc the necessary space.
     * Assume entries are zeroed upon creation, so no need to initialize.
     */
 
    /*
     * Walk the linked list of Security Association IDs, typically there'll
     * only be one.
     *
     * Note we can do one of three things here:
     *
     * 1.  Add a new SAID to the hash table. Also add it to
     *     the subinterface software IDB.
     *
     * 2.  Link a new SAID structure to an existing chain.
     *
     * 3.  Add a subinterface to an existing SAID entry. i.e. another interface
     *     connecting to the Transit Backbone.
     *     Adding multiple subinterfaces to a virtual LAN is done via
     *     using the hardware IDBs interface index as a subscript into an
     *     array of MAX interfaces.
     */

    index = nethash(info->said);          /* This should be an inline
					     for the per-packet match */
    if (said_table[index] == NULL) {
	
        said_table[index] = malloc(sizeof(said_descriptor));
	
        if (said_table[index] == NULL)
            return;
	
        swidb->sde_said_db = said_table[index];
    }

    for (said_index = said_table[index]; said_index;
         said_index = said_index->next_said_db) {
	
	/* If first entry or we've found a SAID matching that entered */
	
        if ((said_index->sdb_said == 0) ||
            (said_index->sdb_said == info->said)) {
 
            said_index->sdb_said = info->said;
            subif_index = swidb->hwptr->hw_if_index;
            said_index->sdb_sub_interface[subif_index] = swidb;
            swidb->sde_said_db = said_index;
            said_index->sdb_number_subif_in_bgroup++;
            return;
        }
 
    }
 
    said_index = malloc(sizeof(said_descriptor));
    if (said_index == NULL)
        return;

    said_index->next_said_db = said_index;
    said_index->sdb_said = info->said;
    subif_index = swidb->hwptr->hw_if_index;
    said_index->sdb_sub_interface[subif_index] = swidb;
    said_index->sdb_number_subif_in_bgroup++;
}



/*
 *
 */
idbtype *vtp_find_idb_for_isl (unsigned short isl_vlan_id, idbtype *trunk_idb)
{

   unsigned char index;
   isl_vlan_descriptor *isl_vlan_index;
   idbtype *swidb;
   int subif_index;


   /* obtain hash index from isl vlan id (i.e. color) */
   index  = isl_vlan_id >> 8;
   index ^= isl_vlan_id;

   isl_vlan_index = isl_vlan_table[index];

   while (isl_vlan_index) {
      if (isl_vlan_index->colour == isl_vlan_id) {
         subif_index = trunk_idb->hwptr->hw_if_index; 
         swidb = isl_vlan_index->isl_vlan_subinterface[subif_index];
         return (swidb);
      }
      isl_vlan_index = isl_vlan_index->next_isl_vlan_db;
   }

   return (NULL);

}



/*
 *
 */
idbtype *vtp_find_sde_on_idb (unsigned long said, idbtype *trunk_idb)
{
    idbtype* swidb;

    FOR_ALL_SWIDBS_ON_HW(trunk_idb->hwptr, swidb) {
	if (swidb->sde_said_db && (swidb->sde_said_db->sdb_said ==
				   said)) 
	    return swidb;
    }

   return (NULL);

}




SUBSYS_HEADER(vtp, VTP_MAJVERSION, VTP_MINVERSION, VTP_EDITVERSION,
              vtp_sys_init, SUBSYS_CLASS_PROTOCOL, "req: lane, tbridge", NULL);
      
