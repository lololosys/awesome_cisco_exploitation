/* $Id: sr_syslogmib.c,v 3.1.48.5 1996/09/14 00:55:20 ramk Exp $
 * $Source: /release/112/cvs/Xsys/os/sr_syslogmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * August 1995, Anke Dosedal (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_syslogmib.c,v $
 * Revision 3.1.48.5  1996/09/14  00:55:20  ramk
 * CSCdi66329:  SNMP syslog traps specify wrong enterpriseOID,sysLogEvent
 * needs fix
 * Branch: California_branch
 * Pass a (pointer to) sysloghistoryentry_t as a parameter to syslogEvent
 *
 * Revision 3.1.48.4  1996/07/01  18:45:45  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.48.3  1996/06/27  19:11:32  anke
 * CSCdi58471:  Invalid memory action in syslog mib
 * Pre-allocate the memory that might be needed so we don't get caught
 * during an interupt.  Change to queue to a circular one to reduce
 * amount of pointer moving that might need to happen.
 * Branch: California_branch
 *
 * Revision 3.1.48.2  1996/06/22  23:47:07  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.1.48.1  1996/05/21  06:36:40  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/01/19  03:24:21  anke
 * Placeholder for Syslog MIB
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
#include <ciscolib.h>
#include "packet.h"
#include "logger.h"
#include "sys_registry.h"
#include "syslogmib_history.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/snmp_trap_fe.h"
#include "sr_syslogmib.h"
#include "sr_syslogmib-mib.h"

#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_snmp.h"
#include "../h/sys_registry.h"

/* Internal Constants */

#define SYSLOG_TRAP_NUMBER 1
#define TRAP_VARBIND_COUNT 5

#define FACILITY  0
#define SEVERITY  1
#define MSGNAME   2
#define MSGTEXT   3
#define TIMESTAMP 4

/*Max traps processed before relinquishing CPU */
#define SYSLOG_TRAP_SWITCHCOUNT 100
#define MAX_SYSLOG_QUEUE_LENGTH 200

/* Globals */

pid_t syslog_trap_pid;
watched_queue *SyslogTrapQueue;

/* Locals */

static const OID syslogtrap_OID = {LNclogMessageGenerated, 
                                   (ulong *)IDclogMessageGenerated};

static const OID enterpriseOID =
{LNclogMessageGenerated - 2, (ulong *)IDclogMessageGenerated};

static const OID syslogVarBinds[TRAP_VARBIND_COUNT] = {
    {LNclogHistFacility, (ulong *)IDclogHistFacility},
    {LNclogHistSeverity, (ulong *)IDclogHistSeverity},
    {LNclogHistMsgName, (ulong *)IDclogHistMsgName},
    {LNclogHistMsgText, (ulong *)IDclogHistMsgText},
    {LNclogHistTimestamp, (ulong *)IDclogHistTimestamp}
};

static char syslog_trap_OID_str[80];



