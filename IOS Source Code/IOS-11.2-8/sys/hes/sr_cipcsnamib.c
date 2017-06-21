/* $Id: sr_cipcsnamib.c,v 3.4.18.4 1996/07/01 18:44:21 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_cipcsnamib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Michael Otto (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_cipcsnamib.c,v $
 * Revision 3.4.18.4  1996/07/01  18:44:21  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.18.3  1996/04/04  05:32:39  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.4.18.2  1996/04/04  01:54:11  mordock
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Branch: California_branch
 * more than one source needs access to do_snallc_trap().  callers of this
 * routine need to use new type/value struct for passing in varbind values.
 *
 * Revision 3.4.18.1  1996/03/18  19:43:17  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:40:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:53:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/09  20:15:26  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1996/01/24  22:02:58  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.2  1995/11/17  09:19:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/08/09  19:29:20  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.5  1995/07/18  21:46:31  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.4  1995/06/30  21:31:37  motto
 * Merge confict was resolved incorrectly causing this error message.
 * This fixes the merge conflict error.
 * CSCdi36634:  %SUBSYS-2-NOTFOUND: Subsystem (cipcsnamib) needs subsystem
 * (cipvlanm
 *
 * Revision 2.3  1995/06/28  09:24:42  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/27  20:46:16  motto
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
 * Revision 2.1  1995/06/07  20:43:42  hampton
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
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_channelmibsupp.h"
#include "interface_private.h"
#include "if_cip.h"
#include "channelmib_utils.h"
#include "if_cip_sna.h"
#include "if_cip_vlan.h"
#include "sr_cipcsnamib.h"
#include "sr_cipcsnamib-mib.h"
#include "sr_ciplanmib.h"
#include "sr_snallcmib.h"

#include "../dspu/cipllc_types.h"
#include "cbus_registry.h"
#include "../snmp/snmp_trap_fe.h"
#include "../parser/parser_defs_snmp.h"
#include "../snmp/snmp_registry.h"

static void cip_csnatrap( hwidbtype *hwidb, ushort code, cip_llctype *csna_data);
static void cSnaDuplicateSapTrap( hwidbtype * hwidb, cip_llctype *csna_data);
static void cSnaLlcConnLimitTrap( hwidbtype *hwidb);

/*
 * Start of system-independent routines
 */
/*---------------------------------------------------------------------
 * Retrieve data from the cipCardCsnaAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardCsnaAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    cipCardCsnaAdminEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;


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

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cipCardCsnaAdminEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->cipCardEntryIndex;
        inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
        inst.oid_ptr[2] = data->cipCardSubChannelIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardCsnaAdminPath
    case I_cipCardCsnaAdminPath:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardCsnaAdminPath));
       }
       break;
#endif /* I_cipCardCsnaAdminPath */

#ifdef I_cipCardCsnaAdminDevice
    case I_cipCardCsnaAdminDevice:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardCsnaAdminDevice));
       }
       break;
#endif /* I_cipCardCsnaAdminDevice */

#ifdef I_cipCardCsnaAdminBlockDelayTime
    case I_cipCardCsnaAdminBlockDelayTime:
       dp = (void *) (&data->cipCardCsnaAdminBlockDelayTime);
       break;
#endif /* I_cipCardCsnaAdminBlockDelayTime */

#ifdef I_cipCardCsnaAdminBlockDelayLength
    case I_cipCardCsnaAdminBlockDelayLength:
       dp = (void *) (&data->cipCardCsnaAdminBlockDelayLength);
       break;
#endif /* I_cipCardCsnaAdminBlockDelayLength */

#ifdef I_cipCardCsnaAdminMaxBlockLength
    case I_cipCardCsnaAdminMaxBlockLength:
       dp = (void *) (&data->cipCardCsnaAdminMaxBlockLength);
       break;
#endif /* I_cipCardCsnaAdminMaxBlockLength */

#ifdef I_cipCardCsnaAdminRowStatus
    case I_cipCardCsnaAdminRowStatus:
       dp = (void *) (&data->cipCardCsnaAdminRowStatus);
       break;
#endif /* I_cipCardCsnaAdminRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cipCardCsnaAdminEntry data object.
 *---------------------------------------------------------------------*/
void
cipCardCsnaAdminEntry_free(cipCardCsnaAdminEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->cipCardCsnaAdminPath);
       FreeOctetString(data->cipCardCsnaAdminDevice);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cipCardCsnaAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
