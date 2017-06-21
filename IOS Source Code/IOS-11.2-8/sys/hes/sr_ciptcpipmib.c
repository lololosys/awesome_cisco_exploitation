/* $Id: sr_ciptcpipmib.c,v 3.4.8.2 1996/07/01 18:44:26 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_ciptcpipmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1995, Michael Otto (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_ciptcpipmib.c,v $
 * Revision 3.4.8.2  1996/07/01  18:44:26  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.8.1  1996/04/04  05:32:42  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.4  1996/02/28  16:55:21  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1996/01/24  22:03:01  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.2  1995/11/17  09:20:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/28  09:24:46  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/27  20:46:21  motto
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
 * Revision 2.1  1995/06/07  20:43:46  hampton
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
#include "sr_ciptcpipmib.h"
#include "sr_ciptcpipmib-mib.h"
#include "interface_private.h"
#include "if_cip.h"
#include "if_cip_tcpip.h"
#include "sysmgt_ciptcpip.h"
#include "channelmib_utils.h"


/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the cipIpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipIpEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipIpEntry_t    *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    long            ifIndex;
    unsigned long   cipIpAddress;


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

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, searchType, &carry)) < 0 ) {
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
     if ( (arg == -1) || (data = k_cipIpEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipIpAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 5;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = (data->cipIpAddress >> 24) & 0xff;
        inst.oid_ptr[2] = (data->cipIpAddress >> 16) & 0xff;
        inst.oid_ptr[3] = (data->cipIpAddress >> 8) & 0xff;
        inst.oid_ptr[4] = (data->cipIpAddress >> 0) & 0xff;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipIpForwarding
    case I_cipIpForwarding:
       dp = (void *) (&data->cipIpForwarding);
       break;
#endif /* I_cipIpForwarding */

#ifdef I_cipIpDefaultTTL
    case I_cipIpDefaultTTL:
       dp = (void *) (&data->cipIpDefaultTTL);
       break;
#endif /* I_cipIpDefaultTTL */

#ifdef I_cipIpInReceives
    case I_cipIpInReceives:
       dp = (void *) (&data->cipIpInReceives);
       break;
#endif /* I_cipIpInReceives */

#ifdef I_cipIpInHdrErrors
    case I_cipIpInHdrErrors:
       dp = (void *) (&data->cipIpInHdrErrors);
       break;
#endif /* I_cipIpInHdrErrors */

#ifdef I_cipIpInAddrErrors
    case I_cipIpInAddrErrors:
       dp = (void *) (&data->cipIpInAddrErrors);
       break;
#endif /* I_cipIpInAddrErrors */

#ifdef I_cipIpForwDatagrams
    case I_cipIpForwDatagrams:
       dp = (void *) (&data->cipIpForwDatagrams);
       break;
#endif /* I_cipIpForwDatagrams */

#ifdef I_cipIpInUnknownProtos
    case I_cipIpInUnknownProtos:
       dp = (void *) (&data->cipIpInUnknownProtos);
       break;
#endif /* I_cipIpInUnknownProtos */

#ifdef I_cipIpInDiscards
    case I_cipIpInDiscards:
       dp = (void *) (&data->cipIpInDiscards);
       break;
#endif /* I_cipIpInDiscards */

#ifdef I_cipIpInDelivers
    case I_cipIpInDelivers:
       dp = (void *) (&data->cipIpInDelivers);
       break;
#endif /* I_cipIpInDelivers */

#ifdef I_cipIpOutRequests
    case I_cipIpOutRequests:
       dp = (void *) (&data->cipIpOutRequests);
       break;
#endif /* I_cipIpOutRequests */

#ifdef I_cipIpOutDiscards
    case I_cipIpOutDiscards:
       dp = (void *) (&data->cipIpOutDiscards);
       break;
#endif /* I_cipIpOutDiscards */

#ifdef I_cipIpOutNoRoutes
    case I_cipIpOutNoRoutes:
       dp = (void *) (&data->cipIpOutNoRoutes);
       break;
#endif /* I_cipIpOutNoRoutes */

#ifdef I_cipIpReasmTimeout
    case I_cipIpReasmTimeout:
       dp = (void *) (&data->cipIpReasmTimeout);
       break;
#endif /* I_cipIpReasmTimeout */

#ifdef I_cipIpReasmReqds
    case I_cipIpReasmReqds:
       dp = (void *) (&data->cipIpReasmReqds);
       break;
#endif /* I_cipIpReasmReqds */

#ifdef I_cipIpReasmOKs
    case I_cipIpReasmOKs:
       dp = (void *) (&data->cipIpReasmOKs);
       break;
#endif /* I_cipIpReasmOKs */

#ifdef I_cipIpReasmFails
    case I_cipIpReasmFails:
       dp = (void *) (&data->cipIpReasmFails);
       break;
#endif /* I_cipIpReasmFails */

#ifdef I_cipIpFragOKs
    case I_cipIpFragOKs:
       dp = (void *) (&data->cipIpFragOKs);
       break;
#endif /* I_cipIpFragOKs */

#ifdef I_cipIpFragFails
    case I_cipIpFragFails:
       dp = (void *) (&data->cipIpFragFails);
       break;
#endif /* I_cipIpFragFails */

#ifdef I_cipIpFragCreates
    case I_cipIpFragCreates:
       dp = (void *) (&data->cipIpFragCreates);
       break;
#endif /* I_cipIpFragCreates */

#ifdef I_cipIpRoutingDiscards
    case I_cipIpRoutingDiscards:
       dp = (void *) (&data->cipIpRoutingDiscards);
       break;
#endif /* I_cipIpRoutingDiscards */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cipIpEntry data object.
 *---------------------------------------------------------------------*/
