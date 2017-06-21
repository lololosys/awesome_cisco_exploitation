/* $Id: vtp.c,v 1.2.58.12 1996/09/13 21:47:37 thille Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp.c,v $
 *------------------------------------------------------------------
 * vtp.c - VLAN transport protocol (portable code module)
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp.c,v $
 * Revision 1.2.58.12  1996/09/13  21:47:37  thille
 * CSCdi69139:  IOS includes in platform independent VTP code.
 * Branch: California_branch
 * Remove the inclusion of IOS include files from source files shared with
 * non-IOS platforms.  Add comments so someone else won't make the same
 * mistake later.  Move the inclusions to platform specific include file
 * vtp_debug.h.
 *
 * Revision 1.2.58.11  1996/08/28  13:20:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.2.58.10  1996/08/15  21:52:07  cyoung
 * CSCdi65439:  Cannot change name of VLAN on Cat3000
 * Branch: California_branch
 *
 * Revision 1.2.58.9  1996/07/17  01:43:51  cyoung
 * Allow API users to omit specifying a domain mode when deleting.
 * CSCdi63063:  vtp_configure_domain rejects based on mode for VTP_DELETE
 * operation
 * Branch: California_branch
 *
 * Revision 1.2.58.8  1996/07/10  20:29:27  cyoung
 * Set number of VLANs before trying to generate MD5 digest
 * CSCdi62391:  VTP core code attempts to generate MD5 checksum from
 * uninited data
 * Branch: California_branch
 *
 * Revision 1.2.58.7  1996/06/29  01:56:10  cyoung
 * Don't write to a free'd memory block (duh).  Make the stack for the VTP
 * process a more reasonable size.
 * CSCdi61006:  Systems crash, 7k & 4500 simultaneously, while deleting
 * vlan.
 * Branch: California_branch
 *
 * Revision 1.2.58.6  1996/06/21  23:08:25  cyoung
 * VLAN change notifications for deleted VLANs when domain is going away
 * CSCdi60861:  bridge groups for default vlans stay after vtp domain is
 * removed
 * Branch: California_branch
 *
 * Revision 1.2.58.5  1996/06/21  19:16:35  cyoung
 * Cause notifications to occur when deleting trunks and vlans while
 * deleting a domain.
 * CSCdi60874:  sub interfaces not being removed after [no vtp-domain foo]
 * Branch: California_branch
 *
 * Revision 1.2.58.4  1996/06/20  19:50:18  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.2.58.3  1996/06/18  00:50:54  cyoung
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
 * Revision 1.2.58.2  1996/05/21  22:29:20  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.1  1996/05/09  14:48:24  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.3  1996/05/03  18:08:14  cyoung
 * Fix typo in learn_new_revision regarding info retrieval.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.84.2  1996/05/03  00:44:39  cyoung
 * Allow database manipulation in null domain state.
 * CSCdi56448:  vtp_download_info rejects null domain configuration file
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.84.1  1996/04/27  07:38:26  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.13  1996/04/22  21:38:47  percyk
 * vlan change notification for translational vlan being deleted.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.12  1996/04/18  21:26:45  percyk
 * added consistency checks for translational vlans via snmp.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.11  1996/04/17  21:56:53  cyoung
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
 * Revision 1.2.74.10  1996/04/12  23:06:10  percyk
 * ether-fddi translations cannot be set after clear/resetting vlans.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.9  1996/04/11  22:31:20  percyk
 * Catalyst does not issue 2 vtp messages for enet-fddi translation.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.8  1996/04/11  22:01:07  cyoung
 * Don't add one tlb, if you've already added two (oops).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.7  1996/04/11  21:48:24  percyk
 * check array bounds before accessing it.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.6  1996/04/11  21:29:14  percyk
 * byte swapping fixes to support intel platforms.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.5  1996/04/11  20:50:40  cyoung
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
 * Revision 1.2.74.4  1996/04/09  17:09:43  percyk
 * vlan config consistency between snmp and cli.
 * advt interval and passwd needs to be accepted from download data.
 * cannot change vlan name length once it's set.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.3  1996/04/09  04:23:59  cyoung
 * Report TFTP load failure back to VTP process.
 * CSCdi53758:  vtp server does not turn into client when fail to read
 * tftp file
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.2  1996/04/08  18:43:55  cyoung
 * Generate domain change notification regardless of whether change is
 * detectable or not.
 * CSCdi53869:  should load tftp file whenever the command is issued
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.7  1996/04/06  02:07:00  cyoung
 * CSCdi53693:  vlan is not built when it should be
 * don't abort the vtp-database command if nv_valid false
 * And the following fixes while we're at it:
 *  don't load database when writing into it
 *  set the revision number to zero when loading from NVRAM (to avoid
 *   conflict with TFTP).
 *  use vtp_rev_less in vtp_download_info
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.6  1996/04/05  19:07:28  cyoung
 * CSCdi53470:  wrong behaviour while deleting vtp-created vlans
 * CSCdi53667:  Spurious memory access found on 4500
 * Also fix possible memory leak on LEC configuration failure
 *
 * core - set the vlan type to pass checks in vtp_delete_two_tlb_entries
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.5  1996/04/04  18:24:32  percyk
 * more error codes when configuring vlans.
 * more error checking for translational vlans.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.4  1996/04/01  18:51:58  percyk
 * vtp allows more than one eth vlan to be translated to fddi vlan.
 * vtp does not update fcp when a new translation is added.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.3  1996/03/27  23:41:27  cyoung
 * MIB - implement new trap formats (correct varbinds)
 * core - vtp_delete_domain crashes with trunk defined
 * depend - TFTP read needs its own process, implement VLAN state
 *          modifications, some cleanup
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.2  1996/03/25  02:21:44  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:44:55  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.12  1996/03/22  16:22:13  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.11  1996/03/19  00:21:17  percyk
 * vtp support on isl links.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.10  1996/03/13  17:27:52  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.9  1996/03/08  18:18:57  cyoung
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
 * Revision 1.2.36.8  1996/03/07  02:35:01  cyoung
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
 * Revision 1.2.36.7  1996/03/05  01:55:19  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.6  1996/03/05  00:54:15  percyk
 * transition back to null domain state on md5 digest errors.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.5  1996/03/04  23:48:33  percyk
 * define new platform dependent routine on rcving no response to req
 * packet.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.4  1996/03/04  22:41:08  percyk
 * fix follower field in summary packet.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.3  1996/03/04  22:02:55  percyk
 * change revision number logic.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.2  1996/03/02  21:12:42  percyk
 * include bug fixes done after branch sync.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:20  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.30  1996/02/26  18:37:00  percyk
 * deleting a domain causes spurious memory access.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.29  1996/02/26  18:07:27  percyk
 * more debug messages.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.28  1996/02/24  00:58:53  percyk
 * don't sniff packets in transparent mode.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.27  1996/02/23  23:05:48  cyoung
 * Independant code put in for checking native vlan range.
 * Do not increment the domain index when adding the 'null domain'.
 * remove duplicate function declarations from vtp_interface.h
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.26  1996/02/23  22:49:07  percyk
 * don't send vtp requests in null domain state.  freeing buffer twice.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.25  1996/02/23  19:35:58  percyk
 * add accumulate/mirror logic for vtp counters.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.24  1996/02/23  17:10:30  percyk
 * misc bug fixes by cat5k group.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.23  1996/02/23  03:15:20  cyoung
 * Do not use ulong in the platform independant code.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.22  1996/02/22  23:02:05  percyk
 * delete tlb entries from partner vlans when a vlan is deleted.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.21  1996/02/22  21:51:25  cyoung
 * Report apply status of succeeded and now someOtherError where
 * appropriate.  Put back in fix for spurious memory accesses.
 * Minor code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.20  1996/02/22  04:42:50  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.19  1996/02/21  04:12:01  cyoung
 * bridge numbers start at 1. Fix spurious memory in the vtp_receive_xxx
 * functions.  VTP multicast address added to SMF.  Make sure the firstsw
 * is passed into the the vtp_receive_packetfunction instead of the
 * subinterface.  Set revision number to zero if modification is made
 * to vtp database when in other than server mode
 *
 * VTP now works over LANE.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.18  1996/02/19  22:26:39  percyk
 * fixed bug found in tlb testing.  more debugging added.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.17  1996/02/16  19:32:11  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup, add more vtp debugging.
 *
 * Revision 1.2.4.16  1996/02/15  17:49:45  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup.
 *
 * Revision 1.2.4.15  1996/02/15  02:30:18  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * move platform dependent stuff into interface files.
 *
 * Revision 1.2.4.14  1996/02/14  18:33:09  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * do not allow any changes to default vlans.
 *
 * Revision 1.2.4.13  1996/02/13  04:03:04  cyoung
 * move notification_enabled to vtp_dep.c, minor bug fixes, add code to
 * check for native vlan range and existance of trunk before allowing sets.
 * Also some code shuffle as a result of resolving conflicts.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.12  1996/02/12  22:52:04  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * define a new bootup flag.
 *
 * Revision 1.2.4.11  1996/02/12  21:32:32  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.2.4.10  1996/02/09  21:27:12  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * vtp debug support - part 1.
 *
 * Revision 1.2.4.9  1996/02/09  01:09:58  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.2.4.8  1996/02/08  04:12:31  cyoung
 * Set default time interval when creating or modifying management domain.
 * Add support for setting vlan filter on trunks and the native vlan.
 * Add support for transmitting adverts over LANE.
 * Add code for removing trunks from idblist.
 * Add interface for core code to notify dependant code when a mod has
 *  been made to the trunk list.
 * More code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.7  1996/02/01  19:17:32  cyoung
 * Start work on LANE encaps for VTP adverts.  Reset timer to zero when
 * freed.  Fix 'imaginative' cast for SAID value.  More basic cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.6  1996/01/31  02:34:01  cyoung
 * Provide function to allow variance in managementDomainEntry row
 * creation.
 * Remove redundant consistency checking.  Add hooks to correctly report
 * result of 'apply' operation.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.5  1996/01/31  00:44:03  cyoung
 * MIB fixes and beginning code for sub-interface creation
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.4  1996/01/24  01:37:51  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.3  1996/01/23  18:59:44  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * more error checking when adding vlans.
 *
 * Revision 1.2.4.2  1996/01/23  00:47:11  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.2.4.1  1996/01/11  01:47:47  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.5  1996/01/09  00:36:47  dcheng
 * Changed the HDLC type code for VTP.
 * Branch: LE_Calif_branch
 * Fixed an error in VLAN name length handling.
 *
 * Revision 1.1.2.4  1996/01/06  00:37:38  dcheng
 * Update the VTP packet format to include ring number and parent VLAN for
 * FDDI/TR VLAN, bridge number and STP type for FDDI/TR net VLAN, and
 * translational bridged VLAN id (0, 1 or 2). Update the database and
 * user interface for these as well.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.3  1996/01/04  00:38:04  dcheng
 * Delete the vtp_boolean flag "vtp_enabled" in the idb structure, clean up the
 * related code. Fix some bugs in the CLI for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.2  1996/01/03  01:49:18  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  01:40:08  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  01:36:59  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 **********************************************************************
 *   *WARNING*  This VTP file is platform independent.  *WARNING*     *
 *                                                                    *
 * This file is shared between IOS and non-IOS platforms.  Please do  *
 * not include *any* IOS include files here.  Instead, put your IOS   *
 * includes in a platform dependant file, such as vtp_debug.h.        *
 **********************************************************************
 */

/* 
 * Each platform needs to provide contents for this file for error and run
 * time logging.
 */ 
#include "vtp_externs.h"
#include "vtp_debug.h"

#include "vtp_interface.h"
#include "vtp_pub.h"
#include "vtp_private.h"
#include "vtp_pruning.h"
#include "vtp_local.h"
#include "vtp_to_mib.h"


/**
 ** Please do NOT use any uchar, ushort, uint or ulong in this file for
 ** portability reasons.
 **
 **/

/*
 *  Global variables
 */

vtp_boolean           no_domain_state;
vlan_domain_ptr   vlan_domain_head;
vtp_trunk_itf_ptr vtp_trunk_head;
vtp_boolean           vtp_is_initialized = FALSE;

static unsigned char vtp_null_domain_name[] = {0x00, 0x00};


/*
 * factory default vlan names
 */

static char vtp_factory_default_ethernet_vlan_name[] =
{ 'd', 'e', 'f', 'a', 'u', 'l', 't' };

static char vtp_factory_default_fddi_vlan_name[] =
{ 'f', 'd', 'd', 'i', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't' };

static char vtp_factory_default_tr_vlan_name[] =
{ 't', 'o', 'k', 'e', 'n', '-', 'r', 'i', 'n', 'g', '-', 'd', 'e',
      'f', 'a', 'u', 'l', 't' }; 

static char vtp_factory_default_fddinet_vlan_name[] =
{ 'f', 'd', 'd', 'i', 'n', 'e', 't', '-', 'd', 'e', 'f', 'a', 'u',
      'l', 't' };

static char vtp_factory_default_trnet_vlan_name[] =
{ 't', 'r', 'n', 'e', 't', '-', 'd', 'e', 'f', 'a', 'u', 'l', 't' };


/*
 * factory default vlan name lengths
 */

#define VTP_FACTORY_DEFAULT_ETHERNET_VLAN_NAME_LENGTH     7
#define VTP_FACTORY_DEFAULT_FDDI_VLAN_NAME_LENGTH        12
#define VTP_FACTORY_DEFAULT_TR_VLAN_NAME_LENGTH          18
#define VTP_FACTORY_DEFAULT_FDDINET_VLAN_NAME_LENGTH     15
#define VTP_FACTORY_DEFAULT_TRNET_VLAN_NAME_LENGTH       13 


unsigned long vtp_domain_index = 0;

unsigned char vtp_version;             /* version number */
short vtp_max_vlan_storage;            /* max vlan storage */



static void change_to_transparent (vlan_domain_ptr domain_p) {
    vtp_domain_info old_domain, new_domain;

    retrieve_domain_info(domain_p, &old_domain);
    domain_p->mode = VTP_TRANSPARENT;
    retrieve_domain_info(domain_p, &new_domain);
    vtp_domain_change_notification(VTP_MODIFY, &old_domain,
				   &new_domain); 
    VTP_LOG_RUNTIME("VTP mode changed to Transparent.\n");
} 


void change_to_client (vlan_domain_ptr domain_p) {
    vtp_domain_info old_domain, new_domain;

    retrieve_domain_info(domain_p, &old_domain);
    domain_p->mode = VTP_CLIENT;
    retrieve_domain_info(domain_p, &new_domain);
    vtp_domain_change_notification(VTP_MODIFY, &old_domain,
				   &new_domain);  
    VTP_LOG_RUNTIME("VTP mode changed to Client.\n");
} 


/* rev number comparisons */

vtp_boolean vtp_rev_less (unsigned long cur_rev, unsigned long rcd_rev) {
    if (cur_rev == 0)  /* zero is always less than any other number */
	return TRUE;

    if (((cur_rev < rcd_rev) && ((rcd_rev - cur_rev) < VTP_BIG_REV)) ||     
        ((cur_rev > rcd_rev) && ((cur_rev - rcd_rev) > VTP_BIG_REV)))
	return TRUE;
    
    return FALSE;
}
/*
 * Do the initialization. 
 */
void vtp_init (void)
{
    vtp_domain_info null_domain;

    /* initialize some global variables to default */
    vtp_is_initialized = TRUE;
    no_domain_state      = TRUE; 
    vtp_version          = VTP_VERSION;                          
    vtp_max_vlan_storage = VTP_STORAGE_UNKNOWN; 
    vtp_domain_index = 0;            
    
    /* initialize domain list and trunk list */
    vlan_domain_head = NULL;
    vtp_trunk_head = NULL;

    vtp_pruning_init();

    null_domain.mode = VTP_SERVER;
    null_domain.domain_name_length = VTP_DEFAULT_DOMAIN_NAME_LENGTH;
    null_domain.revision =  VTP_DEFAULT_REVISION;
    memset(null_domain.timestamp, 0,
	   VTP_TIMESTAMP_SIZE); 
    null_domain.index = VTP_DEFAULT_DOMAIN_INDEX;
    null_domain.advt_interval = VTP_FLOOD_INTERVAL;
    null_domain.password_length = VTP_DEFAULT_PASSWORD_LENGTH;
    null_domain.pruning_mode = DEFAULT_VTP_PRUNING_MODE;

    vtp_configure_domain(VTP_ADD, &null_domain);
}



