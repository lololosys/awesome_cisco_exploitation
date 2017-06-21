/* $Id: sr_busmib.c,v 3.6.8.4 1996/07/01 18:45:18 hampton Exp $
 * $Source: /release/112/cvs/Xsys/lane/sr_busmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1995, Christopher Young (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_busmib.c,v $
 * Revision 3.6.8.4  1996/07/01  18:45:18  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.6.8.3  1996/05/21  09:54:27  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.8.2  1996/05/17  11:25:39  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.8.2  1996/04/26  07:56:01  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.8.1  1996/03/17  18:05:54  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.8.1  1996/05/09  14:33:30  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.34.1  1996/04/27  07:07:19  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.24.2  1996/04/08  14:52:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.6.24.1  1996/04/08  01:57:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.14.1  1996/03/22  22:55:48  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6  1996/02/27  20:54:12  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5.12.1  1996/03/05  06:43:52  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.6.1  1996/02/27  21:11:08  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/24  21:18:13  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.4  1996/01/18  15:50:38  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3.14.1  1996/01/11  01:47:04  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.10.1  1995/12/29  01:07:17  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.3  1995/11/17  17:51:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:17  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/13  03:05:27  cyoung
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
#include "sr_busmib.h"
#include "sr_busmib-mib.h"
#include "sr_lesmib.h" /* routine to retrieve the les/bus */
#include "snmp_interface.h"
#include "interface_private.h"
#include "subsys.h"
/* lsv_private.h needs the following files. */
#include "interface.h"
#include "address.h"
#include "packet.h"
#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../atm/atmsig_api.h"
#include "lane.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "lsv_private.h"  

/*---------------------------------------------------------------------
 * Retrieve data from the busEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
busEntry_get (OID* incoming, ObjectInfo *object, int searchType,
	      ContextInfo *contextInfo, int serialNum)
{
    int             arg;
    void            *dp;
    busEntry_t      *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   busElanName;
    int             busElanName_offset;
    long            busIndex;
    int             busIndex_offset;
    int             index;
    int             final_index;

    arg = object->nominator;
    data = 0;
    busElanName = NULL;
    
    busElanName_offset = object->oid.length;
    busIndex_offset = busElanName_offset +
	GetVariableIndexLength(incoming, busElanName_offset); 
    final_index = busIndex_offset + 1;
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToInt(incoming, busIndex_offset, &busIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        busIndex = MAX(1, busIndex);
    }
    
    if ( (InstToVariableOctetString(incoming, busElanName_offset,
				    &busElanName, searchType, &carry))
	< 0 ) { 
	arg = -1;
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_busEntry_get(serialNum, contextInfo,
					       arg ,searchType,
					       busElanName, busIndex))
	== NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->busElanName->length;
        for(i = 0; i < data->busElanName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->busElanName->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->busIndex;
        inst.length = index;
    }
    
    FreeOctetString(busElanName);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_busAtmAddrSpec
    case I_busAtmAddrSpec:
	dp = (void *) (CloneOctetString(data->busAtmAddrSpec));
	break;
#endif /* I_busAtmAddrSpec */
	
#ifdef I_busAtmAddrMask
    case I_busAtmAddrMask:
	dp = (void *) (CloneOctetString(data->busAtmAddrMask));
	break;
#endif /* I_busAtmAddrMask */
	
#ifdef I_busAtmAddrActl
    case I_busAtmAddrActl:
	dp = (void *) (CloneOctetString(data->busAtmAddrActl));
	break;
#endif /* I_busAtmAddrActl */
	
#ifdef I_busIfIndex
    case I_busIfIndex:
	dp = (void *) (&data->busIfIndex);
	break;
#endif /* I_busIfIndex */
	
#ifdef I_busSubIfNum
    case I_busSubIfNum:
	dp = (void *) (&data->busSubIfNum);
	break;
#endif /* I_busSubIfNum */
	
#ifdef I_busUpTime
    case I_busUpTime:
	dp = (void *) (&data->busUpTime);
	break;
