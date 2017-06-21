/* $Id: sr_rttmib.c,v 1.1.4.5 1996/07/01 18:45:54 hampton Exp $
 * $Source: /release/112/cvs/Xsys/rtt/sr_rttmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * December 1995, Larry Metzger (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_rttmib.c,v $
 * Revision 1.1.4.5  1996/07/01  18:45:54  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.4.4  1996/06/22  23:47:03  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 1.1.4.3  1996/06/04  18:28:57  lmetzger
 * CSCdi59459:  See CSCdi589xx, xx = 29,30,31,32,33,35,36,47
 * Branch: California_branch
 * - Group of defects that are interdependent and tested.
 *
 * Revision 1.1.4.2  1996/06/03  23:27:57  lmetzger
 * CSCdi58510:  strange snmp command in config
 * Branch: California_branch
 * - The initial state for snmp-server enable traps rtr was incorrect.
 *
 * Revision 1.1.4.1  1996/05/17  11:42:00  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:57  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:42:03  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:58  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:42  lmetzger
 * Placeholder for IbuMod_Calif_branch
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
#include "sr_rttmib.h"
#include "sr_rttmib-mib.h"
#include "rtt_lib.h"
#include "config.h"
#include "rtt_snmp_chain.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_snmp.h"
#include "../rtt/rtt_registry.h"


/*
 * File Scoped pointers to reusable structures that are 
 *   malloced at init.
 */