void cipIpEntry_free(cipIpEntry_t    *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cipIpEntry set/undo
 *---------------------------------------------------------------------*/
static int cipIpEntry_cleanup(doList_t *trash)
{
   cipIpEntry_free(trash->data);
#ifdef SR_SNMPv2
   cipIpEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cipIpEntry family
 *---------------------------------------------------------------------*/
cipIpEntry_t *Clone_cipIpEntry(cipIpEntry_t *cipIpEntry)
{
    cipIpEntry_t *data;

    if ((data = malloc(sizeof(cipIpEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cipIpEntry), sizeof(cipIpEntry_t));


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
cipIpEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cipIpEntry_t     *cipIpEntry;
    long           ifIndex;
    unsigned long  cipIpAddress;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 5) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    cipIpEntry = k_cipIpEntry_get(-1, contextInfo, -1, EXACT, ifIndex, cipIpAddress);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cipIpEntry_set) &&
            (((cipIpEntry_t *) (dp->data)) != NULL) &&
            (((cipIpEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((cipIpEntry_t *) (dp->data))->cipIpAddress == cipIpAddress) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cipIpEntry_set;
        dp->cleanupMethod = cipIpEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cipIpEntry_UNDO
        dp->undoMethod = cipIpEntry_undo;
#else /* SR_cipIpEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cipIpEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cipIpEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cipIpEntry(cipIpEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cipIpEntry(cipIpEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef cipIpEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(cipIpEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cipIpEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cipIpEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((cipIpEntry_t *) (dp->data))->valid);

                ((cipIpEntry_t *) (dp->data))->cipIpAddress = (cipIpAddress);
                SET_VALID(I_cipIpAddress, ((cipIpEntry_t *) (dp->data))->valid);

                error_status = k_cipIpEntry_set_defaults(dp);
            }
#else /* cipIpEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* cipIpEntry_READ_CREATE */
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cipIpForwarding
   case I_cipIpForwarding:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((cipIpEntry_t *) (dp->data))->cipIpForwarding = value->sl_value;
     break;
#endif /* I_cipIpForwarding */

#ifdef I_cipIpDefaultTTL
   case I_cipIpDefaultTTL:

     if ( ((value->sl_value < 1) || (value->sl_value > 255)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((cipIpEntry_t *) (dp->data))->cipIpDefaultTTL = value->ul_value;
     break;
#endif /* I_cipIpDefaultTTL */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipIpEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cipIpEntry_test */
   error_status = k_cipIpEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cipIpEntry_t *) (dp->data))->valid);
        error_status = k_cipIpEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cipIpEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cipIpEntry_set((cipIpEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cipTcpStackEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipTcpStackEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipTcpStackEntry_t *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    long            ifIndex;
    unsigned long   cipIpAddress;


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

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, searchType, &carry)) < 0 ) {
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
     if ( (arg == -1) || (data = k_cipTcpStackEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipIpAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 5;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = (data->cipIpAddress >> 24) & 0xff;
        inst.oid_ptr[2] = (data->cipIpAddress >> 16) & 0xff;
        inst.oid_ptr[3] = (data->cipIpAddress >> 8) & 0xff;
        inst.oid_ptr[4] = (data->cipIpAddress >> 0) & 0xff;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipTcpRtoAlgorithm
    case I_cipTcpRtoAlgorithm:
       dp = (void *) (&data->cipTcpRtoAlgorithm);
       break;
#endif /* I_cipTcpRtoAlgorithm */

#ifdef I_cipTcpRtoMin
    case I_cipTcpRtoMin:
       dp = (void *) (&data->cipTcpRtoMin);
       break;
#endif /* I_cipTcpRtoMin */

#ifdef I_cipTcpRtoMax
    case I_cipTcpRtoMax:
       dp = (void *) (&data->cipTcpRtoMax);
       break;
#endif /* I_cipTcpRtoMax */

#ifdef I_cipTcpMaxConn
    case I_cipTcpMaxConn:
       dp = (void *) (&data->cipTcpMaxConn);
       break;
#endif /* I_cipTcpMaxConn */

#ifdef I_cipTcpActiveOpens
    case I_cipTcpActiveOpens:
       dp = (void *) (&data->cipTcpActiveOpens);
       break;
#endif /* I_cipTcpActiveOpens */

#ifdef I_cipTcpPassiveOpens
    case I_cipTcpPassiveOpens:
       dp = (void *) (&data->cipTcpPassiveOpens);
       break;
#endif /* I_cipTcpPassiveOpens */

#ifdef I_cipTcpAttemptFails
    case I_cipTcpAttemptFails:
       dp = (void *) (&data->cipTcpAttemptFails);
       break;
#endif /* I_cipTcpAttemptFails */

#ifdef I_cipTcpEstabResets
    case I_cipTcpEstabResets:
       dp = (void *) (&data->cipTcpEstabResets);
       break;
#endif /* I_cipTcpEstabResets */

#ifdef I_cipTcpCurrEstab
    case I_cipTcpCurrEstab:
       dp = (void *) (&data->cipTcpCurrEstab);
       break;
#endif /* I_cipTcpCurrEstab */

#ifdef I_cipTcpInSegs
    case I_cipTcpInSegs:
       dp = (void *) (&data->cipTcpInSegs);
       break;
#endif /* I_cipTcpInSegs */

#ifdef I_cipTcpOutSegs
    case I_cipTcpOutSegs:
       dp = (void *) (&data->cipTcpOutSegs);
       break;
#endif /* I_cipTcpOutSegs */

#ifdef I_cipTcpRetransSegs
    case I_cipTcpRetransSegs:
       dp = (void *) (&data->cipTcpRetransSegs);
       break;
#endif /* I_cipTcpRetransSegs */

#ifdef I_cipTcpInErrs
    case I_cipTcpInErrs:
       dp = (void *) (&data->cipTcpInErrs);
       break;
#endif /* I_cipTcpInErrs */

#ifdef I_cipTcpOutRsts
    case I_cipTcpOutRsts:
       dp = (void *) (&data->cipTcpOutRsts);
       break;
#endif /* I_cipTcpOutRsts */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipTcpConnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipTcpConnEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipTcpConnEntry_t *data = NULL;
    unsigned long   buffer[11];
    OID             inst;
    int             carry;
    long            ifIndex;
    unsigned long   cipIpAddress;
    long            cipTcpConnLocalPort;
    unsigned long   cipTcpConnRemAddress;
    long            cipTcpConnRemPort;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 11 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
        if (instLength != 11) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToUInt(incoming, 10 + object->oid.length, &cipTcpConnRemPort, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cipTcpConnRemPort = MAX(0, cipTcpConnRemPort);
    }

    if ( (InstToIP(incoming, 6 + object->oid.length, &cipTcpConnRemAddress, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToUInt(incoming, 5 + object->oid.length, &cipTcpConnLocalPort, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cipTcpConnLocalPort = MAX(0, cipTcpConnLocalPort);
    }

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, searchType, &carry)) < 0 ) {
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
     if ( (arg == -1) || (data = k_cipTcpConnEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipIpAddress, cipTcpConnLocalPort, cipTcpConnRemAddress, cipTcpConnRemPort)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 11;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = (data->cipIpAddress >> 24) & 0xff;
        inst.oid_ptr[2] = (data->cipIpAddress >> 16) & 0xff;
        inst.oid_ptr[3] = (data->cipIpAddress >> 8) & 0xff;
        inst.oid_ptr[4] = (data->cipIpAddress >> 0) & 0xff;
        inst.oid_ptr[5] = data->cipTcpConnLocalPort;
        inst.oid_ptr[6] = (data->cipTcpConnRemAddress >> 24) & 0xff;
        inst.oid_ptr[7] = (data->cipTcpConnRemAddress >> 16) & 0xff;
        inst.oid_ptr[8] = (data->cipTcpConnRemAddress >> 8) & 0xff;
        inst.oid_ptr[9] = (data->cipTcpConnRemAddress >> 0) & 0xff;
        inst.oid_ptr[10] = data->cipTcpConnRemPort;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipTcpConnState
    case I_cipTcpConnState:
       dp = (void *) (&data->cipTcpConnState);
       break;
#endif /* I_cipTcpConnState */

#ifdef I_cipTcpConnInHCBytes
    case I_cipTcpConnInHCBytes:
       dp = (void *) (CloneUInt64(data->cipTcpConnInHCBytes));
       break;
#endif /* I_cipTcpConnInHCBytes */

#ifdef I_cipTcpConnInBytes
    case I_cipTcpConnInBytes:
       dp = (void *) (&data->cipTcpConnInBytes);
       break;
#endif /* I_cipTcpConnInBytes */

#ifdef I_cipTcpConnOutHCBytes
    case I_cipTcpConnOutHCBytes:
       dp = (void *) (CloneUInt64(data->cipTcpConnOutHCBytes));
       break;
#endif /* I_cipTcpConnOutHCBytes */

#ifdef I_cipTcpConnOutBytes
    case I_cipTcpConnOutBytes:
       dp = (void *) (&data->cipTcpConnOutBytes);
       break;
#endif /* I_cipTcpConnOutBytes */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cipTcpConnEntry data object.
 *---------------------------------------------------------------------*/
void cipTcpConnEntry_free(cipTcpConnEntry_t *data)
{
   if (data != NULL) {
       FreeUInt64(data->cipTcpConnInHCBytes);
       FreeUInt64(data->cipTcpConnOutHCBytes);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cipTcpConnEntry set/undo
 *---------------------------------------------------------------------*/
static int cipTcpConnEntry_cleanup(doList_t *trash)
{
   cipTcpConnEntry_free(trash->data);
#ifdef SR_SNMPv2
   cipTcpConnEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cipTcpConnEntry family
 *---------------------------------------------------------------------*/
cipTcpConnEntry_t *Clone_cipTcpConnEntry(cipTcpConnEntry_t *cipTcpConnEntry)
{
    cipTcpConnEntry_t *data;

    if ((data = malloc(sizeof(cipTcpConnEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cipTcpConnEntry), sizeof(cipTcpConnEntry_t));

    data->cipTcpConnInHCBytes = CloneUInt64(cipTcpConnEntry->cipTcpConnInHCBytes);
    data->cipTcpConnOutHCBytes = CloneUInt64(cipTcpConnEntry->cipTcpConnOutHCBytes);

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
cipTcpConnEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cipTcpConnEntry_t     *cipTcpConnEntry;
    long           ifIndex;
    unsigned long  cipIpAddress;
    long           cipTcpConnLocalPort;
    unsigned long  cipTcpConnRemAddress;
    long           cipTcpConnRemPort;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 11) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToUInt(incoming, 5 + object->oid.length, &cipTcpConnLocalPort, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToIP(incoming, 6 + object->oid.length, &cipTcpConnRemAddress, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToUInt(incoming, 10 + object->oid.length, &cipTcpConnRemPort, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    cipTcpConnEntry = k_cipTcpConnEntry_get(-1, contextInfo, -1, EXACT, ifIndex, cipIpAddress, cipTcpConnLocalPort, cipTcpConnRemAddress, cipTcpConnRemPort);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cipTcpConnEntry_set) &&
            (((cipTcpConnEntry_t *) (dp->data)) != NULL) &&
            (((cipTcpConnEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((cipTcpConnEntry_t *) (dp->data))->cipIpAddress == cipIpAddress) &&
            (((cipTcpConnEntry_t *) (dp->data))->cipTcpConnLocalPort == cipTcpConnLocalPort) &&
            (((cipTcpConnEntry_t *) (dp->data))->cipTcpConnRemAddress == cipTcpConnRemAddress) &&
            (((cipTcpConnEntry_t *) (dp->data))->cipTcpConnRemPort == cipTcpConnRemPort) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cipTcpConnEntry_set;
        dp->cleanupMethod = cipTcpConnEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cipTcpConnEntry_UNDO
        dp->undoMethod = cipTcpConnEntry_undo;
#else /* SR_cipTcpConnEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cipTcpConnEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cipTcpConnEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cipTcpConnEntry(cipTcpConnEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cipTcpConnEntry(cipTcpConnEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef cipTcpConnEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(cipTcpConnEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cipTcpConnEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cipTcpConnEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((cipTcpConnEntry_t *) (dp->data))->valid);

                ((cipTcpConnEntry_t *) (dp->data))->cipIpAddress = (cipIpAddress);
                SET_VALID(I_cipIpAddress, ((cipTcpConnEntry_t *) (dp->data))->valid);

                ((cipTcpConnEntry_t *) (dp->data))->cipTcpConnLocalPort = (cipTcpConnLocalPort);
                SET_VALID(I_cipTcpConnLocalPort, ((cipTcpConnEntry_t *) (dp->data))->valid);

                ((cipTcpConnEntry_t *) (dp->data))->cipTcpConnRemAddress = (cipTcpConnRemAddress);
                SET_VALID(I_cipTcpConnRemAddress, ((cipTcpConnEntry_t *) (dp->data))->valid);

                ((cipTcpConnEntry_t *) (dp->data))->cipTcpConnRemPort = (cipTcpConnRemPort);
                SET_VALID(I_cipTcpConnRemPort, ((cipTcpConnEntry_t *) (dp->data))->valid);

                error_status = k_cipTcpConnEntry_set_defaults(dp);
            }
#else /* cipTcpConnEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* cipTcpConnEntry_READ_CREATE */
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cipTcpConnState
   case I_cipTcpConnState:

     if ((value->sl_value < 1) || (value->sl_value > 12)) {
         return WRONG_VALUE_ERROR;
     }

     ((cipTcpConnEntry_t *) (dp->data))->cipTcpConnState = value->sl_value;
     break;
#endif /* I_cipTcpConnState */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipTcpConnEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cipTcpConnEntry_test */
   error_status = k_cipTcpConnEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cipTcpConnEntry_t *) (dp->data))->valid);
        error_status = k_cipTcpConnEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cipTcpConnEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cipTcpConnEntry_set((cipTcpConnEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the cipIcmpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipIcmpEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipIcmpEntry_t  *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    long            ifIndex;
    unsigned long   cipIpAddress;


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

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, searchType, &carry)) < 0 ) {
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
     if ( (arg == -1) || (data = k_cipIcmpEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipIpAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 5;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = (data->cipIpAddress >> 24) & 0xff;
        inst.oid_ptr[2] = (data->cipIpAddress >> 16) & 0xff;
        inst.oid_ptr[3] = (data->cipIpAddress >> 8) & 0xff;
        inst.oid_ptr[4] = (data->cipIpAddress >> 0) & 0xff;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipIcmpInMsgs
    case I_cipIcmpInMsgs:
       dp = (void *) (&data->cipIcmpInMsgs);
       break;
#endif /* I_cipIcmpInMsgs */

#ifdef I_cipIcmpInErrors
    case I_cipIcmpInErrors:
       dp = (void *) (&data->cipIcmpInErrors);
       break;
#endif /* I_cipIcmpInErrors */

#ifdef I_cipIcmpInDestUnreachs
    case I_cipIcmpInDestUnreachs:
       dp = (void *) (&data->cipIcmpInDestUnreachs);
       break;
#endif /* I_cipIcmpInDestUnreachs */

#ifdef I_cipIcmpInTimeExcds
    case I_cipIcmpInTimeExcds:
       dp = (void *) (&data->cipIcmpInTimeExcds);
       break;
#endif /* I_cipIcmpInTimeExcds */

#ifdef I_cipIcmpInParmProbs
    case I_cipIcmpInParmProbs:
       dp = (void *) (&data->cipIcmpInParmProbs);
       break;
#endif /* I_cipIcmpInParmProbs */

#ifdef I_cipIcmpInSrcQuenchs
    case I_cipIcmpInSrcQuenchs:
       dp = (void *) (&data->cipIcmpInSrcQuenchs);
       break;
#endif /* I_cipIcmpInSrcQuenchs */

#ifdef I_cipIcmpInRedirects
    case I_cipIcmpInRedirects:
       dp = (void *) (&data->cipIcmpInRedirects);
       break;
#endif /* I_cipIcmpInRedirects */

#ifdef I_cipIcmpInEchos
    case I_cipIcmpInEchos:
       dp = (void *) (&data->cipIcmpInEchos);
       break;
#endif /* I_cipIcmpInEchos */

#ifdef I_cipIcmpInAddrMaskReps
    case I_cipIcmpInAddrMaskReps:
       dp = (void *) (&data->cipIcmpInAddrMaskReps);
       break;
#endif /* I_cipIcmpInAddrMaskReps */

#ifdef I_cipIcmpOutMsgs
    case I_cipIcmpOutMsgs:
       dp = (void *) (&data->cipIcmpOutMsgs);
       break;
#endif /* I_cipIcmpOutMsgs */

#ifdef I_cipIcmpOutErrors
    case I_cipIcmpOutErrors:
       dp = (void *) (&data->cipIcmpOutErrors);
       break;
#endif /* I_cipIcmpOutErrors */

#ifdef I_cipIcmpOutDestUnreachs
    case I_cipIcmpOutDestUnreachs:
       dp = (void *) (&data->cipIcmpOutDestUnreachs);
       break;
#endif /* I_cipIcmpOutDestUnreachs */

#ifdef I_cipIcmpOutEchos
    case I_cipIcmpOutEchos:
       dp = (void *) (&data->cipIcmpOutEchos);
       break;
#endif /* I_cipIcmpOutEchos */

#ifdef I_cipIcmpOutEchoReps
    case I_cipIcmpOutEchoReps:
       dp = (void *) (&data->cipIcmpOutEchoReps);
       break;
#endif /* I_cipIcmpOutEchoReps */

#ifdef I_cipIcmpOutTimestamps
    case I_cipIcmpOutTimestamps:
       dp = (void *) (&data->cipIcmpOutTimestamps);
       break;
#endif /* I_cipIcmpOutTimestamps */

#ifdef I_cipIcmpOutTimestampReps
    case I_cipIcmpOutTimestampReps:
       dp = (void *) (&data->cipIcmpOutTimestampReps);
       break;
#endif /* I_cipIcmpOutTimestampReps */

#ifdef I_cipIcmpOutAddrMasks
    case I_cipIcmpOutAddrMasks:
       dp = (void *) (&data->cipIcmpOutAddrMasks);
       break;
#endif /* I_cipIcmpOutAddrMasks */

#ifdef I_cipIcmpOutAddrMaskReps
    case I_cipIcmpOutAddrMaskReps:
       dp = (void *) (&data->cipIcmpOutAddrMaskReps);
       break;
#endif /* I_cipIcmpOutAddrMaskReps */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipUdpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipUdpEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cipUdpEntry_t   *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    long            ifIndex;
    unsigned long   cipIpAddress;


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

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, searchType, &carry)) < 0 ) {
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
     if ( (arg == -1) || (data = k_cipUdpEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipIpAddress)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 5;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = (data->cipIpAddress >> 24) & 0xff;
        inst.oid_ptr[2] = (data->cipIpAddress >> 16) & 0xff;
        inst.oid_ptr[3] = (data->cipIpAddress >> 8) & 0xff;
        inst.oid_ptr[4] = (data->cipIpAddress >> 0) & 0xff;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipUdpInDatagrams
    case I_cipUdpInDatagrams:
       dp = (void *) (&data->cipUdpInDatagrams);
       break;
#endif /* I_cipUdpInDatagrams */

#ifdef I_cipUdpNoPorts
    case I_cipUdpNoPorts:
       dp = (void *) (&data->cipUdpNoPorts);
       break;
#endif /* I_cipUdpNoPorts */

#ifdef I_cipUdpInErrors
    case I_cipUdpInErrors:
       dp = (void *) (&data->cipUdpInErrors);
       break;
#endif /* I_cipUdpInErrors */

#ifdef I_cipUdpOutDatagrams
    case I_cipUdpOutDatagrams:
       dp = (void *) (&data->cipUdpOutDatagrams);
       break;
#endif /* I_cipUdpOutDatagrams */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cipUdpListenersEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipUdpListenersEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    cipUdpListenersEntry_t *data = NULL;
    unsigned long   buffer[6];
    OID             inst;
    int             carry;
    long            ifIndex;
    unsigned long   cipIpAddress;
    long            cipUdpLocalPort;


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

    if ( (InstToUInt(incoming, 5 + object->oid.length, &cipUdpLocalPort, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cipUdpLocalPort = MAX(0, cipUdpLocalPort);
    }

    if ( (InstToIP(incoming, 1 + object->oid.length, &cipIpAddress, searchType, &carry)) < 0 ) {
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
     if ( (arg == -1) || (data = k_cipUdpListenersEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, cipIpAddress, cipUdpLocalPort)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 6;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = (data->cipIpAddress >> 24) & 0xff;
        inst.oid_ptr[2] = (data->cipIpAddress >> 16) & 0xff;
        inst.oid_ptr[3] = (data->cipIpAddress >> 8) & 0xff;
        inst.oid_ptr[4] = (data->cipIpAddress >> 0) & 0xff;
        inst.oid_ptr[5] = data->cipUdpLocalPort;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cipUdpLocalPort
    case I_cipUdpLocalPort:
       dp = (void *) (&data->cipUdpLocalPort);
       break;
#endif /* I_cipUdpLocalPort */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*
 * Start of system-dependent routines
 */

cipIpEntry_t *
k_cipIpEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex, cipIpAddress)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   unsigned long cipIpAddress;
{
   static cipIpEntry_t  cipIpData;
   static int           lastSerialNum = MAXINT;
   static long          lastIfIndex = MAXLONG;
   static unsigned long lastCipIpAddress = MAXULONG;

   cip_ip_t cip_ip;
   cipIpEntry_t *result_p = NULL;
   hwidbtype *hwidb;
   boolean exactSearch;

   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipIpAddress == cipIpAddress)) {
      result_p = &cipIpData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      lastCipIpAddress = cipIpAddress;
      lastIfIndex = ifIndex;
      exactSearch = searchType == EXACT;
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         if (get_cip_ip( exactSearch, hwidb, lastCipIpAddress, &cip_ip)) {
            /*
             *  Found instance, copy data to cached structure and exit
             *  FOR_ALL_HWIDBS loop with result_p pointing to cached 
             *  data structure.
             */
            cipIpData.cipIpForwarding      = cip_ip.cipIpForwarding;
            cipIpData.cipIpDefaultTTL      = cip_ip.cipIpDefaultTTL;
            cipIpData.cipIpInReceives      = cip_ip.cipIpInReceives;
            cipIpData.cipIpInHdrErrors     = cip_ip.cipIpInHdrErrors;
            cipIpData.cipIpInAddrErrors    = cip_ip.cipIpInAddrErrors;
            cipIpData.cipIpForwDatagrams   = cip_ip.cipIpForwDatagrams;
            cipIpData.cipIpInUnknownProtos = cip_ip.cipIpInUnknownProtos;
            cipIpData.cipIpInDiscards      = cip_ip.cipIpInDiscards;
            cipIpData.cipIpInDelivers      = cip_ip.cipIpInDelivers;
            cipIpData.cipIpOutRequests     = cip_ip.cipIpOutRequests;
            cipIpData.cipIpOutDiscards     = cip_ip.cipIpOutDiscards;
            cipIpData.cipIpOutNoRoutes     = cip_ip.cipIpOutNoRoutes;
            cipIpData.cipIpReasmTimeout    = cip_ip.cipIpReasmTimeout;
            cipIpData.cipIpReasmReqds      = cip_ip.cipIpReasmReqds;
            cipIpData.cipIpReasmOKs        = cip_ip.cipIpReasmOKs;
            cipIpData.cipIpReasmFails      = cip_ip.cipIpReasmFails;
            cipIpData.cipIpFragOKs         = cip_ip.cipIpFragOKs;
            cipIpData.cipIpFragFails       = cip_ip.cipIpFragFails;
            cipIpData.cipIpFragCreates     = cip_ip.cipIpFragCreates;
            cipIpData.cipIpRoutingDiscards = cip_ip.cipIpRoutingDiscards;
            cipIpData.cipIpAddress         = cip_ip.index.cipIpAddress;
            cipIpData.ifIndex              = hwidb->snmp_if_index;
            lastIfIndex = ifIndex;
            lastCipIpAddress = cipIpAddress;
            lastSerialNum = serialNum;
            SET_ALL_VALID( cipIpData.valid);
            result_p = &cipIpData;
            break;
	 }
         else {
            if (!exactSearch) {
               /* 
                * If not found on this hwidb, go to next hwidb 
                */
               lastCipIpAddress = 0;
               lastIfIndex = hwidb->snmp_if_index + 1;
            }
            else {
               /* 
                * Not found for exact match, then exit FOR_ALL_HWIDBS loop
                */
               break;
            }
         }
      }
   }
   return result_p;
}

#ifdef SETS
int
k_cipIpEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cipIpEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cipIpEntry_set_defaults(doList_t *dp)
{
    return NO_ERROR;
}

int
k_cipIpEntry_set(data, contextInfo, function)
   cipIpEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cipIpEntry_UNDO
/* add #define SR_cipIpEntry_UNDO in ciptcpipmib.h to
 * include the undo routine for the cipIpEntry family.
 */
int
cipIpEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cipIpEntry_UNDO */
#endif /* SR_SNMPv2 */

cipTcpStackEntry_t *
k_cipTcpStackEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex, cipIpAddress)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   unsigned long cipIpAddress;
{
   static cipTcpStackEntry_t cipTcpStackData;
   static int           lastSerialNum = MAXINT;
   static long          lastIfIndex = MAXLONG;
   static unsigned long lastCipIpAddress = MAXULONG;

   cip_tcp_t cip_tcp;
   cipTcpStackEntry_t *result_p = NULL;
   hwidbtype *hwidb;
   boolean exactSearch;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipIpAddress == cipIpAddress)) {
      result_p = &cipTcpStackData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      lastCipIpAddress = cipIpAddress;
      lastIfIndex = ifIndex;
      exactSearch = searchType == EXACT;
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         if (get_cip_tcp( exactSearch, hwidb, lastCipIpAddress, &cip_tcp)) {
            /*
             *  Found instance, copy data to cached structure and exit
             *  FOR_ALL_HWIDBS loop with result_p pointing to cached 
             *  data structure.
             */
            cipTcpStackData.cipTcpRtoAlgorithm = cip_tcp.cipTcpRtoAlgorithm;
            cipTcpStackData.cipTcpRtoMin       = cip_tcp.cipTcpRtoMin;
            cipTcpStackData.cipTcpRtoMax       = cip_tcp.cipTcpRtoMax;
            cipTcpStackData.cipTcpMaxConn      = cip_tcp.cipTcpMaxConn;
            cipTcpStackData.cipTcpActiveOpens  = cip_tcp.cipTcpActiveOpens;
            cipTcpStackData.cipTcpPassiveOpens = cip_tcp.cipTcpPassiveOpens;
            cipTcpStackData.cipTcpAttemptFails = cip_tcp.cipTcpAttemptFails;
            cipTcpStackData.cipTcpEstabResets  = cip_tcp.cipTcpEstabResets;
            cipTcpStackData.cipTcpCurrEstab    = cip_tcp.cipTcpCurrEstab;
            cipTcpStackData.cipTcpInSegs       = cip_tcp.cipTcpInSegs;
            cipTcpStackData.cipTcpOutSegs      = cip_tcp.cipTcpOutSegs;
            cipTcpStackData.cipTcpRetransSegs  = cip_tcp.cipTcpRetransSegs;
            cipTcpStackData.cipTcpInErrs       = cip_tcp.cipTcpInErrs;
            cipTcpStackData.cipTcpOutRsts      = cip_tcp.cipTcpOutRsts;
            cipTcpStackData.cipIpAddress       = cip_tcp.index.cipIpAddress;
            cipTcpStackData.ifIndex            = hwidb->snmp_if_index;
            lastIfIndex = ifIndex;
            lastCipIpAddress = cipIpAddress;
            lastSerialNum = serialNum;
            SET_ALL_VALID( cipTcpStackData.valid);
            result_p = &cipTcpStackData;
            break;
   	 }
         else {
            if (!exactSearch) {
               /* 
                * If not found on this hwidb, go to next hwidb 
                */
               lastCipIpAddress = 0;
               lastIfIndex = hwidb->snmp_if_index + 1;
            }
            else {
               /* 
                * Not found for exact match, then exit FOR_ALL_HWIDBS loop
                */
               break;
            }
	 }
      }
   }
  return result_p;
}

cipTcpConnEntry_t *
k_cipTcpConnEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex, cipIpAddress, cipTcpConnLocalPort, cipTcpConnRemAddress, cipTcpConnRemPort)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   unsigned long cipIpAddress;
   long cipTcpConnLocalPort;
   unsigned long cipTcpConnRemAddress;
   long cipTcpConnRemPort;
{
   static cipTcpConnEntry_t cipTcpConnData;
   static int           lastSerialNum = MAXINT;
   static long          lastIfIndex = MAXLONG;
   static unsigned long lastCipIpAddress = MAXULONG;
   static long          lastCipTcpConnLocalPort  = MAXLONG;
   static unsigned long lastCipTcpConnRemAddress = MAXULONG;
   static long          lastCipTcpConnRemPort    = MAXLONG;
   static UInt64 outBytesHC, inBytesHC;
   
   cip_tcp_conn_t cip_tcp_conn;
   cipTcpConnEntry_t *result_p = NULL;
   hwidbtype *hwidb;
   boolean exactSearch;
   
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipIpAddress == cipIpAddress) && 
       (lastCipTcpConnLocalPort == cipTcpConnLocalPort) &&
       (lastCipTcpConnRemAddress == cipTcpConnRemAddress) &&
       (lastCipTcpConnRemPort == cipTcpConnRemPort)) {
      result_p = &cipTcpConnData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      
      lastCipIpAddress = cipIpAddress;
      lastCipTcpConnLocalPort = cipTcpConnLocalPort;
      lastCipTcpConnRemAddress = cipTcpConnRemAddress;
      lastCipTcpConnRemPort = cipTcpConnRemPort;
      lastIfIndex = ifIndex;
      exactSearch = searchType == EXACT;
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         if (get_cip_tcp_conn( exactSearch, hwidb, lastCipIpAddress, 
                               lastCipTcpConnLocalPort, lastCipTcpConnRemAddress, 
                               lastCipTcpConnRemPort, &cip_tcp_conn)) {
            /*
             *  Found instance, copy data to cached structure and exit
             *  FOR_ALL_HWIDBS loop with result_p pointing to cached 
             *  data structure.
             */
            cipTcpConnData.cipTcpConnState      = cip_tcp_conn.cipTcpConnState;
            outBytesHC.big_end = cip_tcp_conn.cipTcpConnOutHCBytes_msw;
            outBytesHC.little_end = cip_tcp_conn.cipTcpConnOutHCBytes_lsw;
            cipTcpConnData.cipTcpConnOutHCBytes = &outBytesHC;
            cipTcpConnData.cipTcpConnOutBytes   = cip_tcp_conn.cipTcpConnOutHCBytes_lsw;
            inBytesHC.big_end = cip_tcp_conn.cipTcpConnInHCBytes_msw;
            inBytesHC.little_end = cip_tcp_conn.cipTcpConnInHCBytes_lsw;
            cipTcpConnData.cipTcpConnInHCBytes = &inBytesHC;
            cipTcpConnData.cipTcpConnInBytes    = cip_tcp_conn.cipTcpConnInHCBytes_lsw;
            cipTcpConnData.ifIndex              = hwidb->snmp_if_index;
            cipTcpConnData.cipIpAddress         = cip_tcp_conn.index.cipIpAddress;
            cipTcpConnData.cipTcpConnLocalPort  = cip_tcp_conn.index.cipTcpConnLocalPort;
            cipTcpConnData.cipTcpConnRemAddress = cip_tcp_conn.index.cipTcpConnRemAddress;
            cipTcpConnData.cipTcpConnRemPort    = cip_tcp_conn.index.cipTcpConnRemPort;
            lastIfIndex = ifIndex;
            lastSerialNum = serialNum;
            lastCipIpAddress = cipIpAddress;
            lastCipTcpConnLocalPort = cipTcpConnLocalPort;
            lastCipTcpConnRemAddress = cipTcpConnRemAddress;
            lastCipTcpConnRemPort= cipTcpConnRemPort;
            SET_ALL_VALID( cipTcpConnData.valid);
            result_p = &cipTcpConnData;
            break;
	 }
         else {
            if (!exactSearch) {
               /* 
                * If not found on this hwidb, go to next hwidb 
                */
               lastCipIpAddress = 0;
               lastCipTcpConnLocalPort  = 0;
               lastCipTcpConnRemAddress = 0;
               lastCipTcpConnRemPort    = 0;
               lastIfIndex = hwidb->snmp_if_index + 1;
            }
            else {
               /* 
                * Not found for exact match, then exit FOR_ALL_HWIDBS loop
                */
               break;
            }
         }
      }
   }
   return result_p;
}

#ifdef SETS
int
k_cipTcpConnEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cipTcpConnEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int k_cipTcpConnEntry_set_defaults(doList_t       *dp)
{
    return NO_ERROR;
}

int
k_cipTcpConnEntry_set(data, contextInfo, function)
   cipTcpConnEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cipTcpConnEntry_UNDO
/* add #define SR_cipTcpConnEntry_UNDO in ciptcpipmib.h to
 * include the undo routine for the cipTcpConnEntry family.
 */
int
cipTcpConnEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cipTcpConnEntry_UNDO */
#endif /* SR_SNMPv2 */

cipIcmpEntry_t *
k_cipIcmpEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex, cipIpAddress)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   unsigned long cipIpAddress;
{
   static cipIcmpEntry_t cipIcmpData;
   static int           lastSerialNum = MAXINT;
   static long          lastIfIndex = MAXLONG;
   static unsigned long lastCipIpAddress = MAXULONG;

   cip_icmp_t cip_icmp;
   cipIcmpEntry_t *result_p = NULL;
   hwidbtype *hwidb;
   boolean exactSearch;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipIpAddress == cipIpAddress)) {
      result_p = &cipIcmpData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      lastCipIpAddress = cipIpAddress;
      lastIfIndex = ifIndex;
      exactSearch = searchType == EXACT;
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         if (get_cip_icmp( exactSearch, hwidb, lastCipIpAddress, &cip_icmp)) {
            /*
             *  Found instance, copy data to cached structure and exit
             *  FOR_ALL_HWIDBS loop with result_p pointing to cached 
             *  data structure.
             */
            cipIcmpData.cipIcmpInMsgs           = cip_icmp.cipIcmpInMsgs;
            cipIcmpData.cipIcmpInErrors         = cip_icmp.cipIcmpInErrors;
            cipIcmpData.cipIcmpInDestUnreachs   = cip_icmp.cipIcmpInDestUnreachs;
            cipIcmpData.cipIcmpInTimeExcds      = cip_icmp.cipIcmpInTimeExcds;
            cipIcmpData.cipIcmpInParmProbs      = cip_icmp.cipIcmpInParmProbs;
            cipIcmpData.cipIcmpInSrcQuenchs     = cip_icmp.cipIcmpInSrcQuenchs;
            cipIcmpData.cipIcmpInRedirects      = cip_icmp.cipIcmpInRedirects;
            cipIcmpData.cipIcmpInEchos          = cip_icmp.cipIcmpInEchos;
            cipIcmpData.cipIcmpInAddrMaskReps   = cip_icmp.cipIcmpInAddrMaskReps;
            cipIcmpData.cipIcmpOutMsgs          = cip_icmp.cipIcmpOutMsgs;
            cipIcmpData.cipIcmpOutErrors        = cip_icmp.cipIcmpOutErrors;
            cipIcmpData.cipIcmpOutDestUnreachs  = cip_icmp.cipIcmpOutDestUnreachs;
            cipIcmpData.cipIcmpOutEchos         = cip_icmp.cipIcmpOutEchos;
            cipIcmpData.cipIcmpOutEchoReps      = cip_icmp.cipIcmpOutEchoReps;
            cipIcmpData.cipIcmpOutTimestamps    = cip_icmp.cipIcmpOutTimestamps;
            cipIcmpData.cipIcmpOutTimestampReps = cip_icmp.cipIcmpOutTimestampReps;
            cipIcmpData.cipIcmpOutAddrMasks     = cip_icmp.cipIcmpOutAddrMasks;
            cipIcmpData.cipIcmpOutAddrMaskReps  = cip_icmp.cipIcmpOutAddrMaskReps;
            cipIcmpData.cipIpAddress            = cip_icmp.index.cipIpAddress;
            cipIcmpData.ifIndex                 = hwidb->snmp_if_index;
            lastIfIndex = ifIndex;
            lastCipIpAddress = cipIpAddress;
            lastSerialNum = serialNum;
            SET_ALL_VALID( cipIcmpData.valid);
            result_p = &cipIcmpData;
            break;
	 }
         else {
            if (!exactSearch) {
               /* 
                * If not found on this hwidb, go to next hwidb 
                */
               lastIfIndex = hwidb->snmp_if_index + 1;
               lastCipIpAddress = 0;
            }
            else {
               /* 
                * Not found for exact match, then exit FOR_ALL_HWIDBS loop
                */
               break;
            }
         }
      }
   }
  return result_p;
}

cipUdpEntry_t *
k_cipUdpEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex, cipIpAddress)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   unsigned long cipIpAddress;
{
   static cipUdpEntry_t cipUdpData;
   static int lastSerialNum              = MAXINT;
   static long lastIfIndex               = MAXLONG;
   static unsigned long lastCipIpAddress = MAXULONG;
   
   cip_udp_t cip_udp;
   cipUdpEntry_t *result_p = NULL;
   hwidbtype *hwidb;
   boolean exactSearch;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipIpAddress == cipIpAddress)) {
      result_p = &cipUdpData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      lastCipIpAddress = cipIpAddress;
      lastIfIndex = ifIndex;
      exactSearch = searchType == EXACT;
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         if (get_cip_udp( exactSearch, hwidb, lastCipIpAddress, &cip_udp)) {
            /*
             *  Found instance, copy data to cached structure and exit
             *  FOR_ALL_HWIDBS loop with result_p pointing to cached 
             *  data structure.
             */
            cipUdpData.cipUdpInDatagrams  = cip_udp.cipUdpInDatagrams;
            cipUdpData.cipUdpNoPorts      = cip_udp.cipUdpNoPorts;
            cipUdpData.cipUdpInErrors     = cip_udp.cipUdpInErrors;
            cipUdpData.cipUdpOutDatagrams = cip_udp.cipUdpOutDatagrams;
            cipUdpData.cipIpAddress       = cip_udp.index.cipIpAddress;
            cipUdpData.ifIndex            = hwidb->snmp_if_index;
            lastIfIndex = ifIndex;
            lastCipIpAddress = cipIpAddress;
            lastSerialNum = serialNum;
            SET_ALL_VALID( cipUdpData.valid);
            result_p = &cipUdpData;
            break;
         }
         else {
            if (!exactSearch) {
               /* 
                * If not found on this hwidb, go to next hwidb 
                */
               lastIfIndex = hwidb->snmp_if_index + 1;
               lastCipIpAddress = 0;
            }
            else {
               /* 
                * Not found for exact match, then exit FOR_ALL_HWIDBS loop
                */
               break;
            }
         }
      }
   }
  return result_p;
}

