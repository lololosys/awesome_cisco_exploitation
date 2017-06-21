/* $Id: sr_isdnmib.c,v 3.10.6.5 1996/08/28 12:54:15 thille Exp $
 * $Source: /release/112/cvs/Xsys/isdn/sr_isdnmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1995, Bibek A Das (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_isdnmib.c,v $
 * Revision 3.10.6.5  1996/08/28  12:54:15  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.6.4  1996/07/01  18:45:15  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.10.6.3  1996/05/07  01:40:01  bdas
 * CSCdi53120:  spurious access in isdn_neighbor_update+0x304
 * Branch: California_branch
 *
 * Revision 3.10.6.2  1996/04/27  05:54:46  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.7.8.3  1996/04/26  01:58:23  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.8.2  1996/04/17  03:05:17  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.8.1  1996/02/23  20:24:48  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.10.6.1  1996/03/28  02:37:39  bdas
 * CSCdi40727:  more than one demandNbrEntry for a single neighbor in ISDN
 * MIB
 * Branch: California_branch
 *
 * Revision 3.10  1996/03/05  19:40:04  bdas
 * CSCdi49512:  ISDN MIB trap needs to specify the direction of the call
 *
 * Revision 3.9  1996/02/12  20:32:14  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.8  1996/01/24  22:04:53  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.7  1995/12/27  23:48:23  bdas
 * CSCdi46167:  ISDN MIB syslog messages contain incorrect duration
 *
 * Revision 3.6  1995/12/14  08:26:30  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.5  1995/11/29  18:15:00  bdas
 * CSCdi44122:  ISDN MIB needs to interoperate with HP OpenView
 *
 * Revision 3.4  1995/11/18  00:16:43  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.3  1995/11/17  17:42:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:50:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:15:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/10/21  07:32:39  bdas
 * CSCdi38453:  ISDN traps need corresponding syslog messages
 *
 * Revision 2.5  1995/10/21  06:35:24  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.4  1995/06/30  00:20:31  bdas
 * CSCdi36243:  isdn mib trap sent for every failed call retry attempt.
 * Send the trap just once, when the first call attempt fails.
 *
 * Revision 2.3  1995/06/28  09:28:22  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/15  23:10:25  bdas
 * CSCdi33538:  incomplete/wrong object instances returned; ISDN MIB,
 * Call History Mib.
 *
 * Revision 2.1  1995/06/07  21:17:35  hampton
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
#include "../snmp/snmp_api.h"
#include "sr_isdnmib.h"
#include "sr_isdnmib-mib.h"
#include "sys_registry.h"
#include "subsys.h"
#include "packet.h"
#include "config.h"
#include "../snmp/snmp_registry.h"
#include "interface_private.h"
#include "../parser/parser_defs_snmp.h"
#include "../les/isdn_bri.h"
#include "isdn_registry.h"
#include "address.h"
#include "../wan/dialer.h"
#include "../wan/isdn.h"
#include "../isdn/tune.h"
#include "../h/logger.h"
#include "../isdn/msg_isdn.c"		/* Not a typo, see logger.h */


#define callQ struct callQ_
callQ {
    callQ              *next;
    demandNbrEntry_t   *entry;
    sys_timestamp      age_timer;
    sys_timestamp      trap_timer;
};

static queuetype        isdnNeighborQ;

static neighbor_info_type neighbor_info[MAX_NEIGHBORS][MAX_DSL];

static boolean isdnTrapsEnabled;

/*
 * forward reference
 */

