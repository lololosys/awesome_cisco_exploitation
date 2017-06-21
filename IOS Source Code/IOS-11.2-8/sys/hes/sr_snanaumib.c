/* $Id: sr_snanaumib.c,v 3.1.4.4 1996/09/07 02:07:47 snyder Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_snanaumib.c,v $
 *------------------------------------------------------------------
 * NAU mib SNMP sub-agent code
 *
 * Jan 96, Barry Chan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_snanaumib.c,v $
 * Revision 3.1.4.4  1996/09/07  02:07:47  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.1.4.3  1996/07/01  18:44:35  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.4.2  1996/06/22  23:47:27  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.1.4.1  1996/05/17  11:19:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.2  1996/05/12  08:10:08  bchan
 * CSCdi57469:  change tn3270smib and show tn3270
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.2.1  1996/03/21  03:21:43  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/20  07:00:44  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_snanaumib.h"
#include "sr_snanaumib-mib.h"
#include "sysmgt_snanaumib.h"
#include "sysmgt_tn3270s.h"
#include "if_tn3270s.h"

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




#ifdef HAVE_STDLIB_H
#endif	/* HAVE_STDLIB_H */

#ifdef HAVE_MALLOC_H
#endif	/* HAVE_MALLOC_H */


/*---------------------------------------------------------------------
 * Retrieve data from the snaNode family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaNode_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    snaNode_t       *data = NULL;


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
     if ( (arg == -1) || (data = k_snaNode_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaNodeAdminTableLastChange
    case I_snaNodeAdminTableLastChange:
       dp = (void *) (&data->snaNodeAdminTableLastChange);
       break;
#endif /* I_snaNodeAdminTableLastChange */

#ifdef I_snaNodeOperTableLastChange
    case I_snaNodeOperTableLastChange:
       dp = (void *) (&data->snaNodeOperTableLastChange);
       break;
#endif /* I_snaNodeOperTableLastChange */

#ifdef I_snaNodeLinkAdminTableLastChange
    case I_snaNodeLinkAdminTableLastChange:
       dp = (void *) (&data->snaNodeLinkAdminTableLastChange);
       break;
#endif /* I_snaNodeLinkAdminTableLastChange */

#ifdef I_snaNodeLinkOperTableLastChange
    case I_snaNodeLinkOperTableLastChange:
       dp = (void *) (&data->snaNodeLinkOperTableLastChange);
       break;
#endif /* I_snaNodeLinkOperTableLastChange */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the snaNodeAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaNodeAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaNodeAdminEntry_t *data;
    SR_UINT32   buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;

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

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaNodeAdminEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaNodeAdminName
    case I_snaNodeAdminName:
       dp = (void *) (CloneOctetString(data->snaNodeAdminName));
       break;
#endif /* I_snaNodeAdminName */

#ifdef I_snaNodeAdminType
    case I_snaNodeAdminType:
       dp = (void *) (&data->snaNodeAdminType);
       break;
#endif /* I_snaNodeAdminType */

#ifdef I_snaNodeAdminXidFormat
    case I_snaNodeAdminXidFormat:
       dp = (void *) (&data->snaNodeAdminXidFormat);
       break;
#endif /* I_snaNodeAdminXidFormat */

#ifdef I_snaNodeAdminBlockNum
    case I_snaNodeAdminBlockNum:
       dp = (void *) (CloneOctetString(data->snaNodeAdminBlockNum));
       break;
#endif /* I_snaNodeAdminBlockNum */

#ifdef I_snaNodeAdminIdNum
    case I_snaNodeAdminIdNum:
       dp = (void *) (CloneOctetString(data->snaNodeAdminIdNum));
       break;
#endif /* I_snaNodeAdminIdNum */

#ifdef I_snaNodeAdminEnablingMethod
    case I_snaNodeAdminEnablingMethod:
       dp = (void *) (&data->snaNodeAdminEnablingMethod);
       break;
#endif /* I_snaNodeAdminEnablingMethod */

#ifdef I_snaNodeAdminLuTermDefault
    case I_snaNodeAdminLuTermDefault:
       dp = (void *) (&data->snaNodeAdminLuTermDefault);
       break;
#endif /* I_snaNodeAdminLuTermDefault */

#ifdef I_snaNodeAdminMaxLu
    case I_snaNodeAdminMaxLu:
       dp = (void *) (&data->snaNodeAdminMaxLu);
       break;
#endif /* I_snaNodeAdminMaxLu */

#ifdef I_snaNodeAdminHostDescription
    case I_snaNodeAdminHostDescription:
       dp = (void *) (CloneOctetString(data->snaNodeAdminHostDescription));
       break;
#endif /* I_snaNodeAdminHostDescription */

#ifdef I_snaNodeAdminStopMethod
    case I_snaNodeAdminStopMethod:
       dp = (void *) (&data->snaNodeAdminStopMethod);
       break;
#endif /* I_snaNodeAdminStopMethod */

#ifdef I_snaNodeAdminState
    case I_snaNodeAdminState:
       dp = (void *) (&data->snaNodeAdminState);
       break;
#endif /* I_snaNodeAdminState */

#ifdef I_snaNodeAdminRowStatus
    case I_snaNodeAdminRowStatus:
       dp = (void *) (&data->snaNodeAdminRowStatus);
       break;
#endif /* I_snaNodeAdminRowStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the snaNodeAdminEntry data object.
 *---------------------------------------------------------------------*/
