/* $Id: sr_callmib.c,v 3.10.4.7 1996/08/28 12:54:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/isdn/sr_callmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * January 1995, Bibek A Das (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_callmib.c,v $
 * Revision 3.10.4.7  1996/08/28  12:54:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.4.6  1996/08/04  08:07:22  fox
 * CSCdi62182:  %SCHED-3-PAGEZERO: Low memory modified by ISDN (0x74 =
 * 0x0).
 * Branch: California_branch
 * Convert ddb->dial_reason to an array of char from a char *.  Modify all
 * necessary references appropriately.
 *
 * Revision 3.10.4.5  1996/07/01  18:45:12  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.10.4.4  1996/05/28  16:20:34  vandys
 * CSCdi50153:  systat does not display ISDN access users
 * Branch: California_branch
 *
 * Revision 3.10.4.3  1996/04/27  05:54:43  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.5.8.3  1996/04/26  01:58:18  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.5.8.2  1996/04/17  03:05:13  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.5.8.1  1996/02/23  20:24:46  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.10.4.2  1996/04/12  02:29:45  bdas
 * CSCdi53500:  SNMP output byte counts differ ISDN Primary Rate interface
 * stats
 * Branch: California_branch
 *
 * Revision 3.10.4.1  1996/03/18  20:31:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.4  1996/03/16  07:00:44  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.6.2.3  1996/03/13  20:50:17  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups; changes from reviews
 *
 * Revision 3.6.2.2  1996/03/13  01:40:21  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.1  1996/02/20  01:05:45  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.10  1996/03/08  00:49:00  bdas
 * CSCdi42027:  crash when doing show isdn history. Take care of a case
 * where entries may get removed (due to timer expiry) while printing call
 * history.
 *
 * Revision 3.9  1996/02/12  19:23:33  bdas
 * CSCdi48372:  ciscoCallHistoryRetainTimer need not be started for active
 * calls
 *
 * Revision 3.8  1996/02/09  14:46:09  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.7  1996/01/24  22:04:52  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.6  1996/01/22  06:35:30  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/27  23:48:22  bdas
 * CSCdi46167:  ISDN MIB syslog messages contain incorrect duration
 *
 * Revision 3.4  1995/11/29  18:14:58  bdas
 * CSCdi44122:  ISDN MIB needs to interoperate with HP OpenView
 *
 * Revision 3.3  1995/11/17  17:42:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:50:04  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:15:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/10/21  06:35:21  bdas
 * CSCdi36921:  ISDN MIB needs to contain info about active calls
 *
 * Revision 2.6  1995/08/12  04:21:08  bdas
 * CSCdi36875:  Need to provide a show isdn history command for call
 * history mib
 *
 * Revision 2.5  1995/08/07  20:31:59  bdas
 * CSCdi38212:  Dialing reason may be incorrect in some call history mib
 * entries
 *
 * Revision 2.4  1995/07/21  01:47:14  bdas
 * CSCdi36879:  Need to provide dialing reason in the call history mib.
 *
 * Revision 2.3  1995/06/28  09:28:20  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/15  23:10:23  bdas
 * CSCdi33538:  incomplete/wrong object instances returned; ISDN MIB,
 * Call History Mib.
 *
 * Revision 2.1  1995/06/07  21:17:30  hampton
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
#include "logger.h"
#include "sr_callmib.h"
#include "sr_callmib-mib.h"
#include "subsys.h"
#include "packet.h"
#include "sys_registry.h"
#include "../h/interface_private.h"
#include "../h/interface.h"
#include "../snmp/snmp_registry.h"
#include "../les/isdn_bri.h"
#include "isdn_registry.h"
#include "address.h"
#include "../wan/dialer.h"
#include "../wan/isdn.h"
#include "tune.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../os/clock.h"
#include "../if/network.h"
#include "../wan/ppp.h"
#include "../h/cs_registry.regh"
#include "../h/auth_registry.regh"
#include "../if/network.h"
#include "../wan/dialer_private.h"

#define historyQ struct historyQ_
historyQ {
    historyQ              *next;
    ciscoCallHistoryEntry_t   *entry;
    sys_timestamp       age_timer;
    sys_timestamp       history_timer;
};
static queuetype        callHistoryQ;
static long history_index;
static ciscoCallHistory_t *ciscoCallHistoryParameters;
static boolean show_active;

/*
 * forward reference
 */

static void call_history_ageout(void);
static historyQ * historyQget(int searchType, ulong start_time, long index);
static void historyQinsert(historyQ   *newcall);
static void get_history_info(void);
static void get_caller_info(ciscoCallHistoryEntry_t *history);


/*
 * Start of system-independent routines
 */
  



/*---------------------------------------------------------------------
 * Retrieve data from the ciscoCallHistory family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoCallHistory_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ciscoCallHistory_t *data;

     data = NULL;
     dp = NULL;
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
     if ( (arg == -1) || (data = k_ciscoCallHistory_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoCallHistoryTableMaxLength
    case I_ciscoCallHistoryTableMaxLength:
#endif /* I_ciscoCallHistoryTableMaxLength */

#ifdef I_ciscoCallHistoryRetainTimer
    case I_ciscoCallHistoryRetainTimer:
#endif /* I_ciscoCallHistoryRetainTimer */
       if (VALID(arg, data->valid))
	      dp = SR_OFFSET(data, arg);

       break;
     }      /* switch */
     if (dp == NULL)
	 return ((VarBind *) NULL);
     else
         return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the ciscoCallHistory data object.
 *---------------------------------------------------------------------*/