/* vtp_add_default_vlans
 *
 * Called by the VTP platform-independant code whenever a domain is
 * created. The routine causes the factory-default VLANs to be added
 * to the new domain's VLAN database. 
 */
void vtp_add_default_vlans (vlan_domain_ptr d_ptr) {
    vtp_vlan_info v_info;
    unsigned char mode_hold;
    
    /* pretend we are a server so the VTP code will allow us to add
       VLANs */
    mode_hold = d_ptr->mode;
    d_ptr->mode = VTP_SERVER;
    
    /*
     * add factory-default VLANs to receive VTP packets.
     */
    
    v_info.isl_vlan_id = VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL;
    v_info.vlan_name_length =
	VTP_FACTORY_DEFAULT_ETHERNET_VLAN_NAME_LENGTH;
    memcpy(v_info.vlan_name,
	   vtp_factory_default_ethernet_vlan_name,
	   v_info.vlan_name_length); 
    v_info.type = VLAN_ETH_TYPE;
    v_info.said = VTP_VLAN_DEFAULT_SAID(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL);
    v_info.mtu = VTP_VLAN_DEFAULT_MTU;
    v_info.state = VTP_VLAN_DEFAULT_STATUS;
    v_info.ring_number = VTP_VLAN_NO_RING_NUMBER;
    v_info.bridge_number = VTP_VLAN_NO_BRIDGE_NUMBER;
    v_info.parent_vlan = VTP_VLAN_NO_PARENT_VLAN;
    v_info.stp_type = VTP_VLAN_NO_STP_TYPE;
    v_info.tb_vlan_1 = VTP_VLAN_NO_TB_VLAN;
    v_info.tb_vlan_2 = VTP_VLAN_NO_TB_VLAN;
    
   /* default ethernet vlan */
    vtp_configure_vlan(d_ptr->dname_length, d_ptr->domain_name,
		       VTP_ADD, &v_info); 
    
    v_info.isl_vlan_id = VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL;
    v_info.vlan_name_length =
	VTP_FACTORY_DEFAULT_FDDI_VLAN_NAME_LENGTH;
    memcpy(v_info.vlan_name,
	   vtp_factory_default_fddi_vlan_name,
	   v_info.vlan_name_length); 
    v_info.type = VLAN_FDDI_TYPE;
    v_info.said = VTP_VLAN_DEFAULT_SAID(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL);
    v_info.ring_number = VTP_VLAN_NO_RING_NUMBER;
    v_info.parent_vlan = VTP_VLAN_NO_PARENT_VLAN;
    v_info.tb_vlan_1 = VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL;
    v_info.tb_vlan_2 = VTP_VLAN_NO_TB_VLAN;
    
    /* default fddi vlan */
    vtp_configure_vlan(d_ptr->dname_length, d_ptr->domain_name,
		       VTP_ADD, &v_info); 
    
    v_info.isl_vlan_id = VTP_FACTORY_DEFAULT_VLAN_TR_ISL;
    v_info.vlan_name_length =
	VTP_FACTORY_DEFAULT_TR_VLAN_NAME_LENGTH;
    memcpy(v_info.vlan_name,
	   vtp_factory_default_tr_vlan_name,
	   v_info.vlan_name_length); 
    v_info.type = VLAN_TR_TYPE;
    v_info.said = VTP_VLAN_DEFAULT_SAID(VTP_FACTORY_DEFAULT_VLAN_TR_ISL);
    v_info.ring_number = VTP_VLAN_DEFAULT_RING_NUMBER;
    v_info.parent_vlan = VTP_VLAN_DEFAULT_PARENT_VLAN;
    v_info.tb_vlan_1 = VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL;
    v_info.tb_vlan_2 = VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL;
    
    /* default token ring vlan */
    vtp_configure_vlan(d_ptr->dname_length, d_ptr->domain_name,
		       VTP_ADD, &v_info); 
    
    v_info.isl_vlan_id = VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL;
    v_info.vlan_name_length =
	VTP_FACTORY_DEFAULT_FDDINET_VLAN_NAME_LENGTH;
    memcpy(v_info.vlan_name,
	   vtp_factory_default_fddinet_vlan_name,
	   v_info.vlan_name_length); 
    v_info.type = VLAN_FDDI_NET_TYPE;
    v_info.said =
	VTP_VLAN_DEFAULT_SAID(VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL);
    v_info.ring_number = VTP_VLAN_NO_RING_NUMBER;
    v_info.parent_vlan = VTP_VLAN_NO_PARENT_VLAN;
    v_info.bridge_number = VTP_VLAN_DEFAULT_BRIDGE_NUMBER;
    v_info.stp_type = VTP_VLAN_DEFAULT_STP_TYPE;
    v_info.tb_vlan_1 = VTP_VLAN_NO_TB_VLAN;
    v_info.tb_vlan_2 = VTP_VLAN_NO_TB_VLAN;
    
    /* default fddi-net vlan */
    vtp_configure_vlan(d_ptr->dname_length, d_ptr->domain_name,
		       VTP_ADD, &v_info); 

    v_info.isl_vlan_id = VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL;
    v_info.vlan_name_length =
	VTP_FACTORY_DEFAULT_TRNET_VLAN_NAME_LENGTH;
    memcpy(v_info.vlan_name,
	   vtp_factory_default_trnet_vlan_name,
	   v_info.vlan_name_length); 
    v_info.type = VLAN_TR_NET_TYPE;
    v_info.said =
	VTP_VLAN_DEFAULT_SAID(VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL);
    v_info.tb_vlan_1 = VTP_VLAN_NO_TB_VLAN;
    v_info.tb_vlan_2 = VTP_VLAN_NO_TB_VLAN;
    
    /* default tr-net vlan */
    vtp_configure_vlan(d_ptr->dname_length, d_ptr->domain_name,
		       VTP_ADD, &v_info); 
    d_ptr->mode = mode_hold;

}



/* vtp_flush_update_adv_info
 *
 * Called by the VTP platform-independant code to delete any
 * outstanding update information received from another VTP entity.
 * This could be due to configuration change (e.g. the domain is
 * deleted) or a network event (e.g. the timeout between subsets pops
 * and a new request is generated or a different/higher-rev source is
 * detected while updatin).  The routine stops the
 * update_pending_timer if active, initializes all update pending
 * values and frees the scracth area where the incoming VLAN database
 * is temporarily stored. 
 */
void vtp_flush_update_adv_info (vlan_domain_ptr domain_p)
{
    if (vtp_timer_active(domain_p->update_pending_timer)) {
	vtp_stop_timer(domain_p->update_pending_timer);
    }
    
    domain_p->vtp_updating       = FALSE;
    domain_p->next_revision      = 0;
    domain_p->next_updater_id    = 0;
    domain_p->update_trunk_p     = NULL;
    domain_p->first_vlan_p       = NULL;
    domain_p->last_vlan_p        = NULL;
    domain_p->last_value_learned = 0;
    domain_p->update_vlan_count  = 0;
    domain_p->update_vlan_length = 0;
    domain_p->next_seq_expected  = 0;
    domain_p->next_pruning_mode = 0;

    memset(domain_p->next_update_timestamp, 0, VTP_TIMESTAMP_SIZE);
    memset(domain_p->next_md5_digest, 0, VTP_MD5_DIGEST_SIZE);
    memset(domain_p->next_adv_src_mac, 0, VTP_MAC_ADDRESS_SIZE);

    domain_p->next_num_adv_subset_count = 0;
    domain_p->update_trunk_p            = NULL; 

    if (domain_p->scratch) {
       free(domain_p->scratch);
       domain_p->scratch = NULL;
    }

}


/* vtp_flush_tx_subset_adv_queue
 *
 * Called by the VTP platform-independant code when either a domain is
 * deleted or the domain's mode has changed to transparent.  This
 * routine frees any packet buffers held onto by VTP that continue
 * subset advertisements but have not been transmitted yet. 
 */
void vtp_flush_tx_subset_adv_queue (vlan_domain_ptr domain_p)
{

   unsigned long ix;
   void *buff_p;


   for (ix = 1; ix < MAX_VTP_TX_QUEUE_SIZE; ++ix) {

      buff_p = domain_p->tx_buff_queue[ix].buff_p;

      if (buff_p) {
         vtp_free_buffer(buff_p);  
         domain_p->tx_buff_queue[ix].buff_p = NULL;
         domain_p->tx_queue_count--;
      }

   }

}



/*
 * Process a received VTP packet, the top level entry.
 */
void vtp_receive_packet (void *trunk_id, void *buff_p, 
			 unsigned long vtp_pdu_length, 
			 unsigned char *src_mac) {

    adv_header_ptr pkt_p;


    /* vtp packet header */
    pkt_p = vtp_buffer_payload( trunk_id, buff_p);

    /* version mismatch? */
    if (pkt_p->version != VTP_VERSION) {
	VTP_LOG_RUNTIME("VTP: incoming packet version rcvd %d, expected %d\n",
			pkt_p->version, VTP_VERSION);
	vtp_free_buffer(buff_p);
	return; 
    }

    /* handle packet based on opcode */
    switch (pkt_p->code) {

       /* summary packet */
       case VTP_SUMMARY_ADV:
          vtp_receive_summary_adv_packet(trunk_id, buff_p,
					 vtp_pdu_length, src_mac); 
          break;
   
       /* subset packet */ 
       case VTP_SUBSET_ADV:
          vtp_receive_subset_adv_packet(trunk_id, buff_p,
					vtp_pdu_length, src_mac); 
	  break;

       /* request packet */
       case VTP_ADV_RQST:
          vtp_receive_adv_request_packet(trunk_id, buff_p,
					 vtp_pdu_length, src_mac); 
          break;

      case VTP_JOIN_MSG:
	  vtp_receive_join_packet(trunk_id, buff_p, vtp_pdu_length,
				  src_mac); 
	  break;

       /* invaild/unknown packet */
       default:
          VTP_LOG_RUNTIME("VTP: received invalid packet %x\n", pkt_p->code);
          break;
    }

    vtp_free_buffer(buff_p);

}



/* vtp_receive_summary_adv_packet
 *
 * Called by the VTP platform-independant code when a summary
 * advertisement has been received.  The routine decodes the packet
 * and will learn the domain name from the packet if in the no domain
 * state.   If in transparent mode the packet is relayed to all other
 * trunks in the VTP domain. Otherwise if the summary indicates that
 * new VTP configuration is available then the routine will cause a
 * advertisement request to be generated on the trunk the summary was
 * received on to receive the new configuration.  If there is any
 * problem decoding the packet, it is discarded 
 */
void vtp_receive_summary_adv_packet (void *trunk_id, void *buff_p, 
				     unsigned long length, 
				     unsigned char *src_mac) 
{
   vlan_domain_ptr domain_p;
   vtp_trunk_itf_ptr trunk_p, trunk_start_p;
   summary_adv_ptr summary_p;
   unsigned long rcvd_rev;

   trunk_p = NULL;
   summary_p = vtp_buffer_payload(trunk_id, buff_p);
   rcvd_rev = vtp_format_4_bytes(summary_p->revision);

   /* null domain in summary packet? */
   if (!summary_p->dname_length) {
      VTP_LOG_RUNTIME("VTP: no domain identified in summary advertisement\n");
      return;
   }

   VTP_LOG_RUNTIME("VTP: i summary, domain = %s, rev = %d, followers = %d\n",
		   summary_p->domain_name, rcvd_rev,
		   summary_p->followers);

   /* no valid domain yet? */
   if (no_domain_state) {
       vtp_domain_info new_info, old_info;
       /*
	* This is the case for "plug and play": the trunk link
	* is in no-domain state, and it learns from the recieved
	* summary advt IF domain in non-transparent mode.  This only occurs 
	* once.
	*/
       domain_p = vtp_search_domain(0, vtp_null_domain_name);
       
       if (!domain_p) {
	   VTP_LOG_RUNTIME("VTP: null domain not found for summary packet\n");
	   return;
       }
       
       if (domain_p)
	   trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
       
       if (!trunk_p) {
	   VTP_LOG_RUNTIME("VTP: no trunk link identified with null domain\n");
	   return;
       }
       
       /* transparent mode? */
       if (domain_p->mode == VTP_TRANSPARENT) {
	   VTP_LOG_RUNTIME("VTP: relaying summary packet for null domain\n");
	   vtp_relay_packet(trunk_p, buff_p, length);
	   return;
       } 
 
       retrieve_domain_info(domain_p, &old_info);
       domain_p->dname_length = summary_p->dname_length;
       memcpy(domain_p->domain_name, summary_p->domain_name,
	      summary_p->dname_length);
       domain_p->revision      = 0;
       domain_p->transitioning = TRUE;
       no_domain_state         = FALSE;
       
       retrieve_domain_info(domain_p, &new_info);
       vtp_domain_change_notification(VTP_MODIFY, &old_info,
				      &new_info);    
       VTP_LOG_RUNTIME("VTP: transitioning from null to %s domain\n",
		       domain_p->domain_name);
   } else {
       domain_p = vtp_search_domain(summary_p->dname_length,
				    summary_p->domain_name);
       
       /* discard vtp packet with no management domain identified */
       if (!domain_p) {
	   VTP_LOG_RUNTIME("VTP: domain %s not found for summary"
			   " pkt\n", domain_p->domain_name); 
	   return;
       } else {
	   VTP_LOG_RUNTIME("VTP: domain %s, current rev = %d found for"
			   " summary pkt\n", domain_p->domain_name,
			   domain_p->revision);
       } 
       
       /* discard vtp packet with no trunk link identified */
       trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
       
       if (!trunk_p) {
	   VTP_LOG_RUNTIME("VTP: no trunk link found for domain %s\n",
			   domain_p->domain_name); 
	   return;
       }
   }
   
   /* count summary packet */
   domain_p->acc.rx_summary_adv_count++;
   
   /*
    * stop the request timer which may have been started to receive a summary
    * and subset packets.
    */ 
   if (vtp_timer_active(trunk_p->rqst_timer_p)) {
      vtp_stop_timer(trunk_p->rqst_timer_p);
      trunk_p->rqst_retry_count = 0;
   }
 
   /* process or relay the packet depending on the mode */

   /* transparent mode? */
   if (domain_p->mode == VTP_TRANSPARENT) {
      VTP_LOG_RUNTIME("VTP: relaying summary packet for domain %s\n",
		      domain_p->domain_name); 
      vtp_relay_packet(trunk_p, buff_p, length);
      return;
   }

   /***** PRUNING: check if a version 1 pkt */
   vtp_pruning_check_version(trunk_p, summary_p, length);

   /* non-transparent mode */ 

   /* rcvd summary packet has lower rev number than domain's rev number */
   if ( vtp_update_revision_less_than_domain(rcvd_rev,
					     domain_p->revision) ) { 
       VTP_LOG_RUNTIME("VTP: summary packet rev %d lower than domain"
		       " %s rev %d\n", rcvd_rev, domain_p->domain_name,
		       domain_p->revision);
       vtp_rcvd_lower_rev_number(trunk_p);
       return;  
   }
   
   /* rcvd revision number equal to domain rev number? */
   if ( vtp_update_revision_equal_to_domain(rcvd_rev,
					    domain_p->revision) ) { 
       VTP_LOG_RUNTIME("VTP: summary packet rev %d equal to domain %s"
		       " rev %d\n", rcvd_rev, domain_p->domain_name,
		       domain_p->revision); 
       vtp_rcvd_equal_rev_number(trunk_p, buff_p);
       return;
   }

   if ((!rcvd_rev) && (name_cmp(summary_p->md5_digest,
				domain_p->md5_digest,
				VTP_MD5_DIGEST_SIZE, VTP_MD5_DIGEST_SIZE) ==
		       NAME_EQUAL)) {
       VTP_LOG_RUNTIME("VTP: summary packet rev %d equal to domain %s"
		       " rev %d\n", rcvd_rev, domain_p->domain_name,
		       domain_p->revision); 
       vtp_rcvd_equal_rev_number(trunk_p, buff_p);
       return;
   }

   /* rcvd revision number is higher */
   VTP_LOG_RUNTIME("VTP: summary packet rev %d greater than domain %s"
		   " rev %d\n", rcvd_rev, domain_p->domain_name,
		   domain_p->revision); 
   vtp_rcvd_higher_rev_number(trunk_p, buff_p, src_mac);
 
}



