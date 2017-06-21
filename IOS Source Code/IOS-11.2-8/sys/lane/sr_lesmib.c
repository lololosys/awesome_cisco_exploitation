/* $Id: sr_lesmib.c,v 3.8.8.8 1996/08/12 20:05:28 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/lane/sr_lesmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1995, Christopher Young (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lesmib.c,v $
 * Revision 3.8.8.8  1996/08/12  20:05:28  cyoung
 * Add support for more than one LES per ELAN.
 * CSCdi62514:  LES for same ELAN on different physical interfaces missing
 * from MIB
 * Branch: California_branch
 *
 * Revision 3.8.8.7  1996/07/01  18:45:33  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8.8.6  1996/05/22  20:58:32  kjadams
 * CSCdi58286:  snmp management of TRLANE specific fields in LES mib
 * Branch: California_branch
 * fill in TRLANE specific fields.
 *
 * Revision 3.8.8.5  1996/05/21  09:54:33  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.8.8.4  1996/05/21  06:34:18  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.8.8.3  1996/05/09  14:33:42  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.8.2.2.1  1996/04/27  07:07:28  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.8.2  1996/04/25  23:15:29  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.8.8.1.8.2  1996/04/08  14:52:45  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.8.8.1.8.1  1996/04/08  01:58:33  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.8.8.1  1996/03/18  20:33:12  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.12.2  1996/03/25  02:20:16  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8.12.1  1996/03/22  22:56:04  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.4.4  1996/03/13  17:46:37  cyoung
 * Add skeleton code for the _get routines for the lesRDRegEntry.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7.4.3  1996/03/07  02:29:53  cyoung
 * Add lesSegmentID to LES MIB.
 * Remove redundant code.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7.4.2  1996/02/29  22:16:18  cakyol
 * correct some slight merge errors and clean up stuff related
 * to config access, which somehow must not have been totally
 * cleaned before.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7.4.1  1996/02/27  21:11:18  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7  1996/02/03  02:28:39  cyoung
 * Take notice of the config access code.  Reset LECID and MAC addresses
 * when switching LESs.  Fixed length MAC address should not have a length
 * sub-id.
 * CSCdi48108:  LES MIB fails to report LECS address and all LECs
 *
 * Revision 3.6  1996/01/24  21:18:23  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.5  1996/01/18  15:50:47  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4.2.2  1996/02/22  09:05:58  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.1  1996/01/11  01:47:13  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4  1996/01/05  04:02:07  cyoung
 * Error in table traversal logic.
 * CSCdi46481:  not all LESs appearing in LES MIB
 *
 * Revision 3.3.10.1  1995/12/29  01:07:25  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:52:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:28  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:24:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:13:03  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/13  03:05:30  cyoung
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
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_lesmib.h"
#include "sr_lesmib-mib.h"
#include "lecs.h"
#include "interface.h"
#include "interface_private.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"
#include "address.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../atm/atmsig_api.h"
#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "lsv_private.h"


/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the lesEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lesEntry_get (OID             *incoming,
	     ObjectInfo      *object,
	     int             searchType,
	     ContextInfo     *contextInfo,
	     int             serialNum)
{
    int             arg;
    void            *dp;
    lesEntry_t      *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lesElanName;
    int             lesElanName_offset;
    long            lesIndex;
    int             lesIndex_offset;
    int             index;
    int             final_index;

    arg = object->nominator;
    data = 0;
    lesElanName = NULL;
    
    lesElanName_offset = object->oid.length;
    lesIndex_offset = lesElanName_offset +
	GetVariableIndexLength(incoming, lesElanName_offset); 
    final_index = lesIndex_offset + 1;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, lesIndex_offset, &lesIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesIndex = MAX(1, lesIndex);
    }
    
    if ( (InstToVariableOctetString(incoming, lesElanName_offset,
				    &lesElanName, searchType, &carry))
	< 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lesEntry_get(serialNum, contextInfo,
					       arg ,searchType,
					       lesElanName, lesIndex))
	== NULL) { 
        arg = -1;
    }
    
    else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lesElanName->length;
        for(i = 0; i < data->lesElanName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lesElanName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->lesIndex;
        inst.length = index;
    }
    
    FreeOctetString(lesElanName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lesAtmAddrSpec
    case I_lesAtmAddrSpec:
	dp = (void *) (CloneOctetString(data->lesAtmAddrSpec));
	break;
#endif /* I_lesAtmAddrSpec */
	
#ifdef I_lesAtmAddrMask
    case I_lesAtmAddrMask:
	dp = (void *) (CloneOctetString(data->lesAtmAddrMask));
	break;
#endif /* I_lesAtmAddrMask */
	
#ifdef I_lesAtmAddrActual
    case I_lesAtmAddrActual:
	dp = (void *) (CloneOctetString(data->lesAtmAddrActual));
	break;
#endif /* I_lesAtmAddrActual */
	
#ifdef I_lesIfIndex
    case I_lesIfIndex:
	dp = (void *) (&data->lesIfIndex);
	break;
#endif /* I_lesIfIndex */
	
#ifdef I_lesSubIfNum
    case I_lesSubIfNum:
	dp = (void *) (&data->lesSubIfNum);
	break;
#endif /* I_lesSubIfNum */
	
#ifdef I_lesColocBusAtmAddrSpec
    case I_lesColocBusAtmAddrSpec:
	dp = (void *) (CloneOctetString(data->lesColocBusAtmAddrSpec));
	break;
#endif /* I_lesColocBusAtmAddrSpec */
	
#ifdef I_lesColocBusAtmAddrMask
    case I_lesColocBusAtmAddrMask:
	dp = (void *) (CloneOctetString(data->lesColocBusAtmAddrMask));
	break;
#endif /* I_lesColocBusAtmAddrMask */
	
#ifdef I_lesColocBusAtmAddrActl
    case I_lesColocBusAtmAddrActl:
	dp = (void *) (CloneOctetString(data->lesColocBusAtmAddrActl));
	break;
#endif /* I_lesColocBusAtmAddrActl */
	
#ifdef I_lesUpTime
    case I_lesUpTime:
	dp = (void *) (&data->lesUpTime);
	break;
#endif /* I_lesUpTime */
	
#ifdef I_lesLanType
    case I_lesLanType:
	dp = (void *) (&data->lesLanType);
	break;
#endif /* I_lesLanType */
	
#ifdef I_lesMaxFrm
    case I_lesMaxFrm:
	dp = (void *) (&data->lesMaxFrm);
	break;
#endif /* I_lesMaxFrm */
	
#ifdef I_lesJoinTimeout
    case I_lesJoinTimeout:
	dp = (void *) (&data->lesJoinTimeout);
	break;
#endif /* I_lesJoinTimeout */
	
#ifdef I_lesLecsAtmAddr
    case I_lesLecsAtmAddr:
	dp = (void *) (CloneOctetString(data->lesLecsAtmAddr));
	break;
#endif /* I_lesLecsAtmAddr */
	
#ifdef I_lesControlDistVpi
    case I_lesControlDistVpi:
	dp = (void *) (&data->lesControlDistVpi);
	break;
#endif /* I_lesControlDistVpi */
	
#ifdef I_lesControlDistVci
    case I_lesControlDistVci:
	dp = (void *) (&data->lesControlDistVci);
	break;
#endif /* I_lesControlDistVci */
	
#ifdef I_lesOperStatus
    case I_lesOperStatus:
	dp = (void *) (&data->lesOperStatus);
	break;
#endif /* I_lesOperStatus */
	
#ifdef I_lesAdminStatus
    case I_lesAdminStatus:
	dp = (void *) (&data->lesAdminStatus);
	break;
#endif /* I_lesAdminStatus */
	
#ifdef I_lesStatus
    case I_lesStatus:
	dp = (void *) (&data->lesStatus);
	break;
#endif /* I_lesStatus */

#ifdef I_lesSegmentID
    case I_lesSegmentID:
	dp = (void *) (&data->lesSegmentID);
	break;
#endif /* I_lesSegmentID */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lesEntry data object.
 *---------------------------------------------------------------------*/
void
lesEntry_free (lesEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lesElanName);
	FreeOctetString(data->lesAtmAddrSpec);
	FreeOctetString(data->lesAtmAddrMask);
	FreeOctetString(data->lesAtmAddrActual);
	FreeOctetString(data->lesColocBusAtmAddrSpec);
	FreeOctetString(data->lesColocBusAtmAddrMask);
	FreeOctetString(data->lesColocBusAtmAddrActl);
	FreeOctetString(data->lesLecsAtmAddr);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lesEntry set/undo
 *---------------------------------------------------------------------*/
