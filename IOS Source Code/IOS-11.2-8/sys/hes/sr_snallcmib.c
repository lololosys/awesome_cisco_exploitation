/* $Id: sr_snallcmib.c,v 3.7.10.7 1996/07/01 18:44:32 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_snallcmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Michael Otto (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snallcmib.c,v $
 * Revision 3.7.10.7  1996/07/01  18:44:32  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.7.10.6  1996/05/21  09:48:09  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.5  1996/04/04  05:32:45  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.7.10.4  1996/04/04  01:54:13  mordock
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Branch: California_branch
 * more than one source needs access to do_snallc_trap().  callers of this
 * routine need to use new type/value struct for passing in varbind values.
 *
 * Revision 3.7.10.3  1996/04/03  23:27:10  mordock
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Branch: California_branch
 * fixed typo
 *
 * Revision 3.7.10.2  1996/04/03  00:12:56  jjohnson
 * CSCdi53463:  Invalid access to the snmp mib dispatch table
 * Implement a different mechanism for creating trap varbind lists
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/03/18  19:43:34  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.12.3  1996/03/16  06:51:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.12.2  1996/03/07  09:40:47  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.12.1  1996/02/20  00:53:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/22  14:33:21  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1996/02/14  19:37:45  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.5  1996/01/29  07:27:42  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.4  1996/01/24  22:03:05  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.3  1995/12/17  18:27:35  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:20:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:01:18  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/09/13  04:07:07  hampton
 * The Fall of the House of Msclock.  [CSCdi40182]
 *
 * Revision 2.6  1995/08/09  19:29:24  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.5  1995/07/20  13:34:03  motto
 * CSCdi37360:  CIP SNA LLC CC state display incorrect
 *
 * Revision 2.4  1995/07/18  21:46:34  motto
 * Update MIBs to current level of CIP support
 * Cleaned up CSNA show command support
 * CSCdi35714:  Some show commands for CIP-SNA are not working
 *
 * Revision 2.3  1995/06/28  09:25:05  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/27  20:46:31  motto
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
 * Revision 2.1  1995/06/07  20:44:30  hampton
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
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "subsys.h"
#include "cbus_registry.h"
#include "../snmp/snmp_api.h"
#include "sr_snallcmib.h"
#include "sr_snallcmib-mib.h"
#include "interface_private.h"
#include "sysmgt_snallc.h"
#include "channelmib_utils.h"
#include "../dspu/cipllc_types.h"
#include "../snmp/snmp_trap_fe.h"
#include "../parser/parser_defs_snmp.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../snmp/snmp_registry.regh"

static void CopyOctetStrings(OctetString *src, OctetString *dest, int maxlen);


/*
 * Start of system-independent routines
 */


/*---------------------------------------------------------------------
 * Retrieve data from the llcPortAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcPortAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    llcPortAdminEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            llcPortVirtualIndex;


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
     if ( (arg == -1) || (data = k_llcPortAdminEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->llcPortVirtualIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_llcPortAdminName
    case I_llcPortAdminName:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->llcPortAdminName));
       }
       break;
#endif /* I_llcPortAdminName */

#ifdef I_llcPortAdminMaxSaps
    case I_llcPortAdminMaxSaps:
       dp = (void *) (&data->llcPortAdminMaxSaps);
       break;
#endif /* I_llcPortAdminMaxSaps */

#ifdef I_llcPortAdminMaxCcs
    case I_llcPortAdminMaxCcs:
       dp = (void *) (&data->llcPortAdminMaxCcs);
       break;
#endif /* I_llcPortAdminMaxCcs */

#ifdef I_llcPortAdminMaxPDUOctets
    case I_llcPortAdminMaxPDUOctets:
       dp = (void *) (&data->llcPortAdminMaxPDUOctets);
       break;
#endif /* I_llcPortAdminMaxPDUOctets */

#ifdef I_llcPortAdminMaxUnackedIPDUsSend
    case I_llcPortAdminMaxUnackedIPDUsSend:
       dp = (void *) (&data->llcPortAdminMaxUnackedIPDUsSend);
       break;
#endif /* I_llcPortAdminMaxUnackedIPDUsSend */

#ifdef I_llcPortAdminMaxUnackedIPDUsRcv
    case I_llcPortAdminMaxUnackedIPDUsRcv:
       dp = (void *) (&data->llcPortAdminMaxUnackedIPDUsRcv);
       break;
#endif /* I_llcPortAdminMaxUnackedIPDUsRcv */

#ifdef I_llcPortAdminMaxRetransmits
    case I_llcPortAdminMaxRetransmits:
       dp = (void *) (&data->llcPortAdminMaxRetransmits);
       break;
#endif /* I_llcPortAdminMaxRetransmits */

#ifdef I_llcPortAdminAckTimer
    case I_llcPortAdminAckTimer:
       dp = (void *) (&data->llcPortAdminAckTimer);
       break;
#endif /* I_llcPortAdminAckTimer */

#ifdef I_llcPortAdminPbitTimer
    case I_llcPortAdminPbitTimer:
       dp = (void *) (&data->llcPortAdminPbitTimer);
       break;
#endif /* I_llcPortAdminPbitTimer */

#ifdef I_llcPortAdminRejTimer
    case I_llcPortAdminRejTimer:
       dp = (void *) (&data->llcPortAdminRejTimer);
       break;
#endif /* I_llcPortAdminRejTimer */

#ifdef I_llcPortAdminBusyTimer
    case I_llcPortAdminBusyTimer:
       dp = (void *) (&data->llcPortAdminBusyTimer);
       break;
#endif /* I_llcPortAdminBusyTimer */

#ifdef I_llcPortAdminInactTimer
    case I_llcPortAdminInactTimer:
       dp = (void *) (&data->llcPortAdminInactTimer);
       break;
#endif /* I_llcPortAdminInactTimer */

#ifdef I_llcPortAdminDelayAckCount
    case I_llcPortAdminDelayAckCount:
       dp = (void *) (&data->llcPortAdminDelayAckCount);
       break;
#endif /* I_llcPortAdminDelayAckCount */

#ifdef I_llcPortAdminDelayAckTimer
    case I_llcPortAdminDelayAckTimer:
       dp = (void *) (&data->llcPortAdminDelayAckTimer);
       break;
#endif /* I_llcPortAdminDelayAckTimer */

#ifdef I_llcPortAdminNw
    case I_llcPortAdminNw:
       dp = (void *) (&data->llcPortAdminNw);
       break;
#endif /* I_llcPortAdminNw */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the llcPortAdminEntry data object.
 *---------------------------------------------------------------------*/
