/* $Id: vtp_util.c,v 1.1.42.9 1996/09/13 21:47:49 thille Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_util.c,v $
 *------------------------------------------------------------------
 * vtp_util.c - vtp utilities  (portable code module)
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_util.c,v $
 * Revision 1.1.42.9  1996/09/13  21:47:49  thille
 * CSCdi69139:  IOS includes in platform independent VTP code.
 * Branch: California_branch
 * Remove the inclusion of IOS include files from source files shared with
 * non-IOS platforms.  Add comments so someone else won't make the same
 * mistake later.  Move the inclusions to platform specific include file
 * vtp_debug.h.
 *
 * Revision 1.1.42.8  1996/08/28  13:20:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 1.1.42.7  1996/08/15  21:52:13  cyoung
 * CSCdi65439:  Cannot change name of VLAN on Cat3000
 * Branch: California_branch
 *
 * Revision 1.1.42.6  1996/06/26  02:18:57  cyoung
 * Retrieving or configuring more than one trunk a problem.  Also
 * reformat some parser output.
 * CSCdi61201:  On 7k: Trunk link shows on one interface(I have two atm
 * int)only.
 * Branch: California_branch
 *
 * Revision 1.1.42.5  1996/06/20  21:13:32  cyoung
 * Deleting old VLAN subinterfaces can wipe new subinterfaces when trunk is
 * either LANE or 802.10.  Do deletions first to avoid conflict.
 * CSCdi59372:  wrong behavior when conflicting vlan info is received
 * Branch: California_branch
 *
 * Revision 1.1.42.4  1996/06/20  19:50:43  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.1.42.3  1996/06/18  00:51:14  cyoung
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
 * Revision 1.1.42.2  1996/05/21  22:29:37  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.1.42.1  1996/05/09  14:48:43  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.72.1  1996/04/27  07:38:46  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.60.7  1996/04/18  21:26:54  percyk
 * added consistency checks for translational vlans via snmp.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.60.6  1996/04/17  21:57:04  cyoung
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
 * Revision 1.1.60.5  1996/04/11  21:29:16  percyk
 * byte swapping fixes to support intel platforms.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.60.4  1996/04/11  20:50:49  cyoung
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
 * Revision 1.1.60.3  1996/04/10  17:14:28  percyk
 * vtp_enter_two_tlb_entries always returns false.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.60.2  1996/04/09  17:09:48  percyk
 * vlan config consistency between snmp and cli.
 * advt interval and passwd needs to be accepted from download data.
 * cannot change vlan name length once it's set.
 *
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.60.1  1996/04/08  02:42:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.48.5  1996/04/06  02:07:08  cyoung
 * CSCdi53693:  vlan is not built when it should be
 * don't abort the vtp-database command if nv_valid false
 * And the following fixes while we're at it:
 *  don't load database when writing into it
 *  set the revision number to zero when loading from NVRAM (to avoid
 *   conflict with TFTP).
 *  use vtp_rev_less in vtp_download_info
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.48.4  1996/04/03  22:37:05  cyoung
 * New objects for TFTP support, ETR vlan type and trunk table creation
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MIB - TFTP support, ETR type allowed, still need work on Trunk creation
 * dep - use temp buf to determine add/mod mode instead of real database
 * core - add tooBig hook for trap generation
 *
 * Revision 1.1.48.3  1996/04/01  18:52:02  percyk
 * vtp allows more than one eth vlan to be translated to fddi vlan.
 * vtp does not update fcp when a new translation is added.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.48.2  1996/03/25  02:21:58  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.48.1  1996/03/22  09:45:13  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.12.12  1996/03/22  16:22:21  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.11  1996/03/19  00:21:22  percyk
 * vtp support on isl links.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.10  1996/03/15  18:57:56  cyoung
 * No form of vtp trunk no longer requires domain info.
 * vtp trunk command not executed until all configuration read.
 * clean up trunk info if default VLAN additions fail.
 * Move vtp_intf_enable function to vtp process.
 * more vtp configure support.
 * add subsystem dependancies and sequence.
 * Fix bug with modifying an existing vlan from CLI.
 * Fix pointer arithmetic in vtp_prepare_storage.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.9  1996/03/13  17:28:11  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.8  1996/03/08  18:19:06  cyoung
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
 * Revision 1.1.12.7  1996/03/07  02:35:19  cyoung
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
 * Revision 1.1.12.6  1996/03/05  01:56:38  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.5  1996/03/04  23:48:48  percyk
 * define new platform dependent routine on rcving no response to req
 * packet.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.4  1996/03/04  22:41:12  percyk
 * fix follower field in summary packet.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.3  1996/03/04  22:03:04  percyk
 * change revision number logic.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.2  1996/03/02  21:12:49  percyk
 * include bug fixes done after branch sync.
 *
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.12.1  1996/02/27  21:20:40  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.1.2.16  1996/02/26  18:07:32  percyk
 * more debug messages.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.15  1996/02/24  00:58:55  percyk
 * don't sniff packets in transparent mode.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.14  1996/02/23  22:49:09  percyk
 * don't send vtp requests in null domain state.  freeing buffer twice.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.13  1996/02/23  19:36:01  percyk
 * add accumulate/mirror logic for vtp counters.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.12  1996/02/23  17:10:33  percyk
 * misc bug fixes by cat5k group.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.11  1996/02/22  23:30:26  percyk
 * allow downloading vlan configs in transparent & no_domain_state mode.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.10  1996/02/22  21:51:29  cyoung
 * Report apply status of succeeded and now someOtherError where
 * appropriate.  Put back in fix for spurious memory accesses.
 * Minor code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.9  1996/02/22  04:42:59  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.8  1996/02/19  22:26:42  percyk
 * fixed bug found in tlb testing.  more debugging added.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.7  1996/02/16  19:32:17  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup, add more vtp debugging.
 *
 * Revision 1.1.2.6  1996/02/15  17:49:50  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * more code cleanup.
 *
 * Revision 1.1.2.5  1996/02/15  02:30:45  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * move platform dependent stuff into interface files.
 *
 * Revision 1.1.2.4  1996/02/14  18:33:13  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * do not allow any changes to default vlans.
 *
 * Revision 1.1.2.3  1996/02/13  18:54:47  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * sync router files with cat5k
 *
 * Revision 1.1.2.2  1996/02/13  04:03:12  cyoung
 * move notification_enabled to vtp_dep.c, minor bug fixes, add code to
 * check for native vlan range and existance of trunk before allowing sets.
 * Also some code shuffle as a result of resolving conflicts.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.1  1996/02/12  21:32:40  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.1  1996/01/26  16:54:30  percyk
 * placeholder file
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

/* vtp_rcvd_lower_rev_number
 *
 * Called when a summary advert is received with a lower configuration
 * revision number than the receiving VTP domain.  The routine will
 * cause a summary advertisement to be sent out on that trunk in
 * 'defense' of the correct revision.  There is a jittered delay
 * before sending out the advertisement to avoid flooding a link with
 * potentially many advertisements from all receiving VTP entities.
 * All VTP entities receiving the first defense will refrain from
 * sending out their advertisements. 
 */
void vtp_rcvd_lower_rev_number (vtp_trunk_itf_ptr trunk_p)
{
   /* stop the advertising timer if running */
   if (vtp_timer_active(trunk_p->advt_timer_p)) {
      vtp_stop_timer(trunk_p->advt_timer_p);
  }
 
   trunk_p->jittered_value =  vtp_get_random_jittered_timer_value();
   trunk_p->advt_start_value = SEND_NONE;

   vtp_start_timer(trunk_p->advt_timer_p, trunk_p->jittered_value);

}



/* vtp_rcvd_equal_rev_number
 *
 * Called when a summary advert is received with a configuration
 * revision number equal to that of the receiving VTP domain.  The
 * routine will compare the summary's MD5 digest that that of the
 * domains.  If they are equal, then the advertising timer on this
 * trunk is reset.  Otherwise an error is flagged and a SNMP trap sent
 * (if enabled). 
 */
void vtp_rcvd_equal_rev_number (vtp_trunk_itf_ptr trunk_p, void *buff_p)
{

   vlan_domain_ptr domain_p;
   summary_adv_ptr summary_p;
   long result;

   domain_p = trunk_p->domain_p;
   summary_p = vtp_buffer_payload( trunk_p->trunk_id, buff_p);

   result = name_cmp(summary_p->md5_digest, domain_p->md5_digest,
		     VTP_MD5_DIGEST_SIZE, VTP_MD5_DIGEST_SIZE);

   /* md5 digest checksum match? */
   switch (result) {

      case NAME_EQUAL:
         /* stop the advertising timer */
       if (vtp_timer_active(trunk_p->advt_timer_p)) {
	   vtp_stop_timer(trunk_p->advt_timer_p);
       }
       
       trunk_p->jittered_value   =  vtp_get_random_jittered_timer_value();
       trunk_p->advt_start_value = SEND_NONE;
       vtp_start_timer(trunk_p->advt_timer_p, domain_p->advt_interval +
		       trunk_p->jittered_value);
       break;
       
      /* md5 digest checksum mismatch */
   case NAME_LESS:
   case NAME_GREATER:
   default:
       VTP_LOG_RUNTIME("VTP: rcvd_equal_rev - md5 digest checksum"
		       " mismatch\n"); 
       domain_p->acc.revision_error_count++;
       vtp_error_notification(domain_p->dname_length, domain_p->domain_name,
			      VTP_ERROR_REVISION_ERROR);
       vtpAPI_revnum_error_detected(domain_p->domain_name, 
				    domain_p->dname_length);
   }
 
}


