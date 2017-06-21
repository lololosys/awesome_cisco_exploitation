/* $Id: sr_encryptmib.c,v 3.1.4.12 1996/08/27 19:45:50 snijsure Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sr_encryptmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * March 1996, Subodh Nijsure (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_encryptmib.c,v $
 * Revision 3.1.4.12  1996/08/27  19:45:50  snijsure
 * CSCdi66755:  Mib object cieEsaMode is not updated to show current
 * status of ESA.
 * Branch: California_branch
 * Extract status of ESA and copy it into cieESaMode.
 *
 * Revision 3.1.4.11  1996/08/08  15:07:21  snijsure
 * CSCdi65248:  encryption mib doesnt show correct status of ESA.
 * Branch: California_branch
 * Name of epa is changed from epa to sa-encryption.
 *
 * Revision 3.1.4.10  1996/08/07  08:26:17  snijsure
 * CSCdi65248:  encryption mib doesnt show correct status of ESA.
 * Branch: California_branch
 * Retrieve ESA status using crypto_show_card_status,
 * Also assign proper value to cieEngineCardIndex, using the chassis mib
 * interface.
 *
 * Revision 3.1.4.9  1996/08/06  00:36:20  snijsure
 * CSCdi65133:  Change reference to encryption hardware to ESA from EPA.
 * Branch: California_branch
 * Changed mib objects name prefix from epa* to esa*
 *
 * Revision 3.1.4.8  1996/08/02  20:16:01  snijsure
 * CSCdi64981:  make crypto snmp modular so images can be built without
 * snmp.
 * Branch: California_branch
 * Add the crypto_snmp_conn_update function to registry...
 *
 * Revision 3.1.4.7  1996/07/15  10:05:32  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 3.1.4.6  1996/07/03  05:48:34  snijsure
 * CSCdi60205:  SNMP initiated crypto ping does not work
 * Branch: California_branch
 *
 * Revision 3.1.4.5  1996/06/28  23:40:35  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.4.4  1996/06/18  00:21:58  bew
 * CSCdi60362:  Crypto router choose 40-des over 56-des
 * Branch: California_branch
 * The strongest DES possible is now negotiated during connection setup.
 *
 * Revision 3.1.4.3  1996/06/07  00:24:36  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.4.2  1996/05/12  23:12:33  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 3.1.4.1  1996/04/19  14:56:15  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.5  1996/04/17  23:47:41  xliu
 * Branch: IosSec_branch
 * Incorporate code review comments and removing wrong slot assumption.
 *
 * Revision 3.1.2.4  1996/04/16  21:53:02  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.3  1996/04/12  16:07:45  snijsure
 * Branch: IosSec_branch
 * Remove printf and modify code per cisco
 * coding conventions.
 *
 * Revision 3.1.2.2  1996/04/10  23:48:02  snijsure
 * Branch: IosSec_branch
 * Code to set value for newly added cieTestConnCryptoMapName object.
 *
 * Revision 3.1.2.1  1996/04/10  17:40:35  snijsure
 * Branch: IosSec_branch
 * Checking in implementation of Cisco IP encryption MIB.
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
#include <ciscolib.h>
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_encryptmib.h"
#include "sr_encryptmib-mib.h"

#include "config.h"
#include "globs.h"
#include "interface_private.h"
#include "sys_registry.h"
#include "../ipc/ipc_util.h"
#include "../ipc/ipc.h"
#include "../ip/ip.h"
#include "../crypto/crypto.h"
#include "../crypto/crypto_engine_api.h"
#include "../crypto/crypto_glue_private.h" 
#include "../crypto/crypto_engine/core/crypto_engine.h"
#include "../crypto/crypto_pubkey.h"
#include "../crypto/crypto_conntable.h"
#include "../crypto/crypto_snmp_conn_list.h"
#include "../crypto/crypto_debug_flags.h"
#include "../crypto/crypto_map.h"
#include "../snmp/chassismib_api.h"
#include "../snmp/chassismib_registry.h"
#include "crypto_registry.h"
#include "crypto_snmp_conn_list.h"

#define MIN_cieTestConnSerialNumber     1
#define MAX_cieTestConnSerialNumber     2147483647

#define encryptTestQ struct encryptTestQ_

extern long crypto_no_of_connections(void);

static snmp_crypto_mykey *snmp_crypto_my_key_list_head = NULL;

encryptTestQ {
    encryptTestQ        *next;
    cieTestConnEntry_t  *entry;
    boolean             active;
};

static queuetype        snmpencryptTestQ;

/*
 * Start of system-independent routines
 */
/*---------------------------------------------------------------------
 * Retrieve data from the cieConfig family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cieConfig_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    cieConfig_t     *data=NULL;


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
     if ( (arg == -1) || (data = k_cieConfig_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cieConfiguredAlgorithms
    case I_cieConfiguredAlgorithms:
       dp = (void *) (CloneOctetString(data->cieConfiguredAlgorithms));
       break;
#endif /* I_cieConfiguredAlgorithms */

#ifdef I_cieEncryptionKeyTimeout
    case I_cieEncryptionKeyTimeout:
       dp = (void *) (&data->cieEncryptionKeyTimeout);
       break;
#endif /* I_cieEncryptionKeyTimeout */

#ifdef I_cieNumberOfCryptoEngines
    case I_cieNumberOfCryptoEngines:
       dp = (void *) (&data->cieNumberOfCryptoEngines);
       break;
#endif /* I_cieNumberOfCryptoEngines */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cieEngineStatusEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cieEngineStatusEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cieEngineStatusEntry_t *data=NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            cieEngineID;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &cieEngineID, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cieEngineStatusEntry_get(serialNum, contextInfo, arg ,searchType, cieEngineID)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->cieEngineID;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cieEngineID
    case I_cieEngineID:
       dp = (void *) (&data->cieEngineID);
       break;
#endif /* I_cieEngineID */

