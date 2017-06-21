/* $Id: vtp_parse.c,v 1.2.58.7 1996/06/29 02:13:53 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_parse.c,v $
 *------------------------------------------------------------------
 * vtp_parse.c - VTP CLI parser.
 *
 * Dec 1995, Dean Cheng 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_parse.c,v $
 * Revision 1.2.58.7  1996/06/29  02:13:53  cyoung
 * CSCdi61713:  make sure domain name length does not exceed 32 bytes from
 * CLI
 * Branch: California_branch
 *
 * Revision 1.2.58.6  1996/06/26  02:18:54  cyoung
 * Retrieving or configuring more than one trunk a problem.  Also
 * reformat some parser output.
 * CSCdi61201:  On 7k: Trunk link shows on one interface(I have two atm
 * int)only.
 * Branch: California_branch
 *
 * Revision 1.2.58.5  1996/06/24  23:23:40  cyoung
 * CSCdi61160:  VTP password length is not checked
 * Branch: California_branch
 *
 * Revision 1.2.58.4  1996/06/20  19:50:29  cyoung
 * Use correct VLAN count to calculate TFTP file size
 * CSCdi60853:  wrong behaviour while reading tftp file --> vlan info
 * recovery fail Branch: California_branch
 * Also while bonnet is up: Put in a limiting depth in for waiting for
 * semaphore to be released.  Allow null domain to receive domain change
 * notifications.  Provide function to manually load TFTP database and
 * load the database not only when TFTP server or file change but also
 * VTP entity state (client --> server).
 *
 * Revision 1.2.58.3  1996/06/18  00:51:10  cyoung
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
 * Revision 1.2.58.2  1996/05/21  22:29:35  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.1  1996/05/09  14:48:38  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:41  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.2  1996/04/16  18:59:04  cyoung
 * Using 'exit','end' or '^Z' as an implicit apply breaks when the parser
 * needs to pop out of the sub-command scope to check against the global
 * parse chain.  Make 'apply' an explicit command to avoid this blunder.
 * CSCdi54003:  write to tftp file when it shouldnt, doesnt when it should
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.3  1996/04/03  22:37:02  cyoung
 * New objects for TFTP support, ETR vlan type and trunk table creation
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MIB - TFTP support, ETR type allowed, still need work on Trunk creation
 * dep - use temp buf to determine add/mod mode instead of real database
 * core - add tooBig hook for trap generation
 *
 * Revision 1.2.64.2  1996/03/25  02:21:55  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:45:07  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.9  1996/03/23  02:21:03  cyoung
 * Quick fix for database exit command.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.8  1996/03/22  03:16:05  bbenson
 * Branch: LE_Cal_V111_1_0_3_branch
 * Rearrange cadillac fins on the VTP command parser.  Install
 * electroglide subcommand modes for vtp-domain and vtp-database commands.
 * Running lights coming in next commit.
 *
 * Revision 1.2.36.7  1996/03/15  18:57:54  cyoung
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
 * Revision 1.2.36.6  1996/03/13  17:28:09  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.5  1996/03/08  18:19:04  cyoung
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
 * Revision 1.2.36.4  1996/03/07  02:35:17  cyoung
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
 * Revision 1.2.36.3  1996/03/05  21:42:55  cyoung
 * Make the new CLI commands more civilized.  Start to add basic nvgen
 * support for VTP commands.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.2  1996/03/05  01:56:23  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:34  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.8  1996/02/26  18:07:30  percyk
 * more debug messages.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.7  1996/02/22  04:42:58  percyk
 * remove basic typedefs from core code for portability reasons.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.6  1996/02/12  21:32:39  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.2.4.5  1996/02/09  01:10:03  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.2.4.4  1996/02/08  04:12:38  cyoung
 * Set default time interval when creating or modifying management domain.
 * Add support for setting vlan filter on trunks and the native vlan.
 * Add support for transmitting adverts over LANE.
 * Add code for removing trunks from idblist.
 * Add interface for core code to notify dependant code when a mod has
 *  been made to the trunk list.
 * More code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.3  1996/01/24  01:38:00  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.2  1996/01/23  00:47:20  percyk
 * Branch: LE_Cal_V111_0_16_branch
 * add default vlans at startup time.  add meat to debug files.
 *
 * Revision 1.2.4.1  1996/01/11  01:47:53  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.1.2.4  1996/01/06  00:37:44  dcheng
 * Update the VTP packet format to include ring number and parent VLAN for
 * FDDI/TR VLAN, bridge number and STP type for FDDI/TR net VLAN, and
 * translational bridged VLAN id (0, 1 or 2). Update the database and
 * user interface for these as well.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.3  1996/01/04  00:38:10  dcheng
 * Delete the boolean flag "vtp_enabled" in the idb structure, clean up the
 * related code. Fix some bugs in the CLI for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.2  1996/01/03  01:49:23  dcheng
 * Submit CLI code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 1.1.2.1  1995/12/23  02:04:00  dcheng
 * Branch: LE_Calif_branch
 *
 * Revision 1.1  1995/12/23  02:02:50  dcheng
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "sys_registry.h"
#include "interface_private.h"
#include "ieee.h"
#include "config.h"
#include "address.h"
#include "parser.h"
#include "../if/network.h"
#include "parser_defs_vtp.h"
#include "../parser/parser_defs_exec.h"
#include "../srt/srt_registry.h"
#include "../srt/span.h"
#include "../ui/debug.h"
#include "ctype.h"
#include "vtp_externs.h"
#include "vtp_interface.h"
#include "vtp_pub.h"
#include "vtp_dep.h"

/* also declared in sr_vtpmib.h */
extern unsigned long vtp_dep_get_local_ipaddr(void);


void *vtp_save_var (parseinfo *csb)
{
    /* upon mode change back to others */
    /* vtp_cli_database_abort(); */
    return(NULL);
}
 
void vtp_reset_var (parseinfo *csb, void *var)
{
    /* never called ? */
    printf("reset var\n");          
}


/*
 * VTP global config commands.
 *
 *    [no] vtp domain <domain-name>
 *		[{server | client}]
 *		[interval <value>]
 *		[password <password>]
 *    [no] vtp database domain <domain-name>
 */