/* vtp_rcvd_higher_rev_number
 *
 * Called when a summary advert is received with a higher
 * configuration revision number than that of the receiving VTP
 * domain. If not already in the process of receiving subsets and
 * there are no followers indicated in the summary then an
 * advertisement request is sent out.  If followers are indicated then
 * no advertisement is sent out and the subsets are waited for.  If
 * the domain was already in the act of receiving subsets and the
 * summary is from a different source with a higher revision number
 * than the advertisement received from the last source and the
 * summary also indicates followers then the subsets to that point are
 * discarded and the new summary is marked as the source of the
 * update.  If the domain was already in the process of receiving
 * subsets and a summary is received of a lower rev than that which
 * was originally advertised, then the new summary is discarded.  If
 * the summary has an equal rev to that expected and a retransmit
 * request had been sent out, then the request timer is stopped
 * (provided followers are indicated) and the domain prepares to
 * receive the retransmitted subsets.  If the summary (from the same
 * source) indicates a higher revision than that expected, then the
 * saved subsets to that point are discarded and the domain prepares
 * to receive the higher revision configuration. 
 */
void vtp_rcvd_higher_rev_number (vtp_trunk_itf_ptr trunk_p, void *buff_p,
                                                       unsigned char *src_mac)
{

    summary_adv_ptr summary_p;
    vlan_domain_ptr domain_p;
    unsigned long rcvd_rev;
    
    domain_p = trunk_p->domain_p;
    summary_p = vtp_buffer_payload( trunk_p->trunk_id, buff_p);
    rcvd_rev = vtp_format_4_bytes(summary_p->revision);

    /* Currently not in updating state.  This is the most common case. */ 
    if (!domain_p->vtp_updating) {
	VTP_LOG_RUNTIME("VTP: domain %s currently not in updating"
			" state\n", summary_p->domain_name); 
	
	/*
	 * rcvd a higher rev number in the summary packet than currently in
	 * the domain but followers field is empty.  Send an adv request to
	 * the source.
	 */   
	if (!summary_p->followers) {
	    VTP_LOG_RUNTIME("VTP: summary packet with followers field zero\n");
	    vtp_transmit_adv_request_packet(trunk_p, 0);
	    return;
	}
	
	domain_p->vtp_updating              = TRUE;
	domain_p->next_revision             = summary_p->revision;
	domain_p->next_updater_id           = summary_p->updater_id;
	domain_p->next_num_adv_subset_count = summary_p->followers;
	domain_p->next_seq_expected         = 1;

	if (is_vtp_tlv_pruning_ok(domain_p)) {
	    domain_p->next_pruning_mode =
		get_vtp_tlv_pruning_mode(domain_p); 
	} else {
	    domain_p->next_pruning_mode = domain_p->pruning_mode;
	}
	
	memcpy(domain_p->next_update_timestamp,
	       summary_p->update_timestamp, VTP_TIMESTAMP_SIZE); 
	memcpy(domain_p->next_md5_digest, summary_p->md5_digest,
	       VTP_MD5_DIGEST_SIZE); 
	memcpy(domain_p->next_adv_src_mac, src_mac, VTP_MAC_ADDRESS_SIZE);
	
	vtp_start_timer(domain_p->update_pending_timer,
			MAX_VTP_UPDATE_PENDING_TIME); 
	
	domain_p->update_trunk_p = (void *)trunk_p;
	vtp_allocate_update_memory(trunk_p, summary_p->followers);
	return;
    }
    
    /* an earlier summary received from the same source */
    if (name_cmp(domain_p->next_adv_src_mac, src_mac,
		 VTP_MAC_ADDRESS_SIZE, VTP_MAC_ADDRESS_SIZE) ==
	NAME_EQUAL) { 
	
	/*
	 * rcvd a lower rev number in summary packet than the one previously
	 * sent, the source seems to be badly broken, log it and toss the
	 * packet.
	 */ 
	if ( vtp_rev_less(rcvd_rev,
			  vtp_format_4_bytes(domain_p->next_revision)) ) { 
	    VTP_LOG_RUNTIME("VTP: lower rev summary from same source"
			    " while updating\n");  
	    return;
	}
	
	if ( VTP_REV_EQUAL(rcvd_rev,
			   vtp_format_4_bytes(domain_p->next_revision))) {
	    if ( (summary_p->followers > 0) &&
		(domain_p->flags & UPDATE_RXMT_REQUESTED) ) {
		if (vtp_timer_active(trunk_p->rqst_timer_p)) {
		    vtp_stop_timer(trunk_p->rqst_timer_p);
		}
		domain_p->flags &= ~UPDATE_RXMT_REQUESTED;
		domain_p->next_num_adv_subset_count = summary_p->followers;
		domain_p->next_seq_expected = 1;
		vtp_start_timer(domain_p->update_pending_timer,
				MAX_VTP_UPDATE_PENDING_TIME);
	    }
	} else {
	  /* switch to the higher revision from the same source */
	    VTP_LOG_RUNTIME("VTP: higher rev num from same source while"
			    " updating\n"); 
	    vtp_flush_update_adv_info(domain_p);
	    domain_p->vtp_updating = TRUE;
	    domain_p->next_revision = summary_p->revision;
	    domain_p->next_updater_id = summary_p->updater_id;
	    domain_p->next_num_adv_subset_count = summary_p->followers;
	    domain_p->next_seq_expected = 1;

	    if (is_vtp_tlv_pruning_ok(domain_p)) {
		domain_p->next_pruning_mode =
		    get_vtp_tlv_pruning_mode(domain_p); 
	    } else {
		domain_p->next_pruning_mode = domain_p->pruning_mode;
	    }
	    
	    memcpy(domain_p->next_update_timestamp,
		   summary_p->update_timestamp, VTP_TIMESTAMP_SIZE);
	    memcpy(domain_p->next_md5_digest, summary_p->md5_digest,
		   VTP_MD5_DIGEST_SIZE); 
	    vtp_start_timer(domain_p->update_pending_timer,
			    MAX_VTP_UPDATE_PENDING_TIME);
	    domain_p->update_trunk_p = (void *)trunk_p;
	    vtp_allocate_update_memory(trunk_p, summary_p->followers);
	}
	return;
    }
    
    /*
     * a higher revision receive from different source while doing
     * updating, switch to the new source if with non-zero followers.
     */
    if ( (vtp_rev_less(vtp_format_4_bytes(domain_p->next_revision),
		       rcvd_rev) ) && (summary_p->followers > 0) ) {
	VTP_LOG_RUNTIME("VTP: higher rev num from different source while"
			" updating\n"); 
	vtp_flush_update_adv_info(domain_p);
	domain_p->vtp_updating              = TRUE;
	domain_p->next_revision             = summary_p->revision;
	domain_p->next_updater_id           = summary_p->updater_id;
	domain_p->next_num_adv_subset_count = summary_p->followers;
	domain_p->next_seq_expected = 1;

	if (is_vtp_tlv_pruning_ok(domain_p)) {
	    domain_p->next_pruning_mode =
		get_vtp_tlv_pruning_mode(domain_p);  
	} else {
	    domain_p->next_pruning_mode = domain_p->pruning_mode;
	}

	memcpy(domain_p->next_update_timestamp,
	       summary_p->update_timestamp, VTP_TIMESTAMP_SIZE);
	memcpy(domain_p->next_md5_digest, summary_p->md5_digest,
	       VTP_MD5_DIGEST_SIZE); 
	memcpy(domain_p->next_adv_src_mac, src_mac,
	       VTP_MAC_ADDRESS_SIZE); 
	vtp_start_timer(domain_p->update_pending_timer,
			MAX_VTP_UPDATE_PENDING_TIME);
	domain_p->update_trunk_p = (void *)trunk_p;
	vtp_allocate_update_memory(trunk_p, summary_p->followers);
    }
    
}


/* vtp_sort_vlan_entry
 *
 * Called by the VTP platform-independant code whenever a vlan is
 * created.  The routine takes the new vlan info and places it in the
 * sorted linked list of VLANs
 */
void vtp_sort_vlan_entry (vlan_domain_ptr domain_p,
			  vlan_info_ent_ptr new_vlan_info_p)
{
    vlan_info_ent_ptr vlan_info_p, insert_point;
    
    insert_point = NULL;
    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p =
	 vlan_info_p->isl_next ) {
	
        if (new_vlan_info_p->isl_vlan_id < vlan_info_p->isl_vlan_id)
            break;
	insert_point = vlan_info_p;
    }
    
    if (!domain_p->vlan_head) {     /* first vlan */
	domain_p->vlan_head = new_vlan_info_p;
    } else if (!insert_point) {     /* in front of head */
	new_vlan_info_p->isl_next = domain_p->vlan_head;
	domain_p->vlan_head = new_vlan_info_p;
    } else {                       /* middle to end */
	new_vlan_info_p->isl_next = insert_point->isl_next;
	insert_point->isl_next = new_vlan_info_p;
    }
    
}



/* vtp_search_info_entry_by_vlan 
 *
 * Used throughout the VTP platform-independant code to locate a VLAN
 * identified by it's ISL VLAN ID.   The routine returns a pointer to
 * the VLAN element if found, NULL otherwise. 
 */
vlan_info_ent_ptr vtp_search_info_entry_by_vlan (vlan_domain_ptr domain_p,
                                                   unsigned short vlan_id)
{
    vlan_info_ent_ptr vlan_info_p;
 
    if (!vlan_id)
       return (NULL);
 
    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p =
	 vlan_info_p->isl_next ) {

        if (vlan_id == vlan_info_p->isl_vlan_id)
	    return(vlan_info_p);
        else if (vlan_id < vlan_info_p->isl_vlan_id)
	    return NULL;
    }
 
    return NULL;
 
}



/* vtp_search_info_entry_by_name
 *
 * Called by the VTP platform-independant code to insure that a new or
 * modified VLAN is unique by name.  The routine returns a pointer to
 * the VLAN element if found, NULL otherwise.  The routine will not
 * find VLANs with the value of OLD_VLAN in the flags field. 
 */