/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the clogBasic family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
clogBasic_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    clogBasic_t     *data = NULL;


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
     if ( (arg == -1) || (data = k_clogBasic_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_clogNotificationsSent
    case I_clogNotificationsSent:
       dp = (void *) (&data->clogNotificationsSent);
       break;
#endif /* I_clogNotificationsSent */

#ifdef I_clogNotificationsEnabled
    case I_clogNotificationsEnabled:
       dp = (void *) (&data->clogNotificationsEnabled);
       break;
#endif /* I_clogNotificationsEnabled */

#ifdef I_clogMaxSeverity
    case I_clogMaxSeverity:
       dp = (void *) (&data->clogMaxSeverity);
       break;
#endif /* I_clogMaxSeverity */

#ifdef I_clogMsgIgnores
    case I_clogMsgIgnores:
       dp = (void *) (&data->clogMsgIgnores);
       break;
#endif /* I_clogMsgIgnores */

#ifdef I_clogMsgDrops
    case I_clogMsgDrops:
       dp = (void *) (&data->clogMsgDrops);
       break;
#endif /* I_clogMsgDrops */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

 }

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the clogBasic data object.
 *---------------------------------------------------------------------*/
void
clogBasic_free(clogBasic_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after clogBasic set/undo
 *---------------------------------------------------------------------*/
static int
clogBasic_cleanup(doList_t *trash)
{
   clogBasic_free(trash->data);
#ifdef SR_SNMPv2
   clogBasic_free(trash->undodata);
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
clogBasic_test(incoming, object, value, doHead, doCur, contextInfo)
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
         if ( (dp->setMethod == clogBasic_set) &&
            (((clogBasic_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(clogBasic_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(clogBasic_t));
#ifdef SR_SNMPv2
        if ( (dp->undodata = malloc(sizeof(clogBasic_t))) == NULL) { 
            DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->undodata, 0, sizeof(clogBasic_t));
#endif /* SR_SNMPv2 */

        dp->setMethod = clogBasic_set;
        dp->cleanupMethod = clogBasic_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_clogBasic_UNDO
        dp->undoMethod = clogBasic_undo;
#else /* SR_clogBasic_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_clogBasic_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_clogNotificationsEnabled
   case I_clogNotificationsEnabled:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((clogBasic_t *) (dp->data))->clogNotificationsEnabled = value->sl_value;
     break;
#endif /* I_clogNotificationsEnabled */

#ifdef I_clogMaxSeverity
   case I_clogMaxSeverity:

     if ((value->sl_value < 1) || (value->sl_value > 8)) {
         return WRONG_VALUE_ERROR;
     }

     ((clogBasic_t *) (dp->data))->clogMaxSeverity = value->sl_value;
     break;
#endif /* I_clogMaxSeverity */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in clogBasic_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_clogBasic_test */
   error_status = k_clogBasic_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((clogBasic_t *) (dp->data))->valid);
        error_status = k_clogBasic_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
clogBasic_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_clogBasic_set((clogBasic_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the clogHistory family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
clogHistory_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    clogHistory_t   *data = NULL;


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
     if ( (arg == -1) || (data = k_clogHistory_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_clogHistTableMaxLength
    case I_clogHistTableMaxLength:
       dp = (void *) (&data->clogHistTableMaxLength);
       break;
#endif /* I_clogHistTableMaxLength */

#ifdef I_clogHistMsgsFlushed
    case I_clogHistMsgsFlushed:
       dp = (void *) (&data->clogHistMsgsFlushed);
       break;
#endif /* I_clogHistMsgsFlushed */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the clogHistory data object.
 *---------------------------------------------------------------------*/
void
clogHistory_free(clogHistory_t *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after clogHistory set/undo
 *---------------------------------------------------------------------*/
static int
clogHistory_cleanup(doList_t *trash)
{
   clogHistory_free(trash->data);
#ifdef SR_SNMPv2
   clogHistory_free(trash->undodata);
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
clogHistory_test(incoming, object, value, doHead, doCur, contextInfo)
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
         if ( (dp->setMethod == clogHistory_set) &&
            (((clogHistory_t *) (dp->data)) != NULL) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;
        if ( (dp->data = malloc(sizeof(clogHistory_t))) == NULL) { 
            DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->data, 0, sizeof(clogHistory_t));
#ifdef SR_SNMPv2
        if ( (dp->undodata = malloc(sizeof(clogHistory_t))) == NULL) { 
            DPRINTF((APALWAYS, "snmpd: Cannot allocate memory\n"));
            return RESOURCE_UNAVAILABLE_ERROR;
        }
        memset(dp->undodata, 0, sizeof(clogHistory_t));
#endif /* SR_SNMPv2 */

        dp->setMethod = clogHistory_set;
        dp->cleanupMethod = clogHistory_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_clogHistory_UNDO
        dp->undoMethod = clogHistory_undo;
#else /* SR_clogHistory_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_clogHistory_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_clogHistTableMaxLength
   case I_clogHistTableMaxLength:

     ((clogHistory_t *) (dp->data))->clogHistTableMaxLength = value->sl_value;
     break;
#endif /* I_clogHistTableMaxLength */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in clogHistory_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_clogHistory_test */
   error_status = k_clogHistory_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((clogHistory_t *) (dp->data))->valid);
        error_status = k_clogHistory_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
clogHistory_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_clogHistory_set((clogHistory_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the clogHistoryEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
clogHistoryEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    clogHistoryEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            clogHistIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &clogHistIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        clogHistIndex = MAX(1, clogHistIndex);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_clogHistoryEntry_get(serialNum, contextInfo, arg ,searchType, clogHistIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->clogHistIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_clogHistFacility
    case I_clogHistFacility:
       dp = (void *) (CloneOctetString(data->clogHistFacility));
       break;
#endif /* I_clogHistFacility */

#ifdef I_clogHistSeverity
    case I_clogHistSeverity:
       dp = (void *) (&data->clogHistSeverity);
       break;
#endif /* I_clogHistSeverity */

#ifdef I_clogHistMsgName
    case I_clogHistMsgName:
       dp = (void *) (CloneOctetString(data->clogHistMsgName));
       break;
#endif /* I_clogHistMsgName */

#ifdef I_clogHistMsgText
    case I_clogHistMsgText:
       dp = (void *) (CloneOctetString(data->clogHistMsgText));
       break;
#endif /* I_clogHistMsgText */

#ifdef I_clogHistTimestamp
    case I_clogHistTimestamp:
       dp = (void *) (&data->clogHistTimestamp);
       break;
#endif /* I_clogHistTimestamp */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */





clogBasic_t *
k_clogBasic_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static clogBasic_t clogBasicData;

   clogBasicData.clogNotificationsSent = get_syslog_notification_count();

   if (syslog_notification_enabled == TRUE)
       clogBasicData.clogNotificationsEnabled = 
                     D_clogNotificationsEnabled_true;
   else
       clogBasicData.clogNotificationsEnabled = 
                     D_clogNotificationsEnabled_false;

   /* max_severity of -1 is a flag letting us know that the severity
    * has been set to the default value.  Note: SyslogSeverity enumeration 
    * values are equal to the values that syslog uses + 1 
    */
   if (max_severity == -1) 
       clogBasicData.clogMaxSeverity = DEF_MAX_SEVERITY + 1;
   else
       clogBasicData.clogMaxSeverity = max_severity + 1; 

   clogBasicData.clogMsgIgnores = get_syslog_ignored_count();
   clogBasicData.clogMsgDrops = logger_drops();
   return(&clogBasicData);
}

#ifdef SETS
int
k_clogBasic_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_clogBasic_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_clogBasic_set(data, contextInfo, function)
   clogBasic_t *data;
   ContextInfo *contextInfo;
   int function;
{
   if (VALID(I_clogNotificationsEnabled, data->valid)) {
       if (data->clogNotificationsEnabled == D_clogNotificationsEnabled_true)
           syslog_notification_enabled = TRUE;
       else
           syslog_notification_enabled = FALSE;
   } 
   if (VALID(I_clogMaxSeverity, data->valid)) {
       max_severity = data->clogMaxSeverity - 1;/*The logger code knows this 
                                   number as being 1 less than the mib code*/
   }
   return NO_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_clogBasic_UNDO
/* add #define SR_clogBasic_UNDO in syslogmib.h to
 * include the undo routine for the clogBasic family.
 */
int
clogBasic_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_clogBasic_UNDO */
#endif /* SR_SNMPv2 */

clogHistory_t *
k_clogHistory_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static clogHistory_t clogHistoryData;

   clogHistoryData.clogHistTableMaxLength = syslog_table_size;
   clogHistoryData.clogHistMsgsFlushed = get_entries_flushed_count();
   return(&clogHistoryData);
}

#ifdef SETS
int
k_clogHistory_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_clogHistory_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_clogHistory_set(data, contextInfo, function)
   clogHistory_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_clogHistory_UNDO
/* add #define SR_clogHistory_UNDO in syslogmib.h to
 * include the undo routine for the clogHistory family.
 */
int
clogHistory_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_clogHistory_UNDO */
#endif /* SR_SNMPv2 */

clogHistoryEntry_t *
k_clogHistoryEntry_get(serialNum, contextInfo, nominator, searchType, clogHistIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long clogHistIndex;
{
   static clogHistoryEntry_t clogHistoryEntryData;

   static OctetString Facility;  
   static OctetString MsgName;
   static OctetString MsgText;

   sysloghistoryentry_t *entry;

   /* try to find an entry and give up if none */

   entry = syslog_history_find(clogHistIndex, (searchType == EXACT));
   if (!entry)
       return(NULL);

   /* set return values from entry */

   clogHistoryEntryData.clogHistIndex = entry->hist_index;

   Facility.octet_ptr = entry->facility;
   Facility.length = strlen(entry->facility);
   clogHistoryEntryData.clogHistFacility = &Facility;

   clogHistoryEntryData.clogHistSeverity = entry->severity + 1;

   MsgName.octet_ptr = entry->msg_name;
   MsgName.length = strlen(entry->msg_name);
   clogHistoryEntryData.clogHistMsgName = &MsgName;

   MsgText.octet_ptr = entry->msg_text;
   MsgText.length = strlen(entry->msg_text);
   clogHistoryEntryData.clogHistMsgText = &MsgText;

   clogHistoryEntryData.clogHistTimestamp = entry->timestamp;

   return(&clogHistoryEntryData);
}

static void do_syslog_trap (sysloghistoryentry_t *entry)
{
    ulong index[1];
    OID instanceOID;
    VarBind   *next;
    VarBind   *current;
    OctetString *Facility, *MsgName, *MsgText;

    index[0] = entry->hist_index;
    instanceOID.length = 1;
    instanceOID.oid_ptr = index;

    Facility = MakeOctetStringFromText(entry->facility);
    MsgName = MakeOctetStringFromText(entry->msg_name);
    MsgText = MakeOctetStringFromText(entry->msg_text);

    /* 
     * If any of the MakeOctetStringFromText calls returned NULL,
     * free any allocated memory and return
     */
    if ((Facility == NULL) || (MsgName == NULL) || (MsgText == NULL)) {
        if (Facility) FreeOctetString(Facility);
        if (MsgName) FreeOctetString(MsgName);
        if (MsgText) FreeOctetString(MsgText);
        return;
    }

    /*
     * For each entry, form the VarBinds.
     * If any of the MakeVarBindWithValue calls returns NULL,
     * stop creating additional VarBinds, free allocated
     * VarBinds and return.
     */
    current = MakeVarBindWithValue((OID *) &syslogVarBinds[FACILITY],
                                   &instanceOID, 
                                   OCTET_PRIM_TYPE,
                                   (void *) Facility);
    next = current;

    if (next != NULL) {
        /*
         * The values specified in the MIB for syslog_severity 
         * start from emergency(1), alert(2)...
         * (entry->severity) starts from zero, so bump it up by 1
         * to reflect the MIB-defined value
         */
        ++entry->severity;
        next->next_var = MakeVarBindWithValue(
                                       (OID *) &syslogVarBinds[SEVERITY],
                                       &instanceOID,
                                       INTEGER_TYPE,
                                       (void *) &entry->severity);
        next = next->next_var;
    }

    if (next != NULL) {
        next->next_var = MakeVarBindWithValue(
                                       (OID *) &syslogVarBinds[MSGNAME],
                                       &instanceOID, 
                                       OCTET_PRIM_TYPE,
                                       (void *) MsgName);
        next = next->next_var;
    }

    if (next != NULL) {
        next->next_var = MakeVarBindWithValue(
                                       (OID *) &syslogVarBinds[MSGTEXT],
                                       &instanceOID, 
                                       OCTET_PRIM_TYPE,
                                       (void *) MsgText);
        next = next->next_var;
    }

    if (next != NULL) {
        next->next_var = MakeVarBindWithValue(
                                       (OID *) &syslogVarBinds[TIMESTAMP],
                                       &instanceOID, 
                                       TIME_TICKS_TYPE,
                                       (void *) &entry->timestamp);
        next = next->next_var;
    }

    if (next != NULL) {
        /* All VarBinds were created - mark end of VarBind list as NULL */
        next->next_var = NULL;
    } else {
        FreeVarBindList(current);
        FreeOctetString(Facility);
        FreeOctetString(MsgName);
        FreeOctetString(MsgText);
        return;
    }

    /* Make the call to generate the trap!  It will free the varbinds. */
    do_trap(ENTERPRISE_TRAP, 
            SYSLOG_TRAP_NUMBER, 
            current,
            (OID *) &enterpriseOID, 
            syslog_trap_OID_str);
}

static void process_SyslogTrapQueue (void) 
{
    paktype *trap_pkt;
    unsigned short pkt_count;

    /* 
     * pkt_count is used to make sure that process_SyslogTrapQueue does:
     *   1. not hog the cpu
     *   2. frees buffers quickly
     * The aim is to let a max of SYSLOG_TRAP_SWITCHCOUNT trap packets be 
     * processed consecutively, before relinquishing the CPU. This way, 
     * a max of SYSLOG_TRAP_SWITCHCOUNT  buffers are freed at one 
     * stretch of cpu usage. 
     */
    pkt_count = SYSLOG_TRAP_SWITCHCOUNT;

    while ((--pkt_count) && (trap_pkt = process_dequeue(SyslogTrapQueue))) {
        /* 
         * 1. bind the VarBind info for the trap and send it
         * 2. free the packet buffer
         */
        do_syslog_trap((sysloghistoryentry_t *)trap_pkt->data_area);

        datagram_done(trap_pkt);
    }
}

static process SyslogTrapProcess (void)
{
    process_watch_queue(SyslogTrapQueue, ENABLE, RECURRING);
    
    for (;;) {
        process_wait_for_event();
        process_SyslogTrapQueue();

        /* 
         * process_SyslogTrapQueue() may have returned with
         * some packets still enqueued, to prevent CPU hogging.
         * Voluntarily relinquish CPU.
         */
        process_may_suspend();
    }
}

static void syslogEvent (sysloghistoryentry_t* entry_ptr)
{
    paktype *trap_pkt;

    /* 
     * Traps are to be sent out only if syslog_notification_enabled
     * has been configured to TRUE
     */
    if (syslog_notification_enabled != TRUE) {
        return;
    }

    /* 
     * Create SyslogTrapProcess and SyslogTrapQueue if they
     * are not yet created
     */
    if (syslog_trap_pid == NO_PROCESS) {
        syslog_trap_pid = process_create(SyslogTrapProcess, 
                                         "Syslog Traps",
                                         NORMAL_STACK, 
                                         PRIO_NORMAL);

       if (syslog_trap_pid != NO_PROCESS) {
            process_set_crashblock(syslog_trap_pid, TRUE);
            SyslogTrapQueue = create_watched_queue("SyslogTrapQueue", 
                                                    MAX_SYSLOG_QUEUE_LENGTH, 
                                                    0);
        }
    }

    /* If either process or queue creation failed, return */
    if ((syslog_trap_pid == NO_PROCESS) || (SyslogTrapQueue == NULL)) {
        return;
    }

    /* Make sure you dont hog buffers by enqueueing too many packets */
    if (process_is_queue_full(SyslogTrapQueue)) {
        return;
    }

    /* Get a buffer, copy and enqueue syslog data */
    trap_pkt = getbuffer(sizeof(sysloghistoryentry_t));
    if (trap_pkt) {
        bcopy(entry_ptr, trap_pkt->data_area, sizeof(sysloghistoryentry_t));
        process_enqueue(SyslogTrapQueue, trap_pkt);
    }
}

static void
init_syslogmib (subsystype *subsys)
{
    load_mib(syslogmib_OidList, syslogmib_OidListNum);
    load_oid(syslogmib_oid_table);

    MakeDotFromOID((OID *)&syslogtrap_OID, syslog_trap_OID_str);
    register_snmp_trap(TRAP_SYSLOG, syslog_trap_OID_str);
    syslogmib_parser_init();
    reg_add_syslogEvent(syslogEvent, "syslogEvent");
    syslog_trap_pid = NO_PROCESS;
    SyslogTrapQueue = NULL;
}


/*
 * syslogmib subsystem header
 */

#define MAJVERSION_syslogmib 1
#define MINVERSION_syslogmib 0
#define EDITVERSION_syslogmib 0

SUBSYS_HEADER(syslogmib,
              MAJVERSION_syslogmib,
              MINVERSION_syslogmib,
              EDITVERSION_syslogmib,
              init_syslogmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
