/* $Id: sr_lecsmib.c,v 3.8.8.9 1996/08/26 19:46:27 cakyol Exp $
 * $Source: /release/112/cvs/Xsys/lane/sr_lecsmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1995, Christopher Young (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lecsmib.c,v $
 * Revision 3.8.8.9  1996/08/26  19:46:27  cakyol
 * CSCdi66421:  the well known LECS address needs to follow the master LECS
 * Branch: California_branch
 *
 * Revision 3.8.8.8  1996/07/01  18:45:28  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.8.7  1996/06/11  19:22:22  kjadams
 * CSCdi58289:  snmp management of TRLANE specific fields in the LECS mib
 * Branch: California_branch
 * Added the code to return/assign values for TR LANE specific fields which
 * exist in the LECS mib.
 *
 * Revision 3.8.8.6  1996/05/29  21:39:46  cyoung
 * hyphens removed
 * CSCdi58985:  LECS MIB changes use hyphen in enumerations
 * Branch: California_branch
 *
 * Revision 3.8.8.5  1996/05/21  22:04:07  cyoung
 * Add TR LANE objects to LECS MIB.  Also add compliance groups (including
 * redundancy objects).  Access functions to be filled in.
 * CSCdi58289:  snmp management of TRLANE specific fields in the LECS mib
 * Branch: California_branch
 *
 * Revision 3.8.8.4  1996/05/21  09:54:30  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.8.3  1996/05/21  06:34:14  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.8.8.2  1996/05/09  14:33:38  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.8.1.2.2  1996/05/01  23:45:47  cyoung
 * Get next traversal broken with certain values of atm addr mask.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.8.1.2.1  1996/04/27  07:07:24  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.8.1  1996/04/25  23:15:21  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.8.24.2  1996/04/16  02:45:31  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.8.24.1  1996/04/08  01:57:47  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.8.14.2  1996/03/30  04:13:16  cyoung
 * Add objects for LECS/LES redundancy
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8.14.1  1996/03/22  22:55:57  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8  1996/02/28  21:08:14  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.7.12.1  1996/03/05  06:43:58  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.7.6.1  1996/02/27  21:11:14  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7  1996/01/24  21:18:20  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.6  1996/01/22  06:35:59  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/18  15:50:44  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1996/01/17  23:12:08  cyoung
 * Removed bogus return statement and properly set length in
 * k_lecsAtmAddrConfigEntry_get ATM address
 * CSCdi47068:  lecsAtmAddrConfigTable sets not working
 *
 * Revision 3.3.14.5  1996/02/22  09:05:53  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.4  1996/01/30  23:56:44  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.3  1996/01/29  22:06:38  cakyol
 * - as per the signalling group's request, eliminate the
 *   direct invasion of their private data structures by
 *   using:
 *     atmSig_api_callId2CallingPartyAddress   and
 *     atmSig_api_sameCallingPartyAddress
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.2  1996/01/26  02:03:04  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.14.1  1996/01/11  01:47:10  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:07:22  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:52:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:22  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:24:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:58  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/13  03:05:29  cyoung
 * place holder file for LANE Service MIB access routines.
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

#include "master.h"
#include "../ui/common_strings.h"
#include "../snmp/snmp_api.h"
#include "sr_lecsmib.h"
#include "sr_lecsmib-mib.h"
#include "lecs.h"
#include "parser_defs_lane.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"
#include "subsys.h"
#include "address.h"
#include "generic_objects.h"

#define CONFIG_IMPLIED 0
#define CONFIG_VARIABLE 1

/*
 * Start of system-independent routines
 */

enum {
    TYPE_SVC = 1
};

static char* convert_to_null_terminated_string( OctetString* os);
static ConfigTable* getConfigTableBySNMPName(OctetString* os);
static ElanInfoBlock* getElanBySNMPName(ConfigTable* pCfg, OctetString* os);
static LecsInfo* LecsInfo_searchIfIndex(long index);
static unsigned char valid_configtab(OctetString* os);

/* This function takes an OctetString and scans for any <NUL> characters.
   If found TRUE(1) is returned, otherwise FALSE(0)
   */

static unsigned char contains_null (OctetString *string)
{
    int offset;
    if (!string)
	return FALSE;
    
    for (offset = 0; offset < string->length; offset++)
	if (string->octet_ptr[offset] == '\0')
	    return(TRUE);
    
    return(FALSE);
}

/*---------------------------------------------------------------------
 * Retrieve data from the lecsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsEntry_get (OID *incoming, ObjectInfo* object, int searchType,
	       ContextInfo* contextInfo, int serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    lecsEntry_t     *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ifIndex;
    
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;
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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex,
		    searchType, &carry)) < 0 ) {
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecsEntry_get(serialNum,
						contextInfo, arg
						,searchType, ifIndex))
	== NULL) {
        arg = -1;
    }
    
    else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ifIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsConfigTableName
    case I_lecsConfigTableName:
	dp = (void *) (CloneOctetString(data->lecsConfigTableName));
	break;
#endif /* I_lecsConfigTableName */
	
#ifdef I_lecsUpTime
    case I_lecsUpTime:
	dp = (void *) (&data->lecsUpTime);
	break;
#endif /* I_lecsUpTime */
	
#ifdef I_lecsInConfigReqs
    case I_lecsInConfigReqs:
	dp = (void *) (&data->lecsInConfigReqs);
	break;
#endif /* I_lecsInConfigReqs */
	
#ifdef I_lecsInConfigErrors
    case I_lecsInConfigErrors:
	dp = (void *) (&data->lecsInConfigErrors);
	break;
#endif /* I_lecsInConfigErrors */
	
#ifdef I_lecsOutConfigFails
    case I_lecsOutConfigFails:
	dp = (void *) (&data->lecsOutConfigFails);
	break;
#endif /* I_lecsOutConfigFails */
	
#ifdef I_lecsLastFailCause
    case I_lecsLastFailCause:
	dp = (void *) (&data->lecsLastFailCause);
	break;
#endif /* I_lecsLastFailCause */
	
#ifdef I_lecsLastFailLec
    case I_lecsLastFailLec:
	dp = (void *) (CloneOctetString(data->lecsLastFailLec));
	break;
#endif /* I_lecsLastFailLec */
	
#ifdef I_lecsOperStatus
    case I_lecsOperStatus:
	dp = (void *) (&data->lecsOperStatus);
	break;
#endif /* I_lecsOperStatus */
	
#ifdef I_lecsAdminStatus
    case I_lecsAdminStatus:
	dp = (void *) (&data->lecsAdminStatus);
	break;
#endif /* I_lecsAdminStatus */
	
#ifdef I_lecsStatus
    case I_lecsStatus:
	dp = (void *) (&data->lecsStatus);
	break;
#endif /* I_lecsStatus */

#ifdef I_lecsMasterState
    case I_lecsMasterState:
	dp = (void *) (&data->lecsMasterState);
	break;
#endif /* I_lecsMasterState */	

    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsEntry data object.
 *---------------------------------------------------------------------*/
void
lecsEntry_free (lecsEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lecsConfigTableName);
	FreeOctetString(data->lecsLastFailLec);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecsEntry_cleanup (doList_t *trash)
{
    lecsEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecsEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsEntry family
 *---------------------------------------------------------------------*/
lecsEntry_t *
Clone_lecsEntry (lecsEntry_t* lecsEntry)
{
    lecsEntry_t *data;
    
    if ((data = malloc(sizeof(lecsEntry_t))) == NULL)
	{ 
	    return NULL;
	}
    memcpy((char *) (data), (char *) (lecsEntry),
	   sizeof(lecsEntry_t));
    
    if (lecsEntry->lecsConfigTableName)
	data->lecsConfigTableName =
	    CloneOctetString(lecsEntry->lecsConfigTableName); 
    data->lecsLastFailLec =
	CloneOctetString(lecsEntry->lecsLastFailLec); 
    
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
lecsEntry_test (OID* incoming, ObjectInfo* object, ObjectSyntax*
		value, doList_t* doHead, doList_t* doCur, ContextInfo*
		contextInfo) { 
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;     
    lecsEntry_t    *lecsEntry;
    long           ifIndex;
    
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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT,
		    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    lecsEntry = k_lecsEntry_get(-1, contextInfo, -1, EXACT, ifIndex);
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsEntry_set) &&
            (((lecsEntry_t *) (dp->data)) != NULL) &&
            (((lecsEntry_t *) (dp->data))->ifIndex == ifIndex) ) {
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsEntry_set;
        dp->cleanupMethod = lecsEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecsEntry_UNDO
        dp->undoMethod = lecsEntry_undo;
#else /* SR_lecsEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecsEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_lecsEntry(lecsEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_lecsEntry(lecsEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        }
        else {
            if ( (dp->data = malloc(sizeof(lecsEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lecsEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecsEntry_t *) (dp->data))->ifIndex = (ifIndex);
		
		/* ifIndex is not part of the lecsEntry valid bitmask,
		   setting it to valid is incorrect and conflicts with
		   the I_lecsConfigTableName bit 
		   
		   -- SET_VALID(I_ifIndex, ((lecsEntry_t *)
		   
		   (dp->data))->valid); */ 
		
                error_status = k_lecsEntry_set_defaults(dp);
            }
        }
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsConfigTableName
    case I_lecsConfigTableName:
	
	if (((lecsEntry_t *) (dp->data))->lecsConfigTableName != NULL) {
	    FreeOctetString(((lecsEntry_t *) (dp->data))->lecsConfigTableName);
	}
	
	((lecsEntry_t *) (dp->data))->lecsConfigTableName = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsConfigTableName */
	
#ifdef I_lecsAdminStatus
    case I_lecsAdminStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecsEntry_t *) (dp->data))->lecsAdminStatus = value->sl_value;
	break;
#endif /* I_lecsAdminStatus */
	
#ifdef I_lecsStatus
    case I_lecsStatus:
	
     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }
	
	if (value->sl_value == D_lecsStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecsStatus_notInService) ||
	       (value->sl_value == D_lecsStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecsStatus_createAndGo) ||
		(value->sl_value == D_lecsStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecsEntry_t *) (dp->data))->lecsStatus = value->sl_value;
	break;
#endif /* I_lecsStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecsEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecsEntry_test */
    error_status = k_lecsEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsEntry_t *) (dp->data))->valid);
        error_status = k_lecsEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecsEntry_set (doList_t* doHead, doList_t* doCur, ContextInfo*
	       contextInfo) {
    return (k_lecsEntry_set((lecsEntry_t *) (doCur->data),
			    contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecsAtmAddrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsAtmAddrEntry_get (OID* incoming, ObjectInfo* object, int
		      searchType, ContextInfo* contextInfo, int
		      serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    lecsAtmAddrEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            lecsAtmAddrIndex;

    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
    data = NULL;
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
		    &lecsAtmAddrIndex, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_lecsAtmAddrEntry_get(serialNum,
							contextInfo,
							arg
							,searchType,
							ifIndex,
							lecsAtmAddrIndex)) == NULL) { 
	 arg = -1;
     } else {
	 /*
	  * Build instance information
	  */
	 inst.oid_ptr = buffer;
	 inst.length = 2;
	 inst.oid_ptr[0] = data->ifIndex;
	 inst.oid_ptr[1] = data->lecsAtmAddrIndex;
     }

    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsAtmAddrSpec
    case I_lecsAtmAddrSpec:
	dp = (void *) (CloneOctetString(data->lecsAtmAddrSpec));
	break;
#endif /* I_lecsAtmAddrSpec */
	
#ifdef I_lecsAtmAddrMask
    case I_lecsAtmAddrMask:
	dp = (void *) (CloneOctetString(data->lecsAtmAddrMask));
	break;
#endif /* I_lecsAtmAddrMask */
	
#ifdef I_lecsAtmAddrActual
    case I_lecsAtmAddrActual:
	dp = (void *) (CloneOctetString(data->lecsAtmAddrActual));
	break;
#endif /* I_lecsAtmAddrActual */

#ifdef I_lecsAtmAddrState
    case I_lecsAtmAddrState:
	dp = (void *) (&data->lecsAtmAddrState);
	break;
#endif /* I_lecsAtmAddrState */
	
#ifdef I_lecsAtmAddrStatus
    case I_lecsAtmAddrStatus:
	dp = (void *) (&data->lecsAtmAddrStatus);
	break;
#endif /* I_lecsAtmAddrStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsAtmAddrEntry data object.
 *---------------------------------------------------------------------*/
void
lecsAtmAddrEntry_free (lecsAtmAddrEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lecsAtmAddrSpec);
	FreeOctetString(data->lecsAtmAddrMask);
	FreeOctetString(data->lecsAtmAddrActual);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsAtmAddrEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecsAtmAddrEntry_cleanup (doList_t* trash)
{
    lecsAtmAddrEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecsAtmAddrEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsAtmAddrEntry family
 *---------------------------------------------------------------------*/
lecsAtmAddrEntry_t *
Clone_lecsAtmAddrEntry (lecsAtmAddrEntry_t* lecsAtmAddrEntry)
{
    lecsAtmAddrEntry_t *data;
    
    if ((data = malloc(sizeof(lecsAtmAddrEntry_t))) == NULL) { 
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecsAtmAddrEntry),
	   sizeof(lecsAtmAddrEntry_t)); 
    
    data->lecsAtmAddrSpec =
	CloneOctetString(lecsAtmAddrEntry->lecsAtmAddrSpec); 
    data->lecsAtmAddrMask =
	CloneOctetString(lecsAtmAddrEntry->lecsAtmAddrMask); 
    data->lecsAtmAddrActual =
	CloneOctetString(lecsAtmAddrEntry->lecsAtmAddrActual); 

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
lecsAtmAddrEntry_test (OID *incoming, ObjectInfo* object,
		       ObjectSyntax* value, doList_t* doHead,
		       doList_t* doCur, ContextInfo* contextInfo) {
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lecsAtmAddrEntry_t     *lecsAtmAddrEntry;
    long           ifIndex;
    long           lecsAtmAddrIndex;

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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT,
		    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length,
		    &lecsAtmAddrIndex, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    lecsAtmAddrEntry = k_lecsAtmAddrEntry_get(-1, contextInfo, -1,
					      EXACT, ifIndex,
					      lecsAtmAddrIndex); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsAtmAddrEntry_set) &&
            (((lecsAtmAddrEntry_t *) (dp->data)) != NULL) &&
            (((lecsAtmAddrEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((lecsAtmAddrEntry_t *) (dp->data))->lecsAtmAddrIndex ==
	     lecsAtmAddrIndex) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsAtmAddrEntry_set;
        dp->cleanupMethod = lecsAtmAddrEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecsAtmAddrEntry_UNDO
        dp->undoMethod = lecsAtmAddrEntry_undo;
#else /* SR_lecsAtmAddrEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsAtmAddrEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecsAtmAddrEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecsAtmAddrEntry(lecsAtmAddrEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecsAtmAddrEntry(lecsAtmAddrEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        }
        else {
            if ( (dp->data = malloc(sizeof(lecsAtmAddrEntry_t))) == NULL) {  
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lecsAtmAddrEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecsAtmAddrEntry_t *) (dp->data))->ifIndex =
		    (ifIndex);
		
		/* Do not set valid bit for I_ifIndex 
		   -- SET_VALID(I_ifIndex, ((lecsAtmAddrEntry_t *)
		   (dp->data))->valid); */
		
                ((lecsAtmAddrEntry_t *) (dp->data))->lecsAtmAddrIndex
		    = (lecsAtmAddrIndex); 
                SET_VALID(I_lecsAtmAddrIndex, ((lecsAtmAddrEntry_t *)
					       (dp->data))->valid); 
		
                error_status = k_lecsAtmAddrEntry_set_defaults(dp);
            }
        }
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsAtmAddrSpec
    case I_lecsAtmAddrSpec:
	
	if (((lecsAtmAddrEntry_t *) (dp->data))->lecsAtmAddrSpec != NULL) {
	    FreeOctetString(((lecsAtmAddrEntry_t *)
			     (dp->data))->lecsAtmAddrSpec); 
	}
	
	((lecsAtmAddrEntry_t *) (dp->data))->lecsAtmAddrSpec = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsAtmAddrSpec */
	
#ifdef I_lecsAtmAddrMask
    case I_lecsAtmAddrMask:
	
	if (((lecsAtmAddrEntry_t *) (dp->data))->lecsAtmAddrMask != NULL) {
	    FreeOctetString(((lecsAtmAddrEntry_t *)
			     (dp->data))->lecsAtmAddrMask); 
	}
	
	((lecsAtmAddrEntry_t *) (dp->data))->lecsAtmAddrMask = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsAtmAddrMask */
	
#ifdef I_lecsAtmAddrStatus
    case I_lecsAtmAddrStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecsAtmAddrStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsAtmAddrEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecsAtmAddrStatus_notInService)
	       || (value->sl_value == D_lecsAtmAddrStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecsAtmAddrStatus_createAndGo)
		|| (value->sl_value ==
		    D_lecsAtmAddrStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecsAtmAddrEntry_t *) (dp->data))->lecsAtmAddrStatus =
	    value->sl_value; 
	break;
#endif /* I_lecsAtmAddrStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecsAtmAddrEntry_test)\n")); 
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecsAtmAddrEntry_test */
    error_status = k_lecsAtmAddrEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsAtmAddrEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecsAtmAddrEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecsAtmAddrEntry_set (doList_t* doHead, doList_t* doCur, ContextInfo*
		      contextInfo) {
    return (k_lecsAtmAddrEntry_set((lecsAtmAddrEntry_t *) (doCur->data),
				   contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecsConfigDirectConnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsConfigDirectConnEntry_get (OID* incoming, ObjectInfo* object, int
			       searchType, ContextInfo* contextInfo,
			       int serialNum) {
    int             instLength;
    int             arg;
    void            *dp;
    lecsConfigDirectConnEntry_t *data;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            lecsConfigDirectConnVpi;
    long            lecsConfigDirectConnVci;
    
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;
    
    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 3 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    
    if (searchType == EXACT) {
        if (instLength != 3) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, 2 + object->oid.length,
		    &lecsConfigDirectConnVci, searchType, &carry)) < 0
	) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecsConfigDirectConnVci = MAX(0, lecsConfigDirectConnVci);
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length,
		    &lecsConfigDirectConnVpi, searchType, &carry)) < 0
	) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecsConfigDirectConnVpi = MAX(0, lecsConfigDirectConnVpi);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex,
		    searchType, &carry)) < 0 ) { 
       arg = -1;
   }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data =
			 k_lecsConfigDirectConnEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 ifIndex,
							 lecsConfigDirectConnVpi, lecsConfigDirectConnVci)) == NULL) { 
        arg = -1;
    }
    
    else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->lecsConfigDirectConnVpi;
        inst.oid_ptr[2] = data->lecsConfigDirectConnVci;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsConfigDirectConnVCType
    case I_lecsConfigDirectConnVCType:
	dp = (void *) (&data->lecsConfigDirectConnVCType);
	break;