static int
lesEntry_cleanup (doList_t *trash)
{
    lesEntry_free(trash->data);
#ifdef SR_SNMPv2
    lesEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lesEntry family
 *---------------------------------------------------------------------*/
lesEntry_t *
Clone_lesEntry (lesEntry_t* lesEntry)
{
    lesEntry_t *data;
    
    if ((data = malloc(sizeof(lesEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lesEntry), sizeof(lesEntry_t));
    
    data->lesElanName = CloneOctetString(lesEntry->lesElanName);
    data->lesAtmAddrSpec = CloneOctetString(lesEntry->lesAtmAddrSpec);
    data->lesAtmAddrMask = CloneOctetString(lesEntry->lesAtmAddrMask);
    data->lesAtmAddrActual = CloneOctetString(lesEntry->lesAtmAddrActual);
    data->lesColocBusAtmAddrSpec =
	CloneOctetString(lesEntry->lesColocBusAtmAddrSpec); 
    data->lesColocBusAtmAddrMask =
	CloneOctetString(lesEntry->lesColocBusAtmAddrMask); 
    data->lesColocBusAtmAddrActl =
	CloneOctetString(lesEntry->lesColocBusAtmAddrActl); 
    data->lesLecsAtmAddr = CloneOctetString(lesEntry->lesLecsAtmAddr);
    
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
lesEntry_test (
	       OID            *incoming,
	       ObjectInfo     *object,
	       ObjectSyntax   *value,
	       doList_t       *doHead,
	       doList_t       *doCur,
	       ContextInfo    *contextInfo)
{
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lesEntry_t     *lesEntry;
    OctetString *  lesElanName;
    int            lesElanName_offset;
    long           lesIndex;
    int            lesIndex_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    lesElanName = NULL;

    /*
     * Validate the object instance
     *
     */
    lesElanName_offset = object->oid.length;
    lesIndex_offset = lesElanName_offset +
	GetVariableIndexLength(incoming, lesElanName_offset); 
    final_index = lesIndex_offset + 1;
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming, lesElanName_offset, &lesElanName, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, lesIndex_offset, &lesIndex, EXACT,
		    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lesElanName);
        return error_status;
    }
    
    lesEntry = k_lesEntry_get(-1, contextInfo, -1, EXACT, lesElanName,
			      lesIndex); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lesEntry_set) &&
            (((lesEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lesEntry_t *) (dp->data))->lesElanName,
			     lesElanName) == 0) && 
            (((lesEntry_t *) (dp->data))->lesIndex == lesIndex) ) {
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lesEntry_set;
        dp->cleanupMethod = lesEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lesEntry_UNDO
        dp->undoMethod = lesEntry_undo;
#else /* SR_lesEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lesEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lesEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_lesEntry(lesEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_lesEntry(lesEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(lesEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lesEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lesEntry_t *) (dp->data))->lesElanName =
		    CloneOctetString(lesElanName); 
                SET_VALID(I_lesElanName, ((lesEntry_t *) (dp->data))->valid);
		
                ((lesEntry_t *) (dp->data))->lesIndex = (lesIndex);
                SET_VALID(I_lesIndex, ((lesEntry_t *) (dp->data))->valid);
		
                error_status = k_lesEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(lesElanName);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lesAtmAddrSpec
    case I_lesAtmAddrSpec:
	
	if (((lesEntry_t *) (dp->data))->lesAtmAddrSpec != NULL) {
	    FreeOctetString(((lesEntry_t *) (dp->data))->lesAtmAddrSpec);
	}
	
	((lesEntry_t *) (dp->data))->lesAtmAddrSpec = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lesAtmAddrSpec */
	
#ifdef I_lesAtmAddrMask
    case I_lesAtmAddrMask:
	
	if (((lesEntry_t *) (dp->data))->lesAtmAddrMask != NULL) {
	    FreeOctetString(((lesEntry_t *) (dp->data))->lesAtmAddrMask);
	}
	
	((lesEntry_t *) (dp->data))->lesAtmAddrMask = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lesAtmAddrMask */
	
#ifdef I_lesIfIndex
    case I_lesIfIndex:
	
	((lesEntry_t *) (dp->data))->lesIfIndex = value->sl_value;
	break;
#endif /* I_lesIfIndex */
	
#ifdef I_lesSubIfNum
    case I_lesSubIfNum:
	
	((lesEntry_t *) (dp->data))->lesSubIfNum = value->sl_value;
	break;
#endif /* I_lesSubIfNum */
	
#ifdef I_lesColocBusAtmAddrSpec
    case I_lesColocBusAtmAddrSpec:
	
	if (((lesEntry_t *) (dp->data))->lesColocBusAtmAddrSpec != NULL) {
	    FreeOctetString(((lesEntry_t *)
			     (dp->data))->lesColocBusAtmAddrSpec); 
	}
	
	((lesEntry_t *) (dp->data))->lesColocBusAtmAddrSpec = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lesColocBusAtmAddrSpec */
	
#ifdef I_lesColocBusAtmAddrMask
    case I_lesColocBusAtmAddrMask:
	
	if (((lesEntry_t *) (dp->data))->lesColocBusAtmAddrMask != NULL) {
	    FreeOctetString(((lesEntry_t *)
			     (dp->data))->lesColocBusAtmAddrMask); 
	}
	
	((lesEntry_t *) (dp->data))->lesColocBusAtmAddrMask = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lesColocBusAtmAddrMask */
	
#ifdef I_lesLanType
    case I_lesLanType:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lesEntry_t *) (dp->data))->lesLanType = value->sl_value;
	break;
#endif /* I_lesLanType */
	
#ifdef I_lesMaxFrm
    case I_lesMaxFrm:
	
	switch (value->sl_value) {
        case 1516:
        case 4544:
        case 9234:
        case 18190:
            break;
        default:
            return WRONG_VALUE_ERROR;
	}
	
	((lesEntry_t *) (dp->data))->lesMaxFrm = value->sl_value;
	break;
#endif /* I_lesMaxFrm */
	
#ifdef I_lesJoinTimeout
    case I_lesJoinTimeout:
	
	if ( ((value->sl_value < 10) || (value->sl_value > 300)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lesEntry_t *) (dp->data))->lesJoinTimeout = value->sl_value;
	break;
#endif /* I_lesJoinTimeout */
	
#ifdef I_lesLecsAtmAddr
    case I_lesLecsAtmAddr:
	
	if (((lesEntry_t *) (dp->data))->lesLecsAtmAddr != NULL) {
	    FreeOctetString(((lesEntry_t *) (dp->data))->lesLecsAtmAddr);
	}
	
	((lesEntry_t *) (dp->data))->lesLecsAtmAddr = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_lesLecsAtmAddr */
	
#ifdef I_lesAdminStatus
    case I_lesAdminStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lesEntry_t *) (dp->data))->lesAdminStatus = value->sl_value;
	break;
#endif /* I_lesAdminStatus */
	
#ifdef I_lesStatus
    case I_lesStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lesStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lesEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lesStatus_notInService) ||
	       (value->sl_value == D_lesStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lesStatus_createAndGo) ||
		(value->sl_value == D_lesStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lesEntry_t *) (dp->data))->lesStatus = value->sl_value;
	break;
#endif /* I_lesStatus */

#ifdef I_lesSegmentID
    case I_lesSegmentID:
	
	if ((value->sl_value < 0) || (value->sl_value > 4095)) {
	    return WRONG_VALUE_ERROR;
	}

	((lesEntry_t *) (dp->data))->lesSegmentID = value->sl_value;
	break;