vlan_info_ent_ptr vtp_search_info_entry_by_name (vlan_domain_ptr domain_p,
                       unsigned char name_length, unsigned char *vlan_name)
{
    vlan_info_ent_ptr vlan_info_p;
    char cmp;
 
    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p =
	 vlan_info_p->isl_next) {
	if (vlan_info_p->flags != OLD_VLAN) {
	    cmp = name_cmp(vlan_name, vlan_info_p->vlan_name, name_length,
			   vlan_info_p->name_length);
	    
	    if (cmp == NAME_EQUAL)
		return (vlan_info_p);
	}
    }
    
    return (NULL);
}



/* vtp_search_info_entry_by_said
 *
 * Called by the VTP platform-independant code to insure that a new or
 * modified VLAN is unique by SAID.  The routine returns a pointer to
 * the VLAN element if found, NULL otherwise.  The routine will not
 * find VLANs with the value of OLD_VLAN in the flags field. 
 */
vlan_info_ent_ptr vtp_search_info_entry_by_said (vlan_domain_ptr domain_p,
						 unsigned long said) {
    vlan_info_ent_ptr vlan_info_p;

    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; 
	 vlan_info_p = vlan_info_p->isl_next) {
	if ((vlan_info_p->flags != OLD_VLAN) && 
	    (said == vlan_info_p->said)) 
            return(vlan_info_p);
    }
    
    return (NULL);
    
}


/* vtp_insert_info_entry
 *
 * Called by the VTP platform-independant code whenever a VLAN is
 * being added to a domain's VLAN database.  The routine allocates
 * space for the new entry, sets it's VLAN ID (but nothing else),
 * inserts it into the VLAN list and updates the vlan count.  If the
 * routine cannot allocate memory for the new element it returns NULL.
 */ 
vlan_info_ent_ptr vtp_insert_info_entry (vlan_domain_ptr domain_p,
					 vtp_vlan_info* v_info_p) {
    vlan_info_ent_ptr vlan_info_p;

    vlan_info_p = NULL;

    /* create a new entry and insert it into the database */
    vlan_info_p = malloc(sizeof(vlan_info_ent));
 
    if (!vlan_info_p) {
	VTP_LOG_RUNTIME("VTP: insert_info_entry - no memory\n");
	return NULL;
    }
 
    vlan_info_p->isl_next = NULL;
    vlan_info_p->flags       = 0;
 
    /* Need to set this to enter the new element into the correct spot
       within the info_sorted_list */

    vlan_info_p->isl_vlan_id = v_info_p->isl_vlan_id;

    /* insert it into the sorted list */
    vtp_sort_vlan_entry(domain_p, vlan_info_p);
 
    /* increment vlan count */
    domain_p->vlan_count++;

    return (vlan_info_p);
 
}


/*
 * Delete the database for a given domain.
 */
void vtp_delete_database (unsigned char dname_length, 
			  unsigned char *domain_name) {
 
    vlan_domain_ptr domain_p;
  
    domain_p = vtp_search_domain(dname_length, domain_name);

    if (domain_p)
       vtp_flush_database(domain_p);
 
}

 

/* vtp_flush_database
 *
 * This function is really the guts of vtp_delete_database (found in
 * vtp_interface.h), it is also used by the platform-independant code
 * to wipe out a domain's database when restoring to the original
 * database after a consistency failure.  The routine traverses the
 * linked list of VLANs in a domain and systematically deletes each
 * one (freeing the held memory in the process).
 */

void vtp_flush_database (vlan_domain_ptr domain_p)
{
    vlan_info_ent_ptr vlan_p, dead_vlan;
 
    for (vlan_p = domain_p->vlan_head; 
	 vlan_p; ) { 
	dead_vlan = vlan_p;
	vlan_p = vlan_p->isl_next;
        free(dead_vlan);
    }

    domain_p->vlan_count = 0;
    domain_p->vlan_head = NULL;

    vtp_pruning_flush_database(domain_p);
 
}



/* vtp_search_domain
 *
 * Called throughout the VTP platform-independant code to find a VTP
 * domain based on its name.  The routine returns a pointer to the
 * domain information, NULL otherwise. 
 */
vlan_domain_ptr vtp_search_domain (unsigned char dname_length, 
				   unsigned char *domain_name) {
    vlan_domain_ptr domain_p;
    long cmp;

    if (!vtp_is_initialized)
	return NULL;

    if (!domain_name)
	return NULL;
 
    for (domain_p = vlan_domain_head; domain_p; domain_p =
	 domain_p->domain_next) {
	
        cmp = name_cmp(domain_name, domain_p->domain_name,
		       dname_length, domain_p->dname_length); 
 
        if (cmp == NAME_EQUAL)
            return domain_p;
        else if (cmp == NAME_LESS)
	    return NULL;
    }
 
    return (NULL);

}


/* vtp_get_index 
 *
 * Called when a new domain is being created and it's SNMP index has
 * not been specified.  The routine returns a unique index for the new
 * domain. 
 */
unsigned long vtp_get_index (void) {

    unsigned long index;
    vlan_domain_ptr domain_p;
    vtp_boolean unique;

    unique = TRUE;
 
    do {
	index = ++vtp_domain_index;
	for (domain_p = vlan_domain_head; domain_p; 
	     domain_p = domain_p->domain_next) {
	    if (index == domain_p->index)
		unique = FALSE;
	}
    } while (!unique);
    
    return (index);
}



/*
 * Check a domain's database busy status.
 */
vtp_boolean vtp_test_database_busy (unsigned char dname_length, 
				unsigned char *domain_name) {
    vlan_domain_ptr domain_p;

    domain_p = vtp_search_domain(dname_length, domain_name); 
 
    if (!domain_p)
	return FALSE;

    return domain_p->busy;
}


/*
 * Set a domain's database busy status.
 */
void vtp_set_database_busy (unsigned char dname_length, 
			    unsigned char *domain_name) {
    vlan_domain_ptr domain_p;

    domain_p = vtp_search_domain(dname_length, domain_name); 
 
    if (!domain_p) {
	VTP_LOG_RUNTIME("VTP: no database found. Attempting to set"
			" busy flag\n");
	return;
    }

    domain_p->busy = TRUE;
}
 
/*
 * Reset a domain's database busy status.
 */
void vtp_reset_database_busy (unsigned char dname_length, 
			      unsigned char *domain_name) {
    vlan_domain_ptr domain_p;

    domain_p = vtp_search_domain(dname_length, domain_name); 
 
    if (!domain_p) {
	VTP_LOG_RUNTIME("VTP: no database found. Attempting to clear"
			" busy flag\n");
	return;
    }

    domain_p->busy = FALSE;
}

/*	
 * Called throughout the VTP platform-independant code to find a trunk
 * link given its trunk-id (void *) and domain.  The routine returns a
 * pointer to the trunk information, NULL otherwise. 
 */
vtp_trunk_itf_ptr vtp_find_trunk_link_in_domain (vlan_domain_ptr domain_p,
						 void *trunk_id) {
    vtp_trunk_itf_ptr trunk_p;
 
    for (trunk_p = domain_p->trunk_head; trunk_p; 
	 trunk_p = trunk_p->next_trunk_in_domain) {
        if (trunk_id == trunk_p->trunk_id)
	    return (trunk_p);
        else if (trunk_id < trunk_p->trunk_id)
	    return NULL;
    }
 
    return NULL;

}

/*
 * Called throughout the VTP platform-independant code to find a trunk
 * link given its trunk-id (void *).  The routine returns a pointer to
 * the trunk information, NULL otherwise. 
 */
vtp_trunk_itf_ptr vtp_find_trunk_link (void *trunk_id) {
    vtp_trunk_itf_ptr trunk_p;
 
    for (trunk_p = vtp_trunk_head; trunk_p; 
	 trunk_p = trunk_p->trunk_next) {
        if (trunk_id == trunk_p->trunk_id)
	    return (trunk_p);
        else if (trunk_id < trunk_p->trunk_id)
	    return NULL;
    }
 
    return NULL;

}



/* vtp_insert_domain_trunk
 *
 * Called from the vtp_configure_trunk routine, this routine inserts a
 * trunk, according to trunk-id, into a domain's trunk list.
 */
void vtp_insert_domain_trunk (vlan_domain_ptr domain_p,
			      vtp_trunk_itf_ptr trunk_p) {
    vtp_trunk_itf_ptr this_trunk_p, insert_point;
 
    insert_point = NULL;

    for (this_trunk_p = domain_p->trunk_head; this_trunk_p;
	 this_trunk_p = this_trunk_p->next_trunk_in_domain) {
        if (trunk_p->trunk_id < this_trunk_p->trunk_id)
	    break;
	insert_point = this_trunk_p;
    }
 
    if (!domain_p->trunk_head) {     /* first vlan */
	domain_p->trunk_head = trunk_p;
	trunk_p->next_trunk_in_domain = NULL;
    } else if (!insert_point) {     /* in front of head */
	trunk_p->next_trunk_in_domain = domain_p->trunk_head;
	domain_p->trunk_head = trunk_p;
    } else {                       /* middle to end */
	trunk_p->next_trunk_in_domain =
	    insert_point->next_trunk_in_domain; 
	insert_point->next_trunk_in_domain = trunk_p;
    }
}

/* vtp_delete_domain_trunk
 *
 * Called from vtp_configure_trunk routine.  This routine deletes a
 * trunk from a given domain's list. 
 */

void vtp_delete_domain_trunk (vlan_domain_ptr domain_p,
			      vtp_trunk_itf_ptr trunk_p) {
    vtp_trunk_itf_ptr this_trunk_p, delete_point;

    delete_point = NULL;

    for (this_trunk_p = domain_p->trunk_head; this_trunk_p;
	 this_trunk_p = this_trunk_p->next_trunk_in_domain) {
        if (trunk_p == this_trunk_p)
	    break;
	delete_point = this_trunk_p;
    }
    
    if (!this_trunk_p)
	return;

    if (!delete_point) { /* deleting from head */
	domain_p->trunk_head = trunk_p->next_trunk_in_domain;
    } else {             /* deleting from middle to tail */
	delete_point->next_trunk_in_domain =
	    trunk_p->next_trunk_in_domain; 
    }    
}


