/* $Id: sr_int_serv_mib.c,v 1.1.2.3 1996/07/01 18:45:49 hampton Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/sr_int_serv_mib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_int_serv_mib.c,v $
 * Revision 1.1.2.3  1996/07/01  18:45:49  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.2.2  1996/04/18  10:48:57  fred
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
#include "sr_int_serv_mib.h"
#include "sr_int_serv_mib-mib.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the intSrvIfAttribEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
intSrvIfAttribEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    intSrvIfAttribEntry_t *data = NULL;
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
     if ( (arg == -1) || (data = k_intSrvIfAttribEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
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

#ifdef I_intSrvIfAttribAllocatedBits
    case I_intSrvIfAttribAllocatedBits:
       dp = (void *) (&data->intSrvIfAttribAllocatedBits);
       break;
#endif /* I_intSrvIfAttribAllocatedBits */

#ifdef I_intSrvIfAttribMaxAllocatedBits
    case I_intSrvIfAttribMaxAllocatedBits:
       dp = (void *) (&data->intSrvIfAttribMaxAllocatedBits);
       break;
#endif /* I_intSrvIfAttribMaxAllocatedBits */

#ifdef I_intSrvIfAttribAllocatedBuffer
    case I_intSrvIfAttribAllocatedBuffer:
       dp = (void *) (&data->intSrvIfAttribAllocatedBuffer);
       break;
#endif /* I_intSrvIfAttribAllocatedBuffer */

#ifdef I_intSrvIfAttribFlows
    case I_intSrvIfAttribFlows:
       dp = (void *) (&data->intSrvIfAttribFlows);
       break;
#endif /* I_intSrvIfAttribFlows */

#ifdef I_intSrvIfAttribPropagationDelay
    case I_intSrvIfAttribPropagationDelay:
       dp = (void *) (&data->intSrvIfAttribPropagationDelay);
       break;
#endif /* I_intSrvIfAttribPropagationDelay */

#ifdef I_intSrvIfAttribStatus
    case I_intSrvIfAttribStatus:
       dp = (void *) (&data->intSrvIfAttribStatus);
       break;
#endif /* I_intSrvIfAttribStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the intSrvIfAttribEntry data object.
 *---------------------------------------------------------------------*/