/* vtp_receive_subset_adv_packet
 *
 * Called by the VTP platform-independant code when a subset
 * advertisement has been received.  The routine decodes and processes
 * the packet.  Subsets are dropped if: the device is in the no domain
 * state (except in transparent mode); the revision in the subset is
 * less than the revision for the domain internally; the revision in
 * the subset is less than the revision in the preceeding summary; the
 * sequence number of the subset is not the next expected.
 * If the packet header is syntatically correct the subset advertisement
 * counter is incremented. If the domain is in transparent mode the
 * packet is relayed to other trunks in the domain (or all trunks in
 * the no domain state) and processing stops. If the revision in the
 * packet is greater but VTP was not expecting a subset (because it
 * missed the summary before the subset) then it sends out a request
 * for the complete update.  If VTP was expecting the subset but
 * received it from a different MAC address than it was expecting, it
 * flushes the update information received to that point and reissues
 * a request on that trunk.  Likewise if VTP was expecting the subset
 * but the revision for the subset is greater than the one expected,
 * then the stored subsets are discarded and a new request is
 * generated. If no problem has been detected by the packet at this
 * point then the subset information is recorded.  If this is the last
 * subset expected then the stored subsets are processed otherwise the
 * next expected sequence number is incremented. 
 */
void vtp_receive_subset_adv_packet (void *trunk_id, void *buff_p, 
                                  unsigned long length, unsigned char *src_mac)
{

   subset_adv_ptr subset_p;
   vlan_domain_ptr domain_p;
   vtp_trunk_itf_ptr trunk_p;
   unsigned long rcvd_rev;
   unsigned long next_rev;

   subset_p = vtp_buffer_payload(trunk_id, buff_p);

   rcvd_rev = vtp_format_4_bytes(subset_p->revision);

   /*
    * By the time a subset packet is rcvd a device must be in a particular
    * domain.  If the device is not in a particular domain or the subset
    * packet does not identify a domain it belongs to then toss it.
    * This case could also happen if the first summary packet got dropped
    * for any reason.  Should a vtp request be sent in such a case?  
    */
   if (no_domain_state) {
      VTP_LOG_RUNTIME("VTP: device in no domain state for subset packet\n");
      domain_p = vtp_search_domain(0, vtp_null_domain_name);

      if (domain_p) {
	  trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id); 
      } else {
	  VTP_LOG_RUNTIME("VTP: null domain not found for subset packet\n");
	  return;
      }
 
      if (!trunk_p) {
         VTP_LOG_RUNTIME("VTP: no trunk link identified with null domain\n");
         return;
      }
 
      /* transparent mode? */
      if (domain_p->mode == VTP_TRANSPARENT) {
         VTP_LOG_RUNTIME("VTP: relaying subset packet for null domain\n");
         vtp_relay_packet(trunk_p, buff_p, length);
         return;
      }

      return;
   }

   if (!subset_p->dname_length) {
      VTP_LOG_RUNTIME("VTP: subset packet rcvd with no domain length\n");
      return;
   }

   VTP_LOG_RUNTIME("VTP: i subset, domain = %s, rev = %d, seq = %d,"
		   " length = %d\n", subset_p->domain_name, rcvd_rev,
		   subset_p->sequence, length);
 
   domain_p = vtp_search_domain(subset_p->dname_length, subset_p->domain_name);
 
   /* domain mentioned in subset packet not found? */
   if (!domain_p) {
      VTP_LOG_RUNTIME("VTP: domain %s not found for subset pkt\n",
		      domain_p->domain_name);  
      return;
   } else {
      VTP_LOG_RUNTIME("VTP: domain %s, current rev = %d found for"
		      " subset pkt\n", domain_p->domain_name,
		      domain_p->revision); 
  }
 
   /* toss subset packet with no trunk link identified */
   trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
   if (!trunk_p)
      return;

   /* count subset packet */
   domain_p->acc.rx_subset_adv_count++;
 
   /* domain in transparent mode? */
   if (domain_p->mode == VTP_TRANSPARENT) {
      VTP_LOG_RUNTIME("VTP: relaying subset packet for domain %s\n",
		      domain_p->domain_name); 
      vtp_relay_packet(trunk_p, buff_p, length);
      return;
   }

   /* Do some basic checking */
   if ((vtp_update_revision_equal_to_domain(rcvd_rev, domain_p->revision)) ||
       (vtp_update_revision_less_than_domain(rcvd_rev, domain_p->revision)))
       return;

   /* validate packet */
   if ( vtp_validate_subset_adv(trunk_p, buff_p, length) == FALSE )
      return;

   if (!domain_p->vtp_updating) {
      if ( vtp_rev_less( vtp_format_4_bytes(domain_p->revision), rcvd_rev) && 
                                                         (rcvd_rev != 0) )
         vtp_transmit_adv_request_packet(trunk_p, 0);
      return;
   }

   /* domain currently in updating mode */

   next_rev = vtp_format_4_bytes(domain_p->next_revision);

   /* subset packet from same source as we rcvd summary packet from */
   if (name_cmp(src_mac, domain_p->next_adv_src_mac,
		VTP_MAC_ADDRESS_SIZE, VTP_MAC_ADDRESS_SIZE) ==
       NAME_EQUAL)  { 

      if ( vtp_update_revision_less_than_domain(rcvd_rev, next_rev) )
         return; 

      if (rcvd_rev == next_rev) {
         if (subset_p->sequence != domain_p->next_seq_expected)    
            return;

         vtp_record_subset_adv(trunk_p, buff_p, length);

         /* check to see if we've received all subset advs */ 
         if (domain_p->next_num_adv_subset_count == 
	     domain_p->next_seq_expected) {
            vtp_process_subset_adv(domain_p, trunk_p);
	} else {
            domain_p->next_seq_expected++;
	}
         return;
      }

      vtp_flush_update_adv_info(domain_p);
      vtp_transmit_adv_request_packet(trunk_p, 0);

   } else {
      /* subset packet from different source as we rcvd summary packet from */ 
      if ( vtp_rev_less(next_rev, rcvd_rev) ) {
         vtp_flush_update_adv_info(domain_p);
         vtp_transmit_adv_request_packet(trunk_p, 0);
      }
   } 

}


/* vtp_receive_adv_request_packet
 *
 * Called by the VTP platform-independant code when a advertisement
 * request is received.  This routine decodes and processes the
 * advertisement request.  If the device is in the no domain state
 * then the request is dropped.  If the request is for a domain that
 * the receiving trunk does not belong to then the request is dropped
 * as well.  If there is an outstanding request on the receiving trunk
 * then the request timer is stopped.  If the receiving domain is in
 * transparent mode then the request is relayed to all other trunks in
 * that domain.  Otherwise a summary advert, followed by subsets, is
 * then transmitted out the receiving trunk for that trunks domain. 
 */
void vtp_receive_adv_request_packet (void *trunk_id, void *buff_p,
				     unsigned long length, 
				     unsigned char *src_mac) {

   vlan_domain_ptr domain_p;
   adv_request_ptr adv_rqst_p;
   vtp_trunk_itf_ptr trunk_p = NULL;
   unsigned short start_value;

   /* Identify vlan management domain */
   if (no_domain_state)
      return;

   adv_rqst_p = vtp_buffer_payload(trunk_id, buff_p);
 
   domain_p = vtp_search_domain(adv_rqst_p->dname_length, 
                                            adv_rqst_p->domain_name);
   /* no management domain? */
   if (!domain_p ) {
      VTP_LOG_RUNTIME("VTP: device in no domain state for request packet\n");
      domain_p = vtp_search_domain(0, vtp_null_domain_name);

      if (domain_p) {
         trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
     } else {
         VTP_LOG_RUNTIME("VTP: null domain not found for request packet\n");
         return;
     }
 
      if (!trunk_p) {
	  VTP_LOG_RUNTIME("VTP: no trunk link identified with null domain\n");
	  return;
      }
      
      /* transparent mode? */
      if (domain_p->mode == VTP_TRANSPARENT) {
	  VTP_LOG_RUNTIME("VTP: relaying request packet for null domain\n");
	  vtp_relay_packet(trunk_p, buff_p, length);
	  return;
      }
      return;
  }
   
   /* no trunk link? */
   trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
   if (!trunk_p)
       return;
   
   /* count the packets */
   domain_p->acc.rx_request_adv_count++;
   
   if (vtp_timer_active(trunk_p->rqst_timer_p)) {
       vtp_stop_timer(trunk_p->rqst_timer_p);
       trunk_p->rqst_retry_count = 0;
   }
   
   /* domain in transparent mode? */
   if (domain_p->mode == VTP_TRANSPARENT) {
       VTP_LOG_RUNTIME("VTP: relaying advertisement packet for domain"
		       " %s\n", domain_p->domain_name); 
       vtp_relay_packet(trunk_p, buff_p, length);
       return;
   }
   
   adv_rqst_p = vtp_buffer_payload(trunk_p->trunk_id, buff_p);
   
   if (vtp_timer_active(trunk_p->advt_timer_p)) {
       vtp_stop_timer(trunk_p->advt_timer_p);
   }
   
   /* 
    * send out summary advt and subset advt starting from the
    * requested VLANs.
    */
   start_value = vtp_format_2_bytes(adv_rqst_p->start_value);
   
   if (trunk_p->jittered_value == 0) {
       vtp_do_trunk_flooding(trunk_p, start_value); 
   } else {
       trunk_p->advt_start_value = start_value; 
       vtp_start_timer(trunk_p->advt_timer_p, trunk_p->jittered_value);
   }
   
}



/* vtp_validate_subset_adv
 *
 * Called by the VTP platform-independant code to insure that the
 * fields in the subset advertisement are within the permitted ranges.
 * The routine returns TRUE is all fields are within specified range,
 * FALSE otherwise. 
 */
vtp_boolean vtp_validate_subset_adv (vtp_trunk_itf_ptr trunk_p, void *buff_p,
				 unsigned long total_length) {
    
    vlan_info_ptr info_p;
    unsigned long length; 
    unsigned char *payload_p;
    vlan_domain_ptr domain_p;
    unsigned short last_vlan_id;
    unsigned short rcvd_vlan_id;
    

    domain_p = trunk_p->domain_p;
    
    last_vlan_id = domain_p->last_value_learned;
    
    payload_p = vtp_buffer_payload( trunk_p->trunk_id, buff_p);
    
    info_p = (vlan_info_ptr)( payload_p + sizeof(subset_adv) );
    
    length = sizeof(subset_adv);
    
    /* process one vlan info entry at a time */
    while ((length + VTP_MIN_VLAN_INFO_SIZE) <= total_length) {
	
        rcvd_vlan_id = vtp_format_2_bytes(info_p->isl_vlan_id);
	
        if (rcvd_vlan_id <= last_vlan_id) {
            VTP_LOG_RUNTIME("VTP: validation of subset packet failed\n");
            return (FALSE);
        }
	
        last_vlan_id = rcvd_vlan_id;
        length += info_p->info_length;
        info_p = (vlan_info_ptr)((unsigned char *)info_p +
				 info_p->info_length); 
    }
    
    if (length != total_length) {
	VTP_LOG_RUNTIME("VTP: validation of subset packet - length"
			" mismatch\n"); 
	return (FALSE);
    }
    
    return(TRUE);
}



/* vtp_allocate_update_memory
 *
 * Called by VTP platform-independant code when preparing to receiving
 * a configuration update (from the network).  The routine allocates a
 * chunk of memory capable of holding all subsets.  If there is not
 * enough memory available the domain is changed into transparent
 * mode. 
 */
void vtp_allocate_update_memory (vtp_trunk_itf_ptr trunk_p, 
				 unsigned char followers) {

    unsigned char *mem;
    vlan_domain_ptr domain_p = trunk_p->domain_p;
    unsigned long size;

    /* The secret both preceeds and proceeds the calculated area for
       added security */
    size = followers * trunk_p->mtu + sizeof(summary_adv) +
	VTP_MD5_DIGEST_SIZE + VTP_MD5_DIGEST_SIZE;

    mem = malloc(size);

    if (mem) {
       domain_p->scratch = (void *)mem;
       domain_p->first_vlan_p = (vlan_info_ptr)(mem + 
                   VTP_MD5_DIGEST_SIZE + sizeof(summary_adv));
    }
    else
        change_to_transparent(domain_p);                          

}


/* vtp_record_subset_adv
 *  
 * Called by the VTP platform-independant code to store a valid subset
 * advertisement.  The routine copies the received subset into the
 * scratch area allocated, perserving the network byte ordering for
 * MD5 digest calculation.  The subset is appended to any other
 * subsets already received. 
 */
void vtp_record_subset_adv (vtp_trunk_itf_ptr trunk_p, void *buff_p,
			    unsigned long total_length)
{

    vlan_info_ptr dst_info_p;
    vlan_info_ptr src_info_p;
    unsigned long length; 
    unsigned char *payload_p;
    vlan_domain_ptr domain_p = trunk_p->domain_p;

    payload_p = vtp_buffer_payload(trunk_p->trunk_id, buff_p);
    src_info_p = (vlan_info_ptr)(payload_p + sizeof(subset_adv));

    if (domain_p->last_vlan_p == NULL)  
        dst_info_p = domain_p->first_vlan_p;
    else 
        dst_info_p = (vlan_info_ptr)((unsigned char
				      *)(domain_p->last_vlan_p) +  
				     (domain_p->last_vlan_p)->info_length );

    length = sizeof(subset_adv);

    /* process one VLAN info entry at a time */
    while ((length + VTP_MIN_VLAN_INFO_SIZE) <= total_length) {

	memcpy(dst_info_p, src_info_p, src_info_p->info_length);

        length += src_info_p->info_length;
        domain_p->update_vlan_length += src_info_p->info_length;
        domain_p->update_vlan_count++;
        domain_p->last_vlan_p = dst_info_p;
        src_info_p = (vlan_info_ptr)((unsigned char *)src_info_p + 
				     src_info_p->info_length);
        dst_info_p = (vlan_info_ptr)((unsigned char *)dst_info_p + 
				     dst_info_p->info_length);
    }

    domain_p->last_value_learned = 
                 vtp_format_2_bytes(domain_p->last_vlan_p->isl_vlan_id);

}


/* vtp_mark_database_old
 *
 * Called by the VTP platform-independant code to flag all existing
 * VLANs as OLD_VLAN and therefore to be deleted (if not modified
 * subsequently before deletion, see vtp_download_info in
 * vtp_interface.h) 
 */ 
void vtp_mark_database_old (vlan_domain_ptr domain_p) {

    vlan_info_ent_ptr ent_p;

    for (ent_p = domain_p->vlan_head; ent_p; ent_p = ent_p->isl_next) {
        ent_p->flags = OLD_VLAN; 
    }
}


/* vtp_delete_old_vlans
 *
 * Called by the VTP platform-independant code to remove all VLANs in
 * the domain VLAN list that are marked as OLD_VLAN.  Any VLANs not
 * marked old have their flag field cleared. 
 */
