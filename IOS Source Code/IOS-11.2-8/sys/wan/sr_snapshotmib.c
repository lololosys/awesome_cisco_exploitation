/* $Id: sr_snapshotmib.c,v 3.8.8.1 1996/07/01 18:46:50 hampton Exp $
 * $Source: /release/112/cvs/Xsys/wan/sr_snapshotmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * September 1994, Joel P. Bion (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snapshotmib.c,v $
 * Revision 3.8.8.1  1996/07/01  18:46:50  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.8  1996/02/29  16:47:29  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.7  1996/01/24  21:19:37  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.6  1996/01/22  07:46:07  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1996/01/18  15:55:53  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.4  1995/12/15  19:39:42  bdas
 * CSCdi44320:  restarted by bus error at PC 0x22146CC
 * [_k_ciscoSnapshotActivityEntry_get
 *
 * Revision 3.3  1995/11/17  18:05:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:51:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:33:07  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  23:20:19  hampton
 * Bump version numbers from 1.x to 2.x.
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
#include "sys_registry.h"
#include "../snmp/snmp_api.h"
#include "subsys.h"
#include "../h/linktype.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "../h/bitlogic.h"
#include "../if/network.h"
#include "snapshot.h"
#include "snapshot_private.h"
#include "sr_snapshotmib.h"
#include "sr_snapshotmib-mib.h"
#include "sr_snapshotmib_externs.h"


#define DEF_ciscoSnapshotClient           TRUE
#define DEF_ciscoSnapshotDialer           FALSE

#define MIN_ciscoSnapshotActiveInterval   5
#define DEF_ciscoSnapshotActiveInterval   5
#define MAX_ciscoSnapshotActiveInterval   1000

#define MIN_ciscoSnapshotQuietInterval    8
#define DEF_ciscoSnapshotQuietInterval    8
#define MAX_ciscoSnapshotQuietInterval    100000

#define DEF_ciscoSnapshotIfUpAction       1


/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the ciscoSnapshotMIBObjects family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoSnapshotMIBObjects_get (OID             *incoming,
			     ObjectInfo      *object,
			     int             searchType,
			     ContextInfo     *contextInfo,
			     int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ciscoSnapshotMIBObjects_t *data = NULL;


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
     if ( (arg == -1) || (data = k_ciscoSnapshotMIBObjects_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoSnapshotForceActive
    case I_ciscoSnapshotForceActive:
       dp = (void *) (&data->ciscoSnapshotForceActive);
       break;
#endif /* I_ciscoSnapshotForceActive */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the ciscoSnapshotMIBObjects data object.
 *---------------------------------------------------------------------*/