#endif /* I_lesSegmentID */

    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lesEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lesEntry_test */
    error_status = k_lesEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lesEntry_t *) (dp->data))->valid);
        error_status = k_lesEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lesEntry_set (
	      doList_t       *doHead,
	      doList_t       *doCur,
	      ContextInfo    *contextInfo)
{
    return (k_lesEntry_set((lesEntry_t *) (doCur->data),
			   contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lesStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lesStatsEntry_get ( 
		   OID             *incoming,
		   ObjectInfo      *object,
		   int             searchType,
		   ContextInfo     *contextInfo,
		   int             serialNum)
{
    int             arg;
    void            *dp;
    lesStatsEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lesElanName;
    int             lesElanName_offset;
    long            lesIndex;
    int             lesIndex_offset;
    int             index;
    int             final_index;
    
    arg = object->nominator;
    data = 0;
    lesElanName = NULL;

    lesElanName_offset = object->oid.length;
    lesIndex_offset = lesElanName_offset +
	GetVariableIndexLength(incoming, lesElanName_offset); 
    final_index = lesIndex_offset + 1;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, lesIndex_offset, &lesIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesIndex = MAX(1, lesIndex);
    }
    
    if ( (InstToVariableOctetString(incoming, lesElanName_offset,
				    &lesElanName, searchType, &carry))
	< 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lesStatsEntry_get(serialNum,
						    contextInfo, arg
						    ,searchType,
						    lesElanName,
						    lesIndex)) ==
	NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lesElanName->length;
        for(i = 0; i < data->lesElanName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lesElanName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->lesIndex;
        inst.length = index;
    }
    
    FreeOctetString(lesElanName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lesInErrors
    case I_lesInErrors:
	dp = (void *) (&data->lesInErrors);
	break;
#endif /* I_lesInErrors */
	
#ifdef I_lesInErrorLastLec
    case I_lesInErrorLastLec:
	dp = (void *) (CloneOctetString(data->lesInErrorLastLec));
	break;
#endif /* I_lesInErrorLastLec */
	
#ifdef I_lesInFlushReplies
    case I_lesInFlushReplies:
	dp = (void *) (&data->lesInFlushReplies);
	break;
#endif /* I_lesInFlushReplies */
	
#ifdef I_lesInJoinReqs
    case I_lesInJoinReqs:
	dp = (void *) (&data->lesInJoinReqs);
	break;
#endif /* I_lesInJoinReqs */
	
#ifdef I_lesOutJoinFails
    case I_lesOutJoinFails:
	dp = (void *) (&data->lesOutJoinFails);
	break;
#endif /* I_lesOutJoinFails */
	
#ifdef I_lesJoinLastFailCause
    case I_lesJoinLastFailCause:
	dp = (void *) (&data->lesJoinLastFailCause);
	break;
#endif /* I_lesJoinLastFailCause */
	
#ifdef I_lesJoinLastFailLec
    case I_lesJoinLastFailLec:
	dp = (void *) (CloneOctetString(data->lesJoinLastFailLec));
	break;
#endif /* I_lesJoinLastFailLec */
	
#ifdef I_lesOutConfigReqs
    case I_lesOutConfigReqs:
	dp = (void *) (&data->lesOutConfigReqs);
	break;
#endif /* I_lesOutConfigReqs */
	
#ifdef I_lesInConfigResps
    case I_lesInConfigResps:
	dp = (void *) (&data->lesInConfigResps);
	break;
#endif /* I_lesInConfigResps */
	
#ifdef I_lesInConfigFails
    case I_lesInConfigFails:
	dp = (void *) (&data->lesInConfigFails);
	break;
#endif /* I_lesInConfigFails */
	
#ifdef I_lesInRegisReqs
    case I_lesInRegisReqs:
	dp = (void *) (&data->lesInRegisReqs);
	break;
#endif /* I_lesInRegisReqs */
	
#ifdef I_lesOutRegisFails
    case I_lesOutRegisFails:
	dp = (void *) (&data->lesOutRegisFails);
	break;
#endif /* I_lesOutRegisFails */
	
#ifdef I_lesRegisLastFailCause
    case I_lesRegisLastFailCause:
	dp = (void *) (&data->lesRegisLastFailCause);
	break;
#endif /* I_lesRegisLastFailCause */
	
#ifdef I_lesRegisLastFailLec
    case I_lesRegisLastFailLec:
	dp = (void *) (CloneOctetString(data->lesRegisLastFailLec));
	break;
#endif /* I_lesRegisLastFailLec */
	
#ifdef I_lesInUnregReqs
    case I_lesInUnregReqs:
	dp = (void *) (&data->lesInUnregReqs);
	break;
#endif /* I_lesInUnregReqs */
	
#ifdef I_lesInLearpUcasts
    case I_lesInLearpUcasts:
	dp = (void *) (&data->lesInLearpUcasts);
	break;
#endif /* I_lesInLearpUcasts */
	
#ifdef I_lesInLearpBroadcasts
    case I_lesInLearpBroadcasts:
	dp = (void *) (&data->lesInLearpBroadcasts);
	break;
#endif /* I_lesInLearpBroadcasts */
	
#ifdef I_lesOutLearpFwd
    case I_lesOutLearpFwd:
	dp = (void *) (&data->lesOutLearpFwd);
	break;
#endif /* I_lesOutLearpFwd */
	
#ifdef I_lesInLearpResps
    case I_lesInLearpResps:
	dp = (void *) (&data->lesInLearpResps);
	break;
#endif /* I_lesInLearpResps */
	
#ifdef I_lesInNarpReqs
    case I_lesInNarpReqs:
	dp = (void *) (&data->lesInNarpReqs);
	break;
#endif /* I_lesInNarpReqs */
	
#ifdef I_lesInTopolReqs
    case I_lesInTopolReqs:
	dp = (void *) (&data->lesInTopolReqs);
	break;
#endif /* I_lesInTopolReqs */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lesClientEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lesClientEntry_get (
		    OID             *incoming,
		    ObjectInfo      *object,
		    int             searchType,
		    ContextInfo     *contextInfo,
		    int             serialNum)
{
    int             arg;
    void            *dp;
    lesClientEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lesElanName;
    int             lesElanName_offset;
    long            lesIndex;
    int             lesIndex_offset;
    long            lesClientLecid;
    int             lesClientLecid_offset;
    int             index;
    int             final_index;
    
    arg = object->nominator;
    data = 0;
    lesElanName = NULL;

    lesElanName_offset = object->oid.length;
    lesIndex_offset = lesElanName_offset +
	GetVariableIndexLength(incoming, lesElanName_offset); 
    lesClientLecid_offset = lesIndex_offset + 1;
    final_index = lesClientLecid_offset + 1;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, lesClientLecid_offset, &lesClientLecid,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesClientLecid = MAX(1, lesClientLecid);
    }
    
    if ( (InstToInt(incoming, lesIndex_offset, &lesIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesIndex = MAX(1, lesIndex);
    }
    
    if ( (InstToVariableOctetString(incoming, lesElanName_offset,
				    &lesElanName, searchType, &carry))
	< 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lesClientEntry_get(serialNum,
						     contextInfo, arg
						     ,searchType,
						     lesElanName,
						     lesIndex,
						     lesClientLecid))
	== NULL) { 
        arg = -1;
    }
    
    else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lesElanName->length;
        for(i = 0; i < data->lesElanName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lesElanName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->lesIndex;
        if (index > MAX_OID_SIZE) {
            arg = -1;
            index--;
        }
        inst.oid_ptr[index++] = (unsigned long) data->lesClientLecid;
        inst.length = index;
    }
    
    FreeOctetString(lesElanName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lesClientAtmAddr
    case I_lesClientAtmAddr:
	dp = (void *) (CloneOctetString(data->lesClientAtmAddr));
	break;
#endif /* I_lesClientAtmAddr */
	    
#ifdef I_lesClientState
	case I_lesClientState:
	    dp = (void *) (&data->lesClientState);
	break;
#endif /* I_lesClientState */
	
#ifdef I_lesClientIfIndex
    case I_lesClientIfIndex:
	dp = (void *) (&data->lesClientIfIndex);
	break;
#endif /* I_lesClientIfIndex */
	
#ifdef I_lesClientControlVpi
    case I_lesClientControlVpi:
	dp = (void *) (&data->lesClientControlVpi);
	break;
#endif /* I_lesClientControlVpi */
	
#ifdef I_lesClientControlVci
    case I_lesClientControlVci:
	dp = (void *) (&data->lesClientControlVci);
	break;
#endif /* I_lesClientControlVci */
	
#ifdef I_lesClientStatus
    case I_lesClientStatus:
	dp = (void *) (&data->lesClientStatus);
	break;
#endif /* I_lesClientStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lesClientEntry data object.
 *---------------------------------------------------------------------*/
void
lesClientEntry_free (lesClientEntry_t *data) {
    if (data != NULL) {
	FreeOctetString(data->lesClientAtmAddr);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lesClientEntry set/undo
 *---------------------------------------------------------------------*/
static int
lesClientEntry_cleanup(doList_t *trash) {
    lesClientEntry_free(trash->data);
#ifdef SR_SNMPv2
    lesClientEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lesClientEntry family
 *---------------------------------------------------------------------*/
lesClientEntry_t *
Clone_lesClientEntry (lesClientEntry_t *lesClientEntry) {
    lesClientEntry_t *data;
    
    if ((data = malloc(sizeof(lesClientEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lesClientEntry),
	   sizeof(lesClientEntry_t)); 
    
    data->lesClientAtmAddr =
	CloneOctetString(lesClientEntry->lesClientAtmAddr); 
    
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
lesClientEntry_test (
		     OID            *incoming,
		     ObjectInfo     *object,
		     ObjectSyntax   *value,
		     doList_t       *doHead,
		     doList_t       *doCur,
		     ContextInfo    *contextInfo)
{
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lesClientEntry_t     *lesClientEntry;
    OctetString *  lesElanName;
    int            lesElanName_offset;
    long           lesIndex;
    int            lesIndex_offset;
    long           lesClientLecid;
    int            lesClientLecid_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    lesElanName = NULL;

    /*
     * Validate the object instance
     *
     */
    lesElanName_offset = object->oid.length;
    lesIndex_offset = lesElanName_offset +
	GetVariableIndexLength(incoming, lesElanName_offset); 
    lesClientLecid_offset = lesIndex_offset + 1;
    final_index = lesClientLecid_offset + 1;
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming, lesElanName_offset,
				    &lesElanName, EXACT, &carry)) < 0
	) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, lesIndex_offset, &lesIndex, EXACT,
		    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, lesClientLecid_offset, &lesClientLecid,
		    EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lesElanName);
        return error_status;
    }
    
    lesClientEntry = k_lesClientEntry_get(-1, contextInfo, -1, EXACT,
					  lesElanName, lesIndex,
					  lesClientLecid); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lesClientEntry_set) &&
            (((lesClientEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((lesClientEntry_t *)
			      (dp->data))->lesElanName, lesElanName)
	     == 0) && 
            (((lesClientEntry_t *) (dp->data))->lesIndex == lesIndex) &&
            (((lesClientEntry_t *) (dp->data))->lesClientLecid ==
	     lesClientLecid) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lesClientEntry_set;
        dp->cleanupMethod = lesClientEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lesClientEntry_UNDO
        dp->undoMethod = lesClientEntry_undo;
#else /* SR_lesClientEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lesClientEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lesClientEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lesClientEntry(lesClientEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lesClientEntry(lesClientEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
	    
#ifdef lesClientEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(lesClientEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(lesClientEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lesClientEntry_t *) (dp->data))->lesElanName =
		    CloneOctetString(lesElanName); 
                SET_VALID(I_lesElanName, ((lesClientEntry_t *)
					  (dp->data))->valid); 
		
                ((lesClientEntry_t *) (dp->data))->lesIndex = (lesIndex);
                SET_VALID(I_lesIndex, ((lesClientEntry_t *)
				       (dp->data))->valid); 
		
                ((lesClientEntry_t *) (dp->data))->lesClientLecid =
		    (lesClientLecid); 
                SET_VALID(I_lesClientLecid, ((lesClientEntry_t *)
					     (dp->data))->valid); 
		
                error_status = k_lesClientEntry_set_defaults(dp);
            }
#else /* lesClientEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* lesClientEntry_READ_CREATE */
        }
    }
    
    FreeOctetString(lesElanName);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lesClientStatus
    case I_lesClientStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lesClientStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lesClientEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lesClientStatus_notInService) ||
	       (value->sl_value == D_lesClientStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lesClientStatus_createAndGo) ||
		(value->sl_value == D_lesClientStatus_createAndWait))
		{ 
		    return INCONSISTENT_VALUE_ERROR;
		}
	}
	
	((lesClientEntry_t *) (dp->data))->lesClientStatus = value->sl_value;
	break;
#endif /* I_lesClientStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lesClientEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lesClientEntry_test */
    error_status = k_lesClientEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lesClientEntry_t *) (dp->data))->valid);
        error_status = k_lesClientEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lesClientEntry_set (
		    doList_t       *doHead,
		    doList_t       *doCur,
		    ContextInfo    *contextInfo) 
{
    return (k_lesClientEntry_set((lesClientEntry_t *) (doCur->data),
				 contextInfo, doCur->state));
}
#endif /* SETS */

