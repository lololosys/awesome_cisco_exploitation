/* $Id: sr_rsvp_mib.c,v 1.1.50.2 1996/07/01 18:45:51 hampton Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/sr_rsvp_mib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rsvp_mib.c,v $
 * Revision 1.1.50.2  1996/07/01  18:45:51  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.50.1  1996/04/18  10:48:59  fred
 * CSCdi55001:  compiles; MIB implementation upon IETF consensus
 * Branch: California_branch
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
#include "../snmp/snmp_api.h"
#include "sr_rsvp_mib.h"
#include "sr_rsvp_mib-mib.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the rsvpSenderEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsvpSenderEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    rsvpSenderEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   rsvpSenderSessionIndex = NULL;
    int             rsvpSenderSessionIndex_offset;
    long            ifIndex;
    int             ifIndex_offset;
    int             index;
    int             final_index;

    rsvpSenderSessionIndex_offset = object->oid.length;
    ifIndex_offset = rsvpSenderSessionIndex_offset + GetVariableIndexLength(incoming, rsvpSenderSessionIndex_offset);
    final_index = ifIndex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, rsvpSenderSessionIndex_offset, &rsvpSenderSessionIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsvpSenderEntry_get(serialNum, contextInfo, arg ,searchType, rsvpSenderSessionIndex, ifIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->rsvpSenderSessionIndex->length;
        for(i = 0; i < data->rsvpSenderSessionIndex->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->rsvpSenderSessionIndex->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.length = index;
     }

        FreeOctetString(rsvpSenderSessionIndex);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsvpSenderTSpecRate
    case I_rsvpSenderTSpecRate:
       dp = (void *) (&data->rsvpSenderTSpecRate);
       break;
#endif /* I_rsvpSenderTSpecRate */

#ifdef I_rsvpSenderTSpecPeakRate
    case I_rsvpSenderTSpecPeakRate:
       dp = (void *) (&data->rsvpSenderTSpecPeakRate);
       break;
#endif /* I_rsvpSenderTSpecPeakRate */

#ifdef I_rsvpSenderTSpecBurst
    case I_rsvpSenderTSpecBurst:
       dp = (void *) (&data->rsvpSenderTSpecBurst);
       break;
#endif /* I_rsvpSenderTSpecBurst */

#ifdef I_rsvpSenderTSpecMinTU
    case I_rsvpSenderTSpecMinTU:
       dp = (void *) (&data->rsvpSenderTSpecMinTU);
       break;
#endif /* I_rsvpSenderTSpecMinTU */

#ifdef I_rsvpSenderTSpecMaxTU
    case I_rsvpSenderTSpecMaxTU:
       dp = (void *) (&data->rsvpSenderTSpecMaxTU);
       break;
#endif /* I_rsvpSenderTSpecMaxTU */

#ifdef I_rsvpSenderInterval
    case I_rsvpSenderInterval:
       dp = (void *) (&data->rsvpSenderInterval);
       break;
#endif /* I_rsvpSenderInterval */

#ifdef I_rsvpSenderRSVPHop
    case I_rsvpSenderRSVPHop:
       dp = (void *) (&data->rsvpSenderRSVPHop);
       break;
#endif /* I_rsvpSenderRSVPHop */

#ifdef I_rsvpSenderLastChange
    case I_rsvpSenderLastChange:
       dp = (void *) (&data->rsvpSenderLastChange);
       break;
#endif /* I_rsvpSenderLastChange */

#ifdef I_rsvpSenderStatus
    case I_rsvpSenderStatus:
       dp = (void *) (&data->rsvpSenderStatus);
       break;
#endif /* I_rsvpSenderStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rsvpSenderEntry data object.
 *---------------------------------------------------------------------*/
void
rsvpSenderEntry_free(
		     rsvpSenderEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->rsvpSenderSessionIndex);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after rsvpSenderEntry set/undo
 *---------------------------------------------------------------------*/
