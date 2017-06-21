/* $Id: sr_tcpoffloadmib.c,v 3.4.8.1 1996/07/01 18:44:37 hampton Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_tcpoffloadmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1995, Michael Otto (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_tcpoffloadmib.c,v $
 * Revision 3.4.8.1  1996/07/01  18:44:37  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4  1996/02/29  17:00:42  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.3  1996/01/24  22:03:07  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.2  1995/11/17  09:20:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:42:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:25:07  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:44:34  hampton
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
#include "sr_tcpoffloadmib.h"
#include "sr_tcpoffloadmib-mib.h"
#include "if_cip.h"
#include "channelmib_utils.h"

/*
 * Start of system-independent routines
 */




/*---------------------------------------------------------------------
 * Retrieve data from the cipCardOffloadConfigEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cipCardOffloadConfigEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp = NULL;
    cipCardOffloadConfigEntry_t *data = NULL;
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
     if ( (arg == -1) || (data = k_cipCardOffloadConfigEntry_get(serialNum, contextInfo, arg ,searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)) == NULL) {
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

#ifdef I_cipCardOffloadConfigPath
    case I_cipCardOffloadConfigPath:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigPath));
       }
       break;
#endif /* I_cipCardOffloadConfigPath */

#ifdef I_cipCardOffloadConfigDevice
    case I_cipCardOffloadConfigDevice:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigDevice));
       }
       break;
#endif /* I_cipCardOffloadConfigDevice */

#ifdef I_cipCardOffloadConfigIpAddr
    case I_cipCardOffloadConfigIpAddr:
          dp = (void *) (IPToOctetString(data->cipCardOffloadConfigIpAddr));
       break;
#endif /* I_cipCardOffloadConfigIpAddr */

#ifdef I_cipCardOffloadConfigHostName
    case I_cipCardOffloadConfigHostName:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigHostName));
       }
       break;
#endif /* I_cipCardOffloadConfigHostName */

#ifdef I_cipCardOffloadConfigRouterName
    case I_cipCardOffloadConfigRouterName:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigRouterName));
       }
       break;
#endif /* I_cipCardOffloadConfigRouterName */

#ifdef I_cipCardOffloadConfigLinkHostAppl
    case I_cipCardOffloadConfigLinkHostAppl:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigLinkHostAppl));
       }
       break;
#endif /* I_cipCardOffloadConfigLinkHostAppl */

#ifdef I_cipCardOffloadConfigLinkRouterAppl
    case I_cipCardOffloadConfigLinkRouterAppl:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigLinkRouterAppl));
       }
       break;
#endif /* I_cipCardOffloadConfigLinkRouterAppl */

#ifdef I_cipCardOffloadConfigAPIHostAppl
    case I_cipCardOffloadConfigAPIHostAppl:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigAPIHostAppl));
       }
       break;
#endif /* I_cipCardOffloadConfigAPIHostAppl */

#ifdef I_cipCardOffloadConfigAPIRouterAppl
    case I_cipCardOffloadConfigAPIRouterAppl:
       if (VALID(arg, data->valid)) {
          dp = (void *) (CloneOctetString(data->cipCardOffloadConfigAPIRouterAppl));
       }
       break;
#endif /* I_cipCardOffloadConfigAPIRouterAppl */

#ifdef I_cipCardOffloadConfigBroadcastEnable
    case I_cipCardOffloadConfigBroadcastEnable:
       dp = (void *) (&data->cipCardOffloadConfigBroadcastEnable);
       break;
#endif /* I_cipCardOffloadConfigBroadcastEnable */

#ifdef I_cipCardOffloadConfigRowStatus
    case I_cipCardOffloadConfigRowStatus:
       dp = (void *) (&data->cipCardOffloadConfigRowStatus);
       break;
#endif /* I_cipCardOffloadConfigRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cipCardOffloadConfigEntry data object.
 *---------------------------------------------------------------------*/