cipUdpListenersEntry_t *
k_cipUdpListenersEntry_get(serialNum, contextInfo, nominator, searchType, ifIndex, cipIpAddress, cipUdpLocalPort)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   unsigned long cipIpAddress;
   long          cipUdpLocalPort;
{
   static cipUdpListenersEntry_t cipUdpListenersData;
   static int lastSerialNum                 = MAXINT;
   static long lastIfIndex                  = MAXLONG;
   static unsigned long lastCipIpAddress    = MAXULONG;
   static long          lastCipUdpLocalPort = MAXLONG;
   
   cip_udp_listeners_t cip_udp_listeners;
   cipUdpListenersEntry_t *result_p = NULL;
   hwidbtype *hwidb;
   boolean exactSearch;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) && 
       (lastCipIpAddress == cipIpAddress) &&
       (lastCipUdpLocalPort == cipUdpLocalPort)) {
      result_p = &cipUdpListenersData;
   }
   else {
      /* 
       * Normalize search type for CIP
       * Search all HWIDBS for requested instance
       */
      lastCipIpAddress = cipIpAddress;
      lastCipUdpLocalPort = cipUdpLocalPort;
      lastIfIndex = ifIndex;
      exactSearch = searchType == EXACT;
      while((hwidb = cip_get_hwidb(searchType, lastIfIndex)) != NULL) {
         if (get_cip_udp_listeners( exactSearch, hwidb, lastCipIpAddress, 
                                   lastCipUdpLocalPort, &cip_udp_listeners)) {
            /*
             *  Found instance, copy data to cached structure and exit
             *  FOR_ALL_HWIDBS loop with result_p pointing to cached 
             *  data structure.
             */
            cipUdpListenersData.cipIpAddress    = cip_udp_listeners.index.cipIpAddress;
            cipUdpListenersData.cipUdpLocalPort = cip_udp_listeners.index.cipUdpLocalPort;
            cipUdpListenersData.ifIndex         = hwidb->snmp_if_index;
            lastIfIndex = ifIndex;
            lastCipIpAddress = cipIpAddress;
            lastCipUdpLocalPort = cipUdpLocalPort;
            lastSerialNum = serialNum;
            SET_ALL_VALID( cipUdpListenersData.valid);
            result_p = &cipUdpListenersData;
            break;
	 }
         else {
            if (!exactSearch) {
               /* 
                * If not found on this hwidb, go to next hwidb 
                */
               lastIfIndex = hwidb->snmp_if_index + 1;
               lastCipIpAddress = 0;
               lastCipUdpLocalPort = 0;
            }
            else {
               /* 
                * Not found for exact match, then exit FOR_ALL_HWIDBS loop
                */
               break;
            }
         }
      }
   }
  return result_p;
}


static void init_ciptcpipmib (subsystype *subsys)
{
    load_mib(ciptcpipmib_OidList, ciptcpipmib_OidListNum);
    load_oid(ciptcpipmib_oid_table);
}

/*
 * ciptcpipmib subsystem header
 */
#define MAJVERSION_ciptcpipmib 1
#define MINVERSION_ciptcpipmib 0
#define EDITVERSION_ciptcpipmib 0

SUBSYS_HEADER(ciptcpipmib,
              MAJVERSION_ciptcpipmib,
              MINVERSION_ciptcpipmib,
              EDITVERSION_ciptcpipmib,
              init_ciptcpipmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: cip", NULL);
      