#endif /* I_busUpTime */
	
#ifdef I_busLanType
    case I_busLanType:
	dp = (void *) (&data->busLanType);
	break;
#endif /* I_busLanType */
	
#ifdef I_busMaxFrm
    case I_busMaxFrm:
	dp = (void *) (&data->busMaxFrm);
	break;
#endif /* I_busMaxFrm */
	
#ifdef I_busMaxFrmAge
    case I_busMaxFrmAge:
	dp = (void *) (&data->busMaxFrmAge);
	break;
#endif /* I_busMaxFrmAge */
	
#ifdef I_busOutFwdOctets
    case I_busOutFwdOctets:
	dp = (void *) (&data->busOutFwdOctets);
	break;
#endif /* I_busOutFwdOctets */
	
#ifdef I_busOutFwdUcastFrms
    case I_busOutFwdUcastFrms:
	dp = (void *) (&data->busOutFwdUcastFrms);
	break;
#endif /* I_busOutFwdUcastFrms */
	
#ifdef I_busOutFwdNUcastFrms
    case I_busOutFwdNUcastFrms:
	dp = (void *) (&data->busOutFwdNUcastFrms);
	break;
#endif /* I_busOutFwdNUcastFrms */
	
#ifdef I_busFrmTimeOuts
    case I_busFrmTimeOuts:
	dp = (void *) (&data->busFrmTimeOuts);
	break;
#endif /* I_busFrmTimeOuts */
	
#ifdef I_busMultiFwdVpi
    case I_busMultiFwdVpi:
	dp = (void *) (&data->busMultiFwdVpi);
	break;
#endif /* I_busMultiFwdVpi */
	
#ifdef I_busMultiFwdVci
    case I_busMultiFwdVci:
	dp = (void *) (&data->busMultiFwdVci);
	break;
#endif /* I_busMultiFwdVci */
	
#ifdef I_busOperStatus
    case I_busOperStatus:
	dp = (void *) (&data->busOperStatus);
	break;
#endif /* I_busOperStatus */
	
#ifdef I_busAdminStatus
    case I_busAdminStatus:
	dp = (void *) (&data->busAdminStatus);
	break;
#endif /* I_busAdminStatus */
	
#ifdef I_busStatus
    case I_busStatus:
	dp = (void *) (&data->busStatus);
	break;
#endif /* I_busStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the busEntry data object.
 *---------------------------------------------------------------------*/