void
cipCardOffloadConfigEntry_free(cipCardOffloadConfigEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->cipCardOffloadConfigPath);
       FreeOctetString(data->cipCardOffloadConfigDevice);
       FreeOctetString(data->cipCardOffloadConfigHostName);
       FreeOctetString(data->cipCardOffloadConfigRouterName);
       FreeOctetString(data->cipCardOffloadConfigLinkHostAppl);
       FreeOctetString(data->cipCardOffloadConfigLinkRouterAppl);
       FreeOctetString(data->cipCardOffloadConfigAPIHostAppl);
       FreeOctetString(data->cipCardOffloadConfigAPIRouterAppl);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cipCardOffloadConfigEntry set/undo
 *---------------------------------------------------------------------*/
static int
cipCardOffloadConfigEntry_cleanup(doList_t *trash)
{
   cipCardOffloadConfigEntry_free(trash->data);
#ifdef SR_SNMPv2
   cipCardOffloadConfigEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cipCardOffloadConfigEntry family
 *---------------------------------------------------------------------*/
cipCardOffloadConfigEntry_t *
Clone_cipCardOffloadConfigEntry(cipCardOffloadConfigEntry_t *cipCardOffloadConfigEntry)
{
    cipCardOffloadConfigEntry_t *data;

    if ((data = malloc(sizeof(cipCardOffloadConfigEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cipCardOffloadConfigEntry), sizeof(cipCardOffloadConfigEntry_t));

    data->cipCardOffloadConfigPath = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigPath);
    data->cipCardOffloadConfigDevice = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigDevice);
    data->cipCardOffloadConfigHostName = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigHostName);
    data->cipCardOffloadConfigRouterName = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigRouterName);
    data->cipCardOffloadConfigLinkHostAppl = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigLinkHostAppl);
    data->cipCardOffloadConfigLinkRouterAppl = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigLinkRouterAppl);
    data->cipCardOffloadConfigAPIHostAppl = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigAPIHostAppl);
    data->cipCardOffloadConfigAPIRouterAppl = CloneOctetString(cipCardOffloadConfigEntry->cipCardOffloadConfigAPIRouterAppl);

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
cipCardOffloadConfigEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cipCardOffloadConfigEntry_t     *cipCardOffloadConfigEntry;
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

    cipCardOffloadConfigEntry = k_cipCardOffloadConfigEntry_get(-1, contextInfo, -1, EXACT, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cipCardOffloadConfigEntry_set) &&
            (((cipCardOffloadConfigEntry_t *) (dp->data)) != NULL) &&
            (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardEntryIndex == cipCardEntryIndex) &&
            (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardDtrBrdIndex == cipCardDtrBrdIndex) &&
            (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardSubChannelIndex == cipCardSubChannelIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cipCardOffloadConfigEntry_set;
        dp->cleanupMethod = cipCardOffloadConfigEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cipCardOffloadConfigEntry_UNDO
        dp->undoMethod = cipCardOffloadConfigEntry_undo;
#else /* SR_cipCardOffloadConfigEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cipCardOffloadConfigEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cipCardOffloadConfigEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cipCardOffloadConfigEntry(cipCardOffloadConfigEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cipCardOffloadConfigEntry(cipCardOffloadConfigEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(cipCardOffloadConfigEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cipCardOffloadConfigEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardEntryIndex = (cipCardEntryIndex);
                SET_VALID(I_cipCardEntryIndex, ((cipCardOffloadConfigEntry_t *) (dp->data))->valid);

                ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardDtrBrdIndex = (cipCardDtrBrdIndex);
                SET_VALID(I_cipCardDtrBrdIndex, ((cipCardOffloadConfigEntry_t *) (dp->data))->valid);

                ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardSubChannelIndex = (cipCardSubChannelIndex);
                SET_VALID(I_cipCardSubChannelIndex, ((cipCardOffloadConfigEntry_t *) (dp->data))->valid);

                error_status = k_cipCardOffloadConfigEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cipCardOffloadConfigPath
   case I_cipCardOffloadConfigPath:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigPath != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigPath);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigPath = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigPath */

#ifdef I_cipCardOffloadConfigDevice
   case I_cipCardOffloadConfigDevice:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigDevice != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigDevice);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigDevice = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigDevice */

#ifdef I_cipCardOffloadConfigIpAddr
   case I_cipCardOffloadConfigIpAddr:

     if (value->os_value->length != 4) {
         return WRONG_LENGTH_ERROR;
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigIpAddr = OctetStringToIP(value->os_value);
     break;
#endif /* I_cipCardOffloadConfigIpAddr */

#ifdef I_cipCardOffloadConfigHostName
   case I_cipCardOffloadConfigHostName:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigHostName != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigHostName);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigHostName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigHostName */

#ifdef I_cipCardOffloadConfigRouterName
   case I_cipCardOffloadConfigRouterName:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigRouterName != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigRouterName);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigRouterName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigRouterName */

#ifdef I_cipCardOffloadConfigLinkHostAppl
   case I_cipCardOffloadConfigLinkHostAppl:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigLinkHostAppl != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigLinkHostAppl);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigLinkHostAppl = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigLinkHostAppl */

#ifdef I_cipCardOffloadConfigLinkRouterAppl
   case I_cipCardOffloadConfigLinkRouterAppl:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigLinkRouterAppl != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigLinkRouterAppl);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigLinkRouterAppl = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigLinkRouterAppl */

#ifdef I_cipCardOffloadConfigAPIHostAppl
   case I_cipCardOffloadConfigAPIHostAppl:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigAPIHostAppl != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigAPIHostAppl);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigAPIHostAppl = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigAPIHostAppl */

#ifdef I_cipCardOffloadConfigAPIRouterAppl
   case I_cipCardOffloadConfigAPIRouterAppl:

     if (((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigAPIRouterAppl != NULL) {
        FreeOctetString(((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigAPIRouterAppl);
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigAPIRouterAppl = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cipCardOffloadConfigAPIRouterAppl */

#ifdef I_cipCardOffloadConfigBroadcastEnable
   case I_cipCardOffloadConfigBroadcastEnable:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigBroadcastEnable = value->sl_value;
     break;
#endif /* I_cipCardOffloadConfigBroadcastEnable */

#ifdef I_cipCardOffloadConfigRowStatus
   case I_cipCardOffloadConfigRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_cipCardOffloadConfigRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (cipCardOffloadConfigEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_cipCardOffloadConfigRowStatus_notInService) || (value->sl_value == D_cipCardOffloadConfigRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_cipCardOffloadConfigRowStatus_createAndGo) || (value->sl_value == D_cipCardOffloadConfigRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((cipCardOffloadConfigEntry_t *) (dp->data))->cipCardOffloadConfigRowStatus = value->sl_value;
     break;
#endif /* I_cipCardOffloadConfigRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cipCardOffloadConfigEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cipCardOffloadConfigEntry_test */
   error_status = k_cipCardOffloadConfigEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cipCardOffloadConfigEntry_t *) (dp->data))->valid);
        error_status = k_cipCardOffloadConfigEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cipCardOffloadConfigEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cipCardOffloadConfigEntry_set((cipCardOffloadConfigEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */

cipCardOffloadConfigEntry_t *
k_cipCardOffloadConfigEntry_get(serialNum, contextInfo, nominator, searchType, cipCardEntryIndex, cipCardDtrBrdIndex, cipCardSubChannelIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cipCardEntryIndex;
   long cipCardDtrBrdIndex;
   long cipCardSubChannelIndex;
{
  static cipCardOffloadConfigEntry_t cipCardOffloadConfig;
  cipCardOffloadConfigEntry_t * resultP = NULL;
  cip_vctype *vc;
  long broadcast;
  long subchannel_path, subchannel_device;
  int getType = (searchType == NEXT) ? SEARCH_NEXT : SEARCH_MATCH;
  static unsigned char path[2], device[2];
  static OctetString path_OS, device_OS, 
                     host_name_OS, router_name_OS,
                     host_appl_OS, router_appl_OS, 
                     api_host_appl_OS, api_router_appl_OS;

  index_to_path_device(cipCardSubChannelIndex, &subchannel_path, 
		       &subchannel_device);
    
  if ((vc = cip_getSubchannelOffloadConfig(&cipCardEntryIndex, &cipCardDtrBrdIndex, 
	    &subchannel_path, &subchannel_device, getType)) != NULL) {
    cipCardOffloadConfig.cipCardEntryIndex = cipCardEntryIndex;
    cipCardOffloadConfig.cipCardDtrBrdIndex = cipCardDtrBrdIndex;
    cipCardOffloadConfig.cipCardSubChannelIndex = 
        path_device_to_index(subchannel_path, subchannel_device);
    
    path[0] = vc->path >> 8;
    path[1] = vc->path & 0x00FF;
    InitOctetString(&path_OS, &path[0], sizeof(path));
    cipCardOffloadConfig.cipCardOffloadConfigPath = &path_OS;

    device[0] = vc->device >> 8;
    device[1] = vc->device & 0x00FF;
    InitOctetString(&device_OS, &device[0], sizeof(device));
    cipCardOffloadConfig.cipCardOffloadConfigDevice = &device_OS;
    
    cipCardOffloadConfig.cipCardOffloadConfigIpAddr = vc->ipaddr;

    InitOctetString(&host_name_OS, vc->hostname, strlen(vc->hostname));
    cipCardOffloadConfig.cipCardOffloadConfigHostName = &host_name_OS;
    
    InitOctetString(&router_name_OS, vc->cipname, strlen(vc->cipname));
    cipCardOffloadConfig.cipCardOffloadConfigRouterName = &router_name_OS;

    InitOctetString(&host_appl_OS, vc->hostappl, strlen(vc->hostappl));
    cipCardOffloadConfig.cipCardOffloadConfigLinkHostAppl = &host_appl_OS;

    InitOctetString(&router_appl_OS, vc->cipappl, strlen(vc->cipappl));
    cipCardOffloadConfig.cipCardOffloadConfigLinkRouterAppl = &router_appl_OS;

    InitOctetString(&api_host_appl_OS, vc->hostappl2, strlen(vc->hostappl2));
    cipCardOffloadConfig.cipCardOffloadConfigAPIHostAppl = &api_host_appl_OS;

    InitOctetString(&api_router_appl_OS, vc->cipappl2, strlen(vc->cipappl2));
    cipCardOffloadConfig.cipCardOffloadConfigAPIRouterAppl = &api_router_appl_OS;

    broadcast = (vc->flags & CIP_VC_BRDCST) ? D_cipCardOffloadConfigBroadcastEnable_true : 
                                              D_cipCardOffloadConfigBroadcastEnable_false;
    cipCardOffloadConfig.cipCardOffloadConfigBroadcastEnable = broadcast;
    cipCardOffloadConfig.cipCardOffloadConfigRowStatus = D_cipCardOffloadConfigRowStatus_active;
    SET_ALL_VALID( cipCardOffloadConfig.valid);
    resultP = &cipCardOffloadConfig;
  };
  return resultP;
}

#ifdef SETS
int
k_cipCardOffloadConfigEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cipCardOffloadConfigEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cipCardOffloadConfigEntry_set_defaults(dp)
    doList_t       *dp;
{
    return NO_ERROR;
}

int
k_cipCardOffloadConfigEntry_set(data, contextInfo, function)
   cipCardOffloadConfigEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cipCardOffloadConfigEntry_UNDO
/* add #define SR_cipCardOffloadConfigEntry_UNDO in tcpoffloadmib.h to
 * include the undo routine for the cipCardOffloadConfigEntry family.
 */
int
cipCardOffloadConfigEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cipCardOffloadConfigEntry_UNDO */
#endif /* SR_SNMPv2 */



static void init_tcpoffloadmib (subsystype *subsys)
{
    load_mib(tcpoffloadmib_OidList, tcpoffloadmib_OidListNum);
    load_oid(tcpoffloadmib_oid_table);
}

/*
 * tcpoffloadmib subsystem header
 */

#define MAJVERSION_tcpoffloadmib 1
#define MINVERSION_tcpoffloadmib 0
#define EDITVERSION_tcpoffloadmib 0

SUBSYS_HEADER(tcpoffloadmib,
              MAJVERSION_tcpoffloadmib,
              MINVERSION_tcpoffloadmib,
              EDITVERSION_tcpoffloadmib,
              init_tcpoffloadmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: cip", NULL);
      