#endif /* I_lecsConfigDirectConnVCType */
	
#ifdef I_lecsConfigDirectConnSrc
    case I_lecsConfigDirectConnSrc:
	dp = (void *) (CloneOctetString(data->lecsConfigDirectConnSrc));
	break;
#endif /* I_lecsConfigDirectConnSrc */
	
#ifdef I_lecsConfigDirectConnDst
    case I_lecsConfigDirectConnDst:
	dp = (void *) (CloneOctetString(data->lecsConfigDirectConnDst));
	break;
#endif /* I_lecsConfigDirectConnDst */

#ifdef I_lecsConfigDirectDstType
    case I_lecsConfigDirectDstType:
	dp = (void *) (&data->lecsConfigDirectDstType);
	break;
#endif /* I_lecsConfigDirectDstType */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lecsConfigTblEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsConfigTblEntry_get (OID* incoming, ObjectInfo* object, int
			searchType, ContextInfo* contextInfo, int
			serialNum) {
    int             arg;
    void            *dp;
    lecsConfigTblEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lecsConfigTblName;
    int             lecsConfigTblName_offset;
    int             index;
    int             final_index;

    arg = object->nominator;
    data = NULL;
    lecsConfigTblName = NULL;
    lecsConfigTblName_offset = object->oid.length;

    final_index = lecsConfigTblName_offset +
	GetImpliedIndexLength(incoming, lecsConfigTblName_offset); 

    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToImpliedOctetString(incoming, lecsConfigTblName_offset,
				   &lecsConfigTblName, searchType,
				   &carry)) < 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecsConfigTblEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 lecsConfigTblName)) == NULL) { 
        arg = -1;
    }
    
    else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        for(i = 0; i < data->lecsConfigTblName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsConfigTblName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }
    
    FreeOctetString(lecsConfigTblName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsConfigTblDefaultElanName
    case I_lecsConfigTblDefaultElanName:
	dp = (void *) (CloneOctetString(data->lecsConfigTblDefaultElanName));
	break;
#endif /* I_lecsConfigTblDefaultElanName */
	
#ifdef I_lecsConfigTblStatus
    case I_lecsConfigTblStatus:
	dp = (void *) (&data->lecsConfigTblStatus);
	break;
#endif /* I_lecsConfigTblStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsConfigTblEntry data object.
 *---------------------------------------------------------------------*/
void
lecsConfigTblEntry_free (lecsConfigTblEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lecsConfigTblName);
	FreeOctetString(data->lecsConfigTblDefaultElanName);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsConfigTblEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecsConfigTblEntry_cleanup (doList_t* trash)
{
    lecsConfigTblEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecsConfigTblEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsConfigTblEntry family
 *---------------------------------------------------------------------*/
lecsConfigTblEntry_t *
Clone_lecsConfigTblEntry (lecsConfigTblEntry_t *lecsConfigTblEntry)
{
    lecsConfigTblEntry_t *data;
    
    if ((data = malloc(sizeof(lecsConfigTblEntry_t))) == NULL) { 
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecsConfigTblEntry),
	   sizeof(lecsConfigTblEntry_t)); 
    
    data->lecsConfigTblName =
	CloneOctetString(lecsConfigTblEntry->lecsConfigTblName); 
    data->lecsConfigTblDefaultElanName =
	CloneOctetString(lecsConfigTblEntry->lecsConfigTblDefaultElanName);
    
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
lecsConfigTblEntry_test (OID* incoming, ObjectInfo* object,
			 ObjectSyntax* value, doList_t* doHead,
			 doList_t* doCur, ContextInfo* contextInfo) {
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lecsConfigTblEntry_t     *lecsConfigTblEntry;
    OctetString *  lecsConfigTblName;
    int            lecsConfigTblName_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    lecsConfigTblName = NULL;
    /*
     * Validate the object instance
     *
     */
    lecsConfigTblName_offset = object->oid.length;
    final_index = lecsConfigTblName_offset +
	GetImpliedIndexLength(incoming, lecsConfigTblName_offset); 

    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToImpliedOctetString(incoming, lecsConfigTblName_offset,
				   &lecsConfigTblName, EXACT, &carry))
	< 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lecsConfigTblName);
        return error_status;
    }
    
    lecsConfigTblEntry = k_lecsConfigTblEntry_get(-1, contextInfo, -1,
						  EXACT,
						  lecsConfigTblName); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsConfigTblEntry_set) &&
            (((lecsConfigTblEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lecsConfigTblEntry_t *)
			      (dp->data))->lecsConfigTblName,
			     lecsConfigTblName) == 0) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsConfigTblEntry_set;
        dp->cleanupMethod = lecsConfigTblEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecsConfigTblEntry_UNDO
        dp->undoMethod = lecsConfigTblEntry_undo;
#else /* SR_lecsConfigTblEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsConfigTblEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecsConfigTblEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecsConfigTblEntry(lecsConfigTblEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecsConfigTblEntry(lecsConfigTblEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        }
        else {
            if ( (dp->data = malloc(sizeof(lecsConfigTblEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lecsConfigTblEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecsConfigTblEntry_t *)
		 (dp->data))->lecsConfigTblName =
		     CloneOctetString(lecsConfigTblName); 
                SET_VALID(I_lecsConfigTblName, ((lecsConfigTblEntry_t
						 *)
						(dp->data))->valid); 
                error_status = k_lecsConfigTblEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(lecsConfigTblName);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsConfigTblDefaultElanName
    case I_lecsConfigTblDefaultElanName:
	
	if (((lecsConfigTblEntry_t *)
	     (dp->data))->lecsConfigTblDefaultElanName != NULL) { 
	    FreeOctetString(((lecsConfigTblEntry_t *)
			     (dp->data))->lecsConfigTblDefaultElanName); 
	}
	
	((lecsConfigTblEntry_t *) (dp->data))->lecsConfigTblDefaultElanName = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsConfigTblDefaultElanName */
	
#ifdef I_lecsConfigTblStatus
    case I_lecsConfigTblStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecsConfigTblStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsConfigTblEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecsConfigTblStatus_notInService)
	       || (value->sl_value == D_lecsConfigTblStatus_active)) {
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecsConfigTblStatus_createAndGo)
		|| (value->sl_value ==
		    D_lecsConfigTblStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecsConfigTblEntry_t *) (dp->data))->lecsConfigTblStatus =
	    value->sl_value; 
	break;
#endif /* I_lecsConfigTblStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecsConfigTblEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecsConfigTblEntry_test */
    error_status = k_lecsConfigTblEntry_test(object, value, dp,
					     contextInfo); 
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsConfigTblEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecsConfigTblEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecsConfigTblEntry_set (doList_t* doHead, doList_t* doCur,
			ContextInfo* contextInfo) {
    return (k_lecsConfigTblEntry_set((lecsConfigTblEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecsElanConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsElanConfigEntry_get (OID* incoming, ObjectInfo* object, int
			 searchType, ContextInfo* contextInfo, int
			 serialNum) {
    int             arg;
    void            *dp;
    lecsElanConfigEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lecsConfigTblName;
    int             lecsConfigTblName_offset;
    OctetString *   lecsElanConfigName;
    int             lecsElanConfigName_offset;
    int             index;
    int             final_index;
    
    arg = object->nominator;
    data = NULL;
    lecsConfigTblName = NULL;
    lecsElanConfigName = NULL;
    
    lecsConfigTblName_offset = object->oid.length;
    lecsElanConfigName_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset);
    final_index = lecsElanConfigName_offset +
	GetImpliedIndexLength(incoming, lecsElanConfigName_offset); 
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToImpliedOctetString(incoming,
				   lecsElanConfigName_offset,
				   &lecsElanConfigName, searchType,
				   &carry)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || 
	(data = k_lecsElanConfigEntry_get(serialNum, contextInfo, arg,
					  searchType,
					  lecsConfigTblName,
					  lecsElanConfigName)) ==
	NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lecsConfigTblName->length;
        for(i = 0; i < data->lecsConfigTblName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsConfigTblName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->lecsElanConfigName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsElanConfigName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
    }
    
    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsElanConfigName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsElanLesAtmAddr
    case I_lecsElanLesAtmAddr:
	dp = (void *) (CloneOctetString(data->lecsElanLesAtmAddr));
	break;
#endif /* I_lecsElanLesAtmAddr */
	
#ifdef I_lecsElanAccess
    case I_lecsElanAccess:
	dp = (void *) (&data->lecsElanAccess);
	break;
#endif /* I_lecsElanAccess */
	
#ifdef I_lecsElanConfigStatus
    case I_lecsElanConfigStatus:
	dp = (void *) (&data->lecsElanConfigStatus);
	break;
#endif /* I_lecsElanConfigStatus */
	
#ifdef I_lecsElanSegmentId
    case I_lecsElanSegmentId:
       dp = (void *) (&data->lecsElanSegmentId);
       break;
#endif /* I_lecsElanSegmentId */

    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsElanConfigEntry data object.
 *---------------------------------------------------------------------*/
void
lecsElanConfigEntry_free (lecsElanConfigEntry_t* data)
{
    if (data != NULL) {
	FreeOctetString(data->lecsElanConfigName);
	FreeOctetString(data->lecsElanLesAtmAddr);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsElanConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecsElanConfigEntry_cleanup (doList_t *trash)
{
    lecsElanConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecsElanConfigEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsElanConfigEntry family
 *---------------------------------------------------------------------*/
lecsElanConfigEntry_t *
Clone_lecsElanConfigEntry (lecsElanConfigEntry_t *lecsElanConfigEntry)
{
    lecsElanConfigEntry_t *data;
    
    if ((data = malloc(sizeof(lecsElanConfigEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecsElanConfigEntry),
	   sizeof(lecsElanConfigEntry_t)); 
    
    data->lecsElanConfigName =
	CloneOctetString(lecsElanConfigEntry->lecsElanConfigName); 
    data->lecsElanLesAtmAddr =
	CloneOctetString(lecsElanConfigEntry->lecsElanLesAtmAddr); 

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
lecsElanConfigEntry_test (OID* incoming, ObjectInfo* object,
			  ObjectSyntax* value, doList_t* doHead,
			  doList_t* doCur, ContextInfo* contextInfo) {
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry = 0;
    lecsElanConfigEntry_t     *lecsElanConfigEntry;
    OctetString *  lecsConfigTblName;
    int            lecsConfigTblName_offset;
    OctetString *  lecsElanConfigName;
    int            lecsElanConfigName_offset;
    int            final_index;

    error_status = NO_ERROR;
    carry = 0;
    lecsConfigTblName = NULL;
    lecsElanConfigName = NULL;
   /*
    * Validate the object instance
    *
    */
    lecsConfigTblName_offset = object->oid.length;
    lecsElanConfigName_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    final_index = lecsElanConfigName_offset +
	GetImpliedIndexLength(incoming, lecsElanConfigName_offset); 
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, EXACT,
				    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToImpliedOctetString(incoming,
				   lecsElanConfigName_offset,
				   &lecsElanConfigName, EXACT,
				   &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lecsConfigTblName);
        FreeOctetString(lecsElanConfigName);
        return error_status;
    }
    
    lecsElanConfigEntry = k_lecsElanConfigEntry_get(-1, contextInfo,
						    -1, EXACT,
						    lecsConfigTblName,
						    lecsElanConfigName); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsElanConfigEntry_set) &&
            (((lecsElanConfigEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lecsElanConfigEntry_t *)
			      (dp->data))->lecsConfigTblName,
			     lecsConfigTblName) == 0) && 
            (CmpOctetStrings(((lecsElanConfigEntry_t *)
			      (dp->data))->lecsElanConfigName,
			     lecsElanConfigName) == 0) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsElanConfigEntry_set;
        dp->cleanupMethod = lecsElanConfigEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecsElanConfigEntry_UNDO
        dp->undoMethod = lecsElanConfigEntry_undo;
#else /* SR_lecsElanConfigEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsElanConfigEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecsElanConfigEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecsElanConfigEntry(lecsElanConfigEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecsElanConfigEntry(lecsElanConfigEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(lecsElanConfigEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lecsElanConfigEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecsElanConfigEntry_t *)
		 (dp->data))->lecsConfigTblName =
		     CloneOctetString(lecsConfigTblName); 
                SET_VALID(I_lecsConfigTblName, ((lecsElanConfigEntry_t
						 *)
						(dp->data))->valid); 
		
                ((lecsElanConfigEntry_t *)
		 (dp->data))->lecsElanConfigName =
		     CloneOctetString(lecsElanConfigName); 
                SET_VALID(I_lecsElanConfigName,
			  ((lecsElanConfigEntry_t *)
			   (dp->data))->valid); 
		
                error_status = k_lecsElanConfigEntry_set_defaults(dp);
            }
        }
    }

    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsElanConfigName);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsElanLesAtmAddr
    case I_lecsElanLesAtmAddr:
	
	if (((lecsElanConfigEntry_t *) (dp->data))->lecsElanLesAtmAddr
	    != NULL) { 
	    FreeOctetString(((lecsElanConfigEntry_t *)
			     (dp->data))->lecsElanLesAtmAddr); 
	}
	
	((lecsElanConfigEntry_t *) (dp->data))->lecsElanLesAtmAddr = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsElanLesAtmAddr */
	
#ifdef I_lecsElanAccess
    case I_lecsElanAccess:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}

	((lecsElanConfigEntry_t *) (dp->data))->lecsElanAccess =
	    value->sl_value; 
	break;
#endif 
	
#ifdef I_lecsElanConfigStatus
    case I_lecsElanConfigStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecsElanConfigStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsElanConfigEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value ==
		D_lecsElanConfigStatus_notInService) ||
	       (value->sl_value == D_lecsElanConfigStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
        if ((value->sl_value == D_lecsElanConfigStatus_createAndGo) ||
	    (value->sl_value == D_lecsElanConfigStatus_createAndWait))
	    { 
		return INCONSISTENT_VALUE_ERROR;
	    }
    }
	
	((lecsElanConfigEntry_t *) (dp->data))->lecsElanConfigStatus =
	    value->sl_value; 
	break;
#endif /* I_lecsElanConfigStatus */

#ifdef I_lecsElanSegmentId
    case I_lecsElanSegmentId:

	if ( ((value->sl_value < 0) || (value->sl_value > 4095)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecsElanConfigEntry_t *) (dp->data))->lecsElanSegmentId =
	    value->sl_value; 
	break;
#endif /* I_lecsElanSegmentId */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator"
		 " in lecsElanConfigEntry_test)\n")); 
	return GEN_ERROR;
	
   }        /* switch */
    
    /* Do system dependent testing in k_lecsElanConfigEntry_test */
    error_status = k_lecsElanConfigEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsElanConfigEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecsElanConfigEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecsElanConfigEntry_set (doList_t* doHead, doList_t* doCur,
			 ContextInfo* contextInfo) {
    return (k_lecsElanConfigEntry_set((lecsElanConfigEntry_t *) (doCur->data),
				      contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecsMacConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsMacConfigEntry_get (OID* incoming, ObjectInfo* object, int
			searchType, ContextInfo* contextInfo, int
			serialNum) {
    int             arg;
    void            *dp;
    lecsMacConfigEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lecsConfigTblName;
    int             lecsConfigTblName_offset;
    OctetString *   lecsMacConfigAddress;
    int             lecsMacConfigAddress_offset;
    int             index;
    int             final_index;
    
    arg = object->nominator;
    dp = NULL;
    data = NULL;
    lecsConfigTblName = NULL;
    lecsMacConfigAddress = NULL;
    
    lecsConfigTblName_offset = object->oid.length;
    lecsMacConfigAddress_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    final_index = lecsMacConfigAddress_offset + 6;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToFixedOctetString(incoming,
				 lecsMacConfigAddress_offset,
				 &lecsMacConfigAddress, searchType,
				 &carry, 6)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecsMacConfigEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 lecsConfigTblName, lecsMacConfigAddress)) == NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lecsConfigTblName->length;
        for(i = 0; i < data->lecsConfigTblName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsConfigTblName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->lecsMacConfigAddress->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsMacConfigAddress->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
    }
    
    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsMacConfigAddress);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsMacConfigElanName
    case I_lecsMacConfigElanName:
	dp = (void *) (CloneOctetString(data->lecsMacConfigElanName));
	break;
#endif /* I_lecsMacConfigElanName */
	
#ifdef I_lecsMacConfigLastUsed
    case I_lecsMacConfigLastUsed:
	dp = (void *) (&data->lecsMacConfigLastUsed);
	break;
#endif /* I_lecsMacConfigLastUsed */
	
#ifdef I_lecsMacConfigStatus
    case I_lecsMacConfigStatus:
	dp = (void *) (&data->lecsMacConfigStatus);
	break;
#endif /* I_lecsMacConfigStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsMacConfigEntry data object.
 *---------------------------------------------------------------------*/
void
lecsMacConfigEntry_free (lecsMacConfigEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lecsMacConfigAddress);
	FreeOctetString(data->lecsMacConfigElanName);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsMacConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecsMacConfigEntry_cleanup (doList_t *trash)
{
    lecsMacConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecsMacConfigEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsMacConfigEntry family
 *---------------------------------------------------------------------*/
lecsMacConfigEntry_t *
Clone_lecsMacConfigEntry (lecsMacConfigEntry_t *lecsMacConfigEntry)
{
    lecsMacConfigEntry_t *data;
    
    if ((data = malloc(sizeof(lecsMacConfigEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecsMacConfigEntry),
	   sizeof(lecsMacConfigEntry_t)); 
    
    data->lecsMacConfigAddress =
	CloneOctetString(lecsMacConfigEntry->lecsMacConfigAddress); 
    data->lecsMacConfigElanName =
	CloneOctetString(lecsMacConfigEntry->lecsMacConfigElanName); 

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
lecsMacConfigEntry_test (OID* incoming, ObjectInfo* object,
			 ObjectSyntax* value, doList_t* doHead,
			 doList_t* doCur, ContextInfo* contextInfo) {
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lecsMacConfigEntry_t     *lecsMacConfigEntry;
    OctetString *  lecsConfigTblName;
    int            lecsConfigTblName_offset;
    OctetString *  lecsMacConfigAddress;
    int            lecsMacConfigAddress_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    lecsConfigTblName = NULL;
    lecsMacConfigAddress = NULL;
    
    /*
     * Validate the object instance
     *
     */
    lecsConfigTblName_offset = object->oid.length;
    lecsMacConfigAddress_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    final_index = lecsMacConfigAddress_offset + 6;
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, EXACT,
				    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToFixedOctetString(incoming,
				 lecsMacConfigAddress_offset,
				 &lecsMacConfigAddress, EXACT,
				 &carry,6 )) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lecsConfigTblName);
        FreeOctetString(lecsMacConfigAddress);
        return error_status;
    }
    
    lecsMacConfigEntry = k_lecsMacConfigEntry_get(-1, contextInfo, -1,
						  EXACT,
						  lecsConfigTblName,
						  lecsMacConfigAddress); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsMacConfigEntry_set) &&
            (((lecsMacConfigEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lecsMacConfigEntry_t *)
			      (dp->data))->lecsConfigTblName,
			     lecsConfigTblName) == 0) && 
            (CmpOctetStrings(((lecsMacConfigEntry_t *)
			      (dp->data))->lecsMacConfigAddress,
			     lecsMacConfigAddress) == 0) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsMacConfigEntry_set;
        dp->cleanupMethod = lecsMacConfigEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecsMacConfigEntry_UNDO
        dp->undoMethod = lecsMacConfigEntry_undo;
#else /* SR_lecsMacConfigEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsMacConfigEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecsMacConfigEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecsMacConfigEntry(lecsMacConfigEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecsMacConfigEntry(lecsMacConfigEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(lecsMacConfigEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lecsMacConfigEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecsMacConfigEntry_t *)
		 (dp->data))->lecsConfigTblName =
		     CloneOctetString(lecsConfigTblName); 
                SET_VALID(I_lecsConfigTblName, ((lecsMacConfigEntry_t
						 *)
						(dp->data))->valid); 
		
                ((lecsMacConfigEntry_t *)
		 (dp->data))->lecsMacConfigAddress =
		     CloneOctetString(lecsMacConfigAddress); 
                SET_VALID(I_lecsMacConfigAddress,
			  ((lecsMacConfigEntry_t *)
			   (dp->data))->valid); 
		
                error_status = k_lecsMacConfigEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsMacConfigAddress);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsMacConfigElanName
    case I_lecsMacConfigElanName:
	
	if (((lecsMacConfigEntry_t *)
	     (dp->data))->lecsMacConfigElanName != NULL) { 
        FreeOctetString(((lecsMacConfigEntry_t *)
			 (dp->data))->lecsMacConfigElanName); 
    }
	
	((lecsMacConfigEntry_t *) (dp->data))->lecsMacConfigElanName = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsMacConfigElanName */
	