static rttMonCtrlAdminEntry_t         *rttMonCtrlAdminEntryData;
static rttMonCtrlAdminEntry_t         *rttMonCtrlAdminEntryBuff;
static rttMonEchoAdminEntry_t         *rttMonEchoAdminEntryData;
static rttMonEchoAdminEntry_t         *rttMonEchoAdminEntryBuff;
static rttMonStatsCollectEntry_t      *rttMonStatsCollectEntryData;
static rttMonHistoryCollectionEntry_t *rttMonHistoryCollectionEntryData;
static rttMonCtrlAdminEntry_t         *currRttMonCtrlAdminEntryPtr;
static rttMonEchoAdminEntry_t         *currRttMonEchoAdminEntryPtr;
static int                            numVarBinds;
static boolean                        newRttMonAdminEntryRow;
static boolean                        newRttMonReactTriggerAdminEntryRow;

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the rttMonAppl family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonAppl_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    rttMonAppl_t    *data = NULL;


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
     if ( (arg == -1) || (data = k_rttMonAppl_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonApplVersion
    case I_rttMonApplVersion:
       dp = (void *) (CloneOctetString(data->rttMonApplVersion));
       break;
#endif /* I_rttMonApplVersion */

#ifdef I_rttMonApplMaxPacketDataSize
    case I_rttMonApplMaxPacketDataSize:
       dp = (void *) (&data->rttMonApplMaxPacketDataSize);
       break;
#endif /* I_rttMonApplMaxPacketDataSize */

#ifdef I_rttMonApplTimeOfLastSet
    case I_rttMonApplTimeOfLastSet:
       dp = (void *) (&data->rttMonApplTimeOfLastSet);
       break;
#endif /* I_rttMonApplTimeOfLastSet */

#ifdef I_rttMonApplNumCtrlAdminEntry
    case I_rttMonApplNumCtrlAdminEntry:
       dp = (void *) (&data->rttMonApplNumCtrlAdminEntry);
       break;
#endif /* I_rttMonApplNumCtrlAdminEntry */

#ifdef I_rttMonApplReset
    case I_rttMonApplReset:
       dp = (void *) (&data->rttMonApplReset);
       break;
#endif /* I_rttMonApplReset */

#ifdef I_rttMonApplPreConfigedReset
    case I_rttMonApplPreConfigedReset:
       dp = (void *) (&data->rttMonApplPreConfigedReset);
       break;
#endif /* I_rttMonApplPreConfigedReset */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rttMonAppl data object.
 *---------------------------------------------------------------------*/
void
rttMonAppl_free (
   rttMonAppl_t    *data)
{
   if (data != NULL) {
       FreeOctetString(data->rttMonApplVersion);

       free ((char *) data);
   }

   numVarBinds = 0;
   
   return;
}

/*----------------------------------------------------------------------
 * cleanup after rttMonAppl set/undo
 *---------------------------------------------------------------------*/
static int rttMonAppl_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
rttMonAppl_cleanup (
   doList_t *trash)
{
   rttMonAppl_free(trash->data);
   rttMonAppl_free(trash->undodata);
   return (NO_ERROR);
}

#ifdef SR_rttMonAppl_UNDO
/*----------------------------------------------------------------------
 * clone the rttMonAppl family
 *---------------------------------------------------------------------*/
rttMonAppl_t *
Clone_rttMonAppl(rttMonAppl)
    rttMonAppl_t *rttMonAppl;
{
    rttMonAppl_t *data;
 
    if (rttMonAppl == NULL) {
        return NULL;
    }
 
    if ((data = malloc(sizeof(rttMonAppl_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (rttMonAppl), sizeof(rttMonAppl_t));
 
    data->rttMonApplVersion = CloneOctetString(rttMonAppl->rttMonApplVersion);
 
    return data;
}
#endif /* defined(SR_rttMonAppl_UNDO) */

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
rttMonAppl_test (
    OID            *incoming,
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
#ifdef SR_rttMonAppl_UNDO
    rttMonAppl_t     *rttMonAppl;
#endif /* defined(SR_rttMonAppl_UNDO) */

   /*
    * Validate the object instance: 1) It must be of length 1  2) and the
    * instance must be 0.
    */
    if (instLength != 1 || incoming->oid_ptr[incoming->length - 1] != 0) {
        return (NO_CREATION_ERROR); 
    }

#ifdef SR_rttMonAppl_UNDO
    rttMonAppl = k_rttMonAppl_get(-1, contextInfo, -1);
#endif /* defined SR_rttMonAppl_UNDO */

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same family.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rttMonAppl_set) &&
            (((rttMonAppl_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(rttMonAppl_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return (RESOURCE_UNAVAILABLE_ERROR);
        }
        memset(dp->data, 0, sizeof(rttMonAppl_t));
#ifdef SR_rttMonAppl_UNDO
        if ((dp->undodata = (void *) Clone_rttMonAppl(rttMonAppl)) == NULL) {
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            error_status = RESOURCE_UNAVAILABLE_ERROR;
        }
#endif /* defined(SR_rttMonAppl_UNDO) */

        dp->setMethod = rttMonAppl_set;
        dp->cleanupMethod = rttMonAppl_cleanup;
#ifdef SR_rttMonAppl_UNDO
        dp->undoMethod = rttMonAppl_undo;
#else /* SR_rttMonAppl_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rttMonAppl_UNDO */
        dp->state = UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    switch (object->nominator) {

#ifdef I_rttMonApplReset
   case I_rttMonApplReset:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonAppl_t *) (dp->data))->rttMonApplReset = value->sl_value;
     break;
#endif /* I_rttMonApplReset */

#ifdef I_rttMonApplPreConfigedReset
   case I_rttMonApplPreConfigedReset:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonAppl_t *) (dp->data))->rttMonApplPreConfigedReset = value->sl_value;
     break;
#endif /* I_rttMonApplPreConfigedReset */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rttMonAppl_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

   /* Do system dependent testing in k_rttMonAppl_test */
   error_status = k_rttMonAppl_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rttMonAppl_t *) (dp->data))->valid);
        error_status = k_rttMonAppl_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rttMonAppl_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_rttMonAppl_set((rttMonAppl_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rttMonApplSupportedRttTypesEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonApplSupportedRttTypesEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonApplSupportedRttTypesEntry_t *data = NULL;
    SR_UINT32       buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        rttMonApplSupportedRttTypes;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonApplSupportedRttTypes, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonApplSupportedRttTypesEntry_get(serialNum, contextInfo, arg ,searchType, rttMonApplSupportedRttTypes)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->rttMonApplSupportedRttTypes;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonApplSupportedRttTypesValid
    case I_rttMonApplSupportedRttTypesValid:
       dp = (void *) (&data->rttMonApplSupportedRttTypesValid);
       break;
#endif /* I_rttMonApplSupportedRttTypesValid */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rttMonApplSupportedProtocolsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonApplSupportedProtocolsEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonApplSupportedProtocolsEntry_t *data = NULL;
    SR_UINT32       buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        rttMonApplSupportedProtocols;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonApplSupportedProtocols, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonApplSupportedProtocolsEntry_get(serialNum, contextInfo, arg ,searchType, rttMonApplSupportedProtocols)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->rttMonApplSupportedProtocols;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonApplSupportedProtocolsValid
    case I_rttMonApplSupportedProtocolsValid:
       dp = (void *) (&data->rttMonApplSupportedProtocolsValid);
       break;
#endif /* I_rttMonApplSupportedProtocolsValid */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rttMonApplPreConfigedEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonApplPreConfigedEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             arg = object->nominator;
    void            *dp;
    rttMonApplPreConfigedEntry_t *data = NULL;
    SR_UINT32       buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    SR_INT32        rttMonApplPreConfigedType;
    int             rttMonApplPreConfigedType_offset;
    OctetString *   rttMonApplPreConfigedName = NULL;
    int             rttMonApplPreConfigedName_offset;
    int             index;
    int             final_index;

    rttMonApplPreConfigedType_offset = object->oid.length;
    rttMonApplPreConfigedName_offset = rttMonApplPreConfigedType_offset + 1;
    final_index = rttMonApplPreConfigedName_offset + GetVariableIndexLength(incoming, rttMonApplPreConfigedName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, rttMonApplPreConfigedName_offset, &rttMonApplPreConfigedName, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, rttMonApplPreConfigedType_offset, &rttMonApplPreConfigedType, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonApplPreConfigedEntry_get(serialNum, contextInfo, arg ,searchType, rttMonApplPreConfigedType, rttMonApplPreConfigedName)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->rttMonApplPreConfigedType;
        inst.oid_ptr[index++] = data->rttMonApplPreConfigedName->length;
        for(i = 0; i < data->rttMonApplPreConfigedName->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->rttMonApplPreConfigedName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.length = index;
     }

        FreeOctetString(rttMonApplPreConfigedName);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonApplPreConfigedValid
    case I_rttMonApplPreConfigedValid:
       dp = (void *) (&data->rttMonApplPreConfigedValid);
       break;
#endif /* I_rttMonApplPreConfigedValid */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rttMonCtrlAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonCtrlAdminEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    rttMonCtrlAdminEntry_t *data = NULL;
    SR_UINT32       buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonCtrlAdminEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {
     
#ifdef I_rttMonCtrlAdminOwner
    case I_rttMonCtrlAdminOwner:
       dp = (void *) (CloneOctetString(data->rttMonCtrlAdminOwner));
       break;
#endif /* I_rttMonCtrlAdminOwner */

#ifdef I_rttMonCtrlAdminTag
    case I_rttMonCtrlAdminTag:
       dp = (void *) (CloneOctetString(data->rttMonCtrlAdminTag));
       break;
#endif /* I_rttMonCtrlAdminTag */

#ifdef I_rttMonCtrlAdminRttType
    case I_rttMonCtrlAdminRttType:
       dp = (void *) (&data->rttMonCtrlAdminRttType);
       break;
#endif /* I_rttMonCtrlAdminRttType */

#ifdef I_rttMonCtrlAdminThreshold
    case I_rttMonCtrlAdminThreshold:
       dp = (void *) (&data->rttMonCtrlAdminThreshold);
       break;
#endif /* I_rttMonCtrlAdminThreshold */

#ifdef I_rttMonCtrlAdminFrequency
    case I_rttMonCtrlAdminFrequency:
       dp = (void *) (&data->rttMonCtrlAdminFrequency);
       break;
#endif /* I_rttMonCtrlAdminFrequency */

#ifdef I_rttMonCtrlAdminTimeout
    case I_rttMonCtrlAdminTimeout:
       dp = (void *) (&data->rttMonCtrlAdminTimeout);
       break;
#endif /* I_rttMonCtrlAdminTimeout */

#ifdef I_rttMonCtrlAdminVerifyData
    case I_rttMonCtrlAdminVerifyData:
       dp = (void *) (&data->rttMonCtrlAdminVerifyData);
       break;
#endif /* I_rttMonCtrlAdminVerifyData */

#ifdef I_rttMonCtrlAdminStatus
    case I_rttMonCtrlAdminStatus:
       dp = (void *) (&data->rttMonCtrlAdminStatus);
       break;
#endif /* I_rttMonCtrlAdminStatus */

     /*
      *  AUGMENT Tables BUG!!!!!! Fill in the rest of the table entries
      *
      */

#ifdef I_rttMonScheduleAdminRttLife
    case I_rttMonScheduleAdminRttLife:
       dp = (void *) (&data->rttMonScheduleAdminRttLife);
       break;
#endif

#ifdef I_rttMonScheduleAdminRttStartTime
    case I_rttMonScheduleAdminRttStartTime:
       dp = (void *) (&data->rttMonScheduleAdminRttStartTime);
       break;
#endif

#ifdef I_rttMonScheduleAdminConceptRowAgeout
    case I_rttMonScheduleAdminConceptRowAgeout:
       dp = (void *) (&data->rttMonScheduleAdminConceptRowAgeout);
       break;
#endif

#ifdef I_rttMonReactAdminConnectionEnable
    case I_rttMonReactAdminConnectionEnable:
       dp = (void *) (&data->rttMonReactAdminConnectionEnable);
       break;
#endif

#ifdef I_rttMonReactAdminTimeoutEnable
    case I_rttMonReactAdminTimeoutEnable:
       dp = (void *) (&data->rttMonReactAdminTimeoutEnable);
       break;
#endif

#ifdef I_rttMonReactAdminThresholdType
    case I_rttMonReactAdminThresholdType:
       dp = (void *) (&data->rttMonReactAdminThresholdType);
       break;
#endif

#ifdef I_rttMonReactAdminThresholdFalling
    case I_rttMonReactAdminThresholdFalling:
       dp = (void *) (&data->rttMonReactAdminThresholdFalling);
       break;
#endif

#ifdef I_rttMonReactAdminThresholdCount
    case I_rttMonReactAdminThresholdCount:
       dp = (void *) (&data->rttMonReactAdminThresholdCount);
       break;
#endif

#ifdef I_rttMonReactAdminThresholdCount2
    case I_rttMonReactAdminThresholdCount2:
       dp = (void *) (&data->rttMonReactAdminThresholdCount2);
       break;
#endif

#ifdef I_rttMonReactAdminActionType
    case I_rttMonReactAdminActionType:
       dp = (void *) (&data->rttMonReactAdminActionType);
       break;
#endif

#ifdef I_rttMonStatisticsAdminNumHourGroups
    case I_rttMonStatisticsAdminNumHourGroups:
       dp = (void *) (&data->rttMonStatisticsAdminNumHourGroups);
       break;
#endif

#ifdef I_rttMonStatisticsAdminNumPaths
    case I_rttMonStatisticsAdminNumPaths:
       dp = (void *) (&data->rttMonStatisticsAdminNumPaths);
       break;
#endif

#ifdef I_rttMonStatisticsAdminNumHops
    case I_rttMonStatisticsAdminNumHops:
       dp = (void *) (&data->rttMonStatisticsAdminNumHops);
       break;
#endif

#ifdef I_rttMonStatisticsAdminNumDistBuckets
    case I_rttMonStatisticsAdminNumDistBuckets:
       dp = (void *) (&data->rttMonStatisticsAdminNumDistBuckets);
       break;
#endif

#ifdef I_rttMonStatisticsAdminDistInterval
    case I_rttMonStatisticsAdminDistInterval:
       dp = (void *) (&data->rttMonStatisticsAdminDistInterval);
       break;
#endif

#ifdef I_rttMonHistoryAdminNumLives
    case I_rttMonHistoryAdminNumLives:
       dp = (void *) (&data->rttMonHistoryAdminNumLives);
       break;
#endif

#ifdef I_rttMonHistoryAdminNumBuckets
    case I_rttMonHistoryAdminNumBuckets:
       dp = (void *) (&data->rttMonHistoryAdminNumBuckets);
       break;
#endif

#ifdef I_rttMonHistoryAdminNumSamples
    case I_rttMonHistoryAdminNumSamples:
       dp = (void *) (&data->rttMonHistoryAdminNumSamples);
       break;
#endif

#ifdef I_rttMonHistoryAdminFilter
    case I_rttMonHistoryAdminFilter:
       dp = (void *) (&data->rttMonHistoryAdminFilter);
       break;
#endif

#ifdef I_rttMonCtrlOperModificationTime
    case I_rttMonCtrlOperModificationTime:
       dp = (void *) (&data->rttMonCtrlOperModificationTime);
       break;
#endif

#ifdef I_rttMonCtrlOperDiagText
    case I_rttMonCtrlOperDiagText:
       dp = (void *) CloneOctetString(data->rttMonCtrlOperDiagText);
       break;
#endif

#ifdef I_rttMonCtrlOperResetTime
    case I_rttMonCtrlOperResetTime:
       dp = (void *) (&data->rttMonCtrlOperResetTime);
       break;
#endif

#ifdef I_rttMonCtrlOperOctetsInUse
    case I_rttMonCtrlOperOctetsInUse:
       dp = (void *) (&data->rttMonCtrlOperOctetsInUse);
       break;
#endif

#ifdef I_rttMonCtrlOperConnectionLostOccurred
    case I_rttMonCtrlOperConnectionLostOccurred:
       dp = (void *) (&data->rttMonCtrlOperConnectionLostOccurred);
       break;
#endif

#ifdef I_rttMonCtrlOperTimeoutOccurred
    case I_rttMonCtrlOperTimeoutOccurred:
       dp = (void *) (&data->rttMonCtrlOperTimeoutOccurred);
       break;
#endif

#ifdef I_rttMonCtrlOperOverThresholdOccurred
    case I_rttMonCtrlOperOverThresholdOccurred:
       dp = (void *) (&data->rttMonCtrlOperOverThresholdOccurred);
       break;
#endif

#ifdef I_rttMonCtrlOperNumRtts
    case I_rttMonCtrlOperNumRtts:
       dp = (void *) (&data->rttMonCtrlOperNumRtts);
       break;
#endif

#ifdef I_rttMonCtrlOperRttLife
    case I_rttMonCtrlOperRttLife:
       dp = (void *) (&data->rttMonCtrlOperRttLife);
       break;
#endif

#ifdef I_rttMonCtrlOperState
    case I_rttMonCtrlOperState:
       dp = (void *) (&data->rttMonCtrlOperState);
       break;
#endif

#ifdef I_rttMonLatestRttOperCompletionTime
    case I_rttMonLatestRttOperCompletionTime:
       dp = (void *) (&data->rttMonLatestRttOperCompletionTime);
       break;
#endif

#ifdef I_rttMonLatestRttOperSense
    case I_rttMonLatestRttOperSense:
       dp = (void *) (&data->rttMonLatestRttOperSense);
       break;
#endif

#ifdef I_rttMonLatestRttOperApplSpecificSense
    case I_rttMonLatestRttOperApplSpecificSense:
       dp = (void *) (&data->rttMonLatestRttOperApplSpecificSense);
       break;
#endif

#ifdef I_rttMonLatestRttOperSenseDescription
    case I_rttMonLatestRttOperSenseDescription:
       dp = (void *) CloneOctetString(data->rttMonLatestRttOperSenseDescription);
       break;
#endif

#ifdef I_rttMonLatestRttOperTime
    case I_rttMonLatestRttOperTime:
       dp = (void *) (&data->rttMonLatestRttOperTime);
       break;
#endif

#ifdef I_rttMonLatestRttOperAddress
    case I_rttMonLatestRttOperAddress:
       dp = (void *) CloneOctetString(data->rttMonLatestRttOperAddress);
       break;
#endif

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rttMonCtrlAdminEntry data object.
 *---------------------------------------------------------------------*/
void
rttMonCtrlAdminEntry_free (
   rttMonCtrlAdminEntry_t *data)
{
   if (data != NULL) {
     /*  
      *	AUGMENT Tables BUG!!!!!! Not Freeing ALL!!!!!!! 
      *	FreeOctetString(data->rttMonCtrlAdminOwner);
      *	FreeOctetString(data->rttMonCtrlAdminTag);
      *	
      *	free ((char *) data);
      *	
      *	The following call works!
      */
     
     /*
      *  If createAndGo or createAndWait and an CtrlAdminQItem 
      *   has already been created, delete it.
      */
     if ((data->rttMonCtrlAdminStatus == CREATE_AND_GO) ||
	 (data->rttMonCtrlAdminStatus == CREATE_AND_WAIT))
     {
       if (getRttMonCtrlAdminQItem(
		   data->
		     rttMonCtrlAdminIndex) != NULL)
       {
	 freeRttMonCtrlAdminQItemByIndex(
			 data->
			   rttMonCtrlAdminIndex);
       }
     }

     freeRttMonCtrlAdminEntry(data);

   }

   currRttMonEchoAdminEntryPtr = NULL;
   currRttMonCtrlAdminEntryPtr = NULL;

   numVarBinds = 0;

   newRttMonAdminEntryRow = FALSE;

   return;
}

/*----------------------------------------------------------------------
 * cleanup after rttMonCtrlAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int rttMonCtrlAdminEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
rttMonCtrlAdminEntry_cleanup (
   doList_t *trash)
{
   rttMonCtrlAdminEntry_free(trash->data);
   rttMonCtrlAdminEntry_free(trash->undodata);
   return (NO_ERROR);
}

/*----------------------------------------------------------------------
 * clone the rttMonCtrlAdminEntry family
 *---------------------------------------------------------------------*/
rttMonCtrlAdminEntry_t *
Clone_rttMonCtrlAdminEntry(
    rttMonCtrlAdminEntry_t *rttMonCtrlAdminEntry)
{
    rttMonCtrlAdminEntry_t *data;

    if (rttMonCtrlAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(rttMonCtrlAdminEntry_t))) == NULL) {
        return (NULL);
    }
    memcpy((char *) (data), (char *) (rttMonCtrlAdminEntry), sizeof(rttMonCtrlAdminEntry_t));

    data->rttMonCtrlAdminOwner = CloneOctetString(rttMonCtrlAdminEntry->rttMonCtrlAdminOwner);
    data->rttMonCtrlAdminTag = CloneOctetString(rttMonCtrlAdminEntry->rttMonCtrlAdminTag);
 
    /*
     *  AUGMENT Tables BUG!!!!!! Clone in the rest of the table entries
     *
     */

    data->rttMonCtrlOperDiagText = 
            CloneOctetString(rttMonCtrlAdminEntry->rttMonCtrlOperDiagText);
    data->rttMonLatestRttOperSenseDescription = 
            CloneOctetString(rttMonCtrlAdminEntry->rttMonLatestRttOperSenseDescription);
    data->rttMonLatestRttOperAddress = 
	CloneOctetString(rttMonCtrlAdminEntry->rttMonLatestRttOperAddress);

    return (data);
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
rttMonCtrlAdminEntry_test (
    OID            *incoming,
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
    rttMonCtrlAdminEntry_t     *rttMonCtrlAdminEntry;
    SR_INT32       rttMonCtrlAdminIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return (NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    rttMonCtrlAdminEntry = k_rttMonCtrlAdminEntry_get(-1, contextInfo, -1, EXACT, rttMonCtrlAdminIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rttMonCtrlAdminEntry_set) &&
            (((rttMonCtrlAdminEntry_t *) (dp->data)) != NULL) &&
            (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex == rttMonCtrlAdminIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rttMonCtrlAdminEntry_set;
        dp->cleanupMethod = rttMonCtrlAdminEntry_cleanup;
#ifdef SR_rttMonCtrlAdminEntry_UNDO
        dp->undoMethod = rttMonCtrlAdminEntry_undo;
#else /* SR_rttMonCtrlAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rttMonCtrlAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (rttMonCtrlAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rttMonCtrlAdminEntry(rttMonCtrlAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_rttMonCtrlAdminEntry(rttMonCtrlAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = malloc(sizeof(rttMonCtrlAdminEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rttMonCtrlAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex = (rttMonCtrlAdminIndex);
                SET_VALID(I_rttMonCtrlAdminIndex, ((rttMonCtrlAdminEntry_t *) (dp->data))->valid);

                error_status = k_rttMonCtrlAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    switch (object->nominator) {


#ifdef I_rttMonCtrlAdminOwner
   case I_rttMonCtrlAdminOwner:

     if (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminOwner != NULL) {
        FreeOctetString(((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminOwner);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminOwner = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonCtrlAdminOwner */

#ifdef I_rttMonCtrlAdminTag
   case I_rttMonCtrlAdminTag:

     if (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminTag != NULL) {
        FreeOctetString(((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminTag);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminTag = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonCtrlAdminTag */

#ifdef I_rttMonCtrlAdminRttType
   case I_rttMonCtrlAdminRttType:

     if ((value->sl_value < 1) || (value->sl_value > 4)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminRttType = value->sl_value;
     break;
#endif /* I_rttMonCtrlAdminRttType */

#ifdef I_rttMonCtrlAdminThreshold
   case I_rttMonCtrlAdminThreshold:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminThreshold = value->sl_value;
     break;
#endif /* I_rttMonCtrlAdminThreshold */

#ifdef I_rttMonCtrlAdminFrequency
   case I_rttMonCtrlAdminFrequency:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminFrequency = value->sl_value;
     break;
#endif /* I_rttMonCtrlAdminFrequency */

#ifdef I_rttMonCtrlAdminTimeout
   case I_rttMonCtrlAdminTimeout:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminTimeout = value->sl_value;
     break;
#endif /* I_rttMonCtrlAdminTimeout */

#ifdef I_rttMonCtrlAdminVerifyData
   case I_rttMonCtrlAdminVerifyData:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminVerifyData = value->sl_value;
     break;
#endif /* I_rttMonCtrlAdminVerifyData */

#ifdef I_rttMonCtrlAdminStatus
   case I_rttMonCtrlAdminStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return (WRONG_VALUE_ERROR);
     }

     if (value->sl_value == D_rttMonCtrlAdminStatus_notReady) {
         return (WRONG_VALUE_ERROR);
     }

     if (rttMonCtrlAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rttMonCtrlAdminStatus_notInService) || (value->sl_value == D_rttMonCtrlAdminStatus_active)) {
            return (INCONSISTENT_VALUE_ERROR);
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rttMonCtrlAdminStatus_createAndGo) || (value->sl_value == D_rttMonCtrlAdminStatus_createAndWait)) {
            return (INCONSISTENT_VALUE_ERROR);
        }
     }

     /*
      *  Transition the Status value in the k_test routine
      *     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus = value->sl_value;
      */
     break;
#endif /* I_rttMonCtrlAdminStatus */

    /*
     *  AUGMENT Tables BUG!!!!!! OperState is Settable and needs checked!!!
     *
     */

#ifdef I_rttMonScheduleAdminRttLife
   case I_rttMonScheduleAdminRttLife:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonScheduleAdminRttLife = value->sl_value;
     break;
#endif /* I_rttMonScheduleAdminRttLife */

#ifdef I_rttMonScheduleAdminRttStartTime
   case I_rttMonScheduleAdminRttStartTime:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonScheduleAdminRttStartTime = value->ul_value;
     break;
#endif /* I_rttMonScheduleAdminRttStartTime */

#ifdef I_rttMonScheduleAdminConceptRowAgeout
   case I_rttMonScheduleAdminConceptRowAgeout:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonScheduleAdminConceptRowAgeout = value->sl_value;
     break;
#endif /* I_rttMonScheduleAdminConceptRowAgeout */

#ifdef I_rttMonReactAdminConnectionEnable
   case I_rttMonReactAdminConnectionEnable:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonReactAdminConnectionEnable = value->sl_value;
     break;
#endif /* I_rttMonReactAdminConnectionEnable */

#ifdef I_rttMonReactAdminTimeoutEnable
   case I_rttMonReactAdminTimeoutEnable:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonReactAdminTimeoutEnable = value->sl_value;
     break;
#endif /* I_rttMonReactAdminTimeoutEnable */

#ifdef I_rttMonReactAdminThresholdType
   case I_rttMonReactAdminThresholdType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonReactAdminThresholdType = value->sl_value;
     break;
#endif /* I_rttMonReactAdminThresholdType */

#ifdef I_rttMonReactAdminThresholdFalling
   case I_rttMonReactAdminThresholdFalling:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonReactAdminThresholdFalling = value->sl_value;
     break;
#endif /* I_rttMonReactAdminThresholdFalling */

#ifdef I_rttMonReactAdminThresholdCount
   case I_rttMonReactAdminThresholdCount:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonReactAdminThresholdCount = value->sl_value;
     break;
#endif /* I_rttMonReactAdminThresholdCount */

#ifdef I_rttMonReactAdminThresholdCount2
   case I_rttMonReactAdminThresholdCount2:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonReactAdminThresholdCount2 = value->sl_value;
     break;
#endif /* I_rttMonReactAdminThresholdCount2 */

#ifdef I_rttMonReactAdminActionType
   case I_rttMonReactAdminActionType:

     if ((value->sl_value < 1) || (value->sl_value > 8)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonReactAdminActionType = value->sl_value;
     break;
#endif /* I_rttMonReactAdminActionType */

#ifdef I_rttMonStatisticsAdminNumHourGroups
   case I_rttMonStatisticsAdminNumHourGroups:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonStatisticsAdminNumHourGroups = value->sl_value;
     break;
#endif /* I_rttMonStatisticsAdminNumHourGroups */

#ifdef I_rttMonStatisticsAdminNumPaths
   case I_rttMonStatisticsAdminNumPaths:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonStatisticsAdminNumPaths = value->sl_value;
     break;
#endif /* I_rttMonStatisticsAdminNumPaths */

#ifdef I_rttMonStatisticsAdminNumHops
   case I_rttMonStatisticsAdminNumHops:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonStatisticsAdminNumHops = value->sl_value;
     break;
#endif /* I_rttMonStatisticsAdminNumHops */

#ifdef I_rttMonStatisticsAdminNumDistBuckets
   case I_rttMonStatisticsAdminNumDistBuckets:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonStatisticsAdminNumDistBuckets = value->sl_value;
     break;
#endif /* I_rttMonStatisticsAdminNumDistBuckets */

#ifdef I_rttMonStatisticsAdminDistInterval
   case I_rttMonStatisticsAdminDistInterval:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonStatisticsAdminDistInterval = value->sl_value;
     break;
#endif /* I_rttMonStatisticsAdminDistInterval */

#ifdef I_rttMonHistoryAdminNumLives
   case I_rttMonHistoryAdminNumLives:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonHistoryAdminNumLives = value->sl_value;
     break;
#endif /* I_rttMonHistoryAdminNumLives */

#ifdef I_rttMonHistoryAdminNumBuckets
   case I_rttMonHistoryAdminNumBuckets:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonHistoryAdminNumBuckets = value->sl_value;
     break;
#endif /* I_rttMonHistoryAdminNumBuckets */

#ifdef I_rttMonHistoryAdminNumSamples
   case I_rttMonHistoryAdminNumSamples:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonHistoryAdminNumSamples = value->sl_value;
     break;
#endif /* I_rttMonHistoryAdminNumSamples */

#ifdef I_rttMonHistoryAdminFilter
   case I_rttMonHistoryAdminFilter:

     if ((value->sl_value < 1) || (value->sl_value > 4)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonHistoryAdminFilter = value->sl_value;
     break;
#endif /* I_rttMonHistoryAdminFilter */

#ifdef I_rttMonCtrlOperModificationTime
   case I_rttMonCtrlOperModificationTime:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperModificationTime = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperModificationTime */

#ifdef I_rttMonCtrlOperDiagText
   case I_rttMonCtrlOperDiagText:

     if (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperDiagText != NULL) {
        FreeOctetString(((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperDiagText);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperDiagText = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonCtrlOperDiagText */

#ifdef I_rttMonCtrlOperResetTime
   case I_rttMonCtrlOperResetTime:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperResetTime = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperResetTime */

#ifdef I_rttMonCtrlOperOctetsInUse
   case I_rttMonCtrlOperOctetsInUse:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperOctetsInUse = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperOctetsInUse */

#ifdef I_rttMonCtrlOperConnectionLostOccurred
   case I_rttMonCtrlOperConnectionLostOccurred:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperConnectionLostOccurred = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperConnectionLostOccurred */

#ifdef I_rttMonCtrlOperTimeoutOccurred
   case I_rttMonCtrlOperTimeoutOccurred:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperTimeoutOccurred = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperTimeoutOccurred */

#ifdef I_rttMonCtrlOperOverThresholdOccurred
   case I_rttMonCtrlOperOverThresholdOccurred:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperOverThresholdOccurred = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperOverThresholdOccurred */

#ifdef I_rttMonCtrlOperNumRtts
   case I_rttMonCtrlOperNumRtts:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperNumRtts = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperNumRtts */

#ifdef I_rttMonCtrlOperRttLife
   case I_rttMonCtrlOperRttLife:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperRttLife = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperRttLife */

#ifdef I_rttMonCtrlOperState
   case I_rttMonCtrlOperState:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return (WRONG_VALUE_ERROR);
     }
     /*
      *  Transition the Status value in the k_test routine
      *  ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperState =
      *            value->sl_value;
      */

     break;
#endif /* I_rttMonCtrlOperState */

#ifdef I_rttMonLatestRttOperCompletionTime
   case I_rttMonLatestRttOperCompletionTime:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperCompletionTime = value->sl_value;
     break;
#endif /* I_rttMonLatestRttOperCompletionTime */

#ifdef I_rttMonLatestRttOperSense
   case I_rttMonLatestRttOperSense:

     if ((value->sl_value < 1) || (value->sl_value > 10)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperSense = value->sl_value;
     break;
#endif /* I_rttMonLatestRttOperSense */

#ifdef I_rttMonLatestRttOperApplSpecificSense
   case I_rttMonLatestRttOperApplSpecificSense:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperApplSpecificSense = value->sl_value;
     break;
#endif /* I_rttMonLatestRttOperApplSpecificSense */

#ifdef I_rttMonLatestRttOperSenseDescription
   case I_rttMonLatestRttOperSenseDescription:

     if (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperSenseDescription != NULL) {
        FreeOctetString(((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperSenseDescription);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperSenseDescription = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonLatestRttOperSenseDescription */

#ifdef I_rttMonLatestRttOperTime
   case I_rttMonLatestRttOperTime:

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperTime = value->sl_value;
     break;
#endif /* I_rttMonLatestRttOperTime */

#ifdef I_rttMonLatestRttOperAddress
   case I_rttMonLatestRttOperAddress:

     if (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperAddress != NULL) {
        FreeOctetString(((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperAddress);
     }

     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonLatestRttOperAddress = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonLatestRttOperAddress */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rttMonCtrlAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

   /* Do system dependent testing in k_rttMonCtrlAdminEntry_test */
   error_status = k_rttMonCtrlAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rttMonCtrlAdminEntry_t *) (dp->data))->valid);
        error_status = k_rttMonCtrlAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rttMonCtrlAdminEntry_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_rttMonCtrlAdminEntry_set((rttMonCtrlAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rttMonEchoAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonEchoAdminEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonEchoAdminEntry_t *data = NULL;
    SR_UINT32       buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonEchoAdminEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonEchoAdminProtocol
    case I_rttMonEchoAdminProtocol:
       dp = (void *) (&data->rttMonEchoAdminProtocol);
       break;
#endif /* I_rttMonEchoAdminProtocol */

#ifdef I_rttMonEchoAdminTargetAddress
    case I_rttMonEchoAdminTargetAddress:
       dp = (void *) (CloneOctetString(data->rttMonEchoAdminTargetAddress));
       break;
#endif /* I_rttMonEchoAdminTargetAddress */

#ifdef I_rttMonEchoAdminPktDataRequestSize
    case I_rttMonEchoAdminPktDataRequestSize:
       dp = (void *) (&data->rttMonEchoAdminPktDataRequestSize);
       break;
#endif /* I_rttMonEchoAdminPktDataRequestSize */

#ifdef I_rttMonEchoAdminPktDataResponseSize
    case I_rttMonEchoAdminPktDataResponseSize:
       dp = (void *) (&data->rttMonEchoAdminPktDataResponseSize);
       break;
#endif /* I_rttMonEchoAdminPktDataResponseSize */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rttMonEchoAdminEntry data object.
 *---------------------------------------------------------------------*/
void
rttMonEchoAdminEntry_free (
   rttMonEchoAdminEntry_t *data)
{

   if (data != NULL) {

     /*
      *  If createAndGo and an CtrlAdminQItem has already been
      *   created, delete it.
      */
     if ((currRttMonCtrlAdminEntryPtr != NULL) &&
	 (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus ==
	  CREATE_AND_GO))
     {
       if (getRttMonCtrlAdminQItem(
		   data->
		   rttMonCtrlAdminIndex) != NULL)
       {
	 freeRttMonCtrlAdminQItemByIndex(
			 data->
			   rttMonCtrlAdminIndex);
       }
     }

       
     FreeOctetString(data->rttMonEchoAdminTargetAddress);

     free ((char *) data);

   }

   currRttMonCtrlAdminEntryPtr = NULL;
   currRttMonEchoAdminEntryPtr = NULL;


   numVarBinds = 0;

   newRttMonAdminEntryRow = FALSE;

   return;
}

/*----------------------------------------------------------------------
 * cleanup after rttMonEchoAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int rttMonEchoAdminEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
rttMonEchoAdminEntry_cleanup (
   doList_t *trash)
{
   rttMonEchoAdminEntry_free(trash->data);
   rttMonEchoAdminEntry_free(trash->undodata);
   return (NO_ERROR);
}

/*----------------------------------------------------------------------
 * clone the rttMonEchoAdminEntry family
 *---------------------------------------------------------------------*/
rttMonEchoAdminEntry_t *
Clone_rttMonEchoAdminEntry (
    rttMonEchoAdminEntry_t *rttMonEchoAdminEntry)
{
    rttMonEchoAdminEntry_t *data;

    if (rttMonEchoAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(rttMonEchoAdminEntry_t))) == NULL) {
        return (NULL);
    }
    memcpy((char *) (data), (char *) (rttMonEchoAdminEntry), sizeof(rttMonEchoAdminEntry_t));

    data->rttMonEchoAdminTargetAddress = CloneOctetString(rttMonEchoAdminEntry->rttMonEchoAdminTargetAddress);

    return (data);
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
rttMonEchoAdminEntry_test (
    OID            *incoming,
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
    rttMonEchoAdminEntry_t     *rttMonEchoAdminEntry;
    SR_INT32       rttMonCtrlAdminIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return (NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    rttMonEchoAdminEntry = k_rttMonEchoAdminEntry_get(-1, contextInfo, -1, EXACT, rttMonCtrlAdminIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rttMonEchoAdminEntry_set) &&
            (((rttMonEchoAdminEntry_t *) (dp->data)) != NULL) &&
            (((rttMonEchoAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex == rttMonCtrlAdminIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rttMonEchoAdminEntry_set;
        dp->cleanupMethod = rttMonEchoAdminEntry_cleanup;
#ifdef SR_rttMonEchoAdminEntry_UNDO
        dp->undoMethod = rttMonEchoAdminEntry_undo;
#else /* SR_rttMonEchoAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rttMonEchoAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (rttMonEchoAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rttMonEchoAdminEntry(rttMonEchoAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_rttMonEchoAdminEntry(rttMonEchoAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = malloc(sizeof(rttMonEchoAdminEntry_t))) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rttMonEchoAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((rttMonEchoAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex = (rttMonCtrlAdminIndex);
                SET_VALID(I_rttMonCtrlAdminIndex, ((rttMonEchoAdminEntry_t *) (dp->data))->valid);

                error_status = k_rttMonEchoAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    switch (object->nominator) {

#ifdef I_rttMonEchoAdminProtocol
   case I_rttMonEchoAdminProtocol:

     if ((value->sl_value < 1) || (value->sl_value > 22)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonEchoAdminEntry_t *) (dp->data))->rttMonEchoAdminProtocol = value->sl_value;
     break;
#endif /* I_rttMonEchoAdminProtocol */

#ifdef I_rttMonEchoAdminTargetAddress
   case I_rttMonEchoAdminTargetAddress:

     if (((rttMonEchoAdminEntry_t *) (dp->data))->rttMonEchoAdminTargetAddress != NULL) {
        FreeOctetString(((rttMonEchoAdminEntry_t *) (dp->data))->rttMonEchoAdminTargetAddress);
     }

     ((rttMonEchoAdminEntry_t *) (dp->data))->rttMonEchoAdminTargetAddress = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonEchoAdminTargetAddress */

#ifdef I_rttMonEchoAdminPktDataRequestSize
   case I_rttMonEchoAdminPktDataRequestSize:

     ((rttMonEchoAdminEntry_t *) (dp->data))->rttMonEchoAdminPktDataRequestSize = value->sl_value;
     break;
#endif /* I_rttMonEchoAdminPktDataRequestSize */

#ifdef I_rttMonEchoAdminPktDataResponseSize
   case I_rttMonEchoAdminPktDataResponseSize:

     ((rttMonEchoAdminEntry_t *) (dp->data))->rttMonEchoAdminPktDataResponseSize = value->sl_value;
     break;
#endif /* I_rttMonEchoAdminPktDataResponseSize */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rttMonEchoAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

   /* Do system dependent testing in k_rttMonEchoAdminEntry_test */
   error_status = k_rttMonEchoAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rttMonEchoAdminEntry_t *) (dp->data))->valid);
        error_status = k_rttMonEchoAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rttMonEchoAdminEntry_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_rttMonEchoAdminEntry_set((rttMonEchoAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rttMonFileIOAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonFileIOAdminEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonFileIOAdminEntry_t *data=NULL;
    SR_UINT32       buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonFileIOAdminEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonFileIOAdminFilePath
    case I_rttMonFileIOAdminFilePath:
       dp = (void *) (CloneOctetString(data->rttMonFileIOAdminFilePath));
       break;
#endif /* I_rttMonFileIOAdminFilePath */

#ifdef I_rttMonFileIOAdminSize
    case I_rttMonFileIOAdminSize:
       dp = (void *) (&data->rttMonFileIOAdminSize);
       break;
#endif /* I_rttMonFileIOAdminSize */

#ifdef I_rttMonFileIOAdminAction
    case I_rttMonFileIOAdminAction:
       dp = (void *) (&data->rttMonFileIOAdminAction);
       break;
#endif /* I_rttMonFileIOAdminAction */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rttMonFileIOAdminEntry data object.
 *---------------------------------------------------------------------*/
void
rttMonFileIOAdminEntry_free (
   rttMonFileIOAdminEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->rttMonFileIOAdminFilePath);

       free ((char *) data);
   }

   numVarBinds = 0;

   return;
}

/*----------------------------------------------------------------------
 * cleanup after rttMonFileIOAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int rttMonFileIOAdminEntry_cleanup
    SR_PROTOTYPE((doList_t *trash));

static int
rttMonFileIOAdminEntry_cleanup (
   doList_t *trash)
{
   rttMonFileIOAdminEntry_free(trash->data);
   rttMonFileIOAdminEntry_free(trash->undodata);
   return (NO_ERROR);
}

/*----------------------------------------------------------------------
 * clone the rttMonFileIOAdminEntry family
 *---------------------------------------------------------------------*/
rttMonFileIOAdminEntry_t *
Clone_rttMonFileIOAdminEntry (
    rttMonFileIOAdminEntry_t *rttMonFileIOAdminEntry)
{
    rttMonFileIOAdminEntry_t *data;

    if (rttMonFileIOAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(rttMonFileIOAdminEntry_t))) == NULL) {
        return (NULL);
    }
    memcpy((char *) (data), (char *) (rttMonFileIOAdminEntry), sizeof(rttMonFileIOAdminEntry_t));

    data->rttMonFileIOAdminFilePath = CloneOctetString(rttMonFileIOAdminEntry->rttMonFileIOAdminFilePath);

    return (data);
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
rttMonFileIOAdminEntry_test (
    OID            *incoming,
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
    rttMonFileIOAdminEntry_t     *rttMonFileIOAdminEntry;
    SR_INT32       rttMonCtrlAdminIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return (NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    rttMonFileIOAdminEntry = k_rttMonFileIOAdminEntry_get(-1, contextInfo, -1, EXACT, rttMonCtrlAdminIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rttMonFileIOAdminEntry_set) &&
            (((rttMonFileIOAdminEntry_t *) (dp->data)) != NULL) &&
            (((rttMonFileIOAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex == rttMonCtrlAdminIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rttMonFileIOAdminEntry_set;
        dp->cleanupMethod = rttMonFileIOAdminEntry_cleanup;
#ifdef SR_rttMonFileIOAdminEntry_UNDO
        dp->undoMethod = rttMonFileIOAdminEntry_undo;
#else /* SR_rttMonFileIOAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rttMonFileIOAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (rttMonFileIOAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rttMonFileIOAdminEntry(rttMonFileIOAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_rttMonFileIOAdminEntry(rttMonFileIOAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ((dp->data = malloc(sizeof(rttMonFileIOAdminEntry_t))) == NULL){
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rttMonFileIOAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((rttMonFileIOAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex = (rttMonCtrlAdminIndex);
                SET_VALID(I_rttMonCtrlAdminIndex, ((rttMonFileIOAdminEntry_t *) (dp->data))->valid);

                error_status = k_rttMonFileIOAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    switch (object->nominator) {

#ifdef I_rttMonFileIOAdminFilePath
   case I_rttMonFileIOAdminFilePath:

     if (((rttMonFileIOAdminEntry_t *) (dp->data))->rttMonFileIOAdminFilePath != NULL) {
        FreeOctetString(((rttMonFileIOAdminEntry_t *) (dp->data))->rttMonFileIOAdminFilePath);
     }

     ((rttMonFileIOAdminEntry_t *) (dp->data))->rttMonFileIOAdminFilePath = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonFileIOAdminFilePath */

#ifdef I_rttMonFileIOAdminSize
   case I_rttMonFileIOAdminSize:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonFileIOAdminEntry_t *) (dp->data))->rttMonFileIOAdminSize = value->sl_value;
     break;
#endif /* I_rttMonFileIOAdminSize */

#ifdef I_rttMonFileIOAdminAction
   case I_rttMonFileIOAdminAction:

     if ((value->sl_value < 1) || (value->sl_value > 3)) {
         return (WRONG_VALUE_ERROR);
     }

     ((rttMonFileIOAdminEntry_t *) (dp->data))->rttMonFileIOAdminAction = value->sl_value;
     break;
#endif /* I_rttMonFileIOAdminAction */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rttMonFileIOAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

   /* Do system dependent testing in k_rttMonFileIOAdminEntry_test */
   error_status = k_rttMonFileIOAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rttMonFileIOAdminEntry_t *) (dp->data))->valid);
        error_status = k_rttMonFileIOAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rttMonFileIOAdminEntry_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_rttMonFileIOAdminEntry_set((rttMonFileIOAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rttMonScriptAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonScriptAdminEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonScriptAdminEntry_t *data=NULL;
    SR_UINT32       buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonScriptAdminEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonScriptAdminName
    case I_rttMonScriptAdminName:
       dp = (void *) (CloneOctetString(data->rttMonScriptAdminName));
       break;
#endif /* I_rttMonScriptAdminName */

#ifdef I_rttMonScriptAdminCmdLineParams
    case I_rttMonScriptAdminCmdLineParams:
       dp = (void *) (CloneOctetString(data->rttMonScriptAdminCmdLineParams));
       break;
#endif /* I_rttMonScriptAdminCmdLineParams */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rttMonScriptAdminEntry data object.
 *---------------------------------------------------------------------*/
void
rttMonScriptAdminEntry_free (
   rttMonScriptAdminEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->rttMonScriptAdminName);
       FreeOctetString(data->rttMonScriptAdminCmdLineParams);

       free ((char *) data);
   }

   numVarBinds = 0;

   return;

}

/*----------------------------------------------------------------------
 * cleanup after rttMonScriptAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
rttMonScriptAdminEntry_cleanup (
   doList_t *trash)
{
   rttMonScriptAdminEntry_free(trash->data);
   rttMonScriptAdminEntry_free(trash->undodata);
   return (NO_ERROR);
}

/*----------------------------------------------------------------------
 * clone the rttMonScriptAdminEntry family
 *---------------------------------------------------------------------*/
rttMonScriptAdminEntry_t *
Clone_rttMonScriptAdminEntry (
    rttMonScriptAdminEntry_t *rttMonScriptAdminEntry)
{
    rttMonScriptAdminEntry_t *data;

    if (rttMonScriptAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(rttMonScriptAdminEntry_t))) == NULL) {
        return (NULL);
    }
    memcpy((char *) (data), (char *) (rttMonScriptAdminEntry), sizeof(rttMonScriptAdminEntry_t));

    data->rttMonScriptAdminName = CloneOctetString(rttMonScriptAdminEntry->rttMonScriptAdminName);
    data->rttMonScriptAdminCmdLineParams = CloneOctetString(rttMonScriptAdminEntry->rttMonScriptAdminCmdLineParams);

    return (data);
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
rttMonScriptAdminEntry_test (
    OID            *incoming,
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
    rttMonScriptAdminEntry_t     *rttMonScriptAdminEntry;
    SR_INT32       rttMonCtrlAdminIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return (NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    rttMonScriptAdminEntry = k_rttMonScriptAdminEntry_get(-1, contextInfo, -1, EXACT, rttMonCtrlAdminIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rttMonScriptAdminEntry_set) &&
            (((rttMonScriptAdminEntry_t *) (dp->data)) != NULL) &&
            (((rttMonScriptAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex == rttMonCtrlAdminIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rttMonScriptAdminEntry_set;
        dp->cleanupMethod = rttMonScriptAdminEntry_cleanup;
#ifdef SR_rttMonScriptAdminEntry_UNDO
        dp->undoMethod = rttMonScriptAdminEntry_undo;
#else /* SR_rttMonScriptAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rttMonScriptAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (rttMonScriptAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rttMonScriptAdminEntry(rttMonScriptAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_rttMonScriptAdminEntry(rttMonScriptAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
	    if ( (dp->data = malloc(sizeof(rttMonScriptAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rttMonScriptAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((rttMonScriptAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex = (rttMonCtrlAdminIndex);
                SET_VALID(I_rttMonCtrlAdminIndex, ((rttMonScriptAdminEntry_t *) (dp->data))->valid);

                error_status = k_rttMonScriptAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    switch (object->nominator) {

#ifdef I_rttMonScriptAdminName
   case I_rttMonScriptAdminName:

     if (((rttMonScriptAdminEntry_t *) (dp->data))->rttMonScriptAdminName != NULL) {
        FreeOctetString(((rttMonScriptAdminEntry_t *) (dp->data))->rttMonScriptAdminName);
     }

     ((rttMonScriptAdminEntry_t *) (dp->data))->rttMonScriptAdminName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonScriptAdminName */

#ifdef I_rttMonScriptAdminCmdLineParams
   case I_rttMonScriptAdminCmdLineParams:

     if (((rttMonScriptAdminEntry_t *) (dp->data))->rttMonScriptAdminCmdLineParams != NULL) {
        FreeOctetString(((rttMonScriptAdminEntry_t *) (dp->data))->rttMonScriptAdminCmdLineParams);
     }

     ((rttMonScriptAdminEntry_t *) (dp->data))->rttMonScriptAdminCmdLineParams = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_rttMonScriptAdminCmdLineParams */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rttMonScriptAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

   /* Do system dependent testing in k_rttMonScriptAdminEntry_test */
   error_status = k_rttMonScriptAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rttMonScriptAdminEntry_t *) (dp->data))->valid);
        error_status = k_rttMonScriptAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rttMonScriptAdminEntry_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_rttMonScriptAdminEntry_set((rttMonScriptAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rttMonReactTriggerAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonReactTriggerAdminEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonReactTriggerAdminEntry_t *data = NULL;
    SR_UINT32       buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;
    SR_INT32        rttMonReactTriggerAdminRttMonCtrlAdminIndex;


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

    if ( (InstToInt(incoming, 1 + object->oid.length, &rttMonReactTriggerAdminRttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonReactTriggerAdminRttMonCtrlAdminIndex = MAX(1, rttMonReactTriggerAdminRttMonCtrlAdminIndex);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonCtrlAdminIndex < 1) {
	    rttMonReactTriggerAdminRttMonCtrlAdminIndex = 1;
	}
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonReactTriggerAdminEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex, rttMonReactTriggerAdminRttMonCtrlAdminIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
        inst.oid_ptr[1] = data->rttMonReactTriggerAdminRttMonCtrlAdminIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonReactTriggerAdminStatus
    case I_rttMonReactTriggerAdminStatus:
       dp = (void *) (&data->rttMonReactTriggerAdminStatus);
       break;
#endif /* I_rttMonReactTriggerAdminStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the rttMonReactTriggerAdminEntry data object.
 *---------------------------------------------------------------------*/
void
rttMonReactTriggerAdminEntry_free (
   rttMonReactTriggerAdminEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }

   numVarBinds = 0;

   newRttMonReactTriggerAdminEntryRow = FALSE;

   return;

}

/*----------------------------------------------------------------------
 * cleanup after rttMonReactTriggerAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
rttMonReactTriggerAdminEntry_cleanup (
   doList_t *trash)
{
   rttMonReactTriggerAdminEntry_free(trash->data);
   rttMonReactTriggerAdminEntry_free(trash->undodata);
   return (NO_ERROR);
}

/*----------------------------------------------------------------------
 * clone the rttMonReactTriggerAdminEntry family
 *---------------------------------------------------------------------*/
rttMonReactTriggerAdminEntry_t *
Clone_rttMonReactTriggerAdminEntry (
    rttMonReactTriggerAdminEntry_t *rttMonReactTriggerAdminEntry)
{
    rttMonReactTriggerAdminEntry_t *data;

    if (rttMonReactTriggerAdminEntry == NULL) {
        return NULL;
    }

    if ((data = malloc(sizeof(rttMonReactTriggerAdminEntry_t))) == NULL) {
        return (NULL);
    }
    memcpy((char *) (data), (char *) (rttMonReactTriggerAdminEntry), sizeof(rttMonReactTriggerAdminEntry_t));


    return (data);
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
rttMonReactTriggerAdminEntry_test (
    OID            *incoming,
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
    rttMonReactTriggerAdminEntry_t     *rttMonReactTriggerAdminEntry;
    SR_INT32       rttMonCtrlAdminIndex;
    SR_INT32       rttMonReactTriggerAdminRttMonCtrlAdminIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 2) {
        return (NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &rttMonReactTriggerAdminRttMonCtrlAdminIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    rttMonReactTriggerAdminEntry = k_rttMonReactTriggerAdminEntry_get(-1, contextInfo, -1, EXACT, rttMonCtrlAdminIndex, rttMonReactTriggerAdminRttMonCtrlAdminIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == rttMonReactTriggerAdminEntry_set) &&
            (((rttMonReactTriggerAdminEntry_t *) (dp->data)) != NULL) &&
            (((rttMonReactTriggerAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex == rttMonCtrlAdminIndex) &&
            (((rttMonReactTriggerAdminEntry_t *) (dp->data))->rttMonReactTriggerAdminRttMonCtrlAdminIndex == rttMonReactTriggerAdminRttMonCtrlAdminIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = rttMonReactTriggerAdminEntry_set;
        dp->cleanupMethod = rttMonReactTriggerAdminEntry_cleanup;
#ifdef SR_rttMonReactTriggerAdminEntry_UNDO
        dp->undoMethod = rttMonReactTriggerAdminEntry_undo;
#else /* SR_rttMonReactTriggerAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_rttMonReactTriggerAdminEntry_UNDO */
        dp->state = UNKNOWN;

        if (rttMonReactTriggerAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_rttMonReactTriggerAdminEntry(rttMonReactTriggerAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            if ((dp->undodata = (void *) Clone_rttMonReactTriggerAdminEntry(rttMonReactTriggerAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }

        }
        else {
            if ( (dp->data = malloc(sizeof(rttMonReactTriggerAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(rttMonReactTriggerAdminEntry_t));
                dp->undodata = NULL;

                /* Fill in reasonable default values for this new entry */
                ((rttMonReactTriggerAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex = (rttMonCtrlAdminIndex);
                SET_VALID(I_rttMonCtrlAdminIndex, ((rttMonReactTriggerAdminEntry_t *) (dp->data))->valid);

                ((rttMonReactTriggerAdminEntry_t *) (dp->data))->rttMonReactTriggerAdminRttMonCtrlAdminIndex = (rttMonReactTriggerAdminRttMonCtrlAdminIndex);
                SET_VALID(I_rttMonReactTriggerAdminRttMonCtrlAdminIndex, ((rttMonReactTriggerAdminEntry_t *) (dp->data))->valid);

                error_status = k_rttMonReactTriggerAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return (error_status);
    }

    switch (object->nominator) {

#ifdef I_rttMonReactTriggerAdminStatus
   case I_rttMonReactTriggerAdminStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return (WRONG_VALUE_ERROR);
     }

     if (value->sl_value == D_rttMonReactTriggerAdminStatus_notReady) {
         return (WRONG_VALUE_ERROR);
     }

     if (rttMonReactTriggerAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_rttMonReactTriggerAdminStatus_notInService) || (value->sl_value == D_rttMonReactTriggerAdminStatus_active)) {
            return (INCONSISTENT_VALUE_ERROR);
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_rttMonReactTriggerAdminStatus_createAndGo) || (value->sl_value == D_rttMonReactTriggerAdminStatus_createAndWait)) {
            return (INCONSISTENT_VALUE_ERROR);
        }
     }

     ((rttMonReactTriggerAdminEntry_t *) (dp->data))->rttMonReactTriggerAdminStatus = value->sl_value;
     break;
#endif /* I_rttMonReactTriggerAdminStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in rttMonReactTriggerAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

   /* Do system dependent testing in k_rttMonReactTriggerAdminEntry_test */
   error_status = k_rttMonReactTriggerAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((rttMonReactTriggerAdminEntry_t *) (dp->data))->valid);
        error_status = k_rttMonReactTriggerAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
rttMonReactTriggerAdminEntry_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_rttMonReactTriggerAdminEntry_set((rttMonReactTriggerAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the rttMonStatsCaptureEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonStatsCaptureEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonStatsCaptureEntry_t *data = NULL;
    SR_UINT32       buffer[5];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;
    SR_UINT32       rttMonStatsCaptureStartTimeIndex;
    SR_INT32        rttMonStatsCapturePathIndex;
    SR_INT32        rttMonStatsCaptureHopIndex;
    SR_INT32        rttMonStatsCaptureDistIndex;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 5 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 5) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 4 + object->oid.length, &rttMonStatsCaptureDistIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonStatsCaptureDistIndex = MAX(1, rttMonStatsCaptureDistIndex);
    }

    if ( (InstToInt(incoming, 3 + object->oid.length, &rttMonStatsCaptureHopIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonStatsCaptureHopIndex < 1) {
	    rttMonStatsCaptureDistIndex = 1;
	}
        rttMonStatsCaptureHopIndex = MAX(1, rttMonStatsCaptureHopIndex);
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &rttMonStatsCapturePathIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonStatsCapturePathIndex < 1) {
	    rttMonStatsCaptureDistIndex = 1;
	    rttMonStatsCaptureHopIndex = 1;
	}
        rttMonStatsCapturePathIndex = MAX(1, rttMonStatsCapturePathIndex);
    }

    if ( (InstToUInt(incoming, 1 + object->oid.length, &rttMonStatsCaptureStartTimeIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 *   UInt do not have to start at 1, thus not MAX call
	 */
	if (rttMonStatsCaptureStartTimeIndex < 1) {
	    rttMonStatsCaptureDistIndex = 1;
	    rttMonStatsCaptureHopIndex = 1;
	    rttMonStatsCapturePathIndex = 1;
	}
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonCtrlAdminIndex < 1) {
	    rttMonStatsCaptureDistIndex = 1;
	    rttMonStatsCaptureHopIndex = 1;
	    rttMonStatsCapturePathIndex = 1;
	    rttMonStatsCaptureStartTimeIndex = 1;
	}
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonStatsCaptureEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex, rttMonStatsCaptureStartTimeIndex, rttMonStatsCapturePathIndex, rttMonStatsCaptureHopIndex, rttMonStatsCaptureDistIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 5;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
        inst.oid_ptr[1] = data->rttMonStatsCaptureStartTimeIndex;
        inst.oid_ptr[2] = data->rttMonStatsCapturePathIndex;
        inst.oid_ptr[3] = data->rttMonStatsCaptureHopIndex;
        inst.oid_ptr[4] = data->rttMonStatsCaptureDistIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonStatsCaptureCompletions
    case I_rttMonStatsCaptureCompletions:
       dp = (void *) (&data->rttMonStatsCaptureCompletions);
       break;
#endif /* I_rttMonStatsCaptureCompletions */

#ifdef I_rttMonStatsCaptureOverThresholds
    case I_rttMonStatsCaptureOverThresholds:
       dp = (void *) (&data->rttMonStatsCaptureOverThresholds);
       break;
#endif /* I_rttMonStatsCaptureOverThresholds */

#ifdef I_rttMonStatsCaptureSumCompletionTime
    case I_rttMonStatsCaptureSumCompletionTime:
       dp = (void *) (&data->rttMonStatsCaptureSumCompletionTime);
       break;
#endif /* I_rttMonStatsCaptureSumCompletionTime */

#ifdef I_rttMonStatsCaptureSumCompletionTime2Low
    case I_rttMonStatsCaptureSumCompletionTime2Low:
       dp = (void *) (&data->rttMonStatsCaptureSumCompletionTime2Low);
       break;
#endif /* I_rttMonStatsCaptureSumCompletionTime2Low */

#ifdef I_rttMonStatsCaptureSumCompletionTime2High
    case I_rttMonStatsCaptureSumCompletionTime2High:
       dp = (void *) (&data->rttMonStatsCaptureSumCompletionTime2High);
       break;
#endif /* I_rttMonStatsCaptureSumCompletionTime2High */

#ifdef I_rttMonStatsCaptureCompletionTimeMax
    case I_rttMonStatsCaptureCompletionTimeMax:
       dp = (void *) (&data->rttMonStatsCaptureCompletionTimeMax);
       break;
#endif /* I_rttMonStatsCaptureCompletionTimeMax */

#ifdef I_rttMonStatsCaptureCompletionTimeMin
    case I_rttMonStatsCaptureCompletionTimeMin:
       dp = (void *) (&data->rttMonStatsCaptureCompletionTimeMin);
       break;
#endif /* I_rttMonStatsCaptureCompletionTimeMin */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rttMonStatsCollectEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonStatsCollectEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonStatsCollectEntry_t *data = NULL;
    SR_UINT32       buffer[4];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;
    SR_UINT32       rttMonStatsCaptureStartTimeIndex;
    SR_INT32        rttMonStatsCapturePathIndex;
    SR_INT32        rttMonStatsCaptureHopIndex;


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

    if ( (InstToInt(incoming, 3 + object->oid.length, &rttMonStatsCaptureHopIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonStatsCaptureHopIndex = MAX(1, rttMonStatsCaptureHopIndex);
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &rttMonStatsCapturePathIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonStatsCapturePathIndex < 1) {
	    rttMonStatsCaptureHopIndex = 1;
	}
        rttMonStatsCapturePathIndex = MAX(1, rttMonStatsCapturePathIndex);
    }

    if ( (InstToUInt(incoming, 1 + object->oid.length, &rttMonStatsCaptureStartTimeIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 *   UInt do not have to start at 1, thus not MAX call
	 */
	if (rttMonStatsCaptureStartTimeIndex < 1) {
	    rttMonStatsCaptureHopIndex = 1;
	    rttMonStatsCapturePathIndex = 1;
	}
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonCtrlAdminIndex < 1) {
	    rttMonStatsCaptureHopIndex = 1;
	    rttMonStatsCapturePathIndex = 1;
	    rttMonStatsCaptureStartTimeIndex = 1;
	}
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonStatsCollectEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex, rttMonStatsCaptureStartTimeIndex, rttMonStatsCapturePathIndex, rttMonStatsCaptureHopIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 4;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
        inst.oid_ptr[1] = data->rttMonStatsCaptureStartTimeIndex;
        inst.oid_ptr[2] = data->rttMonStatsCapturePathIndex;
        inst.oid_ptr[3] = data->rttMonStatsCaptureHopIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonStatsCollectNumDisconnects
    case I_rttMonStatsCollectNumDisconnects:
       dp = (void *) (&data->rttMonStatsCollectNumDisconnects);
       break;
#endif /* I_rttMonStatsCollectNumDisconnects */

#ifdef I_rttMonStatsCollectTimeouts
    case I_rttMonStatsCollectTimeouts:
       dp = (void *) (&data->rttMonStatsCollectTimeouts);
       break;
#endif /* I_rttMonStatsCollectTimeouts */

#ifdef I_rttMonStatsCollectBusies
    case I_rttMonStatsCollectBusies:
       dp = (void *) (&data->rttMonStatsCollectBusies);
       break;
#endif /* I_rttMonStatsCollectBusies */

#ifdef I_rttMonStatsCollectNoConnections
    case I_rttMonStatsCollectNoConnections:
       dp = (void *) (&data->rttMonStatsCollectNoConnections);
       break;
#endif /* I_rttMonStatsCollectNoConnections */

#ifdef I_rttMonStatsCollectDrops
    case I_rttMonStatsCollectDrops:
       dp = (void *) (&data->rttMonStatsCollectDrops);
       break;
#endif /* I_rttMonStatsCollectDrops */

#ifdef I_rttMonStatsCollectSequenceErrors
    case I_rttMonStatsCollectSequenceErrors:
       dp = (void *) (&data->rttMonStatsCollectSequenceErrors);
       break;
#endif /* I_rttMonStatsCollectSequenceErrors */

#ifdef I_rttMonStatsCollectVerifyErrors
    case I_rttMonStatsCollectVerifyErrors:
       dp = (void *) (&data->rttMonStatsCollectVerifyErrors);
       break;
#endif /* I_rttMonStatsCollectVerifyErrors */

#ifdef I_rttMonStatsCollectAddress
    case I_rttMonStatsCollectAddress:
       dp = (void *) (CloneOctetString(data->rttMonStatsCollectAddress));
       break;
#endif /* I_rttMonStatsCollectAddress */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rttMonStatsTotalsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonStatsTotalsEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonStatsTotalsEntry_t *data = NULL;
    SR_UINT32       buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;
    SR_UINT32       rttMonStatsCaptureStartTimeIndex;


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

    if ( (InstToUInt(incoming, 1 + object->oid.length, &rttMonStatsCaptureStartTimeIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonCtrlAdminIndex < 1) {
	    rttMonStatsCaptureStartTimeIndex = 1;
	}
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonStatsTotalsEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex, rttMonStatsCaptureStartTimeIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
        inst.oid_ptr[1] = data->rttMonStatsCaptureStartTimeIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonStatsTotalsElapsedTime
    case I_rttMonStatsTotalsElapsedTime:
       dp = (void *) (&data->rttMonStatsTotalsElapsedTime);
       break;
#endif /* I_rttMonStatsTotalsElapsedTime */

#ifdef I_rttMonStatsTotalsInitiations
    case I_rttMonStatsTotalsInitiations:
       dp = (void *) (&data->rttMonStatsTotalsInitiations);
       break;
#endif /* I_rttMonStatsTotalsInitiations */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the rttMonHistoryCollectionEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
rttMonHistoryCollectionEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    rttMonHistoryCollectionEntry_t *data = NULL;
    SR_UINT32       buffer[4];
    OID             inst;
    int             carry;
    SR_INT32        rttMonCtrlAdminIndex;
    SR_INT32        rttMonHistoryCollectionLifeIndex;
    SR_INT32        rttMonHistoryCollectionBucketIndex;
    SR_INT32        rttMonHistoryCollectionSampleIndex;


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

    if ( (InstToInt(incoming, 3 + object->oid.length, &rttMonHistoryCollectionSampleIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        rttMonHistoryCollectionSampleIndex = MAX(1, rttMonHistoryCollectionSampleIndex);
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &rttMonHistoryCollectionBucketIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonHistoryCollectionBucketIndex < 1) {
	    rttMonHistoryCollectionSampleIndex = 1;
	}
        rttMonHistoryCollectionBucketIndex = MAX(1, rttMonHistoryCollectionBucketIndex);
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &rttMonHistoryCollectionLifeIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonHistoryCollectionLifeIndex < 1) {
	    rttMonHistoryCollectionSampleIndex = 1;
	    rttMonHistoryCollectionBucketIndex = 1;
	}
        rttMonHistoryCollectionLifeIndex = MAX(1, rttMonHistoryCollectionLifeIndex);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &rttMonCtrlAdminIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
	/*
	 *  Support getNext X.0.3 to return X.1.1 and NOT X.1.4
	 */
	if (rttMonCtrlAdminIndex < 1) {
	    rttMonHistoryCollectionSampleIndex = 1;
	    rttMonHistoryCollectionBucketIndex = 1;
	    rttMonHistoryCollectionLifeIndex = 1;
	}
        rttMonCtrlAdminIndex = MAX(1, rttMonCtrlAdminIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_rttMonHistoryCollectionEntry_get(serialNum, contextInfo, arg ,searchType, rttMonCtrlAdminIndex, rttMonHistoryCollectionLifeIndex, rttMonHistoryCollectionBucketIndex, rttMonHistoryCollectionSampleIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 4;
        inst.oid_ptr[0] = data->rttMonCtrlAdminIndex;
        inst.oid_ptr[1] = data->rttMonHistoryCollectionLifeIndex;
        inst.oid_ptr[2] = data->rttMonHistoryCollectionBucketIndex;
        inst.oid_ptr[3] = data->rttMonHistoryCollectionSampleIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_rttMonHistoryCollectionSampleTime
    case I_rttMonHistoryCollectionSampleTime:
       dp = (void *) (&data->rttMonHistoryCollectionSampleTime);
       break;
#endif /* I_rttMonHistoryCollectionSampleTime */

#ifdef I_rttMonHistoryCollectionAddress
    case I_rttMonHistoryCollectionAddress:
       dp = (void *) (CloneOctetString(data->rttMonHistoryCollectionAddress));
       break;
#endif /* I_rttMonHistoryCollectionAddress */

#ifdef I_rttMonHistoryCollectionCompletionTime
    case I_rttMonHistoryCollectionCompletionTime:
       dp = (void *) (&data->rttMonHistoryCollectionCompletionTime);
       break;
#endif /* I_rttMonHistoryCollectionCompletionTime */

#ifdef I_rttMonHistoryCollectionSense
    case I_rttMonHistoryCollectionSense:
       dp = (void *) (&data->rttMonHistoryCollectionSense);
       break;
#endif /* I_rttMonHistoryCollectionSense */

#ifdef I_rttMonHistoryCollectionApplSpecificSense
    case I_rttMonHistoryCollectionApplSpecificSense:
       dp = (void *) (&data->rttMonHistoryCollectionApplSpecificSense);
       break;
#endif /* I_rttMonHistoryCollectionApplSpecificSense */

#ifdef I_rttMonHistoryCollectionSenseDescription
    case I_rttMonHistoryCollectionSenseDescription:
       dp = (void *) (CloneOctetString(data->rttMonHistoryCollectionSenseDescription));
       break;
#endif /* I_rttMonHistoryCollectionSenseDescription */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */





rttMonAppl_t *
k_rttMonAppl_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{
   return(getRttMonApplPtr());
}

#ifdef SETS
int
k_rttMonAppl_test (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
    numVarBinds++;
    
    switch (object->nominator) {

#ifdef I_rttMonApplReset
   case I_rttMonApplReset:

     if (value->sl_value != D_rttMonApplReset_reset) {
         return (WRONG_VALUE_ERROR);
     }
     if (numVarBinds != 1)
     {
       return (INCONSISTENT_VALUE_ERROR);
     }
     break;
#endif /* I_rttMonApplReset */

#ifdef I_rttMonApplPreConfigedReset
   case I_rttMonApplPreConfigedReset:

     if (value->sl_value != D_rttMonApplPreConfigedReset_reset) {
         return (WRONG_VALUE_ERROR);
     }
     if (numVarBinds != 1)
     {
       return (INCONSISTENT_VALUE_ERROR);
     }
     break;
#endif /* I_rttMonApplPreConfigedReset */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in k_rttMonAppl_test)\n"));
       return (GEN_ERROR);

   }        /* switch */


    return (NO_ERROR);
}

int
k_rttMonAppl_ready (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{
    dp->state = ADD_MODIFY;
    return (NO_ERROR);
}

int
k_rttMonAppl_set (
   rttMonAppl_t *data,
   ContextInfo *contextInfo,
   int function)
{
   numVarBinds = 0;

   if (VALID(I_rttMonApplReset, data->valid))
   {
     setRttMonApplReset();
   }

   if (VALID(I_rttMonApplPreConfigedReset, data->valid))
   {
     setRttMonAppPreConfigedReset();
   }
   return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_rttMonAppl_UNDO
/* add #define SR_rttMonAppl_UNDO in rttmib.h to
 * include the undo routine for the rttMonAppl family.
 */
int
rttMonAppl_undo (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return (UNDO_FAILED_ERROR);
}
#endif /* SR_rttMonAppl_UNDO */

rttMonApplSupportedRttTypesEntry_t *
k_rttMonApplSupportedRttTypesEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonApplSupportedRttTypes)
{
  if (searchType == EXACT)
  {
    return (getRttMonApplSupportedRttTypesEntryPtr(
		      rttMonApplSupportedRttTypes));
  }
  else
  {
    return (getNextRttMonApplSupportedRttTypesEntryPtr(
		      rttMonApplSupportedRttTypes));
  }
}

rttMonApplSupportedProtocolsEntry_t *
k_rttMonApplSupportedProtocolsEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonApplSupportedProtocols)
{
  if (searchType == EXACT)
  {
    return (getRttMonApplSupportedProtocolsEntryPtr(
                     rttMonApplSupportedProtocols)); 
  }
  else
  {
    return (getNextRttMonApplSupportedProtocolsEntryPtr(
                     rttMonApplSupportedProtocols)); 

  }
}

rttMonApplPreConfigedEntry_t *
k_rttMonApplPreConfigedEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonApplPreConfigedType,
   OctetString * rttMonApplPreConfigedName)
{
  return (NULL);
}

rttMonCtrlAdminEntry_t *
k_rttMonCtrlAdminEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex)
{
   int         return_code;

   if (searchType == EXACT)
   {
     return_code = getRttMonCtrlAdminEntry(
                     rttMonCtrlAdminIndex,
                     rttMonCtrlAdminEntryData);
   }
   else
   {
     return_code = getNextRttMonCtrlAdminEntry(
                     rttMonCtrlAdminIndex,
                     rttMonCtrlAdminEntryData);
   }

   if (return_code != D_Return_Successfull)
   {
     return(NULL);
   }
   else
   {
     /* for internal calls don't update the adminStatus */
     if (serialNum > -1) {
	 if (rttMonCtrlAdminEntryData->
	     rttMonCtrlAdminStatus ==
             NOT_IN_SERVICE_AND_CHANGABLE) {
	     rttMonCtrlAdminEntryData->
		 rttMonCtrlAdminStatus =
		     NOT_IN_SERVICE;
	 }
     }

   }

   return(rttMonCtrlAdminEntryData);

}

#ifdef SETS

static void
setRttMonEchoAdminEntryPtr (
  rttMonEchoAdminEntry_t *tmpRttMonEchoAdminEntry)
{
  /* 
   *  Since the tmpRttMonEchoAdminEntry is a global
   *   buffer, it is ok to update the 
   *   currRttMonEchoAdminEntryPtr without checking
   */
  currRttMonEchoAdminEntryPtr =
    tmpRttMonEchoAdminEntry;

  /* 
   * No var in the Ctrl Table may be bound and
   *  thus lets get it from the real table
   *  entry.
   */
  if (currRttMonCtrlAdminEntryPtr == NULL)
  { 
    currRttMonCtrlAdminEntryPtr =
        k_rttMonCtrlAdminEntry_get(
              -2,
              NULL,
              -2,
              EXACT,
              tmpRttMonEchoAdminEntry->
                  rttMonCtrlAdminIndex);
  }
    
  return;
}

static void
setRttMonCtrlAdminEntryPtr (
  rttMonCtrlAdminEntry_t *tmpRttMonCtrlAdminEntry)
{

  /* 
   *  Since the tmpRttMonCtrlAdminEntry is a global
   *   buffer, it is ok to update the 
   *   currRttMonCtrlAdminEntryPtr without checking
   */
  currRttMonCtrlAdminEntryPtr =
    tmpRttMonCtrlAdminEntry;

  /* 
   * No var in the Echo Table may be bound and
   *  thus lets get it from the real table
   *  entry.
   */
  if (currRttMonEchoAdminEntryPtr == NULL)
  {
    currRttMonEchoAdminEntryPtr =
        k_rttMonEchoAdminEntry_get(
              -2,
              NULL,
              -2,
              EXACT,
              tmpRttMonCtrlAdminEntry->
                  rttMonCtrlAdminIndex);
  }

  return;
}


int
k_rttMonCtrlAdminEntry_test (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
    int                    return_code;
    int                    tmp_doList_state;
    rttMonApplSupportedRttTypesEntry_t *tmpRttTypesPtr;

    setRttMonCtrlAdminEntryPtr(((rttMonCtrlAdminEntry_t *) (dp->data)));

    numVarBinds++;

    /*
     *  Check the index values
     */
    if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex <
	MIN_rttMonCtrlAdminIdx) ||
	(((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex >
	MAX_rttMonCtrlAdminIdx))
    {
	return (GEN_ERROR);
    }
  
    /*
     *  If this is a new row being created, check to see if this new
     *    row will put us over the maximum number of rows that we 
     *    support.
     *  This function is called numerous times, one for each varbind,
     *    thus when this is a new row being created, only check on
     *    on the first varbind.  After the first varbind a placeholder
     *    is created and the number of rows will be equal to the 
     *    max, but in this situation this is ok.
     */
    if (newRttMonAdminEntryRow && 
	(numVarBinds == 1) &&
	(getRttMonCtrlAdminQSize() >=
	 SYS_MAX_rttMonApplNumCtrlAdminEntry)) {
	return (GEN_ERROR);
    }


    /*
     *  All Enumerated types have been checked, but the ranges have not.
     */

    switch (object->nominator) {

#ifdef I_rttMonCtrlAdminOwner
   case I_rttMonCtrlAdminOwner:
       if ((value->os_value->length < MIN_rttMonCtrlAdminOwner) ||
	   (value->os_value->length > MAX_rttMonCtrlAdminOwner))
       {
	   return (WRONG_LENGTH_ERROR);
       }
       if (!IsDisplayString(value->os_value))
       {
	   return (WRONG_VALUE_ERROR);
       }
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonCtrlAdminOwner */

#ifdef I_rttMonCtrlAdminTag
   case I_rttMonCtrlAdminTag:
       if ((value->os_value->length < MIN_rttMonCtrlAdminTag) ||
	   (value->os_value->length > MAX_rttMonCtrlAdminTag))
       {
	   return (WRONG_LENGTH_ERROR);
       }
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonCtrlAdminTag */

#ifdef I_rttMonCtrlAdminRttType
   case I_rttMonCtrlAdminRttType:
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	 return (INCONSISTENT_VALUE_ERROR);
       }
       /*
	* check the supported RttTypes
	*/
       tmpRttTypesPtr = 
	 getRttMonApplSupportedRttTypesEntryPtr(value->sl_value);
       if ((tmpRttTypesPtr == NULL) ||
	   (tmpRttTypesPtr->
	    rttMonApplSupportedRttTypesValid ==
	    D_rttMonApplSupportedRttTypesValid_false))
       {
	 return (INCONSISTENT_VALUE_ERROR);	 
       }

     break;
#endif /* I_rttMonCtrlAdminRttType */

#ifdef I_rttMonCtrlAdminThreshold
   case I_rttMonCtrlAdminThreshold:
       if ((value->sl_value < MIN_rttMonCtrlAdminThreshold) ||
	   (value->sl_value > MAX_rttMonCtrlAdminThreshold))
       {
	   return (WRONG_VALUE_ERROR);
       }
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonCtrlAdminThreshold */

#ifdef I_rttMonCtrlAdminFrequency
   case I_rttMonCtrlAdminFrequency:
	if ((value->sl_value < MIN_rttMonCtrlAdminFrequency) ||
	    (value->sl_value > MAX_rttMonCtrlAdminFrequency) ||
	    ((value->sl_value * 1000) < 
	     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminTimeout))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonCtrlAdminFrequency */

#ifdef I_rttMonCtrlAdminTimeout
   case I_rttMonCtrlAdminTimeout:
	if ((value->sl_value < MIN_rttMonCtrlAdminTimeout) ||
	    (value->sl_value > MAX_rttMonCtrlAdminTimeout) ||
	    (value->sl_value > 
	    (((rttMonCtrlAdminEntry_t *)
	      (dp->data))->rttMonCtrlAdminFrequency * 1000)))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonCtrlAdminTimeout */

#ifdef I_rttMonCtrlAdminVerifyData
   case I_rttMonCtrlAdminVerifyData:
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonCtrlAdminVerifyData */

#ifdef I_rttMonCtrlAdminStatus
   case I_rttMonCtrlAdminStatus:
      if ((value->sl_value != DESTROY)  &&
	  (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperState ==
                D_rttMonCtrlOperState_active))
      {
	   return (INCONSISTENT_VALUE_ERROR);
      }
      /* 
       * tmp_doList_state is not used.
       */
      if (newRttMonAdminEntryRow == TRUE)
      {
	/*
	 * NOT_IN_SERVICE_AND_CHANGABLE allow all 
	 *   required values to be set with good values, 
	 *   but still change any value before
	 *   attempting to set the conceptual row
	 *   to ACTIVE.
	 */
	if ((((rttMonCtrlAdminEntry_t
	      *)(dp->data))->rttMonCtrlAdminStatus) ==
	    NOT_IN_SERVICE_AND_CHANGABLE) {
	    (((rttMonCtrlAdminEntry_t *) 
	      (dp->data))->rttMonCtrlAdminStatus) =
		  NOT_IN_SERVICE;
	}

        return_code = set_row_status(
                       value->sl_value,
                       -1,
                       &tmp_doList_state,
                       &(((rttMonCtrlAdminEntry_t *) 
                                  (dp->data))->rttMonCtrlAdminStatus));
      }
      else
      {
	if ((((rttMonCtrlAdminEntry_t
	      *)(dp->data))->rttMonCtrlAdminStatus) ==
	    NOT_IN_SERVICE_AND_CHANGABLE) {
	    (((rttMonCtrlAdminEntry_t *) 
	      (dp->data))->rttMonCtrlAdminStatus) =
		  NOT_IN_SERVICE;
	}
        return_code = set_row_status(
                       value->sl_value,
                       1,
                       &tmp_doList_state,
                       &(((rttMonCtrlAdminEntry_t *) 
                                  (dp->data))->rttMonCtrlAdminStatus));
      }
      /* check Return Code */
      if (return_code != NO_ERROR)
      {
	   return (return_code);
      }
      if ((((rttMonCtrlAdminEntry_t *) 
              (dp->data))->rttMonCtrlAdminStatus != 
                 CREATE_AND_GO) &&
          (((rttMonCtrlAdminEntry_t *) 
              (dp->data))->rttMonCtrlAdminStatus != 
                 CREATE_AND_WAIT) &&
          (numVarBinds != 1))
      {
	   return (INCONSISTENT_VALUE_ERROR);
      }
     break;
#endif /* I_rttMonCtrlAdminStatus */

#ifdef I_rttMonScheduleAdminRttLife
   case I_rttMonScheduleAdminRttLife:
	if ((value->sl_value < MIN_rttMonScheduleAdminRttLife) ||
	    (value->sl_value > MAX_rttMonScheduleAdminRttLife))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonScheduleAdminRttLife */

#ifdef I_rttMonScheduleAdminRttStartTime
   case I_rttMonScheduleAdminRttStartTime:
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
       if (value->ul_value > RTT_MAX_DELAY_CENTISECONDS) {
	   return (INCONSISTENT_VALUE_ERROR);	   
       }
     break;
#endif /* I_rttMonScheduleAdminRttStartTime */

#ifdef I_rttMonScheduleAdminConceptRowAgeout
   case I_rttMonScheduleAdminConceptRowAgeout:
       if ((value->sl_value < MIN_rttMonScheduleAdminConceptRowAgeout) ||
	   (value->sl_value > MAX_rttMonScheduleAdminConceptRowAgeout))
       {
	 return (WRONG_VALUE_ERROR);
       }
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonScheduleAdminConceptRowAgeout */

#ifdef I_rttMonReactAdminConnectionEnable
   case I_rttMonReactAdminConnectionEnable:
     break;
#endif /* I_rttMonReactAdminConnectionEnable */

#ifdef I_rttMonReactAdminTimeoutEnable
   case I_rttMonReactAdminTimeoutEnable:
     break;
#endif /* I_rttMonReactAdminTimeoutEnable */

#ifdef I_rttMonReactAdminThresholdType
   case I_rttMonReactAdminThresholdType:
     break;
#endif /* I_rttMonReactAdminThresholdType */

#ifdef I_rttMonReactAdminThresholdFalling
   case I_rttMonReactAdminThresholdFalling:
	if ((value->sl_value < MIN_rttMonReactAdminThresholdFalling) ||
	    (value->sl_value > MAX_rttMonReactAdminThresholdFalling))
        {
	   return (WRONG_VALUE_ERROR);
	}
     break;
#endif /* I_rttMonReactAdminThresholdFalling */

#ifdef I_rttMonReactAdminThresholdCount
   case I_rttMonReactAdminThresholdCount:
	if ((value->sl_value < MIN_rttMonReactAdminThresholdCount) ||
	    (value->sl_value > MAX_rttMonReactAdminThresholdCount))
        {
	   return (WRONG_VALUE_ERROR);
	}
     break;
#endif /* I_rttMonReactAdminThresholdCount */

#ifdef I_rttMonReactAdminThresholdCount2
   case I_rttMonReactAdminThresholdCount2:
	if ((value->sl_value < MIN_rttMonReactAdminThresholdCount2) ||
	    (value->sl_value > MAX_rttMonReactAdminThresholdCount2))
        {
	   return (WRONG_VALUE_ERROR);
	}
     break;
#endif /* I_rttMonReactAdminThresholdCount2 */

#ifdef I_rttMonReactAdminActionType
   case I_rttMonReactAdminActionType:
     break;
#endif /* I_rttMonReactAdminActionType */

#ifdef I_rttMonStatisticsAdminNumHourGroups
   case I_rttMonStatisticsAdminNumHourGroups:
	if ((value->sl_value < MIN_rttMonStatisticsAdminNumHourGroups) ||
	    (value->sl_value > SYS_MAX_rttMonStatisticsAdminNumHourGroups))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonStatisticsAdminNumHourGroups */

#ifdef I_rttMonStatisticsAdminNumPaths
   case I_rttMonStatisticsAdminNumPaths:
	if ((value->sl_value < MIN_rttMonStatisticsAdminNumPaths) ||
	    (value->sl_value > SYS_MAX_rttMonStatisticsAdminNumPaths))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonStatisticsAdminNumPaths */

#ifdef I_rttMonStatisticsAdminNumHops
   case I_rttMonStatisticsAdminNumHops:
	if ((value->sl_value < MIN_rttMonStatisticsAdminNumHops) ||
	    (value->sl_value > SYS_MAX_rttMonStatisticsAdminNumHops))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonStatisticsAdminNumHops */

#ifdef I_rttMonStatisticsAdminNumDistBuckets
   case I_rttMonStatisticsAdminNumDistBuckets:
	if ((value->sl_value < MIN_rttMonStatisticsAdminNumDistBuckets) ||
	    (value->sl_value > SYS_MAX_rttMonStatisticsAdminNumDistBuckets))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonStatisticsAdminNumDistBuckets */

#ifdef I_rttMonStatisticsAdminDistInterval
   case I_rttMonStatisticsAdminDistInterval:
	if ((value->sl_value < MIN_rttMonStatisticsAdminDistInterval) ||
	    (value->sl_value > MAX_rttMonStatisticsAdminDistInterval))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonStatisticsAdminDistInterval */

#ifdef I_rttMonHistoryAdminNumLives
   case I_rttMonHistoryAdminNumLives:
	if ((value->sl_value < MIN_rttMonHistoryAdminNumLives) ||
	    (value->sl_value > SYS_MAX_rttMonHistoryAdminNumLives))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonHistoryAdminNumLives */

#ifdef I_rttMonHistoryAdminNumBuckets
   case I_rttMonHistoryAdminNumBuckets:
	if ((value->sl_value < MIN_rttMonHistoryAdminNumBuckets) ||
	    (value->sl_value > SYS_MAX_rttMonHistoryAdminNumBuckets))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonHistoryAdminNumBuckets */

#ifdef I_rttMonHistoryAdminNumSamples
   case I_rttMonHistoryAdminNumSamples:
	if ((value->sl_value < MIN_rttMonHistoryAdminNumSamples) ||
	    (value->sl_value > SYS_MAX_rttMonHistoryAdminNumSamples))
        {
	   return (WRONG_VALUE_ERROR);
	}
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonHistoryAdminNumSamples */

#ifdef I_rttMonHistoryAdminFilter
   case I_rttMonHistoryAdminFilter:
       if ((((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonHistoryAdminFilter */

#ifdef I_rttMonCtrlOperModificationTime
   case I_rttMonCtrlOperModificationTime:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperModificationTime */

#ifdef I_rttMonCtrlOperDiagText
   case I_rttMonCtrlOperDiagText:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperDiagText */

#ifdef I_rttMonCtrlOperResetTime
   case I_rttMonCtrlOperResetTime:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperResetTime */

#ifdef I_rttMonCtrlOperOctetsInUse
   case I_rttMonCtrlOperOctetsInUse:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperOctetsInUse */

#ifdef I_rttMonCtrlOperConnectionLostOccurred
   case I_rttMonCtrlOperConnectionLostOccurred:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperConnectionLostOccurred */

#ifdef I_rttMonCtrlOperTimeoutOccurred
   case I_rttMonCtrlOperTimeoutOccurred:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperTimeoutOccurred */

#ifdef I_rttMonCtrlOperOverThresholdOccurred
   case I_rttMonCtrlOperOverThresholdOccurred:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperOverThresholdOccurred */

#ifdef I_rttMonCtrlOperNumRtts
   case I_rttMonCtrlOperNumRtts:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperNumRtts */

#ifdef I_rttMonCtrlOperRttLife
   case I_rttMonCtrlOperRttLife:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonCtrlOperRttLife */

#ifdef I_rttMonCtrlOperState
   case I_rttMonCtrlOperState:
     if ((value->sl_value != D_rttMonCtrlOperState_reset) &&
	 (value->sl_value != D_rttMonCtrlOperState_orderlyStop) &&
         (value->sl_value != D_rttMonCtrlOperState_immediateStop)) {
	   return (WRONG_VALUE_ERROR);
     }
     if (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlAdminStatus !=
	            ACTIVE)
     {
	   return (INCONSISTENT_VALUE_ERROR);
     }
     if ((value->sl_value == D_rttMonCtrlOperState_reset) &&
	 (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperState !=
	  D_rttMonCtrlOperState_active) &&
	  (((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperState !=
	   D_rttMonCtrlOperState_pending) )
     {
	   return (INCONSISTENT_VALUE_ERROR);
     }
     if (numVarBinds != 1)
     {
	   return (INCONSISTENT_VALUE_ERROR);
     }
     /* 
      *  Transition the value here as apposed to the 
      *    rttMonCtrlAdminEntry_test function.
      */
     ((rttMonCtrlAdminEntry_t *) (dp->data))->rttMonCtrlOperState = value->sl_value;
     break;
#endif /* I_rttMonCtrlOperState */

#ifdef I_rttMonLatestRttOperCompletionTime
   case I_rttMonLatestRttOperCompletionTime:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonLatestRttOperCompletionTime */

#ifdef I_rttMonLatestRttOperSense
   case I_rttMonLatestRttOperSense:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonLatestRttOperSense */

#ifdef I_rttMonLatestRttOperApplSpecificSense
   case I_rttMonLatestRttOperApplSpecificSense:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonLatestRttOperApplSpecificSense */

#ifdef I_rttMonLatestRttOperSenseDescription
   case I_rttMonLatestRttOperSenseDescription:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonLatestRttOperSenseDescription */

#ifdef I_rttMonLatestRttOperTime
   case I_rttMonLatestRttOperTime:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonLatestRttOperTime */

#ifdef I_rttMonLatestRttOperAddress
   case I_rttMonLatestRttOperAddress:
	   return (NOT_WRITABLE_ERROR);
     break;
#endif /* I_rttMonLatestRttOperAddress */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in k_rttMonCtrlAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

    return (NO_ERROR);
}


static boolean
validThresholdValues (void)
{
  if ((currRttMonCtrlAdminEntryPtr->
        rttMonReactAdminThresholdType ==
       D_rttMonReactAdminThresholdType_xOfy) &&
      (currRttMonCtrlAdminEntryPtr->
        rttMonReactAdminThresholdCount >
       currRttMonCtrlAdminEntryPtr->
        rttMonReactAdminThresholdCount2))
  {
    return (FALSE);
  }
  else
  {
    return (TRUE);
  }

}

static boolean
allRequiredValuesExist (void)
{
  if ((currRttMonCtrlAdminEntryPtr != NULL) &&
      (currRttMonEchoAdminEntryPtr != NULL) &&
      (currRttMonEchoAdminEntryPtr->
          rttMonEchoAdminProtocol != 
            DEF_rttMonEchoAdminProtocol) &&
      (currRttMonEchoAdminEntryPtr->
          rttMonEchoAdminTargetAddress != NULL) &&
      (currRttMonEchoAdminEntryPtr->
          rttMonEchoAdminTargetAddress->length != 0) &&
      validThresholdValues())
  {
    return (TRUE);
  }

  return (FALSE);
}

static int
doMultiTableCreateQItem (doList_t *dp)
{
  SR_INT32 tmpRttMonCtrlAdminIndex;
  int  return_code;

  if (dp->setMethod == rttMonCtrlAdminEntry_set)
  {
    tmpRttMonCtrlAdminIndex =
       ((rttMonCtrlAdminEntry_t *) (dp->data))->
           rttMonCtrlAdminIndex;
  }
  else if (dp->setMethod == 
             rttMonEchoAdminEntry_set)
  {
    tmpRttMonCtrlAdminIndex =
       ((rttMonEchoAdminEntry_t *) (dp->data))->
           rttMonCtrlAdminIndex;
  }
  else
  {
    DPRINTF((APALWAYS, "snmpd: Internal error (in doMultiTableCreateQItem)\n"));
    return (GEN_ERROR);
  }

  if (getRttMonCtrlAdminQItem(
          tmpRttMonCtrlAdminIndex) != NULL)
  {
      return (NO_ERROR);
  }
  /*
   * create AdminQItem
   */
  if (dp->setMethod == rttMonCtrlAdminEntry_set)
  {  
    rttMonEchoAdminEntryBuff->
      rttMonCtrlAdminIndex =
	((rttMonCtrlAdminEntry_t *) (dp->data))->
	  rttMonCtrlAdminIndex;
    /*
     *  Set the defaults for the Echo portion of
     *   this newly created row.
     */
    setRttMonEchoAdminEntryDefaults(
	     rttMonEchoAdminEntryBuff);
    /*  Use a static buffer here */
    return_code = newRttMonCtrlAdminQItem(
       ((rttMonCtrlAdminEntry_t *) (dp->data)),
       (void *)rttMonEchoAdminEntryBuff);
    if (return_code != D_Return_Successfull)
    {
      return(RESOURCE_UNAVAILABLE_ERROR);
    }
  }
  else
  {
    rttMonCtrlAdminEntryBuff->
      rttMonCtrlAdminIndex =
	((rttMonEchoAdminEntry_t *) (dp->data))->
	  rttMonCtrlAdminIndex;
    /*
     *  Set the defaults for the Ctrl portion of
     *   this newly created row.
     */
    setRttMonCtrlAdminEntryDefaults(
             rttMonCtrlAdminEntryBuff);
    /* Use a static buffer here */
    return_code = newRttMonCtrlAdminQItem(
       rttMonCtrlAdminEntryBuff,
       (void *)dp->data);
    if (return_code != D_Return_Successfull)
    {
      return(RESOURCE_UNAVAILABLE_ERROR);
    }
  }

  return (NO_ERROR);
}

static int
updateEntries (void)
{
  int return_code;
  rttMonCtrlAdminEntry_t *tmpRttMonCtrlAdminEntry = NULL;

  return_code =
    updateRttMonCtrlAdminEntry(
             currRttMonCtrlAdminEntryPtr);
  if (return_code != D_Return_Successfull)
  {
    return (GEN_ERROR);
  }

  return_code =
    updateRttMonEchoAdminEntry(
             currRttMonCtrlAdminEntryPtr->
                rttMonCtrlAdminIndex,
             currRttMonEchoAdminEntryPtr);
  if (return_code != D_Return_Successfull)
  {
    return (GEN_ERROR);
  }

  tmpRttMonCtrlAdminEntry =
        k_rttMonCtrlAdminEntry_get(
              -2,
              NULL,
              -2,
              EXACT,
              currRttMonCtrlAdminEntryPtr->
                  rttMonCtrlAdminIndex);

  if (tmpRttMonCtrlAdminEntry == NULL)
  {
    return (GEN_ERROR);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonScheduleAdminRttLife !=
      currRttMonCtrlAdminEntryPtr->rttMonScheduleAdminRttLife)
  {
    setRttMonScheduleAdminRttLife(currRttMonCtrlAdminEntryPtr->
				  rttMonCtrlAdminIndex,
				  currRttMonCtrlAdminEntryPtr->
				  rttMonScheduleAdminRttLife);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonScheduleAdminRttStartTime !=
      currRttMonCtrlAdminEntryPtr->rttMonScheduleAdminRttStartTime)
  {
    setRttMonScheduleAdminRttStartTime(currRttMonCtrlAdminEntryPtr->
				       rttMonCtrlAdminIndex,
				       currRttMonCtrlAdminEntryPtr->
				       rttMonScheduleAdminRttStartTime);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonScheduleAdminConceptRowAgeout !=
      currRttMonCtrlAdminEntryPtr->rttMonScheduleAdminConceptRowAgeout)
  {
    setRttMonScheduleAdminConceptRowAgeout(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonScheduleAdminConceptRowAgeout);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonReactAdminConnectionEnable !=
      currRttMonCtrlAdminEntryPtr->rttMonReactAdminConnectionEnable)
  {
    setRttMonReactAdminConnectionEnable(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonReactAdminConnectionEnable);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonReactAdminTimeoutEnable !=
      currRttMonCtrlAdminEntryPtr->rttMonReactAdminTimeoutEnable)
  {
    setRttMonReactAdminTimeoutEnable(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonReactAdminTimeoutEnable);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonReactAdminThresholdFalling !=
      currRttMonCtrlAdminEntryPtr->rttMonReactAdminThresholdFalling)
  {
    setRttMonReactAdminThresholdFalling(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonReactAdminThresholdFalling);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonReactAdminThresholdCount2 !=
      currRttMonCtrlAdminEntryPtr->rttMonReactAdminThresholdCount2)
  {
    setRttMonReactAdminThresholdCount2(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonReactAdminThresholdCount2);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonReactAdminThresholdCount !=
      currRttMonCtrlAdminEntryPtr->rttMonReactAdminThresholdCount)
  {
    setRttMonReactAdminThresholdCount(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonReactAdminThresholdCount);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonReactAdminThresholdType !=
      currRttMonCtrlAdminEntryPtr->rttMonReactAdminThresholdType)
  {
    setRttMonReactAdminThresholdType(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonReactAdminThresholdType);
  }

  if (tmpRttMonCtrlAdminEntry->rttMonReactAdminActionType !=
      currRttMonCtrlAdminEntryPtr->rttMonReactAdminActionType)
  {
    setRttMonReactAdminActionTypes(currRttMonCtrlAdminEntryPtr->
				   rttMonCtrlAdminIndex,
				   currRttMonCtrlAdminEntryPtr->
				   rttMonReactAdminActionType);
  }

  return (NO_ERROR);
}

static void
set_all_conceptual_tabs_dp_state_ADD_MODIFY (
    doList_t       *doHead)
{
  doList_t *tmpDoPtr;

  for (tmpDoPtr = doHead; tmpDoPtr != NULL;
       tmpDoPtr = tmpDoPtr->next)
  {
    if ((tmpDoPtr->setMethod ==
	 rttMonCtrlAdminEntry_set) ||
	(tmpDoPtr->setMethod ==
	 rttMonEchoAdminEntry_set))
      tmpDoPtr->state = ADD_MODIFY;
  }
}

int
k_rttMonCtrlAdminEntry_ready (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{
  boolean                 good_row;
  int                     return_code;
  int                     row_status;
  boolean                 secondaryMemoryForTables;

  row_status = currRttMonCtrlAdminEntryPtr->
                      rttMonCtrlAdminStatus;
  /*
   *  Check for Destroy here so a row is
   *    not created.
   */
  if ((newRttMonAdminEntryRow == TRUE) &&
      (row_status == DESTROY))
  {
    dp->state = DELETE;
    return (NO_ERROR);
  }

  good_row = allRequiredValuesExist();

  return_code = doMultiTableCreateQItem(dp);
  if (return_code != NO_ERROR)
  {
    return (return_code);
  }

  /* 
   *  When we have a new row, it was just
   *    created via the previous call, so
   *    we nee to setup the global pointers
   *    to both tables.
   */
  if (newRttMonAdminEntryRow == TRUE)
  {
    setRttMonCtrlAdminEntryPtr(((rttMonCtrlAdminEntry_t *) (dp->data)));
  }

  if (object->nominator == I_rttMonCtrlAdminRttType)
  {
    /* 
     *  Allow changing the type when the protocol is 
     *    D_rttMonEchoAdminProtocol_notApplicable or
     *    changing to something other than echo, 
     *    don't check for support.  (the above call
     *    will check to see if things other than echo
     *    are supported.
     */
    if ((currRttMonEchoAdminEntryPtr->
	 rttMonEchoAdminProtocol != 
	 D_rttMonEchoAdminProtocol_notApplicable) &&
	((value->sl_value != D_rttMonCtrlAdminRttType_echo) ||
	 (value->sl_value != D_rttMonCtrlAdminRttType_pathEcho)))
    {
      if (reg_invoke_rtt_echo_exists(currRttMonEchoAdminEntryPtr->
				     rttMonEchoAdminProtocol))
      {
	return_code = reg_invoke_rtt_echo_validate(
				   currRttMonEchoAdminEntryPtr->
				   rttMonEchoAdminProtocol,
				   currRttMonCtrlAdminEntryPtr,
				   (void *)currRttMonEchoAdminEntryPtr,
				   D_Check_RttType);
	/* check return code */
	if (return_code != D_VALID_RttEntry)
	{
	  return (INCONSISTENT_VALUE_ERROR);
	}
      }
      else
      {
	return (INCONSISTENT_VALUE_ERROR);
      }
    }
  }

  if (row_status == CREATE_AND_GO)
  {
    if (good_row)
    {
      /*
       * With Create And Go we have a problem of determining
       *  when all VarBinds have been processed.  We need to
       *  update entries and allocate memory when we have 
       *  finished with the VarBinds and the row is valid (ie
       *  ready to transition to active.  We are forced to
       *  do the row update and malloc in the set!!  I don't 
       *  like this but...
       */
      set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
      /* real activation done is set */
    }
    else
    {
      /*  Create and go must have a good 
       *   row on one PDU.
       *  Noop, same as doing nothing, ie
       *   leaving the dp->state as UNKNOWN.  
       */
      dp->state = UNKNOWN;
    }
  }
  else if (row_status == CREATE_AND_WAIT)
  {
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else if (row_status == NOT_IN_SERVICE)
  { /* was transitioned by Mgr */
    if (good_row)
    {
      set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
    }
    else
    { /* This is done by all cisco mibs??  It prevents
       *   transitioning when in a bad state.
       *     Even though this is the same as doing nothing.
       */
      dp->state = UNKNOWN;
    }
  }
  else if (row_status == NOT_IN_SERVICE_AND_CHANGABLE)
  { /* was transitioned by agent */
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else if (row_status == NOT_READY)
  {
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else if (row_status == DESTROY)
  {
    dp->state = DELETE;
    /* delete actually done in set */
  }
  else if (row_status == ACTIVE)
  {
    return_code = 
       rttMonCtrlAdminQItemSecondaryMemory(
           currRttMonCtrlAdminEntryPtr->
                 rttMonCtrlAdminIndex,
           &secondaryMemoryForTables);
    if (return_code != D_Return_Successfull)
    {
      return (RESOURCE_UNAVAILABLE_ERROR);
    }
    if (!secondaryMemoryForTables)
    {
      /* update entries here, so when we
       *  create the secondary memory, we
       *  have valid sizes, which will not
       *  be changable after the creation
       */
      return_code = updateEntries();
      if (return_code != NO_ERROR)
      {
        return (return_code);
      }
      return_code = 
        createRttMonCtrlAdminQItemSecondaryMemory(
          currRttMonCtrlAdminEntryPtr->
                 rttMonCtrlAdminIndex);
      if (return_code != D_Return_Successfull)
      {
        return (RESOURCE_UNAVAILABLE_ERROR);
      }
    }
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else
  {
    DPRINTF((APALWAYS, "snmpd: Internal error (invalid rowStatus in k_rttMonCtrlAdminEntry_ready)\n"));
    return (INCONSISTENT_VALUE_ERROR);
  }

  return (NO_ERROR);
}

int
k_rttMonCtrlAdminEntry_set_defaults (
    doList_t       *dp)
{
    rttMonCtrlAdminEntry_t *data = (rttMonCtrlAdminEntry_t *) (dp->data);

    setRttMonCtrlAdminEntryDefaults(data);

    newRttMonAdminEntryRow = TRUE;

    return (NO_ERROR);
}

int
k_rttMonCtrlAdminEntry_set (
   rttMonCtrlAdminEntry_t *data,
   ContextInfo *contextInfo,
   int function)
{
  boolean                 good_row;
  int                     row_status;
  int                     return_code;
  rttMonCtrlAdminEntry_t *tmpRttMonCtrlAdminEntry = NULL;

  row_status = currRttMonCtrlAdminEntryPtr->
                  rttMonCtrlAdminStatus;
  /*
   *  If a request to DESTROY has been sent, it must be
   *   a singly bound PDU.  So just delete the entry
   *   before changing any state and return.
   */
  if (row_status == DESTROY)
  {
      setRttMonCtrlAdminStatus(currRttMonCtrlAdminEntryPtr->
			       rttMonCtrlAdminIndex,
			       row_status);
      return (NO_ERROR);
  }

  good_row = allRequiredValuesExist();

  numVarBinds = 0;

  return_code = updateEntries();
  if (return_code != NO_ERROR)
  {
    return (return_code);
  }
  /* 
   *  Do the End of validate transtions.  These will almost 
   *   all not fail.
   */
  if (row_status == CREATE_AND_GO)
  {
    /*
     * With Create And Go we have a problem of determining
     *  when all VarBinds have been processed.  We need to
     *  update entries and allocate memory when we have 
     *  finished with the VarBinds and the row is valid (ie
     *  ready to transition to active.  We are forced to
     *  do the row update and malloc in the set!!  I don't 
     *  like this but...
     * One good thing, no need for an undo!!!
     */
    return_code = 
      createRttMonCtrlAdminQItemSecondaryMemory(
        currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminIndex);
    if (return_code != D_Return_Successfull)
    {
      return (RESOURCE_UNAVAILABLE_ERROR);
    }

    currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus = ACTIVE;
  }
  else if (row_status == CREATE_AND_WAIT)
  {
    if (good_row)
    {
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus =
                              NOT_IN_SERVICE_AND_CHANGABLE;
    }
    else
    {
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus =
                              NOT_READY;
    }
  }
  else if (row_status == NOT_IN_SERVICE_AND_CHANGABLE)
  { /* was transitioned by agent */
    if (!good_row)
    {  /*  We allow the transitioning back and forth
        *   between here and NOT_READY when the 
        *   CREATE_AND_WAIT steps are in place
        *  This is ambiguous in the RowStatus TC, where
        *    the state machine has no way to transition 
        *    to NOT_READY on creation, ater being in
        *    NOT_IN_SERVICE, but when all 
        *    objects default to valid value and they all
        *    cannot be modified via 1 PDU, it must be 
        *    valid to change the defaults, and thus 
        *    causing an intermediate NOT_READY state.
        */
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus =
                              NOT_READY;
    }
  }
  else if (row_status == NOT_READY)
  {
    if (good_row)
    {
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus = 
                   NOT_IN_SERVICE_AND_CHANGABLE;
    }
  }

  /* 
   *  This has to exist to get to here.
   */
  tmpRttMonCtrlAdminEntry =
        k_rttMonCtrlAdminEntry_get(
              -2,
              NULL,
              -2,
              EXACT,
              currRttMonCtrlAdminEntryPtr->
                  rttMonCtrlAdminIndex);

  /*
   *  OperState and AdminStatus are required to be 
   *   singly bound.
   */
  if (tmpRttMonCtrlAdminEntry->rttMonCtrlOperState !=
      currRttMonCtrlAdminEntryPtr->rttMonCtrlOperState)
  {
    setRttMonCtrlOperState(
          currRttMonCtrlAdminEntryPtr->
               rttMonCtrlAdminIndex,
          currRttMonCtrlAdminEntryPtr->
               rttMonCtrlOperState);
  }
  else if (tmpRttMonCtrlAdminEntry->rttMonCtrlAdminStatus !=
	   currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus)
  {
    /* This can be called for any valid state change */
    setRttMonCtrlAdminStatus(
           currRttMonCtrlAdminEntryPtr->
               rttMonCtrlAdminIndex,
           currRttMonCtrlAdminEntryPtr->
               rttMonCtrlAdminStatus);
  }

    return (NO_ERROR);
}
#endif /* SETS */

#ifdef SR_rttMonCtrlAdminEntry_UNDO
/* add #define SR_rttMonCtrlAdminEntry_UNDO in rttmib.h to
 * include the undo routine for the rttMonCtrlAdminEntry family.
 */
int
rttMonCtrlAdminEntry_undo (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return (UNDO_FAILED_ERROR);
}
#endif /* SR_rttMonCtrlAdminEntry_UNDO */

rttMonEchoAdminEntry_t *
k_rttMonEchoAdminEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex)
{

   int         return_code;

   if (searchType == EXACT)
   {
     return_code = getRttMonEchoAdminEntry(
                     rttMonCtrlAdminIndex,
                     rttMonEchoAdminEntryData);
   }
   else
   {
     return_code = getNextRttMonEchoAdminEntry(
                     rttMonCtrlAdminIndex,
                     rttMonEchoAdminEntryData);
   }

   if (return_code != D_Return_Successfull)
   {
     return(NULL);
   }

   return(rttMonEchoAdminEntryData);

}

#ifdef SETS
int
k_rttMonEchoAdminEntry_test (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
    rttMonAppl_t  *tmpRttMonAppl;

    setRttMonEchoAdminEntryPtr(((rttMonEchoAdminEntry_t *) (dp->data)));

    numVarBinds++;

    /*
     *  Check the index values
     */
    if ((((rttMonEchoAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex <
	MIN_rttMonCtrlAdminIdx) ||
	(((rttMonEchoAdminEntry_t *) (dp->data))->rttMonCtrlAdminIndex >
	MAX_rttMonCtrlAdminIdx))
    {
	return (GEN_ERROR);
    }
  
    /*
     *  If this is a new row being created, check to see if this new
     *    row will put us over the maximum number of rows that we 
     *    support.
     *  This function is call numerous times, one for each varbind,
     *    thus when this is a new row being created, only check on
     *    on the first varbind.  After the first varbind a placeholder
     *    is created and the number of rows will be equal to the 
     *    max, but in this situation this is ok.
     */
    if (newRttMonAdminEntryRow && 
	(numVarBinds == 1) &&
	(getRttMonCtrlAdminQSize() >=
	 SYS_MAX_rttMonApplNumCtrlAdminEntry)) {
	return (GEN_ERROR);
    }

    /* 
     *  Psuedo-random number as a new index, must be unique
     */
    if ((!newRttMonAdminEntryRow) && 
        ((currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus ==
                    CREATE_AND_GO) ||
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus ==
	            CREATE_AND_WAIT)))
    {
      return (INCONSISTENT_VALUE_ERROR);
    }

    tmpRttMonAppl = getRttMonApplPtr();

    switch (object->nominator) {

#ifdef I_rttMonEchoAdminProtocol
   case I_rttMonEchoAdminProtocol:
       if ((currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }

     break;
#endif /* I_rttMonEchoAdminProtocol */

#ifdef I_rttMonEchoAdminTargetAddress
   case I_rttMonEchoAdminTargetAddress:
       if ((value->os_value->length < MIN_rttMonTargetAddress) ||
	   (value->os_value->length > MAX_rttMonTargetAddress))
       {
	   return (WRONG_LENGTH_ERROR);
       }
       if ((currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }

     break;
#endif /* I_rttMonEchoAdminTargetAddress */

#ifdef I_rttMonEchoAdminPktDataRequestSize
   case I_rttMonEchoAdminPktDataRequestSize:
       if ((value->sl_value < MIN_rttMonEchoAdminPktDataRequestSize) ||
           (value->sl_value > MAX_rttMonEchoAdminPktDataRequestSize) ||
           (value->sl_value + ARR_HEADER_SIZE > 
             tmpRttMonAppl->rttMonApplMaxPacketDataSize))
       {
           return (WRONG_VALUE_ERROR);
       }
       if ((currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonEchoAdminPktDataRequestSize */

#ifdef I_rttMonEchoAdminPktDataResponseSize
   case I_rttMonEchoAdminPktDataResponseSize:
       if ((value->sl_value < MIN_rttMonEchoAdminPktDataResponseSize) ||
           (value->sl_value > MAX_rttMonEchoAdminPktDataResponseSize) ||
           (value->sl_value + ARR_HEADER_SIZE > 
             tmpRttMonAppl->rttMonApplMaxPacketDataSize))
       {
           return (WRONG_VALUE_ERROR);
       }
       if ((currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_IN_SERVICE_AND_CHANGABLE) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            NOT_READY) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_GO) &&
           (currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus !=
	            CREATE_AND_WAIT))
       {
	   return (INCONSISTENT_VALUE_ERROR);
       }
     break;
#endif /* I_rttMonEchoAdminPktDataResponseSize */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in k_rttMonEchoAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */

    return (NO_ERROR);
}

int
k_rttMonEchoAdminEntry_ready (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{
  boolean                 good_row;
  int                     return_code;
  int                     row_status;
  boolean                 secondaryMemoryForTables;

  good_row = allRequiredValuesExist();

  return_code = doMultiTableCreateQItem(dp);
  if (return_code != NO_ERROR)
  {
    return (return_code);
  }
  /* 
   *  When we have a new row, it was just
   *    created via the previous call, so
   *    we nee to setup the global pointers
   *    to both tables.
   */
  if (newRttMonAdminEntryRow == TRUE)
  {
    setRttMonEchoAdminEntryPtr(((rttMonEchoAdminEntry_t *) (dp->data)));
  }

  switch (object->nominator)
  {
  case I_rttMonEchoAdminProtocol:
    if (reg_invoke_rtt_echo_exists(currRttMonEchoAdminEntryPtr->
				   rttMonEchoAdminProtocol))
    {
      return_code = reg_invoke_rtt_echo_validate(
				 currRttMonEchoAdminEntryPtr->
				 rttMonEchoAdminProtocol,
				 currRttMonCtrlAdminEntryPtr,
				 (void *)currRttMonEchoAdminEntryPtr,
				 D_Check_Protocol);
      /* check return code */
      if (return_code != D_VALID_RttEntry)
      {
	return (INCONSISTENT_VALUE_ERROR);
      }
    }
    else
    {
      return (INCONSISTENT_VALUE_ERROR);
    }
    break;
  case I_rttMonEchoAdminTargetAddress:
    if (reg_invoke_rtt_echo_exists(currRttMonEchoAdminEntryPtr->
				   rttMonEchoAdminProtocol))
    {
      return_code = reg_invoke_rtt_echo_validate(
				 currRttMonEchoAdminEntryPtr->
				 rttMonEchoAdminProtocol,
				 currRttMonCtrlAdminEntryPtr,
				 (void *)currRttMonEchoAdminEntryPtr,
				 D_Check_Address);
      /* check return code */
      if (return_code != D_VALID_RttEntry)
      {
	return (INCONSISTENT_VALUE_ERROR);
      }
    }
    else
    {
      return (INCONSISTENT_VALUE_ERROR);
    }
    break;
  }

  /* 
   * Call this on every pass, it is used to 
   *  either validate or update the Response Size.
   */
  if (reg_invoke_rtt_echo_exists(currRttMonEchoAdminEntryPtr->
				 rttMonEchoAdminProtocol))
  {
    return_code = reg_invoke_rtt_echo_validate(
			       currRttMonEchoAdminEntryPtr->
			       rttMonEchoAdminProtocol,
			       currRttMonCtrlAdminEntryPtr,
			       (void *)currRttMonEchoAdminEntryPtr,
			       D_Check_Response_Size);
    /* check return code */
    if (return_code != D_VALID_RttEntry)
    {
      return (INCONSISTENT_VALUE_ERROR);
    }
  }
  else
  {
    return (INCONSISTENT_VALUE_ERROR);
  }

  row_status = currRttMonCtrlAdminEntryPtr->
                      rttMonCtrlAdminStatus;
  if (row_status == CREATE_AND_GO)
  {
    if (good_row)
    {
      /*
       * With Create And Go we have a problem of determining
       *  when all VarBinds have been processed.  We need to
       *  update entries and allocate memory when we have 
       *  finished with the VarBinds and the row is valid (ie
       *  ready to transition to active.  We are forced to
       *  do the row update and malloc in the set!!  I don't 
       *  like this but...
       */
      set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
      /* real activation done is set */
    }
    else
    {
      /*  Create and go must have a good 
       *   row on one PDU.
       *  Noop, same as doing nothing, ie
       *   leaving the dp->state as UNKNOWN.  
       */
      dp->state = UNKNOWN;
    }
  }
  else if (row_status == CREATE_AND_WAIT)
  {
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else if (row_status == NOT_IN_SERVICE)
  { /* was transitioned by Mgr */
    if (good_row)
    {
      set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
    }
    else
    { /* This is done by all cisco mibs??  It prevents
       *   transitioning when in a bad state.
       *     Even though this is the same as doing nothing.
       */
      dp->state = UNKNOWN;
    }
  }
  else if (row_status == NOT_IN_SERVICE_AND_CHANGABLE)
  { /* was transitioned by agent */
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else if (row_status == NOT_READY)
  {
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else if (row_status == DESTROY)
  {
    dp->state = DELETE;
    /* delete actually done in set */
  }
  else if (row_status == ACTIVE)
  {
    return_code = 
       rttMonCtrlAdminQItemSecondaryMemory(
           currRttMonCtrlAdminEntryPtr->
                 rttMonCtrlAdminIndex,
           &secondaryMemoryForTables);
    if (return_code != D_Return_Successfull)
    {
      return (RESOURCE_UNAVAILABLE_ERROR);
    }
    if (!secondaryMemoryForTables)
    {
      /* update the entries now, so
       *  the proper secondary memory sizes
       *  will be used. 
       */
      return_code = updateEntries();
      if (return_code != NO_ERROR)
      {
        return (return_code);
      }
      return_code = 
        createRttMonCtrlAdminQItemSecondaryMemory(
          currRttMonCtrlAdminEntryPtr->
                 rttMonCtrlAdminIndex);
      if (return_code != D_Return_Successfull)
      {
        return (RESOURCE_UNAVAILABLE_ERROR);
      }
    }
    set_all_conceptual_tabs_dp_state_ADD_MODIFY(doHead);
  }
  else
  {
    DPRINTF((APALWAYS, "snmpd: Internal error (invalid rowStatus in k_rttMonEchoAdminEntry_ready)\n"));
    return (INCONSISTENT_VALUE_ERROR);
  }

  return (NO_ERROR);

}

int
k_rttMonEchoAdminEntry_set_defaults (
    doList_t       *dp)
{
    rttMonEchoAdminEntry_t *data = (rttMonEchoAdminEntry_t *) (dp->data);
    
    setRttMonEchoAdminEntryDefaults(data);

    newRttMonAdminEntryRow = TRUE;

    return (NO_ERROR);
}

int
k_rttMonEchoAdminEntry_set (
   rttMonEchoAdminEntry_t *data,
   ContextInfo *contextInfo,
   int function)
{
  boolean                 good_row;
  int                     row_status;
  int                     return_code;

  good_row = allRequiredValuesExist();

  numVarBinds = 0;
  row_status = currRttMonCtrlAdminEntryPtr->
                  rttMonCtrlAdminStatus;

  return_code = updateEntries();
  if (return_code != NO_ERROR)
  {
    return (return_code);
  }
  /* 
   *  Do the End of validate transtions.  These will almost 
   *   all not fail.
   */
  if (row_status == CREATE_AND_GO)
  {
    /*
     * With Create And Go we have a problem of determining
     *  when all VarBinds have been processed.  We need to
     *  update entries and allocate memory when we have 
     *  finished with the VarBinds and the row is valid (ie
     *  ready to transition to active.  We are forced to
     *  do the row update and malloc in the set!!  I don't 
     *  like this but...
     * One good thing, no need for an undo!!!
     */
    return_code = 
      createRttMonCtrlAdminQItemSecondaryMemory(
        currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminIndex);
    if (return_code != D_Return_Successfull)
    {
      return (RESOURCE_UNAVAILABLE_ERROR);
    }

    currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus = ACTIVE;
  }
  else if (row_status == CREATE_AND_WAIT)
  {
    if (good_row)
    {
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus =
                              NOT_IN_SERVICE_AND_CHANGABLE;
    }
    else
    {
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus =
                              NOT_READY;
    }
  }
  else if (row_status == NOT_IN_SERVICE_AND_CHANGABLE)
  { /* was transitioned by agent */
    if (!good_row)
    {  /*  We allow the transitioning back and forth
        *   between here and NOT_READY when the 
        *   CREATE_AND_WAIT steps are in place
        *  This is ambiguous in the RowStatus TC, where
        *    the state machine has no way to transition 
        *    to NOT_READY on creation, ater being in
        *    NOT_IN_SERVICE, but when all 
        *    objects default to valid value and they all
        *    cannot be modified via 1 PDU, it must be 
        *    valid to change the defaults, and thus 
        *    causing an intermediate NOT_READY state.
        */
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus =
                              NOT_READY;
    }
  }
  else if (row_status == NOT_READY)
  {
    if (good_row)
    {
      currRttMonCtrlAdminEntryPtr->rttMonCtrlAdminStatus = 
                   NOT_IN_SERVICE_AND_CHANGABLE;
    }
  }

  row_status = currRttMonCtrlAdminEntryPtr->
                       rttMonCtrlAdminStatus;

  setRttMonCtrlAdminStatus(
           currRttMonCtrlAdminEntryPtr->
               rttMonCtrlAdminIndex,
           row_status);

  return (NO_ERROR);

}
#endif /* SETS */

#ifdef SR_rttMonEchoAdminEntry_UNDO
/* add #define SR_rttMonEchoAdminEntry_UNDO in rttmib.h to
 * include the undo routine for the rttMonEchoAdminEntry family.
 */
int
rttMonEchoAdminEntry_undo (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return (UNDO_FAILED_ERROR);
}
#endif /* SR_rttMonEchoAdminEntry_UNDO */

rttMonFileIOAdminEntry_t *
k_rttMonFileIOAdminEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex)
{
   return(NULL);
}

#ifdef SETS
int
k_rttMonFileIOAdminEntry_test (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
    return (INCONSISTENT_VALUE_ERROR);
}

int
k_rttMonFileIOAdminEntry_ready (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{

    dp->state = ADD_MODIFY;
    return (NO_ERROR);
}

int
k_rttMonFileIOAdminEntry_set_defaults (
    doList_t       *dp)
{
    return (NO_ERROR);
}

int
k_rttMonFileIOAdminEntry_set (
   rttMonFileIOAdminEntry_t *data,
   ContextInfo *contextInfo,
   int function)
{
   return (GEN_ERROR);
}
#endif /* SETS */

#ifdef SR_rttMonFileIOAdminEntry_UNDO
/* add #define SR_rttMonFileIOAdminEntry_UNDO in rttmib.h to
 * include the undo routine for the rttMonFileIOAdminEntry family.
 */
int
rttMonFileIOAdminEntry_undo (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return (UNDO_FAILED_ERROR);
}
#endif /* SR_rttMonFileIOAdminEntry_UNDO */

rttMonScriptAdminEntry_t *
k_rttMonScriptAdminEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex)
{
   return(NULL);
}

#ifdef SETS
int
k_rttMonScriptAdminEntry_test (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
    return (INCONSISTENT_VALUE_ERROR);
}

int
k_rttMonScriptAdminEntry_ready (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{
    dp->state = ADD_MODIFY;
    return (NO_ERROR);
}

int
k_rttMonScriptAdminEntry_set_defaults (
    doList_t       *dp)
{
    return (NO_ERROR);
}

int
k_rttMonScriptAdminEntry_set (
   rttMonScriptAdminEntry_t *data,
   ContextInfo *contextInfo,
   int function)
{
   return (GEN_ERROR);
}
#endif /* SETS */

#ifdef SR_rttMonScriptAdminEntry_UNDO
/* add #define SR_rttMonScriptAdminEntry_UNDO in rttmib.h to
 * include the undo routine for the rttMonScriptAdminEntry family.
 */
int
rttMonScriptAdminEntry_undo (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return (UNDO_FAILED_ERROR);
}
#endif /* SR_rttMonScriptAdminEntry_UNDO */

rttMonReactTriggerAdminEntry_t *
k_rttMonReactTriggerAdminEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex,
   SR_INT32 rttMonReactTriggerAdminRttMonCtrlAdminIndex)
{
   static rttMonReactTriggerAdminEntry_t rttMonReactTriggerAdminEntryData;

   int         return_code;

   if (searchType == EXACT)
   {
     return_code = getRttMonReactTriggerAdminEntry(
                     rttMonCtrlAdminIndex,
                     rttMonReactTriggerAdminRttMonCtrlAdminIndex,
                     &rttMonReactTriggerAdminEntryData);
   }
   else
   {
     return_code = getNextRttMonReactTriggerAdminEntry(
                     rttMonCtrlAdminIndex,
                     rttMonReactTriggerAdminRttMonCtrlAdminIndex,
                     &rttMonReactTriggerAdminEntryData);
   }

   if (return_code != D_Return_Successfull)
   {
     return(NULL);
   }

   return(&rttMonReactTriggerAdminEntryData);

}

#ifdef SETS
int
k_rttMonReactTriggerAdminEntry_test (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
    int                    tmp_doList_state;
    int                    return_code;

    switch (object->nominator) {

#ifdef I_rttMonReactTriggerAdminStatus
   case I_rttMonReactTriggerAdminStatus:

      /* 
       * tmp_doList_state is not used.
       */
      if (newRttMonReactTriggerAdminEntryRow == TRUE)
      {
        return_code = set_row_status(
                       value->sl_value,
                       -1,
                       &tmp_doList_state,
                       &(((rttMonReactTriggerAdminEntry_t *) 
                                  (dp->data))->rttMonReactTriggerAdminStatus));
      }
      else
      {
        return_code = set_row_status(
                       value->sl_value,
                       1,
                       &tmp_doList_state,
                       &(((rttMonReactTriggerAdminEntry_t *) 
                                  (dp->data))->rttMonReactTriggerAdminStatus));
      }
      /* check Return Code */
      if (return_code != NO_ERROR)
      {
	   return (return_code);
      }
     break;

#endif /* I_rttMonReactTriggerAdminStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in k_rttMonReactTriggerAdminEntry_test)\n"));
       return (GEN_ERROR);

   }        /* switch */


    return (NO_ERROR);
}

int
k_rttMonReactTriggerAdminEntry_ready (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{
  int                     row_status;
  boolean                 good_row = TRUE;

  row_status =
    ((rttMonReactTriggerAdminEntry_t *) 
       (dp->data))->rttMonReactTriggerAdminStatus;
  if (row_status == CREATE_AND_GO)
  {
    if (good_row)
    {
      /*
       * With Create And Go we have a problem of determining
       *  when all VarBinds have been processed.  We need to
       *  update entries and allocate memory when we have 
       *  finished with the VarBinds and the row is valid (ie
       *  ready to transition to active.  We are forced to
       *  do the row update and malloc in the set!!  I don't 
       *  like this but...
       */
      dp->state = ADD_MODIFY;
      /* real activation done is set */
    }
    else
    {
      /*  Create and go must have a good 
       *   row on one PDU.
       *  Noop, same as doing nothing, ie
       *   leaving the dp->state as UNKNOWN.  
       */
      dp->state = UNKNOWN;
    }
  }
  else if (row_status == CREATE_AND_WAIT)
  {
    dp->state = ADD_MODIFY;
  }
  else if (row_status == NOT_IN_SERVICE)
  { /* was transitioned by Mgr */
    if (good_row)
    {
      dp->state = ADD_MODIFY;
    }
    else
    { /* This is done by all cisco mibs??  It prevents
       *   transitioning when in a bad state.
       *     Even though this is the same as doing nothing.
       */
      dp->state = UNKNOWN;
    }
  }
  else if (row_status == NOT_IN_SERVICE_AND_CHANGABLE)
  { /* was transitioned by agent */
    dp->state = ADD_MODIFY;
  }
  else if (row_status == NOT_READY)
  {
    dp->state = ADD_MODIFY;
  }
  else if (row_status == DESTROY)
  {
    dp->state = DELETE;
    /* delete actually done in set */
  }
  else if (row_status == ACTIVE)
  {
    /*  Check the return code */
    dp->state = ADD_MODIFY;
  }
  else
  {
    DPRINTF((APALWAYS, "snmpd: Internal error (invalid rowStatus in k_rttMonCtrlAdminEntry_ready)\n"));
    return (INCONSISTENT_VALUE_ERROR);
  }

    return (NO_ERROR);
}

int
k_rttMonReactTriggerAdminEntry_set_defaults (
    doList_t       *dp)
{
    newRttMonReactTriggerAdminEntryRow = TRUE;
    return (NO_ERROR);
}

int
k_rttMonReactTriggerAdminEntry_set (
   rttMonReactTriggerAdminEntry_t *data,
   ContextInfo *contextInfo,
   int function)
{
  int     row_status;
  boolean good_row;  

  good_row = TRUE;

  numVarBinds = 0;

  row_status = data->rttMonReactTriggerAdminStatus;
  /* 
   *  Do the End of validate transtions.  These will almost 
   *   all not fail.
   */
  if (row_status == CREATE_AND_GO)
  {
    data->rttMonReactTriggerAdminStatus = ACTIVE;
  }
  else if (row_status == CREATE_AND_WAIT)
  {
    if (good_row)
    {
      data->rttMonReactTriggerAdminStatus =
                              NOT_IN_SERVICE_AND_CHANGABLE;
    }
    else
    {
      data->rttMonReactTriggerAdminStatus =
                              NOT_READY;
    }
  }
  else if (row_status == NOT_IN_SERVICE_AND_CHANGABLE)
  { /* was transitioned by agent */
    if (!good_row)
    {  /*  We allow the transitioning back and forth
        *   between here and NOT_READY when the 
        *   CREATE_AND_WAIT steps are in place
        *  This is ambiguous in the RowStatus TC, where
        *    the state machine has no way to transition 
        *    to NOT_READY on creation, ater being in
        *    NOT_IN_SERVICE, but when all 
        *    objects default to valid value and they all
        *    cannot be modified via 1 PDU, it must be 
        *    valid to change the defaults, and thus 
        *    causing an intermediate NOT_READY state.
        */
      data->rttMonReactTriggerAdminStatus =
                              NOT_READY;
    }
  }
  else if (row_status == NOT_READY)
  {
    if (good_row)
    {
      data->rttMonReactTriggerAdminStatus = 
                   NOT_IN_SERVICE_AND_CHANGABLE;
    }
  }
  else
  {
    DPRINTF((APALWAYS, "snmpd: Internal error (invalid rowStatus in k_rttMonCtrlAdminEntry_set)\n"));
  }

  row_status = data->rttMonReactTriggerAdminStatus;

  addUpdateRttMonReactTriggerAdminEntry(
                data->rttMonCtrlAdminIndex,
                data);

  setRttMonReactTriggerAdminStatus(
                data->rttMonCtrlAdminIndex,
                data->rttMonReactTriggerAdminRttMonCtrlAdminIndex,
                data->rttMonReactTriggerAdminStatus);

  return (NO_ERROR);

}
#endif /* SETS */

#ifdef SR_rttMonReactTriggerAdminEntry_UNDO
/* add #define SR_rttMonReactTriggerAdminEntry_UNDO in rttmib.h to
 * include the undo routine for the rttMonReactTriggerAdminEntry family.
 */
int
rttMonReactTriggerAdminEntry_undo (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return (UNDO_FAILED_ERROR);
}
#endif /* SR_rttMonReactTriggerAdminEntry_UNDO */

rttMonStatsCaptureEntry_t *
k_rttMonStatsCaptureEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex,
   SR_UINT32 rttMonStatsCaptureStartTimeIndex,
   SR_INT32 rttMonStatsCapturePathIndex,
   SR_INT32 rttMonStatsCaptureHopIndex,
   SR_INT32 rttMonStatsCaptureDistIndex)
{
   static rttMonStatsCaptureEntry_t rttMonStatsCaptureEntryData;

   int         return_code;

   if (searchType == EXACT)
   {
     return_code = getRttMonStatsCaptureDistEntry(
                     rttMonCtrlAdminIndex,
                     rttMonStatsCaptureStartTimeIndex,
                     rttMonStatsCapturePathIndex,
                     rttMonStatsCaptureHopIndex,
                     rttMonStatsCaptureDistIndex,
                     &rttMonStatsCaptureEntryData);
   }
   else
   {
     return_code = getNextRttMonStatsCaptureDistEntry(
                     rttMonCtrlAdminIndex,
                     rttMonStatsCaptureStartTimeIndex,
                     rttMonStatsCapturePathIndex,
                     rttMonStatsCaptureHopIndex,
                     rttMonStatsCaptureDistIndex,
                     &rttMonStatsCaptureEntryData);
   }

   if (return_code != D_Return_Successfull)
   {
     return(NULL);
   }
   else
   {
     SET_ALL_VALID(rttMonStatsCaptureEntryData.valid);
   }

   return(&rttMonStatsCaptureEntryData);

}

rttMonStatsCollectEntry_t *
k_rttMonStatsCollectEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex,
   SR_UINT32 rttMonStatsCaptureStartTimeIndex,
   SR_INT32 rttMonStatsCapturePathIndex,
   SR_INT32 rttMonStatsCaptureHopIndex)
{

   int         return_code;
   
   if (searchType == EXACT)
   {
     return_code = getRttMonStatsCollectEntry(
                     rttMonCtrlAdminIndex,
                     rttMonStatsCaptureStartTimeIndex,
                     rttMonStatsCapturePathIndex,
                     rttMonStatsCaptureHopIndex,
                     rttMonStatsCollectEntryData);
   }
   else
   {
     return_code = getNextRttMonStatsCollectEntry(
                     rttMonCtrlAdminIndex,
                     rttMonStatsCaptureStartTimeIndex,
                     rttMonStatsCapturePathIndex,
                     rttMonStatsCaptureHopIndex,
                     rttMonStatsCollectEntryData);
   }

   if (return_code != D_Return_Successfull)
   {
     return(NULL);
   }
   else
   {
     SET_ALL_VALID(rttMonStatsCollectEntryData->valid);
   }

   return(rttMonStatsCollectEntryData);

}

rttMonStatsTotalsEntry_t *
k_rttMonStatsTotalsEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex,
   SR_UINT32 rttMonStatsCaptureStartTimeIndex)
{
   static rttMonStatsTotalsEntry_t rttMonStatsTotalsEntryData;

   int         return_code;

   if (searchType == EXACT)
   {
     return_code = getRttMonStatsTotalsEntry(
                     rttMonCtrlAdminIndex,
                     rttMonStatsCaptureStartTimeIndex,
                     &rttMonStatsTotalsEntryData);
   }
   else
   {
     return_code = getNextRttMonStatsTotalsEntry(
                     rttMonCtrlAdminIndex,
                     rttMonStatsCaptureStartTimeIndex,
                     &rttMonStatsTotalsEntryData);
   }

   if (return_code != D_Return_Successfull)
   {
     return(NULL);
   }
   else
   {
     SET_ALL_VALID(rttMonStatsTotalsEntryData.valid);
   }

   return(&rttMonStatsTotalsEntryData);

}

rttMonHistoryCollectionEntry_t *
k_rttMonHistoryCollectionEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   SR_INT32 rttMonCtrlAdminIndex,
   SR_INT32 rttMonHistoryCollectionLifeIndex,
   SR_INT32 rttMonHistoryCollectionBucketIndex,
   SR_INT32 rttMonHistoryCollectionSampleIndex)
{

   int         return_code;

   if (searchType == EXACT)
   {
     return_code = getRttMonHistoryCollectionEntry(
                     rttMonCtrlAdminIndex,
                     rttMonHistoryCollectionLifeIndex,
                     rttMonHistoryCollectionBucketIndex,
                     rttMonHistoryCollectionSampleIndex,
                     rttMonHistoryCollectionEntryData);
   }
   else
   {
     return_code = getNextRttMonHistoryCollectionEntry(
                     rttMonCtrlAdminIndex,
                     rttMonHistoryCollectionLifeIndex,
                     rttMonHistoryCollectionBucketIndex,
                     rttMonHistoryCollectionSampleIndex,
                     rttMonHistoryCollectionEntryData);
   }

   if (return_code != D_Return_Successfull)
   {
     return(NULL);
   }
   else
   {
     SET_ALL_VALID(rttMonHistoryCollectionEntryData->valid);
   }

   return(rttMonHistoryCollectionEntryData);

}

/*
 * rttMonConnectionChangeNotification NOTIFICATION-TYPE
 *   OBJECTS   {     rttMonCtrlAdminTag, 
 *                   rttMonHistoryCollectionAddress,
 *                   rttMonCtrlOperConnectionLostOccurred 
 *             }
 *   STATUS    current
 *   DESCRIPTION
 *        "This notification is only valid when the RttMonRttType
 *        is 'echo' or 'pathEcho'.
 *
 *        A rttMonConnectionChangeNotification indicates that a
 *        connection to a target (not to a hop along the path
 *        to a target) has either failed on establishment or 
 *        been lost and when reestablished.  Precisely, this
 *        has resulted in rttMonCtrlOperConnectionLostOccurred
 *        changing value.
 *
 *        If History is not being collected, or the RttMonRttType
 *        is not 'echo' or 'pathEcho', the 
 *        rttMonHistoryCollectionAddress will be null."
 *   ::= { rttMonNotifications 1 }
 *
 *
 * rttMonTimeoutNotification NOTIFICATION-TYPE
 *   OBJECTS   {     rttMonCtrlAdminTag, 
 *                   rttMonHistoryCollectionAddress,
 *                   rttMonCtrlOperTimeoutOccurred 
 *             }
 *   STATUS    current
 *   DESCRIPTION
 *        "A rttMonTimeoutNotification indicates the occurrence of 
 *        a timeout for a RTT operation, and it indicates the 
 *        clearing of such a condition by a subsequent RTT 
 *        operation. Precisely, this has resulted in 
 *        rttMonCtrlOperTimeoutOccurred changing value. 
 *
 *        When the RttMonRttType is 'pathEcho', this 
 *        notification will only be sent when the timeout 
 *        occurs during an operation to the target and not to
 *        a hop along the path to the target.  This also 
 *        applies to the clearing of the timeout. 
 *     
 *        If History is not being collected, or the RttMonRttType
 *        is not 'echo' or 'pathEcho', the 
 *        rttMonHistoryCollectionAddress will be null."
 *   ::= { rttMonNotifications 2 }
 *
 *
 * rttMonThresholdNotification NOTIFICATION-TYPE
 *   OBJECTS   {     rttMonCtrlAdminTag, 
 *                   rttMonHistoryCollectionAddress,
 *                   rttMonCtrlOperOverThresholdOccurred 
 *             }
 *   STATUS    current
 *   DESCRIPTION
 *        "A rttMonThresholdNotification indicates the 
 *        occurrence of a threshold violation for a RTT operation,
 *        and it indicates the previous violation has subsided for
 *        a subsequent RTT operation.  Precisely, this has resulted
 *        in rttMonCtrlOperOverThresholdOccurred changing value.  
 *  
 *        When the RttMonRttType is 'pathEcho', this 
 *        notification will only be sent when the threshold
 *        violation occurs during an operation to the target and
 *        not to a hop along the path to the target.  This also
 *        applies to the subsiding of a threshold condition.
 *
 *        If History is not being collected, or the RttMonRttType
 *        is not 'echo' or 'pathEcho', the 
 *        rttMonHistoryCollectionAddress will be null."
 *   ::= { rttMonNotifications 3 }
 */

static boolean global_rtt_trap_enable;

static const OID rttMonConnectionChangeNotificationOID = 
         { LNrttMonConnectionChangeNotification, 
           (ulong *)IDrttMonConnectionChangeNotification };

static char rttMonConnectionChangeNotificationOID_str[80];

static const OID rttMonTimeoutNotificationOID = 
         { LNrttMonTimeoutNotification, 
           (ulong *)IDrttMonTimeoutNotification };

static char rttMonTimeoutNotificationOID_str[80];

static const OID rttMonThresholdNotificationOID = 
         { LNrttMonThresholdNotification, 
           (ulong *)IDrttMonThresholdNotification };

static char rttMonThresholdNotificationOID_str[80];

static const OID rttMonCtrlAdminTag_OID =
  { LNrttMonCtrlAdminTag,
    (ulong *)IDrttMonCtrlAdminTag };

static const OID rttMonHistoryCollectionAddress_OID =
  { LNrttMonHistoryCollectionAddress,
    (ulong *)IDrttMonHistoryCollectionAddress };

static const OID rttMonCtrlOperConnectionLostOccurred_OID =
  { LNrttMonCtrlOperConnectionLostOccurred,
    (ulong *)IDrttMonCtrlOperConnectionLostOccurred };

static const OID rttMonCtrlOperTimeoutOccurred_OID =
  { LNrttMonCtrlOperTimeoutOccurred,
    (ulong *)IDrttMonCtrlOperTimeoutOccurred };

static const OID rttMonCtrlOperOverThresholdOccurred_OID =
  { LNrttMonCtrlOperOverThresholdOccurred,
    (ulong *)IDrttMonCtrlOperOverThresholdOccurred };


/*
 * all of the rtt traps are characterized by the following:
 * 1) three varbinds
 * 2) first varbind is always rttMonCtrlAdminTag
 * 3) second varbind is always rttMonHistoryCollectionAddress
 * 4) third varbind is one of rttMon*ThresholdOccurred
 *
 * so given the oid of the third varbind, the values and indices
 * of all of the varbinds, and the oid of the trap, all of the
 * trap generation code can be handled in one place
 */
static void
rtt_snmp_trap_generic (const OID *rttMonNotification,
		       SR_INT32	rttMonCtrlAdminIndex,
		       SR_INT32	rttMonHistoryCollectionLifeIndex,
		       SR_INT32	rttMonHistoryCollectionBucketIndex,
		       SR_INT32	rttMonHistoryCollectionSampleIndex,
		       const OctetString *rttMonCtrlAdminTag, 
		       const OctetString *rttMonHistoryCollectionAddress, 
		       const OID *rttMonThresholdOccurred_OID,     
		       SR_INT32 rttMonThresholdOccurred)
{
    ulong	instance[4];
    OID		instanceOID1;
    OID		instanceOID2;
    OID		enterprise;
    VarBind    *varBind1 = NULL;
    VarBind    *varBind2 = NULL;
    VarBind    *varBind3 = NULL;
    OctetString *cloneOS;
    int		trapNum;
    uchar	buffer[128];

    if (global_rtt_trap_enable == FALSE) {
	return;
    }

    /*
     * all of the objects in the varbind list are indexed either singularly
     * by rttMonCtrlAdminIndex, or by the four objects rttMonCtrlAdminIndex,
     * rttMonHistoryCollectionLifeIndex, rttMonHistoryCollectionBucketIndex,
     * and rttMonHistoryCollectionSampleIndex
     */

    instance[0] = rttMonCtrlAdminIndex;
    instance[1] = rttMonHistoryCollectionLifeIndex;
    instance[2] = rttMonHistoryCollectionBucketIndex;
    instance[3] = rttMonHistoryCollectionSampleIndex;
    instanceOID1.oid_ptr = instance;
    instanceOID1.length = 1;
    instanceOID2.oid_ptr = instance;
    instanceOID2.length = 4;

    /*
     * create the individual varbinds.  this is complicated by:
     * 1) the incoming octet strings need to be cloned so that they
     *    can be attached to the VarBind (the message sending code
     *    will free them)
     * 2) mallocs can fail, so must make allowances for that including
     *    not trying to make a varbind if the cloning fails and to
     *    make sure the clone is freed if making the varbind fails
     *
     * the following might be better coded as a series of nested
     * conditionals, but it would be difficult to read.  since
     * execution time is not a high priority when sending traps, we'll
     * just create what we need and do one sanity check at the end
     */

    /*
     * create the rttMonCtrlAdminTag varBind
     */
    cloneOS = CloneOctetString((OctetString *)rttMonCtrlAdminTag);
    if (cloneOS) {
	varBind1 = MakeVarBindWithValue((OID *)&rttMonCtrlAdminTag_OID,
					&instanceOID1,
					OCTET_PRIM_TYPE,
					cloneOS);
	if (varBind1 == NULL) {
	    FreeOctetString(cloneOS);
	}
    }

    /*
     * create the rttMonHistoryCollectionAddress varBind
     */
    cloneOS = CloneOctetString((OctetString *)rttMonHistoryCollectionAddress);
    if (cloneOS) {
	varBind2 = MakeVarBindWithValue((OID *)&rttMonHistoryCollectionAddress_OID,
					&instanceOID2,
					OCTET_PRIM_TYPE,
					cloneOS);
	if (varBind2 == NULL) {
	    FreeOctetString(cloneOS);
	}
    }

    /*
     * create the rttMon*ThresholdOccurred varBind
     */
    varBind3 = MakeVarBindWithValue((OID *)rttMonThresholdOccurred_OID,
				    &instanceOID2,
				    INTEGER_TYPE,
				    &rttMonThresholdOccurred);

    /*
     * make sure we're in good shape so far
     */
    if ((varBind1 == NULL) || (varBind2 == NULL) || (varBind3 == NULL)) {
	FreeVarBind(varBind1);
	FreeVarBind(varBind2);
	FreeVarBind(varBind3);
	return;
    }

    /*
     * ok, link them up
     */
    varBind1->next_var = varBind2;
    varBind2->next_var = varBind3;
    varBind3->next_var = NULL;

    /*
     * the stupid API provided by SNMP Research wants the V2 trap OID
     * to be supplied as a string.  go figure.
     */
    MakeDotFromOID((OID *)rttMonNotification, buffer);

    /*
     * the V1 trap spec-trap and enterprise parameters can be derived
     * from the V2 trap OID which is enterprise.0.spec-trap
     */
    enterprise.oid_ptr = rttMonNotification->oid_ptr;
    enterprise.length = rttMonNotification->length - 2;
    trapNum = rttMonNotification->oid_ptr[rttMonNotification->length-1];

    /*
     * send the trap
     */
    do_trap(ENTERPRISE_TRAP,
	    trapNum,
	    varBind1,
	    &enterprise,
	    buffer);

}

static void
rttMonConnectionChangeNotification_Notif (
    SR_INT32	 rttMonCtrlAdminIndex,
    SR_INT32	 rttMonHistoryCollectionLifeIndex,
    SR_INT32	 rttMonHistoryCollectionBucketIndex,
    SR_INT32	 rttMonHistoryCollectionSampleIndex,
    OctetString *rttMonCtrlAdminTag, 
    OctetString *rttMonHistoryCollectionAddress, 
    SR_INT32	 rttMonCtrlOperConnectionLostOccurred)
{
    rtt_snmp_trap_generic(&rttMonConnectionChangeNotificationOID,
			  rttMonCtrlAdminIndex,
			  rttMonHistoryCollectionLifeIndex,
			  rttMonHistoryCollectionBucketIndex,
			  rttMonHistoryCollectionSampleIndex,
			  rttMonCtrlAdminTag,
			  rttMonHistoryCollectionAddress,
			  &rttMonCtrlOperConnectionLostOccurred_OID,
			  rttMonCtrlOperConnectionLostOccurred);
}

static void
rttMonTimeoutNotification_Notif (
    SR_INT32	 rttMonCtrlAdminIndex,
    SR_INT32	 rttMonHistoryCollectionLifeIndex,
    SR_INT32	 rttMonHistoryCollectionBucketIndex,
    SR_INT32	 rttMonHistoryCollectionSampleIndex,
    OctetString *rttMonCtrlAdminTag, 
    OctetString *rttMonHistoryCollectionAddress, 
    SR_INT32	 rttMonCtrlOperTimeoutOccurred)
{
    rtt_snmp_trap_generic(&rttMonTimeoutNotificationOID,
			  rttMonCtrlAdminIndex,
			  rttMonHistoryCollectionLifeIndex,
			  rttMonHistoryCollectionBucketIndex,
			  rttMonHistoryCollectionSampleIndex,
			  rttMonCtrlAdminTag,
			  rttMonHistoryCollectionAddress,
			  &rttMonCtrlOperTimeoutOccurred_OID,
			  rttMonCtrlOperTimeoutOccurred);
}

static void
rttMonThresholdNotification_Notif (
    SR_INT32	 rttMonCtrlAdminIndex,
    SR_INT32	 rttMonHistoryCollectionLifeIndex,
    SR_INT32	 rttMonHistoryCollectionBucketIndex,
    SR_INT32	 rttMonHistoryCollectionSampleIndex,
    OctetString *rttMonCtrlAdminTag, 
    OctetString *rttMonHistoryCollectionAddress, 
    SR_INT32	 rttMonCtrlOperOverThresholdOccurred)
{
    rtt_snmp_trap_generic(&rttMonThresholdNotificationOID,
			  rttMonCtrlAdminIndex,
			  rttMonHistoryCollectionLifeIndex,
			  rttMonHistoryCollectionBucketIndex,
			  rttMonHistoryCollectionSampleIndex,
			  rttMonCtrlAdminTag,
			  rttMonHistoryCollectionAddress,
			  &rttMonCtrlOperOverThresholdOccurred_OID,
			  rttMonCtrlOperOverThresholdOccurred);
}

/*
 *  "snmp-server host <host> <community> rtr" nvgen routine 
 */
static void
rtt_snmp_host_nvgen (ulong flags)
{
  nv_add(flags & (1 << TRAP_RTT), " rtr");
}

/*
 * "snmp-server enable traps rtr" action routine
 */
static void
rtt_trap_cfg_set (boolean enable, uint subsys, uint traps)
{
  if ((subsys & (1 << TRAP_RTT))) 
  {
    global_rtt_trap_enable = enable;
  }
}

/*
 * "snmp-server enable traps rtr" nvgen routine
 */
static void
rtt_trap_cfg_nvwrt (parseinfo *csb)
{
  nv_write(global_rtt_trap_enable,
            "%s traps rtr",
            csb->nv_command);
}

static void
init_rttmib (subsystype *subsys)
{
    numVarBinds = 0;
    newRttMonAdminEntryRow = FALSE;
    newRttMonReactTriggerAdminEntryRow = FALSE;
    currRttMonCtrlAdminEntryPtr = NULL;
    currRttMonEchoAdminEntryPtr = NULL;

    global_rtt_trap_enable = FALSE;

    rttMonCtrlAdminEntryData = 
        newRttMonCtrlAdminEntry();
    if (rttMonCtrlAdminEntryData == NULL)
    {
      DPRINTF((APALWAYS, "\n Initialization of the RTT Monitor Mib Failed!\n"));
      return;
    }

    rttMonCtrlAdminEntryBuff = 
        newRttMonCtrlAdminEntry();
    if (rttMonCtrlAdminEntryBuff == NULL)
    {
      DPRINTF((APALWAYS, "\n Initialization of the RTT Monitor Mib Failed!\n"));
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryData);
      return;
    }
    setRttMonCtrlAdminEntryDefaults(rttMonCtrlAdminEntryBuff);

    rttMonEchoAdminEntryData =
        newRttMonEchoAdminEntry();
    if (rttMonEchoAdminEntryData == NULL)
    {
      DPRINTF((APALWAYS, "\n Initialization of the RTT Monitor Mib Failed!\n"));
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryData);
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryBuff);
      return;
    }

    rttMonEchoAdminEntryBuff =
        newRttMonEchoAdminEntry();
    if (rttMonEchoAdminEntryBuff == NULL)
    {
      DPRINTF((APALWAYS, "\n Initialization of the RTT Monitor Mib Failed!\n"));
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryData);
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryBuff);
      freeRttMonEchoAdminEntry(rttMonEchoAdminEntryData);
      return;
    }
    setRttMonEchoAdminEntryDefaults(rttMonEchoAdminEntryBuff);

    rttMonStatsCollectEntryData =
         newRttMonStatsCollectEntry();
    if (rttMonEchoAdminEntryData == NULL)
    {
      DPRINTF((APALWAYS, "\n Initialization of the RTT Monitor Mib Failed!\n"));
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryBuff);
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryData);
      freeRttMonEchoAdminEntry(rttMonEchoAdminEntryData);
      freeRttMonEchoAdminEntry(rttMonEchoAdminEntryBuff);
      return;
    }

    rttMonHistoryCollectionEntryData =
         newRttMonHistoryCollectionEntry();
    if (rttMonEchoAdminEntryData == NULL)
    {
      DPRINTF((APALWAYS, "\n Initialization of the RTT Monitor Mib Failed!\n"));
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryData);
      freeRttMonCtrlAdminEntry(rttMonCtrlAdminEntryBuff);
      freeRttMonEchoAdminEntry(rttMonEchoAdminEntryData);
      freeRttMonEchoAdminEntry(rttMonEchoAdminEntryBuff);
      freeRttMonStatsCollectEntry(rttMonStatsCollectEntryData);
      return;
    }

    MakeDotFromOID((OID *)&rttMonConnectionChangeNotificationOID,
         rttMonConnectionChangeNotificationOID_str);
    MakeDotFromOID((OID *)&rttMonTimeoutNotificationOID,
         rttMonTimeoutNotificationOID_str);
    MakeDotFromOID((OID *)&rttMonThresholdNotificationOID,
         rttMonThresholdNotificationOID_str);

    register_snmp_trap(TRAP_RTT, 
                        rttMonConnectionChangeNotificationOID_str);

    register_snmp_trap(TRAP_RTT,
                        rttMonTimeoutNotificationOID_str);

    register_snmp_trap(TRAP_RTT,
                        rttMonThresholdNotificationOID_str);

    reg_add_snmp_host_nvgen(rtt_snmp_host_nvgen,
                             "rtt_snmp_host_nvgen");
    reg_add_Trap_cfg_set(rtt_trap_cfg_set,
                          "rtt_trap_cfg_set");
    reg_add_Trap_cfg_nvwrt(rtt_trap_cfg_nvwrt,
                            "rtt_trap_cfg_nvwrt");

    rttmon_snmp_chain_init();

    reg_add_rttMonConnectionChangeNotification_activate(
            rttMonConnectionChangeNotification_Notif,
            "rttMonConnectionChangeNotification_Notif");

    reg_add_rttMonTimeoutNotification_activate(
            rttMonTimeoutNotification_Notif,
            "rttMonTimeoutNotification_Notif");

    reg_add_rttMonThresholdNotification_activate(
            rttMonThresholdNotification_Notif,
            "rttMonThresholdNotification_Notif");

    load_mib(rttmib_OidList, rttmib_OidListNum);
    load_oid(rttmib_oid_table);
}

/*
 * rttmib subsystem header
 */

/*
 *  WARNING: update "req" with the subsystem being monitored
 */
#define MAJVERSION_rttmib 1
#define MINVERSION_rttmib 0
#define EDITVERSION_rttmib 0

SUBSYS_HEADER (rttmib,
	       MAJVERSION_rttmib,
	       MINVERSION_rttmib,
	       EDITVERSION_rttmib,
	       init_rttmib,
	       SUBSYS_CLASS_MANAGEMENT,
	       /* "seq: ", */
	       NULL,
	       NULL);
      