/* Called by vtp_configure_trunk to initialize the trunk data
 * structures and create the timers needed on the trunk.
 */
static void vtp_init_trunk_info (vtp_trunk_itf_ptr trunk_p) {
    trunk_p->flood_timestamp  = 0;
    trunk_p->flags            = 0;
    trunk_p->mtu              = DEFAULT_TRUNK_MTU;
    trunk_p->rqst_retry_count = 0;
    trunk_p->advt_start_value = SEND_NONE;
    trunk_p->rqst_start_value = RQST_NONE;

    /*
     * advt_timer_p - start a jittered timer after rcving a vtp request.
     * Send summary packet on jitter timer expiry.
     */ 
    trunk_p->advt_timer_p =
	vtp_create_timer(vtp_trunk_link_flood_timeout, 
			 (void *)trunk_p);
    
    /*
     * rqst_timer_p - start timer after sending a vtp request.  Expect to
     * receive a response within a timeout period.
     */
    trunk_p->rqst_timer_p =
	vtp_create_timer(vtp_trunk_link_rqst_timeout, 
			 (void *)trunk_p);
}

/* Called by vtp_configure_trunk to update the variables in the trunk
 * data structure.
 */
static void vtp_update_trunk_info (vtp_trunk_itf_ptr trunk_p, 
			   vtp_trunk_info* t_info, 
			   vlan_domain_ptr domain_p) {
    trunk_p->domain_p = domain_p;
    trunk_p->trunk_id = t_info->trunk_id;
    memcpy(trunk_p->trunk_name, t_info->trunk_name,
	   VTP_MAX_TRUNK_NAME_LENGTH);
    memcpy(trunk_p->vlans_enabled, t_info->vlans_enabled,
	   VLANS_BITMASK_SIZE);
    vtp_core_change_vlan_pruning_eligible(t_info->trunk_id,
					  t_info->vlans_pruning_eligible);
    trunk_p->native_vlan = t_info->native_vlan;
    trunk_p->encaps_type = t_info->encaps_type;
}

/*
 * Two timers are hinged onto the trunk data structure - 
 * advt_timer_p - start jittered timer after rcving request packet. 
 * rqst_timer_p - start timer after sending a request packet.
 */
Vtp_Err_t vtp_configure_trunk (Vtp_Op_t mode, 
			       vtp_trunk_info* t_info) {
    vlan_domain_ptr domain_p;
    vtp_trunk_itf_ptr trunk_p, action_point;
    vtp_trunk_info new_values, old_values;

    action_point = NULL;
    domain_p = vtp_search_domain(t_info->dname_length,
				 t_info->domain_name); 

    for (trunk_p = vtp_trunk_head; trunk_p; 
	 trunk_p = trunk_p->trunk_next) {
        if (t_info->trunk_id <= trunk_p->trunk_id)
	    break;
	action_point = trunk_p;
    }

    switch (mode) {
    case VTP_ADD:
	if (!domain_p)   {
	    VTP_LOG_ERROR("VTP: can't identify VLAN domain on trunk"); 
	    return VTP_ERROR_NO_DOMAIN;
	}

	if ((trunk_p) && (t_info->trunk_id == trunk_p->trunk_id)) {
	    VTP_LOG_RUNTIME("VTP: trunk link already exists\n");
	    return VTP_ERROR_TRUNK_DUP;
	}

	trunk_p = malloc(sizeof(vtp_trunk_itf));

	if (!trunk_p) {
	    VTP_LOG_ERROR("VTP: add_trunk_link - no memory\n");
	    return VTP_ERROR_INSERT_FAILURE;
	}

	trunk_p->trunk_next = NULL;

	if (!vtp_trunk_head) {     /* first vlan */
	    vtp_trunk_head = trunk_p;
	} else if (!action_point) {     /* in front of head */
	    trunk_p->trunk_next = vtp_trunk_head;
	    vtp_trunk_head = trunk_p;
	} else {                       /* middle to end */
	    trunk_p->trunk_next = action_point->trunk_next;
	    action_point->trunk_next = trunk_p;
	}

	vtp_init_trunk_info(trunk_p);
	vtp_update_trunk_info(trunk_p, t_info, domain_p);
	vtp_insert_domain_trunk(domain_p, trunk_p);
	vtp_pruning_add_trunk_init(domain_p, trunk_p);
	vtp_pruning_add_trunk(domain_p, trunk_p);
	retrieve_trunk_info(trunk_p, &new_values);
	break;
    case VTP_MODIFY:
	if (!domain_p)   {
	    VTP_LOG_ERROR("VTP: can't identify VLAN domain on trunk"); 
	    return VTP_ERROR_NO_DOMAIN;
	}

	if ((!trunk_p) || (t_info->trunk_id != trunk_p->trunk_id)) {
	    VTP_LOG_RUNTIME("VTP: trunk link does not exist\n");
	    return VTP_ERROR_NO_TRUNK;
	}

	retrieve_trunk_info(trunk_p, &old_values);
	vtp_delete_domain_trunk(domain_p, trunk_p);

	if (vtp_timer_active(trunk_p->advt_timer_p)) {
	    vtp_stop_timer(trunk_p->advt_timer_p);
	}
	if (vtp_timer_active(trunk_p->rqst_timer_p)) {
	    vtp_stop_timer(trunk_p->rqst_timer_p);
	}
	vtp_insert_domain_trunk(domain_p, trunk_p);
	vtp_update_trunk_info(trunk_p, t_info, domain_p);
	retrieve_trunk_info(trunk_p, &new_values);
	break;
    case VTP_DELETE:
	if ((!trunk_p) || (t_info->trunk_id != trunk_p->trunk_id)) {
	    VTP_LOG_RUNTIME("VTP: trunk link does not exist\n");
	    return VTP_ERROR_NO_TRUNK;
	}
	retrieve_trunk_info(trunk_p, &old_values);
	if (vtp_timer_active(trunk_p->advt_timer_p)) {
	    vtp_stop_timer(trunk_p->advt_timer_p);
	}

	if (vtp_timer_active(trunk_p->rqst_timer_p)) {
	    vtp_stop_timer(trunk_p->rqst_timer_p);
	}

	vtp_delete_timer(trunk_p->advt_timer_p);
	vtp_delete_timer(trunk_p->rqst_timer_p);

	domain_p = trunk_p->domain_p;
	if (domain_p->update_trunk_p == trunk_p)
	    domain_p->update_trunk_p = NULL;

	if (!action_point) { /* deleting from head */
	    vtp_trunk_head = trunk_p->trunk_next;
	} else {             /* deleting from middle to tail */
	    action_point->trunk_next = trunk_p->trunk_next;
	}    
	
	vtp_delete_domain_trunk(domain_p, trunk_p);
	vtp_pruning_delete_trunk(domain_p, trunk_p);

	free(trunk_p );
    }

    vtp_trunk_change_notification(mode, &old_values, &new_values);

    if (mode != VTP_DELETE) {
	/* send advertisement request */
	if ( (domain_p->mode == VTP_CLIENT) || (domain_p->vlan_count == 0) )
	    vtp_transmit_adv_request_packet(trunk_p, 0);
	else
	    vtp_do_trunk_flooding(trunk_p, 0);
    }

    return VTP_OK;
}

/* An API function to retrieve information about a trunk given it's
 * trunk ID.
 */
Vtp_Err_t vtp_get_trunk_info (vtp_trunk_info* info, 
			      Vtp_Search_t type) { 
    vtp_trunk_itf_ptr trunk_p;
    unsigned char found;

    found = FALSE;
    trunk_p = NULL;

    if (!vtp_is_initialized)
	return VTP_ERROR_NOT_INITIALIZED;

    if (!info)
	return VTP_ERROR_NO_INPUT;

    switch (type) {
    case VTP_FIRST:
	trunk_p = vtp_trunk_head;
	break;
    case VTP_EXACT:
	for (trunk_p = vtp_trunk_head; trunk_p; trunk_p =
	     trunk_p->trunk_next) {
	    if (info->trunk_id == trunk_p->trunk_id) 
		break;
	    if (info->trunk_id < trunk_p->trunk_id)
		return VTP_ERROR_NO_TRUNK;
	}
	break;
    case VTP_NEXT:
	for (trunk_p = vtp_trunk_head; trunk_p; 
	     trunk_p = trunk_p->trunk_next) {
	    if (info->trunk_id < trunk_p->trunk_id) 
		break;
	}
	break;
    case VTP_NAME:
	for (trunk_p = vtp_trunk_head; trunk_p; 
	     trunk_p = trunk_p->trunk_next) {  
	    if (strcmp(info->trunk_name, trunk_p->trunk_name) == 0)
		break;
	}
    }

    if (!trunk_p) 
	return VTP_ERROR_NO_TRUNK;
    
    retrieve_trunk_info(trunk_p, info);
    return VTP_OK;
}

Vtp_Err_t vtp_get_trunk_by_domain (unsigned char dname_length,
				   unsigned char* domain_name,
				   vtp_trunk_info* t_info_p,
				   Vtp_Search_t search_type) {
    vtp_trunk_itf_ptr trunk_p;
    vlan_domain_ptr domain_p;
    unsigned char found;

    found = FALSE;
    trunk_p = NULL;

    if (!vtp_is_initialized)
	return VTP_ERROR_NOT_INITIALIZED;

    if (!t_info_p)
	return VTP_ERROR_NO_INPUT;

    domain_p = vtp_search_domain(dname_length, domain_name);
    if (!domain_p) 
	return VTP_ERROR_NO_DOMAIN;

    switch (search_type) {
    case VTP_FIRST:
	trunk_p = domain_p->trunk_head;
	break;
    case VTP_EXACT:
	for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	     trunk_p->next_trunk_in_domain) {
	    if (t_info_p->trunk_id == trunk_p->trunk_id) 
		break;
	    if (t_info_p->trunk_id < trunk_p->trunk_id)
		return VTP_ERROR_NO_TRUNK;
	}
	break;
    case VTP_NEXT:
	for (trunk_p = domain_p->trunk_head; trunk_p; 
	     trunk_p = trunk_p->next_trunk_in_domain) {
	    if (t_info_p->trunk_id < trunk_p->trunk_id) 
		break;
	}
	break;
    case VTP_NAME:
	for (trunk_p = domain_p->trunk_head; trunk_p; 
	     trunk_p = trunk_p->next_trunk_in_domain) {
	    if (strcmp(t_info_p->trunk_name, trunk_p->trunk_name) == 0) 
		break;
	}
    }

    if (!trunk_p) 
	return VTP_ERROR_NO_TRUNK;
    
    retrieve_trunk_info(trunk_p, t_info_p);
    return VTP_OK;
}