void
busEntry_free (busEntry_t* data)
{
    if (data != NULL) {
	FreeOctetString(data->busElanName);
	FreeOctetString(data->busAtmAddrSpec);
	FreeOctetString(data->busAtmAddrMask);
	FreeOctetString(data->busAtmAddrActl);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after busEntry set/undo
 *---------------------------------------------------------------------*/
static int
busEntry_cleanup (doList_t *trash)
{
    busEntry_free(trash->data);
#ifdef SR_SNMPv2
    busEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the busEntry family
 *---------------------------------------------------------------------*/
busEntry_t *
Clone_busEntry (busEntry_t* busEntry)
{
    busEntry_t *data;
    
    if ((data = malloc(sizeof(busEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (busEntry), sizeof(busEntry_t));
    
    data->busElanName = CloneOctetString(busEntry->busElanName);
    data->busAtmAddrSpec = CloneOctetString(busEntry->busAtmAddrSpec);
    data->busAtmAddrMask = CloneOctetString(busEntry->busAtmAddrMask);
    data->busAtmAddrActl = CloneOctetString(busEntry->busAtmAddrActl);
    
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
busEntry_test (
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
    busEntry_t     *busEntry;
    OctetString *  busElanName;
    int            busElanName_offset;
    long           busIndex;
    int            busIndex_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    busElanName = NULL;

    /*
     * Validate the object instance
     *
     */
    busElanName_offset = object->oid.length;
    busIndex_offset = busElanName_offset +
	GetVariableIndexLength(incoming, busElanName_offset); 
    final_index = busIndex_offset + 1;
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToVariableOctetString(incoming, busElanName_offset,
				    &busElanName, EXACT, &carry)) < 0
	) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, busIndex_offset, &busIndex, EXACT,
		    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(busElanName);
        return error_status;
    }
    
    busEntry = k_busEntry_get(-1, contextInfo, -1, EXACT, busElanName,
			      busIndex); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == busEntry_set) &&
            (((busEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((busEntry_t *) (dp->data))->busElanName,
			     busElanName) == 0) && 
            (((busEntry_t *) (dp->data))->busIndex == busIndex) ) {
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = busEntry_set;
        dp->cleanupMethod = busEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_busEntry_UNDO
        dp->undoMethod = busEntry_undo;
#else /* SR_busEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_busEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (busEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_busEntry(busEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_busEntry(busEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(busEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(busEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((busEntry_t *) (dp->data))->busElanName =
		    CloneOctetString(busElanName); 
                SET_VALID(I_busElanName, ((busEntry_t *) (dp->data))->valid);
		
                ((busEntry_t *) (dp->data))->busIndex = (busIndex);
                SET_VALID(I_busIndex, ((busEntry_t *) (dp->data))->valid);
		
                error_status = k_busEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(busElanName);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_busAtmAddrSpec
    case I_busAtmAddrSpec:
	
	if (((busEntry_t *) (dp->data))->busAtmAddrSpec != NULL) {
	    FreeOctetString(((busEntry_t *) (dp->data))->busAtmAddrSpec);
	}
	
	((busEntry_t *) (dp->data))->busAtmAddrSpec = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_busAtmAddrSpec */
	
#ifdef I_busAtmAddrMask
    case I_busAtmAddrMask:
	
	if (((busEntry_t *) (dp->data))->busAtmAddrMask != NULL) {
	    FreeOctetString(((busEntry_t *) (dp->data))->busAtmAddrMask);
	}
	
	((busEntry_t *) (dp->data))->busAtmAddrMask = 
	    CloneOctetString(value->os_value);
	
	break;
#endif /* I_busAtmAddrMask */	 
	
#ifdef I_busIfIndex
    case I_busIfIndex:
	
	((busEntry_t *) (dp->data))->busIfIndex = value->sl_value;
	break;
#endif /* I_busIfIndex */
	
#ifdef I_busSubIfNum
    case I_busSubIfNum:
	
	((busEntry_t *) (dp->data))->busSubIfNum = value->sl_value;
	break;
#endif /* I_busSubIfNum */
	
#ifdef I_busLanType
    case I_busLanType:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((busEntry_t *) (dp->data))->busLanType = value->sl_value;
	break;
#endif /* I_busLanType */
	
#ifdef I_busMaxFrm
    case I_busMaxFrm:
	
	switch (value->sl_value) {
	case 1516:
	case 4544:
	case 9234:
	case 18190:
	    break;
	default:
	    return WRONG_VALUE_ERROR;
	}
	
	((busEntry_t *) (dp->data))->busMaxFrm = value->sl_value;
	break;
#endif /* I_busMaxFrm */
	
#ifdef I_busMaxFrmAge
    case I_busMaxFrmAge:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 4)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((busEntry_t *) (dp->data))->busMaxFrmAge = value->sl_value;
	break;
#endif /* I_busMaxFrmAge */
	
#ifdef I_busAdminStatus
    case I_busAdminStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((busEntry_t *) (dp->data))->busAdminStatus = value->sl_value;
	break;
#endif /* I_busAdminStatus */
	
#ifdef I_busStatus
    case I_busStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_busStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (busEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_busStatus_notInService) ||
	       (value->sl_value == D_busStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_busStatus_createAndGo) ||
		(value->sl_value == D_busStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((busEntry_t *) (dp->data))->busStatus = value->sl_value;
	break;
#endif /* I_busStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in busEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_busEntry_test */
    error_status = k_busEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((busEntry_t *) (dp->data))->valid);
        error_status = k_busEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
busEntry_set (
	      doList_t       *doHead,
	      doList_t       *doCur,
	      ContextInfo    *contextInfo)
{
    return (k_busEntry_set((busEntry_t *) (doCur->data),
			   contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */





busEntry_t *
k_busEntry_get (
		int serialNum,
		ContextInfo *contextInfo,
		int nominator,
		int searchType,
		OctetString * busElanName,
		long busIndex)
{
    static busEntry_t busEntryData;
    static OctetString elan_os;
    static OctetString busAddrSpec_os, busAddrMask_os, busAddrActl_os;
    lsv_info_t* lsinfo; 
    long port, vpi, vci;
    boolean vc_exists;
    
    vc_exists = FALSE;
    lsinfo = get_lesbus(busElanName, busIndex, searchType);
    
    if ((!lsinfo) || (!lsinfo->lane_info))
	return NULL;
    
    elan_os.octet_ptr = lsinfo->lane_info->name;
    elan_os.length = lsinfo->lane_info->name_size;
    busEntryData.busElanName = &elan_os;
    busEntryData.busIndex = 1; /* never more than one bus per ELAN for
				  this rev */
    
    busAddrSpec_os.octet_ptr = lsinfo->lane_info->bus_nsap.dataBytes;
    busAddrSpec_os.length = lsinfo->lane_info->bus_nsap.length;
    busEntryData.busAtmAddrSpec = &busAddrSpec_os;
    busAddrMask_os.octet_ptr = lsinfo->lane_info->bus_nsap.maskBytes;
    busAddrMask_os.length = lsinfo->lane_info->bus_nsap.length;
    busEntryData.busAtmAddrMask = &busAddrMask_os;
    busAddrActl_os.octet_ptr = lsinfo->bus_nsap.addr;
    busAddrActl_os.length = lsinfo->bus_nsap.length;
    busEntryData.busAtmAddrActl = &busAddrActl_os;
    if (lsinfo->lane_info->swidb) {
	busEntryData.busSubIfNum = lsinfo->lane_info->swidb->sub_number;
	if (lsinfo->lane_info->swidb->hwptr) {
	    busEntryData.busIfIndex = 
		lsinfo->lane_info->swidb->hwptr->snmp_if_index;
	    vc_exists = atm_getvc_mib_id(lsinfo->forward_vcc.vcd,
					 lsinfo->lane_info->swidb->hwptr, 
					 &port, &vpi, &vci);
	} else {
	    busEntryData.busIfIndex = 0;
	}
	
    } else {
	busEntryData.busIfIndex = 0; /* should never happen */
	busEntryData.busSubIfNum = 0;
    }
    busEntryData.busUpTime = SNMP_ABSOLUTE_TIME(lsinfo->start_time);
    busEntryData.busLanType = D_busLanType_dot3;
    switch (lsinfo->lane_info->max_data_frame) {
    case LANE_MAX_FRAME_1516:
	busEntryData.busMaxFrm = 1516;
	break;
    case LANE_MAX_FRAME_4544:
	busEntryData.busMaxFrm = 4544;
	break;
    case LANE_MAX_FRAME_9234:
	busEntryData.busMaxFrm = 9234;
	break;
    case LANE_MAX_FRAME_18190:
	busEntryData.busMaxFrm = 18190;
    }
    busEntryData.busMaxFrmAge = lsinfo->S5_max_frame_age;
    busEntryData.busOutFwdOctets = lsinfo->buss.busBytes;
    busEntryData.busOutFwdUcastFrms = lsinfo->buss.busUnicastForwards;
    busEntryData.busOutFwdNUcastFrms = lsinfo->buss.busInFrames - 
	lsinfo->buss.busUnicastForwards;
    busEntryData.busFrmTimeOuts = 0; /* current implementation can't
					time out */
    if (vc_exists) {
	busEntryData.busMultiFwdVpi = vpi;
	busEntryData.busMultiFwdVci = vci;
    } else {
	busEntryData.busMultiFwdVpi = BUS_VCC_INFO_INVALID;
	busEntryData.busMultiFwdVci = BUS_VCC_INFO_INVALID;
    }
    if (lsinfo->server_state == LSV_LESBUS_OPERATIONAL) 
	busEntryData.busOperStatus = D_busOperStatus_active;
    else 
	busEntryData.busOperStatus = D_busOperStatus_inactive;
    busEntryData.busAdminStatus = lsinfo->admin_state == LSV_LESBUS_ADMIN_UP ? 
	D_busAdminStatus_active : D_busAdminStatus_inactive;
    busEntryData.busStatus = ACTIVE;
    
    return (&busEntryData);
}

#ifdef SETS
int
k_busEntry_test (
		 ObjectInfo     *object,
		 ObjectSyntax   *value,
		 doList_t       *dp,
		 ContextInfo    *contextInfo)
{
    switch (object->nominator) {
#ifdef I_busAtmAddrSpec
    case I_busAtmAddrSpec:
	
	return (NOT_WRITABLE_ERROR);
	break;
#endif
	
#ifdef I_busAtmAddrMask
    case I_busAtmAddrMask:
	
	return (NOT_WRITABLE_ERROR);
	break;
#endif
	
#ifdef I_busIfIndex
    case I_busIfIndex:
	
	return (NOT_WRITABLE_ERROR);
	break;
#endif
	
#ifdef I_busSubIfNum
    case I_busSubIfNum:
	
	return(NOT_WRITABLE_ERROR);
	break;
#endif
	
#ifdef I_busLanType
    case I_busLanType:
	
	return (NOT_WRITABLE_ERROR);
	break;
#endif
	
#ifdef I_busMaxFrm
    case I_busMaxFrm:
	
	return (NOT_WRITABLE_ERROR);
	break;
#endif
	
#ifdef I_busAdminStatus
    case I_busAdminStatus:
	
	return (NOT_WRITABLE_ERROR);
	break;
#endif
	
#ifdef I_busStatus
    case I_busStatus:
	
	/* table is read-write only for this implementation */
	
	if (value->sl_value != D_busStatus_active)
	    return (WRONG_VALUE_ERROR);
	
	break;
#endif
    }
    return NO_ERROR;
}

int
k_busEntry_ready (
		  ObjectInfo     *object,
		  ObjectSyntax   *value,
		  doList_t       *doHead,
		  doList_t       *dp)
{
    
    if (((busEntry_t *) dp->data)->busStatus == D_busStatus_active)
	dp->state = ADD_MODIFY;
    
    return NO_ERROR;
}

int
k_busEntry_set_defaults (doList_t       *dp)
{
    busEntry_t *data;

    data = (busEntry_t *) (dp->data);
    data->busStatus = UNUSED;
    
    return NO_ERROR;
}

int
k_busEntry_set ( 
		busEntry_t *data,
		ContextInfo *contextInfo,
		int function)
{
    lsv_info_t* lsinfo;

    lsinfo = get_lesbus(data->busElanName, data->busIndex,
				    EXACT);
    if (!lsinfo) 
	return (COMMIT_FAILED_ERROR);
    
    lsinfo->S5_max_frame_age = data->busMaxFrmAge;
    
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_busEntry_UNDO
/* add #define SR_busEntry_UNDO in busmib.h to
 * include the undo routine for the busEntry family.
 */
int
busEntry_undo (
	       doList_t       *doHead,
	       doList_t       *doCur,
	       ContextInfo    *contextInfo)
{
    return UNDO_FAILED_ERROR;
}
#endif /* SR_busEntry_UNDO */
#endif /* SR_SNMPv2 */



static void
init_busmib (subsystype* subsys)
{
    load_mib(busmib_OidList, busmib_OidListNum);
    load_oid(busmib_oid_table);
}

/*
 * busmib subsystem header
 */

#define MAJVERSION_busmib 1
#define MINVERSION_busmib 0
#define EDITVERSION_busmib 0

SUBSYS_HEADER(busmib,
              MAJVERSION_busmib,
              MINVERSION_busmib,
              EDITVERSION_busmib,
              init_busmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: lane",
              NULL);








      