#ifdef I_lecsMacConfigStatus
    case I_lecsMacConfigStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecsMacConfigStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsMacConfigEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecsMacConfigStatus_notInService)
	       || (value->sl_value == D_lecsMacConfigStatus_active)) {
		
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecsMacConfigStatus_createAndGo)
		|| (value->sl_value ==
		    D_lecsMacConfigStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecsMacConfigEntry_t *) (dp->data))->lecsMacConfigStatus =
	    value->sl_value; 
	break;
#endif /* I_lecsMacConfigStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecsMacConfigEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecsMacConfigEntry_test */
    error_status = k_lecsMacConfigEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsMacConfigEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecsMacConfigEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecsMacConfigEntry_set (doList_t* doHead, doList_t* doCur,
			ContextInfo* contextInfo) {
    return (k_lecsMacConfigEntry_set((lecsMacConfigEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecsAtmAddrConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsAtmAddrConfigEntry_get (OID* incoming, ObjectInfo* object, int
			    searchType, ContextInfo* contextInfo, int
			    serialNum) {
    int             arg;
    void            *dp;
    lecsAtmAddrConfigEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lecsConfigTblName;
    int             lecsConfigTblName_offset;
    OctetString *   lecsAtmAddrConfigAddress;
    int             lecsAtmAddrConfigAddress_offset;
    OctetString *   lecsAtmAddrConfigMask;
    int             lecsAtmAddrConfigMask_offset;
    int             index;
    int             final_index;

    arg = object->nominator;
    data = NULL;
    lecsConfigTblName = NULL;
    lecsAtmAddrConfigAddress = NULL;
    lecsAtmAddrConfigMask = NULL;

    lecsConfigTblName_offset = object->oid.length;
    lecsAtmAddrConfigAddress_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    lecsAtmAddrConfigMask_offset = lecsAtmAddrConfigAddress_offset + 20;
    final_index = lecsAtmAddrConfigMask_offset + 20;

    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToFixedOctetString(incoming,
				 lecsAtmAddrConfigMask_offset,
				 &lecsAtmAddrConfigMask, searchType,
				 &carry, 20)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToFixedOctetString(incoming,
				 lecsAtmAddrConfigAddress_offset,
				 &lecsAtmAddrConfigAddress,
				 searchType, &carry, 20)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || 
	(data = k_lecsAtmAddrConfigEntry_get(serialNum, contextInfo,
					     arg, searchType,
					     lecsConfigTblName,
					     lecsAtmAddrConfigAddress,
					     lecsAtmAddrConfigMask))
	== NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lecsConfigTblName->length;
        for(i = 0; i < data->lecsConfigTblName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsConfigTblName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->lecsAtmAddrConfigAddress->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsAtmAddrConfigAddress->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        for(i = 0; i < data->lecsAtmAddrConfigMask->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsAtmAddrConfigMask->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
    }
    
    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsAtmAddrConfigAddress);
    FreeOctetString(lecsAtmAddrConfigMask);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsAtmAddrConfigElanName
    case I_lecsAtmAddrConfigElanName:
	dp = (void *) (CloneOctetString(data->lecsAtmAddrConfigElanName));
	break;
#endif /* I_lecsAtmAddrConfigElanName */
	
#ifdef I_lecsAtmAddrLastUsed
    case I_lecsAtmAddrLastUsed:
	dp = (void *) (&data->lecsAtmAddrLastUsed);
	break;
#endif /* I_lecsAtmAddrLastUsed */
	
#ifdef I_lecsAtmAddrConfigStatus
    case I_lecsAtmAddrConfigStatus:
	dp = (void *) (&data->lecsAtmAddrConfigStatus);
	break;
#endif /* I_lecsAtmAddrConfigStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsAtmAddrConfigEntry data object.
 *---------------------------------------------------------------------*/
void
lecsAtmAddrConfigEntry_free (lecsAtmAddrConfigEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lecsAtmAddrConfigAddress);
	FreeOctetString(data->lecsAtmAddrConfigMask);
	FreeOctetString(data->lecsAtmAddrConfigElanName);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsAtmAddrConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecsAtmAddrConfigEntry_cleanup (doList_t *trash)
{
    lecsAtmAddrConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecsAtmAddrConfigEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsAtmAddrConfigEntry family
 *---------------------------------------------------------------------*/
lecsAtmAddrConfigEntry_t *
Clone_lecsAtmAddrConfigEntry (lecsAtmAddrConfigEntry_t *lecsAtmAddrConfigEntry)
{
    lecsAtmAddrConfigEntry_t *data;
    
    if ((data = malloc(sizeof(lecsAtmAddrConfigEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecsAtmAddrConfigEntry),
	   sizeof(lecsAtmAddrConfigEntry_t)); 
    
    data->lecsAtmAddrConfigAddress =
	CloneOctetString(lecsAtmAddrConfigEntry->lecsAtmAddrConfigAddress); 
    data->lecsAtmAddrConfigMask =
	CloneOctetString(lecsAtmAddrConfigEntry->lecsAtmAddrConfigMask); 
    data->lecsAtmAddrConfigElanName =
	CloneOctetString(lecsAtmAddrConfigEntry->lecsAtmAddrConfigElanName); 
    
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
lecsAtmAddrConfigEntry_test (OID* incoming, ObjectInfo* object,
			     ObjectSyntax* value, doList_t* doHead,
			     doList_t* doCur, ContextInfo*
			     contextInfo) {
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lecsAtmAddrConfigEntry_t     *lecsAtmAddrConfigEntry;
    OctetString *  lecsConfigTblName;
    int            lecsConfigTblName_offset;
    OctetString *  lecsAtmAddrConfigAddress;
    int            lecsAtmAddrConfigAddress_offset;
    OctetString *  lecsAtmAddrConfigMask;
    int            lecsAtmAddrConfigMask_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    lecsConfigTblName = NULL;
    lecsAtmAddrConfigAddress = NULL;
    lecsAtmAddrConfigMask = NULL;
    
    /*
     * Validate the object instance
     *
     */
    lecsConfigTblName_offset = object->oid.length;
    lecsAtmAddrConfigAddress_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    lecsAtmAddrConfigMask_offset = lecsAtmAddrConfigAddress_offset + 20;
    final_index = lecsAtmAddrConfigMask_offset + 20;
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, EXACT,
				    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToFixedOctetString(incoming,
				 lecsAtmAddrConfigAddress_offset,
				 &lecsAtmAddrConfigAddress, EXACT,
				 &carry, 20)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToFixedOctetString(incoming,
				 lecsAtmAddrConfigMask_offset,
				 &lecsAtmAddrConfigMask, EXACT,
				 &carry, 20)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lecsConfigTblName);
        FreeOctetString(lecsAtmAddrConfigAddress);
        FreeOctetString(lecsAtmAddrConfigMask);
        return error_status;
    }
    
    lecsAtmAddrConfigEntry = 
	k_lecsAtmAddrConfigEntry_get(-1, contextInfo, -1, EXACT,
				     lecsConfigTblName,
				     lecsAtmAddrConfigAddress,
				     lecsAtmAddrConfigMask);
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsAtmAddrConfigEntry_set) &&
            (((lecsAtmAddrConfigEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lecsAtmAddrConfigEntry_t *)
			      (dp->data))->lecsConfigTblName,
			     lecsConfigTblName) == 0) && 
            (CmpOctetStrings(((lecsAtmAddrConfigEntry_t *)
			      (dp->data))->lecsAtmAddrConfigAddress,
			     lecsAtmAddrConfigAddress) == 0) && 
            (CmpOctetStrings(((lecsAtmAddrConfigEntry_t *)
			      (dp->data))->lecsAtmAddrConfigMask,
			     lecsAtmAddrConfigMask) == 0) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsAtmAddrConfigEntry_set;
        dp->cleanupMethod = lecsAtmAddrConfigEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecsAtmAddrConfigEntry_UNDO
        dp->undoMethod = lecsAtmAddrConfigEntry_undo;
#else /* SR_lecsAtmAddrConfigEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsAtmAddrConfigEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecsAtmAddrConfigEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecsAtmAddrConfigEntry(lecsAtmAddrConfigEntry))
		== NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecsAtmAddrConfigEntry(lecsAtmAddrConfigEntry))
		== NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ((dp->data = malloc(sizeof(lecsAtmAddrConfigEntry_t))) == NULL){
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lecsAtmAddrConfigEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecsAtmAddrConfigEntry_t *)
		 (dp->data))->lecsConfigTblName =
		     CloneOctetString(lecsConfigTblName); 
                SET_VALID(I_lecsConfigTblName,
			  ((lecsAtmAddrConfigEntry_t *)
			   (dp->data))->valid); 
		
                ((lecsAtmAddrConfigEntry_t *)
		 (dp->data))->lecsAtmAddrConfigAddress =
		     CloneOctetString(lecsAtmAddrConfigAddress); 
                SET_VALID(I_lecsAtmAddrConfigAddress,
			  ((lecsAtmAddrConfigEntry_t *)
			   (dp->data))->valid); 
		
                ((lecsAtmAddrConfigEntry_t *)
		 (dp->data))->lecsAtmAddrConfigMask =
		     CloneOctetString(lecsAtmAddrConfigMask); 
                SET_VALID(I_lecsAtmAddrConfigMask,
			  ((lecsAtmAddrConfigEntry_t *)
			   (dp->data))->valid); 
		
                error_status = k_lecsAtmAddrConfigEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsAtmAddrConfigAddress);
    FreeOctetString(lecsAtmAddrConfigMask);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsAtmAddrConfigElanName
    case I_lecsAtmAddrConfigElanName:
	
	if (((lecsAtmAddrConfigEntry_t *)
	     (dp->data))->lecsAtmAddrConfigElanName != NULL) { 
	    FreeOctetString(((lecsAtmAddrConfigEntry_t *)
			     (dp->data))->lecsAtmAddrConfigElanName); 
	}
	
	((lecsAtmAddrConfigEntry_t *) (dp->data))->lecsAtmAddrConfigElanName = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsAtmAddrConfigElanName */
	
#ifdef I_lecsAtmAddrConfigStatus
    case I_lecsAtmAddrConfigStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecsAtmAddrConfigStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsAtmAddrConfigEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value ==
		D_lecsAtmAddrConfigStatus_notInService) ||
	       (value->sl_value == D_lecsAtmAddrConfigStatus_active))
		{ 
		    return INCONSISTENT_VALUE_ERROR;
		}
	} else { 			/* modifying an existing row */
	    if ((value->sl_value ==
		 D_lecsAtmAddrConfigStatus_createAndGo) ||
		(value->sl_value ==
		 D_lecsAtmAddrConfigStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecsAtmAddrConfigEntry_t *)
	 (dp->data))->lecsAtmAddrConfigStatus = value->sl_value; 
	break;
#endif /* I_lecsAtmAddrConfigStatus */ 
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecsAtmAddrConfigEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecsAtmAddrConfigEntry_test */
    error_status = k_lecsAtmAddrConfigEntry_test(object, value, dp,
						 contextInfo); 
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsAtmAddrConfigEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecsAtmAddrConfigEntry_ready(object, value,
						      doHead, dp); 
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecsAtmAddrConfigEntry_set (doList_t* doHead, doList_t* doCur,
			    ContextInfo* contextInfo) {
    return (k_lecsAtmAddrConfigEntry_set((lecsAtmAddrConfigEntry_t *)
					 (doCur->data), 
					 contextInfo, doCur->state));
}

#endif /* SETS */

/*---------------------------------------------------------------------
 * Retrieve data from the lecsLesConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind* lecsLesConfigEntry_get (OID *incoming, ObjectInfo* object,
				 int searchType, ContextInfo*
				 contextInfo, int serialNum) {
    int             arg;
    void            *dp;
    lecsLesConfigEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lecsConfigTblName; 
    int             lecsConfigTblName_offset;
    OctetString *   lecsElanConfigName; 
    int             lecsElanConfigName_offset;
    OctetString *   lecsLesAtmAddr; 
    int             lecsLesAtmAddr_offset;
    int             index;
    int             final_index;

    data = NULL;

    arg = object->nominator;
    lecsConfigTblName = NULL;
    lecsElanConfigName = NULL;
    lecsLesAtmAddr = NULL;
    
    lecsConfigTblName_offset = object->oid.length;
    lecsElanConfigName_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    lecsLesAtmAddr_offset = lecsElanConfigName_offset +
	GetVariableIndexLength(incoming, lecsElanConfigName_offset); 
    final_index = lecsLesAtmAddr_offset + 20;

    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToFixedOctetString(incoming, lecsLesAtmAddr_offset,
				 &lecsLesAtmAddr, searchType, &carry,
				 20)) < 0 ) { 
	arg = -1;
    }

    if ( (InstToVariableOctetString(incoming,
				    lecsElanConfigName_offset,
				    &lecsElanConfigName, searchType,
				    &carry)) < 0 ) { 
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }

    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_lecsLesConfigEntry_get(serialNum,
							  contextInfo,
							  arg,
							  searchType, 
							  lecsConfigTblName, 
							  lecsElanConfigName, 
							  lecsLesAtmAddr))
	 == NULL) { 
	 arg = -1;
     } else {
	 /*
	  * Build instance information
	  */
	 inst.oid_ptr = buffer;
	 index = 0;
	 inst.oid_ptr[index++] = data->lecsConfigTblName->length;
	 for(i = 0; i < data->lecsConfigTblName->length; i++) {
	     inst.oid_ptr[index++] = (unsigned long)
		 data->lecsConfigTblName->octet_ptr[i]; 
	     if (index > MAX_OID_SIZE) {
		 arg = -1;
		 index--;
	     }
	 }
	 
	 inst.oid_ptr[index++] = data->lecsElanConfigName->length;
	 for(i = 0; i < data->lecsElanConfigName->length; i++) {
	     inst.oid_ptr[index++] = (unsigned long)
		 data->lecsElanConfigName->octet_ptr[i]; 
	     if (index > MAX_OID_SIZE) {
		 arg = -1;
		 index--;
	     }
	 }
	 
	 for(i = 0; i < data->lecsLesAtmAddr->length; i++) {
	     inst.oid_ptr[index++] = (unsigned long)
		 data->lecsLesAtmAddr->octet_ptr[i]; 
	     if (index > MAX_OID_SIZE) {
		 arg = -1;
		 index--;
	     }
	 }
	 
	 inst.length = index;
     }
    
    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsElanConfigName);
    FreeOctetString(lecsLesAtmAddr);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsLesPriority
    case I_lecsLesPriority:
	dp = (void *) (&data->lecsLesPriority);
	break;
#endif /* I_lecsLesPriority */
	
#ifdef I_lecsLesConnState
    case I_lecsLesConnState:
	dp = (void *) (&data->lecsLesConnState);
	break;
#endif /* I_lecsLesConnState */
	
#ifdef I_lecsLesConfigStatus
    case I_lecsLesConfigStatus:
	dp = (void *) (&data->lecsLesConfigStatus);
	break;
#endif /* I_lecsLesConfigStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsLesConfigEntry data object.
 *---------------------------------------------------------------------*/