unsigned char vtp_delete_old_vlans (vlan_domain_ptr domain_p) {

    vlan_info_ent_ptr ent_p;

    for (ent_p = domain_p->vlan_head; ent_p; ) {

        if (ent_p->flags == OLD_VLAN) {
	    vtp_vlan_info v_info;
	    retrieve_vlan_info(ent_p, &v_info);
	    ent_p = ent_p->isl_next;  /* follow the pointer before we nuke
				   * it */
	    vtp_configure_vlan_in_database(domain_p, VTP_DELETE,
					   &v_info); 
	    vtp_process_reschedule();
        } else {
	    /* some clean up */
	    ent_p->flags = 0;
	    ent_p = ent_p->isl_next;
	}
    }
    
    return VTP_OK;
}


void vtp_cancel_transition (vlan_domain_ptr domain_p) {
    /* 
     * put domain back in null domain state if domain was transitioning
     * from null domain state.
     */
    if (domain_p->transitioning) {
	vtp_domain_info old_info, new_info;
	
	retrieve_domain_info(domain_p, &old_info);
	no_domain_state         = TRUE;
	domain_p->dname_length  = 0;
	domain_p->transitioning = FALSE;
	retrieve_domain_info(domain_p, &new_info);
	vtp_domain_change_notification(VTP_MODIFY, &old_info,
				       &new_info);
    }
}

/* vtp_process_subset_adv
 *
 * Called by the VTP platform-independant code when all subsets
 * requested have been received.  The routine stops the update pending
 * timer and checks the MD5 digest of the stored information.  If the
 * MD5 digest does not match an error count is incremented and a SNMP
 * trap generated (if enabled).  If this update is causing the device
 * to transition out of the no domain state and ends in a error then
 * the no domain state is re-established.  The stored subsets are then
 * changed into the vtp_download format and fed into the
 * vtp_download_info (found in vtp_interface.h).  After downloading
 * the stored subsets are released and updating fields are
 * initialized. 
 */

void vtp_process_subset_adv (vlan_domain_ptr domain_p,
			     vtp_trunk_itf_ptr trunk_p)
{
    void* mem;
    unsigned long count;
    vlan_info_ptr pkt_vlan_p;
    vtp_vlan_info* v_info_p;
    vtp_domain_info* d_info_p;

    /* stop the update pending timer, awaiting this subset packet */
    if (vtp_timer_active(domain_p->update_pending_timer)) {
        vtp_stop_timer(domain_p->update_pending_timer);
    }

    /* validate MD5 digest */
    if ( vtp_validate_md5_digest(domain_p, trunk_p) == FALSE ) {
        domain_p->acc.digest_error_count++;
        vtp_error_notification(domain_p->dname_length,
			       domain_p->domain_name,
			       VTP_ERROR_DIGEST_ERROR);
        VTP_LOG_RUNTIME("VTP: MD5 digest failing\n");
	vtpAPI_digest_error_detected(domain_p->domain_name,
				     domain_p->dname_length);
	vtp_cancel_transition(domain_p);
	vtp_flush_update_adv_info(domain_p);
	return;
    }

    mem = malloc(sizeof(vtp_domain_info) +
		 (domain_p->update_vlan_count *
		  sizeof(vtp_vlan_info)));
    if (!mem) {
	change_to_transparent(domain_p);
	vtp_error_notification(domain_p->dname_length,
			       domain_p->domain_name,
			       VTP_ERROR_INSERT_FAILURE); 
	vtp_cancel_transition(domain_p);
	vtp_flush_update_adv_info(domain_p);
	return;
    }

    d_info_p = (vtp_domain_info*)mem;
    retrieve_domain_info(domain_p, d_info_p);
    d_info_p->vlan_count = domain_p->update_vlan_count;
    d_info_p->revision = vtp_format_4_bytes(domain_p->next_revision);
    d_info_p->updater = vtp_format_4_bytes(domain_p->next_updater_id);
    d_info_p->pruning_mode = domain_p->next_pruning_mode;
    memcpy(d_info_p->timestamp, domain_p->next_update_timestamp,
	   VTP_TIMESTAMP_SIZE); 
    v_info_p = (vtp_vlan_info*)((unsigned char*)mem +
				sizeof(vtp_domain_info)); 

    count = domain_p->update_vlan_count;
    pkt_vlan_p = domain_p->first_vlan_p;
    while (count--) {
	unsigned char name_length_adjusted;
	unsigned long tlv_count;
	vlan_tlv_ptr tlv_p;

	v_info_p->vlan_name_length = pkt_vlan_p->name_length;
	memcpy(v_info_p->vlan_name, pkt_vlan_p->vlan_name,
	       pkt_vlan_p->name_length);
	v_info_p->type = pkt_vlan_p->type;
	v_info_p->state = pkt_vlan_p->status == VLAN_OPERATIONAL ?
	    VLAN_OPERATIONAL_STATE : VLAN_SUSPENDED_STATE;
	v_info_p->mtu = vtp_format_2_bytes(pkt_vlan_p->mtu);
	v_info_p->isl_vlan_id =
	    vtp_format_2_bytes(pkt_vlan_p->isl_vlan_id);
	v_info_p->said = vtp_format_4_bytes(pkt_vlan_p->said);
	v_info_p->ring_number = 0;
	v_info_p->bridge_number = 0;
	v_info_p->tb_vlan_1 = 0;
	v_info_p->tb_vlan_2 = 0;
	v_info_p->stp_type = 0;
	v_info_p->parent_vlan = 0;
	
	name_length_adjusted = pkt_vlan_p->name_length;
	name_length_adjusted =
	    ADJUST_LENGTH_LONG(name_length_adjusted);
	tlv_count = (pkt_vlan_p->info_length - sizeof(vlan_info) -
		     name_length_adjusted) / sizeof(vlan_tlv);

	tlv_p = (vlan_tlv_ptr)((unsigned char *)pkt_vlan_p +
			       sizeof(vlan_info) +
			       name_length_adjusted);
        while (tlv_count > 0)  {
	    switch (tlv_p->tag) {
	    case TAG_SR_RING:
		v_info_p->ring_number = vtp_format_2_bytes(tlv_p->value);
		break;
	    case TAG_SR_BRIDGE:
		v_info_p->bridge_number =
		    vtp_format_2_bytes(tlv_p->value);
		break;
	    case TAG_STP_TYPE:
		v_info_p->stp_type = vtp_format_2_bytes(tlv_p->value);
		break;
	    case TAG_PARENT_VLAN:
		v_info_p->parent_vlan = vtp_format_2_bytes(tlv_p->value);
		break;
	    case TAG_TB_VLAN:
		if (tlv_p->length == 1) {
		    v_info_p->tb_vlan_1 = vtp_format_2_bytes(tlv_p->value);
		} else {
		    unsigned short *tb_vlan_p;

		    tlv_p++;
		    tlv_count--;
		    tb_vlan_p = (unsigned short *)tlv_p;
		    v_info_p->tb_vlan_1 = vtp_format_2_bytes(*tb_vlan_p++);
		    v_info_p->tb_vlan_2 = vtp_format_2_bytes(*tb_vlan_p);
		}
		break;
	    default:
		VTP_LOG_RUNTIME("VTP: Received unknown TLV in"
				" update\r\n"); 
		tlv_p += tlv_p->length;
	    }
	    tlv_p++;
	    tlv_count--;
	}

	pkt_vlan_p = (vlan_info_ptr)((unsigned char *)pkt_vlan_p + 
                                                  pkt_vlan_p->info_length);
	v_info_p = (vtp_vlan_info*)((unsigned char *)v_info_p +
				    sizeof(vtp_vlan_info)); 
        vtp_process_reschedule();
    }

    /* free up the memory as soon as possible */
    vtp_flush_update_adv_info(domain_p);

    domain_p->update_trunk_p = trunk_p;
    vtp_download_info(TRUE, TRUE, mem);

    /* system successfully transitioned from null to valid domain */
    if (domain_p->transitioning) {
	/* start advertising timer on all trunk links on this domain */
	for (trunk_start_p = domain_p->trunk_head; 
	     trunk_start_p != NULL; trunk_start_p =
		 trunk_start_p->next_trunk_in_domain) { 
	    trunk_start_p->jittered_value =
		vtp_get_random_jittered_timer_value(); 
	    vtp_start_timer(trunk_start_p->advt_timer_p,
			    domain_p->advt_interval + 
			    trunk_start_p->jittered_value);
	}
	vtp_pruning_add_domain_init(domain_p);
	domain_p->transitioning = FALSE;
	vtp_pruning_add_domain(domain_p);
	VTP_PRUNING_LOG_NOTICE(LEARNDOMAIN, "%slearn domain %s from"
			       " network!", domain_p->domain_name);
    }
}



/* vtp_transmit_adv_request_packet
 *
 * Called by the VTP platform-independant code whenever an
 * advertisement request needs to be sent (as detailed in the subset
 * processing; when a update pending or trunk request timeout occur;
 * when a summary is received with a greater revision; when a trunk is
 * created; when a domain changes into a client).  The routine checks
 * to see if there is already a request outstanding that covers the
 * given request.  If so it returns doing nothing.  Otherwise it
 * formats an advertisement request with the appropriate starting
 * sequence value and transmit it out the specified trunk.  If no
 * packet buffers are available the routine simply drops the request.
 * When the request is transmitted the request timer on the trunk is
 * restarted. 
 */
void vtp_transmit_adv_request_packet (vtp_trunk_itf_ptr trunk_p,
				      unsigned short start_value)
{

   void *buff_p;
   adv_request_ptr adv_rqst_p;
   vlan_domain_ptr domain_p;

   domain_p = trunk_p->domain_p;

   /* build a request packet and send it */
   buff_p = vtp_get_buffer(sizeof(adv_request));  

   if (!buff_p) {
      VTP_LOG_RUNTIME("VTP: no buffer for tx adv request\n");
      return;
   }

   /* 
    * if there is already an outstanding request packet with
    *  smaller start value, do nothing. Otherwise, re-transmit
    *  the request packet with smaller start value.
    */
   if (vtp_timer_active(trunk_p->rqst_timer_p)) {
       if (start_value < trunk_p->rqst_start_value) {
	   vtp_stop_timer(trunk_p->rqst_timer_p);
	   trunk_p->rqst_start_value = start_value;
       } else {
	   return;
       }
   } else {
       trunk_p->rqst_start_value = start_value;
   }
   
   adv_rqst_p = vtp_buffer_payload( trunk_p->trunk_id, buff_p);
   
   adv_rqst_p->version      = VTP_VERSION;
   adv_rqst_p->code         = VTP_ADV_RQST;
   adv_rqst_p->reserved     = 0;
   adv_rqst_p->dname_length = domain_p->dname_length;

   memset(adv_rqst_p->domain_name, 0, VTP_MAX_DOMAIN_NAME_LENGTH);
   memcpy(adv_rqst_p->domain_name, domain_p->domain_name,
	  domain_p->dname_length); 

   adv_rqst_p->start_value = vtp_format_2_bytes(trunk_p->rqst_start_value);

   vtp_transmit_packet( trunk_p->trunk_id, buff_p, sizeof(adv_request) );

   VTP_LOG_RUNTIME("VTP: tx vtp request, domain %s, start value %d\n",
		   adv_rqst_p->domain_name,
		   adv_rqst_p->start_value);  

   domain_p->acc.tx_request_adv_count++;

   /* start request timer in anticapation of rcving summary & subset packets */
   vtp_start_timer( trunk_p->rqst_timer_p, 
		   VTP_REQUEST_INTERVAL); 

}



/* vtp_do_trunk_flooding
 *
 * Called by the platform-independant code to send summary (and
 * subset) advertisements as is required when the periodic timer
 * expires, new configuration is available or a request is received.
 * The routine will encode and transmit summary (and specified subset)
 * advertisements, restarting the summary advertisement timer at the
 * end. 
 */
void vtp_do_trunk_flooding (vtp_trunk_itf_ptr trunk_p, 
			    unsigned short start_value) {

    vlan_domain_ptr domain_p;
    vtp_boolean found;    

    domain_p = trunk_p->domain_p;

    /* dont transmit if disabled */
    if (domain_p->mode == VTP_TRANSPARENT)
	return;
    
    /* stop the timer if it is running */
    if (vtp_timer_active(trunk_p->advt_timer_p)) {
	vtp_stop_timer(trunk_p->advt_timer_p);
    }
    
    /* update the start value for ISL VLAN Id to transmit */
    if (start_value < trunk_p->advt_start_value)
       trunk_p->advt_start_value = start_value;

    found = FALSE;
    domain_p->tx_next_vlan_p = NULL;
    domain_p->tx_queue_count = 0;

    /* find the first VLAN info entry in the sorted list */
    for (domain_p->tx_next_vlan_p = domain_p->vlan_head;
	 domain_p->tx_next_vlan_p; 
	 domain_p->tx_next_vlan_p = domain_p->tx_next_vlan_p->isl_next) { 

         if (trunk_p->advt_start_value <=
	     domain_p->tx_next_vlan_p->isl_vlan_id) { 
             found = TRUE;
             break;
         }
     }
    
    /* build the subset advt if there is any VLAN to transmit */
    if (found) 
       vtp_generate_subset_adv_packet(domain_p, trunk_p);

    /* always send out summary advt */
    vtp_transmit_summary_adv_packet(trunk_p, 
				    domain_p->tx_queue_count); 

    /* send out subset advt if any */
    if (domain_p->tx_queue_count > 0) 
	vtp_transmit_subset_adv_packet(trunk_p);
    
    trunk_p->advt_start_value = SEND_NONE;
    trunk_p->jittered_value = vtp_get_random_jittered_timer_value();
    vtp_start_timer(trunk_p->advt_timer_p, domain_p->advt_interval + 
		    trunk_p->jittered_value);
    trunk_p->flood_timestamp = vtp_get_system_uptime();
}



/* vtp_transmit_summary_adv_packet
 *
 * Called by vtp_do_trunk_flooding to transmit a summary
 * advertisement.  If the routine cannot get a packet buffer the
 * summary is dropped.  After transmitting the packet the summary
 * advertisement counter is incremented. 
 */
void vtp_transmit_summary_adv_packet (vtp_trunk_itf_ptr trunk_p,
				      unsigned char count) {
    
    void *buff_p;
    summary_adv_ptr summary_adv_pkt_p;
    vlan_domain_ptr domain_p = trunk_p->domain_p;
    summary_pruning_tlv_ptr tlv_p;

    buff_p = vtp_get_buffer( sizeof(summary_adv) + SUMMARY_PRUNING_TLV_SIZ);

    if (!buff_p) {
       VTP_LOG_RUNTIME("VTP: no buffer to transmit summary packet\n");
       return;
    }

    summary_adv_pkt_p = vtp_buffer_payload( trunk_p->trunk_id, buff_p);

    summary_adv_pkt_p->version      = VTP_VERSION;
    summary_adv_pkt_p->code         = VTP_SUMMARY_ADV;
    summary_adv_pkt_p->followers    = count;
    summary_adv_pkt_p->dname_length = domain_p->dname_length;

    memset(summary_adv_pkt_p->domain_name, 0,
	   VTP_MAX_DOMAIN_NAME_LENGTH); 
    memcpy(summary_adv_pkt_p->domain_name, domain_p->domain_name,
	   domain_p->dname_length); 

    summary_adv_pkt_p->revision   = vtp_format_4_bytes(domain_p->revision);
    summary_adv_pkt_p->updater_id = vtp_format_4_bytes(domain_p->updater);

    memcpy(summary_adv_pkt_p->update_timestamp, domain_p->timestamp,
	   VTP_TIMESTAMP_SIZE );

    memcpy(summary_adv_pkt_p->md5_digest, domain_p->md5_digest,
	   VTP_MD5_DIGEST_SIZE); 

    /* append TLV for pruning support */
    tlv_p = (summary_pruning_tlv_ptr) (((unsigned char*)summary_adv_pkt_p) +
				       sizeof(summary_adv)); 
    tlv_p->tag = TAG_PRUNING_SUPPORT;
    tlv_p->length = TAG_PRUNING_SUPPORT_LEN;
    tlv_p->value  = domain_p->pruning_mode; /*for pruning_mode*/
    vtp_transmit_packet( trunk_p->trunk_id, buff_p,
			sizeof(summary_adv) + SUMMARY_PRUNING_TLV_SIZ);

    VTP_LOG_RUNTIME("VTP: tx vtp summary, domain %s, rev %d, followers"
		    " %d\n", summary_adv_pkt_p->domain_name,
		    summary_adv_pkt_p->revision, 
		    summary_adv_pkt_p->followers); 

    domain_p->acc.tx_summary_adv_count++;
 
}