unsigned long vtp_get_time_until_summary (void* trunk_id) {
    vtp_trunk_itf_ptr trunk_p;

    if (!vtp_is_initialized)
	return 0;
    
    for (trunk_p = vtp_trunk_head; trunk_p; 
	 trunk_p = trunk_p->trunk_next) {
	
	if (trunk_id == trunk_p->trunk_id) 
	    return(vtp_timer_time_left(trunk_p->advt_timer_p));

	if (trunk_id < trunk_p->trunk_id) 
	    return 0;
    }

    return 0;
} 
/*
 * Function to get information for a given domain.
 */
Vtp_Err_t vtp_get_domain_info (vtp_domain_info *info, 
				   Vtp_Search_t type) {
    vlan_domain_ptr domain_p;
    long cmp;

    if (!vtp_is_initialized)
	return VTP_ERROR_NOT_INITIALIZED;

    domain_p = NULL;
    cmp = NAME_GREATER;

    switch (type) {
    case VTP_FIRST:
	domain_p = vlan_domain_head;
	break;
    case VTP_NAME:
    case VTP_EXACT:
	domain_p = vtp_search_domain(info->domain_name_length,
				     info->domain_name); 
	break;
    case VTP_NEXT:
	for (domain_p = vlan_domain_head; domain_p; 
	     domain_p = domain_p->domain_next) {

	    cmp = name_cmp(info->domain_name, domain_p->domain_name,
			   info->domain_name_length,
			   domain_p->dname_length); 
	    if (cmp == NAME_LESS) 
		break;
	}
	break;
    }

    if (domain_p) { 
	retrieve_domain_info(domain_p, info);
	return VTP_OK;
    }

    return VTP_ERROR_NO_DOMAIN;

}


/*
 * Function to get information for a given device.
 */
void vtp_get_device_info (vtp_device_info *info) {

    info->version          = vtp_version;
    info->max_vlan_storage = vtp_max_vlan_storage;
}

void vtp_set_max_storage (short max_vlans) {

    vtp_max_vlan_storage = max_vlans;
}




/*
 * Function to get information for a given VLAN.
 */
Vtp_Err_t vtp_get_vlan_info (unsigned char dname_length, 
			     unsigned char *domain_name, 
			     vtp_vlan_info *info, 
			     Vtp_Search_t type) {
    vlan_domain_ptr domain_p;
    vlan_info_ent_ptr vlan_p;

    vlan_p = NULL;
    domain_p = vtp_search_domain(dname_length, domain_name);

    if (!domain_p) 
	return VTP_ERROR_NO_DOMAIN;

    switch (type) {
    case VTP_FIRST:
	vlan_p = domain_p->vlan_head;
	break;
    case VTP_EXACT:
	vlan_p = vtp_search_info_entry_by_vlan(domain_p,
					       info->isl_vlan_id);
	break;
    case VTP_NEXT:
	for (vlan_p = domain_p->vlan_head; vlan_p; 
	     vlan_p = vlan_p->isl_next) {  
	    if (info->isl_vlan_id < vlan_p->isl_vlan_id)
		break;
	}
	break;
    case VTP_NAME:
	vlan_p = vtp_search_info_entry_by_name(domain_p,
					       info->vlan_name_length,
					       info->vlan_name);
    }

    if (!vlan_p) 
       return VTP_ERROR_NO_VLAN;

    retrieve_vlan_info(vlan_p, info);
    return VTP_OK;
}



/*
 * This function takes the VTP database and puts a copy of it in the
 * format used during vtp_download_info.  The invoker of this function
 * (dependant code) is responsible for freeing the memory.
 */
Vtp_Err_t vtp_upload_info (unsigned char dname_length, 
			   unsigned char *domain_name, 
			   void **info, unsigned long* info_size)
{ 
    vlan_domain_ptr domain_p;
    vtp_domain_info *d_info_p;
    vtp_vlan_info *vlan_info_p;
    vlan_info_ent_ptr vlan_p;
 
    domain_p = vtp_search_domain(dname_length, domain_name);

    if (!domain_p) {
       *info = NULL;
       return VTP_ERROR_NO_DOMAIN;
    }
 
    *info_size = (domain_p->vlan_count * sizeof(vtp_vlan_info)) +
	sizeof(vtp_domain_info);
    *info = malloc(*info_size);

    if (!*info) {
       VTP_LOG_ERROR("VTP: no memory for upload\n");
       return VTP_ERROR_INSERT_FAILURE; 
    }

    /* copy domain information */
    d_info_p = (vtp_domain_info *)(*info);

    d_info_p->domain_name_length = domain_p->dname_length;
    d_info_p->mode            = domain_p->mode;
    d_info_p->revision        = domain_p->revision;
    d_info_p->updater         = domain_p->updater;
    d_info_p->index           = domain_p->index;
    d_info_p->advt_interval   = domain_p->advt_interval;
    d_info_p->password_length = domain_p->password_length;
    d_info_p->vlan_count      = domain_p->vlan_count;
    d_info_p->pruning_mode    = domain_p->pruning_mode;

    memcpy(d_info_p->domain_name, domain_p->domain_name,
	   domain_p->dname_length);
    memcpy(d_info_p->password, domain_p->password,
	   domain_p->password_length);
    memcpy(d_info_p->timestamp, domain_p->timestamp,
	   VTP_TIMESTAMP_SIZE); 
    memcpy(d_info_p->md5_digest, domain_p->md5_digest,
	   VTP_MD5_DIGEST_SIZE); 
 
    /* copy individual VLAN information */
    vlan_p = domain_p->vlan_head;
    vlan_info_p = (vtp_vlan_info *)((unsigned char *)d_info_p +
				    sizeof(vtp_domain_info));
    
    for (vlan_p = domain_p->vlan_head; vlan_p; vlan_p =
	 vlan_p->isl_next, vlan_info_p++) {
	retrieve_vlan_info(vlan_p, vlan_info_p);
    }

    return VTP_OK;
}


/* Function to store the valid database while processing new
   information from a download/network advertisement.  The backup is
   used to restore the database in the case that an inconsistentcy is
   found. 

   vlan_count not preserved, must be recalculated at restore time. */

static unsigned char vtp_backup_database (vlan_domain_ptr domain_p) {

    vlan_info_ent_ptr vlan_p, new_vlan_p, prev_vlan_p;

    prev_vlan_p = domain_p->vlan_backup_head;

    for (vlan_p = domain_p->vlan_head; vlan_p; 
	 vlan_p = vlan_p->isl_next) { 
	new_vlan_p = malloc(sizeof(vlan_info_ent));
	if (!new_vlan_p)
	    return VTP_ERROR_INSERT_FAILURE;

	memcpy(new_vlan_p, vlan_p, sizeof(vlan_info_ent)); 
	if (!domain_p->vlan_backup_head) {
	    domain_p->vlan_backup_head = new_vlan_p; 
	} else {
	    prev_vlan_p->isl_next = new_vlan_p;
	}
	prev_vlan_p = new_vlan_p;
    }

    return VTP_OK;
}

/* This function is called from vtp_download_info when an inconsitency
 * has been detected in the downloaded configuration and the original
 * configuration needs to be restored to the VTP database. 
 */ 
static unsigned char vtp_restore_database (vlan_domain_ptr domain_p) {
    vlan_info_ent_ptr vlan_p, dead_vlan;
    vtp_vlan_info v_info;
    unsigned char error;

    error = VTP_OK;
    vtp_flush_database(domain_p);

    vlan_p = domain_p->vlan_backup_head;
    while (vlan_p) {
	retrieve_vlan_info(vlan_p, &v_info);
	error = vtp_configure_vlan_in_database(domain_p, VTP_ADD,
					       &v_info); 
	dead_vlan = vlan_p;
	vlan_p = vlan_p->isl_next;
	free(dead_vlan);
    }

    domain_p->vlan_backup_head = NULL;

    if (error)
	VTP_LOG_ERROR("VLAN Database Corrupt");

    return error;
}

/* This function is called by vtp_download_info when the new
 * configuration has been committed to the VTP database and verified.
 * It is therefore no longer necessary to keep the backup. 
 */
static unsigned char free_database_backup (vlan_domain_ptr domain_p) {
    vlan_info_ent_ptr vlan_p, free_vlan_p;

    vlan_p = domain_p->vlan_backup_head;
    while (vlan_p) {
        free_vlan_p = vlan_p;
	vlan_p = vlan_p->isl_next;
	free(free_vlan_p);
    }

    domain_p->vlan_backup_head = NULL;
    return VTP_OK;
}

/* Called from vtp_download_info to report the changes made to the VTP
 * database. 
 */