static int
rsvpSenderEntry_cleanup(doList_t *trash)
{
   rsvpSenderEntry_free(trash->data);
#ifdef SR_SNMPv2
   rsvpSenderEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the rsvpSenderEntry family
 *---------------------------------------------------------------------*/
rsvpSenderEntry_t *
Clone_rsvpSenderEntry(rsvpSenderEntry_t *rsvpSenderEntry)
{
    rsvpSenderEntry_t *data;

    if ((data = malloc(sizeof(rsvpSenderEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (rsvpSenderEntry), sizeof(rsvpSenderEntry_t));

    data->rsvpSenderSessionIndex = CloneOctetString(rsvpSenderEntry->rsvpSenderSessionIndex);

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
rsvpSenderEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    rsvpSenderEntry_t     *rsvpSenderEntry;
    OctetString *  rsvpSenderSessionIndex = NULL;
    int            rsvpSenderSessionIndex_offset;
    long           ifIndex;
    int            ifIndex_offset;
    int            final_index;
    int            I_ifIndex = 0;
   /*
    * Validate the object instance
    *
    */
    rsvpSenderSessionIndex_offset = object->oid.length;
    ifIndex_offset = rsvpSenderSessionIndex_offset + GetVariableIndexLength(incoming, rsvpSenderSessionIndex_offset);
    final_index = ifIndex_offset + 1;

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, rsvpSenderSessionIndex_offset, &rsvpSenderSessionIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(rsvpSenderSessionIndex);
        return error_status;
    }

    rsvpSenderEntry = k_rsvpSenderEntry_get(-1, contextInfo, -1, EXACT, rsvpSenderSessionIndex, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rsvpSenderEntry_set) &&
            (((rsvpSenderEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((rsvpSenderEntry_t *) (dp->data))->rsvpSenderSessionIndex, rsvpSenderSessionIndex) == 0) &&
            (((rsvpSenderEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rsvpSenderEntry_set;
        dp->cleanupMethod = rsvpSenderEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_rsvpSenderEntry_UNDO
        dp->undoMethod = rsvpSenderEntry_undo;
#else /* SR_rsvpSenderEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rsvpSenderEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (rsvpSenderEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rsvpSenderEntry(rsvpSenderEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_rsvpSenderEntry(rsvpSenderEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(rsvpSenderEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rsvpSenderEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((rsvpSenderEntry_t *) (dp->data))->rsvpSenderSessionIndex = CloneOctetString(rsvpSenderSessionIndex);
                SET_VALID(I_rsvpSenderSessionIndex, ((rsvpSenderEntry_t *) (dp->data))->valid);

                ((rsvpSenderEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((rsvpSenderEntry_t *) (dp->data))->valid);

                error_status = k_rsvpSenderEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(rsvpSenderSessionIndex);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_rsvpSenderInterval
   case I_rsvpSenderInterval:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpSenderEntry_t *) (dp->data))->rsvpSenderInterval = value->sl_value;
     break;
#endif /* I_rsvpSenderInterval */

#ifdef I_rsvpSenderStatus
   case I_rsvpSenderStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_rsvpSenderStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (rsvpSenderEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rsvpSenderStatus_notInService) || (value->sl_value == D_rsvpSenderStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rsvpSenderStatus_createAndGo) || (value->sl_value == D_rsvpSenderStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((rsvpSenderEntry_t *) (dp->data))->rsvpSenderStatus = value->sl_value;
     break;
#endif /* I_rsvpSenderStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rsvpSenderEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_rsvpSenderEntry_test */
   error_status = k_rsvpSenderEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rsvpSenderEntry_t *) (dp->data))->valid);
        error_status = k_rsvpSenderEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rsvpSenderEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_rsvpSenderEntry_set((rsvpSenderEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rsvpResvEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsvpResvEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    rsvpResvEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   rsvpResvSessionIndex = NULL;
    int             rsvpResvSessionIndex_offset;
    long            ifIndex;
    int             ifIndex_offset;
    int             index;
    int             final_index;

    rsvpResvSessionIndex_offset = object->oid.length;
    ifIndex_offset = rsvpResvSessionIndex_offset + GetVariableIndexLength(incoming, rsvpResvSessionIndex_offset);
    final_index = ifIndex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, rsvpResvSessionIndex_offset, &rsvpResvSessionIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsvpResvEntry_get(serialNum, contextInfo, arg ,searchType, rsvpResvSessionIndex, ifIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->rsvpResvSessionIndex->length;
        for(i = 0; i < data->rsvpResvSessionIndex->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->rsvpResvSessionIndex->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.length = index;
     }

        FreeOctetString(rsvpResvSessionIndex);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsvpResvService
    case I_rsvpResvService:
       dp = (void *) (&data->rsvpResvService);
       break;
#endif /* I_rsvpResvService */

#ifdef I_rsvpResvTSpecRate
    case I_rsvpResvTSpecRate:
       dp = (void *) (&data->rsvpResvTSpecRate);
       break;
#endif /* I_rsvpResvTSpecRate */

#ifdef I_rsvpResvTSpecPeakRate
    case I_rsvpResvTSpecPeakRate:
       dp = (void *) (&data->rsvpResvTSpecPeakRate);
       break;
#endif /* I_rsvpResvTSpecPeakRate */

#ifdef I_rsvpResvTSpecBurst
    case I_rsvpResvTSpecBurst:
       dp = (void *) (&data->rsvpResvTSpecBurst);
       break;
#endif /* I_rsvpResvTSpecBurst */

#ifdef I_rsvpResvTSpecLevel
    case I_rsvpResvTSpecLevel:
       dp = (void *) (&data->rsvpResvTSpecLevel);
       break;
#endif /* I_rsvpResvTSpecLevel */

#ifdef I_rsvpResvTSpecMinTU
    case I_rsvpResvTSpecMinTU:
       dp = (void *) (&data->rsvpResvTSpecMinTU);
       break;
#endif /* I_rsvpResvTSpecMinTU */

#ifdef I_rsvpResvTSpecMaxTU
    case I_rsvpResvTSpecMaxTU:
       dp = (void *) (&data->rsvpResvTSpecMaxTU);
       break;
#endif /* I_rsvpResvTSpecMaxTU */

#ifdef I_rsvpResvInterval
    case I_rsvpResvInterval:
       dp = (void *) (&data->rsvpResvInterval);
       break;
#endif /* I_rsvpResvInterval */

#ifdef I_rsvpResvScope
    case I_rsvpResvScope:
       dp = (void *) (CloneOctetString(data->rsvpResvScope));
       break;
#endif /* I_rsvpResvScope */

#ifdef I_rsvpResvShared
    case I_rsvpResvShared:
       dp = (void *) (&data->rsvpResvShared);
       break;
#endif /* I_rsvpResvShared */

#ifdef I_rsvpResvExplicit
    case I_rsvpResvExplicit:
       dp = (void *) (&data->rsvpResvExplicit);
       break;
#endif /* I_rsvpResvExplicit */

#ifdef I_rsvpResvRSVPHop
    case I_rsvpResvRSVPHop:
       dp = (void *) (&data->rsvpResvRSVPHop);
       break;
#endif /* I_rsvpResvRSVPHop */

#ifdef I_rsvpResvLastChange
    case I_rsvpResvLastChange:
       dp = (void *) (&data->rsvpResvLastChange);
       break;
#endif /* I_rsvpResvLastChange */

#ifdef I_rsvpResvStatus
    case I_rsvpResvStatus:
       dp = (void *) (&data->rsvpResvStatus);
       break;
#endif /* I_rsvpResvStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rsvpResvEntry data object.
 *---------------------------------------------------------------------*/
void
rsvpResvEntry_free(rsvpResvEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->rsvpResvSessionIndex);
       FreeOctetString(data->rsvpResvScope);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after rsvpResvEntry set/undo
 *---------------------------------------------------------------------*/
static int
rsvpResvEntry_cleanup(doList_t *trash)
{
   rsvpResvEntry_free(trash->data);
#ifdef SR_SNMPv2
   rsvpResvEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the rsvpResvEntry family
 *---------------------------------------------------------------------*/
rsvpResvEntry_t *
Clone_rsvpResvEntry(rsvpResvEntry_t *rsvpResvEntry)
{
    rsvpResvEntry_t *data;

    if ((data = malloc(sizeof(rsvpResvEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (rsvpResvEntry), sizeof(rsvpResvEntry_t));

    data->rsvpResvSessionIndex = CloneOctetString(rsvpResvEntry->rsvpResvSessionIndex);
    data->rsvpResvScope = CloneOctetString(rsvpResvEntry->rsvpResvScope);

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
rsvpResvEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    rsvpResvEntry_t     *rsvpResvEntry;
    OctetString *  rsvpResvSessionIndex = NULL;
    int            rsvpResvSessionIndex_offset;
    long           ifIndex;
    int            ifIndex_offset;
    int            final_index;
    int            I_ifIndex = 0;
   /*
    * Validate the object instance
    *
    */
    rsvpResvSessionIndex_offset = object->oid.length;
    ifIndex_offset = rsvpResvSessionIndex_offset + GetVariableIndexLength(incoming, rsvpResvSessionIndex_offset);
    final_index = ifIndex_offset + 1;

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, rsvpResvSessionIndex_offset, &rsvpResvSessionIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(rsvpResvSessionIndex);
        return error_status;
    }

    rsvpResvEntry = k_rsvpResvEntry_get(-1, contextInfo, -1, EXACT, rsvpResvSessionIndex, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rsvpResvEntry_set) &&
            (((rsvpResvEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((rsvpResvEntry_t *) (dp->data))->rsvpResvSessionIndex, rsvpResvSessionIndex) == 0) &&
            (((rsvpResvEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rsvpResvEntry_set;
        dp->cleanupMethod = rsvpResvEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_rsvpResvEntry_UNDO
        dp->undoMethod = rsvpResvEntry_undo;
#else /* SR_rsvpResvEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rsvpResvEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (rsvpResvEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rsvpResvEntry(rsvpResvEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_rsvpResvEntry(rsvpResvEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(rsvpResvEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rsvpResvEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((rsvpResvEntry_t *) (dp->data))->rsvpResvSessionIndex = CloneOctetString(rsvpResvSessionIndex);
                SET_VALID(I_rsvpResvSessionIndex, ((rsvpResvEntry_t *) (dp->data))->valid);

                ((rsvpResvEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((rsvpResvEntry_t *) (dp->data))->valid);

                error_status = k_rsvpResvEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(rsvpResvSessionIndex);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_rsvpResvService
   case I_rsvpResvService:

    switch (value->sl_value) {
        case 1:
        case 2:
        case 3:
        case 5:
            break;
        default:
            return WRONG_VALUE_ERROR;
    }

     ((rsvpResvEntry_t *) (dp->data))->rsvpResvService = value->sl_value;
     break;
#endif /* I_rsvpResvService */

#ifdef I_rsvpResvInterval
   case I_rsvpResvInterval:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpResvEntry_t *) (dp->data))->rsvpResvInterval = value->sl_value;
     break;
#endif /* I_rsvpResvInterval */

#ifdef I_rsvpResvScope
   case I_rsvpResvScope:

     if (((rsvpResvEntry_t *) (dp->data))->rsvpResvScope != NULL) {
        FreeOctetString(((rsvpResvEntry_t *) (dp->data))->rsvpResvScope);
     }

     ((rsvpResvEntry_t *) (dp->data))->rsvpResvScope = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rsvpResvScope */

#ifdef I_rsvpResvShared
   case I_rsvpResvShared:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpResvEntry_t *) (dp->data))->rsvpResvShared = value->sl_value;
     break;
#endif /* I_rsvpResvShared */

#ifdef I_rsvpResvExplicit
   case I_rsvpResvExplicit:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpResvEntry_t *) (dp->data))->rsvpResvExplicit = value->sl_value;
     break;
#endif /* I_rsvpResvExplicit */

#ifdef I_rsvpResvStatus
   case I_rsvpResvStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_rsvpResvStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (rsvpResvEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rsvpResvStatus_notInService) || (value->sl_value == D_rsvpResvStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rsvpResvStatus_createAndGo) || (value->sl_value == D_rsvpResvStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((rsvpResvEntry_t *) (dp->data))->rsvpResvStatus = value->sl_value;
     break;
#endif /* I_rsvpResvStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rsvpResvEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_rsvpResvEntry_test */
   error_status = k_rsvpResvEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rsvpResvEntry_t *) (dp->data))->valid);
        error_status = k_rsvpResvEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rsvpResvEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_rsvpResvEntry_set((rsvpResvEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rsvpResvFwdEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsvpResvFwdEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    rsvpResvFwdEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   rsvpResvFwdSessionIndex = NULL;
    int             rsvpResvFwdSessionIndex_offset;
    long            ifIndex;
    int             ifIndex_offset;
    int             index;
    int             final_index;

    rsvpResvFwdSessionIndex_offset = object->oid.length;
    ifIndex_offset = rsvpResvFwdSessionIndex_offset + GetVariableIndexLength(incoming, rsvpResvFwdSessionIndex_offset);
    final_index = ifIndex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, rsvpResvFwdSessionIndex_offset, &rsvpResvFwdSessionIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsvpResvFwdEntry_get(serialNum, contextInfo, arg ,searchType, rsvpResvFwdSessionIndex, ifIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->rsvpResvFwdSessionIndex->length;
        for(i = 0; i < data->rsvpResvFwdSessionIndex->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->rsvpResvFwdSessionIndex->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.length = index;
     }

        FreeOctetString(rsvpResvFwdSessionIndex);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsvpResvFwdService
    case I_rsvpResvFwdService:
       dp = (void *) (&data->rsvpResvFwdService);
       break;
#endif /* I_rsvpResvFwdService */

#ifdef I_rsvpResvFwdTSpecRate
    case I_rsvpResvFwdTSpecRate:
       dp = (void *) (&data->rsvpResvFwdTSpecRate);
       break;
#endif /* I_rsvpResvFwdTSpecRate */

#ifdef I_rsvpResvFwdTSpecPeakRate
    case I_rsvpResvFwdTSpecPeakRate:
       dp = (void *) (&data->rsvpResvFwdTSpecPeakRate);
       break;
#endif /* I_rsvpResvFwdTSpecPeakRate */

#ifdef I_rsvpResvFwdTSpecBurst
    case I_rsvpResvFwdTSpecBurst:
       dp = (void *) (&data->rsvpResvFwdTSpecBurst);
       break;
#endif /* I_rsvpResvFwdTSpecBurst */

#ifdef I_rsvpResvFwdTSpecLevel
    case I_rsvpResvFwdTSpecLevel:
       dp = (void *) (&data->rsvpResvFwdTSpecLevel);
       break;
#endif /* I_rsvpResvFwdTSpecLevel */

#ifdef I_rsvpResvFwdTSpecMinTU
    case I_rsvpResvFwdTSpecMinTU:
       dp = (void *) (&data->rsvpResvFwdTSpecMinTU);
       break;
#endif /* I_rsvpResvFwdTSpecMinTU */

#ifdef I_rsvpResvFwdTSpecMaxTU
    case I_rsvpResvFwdTSpecMaxTU:
       dp = (void *) (&data->rsvpResvFwdTSpecMaxTU);
       break;
#endif /* I_rsvpResvFwdTSpecMaxTU */

#ifdef I_rsvpResvFwdInterval
    case I_rsvpResvFwdInterval:
       dp = (void *) (&data->rsvpResvFwdInterval);
       break;
#endif /* I_rsvpResvFwdInterval */

#ifdef I_rsvpResvFwdScope
    case I_rsvpResvFwdScope:
       dp = (void *) (CloneOctetString(data->rsvpResvFwdScope));
       break;
#endif /* I_rsvpResvFwdScope */

#ifdef I_rsvpResvFwdShared
    case I_rsvpResvFwdShared:
       dp = (void *) (&data->rsvpResvFwdShared);
       break;
#endif /* I_rsvpResvFwdShared */

#ifdef I_rsvpResvFwdExplicit
    case I_rsvpResvFwdExplicit:
       dp = (void *) (&data->rsvpResvFwdExplicit);
       break;
#endif /* I_rsvpResvFwdExplicit */

#ifdef I_rsvpResvFwdRSVPHop
    case I_rsvpResvFwdRSVPHop:
       dp = (void *) (&data->rsvpResvFwdRSVPHop);
       break;
#endif /* I_rsvpResvFwdRSVPHop */

#ifdef I_rsvpResvFwdLastChange
    case I_rsvpResvFwdLastChange:
       dp = (void *) (&data->rsvpResvFwdLastChange);
       break;
#endif /* I_rsvpResvFwdLastChange */

#ifdef I_rsvpResvFwdStatus
    case I_rsvpResvFwdStatus:
       dp = (void *) (&data->rsvpResvFwdStatus);
       break;
#endif /* I_rsvpResvFwdStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rsvpResvFwdEntry data object.
 *---------------------------------------------------------------------*/
void
rsvpResvFwdEntry_free(rsvpResvFwdEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->rsvpResvFwdSessionIndex);
       FreeOctetString(data->rsvpResvFwdScope);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after rsvpResvFwdEntry set/undo
 *---------------------------------------------------------------------*/
static int
rsvpResvFwdEntry_cleanup(doList_t *trash)
{
   rsvpResvFwdEntry_free(trash->data);
#ifdef SR_SNMPv2
   rsvpResvFwdEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the rsvpResvFwdEntry family
 *---------------------------------------------------------------------*/
rsvpResvFwdEntry_t *
Clone_rsvpResvFwdEntry(rsvpResvFwdEntry_t *rsvpResvFwdEntry)
{
    rsvpResvFwdEntry_t *data;

    if ((data = malloc(sizeof(rsvpResvFwdEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (rsvpResvFwdEntry), sizeof(rsvpResvFwdEntry_t));

    data->rsvpResvFwdSessionIndex = CloneOctetString(rsvpResvFwdEntry->rsvpResvFwdSessionIndex);
    data->rsvpResvFwdScope = CloneOctetString(rsvpResvFwdEntry->rsvpResvFwdScope);

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
rsvpResvFwdEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    rsvpResvFwdEntry_t     *rsvpResvFwdEntry;
    OctetString *  rsvpResvFwdSessionIndex = NULL;
    int            rsvpResvFwdSessionIndex_offset;
    long           ifIndex;
    int            ifIndex_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    rsvpResvFwdSessionIndex_offset = object->oid.length;
    ifIndex_offset = rsvpResvFwdSessionIndex_offset + GetVariableIndexLength(incoming, rsvpResvFwdSessionIndex_offset);
    final_index = ifIndex_offset + 1;

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, rsvpResvFwdSessionIndex_offset, &rsvpResvFwdSessionIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(rsvpResvFwdSessionIndex);
        return error_status;
    }

    rsvpResvFwdEntry = k_rsvpResvFwdEntry_get(-1, contextInfo, -1, EXACT, rsvpResvFwdSessionIndex, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rsvpResvFwdEntry_set) &&
            (((rsvpResvFwdEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((rsvpResvFwdEntry_t *) (dp->data))->rsvpResvFwdSessionIndex, rsvpResvFwdSessionIndex) == 0) &&
            (((rsvpResvFwdEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rsvpResvFwdEntry_set;
        dp->cleanupMethod = rsvpResvFwdEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_rsvpResvFwdEntry_UNDO
        dp->undoMethod = rsvpResvFwdEntry_undo;
#else /* SR_rsvpResvFwdEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rsvpResvFwdEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (rsvpResvFwdEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rsvpResvFwdEntry(rsvpResvFwdEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_rsvpResvFwdEntry(rsvpResvFwdEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef rsvpResvFwdEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(rsvpResvFwdEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rsvpResvFwdEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((rsvpResvFwdEntry_t *) (dp->data))->rsvpResvFwdSessionIndex = CloneOctetString(rsvpResvFwdSessionIndex);
                SET_VALID(I_rsvpResvFwdSessionIndex, ((rsvpResvFwdEntry_t *) (dp->data))->valid);

                ((rsvpResvFwdEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((rsvpResvFwdEntry_t *) (dp->data))->valid);

                error_status = k_rsvpResvFwdEntry_set_defaults(dp);
            }
#else /* rsvpResvFwdEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* rsvpResvFwdEntry_READ_CREATE */
        }
    }

        FreeOctetString(rsvpResvFwdSessionIndex);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_rsvpResvFwdStatus
   case I_rsvpResvFwdStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_rsvpResvFwdStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (rsvpResvFwdEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rsvpResvFwdStatus_notInService) || (value->sl_value == D_rsvpResvFwdStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rsvpResvFwdStatus_createAndGo) || (value->sl_value == D_rsvpResvFwdStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((rsvpResvFwdEntry_t *) (dp->data))->rsvpResvFwdStatus = value->sl_value;
     break;
#endif /* I_rsvpResvFwdStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rsvpResvFwdEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_rsvpResvFwdEntry_test */
   error_status = k_rsvpResvFwdEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rsvpResvFwdEntry_t *) (dp->data))->valid);
        error_status = k_rsvpResvFwdEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rsvpResvFwdEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_rsvpResvFwdEntry_set((rsvpResvFwdEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rsvpIfAttribEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsvpIfAttribEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rsvpIfAttribEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ifIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsvpIfAttribEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
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

#ifdef I_rsvpIfAttribUdpNbrs
    case I_rsvpIfAttribUdpNbrs:
       dp = (void *) (&data->rsvpIfAttribUdpNbrs);
       break;
#endif /* I_rsvpIfAttribUdpNbrs */

#ifdef I_rsvpIfAttribIpNbrs
    case I_rsvpIfAttribIpNbrs:
       dp = (void *) (&data->rsvpIfAttribIpNbrs);
       break;
#endif /* I_rsvpIfAttribIpNbrs */

#ifdef I_rsvpIfAttribNbrs
    case I_rsvpIfAttribNbrs:
       dp = (void *) (&data->rsvpIfAttribNbrs);
       break;
#endif /* I_rsvpIfAttribNbrs */

#ifdef I_rsvpIfAttribEnabled
    case I_rsvpIfAttribEnabled:
       dp = (void *) (&data->rsvpIfAttribEnabled);
       break;
#endif /* I_rsvpIfAttribEnabled */

#ifdef I_rsvpIfAttribUdpRequired
    case I_rsvpIfAttribUdpRequired:
       dp = (void *) (&data->rsvpIfAttribUdpRequired);
       break;
#endif /* I_rsvpIfAttribUdpRequired */

#ifdef I_rsvpIfAttribStatus
    case I_rsvpIfAttribStatus:
       dp = (void *) (&data->rsvpIfAttribStatus);
       break;
#endif /* I_rsvpIfAttribStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rsvpIfAttribEntry data object.
 *---------------------------------------------------------------------*/
void
rsvpIfAttribEntry_free(rsvpIfAttribEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after rsvpIfAttribEntry set/undo
 *---------------------------------------------------------------------*/
static int
rsvpIfAttribEntry_cleanup(doList_t *trash)
{
   rsvpIfAttribEntry_free(trash->data);
#ifdef SR_SNMPv2
   rsvpIfAttribEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the rsvpIfAttribEntry family
 *---------------------------------------------------------------------*/
rsvpIfAttribEntry_t *
Clone_rsvpIfAttribEntry(rsvpIfAttribEntry_t *rsvpIfAttribEntry)
{
    rsvpIfAttribEntry_t *data;

    if ((data = malloc(sizeof(rsvpIfAttribEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (rsvpIfAttribEntry), sizeof(rsvpIfAttribEntry_t));


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
rsvpIfAttribEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    int            instLength = incoming->length - object->oid.length;
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    rsvpIfAttribEntry_t     *rsvpIfAttribEntry;
    long           ifIndex;
    long           I_ifIndex = 0;
   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    rsvpIfAttribEntry = k_rsvpIfAttribEntry_get(-1, contextInfo, -1, EXACT, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rsvpIfAttribEntry_set) &&
            (((rsvpIfAttribEntry_t *) (dp->data)) != NULL) &&
            (((rsvpIfAttribEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rsvpIfAttribEntry_set;
        dp->cleanupMethod = rsvpIfAttribEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_rsvpIfAttribEntry_UNDO
        dp->undoMethod = rsvpIfAttribEntry_undo;
#else /* SR_rsvpIfAttribEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rsvpIfAttribEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (rsvpIfAttribEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rsvpIfAttribEntry(rsvpIfAttribEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_rsvpIfAttribEntry(rsvpIfAttribEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(rsvpIfAttribEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rsvpIfAttribEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((rsvpIfAttribEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((rsvpIfAttribEntry_t *) (dp->data))->valid);

                error_status = k_rsvpIfAttribEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_rsvpIfAttribEnabled
   case I_rsvpIfAttribEnabled:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpIfAttribEntry_t *) (dp->data))->rsvpIfAttribEnabled = value->sl_value;
     break;
#endif /* I_rsvpIfAttribEnabled */

#ifdef I_rsvpIfAttribUdpRequired
   case I_rsvpIfAttribUdpRequired:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpIfAttribEntry_t *) (dp->data))->rsvpIfAttribUdpRequired = value->sl_value;
     break;
#endif /* I_rsvpIfAttribUdpRequired */

#ifdef I_rsvpIfAttribStatus
   case I_rsvpIfAttribStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_rsvpIfAttribStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (rsvpIfAttribEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rsvpIfAttribStatus_notInService) || (value->sl_value == D_rsvpIfAttribStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rsvpIfAttribStatus_createAndGo) || (value->sl_value == D_rsvpIfAttribStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((rsvpIfAttribEntry_t *) (dp->data))->rsvpIfAttribStatus = value->sl_value;
     break;
#endif /* I_rsvpIfAttribStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rsvpIfAttribEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_rsvpIfAttribEntry_test */
   error_status = k_rsvpIfAttribEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rsvpIfAttribEntry_t *) (dp->data))->valid);
        error_status = k_rsvpIfAttribEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rsvpIfAttribEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_rsvpIfAttribEntry_set((rsvpIfAttribEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rsvpNbrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsvpNbrEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    rsvpNbrEntry_t  *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            ifIndex;
    int             ifIndex_offset;
    OctetString *   rsvpNbrAddress = NULL;
    int             rsvpNbrAddress_offset;
    int             index;
    int             final_index;

    ifIndex_offset = object->oid.length;
    rsvpNbrAddress_offset = ifIndex_offset + 1;
    final_index = rsvpNbrAddress_offset + GetVariableIndexLength(incoming, rsvpNbrAddress_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, rsvpNbrAddress_offset, &rsvpNbrAddress, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsvpNbrEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, rsvpNbrAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.oid_ptr[index++] = data->rsvpNbrAddress->length;
        for(i = 0; i < data->rsvpNbrAddress->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->rsvpNbrAddress->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(rsvpNbrAddress);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsvpNbrProtocol
    case I_rsvpNbrProtocol:
       dp = (void *) (&data->rsvpNbrProtocol);
       break;
#endif /* I_rsvpNbrProtocol */

#ifdef I_rsvpNbrStatus
    case I_rsvpNbrStatus:
       dp = (void *) (&data->rsvpNbrStatus);
       break;
#endif /* I_rsvpNbrStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rsvpNbrEntry data object.
 *---------------------------------------------------------------------*/
void
rsvpNbrEntry_free(
   rsvpNbrEntry_t  *data)
{
   if (data != NULL) {
       FreeOctetString(data->rsvpNbrAddress);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after rsvpNbrEntry set/undo
 *---------------------------------------------------------------------*/
static int
rsvpNbrEntry_cleanup(
   doList_t *trash)
{
   rsvpNbrEntry_free(trash->data);
#ifdef SR_SNMPv2
   rsvpNbrEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the rsvpNbrEntry family
 *---------------------------------------------------------------------*/
rsvpNbrEntry_t *
Clone_rsvpNbrEntry(
    rsvpNbrEntry_t *rsvpNbrEntry)
{
    rsvpNbrEntry_t *data;

    if ((data = malloc(sizeof(rsvpNbrEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (rsvpNbrEntry), sizeof(rsvpNbrEntry_t));

    data->rsvpNbrAddress = CloneOctetString(rsvpNbrEntry->rsvpNbrAddress);

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
rsvpNbrEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    rsvpNbrEntry_t     *rsvpNbrEntry;
    long           ifIndex;
    int            ifIndex_offset;
    OctetString *  rsvpNbrAddress = NULL;
    int            rsvpNbrAddress_offset;
    int            final_index;
    int            I_ifIndex = 0;
   /*
    * Validate the object instance
    *
    */
    ifIndex_offset = object->oid.length;
    rsvpNbrAddress_offset = ifIndex_offset + 1;
    final_index = rsvpNbrAddress_offset + GetVariableIndexLength(incoming, rsvpNbrAddress_offset);

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToVariableOctetString(incoming, rsvpNbrAddress_offset, &rsvpNbrAddress, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(rsvpNbrAddress);
        return error_status;
    }

    rsvpNbrEntry = k_rsvpNbrEntry_get(-1, contextInfo, -1, EXACT, ifIndex, rsvpNbrAddress);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rsvpNbrEntry_set) &&
            (((rsvpNbrEntry_t *) (dp->data)) != NULL) &&
            (((rsvpNbrEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (CmpOctetStrings(((rsvpNbrEntry_t *) (dp->data))->rsvpNbrAddress, rsvpNbrAddress) == 0) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rsvpNbrEntry_set;
        dp->cleanupMethod = rsvpNbrEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_rsvpNbrEntry_UNDO
        dp->undoMethod = rsvpNbrEntry_undo;
#else /* SR_rsvpNbrEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rsvpNbrEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (rsvpNbrEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rsvpNbrEntry(rsvpNbrEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_rsvpNbrEntry(rsvpNbrEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(rsvpNbrEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rsvpNbrEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((rsvpNbrEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((rsvpNbrEntry_t *) (dp->data))->valid);

                ((rsvpNbrEntry_t *) (dp->data))->rsvpNbrAddress = CloneOctetString(rsvpNbrAddress);
                SET_VALID(I_rsvpNbrAddress, ((rsvpNbrEntry_t *) (dp->data))->valid);

                error_status = k_rsvpNbrEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(rsvpNbrAddress);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_rsvpNbrProtocol
   case I_rsvpNbrProtocol:

     if ((value->sl_value < 1) || (value->sl_value > 3)) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpNbrEntry_t *) (dp->data))->rsvpNbrProtocol = value->sl_value;
     break;
#endif /* I_rsvpNbrProtocol */

#ifdef I_rsvpNbrStatus
   case I_rsvpNbrStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_rsvpNbrStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (rsvpNbrEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rsvpNbrStatus_notInService) || (value->sl_value == D_rsvpNbrStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rsvpNbrStatus_createAndGo) || (value->sl_value == D_rsvpNbrStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((rsvpNbrEntry_t *) (dp->data))->rsvpNbrStatus = value->sl_value;
     break;
#endif /* I_rsvpNbrStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rsvpNbrEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_rsvpNbrEntry_test */
   error_status = k_rsvpNbrEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rsvpNbrEntry_t *) (dp->data))->valid);
        error_status = k_rsvpNbrEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rsvpNbrEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_rsvpNbrEntry_set((rsvpNbrEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rsvpFlowEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsvpFlowEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    rsvpFlowEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   rsvpFlowIndex = NULL;
    int             rsvpFlowIndex_offset;
    long            ifIndex;
    int             ifIndex_offset;
    int             index;
    int             final_index;

    rsvpFlowIndex_offset = object->oid.length;
    ifIndex_offset = rsvpFlowIndex_offset + GetVariableIndexLength(incoming, rsvpFlowIndex_offset);
    final_index = ifIndex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToVariableOctetString(incoming, rsvpFlowIndex_offset, &rsvpFlowIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsvpFlowEntry_get(serialNum, contextInfo, arg ,searchType, rsvpFlowIndex, ifIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = data->rsvpFlowIndex->length;
        for(i = 0; i < data->rsvpFlowIndex->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->rsvpFlowIndex->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.length = index;
     }

        FreeOctetString(rsvpFlowIndex);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsvpFlowRate
    case I_rsvpFlowRate:
       dp = (void *) (&data->rsvpFlowRate);
       break;
#endif /* I_rsvpFlowRate */

#ifdef I_rsvpFlowBurst
    case I_rsvpFlowBurst:
       dp = (void *) (&data->rsvpFlowBurst);
       break;
#endif /* I_rsvpFlowBurst */

#ifdef I_rsvpFlowWeight
    case I_rsvpFlowWeight:
       dp = (void *) (&data->rsvpFlowWeight);
       break;
#endif /* I_rsvpFlowWeight */

#ifdef I_rsvpFlowQueue
    case I_rsvpFlowQueue:
       dp = (void *) (&data->rsvpFlowQueue);
       break;
#endif /* I_rsvpFlowQueue */

#ifdef I_rsvpFlowMinTU
    case I_rsvpFlowMinTU:
       dp = (void *) (&data->rsvpFlowMinTU);
       break;
#endif /* I_rsvpFlowMinTU */

#ifdef I_rsvpFlowStatus
    case I_rsvpFlowStatus:
       dp = (void *) (&data->rsvpFlowStatus);
       break;
#endif /* I_rsvpFlowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rsvpFlowEntry data object.
 *---------------------------------------------------------------------*/
void
rsvpFlowEntry_free(
   rsvpFlowEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->rsvpFlowIndex);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after rsvpFlowEntry set/undo
 *---------------------------------------------------------------------*/
static int
rsvpFlowEntry_cleanup(
   doList_t *trash)
{
   rsvpFlowEntry_free(trash->data);
#ifdef SR_SNMPv2
   rsvpFlowEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the rsvpFlowEntry family
 *---------------------------------------------------------------------*/
rsvpFlowEntry_t *
Clone_rsvpFlowEntry(
    rsvpFlowEntry_t *rsvpFlowEntry)
{
    rsvpFlowEntry_t *data;

    if ((data = malloc(sizeof(rsvpFlowEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (rsvpFlowEntry), sizeof(rsvpFlowEntry_t));

    data->rsvpFlowIndex = CloneOctetString(rsvpFlowEntry->rsvpFlowIndex);

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
rsvpFlowEntry_test(incoming, object, value, doHead, doCur, contextInfo)
    OID            *incoming;
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
    doList_t       *dp;
    int            found;
    int            error_status = NO_ERROR;
    int            carry = 0;
    rsvpFlowEntry_t     *rsvpFlowEntry;
    OctetString *  rsvpFlowIndex = NULL;
    int            rsvpFlowIndex_offset;
    long           ifIndex;
    int            ifIndex_offset;
    int            final_index;
    int            I_ifIndex = 0;
   /*
    * Validate the object instance
    *
    */
    rsvpFlowIndex_offset = object->oid.length;
    ifIndex_offset = rsvpFlowIndex_offset + GetVariableIndexLength(incoming, rsvpFlowIndex_offset);
    final_index = ifIndex_offset + 1;

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToVariableOctetString(incoming, rsvpFlowIndex_offset, &rsvpFlowIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(rsvpFlowIndex);
        return error_status;
    }

    rsvpFlowEntry = k_rsvpFlowEntry_get(-1, contextInfo, -1, EXACT, rsvpFlowIndex, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rsvpFlowEntry_set) &&
            (((rsvpFlowEntry_t *) (dp->data)) != NULL) &&
            (CmpOctetStrings(((rsvpFlowEntry_t *) (dp->data))->rsvpFlowIndex, rsvpFlowIndex) == 0) &&
            (((rsvpFlowEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rsvpFlowEntry_set;
        dp->cleanupMethod = rsvpFlowEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_rsvpFlowEntry_UNDO
        dp->undoMethod = rsvpFlowEntry_undo;
#else /* SR_rsvpFlowEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rsvpFlowEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (rsvpFlowEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rsvpFlowEntry(rsvpFlowEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_rsvpFlowEntry(rsvpFlowEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(rsvpFlowEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rsvpFlowEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((rsvpFlowEntry_t *) (dp->data))->rsvpFlowIndex = CloneOctetString(rsvpFlowIndex);
                SET_VALID(I_rsvpFlowIndex, ((rsvpFlowEntry_t *) (dp->data))->valid);

                ((rsvpFlowEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((rsvpFlowEntry_t *) (dp->data))->valid);

                error_status = k_rsvpFlowEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(rsvpFlowIndex);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_rsvpFlowRate
   case I_rsvpFlowRate:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpFlowEntry_t *) (dp->data))->rsvpFlowRate = value->sl_value;
     break;
#endif /* I_rsvpFlowRate */

#ifdef I_rsvpFlowBurst
   case I_rsvpFlowBurst:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpFlowEntry_t *) (dp->data))->rsvpFlowBurst = value->sl_value;
     break;
#endif /* I_rsvpFlowBurst */

#ifdef I_rsvpFlowWeight
   case I_rsvpFlowWeight:

     ((rsvpFlowEntry_t *) (dp->data))->rsvpFlowWeight = value->sl_value;
     break;
#endif /* I_rsvpFlowWeight */

#ifdef I_rsvpFlowQueue
   case I_rsvpFlowQueue:

     ((rsvpFlowEntry_t *) (dp->data))->rsvpFlowQueue = value->sl_value;
     break;
#endif /* I_rsvpFlowQueue */

#ifdef I_rsvpFlowMinTU
   case I_rsvpFlowMinTU:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((rsvpFlowEntry_t *) (dp->data))->rsvpFlowMinTU = value->sl_value;
     break;
#endif /* I_rsvpFlowMinTU */

#ifdef I_rsvpFlowStatus
   case I_rsvpFlowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_rsvpFlowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (rsvpFlowEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rsvpFlowStatus_notInService) || (value->sl_value == D_rsvpFlowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rsvpFlowStatus_createAndGo) || (value->sl_value == D_rsvpFlowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((rsvpFlowEntry_t *) (dp->data))->rsvpFlowStatus = value->sl_value;
     break;
#endif /* I_rsvpFlowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rsvpFlowEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_rsvpFlowEntry_test */
   error_status = k_rsvpFlowEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rsvpFlowEntry_t *) (dp->data))->valid);
        error_status = k_rsvpFlowEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rsvpFlowEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_rsvpFlowEntry_set((rsvpFlowEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rsvpSessionEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rsvpSessionEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rsvpSessionEntry_t *data = NULL;
    unsigned long   buffer[6];
    OID             inst;
    int             carry;
    unsigned long   rsvpSessionAddress;
    long            rsvpSessionProtocol;
    long            rsvpSessionPort;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 6 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 6) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 5 + object->oid.length, &rsvpSessionPort, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rsvpSessionPort = MAX(0, rsvpSessionPort);
    }

    if ( (InstToInt(incoming, 4 + object->oid.length, &rsvpSessionProtocol, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rsvpSessionProtocol = MAX(1, rsvpSessionProtocol);
    }

    if ( (InstToIP(incoming, 0 + object->oid.length, &rsvpSessionAddress, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rsvpSessionEntry_get(serialNum, contextInfo, arg ,searchType, rsvpSessionAddress, rsvpSessionProtocol, rsvpSessionPort)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 6;
        inst.oid_ptr[0] = (data->rsvpSessionAddress >> 24) & 0xff;
        inst.oid_ptr[1] = (data->rsvpSessionAddress >> 16) & 0xff;
        inst.oid_ptr[2] = (data->rsvpSessionAddress >> 8) & 0xff;
        inst.oid_ptr[3] = (data->rsvpSessionAddress >> 0) & 0xff;
        inst.oid_ptr[4] = data->rsvpSessionProtocol;
        inst.oid_ptr[5] = data->rsvpSessionPort;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rsvpSessionSenders
    case I_rsvpSessionSenders:
       dp = (void *) (&data->rsvpSessionSenders);
       break;
#endif /* I_rsvpSessionSenders */

#ifdef I_rsvpSessionReceivers
    case I_rsvpSessionReceivers:
       dp = (void *) (&data->rsvpSessionReceivers);
       break;
#endif /* I_rsvpSessionReceivers */

#ifdef I_rsvpSessionRequests
    case I_rsvpSessionRequests:
       dp = (void *) (&data->rsvpSessionRequests);
       break;
#endif /* I_rsvpSessionRequests */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */





rsvpSenderEntry_t *
k_rsvpSenderEntry_get(serialNum, contextInfo, nominator, searchType, rsvpSenderSessionIndex, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * rsvpSenderSessionIndex;
   long ifIndex;
{
#ifdef NOT_YET
   static rsvpSenderEntry_t rsvpSenderEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpSenderEntryData.rsvpSenderSessionIndex = ;
   rsvpSenderEntryData.rsvpSenderTSpecRate = ;
   rsvpSenderEntryData.rsvpSenderTSpecPeakRate = ;
   rsvpSenderEntryData.rsvpSenderTSpecBurst = ;
   rsvpSenderEntryData.rsvpSenderTSpecMinTU = ;
   rsvpSenderEntryData.rsvpSenderTSpecMaxTU = ;
   rsvpSenderEntryData.rsvpSenderInterval = ;
   rsvpSenderEntryData.rsvpSenderRSVPHop = ;
   rsvpSenderEntryData.rsvpSenderLastChange = ;
   rsvpSenderEntryData.rsvpSenderStatus = ;
   rsvpSenderEntryData.ifIndex = ;
   return(&rsvpSenderEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpSenderEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_rsvpSenderEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpSenderEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
    rsvpSenderEntry_t *data = (rsvpSenderEntry_t *) (dp->data);
 */
    return NO_ERROR;
}

int
k_rsvpSenderEntry_set(data, contextInfo, function)
   rsvpSenderEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_rsvpSenderEntry_UNDO
/* add #define SR_rsvpSenderEntry_UNDO in rsvp_mib.h to
 * include the undo routine for the rsvpSenderEntry family.
 */
int
rsvpSenderEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpSenderEntry_UNDO */
#endif /* SR_SNMPv2 */

rsvpResvEntry_t *
k_rsvpResvEntry_get(serialNum, contextInfo, nominator, searchType, rsvpResvSessionIndex, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * rsvpResvSessionIndex;
   long ifIndex;
{
#ifdef NOT_YET
   static rsvpResvEntry_t rsvpResvEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpResvEntryData.rsvpResvSessionIndex = ;
   rsvpResvEntryData.rsvpResvService = ;
   rsvpResvEntryData.rsvpResvTSpecRate = ;
   rsvpResvEntryData.rsvpResvTSpecPeakRate = ;
   rsvpResvEntryData.rsvpResvTSpecBurst = ;
   rsvpResvEntryData.rsvpResvTSpecLevel = ;
   rsvpResvEntryData.rsvpResvTSpecMinTU = ;
   rsvpResvEntryData.rsvpResvTSpecMaxTU = ;
   rsvpResvEntryData.rsvpResvInterval = ;
   rsvpResvEntryData.rsvpResvScope = ;
   rsvpResvEntryData.rsvpResvShared = ;
   rsvpResvEntryData.rsvpResvExplicit = ;
   rsvpResvEntryData.rsvpResvRSVPHop = ;
   rsvpResvEntryData.rsvpResvLastChange = ;
   rsvpResvEntryData.rsvpResvStatus = ;
   rsvpResvEntryData.ifIndex = ;
   return(&rsvpResvEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpResvEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_rsvpResvEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpResvEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
   rsvpResvEntry_t *data = (rsvpResvEntry_t *) (dp->data);
 */
    return NO_ERROR;
}

int
k_rsvpResvEntry_set(data, contextInfo, function)
   rsvpResvEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_rsvpResvEntry_UNDO
/* add #define SR_rsvpResvEntry_UNDO in rsvp_mib.h to
 * include the undo routine for the rsvpResvEntry family.
 */
int
rsvpResvEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpResvEntry_UNDO */
#endif /* SR_SNMPv2 */

rsvpResvFwdEntry_t *
k_rsvpResvFwdEntry_get(serialNum, contextInfo, nominator, searchType, rsvpResvFwdSessionIndex, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * rsvpResvFwdSessionIndex;
   long ifIndex;
{
#ifdef NOT_YET
   static rsvpResvFwdEntry_t rsvpResvFwdEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpResvFwdEntryData.rsvpResvFwdSessionIndex = ;
   rsvpResvFwdEntryData.rsvpResvFwdService = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecRate = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecPeakRate = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecBurst = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecLevel = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecMinTU = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecMaxTU = ;
   rsvpResvFwdEntryData.rsvpResvFwdInterval = ;
   rsvpResvFwdEntryData.rsvpResvFwdScope = ;
   rsvpResvFwdEntryData.rsvpResvFwdShared = ;
   rsvpResvFwdEntryData.rsvpResvFwdExplicit = ;
   rsvpResvFwdEntryData.rsvpResvFwdRSVPHop = ;
   rsvpResvFwdEntryData.rsvpResvFwdLastChange = ;
   rsvpResvFwdEntryData.rsvpResvFwdStatus = ;
   rsvpResvFwdEntryData.ifIndex = ;
   return(&rsvpResvFwdEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpResvFwdEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_rsvpResvFwdEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpResvFwdEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
   rsvpResvFwdEntry_t *data = (rsvpResvFwdEntry_t *) (dp->data);
*/
    return NO_ERROR;
}

int
k_rsvpResvFwdEntry_set(data, contextInfo, function)
   rsvpResvFwdEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_rsvpResvFwdEntry_UNDO
/* add #define SR_rsvpResvFwdEntry_UNDO in rsvp_mib.h to
 * include the undo routine for the rsvpResvFwdEntry family.
 */
int
rsvpResvFwdEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpResvFwdEntry_UNDO */
#endif /* SR_SNMPv2 */

rsvpIfAttribEntry_t *
k_rsvpIfAttribEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
{
#ifdef NOT_YET
   static rsvpIfAttribEntry_t rsvpIfAttribEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpIfAttribEntryData.rsvpIfAttribUdpNbrs = ;
   rsvpIfAttribEntryData.rsvpIfAttribIpNbrs = ;
   rsvpIfAttribEntryData.rsvpIfAttribNbrs = ;
   rsvpIfAttribEntryData.rsvpIfAttribEnabled = ;
   rsvpIfAttribEntryData.rsvpIfAttribUdpRequired = ;
   rsvpIfAttribEntryData.rsvpIfAttribStatus = ;
   rsvpIfAttribEntryData.ifIndex = ;
   return(&rsvpIfAttribEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpIfAttribEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_rsvpIfAttribEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpIfAttribEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
    rsvpIfAttribEntry_t *data = (rsvpIfAttribEntry_t *) (dp->data);
 */
    return NO_ERROR;
}

int
k_rsvpIfAttribEntry_set(data, contextInfo, function)
   rsvpIfAttribEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_rsvpIfAttribEntry_UNDO
/* add #define SR_rsvpIfAttribEntry_UNDO in rsvp_mib.h to
 * include the undo routine for the rsvpIfAttribEntry family.
 */
int
rsvpIfAttribEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpIfAttribEntry_UNDO */
#endif /* SR_SNMPv2 */

rsvpNbrEntry_t *
k_rsvpNbrEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex, rsvpNbrAddress)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   OctetString * rsvpNbrAddress;
{
#ifdef NOT_YET
   static rsvpNbrEntry_t rsvpNbrEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpNbrEntryData.rsvpNbrAddress = ;
   rsvpNbrEntryData.rsvpNbrProtocol = ;
   rsvpNbrEntryData.rsvpNbrStatus = ;
   rsvpNbrEntryData.ifIndex = ;
   return(&rsvpNbrEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpNbrEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_rsvpNbrEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpNbrEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
    rsvpNbrEntry_t *data = (rsvpNbrEntry_t *) (dp->data);
 */
    return NO_ERROR;
}

int
k_rsvpNbrEntry_set(data, contextInfo, function)
   rsvpNbrEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_rsvpNbrEntry_UNDO
/* add #define SR_rsvpNbrEntry_UNDO in rsvp_mib.h to
 * include the undo routine for the rsvpNbrEntry family.
 */
int
rsvpNbrEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpNbrEntry_UNDO */
#endif /* SR_SNMPv2 */

rsvpFlowEntry_t *
k_rsvpFlowEntry_get(serialNum, contextInfo, nominator, searchType, rsvpFlowIndex, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   OctetString * rsvpFlowIndex;
   long ifIndex;
{
#ifdef NOT_YET
   static rsvpFlowEntry_t rsvpFlowEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpFlowEntryData.rsvpFlowIndex = ;
   rsvpFlowEntryData.rsvpFlowRate = ;
   rsvpFlowEntryData.rsvpFlowBurst = ;
   rsvpFlowEntryData.rsvpFlowWeight = ;
   rsvpFlowEntryData.rsvpFlowQueue = ;
   rsvpFlowEntryData.rsvpFlowMinTU = ;
   rsvpFlowEntryData.rsvpFlowStatus = ;
   rsvpFlowEntryData.ifIndex = ;
   return(&rsvpFlowEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpFlowEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_rsvpFlowEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpFlowEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
    rsvpFlowEntry_t *data = (rsvpFlowEntry_t *) (dp->data);
 */
    return NO_ERROR;
}

int
k_rsvpFlowEntry_set(data, contextInfo, function)
   rsvpFlowEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_rsvpFlowEntry_UNDO
/* add #define SR_rsvpFlowEntry_UNDO in rsvp_mib.h to
 * include the undo routine for the rsvpFlowEntry family.
 */
int
rsvpFlowEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpFlowEntry_UNDO */
#endif /* SR_SNMPv2 */

rsvpSessionEntry_t *
k_rsvpSessionEntry_get(serialNum, contextInfo, nominator, searchType, rsvpSessionAddress, rsvpSessionProtocol, rsvpSessionPort)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   unsigned long rsvpSessionAddress;
   long rsvpSessionProtocol;
   long rsvpSessionPort;
{
#ifdef NOT_YET
   static rsvpSessionEntry_t rsvpSessionEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpSessionEntryData.rsvpSessionAddress = ;
   rsvpSessionEntryData.rsvpSessionProtocol = ;
   rsvpSessionEntryData.rsvpSessionPort = ;
   rsvpSessionEntryData.rsvpSessionSenders = ;
   rsvpSessionEntryData.rsvpSessionReceivers = ;
   rsvpSessionEntryData.rsvpSessionRequests = ;
   return(&rsvpSessionEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}



static void
init_rsvp_mib (subsystype *subsys)
{
    load_mib(rsvp_mib_OidList, rsvp_mib_OidListNum);
    load_oid(rsvp_mib_oid_table);
}

/*
 * rsvp_mib subsystem header
 */

#define MAJVERSION_rsvp_mib 1
#define MINVERSION_rsvp_mib 0
#define EDITVERSION_rsvp_mib 0

SUBSYS_HEADER(rsvp_mib,
              MAJVERSION_rsvp_mib,
              MINVERSION_rsvp_mib,
              EDITVERSION_rsvp_mib,
              init_rsvp_mib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