/* 
 * vtp_add_vlan_to_subset: formats the memory pointed at by
 * pkt_vlan_info_p with the information from vlan_p, including any
 * TLVs (which are appended after the vlan_info structure area).
 * 'length' dictates how far into a packet buffer the current encoding
 * is and max_size indicates the buffer's size.  If adding the vlan to
 * the subset would result in surpassing the buffer limit then the
 * buffer is not altered and FALSE is returned.  Otherwise the length
 * is updated as appropriate.
 */
 
vtp_boolean vtp_add_vlan_to_subset (vlan_info_ptr pkt_vlan_info_p,
				unsigned short max_size,
				vlan_info_ent_ptr vlan_p, 
				unsigned long* length) { 
    unsigned char name_length;
    unsigned char info_length;
    unsigned char tlv_length; 
    vlan_tlv_ptr tlv_p;
    unsigned short *tb_vlan_p;

    /* 
     * fill in vlan info field if it will fit, otherwise return FALSE.
     */
    name_length = vlan_p->name_length;
    info_length = ADJUST_LENGTH_LONG(name_length) + sizeof(vlan_info);

    /* total bytes for all tlv fields for this vlan */
    tlv_length = 0;

    if (vlan_p->type != VLAN_ETHER)
	tlv_length += sizeof(vlan_tlv) + sizeof(vlan_tlv);  /* bytes */

    if (vlan_p->tb_vlan_1) {
	if (vlan_p->tb_vlan_2) {
              tlv_length += sizeof(vlan_tlv) + sizeof(vlan_tlv);  
	  } else {
              tlv_length += sizeof(vlan_tlv);  /* bytes */
	  }
    }

    if (*length + info_length + tlv_length > max_size)
	return FALSE;

    pkt_vlan_info_p->info_length = info_length + tlv_length; 
    pkt_vlan_info_p->status = vlan_p->status;
    pkt_vlan_info_p->type = vlan_p->type;
    pkt_vlan_info_p->name_length = vlan_p->name_length; 
    pkt_vlan_info_p->isl_vlan_id =
	vtp_format_2_bytes(vlan_p->isl_vlan_id);  
    pkt_vlan_info_p->mtu = vtp_format_2_bytes(vlan_p->mtu); 
    pkt_vlan_info_p->said = vtp_format_4_bytes(vlan_p->said); 
    memset(pkt_vlan_info_p->vlan_name, 0,
	   ADJUST_LENGTH_LONG(vlan_p->name_length));
    memcpy(pkt_vlan_info_p->vlan_name, vlan_p->vlan_name,
	   vlan_p->name_length); 
	
    /* append TLV fields if vlan type is not Ethernet */
    tlv_p = (vlan_tlv_ptr)((unsigned char *)pkt_vlan_info_p +
			   info_length); 
    if ( (vlan_p->type == VLAN_FDDI) || (vlan_p->type == VLAN_TR) ) { 
	tlv_p->tag = TAG_SR_RING;
	tlv_p->length = 1;
	tlv_p->value = 
	    vtp_format_2_bytes(vlan_p->ring_number);
	tlv_p++;
	tlv_p->tag = TAG_PARENT_VLAN;
	tlv_p->length = 1;
	tlv_p->value = vtp_format_2_bytes(vlan_p->parent_vlan); 
	tlv_p++;
    } else if ((vlan_p->type == VLAN_FDDI_NET) || 
	       (vlan_p->type == VLAN_TR_NET)) { 
	tlv_p->tag = TAG_SR_BRIDGE;
	tlv_p->length = 1;
	tlv_p->value = vtp_format_2_bytes(vlan_p->bridge_number);
	tlv_p++;
	tlv_p->tag = TAG_STP_TYPE;   
	tlv_p->length = 1;
	tlv_p->value = vtp_format_2_bytes(vlan_p->stp_type); 
	tlv_p++;
    }

    /* append TLV fields if vlan is translational bridged */
    if (vlan_p->tb_vlan_1)  {
	tlv_p->tag = TAG_TB_VLAN;
	if (vlan_p->tb_vlan_2) {
	    tlv_p->length = 2;
	    tlv_p->value = 0;
	    tlv_p++;
	    tb_vlan_p = (unsigned short *)tlv_p;
	    *tb_vlan_p = vtp_format_2_bytes(vlan_p->tb_vlan_1);
	    tb_vlan_p++;
	    *tb_vlan_p = vtp_format_2_bytes(vlan_p->tb_vlan_2);
	} else {
	    tlv_p->length = 1;
	    tlv_p->value = 
		vtp_format_2_bytes(vlan_p->tb_vlan_1);
	}
    }
	
    /* update the total packet length so far */   
    *length += pkt_vlan_info_p->info_length;
	
    return TRUE;
}

/* 
 * The following function fills a packet buffer with a VTP subset
 * advertisement, starting with the VLAN specified in the vlan_p
 * pointer.  The function returns the number of bytes actually put
 * into the packet, not to exceed the MTU for the given trunk.  Also
 * the vlan_p pointer is updated to the VLAN after the last VLAN
 * encoded in the packet.  If all VLANs can be fit into the packet
 * then the vlan_p is set to the beginning of the info_sorted_list.
 */
static unsigned long 
vtp_generate_one_subset (void* buff_p, vlan_domain_ptr domain_p,
			 vtp_trunk_itf_ptr trunk_p, 
			 unsigned char sequence, 
			 vlan_info_ent** vlan_p) { 

    subset_adv_ptr subset_adv_pkt_p;
    unsigned long length;
    vlan_info_ptr pkt_vlan_info_p;
    vtp_boolean can_fit_more;
    
    can_fit_more = TRUE;

    /* build subset header */
    subset_adv_pkt_p = vtp_buffer_payload(trunk_p->trunk_id, buff_p);
         
    subset_adv_pkt_p->version = VTP_VERSION;
    subset_adv_pkt_p->code    = VTP_SUBSET_ADV;
    subset_adv_pkt_p->sequence = sequence;

    subset_adv_pkt_p->dname_length = domain_p->dname_length;
    memset(subset_adv_pkt_p->domain_name, 0,
	   VTP_MAX_DOMAIN_NAME_LENGTH); 
    memcpy(subset_adv_pkt_p->domain_name, domain_p->domain_name,
	   domain_p->dname_length); 
    subset_adv_pkt_p->revision = vtp_format_4_bytes(domain_p->revision);

    length = sizeof(subset_adv);
    pkt_vlan_info_p = (vlan_info_ptr)
	((unsigned char *)subset_adv_pkt_p + sizeof(subset_adv) );
	
    while (can_fit_more && *vlan_p) {
	can_fit_more = vtp_add_vlan_to_subset(pkt_vlan_info_p,
					      trunk_p->mtu, *vlan_p,
					      &length); 
	pkt_vlan_info_p = (vlan_info_ptr) 
	    ((unsigned char *)pkt_vlan_info_p +
	     pkt_vlan_info_p->info_length);  
	*vlan_p = (*vlan_p)->isl_next;
    }
    return length;
}

/* 
 * Called by vtp_generate_subset_adv_packet to abort sending a stream
 *  of subset packets.
 */

void vtp_cancel_subset_transmission (vlan_domain_ptr domain_p) {

    unsigned long ix;

    for (ix = 0; ix < domain_p->tx_queue_count; ix++) {
	vtp_free_buffer(domain_p->tx_buff_queue[ix].buff_p);
    }
}

/* vtp_generate_subset_adv_packet
 *
 * Called by vtp_do_trunk_flooding to generate subset advertisements
 * and put them in a transmit queue.  The routine allocates a packet
 * buffer and fills it with the VLAN configuration from the domain,
 * starting with the VLAN ID specified in the vlan_do_trunk_flooding
 * function.  If the configuration will not fit in one packet it
 * allocates more as needed.  All compelete packets are put into a
 * transmit queue.  If a packet buffer is not available, processing
 * stops and the function returns without cleaning up. 
 */
void vtp_generate_subset_adv_packet (vlan_domain_ptr domain_p,
				     vtp_trunk_itf_ptr trunk_p) {
    void* buff_p;
    vlan_info_ent_ptr vlan_p;
    unsigned long length;
    unsigned char sequence;

    sequence = 1;

    for (vlan_p = domain_p->vlan_head; vlan_p; ) {
	if (domain_p->tx_queue_count == MAX_VTP_TX_QUEUE_SIZE) {
	    VTP_LOG_RUNTIME("VTP: Transmit queue size"
			    " exceeded.\r\n"); 
	    vtp_cancel_subset_transmission(domain_p);
	    return;
	}
	buff_p = vtp_get_buffer(trunk_p->mtu); 
	if (!buff_p) {
	    vtp_cancel_subset_transmission(domain_p);
	    VTP_LOG_RUNTIME("VTP: Cannot get buffer for subset"
			    " transmission\r\n");
	    return;
	}
	length = vtp_generate_one_subset(buff_p, domain_p,
					 trunk_p, sequence++,
					 &vlan_p); 
	domain_p->tx_buff_queue[domain_p->tx_queue_count].buff_p =
	    buff_p; 
	domain_p->tx_buff_queue[domain_p->tx_queue_count].length =
	    length; 
	domain_p->tx_queue_count++;
	vtp_process_reschedule();
    }
}



/* vtp_transmit_subset_adv_packet
 *
 * Called by vtp_do_trunk_flooding to transmit all subset
 * advertisements in the transmit queue.  After each subset is
 * transmitted the subset advertisement counter is incremented.  After
 * completion of all transmissions the queue count is reset to zero. 
 */
void vtp_transmit_subset_adv_packet (vtp_trunk_itf_ptr trunk_p)
{

   unsigned long ix;
   buff_chain_ptr buff_p;
   vlan_domain_ptr domain_p = trunk_p->domain_p;


   for (ix = 0, buff_p = &(domain_p->tx_buff_queue[0]); 
                  ix < domain_p->tx_queue_count; ix++, buff_p++) {

       vtp_transmit_packet(trunk_p->trunk_id, buff_p->buff_p, buff_p->length);

       buff_p->buff_p = NULL;
       buff_p->length = 0;       

       domain_p->acc.tx_subset_adv_count++;
   }

   domain_p->tx_queue_count = 0;

}



/*
 * Handle global config updating timeout event.
 * This routine is invoked upon expiry of update_pending timer which was
 * started upon receipt of summary packet and awaiting subset(s) packet.
 * Only one retry of a request packet is allowed
 */
void vtp_config_updating_timeout (void *context)
{

   vtp_trunk_itf_ptr trunk_p;
   vlan_domain_ptr domain_p;


   domain_p = (vlan_domain_ptr)context;
   trunk_p  = (vtp_trunk_itf_ptr)(domain_p->update_trunk_p);

   /* allow only one timeout i.e. send only one vtp request on timer expiry */
   if (domain_p->flags & UPDATE_RXMT_REQUESTED) {
      domain_p->flags &= ~UPDATE_RXMT_REQUESTED;
      vtp_flush_update_adv_info(domain_p);
   } else if (trunk_p) { 
       /* request the next needed VLAN (last learned vlan id plus one)
	*/
       vtp_transmit_adv_request_packet(trunk_p,
				       domain_p->last_value_learned
				       + 1); 
       /* 
	* flag set to indicate that a vtp request was sent on expiry of
	* update pending timer.
	*/
       domain_p->flags |= UPDATE_RXMT_REQUESTED;
   }
}



/*
 * Handle trunk link rxmt timeout event.  This routine is invoked when a
 * jitter timer expires on receiving a vtp request.
 */
void vtp_trunk_link_flood_timeout (void *context)
{

    vtp_trunk_itf_ptr trunk_p;


    trunk_p = (vtp_trunk_itf_ptr)context;

    /* only need to send the summary advt, no subset needed */
    if (trunk_p->domain_p) 
       vtp_do_trunk_flooding(trunk_p, SEND_NONE);

}


/* vtp_trunk_list_rqst_timeout
 *
 * Called when a advertisement request has been outstanding for longer
 * than alotted.  The routine retries the request multiple times
 * before giving up and notifying the dependant code of the failure 
 */
void vtp_trunk_link_rqst_timeout (vtp_trunk_itf_ptr trunk_p)
{
    /* try retransmitting vtp request if retry count not exceeded */
    if (++trunk_p->rqst_retry_count < VTP_MAX_RQST_RETRY_COUNT) {
	vtp_stop_timer(trunk_p->rqst_timer_p);
	vtp_transmit_adv_request_packet(trunk_p,
					trunk_p->rqst_start_value);
    } else {
       /* retry limit exceeded - need to take some corrective action here */

       if (vtp_timer_active(trunk_p->rqst_timer_p)) {
          vtp_stop_timer(trunk_p->rqst_timer_p);
      }

       trunk_p->rqst_retry_count = 0;
       /* log this error condition */

       /* notify platform - no response to vtp request */
       vtp_no_request_response();

    }

}


/* This function is called by the dependant code during the
 * vtp_add/change_domain_notification.  It is used to indicate that
 * TFTP loading of the specified database was unsuccessful.  Hence the
 * domain should be changed to the client mode. 
 */

void vtp_report_load_failure (unsigned char* domain_name, 
			      unsigned char dname_length) {
    vlan_domain_ptr domain_p;

    domain_p = vtp_search_domain(dname_length, domain_name);
    if (!domain_p)
	return;

    VTP_LOG_ERROR("Failed to load TFTP file");
    change_to_client(domain_p);
}

/* vtp_check_domain_info
 *
 * Called by functions modifying domain information.  The routine does
 * some sanity checks on provided domain information (like name and
 * mode). The routine returns a specific error code upon failure,
 * VTP_OK otherwise 
 */
unsigned char vtp_check_domain_info (vtp_domain_info* domain_p) {
    
    /* A 'special' to allow modifications to the null domain state
     * database. 
     */

    if ((no_domain_state) && (domain_p->domain_name_length == 0)) 
	return VTP_OK; 

    if ((domain_p->domain_name_length < 1) ||
	(domain_p->domain_name_length > VTP_MAX_DOMAIN_NAME_LENGTH)) { 
	VTP_LOG_RUNTIME("VTP: illegal domain name length\n"); 
	return VTP_ERROR_DOMAIN_NAME_LENGTH;
    }

    if (!vtp_is_print(domain_p->domain_name,
		      domain_p->domain_name_length)) {
	VTP_LOG_RUNTIME("VTP: domain name contains unprintable characters\n"); 
	return VTP_ERROR_DOMAIN_NAME_UNPRINT;
    }
    
    /* This should be the last check, add more checks above this one
     */
    switch (domain_p->mode) {
    case VTP_CLIENT:
    case VTP_SERVER:
    case VTP_TRANSPARENT:
	break;
    default:
	VTP_LOG_RUNTIME("VTP: illegal mode value in domain\n"); 
	return VTP_ERROR_MODE_UNKNOWN;
    }

    if (!is_valid_pruning_mode(domain_p->pruning_mode)) {
	VTP_PRUNING_LOG_WARNING(INVPMODE, "%sinv"
				" pruning_mode(%x)(add)",
				domain_p->pruning_mode);
	return VTP_ERROR_PRUNING_MODE_UNKNOWN;
    }

    return VTP_OK;
}

/* Called by dependant code to set the update_id for use with the
 * vtp_configure_vlan function call.
 */
Vtp_Err_t vtp_set_update_id (unsigned char dname_length, 
			     unsigned char* domain_name,
			     unsigned long update_id) {
  vlan_domain_ptr domain_p;

  domain_p = vtp_search_domain(dname_length, domain_name);
  if (!domain_p)
    return VTP_ERROR_NO_DOMAIN;

  domain_p->my_update_id = update_id;

  return VTP_OK;
}

