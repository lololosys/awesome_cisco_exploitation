/* $Id: sr_atommib_es.c,v 3.1.4.11 1996/09/06 16:40:14 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sr_atommib_es.c,v $
 *------------------------------------------------------------------
 * ATOMMIB support for the endstations.
 *
 * April 1996, Andrew Wu 
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_atommib_es.c,v $
 * Revision 3.1.4.11  1996/09/06  16:40:14  jwjang
 * CSCdi66728:  ATMSIG: SVCs are not torn-down after peer-switch reload
 * Branch: California_branch
 *
 * Revision 3.1.4.10  1996/07/10  22:20:49  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.1.4.9  1996/07/01  18:44:01  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.4.8  1996/06/22  23:47:31  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.1.4.7  1996/06/05  21:11:11  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/05/23  20:00:18  awu
 * CSCdi56245:  agent returned non-enumeration value for row status in
 *              AToM MIB.
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/05/21  09:43:04  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.1.4.4  1996/05/21  06:21:19  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1.4.3  1996/05/17  10:38:24  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.1.6.1  1996/05/05  22:49:15  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.1.4.2  1996/05/05  19:37:09  awu
 * CSCdi56759:  atommib may get into deadloop
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/25  23:03:00  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1  1996/04/10  08:29:22  awu
 * Placeholer file.
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
#include "registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "../snmp/snmp_api.h"
#include "sr_atommib_es.h"
#include "sr_atommib_es-mib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"
#include "address.h"
#include "../if/atm.h"
#include "../atm/ilmi_public.h"		/* needed by atm_registry.h */
#include "../atm/atmsig_public.h"
#include "../atm/atmsig_proto.h"
#include "../atm/atm_registry.h"


/*
 * Start of system-independent routines
 */

typedef enum trParam_type_t_ {
    E_NoTrDescr 	=1,
    E_NoClpNoScr	=2,
    E_ClpNoTagNoScr	=3,
    E_ClpTagNoScr	=4,
    E_NoClpScr		=5,
    E_ClpNoTagScr	=6,
    E_ClpTagScr		=7,
    E_MAX_trParam_type	=8
    } trParam_type_t ;

typedef struct svc_params_t_ {
    ulong trParam1;
    ulong trParam2;
    ulong trParam3;
    ulong trParam4;
    ulong trParam5;
} svc_params_t;

typedef struct pvc_params_t_ {
    unsigned *trParam1;
    unsigned *trParam2;
    unsigned *trParam3;
    unsigned *trParam4;
    unsigned *trParam5;
} pvc_params_t;

typedef struct my_atommib_es_trParam_t_ {
    ushort trParam_index;
    trParam_type_t trParam_type;
    union params_t {
	svc_params_t	svc;
	pvc_params_t	pvc;
    } params;
    atm_qos_t trQoS;
} my_atommib_es_trParam_t;


typedef struct trParam_entry_t_ { 
    wavl_node_type wavl;
    my_atommib_es_trParam_t trParam_entry;
    vc_info_t	*vc_info;
} trParam_entry_t;

typedef struct vc_atommib_param_t_ {
    ushort	TxTrafficDescr_index;
    ushort	RxTrafficDescr_index;
} vc_atommib_param_t;


ushort atm_atommib_es_last_index;			/* last index */
static wavl_handle atommib_es_handle;
static trParam_entry_t trFailed_entry;

#define ATM_TP_OID_LEN 10
static ulong 
    OIDatmNoTrDescr[ATM_TP_OID_LEN] 	= {1,3,6,1,2,1,37,1,1,1};
static ulong
    OIDatmNoClpNoScr[ATM_TP_OID_LEN] 	= {1,3,6,1,2,1,37,1,1,2};
static ulong
    OIDatmClpNoTagNoScr[ATM_TP_OID_LEN]	= {1,3,6,1,2,1,37,1,1,3};
static ulong
    OIDatmClpTagNoScr[ATM_TP_OID_LEN]   = {1,3,6,1,2,1,37,1,1,4};
static ulong
    OIDatmNoClpScr[ATM_TP_OID_LEN]      = {1,3,6,1,2,1,37,1,1,5};
static ulong
    OIDatmClpNoTagScr[ATM_TP_OID_LEN]   = {1,3,6,1,2,1,37,1,1,6};
static ulong 
    OIDatmClpTagScr[ATM_TP_OID_LEN]     = {1,3,6,1,2,1,37,1,1,7};

static OID trTypeOIDarr[E_MAX_trParam_type] = {
    {ATM_TP_OID_LEN, OIDatmNoTrDescr},
    {ATM_TP_OID_LEN, OIDatmNoClpNoScr},
    {ATM_TP_OID_LEN, OIDatmClpNoTagNoScr},
    {ATM_TP_OID_LEN, OIDatmClpTagNoScr},
    {ATM_TP_OID_LEN, OIDatmNoClpScr},
    {ATM_TP_OID_LEN, OIDatmNoClpNoScr},
    {ATM_TP_OID_LEN, OIDatmClpNoTagScr},
    {ATM_TP_OID_LEN, OIDatmClpTagScr}
    };


/*
 * 
 * 1) Static array is used, as it may be used at interrup level
 *    where we can't do malloc.
 * 2) one atm_atommib_es block per ATM port, so we need MAX_ATM_INTFC 
 *    elements
 */

/*---------------------------------------------------------------------
 * Retrieve data from the atmInterfaceConfEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
atmInterfaceConfEntry_get(
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    atmInterfaceConfEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int            carry;
    long            ifIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_atmInterfaceConfEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->ifIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_atmInterfaceMaxVpcs
    case I_atmInterfaceMaxVpcs:
       dp = (void *) (&data->atmInterfaceMaxVpcs);
       break;
#endif /* I_atmInterfaceMaxVpcs */

#ifdef I_atmInterfaceMaxVccs
    case I_atmInterfaceMaxVccs:
       dp = (void *) (&data->atmInterfaceMaxVccs);
       break;
#endif /* I_atmInterfaceMaxVccs */

#ifdef I_atmInterfaceConfVpcs
    case I_atmInterfaceConfVpcs:
       dp = (void *) (&data->atmInterfaceConfVpcs);
       break;
#endif /* I_atmInterfaceConfVpcs */

#ifdef I_atmInterfaceConfVccs
    case I_atmInterfaceConfVccs:
       dp = (void *) (&data->atmInterfaceConfVccs);
       break;
#endif /* I_atmInterfaceConfVccs */

#ifdef I_atmInterfaceMaxActiveVpiBits
    case I_atmInterfaceMaxActiveVpiBits:
       dp = (void *) (&data->atmInterfaceMaxActiveVpiBits);
       break;
#endif /* I_atmInterfaceMaxActiveVpiBits */

#ifdef I_atmInterfaceMaxActiveVciBits
    case I_atmInterfaceMaxActiveVciBits:
       dp = (void *) (&data->atmInterfaceMaxActiveVciBits);
       break;
#endif /* I_atmInterfaceMaxActiveVciBits */

#ifdef I_atmInterfaceIlmiVpi
    case I_atmInterfaceIlmiVpi:
       dp = (void *) (&data->atmInterfaceIlmiVpi);
       break;
#endif /* I_atmInterfaceIlmiVpi */

#ifdef I_atmInterfaceIlmiVci
    case I_atmInterfaceIlmiVci:
       dp = (void *) (&data->atmInterfaceIlmiVci);
       break;
#endif /* I_atmInterfaceIlmiVci */

#ifdef I_atmInterfaceAddressType
    case I_atmInterfaceAddressType:
       dp = (void *) (&data->atmInterfaceAddressType);
       break;
#endif /* I_atmInterfaceAddressType */

