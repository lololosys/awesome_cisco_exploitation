/* $Id: sr_lecmib.c,v 3.7.4.5 1996/07/11 20:34:59 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/lane/sr_lecmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Andy Bierman (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_lecmib.c,v $
 * Revision 3.7.4.5  1996/07/11  20:34:59  cyoung
 * Deregister RcvAddresses when client shutsdown/tearsdown.
 * CSCdi61990:  LANE CLIENT: memory leak
 * Branch: California_branch
 *
 * Revision 3.7.4.4  1996/07/01  18:45:23  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.4.3  1996/05/30  21:33:27  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.7.4.2  1996/05/09  14:33:33  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.4.1.2.1  1996/04/27  07:07:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.4.1  1996/04/25  23:15:16  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.20.2  1996/04/08  14:52:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.20.1  1996/04/08  01:57:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.10.1  1996/03/22  22:55:50  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7  1996/03/07  22:50:14  smackie
 * Fix inlines that could theoretically return without a value so that
 * the 95q4 toolchain will compile them. (CSCdi49069)
 *
 * Revision 3.6  1996/02/29  21:22:26  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5.12.1  1996/03/05  06:43:54  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.5.6.1  1996/02/27  21:11:10  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.5  1996/01/24  21:18:16  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.4  1996/01/18  15:50:41  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3.2.1  1996/01/11  01:47:06  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1996/01/05  04:56:26  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.2.10.1  1995/12/29  01:07:18  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/05  21:49:17  abierman
 * Adding placeholder files to support ATMF LEC MIB (atm94-0737R4)
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
#include "subsys.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../ui/debug.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "../snmp/ifmib_registry.h"

#include "../if/atm.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_ilmi.h"
#include "lane_registry.h"

#include "../snmp/snmp_api.h"
#include "sr_lecmib.h"
#include "sr_lecmib-mib.h"
#include "lecmib_util.h"
#include "lecmib_if.h"

/* temp SNMP index assignment scheme--declared external in lecmib_if.h */
unsigned         next_snmp_lec_index;


/*
 * Start of system-independent routines
 */


/**** undocumented code generated by SNMP Research MIB tools ****/
/*---------------------------------------------------------------------
 * Retrieve data from the lecConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecConfigEntry_get (
		    OID             *incoming,
		    ObjectInfo      *object,
		    int             searchType,
		    ContextInfo     *contextInfo,
		    int             serialNum)
{
    int             instLength;
    int             arg;
    void            *dp;
    lecConfigEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            lecIndex;
    
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    data = NULL;
    dp = NULL;

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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecConfigEntry_get(serialNum,
						     contextInfo, arg
						     ,searchType,
						     lecIndex)) ==
	NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->lecIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecRowStatus
    case I_lecRowStatus:
	dp = (void *) (&data->lecRowStatus);
	break;
#endif /* I_lecRowStatus */
	
#ifdef I_lecOwner
    case I_lecOwner:
	if (data->lecOwner)
	    dp = (void *) (CloneOctetString(data->lecOwner));
	break;
#endif /* I_lecOwner */
	
#ifdef I_lecConfigMode
    case I_lecConfigMode:
	dp = (void *) (&data->lecConfigMode);
	break;
#endif /* I_lecConfigMode */
	
#ifdef I_lecConfigLanType
    case I_lecConfigLanType:
	dp = (void *) (&data->lecConfigLanType);
	break;
#endif /* I_lecConfigLanType */
	
#ifdef I_lecConfigMaxDataFrameSize
    case I_lecConfigMaxDataFrameSize:
	dp = (void *) (&data->lecConfigMaxDataFrameSize);
	break;
#endif /* I_lecConfigMaxDataFrameSize */
	
#ifdef I_lecConfigLanName
    case I_lecConfigLanName:
	if (data->lecConfigLanName)
	    dp = (void *) (CloneOctetString(data->lecConfigLanName));
	break;
#endif /* I_lecConfigLanName */
	
#ifdef I_lecConfigLesAtmAddress
    case I_lecConfigLesAtmAddress:
	if (data->lecConfigLesAtmAddress)
	    dp = (void *) (CloneOctetString(data->lecConfigLesAtmAddress));
	break;
#endif /* I_lecConfigLesAtmAddress */
	
#ifdef I_lecControlTimeout
    case I_lecControlTimeout:
	dp = (void *) (&data->lecControlTimeout);
	break;
#endif /* I_lecControlTimeout */
	
#ifdef I_lecMaxUnknownFrameCount
    case I_lecMaxUnknownFrameCount:
	dp = (void *) (&data->lecMaxUnknownFrameCount);
	break;
#endif /* I_lecMaxUnknownFrameCount */
	
#ifdef I_lecMaxUnknownFrameTime
    case I_lecMaxUnknownFrameTime:
	dp = (void *) (&data->lecMaxUnknownFrameTime);
	break;
#endif /* I_lecMaxUnknownFrameTime */
	
#ifdef I_lecVccTimeoutPeriod
    case I_lecVccTimeoutPeriod:
	dp = (void *) (&data->lecVccTimeoutPeriod);
	break;
#endif /* I_lecVccTimeoutPeriod */
	
#ifdef I_lecMaxRetryCount
    case I_lecMaxRetryCount:
	dp = (void *) (&data->lecMaxRetryCount);
	break;
#endif /* I_lecMaxRetryCount */
	
#ifdef I_lecAgingTime
    case I_lecAgingTime:
	dp = (void *) (&data->lecAgingTime);
	break;
#endif /* I_lecAgingTime */
	
#ifdef I_lecForwardDelayTime
    case I_lecForwardDelayTime:
	dp = (void *) (&data->lecForwardDelayTime);
	break;
#endif /* I_lecForwardDelayTime */
	
#ifdef I_lecExpectedArpResponseTime
    case I_lecExpectedArpResponseTime:
       dp = (void *) (&data->lecExpectedArpResponseTime);
	break;
#endif /* I_lecExpectedArpResponseTime */
	
#ifdef I_lecFlushTimeOut
    case I_lecFlushTimeOut:
	dp = (void *) (&data->lecFlushTimeOut);
	break;
#endif /* I_lecFlushTimeOut */
	
#ifdef I_lecPathSwitchingDelay
    case I_lecPathSwitchingDelay:
	dp = (void *) (&data->lecPathSwitchingDelay);
	break;
#endif /* I_lecPathSwitchingDelay */
	
#ifdef I_lecLocalSegmentID
    case I_lecLocalSegmentID:
	dp = (void *) (&data->lecLocalSegmentID);
	break;
#endif /* I_lecLocalSegmentID */
	
#ifdef I_lecMulticastSendType
    case I_lecMulticastSendType:
	dp = (void *) (&data->lecMulticastSendType);
	break;
#endif /* I_lecMulticastSendType */
	
#ifdef I_lecMulticastSendAvgRate
    case I_lecMulticastSendAvgRate:
	dp = (void *) (&data->lecMulticastSendAvgRate);
	break;
#endif /* I_lecMulticastSendAvgRate */
	
#ifdef I_lecMulticastSendPeakRate
    case I_lecMulticastSendPeakRate:
	dp = (void *) (&data->lecMulticastSendPeakRate);
	break;
#endif /* I_lecMulticastSendPeakRate */
	
#ifdef I_lecConnectionCompleteTimer
    case I_lecConnectionCompleteTimer:
	dp = (void *) (&data->lecConnectionCompleteTimer);
	break;
#endif /* I_lecConnectionCompleteTimer */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecConfigEntry data object.
 *---------------------------------------------------------------------*/
void
lecConfigEntry_free (lecConfigEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lecOwner);
	FreeOctetString(data->lecConfigLanName);
	FreeOctetString(data->lecConfigLesAtmAddress);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecConfigEntry_cleanup (doList_t *trash)
{
    lecConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecConfigEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecConfigEntry family
 *---------------------------------------------------------------------*/
lecConfigEntry_t *
Clone_lecConfigEntry (lecConfigEntry_t *lecConfigEntry)
{
    lecConfigEntry_t *data;
    
    data = malloc(sizeof(lecConfigEntry_t));
    if (data == NULL)  {
	return(NULL);
    }
    memcpy((char *) (data), (char *) (lecConfigEntry),
	   sizeof(lecConfigEntry_t)); 
    
    if (lecConfigEntry->lecOwner)
	data->lecOwner = CloneOctetString(lecConfigEntry->lecOwner);
    if (lecConfigEntry->lecConfigLanName)
	data->lecConfigLanName =
	    CloneOctetString(lecConfigEntry->lecConfigLanName);
    if (lecConfigEntry->lecConfigLesAtmAddress)
	data->lecConfigLesAtmAddress =
	    CloneOctetString(lecConfigEntry->lecConfigLesAtmAddress); 
    
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
lecConfigEntry_test (
		     OID            *incoming,
		     ObjectInfo     *object,
		     ObjectSyntax   *value,
		     doList_t       *doHead,
		     doList_t       *doCur,
		     ContextInfo    *contextInfo)
{
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    lecConfigEntry_t     *lecConfigEntry;
    long           lecIndex;
    
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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    lecConfigEntry = k_lecConfigEntry_get(-1, contextInfo, -1, EXACT,
					  lecIndex); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecConfigEntry_set) &&
            (((lecConfigEntry_t *) (dp->data)) != NULL) &&
            (((lecConfigEntry_t *) (dp->data))->lecIndex == lecIndex) ) {
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecConfigEntry_set;
        dp->cleanupMethod = lecConfigEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecConfigEntry_UNDO
        dp->undoMethod = lecConfigEntry_undo;
#else /* SR_lecConfigEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecConfigEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecConfigEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecConfigEntry(lecConfigEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecConfigEntry(lecConfigEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(lecConfigEntry_t))) == NULL) {  
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(lecConfigEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecConfigEntry_t *) (dp->data))->lecIndex = (lecIndex);
                SET_VALID(I_lecIndex, ((lecConfigEntry_t *)
				       (dp->data))->valid); 
		
                error_status = k_lecConfigEntry_set_defaults(dp);
            }
        }
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecRowStatus
    case I_lecRowStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecRowStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecConfigEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecRowStatus_notInService) ||
	       (value->sl_value == D_lecRowStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecRowStatus_createAndGo) ||
		(value->sl_value == D_lecRowStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecConfigEntry_t *) (dp->data))->lecRowStatus = value->sl_value;
	break;
#endif /* I_lecRowStatus */
	
#ifdef I_lecOwner
    case I_lecOwner:
	
	if (((lecConfigEntry_t *) (dp->data))->lecOwner != NULL) {
	    FreeOctetString(((lecConfigEntry_t *) (dp->data))->lecOwner);
	}
	
	if (value->os_value)
	    ((lecConfigEntry_t *) (dp->data))->lecOwner = 
		CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecOwner */
	
