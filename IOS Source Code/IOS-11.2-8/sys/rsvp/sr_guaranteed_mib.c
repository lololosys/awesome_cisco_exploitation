/* $Id: sr_guaranteed_mib.c,v 1.1.50.2 1996/07/01 18:45:48 hampton Exp $
 * $Source: /release/112/cvs/Xsys/rsvp/sr_guaranteed_mib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1996, Fred Baker (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_guaranteed_mib.c,v $
 * Revision 1.1.50.2  1996/07/01  18:45:48  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.50.1  1996/04/18  10:48:55  fred
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
#include "sr_guaranteed_mib.h"
#include "sr_guaranteed_mib-mib.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the intSrvGuaranteedIfEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
intSrvGuaranteedIfEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    intSrvGuaranteedIfEntry_t *data = NULL;
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
     if ( (arg == -1) || (data = k_intSrvGuaranteedIfEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
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

#ifdef I_intSrvGuaranteedIfC
    case I_intSrvGuaranteedIfC:
       dp = (void *) (&data->intSrvGuaranteedIfC);
       break;
#endif /* I_intSrvGuaranteedIfC */

#ifdef I_intSrvGuaranteedIfD
    case I_intSrvGuaranteedIfD:
       dp = (void *) (&data->intSrvGuaranteedIfD);
       break;
#endif /* I_intSrvGuaranteedIfD */

#ifdef I_intSrvGuaranteedIfSlack
    case I_intSrvGuaranteedIfSlack:
       dp = (void *) (&data->intSrvGuaranteedIfSlack);
       break;
#endif /* I_intSrvGuaranteedIfSlack */

#ifdef I_intSrvGuaranteedIfStatus
    case I_intSrvGuaranteedIfStatus:
       dp = (void *) (&data->intSrvGuaranteedIfStatus);
       break;
#endif /* I_intSrvGuaranteedIfStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the intSrvGuaranteedIfEntry data object.
 *---------------------------------------------------------------------*/