/* Called by vtp_configure_domain to either add or modify an existing
 * domain's values. 
 */
void vtp_update_domain_info (vlan_domain_ptr domain_p,
			     vtp_domain_info* d_info) {

    domain_p->dname_length = d_info->domain_name_length;
    memcpy(domain_p->domain_name, d_info->domain_name,
	     d_info->domain_name_length); 
    domain_p->domain_name[domain_p->dname_length] = '\0';
    
    if (d_info->index)
	domain_p->index = d_info->index;
    
    domain_p->mode     = d_info->mode;          
    domain_p->revision = d_info->revision;
    domain_p->updater  = d_info->updater;

    memcpy(domain_p->timestamp, d_info->timestamp,
	   VTP_TIMESTAMP_SIZE); 

    /* only generated a new secret if the password is changing, saves
     * on CPU time
     */
    if ((d_info->password_length != domain_p->password_length) ||
	(memcmp(domain_p->password, d_info->password,
		d_info->password_length) != 0)) {
	vtp_get_secret_value(d_info->password_length, d_info->password,
			     domain_p->secret_value);    
    }
 
    domain_p->password_length = d_info->password_length;
    memcpy(domain_p->password, d_info->password,
	     d_info->password_length); 
    domain_p->password[domain_p->password_length] = '\0';

    /* vtp_domain_info advertisement interval is in seconds while the
     * internal units is milliseconds.  Must multiply by 1000 to
     * convert.
     */
    domain_p->advt_interval = d_info->advt_interval * 1000; 
    vtp_generate_md5_digest(domain_p);

    /* unlike other ASCII names, the TFTP filename is assumed to be
     * NULL terminated.  Hence the NULL is copied into the internal
     * field instead of setting it on the end. 
     */
    memcpy(domain_p->tftp_file_name, d_info->tftp_file_name,
	     VTP_MAX_TFTP_FILENAME_LENGTH + 1);
    domain_p->tftp_server = d_info->tftp_server;

    domain_p->pruning_mode = d_info->pruning_mode;
}

/* This function is available to the external API.  Called by SNMP and
 *  CLI to add/modify/delete domains. 
 */
Vtp_Err_t vtp_configure_domain (Vtp_Op_t mode,
				vtp_domain_info* d_info) {
    
    vlan_domain_ptr domain_p, action_point;
    long cmp;
    unsigned char error;
    vtp_domain_info old_values, new_values;
    vtp_trunk_itf_ptr trunk_p;
    vlan_info_ent_ptr vlan_p;
    
    domain_p = NULL;
    cmp = NAME_GREATER;
    action_point = NULL;

    error = vtp_check_domain_info(d_info);
    if (error) 
	/* An exemption for deleting a domain, don't mandate that the
	 * API user fill in a legal mode 
	 */
	if (error != VTP_ERROR_MODE_UNKNOWN)
	    return error;
	else if (mode != VTP_DELETE)
	    return error;
    
    /* see if the domain name or index already exists */
    for (domain_p = vlan_domain_head; domain_p; domain_p =
	 domain_p->domain_next) { 
	cmp = name_cmp(d_info->domain_name, domain_p->domain_name,
		       d_info->domain_name_length,
		       domain_p->dname_length);
	if (cmp != NAME_GREATER)
	    break;
	action_point = domain_p;
    }
    
    switch (mode) {
    case VTP_ADD:
	
	if (cmp == NAME_EQUAL) {
	    VTP_LOG_RUNTIME("VTP: add_domain - domain %s already"
			    " exists\n", domain_p->domain_name);
	    return VTP_ERROR_DOMAIN_NAME_DUP;
	}
	/* 
	 * if is in no domain state and the new domain is not a default
	 *  domain (null domain name), simply overwrites the default
	 *  domain, which already exists.
	 */
	if ((no_domain_state == TRUE) &&
	    (d_info->domain_name_length > 0) &&
	    (vlan_domain_head)) {
	    
	    domain_p = vlan_domain_head;
	    vtp_update_domain_info(domain_p, d_info);
	    vtp_pruning_add_domain(domain_p);
	    no_domain_state = FALSE;
	} else {
	    /* add new domain */
	    domain_p = malloc(sizeof(vlan_domain));
	    
	    if (!domain_p) {
		VTP_LOG_RUNTIME("VTP: add_domain - no memory\n");
		return VTP_ERROR_INSERT_FAILURE;
	    }
	    
	    memset(domain_p, 0, sizeof(vlan_domain));
	    domain_p->update_pending_timer =
		vtp_create_timer(vtp_config_updating_timeout,  
				 (void *)domain_p); 
	    if (!d_info->index)
		d_info->index = vtp_get_index();
	    
	    domain_p->domain_next = NULL;
	    domain_p->trunk_head = NULL;
	    domain_p->vlan_head = NULL;
	    domain_p->vlan_backup_head = NULL;
	    
	    if (!vlan_domain_head) { 	    /* first domain */
		vlan_domain_head = domain_p;
	    } else if (!action_point) {     /* in front of head */
		domain_p->domain_next = vlan_domain_head;
		vlan_domain_head = domain_p;
	    } else {                       /* middle to end */
		domain_p->domain_next = action_point->domain_next;
		action_point->domain_next = domain_p;
	    }

	    vtp_update_domain_info(domain_p, d_info);
	    
	    /*
	     * add factory-default VLANs to receive VTP packets.
	     */
	    vtp_pruning_add_domain_init(domain_p);
	    vtp_add_default_vlans(domain_p);
	    vtp_pruning_add_domain(domain_p);
	}
	retrieve_domain_info(domain_p, &new_values);
	break;
    case VTP_MODIFY:
	if (cmp != NAME_EQUAL) {
	    VTP_LOG_RUNTIME("VTP: change_domain - domain %s does not"
			    " exist\n", d_info->domain_name);  
	    return VTP_ERROR_NO_DOMAIN;
	}
	
	retrieve_domain_info(domain_p, &old_values);
	vtp_update_domain_info(domain_p, d_info);

	if (old_values.mode != domain_p->mode) {
	    switch (domain_p->mode) {
	    case VTP_SERVER:
		vtp_change_to_server(domain_p);
		if ((old_values.mode == VTP_TRANSPARENT) &&
		    (is_pruning_mode_enable(domain_p))) {
		    vtp_pruning_change_to_non_transparent(domain_p);
		}
		break;
	    case VTP_CLIENT:
		vtp_change_to_client(domain_p);
		if ((old_values.mode == VTP_TRANSPARENT) &&
		    (is_pruning_mode_enable(domain_p))) {
		    vtp_pruning_change_to_non_transparent(domain_p);
		}
		break;
	    case VTP_TRANSPARENT:
		vtp_change_to_transparent(domain_p);
		if (is_pruning_mode_enable(domain_p)) {
		    vtp_pruning_change_to_transparent(domain_p);
		}
		break;
	    }
	} else if ((domain_p->mode != VTP_TRANSPARENT) &&
		   (old_values.pruning_mode !=
		    domain_p->pruning_mode)) {
	    if (domain_p->pruning_mode) {
		vtp_pruning_change_to_non_transparent(domain_p);
	    } else {
		vtp_pruning_change_to_transparent(domain_p);
	    }
	}
	retrieve_domain_info(domain_p, &new_values);
	break;
    case VTP_DELETE:
	if (cmp != NAME_EQUAL) {
	    VTP_LOG_RUNTIME("VTP: delete_domain - domain %s does not"
			    " exist\n", d_info->domain_name);   
	    return VTP_ERROR_NO_DOMAIN;
	}
	
	retrieve_domain_info(domain_p, &old_values);
	/* delete all trunk links associated with this domain */
	for (trunk_p = domain_p->trunk_head; trunk_p; ) {
	    vtp_trunk_info t_info;
	    
	    retrieve_trunk_info(trunk_p, &t_info);
	    trunk_p = trunk_p->next_trunk_in_domain;  
	    vtp_configure_trunk(VTP_DELETE, &t_info);
	}

	for (vlan_p = domain_p->vlan_head; vlan_p; ) {
	    vtp_vlan_info v_info;
	    
	    retrieve_vlan_info(vlan_p, &v_info);
	    vlan_p = vlan_p->isl_next;  /* move pointer before structure
					 * freed */
	    vtp_configure_vlan_in_database(domain_p, VTP_DELETE,
					   &v_info); 
	    vtp_vlan_change_notification(VTP_DELETE,
					 domain_p->dname_length,
					 domain_p->domain_name,
					 &v_info, NULL);
	    vtp_process_reschedule();
	}
	
	/* flush the update data structures */
	vtp_flush_update_adv_info(domain_p);
	
	/* flush the transmit packet queue */
	vtp_flush_tx_subset_adv_queue(domain_p);
	
	vtp_delete_timer(domain_p->update_pending_timer);
	
	/* de-link from the domain list and free the resouce */

	vtp_pruning_delete_domain(domain_p); 

	if (!action_point) {              /* deleting head */
	    vlan_domain_head = domain_p->domain_next;
	} else {                          /* deleting middle to tail */
	    action_point->domain_next = domain_p->domain_next;
	}

	free(domain_p );
    }
    
    if (vtp_domain_change_notification(mode, &old_values,
				       &new_values)) {
	change_to_client(domain_p);
	vtpAPI_storage_exceeded(domain_p->domain_name,
				domain_p->dname_length);
    }
    
    return VTP_OK;
}


/* This function can cause a request to be generated if the domain
 * transitions from the no domain state.
 */
Vtp_Err_t vtp_change_domain_name (unsigned char old_length,
				  unsigned char* old_name, 
				  unsigned char new_length,
				  unsigned char* new_name) {
    vlan_domain_ptr domain_p, index_p, insert_point;
    vtp_domain_info old_values, new_values;
    long cmp;

    index_p = NULL;

    for (domain_p = vlan_domain_head;
	 domain_p; domain_p = domain_p->domain_next) {

	cmp = name_cmp(old_name, domain_p->domain_name,
		       old_length, domain_p->dname_length);
	if (cmp == NAME_EQUAL)    
	    break;
	index_p = domain_p;
    }

    if (!domain_p)
	return VTP_ERROR_NO_DOMAIN;

    retrieve_domain_info(domain_p, &old_values);

    if (!index_p) {                   /* deleting head */
	vlan_domain_head = domain_p->domain_next;
    } else {                          /* deleting middle to tail */
	index_p->domain_next = domain_p->domain_next;
    }

    domain_p->dname_length = new_length;
    memcpy(domain_p->domain_name, new_name, new_length);

    /* don't need to do this for null domain */
    if (old_length) {
	insert_point = NULL;

	/* re-link */
	for (index_p = vlan_domain_head;
	     index_p; index_p = index_p->domain_next)  {
	    
	    cmp = name_cmp(new_name, index_p->domain_name,
			   new_length, index_p->dname_length);
	    if (cmp == NAME_LESS)    
		break;
	    insert_point = index_p;
	}

	if (!insert_point) {     /* in front of head */
	    domain_p->domain_next = vlan_domain_head;
	    vlan_domain_head = domain_p;
	} else {                       /* middle to end */
	    domain_p->domain_next = insert_point->domain_next;
	    insert_point->domain_next = domain_p;
	}
	
	domain_p->revision = 0;
	vtp_generate_md5_digest(domain_p);
    } else {
	no_domain_state = FALSE;
	vlan_domain_head = domain_p;
	vtp_generate_md5_digest(domain_p);
	vtp_do_flooding(domain_p, 0, 0);
	vtp_pruning_add_domain(domain_p);
    }

    retrieve_domain_info(domain_p, &new_values);
    if (vtp_domain_change_notification(VTP_MODIFY, &old_values,
				       &new_values)) {
      change_to_client(domain_p);
      vtpAPI_storage_exceeded(domain_p->domain_name,
			      domain_p->dname_length);
    }

    return VTP_OK;
}

/* vtp_precheck_vlan
 *
 * Called from within vtp_download_info to verify that a VLANs values
 * are within specified ranges before adding them to the domain's VLAN
 * list. The routine returns a specific error result if the check
 * fails, VTP_OK otherwise.  The failure codes are found in
 * vtp_interface.h 
 */
unsigned char vtp_precheck_vlan (vtp_vlan_info* v_info) {

    if ((v_info->vlan_name_length < 1) || (v_info->vlan_name_length > 32)) {
	VTP_LOG_RUNTIME("VTP: illegal VLAN name length\n"); 
	return VTP_ERROR_DOMAIN_NAME_LENGTH;
    }
    
    if (!vtp_is_print(v_info->vlan_name, v_info->vlan_name_length)) { 
	VTP_LOG_RUNTIME("VTP: vlan name contains unprintable characters\n"); 
	return VTP_ERROR_DOMAIN_NAME_UNPRINT;
    }

    switch (v_info->type) {
    case VLAN_ETH_TYPE:
    case VLAN_FDDI_TYPE:
    case VLAN_TR_TYPE:
    case VLAN_FDDI_NET_TYPE:
    case VLAN_TR_NET_TYPE:
	break;
    default:
	VTP_LOG_RUNTIME("VTP: illegal VLAN type\n");
	return VTP_ERROR_VLAN_BADTYPE;
    }

    switch (v_info->state) {
    case VLAN_OPERATIONAL_STATE:
    case VLAN_SUSPENDED_STATE:
    case VLAN_MTU_TOO_BIG_FOR_DEVICE_STATE:
    case VLAN_MTU_TOO_BIG_FOR_TRUNK_STATE:
	break;
    default:
	VTP_LOG_RUNTIME("VTP: illegal VLAN state\n");
	return VTP_ERROR_VLAN_BADSTATE;
    }

    if ((v_info->mtu < VLAN_MIN_MTU) || (v_info->mtu > VLAN_MAX_MTU))
	{ 
	    VTP_LOG_RUNTIME("VTP: illegal VLAN MTU size\n");
	    return VTP_ERROR_VLAN_BADMTU;
	}

    if ((v_info->isl_vlan_id < 1) || (v_info->isl_vlan_id >
				      VTP_MAXIMUM_VLAN_ISL)) { 
	VTP_LOG_RUNTIME("VTP: illegal VLAN ID\n");
	return VTP_ERROR_VLAN_BADID;
    }

    if ((v_info->said < 1) || (v_info->said > VTP_MAXIMUM_VLAN_SAID)) {
	VTP_LOG_RUNTIME("VTP: illegal VLAN SAID\n");
	return VTP_ERROR_VLAN_BADSAID;
    }

    if (v_info->ring_number > VTP_MAXIMUM_VLAN_RING_NUMBER) {
	VTP_LOG_RUNTIME("VTP: illegal VLAN ring number\n");
	return VTP_ERROR_VLAN_BADRING;
    }

    if (v_info->bridge_number > VTP_MAXIMUM_VLAN_BRIDGE_NUMBER) {
	VTP_LOG_RUNTIME("VTP: illegal VLAN bridge number\n");
	return VTP_ERROR_VLAN_BADBRIDGE;
    }

    switch (v_info->stp_type) {
    case VTP_VLAN_NO_STP_TYPE:
    case VLAN_STP_IBM:
    case VLAN_STP_IEEE:
	break;
    default:
	VTP_LOG_RUNTIME("VTP: illegal VLAN spanning tree type\n");
	return VTP_ERROR_VLAN_BADSTPTYPE;
    }

    if (v_info->parent_vlan > VTP_MAXIMUM_VLAN_ISL) {
	VTP_LOG_RUNTIME("VTP: illegal VLAN parent\n");
	return VTP_ERROR_VLAN_BADPARENT;
    }

    if ((v_info->tb_vlan_1 > VTP_MAXIMUM_VLAN_ISL) ||
	(v_info->tb_vlan_2 > VTP_MAXIMUM_VLAN_ISL)) { 
	VTP_LOG_RUNTIME("VTP: illegal VLAN translation value\n");
	return VTP_ERROR_VLAN_BADTRANSLATION;
    }

    return VTP_OK;
}

/* Called by vtp_test_vlan to insure that a new or modified VLANs
 * values do not conflict with other references within the VLAN
 * database. 
 */