void
snaNodeAdminEntry_free(data)
   snaNodeAdminEntry_t *data;
{
   if (data != NULL) {
       FreeOctetString(data->snaNodeAdminName);
       FreeOctetString(data->snaNodeAdminBlockNum);
       FreeOctetString(data->snaNodeAdminIdNum);
       FreeOctetString(data->snaNodeAdminHostDescription);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after snaNodeAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int snaNodeAdminEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
snaNodeAdminEntry_cleanup(trash)
   doList_t *trash;
{
   snaNodeAdminEntry_free((snaNodeAdminEntry_t *) trash->data);
   snaNodeAdminEntry_free((snaNodeAdminEntry_t *) trash->undodata);
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the snaNodeAdminEntry family
 *---------------------------------------------------------------------*/
snaNodeAdminEntry_t *
Clone_snaNodeAdminEntry(snaNodeAdminEntry_t *snaNodeAdminEntry)
{
    snaNodeAdminEntry_t *data;

    if (snaNodeAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(snaNodeAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (snaNodeAdminEntry), sizeof(snaNodeAdminEntry_t));

    data->snaNodeAdminName = CloneOctetString(snaNodeAdminEntry->snaNodeAdminName);
    data->snaNodeAdminBlockNum = CloneOctetString(snaNodeAdminEntry->snaNodeAdminBlockNum);
    data->snaNodeAdminIdNum = CloneOctetString(snaNodeAdminEntry->snaNodeAdminIdNum);
    data->snaNodeAdminHostDescription = CloneOctetString(snaNodeAdminEntry->snaNodeAdminHostDescription);

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
snaNodeAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    snaNodeAdminEntry_t     *snaNodeAdminEntry;
    SR_INT32       snaNodeAdminIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    snaNodeAdminEntry = k_snaNodeAdminEntry_get(-1, contextInfo, -1, EXACT, snaNodeAdminIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == snaNodeAdminEntry_set) &&
            (((snaNodeAdminEntry_t *) (dp->data)) != NULL) &&
            (((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminIndex == snaNodeAdminIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = snaNodeAdminEntry_set;
        dp->cleanupMethod = snaNodeAdminEntry_cleanup;
#ifdef SR_snaNodeAdminEntry_UNDO
        dp->undoMethod = snaNodeAdminEntry_undo;
#else /* SR_snaNodeAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_snaNodeAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (snaNodeAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_snaNodeAdminEntry(snaNodeAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_snaNodeAdminEntry(snaNodeAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = malloc(sizeof(snaNodeAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(snaNodeAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminIndex = (snaNodeAdminIndex);
                SET_VALID(I_snaNodeAdminIndex, ((snaNodeAdminEntry_t *) (dp->data))->valid);

                error_status = k_snaNodeAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_snaNodeAdminName
   case I_snaNodeAdminName:

     if (!IsDisplayString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminName)) {
        return (WRONG_VALUE_ERROR);
     }

     if (((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminName != NULL) {
        FreeOctetString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminName);
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_snaNodeAdminName */

#ifdef I_snaNodeAdminType
   case I_snaNodeAdminType:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminType = value->sl_value;
     break;
#endif /* I_snaNodeAdminType */

#ifdef I_snaNodeAdminXidFormat
   case I_snaNodeAdminXidFormat:

     if ((value->sl_value < 1) || (value->sl_value > 3)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminXidFormat = value->sl_value;
     break;
#endif /* I_snaNodeAdminXidFormat */

#ifdef I_snaNodeAdminBlockNum
   case I_snaNodeAdminBlockNum:

     if (!IsDisplayString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminBlockNum)) {
        return (WRONG_VALUE_ERROR);
     }

     if (((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminBlockNum != NULL) {
        FreeOctetString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminBlockNum);
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminBlockNum = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_snaNodeAdminBlockNum */

#ifdef I_snaNodeAdminIdNum
   case I_snaNodeAdminIdNum:

     if (!IsDisplayString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminIdNum)) {
        return (WRONG_VALUE_ERROR);
     }

     if (((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminIdNum != NULL) {
        FreeOctetString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminIdNum);
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminIdNum = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_snaNodeAdminIdNum */

#ifdef I_snaNodeAdminEnablingMethod
   case I_snaNodeAdminEnablingMethod:

     if ((value->sl_value < 1) || (value->sl_value > 4)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminEnablingMethod = value->sl_value;
     break;
#endif /* I_snaNodeAdminEnablingMethod */

#ifdef I_snaNodeAdminLuTermDefault
   case I_snaNodeAdminLuTermDefault:

     if ((value->sl_value < 1) || (value->sl_value > 4)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminLuTermDefault = value->sl_value;
     break;
#endif /* I_snaNodeAdminLuTermDefault */

#ifdef I_snaNodeAdminMaxLu
   case I_snaNodeAdminMaxLu:

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminMaxLu = value->sl_value;
     break;
#endif /* I_snaNodeAdminMaxLu */

#ifdef I_snaNodeAdminHostDescription
   case I_snaNodeAdminHostDescription:

     if (!IsDisplayString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminHostDescription)) {
        return (WRONG_VALUE_ERROR);
     }

     if (((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminHostDescription != NULL) {
        FreeOctetString(((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminHostDescription);
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminHostDescription = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_snaNodeAdminHostDescription */

#ifdef I_snaNodeAdminStopMethod
   case I_snaNodeAdminStopMethod:

     if ((value->sl_value < 1) || (value->sl_value > 4)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminStopMethod = value->sl_value;
     break;
#endif /* I_snaNodeAdminStopMethod */

#ifdef I_snaNodeAdminState
   case I_snaNodeAdminState:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminState = value->sl_value;
     break;
#endif /* I_snaNodeAdminState */

#ifdef I_snaNodeAdminRowStatus
   case I_snaNodeAdminRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_snaNodeAdminRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (snaNodeAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_snaNodeAdminRowStatus_notInService) || (value->sl_value == D_snaNodeAdminRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_snaNodeAdminRowStatus_createAndGo) || (value->sl_value == D_snaNodeAdminRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((snaNodeAdminEntry_t *) (dp->data))->snaNodeAdminRowStatus = value->sl_value;
     break;
#endif /* I_snaNodeAdminRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in snaNodeAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_snaNodeAdminEntry_test */
   error_status = k_snaNodeAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((snaNodeAdminEntry_t *) (dp->data))->valid);
        error_status = k_snaNodeAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snaNodeAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_snaNodeAdminEntry_set((snaNodeAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the snaNodeOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaNodeOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaNodeOperEntry_t *data;
    SR_UINT32   buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;

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

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaNodeOperEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaNodeOperName
    case I_snaNodeOperName:
       dp = (void *) (CloneOctetString(data->snaNodeOperName));
       break;
#endif /* I_snaNodeOperName */

#ifdef I_snaNodeOperType
    case I_snaNodeOperType:
       dp = (void *) (&data->snaNodeOperType);
       break;
#endif /* I_snaNodeOperType */

#ifdef I_snaNodeOperXidFormat
    case I_snaNodeOperXidFormat:
       dp = (void *) (&data->snaNodeOperXidFormat);
       break;
#endif /* I_snaNodeOperXidFormat */

#ifdef I_snaNodeOperBlockNum
    case I_snaNodeOperBlockNum:
       dp = (void *) (CloneOctetString(data->snaNodeOperBlockNum));
       break;
#endif /* I_snaNodeOperBlockNum */

#ifdef I_snaNodeOperIdNum
    case I_snaNodeOperIdNum:
       dp = (void *) (CloneOctetString(data->snaNodeOperIdNum));
       break;
#endif /* I_snaNodeOperIdNum */

#ifdef I_snaNodeOperEnablingMethod
    case I_snaNodeOperEnablingMethod:
       dp = (void *) (&data->snaNodeOperEnablingMethod);
       break;
#endif /* I_snaNodeOperEnablingMethod */

#ifdef I_snaNodeOperLuTermDefault
    case I_snaNodeOperLuTermDefault:
       dp = (void *) (&data->snaNodeOperLuTermDefault);
       break;
#endif /* I_snaNodeOperLuTermDefault */

#ifdef I_snaNodeOperMaxLu
    case I_snaNodeOperMaxLu:
       dp = (void *) (&data->snaNodeOperMaxLu);
       break;
#endif /* I_snaNodeOperMaxLu */

#ifdef I_snaNodeOperHostDescription
    case I_snaNodeOperHostDescription:
       dp = (void *) (CloneOctetString(data->snaNodeOperHostDescription));
       break;
#endif /* I_snaNodeOperHostDescription */

#ifdef I_snaNodeOperStopMethod
    case I_snaNodeOperStopMethod:
       dp = (void *) (&data->snaNodeOperStopMethod);
       break;
#endif /* I_snaNodeOperStopMethod */

#ifdef I_snaNodeOperState
    case I_snaNodeOperState:
       dp = (void *) (&data->snaNodeOperState);
       break;
#endif /* I_snaNodeOperState */

#ifdef I_snaNodeOperHostSscpId
    case I_snaNodeOperHostSscpId:
       dp = (void *) (CloneOctetString(data->snaNodeOperHostSscpId));
       break;
#endif /* I_snaNodeOperHostSscpId */

#ifdef I_snaNodeOperStartTime
    case I_snaNodeOperStartTime:
       dp = (void *) (&data->snaNodeOperStartTime);
       break;
#endif /* I_snaNodeOperStartTime */

#ifdef I_snaNodeOperLastStateChange
    case I_snaNodeOperLastStateChange:
       dp = (void *) (&data->snaNodeOperLastStateChange);
       break;
#endif /* I_snaNodeOperLastStateChange */

#ifdef I_snaNodeOperActFailures
    case I_snaNodeOperActFailures:
       dp = (void *) (&data->snaNodeOperActFailures);
       break;
#endif /* I_snaNodeOperActFailures */

#ifdef I_snaNodeOperActFailureReason
    case I_snaNodeOperActFailureReason:
       dp = (void *) (&data->snaNodeOperActFailureReason);
       break;
#endif /* I_snaNodeOperActFailureReason */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the snaPu20StatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaPu20StatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaPu20StatsEntry_t *data;
    SR_UINT32   buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;

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

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaPu20StatsEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaPu20StatsSentBytes
    case I_snaPu20StatsSentBytes:
       dp = (void *) (&data->snaPu20StatsSentBytes);
       break;
#endif /* I_snaPu20StatsSentBytes */

#ifdef I_snaPu20StatsReceivedBytes
    case I_snaPu20StatsReceivedBytes:
       dp = (void *) (&data->snaPu20StatsReceivedBytes);
       break;
#endif /* I_snaPu20StatsReceivedBytes */

#ifdef I_snaPu20StatsSentPius
    case I_snaPu20StatsSentPius:
       dp = (void *) (&data->snaPu20StatsSentPius);
       break;
#endif /* I_snaPu20StatsSentPius */

#ifdef I_snaPu20StatsReceivedPius
    case I_snaPu20StatsReceivedPius:
       dp = (void *) (&data->snaPu20StatsReceivedPius);
       break;
#endif /* I_snaPu20StatsReceivedPius */

#ifdef I_snaPu20StatsSentNegativeResps
    case I_snaPu20StatsSentNegativeResps:
       dp = (void *) (&data->snaPu20StatsSentNegativeResps);
       break;
#endif /* I_snaPu20StatsSentNegativeResps */

#ifdef I_snaPu20StatsReceivedNegativeResps
    case I_snaPu20StatsReceivedNegativeResps:
       dp = (void *) (&data->snaPu20StatsReceivedNegativeResps);
       break;
#endif /* I_snaPu20StatsReceivedNegativeResps */

#ifdef I_snaPu20StatsActLus
    case I_snaPu20StatsActLus:
       dp = (void *) (&data->snaPu20StatsActLus);
       break;
#endif /* I_snaPu20StatsActLus */

#ifdef I_snaPu20StatsInActLus
    case I_snaPu20StatsInActLus:
       dp = (void *) (&data->snaPu20StatsInActLus);
       break;
#endif /* I_snaPu20StatsInActLus */

#ifdef I_snaPu20StatsBindLus
    case I_snaPu20StatsBindLus:
       dp = (void *) (&data->snaPu20StatsBindLus);
       break;
#endif /* I_snaPu20StatsBindLus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the snaNodeLinkAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaNodeLinkAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaNodeLinkAdminEntry_t *data;
    SR_UINT32   buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;
    SR_INT32        snaNodeLinkAdminIndex;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaNodeLinkAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaNodeLinkAdminEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex, snaNodeLinkAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
        inst.oid_ptr[1] = data->snaNodeLinkAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaNodeLinkAdminSpecific
    case I_snaNodeLinkAdminSpecific:
       dp = (void *) (CloneOID(data->snaNodeLinkAdminSpecific));
       break;
#endif /* I_snaNodeLinkAdminSpecific */

#ifdef I_snaNodeLinkAdminMaxPiu
    case I_snaNodeLinkAdminMaxPiu:
       dp = (void *) (&data->snaNodeLinkAdminMaxPiu);
       break;
#endif /* I_snaNodeLinkAdminMaxPiu */

#ifdef I_snaNodeLinkAdminRowStatus
    case I_snaNodeLinkAdminRowStatus:
       dp = (void *) (&data->snaNodeLinkAdminRowStatus);
       break;
#endif /* I_snaNodeLinkAdminRowStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the snaNodeLinkAdminEntry data object.
 *---------------------------------------------------------------------*/
void
snaNodeLinkAdminEntry_free(data)
   snaNodeLinkAdminEntry_t *data;
{
   if (data != NULL) {
       FreeOID(data->snaNodeLinkAdminSpecific);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after snaNodeLinkAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int snaNodeLinkAdminEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
snaNodeLinkAdminEntry_cleanup(trash)
   doList_t *trash;
{
   snaNodeLinkAdminEntry_free((snaNodeLinkAdminEntry_t *) trash->data);
   snaNodeLinkAdminEntry_free((snaNodeLinkAdminEntry_t *) trash->undodata);
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the snaNodeLinkAdminEntry family
 *---------------------------------------------------------------------*/
snaNodeLinkAdminEntry_t *
Clone_snaNodeLinkAdminEntry(snaNodeLinkAdminEntry_t *snaNodeLinkAdminEntry)
{
    snaNodeLinkAdminEntry_t *data;

    if (snaNodeLinkAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(snaNodeLinkAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (snaNodeLinkAdminEntry), sizeof(snaNodeLinkAdminEntry_t));

    data->snaNodeLinkAdminSpecific = CloneOID(snaNodeLinkAdminEntry->snaNodeLinkAdminSpecific);

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
snaNodeLinkAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    snaNodeLinkAdminEntry_t     *snaNodeLinkAdminEntry;
    SR_INT32       snaNodeAdminIndex;
    SR_INT32       snaNodeLinkAdminIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 2) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaNodeLinkAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    snaNodeLinkAdminEntry = k_snaNodeLinkAdminEntry_get(-1, contextInfo, -1, EXACT, snaNodeAdminIndex, snaNodeLinkAdminIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == snaNodeLinkAdminEntry_set) &&
            (((snaNodeLinkAdminEntry_t *) (dp->data)) != NULL) &&
            (((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeAdminIndex == snaNodeAdminIndex) &&
            (((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeLinkAdminIndex == snaNodeLinkAdminIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = snaNodeLinkAdminEntry_set;
        dp->cleanupMethod = snaNodeLinkAdminEntry_cleanup;
#ifdef SR_snaNodeLinkAdminEntry_UNDO
        dp->undoMethod = snaNodeLinkAdminEntry_undo;
#else /* SR_snaNodeLinkAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_snaNodeLinkAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (snaNodeLinkAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_snaNodeLinkAdminEntry(snaNodeLinkAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_snaNodeLinkAdminEntry(snaNodeLinkAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = malloc(sizeof(snaNodeLinkAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(snaNodeLinkAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeAdminIndex = (snaNodeAdminIndex);
                SET_VALID(I_snaNodeLinkAdminEntryIndex_snaNodeAdminIndex, ((snaNodeLinkAdminEntry_t *) (dp->data))->valid);

                ((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeLinkAdminIndex = (snaNodeLinkAdminIndex);
                SET_VALID(I_snaNodeLinkAdminIndex, ((snaNodeLinkAdminEntry_t *) (dp->data))->valid);

                error_status = k_snaNodeLinkAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_snaNodeLinkAdminSpecific
   case I_snaNodeLinkAdminSpecific:

     if (((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeLinkAdminSpecific != NULL) {
        FreeOID(((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeLinkAdminSpecific);
     }

     ((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeLinkAdminSpecific = 
            CloneOID(value->oid_value);

     break;
#endif /* I_snaNodeLinkAdminSpecific */

#ifdef I_snaNodeLinkAdminMaxPiu
   case I_snaNodeLinkAdminMaxPiu:

     ((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeLinkAdminMaxPiu = value->sl_value;
     break;
#endif /* I_snaNodeLinkAdminMaxPiu */

#ifdef I_snaNodeLinkAdminRowStatus
   case I_snaNodeLinkAdminRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_snaNodeLinkAdminRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (snaNodeLinkAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_snaNodeLinkAdminRowStatus_notInService) || (value->sl_value == D_snaNodeLinkAdminRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_snaNodeLinkAdminRowStatus_createAndGo) || (value->sl_value == D_snaNodeLinkAdminRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((snaNodeLinkAdminEntry_t *) (dp->data))->snaNodeLinkAdminRowStatus = value->sl_value;
     break;
#endif /* I_snaNodeLinkAdminRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in snaNodeLinkAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_snaNodeLinkAdminEntry_test */
   error_status = k_snaNodeLinkAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((snaNodeLinkAdminEntry_t *) (dp->data))->valid);
        error_status = k_snaNodeLinkAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snaNodeLinkAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_snaNodeLinkAdminEntry_set((snaNodeLinkAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the snaNodeLinkOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaNodeLinkOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaNodeLinkOperEntry_t *data;
    SR_UINT32   buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;
    SR_INT32        snaNodeLinkAdminIndex;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaNodeLinkAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaNodeLinkOperEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex, snaNodeLinkAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
        inst.oid_ptr[1] = data->snaNodeLinkAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaNodeLinkOperSpecific
    case I_snaNodeLinkOperSpecific:
       dp = (void *) (CloneOID(data->snaNodeLinkOperSpecific));
       break;
#endif /* I_snaNodeLinkOperSpecific */

#ifdef I_snaNodeLinkOperMaxPiu
    case I_snaNodeLinkOperMaxPiu:
       dp = (void *) (&data->snaNodeLinkOperMaxPiu);
       break;
#endif /* I_snaNodeLinkOperMaxPiu */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the snaLuAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaLuAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaLuAdminEntry_t *data;
    SR_UINT32   buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;
    SR_INT32        snaLuAdminLuIndex;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaLuAdminLuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaLuAdminEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex, snaLuAdminLuIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
        inst.oid_ptr[1] = data->snaLuAdminLuIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaLuAdminName
    case I_snaLuAdminName:
       dp = (void *) (CloneOctetString(data->snaLuAdminName));
       break;
#endif /* I_snaLuAdminName */

#ifdef I_snaLuAdminSnaName
    case I_snaLuAdminSnaName:
       dp = (void *) (CloneOctetString(data->snaLuAdminSnaName));
       break;
#endif /* I_snaLuAdminSnaName */

#ifdef I_snaLuAdminType
    case I_snaLuAdminType:
       dp = (void *) (&data->snaLuAdminType);
       break;
#endif /* I_snaLuAdminType */

#ifdef I_snaLuAdminDepType
    case I_snaLuAdminDepType:
       dp = (void *) (&data->snaLuAdminDepType);
       break;
#endif /* I_snaLuAdminDepType */

#ifdef I_snaLuAdminLocalAddress
    case I_snaLuAdminLocalAddress:
       dp = (void *) (CloneOctetString(data->snaLuAdminLocalAddress));
       break;
#endif /* I_snaLuAdminLocalAddress */

#ifdef I_snaLuAdminDisplayModel
    case I_snaLuAdminDisplayModel:
       dp = (void *) (&data->snaLuAdminDisplayModel);
       break;
#endif /* I_snaLuAdminDisplayModel */

#ifdef I_snaLuAdminTerm
    case I_snaLuAdminTerm:
       dp = (void *) (&data->snaLuAdminTerm);
       break;
#endif /* I_snaLuAdminTerm */

#ifdef I_snaLuAdminRowStatus
    case I_snaLuAdminRowStatus:
       dp = (void *) (&data->snaLuAdminRowStatus);
       break;
#endif /* I_snaLuAdminRowStatus */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the snaLuAdminEntry data object.
 *---------------------------------------------------------------------*/
void
snaLuAdminEntry_free(data)
   snaLuAdminEntry_t *data;
{
   if (data != NULL) {
       FreeOctetString(data->snaLuAdminName);
       FreeOctetString(data->snaLuAdminSnaName);
       FreeOctetString(data->snaLuAdminLocalAddress);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after snaLuAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int snaLuAdminEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
snaLuAdminEntry_cleanup(trash)
   doList_t *trash;
{
   snaLuAdminEntry_free((snaLuAdminEntry_t *) trash->data);
   snaLuAdminEntry_free((snaLuAdminEntry_t *) trash->undodata);
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the snaLuAdminEntry family
 *---------------------------------------------------------------------*/
snaLuAdminEntry_t *
Clone_snaLuAdminEntry(snaLuAdminEntry_t *snaLuAdminEntry)
{
    snaLuAdminEntry_t *data;

    if (snaLuAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(snaLuAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (snaLuAdminEntry), sizeof(snaLuAdminEntry_t));

    data->snaLuAdminName = CloneOctetString(snaLuAdminEntry->snaLuAdminName);
    data->snaLuAdminSnaName = CloneOctetString(snaLuAdminEntry->snaLuAdminSnaName);
    data->snaLuAdminLocalAddress = CloneOctetString(snaLuAdminEntry->snaLuAdminLocalAddress);

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
snaLuAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    snaLuAdminEntry_t     *snaLuAdminEntry;
    SR_INT32       snaNodeAdminIndex;
    SR_INT32       snaLuAdminLuIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 2) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaLuAdminLuIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    snaLuAdminEntry = k_snaLuAdminEntry_get(-1, contextInfo, -1, EXACT, snaNodeAdminIndex, snaLuAdminLuIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == snaLuAdminEntry_set) &&
            (((snaLuAdminEntry_t *) (dp->data)) != NULL) &&
            (((snaLuAdminEntry_t *) (dp->data))->snaNodeAdminIndex == snaNodeAdminIndex) &&
            (((snaLuAdminEntry_t *) (dp->data))->snaLuAdminLuIndex == snaLuAdminLuIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = snaLuAdminEntry_set;
        dp->cleanupMethod = snaLuAdminEntry_cleanup;
#ifdef SR_snaLuAdminEntry_UNDO
        dp->undoMethod = snaLuAdminEntry_undo;
#else /* SR_snaLuAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_snaLuAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (snaLuAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_snaLuAdminEntry(snaLuAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_snaLuAdminEntry(snaLuAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = malloc(sizeof(snaLuAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(snaLuAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((snaLuAdminEntry_t *) (dp->data))->snaNodeAdminIndex = (snaNodeAdminIndex);
                SET_VALID(I_snaLuAdminEntryIndex_snaNodeAdminIndex, ((snaLuAdminEntry_t *) (dp->data))->valid);

                ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminLuIndex = (snaLuAdminLuIndex);
                SET_VALID(I_snaLuAdminLuIndex, ((snaLuAdminEntry_t *) (dp->data))->valid);

                error_status = k_snaLuAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_snaLuAdminName
   case I_snaLuAdminName:

     if (!IsDisplayString(((snaLuAdminEntry_t *) (dp->data))->snaLuAdminName)) {
        return (WRONG_VALUE_ERROR);
     }

     if (((snaLuAdminEntry_t *) (dp->data))->snaLuAdminName != NULL) {
        FreeOctetString(((snaLuAdminEntry_t *) (dp->data))->snaLuAdminName);
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_snaLuAdminName */

#ifdef I_snaLuAdminSnaName
   case I_snaLuAdminSnaName:

     if (!IsDisplayString(((snaLuAdminEntry_t *) (dp->data))->snaLuAdminSnaName)) {
        return (WRONG_VALUE_ERROR);
     }

     if (((snaLuAdminEntry_t *) (dp->data))->snaLuAdminSnaName != NULL) {
        FreeOctetString(((snaLuAdminEntry_t *) (dp->data))->snaLuAdminSnaName);
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminSnaName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_snaLuAdminSnaName */

#ifdef I_snaLuAdminType
   case I_snaLuAdminType:

     if ((value->sl_value < 1) || (value->sl_value > 8)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminType = value->sl_value;
     break;
#endif /* I_snaLuAdminType */

#ifdef I_snaLuAdminDepType
   case I_snaLuAdminDepType:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminDepType = value->sl_value;
     break;
#endif /* I_snaLuAdminDepType */

#ifdef I_snaLuAdminLocalAddress
   case I_snaLuAdminLocalAddress:

     if (((snaLuAdminEntry_t *) (dp->data))->snaLuAdminLocalAddress != NULL) {
        FreeOctetString(((snaLuAdminEntry_t *) (dp->data))->snaLuAdminLocalAddress);
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminLocalAddress = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_snaLuAdminLocalAddress */

#ifdef I_snaLuAdminDisplayModel
   case I_snaLuAdminDisplayModel:

     if ((value->sl_value < 1) || (value->sl_value > 10)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminDisplayModel = value->sl_value;
     break;
#endif /* I_snaLuAdminDisplayModel */

#ifdef I_snaLuAdminTerm
   case I_snaLuAdminTerm:

     if ((value->sl_value < 1) || (value->sl_value > 4)) {
         return WRONG_VALUE_ERROR;
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminTerm = value->sl_value;
     break;
#endif /* I_snaLuAdminTerm */

#ifdef I_snaLuAdminRowStatus
   case I_snaLuAdminRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_snaLuAdminRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (snaLuAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_snaLuAdminRowStatus_notInService) || (value->sl_value == D_snaLuAdminRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_snaLuAdminRowStatus_createAndGo) || (value->sl_value == D_snaLuAdminRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((snaLuAdminEntry_t *) (dp->data))->snaLuAdminRowStatus = value->sl_value;
     break;
#endif /* I_snaLuAdminRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in snaLuAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_snaLuAdminEntry_test */
   error_status = k_snaLuAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((snaLuAdminEntry_t *) (dp->data))->valid);
        error_status = k_snaLuAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snaLuAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_snaLuAdminEntry_set((snaLuAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the snaLuOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaLuOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaLuOperEntry_t *data;
    SR_UINT32   buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;
    SR_INT32        snaLuAdminLuIndex;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaLuAdminLuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaLuOperEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex, snaLuAdminLuIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
        inst.oid_ptr[1] = data->snaLuAdminLuIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaLuOperName
    case I_snaLuOperName:
       dp = (void *) (CloneOctetString(data->snaLuOperName));
       break;
#endif /* I_snaLuOperName */

#ifdef I_snaLuOperSnaName
    case I_snaLuOperSnaName:
       dp = (void *) (CloneOctetString(data->snaLuOperSnaName));
       break;
#endif /* I_snaLuOperSnaName */

#ifdef I_snaLuOperType
    case I_snaLuOperType:
       dp = (void *) (&data->snaLuOperType);
       break;
#endif /* I_snaLuOperType */

#ifdef I_snaLuOperDepType
    case I_snaLuOperDepType:
       dp = (void *) (&data->snaLuOperDepType);
       break;
#endif /* I_snaLuOperDepType */

#ifdef I_snaLuOperLocalAddress
    case I_snaLuOperLocalAddress:
       dp = (void *) (CloneOctetString(data->snaLuOperLocalAddress));
       break;
#endif /* I_snaLuOperLocalAddress */

#ifdef I_snaLuOperDisplayModel
    case I_snaLuOperDisplayModel:
       dp = (void *) (&data->snaLuOperDisplayModel);
       break;
#endif /* I_snaLuOperDisplayModel */

#ifdef I_snaLuOperTerm
    case I_snaLuOperTerm:
       dp = (void *) (&data->snaLuOperTerm);
       break;
#endif /* I_snaLuOperTerm */

#ifdef I_snaLuOperState
    case I_snaLuOperState:
       dp = (void *) (&data->snaLuOperState);
       break;
#endif /* I_snaLuOperState */

#ifdef I_snaLuOperSessnCount
    case I_snaLuOperSessnCount:
       dp = (void *) (&data->snaLuOperSessnCount);
       break;
#endif /* I_snaLuOperSessnCount */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the snaLuSessnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaLuSessnEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaLuSessnEntry_t *data;
    SR_UINT32   buffer[4];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;
    SR_INT32        snaLuAdminLuIndex;
    SR_INT32        snaLuSessnRluIndex;
    SR_INT32        snaLuSessnIndex;

    data = NULL;
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

    if ( (InstToInt(incoming, 3 + object->oid.length, &snaLuSessnIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &snaLuSessnRluIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaLuAdminLuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaLuSessnEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex, snaLuAdminLuIndex, snaLuSessnRluIndex, snaLuSessnIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 4;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
        inst.oid_ptr[1] = data->snaLuAdminLuIndex;
        inst.oid_ptr[2] = data->snaLuSessnRluIndex;
        inst.oid_ptr[3] = data->snaLuSessnIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaLuSessnRluIndex
    case I_snaLuSessnRluIndex:
       dp = (void *) (&data->snaLuSessnRluIndex);
       break;
#endif /* I_snaLuSessnRluIndex */

#ifdef I_snaLuSessnIndex
    case I_snaLuSessnIndex:
       dp = (void *) (&data->snaLuSessnIndex);
       break;
#endif /* I_snaLuSessnIndex */

#ifdef I_snaLuSessnLocalApplName
    case I_snaLuSessnLocalApplName:
       dp = (void *) (CloneOctetString(data->snaLuSessnLocalApplName));
       break;
#endif /* I_snaLuSessnLocalApplName */

#ifdef I_snaLuSessnRemoteLuName
    case I_snaLuSessnRemoteLuName:
       dp = (void *) (CloneOctetString(data->snaLuSessnRemoteLuName));
       break;
#endif /* I_snaLuSessnRemoteLuName */

#ifdef I_snaLuSessnMaxSndRuSize
    case I_snaLuSessnMaxSndRuSize:
       dp = (void *) (&data->snaLuSessnMaxSndRuSize);
       break;
#endif /* I_snaLuSessnMaxSndRuSize */

#ifdef I_snaLuSessnMaxRcvRuSize
    case I_snaLuSessnMaxRcvRuSize:
       dp = (void *) (&data->snaLuSessnMaxRcvRuSize);
       break;
#endif /* I_snaLuSessnMaxRcvRuSize */

#ifdef I_snaLuSessnSndPacingSize
    case I_snaLuSessnSndPacingSize:
       dp = (void *) (&data->snaLuSessnSndPacingSize);
       break;
#endif /* I_snaLuSessnSndPacingSize */

#ifdef I_snaLuSessnRcvPacingSize
    case I_snaLuSessnRcvPacingSize:
       dp = (void *) (&data->snaLuSessnRcvPacingSize);
       break;
#endif /* I_snaLuSessnRcvPacingSize */

#ifdef I_snaLuSessnActiveTime
    case I_snaLuSessnActiveTime:
       dp = (void *) (&data->snaLuSessnActiveTime);
       break;
#endif /* I_snaLuSessnActiveTime */

#ifdef I_snaLuSessnAdminState
    case I_snaLuSessnAdminState:
       dp = (void *) (&data->snaLuSessnAdminState);
       break;
#endif /* I_snaLuSessnAdminState */

#ifdef I_snaLuSessnOperState
    case I_snaLuSessnOperState:
       dp = (void *) (&data->snaLuSessnOperState);
       break;
#endif /* I_snaLuSessnOperState */

#ifdef I_snaLuSessnSenseData
    case I_snaLuSessnSenseData:
       dp = (void *) (CloneOctetString(data->snaLuSessnSenseData));
       break;
#endif /* I_snaLuSessnSenseData */

#ifdef I_snaLuSessnTerminationRu
    case I_snaLuSessnTerminationRu:
       dp = (void *) (&data->snaLuSessnTerminationRu);
       break;
#endif /* I_snaLuSessnTerminationRu */

#ifdef I_snaLuSessnUnbindType
    case I_snaLuSessnUnbindType:
       dp = (void *) (CloneOctetString(data->snaLuSessnUnbindType));
       break;
#endif /* I_snaLuSessnUnbindType */

#ifdef I_snaLuSessnLinkIndex
    case I_snaLuSessnLinkIndex:
       dp = (void *) (&data->snaLuSessnLinkIndex);
       break;
#endif /* I_snaLuSessnLinkIndex */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the snaLuSessnEntry data object.
 *---------------------------------------------------------------------*/
void
snaLuSessnEntry_free(data)
   snaLuSessnEntry_t *data;
{
   if (data != NULL) {
       FreeOctetString(data->snaLuSessnLocalApplName);
       FreeOctetString(data->snaLuSessnRemoteLuName);
       FreeOctetString(data->snaLuSessnSenseData);
       FreeOctetString(data->snaLuSessnUnbindType);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after snaLuSessnEntry set/undo
 *---------------------------------------------------------------------*/
static int snaLuSessnEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
snaLuSessnEntry_cleanup(trash)
   doList_t *trash;
{
   snaLuSessnEntry_free((snaLuSessnEntry_t *) trash->data);
   snaLuSessnEntry_free((snaLuSessnEntry_t *) trash->undodata);
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the snaLuSessnEntry family
 *---------------------------------------------------------------------*/
snaLuSessnEntry_t *
Clone_snaLuSessnEntry(snaLuSessnEntry_t *snaLuSessnEntry)
{
    snaLuSessnEntry_t *data;

    if (snaLuSessnEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(snaLuSessnEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (snaLuSessnEntry), sizeof(snaLuSessnEntry_t));

    data->snaLuSessnLocalApplName = CloneOctetString(snaLuSessnEntry->snaLuSessnLocalApplName);
    data->snaLuSessnRemoteLuName = CloneOctetString(snaLuSessnEntry->snaLuSessnRemoteLuName);
    data->snaLuSessnSenseData = CloneOctetString(snaLuSessnEntry->snaLuSessnSenseData);
    data->snaLuSessnUnbindType = CloneOctetString(snaLuSessnEntry->snaLuSessnUnbindType);

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
snaLuSessnEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    snaLuSessnEntry_t     *snaLuSessnEntry;
    SR_INT32       snaNodeAdminIndex;
    SR_INT32       snaLuAdminLuIndex;
    SR_INT32       snaLuSessnRluIndex;
    SR_INT32       snaLuSessnIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 4) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaLuAdminLuIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &snaLuSessnRluIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 3 + object->oid.length, &snaLuSessnIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    snaLuSessnEntry = k_snaLuSessnEntry_get(-1, contextInfo, -1, EXACT, snaNodeAdminIndex, snaLuAdminLuIndex, snaLuSessnRluIndex, snaLuSessnIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == snaLuSessnEntry_set) &&
            (((snaLuSessnEntry_t *) (dp->data)) != NULL) &&
            (((snaLuSessnEntry_t *) (dp->data))->snaNodeAdminIndex == snaNodeAdminIndex) &&
            (((snaLuSessnEntry_t *) (dp->data))->snaLuAdminLuIndex == snaLuAdminLuIndex) &&
            (((snaLuSessnEntry_t *) (dp->data))->snaLuSessnRluIndex == snaLuSessnRluIndex) &&
            (((snaLuSessnEntry_t *) (dp->data))->snaLuSessnIndex == snaLuSessnIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = snaLuSessnEntry_set;
        dp->cleanupMethod = snaLuSessnEntry_cleanup;
#ifdef SR_snaLuSessnEntry_UNDO
        dp->undoMethod = snaLuSessnEntry_undo;
#else /* SR_snaLuSessnEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_snaLuSessnEntry_UNDO */
        dp->state = UNKNOWN;

        if (snaLuSessnEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_snaLuSessnEntry(snaLuSessnEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_snaLuSessnEntry(snaLuSessnEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {

#ifdef snaLuSessnEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(snaLuSessnEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(snaLuSessnEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((snaLuSessnEntry_t *) (dp->data))->snaNodeAdminIndex = (snaNodeAdminIndex);
                SET_VALID(I_snaLuSessnEntryIndex_snaNodeAdminIndex, ((snaLuSessnEntry_t *) (dp->data))->valid);

                ((snaLuSessnEntry_t *) (dp->data))->snaLuAdminLuIndex = (snaLuAdminLuIndex);
                SET_VALID(I_snaLuSessnEntryIndex_snaLuAdminLuIndex, ((snaLuSessnEntry_t *) (dp->data))->valid);

                ((snaLuSessnEntry_t *) (dp->data))->snaLuSessnRluIndex = (snaLuSessnRluIndex);
                SET_VALID(I_snaLuSessnRluIndex, ((snaLuSessnEntry_t *) (dp->data))->valid);

                ((snaLuSessnEntry_t *) (dp->data))->snaLuSessnIndex = (snaLuSessnIndex);
                SET_VALID(I_snaLuSessnIndex, ((snaLuSessnEntry_t *) (dp->data))->valid);

                error_status = k_snaLuSessnEntry_set_defaults(dp);
            }
#else /* snaLuSessnEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* snaLuSessnEntry_READ_CREATE */
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_snaLuSessnAdminState
   case I_snaLuSessnAdminState:

    switch (value->sl_value) {
        case 1:
        case 3:
            break;
        default:
            return WRONG_VALUE_ERROR;
    }

     ((snaLuSessnEntry_t *) (dp->data))->snaLuSessnAdminState = value->sl_value;
     break;
#endif /* I_snaLuSessnAdminState */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in snaLuSessnEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_snaLuSessnEntry_test */
   error_status = k_snaLuSessnEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((snaLuSessnEntry_t *) (dp->data))->valid);
        error_status = k_snaLuSessnEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
snaLuSessnEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_snaLuSessnEntry_set((snaLuSessnEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the snaLuSessnStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaLuSessnStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaLuSessnStatsEntry_t *data;
    SR_UINT32   buffer[4];
    OID             inst;
    int             carry;
    SR_INT32        snaNodeAdminIndex;
    SR_INT32        snaLuAdminLuIndex;
    SR_INT32        snaLuSessnRluIndex;
    SR_INT32        snaLuSessnIndex;

    data = NULL;
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

    if ( (InstToInt(incoming, 3 + object->oid.length, &snaLuSessnIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &snaLuSessnRluIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaLuAdminLuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaNodeAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaLuSessnStatsEntry_get(serialNum, contextInfo, arg ,searchType, snaNodeAdminIndex, snaLuAdminLuIndex, snaLuSessnRluIndex, snaLuSessnIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 4;
        inst.oid_ptr[0] = data->snaNodeAdminIndex;
        inst.oid_ptr[1] = data->snaLuAdminLuIndex;
        inst.oid_ptr[2] = data->snaLuSessnRluIndex;
        inst.oid_ptr[3] = data->snaLuSessnIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaLuSessnStatsSentBytes
    case I_snaLuSessnStatsSentBytes:
       dp = (void *) (&data->snaLuSessnStatsSentBytes);
       break;
#endif /* I_snaLuSessnStatsSentBytes */

#ifdef I_snaLuSessnStatsReceivedBytes
    case I_snaLuSessnStatsReceivedBytes:
       dp = (void *) (&data->snaLuSessnStatsReceivedBytes);
       break;
#endif /* I_snaLuSessnStatsReceivedBytes */

#ifdef I_snaLuSessnStatsSentRus
    case I_snaLuSessnStatsSentRus:
       dp = (void *) (&data->snaLuSessnStatsSentRus);
       break;
#endif /* I_snaLuSessnStatsSentRus */

#ifdef I_snaLuSessnStatsReceivedRus
    case I_snaLuSessnStatsReceivedRus:
       dp = (void *) (&data->snaLuSessnStatsReceivedRus);
       break;
#endif /* I_snaLuSessnStatsReceivedRus */

#ifdef I_snaLuSessnStatsSentNegativeResps
    case I_snaLuSessnStatsSentNegativeResps:
       dp = (void *) (&data->snaLuSessnStatsSentNegativeResps);
       break;
#endif /* I_snaLuSessnStatsSentNegativeResps */

#ifdef I_snaLuSessnStatsReceivedNegativeResps
    case I_snaLuSessnStatsReceivedNegativeResps:
       dp = (void *) (&data->snaLuSessnStatsReceivedNegativeResps);
       break;
#endif /* I_snaLuSessnStatsReceivedNegativeResps */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the snaLuRtmEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
snaLuRtmEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    snaLuRtmEntry_t *data;
    SR_UINT32   buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        snaLuRtmPuIndex;
    SR_INT32        snaLuRtmLuIndex;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &snaLuRtmLuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &snaLuRtmPuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_snaLuRtmEntry_get(serialNum, contextInfo, arg ,searchType, snaLuRtmPuIndex, snaLuRtmLuIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->snaLuRtmPuIndex;
        inst.oid_ptr[1] = data->snaLuRtmLuIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_snaLuRtmState
    case I_snaLuRtmState:
       dp = (void *) (&data->snaLuRtmState);
       break;
#endif /* I_snaLuRtmState */

#ifdef I_snaLuRtmStateTime
    case I_snaLuRtmStateTime:
       dp = (void *) (&data->snaLuRtmStateTime);
       break;
#endif /* I_snaLuRtmStateTime */

#ifdef I_snaLuRtmDef
    case I_snaLuRtmDef:
       dp = (void *) (&data->snaLuRtmDef);
       break;
#endif /* I_snaLuRtmDef */

#ifdef I_snaLuRtmBoundary1
    case I_snaLuRtmBoundary1:
       dp = (void *) (&data->snaLuRtmBoundary1);
       break;
#endif /* I_snaLuRtmBoundary1 */

#ifdef I_snaLuRtmBoundary2
    case I_snaLuRtmBoundary2:
       dp = (void *) (&data->snaLuRtmBoundary2);
       break;
#endif /* I_snaLuRtmBoundary2 */

#ifdef I_snaLuRtmBoundary3
    case I_snaLuRtmBoundary3:
       dp = (void *) (&data->snaLuRtmBoundary3);
       break;
#endif /* I_snaLuRtmBoundary3 */

#ifdef I_snaLuRtmBoundary4
    case I_snaLuRtmBoundary4:
       dp = (void *) (&data->snaLuRtmBoundary4);
       break;
#endif /* I_snaLuRtmBoundary4 */

#ifdef I_snaLuRtmCounter1
    case I_snaLuRtmCounter1:
       dp = (void *) (&data->snaLuRtmCounter1);
       break;
#endif /* I_snaLuRtmCounter1 */

#ifdef I_snaLuRtmCounter2
    case I_snaLuRtmCounter2:
       dp = (void *) (&data->snaLuRtmCounter2);
       break;
#endif /* I_snaLuRtmCounter2 */

#ifdef I_snaLuRtmCounter3
    case I_snaLuRtmCounter3:
       dp = (void *) (&data->snaLuRtmCounter3);
       break;
#endif /* I_snaLuRtmCounter3 */

#ifdef I_snaLuRtmCounter4
    case I_snaLuRtmCounter4:
       dp = (void *) (&data->snaLuRtmCounter4);
       break;
#endif /* I_snaLuRtmCounter4 */

#ifdef I_snaLuRtmOverFlows
    case I_snaLuRtmOverFlows:
       dp = (void *) (&data->snaLuRtmOverFlows);
       break;
#endif /* I_snaLuRtmOverFlows */

#ifdef I_snaLuRtmObjPercent
    case I_snaLuRtmObjPercent:
       dp = (void *) (&data->snaLuRtmObjPercent);
       break;
#endif /* I_snaLuRtmObjPercent */

#ifdef I_snaLuRtmObjRange
    case I_snaLuRtmObjRange:
       dp = (void *) (&data->snaLuRtmObjRange);
       break;
#endif /* I_snaLuRtmObjRange */

#ifdef I_snaLuRtmNumTrans
    case I_snaLuRtmNumTrans:
       dp = (void *) (&data->snaLuRtmNumTrans);
       break;
#endif /* I_snaLuRtmNumTrans */

#ifdef I_snaLuRtmLastRspTime
    case I_snaLuRtmLastRspTime:
       dp = (void *) (&data->snaLuRtmLastRspTime);
       break;
#endif /* I_snaLuRtmLastRspTime */

#ifdef I_snaLuRtmAvgRspTime
    case I_snaLuRtmAvgRspTime:
       dp = (void *) (&data->snaLuRtmAvgRspTime);
       break;
#endif /* I_snaLuRtmAvgRspTime */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}


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

static snaNode_t snaNodeData;


snaNode_t *
k_snaNode_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   return(&snaNodeData);
}


static const char* FormatXid(long xid)
{
   static unsigned char xidString[10];
   char* s = xidString;
   static const char hex[] = "0123456789ABCDEF";
   int i;
   for (i = 28; i >= 0; i -= 4)
      *s++ = hex[(xid >> i) & 0xf];
   return xidString;
}



snaNodeAdminEntry_t *
k_snaNodeAdminEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
{

   static snaNodeAdminEntry_t snaNodeAdminEntryData;
   static SnaNodeAdminEntry_t adminEntryData;

   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   hwidbtype* idb;

   /*
    * put your code to retrieve the information here
    */

   if (lastSerialNum == serialNum && lastPuIndex == snaNodeAdminIndex)
      return &snaNodeAdminEntryData;

   adminEntryData.snaNodeAdminIndex = snaNodeAdminIndex;
   idb = UpdatePuConfig(searchType, SYSMGT_TN3270S_NODE_ADMIN, (LuControlCommon*)&adminEntryData);
   if (idb != NULL){
       static OctetString snaNodeAdminName;
       static OctetString idBlock;
       static OctetString idNum;
       static OctetString descr;
       const char* xid = FormatXid(adminEntryData.snaNodeAdminXid);
       snaNodeAdminName.octet_ptr = adminEntryData.snaNodeAdminName;
       snaNodeAdminName.length = strlen(adminEntryData.snaNodeAdminName);
       idBlock.octet_ptr = (char*)xid;
       idBlock.length = 3;
       idNum.octet_ptr =(char*)&xid[3];
       idNum.length = 5;
       descr.octet_ptr = adminEntryData.snaNodeAdminHostDescription;
       descr.length = strlen(adminEntryData.snaNodeAdminHostDescription);

       snaNodeAdminEntryData.snaNodeAdminIndex = adminEntryData.snaNodeAdminIndex;
       snaNodeAdminEntryData.snaNodeAdminName = &snaNodeAdminName;
       snaNodeAdminEntryData.snaNodeAdminType = adminEntryData.snaNodeAdminType;
       snaNodeAdminEntryData.snaNodeAdminXidFormat = adminEntryData.snaNodeAdminXidFormat;
       snaNodeAdminEntryData.snaNodeAdminBlockNum = &idBlock;
       snaNodeAdminEntryData.snaNodeAdminIdNum = &idNum;
       snaNodeAdminEntryData.snaNodeAdminEnablingMethod = adminEntryData.snaNodeAdminEnablingMethod;
       snaNodeAdminEntryData.snaNodeAdminLuTermDefault = adminEntryData.snaNodeAdminLuTermDefault;
       snaNodeAdminEntryData.snaNodeAdminMaxLu = adminEntryData.snaNodeAdminMaxLu;
       snaNodeAdminEntryData.snaNodeAdminHostDescription = &descr;
       snaNodeAdminEntryData.snaNodeAdminStopMethod = adminEntryData.snaNodeAdminStopMethod;
       snaNodeAdminEntryData.snaNodeAdminState = adminEntryData.snaNodeAdminState;
       snaNodeAdminEntryData.snaNodeAdminRowStatus = adminEntryData.snaNodeAdminRowStatus;

       lastSerialNum = serialNum;
       lastPuIndex = snaNodeAdminIndex;
       return(&snaNodeAdminEntryData);
   }
   return(NULL);
}

#ifdef SETS
int
k_snaNodeAdminEntry_test(
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{

    return NO_ERROR;
}

int
k_snaNodeAdminEntry_ready(
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_snaNodeAdminEntry_set_defaults(
    doList_t       *dp)
{
    snaNodeAdminEntry_t *data = (snaNodeAdminEntry_t *) (dp->data);

    if ((data->snaNodeAdminName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaNodeAdminBlockNum = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaNodeAdminIdNum = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaNodeAdminHostDescription = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_snaNodeAdminEntry_set(data, contextInfo, function)
   snaNodeAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}

#ifdef SR_snaNodeAdminEntry_UNDO
/* add #define SR_snaNodeAdminEntry_UNDO in snanaumib.h to
 * include the undo routine for the snaNodeAdminEntry family.
 */
int
snaNodeAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_snaNodeAdminEntry_UNDO */

#endif /* SETS */

snaNodeOperEntry_t *
k_snaNodeOperEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
{
   static snaNodeOperEntry_t snaNodeOperEntryData;
   static SnaNodeOperEntry_t operEntryData;
   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   hwidbtype* idb;

   /*
    * put your code to retrieve the information here
    */
   
   if (lastSerialNum == serialNum && lastPuIndex == snaNodeAdminIndex)
      return &snaNodeOperEntryData;
   
   operEntryData.snaNodeAdminIndex = snaNodeAdminIndex;
   idb = UpdatePuConfig(searchType, SYSMGT_TN3270S_NODE_OPER, (LuControlCommon*)&operEntryData);
   if (idb != NULL){
       static OctetString snaNodeOperName;
       static OctetString idBlock;
       static OctetString idNum;
       static OctetString descr;
       static OctetString sscpId;
       const char* xid = FormatXid(operEntryData.snaNodeOperXid);
       snaNodeOperName.octet_ptr = operEntryData.snaNodeOperName;
       snaNodeOperName.length = strlen(operEntryData.snaNodeOperName);
       idBlock.octet_ptr = (char*)xid;
       idBlock.length = 3;
       idNum.octet_ptr =(char*)&xid[3];
       idNum.length = 5;
       descr.octet_ptr = operEntryData.snaNodeOperHostDescription;
       descr.length = strlen(operEntryData.snaNodeOperHostDescription);
       sscpId.octet_ptr = operEntryData.snaNodeOperHostSscpId;
       sscpId.length = strlen(operEntryData.snaNodeOperHostSscpId);

       snaNodeOperEntryData.snaNodeOperName = &snaNodeOperName;
       snaNodeOperEntryData.snaNodeOperType = operEntryData.snaNodeOperType;
       snaNodeOperEntryData.snaNodeOperXidFormat = operEntryData.snaNodeOperXidFormat;
       snaNodeOperEntryData.snaNodeOperBlockNum = &idBlock;
       snaNodeOperEntryData.snaNodeOperIdNum = &idNum;
       snaNodeOperEntryData.snaNodeOperEnablingMethod = operEntryData.snaNodeOperEnablingMethod;
       snaNodeOperEntryData.snaNodeOperLuTermDefault = operEntryData.snaNodeOperLuTermDefault;
       snaNodeOperEntryData.snaNodeOperMaxLu = operEntryData.snaNodeOperMaxLu;
       snaNodeOperEntryData.snaNodeOperHostDescription = &descr;
       snaNodeOperEntryData.snaNodeOperStopMethod = operEntryData.snaNodeOperStopMethod;
       snaNodeOperEntryData.snaNodeOperState = operEntryData.snaNodeOperState;
       snaNodeOperEntryData.snaNodeOperHostSscpId = &sscpId;
       snaNodeOperEntryData.snaNodeOperStartTime = operEntryData.snaNodeOperStartTime;
       snaNodeOperEntryData.snaNodeOperLastStateChange = operEntryData.snaNodeOperLastStateChange;
       snaNodeOperEntryData.snaNodeOperActFailures = operEntryData.snaNodeOperActFailures;
       snaNodeOperEntryData.snaNodeOperActFailureReason = operEntryData.snaNodeOperActFailureReason;
       snaNodeOperEntryData.snaNodeAdminIndex = operEntryData.snaNodeAdminIndex;

       lastSerialNum = serialNum;
       lastPuIndex = snaNodeAdminIndex;
       return(&snaNodeOperEntryData);
    }
    return(NULL);
}

snaPu20StatsEntry_t *
k_snaPu20StatsEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
{
   static snaPu20StatsEntry_t snaPu20StatsEntryData;
   static SnaPu20StatsEntry_t entryData;
   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   hwidbtype* idb;
   
   /*
    * put your code to retrieve the information here
    */
   if (lastSerialNum == serialNum && lastPuIndex == snaNodeAdminIndex)
      return &snaPu20StatsEntryData;
 
   entryData.snaNodeAdminIndex = snaNodeAdminIndex;
   idb = UpdatePuConfig(searchType, SYSMGT_TN3270S_PU_STAT, (LuControlCommon*)&entryData);
   if (idb != NULL){
      snaPu20StatsEntryData.snaPu20StatsSentBytes = entryData.snaPu20StatsSentBytes;
      snaPu20StatsEntryData.snaPu20StatsReceivedBytes = entryData.snaPu20StatsReceivedBytes;
      snaPu20StatsEntryData.snaPu20StatsSentPius = entryData.snaPu20StatsSentPius;
      snaPu20StatsEntryData.snaPu20StatsReceivedPius = entryData.snaPu20StatsReceivedPius;
      snaPu20StatsEntryData.snaPu20StatsSentNegativeResps = entryData.snaPu20StatsSentNegativeResps;
      snaPu20StatsEntryData.snaPu20StatsReceivedNegativeResps = entryData.snaPu20StatsReceivedNegativeResps;
      snaPu20StatsEntryData.snaPu20StatsActLus = entryData.snaPu20StatsActLus;
      snaPu20StatsEntryData.snaPu20StatsInActLus = entryData.snaPu20StatsInActLus;
      snaPu20StatsEntryData.snaPu20StatsBindLus = entryData.snaPu20StatsBindLus;
      snaPu20StatsEntryData.snaNodeAdminIndex = entryData.snaNodeAdminIndex;

      lastSerialNum = serialNum;
      lastPuIndex =  snaNodeAdminIndex;
      return(&snaPu20StatsEntryData);
   }
   return(NULL);
}

snaNodeLinkAdminEntry_t *
k_snaNodeLinkAdminEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex, snaNodeLinkAdminIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
   SR_INT32 snaNodeLinkAdminIndex;
{

   static snaNodeLinkAdminEntry_t snaNodeLinkAdminEntryData;
   static SnaNodeLinkAdminEntry_t entryData;

   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   static long lastLinkAdminIndex = MAXLONG;
   hwidbtype* idb;

   /*
    * put your code to retrieve the information here
    */
   if (lastSerialNum == serialNum && lastPuIndex == snaNodeAdminIndex && 
       lastLinkAdminIndex == snaNodeLinkAdminIndex)
      return &snaNodeLinkAdminEntryData;

   if (snaNodeLinkAdminIndex > 1) {
      if (searchType == EXACT)
         return(NULL);
      else
         entryData.snaNodeAdminIndex = snaNodeAdminIndex+1;
   }
   else
      entryData.snaNodeAdminIndex = snaNodeAdminIndex;

   idb = UpdatePuConfig(searchType, SYSMGT_TN3270S_LINK_ADMIN,  (LuControlCommon*)&entryData);
   if (idb != NULL) {
       snaNodeLinkAdminEntryData.snaNodeLinkAdminIndex = 1;
       snaNodeLinkAdminEntryData.snaNodeLinkAdminSpecific = &nullOID;
       snaNodeLinkAdminEntryData.snaNodeLinkAdminMaxPiu = entryData.snaNodeLinkAdminMaxPiu;
       snaNodeLinkAdminEntryData.snaNodeLinkAdminRowStatus = entryData.snaNodeLinkAdminRowStatus;
       snaNodeLinkAdminEntryData.snaNodeAdminIndex = entryData.snaNodeAdminIndex;

       lastSerialNum = serialNum;
       lastPuIndex = snaNodeAdminIndex;
       lastLinkAdminIndex = snaNodeLinkAdminIndex;
       return(&snaNodeLinkAdminEntryData);
   }
   return(NULL);
}

#ifdef SETS
int
k_snaNodeLinkAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_snaNodeLinkAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_snaNodeLinkAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    snaNodeLinkAdminEntry_t *data = (snaNodeLinkAdminEntry_t *) (dp->data);

    if ((data->snaNodeLinkAdminSpecific = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_snaNodeLinkAdminEntry_set(data, contextInfo, function)
   snaNodeLinkAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}

#ifdef SR_snaNodeLinkAdminEntry_UNDO
/* add #define SR_snaNodeLinkAdminEntry_UNDO in snanaumib.h to
 * include the undo routine for the snaNodeLinkAdminEntry family.
 */
int
snaNodeLinkAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_snaNodeLinkAdminEntry_UNDO */

#endif /* SETS */

snaNodeLinkOperEntry_t *
k_snaNodeLinkOperEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex, snaNodeLinkAdminIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
   SR_INT32 snaNodeLinkAdminIndex;
{
   static snaNodeLinkOperEntry_t snaNodeLinkOperEntryData;
   static SnaNodeLinkOperEntry_t linkOperEntryData;

   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   static long lastLinkAdminIndex = MAXLONG;

   /*
    * put your code to retrieve the information here
    */
   if (lastSerialNum == serialNum && lastPuIndex == snaNodeAdminIndex &&
       lastLinkAdminIndex == snaNodeLinkAdminIndex)
      return &snaNodeLinkOperEntryData;

   linkOperEntryData.snaNodeAdminIndex = snaNodeAdminIndex;
   linkOperEntryData.snaNodeLinkAdminIndex = snaNodeLinkAdminIndex;
   if (GetLuControlBlock(searchType, SYSMGT_TN3270S_LINK_OPER, (LuControlCommon*)&linkOperEntryData)){
       snaNodeLinkOperEntryData.snaNodeLinkOperSpecific = &nullOID;
       snaNodeLinkOperEntryData.snaNodeLinkOperMaxPiu = linkOperEntryData.snaNodeLinkOperMaxPiu;
       snaNodeLinkOperEntryData.snaNodeAdminIndex = linkOperEntryData.snaNodeAdminIndex;
       snaNodeLinkOperEntryData.snaNodeLinkAdminIndex = linkOperEntryData.snaNodeLinkAdminIndex;

       lastSerialNum = serialNum;
       lastPuIndex = snaNodeAdminIndex;
       lastLinkAdminIndex = snaNodeLinkAdminIndex;
       return(&snaNodeLinkOperEntryData);
   }
   return(NULL);
}

static long GetModelNum (const char* model)
{
   /* the model is stored in xxxx0yE or xxxx0y */
   if (model[0] != 0 &&                  /* model number defined */
       model[4] == '0' && model[5] >= '2' && model[5] <= '5') {
       const char* s;
       static const char base[] = {D_snaLuOperDisplayModel_model2A, 
                                   D_snaLuOperDisplayModel_model3A,
                                   D_snaLuOperDisplayModel_model4A,
                                   D_snaLuOperDisplayModel_model5A};

       static const char extended[] = {D_snaLuOperDisplayModel_model2B, 
                                   D_snaLuOperDisplayModel_model3B,
                                   D_snaLuOperDisplayModel_model4B,
                                   D_snaLuOperDisplayModel_model5B};

       s = (model[6] == 'E') ? extended : base;
       return s[model[5] - '2'];
   }
   if (memcmp(model, "DYNAMIC", 7) == 0){
      return D_snaLuOperDisplayModel_dynamic;
   }
   return D_snaLuOperDisplayModel_invalid;
}

snaLuAdminEntry_t *
k_snaLuAdminEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex, snaLuAdminLuIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
   SR_INT32 snaLuAdminLuIndex;
{
   static snaLuAdminEntry_t snaLuAdminEntryData;
   static SnaLuAdminEntry_t luAdminEntryData;

   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   static long lastLuIndex = MAXLONG;

   /*
    * put your code to retrieve the information here
    */
   if (lastSerialNum == serialNum && lastPuIndex ==  snaNodeAdminIndex &&
          lastLuIndex ==  snaLuAdminLuIndex)
      return &snaLuAdminEntryData;

   luAdminEntryData.snaNodeAdminIndex = snaNodeAdminIndex;
   luAdminEntryData.snaLuAdminLuIndex = snaLuAdminLuIndex;

   if (GetLuControlBlock(searchType, SYSMGT_TN3270S_LU_ADMIN, (LuControlCommon*)&luAdminEntryData)){
       static OctetString name;
       static OctetString snaName;
       static OctetString locaddr;

       snaLuAdminEntryData.snaLuAdminLuIndex = luAdminEntryData.snaLuAdminLuIndex;
       name.octet_ptr = luAdminEntryData.snaLuAdminName;
       name.length = strlen(luAdminEntryData.snaLuAdminName);
       snaName.octet_ptr = luAdminEntryData.snaLuAdminSnaName;
       snaName.length = strlen(luAdminEntryData.snaLuAdminSnaName);
       snaLuAdminEntryData.snaLuAdminName = &name;
       snaLuAdminEntryData.snaLuAdminSnaName = &snaName;
       snaLuAdminEntryData.snaLuAdminType = luAdminEntryData.snaLuAdminType;
       snaLuAdminEntryData.snaLuAdminDepType = luAdminEntryData.snaLuAdminDepType;

       locaddr.octet_ptr = &luAdminEntryData.snaLuAdminLocalAddress;
       locaddr.length = 1;
       snaLuAdminEntryData.snaLuAdminLocalAddress = &locaddr;
       snaLuAdminEntryData.snaLuAdminDisplayModel = GetModelNum(
                         luAdminEntryData.snaLuAdminDisplayModel);
       snaLuAdminEntryData.snaLuAdminTerm = luAdminEntryData.snaLuAdminTerm;
       snaLuAdminEntryData.snaLuAdminRowStatus = luAdminEntryData.snaLuAdminRowStatus;
       snaLuAdminEntryData.snaNodeAdminIndex = luAdminEntryData.snaNodeAdminIndex;

       lastSerialNum = serialNum;
       lastPuIndex = snaNodeAdminIndex;
       lastLuIndex = snaLuAdminLuIndex;
       return(&snaLuAdminEntryData);
   }
   return(NULL);
}

#ifdef SETS
int
k_snaLuAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_snaLuAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_snaLuAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    snaLuAdminEntry_t *data = (snaLuAdminEntry_t *) (dp->data);

    if ((data->snaLuAdminName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaLuAdminSnaName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaLuAdminLocalAddress = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_snaLuAdminEntry_set(data, contextInfo, function)
   snaLuAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}

#ifdef SR_snaLuAdminEntry_UNDO
/* add #define SR_snaLuAdminEntry_UNDO in snanaumib.h to
 * include the undo routine for the snaLuAdminEntry family.
 */
int
snaLuAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_snaLuAdminEntry_UNDO */

#endif /* SETS */

snaLuOperEntry_t *
k_snaLuOperEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex, snaLuAdminLuIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
   SR_INT32 snaLuAdminLuIndex;
{

   static snaLuOperEntry_t snaLuOperEntryData;
   static SnaLuOperEntry_t luOperEntryData;

   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   static long lastLuIndex = MAXLONG;
   
   if (lastSerialNum == serialNum && lastPuIndex ==  snaNodeAdminIndex &&
          lastLuIndex ==  snaLuAdminLuIndex)
      return &snaLuOperEntryData;
   
   luOperEntryData.snaNodeAdminIndex = snaNodeAdminIndex;
   luOperEntryData.snaLuAdminLuIndex = snaLuAdminLuIndex;

   if (GetLuControlBlock(searchType, SYSMGT_TN3270S_LU_OPER, (LuControlCommon*)&luOperEntryData)){
       static OctetString name;
       static OctetString snaName;
       static OctetString locaddr;

       snaLuOperEntryData.snaLuAdminLuIndex = luOperEntryData.snaLuAdminLuIndex;
       name.octet_ptr = luOperEntryData.snaLuOperName;
       name.length = strlen(luOperEntryData.snaLuOperName);
       snaName.octet_ptr = luOperEntryData.snaLuOperSnaName;
       snaName.length = strlen(luOperEntryData.snaLuOperSnaName);
       snaLuOperEntryData.snaLuOperName = &name;
       snaLuOperEntryData.snaLuOperSnaName = &snaName;
       snaLuOperEntryData.snaLuOperType = luOperEntryData.snaLuOperType;
       snaLuOperEntryData.snaLuOperDepType = luOperEntryData.snaLuOperDepType;

       locaddr.octet_ptr = &luOperEntryData.snaLuOperLocalAddress;
       locaddr.length = 1;
       snaLuOperEntryData.snaLuOperLocalAddress = &locaddr;
       snaLuOperEntryData.snaLuOperDisplayModel = GetModelNum(
                                     luOperEntryData.snaLuOperDisplayModel);
       snaLuOperEntryData.snaLuOperTerm = luOperEntryData.snaLuOperTerm;
       snaLuOperEntryData.snaLuOperState = luOperEntryData.snaLuOperState;
       snaLuOperEntryData.snaLuOperSessnCount = luOperEntryData.snaLuOperSessnCount;
       snaLuOperEntryData.snaNodeAdminIndex = luOperEntryData.snaNodeAdminIndex;

       lastSerialNum = serialNum;
       lastPuIndex = snaNodeAdminIndex;
       lastLuIndex = snaLuAdminLuIndex;
       return(&snaLuOperEntryData);
   }
   return(NULL);
}


snaLuSessnEntry_t *
k_snaLuSessnEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex, snaLuAdminLuIndex, snaLuSessnRluIndex, snaLuSessnIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
   SR_INT32 snaLuAdminLuIndex;
   SR_INT32 snaLuSessnRluIndex;
   SR_INT32 snaLuSessnIndex;
{

   static snaLuSessnEntry_t snaLuSessnEntryData;
   static SnaLuSessnEntry_t luSessnEntryData;     /* static, pointer to snaLuSessn */

   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   static long lastLuIndex = MAXLONG;

   /*
    * put your code to retrieve the information here
    */
   if (snaLuSessnRluIndex != 0 || snaLuSessnIndex != 0){
       if (searchType == EXACT){
           return NULL;                          /* we dont use these index */
       }
       else {
          /* mibview had adavanced the session control block */
          snaLuAdminLuIndex++;
      }
   }
          
   if (lastSerialNum == serialNum && lastPuIndex ==  snaNodeAdminIndex &&
          lastLuIndex ==  snaLuAdminLuIndex && snaLuSessnRluIndex == 0 && snaLuSessnIndex == 0)
      return &snaLuSessnEntryData;

   luSessnEntryData.snaNodeAdminIndex = snaNodeAdminIndex;
   luSessnEntryData.snaLuAdminLuIndex = snaLuAdminLuIndex;
   if (GetLuControlBlock(searchType, SYSMGT_TN3270S_LU_SESSN, (LuControlCommon*)&luSessnEntryData)){

       static OctetString locAppName;
       static OctetString remAppName;
       static OctetString senseCode;
       static OctetString unbindType;
       static char sense[9];
       static char unbindData[3];
       locAppName.octet_ptr = "";
       locAppName.length = 0;
       senseCode.octet_ptr = sense;
       senseCode.length = 8;
       unbindType.octet_ptr = unbindData;
       unbindType.length = 2;
       snaLuSessnEntryData.snaLuSessnRluIndex = 0;
       snaLuSessnEntryData.snaLuSessnIndex = 0;
       snaLuSessnEntryData.snaLuSessnLocalApplName = &locAppName;
       remAppName.octet_ptr = luSessnEntryData.snaLuSessnRemoteLuName;
       remAppName.length = strlen(luSessnEntryData.snaLuSessnRemoteLuName);
       snaLuSessnEntryData.snaLuSessnRemoteLuName = &remAppName;
       snaLuSessnEntryData.snaLuSessnMaxSndRuSize = luSessnEntryData.snaLuSessnMaxSndRuSize;
       snaLuSessnEntryData.snaLuSessnMaxRcvRuSize = luSessnEntryData.snaLuSessnMaxRcvRuSize;
       snaLuSessnEntryData.snaLuSessnSndPacingSize = luSessnEntryData.snaLuSessnSndPacingSize;
       snaLuSessnEntryData.snaLuSessnRcvPacingSize = luSessnEntryData.snaLuSessnRcvPacingSize;
       snaLuSessnEntryData.snaLuSessnActiveTime = luSessnEntryData.snaLuSessnActiveTime;
       snaLuSessnEntryData.snaLuSessnAdminState = luSessnEntryData.snaLuSessnAdminState;
       snaLuSessnEntryData.snaLuSessnOperState = luSessnEntryData.snaLuSessnOperState;
       sprintf(sense, "%08x", luSessnEntryData.snaLuSessnSenseData);
       snaLuSessnEntryData.snaLuSessnSenseData = &senseCode;
       snaLuSessnEntryData.snaLuSessnTerminationRu = luSessnEntryData.snaLuSessnTerminationRu;
       sprintf(unbindData, "%02x", luSessnEntryData.snaLuSessnUnbindType & 0xff);
       snaLuSessnEntryData.snaLuSessnUnbindType = &unbindType;
       snaLuSessnEntryData.snaLuSessnLinkIndex = 0;
       snaLuSessnEntryData.snaNodeAdminIndex = luSessnEntryData.snaNodeAdminIndex;
       snaLuSessnEntryData.snaLuAdminLuIndex = luSessnEntryData.snaLuAdminLuIndex;

       lastSerialNum = serialNum;
       lastPuIndex = snaNodeAdminIndex;
       lastLuIndex = snaLuAdminLuIndex;
       return(&snaLuSessnEntryData);
    }
    return(NULL);
}

#ifdef SETS
int
k_snaLuSessnEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_snaLuSessnEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_snaLuSessnEntry_set_defaults(dp)
    doList_t       *dp;
{
    snaLuSessnEntry_t *data = (snaLuSessnEntry_t *) (dp->data);

    if ((data->snaLuSessnLocalApplName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaLuSessnRemoteLuName = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaLuSessnSenseData = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->snaLuSessnUnbindType = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_snaLuSessnEntry_set(data, contextInfo, function)
   snaLuSessnEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}

#ifdef SR_snaLuSessnEntry_UNDO
/* add #define SR_snaLuSessnEntry_UNDO in snanaumib.h to
 * include the undo routine for the snaLuSessnEntry family.
 */
int
snaLuSessnEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_snaLuSessnEntry_UNDO */

#endif /* SETS */

snaLuSessnStatsEntry_t *
k_snaLuSessnStatsEntry_get(serialNum, contextInfo, nominator, searchType, snaNodeAdminIndex, snaLuAdminLuIndex, snaLuSessnRluIndex, snaLuSessnIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaNodeAdminIndex;
   SR_INT32 snaLuAdminLuIndex;
   SR_INT32 snaLuSessnRluIndex;
   SR_INT32 snaLuSessnIndex;
{
   static snaLuSessnStatsEntry_t snaLuSessnStatsEntryData;
   SnaLuSessnStatsEntry_t luSessnStatsEntryData;

   static int lastSerialNum = MAXINT;
   static long lastPuIndex = MAXLONG;
   static long lastLuIndex = MAXLONG;

   if (snaLuSessnRluIndex != 0 || snaLuSessnIndex != 0){
       if (searchType == EXACT){
           return NULL;                          /* we dont use these index */
       }
       else {
          /* mibview had advanced the session control block */
          snaLuAdminLuIndex++;
      }
   }
          
   if (lastSerialNum == serialNum && lastPuIndex ==  snaNodeAdminIndex &&
          lastLuIndex ==  snaLuAdminLuIndex && snaLuSessnRluIndex == 0 && snaLuSessnIndex == 0){
      return &snaLuSessnStatsEntryData;
   }

   luSessnStatsEntryData.snaNodeAdminIndex = snaNodeAdminIndex;
   luSessnStatsEntryData.snaLuAdminLuIndex = snaLuAdminLuIndex;
   if (GetLuControlBlock(searchType, SYSMGT_TN3270S_LU_SESSN_STAT, (LuControlCommon*)&luSessnStatsEntryData)){

      snaLuSessnStatsEntryData.snaLuSessnStatsSentBytes = luSessnStatsEntryData.snaLuSessnStatsSentBytes;
      snaLuSessnStatsEntryData.snaLuSessnStatsReceivedBytes = luSessnStatsEntryData.snaLuSessnStatsReceivedBytes;
      snaLuSessnStatsEntryData.snaLuSessnStatsSentRus = luSessnStatsEntryData.snaLuSessnStatsSentRus;
      snaLuSessnStatsEntryData.snaLuSessnStatsReceivedRus = luSessnStatsEntryData.snaLuSessnStatsReceivedRus;
      snaLuSessnStatsEntryData.snaLuSessnStatsSentNegativeResps = luSessnStatsEntryData.snaLuSessnStatsSentNegativeResps;
      snaLuSessnStatsEntryData.snaLuSessnStatsReceivedNegativeResps = luSessnStatsEntryData.snaLuSessnStatsReceivedNegativeResps;
      snaLuSessnStatsEntryData.snaNodeAdminIndex = luSessnStatsEntryData.snaNodeAdminIndex;
      snaLuSessnStatsEntryData.snaLuAdminLuIndex = luSessnStatsEntryData.snaLuAdminLuIndex;
      snaLuSessnStatsEntryData.snaLuSessnRluIndex = 0;
      snaLuSessnStatsEntryData.snaLuSessnIndex = 0;

      lastSerialNum = serialNum;
      lastPuIndex = snaNodeAdminIndex;
      lastLuIndex = snaLuAdminLuIndex;
      return(&snaLuSessnStatsEntryData);
   }
   return(NULL);
}

snaLuRtmEntry_t *
k_snaLuRtmEntry_get(serialNum, contextInfo, nominator, searchType, snaLuRtmPuIndex, snaLuRtmLuIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 snaLuRtmPuIndex;
   SR_INT32 snaLuRtmLuIndex;
{
#ifdef NOT_YET
   static snaLuRtmEntry_t snaLuRtmEntryData;

   /*
    * put your code to retrieve the information here
    */

   snaLuRtmEntryData.snaLuRtmPuIndex = ;
   snaLuRtmEntryData.snaLuRtmLuIndex = ;
   snaLuRtmEntryData.snaLuRtmState = ;
   snaLuRtmEntryData.snaLuRtmStateTime = ;
   snaLuRtmEntryData.snaLuRtmDef = ;
   snaLuRtmEntryData.snaLuRtmBoundary1 = ;
   snaLuRtmEntryData.snaLuRtmBoundary2 = ;
   snaLuRtmEntryData.snaLuRtmBoundary3 = ;
   snaLuRtmEntryData.snaLuRtmBoundary4 = ;
   snaLuRtmEntryData.snaLuRtmCounter1 = ;
   snaLuRtmEntryData.snaLuRtmCounter2 = ;
   snaLuRtmEntryData.snaLuRtmCounter3 = ;
   snaLuRtmEntryData.snaLuRtmCounter4 = ;
   snaLuRtmEntryData.snaLuRtmOverFlows = ;
   snaLuRtmEntryData.snaLuRtmObjPercent = ;
   snaLuRtmEntryData.snaLuRtmObjRange = ;
   snaLuRtmEntryData.snaLuRtmNumTrans = ;
   snaLuRtmEntryData.snaLuRtmLastRspTime = ;
   snaLuRtmEntryData.snaLuRtmAvgRspTime = ;
   return(&snaLuRtmEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}



static void
init_snanaumib (subsystype *subsys)
{
   sys_timestamp timestamp;
   GET_TIMESTAMP(timestamp);
   snaNodeData.snaNodeAdminTableLastChange = timestamp.u.p.low;
   snaNodeData.snaNodeOperTableLastChange = timestamp.u.p.low;
   snaNodeData.snaNodeLinkAdminTableLastChange = timestamp.u.p.low;
   snaNodeData.snaNodeLinkOperTableLastChange = timestamp.u.p.low;
   load_mib(snanaumib_OidList, snanaumib_OidListNum);
    load_oid(snanaumib_oid_table);
}

/*
 * snanaumib subsystem header
 */


#define MAJVERSION_snanaumib 1
#define MINVERSION_snanaumib 0
#define EDITVERSION_snanaumib 0

SUBSYS_HEADER(snanaumib,
              MAJVERSION_snanaumib,
              MINVERSION_snanaumib,
              EDITVERSION_snanaumib,
              init_snanaumib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
      