void
ciscoSnapshotMIBObjects_free (ciscoSnapshotMIBObjects_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after ciscoSnapshotMIBObjects set/undo
 *---------------------------------------------------------------------*/
static int
ciscoSnapshotMIBObjects_cleanup (doList_t *trash)
{
   ciscoSnapshotMIBObjects_free(trash->data);
#ifdef SR_SNMPv2
   ciscoSnapshotMIBObjects_free(trash->undodata);
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
ciscoSnapshotMIBObjects_test (OID            *incoming,
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
         if ( (dp->setMethod == ciscoSnapshotMIBObjects_set) &&
            (((ciscoSnapshotMIBObjects_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(ciscoSnapshotMIBObjects_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(ciscoSnapshotMIBObjects_t));
#ifdef SR_SNMPv2
        if ( (dp->undodata = malloc(sizeof(ciscoSnapshotMIBObjects_t))) == NULL) { 
            DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->undodata, 0, sizeof(ciscoSnapshotMIBObjects_t));
#endif /* SR_SNMPv2 */

        dp->setMethod = ciscoSnapshotMIBObjects_set;
        dp->cleanupMethod = ciscoSnapshotMIBObjects_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_ciscoSnapshotMIBObjects_UNDO
        dp->undoMethod = ciscoSnapshotMIBObjects_undo;
#else /* SR_ciscoSnapshotMIBObjects_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_ciscoSnapshotMIBObjects_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_ciscoSnapshotForceActive
   case I_ciscoSnapshotForceActive:

     ((ciscoSnapshotMIBObjects_t *) (dp->data))->ciscoSnapshotForceActive = value->sl_value;
     break;
#endif /* I_ciscoSnapshotForceActive */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ciscoSnapshotMIBObjects_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_ciscoSnapshotMIBObjects_test */
   error_status = k_ciscoSnapshotMIBObjects_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((ciscoSnapshotMIBObjects_t *) (dp->data))->valid);
        error_status = k_ciscoSnapshotMIBObjects_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ciscoSnapshotMIBObjects_set (doList_t       *doHead,
			     doList_t       *doCur,
			     ContextInfo    *contextInfo)
{
  return (k_ciscoSnapshotMIBObjects_set((ciscoSnapshotMIBObjects_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ciscoSnapshotInterfaceEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoSnapshotInterfaceEntry_get (OID             *incoming,
				 ObjectInfo      *object,
				 int             searchType,
				 ContextInfo     *contextInfo,
				 int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    ciscoSnapshotInterfaceEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ciscoSnapshotIfIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ciscoSnapshotIfIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoSnapshotInterfaceEntry_get(serialNum, contextInfo, arg ,searchType, ciscoSnapshotIfIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ciscoSnapshotIfIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoSnapshotClient
    case I_ciscoSnapshotClient:
       dp = (void *) (&data->ciscoSnapshotClient);
       break;
#endif /* I_ciscoSnapshotClient */

#ifdef I_ciscoSnapshotDialer
    case I_ciscoSnapshotDialer:
       dp = (void *) (&data->ciscoSnapshotDialer);
       break;
#endif /* I_ciscoSnapshotDialer */

#ifdef I_ciscoSnapshotActiveInterval
    case I_ciscoSnapshotActiveInterval:
       dp = (void *) (&data->ciscoSnapshotActiveInterval);
       break;
#endif /* I_ciscoSnapshotActiveInterval */

#ifdef I_ciscoSnapshotQuietInterval
    case I_ciscoSnapshotQuietInterval:
       dp = (void *) (&data->ciscoSnapshotQuietInterval);
       break;
#endif /* I_ciscoSnapshotQuietInterval */

#ifdef I_ciscoSnapshotRetryInterval
    case I_ciscoSnapshotRetryInterval:
       dp = (void *) (&data->ciscoSnapshotRetryInterval);
       break;
#endif /* I_ciscoSnapshotRetryInterval */

#ifdef I_ciscoSnapshotIfUpAction
    case I_ciscoSnapshotIfUpAction:
       dp = (void *) (&data->ciscoSnapshotIfUpAction);
       break;
#endif /* I_ciscoSnapshotIfUpAction */

#ifdef I_ciscoSnapshotRowStatus
    case I_ciscoSnapshotRowStatus:
       dp = (void *) (&data->ciscoSnapshotRowStatus);
       break;
#endif /* I_ciscoSnapshotRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the ciscoSnapshotInterfaceEntry data object.
 *---------------------------------------------------------------------*/
void
ciscoSnapshotInterfaceEntry_free (ciscoSnapshotInterfaceEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after ciscoSnapshotInterfaceEntry set/undo
 *---------------------------------------------------------------------*/
static int
ciscoSnapshotInterfaceEntry_cleanup (doList_t *trash)
{
   ciscoSnapshotInterfaceEntry_free(trash->data);
#ifdef SR_SNMPv2
   ciscoSnapshotInterfaceEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the ciscoSnapshotInterfaceEntry family
 *---------------------------------------------------------------------*/
ciscoSnapshotInterfaceEntry_t *
Clone_ciscoSnapshotInterfaceEntry (
		  ciscoSnapshotInterfaceEntry_t *ciscoSnapshotInterfaceEntry)
{
    ciscoSnapshotInterfaceEntry_t *data;

    if ((data = malloc(sizeof(ciscoSnapshotInterfaceEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (ciscoSnapshotInterfaceEntry), sizeof(ciscoSnapshotInterfaceEntry_t));


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
ciscoSnapshotInterfaceEntry_test (OID            *incoming,
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
    ciscoSnapshotInterfaceEntry_t     *ciscoSnapshotInterfaceEntry;
    long           ciscoSnapshotIfIndex;
    boolean	    entry_found = FALSE;
    int		    retval;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ciscoSnapshotIfIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    ciscoSnapshotInterfaceEntry = k_ciscoSnapshotInterfaceEntry_get(-1, contextInfo, -1, EXACT, ciscoSnapshotIfIndex);

    entry_found = (ciscoSnapshotInterfaceEntry != NULL);
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == ciscoSnapshotInterfaceEntry_set) &&
            (((ciscoSnapshotInterfaceEntry_t *) (dp->data)) != NULL) &&
            (((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotIfIndex == ciscoSnapshotIfIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = ciscoSnapshotInterfaceEntry_set;
        dp->cleanupMethod = ciscoSnapshotInterfaceEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_ciscoSnapshotInterfaceEntry_UNDO
        dp->undoMethod = ciscoSnapshotInterfaceEntry_undo;
#else /* SR_ciscoSnapshotInterfaceEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_ciscoSnapshotInterfaceEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (ciscoSnapshotInterfaceEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_ciscoSnapshotInterfaceEntry(ciscoSnapshotInterfaceEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_ciscoSnapshotInterfaceEntry(ciscoSnapshotInterfaceEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(ciscoSnapshotInterfaceEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(ciscoSnapshotInterfaceEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotIfIndex = ciscoSnapshotIfIndex;
                error_status = k_ciscoSnapshotInterfaceEntry_set_defaults(dp);
                SET_VALID(I_ciscoSnapshotIfIndex, ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_ciscoSnapshotClient
   case I_ciscoSnapshotClient:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotClient = value->sl_value;
     break;
#endif /* I_ciscoSnapshotClient */

#ifdef I_ciscoSnapshotDialer
   case I_ciscoSnapshotDialer:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotDialer = value->sl_value;
     break;
#endif /* I_ciscoSnapshotDialer */

#ifdef I_ciscoSnapshotActiveInterval
   case I_ciscoSnapshotActiveInterval:

     ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotActiveInterval = value->sl_value;
     break;
#endif /* I_ciscoSnapshotActiveInterval */

#ifdef I_ciscoSnapshotQuietInterval
   case I_ciscoSnapshotQuietInterval:

     ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotQuietInterval = value->sl_value;
     break;
#endif /* I_ciscoSnapshotQuietInterval */

#ifdef I_ciscoSnapshotIfUpAction
   case I_ciscoSnapshotIfUpAction:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotIfUpAction = value->sl_value;
     break;
#endif /* I_ciscoSnapshotIfUpAction */

#ifdef I_ciscoSnapshotRowStatus
   case I_ciscoSnapshotRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_ciscoSnapshotRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (ciscoSnapshotInterfaceEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_ciscoSnapshotRowStatus_notInService) || (value->sl_value == D_ciscoSnapshotRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_ciscoSnapshotRowStatus_createAndGo) || (value->sl_value == D_ciscoSnapshotRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->ciscoSnapshotRowStatus = value->sl_value;
	/*
	 * perform generic row status processing
	 */
	retval = set_row_status(value->sl_value,
				entry_found ? 1 : -1,
				&(dp->state),
				&((ciscoSnapshotInterfaceEntry_t *)
				  (dp->data))->ciscoSnapshotRowStatus);

	if (retval != NO_ERROR)
	    return (retval);

     break;
#endif /* I_ciscoSnapshotRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ciscoSnapshotInterfaceEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_ciscoSnapshotInterfaceEntry_test */
   error_status = k_ciscoSnapshotInterfaceEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);
        error_status = k_ciscoSnapshotInterfaceEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ciscoSnapshotInterfaceEntry_set (doList_t       *doHead,
				 doList_t       *doCur,
				 ContextInfo    *contextInfo)
{
  return (k_ciscoSnapshotInterfaceEntry_set((ciscoSnapshotInterfaceEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ciscoSnapshotActivityEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoSnapshotActivityEntry_get (OID             *incoming,
				ObjectInfo      *object,
				int             searchType,
				ContextInfo     *contextInfo,
				int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ciscoSnapshotActivityEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            ciscoSnapshotIfIndex;
    long            ciscoSnapshotActivityIndex;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &ciscoSnapshotActivityIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ciscoSnapshotIfIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoSnapshotActivityEntry_get(serialNum, contextInfo, arg ,searchType, ciscoSnapshotIfIndex, ciscoSnapshotActivityIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->ciscoSnapshotIfIndex;
        inst.oid_ptr[1] = data->ciscoSnapshotActivityIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoSnapshotActivityState
    case I_ciscoSnapshotActivityState:
       dp = (void *) (&data->ciscoSnapshotActivityState);
       break;
#endif /* I_ciscoSnapshotActivityState */

#ifdef I_ciscoSnapshotActivityTimer
    case I_ciscoSnapshotActivityTimer:
       dp = (void *) (&data->ciscoSnapshotActivityTimer);
       break;
#endif /* I_ciscoSnapshotActivityTimer */

#ifdef I_ciscoSnapshotExchangeTimer
    case I_ciscoSnapshotExchangeTimer:
       dp = (void *) (&data->ciscoSnapshotExchangeTimer);
       break;
#endif /* I_ciscoSnapshotExchangeTimer */

#ifdef I_ciscoSnapshotDialerMap
    case I_ciscoSnapshotDialerMap:
       dp = (void *) (&data->ciscoSnapshotDialerMap);
       break;
#endif /* I_ciscoSnapshotDialerMap */

#ifdef I_ciscoSnapshotSourceProtocol
    case I_ciscoSnapshotSourceProtocol:
       dp = (void *) (&data->ciscoSnapshotSourceProtocol);
       break;
#endif /* I_ciscoSnapshotSourceProtocol */

#ifdef I_ciscoSnapshotSourceAddress
    case I_ciscoSnapshotSourceAddress:
       dp = (void *) (CloneOctetString(data->ciscoSnapshotSourceAddress));
       break;
#endif /* I_ciscoSnapshotSourceAddress */

#ifdef I_ciscoSnapshotProtocolsExchanged
    case I_ciscoSnapshotProtocolsExchanged:
       dp = (void *) (CloneOctetString(data->ciscoSnapshotProtocolsExchanged));
       break;
#endif /* I_ciscoSnapshotProtocolsExchanged */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */





ciscoSnapshotMIBObjects_t *
k_ciscoSnapshotMIBObjects_get (int serialNum,
			       ContextInfo *contextInfo,
			       int nominator)
{
    static ciscoSnapshotMIBObjects_t ciscoSnapshotMIBObjectsData;

    ciscoSnapshotMIBObjectsData.ciscoSnapshotForceActive =
      snapshot_last_forced_if_index;
    return(&ciscoSnapshotMIBObjectsData);
}

#ifdef SETS
int
k_ciscoSnapshotMIBObjects_test (ObjectInfo     *object,
				ObjectSyntax   *value,
				doList_t       *dp,
				ContextInfo    *contextInfo)

{
    idbtype *idb;
    snapshot_interface_info *sii = NULL;

    switch (object->nominator) {

#ifdef I_ciscoSnapshotForceActive
      case I_ciscoSnapshotForceActive:
	/* 
	 * now check if interface exists in the system. 
	 * Also it must be a client to be forced active.
	 */

	FOR_ALL_SWIDBS(idb) {
	    if (idb->hwptr->snmp_if_index == value->sl_value) {
		sii= snapshot_find_idb(idb);
		if (sii == NULL)
		    return (INCONSISTENT_VALUE_ERROR);
		if (SNAPSHOT_IS_CLIENT(sii)) {
		    SET_VALID(I_ciscoSnapshotForceActive,
			     ((ciscoSnapshotMIBObjects_t *)(dp->data))->valid);

		    ((ciscoSnapshotMIBObjects_t *) 
		     (dp->data))->ciscoSnapshotForceActive =
			 value->sl_value;
		}
		break;
	    }
	}
	if (!VALID(I_ciscoSnapshotForceActive,
		   ((ciscoSnapshotMIBObjects_t *)(dp->data))->valid))
	    return (INCONSISTENT_VALUE_ERROR);
	break;
#endif				       /* I_ciscoSnapshotForceActive */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ciscoSnapshotMIBObjects_test)\n"));
	return (GEN_ERROR);
    }

    return NO_ERROR;
}

int
k_ciscoSnapshotMIBObjects_ready (ObjectInfo     *object,
				 ObjectSyntax   *value,
				 doList_t       *doHead,
				 doList_t       *dp)
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_ciscoSnapshotMIBObjects_set (ciscoSnapshotMIBObjects_t *data,
			       ContextInfo *contextInfo,
			       int function)
{

    idbtype *idb;
    
    FOR_ALL_SWIDBS(idb) {
	if (idb->hwptr->snmp_if_index == data->ciscoSnapshotForceActive) {
	    snapshot_force_idb_active(idb, FALSE);
	    break;
	}
    }
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_ciscoSnapshotMIBObjects_UNDO
/* add #define SR_ciscoSnapshotMIBObjects_UNDO in snapshotmib.h to
 * include the undo routine for the ciscoSnapshotMIBObjects family.
 */
int
ciscoSnapshotMIBObjects_undo (doList_t       *doHead,
			      doList_t       *doCur,
			      ContextInfo    *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ciscoSnapshotMIBObjects_UNDO */
#endif /* SR_SNMPv2 */

ciscoSnapshotInterfaceEntry_t *
k_ciscoSnapshotInterfaceEntry_get (int serialNum,
				   ContextInfo *contextInfo,
				   int nominator,
				   int searchType,
				   long ciscoSnapshotIfIndex)
{
    static ciscoSnapshotInterfaceEntry_t ciscoSnapshotIfData;
    snapshot_interface_info *sii = NULL;
    snapshot_interface_info *lastSii = NULL;
    long   lastsnmp_if_index = 0xffff;        

    /*
     * set up a loop to check for valid column object in getnext case
     */
    while (TRUE) {

	if (searchType == EXACT) {
	    SNAPSHOT_FOR_ALL_SII(sii) {
		if (sii->idb->hwptr->snmp_if_index == ciscoSnapshotIfIndex)
		    break;
	    }
	} else {
	    /* Get next. First find sii with value >= current */
	    SNAPSHOT_FOR_ALL_SII(sii) {
		/* must search entire sii since they aren't in
		 *  ascending order by snmp_if_index...
		 */
		if ((sii->idb->hwptr->snmp_if_index >= ciscoSnapshotIfIndex) &&
		    (sii->idb->hwptr->snmp_if_index < lastsnmp_if_index)) {
		    lastsnmp_if_index = sii->idb->hwptr->snmp_if_index;
		    lastSii = sii;
		}
	    }
	}

	/* Phew! We have chosen the correct entry. Now, return values... */

        sii = lastSii;
	if (sii == NULL) {
	    /*
	     * Add code here to search notInService/notReady when supported
	     */	
	    return(NULL);
	}

	/*
	 * set data invalid before filling in.
	 */
	ZERO_VALID(ciscoSnapshotIfData.valid);

	ciscoSnapshotIfData.ciscoSnapshotIfIndex =lastsnmp_if_index;
	SET_VALID(I_ciscoSnapshotIfIndex, ciscoSnapshotIfData.valid);

	if (SNAPSHOT_IS_CLIENT(sii)) {
	    ciscoSnapshotIfData.ciscoSnapshotClient = D_ciscoSnapshotClient_true; 
	    ciscoSnapshotIfData.ciscoSnapshotQuietInterval = sii->quiet_time;
	    SET_VALID(I_ciscoSnapshotQuietInterval, ciscoSnapshotIfData.valid);
	    ciscoSnapshotIfData.ciscoSnapshotRetryInterval = 
		SNAPSHOT_RETRY_INTERVAL(sii->active_time);
	    SET_VALID(I_ciscoSnapshotRetryInterval, ciscoSnapshotIfData.valid);
	    ciscoSnapshotIfData.ciscoSnapshotIfUpAction =  
		SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii) ?
	    D_ciscoSnapshotIfUpAction_noAction : 
		D_ciscoSnapshotIfUpAction_goActive;
	    SET_VALID(I_ciscoSnapshotIfUpAction, ciscoSnapshotIfData.valid);
	} else {	
	    ciscoSnapshotIfData.ciscoSnapshotClient = D_ciscoSnapshotClient_false; 
	}
	SET_VALID(I_ciscoSnapshotClient, ciscoSnapshotIfData.valid);

	ciscoSnapshotIfData.ciscoSnapshotDialer = SNAPSHOT_IS_DIALER(sii) ?
	    D_ciscoSnapshotDialer_true : D_ciscoSnapshotDialer_false;
	SET_VALID(I_ciscoSnapshotDialer, ciscoSnapshotIfData.valid);

	ciscoSnapshotIfData.ciscoSnapshotActiveInterval = sii->active_time;
	SET_VALID(I_ciscoSnapshotActiveInterval, ciscoSnapshotIfData.valid);
	/*
	 * currently only support active rows!
	 */
	ciscoSnapshotIfData.ciscoSnapshotRowStatus = 
	    D_ciscoSnapshotRowStatus_active;
	SET_VALID(I_ciscoSnapshotRowStatus, ciscoSnapshotIfData.valid);

	if ((searchType == EXACT) || 
	    (VALID(nominator,ciscoSnapshotIfData.valid))) { 
	    return (&ciscoSnapshotIfData);
	    
	    } else {
		ciscoSnapshotIfIndex++;
		lastSii = sii = NULL;
		lastsnmp_if_index = 0xffff;
	    }
    }
}

#ifdef SETS
int
k_ciscoSnapshotInterfaceEntry_test (ObjectInfo     *object,
				    ObjectSyntax   *value,
				    doList_t       *dp,
				    ContextInfo    *contextInfo)
{

    switch (object->nominator) {

#ifdef I_ciscoSnapshotClient
      case I_ciscoSnapshotClient:
	/*
	 * don't allow the value to change once the row is active
	 */	
	if (((ciscoSnapshotInterfaceEntry_t *)
	(dp->data))->ciscoSnapshotRowStatus == D_ciscoSnapshotRowStatus_active)
	    return (INCONSISTENT_VALUE_ERROR);

	SET_VALID(I_ciscoSnapshotClient, 
		  ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);

	((ciscoSnapshotInterfaceEntry_t *) 
	 (dp->data))->ciscoSnapshotClient = value->sl_value;
	break;
#endif				       /* I_ciscoSnapshotClient */

#ifdef I_ciscoSnapshotDialer
      case I_ciscoSnapshotDialer:
	/*
	 * don't allow the value to change once the row is active
	 */
	if (((ciscoSnapshotInterfaceEntry_t *)
	(dp->data))->ciscoSnapshotRowStatus == D_ciscoSnapshotRowStatus_active)
	    return (INCONSISTENT_VALUE_ERROR);

	SET_VALID(I_ciscoSnapshotDialer, 
		  ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);

	((ciscoSnapshotInterfaceEntry_t *) 
	 (dp->data))->ciscoSnapshotDialer = value->sl_value;
	break;
#endif				       /* I_ciscoSnapshotDialer */

#ifdef I_ciscoSnapshotActiveInterval
      case I_ciscoSnapshotActiveInterval:
	if ((value->sl_value < MIN_ciscoSnapshotActiveInterval) ||
	    (value->sl_value > MAX_ciscoSnapshotActiveInterval))
	    return (WRONG_VALUE_ERROR);

	/*
	 * don't allow the value to change once the row is active
	 */
	if (((ciscoSnapshotInterfaceEntry_t *)
	(dp->data))->ciscoSnapshotRowStatus == D_ciscoSnapshotRowStatus_active)
	    return (INCONSISTENT_VALUE_ERROR);

	SET_VALID(I_ciscoSnapshotActiveInterval, 
		  ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);

	((ciscoSnapshotInterfaceEntry_t *) 
	 (dp->data))->ciscoSnapshotActiveInterval = value->sl_value;
	break;
#endif				       /* I_ciscoSnapshotActiveInterval */

#ifdef I_ciscoSnapshotQuietInterval
      case I_ciscoSnapshotQuietInterval:
	/*
	 * only clients can have quiet interval set...
	 */
	if (((ciscoSnapshotInterfaceEntry_t *)
	     (dp->data))->ciscoSnapshotClient == D_ciscoSnapshotClient_false)
	    return (INCONSISTENT_VALUE_ERROR);	
    
	if ((value->sl_value < MIN_ciscoSnapshotQuietInterval) ||
	    (value->sl_value > MAX_ciscoSnapshotQuietInterval))
	    return (WRONG_VALUE_ERROR);

							  
	/*
	 * don't allow the value to change once the row is active
	 */
	if (((ciscoSnapshotInterfaceEntry_t *)
	(dp->data))->ciscoSnapshotRowStatus == D_ciscoSnapshotRowStatus_active)
	    return (INCONSISTENT_VALUE_ERROR);

	SET_VALID(I_ciscoSnapshotQuietInterval, 
		  ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);

	((ciscoSnapshotInterfaceEntry_t *) 
	 (dp->data))->ciscoSnapshotQuietInterval = value->sl_value;

	break;
#endif				       /* I_ciscoSnapshotQuietInterval */

#ifdef I_ciscoSnapshotIfUpAction
      case I_ciscoSnapshotIfUpAction:
	/*
	 * only clients can have IfUpAction set...
	 */
	if (((ciscoSnapshotInterfaceEntry_t *)
	(dp->data))->ciscoSnapshotClient ==  D_ciscoSnapshotClient_false)
	    return (INCONSISTENT_VALUE_ERROR);	

	/*
	 * don't allow the value to change once the row is active
	 */
	if (((ciscoSnapshotInterfaceEntry_t *)
	(dp->data))->ciscoSnapshotRowStatus == D_ciscoSnapshotRowStatus_active)
	    return (INCONSISTENT_VALUE_ERROR);

	SET_VALID(I_ciscoSnapshotIfUpAction, 
		  ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);

	((ciscoSnapshotInterfaceEntry_t *) 
	 (dp->data))->ciscoSnapshotIfUpAction = value->sl_value;
	break;
#endif				       /* I_ciscoSnapshotIfUpAction */

#ifdef I_ciscoSnapshotRowStatus
      case I_ciscoSnapshotRowStatus:
	/*
	 * allow status to be set to createAndGo or detroy only for now...
	 */
	if (value->sl_value != D_ciscoSnapshotRowStatus_createAndGo)
	    if (value->sl_value != D_ciscoSnapshotRowStatus_destroy)
		return (INCONSISTENT_VALUE_ERROR);
	SET_VALID(I_ciscoSnapshotRowStatus, 
		  ((ciscoSnapshotInterfaceEntry_t *) (dp->data))->valid);

	break;
#endif				       /* I_ciscoSnapshotRowStatus */

      default:
	DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ciscoSnapshotInterfaceEntry_test)\n"));
	return (GEN_ERROR);

    }				       /* switch */


    return NO_ERROR;
}

int
k_ciscoSnapshotInterfaceEntry_ready (ObjectInfo     *object,
				     ObjectSyntax   *value,
				     doList_t       *doHead,
				     doList_t       *dp)
{
    ciscoSnapshotInterfaceEntry_t * ifEntry;
    boolean good_row = FALSE;
    /*
     * check for consistency between valid column objects for this
     * row.  IF inconsistencies exist with an invalid (default)
     * object, this function will set dp->state to UNKNOWN.
     */ 

    ifEntry = (ciscoSnapshotInterfaceEntry_t *) dp->data;

    if(ifEntry->ciscoSnapshotClient == D_ciscoSnapshotClient_true) {

	/*
	 * The quiet interval must never be allowed to be less than
	 * the retry interval, which is the minimum period of time
	 * to allow the server/client to both time out to fully
	 * 'quiet' in any communication mishap between client and
	 * server.
	 */
	if(VALID(I_ciscoSnapshotQuietInterval,ifEntry->valid) &&
	   VALID(I_ciscoSnapshotActiveInterval,ifEntry->valid)) {
	    if (ifEntry->ciscoSnapshotQuietInterval <
		SNAPSHOT_RETRY_INTERVAL(ifEntry->ciscoSnapshotActiveInterval))
		{
		    return (WRONG_VALUE_ERROR);
		}	
	}
    } else {
	/* must be valid server 
	 */

	if (VALID(I_ciscoSnapshotQuietInterval,ifEntry->valid))
	    return (WRONG_VALUE_ERROR);

	if (VALID(I_ciscoSnapshotIfUpAction,ifEntry->valid))
	    return (WRONG_VALUE_ERROR);
    }

    /* test for complete row - only rowstatus needs to be set.
     * Defaults can be accepted for all other objects.
     */
    if ((VALID(I_ciscoSnapshotRowStatus, ifEntry->valid)) && 
    (ifEntry->ciscoSnapshotQuietInterval >=
		SNAPSHOT_RETRY_INTERVAL(ifEntry->ciscoSnapshotActiveInterval)))
	good_row = TRUE;
    else
	good_row = FALSE;

    /*
     * transition the row status as necessary
     */
      switch (ifEntry->ciscoSnapshotRowStatus) {
	case D_ciscoSnapshotRowStatus_createAndGo:
	  if (good_row) 
	      dp->state = ADD_MODIFY;
	  else
	      dp->state = CREATE_AND_GO_STATE;
	  break;

	case D_ciscoSnapshotRowStatus_destroy:
	    dp->state = DELETE;
	    break;

	    /*  
	     * No support yet for modifying active rows, or setting row to
	     *  notInService.
	     */
	case D_ciscoSnapshotRowStatus_createAndWait:
        case D_ciscoSnapshotRowStatus_notInService:
	case D_ciscoSnapshotRowStatus_notReady:
	case D_ciscoSnapshotRowStatus_active:
	default:
	    return (INCONSISTENT_VALUE_ERROR);
            break;

      }
    return (NO_ERROR);
}


int
k_ciscoSnapshotInterfaceEntry_set_defaults (doList_t       *dp)
{
    ciscoSnapshotInterfaceEntry_t *data = (ciscoSnapshotInterfaceEntry_t *) (dp->data);
    boolean	    ifValid = FALSE;
    idbtype         *idb;

    FOR_ALL_SWIDBS(idb) {             /* validate the interface */
	if (data->ciscoSnapshotIfIndex == idb->hwptr->snmp_if_index)  {
	    ifValid = TRUE;
	    break;
	}	    
    }	    
    if (!ifValid)
	return (NO_CREATION_ERROR);


    /* Fill in default values here */

    data->ciscoSnapshotClient = DEF_ciscoSnapshotClient;
    data->ciscoSnapshotDialer = DEF_ciscoSnapshotDialer;
    data->ciscoSnapshotActiveInterval =	DEF_ciscoSnapshotActiveInterval;

    data->ciscoSnapshotQuietInterval = DEF_ciscoSnapshotQuietInterval;

    data->ciscoSnapshotIfUpAction = DEF_ciscoSnapshotIfUpAction;

    /*
     * we'll assign a value to ciscoSnapshotRowStatus but 
     * won't set the valid flag.  The valid flag will be set  when a 
     * set request for this object is received
     */
    data->ciscoSnapshotRowStatus = D_ciscoSnapshotRowStatus_createAndGo;

    return NO_ERROR;
}

int
k_ciscoSnapshotInterfaceEntry_set (ciscoSnapshotInterfaceEntry_t *data,
                                   ContextInfo *contextInfo,
                                   int function)
{
    idbtype *idb;
    snapshot_interface_info *sii = NULL;
    
    FOR_ALL_SWIDBS(idb) {
	if (idb->hwptr->snmp_if_index == data->ciscoSnapshotIfIndex) {
	    sii = idb->sii;
	    if ((sii != NULL) && (sii->idb != idb))
		return(NO_ERROR);
	    break;
	}
    }

    switch (function) {

    case DELETE:
	if (sii == NULL)
	    return (INCONSISTENT_VALUE_ERROR);

	snapshot_delete_usage(sii);
	break;

    case ADD_MODIFY:
	/*
	 * adding a new row only, if sii exists, return with error
	 */
	if (sii != NULL)
	    return (INCONSISTENT_VALUE_ERROR);
	
	if (VALID(I_ciscoSnapshotRowStatus, data->valid)) {
	    /*
	     * turn createAndGo into active since they're the same
	     * thing at this point.  Dummy placeholder code since all
	     * sets must be createAndGo at this point.
	     */
	    if (data->ciscoSnapshotRowStatus ==
		D_ciscoSnapshotRowStatus_createAndGo)
		data->ciscoSnapshotRowStatus = 
		    D_ciscoSnapshotRowStatus_active;

	    /*
	     *  add to sii since row is active 
	     */
	    sii = (snapshot_interface_info *)
		malloc(sizeof(snapshot_interface_info));
	    if (sii == NULL) {
		return(RESOURCE_UNAVAILABLE_ERROR);
	    }
	    if (QUEUEEMPTY(&snapshot_interfaces)) {
		cfork(snapshot_tasker, 0L, 0, "Snapshot", 0);
	    }
	    sii->data_integrity_check =	SNAPSHOT_INTERFACE_INFO_DATA_INTEGRITY;
	    sii->index = snapshot_interface_info_index++;  
	    sii->activity_index = 0;
	    sii->activity_blocks = NULL;
	    sii->idb = idb;
	    idb->sii = sii;
	    if (interface_up(idb)) {
		SNAPSHOT_SET_IDB_UP(sii);
	    } else {
		SNAPSHOT_SET_IDB_DOWN(sii);
	    }
	    enqueue(&snapshot_interfaces, sii);

	    SNAPSHOT_CLEAR_FLAGS(sii);
	    if (data->ciscoSnapshotClient ==
		D_ciscoSnapshotClient_false) {
		SNAPSHOT_SET_SERVER(sii);
	    }
	    sii->active_time = data->ciscoSnapshotActiveInterval;
	    sii->quiet_time = data->ciscoSnapshotQuietInterval;

	    if (data->ciscoSnapshotIfUpAction == 
		D_ciscoSnapshotIfUpAction_noAction) {
		SNAPSHOT_SET_SUPPRESS_STATECHANGE_UPDATES(sii);
	    }
	    if (data->ciscoSnapshotDialer == D_ciscoSnapshotDialer_true) {
		SNAPSHOT_SET_DIALER(sii);
	    }
	    snapshot_delete_all_activity_blocks(sii);
	    snapshot_create_initial_activity_blocks(sii);
	    reg_invoke_snapshot_state_change(sii->idb, TRUE);

	return (NO_ERROR);
	}

    default:
       /*
	* no other row operations allowed yet
	*/
	return (GEN_ERROR);
    }
    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_ciscoSnapshotInterfaceEntry_UNDO
/* add #define SR_ciscoSnapshotInterfaceEntry_UNDO in snapshotmib.h to
 * include the undo routine for the ciscoSnapshotInterfaceEntry family.
 */
int
ciscoSnapshotInterfaceEntry_undo (doList_t       *doHead,
				  doList_t       *doCur,
				  ContextInfo    *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ciscoSnapshotInterfaceEntry_UNDO */
#endif /* SR_SNMPv2 */

ciscoSnapshotActivityEntry_t *
k_ciscoSnapshotActivityEntry_get (int serialNum,
				  ContextInfo *contextInfo,
				  int nominator,
				  int searchType,
				  long ciscoSnapshotIfIndex,
				  long ciscoSnapshotActivityIndex)
{
   static ciscoSnapshotActivityEntry_t ciscoSnapshotActivityEntryData;
    snapshot_interface_info *sii = NULL;
    snapshot_activity_block *sab = NULL;
    static BITMASK_DEFINITION(protocolBits, LINK_MAXLINKTYPE);
    char addr_string[1000];
    snapshot_interface_info *lastSii = NULL;
    long   lastsnmp_if_index = 0xffff;        
    static OctetString src_addr;
    static OctetString proto_bits;

    BITMASK_CLEAR_ALL(protocolBits, LINK_MAXLINKTYPE);
    /*
     * loop through if getnext comes up invalid...
     */
    
    while (TRUE){
	if (searchType == EXACT) {
	    SNAPSHOT_FOR_ALL_SII(sii) {
		if (sii->idb->hwptr->snmp_if_index == ciscoSnapshotIfIndex)
		    break;
	    }
	    if (sii != NULL) {
		SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
		    if (sab->index == ciscoSnapshotActivityIndex)
			break;
		}
	    }
	} else {
	    /* Get next. First find sii with value >= current */
	    SNAPSHOT_FOR_ALL_SII(sii) {
		/* must search entire sii since they aren't in
		 *  ascending order by snmp_if_index...
		 */
		if ((sii->idb->hwptr->snmp_if_index >= ciscoSnapshotIfIndex) &&
		    (sii->idb->hwptr->snmp_if_index < lastsnmp_if_index)) {
		    lastsnmp_if_index = sii->idb->hwptr->snmp_if_index;
		    lastSii = sii;
		}
	    }
	    sii = lastSii;
	    if (sii != NULL) {
		/* Found. Now, choose next sab... */
		SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
		    if (sab->index >= ciscoSnapshotActivityIndex)
			break;
		}
		/* If SAB is null here, we want the sab of next entry... */
		if (sab == NULL) {
		    ciscoSnapshotIfIndex++;
		    ciscoSnapshotActivityIndex = 0;
		    lastSii = sii = NULL;
		    lastsnmp_if_index = 0xffff;
		    continue;
		}
	    }
	}

	/* Phew! We have chosen the correct entry. Now, return values... */
	if ((sii == NULL) || (sab == NULL)) {
	    return(NULL);
} 	
	/*
	 * set data invalid before filling in.
	 */
	ZERO_VALID(ciscoSnapshotActivityEntryData.valid);

	ciscoSnapshotActivityEntryData.ciscoSnapshotActivityIndex = sab->index;
	SET_VALID(I_ciscoSnapshotActivityIndex, ciscoSnapshotActivityEntryData.valid);
	ciscoSnapshotActivityEntryData.ciscoSnapshotIfIndex = sii->idb->hwptr->snmp_if_index;
	SET_VALID(I_ciscoSnapshotIfIndex, ciscoSnapshotActivityEntryData.valid);

	switch (sab->runtime_queue) {
	case SNAPSHOT_ACTIVE_Q:
	    ciscoSnapshotActivityEntryData.ciscoSnapshotActivityState =
		D_ciscoSnapshotActivityState_active; 
	    break;
	case SNAPSHOT_CLIENT_QUIET_Q: 
	    ciscoSnapshotActivityEntryData.ciscoSnapshotActivityState =
		D_ciscoSnapshotActivityState_quiet; 
	    break;
	case SNAPSHOT_SERVER_POST_ACTIVE_Q: 
	    ciscoSnapshotActivityEntryData.ciscoSnapshotActivityState =
		D_ciscoSnapshotActivityState_serverPostActive; 
	    break;
	case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q:
	    ciscoSnapshotActivityEntryData.ciscoSnapshotActivityState =
		D_ciscoSnapshotActivityState_transitionToQuiet;
	    break;
	case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q:
	    ciscoSnapshotActivityEntryData.ciscoSnapshotActivityState =
		D_ciscoSnapshotActivityState_transitionToActive;
	    break;
	case SNAPSHOT_NO_Q:
	default:
	    ciscoSnapshotActivityEntryData.ciscoSnapshotActivityState =
		D_ciscoSnapshotActivityState_limbo;
	    break;
	}
	SET_VALID(I_ciscoSnapshotActivityState, ciscoSnapshotActivityEntryData.valid);

	ciscoSnapshotActivityEntryData.ciscoSnapshotActivityTimer = 
	    sab->time_left_in_this_queue;
	SET_VALID(I_ciscoSnapshotActivityTimer, ciscoSnapshotActivityEntryData.valid);
	if (SNAPSHOT_IS_DIALER(sii)) {
	    ciscoSnapshotActivityEntryData.ciscoSnapshotDialerMap = sab->address;
	    SET_VALID(I_ciscoSnapshotDialerMap,
		      ciscoSnapshotActivityEntryData.valid); 
	}

	if (SNAPSHOT_IS_SERVER(sii)) {
	    addr_string[0] = '\0';
	    reg_invoke_sprintf_address_addr(sab->protocol_address.type,
					    addr_string,
					    &sab->protocol_address);
	    ciscoSnapshotActivityEntryData.ciscoSnapshotSourceAddress =
		&src_addr;
	    ciscoSnapshotActivityEntryData.ciscoSnapshotSourceAddress->
		octet_ptr = (uchar *)&addr_string;
	    ciscoSnapshotActivityEntryData.ciscoSnapshotSourceAddress->length =
		strlen(addr_string);
	    SET_VALID(I_ciscoSnapshotSourceAddress, ciscoSnapshotActivityEntryData.valid);

	    switch ( sab->protocol_address.type) {
	    case ADDR_APPLE :
		ciscoSnapshotActivityEntryData.ciscoSnapshotSourceProtocol = 
		    D_ciscoSnapshotSourceProtocol_appletalk;
		break;
	    
	    case  ADDR_IP :
		ciscoSnapshotActivityEntryData.ciscoSnapshotSourceProtocol = 
		    D_ciscoSnapshotSourceProtocol_ip;
		break;
	    
	    case  ADDR_NOVELL :
		ciscoSnapshotActivityEntryData.ciscoSnapshotSourceProtocol = 
		    D_ciscoSnapshotSourceProtocol_novell;
		break;

	    case ADDR_VINES :
		ciscoSnapshotActivityEntryData.ciscoSnapshotSourceProtocol = 
		    D_ciscoSnapshotSourceProtocol_vines;
		break;

	    default:
		/* add code for error - not right protocol! */
		break;
	    }
	    SET_VALID(I_ciscoSnapshotSourceProtocol, ciscoSnapshotActivityEntryData.valid);

	} else {
	    /*  must be client
	     *
	     */
	    ciscoSnapshotActivityEntryData.ciscoSnapshotExchangeTimer =
		sab->active_exchange_period;
	    SET_VALID(I_ciscoSnapshotExchangeTimer, ciscoSnapshotActivityEntryData.valid);
	
	    if (BITMASK_TEST(sab->protocols_started_to_age, LINK_APPLETALK)) {
		BITMASK_SET(protocolBits,D_ciscoSnapshotSourceProtocol_appletalk);
	    }
	    if (BITMASK_TEST(sab->protocols_started_to_age, LINK_IP)) {
		BITMASK_SET(protocolBits,D_ciscoSnapshotSourceProtocol_ip);
	    }
	    if (BITMASK_TEST(sab->protocols_started_to_age, LINK_NOVELL)) {
		BITMASK_SET(protocolBits,D_ciscoSnapshotSourceProtocol_novell);
	    }
	    if (BITMASK_TEST(sab->protocols_started_to_age, LINK_VINES)) {
		BITMASK_SET(protocolBits,D_ciscoSnapshotSourceProtocol_vines);
	    }

	    ciscoSnapshotActivityEntryData.ciscoSnapshotProtocolsExchanged =
		&proto_bits;
	    ciscoSnapshotActivityEntryData.ciscoSnapshotProtocolsExchanged->
		octet_ptr = protocolBits;
	    ciscoSnapshotActivityEntryData.ciscoSnapshotProtocolsExchanged->length =
		strlen(protocolBits);   
	    SET_VALID(I_ciscoSnapshotProtocolsExchanged, ciscoSnapshotActivityEntryData.valid);
	}
	if ((searchType == EXACT) ||
 	    (VALID(nominator,ciscoSnapshotActivityEntryData.valid))) {
	    return (&ciscoSnapshotActivityEntryData);
	} else {
	    ciscoSnapshotIfIndex++;
	    lastSii = sii = NULL;
	    lastsnmp_if_index = 0xffff;
	    sab = NULL;
	}
    }
}




static void
init_snapshotmib (subsystype *subsys)
{
    load_mib(snapshotmib_OidList, snapshotmib_OidListNum);
    load_oid(snapshotmib_oid_table);
}

/*
 * snapshotmib subsystem header
 */

/*WARNING: update "req" with the subsystem being monitored */

#define MAJVERSION_snapshotmib 1
#define MINVERSION_snapshotmib 0
#define EDITVERSION_snapshotmib 0

SUBSYS_HEADER(snapshotmib,
              MAJVERSION_snapshotmib,
              MINVERSION_snapshotmib,
              EDITVERSION_snapshotmib,
              init_snapshotmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: snapshot",
              NULL);

      