#ifdef I_lecConfigMode
    case I_lecConfigMode:
	
	if ((value->sl_value < 1) || (value->sl_value > 2)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecConfigMode = value->sl_value;
	break;
#endif /* I_lecConfigMode */
	
#ifdef I_lecConfigLanType
    case I_lecConfigLanType:
	
	if ((value->sl_value < 1) || (value->sl_value > 3)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecConfigLanType = value->sl_value;
	break;
#endif /* I_lecConfigLanType */
	
#ifdef I_lecConfigMaxDataFrameSize
    case I_lecConfigMaxDataFrameSize:
	
	if ((value->sl_value < 1) || (value->sl_value > 5)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecConfigMaxDataFrameSize =
	    value->sl_value; 
	break;
#endif /* I_lecConfigMaxDataFrameSize */
	
#ifdef I_lecConfigLanName
    case I_lecConfigLanName:
	
	if (((lecConfigEntry_t *) (dp->data))->lecConfigLanName != NULL) {
	    FreeOctetString(((lecConfigEntry_t *)
			     (dp->data))->lecConfigLanName); 
	}
	
	if (value->os_value)
	    ((lecConfigEntry_t *) (dp->data))->lecConfigLanName = 
		CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecConfigLanName */
	
#ifdef I_lecConfigLesAtmAddress
    case I_lecConfigLesAtmAddress:
	
	if (((lecConfigEntry_t *) (dp->data))->lecConfigLesAtmAddress
	    != NULL) { 
	    FreeOctetString(((lecConfigEntry_t *)
			     (dp->data))->lecConfigLesAtmAddress); 
	}
	
	if (value->os_value)
	    ((lecConfigEntry_t *) (dp->data))->lecConfigLesAtmAddress = 
		CloneOctetString(value->os_value);
	
	break;
#endif /* I_lecConfigLesAtmAddress */
	
#ifdef I_lecControlTimeout
    case I_lecControlTimeout:
	
	if ( ((value->sl_value < 10) || (value->sl_value > 300)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecControlTimeout = value->sl_value;
	break;
#endif /* I_lecControlTimeout */
	
#ifdef I_lecMaxUnknownFrameCount
    case I_lecMaxUnknownFrameCount:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 10)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecMaxUnknownFrameCount =
	    value->sl_value; 
	break;
#endif /* I_lecMaxUnknownFrameCount */
	
#ifdef I_lecMaxUnknownFrameTime
    case I_lecMaxUnknownFrameTime:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 60)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecMaxUnknownFrameTime =
	    value->sl_value; 
	break;
#endif /* I_lecMaxUnknownFrameTime */
	
#ifdef I_lecVccTimeoutPeriod
    case I_lecVccTimeoutPeriod:
	
	((lecConfigEntry_t *) (dp->data))->lecVccTimeoutPeriod =
	    value->sl_value; 
	break;
#endif /* I_lecVccTimeoutPeriod */
	
#ifdef I_lecMaxRetryCount
    case I_lecMaxRetryCount:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 2)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecMaxRetryCount = value->sl_value;
	break;
#endif /* I_lecMaxRetryCount */
	
#ifdef I_lecAgingTime
    case I_lecAgingTime:
	
	if ( ((value->sl_value < 10) || (value->sl_value > 300)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecAgingTime = value->sl_value;
	break;
#endif /* I_lecAgingTime */
	
#ifdef I_lecForwardDelayTime
    case I_lecForwardDelayTime:
	
	if ( ((value->sl_value < 4) || (value->sl_value > 30)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecForwardDelayTime =
	    value->sl_value; 
	break;
#endif /* I_lecForwardDelayTime */
	
#ifdef I_lecExpectedArpResponseTime
    case I_lecExpectedArpResponseTime:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 30)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecExpectedArpResponseTime
	    = value->sl_value; 
	break;
#endif /* I_lecExpectedArpResponseTime */
	
#ifdef I_lecFlushTimeOut
    case I_lecFlushTimeOut:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 4)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecFlushTimeOut = value->sl_value;
	break;
#endif /* I_lecFlushTimeOut */
	
#ifdef I_lecPathSwitchingDelay
    case I_lecPathSwitchingDelay:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 8)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecPathSwitchingDelay =
	    value->sl_value; 
	break;
#endif /* I_lecPathSwitchingDelay */
	
#ifdef I_lecLocalSegmentID
    case I_lecLocalSegmentID:
	
	if ( ((value->sl_value < 0) || (value->sl_value > 4095)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecLocalSegmentID = value->sl_value;
	break;
#endif /* I_lecLocalSegmentID */
	
#ifdef I_lecMulticastSendType
    case I_lecMulticastSendType:
	
	if ((value->sl_value < 1) || (value->sl_value > 3)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecMulticastSendType =
	    value->sl_value; 
	break;
#endif /* I_lecMulticastSendType */
	
#ifdef I_lecMulticastSendAvgRate
    case I_lecMulticastSendAvgRate:
	
	((lecConfigEntry_t *) (dp->data))->lecMulticastSendAvgRate =
	    value->sl_value; 
	break;
#endif /* I_lecMulticastSendAvgRate */
	
#ifdef I_lecMulticastSendPeakRate
    case I_lecMulticastSendPeakRate:
	
	((lecConfigEntry_t *) (dp->data))->lecMulticastSendPeakRate =
	    value->sl_value; 
	break;
#endif /* I_lecMulticastSendPeakRate */
	
#ifdef I_lecConnectionCompleteTimer
    case I_lecConnectionCompleteTimer:
	
	if ( ((value->sl_value < 1) || (value->sl_value > 10)) ) {
	    return WRONG_VALUE_ERROR;
	}
	
	((lecConfigEntry_t *) (dp->data))->lecConnectionCompleteTimer
	    = value->sl_value; 
	break;
#endif /* I_lecConnectionCompleteTimer */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecConfigEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecConfigEntry_test */
    error_status = k_lecConfigEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecConfigEntry_t *) (dp->data))->valid);
        error_status = k_lecConfigEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecConfigEntry_set (
		    doList_t       *doHead,
		    doList_t       *doCur,
		    ContextInfo    *contextInfo)
{
    return (k_lecConfigEntry_set((lecConfigEntry_t *) (doCur->data),
				 contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecStatusEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecStatusEntry_get (
		    OID             *incoming,
		    ObjectInfo      *object,
		    int             searchType,
		    ContextInfo     *contextInfo,
		    int             serialNum)
{
    int             instLength;
    int             arg;
    void            *dp;
    lecStatusEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            lecIndex;
    
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecStatusEntry_get(serialNum,
						     contextInfo, arg
						     ,searchType,
						     lecIndex)) ==
	NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->lecIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecIfIndex
    case I_lecIfIndex:
	dp = (void *) (&data->lecIfIndex);
	break;
#endif /* I_lecIfIndex */
	
#ifdef I_lecPrimaryAtmAddress
    case I_lecPrimaryAtmAddress:
	if (data->lecPrimaryAtmAddress)
	    dp = (void *) (CloneOctetString(data->lecPrimaryAtmAddress));
	break;
#endif /* I_lecPrimaryAtmAddress */
	
#ifdef I_lecID
    case I_lecID:
	dp = (void *) (&data->lecID);
	break;
#endif /* I_lecID */
	
#ifdef I_lecInterfaceState
    case I_lecInterfaceState:
	dp = (void *) (&data->lecInterfaceState);
	break;
#endif /* I_lecInterfaceState */
	
#ifdef I_lecLastFailureRespCode
    case I_lecLastFailureRespCode:
	dp = (void *) (&data->lecLastFailureRespCode);
	break;
#endif /* I_lecLastFailureRespCode */
	
#ifdef I_lecLastFailureState
    case I_lecLastFailureState:
	dp = (void *) (&data->lecLastFailureState);
	break;
#endif /* I_lecLastFailureState */

#ifdef I_lecProtocol
    case I_lecProtocol:
	dp = (void *) (&data->lecProtocol);
	break;
#endif /* I_lecProtocol */
	
#ifdef I_lecVersion
    case I_lecVersion:
	dp = (void *) (&data->lecVersion);
	break;
#endif /* I_lecVersion */
	
#ifdef I_lecTopologyChange
    case I_lecTopologyChange:
       dp = (void *) (&data->lecTopologyChange);
	break;
#endif /* I_lecTopologyChange */
	
#ifdef I_lecConfigServerAtmAddress
    case I_lecConfigServerAtmAddress:
	if (data->lecConfigServerAtmAddress)
	    dp = (void *) (CloneOctetString(data->lecConfigServerAtmAddress));
	break;
#endif /* I_lecConfigServerAtmAddress */
	
#ifdef I_lecConfigSource
    case I_lecConfigSource:
	dp = (void *) (&data->lecConfigSource);
	break;
#endif /* I_lecConfigSource */
	
#ifdef I_lecActualLanType
    case I_lecActualLanType:
	dp = (void *) (&data->lecActualLanType);
	break;
#endif /* I_lecActualLanType */
	
#ifdef I_lecActualMaxDataFrameSize
    case I_lecActualMaxDataFrameSize:
	dp = (void *) (&data->lecActualMaxDataFrameSize);
	break;
#endif /* I_lecActualMaxDataFrameSize */
	
#ifdef I_lecActualLanName
    case I_lecActualLanName:
	if (data->lecActualLanName)
	    dp = (void *) (CloneOctetString(data->lecActualLanName));
	break;
#endif /* I_lecActualLanName */
	
#ifdef I_lecActualLesAtmAddress
    case I_lecActualLesAtmAddress:
	if (data->lecActualLesAtmAddress)
	    dp = (void *) (CloneOctetString(data->lecActualLesAtmAddress));
	break;
#endif /* I_lecActualLesAtmAddress */
	
#ifdef I_lecProxyClient
    case I_lecProxyClient:
	dp = (void *) (&data->lecProxyClient);
	break;
#endif /* I_lecProxyClient */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lecMappingEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecMappingEntry_get (
		     OID             *incoming,
		     ObjectInfo      *object,
		     int             searchType,
		     ContextInfo     *contextInfo,
		     int             serialNum)
{
    int             instLength;
    int             arg;
    void            *dp;
    lecMappingEntry_t *data;
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
     if ( (arg == -1) || (data = k_lecMappingEntry_get(serialNum,
						       contextInfo,
						       arg
						       ,searchType,
						       ifIndex)) ==
	 NULL) { 
	 arg = -1;
     } else {
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
	
#ifdef I_lecMappingIndex
    case I_lecMappingIndex:
	dp = (void *) (&data->lecMappingIndex);
	break;
#endif /* I_lecMappingIndex */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lecStatisticsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecStatisticsEntry_get (
			OID             *incoming,
			ObjectInfo      *object,
			int             searchType,
			ContextInfo     *contextInfo,
			int             serialNum)
{
    int             instLength;
    int             arg;
    void            *dp;
    lecStatisticsEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            lecIndex;
    
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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecStatisticsEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 lecIndex)) ==
	NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->lecIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecArpRequestsOut
    case I_lecArpRequestsOut:
	dp = (void *) (&data->lecArpRequestsOut);
	break;
#endif /* I_lecArpRequestsOut */
	
#ifdef I_lecArpRequestsIn
    case I_lecArpRequestsIn:
	dp = (void *) (&data->lecArpRequestsIn);
	break;
#endif /* I_lecArpRequestsIn */
	
#ifdef I_lecArpRepliesOut
    case I_lecArpRepliesOut:
	dp = (void *) (&data->lecArpRepliesOut);
	break;
#endif /* I_lecArpRepliesOut */
	
#ifdef I_lecArpRepliesIn
    case I_lecArpRepliesIn:
	dp = (void *) (&data->lecArpRepliesIn);
	break;
#endif /* I_lecArpRepliesIn */
	
#ifdef I_lecControlFramesOut
    case I_lecControlFramesOut:
	dp = (void *) (&data->lecControlFramesOut);
	break;
#endif /* I_lecControlFramesOut */
	
#ifdef I_lecControlFramesIn
    case I_lecControlFramesIn:
	dp = (void *) (&data->lecControlFramesIn);
	break;
#endif /* I_lecControlFramesIn */
	
#ifdef I_lecSvcFailures
    case I_lecSvcFailures:
	dp = (void *) (&data->lecSvcFailures);
	break;
#endif /* I_lecSvcFailures */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lecServerVccEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecServerVccEntry_get (
		       OID             *incoming,
		       ObjectInfo      *object,
		       int             searchType,
		       ContextInfo     *contextInfo,
		       int             serialNum)
{
    int             instLength;
    int             arg;
    void            *dp;
    lecServerVccEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            lecIndex;
    
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
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecServerVccEntry_get(serialNum,
							contextInfo,
							arg
							,searchType,
							lecIndex)) ==
	NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->lecIndex;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecConfigDirectInterface
    case I_lecConfigDirectInterface:
	dp = (void *) (&data->lecConfigDirectInterface);
	break;
#endif /* I_lecConfigDirectInterface */
	
#ifdef I_lecConfigDirectVpi
    case I_lecConfigDirectVpi:
	dp = (void *) (&data->lecConfigDirectVpi);
	break;
#endif /* I_lecConfigDirectVpi */
	
#ifdef I_lecConfigDirectVci
    case I_lecConfigDirectVci:
	dp = (void *) (&data->lecConfigDirectVci);
	break;
#endif /* I_lecConfigDirectVci */
	
#ifdef I_lecControlDirectInterface
    case I_lecControlDirectInterface:
	dp = (void *) (&data->lecControlDirectInterface);
	break;
#endif /* I_lecControlDirectInterface */
	
#ifdef I_lecControlDirectVpi
    case I_lecControlDirectVpi:
	dp = (void *) (&data->lecControlDirectVpi);
	break;
#endif /* I_lecControlDirectVpi */
	
#ifdef I_lecControlDirectVci
    case I_lecControlDirectVci:
	dp = (void *) (&data->lecControlDirectVci);
	break;
#endif /* I_lecControlDirectVci */
	
#ifdef I_lecControlDistributeInterface
    case I_lecControlDistributeInterface:
	dp = (void *) (&data->lecControlDistributeInterface);
	break;
#endif /* I_lecControlDistributeInterface */
	
#ifdef I_lecControlDistributeVpi
    case I_lecControlDistributeVpi:
	dp = (void *) (&data->lecControlDistributeVpi);
	break;
#endif /* I_lecControlDistributeVpi */
	
#ifdef I_lecControlDistributeVci
    case I_lecControlDistributeVci:
	dp = (void *) (&data->lecControlDistributeVci);
	break;
#endif /* I_lecControlDistributeVci */
	
#ifdef I_lecMulticastSendInterface
    case I_lecMulticastSendInterface:
	dp = (void *) (&data->lecMulticastSendInterface);
	break;
#endif /* I_lecMulticastSendInterface */
	
#ifdef I_lecMulticastSendVpi
    case I_lecMulticastSendVpi:
	dp = (void *) (&data->lecMulticastSendVpi);
	break;
#endif /* I_lecMulticastSendVpi */
	
#ifdef I_lecMulticastSendVci
    case I_lecMulticastSendVci:
	dp = (void *) (&data->lecMulticastSendVci);
	break;
#endif /* I_lecMulticastSendVci */
	
#ifdef I_lecMulticastForwardInterface
    case I_lecMulticastForwardInterface:
	dp = (void *) (&data->lecMulticastForwardInterface);
	break;
#endif /* I_lecMulticastForwardInterface */
	
#ifdef I_lecMulticastForwardVpi
    case I_lecMulticastForwardVpi:
	dp = (void *) (&data->lecMulticastForwardVpi);
	break;
#endif /* I_lecMulticastForwardVpi */
	
#ifdef I_lecMulticastForwardVci
    case I_lecMulticastForwardVci:
	dp = (void *) (&data->lecMulticastForwardVci);
	break;
#endif /* I_lecMulticastForwardVci */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lecAtmAddressEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecAtmAddressEntry_get (
			OID             *incoming,
			ObjectInfo      *object,
			int             searchType,
			ContextInfo     *contextInfo,
			int             serialNum)
{
    int             arg;
    void            *dp;
    lecAtmAddressEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            lecIndex;
    int             lecIndex_offset;
    OctetString *   lecAtmAddress;
    int             lecAtmAddress_offset;
    int             index;
    int             final_index;

    arg = object->nominator;
    data = NULL;
    lecAtmAddress = NULL;
    
    lecIndex_offset = object->oid.length;
    lecAtmAddress_offset = lecIndex_offset + 1;
    final_index = lecAtmAddress_offset +
	GetVariableIndexLength(incoming, lecAtmAddress_offset); 

    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToVariableOctetString(incoming, lecAtmAddress_offset,
				    &lecAtmAddress, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToInt(incoming, lecIndex_offset, &lecIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecAtmAddressEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 lecIndex,
							 lecAtmAddress)) == NULL) {
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->lecIndex;
        inst.oid_ptr[index++] = data->lecAtmAddress->length;
        for(i = 0; i < data->lecAtmAddress->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecAtmAddress->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
    }
    
    FreeOctetString(lecAtmAddress);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecAtmAddressStatus
    case I_lecAtmAddressStatus:
	dp = (void *) (&data->lecAtmAddressStatus);
	break;
#endif /* I_lecAtmAddressStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the lecAtmAddressEntry data object.
 *---------------------------------------------------------------------*/
void
lecAtmAddressEntry_free (lecAtmAddressEntry_t *data)
{
    if (data != NULL) {
	FreeOctetString(data->lecAtmAddress);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after lecAtmAddressEntry set/undo
 *---------------------------------------------------------------------*/
static int
lecAtmAddressEntry_cleanup (doList_t *trash)
{
    lecAtmAddressEntry_free(trash->data);
#ifdef SR_SNMPv2
    lecAtmAddressEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the lecAtmAddressEntry family
 *---------------------------------------------------------------------*/
lecAtmAddressEntry_t *
Clone_lecAtmAddressEntry (lecAtmAddressEntry_t *lecAtmAddressEntry)
{
    lecAtmAddressEntry_t *data;

    data = malloc(sizeof(lecAtmAddressEntry_t));
    if (data == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (lecAtmAddressEntry),
	   sizeof(lecAtmAddressEntry_t));
    
    if (lecAtmAddressEntry->lecAtmAddress)
	data->lecAtmAddress =
	    CloneOctetString(lecAtmAddressEntry->lecAtmAddress); 
    
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
lecAtmAddressEntry_test (
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
    lecAtmAddressEntry_t     *lecAtmAddressEntry;
    long           lecIndex;
    int            lecIndex_offset;
    OctetString *  lecAtmAddress;
    int            lecAtmAddress_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    lecAtmAddress = NULL;

    /*
     * Validate the object instance
     *
     */
    lecIndex_offset = object->oid.length;
    lecAtmAddress_offset = lecIndex_offset + 1;
    final_index = lecAtmAddress_offset +
	GetVariableIndexLength(incoming, lecAtmAddress_offset); 
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToInt(incoming, lecIndex_offset, &lecIndex, EXACT,
		    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToVariableOctetString(incoming, lecAtmAddress_offset,
				    &lecAtmAddress, EXACT, &carry)) <
	0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(lecAtmAddress);
        return error_status;
    }
    
    lecAtmAddressEntry = k_lecAtmAddressEntry_get(-1, contextInfo, -1,
						  EXACT, lecIndex,
						  lecAtmAddress); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == lecAtmAddressEntry_set) &&
            (((lecAtmAddressEntry_t *) (dp->data)) != NULL) &&
            (((lecAtmAddressEntry_t *) (dp->data))->lecIndex == lecIndex) &&
            (CmpOctetStrings(((lecAtmAddressEntry_t *)
			      (dp->data))->lecAtmAddress,
			     lecAtmAddress) == 0) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = lecAtmAddressEntry_set;
        dp->cleanupMethod = lecAtmAddressEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_lecAtmAddressEntry_UNDO
        dp->undoMethod = lecAtmAddressEntry_undo;
#else /* SR_lecAtmAddressEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_lecAtmAddressEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (lecAtmAddressEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *)
		 Clone_lecAtmAddressEntry(lecAtmAddressEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_lecAtmAddressEntry(lecAtmAddressEntry)) ==
		NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(lecAtmAddressEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(lecAtmAddressEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((lecAtmAddressEntry_t *) (dp->data))->lecIndex = (lecIndex);
                SET_VALID(I_lecIndex, ((lecAtmAddressEntry_t *)
				       (dp->data))->valid); 
		
		if (lecAtmAddress)
		    ((lecAtmAddressEntry_t *)
		     (dp->data))->lecAtmAddress =
			 CloneOctetString(lecAtmAddress); 
                SET_VALID(I_lecAtmAddress, ((lecAtmAddressEntry_t *)
					    (dp->data))->valid); 
		
                error_status = k_lecAtmAddressEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(lecAtmAddress);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_lecAtmAddressStatus
    case I_lecAtmAddressStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_lecAtmAddressStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (lecAtmAddressEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_lecAtmAddressStatus_notInService)
	       || (value->sl_value == D_lecAtmAddressStatus_active)) {
		
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_lecAtmAddressStatus_createAndGo)
		|| (value->sl_value ==
		    D_lecAtmAddressStatus_createAndWait)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((lecAtmAddressEntry_t *) (dp->data))->lecAtmAddressStatus =
	    value->sl_value; 
	break;
#endif /* I_lecAtmAddressStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in lecAtmAddressEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_lecAtmAddressEntry_test */
    error_status = k_lecAtmAddressEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((lecAtmAddressEntry_t *)
				      (dp->data))->valid); 
        error_status = k_lecAtmAddressEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
lecAtmAddressEntry_set (
			doList_t       *doHead,
			doList_t       *doCur,
			ContextInfo    *contextInfo)
{
  return (k_lecAtmAddressEntry_set((lecAtmAddressEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the lecMacAddressEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecMacAddressEntry_get (
			OID             *incoming,
			ObjectInfo      *object,
			int             searchType,
			ContextInfo     *contextInfo,
			int             serialNum)
{
    int             arg;
    void            *dp;
    lecMacAddressEntry_t *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            lecIndex;
    int             lecIndex_offset;
    OctetString *   lecMacAddress;
    int             lecMacAddress_offset;
    int             index;
    int             final_index;
    
    arg = object->nominator;
    dp = NULL;
    data = NULL;
    lecMacAddress = NULL;
    
    lecIndex_offset = object->oid.length;
    lecMacAddress_offset = lecIndex_offset + 1;
    final_index = lecMacAddress_offset +
	GetVariableIndexLength(incoming, lecMacAddress_offset); 
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToVariableOctetString(incoming, lecMacAddress_offset,
				    &lecMacAddress, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToInt(incoming, lecIndex_offset, &lecIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecMacAddressEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 lecIndex,
							 lecMacAddress)) == NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->lecIndex;
        inst.oid_ptr[index++] = data->lecMacAddress->length;
        for(i = 0; i < data->lecMacAddress->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->lecMacAddress->octet_ptr[i]; 
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
    }
    
    FreeOctetString(lecMacAddress);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecMacAddressAtmBinding
    case I_lecMacAddressAtmBinding:
	if (data->lecMacAddressAtmBinding)
	    dp = (void *) (CloneOctetString(data->lecMacAddressAtmBinding));
	break;
#endif /* I_lecMacAddressAtmBinding */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the lecRouteDescrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
lecRouteDescrEntry_get (
			OID             *incoming,
			ObjectInfo      *object,
			int             searchType,
			ContextInfo     *contextInfo,
			int             serialNum)
{
    int             instLength;
    int             arg;
    void            *dp;
    lecRouteDescrEntry_t *data;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            lecIndex;
    long            lecRouteDescrSegmentID;
    long            lecRouteDescrBridgeNumber;
    
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
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
		    &lecRouteDescrBridgeNumber, searchType, &carry)) <
	0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecRouteDescrBridgeNumber = MAX(0, lecRouteDescrBridgeNumber);
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length,
		    &lecRouteDescrSegmentID, searchType, &carry)) < 0
	) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecRouteDescrSegmentID = MAX(0, lecRouteDescrSegmentID);
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_lecRouteDescrEntry_get(serialNum,
							 contextInfo,
							 arg
							 ,searchType,
							 lecIndex,
							 lecRouteDescrSegmentID, lecRouteDescrBridgeNumber)) == NULL) {  
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->lecIndex;
        inst.oid_ptr[1] = data->lecRouteDescrSegmentID;
        inst.oid_ptr[2] = data->lecRouteDescrBridgeNumber;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_lecRouteDescrAtmBinding
    case I_lecRouteDescrAtmBinding:
	if (data->lecRouteDescrAtmBinding)
	    dp = (void *) (CloneOctetString(data->lecRouteDescrAtmBinding));
	break;
#endif /* I_lecRouteDescrAtmBinding */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

/*---------------------------------------------------------------------
 * Retrieve data from the leArpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
leArpEntry_get (
		OID             *incoming,
		ObjectInfo      *object,
		int             searchType,
		ContextInfo     *contextInfo,
		int             serialNum)
{
    int             arg;
    void            *dp;
    leArpEntry_t    *data;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            lecIndex;
    int             lecIndex_offset;
    OctetString *   leArpMacAddress;
    int             leArpMacAddress_offset;
    int             index;
    int             final_index;
    
    arg = object->nominator;
    dp = NULL;
    data = NULL;
    leArpMacAddress = NULL;
    leArpMacAddress = NULL;
    leArpMacAddress = NULL;
    
    lecIndex_offset = object->oid.length;
    leArpMacAddress_offset = lecIndex_offset + 1;
    final_index = leArpMacAddress_offset +
	GetVariableIndexLength(incoming, leArpMacAddress_offset); 
    
    if (searchType == EXACT) {
	if (final_index != incoming->length) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }
    
    if ( (InstToVariableOctetString(incoming, leArpMacAddress_offset,
				    &leArpMacAddress, searchType,
				    &carry)) < 0 ) { 
	arg = -1;
    }
    
    if ( (InstToInt(incoming, lecIndex_offset, &lecIndex, searchType,
		    &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_leArpEntry_get(serialNum,
						 contextInfo, arg
						 ,searchType,
						 lecIndex,
						 leArpMacAddress)) ==
	NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->lecIndex;
        inst.oid_ptr[index++] = data->leArpMacAddress->length;
        for(i = 0; i < data->leArpMacAddress->length; i++) {
            inst.oid_ptr[index++] = (unsigned long)
		data->leArpMacAddress->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
    }
    
    FreeOctetString(leArpMacAddress);
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_leArpAtmAddress
    case I_leArpAtmAddress:
	if (data->leArpAtmAddress)
	    dp = (void *) (CloneOctetString(data->leArpAtmAddress));
	break;
#endif /* I_leArpAtmAddress */
	
#ifdef I_leArpIsRemoteAddress
    case I_leArpIsRemoteAddress:
	dp = (void *) (&data->leArpIsRemoteAddress);
	break;
#endif /* I_leArpIsRemoteAddress */
	
#ifdef I_leArpEntryType
    case I_leArpEntryType:
	dp = (void *) (&data->leArpEntryType);
	break;
#endif /* I_leArpEntryType */
	
#ifdef I_leArpRowStatus
    case I_leArpRowStatus:
	dp = (void *) (&data->leArpRowStatus);
	break;
#endif /* I_leArpRowStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the leArpEntry data object.
 *---------------------------------------------------------------------*/
void
leArpEntry_free (leArpEntry_t    *data)
{
    if (data != NULL) {
	FreeOctetString(data->leArpMacAddress);
	FreeOctetString(data->leArpAtmAddress);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after leArpEntry set/undo
 *---------------------------------------------------------------------*/
static int
leArpEntry_cleanup (doList_t *trash)
{
    leArpEntry_free(trash->data);
#ifdef SR_SNMPv2
    leArpEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the leArpEntry family
 *---------------------------------------------------------------------*/
leArpEntry_t *
Clone_leArpEntry (leArpEntry_t *leArpEntry)
{
    leArpEntry_t *data;
    
    data = malloc(sizeof(leArpEntry_t));
    if (data == NULL) {
	return(NULL);
    }
    memcpy((char *) (data), (char *) (leArpEntry), sizeof(leArpEntry_t));
    
    if (leArpEntry->leArpMacAddress)
	data->leArpMacAddress =
	    CloneOctetString(leArpEntry->leArpMacAddress);
    if (leArpEntry->leArpAtmAddress)
	data->leArpAtmAddress =
	    CloneOctetString(leArpEntry->leArpAtmAddress);
    
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
leArpEntry_test (
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
    leArpEntry_t     *leArpEntry;
    long           lecIndex;
    int            lecIndex_offset;
    OctetString *  leArpMacAddress;
    int            leArpMacAddress_offset;
    int            final_index;
    
    error_status = NO_ERROR;
    carry = 0;
    leArpMacAddress = NULL;

    /*
     * Validate the object instance
     *
     */
    lecIndex_offset = object->oid.length;
    leArpMacAddress_offset = lecIndex_offset + 1;
    final_index = leArpMacAddress_offset +
	GetVariableIndexLength(incoming, leArpMacAddress_offset); 
    
    if (final_index != incoming->length) {
	return(NO_CREATION_ERROR);
    }
    
    if ( (InstToInt(incoming, lecIndex_offset, &lecIndex, EXACT,
		    &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToVariableOctetString(incoming, leArpMacAddress_offset,
				    &leArpMacAddress, EXACT, &carry))
	< 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        FreeOctetString(leArpMacAddress);
        return error_status;
    }
    
    leArpEntry = k_leArpEntry_get(-1, contextInfo, -1, EXACT,
				  lecIndex, leArpMacAddress); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == leArpEntry_set) &&
            (((leArpEntry_t *) (dp->data)) != NULL) &&
            (((leArpEntry_t *) (dp->data))->lecIndex == lecIndex) &&
            (CmpOctetStrings(((leArpEntry_t *)
			      (dp->data))->leArpMacAddress,
			     leArpMacAddress) == 0) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = leArpEntry_set;
        dp->cleanupMethod = leArpEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_leArpEntry_UNDO
        dp->undoMethod = leArpEntry_undo;
#else /* SR_leArpEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_leArpEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (leArpEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_leArpEntry(leArpEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_leArpEntry(leArpEntry))
		== NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(leArpEntry_t))) == NULL) {  
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(leArpEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((leArpEntry_t *) (dp->data))->lecIndex = (lecIndex);
                SET_VALID(I_lecIndex, ((leArpEntry_t *) (dp->data))->valid);
		
		if (leArpMacAddress)
		    ((leArpEntry_t *) (dp->data))->leArpMacAddress =
			CloneOctetString(leArpMacAddress); 
                SET_VALID(I_leArpMacAddress, ((leArpEntry_t *)
					      (dp->data))->valid); 
		
                error_status = k_leArpEntry_set_defaults(dp);
            }
        }
    }
    
    FreeOctetString(leArpMacAddress);
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_leArpAtmAddress
    case I_leArpAtmAddress:
	
	if (((leArpEntry_t *) (dp->data))->leArpAtmAddress != NULL) {
	    FreeOctetString(((leArpEntry_t *) (dp->data))->leArpAtmAddress);
	}
	
	if (value->os_value)
	    ((leArpEntry_t *) (dp->data))->leArpAtmAddress = 
		CloneOctetString(value->os_value);
	
	break;
#endif /* I_leArpAtmAddress */
	
#ifdef I_leArpEntryType
    case I_leArpEntryType:
	
	if ((value->sl_value < 1) || (value->sl_value > 5)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((leArpEntry_t *) (dp->data))->leArpEntryType = value->sl_value;
	break;
#endif /* I_leArpEntryType */
	
#ifdef I_leArpRowStatus
    case I_leArpRowStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_leArpRowStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (leArpEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_leArpRowStatus_notInService) ||
	       (value->sl_value == D_leArpRowStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_leArpRowStatus_createAndGo) ||
		(value->sl_value == D_leArpRowStatus_createAndWait)) {
		
		return INCONSISTENT_VALUE_ERROR;
	    }
	}
	
	((leArpEntry_t *) (dp->data))->leArpRowStatus = value->sl_value;
	break;
#endif /* I_leArpRowStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in leArpEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_leArpEntry_test */
    error_status = k_leArpEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((leArpEntry_t *) (dp->data))->valid);
        error_status = k_leArpEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
leArpEntry_set (
		doList_t       *doHead,
		doList_t       *doCur,
		ContextInfo    *contextInfo)
{
    return (k_leArpEntry_set((leArpEntry_t *) (doCur->data),
			     contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the leRDArpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
leRDArpEntry_get (
		  OID             *incoming,
		  ObjectInfo      *object,
		  int             searchType,
		  ContextInfo     *contextInfo,
		  int             serialNum)
{
    int             instLength;
    int             arg;
    void            *dp;
    leRDArpEntry_t  *data;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            lecIndex;
    long            leRDArpSegmentID;
    long            leRDArpBridgeNumber;
    
    instLength = incoming->length - object->oid.length;
    arg = object->nominator;
    dp = NULL;
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
		    &leRDArpBridgeNumber, searchType, &carry)) < 0 ) {
	
	arg = -1;
    }
    if (searchType == NEXT) {
        leRDArpBridgeNumber = MAX(0, leRDArpBridgeNumber);
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length,
		    &leRDArpSegmentID, searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        leRDArpSegmentID = MAX(0, leRDArpSegmentID);
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    searchType, &carry)) < 0 ) { 
	arg = -1;
    }
    if (searchType == NEXT) {
        lecIndex = MAX(1, lecIndex);
    }
    
    if (carry) {
	arg = -1;
    }
    
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_leRDArpEntry_get(serialNum,
						   contextInfo, arg
						   ,searchType,
						   lecIndex,
						   leRDArpSegmentID,
						   leRDArpBridgeNumber)) == NULL) { 
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->lecIndex;
        inst.oid_ptr[1] = data->leRDArpSegmentID;
        inst.oid_ptr[2] = data->leRDArpBridgeNumber;
    }
    
    
    /*
     * Build the variable binding for the variable that will be returned.
     */
    
    switch (arg) {
	
#ifdef I_leRDArpAtmAddress
    case I_leRDArpAtmAddress:
	if (data->leRDArpAtmAddress)
	    dp = (void *) (CloneOctetString(data->leRDArpAtmAddress));
	break;
#endif /* I_leRDArpAtmAddress */
	
#ifdef I_leRDArpEntryType
    case I_leRDArpEntryType:
	dp = (void *) (&data->leRDArpEntryType);
	break;
#endif /* I_leRDArpEntryType */
	
#ifdef I_leRDArpRowStatus
    case I_leRDArpRowStatus:
	dp = (void *) (&data->leRDArpRowStatus);
	break;
#endif /* I_leRDArpRowStatus */
	
    default:
	return ((VarBind *) NULL);
	
    }      /* switch */
    
    return (MakeVarBind(object, &inst, dp));
    
}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the leRDArpEntry data object.
 *---------------------------------------------------------------------*/
void
leRDArpEntry_free (leRDArpEntry_t  *data)
{
    if (data != NULL) {
	FreeOctetString(data->leRDArpAtmAddress);
	
	free ((char *) data);
    }
}

/*----------------------------------------------------------------------
 * cleanup after leRDArpEntry set/undo
 *---------------------------------------------------------------------*/
static int
leRDArpEntry_cleanup (doList_t *trash)
{
    leRDArpEntry_free(trash->data);
#ifdef SR_SNMPv2
    leRDArpEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
    return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the leRDArpEntry family
 *---------------------------------------------------------------------*/
leRDArpEntry_t *
Clone_leRDArpEntry (leRDArpEntry_t *leRDArpEntry)
{
    leRDArpEntry_t *data;
    
    data = malloc(sizeof(leRDArpEntry_t));
    if (data==NULL)  {
	return(NULL);
    }
    memcpy((char *) (data), (char *) (leRDArpEntry), sizeof(leRDArpEntry_t));
    
    if (leRDArpEntry->leRDArpAtmAddress)
	data->leRDArpAtmAddress =
	    CloneOctetString(leRDArpEntry->leRDArpAtmAddress); 
    
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
leRDArpEntry_test (
		   OID            *incoming,
		   ObjectInfo     *object,
		   ObjectSyntax   *value,
		   doList_t       *doHead,
		   doList_t       *doCur,
		   ContextInfo    *contextInfo)
{
    int            instLength;
    doList_t       *dp;
    int            found;
    int            error_status;
    int            carry;
    leRDArpEntry_t     *leRDArpEntry;
    long           lecIndex;
    long           leRDArpSegmentID;
    long           leRDArpBridgeNumber;
    
    instLength = incoming->length - object->oid.length;
    error_status = NO_ERROR;
    carry = 0;

    /*
     * Validate the object instance
     *
     */
    if (instLength != 3) {
        return NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 0 + object->oid.length, &lecIndex,
		    EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 1 + object->oid.length,
		    &leRDArpSegmentID, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if ( (InstToInt(incoming, 2 + object->oid.length,
		    &leRDArpBridgeNumber, EXACT, &carry)) < 0 ) { 
        error_status = NO_CREATION_ERROR;
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    leRDArpEntry = k_leRDArpEntry_get(-1, contextInfo, -1, EXACT,
				      lecIndex, leRDArpSegmentID,
				      leRDArpBridgeNumber); 
    
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
    
    for (dp = doHead; dp != NULL; dp = dp->next) { 
	if ( (dp->setMethod == leRDArpEntry_set) &&
            (((leRDArpEntry_t *) (dp->data)) != NULL) &&
            (((leRDArpEntry_t *) (dp->data))->lecIndex == lecIndex) &&
            (((leRDArpEntry_t *) (dp->data))->leRDArpSegmentID ==
	     leRDArpSegmentID) && 
            (((leRDArpEntry_t *) (dp->data))->leRDArpBridgeNumber ==
	     leRDArpBridgeNumber) ) { 
	    
            found = 1;
            break; 
        }
    }
    
    if (!found) {
        dp = doCur;
	
        dp->setMethod = leRDArpEntry_set;
        dp->cleanupMethod = leRDArpEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_leRDArpEntry_UNDO
        dp->undoMethod = leRDArpEntry_undo;
#else /* SR_leRDArpEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_leRDArpEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;
	
        if (leRDArpEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_leRDArpEntry(leRDArpEntry))
		== NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *)
		 Clone_leRDArpEntry(leRDArpEntry)) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */
	    
        } else {
            if ( (dp->data = malloc(sizeof(leRDArpEntry_t))) == NULL) {  
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            } else {
                memset(dp->data, 0, sizeof(leRDArpEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */
		
                /* Fill in reasonable default values for this new entry */
                ((leRDArpEntry_t *) (dp->data))->lecIndex = (lecIndex);
                SET_VALID(I_lecIndex, ((leRDArpEntry_t *) (dp->data))->valid);
		
                ((leRDArpEntry_t *) (dp->data))->leRDArpSegmentID =
		    (leRDArpSegmentID); 
                SET_VALID(I_leRDArpSegmentID, ((leRDArpEntry_t *)
					       (dp->data))->valid); 
		
                ((leRDArpEntry_t *) (dp->data))->leRDArpBridgeNumber =
		    (leRDArpBridgeNumber); 
                SET_VALID(I_leRDArpBridgeNumber, ((leRDArpEntry_t *)
						  (dp->data))->valid);
		
		
                error_status = k_leRDArpEntry_set_defaults(dp);
            }
        }
    }
    
    if (error_status != NO_ERROR) {
        return error_status;
    }
    
    switch (object->nominator) {
	
#ifdef I_leRDArpAtmAddress
    case I_leRDArpAtmAddress:
	
	if (((leRDArpEntry_t *) (dp->data))->leRDArpAtmAddress != NULL) {
	    FreeOctetString(((leRDArpEntry_t *)
			     (dp->data))->leRDArpAtmAddress); 
	}
	
	if (value->os_value)
	    ((leRDArpEntry_t *) (dp->data))->leRDArpAtmAddress = 
		CloneOctetString(value->os_value);
	
	break;
#endif /* I_leRDArpAtmAddress */
	
#ifdef I_leRDArpEntryType
    case I_leRDArpEntryType:
	
	if ((value->sl_value < 1) || (value->sl_value > 5)) {
	    return WRONG_VALUE_ERROR;
	}
	
	((leRDArpEntry_t *) (dp->data))->leRDArpEntryType = value->sl_value;
	break;
#endif /* I_leRDArpEntryType */
	
#ifdef I_leRDArpRowStatus
    case I_leRDArpRowStatus:
	
	if ((value->sl_value < 1) || (value->sl_value > 6)) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (value->sl_value == D_leRDArpRowStatus_notReady) {
	    return WRONG_VALUE_ERROR;
	}
	
	if (leRDArpEntry == NULL) { 	/* creating a new row */
	    if((value->sl_value == D_leRDArpRowStatus_notInService) ||
	       (value->sl_value == D_leRDArpRowStatus_active)) { 
		return INCONSISTENT_VALUE_ERROR;
	    }
	} else { 			/* modifying an existing row */
	    if ((value->sl_value == D_leRDArpRowStatus_createAndGo) ||
		(value->sl_value == D_leRDArpRowStatus_createAndWait))
		{ 
		    return INCONSISTENT_VALUE_ERROR;
		}
	}
	
	((leRDArpEntry_t *) (dp->data))->leRDArpRowStatus = value->sl_value;
	break;
#endif /* I_leRDArpRowStatus */
	
    default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in leRDArpEntry_test)\n"));
	return GEN_ERROR;
	
    }        /* switch */
    
    /* Do system dependent testing in k_leRDArpEntry_test */
    error_status = k_leRDArpEntry_test(object, value, dp, contextInfo);
    
    if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((leRDArpEntry_t *) (dp->data))->valid);
        error_status = k_leRDArpEntry_ready(object, value, doHead, dp);
    }
    
    return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
leRDArpEntry_set (
		  doList_t       *doHead,
		  doList_t       *doCur,
		  ContextInfo    *contextInfo)
{
    return (k_leRDArpEntry_set((leRDArpEntry_t *) (doCur->data),
			       contextInfo, doCur->state));
}

#endif /* SETS */



/********************************************************
 *                                                      *
 *          B E G I N    C I S C O    C O D E           *
 *                                                      *
 ********************************************************/

/* +++
 * Start of system-dependent routines
 */


/*---------------------------------------------------------------------
 * convert_lecInfoState
 *
 * convert the lec_info state field into one of the values
 * defined for the LecState TC--matches def in sec 2.3.1 of the
 * LANE spec
 * INPUTS:
 *    lecInfoState == lec_info.state
 * RETURNS:
 *    LecState value
 *---------------------------------------------------------------------*/
static inline long convert_lecInfoState (lec_state_t lecInfoState)
{
    switch (lecInfoState) {
    case LEC_ST_IDLE:
	return D_lecInterfaceState_initialState;
    case LEC_ST_REGISTER_ADDR:
	return D_lecInterfaceState_initialState;
    case LEC_ST_POSTING_LISTEN:
	return D_lecInterfaceState_initialState;
    case LEC_ST_GET_LECS_ADDR:
 	return D_lecInterfaceState_initialState;
    case LEC_ST_LECS_CONNECT:
 	return D_lecInterfaceState_lecsConnect;
    case LEC_ST_GET_LES_ADDR:
 	return D_lecInterfaceState_configure;
    case LEC_ST_LECS_RELEASE:
    case LEC_ST_CTL_DIRECT_CONN:
 	return D_lecInterfaceState_join;
    case LEC_ST_JOIN_CTL_DIST_CONN:
 	return D_lecInterfaceState_join;
    case LEC_ST_CTL_DIST_CONN:
 	return D_lecInterfaceState_join;
    case LEC_ST_JOIN:
 	return D_lecInterfaceState_join;
    case LEC_ST_REGISTRATION:
 		return D_lecInterfaceState_initialRegistration;
    case LEC_ST_GET_BUS_ADDR:
 	return D_lecInterfaceState_busConnect;
    case LEC_ST_MCAST_SEND_FORWARD_CONN:
 	return D_lecInterfaceState_busConnect;
    case LEC_ST_MCAST_SEND_CONN:
 	return D_lecInterfaceState_busConnect;
    case LEC_ST_MCAST_FORWARD_CONN:
 	return D_lecInterfaceState_busConnect;
    case LEC_ST_ACTIVE:
 	return D_lecInterfaceState_operational;
    case LEC_ST_TERMINATING:
	return D_lecInterfaceState_initialState;
    }

    /*
     * This isn't reached but is required by the compiler as the switch
     * doesn't have a default
     */
    return(0);
}   /* END convert_lecInfoState */


/*---------------------------------------------------------------------
 * convert_laneLanType
 *
 * convert the lane LAN type into the appropriate MIB value
 * INPUTS:
 *    laneLanType == internal LAN type defined in LANE PDUs
 * RETURNS:
 *    LEC MIB LAN type
 *---------------------------------------------------------------------*/
static inline long convert_laneLanType (uchar laneLanType)
{
    switch (laneLanType) {
    case LANE_TYPE_UNSPECIFIED:
	return D_lecConfigLanType_unspecified;
    case LANE_TYPE_802_3:
	return D_lecConfigLanType_aflane8023;
    case LANE_TYPE_802_5:
 	return D_lecConfigLanType_aflane8025;
    default:   /* should not happen!! */
	return D_lecConfigLanType_unspecified;
    }
    /*NOTREACHED*/
}   /* END convert_laneLanType */


/*---------------------------------------------------------------------
 * convert_laneMaxFrameSize
 *
 * convert the lane max frame size into the appropriate MIB value
 * INPUTS:
 *    laneMaxFrameSize == internal LAN max frame size defined in LANE PDUs
 * RETURNS:
 *    LEC MIB max frame size
 *---------------------------------------------------------------------*/
static inline long convert_laneMaxFrameSize (uchar laneMaxFrameSize)
{
    switch (laneMaxFrameSize) {
    case LANE_MAX_FRAME_UNSPECIFIED:
	return D_lecConfigMaxDataFrameSize_unspecified;
    case LANE_MAX_FRAME_1516:
	return D_lecConfigMaxDataFrameSize_max1516;
    case LANE_MAX_FRAME_4544:
	return D_lecConfigMaxDataFrameSize_max4544;
    case LANE_MAX_FRAME_9234:
	return D_lecConfigMaxDataFrameSize_max9234;
    case LANE_MAX_FRAME_18190:
	return D_lecConfigMaxDataFrameSize_max18190;
    default:   /* should not happen!! */
	return D_lecConfigMaxDataFrameSize_unspecified;
    }
    /*NOTREACHED*/
}   /* END convert_laneMaxFrameSize */


/*---------------------------------------------------------------------
 * convert_leArpEntryType
 *
 * convert the internal LE-ARP entry status to the lane LE-ARP entry
 * status
 *
 * INPUTS:
 *    learpIntStatus  == internal LE-ARP status
 * RETURNS:
 *    LEC MIB leArpEntryType
 *---------------------------------------------------------------------*/
static inline long convert_leArpEntryType (lec_arp_entry_type_t  entry_type)
{
    /* 
     * we do not support staticNonVolatile(5) 
     */
    switch (entry_type)   {
    case LEC_LE_ARP_PERMANENT:
	return D_leArpEntryType_staticVolatile;
    case LEC_LE_ARP_LOCAL:
    case LEC_LE_ARP_REMOTE:
	return D_leArpEntryType_learnedViaControl;
    }

    /*
     * This isn't reached but is required by the compiler as the switch
     * doesn't have a default
     */
    return(0);
}   /* END convert_leArpEntryType */


/*---------------------------------------------------------------------
 * convert_lecFailRespCode
 *
 * convert the internal failure response code to the LEC MIB encoding
 *
 * INPUTS:
 *    failCode  == internal error code
 * RETURNS:
 *    LEC MIB failure code as defined in the lecLastFailureRespCode 
 *    object syntax
 * NOTES:
 * The D_lecLastFailureRespCode_timeout return value is not
 * supported by the LANE spec error codes...it is added internally
 * at the end of the LANE_STATUS #define list
 *---------------------------------------------------------------------*/
static inline long convert_lecFailRespCode (ushort status)
{
    switch (status)  {
    case LANE_STATUS_SUCCESS:
        return D_lecLastFailureRespCode_none;
    case LANE_STATUS_VERSION_NOT_SUP:
	return D_lecLastFailureRespCode_versionNotSupported;
    case LANE_STATUS_INVALID_REQ_PARMS:
	return D_lecLastFailureRespCode_invalidRequestParameters;
    case LANE_STATUS_DUP_LAN_DEST:
        return D_lecLastFailureRespCode_duplicateLanDestination;
    case LANE_STATUS_DUP_ATM_ADDR:
        return D_lecLastFailureRespCode_duplicateAtmAddress;
    case LANE_STATUS_INSUFF_RESOURCE:
        return D_lecLastFailureRespCode_insufficientResources;
    case LANE_STATUS_ACCESS_DENIED:
        return D_lecLastFailureRespCode_accessDenied;
    case LANE_STATUS_INVALID_REQ_LECID:
        return D_lecLastFailureRespCode_invalidRequesterId;
    case LANE_STATUS_INVALID_LAN_DEST:
        return D_lecLastFailureRespCode_invalidLanDestination;
    case LANE_STATUS_INVALID_ATM_ADDR:
        return D_lecLastFailureRespCode_invalidAtmAddress;
    case LANE_STATUS_NO_CONFIG:
	return D_lecLastFailureRespCode_noConfiguration;
    case LANE_STATUS_LE_CONFIG_ERROR:
        return D_lecLastFailureRespCode_leConfigureError;
    case LANE_STATUS_INSUFF_INFO:
        return D_lecLastFailureRespCode_insufficientResources;
    case LANE_STATUS_TIMEOUT:
	return D_lecLastFailureRespCode_timeout;
    default:
        return D_lecLastFailureRespCode_undefinedError;
    }
    /*NOTREACHED*/
    /* not supported D_lecLastFailureRespCode_insufficientInformation */
    
}   /* END convert_lecFailRespCode */

/*---------------------------------------------------------------------
 * k_lecConfigEntry_get
 *
 * SNMPR get function for lecConfigTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to struct with filled in values if get succeeded
 *---------------------------------------------------------------------*/
lecConfigEntry_t *k_lecConfigEntry_get (int serialNum, ContextInfo
					*contextInfo, int nominator,
					int searchType, long lecIndex
					) {

    static lecConfigEntry_t   lecConfigEntryData;
    static lecmib_cache_t     lec_cache = NULL_CACHE;
    static OctetString        lec_owner;
    static OctetString        lec_config_name;
    static OctetString        les_config_addr;
    lec_info_t                *lec_info;
    lec_snmp_info_t           *lec_snmp;
    boolean                   use_cache;

    /* find the proper lec_info entry and check if the cached entry is
       valid */
    lec_info = find_lecInfo(searchType, serialNum, lecIndex,
			    &lec_cache, &use_cache);
    if (use_cache)
	return &lecConfigEntryData;
    else if (lec_info==NULL_LEC)
	return NULL;
    
    lec_snmp = lec_info->lec_snmp;
    
    /* lecIndex */
    lecConfigEntryData.lecIndex = get_lec_index(lec_info);
    
    /* ??? lecRowStatus--hard-wire to active(1) for now */
    lecConfigEntryData.lecRowStatus = D_lecRowStatus_active;   
    
    /* lecOwner */
    lec_owner.octet_ptr = lec_snmp->lec_owner;
    lec_owner.length = strlen(lec_owner.octet_ptr);
    lecConfigEntryData.lecOwner = &lec_owner;
    
    /* lecConfigMode */
    if (lec_snmp->lec_config_mode_auto)
	lecConfigEntryData.lecConfigMode = D_lecConfigMode_automatic;
    else
	lecConfigEntryData.lecConfigMode = D_lecConfigMode_manual;
    
    /* lecConfigLanType */
    lecConfigEntryData.lecConfigLanType = 
	convert_laneLanType(lec_info->lane_info->type);
    
    /* lecConfigMaxDataFrameSize */
    lecConfigEntryData.lecConfigMaxDataFrameSize =
	convert_laneMaxFrameSize(lec_info->lane_info->max_data_frame);
    
    /* lecConfigLanName */
    lec_config_name.octet_ptr = lec_info->lane_info->name;
    lec_config_name.length = lec_info->lane_info->name_size;
    lecConfigEntryData.lecConfigLanName = &lec_config_name;
    
    /* lecConfigLesAtmAddress */   
    if (verify_nsap_hwaddr(&lec_info->ctl_direct.nsap))  {
	les_config_addr.octet_ptr = lec_info->ctl_direct.nsap.addr;
	les_config_addr.length = STATIONLEN_ATMNSAP;
    } else {
	les_config_addr.octet_ptr = "";
	les_config_addr.length = 0;
    }
    lecConfigEntryData.lecConfigLesAtmAddress = &les_config_addr;
    
    /* lecControlTimeout */
    lecConfigEntryData.lecControlTimeout =
	(long)(lec_info->C7_control_timeout/ONESEC);
    
    /* lecMaxUnknownFrameCount */
    lecConfigEntryData.lecMaxUnknownFrameCount =
	(long)lec_info->C10_max_unknown_count;
    
    /* lecMaxUnknownFrameSize */
    lecConfigEntryData.lecMaxUnknownFrameTime =
	(long)(lec_info->C11_max_unknown_time/ONESEC);
    
    /* lecVccTimeoutPeriod */

    if (lec_info->swidb && 
	lec_info->swidb->hwptr &&
	lec_info->swidb->hwptr->atm_db) {
	lecConfigEntryData.lecVccTimeoutPeriod =
	    (long)(lec_info->swidb->hwptr->atm_db->idle_time)/ONESEC;
    } 
    
    /* lecMaxRetryCount */
    lecConfigEntryData.lecMaxRetryCount =
	(long)lec_info->C13_max_retry_count;
    
    /* lecAgingTime */
    lecConfigEntryData.lecAgingTime = 
	(long)(lec_info->C17_aging_time/ONESEC);
    
    /* lecForwardDelayTime */
    lecConfigEntryData.lecForwardDelayTime =
	(long)(lec_info->C18_forward_delay_time/ONESEC);
    
    /* lecExpectedArpResponseTime */
    lecConfigEntryData.lecExpectedArpResponseTime =
	(long)(lec_info->C20_arp_response_time/ONESEC);
    
    /* lecFlushTimeOut */
    lecConfigEntryData.lecFlushTimeOut = 
	(long)(lec_info->C21_flush_time/ONESEC);
    
    /* lecPathSwitchingDelay */
    lecConfigEntryData.lecPathSwitchingDelay =
	(long)(lec_info->C22_path_switching_delay/ONESEC);
    
    /* lecLocalSegmentId -- hard-wired until token-ring support is
       added */
    if (lec_info->lane_info->type==LANE_TYPE_802_5)
	lecConfigEntryData.lecLocalSegmentID = 0; /* until TR support */
    else
	lecConfigEntryData.lecLocalSegmentID = 0;
    
    /* lecMulticastSendType -- we do not allow this to be changed */
    lecConfigEntryData.lecMulticastSendType =
	D_lecMulticastSendType_bestEffort;
    
    /* lecMulticastSendAvgRate */
    lecConfigEntryData.lecMulticastSendAvgRate =
	lec_info->C25_mcast_ave_rate;
    
    /* lecMulticastSendPeakRate */
    lecConfigEntryData.lecMulticastSendPeakRate =
	lec_info->C26_mcast_peak_rate;
    
    /* lecConnectionCompleteTimer */
    lecConfigEntryData.lecConnectionCompleteTimer =
	(long)(lec_info->C28_connect_comp_time/ONESEC);
    
    return(&lecConfigEntryData);

}  /* k_lecConfigEntry_get */


#ifdef SETS
/*---------------------------------------------------------------------
 * k_lecConfigEntry_test
 *
 * SNMPR test function for lecConfigTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    test phase status code
 *---------------------------------------------------------------------*/
int k_lecConfigEntry_test (
			   ObjectInfo     *object,
			   ObjectSyntax   *value,
			   doList_t       *dp,
			   ContextInfo    *contextInfo)
{
    return NO_CREATION_ERROR;
}

int
k_lecConfigEntry_ready (
			ObjectInfo     *object,
			ObjectSyntax   *value,
			doList_t       *doHead,
			doList_t       *dp)
{
    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_lecConfigEntry_set_defaults (doList_t       *dp)
{
    /*    lecConfigEntry_t *data = (lecConfigEntry_t *) (dp->data); */
    return NO_ERROR;
}

int
k_lecConfigEntry_set (
		      lecConfigEntry_t *data,
		      ContextInfo *contextInfo,
		      int function)
{
    return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_lecConfigEntry_UNDO
/* add #define SR_lecConfigEntry_UNDO in lecmib.h to
 * include the undo routine for the lecConfigEntry family.
 */
int
lecConfigEntry_undo (
		     doList_t       *doHead,
		     doList_t       *doCur,
		     ContextInfo    *contextInfo)
{
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecConfigEntry_UNDO */
#endif /* SR_SNMPv2 */

/*---------------------------------------------------------------------
 * k_lecStatusEntry_get
 *
 * SNMPR get function for lecStatusTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to filled in struct if get succeeded
 *---------------------------------------------------------------------*/
lecStatusEntry_t *k_lecStatusEntry_get (
					int serialNum,
					ContextInfo *contextInfo,
					int nominator,
					int searchType,
					long lecIndex)
{
    static lecStatusEntry_t   lecStatusEntryData;
    static lecmib_cache_t     lec_status_cache = NULL_CACHE;
    static OctetString        lecs_addr;
    static OctetString        lec_addr;
    static OctetString        lec_actual_name;
    static OctetString        les_actual_addr;
    lec_info_t                *lec_info;
    boolean                   use_cache;

    /* find the proper lec_info entry and check if the cached entry is
       valid */
    lec_info = find_lecInfo(searchType, serialNum, lecIndex,
			    &lec_status_cache, &use_cache);
    if (use_cache)
	return &lecStatusEntryData;
    else if (lec_info==NULL_LEC)
	return NULL;
    
    /* lecIndex */
    lecStatusEntryData.lecIndex = get_lec_index(lec_info);
    
    /* lecIfIndex */
    lecStatusEntryData.lecIfIndex = get_lec_ifIndex(lec_info);
    
    /* lecPrimaryAtmAddress */
    if (verify_nsap_hwaddr(&lec_info->nsap))  {
	lec_addr.octet_ptr = lec_info->nsap.addr;
	lec_addr.length = STATIONLEN_ATMNSAP;
    } else  {
	lec_addr.octet_ptr = "";
	lec_addr.length = 0;
    }
    lecStatusEntryData.lecPrimaryAtmAddress = &lec_addr;
    
    /* lecID */
    lecStatusEntryData.lecID = (long)lec_info->lecid;
    
    /* lecInterfaceState--return the LANE state enumeration, not the
       internal enumeration */
    lecStatusEntryData.lecInterfaceState = 
	convert_lecInfoState(lec_info->state);
    
    /* lecLastFailureRespCode */
    lecStatusEntryData.lecLastFailureRespCode =
	convert_lecFailRespCode(lec_info->lec_snmp->last_fail_resp_code);
    
    /* lecLastFailureState */
    lecStatusEntryData.lecLastFailureState =
	convert_lecInfoState(lec_info->lec_snmp->last_fail_state);
    
    /* lecProtocol */
    lecStatusEntryData.lecProtocol = LANE_PROTOCOL_ID;
    
    /* lecVersion--hard-wired all over code--this will have to change
       if multiple LANE versions are supported concurrently */
    lecStatusEntryData.lecVersion = LANE_PROTOCOL_VERSION;
    
    /* lecTopologyChange */
    lecStatusEntryData.lecTopologyChange =
	(lec_info->C19_topology_change) ? D_lecTopologyChange_true :
	    D_lecTopologyChange_false;
    
    lecs_addr.octet_ptr = lec_info->configure.nsap.addr;
    lecs_addr.length = lec_info->configure.nsap.length;

    lecStatusEntryData.lecConfigServerAtmAddress = &lecs_addr;
    
    /* lecConfigSource */
    lecStatusEntryData.lecConfigSource = lec_info->lec_snmp->lec_config_source;
    
    /* lecActualLanType */
    lecStatusEntryData.lecActualLanType = 
	convert_laneLanType(lec_info->actual_lan_type);
    
    /* lecActualMaxDataFrameSize */
    lecStatusEntryData.lecActualMaxDataFrameSize = 
	convert_laneMaxFrameSize(lec_info->actual_max_frame_size);
    
    /* lecActualLanName */
    lec_actual_name.octet_ptr = lec_info->actual_elan_name;
    lec_actual_name.length = lec_info->actual_name_size;
    lecStatusEntryData.lecActualLanName = &lec_actual_name;
    
    /* lecActualLesAtmAddress */
    if (verify_nsap_hwaddr(&lec_info->ctl_direct.nsap))  {
	les_actual_addr.octet_ptr = lec_info->ctl_direct.nsap.addr;
	les_actual_addr.length = STATIONLEN_ATMNSAP;
    } else {
	les_actual_addr.octet_ptr = "";
	les_actual_addr.length = 0;
    }
    lecStatusEntryData.lecActualLesAtmAddress = &les_actual_addr;
    
    /* lecProxyClient */
    lecStatusEntryData.lecProxyClient = (lec_info->C4_proxy) ?
	D_lecProxyClient_true : D_lecProxyClient_false;
    
    return(&lecStatusEntryData);

}  /* k_lecStatusEntry_get */


/*---------------------------------------------------------------------
 * k_lecMappingEntry_get
 *
 * SNMPR get function for lecMappingTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to filled in struct if get succeeded
 *---------------------------------------------------------------------*/
lecMappingEntry_t *k_lecMappingEntry_get (
					  int serialNum,
					  ContextInfo *contextInfo,
					  int nominator,
					  int searchType,
					  long ifIndex)
{
    static lecMappingEntry_t lecMappingEntryData;
    lec_info_t		    *lec_info;
    
    /* find the proper lec_info entry and get the ifIndex and lecIndex */
    lec_info = find_lecInfo_by_ifIndex(searchType, ifIndex);
    if (lec_info==NULL_LEC)
	return NULL;
    
    lecMappingEntryData.lecMappingIndex = get_lec_index(lec_info);
    lecMappingEntryData.ifIndex = get_lec_ifIndex(lec_info);
    return(&lecMappingEntryData);
    
} /* k_lecMappingEntry_get */


/*---------------------------------------------------------------------
 * k_lecStatisticsEntry_get
 *
 * SNMPR get function for lecStatisticsTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to filled in struct if get succeeded
 *---------------------------------------------------------------------*/
lecStatisticsEntry_t *k_lecStatisticsEntry_get (
						int serialNum,
						ContextInfo *contextInfo,
						int nominator,
						int searchType,
						long lecIndex)
{
    static lecStatisticsEntry_t lecStatisticsEntryData;
    static lecmib_cache_t     stat_cache = NULL_CACHE;
    lec_info_t                *lec_info;
    boolean                   use_cache;

    /* find the proper lec_info entry and check if the cached entry is
       valid */
    lec_info = find_lecInfo(searchType, serialNum, lecIndex,
			    &stat_cache, &use_cache);
    if (use_cache)
	return &lecStatisticsEntryData;
    else if (lec_info==NULL_LEC)
	return NULL;
    
    /* lecArpRequestsOut */
    lecStatisticsEntryData.lecArpRequestsOut =
	lec_info->lecARPrequestsOut;
    
    /* lecArpRequestsIn */
    lecStatisticsEntryData.lecArpRequestsIn =
	lec_info->lecARPrequestsIn;
    
   /* lecArpRepliesOut */

    lecStatisticsEntryData.lecArpRepliesOut = lec_info->lecARPrepliesOut;
    
    /* lecArpRepliesIn */
    lecStatisticsEntryData.lecArpRepliesIn = lec_info->lecARPrepliesIn;
    
    /* lecControlFramesOut */
    lecStatisticsEntryData.lecControlFramesOut =
	lec_info->lecControlFramesOut;
    
    /* lecControlFramesIn */
    lecStatisticsEntryData.lecControlFramesIn =
	lec_info->lecControlFramesIn;
    
    /* lecSvcFailures */
    lecStatisticsEntryData.lecSvcFailures = lec_info->lecSVCFailures;
    
    /* lecIndex */
    lecStatisticsEntryData.lecIndex = get_lec_index(lec_info);
    
    return(&lecStatisticsEntryData);
    
}   /* k_lecStatisticsEntry_get */ 



/*---------------------------------------------------------------------
 * k_lecServerVccEntry_get
 *
 * SNMPR get function for lecServerVccTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to filled in struct if get succeeded
 *---------------------------------------------------------------------*/
lecServerVccEntry_t *k_lecServerVccEntry_get (
					      int serialNum,
					      ContextInfo *contextInfo,
					      int nominator,
					      int searchType,
					      long lecIndex)
{
    static lecServerVccEntry_t lecServerVccEntryData;
    static lecmib_cache_t     vcc_cache = NULL_CACHE;
    lec_info_t                *lec_info;
    boolean                   use_cache;
    long                      vc_interface;
    long                      vc_vpi;
    long                      vc_vci;

    /* find the proper lec_info entry and check if the cached entry is
       valid */
    lec_info = find_lecInfo(searchType, serialNum, lecIndex,
			    &vcc_cache, &use_cache);
    if (use_cache)
	return &lecServerVccEntryData;
    else if (lec_info==NULL_LEC)
	return NULL;
    
    /* lecConfigDirect [Interface, Vpi, Vci] */
    lecServerVccEntryData.lecConfigDirectInterface = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecConfigDirectVpi = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecConfigDirectVci = LEC_VCC_INFO_INVALID;
    if (lec_info->configure.call_state == CALL_STATE_UP)  {
	if (lec_info->configure.swidb->hwptr &&
	    atm_getvc_mib_id(lec_info->configure.vcd, 
			     lec_info->configure.swidb->hwptr,
			     &vc_interface, &vc_vpi, &vc_vci))  {
	    lecServerVccEntryData.lecConfigDirectInterface =
		vc_interface;
	    lecServerVccEntryData.lecConfigDirectVpi = vc_vpi;
	    lecServerVccEntryData.lecConfigDirectVci = vc_vci;
	}
    }
    
    /* lecControlDirect [Interface, Vpi, Vci] */
    lecServerVccEntryData.lecControlDirectInterface = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecControlDirectVpi = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecControlDirectVci = LEC_VCC_INFO_INVALID;
    if (lec_info->ctl_direct.call_state == CALL_STATE_UP)  {
	if (lec_info->ctl_direct.swidb->hwptr &&
	    atm_getvc_mib_id(lec_info->ctl_direct.vcd,  
			     lec_info->ctl_direct.swidb->hwptr,
			     &vc_interface, &vc_vpi, &vc_vci))  {
	    lecServerVccEntryData.lecControlDirectInterface =
		vc_interface;
	    lecServerVccEntryData.lecControlDirectVpi = vc_vpi;
	    lecServerVccEntryData.lecControlDirectVci = vc_vci;
	}
    }
    
    /* lecControlDistribute [Interface, Vpi, Vci] */
    lecServerVccEntryData.lecControlDistributeInterface = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecControlDistributeVpi = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecControlDistributeVci = LEC_VCC_INFO_INVALID;
    if (lec_info->ctl_distribute.call_state == CALL_STATE_UP)  {
	if (lec_info->ctl_distribute.swidb->hwptr &&
	    atm_getvc_mib_id(lec_info->ctl_distribute.vcd,  
			     lec_info->ctl_distribute.swidb->hwptr,
			     &vc_interface, &vc_vpi, &vc_vci))  {
	    lecServerVccEntryData.lecControlDistributeInterface =
		vc_interface;
	    lecServerVccEntryData.lecControlDistributeVpi = vc_vpi;
	    lecServerVccEntryData.lecControlDistributeVci = vc_vci;
       }
    }
    
    /* lecMulticastSend [Interface, Vpi, Vci] */
    lecServerVccEntryData.lecMulticastSendInterface = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecMulticastSendVpi = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecMulticastSendVci = LEC_VCC_INFO_INVALID;
    if (lec_info->mcast_send.call_state == CALL_STATE_UP)  {
	if (lec_info->mcast_send.swidb->hwptr &&
	    atm_getvc_mib_id(lec_info->mcast_send.vcd,  
			     lec_info->mcast_send.swidb->hwptr,
			     &vc_interface, &vc_vpi, &vc_vci))  {
	    lecServerVccEntryData.lecMulticastSendInterface =
		vc_interface;
	    lecServerVccEntryData.lecMulticastSendVpi = vc_vpi;
	    lecServerVccEntryData.lecMulticastSendVci = vc_vci;
	}
    }
    
    /* lecMulticastForward [Interface, Vpi, Vci] */
    lecServerVccEntryData.lecMulticastForwardInterface = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecMulticastForwardVpi = LEC_VCC_INFO_INVALID;
    lecServerVccEntryData.lecMulticastForwardVci = LEC_VCC_INFO_INVALID;
    if (lec_info->mcast_forward.call_state == CALL_STATE_UP)  {
	if (lec_info->mcast_forward.swidb->hwptr &&
	    atm_getvc_mib_id(lec_info->mcast_forward.vcd,  
			     lec_info->mcast_forward.swidb->hwptr,
			     &vc_interface, &vc_vpi, &vc_vci))  {
	    lecServerVccEntryData.lecMulticastForwardInterface =
		vc_interface;
	    lecServerVccEntryData.lecMulticastForwardVpi = vc_vpi;
	    lecServerVccEntryData.lecMulticastForwardVci = vc_vci;
	}
    }
    
    /* lecIndex */
    lecServerVccEntryData.lecIndex = get_lec_index(lec_info);
    return(&lecServerVccEntryData);
}  /* k_lecServerVccEntry_get */


/*---------------------------------------------------------------------
 * k_lecAtmAddressEntry_get
 *
 * SNMPR get function for lecAtmAddressTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to filled in struct if get succeeded
 *---------------------------------------------------------------------*/
lecAtmAddressEntry_t *k_lecAtmAddressEntry_get (
						int serialNum,
						ContextInfo *contextInfo,
						int nominator,
						int searchType,
						long lecIndex,
						OctetString * lecAtmAddress)
{
    static lecAtmAddressEntry_t lecAtmAddressEntryData;
    static lecmib_cache_t     atm_cache = NULL_CACHE;
    static OctetString        atm_addr;
    lec_info_t                *lec_info;
    boolean                   use_cache;
    
    /* find the proper lec_info entry */
    lec_info = find_lecInfo_by_atm_addr(searchType, 
					serialNum,
					lecIndex,
					lecAtmAddress,
					&atm_cache,
					&use_cache);
    if (use_cache)
	return &lecAtmAddressEntryData;
    
    if (lec_info==NULL_LEC)
	return NULL;    /* no entry found */
    
    /* lecAtmAddress -- returned nsap already checked as valid */
    atm_addr.octet_ptr = lec_info->nsap.addr;
    atm_addr.length = STATIONLEN_ATMNSAP;
    lecAtmAddressEntryData.lecAtmAddress = &atm_addr;
    
    /* lecAtmAddressStatus */
    lecAtmAddressEntryData.lecAtmAddressStatus =
	D_lecAtmAddressStatus_active;
    
    /* lecIndex */
    lecAtmAddressEntryData.lecIndex = get_lec_index(lec_info);
    
    return(&lecAtmAddressEntryData);
    
}  /* k_lecAtmAddressEntry_get */


#ifdef SETS
int
k_lecAtmAddressEntry_test (
			   ObjectInfo     *object,
			   ObjectSyntax   *value,
			   doList_t       *dp,
			   ContextInfo    *contextInfo)
{
    return GEN_ERROR;
}


int
k_lecAtmAddressEntry_ready (
			    ObjectInfo     *object,
			    ObjectSyntax   *value,
			    doList_t       *doHead,
			    doList_t       *dp)
{
    dp->state = ADD_MODIFY;
    return NO_ERROR;
}


int
k_lecAtmAddressEntry_set_defaults (doList_t       *dp)
{
    /* lecAtmAddressEntry_t *data = (lecAtmAddressEntry_t *) (dp->data); */
    return NO_ERROR;
}


int
k_lecAtmAddressEntry_set (
			  lecAtmAddressEntry_t *data,
			  ContextInfo *contextInfo,
			  int function)
{
    return GEN_ERROR;
}
#endif /* SETS */


#ifdef SR_SNMPv2
#ifdef SR_lecAtmAddressEntry_UNDO
/* add #define SR_lecAtmAddressEntry_UNDO in lecmib.h to
 * include the undo routine for the lecAtmAddressEntry family.
 */
int
lecAtmAddressEntry_undo ( 
			 doList_t       *doHead,
			 doList_t       *doCur,
			 ContextInfo    *contextInfo)
{
    return UNDO_FAILED_ERROR;
}
#endif /* SR_lecAtmAddressEntry_UNDO */
#endif /* SR_SNMPv2 */


/*---------------------------------------------------------------------
 * k_lecMacAddressEntry_get
 *
 * SNMPR get function for lecMacAddressTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to filled in struct if get succeeded
 *---------------------------------------------------------------------*/
lecMacAddressEntry_t *k_lecMacAddressEntry_get (
						int serialNum,
						ContextInfo *contextInfo,
						int nominator,
						int searchType,
						long lecIndex,
						OctetString * lecMacAddress)
{
    static lecMacAddressEntry_t lecMacAddressEntryData;
    static lecmib_cache_t     mac_cache = NULL_CACHE;
    static OctetString        atm_addr;
    static OctetString        mac_addr;
    lec_info_t                *lec_info;
    boolean                   use_cache;

    /* find the proper lec_info entry */
    lec_info = find_lecInfo_by_mac_addr(searchType, 
					serialNum,
					lecIndex,
					lecMacAddress,
					&mac_cache,
					&use_cache);
    if (use_cache)
	return &lecMacAddressEntryData;
    if (lec_info==NULL_LEC)
	return NULL;    /* no entry found */
    
    /* lecMacAddress */
    mac_addr.octet_ptr = lec_info->normalMac;
    mac_addr.length = STATIONLEN_IEEE48;
    lecMacAddressEntryData.lecMacAddress = &mac_addr;
    
    /* lecMacAddressAtmBinding */
    atm_addr.octet_ptr = lec_info->nsap.addr;
    atm_addr.length = STATIONLEN_ATMNSAP;
    lecMacAddressEntryData.lecMacAddressAtmBinding = &atm_addr;
    
    /* lecIndex */
    lecMacAddressEntryData.lecIndex = get_lec_index(lec_info);
    
    return(&lecMacAddressEntryData);
}  /* k_lecMacAddressEntry_get */


lecRouteDescrEntry_t *
k_lecRouteDescrEntry_get (
			  int serialNum,
			  ContextInfo *contextInfo,
			  int nominator,
			  int searchType,
			  long lecIndex,
			  long lecRouteDescrSegmentID,
			  long lecRouteDescrBridgeNumber)
{
    /* implement this when token ring is added */
#ifdef NOT_YET
    static lecRouteDescrEntry_t lecRouteDescrEntryData;
    
    /*
     * put your code to retrieve the information here
     */
    
    lecRouteDescrEntryData.lecRouteDescrSegmentID = ;
    lecRouteDescrEntryData.lecRouteDescrBridgeNumber = ;
    lecRouteDescrEntryData.lecRouteDescrAtmBinding = ;
    lecRouteDescrEntryData.lecIndex = ;
    return(&lecRouteDescrEntryData);
#else /* NOT_YET */
    return(NULL);
#endif /* NOT_YET */
}


/*---------------------------------------------------------------------
 * k_leArpEntry_get
 *
 * SNMPR get function for leArpTable
 * INPUTS:
 *    see SNMPR dox
 * RETURNS:
 *    ptr to filled in struct if get succeeded
 *---------------------------------------------------------------------*/
leArpEntry_t *k_leArpEntry_get ( 
				int serialNum,
				ContextInfo *contextInfo,
				int nominator,
				int searchType,
				long lecIndex,
				OctetString * leArpMacAddress)
{
    static leArpEntry_t       leArpEntryData;
    static lecmib_cache_t     learp_cache = NULL_CACHE;
    static OctetString        arp_mac_addr;
    static OctetString        arp_atm_addr;
    lec_info_t                *lec_info;
    lec_le_arp_t              *learp_info;
    boolean                   use_cache;

    /* find the proper lec_info entry */
    lec_info = find_lecInfo_by_learp_addr(searchType, 
					  serialNum,
					  lecIndex,
					  leArpMacAddress,
					  &learp_cache,
					  &use_cache,
					  &learp_info);
    if (use_cache)
	return &leArpEntryData;
    if (lec_info==NULL_LEC)
	return NULL;    /* no entry found */
    
    /* leArpMacAddress */
    arp_mac_addr.octet_ptr = learp_info->mac;
    arp_mac_addr.length = STATIONLEN_IEEE48;
    leArpEntryData.leArpMacAddress = &arp_mac_addr;
    
    /* leArpAtmAddress */
    arp_atm_addr.octet_ptr = learp_info->nsap.addr;
    arp_atm_addr.length = snpa_get_length(&learp_info->nsap);
    leArpEntryData.leArpAtmAddress = &arp_atm_addr;
    
    /* leArpIsRemoteAddress */
    leArpEntryData.leArpIsRemoteAddress =
	(learp_info->remote_addr_flag_set) ?
	    D_leArpIsRemoteAddress_true : D_leArpIsRemoteAddress_false;
    
    /* leArpEntryType */
    leArpEntryData.leArpEntryType =
	convert_leArpEntryType(learp_info->entry_type);
    
    /* leArpRowStatus */
    leArpEntryData.leArpRowStatus = D_leArpRowStatus_active;
    
    /* lecIndex */
    leArpEntryData.lecIndex = get_lec_index(lec_info);
    
    return(&leArpEntryData);
}  /* k_leArpEntry_get */


#ifdef SETS
int
k_leArpEntry_test (
		   ObjectInfo     *object,
		   ObjectSyntax   *value,
		   doList_t       *dp,
		   ContextInfo    *contextInfo)
{
    return GEN_ERROR;
}

int
k_leArpEntry_ready (
		    ObjectInfo     *object,
		    ObjectSyntax   *value,
		    doList_t       *doHead,
		    doList_t       *dp)
{
    dp->state = ADD_MODIFY;
    return NO_ERROR;
}


int
k_leArpEntry_set_defaults (doList_t       *dp)
{
    /* leArpEntry_t *data = (leArpEntry_t *) (dp->data); */
    
    return NO_ERROR;
}

int
k_leArpEntry_set (
		  leArpEntry_t *data,
		  ContextInfo *contextInfo,
		  int function)
{
    return GEN_ERROR;
}
#endif /* SETS */


#ifdef SR_SNMPv2
#ifdef SR_leArpEntry_UNDO
/* add #define SR_leArpEntry_UNDO in lecmib.h to
 * include the undo routine for the leArpEntry family.
 */
int
leArpEntry_undo (
		 doList_t       *doHead,
		 doList_t       *doCur,
		 ContextInfo    *contextInfo)
{
    return UNDO_FAILED_ERROR;
}
#endif /* SR_leArpEntry_UNDO */
#endif /* SR_SNMPv2 */


leRDArpEntry_t *
k_leRDArpEntry_get (
		    int serialNum,
		    ContextInfo *contextInfo,
		    int nominator,
		    int searchType,
		    long lecIndex,
		    long leRDArpSegmentID,
		    long leRDArpBridgeNumber)
{
    /* implement when token ring is added */	
#ifdef NOT_YET
    static leRDArpEntry_t leRDArpEntryData;
    
    /*
     * put your code to retrieve the information here
     */
    
    leRDArpEntryData.leRDArpSegmentID = ;
    leRDArpEntryData.leRDArpBridgeNumber = ;
    leRDArpEntryData.leRDArpAtmAddress = ;
    leRDArpEntryData.leRDArpEntryType = ;
    leRDArpEntryData.leRDArpRowStatus = ;
    leRDArpEntryData.lecIndex = ;
    return(&leRDArpEntryData);
#else /* NOT_YET */
    return(NULL);
#endif /* NOT_YET */
}


#ifdef SETS
int
k_leRDArpEntry_test (
		     ObjectInfo     *object,
		     ObjectSyntax   *value,
		     doList_t       *dp,
		     ContextInfo    *contextInfo)
{
    return GEN_ERROR;
}


int
k_leRDArpEntry_ready ( 
		      ObjectInfo     *object,
		      ObjectSyntax   *value,
		      doList_t       *doHead,
		      doList_t       *dp)
{
    dp->state = ADD_MODIFY;
    return NO_ERROR;
}


int
k_leRDArpEntry_set_defaults (doList_t       *dp)
{
    /* leRDArpEntry_t *data = (leRDArpEntry_t *) (dp->data); */
    return NO_ERROR;
}


int
k_leRDArpEntry_set (
		    leRDArpEntry_t *data,
		    ContextInfo *contextInfo,
		    int function)
{
    return GEN_ERROR;
}
#endif /* SETS */


#ifdef SR_SNMPv2
#ifdef SR_leRDArpEntry_UNDO
/* add #define SR_leRDArpEntry_UNDO in lecmib.h to
 * include the undo routine for the leRDArpEntry family.
 */
int
leRDArpEntry_undo (
		   doList_t       *doHead,
		   doList_t       *doCur,
		   ContextInfo    *contextInfo)
{
    return UNDO_FAILED_ERROR;
}
#endif /* SR_leRDArpEntry_UNDO */
#endif /* SR_SNMPv2 */


/*ARGSUSED*/
static void init_lecmib (subsystype *subsys)
{
    load_mib(lecmib_OidList, lecmib_OidListNum);
    load_oid(lecmib_oid_table);
    next_snmp_lec_index = 1;            /* TEMP SNMP index handling */
    
    reg_add_lecmib_register_lec_info(lecmib_register_lec_info, 
				     "lecmib_register_lec_info");
    reg_add_lecmib_complete_lec_info(lecmib_complete_lec_info,
				     "lecmib_complete_lec_info");
    reg_add_lecmib_deregister_lec_info(lecmib_deregister_lec_info,
				       "lecmib_deregister_lec_info");
    reg_add_lecmib_deregister_rcv_addr(lecmib_deregister_rcv_addr,
				       "lecmib_deregister_rcv_addr");
    reg_add_lecmib_count_deleted_SVC(lecmib_count_deleted_SVC,
				     "lecmib_count_deleted_SVC");
    
    lecmib_if_init();
}


/*
 * lecmib subsystem header
 */

#define MAJVERSION_lecmib 1
#define MINVERSION_lecmib 0
#define EDITVERSION_lecmib 0

SUBSYS_HEADER(lecmib,
              MAJVERSION_lecmib,
              MINVERSION_lecmib,
              EDITVERSION_lecmib,
              init_lecmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: lane",
              NULL);

      
