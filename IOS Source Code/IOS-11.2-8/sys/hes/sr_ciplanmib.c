/* $Id: sr_ciplanmib.c,v 3.4.8.2 1996/07/01 18:44:23 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_ciplanmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Michael Otto (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ciplanmib.c,v $
 * Revision 3.4.8.2  1996/07/01  18:44:23  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.8.1  1996/04/04  05:32:41  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.4  1996/02/28  16:34:58  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1996/01/24  22:02:59  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.2  1995/11/17  09:19:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:01:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/28  09:24:44  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/27  20:46:17  motto
 * Code Review updates:
 *   - Make use of snmp_if_index instead of hw_if_index
 *     using snmp functions in sr_core.c.
 *   - Source line length adjustments.
 *   - Change VLAN to LAN for CIP internal LANs.
 *     This was basically renaming the MIBS,
 *     mib source files, and makefiles.
 *     This also involved changing the
 *     CIPCSNA mib references to virtual
 *     LANs on the CIP.
 *   - Fix query bug in sr_ciplanmib.c for
 *     CIP internal LAN adapters.
 * CSCdi33408:  CIP SNA SNMP MIB support
 *
 * Revision 2.1  1995/06/25  16:40:29  motto
 * Placeholder for Arizona
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
#include "interface_private.h"
#include "ieee.h"
#include "../snmp/snmp_api.h"
#include "if_cip.h"
#include "if_cip_vlan.h"
#include "sr_ciplanmib.h"
#include "sr_ciplanmib-mib.h"
#include "channelmib_utils.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the cipCardLanAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardLanAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipCardLanAdminEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            cipCardLanAdminLanType;
    long            cipCardLanAdminLanId;


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

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardLanAdminLanId, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cipCardLanAdminLanId = MAX(0, cipCardLanAdminLanId);
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardLanAdminLanType, searchType, &carry)) < 0 ) {
       arg = -1;
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
     if ( (arg == -1) || (data = k_cipCardLanAdminEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipCardLanAdminLanType, cipCardLanAdminLanId)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->cipCardLanAdminLanType;
        inst.oid_ptr[2] = data->cipCardLanAdminLanId;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardLanAdminBridgeType
    case I_cipCardLanAdminBridgeType:
       dp = (void *) (&data->cipCardLanAdminBridgeType);
       break;
#endif /* I_cipCardLanAdminBridgeType */

#ifdef I_cipCardLanAdminSrbLocalRing
    case I_cipCardLanAdminSrbLocalRing:
       dp = (void *) (&data->cipCardLanAdminSrbLocalRing);
       break;
#endif /* I_cipCardLanAdminSrbLocalRing */

#ifdef I_cipCardLanAdminSrbBridgeNum
    case I_cipCardLanAdminSrbBridgeNum:
       dp = (void *) (&data->cipCardLanAdminSrbBridgeNum);
       break;
#endif /* I_cipCardLanAdminSrbBridgeNum */

#ifdef I_cipCardLanAdminSrbTargetRing
    case I_cipCardLanAdminSrbTargetRing:
       dp = (void *) (&data->cipCardLanAdminSrbTargetRing);
       break;
#endif /* I_cipCardLanAdminSrbTargetRing */

#ifdef I_cipCardLanAdminTbBridgeGrp
    case I_cipCardLanAdminTbBridgeGrp:
       dp = (void *) (&data->cipCardLanAdminTbBridgeGrp);
       break;
#endif /* I_cipCardLanAdminTbBridgeGrp */

#ifdef I_cipCardLanAdminRowStatus
    case I_cipCardLanAdminRowStatus:
       dp = (void *) (&data->cipCardLanAdminRowStatus);
       break;
#endif /* I_cipCardLanAdminRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cipCardLanAdminEntry data object.
 *---------------------------------------------------------------------*/