void
ciscoCallHistory_free (ciscoCallHistory_t *data)
{
   if (data != NULL) {
       free (data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after ciscoCallHistory set/undo
 *---------------------------------------------------------------------*/
static int
ciscoCallHistory_cleanup (doList_t *trash)
{
   ciscoCallHistory_free(trash->data);
#ifdef SR_SNMPv2
   ciscoCallHistory_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}


/*----------------------------------------------------------------------
 * Free the ciscoCallHistoryEntry data object.
 *---------------------------------------------------------------------*/
static void
ciscoCallHistoryEntry_free (ciscoCallHistoryEntry_t *data)
{
   if (data != NULL) {
       if (data->ciscoCallHistoryDialReason != NULL)
             FreeOctetString(data->ciscoCallHistoryDialReason);
       if (data->ciscoCallHistoryCallingNumber != NULL)
             FreeOctetString(data->ciscoCallHistoryCallingNumber);
       if (data->ciscoCallHistoryCalledNumber != NULL)
             FreeOctetString(data->ciscoCallHistoryCalledNumber);
       if (data->ciscoCallHistoryDestinationAddress != NULL)
             FreeOctetString(data->ciscoCallHistoryDestinationAddress);
       if (data->ciscoCallHistoryDestinationHostName != NULL)
             FreeOctetString(data->ciscoCallHistoryDestinationHostName);
       if (data->ciscoCallHistoryConnectTimeOfDay != NULL)
             FreeOctetString(data->ciscoCallHistoryConnectTimeOfDay);
       if (data->ciscoCallHistoryDisconnectTimeOfDay != NULL)
             FreeOctetString(data->ciscoCallHistoryDisconnectTimeOfDay);

       free (data);
   }
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
ciscoCallHistory_test (
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
         if ( (dp->setMethod == ciscoCallHistory_set) &&
            (((ciscoCallHistory_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(ciscoCallHistory_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(ciscoCallHistory_t));
#ifdef SR_SNMPv2
        if ( (dp->undodata = malloc(sizeof(ciscoCallHistory_t))) == NULL) { 
            DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->undodata, 0, sizeof(ciscoCallHistory_t));
#endif /* SR_SNMPv2 */

        dp->setMethod = ciscoCallHistory_set;
        dp->cleanupMethod = ciscoCallHistory_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_ciscoCallHistory_UNDO
        dp->undoMethod = ciscoCallHistory_undo;
#else /* SR_ciscoCallHistory_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_ciscoCallHistory_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_ciscoCallHistoryTableMaxLength
   case I_ciscoCallHistoryTableMaxLength:

     ((ciscoCallHistory_t *) (dp->data))->ciscoCallHistoryTableMaxLength = value->sl_value;
     break;
#endif /* I_ciscoCallHistoryTableMaxLength */

#ifdef I_ciscoCallHistoryRetainTimer
   case I_ciscoCallHistoryRetainTimer:

     ((ciscoCallHistory_t *) (dp->data))->ciscoCallHistoryRetainTimer = value->sl_value;
     break;
#endif /* I_ciscoCallHistoryRetainTimer */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in ciscoCallHistory_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_ciscoCallHistory_test */
   error_status = k_ciscoCallHistory_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((ciscoCallHistory_t *) (dp->data))->valid);
        error_status = k_ciscoCallHistory_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
ciscoCallHistory_set (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_ciscoCallHistory_set((ciscoCallHistory_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the ciscoCallHistoryEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ciscoCallHistoryEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    ciscoCallHistoryEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    unsigned long   ciscoCallHistoryStartTime;
    long            ciscoCallHistoryIndex;
    static OctetString     sparse_string;

     data = NULL;
     dp = NULL;
     sparse_string.length = 0;
     sparse_string.octet_ptr = NULL;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &ciscoCallHistoryIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToUInt(incoming, 0 + object->oid.length, &ciscoCallHistoryStartTime, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ciscoCallHistoryEntry_get(serialNum, contextInfo, arg ,searchType, ciscoCallHistoryStartTime, ciscoCallHistoryIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->ciscoCallHistoryStartTime;
        inst.oid_ptr[1] = data->ciscoCallHistoryIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_ciscoCallHistoryDialReason
    case I_ciscoCallHistoryDialReason:
       if (VALID(I_ciscoCallHistoryDialReason, data->valid))
          dp = (void *) (CloneOctetString(data->ciscoCallHistoryDialReason));
       break;
#endif /* I_ciscoCallHistoryDialReason */

#ifdef I_ciscoCallHistoryConnectTimeOfDay
    case I_ciscoCallHistoryConnectTimeOfDay:
       dp = (void *) (CloneOctetString(data->ciscoCallHistoryConnectTimeOfDay));
       break;
#endif /* I_ciscoCallHistoryConnectTimeOfDay */

#ifdef I_ciscoCallHistoryDisconnectTimeOfDay
    case I_ciscoCallHistoryDisconnectTimeOfDay:
        dp = (void *) (CloneOctetString(data->ciscoCallHistoryDisconnectTimeOfDay));
        break;
#endif /* I_ciscoCallHistoryDisconnectTimeOfDay */


#ifdef I_ciscoCallHistoryCallingNumber
    case I_ciscoCallHistoryCallingNumber:
       if (VALID(I_ciscoCallHistoryCallingNumber, data->valid))
          dp = (void *) (CloneOctetString(data->ciscoCallHistoryCallingNumber));
       break;
#endif /* I_ciscoCallHistoryCallingNumber */

#ifdef I_ciscoCallHistoryCalledNumber
    case I_ciscoCallHistoryCalledNumber:
       if (VALID(I_ciscoCallHistoryCalledNumber, data->valid))
           dp = (void *) (CloneOctetString(data->ciscoCallHistoryCalledNumber));
       break;
#endif /* I_ciscoCallHistoryCalledNumber */

#ifdef I_ciscoCallHistoryDestinationAddress
    case I_ciscoCallHistoryDestinationAddress:
       if (VALID(I_ciscoCallHistoryDestinationAddress, data->valid))
           dp = (void *) (CloneOctetString(data->ciscoCallHistoryDestinationAddress));
       break;
#endif /* I_ciscoCallHistoryDestinationAddress */

#ifdef I_ciscoCallHistoryDestinationHostName
    case I_ciscoCallHistoryDestinationHostName:
       if (VALID(I_ciscoCallHistoryDestinationHostName, data->valid))
           dp = (void *) (CloneOctetString(data->ciscoCallHistoryDestinationHostName));
       break;
#endif /* I_ciscoCallHistoryDestinationHostName */

#ifdef I_ciscoCallHistoryInterfaceNumber
    case I_ciscoCallHistoryInterfaceNumber:
#endif /* I_ciscoCallHistoryInterfaceNumber */

#ifdef I_ciscoCallHistoryCallDisconnectCause
    case I_ciscoCallHistoryCallDisconnectCause:
#endif /* I_ciscoCallHistoryCallDisconnectCause */

#ifdef I_ciscoCallHistoryCallConnectionTime
    case I_ciscoCallHistoryCallConnectionTime:
#endif /* I_ciscoCallHistoryCallConnectionTime */

#ifdef I_ciscoCallHistoryCallDisconnectTime
    case I_ciscoCallHistoryCallDisconnectTime:
#endif /* I_ciscoCallHistoryCallDisconnectTime */

#ifdef I_ciscoCallHistoryTransmitPackets
    case I_ciscoCallHistoryTransmitPackets:
#endif /* I_ciscoCallHistoryTransmitPackets*/

#ifdef I_ciscoCallHistoryTransmitBytes
    case I_ciscoCallHistoryTransmitBytes:
#endif /* I_ciscoCallHistoryTransmitBytes*/

#ifdef I_ciscoCallHistoryReceivePackets
    case I_ciscoCallHistoryReceivePackets:
#endif /* I_ciscoCallHistoryReceivePackets*/

#ifdef I_ciscoCallHistoryReceiveBytes
    case I_ciscoCallHistoryReceiveBytes:
#endif /* I_ciscoCallHistoryReceiveBytes*/

	if (VALID(arg, data->valid))
	    dp = SR_OFFSET(data, arg);
        break;
     }      /* switch */

     if  (dp == NULL)  {
         if (snmp_nosparse) {
             switch (arg) {
               case I_ciscoCallHistoryDialReason:
               case I_ciscoCallHistoryConnectTimeOfDay:
               case I_ciscoCallHistoryDisconnectTimeOfDay:
               case I_ciscoCallHistoryCallingNumber:
               case I_ciscoCallHistoryCalledNumber:
               case I_ciscoCallHistoryDestinationAddress:
               case I_ciscoCallHistoryDestinationHostName:
                     dp = CloneOctetString(&sparse_string);
                     return (MakeVarBind(object, &inst, dp));
               case I_ciscoCallHistoryInterfaceNumber:
               case I_ciscoCallHistoryCallDisconnectTime:
               case I_ciscoCallHistoryCallConnectionTime:
               case I_ciscoCallHistoryCallDisconnectCause:
               case I_ciscoCallHistoryTransmitPackets:
               case I_ciscoCallHistoryTransmitBytes:
               case I_ciscoCallHistoryReceivePackets:
               case I_ciscoCallHistoryReceiveBytes:
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


/*
 * Start of system-dependent routines
 */





ciscoCallHistory_t *
k_ciscoCallHistory_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator)
{

    return (ciscoCallHistoryParameters);
}

#ifdef SETS
int
k_ciscoCallHistory_test (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
     switch (object->nominator) {
       case I_ciscoCallHistoryTableMaxLength :
         if ((value->sl_value < 0 ) ||
            (value->sl_value > HISTORY_TABLE_LENGTH_MAX)) 
	   return WRONG_VALUE_ERROR;
         break;
       case I_ciscoCallHistoryRetainTimer:	 
         if ((value->sl_value < 0 ) ||
             (value->sl_value > HISTORY_RETAIN_TIMER_MAX)) 
	   return WRONG_VALUE_ERROR;
         break;
       default :
	   return WRONG_VALUE_ERROR;
     }
     return NO_ERROR;
}

int
k_ciscoCallHistory_ready (
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_ciscoCallHistory_set (
   ciscoCallHistory_t *data,
   ContextInfo *contextInfo,
   int function)
{

    if (VALID(I_ciscoCallHistoryTableMaxLength, data->valid)) {
       SET_VALID(I_ciscoCallHistoryTableMaxLength, 
		 ciscoCallHistoryParameters->valid);
       ciscoCallHistoryParameters->ciscoCallHistoryTableMaxLength =
	     data->ciscoCallHistoryTableMaxLength;
    }
    if (VALID(I_ciscoCallHistoryRetainTimer, data->valid)) {
       SET_VALID(I_ciscoCallHistoryRetainTimer, 
		  ciscoCallHistoryParameters->valid);
       ciscoCallHistoryParameters->ciscoCallHistoryRetainTimer =
	     data->ciscoCallHistoryRetainTimer;
    }
    return (NO_ERROR);
}
#endif /* SETS */


#ifdef SR_SNMPv2
#ifdef SR_ciscoCallHistory_UNDO
/* add #define SR_ciscoCallHistory_UNDO in callmib.h to
 * include the undo routine for the ciscoCallHistory family.
 */
int
ciscoCallHistory_undo (
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_ciscoCallHistory_UNDO */
#endif /* SR_SNMPv2 */

ciscoCallHistoryEntry_t *
k_ciscoCallHistoryEntry_get (
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   unsigned long ciscoCallHistoryStartTime,
   long ciscoCallHistoryIndex)
{
   historyQ      *data;

   while (TRUE) {
       data = historyQget(searchType, ciscoCallHistoryStartTime, 
		    ciscoCallHistoryIndex);

       if (data == NULL) 
	   return (NULL);

       if (VALID(nominator, data->entry->valid) || snmp_nosparse) {
            get_caller_info(data->entry);
            return(data->entry);
       }
       if (searchType == EXACT) 
            return (NULL);

       ciscoCallHistoryStartTime = data->entry->ciscoCallHistoryStartTime;
       ciscoCallHistoryIndex = data->entry->ciscoCallHistoryIndex;
       if ( (++ciscoCallHistoryIndex > MAX_HISTORY_INDEX) ||
             ( ++ciscoCallHistoryStartTime >= MAX_HISTORY_TIME))
		   return (NULL);

   }
}


/*
 * call entry ageout function
 */
static void
call_history_ageout (void)
{
    leveltype		level;
    historyQ	       *test;
    historyQ	       *next;

    /*
     * make sure we have exclusive access to the callHistoryQ
     */
    level = raise_interrupt_level(ALL_DISABLE);

    for (test = callHistoryQ.qhead; test; test = next) {
	next = test->next;

	if (TIMER_RUNNING(test->age_timer) && AWAKE(test->age_timer)) {
	    unqueue(&callHistoryQ, test);
	    ciscoCallHistoryEntry_free(test->entry);
	    free(test);
	}
    }

    /*
     * re-allow access to the callHistoryQ
     */
    reset_interrupt_level(level);

}




/*
 * historyQget:  grab the requested callHistoryQ entry
 */
static historyQ *
historyQget (
	  int	searchType,
	  unsigned long	time_index,
	  long  entry_index)
{
    leveltype		level;
    historyQ	       *test;
    ciscoCallHistoryEntry_t   *entry;

    /*
     * don't allow the timeout task access to the Q
     */
    level = raise_interrupt_level(ALL_DISABLE);

    for (test = callHistoryQ.qhead; test; test = test->next) {

	entry = test->entry;
	if ( (entry->ciscoCallHistoryStartTime == time_index) &&
	      (entry->ciscoCallHistoryIndex == entry_index) )
             break;
        if (searchType == NEXT) {
            if  (entry->ciscoCallHistoryStartTime > time_index)
		  break;
	    else {
	       if (entry->ciscoCallHistoryStartTime == time_index)
	           if (entry->ciscoCallHistoryIndex > entry_index)
	             break;
            }
        }
        if (searchType == EXACT) {
            if ((entry->ciscoCallHistoryStartTime > time_index) ||
	         ((entry->ciscoCallHistoryStartTime == time_index)  &&
		    (entry->ciscoCallHistoryIndex > entry_index))) {
	         test = NULL;
		 break;
            } 
        }

    }

    /*
     * re-allow access to the callHistoryQ
     */
    reset_interrupt_level(level);

    return (test);
}

/*
 * historyQinsert:  insert a new entry 
 */
void
historyQinsert (historyQ     *newcall)
{
    leveltype		level;
    historyQ            *prev, *test;
    unsigned long       time_index;
    long                history_index;
    boolean             entry_found;

    time_index = newcall->entry->ciscoCallHistoryStartTime;
    history_index = newcall->entry->ciscoCallHistoryIndex;
    prev = NULL;
    entry_found = FALSE;

    /*
     * don't allow the timeout task access to the Q
     */
    level = raise_interrupt_level(ALL_DISABLE);
    if (callHistoryQ.count >= ciscoCallHistoryParameters->ciscoCallHistoryTableMaxLength) {
        test = callHistoryQ.qhead;
        while ((test) && (!entry_found)) {
           if (VALID(I_ciscoCallHistoryCallDisconnectTime, test->entry->valid)){
              entry_found = TRUE;
           } else  {
              test = test->next;
           }
        }
        if (entry_found) {
           unqueue(&callHistoryQ, test);
           ciscoCallHistoryEntry_free(test->entry);
           free(test);
        }
    }
    test = callHistoryQ.qhead;
    while (test) {
       if (time_index < test->entry->ciscoCallHistoryStartTime)
	    break;
       else {
            if (time_index == test->entry->ciscoCallHistoryStartTime)
               if (history_index < test->entry->ciscoCallHistoryIndex)
                      break;
       }
       prev = test;
       test = test->next;
    }
    insqueue(&callHistoryQ, newcall, prev);

    /*
     * re-allow access to the callHistoryQ
     */
    reset_interrupt_level(level);
    return;
}


void isdn_history_update (hwidbtype *hwidb)
{
    ciscoCallHistoryEntry_t *history;
    ContextInfo *context;
    isdn_bri_t *isdn_info, *d_isdn_info;    
    historyQ           *current;
    int dsl;
    char *peer_number, *reason;
    dialerdbtype *ddb;

    history = NULL;
    context = NULL;
    reason = NULL;
    current = NULL;

    isdn_info = hwidb->isdn_info;
    if((isdn_info->call_is_voice == TRUE) && (isdn_info->async_idb != NULL))
        ddb = get_dialerdb(isdn_info->async_idb);
    else {
	if (isdn_info->call_is_voice == TRUE)
	    ddb = NULL;
	else
            ddb = get_dialerdb(hwidb);
    }

    d_isdn_info = isdn_info->d_chan_idb->isdn_info;
    for (dsl=0; dsl < MAX_DSL; dsl++)
        if (isdn_idblist[dsl] == isdn_info->d_chan_idb)
            break;
    if (dsl == MAX_DSL)
        return;
    if ( (ciscoCallHistoryParameters->ciscoCallHistoryTableMaxLength > 0) &&
	 (ciscoCallHistoryParameters->ciscoCallHistoryRetainTimer > 0 )) {
        if (!isdn_info->call_connected)
            current = historyQget(EXACT, isdn_info->time_index,
                              isdn_info->history_index);
        if (current == NULL) {
            current = malloc(sizeof(historyQ));
            history = malloc(sizeof(ciscoCallHistoryEntry_t));
            if ((current == NULL) || (history == NULL))
                return;
            current->next = NULL;
            current->entry = history;
            history->ciscoCallHistoryIndex = history_index;
            if (history_index == MAX_HISTORY_INDEX )
                history_index = FIRST_HISTORY_INDEX;
            else
		history_index++;
            SET_VALID(I_ciscoCallHistoryIndex, history->valid);
            history->ciscoCallHistoryStartTime = GetTimeNow();
            SET_VALID(I_ciscoCallHistoryStartTime, history->valid);
            historyQinsert(current);
            isdn_info->history_index = history->ciscoCallHistoryIndex;
            isdn_info->time_index = history->ciscoCallHistoryStartTime;
        } else {
            history = current->entry;
        }
        if ( (ddb) && (*ddb->remote_name) && (! VALID(I_ciscoCallHistoryDestinationHostName, history->valid)) ) {
            history->ciscoCallHistoryDestinationHostName =
	    MakeOctetString(ddb->remote_name, strlen(ddb->remote_name));
	    SET_VALID(I_ciscoCallHistoryDestinationHostName, history->valid); 
        }
        if ((isdn_info->remote_number && *isdn_info->remote_number) ||
             TIMER_RUNNING(isdn_info->connect_time))
              peer_number = isdn_info->remote_number;
        else if (ddb)
              peer_number = ddb->number_called;
        else
              peer_number = NULL;

        if (peer_number && *peer_number) {
              if (!TIMER_RUNNING(isdn_info->connect_time) || (isdn_info->outgoing)) {
                   if (! VALID(I_ciscoCallHistoryCalledNumber, history->valid))
                      history->ciscoCallHistoryCalledNumber = MakeOctetString(peer_number, strlen(peer_number));
                    SET_VALID(I_ciscoCallHistoryCalledNumber, history->valid);
                    if ( (ddb) && (*ddb->dial_reason) ) {
                       if (! VALID(I_ciscoCallHistoryDialReason, history->valid))
                           history->ciscoCallHistoryDialReason = MakeOctetString(ddb->dial_reason, strlen(ddb->dial_reason));
                        SET_VALID(I_ciscoCallHistoryDialReason, history->valid);
                    }
              } else {
                    if (!VALID(I_ciscoCallHistoryCallingNumber, history->valid))
                        history->ciscoCallHistoryCallingNumber = MakeOctetString(peer_number, strlen(peer_number));
                    SET_VALID(I_ciscoCallHistoryCallingNumber, history->valid);
              }
        }
        if (isdn_info->connect_tod[0] != '\0') {
                 if (!VALID(I_ciscoCallHistoryConnectTimeOfDay, history->valid))
                      history->ciscoCallHistoryConnectTimeOfDay = MakeOctetString(isdn_info->connect_tod, strlen(isdn_info->connect_tod));
                  SET_VALID(I_ciscoCallHistoryConnectTimeOfDay, history->valid);
        }
        if (!isdn_info->call_connected) {
            current_time_string(isdn_info->disconnect_tod);
            if (isdn_info->disconnect_tod[0] != '\0') {
                 history->ciscoCallHistoryDisconnectTimeOfDay = MakeOctetString(isdn_info->disconnect_tod, strlen(isdn_info->disconnect_tod));
                 SET_VALID(I_ciscoCallHistoryDisconnectTimeOfDay, history->valid);
	     }
             history->ciscoCallHistoryCallDisconnectTime = GetTimeNow();
             SET_VALID(I_ciscoCallHistoryCallDisconnectTime, history->valid);
	     isdn_info->connect_tod[0] = '\0';
             isdn_info->disconnect_tod[0] = '\0';
	     history->ciscoCallHistoryCallDisconnectCause =
                   hwidb->isdn_info->disconnect_cause;
             SET_VALID(I_ciscoCallHistoryCallDisconnectCause, history->valid);
             isdn_info->history_index = 0;
             isdn_info->time_index = 0;
        }
        history->ciscoCallHistoryInterfaceNumber = isdn_info->d_chan_idb->snmp_if_index;
        SET_VALID(I_ciscoCallHistoryInterfaceNumber, history->valid);
        if (TIMER_RUNNING(isdn_info->connect_time)) {
              history->ciscoCallHistoryCallConnectionTime =
		  SNMP_ABSOLUTE_TIME(isdn_info->connect_time);
              SET_VALID(I_ciscoCallHistoryCallConnectionTime, history->valid);
              if (!isdn_info->call_connected)
                  TIMER_STOP(isdn_info->connect_time);
        }
        if (isdn_info->call_connected) {
             if(isdn_info->call_is_voice == TRUE){
                 /* For a modem call, we need to give it time to train,
                  * negotiate, etc.
                  */
                 TIMER_START(current->history_timer, 30 * ONESEC );
             } else {
                 TIMER_START(current->history_timer, 4 * ONESEC );
             }
             SET_VALID(I_ciscoCallHistoryTransmitPackets, history->valid);
             SET_VALID(I_ciscoCallHistoryTransmitBytes, history->valid);
             SET_VALID(I_ciscoCallHistoryReceivePackets, history->valid);
             SET_VALID(I_ciscoCallHistoryReceiveBytes, history->valid);
        } else {
            /* If this is a modem call, we should get the stats from the 
             * async idb
             */
            hwidbtype * idb_to_use;
            if ((isdn_info->call_is_voice == TRUE) && (isdn_info->async_idb != NULL))
                idb_to_use = isdn_info->async_idb;
            else {
		if (isdn_info->call_is_voice == TRUE)
		    idb_to_use = NULL;
		else
                    idb_to_use = hwidb;
	    }
	    if (idb_to_use != NULL) {
                history->ciscoCallHistoryTransmitPackets = idb_outputs(idb_to_use) - isdn_info->tx_packets;
                history->ciscoCallHistoryTransmitBytes = idb_tx_cumbytes(idb_to_use) - isdn_info->tx_bytes;
                history->ciscoCallHistoryReceivePackets = idb_inputs(idb_to_use) - isdn_info->rx_packets;
                history->ciscoCallHistoryReceiveBytes = idb_rx_cumbytes(idb_to_use) - isdn_info->rx_bytes;
	    }
            TIMER_START(current->age_timer,
                        ciscoCallHistoryParameters->ciscoCallHistoryRetainTimer * ONEMIN );
        }
    }
}


  
/*----------------------------------------------------------------------
 * copy the ciscoCallHistoryEntry family
 *---------------------------------------------------------------------*/
static void
copy_ciscoCallHistoryEntry (ciscoCallHistoryEntry_t *ciscoCallHistoryEntry, ciscoCallHistoryEntry_t *data)
{

    memcpy((char *) (data), (char *) (ciscoCallHistoryEntry), sizeof(ciscoCallHistoryEntry_t));

    if (VALID(I_ciscoCallHistoryCallingNumber, ciscoCallHistoryEntry->valid)) 
       data->ciscoCallHistoryCallingNumber = CloneOctetString(ciscoCallHistoryEntry->ciscoCallHistoryCallingNumber);
    if (VALID(I_ciscoCallHistoryDestinationHostName, ciscoCallHistoryEntry->valid)) 
       data->ciscoCallHistoryDestinationHostName = CloneOctetString(ciscoCallHistoryEntry->ciscoCallHistoryDestinationHostName);
    if (VALID(I_ciscoCallHistoryDestinationAddress, ciscoCallHistoryEntry->valid))
       data->ciscoCallHistoryDestinationAddress= CloneOctetString(ciscoCallHistoryEntry->ciscoCallHistoryDestinationAddress);
    if (VALID(I_ciscoCallHistoryDialReason, ciscoCallHistoryEntry->valid)) 
       data->ciscoCallHistoryDialReason = CloneOctetString(ciscoCallHistoryEntry->ciscoCallHistoryDialReason);
    if (VALID(I_ciscoCallHistoryCalledNumber, ciscoCallHistoryEntry->valid)) 
       data->ciscoCallHistoryCalledNumber = CloneOctetString(ciscoCallHistoryEntry->ciscoCallHistoryCalledNumber);
    if (VALID(I_ciscoCallHistoryConnectTimeOfDay, ciscoCallHistoryEntry->valid)) 
       data->ciscoCallHistoryConnectTimeOfDay = CloneOctetString(ciscoCallHistoryEntry->ciscoCallHistoryConnectTimeOfDay);
    if (VALID(I_ciscoCallHistoryDisconnectTimeOfDay, ciscoCallHistoryEntry->valid)) 
       data->ciscoCallHistoryDisconnectTimeOfDay = CloneOctetString(ciscoCallHistoryEntry->ciscoCallHistoryDisconnectTimeOfDay);

}



static hwidbtype *get_call_idb(ciscoCallHistoryEntry_t *history)
{
  int dsl, channel;
  isdn_bri_t *isdn_info;
  hwidbtype *idb;
  
  idb = NULL;
  for (dsl=0; dsl < MAX_DSL; dsl++)
      if (isdn_idblist[dsl] && (isdn_idblist[dsl]->snmp_if_index == history->ciscoCallHistoryInterfaceNumber))
          break;
  if (dsl == MAX_DSL)
      return NULL;
  for (channel = 0; channel < ISDN_MAX_CHAN; channel++) {
        if ((idb = isdn_idblist[dsl]->isdn_info->b_chan_idb[channel]) == NULL)
	  continue;
	isdn_info = idb->isdn_info;
	if (isdn_info == NULL)
	   continue;
	if ( (isdn_info->call_connected) &&
	     (isdn_info->history_index == history->ciscoCallHistoryIndex) &&
	     (isdn_info->time_index == history->ciscoCallHistoryStartTime) ) 
            return(idb);
  }
  return(NULL);
}

void get_caller_info(ciscoCallHistoryEntry_t *history)
{
  isdn_bri_t *isdn_info;
  hwidbtype *idb;
  dialerdbtype *ddb;
  hwidbtype * idb_to_use;

  if ((idb = get_call_idb(history)) != NULL) { 
     isdn_info = idb->isdn_info;
     if((isdn_info->call_is_voice == TRUE) && (isdn_info->async_idb != NULL))
         ddb = get_dialerdb(isdn_info->async_idb);
     else {
	if (isdn_info->call_is_voice == TRUE)
	    return;
	else
            ddb = get_dialerdb(idb);
     }
     
     if(ddb){
         if ((ddb->number_called[0] != '\0') &&
             (!isdn_info->outgoing) && 
             (!VALID(I_ciscoCallHistoryCallingNumber, history->valid))) {
             history->ciscoCallHistoryCallingNumber = MakeOctetString(ddb->number_called, strlen(ddb->number_called));
                   SET_VALID(I_ciscoCallHistoryCallingNumber, history->valid);
         }
         if ((ddb->remote_name[0] != '\0') &&
             (!VALID(I_ciscoCallHistoryDestinationHostName, history->valid))){
             history->ciscoCallHistoryDestinationHostName = 
                   MakeOctetString(ddb->remote_name, strlen(ddb->remote_name));
             SET_VALID(I_ciscoCallHistoryDestinationHostName, history->valid); 
         }
     }
     /* If this is a modem call, we should get the stats from the 
      * async idb
      */
     if ((isdn_info->call_is_voice == TRUE) && (isdn_info->async_idb != NULL))
         idb_to_use = isdn_info->async_idb;
     else
         idb_to_use = idb;

     history->ciscoCallHistoryTransmitPackets = idb_outputs(idb_to_use) - isdn_info->tx_packets;
     history->ciscoCallHistoryTransmitBytes = idb_tx_cumbytes(idb_to_use) - isdn_info->tx_bytes;
     history->ciscoCallHistoryReceivePackets = idb_inputs(idb_to_use) - isdn_info->rx_packets;
     history->ciscoCallHistoryReceiveBytes = idb_rx_cumbytes(idb_to_use) - isdn_info->rx_bytes;
  }
}
static void print_call_history (ciscoCallHistoryEntry_t *history, int dsl)
{
    long call_duration;
    int index;
    ulong disc_time;
    hwidbtype *idb;
    dialerdbtype *ddb;
    isdn_bri_t * isdn_info;
    char *ptr;

    idb = NULL;
    if (VALID(I_ciscoCallHistoryCallDisconnectTime, history->valid)) 
	disc_time = history->ciscoCallHistoryCallDisconnectTime;
    else
	disc_time = GetTimeNow();

    if (VALID(I_ciscoCallHistoryCallConnectionTime, history->valid)) 
        call_duration =
	    (disc_time - history->ciscoCallHistoryCallConnectionTime)/100;
    else
	call_duration = 0;
    
    if (VALID(I_ciscoCallHistoryCalledNumber, history->valid)) {
	printf("\nOut             ");
	for (index = 0, ptr = history->ciscoCallHistoryCalledNumber->octet_ptr;
	     index < history->ciscoCallHistoryCalledNumber->length;
	     index++)
	    printc(*ptr++);
	for (; index < 16; index++)
	    printc(' ');

    } else {
	if (VALID(I_ciscoCallHistoryCallingNumber, history->valid)) {
	    printf("\nIn   ");
	    for (index = 0,
		     ptr = history->ciscoCallHistoryCallingNumber->octet_ptr; 
		 index < history->ciscoCallHistoryCallingNumber->length;
		 index++)
		printc(*ptr++);
	    for (; index < 27; index++)
		printc(' ');
	} else {
	    printf("\nIn                               ");
	}
    }
    if (VALID(I_ciscoCallHistoryCallDisconnectTime, history->valid)) {
	if (call_duration > 0)
	    printf("%7d  ", call_duration);
	else
	    printf("Failed  ");
    } else {
	printf("Active(%d)  ", call_duration);
    }
    if (VALID(I_ciscoCallHistoryDestinationHostName, history->valid)) {
	for (index = 0,
	     ptr = history->ciscoCallHistoryDestinationHostName->octet_ptr; 
	     index < history->ciscoCallHistoryDestinationHostName->length;
	     index++)
	    printc(*ptr++);
    } else
	printf("          ");

    idb = get_call_idb(history);
    if (idb == NULL)
	return;
    isdn_info = idb->isdn_info;
    if((isdn_info->call_is_voice == TRUE) && (isdn_info->async_idb != NULL))
	ddb = get_dialerdb(isdn_info->async_idb);
    else
	ddb = get_dialerdb(idb);

    if (ddb && (TIMER_RUNNING(ddb->dialer_idle_timer)))
	printf(" %7u ", TIME_LEFT_SLEEPING(ddb->dialer_idle_timer) / ONESEC);

}

void isdn_show_history (int dsl)
{
    leveltype		level;
    historyQ	       *test;
    ciscoCallHistoryEntry_t   *entry;
    int                 q_ind;
    
    printf("-------------------------------------------------------------\n");
    if (show_active)
    printf("                    ISDN ACTIVE CALLS\n");
    else
    printf("                    ISDN CALL HISTORY\n");
    printf("-------------------------------------------------------------\n");
    printf("History Table MaxLength = %d entries\n",
	    ciscoCallHistoryParameters->ciscoCallHistoryTableMaxLength);
    printf("History Retain Timer = %d Minutes\n", 
            ciscoCallHistoryParameters->ciscoCallHistoryRetainTimer);
    printf("-------------------------------------------------------------\n");
    printf("Call Calling     Called        Duration  Remote   Time until\n");
    printf("Type Number      Number        Seconds   Name     Disconnect\n");
    printf("-------------------------------------------------------------");
    automore_enable(NULL);
    for (q_ind = 0; (test = queuepick(&callHistoryQ.qhead, q_ind)); q_ind++) {
         if ( ((dsl >= 0) && (test->entry->ciscoCallHistoryInterfaceNumber != isdn_idblist[dsl]->snmp_if_index)) ||
        (show_active && (VALID(I_ciscoCallHistoryCallDisconnectTime, test->entry->valid))) )
	     continue;
         else {
             entry = malloc(sizeof(ciscoCallHistoryEntry_t));
             level = raise_interrupt_level(ALL_DISABLE);
             copy_ciscoCallHistoryEntry(test->entry, entry);
             reset_interrupt_level(level);
             print_call_history(entry, dsl);
             ciscoCallHistoryEntry_free(entry);
          }
       automore_conditional(0);
    }
    printf("\n-------------------------------------------------------------\n");
    show_active = FALSE;
}

void isdn_show_active (int dsl)
{
  show_active = TRUE;
  isdn_show_history(dsl);
}

static void
get_history_info (void)
{
    historyQ         *test;
    historyQ         *next;

    for (test = callHistoryQ.qhead; test; test = next) {
        next = test->next;
        if (TIMER_RUNNING(test->history_timer) && AWAKE(test->history_timer)) {
           TIMER_STOP(test->history_timer);
           get_caller_info(test->entry);
        }
    }
}

/*
 * do_isdn_systat()
 *	Display status of active ISDN B channels
 */
static void
do_isdn_systat (dialerdbtype *ddb)
{
    char *name;
    hwidbtype *hw;

    if (!ddb)
	return;

    /* Point to hwidb of B-channel */
    hw = ddb->ownidb;

    /*
     * Only display for active ISDN interfaces in PPP mode
     */
    if (!is_isdn(hw) ||
	(hw->enctype != ET_PPP) ||
	!hw_interface_up(hw)) {
	return;
    }

    /*
     * Dump out the display
     */
    name = "";
    (void)reg_invoke_get_authname(hw, &name);
    printf("  %10s %9s %20s ", hw->hw_short_namestring, name, "Sync PPP");
    print_dhms(hw->lastinput);
    printf("\n");
}

/*
 * isdn_show_systat()
 *	Display systat output for ISDN PPP interfaces
 */
static void
isdn_show_systat (void)
{
    hwidbtype *hw;
    dialerdbtype *ddb;

    FOR_ALL_HWIDBS(hw) {
	/*
 	 * Dump ISDN B-channels for a given D-channel
  	 */
 	if (is_ddr(hw) &&
	    is_isdn(hw) && hw->isdn_info->chantype == ISDN_Dchan) {
 	    FOR_ALL_HUNTDDBS_ON_MASTER(hw->dialerdb, ddb) {
 		do_isdn_systat(ddb);
 	    }
	}
    }
}

static void
init_callmib (subsystype *subsys)
{

    queue_init(&callHistoryQ, 0);
    load_mib(callmib_OidList, callmib_OidListNum);
    load_oid(callmib_oid_table);
    reg_add_onemin(call_history_ageout, "call_history_ageout");
    reg_add_onesec(get_history_info, "get_history_info");
    history_index = FIRST_HISTORY_INDEX;
    show_active = FALSE;
    ciscoCallHistoryParameters = malloc( sizeof(ciscoCallHistory_t)); 
    if (ciscoCallHistoryParameters != NULL) {
        ciscoCallHistoryParameters->ciscoCallHistoryTableMaxLength =
	      HISTORY_TABLE_LENGTH_DEFAULT;
        SET_VALID(I_ciscoCallHistoryTableMaxLength, 
		 ciscoCallHistoryParameters->valid);
        ciscoCallHistoryParameters->ciscoCallHistoryRetainTimer =
	      HISTORY_RETAIN_TIMER_DEFAULT;
        SET_VALID(I_ciscoCallHistoryRetainTimer, 
		  ciscoCallHistoryParameters->valid);
        reg_add_isdn_history_update(isdn_history_update, "isdn_history_update");
        reg_add_isdn_show_history(isdn_show_history, "isdn_show_history");
        reg_add_isdn_show_active(isdn_show_active, "isdn_show_active");
	reg_add_systat(isdn_show_systat, "isdn_show_systat");
    } else
        errmsg(&msgsym(NOMEMORY, SYS), "Parameters in call history mib", 0);
}

/*
 * callmib subsystem header
 */

#define MAJVERSION_callmib 1
#define MINVERSION_callmib 0
#define EDITVERSION_callmib 0

SUBSYS_HEADER(callmib,
              MAJVERSION_callmib,
              MINVERSION_callmib,
              EDITVERSION_callmib,
              init_callmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: isdn", NULL);
      