static unsigned char vtp_report_database_modifications
(vlan_domain_ptr domain_p) { 
    vtp_vlan_info info_before, *ibef, info_after, *iaft;
    vlan_info_ent_ptr vlan_px, vlan_py;
    unsigned char mode;

    mode = 0;
    vlan_py = NULL;
    ibef = NULL;
    iaft = NULL;

    /* report deletions first.  This way old dependent configuration
     * can be cleared out first before new configuration is required
     * (i.e. if vlan 4 is new with name 'blue' and vlan 5 is old with
     * name 'blue' you want to make sure that 'blue' gets deleted
     * before 'blue' is created). */

    for (vlan_px = domain_p->vlan_head; vlan_px; 
	 vlan_px = vlan_px->isl_next) {
	if (vlan_px->flags == OLD_VLAN) {
	    retrieve_vlan_info(vlan_px, &info_before);
	    vtp_vlan_change_notification(VTP_DELETE,
					 domain_p->dname_length, 
					 domain_p->domain_name,
					 &info_before, NULL);
	}
    }

    for (vlan_px = domain_p->vlan_head; vlan_px; 
	 vlan_px = vlan_px->isl_next) {
	switch (vlan_px->flags) {
	case NEW_VLAN:
	    mode = VTP_ADD;
	    retrieve_vlan_info(vlan_px, &info_after);
	    iaft = &info_after;
	    ibef = NULL;
	    break;
	case MODIFIED_VLAN:
	    mode = VTP_MODIFY;
	    for (vlan_py = domain_p->vlan_backup_head; vlan_py;
		 vlan_py = vlan_py->isl_next) {
		if (vlan_py->isl_vlan_id == vlan_px->isl_vlan_id)
		    break;
	    }
	    retrieve_vlan_info(vlan_py, &info_before);
	    retrieve_vlan_info(vlan_px, &info_after);
	    iaft = &info_after;
	    ibef = &info_before;
	    break;
	case OLD_VLAN:
	    continue;
	}
	vtp_vlan_change_notification(mode, domain_p->dname_length,
				     domain_p->domain_name, ibef,
				     iaft);
    }
    
    return VTP_OK;
}

/*
 * Function to download the VTP configuration file.
 * Note - if the need_swap is a non-zero value, the 16/32 bit
 *        numbers must be swapped into natural memory format.
 *        When copied into a scratch area for MD5 digest
 *        calculation, these numbers must be in network/packet
 *        format, and this is done by calling vtp_format_2_bytes
 *        and vtp_format_4_bytes, respectively, and the actual
 *        conversion differs based on the little/big endian
 *        of the target processor itself.
 */
Vtp_Err_t vtp_download_info (vtp_boolean need_save, 
			     vtp_boolean from_network,
			     void *download_info) {
    vtp_domain_info *domain_info_p;
    vtp_vlan_info *vlan_info_p, retrieve_info;
    vlan_domain_ptr domain_p;
    unsigned long vlan_count;
    unsigned long store_info_size;
    unsigned char error, mode;
    vlan_info_ent_ptr vlan_p;
    void* trunk_id;
    unsigned short retries;
    vtp_boolean new_vlan_added;

    new_vlan_added = FALSE;
    mode = 0;
    /* do some basic check */
    domain_info_p = (vtp_domain_info *)download_info;
   
    error = vtp_check_domain_info(domain_info_p);
    if (error) {
        VTP_LOG_RUNTIME("VTP: invalid downloading\n");
        vtpAPI_report_apply_status(domain_info_p, vtp_someOtherError);
	free(download_info);
	return error;
    }
    
    if ((domain_info_p->vlan_count < VTP_MIN_VLANS) ||
	(domain_info_p->vlan_count > VTP_MAX_VLANS))  {
	    VTP_LOG_RUNTIME("VTP: illegal number of VLANs\n"); 
	    vtpAPI_report_apply_status(domain_info_p, vtp_someOtherError);
	    free(download_info);
	    return VTP_ERROR_DOMAIN_VLAN_COUNT;
	}

    domain_p = vtp_search_domain(domain_info_p->domain_name_length,
				 domain_info_p->domain_name);
    if (domain_p == NULL) {
        vtp_configure_domain(VTP_ADD, domain_info_p);
	
        domain_p = vtp_search_domain(domain_info_p->domain_name_length,
				     domain_info_p->domain_name);
	
        if (domain_p == NULL) {
	    VTP_LOG_RUNTIME("VTP: cannot create domain for downloading\n");
	    vtpAPI_report_apply_status(domain_info_p, vtp_someOtherError);
	    free(download_info);
	    return VTP_ERROR_INSERT_FAILURE;
        }
    } else if (domain_info_p->domain_name_length != 0) {
	if (!vtp_rev_less(domain_p->revision,
			  domain_info_p->revision)) { 
	    VTP_LOG_RUNTIME("VTP: Update revision is equal to"
			    " or less than revision of"
			    " database\r\n");  
	    vtpAPI_report_apply_status(domain_info_p,
				       vtp_configNumberError); 
	    error = VTP_ERROR_REVISION_ERROR;
	    free(download_info);
	    return error;
	}
    }

    for (retries = 0; (domain_p->busy) && 
	 (retries <  MAX_VTP_LOCK_RETRY); retries++) {
	VTP_LOG_RUNTIME("VTP: database busy - download waiting\n");
	vtp_process_reschedule();
    }

    if (retries == MAX_VTP_LOCK_RETRY) {
	vtpAPI_report_apply_status(domain_info_p, vtp_someOtherError);
	error = VTP_ERROR_DATABASE_LOCKED;
	free(download_info);
	return error;
    }
	
    /* set semaphore */
    domain_p->busy = TRUE;

    error = vtp_backup_database(domain_p);
    if (error) {
	VTP_LOG_RUNTIME("VTP: no space available for update\n");
	vtpAPI_report_apply_status(domain_info_p, vtp_someOtherError);
	free(download_info);
	domain_p->busy = FALSE;
	return error;
    }
    
    /* copy VLAN info to the memory */
    vlan_count = domain_info_p->vlan_count;
    vlan_info_p = (vtp_vlan_info *)((unsigned char *)download_info +
				    sizeof(vtp_domain_info));
    vtp_mark_database_old(domain_p);
    
    while (vlan_count--)  {
	if (vtp_search_info_entry_by_vlan(domain_p,
					  vlan_info_p->isl_vlan_id)) {
	    mode = VTP_MODIFY;
	} else {
	    mode = VTP_ADD;
	    new_vlan_added = TRUE;
	}

	error = vtp_precheck_vlan(vlan_info_p);
	if (error) {
	    vtp_restore_database(domain_p);
	    vtpAPI_report_apply_status(domain_info_p,
				       vtp_someOtherError); 
	    free(download_info);
	    domain_p->busy = FALSE;
	    return error;
	}

	error = vtp_configure_vlan_in_database(domain_p, mode,
					       vlan_info_p); 
	if (error) {
	    vtp_restore_database(domain_p);
	    VTP_LOG_RUNTIME("VTP: no space available for update\n");
	    vtpAPI_report_apply_status(domain_info_p,
				       vtp_someOtherError);
	    free(download_info);
	    domain_p->busy = FALSE;
	    return error;
	}
	
        vlan_info_p = (vtp_vlan_info *)((unsigned char *)vlan_info_p +
					sizeof(vtp_vlan_info));
    }

    for (vlan_p = domain_p->vlan_head; vlan_p; 
	 vlan_p = vlan_p->isl_next) { 
	switch (vlan_p->flags) {
	case NEW_VLAN:
	    mode = VTP_ADD;
	    break;
	case MODIFIED_VLAN:
	    mode = VTP_MODIFY;
	    break;
	case OLD_VLAN:
	    mode = VTP_DELETE;
	}

	retrieve_vlan_info(vlan_p, &retrieve_info);
	error = vtp_test_vlan(domain_p, mode, 
			      &retrieve_info);
	if (error) {
	    vtp_restore_database(domain_p);
	    VTP_LOG_RUNTIME("VTP: inconsistent edit\n");
	    free(download_info);
	    domain_p->busy = FALSE;
	    return error;
	}
    }

    if (need_save) {
	store_info_size = (domain_info_p->vlan_count *
			   sizeof(vtp_vlan_info)) +
			       sizeof(vtp_domain_info); 

	error = vtp_store_domain_info(domain_p->dname_length,
				      domain_p->domain_name,
				      download_info, store_info_size);
	if (error) 
	    if (from_network) {
		change_to_client(domain_p);
		vtpAPI_storage_exceeded(domain_p->domain_name,
					domain_p->dname_length);
	    } else {
		vtp_restore_database(domain_p);
		VTP_LOG_ERROR("VTP: error in store_domain_info\n");
		free(download_info);
		domain_p->busy = FALSE;
		return VTP_ERROR_NVSTORE_FAILURE;
	    } 
    }

    if (new_vlan_added) {
	vtp_pruning_add_vlan(domain_p, NULL);
    }

    /* validate each existing entry in the database */
    vtp_report_database_modifications(domain_p);

    vtp_delete_old_vlans(domain_p);

    free_database_backup(domain_p);

    /* update the global infomration in the database */
    vtp_configure_domain(VTP_MODIFY, domain_info_p);
      
    vtp_generate_md5_digest(domain_p);

    if ((from_network) && (domain_p->update_trunk_p))
	trunk_id = domain_p->update_trunk_p;
    else
	trunk_id = NULL;

    /* do the flooding on all trunks but the one learned from */
    vtp_do_flooding(domain_p, trunk_id, 0);
    
    vtpAPI_report_apply_status(domain_info_p, vtp_succeeded);
    
    free(download_info);
    
    /* set semaphore */
    domain_p->busy = FALSE;
    return error;
}



/*
 * Function to collect statistics for a given domain.
 * get stats = accumulated stats - mirror stats
 */