void lecsLesConfigEntry_free (lecsLesConfigEntry_t* data) {
    if (data != NULL) {
	FreeOctetString(data->lecsLesAtmAddr);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsLesConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int lecsLesConfigEntry_cleanup (doList_t* trash) {
    lecsLesConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecsLesConfigEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsLesConfigEntry family
 *---------------------------------------------------------------------*/
lecsLesConfigEntry_t* Clone_lecsLesConfigEntry (lecsLesConfigEntry_t*
						lecsLesConfigEntry) {
    lecsLesConfigEntry_t *data;
    
    if ((data = malloc(sizeof(lecsLesConfigEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecsLesConfigEntry),
	   sizeof(lecsLesConfigEntry_t)); 
    
    data->lecsLesAtmAddr =
	CloneOctetString(lecsLesConfigEntry->lecsLesAtmAddr); 
    data->lecsConfigTblName = 
	CloneOctetString(lecsLesConfigEntry->lecsConfigTblName);
    data->lecsElanConfigName = 
	CloneOctetString(lecsLesConfigEntry->lecsElanConfigName);
    
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
int lecsLesConfigEntry_test (OID* incoming, ObjectInfo* object,
			     ObjectSyntax* value, doList_t* doHead,
			     doList_t* doCur, ContextInfo*
			     contextInfo) {
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry; 
    lecsLesConfigEntry_t     *lecsLesConfigEntry;
    OctetString *  lecsConfigTblName;
    int            lecsConfigTblName_offset;
    OctetString *  lecsElanConfigName;
    int            lecsElanConfigName_offset;
    OctetString *  lecsLesAtmAddr;
    int            lecsLesAtmAddr_offset;
    int            final_index;

    error_status = NO_ERROR;
    carry = 0;
    lecsConfigTblName = NULL;
    lecsElanConfigName = NULL;
    lecsLesAtmAddr = NULL;

   /*
    * Validate the object instance
    *
    */
    lecsConfigTblName_offset = object->oid.length;
    lecsElanConfigName_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    lecsLesAtmAddr_offset = lecsElanConfigName_offset +
	GetVariableIndexLength(incoming, lecsElanConfigName_offset); 
    final_index = lecsLesAtmAddr_offset + 20;
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, EXACT,
				    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsElanConfigName_offset,
				    &lecsElanConfigName, EXACT,
				    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToFixedOctetString(incoming, lecsLesAtmAddr_offset,
				 &lecsLesAtmAddr, EXACT, &carry, 20))
	< 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lecsConfigTblName);
        FreeOctetString(lecsElanConfigName);
        FreeOctetString(lecsLesAtmAddr);
        return error_status;
    }
    
    lecsLesConfigEntry = k_lecsLesConfigEntry_get(-1, contextInfo, -1,
						  EXACT,
						  lecsConfigTblName,
						  lecsElanConfigName,
						  lecsLesAtmAddr); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsLesConfigEntry_set) &&
            (((lecsLesConfigEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lecsLesConfigEntry_t *)
			      (dp->data))->lecsConfigTblName,
			     lecsConfigTblName) == 0) && 
            (CmpOctetStrings(((lecsLesConfigEntry_t *)
			      (dp->data))->lecsElanConfigName,
			     lecsElanConfigName) == 0) && 
            (CmpOctetStrings(((lecsLesConfigEntry_t *)
			      (dp->data))->lecsLesAtmAddr,
			     lecsLesAtmAddr) == 0) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsLesConfigEntry_set;
        dp->cleanupMethod = lecsLesConfigEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecsLesConfigEntry_UNDO
        dp->undoMethod = lecsLesConfigEntry_undo;
#else /* SR_lecsLesConfigEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsLesConfigEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecsLesConfigEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecsLesConfigEntry(lecsLesConfigEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecsLesConfigEntry(lecsLesConfigEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(lecsLesConfigEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(lecsLesConfigEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecsLesConfigEntry_t *)
		 (dp->data))->lecsConfigTblName =
		     CloneOctetString(lecsConfigTblName); 
                SET_VALID(I_lecsConfigTblName, ((lecsLesConfigEntry_t*)
						(dp->data))->valid); 
		
                ((lecsLesConfigEntry_t *)
		 (dp->data))->lecsElanConfigName =
		     CloneOctetString(lecsElanConfigName); 
                SET_VALID(I_lecsElanConfigName, ((lecsLesConfigEntry_t*)
						 (dp->data))->valid); 
		
                ((lecsLesConfigEntry_t *) (dp->data))->lecsLesAtmAddr
		    = CloneOctetString(lecsLesAtmAddr); 
                SET_VALID(I_lecsLesAtmAddr, ((lecsLesConfigEntry_t *)
					     (dp->data))->valid); 
		
                error_status = k_lecsLesConfigEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(lecsConfigTblName);
    FreeOctetString(lecsElanConfigName);
    FreeOctetString(lecsLesAtmAddr);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsLesPriority
    case I_lecsLesPriority:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 10000)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecsLesConfigEntry_t *) (dp->data))->lecsLesPriority = value->sl_value;
	break;
#endif /* I_lecsLesPriority */
	
#ifdef I_lecsLesConfigStatus
    case I_lecsLesConfigStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecsLesConfigStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsLesConfigEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecsLesConfigStatus_notInService)
	       || (value->sl_value == D_lecsLesConfigStatus_active)) {
		
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecsLesConfigStatus_createAndGo)
		|| (value->sl_value ==
		    D_lecsLesConfigStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecsLesConfigEntry_t *) (dp->data))->lecsLesConfigStatus =
	    value->sl_value; 
	break;
