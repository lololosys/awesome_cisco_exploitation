/* $Id: sr_modem_mgmtmib.c,v 3.1.42.4 1996/07/11 01:11:46 styang Exp $
 * $Source: /release/112/cvs/Xsys/les/sr_modem_mgmtmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1996, Viet Nguyen (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_modem_mgmtmib.c,v $
 * Revision 3.1.42.4  1996/07/11  01:11:46  styang
 * CSCdi62212:  code review clean up
 * Branch: California_branch
 * Code review clean up.
 *
 * Revision 3.1.42.3  1996/07/01  18:45:39  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.42.2  1996/06/22  23:47:10  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.1.42.1  1996/06/16  21:16:50  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1996/01/25  00:47:53  vnguyen
 * placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1992-1995 by SNMP Research, Incorporated.
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
#include "config.h"
#include "logger.h"
#include "ttysrv.h"
#include "../snmp/snmp_api.h"
#include "sr_modem_mgmtmib.h"
#include "sr_modem_mgmtmib-mib.h"
#include "interface_private.h"
#include "../ts/if_async.h"
#include "../ts/modem_config.h"
#include "../dev/flash_defines.h"
#include "if_as5200_tdm.h"
#include "if_as5200_modem.h"
#include "modem_mgmt.h"
#include "parser_defs_modem.h"
#include "../parser/parser_defs_config.h"

static OctetString _cmManufacturerID;
static OctetString _cmProductDetails;
static OctetString _cmCallPhoneNumber;
static OctetString _cmCallerID;
static char string_buf[MAX_MODEM_STRING_LEN];

ulong *tx_cntr_from_speed(modem_info_t *, SR_UINT32);

/*
 * Start of system-independent routines
 */


/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */




/*---------------------------------------------------------------------
 * Retrieve data from the cmSystemInfo family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cmSystemInfo_get (OID             *incoming,
                  ObjectInfo      *object,
                  int             searchType,
                  ContextInfo     *contextInfo,
                  int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    cmSystemInfo_t  *data = NULL;

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
     if ( (arg == -1) || (data = k_cmSystemInfo_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cmSystemInstalledModem
    case I_cmSystemInstalledModem:
       dp = (void *) (&data->cmSystemInstalledModem);
       break;
#endif /* I_cmSystemInstalledModem */

#ifdef I_cmSystemConfiguredGroup
    case I_cmSystemConfiguredGroup:
       dp = (void *) (&data->cmSystemConfiguredGroup);
       break;
#endif /* I_cmSystemConfiguredGroup */

#ifdef I_cmSystemWatchdogTime
    case I_cmSystemWatchdogTime:
       dp = (void *) (&data->cmSystemWatchdogTime);
       break;
#endif /* I_cmSystemWatchdogTime */

#ifdef I_cmSystemStatusPollTime
    case I_cmSystemStatusPollTime:
       dp = (void *) (&data->cmSystemStatusPollTime);
       break;
#endif /* I_cmSystemStatusPollTime */

#ifdef I_cmSystemMaxRetries
    case I_cmSystemMaxRetries:
       dp = (void *) (&data->cmSystemMaxRetries);
       break;
#endif /* I_cmSystemMaxRetries */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cmSystemInfo data object.
 *---------------------------------------------------------------------*/