Vtp_Err_t vtp_get_statistics (unsigned char dname_length, 
			      unsigned char *domain_name, 
			      unsigned long *rx_summary_advt,
			      unsigned long *rx_subset_advt, 
			      unsigned long *rx_request_advt, 
			      unsigned long *tx_summary_advt, 
			      unsigned long *tx_subset_advt, 
			      unsigned long *tx_request_advt, 
			      unsigned long *config_rev_error, 
			      unsigned long *config_digest_error,
			      unsigned char type)
{
    vlan_domain_ptr domain_p;
 
    domain_p = vtp_search_domain(dname_length, domain_name);
    if (!domain_p) 
       return VTP_ERROR_NO_DOMAIN;

    *rx_summary_advt     = domain_p->acc.rx_summary_adv_count; 
    *rx_subset_advt      = domain_p->acc.rx_subset_adv_count; 
    *rx_request_advt     = domain_p->acc.rx_request_adv_count; 
    *tx_summary_advt     = domain_p->acc.tx_summary_adv_count; 
    *tx_subset_advt      = domain_p->acc.tx_subset_adv_count; 
    *tx_request_advt     = domain_p->acc.tx_request_adv_count;
    *config_rev_error    = domain_p->acc.revision_error_count;
    *config_digest_error = domain_p->acc.digest_error_count; 

    if (type == VTP_STATS_RELATIVE) {
	*rx_summary_advt  -=  domain_p->mirror.rx_summary_adv_count;
	*rx_subset_advt   -=  domain_p->mirror.rx_subset_adv_count;
	*rx_request_advt  -=  domain_p->mirror.rx_request_adv_count;
	*tx_summary_advt  -=  domain_p->mirror.tx_summary_adv_count;
	*tx_subset_advt   -=  domain_p->mirror.tx_subset_adv_count;
	*tx_request_advt  -=  domain_p->mirror.tx_request_adv_count;
	*config_rev_error -=  domain_p->mirror.revision_error_count;
	*config_digest_error -= domain_p->mirror.digest_error_count;
    }
    return VTP_OK;
}

/*
 * clear stats for particular domain.
 * clearing means copying the accumulated stats into mirror stats.
 */
void vtp_clear_domain_stats (vlan_domain_ptr domain_p)
{

   domain_p->mirror.rx_summary_adv_count =         
                        domain_p->acc.rx_summary_adv_count; 
                                                              
   domain_p->mirror.rx_subset_adv_count  =         
                        domain_p->acc.rx_subset_adv_count;  
                                                         
   domain_p->mirror.rx_request_adv_count =   
                        domain_p->acc.rx_request_adv_count;
                                                 
   domain_p->mirror.tx_summary_adv_count =
                        domain_p->acc.tx_summary_adv_count; 
                                                          
   domain_p->mirror.tx_subset_adv_count  =      
                        domain_p->acc.tx_subset_adv_count; 
                                                            
   domain_p->mirror.tx_request_adv_count =      
                        domain_p->acc.tx_request_adv_count;
                                                          
   domain_p->mirror.revision_error_count =     
                        domain_p->acc.revision_error_count; 
                                                             
   domain_p->mirror.digest_error_count   =       
                        domain_p->acc.digest_error_count;

}

 
 
/*
 * Function to clear statistics for a given/all domain(s).
 * clear_all = TRUE indicates clear stats for all the existing domains.
 *           = FALSE indicates clear stats for the given domain.
 *
 * copy the accumulated stats into mirror stats.
 */
void vtp_clear_statistics (vtp_boolean clear_all, 
			   unsigned char dname_length,  
			   unsigned char *domain_name) {
    vlan_domain_ptr domain_p;
 
    if (clear_all) {
       for (domain_p = vlan_domain_head; domain_p; domain_p =
	    domain_p->domain_next) {
           vtp_clear_domain_stats(domain_p);
       }
       return;
    }
 
    domain_p = vtp_search_domain(dname_length, domain_name);

    if (domain_p)
       vtp_clear_domain_stats(domain_p);

}


/* Called from vtp_configure_vlan to 'fix' the database when using a
 * single-shot configuration change (only changing one VLAN at a time
 * from the CLI.  This function attempts to make the referenced VLANs
 * jive with source VLAN as far as translation information.  If the
 * referenced VLANs are in conflict with the information passed into
 * the function then FALSE is returned.
 */
vtp_boolean vtp_enter_tlb_entries (vlan_domain_ptr domain_p,
			       vtp_vlan_info* v_info_p) {
   unsigned short* restore_location;
   unsigned short* restore_loc2;
   unsigned short* restore_loc3;
   vlan_info_ent_ptr tb_vlan1_p;
   vlan_info_ent_ptr tb_vlan2_p;
 
   /* restore_locXXX is used to keep track of any changes made to
    * the first VLANs in event that changes to the second VLAN fail.
    * Then the original state of the first VLANs can be restored,
    * leaving the database as found initially.
    */
   restore_location = NULL;
   restore_loc2 = NULL;
   restore_loc3 = NULL;
 
   tb_vlan1_p = vtp_search_info_entry_by_vlan(domain_p,
					      v_info_p->tb_vlan_1); 
   tb_vlan2_p = vtp_search_info_entry_by_vlan(domain_p,
					      v_info_p->tb_vlan_2); 

   /* The logic here is kind of wierd but I can't think of a better
    * way.  For each tb_vlan_x value passed in, make sure that the
    * VLAN pointed to is pointing back at the source VLAN.  This could
    * be either tb_vlan_1 or tb_vlan_2 in the referenced VLAN.  Only
    * tb_vlan_x values of zero may be changed to another value. 
    */

   if (tb_vlan1_p) {
       if (tb_vlan1_p->type == v_info_p->type)
	   return FALSE;

       if (tb_vlan1_p->tb_vlan_1) {
	   if (tb_vlan1_p->tb_vlan_1 != v_info_p->isl_vlan_id) {
	       if (!tb_vlan2_p) 
		       return FALSE;
	       if (tb_vlan1_p->tb_vlan_2) {
		   if (tb_vlan1_p->tb_vlan_2 != v_info_p->isl_vlan_id)
		       return FALSE;
	       } else {
		   tb_vlan1_p->tb_vlan_2 = v_info_p->isl_vlan_id;
		   restore_location = &tb_vlan1_p->tb_vlan_2;
	       }
	   }
       } else if (tb_vlan1_p->tb_vlan_2) {
	   if (tb_vlan1_p->tb_vlan_2 != v_info_p->isl_vlan_id) {
	       if (!tb_vlan2_p) {
		   return FALSE;
	       }
	       tb_vlan1_p->tb_vlan_1 = v_info_p->isl_vlan_id;
	       restore_location = &tb_vlan1_p->tb_vlan_1;
	   }
       } else {
	   tb_vlan1_p->tb_vlan_1 = v_info_p->isl_vlan_id;
	   restore_location = &tb_vlan1_p->tb_vlan_1;
       }
   }
   
   if (tb_vlan2_p) {
       if (tb_vlan2_p->type == v_info_p->type) {
	   if (restore_location) {
	       *restore_location = 0;
	   }
	   return FALSE;
       }

       if (tb_vlan2_p->tb_vlan_1) {
	   if (tb_vlan2_p->tb_vlan_1 != v_info_p->isl_vlan_id) {
	       if (!tb_vlan1_p) {
		   if (restore_location) {
		       *restore_location = 0;
		   }
		   return FALSE;
	       }
	       if (tb_vlan2_p->tb_vlan_2) {
		   if (tb_vlan2_p->tb_vlan_2 != v_info_p->isl_vlan_id) {
		       if (restore_location) {
			   *restore_location = 0;
			   return FALSE;
		       }
		   }
	       } else {
		   tb_vlan2_p->tb_vlan_2 = v_info_p->isl_vlan_id;
		   restore_loc2 = &tb_vlan2_p->tb_vlan_2;
	       }
	   }
       } else if (tb_vlan2_p->tb_vlan_2) {
	   if (tb_vlan2_p->tb_vlan_2 != v_info_p->isl_vlan_id) {
	       if (!tb_vlan1_p) {
		   return FALSE;
	       }
	       tb_vlan2_p->tb_vlan_1 = v_info_p->isl_vlan_id;
	       restore_loc2 = &tb_vlan2_p->tb_vlan_1;
	   }
       } else {
	   tb_vlan2_p->tb_vlan_1 = v_info_p->isl_vlan_id;
	   restore_loc2 = &tb_vlan2_p->tb_vlan_1;
       }
   }

   if (tb_vlan1_p && tb_vlan2_p) {
       if (tb_vlan1_p->type == tb_vlan2_p->type) {
	   if (restore_location) {
	       *restore_location = 0;
	   }
	   if (restore_loc2) {
	       *restore_loc2 = 0;
	   }
	   return FALSE;
       }

       if (tb_vlan1_p->tb_vlan_1) {
	   if (tb_vlan1_p->tb_vlan_1 != tb_vlan2_p->isl_vlan_id) 
	       if (tb_vlan1_p->tb_vlan_2) {
		   if (tb_vlan1_p->tb_vlan_2 != tb_vlan2_p->isl_vlan_id) {
		       if (restore_location) {
			   *restore_location = 0;
		       }
		       if (restore_loc2) {
			   *restore_loc2 = 0;
		       }
		       return FALSE;
		   }
	       } else {
		   tb_vlan1_p->tb_vlan_2 = tb_vlan2_p->isl_vlan_id;
		   restore_loc3 = &tb_vlan1_p->tb_vlan_2;
	       }
       } else if (tb_vlan1_p->tb_vlan_2 != tb_vlan2_p->isl_vlan_id) {
	   tb_vlan1_p->tb_vlan_1 = tb_vlan2_p->isl_vlan_id;
	   restore_loc3 = &tb_vlan1_p->tb_vlan_1;
       }
       if (tb_vlan2_p->tb_vlan_1) {
	   if (tb_vlan2_p->tb_vlan_1 != tb_vlan1_p->isl_vlan_id) 
	       if (tb_vlan2_p->tb_vlan_2) {
		   if (tb_vlan2_p->tb_vlan_2 != tb_vlan1_p->isl_vlan_id) {
		       if (restore_location) {
			   *restore_location = 0;
		       }
		       if (restore_loc2) {
			   *restore_loc2 = 0;
		       }
		       if (restore_loc3) {
			   *restore_loc3 = 0;
		       }
		       return FALSE;
		   }
	       } else {
		   tb_vlan2_p->tb_vlan_2 = tb_vlan1_p->isl_vlan_id;
	       }
       } else if (tb_vlan2_p->tb_vlan_2 != tb_vlan1_p->isl_vlan_id) {
	   tb_vlan2_p->tb_vlan_1 = tb_vlan1_p->isl_vlan_id;
       }
   }
   return (TRUE);
}