#endif /* I_lecsLesConfigStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecsLesConfigEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecsLesConfigEntry_test */
    error_status = k_lecsLesConfigEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsLesConfigEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecsLesConfigEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int lecsLesConfigEntry_set (doList_t* doHead, doList_t* doCur,
			    ContextInfo* contextInfo) {
    return (k_lecsLesConfigEntry_set((lecsLesConfigEntry_t *) (doCur->data),
				     contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecsRDConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecsRDConfigEntry_get (OID* incoming, ObjectInfo* object, int
		       searchType, ContextInfo* contextInfo, 
		       int serialNum) {
    
    int             arg;
    void            *dp;
    lecsRDConfigEntry_t *data;
    SR_UINT32   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lecsConfigTblName;
    int             lecsConfigTblName_offset;
    SR_INT32        lecsRDConfigSegmentId;
    int             lecsRDConfigSegmentId_offset;
    SR_INT32        lecsRDConfigBridgeNum;
    int             lecsRDConfigBridgeNum_offset;
    int             index;
    int             final_index;
    
    data = NULL;
    arg = object->nominator;
    lecsConfigTblName = NULL;
    
    lecsConfigTblName_offset = object->oid.length;
    lecsRDConfigSegmentId_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    lecsRDConfigBridgeNum_offset = lecsRDConfigSegmentId_offset + 1;
    final_index = lecsRDConfigBridgeNum_offset + 1;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, lecsRDConfigBridgeNum_offset,
		    &lecsRDConfigBridgeNum, searchType, &carry)) < 0 )
	{ 
	    arg = -1;
	}
    if (searchType == NEXT) {
        lecsRDConfigBridgeNum = MAX(0, lecsRDConfigBridgeNum);
    }
    
    if ( (InstToInt(incoming, lecsRDConfigSegmentId_offset,
		    &lecsRDConfigSegmentId, searchType, &carry)) < 0 )
	{ 
	    arg = -1;
	}
    if (searchType == NEXT) {
        lecsRDConfigSegmentId = MAX(0, lecsRDConfigSegmentId);
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || 
	(data = k_lecsRDConfigEntry_get(serialNum, contextInfo,	arg,
					searchType, lecsConfigTblName,
					lecsRDConfigSegmentId,
					lecsRDConfigBridgeNum)) ==
	NULL) {  
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lecsConfigTblName->length;
        for(i = 0; i < data->lecsConfigTblName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecsConfigTblName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->lecsRDConfigSegmentId;
        if (index > MAX_OID_SIZE) {
            arg = -1;
            index--;
        }
        inst.oid_ptr[index++] = (unsigned long) data->lecsRDConfigBridgeNum;
        inst.length = index;
    }
    
    FreeOctetString(lecsConfigTblName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecsRDConfigElanName
    case I_lecsRDConfigElanName:
	dp = (void *) (CloneOctetString(data->lecsRDConfigElanName));
	break;
#endif /* I_lecsRDConfigElanName */
	
#ifdef I_lecsRDConfigLastUsed
    case I_lecsRDConfigLastUsed:
	dp = (void *) (&data->lecsRDConfigLastUsed);
	break;
#endif /* I_lecsRDConfigLastUsed */
	
#ifdef I_lecsRDConfigStatus
    case I_lecsRDConfigStatus:
	dp = (void *) (&data->lecsRDConfigStatus);
	break;
#endif /* I_lecsRDConfigStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecsRDConfigEntry data object.
 *---------------------------------------------------------------------*/
void
lecsRDConfigEntry_free (lecsRDConfigEntry_t *data) {
    if (data != NULL) {
	FreeOctetString(data->lecsRDConfigElanName);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecsRDConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int lecsRDConfigEntry_cleanup
SR_PROTOTYPE((doList_t *trash));

static int
lecsRDConfigEntry_cleanup (doList_t* trash) {
    lecsRDConfigEntry_free((lecsRDConfigEntry_t *) trash->data);
    lecsRDConfigEntry_free((lecsRDConfigEntry_t *) trash->undodata);
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecsRDConfigEntry family
 *---------------------------------------------------------------------*/
lecsRDConfigEntry_t *
Clone_lecsRDConfigEntry (lecsRDConfigEntry_t* lecsRDConfigEntry) {
    lecsRDConfigEntry_t *data;
    
    if (lecsRDConfigEntry == NULL) {
        return NULL;
    }
    
    if ((data = malloc(sizeof(lecsRDConfigEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecsRDConfigEntry),
	   sizeof(lecsRDConfigEntry_t)); 
    
    data->lecsRDConfigElanName =
	CloneOctetString(lecsRDConfigEntry->lecsRDConfigElanName); 
    
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
lecsRDConfigEntry_test(OID* incoming, ObjectInfo* object,
		       ObjectSyntax* value, doList_t* doHead,
		       doList_t* doCur, ContextInfo* contextInfo) {
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lecsRDConfigEntry_t     *lecsRDConfigEntry;
    OctetString *  lecsConfigTblName;
    int            lecsConfigTblName_offset;
    SR_INT32       lecsRDConfigSegmentId;
    int            lecsRDConfigSegmentId_offset;
    SR_INT32       lecsRDConfigBridgeNum;
    int            lecsRDConfigBridgeNum_offset;
    int            final_index;

    error_status = NO_ERROR;
    carry = 0;
    lecsConfigTblName = NULL;

   /*
    * Validate the object instance
    *
    */
    lecsConfigTblName_offset = object->oid.length;
    lecsRDConfigSegmentId_offset = lecsConfigTblName_offset +
	GetVariableIndexLength(incoming, lecsConfigTblName_offset); 
    lecsRDConfigBridgeNum_offset = lecsRDConfigSegmentId_offset + 1;
    final_index = lecsRDConfigBridgeNum_offset + 1;
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming,
				    lecsConfigTblName_offset,
				    &lecsConfigTblName, EXACT,
				    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, lecsRDConfigSegmentId_offset,
		    &lecsRDConfigSegmentId, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, lecsRDConfigBridgeNum_offset,
		    &lecsRDConfigBridgeNum, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lecsConfigTblName);
        return error_status;
    }
    
    lecsRDConfigEntry = k_lecsRDConfigEntry_get(-1, contextInfo, -1,
						EXACT,
						lecsConfigTblName,
						lecsRDConfigSegmentId,
						lecsRDConfigBridgeNum); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecsRDConfigEntry_set) &&
            (((lecsRDConfigEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lecsRDConfigEntry_t *)
			      (dp->data))->lecsConfigTblName,
			     lecsConfigTblName) == 0) && 
            (((lecsRDConfigEntry_t *)
	      (dp->data))->lecsRDConfigSegmentId ==
	     lecsRDConfigSegmentId) && 
            (((lecsRDConfigEntry_t *)
	      (dp->data))->lecsRDConfigBridgeNum ==
	     lecsRDConfigBridgeNum) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecsRDConfigEntry_set;
        dp->cleanupMethod = lecsRDConfigEntry_cleanup;
#ifdef SR_lecsRDConfigEntry_UNDO
        dp->undoMethod = lecsRDConfigEntry_undo;
#else /* SR_lecsRDConfigEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecsRDConfigEntry_UNDO */
        dp->state = UNKNOWN;
	
        if (lecsRDConfigEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecsRDConfigEntry(lecsRDConfigEntry)) == NULL)
		{ 
		    DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		    error_status = RESOURCE_UNAVAILABLE_ERROR;
		}
            if ((dp->undodata = (void *)
		 Clone_lecsRDConfigEntry(lecsRDConfigEntry)) == NULL)
		{ 
		    DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
		    error_status = RESOURCE_UNAVAILABLE_ERROR;
		}
	    
        }
        else {
            if ( (dp->data = malloc(sizeof(lecsRDConfigEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(lecsRDConfigEntry_t));
                dp->undodata = NULL;
		
                /* Fill in reasonable default values for this new entry */
                ((lecsRDConfigEntry_t *)
		 (dp->data))->lecsConfigTblName =
		     CloneOctetString(lecsConfigTblName); 
                SET_VALID(I_lecsRDConfigEntryIndex_lecsConfigTblName,
			  ((lecsRDConfigEntry_t *)
			   (dp->data))->valid); 
		
                ((lecsRDConfigEntry_t *)
		 (dp->data))->lecsRDConfigSegmentId =
		     (lecsRDConfigSegmentId); 
                SET_VALID(I_lecsRDConfigSegmentId,
			  ((lecsRDConfigEntry_t *)
			   (dp->data))->valid); 
		
                ((lecsRDConfigEntry_t *)
		 (dp->data))->lecsRDConfigBridgeNum =
		     (lecsRDConfigBridgeNum); 
                SET_VALID(I_lecsRDConfigBridgeNum,
			  ((lecsRDConfigEntry_t *)
			   (dp->data))->valid); 
		
                error_status = k_lecsRDConfigEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(lecsConfigTblName);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecsRDConfigElanName
    case I_lecsRDConfigElanName:
	
	if (!IsDisplayString(((lecsRDConfigEntry_t *)
			      (dp->data))->lecsRDConfigElanName)) { 
	    return (WRONG_VALUE_ERROR);
	}
	
	if (((lecsRDConfigEntry_t *) (dp->data))->lecsRDConfigElanName
	    != NULL) { 
	    FreeOctetString(((lecsRDConfigEntry_t *)
			     (dp->data))->lecsRDConfigElanName); 
	}
	
	((lecsRDConfigEntry_t *) (dp->data))->lecsRDConfigElanName = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecsRDConfigElanName */
	
#ifdef I_lecsRDConfigStatus
    case I_lecsRDConfigStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecsRDConfigStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecsRDConfigEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecsRDConfigStatus_notInService)
	       || (value->sl_value == D_lecsRDConfigStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecsRDConfigStatus_createAndGo)
		|| (value->sl_value ==
		    D_lecsRDConfigStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecsRDConfigEntry_t *) (dp->data))->lecsRDConfigStatus =
	    value->sl_value; 
	break;
#endif /* I_lecsRDConfigStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator"
		 " in lecsRDConfigEntry_test)\n")); 
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecsRDConfigEntry_test */
    error_status = k_lecsRDConfigEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecsRDConfigEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecsRDConfigEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecsRDConfigEntry_set(doList_t* doHead, doList_t* doCur, 
		      ContextInfo* contextInfo) {
    return (k_lecsRDConfigEntry_set((lecsRDConfigEntry_t *) (doCur->data),
				    contextInfo, doCur->state));
}

#endif /* SETS */

/*
 * Start of system-dependent routines
 */

static char* convert_to_null_terminated_string ( OctetString* os) {
    static char outstring[256];
    
    memcpy(outstring, os->octet_ptr, os->length);
    outstring[os->length] = '\0';
    
    return (outstring);
}

static LecsInfo* getInterface (long ifIndex, int searchType) {
    
    LecsInfo* pInt, *next;

    next = 0;
    for (pInt = List_head(G_LecsInfosList); pInt; pInt = List_next(pInt))
	if ((pInt->hwidb)) {
	    if (pInt->hwidb->snmp_if_index == ifIndex)
		break;
	    if (pInt->hwidb->snmp_if_index > ifIndex)
		if (next) {
		    if (next->hwidb->snmp_if_index >
			pInt->hwidb->snmp_if_index) 
			next = pInt;
		} else 
		    next = pInt;
	}
    if (!pInt) 
	if (searchType == EXACT)
	    return (NULL);
	else
	    if (!next)
		return (NULL);
	    else 
		pInt = next;
    
    return pInt;
}

lecsEntry_t    *
k_lecsEntry_get (int serialNum, ContextInfo* contextInfo, int
		 nominator, int searchType, long ifIndex) {
    static lecsEntry_t lecsEntryData;
    static OctetString config_os, lecFailAddr_os;
    LecsInfo* pInt;
  
    pInt = getInterface(ifIndex, searchType);
    if (!pInt) 
	return (NULL);

    while ((nominator == I_lecsConfigTableName) && (!pInt->cfgTable) && 
	   (searchType == NEXT)) {
	if (ifIndex > pInt->hwidb->snmp_if_index + 1)
	    return NULL;
	ifIndex = pInt->hwidb->snmp_if_index + 1;
	pInt = getInterface(ifIndex, NEXT);
	if (!pInt)
	    return (NULL);
    }
    
    SET_ALL_VALID(lecsEntryData.valid);
    
    /* Table may be sparse */
    if (pInt->cfgTable) {
	config_os.octet_ptr = pInt->cfgTable->tableName;
	config_os.length = strlen(pInt->cfgTable->tableName);
	lecsEntryData.lecsConfigTableName = &config_os;
    } else {
	if (nominator == I_lecsConfigTableName)
	    return NULL;
	lecsEntryData.lecsConfigTableName = NULL;
	CLR_VALID(I_lecsConfigTableName, lecsEntryData.valid);
    }
    lecsEntryData.lecsUpTime = SNMP_ABSOLUTE_TIME(pInt->lecsUpTime);
    lecsEntryData.lecsInConfigReqs = pInt->lecsInConfigReqs;
    lecsEntryData.lecsInConfigErrors = pInt->lecsInConfigErrors;
    lecsEntryData.lecsOutConfigFails = pInt->lecsOutConfigFails;
    lecsEntryData.lecsLastFailCause = pInt->causeOfLastFailure;
    lecFailAddr_os.octet_ptr = pInt->lastOffender;
    lecFailAddr_os.length = STATIONLEN_ATMNSAP;
    lecsEntryData.lecsLastFailLec = &lecFailAddr_os;
    
    /* For the operational status to be active the hardware interface must be
       up, there must be a configuration table associated with the interface,
       the LECS must be listening to an address on this interface. */
    
    if ((pInt->hwidb->state == IDBS_UP) && (pInt->cfgTable) &&
	 List_size(pInt->lecsAddressesList))
	lecsEntryData.lecsOperStatus = pInt->shutDown ? 
	    D_lecsOperStatus_inactive : D_lecsOperStatus_active; 
    else 
	lecsEntryData.lecsOperStatus = D_lecsOperStatus_inactive;
    
    lecsEntryData.lecsAdminStatus = pInt->shutDown ? D_lecsAdminStatus_inactive
	: D_lecsAdminStatus_active;
    lecsEntryData.lecsStatus = ACTIVE;
    lecsEntryData.lecsMasterState = pInt->thisLecsIsTheMaster ?
	D_lecsMasterState_true : D_lecsMasterState_false;
    lecsEntryData.ifIndex = pInt->hwidb->snmp_if_index;
    
    return (&lecsEntryData);
}

#ifdef SETS
int
k_lecsEntry_test (ObjectInfo* object, ObjectSyntax* value, doList_t*
		  dp, ContextInfo* contextInfo) {
  lecsEntry_t* data;
  snmpidbtype* snmpIfEntry;
  
  data  = (lecsEntry_t *)dp->data;
  snmpIfEntry = ifmib_snmpidb_get(NULL,-1, EXACT, data->ifIndex,
				  NULL, NULL);
  if (!snmpIfEntry)
      return (INCONSISTENT_NAME_ERROR);
  if (snmpIfEntry->snmp_if_struct != SNMP_IF_HWIDB)
      return (INCONSISTENT_NAME_ERROR);
  if (!snmpIfEntry->snmp_if_ptr.ifptr.hwidb)
      return (GEN_ERROR);
  if ((snmpIfEntry->snmp_if_ptr.ifptr.hwidb->type != IDBTYPE_AIP) &&
      (snmpIfEntry->snmp_if_ptr.ifptr.hwidb->type != IDBTYPE_ATMZR))
      return (INCONSISTENT_NAME_ERROR);
  
  switch (object->nominator) {
      
#ifdef I_lecsConfigTableName
  case I_lecsConfigTableName:
      
      /* SR stack still doesn't check for length */
      
      if (value->os_value->length > 32)
	  return (WRONG_LENGTH_ERROR);
      
      /* Cisco LECS cannot handle NUL in config table names */
      
      if (contains_null(value->os_value))
	  return (WRONG_VALUE_ERROR);
      
      /* Config Table name must exist prior to using in the LECS */
      
      if (!k_lecsConfigTblEntry_get(-1, contextInfo, object->nominator, EXACT,
				    value->os_value))
	  return (INCONSISTENT_VALUE_ERROR);
      
      SET_VALID(I_lecsConfigTableName, data->valid);
      
      break;
#endif
      
#ifdef I_lecsStatus
  case I_lecsStatus:
      
      /* These states are not supported */
      
      if ((value->sl_value == D_lecsStatus_createAndWait) || 
	  (value->sl_value == D_lecsStatus_notInService))
	  return (WRONG_VALUE_ERROR);
      
      SET_VALID(I_lecsStatus, data->valid);
      
      break;
#endif
  }

  return NO_ERROR;
}

int
k_lecsEntry_ready (ObjectInfo* object, ObjectSyntax* value, doList_t*
		   doHead, doList_t* dp) {

    /* Setting the dp->state to ADD_MODIFY is a mechanism to validate
       the row.  If the dp->state is left in UNKNOWN then the SR SNMP
       engine will through out the SET request.  Setting the state to
       ADD_MODIFY does not necessarily mean a row will be
       added/modified, it could just as well be deleted.  Look at the
       RowStatus object for the row operation. */ 

    if (((lecsEntry_t *) (dp->data))->lecsStatus != UNUSED)
	dp->state = ADD_MODIFY;
    
    return NO_ERROR;
}

int
k_lecsEntry_set_defaults (doList_t* dp) {
    lecsEntry_t *data;
    
    data = (lecsEntry_t *) (dp->data);
    data->lecsAdminStatus = D_lecsAdminStatus_active;
    SET_VALID(I_lecsAdminStatus, data->valid);
    
    data->lecsStatus = UNUSED;
    
    return NO_ERROR;
}

int
k_lecsEntry_set (lecsEntry_t* data, ContextInfo* contextInfo, int
		 function) {
    LecsInfo* pInt;
    ConfigTable* pCfg;
    snmpidbtype* snmpIfEntry;
    idbtype* swidb;
    
    for (pInt = List_head(G_LecsInfosList); pInt; pInt = List_next(pInt))
	if (pInt->hwidb)
	    if (pInt->hwidb->snmp_if_index == data->ifIndex)
		break;
    
    switch (data->lecsStatus) {
    case CREATE_AND_GO:
	if (pInt) 
	    return (COMMIT_FAILED_ERROR);
	snmpIfEntry = 
	    ifmib_snmpidb_get(NULL,-1, EXACT, data->ifIndex, NULL, NULL);
	
	swidb = snmpIfEntry->snmp_if_ptr.ifptr.hwidb->firstsw;
	
	if(!swidb)
	    return (GEN_ERROR);
	
	pInt = buildLecsPointer(swidb);
	
    case ACTIVE:
	if (!pInt) 
	    return (COMMIT_FAILED_ERROR);
	if (VALID(I_lecsConfigTableName, data->valid)) {
	    pCfg = getConfigTableBySNMPName(data->lecsConfigTableName);
	    if (!pCfg)
		return (COMMIT_FAILED_ERROR);
	    pInt->cfgTable = pCfg;
	}
	if (VALID(I_lecsAdminStatus, data->valid)) 
	    pInt->shutDown = data->lecsAdminStatus == D_lecsAdminStatus_active 
		? 0 : 1;
	break;
    case DESTROY: 
	LecsInfo_destroy(pInt);
	break;
    default:
	return (GEN_ERROR);
    }
    
    return (NO_ERROR);
}

#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecsEntry_UNDO
/* add #define SR_lecsEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsEntry family.
 */
int
lecsEntry_undo (doList_t* doHead, doList_t* doCur, ContextInfo*
		contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsEntry_UNDO */
#endif /* SR_SNMPv2 */

lecsAtmAddrEntry_t *
k_lecsAtmAddrEntry_get (int serialNum, ContextInfo* contextInfo, int
			nominator, int searchType, long ifIndex, long
			lecsAtmAddrIndex) {
    static lecsAtmAddrEntry_t lecsAtmAddrEntryData;
    static OctetString spec_os, mask_os, actual_os;
    LecsInfo* pInt, *next_Int;
    LecsAddress* pAddr, *next_Addr;
  
    next_Int = 0;
    pAddr = 0;
    next_Addr = 0;
    
    SET_ALL_VALID(lecsAtmAddrEntryData.valid);
    
    for (pInt = List_head(G_LecsInfosList); pInt; pInt = List_next(pInt))
	if (pInt->hwidb) {
	    if (pInt->hwidb->snmp_if_index == ifIndex) {
		for (pAddr = List_head(pInt->lecsAddressesList); pAddr; 
		     pAddr = List_next(pAddr)) 
		    if (pAddr->snmpIndex == lecsAtmAddrIndex)
			break;
		    else if (pAddr->snmpIndex > lecsAtmAddrIndex)
			if ((!next_Addr) || (next_Addr->snmpIndex > 
					     pAddr->snmpIndex)) {
			    next_Addr = pAddr;
			    next_Int = pInt;
			}
		if ((searchType == EXACT) || (pAddr) || (next_Addr))
		    break;
	    } else {
		if (pInt->hwidb->snmp_if_index > ifIndex)
		    if ((!next_Int) || (next_Int->hwidb->snmp_if_index > 
					pInt->hwidb->snmp_if_index)) {
			next_Int = pInt;
			for (pAddr = List_head(pInt->lecsAddressesList); 
			     pAddr; pAddr = List_next(pAddr))
			    if ((!next_Addr) || (pAddr->snmpIndex <
						 next_Addr->snmpIndex)) { 
				next_Addr = pAddr;
			    }
		    }
	    }
	}
    
    if (!pAddr)
	if (searchType == EXACT) 
	    return NULL;
	else 
	    if ((!next_Addr) || (!next_Int))
		return (NULL);
	    else {
		pInt = next_Int;
		pAddr = next_Addr;
	    }
    
    lecsAtmAddrEntryData.lecsAtmAddrIndex = pAddr->snmpIndex;
    spec_os.octet_ptr = pAddr->DATA_BYTES;
    spec_os.length = STATIONLEN_ATMNSAP;
    lecsAtmAddrEntryData.lecsAtmAddrSpec = &spec_os;
    mask_os.octet_ptr = pAddr->maskedAddr.maskBytes; 
    mask_os.length = STATIONLEN_ATMNSAP;
    lecsAtmAddrEntryData.lecsAtmAddrMask = &mask_os;
    actual_os.octet_ptr = pAddr->actualValue.addr;
    actual_os.length = STATIONLEN_ATMNSAP;
    lecsAtmAddrEntryData.lecsAtmAddrActual = &actual_os;
    
    /* Code does not support tracking of the registering state.  The
       addrFlags is incremented by one below to avoid using a 0 enumeration. */
    
    lecsAtmAddrEntryData.lecsAtmAddrState = pAddr->addrFlags + 1;
    lecsAtmAddrEntryData.lecsAtmAddrStatus = ACTIVE;
    lecsAtmAddrEntryData.ifIndex = pInt->hwidb->snmp_if_index;
    
    return (&lecsAtmAddrEntryData);
}

static LecsInfo* LecsInfo_searchIfIndex (long index) {

    LecsInfo* pInt;
    for (pInt = List_head(G_LecsInfosList); pInt; pInt = List_next(pInt))
	if (pInt->hwidb) 
	    if (pInt->hwidb->snmp_if_index == index)
		return (pInt);
    
    return (pInt);
}

#ifdef SETS
int
k_lecsAtmAddrEntry_test (ObjectInfo* object, ObjectSyntax* value,
			 doList_t* dp, ContextInfo* contextInfo) {
  lecsAtmAddrEntry_t* data;
  LecsInfo* pInt;

  data = (lecsAtmAddrEntry_t *) dp->data;
  pInt = LecsInfo_searchIfIndex(data->ifIndex);

  if (!pInt)
      return (INCONSISTENT_NAME_ERROR);

  switch (object->nominator) {
      
#ifdef I_lecsAtmAddrSpec
  case I_lecsAtmAddrSpec:
      
      if (value->os_value->length != 20)
	  return (WRONG_LENGTH_ERROR);
      
      SET_VALID(I_lecsAtmAddrSpec, data->valid);
      break;
#endif
      
#ifdef I_lecsAtmAddrMask
  case I_lecsAtmAddrMask:
      
      if (value->os_value->length != 20)
	  return (WRONG_LENGTH_ERROR);
      
      SET_VALID(I_lecsAtmAddrMask, data->valid);
      break;
#endif
      
#ifdef I_lecsAtmAddrStatus
  case I_lecsAtmAddrStatus:
      
      /* these states not supported */
      if ((value->sl_value == D_lecsAtmAddrStatus_notInService) || 
	  (value->sl_value == D_lecsAtmAddrStatus_createAndWait))
	  return (WRONG_VALUE_ERROR);
      
      break;
#endif
  }  /* switch */
  
  return NO_ERROR;
}

int
k_lecsAtmAddrEntry_ready (ObjectInfo* object, ObjectSyntax* value,
			  doList_t* doHead, doList_t* dp) {
    lecsAtmAddrEntry_t* data;
    LecsInfo* pInt;
    MaskedAddress maskedAddr;
    
    data = (lecsAtmAddrEntry_t*)dp->data;
    switch (data->lecsAtmAddrStatus) {
    case ACTIVE:
    case CREATE_AND_GO:
	pInt = LecsInfo_searchIfIndex(data->ifIndex);
	
	if (VALID(I_lecsAtmAddrMask, data->valid) &&
	    (!VALID(I_lecsAtmAddrSpec, data->valid))) {
	    dp->state = UNKNOWN;
	    break;
	}
	
	maskedAddr.length = STATIONLEN_ATMNSAP;
	maskedAddr.type = STATION_ATMNSAP;
	if (VALID(I_lecsAtmAddrSpec, data->valid)) {
	    memcpy(maskedAddr.dataBytes, data->lecsAtmAddrSpec->octet_ptr,
		   data->lecsAtmAddrSpec->length);
	    if (VALID(I_lecsAtmAddrMask, data->valid))
		memcpy(maskedAddr.maskBytes, data->lecsAtmAddrMask->octet_ptr,
		       data->lecsAtmAddrMask->length);
	    else 
		memset(maskedAddr.maskBytes, 0xff, STATIONLEN_ATMNSAP);
	} else {
	    memset(maskedAddr.dataBytes, 0, STATIONLEN_ATMNSAP);
	    memset(maskedAddr.maskBytes, 0, STATIONLEN_ATMNSAP);
	}
	
	/* Check to see if this address matches an existing one.  The
	   row state must be set even on failure to possibly correct a
	   previous false positive (in the event that a specified
	   address didn't exist with an old mask but does with the
	   new.  Can only happen in multi-varbind SET. */

	if (!searchLecsAddress (pInt, &maskedAddr, 
			        LANE_LECS_BIND_ADDR, 0))
	    dp->state = ADD_MODIFY;
	else 
	    dp->state = UNKNOWN;
	break;
    case DESTROY:
	dp->state = DELETE;
    }
    
    return NO_ERROR;
}

int
k_lecsAtmAddrEntry_set_defaults (doList_t* dp) {
    lecsAtmAddrEntry_t *data;
    
    data = (lecsAtmAddrEntry_t *) (dp->data);
    
    data->lecsAtmAddrStatus = UNUSED;
    return NO_ERROR;
}

int
k_lecsAtmAddrEntry_set (lecsAtmAddrEntry_t* data, ContextInfo*
			contextInfo, int function) {
  snmpidbtype* pInt;
  idbtype* swidb;
  MaskedAddress maskedAddr;

  pInt = ifmib_snmpidb_get(NULL,-1, EXACT, data->ifIndex, NULL, NULL);
  swidb = 0;

  maskedAddr.type = STATION_ATMNSAP;
  maskedAddr.length = STATIONLEN_ATMNSAP;
  if (VALID(I_lecsAtmAddrSpec, data->valid)) {
      memcpy(maskedAddr.dataBytes, data->lecsAtmAddrSpec->octet_ptr,
	     data->lecsAtmAddrSpec->length);
      if (VALID(I_lecsAtmAddrMask, data->valid))
	  memcpy(maskedAddr.maskBytes, data->lecsAtmAddrMask->octet_ptr,
		 data->lecsAtmAddrSpec->length);
      else 
	  memset(maskedAddr.maskBytes, 0xff, STATIONLEN_ATMNSAP);
  } else {
      memset(maskedAddr.dataBytes, 0, STATIONLEN_ATMNSAP);
      memset(maskedAddr.maskBytes, 0, STATIONLEN_ATMNSAP);
  }
  
  if (pInt)
      swidb = pInt->snmp_if_ptr.ifptr.hwidb->firstsw;

  if (!swidb)
      return (GEN_ERROR);

  switch (data->lecsAtmAddrStatus) {
  case ACTIVE:
    deleteLecsAddress(swidb, NULL, LANE_LECS_BIND_ADDR,
		        data->lecsAtmAddrIndex);  
    if (addLecsAddress(swidb, &maskedAddr,
		       LANE_LECS_BIND_ADDR,
		       data->lecsAtmAddrIndex)) 
    return (COMMIT_FAILED_ERROR);
    break;
  case CREATE_AND_GO:
    if (addLecsAddress(swidb, &maskedAddr,
		       LANE_LECS_BIND_ADDR,
		       data->lecsAtmAddrIndex)) 
    return (COMMIT_FAILED_ERROR);
    break;
  case DESTROY:
    deleteLecsAddress(swidb, NULL, LANE_LECS_BIND_ADDR,
		      data->lecsAtmAddrIndex);
    break;
  default:
      return (GEN_ERROR);
  }
  
  return (NO_ERROR);
}

#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecsAtmAddrEntry_UNDO
/* add #define SR_lecsAtmAddrEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsAtmAddrEntry family.
 */
int
lecsAtmAddrEntry_undo (doList_t* doHead, doList_t* doCur, ContextInfo*
		       contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsAtmAddrEntry_UNDO */
#endif /* SR_SNMPv2 */

static void* getConnection (long *ifIndex, long* vpi, long* vci, int searchType, 
		     int* type) {
    
    Connection *pConn, *pNext;
    long nextIndex;
    LecsInfo* pif;
    int vcd;
    long exact_port, exact_vpi, exact_vci;
    long next_vpi, next_vci;
    
    pConn = 0;
    pNext = 0;
    next_vpi = LECS_VCC_INFO_INVALID;
    next_vci = LECS_VCC_INFO_INVALID;
    if ((!ifIndex) || (!vpi) || (!vci))
	return NULL;
    nextIndex = *ifIndex;
    
    pif = getInterface(*ifIndex, searchType);
    if (!pif)
	return (NULL);
    
    while (pif) {
	if (!pif->hwidb) {
	    nextIndex++;
	    pif = getInterface(nextIndex, NEXT);
	}
	    
	for (pConn = List_head(pif->connectionList); pConn; 
	     pConn = List_next(pConn)) {

	    vcd = atmSig_api_callId2vcd(pConn->callId);
	    if (!atm_getvc_mib_id(vcd, pif->hwidb, &exact_port,
				  &exact_vpi, &exact_vci))
		continue;
	    
	    if (exact_vpi == *vpi) {
		if (exact_vci == *vci)
		    break;
		else  
		    if (exact_vci > *vci)
			if ((!pNext) || 
			    (next_vci > exact_vci)) {
			    next_vpi = exact_vpi;
			    next_vci = exact_vci;
			    pNext = pConn;
			}
	    } else  
		if (exact_vpi > *vpi)
		    if ((!pNext) || (next_vpi > exact_vpi)) {
			next_vpi = exact_vpi;
			next_vci = exact_vci;
			pNext = pConn;
		    } 
	}
	
	if (pConn)
	    break;
	
	if (pNext || (searchType == EXACT))
	    break;
	
	nextIndex++;
	pif = getInterface(nextIndex, NEXT);
    }
    
    if (!pConn)
	if (searchType == EXACT)
	    return (NULL);
	else 
	    if (!pNext)
		return (NULL);
	    else {
		pConn = pNext;
		exact_vpi = next_vpi;
		exact_vci = next_vci;
	    }
    
    *vpi = exact_vpi;
    *vci = exact_vci;
    *ifIndex = pif->hwidb->snmp_if_index;
    *type = TYPE_SVC;
    return (pConn);
}

lecsConfigDirectConnEntry_t *
k_lecsConfigDirectConnEntry_get (int serialNum, ContextInfo*
				 contextInfo, int nominator, int
				 searchType, long ifIndex, long
				 lecsConfigDirectConnVpi, long
				 lecsConfigDirectConnVci) {
    static lecsConfigDirectConnEntry_t lecsConfigDirectConnEntryData;
    static OctetString src_os, dst_os;
    void* pConn;
    Connection* pSvc;
    int carry, type;
    
    carry = 0;
    pConn = getConnection(&ifIndex, &lecsConfigDirectConnVpi, 
			  &lecsConfigDirectConnVci, searchType, &type);
    if (!pConn)
	return NULL;
    
    while ((searchType == NEXT) && 
	((nominator == I_lecsConfigDirectConnDst) &&  
	 (type == TYPE_SVC) &&
	 (!((Connection*)pConn)->calledParty))) { 
	if (++lecsConfigDirectConnVci > 65536) {
	    carry = 1;
	    lecsConfigDirectConnVci = 0;
	}
	lecsConfigDirectConnVpi += carry;
	carry = 0;
	if (lecsConfigDirectConnVpi > 256) {
	    lecsConfigDirectConnVpi = 0;
	    carry = 1;
	}
	if (ifIndex > ifIndex + carry)
	    return (NULL);
	ifIndex += carry;
	pConn = getConnection(&ifIndex, &lecsConfigDirectConnVpi, 
			      &lecsConfigDirectConnVci, NEXT, &type);
	if (!pConn)
	    return NULL;
    }
    
    lecsConfigDirectConnEntryData.ifIndex = ifIndex;
    lecsConfigDirectConnEntryData.lecsConfigDirectConnVpi =
	lecsConfigDirectConnVpi ;
    lecsConfigDirectConnEntryData.lecsConfigDirectConnVci =
	lecsConfigDirectConnVci ;
    lecsConfigDirectConnEntryData.lecsConfigDirectConnVCType = 
	type;
    lecsConfigDirectConnEntryData.lecsConfigDirectDstType = 
	D_lecsConfigDirectDstType_unknown;
    if (type == TYPE_SVC) 
    {
	hwaddrtype cpa;

	pSvc = (Connection*)pConn;
	if (pSvc->callerType)
	    lecsConfigDirectConnEntryData.lecsConfigDirectDstType = 
		pSvc->callerType;
	atmSig_api_callId2CallingPartyAddress(pSvc->callId, &cpa);
	if (cpa.type == STATION_ATMNSAP)
	{
	    src_os.octet_ptr = cpa.addr;	
	    src_os.length = STATIONLEN_ATMNSAP;
	    lecsConfigDirectConnEntryData.lecsConfigDirectConnSrc = &src_os;
	}
	else
	    lecsConfigDirectConnEntryData.lecsConfigDirectConnSrc = NULL;
	if (pSvc->calledParty) {
	    dst_os.octet_ptr = pSvc->calledParty->DATA_BYTES;
	    dst_os.length = pSvc->calledParty->ADDR_LEN;
	    lecsConfigDirectConnEntryData.lecsConfigDirectConnDst = &dst_os;
	} else {
	    if (nominator == I_lecsConfigDirectConnDst)
		return NULL;
	    lecsConfigDirectConnEntryData.lecsConfigDirectConnDst =
		NULL;
	}
    } else {
	if ((nominator == I_lecsConfigDirectConnSrc) || 
	    (nominator == I_lecsConfigDirectConnDst))
	    return NULL;
	lecsConfigDirectConnEntryData.lecsConfigDirectConnSrc = NULL;
	lecsConfigDirectConnEntryData.lecsConfigDirectConnDst = NULL;
    }
    
    return (&lecsConfigDirectConnEntryData);
}

static unsigned char 
valid_configtab(OctetString * os) {
    ConfigTable *pCfg;

    pCfg = getConfigTableBySNMPName (os);
    if (!pCfg)
	return 0;
    
    return 1;
}

static ConfigTable* getConfigTableBySNMPName (OctetString* os) {

    ConfigTable *pCfg;
    int len;
    
    if (os == NULL) 
	return NULL;
    pCfg = List_head(G_ConfigTablesList);
    while (pCfg != NULL) {
	len = strlen(pCfg->tableName);
	if ((os->length == len) && (memcmp(os->octet_ptr, pCfg->tableName, 
					   len) == 0))
	    return pCfg;
	pCfg = List_next(pCfg);
    }
    return NULL;
}

static ConfigTable* getNextConfigTableBySNMPName (OctetString* os, int type) {
    
    ConfigTable *pCfg, *retval;
    int name_len, retval_len, found;
    int result;
    
    retval = 0;
    retval_len = NAMESIZE + 1;
    found =0;
    result = 0;
    
    if (os == NULL) 
	return NULL;
    
    pCfg = List_head(G_ConfigTablesList);
    retval = pCfg;
    
    while (pCfg != NULL) {
	name_len = strlen(pCfg->tableName);
	if (type == CONFIG_VARIABLE) {
	    if (name_len >= os->length) {
		if (name_len == os->length)
		    if (memcmp(pCfg->tableName, os->octet_ptr,
			       name_len) <= 0) { 
			pCfg = List_next(pCfg);
			continue;
		    }
		found = 1;
		if ( name_len <= retval_len) { /* We want the
						  immediate successor
						  */ 
		    if (name_len == retval_len)
			if (strcmp (pCfg->tableName, retval->tableName) >= 0) {
			    pCfg = List_next(pCfg);
			    continue;
			}
		    retval = pCfg;
		    retval_len = name_len;
		}
	    }
	} else { /* type is IMPLIED */ 
	    result = memcmp(pCfg->tableName, os->octet_ptr, name_len < 
			    os->length ? name_len : os->length);
	    if (((result == 0) && (name_len > os->length )) || (result > 0))
		if ((!found) || (strcmp(pCfg->tableName,
					retval->tableName) < 0)) { 
		    retval = pCfg;
		    found = 1;
		}
	}
	pCfg = List_next(pCfg);
    }
    if (found)
	return (retval);
    else
	return (NULL);
}

lecsConfigTblEntry_t *
k_lecsConfigTblEntry_get (int serialNum, ContextInfo* contextInfo, int
			  nominator, int searchType, OctetString*
			  lecsConfigTblName) {
    static lecsConfigTblEntry_t lecsConfigTblEntryData;
    static OctetString config_os, defaultElan_os;
    ConfigTable *pCfg;
    
    pCfg = getConfigTableBySNMPName(lecsConfigTblName);
    
    if (!pCfg)
	if (searchType == EXACT)
	    return NULL; 
	else {
	    pCfg = getNextConfigTableBySNMPName(lecsConfigTblName,
						CONFIG_IMPLIED); 
	    if (!pCfg)
		return NULL;
	}
    
    config_os.octet_ptr = (unsigned char*)pCfg->tableName;
    config_os.length = strlen(pCfg->tableName);
    lecsConfigTblEntryData.lecsConfigTblName = &config_os;
    if (pCfg->defaultElan) {
        defaultElan_os.octet_ptr = pCfg->defaultElan->elanName;
        defaultElan_os.length = strlen(pCfg->defaultElan->elanName);
    } else 
        defaultElan_os.length = 0;
    lecsConfigTblEntryData.lecsConfigTblDefaultElanName = &defaultElan_os;
    lecsConfigTblEntryData.lecsConfigTblStatus = ACTIVE;
    
    return (&lecsConfigTblEntryData);
}

#ifdef SETS
int
k_lecsConfigTblEntry_test (ObjectInfo* object, ObjectSyntax* value,
			   doList_t* dp, ContextInfo* contextInfo) {
    lecsConfigTblEntry_t* data;
    lecsElanConfigEntry_t* elan;

    data = (lecsConfigTblEntry_t *)dp->data;
    elan = NULL;
  
    /* SR stack doesn't check TCs */
    if (!IsDisplayString(data->lecsConfigTblName))
	return (NO_CREATION_ERROR);
  
    /* Implementation cannot handle NULs in ELAN names */
    if (contains_null(data->lecsConfigTblName))
	return (NO_CREATION_ERROR);
  
    /* SR stack won't check lengths */
    if (data->lecsConfigTblName->length > 32)
	return (NO_CREATION_ERROR);
    
    switch (object->nominator) {
	
#ifdef I_lecsConfigTblDefaultElanName
    case I_lecsConfigTblDefaultElanName:
	
	/* This checks to see if the Default Elan is being cleared.
	   If it is then there is no need to check the validty of the
	   Elan name */ 
	
	if (value->os_value->length != 0) {
	    
	    /* the following ensures that the appropriate elan entry exists */
	    elan = k_lecsElanConfigEntry_get(-1, contextInfo,
					     object->nominator, EXACT,
					     data->lecsConfigTblName,
					     value->os_value);
	    if (!elan)
		return(INCONSISTENT_VALUE_ERROR);
	    
	    /* A default ELAN is not allowed to be 'closed' */
	    if (elan->lecsElanAccess == D_lecsElanAccess_closed)
		return(INCONSISTENT_VALUE_ERROR);
	}
	
	break;
#endif
	
#ifdef I_lecsConfigTblStatus
    case I_lecsConfigTblStatus:
	
	/* This implementation does not support the following states */
	
	if ((value->sl_value == D_lecsConfigTblStatus_createAndWait) || 
	    (value->sl_value == D_lecsConfigTblStatus_notInService)) 
	    return (WRONG_VALUE_ERROR);
	break;
#endif
    }
    
    return NO_ERROR;
}

int
k_lecsConfigTblEntry_ready (ObjectInfo* object, ObjectSyntax* value,
			    doList_t* doHead, doList_t* dp) {
    if (((lecsConfigTblEntry_t *) (dp->data))->lecsConfigTblStatus != UNUSED)
	dp->state = ADD_MODIFY;
  
    return NO_ERROR;
}

int
k_lecsConfigTblEntry_set_defaults (doList_t* dp) {
    lecsConfigTblEntry_t *data;

    data = (lecsConfigTblEntry_t *) (dp->data);
    data->lecsConfigTblStatus = UNUSED;

    return NO_ERROR;
}

int
k_lecsConfigTblEntry_set (lecsConfigTblEntry_t* data, ContextInfo*
			  contextInfo, int function) {
  ConfigTable *pCfg;
  char* transfer;
  
  switch (data->lecsConfigTblStatus) {
  case ACTIVE: 	
      if (VALID(I_lecsConfigTblDefaultElanName, data->valid)) {
	  pCfg = getConfigTableBySNMPName(data->lecsConfigTblName);
	  if (!pCfg)
	      return (COMMIT_FAILED_ERROR);
	  
	  if (data->lecsConfigTblDefaultElanName->length == 0)
	      transfer = 0;
	  else 
	      transfer =
		  convert_to_null_terminated_string(data->lecsConfigTblDefaultElanName); 
	  
	  if (ConfigTable_setDefaultElan(pCfg, transfer) < 0)
	      return (COMMIT_FAILED_ERROR);
      }
      break;
  case CREATE_AND_GO: 
      transfer = convert_to_null_terminated_string(data->lecsConfigTblName);
      if (!ConfigTable_alloc(transfer))
	  return (RESOURCE_UNAVAILABLE_ERROR);
      break;
  case DESTROY:
      pCfg = getConfigTableBySNMPName(data->lecsConfigTblName);
      if (pCfg) {
	  ConfigTable_destroyByPtr(pCfg);
      }
      break;
  default:
      return (GEN_ERROR);
  }
  
  return (NO_ERROR);
}

#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecsConfigTblEntry_UNDO
/* add #define SR_lecsConfigTblEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsConfigTblEntry family.
 */
int
lecsConfigTblEntry_undo (doList_t* doHead, doList_t* doCur,
			 ContextInfo* contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsConfigTblEntry_UNDO */
#endif /* SR_SNMPv2 */

static ElanInfoBlock* getElanBySNMPName (ConfigTable* pCfg, OctetString* os) {
  ElanInfoBlock *pElan;
  int len;

  if (os == NULL) 
      return NULL;
  pElan = List_head(pCfg->elanList);
  while (pElan != NULL) {
      len = strlen(pElan->elanName);
      if ((len == os->length) && (memcmp (pElan->elanName,
					  os->octet_ptr, len ) == 0)) 
	  return pElan;
      pElan = List_next(pElan);
  }
  return NULL;
}

static ElanInfoBlock* getNextElanBySNMPName (ConfigTable *pCfg,
					     OctetString* os,
					     boolean is_implied) { 
    ElanInfoBlock *pElan, *retval;
    int result, found;
    int len;

    found = 0;
    if (os == NULL)
	return NULL;
    
    if (pCfg == NULL)
	return NULL;
    
    pElan = List_head(pCfg->elanList);
    retval = pElan; /* initialize retval so its pointer is valid */
    
    while (pElan != NULL) {
	len = strlen(pElan->elanName);

	if (is_implied) {
	    result = memcmp(pElan->elanName, os->octet_ptr, len <
			    os->length ? len : os->length );

	    /* We want the entry closest to the search string */	
	    if ((result > 0) || ((result == 0) && (os->length < len))) {
		if ((!found) || (strcmp(pElan->elanName,
					retval->elanName) < 0)) {
		    retval = pElan;
		    found = 1; /* a success	or has been found */
		}
	    }
	} else {  /* Is not IMPLIED, length determines ordering */
	    if ((len > os->length) || 
		((len == os->length) && 
		 (memcmp(os->octet_ptr,	pElan->elanName, len) < 0))) {
		if (!found) {
		    retval = pElan;
		    found = 1;
		} else if (len < strlen(retval->elanName)) {
		    retval = pElan;
		} else if ((len == strlen(retval->elanName)) &&
			   (strcmp(pElan->elanName, retval->elanName)
			    < 0)) {
		    retval = pElan;
		}
	    } 
	}
	pElan = List_next(pElan);
    }
    
    if (found)
	return (retval);
    else
	return (NULL); /* off the end of the table */
}

lecsElanConfigEntry_t *
k_lecsElanConfigEntry_get (int serialNum, ContextInfo* contextInfo,
			   int nominator, int searchType, OctetString*
			   lecsConfigTblName, OctetString*
			   lecsElanConfigName) {
  static lecsElanConfigEntry_t lecsElanConfigEntryData;
  static OctetString elan_os, config_os, lesaddr_os;
  OctetString temp_os; 
  ElanInfoBlock *pElan;
  ConfigTable *pCfg;
  ServerAtmAddress *psaa;

  pCfg = getConfigTableBySNMPName(lecsConfigTblName);
  if (!pCfg)
      if (searchType == EXACT) 
	  return (NULL);
      else { 
	  pCfg = getNextConfigTableBySNMPName(lecsConfigTblName,
					      CONFIG_VARIABLE); 
	  lecsElanConfigName->length = 0;
	  if (!pCfg)
	      return NULL;
      }
  
  pElan = getElanBySNMPName(pCfg, lecsElanConfigName);
  if (!pElan)
      if (searchType == EXACT)
	  return (NULL);
      else 
	  pElan = getNextElanBySNMPName(pCfg, lecsElanConfigName, TRUE);
  
  if (!pElan) {
      lecsElanConfigName->length = 0;
      while (pCfg){
	  temp_os.octet_ptr = pCfg->tableName;
	  temp_os.length = strlen (pCfg->tableName);
	  pCfg = getNextConfigTableBySNMPName (&temp_os, CONFIG_VARIABLE);
	  pElan = getNextElanBySNMPName(pCfg, lecsElanConfigName, TRUE);
	  
	  if (pElan)
	      break;
      } 
      if (!pCfg)
	  return NULL;
  }
  config_os.octet_ptr = pCfg->tableName;
  config_os.length = strlen(pCfg->tableName);
  lecsElanConfigEntryData.lecsConfigTblName = &config_os;
  elan_os.octet_ptr = pElan->elanName;
  elan_os.length =strlen(pElan->elanName);
  lecsElanConfigEntryData.lecsElanConfigName = &elan_os;
  psaa = pElan->activeLes;
  if (!psaa)
      psaa = List_head(pElan->lesAddressesList);
  lesaddr_os.octet_ptr = psaa->lesAddr.addr;
  lesaddr_os.length = STATIONLEN_ATMNSAP; 
  lecsElanConfigEntryData.lecsElanLesAtmAddr = &lesaddr_os;
  if (pElan->restricted)
      lecsElanConfigEntryData.lecsElanAccess =
	  D_lecsElanAccess_closed;
  else 
      lecsElanConfigEntryData.lecsElanAccess = D_lecsElanAccess_open;
  lecsElanConfigEntryData.lecsElanConfigStatus = ACTIVE;

  lecsElanConfigEntryData.lecsElanSegmentId = pElan->local_segment_id;
  
  return (&lecsElanConfigEntryData);
}

#ifdef SETS
int
  k_lecsElanConfigEntry_test (ObjectInfo* object, ObjectSyntax* value,
			      doList_t* dp, ContextInfo* contextInfo) {
  lecsElanConfigEntry_t *data;
  ConfigTable* pConfig;
  ElanInfoBlock* pElan;
  
  data = (lecsElanConfigEntry_t *)dp->data;
  pConfig = NULL;
  pElan = NULL;

  if (data->lecsElanConfigName->length > 32)
      return (NO_CREATION_ERROR);
  
  if (!IsDisplayString(data->lecsElanConfigName))
      return (NO_CREATION_ERROR);
  
  /* This implementation cannot handle NUL characters in it's ELAN names so
     we cannot accept them */
  
  if (contains_null(data->lecsElanConfigName))
      return(NO_CREATION_ERROR);
  
  if (!valid_configtab(data->lecsConfigTblName))	
      return (INCONSISTENT_NAME_ERROR);
  
  switch (object->nominator) {
      
#ifdef I_lecsElanLesAtmAddr
  case I_lecsElanLesAtmAddr:
      
      /* SR stack still does not check length */
      
      if (value->os_value->length != 20)
	  return (WRONG_LENGTH_ERROR);
      
      SET_VALID(I_lecsElanLesAtmAddr, data->valid);
    break;
#endif 
      
#ifdef I_lecsElanAccess
  case I_lecsElanAccess:
      if (value->sl_value == D_lecsElanAccess_closed) {
	  pConfig = getConfigTableBySNMPName(data->lecsConfigTblName);
	  pElan = getElanBySNMPName(pConfig, data->lecsElanConfigName);
	  if (pElan && (pElan == pConfig->defaultElan))
	      /* Cannot change the default ELAN to a closed ELAN */
	      return(INCONSISTENT_VALUE_ERROR);
      }
      break;
#endif
      
#ifdef I_lecsElanConfigStatus
  case I_lecsElanConfigStatus:
      
      /* The following states are not supported */
      
      if ((value->sl_value == D_lecsElanConfigStatus_createAndWait) || 
	  (value->sl_value == D_lecsElanConfigStatus_notInService))
	  return (WRONG_VALUE_ERROR);
      
      break;
#endif
  } /* switch */
  
  return NO_ERROR;
}

int
k_lecsElanConfigEntry_ready (ObjectInfo* object, ObjectSyntax* value,
			     doList_t* doHead, doList_t* dp) {

    switch (((lecsElanConfigEntry_t *) (dp->data))->lecsElanConfigStatus) {
    case ACTIVE:
	dp->state = ADD_MODIFY;
	break;
    case CREATE_AND_GO:
	if (VALID(I_lecsElanLesAtmAddr,
		  ((lecsElanConfigEntry_t *) (dp->data))->valid))
	    dp->state = ADD_MODIFY;
	break;
    case DESTROY:
	dp->state = DELETE;
    }
    
    return NO_ERROR;
}

int
k_lecsElanConfigEntry_set_defaults (doList_t* dp) {
    lecsElanConfigEntry_t *data;
    
    data = (lecsElanConfigEntry_t *) (dp->data);
    data->lecsElanAccess = D_lecsElanAccess_open;
    data->lecsElanConfigStatus = UNUSED;
    
    return NO_ERROR;
}

int
k_lecsElanConfigEntry_set (lecsElanConfigEntry_t* data, ContextInfo*
			   contextInfo, int function) {
    ConfigTable *pCfg;
    hwaddrtype lesAtmAddr;
    char* transfer;
    ElanRestrictedStatus elan_status;
    
    pCfg = getConfigTableBySNMPName(data->lecsConfigTblName);
    if (!pCfg)
	return(COMMIT_FAILED_ERROR);
    
    transfer = convert_to_null_terminated_string(data->lecsElanConfigName);
    
    lesAtmAddr.type = STATION_ATMNSAP;
    lesAtmAddr.length = STATIONLEN_ATMNSAP;
    memcpy(lesAtmAddr.addr, data->lecsElanLesAtmAddr->octet_ptr,
	   STATIONLEN_ATMNSAP);

    if (data->lecsElanAccess == D_lecsElanAccess_closed)
	elan_status = ELAN_RESTRICTED;
    else
	elan_status = ELAN_NOT_RESTRICTED;

    switch (data->lecsElanConfigStatus) {
    case CREATE_AND_GO:
    case ACTIVE:
	if (!ConfigTable_elanCreate(pCfg, transfer, &lesAtmAddr, 
	    LOWEST_PRIO, elan_status))
		return(COMMIT_FAILED_ERROR);
	break;
    case DESTROY:
	ConfigTable_destroyElanByName(pCfg, transfer);
	break;
    }
    
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecsElanConfigEntry_UNDO
/* add #define SR_lecsElanConfigEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsElanConfigEntry family.
 */
int
lecsElanConfigEntry_undo (doList_t* doHead, doList_t* doCur,
			  ContextInfo* contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsElanConfigEntry_UNDO */
#endif /* SR_SNMPv2 */

lecsMacConfigEntry_t *
k_lecsMacConfigEntry_get (int serialNum, ContextInfo* contextInfo, int
			  nominator, int searchType, OctetString*
			  lecsConfigTblName, OctetString*
			  lecsMacConfigAddress) {
    static lecsMacConfigEntry_t lecsMacConfigEntryData;
    static OctetString config_os, mac_os, elan_os;
    ConfigTable *pCfg;
    OctetString temp_os;
    LecInfoBlock *pLec;
    unsigned char mac_address[6];
    
    pLec = 0;
    memset(mac_address, 0, 6);
    memcpy(mac_address, lecsMacConfigAddress->octet_ptr, 
	   lecsMacConfigAddress->length);
    
    pCfg = getConfigTableBySNMPName(lecsConfigTblName);
    
    if (!pCfg)  
	if (searchType == EXACT) 
	    return NULL;
	else {
	    pCfg = getNextConfigTableBySNMPName(lecsConfigTblName,
						CONFIG_VARIABLE); 
	    if (!pCfg)
		return NULL;
	    memset(mac_address, 0, 6);
	}
    
    pLec = Index_searchElement(pCfg->macAddressIndex, mac_address, 
			       NULL);
    
    if (!pLec)
	if (searchType == EXACT)
	    return NULL;
	else 
	    pLec = Index_searchNextElement(pCfg->macAddressIndex, 
					   mac_address);
    
    if (!pLec) {
	memset(mac_address, 0, 6);
	
	while (pCfg) {
	    temp_os.octet_ptr = pCfg->tableName;
	    temp_os.length = strlen(pCfg->tableName);
	    pCfg = getNextConfigTableBySNMPName(&temp_os, CONFIG_VARIABLE);
	    
	    if (pCfg)
		pLec = Index_searchNextElement(pCfg->macAddressIndex, 
					       mac_address);
	    
	    if (pLec)
		break;
	    
	}
	
	if (!pCfg)
	    return NULL; /* end of view */
    }
    
    config_os.octet_ptr = pCfg->tableName;
    config_os.length = strlen(pCfg->tableName);
    lecsMacConfigEntryData.lecsConfigTblName = &config_os;
    mac_os.octet_ptr  = pLec->address.dataBytes;
    mac_os.length = STATIONLEN_IEEE48;
    lecsMacConfigEntryData.lecsMacConfigAddress = &mac_os;
    elan_os.octet_ptr = pLec->parentElan->elanName;
    elan_os.length = strlen(pLec->parentElan->elanName);
    lecsMacConfigEntryData.lecsMacConfigElanName = &elan_os;
    lecsMacConfigEntryData.lecsMacConfigLastUsed =
	SNMP_ABSOLUTE_TIME(pLec->lastAccessTime);
    lecsMacConfigEntryData.lecsMacConfigStatus = ACTIVE;
    
    return (&lecsMacConfigEntryData);
}

#ifdef SETS
int
k_lecsMacConfigEntry_test (ObjectInfo* object, ObjectSyntax* value,
			   doList_t* dp, ContextInfo* contextInfo) {
  lecsMacConfigEntry_t* data;

  data = (lecsMacConfigEntry_t*) dp->data;
  if (!valid_configtab(data->lecsConfigTblName))
      return (INCONSISTENT_NAME_ERROR);
  
  switch (object->nominator) {
      
#ifdef I_lecsMacConfigElanName
  case I_lecsMacConfigElanName:
      
      if (value->os_value->length > 32)
	  return (WRONG_LENGTH_ERROR);
      
      if (!k_lecsElanConfigEntry_get(-1, contextInfo, object->nominator, 
				     EXACT, data->lecsConfigTblName,
				     value->os_value)) 
	  return (INCONSISTENT_VALUE_ERROR);
      
      SET_VALID(I_lecsMacConfigElanName, data->valid);
      break;
#endif
      
#ifdef I_lecsMacConfigStatus
  case I_lecsMacConfigStatus:
      
      if ((value->sl_value == D_lecsMacConfigStatus_createAndWait) || 
	  (value->sl_value == D_lecsMacConfigStatus_notInService))
	  return (WRONG_VALUE_ERROR);
      
      break;
#endif
  }
  
  return NO_ERROR;
}

int
k_lecsMacConfigEntry_ready (ObjectInfo* object, ObjectSyntax* value,
			    doList_t* doHead, doList_t* dp) {
    switch (((lecsMacConfigEntry_t *) (dp->data))->lecsMacConfigStatus) {
    case ACTIVE:
	dp->state = ADD_MODIFY;
	break;
    case CREATE_AND_GO:
	if (VALID(I_lecsMacConfigElanName,
		  ((lecsMacConfigEntry_t *) (dp->data))->valid))
	    dp->state = ADD_MODIFY;
	break;
    case DESTROY:
	dp->state = DELETE;
    }
    
    return NO_ERROR;
}

int
k_lecsMacConfigEntry_set_defaults (doList_t* dp) {
    lecsMacConfigEntry_t *data;
    lecsConfigTblEntry_t* ct;
    
    data = (lecsMacConfigEntry_t *) (dp->data);
    ct = k_lecsConfigTblEntry_get(-1, NULL, -1, EXACT,
				  data->lecsConfigTblName); 
    if (ct && ct->lecsConfigTblDefaultElanName) {
	data->lecsMacConfigElanName =
	    CloneOctetString(ct->lecsConfigTblDefaultElanName); 
	SET_VALID(I_lecsMacConfigElanName, data->valid);
    }
    
    data->lecsMacConfigLastUsed = snmp_sysUpTime();
    SET_VALID(I_lecsMacConfigLastUsed, data->valid);
    
    data->lecsMacConfigStatus = UNUSED;
    
    return NO_ERROR;
}

int
k_lecsMacConfigEntry_set (lecsMacConfigEntry_t* data, ContextInfo*
			  contextInfo, int function) {
  ConfigTable *pCfg;
  LecInfoBlock *pLec;
  ElanInfoBlock *pElan; 
  MaskedAddress RegisteredMacAddress;
  char* transfer;

  maskedAddress_setMacValues(&RegisteredMacAddress, 
			     data->lecsMacConfigAddress->octet_ptr);
  
  pCfg = getConfigTableBySNMPName(data->lecsConfigTblName);
  if (!pCfg)
      return (COMMIT_FAILED_ERROR);
  
  switch (data->lecsMacConfigStatus) {
  case ACTIVE:
      if (VALID(I_lecsMacConfigElanName, data->valid)) {
	  pLec = Index_searchElement(pCfg->macAddressIndex, 
				     &RegisteredMacAddress, NULL);
	  if (!pLec)
	      return (COMMIT_FAILED_ERROR);
	  pElan = getElanBySNMPName(pCfg, data->lecsMacConfigElanName);
	  if (!pElan)
	      return (COMMIT_FAILED_ERROR);
	  pLec->parentElan = pElan;
      }
      break;
  case CREATE_AND_GO:
      transfer =
	  convert_to_null_terminated_string(data->lecsMacConfigElanName); 
      if (!ConfigTable_addLec(pCfg, transfer, &RegisteredMacAddress))
	  return (COMMIT_FAILED_ERROR);
      break;
  case DESTROY:
      ConfigTable_destroyLecByMacAddr(pCfg, &RegisteredMacAddress);
      break;
  default:
      return (GEN_ERROR);
  }
  return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecsMacConfigEntry_UNDO
/* add #define SR_lecsMacConfigEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsMacConfigEntry family.
 */
int
lecsMacConfigEntry_undo (doList_t* doHead, doList_t* doCur,
			 ContextInfo* contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsMacConfigEntry_UNDO */
#endif /* SR_SNMPv2 */

/* This routine scans the ATM address table within the config table for
   a match to this particular ATM address.  
   */

static LecInfoBlock*
k_lecsAtmAddrConfigEntry_getExact (ConfigTable* pCfg, MaskedAddress*
				   address) {
    LecInfoBlock* pLec;
    int position;
    
    position = 1;
    
    if (allOnesMask(address->maskBytes, address->length)) {
	pLec = Index_searchElement(pCfg->UNmaskedAtmAddressIndex,
				   address->dataBytes, NULL);
	return (pLec);
    } 
    
    pLec = Index_getElementPointer(pCfg->maskedAtmAddressIndex, 0);
    while (pLec) {
	if (memcmp(pLec->address.dataBytes, address->dataBytes, 
		   address->length) == 0)
	    if (memcmp(pLec->address.maskBytes, address->maskBytes, 
		       STATIONLEN_ATMNSAP) == 0)
		return(pLec);
	pLec = Index_getElementPointer(pCfg->maskedAtmAddressIndex,
				       position++); 
    }
    
    return (NULL);
}

/* This routine is necessary since the list of ATM addresses in the LECS is
   not ordered according to address and mask but only a combination of the
   two.  Also note that the mask bits of the atmAddr cannot be used in this
   routine as this bits are not guarenteed to be contiguous in a Get Next
   packet. 
   */
static LecInfoBlock *k_lecsAtmAddrConfigEntry_getNext (ConfigTable *pCfg, 
						MaskedAddress *atmAddr) {
    LecInfoBlock *pLec, *next;
    int element, result;
    
    next = 0;
    result = 0;
    element = 0;
    pLec = Index_getElementPointer(pCfg->UNmaskedAtmAddressIndex, element);
    while (pLec) {
	
	result = memcmp(pLec->address.dataBytes, atmAddr->dataBytes, 
			pLec->address.length);
	if (result > 0)
	    if ((!next) || (memcmp(next->address.dataBytes, 
				   pLec->address.dataBytes, 
				   STATIONLEN_ATMNSAP) > 0)) 
		next = pLec;
	
	element++;
	pLec = Index_getElementPointer(pCfg->UNmaskedAtmAddressIndex, element);
    }
    
    element = 0;
    pLec = Index_getElementPointer(pCfg->maskedAtmAddressIndex,
				   element);
    while (pLec) {
	
	result = memcmp(pLec->address.dataBytes, atmAddr->dataBytes, 
			pLec->address.length);
	if (result >= 0)
	    if (result == 0) {
		if (memcmp(pLec->address.maskBytes, atmAddr->maskBytes,
			   STATIONLEN_ATMNSAP) <= 0)
		    goto skip_to_next;
		if ((!next) || (memcmp(next->address.maskBytes, 
				       pLec->address.maskBytes,
				       STATIONLEN_ATMNSAP) > 0)) 
		    next = pLec;
	    } else 
		if ((!next) || (memcmp(next->address.dataBytes, 
				       pLec->address.dataBytes, 
				       STATIONLEN_ATMNSAP) > 0)) 
		    next = pLec;

skip_to_next:	
	element++;
	pLec = Index_getElementPointer(pCfg->maskedAtmAddressIndex, element);
    }
    
    return (next);
}

/* Since the ATM addresses are only ordered as a function of ATM address 
   and mask (but not each individually).  We must do a search over the
   whole list to find the least lexicographical entry.
   */

static LecInfoBlock*
k_lecsAtmAddrConfigEntry_getFirst (ConfigTable* pCfg) {
    LecInfoBlock *pLec, *first;
    int element, result;
    
    pLec = 0;
    first = 0;
    element = 0;
    
    first = Index_getElementPointer(pCfg->UNmaskedAtmAddressIndex, 0);
    for (; pLec; pLec = Index_getElementPointer(pCfg->UNmaskedAtmAddressIndex,
						element++)) {
	result = memcmp(pLec->address.dataBytes,
			first->address.dataBytes,
			first->address.length);
	if (result > 0)
	    continue;
	
	first = pLec;
    }
    
    for (; pLec; pLec =
	 Index_getElementPointer(pCfg->maskedAtmAddressIndex,
				 element++)) {
	result = memcmp(pLec->address.dataBytes,
			first->address.dataBytes, 
			first->address.length);
	if (result > 0) 
	    continue;
	
	if ((result == 0) && (memcmp(pLec->address.maskBytes, 
				     first->address.maskBytes,
				     first->address.length) >= 0))
	    continue;
	
	first = pLec;
    }
    
    return(first);
}

lecsAtmAddrConfigEntry_t *
  k_lecsAtmAddrConfigEntry_get (int serialNum, ContextInfo*
				contextInfo, int nominator, int
				searchType, OctetString*
				lecsConfigTblName, OctetString*
				lecsAtmAddrConfigAddress, OctetString*
				lecsAtmAddrConfigMask) {
      static lecsAtmAddrConfigEntry_t lecsAtmAddrConfigEntryData;
      static OctetString config_os, addr_os, mask_os, elan_os;
      MaskedAddress lecAtmAddress;
      ConfigTable *pCfg;
      LecInfoBlock* pLec;
      OctetString temp_os;
      
      pLec = 0;
      memcpy(lecAtmAddress.dataBytes,
	     lecsAtmAddrConfigAddress->octet_ptr, 
	     STATIONLEN_ATMNSAP);
      memcpy(lecAtmAddress.maskBytes, lecsAtmAddrConfigMask->octet_ptr,
	     STATIONLEN_ATMNSAP);
      lecAtmAddress.length = STATIONLEN_ATMNSAP;
      
      pCfg = getConfigTableBySNMPName(lecsConfigTblName);
      
      if (!pCfg) {
	  if (searchType == EXACT) 
	      return NULL;
	  else { 
	      pCfg = getNextConfigTableBySNMPName(lecsConfigTblName, 
						  CONFIG_VARIABLE);
	      if (!pCfg)
		  return NULL;
	      /* Normally the below bit mask would be invalid.  However, it
		 is quite appropiate for use in get-next traversal. */
	      memset(lecAtmAddress.dataBytes, 0, STATIONLEN_ATMNSAP);
	      memset(lecAtmAddress.maskBytes, 0, STATIONLEN_ATMNSAP);
	  }
      }
      
      pLec = k_lecsAtmAddrConfigEntry_getExact(pCfg, &lecAtmAddress);
      
      if (!pLec) 
	  if (searchType == EXACT)
	      return NULL;
	  else 
	      pLec = k_lecsAtmAddrConfigEntry_getNext(pCfg, &lecAtmAddress);
      
      if (!pLec) {
	  temp_os.octet_ptr = pCfg->tableName;
	  temp_os.length = strlen(pCfg->tableName);
	  pCfg = getNextConfigTableBySNMPName(&temp_os, CONFIG_VARIABLE);
	  
	  while (pCfg) {
	      pLec = k_lecsAtmAddrConfigEntry_getFirst(pCfg);
	      
	      if (pLec)
		  break;
	      
	      temp_os.octet_ptr = pCfg->tableName;
	      temp_os.length = strlen(pCfg->tableName);
	      pCfg = getNextConfigTableBySNMPName(&temp_os, CONFIG_VARIABLE);
	  }
	  if (!pCfg)
	      return NULL;
      }
      
      config_os.octet_ptr = pCfg->tableName;
      config_os.length = strlen (pCfg->tableName);
      lecsAtmAddrConfigEntryData.lecsConfigTblName = &config_os;
      addr_os.octet_ptr = pLec->address.dataBytes;
      addr_os.length = STATIONLEN_ATMNSAP; 
      lecsAtmAddrConfigEntryData.lecsAtmAddrConfigAddress = &addr_os;
      mask_os.octet_ptr = pLec->address.maskBytes;
      mask_os.length = STATIONLEN_ATMNSAP; 
      lecsAtmAddrConfigEntryData.lecsAtmAddrConfigMask = &mask_os;
      elan_os.octet_ptr = pLec->parentElan->elanName;
      elan_os.length = strlen(pLec->parentElan->elanName);
      lecsAtmAddrConfigEntryData.lecsAtmAddrConfigElanName = &elan_os;
      lecsAtmAddrConfigEntryData.lecsAtmAddrLastUsed =
	  SNMP_ABSOLUTE_TIME(pLec->lastAccessTime);
      lecsAtmAddrConfigEntryData.lecsAtmAddrConfigStatus = ACTIVE;
      
      return (&lecsAtmAddrConfigEntryData);
  }

#ifdef SETS
int
k_lecsAtmAddrConfigEntry_test (ObjectInfo* object, ObjectSyntax*
			       value, doList_t* dp, ContextInfo*
			       contextInfo) {
    int count, valid_mask;
    lecsAtmAddrConfigEntry_t *data;
    
    data = (lecsAtmAddrConfigEntry_t *)dp->data;
    
    for (count = 0; count < 20; count++)
	if ((valid_mask = data->lecsAtmAddrConfigMask->octet_ptr[count]))
	    break;
      
    if (!valid_mask)
	return (NO_CREATION_ERROR);
    
    if (!valid_configtab(data->lecsConfigTblName))
	return (INCONSISTENT_NAME_ERROR);
    
    switch(object->nominator) {
	
#ifdef I_lecsAtmAddrConfigElanName
    case I_lecsAtmAddrConfigElanName:
	
	if (value->os_value->length > 32)
	    return (WRONG_LENGTH_ERROR);
	
	if (!k_lecsElanConfigEntry_get(-1, contextInfo,
				       object->nominator, EXACT, 
				       data->lecsConfigTblName,
				       value->os_value)) 
	    return (INCONSISTENT_VALUE_ERROR);
	
	SET_VALID(I_lecsAtmAddrConfigElanName, data->valid);
	break;
#endif
	
#ifdef I_lecsAtmAddrConfigStatus
    case I_lecsAtmAddrConfigStatus:
	
	/* The following states are not supported */
	
	if ((value->sl_value == D_lecsAtmAddrConfigStatus_createAndWait) || 
	    (value->sl_value == D_lecsAtmAddrStatus_notInService))
	    return (WRONG_VALUE_ERROR);
	
	break;
#endif
    }
    
    return NO_ERROR;
}

int
k_lecsAtmAddrConfigEntry_ready (ObjectInfo* object, ObjectSyntax*
				value, doList_t* doHead, doList_t* dp) {

    switch (((lecsAtmAddrConfigEntry_t *)
	     (dp->data))->lecsAtmAddrConfigStatus) {
    case ACTIVE:
	dp->state = ADD_MODIFY;
	break;
    case CREATE_AND_GO:
	if (VALID(I_lecsAtmAddrConfigElanName,
		  ((lecsAtmAddrConfigEntry_t *) (dp->data))->valid))
	    dp->state = ADD_MODIFY;
	break;
    case DESTROY:
	dp->state = DELETE;
    }
    
    return NO_ERROR;
}

int
k_lecsAtmAddrConfigEntry_set_defaults (doList_t* dp) {
    lecsAtmAddrConfigEntry_t *data;
    lecsConfigTblEntry_t* ct;
    
    data = (lecsAtmAddrConfigEntry_t *) (dp->data);
    ct = k_lecsConfigTblEntry_get(-1, NULL, -1, EXACT,
				  data->lecsConfigTblName); 

    if (ct && ct->lecsConfigTblDefaultElanName) {
	data->lecsAtmAddrConfigElanName =
	    CloneOctetString(ct->lecsConfigTblDefaultElanName); 
	SET_VALID(I_lecsAtmAddrConfigElanName, data->valid);
    }
    
    data->lecsAtmAddrLastUsed = snmp_sysUpTime();
    SET_VALID(I_lecsAtmAddrLastUsed, data->valid);
    
    data->lecsAtmAddrConfigStatus = UNUSED;
    
    return NO_ERROR;
}

int
k_lecsAtmAddrConfigEntry_set (lecsAtmAddrConfigEntry_t* data,
			      ContextInfo* contextInfo, int function) {
    ConfigTable *pCfg;
    LecInfoBlock *pLec;
    ElanInfoBlock *pElan; 
    MaskedAddress RegisteredAtmAddress;
    char* transfer;
    
    maskedAddress_setAtmValues(&RegisteredAtmAddress, 
			       data->lecsAtmAddrConfigAddress->octet_ptr, 
			       data->lecsAtmAddrConfigMask->octet_ptr);
    
    pCfg = getConfigTableBySNMPName(data->lecsConfigTblName);
    if (!pCfg)
	return (COMMIT_FAILED_ERROR);
    
    switch (data->lecsAtmAddrConfigStatus) {
    case ACTIVE:
	if (VALID(I_lecsAtmAddrConfigElanName, data->valid)) {
	    pLec = k_lecsAtmAddrConfigEntry_getExact(pCfg, 
						     &RegisteredAtmAddress);
	    if (!pLec)
		return (COMMIT_FAILED_ERROR);
	    
	    pElan = getElanBySNMPName(pCfg, data->lecsAtmAddrConfigElanName);
	    if (!pElan)
		return (COMMIT_FAILED_ERROR);
	    pLec->parentElan = pElan;
	}
	break;
    case CREATE_AND_GO:
	transfer =
	    convert_to_null_terminated_string(data->lecsAtmAddrConfigElanName);
	if (!ConfigTable_addLec(pCfg, transfer, &RegisteredAtmAddress))
	    return (COMMIT_FAILED_ERROR);
	break;
    case DESTROY:
	ConfigTable_destroyLecByAtmAddr(pCfg, &RegisteredAtmAddress);
	break;
    default:
	return (GEN_ERROR);
    }
    
    return (NO_ERROR);
    
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecsAtmAddrConfigEntry_UNDO
/* add #define SR_lecsAtmAddrConfigEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsAtmAddrConfigEntry family.
 */
int
lecsAtmAddrConfigEntry_undo (doList_t* doHead, doList_t* doCur,
			     ContextInfo* contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsAtmAddrConfigEntry_UNDO */
#endif /* SR_SNMPv2 */

static ServerAtmAddress* getLesByAddress (ElanInfoBlock* pElan,
					  OctetString* address, 
					  int searchType) {
    ServerAtmAddress *psaa, *next;
    int result;

    next = NULL;
    psaa = List_head(pElan->lesAddressesList);
    while (psaa) {
	result = memcmp(psaa->lesAddr.addr, address->octet_ptr,
			STATIONLEN_ATMNSAP);
	if (result == 0)
	    return psaa;
	if (result > 0)
	    if ((!next) || (memcmp(psaa->lesAddr.addr,
				   next->lesAddr.addr,
				   STATIONLEN_ATMNSAP) < 0))
		next = psaa;
	psaa = List_next(psaa);
    }
    if (searchType == EXACT)
	return NULL;
    return next;
}
    
lecsLesConfigEntry_t *
k_lecsLesConfigEntry_get (int serialNum, ContextInfo* contextInfo, int
			  nominator, int searchType, OctetString*
			  lecsConfigTblName, OctetString*
			  lecsElanConfigName, OctetString*
			  lecsLesAtmAddr) {
    static lecsLesConfigEntry_t lecsLesConfigEntryData;
    static OctetString elan_os, config_os, lesaddr_os;
    ElanInfoBlock *pElan;
    ConfigTable *pCfg;
    ServerAtmAddress *psaa;
    long server_state;
   
    pCfg = NULL;
    pElan = NULL;
    psaa = NULL;

    config_os.octet_ptr = lecsConfigTblName->octet_ptr;
    config_os.length = lecsConfigTblName->length;
    elan_os.octet_ptr = lecsElanConfigName->octet_ptr;
    elan_os.length = lecsElanConfigName->length;
    lesaddr_os.octet_ptr = lecsLesAtmAddr->octet_ptr;
    lesaddr_os.length = lecsLesAtmAddr->length;
    if (searchType == EXACT) {
	pCfg = getConfigTableBySNMPName(lecsConfigTblName);
	if (!pCfg)
	    return NULL;
	pElan = getElanBySNMPName(pCfg, lecsElanConfigName);
	if (!pElan)
	    return NULL;
	psaa = getLesByAddress(pElan, lecsLesAtmAddr, EXACT);
	if (!psaa)
	    return NULL;
    } else {             /* NEXT */
	pCfg = getConfigTableBySNMPName(lecsConfigTblName);
	if (pCfg) 
	    pElan = getElanBySNMPName(pCfg, lecsElanConfigName);
	if (pElan)
	    psaa = getLesByAddress(pElan, lecsLesAtmAddr, EXACT);
	while (!psaa) {
	    while (!pElan) {
		while (!pCfg) {
		    pCfg = getNextConfigTableBySNMPName(&config_os,
							CONFIG_VARIABLE);
		    if (!pCfg)
			return NULL;
		    config_os.octet_ptr = pCfg->tableName;
		    config_os.length = strlen(pCfg->tableName);
		    elan_os.length = 0;

		}
		pElan = getNextElanBySNMPName(pCfg, &elan_os, FALSE);
		if (!pElan) {
		    pCfg = NULL;
		    continue;
		}
		elan_os.octet_ptr = pElan->elanName;
		elan_os.length =strlen(pElan->elanName);
		memset(lesaddr_os.octet_ptr, 0, STATIONLEN_ATMNSAP);
	    }
	    psaa = getLesByAddress(pElan, &lesaddr_os, NEXT);
	    if (!psaa) {
		pElan = NULL;
		continue;
	    }
	    lesaddr_os.octet_ptr = psaa->lesAddr.addr;
	    lesaddr_os.length = STATIONLEN_ATMNSAP;
	}
    }
    
    lecsLesConfigEntryData.lecsLesAtmAddr = &lesaddr_os;
    lecsLesConfigEntryData.lecsLesPriority = psaa->priority;
    if (psaa == pElan->activeLes)
	server_state = D_lecsLesConnState_active;
    else if (psaa->connPtr)
	server_state = D_lecsLesConnState_inactive;
    else 
	server_state = D_lecsLesConnState_notConnected;
    lecsLesConfigEntryData.lecsLesConnState = server_state;
    lecsLesConfigEntryData.lecsLesConfigStatus = D_lecsLesConnState_active;
    lecsLesConfigEntryData.lecsConfigTblName = &config_os;
    lecsLesConfigEntryData.lecsElanConfigName = &elan_os;
    return(&lecsLesConfigEntryData);
}

#ifdef SETS
int k_lecsLesConfigEntry_test (ObjectInfo* object, ObjectSyntax*
			       value, doList_t* dp, ContextInfo*
			       contextInfo) {
    lecsLesConfigEntry_t *data = (lecsLesConfigEntry_t *) (dp->data);
    ConfigTable *pCfg;    

    pCfg = getConfigTableBySNMPName(data->lecsConfigTblName);
    if (!pCfg)
	return(INCONSISTENT_NAME_ERROR);

    /* The following two states are not supported */
    if (object->nominator == I_lecsLesConfigStatus)
	if ((data->lecsLesConfigStatus ==
	    D_lecsLesConfigStatus_createAndWait) ||
	    (data->lecsLesConfigStatus ==
	     D_lecsLesConfigStatus_notInService)) 
	    return WRONG_VALUE_ERROR;

    return NO_ERROR;
}

int
k_lecsLesConfigEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_lecsLesConfigEntry_set_defaults(dp)
    doList_t       *dp;
{
    lecsLesConfigEntry_t *data = (lecsLesConfigEntry_t *) (dp->data);

    data->lecsLesPriority = 0;
    return NO_ERROR;
}

int k_lecsLesConfigEntry_set (lecsLesConfigEntry_t* data, ContextInfo*
			      contextInfo, int function) {
    ConfigTable *pCfg;
    hwaddrtype lesAtmAddr;
    char* transfer;
    
    pCfg = getConfigTableBySNMPName(data->lecsConfigTblName);
    if (!pCfg)
	return(COMMIT_FAILED_ERROR);
    
    transfer = convert_to_null_terminated_string(data->lecsElanConfigName);
    
    lesAtmAddr.type = STATION_ATMNSAP;
    lesAtmAddr.length = STATIONLEN_ATMNSAP;
    memcpy(lesAtmAddr.addr, data->lecsLesAtmAddr->octet_ptr,
	   STATIONLEN_ATMNSAP);

    switch (data->lecsLesConfigStatus) {
    case CREATE_AND_GO:
    case ACTIVE:
	if (!ConfigTable_elanCreate(pCfg, transfer, &lesAtmAddr, 
	    data->lecsLesPriority, ELAN_NO_CHANGE))
		return(COMMIT_FAILED_ERROR);
	break;
    case DESTROY:
	deleteElan_AndOr_ServerAtmAddress(pCfg, transfer, &lesAtmAddr);
	break;
    }
    
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecsLesConfigEntry_UNDO
/* add #define SR_lecsLesConfigEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsLesConfigEntry family.
 */
int
lecsLesConfigEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsLesConfigEntry_UNDO */
#endif /* SR_SNMPv2 */

lecsRDConfigEntry_t *
k_lecsRDConfigEntry_get (int serialNum, ContextInfo* contextInfo, 
			 int nominator, int searchType, 
			 OctetString* lecsConfigTblName, 
			 SR_INT32 lecsRDConfigSegmentId, 
			 SR_INT32 lecsRDConfigBridgeNum) {
   static lecsRDConfigEntry_t lecsRDConfigEntryData;
   static OctetString config_os, elan_os;
   OctetString temp_os;
   ConfigTable *pCfg;
   LecInfoBlock *pLec;
   ushort route_desc;
   MaskedAddress masked;

   pCfg = NULL;
   pLec = NULL;

   pCfg = getConfigTableBySNMPName(lecsConfigTblName);
   if (searchType == EXACT) {
      if (!pCfg)
	     return NULL;
   } else {             /* NEXT */
		pCfg = getNextConfigTableBySNMPName(lecsConfigTblName,
							CONFIG_VARIABLE);
		if (!pCfg)
		  return NULL;
      route_desc = 0;
	}
    
   /*
    * find the route descriptor in the RD Index of the config table
    */
   route_desc = (lecsRDConfigSegmentId << 4) | (lecsRDConfigBridgeNum & 0x0F);
   maskedAddress_setRDValues(&masked, &route_desc);
   pLec = Index_searchElement(pCfg->rdIndex, masked.dataBytes, NULL);
   if (!pLec) {
	  if (searchType == EXACT)
	    return NULL;
	  else 
	    pLec = Index_searchNextElement(pCfg->rdIndex, &route_desc);
   }
    
   if (!pLec)  {
	   route_desc = 0; 
	
      while (pCfg) {
         temp_os.octet_ptr = pCfg->tableName;
         temp_os.length = strlen(pCfg->tableName);
         pCfg = getNextConfigTableBySNMPName(&temp_os, CONFIG_VARIABLE);

         if (pCfg)
            pLec = Index_searchNextElement(pCfg->rdIndex, &route_desc);

         if (pLec)
            break;
      }

      if (!pCfg)
         return NULL; /* end of view */
   }

   config_os.octet_ptr = pCfg->tableName;
   config_os.length = strlen(pCfg->tableName);

      /* found route descriptor */ 
   lecsRDConfigEntryData.lecsRDConfigSegmentId = lecsRDConfigSegmentId;
   lecsRDConfigEntryData.lecsRDConfigBridgeNum =  lecsRDConfigBridgeNum;
 
   elan_os.octet_ptr = pLec->parentElan->elanName;
   elan_os.length = strlen(pLec->parentElan->elanName);
   lecsRDConfigEntryData.lecsRDConfigElanName = &elan_os;
 
   lecsRDConfigEntryData.lecsRDConfigLastUsed = 
	      SNMP_ABSOLUTE_TIME(pLec->lastAccessTime);
   lecsRDConfigEntryData.lecsRDConfigStatus = ACTIVE;
   lecsRDConfigEntryData.lecsConfigTblName = &config_os;
   return(&lecsRDConfigEntryData);
}

#ifdef SETS
int
k_lecsRDConfigEntry_test (ObjectInfo* object, ObjectSyntax* value,
			  doList_t* dp, ContextInfo* contextInfo) {
    return NO_ERROR;
}

int
k_lecsRDConfigEntry_ready (ObjectInfo* object, ObjectSyntax* value,
			   doList_t* doHead, doList_t* dp) {
    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_lecsRDConfigEntry_set_defaults (doList_t* dp) {
    lecsRDConfigEntry_t *data = (lecsRDConfigEntry_t *) (dp->data);

    if ((data->lecsRDConfigElanName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_lecsRDConfigEntry_set (lecsRDConfigEntry_t *data, 
			 ContextInfo* contextInfo, int function) {
    return GEN_ERROR;
}

#ifdef SR_lecsRDConfigEntry_UNDO
/* add #define SR_lecsRDConfigEntry_UNDO in lecsmib.h to
 * include the undo routine for the lecsRDConfigEntry family.
 */
int
lecsRDConfigEntry_undo (doList_t* doHead, doList_t* doCur,
			ContextInfo* contextInfo) {
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecsRDConfigEntry_UNDO */

#endif /* SETS */

static void
init_lecsmib (subsystype* subsys)
{
    load_mib(lecsmib_OidList, lecsmib_OidListNum);
    load_oid(lecsmib_oid_table);
}

/*
 * lecsmib subsystem header
 */

#define MAJVERSION_lecsmib 1
#define MINVERSION_lecsmib 0
#define EDITVERSION_lecsmib 0

SUBSYS_HEADER(lecsmib,
              MAJVERSION_lecsmib,
              MINVERSION_lecsmib,
              EDITVERSION_lecsmib,
              init_lecsmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: lane",
              NULL);


      