#ifdef I_cieEngineCardIndex
    case I_cieEngineCardIndex:
       dp = (void *) (&data->cieEngineCardIndex);
       break;
#endif /* I_cieEngineCardIndex */

#ifdef I_cieEnginePublicKey
    case I_cieEnginePublicKey:
       dp = (void *) (CloneOctetString(data->cieEnginePublicKey));
       break;
#endif /* I_cieEnginePublicKey */

#ifdef I_cieEsaTampered
    case I_cieEsaTampered:
       dp = (void *) (&data->cieEsaTampered);
       break;
#endif /* I_cieEsaTampered */

#ifdef I_cieEsaAuthenticated
    case I_cieEsaAuthenticated:
       dp = (void *) (&data->cieEsaAuthenticated);
       break;
#endif /* I_cieEsaAuthenticated */

#ifdef I_cieEsaMode
    case I_cieEsaMode:
       dp = (void *) (&data->cieEsaMode);
       break;
#endif /* I_cieEsaMode */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cieConnections family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cieConnections_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    cieConnections_t *data=NULL;


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
     if ( (arg == -1) || (data = k_cieConnections_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cieNumberOfConnections
    case I_cieNumberOfConnections:
       dp = (void *) (&data->cieNumberOfConnections);
       break;
#endif /* I_cieNumberOfConnections */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cieConnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cieConnEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cieConnEntry_t  *data=NULL;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            cieEngineID;
    long            cieConnIndex;
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
    if ((InstToInt(incoming, 1 + object->oid.length, &cieConnIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cieConnIndex = MAX(1, cieConnIndex);
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &cieEngineID, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cieConnEntry_get(serialNum, contextInfo, arg ,searchType, cieEngineID, cieConnIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->cieEngineID;
        inst.oid_ptr[1] = data->cieConnIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cieProtectedAddr
    case I_cieProtectedAddr:
       dp = (void *) (IPToOctetString(data->cieProtectedAddr));
       break;
#endif /* I_cieProtectedAddr */

#ifdef I_cieUnprotectedAddr
    case I_cieUnprotectedAddr:
       dp = (void *) (IPToOctetString(data->cieUnprotectedAddr));
       break;
#endif /* I_cieUnprotectedAddr */

#ifdef I_cieConnStatus
    case I_cieConnStatus:
       dp = (void *) (&data->cieConnStatus);
       break;
#endif /* I_cieConnStatus */

#ifdef I_ciePktsEncrypted
    case I_ciePktsEncrypted:
       dp = (void *) (&data->ciePktsEncrypted);
       break;
#endif /* I_ciePktsEncrypted */

#ifdef I_ciePktsDecrypted
    case I_ciePktsDecrypted:
       dp = (void *) (&data->ciePktsDecrypted);
       break;
#endif /* I_ciePktsDecrypted */

#ifdef I_ciePktsDropped
    case I_ciePktsDropped:
       dp = (void *) (&data->ciePktsDropped);
       break;
#endif /* I_ciePktsDropped */

#ifdef I_cieLocalTimeEstablished
    case I_cieLocalTimeEstablished:
       dp = (void *) (&data->cieLocalTimeEstablished);
       break;
#endif /* I_cieLocalTimeEstablished */

#ifdef I_cieAlgorithmType
    case I_cieAlgorithmType:
       dp = (void *) (&data->cieAlgorithmType);
       break;
#endif /* I_cieAlgorithmType */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the cieTestConnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
cieTestConnEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    cieTestConnEntry_t *data=NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            cieTestConnSerialNumber;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &cieTestConnSerialNumber, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        cieTestConnSerialNumber = MAX(1, cieTestConnSerialNumber);
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_cieTestConnEntry_get(serialNum, contextInfo, arg ,searchType, cieTestConnSerialNumber)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->cieTestConnSerialNumber;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_cieTestConnProtectedAddr
    case I_cieTestConnProtectedAddr:
       dp = (void *) (IPToOctetString(data->cieTestConnProtectedAddr));
       break;
#endif /* I_cieTestConnProtectedAddr */

#ifdef I_cieTestConnUnprotectedAddr
    case I_cieTestConnUnprotectedAddr:
       dp = (void *) (IPToOctetString(data->cieTestConnUnprotectedAddr));
       break;
#endif /* I_cieTestConnUnprotectedAddr */

#ifdef I_cieTestConnTrapOnCompletion
    case I_cieTestConnTrapOnCompletion:
       dp = (void *) (&data->cieTestConnTrapOnCompletion);
       break;
#endif /* I_cieTestConnTrapOnCompletion */

#ifdef I_cieTestConnCryptoMapName
    case I_cieTestConnCryptoMapName:
       dp = (void *) (CloneOctetString(data->cieTestConnCryptoMapName));
       break;
#endif /* I_cieTestConnCryptoMapName */

#ifdef I_cieTestConnCryptoMapTagNumber
    case I_cieTestConnCryptoMapTagNumber:
       dp = (void *) (&data->cieTestConnCryptoMapTagNumber);
       break;
#endif /* I_cieTestConnCryptoMapTagNumber */

#ifdef I_cieTestConnSessionStatus
    case I_cieTestConnSessionStatus:
       dp = (void *) (&data->cieTestConnSessionStatus);
       break;
#endif /* I_cieTestConnSessionStatus */

#ifdef I_cieTestConnEntryOwner
    case I_cieTestConnEntryOwner:
       dp = (void *) (CloneOctetString(data->cieTestConnEntryOwner));
       break;
#endif /* I_cieTestConnEntryOwner */

#ifdef I_cieTestConnEntryStatus
    case I_cieTestConnEntryStatus:
       dp = (void *) (&data->cieTestConnEntryStatus);
       break;
#endif /* I_cieTestConnEntryStatus */

    default:
       return ((VarBind *) NULL);

     }      /* switch */

     return (MakeVarBind(object, &inst, dp));

}

#ifdef SETS 

/*----------------------------------------------------------------------
 * Free the cieTestConnEntry data object.
 *---------------------------------------------------------------------*/
void
cieTestConnEntry_free(cieTestConnEntry_t *data)
{
   if (data != NULL) {
       FreeOctetString(data->cieTestConnCryptoMapName);
       FreeOctetString(data->cieTestConnEntryOwner);

       free ((char *) data);
   }
}

/*----------------------------------------------------------------------
 * cleanup after cieTestConnEntry set/undo
 *---------------------------------------------------------------------*/
static int
cieTestConnEntry_cleanup(doList_t *trash)
{
   cieTestConnEntry_free(trash->data);
#ifdef SR_SNMPv2
   cieTestConnEntry_free(trash->undodata);
#endif /* SR_SNMPv2 */
   return NO_ERROR;
}

/*----------------------------------------------------------------------
 * clone the cieTestConnEntry family
 *---------------------------------------------------------------------*/
cieTestConnEntry_t *
Clone_cieTestConnEntry( cieTestConnEntry_t *cieTestConnEntry)
{
    cieTestConnEntry_t *data=NULL;

    if ((data = malloc(sizeof(cieTestConnEntry_t))) == NULL) {
        return NULL;
    }
    memcpy((char *) (data), (char *) (cieTestConnEntry), sizeof(cieTestConnEntry_t));

    data->cieTestConnCryptoMapName = CloneOctetString(cieTestConnEntry->cieTestConnCryptoMapName);
    data->cieTestConnEntryOwner = CloneOctetString(cieTestConnEntry->cieTestConnEntryOwner);

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
cieTestConnEntry_test(incoming, object, value, doHead, doCur, contextInfo)
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
    cieTestConnEntry_t     *cieTestConnEntry;
    long           cieTestConnSerialNumber;

   /*
    * Validate the object instance
    *
    */
    if (instLength != 1) {
        return NO_CREATION_ERROR;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &cieTestConnSerialNumber, EXACT, &carry)) < 0 ) {
        error_status = NO_CREATION_ERROR;
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }
    if ( cieTestConnSerialNumber<MIN_cieTestConnSerialNumber ||
	 cieTestConnSerialNumber > MAX_cieTestConnSerialNumber )
	return NO_CREATION_ERROR;

    cieTestConnEntry = k_cieTestConnEntry_get(-1, contextInfo, -1, EXACT, cieTestConnSerialNumber);

    /*
     * Determine the element of the do-list which should contain this SET
     * request. This is decided by looking for a previous do-list element for
     * the same row.
     */
    found = 0;

    for (dp = doHead; dp != NULL; dp = dp->next) { 
         if ( (dp->setMethod == cieTestConnEntry_set) &&
            (((cieTestConnEntry_t *) (dp->data)) != NULL) &&
            (((cieTestConnEntry_t *) (dp->data))->cieTestConnSerialNumber == cieTestConnSerialNumber) ) {

            found = 1;
            break; 
        }
    }

    if (!found) {
        dp = doCur;

        dp->setMethod = cieTestConnEntry_set;
        dp->cleanupMethod = cieTestConnEntry_cleanup;
#ifdef SR_SNMPv2
#ifdef SR_cieTestConnEntry_UNDO
        dp->undoMethod = cieTestConnEntry_undo;
#else /* SR_cieTestConnEntry_UNDO */
        dp->undoMethod = NULL;
#endif /* SR_cieTestConnEntry_UNDO */
#endif /* SR_SNMPv2 */ 
        dp->state = UNKNOWN;

        if (cieTestConnEntry != NULL) {
            /* fill in existing values */
            if ((dp->data = (void *) Clone_cieTestConnEntry(cieTestConnEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#ifdef SR_SNMPv2 
            if ((dp->undodata = (void *) Clone_cieTestConnEntry(cieTestConnEntry)) == NULL) {
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
#endif /* SR_SNMPv2 */

        }
        else {
            if ( (dp->data = malloc(sizeof(cieTestConnEntry_t))) == NULL) { 
                DPRINTF((APWARN, "snmpd: Cannot allocate memory\n"));
                error_status = RESOURCE_UNAVAILABLE_ERROR;
            }
            else {
                memset(dp->data, 0, sizeof(cieTestConnEntry_t));
#ifdef SR_SNMPv2 
                dp->undodata = NULL;
#endif /* SR_SNMPv2 */

		/*
		 * Create NEW ENTRY IN THE TABLE with serial id 
		 * cieTestConnSerialNumber
		 */

                /* Fill in reasonable default values for this new entry */
                ((cieTestConnEntry_t *) (dp->data))->cieTestConnSerialNumber = (cieTestConnSerialNumber);
                SET_VALID(I_cieTestConnSerialNumber, ((cieTestConnEntry_t *) (dp->data))->valid);
                error_status = k_cieTestConnEntry_set_defaults(dp);
            }
        }
    }

    if (error_status != NO_ERROR) {
        return error_status;
    }

    switch (object->nominator) {

#ifdef I_cieTestConnProtectedAddr
   case I_cieTestConnProtectedAddr:

     if (value->os_value->length != 4) {
         return WRONG_LENGTH_ERROR;
     }

     ((cieTestConnEntry_t *) (dp->data))->cieTestConnProtectedAddr = OctetStringToIP(value->os_value);
     break;
#endif /* I_cieTestConnProtectedAddr */

#ifdef I_cieTestConnUnprotectedAddr
   case I_cieTestConnUnprotectedAddr:

     if (value->os_value->length != 4) {
         return WRONG_LENGTH_ERROR;
     }

     ((cieTestConnEntry_t *) (dp->data))->cieTestConnUnprotectedAddr = OctetStringToIP(value->os_value);
     break;
#endif /* I_cieTestConnUnprotectedAddr */

#ifdef I_cieTestConnTrapOnCompletion
   case I_cieTestConnTrapOnCompletion:

     if ((value->sl_value < 1) || (value->sl_value > 2)) {
         return WRONG_VALUE_ERROR;
     }

     ((cieTestConnEntry_t *) (dp->data))->cieTestConnTrapOnCompletion = value->sl_value;
     break;
#endif /* I_cieTestConnTrapOnCompletion */

#ifdef I_cieTestConnCryptoMapName
   case I_cieTestConnCryptoMapName:

     if (((cieTestConnEntry_t *) (dp->data))->cieTestConnCryptoMapName != NULL) {
        FreeOctetString(((cieTestConnEntry_t *) (dp->data))->cieTestConnCryptoMapName);
     }

     ((cieTestConnEntry_t *) (dp->data))->cieTestConnCryptoMapName = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cieTestConnCryptoMapName */

#ifdef I_cieTestConnCryptoMapTagNumber
   case I_cieTestConnCryptoMapTagNumber:

     ((cieTestConnEntry_t *) (dp->data))->cieTestConnCryptoMapTagNumber = value->sl_value;
     break;
#endif /* I_cieTestConnCryptoMapTagNumber */

#ifdef I_cieTestConnEntryOwner
   case I_cieTestConnEntryOwner:

     if (((cieTestConnEntry_t *) (dp->data))->cieTestConnEntryOwner != NULL) {
        FreeOctetString(((cieTestConnEntry_t *) (dp->data))->cieTestConnEntryOwner);
     }

     ((cieTestConnEntry_t *) (dp->data))->cieTestConnEntryOwner = 
         CloneOctetString(value->os_value);

     break;
#endif /* I_cieTestConnEntryOwner */

#ifdef I_cieTestConnEntryStatus
   case I_cieTestConnEntryStatus:

     if ((value->sl_value < 1) || (value->sl_value > 6)) {
         return WRONG_VALUE_ERROR;
     }

     if (value->sl_value == D_cieTestConnEntryStatus_notReady) {
         return WRONG_VALUE_ERROR;
     }

     if (cieTestConnEntry == NULL) { 	/* creating a new row */
        if((value->sl_value == D_cieTestConnEntryStatus_notInService) || (value->sl_value == D_cieTestConnEntryStatus_active)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     } else { 			/* modifying an existing row */
        if ((value->sl_value == D_cieTestConnEntryStatus_createAndGo) || (value->sl_value == D_cieTestConnEntryStatus_createAndWait)) {
            return INCONSISTENT_VALUE_ERROR;
        }
     }
     
     ((cieTestConnEntry_t *) (dp->data))->cieTestConnEntryStatus = value->sl_value;
     break;
#endif /* I_cieTestConnEntryStatus */

   default:
       DPRINTF((APALWAYS, "snmpd: Internal error (invalid nominator in cieTestConnEntry_test)\n"));
       return GEN_ERROR;

   }        /* switch */

   /* Do system dependent testing in k_cieTestConnEntry_test */
   error_status = k_cieTestConnEntry_test(object, value, dp, contextInfo);

   if (error_status == NO_ERROR) {
        SET_VALID(object->nominator, ((cieTestConnEntry_t *) (dp->data))->valid);
        error_status = k_cieTestConnEntry_ready(object, value, doHead, dp);
   }

   return (error_status);
}

/*---------------------------------------------------------------------
 * Perform the kernel-specific set function for this group of
 * related objects.
 *---------------------------------------------------------------------*/
int 
cieTestConnEntry_set(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
  return (k_cieTestConnEntry_set((cieTestConnEntry_t *) (doCur->data),
            contextInfo, doCur->state));
}

#endif /* SETS */



/*
 * Start of system-dependent routines
 */

static boolean insert_key (crypto_mykey *key)
{
   snmp_crypto_mykey  *last = NULL, *tmp, 
		      *snmp_key_entry = snmp_crypto_my_key_list_head; 

   /* Case # 1, we need to insert key at the beginning of list */ 
   if ( snmp_key_entry->key->serial > key->serial ) {
       /* Insert at the beginning of list */
       tmp = malloc(sizeof(snmp_crypto_mykey));
       if (!tmp) {
	    return FALSE;
       }
       tmp->next = snmp_key_entry;
       tmp->key = key;
       snmp_crypto_my_key_list_head = tmp; 
       return TRUE;
   }

   /* Case # 2, we need to insert key between two keys */ 
   while ( snmp_key_entry  ) {
      if ( snmp_key_entry->next && snmp_key_entry->key->serial < key->serial && 
	    snmp_key_entry->next->key->serial > key->serial ) {
          tmp = malloc(sizeof(snmp_crypto_mykey));
	  tmp->key = key;
	  tmp->next = snmp_key_entry->next;
	  snmp_key_entry->next = tmp;
	  return TRUE;
      }
      last = snmp_key_entry;
      snmp_key_entry = snmp_key_entry->next;
   }
   /* Case # 3, we need to append the key */
   tmp = malloc(sizeof(snmp_crypto_mykey));
   tmp->key = key;
   tmp->next = NULL; 
   last->next = tmp;
   return TRUE;
}

/* 
 * Create sorted list of crypto key list , sorted based on
 * serial number
 */
static int create_sorted_key_list (void)
{
   extern crypto_mykey *crypto_my_key_list_head ;
   crypto_mykey  *key_entry = crypto_my_key_list_head;

   snmp_crypto_mykey  *tmp, *snmp_key_entry = snmp_crypto_my_key_list_head; 

   int count = 0;

   if ( snmp_crypto_my_key_list_head != NULL ) {
        while ( snmp_key_entry ) {
	    tmp = snmp_key_entry->next;
	    free(snmp_key_entry);
	    snmp_key_entry = tmp;
	}
   }
   snmp_crypto_my_key_list_head = NULL;

   if ( key_entry == NULL )
       return 0;
   else {
       snmp_crypto_my_key_list_head = malloc(sizeof(snmp_crypto_mykey));
       snmp_crypto_my_key_list_head->key = key_entry;
       snmp_crypto_my_key_list_head->next = NULL;
       key_entry = key_entry->next;
       count++;
   }
   while  (key_entry) {
      if (!insert_key(key_entry)) {
	break;
      }
      key_entry = key_entry->next;
      count++;
   } 
   return count;
}


cieConfig_t *
k_cieConfig_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static cieConfig_t cieConfigData;

   unsigned char ch;
   int algorithm;
   extern ushort crypto_glue_get_session_key_timeout(void);
   ushort engineCount = 0, timeout;
   extern crypto_engine_config_t crypto_engine_tbl[]; 

   if ( nominator == I_cieNumberOfCryptoEngines ) { 
       engineCount = create_sorted_key_list();
       cieConfigData.cieNumberOfCryptoEngines = engineCount;
       return(&cieConfigData);
   }

   timeout = crypto_glue_get_session_key_timeout();
   algorithm = 0; 
   if (crypto_is_alg_set(CRYPTO_DES_56)) {
        /* printf("CRYPTO_DES_56_STR, CRYPTO_CFB64_STR\n");  */
	algorithm |= 0x1;
   }
   if (crypto_is_alg_set(CRYPTO_DES_56_8)) {
        /* printf("CRYPTO_DES_56_STR, CRYPTO_CFB8_STR\n");  */
	algorithm |= 0x2;
   }
   if (crypto_is_alg_set(CRYPTO_DES_40)) {
        /* printf("CRYPTO_DES_40_STR, CRYPTO_CFB64_STR\n"); */
	algorithm |= 0x4;
   }
   if (crypto_is_alg_set(CRYPTO_DES_40_8)) {
        /* printf("CRYPTO_DES_40_STR, CRYPTO_CFB8_STR\n"); */
	algorithm |= 0x8;
   }
   ch = algorithm;
   cieConfigData.cieConfiguredAlgorithms = MakeOctetString(&ch,1);
   cieConfigData.cieEncryptionKeyTimeout = timeout;

   return(&cieConfigData);
}

cieEngineStatusEntry_t *
k_cieEngineStatusEntry_get(serialNum, contextInfo, nominator, searchType, cieEngineID)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cieEngineID;
{
   static cieEngineStatusEntry_t cieEngineStatusEntryData;
   /*
    * put your code to retrieve the information here
    */
   ushort slot;
   ulong  status, cardIndex;
   card_entry  *cep = NULL;
   int    vip2_index,count;
   extern crypto_mykey *crypto_my_key_list_head ;
   crypto_mykey  *key_entry = crypto_my_key_list_head;
   snmp_crypto_mykey  *snmp_key_entry; 

   if ( searchType == EXACT ) {
	while ( key_entry != NULL ) {
		if ( key_entry->serial == cieEngineID )
			break;
		key_entry = key_entry->next;
	}
   	if ( key_entry == NULL )
		return (NULL);
   }
   else {
	
	/* Search the linked list of "sorted" keys so we can handle
	 * the lexically next key correctly 
	 */
	/* First create sorted list */
        count = create_sorted_key_list ();
        snmp_key_entry = snmp_crypto_my_key_list_head; 
	while ( snmp_key_entry != NULL ) {
		if ( snmp_key_entry->key->serial > cieEngineID ) {
			key_entry = snmp_key_entry->key;
			break;
		}
		else
			snmp_key_entry = snmp_key_entry->next;
	}
   	if ( snmp_key_entry == NULL )
		return (NULL);
   }

   cieEngineStatusEntryData.cieEngineID = key_entry->serial;
   slot = key_entry->slot;
   if (crypto_engine_tbl[slot].crypto_unit_type == CRYPTO_HWEPA) {

       vip2_index = reg_invoke_chassismib_find_slot_card_index(slot);
       /* See if vip2 bay 0 has ESA */
       cardIndex = reg_invoke_chassismib_find_sub_card_index(vip2_index,0);
       cep = reg_invoke_chassismib_get_cep(EXACT,&cardIndex); 
       if ( cep && cep->card_type != D_cardType_sa_encryption ) {
           /* See if vip2 bay  1 has EPA */
          cardIndex = reg_invoke_chassismib_find_sub_card_index(vip2_index, 1);
          cep = reg_invoke_chassismib_get_cep(EXACT,&cardIndex); 
          if ( cep && cep->card_type != D_cardType_sa_encryption ) 
                   cardIndex = -1; 
       }
       cieEngineStatusEntryData.cieEngineCardIndex = cardIndex;
       reg_invoke_crypto_show_card_status(slot, FALSE);
       status = crypto_engine_tbl[slot].epa_status;  
       if (EPA_TAMPERED(status))
           cieEngineStatusEntryData.cieEsaTampered = D_cieEsaTampered_true;
       else	
           cieEngineStatusEntryData.cieEsaTampered = D_cieEsaTampered_false;

       if (EPA_PASSWD_SET(status))
           cieEngineStatusEntryData.cieEsaAuthenticated = D_cieEsaAuthenticated_true;
       else	
           cieEngineStatusEntryData.cieEsaAuthenticated = D_cieEsaAuthenticated_false;
       if (EPA_BOOT_LED(status))
           cieEngineStatusEntryData.cieEsaMode = D_cieEsaMode_boot;
       else if (EPA_ERROR_LED(status))
           cieEngineStatusEntryData.cieEsaMode = D_cieEsaMode_error;
       else 
           cieEngineStatusEntryData.cieEsaMode = D_cieEsaMode_enableActive;
   }
   else { 
       cieEngineStatusEntryData.cieEngineCardIndex = 0;
       cieEngineStatusEntryData.cieEsaTampered = 0;
       cieEngineStatusEntryData.cieEsaAuthenticated = 0;
       cieEngineStatusEntryData.cieEsaMode = 0;
   } 
   cieEngineStatusEntryData.cieEnginePublicKey = 
     MakeOctetString(key_entry->public_key.key, key_entry->public_key.keylen);
   return(&cieEngineStatusEntryData);
}

cieConnections_t *
k_cieConnections_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static cieConnections_t cieConnectionsData;
   /*
    * put your code to retrieve the information here
    */
   cieConnectionsData.cieNumberOfConnections = crypto_no_of_connections();
   return(&cieConnectionsData);
}

cieConnEntry_t *
k_cieConnEntry_get(serialNum, contextInfo, nominator, searchType, cieEngineID, cieConnIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cieEngineID;
   long cieConnIndex;
{
   static cieConnEntry_t cieConnEntryData;
   extern crypto_snmp_connection *crypto_snmp_connection_head;

   /* long numberOfConnections = crypto_no_of_connections(); */
   crypto_snmp_connection *conn;

   /*
    * put your code to retrieve the information here
    */
   /*
   if ( numberOfConnections == 0 )
	return (NULL);
	*/
   if ( searchType == EXACT ) {
	/* search for exact match */
        conn = crypto_snmp_connection_head;
	while ( conn != NULL ) {
	   if ( conn->connectionIndex == cieConnIndex )
		break;	
	   conn = conn->next;
	}
	if (conn == NULL )
		return NULL;	/* EXACT match failed. */
   }
   else {
	/* This is a next request */
        conn = crypto_snmp_connection_head;
	while ( conn != NULL ) {
	   if ( cieConnIndex <= conn->connectionIndex )
		break;
	   conn = conn->next;
	}
	if (conn == NULL )
		return NULL; /* End of the table */	
   }
   cieConnEntryData.cieEngineID = conn->engineID;
   cieConnEntryData.cieConnIndex = conn->connectionIndex;
   cieConnEntryData.cieProtectedAddr = conn->protectedEntity ;
   cieConnEntryData.cieUnprotectedAddr = conn->unprotectedEntity;
   cieConnEntryData.cieConnStatus = conn->connectionStatus;
   cieConnEntryData.ciePktsEncrypted = conn->noPktsEncrypted;
   cieConnEntryData.ciePktsDecrypted = conn->noPktsDecrypted;
   cieConnEntryData.ciePktsDropped = conn->noPktsDropped;
   cieConnEntryData.cieLocalTimeEstablished = conn->localTimeEstablished;
   cieConnEntryData.cieAlgorithmType = conn->alg_type;
   return(&cieConnEntryData);
}

static encryptTestQ *
encryptTestQget( int searchType, long serialNumber)
{
    leveltype		level;
    encryptTestQ	*test;
    cieTestConnEntry_t   *entry;

    /*
     * don't allow the timeout task access to the Q
     */
    level = raise_interrupt_level(ALL_DISABLE);

    for (test = snmpencryptTestQ.qhead; test; test = test->next) {
	/*
	 * skip any entries that are being hidden from SNMP
	 */
	if (!test->active)
	    continue;

	entry = test->entry;
	/*
	 * the snmpencryptTestQ is sorted by serial number, so we can stop a NEXT
	 * search when we find a candidate
	 */
	if (((entry->cieTestConnSerialNumber == serialNumber) &&
	     (searchType == EXACT)) ||
	    ((entry->cieTestConnSerialNumber >= serialNumber) &&
	     (searchType == NEXT)))
	    break;

	if ((entry->cieTestConnSerialNumber >= serialNumber) &&
	    (searchType == EXACT)) {
	    /*
	     * gone too far for an exact match
	     */
	    test = NULL;
	    break;
	}
    }

    /*
     * re-allow access to the snmppingQ
     */
    reset_interrupt_level(level);

    return (test);
}

static void encryptTestQinsert( encryptTestQ *newEntry)
{
    leveltype		level;
    encryptTestQ	       *prev;
    encryptTestQ	       *test;
    long		serialNumber;

    /*
     * don't allow the timeout task access to the Q
     */
    level = raise_interrupt_level(ALL_DISABLE);

    serialNumber = newEntry->entry->cieTestConnSerialNumber;
    prev = NULL;
    test = snmpencryptTestQ.qhead;
    while (test) {
	if (serialNumber < test->entry->cieTestConnSerialNumber)
	    break;
	prev = test;
	test = test->next;
    }

    insqueue(&snmpencryptTestQ, newEntry, prev);

    /*
     * re-allow access to the snmppingQ
     */
    reset_interrupt_level(level);

    return;
}

cieTestConnEntry_t *
k_cieTestConnEntry_get(serialNum, contextInfo, nominator, searchType, cieTestConnSerialNumber)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   long cieTestConnSerialNumber;
{

   encryptTestQ *data;
   static cieTestConnEntry_t cieTestConnEntryData;

   while (TRUE) {
	data = encryptTestQget(searchType,cieTestConnSerialNumber);
	if ( data == NULL )
           return NULL;
	memcpy(&cieTestConnEntryData,data->entry,
		sizeof(cieTestConnEntry_t));
	if ( (searchType == EXACT) ||
		(VALID(nominator,cieTestConnEntryData.valid))) { 
			return(&cieTestConnEntryData);
	}
	/* request is get-next */
	cieTestConnSerialNumber += 1;
	if ( cieTestConnSerialNumber > MAX_cieTestConnSerialNumber)
		return NULL;
   }
}

#ifdef SETS
int
k_cieTestConnEntry_test(object, value, dp, contextInfo)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *dp;
    ContextInfo    *contextInfo;
{

    return NO_ERROR;
}

int
k_cieTestConnEntry_ready(object, value, doHead, dp)
    ObjectInfo     *object;
    ObjectSyntax   *value;
    doList_t       *doHead;
    doList_t       *dp;
{

    dp->state = ADD_MODIFY;
    return NO_ERROR;
}

int
k_cieTestConnEntry_set_defaults(dp)
    doList_t       *dp;
{
    cieTestConnEntry_t *data = (cieTestConnEntry_t *) (dp->data); 

    data->cieTestConnTrapOnCompletion = D_cieTestConnTrapOnCompletion_false;
    SET_VALID(I_cieTestConnTrapOnCompletion, data->valid);

    data->cieTestConnEntryStatus = D_cieTestConnEntryStatus_createAndGo;
    SET_VALID(I_cieTestConnEntryStatus, data->valid);

    return NO_ERROR;
}

boolean snmp_test_crypto (long sourceAddr, long destAddr, 
		char *map, uint tagnum)
{
    int status;
    addrtype peAddr, upeAddr;
    int alg;
    char **peers;
    boolean ok;
    ok=find_crypto_map_policy(map, tagnum, &peAddr, &upeAddr, &alg, &peers);
    if (!ok) {
        return FALSE;
    }

    if ( crypto_sm_debug ) 
        buginf("\nCRYPTO SNMP: Sending CIM to: %i from: %i", sourceAddr, 
	  destAddr); 

    crypto_conn_entry_initiate_ip(&status,peAddr.ip_addr,upeAddr.ip_addr,
				 sourceAddr,destAddr,0, alg, peers, 0);
    return TRUE;
}

int
k_cieTestConnEntry_set(data, contextInfo, function)
   cieTestConnEntry_t *data;
   ContextInfo *contextInfo;
   int function;
{
    encryptTestQ	       *current;
    cieTestConnEntry_t   *entry;
    char *mapName;

    current = encryptTestQget(EXACT, data->cieTestConnSerialNumber);

    switch (function) {
    case DELETE:
	if (current) {
		buginf("Request to delete row entry\n");
	}
	return (NO_ERROR);
	
    case ADD_MODIFY:
	if (current == NULL) {
	    /*
	     * adding a new row
	     */
	    current = malloc(sizeof(encryptTestQ));
	    if (current == NULL)
		return (RESOURCE_UNAVAILABLE_ERROR);

	    entry = malloc(sizeof(cieTestConnEntry_t));
	    if (entry == NULL) {
		free(current);
		return (RESOURCE_UNAVAILABLE_ERROR);
	    }

	    current->next = NULL;
	    current->active = TRUE;
	    current->entry = entry;
	    entry->cieTestConnSerialNumber = data->cieTestConnSerialNumber;
	    entry->cieTestConnEntryStatus = data->cieTestConnEntryStatus;
	    entry->cieTestConnEntryStatus = D_cieTestConnEntryStatus_active;
	    encryptTestQinsert(current);
	} else {
	    entry = current->entry;
	}

	if ( VALID(I_cieTestConnProtectedAddr,data->valid)  ) {
	    SET_VALID(I_cieTestConnProtectedAddr,entry->valid);
	    entry->cieTestConnProtectedAddr = data->cieTestConnProtectedAddr;
	}

	if ( VALID(I_cieTestConnUnprotectedAddr,data->valid)  ) {
	    SET_VALID(I_cieTestConnUnprotectedAddr,entry->valid);
	    entry->cieTestConnUnprotectedAddr = data->cieTestConnUnprotectedAddr;
	}
	if ( VALID(I_cieTestConnTrapOnCompletion,data->valid)  ) {
	    SET_VALID(I_cieTestConnTrapOnCompletion,entry->valid);
	    entry->cieTestConnTrapOnCompletion = data->cieTestConnTrapOnCompletion;
	}
	if ( VALID(I_cieTestConnEntryOwner,data->valid)  ) {
	    SET_VALID(I_cieTestConnEntryOwner,entry->valid);
	    if (entry->cieTestConnEntryOwner != NULL) {
                FreeOctetString(entry->cieTestConnEntryOwner);
            }
	    entry->cieTestConnEntryOwner = CloneOctetString(data->cieTestConnEntryOwner);
	}

	if ( VALID(I_cieTestConnCryptoMapName,data->valid)  ) {
	    SET_VALID(I_cieTestConnCryptoMapName,entry->valid);
	    if (entry->cieTestConnCryptoMapName != NULL) {
                FreeOctetString(entry->cieTestConnCryptoMapName);
            }
	    entry->cieTestConnCryptoMapName = CloneOctetString(data->cieTestConnCryptoMapName);
	}

	if ( VALID(I_cieTestConnCryptoMapTagNumber,data->valid)  ) {
	    SET_VALID(I_cieTestConnCryptoMapTagNumber,entry->valid);
	    entry->cieTestConnCryptoMapTagNumber = data->cieTestConnCryptoMapTagNumber;
	}


	if (VALID(I_cieTestConnEntryStatus, data->valid)) {
	    /*
	     * turn createAndGo into active since they're the same
	     * thing at this point
	     */
	    if (data->cieTestConnEntryStatus == D_cieTestConnEntryStatus_createAndGo)
		data->cieTestConnEntryStatus =  D_cieTestConnEntryStatus_active;

	    SET_VALID(I_cieTestConnEntryStatus, entry->valid);
	    entry->cieTestConnEntryStatus = data->cieTestConnEntryStatus;

            mapName = malloc(entry->cieTestConnCryptoMapName->length+1);
	    if ( mapName == NULL )
                return (GEN_ERROR);

            strncpy(mapName,entry->cieTestConnCryptoMapName->octet_ptr,
                entry->cieTestConnCryptoMapName->length);
            mapName[entry->cieTestConnCryptoMapName->length] = '\0';

	    if ( !snmp_test_crypto(entry->cieTestConnProtectedAddr,
		     entry->cieTestConnUnprotectedAddr,
		     mapName, 
		     entry->cieTestConnCryptoMapTagNumber) )
                entry->cieTestConnSessionStatus = D_cieTestConnSessionStatus_badCryptoMapName;
	    else
                entry->cieTestConnSessionStatus = D_cieTestConnSessionStatus_inProgress;
	    free(mapName);

	    SET_VALID(I_cieTestConnSessionStatus, entry->valid);
	}
	return (NO_ERROR);
    }
    return (GEN_ERROR);
}

#endif /* SETS */

#ifdef SR_SNMPv2
#ifdef SR_cieTestConnEntry_UNDO
/* add #define SR_cieTestConnEntry_UNDO in encryptmib.h to
 * include the undo routine for the cieTestConnEntry family.
 */
int
cieTestConnEntry_undo(doHead, doCur, contextInfo)
    doList_t       *doHead;
    doList_t       *doCur;
    ContextInfo    *contextInfo;
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_cieTestConnEntry_UNDO */
#endif /* SR_SNMPv2 */

/*
cieMIBTrapPrefix OBJECT IDENTIFIER ::= { ciscoIpEncryptionMIB 2 }
cieMIBTraps OBJECT IDENTIFIER ::= { cieMIBTrapPrefix 0 }

cieTestCompletion NOTIFICATION-TYPE
	OBJECTS    {    
		cieTestConnSessionStatus,
		cieTestConnProtectedAddr,
		cieTestConnUnprotectedAddr
	}
	STATUS     current
	DESCRIPTION
		"A cieTestCompletion trap is sent at the completion
		of a crypto session establishment if such a trap was requested
		when the sequence was initiated.  "
	::= { cieMIBTraps 1 }

*/

#define TRAP_NUMBER 1
#define TRAP_VARBIND_COUNT 3

/*
 * The OID that identifies the SNMPv2 trap
 */
static const OID trapOID = 
    {LNcieTestCompletion, (ulong *)IDcieTestCompletion};

/*
 * The OID that identifies the SNMPv1 enterprise. This is identical to the
 * SNMPv2 trap OID with the last two subidentifiers removed
 */

static const OID enterpriseOID =
    {LNcieTestCompletion - 2, (ulong *)IDcieTestCompletion};

static const OID trap_varbinds[TRAP_VARBIND_COUNT] = {
    {LNcieTestConnSessionStatus, (ulong *)IDcieTestConnSessionStatus},
    {LNcieTestConnProtectedAddr, (ulong *)IDcieTestConnProtectedAddr},
    {LNcieTestConnUnprotectedAddr, (ulong *)IDcieTestConnUnprotectedAddr}
    };

/*
 * ping completion trap
 * called when a ping has completed and a completion trap was requested
 */
static void
snmp_test_crypto_completion_trap (cieTestConnEntry_t  *entry)
{
    int         vbIndex;
    int		vbCount;
    OID        *vbList[TRAP_VARBIND_COUNT+1];
    OID         instanceOID;
    ulong       instance[1];
    char	trapoid_str[80];

    if (MakeDotFromOID((OID *)&trapOID, trapoid_str) == -1) {
	return;
    }	

    instance[0] = entry->cieTestConnSerialNumber;
    instanceOID.oid_ptr = instance;
    instanceOID.length = 1;
    vbCount = TRAP_VARBIND_COUNT;

    /*
     * create the varbind list
     */
    for (vbIndex = 0; vbIndex < vbCount; vbIndex++) {
        vbList[vbIndex] = CatOID((OID *) &trap_varbinds[vbIndex],
				 &instanceOID);
    }
    vbList[vbIndex] = NULL;

    /*
     * tell snmp to do something
     */
     snmp_trap(ENTERPRISE_TRAP, TRAP_NUMBER, vbList,
	       (OID *)&enterpriseOID, trapoid_str);

    /*
     * free data allocated by the CatOID service
     */
    for (vbIndex = 0; vbIndex < vbCount; vbIndex++) {
        FreeOID(vbList[vbIndex]);
    }
}

void crypto_snmp_conn_update(void)
{
    encryptTestQ       *test;
    extern boolean crypto_ping_update_entry(unsigned long peAddr, 
        unsigned long upeAddr, long *cieTestConnEntryStatus);

    encryptTestQ       *next;
    boolean	completeFlag;

    for (test = snmpencryptTestQ.qhead; test; test = next) {
	next = test->next;
        if ( crypto_sm_debug ) 
	    buginf("\nCRYPTO SNMP: Trap on completion == %d\n",test->entry->cieTestConnTrapOnCompletion);
	completeFlag = FALSE;
	completeFlag = crypto_ping_update_entry( 
	    test->entry->cieTestConnProtectedAddr,
	    test->entry->cieTestConnUnprotectedAddr,
            &test->entry->cieTestConnSessionStatus);
	/* 
	 * If completeFlag is TRUE, it means connection has transitioned into
	 * its final state, if so check the if user requested trap when
	 * entry was created, if yes send the trap and remove the entry 
	 * from the table
	 */
	if ( completeFlag ) {
	     if ( test->entry->cieTestConnTrapOnCompletion 
		       == D_cieTestConnTrapOnCompletion_true) 
                 if ( crypto_sm_debug ) 
                    buginf("\nCRYPTO SNMP: Send trap PE address %i, UPE address %i, status is %d, serial number = %d\n", test->entry->cieTestConnProtectedAddr, test->entry->cieTestConnUnprotectedAddr,test->entry->cieTestConnEntryStatus,test->entry->cieTestConnSessionStatus);

                 snmp_test_crypto_completion_trap (test->entry);
	         unqueue(&snmpencryptTestQ, test);
	         cieTestConnEntry_free(test->entry);
	         free(test);
	}
    }
}

void
init_encryptmib (subsystype *subsys)
{
    reg_add_crypto_snmp_conn_update(crypto_snmp_conn_update,
	"crypto_snmp_conn_update"); 
    queue_init(&snmpencryptTestQ, 0);
    load_mib(encryptmib_OidList, encryptmib_OidListNum);
    load_oid(encryptmib_oid_table);
}

#define MAJVERSION_encryptmib 1
#define MINVERSION_encryptmib 0
#define EDITVERSION_encryptmib 0

SUBSYS_HEADER(encryptmib,
              MAJVERSION_encryptmib,
              MINVERSION_encryptmib,
              EDITVERSION_encryptmib,
              init_encryptmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