void
llcPortAdminEntry_free(llcPortAdminEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->llcPortAdminName);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after llcPortAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
llcPortAdminEntry_cleanup(doList_t *trash)
{
   llcPortAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
   llcPortAdminEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the llcPortAdminEntry family
 *---------------------------------------------------------------------*/
llcPortAdminEntry_t *
Clone_llcPortAdminEntry(llcPortAdminEntry_t *llcPortAdminEntry)
{
    llcPortAdminEntry_t *data;

    if ((data = malloc(sizeof(llcPortAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (llcPortAdminEntry), sizeof(llcPortAdminEntry_t));

    data->llcPortAdminName = CloneOctetString(llcPortAdminEntry->llcPortAdminName);

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
llcPortAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    llcPortAdminEntry_t     *llcPortAdminEntry;
    long           ifIndex;
    long           llcPortVirtualIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 2) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &llcPortVirtualIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    llcPortAdminEntry = k_llcPortAdminEntry_get(-1, contextInfo, -1, EXACT, ifIndex, llcPortVirtualIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == llcPortAdminEntry_set) &&
            (((llcPortAdminEntry_t *) (dp->data)) != NULL) &&
            (((llcPortAdminEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((llcPortAdminEntry_t *) (dp->data))->llcPortVirtualIndex == llcPortVirtualIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = llcPortAdminEntry_set;
        dp->cleanupMethod = llcPortAdminEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_llcPortAdminEntry_UNDO
        dp->undoMethod = llcPortAdminEntry_undo;
#else /* SR_llcPortAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_llcPortAdminEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (llcPortAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_llcPortAdminEntry(llcPortAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_llcPortAdminEntry(llcPortAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef llcPortAdminEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(llcPortAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(llcPortAdminEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((llcPortAdminEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((llcPortAdminEntry_t *) (dp->data))->valid);

                ((llcPortAdminEntry_t *) (dp->data))->llcPortVirtualIndex = (llcPortVirtualIndex);
                SET_VALID(I_llcPortVirtualIndex, ((llcPortAdminEntry_t *) (dp->data))->valid);

                error_status = k_llcPortAdminEntry_set_defaults(dp);
            }
#else /* llcPortAdminEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* llcPortAdminEntry_READ_CREATE */
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_llcPortAdminName
   case I_llcPortAdminName:

     if (((llcPortAdminEntry_t *) (dp->data))->llcPortAdminName != NULL) {
        FreeOctetString(((llcPortAdminEntry_t *) (dp->data))->llcPortAdminName);
     }

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_llcPortAdminName */

#ifdef I_llcPortAdminMaxSaps
   case I_llcPortAdminMaxSaps:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminMaxSaps = value->ul_value;
     break;
#endif /* I_llcPortAdminMaxSaps */

#ifdef I_llcPortAdminMaxCcs
   case I_llcPortAdminMaxCcs:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminMaxCcs = value->ul_value;
     break;
#endif /* I_llcPortAdminMaxCcs */

#ifdef I_llcPortAdminMaxPDUOctets
   case I_llcPortAdminMaxPDUOctets:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminMaxPDUOctets = value->sl_value;
     break;
#endif /* I_llcPortAdminMaxPDUOctets */

#ifdef I_llcPortAdminMaxUnackedIPDUsSend
   case I_llcPortAdminMaxUnackedIPDUsSend:

     if ( ((value->sl_value < 1) || (value->sl_value > 127)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminMaxUnackedIPDUsSend = value->sl_value;
     break;
#endif /* I_llcPortAdminMaxUnackedIPDUsSend */

#ifdef I_llcPortAdminMaxUnackedIPDUsRcv
   case I_llcPortAdminMaxUnackedIPDUsRcv:

     if ( ((value->sl_value < 1) || (value->sl_value > 127)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminMaxUnackedIPDUsRcv = value->sl_value;
     break;
#endif /* I_llcPortAdminMaxUnackedIPDUsRcv */

#ifdef I_llcPortAdminMaxRetransmits
   case I_llcPortAdminMaxRetransmits:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminMaxRetransmits = value->sl_value;
     break;
#endif /* I_llcPortAdminMaxRetransmits */

#ifdef I_llcPortAdminAckTimer
   case I_llcPortAdminAckTimer:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminAckTimer = value->ul_value;
     break;
#endif /* I_llcPortAdminAckTimer */

#ifdef I_llcPortAdminPbitTimer
   case I_llcPortAdminPbitTimer:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminPbitTimer = value->ul_value;
     break;
#endif /* I_llcPortAdminPbitTimer */

#ifdef I_llcPortAdminRejTimer
   case I_llcPortAdminRejTimer:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminRejTimer = value->ul_value;
     break;
#endif /* I_llcPortAdminRejTimer */

#ifdef I_llcPortAdminBusyTimer
   case I_llcPortAdminBusyTimer:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminBusyTimer = value->ul_value;
     break;
#endif /* I_llcPortAdminBusyTimer */

#ifdef I_llcPortAdminInactTimer
   case I_llcPortAdminInactTimer:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminInactTimer = value->ul_value;
     break;
#endif /* I_llcPortAdminInactTimer */

#ifdef I_llcPortAdminDelayAckCount
   case I_llcPortAdminDelayAckCount:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminDelayAckCount = value->sl_value;
     break;
#endif /* I_llcPortAdminDelayAckCount */

#ifdef I_llcPortAdminDelayAckTimer
   case I_llcPortAdminDelayAckTimer:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminDelayAckTimer = value->ul_value;
     break;
#endif /* I_llcPortAdminDelayAckTimer */

#ifdef I_llcPortAdminNw
   case I_llcPortAdminNw:

     ((llcPortAdminEntry_t *) (dp->data))->llcPortAdminNw = value->sl_value;
     break;
#endif /* I_llcPortAdminNw */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in llcPortAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_llcPortAdminEntry_test */
   error_status = k_llcPortAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((llcPortAdminEntry_t *) (dp->data))->valid);
        error_status = k_llcPortAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
llcPortAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_llcPortAdminEntry_set((llcPortAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the llcPortOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcPortOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    llcPortOperEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            llcPortVirtualIndex;


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
     if ( (arg == -1) || (data = k_llcPortOperEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->llcPortVirtualIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_llcPortOperMacAddress
    case I_llcPortOperMacAddress:
       if (VALID( arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->llcPortOperMacAddress));
       }
       break;
#endif /* I_llcPortOperMacAddress */

#ifdef I_llcPortOperNumSaps
    case I_llcPortOperNumSaps:
       dp = (void *) (&data->llcPortOperNumSaps);
       break;
#endif /* I_llcPortOperNumSaps */

#ifdef I_llcPortOperHiWaterNumSaps
    case I_llcPortOperHiWaterNumSaps:
       dp = (void *) (&data->llcPortOperHiWaterNumSaps);
       break;
#endif /* I_llcPortOperHiWaterNumSaps */

#ifdef I_llcPortOperSimRim
    case I_llcPortOperSimRim:
       dp = (void *) (&data->llcPortOperSimRim);
       break;
#endif /* I_llcPortOperSimRim */

#ifdef I_llcPortOperLastModifyTime
    case I_llcPortOperLastModifyTime:
       dp = (void *) (&data->llcPortOperLastModifyTime);
       break;
#endif /* I_llcPortOperLastModifyTime */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the llcPortStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcPortStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    llcPortStatsEntry_t *data = NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            llcPortVirtualIndex;


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
     if ( (arg == -1) || (data = k_llcPortStatsEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->llcPortVirtualIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_llcPortStatsPDUsIn
    case I_llcPortStatsPDUsIn:
       dp = (void *) (&data->llcPortStatsPDUsIn);
       break;
#endif /* I_llcPortStatsPDUsIn */

#ifdef I_llcPortStatsPDUsOut
    case I_llcPortStatsPDUsOut:
       dp = (void *) (&data->llcPortStatsPDUsOut);
       break;
#endif /* I_llcPortStatsPDUsOut */

#ifdef I_llcPortStatsOctetsIn
    case I_llcPortStatsOctetsIn:
       dp = (void *) (&data->llcPortStatsOctetsIn);
       break;
#endif /* I_llcPortStatsOctetsIn */

#ifdef I_llcPortStatsOctetsOut
    case I_llcPortStatsOctetsOut:
       dp = (void *) (&data->llcPortStatsOctetsOut);
       break;
#endif /* I_llcPortStatsOctetsOut */

#ifdef I_llcPortStatsTESTCommandsIn
    case I_llcPortStatsTESTCommandsIn:
       dp = (void *) (&data->llcPortStatsTESTCommandsIn);
       break;
#endif /* I_llcPortStatsTESTCommandsIn */

#ifdef I_llcPortStatsTESTResponsesOut
    case I_llcPortStatsTESTResponsesOut:
       dp = (void *) (&data->llcPortStatsTESTResponsesOut);
       break;
#endif /* I_llcPortStatsTESTResponsesOut */

#ifdef I_llcPortStatsLocalBusies
    case I_llcPortStatsLocalBusies:
       dp = (void *) (&data->llcPortStatsLocalBusies);
       break;
#endif /* I_llcPortStatsLocalBusies */

#ifdef I_llcPortStatsUnknownSaps
    case I_llcPortStatsUnknownSaps:
       dp = (void *) (&data->llcPortStatsUnknownSaps);
       break;
#endif /* I_llcPortStatsUnknownSaps */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the llcSapAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcSapAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    llcSapAdminEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            llcPortVirtualIndex;
    long            llcSapNumber;


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
     if ( (arg == -1) || (data = k_llcSapAdminEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)) == NULL) {
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

#ifdef I_llcSapAdminMaxPDUOctets
    case I_llcSapAdminMaxPDUOctets:
       dp = (void *) (&data->llcSapAdminMaxPDUOctets);
       break;
#endif /* I_llcSapAdminMaxPDUOctets */

#ifdef I_llcSapAdminMaxUnackedIPDUsSend
    case I_llcSapAdminMaxUnackedIPDUsSend:
       dp = (void *) (&data->llcSapAdminMaxUnackedIPDUsSend);
       break;
#endif /* I_llcSapAdminMaxUnackedIPDUsSend */

#ifdef I_llcSapAdminMaxUnackedIPDUsRcv
    case I_llcSapAdminMaxUnackedIPDUsRcv:
       dp = (void *) (&data->llcSapAdminMaxUnackedIPDUsRcv);
       break;
#endif /* I_llcSapAdminMaxUnackedIPDUsRcv */

#ifdef I_llcSapAdminMaxRetransmits
    case I_llcSapAdminMaxRetransmits:
       dp = (void *) (&data->llcSapAdminMaxRetransmits);
       break;
#endif /* I_llcSapAdminMaxRetransmits */

#ifdef I_llcSapAdminAckTimer
    case I_llcSapAdminAckTimer:
       dp = (void *) (&data->llcSapAdminAckTimer);
       break;
#endif /* I_llcSapAdminAckTimer */

#ifdef I_llcSapAdminPbitTimer
    case I_llcSapAdminPbitTimer:
       dp = (void *) (&data->llcSapAdminPbitTimer);
       break;
#endif /* I_llcSapAdminPbitTimer */

#ifdef I_llcSapAdminRejTimer
    case I_llcSapAdminRejTimer:
       dp = (void *) (&data->llcSapAdminRejTimer);
       break;
#endif /* I_llcSapAdminRejTimer */

#ifdef I_llcSapAdminBusyTimer
    case I_llcSapAdminBusyTimer:
       dp = (void *) (&data->llcSapAdminBusyTimer);
       break;
#endif /* I_llcSapAdminBusyTimer */

#ifdef I_llcSapAdminInactTimer
    case I_llcSapAdminInactTimer:
       dp = (void *) (&data->llcSapAdminInactTimer);
       break;
#endif /* I_llcSapAdminInactTimer */

#ifdef I_llcSapAdminDelayAckCount
    case I_llcSapAdminDelayAckCount:
       dp = (void *) (&data->llcSapAdminDelayAckCount);
       break;
#endif /* I_llcSapAdminDelayAckCount */

#ifdef I_llcSapAdminDelayAckTimer
    case I_llcSapAdminDelayAckTimer:
       dp = (void *) (&data->llcSapAdminDelayAckTimer);
       break;
#endif /* I_llcSapAdminDelayAckTimer */

#ifdef I_llcSapAdminNw
    case I_llcSapAdminNw:
       dp = (void *) (&data->llcSapAdminNw);
       break;
#endif /* I_llcSapAdminNw */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the llcSapAdminEntry data object.
 *---------------------------------------------------------------------*/
void
llcSapAdminEntry_free(llcSapAdminEntry_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after llcSapAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
llcSapAdminEntry_cleanup(doList_t *trash)
{
   llcSapAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
   llcSapAdminEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the llcSapAdminEntry family
 *---------------------------------------------------------------------*/
llcSapAdminEntry_t *
Clone_llcSapAdminEntry(llcSapAdminEntry_t *llcSapAdminEntry)
{
    llcSapAdminEntry_t *data;

    if ((data = malloc(sizeof(llcSapAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (llcSapAdminEntry), sizeof(llcSapAdminEntry_t));


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
llcSapAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    llcSapAdminEntry_t     *llcSapAdminEntry;
    long           ifIndex;
    long           llcPortVirtualIndex;
    long           llcSapNumber;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &llcPortVirtualIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &llcSapNumber, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    llcSapAdminEntry = k_llcSapAdminEntry_get(-1, contextInfo, -1, EXACT, ifIndex, llcPortVirtualIndex, llcSapNumber);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == llcSapAdminEntry_set) &&
            (((llcSapAdminEntry_t *) (dp->data)) != NULL) &&
            (((llcSapAdminEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((llcSapAdminEntry_t *) (dp->data))->llcPortVirtualIndex == llcPortVirtualIndex) &&
            (((llcSapAdminEntry_t *) (dp->data))->llcSapNumber == llcSapNumber) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = llcSapAdminEntry_set;
        dp->cleanupMethod = llcSapAdminEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_llcSapAdminEntry_UNDO
        dp->undoMethod = llcSapAdminEntry_undo;
#else /* SR_llcSapAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_llcSapAdminEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (llcSapAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_llcSapAdminEntry(llcSapAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_llcSapAdminEntry(llcSapAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef llcSapAdminEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(llcSapAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(llcSapAdminEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((llcSapAdminEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((llcSapAdminEntry_t *) (dp->data))->valid);

                ((llcSapAdminEntry_t *) (dp->data))->llcPortVirtualIndex = (llcPortVirtualIndex);
                SET_VALID(I_llcPortVirtualIndex, ((llcSapAdminEntry_t *) (dp->data))->valid);

                ((llcSapAdminEntry_t *) (dp->data))->llcSapNumber = (llcSapNumber);
                SET_VALID(I_llcSapNumber, ((llcSapAdminEntry_t *) (dp->data))->valid);

                error_status = k_llcSapAdminEntry_set_defaults(dp);
            }
#else /* llcSapAdminEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* llcSapAdminEntry_READ_CREATE */
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_llcSapAdminMaxPDUOctets
   case I_llcSapAdminMaxPDUOctets:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminMaxPDUOctets = value->sl_value;
     break;
#endif /* I_llcSapAdminMaxPDUOctets */

#ifdef I_llcSapAdminMaxUnackedIPDUsSend
   case I_llcSapAdminMaxUnackedIPDUsSend:

     if ( ((value->sl_value < 0) || (value->sl_value > 127)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminMaxUnackedIPDUsSend = value->sl_value;
     break;
#endif /* I_llcSapAdminMaxUnackedIPDUsSend */

#ifdef I_llcSapAdminMaxUnackedIPDUsRcv
   case I_llcSapAdminMaxUnackedIPDUsRcv:

     if ( ((value->sl_value < 0) || (value->sl_value > 127)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminMaxUnackedIPDUsRcv = value->sl_value;
     break;
#endif /* I_llcSapAdminMaxUnackedIPDUsRcv */

#ifdef I_llcSapAdminMaxRetransmits
   case I_llcSapAdminMaxRetransmits:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminMaxRetransmits = value->sl_value;
     break;
#endif /* I_llcSapAdminMaxRetransmits */

#ifdef I_llcSapAdminAckTimer
   case I_llcSapAdminAckTimer:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminAckTimer = value->ul_value;
     break;
#endif /* I_llcSapAdminAckTimer */

#ifdef I_llcSapAdminPbitTimer
   case I_llcSapAdminPbitTimer:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminPbitTimer = value->ul_value;
     break;
#endif /* I_llcSapAdminPbitTimer */

#ifdef I_llcSapAdminRejTimer
   case I_llcSapAdminRejTimer:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminRejTimer = value->ul_value;
     break;
#endif /* I_llcSapAdminRejTimer */

#ifdef I_llcSapAdminBusyTimer
   case I_llcSapAdminBusyTimer:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminBusyTimer = value->ul_value;
     break;
#endif /* I_llcSapAdminBusyTimer */

#ifdef I_llcSapAdminInactTimer
   case I_llcSapAdminInactTimer:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminInactTimer = value->ul_value;
     break;
#endif /* I_llcSapAdminInactTimer */

#ifdef I_llcSapAdminDelayAckCount
   case I_llcSapAdminDelayAckCount:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminDelayAckCount = value->sl_value;
     break;
#endif /* I_llcSapAdminDelayAckCount */

#ifdef I_llcSapAdminDelayAckTimer
   case I_llcSapAdminDelayAckTimer:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminDelayAckTimer = value->ul_value;
     break;
#endif /* I_llcSapAdminDelayAckTimer */

#ifdef I_llcSapAdminNw
   case I_llcSapAdminNw:

     ((llcSapAdminEntry_t *) (dp->data))->llcSapAdminNw = value->sl_value;
     break;
#endif /* I_llcSapAdminNw */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in llcSapAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_llcSapAdminEntry_test */
   error_status = k_llcSapAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((llcSapAdminEntry_t *) (dp->data))->valid);
        error_status = k_llcSapAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
llcSapAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_llcSapAdminEntry_set((llcSapAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the llcSapOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcSapOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    llcSapOperEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            llcPortVirtualIndex;
    long            llcSapNumber;


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
     if ( (arg == -1) || (data = k_llcSapOperEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)) == NULL) {
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

#ifdef I_llcSapOperStatus
    case I_llcSapOperStatus:
       dp = (void *) (&data->llcSapOperStatus);
       break;
#endif /* I_llcSapOperStatus */

#ifdef I_llcSapOperNumCcs
    case I_llcSapOperNumCcs:
       dp = (void *) (&data->llcSapOperNumCcs);
       break;
#endif /* I_llcSapOperNumCcs */

#ifdef I_llcSapOperHiWaterNumCcs
    case I_llcSapOperHiWaterNumCcs:
       dp = (void *) (&data->llcSapOperHiWaterNumCcs);
       break;
#endif /* I_llcSapOperHiWaterNumCcs */

#ifdef I_llcSapOperLlc2Support
    case I_llcSapOperLlc2Support:
       dp = (void *) (&data->llcSapOperLlc2Support);
       break;
#endif /* I_llcSapOperLlc2Support */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the llcSapStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcSapStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    llcSapStatsEntry_t *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            llcPortVirtualIndex;
    long            llcSapNumber;


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
     if ( (arg == -1) || (data = k_llcSapStatsEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)) == NULL) {
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

#ifdef I_llcSapStatsLocalBusies
    case I_llcSapStatsLocalBusies:
       dp = (void *) (&data->llcSapStatsLocalBusies);
       break;
#endif /* I_llcSapStatsLocalBusies */

#ifdef I_llcSapStatsRemoteBusies
    case I_llcSapStatsRemoteBusies:
       dp = (void *) (&data->llcSapStatsRemoteBusies);
       break;
#endif /* I_llcSapStatsRemoteBusies */

#ifdef I_llcSapStatsIFramesIn
    case I_llcSapStatsIFramesIn:
       dp = (void *) (&data->llcSapStatsIFramesIn);
       break;
#endif /* I_llcSapStatsIFramesIn */

#ifdef I_llcSapStatsIFramesOut
    case I_llcSapStatsIFramesOut:
       dp = (void *) (&data->llcSapStatsIFramesOut);
       break;
#endif /* I_llcSapStatsIFramesOut */

#ifdef I_llcSapStatsIOctetsIn
    case I_llcSapStatsIOctetsIn:
       dp = (void *) (&data->llcSapStatsIOctetsIn);
       break;
#endif /* I_llcSapStatsIOctetsIn */

#ifdef I_llcSapStatsIOctetsOut
    case I_llcSapStatsIOctetsOut:
       dp = (void *) (&data->llcSapStatsIOctetsOut);
       break;
#endif /* I_llcSapStatsIOctetsOut */

#ifdef I_llcSapStatsSFramesIn
    case I_llcSapStatsSFramesIn:
       dp = (void *) (&data->llcSapStatsSFramesIn);
       break;
#endif /* I_llcSapStatsSFramesIn */

#ifdef I_llcSapStatsSFramesOut
    case I_llcSapStatsSFramesOut:
       dp = (void *) (&data->llcSapStatsSFramesOut);
       break;
#endif /* I_llcSapStatsSFramesOut */

#ifdef I_llcSapStatsRetransmitsOut
    case I_llcSapStatsRetransmitsOut:
       dp = (void *) (&data->llcSapStatsRetransmitsOut);
       break;
#endif /* I_llcSapStatsRetransmitsOut */

#ifdef I_llcSapStatsREJsIn
    case I_llcSapStatsREJsIn:
       dp = (void *) (&data->llcSapStatsREJsIn);
       break;
#endif /* I_llcSapStatsREJsIn */

#ifdef I_llcSapStatsREJsOut
    case I_llcSapStatsREJsOut:
       dp = (void *) (&data->llcSapStatsREJsOut);
       break;
#endif /* I_llcSapStatsREJsOut */

#ifdef I_llcSapStatsWwCount
    case I_llcSapStatsWwCount:
       dp = (void *) (&data->llcSapStatsWwCount);
       break;
#endif /* I_llcSapStatsWwCount */

#ifdef I_llcSapStatsTESTCommandsIn
    case I_llcSapStatsTESTCommandsIn:
       dp = (void *) (&data->llcSapStatsTESTCommandsIn);
       break;
#endif /* I_llcSapStatsTESTCommandsIn */

#ifdef I_llcSapStatsTESTCommandsOut
    case I_llcSapStatsTESTCommandsOut:
       dp = (void *) (&data->llcSapStatsTESTCommandsOut);
       break;
#endif /* I_llcSapStatsTESTCommandsOut */

#ifdef I_llcSapStatsTESTResponsesIn
    case I_llcSapStatsTESTResponsesIn:
       dp = (void *) (&data->llcSapStatsTESTResponsesIn);
       break;
#endif /* I_llcSapStatsTESTResponsesIn */

#ifdef I_llcSapStatsTESTResponsesOut
    case I_llcSapStatsTESTResponsesOut:
       dp = (void *) (&data->llcSapStatsTESTResponsesOut);
       break;
#endif /* I_llcSapStatsTESTResponsesOut */

#ifdef I_llcSapStatsXIDCommandsIn
    case I_llcSapStatsXIDCommandsIn:
       dp = (void *) (&data->llcSapStatsXIDCommandsIn);
       break;
#endif /* I_llcSapStatsXIDCommandsIn */

#ifdef I_llcSapStatsXIDCommandsOut
    case I_llcSapStatsXIDCommandsOut:
       dp = (void *) (&data->llcSapStatsXIDCommandsOut);
       break;
#endif /* I_llcSapStatsXIDCommandsOut */

#ifdef I_llcSapStatsXIDResponsesIn
    case I_llcSapStatsXIDResponsesIn:
       dp = (void *) (&data->llcSapStatsXIDResponsesIn);
       break;
#endif /* I_llcSapStatsXIDResponsesIn */

#ifdef I_llcSapStatsXIDResponsesOut
    case I_llcSapStatsXIDResponsesOut:
       dp = (void *) (&data->llcSapStatsXIDResponsesOut);
       break;
#endif /* I_llcSapStatsXIDResponsesOut */

#ifdef I_llcSapStatsUIFramesIn
    case I_llcSapStatsUIFramesIn:
       dp = (void *) (&data->llcSapStatsUIFramesIn);
       break;
#endif /* I_llcSapStatsUIFramesIn */

#ifdef I_llcSapStatsUIFramesOut
    case I_llcSapStatsUIFramesOut:
       dp = (void *) (&data->llcSapStatsUIFramesOut);
       break;
#endif /* I_llcSapStatsUIFramesOut */

#ifdef I_llcSapStatsUIOctetsIn
    case I_llcSapStatsUIOctetsIn:
       dp = (void *) (&data->llcSapStatsUIOctetsIn);
       break;
#endif /* I_llcSapStatsUIOctetsIn */

#ifdef I_llcSapStatsUIOctetsOut
    case I_llcSapStatsUIOctetsOut:
       dp = (void *) (&data->llcSapStatsUIOctetsOut);
       break;
#endif /* I_llcSapStatsUIOctetsOut */

#ifdef I_llcSapStatsConnectOk
    case I_llcSapStatsConnectOk:
       dp = (void *) (&data->llcSapStatsConnectOk);
       break;
#endif /* I_llcSapStatsConnectOk */

#ifdef I_llcSapStatsConnectFail
    case I_llcSapStatsConnectFail:
       dp = (void *) (&data->llcSapStatsConnectFail);
       break;
#endif /* I_llcSapStatsConnectFail */

#ifdef I_llcSapStatsDisconnect
    case I_llcSapStatsDisconnect:
       dp = (void *) (&data->llcSapStatsDisconnect);
       break;
#endif /* I_llcSapStatsDisconnect */

#ifdef I_llcSapStatsDisconnectFRMRSend
    case I_llcSapStatsDisconnectFRMRSend:
       dp = (void *) (&data->llcSapStatsDisconnectFRMRSend);
       break;
#endif /* I_llcSapStatsDisconnectFRMRSend */

#ifdef I_llcSapStatsDisconnectFRMRRcv
    case I_llcSapStatsDisconnectFRMRRcv:
       dp = (void *) (&data->llcSapStatsDisconnectFRMRRcv);
       break;
#endif /* I_llcSapStatsDisconnectFRMRRcv */

#ifdef I_llcSapStatsDisconnectTimer
    case I_llcSapStatsDisconnectTimer:
       dp = (void *) (&data->llcSapStatsDisconnectTimer);
       break;
#endif /* I_llcSapStatsDisconnectTimer */

#ifdef I_llcSapStatsDMsInABM
    case I_llcSapStatsDMsInABM:
       dp = (void *) (&data->llcSapStatsDMsInABM);
       break;
#endif /* I_llcSapStatsDMsInABM */

#ifdef I_llcSapStatsSABMEsInABM
    case I_llcSapStatsSABMEsInABM:
       dp = (void *) (&data->llcSapStatsSABMEsInABM);
       break;
#endif /* I_llcSapStatsSABMEsInABM */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the llcCcAdminEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcCcAdminEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp = NULL;
    llcCcAdminEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            ifIndex;
    int             ifIndex_offset;
    long            llcPortVirtualIndex;
    int             llcPortVirtualIndex_offset;
    long            llcSapNumber;
    int             llcSapNumber_offset;
    OctetString *   llcCcRMac = NULL;
    int             llcCcRMac_offset;
    long            llcCcRSap;
    int             llcCcRSap_offset;
    int             index;
    int             final_index;

    ifIndex_offset = object->oid.length;
    llcPortVirtualIndex_offset = ifIndex_offset + 1;
    llcSapNumber_offset = llcPortVirtualIndex_offset + 1;
    llcCcRMac_offset = llcSapNumber_offset + 1;
    llcCcRSap_offset = llcCcRMac_offset + GetVariableIndexLength(incoming, llcCcRMac_offset);
    final_index = llcCcRSap_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, llcCcRSap_offset, &llcCcRSap, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcCcRSap = MAX(1, llcCcRSap);
    }

    if ( (InstToVariableOctetString(incoming, llcCcRMac_offset, &llcCcRMac, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, llcSapNumber_offset, &llcSapNumber, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcSapNumber = MAX(1, llcSapNumber);
    }

    if ( (InstToInt(incoming, llcPortVirtualIndex_offset, &llcPortVirtualIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcPortVirtualIndex = MAX(0, llcPortVirtualIndex);
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
     if ( (arg == -1) || (data = k_llcCcAdminEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex, llcSapNumber, llcCcRMac, llcCcRSap)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.oid_ptr[index++] = (unsigned long) data->llcPortVirtualIndex;
        inst.oid_ptr[index++] = (unsigned long) data->llcSapNumber;
        inst.oid_ptr[index++] = data->llcCcRMac->length;
        for(i = 0; i < data->llcCcRMac->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->llcCcRMac->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->llcCcRSap;
        inst.length = index;
     }

        FreeOctetString(llcCcRMac);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_llcCcAdminBounce
    case I_llcCcAdminBounce:
       dp = (void *) (&data->llcCcAdminBounce);
       break;
#endif /* I_llcCcAdminBounce */

#ifdef I_llcCcAdminMaxPDUOctets
    case I_llcCcAdminMaxPDUOctets:
       dp = (void *) (&data->llcCcAdminMaxPDUOctets);
       break;
#endif /* I_llcCcAdminMaxPDUOctets */

#ifdef I_llcCcAdminMaxUnackedIPDUsSend
    case I_llcCcAdminMaxUnackedIPDUsSend:
       dp = (void *) (&data->llcCcAdminMaxUnackedIPDUsSend);
       break;
#endif /* I_llcCcAdminMaxUnackedIPDUsSend */

#ifdef I_llcCcAdminMaxUnackedIPDUsRcv
    case I_llcCcAdminMaxUnackedIPDUsRcv:
       dp = (void *) (&data->llcCcAdminMaxUnackedIPDUsRcv);
       break;
#endif /* I_llcCcAdminMaxUnackedIPDUsRcv */

#ifdef I_llcCcAdminMaxRetransmits
    case I_llcCcAdminMaxRetransmits:
       dp = (void *) (&data->llcCcAdminMaxRetransmits);
       break;
#endif /* I_llcCcAdminMaxRetransmits */

#ifdef I_llcCcAdminAckTimer
    case I_llcCcAdminAckTimer:
       dp = (void *) (&data->llcCcAdminAckTimer);
       break;
#endif /* I_llcCcAdminAckTimer */

#ifdef I_llcCcAdminPbitTimer
    case I_llcCcAdminPbitTimer:
       dp = (void *) (&data->llcCcAdminPbitTimer);
       break;
#endif /* I_llcCcAdminPbitTimer */

#ifdef I_llcCcAdminRejTimer
    case I_llcCcAdminRejTimer:
       dp = (void *) (&data->llcCcAdminRejTimer);
       break;
#endif /* I_llcCcAdminRejTimer */

#ifdef I_llcCcAdminBusyTimer
    case I_llcCcAdminBusyTimer:
       dp = (void *) (&data->llcCcAdminBusyTimer);
       break;
#endif /* I_llcCcAdminBusyTimer */

#ifdef I_llcCcAdminInactTimer
    case I_llcCcAdminInactTimer:
       dp = (void *) (&data->llcCcAdminInactTimer);
       break;
#endif /* I_llcCcAdminInactTimer */

#ifdef I_llcCcAdminDelayAckCount
    case I_llcCcAdminDelayAckCount:
       dp = (void *) (&data->llcCcAdminDelayAckCount);
       break;
#endif /* I_llcCcAdminDelayAckCount */

#ifdef I_llcCcAdminDelayAckTimer
    case I_llcCcAdminDelayAckTimer:
       dp = (void *) (&data->llcCcAdminDelayAckTimer);
       break;
#endif /* I_llcCcAdminDelayAckTimer */

#ifdef I_llcCcAdminNw
    case I_llcCcAdminNw:
       dp = (void *) (&data->llcCcAdminNw);
       break;
#endif /* I_llcCcAdminNw */

#ifdef I_llcCcAdminRowStatus
    case I_llcCcAdminRowStatus:
       dp = (void *) (&data->llcCcAdminRowStatus);
       break;
#endif /* I_llcCcAdminRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the llcCcAdminEntry data object.
 *---------------------------------------------------------------------*/
void
llcCcAdminEntry_free(llcCcAdminEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->llcCcRMac);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after llcCcAdminEntry set/undo
 *---------------------------------------------------------------------*/
static int
llcCcAdminEntry_cleanup(doList_t *trash)
{
   llcCcAdminEntry_free(trash->data);
#ifdef SR_SNMPv2
   llcCcAdminEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the llcCcAdminEntry family
 *---------------------------------------------------------------------*/
llcCcAdminEntry_t *
Clone_llcCcAdminEntry(llcCcAdminEntry_t *llcCcAdminEntry)
{
    llcCcAdminEntry_t *data;

    if ((data = malloc(sizeof(llcCcAdminEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (llcCcAdminEntry), sizeof(llcCcAdminEntry_t));

    data->llcCcRMac = CloneOctetString(llcCcAdminEntry->llcCcRMac);

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
llcCcAdminEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    llcCcAdminEntry_t     *llcCcAdminEntry;
    long           ifIndex;
    int            ifIndex_offset;
    long           llcPortVirtualIndex;
    int            llcPortVirtualIndex_offset;
    long           llcSapNumber;
    int            llcSapNumber_offset;
    OctetString *  llcCcRMac = NULL;
    int            llcCcRMac_offset;
    long           llcCcRSap;
    int            llcCcRSap_offset;
    int            final_index;

   /*
    * Validate the object instance
    *
    */
    ifIndex_offset = object->oid.length;
    llcPortVirtualIndex_offset = ifIndex_offset + 1;
    llcSapNumber_offset = llcPortVirtualIndex_offset + 1;
    llcCcRMac_offset = llcSapNumber_offset + 1;
    llcCcRSap_offset = llcCcRMac_offset + GetVariableIndexLength(incoming, llcCcRMac_offset);
    final_index = llcCcRSap_offset + 1;

    if (final_index != incoming->length) {
          return(NO_CREATION_ERROR);
    }

    if ( (InstToInt(incoming, ifIndex_offset, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, llcPortVirtualIndex_offset, &llcPortVirtualIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, llcSapNumber_offset, &llcSapNumber, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToVariableOctetString(incoming, llcCcRMac_offset, &llcCcRMac, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, llcCcRSap_offset, &llcCcRSap, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        FreeOctetString(llcCcRMac);
        return error_status;
    }

    llcCcAdminEntry = k_llcCcAdminEntry_get(-1, contextInfo, -1, EXACT, ifIndex, llcPortVirtualIndex, llcSapNumber, llcCcRMac, llcCcRSap);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == llcCcAdminEntry_set) &&
            (((llcCcAdminEntry_t *) (dp->data)) != NULL) &&
            (((llcCcAdminEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((llcCcAdminEntry_t *) (dp->data))->llcPortVirtualIndex == llcPortVirtualIndex) &&
            (((llcCcAdminEntry_t *) (dp->data))->llcSapNumber == llcSapNumber) &&
            (CmpOctetStrings(((llcCcAdminEntry_t *) (dp->data))->llcCcRMac, llcCcRMac) == 0) &&
            (((llcCcAdminEntry_t *) (dp->data))->llcCcRSap == llcCcRSap) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = llcCcAdminEntry_set;
        dp->cleanupMethod = llcCcAdminEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_llcCcAdminEntry_UNDO
        dp->undoMethod = llcCcAdminEntry_undo;
#else /* SR_llcCcAdminEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_llcCcAdminEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (llcCcAdminEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_llcCcAdminEntry(llcCcAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_llcCcAdminEntry(llcCcAdminEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(llcCcAdminEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(llcCcAdminEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((llcCcAdminEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((llcCcAdminEntry_t *) (dp->data))->valid);

                ((llcCcAdminEntry_t *) (dp->data))->llcPortVirtualIndex = (llcPortVirtualIndex);
                SET_VALID(I_llcPortVirtualIndex, ((llcCcAdminEntry_t *) (dp->data))->valid);

                ((llcCcAdminEntry_t *) (dp->data))->llcSapNumber = (llcSapNumber);
                SET_VALID(I_llcSapNumber, ((llcCcAdminEntry_t *) (dp->data))->valid);

                ((llcCcAdminEntry_t *) (dp->data))->llcCcRMac = CloneOctetString(llcCcRMac);
                SET_VALID(I_llcCcRMac, ((llcCcAdminEntry_t *) (dp->data))->valid);

                ((llcCcAdminEntry_t *) (dp->data))->llcCcRSap = (llcCcRSap);
                SET_VALID(I_llcCcRSap, ((llcCcAdminEntry_t *) (dp->data))->valid);

                error_status = k_llcCcAdminEntry_set_defaults(dp);
            }
        }
    }

        FreeOctetString(llcCcRMac);
    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_llcCcAdminBounce
   case I_llcCcAdminBounce:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminBounce = value->sl_value;
     break;
#endif /* I_llcCcAdminBounce */

#ifdef I_llcCcAdminMaxPDUOctets
   case I_llcCcAdminMaxPDUOctets:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminMaxPDUOctets = value->sl_value;
     break;
#endif /* I_llcCcAdminMaxPDUOctets */

#ifdef I_llcCcAdminMaxUnackedIPDUsSend
   case I_llcCcAdminMaxUnackedIPDUsSend:

     if ( ((value->sl_value < 0) || (value->sl_value > 127)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminMaxUnackedIPDUsSend = value->sl_value;
     break;
#endif /* I_llcCcAdminMaxUnackedIPDUsSend */

#ifdef I_llcCcAdminMaxUnackedIPDUsRcv
   case I_llcCcAdminMaxUnackedIPDUsRcv:

     if ( ((value->sl_value < 0) || (value->sl_value > 127)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminMaxUnackedIPDUsRcv = value->sl_value;
     break;
#endif /* I_llcCcAdminMaxUnackedIPDUsRcv */

#ifdef I_llcCcAdminMaxRetransmits
   case I_llcCcAdminMaxRetransmits:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminMaxRetransmits = value->sl_value;
     break;
#endif /* I_llcCcAdminMaxRetransmits */

#ifdef I_llcCcAdminAckTimer
   case I_llcCcAdminAckTimer:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminAckTimer = value->ul_value;
     break;
#endif /* I_llcCcAdminAckTimer */

#ifdef I_llcCcAdminPbitTimer
   case I_llcCcAdminPbitTimer:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminPbitTimer = value->ul_value;
     break;
#endif /* I_llcCcAdminPbitTimer */

#ifdef I_llcCcAdminRejTimer
   case I_llcCcAdminRejTimer:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminRejTimer = value->ul_value;
     break;
#endif /* I_llcCcAdminRejTimer */

#ifdef I_llcCcAdminBusyTimer
   case I_llcCcAdminBusyTimer:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminBusyTimer = value->ul_value;
     break;
#endif /* I_llcCcAdminBusyTimer */

#ifdef I_llcCcAdminInactTimer
   case I_llcCcAdminInactTimer:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminInactTimer = value->ul_value;
     break;
#endif /* I_llcCcAdminInactTimer */

#ifdef I_llcCcAdminDelayAckCount
   case I_llcCcAdminDelayAckCount:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminDelayAckCount = value->sl_value;
     break;
#endif /* I_llcCcAdminDelayAckCount */

#ifdef I_llcCcAdminDelayAckTimer
   case I_llcCcAdminDelayAckTimer:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminDelayAckTimer = value->ul_value;
     break;
#endif /* I_llcCcAdminDelayAckTimer */

#ifdef I_llcCcAdminNw
   case I_llcCcAdminNw:

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminNw = value->sl_value;
     break;
#endif /* I_llcCcAdminNw */

#ifdef I_llcCcAdminRowStatus
   case I_llcCcAdminRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_llcCcAdminRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (llcCcAdminEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_llcCcAdminRowStatus_notInService) || (value->sl_value == D_llcCcAdminRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_llcCcAdminRowStatus_createAndGo) || (value->sl_value == D_llcCcAdminRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((llcCcAdminEntry_t *) (dp->data))->llcCcAdminRowStatus = value->sl_value;
     break;
#endif /* I_llcCcAdminRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in llcCcAdminEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_llcCcAdminEntry_test */
   error_status = k_llcCcAdminEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((llcCcAdminEntry_t *) (dp->data))->valid);
        error_status = k_llcCcAdminEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
llcCcAdminEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_llcCcAdminEntry_set((llcCcAdminEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the llcCcOperEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcCcOperEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp = NULL;
    llcCcOperEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            ifIndex;
    int             ifIndex_offset;
    long            llcPortVirtualIndex;
    int             llcPortVirtualIndex_offset;
    long            llcSapNumber;
    int             llcSapNumber_offset;
    OctetString *   llcCcRMac = NULL;
    int             llcCcRMac_offset;
    long            llcCcRSap;
    int             llcCcRSap_offset;
    int             index;
    int             final_index;

    ifIndex_offset = object->oid.length;
    llcPortVirtualIndex_offset = ifIndex_offset + 1;
    llcSapNumber_offset = llcPortVirtualIndex_offset + 1;
    llcCcRMac_offset = llcSapNumber_offset + 1;
    llcCcRSap_offset = llcCcRMac_offset + GetVariableIndexLength(incoming, llcCcRMac_offset);
    final_index = llcCcRSap_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, llcCcRSap_offset, &llcCcRSap, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcCcRSap = MAX(1, llcCcRSap);
    }

    if ( (InstToVariableOctetString(incoming, llcCcRMac_offset, &llcCcRMac, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, llcSapNumber_offset, &llcSapNumber, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcSapNumber = MAX(1, llcSapNumber);
    }

    if ( (InstToInt(incoming, llcPortVirtualIndex_offset, &llcPortVirtualIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcPortVirtualIndex = MAX(0, llcPortVirtualIndex);
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
     if ( (arg == -1) || (data = k_llcCcOperEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex, llcSapNumber, llcCcRMac, llcCcRSap)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.oid_ptr[index++] = (unsigned long) data->llcPortVirtualIndex;
        inst.oid_ptr[index++] = (unsigned long) data->llcSapNumber;
        inst.oid_ptr[index++] = data->llcCcRMac->length;
        for(i = 0; i < data->llcCcRMac->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->llcCcRMac->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->llcCcRSap;
        inst.length = index;
     }

        FreeOctetString(llcCcRMac);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_llcCcOperState
    case I_llcCcOperState:
       dp = (void *) (&data->llcCcOperState);
       break;
#endif /* I_llcCcOperState */

#ifdef I_llcCcOperMaxIPDUOctetsSend
    case I_llcCcOperMaxIPDUOctetsSend:
       dp = (void *) (&data->llcCcOperMaxIPDUOctetsSend);
       break;
#endif /* I_llcCcOperMaxIPDUOctetsSend */

#ifdef I_llcCcOperMaxIPDUOctetsRcv
    case I_llcCcOperMaxIPDUOctetsRcv:
       dp = (void *) (&data->llcCcOperMaxIPDUOctetsRcv);
       break;
#endif /* I_llcCcOperMaxIPDUOctetsRcv */

#ifdef I_llcCcOperMaxUnackedIPDUsSend
    case I_llcCcOperMaxUnackedIPDUsSend:
       dp = (void *) (&data->llcCcOperMaxUnackedIPDUsSend);
       break;
#endif /* I_llcCcOperMaxUnackedIPDUsSend */

#ifdef I_llcCcOperMaxUnackedIPDUsRcv
    case I_llcCcOperMaxUnackedIPDUsRcv:
       dp = (void *) (&data->llcCcOperMaxUnackedIPDUsRcv);
       break;
#endif /* I_llcCcOperMaxUnackedIPDUsRcv */

#ifdef I_llcCcOperMaxRetransmits
    case I_llcCcOperMaxRetransmits:
       dp = (void *) (&data->llcCcOperMaxRetransmits);
       break;
#endif /* I_llcCcOperMaxRetransmits */

#ifdef I_llcCcOperAckTimer
    case I_llcCcOperAckTimer:
       dp = (void *) (&data->llcCcOperAckTimer);
       break;
#endif /* I_llcCcOperAckTimer */

#ifdef I_llcCcOperPbitTimer
    case I_llcCcOperPbitTimer:
       dp = (void *) (&data->llcCcOperPbitTimer);
       break;
#endif /* I_llcCcOperPbitTimer */

#ifdef I_llcCcOperRejTimer
    case I_llcCcOperRejTimer:
       dp = (void *) (&data->llcCcOperRejTimer);
       break;
#endif /* I_llcCcOperRejTimer */

#ifdef I_llcCcOperBusyTimer
    case I_llcCcOperBusyTimer:
       dp = (void *) (&data->llcCcOperBusyTimer);
       break;
#endif /* I_llcCcOperBusyTimer */

#ifdef I_llcCcOperInactTimer
    case I_llcCcOperInactTimer:
       dp = (void *) (&data->llcCcOperInactTimer);
       break;
#endif /* I_llcCcOperInactTimer */

#ifdef I_llcCcOperDelayAckCount
    case I_llcCcOperDelayAckCount:
       dp = (void *) (&data->llcCcOperDelayAckCount);
       break;
#endif /* I_llcCcOperDelayAckCount */

#ifdef I_llcCcOperDelayAckTimer
    case I_llcCcOperDelayAckTimer:
       dp = (void *) (&data->llcCcOperDelayAckTimer);
       break;
#endif /* I_llcCcOperDelayAckTimer */

#ifdef I_llcCcOperNw
    case I_llcCcOperNw:
       dp = (void *) (&data->llcCcOperNw);
       break;
#endif /* I_llcCcOperNw */

#ifdef I_llcCcOperWw
    case I_llcCcOperWw:
       dp = (void *) (&data->llcCcOperWw);
       break;
#endif /* I_llcCcOperWw */

#ifdef I_llcCcOperCreateTime
    case I_llcCcOperCreateTime:
       dp = (void *) (&data->llcCcOperCreateTime);
       break;
#endif /* I_llcCcOperCreateTime */

#ifdef I_llcCcOperLastModifyTime
    case I_llcCcOperLastModifyTime:
       dp = (void *) (&data->llcCcOperLastModifyTime);
       break;
#endif /* I_llcCcOperLastModifyTime */

#ifdef I_llcCcOperLastFailTime
    case I_llcCcOperLastFailTime:
       dp = (void *) (&data->llcCcOperLastFailTime);
       break;
#endif /* I_llcCcOperLastFailTime */

#ifdef I_llcCcOperLastFailCause
    case I_llcCcOperLastFailCause:
       dp = (void *) (&data->llcCcOperLastFailCause);
       break;
#endif /* I_llcCcOperLastFailCause */

#ifdef I_llcCcOperLastFailFRMRInfo
    case I_llcCcOperLastFailFRMRInfo:
       dp = (void *) (CloneOctetString(data->llcCcOperLastFailFRMRInfo));
       break;
#endif /* I_llcCcOperLastFailFRMRInfo */

#ifdef I_llcCcOperLastWwCause
    case I_llcCcOperLastWwCause:
       dp = (void *) (&data->llcCcOperLastWwCause);
       break;
#endif /* I_llcCcOperLastWwCause */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the llcCcStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
llcCcStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp = NULL;
    llcCcStatsEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            ifIndex;
    int             ifIndex_offset;
    long            llcPortVirtualIndex;
    int             llcPortVirtualIndex_offset;
    long            llcSapNumber;
    int             llcSapNumber_offset;
    OctetString *   llcCcRMac = NULL;
    int             llcCcRMac_offset;
    long            llcCcRSap;
    int             llcCcRSap_offset;
    int             index;
    int             final_index;

    ifIndex_offset = object->oid.length;
    llcPortVirtualIndex_offset = ifIndex_offset + 1;
    llcSapNumber_offset = llcPortVirtualIndex_offset + 1;
    llcCcRMac_offset = llcSapNumber_offset + 1;
    llcCcRSap_offset = llcCcRMac_offset + GetVariableIndexLength(incoming, llcCcRMac_offset);
    final_index = llcCcRSap_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, llcCcRSap_offset, &llcCcRSap, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcCcRSap = MAX(1, llcCcRSap);
    }

    if ( (InstToVariableOctetString(incoming, llcCcRMac_offset, &llcCcRMac, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, llcSapNumber_offset, &llcSapNumber, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcSapNumber = MAX(1, llcSapNumber);
    }

    if ( (InstToInt(incoming, llcPortVirtualIndex_offset, &llcPortVirtualIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        llcPortVirtualIndex = MAX(0, llcPortVirtualIndex);
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
     if ( (arg == -1) || (data = k_llcCcStatsEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, llcPortVirtualIndex, llcSapNumber, llcCcRMac, llcCcRSap)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        index = 0;
        inst.oid_ptr[index++] = (unsigned long) data->ifIndex;
        inst.oid_ptr[index++] = (unsigned long) data->llcPortVirtualIndex;
        inst.oid_ptr[index++] = (unsigned long) data->llcSapNumber;
        inst.oid_ptr[index++] = data->llcCcRMac->length;
        for(i = 0; i < data->llcCcRMac->length; i++) {
            inst.oid_ptr[index++] = (unsigned long) data->llcCcRMac->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
        }
        
        inst.oid_ptr[index++] = (unsigned long) data->llcCcRSap;
        inst.length = index;
     }

        FreeOctetString(llcCcRMac);

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_llcCcStatsLocalBusies
    case I_llcCcStatsLocalBusies:
       dp = (void *) (&data->llcCcStatsLocalBusies);
       break;
#endif /* I_llcCcStatsLocalBusies */

#ifdef I_llcCcStatsRemoteBusies
    case I_llcCcStatsRemoteBusies:
       dp = (void *) (&data->llcCcStatsRemoteBusies);
       break;
#endif /* I_llcCcStatsRemoteBusies */

#ifdef I_llcCcStatsIFramesIn
    case I_llcCcStatsIFramesIn:
       dp = (void *) (&data->llcCcStatsIFramesIn);
       break;
#endif /* I_llcCcStatsIFramesIn */

#ifdef I_llcCcStatsIFramesOut
    case I_llcCcStatsIFramesOut:
       dp = (void *) (&data->llcCcStatsIFramesOut);
       break;
#endif /* I_llcCcStatsIFramesOut */

#ifdef I_llcCcStatsIOctetsIn
    case I_llcCcStatsIOctetsIn:
       dp = (void *) (&data->llcCcStatsIOctetsIn);
       break;
#endif /* I_llcCcStatsIOctetsIn */

#ifdef I_llcCcStatsIOctetsOut
    case I_llcCcStatsIOctetsOut:
       dp = (void *) (&data->llcCcStatsIOctetsOut);
       break;
#endif /* I_llcCcStatsIOctetsOut */

#ifdef I_llcCcStatsSFramesIn
    case I_llcCcStatsSFramesIn:
       dp = (void *) (&data->llcCcStatsSFramesIn);
       break;
#endif /* I_llcCcStatsSFramesIn */

#ifdef I_llcCcStatsSFramesOut
    case I_llcCcStatsSFramesOut:
       dp = (void *) (&data->llcCcStatsSFramesOut);
       break;
#endif /* I_llcCcStatsSFramesOut */

#ifdef I_llcCcStatsRetransmitsOut
    case I_llcCcStatsRetransmitsOut:
       dp = (void *) (&data->llcCcStatsRetransmitsOut);
       break;
#endif /* I_llcCcStatsRetransmitsOut */

#ifdef I_llcCcStatsREJsIn
    case I_llcCcStatsREJsIn:
       dp = (void *) (&data->llcCcStatsREJsIn);
       break;
#endif /* I_llcCcStatsREJsIn */

#ifdef I_llcCcStatsREJsOut
    case I_llcCcStatsREJsOut:
       dp = (void *) (&data->llcCcStatsREJsOut);
       break;
#endif /* I_llcCcStatsREJsOut */

#ifdef I_llcCcStatsWwCount
    case I_llcCcStatsWwCount:
       dp = (void *) (&data->llcCcStatsWwCount);
       break;
#endif /* I_llcCcStatsWwCount */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */

/*
 * Convert llc cc fail cause to snmp value
 */
static long get_snmp_cc_fail_cause (long fail_cause) {
    switch (fail_cause) {
        case SYSMGT_RX_FRMR:         return D_llcCcOperLastFailCause_rxFRMR;
        case SYSMGT_TX_FRMR:         return D_llcCcOperLastFailCause_txFRMR;
        case SYSMGT_DISC_RECEIVED:   return D_llcCcOperLastFailCause_discReceived;
        case SYSMGT_DISC_SENT:       return D_llcCcOperLastFailCause_discSent;
        case SYSMGT_RETRIES_EXPIRED: return D_llcCcOperLastFailCause_retriesExpired;
        case SYSMGT_FORCED_SHUTDOWN: return D_llcCcOperLastFailCause_forcedShutdown;
        default:                            break;
    }
    return D_llcCcOperLastFailCause_undefined;
}

/*
 * Convert llc cc ww cause to snmp value
 */
static long get_snmp_cc_ww_cause (long ww_cause) {
    switch (ww_cause) {
      case SYSMGT_NEVER_INVOKED:        return D_llcCcOperLastWwCause_neverInvoked;
      case SYSMGT_LOST_DATA:            return D_llcCcOperLastWwCause_lostData;
      case SYSMGT_MAC_LAYER_CONGESTION: return D_llcCcOperLastWwCause_macLayerCongestion;
      default:                          break;
    }
    return D_llcCcOperLastWwCause_other;
}

/*
 * Convert llc cc state to snmp llc cc state value
 */
static long get_snmp_llc_cc_state ( long state) {
    switch (state) {
      case SYSMGT_ADM:           return D_llcCcOperState_aDM;
      case SYSMGT_SETUP:         return D_llcCcOperState_setup;
      case SYSMGT_CONN:          return D_llcCcOperState_conn;
      case SYSMGT_NORMAL:        return D_llcCcOperState_normal;
      case SYSMGT_BUSY:          return D_llcCcOperState_busy;
      case SYSMGT_REJECT:        return D_llcCcOperState_reject;
      case SYSMGT_AWAIT:         return D_llcCcOperState_await;
      case SYSMGT_AWAIT_BUSY:    return D_llcCcOperState_awaitBusy;
      case SYSMGT_AWAIT_REJECT:  return D_llcCcOperState_awaitReject;
      case SYSMGT_D_CONN:        return D_llcCcOperState_dConn;
      case SYSMGT_RESET:         return D_llcCcOperState_reset;
      case SYSMGT_ERROR:         return D_llcCcOperState_error;
      case SYSMGT_PEND_DISC_RSP: return D_llcCcOperState_aDM;
      default: break;
    }
    return 0;
}


/* copyOctetStrings - Copy OctetString type.
 *                    Src length must be <= maxlen.
 * Input  : OctetString pointer to source
 *          OctetString pointer to Destination
 *          max length to copy
 */
static void CopyOctetStrings (OctetString *src, OctetString 
                              *dest, int maxlen) {
   if (dest) {
      if (src) {
         if (src->length <= maxlen) {
            if (src->octet_ptr) {
               dest->length = src->length;
               bcopy( src->octet_ptr, dest->octet_ptr, src->length);
            }
            else {
               dest->length = 0;
               memset( dest->octet_ptr, 0, maxlen);
	    }
         }
      }
      else {
         dest->length = 0;
         memset( dest->octet_ptr, 0, maxlen);
      }
   }
}

/*
 * get_port_entry - find an llc port entry
 */
static void *get_port_entry (int searchType, long *ifIndex, 
                             long *llcPortVirtualIndex, 
                             void *port_data, ushort size, ushort table_id) {
   void *result_p = NULL;
   hwidbtype *hwidb;
   sna_dlc_llc_t  llc;
   boolean exactSearch;
   
   /* 
    * Initialize temporary MAC addr Octet String for search
    */
   llc.size = size;
   llc.table_id = table_id;
   llc.llc_info = port_data;
   llc.info_type = SYSMGT_LLC_PORT;
   llc.llcPortVirtualIndex = *llcPortVirtualIndex;
   exactSearch = (searchType == EXACT);
   while((hwidb = cip_get_hwidb( searchType, *ifIndex)) != NULL) {
      if (reg_invoke_get_llc_info(exactSearch, hwidb, &llc)) {
         result_p = port_data;
         *llcPortVirtualIndex = llc.llcPortVirtualIndex;
         *ifIndex = hwidb->snmp_if_index;
      }	
      if ((result_p == NULL) && (searchType == NEXT)) {
         /* 
          * If not found on this hwidb, go to next hwidb 
          */
         llc.llcPortVirtualIndex = 0;
         *ifIndex = hwidb->snmp_if_index + 1;
      }
      else {
         break;
      }
   }
   return result_p;
}

/*
 * get_sap_entry - find an llc sap entry
 */
void *get_sap_entry (int searchType, long *ifIndex,          
                     long *llcPortVirtualIndex,
                     long llcSapNumber, void *sap_data, ushort size, 
                     ushort table_id) {
   void *result_p = NULL;
   hwidbtype *hwidb;
   sna_dlc_llc_t  llc;
   boolean exactSearch;
   
   /* 
    * Initialize temporary MAC addr Octet String for search
    */
   llc.size = size;
   llc.table_id = table_id;
   llc.llc_info = sap_data;
   llc.info_type = SYSMGT_LLC_SAP;
   llc.llcPortVirtualIndex = *llcPortVirtualIndex;
   llc.llcSapNumber = llcSapNumber;
   exactSearch = (searchType == EXACT);
   while((hwidb = cip_get_hwidb( searchType, *ifIndex)) != NULL) {
      if (reg_invoke_get_llc_info(exactSearch, hwidb, &llc)) {
         result_p = sap_data;
         *llcPortVirtualIndex = llc.llcPortVirtualIndex;
         *ifIndex = hwidb->snmp_if_index;
      }
      if ((result_p == NULL) && (searchType == NEXT)) {
         /* 
          * If not found on this hwidb, go to next hwidb 
          */
         *ifIndex = hwidb->snmp_if_index + 1;
         llc.llcPortVirtualIndex = 0;
         llc.llcSapNumber = 0;
      }
      else {
         break;
      }
   }
   return result_p;
}

/*
 * get_cc_entry - find an llc cc entry
 */
static void *get_cc_entry (int searchType, long *ifIndex, 
                           long *llcPortVirtualIndex,
                           long llcSapNumber, OctetString *llcCcRMac,
                           long llcCcRSap, void *cc_data, ushort size, 
                           ushort table_id) {
   void *result_p = NULL;
   hwidbtype *hwidb;
   sna_dlc_llc_t  llc;
   boolean exactSearch;
   
   /* 
    * Initialize temporary MAC addr Octet String for search
    */
   llc.size = size;
   llc.table_id = table_id;
   llc.llc_info = cc_data;
   llc.info_type = SYSMGT_LLC_CC;
   llc.llcPortVirtualIndex = *llcPortVirtualIndex;
   llc.llcSapNumber = llcSapNumber;
   if (llcCcRMac->octet_ptr) {
      bcopy( llcCcRMac->octet_ptr, llc.llcCcRMac, MAC_ADDR_LEN);
   }
   else {
      memset( llc.llcCcRMac, 0, MAC_ADDR_LEN);
   }
   llc.llcCcRSap = llcCcRSap;
   exactSearch = (searchType == EXACT);
   while((hwidb = cip_get_hwidb( searchType, *ifIndex)) != NULL) {
      if (reg_invoke_get_llc_info( exactSearch, hwidb, &llc)) {
         result_p = cc_data;
         *llcPortVirtualIndex = llc.llcPortVirtualIndex;
         *ifIndex = hwidb->snmp_if_index;
      }
      if ((result_p == NULL) && (searchType == NEXT)) {
         /* 
          * If not found on this hwidb, go to next hwidb 
          */
         *ifIndex = hwidb->snmp_if_index + 1;
         llc.llcPortVirtualIndex = 0;
         llc.llcSapNumber = 0;
         memset( llc.llcCcRMac, 0, MAC_ADDR_LEN);
         llc.llcCcRSap = 0;
      }
      else {
         break;
      }
   }
   return result_p;
}



llcPortAdminEntry_t *
k_llcPortAdminEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
{
   static llcPortAdminEntry_t llcPortAdminData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static OctetString         name_OS;
   static char                name[PORT_NAME_LEN+1];
   
   llc_port_admin_t port_admin;
   llcPortAdminEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex)) {
      result_p = &llcPortAdminData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      if (get_port_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                          &port_admin, LLC_PORT_ADMIN_SIZE, 
                          SYSMGT_LLC_PORT_ADMIN)) {
         bcopy(port_admin.llcPortAdminName, name, PORT_NAME_LEN);
         name[PORT_NAME_LEN] = 0;
         name_OS.octet_ptr = name;
         name_OS.length = strlen(name);
         llcPortAdminData.llcPortAdminName = &name_OS;
         llcPortAdminData.llcPortAdminMaxSaps = port_admin.llcPortAdminMaxSaps;
         llcPortAdminData.llcPortAdminMaxCcs = port_admin.llcPortAdminMaxCcs;
         llcPortAdminData.llcPortAdminMaxPDUOctets = 
                          port_admin.llcPortAdminMaxPDUOctets;
         llcPortAdminData.llcPortAdminMaxUnackedIPDUsSend = 
                          port_admin.llcPortAdminMaxUnackedIPDUsSend;
         llcPortAdminData.llcPortAdminMaxUnackedIPDUsRcv = 
                          port_admin.llcPortAdminMaxUnackedIPDUsRcv;
         llcPortAdminData.llcPortAdminMaxRetransmits = 
                          port_admin.llcPortAdminMaxRetransmits;
         llcPortAdminData.llcPortAdminAckTimer = 
                          port_admin.llcPortAdminAckTimer;
         llcPortAdminData.llcPortAdminPbitTimer = 
                          port_admin.llcPortAdminPbitTimer;
         llcPortAdminData.llcPortAdminRejTimer = 
                          port_admin.llcPortAdminRejTimer;
         llcPortAdminData.llcPortAdminBusyTimer = 
                          port_admin.llcPortAdminBusyTimer;
         llcPortAdminData.llcPortAdminInactTimer = 
                          port_admin.llcPortAdminInactTimer ;
         llcPortAdminData.llcPortAdminDelayAckCount = 
                          port_admin.llcPortAdminDelayAckCount;
         llcPortAdminData.llcPortAdminDelayAckTimer = 
                          port_admin.llcPortAdminDelayAckTimer;
         llcPortAdminData.llcPortAdminNw = port_admin.llcPortAdminNw;
         llcPortAdminData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcPortAdminData.ifIndex = lastIfIndex;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcPortAdminData.valid);
         result_p = &llcPortAdminData;
      }
   }
   return result_p;
}

#ifdef SETS
int
k_llcPortAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_llcPortAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_llcPortAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    llcPortAdminEntry_t *data = (llcPortAdminEntry_t *) (dp->data);

    return NO_ERROR;
}

int
k_llcPortAdminEntry_set(data, contextInfo, function)
   llcPortAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_llcPortAdminEntry_UNDO
/* add #define SR_llcPortAdminEntry_UNDO in snallcmib.h to
 * include the undo routine for the llcPortAdminEntry family.
 */
int
llcPortAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_llcPortAdminEntry_UNDO */
#endif /* SR_SNMPv2 */

llcPortOperEntry_t *
k_llcPortOperEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
{
   static llcPortOperEntry_t  llcPortOperData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static uchar               localMac[MAC_ADDR_LEN];
   static OctetString         localMac_OS = {NULL, 0};
   
   llc_port_oper_t port_oper;
   llcPortOperEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex)) {
      result_p = &llcPortOperData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      if (get_port_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                          &port_oper,
                          LLC_PORT_OPER_SIZE, SYSMGT_LLC_PORT_OPER)) {
         localMac_OS.octet_ptr = localMac;
         localMac_OS.length = MAC_ADDR_LEN;
         bcopy( port_oper.llcPortOperMacAddress, localMac, MAC_ADDR_LEN);
         llcPortOperData.llcPortOperMacAddress = &localMac_OS;
         llcPortOperData.llcPortOperNumSaps = port_oper.llcPortOperNumSaps;
         llcPortOperData.llcPortOperHiWaterNumSaps = 
                         port_oper.llcPortOperHiWaterNumSaps;
         llcPortOperData.llcPortOperSimRim = D_llcPortOperSimRim_no;
         llcPortOperData.llcPortOperLastModifyTime = 
                         port_oper.llcPortOperLastModifyTime;
         llcPortOperData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcPortOperData.ifIndex = lastIfIndex;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcPortOperData.valid);
         result_p = &llcPortOperData;
      }
   }
   return result_p;
}

llcPortStatsEntry_t *
k_llcPortStatsEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
{
   static llcPortStatsEntry_t llcPortStatsData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;

   llc_port_stats_t port_stats;
   llcPortStatsEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex)) {
      result_p = &llcPortStatsData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      if (get_port_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                          &port_stats,
                          LLC_PORT_STATS_SIZE, SYSMGT_LLC_PORT_STATS)) {
         llcPortStatsData.llcPortStatsPDUsIn = 
                          port_stats.llcPortStatsPDUsIn;
         llcPortStatsData.llcPortStatsPDUsOut = 
                          port_stats.llcPortStatsPDUsOut;
         llcPortStatsData.llcPortStatsOctetsIn = 
                          port_stats.llcPortStatsOctetsIn;
         llcPortStatsData.llcPortStatsOctetsOut = 
                          port_stats.llcPortStatsOctetsOut;
         llcPortStatsData.llcPortStatsTESTCommandsIn = 
                          port_stats.llcPortStatsTESTCommandsIn;
         llcPortStatsData.llcPortStatsTESTResponsesOut = 
                          port_stats.llcPortStatsTESTResponsesOut;
         llcPortStatsData.llcPortStatsLocalBusies = 
                          port_stats.llcPortStatsLocalBusies;
         llcPortStatsData.llcPortStatsUnknownSaps = 
                          port_stats.llcPortStatsUnknownSaps;
         llcPortStatsData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcPortStatsData.ifIndex = lastIfIndex;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcPortStatsData.valid);
         result_p = &llcPortStatsData;
      }
   }
   return result_p;
}

llcSapAdminEntry_t *
k_llcSapAdminEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
   long llcSapNumber;
{
   static llcSapAdminEntry_t  llcSapAdminData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static long                lastLlcSapNumber = MAXLONG;

   llc_sap_admin_t sap_admin;
   llcSapAdminEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex) && 
       (lastLlcSapNumber == llcSapNumber)) {
      result_p = &llcSapAdminData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      lastLlcSapNumber = llcSapNumber;
      if (get_sap_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                         lastLlcSapNumber, &sap_admin, 
                         LLC_SAP_ADMIN_SIZE, SYSMGT_LLC_SAP_ADMIN)) {
         llcSapAdminData.llcSapAdminMaxPDUOctets = 
                         sap_admin.llcSapAdminMaxPDUOctets;
         llcSapAdminData.llcSapAdminMaxUnackedIPDUsSend = 
                         sap_admin.llcSapAdminMaxUnackedIPDUsSend;
         llcSapAdminData.llcSapAdminMaxUnackedIPDUsRcv = 
                         sap_admin.llcSapAdminMaxUnackedIPDUsRcv;
         llcSapAdminData.llcSapAdminMaxRetransmits = 
                         sap_admin.llcSapAdminMaxRetransmits;
         llcSapAdminData.llcSapAdminAckTimer = sap_admin.llcSapAdminAckTimer;
         llcSapAdminData.llcSapAdminPbitTimer = sap_admin.llcSapAdminPbitTimer;
         llcSapAdminData.llcSapAdminRejTimer = sap_admin.llcSapAdminRejTimer;
         llcSapAdminData.llcSapAdminBusyTimer = sap_admin.llcSapAdminBusyTimer;
         llcSapAdminData.llcSapAdminInactTimer = 
                         sap_admin.llcSapAdminInactTimer;
         llcSapAdminData.llcSapAdminDelayAckCount = 
                         sap_admin.llcSapAdminDelayAckCount;
         llcSapAdminData.llcSapAdminDelayAckTimer = 
                         sap_admin.llcSapAdminDelayAckTimer;
         llcSapAdminData.llcSapAdminNw = sap_admin.llcSapAdminNw;
         llcSapAdminData.ifIndex = lastIfIndex;
         llcSapAdminData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcSapAdminData.llcSapNumber = sap_admin.index.local_sap;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastLlcSapNumber = llcSapNumber;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcSapAdminData.valid);
         result_p = &llcSapAdminData;
      }
   }
   return result_p;
}

#ifdef SETS
int
k_llcSapAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_llcSapAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_llcSapAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    llcSapAdminEntry_t *data = (llcSapAdminEntry_t *) (dp->data);

    return NO_ERROR;
}

int
k_llcSapAdminEntry_set(data, contextInfo, function)
   llcSapAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_llcSapAdminEntry_UNDO
/* add #define SR_llcSapAdminEntry_UNDO in snallcmib.h to
 * include the undo routine for the llcSapAdminEntry family.
 */
int
llcSapAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_llcSapAdminEntry_UNDO */
#endif /* SR_SNMPv2 */

llcSapOperEntry_t *
k_llcSapOperEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
   long llcSapNumber;
{
   static llcSapOperEntry_t   llcSapOperData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static long                lastLlcSapNumber = MAXLONG;

   llc_sap_oper_t sap_oper;
   llcSapOperEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex) && 
       (lastLlcSapNumber == llcSapNumber)) {
      result_p = &llcSapOperData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      lastLlcSapNumber = llcSapNumber;
      if (get_sap_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                         lastLlcSapNumber, &sap_oper, 
                         LLC_SAP_OPER_SIZE, SYSMGT_LLC_SAP_OPER)) {
         llcSapOperData.llcSapOperStatus = D_llcSapOperStatus_active;
         llcSapOperData.llcSapOperNumCcs = sap_oper.llcSapOperNumCcs;
         llcSapOperData.llcSapOperHiWaterNumCcs = 
                        sap_oper.llcSapOperHiWaterNumCcs;
         llcSapOperData.llcSapOperLlc2Support = D_llcSapOperLlc2Support_yes;
         llcSapOperData.ifIndex = lastIfIndex;
         llcSapOperData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcSapOperData.llcSapNumber = sap_oper.index.local_sap;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastLlcSapNumber = llcSapNumber;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcSapOperData.valid);
         result_p = &llcSapOperData;
      }
   }
   return result_p;
}

llcSapStatsEntry_t *
k_llcSapStatsEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex, llcSapNumber)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
   long llcSapNumber;
{
   static llcSapStatsEntry_t  llcSapStatsData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static long                lastLlcSapNumber = MAXLONG;

   llc_sap_stats_t sap_stats;
   llcSapStatsEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex) && 
       (lastLlcSapNumber == llcSapNumber)) {
      result_p = &llcSapStatsData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      lastLlcSapNumber = llcSapNumber;
      if (get_sap_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                         lastLlcSapNumber, &sap_stats, LLC_SAP_STATS_SIZE, 
                         SYSMGT_LLC_SAP_STATS)) {
         llcSapStatsData.llcSapStatsLocalBusies = 0;
         llcSapStatsData.llcSapStatsRemoteBusies = 0;
         llcSapStatsData.llcSapStatsIFramesIn = 0;
         llcSapStatsData.llcSapStatsIFramesOut = 0;
         llcSapStatsData.llcSapStatsIOctetsIn = 0;
         llcSapStatsData.llcSapStatsIOctetsOut = 0;
         llcSapStatsData.llcSapStatsSFramesIn = 0;
         llcSapStatsData.llcSapStatsSFramesOut = 0;
         llcSapStatsData.llcSapStatsRetransmitsOut = 0;
         llcSapStatsData.llcSapStatsREJsIn = 0;
         llcSapStatsData.llcSapStatsREJsOut = 0;
         llcSapStatsData.llcSapStatsWwCount = 0;
         llcSapStatsData.llcSapStatsTESTCommandsIn = 0;
         llcSapStatsData.llcSapStatsTESTResponsesOut = 0;
         llcSapStatsData.llcSapStatsTESTCommandsOut = 
                         sap_stats.llcSapStatsTESTCommandsOut;
         llcSapStatsData.llcSapStatsTESTResponsesIn = 
                         sap_stats.llcSapStatsTESTResponsesIn;
         llcSapStatsData.llcSapStatsXIDCommandsIn = 
                         sap_stats.llcSapStatsXIDCommandsIn;
         llcSapStatsData.llcSapStatsXIDCommandsOut = 
                         sap_stats.llcSapStatsXIDCommandsOut;
         llcSapStatsData.llcSapStatsXIDResponsesIn = 
                         sap_stats.llcSapStatsXIDResponsesIn;
         llcSapStatsData.llcSapStatsXIDResponsesOut = 
                         sap_stats.llcSapStatsXIDResponsesOut;
         llcSapStatsData.llcSapStatsUIFramesIn = 
                         sap_stats.llcSapStatsUIFramesIn;
         llcSapStatsData.llcSapStatsUIFramesOut = 
                         sap_stats.llcSapStatsUIFramesOut;
         llcSapStatsData.llcSapStatsUIOctetsIn = 
                         sap_stats.llcSapStatsUIOctetsIn;
         llcSapStatsData.llcSapStatsUIOctetsOut = 
                         sap_stats.llcSapStatsUIOctetsOut;
         llcSapStatsData.llcSapStatsConnectOk = 
                         sap_stats.llcSapStatsConnectOk;
         llcSapStatsData.llcSapStatsConnectFail = 
                         sap_stats.llcSapStatsConnectFail;
         llcSapStatsData.llcSapStatsDisconnect = 
                         sap_stats.llcSapStatsDisconnect;
         llcSapStatsData.llcSapStatsDisconnectFRMRSend =
                         sap_stats.llcSapStatsDisconnectFRMRSend;
         llcSapStatsData.llcSapStatsDisconnectFRMRRcv = 
                         sap_stats.llcSapStatsDisconnectFRMRRcv;
         llcSapStatsData.llcSapStatsDisconnectTimer = 
                         sap_stats.llcSapStatsDisconnectTimer;
         llcSapStatsData.llcSapStatsDMsInABM = sap_stats.llcSapStatsDMsInABM;
         llcSapStatsData.llcSapStatsSABMEsInABM = 
                         sap_stats.llcSapStatsSABMEsInABM;
         llcSapStatsData.ifIndex = lastIfIndex;
         llcSapStatsData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcSapStatsData.llcSapNumber = sap_stats.index.local_sap;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastLlcSapNumber = llcSapNumber;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcSapStatsData.valid);
         result_p = &llcSapStatsData;
      }
   }
   return result_p;
}

llcCcAdminEntry_t *
k_llcCcAdminEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex, llcSapNumber, llcCcRMac, llcCcRSap)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
   long llcSapNumber;
   OctetString * llcCcRMac;
   long llcCcRSap;
{
   static llcCcAdminEntry_t   llcCcAdminData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static long                lastLlcSapNumber = MAXLONG;
   static OctetString         lastLlcCcRMac_OS = {NULL, 0};
   static uchar               lastLlcCcRMac[MAC_ADDR_LEN];
   static long                lastLlcCcRSap = MAXLONG;
   static OctetString         tmpLlcCcRMac_OS = {NULL, 0};
   static uchar               tmpLlcCcRMac[MAC_ADDR_LEN];

   llc_cc_admin_t cc_admin;
   llcCcAdminEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex) && 
       (lastLlcSapNumber == llcSapNumber) &&
       (CmpOctetStrings(&lastLlcCcRMac_OS, llcCcRMac) == 0) &&
       (lastLlcCcRSap == llcCcRSap)) {
      result_p = &llcCcAdminData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      lastLlcSapNumber = llcSapNumber;
      lastLlcCcRMac_OS.length = llcCcRMac->length;
      lastLlcCcRMac_OS.octet_ptr = lastLlcCcRMac;
      lastLlcCcRSap = llcCcRSap;
      CopyOctetStrings( llcCcRMac, &lastLlcCcRMac_OS, MAC_ADDR_LEN);
      if (get_cc_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                        lastLlcSapNumber, &lastLlcCcRMac_OS, lastLlcCcRSap, &
                        cc_admin, LLC_CC_ADMIN_SIZE, SYSMGT_LLC_CC_ADMIN)) {
         llcCcAdminData.llcCcAdminBounce = D_llcCcAdminBounce_no;
         llcCcAdminData.llcCcAdminMaxPDUOctets = 
                        cc_admin.llcCcAdminMaxPDUOctets;
         llcCcAdminData.llcCcAdminMaxUnackedIPDUsSend = 
                        cc_admin.llcCcAdminMaxUnackedIPDUsSend;
         llcCcAdminData.llcCcAdminMaxUnackedIPDUsRcv = 
                        cc_admin.llcCcAdminMaxUnackedIPDUsRcv;
         llcCcAdminData.llcCcAdminMaxRetransmits = 
                        cc_admin.llcCcAdminMaxRetransmits;
         llcCcAdminData.llcCcAdminAckTimer = cc_admin.llcCcAdminAckTimer;
         llcCcAdminData.llcCcAdminPbitTimer = cc_admin.llcCcAdminPbitTimer;
         llcCcAdminData.llcCcAdminRejTimer = cc_admin.llcCcAdminRejTimer;
         llcCcAdminData.llcCcAdminBusyTimer = cc_admin.llcCcAdminBusyTimer;
         llcCcAdminData.llcCcAdminInactTimer = cc_admin.llcCcAdminInactTimer;
         llcCcAdminData.llcCcAdminDelayAckCount = 
                        cc_admin.llcCcAdminDelayAckCount;
         llcCcAdminData.llcCcAdminDelayAckTimer = 
                        cc_admin.llcCcAdminDelayAckTimer;
         llcCcAdminData.llcCcAdminNw = cc_admin.llcCcAdminNw;
         llcCcAdminData.llcCcAdminRowStatus = D_llcCcAdminRowStatus_active;
         llcCcAdminData.ifIndex = lastIfIndex;
         llcCcAdminData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcCcAdminData.llcSapNumber = cc_admin.index.local_sap;
         tmpLlcCcRMac_OS.length = MAC_ADDR_LEN;
         bcopy( cc_admin.index.rem_mac, tmpLlcCcRMac, MAC_ADDR_LEN);
         llcCcAdminData.llcCcRMac = &tmpLlcCcRMac_OS;
         llcCcAdminData.llcCcRSap = cc_admin.index.rem_sap;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastLlcSapNumber = llcSapNumber;
         CopyOctetStrings( llcCcRMac, &lastLlcCcRMac_OS, MAC_ADDR_LEN);
         lastLlcCcRSap = lastLlcCcRSap;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcCcAdminData.valid);
         result_p = &llcCcAdminData;
      }
   }
   return result_p;
}

#ifdef SETS
int
k_llcCcAdminEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_llcCcAdminEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_llcCcAdminEntry_set_defaults(dp)
    doList_t       *dp;
{
    llcCcAdminEntry_t *data = (llcCcAdminEntry_t *) (dp->data);

    return NO_ERROR;
}

int
k_llcCcAdminEntry_set(data, contextInfo, function)
   llcCcAdminEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_llcCcAdminEntry_UNDO
/* add #define SR_llcCcAdminEntry_UNDO in snallcmib.h to
 * include the undo routine for the llcCcAdminEntry family.
 */
int
llcCcAdminEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_llcCcAdminEntry_UNDO */
#endif /* SR_SNMPv2 */

llcCcOperEntry_t *
k_llcCcOperEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex, llcSapNumber, llcCcRMac, llcCcRSap)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
   long llcSapNumber;
   OctetString * llcCcRMac;
   long llcCcRSap;
{
   static llcCcOperEntry_t    llcCcOperData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static long                lastLlcSapNumber = MAXLONG;
   static OctetString         lastLlcCcRMac_OS = {NULL, 0};
   static uchar               lastLlcCcRMac[MAC_ADDR_LEN];
   static long                lastLlcCcRSap = MAXLONG;
   static OctetString         FRMRInfo_OS;
   static uchar               FRMRInfo[FRMR_INFO_LEN];
   static OctetString         tmpLlcCcRMac_OS = {NULL, 0};
   static uchar               tmpLlcCcRMac[MAC_ADDR_LEN];

   llc_cc_oper_t cc_oper;
   llcCcOperEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex) && 
       (lastLlcSapNumber == llcSapNumber) &&
       (CmpOctetStrings(&lastLlcCcRMac_OS, llcCcRMac) == 0) &&
       (lastLlcCcRSap == llcCcRSap)) {
      result_p = &llcCcOperData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      lastLlcSapNumber = llcSapNumber;
      lastLlcCcRMac_OS.length = llcCcRMac->length;
      lastLlcCcRMac_OS.octet_ptr = lastLlcCcRMac;
      CopyOctetStrings( llcCcRMac, &lastLlcCcRMac_OS, MAC_ADDR_LEN);
      lastLlcCcRSap = llcCcRSap;
      if (get_cc_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                        lastLlcSapNumber, &lastLlcCcRMac_OS, lastLlcCcRSap, 
                        &cc_oper, LLC_CC_OPER_SIZE, SYSMGT_LLC_CC_OPER)) {
         llcCcOperData.llcCcOperState = get_snmp_llc_cc_state( cc_oper.llcCcOperState);
         llcCcOperData.llcCcOperMaxIPDUOctetsSend = 
                       cc_oper.llcCcOperMaxIPDUOctetsSend;
         llcCcOperData.llcCcOperMaxIPDUOctetsRcv = 
                       cc_oper.llcCcOperMaxIPDUOctetsRcv;
         llcCcOperData.llcCcOperMaxUnackedIPDUsSend = 
                       cc_oper.llcCcOperMaxUnackedIPDUsSend;
         llcCcOperData.llcCcOperMaxUnackedIPDUsRcv = 
                       cc_oper.llcCcOperMaxUnackedIPDUsRcv;
         llcCcOperData.llcCcOperMaxRetransmits = 
                       cc_oper.llcCcOperMaxRetransmits;
         llcCcOperData.llcCcOperAckTimer = cc_oper.llcCcOperAckTimer;
         llcCcOperData.llcCcOperPbitTimer = cc_oper.llcCcOperPbitTimer;
         llcCcOperData.llcCcOperRejTimer = cc_oper.llcCcOperRejTimer;
         llcCcOperData.llcCcOperBusyTimer = cc_oper.llcCcOperBusyTimer;
         llcCcOperData.llcCcOperInactTimer = cc_oper.llcCcOperInactTimer;
         llcCcOperData.llcCcOperDelayAckCount = cc_oper.llcCcOperDelayAckCount;
         llcCcOperData.llcCcOperDelayAckTimer = cc_oper.llcCcOperDelayAckTimer;
         llcCcOperData.llcCcOperNw = cc_oper.llcCcOperNw;
         llcCcOperData.llcCcOperWw = cc_oper.llcCcOperWw;
         llcCcOperData.llcCcOperCreateTime = cc_oper.llcCcOperCreateTime;
         llcCcOperData.llcCcOperLastModifyTime = 
                       cc_oper.llcCcOperLastModifyTime;
         llcCcOperData.llcCcOperLastFailTime = cc_oper.llcCcOperLastFailTime;
         llcCcOperData.llcCcOperLastFailCause = 
                       get_snmp_cc_fail_cause( cc_oper.llcCcOperLastFailCause);
         bcopy(cc_oper.llcCcOperLastFailFRMRInfo, FRMRInfo, FRMR_INFO_LEN);
         FRMRInfo_OS.octet_ptr = FRMRInfo;
         FRMRInfo_OS.length = FRMR_INFO_LEN;
         llcCcOperData.llcCcOperLastFailFRMRInfo = &FRMRInfo_OS;
         llcCcOperData.llcCcOperLastWwCause = 
                       get_snmp_cc_ww_cause( cc_oper.llcCcOperLastWwCause);
         llcCcOperData.ifIndex = lastIfIndex;
         llcCcOperData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcCcOperData.llcSapNumber = cc_oper.index.local_sap;
         tmpLlcCcRMac_OS.length = MAC_ADDR_LEN;
         tmpLlcCcRMac_OS.octet_ptr = tmpLlcCcRMac;
         bcopy( cc_oper.index.rem_mac, tmpLlcCcRMac, MAC_ADDR_LEN);
         llcCcOperData.llcCcRMac = &tmpLlcCcRMac_OS;
         llcCcOperData.llcCcRSap = cc_oper.index.rem_sap;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastLlcSapNumber = llcSapNumber;
         CopyOctetStrings( llcCcRMac, &lastLlcCcRMac_OS, MAC_ADDR_LEN);
         lastLlcCcRSap = lastLlcCcRSap;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcCcOperData.valid);
         result_p = &llcCcOperData;
      }
   }
   return result_p;
}

llcCcStatsEntry_t *
k_llcCcStatsEntry_get (serialNum, contextInfo, nominator, searchType, ifIndex, llcPortVirtualIndex, llcSapNumber, llcCcRMac, llcCcRSap)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long ifIndex;
   long llcPortVirtualIndex;
   long llcSapNumber;
   OctetString * llcCcRMac;
   long llcCcRSap;
{
   static llcCcStatsEntry_t   llcCcStatsData;
   static int                 lastSerialNum = MAXINT;
   static long                lastIfIndex = MAXLONG;
   static long                lastLlcPortVirtualIndex = MAXLONG;
   static long                lastLlcSapNumber = MAXLONG;
   static OctetString         lastLlcCcRMac_OS = {NULL, 0};
   static uchar               lastLlcCcRMac[MAC_ADDR_LEN];
   static long                lastLlcCcRSap = MAXLONG;
   static OctetString         tmpLlcCcRMac_OS = {NULL, 0};
   static uchar               tmpLlcCcRMac[MAC_ADDR_LEN];

   llc_cc_stats_t cc_stats;
   llcCcStatsEntry_t *result_p = NULL;
   
   /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
   if ((serialNum != -1) &&
       (lastSerialNum == serialNum) && 
       (lastIfIndex == ifIndex) &&
       (lastLlcPortVirtualIndex == llcPortVirtualIndex) && 
       (lastLlcSapNumber == llcSapNumber) &&
       (CmpOctetStrings(&lastLlcCcRMac_OS, llcCcRMac) == 0) &&
       (lastLlcCcRSap == llcCcRSap)) {
      result_p = &llcCcStatsData;
   }
   else {
      lastIfIndex = ifIndex;
      lastLlcPortVirtualIndex = llcPortVirtualIndex;
      lastLlcSapNumber = llcSapNumber;
      lastLlcCcRMac_OS.length = llcCcRMac->length;
      lastLlcCcRMac_OS.octet_ptr = lastLlcCcRMac;
      CopyOctetStrings( llcCcRMac, &lastLlcCcRMac_OS, MAC_ADDR_LEN);
      lastLlcCcRSap = llcCcRSap;
      if (get_cc_entry( searchType, &lastIfIndex, &lastLlcPortVirtualIndex, 
                        lastLlcSapNumber, 
                        &lastLlcCcRMac_OS, lastLlcCcRSap, &cc_stats, 
                        LLC_CC_STATS_SIZE, SYSMGT_LLC_CC_STATS)) {
         llcCcStatsData.llcCcStatsLocalBusies = 
                        cc_stats.llcCcStatsLocalBusies ;
         llcCcStatsData.llcCcStatsRemoteBusies = 
                        cc_stats.llcCcStatsRemoteBusies;
         llcCcStatsData.llcCcStatsIFramesIn = cc_stats.llcCcStatsIFramesIn;
         llcCcStatsData.llcCcStatsIFramesOut = cc_stats.llcCcStatsIFramesOut;
         llcCcStatsData.llcCcStatsIOctetsIn = cc_stats.llcCcStatsIOctetsIn;
         llcCcStatsData.llcCcStatsIOctetsOut = cc_stats.llcCcStatsIOctetsOut;
         llcCcStatsData.llcCcStatsSFramesIn = cc_stats.llcCcStatsSFramesIn;
         llcCcStatsData.llcCcStatsSFramesOut = cc_stats.llcCcStatsSFramesOut;
         llcCcStatsData.llcCcStatsRetransmitsOut = 
                        cc_stats.llcCcStatsRetransmitsOut;
         llcCcStatsData.llcCcStatsREJsIn = cc_stats.llcCcStatsREJsIn;
         llcCcStatsData.llcCcStatsREJsOut = cc_stats.llcCcStatsREJsOut;
         llcCcStatsData.llcCcStatsWwCount = cc_stats.llcCcStatsWwCount;
         llcCcStatsData.ifIndex = lastIfIndex;
         llcCcStatsData.llcPortVirtualIndex = lastLlcPortVirtualIndex;
         llcCcStatsData.llcSapNumber = cc_stats.index.local_sap;
         tmpLlcCcRMac_OS.length = MAC_ADDR_LEN;
         tmpLlcCcRMac_OS.octet_ptr = tmpLlcCcRMac;
         bcopy( cc_stats.index.rem_mac, tmpLlcCcRMac, MAC_ADDR_LEN);
         llcCcStatsData.llcCcRMac = &tmpLlcCcRMac_OS;
         llcCcStatsData.llcCcRSap = cc_stats.index.rem_sap;
         lastIfIndex = ifIndex;
         lastLlcPortVirtualIndex = llcPortVirtualIndex;
         lastLlcSapNumber = llcSapNumber;
         CopyOctetStrings( llcCcRMac, &lastLlcCcRMac_OS, MAC_ADDR_LEN);
         lastLlcCcRSap = lastLlcCcRSap;
         lastSerialNum = serialNum;
         SET_ALL_VALID( llcCcStatsData.valid);
         result_p = &llcCcStatsData;
      }
   }
   return result_p;
}

/* Parse chain for SNA LLC2 snmp-server host command 
 */
LINK_EXIT(cfg_snmp_host_snallc2_exit, no_alt);
KEYWORD_OR(cfg_snmp_host_snallc2, cfg_snmp_host_snallc2_exit, NONE,
	   OBJ(int,1), (1<<TRAP_LLC2),
	   "llc2", "Allow llc2 event traps (currently CIP CSNA only)", 
           PRIV_CONF);
LINK_POINT(cfg_snmp_host_snallc2_entry, cfg_snmp_host_snallc2);

/* Parse chain registration array for SNA LLC2 Trap Control
 */
static parser_extension_request snallc2_chain_init_table[] = {
    { PARSE_ADD_CFG_SNMP_HOST_CMD, &pname(cfg_snmp_host_snallc2_entry) },
    { PARSE_ADD_CFG_SNMP_HOST_EXIT,
	  (dynamic_transition *) &pname(cfg_snmp_host_snallc2_exit) },
    { PARSE_LIST_END, NULL }
};

static void snallc2_snmp_host_nvgen (ulong flags) {
    nv_add(flags & (1 << TRAP_LLC2), " llc2");
}

static char * trap_snaLlcStatus = "ciscoSnaLlcMIB.2.0.1";

static void init_snallcmib (subsystype *subsys)
{
    load_mib(snallcmib_OidList, snallcmib_OidListNum);
    load_oid(snallcmib_oid_table);
    register_snmp_trap(TRAP_LLC2, trap_snaLlcStatus);
    parser_add_command_list(snallc2_chain_init_table, "llc2");
    reg_add_snmp_host_nvgen(snallc2_snmp_host_nvgen, "snallc2_snmp_host_nvgen");
}

static const OID enterprise_snaLlcTrap = {LNsnaLlcMIBNotificationPrefix, 
                                 (ulong *)IDsnaLlcMIBNotificationPrefix};

#define SNA_LLC_STATUS_TRAP_VBCOUNT 4
#define SNA_LLC_STATUS_TRAP_NUM 1

static const OID varbinds_snaLlcStatus[SNA_LLC_STATUS_TRAP_VBCOUNT] = { 
     OIDconversion(llcCcOperState),
     OIDconversion(llcCcOperLastFailTime),
     OIDconversion(llcCcOperLastFailCause),
     OIDconversion(llcCcOperLastFailFRMRInfo)
};

/* Currently, this trap is only called from the LLC2 stack on the CIP.
 * When (if) this trap is also called by the llc2 stack on the RP, this
 * should become a registry call.  At that time, the arguments passed to
 * this function may need to be different, since the RP LLC2 stack may not
 * have the same structures.  The CIP call to this function occurs in
 * sr_cipcsnamib.c.  If necessary, parameters could be remapped from CIP 
 * types/codes to common types/codes to enable both stacks to use this call
 * to send the trap.
 */
void snaLlcStatusChangeTrap (hwidbtype * hwidb, 
                             cip_llctype *csna_data,
                             ushort code)
{
  int           idx;
  OID           indexOID[SNA_LLC_STATUS_TRAP_VBCOUNT];
  ulong         index[5 + IEEEBYTES];
  vb_value      dataP[SNA_LLC_STATUS_TRAP_VBCOUNT];
  int           failCause;
  ulong         lastFailTime;
  long          operState;

  /* Get the timestamp and state */
  GET_TIMESTAMP32(lastFailTime); 
  operState = get_snmp_llc_cc_state( (long) csna_data->cc_state);

  /* Fill in the indexing information */
  index[0] = hwidb->snmp_if_index;
  index[1] = csna_data->virtual_index;
  index[2] = csna_data->local_sap;
  index[3] = IEEEBYTES;

  /* Can't bcopy from 'char' array to 'ulong' array */
  for (idx = 0; idx < IEEEBYTES; ++idx)
       index[idx + 4] = (ulong) csna_data->remote_macaddr[idx];

  index[4 + IEEEBYTES] = csna_data->remote_sap;

  for (idx = 0; idx < SNA_LLC_STATUS_TRAP_VBCOUNT; idx++) {
       indexOID[idx].length = 5 + IEEEBYTES;
       indexOID[idx].oid_ptr = index;
  }

  /* Events for CIPCSNA do NOT map very well to these! */
  switch (code) {
    case FRMR_TXD_EVENT:
         failCause = D_llcCcOperLastFailCause_txFRMR;
         break;
    case FRMR_RXD_EVENT:
         failCause = D_llcCcOperLastFailCause_rxFRMR;
         break;
    case LINK_LOST_EVENT:
         failCause = D_llcCcOperLastFailCause_retriesExpired;
         break;
    case DM_RXD_EVENT:
    case SABME_RXD_IN_ABME_EVENT:
    default:
         failCause = D_llcCcOperLastFailCause_undefined;
         break;  
  }

  /* Fill in the data pointers */
  dataP[0].value = &operState;
  dataP[0].type = INTEGER_TYPE;
  dataP[1].value = &lastFailTime;
  dataP[1].type = TIME_TICKS_TYPE;
  dataP[2].value = &failCause;
  dataP[2].type = INTEGER_TYPE;

  /* MIB specifies 3 bytes instead of FRMR_INFO_LEN (5) */
  dataP[3].value = MakeOctetString(csna_data->frmr, 3); 
  dataP[3].type = OCTET_PRIM_TYPE;

  do_snallc_trap(varbinds_snaLlcStatus, &enterprise_snaLlcTrap, 
                 trap_snaLlcStatus, indexOID, 
                 SNA_LLC_STATUS_TRAP_VBCOUNT, ENTERPRISE_TRAP, 
                 SNA_LLC_STATUS_TRAP_NUM, dataP);
}


void do_snallc_trap (const OID*       vbList,
                     const OID*       entOID,
                     char*            v2TrapOID,
                     OID*             indexOID,
                     int              vbLength,
                     int              genericTrapNum,
                     int              specificTrapNum,
                     vb_value         dataP[] )
{
  int       vbIndex;
  VarBind   *previous = NULL;
  VarBind   *current;
  VarBind   *varBinds = NULL;

  for (vbIndex = 0; vbIndex < vbLength; vbIndex++) {  
       current = MakeVarBindWithValue((OID *) &vbList[vbIndex],
				      &indexOID[vbIndex],
				      dataP[vbIndex].type,
				      dataP[vbIndex].value);
       if (current) {
	   /*
	    * if this is the first VarBind, make it the head of
	    * the list, otherwise attach it to the end
	    */
	   if (varBinds == NULL)
	       varBinds = current;
	   else
	       previous->next_var = current;

	   current->next_var = NULL;
	   previous = current;
       } else {
	   FreeVarBindList(varBinds);
	   varBinds = NULL;
       }
  }

  /* Make the call to generate the trap!  It will free the varbinds. */
  do_trap(genericTrapNum, specificTrapNum, varBinds, (OID *)entOID, v2TrapOID);
}

/*
 * snallcmib subsystem header
 */
#define MAJVERSION_snallcmib 1
#define MINVERSION_snallcmib 0
#define EDITVERSION_snallcmib 0

SUBSYS_HEADER(snallcmib,
              MAJVERSION_snallcmib,
              MINVERSION_snallcmib,
              EDITVERSION_snallcmib,
              init_snallcmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL, NULL);
      