static unsigned char vtp_verify_vlan_values (vlan_domain_ptr domain_p,
					     vlan_info_ent_ptr ent_p,
					     vtp_vlan_info* v_info) { 
    vlan_info_ent_ptr test_p;
    vlan_info_ent_ptr tb_vlan1_p; 
    vlan_info_ent_ptr tb_vlan2_p;

    tb_vlan1_p = NULL;
    tb_vlan2_p = NULL;

    /* check uniqueness of vlan name */
    test_p = vtp_search_info_entry_by_name(domain_p,
					   v_info->vlan_name_length, 
					   v_info->vlan_name);
    if ((test_p) && (test_p != ent_p)) { 
	VTP_LOG_ERROR("Duplicate VLAN name\n");  
	return VTP_ERROR_DUPLICATE_VLAN_NAME;
    }
	
    /* check uniqueness of vlan dot10 said */
    test_p = vtp_search_info_entry_by_said(domain_p,
					   v_info->said);
    if ((test_p) && (test_p != ent_p)) { 
	    VTP_LOG_ERROR("Duplicate SAID value\n");  
	    return VTP_ERROR_DUPLICATE_VLAN_SAID;
	}

    /* 
     * tb_vlan_1 must exist, be of different type & must not have any
     * translational bridge entries before cli cmd accepted.
     */ 
    
    if (v_info->tb_vlan_1) {

	/* tb_vlan_1 exists? */
	tb_vlan1_p = vtp_search_info_entry_by_vlan(domain_p,
						   v_info->tb_vlan_1);

	if (!tb_vlan1_p) {
	    VTP_LOG_ERROR("Invalid translation VLAN\n");
	    return VTP_ERROR_UNKNOWN_TLB;
	}
       
	/* similar type of vlans? */
	if (tb_vlan1_p->type == v_info->type) {
	    VTP_LOG_ERROR("Translation VLAN must be of different"
			  " media type\n");
	    return VTP_ERROR_SIMILAR_TLB;
	} else {
	    VTP_LOG_RUNTIME("VTP: dissimilar type of translational"
			    " bridge\n"); 
	}

	if ((tb_vlan1_p->tb_vlan_1 != v_info->isl_vlan_id) &&
	    (tb_vlan1_p->tb_vlan_2 != v_info->isl_vlan_id)) {
	    VTP_LOG_ERROR("VLAN translation inconsistent\n"); 
	    return VTP_ERROR_INCONSISTENT_VLAN;
	}
    }
    /* 
     * tb_vlan_2 must exist, be of different type & must not have a similar
     * type of translational bridge entry before cli cmd to be accepted 
     */ 
    if (v_info->tb_vlan_2) {
	
	/* tb_vlan_2 exists? */
	tb_vlan2_p = vtp_search_info_entry_by_vlan(domain_p,
						   v_info->tb_vlan_2);
	
	if (!tb_vlan2_p) {
	    VTP_LOG_ERROR("Invalid Translation VLAN\n");
	    return VTP_ERROR_UNKNOWN_TLB;
	}
	
	/* similar type of vlans? */
	if (tb_vlan2_p->type == v_info->type) {
	    VTP_LOG_ERROR("Translation VLAN must be of different"
			  " media type\n");
	    return VTP_ERROR_SIMILAR_TLB;
	} else {
	    VTP_LOG_RUNTIME("VTP: dissimilar type of translational"
			    " bridge\n"); 
	}

	if ((tb_vlan2_p->tb_vlan_1 != v_info->isl_vlan_id) &&
	    (tb_vlan2_p->tb_vlan_2 != v_info->isl_vlan_id)) {
	    VTP_LOG_ERROR("VLAN translation inconsistent\n"); 
	    return VTP_ERROR_INCONSISTENT_VLAN;
	}
    }

    /* tb_vlan1 & tb_vlan2 must also be of different type */
    if ( v_info->tb_vlan_1  && v_info->tb_vlan_2  && 
	tb_vlan1_p && tb_vlan2_p &&
	(tb_vlan1_p->type == tb_vlan2_p->type) ) {
	VTP_LOG_ERROR("Translation VLANs must be of different"
		      " media types\n");
	return VTP_ERROR_SIMILAR_TLB;
    }

    return VTP_OK;
}

/* vtp_test_vlan
 *
 * Called from the bulk change vtp_download_info and the single-shot
 * vtp_configure_vlan, this routine insures that a VLAN modification
 * is legal and consistent with the result of the domain VLAN list.
 * The routine returns a specific error code on failure, VTP_OK
 * otherwise.  The failure codes are found in vtp_interface.h 
 */
unsigned char vtp_test_vlan (vlan_domain_ptr domain_p,
			     unsigned char mode, 
			     vtp_vlan_info* v_info) {
    vlan_info_ent_ptr ent_p;

    ent_p = NULL;

    switch (mode) {
    case VTP_ADD:
	/* check uniqueness of vlan id */
	ent_p = vtp_search_info_entry_by_vlan(domain_p,
					      v_info->isl_vlan_id);
	if (ent_p && ent_p->flags != NEW_VLAN) {  
	    VTP_LOG_ERROR("Duplicate VLAN ID\n");
	    return VTP_ERROR_DUPLICATE_VLAN_ID;
	}
	return vtp_verify_vlan_values(domain_p, ent_p, v_info); 
    case VTP_MODIFY:
	/* vlan exists? */
	ent_p = vtp_search_info_entry_by_vlan(domain_p,
					      v_info->isl_vlan_id); 
	if (!ent_p) {
	    /* magic number 26: It's just how much space the
	     * message needs when formatted. 
	     */
	    char msg_buf[26];
	    sprintf(msg_buf, "Vlan %d does not exist\n",
		    v_info->isl_vlan_id); 
	    VTP_LOG_ERROR(msg_buf); 
	    return VTP_ERROR_NO_VLAN;
	}
	
	/* default vlan? */
	if ( vtp_is_default_vlan_id(ent_p->isl_vlan_id) ) {
	    
	    /* cannot change name of default vlans */
	    if ((name_cmp(ent_p->vlan_name, v_info->vlan_name,
			  ent_p->name_length,
			  v_info->vlan_name_length)) !=
		NAME_EQUAL) { 
		VTP_LOG_ERROR("Cannot change the name of"
			      " a default VLAN\n"); 
		return VTP_ERROR_DEFAULT_VLAN_NAME_CHANGE;
	    }
	    
	    /* cannot change type of default vlans */
	    if (ent_p->type != v_info->type) {
		VTP_LOG_ERROR("Cannot change the type of a"
			      " default VLAN\n"); 
		return VTP_ERROR_DEFAULT_VLAN_TYPE_CHANGE;
	    }
	    
	    /* cannot chage mtu of default vlans */
	    if (ent_p->mtu != v_info->mtu) {
		VTP_LOG_ERROR("Cannot change the MTU of a"
			      " default VLAN\n"); 
		return VTP_ERROR_DEFAULT_VLAN_MTU_CHANGE;
	    }
	    
	    /* cannot change state of default vlans */
	    if (v_info->state == VLAN_SUSPENDED_STATE) {
		VTP_LOG_ERROR("Cannot change the state of a"
			      " default VLAN\n"); 
		return VTP_ERROR_DEFAULT_VLAN_STATE_CHANGE;
	    }
	}
	return vtp_verify_vlan_values(domain_p, ent_p, v_info);
    case VTP_DELETE:
	ent_p = vtp_search_info_entry_by_vlan(domain_p,
					      v_info->isl_vlan_id); 
	if (!ent_p) {
	    VTP_LOG_RUNTIME("VTP: VLAN %d does not exist\n",
			    v_info->isl_vlan_id); 
	    return VTP_ERROR_NO_VLAN;
	}

	/* default vlan? */
	if ( vtp_is_default_vlan_id(ent_p->isl_vlan_id) ) {
	    VTP_LOG_ERROR("Cannot delete a default VLAN\n");
	    return VTP_ERROR_DEFAULT_VLAN;
	}
    }
    return VTP_OK;
}

void vtp_update_vlan_info (vlan_info_ent_ptr vlan_info_p,  
			   vtp_vlan_info* v_info) {
    vlan_info_p->said        = v_info->said;
    vlan_info_p->isl_vlan_id = v_info->isl_vlan_id;
    vlan_info_p->status      = v_info->state ==
	VLAN_SUSPENDED_STATE ? VLAN_SUSPENDED : VLAN_OPERATIONAL;
    vlan_info_p->type        = v_info->type;
    vlan_info_p->mtu         = v_info->mtu;
    vlan_info_p->name_length = v_info->vlan_name_length;
    
    memcpy(vlan_info_p->vlan_name, v_info->vlan_name,
	     v_info->vlan_name_length);
    vlan_info_p->vlan_name[v_info->vlan_name_length] = '\0';
    
    vlan_info_p->ring_number   = v_info->ring_number;
    vlan_info_p->parent_vlan   = v_info->parent_vlan;
    if (v_info->ring_number && v_info->type == VLAN_FDDI_TYPE)
	vlan_info_p->parent_valid = TRUE;
    else 
	vlan_info_p->parent_valid = FALSE;
    vlan_info_p->bridge_number = v_info->bridge_number;
    vlan_info_p->stp_type      = v_info->stp_type;
    vlan_info_p->tb_vlan_1     = v_info->tb_vlan_1;
    vlan_info_p->tb_vlan_2     = v_info->tb_vlan_2;
}

/* vtp_configure_vlan_in_database
 *
 * This function is called from within the VTP platform-independant
 * code to add, modify or delete a VLAN (e.g. by download, single-shot
 * or domain addition/deletion).  The routine performs the operation
 * without consistency checks (which must be done in the test and
 * precheck functions) which gives the advantage that bulk changes
 * (via download) can be preformed all at once and then checked for
 * consistency at the end.  If there is not enough memory to add a
 * VLAN or a database corruption is detected upon VLAN deletion (with
 * reference to cleaning up old TLB links) then an error is returned.
 */ 
unsigned char vtp_configure_vlan_in_database (vlan_domain_ptr domain_p,
					      unsigned char mode,
					      vtp_vlan_info* v_info_p) { 
    vlan_info_ent_ptr ent_p, delete_point;

    ent_p = NULL;
    delete_point = NULL;

    switch (mode) {
    case VTP_ADD:
	/* add new vlan in database */
	ent_p = vtp_insert_info_entry(domain_p, v_info_p);
	if (!ent_p) {
	    VTP_LOG_RUNTIME("Failed to add a new VLAN entry\n");
	    return (VTP_ERROR_INSERT_FAILURE);
	}
	ent_p->flags = NEW_VLAN;
	vtp_update_vlan_info(ent_p, v_info_p);
	vtp_pruning_add_vlan_init(domain_p, ent_p);
	vtp_pruning_add_vlan_refresh(domain_p, ent_p);
	break;
    case VTP_MODIFY:
	ent_p = vtp_search_info_entry_by_vlan(domain_p,
					      v_info_p->isl_vlan_id);
	ent_p->flags = MODIFIED_VLAN;
	ent_p->name_length = v_info_p->vlan_name_length;
	memcpy(ent_p->vlan_name, v_info_p->vlan_name,
	       v_info_p->vlan_name_length); 
	vtp_update_vlan_info(ent_p, v_info_p);
	break;
    case VTP_DELETE:
	for (ent_p = domain_p->vlan_head; ent_p; ent_p = ent_p->isl_next)
	    { 
		if (ent_p->isl_vlan_id == v_info_p->isl_vlan_id)
		    break;
		delete_point = ent_p;
	    }
	
	if (!delete_point) { /* deleting from head */
	    domain_p->vlan_head = ent_p->isl_next;
	} else {             /* deleting from middle to tail */
	    delete_point->isl_next = ent_p->isl_next;
	}

	/* decrement vlan count */
	domain_p->vlan_count--;
	vtp_pruning_delete_vlan(domain_p, ent_p); 

	free(ent_p);
    }
	
    return (VTP_OK);
    
}


/* This function is called by vtp_configure_vlan to patch up the VLAN
 * translation information.  This is necessary since the
 * vtp_configure_vlan interface only permits one VLAN to be modified
 * at a time.  As such it is impossible to keep three VLANs consistent
 * by only changing one.  This function causes the one or two
 * referenced VLANs to be kept in sync with the changed VLAN. 
 */

static unsigned char 
vtp_automate_translation_info (vlan_domain_ptr domain_p,
			       vtp_vlan_info* v_info_p) 
{
    /* both zero */
    if (!v_info_p->tb_vlan_1 && !v_info_p->tb_vlan_2 ) {
	vtp_delete_tlb_entries(domain_p, v_info_p);
    } else {
	if (!vtp_enter_tlb_entries(domain_p, v_info_p)) {
	    VTP_LOG_ERROR("Translation information inconsistent within"
			  "VLAN\n");
	    return VTP_ERROR_INCONSISTENT_VLAN;
	}
    } 

    return VTP_OK;
}

/* The following static vars and two functions are used to save state
 * of referenced translation VLANs during a call to
 * vtp_configure_vlan and restore them in event that the configuration
 * fails.  This code gymnastics is necessary to enable the 'change
 * one at a time' mode of operation as well as the 'bulk change' mode
 * normally used by SNMP and VTP PDUs.
 */
static vlan_info_ent_ptr trans_vlan_1;
static vlan_info_ent_ptr trans_vlan_2;
static unsigned short tb1_1, tb1_2, tb2_1, tb2_2;

static void vtp_save_translation_values (vlan_domain_ptr domain_p,
					 vtp_vlan_info* vlan_p) { 
    trans_vlan_1 = vtp_search_info_entry_by_vlan(domain_p, vlan_p->tb_vlan_1);
    trans_vlan_2 = vtp_search_info_entry_by_vlan(domain_p, vlan_p->tb_vlan_2);
    
    if (trans_vlan_1) {
	tb1_1 = trans_vlan_1->tb_vlan_1;
	tb1_2 = trans_vlan_1->tb_vlan_2;
    }

    if (trans_vlan_2) {
	tb2_1 = trans_vlan_2->tb_vlan_1;
	tb2_2 = trans_vlan_2->tb_vlan_2;
    }
}

static void vtp_restore_translation_values (void) {
    if (trans_vlan_1) {
	trans_vlan_1->tb_vlan_1 = tb1_1;
	trans_vlan_1->tb_vlan_2 = tb1_2;
    }

    if (trans_vlan_2) {
	trans_vlan_2->tb_vlan_1 = tb2_1;
	trans_vlan_2->tb_vlan_2 = tb2_2;
    }
}

/*
 * If vlan successfully added to network then trigger a vtp update
 * to inform the rest of the world of the addition. 
 */