static void isdn_call_ageout(void);
static callQ * callQget(int searchType, long d_chan_index, long nbr_index);
static void callQinsert(callQ   *newcall);
static void isdn_send_trap(void);
static void get_more_info(demandNbrEntry_t *, int);
static long get_nbr_id (char *, int);

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the demandNbrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
demandNbrEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    demandNbrEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            demandNbrPhysIf;
    long            demandNbrId;
    static OctetString     sparse_string;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 2 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */
    data = NULL;
    dp = NULL;
    sparse_string.length = 0;
    sparse_string.octet_ptr = NULL;
 

    if (searchType == EXACT) {
        if (instLength != 2) {
          return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &demandNbrId, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &demandNbrPhysIf, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_demandNbrEntry_get(serialNum, contextInfo, arg ,searchType, demandNbrPhysIf, demandNbrId)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->demandNbrPhysIf;
        inst.oid_ptr[1] = data->demandNbrId;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {


#ifdef I_demandNbrName
    case I_demandNbrName:
    if ( VALID(I_demandNbrName, data->valid)) 
       dp =  (void *) CloneOctetString(data->demandNbrName);
       break;
#endif /* I_demandNbrName */

#ifdef I_demandNbrAddress
    case I_demandNbrAddress:
    if ( VALID(I_demandNbrAddress, data->valid)) 
       dp =  (void *) CloneOctetString(data->demandNbrAddress);
       break;
#endif /* I_demandNbrAddress */
#ifdef I_demandNbrClearReason
    case I_demandNbrClearReason:
    if ( VALID(I_demandNbrClearReason, data->valid)) 
       dp = (void *) (CloneOctetString(data->demandNbrClearReason));
       break;
#endif /* I_demandNbrClearReason */

#ifdef I_demandNbrClearCode
    case I_demandNbrClearCode:
    if ( VALID(I_demandNbrClearCode, data->valid)) 
       dp = (void *) (CloneOctetString(data->demandNbrClearCode));
       break;
#endif /* I_demandNbrClearCode */

#ifdef I_demandNbrLogIf
    case I_demandNbrLogIf:
#endif /* I_demandNbrLogIf */
#ifdef I_demandNbrPermission
    case I_demandNbrPermission:
#endif /* I_demandNbrPermission */

#ifdef I_demandNbrMaxDuration
    case I_demandNbrMaxDuration:
#endif /* I_demandNbrMaxDuration */

#ifdef I_demandNbrLastDuration
    case I_demandNbrLastDuration:
#endif /* I_demandNbrLastDuration */


#ifdef I_demandNbrSuccessCalls
    case I_demandNbrSuccessCalls:
#endif /* I_demandNbrSuccessCalls */

#ifdef I_demandNbrFailCalls
    case I_demandNbrFailCalls:
#endif /* I_demandNbrFailCalls */

#ifdef I_demandNbrAcceptCalls
    case I_demandNbrAcceptCalls:
#endif /* I_demandNbrAcceptCalls */

#ifdef I_demandNbrRefuseCalls
    case I_demandNbrRefuseCalls:
#endif /* I_demandNbrRefuseCalls */

#ifdef I_demandNbrLastAttemptTime
    case I_demandNbrLastAttemptTime:
#endif /* I_demandNbrLastAttemptTime */

#ifdef I_demandNbrCallOrigin
     case I_demandNbrCallOrigin:
#endif /* I_demandNbrCallOrigin */
  
#ifdef I_demandNbrStatus
    case I_demandNbrStatus:
#endif /* I_demandNbrStatus */
	 dp = SR_OFFSET(data, arg);
	 break;

     }      /* switch */
     if  (dp == NULL)  {
       if (snmp_nosparse) {
            switch (arg) {
            case I_demandNbrName:
            case I_demandNbrAddress:
            case I_demandNbrClearReason:
            case I_demandNbrClearCode:
               dp = CloneOctetString(&sparse_string);
                 return (MakeVarBind(object, &inst, dp));
            case I_demandNbrLogIf:
            case I_demandNbrLastDuration:
            case I_demandNbrLastAttemptTime:
               dp = 0;
                 return (MakeVarBind(object, &inst, dp));
            default:
               return ((VarBind *) NULL);
            }
         } else
           return ((VarBind *) NULL);
     } else
         return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the demandNbrEntry data object.
 *---------------------------------------------------------------------*/
void
demandNbrEntry_free (demandNbrEntry_t *data)
{
   if (data != NULL) {
       if (data->demandNbrName != NULL)
           FreeOctetString(data->demandNbrName);
       if (data->demandNbrAddress != NULL)
           FreeOctetString(data->demandNbrAddress);
       if (data->demandNbrClearReason != NULL)
           FreeOctetString(data->demandNbrClearReason);
       if (data->demandNbrClearCode != NULL)
           FreeOctetString(data->demandNbrClearCode);

       free (data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after demandNbrEntry set/undo
 *---------------------------------------------------------------------*/
static int
demandNbrEntry_cleanup (doList_t *trash)
{
   demandNbrEntry_free(trash->data);
#ifdef SR_SNMPv2
   demandNbrEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the demandNbrEntry family
 *---------------------------------------------------------------------*/
demandNbrEntry_t *
Clone_demandNbrEntry (demandNbrEntry_t *demandNbrEntry)
{
    demandNbrEntry_t *data;

    if ((data = malloc(sizeof(demandNbrEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (demandNbrEntry), sizeof(demandNbrEntry_t));
    if ( VALID(I_demandNbrName, demandNbrEntry->valid)) 
        data->demandNbrName = CloneOctetString(demandNbrEntry->demandNbrName);
    if ( VALID(I_demandNbrAddress, demandNbrEntry->valid) ) 
        data->demandNbrAddress = CloneOctetString(demandNbrEntry->demandNbrAddress);
    if (VALID(I_demandNbrClearReason, demandNbrEntry->valid) ) 
        data->demandNbrClearReason = CloneOctetString(demandNbrEntry->demandNbrClearReason);
    if (VALID(I_demandNbrClearCode, demandNbrEntry->valid)) 
        data->demandNbrClearCode = CloneOctetString(demandNbrEntry->demandNbrClearCode);

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
demandNbrEntry_test (
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
    demandNbrEntry_t     *demandNbrEntry;
    long           demandNbrPhysIf;
    long           demandNbrId;
    int            retval;
    boolean         entry_found;

    entry_found = FALSE;
   /*
    * Validate the object instance
    *
    */
    if (instLength != 2) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &demandNbrPhysIf, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &demandNbrId, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    demandNbrEntry = k_demandNbrEntry_get(-1, contextInfo, -1, EXACT, demandNbrPhysIf, demandNbrId);

    if (demandNbrEntry != NULL)
	entry_found = TRUE;
    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == demandNbrEntry_set) &&
            (((demandNbrEntry_t *) (dp->data)) != NULL) &&
            (((demandNbrEntry_t *) (dp->data))->demandNbrPhysIf == demandNbrPhysIf) &&
            (((demandNbrEntry_t *) (dp->data))->demandNbrId == demandNbrId) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = demandNbrEntry_set;
        dp->cleanupMethod = demandNbrEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_demandNbrEntry_UNDO
        dp->undoMethod = demandNbrEntry_undo;
#else /* SR_demandNbrEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_demandNbrEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (demandNbrEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_demandNbrEntry(demandNbrEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_demandNbrEntry(demandNbrEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(demandNbrEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(demandNbrEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((demandNbrEntry_t *) (dp->data))->demandNbrPhysIf = (demandNbrPhysIf);
                SET_VALID(I_demandNbrPhysIf, ((demandNbrEntry_t *) (dp->data))->valid);

                ((demandNbrEntry_t *) (dp->data))->demandNbrId = (demandNbrId);
                SET_VALID(I_demandNbrId, ((demandNbrEntry_t *) (dp->data))->valid);

                error_status = k_demandNbrEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_demandNbrLogIf
   case I_demandNbrLogIf:

     ((demandNbrEntry_t *) (dp->data))->demandNbrLogIf = value->sl_value;
     break;
#endif /* I_demandNbrLogIf */

#ifdef I_demandNbrName
   case I_demandNbrName:

     if (((demandNbrEntry_t *) (dp->data))->demandNbrName != NULL) {
        FreeOctetString(((demandNbrEntry_t *) (dp->data))->demandNbrName);
     }

     ((demandNbrEntry_t *) (dp->data))->demandNbrName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_demandNbrName */

#ifdef I_demandNbrAddress
   case I_demandNbrAddress:

     if (((demandNbrEntry_t *) (dp->data))->demandNbrAddress != NULL) {
        FreeOctetString(((demandNbrEntry_t *) (dp->data))->demandNbrAddress);
     }

     ((demandNbrEntry_t *) (dp->data))->demandNbrAddress = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_demandNbrAddress */

#ifdef I_demandNbrPermission
   case I_demandNbrPermission:

     if ((value->sl_value < 1) || (value->sl_value > 3)) {
         return WRONG_VALUE_ERROR;
     }

     ((demandNbrEntry_t *) (dp->data))->demandNbrPermission = value->sl_value;
     break;
#endif /* I_demandNbrPermission */

#ifdef I_demandNbrMaxDuration
   case I_demandNbrMaxDuration:

     ((demandNbrEntry_t *) (dp->data))->demandNbrMaxDuration = value->sl_value;
     break;
#endif /* I_demandNbrMaxDuration */

#ifdef I_demandNbrStatus
   case I_demandNbrStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_demandNbrStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (demandNbrEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_demandNbrStatus_notInService) || (value->sl_value == D_demandNbrStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_demandNbrStatus_createAndGo) || (value->sl_value == D_demandNbrStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((demandNbrEntry_t *) (dp->data))->demandNbrStatus = value->sl_value;
     retval = set_row_status(value->sl_value,
			     entry_found ? 1 : -1,
			     &(dp->state),
			     &((demandNbrEntry_t *) (dp->data))->demandNbrStatus);
     if (retval != NO_ERROR)
         return (retval);
     break;
#endif /* I_demandNbrStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in demandNbrEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_demandNbrEntry_test */
   error_status = k_demandNbrEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((demandNbrEntry_t *) (dp->data))->valid);
        error_status = k_demandNbrEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
demandNbrEntry_set (doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_demandNbrEntry_set((demandNbrEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */





demandNbrEntry_t *
k_demandNbrEntry_get (serialNum, contextInfo, nominator, searchType, demandNbrPhysIf, demandNbrId)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long demandNbrPhysIf;
   long demandNbrId;
{

    callQ      *data;

    while (TRUE) { 
       data = callQget(searchType, demandNbrPhysIf, demandNbrId);

       if (data == NULL) 
	  return (NULL);


       if ((searchType == EXACT) || (VALID(nominator, data->entry->valid)) ||
	    snmp_nosparse) {
           return(data->entry);
       }

       demandNbrPhysIf = data->entry->demandNbrPhysIf;
       demandNbrId = data->entry->demandNbrId;
       if ( ++demandNbrId >= MAX_NEIGHBORS) {
	   demandNbrId = FIRST_NEIGHBOR;
	   if (++demandNbrPhysIf > MAX_IFINDEX)
              return (NULL);
       }

     }   
}

#ifdef SETS
int
k_demandNbrEntry_test (object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{
    switch (object->nominator) {
#ifdef I_demandNbrLogIf
   case I_demandNbrLogIf:
     if ((value->sl_value < 1) || (value->sl_value > MAX_IFINDEX)) {
         return WRONG_VALUE_ERROR;
     }
     break;
#endif /* I_demandNbrLogIf */

#ifdef I_demandNbrMaxDuration
   case I_demandNbrMaxDuration:
     if ((value->sl_value < 1) || (value->sl_value > MAX_DURATION)) 
         return WRONG_VALUE_ERROR;
     break;
#endif /* I_demandNbrMaxDuration */
#ifdef I_demandNbrName
   case I_demandNbrName:
         if (!IsDisplayString(value->os_value))
            return WRONG_VALUE_ERROR;
     break;
#endif /* I_demandNbrName */

#ifdef I_demandNbrAddress
   case I_demandNbrAddress:
         if (!IsDisplayString(value->os_value))
            return WRONG_VALUE_ERROR;
     break;
#endif /* I_demandNbrAddress */
    default:
     break;
   }
  return NO_ERROR;
}

int
k_demandNbrEntry_ready (object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{
    boolean valid_data;
    int dsl;

    demandNbrEntry_t *data = (demandNbrEntry_t *) (dp->data);
    
    for (dsl = 0; dsl < MAX_DSL; dsl++) {
       if ((isdn_idblist[dsl]) &&
	  (isdn_idblist[dsl]->snmp_if_index == 
	     ((demandNbrEntry_t *) (dp->data))->demandNbrPhysIf))
	     break;
    }
    if (dsl == MAX_DSL)
         return INCONSISTENT_NAME_ERROR;
    if (dsl >= MAX_NEIGHBORS)
         return INCONSISTENT_NAME_ERROR;

    if ( VALID(I_demandNbrLogIf, data->valid) &&
        VALID(I_demandNbrAddress, data->valid) &&
        VALID(I_demandNbrPermission, data->valid) &&
        VALID(I_demandNbrMaxDuration, data->valid) &&
        VALID(I_demandNbrStatus, data->valid) )
	  valid_data = TRUE;
    else
	valid_data = FALSE;

    switch (data->demandNbrStatus) {
       case D_demandNbrStatus_active :
	   if (valid_data)
              dp->state = ADD_MODIFY;
           else
              dp->state = NOT_READY_STATE;
	   break;
       case D_demandNbrStatus_notInService:
	   if (valid_data)
              dp->state = ADD_MODIFY;
           else
              dp->state = NOT_READY_STATE;
	   break;
       case D_demandNbrStatus_notReady:
	   if (valid_data)
	      data->demandNbrStatus = D_demandNbrStatus_notInService;
           if (VALID(I_demandNbrStatus, data->valid) )
              dp->state = ADD_MODIFY;
	   break;
       case D_demandNbrStatus_createAndGo:
	   if (valid_data)
              dp->state = ADD_MODIFY;
           else
	      dp->state = CREATE_AND_GO_STATE;
	   break;
       case D_demandNbrStatus_createAndWait:
	   if (valid_data) 
	      data->demandNbrStatus = D_demandNbrStatus_notInService;
           else 
	      data->demandNbrStatus = D_demandNbrStatus_notReady;
           dp->state = ADD_MODIFY;
	   break;
       case D_demandNbrStatus_destroy:
           dp->state = DELETE;
	   break;
       default :
           return (INCONSISTENT_VALUE_ERROR);
	   break;
    } 
    return NO_ERROR;
}

int
k_demandNbrEntry_set_defaults (dp)
    doList_t       *dp;
{
    demandNbrEntry_t *data = (demandNbrEntry_t *) (dp->data);
    data->demandNbrPermission = D_demandNbrPermission_weCanCallEachOther;
    SET_VALID(I_demandNbrPermission, ((demandNbrEntry_t *) (dp->data))->valid);
    data->demandNbrMaxDuration = MAX_DURATION;
    SET_VALID(I_demandNbrMaxDuration, ((demandNbrEntry_t *) (dp->data))->valid);
    data->demandNbrStatus = D_demandNbrStatus_notReady;
    /*
     * Do not set Status to VALID as this will create the row
     */
    return NO_ERROR;
}



int
k_demandNbrEntry_set (demandNbrEntry_t *data,
		      ContextInfo      *contextInfo,
		      int		function)
{
    callQ	       *current;
    demandNbrEntry_t   *entry;

    current = callQget(EXACT, data->demandNbrPhysIf, data->demandNbrId);
    switch (function) {
    case DELETE:
	if (current) {

	    /*
	     * we've located the appropriate callQ record
	     */
	     p_unqueue(&isdnNeighborQ, current);
	     demandNbrEntry_free(current->entry);
	     free(current);
	}
	return (NO_ERROR);
	
    case ADD_MODIFY:
	if (current == NULL) {
	    /*
	     * adding a new row
	     */
	    current = malloc(sizeof(callQ));
	    if (current == NULL)
		return (RESOURCE_UNAVAILABLE_ERROR);

	    entry = malloc(sizeof(demandNbrEntry_t));
	    if (entry == NULL) {
		free(current);
		return (RESOURCE_UNAVAILABLE_ERROR);
	    }
	    current->next = NULL;
	    current->entry = entry;
	    entry->demandNbrPhysIf = data->demandNbrPhysIf;
	    entry->demandNbrId = data->demandNbrId;
	    entry->demandNbrStatus = D_demandNbrStatus_notInService;
	    callQinsert(current);
	} else {
	    entry = current->entry;
	}

	/*
	 * entry is being modified, so restart the ageout timer
	 */
	TIMER_START(current->age_timer, DEFAULT_NEIGHBOR_ENTRY_AGEOUT);

	if (VALID(I_demandNbrLogIf, data->valid)) {
	    SET_VALID(I_demandNbrLogIf, entry->valid);
	    entry->demandNbrLogIf = data->demandNbrLogIf;
	}

	if (VALID(I_demandNbrName, data->valid)) {
	    SET_VALID(I_demandNbrName, entry->valid);
	    if (entry->demandNbrName != NULL) {
		FreeOctetString(entry->demandNbrName);
	    }
	    entry->demandNbrName = CloneOctetString(data->demandNbrName);
	}

	if (VALID(I_demandNbrAddress, data->valid)) {
	    SET_VALID(I_demandNbrAddress, entry->valid);
	    if (entry->demandNbrAddress != NULL) {
		FreeOctetString(entry->demandNbrAddress);
	    }
	    entry->demandNbrAddress = CloneOctetString(data->demandNbrAddress);
	}

	if (VALID(I_demandNbrPermission, data->valid)) {
	    SET_VALID(I_demandNbrPermission, entry->valid);
	    entry->demandNbrPermission = data->demandNbrPermission;
	}

	if (VALID(I_demandNbrMaxDuration, data->valid)) {
	    SET_VALID(I_demandNbrMaxDuration, entry->valid);
	    entry->demandNbrMaxDuration = data->demandNbrMaxDuration;
	}

	if (VALID(I_demandNbrStatus, data->valid)) {
	    /*
	     * turn createAndGo into active since they're the same
	     * thing at this point
	     */
	    if (data->demandNbrStatus == D_demandNbrStatus_createAndGo)
		data->demandNbrStatus = D_demandNbrStatus_active;

	    SET_VALID(I_demandNbrStatus, entry->valid);
	    entry->demandNbrStatus = data->demandNbrStatus;

	}
	return (NO_ERROR);
    }
    return (GEN_ERROR);
}

#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_demandNbrEntry_UNDO
/* add #define SR_demandNbrEntry_UNDO in isdnmib.h to
 * include the undo routine for the demandNbrEntry family.
 */
int
demandNbrEntry_undo (doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_demandNbrEntry_UNDO */
#endif /* SR_SNMPv2 */




/*
 * callQget:  grab the requested isdnNeighborQ entry
 */
static callQ *
callQget (int	searchType,
	  long	d_chan_index,
	  long  nbr_index)
{
    leveltype		level;
    callQ	       *test;
    demandNbrEntry_t   *entry;

    /*
     * don't allow the timeout task access to the Q
     */
    level = raise_interrupt_level(ALL_DISABLE);

    for (test = isdnNeighborQ.qhead; test; test = test->next) {

	entry = test->entry;
	if ( (entry->demandNbrPhysIf == d_chan_index) &&
	      (entry->demandNbrId == nbr_index) )
           break;
        if (searchType == NEXT) {
	      if  (entry->demandNbrPhysIf > d_chan_index) 
	          break;
              else {
		 if (entry->demandNbrPhysIf == d_chan_index)
	            if (entry->demandNbrId > nbr_index) 
	               break;
              }
        }
        if (searchType == EXACT) {
            if ((entry->demandNbrPhysIf > d_chan_index) ||
	         ((entry->demandNbrPhysIf == d_chan_index)  &&
		    (entry->demandNbrId > nbr_index))) {
	         test = NULL;
		 break;
            } 
        }

    }

    /*
     * re-allow access to the isdnNeighborQ
     */
    reset_interrupt_level(level);

    return (test);
}


/*
 * call entry ageout function
 * called via the ONEMIN registry
 */
static void
isdn_call_ageout (void)
{
    leveltype		level;
    callQ	       *test;
    callQ	       *next;

    /*
     * make sure we have exclusive access to the isdnNeighborQ
     */
    level = raise_interrupt_level(ALL_DISABLE);

    for (test = isdnNeighborQ.qhead; test; test = next) {
	next = test->next;

	if (TIMER_RUNNING(test->age_timer) && AWAKE(test->age_timer)) {
	    unqueue(&isdnNeighborQ, test);
	    demandNbrEntry_free(test->entry);
	    free(test);
	}
    }

    /*
     * re-allow access to the isdnNeighborQ
     */
    reset_interrupt_level(level);

}

/*
 * callQinsert:  insert a new entry 
 * Maintain the SNMP lexicographical order of the Q based on the
 * values of demandNbrPhysIf and demandNbrId.
 */
static void
callQinsert (callQ     *newcall)
{
    leveltype		level;
    callQ	       *prev;
    callQ	       *test;
    long		d_chan_index, nbr_index;


    d_chan_index = newcall->entry->demandNbrPhysIf;
    nbr_index = newcall->entry->demandNbrId;
    prev = NULL;
    level = raise_interrupt_level(ALL_DISABLE);
    test = isdnNeighborQ.qhead;
    while (test) {
	if (d_chan_index < test->entry->demandNbrPhysIf)  
	   break;
        else {
	   if (d_chan_index == test->entry->demandNbrPhysIf)  
	      if (nbr_index <= test->entry->demandNbrId) 
	         break;
        }
        prev = test;
	test = test->next;
    }
    insqueue(&isdnNeighborQ, newcall, prev);

    /*
     * re-allow access to the isdnNeighborQ
     */
    reset_interrupt_level(level);

    return;
}

/*
 * isdn_trap_cfg_nvwrt -- nvwrite for the isdn trap enabled command
 */
static void
isdn_trap_cfg_nvwrt (parseinfo *csb)
{
    nv_write(isdnTrapsEnabled, "%s traps isdn", csb->nv_command);
}

/*
 * isdn_trap_cfg_set -- set the isdnTrapsEnabled through config cmd
 */
static void
isdn_trap_cfg_set (boolean enable, uint subsys, uint traps)
{
    if ((subsys & (1 << TRAP_ISDN)))
        isdnTrapsEnabled = enable; 
}
void get_more_info(demandNbrEntry_t *entry, int dsl)
{
  int  channel;
  hwidbtype *idb, *netidb;
  dialerdbtype *ddb;
  isdn_bri_t *isdn_info;
  char active_cause[] = "Call is Active";
  char active_code[] = "Connected";

  for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
     idb = isdn_idblist[dsl]->isdn_info->b_chan_idb[channel];
     if (idb == NULL)
         continue;
     isdn_info = idb->isdn_info;
     ddb = get_dialerdb(idb);
     if ((idb->isdn_info == NULL) || (ddb == NULL))
         continue;
     if ( (isdn_info->call_connected) &&
        (idb->snmp_if_index == entry->demandNbrLogIf) ) {
        if ((ddb->number_called != NULL) && (ddb->number_called[0] != '\0') &&
           (!isdn_info->outgoing) && (!VALID(I_demandNbrAddress, entry->valid))){
               entry->demandNbrAddress = MakeOctetString(ddb->number_called, strlen(ddb->number_called));
               SET_VALID(I_demandNbrAddress, entry->valid);
               entry->demandNbrId = get_nbr_id(ddb->number_called, dsl);
               SET_VALID(I_demandNbrId, entry->valid);
        }
        if ((ddb->remote_name != NULL) && (ddb->remote_name[0] != '\0') &&
            (!VALID(I_demandNbrName, entry->valid))) {
             entry->demandNbrName = MakeOctetString(ddb->remote_name, strlen(ddb->remote_name));
             SET_VALID(I_demandNbrName, entry->valid);
        }
        entry->demandNbrClearReason = MakeOctetString(active_cause, strlen(active_cause));
        SET_VALID(I_demandNbrClearReason, entry->valid);
        entry->demandNbrClearCode = MakeOctetString(active_code, strlen(active_code));
        SET_VALID(I_demandNbrClearCode, entry->valid);
	netidb = get_netidb(idb);
	if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(netidb)) {
	    if (*ddb->number_called || *ddb->remote_name) {
		errmsg(&msgsym(CONNECT, ISDN), idb->hw_namestring,
		       ddb->number_called, ddb->remote_name);
	    } else {	
		errmsg(&msgsym(CONNECT, ISDN),
		       idb->hw_namestring, "unknown", "");
	    }
	}
        break;
    }
  }
}

#define CALL_INFO_VARBIND_COUNT 7
#define CALL_INFO_TRAP 2

static const OID trapOID = 
    {LNdemandNbrCallDetails, (ulong *)IDdemandNbrCallDetails};
static const OID enterpriseOID =
    {LNdemandNbrCallDetails - 2, (ulong *)IDdemandNbrCallDetails};
static const OID callInfo_trap_varbinds[CALL_INFO_VARBIND_COUNT] = {
    {LNdemandNbrLogIf, (ulong *)IDdemandNbrLogIf},
    {LNdemandNbrName, (ulong *)IDdemandNbrName}, 
    {LNdemandNbrAddress, (ulong *)IDdemandNbrAddress},
    {LNdemandNbrLastDuration, (ulong *)IDdemandNbrLastDuration},
    {LNdemandNbrClearReason, (ulong *)IDdemandNbrClearReason},
    {LNdemandNbrClearCode, (ulong *)IDdemandNbrClearCode}, 
    {LNdemandNbrCallOrigin, (ulong *)IDdemandNbrCallOrigin} };
/*********************************************************
demandNbrCallDetails NOTIFICATION-TYPE
 	OBJECTS {
 		demandNbrLogIf,
 		demandNbrName,
 		demandNbrAddress,
 		demandNbrLastDuration,
 		demandNbrClearReason,
 		demandNbrClearCode,
                demandNbrCallOrigin
 	}
 	STATUS	current
 	DESCRIPTION
           "This trap/inform is sent to the manager whenever a call
            connects, or  clears, or a failed call attempt is determined to
            have ultimately failed. In the event that call retry is active,
            then this is after all retry attempts have failed. However,
            only one such trap is sent in between successful call
            attempts; subsequent call attempts result in no trap."
      ::= { ciscoIsdnMibTraps 2 } 
***********************************************************/

static void
demandNbrCallDetails_trap (demandNbrEntry_t *entry)
{
    int		vbIndex;
    OID	        *vbList[CALL_INFO_VARBIND_COUNT+1];
    OID         instanceOID;
    ulong       instance[2];
    char        trap_oid_str[200]; 
    
    /*
     * create the OID * array
     */ 
     if (MakeDotFromOID((OID *)&trapOID, trap_oid_str) == -1) {
        return;
     } 

    /* All the trap varbind objects have an instance described by
     * INDEX { demandNbrPhysIf, demandNbrId }
     */
     instance[0] = entry->demandNbrPhysIf;
     instance[1] = entry->demandNbrId;
     instanceOID.oid_ptr = instance;
     instanceOID.length = 2;

  /*
   * Create the trap's varbind list.
   */
   for (vbIndex = 0; vbIndex < CALL_INFO_VARBIND_COUNT; vbIndex++) {
	vbList[vbIndex] = 
	   CatOID((OID *) &callInfo_trap_varbinds[vbIndex],
	                 &instanceOID);
    }
    vbList[vbIndex] = NULL;


    /*
     * tell snmp to do something
     */
   snmp_trap(ENTERPRISE_TRAP, CALL_INFO_TRAP, vbList,
	     (OID *)&enterpriseOID,  trap_oid_str);

    /*
     * free data allocated 
     */
    for (vbIndex = 0; vbIndex <  CALL_INFO_VARBIND_COUNT; vbIndex++) {
	FreeOID(vbList[vbIndex]);
    }
}

static long get_nbr_id (char *remote_number, int dsl)
{
   int index;
   char *self_number = "9999999";

   /*
    * Get the neigbor id. Always start from the first neighbor.
    * Incoming calls will have 9999999 as the remote number.
    */
   if (remote_number == NULL)
      remote_number = self_number;
   for (index = FIRST_NEIGHBOR; index < MAX_NEIGHBORS; index++) {
      if (neighbor_info[index][dsl].nbr_number[0] == '\0')
	  break;
      else
	 if ((strcmp(neighbor_info[index][dsl].nbr_number, remote_number)) == 0)
	    break;
   }
   if (index >= MAX_NEIGHBORS) 
        index = FIRST_NEIGHBOR;
   strcpy(neighbor_info[index][dsl].nbr_number, remote_number);
   return (index);
}


void isdn_neighbor_update (hwidbtype *hwidb)
{
    ulong call_duration;
    demandNbrEntry_t *entry;
    ContextInfo *context;
    isdn_bri_t *isdn_info;
    isdn_bri_t *d_isdn_info;
    callQ  *current;
    dialerdbtype *ddb;
    int dsl;
    char *peer_number;
    hwidbtype *netidb;

    entry = NULL;
    context = NULL;
    call_duration = 0;

    isdn_info = hwidb->isdn_info;
    ddb = get_dialerdb(hwidb);
    d_isdn_info = isdn_info->d_chan_idb->isdn_info;
    for (dsl=0; dsl < MAX_DSL; dsl++)
      if (isdn_idblist[dsl] == isdn_info->d_chan_idb)
	 break;
    if (dsl == MAX_DSL)
	return;
    if ((entry = malloc(sizeof(demandNbrEntry_t))) != NULL) {
         entry->demandNbrPhysIf = isdn_info->d_chan_idb->snmp_if_index;
         SET_VALID(I_demandNbrPhysIf, entry->valid); 
         /* 
          * Let's get the neighbor's id. If the remote number is available,
          * then use it. Otherwise, the number_called field of the B channel
          * may contain the  number if ppp and chap are enabled.
          */
         if (isdn_info->remote_number && *isdn_info->remote_number) {
                  peer_number = isdn_info->remote_number;
         }  else {
                  peer_number = ddb->number_called;
         }
           entry->demandNbrId = get_nbr_id(peer_number, dsl);
	   SET_VALID(I_demandNbrId, entry->valid); 
	   entry->demandNbrName = NULL;
	   entry->demandNbrAddress = NULL;
           entry->demandNbrClearReason = NULL;
           entry->demandNbrClearCode = NULL;
           current = callQget(EXACT, entry->demandNbrPhysIf, entry->demandNbrId);
           if (current == NULL) {
	      current = malloc(sizeof(callQ));
	      if (current == NULL) {
		buginf("\nisdn mib malloc failed\n");
	        demandNbrEntry_free(entry);
		return;
              }
	      current->next = NULL;
           } else {
	      /* 
	       * Something was existing. We must overwrite.
	       */
	      p_unqueue(&isdnNeighborQ, current);
	      demandNbrEntry_free(current->entry);
           }
	   current->entry = entry;
	   TIMER_STOP(current->age_timer);
	   TIMER_START(current->age_timer, DEFAULT_NEIGHBOR_ENTRY_AGEOUT);
           entry->demandNbrLogIf = hwidb->snmp_if_index;
	   SET_VALID(I_demandNbrLogIf, entry->valid); 
	   if (*ddb->remote_name) {
               entry->demandNbrName = MakeOctetString(ddb->remote_name, 
			  strlen(ddb->remote_name));
	       SET_VALID(I_demandNbrName, entry->valid); 
           }
           if (peer_number && *peer_number) {
                entry->demandNbrAddress = MakeOctetString(peer_number, strlen(peer_number));
	       SET_VALID(I_demandNbrAddress, entry->valid);
           }
           entry->demandNbrPermission = D_demandNbrPermission_weCanCallEachOther;
	   SET_VALID(I_demandNbrPermission, entry->valid); 
           entry->demandNbrMaxDuration = MAX_DURATION;
	   SET_VALID(I_demandNbrMaxDuration, entry->valid); 
           entry->demandNbrCallOrigin = (isdn_info->call_id & 0x8000) ? 
                D_demandNbrCallOrigin_originate : D_demandNbrCallOrigin_answer;
           SET_VALID(I_demandNbrCallOrigin, entry->valid); 
           if (!hwidb->isdn_info->call_connected) {
              if (TIMER_RUNNING(isdn_info->connect_time)) {
                 neighbor_info[entry->demandNbrId][dsl].retrying_call = FALSE;
                 call_duration = (ELAPSED_TIME(isdn_info->connect_time))/ONESEC;
		 entry->demandNbrLastDuration = call_duration;
		 SET_VALID(I_demandNbrLastDuration, entry->valid);
              }
              if (isdn_info->call_id & 0x8000) {
                 entry->demandNbrLastAttemptTime = (call_duration > 0) ?
                   SNMP_ABSOLUTE_TIME(isdn_info->connect_time) : GetTimeNow();
                 SET_VALID(I_demandNbrLastAttemptTime, entry->valid);
              }
		  if (hwidb->isdn_info->clear_reason && (*hwidb->isdn_info->clear_reason)) {
		      entry->demandNbrClearReason = MakeOctetString(isdn_info->clear_reason, strlen(isdn_info->clear_reason));
		      SET_VALID(I_demandNbrClearReason, entry->valid);
		  }
		  if (hwidb->isdn_info->clear_code && (*hwidb->isdn_info->clear_code)) {
		      entry->demandNbrClearCode = MakeOctetString(isdn_info->clear_code, strlen(isdn_info->clear_code));
		      SET_VALID(I_demandNbrClearCode, entry->valid);
		  }
	    } else { /* call has been connected, store stats */
		   if (hwidb->isdn_info->outgoing)
		      neighbor_info[entry->demandNbrId][dsl].successful_calls++;
		   else
		      neighbor_info[entry->demandNbrId][dsl].accepted_calls++;
	    }
	    entry->demandNbrSuccessCalls = 
		   neighbor_info[entry->demandNbrId][dsl].successful_calls;
	    SET_VALID(I_demandNbrSuccessCalls, entry->valid);
	    entry->demandNbrFailCalls = 
		  neighbor_info[entry->demandNbrId][dsl].failed_calls;
	    SET_VALID(I_demandNbrFailCalls, entry->valid);
	    entry->demandNbrAcceptCalls = 
		   neighbor_info[entry->demandNbrId][dsl].accepted_calls;
	    SET_VALID(I_demandNbrAcceptCalls, entry->valid);
	    entry->demandNbrRefuseCalls = 
		  neighbor_info[entry->demandNbrId][dsl].refused_calls;
	    SET_VALID(I_demandNbrRefuseCalls, entry->valid);
	    entry->demandNbrStatus = D_demandNbrStatus_active;
	    SET_VALID(I_demandNbrStatus, entry->valid);
	    callQinsert(current);
	    if (isdn_info->call_connected) {
		   TIMER_START(current->trap_timer, 6 * ONESEC);
	    } else {
		  if (isdnTrapsEnabled && !neighbor_info[entry->demandNbrId][dsl].retrying_call)
		     demandNbrCallDetails_trap(current->entry);
                  if (call_duration > 0 ) {
		      netidb = get_netidb(hwidb);
		      if (LOG_INTF_IS_LOGGING_EVENTS_LINK_STATUS(netidb)) {
			  if (*ddb->number_called || *ddb->remote_name) {
			      errmsg(&msgsym(DISCONNECT, ISDN),
				     hwidb->hw_namestring, peer_number,
				     ddb->remote_name, call_duration);
			  } else {
			      errmsg(&msgsym(DISCONNECT, ISDN),
				     hwidb->hw_namestring, "unknown", "",
				     call_duration);
			  }
		      }
		  }
            }
           if (!TIMER_RUNNING(isdn_info->connect_time)) 
           neighbor_info[entry->demandNbrId][dsl].retrying_call = TRUE;
    }
}

void
isdn_stats_update (hwidbtype *hwidb, int stat_type, char *nbr_number)
{
  long nbr_id;
  long physIf;
  callQ *current;
  demandNbrEntry_t *entry;
  isdn_bri_t *isdn_info;
  isdn_bri_t *d_isdn_info;
  int dsl;

  isdn_info = hwidb->isdn_info;
  d_isdn_info = isdn_info->d_chan_idb->isdn_info;
  for (dsl=0; dsl < MAX_DSL; dsl++)
      if (isdn_idblist[dsl] == isdn_info->d_chan_idb)
	 break;
  if (dsl == MAX_DSL)
	return;
  physIf = hwidb->snmp_if_index;
  nbr_id = get_nbr_id(nbr_number, dsl);
  if ((current = callQget(EXACT, physIf, nbr_id)) == NULL) {
      if ((current = malloc(sizeof(callQ))) == NULL) 
	   return;
      current->next = NULL;
      if ((entry = malloc(sizeof(demandNbrEntry_t))) == NULL) {
	    free(current);
	    return;
      } else {
	    current->entry = entry;
            entry->demandNbrPhysIf = physIf;
            SET_VALID(I_demandNbrPhysIf, entry->valid); 
            entry->demandNbrId = nbr_id;
            SET_VALID(I_demandNbrId, entry->valid); 
            current->entry->demandNbrStatus = D_demandNbrStatus_notReady;
            SET_VALID(I_demandNbrStatus, current->entry->valid);
            SET_VALID(I_demandNbrFailCalls, current->entry->valid);
            SET_VALID(I_demandNbrRefuseCalls, current->entry->valid);
            SET_VALID(I_demandNbrSuccessCalls, current->entry->valid);
            SET_VALID(I_demandNbrAcceptCalls, current->entry->valid);
	    current->entry->demandNbrPermission = D_demandNbrPermission_weCanCallEachOther;
	    current->entry->demandNbrPermission = D_demandNbrPermission_weCanCallEachOther;
            SET_VALID(I_demandNbrPermission, current->entry->valid);
            current->entry->demandNbrMaxDuration = MAX_DURATION;
            SET_VALID(I_demandNbrMaxDuration, current->entry->valid);
            if (nbr_number && *nbr_number) {
                entry->demandNbrAddress = MakeOctetString(nbr_number, strlen(nbr_number));
              SET_VALID(I_demandNbrAddress, entry->valid);
            }
            if (isdn_info->clear_reason && *isdn_info->clear_reason) {
                entry->demandNbrClearReason = MakeOctetString(isdn_info->clear_reason, strlen(isdn_info->clear_reason));
               SET_VALID(I_demandNbrClearReason, entry->valid);
            }
            if (isdn_info->clear_code && *isdn_info->clear_code) {
                entry->demandNbrClearCode = MakeOctetString(isdn_info->clear_code, strlen(isdn_info->clear_code));
               SET_VALID(I_demandNbrClearCode, entry->valid);
            }
	    callQinsert(current);
      }
  }       
  switch (stat_type) {
        case FAILED_CALL :
          current->entry->demandNbrFailCalls = 
               ++neighbor_info[nbr_id][dsl].failed_calls;
          break;
        case REFUSED_CALL :
           current->entry->demandNbrRefuseCalls = 
               ++neighbor_info[nbr_id][dsl].refused_calls;
          break;
        default:
          buginf("\nstat update.");
          break;
  }
}
static void
isdn_send_trap (void)
{
    callQ            *test;
    callQ            *next, *match;
    long           d_chan_index, nbr_index;
    boolean          requeue_required;
    int              dsl;

    requeue_required = FALSE;
    dsl = MAX_DSL;

    for (test = isdnNeighborQ.qhead; test; test = next) {
       next = test->next;
       if (TIMER_RUNNING(test->trap_timer) && AWAKE(test->trap_timer)) {
           TIMER_STOP(test->trap_timer);
           nbr_index = test->entry->demandNbrId;
           for (dsl=0; dsl < MAX_DSL; dsl++) {
               if (isdn_idblist[dsl] && isdn_idblist[dsl]->snmp_if_index ==
                                               test->entry->demandNbrPhysIf)
                  break;
           }
           if (dsl == MAX_DSL)
                continue;
           get_more_info(test->entry, dsl);
           if (nbr_index != test->entry->demandNbrId) {
                p_unqueue(&isdnNeighborQ, test);
                requeue_required = TRUE;
                break;
           } else {
            if (isdnTrapsEnabled)
              demandNbrCallDetails_trap(test->entry);
            }
       }
    }
    if (requeue_required) {
          d_chan_index = test->entry->demandNbrPhysIf;
          nbr_index = test->entry->demandNbrId;
          match = isdnNeighborQ.qhead;
          while (match) {
              if (d_chan_index < match->entry->demandNbrPhysIf)
                break;
              else {
                if (d_chan_index == match->entry->demandNbrPhysIf)
                   if (nbr_index < match->entry->demandNbrId)
                      break;
                   else
                      if (nbr_index == match->entry->demandNbrId) {
                          p_unqueue(&isdnNeighborQ, match);
                          demandNbrEntry_free(match->entry);
                          free(match);
                          break;
                      }
              }
              match = match->next;
           }
           test->entry->demandNbrSuccessCalls =
                  neighbor_info[nbr_index][dsl].successful_calls;
           test->entry->demandNbrFailCalls =
                 neighbor_info[nbr_index][dsl].failed_calls;
           test->entry->demandNbrAcceptCalls =
                  ++neighbor_info[nbr_index][dsl].accepted_calls;
           test->entry->demandNbrRefuseCalls =
                 neighbor_info[nbr_index][dsl].refused_calls;
           callQinsert(test);
           if (isdnTrapsEnabled)
              demandNbrCallDetails_trap(test->entry);
      }
}


static void
init_isdnmib (subsystype *subsys)
{
    int index, dsl;
    
    isdnTrapsEnabled = FALSE;
    queue_init(&isdnNeighborQ, 0);
    load_mib(isdnmib_OidList, isdnmib_OidListNum);
    load_oid(isdnmib_oid_table);
    reg_add_Trap_cfg_set(isdn_trap_cfg_set, "isdn_trap_cfg_set");
    reg_add_Trap_cfg_nvwrt(isdn_trap_cfg_nvwrt, "isdn_trap_cfg_nvwrt");
    reg_add_onemin(isdn_call_ageout, "isdn_call_ageout");
    reg_add_isdn_neighbor_update(isdn_neighbor_update, "isdn_neighbor_update");
    reg_add_isdn_stats_update(isdn_stats_update, "isdn_stats_update");
    reg_add_onesec(isdn_send_trap, "isdn_send_trap");
    for (index=0; index < MAX_NEIGHBORS; index++) {
      for (dsl=0; dsl < MAX_DSL; dsl++) {
       neighbor_info[index][dsl].nbr_number[0] = '\0';
       neighbor_info[index][dsl].successful_calls = 0;
       neighbor_info[index][dsl].accepted_calls = 0;
       neighbor_info[index][dsl].refused_calls = 0;
       neighbor_info[index][dsl].failed_calls = 0;
       neighbor_info[index][dsl].retrying_call = FALSE;
      }
    }
}
/*
 * isdnmib subsystem header
 */

/*WARNING: update "req" with the subsystem being monitored */

#define MAJVERSION_isdnmib 1
#define MINVERSION_isdnmib 0
#define EDITVERSION_isdnmib 0

SUBSYS_HEADER(isdnmib,
              MAJVERSION_isdnmib,
              MINVERSION_isdnmib,
              EDITVERSION_isdnmib,
              init_isdnmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: isdn", NULL);
      
