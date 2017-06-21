/* $Id: sr_vtpmib.c,v 1.2.58.7 1996/07/02 02:51:07 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/sr_vtpmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * November 1995, Christopher Young (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_vtpmib.c,v $
 * Revision 1.2.58.7  1996/07/02  02:51:07  cyoung
 * load the VTP OID table into the hash list so MakeOIDFragFromDot
 * doesn't fail.
 * CSCdi61363:  vtp notifications (traps) are not sent to management
 * station.
 * Branch: California_branch
 *
 * Revision 1.2.58.6  1996/07/01  18:46:44  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.2.58.5  1996/06/29  02:17:27  cyoung
 * Initialize all VLAN variables to valid values to avoid consistency
 * check failures.
 * CSCdi61716:  applying edit buffer fails when buffer appears valid
 * Branch: California_branch
 *
 * Revision 1.2.58.4  1996/06/24  22:07:02  cyoung
 * Be a little more prudent on when to configure a domain change
 * CSCdi61064:  managementDomainConfigRevNumber changed to unknown value
 * Branch: California_branch
 *
 * Revision 1.2.58.3  1996/06/22  23:46:33  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 1.2.58.2  1996/05/21  22:29:15  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.1  1996/05/09  14:48:21  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:23  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.2  1996/04/11  20:50:37  cyoung
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
 * Revision 1.2.74.1  1996/04/08  02:42:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.6  1996/04/06  03:53:37  cyoung
 * Add trunk creation/deletion capability to VTP MIB code.
 * Beautify some code formatting.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.5  1996/04/06  02:47:17  cyoung
 * IP address switch between ulongs and OctetStrings (duh)
 * CSCdi53871:  getmany not responding in the middle of its execution
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.4  1996/04/03  22:36:53  cyoung
 * New objects for TFTP support, ETR vlan type and trunk table creation
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MIB - TFTP support, ETR type allowed, still need work on Trunk creation
 * dep - use temp buf to determine add/mod mode instead of real database
 * core - add tooBig hook for trap generation
 *
 * Revision 1.2.64.3  1996/03/27  23:41:18  cyoung
 * MIB - implement new trap formats (correct varbinds)
 * core - vtp_delete_domain crashes with trunk defined
 * depend - TFTP read needs its own process, implement VLAN state
 *          modifications, some cleanup
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.2  1996/03/25  02:21:40  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:44:52  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.4  1996/03/22  16:22:10  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.3  1996/03/08  18:18:55  cyoung
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
 * Revision 1.2.36.2  1996/03/05  01:54:47  cyoung
 * Add vtp configured interface command.  Provide function to retrieve
 * SNMP stats. Fix problem which tries to access NULL pointer when
 * adding a domain after all have been deleted.  Make vtp_upload_info
 * more useful.  Put in beginning TFTP support.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:17  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.11  1996/02/23  23:05:46  cyoung
 * Independant code put in for checking native vlan range.
 * Do not increment the domain index when adding the 'null domain'.
 * remove duplicate function declarations from vtp_interface.h
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.10  1996/02/22  03:09:58  cyoung
 * Move include files around to make code more easily ported to Cat5000.
 * TruthValue false is 2 and not 0.
 * Roll function declarations from vtp_agent into vtp_interface and
 * vtp_externs (cut down on number of include files).
 * Add IOS_SPECIFIC compile flag to mark non-portable code.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.9  1996/02/21  04:11:59  cyoung
 * bridge numbers start at 1. Fix spurious memory in the vtp_receive_xxx
 * functions.  VTP multicast address added to SMF.  Make sure the firstsw
 * is passed into the the vtp_receive_packet function instead of the
 * subinterface.  Set revision number to zero if modification is made
 * to vtp database when in other than server mode
 *
 * VTP now works over LANE.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.8  1996/02/13  04:03:02  cyoung
 * move notification_enabled to vtp_dep.c, minor bug fixes, add code to
 * check for native vlan range and existance of trunk before allowing sets.
 * Also some code shuffle as a result of resolving conflicts.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.7  1996/02/12  21:32:30  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * split vtp.c into vtp.c and vtp_util.c
 *
 * Revision 1.2.4.6  1996/02/09  01:09:56  percyk
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Add tlb vlan support.  Various misc bug fixes.
 *
 * Revision 1.2.4.5  1996/02/08  04:12:29  cyoung
 * Set default time interval when creating or modifying management domain.
 * Add support for setting vlan filter on trunks and the native vlan.
 * Add support for transmitting adverts over LANE.
 * Add code for removing trunks from idblist.
 * Add interface for core code to notify dependant code when a mod has
 *  been made to the trunk list.
 * More code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.4  1996/01/31  02:33:58  cyoung
 * Provide function to allow variance in managementDomainEntry row
 * creation.
 * Remove redundant consistency checking.  Add hooks to correctly report
 * result of 'apply' operation.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.3  1996/01/31  00:44:01  cyoung
 * MIB fixes and beginning code for sub-interface creation
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.2  1996/01/24  03:28:01  cyoung
 * vlanTrunkPortTable not being returned.  Fix get-next logic.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.1  1996/01/24  01:37:48  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2  1996/01/02  04:50:04  shankar
 * Branch: FR_SVC_branch
 * Sync to mainline
 *
 * Revision 1.1  1995/12/23  00:31:20  cyoung
 * placeholder for VTP MIB code
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */
#include "sr_vtpmib.h"
#include "sr_vtpmib-mib.h"

static unsigned long vtpTrunkSerialNo = 0;;
static unsigned long IDvtpNotifications[] = {1, 3, 6, 1, 4, 1, 9, 9,
						 46, 2 };
#define LNvtpNotifications 10