cipCardCsnaAdminEntry_cleanup(doList_t *trash)
{
   cipCardCsnaAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
   cipCardCsnaAdminEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cipCardCsnaAdminEntry family
 *---------------------------------------------------------------------*/
cipCardCsnaAdminEntry_t *
Clone_cipCardCsnaAdminEntry(cipCardCsnaAdminEntry_t *cipCardCsnaAdminEntry)
{
    cipCardCsnaAdminEntry_t *data;

    if ((data = malloc(sizeof(cipCardCsnaAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cipCardCsnaAdminEntry), sizeof(cipCardCsnaAdminEntry_t));

    data->cipCardCsnaAdminPath = CloneOctetString(cipCardCsnaAdminEntry->cipCardCsnaAdminPath);
    data->cipCardCsnaAdminDevice = CloneOctetString(cipCardCsnaAdminEntry->cipCardCsnaAdminDevice);

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
cipCardCsnaAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cipCardCsnaAdminEntry_t     *cipCardCsnaAdminEntry;
    long           cipCardEntryIndex;
    long           cipCardDtrBrdIndex;
    long           cipCardSubChannelIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 3) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    cipCardCsnaAdminEntry = k_cipCardCsnaAdminEntry_get(-1, contextInfo, -1, EXACT, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cipCardCsnaAdminEntry_set) &&
            (((cipCardCsnaAdminEntry_t *) (dp->data)) != NULL) &&
            (((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardEntryIndex == cipCardEntryIndex) &&
            (((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardDtrBrdIndex == cipCardDtrBrdIndex) &&
            (((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardSubChannelIndex == cipCardSubChannelIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cipCardCsnaAdminEntry_set;
        dp->cleanupMethod = cipCardCsnaAdminEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cipCardCsnaAdminEntry_UNDO
        dp->undoMethod = cipCardCsnaAdminEntry_undo;
#else /* SR_cipCardCsnaAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cipCardCsnaAdminEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cipCardCsnaAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cipCardCsnaAdminEntry(cipCardCsnaAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cipCardCsnaAdminEntry(cipCardCsnaAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(cipCardCsnaAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cipCardCsnaAdminEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardEntryIndex = (cipCardEntryIndex);
                SET_VALID(I_cipCardEntryIndex, ((cipCardCsnaAdminEntry_t *) (dp->data))->valid);

                ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardDtrBrdIndex = (cipCardDtrBrdIndex);
                SET_VALID(I_cipCardDtrBrdIndex, ((cipCardCsnaAdminEntry_t *) (dp->data))->valid);

                ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardSubChannelIndex = (cipCardSubChannelIndex);
                SET_VALID(I_cipCardSubChannelIndex, ((cipCardCsnaAdminEntry_t *) (dp->data))->valid);

                error_status = k_cipCardCsnaAdminEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cipCardCsnaAdminPath
   case I_cipCardCsnaAdminPath:

     if (((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminPath != NULL) {
        FreeOctetString(((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminPath);
     }

     ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminPath = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardCsnaAdminPath */

#ifdef I_cipCardCsnaAdminDevice
   case I_cipCardCsnaAdminDevice:

     if (((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminDevice != NULL) {
        FreeOctetString(((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminDevice);
     }

     ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminDevice = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardCsnaAdminDevice */

#ifdef I_cipCardCsnaAdminBlockDelayTime
   case I_cipCardCsnaAdminBlockDelayTime:

     if ( ((value->sl_value < 0) || (value->sl_value > 100)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminBlockDelayTime = value->sl_value;
     break;
#endif /* I_cipCardCsnaAdminBlockDelayTime */

#ifdef I_cipCardCsnaAdminBlockDelayLength
   case I_cipCardCsnaAdminBlockDelayLength:

     if ( ((value->sl_value < 0) || (value->sl_value > 65535)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminBlockDelayLength = value->sl_value;
     break;
#endif /* I_cipCardCsnaAdminBlockDelayLength */

#ifdef I_cipCardCsnaAdminMaxBlockLength
   case I_cipCardCsnaAdminMaxBlockLength:

     if ( ((value->sl_value < 4096) || (value->sl_value > 65535)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminMaxBlockLength = value->sl_value;
     break;
#endif /* I_cipCardCsnaAdminMaxBlockLength */

#ifdef I_cipCardCsnaAdminRowStatus
   case I_cipCardCsnaAdminRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_cipCardCsnaAdminRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (cipCardCsnaAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_cipCardCsnaAdminRowStatus_notInService) || (value->sl_value == D_cipCardCsnaAdminRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_cipCardCsnaAdminRowStatus_createAndGo) || (value->sl_value == D_cipCardCsnaAdminRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((cipCardCsnaAdminEntry_t *) (dp->data))->cipCardCsnaAdminRowStatus = value->sl_value;
     break;
#endif /* I_cipCardCsnaAdminRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipCardCsnaAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cipCardCsnaAdminEntry_test */
   error_status = k_cipCardCsnaAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cipCardCsnaAdminEntry_t *) (dp->data))->valid);
        error_status = k_cipCardCsnaAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cipCardCsnaAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cipCardCsnaAdminEntry_set((cipCardCsnaAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cipCardCsnaOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardCsnaOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipCardCsnaOperEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;


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

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cipCardCsnaOperEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->cipCardEntryIndex;
        inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
        inst.oid_ptr[2] = data->cipCardSubChannelIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardCsnaOperState
    case I_cipCardCsnaOperState:
       dp = (void *) (&data->cipCardCsnaOperState);
       break;
#endif /* I_cipCardCsnaOperState */

#ifdef I_cipCardCsnaOperSlowDownState
    case I_cipCardCsnaOperSlowDownState:
       dp = (void *) (&data->cipCardCsnaOperSlowDownState);
       break;
#endif /* I_cipCardCsnaOperSlowDownState */

#ifdef I_cipCardCsnaOperBlockDelayTime
    case I_cipCardCsnaOperBlockDelayTime:
       dp = (void *) (&data->cipCardCsnaOperBlockDelayTime);
       break;
#endif /* I_cipCardCsnaOperBlockDelayTime */

#ifdef I_cipCardCsnaOperBlockDelayLength
    case I_cipCardCsnaOperBlockDelayLength:
       dp = (void *) (&data->cipCardCsnaOperBlockDelayLength);
       break;
#endif /* I_cipCardCsnaOperBlockDelayLength */

#ifdef I_cipCardCsnaOperMaxBlockLength
    case I_cipCardCsnaOperMaxBlockLength:
       dp = (void *) (&data->cipCardCsnaOperMaxBlockLength);
       break;
#endif /* I_cipCardCsnaOperMaxBlockLength */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardCsnaStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardCsnaStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipCardCsnaStatsEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;
    long            cipCardDtrBrdIndex;
    long            cipCardSubChannelIndex;


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

    if ( (InstToInt(incoming, 2 + object->oid.length, &cipCardSubChannelIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &cipCardDtrBrdIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cipCardCsnaStatsEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->cipCardEntryIndex;
        inst.oid_ptr[1] = data->cipCardDtrBrdIndex;
        inst.oid_ptr[2] = data->cipCardSubChannelIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardCsnaStatsBlocksTxd
    case I_cipCardCsnaStatsBlocksTxd:
       dp = (void *) (&data->cipCardCsnaStatsBlocksTxd);
       break;
#endif /* I_cipCardCsnaStatsBlocksTxd */

#ifdef I_cipCardCsnaStatsBlocksRxd
    case I_cipCardCsnaStatsBlocksRxd:
       dp = (void *) (&data->cipCardCsnaStatsBlocksRxd);
       break;
#endif /* I_cipCardCsnaStatsBlocksRxd */

#ifdef I_cipCardCsnaStatsBytesTxd
    case I_cipCardCsnaStatsBytesTxd:
       dp = (void *) (&data->cipCardCsnaStatsBytesTxd);
       break;
#endif /* I_cipCardCsnaStatsBytesTxd */

#ifdef I_cipCardCsnaStatsHCBytesTxd
    case I_cipCardCsnaStatsHCBytesTxd:
       dp = (void *) (CloneUInt64(data->cipCardCsnaStatsHCBytesTxd));
       break;
#endif /* I_cipCardCsnaStatsHCBytesTxd */

#ifdef I_cipCardCsnaStatsBytesRxd
    case I_cipCardCsnaStatsBytesRxd:
       dp = (void *) (&data->cipCardCsnaStatsBytesRxd);
       break;
#endif /* I_cipCardCsnaStatsBytesRxd */

#ifdef I_cipCardCsnaStatsHCBytesRxd
    case I_cipCardCsnaStatsHCBytesRxd:
       dp = (void *) (CloneUInt64(data->cipCardCsnaStatsHCBytesRxd));
       break;
#endif /* I_cipCardCsnaStatsHCBytesRxd */

#ifdef I_cipCardCsnaStatsBlocksTxByBlockDelayTime
    case I_cipCardCsnaStatsBlocksTxByBlockDelayTime:
       dp = (void *) (&data->cipCardCsnaStatsBlocksTxByBlockDelayTime);
       break;
#endif /* I_cipCardCsnaStatsBlocksTxByBlockDelayTime */

#ifdef I_cipCardCsnaStatsBytesTxByBlockDelayTime
    case I_cipCardCsnaStatsBytesTxByBlockDelayTime:
       dp = (void *) (&data->cipCardCsnaStatsBytesTxByBlockDelayTime);
       break;
#endif /* I_cipCardCsnaStatsBytesTxByBlockDelayTime */

#ifdef I_cipCardCsnaStatsHCBytesTxByBlockDelayTime
    case I_cipCardCsnaStatsHCBytesTxByBlockDelayTime:
       dp = (void *) (CloneUInt64(data->cipCardCsnaStatsHCBytesTxByBlockDelayTime));
       break;
#endif /* I_cipCardCsnaStatsHCBytesTxByBlockDelayTime */

#ifdef I_cipCardCsnaStatsBlocksTxByBlockDelayLength
    case I_cipCardCsnaStatsBlocksTxByBlockDelayLength:
       dp = (void *) (&data->cipCardCsnaStatsBlocksTxByBlockDelayLength);
       break;
#endif /* I_cipCardCsnaStatsBlocksTxByBlockDelayLength */

#ifdef I_cipCardCsnaStatsBytesTxByBlockDelayLength
    case I_cipCardCsnaStatsBytesTxByBlockDelayLength:
       dp = (void *) (&data->cipCardCsnaStatsBytesTxByBlockDelayLength);
       break;
#endif /* I_cipCardCsnaStatsBytesTxByBlockDelayLength */

#ifdef I_cipCardCsnaStatsHCBytesTxByBlockDelayLength
    case I_cipCardCsnaStatsHCBytesTxByBlockDelayLength:
       dp = (void *) (CloneUInt64(data->cipCardCsnaStatsHCBytesTxByBlockDelayLength));
       break;
#endif /* I_cipCardCsnaStatsHCBytesTxByBlockDelayLength */

#ifdef I_cipCardCsnaStatsBlocksTxByMaxBlockLength
    case I_cipCardCsnaStatsBlocksTxByMaxBlockLength:
       dp = (void *) (&data->cipCardCsnaStatsBlocksTxByMaxBlockLength);
       break;
#endif /* I_cipCardCsnaStatsBlocksTxByMaxBlockLength */

#ifdef I_cipCardCsnaStatsBytesTxByMaxBlockLength
    case I_cipCardCsnaStatsBytesTxByMaxBlockLength:
       dp = (void *) (&data->cipCardCsnaStatsBytesTxByMaxBlockLength);
       break;
#endif /* I_cipCardCsnaStatsBytesTxByMaxBlockLength */

#ifdef I_cipCardCsnaStatsHCBytesTxByMaxBlockLength
    case I_cipCardCsnaStatsHCBytesTxByMaxBlockLength:
       dp = (void *) (CloneUInt64(data->cipCardCsnaStatsHCBytesTxByMaxBlockLength));
       break;
#endif /* I_cipCardCsnaStatsHCBytesTxByMaxBlockLength */

#ifdef I_cipCardCsnaStatsSlowDownsReceived
    case I_cipCardCsnaStatsSlowDownsReceived:
       dp = (void *) (&data->cipCardCsnaStatsSlowDownsReceived);
       break;
#endif /* I_cipCardCsnaStatsSlowDownsReceived */

#ifdef I_cipCardCsnaStatsSlowDownsSent
    case I_cipCardCsnaStatsSlowDownsSent:
       dp = (void *) (&data->cipCardCsnaStatsSlowDownsSent);
       break;
#endif /* I_cipCardCsnaStatsSlowDownsSent */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardSessionsAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardSessionsAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipCardSessionsAdminEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cipCardSessionsAdminEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->cipCardEntryIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardAdminMaxLlc2Sessions
    case I_cipCardAdminMaxLlc2Sessions:
       dp = (void *) (&data->cipCardAdminMaxLlc2Sessions);
       break;
#endif /* I_cipCardAdminMaxLlc2Sessions */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cipCardSessionsAdminEntry data object.
 *---------------------------------------------------------------------*/
void
cipCardSessionsAdminEntry_free(cipCardSessionsAdminEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cipCardSessionsAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
cipCardSessionsAdminEntry_cleanup( doList_t *trash)
{
   cipCardSessionsAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
   cipCardSessionsAdminEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cipCardSessionsAdminEntry family
 *---------------------------------------------------------------------*/
cipCardSessionsAdminEntry_t *
Clone_cipCardSessionsAdminEntry(cipCardSessionsAdminEntry_t *cipCardSessionsAdminEntry)
{
    cipCardSessionsAdminEntry_t *data;

    if ((data = malloc(sizeof(cipCardSessionsAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cipCardSessionsAdminEntry), sizeof(cipCardSessionsAdminEntry_t));


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
cipCardSessionsAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cipCardSessionsAdminEntry_t     *cipCardSessionsAdminEntry;
    long           cipCardEntryIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    cipCardSessionsAdminEntry = k_cipCardSessionsAdminEntry_get(-1, contextInfo, -1, EXACT, cipCardEntryIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cipCardSessionsAdminEntry_set) &&
            (((cipCardSessionsAdminEntry_t *) (dp->data)) != NULL) &&
            (((cipCardSessionsAdminEntry_t *) (dp->data))->cipCardEntryIndex == cipCardEntryIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cipCardSessionsAdminEntry_set;
        dp->cleanupMethod = cipCardSessionsAdminEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cipCardSessionsAdminEntry_UNDO
        dp->undoMethod = cipCardSessionsAdminEntry_undo;
#else /* SR_cipCardSessionsAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cipCardSessionsAdminEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cipCardSessionsAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cipCardSessionsAdminEntry(cipCardSessionsAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cipCardSessionsAdminEntry(cipCardSessionsAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef cipCardSessionsAdminEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(cipCardSessionsAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cipCardSessionsAdminEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cipCardSessionsAdminEntry_t *) (dp->data))->cipCardEntryIndex = (cipCardEntryIndex);
                SET_VALID(I_cipCardEntryIndex, ((cipCardSessionsAdminEntry_t *) (dp->data))->valid);

                error_status = k_cipCardSessionsAdminEntry_set_defaults(dp);
            }
#else /* cipCardSessionsAdminEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* cipCardSessionsAdminEntry_READ_CREATE */
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cipCardAdminMaxLlc2Sessions
   case I_cipCardAdminMaxLlc2Sessions:

     if ( ((value->sl_value < 0) || (value->sl_value > 4000)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardSessionsAdminEntry_t *) (dp->data))->cipCardAdminMaxLlc2Sessions = value->sl_value;
     break;
#endif /* I_cipCardAdminMaxLlc2Sessions */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipCardSessionsAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cipCardSessionsAdminEntry_test */
   error_status = k_cipCardSessionsAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cipCardSessionsAdminEntry_t *) (dp->data))->valid);
        error_status = k_cipCardSessionsAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cipCardSessionsAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cipCardSessionsAdminEntry_set((cipCardSessionsAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cipCardSessionsOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardSessionsOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipCardSessionsOperEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cipCardSessionsOperEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->cipCardEntryIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardOperMaxLlc2Sessions
    case I_cipCardOperMaxLlc2Sessions:
       dp = (void *) (&data->cipCardOperMaxLlc2Sessions);
       break;
#endif /* I_cipCardOperMaxLlc2Sessions */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardSessionsStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardSessionsStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipCardSessionsStatsEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            cipCardEntryIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &cipCardEntryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cipCardSessionsStatsEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->cipCardEntryIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardStatsHiWaterLlc2Sessions
    case I_cipCardStatsHiWaterLlc2Sessions:
       dp = (void *) (&data->cipCardStatsHiWaterLlc2Sessions);
       break;
#endif /* I_cipCardStatsHiWaterLlc2Sessions */

#ifdef I_cipCardStatsLlc2SessionAllocationErrs
    case I_cipCardStatsLlc2SessionAllocationErrs:
       dp = (void *) (&data->cipCardStatsLlc2SessionAllocationErrs);
       break;
#endif /* I_cipCardStatsLlc2SessionAllocationErrs */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipCardCsnaConnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardCsnaConnEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    cipCardCsnaConnEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            llcPortVirtualIndex;
    long            llcSapNumber;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 5 
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

    if ( (InstToInt(incoming, 2 + object->oid.length, &llcSapNumber, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcSapNumber = MAX(1, llcSapNumber);
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &llcPortVirtualIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcPortVirtualIndex = MAX(0, llcPortVirtualIndex);
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
     if ( (arg == -1) || (data = k_cipCardCsnaConnEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->llcPortVirtualIndex;
        inst.oid_ptr[2] = data->llcSapNumber;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipCardCsnaConnActiveSessions
    case I_cipCardCsnaConnActiveSessions:
       dp = (void *) (&data->cipCardCsnaConnActiveSessions);
       break;
#endif /* I_cipCardCsnaConnActiveSessions */

#ifdef I_cipCardCsnaSlot
    case I_cipCardCsnaSlot:
       dp = (void *) (&data->cipCardCsnaSlot);
       break;
#endif /* I_cipCardCsnaSlot */

#ifdef I_cipCardCsnaPort
    case I_cipCardCsnaPort:
       dp = (void *) (&data->cipCardCsnaPort);
       break;
#endif /* I_cipCardCsnaPort */

#ifdef I_cipCardCsnaConnPath
    case I_cipCardCsnaConnPath:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardCsnaConnPath));
       }
       break;
#endif /* I_cipCardCsnaConnPath */

#ifdef I_cipCardCsnaConnDevice
    case I_cipCardCsnaConnDevice:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardCsnaConnDevice));
       }
       break;
#endif /* I_cipCardCsnaConnDevice */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */





cipCardCsnaAdminEntry_t *
k_cipCardCsnaAdminEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cipCardEntryIndex;
   long cipCardDtrBrdIndex;
   long cipCardSubChannelIndex;
{
   static cipCardCsnaAdminEntry_t cipCardCsnaAdminData;
   
   cipCardCsnaAdminEntry_t *result_p = NULL;
 
   cip_vctype *vc;
   long subchannel_path, subchannel_device;
   int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;
   static unsigned char path[2], device[2];
   static OctetString path_OS, device_OS;

   index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
	   	        &subchannel_device);
    
   if ((vc = cip_getSubchannelCsnaConfig(&cipCardEntryIndex, &cipCardDtrBrdIndex, 
	     &subchannel_path, &subchannel_device, NULL, getType)) != NULL) {
      cipCardCsnaAdminData.cipCardEntryIndex = cipCardEntryIndex;
      cipCardCsnaAdminData.cipCardDtrBrdIndex = cipCardDtrBrdIndex;
      cipCardCsnaAdminData.cipCardSubChannelIndex = 
          path_device_to_index(subchannel_path, subchannel_device);
    
      path[0] = vc->path >> 8;
      path[1] = vc->path & 0x00FF;
      InitOctetString(&path_OS, &path[0], sizeof(path));
      cipCardCsnaAdminData.cipCardCsnaAdminPath = &path_OS;

      device[0] = vc->device >> 8;
      device[1] = vc->device & 0x00FF;
      InitOctetString(&device_OS, &device[0], sizeof(device));
      cipCardCsnaAdminData.cipCardCsnaAdminDevice = &device_OS;
      cipCardCsnaAdminData.cipCardCsnaAdminBlockDelayTime = 
                           vc->blocking_time_delay;
      cipCardCsnaAdminData.cipCardCsnaAdminBlockDelayLength = 
                           vc->blocking_length_delay;
      cipCardCsnaAdminData.cipCardCsnaAdminMaxBlockLength = vc->maxpiu;
      cipCardCsnaAdminData.cipCardCsnaAdminRowStatus = 
                           D_cipCardCsnaAdminRowStatus_active;

      SET_ALL_VALID( cipCardCsnaAdminData.valid);
      result_p = &cipCardCsnaAdminData;
   };
   return result_p;
}

#ifdef SETS
int
k_cipCardCsnaAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cipCardCsnaAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cipCardCsnaAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    return NO_ERROR;
}

int
k_cipCardCsnaAdminEntry_set(data, contextInfo, function)
   cipCardCsnaAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cipCardCsnaAdminEntry_UNDO
/* add #define SR_cipCardCsnaAdminEntry_UNDO in cipcsnamib.h to
 * include the undo routine for the cipCardCsnaAdminEntry family.
 */
int
cipCardCsnaAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cipCardCsnaAdminEntry_UNDO */
#endif /* SR_SNMPv2 */

static long get_snmp_csna_state (long cip_csna_state) {
    long state;
    
    switch (cip_csna_state) {
      case CSNA_CLOSED:
        state = D_cipCardCsnaOperState_closed;
        break;
      case CSNA_PENDING_OPEN:
        state = D_cipCardCsnaOperState_pendingOpen;
        break;
      case CSNA_OPEN:
        state = D_cipCardCsnaOperState_open;
        break;
      case CSNA_PENDING_SETUP:
        state = D_cipCardCsnaOperState_pendingSetup;
        break;
      case CSNA_SETUP_COMPLETE:
        state = D_cipCardCsnaOperState_setupComplete;
        break;
      case CSNA_PENDING_CLOSE:
        state = D_cipCardCsnaOperState_pendingClose;
        break;
      default:
        state = D_cipCardCsnaOperState_closed;
        break;
    }
    return state;
}

static long get_snmp_slowdown_state (long cip_slowdown_state) {
    long state;
    
    switch (cip_slowdown_state) {
      case CSNA_SLOWDOWN_NORMAL:
        state = D_cipCardCsnaOperSlowDownState_normal;
        break;
      case CSNA_SLOWDOWN_SENT:   
        state = D_cipCardCsnaOperSlowDownState_slowDownSent;
        break;
      case CSNA_SLOWDOWN_RECEIVED:
        state = D_cipCardCsnaOperSlowDownState_slowDownReceived;
        break;
      case CSNA_SLOWDOWN_SENT_AND_RECEIVED:
        state = D_cipCardCsnaOperSlowDownState_slowDownSentAndReceived;
        break;
      default:
        state = D_cipCardCsnaOperSlowDownState_normal;
        break;
    }
    return state;
}

cipCardCsnaOperEntry_t *
k_cipCardCsnaOperEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cipCardEntryIndex;
   long cipCardDtrBrdIndex;
   long cipCardSubChannelIndex;
{
   static cipCardCsnaOperEntry_t cipCardCsnaOperData;
   static int lastSerialNum = MAXINT;
   static long lastCipCardEntryIndex = MAXLONG;
   static long lastCipCardDtrBrdIndex = MAXLONG;
   static long lastCipCardSubChannelIndex = MAXLONG;

   cipCardCsnaOperEntry_t *result_p = NULL;
   csna_dev_t csna_dev;
   long subchannel_path, subchannel_device;
   int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;

   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) &&
       (lastCipCardEntryIndex == cipCardEntryIndex) &&
       (lastCipCardDtrBrdIndex == cipCardDtrBrdIndex) &&
       (lastCipCardSubChannelIndex == cipCardSubChannelIndex)) {
      result_p = &cipCardCsnaOperData;
   }
   else {
      lastCipCardEntryIndex = cipCardEntryIndex;
      lastCipCardDtrBrdIndex = cipCardDtrBrdIndex;
      index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
 		           &subchannel_device);
      if (get_csna_dev( getType, &lastCipCardEntryIndex, 
                        &lastCipCardDtrBrdIndex,
                        &subchannel_path, &subchannel_device, &csna_dev)) {
         cipCardCsnaOperData.cipCardCsnaOperState = 
                        get_snmp_csna_state(csna_dev.cipCardCsnaOperState);
         cipCardCsnaOperData.cipCardCsnaOperSlowDownState = 
                        get_snmp_slowdown_state(csna_dev.cipCardCsnaOperSlowDownState);
         cipCardCsnaOperData.cipCardCsnaOperBlockDelayTime = 
                             csna_dev.cipCardCsnaOperBlockDelayTime;
         cipCardCsnaOperData.cipCardCsnaOperBlockDelayLength = 
                             csna_dev.cipCardCsnaOperBlockDelayLength;
         cipCardCsnaOperData.cipCardCsnaOperMaxBlockLength = 
                             csna_dev.cipCardCsnaOperMaxBlockLength;
         cipCardCsnaOperData.cipCardEntryIndex = lastCipCardEntryIndex;
         cipCardCsnaOperData.cipCardDtrBrdIndex = lastCipCardDtrBrdIndex;
         lastCipCardSubChannelIndex = cipCardSubChannelIndex;
         lastCipCardDtrBrdIndex = cipCardDtrBrdIndex;
         lastCipCardEntryIndex = cipCardEntryIndex;
         cipCardCsnaOperData.cipCardSubChannelIndex = 
                    path_device_to_index(subchannel_path, subchannel_device);
         lastSerialNum = serialNum;
         SET_ALL_VALID( cipCardCsnaOperData.valid);
         result_p = &cipCardCsnaOperData;
      }
   }
   return result_p;
}

cipCardCsnaStatsEntry_t *
k_cipCardCsnaStatsEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cipCardEntryIndex;
   long cipCardDtrBrdIndex;
   long cipCardSubChannelIndex;
{
   static cipCardCsnaStatsEntry_t cipCardCsnaStatsData;
   static int lastSerialNum = MAXINT;
   static long lastCipCardEntryIndex = MAXLONG;
   static long lastCipCardDtrBrdIndex = MAXLONG;
   static long lastCipCardSubChannelIndex = MAXLONG;
   static UInt64 txBytes, rxBytes;
   static UInt64 bytesByDelayTime, 
                 bytesByDelayLength,
                 bytesByBlockLength;

   cipCardCsnaStatsEntry_t *result_p= NULL;
   csna_dev_t csna_dev;
   long subchannel_path, subchannel_device;
   int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;

   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) &&
       (lastCipCardEntryIndex == cipCardEntryIndex) &&
       (lastCipCardDtrBrdIndex == cipCardDtrBrdIndex) &&
       (lastCipCardSubChannelIndex == cipCardSubChannelIndex)) {
      result_p = &cipCardCsnaStatsData;
   }
   else {
      lastCipCardEntryIndex = cipCardEntryIndex;
      lastCipCardDtrBrdIndex = cipCardDtrBrdIndex;
      index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
 		           &subchannel_device);
      if (get_csna_dev( getType, &lastCipCardEntryIndex, &lastCipCardDtrBrdIndex,
                        &subchannel_path, &subchannel_device, &csna_dev)) {
         cipCardCsnaStatsData.cipCardCsnaStatsBlocksTxd = 
                     csna_dev.cipCardCsnaStatsBlocksTxd;
         cipCardCsnaStatsData.cipCardCsnaStatsBlocksRxd = 
                     csna_dev.cipCardCsnaStatsBlocksRxd;
         cipCardCsnaStatsData.cipCardCsnaStatsBytesTxd = 
                     csna_dev.cipCardCsnaStatsBytesTxd_lsw;
         txBytes.big_end = csna_dev.cipCardCsnaStatsBytesTxd_msw;
         txBytes.little_end = csna_dev.cipCardCsnaStatsBytesTxd_lsw;
         cipCardCsnaStatsData.cipCardCsnaStatsHCBytesTxd = &txBytes;
         cipCardCsnaStatsData.cipCardCsnaStatsBytesRxd = 
                     csna_dev.cipCardCsnaStatsBytesRxd_lsw;
         rxBytes.big_end = csna_dev.cipCardCsnaStatsBytesRxd_msw;
         rxBytes.little_end = csna_dev.cipCardCsnaStatsBytesRxd_lsw;
         cipCardCsnaStatsData.cipCardCsnaStatsHCBytesRxd = &rxBytes;

         cipCardCsnaStatsData.cipCardCsnaStatsBlocksTxByBlockDelayTime = 
                     csna_dev.cipCardCsnaStatsBlocksTxByBlockDelayTime;
         cipCardCsnaStatsData.cipCardCsnaStatsBytesTxByBlockDelayTime =
                     csna_dev.cipCardCsnaStatsBytesTxByBlockDelayTime_lsw;
         bytesByDelayTime.big_end = 
                     csna_dev.cipCardCsnaStatsBytesTxByBlockDelayTime_msw;
         bytesByDelayTime.little_end = 
                     csna_dev.cipCardCsnaStatsBytesTxByBlockDelayTime_lsw;
         cipCardCsnaStatsData.cipCardCsnaStatsHCBytesTxByBlockDelayTime = 
                     &bytesByDelayTime;

         cipCardCsnaStatsData.cipCardCsnaStatsBlocksTxByBlockDelayLength = 
                     csna_dev.cipCardCsnaStatsBlocksTxByBlockDelayLength;
         cipCardCsnaStatsData.cipCardCsnaStatsBytesTxByBlockDelayLength =
                     csna_dev.cipCardCsnaStatsBytesTxByBlockDelayLength_lsw;
         bytesByDelayLength.big_end = 
                     csna_dev.cipCardCsnaStatsBytesTxByBlockDelayLength_msw;
         bytesByDelayLength.little_end = 
                     csna_dev.cipCardCsnaStatsBytesTxByBlockDelayLength_lsw;
         cipCardCsnaStatsData.cipCardCsnaStatsHCBytesTxByBlockDelayLength = 
                     &bytesByDelayLength;

         cipCardCsnaStatsData.cipCardCsnaStatsBlocksTxByMaxBlockLength = 
                     csna_dev.cipCardCsnaStatsBlocksTxByMaxBlockLength;
         cipCardCsnaStatsData.cipCardCsnaStatsBytesTxByMaxBlockLength =
                     csna_dev.cipCardCsnaStatsBytesTxByMaxBlockLength_lsw;
         bytesByBlockLength.big_end = 
                     csna_dev.cipCardCsnaStatsBytesTxByMaxBlockLength_msw;
         bytesByBlockLength.little_end = 
                     csna_dev.cipCardCsnaStatsBytesTxByMaxBlockLength_lsw;
         cipCardCsnaStatsData.cipCardCsnaStatsHCBytesTxByMaxBlockLength = 
                     &bytesByBlockLength;


         cipCardCsnaStatsData.cipCardCsnaStatsSlowDownsSent = 
                     csna_dev.cipCardCsnaStatsSlowDownsSent;
         cipCardCsnaStatsData.cipCardCsnaStatsSlowDownsReceived = 
                     csna_dev.cipCardCsnaStatsSlowDownsReceived;

         cipCardCsnaStatsData.cipCardEntryIndex = lastCipCardEntryIndex;
         cipCardCsnaStatsData.cipCardDtrBrdIndex = lastCipCardDtrBrdIndex;
         lastCipCardSubChannelIndex = cipCardSubChannelIndex;
         lastCipCardDtrBrdIndex = cipCardDtrBrdIndex;
         lastCipCardEntryIndex = cipCardEntryIndex;
         cipCardCsnaStatsData.cipCardSubChannelIndex = 
                    path_device_to_index(subchannel_path, subchannel_device);
         lastSerialNum = serialNum;
         SET_ALL_VALID( cipCardCsnaStatsData.valid);
         result_p = &cipCardCsnaStatsData;
      }
   }
   return result_p;
}

cipCardSessionsAdminEntry_t *
k_cipCardSessionsAdminEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cipCardEntryIndex;
{
   static cipCardSessionsAdminEntry_t cipCardSessionsAdminData;
   long llc2_sessions;
   cipCardSessionsAdminEntry_t *result_p = NULL;
   int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;

   if (get_admin_llc2_sessions( getType, &cipCardEntryIndex, &llc2_sessions)) {
      cipCardSessionsAdminData.cipCardAdminMaxLlc2Sessions = llc2_sessions;
      cipCardSessionsAdminData.cipCardEntryIndex = cipCardEntryIndex;
      result_p = &cipCardSessionsAdminData;
   }
   return result_p;
}

#ifdef SETS
int
k_cipCardSessionsAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cipCardSessionsAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cipCardSessionsAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    return NO_ERROR;
}

int
k_cipCardSessionsAdminEntry_set(data, contextInfo, function)
   cipCardSessionsAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cipCardSessionsAdminEntry_UNDO
/* add #define SR_cipCardSessionsAdminEntry_UNDO in cipcsnamib.h to
 * include the undo routine for the cipCardSessionsAdminEntry family.
 */
int
cipCardSessionsAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cipCardSessionsAdminEntry_UNDO */
#endif /* SR_SNMPv2 */

cipCardSessionsOperEntry_t *
k_cipCardSessionsOperEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cipCardEntryIndex;
{
   static cipCardSessionsOperEntry_t cipCardSessionsOperData;
   
   llc2_sessions_t llc2_sess;
   cipCardSessionsOperEntry_t *result_p = NULL;
   int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;

   if (get_llc2_sessions_info( getType, &cipCardEntryIndex, &llc2_sess)) {
      cipCardSessionsOperData.cipCardOperMaxLlc2Sessions = 
                    llc2_sess.cipCardOperMaxLlc2Sessions;
      cipCardSessionsOperData.cipCardEntryIndex = cipCardEntryIndex;
      result_p = &cipCardSessionsOperData;
   }
   return result_p;
}

cipCardSessionsStatsEntry_t *
k_cipCardSessionsStatsEntry_get (serialNum, contextInfo, nominator, searchType, cipCardEntryIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cipCardEntryIndex;
{
   static cipCardSessionsStatsEntry_t cipCardSessionsStatsData;
   
   llc2_sessions_t llc2_sess;
   cipCardSessionsStatsEntry_t *result_p = NULL;
   int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;

   if (get_llc2_sessions_info( getType, &cipCardEntryIndex, &llc2_sess)) {
      cipCardSessionsStatsData.cipCardStatsHiWaterLlc2Sessions = 
                     llc2_sess.cipCardStatsHiWaterLlc2Sessions;
      cipCardSessionsStatsData.cipCardStatsLlc2SessionAllocationErrs = 
                     llc2_sess.cipCardStatsLlc2SessionAllocationErrs;
      cipCardSessionsStatsData.cipCardEntryIndex = cipCardEntryIndex;
      result_p = &cipCardSessionsStatsData;
   }
   return result_p;
}

cipCardCsnaConnEntry_t *
k_cipCardCsnaConnEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
   long llcSapNumber;
{
   static cipCardCsnaConnEntry_t cipCardCsnaConnData;
   static int   lastSerialNum = MAXINT;
   static long  lastIfIndex = MAXLONG;
   static long  lastLlcPortVirtualIndex = MAXLONG;
   static long  lastLlcSapNumber = MAXLONG;

   static unsigned char path[2], device[2];
   static OctetString path_OS, device_OS;

   hwidbtype *hwidb = NULL;
   csna_llc2_conn_t csna_llc2_conn;
   cipCardCsnaConnEntry_t *result_p = NULL;
   llc_sap_oper_t sap_oper;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex) && 
       (lastLlcSapNumber == llcSapNumber)) {
      result_p = &cipCardCsnaConnData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      lastLlcSapNumber = llcSapNumber;
      if (get_sap_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                         lastLlcSapNumber, &sap_oper, LLC_SAP_OPER_SIZE, 
                         SYSMGT_LLC_SAP_OPER)) {
         hwidb = cip_get_hwidb( EXACT, lastIfIndex);
         if (hwidb && get_llc2_conn(hwidb, sap_oper.csnaHandle, 
                                    &csna_llc2_conn)) {
            cipCardCsnaConnData.cipCardCsnaConnActiveSessions = 
                                sap_oper.llcSapOperNumCcs;
            cipCardCsnaConnData.cipCardCsnaSlot = hwidb->slot;
            cipCardCsnaConnData.cipCardCsnaPort = 
                                csna_llc2_conn.cipCardCsnaPort;
            path[0] = csna_llc2_conn.cipCardCsnaConnPath >> 8;
            path[1] = csna_llc2_conn.cipCardCsnaConnPath & 0x00FF;
            InitOctetString(&path_OS, &path[0], sizeof(path));
            cipCardCsnaConnData.cipCardCsnaConnPath = &path_OS;
            device[0] = csna_llc2_conn.cipCardCsnaConnDevice >> 8;
            device[1] = csna_llc2_conn.cipCardCsnaConnDevice & 0x00FF;
            InitOctetString(&device_OS, &device[0], sizeof(device));
            cipCardCsnaConnData.cipCardCsnaConnDevice = &device_OS;
            cipCardCsnaConnData.llcSapNumber = sap_oper.index.local_sap;
            cipCardCsnaConnData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
            cipCardCsnaConnData.ifIndex = lastIfIndex;
            lastIfIndex = ifIndex;
            lastLlcPortVirtualIndex = llcPortVirtualIndex;
            lastLlcSapNumber = llcSapNumber;
            lastSerialNum = serialNum;
            SET_ALL_VALID( cipCardCsnaConnData.valid);
            result_p = &cipCardCsnaConnData;
	 }
      }
   }
   return result_p;
}

static char * trap_cSnaDuplicateSap = "ciscoCipCsnaMIB.2.0.1";
static char * trap_cSnaLlcConnLimit = "ciscoCipCsnaMIB.2.0.2";

static void init_cipcsnamib (subsystype *subsys)
{
    load_mib(cipcsnamib_OidList, cipcsnamib_OidListNum);
    load_oid(cipcsnamib_oid_table);
    reg_add_cip_cipcsna_trap(cip_csnatrap, "cip_csnatrap");
    register_snmp_trap(TRAP_LLC2, trap_cSnaDuplicateSap);
    register_snmp_trap(TRAP_LLC2, trap_cSnaLlcConnLimit);
}

static void cip_csnatrap (hwidbtype *hwidb, ushort code, cip_llctype *csna_data)
{
    switch (code) {
        case DUP_SAP_EVENT:
            cSnaDuplicateSapTrap(hwidb, csna_data);
            break;
        case CCS_EXCEEDED_EVENT:
            cSnaLlcConnLimitTrap(hwidb);
            break;
        case LINK_LOST_EVENT:
        case DM_RXD_EVENT:
        case SABME_RXD_IN_ABME_EVENT:
        case FRMR_RXD_EVENT:
        case FRMR_TXD_EVENT:
            snaLlcStatusChangeTrap(hwidb, csna_data, code);
            break;
        default:
            break;
    }
}

static const OID enterprise_cipcSnaTrap = {LNcipCsnaNotificationPrefix,
                                  (ulong *)IDcipCsnaNotificationPrefix};

#define CSNA_DUPSAP_TRAP_VBCOUNT 4
#define CSNA_DUPSAP_TRAP_NUM 1

static const OID varbinds_cSnaLlcConnLimitTrap[CSNA_DUPSAP_TRAP_VBCOUNT] = {
     OIDconversion(cipCardCsnaSlot),
     OIDconversion(cipCardCsnaPort),
     OIDconversion(cipCardCsnaConnPath),
     OIDconversion(cipCardCsnaConnDevice)
};

static void cSnaDuplicateSapTrap (hwidbtype * hwidb, cip_llctype *csna_data)
{
  int       idx;
  OID       indexOID[CSNA_DUPSAP_TRAP_VBCOUNT];
  ulong     index[3];
  vb_value  dataP[CSNA_DUPSAP_TRAP_VBCOUNT];
  char      tbuff[2];
  u_long    csna_slot = hwidb->slot;
  u_long    csna_port = csna_data->csna_port;

  index[0] = hwidb->snmp_if_index;
  index[1] = csna_data->virtual_index;
  index[2] = csna_data->local_sap;

  /* Fill in the indexing information */
  for (idx = 0; idx < CSNA_DUPSAP_TRAP_VBCOUNT; idx++) {
       indexOID[idx].length = 3;
       indexOID[idx].oid_ptr = index;
  }

  /* Fill in the data pointers */
  dataP[0].value = &csna_slot;
  dataP[0].type = INTEGER_TYPE;
  dataP[1].value = &csna_port;
  dataP[1].type = INTEGER_TYPE;

  tbuff[0] = csna_data->csna_path >> 8;
  tbuff[1] = csna_data->csna_path & 0x00FF;
  dataP[2].value = MakeOctetString(tbuff, 2);
  dataP[2].type = OCTET_PRIM_TYPE;

  tbuff[0] = csna_data->csna_device >> 8;
  tbuff[1] = csna_data->csna_device & 0x00FF;
  dataP[3].value = MakeOctetString(tbuff, 2);
  dataP[3].type = OCTET_PRIM_TYPE;

  do_snallc_trap(varbinds_cSnaLlcConnLimitTrap, &enterprise_cipcSnaTrap, 
                 trap_cSnaDuplicateSap, indexOID, CSNA_DUPSAP_TRAP_VBCOUNT, 
                 ENTERPRISE_TRAP, CSNA_DUPSAP_TRAP_NUM, dataP);
}


#define CSNA_LLCLIM_TRAP_VBCOUNT 4
#define CSNA_LLCLIM_TRAP_NUM 2

static const OID varbinds_cSnaLlcConnLimit[CSNA_LLCLIM_TRAP_VBCOUNT] = {
    OIDconversion(cipCardAdminMaxLlc2Sessions),
    OIDconversion(cipCardOperMaxLlc2Sessions),
    OIDconversion(cipCardStatsHiWaterLlc2Sessions),
    OIDconversion(cipCardStatsLlc2SessionAllocationErrs)
};

static void cSnaLlcConnLimitTrap (hwidbtype *hwidb)
{
  int       idx;
  OID       indexOID[CSNA_LLCLIM_TRAP_VBCOUNT];
  vb_value  dataP[CSNA_LLCLIM_TRAP_VBCOUNT];
  ulong     index = hwidb->slot;
  ulong sessions;
  llc2_sessions_t llc2_sess; 

  /* Fill in the indexing information */
  for (idx = 0; idx < CSNA_LLCLIM_TRAP_VBCOUNT; idx++) {
       indexOID[idx].length = 1;
       indexOID[idx].oid_ptr = &index;
  }

  /* Make calls to get LLC2 Session Information */
  sessions = cip_get_csna_vlan_maxllc2_sessions( hwidb);
  if (!get_from_cip_llc2_session_info(hwidb, SEARCH_MATCH, &llc2_sess))
       return;

  /* Fill in the data pointers */
  dataP[0].value = &sessions;
  dataP[0].type = INTEGER_TYPE;
  dataP[1].value = &llc2_sess.cipCardOperMaxLlc2Sessions;
  dataP[1].type = INTEGER_TYPE;
  dataP[2].value = &llc2_sess.cipCardStatsHiWaterLlc2Sessions;
  dataP[2].type = GAUGE_TYPE;
  dataP[3].value = &llc2_sess.cipCardStatsLlc2SessionAllocationErrs;
  dataP[3].type = COUNTER_TYPE;

  do_snallc_trap(varbinds_cSnaLlcConnLimit, &enterprise_cipcSnaTrap, 
                 trap_cSnaLlcConnLimit, indexOID, CSNA_LLCLIM_TRAP_VBCOUNT, 
                 ENTERPRISE_TRAP, CSNA_LLCLIM_TRAP_NUM, dataP);
}

/*
 * cipcsnamib subsystem header
 */
#define MAJVERSION_cipcsnamib 1
#define MINVERSION_cipcsnamib 0
#define EDITVERSION_cipcsnamib 0

SUBSYS_HEADER(cipcsnamib,
              MAJVERSION_cipcsnamib,
              MINVERSION_cipcsnamib,
              EDITVERSION_cipcsnamib,
              init_cipcsnamib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: cip, ciplanmib", NULL);
      