Vtp_Err_t vtp_configure_vlan (unsigned char dname_length, 
			      unsigned char *domain_name, 
			      Vtp_Op_t mode, 
			      vtp_vlan_info*  v_info_p) {
   unsigned char result;
   vlan_domain_ptr domain_p;
   vlan_info_ent_ptr vlan_p;
   vtp_vlan_info vlan_before, *vb, vlan_after, *va;
   unsigned char recursive_bailout;

   vb = NULL;
   va = NULL;
   recursive_bailout = 0;

   /* identify the domain */
   domain_p = vtp_search_domain(dname_length, domain_name);
   if (!domain_p) {
       VTP_LOG_CONFIG_ERROR("Domain not found\n");
       return VTP_ERROR_NO_DOMAIN;
   }

   /* do it only on server */
   if (domain_p->mode == VTP_CLIENT) {
       VTP_LOG_CONFIG_ERROR("Cannot configure a VLAN in client mode\n");
       return VTP_ERROR_NOT_SERVER;
   }

   while ((domain_p->busy) && (recursive_bailout <
			       MAX_VTP_LOCK_RETRY)) { 
       vtp_process_reschedule();
       recursive_bailout++;
   }
   
   if (recursive_bailout == MAX_VTP_LOCK_RETRY) {
       VTP_LOG_CONFIG_ERROR("Database locked, giving up.\n");
       return VTP_ERROR_DATABASE_LOCKED;
   }

   domain_p->busy = TRUE;

   vlan_p = vtp_search_info_entry_by_vlan(domain_p,
					  v_info_p->isl_vlan_id);
   if (vlan_p) {
       retrieve_vlan_info(vlan_p, &vlan_before);
       vb = &vlan_before;
   }

   result = vtp_precheck_vlan(v_info_p);
   if (result != VTP_OK) {
       VTP_LOG_CONFIG_ERROR("VLAN %d configuration unsuccessful\n",
			    v_info_p->isl_vlan_id); 
       domain_p->busy = FALSE;
       return result;
   }

   vtp_save_translation_values(domain_p, v_info_p);

   result = vtp_automate_translation_info(domain_p, v_info_p);
   if (result != VTP_OK) {
       domain_p->busy = FALSE;
       return result;
   }

   result = vtp_test_vlan(domain_p, mode, v_info_p);
   if (result != VTP_OK) { 
       VTP_LOG_CONFIG_ERROR("VLAN %d configuration unsuccessful\n",
			    v_info_p->isl_vlan_id); 
       vtp_restore_translation_values();
       domain_p->busy = FALSE;
       return result;
   }

   result = vtp_configure_vlan_in_database(domain_p, mode, v_info_p);
   if (result != VTP_OK) { 
       VTP_LOG_CONFIG_ERROR("VLAN %d configuration unsuccessful\n",
			    v_info_p->isl_vlan_id); 
       domain_p->busy = FALSE;
       return result;
   }

   vlan_p = vtp_search_info_entry_by_vlan(domain_p,
					  v_info_p->isl_vlan_id);
   if (vlan_p) {
       retrieve_vlan_info(vlan_p, &vlan_after);
       va = &vlan_after;
   }

   vtp_vlan_change_notification(mode, domain_p->dname_length,
				domain_p->domain_name, vb,
				va); 

   if (mode == VTP_ADD) {
       vtp_pruning_add_vlan(domain_p, vlan_p);
   }

   /* Don't increment the revision number if all we are doing is
    * adding the default VLANs */

   if ((vtp_is_default_vlan_id(v_info_p->isl_vlan_id)) && 
       (mode == VTP_ADD)) {
       vtp_create_new_revision(domain_p, FALSE);
   } else {
       vtp_create_new_revision(domain_p, TRUE);
   }

   VTP_LOG_CONFIRMATION("Vlan %d configuration successful\n",
			v_info_p->isl_vlan_id); 
   domain_p->busy = FALSE;
   return (result); 
}


/* vtp_do_flooding
 *
 * Called when the VTP entity changes into a server or experiences a
 * configuration change (such as a higher revision or a name change).
 * The routine calls vtp_do_trunk_flooding on each of the trunks in
 * the domain, skipping the one specified in the function call. 
 */
void vtp_do_flooding (vlan_domain_ptr domain_p, void *trunk_id, 
		      unsigned short start_value) {

   vtp_trunk_itf_ptr trunk_p;

   /* look for the insertion point in the trunk list */
   for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	trunk_p->next_trunk_in_domain) {
       if (trunk_p->trunk_id != trunk_id) {
	   vtp_do_trunk_flooding(trunk_p, start_value);
	   vtp_process_reschedule(); 
       }
   }

}


/* vtp_update_revision_number
 *
 * Called when a single-shot VLAN change has been made.  This routine
 * increments the revision number by one, wrapping back to one (not
 * zero) when the revision is maxxed out.  If the domain is in
 * transparent mode then the revision is reset to zero. 
 */
void vtp_update_revision_number(vlan_domain_ptr domain_p)
{

    if (domain_p->mode == VTP_SERVER) {
        if (domain_p->revision == 0xffffffff) {
            domain_p->revision = 1;
	} else {
            ++domain_p->revision;
	}
    } else {
	domain_p->revision = 0;
    }
}



/* vtp_create_new_revision
 *
 * Called when a single-shot (one VLAN change by CLI) has occurred.
 * The routine updates the domain's configuration revision number and
 * sets the timestamp and updater accordingly.  The new configuration
 * is then flooded out the domain's trunks.
 */
void vtp_create_new_revision (vlan_domain_ptr domain_p, 
			      vtp_boolean update_rev) {
    vtp_domain_info old_domain, new_domain;
    unsigned char error;

    retrieve_domain_info(domain_p, &old_domain);
    if (update_rev)
       vtp_update_revision_number(domain_p);

    domain_p->updater = domain_p->my_update_id;

    vtp_get_timestamp(domain_p->timestamp);

    vtp_generate_md5_digest(domain_p);

    retrieve_domain_info(domain_p, &new_domain);
    error = vtp_domain_change_notification(VTP_MODIFY, &old_domain,
					      &new_domain);

    if ((error == VTP_NOTIFICATION_OK) && (domain_p->dname_length)) {
	    /* 
	     * Do the flooding on all trunk links.
	     * All existing VLANs must be sent out in the advt.
	     */
	    vtp_do_flooding(domain_p, 0, 0);
	}
    
}


/* vtp_validate_md5_digest
 *
 * Called from vtp_process_subset_adv function this routine calculates
 * the MD5 digest over the scratch area and compares it to the digest
 * contained therein.  If they are equal it returns TRUE, FALSE
 * otherwise. 
 */
vtp_boolean vtp_validate_md5_digest (vlan_domain_ptr domain_p, 
				 vtp_trunk_itf_ptr trunk_p) {
    unsigned char digest[VTP_MD5_DIGEST_SIZE];
    unsigned long length;
    unsigned char *write_p;
    summary_adv_ptr summ_p;

    /* some initialization */
    length = 0;

    /* copy secret value to the memory */
    memcpy(domain_p->scratch, domain_p->secret_value,
	   VTP_MD5_DIGEST_SIZE); 
    length += VTP_MD5_DIGEST_SIZE;

    summ_p = (summary_adv_ptr) ((unsigned char*)domain_p->scratch +
				VTP_MD5_DIGEST_SIZE);

    /* copy global info to the memory */
    summ_p->version = VTP_VERSION;
    summ_p->code = VTP_SUMMARY_ADV;
    summ_p->followers = 0;             /* zeroed out for digest
					* calculation */
    summ_p->dname_length = domain_p->dname_length;

    memset(summ_p->domain_name, 0, VTP_MAX_DOMAIN_NAME_LENGTH);
    memcpy(summ_p->domain_name, domain_p->domain_name,
	   domain_p->dname_length); 

    summ_p->revision = domain_p->next_revision;
    summ_p->updater_id = domain_p->next_updater_id;

    /* Zero'ed out for digest calculation */
    memset(summ_p->update_timestamp, 0, VTP_TIMESTAMP_SIZE); 
    memset(summ_p->md5_digest, 0, VTP_MD5_DIGEST_SIZE);

    length += sizeof(summary_adv);

    /* VLAN info is alrady there, calculate the length */
    length += domain_p->update_vlan_length;

    /* copy secret value to the memory */
    write_p = (unsigned char *)(domain_p->last_vlan_p) + 
                                    (domain_p->last_vlan_p)->info_length; 
    memcpy(write_p, domain_p->secret_value, VTP_MD5_DIGEST_SIZE);
    length += VTP_MD5_DIGEST_SIZE;

    vtp_get_md5_digest(domain_p->scratch, length, digest);

    return(name_cmp(digest, domain_p->next_md5_digest,
		    VTP_MD5_DIGEST_SIZE, VTP_MD5_DIGEST_SIZE) ==
	   NAME_EQUAL);
}



/* vtp_generate_md5_digest
 *
 * This function is called by the VTP platform-independant code
 * whenever configuration has changed for a domain.  It puts the VTP
 * database information for the given domain in network byte order and
 * format and calculates the MD5 digest over it.  The routine needs a
 * chunk of memory to calculate the digest and will force the domain
 * into transparent mode if it can't get it. 
 */
void vtp_generate_md5_digest (vlan_domain_ptr domain_p)
{

    unsigned char *mem_p;
    vlan_info_ent_ptr ent_p;
    unsigned long length;
    unsigned char info_length;
    unsigned char *write_p;
    summary_adv_ptr summ_p;
    vlan_info_ptr vlan_info_p; 
    vlan_tlv_ptr tlv_p;
    unsigned short *tb_vlan_p;  

    ent_p = NULL;
    length = 0;
    info_length = 0;
    vlan_info_p = NULL; 
    tlv_p = NULL;

    /* get a chunk of contiguous memory */
    mem_p = malloc(domain_p->vlan_count * VTP_MAX_VLAN_INFO_SIZE + 
		   sizeof(summary_adv) + VTP_MD5_DIGEST_SIZE +
		   VTP_MD5_DIGEST_SIZE); 

    if (!mem_p) {
        VTP_LOG_RUNTIME("VTP: no memory for md5 calculation\n");
	vtp_error_notification(domain_p->dname_length,
			       domain_p->domain_name,
			       VTP_ERROR_INSERT_FAILURE); 
        change_to_transparent(domain_p);
        return;
    } 

    /* copy secret value to the memory */
    memcpy(mem_p, domain_p->secret_value, VTP_MD5_DIGEST_SIZE);
    length += VTP_MD5_DIGEST_SIZE;

    summ_p = (summary_adv_ptr)((unsigned char*)mem_p +
			       VTP_MD5_DIGEST_SIZE); 
    /* copy global info to the memory */
    summ_p->version = VTP_VERSION;
    summ_p->code = VTP_SUMMARY_ADV;

    /* Zero'ed out for MD5 digest calculation */
    summ_p->followers = 0; 
    memset(summ_p->update_timestamp, 0, VTP_TIMESTAMP_SIZE);
    memset(summ_p->md5_digest, 0, VTP_MD5_DIGEST_SIZE);

    summ_p->dname_length  = domain_p->dname_length;
    memset(summ_p->domain_name, 0, VTP_MAX_DOMAIN_NAME_LENGTH);
    memcpy(summ_p->domain_name, domain_p->domain_name,
	   domain_p->dname_length); 
    summ_p->revision = vtp_format_4_bytes(domain_p->revision);
    summ_p->updater_id = vtp_format_4_bytes(domain_p->updater);

    length += sizeof(summary_adv);
    write_p = mem_p + length;

    /* copy VLAN info over to the memory */
    for (ent_p = domain_p->vlan_head; ent_p; ent_p = ent_p->isl_next)
	{ 
	    vlan_info_p = (vlan_info_ptr)write_p;

	    vlan_info_p->status = ent_p->status;
	    vlan_info_p->type = ent_p->type;
	    vlan_info_p->name_length = ent_p->name_length;
	    vlan_info_p->isl_vlan_id = vtp_format_2_bytes(ent_p->isl_vlan_id);
	    vlan_info_p->mtu = vtp_format_2_bytes(ent_p->mtu);
	    vlan_info_p->said = vtp_format_4_bytes(ent_p->said);
	
	    memset(vlan_info_p->vlan_name, 0, 
		   ADJUST_LENGTH_LONG(ent_p->name_length)); 
	    memcpy(vlan_info_p->vlan_name, ent_p->vlan_name,
		   ent_p->name_length); 
	    info_length = ent_p->name_length + sizeof(vlan_info);
	    info_length = ADJUST_LENGTH_LONG(info_length);
	    write_p += info_length;

	    /* append the TLV fields if the VLAN type is not Ethernet */
	    tlv_p = (vlan_tlv_ptr)write_p;
	    if ((ent_p->type == VLAN_FDDI) || (ent_p->type == VLAN_TR))   {
		tlv_p->tag = TAG_SR_RING;
		tlv_p->length = 1;
		tlv_p->value = vtp_format_2_bytes(ent_p->ring_number);
		info_length += sizeof(vlan_tlv);
		write_p += sizeof(vlan_tlv);
		tlv_p++;
		tlv_p->tag = TAG_PARENT_VLAN;
		tlv_p->length = 1;
		tlv_p->value = vtp_format_2_bytes(ent_p->parent_vlan);
		tlv_p++;
		info_length += sizeof(vlan_tlv);
		write_p += sizeof(vlan_tlv);
	    } else if ((ent_p->type == VLAN_FDDI_NET) || 
		       (ent_p->type == VLAN_TR_NET)) { 
		tlv_p->tag = TAG_SR_BRIDGE;
		tlv_p->length = 1;
		tlv_p->value = vtp_format_2_bytes(ent_p->bridge_number);
		tlv_p++;
		info_length += sizeof(vlan_tlv);
		write_p += sizeof(vlan_tlv);
		tlv_p->tag = TAG_STP_TYPE;   
		tlv_p->length = 1;
		tlv_p->value = vtp_format_2_bytes(ent_p->stp_type);
		tlv_p++;
		info_length += sizeof(vlan_tlv);
		write_p += sizeof(vlan_tlv);
	    }
	    
	    /* append the TLV fields if the VLAN is translational bridged */
	    if (ent_p->tb_vlan_1 != 0)  {
		tlv_p->tag = TAG_TB_VLAN;
		if (ent_p->tb_vlan_2 != 0) {
		    tlv_p->length = 2;
		    tlv_p->value = 0;
		    tlv_p++;
		    info_length += sizeof(vlan_tlv);
		    write_p += sizeof(vlan_tlv);
		    tb_vlan_p = (unsigned short *)tlv_p;
		    *tb_vlan_p = vtp_format_2_bytes(ent_p->tb_vlan_1);
		    tb_vlan_p++;
		    *tb_vlan_p = vtp_format_2_bytes(ent_p->tb_vlan_2);
		    info_length += sizeof(vlan_tlv);
		    write_p += sizeof(vlan_tlv);
		} else {
		    tlv_p->length = 1;
		    tlv_p->value = vtp_format_2_bytes(ent_p->tb_vlan_1);
		    info_length += sizeof(vlan_tlv);
		    write_p += sizeof(vlan_tlv);
		}
	    }
	    
	    vlan_info_p->info_length = info_length;  
	    length += info_length; 
	    vtp_process_reschedule();
	}
    
    /* copy secret value to the memory */
    memcpy(write_p, domain_p->secret_value, VTP_MD5_DIGEST_SIZE);
    length += VTP_MD5_DIGEST_SIZE;            

    /* get the checksum */
    vtp_get_md5_digest(mem_p, length, domain_p->md5_digest);

    free(mem_p);
}



/*
 * Function to get a random number between 0 and 1000 ms.
 */
unsigned long vtp_get_random_jittered_timer_value (void)
{
    unsigned long random_number;

    random_number = vtp_get_random_number();

    return (random_number % VTP_TIMER_MAX_RANDOM_MS);

}


/* vtp_relay_packet
 *  
 * Called to relay a packet when in transparent mode. 
 */
void vtp_relay_packet (vtp_trunk_itf_ptr trunk_p, void *buff_p,
		       unsigned long length) {

   vlan_domain_ptr domain_p;
   vtp_trunk_itf_ptr other_trunk_p;
   void *copy_bp;
    
   domain_p = trunk_p->domain_p;
    
   for (other_trunk_p = domain_p->trunk_head; other_trunk_p;
	other_trunk_p = other_trunk_p->next_trunk_in_domain) { 
       if (other_trunk_p->trunk_id != trunk_p->trunk_id) {
	   copy_bp = vtp_buffer_duplicate(buff_p);
	   if (copy_bp) {
	       vtp_transmit_packet(other_trunk_p->trunk_id, copy_bp,
				  length);
	   } else {
	       VTP_LOG_RUNTIME("VTP: relay_packet - no buffer\n");
	   }
       }
   }
}



/*
 * Clean up when the VTP being disabled.
 */
void vtp_terminate (void)
{
    vlan_domain_ptr domain_p;
    vtp_domain_info dead_domain;

    /* clean up the domain based trunk table, database */
    for (domain_p = vlan_domain_head; domain_p;)  {
	dead_domain.domain_name_length = domain_p->dname_length;
	memcpy(dead_domain.domain_name, domain_p->domain_name,
	       domain_p->dname_length); 
	domain_p = domain_p->domain_next;
	vtp_configure_domain(VTP_DELETE, &dead_domain);
    }
}