/* Called from vtp_configure_vlan to patch up the database when a
 * VLAN's translation values are being cleared. Since the
 * vtp_configure_vlan may only change one VLAN at a time it cannot
 * resolve dangling references from others VLANs. This function zeros
 * out those references.
 */
void vtp_delete_tlb_entries (vlan_domain_ptr domain_p,
			     vtp_vlan_info* v_info_p) {
 
   vlan_info_ent_ptr vlan_p, tb_vlan1_p, tb_vlan2_p;
 
   vlan_p = vtp_search_info_entry_by_vlan(domain_p, v_info_p->isl_vlan_id);
   if (!vlan_p)
       return;

   tb_vlan1_p = vtp_search_info_entry_by_vlan(domain_p,
					      vlan_p->tb_vlan_1); 
   if (tb_vlan1_p) {
       if (tb_vlan1_p->tb_vlan_1 == v_info_p->isl_vlan_id) {
	   tb_vlan1_p->tb_vlan_1 = 0;
       } else if (tb_vlan1_p->tb_vlan_2 == v_info_p->isl_vlan_id) {
	   tb_vlan1_p->tb_vlan_2 = 0;
       }
   }
	   
   tb_vlan2_p = vtp_search_info_entry_by_vlan(domain_p,
					      vlan_p->tb_vlan_2); 
 
   if (tb_vlan2_p) {
        if (tb_vlan2_p->tb_vlan_1 == v_info_p->isl_vlan_id) {
	   tb_vlan2_p->tb_vlan_1 = 0;
       } else if (tb_vlan2_p->tb_vlan_2 == v_info_p->isl_vlan_id) {
	   tb_vlan2_p->tb_vlan_2 = 0;
       }
    }
}


/* The following function take an internal data representation of an
 * element (vlan) and fills in the external representation (found in
 * vtp_interface.h) with equivalent values.  
 */ 
void retrieve_vlan_info (vlan_info_ent_ptr info_p, vtp_vlan_info* info)
{
    info->length = sizeof(vtp_vlan_info);
    info->vlan_name_length = info_p->name_length;
 
    memcpy(info->vlan_name, info_p->vlan_name, info_p->name_length);
    info->vlan_name[info->vlan_name_length] = '\0';
 
    info->type = info_p->type;
    info->mtu  = info_p->mtu;
 
    switch (info_p->status) {
    case VLAN_OPERATIONAL:
	info->state = VLAN_OPERATIONAL_STATE;
	break;
    case VLAN_SUSPENDED:
	info->state = VLAN_SUSPENDED_STATE;
	break;
    case VLAN_MTU_TOO_BIG_FOR_DEVICE:
	info->state = VLAN_MTU_TOO_BIG_FOR_DEVICE_STATE;
	break;
    case VLAN_MTU_TOO_BIG_FOR_TRUNK:
	info->state = VLAN_MTU_TOO_BIG_FOR_TRUNK_STATE;
	break;
    }
 
    info->said          = info_p->said;
    info->isl_vlan_id   = info_p->isl_vlan_id;
    info->ring_number   = info_p->ring_number;
    info->parent_vlan   = info_p->parent_vlan;
    info->bridge_number = info_p->bridge_number;
    info->parent_valid  = info_p->parent_valid;
    info->stp_type      = info_p->stp_type;
    info->tb_vlan_1     = info_p->tb_vlan_1;
    info->tb_vlan_2     = info_p->tb_vlan_2;
}


/* The following function take an internal data representation of an
 * element (domain) and fills in the external representation (found in
 * vtp_interface.h) with equivalent values.  
 */ 
void retrieve_domain_info (vlan_domain_ptr domain_p, vtp_domain_info* info)
{
    info->mode = (unsigned long)domain_p->mode;
    info->domain_name_length = domain_p->dname_length;
    memcpy(info->domain_name, domain_p->domain_name,
	     domain_p->dname_length); 
    info->domain_name[domain_p->dname_length] = '\0';
    memcpy(info->timestamp, domain_p->timestamp, VTP_TIMESTAMP_SIZE);
    memcpy(info->md5_digest, domain_p->md5_digest, VTP_MD5_DIGEST_SIZE);

    info->revision        = domain_p->revision;
    info->updater         = domain_p->updater;
    info->index           = domain_p->index;
    info->advt_interval   = domain_p->advt_interval / VTP_ONE_SECOND;
    info->password_length = domain_p->password_length;
 
    memcpy(info->password, domain_p->password,
	     domain_p->password_length); 
 
    info->vlan_count = domain_p->vlan_count;
    info->tftp_server = domain_p->tftp_server;
    memcpy(info->tftp_file_name, domain_p->tftp_file_name,
	     VTP_MAX_TFTP_FILENAME_LENGTH + 1);
    info->pruning_mode = domain_p->pruning_mode;
}


/* The following function take an internal data representation of an
 * element (trunk) and fills in the external representation (found in
 * vtp_interface.h) with equivalent values.  
 */ 
void retrieve_trunk_info (vtp_trunk_itf_ptr trunk_p, 
			  vtp_trunk_info* info) {
    info->trunk_id = trunk_p->trunk_id;
    memcpy(info->trunk_name, trunk_p->trunk_name,
	   VTP_MAX_TRUNK_NAME_LENGTH + 1);
    info->native_vlan = trunk_p->native_vlan;
    info->encaps_type = trunk_p->encaps_type;
    memcpy(info->vlans_enabled, trunk_p->vlans_enabled,
	    VLANS_BITMASK_SIZE); 
    memcpy(info->vlans_pruning_eligible,
	   trunk_p->vlan_pruning_eligibility, VLANS_BITMASK_SIZE);
    info->dname_length = trunk_p->domain_p->dname_length;
    memcpy(info->domain_name, trunk_p->domain_p->domain_name,
	    info->dname_length); 
    info->domain_name[info->dname_length] = '\0';
}

/*
 * Returns TRUE if vlan_id is default, FALSE otherwise.
 */
vtp_boolean vtp_is_default_vlan_id (unsigned short isl_vlan_id)
{
    switch (isl_vlan_id) {
    case VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL:
    case VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL:
    case VTP_FACTORY_DEFAULT_VLAN_TR_ISL:
    case VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL:
    case VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL:
	return (TRUE);
    default:
	return (FALSE);
    } 
}



/* 
 * compare two tyte strings in lexicographical order 
 */
long name_cmp (unsigned char *name1, unsigned char *name2, 
	      unsigned long length1, unsigned long length2) {

    unsigned long ix;
    unsigned long length;
    
    length = length1 > length2 ? length2 : length1;
    
    for (ix = 0; ix < length; ix++)  {
        if (name1[ix] < name2[ix])
            return (NAME_LESS);
        else if (name1[ix] > name2[ix])
            return (NAME_GREATER);
    }
    
    if (length1 > length2)
        return (NAME_GREATER);
    else if (length1 < length2)
        return (NAME_LESS);
    else
        return (NAME_EQUAL);
}


/*
 * This routine is invoked when domain is changed to server mode.
 * In server mode send out vtp summaries on each active trunk link.
 */
void vtp_change_to_server (vlan_domain_ptr domain_p)
{
  if (domain_p->dname_length == 0)
    return;

  vtp_do_flooding(domain_p, 0, 0);
 
}



/*
 * This routine is invoked when domain is changed to client mode.
 * In client mode send out vtp requests on each active trunk link.
 */
void vtp_change_to_client (vlan_domain_ptr domain_p)
{
   vtp_trunk_itf_ptr trunk_p;

   if (domain_p->dname_length == 0)
     return;

   for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	trunk_p->next_trunk_in_domain) {
       vtp_transmit_adv_request_packet(trunk_p, 0);
   }
}



/*
 * This routine is invoked when domain is changed to transparent mode.
 * On entering transparent mode stop all active timers, flush any
 * updates in process.
 */
void vtp_change_to_transparent (vlan_domain_ptr domain_p)
{
   vtp_trunk_itf_ptr trunk_p;
 
   domain_p->revision = 0;
 
   /* stop all timers on trunk links associated with this domain */
   for (trunk_p = domain_p->trunk_head; trunk_p; 
	trunk_p = trunk_p->next_trunk_in_domain) {
       if (vtp_timer_active(trunk_p->advt_timer_p)) {
	   vtp_stop_timer(trunk_p->advt_timer_p);
       }
       
       if (vtp_timer_active(trunk_p->rqst_timer_p)) {
	   vtp_stop_timer(trunk_p->rqst_timer_p);
       }
       
   }
 
   /* flush the update data structures */
   vtp_flush_update_adv_info(domain_p);
 
   /* flush the transmit packet queue */
   vtp_flush_tx_subset_adv_queue(domain_p);
 
}



/*
 * returns TRUE if rev number in vtp packet is less than domain rev number
 */
vtp_boolean vtp_update_revision_less_than_domain (unsigned long update_rev,
					      unsigned long domain_rev)
{
    /* 
     * domain rev number 0 is always less than any rev number rcvd in
     * vtp packet.
     */
    if (!domain_rev)
	return (FALSE);
    
    if ((update_rev < domain_rev) && 
	((domain_rev - update_rev) < VTP_BIG_REV))
	return (TRUE);
    
    if ((update_rev > domain_rev) &&
	((update_rev - domain_rev) > VTP_BIG_REV))
	return (TRUE);
    
    return (FALSE);

}



/*
 * returns TRUE if rev number in vtp packet is equal to domain rev number
 */
vtp_boolean vtp_update_revision_equal_to_domain (unsigned long update_rev,
                                               unsigned long domain_rev)
{
    /* 
     * domain rev number 0 is always less than any rev number rcvd in
     * vtp packet.
     */
    if (!domain_rev)
	return (FALSE);
    
    if (update_rev == domain_rev)
	return (TRUE);
    
    return (FALSE);
    
}