void
cmSystemInfo_free (cmSystemInfo_t  *data)
{
   if (data != NULL) {
       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cmSystemInfo set/undo
 *---------------------------------------------------------------------*/
static int cmSystemInfo_cleanup SR_PROTOTYPE ((doList_t *trash));

static int
cmSystemInfo_cleanup (doList_t *trash)
{
   cmSystemInfo_free((cmSystemInfo_t *) trash->data);
#ifdef SR_SNMPv2
   cmSystemInfo_free((cmSystemInfo_t *) trash->undodata);
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
cmSystemInfo_test (OID            *incoming,
                   ObjectInfo     *object,
                   ObjectSyntax   *value,
                   doList_t       *doHead,
                   doList_t       *doCur,
                   ContextInfo    *contextInfo)
{
    int            instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;

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
         if ( (dp->setMethod == cmSystemInfo_set) &&
            (((cmSystemInfo_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(cmSystemInfo_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(cmSystemInfo_t));

        dp->setMethod = cmSystemInfo_set;
        dp->cleanupMethod = cmSystemInfo_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cmSystemInfo_UNDO
        dp->undoMethod = cmSystemInfo_undo;
#else /* SR_cmSystemInfo_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cmSystemInfo_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

    }

    switch (object->nominator) {

#ifdef I_cmSystemWatchdogTime
   case I_cmSystemWatchdogTime:

     /* verify supplied value */
     if ((value->sl_value < MIN_WDT_VALUE) || 
         (value->sl_value > MAX_WDT_VALUE))
        return(WRONG_VALUE_ERROR);

     ((cmSystemInfo_t *) (dp->data))->cmSystemWatchdogTime = value->sl_value;
     break;
#endif /* I_cmSystemWatchdogTime */

#ifdef I_cmSystemStatusPollTime
   case I_cmSystemStatusPollTime:

     /* verify supplied value */
     if ((value->sl_value < MIN_POLL_TIME) || 
         (value->sl_value > MAX_POLL_TIME))
        return(WRONG_VALUE_ERROR);

     ((cmSystemInfo_t *) (dp->data))->cmSystemStatusPollTime = value->sl_value;
     break;
#endif /* I_cmSystemStatusPollTime */

#ifdef I_cmSystemMaxRetries
   case I_cmSystemMaxRetries:

     /* verify supplied value */
     if ((value->sl_value < MIN_POLL_RETRY) ||
         (value->sl_value > MAX_POLL_RETRY))
        return(WRONG_VALUE_ERROR);
 
     ((cmSystemInfo_t *) (dp->data))->cmSystemMaxRetries = value->sl_value;
     break;
#endif /* I_cmSystemMaxRetries */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cmSystemInfo_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cmSystemInfo_test */
   error_status = k_cmSystemInfo_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cmSystemInfo_t *) (dp->data))->valid);
        error_status = k_cmSystemInfo_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cmSystemInfo_set (doList_t       *doHead,
                  doList_t       *doCur,
                  ContextInfo    *contextInfo)
{
  return (k_cmSystemInfo_set((cmSystemInfo_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cmGroupEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cmGroupEntry_get (OID             *incoming,
                  ObjectInfo      *object,
                  int             searchType,
                  ContextInfo     *contextInfo,
                  int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cmGroupEntry_t  *data = NULL;
    SR_UINT32   buffer[1];
    OID             inst;
    int             carry;
    SR_UINT32       cmGroupIndex;

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

    if ( (InstToUInt(incoming, 0 + object->oid.length, &cmGroupIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cmGroupEntry_get(serialNum, contextInfo, arg ,searchType, cmGroupIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->cmGroupIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cmGroupTotalDevices
    case I_cmGroupTotalDevices:
       dp = (void *) (&data->cmGroupTotalDevices);
       break;
#endif /* I_cmGroupTotalDevices */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cmGroupMemberEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cmGroupMemberEntry_get (OID             *incoming,
                        ObjectInfo      *object,
                        int             searchType,
                        ContextInfo     *contextInfo,
                        int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cmGroupMemberEntry_t *data = NULL;
    SR_UINT32   buffer[3];
    OID             inst;
    int             carry;
    SR_UINT32       cmGroupIndex;
    SR_UINT32       cmSlotIndex;
    SR_UINT32       cmPortIndex;

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

    if ( (InstToUInt(incoming, 2 + object->oid.length, &cmPortIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToUInt(incoming, 1 + object->oid.length, &cmSlotIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToUInt(incoming, 0 + object->oid.length, &cmGroupIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cmGroupMemberEntry_get(serialNum, contextInfo, arg ,searchType, cmGroupIndex, cmSlotIndex, cmPortIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->cmGroupIndex;
        inst.oid_ptr[1] = data->cmSlotIndex;
        inst.oid_ptr[2] = data->cmPortIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cmPortIndex
    case I_cmPortIndex:
       dp = (void *) (&data->cmPortIndex);
       break;
#endif /* I_cmPortIndex */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cmLineStatusEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cmLineStatusEntry_get (OID             *incoming,
                       ObjectInfo      *object,
                       int             searchType,
                       ContextInfo     *contextInfo,
                       int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    cmLineStatusEntry_t *data = NULL;
    SR_UINT32   buffer[2];
    OID             inst;
    int             carry;
    SR_UINT32       cmSlotIndex;
    SR_UINT32       cmPortIndex;
    modem_info_t    *modem_info = NULL;

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

    if ( (InstToUInt(incoming, 1 + object->oid.length, &cmPortIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToUInt(incoming, 0 + object->oid.length, &cmSlotIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cmLineStatusEntry_get(serialNum, contextInfo, arg ,searchType, cmSlotIndex, cmPortIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->cmSlotIndex;
        inst.oid_ptr[1] = data->cmPortIndex;
     }

     if ((data->cmSlotIndex < modem_pool->max_slots) && 
         (modem_pool->modem_slot[data->cmSlotIndex]) &&
         (data->cmPortIndex < modem_pool->modem_slot[data->cmSlotIndex]->total_modem_num)) { 
         modem_info = 
             CSM_MODEM_INFO(data->cmSlotIndex, data->cmPortIndex);
     }

     if (!modem_info) {
         return(NULL);
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cmInterface
    case I_cmInterface:
       dp = (void *) (&data->cmInterface);
       break;
#endif /* I_cmInterface */

#ifdef I_cmGroup
    case I_cmGroup:
       dp = (void *) (&data->cmGroup);
       break;
#endif /* I_cmGroup */

#ifdef I_cmManufacturerID
    case I_cmManufacturerID:
       dp = (void *) (CloneOctetString(data->cmManufacturerID));
       break;
#endif /* I_cmManufacturerID */

#ifdef I_cmProductDetails
    case I_cmProductDetails:
       dp = (void *) (CloneOctetString(data->cmProductDetails));
       break;
#endif /* I_cmProductDetails */

#ifdef I_cmManageable
    case I_cmManageable:
       dp = (void *) (&data->cmManageable);
       break;
#endif /* I_cmManageable */

#ifdef I_cmState
    case I_cmState:
       dp = (void *) (&data->cmState);
       break;
#endif /* I_cmState */

#ifdef I_cmCallDirection
    case I_cmCallDirection:
       dp = (void *) (&data->cmCallDirection);
       break;
#endif /* I_cmCallDirection */

#ifdef I_cmDisconnectReason
    case I_cmDisconnectReason:
       dp = (void *) (&data->cmDisconnectReason);
       break;
#endif /* I_cmDisconnectReason */

#ifdef I_cmCallDuration
    case I_cmCallDuration:
       dp = (void *) (&data->cmCallDuration);
       break;
#endif /* I_cmCallDuration */

#ifdef I_cmCallPhoneNumber
    case I_cmCallPhoneNumber:
       dp = (void *) (CloneOctetString(data->cmCallPhoneNumber));
       break;
#endif /* I_cmCallPhoneNumber */

#ifdef I_cmCallerID
    case I_cmCallerID:
       dp = (void *) (CloneOctetString(data->cmCallerID));
       break;
#endif /* I_cmCallerID */

#ifdef I_cmModulationSchemeUsed
    case I_cmModulationSchemeUsed:
       /* 
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmModulationSchemeUsed);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmModulationSchemeUsed */

#ifdef I_cmProtocolUsed
    case I_cmProtocolUsed:
       /* 
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmProtocolUsed);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmProtocolUsed */

#ifdef I_cmTXRate
    case I_cmTXRate:
       /* 
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmTXRate);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmTXRate */

#ifdef I_cmRXRate
    case I_cmRXRate:
       /* 
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmRXRate);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmRXRate */

#ifdef I_cmTXAnalogSignalLevel
    case I_cmTXAnalogSignalLevel:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmTXAnalogSignalLevel);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmTXAnalogSignalLevel */

#ifdef I_cmRXAnalogSignalLevel
    case I_cmRXAnalogSignalLevel:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmRXAnalogSignalLevel);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmRXAnalogSignalLevel */

#ifdef I_cmATModePermit
    case I_cmATModePermit:
       dp = (void *) (&data->cmATModePermit);
       break;
#endif /* I_cmATModePermit */

#ifdef I_cmStatusPolling
    case I_cmStatusPolling:
       dp = (void *) (&data->cmStatusPolling);
       break;
#endif /* I_cmStatusPolling */

#ifdef I_cmBusyOutRequest
    case I_cmBusyOutRequest:
       dp = (void *) (&data->cmBusyOutRequest);
       break;
#endif /* I_cmBusyOutRequest */

#ifdef I_cmShutdown
    case I_cmShutdown:
       dp = (void *) (&data->cmShutdown);
       break;
#endif /* I_cmShutdown */

#ifdef I_cmHoldReset
    case I_cmHoldReset:
       dp = (void *) (&data->cmHoldReset);
       break;
#endif /* I_cmHoldReset */

#ifdef I_cmBad
    case I_cmBad:
       dp = (void *) (&data->cmBad);
       break;
#endif /* I_cmBad */

#ifdef I_cmRingNoAnswers
    case I_cmRingNoAnswers:
       dp = (void *) (&data->cmRingNoAnswers);
       break;
#endif /* I_cmRingNoAnswers */

#ifdef I_cmIncomingConnectionFailures
    case I_cmIncomingConnectionFailures:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmIncomingConnectionFailures);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmIncomingConnectionFailures */

#ifdef I_cmIncomingConnectionCompletions
    case I_cmIncomingConnectionCompletions:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmIncomingConnectionCompletions);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmIncomingConnectionCompletions */

#ifdef I_cmOutgoingConnectionFailures
    case I_cmOutgoingConnectionFailures:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmOutgoingConnectionFailures);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmOutgoingConnectionFailures */

#ifdef I_cmOutgoingConnectionCompletions
    case I_cmOutgoingConnectionCompletions:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmOutgoingConnectionCompletions);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmOutgoingConnectionCompletions */

#ifdef I_cmFailedDialAttempts
    case I_cmFailedDialAttempts:
       dp = (void *) (&data->cmFailedDialAttempts);
       break;
#endif /* I_cmFailedDialAttempts */

#ifdef I_cmNoDialTones
    case I_cmNoDialTones:
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmNoDialTones);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmNoDialTones */

#ifdef I_cmDialTimeouts
    case I_cmDialTimeouts:
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmDialTimeouts);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmDialTimeouts */

#ifdef I_cmWatchdogTimeouts
    case I_cmWatchdogTimeouts:
       dp = (void *) (&data->cmWatchdogTimeouts);
       break;
#endif /* I_cmWatchdogTimeouts */

#ifdef I_cm2400OrLessConnections
    case I_cm2400OrLessConnections:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cm2400OrLessConnections);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cm2400OrLessConnections */

#ifdef I_cm2400To14400Connections
    case I_cm2400To14400Connections:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cm2400To14400Connections);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cm2400To14400Connections */

#ifdef I_cmGreaterThan14400Connections
    case I_cmGreaterThan14400Connections:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmGreaterThan14400Connections);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmGreaterThan14400Connections */

#ifdef I_cmNoCarriers
    case I_cmNoCarriers:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmNoCarriers);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmNoCarriers */

#ifdef I_cmLinkFailures
    case I_cmLinkFailures:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmLinkFailures);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmLinkFailures */

#ifdef I_cmProtocolErrors
    case I_cmProtocolErrors:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmProtocolErrors);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmProtocolErrors */

#ifdef I_cmPollingTimeouts
    case I_cmPollingTimeouts:
       /*
        * This field is only valid for manageable modems
        */
       if (modem_info->modem_mgmt_info) {
           dp = (void *) (&data->cmPollingTimeouts);
       } else {
           return ((VarBind *) NULL);
       }
       break;
#endif /* I_cmPollingTimeouts */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}


/*---------------------------------------------------------------------
 * Retrieve data from the cmLineSpeedStatisticsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cmLineSpeedStatisticsEntry_get(OID          *incoming,
                               ObjectInfo   *object,
                               int          searchType,
                               ContextInfo  *contextInfo,
                               int          serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cmLineSpeedStatisticsEntry_t *data = NULL;
    SR_UINT32       buffer[3];
    OID             inst;
    int             carry;
    SR_UINT32       cmSlotIndex;
    SR_UINT32       cmPortIndex;
    SR_UINT32       cmInitialLineSpeed;


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

    if ( (InstToInt(incoming, 2 + object->oid.length, &cmInitialLineSpeed, searchType ,
 &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToUInt(incoming, 1 + object->oid.length, &cmPortIndex, searchType,
 &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToUInt(incoming, 0 + object->oid.length, &cmSlotIndex, searchType,
 &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cmLineSpeedStatisticsEntry_get(serialNum,
contextInfo, arg ,searchType, cmSlotIndex, cmPortIndex, cmInitialLineSpeed)) == NULL)
{
        arg = -1;
     }
     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->cmSlotIndex;
        inst.oid_ptr[1] = data->cmPortIndex;
        inst.oid_ptr[2] = data->cmInitialLineSpeed;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cmInitialLineConnections
    case I_cmInitialLineConnections:
       dp = (void *) (&data->cmInitialLineConnections);
       break;
#endif /* I_cmInitialLineConnections */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}




#ifdef SETS
 
/*----------------------------------------------------------------------
 * Free the cmLineStatusEntry data object.
 *---------------------------------------------------------------------*/
void
cmLineStatusEntry_free (cmLineStatusEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->cmManufacturerID);
       FreeOctetString(data->cmProductDetails);
       FreeOctetString(data->cmCallPhoneNumber);
       FreeOctetString(data->cmCallerID);

       free ((char *) data);
   }
}
 
/*----------------------------------------------------------------------
 * cleanup after cmLineStatusEntry set/undo
 *---------------------------------------------------------------------*/
static int cmLineStatusEntry_cleanup SR_PROTOTYPE ((doList_t *trash));
 
static int
cmLineStatusEntry_cleanup (doList_t *trash)
{
   cmLineStatusEntry_free((cmLineStatusEntry_t *) trash->data);
#ifdef SR_SNMPv2
   cmLineStatusEntry_free((cmLineStatusEntry_t *) trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}
 
/*----------------------------------------------------------------------
 * clone the cmLineStatusEntry family
 *---------------------------------------------------------------------*/
cmLineStatusEntry_t *
Clone_cmLineStatusEntry (cmLineStatusEntry_t *cmLineStatusEntry)
{
    cmLineStatusEntry_t *data;

    if ((data = malloc(sizeof(cmLineStatusEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cmLineStatusEntry), sizeof(cmLineStatusEntry_t));

    data->cmManufacturerID = CloneOctetString(cmLineStatusEntry->cmManufacturerID);
    data->cmProductDetails = CloneOctetString(cmLineStatusEntry->cmProductDetails);
    data->cmCallPhoneNumber = CloneOctetString(cmLineStatusEntry->cmCallPhoneNumber);
    data->cmCallerID = CloneOctetString(cmLineStatusEntry->cmCallerID);

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
cmLineStatusEntry_test (OID            *incoming,
                        ObjectInfo     *object,
                        ObjectSyntax   *value,
                        doList_t       *doHead,
                        doList_t       *doCur,
                        ContextInfo    *contextInfo)
{
    int            instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    cmLineStatusEntry_t     *cmLineStatusEntry;
    SR_UINT32      cmSlotIndex;
    SR_UINT32      cmPortIndex;
 
   /*
    * Validate the object instance
    *
    */
    if (instLength != 2) {
        return NO_CREATION_ERROR;
    }
 
    if ( (InstToInt(incoming, 0 + object->oid.length, &cmSlotIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }
 
    if ( (InstToInt(incoming, 1 + object->oid.length, &cmPortIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }
 
    if (error_status != NO_ERROR) {
        return error_status;
    }
 
    cmLineStatusEntry = k_cmLineStatusEntry_get(-1, contextInfo, -1, EXACT, cmSlotIndex, cmPortIndex);
 
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;
 
    for (dp = doHead; dp != NULL; dp = dp->next) {
         if ( (dp->setMethod == cmLineStatusEntry_set) &&
            (((cmLineStatusEntry_t *) (dp->data)) != NULL) &&
            (((cmLineStatusEntry_t *) (dp->data))->cmSlotIndex == cmSlotIndex) &&
            (((cmLineStatusEntry_t *) (dp->data))->cmPortIndex == cmPortIndex) ) {
 
            found = 1;
            break;
        }
    }
 
    if (!found) {
        dp = doCur;
 
        dp->setMethod = cmLineStatusEntry_set;
        dp->cleanupMethod = cmLineStatusEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cmLineStatusEntry_UNDO
        dp->undoMethod = cmLineStatusEntry_undo;
#else /* SR_cmLineStatusEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cmLineStatusEntry_UNDO */
#endif /* SR_SNMPv2 */
        dp->state = UNKNOWN;

        if (cmLineStatusEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cmLineStatusEntry(cmLineStatusEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2
            if ((dp->undodata = (void *) Clone_cmLineStatusEntry(cmLineStatusEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef cmLineStatusEntry_READ_CREATE
            if ( (dp->data = malloc(sizeof(cmLineStatusEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cmLineStatusEntry_t));
#ifdef SR_SNMPv2
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cmLineStatusEntry_t *) (dp->data))->cmSlotIndex = (cmSlotIndex);
                SET_VALID(I_cmLineStatusEntryIndex_cmSlotIndex, ((cmLineStatus Entry_t *) (dp->data))->valid);

                ((cmLineStatusEntry_t *) (dp->data))->cmPortIndex = (cmPortIndex);
                SET_VALID(I_cmLineStatusEntryIndex_cmPortIndex, ((cmLineStatusEntry_t *) (dp->data))->valid);

                error_status = k_cmLineStatusEntry_set_defaults(dp);
            }
#else /* cmLineStatusEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* cmLineStatusEntry_READ_CREATE */
        }
    }
 
    if (error_status != NO_ERROR) {
        return error_status;
    }
 
    switch (object->nominator) {
 
#ifdef I_cmATModePermit
   case I_cmATModePermit:
 
     if ((value->sl_value != D_cmATModePermit_true) && 
         (value->sl_value != D_cmATModePermit_false)) {
         return WRONG_VALUE_ERROR;
     }
 
     ((cmLineStatusEntry_t *) (dp->data))->cmATModePermit = value->sl_value;
     break;
#endif /* I_cmATModePermit */
 
#ifdef I_cmStatusPolling
   case I_cmStatusPolling:
 
     if ((value->sl_value != D_cmStatusPolling_true) && 
         (value->sl_value != D_cmStatusPolling_false)) {
         return WRONG_VALUE_ERROR;
     }
     ((cmLineStatusEntry_t *) (dp->data))->cmStatusPolling = value->sl_value;
     break;
#endif /* I_cmStatusPolling */
 
#ifdef I_cmBusyOutRequest
   case I_cmBusyOutRequest:
 
     if ((value->sl_value != D_cmBusyOutRequest_true) && 
         (value->sl_value != D_cmBusyOutRequest_false)) {
         return WRONG_VALUE_ERROR;
     }

     ((cmLineStatusEntry_t *) (dp->data))->cmBusyOutRequest = value->sl_value;
     break;
#endif /* I_cmBusyOutRequest */
 
#ifdef I_cmShutdown
   case I_cmShutdown:
 
     if ((value->sl_value != D_cmShutdown_true) && 
         (value->sl_value != D_cmShutdown_false)) {
         return WRONG_VALUE_ERROR;
     }
 
     ((cmLineStatusEntry_t *) (dp->data))->cmShutdown = value->sl_value;
     break;
#endif /* I_cmShutdown */
 
#ifdef I_cmHoldReset
   case I_cmHoldReset:
 
     if ((value->sl_value != D_cmHoldReset_true) && 
         (value->sl_value != D_cmHoldReset_false)) {
         return WRONG_VALUE_ERROR;
     }
 
     ((cmLineStatusEntry_t *) (dp->data))->cmHoldReset = value->sl_value;
     break;
#endif /* I_cmHoldReset */
 
#ifdef I_cmBad
   case I_cmBad:
 
     if ((value->sl_value != D_cmBad_true) && 
         (value->sl_value != D_cmBad_false)) {
         return WRONG_VALUE_ERROR;
     }
 
     ((cmLineStatusEntry_t *) (dp->data))->cmBad = value->sl_value;
     break;
#endif /* I_cmBad */
 
   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cmLineStatusEntry_test)\n"));
       return GEN_ERROR;
 
   }        /* switch */
 
   /* Do system dependent testing in k_cmLineStatusEntry_test */
   error_status = k_cmLineStatusEntry_test(object, value, dp, contextInfo);
 
   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cmLineStatusEntry_t *) (dp->data))->valid);
        error_status = k_cmLineStatusEntry_ready(object, value, doHead, dp);
   }
 
   return (error_status);
}
 
/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int
cmLineStatusEntry_set (doList_t       *doHead,
                       doList_t       *doCur,
                       ContextInfo    *contextInfo)
{
  return (k_cmLineStatusEntry_set((cmLineStatusEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}
 
#endif /* SETS */



/*
 * Start of system-dependent routines
 */


/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */




cmSystemInfo_t *
k_cmSystemInfo_get (int serialNum,
                    ContextInfo *contextInfo,
                    int nominator)
{
   int slot;
   hwidbtype *group_idb;
   modem_group_t *tmp;
   static cmSystemInfo_t cmSystemInfoData;

   /*
    * cmSystemInstalledModem
    */
   cmSystemInfoData.cmSystemInstalledModem = 0; 
   if (modem_pool) {
       for (slot = 0; slot < modem_pool->max_slots; slot++) {
           if (modem_pool->modem_slot[slot]) {
               cmSystemInfoData.cmSystemInstalledModem += 
                   modem_pool->modem_slot[slot]->total_modem_num;;
           }
       }
   }
   
   /*
    * cmSystemConfiguredGroup
    */
   cmSystemInfoData.cmSystemConfiguredGroup = 0;
   tmp = modem_group_list;
   while (tmp) {
        group_idb = tmp->hwidb;
        /*
         * make sure the group isn't deleted before proceeding
         */
        if (group_idb && !(group_idb->status & IDB_DELETED)) {
            cmSystemInfoData.cmSystemConfiguredGroup++;
        }
        tmp = tmp->next;
   }

   /*
    * cmSystemWatchdogTime
    */
   if (modem_pool) {
   	cmSystemInfoData.cmSystemWatchdogTime = modem_pool->wdt_timeout_val / ONEMIN;
    }
    else {
    	cmSystemInfoData.cmSystemWatchdogTime = CSM_DEFAULT_WATCH_DOG_TIME_OUT / ONEMIN;
    }
   
   /*
    * cmSystemStatusPollTime
    */
   cmSystemInfoData.cmSystemStatusPollTime = modem_poll_time / ONESEC;

   /*
    * cmSystemMaxRetries
    */
   cmSystemInfoData.cmSystemMaxRetries = modem_poll_retry;

   return(&cmSystemInfoData);
}

#ifdef SETS
int
k_cmSystemInfo_test (ObjectInfo     *object,
                     ObjectSyntax   *value,
                     doList_t       *dp,
                     ContextInfo    *contextInfo)
{

    return NO_ERROR;
}

int
k_cmSystemInfo_ready (ObjectInfo     *object,
                      ObjectSyntax   *value,
                      doList_t       *doHead,
                      doList_t       *dp)
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cmSystemInfo_set (cmSystemInfo_t *data, 
                    ContextInfo *contextInfo, 
                    int function)
{
   /*
    * cmSystemWatchdogTime: SET REQUEST
    */
   if (VALID(I_cmSystemWatchdogTime, data->valid)) {
	   if (modem_pool) {
		   modem_pool->wdt_timeout_val = data->cmSystemWatchdogTime * ONEMIN;
	    }
   }

   /*
    * cmSystemStatusPollTime: SET REQUEST
    */
   if (VALID(I_cmSystemStatusPollTime, data->valid)) {
       modem_poll_time = data->cmSystemStatusPollTime * ONESEC;
   }

   /*
    * cmSystemMaxRetries: SET REQUEST
    */
   if (VALID(I_cmSystemMaxRetries, data->valid)) {
       modem_poll_retry = data->cmSystemMaxRetries;
   }
  
   return(NO_ERROR);
}

#ifdef SR_SNMPv2
#ifdef SR_cmSystemInfo_UNDO
/* add #define SR_cmSystemInfo_UNDO in modem_mgmtmib.h to
 * include the undo routine for the cmSystemInfo family.
 */
int
cmSystemInfo_undo (doList_t       *doHead,
                   doList_t       *doCur,
                   ContextInfo    *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cmSystemInfo_UNDO */
#endif /* SR_SNMPv2 */

#endif /* SETS */

cmGroupEntry_t *
k_cmGroupEntry_get (int serialNum,
                    ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_UINT32 cmGroupIndex)
{
   hwidbtype *group_idb = NULL;
   queuetype *memberQ = NULL;
   modem_group_t *tmp;
   static cmGroupEntry_t cmGroupEntryData;
   boolean found = FALSE;

   if (searchType == EXACT) {
        /*
         * find the EXACT group
         */
        tmp = modem_group_list;
        while (tmp && (tmp->hwidb->unit < cmGroupIndex)) {
            tmp = tmp->next;
        }
        if (tmp && (tmp->hwidb->unit == cmGroupIndex)) {
            group_idb = tmp->hwidb;
            /*
             * make sure the group isn't deleted before proceeding
             */
            if (group_idb && !(group_idb->status & IDB_DELETED)) {
                found = TRUE;
            }
        }
   } else {
        /*
         * find the NEXT group 
         */
       tmp = modem_group_list;
       while (tmp && (tmp->hwidb->unit < cmGroupIndex)) {
           tmp = tmp->next;
       }

       while (tmp && (!found)) {
           if (tmp) {
               group_idb = tmp->hwidb;
               /*
                * make sure the group isn't deleted before proceeding
                */
               if (group_idb && !(group_idb->status & IDB_DELETED)) {
                   found = TRUE;
                   cmGroupIndex = group_idb->unit;
               } else {
                   /*
                    * group is deleted, look at the next group
                    */
                   tmp = tmp->next;
               }
           } 
       }
   }

   if (found) {
       /*
        * cmGroupIndex
        */
       cmGroupEntryData.cmGroupIndex = cmGroupIndex;
   
       /*
        * cmGroupTotalDevices
        */
       memberQ = &group_idb->firstsw->async_groupQ;
       cmGroupEntryData.cmGroupTotalDevices = QUEUESIZE(memberQ);
       return(&cmGroupEntryData);
   } else {
       return(NULL);
   }
}

cmGroupMemberEntry_t *
k_cmGroupMemberEntry_get (int serialNum,
                          ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_UINT32 cmGroupIndex,
                          SR_UINT32 cmSlotIndex,
                          SR_UINT32 cmPortIndex)
{
   static cmGroupMemberEntry_t cmGroupMemberEntryData;
   modem_info_t *modem_info;
   hwidbtype *group_idb = NULL;
   queuetype *memberQ = NULL;
   async_member_info *member;
   boolean found = FALSE;
   modem_group_t *tmp;
   int slot = 0;
   int port = 0;

   if (searchType == EXACT) {
       /*
        * look for an EXACT modem in a modem group
        */    

       /*
        * find the group hwidb
        */
       tmp = modem_group_list;
       while (tmp && (tmp->hwidb->unit < cmGroupIndex)) {
           tmp = tmp->next;
       }
       if (tmp && (tmp->hwidb->unit == cmGroupIndex)) {
           group_idb = tmp->hwidb;
           /*
            * make sure the group isn't deleted before proceeding
            */
           if (group_idb && !(group_idb->status & IDB_DELETED)) {
               memberQ = &group_idb->firstsw->async_groupQ;
               for (member = peekqueuehead(memberQ);
                    (member && (!found)); member=member->next) { 
                   modem_info = idb_get_hwsb_inline(member->hwidb, 
                                                    HWIDB_SB_MODEM_INFO);
                   if ((cmSlotIndex == CSM_MODEM_SLOT(modem_info)) &&
                      (cmPortIndex == CSM_MODEM_PORT(modem_info))) { 
                       /* 
                        * found the EXACT modem in group
                        */
                       found = TRUE;
                   }
               }
           }
       }
   } else {
       /*
        * find the NEXT modem in a group
        * NOTE: we are relying that the modems are in sequential
        * order in the modem group.
        */

       /*
        * find the group hwidb
        */
       tmp = modem_group_list;
       while (tmp && (tmp->hwidb->unit < cmGroupIndex)) {
           tmp = tmp->next;
       }
       if (tmp && (tmp->hwidb->unit == cmGroupIndex)) {
           group_idb = tmp->hwidb;
           /*
            * make sure the group isn't deleted before proceeding
            */
           if (group_idb && !(group_idb->status & IDB_DELETED)) {
               memberQ = &group_idb->firstsw->async_groupQ;
               for (member = peekqueuehead(memberQ);
                    (member && (member->hwidb) && (!found)); 
                     member = member->next) {
                   modem_info = idb_get_hwsb_inline(member->hwidb,
                                                    HWIDB_SB_MODEM_INFO);
                   slot = CSM_MODEM_SLOT(modem_info);
                   port = CSM_MODEM_PORT(modem_info);
                   /* 
                    * This modem is the first modem in the same slot right 
                    * after the passed in modem cmPortIndex so it must be 
                    * the NEXT modem
                    *
                    * OR
                    *
                    * We looked through all the modems in the same slot so
                    * if the slot is greater than the passed in cmSlotIndex,
                    * this must be the NEXT modem.
                    */ 
                   if (((slot == cmSlotIndex) && (port >= cmPortIndex)) ||
                       (slot > cmSlotIndex)) {
                       cmSlotIndex = slot;
                       cmPortIndex = port;
                       found = TRUE;  
                   } 
               }
           }
       }

       /* 
        * Did not find a member in the cmGroupIndex, so just retrieve a 
        * member from the next available group as the NEXT modem.
        */
       if (!found) {
           /* 
            * do we need to increment to the next group?
            */
           if (tmp && (tmp->hwidb->unit == cmGroupIndex)) {
               tmp = tmp->next;
           }

           while (tmp && (!found)) {
               group_idb = tmp->hwidb;
               /*
                * make sure the group isn't deleted before proceeding
                */
               if (group_idb && !(group_idb->status & IDB_DELETED)) {
                   /*
                    * we have found the next group, retrieve the first 
                    * member from this group to return
                    */
                   memberQ = &group_idb->firstsw->async_groupQ;
                   member = peekqueuehead(memberQ);
                   if (member && member->hwidb) {
                       modem_info = idb_get_hwsb_inline(member->hwidb,
                                                        HWIDB_SB_MODEM_INFO);
                       cmGroupIndex = group_idb->unit;
                       cmSlotIndex = CSM_MODEM_SLOT(modem_info);
                       cmPortIndex = CSM_MODEM_PORT(modem_info);
                       found = TRUE;
                   } else {
                       tmp = tmp->next;
                   }
               } else {
                   /*
                    * group has been deleted, go to next group
                    */
                   tmp = tmp->next;
               }
           }
       }
   }

   /*
    * Did we find the EXACT or NEXT modem?
    */
   if (found) {
       cmGroupMemberEntryData.cmGroupIndex = cmGroupIndex;
       cmGroupMemberEntryData.cmSlotIndex = cmSlotIndex;
       cmGroupMemberEntryData.cmPortIndex = cmPortIndex;
       return(&cmGroupMemberEntryData);
   } else {
       return(NULL);
   }
}

cmLineStatusEntry_t *
k_cmLineStatusEntry_get (int serialNum,
                         ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_UINT32 cmSlotIndex,
                         SR_UINT32 cmPortIndex)
{
   static cmLineStatusEntry_t cmLineStatusEntryData;
   modem_info_t *modem_info = NULL;
   modem_mgmt_info_t *modem_mgmt_info;
   modem_statistics_t *modem_stats = NULL;
   long tmp;
   sys_timestamp current_time;
   hwidbtype *group_idb;
   int modem_in_slot = 0;

   if (modem_pool) {
       if (searchType == EXACT) {
           /*
            * retrieve EXACT modem with slot and port index
            */
           if ((cmSlotIndex < modem_pool->max_slots) &&
               modem_pool->modem_slot[cmSlotIndex] &&
               (cmPortIndex < modem_pool->modem_slot[cmSlotIndex]->total_modem_num))
               modem_info = CSM_MODEM_INFO(cmSlotIndex, cmPortIndex);
       } else {
           /*
            * Try to find NEXT modem
            */
           while ((cmSlotIndex < modem_pool->max_slots) && (!modem_info)) {
	       if (modem_pool->modem_slot[cmSlotIndex]) {
                   modem_in_slot = modem_pool->modem_slot[cmSlotIndex]->total_modem_num;
                   while ((cmPortIndex < modem_in_slot) && (!modem_info)) {
                       modem_info = CSM_MODEM_INFO(cmSlotIndex, cmPortIndex);
                       if (!modem_info) {
                           cmPortIndex++;
                       }
                   }
               }
               /*
                * Is there a NEXT modem yet?  If not, look at next slot.
                */
               if (!modem_info) {
                   cmSlotIndex++;
                   cmPortIndex = 0;
               }
           }
       }
   }

   /*
    * don't have a modem?
    */
   if (!modem_info) {
       return(NULL);
   }

   modem_mgmt_info = modem_info->modem_mgmt_info;

   /*
    * cmSlotIndex and cmPortIndex
    */
   cmLineStatusEntryData.cmSlotIndex = cmSlotIndex;
   cmLineStatusEntryData.cmPortIndex = cmPortIndex;

   /* 
    * cmInterface
    */
   if (modem_info->tty_hwidb && !(modem_info->tty_hwidb->status&IDB_DELETED)) {
       cmLineStatusEntryData.cmInterface = modem_info->tty_hwidb->snmp_if_index;
   } else {
       cmLineStatusEntryData.cmInterface = 0;
   }

   /*
    * cmGroup: check to see if the async idb that the modem is
    * connected to is in group-async, if so, use that group #
    */
   if (modem_info->tty_hwidb) {
       group_idb = find_async_group_leader(modem_info->tty_hwidb);
       if (group_idb && !(group_idb->status & IDB_DELETED)) {
           cmLineStatusEntryData.cmGroup = group_idb->unit;
       } else {
           cmLineStatusEntryData.cmGroup = 0;
       }
    } else {
       cmLineStatusEntryData.cmGroup = 0;
    }

   /*
    * cmManufacturerID
    */
   cmLineStatusEntryData.cmManufacturerID = &_cmManufacturerID;
   cmLineStatusEntryData.cmManufacturerID->length = 
       strlen(MODEM_VENDOR_NAME(modem_info));
   cmLineStatusEntryData.cmManufacturerID->octet_ptr = 
       MODEM_VENDOR_NAME(modem_info);

   /*
    * cmProductDetails
    */
   if (modem_mgmt_info) {
       sprintf(string_buf, "Modem (Select Option), "
           "Firmware (Boot) Rev: %d.%d(%d) (%d.%d(%d))",
           modem_mgmt_info->modem_status_info.modem_firmware_rev[0],
           modem_mgmt_info->modem_status_info.modem_firmware_rev[1],
           modem_mgmt_info->modem_status_info.modem_firmware_rev[2],
           modem_mgmt_info->modem_status_info.modem_boot_firmware_rev[0],
           modem_mgmt_info->modem_status_info.modem_boot_firmware_rev[1],
           modem_mgmt_info->modem_status_info.modem_boot_firmware_rev[2]);
   } else {
       sprintf(string_buf,"Modem (Reliable Option)");
   }
   cmLineStatusEntryData.cmProductDetails = &_cmProductDetails;
   cmLineStatusEntryData.cmProductDetails->length = strlen(string_buf);
   cmLineStatusEntryData.cmProductDetails->octet_ptr = &string_buf[0];

   /* 
    * cmManageable
    */
   if (modem_mgmt_info) {
       cmLineStatusEntryData.cmManageable = D_cmManageable_true;
   } else {
       cmLineStatusEntryData.cmManageable = D_cmManageable_false;
   }

   /*
    * cmState
    */
   if (CSM_IS_ACTIVE_CALL(modem_info)) {
       if (CSM_IS_CONNECTED_STATE(modem_info)) {
           cmLineStatusEntryData.cmState = D_cmState_connected;
       } else {
           cmLineStatusEntryData.cmState = D_cmState_offHook;
       }
   } else if (CSM_IS_BACK2BACK_TEST(modem_info)) {
       cmLineStatusEntryData.cmState = D_cmState_loopback;
   } else if (CSM_IS_MODEM_RESET(modem_info)) {
       cmLineStatusEntryData.cmState = D_cmState_disabled;
   } else if (CSM_IS_DOWNLOAD_FILE(modem_info)) {
       cmLineStatusEntryData.cmState = D_cmState_downloadFirmware;
   } else if (CSM_IS_DOWNLOAD_FAIL(modem_info)) {
       cmLineStatusEntryData.cmState = D_cmState_downloadFirmwareFailed;
   } else if (CSM_IS_MODEM_BAD(modem_info)) {
       cmLineStatusEntryData.cmState = D_cmState_bad;
   } else if (CSM_IS_BUSY_OUT(modem_info) || CSM_IS_SHUTDOWN(modem_info)) {
       cmLineStatusEntryData.cmState = D_cmState_busiedOut;
   } else if (CSM_IS_IDLE_READY(modem_info)) {
       cmLineStatusEntryData.cmState = D_cmState_onHook;
   } else {
       cmLineStatusEntryData.cmState = D_cmState_unknown;
   }

   /*
    * cmCallDirection
    */
   if (modem_info->last_call_direction == CALL_ORIGINATE) {
       cmLineStatusEntryData.cmCallDirection = D_cmCallDirection_outgoing;
   } else if (modem_info->last_call_direction == CALL_ANSWER) {
       cmLineStatusEntryData.cmCallDirection = D_cmCallDirection_incoming;
   } else {
       cmLineStatusEntryData.cmCallDirection = D_cmCallDirection_none;
   }

   /*
    * cmCallDuration
    */
   if (CSM_IS_ACTIVE_CALL(modem_info)) {
       /* 
        * call is still up so use current time
        */
       GET_TIMESTAMP(current_time);
       cmLineStatusEntryData.cmCallDuration = 
           (CLOCK_DIFF_UNSIGNED(modem_info->stat_call_duration_started,
                               current_time) / SYSUPTIME_SCALE) & MAXULONG;
   } else {
       cmLineStatusEntryData.cmCallDuration = 
           (CLOCK_DIFF_UNSIGNED(modem_info->stat_call_duration_started,
                               modem_info->stat_call_duration_ended) /
           SYSUPTIME_SCALE) & MAXULONG;
   }

   /*
    * cmCallPhoneNumber
    */
   cmLineStatusEntryData.cmCallPhoneNumber = &_cmCallPhoneNumber;
   cmLineStatusEntryData.cmCallPhoneNumber->length = 
      strlen(modem_info->last_called_number);
   cmLineStatusEntryData.cmCallPhoneNumber->octet_ptr = 
       modem_info->last_called_number;

   
   /*
    * cmCallerID
    */
   cmLineStatusEntryData.cmCallerID = &_cmCallerID;
   cmLineStatusEntryData.cmCallerID->length = 
       strlen(modem_info->last_calling_number);
   cmLineStatusEntryData.cmCallerID->octet_ptr = 
       modem_info->last_calling_number;

   /*
    * cmATModePermit
    */
   if (modem_info->modem_config & CFG_MODEM_DC_SESSION) {
       cmLineStatusEntryData.cmATModePermit = D_cmATModePermit_true;
   } else {
       cmLineStatusEntryData.cmATModePermit = D_cmATModePermit_false;
   }
  
   /*
    * cmStatusPolling
    */
   if (modem_info->modem_config & CFG_MODEM_STATUS_POLL) {
       cmLineStatusEntryData.cmStatusPolling = D_cmStatusPolling_true;
   } else {
       cmLineStatusEntryData.cmStatusPolling = D_cmStatusPolling_false;
   }

   /*
    * cmBusyOutRequest
    */
   if (modem_info->modem_tty && 
       (modem_info->modem_tty->capabilities2 & MODEM_BUSYOUT_2)) {
       cmLineStatusEntryData.cmBusyOutRequest = D_cmBusyOutRequest_true;
   } else {
       cmLineStatusEntryData.cmBusyOutRequest = D_cmBusyOutRequest_false;
   }

   /*
    * cmShutdown
    */
   if (modem_info->modem_config & CFG_MODEM_SHUTDOWN) {
       cmLineStatusEntryData.cmShutdown = D_cmShutdown_true;
   } else {
       cmLineStatusEntryData.cmShutdown = D_cmShutdown_false;
   }

   /*
    * cmHoldReset
    */
   if (modem_info->modem_config & CFG_MODEM_HOLD_RESET) {
       cmLineStatusEntryData.cmHoldReset = D_cmHoldReset_true;
   } else {
       cmLineStatusEntryData.cmHoldReset = D_cmHoldReset_false;
   }

   /* 
    * cmBad
    */
   if (modem_info->modem_config & CFG_MODEM_BAD) {
       cmLineStatusEntryData.cmBad = D_cmBad_true;
   } else {
       cmLineStatusEntryData.cmBad = D_cmBad_false;
   }

   /*
    * cmRingNoAnswers
    */
   cmLineStatusEntryData.cmRingNoAnswers = modem_info->stat_ring_no_answer;
 
   /*
    * cmFailedDialAttempts
    */
   cmLineStatusEntryData.cmFailedDialAttempts = modem_info->stat_dial_failure;

   /*
    * cmWatchdogTimeouts
    */
   cmLineStatusEntryData.cmWatchdogTimeouts = modem_info->wdt_timeout_count;

   /*
    * These fields are only valid for Manageable modems
    */
   if (modem_mgmt_info) {
       modem_stats = &(modem_mgmt_info->modem_stats);
       /*
        * cmIncomingConnectionFailures
        */
       tmp = modem_stats->modem_ic_total - modem_stats->modem_ic_complete;
       if (tmp < 0) {
           tmp = 0;
       }
       cmLineStatusEntryData.cmIncomingConnectionFailures = tmp;
    
       /* 
        * cmIncomingConnectionCompletions
        */
       cmLineStatusEntryData.cmIncomingConnectionCompletions = 
           modem_stats->modem_ic_complete;
    
       /*
        * cmOutgoingConnectionFailures
        */
       tmp = modem_stats->modem_oc_total - modem_stats->modem_oc_complete;
       if (tmp < 0) {
           tmp = 0;
       }
       cmLineStatusEntryData.cmOutgoingConnectionFailures = tmp;
    
       /*
        * cmOutgoingConnectionCompletions
        */
       cmLineStatusEntryData.cmOutgoingConnectionCompletions = 
           modem_stats->modem_oc_complete;

       /*
        * cmDisconnectReason
        */
       switch (modem_mgmt_info->modem_status_info.disconnect_reason_local) {
       case DISC_LOST_CARRIER:
       case DISC_TERMINATE:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_lostCarrier;
           break;
       case DISC_NO_CARRIER:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_noCarrier;
           break;
       case DISC_NO_DIALTONE:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_noDialTone;
           break;
       case DISC_BUSY:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_busy;
           break;
       case DISC_WATCHDOG_TIMEOUT:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_modemWatchdogTimeout;
           break;
       case DISC_DTR_DROP:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_dtrDrop;
           break;
       case DISC_USER_HANGUP:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_userHangup;
           break;
       case DISC_COMPRESSION_PROBLEM:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_compressionProblem;
           break;
       case DISC_RETRAIN_FAILURE:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_retrainFailure;
           break;
       case DISC_REMOTE_LINK:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_remoteLinkDisconnect;
           break;
       case DISC_ABORT:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_abort;
           break;
       case DISC_INACTIVITY_TIMEOUT:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_inactivityTimeout;
           break;
       case DISC_DIAL_STRING_ERROR:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_dialStringError;
           break;
       case DISC_LINK_FAILURE:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_linkFailure;
           break;
       case DISC_MODULATION_ERROR:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_modulationError;
           break;
       case DISC_DIAL_TIMEOUT:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_dialTimeout;
           break;
       case DISC_REMOTE_HANGUP:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_remoteHangup;
           break;
       case DISC_MNP10_PROTOCOL_ERROR:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_mnp10ProtocolError;
           break;
       case DISC_LAPM_PROTOCOL_ERROR:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_lapmProtocolError;
           break;
       case DISC_FAX_CLASS2_ERROR:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_faxClass2Error;
           break;
       default:
           cmLineStatusEntryData.cmDisconnectReason = 
               D_cmDisconnectReason_unknown;
           break;
       }

       /*
        * cmModulationSchemeUsed
        */
       switch (modem_mgmt_info->modem_status_info.modulation) {
       case MODULATION_BELL_103A:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_bell103a;
           break;
       case MODULATION_BELL_212A:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_bell212a;
           break;
       case MODULATION_V21:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v21;
           break;
       case MODULATION_V22:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v22;
           break;
       case MODULATION_V22BIS:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v22bis;
           break;
       case MODULATION_V32:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v32;
           break;
       case MODULATION_V32BIS:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v32bis;
           break;
       case MODULATION_VFC:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_vfc;
           break;
       case MODULATION_V34:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v34;
           break;
       case MODULATION_V17:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v17;
           break;
       case MODULATION_V29:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v29;
           break;
       case MODULATION_V33:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_v33;
           break;
       case MODULATION_K56PLUS:
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_k56plus;
           break;
       default: 
           cmLineStatusEntryData.cmModulationSchemeUsed = 
               D_cmModulationSchemeUsed_unknown;
           break;
       }

       /*
        * cmProtocolUsed
        */
       switch (modem_mgmt_info->modem_status_info.protocol) {
       case PROTOCOL_NORMAL:
           cmLineStatusEntryData.cmProtocolUsed =
               D_cmProtocolUsed_normal; 
           break;
       case PROTOCOL_DIRECT:
           cmLineStatusEntryData.cmProtocolUsed =
               D_cmProtocolUsed_direct; 
           break;
       case PROTOCOL_RELIABLE_MNP:
           cmLineStatusEntryData.cmProtocolUsed =
               D_cmProtocolUsed_reliableMNP; 
           break;
       case PROTOCOL_RELIABLE_LAPM:
           cmLineStatusEntryData.cmProtocolUsed =
               D_cmProtocolUsed_reliableLAPM; 
           break;
       default:
           cmLineStatusEntryData.cmProtocolUsed =
               D_cmProtocolUsed_normal; 
           break;
       }
       
       /* 
        * cmTXRate and cmRXRate: right now this is only for Async
        * interfaces so transmits and receives are the same value
        */
       cmLineStatusEntryData.cmTXRate = 
           modem_mgmt_info->modem_status_info.tx_speed;
       cmLineStatusEntryData.cmRXRate = 
           modem_mgmt_info->modem_status_info.rx_speed;

       /*
        * cmTXAnalogSignalLevel
        */
       cmLineStatusEntryData.cmTXAnalogSignalLevel = 
           -1*modem_mgmt_info->modem_status_info.tx_level;

       /*
        * cmRXAnalogSignalLevel
        */
       cmLineStatusEntryData.cmRXAnalogSignalLevel = 
           -1*modem_mgmt_info->modem_status_info.rx_level;

       /*
        * cmNoDialTones
        */
       cmLineStatusEntryData.cmNoDialTones = modem_stats->modem_no_dial_tones;

       /*
        * cmDialTimeouts
        */
       cmLineStatusEntryData.cmDialTimeouts = modem_stats->modem_dial_timeouts;

       /*
        * cm2400OrLessConnections
        */
       cmLineStatusEntryData.cm2400OrLessConnections = 
           modem_stats->modem_75_connections +
           modem_stats->modem_300_connections +
           modem_stats->modem_600_connections +
           modem_stats->modem_1200_connections +
           modem_stats->modem_2400_connections;

       /*
        * cm2400To14400Connections
        */
       cmLineStatusEntryData.cm2400To14400Connections = 
           modem_stats->modem_4800_connections +
           modem_stats->modem_7200_connections +
           modem_stats->modem_9600_connections +
           modem_stats->modem_12000_connections +
           modem_stats->modem_14400_connections;

       /*
        * cmGreaterThan14400Connections
        */
       cmLineStatusEntryData.cmGreaterThan14400Connections = 
           modem_stats->modem_16800_connections +
           modem_stats->modem_21600_connections +
           modem_stats->modem_24000_connections +
           modem_stats->modem_26400_connections +
           modem_stats->modem_28800_connections +
           modem_stats->modem_31200_connections +
           modem_stats->modem_33600_connections;

       /*
        * cmNoCarriers
        */
       cmLineStatusEntryData.cmNoCarriers = 
           modem_stats->modem_no_carriers;
   
       /*
        * cmLinkFailures
        */
       cmLineStatusEntryData.cmLinkFailures = 
           modem_stats->modem_link_failures;

       /*  
        * cmProtocolErrors
        */
       cmLineStatusEntryData.cmProtocolErrors = 
           modem_stats->modem_protocol_errors;

       /* 
        * cmPollingTimeouts
        */
       cmLineStatusEntryData.cmPollingTimeouts = 
           modem_stats->modem_timeouts;
   }

   return(&cmLineStatusEntryData);
}


/*
 *  Description:
 *     Return the pointer to the appropriate tx_speed_counter in
 *     the structure modem_statistics_t.
 *  In:
 *     The numerical value of the line speed (75, 300, 600, etc.)
 *  Out:
 *     The pointer to the tx_speed_counter corresponding to the
 *     specified speed.
 */
ulong *tx_cntr_from_speed (modem_info_t *modem_info, 
                           SR_UINT32 cmInitialLineSpeed)
{
   modem_mgmt_info_t *modem_mgmt_info;
   modem_statistics_t *modem_stats;
   ulong *ret_cntr;

   if (!modem_info) {
       return(NULL);
   }

   modem_mgmt_info = modem_info->modem_mgmt_info;
   if (!modem_mgmt_info) {
       return(NULL);
   }

   modem_stats = &(modem_mgmt_info->modem_stats);

   switch(cmInitialLineSpeed) {
           case 75:
		ret_cntr = &modem_stats->modem_75_connections;
                break;
           case 300:
                ret_cntr = &modem_stats->modem_300_connections;
                break;
           case 600:
                ret_cntr = &modem_stats->modem_600_connections;
                break;
           case 1200:
                ret_cntr = &modem_stats->modem_1200_connections;
                break;
           case 2400:
                ret_cntr = &modem_stats->modem_2400_connections;
                break;
           case 4800:
                ret_cntr = &modem_stats->modem_4800_connections;
                break;
           case 7200:
                ret_cntr = &modem_stats->modem_7200_connections;
                break;
           case 9600:
                ret_cntr = &modem_stats->modem_9600_connections;
                break;
           case 12000:
                ret_cntr = &modem_stats->modem_12000_connections;
                break;
           case 14400:
                ret_cntr = &modem_stats->modem_14400_connections;
                break;
           case 16800:
                ret_cntr = &modem_stats->modem_16800_connections;
                break;
           case 19200:
                ret_cntr = &modem_stats->modem_19200_connections;
                break;
           case 21600:
                ret_cntr = &modem_stats->modem_21600_connections;
                break;
           case 24000:
                ret_cntr = &modem_stats->modem_24000_connections;
                break;
           case 26400:
                ret_cntr = &modem_stats->modem_26400_connections;
                break;
           case 28800:
                ret_cntr = &modem_stats->modem_28800_connections;
                break;
           case 31200:
                ret_cntr = &modem_stats->modem_31200_connections;
                break;
           case 33600:
                ret_cntr = &modem_stats->modem_33600_connections;
                break;
           default:
                return(NULL);
   }

   return(ret_cntr);
}


cmLineSpeedStatisticsEntry_t *
k_cmLineSpeedStatisticsEntry_get(int serialNum, ContextInfo *contextInfo,
                                 int nominator, int searchType,
                                 SR_UINT32 cmSlotIndex,
                                 SR_UINT32 cmPortIndex,
                                 SR_UINT32 cmInitialLineSpeed)
{
   static cmLineSpeedStatisticsEntry_t cmLineSpeedStatisticsEntryData;
   modem_info_t *modem_info = NULL;
   ulong *cntr_ptr;
   int modem_in_slot = 0;
   int i;

   cntr_ptr = NULL;
   if (modem_pool) {
       if (searchType == EXACT) {
           /*
            * retrieve EXACT modem with slot and port index
            */
           if ((cmSlotIndex < modem_pool->max_slots) &&
               modem_pool->modem_slot[cmSlotIndex] &&
               (cmPortIndex < modem_pool->modem_slot[cmSlotIndex]->total_modem_num)) {
               modem_info = CSM_MODEM_INFO(cmSlotIndex, cmPortIndex);
           } else {
	       return(NULL);
	   }

           /*
            * Check whether modem speed is valid or not
            */
           for(i = 0;i < MAX_MODEM_LINE_SPEEDS;i++)
           {
               if (cmInitialLineSpeed == modem_speed[i])
                   break;
           }
           if (i >= MAX_MODEM_LINE_SPEEDS){
                   return (NULL);
           }
	   cntr_ptr = tx_cntr_from_speed(modem_info, cmInitialLineSpeed);
	   if (!cntr_ptr || (*cntr_ptr <= 0)) {
	       return(NULL);
	   }

       } else {
           /*
            * Try to find NEXT modem
            */
           while ((cmSlotIndex < modem_pool->max_slots) && (!modem_info)) {
               if (modem_pool->modem_slot[cmSlotIndex]) {
                   modem_in_slot =
                       modem_pool->modem_slot[cmSlotIndex]->total_modem_num;

                   while ((cmPortIndex < modem_in_slot) && (!modem_info)) {
                       if (cmInitialLineSpeed >= 33600) {
                           cmPortIndex++;
                           cmInitialLineSpeed = 75;
                       }
                       else if (cmInitialLineSpeed >= 31200) {
                           cmInitialLineSpeed = 33600;
                       }
                       else if (cmInitialLineSpeed >= 28800) {
                           cmInitialLineSpeed = 31200;
                       }
                       else if (cmInitialLineSpeed >= 26400) {
                           cmInitialLineSpeed = 28800;
                       }
                       else if (cmInitialLineSpeed >= 24000) {
                           cmInitialLineSpeed = 26400;
                       }
                       else if (cmInitialLineSpeed >= 21600) {
                           cmInitialLineSpeed = 24000;
                       }
                       else if (cmInitialLineSpeed >= 19200) {
                           cmInitialLineSpeed = 21600;
                       }
                       else if (cmInitialLineSpeed >= 16800) {
                           cmInitialLineSpeed = 19200;
                       }
                       else if (cmInitialLineSpeed >= 14400) {
                           cmInitialLineSpeed = 16800;
                       }
                       else if (cmInitialLineSpeed >= 12000) {
                           cmInitialLineSpeed = 14400;
                       }
                       else if (cmInitialLineSpeed >= 9600) {
                           cmInitialLineSpeed = 12000;
                       }
                       else if (cmInitialLineSpeed >= 7200) {
                           cmInitialLineSpeed = 9600;
                       }
                       else if (cmInitialLineSpeed >= 4800) {
                           cmInitialLineSpeed = 7200;
                       }
                       else if (cmInitialLineSpeed >= 2400) {
                           cmInitialLineSpeed = 4800;
                       }
                       else if (cmInitialLineSpeed >= 1200) {
                           cmInitialLineSpeed = 2400;
                       }
                       else if (cmInitialLineSpeed >= 600) {
                           cmInitialLineSpeed = 1200;
                       }
                       else if (cmInitialLineSpeed >= 300) {
                           cmInitialLineSpeed = 600;
                       }
                       else if (cmInitialLineSpeed >= 75) {
                           cmInitialLineSpeed = 300;
                       }
                       else {
                           cmInitialLineSpeed = 75;
                       }

                       if (cmPortIndex < modem_in_slot) {
                           modem_info =
                             CSM_MODEM_INFO_OR_NULL(cmSlotIndex, cmPortIndex);
			   cntr_ptr =
			     tx_cntr_from_speed(modem_info, cmInitialLineSpeed);
                           if (!cntr_ptr || (*cntr_ptr <= 0)) {
			       modem_info = NULL;
			   }
                       }
                   }
               }
               /*
                * Is there a NEXT modem yet?  If not, look at next slot.
                */
               if (!modem_info) {
                   cmSlotIndex++;
                   cmPortIndex = 0;
                   cmInitialLineSpeed = 0;
               }
           }
       }
   }

   /*
    * don't have a modem?
    */
   if (!modem_info || !cntr_ptr) {
       return(NULL);
   }

   /*
    * cmSlotIndex, cmPortIndex and cmInitialLineSpeed
    */
   cmLineSpeedStatisticsEntryData.cmSlotIndex = cmSlotIndex;
   cmLineSpeedStatisticsEntryData.cmPortIndex = cmPortIndex;
   cmLineSpeedStatisticsEntryData.cmInitialLineSpeed = cmInitialLineSpeed;
   cmLineSpeedStatisticsEntryData.cmInitialLineConnections = *cntr_ptr;

   return(&cmLineSpeedStatisticsEntryData);
}


#ifdef SETS
int
k_cmLineStatusEntry_test (ObjectInfo     *object,
                          ObjectSyntax   *value,
                          doList_t       *dp,
                          ContextInfo    *contextInfo)
{
    return NO_ERROR;
}
 
int
k_cmLineStatusEntry_ready (ObjectInfo     *object,
                           ObjectSyntax   *value,
                           doList_t       *doHead,
                           doList_t       *dp)
{
    dp->state = ADD_MODIFY;
    return NO_ERROR;
}
 
int
k_cmLineStatusEntry_set_defaults (doList_t *dp)
{
    cmLineStatusEntry_t *data = (cmLineStatusEntry_t *) (dp->data);

    data->cmATModePermit = D_cmATModePermit_true;
    data->cmStatusPolling = D_cmStatusPolling_true;
    data->cmBusyOutRequest = D_cmBusyOutRequest_false;
    data->cmShutdown = D_cmShutdown_false;
    data->cmHoldReset = D_cmHoldReset_false;
    data->cmBad = D_cmBad_false;

    SET_ALL_VALID(data->valid);

    return NO_ERROR;
}

int
k_cmLineStatusEntry_set (cmLineStatusEntry_t *data,
                         ContextInfo *contextInfo,
                         int function)
{
   modem_info_t *modem_info;
   parseinfo *csb = NULL;

   modem_info = CSM_MODEM_INFO_OR_NULL(data->cmSlotIndex, data->cmPortIndex); 
   if (!modem_info) {
       return (GEN_ERROR);
   }

   /*
    * cmATModePermit: SET REQUEST
    */
   if (VALID(I_cmATModePermit, data->valid)) {
       switch (data->cmATModePermit) {
       case D_cmATModePermit_true:
           modem_info->modem_config |= CFG_MODEM_DC_SESSION;
           break;
       case D_cmATModePermit_false:
           modem_info->modem_config &= ~CFG_MODEM_DC_SESSION;
           break;
       }
   }

   /*
    * cmStatusPolling: SET REQUEST
    */
   if (VALID(I_cmStatusPolling, data->valid)) {
       switch (data->cmStatusPolling) {
       case D_cmStatusPolling_true:
           modem_info->modem_config |= CFG_MODEM_STATUS_POLL;
           break;
       case D_cmStatusPolling_false:
           modem_info->modem_config &= ~CFG_MODEM_STATUS_POLL;
           break;
       }
   }
 
   /*
    * cmBusyOutRequest: SET REQUEST
    */
   if (VALID(I_cmBusyOutRequest, data->valid)) {
       if (modem_info->modem_tty) {
           csb = malloc(sizeof(parseinfo));
           if (!csb) {
               return RESOURCE_UNAVAILABLE_ERROR;
           }
           csb->which = PARSER_LINE_MODEM_BUSYOUT;
           csb->nvgen = FALSE;
           csb->line1 = modem_info->modem_tty->ttynum;
           csb->line2 = csb->line1 + 1;
           switch (data->cmBusyOutRequest) {
           case D_cmBusyOutRequest_true:
               csb->sense = TRUE;
               modem_busyout_command(csb);
               break;
           case D_cmBusyOutRequest_false:
               csb->sense = FALSE;
               modem_busyout_command(csb);
               break;
           }
           free(csb);
       }
   }
 
   /*
    * cmShutdown: SET REQUEST
    */
   if (VALID(I_cmShutdown, data->valid)) {
       switch (data->cmShutdown) {
       case D_cmShutdown_true:
           if (modem_shutdown(modem_info, TRUE)) {
               modem_info->modem_config |= CFG_MODEM_SHUTDOWN;
           }
           break;
       case D_cmShutdown_false:
           if (modem_shutdown(modem_info, FALSE)) {
               modem_info->modem_config &= ~CFG_MODEM_SHUTDOWN;
           }
           break;
       }
   }

   /*
    * cmHoldReset: SET REQUEST
    */
   if (VALID(I_cmHoldReset, data->valid)) {
       switch (data->cmHoldReset) {
       case D_cmHoldReset_true:
           if (modem_reset(modem_info, TRUE, TRUE)) {
               modem_info->modem_config |= CFG_MODEM_HOLD_RESET;
           }
           break;
       case D_cmHoldReset_false:
           if (modem_reset(modem_info, FALSE, TRUE)) {
               modem_info->modem_config &= ~CFG_MODEM_HOLD_RESET;
           }
           break;
       }
   }

   /*
    * cmBad: SET REQUEST
    */
   if (VALID(I_cmBad, data->valid)) {
       switch (data->cmBad) {
       case D_cmBad_true:
           if (modem_bad(modem_info, TRUE)) {
               modem_info->modem_config |= CFG_MODEM_BAD;
           }
           break;
       case D_cmBad_false:
           if (modem_bad(modem_info, FALSE)) {
               modem_info->modem_config &= ~CFG_MODEM_BAD;
           }
           break;
       }
   }

   return (NO_ERROR);
}
 
#ifdef SR_SNMPv2
#ifdef SR_cmLineStatusEntry_UNDO
/* add #define SR_cmLineStatusEntry_UNDO in modem_mgmtmib.h to
 * include the undo routine for the cmLineStatusEntry family.
 */
int
cmLineStatusEntry_undo (doList_t       *doHead,
                        doList_t       *doCur,
                        ContextInfo    *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cmLineStatusEntry_UNDO */
#endif /* SR_SNMPv2 */
 
#endif /* SETS */


void
init_modem_mgmtmib (subsystype *subsys)
{
    load_mib(modem_mgmtmib_OidList, modem_mgmtmib_OidListNum);
    load_oid(modem_mgmtmib_oid_table);
}

/*
 * modem_mgmtmib subsystem header
 */

#define MAJVERSION_modem_mgmtmib 1
#define MINVERSION_modem_mgmtmib 0
#define EDITVERSION_modem_mgmtmib 0

SUBSYS_HEADER(modem_mgmtmib,
              MAJVERSION_modem_mgmtmib,
              MINVERSION_modem_mgmtmib,
              EDITVERSION_modem_mgmtmib,
              init_modem_mgmtmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              "req: modem_pool");
      