/*---------------------------------------------------------------------
 * Retrieve data from the lesMacRegEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lesMacRegEntry_get (
		    OID             *incoming,
		    ObjectInfo      *object,
		    int             searchType,
		    ContextInfo     *contextInfo,
		    int             serialNum)
{
    int             arg;
    void            *dp;
    lesMacRegEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lesElanName;
    int             lesElanName_offset;
    long            lesIndex;
    int             lesIndex_offset;
    OctetString *   lesMacRegMacAddress;
    int             lesMacRegMacAddress_offset;
    int             index;
    int             final_index;
    
    arg = object->nominator;
    data = 0;
    lesElanName = NULL;
    lesMacRegMacAddress = NULL;

    lesElanName_offset = object->oid.length;
    lesIndex_offset = lesElanName_offset +
	GetVariableIndexLength(incoming, lesElanName_offset); 
    lesMacRegMacAddress_offset = lesIndex_offset + 1;
    final_index = lesMacRegMacAddress_offset + 6;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToFixedOctetString(incoming,
				    lesMacRegMacAddress_offset,
				    &lesMacRegMacAddress, searchType,
				    &carry, 6)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToInt(incoming, lesIndex_offset, &lesIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesIndex = MAX(1, lesIndex);
    }
    
    if ( (InstToVariableOctetString(incoming, lesElanName_offset,
				    &lesElanName, searchType, &carry))
	< 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lesMacRegEntry_get(serialNum,
						     contextInfo, arg
						     ,searchType,
						     lesElanName,
						     lesIndex,
						     lesMacRegMacAddress)) == NULL) {
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lesElanName->length;
        for(i = 0; i < data->lesElanName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lesElanName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->lesIndex;
        if (index > MAX_OID_SIZE) {
            arg = -1;
            index--;
        }
        for(i = 0; i < data->lesMacRegMacAddress->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lesMacRegMacAddress->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
    }
    
    FreeOctetString(lesElanName);
    FreeOctetString(lesMacRegMacAddress);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lesMacRegAtmAddr
    case I_lesMacRegAtmAddr:
	dp = (void *) (CloneOctetString(data->lesMacRegAtmAddr));
	break;
#endif /* I_lesMacRegAtmAddr */
	
#ifdef I_lesMacRegLecid
    case I_lesMacRegLecid:
	dp = (void *) (&data->lesMacRegLecid);
	break;