void vtp_config_commands (parseinfo *csb)
{
    vtp_domain_info *domain_info_ptr;
    vtp_vlan_info vlan_info;
    unsigned char error;
    unsigned char mode;

    domain_info_ptr = csb->vtp_domain;
    error = 0;

    if (csb->nvgen) {
	if (!domain_info_ptr)
	    domain_info_ptr =
		malloc_named(sizeof(vtp_domain_info), "vtp_cli");
	if (!domain_info_ptr)
	    return;
	switch (csb->which) {
	case VTP_DOMAIN:
	    if (vtp_get_domain_info(domain_info_ptr, VTP_FIRST))
		break;
	    
	    while (!error) {
		nv_write(TRUE, "!\n%s", csb->nv_command);
		nv_add(TRUE, " %s", domain_info_ptr->domain_name);
		if (domain_info_ptr->mode == VTP_CLIENT) {
		    nv_write(TRUE, " client");
		} else {
		    nv_write(TRUE, 
			     " server %i %s", 
			     domain_info_ptr->tftp_server,
			     domain_info_ptr->tftp_file_name);
		}
		
		nv_write(domain_info_ptr->advt_interval !=
			 VTP_DEFAULT_ADVT_INTERVAL, 
			 " interval %d",
			 domain_info_ptr->advt_interval);
		
		nv_write(domain_info_ptr->password_length != 0, 
			 " password %s", domain_info_ptr->password);

		nv_write(domain_info_ptr->pruning_mode, " pruning");
		error = vtp_get_domain_info(domain_info_ptr,
					    VTP_NEXT); 
	    }
	    break;
	case VTP_DATABASE:
	    if (vtp_get_domain_info(domain_info_ptr, VTP_FIRST))
		break;
	    while (!error) {
		nv_write(TRUE, "!\n%s", csb->nv_command);
		nv_add(TRUE, " %s", domain_info_ptr->domain_name);
		error =
		    vtp_get_vlan_info(domain_info_ptr->domain_name_length,
				      domain_info_ptr->domain_name,
				      &vlan_info, VTP_FIRST);
		while (!error) {
		    nv_write(TRUE, " vlan %d name %s",
			     vlan_info.isl_vlan_id, vlan_info.vlan_name);
		    nv_write(TRUE, " vlan %d sde %d",
			     vlan_info.isl_vlan_id,
			     vlan_info.said);
		    nv_write(vlan_info.state !=
			     VLAN_OPERATIONAL_STATE, 
			     " vlan %d state %s",
			     vlan_info.isl_vlan_id, "suspended");
		    nv_write(vlan_info.mtu != VTP_VLAN_DEFAULT_MTU,
			     " vlan %d mtu %d", vlan_info.isl_vlan_id,
			     vlan_info.mtu);
		    nv_write(vlan_info.type != VLAN_ETH_TYPE, 
			     " vlan %d ", vlan_info.isl_vlan_id);
		    switch (vlan_info.type) {
		    case VLAN_ETH_TYPE:
			break;
		    case VLAN_FDDI_TYPE:
			nv_add(TRUE, "fddi");
			nv_add(vlan_info.parent_valid, 
			       " ring-number %d parent-vlan %d",
			       vlan_info.ring_number,
			       vlan_info.parent_vlan);
			break;
		    case VLAN_FDDI_NET_TYPE:
			nv_add(TRUE, "%s bridge-number %d stp-type %s", 
			       "fddi-net",
			       vlan_info.bridge_number,
			       vlan_info.stp_type == VLAN_STP_IEEE
			       ? "IEEE" : "IBM"); 
			break;
		    case VLAN_TR_TYPE:
			nv_add(TRUE, "%s", "tokenring");
			nv_add(TRUE, " ring-number %d parent-vlan %d", 
			       vlan_info.ring_number,
			       vlan_info.parent_vlan);
			break;
		    case VLAN_TR_NET_TYPE:
			nv_add(TRUE, "%s bridge-number %d stp-type %s", 
			       "tr-net",
			       vlan_info.bridge_number,
			       vlan_info.stp_type == VLAN_STP_IEEE
			       ? "IEEE" : "IBM"); 
		    default:
		    }
		    nv_write(vlan_info.tb_vlan_1, 
			     " vlan %d tlb-1 %d",
			     vlan_info.isl_vlan_id,
			     vlan_info.tb_vlan_1);
		    nv_add(vlan_info.tb_vlan_2, " tlb-2 %d",
			   vlan_info.tb_vlan_2);
		    error =
			vtp_get_vlan_info(domain_info_ptr->domain_name_length,
					  domain_info_ptr->domain_name, 
					  &vlan_info, VTP_NEXT); 
		}
		nv_write(TRUE, " apply");
		error = vtp_get_domain_info(domain_info_ptr, VTP_NEXT);
	    }
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	
	return;
    }
    
    switch (csb->which) {
    case VTP_DOMAIN:

	/*
	 * [no] vtp domain <domain-name>
	 *
	 *		[server <tftp-server> <file-name>]
	 *		[client]
	 *          [interval <retransmit-seconds>]
	 *          [password <password>]
	 *
	 * OBJ(string,1) = <domain-name>
	 * OBJ(int,1) = Logical OR of VTP_DOMAIN_PASSWORD,
	 *  VTP_DOMAIN_ADVT_INT
	 * OBJ(paddr,1) = <TFTP server>
	 * OBJ(string,2) = <new-domain-name>
	 * OBJ(int,2) = <retransmit-seconds>
	 * OBJ(string,3) = <password>
	 * OBJ(string,4) = <TFTP file name>
	 */

	if (!csb->vtp_domain) {
	    domain_info_ptr = malloc_named(sizeof(vtp_domain_info),
					   "vtp_cli");
	    csb->vtp_domain = domain_info_ptr;
	}
	domain_info_ptr->domain_name_length =
	    strlen(GETOBJ(string, 1));

	if (domain_info_ptr->domain_name_length >
	    VTP_MAX_DOMAIN_NAME_LENGTH) {
	    printf("VTP domain name cannot exceed 32 characters\r\n");
	    return;
	}

	memcpy(domain_info_ptr->domain_name, 
	       GETOBJ(string, 1),
	       domain_info_ptr->domain_name_length);
	error = vtp_get_domain_info(domain_info_ptr, VTP_EXACT);
	if (csb->sense) {
	    vtp_start_process();
	    set_mode_byname(&csb->mode, "vtp-domain", MODE_VERBOSE); 
	    
	    /*
	     * If no domain, add a default by the name asked for.
	     */
	    if (error) {
		domain_info_ptr->mode = VTP_CLIENT;
		domain_info_ptr->revision = VTP_DEFAULT_REVISION,
		domain_info_ptr->index = VTP_DEFAULT_DOMAIN_INDEX;
		domain_info_ptr->updater =
		    vtp_dep_get_local_ipaddr(); 
		vtp_get_timestamp(domain_info_ptr->timestamp);
		domain_info_ptr->advt_interval =
		    VTP_DEFAULT_ADVT_INTERVAL; 
		domain_info_ptr->password_length =
		    VTP_DEFAULT_PASSWORD_LENGTH; 
		domain_info_ptr->password[0] = 0;
		domain_info_ptr->tftp_file_name[0] = '\0';
		domain_info_ptr->tftp_server = 0;
		domain_info_ptr->pruning_mode = DEFAULT_VTP_PRUNING_MODE;
		
		vtp_post_configure_domain(VTP_ADD,
					  domain_info_ptr); 
	    }
	} else {
	    if (error) {
		printf("VTP: Domain does not exist\r\n");
		return;
	    }
	    vtp_post_configure_domain(VTP_DELETE, domain_info_ptr);
	}
	break;
	
    case VTP_DOMAIN_PASSWORD:
	if (!domain_info_ptr) {
	    return;
	}

	if ((strlen(GETOBJ(string, 3)) < 8) || 
	    (strlen(GETOBJ(string,3)) > 64)) {
	    printf("Password of incorrect length\r\n");
	    return;
	}

	if (csb->sense) {
	    memcpy(domain_info_ptr->password, GETOBJ(string, 3),
		   strlen(GETOBJ(string, 3)));
	    domain_info_ptr->password_length = strlen(GETOBJ(string,3));
	} else {
	    domain_info_ptr->password_length = VTP_DEFAULT_PASSWORD_LENGTH;
	    domain_info_ptr->password[0] = 0;
	}
	vtp_post_configure_domain(VTP_MODIFY, domain_info_ptr);
	break;
	
    case VTP_DOMAIN_INTERVAL:
	if (!domain_info_ptr) {
	    return;
	}
	if (csb->sense) {
	    domain_info_ptr->advt_interval = GETOBJ(int, 2);
	} else {
	    domain_info_ptr->advt_interval = VTP_DEFAULT_ADVT_INTERVAL;
	}
	vtp_post_configure_domain(VTP_MODIFY, domain_info_ptr); 
	break;

    case VTP_DOMAIN_PRUNING:
	if (!domain_info_ptr) {
	    return;
	}

	domain_info_ptr->pruning_mode = csb->sense;
	vtp_post_configure_domain(VTP_MODIFY, domain_info_ptr);
	break;

    case VTP_DOMAIN_SERVER:
	if (!domain_info_ptr) {
	    return;
	}
	if (csb->sense) {
	    domain_info_ptr->mode = VTP_SERVER;
	    domain_info_ptr->tftp_server = GETOBJ(paddr, 1)->ip_addr;
	    strcpy(domain_info_ptr->tftp_file_name, GETOBJ(string, 4));
	} else {
	    domain_info_ptr->mode = VTP_CLIENT;
	}
	vtp_post_configure_domain(VTP_MODIFY, domain_info_ptr);
	break;
    case VTP_DOMAIN_CLIENT:
	if (!domain_info_ptr) {
	    return;
	}
	domain_info_ptr->mode = VTP_CLIENT;
	vtp_post_configure_domain(VTP_MODIFY, domain_info_ptr);
	break;
    case VTP_DATABASE:
	/*
	 * vtp database domain <domain-name>
	 * no vtp database domain <domain-name>
	 *
	 * OBJ(string, 1) = domain name
	 */
	if (!domain_info_ptr)
	    domain_info_ptr = malloc_named(sizeof(vtp_domain_info),
					   "vtp_cli"); 
	if (!domain_info_ptr)
	    return;

	domain_info_ptr->domain_name_length = 
	    strlen(GETOBJ(string, 1)); 
	memcpy(domain_info_ptr->domain_name, GETOBJ(string, 1),
	       domain_info_ptr->domain_name_length); 
	csb->vtp_domain = domain_info_ptr;

	error = vtp_get_domain_info(domain_info_ptr, VTP_EXACT);
	    
	/*
	 * If the domain is not found go ahead and enter the
	 * sub-command scope.  It is most likely being loaded from
	 * NVRAM.
	 */
	if ((!error) && (domain_info_ptr->mode != VTP_SERVER)) {
	    printf("VTP: Must be server to modify database.\n");
	    return;
	}

	if (csb->sense) {

	    set_mode_byname(&csb->mode, "vtp-database", MODE_VERBOSE);
	    
	    /* purge the chain list possibly left over from last time */
	    if (vtp_cli_database_init(domain_info_ptr))
		return;

	    /* in editing mode for database */

	} else  {
	    /* delete the specified database */
	    vtp_post_delete_database(domain_info_ptr->domain_name_length, 
				     domain_info_ptr->domain_name); 
	}
	break;

    case VTP_DATABASE_VLAN:
	
	/*           
	 * [no] vlan <vlan-id>
	 *     [name <vlan-name>]
	 *     [sde <said>]
	 *     [state {operational | suspended}]
	 *     [mtu <mtu>]
	 *     [{ethernet |
	 *      {fddi | tokenring} ring-number <ring-number>
	 *       parent-vlan <vlan> | 
	 *      {fdi-net | tr-net} bridge-number <bridge-number>
	 *       stp-type {srt | srb }}]
	 *
	 *  OBJ(int, 1) = ISL VLAN ID
	 *  OBJ(string, 1) = VLAN Name
	 *  OBJ(int, 2) = Logical OR of VTP_VLAN_NAME,
	 *   VTP_VLAN_SAID, VTP_VLAN_STATE, VTP_VLAN_MTU, VTP_VLAN_RING,
	 *   VTP_VLAN_PARENT, VTP_VLAN_BRIDGE, VTP_VLAN_STP
	 *  OBJ(int, 3) = IEEE 801.10 SAID
	 *  OBJ(int, 4) = MTU
	 *  OBJ(int, 5) = VTP_VLAN_OPERATIONAL_STATE or
	 *   VTP_VLAN_SUSPENDED_STATE 
	 *  OBJ(int, 6) = VTP_VLAN_ETH_TYPE, VTP_VLAN_FDDI_TYPE,
	 *   VTP_VLAN_TR_TYPE, VTP_VLAN_FDDI_NET_TYPE, or
	 *   VTP_VLAN_TR_NET_TYPE 
	 *  OBJ(int, 7) = ring number
	 *  OBJ(int, 8) = parent vlan id
	 *  OBJ(int, 9) = bridge number
	 *  OBJ(int, 10) = STP type
	 *  OBJ(int, 11) = translational bridged VLAN 1
	 *  OBJ(int, 12) = translational bridged VLAN 2
	 */

	/* get VLAN id */
	vlan_info.isl_vlan_id = GETOBJ(int,1);

	if (csb->sense) {

	    if (!vtp_cli_database_find(&vlan_info,
				       vlan_info.isl_vlan_id)) {
		mode = VTP_ADD; 
		VTP_VLAN_DEFAULT_NAME(vlan_info.isl_vlan_id,
				      vlan_info.vlan_name,
				      &vlan_info.vlan_name_length); 
		vlan_info.said =
		    VTP_VLAN_DEFAULT_SAID(vlan_info.isl_vlan_id);
		vlan_info.state = VTP_VLAN_DEFAULT_STATUS;
		vlan_info.mtu = VTP_VLAN_DEFAULT_MTU;
		vlan_info.tb_vlan_1 = 0;
		vlan_info.tb_vlan_2 = 0;
		vlan_info.type = VTP_VLAN_ETH_TYPE; 
		vlan_info.ring_number = 0;
		vlan_info.parent_vlan = 0;
		vlan_info.bridge_number = 0;
		vlan_info.stp_type = 0;
	    } else
		mode = VTP_MODIFY;

	    /* get VLAN name */
	    if (GETOBJ(int, 2) & VTP_VLAN_NAME) {
		vlan_info.vlan_name_length = strlen(GETOBJ(string, 1));
		memcpy(vlan_info.vlan_name, GETOBJ(string, 1),
		       vlan_info.vlan_name_length); 
	    } 

	    /* get VLAN said */
	    if (GETOBJ(int, 2) & VTP_VLAN_SAID) 
		vlan_info.said = GETOBJ(int, 3);
        
	    /* get VLAN state */
	    if (GETOBJ(int, 2) & VTP_VLAN_STATE) 
		vlan_info.state = GETOBJ(int, 5);

	    /* get VLAN MTU */
	    if (GETOBJ(int, 2) & VTP_VLAN_MTU) 
		vlan_info.mtu = GETOBJ(int, 4);

	    /* get translational-bridge 1 */
	    if (GETOBJ(int, 2) & VTP_VLAN_TLB1)
		vlan_info.tb_vlan_1 = GETOBJ(int, 11);

	    /* get translational-bridge 2 */
	    if (GETOBJ(int, 2) & VTP_VLAN_TLB2) 
		vlan_info.tb_vlan_2 = GETOBJ(int, 12);
	    
	    /* get VLAN type */
	    if (GETOBJ(int,6))
		vlan_info.type = GETOBJ(int, 6);
	    
	    /* get VLAN optional parameters */
	    if (GETOBJ(int, 13) ||(GETOBJ(int, 6) == VTP_VLAN_TR_TYPE)) {
		vlan_info.ring_number = GETOBJ(int, 7);
		vlan_info.parent_vlan = GETOBJ(int, 8);
	    } else if ((GETOBJ(int, 6) == VTP_VLAN_FDDI_NET_TYPE) ||
		       (GETOBJ(int, 6) == VTP_VLAN_TR_NET_TYPE))  {
		vlan_info.bridge_number = GETOBJ(int, 9);
		vlan_info.stp_type = GETOBJ(int, 10);
            } 
	} else {
	    vlan_info.vlan_name_length = 0;
	    mode = VTP_DELETE;
	}	

	if(vtp_cli_database_editing(mode, &vlan_info)) {
	    printf("\nVTP: Caught an internal error.  Sub-command "
		   "aborted.\n");  
	    vtp_cli_database_abort();
	    exit_config_submode(csb);
	}
	break;
	
    case VTP_DATABASE_ABORT:
	exit_config_submode(csb);
	vtp_cli_database_abort();
	break;
    case VTP_DATABASE_APPLY:
	vtp_cli_database_apply(domain_info_ptr, csb->resolvemethod);
	exit_config_submode(csb);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}




/*
 * VTP trunk config commands.
 *
 *    [no] vtp trunk domain <domain-name>
 */
void vtp_intfc_command (parseinfo *csb)
{
    idbtype *idb, *swidb;
    vtp_subif_subidb *subif_sb;
    unsigned char error;
    swidb_sb_t sb_type;
    vtp_trunk_info t_info;

    idb = csb->interface;
    
    if (csb->nvgen) {
	
	switch (csb->which) {
	case VTP_TRUNK:
	    t_info.trunk_id = idb;
	    error = vtp_get_trunk_info(&t_info, VTP_EXACT);
	    nv_write(error == VTP_OK, "%s %s", csb->nv_command,
		    &t_info.domain_name);
	    break;
	case VTP_CONFIGURED:
	    subif_sb = idb_get_swsb(idb, SWIDB_SB_VTP_SUB);
	    nv_write((subif_sb != NULL) && (subif_sb->vtp_enabled), 
                     "%s", csb->nv_command);
	    break;
	case VTP_TRUNK_PRUNING: {
	    unsigned char eligible_mask[VLANS_BITMASK_SIZE];
	    unsigned short ix;

	    if (!vtp_core_get_trunk_eligibility(idb, eligible_mask))
		break;

#define FIRST_NON_DEFAULT_VLAN 2	   
#define MAXIMUM_NON_DEFAULT_VLAN 1000
	    for (ix = FIRST_NON_DEFAULT_VLAN; ix <= MAXIMUM_NON_DEFAULT_VLAN;
		 ix++) {
		nv_write(!(eligible_mask[vlan_bit_offset(ix, 0)] & 
			 vlan_bit_mask(ix, 0)), "%s %d", csb->nv_command, 
			 ix);
	    }
	    break;
	}
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	
	return;
    }
    
    switch (csb->which) {
	
    case VTP_TRUNK:  
	t_info.dname_length = strlen(GETOBJ(string,1));
	memcpy(t_info.domain_name, GETOBJ(string, 1),
	       t_info.dname_length);
	t_info.trunk_id = idb;
	strcpy(t_info.trunk_name, idb->namestring);
	if ((!idb->hwptr) || (idb != idb->hwptr->firstsw)) {
	    printf("\nVTP: Trunk command not valid on sub-interfaces");
	    return;
	}

	/* trunk HAS to be either ISL, dot10 or ATM at this point */
 
	if ( idb_is_fastethernet(idb) ) {
	    t_info.encaps_type = VTP_TRUNK_ENCAPS_ISL;
	    t_info.native_vlan = 0;
	} else if ( idb_is_fddi(idb) ) {
	    t_info.encaps_type = VTP_TRUNK_ENCAPS_DOT10;
	    t_info.native_vlan = VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL;
	} else  if ( idb_is_atm(idb) ) {
	    t_info.encaps_type = VTP_TRUNK_ENCAPS_LANE;
	    t_info.native_vlan = 0;
	} else {
	    printf("\nVTP: Trunking not supported on specified interface");
	    return;
	}
	
	memset(t_info.vlans_enabled, 0xff, VLANS_BITMASK_SIZE);
	if (csb->sense) {
	    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH + 1];
	    unsigned char dname_length;
	    unsigned short vlan_id;
	    vtp_vlan_info v_info;

            vtp_post_configure_trunk(VTP_ADD, &t_info);
	    FOR_ALL_SWIDBS_ON_HW(idb->hwptr, swidb) {
		if (!swidb->tbridge_on_swidb)
		    continue;

		if (!reg_invoke_vtp_find_domain_by_bridge_group(swidb->span_index,
								domain_name, 
								&dname_length))
		    continue;

		/* If this sub-interface does not belong to the domain
		 * of the trunk then skip it.
		 */
		if ((t_info.dname_length != dname_length) || 
		    (memcmp(domain_name, t_info.domain_name,
			    dname_length) != 0)) 
		    continue;

		vlan_id =
		    reg_invoke_vtp_find_vlan_by_bridge_group(swidb->span_index);  

		if (!vlan_id)
		    continue;

		v_info.isl_vlan_id = vlan_id;
		if (vtp_get_vlan_info(dname_length, domain_name,
				      &v_info, VTP_EXACT) != VTP_OK) 
		    continue;
		
		vtp_post_change_trunk_vlan_stp_state(t_info.dname_length, 
						     t_info.domain_name, 
						     vlan_id,
						     t_info.trunk_id,
						     swidb->port_state ==  
						     PORT_FORWARDING ?
						     VLAN_SPT_STATE_FWD :
						     VLAN_SPT_STATE_NON_FWD); 
	    }
	} else {
            vtp_post_configure_trunk(VTP_DELETE, &t_info);
	}
	break;
	
    case VTP_CONFIGURED:
	subif_sb = idb_get_swsb(idb, SWIDB_SB_VTP_SUB);
	if (csb->sense) {
	    unsigned char domain_name[VTP_MAX_DOMAIN_NAME_LENGTH + 1];
	    unsigned char dname_length;
	    unsigned long vlan_id;
	    boolean is_joined;

	    if (subif_sb) {
		subif_sb->vtp_enabled = TRUE;
	    } else {
		subif_sb = malloc(sizeof(vtp_subif_subidb));
		if (!subif_sb)
		    break;
		subif_sb->vtp_enabled = TRUE;
		subif_sb->is_pruned = TRUE;
		sb_type = SWIDB_SB_VTP_SUB;
		if (!idb_add_swsb(idb, &sb_type, subif_sb)) 
		    free(subif_sb);
	    }

	    subif_sb->is_pruned = FALSE; /* default in case something
					  * goes wrong
					  */
	    if (!idb->tbridge_on_swidb)
		break;
	    if (!reg_invoke_vtp_find_domain_by_bridge_group(idb->span_index, 
							    domain_name, 
							    &dname_length))
		break;

	    vlan_id =
		reg_invoke_vtp_find_vlan_by_bridge_group(idb->span_index); 

	    if (!idb->hwptr)
		break;
		
	    if (vtp_core_get_vlan_pruning_status(dname_length,
						 domain_name,
						 idb->hwptr->firstsw,
						 vlan_id, &is_joined)) 
		break;

	    subif_sb->is_pruned = !is_joined;

	    vtp_post_change_trunk_vlan_stp_state(dname_length,
						 domain_name, vlan_id,
						 idb->hwptr->firstsw,
						 idb->port_state == 
						 PORT_FORWARDING ?
						 VLAN_SPT_STATE_FWD :
						 VLAN_SPT_STATE_NON_FWD);
	} else if (subif_sb) {
	    subif_sb->vtp_enabled = FALSE;
	    subif_sb->is_pruned = FALSE;
	}
	break;
    case VTP_TRUNK_PRUNING:
	vtp_post_set_vlan_eligible_on_trunk(idb, GETOBJ(int, 1), !csb->sense);
	break;
    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
    
}



/*
 * VTP show status.  
 */
void show_vtp_status (vtp_domain_info *info)
{
    unsigned long ix;

    /* print the domain name */
    printf("VTP management domain name - %s\r\n", info->domain_name);

    /* print the local mode */
    printf("    Local Mode - ");
    if (info->mode == VTP_CLIENT)
        printf("Client\r\n");
    else if (info->mode == VTP_SERVER) {
        printf("Server\r\n");
	printf("    TFTP file name: %s TFTP server: %i\r\n",
	       info->tftp_file_name, info->tftp_server);
    } else if (info->mode == VTP_TRANSPARENT)
	printf("Transparent\r\n");

    /* print the revision number */
    printf("    Configuration revision number - %d\r\n", info->revision);

    /* print the update id */
    printf("    Configuration update id - %i\r\n", info->updater);

    /* print the timestamp */
    printf("    Configuration timestamp - ");
    for (ix = 0; ix < 12; ix++)
        printf("%c", info->timestamp[ix]);
    printf("\r\n");
   
    /* print the advt interval */
    printf("    Advertisement time interval - %d seconds\r\n",
	   info->advt_interval); 

    /* print the MD5 checksum */
    printf("    Configuration MD5 digest - ");
    for (ix = 0; ix < 8; ix++)
        printf("0x%+02x ", info->md5_digest[ix]);
    printf("\r\n                               ");
    for (ix = 8; ix < 16; ix++)
        printf("0x%+02x ", info->md5_digest[ix]);

    /* print the VLAN count */
    printf("\r\n    VLAN count - %d\r\n", info->vlan_count);
    printf("    Pruning is %s\n", info->pruning_mode ? "enabled" : "disabled");
}


/*
 * VTP show statistics.
 */
void show_vtp_statistics (unsigned char dname_length, unsigned char
			  *domain_name) 
{
    unsigned char error;
    unsigned long rx_summary_advt;
    unsigned long rx_subset_advt;
    unsigned long rx_request_advt;
    unsigned long tx_summary_advt;
    unsigned long tx_subset_advt;
    unsigned long tx_request_advt;
    unsigned long config_rev_error;
    unsigned long config_digest_error;
    unsigned int ix;

    error = vtp_get_statistics(dname_length, domain_name, &rx_summary_advt,
			       &rx_subset_advt, &rx_request_advt,
			       &tx_summary_advt, &tx_subset_advt,
			       &tx_request_advt, &config_rev_error, 
			       &config_digest_error, VTP_STATS_RELATIVE);

    if (error == 0) {
        /* print the domain name */
        printf("VTP Management Domain Name - ");
        for (ix = 0; ix < dname_length; ix++)
            printf("%c", domain_name[ix]);
        printf("\r\n");

        printf(" Summary advertisement packets received - %d\r\n", 
                                                        rx_summary_advt);
        printf(" Subset advertisement packets received - %d\r\n", 
                                                        rx_subset_advt);
        printf(" Advertisement request advertisement packets received - %d\r\n", 
                                                        rx_request_advt);

        printf(" Summary advertisement packets transmitted - %d\r\n", 
                                                        tx_summary_advt);

        printf(" Subset advertisement packets transmitted - %d\r\n", 
                                                        tx_subset_advt);

        printf(" Advertisement request advertisement packets transmitted - %d\r\n",
                                                                  tx_request_advt);
        printf(" Configuration revision number error occurred - %d\r\n", 
                                                      config_rev_error);
        printf(" Configuration digest error occurred - %d\r\n", 
                                                   config_digest_error);
    }
}


/*
 * VTP show a trunk link.
 */
void show_vtp_trunk_link (vtp_trunk_info* t_info)
{
    unsigned long time_left;
    vtp_trunk_stat statistics;

    printf("Trunk link - %s\r\n",
	   ((idbtype*)t_info->trunk_id)->namestring);  
    printf("  domain %s\r\n", t_info->domain_name);
    printf("  time until next summary sent:");
    time_left = vtp_get_time_until_summary(t_info->trunk_id); 
    if (!time_left) {
	printf(" Summaries not being sent\r\n");
    } else {
	printf(" %d seconds\r\n", time_left / ONESEC);
    } 
    printf("  native vlan %d\r\n", t_info->native_vlan); 
    printf("\r\n");
    if (vtp_core_get_trunk_statistics(t_info->dname_length,
				      t_info->domain_name,
				      t_info->trunk_id, 
				      &statistics)) {
	printf("  %d joins transmitted\n", statistics.tx_join);
	printf("  %d joins received\n", statistics.rx_join);
	printf("  %d summaries from non-pruning capable devices\n",
	       statistics.rx_no_pruning_support_msg);
    }
}



/*
 * VTP show a vlan.
 */
void show_vtp_vlan (vtp_vlan_info *info)
{

   unsigned long ix;


   /* vlan isl */
   printf("  VLAN ISL Id -    %d\r\n", info->isl_vlan_id);

   /* vlan name */
   printf("    ");
   for (ix = 0; ix < info->vlan_name_length; ix++)
       printf("%c", info->vlan_name[ix]);
   printf("\r\n");

   /* vlan said */
   printf("    VLAN 802.10 Id - %d\r\n", info->said);

   /* vlan state */
   switch (info->state) {

      case VLAN_OPERATIONAL_STATE:             
         printf("    operational\r\n");
         break;

      case VLAN_SUSPENDED_STATE:              
         printf("    suspended\r\n");
         break;

      case VLAN_MTU_TOO_BIG_FOR_DEVICE_STATE:  
         printf("    MTU too big for device\r\n");
         break;

      case VLAN_MTU_TOO_BIG_FOR_TRUNK_STATE:   
         printf("    MTU too big for trunk\r\n");
         break;

      default:
         break;

   }

   /* vlan type */
   switch (info->type) {

      case VLAN_ETH_TYPE:             
         printf("    ethernet\r\n");
         break;

      case VLAN_FDDI_TYPE:              
         printf("    fddi");
	 if (info->parent_valid)
	     printf(", SRB ring number %d, parent VLAN Id %d",
		    info->ring_number, info->parent_vlan);
	 printf("\r\n");
         break;

      case VLAN_TR_TYPE:  
         printf("    token ring, SRB ring number %d, parent VLAN Id %d\r\n",
                                      info->ring_number, info->parent_vlan);
         break;

      case VLAN_FDDI_NET_TYPE:   
         printf("    fddi-net, SRB bridge number %d, spanning tree type %s\r\n", 
                    info->bridge_number, 
                    info->stp_type == VLAN_STP_IBM ? "IBM spanning tree" : 
                                        "IBM Spanning Tree");
         break;

      case VLAN_TR_NET_TYPE:   
         printf("    tr-net, SRB bridge number %d, spanning tree type %s\r\n", 
                    info->bridge_number, 
                    info->stp_type == VLAN_STP_IBM ? "IBM spanning tree" : 
                                        "IBM spanning tree");
         break;

      default:
         break;

    }

    /* vlan mtu */
    printf("    MTU %d\r\n", info->mtu);

    /* tlb 1 */
    if (info->tb_vlan_1) 
       printf("    Translational bridged VLAN Id  %d\r\n", info->tb_vlan_1);

    /* tlb 2 */
    if (info->tb_vlan_2)
       printf("    Translational bridged VLAN Id  %d\r\n", info->tb_vlan_2);
}


void show_vtp_pruning_status (vtp_domain_info* domain, 
			      vtp_trunk_info *t_info, 
			      unsigned char* vlan_active_mask,
			      unsigned char* vlan_advert_mask) {
    unsigned short start_vlan, stop_vlan;
    unsigned short vlan_num;
    boolean at_least_one_joined;
    
    start_vlan = 0;
    stop_vlan = 0;
    at_least_one_joined = FALSE;
    
    printf("\nVTP domain: %s VTP trunk: %s\n", domain->domain_name,
	   t_info->trunk_name);
    for (vlan_num = 1; vlan_num <= VTP_MAXIMUM_VLAN_ISL; vlan_num++) {
	if (vlan_advert_mask[vlan_num / 8] & (0x80 >> (vlan_num % 8)))  {
	    if (!start_vlan) {
		start_vlan = vlan_num;
	    }
	} else if (start_vlan && !stop_vlan) {
	    stop_vlan = vlan_num -1;
	    if (start_vlan == stop_vlan) {
		printf("  VLAN %d advertised on trunk\n", start_vlan);
	    } else {
		printf("  VLANs %d - %d advertised on trunk\n", start_vlan,
		       stop_vlan); 
	    }
	    at_least_one_joined = TRUE;
	    start_vlan = 0;
	    stop_vlan = 0;
	}
    }

    if (start_vlan) {
	stop_vlan = vlan_num - 1;
	if (start_vlan == stop_vlan) {
	    printf("  VLAN %d advertised on trunk\n", start_vlan);
	} else {
	    printf("  VLANs %d - %d advertised on trunk\n", start_vlan,
		   stop_vlan); 
	}
	at_least_one_joined = TRUE;
	start_vlan = 0;
	stop_vlan = 0;
    }

    if (!at_least_one_joined) {
	printf("  No VLANs advertised on this trunk\n");
    }

    at_least_one_joined = FALSE;
    start_vlan = 0;
    for (vlan_num = 1; vlan_num <= VTP_MAXIMUM_VLAN_ISL; vlan_num++) {
	if (vlan_active_mask[vlan_num / 8] & (0x80 >> (vlan_num % 8)))  {
	    if (!start_vlan) {
		start_vlan = vlan_num;
	    }
	} else if (start_vlan && !stop_vlan) {
	    stop_vlan = vlan_num - 1;
	    if (start_vlan == stop_vlan) {
		printf("  VLAN %d joined\n", start_vlan);
	    } else {
		printf("  VLANs %d - %d joined\n", start_vlan,
		       stop_vlan); 
	    }
	    at_least_one_joined = TRUE;
	    start_vlan = 0;
	    stop_vlan = 0;
	}
    }
    if (start_vlan) {
	stop_vlan = vlan_num - 1;
	if (start_vlan == stop_vlan) {
	    printf("  VLAN %d joined\n", start_vlan);
	} else {
	    printf("  VLANs %d - %d joined\n", start_vlan,
		   stop_vlan); 
	}
	at_least_one_joined = TRUE;
	start_vlan = 0;
	stop_vlan = 0;
    }
    if (!at_least_one_joined) {
	printf("  No VLANs joined on this trunk\n");
    }
}
/*
 * VTP show commands.
 *
 *   show vtp status [domain <domain-name>]
 *   show vtp statistics [domain <domain-name>]
 *   show vtp trunk [domain <domain-name>]
 *   show vtp database [vlan <vlan-id>] [domain <domain-name>]
 */
void show_vtp (parseinfo *csb)
{
    unsigned char dname_length;
    unsigned char *domain_name; 
    vtp_domain_info domain_info;
    vtp_vlan_info vlan_info;
    vtp_trunk_info t_info;
    unsigned char vlan_active_mask[VLANS_BITMASK_SIZE];
    unsigned char vlan_advert_mask[VLANS_BITMASK_SIZE];
    void *idb;
    unsigned char error;
    unsigned char error_2;
    unsigned long ix;
    boolean any_trunks;

    idb = NULL;
    if (!vtp_running) {
	printf("%% VTP is not enabled");
	automore_disable();
	return;
    }

    switch (csb->which) {

        case SHOW_VTP_STATUS:
            /* 
             * OBJ(int,1) = TRUE if domain presents
             * OBJ(string,1) = domain name
             */
	    automore_enable(NULL);
            if (GETOBJ(int, 1)) {
                domain_name = GETOBJ(string,1);
                dname_length = strlen(domain_name);
                domain_info.domain_name_length = dname_length;
                strcpy(domain_info.domain_name, domain_name);
                if (vtp_get_domain_info(&domain_info, VTP_EXACT))
                    printf("Unknown VTP domain.\r\n");
		else
                    show_vtp_status(&domain_info);
            }
            else {
                error = vtp_get_domain_info(&domain_info, VTP_FIRST);
                if (error == 0) {
                    do {
                        show_vtp_status(&domain_info);
                        error = vtp_get_domain_info(&domain_info, VTP_NEXT);
                    } while (error == 0);
                }
                else
                    printf("No VTP domain exists\r\n");
            }
	    automore_disable();
	    break;

        case SHOW_VTP_TRUNK:
            /* 
             * OBJ(int,1) = TRUE if domain presents
             * OBJ(string,1) = domain name
             */
	    automore_enable(NULL);
	    domain_name = GETOBJ(string,1);
	    error = vtp_get_trunk_info(&t_info, VTP_FIRST); 
	    if (error == 0) 
		do {
		    if (!(GETOBJ(int, 1)) ||
			(strcmp(t_info.domain_name, domain_name) ==
			 0))
			show_vtp_trunk_link(&t_info);
		    error = vtp_get_trunk_info(&t_info, VTP_NEXT);
		} while (error == 0);
	    automore_disable();
	    break;
	    
        case SHOW_VTP_STATISTICS:
            /* 
             * OBJ(int,1) = TRUE if domain presents
             * OBJ(string,1) = domain name
             */
	    automore_enable(NULL);
            if (GETOBJ(int, 1)) {
                domain_name = GETOBJ(string,1);
                dname_length = strlen(domain_name);
                show_vtp_statistics(dname_length, domain_name);
            }
            else {
                error = vtp_get_domain_info(&domain_info, VTP_FIRST);
                if (error == 0) {
                    do {
                        show_vtp_statistics(domain_info.domain_name_length, 
					    domain_info.domain_name); 
                        error = vtp_get_domain_info(&domain_info, VTP_NEXT);
                    } while (error == 0);
                }
                else
                    printf("No VTP Domain Exists\r\n");
            }
	    automore_disable();
	    break;
	    
        case SHOW_VTP_DATABASE:   
            /* 
             * OBJ(int,1) = TRUE if vlan presents
             * OBJ(int,2) = VLAN Id                   
             * OBJ(int,3) = TRUE if domain presents
             * OBJ(string,1) = domain name
             */
	    automore_enable(NULL);
            if (GETOBJ(int,1)) {
                vlan_info.isl_vlan_id = GETOBJ(int,2);
                if (GETOBJ(int,3)) {
                    domain_name = GETOBJ(string,1);
                    dname_length = strlen(domain_name);
                    printf("VTP Management Domain Name - ");
                    for (ix = 0; ix < dname_length; ix++)
                        printf("%c", domain_name[ix]);
                    printf("\r\n");
                    error = vtp_get_vlan_info(dname_length, domain_name,
					      &vlan_info, VTP_EXACT); 
                    if (error == 0) 
                        show_vtp_vlan(&vlan_info);
                }
                else {
                    error = vtp_get_domain_info(&domain_info, VTP_EXACT);
next_vlan_1:
                    if (error == 0) {
                        printf("VTP Management Domain Name - ");
                        for (ix = 0; ix <
			     domain_info.domain_name_length; ix++) 
                            printf("%c", domain_info.domain_name[ix]);
                        printf("\r\n");
                        error_2 =
			    vtp_get_vlan_info(domain_info.domain_name_length,
						    domain_info.domain_name,
						    &vlan_info, VTP_EXACT); 
                        if (error_2 == 0)  
                            show_vtp_vlan(&vlan_info);
                        error = vtp_get_domain_info(&domain_info, VTP_NEXT);
                        goto next_vlan_1;
                    }
                }
            } else  {
                if (GETOBJ(int,3)) {
                    domain_name = GETOBJ(string,1);
                    dname_length = strlen(domain_name);
                    printf("VTP Management Domain Name - ");
                    for (ix = 0; ix < dname_length; ix++)
                        printf("%c", domain_name[ix]);
                    printf("\r\n");
                    error = vtp_get_vlan_info(dname_length, domain_name,
					    &vlan_info, VTP_FIRST); 
                    if (error == 0) {
                        do {
                            show_vtp_vlan(&vlan_info);
                            error = vtp_get_vlan_info(dname_length,
						      domain_name,
						      &vlan_info,
						      VTP_NEXT); 
                        } while (error == 0);
                    }
                }
                else {
                    error = vtp_get_domain_info(&domain_info, VTP_FIRST);
next_vlan_2:
                    if (error == 0) {
                        printf("VTP Management Domain Name - %s\r\n",
			       domain_info.domain_name); 
                        error_2 =
			    vtp_get_vlan_info(domain_info.domain_name_length, 
					      domain_info.domain_name,
					      &vlan_info, VTP_FIRST);
                        if (error_2 == 0) { 
                            do {
                                show_vtp_vlan(&vlan_info);
                                error_2 =
				    vtp_get_vlan_info(domain_info.domain_name_length,
						      domain_info.domain_name,
						      &vlan_info,
						      VTP_NEXT); 
                            } while (error_2 == 0);
                        }
                        error = vtp_get_domain_info(&domain_info,
						    VTP_NEXT);  
                        goto next_vlan_2;
                    }
                }
            }
	    automore_disable();
	    break;

	case SHOW_VTP_PRUNING:
	    if (vtp_get_domain_info(&domain_info, VTP_FIRST)) {
		printf("No VTP domains defined\n");
		return;
	    }
	    any_trunks = FALSE;
	    do {
		if (domain_info.pruning_mode == VTP_PRUNING_DISABLE) {
		    printf("VTP domain %s: pruning disabled\n\n",
			   domain_info.domain_name);
		    continue;
		}
		if (vtp_get_trunk_by_domain(domain_info.domain_name_length, 
					    domain_info.domain_name,
					    &t_info, VTP_FIRST)) 
		    continue;
		any_trunks = TRUE;
		do {
		    if ((vtp_core_get_trunk_pruning_status(domain_info.domain_name_length,
							   domain_info.domain_name,
							   t_info.trunk_id,
							   vlan_active_mask)) && 
			(vtp_core_get_trunk_joining_status(domain_info.domain_name_length, 
							   domain_info.domain_name, 
							   t_info.trunk_id, vlan_advert_mask)))
			{
			    show_vtp_pruning_status(&domain_info, &t_info,
						    vlan_active_mask, vlan_advert_mask);
			}
		} while (vtp_get_trunk_by_domain(domain_info.domain_name_length,
						 domain_info.domain_name,
						 &t_info, VTP_NEXT) ==
			 VTP_OK);
	    } while (vtp_get_domain_info(&domain_info, VTP_NEXT) ==
		     VTP_OK);
	    if (!any_trunks) {
		printf("No VTP trunks defined\n");
	    }
	    break;
	case SHOW_VTP_PRUNING_VLAN:
	    memcpy(domain_info.domain_name, GETOBJ(string,1),
		   strlen(GETOBJ(string,1)));
	    domain_info.domain_name_length = strlen(GETOBJ(string,1));
	    if (vtp_get_domain_info(&domain_info, VTP_EXACT)) {
		printf("VTP domain does not exist\n");
		return;
	    }
	    if (vtp_get_trunk_by_domain(domain_info.domain_name_length,
					domain_info.domain_name,
					&t_info, VTP_FIRST)) {
		printf("No VTP trunk defined in domain\n");
		return;
	    }
	    do {
		unsigned short span_tree_state;
		unsigned short local_assoc;

		span_tree_state = 
		    vtp_core_get_trunk_vlan_spt_state(domain_info.domain_name_length, 
						      domain_info.domain_name, 
						      GETOBJ(int, 1),
						      t_info.trunk_id);
		if (span_tree_state != VLAN_SPT_STATE_NA) {
		    printf("Trunk %s, VLAN %d, Spanning Tree is %s,",
			   t_info.trunk_name, GETOBJ(int,1), 
			   span_tree_state == VLAN_SPT_STATE_FWD ?
			   "fowarding" : "not fowarding"); 
		}

		local_assoc =
		    vtp_core_get_vlan_local_assoc(domain_info.domain_name_length, 
						  domain_info.domain_name, 
						  GETOBJ(int,1));
		if (local_assoc != VLAN_LOCAL_ASSOC_NA) {
		    printf(" %s\n", local_assoc == VLAN_LOCAL_ASSOC ?
			   "local ports are assigned" : 
			   "no local ports assigned");
		}
	    } while (vtp_get_trunk_by_domain(domain_info.domain_name_length,
					     domain_info.domain_name,
					     &t_info, VTP_NEXT) == VTP_OK);
	    break;
	}
}

void vtp_cli_load_database (parseinfo* csb) {
    vtp_domain_info d_info;

    d_info.domain_name_length = strlen(GETOBJ(string, 1));
    strcpy(d_info.domain_name, GETOBJ(string, 1));
    if (vtp_get_domain_info(&d_info, VTP_EXACT)) {
	printf("VTP: VTP domain does not exist\r\n");
	return;
    }

    if (d_info.mode != VTP_SERVER) {
	printf("VTP: VTP process not a server\r\n");
	return;
    }

    vtp_load_database(&d_info);
}