char vtpConfigRevNumberError_str[] = "vtpNotificationsPrefix.1";
char vtpConfigDigestError_str[] = "vtpNotificationsPrefix.2";
char vtpServerDisabled_str[] = "vtpNotificationsPrefix.3";
char vtpMtuTooBig_str[] = "vtpNotificationsPrefix.4";

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the vtpStatus family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
vtpStatus_get (OID *incoming, ObjectInfo *object, int searchType,
	       ContextInfo *contextInfo, int serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    vtpStatus_t     *data;

    data = NULL;
    instLength = incoming->length - object->oid.length;
    arg = -1;
    
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    
    switch (searchType) {
    case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1]
	    == 0) { 
	    arg = object->nominator;
        }
        break;
	
    case NEXT:
        if (instLength <= 0) {
	    arg = object->nominator;
        }
        break;
	
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_vtpStatus_get(serialNum,
						contextInfo, arg)) ==
	NULL) { 
        arg = -1;
    }
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {

#ifdef I_vtpVersion
    case I_vtpVersion:
	dp = (void *) (&data->vtpVersion);
	break;
#endif /* I_vtpVersion */
	
#ifdef I_vtpMaxVlanStorage
    case I_vtpMaxVlanStorage:
	dp = (void *) (&data->vtpMaxVlanStorage);
	break;
#endif /* I_vtpMaxVlanStorage */
	
#ifdef I_vtpNotificationsEnabled
    case I_vtpNotificationsEnabled:
	dp = (void *) (&data->vtpNotificationsEnabled);
	break;
#endif /* I_vtpNotificationsEnabled */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &ZeroOid, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the vtpStatus data object.
 *---------------------------------------------------------------------*/
void
vtpStatus_free (vtpStatus_t *data) {
    if (data != NULL) {
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after vtpStatus set/undo
 *---------------------------------------------------------------------*/
static int
vtpStatus_cleanup(doList_t* trash) {
    vtpStatus_free(trash->data);
#ifdef SR_SNMPv2
    vtpStatus_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
vtpStatus_test(OID *incoming, ObjectInfo *object, ObjectSyntax *value,
	       doList_t *doHead, doList_t *doCur, ContextInfo
	       *contextInfo) {
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;

    instLength = incoming->length - object->oid.length;
    error_status = NO_ERROR;

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] !=
	0) { 
        return (NO_CREATION_ERROR); 
    }
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == vtpStatus_set) &&
            (((vtpStatus_t *) (dp->data)) != NULL) ) {
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
        if ((dp->data = malloc(sizeof(vtpStatus_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(vtpStatus_t));
#ifdef SR_SNMPv2
        if ((dp->undodata = malloc(sizeof(vtpStatus_t))) == NULL) {  
            DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->undodata, 0, sizeof(vtpStatus_t));
#endif /* SR_SNMPv2 */
	
        dp->setMethod = vtpStatus_set;
        dp->cleanupMethod = vtpStatus_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_vtpStatus_UNDO
        dp->undoMethod = vtpStatus_undo;
#else /* SR_vtpStatus_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_vtpStatus_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_vtpNotificationsEnabled
    case I_vtpNotificationsEnabled:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpStatus_t *) (dp->data))->vtpNotificationsEnabled =
	    value->sl_value; 
	break;
#endif /* I_vtpNotificationsEnabled */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator
in vtpStatus_test)\n")); 
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_vtpStatus_test */
    error_status = k_vtpStatus_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((vtpStatus_t *) (dp->data))->valid);
        error_status = k_vtpStatus_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vtpStatus_set (doList_t* doHead, doList_t* doCur, ContextInfo*
	       contextInfo) {
    return (k_vtpStatus_set((vtpStatus_t *) (doCur->data),
			    contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the managementDomainEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
managementDomainEntry_get (OID *incoming, ObjectInfo *object, int
			   searchType, ContextInfo *contextInfo, int
			   serialNum) { 
    int             instLength;
    int             arg;
    void            *dp;
    managementDomainEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            managementDomainIndex;

    data = NULL;
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    
    if (searchType == EXACT) {
        if (instLength != 1) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length,
		    &managementDomainIndex, searchType, &carry)) < 0 )
	{ 
	    arg = -1;
	}
    if (searchType == NEXT) {
        managementDomainIndex = MAX(1, managementDomainIndex);
    }

    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if (arg != -1) {
	data = k_managementDomainEntry_get(serialNum, contextInfo,
					   arg, searchType,
					   managementDomainIndex); 
	if (data == NULL)
	    arg = -1;
	else {
	    /*
	     * Build instance information
	     */
	    inst.oid_ptr = buffer;
	    inst.length = 1;
	    inst.oid_ptr[0] = data->managementDomainIndex;
	}
    }
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_managementDomainName
    case I_managementDomainName:
	dp = (void *) (CloneOctetString(data->managementDomainName));
	break;
#endif /* I_managementDomainName */
	
#ifdef I_managementDomainLocalMode
    case I_managementDomainLocalMode:
	dp = (void *) (&data->managementDomainLocalMode);
	break;
#endif /* I_managementDomainLocalMode */
	
#ifdef I_managementDomainConfigRevNumber
    case I_managementDomainConfigRevNumber:
	dp = (void *) (&data->managementDomainConfigRevNumber);
	break;
#endif /* I_managementDomainConfigRevNumber */
	
#ifdef I_managementDomainLastUpdater
    case I_managementDomainLastUpdater:
	dp = (void *) (IPToOctetString(data->managementDomainLastUpdater));
	break;
#endif /* I_managementDomainLastUpdater */
	
#ifdef I_managementDomainLastChange
    case I_managementDomainLastChange:
	dp = (void *) (CloneOctetString(data->managementDomainLastChange));
	break;
#endif /* I_managementDomainLastChange */

#ifdef I_managementDomainRowStatus
    case I_managementDomainRowStatus:
        dp = (void *) (&data->managementDomainRowStatus);
        break;
#endif /* I_managementDomainRowStatus */

#ifdef I_managementDomainTftpServer
    case I_managementDomainTftpServer:
	dp = (void *)
	    (IPToOctetString(data->managementDomainTftpServer)); 
	break;
#endif /* I_managementDomainTftpServer */

#ifdef I_managementDomainTftpPathname
    case I_managementDomainTftpPathname:
	dp = (void *)
	    (CloneOctetString(data->managementDomainTftpPathname));
	break;
#endif I_managementDomainTftpPathname

#ifdef I_managementDomainPruningState
    case I_managementDomainPruningState:
       dp = (void *) (&data->managementDomainPruningState);
       break;
#endif /* I_managementDomainPruningState */

#ifdef I_vtpVlanEditOperation
    case I_vtpVlanEditOperation:
	dp = (void *) (&data->vtpVlanEditOperation);
	break;
#endif /* I_vtpVlanEditOperation */
	
#ifdef I_vtpVlanApplyStatus
    case I_vtpVlanApplyStatus:
	dp = (void *) (&data->vtpVlanApplyStatus);
	break;
#endif /* I_vtpVlanApplyStatus */
	
#ifdef I_vtpVlanEditBufferOwner
    case I_vtpVlanEditBufferOwner:
	dp = (void *) (CloneOctetString(data->vtpVlanEditBufferOwner));
	break;
#endif /* I_vtpVlanEditBufferOwner */
	
#ifdef I_vtpVlanEditConfigRevNumber
    case I_vtpVlanEditConfigRevNumber:
	dp = (void *) (&data->vtpVlanEditConfigRevNumber);
	break;
#endif /* I_vtpVlanEditConfigRevNumber */	

#ifdef I_vtpInSummaryAdverts
    case I_vtpInSummaryAdverts:
	dp = (void *) (&data->vtpInSummaryAdverts);
	break;
#endif /* I_vtpInSummaryAdverts */
	
#ifdef I_vtpInSubsetAdverts
    case I_vtpInSubsetAdverts:
	dp = (void *) (&data->vtpInSubsetAdverts);
	break;
#endif /* I_vtpInSubsetAdverts */
	
#ifdef I_vtpInAdvertRequests
    case I_vtpInAdvertRequests:
	dp = (void *) (&data->vtpInAdvertRequests);
	break;
#endif /* I_vtpInAdvertRequests */
	
#ifdef I_vtpOutSummaryAdverts
    case I_vtpOutSummaryAdverts:
	dp = (void *) (&data->vtpOutSummaryAdverts);
	break;
#endif /* I_vtpOutSummaryAdverts */
	
#ifdef I_vtpOutSubsetAdverts
    case I_vtpOutSubsetAdverts:
	dp = (void *) (&data->vtpOutSubsetAdverts);
	break;
#endif /* I_vtpOutSubsetAdverts */
	
#ifdef I_vtpOutAdvertRequests
    case I_vtpOutAdvertRequests:
	dp = (void *) (&data->vtpOutAdvertRequests);
	break;
#endif /* I_vtpOutAdvertRequests */
	
#ifdef I_vtpConfigRevNumberErrors
    case I_vtpConfigRevNumberErrors:
	dp = (void *) (&data->vtpConfigRevNumberErrors);
	break;
#endif /* I_vtpConfigRevNumberErrors */
	
#ifdef I_vtpConfigDigestErrors
    case I_vtpConfigDigestErrors:
	dp = (void *) (&data->vtpConfigDigestErrors);
	break;
#endif /* I_vtpConfigDigestErrors */

    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the managementDomainEntry data object.
 *---------------------------------------------------------------------*/
void
managementDomainEntry_free (managementDomainEntry_t *data) {
    if (data != NULL) {
	FreeOctetString(data->managementDomainName);
	FreeOctetString(data->managementDomainLastChange);
	FreeOctetString(data->managementDomainTftpPathname);
	FreeOctetString(data->vtpVlanEditBufferOwner);	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after managementDomainEntry set/undo
 *---------------------------------------------------------------------*/
static int
managementDomainEntry_cleanup (doList_t *trash) {
    managementDomainEntry_free(trash->data);
#ifdef SR_SNMPv2
    managementDomainEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the managementDomainEntry family
 *---------------------------------------------------------------------*/
managementDomainEntry_t *
Clone_managementDomainEntry (managementDomainEntry_t
			     *managementDomainEntry) {
    managementDomainEntry_t *data;
    
    if ((data = malloc(sizeof(managementDomainEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (managementDomainEntry),
	   sizeof(managementDomainEntry_t)); 
    
    data->managementDomainName =
	CloneOctetString(managementDomainEntry->managementDomainName);
    
    data->managementDomainLastChange =
	CloneOctetString(managementDomainEntry->managementDomainLastChange);

    data->managementDomainTftpPathname =
	CloneOctetString(managementDomainEntry->managementDomainTftpPathname);

    data->vtpVlanEditBufferOwner =
	CloneOctetString(managementDomainEntry->vtpVlanEditBufferOwner);
    
    return data;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
managementDomainEntry_test (OID *incoming, ObjectInfo *object,
			    ObjectSyntax *value, doList_t *doHead,
			    doList_t *doCur, ContextInfo *contextInfo)
{
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    managementDomainEntry_t     *managementDomainEntry;
    long           managementDomainIndex;

    instLength = incoming->length - object->oid.length;
    error_status = NO_ERROR;
    carry = 0;
    
    /*
     * Validate the object instance
     *
     */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length,
		    &managementDomainIndex, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    managementDomainEntry = k_managementDomainEntry_get(-1,
							contextInfo,
							-1, EXACT,
							managementDomainIndex);
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == managementDomainEntry_set) &&
            (((managementDomainEntry_t *) (dp->data)) != NULL) &&
            (((managementDomainEntry_t *)
	      (dp->data))->managementDomainIndex ==
	     managementDomainIndex) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = managementDomainEntry_set;
        dp->cleanupMethod = managementDomainEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_managementDomainEntry_UNDO
        dp->undoMethod = managementDomainEntry_undo;
#else /* SR_managementDomainEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_managementDomainEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (managementDomainEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_managementDomainEntry(managementDomainEntry))
		== NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_managementDomainEntry(managementDomainEntry))
		== NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ((dp->data = malloc(sizeof(managementDomainEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(managementDomainEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((managementDomainEntry_t *)
		 (dp->data))->managementDomainIndex =
		     (managementDomainIndex); 
                SET_VALID(I_managementDomainIndex,
			  ((managementDomainEntry_t *)
			   (dp->data))->valid); 
		
                error_status = k_managementDomainEntry_set_defaults(dp);
            }
        }
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_managementDomainName
    case I_managementDomainName:
	
	if (((managementDomainEntry_t *)
	     (dp->data))->managementDomainName != NULL) { 
	    FreeOctetString(((managementDomainEntry_t *)
			     (dp->data))->managementDomainName); 
	}
	
	((managementDomainEntry_t *) (dp->data))->managementDomainName = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_managementDomainName */
	
#ifdef I_managementDomainLocalMode
    case I_managementDomainLocalMode:
	
	if ((value->sl_value < 1) || (value->sl_value > 3)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((managementDomainEntry_t *)
	 (dp->data))->managementDomainLocalMode = value->sl_value; 
	break;
#endif /* I_managementDomainLocalMode */

#ifdef I_managementDomainRowStatus
    case I_managementDomainRowStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_managementDomainRowStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
 
	if (managementDomainEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value ==
		D_managementDomainRowStatus_notInService) ||
	       (value->sl_value ==
		D_managementDomainRowStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value ==
		 D_managementDomainRowStatus_createAndGo) ||
		(value->sl_value ==
		 D_managementDomainRowStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
 
	((managementDomainEntry_t *)
	 (dp->data))->managementDomainRowStatus = value->sl_value; 
	break;
#endif /* I_managementDomainRowStatus */ 

#ifdef I_managementDomainTftpServer
    case I_managementDomainTftpServer:

	if (value->os_value->length != 4) {
	    return WRONG_LENGTH_ERROR;
	}

	((managementDomainEntry_t *)
	 (dp->data))->managementDomainTftpServer =
	     OctetStringToIP(value->os_value); 
	break;
#endif /* I_managementDomainTftpServer */

#ifdef I_managementDomainTftpPathname
    case I_managementDomainTftpPathname:
	if (((managementDomainEntry_t *)
	     (dp->data))->managementDomainTftpPathname != NULL) { 
	    FreeOctetString(((managementDomainEntry_t *)
			     (dp->data))->managementDomainTftpPathname); 
	}
	
	((managementDomainEntry_t *)
	 (dp->data))->managementDomainTftpPathname =
	     CloneOctetString(value->os_value); 
	
	break;
#endif /* I_managementDomainTftpPathname */

#ifdef I_managementDomainPruningState
    case I_managementDomainPruningState:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((managementDomainEntry_t *)
	 (dp->data))->managementDomainPruningState = value->sl_value; 
	break;
#endif /* I_managementDomainPruningState */
	
#ifdef I_vtpVlanEditOperation
    case I_vtpVlanEditOperation:
	
	if ((value->sl_value < 1) || (value->sl_value > 5)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((managementDomainEntry_t *) (dp->data))->vtpVlanEditOperation =
	    value->sl_value; 
	break;
#endif /* I_vtpVlanEditOperation */
	
#ifdef I_vtpVlanEditBufferOwner
    case I_vtpVlanEditBufferOwner:
	
	if (((vtpEditControlEntry_t *)
	     (dp->data))->vtpVlanEditBufferOwner != NULL) { 
	    FreeOctetString(((vtpEditControlEntry_t *)
			     (dp->data))->vtpVlanEditBufferOwner); 
	}
	
	((managementDomainEntry_t *) (dp->data))->vtpVlanEditBufferOwner = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_vtpVlanEditBufferOwner */
	
#ifdef I_vtpVlanEditConfigRevNumber
    case I_vtpVlanEditConfigRevNumber:
	
	((managementDomainEntry_t *)
	 (dp->data))->vtpVlanEditConfigRevNumber = value->ul_value; 
	break;
#endif /* I_vtpVlanEditConfigRevNumber */	

    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator
in managementDomainEntry_test)\n")); 
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_managementDomainEntry_test */
    error_status = k_managementDomainEntry_test(object, value, dp,
						contextInfo); 
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((managementDomainEntry_t *)
				      (dp->data))->valid); 
        error_status = k_managementDomainEntry_ready(object, value,
						     doHead, dp); 
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
managementDomainEntry_set (doList_t *doHead, doList_t *doCur,
			   ContextInfo *contextInfo) {
  return (k_managementDomainEntry_set((managementDomainEntry_t *)
				      (doCur->data), 
				      contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the vtpVlanEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
vtpVlanEntry_get ( OID *incoming, ObjectInfo *object, int searchType,
		  ContextInfo *contextInfo, int serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    vtpVlanEntry_t  *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            managementDomainIndex;
    long            vtpVlanIndex;

    data = NULL;
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;

    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 2 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 2) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length, &vtpVlanIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        vtpVlanIndex = MAX(0, vtpVlanIndex);
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length,
		    &managementDomainIndex, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        managementDomainIndex = MAX(1, managementDomainIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_vtpVlanEntry_get(serialNum,
						   contextInfo, arg
						   ,searchType,
						   managementDomainIndex, vtpVlanIndex)) == NULL) {  
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->managementDomainIndex;
        inst.oid_ptr[1] = data->vtpVlanIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_vtpVlanState
    case I_vtpVlanState:
	dp = (void *) (&data->vtpVlanState);
	break;
#endif /* I_vtpVlanState */
	
#ifdef I_vtpVlanType
    case I_vtpVlanType:
	dp = (void *) (&data->vtpVlanType);
	break;
#endif /* I_vtpVlanType */
	
#ifdef I_vtpVlanName
    case I_vtpVlanName:
	dp = (void *) (CloneOctetString(data->vtpVlanName));
	break;
#endif /* I_vtpVlanName */
	
#ifdef I_vtpVlanMtu
    case I_vtpVlanMtu:
	dp = (void *) (&data->vtpVlanMtu);
	break;
#endif /* I_vtpVlanMtu */
	
#ifdef I_vtpVlanDot10Said
    case I_vtpVlanDot10Said:
	dp = (void *) (CloneOctetString(data->vtpVlanDot10Said));
	break;
#endif /* I_vtpVlanDot10Said */
	
#ifdef I_vtpVlanRingNumber
    case I_vtpVlanRingNumber:
        dp = (void *) (&data->vtpVlanRingNumber);
        break;
#endif /* I_vtpVlanRingNumber */
	
#ifdef I_vtpVlanBridgeNumber
    case I_vtpVlanBridgeNumber:
        dp = (void *) (&data->vtpVlanBridgeNumber);
        break;
#endif /* I_vtpVlanBridgeNumber */
	
	
#ifdef I_vtpVlanParentVlan
    case I_vtpVlanParentVlan:
        dp = (void *) (&data->vtpVlanParentVlan);
        break;
#endif /* I_vtpVlanParentVlan */

#ifdef I_vtpVlanTranslationalVlan1
    case I_vtpVlanTranslationalVlan1:
	dp = (void *) (&data->vtpVlanTranslationalVlan1);
	break;
#endif

#ifdef I_vtpVlanTranslationalVlan2
    case I_vtpVlanTranslationalVlan2:
	dp = (void *) (&data->vtpVlanTranslationalVlan2);
	break;
#endif
	
#ifdef I_vtpVlanBridgeType
    case I_vtpVlanBridgeType:
	dp = (void *) (&data->vtpVlanBridgeType);
	break;
#endif /* I_vtpVlanBridgeType */

    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the vtpVlanEditEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
vtpVlanEditEntry_get (OID *incoming, ObjectInfo *object, int
		      searchType, ContextInfo *contextInfo, int
		      serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    vtpVlanEditEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            managementDomainIndex;
    long            vtpVlanEditIndex;

    data = NULL;
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 2 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    
    if (searchType == EXACT) {
        if (instLength != 2) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length,
		    &vtpVlanEditIndex, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        vtpVlanEditIndex = MAX(0, vtpVlanEditIndex);
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length,
		    &managementDomainIndex, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        managementDomainIndex = MAX(1, managementDomainIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_vtpVlanEditEntry_get(serialNum,
						       contextInfo,
						       arg
						       ,searchType,
						       managementDomainIndex, vtpVlanEditIndex)) == NULL) {
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->managementDomainIndex;
        inst.oid_ptr[1] = data->vtpVlanEditIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_vtpVlanEditState
    case I_vtpVlanEditState:
	dp = (void *) (&data->vtpVlanEditState);
	break;
#endif /* I_vtpVlanEditState */
	
#ifdef I_vtpVlanEditType
    case I_vtpVlanEditType:
	dp = (void *) (&data->vtpVlanEditType);
	break;
#endif /* I_vtpVlanEditType */
	
#ifdef I_vtpVlanEditName
    case I_vtpVlanEditName:
	dp = (void *) (CloneOctetString(data->vtpVlanEditName));
	break;
#endif /* I_vtpVlanEditName */
	
#ifdef I_vtpVlanEditMtu
    case I_vtpVlanEditMtu:
	dp = (void *) (&data->vtpVlanEditMtu);
	break;
#endif /* I_vtpVlanEditMtu */
	
#ifdef I_vtpVlanEditDot10Said
    case I_vtpVlanEditDot10Said:
	dp = (void *) (CloneOctetString(data->vtpVlanEditDot10Said));
	break;
#endif /* I_vtpVlanEditDot10Said */

#ifdef I_vtpVlanEditRingNumber
    case I_vtpVlanEditRingNumber:
        dp = (void *) (&data->vtpVlanEditRingNumber);
        break;
#endif /* I_vtpVlanEditRingNumber */
	
#ifdef I_vtpVlanEditBridgeNumber
    case I_vtpVlanEditBridgeNumber:
        dp = (void *) (&data->vtpVlanEditBridgeNumber);
        break;
#endif /* I_vtpVlanEditBridgeNumber */
	
#ifdef I_vtpVlanEditParentVlan
    case I_vtpVlanEditParentVlan:
        dp = (void *) (&data->vtpVlanEditParentVlan);
        break;
#endif /* I_vtpVlanEditParentVlan */

#ifdef I_vtpVlanEditRowStatus
    case I_vtpVlanEditRowStatus:
	dp = (void *) (&data->vtpVlanEditRowStatus);
	break;
#endif /* I_vtpVlanEditRowStatus */

#ifdef I_vtpVlanEditTranslationalVlan1
    case I_vtpVlanEditTranslationalVlan1:
	dp = (void *) (&data->vtpVlanEditTranslationalVlan1);
	break;
#endif

#ifdef I_vtpVlanEditTranslationalVlan2
    case I_vtpVlanEditTranslationalVlan2:
	dp = (void *) (&data->vtpVlanEditTranslationalVlan2);
	break;
#endif	

#ifdef I_vtpVlanEditBridgeType
    case I_vtpVlanEditBridgeType:
	dp = (void *) (&data->vtpVlanEditBridgeType);
	break;
#endif /* I_vtpVlanEditBridgeType */

    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the vtpVlanEditEntry data object.
 *---------------------------------------------------------------------*/
void
vtpVlanEditEntry_free (vtpVlanEditEntry_t *data) {
    if (data != NULL) {
	FreeOctetString(data->vtpVlanEditName);
	FreeOctetString(data->vtpVlanEditDot10Said);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after vtpVlanEditEntry set/undo
 *---------------------------------------------------------------------*/
static int
vtpVlanEditEntry_cleanup (doList_t *trash) {
    vtpVlanEditEntry_free(trash->data);
#ifdef SR_SNMPv2
    vtpVlanEditEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the vtpVlanEditEntry family
 *---------------------------------------------------------------------*/
vtpVlanEditEntry_t *
Clone_vtpVlanEditEntry (vtpVlanEditEntry_t *vtpVlanEditEntry) {
    vtpVlanEditEntry_t *data;
    
    if ((data = malloc(sizeof(vtpVlanEditEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (vtpVlanEditEntry),
	   sizeof(vtpVlanEditEntry_t)); 
    
    data->vtpVlanEditName =
	CloneOctetString(vtpVlanEditEntry->vtpVlanEditName); 
    data->vtpVlanEditDot10Said =
	CloneOctetString(vtpVlanEditEntry->vtpVlanEditDot10Said); 
    
    return data;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
vtpVlanEditEntry_test (OID *incoming, ObjectInfo *object, ObjectSyntax
		       *value,doList_t *doHead, doList_t *doCur,
		       ContextInfo *contextInfo) {
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    vtpVlanEditEntry_t     *vtpVlanEditEntry;
    long           managementDomainIndex;
    long           vtpVlanEditIndex;
    
    instLength = incoming->length - object->oid.length;
    error_status = NO_ERROR;
    carry = 0;
    
    /*
     * Validate the object instance
     *
     */
    if (instLength != 2) {
        return NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length,
		    &managementDomainIndex, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length,
		    &vtpVlanEditIndex, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    vtpVlanEditEntry = k_vtpVlanEditEntry_get(-1, contextInfo, -1,
					      EXACT,
					      managementDomainIndex,
					      vtpVlanEditIndex); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == vtpVlanEditEntry_set) &&
            (((vtpVlanEditEntry_t *) (dp->data)) != NULL) &&
            (((vtpVlanEditEntry_t *)
	      (dp->data))->managementDomainIndex ==
	     managementDomainIndex) && 
            (((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditIndex ==
	     vtpVlanEditIndex) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = vtpVlanEditEntry_set;
        dp->cleanupMethod = vtpVlanEditEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_vtpVlanEditEntry_UNDO
        dp->undoMethod = vtpVlanEditEntry_undo;
#else /* SR_vtpVlanEditEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_vtpVlanEditEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (vtpVlanEditEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_vtpVlanEditEntry(vtpVlanEditEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_vtpVlanEditEntry(vtpVlanEditEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ((dp->data = malloc(sizeof(vtpVlanEditEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(vtpVlanEditEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((vtpVlanEditEntry_t *)
		 (dp->data))->managementDomainIndex =
		     (managementDomainIndex); 
                SET_VALID(I_managementDomainIndex,
			  ((vtpVlanEditEntry_t *) (dp->data))->valid);
		
                ((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditIndex
		    = (vtpVlanEditIndex); 
                SET_VALID(I_vtpVlanEditIndex, ((vtpVlanEditEntry_t *)
					       (dp->data))->valid); 
		
                error_status = k_vtpVlanEditEntry_set_defaults(dp);
            }
        }
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_vtpVlanEditState
    case I_vtpVlanEditState:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditState =
	    value->sl_value; 
	break;
#endif /* I_vtpVlanEditState */
	
#ifdef I_vtpVlanEditType
    case I_vtpVlanEditType:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditType = value->sl_value;
	break;
#endif /* I_vtpVlanEditType */
	
#ifdef I_vtpVlanEditName
    case I_vtpVlanEditName:
	
	if (((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditName != NULL) {
	    FreeOctetString(((vtpVlanEditEntry_t *)
			     (dp->data))->vtpVlanEditName); 
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditName = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_vtpVlanEditName */
	
#ifdef I_vtpVlanEditMtu
    case I_vtpVlanEditMtu:
	
	if ((value->sl_value < 1500) || (value->sl_value > 18190)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditMtu = value->sl_value;
	break;
#endif /* I_vtpVlanEditMtu */
	
#ifdef I_vtpVlanEditDot10Said
    case I_vtpVlanEditDot10Said:
	
	if (((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditDot10Said
	    != NULL) { 
	    FreeOctetString(((vtpVlanEditEntry_t *)
			     (dp->data))->vtpVlanEditDot10Said); 
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditDot10Said = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_vtpVlanEditDot10Said */

#ifdef I_vtpVlanEditRingNumber
    case I_vtpVlanEditRingNumber:
 
	if ( ((value->sl_value < 0) || (value->sl_value > 4095)) ) {
	    return WRONG_VALUE_ERROR;
	}
 
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditRingNumber =
	    value->sl_value;
	break;
#endif /* I_vtpVlanEditRingNumber */
 
#ifdef I_vtpVlanEditBridgeNumber
    case I_vtpVlanEditBridgeNumber:
 
	if ( ((value->sl_value < 1) || (value->sl_value > 15)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditBridgeNumber =
	    value->sl_value; 
	break;
#endif /* I_vtpVlanEditBridgeNumber */
	
#ifdef I_vtpVlanEditParentVlan
    case I_vtpVlanEditParentVlan:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 1023)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditParentVlan =
	    value->sl_value;
	break;
#endif /* I_vtpVlanEditParentVlan */
	
#ifdef I_vtpVlanEditTranslationalVlan1
    case I_vtpVlanEditTranslationalVlan1:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 1023)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditTranslationalVlan1 =
	    value->sl_value; 
	break;
#endif /* I_vtpVlanEditTranslationalVlan1 */
	
#ifdef I_vtpVlanEditTranslationalVlan2
    case I_vtpVlanEditTranslationalVlan2:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 1023)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditTranslationalVlan2 =
	    value->sl_value; 
	break;
#endif /* I_vtpVlanEditTranslationalVlan2 */
	
#ifdef I_vtpVlanEditRowStatus
    case I_vtpVlanEditRowStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_vtpVlanEditRowStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (vtpVlanEditEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value ==
		D_vtpVlanEditRowStatus_notInService) ||
	       (value->sl_value == D_vtpVlanEditRowStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value ==
		 D_vtpVlanEditRowStatus_createAndGo) ||
		(value->sl_value ==
		 D_vtpVlanEditRowStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditRowStatus =
	    value->sl_value; 
	break;
#endif /* I_vtpVlanEditRowStatus */

#ifdef I_vtpVlanEditBridgeType
    case I_vtpVlanEditBridgeType:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vtpVlanEditEntry_t *) (dp->data))->vtpVlanEditBridgeType = value->sl_value;
	break;
#endif /* I_vtpVlanEditBridgeType */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator"
		 " in vtpVlanEditEntry_test)\n")); 
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_vtpVlanEditEntry_test */
    error_status = k_vtpVlanEditEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((vtpVlanEditEntry_t *)
				      (dp->data))->valid); 
        error_status = k_vtpVlanEditEntry_ready(object, value, doHead, dp);
    }
    
   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vtpVlanEditEntry_set (doList_t *doHead, doList_t *doCur, ContextInfo
		      *contextInfo) {
    return (k_vtpVlanEditEntry_set((vtpVlanEditEntry_t *) (doCur->data),
				   contextInfo, doCur->state));
}

#endif /* SETS */

/*---------------------------------------------------------------------
 * Retrieve data from the vlanTrunkPorts family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
vlanTrunkPorts_get (OID* incoming, ObjectInfo* object, int searchType,
		    ContextInfo* contextInfo, int serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    vlanTrunkPorts_t *data;
    
    data = NULL;
    instLength = incoming->length - object->oid.length;
    arg = -1;
    
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and
     * the single instance element be 0.
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    
    switch (searchType) {
    case EXACT:
        if (instLength == 1 && incoming->oid_ptr[incoming->length - 1] == 0) {
	    arg = object->nominator;
        }
        break;
	
    case NEXT:
        if (instLength <= 0) {
	    arg = object->nominator;
        }
        break;
	
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_vlanTrunkPorts_get(serialNum,
						     contextInfo,
						     arg)) == NULL) { 
        arg = -1;
    }

    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_vlanTrunkPortSetSerialNo
    case I_vlanTrunkPortSetSerialNo:
	dp = (void *) (&data->vlanTrunkPortSetSerialNo);
	break;
#endif /* I_vlanTrunkPortSetSerialNo */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &ZeroOid, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the vlanTrunkPorts data object.
 *---------------------------------------------------------------------*/
void
vlanTrunkPorts_free (vlanTrunkPorts_t *data) {
    if (data != NULL) {
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after vlanTrunkPorts set/undo
 *---------------------------------------------------------------------*/
static int
vlanTrunkPorts_cleanup (doList_t *trash) {
    vlanTrunkPorts_free(trash->data);
#ifdef SR_SNMPv2
    vlanTrunkPorts_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
vlanTrunkPorts_test (OID* incoming, ObjectInfo* object, ObjectSyntax*
		     value, doList_t* doHead, doList_t* doCur,
		     ContextInfo* contextInfo) {
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;

    instLength = incoming->length - object->oid.length;
    error_status = NO_ERROR;

    /*
     * Validate the object instance: 1) It must be of length 1  2) and the
     * instance must be 0.
     */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == vlanTrunkPorts_set) &&
            (((vlanTrunkPorts_t *) (dp->data)) != NULL) ) {
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(vlanTrunkPorts_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(vlanTrunkPorts_t));
#ifdef SR_SNMPv2
        if ( (dp->undodata = malloc(sizeof(vlanTrunkPorts_t))) == NULL) {
            DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->undodata, 0, sizeof(vlanTrunkPorts_t));
#endif /* SR_SNMPv2 */
	
        dp->setMethod = vlanTrunkPorts_set;
        dp->cleanupMethod = vlanTrunkPorts_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_vlanTrunkPorts_UNDO
        dp->undoMethod = vlanTrunkPorts_undo;
#else /* SR_vlanTrunkPorts_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_vlanTrunkPorts_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_vlanTrunkPortSetSerialNo
    case I_vlanTrunkPortSetSerialNo:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vlanTrunkPorts_t *) (dp->data))->vlanTrunkPortSetSerialNo =
	    value->sl_value; 
	break;
#endif /* I_vlanTrunkPortSetSerialNo */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in vlanTrunkPorts_test)\n")); 
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_vlanTrunkPorts_test */
    error_status = k_vlanTrunkPorts_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((vlanTrunkPorts_t *) (dp->data))->valid);
        error_status = k_vlanTrunkPorts_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vlanTrunkPorts_set (doList_t* doHead, doList_t* doCur, ContextInfo*
		    contextInfo) {
    return (k_vlanTrunkPorts_set((vlanTrunkPorts_t *) (doCur->data),
				 contextInfo, doCur->state));
}

#endif /* SETS */

/*---------------------------------------------------------------------
 * Retrieve data from the vlanTrunkPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
vlanTrunkPortEntry_get (OID *incoming, ObjectInfo *object, int
			searchType, ContextInfo *contextInfo, int
			serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    vlanTrunkPortEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            vlanTrunkPortIfIndex;

    data = NULL;
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    
    if (searchType == EXACT) {
        if (instLength != 1) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length,
		    &vlanTrunkPortIfIndex, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_vlanTrunkPortEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 vlanTrunkPortIfIndex)) == NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->vlanTrunkPortIfIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_vlanTrunkPortManagementDomain
    case I_vlanTrunkPortManagementDomain:
	dp = (void *) (&data->vlanTrunkPortManagementDomain);
	break;
#endif /* I_vlanTrunkPortManagementDomain */
	
#ifdef I_vlanTrunkPortEncapsulationType
    case I_vlanTrunkPortEncapsulationType:
	dp = (void *) (&data->vlanTrunkPortEncapsulationType);
	break;
#endif /* I_vlanTrunkPortEncapsulationType */
	
#ifdef I_vlanTrunkPortVlansEnabled
    case I_vlanTrunkPortVlansEnabled:
	dp = (void *) (CloneOctetString(data->vlanTrunkPortVlansEnabled));
	break;
#endif /* I_vlanTrunkPortVlansEnabled */
	
#ifdef I_vlanTrunkPortNativeVlan
    case I_vlanTrunkPortNativeVlan:
	dp = (void *) (&data->vlanTrunkPortNativeVlan);
	break;
#endif /* I_vlanTrunkPortNativeVlan */

#ifdef I_vlanTrunkPortRowStatus
    case I_vlanTrunkPortRowStatus:
	dp = (void *) (&data->vlanTrunkPortRowStatus);
	break;
#endif /* I_vlanTrunkPortRowStatus */

#ifdef I_vlanTrunkPortInJoins
    case I_vlanTrunkPortInJoins:
	dp = (void *) (&data->vlanTrunkPortInJoins);
	break;
#endif /* I_vlanTrunkPortInJoins */
	
#ifdef I_vlanTrunkPortOutJoins
    case I_vlanTrunkPortOutJoins:
	dp = (void *) (&data->vlanTrunkPortOutJoins);
	break;
#endif /* I_vlanTrunkPortOutJoins */
	
#ifdef I_vlanTrunkPortOldAdverts
    case I_vlanTrunkPortOldAdverts:
	dp = (void *) (&data->vlanTrunkPortOldAdverts);
	break;
#endif /* I_vlanTrunkPortOldAdverts */
	
#ifdef I_vlanTrunkPortVlansPruningEligible
    case I_vlanTrunkPortVlansPruningEligible:
	dp = (void *) (CloneOctetString(data->vlanTrunkPortVlansPruningEligible));
	break;
#endif /* I_vlanTrunkPortVlansPruningEligible */
	
#ifdef I_vlanTrunkPortVlansXmitJoined
    case I_vlanTrunkPortVlansXmitJoined:
	dp = (void *) (CloneOctetString(data->vlanTrunkPortVlansXmitJoined));
	break;
#endif /* I_vlanTrunkPortVlansXmitJoined */
	
#ifdef I_vlanTrunkPortVlansRcvJoined
    case I_vlanTrunkPortVlansRcvJoined:
	dp = (void *) (CloneOctetString(data->vlanTrunkPortVlansRcvJoined));
	break;
#endif /* I_vlanTrunkPortVlansRcvJoined */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the vlanTrunkPortEntry data object.
 *---------------------------------------------------------------------*/
void
vlanTrunkPortEntry_free (vlanTrunkPortEntry_t *data) {
    if (data != NULL) {
	FreeOctetString(data->vlanTrunkPortVlansEnabled);
	FreeOctetString(data->vlanTrunkPortVlansPruningEligible);
	FreeOctetString(data->vlanTrunkPortVlansXmitJoined);
	FreeOctetString(data->vlanTrunkPortVlansRcvJoined);	

	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after vlanTrunkPortEntry set/undo
 *---------------------------------------------------------------------*/
static int
vlanTrunkPortEntry_cleanup (doList_t *trash) {
    vlanTrunkPortEntry_free(trash->data);
#ifdef SR_SNMPv2
    vlanTrunkPortEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the vlanTrunkPortEntry family
 *---------------------------------------------------------------------*/
vlanTrunkPortEntry_t *
Clone_vlanTrunkPortEntry (vlanTrunkPortEntry_t *vlanTrunkPortEntry) {
    vlanTrunkPortEntry_t *data;
    
    if ((data = malloc(sizeof(vlanTrunkPortEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (vlanTrunkPortEntry),
	   sizeof(vlanTrunkPortEntry_t)); 
    
    data->vlanTrunkPortVlansEnabled =
	CloneOctetString(vlanTrunkPortEntry->vlanTrunkPortVlansEnabled); 
    data->vlanTrunkPortVlansPruningEligible =
	CloneOctetString(vlanTrunkPortEntry->vlanTrunkPortVlansPruningEligible); 
    data->vlanTrunkPortVlansXmitJoined =
	CloneOctetString(vlanTrunkPortEntry->vlanTrunkPortVlansXmitJoined); 
    data->vlanTrunkPortVlansRcvJoined =
	CloneOctetString(vlanTrunkPortEntry->vlanTrunkPortVlansRcvJoined); 
    
    return data;
}

/*---------------------------------------------------------------------
 * Determine if this SET request is valid. If so, add it to the do-list.
 * This operation is performed in 4 steps:
 *
 *   1) Validate the object instance.
 *   2) Locate a "related" do-list element, or create a new do-list
 *      element.
 *   3) Add the SET request to the selected do-list element.
 *   4) Finally, determine if the do-list element (all "related" set
 *      requests) is completely valid.
 *---------------------------------------------------------------------*/
int 
vlanTrunkPortEntry_test (OID *incoming, ObjectInfo *object,
			 ObjectSyntax *value, doList_t *doHead,
			 doList_t *doCur, ContextInfo *contextInfo) {
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    vlanTrunkPortEntry_t     *vlanTrunkPortEntry;
    long           vlanTrunkPortIfIndex;

    instLength = incoming->length - object->oid.length; 
    error_status = NO_ERROR;
    carry = 0;
    /*
     * Validate the object instance
     *
     */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length,
		    &vlanTrunkPortIfIndex, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    vlanTrunkPortEntry = k_vlanTrunkPortEntry_get(-1, contextInfo, -1,
						  EXACT,
						  vlanTrunkPortIfIndex); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == vlanTrunkPortEntry_set) &&
            (((vlanTrunkPortEntry_t *) (dp->data)) != NULL) &&
            (((vlanTrunkPortEntry_t *)
	      (dp->data))->vlanTrunkPortIfIndex ==
	     vlanTrunkPortIfIndex) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = vlanTrunkPortEntry_set;
        dp->cleanupMethod = vlanTrunkPortEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_vlanTrunkPortEntry_UNDO
        dp->undoMethod = vlanTrunkPortEntry_undo;
#else /* SR_vlanTrunkPortEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_vlanTrunkPortEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (vlanTrunkPortEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_vlanTrunkPortEntry(vlanTrunkPortEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_vlanTrunkPortEntry(vlanTrunkPortEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
	    
#ifdef vlanTrunkPortEntry_READ_CREATE 
            if ((dp->data = malloc(sizeof(vlanTrunkPortEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(vlanTrunkPortEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((vlanTrunkPortEntry_t *)
		 (dp->data))->vlanTrunkPortIfIndex =
		     (vlanTrunkPortIfIndex); 
                SET_VALID(I_vlanTrunkPortIfIndex,
			  ((vlanTrunkPortEntry_t *)
			   (dp->data))->valid); 
		
                error_status = k_vlanTrunkPortEntry_set_defaults(dp);
            }
#else /* vlanTrunkPortEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* vlanTrunkPortEntry_READ_CREATE */
        }
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_vlanTrunkPortManagementDomain
    case I_vlanTrunkPortManagementDomain:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 255)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vlanTrunkPortEntry_t *)
	 (dp->data))->vlanTrunkPortManagementDomain = value->sl_value;
	
	break;
#endif /* I_vlanTrunkPortManagementDomain */

#ifdef I_vlanTrunkPortEncapsulationType
    case I_vlanTrunkPortEncapsulationType:
	
	if ((value->sl_value < 1) || (value->sl_value > 3)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vlanTrunkPortEntry_t *)
	 (dp->data))->vlanTrunkPortEncapsulationType =
	     value->sl_value; 
	break;
#endif /* I_vlanTrunkPortEncapsulationType */
	
#ifdef I_vlanTrunkPortVlansEnabled
    case I_vlanTrunkPortVlansEnabled:
	
     if (((vlanTrunkPortEntry_t *)
	  (dp->data))->vlanTrunkPortVlansEnabled != NULL) { 
	 FreeOctetString(((vlanTrunkPortEntry_t *)
			  (dp->data))->vlanTrunkPortVlansEnabled); 
     }
	
	((vlanTrunkPortEntry_t *) (dp->data))->vlanTrunkPortVlansEnabled = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_vlanTrunkPortVlansEnabled */

#ifdef I_vlanTrunkPortNativeVlan
    case I_vlanTrunkPortNativeVlan:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 1023)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((vlanTrunkPortEntry_t *) (dp->data))->vlanTrunkPortNativeVlan
	    = value->sl_value; 
	break;
#endif /* I_vlanTrunkPortNativeVlan */

#ifdef I_vlanTrunkPortRowStatus 
    case I_vlanTrunkPortRowStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_vlanTrunkPortRowStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
 
	if (vlanTrunkPortEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value ==
		D_vlanTrunkPortRowStatus_notInService) ||
	       (value->sl_value ==
		D_vlanTrunkPortRowStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value ==
		 D_vlanTrunkPortRowStatus_createAndGo) ||
		(value->sl_value ==
		 D_vlanTrunkPortRowStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
 
	((vlanTrunkPortEntry_t *)
	 (dp->data))->vlanTrunkPortRowStatus = value->sl_value; 
	break;
#endif /* vlanTrunkPortRowStatus */

#ifdef I_vlanTrunkPortVlansPruningEligible
    case I_vlanTrunkPortVlansPruningEligible:

	if (((vlanTrunkPortEntry_t *)
	     (dp->data))->vlanTrunkPortVlansPruningEligible != NULL) {
	    FreeOctetString(((vlanTrunkPortEntry_t *)
			     (dp->data))->vlanTrunkPortVlansPruningEligible); 
	}

	((vlanTrunkPortEntry_t *) (dp->data))->vlanTrunkPortVlansPruningEligible = 
	    CloneOctetString(value->os_value);

	break;
#endif /* I_vlanTrunkPortVlansPruningEligible */
	
    default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in
vlanTrunkPortEntry_test)\n")); 
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_vlanTrunkPortEntry_test */
    error_status = k_vlanTrunkPortEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((vlanTrunkPortEntry_t *)
				      (dp->data))->valid); 
        error_status = k_vlanTrunkPortEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
vlanTrunkPortEntry_set (doList_t *doHead, doList_t *doCur, ContextInfo
			*contextInfo) {
    return (k_vlanTrunkPortEntry_set((vlanTrunkPortEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */

vtpStatus_t *
k_vtpStatus_get (int serialNum, ContextInfo *contextInfo, int
		 nominator) {
    static vtpStatus_t vtpStatusData;
    static vtp_device_info vtpDevice;
    
    vtp_get_device_info(&vtpDevice);

    vtpStatusData.vtpVersion = vtpDevice.version;
    vtpStatusData.vtpMaxVlanStorage = vtpDevice.max_vlan_storage;

    vtpStatusData.vtpNotificationsEnabled =
	vtp_dep_get_notifications_enabled(); 

    return(&vtpStatusData);
}

#ifdef SETS
int
k_vtpStatus_test (ObjectInfo *object, ObjectSyntax *value, doList_t
		  *dp, ContextInfo *contextInfo) {
    
    return NO_ERROR;
}

int
k_vtpStatus_ready (ObjectInfo *object, ObjectSyntax *value, doList_t
		   *doHead, doList_t *dp) {

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_vtpStatus_set (vtpStatus_t *data, ContextInfo *contextInfo, int
		 function) {

    vtp_dep_notification_enabled(data->vtpNotificationsEnabled); 

    return(NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_vtpStatus_UNDO
/* add #define SR_vtpStatus_UNDO in vtpmib.h to
 * include the undo routine for the vtpStatus family.
 */
int
vtpStatus_undo (doList_t *doHead, doList_t *doCur, ContextInfo
		*contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_vtpStatus_UNDO */
#endif /* SR_SNMPv2 */

/* k_get_vtp_domain
 *
 * Called by the VTP MIB system dependant code to retrieve a VTP
 * domain structure according to the snmp research get/get-next
 * paradigm. Upon success (return code of VTP_OK) the vtp_domain_info
 * pointer will be filled in with the correct domain info (note that
 * the routine does not allocate space, an appropriately sized chunk
 * of memory must be passed in by the function caller).  The routine
 * searches for the domain info based on the domain_name and
 * domain_name_length fields in the vtp_domain_info structure passed
 * in as a parameter.  If no domain is found then
 * VTP_ERROR_DOMAIN_UNKNOWN is returned. 
 */

unsigned char k_get_vtp_domain (vtp_domain_info* d_info, 
				unsigned char type) {
    unsigned char error;
    vtp_domain_info info, next;

    next.index = 0;
    error = vtp_get_domain_info(&info, VTP_FIRST);
    while (!error) {
	if (info.index == d_info->index) {
	    memcpy(d_info, &info, sizeof(vtp_domain_info));
	    return VTP_OK;
	}
	if (info.index > d_info->index)
	    if ((!next.index) || (info.index < next.index))
		memcpy(&next, &info, sizeof(vtp_domain_info));
	error = vtp_get_domain_info(&info, VTP_NEXT);
    }
    if ((type == EXACT) || (!next.index))
	return VTP_ERROR_NO_DOMAIN;
    
    memcpy(d_info, &next, sizeof(vtp_domain_info));
    return VTP_OK;
}

managementDomainEntry_t *
k_managementDomainEntry_get (int serialNum, ContextInfo *contextInfo,
			     int nominator, int searchType, 
			     long managementDomainIndex) {
    static managementDomainEntry_t managementDomainEntryData;
    static vtp_domain_info vtpInfo;
    vtpEditDomainType* editDomain;
    static OctetString domain_name, last_change, file_name;
    unsigned char error;
  
    vtpInfo.index = managementDomainIndex;
    error = k_get_vtp_domain(&vtpInfo, searchType);

    if (error)
	return NULL;

    /* should never fail */
    editDomain = vtp_edit_get_domain_info(searchType,
					  managementDomainIndex);
    if (!editDomain)
	return NULL;

    vtp_get_statistics(vtpInfo.domain_name_length, vtpInfo.domain_name,
		       (ulong *)
		       &managementDomainEntryData.vtpInSummaryAdverts,
		       (ulong *)
		       &managementDomainEntryData.vtpInSubsetAdverts,
		       (ulong *)
		       &managementDomainEntryData.vtpInAdvertRequests,
		       (ulong *)
		       &managementDomainEntryData.vtpOutSummaryAdverts,  
		       (ulong *)
		       &managementDomainEntryData.vtpOutSubsetAdverts,
		       (ulong *)
		       &managementDomainEntryData.vtpOutAdvertRequests,  
		       (ulong *)
		       &managementDomainEntryData.vtpConfigRevNumberErrors,
		       (ulong *)
		       &managementDomainEntryData.vtpConfigDigestErrors,
		       VTP_STATS_FIXED);
    
    if (error)
	return NULL; /* should never happen */

    managementDomainEntryData.managementDomainIndex = vtpInfo.index;
    domain_name.octet_ptr = vtpInfo.domain_name;
    domain_name.length = vtpInfo.domain_name_length;
    managementDomainEntryData.managementDomainName = &domain_name;
    managementDomainEntryData.managementDomainLocalMode = vtpInfo.mode;
    managementDomainEntryData.managementDomainConfigRevNumber =
	vtpInfo.revision ;
    managementDomainEntryData.managementDomainLastUpdater =
	vtpInfo.updater;
    last_change.octet_ptr = vtpInfo.timestamp;
    last_change.length = VTP_TIMESTAMP_SIZE;
    managementDomainEntryData.managementDomainLastChange =
	&last_change;
    managementDomainEntryData.managementDomainTftpServer =
	vtpInfo.tftp_server; 

    file_name.octet_ptr = vtpInfo.tftp_file_name;
    file_name.length = strlen(file_name.octet_ptr);
    managementDomainEntryData.managementDomainTftpPathname =
	&file_name;
	
    /* If we found an entry then it's active */
    managementDomainEntryData.managementDomainRowStatus =
	D_managementDomainRowStatus_active; 
    managementDomainEntryData.managementDomainPruningState =
	vtpInfo.pruning_mode ? D_managementDomainPruningState_enabled
	    : D_managementDomainPruningState_disabled;
    managementDomainEntryData.vtpVlanEditOperation =
	D_vtpVlanEditOperation_none; 
    managementDomainEntryData.vtpVlanApplyStatus = editDomain->status;
    managementDomainEntryData.vtpVlanEditBufferOwner = editDomain->owner;
    managementDomainEntryData.vtpVlanEditConfigRevNumber = editDomain->rev_num;
    managementDomainEntryData.managementDomainIndex =
	editDomain->managementDomainIndex; 

    return(&managementDomainEntryData);
}

#ifdef SETS
int
k_managementDomainEntry_test (ObjectInfo *object, ObjectSyntax *value,
			      doList_t *dp, ContextInfo *contextInfo)
{
    managementDomainEntry_t* data;
    vtpEditDomainType* e_domainInfo;
    vtp_domain_info domain;

    data = (managementDomainEntry_t*)(dp->data);
    if (data->managementDomainIndex > 255)
	return (INCONSISTENT_NAME_ERROR);

    switch (object->nominator) {
    case I_managementDomainName: 
	if (!IsDisplayString(value->os_value))
	    return (WRONG_VALUE_ERROR);
	
	if ((value->os_value->length < 1) || (value->os_value->length > 32))
	    return (WRONG_LENGTH_ERROR);
	
	break;
    case I_managementDomainLocalMode:
#ifdef IOS_SPECIFIC
	if (value->sl_value ==
	    D_managementDomainLocalMode_transparent) 
	    return (WRONG_VALUE_ERROR);
#endif
	break;
    case I_managementDomainRowStatus:

	if (!vtp_dep_validate_managementDomainRowStatus(value->sl_value))
	    return WRONG_VALUE_ERROR;

	break;
    case I_vtpVlanEditOperation:
	if (value->sl_value == D_vtpVlanEditOperation_copy) {
	    e_domainInfo = 
		vtp_edit_get_domain_info(EXACT,
					 data->managementDomainIndex);
	    
	    /* If the row doesn't exist yet, an error.  Also if there
	       already exists entries in the vtpVlanEditTable then
	       this operation is prohibited. */
	    
	    if ((!e_domainInfo) || (e_domainInfo->head))
		return (INCONSISTENT_VALUE_ERROR);
	    
	    domain.index = data->managementDomainIndex;
	    if (k_get_vtp_domain(&domain, EXACT))
		return (GEN_ERROR); /* should never happen */

	    if (domain.mode == D_managementDomainLocalMode_client)
		return (INCONSISTENT_VALUE_ERROR);
	}
	break;
    case I_vtpVlanEditBufferOwner:
	if (!IsDisplayString(value->os_value))
	    return WRONG_VALUE_ERROR;
    }

    return NO_ERROR;
}

int
k_managementDomainEntry_ready ( ObjectInfo *object, ObjectSyntax
			       *value, doList_t *doHead, doList_t *dp)
{
    managementDomainEntry_t* data;
    vtp_domain_info test_info;
    unsigned char error;

    data = (managementDomainEntry_t*)(dp->data);

    switch (data->managementDomainRowStatus) {
    case D_managementDomainRowStatus_active:
	dp->state = ADD_MODIFY;
	break;
    case D_managementDomainRowStatus_createAndGo:
#ifdef IOS_SPECIFIC
	if ((data->managementDomainLocalMode ==
	     D_managementDomainLocalMode_server) &&
	    ((!VALID(I_managementDomainTftpServer, data->valid)) ||
	     (!VALID(I_managementDomainTftpPathname, data->valid))))
	    break;
#endif
	if (VALID(I_managementDomainName,
		  ((managementDomainEntry_t*)(dp->data))->valid)) {
	    memcpy(test_info.domain_name,
		   data->managementDomainName->octet_ptr,
		   data->managementDomainName->length);
	    test_info.domain_name_length =
		data->managementDomainName->length;
	    error = vtp_get_domain_info(&test_info, VTP_EXACT);

	    /* if we found an entry with this name already in use,
	       return an error */
	    if (!error) 
		return(INCONSISTENT_VALUE_ERROR);
	    dp->state = ADD_MODIFY;
	}
	break;
    case D_managementDomainRowStatus_destroy:
	dp->state = DELETE;
	break;
    default:
	dp->state = UNKNOWN;
    }
    return NO_ERROR;
}

int
k_managementDomainEntry_set_defaults (doList_t *dp) {
    managementDomainEntry_t *data;
    
    data = (managementDomainEntry_t *) (dp->data);

    data->managementDomainLocalMode =
	D_managementDomainLocalMode_server;
    data->managementDomainPruningState =
	D_managementDomainPruningState_disabled; 

    data->managementDomainRowStatus = UNUSED;

    return NO_ERROR;
}

int
k_managementDomainEntry_set (managementDomainEntry_t *data,
			     ContextInfo *contextInfo, int function) {
    vtp_domain_info d_info;

    d_info.domain_name_length =	data->managementDomainName->length;
    memcpy(d_info.domain_name, data->managementDomainName->octet_ptr,
	   d_info.domain_name_length);

    switch (data->managementDomainRowStatus) {
    case D_managementDomainRowStatus_active: 
	vtp_get_domain_info(&d_info, VTP_EXACT);

	if (VALID(I_managementDomainName, data->valid))
	    vtp_post_change_domain_name(d_info.domain_name_length, 
					d_info.domain_name,
					data->managementDomainName->length, 
					data->managementDomainName->octet_ptr);

	if (VALID(I_managementDomainLocalMode, data->valid))
	    d_info.mode = data->managementDomainLocalMode;
	    
	if (VALID(I_managementDomainTftpServer, data->valid))
	    d_info.tftp_server = data->managementDomainTftpServer;

	if (VALID(I_managementDomainTftpPathname, data->valid)) {
	    memcpy(d_info.tftp_file_name,
		   data->managementDomainTftpPathname->octet_ptr,
		   data->managementDomainTftpPathname->length);
	    d_info.tftp_file_name[data->managementDomainTftpPathname->length]
		= '\0';
	}
	
	if (VALID(I_managementDomainPruningState, data->valid)) {
	    d_info.pruning_mode = data->managementDomainPruningState
		== D_managementDomainPruningState_enabled ?
		    VTP_PRUNING_ENABLE : VTP_PRUNING_DISABLE; 
	}
	    
	if ((VALID(I_managementDomainLocalMode, data->valid)) ||
	    (VALID(I_managementDomainTftpServer, data->valid)) ||
	    (VALID(I_managementDomainTftpPathname, data->valid)) || 
	    (VALID(I_managementDomainPruningState, data->valid)))
	    vtp_post_configure_domain(VTP_MODIFY, &d_info);
	
	if (VALID(I_vtpVlanEditConfigRevNumber, data->valid))
	    vtp_edit_set_revision_number(data->managementDomainIndex,
					 data->vtpVlanEditConfigRevNumber);
	if (VALID(I_vtpVlanEditBufferOwner, data->valid))
	    vtp_edit_set_owner(data->managementDomainIndex,
			       data->vtpVlanEditBufferOwner); 
	
	if (VALID(I_vtpVlanEditOperation, data->valid))
	    if (!vtp_edit_do_operation(data->managementDomainIndex,
				       data->vtpVlanEditOperation))
		return(RESOURCE_UNAVAILABLE_ERROR); /* sorry, out of memory,
						       or worse! */ 
	break;
    case D_managementDomainRowStatus_createAndGo:
	
	if (!vtp_edit_add_domain_info(data->managementDomainIndex))
	    return (RESOURCE_UNAVAILABLE_ERROR);
	
	d_info.index = data->managementDomainIndex;
	d_info.mode = data->managementDomainLocalMode;
	d_info.updater = vtp_dep_get_local_ipaddr();
	vtp_get_timestamp(d_info.timestamp);
	d_info.advt_interval = VTP_DEFAULT_ADVT_INTERVAL;
	d_info.password_length = 0;
	memcpy(d_info.tftp_file_name,
	       data->managementDomainTftpPathname->octet_ptr,
	       data->managementDomainTftpPathname->length);
	d_info.tftp_file_name[data->managementDomainTftpPathname->length]
	    = '\0';
	d_info.tftp_server = data->managementDomainTftpServer;	
	d_info.pruning_mode = data->managementDomainPruningState ==
	    D_managementDomainPruningState_enabled ?
		VTP_PRUNING_ENABLE : VTP_PRUNING_DISABLE;  
	vtp_post_configure_domain(VTP_ADD, &d_info);
	
	if (VALID(I_vtpVlanEditConfigRevNumber, data->valid))
	    vtp_edit_set_revision_number(data->managementDomainIndex,
					 data->vtpVlanEditConfigRevNumber);
	if (VALID(I_vtpVlanEditBufferOwner, data->valid))
	    vtp_edit_set_owner(data->managementDomainIndex,
			       data->vtpVlanEditBufferOwner); 
	
	if (VALID(I_vtpVlanEditOperation, data->valid))
	    if (!vtp_edit_do_operation(data->managementDomainIndex,
				      data->vtpVlanEditOperation))
		return(RESOURCE_UNAVAILABLE_ERROR); /* sorry, out of memory,
						       or worse! */ 
			    
	break;
    case D_managementDomainRowStatus_destroy:
	vtp_edit_delete_domain_info(data->managementDomainIndex);
	vtp_post_configure_domain(VTP_DELETE, &d_info); 
    }
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_managementDomainEntry_UNDO
/* add #define SR_managementDomainEntry_UNDO in vtpmib.h to
 * include the undo routine for the managementDomainEntry family.
 */
int
managementDomainEntry_undo (doList_t *doHead, doList_t *doCur,
			    ContextInfo *contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_managementDomainEntry_UNDO */
#endif /* SR_SNMPv2 */

unsigned char k_get_vtp_vlan (unsigned char dname_length, 
			      unsigned char* domain_name, 
			      vtp_vlan_info* v_info,  
			      unsigned char searchType) {

    if (vtp_get_vlan_info(dname_length, domain_name, v_info, EXACT))
	return vtp_get_vlan_info(dname_length, domain_name, v_info,
				 searchType); 
    else 
	return VTP_OK;
}

vtpVlanEntry_t *
k_vtpVlanEntry_get (int serialNum, ContextInfo *contextInfo, int
		    nominator, int searchType, long
		    managementDomainIndex, long vtpVlanIndex) {
   static vtpVlanEntry_t vtpVlanEntryData;
   static vtp_vlan_info info;
   static OctetString vtp_vlan_name, vtp_said;
   static unsigned long said_value;
   managementDomainEntry_t* domain_info;
   vtp_boolean found;

   found = FALSE;
   
   do {
       domain_info = k_managementDomainEntry_get(-1, contextInfo,
						 I_managementDomainName,
						 searchType,
						 managementDomainIndex);
       if (!domain_info)
	   return(NULL);
       
       info.isl_vlan_id = vtpVlanIndex;
       if (k_get_vtp_vlan(domain_info->managementDomainName->length,
			  domain_info->managementDomainName->octet_ptr,
			  &info, searchType)) {
	   if (searchType == NEXT) {
	       vtpVlanIndex = 0; /* less than smallest valid */
	       managementDomainIndex = domain_info->managementDomainIndex
		   + 1;
	   } else 
	       return NULL;
       } else {
	   found = TRUE;
	   switch (nominator) {
	   case I_vtpVlanRingNumber:
	   case I_vtpVlanParentVlan:
	   case I_vtpVlanBridgeType:
	       if ((info.type != VLAN_TR_TYPE) && 
		   !((info.type == VLAN_FDDI_TYPE) && info.parent_valid))
		   found = FALSE;
	       break;
	   case I_vtpVlanBridgeNumber:
	       if ((info.type != VLAN_TR_NET_TYPE) && (info.type !=
						       VLAN_FDDI_NET_TYPE)) 
		   found = FALSE;
	       break;
	   }
	   if (!found)
	       if (searchType != NEXT)
		   return NULL;
	       else
		   vtpVlanIndex = 
		       info.isl_vlan_id + 1;
       }
   } while ((!found) && (searchType == NEXT));
   vtpVlanEntryData.vtpVlanIndex = info.isl_vlan_id;
   vtpVlanEntryData.vtpVlanState = info.state;
   vtpVlanEntryData.vtpVlanType = info.type;
   vtp_vlan_name.octet_ptr = info.vlan_name;
   vtp_vlan_name.length = info.vlan_name_length;
   vtpVlanEntryData.vtpVlanName = &vtp_vlan_name;
   vtpVlanEntryData.vtpVlanMtu = info.mtu;
   said_value = info.said;
   vtp_said.octet_ptr = (unsigned char*)&said_value;
   vtp_said.length = VTP_SAID_LENGTH;
   vtpVlanEntryData.vtpVlanDot10Said = &vtp_said;
   vtpVlanEntryData.vtpVlanRingNumber = info.ring_number;
   vtpVlanEntryData.vtpVlanBridgeNumber = info.bridge_number;
   vtpVlanEntryData.vtpVlanParentVlan = info.parent_vlan;
   vtpVlanEntryData.vtpVlanTranslationalVlan1 = info.tb_vlan_1;
   vtpVlanEntryData.vtpVlanTranslationalVlan2 = info.tb_vlan_2;
   vtpVlanEntryData.vtpVlanBridgeType = info.stp_type;
   vtpVlanEntryData.managementDomainIndex =
       domain_info->managementDomainIndex; 
   return(&vtpVlanEntryData);
}


vtpVlanEditEntry_t *
k_vtpVlanEditEntry_get (int serialNum, ContextInfo *contextInfo, int
			nominator, int searchType, long
			managementDomainIndex, long vtpVlanEditIndex)
{
    static vtpVlanEditEntry_t vtpVlanEditEntryData;
    static OctetString elan_name, vtp_said;
    static unsigned long said_value;
    vtpEditDomainType* e_domain;
    vtpEditVlanType* vlan;
    vtp_boolean found;

    found = FALSE;

    do {
	e_domain = vtp_edit_get_domain_info(searchType, managementDomainIndex);
	if (!e_domain) 
	    return NULL;
	
	vlan = vtp_edit_get_vlan_info(searchType, e_domain, vtpVlanEditIndex);
	
	if (!vlan) {
	   if (searchType == NEXT) {
	       vtpVlanEditIndex = 0; /* less than smallest valid */
	       managementDomainIndex =
		   e_domain->managementDomainIndex + 1;
	   } else 
	       return NULL;
	} else {
	    found = TRUE;
	    switch (nominator) {
	    case I_vtpVlanEditRingNumber:
	    case I_vtpVlanEditParentVlan:
	    case I_vtpVlanEditBridgeType:
		if ((vlan->info.type != VLAN_TR_TYPE) && 
		    !((vlan->info.type == VLAN_FDDI_TYPE) &&
		      vlan->info.parent_valid))
		    found = FALSE;
		break;
	    case I_vtpVlanBridgeNumber:
		if ((vlan->info.type != VLAN_TR_NET_TYPE) && 
		    (vlan->info.type != VLAN_FDDI_NET_TYPE)) 
		    found = FALSE;
		break;
	    }
	    if (!found)
		if (searchType != NEXT)
		    return NULL;
		else
		    vtpVlanEditIndex = 
			vlan->info.isl_vlan_id + 1;  
	}
    } while ((!found) && (searchType == NEXT));
    
    vtpVlanEditEntryData.vtpVlanEditIndex = vlan->info.isl_vlan_id;
    vtpVlanEditEntryData.vtpVlanEditState = vlan->info.state ==
	D_vtpVlanEditState_operational ?
	    D_vtpVlanEditState_operational : D_vtpVlanEditState_suspended;
    vtpVlanEditEntryData.vtpVlanEditType = vlan->info.type;
    elan_name.octet_ptr = vlan->info.vlan_name;
    elan_name.length = vlan->info.vlan_name_length;
    vtpVlanEditEntryData.vtpVlanEditName = &elan_name;
    vtpVlanEditEntryData.vtpVlanEditMtu = vlan->info.mtu;
    said_value = vlan->info.said;
    vtp_said.octet_ptr = (unsigned char*)&said_value;
    vtp_said.length = VTP_SAID_LENGTH;
    vtpVlanEditEntryData.vtpVlanEditDot10Said = &vtp_said;
    vtpVlanEditEntryData.vtpVlanEditRingNumber =
	vlan->info.ring_number;
    vtpVlanEditEntryData.vtpVlanEditBridgeNumber =
	vlan->info.bridge_number;
    vtpVlanEditEntryData.vtpVlanEditBridgeType = vlan->info.stp_type;
    vtpVlanEditEntryData.vtpVlanEditParentVlan =
	vlan->info.parent_vlan;
    vtpVlanEditEntryData.vtpVlanEditTranslationalVlan1 = vlan->info.tb_vlan_1;
    vtpVlanEditEntryData.vtpVlanEditTranslationalVlan2 = vlan->info.tb_vlan_2;
    vtpVlanEditEntryData.vtpVlanEditRowStatus = D_vtpVlanEditRowStatus_active;
    vtpVlanEditEntryData.managementDomainIndex =
	e_domain->managementDomainIndex; 
    return(&vtpVlanEditEntryData);
}

#ifdef SETS
int
k_vtpVlanEditEntry_test (ObjectInfo *object, ObjectSyntax *value,
			 doList_t *dp, ContextInfo *contextInfo) {

    vtpVlanEditEntry_t* data;
    vtpEditDomainType* e_domain;

    data = (vtpVlanEditEntry_t*)(dp->data);

    if ((data->vtpVlanEditIndex < 1) || (data->vtpVlanEditIndex >
					 1005))
	return INCONSISTENT_NAME_ERROR;

    e_domain = vtp_edit_get_domain_info(EXACT,
					data->managementDomainIndex);

    if (!e_domain) 
	return INCONSISTENT_NAME_ERROR; /* non-existant domain */

    switch (object->nominator) {
    case I_vtpVlanEditName:
	if ((value->os_value->length < 1) || (value->os_value->length
					      > 32))
	    return (WRONG_LENGTH_ERROR);
	if (!IsDisplayString(value->os_value))
	    return (WRONG_VALUE_ERROR);
	break;
    case I_vtpVlanEditDot10Said:
	if (value->os_value->length != VTP_SAID_LENGTH)
	    return (WRONG_LENGTH_ERROR);
	break;
    case I_vtpVlanEditRowStatus:
	/* Don't support these, not necessary */
	if ((value->sl_value == D_vtpVlanEditRowStatus_createAndWait)
	    || (value->sl_value ==
		D_vtpVlanEditRowStatus_notInService))
	    return (WRONG_VALUE_ERROR);
    }
    return NO_ERROR;
}

int
k_vtpVlanEditEntry_ready (ObjectInfo *object, ObjectSyntax *value,
			 doList_t *doHead, doList_t *dp) {
    vtpVlanEditEntry_t* data;

    data = (vtpVlanEditEntry_t*)(dp->data);

    switch (data->vtpVlanEditRowStatus) {
    case D_vtpVlanEditRowStatus_active:
	dp->state = ADD_MODIFY;
	break;
    case D_vtpVlanEditRowStatus_createAndGo:
	if ((VALID(I_vtpVlanEditName, data->valid)) &&
	    (VALID(I_vtpVlanEditDot10Said, data->valid)))
	    dp->state = ADD_MODIFY;
	break;
    case D_vtpVlanEditRowStatus_destroy:
	dp->state = DELETE;
    }
    return NO_ERROR;
}

int
k_vtpVlanEditEntry_set_defaults (doList_t *dp) {
    vtpVlanEditEntry_t *data;

    data = (vtpVlanEditEntry_t *) (dp->data);
    data->vtpVlanEditState = D_vtpVlanEditState_operational;
    data->vtpVlanEditType = D_vtpVlanEditType_ethernet;
    data->vtpVlanEditMtu = 1500;

    /* Initialize the following four objects to indicate that they are
       not instantiated */

    data->vtpVlanEditRingNumber = VTP_VLAN_NO_RING_NUMBER;
    data->vtpVlanEditBridgeNumber = VTP_VLAN_NO_BRIDGE_NUMBER;
    data->vtpVlanEditBridgeType = VTP_VLAN_NO_STP_TYPE;
    data->vtpVlanEditParentVlan = VTP_VLAN_NO_PARENT_VLAN;

    data->vtpVlanEditTranslationalVlan1 = NO_VLANS;
    data->vtpVlanEditTranslationalVlan2 = NO_VLANS;
    data->vtpVlanEditRowStatus = UNUSED;
    return NO_ERROR;
}

int
k_vtpVlanEditEntry_set (vtpVlanEditEntry_t *data, ContextInfo
			*contextInfo, int function) {

    switch (data->vtpVlanEditRowStatus) {
    case D_vtpVlanEditRowStatus_active:
	if (!vtp_edit_update_vlan_info(data))
	    return GEN_ERROR;
	break;
    case D_vtpVlanEditRowStatus_createAndGo:
	if (!vtp_edit_create_vlan_info(data))
	    return GEN_ERROR; /* out of memory or worse */
	break;
    case D_vtpVlanEditRowStatus_destroy:
	vtp_edit_delete_vlan_info(data->managementDomainIndex,
				  data->vtpVlanEditIndex);
    }
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_vtpVlanEditEntry_UNDO
/* add #define SR_vtpVlanEditEntry_UNDO in vtpmib.h to
 * include the undo routine for the vtpVlanEditEntry family.
 */
int
vtpVlanEditEntry_undo (doList_t *doHead, doList_t *doCur, ContextInfo
		       *contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_vtpVlanEditEntry_UNDO */
#endif /* SR_SNMPv2 */

vlanTrunkPorts_t *
k_vlanTrunkPorts_get (int serialNum, ContextInfo* contextInfo, int
		      nominator) {
   static vlanTrunkPorts_t vlanTrunkPortsData;

   vlanTrunkPortsData.vlanTrunkPortSetSerialNo = vtpTrunkSerialNo;
   return(&vlanTrunkPortsData);
}

#ifdef SETS
int
k_vlanTrunkPorts_test(ObjectInfo* object, ObjectSyntax* value,
		      doList_t* dp, ContextInfo* contextInfo) {

    if (value->sl_value != vtpTrunkSerialNo)
	return (INCONSISTENT_VALUE_ERROR);

    return NO_ERROR;
}

int
k_vlanTrunkPorts_ready (ObjectInfo* object, ObjectSyntax* value,
			doList_t* doHead, doList_t* dp) {

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_vlanTrunkPorts_set (vlanTrunkPorts_t* data, ContextInfo*
		      contextInfo, int function) {

    vtpTrunkSerialNo++;

    if (vtpTrunkSerialNo == 0xF0000000) {
      vtpTrunkSerialNo = 0;
    }

    return NO_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_vlanTrunkPorts_UNDO
/* add #define SR_vlanTrunkPorts_UNDO in vtpmib.h to
 * include the undo routine for the vlanTrunkPorts family.
 */
int
vlanTrunkPorts_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_vlanTrunkPorts_UNDO */
#endif /* SR_SNMPv2 */

vlanTrunkPortEntry_t *
k_vlanTrunkPortEntry_get (int serialNum, ContextInfo *contextInfo, int
			  nominator, int searchType, long
			  vlanTrunkPortIfIndex) {
   static vlanTrunkPortEntry_t vlanTrunkPortEntryData; 
   static OctetString vlansEnabled_os, vlansActive_os;
   static OctetString vlansRequested_os, vlansEligible_os;
   vtp_trunk_info info, next;
   vtp_domain_info d_info;
   long vtpIfIndex, nextIfIndex;
   unsigned char error;
   vtp_trunk_stat trunk_stats;
   static unsigned char XmitJoined[VLANS_BITMASK_SIZE];
   static unsigned char RcvJoined[VLANS_BITMASK_SIZE];
   static unsigned char PruningEligible[VLANS_BITMASK_SIZE];
   
   if (vtp_get_trunk_info(&info, VTP_FIRST))
       return NULL;

   error = 0;
   nextIfIndex = 0;
   while (!error) {
       vtpIfIndex = vtp_dep_get_ifIndex_from_trunk(info.trunk_id);

       if (!vtpIfIndex) {
	   error = vtp_get_trunk_info(&info, VTP_NEXT);
	   continue;
       }
       if (vtpIfIndex == vlanTrunkPortIfIndex)
	   break;
       if (vtpIfIndex > vlanTrunkPortIfIndex)
	   if ((!nextIfIndex) ||(nextIfIndex > vtpIfIndex)) {
	       nextIfIndex = vtpIfIndex;
	       memcpy(&next, &info, sizeof(vtp_trunk_info));
	   }
       error = vtp_get_trunk_info(&info, VTP_NEXT);
   }
   
   if (error)
       if (searchType == EXACT)
	   return NULL;
       else
	   if (nextIfIndex) {
	       memcpy(&info, &next, sizeof(vtp_trunk_info));
	       vtpIfIndex = nextIfIndex;
	   } else
	       return NULL;

   memcpy(d_info.domain_name, info.domain_name, info.dname_length);
   d_info.domain_name_length = info.dname_length;
   if (vtp_get_domain_info(&d_info, VTP_EXACT))
       return NULL;

   vlanTrunkPortEntryData.vlanTrunkPortIfIndex = vtpIfIndex;
   vlanTrunkPortEntryData.vlanTrunkPortManagementDomain =
       d_info.index; 
   vlanTrunkPortEntryData.vlanTrunkPortEncapsulationType =
       info.encaps_type;

   vlansEnabled_os.octet_ptr = info.vlans_enabled;
   vlansEnabled_os.length = VLANS_BITMASK_SIZE;
   vlanTrunkPortEntryData.vlanTrunkPortVlansEnabled = &vlansEnabled_os;
   vlanTrunkPortEntryData.vlanTrunkPortNativeVlan =
       info.native_vlan;
   vlanTrunkPortEntryData.vlanTrunkPortRowStatus =
       D_vlanTrunkPortRowStatus_active;
   if (!vtp_core_get_trunk_statistics(d_info.domain_name_length,
				      d_info.domain_name, 
				      info.trunk_id, &trunk_stats))
       return NULL;
   vlanTrunkPortEntryData.vlanTrunkPortInJoins = trunk_stats.rx_join;
   vlanTrunkPortEntryData.vlanTrunkPortOutJoins = trunk_stats.tx_join;
   vlanTrunkPortEntryData.vlanTrunkPortOldAdverts =
       trunk_stats.rx_no_pruning_support_msg;

   if (!vtp_core_get_trunk_eligibility(info.trunk_id,
				       PruningEligible))
       return NULL;
   vlansEligible_os.octet_ptr = PruningEligible;
   vlansEligible_os.length = VLANS_BITMASK_SIZE;
   vlanTrunkPortEntryData.vlanTrunkPortVlansPruningEligible =
       &vlansEligible_os;

   if (!vtp_core_get_trunk_pruning_status(d_info.domain_name_length,
				     d_info.domain_name,
				     info.trunk_id, XmitJoined)) 
       return NULL;

   vlansActive_os.octet_ptr = XmitJoined;
   vlansActive_os.length = VLANS_BITMASK_SIZE;
   vlanTrunkPortEntryData.vlanTrunkPortVlansXmitJoined =
       &vlansActive_os;

   if (!vtp_core_get_trunk_joining_status(d_info.domain_name_length,
					  info.domain_name,
					  info.trunk_id, RcvJoined)) 
       return NULL;

   vlansRequested_os.octet_ptr = RcvJoined;
   vlansRequested_os.length = VLANS_BITMASK_SIZE;
   vlanTrunkPortEntryData.vlanTrunkPortVlansRcvJoined = &vlansRequested_os;
   return(&vlanTrunkPortEntryData);
}

#ifdef SETS
int
k_vlanTrunkPortEntry_test (ObjectInfo *object, ObjectSyntax *value,
			   doList_t *dp, ContextInfo *contextInfo) {
    vlanTrunkPortEntry_t* trunkEntry;
    vtp_domain_info d_info;
    vtp_trunk_info t_info;

    trunkEntry = (vlanTrunkPortEntry_t*)(dp->data);
    t_info.trunk_id =
	vtp_dep_get_trunk_from_ifIndex(trunkEntry->vlanTrunkPortIfIndex);
    if (!t_info.trunk_id)
	return (NO_CREATION_ERROR);

    switch (object->nominator) {
    case I_vlanTrunkPortManagementDomain:
	d_info.index = value->sl_value;
	if (k_get_vtp_domain(&d_info, EXACT))
	    return INCONSISTENT_VALUE_ERROR;
	break;
    case I_vlanTrunkPortEncapsulationType:
	return(vtp_dep_validate_vlanTrunkPortEncaps(trunkEntry->vlanTrunkPortIfIndex, 
						    trunkEntry->vlanTrunkPortEncapsulationType));
    case I_vlanTrunkPortVlansEnabled:
	if (value->os_value->length != VLANS_BITMASK_SIZE)
	    return WRONG_LENGTH_ERROR;
	break;
    case I_vlanTrunkPortNativeVlan:
	vtp_get_trunk_info(&t_info, VTP_EXACT);
	if (t_info.encaps_type != D_vlanTrunkPortEncapsulationType_dot10)
	    return INCONSISTENT_VALUE_ERROR;
	break;
    case I_vlanTrunkPortRowStatus:
	if (!vtp_dep_validate_vlanTrunkPortRowStatus(value->sl_value))
	    return INCONSISTENT_VALUE_ERROR;
	break;
    }
    return NO_ERROR;
}

int
k_vlanTrunkPortEntry_ready (ObjectInfo *object, ObjectSyntax *value,
			    doList_t *doHead, doList_t *dp) {
    vlanTrunkPortEntry_t *new_info, *old_info;
    vtp_domain_info d_info;
    vtp_vlan_info v_info;

    new_info = (vlanTrunkPortEntry_t*)(dp->data);
    if (VALID(I_vlanTrunkPortNativeVlan, new_info->valid)) {

	/* if new management domain is being set for this trunk, make
	   sure that the native VLAN is valid within it's scope */

	if (VALID(I_vlanTrunkPortManagementDomain, new_info->valid)) 
	    d_info.index = new_info->vlanTrunkPortManagementDomain;
	else {
	    old_info = 
		k_vlanTrunkPortEntry_get(-1, NULL,
					 I_vlanTrunkPortManagementDomain, 
					 EXACT,
					 new_info->vlanTrunkPortIfIndex);
	    d_info.index = old_info->vlanTrunkPortManagementDomain;
	}
	k_get_vtp_domain(&d_info, EXACT);
	v_info.isl_vlan_id = new_info->vlanTrunkPortNativeVlan;
	if (vtp_get_vlan_info(d_info.domain_name_length,
			      d_info.domain_name, &v_info, EXACT)) {
	    dp->state = UNKNOWN;
	    return NO_ERROR;
	}
    }
    if ((new_info->vlanTrunkPortRowStatus ==
	 D_vlanTrunkPortRowStatus_createAndGo) &&
	(!VALID(I_vlanTrunkPortManagementDomain, new_info->valid))) {
	dp->state = UNUSED;
	return NO_ERROR;
    }

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_vlanTrunkPortEntry_set_defaults (doList_t *dp) {

    return NO_ERROR;
}

int
k_vlanTrunkPortEntry_set (vlanTrunkPortEntry_t *data, ContextInfo
			 *contextInfo, int function) {

    vtp_trunk_info t_info;
    vtp_domain_info d_info;
    unsigned char error;

    t_info.trunk_id =
	vtp_dep_get_trunk_from_ifIndex(data->vlanTrunkPortIfIndex); 
    d_info.index = data->vlanTrunkPortManagementDomain;
    error = k_get_vtp_domain(&d_info, EXACT); 

    if ((error) || (!t_info.trunk_id))
	return GEN_ERROR; /* How did that happen? */

    t_info.dname_length = d_info.domain_name_length;
    memcpy(t_info.domain_name, d_info.domain_name,
	   d_info.domain_name_length);
    memcpy(t_info.vlans_enabled,
	   data->vlanTrunkPortVlansEnabled->octet_ptr,
	   VLANS_BITMASK_SIZE);
    t_info.native_vlan = data->vlanTrunkPortNativeVlan;
    t_info.encaps_type = data->vlanTrunkPortEncapsulationType;
    switch (data->vlanTrunkPortRowStatus) {
    case D_vlanTrunkPortRowStatus_active:
	vtp_post_configure_trunk(VTP_MODIFY, &t_info);
	if (VALID(I_vlanTrunkPortVlansPruningEligible, data->valid)) {
	    vtp_post_change_vlan_pruning_eligible(t_info.trunk_id,
						  data->vlanTrunkPortVlansPruningEligible->octet_ptr);
	}
	break;
    case D_vlanTrunkPortRowStatus_createAndGo:
	vtp_post_configure_trunk(VTP_ADD, &t_info);
	if (VALID(I_vlanTrunkPortVlansPruningEligible, data->valid)) {
	    vtp_post_change_vlan_pruning_eligible(t_info.trunk_id,
						  data->vlanTrunkPortVlansPruningEligible->octet_ptr); 
	}
	break;
    case D_vlanTrunkPortRowStatus_destroy:
	vtp_post_configure_trunk(VTP_DELETE, &t_info);
    }
    return NO_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_vlanTrunkPortEntry_UNDO
/* add #define SR_vlanTrunkPortEntry_UNDO in vtpmib.h to
 * include the undo routine for the vlanTrunkPortEntry family.
 */
int
vlanTrunkPortEntry_undo (doList_t *doHead, doList_t *doCur,
			 ContextInfo *contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_vlanTrunkPortEntry_UNDO */
#endif /* SR_SNMPv2 */

/* Trap functions */
void vtp_revnum_error_detected (unsigned char* domain_name, unsigned
				char dname_length) {
    vtp_domain_info info;
    vtpEditDomainType* e_domain;
    OID* enterprise, *RevNumber, *instance;
    OID* varbinds[2];
    
    if (!domain_name)
	return;
    
    memcpy(info.domain_name, domain_name, dname_length);
    info.domain_name_length = dname_length;
    if (vtp_get_domain_info(&info, VTP_EXACT))
	return;

    if (vtp_dep_get_notifications_enabled() ==
	D_vtpNotificationsEnabled_false) 
	return;

    e_domain = vtp_edit_get_domain_info(EXACT, info.index);
    if (!e_domain)
	return;

    /* Interesting (mildly) event that can occur is when sysUpTime
     * happens to be exactly zero when a trap is sent.  This sets the
     * last_trap timestamp to zero which will allow one extra trap to
     * be sent (since it matches the initial condition check.  So we
     * have to live with one extra trap every year and a half (worse
     * case). 
     */

    if ((e_domain->last_rev_num_trap) && ((snmp_sysUpTime() -
					   e_domain->last_rev_num_trap) < 
					  VTP_TRAP_INTERVAL))
	return; /* throttled back */

    RevNumber =	MakeOID((unsigned long*)IDmanagementDomainConfigRevNumber,
			LNmanagementDomainConfigRevNumber);  
    if (!RevNumber) 
	return;

    instance = MakeOID((ulong*)&info.index, 1);
    if (!instance) {
	FreeOID(RevNumber);
	return;
    }

    varbinds[0] = CatOID(RevNumber, instance);
    if (!varbinds[0]) {
	FreeOID(RevNumber);
	FreeOID(instance);
	return;
    }
    FreeOID(RevNumber);
    FreeOID(instance);

    enterprise = MakeOID(IDvtpNotifications, LNvtpNotifications);
    if (!enterprise) {
	FreeOID(varbinds[0]);
	return;
    }
    varbinds[1] = NULL;

    snmp_trap(ENTERPRISE_TRAP, D_vtpConfigRevNumberError, (OID**)&varbinds,
	      enterprise, vtpConfigRevNumberError_str);

    FreeOID(enterprise);
    FreeOID(varbinds[0]);

    e_domain->last_rev_num_trap = snmp_sysUpTime();
}

void vtp_digest_error_detected(unsigned char* domain_name, unsigned
			       char dname_length) {
    vtp_domain_info info;
    vtpEditDomainType* e_domain;
    OID* enterprise, *RevNumber, *instance;
    OID* varbinds[2];

    enterprise = NULL;
    instance = NULL;

    if (!domain_name)
	return;

    memcpy(info.domain_name, domain_name, dname_length);
    info.domain_name_length = dname_length;
    if (vtp_get_domain_info(&info, VTP_EXACT))
	return;

    if (vtp_dep_get_notifications_enabled() ==
	D_vtpNotificationsEnabled_false) 
	return;

    e_domain = vtp_edit_get_domain_info(EXACT, info.index);
    if (!e_domain)
	return;

    if ((e_domain->last_rev_num_trap) && ((snmp_sysUpTime() -
					   e_domain->last_digest_trap)
					  < VTP_TRAP_INTERVAL)) 
	return; /* throttled back */

    RevNumber = MakeOID((unsigned long*)IDmanagementDomainConfigRevNumber,
			LNmanagementDomainConfigRevNumber);
    if (!RevNumber)
	return;

    instance = MakeOID((ulong*)&info.index, 1);
    if (!instance) {
	FreeOID(RevNumber);
	return;
    }

    varbinds[0] = CatOID(RevNumber, instance);
    if (!varbinds[0]) {
	FreeOID(RevNumber);
	FreeOID(instance);
	return;
    }
    FreeOID(RevNumber);
    FreeOID(instance);

    enterprise = MakeOID(IDvtpNotifications, LNvtpNotifications);
    if (!enterprise) {
	FreeOID(varbinds[0]);
	return;
    }
    
    varbinds[1] = NULL;
    snmp_trap(ENTERPRISE_TRAP, D_vtpConfigDigestError, (OID**)&varbinds,
	      enterprise, vtpConfigDigestError_str);

    FreeOID(enterprise);
    FreeOID(varbinds[0]);
    e_domain->last_digest_trap = snmp_sysUpTime();
}

void vtp_mtu_too_big (vtp_domain_info* dom_info, vtp_vlan_info*
		      vlan_info, vtp_trunk_info* trunk_info) {
    OID* enterprise, *prefix, *instance;
    OID* varbinds[3];
    unsigned long ifIndex;

    enterprise = NULL;
    instance = NULL;

    if ((!dom_info) || (!vlan_info))
	return;

    if (vtp_dep_get_notifications_enabled() ==
	D_vtpNotificationsEnabled_false) 
	return;

    prefix = MakeOID((unsigned long*)IDvlanTrunkPortManagementDomain,
		     LNvlanTrunkPortManagementDomain);
    if (!prefix) 
	return;

    ifIndex = vtp_dep_get_ifIndex_from_trunk(trunk_info->trunk_id);
    instance = MakeOID(&ifIndex, 1);
    if (!instance) {
	FreeOID(prefix);
	return;
    }

    varbinds[0] = CatOID(prefix, instance);
    if (!varbinds[0]) {
	FreeOID(prefix);
	FreeOID(instance);
	return;
    }

    FreeOID(prefix);
    FreeOID(instance);

    prefix = MakeOID((unsigned long*)IDvtpVlanState, LNvtpVlanState);
    if (!prefix) {
	FreeOID(varbinds[0]);
	return;
    }
    
    instance = MakeOID((ulong*)&vlan_info->state, 1);
    if (!instance) {
	FreeOID(varbinds[0]);
	FreeOID(prefix);
	return;
    }
    
    varbinds[1] = CatOID(prefix, instance);
    if (!varbinds[1]) {
	FreeOID(varbinds[0]);
	FreeOID(prefix);
	FreeOID(instance);
	return;
    }

    FreeOID(prefix);
    FreeOID(instance);

    varbinds[2] = NULL;

    enterprise = MakeOID(IDvtpNotifications, LNvtpNotifications);
    if (!enterprise) {
	FreeOID(varbinds[0]);
	FreeOID(varbinds[1]);
	return;
    }
    
    snmp_trap(ENTERPRISE_TRAP, D_vtpConfigDigestError, (OID**)&varbinds,
	      enterprise, vtpConfigDigestError_str);

    FreeOID(enterprise);
    FreeOID(varbinds[0]);
    FreeOID(varbinds[1]);
}

void vtp_storage_exceeded (unsigned char* domain_name, unsigned char
			   dname_length) {
    vtp_device_info dev_info;
    vtp_domain_info dom_info;
    vtpEditDomainType* e_domain;
    OID* enterprise, *prefix, *instance;
    OID* varbinds[3];

    if (!domain_name)
	return;
    memcpy(dom_info.domain_name, domain_name, dname_length);
    dom_info.domain_name_length = dname_length;
    if (vtp_get_domain_info(&dom_info, VTP_EXACT))
	return;

    vtp_get_device_info(&dev_info);

    if (vtp_dep_get_notifications_enabled() ==
	D_vtpNotificationsEnabled_false) 
	return;

    e_domain = vtp_edit_get_domain_info(EXACT, dom_info.index);
    if (!e_domain)
	return;

    prefix = MakeOID((unsigned long*)IDmanagementDomainConfigRevNumber,
		     LNmanagementDomainConfigRevNumber);
    if (!prefix) 
	return;

    instance = MakeOID((ulong*)&dom_info.index, 1);
    if (!instance) {
	FreeOID(prefix);
	return;
    }

    varbinds[0] = CatOID(prefix, instance);
    FreeOID(prefix);
    FreeOID(instance);
    if (!varbinds[0]) 
	return;

    prefix = MakeOID((unsigned long*)IDvtpMaxVlanStorage, LNvtpMaxVlanStorage);
    if (!prefix) {
	FreeOID(varbinds[0]);
	return;
    }

    varbinds[1] = CatOID(prefix, &ZeroOid);
    FreeOID(prefix);
    if (!varbinds[1]) {
	FreeOID(varbinds[0]);
	return;
    }
    
    enterprise = MakeOID(IDvtpNotifications, LNvtpNotifications);
    if (!enterprise) {
	FreeOID(varbinds[0]);
	FreeOID(varbinds[1]);
	return;
    }

    varbinds[2] = NULL;

    snmp_trap(ENTERPRISE_TRAP, D_vtpConfigDigestError, (OID**)&varbinds,
	      enterprise, vtpConfigDigestError_str);

    FreeOID(enterprise);
    FreeOID(varbinds[0]);
    FreeOID(varbinds[1]);
}

#ifdef IOS_SPECIFIC


LINK_EXIT(cfg_snmp_host_vtp_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_vtp, cfg_snmp_host_vtp_exit, NONE, 
	   OBJ(int, 1), (1<<TRAP_VTP), "vtp", "Allow SNMP VTP traps",
	   PRIV_CONF);
LINK_POINT(cfg_snmp_host_vtp_entry, cfg_snmp_host_vtp);

LINK_EXIT(cfg_snmp_enable_vtp_exit, no_alt);

KEYWORD_OR(cfg_snmp_enable_vtp, cfg_snmp_enable_vtp_exit, NONE,
	   OBJ(int, 1), (1<<TRAP_VTP), "vtp", 
	   "Enable SNMP VTP traps", PRIV_CONF);
LINK_POINT(cfg_snmp_enable_vtp_entry, cfg_snmp_enable_vtp);

/*
 * Parse chain registration array for VTP MIB
 */
static parser_extension_request vtpmib_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_vtp_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT, (dynamic_transition *)
	  &pname(cfg_snmp_host_vtp_exit) },  
    { PARSE_ADD_CFG_SNMP_ENABLE_CMD, &pname(cfg_snmp_enable_vtp_entry) },
    { PARSE_ADD_CFG_SNMP_ENABLE_EXIT, (dynamic_transition *)
	  &pname(cfg_snmp_enable_vtp_exit) },
    { PARSE_LIST_END, NULL }
};

void vtp_snmp_host_nvgen (ulong flags) {
    nv_add(flags & (1<<TRAP_VTP), " vtp");
}

void vtp_trap_cfg_set (vtp_boolean enable, uint subsys, uint trap) {
    if (trap & (1 << TRAP_VTP)) 
	vtp_dep_notification_enabled(enable ?
				     D_vtpNotificationsEnabled_true :
				     D_vtpNotificationsEnabled_false);
}

void vtp_trap_cfg_nvwrt (parseinfo* csb) {
    nv_write(vtp_dep_get_notifications_enabled() ==
	D_vtpNotificationsEnabled_true, "%s traps vtp",
	csb->nv_command);
}

void
init_vtpmib (subsystype *subsys)
{
  vtpTrunkSerialNo = vtp_get_random_number();
  load_mib(vtpmib_OidList, vtpmib_OidListNum);
  load_oid(vtpmib_oid_table);
  
  vtp_dep_add_trap_functions();
  
  reg_add_snmp_host_nvgen(vtp_snmp_host_nvgen,
			  "vtp_snmp_host_nvgen");
  reg_add_Trap_cfg_set(vtp_trap_cfg_set, "vtp_trap_cfg_set");
  reg_add_Trap_cfg_nvwrt(vtp_trap_cfg_nvwrt, "vtp_trap_cfg_nvwrt");
  
  parser_add_command_list(vtpmib_chain_init_table, "VTP MIB");
  register_snmp_trap(TRAP_VTP, vtpConfigRevNumberError_str);
  register_snmp_trap(TRAP_VTP, vtpConfigDigestError_str);
  register_snmp_trap(TRAP_VTP, vtpServerDisabled_str);
  register_snmp_trap(TRAP_VTP, vtpMtuTooBig_str);
}

/*
 * vtpmib subsystem header
 */

#define MAJVERSION_vtpmib 1
#define MINVERSION_vtpmib 0
#define EDITVERSION_vtpmib 0

SUBSYS_HEADER(vtpmib,
              MAJVERSION_vtpmib,
              MINVERSION_vtpmib,
              EDITVERSION_vtpmib,
              init_vtpmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: vtp",
              NULL);
#endif IOS_SPECIFIC