void
intSrvGuaranteedIfEntry_free(intSrvGuaranteedIfEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after intSrvGuaranteedIfEntry set/undo
 *---------------------------------------------------------------------*/
static int
intSrvGuaranteedIfEntry_cleanup(doList_t *trash)
{
   intSrvGuaranteedIfEntry_free(trash->data);
#ifdef SR_SNMPv2
   intSrvGuaranteedIfEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the intSrvGuaranteedIfEntry family
 *---------------------------------------------------------------------*/
intSrvGuaranteedIfEntry_t *
Clone_intSrvGuaranteedIfEntry(
			      intSrvGuaranteedIfEntry_t *intSrvGuaranteedIfEntry)
{
    intSrvGuaranteedIfEntry_t *data;

    if ((data = malloc(sizeof(intSrvGuaranteedIfEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (intSrvGuaranteedIfEntry), sizeof(intSrvGuaranteedIfEntry_t));


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
intSrvGuaranteedIfEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    intSrvGuaranteedIfEntry_t     *intSrvGuaranteedIfEntry;
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

    intSrvGuaranteedIfEntry = k_intSrvGuaranteedIfEntry_get(-1, contextInfo, -1, EXACT, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == intSrvGuaranteedIfEntry_set) &&
            (((intSrvGuaranteedIfEntry_t *) (dp->data)) != NULL) &&
            (((intSrvGuaranteedIfEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = intSrvGuaranteedIfEntry_set;
        dp->cleanupMethod = intSrvGuaranteedIfEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_intSrvGuaranteedIfEntry_UNDO
        dp->undoMethod = intSrvGuaranteedIfEntry_undo;
#else /* SR_intSrvGuaranteedIfEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_intSrvGuaranteedIfEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (intSrvGuaranteedIfEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_intSrvGuaranteedIfEntry(intSrvGuaranteedIfEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_intSrvGuaranteedIfEntry(intSrvGuaranteedIfEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(intSrvGuaranteedIfEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(intSrvGuaranteedIfEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((intSrvGuaranteedIfEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((intSrvGuaranteedIfEntry_t *) (dp->data))->valid);

                error_status = k_intSrvGuaranteedIfEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_intSrvGuaranteedIfC
   case I_intSrvGuaranteedIfC:

     if ( ((value->sl_value < 0) || (value->sl_value > 268435455)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((intSrvGuaranteedIfEntry_t *) (dp->data))->intSrvGuaranteedIfC = value->sl_value;
     break;
#endif /* I_intSrvGuaranteedIfC */

#ifdef I_intSrvGuaranteedIfD
   case I_intSrvGuaranteedIfD:

     if ( ((value->sl_value < 0) || (value->sl_value > 268435455)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((intSrvGuaranteedIfEntry_t *) (dp->data))->intSrvGuaranteedIfD = value->sl_value;
     break;
#endif /* I_intSrvGuaranteedIfD */

#ifdef I_intSrvGuaranteedIfSlack
   case I_intSrvGuaranteedIfSlack:

     if ( ((value->sl_value < 0) || (value->sl_value > 268435455)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((intSrvGuaranteedIfEntry_t *) (dp->data))->intSrvGuaranteedIfSlack = value->sl_value;
     break;
#endif /* I_intSrvGuaranteedIfSlack */

#ifdef I_intSrvGuaranteedIfStatus
   case I_intSrvGuaranteedIfStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_intSrvGuaranteedIfStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (intSrvGuaranteedIfEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_intSrvGuaranteedIfStatus_notInService) || (value->sl_value == D_intSrvGuaranteedIfStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_intSrvGuaranteedIfStatus_createAndGo) || (value->sl_value == D_intSrvGuaranteedIfStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((intSrvGuaranteedIfEntry_t *) (dp->data))->intSrvGuaranteedIfStatus = value->sl_value;
     break;
#endif /* I_intSrvGuaranteedIfStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in intSrvGuaranteedIfEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_intSrvGuaranteedIfEntry_test */
   error_status = k_intSrvGuaranteedIfEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((intSrvGuaranteedIfEntry_t *) (dp->data))->valid);
        error_status = k_intSrvGuaranteedIfEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
intSrvGuaranteedIfEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_intSrvGuaranteedIfEntry_set((intSrvGuaranteedIfEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */





intSrvGuaranteedIfEntry_t *
k_intSrvGuaranteedIfEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
{
#ifdef NOT_YET
   static intSrvGuaranteedIfEntry_t intSrvGuaranteedIfEntryData;

   /*
    * put your code to retrieve the information here
    */

   intSrvGuaranteedIfEntryData.intSrvGuaranteedIfC = ;
   intSrvGuaranteedIfEntryData.intSrvGuaranteedIfD = ;
   intSrvGuaranteedIfEntryData.intSrvGuaranteedIfSlack = ;
   intSrvGuaranteedIfEntryData.intSrvGuaranteedIfStatus = ;
   intSrvGuaranteedIfEntryData.ifIndex = ;
   return(&intSrvGuaranteedIfEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_intSrvGuaranteedIfEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_intSrvGuaranteedIfEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_intSrvGuaranteedIfEntry_set_defaults(dp)
    doList_t       *dp;
{
/*
    intSrvGuaranteedIfEntry_t *data = (intSrvGuaranteedIfEntry_t *) (dp->data);
 */
    return NO_ERROR;
}

int
k_intSrvGuaranteedIfEntry_set(data, contextInfo, function)
   intSrvGuaranteedIfEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_intSrvGuaranteedIfEntry_UNDO
/* add #define SR_intSrvGuaranteedIfEntry_UNDO in guaranteed_mib.h to
 * include the undo routine for the intSrvGuaranteedIfEntry family.
 */
int
intSrvGuaranteedIfEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_intSrvGuaranteedIfEntry_UNDO */
#endif /* SR_SNMPv2 */



static void
init_guaranteed_mib (subsystype *subsys)
{
    load_mib(guaranteed_mib_OidList, guaranteed_mib_OidListNum);
    load_oid(guaranteed_mib_oid_table);
}

/*
 * guaranteed_mib subsystem header
 */

#define MAJVERSION_guaranteed_mib 1
#define MINVERSION_guaranteed_mib 0
#define EDITVERSION_guaranteed_mib 0

SUBSYS_HEADER(guaranteed_mib,
              MAJVERSION_guaranteed_mib,
              MINVERSION_guaranteed_mib,
              EDITVERSION_guaranteed_mib,
              init_guaranteed_mib,
              SUBSYS_CLASS_MANAGEMENT,
              "req:rsvp",
              NULL);
      