void
intSrvIfAttribEntry_free(intSrvIfAttribEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after intSrvIfAttribEntry set/undo
 *---------------------------------------------------------------------*/
static int
intSrvIfAttribEntry_cleanup(doList_t *trash)
{
   intSrvIfAttribEntry_free(trash->data);
#ifdef SR_SNMPv2
   intSrvIfAttribEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the intSrvIfAttribEntry family
 *---------------------------------------------------------------------*/
intSrvIfAttribEntry_t *
Clone_intSrvIfAttribEntry(intSrvIfAttribEntry_t *intSrvIfAttribEntry)
{
    intSrvIfAttribEntry_t *data;

    if ((data = malloc(sizeof(intSrvIfAttribEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (intSrvIfAttribEntry), sizeof(intSrvIfAttribEntry_t));


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
intSrvIfAttribEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    intSrvIfAttribEntry_t     *intSrvIfAttribEntry;
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

    intSrvIfAttribEntry = k_intSrvIfAttribEntry_get(-1, contextInfo, -1, EXACT, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == intSrvIfAttribEntry_set) &&
            (((intSrvIfAttribEntry_t *) (dp->data)) != NULL) &&
            (((intSrvIfAttribEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = intSrvIfAttribEntry_set;
        dp->cleanupMethod = intSrvIfAttribEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_intSrvIfAttribEntry_UNDO
        dp->undoMethod = intSrvIfAttribEntry_undo;
#else /* SR_intSrvIfAttribEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_intSrvIfAttribEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (intSrvIfAttribEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_intSrvIfAttribEntry(intSrvIfAttribEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_intSrvIfAttribEntry(intSrvIfAttribEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(intSrvIfAttribEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(intSrvIfAttribEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((intSrvIfAttribEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((intSrvIfAttribEntry_t *) (dp->data))->valid);

                error_status = k_intSrvIfAttribEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_intSrvIfAttribMaxAllocatedBits
   case I_intSrvIfAttribMaxAllocatedBits:

     if ( ((value->sl_value < 0) || (value->sl_value > 2147483647)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((intSrvIfAttribEntry_t *) (dp->data))->intSrvIfAttribMaxAllocatedBits = value->sl_value;
     break;
#endif /* I_intSrvIfAttribMaxAllocatedBits */

#ifdef I_intSrvIfAttribPropagationDelay
   case I_intSrvIfAttribPropagationDelay:

     ((intSrvIfAttribEntry_t *) (dp->data))->intSrvIfAttribPropagationDelay = value->sl_value;
     break;
#endif /* I_intSrvIfAttribPropagationDelay */

#ifdef I_intSrvIfAttribStatus
   case I_intSrvIfAttribStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_intSrvIfAttribStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (intSrvIfAttribEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_intSrvIfAttribStatus_notInService) || (value->sl_value == D_intSrvIfAttribStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_intSrvIfAttribStatus_createAndGo) || (value->sl_value == D_intSrvIfAttribStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((intSrvIfAttribEntry_t *) (dp->data))->intSrvIfAttribStatus = value->sl_value;
     break;
#endif /* I_intSrvIfAttribStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in intSrvIfAttribEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_intSrvIfAttribEntry_test */
   error_status = k_intSrvIfAttribEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((intSrvIfAttribEntry_t *) (dp->data))->valid);
        error_status = k_intSrvIfAttribEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
intSrvIfAttribEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_intSrvIfAttribEntry_set((intSrvIfAttribEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */





intSrvIfAttribEntry_t *
k_intSrvIfAttribEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
{
#ifdef NOT_YET
   static intSrvIfAttribEntry_t intSrvIfAttribEntryData;

   /*
    * put your code to retrieve the information here
    */

   intSrvIfAttribEntryData.intSrvIfAttribAllocatedBits = ;
   intSrvIfAttribEntryData.intSrvIfAttribMaxAllocatedBits = ;
   intSrvIfAttribEntryData.intSrvIfAttribAllocatedBuffer = ;
   intSrvIfAttribEntryData.intSrvIfAttribFlows = ;
   intSrvIfAttribEntryData.intSrvIfAttribPropagationDelay = ;
   intSrvIfAttribEntryData.intSrvIfAttribStatus = ;
   intSrvIfAttribEntryData.ifIndex = ;
   return(&intSrvIfAttribEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_intSrvIfAttribEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_intSrvIfAttribEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_intSrvIfAttribEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
    intSrvIfAttribEntry_t *data = (intSrvIfAttribEntry_t *) (dp->data);
 */
    return NO_ERROR;
}

int
k_intSrvIfAttribEntry_set(data, contextInfo, function)
   intSrvIfAttribEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_intSrvIfAttribEntry_UNDO
/* add #define SR_intSrvIfAttribEntry_UNDO in int_serv_mib.h to
 * include the undo routine for the intSrvIfAttribEntry family.
 */
int
intSrvIfAttribEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_intSrvIfAttribEntry_UNDO */
#endif /* SR_SNMPv2 */



static void
init_int_serv_mib (subsystype *subsys)
{
    load_mib(int_serv_mib_OidList, int_serv_mib_OidListNum);
    load_oid(int_serv_mib_oid_table);
}

/*
 * int_serv_mib subsystem header
 */

#define MAJVERSION_int_serv_mib 1
#define MINVERSION_int_serv_mib 0
#define EDITVERSION_int_serv_mib 0

SUBSYS_HEADER(int_serv_mib,
              MAJVERSION_int_serv_mib,
              MINVERSION_int_serv_mib,
              EDITVERSION_int_serv_mib,
              init_int_serv_mib,
              SUBSYS_CLASS_MANAGEMENT,
              "req:rsvp",
              NULL);

      