#endif /* I_lesMacRegLecid */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lesRDRegEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lesRDRegEntry_get (OID* incoming, ObjectInfo* object, int searchType,
		   ContextInfo* contextInfo, int serialNum) {
    
    int             arg;
    void            *dp;
    lesRDRegEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   lesElanName;
    int             lesElanName_offset;
    long            lesIndex;
    int             lesIndex_offset;
    long            lesRDRegSegmentId;
    int             lesRDRegSegmentId_offset;
    long            lesRDRegBridgeNum;
    int             lesRDRegBridgeNum_offset;
    int             index;
    int             final_index;

    data = NULL;
    arg = object->nominator;
    lesElanName = NULL;
    lesElanName_offset = object->oid.length;
    lesIndex_offset = lesElanName_offset +
	GetVariableIndexLength(incoming, lesElanName_offset); 
    lesRDRegSegmentId_offset = lesIndex_offset + 1;
    lesRDRegBridgeNum_offset = lesRDRegSegmentId_offset + 1;
    final_index = lesRDRegBridgeNum_offset + 1;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, lesRDRegBridgeNum_offset,
		    &lesRDRegBridgeNum, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesRDRegBridgeNum = MAX(0, lesRDRegBridgeNum);
    }
    
    if ( (InstToInt(incoming, lesRDRegSegmentId_offset,
		    &lesRDRegSegmentId, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesRDRegSegmentId = MAX(0, lesRDRegSegmentId);
    }
    
    if ( (InstToInt(incoming, lesIndex_offset, &lesIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lesIndex = MAX(1, lesIndex);
    }
    
    if ( (InstToVariableOctetString(incoming, lesElanName_offset,
				    &lesElanName, searchType, &carry))
	< 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lesRDRegEntry_get(serialNum,
						    contextInfo, arg
						    ,searchType,
						    lesElanName,
						    lesIndex,
						    lesRDRegSegmentId, lesRDRegBridgeNum)) == NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->lesElanName->length;
        for(i = 0; i < data->lesElanName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lesElanName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->lesIndex;
        if (index > MAX_OID_SIZE) {
            arg = -1;
            index--;
        }
        inst.oid_ptr[index++] = (unsigned long) data->lesRDRegSegmentId;
        if (index > MAX_OID_SIZE) {
            arg = -1;
            index--;
        }
        inst.oid_ptr[index++] = (unsigned long) data->lesRDRegBridgeNum;
        inst.length = index;
    }
    
    FreeOctetString(lesElanName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lesRDRegAtmAddr
    case I_lesRDRegAtmAddr:
	dp = (void *) (CloneOctetString(data->lesRDRegAtmAddr));
       break;
#endif /* I_lesRDRegAtmAddr */
	
#ifdef I_lesRDRegLecid
    case I_lesRDRegLecid:
	dp = (void *) (&data->lesRDRegLecid);
	break;
#endif /* I_lesRDRegLecid */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*
 * Start of system-dependent routines
 */



void* get_lesbus (OctetString* elan_name, long index, int searchType) {
    
    lsv_info_t* lsinfo, *next_ls;
    int result;
    
    next_ls = 0;
    for (lsinfo = lsv_all_lesbus; lsinfo; lsinfo = lsinfo->next) {
	if (!lsinfo->lane_info || !lsinfo->lane_info->type_from_les)
	    continue;
	if (lsinfo->lane_info->name_size != elan_name->length) {
	    if (lsinfo->lane_info->name_size > elan_name->length)
		if (!next_ls || (next_ls->lane_info->name_size >
				 lsinfo->lane_info->name_size)) 
		    next_ls = lsinfo;
		else 
		    if (next_ls->lane_info->name_size ==
			lsinfo->lane_info->name_size) {
			result = memcmp(next_ls->lane_info->name,
					lsinfo->lane_info->name,
					lsinfo->lane_info->name_size);
			if (result > 0) 
			    next_ls = lsinfo;
			if ((result == 0) && (next_ls->lesIndex >
					      lsinfo->lesIndex)) 
			    next_ls = lsinfo;
		    }
	    continue;
	}
	result = memcmp(lsinfo->lane_info->name, elan_name->octet_ptr, 
			lsinfo->lane_info->name_size);
	if (result == 0) {
	    if (index == lsinfo->lesIndex)
		break;
	    if ((index < lsinfo->lesIndex) && 
		((!next_ls) || (next_ls->lesIndex >
				lsinfo->lesIndex)))
		next_ls = lsinfo;
	}
	else if (result > 0)
	    if (!next_ls || (next_ls->lane_info->name_size >
			     lsinfo->lane_info->name_size)) {
		next_ls = lsinfo;
	    } else 
		if (next_ls->lane_info->name_size ==
		    lsinfo->lane_info->name_size) {
		    result = memcmp(next_ls->lane_info->name,
				    lsinfo->lane_info->name,
				    lsinfo->lane_info->name_size);
		    if (result > 0)
			next_ls = lsinfo;
		    if ((result == 0) && (next_ls->lesIndex >
					  lsinfo->lesIndex)) 
			next_ls = lsinfo;
		}
    }
    
    if (!lsinfo) 
	if (searchType == EXACT)
	    return NULL;
	else
	    if (!next_ls)
		return NULL;
	    else 
		lsinfo = next_ls;
    
    return (lsinfo);
}

lesEntry_t *
k_lesEntry_get (
		int serialNum,
		ContextInfo *contextInfo,
		int nominator,
		int searchType,
		OctetString * lesElanName,
		long lesIndex)
{
    static lesEntry_t lesEntryData;
    static OctetString elan_os;
    static OctetString busAddrSpec_os, busAddrMask_os, busAddrActl_os;
    static OctetString lesAddrSpec_os, lesAddrMask_os, lesAddrActl_os;
    static OctetString lecsAddr_os;
    lsv_info_t* lsinfo;
    long port, vpi, vci;
    boolean vc_exists;
    
    vc_exists = FALSE;
    lsinfo = get_lesbus(lesElanName, lesIndex, searchType);
    
    if ((!lsinfo) || (!lsinfo->lane_info))
	return (NULL);
    
    SET_ALL_VALID(lesEntryData.valid);
    
    elan_os.octet_ptr = lsinfo->lane_info->name;
    elan_os.length = lsinfo->lane_info->name_size;
    lesEntryData.lesElanName = &elan_os;
    lesEntryData.lesIndex = lsinfo->lesIndex;
    lesAddrSpec_os.octet_ptr = lsinfo->lane_info->lsv_nsap.dataBytes;
    lesAddrSpec_os.length = lsinfo->lane_info->lsv_nsap.length;
    if (lesAddrSpec_os.length == 0)
	CLR_VALID(I_lesAtmAddrSpec, lesEntryData.valid);
    lesEntryData.lesAtmAddrSpec = &lesAddrSpec_os;
    lesAddrMask_os.octet_ptr = lsinfo->lane_info->lsv_nsap.maskBytes;
    lesAddrMask_os.length = lsinfo->lane_info->lsv_nsap.length;
    lesEntryData.lesAtmAddrMask = &lesAddrMask_os;
    lesAddrActl_os.octet_ptr = lsinfo->lsv_nsap.addr;
    lesAddrActl_os.length = lsinfo->lsv_nsap.length;
    lesEntryData.lesAtmAddrActual = &lesAddrActl_os;
    if (lsinfo->lane_info->swidb) {
	lesEntryData.lesSubIfNum =
	    lsinfo->lane_info->swidb->sub_number;
	
	if (lsinfo->lane_info->swidb->hwptr) {
	    lesEntryData.lesIfIndex = 
		lsinfo->lane_info->swidb->hwptr->snmp_if_index;
	    vc_exists = atm_getvc_mib_id(lsinfo->distribute_vcc.vcd,
					 lsinfo->lane_info->swidb->hwptr, 
					 &port, &vpi, &vci);
	} else {
	    lesEntryData.lesIfIndex = 0;
	    CLR_VALID(I_lesIfIndex, lesEntryData.valid);
	}
	
    } else {
	lesEntryData.lesIfIndex = 0; /* should never happen */
	CLR_VALID(I_lesIfIndex, lesEntryData.valid);
	lesEntryData.lesSubIfNum = 0;
	CLR_VALID(I_lesSubIfNum, lesEntryData.valid);
    }
    
    busAddrSpec_os.octet_ptr = lsinfo->lane_info->bus_nsap.dataBytes;
    busAddrSpec_os.length = lsinfo->lane_info->bus_nsap.length;
    if (busAddrSpec_os.length == 0)
	CLR_VALID(I_lesColocBusAtmAddrSpec, lesEntryData.valid);
    lesEntryData.lesColocBusAtmAddrSpec = &busAddrSpec_os;
    
    busAddrMask_os.octet_ptr = lsinfo->lane_info->bus_nsap.maskBytes;
    busAddrMask_os.length = lsinfo->lane_info->bus_nsap.length;
    lesEntryData.lesColocBusAtmAddrMask = &busAddrMask_os;
    
    busAddrActl_os.octet_ptr = lsinfo->bus_nsap.addr;
    busAddrActl_os.length = lsinfo->bus_nsap.length;
    lesEntryData.lesColocBusAtmAddrActl = &busAddrActl_os ;
    
    lesEntryData.lesUpTime = SNMP_ABSOLUTE_TIME(lsinfo->start_time);
    
    lesEntryData.lesLanType = D_lesLanType_dot3;
    
    switch(lsinfo->lane_info->max_data_frame) {
    case LANE_MAX_FRAME_1516:
	lesEntryData.lesMaxFrm = 1516;
	break;
    case LANE_MAX_FRAME_4544:
	lesEntryData.lesMaxFrm = 4544;
	break;
    case LANE_MAX_FRAME_9234:
	lesEntryData.lesMaxFrm = 9234;
	break;
    case LANE_MAX_FRAME_18190:
	lesEntryData.lesMaxFrm = 18190;
	break;
    }
    
    lesEntryData.lesJoinTimeout = lsinfo->S4_control_timeout;
    lecsAddr_os.octet_ptr = lsinfo->lsv_lecs_nsap.addr;
    lecsAddr_os.length = lsinfo->lsv_lecs_nsap.length;
    if (lecsAddr_os.length == 0)
	CLR_VALID(I_lesLecsAtmAddr, lesEntryData.valid);
    lesEntryData.lesLecsAtmAddr = &lecsAddr_os;
    if (vc_exists) {
	lesEntryData.lesControlDistVpi = vpi;
	lesEntryData.lesControlDistVci = vci;
    } else {
	lesEntryData.lesControlDistVpi = LES_VCC_INFO_INVALID;
	lesEntryData.lesControlDistVci = LES_VCC_INFO_INVALID;
    }
    
    lesEntryData.lesOperStatus = lsinfo->server_state ==
	LSV_LESBUS_OPERATIONAL ? D_lesOperStatus_active :
	    D_lesOperStatus_inactive;
    lesEntryData.lesAdminStatus = lsinfo->admin_state ==
	LSV_LESBUS_ADMIN_UP ? D_lesAdminStatus_active :
	    D_lesAdminStatus_inactive; 
    
    lesEntryData.lesStatus = D_lesStatus_active;
    lesEntryData.lesSegmentID = lsinfo->seg_id;
    return(&lesEntryData);
}

#ifdef SETS
int
k_lesEntry_test (
		 ObjectInfo     *object,
		 ObjectSyntax   *value,
		 doList_t       *dp,
		 ContextInfo    *contextInfo) 
{
    lesEntry_t *data;
    snmpidbtype *snmpidb;
    idbtype* swidb;
    
    data = (lesEntry_t *)dp->data;
    swidb = NULL;

    if (data->lesElanName->length > 32)
	return (NO_CREATION_ERROR);
    
    if (!IsDisplayString(data->lesElanName))
	return (NO_CREATION_ERROR);
    
    switch (object->nominator) {
#ifdef I_lesAtmAddrSpec
    case I_lesAtmAddrSpec:
	
	if (value->os_value->length != STATIONLEN_ATMNSAP)
	    return (WRONG_LENGTH_ERROR);
	
	break;
#endif
	
#ifdef I_lesAtmAddrMask
    case I_lesAtmAddrMask:
	
	if (value->os_value->length != STATIONLEN_ATMNSAP)
	    return (WRONG_LENGTH_ERROR);
	
	break;
#endif 
	
#ifdef I_lesIfIndex
    case I_lesIfIndex:
	
	/* cannot alter lesIfIndex once set */
	
	if (data->lesStatus == D_lesStatus_active)
	    return (INCONSISTENT_VALUE_ERROR);

	snmpidb = 
	    ifmib_snmpidb_get(NULL,-1, EXACT, value->sl_value,NULL,NULL);
	if (!snmpidb)
	    return (INCONSISTENT_VALUE_ERROR);
	
	if (snmpidb->snmp_if_struct != SNMP_IF_HWIDB)
	    return (INCONSISTENT_VALUE_ERROR);
	
	if (!is_atm(snmpidb->snmp_if_ptr.ifptr.hwidb))
		return (INCONSISTENT_VALUE_ERROR);
	
	if (VALID(I_lesSubIfNum, data->valid)) {
	    FOR_ALL_SWIDBS_ON_HW(snmpidb->snmp_if_ptr.ifptr.hwidb,
				 swidb)
		if (swidb->sub_number == value->sl_value)
		    break;
	    
	    if (swidb) {
		if (swidb->lane_info) {
		    if ((swidb->lane_info->name_size !=
			 data->lesElanName->length) ||
			(memcmp(swidb->lane_info->name, 
				data->lesElanName->octet_ptr,
				data->lesElanName->length) != 0))
			return (INCONSISTENT_VALUE_ERROR);
		}
	    } else
		if (!idb_can_create_more())
		    return (RESOURCE_UNAVAILABLE_ERROR);
	} else {	    
	    FOR_ALL_SWIDBS_ON_HW(snmpidb->snmp_if_ptr.ifptr.hwidb, swidb) 
		if ((swidb->lane_info) && (swidb->lane_info->name_size ==
					   data->lesElanName->length) &&
		    (memcmp(swidb->lane_info->name,
			    data->lesElanName->octet_ptr,
			    data->lesElanName->length) == 0))
		    break;
	    
	    if ((!swidb) && (!idb_can_create_more()))
		return (RESOURCE_UNAVAILABLE_ERROR);
	}
	
	break;
#endif
	
#ifdef I_lesSubIfNum
    case I_lesSubIfNum:
	
	/* cannot alter the lesSubIfNum once set */
	if (data->lesStatus == D_lesStatus_active)
	    return (INCONSISTENT_VALUE_ERROR);
	
	/* If the hwidb hasn't been chosen yet then we must wait to do
	   this check */
	
	if (VALID(I_lesIfIndex, data->valid)) {
	    snmpidb = ifmib_snmpidb_get(NULL,-1, EXACT, data->lesIfIndex,
					NULL, NULL);
	    FOR_ALL_SWIDBS_ON_HW(snmpidb->snmp_if_ptr.ifptr.hwidb,
				 swidb)
		if (swidb->sub_number == value->sl_value)
		    break;
	    
	    if (swidb) {
		if (swidb->lane_info) {
		    if ((swidb->lane_info->name_size != 0) &&
			((swidb->lane_info->name_size !=
			  data->lesElanName->length) || 
			 (memcmp(swidb->lane_info->name, 
				 data->lesElanName->octet_ptr,
				 data->lesElanName->length) != 0)))
			return (INCONSISTENT_VALUE_ERROR);
		}
	    } else
		if (!idb_can_create_more())
		    return (RESOURCE_UNAVAILABLE_ERROR);
	}
	
	break;
#endif
	
#ifdef I_lesColocBusAtmAddrSpec
    case I_lesColocBusAtmAddrSpec:
	
	if (value->os_value->length != STATIONLEN_ATMNSAP)
	    return (WRONG_LENGTH_ERROR);
	
	break;
#endif
	
#ifdef I_lesColocBusAtmAddrMask
    case I_lesColocBusAtmAddrMask:
	
	if (value->os_value->length != STATIONLEN_ATMNSAP)
	    return (WRONG_LENGTH_ERROR);
	
	break;
#endif
	
#ifdef I_lesLecsAtmAddr
    case I_lesLecsAtmAddr:
	
	if (value->os_value->length != 20)
	    return (WRONG_LENGTH_ERROR);
	
	SET_VALID(I_lesLecsAtmAddr, data->valid);
	break;
#endif
	
#ifdef I_lesStatus
    case I_lesStatus:
	
	if ((value->sl_value == D_lesStatus_notInService) ||
	    (value->sl_value == D_lesStatus_createAndWait))
	    return (WRONG_VALUE_ERROR);
	
	SET_VALID(I_lesStatus, data->valid);
	break;
#endif
    }
    
    return NO_ERROR;
}

int
k_lesEntry_ready (
		  ObjectInfo     *object,
		  ObjectSyntax   *value,
		  doList_t       *doHead,
		  doList_t       *dp)
{
    lesEntry_t* data;
    
    data = (lesEntry_t*) dp->data;
    
    /* Set the mask to valid if the address has been specified */
    
    switch (data->lesStatus) {
    case D_lesStatus_active:
	dp->state = ADD_MODIFY;
	break;
    case D_lesStatus_createAndGo:
	if ((VALID(I_lesAtmAddrMask, data->valid)) &&
	    (!VALID(I_lesAtmAddrSpec, data->valid)))
	    break;
	if ((VALID(I_lesColocBusAtmAddrMask, data->valid)) &&
	    (!VALID(I_lesColocBusAtmAddrSpec, data->valid)))
	    break;
	if ((data->lesLanType == D_lesLanType_dot5) &&
	    (!VALID(I_lesSegmentID, data->valid)))
	    break;
	if (VALID (I_lesIfIndex, data->valid))
	    dp->state = ADD_MODIFY;
	break;
    case D_lesStatus_destroy:
	dp->state = DELETE;
	break;
    }
    
    return NO_ERROR;
}

int
k_lesEntry_set_defaults (doList_t *dp) {
    lesEntry_t *data;

    data = (lesEntry_t *) (dp->data);
    
    data->lesLanType = D_lesLanType_dot3;
    SET_VALID(I_lesLanType, data->valid);
    
    data->lesMaxFrm = D_lesMaxFrm_dot3;
    SET_VALID(I_lesMaxFrm, data->valid);
    
    data->lesJoinTimeout = S4_DEFAULT;
    SET_VALID(I_lesJoinTimeout, data->valid);
    
    data->lesAdminStatus = D_lesAdminStatus_active;
    SET_VALID(I_lesAdminStatus, data->valid);
    
    data->lesStatus = UNUSED;
    
    return NO_ERROR;
}

static boolean
k_lesEntry_setValues (lsv_info_t* lsinfo, lesEntry_t* data) {
    
    boolean restart;

    restart = FALSE;
    
    if (VALID(I_lesAtmAddrSpec, data->valid)) {
	if ((lsinfo->lane_info->lsv_nsap.length != STATIONLEN_ATMNSAP) 
	    || (memcmp(lsinfo->lane_info->lsv_nsap.dataBytes,
		       data->lesAtmAddrSpec->octet_ptr,
		       data->lesAtmAddrSpec->length) != 0)) {
	    restart = TRUE;
	    memcpy(lsinfo->lane_info->lsv_nsap.dataBytes,
		   data->lesAtmAddrSpec->octet_ptr,
		   data->lesAtmAddrSpec->length);
	    memset(lsinfo->lane_info->lsv_nsap.maskBytes, 0xff,
		   data->lesAtmAddrSpec->length); 
	    lsinfo->lane_info->lsv_nsap.length = STATIONLEN_ATMNSAP;
	    lsinfo->lane_info->lsv_nsap.type = STATION_ATMNSAP;
	}
    } 
    
    if ((VALID(I_lesAtmAddrMask, data->valid)) &&
	(memcmp(lsinfo->lane_info->lsv_nsap.maskBytes,
		data->lesAtmAddrMask->octet_ptr,
		data->lesAtmAddrMask->length) != 0)) {
	restart = TRUE;
	memcpy(lsinfo->lane_info->lsv_nsap.maskBytes,
	       data->lesAtmAddrMask->octet_ptr,
	       data->lesAtmAddrMask->length);
    }    
    
    /* don't move a LES, cannot set lesIfIndex and lesSubIfNum */
    
    if (VALID(I_lesColocBusAtmAddrSpec, data->valid)) {
	if ((lsinfo->lane_info->bus_nsap.length != STATIONLEN_ATMNSAP) ||
	    (memcmp(lsinfo->lane_info->bus_nsap.dataBytes,
		    data->lesColocBusAtmAddrSpec->octet_ptr,
		    data->lesColocBusAtmAddrSpec->length) != 0)) {
	    restart = TRUE;
	    memcpy(lsinfo->lane_info->bus_nsap.dataBytes,
		   data->lesColocBusAtmAddrSpec->octet_ptr,
		   data->lesColocBusAtmAddrSpec->length);
	    memset(lsinfo->lane_info->bus_nsap.maskBytes, 0xff,
		   data->lesColocBusAtmAddrSpec->length); 
	}
    } 
    
    if (VALID(I_lesColocBusAtmAddrMask, data->valid)) {
	if ((lsinfo->lane_info->bus_nsap.length != STATIONLEN_ATMNSAP) ||
	    (memcmp(lsinfo->lane_info->bus_nsap.maskBytes,
		    data->lesColocBusAtmAddrMask->octet_ptr,
		    data->lesColocBusAtmAddrMask->length) != 0)) {
	    restart = TRUE;
	    memcpy(lsinfo->lane_info->bus_nsap.maskBytes,
		   data->lesColocBusAtmAddrMask->octet_ptr,
		   data->lesColocBusAtmAddrMask->length);
	    lsinfo->lane_info->bus_nsap.length = STATIONLEN_ATMNSAP;
	    lsinfo->lane_info->bus_nsap.type = STATION_ATMNSAP;
	}
    }
    
    /* only 802.3 supported for now so no restart needed */
    
    lsinfo->lane_info->type = data->lesLanType;
    
    switch (data->lesMaxFrm) {
    case D_lesMaxFrm_dot3:
	lsinfo->lane_info->max_data_frame = LANE_MAX_FRAME_1516;
	break;
    case D_lesMaxFrm_tr4Mb:
	lsinfo->lane_info->max_data_frame = LANE_MAX_FRAME_4544;
	break;
    case D_lesMaxFrm_rfc1626:
	lsinfo->lane_info->max_data_frame = LANE_MAX_FRAME_9234;
	break;
    case D_lesMaxFrm_tr16Mb:
	lsinfo->lane_info->max_data_frame = LANE_MAX_FRAME_18190;
    }
    
    lsinfo->S4_control_timeout = data->lesJoinTimeout;
    if (VALID(I_lesLecsAtmAddr, data->valid)) {
	if ((lsinfo->lane_info->lecs_nsap.length != STATIONLEN_ATMNSAP) ||
	    (memcmp(lsinfo->lane_info->lecs_nsap.dataBytes,
		    data->lesLecsAtmAddr->octet_ptr,
		    data->lesLecsAtmAddr->length) != 0)) {
	    restart = TRUE;
	    memcpy(lsinfo->lane_info->lecs_nsap.dataBytes,
		   data->lesLecsAtmAddr->octet_ptr,
		   data->lesLecsAtmAddr->length);
	    memset(lsinfo->lane_info->lecs_nsap.maskBytes, 0xff,
		   data->lesLecsAtmAddr->length); 
	    lsinfo->lane_info->lecs_nsap_type = LANE_LECS_BIND_ADDR;
	    lsinfo->lane_info->lecs_nsap.length = STATIONLEN_ATMNSAP;
	    lsinfo->lane_info->lecs_nsap.type = STATION_ATMNSAP;
	}
    } 
    
    if ((data->lesAdminStatus == D_lesAdminStatus_active) &&
	(lsinfo->admin_state == LSV_LESBUS_ADMIN_DOWN)) {
	lsinfo->admin_state = LSV_LESBUS_ADMIN_UP;
	lsv_admin_up(lsinfo);
    } else 
	if ((lsinfo->admin_state == LSV_LESBUS_ADMIN_UP) &&
	    (data->lesAdminStatus == D_lesAdminStatus_inactive)) {
	    lsinfo->admin_state = LSV_LESBUS_ADMIN_DOWN;
	    lsv_admin_down(lsinfo->lane_info->swidb, LSV_DOWN_SHUTDOWN);
	}
    
    return (restart);
}

int
k_lesEntry_set (
		lesEntry_t *data,
		ContextInfo *contextInfo,
		int function)
{
    lsv_info_t* lsinfo; 
    snmpidbtype* snmpidb;
    idbtype* swidb;
    int subifnum;

    swidb = NULL;
    subifnum = 1;
    
    switch(data->lesStatus) {
    case D_lesStatus_createAndGo:
	
	snmpidb = ifmib_snmpidb_get(NULL,-1, EXACT, data->lesIfIndex,
				    NULL, NULL);
	
	/* check to see if the ELAN specified is existant on the
	   interface */
	
	if (VALID(I_lesSubIfNum, data->valid)) {
	    FOR_ALL_SWIDBS_ON_HW(snmpidb->snmp_if_ptr.ifptr.hwidb,
				 swidb) 
		if (swidb->sub_number == data->lesSubIfNum)
		    break;
	    if (!swidb)
		return (COMMIT_FAILED_ERROR);
	    
	} else {
	    FOR_ALL_SWIDBS_ON_HW(snmpidb->snmp_if_ptr.ifptr.hwidb, swidb) {
		if ((swidb->lane_info) && (swidb->lane_info->name_size ==
					   data->lesElanName->length) &&
		    (memcmp(swidb->lane_info->name,
			    
			    data->lesElanName->octet_ptr,
			    data->lesElanName->length) == 0))
		    break;
		
		/* insure that we have a unique subifnum */
		
		if (subifnum <= swidb->sub_number)
		    subifnum = swidb->sub_number +1;
	    }
	}
	
	/* if it is not existant, a new sub-interface must be created
	 */
	
	if (!swidb) {
	    swidb =
		idb_create_subif(snmpidb->snmp_if_ptr.ifptr.hwidb->firstsw, 
				 subifnum); 
	    if (!swidb)
		return (COMMIT_FAILED_ERROR);
	}
	
   switch(data->lesLanType) {
      case(LANE_TYPE_802_3):
			lsinfo = lsv_create_lesbus(swidb, data->lesLanType,
				   data->lesElanName->length,
				   data->lesElanName->octet_ptr,
				   LANE_CALLER_SNMP, 0, data->lesIndex);
         break;
      case(LANE_TYPE_802_5):
         lsinfo = lsv_create_lesbus(swidb, data->lesLanType,
               data->lesElanName->length,
               data->lesElanName->octet_ptr,
               LANE_CALLER_SNMP, data->lesSegmentID, data->lesIndex); 
         break;
      default:
         return(COMMIT_FAILED_ERROR);
      }

	if (!lsinfo)
	    return (COMMIT_FAILED_ERROR);
	
	k_lesEntry_setValues(lsinfo, data);
	
	break;
	
    case D_lesStatus_active:
	
	lsinfo = get_lesbus(data->lesElanName, data->lesIndex, EXACT);
	
	if (k_lesEntry_setValues(lsinfo, data) == TRUE)
	    lsv_kill_lesbus(lsinfo);
	break;
	
    case D_lesStatus_destroy:
	
	lsinfo = get_lesbus(data->lesElanName, data->lesIndex, EXACT);
	
	if ((!lsinfo) || (!lsinfo->lane_info))
	    return (NO_ERROR);

	/* don't do anything if subif is already deleted */

	if ((lsinfo->lane_info->swidb) &&
	    (lsinfo->lane_info->swidb->subif_state ==
	     SUBIF_STATE_DELETED))
	    break;
	    
	/* go ahead and delete the sub-interface if the le-server is
	   brought down.  */
	
	if (!delete_interface(lsinfo->lane_info->swidb))
	    return (COMMIT_FAILED_ERROR);
	
	break;
    }
    
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lesEntry_UNDO
/* add #define SR_lesEntry_UNDO in lesmib.h to
 * include the undo routine for the lesEntry family.
 */
int
lesEntry_undo (
	      doList_t       *doHead,
	      doList_t       *doCur,
	      ContextInfo    *contextInfo)
{
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lesEntry_UNDO */
#endif /* SR_SNMPv2 */

lesStatsEntry_t *
k_lesStatsEntry_get (
		     int serialNum,
		     ContextInfo *contextInfo,
		     int nominator,
		     int searchType,
		     OctetString * lesElanName,
		     long lesIndex)
{
    static lesStatsEntry_t lesStatsEntryData;
    static OctetString elan_os;
    static OctetString errorLec_os, joinFailLec_os, regFailLec_os;
    lsv_info_t* lsinfo;

    lsinfo = get_lesbus(lesElanName, lesIndex, searchType);
    if (!lsinfo)
	return (NULL);
    
    elan_os.octet_ptr = lsinfo->lane_info->name;
    elan_os.length = lsinfo->lane_info->name_size;
    lesStatsEntryData.lesElanName = &elan_os;
    lesStatsEntryData.lesIndex = lsinfo->lesIndex;
    
    /* lesInErrorLastLec not implemented as yet */
    lesStatsEntryData.lesInErrors = lsinfo->lsvs.lsvUnknownFrames +
	lsinfo->lsvs.lsvFastUnknownFrames +
	    lsinfo->lsvs.lsvOutOtherFails;
    errorLec_os.octet_ptr = lsinfo->lsvs.lsvLastJoinFailLec.addr;
    errorLec_os.length = lsinfo->lsvs.lsvLastJoinFailLec.length;
    lesStatsEntryData.lesInErrorLastLec =  &errorLec_os;
    
    lesStatsEntryData.lesInFlushReplies = lsinfo->lsvs.lsvFastFwdFlushResps
	+ lsinfo->lsvs.lsvOutFlushResps;
    
    lesStatsEntryData.lesInJoinReqs = lsinfo->lsvs.lsvInJoinReqs;
    lesStatsEntryData.lesOutJoinFails = lsinfo->lsvs.lsvOutJoinFails;
    lesStatsEntryData.lesJoinLastFailCause = lsinfo->lsvs.lsvLastJoinFailCause;
    joinFailLec_os.octet_ptr = lsinfo->lsvs.lsvLastJoinFailLec.addr;
    joinFailLec_os.length = lsinfo->lsvs.lsvLastJoinFailLec.length;
    lesStatsEntryData.lesJoinLastFailLec = &joinFailLec_os;
    
    lesStatsEntryData.lesOutConfigReqs = lsinfo->lsvs.lsvOutConfigReqs;
    lesStatsEntryData.lesInConfigResps = lsinfo->lsvs.lsvInConfigResps ;
    lesStatsEntryData.lesInConfigFails = lsinfo->lsvs.lsvInConfigFails;
    
    lesStatsEntryData.lesInRegisReqs = lsinfo->lsvs.lsvInRegReqs;
    lesStatsEntryData.lesOutRegisFails = lsinfo->lsvs.lsvOutRegFails;
    lesStatsEntryData.lesRegisLastFailCause = lsinfo->lsvs.lsvLastRegFailCause;
    regFailLec_os.octet_ptr = lsinfo->lsvs.lsvLastRegFailLec.addr;
    regFailLec_os.length = lsinfo->lsvs.lsvLastRegFailLec.length;
    lesStatsEntryData.lesRegisLastFailLec = &regFailLec_os;
    
    lesStatsEntryData.lesInUnregReqs = lsinfo->lsvs.lsvInUnregReqs;
    
    lesStatsEntryData.lesInLearpUcasts = 0; /* current LES doesn't do
					       this */
    lesStatsEntryData.lesInLearpBroadcasts = lsinfo->lsvs.lsvInArpReqs;
    lesStatsEntryData.lesOutLearpFwd = lsinfo->lsvs.lsvFastFwdArpReqs;
    lesStatsEntryData.lesInLearpResps = lsinfo->lsvs.lsvFastFwdArpResps;
    
    lesStatsEntryData.lesInNarpReqs = lsinfo->lsvs.lsvInNarpReqs;
    lesStatsEntryData.lesInTopolReqs = lsinfo->lsvs.lsvInTopReqs;
    
    return(&lesStatsEntryData);
}

lesClientEntry_t *
k_lesClientEntry_get (
		      int serialNum,
		      ContextInfo *contextInfo,
		      int nominator,
		      int searchType,
		      OctetString * lesElanName,
		      long lesIndex,
		      long lesClientLecid)
{
    static lesClientEntry_t lesClientEntryData;
    static OctetString elan_os, lecaddr_os;
    lsv_info_t* lsinfo;
    lsv_client_t* lsclient, *nextlec;
    vc_info_t* vc;
    boolean found;

    nextlec = 0;
    found = FALSE;
    
    elan_os.octet_ptr = lesElanName->octet_ptr;
    elan_os.length = lesElanName->length;
    
    while (!found) {
	lsinfo = get_lesbus(&elan_os, lesIndex, searchType);
	
	if (!lsinfo)
	    return NULL;
	
	for (lsclient = lsinfo->client_table; lsclient; 
	     lsclient = lsclient->next) {
	    if (lsclient->lecid == lesClientLecid) {
		found = TRUE;
		break;
	    }
	    if ((searchType == NEXT) && (lsclient->lecid >
					 lesClientLecid)) {
		found = TRUE;
		if (!nextlec || (nextlec->lecid > lsclient->lecid))
		    nextlec = lsclient;
	    }
	}
	
	if (!found) {
	    if (searchType == EXACT)
		return NULL;
	    lesIndex = lsinfo->lesIndex + 1;
	    lesClientLecid = 0; /* reset the LECID to get the first
				   entry in the next LES */
	}
	
	elan_os.octet_ptr = lsinfo->lane_info->name;
	elan_os.length = lsinfo->lane_info->name_size;
    }
    
    if (!lsclient)
	lsclient = nextlec;
    
    SET_ALL_VALID(lesClientEntryData.valid);
    
    lesClientEntryData.lesClientLecid = lsclient->lecid;
    
    switch (lsclient->client_state) {
    case LSV_CLIENT_INITIALIZED:
    case LSV_CLIENT_CONNECTED:
	lesClientEntryData.lesClientState = D_lesClientState_connected;
	break;
    case LSV_CLIENT_JOIN_RECEIVED:
	lesClientEntryData.lesClientState = D_lesClientState_joinRecv;
	break;
    case LSV_CLIENT_ADD_SENT:
	lesClientEntryData.lesClientState = D_lesClientState_addLec;
	break;
    case LSV_CLIENT_JOINED:
	lesClientEntryData.lesClientState =
	    D_lesClientState_busConnect;
	break;
    case LSV_CLIENT_OPERATIONAL:
	lesClientEntryData.lesClientState =
	    D_lesClientState_operational;
	break;
    case LSV_CLIENT_REJECTED:
    case LSV_CLIENT_TERMINATING:
	lesClientEntryData.lesClientState =
	    D_lesClientState_terminating;
    }
    if ((lsclient->direct_vcc.swidb) &&
	(lsclient->direct_vcc.swidb->hwptr)) {
	lesClientEntryData.lesClientIfIndex = 
	    lsclient->direct_vcc.swidb->hwptr->snmp_if_index;
	lesClientEntryData.lesClientAtmAddr = &lecaddr_os;
	lecaddr_os.octet_ptr = lsclient->direct_vcc.calling_addr.addr;
	lecaddr_os.length = STATIONLEN_ATMNSAP;
	vc = atm_getvc(lsclient->direct_vcc.swidb->hwptr->atm_db, 
		       lsclient->direct_vcc.vcd);
	if (vc) {
	    lesClientEntryData.lesClientControlVpi = vc->vpi;
	    lesClientEntryData.lesClientControlVci = vc->vci;
	} else {
	    lesClientEntryData.lesClientControlVpi = -1;
	    lesClientEntryData.lesClientControlVci = -1;
	}
    } else 
	return NULL;
    
    lesClientEntryData.lesClientStatus = D_lesClientStatus_active;
    lesClientEntryData.lesElanName = &elan_os;
    lesClientEntryData.lesIndex = lsinfo->lesIndex;
    return(&lesClientEntryData);
}

#ifdef SETS
int
k_lesClientEntry_test (
		       ObjectInfo     *object,
		       ObjectSyntax   *value,
		       doList_t       *dp,
		       ContextInfo    *contextInfo)
{
    lesClientEntry_t *data = (lesClientEntry_t *) (dp->data);
    
    if (data->lesClientStatus != D_lesClientStatus_active)
	return (NO_CREATION_ERROR);
    
    if ((value->sl_value != D_lesClientStatus_destroy) &&
	(value->sl_value != D_lesClientStatus_active))
	return (WRONG_VALUE_ERROR);
    return NO_ERROR;
}

int
k_lesClientEntry_ready (
			ObjectInfo     *object,
			ObjectSyntax   *value,
			doList_t       *doHead,
			doList_t       *dp)
{
    
    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_lesClientEntry_set_defaults (doList_t       *dp)
{
    return NO_ERROR;
}

int
k_lesClientEntry_set (
		      lesClientEntry_t *data,
		      ContextInfo *contextInfo,
		      int function)
{
    lsv_client_t* client;
    lsv_info_t* lsinfo;

    lsinfo = get_lesbus(data->lesElanName, data->lesIndex, EXACT);
    
    if (data->lesClientStatus == D_lesClientStatus_active)
	return (NO_ERROR);
    
    client = lsinfo->client_table;
    while (client) {
	if (data->lesClientLecid == client->lecid) {
	    lsv_kill_client(lsinfo, client);
	    break;
	}
	client = client->next;
    }
    
    return NO_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lesClientEntry_UNDO
/* add #define SR_lesClientEntry_UNDO in lesmib.h to
 * include the undo routine for the lesClientEntry family.
 */
int
lesClientEntry_undo (
		     doList_t       *doHead,
		     doList_t       *doCur,
		     ContextInfo    *contextInfo)
{
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lesEntry_UNDO */
#endif /* SR_SNMPv2 */

lesMacRegEntry_t *
k_lesMacRegEntry_get(
		     int serialNum,
		     ContextInfo *contextInfo,
		     int nominator,
		     int searchType,
		     OctetString * lesElanName,
		     long lesIndex,
		     OctetString * lesMacRegMacAddress)
{
    static lesMacRegEntry_t lesMacRegEntryData;
    static OctetString elan_os, lecmac_os, lecatm_os;
    lsv_info_t* lsinfo;
    lsv_reg_table_t* lsreg, *nextreg;
    boolean found;
    int result;

    nextreg = 0;
    found = FALSE;
    
    elan_os.octet_ptr = lesElanName->octet_ptr;
    elan_os.length = lesElanName->length;
    
    while (!found) {
	lsinfo = get_lesbus(&elan_os, lesIndex, searchType);
	
	if (!lsinfo)
	    return NULL;
	
	for (lsreg = lsinfo->reg_table; lsreg; lsreg = lsreg->next) {
	    lecmac_os.octet_ptr = lsreg->lan_dest.addr.mac;
	    lecmac_os.length = STATIONLEN_IEEE48;
	    result = CmpOctetStrings(&lecmac_os, lesMacRegMacAddress);
	    if (result == 0) {
		found = TRUE;
		break;
	    }
	    if ((searchType == NEXT) && (result > 0)) {
		found = TRUE;
		if (!nextreg || (memcmp(nextreg->lan_dest.addr.mac,
					lsreg->lan_dest.addr.mac,
					STATIONLEN_IEEE48) > 0))
		    nextreg = lsreg;
	    }
	}
	
	if (!found) {
	    if (searchType == EXACT)
		return NULL;
	    lesMacRegMacAddress->length = 0; /* to get the first LEC
						from the next LES */
	    lesIndex = lsinfo->lesIndex + 1;
	}

	elan_os.octet_ptr = lsinfo->lane_info->name;
	elan_os.length = lsinfo->lane_info->name_size;
    }
    
    if (!lsreg)
	lsreg = nextreg;
    
    if (!lsreg->client)
	return NULL;
    
    lecmac_os.octet_ptr = lsreg->lan_dest.addr.mac;
    lecmac_os.length = STATIONLEN_IEEE48;
    lesMacRegEntryData.lesMacRegMacAddress = &lecmac_os;
    lecatm_os.octet_ptr = lsreg->nsap.addr;
    lecatm_os.length = lsreg->nsap.length;
    lesMacRegEntryData.lesMacRegAtmAddr = &lecatm_os;
    lesMacRegEntryData.lesMacRegLecid = lsreg->client->lecid;
    lesMacRegEntryData.lesElanName = &elan_os;
    lesMacRegEntryData.lesIndex = lsinfo->lesIndex;
    return(&lesMacRegEntryData);
}

lesRDRegEntry_t *
k_lesRDRegEntry_get (int serialNum, ContextInfo* contextInfo, int
		     nominator, int searchType, OctetString*
		     lesElanName, long lesIndex, long
		     lesRDRegSegmentId, long lesRDRegBridgeNum) {

    static lesRDRegEntry_t lesRDRegEntryData;
    static OctetString elan_os, lecatm_os;
    lsv_info_t* lsinfo;
    lsv_reg_table_t* lsreg, *nextreg;
    boolean found;
    int result;
    ushort search_rd;

    nextreg = 0;
    found = FALSE;
    search_rd = (lesRDRegSegmentId << 4) | lesRDRegBridgeNum;

    elan_os.octet_ptr = lesElanName->octet_ptr;
    elan_os.length = lesElanName->length;

    while (!found) {
       lsinfo = get_lesbus(&elan_os, lesIndex, searchType);
  
       if (!lsinfo)
           return NULL;

       for (lsreg = lsinfo->reg_table; lsreg; lsreg = lsreg->next) {
          result = (lsreg->lan_dest.addr.rd.val == search_rd) ? 0 : 
                   (lsreg->lan_dest.addr.rd.val > search_rd) ? 1 : -1;
          if (result == 0) {
             found = TRUE;
             break;
          }
          if ((searchType == NEXT) && (result > 0)) {
             found = TRUE;
             if (!nextreg || 
                (nextreg->lan_dest.addr.rd.val > lsreg->lan_dest.addr.rd.val))
                nextreg = lsreg;
          }
      }
       
       if (!found) {
	   if (searchType == EXACT)
	       return NULL;
	   search_rd = 0;  /* to get the first LEC in the next LES */
	   lesIndex = lsinfo->lesIndex + 1;
       }
       
       elan_os.octet_ptr = lsinfo->lane_info->name;
       elan_os.length = lsinfo->lane_info->name_size;
   }
    
    if (!lsreg)
	    lsreg = nextreg;
    
    if (!lsreg->client)
	    return NULL;
    
   lesRDRegEntryData.lesRDRegSegmentId = lsreg->lan_dest.addr.rd.val >> 4;
   lesRDRegEntryData.lesRDRegBridgeNum = lsreg->lan_dest.addr.rd.val & 0x0F;
   lecatm_os.octet_ptr = lsreg->nsap.addr;
   lecatm_os.length = lsreg->nsap.length;
   lesRDRegEntryData.lesRDRegAtmAddr = &lecatm_os;
   lesRDRegEntryData.lesRDRegLecid = lsreg->client->lecid; 
   lesRDRegEntryData.lesElanName = &elan_os;
   lesRDRegEntryData.lesIndex = lsinfo->lesIndex;
   return(&lesRDRegEntryData);
}


static void
init_lesmib (subsystype* subsys)
{
    load_mib(lesmib_OidList, lesmib_OidListNum);
    load_oid(lesmib_oid_table);
}

/*
 * lesmib subsystem header
 */

#define MAJVERSION_lesmib 1
#define MINVERSION_lesmib 0
#define EDITVERSION_lesmib 0

SUBSYS_HEADER(lesmib,
              MAJVERSION_lesmib,
              MINVERSION_lesmib,
              EDITVERSION_lesmib,
              init_lesmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: lane",
              NULL);
      