#ifdef I_atmInterfaceAdminAddress
    case I_atmInterfaceAdminAddress:
       dp = (void *) (CloneOctetString(data->atmInterfaceAdminAddress));
       break;
#endif /* I_atmInterfaceAdminAddress */

#ifdef I_atmInterfaceMyNeighborIpAddress
    case I_atmInterfaceMyNeighborIpAddress:
       dp = (void *) (IPToOctetString(data->atmInterfaceMyNeighborIpAddress));
       break;
#endif /* I_atmInterfaceMyNeighborIpAddress */

#ifdef I_atmInterfaceMyNeighborIfName
    case I_atmInterfaceMyNeighborIfName:
       dp = (void *) (CloneOctetString(data->atmInterfaceMyNeighborIfName));
       break;
#endif /* I_atmInterfaceMyNeighborIfName */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the atmInterfaceConfEntry data object.
 *---------------------------------------------------------------------*/
void
atmInterfaceConfEntry_free(atmInterfaceConfEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->atmInterfaceAdminAddress);
       FreeOctetString(data->atmInterfaceMyNeighborIfName);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after atmInterfaceConfEntry set/undo
 *---------------------------------------------------------------------*/
static int
atmInterfaceConfEntry_cleanup(doList_t *trash)
{
   atmInterfaceConfEntry_free(trash->data);
#ifdef SR_SNMPv2
   atmInterfaceConfEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the atmInterfaceConfEntry family
 *---------------------------------------------------------------------*/
atmInterfaceConfEntry_t *
Clone_atmInterfaceConfEntry(atmInterfaceConfEntry_t *atmInterfaceConfEntry)
{
    atmInterfaceConfEntry_t *data;

    if ((data = malloc(sizeof(atmInterfaceConfEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (atmInterfaceConfEntry), sizeof(atmInterfaceConfEntry_t));

    data->atmInterfaceAdminAddress = CloneOctetString(atmInterfaceConfEntry->atmInterfaceAdminAddress);
    data->atmInterfaceMyNeighborIfName = CloneOctetString(atmInterfaceConfEntry->atmInterfaceMyNeighborIfName);

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
atmInterfaceConfEntry_test(
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
    atmInterfaceConfEntry_t     *atmInterfaceConfEntry;
    long           ifIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &ifIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    atmInterfaceConfEntry = k_atmInterfaceConfEntry_get(-1, contextInfo, -1, EXACT, ifIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == atmInterfaceConfEntry_set) &&
            (((atmInterfaceConfEntry_t *) (dp->data)) != NULL) &&
            (((atmInterfaceConfEntry_t *) (dp->data))->ifIndex == ifIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = atmInterfaceConfEntry_set;
        dp->cleanupMethod = atmInterfaceConfEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_atmInterfaceConfEntry_UNDO
        dp->undoMethod = atmInterfaceConfEntry_undo;
#else /* SR_atmInterfaceConfEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_atmInterfaceConfEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (atmInterfaceConfEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_atmInterfaceConfEntry(atmInterfaceConfEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_atmInterfaceConfEntry(atmInterfaceConfEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {

#ifdef atmInterfaceConfEntry_READ_CREATE 
            if ( (dp->data = malloc(sizeof(atmInterfaceConfEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(atmInterfaceConfEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((atmInterfaceConfEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((atmInterfaceConfEntry_t *) (dp->data))->valid);

                error_status = k_atmInterfaceConfEntry_set_defaults(dp);
            }
#else /* atmInterfaceConfEntry_READ_CREATE */
            error_status = NO_ACCESS_ERROR;
#endif /* atmInterfaceConfEntry_READ_CREATE */
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_atmInterfaceMaxVpcs
   case I_atmInterfaceMaxVpcs:

     if ( ((value->sl_value < 0) || (value->sl_value > 4096)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMaxVpcs = value->sl_value;
     break;
#endif /* I_atmInterfaceMaxVpcs */

#ifdef I_atmInterfaceMaxVccs
   case I_atmInterfaceMaxVccs:

     if ( ((value->sl_value < 0) || (value->sl_value > 65536)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMaxVccs = value->sl_value;
     break;
#endif /* I_atmInterfaceMaxVccs */

#ifdef I_atmInterfaceMaxActiveVpiBits
   case I_atmInterfaceMaxActiveVpiBits:

     if ( ((value->sl_value < 0) || (value->sl_value > 12)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMaxActiveVpiBits = value->sl_value;
     break;
#endif /* I_atmInterfaceMaxActiveVpiBits */

#ifdef I_atmInterfaceMaxActiveVciBits
   case I_atmInterfaceMaxActiveVciBits:

     if ( ((value->sl_value < 0) || (value->sl_value > 16)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMaxActiveVciBits = value->sl_value;
     break;
#endif /* I_atmInterfaceMaxActiveVciBits */

#ifdef I_atmInterfaceIlmiVpi
   case I_atmInterfaceIlmiVpi:

     if ( ((value->sl_value < 0) || (value->sl_value > 255)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceIlmiVpi = value->sl_value;
     break;
#endif /* I_atmInterfaceIlmiVpi */

#ifdef I_atmInterfaceIlmiVci
   case I_atmInterfaceIlmiVci:

     if ( ((value->sl_value < 0) || (value->sl_value > 65535)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceIlmiVci = value->sl_value;
     break;
#endif /* I_atmInterfaceIlmiVci */

#ifdef I_atmInterfaceMyNeighborIpAddress
   case I_atmInterfaceMyNeighborIpAddress:

     if (value->os_value->length != 4) {
         return WRONG_LENGTH_ERROR;
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMyNeighborIpAddress = OctetStringToIP(value->os_value);
     break;
#endif /* I_atmInterfaceMyNeighborIpAddress */

#ifdef I_atmInterfaceMyNeighborIfName
   case I_atmInterfaceMyNeighborIfName:

     if (((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMyNeighborIfName != NULL) {
        FreeOctetString(((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMyNeighborIfName);
     }

     ((atmInterfaceConfEntry_t *) (dp->data))->atmInterfaceMyNeighborIfName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_atmInterfaceMyNeighborIfName */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in atmInterfaceConfEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_atmInterfaceConfEntry_test */
   error_status = k_atmInterfaceConfEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((atmInterfaceConfEntry_t *) (dp->data))->valid);
        error_status = k_atmInterfaceConfEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
atmInterfaceConfEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_atmInterfaceConfEntry_set((atmInterfaceConfEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the atmTrafficDescrParamEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
atmTrafficDescrParamEntry_get (
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    atmTrafficDescrParamEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            atmTrafficDescrParamIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &atmTrafficDescrParamIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_atmTrafficDescrParamEntry_get(serialNum, contextInfo, arg ,searchType, atmTrafficDescrParamIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->atmTrafficDescrParamIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_atmTrafficDescrType
    case I_atmTrafficDescrType:
       dp = (void *) (CloneOID(data->atmTrafficDescrType));
       break;
#endif /* I_atmTrafficDescrType */

#ifdef I_atmTrafficDescrParam1
    case I_atmTrafficDescrParam1:
       dp = (void *) (&data->atmTrafficDescrParam1);
       break;
#endif /* I_atmTrafficDescrParam1 */

#ifdef I_atmTrafficDescrParam2
    case I_atmTrafficDescrParam2:
       dp = (void *) (&data->atmTrafficDescrParam2);
       break;
#endif /* I_atmTrafficDescrParam2 */

#ifdef I_atmTrafficDescrParam3
    case I_atmTrafficDescrParam3:
       dp = (void *) (&data->atmTrafficDescrParam3);
       break;
#endif /* I_atmTrafficDescrParam3 */

#ifdef I_atmTrafficDescrParam4
    case I_atmTrafficDescrParam4:
       dp = (void *) (&data->atmTrafficDescrParam4);
       break;
#endif /* I_atmTrafficDescrParam4 */

#ifdef I_atmTrafficDescrParam5
    case I_atmTrafficDescrParam5:
       dp = (void *) (&data->atmTrafficDescrParam5);
       break;
#endif /* I_atmTrafficDescrParam5 */

#ifdef I_atmTrafficQoSClass
    case I_atmTrafficQoSClass:
       dp = (void *) (&data->atmTrafficQoSClass);
       break;
#endif /* I_atmTrafficQoSClass */

#ifdef I_atmTrafficDescrRowStatus
    case I_atmTrafficDescrRowStatus:
       dp = (void *) (&data->atmTrafficDescrRowStatus);
       break;
#endif /* I_atmTrafficDescrRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the atmTrafficDescrParamEntry data object.
 *---------------------------------------------------------------------*/
void
atmTrafficDescrParamEntry_free (atmTrafficDescrParamEntry_t *data)
{
   if (data != NULL) {
       FreeOID(data->atmTrafficDescrType);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after atmTrafficDescrParamEntry set/undo
 *---------------------------------------------------------------------*/
static int
atmTrafficDescrParamEntry_cleanup (doList_t *trash)
{
   atmTrafficDescrParamEntry_free(trash->data);
#ifdef SR_SNMPv2
   atmTrafficDescrParamEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the atmTrafficDescrParamEntry family
 *---------------------------------------------------------------------*/
atmTrafficDescrParamEntry_t *
Clone_atmTrafficDescrParamEntry (
				 atmTrafficDescrParamEntry_t *atmTrafficDescrParamEntry)
{
    atmTrafficDescrParamEntry_t *data;

    if ((data = malloc(sizeof(atmTrafficDescrParamEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (atmTrafficDescrParamEntry), sizeof(atmTrafficDescrParamEntry_t));

    data->atmTrafficDescrType = CloneOID(atmTrafficDescrParamEntry->atmTrafficDescrType);

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
atmTrafficDescrParamEntry_test (
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
    atmTrafficDescrParamEntry_t     *atmTrafficDescrParamEntry;
    long           atmTrafficDescrParamIndex;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &atmTrafficDescrParamIndex, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    atmTrafficDescrParamEntry = k_atmTrafficDescrParamEntry_get(-1, contextInfo, -1, EXACT, atmTrafficDescrParamIndex);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == atmTrafficDescrParamEntry_set) &&
            (((atmTrafficDescrParamEntry_t *) (dp->data)) != NULL) &&
            (((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrParamIndex == atmTrafficDescrParamIndex) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = atmTrafficDescrParamEntry_set;
        dp->cleanupMethod = atmTrafficDescrParamEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_atmTrafficDescrParamEntry_UNDO
        dp->undoMethod = atmTrafficDescrParamEntry_undo;
#else /* SR_atmTrafficDescrParamEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_atmTrafficDescrParamEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (atmTrafficDescrParamEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_atmTrafficDescrParamEntry(atmTrafficDescrParamEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_atmTrafficDescrParamEntry(atmTrafficDescrParamEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(atmTrafficDescrParamEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(atmTrafficDescrParamEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrParamIndex = (atmTrafficDescrParamIndex);
                SET_VALID(I_atmTrafficDescrParamIndex, ((atmTrafficDescrParamEntry_t *) (dp->data))->valid);

                error_status = k_atmTrafficDescrParamEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_atmTrafficDescrType
   case I_atmTrafficDescrType:

     if (((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrType != NULL) {
        FreeOID(((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrType);
     }

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrType = 
            CloneOID(value->oid_value);

     break;
#endif /* I_atmTrafficDescrType */

#ifdef I_atmTrafficDescrParam1
   case I_atmTrafficDescrParam1:

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrParam1 = value->sl_value;
     break;
#endif /* I_atmTrafficDescrParam1 */

#ifdef I_atmTrafficDescrParam2
   case I_atmTrafficDescrParam2:

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrParam2 = value->sl_value;
     break;
#endif /* I_atmTrafficDescrParam2 */

#ifdef I_atmTrafficDescrParam3
   case I_atmTrafficDescrParam3:

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrParam3 = value->sl_value;
     break;
#endif /* I_atmTrafficDescrParam3 */

#ifdef I_atmTrafficDescrParam4
   case I_atmTrafficDescrParam4:

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrParam4 = value->sl_value;
     break;
#endif /* I_atmTrafficDescrParam4 */

#ifdef I_atmTrafficDescrParam5
   case I_atmTrafficDescrParam5:

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrParam5 = value->sl_value;
     break;
#endif /* I_atmTrafficDescrParam5 */

#ifdef I_atmTrafficQoSClass
   case I_atmTrafficQoSClass:

     if ( ((value->sl_value < 0) || (value->sl_value > 255)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficQoSClass = value->sl_value;
     break;
#endif /* I_atmTrafficQoSClass */

#ifdef I_atmTrafficDescrRowStatus
   case I_atmTrafficDescrRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_atmTrafficDescrRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (atmTrafficDescrParamEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_atmTrafficDescrRowStatus_notInService) || (value->sl_value == D_atmTrafficDescrRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_atmTrafficDescrRowStatus_createAndGo) || (value->sl_value == D_atmTrafficDescrRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((atmTrafficDescrParamEntry_t *) (dp->data))->atmTrafficDescrRowStatus = value->sl_value;
     break;
#endif /* I_atmTrafficDescrRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in atmTrafficDescrParamEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_atmTrafficDescrParamEntry_test */
   error_status = k_atmTrafficDescrParamEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((atmTrafficDescrParamEntry_t *) (dp->data))->valid);
        error_status = k_atmTrafficDescrParamEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
atmTrafficDescrParamEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_atmTrafficDescrParamEntry_set((atmTrafficDescrParamEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */


/*---------------------------------------------------------------------
 * Retrieve data from the atmVclEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
atmVclEntry_get(
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    atmVclEntry_t   *data = NULL;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            ifIndex;
    long            atmVclVpi;
    long            atmVclVci;


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

    if ( (InstToInt(incoming, 2 + object->oid.length, &atmVclVci, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        atmVclVci = MAX(0, atmVclVci);
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &atmVclVpi, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        atmVclVpi = MAX(0, atmVclVpi);
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
     if ( (arg == -1) || (data = k_atmVclEntry_get(serialNum, contextInfo, arg ,searchType, ifIndex, atmVclVpi, atmVclVci)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->ifIndex;
        inst.oid_ptr[1] = data->atmVclVpi;
        inst.oid_ptr[2] = data->atmVclVci;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_atmVclAdminStatus
    case I_atmVclAdminStatus:
       dp = (void *) (&data->atmVclAdminStatus);
       break;
#endif /* I_atmVclAdminStatus */

#ifdef I_atmVclOperStatus
    case I_atmVclOperStatus:
       dp = (void *) (&data->atmVclOperStatus);
       break;
#endif /* I_atmVclOperStatus */

#ifdef I_atmVclLastChange
    case I_atmVclLastChange:
       dp = (void *) (&data->atmVclLastChange);
       break;
#endif /* I_atmVclLastChange */

#ifdef I_atmVclReceiveTrafficDescrIndex
    case I_atmVclReceiveTrafficDescrIndex:
       dp = (void *) (&data->atmVclReceiveTrafficDescrIndex);
       break;
#endif /* I_atmVclReceiveTrafficDescrIndex */

#ifdef I_atmVclTransmitTrafficDescrIndex
    case I_atmVclTransmitTrafficDescrIndex:
       dp = (void *) (&data->atmVclTransmitTrafficDescrIndex);
       break;
#endif /* I_atmVclTransmitTrafficDescrIndex */

#ifdef I_atmVccAalType
    case I_atmVccAalType:
       dp = (void *) (&data->atmVccAalType);
       break;
#endif /* I_atmVccAalType */

#ifdef I_atmVccAal5CpcsTransmitSduSize
    case I_atmVccAal5CpcsTransmitSduSize:
       dp = (void *) (&data->atmVccAal5CpcsTransmitSduSize);
       break;
#endif /* I_atmVccAal5CpcsTransmitSduSize */

#ifdef I_atmVccAal5CpcsReceiveSduSize
    case I_atmVccAal5CpcsReceiveSduSize:
       dp = (void *) (&data->atmVccAal5CpcsReceiveSduSize);
       break;
#endif /* I_atmVccAal5CpcsReceiveSduSize */

#ifdef I_atmVccAal5EncapsType
    case I_atmVccAal5EncapsType:
       dp = (void *) (&data->atmVccAal5EncapsType);
       break;
#endif /* I_atmVccAal5EncapsType */

#ifdef I_atmVclCrossConnectIdentifier
    case I_atmVclCrossConnectIdentifier:
       dp = (void *) (&data->atmVclCrossConnectIdentifier);
       break;
#endif /* I_atmVclCrossConnectIdentifier */

#ifdef I_atmVclRowStatus
    case I_atmVclRowStatus:
       dp = (void *) (&data->atmVclRowStatus);
       break;
#endif /* I_atmVclRowStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the atmVclEntry data object.
 *---------------------------------------------------------------------*/
void
atmVclEntry_free(atmVclEntry_t   *data)
{
   if (data != NULL) {

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after atmVclEntry set/undo
 *---------------------------------------------------------------------*/
static int
atmVclEntry_cleanup(doList_t *trash)
{
   atmVclEntry_free(trash->data);
#ifdef SR_SNMPv2
   atmVclEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the atmVclEntry family
 *---------------------------------------------------------------------*/
atmVclEntry_t *
Clone_atmVclEntry(atmVclEntry_t *atmVclEntry)
{
    atmVclEntry_t *data;

    if ((data = malloc(sizeof(atmVclEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (atmVclEntry), sizeof(atmVclEntry_t));


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
atmVclEntry_test(
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
    atmVclEntry_t     *atmVclEntry;
    long           ifIndex;
    long           atmVclVpi;
    long           atmVclVci;

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

    if ( (InstToInt(incoming, 1 + object->oid.length, &atmVclVpi, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 2 + object->oid.length, &atmVclVci, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    atmVclEntry = k_atmVclEntry_get(-1, contextInfo, -1, EXACT, ifIndex, atmVclVpi, atmVclVci);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == atmVclEntry_set) &&
            (((atmVclEntry_t *) (dp->data)) != NULL) &&
            (((atmVclEntry_t *) (dp->data))->ifIndex == ifIndex) &&
            (((atmVclEntry_t *) (dp->data))->atmVclVpi == atmVclVpi) &&
            (((atmVclEntry_t *) (dp->data))->atmVclVci == atmVclVci) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = atmVclEntry_set;
        dp->cleanupMethod = atmVclEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_atmVclEntry_UNDO
        dp->undoMethod = atmVclEntry_undo;
#else /* SR_atmVclEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_atmVclEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (atmVclEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_atmVclEntry(atmVclEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_atmVclEntry(atmVclEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(atmVclEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(atmVclEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

                /* Fill in reasonable default values for this new entry */
                ((atmVclEntry_t *) (dp->data))->ifIndex = (ifIndex);
                SET_VALID(I_ifIndex, ((atmVclEntry_t *) (dp->data))->valid);

                ((atmVclEntry_t *) (dp->data))->atmVclVpi = (atmVclVpi);
                SET_VALID(I_atmVclVpi, ((atmVclEntry_t *) (dp->data))->valid);

                ((atmVclEntry_t *) (dp->data))->atmVclVci = (atmVclVci);
                SET_VALID(I_atmVclVci, ((atmVclEntry_t *) (dp->data))->valid);

                error_status = k_atmVclEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_atmVclAdminStatus
   case I_atmVclAdminStatus:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((atmVclEntry_t *) (dp->data))->atmVclAdminStatus = value->sl_value;
     break;
#endif /* I_atmVclAdminStatus */

#ifdef I_atmVclReceiveTrafficDescrIndex
   case I_atmVclReceiveTrafficDescrIndex:

     ((atmVclEntry_t *) (dp->data))->atmVclReceiveTrafficDescrIndex = value->sl_value;
     break;
#endif /* I_atmVclReceiveTrafficDescrIndex */

#ifdef I_atmVclTransmitTrafficDescrIndex
   case I_atmVclTransmitTrafficDescrIndex:

     ((atmVclEntry_t *) (dp->data))->atmVclTransmitTrafficDescrIndex = value->sl_value;
     break;
#endif /* I_atmVclTransmitTrafficDescrIndex */

#ifdef I_atmVccAalType
   case I_atmVccAalType:

     if ((value->sl_value < 1) || (value->sl_value > 5)) {
         return WRONG_VALUE_ERROR;
     }

     ((atmVclEntry_t *) (dp->data))->atmVccAalType = value->sl_value;
     break;
#endif /* I_atmVccAalType */

#ifdef I_atmVccAal5CpcsTransmitSduSize
   case I_atmVccAal5CpcsTransmitSduSize:

     if ( ((value->sl_value < 1) || (value->sl_value > 65535)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmVclEntry_t *) (dp->data))->atmVccAal5CpcsTransmitSduSize = value->sl_value;
     break;
#endif /* I_atmVccAal5CpcsTransmitSduSize */

#ifdef I_atmVccAal5CpcsReceiveSduSize
   case I_atmVccAal5CpcsReceiveSduSize:

     if ( ((value->sl_value < 1) || (value->sl_value > 65535)) ) {
         return WRONG_VALUE_ERROR;
     }

     ((atmVclEntry_t *) (dp->data))->atmVccAal5CpcsReceiveSduSize = value->sl_value;
     break;
#endif /* I_atmVccAal5CpcsReceiveSduSize */

#ifdef I_atmVccAal5EncapsType
   case I_atmVccAal5EncapsType:

     if ((value->sl_value < 1) || (value->sl_value > 10)) {
         return WRONG_VALUE_ERROR;
     }

     ((atmVclEntry_t *) (dp->data))->atmVccAal5EncapsType = value->sl_value;
     break;
#endif /* I_atmVccAal5EncapsType */

#ifdef I_atmVclRowStatus
   case I_atmVclRowStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_atmVclRowStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (atmVclEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_atmVclRowStatus_notInService) || (value->sl_value == D_atmVclRowStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_atmVclRowStatus_createAndGo) || (value->sl_value == D_atmVclRowStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }

     ((atmVclEntry_t *) (dp->data))->atmVclRowStatus = value->sl_value;
     break;
#endif /* I_atmVclRowStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in atmVclEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_atmVclEntry_test */
   error_status = k_atmVclEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((atmVclEntry_t *) (dp->data))->valid);
        error_status = k_atmVclEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
atmVclEntry_set(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
  return (k_atmVclEntry_set((atmVclEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */


/*
 * static routine to check whether a given hwidb is of ATM.
 * return true if it is, otherwise return false.
 */
static boolean snmp_atm_if_test (hwidbtype *hwidb, long* dummy)
{
    return (is_atm(hwidb));
}

/*
 * construct_vpivci:
 * called to construct a longword vpivci who most significant
 * 16-bit word is the input vpi and whose lower 16-bit word is the input
 * vci.
 */ 
static inline ulong
construct_vpivci (ushort vpi, ushort vci)
{
  return((vpi<<16)|vci);
}

static inline trParam_entry_t *
wavl_to_trParam_entry (wavl_node_type *wavl)
{
    return((trParam_entry_t *)wavl);
}

static trParam_entry_t *
    locate_entry_by_trParamIndex (wavl_handle *p_handle, ulong index)
{
    trParam_entry_t goal;
    goal.trParam_entry.trParam_index = index;
    return(wavl_to_trParam_entry(wavl_search(p_handle, &goal.wavl, 0)));
}

static trParam_entry_t *
my_atmTrafficDescrParamEntry_get (int searchType,
				  long index)
{
   trParam_entry_t *found_entry, goal;
   found_entry = locate_entry_by_trParamIndex(&atommib_es_handle, index);
   if (searchType == EXACT) {
       return(found_entry);
   } else {				/* find next */
       if (found_entry != NULL) {
	   return(found_entry);
       } else {
	   goal.trParam_entry.trParam_index = index;
	   return
	       (wavl_to_trParam_entry(wavl_get_next(&atommib_es_handle,
						    &goal.wavl, 0)));
       }
   }

}

static vc_info_t *
vclEntry_get (int serialNum,
	      int searchType,
	      long *p_ifIndex,
	      long atmVclVpi,
	      long atmVclVci)
{
    hwidbtype *hwidb;
    vc_info_t *found_vc, *vc;
    atm_db_t  *atm_db;
    ulong best_vpivci, compared_vpivci, cur_vpivci;
    ushort ndx;

    hwidb = ifmib_hwidb_get(serialNum, searchType, *p_ifIndex,
			    snmp_atm_if_test, NULL);
    if (hwidb == NULL)
	return (NULL);
    
    if (searchType == EXACT) {
	atm_db = hwidb->atm_db;
	if (atm_db == NULL)
	    return (NULL);
	return(atm_vpivci_to_vc_all(atm_db, atmVclVpi, atmVclVci));
    } else { /* getnext */
	found_vc = NULL;
	/*
	 * if we are at the *p_ifIndex, then we start looking for the VC
	 * w/ (vpi,vci) == (atmVclVpi, atmVclVci). Otherwise, we
	 * start looking for the VC w/ (vpi,vci) == (0,1), which
	 * is the pair w/ least (vpi,vci) value.
	 */
 	if (hwidb->snmp_if_index == *p_ifIndex)
	    compared_vpivci = construct_vpivci(atmVclVpi, atmVclVci);
	else
	    compared_vpivci = construct_vpivci(0, 1);

	while (hwidb) {
	    *p_ifIndex = hwidb->snmp_if_index;
	    atm_db = hwidb->atm_db;  
	    if ((atm_db != NULL) && (atm_db->vc_hash_tbl != NULL)) {
	      best_vpivci =
		construct_vpivci(ATM_BITS2NUM(atm_db->config.max_vpi_bits),
				 ATM_BITS2NUM(atm_db->config.max_vci_bits));
		found_vc = atm_vpivci_to_vc_all(atm_db, atmVclVpi, atmVclVci);
		if (found_vc) {		/* found the match. we are done */
		    return (found_vc);
		}

		/* we have to look for a best match */
		for (ndx = 0; ndx < atm_db->vchash_buckets; ndx++) {
		    for (vc = atm_db->vc_hash_tbl[ndx]; vc; vc = vc->next_vc) {
			if (!ISVCACTIVE(vc) && IS_ATMVC_SVC(vc))
			    continue;
			cur_vpivci = construct_vpivci(vc->vpi, vc->vci);
			if ((cur_vpivci >= compared_vpivci)
			    && (cur_vpivci <= best_vpivci)) {
			    /*
			     * record the qualified one
			     */
			    found_vc = vc;
			    best_vpivci = cur_vpivci;

			}
		    }
		}
		/*
		 * if we already found one, then we are done.
		 * Otherwise, we have to continue to look for one.
		 */
		if (found_vc)
		  return(found_vc);
	    }
	    
	    compared_vpivci = construct_vpivci(0, 1); /* least (vpi,vci) is (0,1) */
	    found_vc = NULL;
	    hwidb = ifmib_hwidb_get(serialNum, searchType, ++*p_ifIndex, 
				    snmp_atm_if_test, NULL);
	}
	return (found_vc);
    }
}


atmInterfaceConfEntry_t *
k_atmInterfaceConfEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   long ifIndex)
{

   static atmInterfaceConfEntry_t atmInterfaceConfEntryData;
   static OctetString adminAddr, name;
       
   hwidbtype *hwidb;
   atm_db_t *atm_db;
   vc_info_t *vc_info;

   hwidb = ifmib_hwidb_get(serialNum, searchType, ifIndex, 
			   snmp_atm_if_test, NULL);
   if (!hwidb)
       return (NULL);

   atm_db = hwidb->atm_db;
   if (!atm_db)
       return (NULL);

   atmInterfaceConfEntryData.atmInterfaceMaxVpcs = 0;
   atmInterfaceConfEntryData.atmInterfaceMaxVccs = atm_db->config.max_intf_vcs;

   /*
    * routers do not support VPC yet.
    */
   atmInterfaceConfEntryData.atmInterfaceConfVpcs = 0;
   atmInterfaceConfEntryData.atmInterfaceConfVccs = atm_db->num_vc;

   atmInterfaceConfEntryData.atmInterfaceMaxActiveVpiBits =
       atm_db->config.max_vpi_bits;
   atmInterfaceConfEntryData.atmInterfaceMaxActiveVciBits =
       atm_db->config.max_vci_bits;

   if (!atm_db->ilmi_vc ||
       !(vc_info = atm_getvc_all(atm_db, atm_db->ilmi_vc))) {
       atmInterfaceConfEntryData.atmInterfaceIlmiVpi = 
       atmInterfaceConfEntryData.atmInterfaceIlmiVci = 0;
   } else {
       atmInterfaceConfEntryData.atmInterfaceIlmiVpi = vc_info->vpi;
       atmInterfaceConfEntryData.atmInterfaceIlmiVci = vc_info->vci;
   }

   atmInterfaceConfEntryData.atmInterfaceAddressType =
       D_atmInterfaceAddressType_private;

   adminAddr.length = 0;
   adminAddr.octet_ptr = NULL;
   atmInterfaceConfEntryData.atmInterfaceAdminAddress = &adminAddr;

   atmInterfaceConfEntryData.atmInterfaceMyNeighborIpAddress =
       atm_db->peeruni_ip;
   name.length = strlen(atm_db->peeruni_ifname);
   name.octet_ptr = atm_db->peeruni_ifname;
   atmInterfaceConfEntryData.atmInterfaceMyNeighborIfName = &name;

   atmInterfaceConfEntryData.ifIndex = hwidb->snmp_if_index;
   return(&atmInterfaceConfEntryData);

}

#ifdef SETS
int
k_atmInterfaceConfEntry_test(
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{
    return NO_ERROR;
}

int
k_atmInterfaceConfEntry_ready(
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_atmInterfaceConfEntry_set_defaults(doList_t       *dp)
{
    /*atmInterfaceConfEntry_t *data = (atmInterfaceConfEntry_t *) (dp->data);*/

    return NO_ERROR;
}

int
k_atmInterfaceConfEntry_set(
   atmInterfaceConfEntry_t *data,
   ContextInfo *contextInfo,
   int function)
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_atmInterfaceConfEntry_UNDO
/* add #define SR_atmInterfaceConfEntry_UNDO in atommib_es.h to
 * include the undo routine for the atmInterfaceConfEntry family.
 */
int
atmInterfaceConfEntry_undo(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_atmInterfaceConfEntry_UNDO */
#endif /* SR_SNMPv2 */

atmTrafficDescrParamEntry_t *
k_atmTrafficDescrParamEntry_get(int serialNum,
				ContextInfo *contextInfo,
				int nominator,
				int searchType,
				long atmTrafficDescrParamIndex)
{
    
    static atmTrafficDescrParamEntry_t atmTrafficDescrParamEntryData;

    trParam_entry_t *found_entry;
    pvc_params_t	*ptr;

	
   /*
    * put your code to retrieve the information here
    */
    found_entry = my_atmTrafficDescrParamEntry_get(searchType,
						  atmTrafficDescrParamIndex);
    if (found_entry == NULL)
	return (NULL);

    if (found_entry->vc_info == NULL &&
	found_entry->trParam_entry.trParam_index != TR_PARAM_FAILED_INDEX)
	return(NULL);

    if (found_entry->vc_info == NULL) {
	atmTrafficDescrParamEntryData.atmTrafficDescrParamIndex =
	    found_entry->trParam_entry.trParam_index;
	atmTrafficDescrParamEntryData.atmTrafficDescrType =
	    &trTypeOIDarr[found_entry->trParam_entry.trParam_type-1];
	atmTrafficDescrParamEntryData.atmTrafficDescrParam1 = 0;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam2 = 0;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam3 = 0;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam4 = 0;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam5 = 0;
	atmTrafficDescrParamEntryData.atmTrafficQoSClass =
	    found_entry->trParam_entry.trQoS;
	atmTrafficDescrParamEntryData.atmTrafficDescrRowStatus =
	    D_atmTrafficDescrRowStatus_active;
	return(&atmTrafficDescrParamEntryData);
    }

    
    atmTrafficDescrParamEntryData.atmTrafficDescrParamIndex =
	found_entry->trParam_entry.trParam_index;
    atmTrafficDescrParamEntryData.atmTrafficDescrType =
	&trTypeOIDarr[found_entry->trParam_entry.trParam_type-1];
   
    if (IS_ATMVC_PVC(found_entry->vc_info)) {
	
	ptr = &found_entry->trParam_entry.params.pvc;
    
	atmTrafficDescrParamEntryData.atmTrafficDescrParam1 =
	    ptr->trParam1 == NULL ? 0: ATM_KBPS_TO_CELLRATE(*ptr->trParam1);

	atmTrafficDescrParamEntryData.atmTrafficDescrParam2 =
	    ptr->trParam2 == NULL ? 0: ATM_KBPS_TO_CELLRATE(*ptr->trParam2);
	    
	atmTrafficDescrParamEntryData.atmTrafficDescrParam3 =
	    ptr->trParam3 == NULL ? 0: (*ptr->trParam3);
   
	atmTrafficDescrParamEntryData.atmTrafficDescrParam4 = 0;

	atmTrafficDescrParamEntryData.atmTrafficDescrParam5 = 0;
    } else {
	atmTrafficDescrParamEntryData.atmTrafficDescrParam1 =
	   found_entry->trParam_entry.params.svc.trParam1;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam2 =
	   found_entry->trParam_entry.params.svc.trParam2;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam3 =
	   found_entry->trParam_entry.params.svc.trParam3;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam4 =
	   found_entry->trParam_entry.params.svc.trParam4;
	atmTrafficDescrParamEntryData.atmTrafficDescrParam5 =
	   found_entry->trParam_entry.params.svc.trParam5;
    }
   
    atmTrafficDescrParamEntryData.atmTrafficQoSClass =
	found_entry->trParam_entry.trQoS;
    atmTrafficDescrParamEntryData.atmTrafficDescrRowStatus =
	D_atmTrafficDescrRowStatus_active;
    return(&atmTrafficDescrParamEntryData);

}

#ifdef SETS
int
k_atmTrafficDescrParamEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_atmTrafficDescrParamEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_atmTrafficDescrParamEntry_set_defaults(dp)
    doList_t       *dp;
{
    /*atmTrafficDescrParamEntry_t *data = (atmTrafficDescrParamEntry_t *) (dp->data);*/

    return NO_ERROR;
}

int
k_atmTrafficDescrParamEntry_set(data, contextInfo, function)
   atmTrafficDescrParamEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_atmTrafficDescrParamEntry_UNDO
/* add #define SR_atmTrafficDescrParamEntry_UNDO in atommib_es.h to
 * include the undo routine for the atmTrafficDescrParamEntry family.
 */
int
atmTrafficDescrParamEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_atmTrafficDescrParamEntry_UNDO */
#endif /* SR_SNMPv2 */

atmVclEntry_t *
k_atmVclEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   long ifIndex,
   long atmVclVpi,
   long atmVclVci)
{

   static atmVclEntry_t atmVclEntryData;
   vc_info_t 	*vc_info;
   vc_atommib_param_t *param;

   /*
    * put your code to retrieve the information here
    */

   vc_info = vclEntry_get(serialNum, searchType, &ifIndex, atmVclVpi,
			  atmVclVci);
   if (vc_info == NULL)
       return (NULL);
   param = vc_info->vc_atommib_param;
   atmVclEntryData.atmVclVpi = vc_info->vpi;
   atmVclEntryData.atmVclVci = vc_info->vci;
   atmVclEntryData.atmVclAdminStatus = D_atmVclAdminStatus_up;
   atmVclEntryData.atmVclOperStatus =
     (ISVCACTIVE(vc_info)? D_atmVclOperStatus_up: D_atmVclOperStatus_down);

   atmVclEntryData.atmVclLastChange =
     SNMP_ABSOLUTE_TIME(vc_info->last_updown_time);

   atmVclEntryData.atmVclReceiveTrafficDescrIndex =
       param->RxTrafficDescr_index;

   atmVclEntryData.atmVclTransmitTrafficDescrIndex =
       param->TxTrafficDescr_index;

   /*
    * we only support aal3/4 or aal5
    */
   if (ISVCAAL34(vc_info)) 
       atmVclEntryData.atmVccAalType = D_atmVccAalType_aal34;
   else
      atmVclEntryData.atmVccAalType = D_atmVccAalType_aal5; 

   /*
    * These should be changed when we are supporting per subinterface
    * mtu.
    */
   if (!ISVCAAL34(vc_info)) {
       atmVclEntryData.atmVccAal5CpcsTransmitSduSize =
	   vc_info->swidb->hwptr->maxmtu;
       atmVclEntryData.atmVccAal5CpcsReceiveSduSize =
	   vc_info->swidb->hwptr->maxmtu;
   } else {
       atmVclEntryData.atmVccAal5CpcsTransmitSduSize = 0;
       atmVclEntryData.atmVccAal5CpcsReceiveSduSize = 0;
   }
   
   switch(vc_info->flags & VC_FLAG_AAL_MASK) {
   case ATM_VC_AAL5SNAP:
       atmVclEntryData.atmVccAal5EncapsType =
	   D_atmVccAal5EncapsType_llcEncapsulation;
       break;
   case ATM_VC_AAL5NLPID:
   case ATM_VC_AAL5FRNLPID:
       atmVclEntryData.atmVccAal5EncapsType =
	   D_atmVccAal5EncapsType_multiprotocolFrameRelaySscs;
       break;
   case ATM_VC_AALQSAAL:
       atmVclEntryData.atmVccAal5EncapsType = D_atmVccAal5EncapsType_other;
       break;
   case ATM_VC_AAL5MUX:
       atmVclEntryData.atmVccAal5EncapsType =
	   D_atmVccAal5EncapsType_vcMultiplexRoutedProtocol;
       break;
   case ATM_VC_AAL5LANE:
       atmVclEntryData.atmVccAal5EncapsType =
	   D_atmVccAal5EncapsType_vcMultiplexLANemulation8023;
       break;
   case ATM_VC_AAL5ILMI:
       atmVclEntryData.atmVccAal5EncapsType = D_atmVccAal5EncapsType_other;
       break;
   case ATM_VC_AAL34SMDS:
       atmVclEntryData.atmVccAal5EncapsType = 0; /* error */
       break;
   default:
       atmVclEntryData.atmVccAal5EncapsType = D_atmVccAal5EncapsType_unknown;
       break;
   }
   atmVclEntryData.atmVclCrossConnectIdentifier = 0;
   atmVclEntryData.atmVclRowStatus = D_atmVclRowStatus_active;
   atmVclEntryData.ifIndex = ifIndex;
   return(&atmVclEntryData);
}

#ifdef SETS
int
k_atmVclEntry_test(
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *dp,
    ContextInfo    *contextInfo)
{

    return NO_ERROR;
}

int
k_atmVclEntry_ready(
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *dp)
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_atmVclEntry_set_defaults(doList_t       *dp)
{
    /*atmVclEntry_t *data = (atmVclEntry_t *) (dp->data);*/

    return NO_ERROR;
}

int
k_atmVclEntry_set(
   atmVclEntry_t *data,
   ContextInfo *contextInfo,
   int function)
{

   return GEN_ERROR;
}
#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_atmVclEntry_UNDO
/* add #define SR_atmVclEntry_UNDO in atommib_es.h to
 * include the undo routine for the atmVclEntry family.
 */
int
atmVclEntry_undo(
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_atmVclEntry_UNDO */
#endif /* SR_SNMPv2 */


/*
 * set_svc_trParam:
 * called to fill up fields in the traffic descriptor param
 * entry for a SVC. caller needs to ensure the VC is a SVC.
 */
static void
set_svc_trParam (trParam_entry_t *entry,
		 vc_info_t *vc_info,
		 boolean is_tx)
{
    atm_trParamQos_t connect_qos;

    if (atmSig_find_trParamQos(vc_info, &connect_qos, is_tx)) {

	entry->trParam_entry.trQoS = connect_qos.qos;

	if (connect_qos.qos == ATM_QOS_BEST_EFFORT) {
	    entry->trParam_entry.trParam_type =  E_NoClpNoScr;
	    entry->trParam_entry.params.svc.trParam1 = 
				connect_qos.usr_cellrate.PeakClp01;
	    return;
	}

	if (connect_qos.usr_cellrate.Tag != -1 &&
	    connect_qos.usr_cellrate.Tag != 0) { /* tyep (4 or 7) */
	    if (connect_qos.usr_cellrate.MaxBurstClp0 != -1) { /* type
								  7 */
		entry->trParam_entry.trParam_type = E_ClpTagScr;
		entry->trParam_entry.params.svc.trParam1 =
		    connect_qos.usr_cellrate.PeakClp01;
		entry->trParam_entry.params.svc.trParam2 =
		    connect_qos.usr_cellrate.SusClp0;
		entry->trParam_entry.params.svc.trParam3 =
		    connect_qos.usr_cellrate.MaxBurstClp0;
		return;
		
	    } else {			/* type 4 */
		entry->trParam_entry.trParam_type = E_ClpTagNoScr;
		entry->trParam_entry.params.svc.trParam1 =
		    connect_qos.usr_cellrate.PeakClp01;
		entry->trParam_entry.params.svc.trParam2 =
		    connect_qos.usr_cellrate.PeakClp0;
		return;
	    }
	} else {
	    
	    /*
	     * type (1; 2, 3, 5, or 6)
	     */

	    if (connect_qos.usr_cellrate.PeakClp01 == -1) {
		entry->trParam_entry.trParam_type = E_NoTrDescr;
		return;
	    }

	    if (connect_qos.usr_cellrate.PeakClp0 != -1) { /* type 3 */
		entry->trParam_entry.trParam_type = E_ClpNoTagNoScr;
		entry->trParam_entry.params.svc.trParam1 = 
		    connect_qos.usr_cellrate.PeakClp01;
		entry->trParam_entry.params.svc.trParam2 = 
		    connect_qos.usr_cellrate.PeakClp0;
		return;
	    }

	    if (connect_qos.usr_cellrate.SusClp0 != -1 &&
		connect_qos.usr_cellrate.MaxBurstClp0 != -1) { /* type 6 */
		entry->trParam_entry.trParam_type = E_ClpNoTagScr;
		entry->trParam_entry.params.svc.trParam1 = 
		    connect_qos.usr_cellrate.PeakClp01;
		entry->trParam_entry.params.svc.trParam2 = 
		    connect_qos.usr_cellrate.SusClp0;
		entry->trParam_entry.params.svc.trParam3 = 
		    connect_qos.usr_cellrate.MaxBurstClp0;
		return;
	    }
		
	    if (connect_qos.usr_cellrate.SusClp01 != -1 &&
		connect_qos.usr_cellrate.MaxBurstClp01 != -1) { /* type 5 */
		entry->trParam_entry.trParam_type = E_NoClpScr;
		entry->trParam_entry.params.svc.trParam1 = 
		    connect_qos.usr_cellrate.PeakClp01;
		entry->trParam_entry.params.svc.trParam2 = 
		    connect_qos.usr_cellrate.SusClp01;
		entry->trParam_entry.params.svc.trParam3 = 
		    connect_qos.usr_cellrate.MaxBurstClp01;
		return;
	    }

	    entry->trParam_entry.trParam_type = E_NoClpNoScr;
	    entry->trParam_entry.params.svc.trParam1 =
		connect_qos.usr_cellrate.PeakClp01;
	}

	    
	
    }


    
}


/*
 * set_trParam_entry:
 * called to set up a traffic descriptor param entry
 * for a VC.
 */
static void
set_trParam_entry (trParam_entry_t *entry,
		   vc_info_t *vc_info,
		   ushort index,
		   boolean is_tx)
{
    entry->trParam_entry.trParam_index = index;
    entry->vc_info = vc_info;

    if (IS_ATMVC_SVC(vc_info) && ISVCACTIVE(vc_info)) {
	set_svc_trParam(entry, vc_info, is_tx);
	return;
    } 
    if (IS_ATMVC_PVC(vc_info)) {
	if (is_tx) {
	    entry->trParam_entry.trParam_type = E_NoClpScr;
	    entry->trParam_entry.trQoS = ATM_QOS_BEST_EFFORT;
	    entry->trParam_entry.params.pvc.trParam1 =
		&vc_info->peakrate;
	    entry->trParam_entry.params.pvc.trParam2 = &vc_info->avgrate;
	    entry->trParam_entry.params.pvc.trParam3 = &vc_info->burstcells;
	    
	    entry->trParam_entry.params.pvc.trParam4 = NULL;
	    entry->trParam_entry.params.pvc.trParam5 = NULL;
	} else {
	    entry->trParam_entry.trParam_type = E_NoTrDescr;
	    entry->trParam_entry.trQoS =  ATM_QOS_BEST_EFFORT;
	    
	    entry->trParam_entry.params.pvc.trParam1 = NULL;
	    entry->trParam_entry.params.pvc.trParam2 = NULL;
	    entry->trParam_entry.params.pvc.trParam3 = NULL;
	    entry->trParam_entry.params.pvc.trParam4 = NULL;
	    entry->trParam_entry.params.pvc.trParam5 = NULL;
	}
    }

}

/*
 * get_trParam_index:
 * called to find an index in the traffic descriptor parameter table.
 * return an index, if find one available. If none available, return
 * the ATM_ATOMMIB_ES_BAD_INDEX.
 */
static short
get_trParam_index (void)
{
    ushort ndx;
    trParam_entry_t *found_entry;

    ndx = atm_atommib_es_last_index + 1;
    if (ndx < ATM_ATOMMIB_ES_MIN_INDEX || ndx > ATM_ATOMMIB_ES_MAX_INDEX)
	ndx = ATM_ATOMMIB_ES_MIN_INDEX;
    while (ndx != atm_atommib_es_last_index) {
	found_entry = locate_entry_by_trParamIndex(&atommib_es_handle, ndx);
	if (found_entry == NULL) {
	    atm_atommib_es_last_index = ndx;
	    return(ndx);
	}
	ndx++;
	if (ndx > ATM_ATOMMIB_ES_MAX_INDEX) {
	    ndx = ATM_ATOMMIB_ES_MIN_INDEX;
	}
    }
    return(ATM_ATOMMIB_ES_BAD_INDEX);
}

/*
 * compare_trParamIndex:
 * the comparison routine for the wavl serach.
 */
static enum avl_compare
compare_trParamIndex (avl_node_type *n1, avl_node_type *n2)
{
    
    trParam_entry_t *node1, *node2;
    ushort id1, id2;
    
    node1 = wavl_to_trParam_entry((wavl_normalize(n1,0)));
    node2 = wavl_to_trParam_entry((wavl_normalize(n2,0)));

    id1 = node1->trParam_entry.trParam_index;
    id2 = node2->trParam_entry.trParam_index;
    
    if (id1 == id2)
	return AVL_EQ;
    return((id1>id2) ? AVL_GT : AVL_LT);
				  
}

/*
 * atommib_es_traffic_init:
 * called to init traffic descriptor stuff.
 */
static void
atommib_es_traffic_init (void)
{

    wavl_init(&atommib_es_handle, 1, NULL, compare_trParamIndex);

    atm_atommib_es_last_index = 0;

    /*
     * fill up the trFailed_entry and add it to the tree.
     */
    trFailed_entry.trParam_entry.trParam_index = TR_PARAM_FAILED_INDEX;
    trFailed_entry.trParam_entry.trParam_type = E_NoTrDescr;
    trFailed_entry.trParam_entry.params.pvc.trParam1 = NULL;
    trFailed_entry.trParam_entry.params.pvc.trParam2 = NULL;
    trFailed_entry.trParam_entry.params.pvc.trParam3 = NULL;
    trFailed_entry.trParam_entry.params.pvc.trParam4 = NULL;
    trFailed_entry.trParam_entry.params.pvc.trParam5 = NULL;
    wavl_insert(&atommib_es_handle, &trFailed_entry.wavl);

}

/*
 * atm_atommib_es_newvc_trap:
 * called/trapped when a new vc is being created to take care of
 * correspondence required by atommib_es.
 */
static void
atm_atommib_es_newvc_trap (vc_info_t *vc_info)
{
    vc_atommib_param_t	*param;
    trParam_entry_t	*entry_tx, *entry_rx;
    short  index ;
    wavl_node_type *wavl_node_tx,*wavl_node_rx;
    
    if (!vc_info)
	return;

    /*
     * return if we don't have enough memory.
     */
    entry_tx = malloc(sizeof(*entry_tx));
    if (entry_tx == NULL)
	return;
    entry_rx = malloc(sizeof(*entry_rx));
    if (entry_rx == NULL) {
	free(entry_tx);
	return;
    }
    param = malloc(sizeof(*param));
    if (param == NULL) {
	free(entry_tx);
	free(entry_rx);
	return;
    }
    vc_info->vc_atommib_param = param;

    /*
     * now we've allocated all the memory blocks we need.
     * let's find an index for each of entry and fill in fields
     * for each of them.
     */
    
    index = get_trParam_index();
    /*
     * if we can't get index at 1st place,
     * simply free the memory blocks for the entries and return fasle.
     */
    if (index == ATM_ATOMMIB_ES_BAD_INDEX) {
	param->TxTrafficDescr_index = TR_PARAM_FAILED_INDEX;
	param->RxTrafficDescr_index = TR_PARAM_FAILED_INDEX;
	free(entry_tx);
	free(entry_rx);
	return;
    }
    /*
     * ok, we got a good index.
     */
    param->TxTrafficDescr_index = index;
    entry_tx->trParam_entry.trParam_index = index;
    set_trParam_entry(entry_tx, vc_info, index, TRUE);

    index = get_trParam_index();
    /*
     * even if we fail to get index at this stage, we can not
     * simply return, as the previous entry has not been inserted
     * into the tree yet.
     */
    if (index == ATM_ATOMMIB_ES_BAD_INDEX) { 
	param->RxTrafficDescr_index = TR_PARAM_FAILED_INDEX;
	free(entry_rx);
    } else {
	param->RxTrafficDescr_index = index;
	set_trParam_entry(entry_rx, vc_info, index, FALSE);
    }

    /*
     * insert the entries into the tree.
     */
    wavl_node_tx = wavl_insert(&atommib_es_handle, &entry_tx->wavl);
    wavl_node_rx = wavl_insert(&atommib_es_handle, &entry_rx->wavl);
    
    if (wavl_node_tx == NULL) {	    /* failed to insert the tx entry */
	param->TxTrafficDescr_index = TR_PARAM_FAILED_INDEX;
	free(entry_tx);
    }
    if (wavl_node_rx == NULL) {	    /* failed to insert the rx entry */
	param->RxTrafficDescr_index = TR_PARAM_FAILED_INDEX;
	free(entry_rx);
    }
    return;
}

/*
 * atm_atommib_es_removevc_trap:
 * called/trapped when a vc is being removed to do correspondent
 * cleanup.
 */
static void
atm_atommib_es_removevc_trap (vc_info_t *vc_info)
{
    trParam_entry_t	*found_entry;
    ushort  index ;

    if (vc_info == NULL || vc_info->vc_atommib_param == NULL)
	return;
    
    index = vc_info->vc_atommib_param->TxTrafficDescr_index;
    if (index != TR_PARAM_FAILED_INDEX ) {
	found_entry = locate_entry_by_trParamIndex(&atommib_es_handle, index);
	if (found_entry != NULL) {
	     wavl_delete(&atommib_es_handle, &found_entry->wavl);
	     free(found_entry);
	 }
    }
    index = vc_info->vc_atommib_param->RxTrafficDescr_index;
    if (index != TR_PARAM_FAILED_INDEX ) {
	found_entry = locate_entry_by_trParamIndex(&atommib_es_handle, index);
	if (found_entry != NULL) {
	     wavl_delete(&atommib_es_handle, &found_entry->wavl);
	     free(found_entry);
	 }
    }
    free(vc_info->vc_atommib_param);
    vc_info->vc_atommib_param = NULL;

    return;
}

 
static void
init_atommib_es (subsystype *subsys)
{
    load_mib(atommib_es_OidList, atommib_es_OidListNum);
    load_oid(atommib_es_oid_table);
    reg_add_atm_vc_created(atm_atommib_es_newvc_trap,
			   "atm_atommib_es_newvc_trap");
    reg_add_atm_vc_removed(atm_atommib_es_removevc_trap,
			   "atm_atomib_es_removevc_trap");
    atommib_es_traffic_init();
    
}


/*
 * atommib_es subsystem header
 */

#define MAJVERSION_atommib_es 1
#define MINVERSION_atommib_es 0
#define EDITVERSION_atommib_es 0

SUBSYS_HEADER(atommib_es,
              MAJVERSION_atommib_es,
              MINVERSION_atommib_es,
              EDITVERSION_atommib_es,
              init_atommib_es,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