void
cipCardLanAdminEntry_free(cipCardLanAdminEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cipCardLanAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
cipCardLanAdminEntry_cleanup(doList_t *trash)
{
   cipCardLanAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
   cipCardLanAdminEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cipCardLanAdminEntry family
 *---------------------------------------------------------------------*/
cipCardLanAdminEntry_t *
Clone_cipCardLanAdminEntry(cipCardLanAdminEntry_t *cipCardLanAdminEntry)
{
    cipCardLanAdminEntry_t *data;

    if ((data = malloc(sizeof(cipCardLanAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cipCardLanAdminEntry), sizeof(cipCardLanAdminEntry_t));


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
cipCardLanAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cipCardLanAdminEntry_t     *cipCardLanAdminEntry;
    long           ifIndex;
    long           cipCardLanAdminLanType;
    long           cipCardLanAdminLanId;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 3) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardLanAdminLanType, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardLanAdminLanId, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    cipCardLanAdminEntry = k_cipCardLanAdminEntry_get(-1, contextInfo, -1, EXACT, ifIndex, cipCardLanAdminLanType, cipCardLanAdminLanId);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cipCardLanAdminEntry_set) &&
            (((cipCardLanAdminEntry_t *) (dp->data)) != NULL) &&
            (((cipCardLanAdminEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminLanType == cipCardLanAdminLanType) &&
            (((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminLanId == cipCardLanAdminLanId) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cipCardLanAdminEntry_set;
        dp->cleanupMethod = cipCardLanAdminEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cipCardLanAdminEntry_UNDO
        dp->undoMethod = cipCardLanAdminEntry_undo;
#else /* SR_cipCardLanAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cipCardLanAdminEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cipCardLanAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cipCardLanAdminEntry(cipCardLanAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cipCardLanAdminEntry(cipCardLanAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(cipCardLanAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cipCardLanAdminEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cipCardLanAdminEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((cipCardLanAdminEntry_t *) (dp->data))->valid);

                ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminLanType = (cipCardLanAdminLanType);
                SET_VALID(I_cipCardLanAdminLanType, ((cipCardLanAdminEntry_t *) (dp->data))->valid);

                ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminLanId = (cipCardLanAdminLanId);
                SET_VALID(I_cipCardLanAdminLanId, ((cipCardLanAdminEntry_t *) (dp->data))->valid);

                error_status = k_cipCardLanAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cipCardLanAdminBridgeType
   case I_cipCardLanAdminBridgeType:

     if ((value->sl_value < 1) || (value->sl_value > 3)) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminBridgeType = value->sl_value;
     break;
#endif /* I_cipCardLanAdminBridgeType */

#ifdef I_cipCardLanAdminSrbLocalRing
   case I_cipCardLanAdminSrbLocalRing:

     if ( ((value->sl_value < 1) || (value->sl_value > 4095)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminSrbLocalRing = value->sl_value;
     break;
#endif /* I_cipCardLanAdminSrbLocalRing */

#ifdef I_cipCardLanAdminSrbBridgeNum
   case I_cipCardLanAdminSrbBridgeNum:

     if ( ((value->sl_value < 1) || (value->sl_value > 15)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminSrbBridgeNum = value->sl_value;
     break;
#endif /* I_cipCardLanAdminSrbBridgeNum */

#ifdef I_cipCardLanAdminSrbTargetRing
   case I_cipCardLanAdminSrbTargetRing:

     if ( ((value->sl_value < 1) || (value->sl_value > 4095)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminSrbTargetRing = value->sl_value;
     break;
#endif /* I_cipCardLanAdminSrbTargetRing */

#ifdef I_cipCardLanAdminTbBridgeGrp
   case I_cipCardLanAdminTbBridgeGrp:

     if ( ((value->sl_value < 1) || (value->sl_value > 63)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminTbBridgeGrp = value->sl_value;
     break;
#endif /* I_cipCardLanAdminTbBridgeGrp */

#ifdef I_cipCardLanAdminRowStatus
   case I_cipCardLanAdminRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_cipCardLanAdminRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (cipCardLanAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_cipCardLanAdminRowStatus_notInService) || (value->sl_value == D_cipCardLanAdminRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_cipCardLanAdminRowStatus_createAndGo) || (value->sl_value == D_cipCardLanAdminRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((cipCardLanAdminEntry_t *) (dp->data))->cipCardLanAdminRowStatus = value->sl_value;
     break;
#endif /* I_cipCardLanAdminRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipCardLanAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cipCardLanAdminEntry_test */
   error_status = k_cipCardLanAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cipCardLanAdminEntry_t *) (dp->data))->valid);
        error_status = k_cipCardLanAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cipCardLanAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cipCardLanAdminEntry_set((cipCardLanAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cipCardLanAdaptAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardLanAdaptAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    cipCardLanAdaptAdminEntry_t *data = NULL;
    unsigned long   buffer[4];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            cipCardLanAdminLanType;
    long            cipCardLanAdminLanId;
    long            cipCardLanAdaptAdminAdaptNo;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 4 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 4) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 3 + object->oid.length, &cipCardLanAdaptAdminAdaptNo, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cipCardLanAdaptAdminAdaptNo = MAX(0, cipCardLanAdaptAdminAdaptNo);
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardLanAdminLanId, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cipCardLanAdminLanId = MAX(0, cipCardLanAdminLanId);
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardLanAdminLanType, searchType, &carry)) < 0 ) {
       arg = -1;
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
     if ( (arg == -1) || (data = k_cipCardLanAdaptAdminEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipCardLanAdminLanType, cipCardLanAdminLanId, cipCardLanAdaptAdminAdaptNo)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 4;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->cipCardLanAdminLanType;
        inst.oid_ptr[2] = data->cipCardLanAdminLanId;
        inst.oid_ptr[3] = data->cipCardLanAdaptAdminAdaptNo;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardLanAdaptAdminMacAddress
    case I_cipCardLanAdaptAdminMacAddress:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardLanAdaptAdminMacAddress));
       }
       break;
#endif /* I_cipCardLanAdaptAdminMacAddress */

#ifdef I_cipCardLanAdaptAdminAdaptName
    case I_cipCardLanAdaptAdminAdaptName:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardLanAdaptAdminAdaptName));
       }
       break;
#endif /* I_cipCardLanAdaptAdminAdaptName */

#ifdef I_cipCardLanAdaptAdminRowStatus
    case I_cipCardLanAdaptAdminRowStatus:
       dp = (void *) (&data->cipCardLanAdaptAdminRowStatus);
       break;
#endif /* I_cipCardLanAdaptAdminRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cipCardLanAdaptAdminEntry data object.
 *---------------------------------------------------------------------*/
void
cipCardLanAdaptAdminEntry_free(cipCardLanAdaptAdminEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->cipCardLanAdaptAdminMacAddress);
       FreeOctetString(data->cipCardLanAdaptAdminAdaptName);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cipCardLanAdaptAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
cipCardLanAdaptAdminEntry_cleanup(doList_t *trash)
{
   cipCardLanAdaptAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
   cipCardLanAdaptAdminEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cipCardLanAdaptAdminEntry family
 *---------------------------------------------------------------------*/
cipCardLanAdaptAdminEntry_t *
Clone_cipCardLanAdaptAdminEntry(cipCardLanAdaptAdminEntry_t *cipCardLanAdaptAdminEntry)
{
    cipCardLanAdaptAdminEntry_t *data;

    if ((data = malloc(sizeof(cipCardLanAdaptAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cipCardLanAdaptAdminEntry), sizeof(cipCardLanAdaptAdminEntry_t));

    data->cipCardLanAdaptAdminMacAddress = CloneOctetString(cipCardLanAdaptAdminEntry->cipCardLanAdaptAdminMacAddress);
    data->cipCardLanAdaptAdminAdaptName = CloneOctetString(cipCardLanAdaptAdminEntry->cipCardLanAdaptAdminAdaptName);

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
cipCardLanAdaptAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cipCardLanAdaptAdminEntry_t     *cipCardLanAdaptAdminEntry;
    long           ifIndex;
    long           cipCardLanAdminLanType;
    long           cipCardLanAdminLanId;
    long           cipCardLanAdaptAdminAdaptNo;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 4) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardLanAdminLanType, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardLanAdminLanId, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 3 + object->oid.length, &cipCardLanAdaptAdminAdaptNo, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    cipCardLanAdaptAdminEntry = k_cipCardLanAdaptAdminEntry_get(-1, contextInfo, -1, EXACT, ifIndex, cipCardLanAdminLanType, cipCardLanAdminLanId, cipCardLanAdaptAdminAdaptNo);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cipCardLanAdaptAdminEntry_set) &&
            (((cipCardLanAdaptAdminEntry_t *) (dp->data)) != NULL) &&
            (((cipCardLanAdaptAdminEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdminLanType == cipCardLanAdminLanType) &&
            (((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdminLanId == cipCardLanAdminLanId) &&
            (((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminAdaptNo == cipCardLanAdaptAdminAdaptNo) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cipCardLanAdaptAdminEntry_set;
        dp->cleanupMethod = cipCardLanAdaptAdminEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cipCardLanAdaptAdminEntry_UNDO
        dp->undoMethod = cipCardLanAdaptAdminEntry_undo;
#else /* SR_cipCardLanAdaptAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cipCardLanAdaptAdminEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cipCardLanAdaptAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cipCardLanAdaptAdminEntry(cipCardLanAdaptAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cipCardLanAdaptAdminEntry(cipCardLanAdaptAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(cipCardLanAdaptAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cipCardLanAdaptAdminEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cipCardLanAdaptAdminEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((cipCardLanAdaptAdminEntry_t *) (dp->data))->valid);

                ((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdminLanType = (cipCardLanAdminLanType);
                SET_VALID(I_cipCardLanAdminLanType, ((cipCardLanAdaptAdminEntry_t *) (dp->data))->valid);

                ((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdminLanId = (cipCardLanAdminLanId);
                SET_VALID(I_cipCardLanAdminLanId, ((cipCardLanAdaptAdminEntry_t *) (dp->data))->valid);

                ((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminAdaptNo = (cipCardLanAdaptAdminAdaptNo);
                SET_VALID(I_cipCardLanAdaptAdminAdaptNo, ((cipCardLanAdaptAdminEntry_t *) (dp->data))->valid);

                error_status = k_cipCardLanAdaptAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cipCardLanAdaptAdminMacAddress
   case I_cipCardLanAdaptAdminMacAddress:

     if (((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminMacAddress != NULL) {
        FreeOctetString(((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminMacAddress);
     }

     ((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminMacAddress = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardLanAdaptAdminMacAddress */

#ifdef I_cipCardLanAdaptAdminAdaptName
   case I_cipCardLanAdaptAdminAdaptName:

     if (((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminAdaptName != NULL) {
        FreeOctetString(((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminAdaptName);
     }

     ((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminAdaptName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardLanAdaptAdminAdaptName */

#ifdef I_cipCardLanAdaptAdminRowStatus
   case I_cipCardLanAdaptAdminRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_cipCardLanAdaptAdminRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (cipCardLanAdaptAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_cipCardLanAdaptAdminRowStatus_notInService) || (value->sl_value == D_cipCardLanAdaptAdminRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_cipCardLanAdaptAdminRowStatus_createAndGo) || (value->sl_value == D_cipCardLanAdaptAdminRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((cipCardLanAdaptAdminEntry_t *) (dp->data))->cipCardLanAdaptAdminRowStatus = value->sl_value;
     break;
#endif /* I_cipCardLanAdaptAdminRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipCardLanAdaptAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cipCardLanAdaptAdminEntry_test */
   error_status = k_cipCardLanAdaptAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cipCardLanAdaptAdminEntry_t *) (dp->data))->valid);
        error_status = k_cipCardLanAdaptAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cipCardLanAdaptAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cipCardLanAdaptAdminEntry_set((cipCardLanAdaptAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */

static long convert_lan_type_to_snmp ( ushort type) {
   switch ( type) {
      case ETHER:
         return D_cipCardLanAdminLanType_iso88023csmacd;
      case TOKEN:
         return D_cipCardLanAdminLanType_iso88025tokenRing;
      case FDDI:
         return D_cipCardLanAdminLanType_fddi;
      default:
         break;
   }
   return 0;
}

static ushort convert_lan_type_from_snmp ( long type) {
   switch ( type) {
      case D_cipCardLanAdminLanType_iso88023csmacd:
         return ETHER;
      case D_cipCardLanAdminLanType_iso88025tokenRing:
         return TOKEN;
      case D_cipCardLanAdminLanType_fddi:
         return FDDI;
      default:
         break;
   }
   return 0;
}

static long convert_bridge_type_to_snmp ( ushort type) {
   switch ( type) {
      case CSNA_VLAN_BRIDGE_TYPE_SRB:
         return D_cipCardLanAdminBridgeType_sourcerouteOnly;
      case CSNA_VLAN_BRIDGE_TYPE_TB:
         return D_cipCardLanAdminBridgeType_transparentOnly;
      case CSNA_VLAN_BRIDGE_TYPE_HUH:
         return D_cipCardLanAdminBridgeType_transpAndSourceRoute;
      default:
         break;
   }
   return 0;
}

cipCardLanAdminEntry_t *
k_cipCardLanAdminEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, cipCardLanAdminLanType, cipCardLanAdminLanId)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long cipCardLanAdminLanType;
   long cipCardLanAdminLanId;
{
   static cipCardLanAdminEntry_t cipCardLanAdminData;
   static int   lastSerialNum = MAXINT;
   static long  lastIfIndex = MAXLONG;
   static long  lastCipCardLanAdminLanType = MAXLONG;
   static long  lastCipCardLanAdminLanId = MAXLONG;

   long lan_type;
   hwidbtype *hwidb;
   boolean exactSearch;
   void *vlan = NULL;
   cipCardLanAdminEntry_t *result_p = NULL;

   /* 
    * Check for match of cached instance to avoid searching
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipCardLanAdminLanType == cipCardLanAdminLanType) &&
       (lastCipCardLanAdminLanId == cipCardLanAdminLanId)) {
      result_p = &cipCardLanAdminData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      lastIfIndex = ifIndex;
      lastCipCardLanAdminLanId = cipCardLanAdminLanId;
      lastCipCardLanAdminLanType = cipCardLanAdminLanType;
      exactSearch = (searchType == EXACT);
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         lan_type = convert_lan_type_from_snmp( lastCipCardLanAdminLanType);
         vlan = cip_get_vlan( hwidb, exactSearch, lan_type,
                              lastCipCardLanAdminLanId);
         if (vlan) {
            /*
             *  Found instance, exit FOR_ALL_HWIDBS loop.
             */
            break;
         }
         else {
            if (exactSearch) {
               /* 
                * Not found for exact match, then exit FOR_ALL_HWIDBS loop
                */
               break;
         }
            else {
               /* 
                * If not found on this hwidb, go to next hwidb 
                */
               lastIfIndex = hwidb->snmp_if_index + 1;
               lastCipCardLanAdminLanType = D_cipCardLanAdminLanType_iso88023csmacd;
               lastCipCardLanAdminLanId = 0;
            }
         }
      }
      if (vlan) {
         cipCardLanAdminData.cipCardLanAdminBridgeType = 
                convert_bridge_type_to_snmp( get_vlan_bridge_type( vlan));
         cipCardLanAdminData.cipCardLanAdminSrbLocalRing = 
                get_vlan_srb_local_ring( vlan);
         cipCardLanAdminData.cipCardLanAdminSrbBridgeNum = 
                get_vlan_srb_bridge_num( vlan);
         cipCardLanAdminData.cipCardLanAdminSrbTargetRing = 
                get_vlan_srb_target_ring( vlan);
         cipCardLanAdminData.cipCardLanAdminTbBridgeGrp = 
                get_vlan_tb_bridge_group( vlan);
         cipCardLanAdminData.cipCardLanAdminRowStatus = 
                D_cipCardLanAdminRowStatus_active;
         cipCardLanAdminData.cipCardLanAdminLanType = 
                convert_lan_type_to_snmp( get_vlan_lan_type( vlan));
         cipCardLanAdminData.cipCardLanAdminLanId = get_vlan_lan_id( vlan);
         cipCardLanAdminData.ifIndex = hwidb->snmp_if_index;
         lastCipCardLanAdminLanType = cipCardLanAdminLanType;
         lastCipCardLanAdminLanId = cipCardLanAdminLanId;
         lastIfIndex = ifIndex;
         lastSerialNum = serialNum;
         SET_ALL_VALID( cipCardLanAdminData.valid);
         result_p = &cipCardLanAdminData;
      }
   }
   return result_p;
}

#ifdef SETS
int
k_cipCardLanAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cipCardLanAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cipCardLanAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    return NO_ERROR;
}

int
k_cipCardLanAdminEntry_set(data, contextInfo, function)
   cipCardLanAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cipCardLanAdminEntry_UNDO
/* add #define SR_cipCardLanAdminEntry_UNDO in ciplanmib.h to
 * include the undo routine for the cipCardLanAdminEntry family.
 */
int
cipCardLanAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cipCardLanAdminEntry_UNDO */
#endif /* SR_SNMPv2 */

cipCardLanAdaptAdminEntry_t *
k_cipCardLanAdaptAdminEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, cipCardLanAdminLanType, cipCardLanAdminLanId, cipCardLanAdaptAdminAdaptNo)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long cipCardLanAdminLanType;
   long cipCardLanAdminLanId;
   long cipCardLanAdaptAdminAdaptNo;
{
   static cipCardLanAdaptAdminEntry_t cipCardLanAdaptAdminData;
   static int   lastSerialNum = MAXINT;
   static long  lastIfIndex = MAXLONG;
   static long  lastCipCardLanAdminLanType = MAXLONG;
   static long  lastCipCardLanAdminLanId = MAXLONG;
   static long  lastCipCardLanAdaptAdminAdaptNo = MAXLONG;
   static OctetString adapt_name_OS;
   static OctetString mac_addr_OS;

   hwidbtype *hwidb;
   boolean exactSearch;
   void *vlan = NULL;
   void *vadapt = NULL;
   cipCardLanAdaptAdminEntry_t *result_p = NULL;

   /* 
    * Check for match of cached instance to avoid searching
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipCardLanAdminLanType == cipCardLanAdminLanType) &&
       (lastCipCardLanAdminLanId == cipCardLanAdminLanId) &&
       (lastCipCardLanAdaptAdminAdaptNo == cipCardLanAdaptAdminAdaptNo)) {
      result_p = &cipCardLanAdaptAdminData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      lastIfIndex = ifIndex;
      lastCipCardLanAdminLanType = cipCardLanAdminLanType;
      lastCipCardLanAdminLanId = cipCardLanAdminLanId;
      lastCipCardLanAdaptAdminAdaptNo = cipCardLanAdaptAdminAdaptNo;
      exactSearch = (searchType == EXACT);
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         while((vlan = cip_get_vlan( hwidb, exactSearch, 
                    convert_lan_type_from_snmp( lastCipCardLanAdminLanType),
                    lastCipCardLanAdminLanId)) != NULL) {
            vadapt = cip_get_vadapt_from_adapno( hwidb, vlan, exactSearch,
                                         lastCipCardLanAdaptAdminAdaptNo);
            if (vadapt || exactSearch) {
               /*
                *  Found instance or exact search not found, 
                *  copy data to cached structure and exit
                *  FOR_ALL_HWIDBS loop with result_p pointing to cached 
                *  data structure.
                */
               break;
            }
            /* 
             * If not found on this vlan, go to next vlan 
             */
            lastCipCardLanAdaptAdminAdaptNo = 0;
            lastCipCardLanAdminLanId = get_vlan_lan_id( vlan) + 1;
            lastCipCardLanAdminLanType = 
                  convert_lan_type_to_snmp( get_vlan_lan_type( vlan));
            if (!lastCipCardLanAdminLanId)
                lastCipCardLanAdminLanType++;
         }
         if (vadapt || exactSearch)
            break;
         /* 
          * If not found on this hwidb, go to next hwidb 
          */
         lastCipCardLanAdaptAdminAdaptNo = 0;
         lastCipCardLanAdminLanType = D_cipCardLanAdminLanType_iso88023csmacd;
         lastCipCardLanAdminLanId = 0;
         lastIfIndex = hwidb->snmp_if_index + 1;
      }
      if (vlan && vadapt) {
         mac_addr_OS.octet_ptr = get_vadapt_mac_address( vadapt);
         mac_addr_OS.length = IEEEBYTES;
         cipCardLanAdaptAdminData.cipCardLanAdaptAdminMacAddress = &mac_addr_OS;
         adapt_name_OS.octet_ptr = get_vadapt_adapt_name( vadapt);
         adapt_name_OS.length = strlen( adapt_name_OS.octet_ptr);
         cipCardLanAdaptAdminData.cipCardLanAdaptAdminAdaptName = &adapt_name_OS;
         cipCardLanAdaptAdminData.cipCardLanAdaptAdminRowStatus = 
                D_cipCardLanAdaptAdminRowStatus_active;
         cipCardLanAdaptAdminData.cipCardLanAdaptAdminAdaptNo = 
                get_vadapt_adapt_no( vadapt);
	 cipCardLanAdaptAdminData.cipCardLanAdminLanType = 
                convert_lan_type_to_snmp( get_vlan_lan_type( vlan));
         cipCardLanAdaptAdminData.cipCardLanAdminLanId = 
                get_vlan_lan_id( vlan);
         cipCardLanAdaptAdminData.ifIndex = hwidb->snmp_if_index;
         lastCipCardLanAdaptAdminAdaptNo = cipCardLanAdaptAdminAdaptNo;
         lastCipCardLanAdminLanType = cipCardLanAdminLanType;
         lastCipCardLanAdminLanId = cipCardLanAdminLanId;
         lastIfIndex = ifIndex;
         lastSerialNum = serialNum;
         SET_ALL_VALID( cipCardLanAdaptAdminData.valid);
         result_p = &cipCardLanAdaptAdminData;
      }
   }
   return result_p;
}

#ifdef SETS
int
k_cipCardLanAdaptAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cipCardLanAdaptAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cipCardLanAdaptAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    return NO_ERROR;
}

int
k_cipCardLanAdaptAdminEntry_set(data, contextInfo, function)
   cipCardLanAdaptAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cipCardLanAdaptAdminEntry_UNDO
/* add #define SR_cipCardLanAdaptAdminEntry_UNDO in ciplanmib.h to
 * include the undo routine for the cipCardLanAdaptAdminEntry family.
 */
int
cipCardLanAdaptAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cipCardLanAdaptAdminEntry_UNDO */
#endif /* SR_SNMPv2 */



static void init_ciplanmib (subsystype *subsys)
{
    load_mib(ciplanmib_OidList, ciplanmib_OidListNum);
    load_oid(ciplanmib_oid_table);
}

#define MAJVERSION_ciplanmib 1
#define MINVERSION_ciplanmib 0
#define EDITVERSION_ciplanmib 0

SUBSYS_HEADER(ciplanmib,
              MAJVERSION_ciplanmib,
              MINVERSION_ciplanmib,
              EDITVERSION_ciplanmib,
              init_ciplanmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: cip", NULL);
 

      
