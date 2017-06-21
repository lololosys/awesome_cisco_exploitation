/* $Id: sr_tn3270servermib.c,v 3.1.2.4 1996/09/10 06:36:17 bchan Exp $
 * $Source: /release/112/cvs/Xsys/hes/sr_tn3270servermib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * July 1996, Barry Chan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_tn3270servermib.c,v $
 * Revision 3.1.2.4  1996/09/10  06:36:17  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/09/02  08:19:34  bchan
 * CSCdi67808:  Change tn3270-server mib
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/07/17  06:52:10  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/07/15  07:19:18  bchan
 * CSCdi62509:  Change tn3270 server mib
 * Branch: California_branch
 *
 * Revision 3.1  1996/07/12  02:39:24  bchan
 * *** empty log message ***
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This file is machine generated
 * Do not attempt to edit it or place it under CVS control
 */

#include "master.h"
#include "subsys.h"
#include "../snmp/snmp_api.h"
#include "sr_tn3270servermib.h"
#include "sr_tn3270servermib-mib.h"
#include "channelmib_utils.h"
#include "interface_private.h"
#include "if_tn3270s.h"

/*
 * Start of system-independent routines
 */


/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */




#ifdef HAVE_STDLIB_H
#endif	/* HAVE_STDLIB_H */

#ifdef HAVE_MALLOC_H
#endif	/* HAVE_MALLOC_H */


/*---------------------------------------------------------------------
 * Retrieve data from the tn3270sGlobalEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
tn3270sGlobalEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    tn3270sGlobalEntry_t *data;
    SR_UINT32   buffer[1];
    OID             inst;
    int             carry;
    SR_INT32        tn3270sIndex;

    data = NULL;
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

    if ( (InstToInt(incoming, 0 + object->oid.length, &tn3270sIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_tn3270sGlobalEntry_get(serialNum, contextInfo, arg ,searchType, tn3270sIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 1;
        inst.oid_ptr[0] = data->tn3270sIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_tn3270sCpuCard
    case I_tn3270sCpuCard:
       dp = (void *) (CloneOctetString(data->tn3270sCpuCard));
       break;
#endif /* I_tn3270sCpuCard */

#ifdef I_tn3270sMaxLus
    case I_tn3270sMaxLus:
       dp = (void *) (&data->tn3270sMaxLus);
       break;
#endif /* I_tn3270sMaxLus */

#ifdef I_tn3270sLusInUse
    case I_tn3270sLusInUse:
       dp = (void *) (&data->tn3270sLusInUse);
       break;
#endif /* I_tn3270sLusInUse */

#ifdef I_tn3270sStartupTime
    case I_tn3270sStartupTime:
       dp = (void *) (&data->tn3270sStartupTime);
       break;
#endif /* I_tn3270sStartupTime */

#ifdef I_tn3270sGlobalTcpPort
    case I_tn3270sGlobalTcpPort:
       dp = (void *) (&data->tn3270sGlobalTcpPort);
       break;
#endif /* I_tn3270sGlobalTcpPort */

#ifdef I_tn3270sGlobalIdleTimeout
    case I_tn3270sGlobalIdleTimeout:
       dp = (void *) (&data->tn3270sGlobalIdleTimeout);
       break;
#endif /* I_tn3270sGlobalIdleTimeout */

#ifdef I_tn3270sGlobalKeepAlive
    case I_tn3270sGlobalKeepAlive:
       dp = (void *) (&data->tn3270sGlobalKeepAlive);
       break;
#endif /* I_tn3270sGlobalKeepAlive */

#ifdef I_tn3270sGlobalUnbindAction
    case I_tn3270sGlobalUnbindAction:
       dp = (void *) (&data->tn3270sGlobalUnbindAction);
       break;
#endif /* I_tn3270sGlobalUnbindAction */

#ifdef I_tn3270sGlobalGenericPool
    case I_tn3270sGlobalGenericPool:
       dp = (void *) (&data->tn3270sGlobalGenericPool);
       break;
#endif /* I_tn3270sGlobalGenericPool */

#ifdef I_tn3270sTimingMarkSupported
    case I_tn3270sTimingMarkSupported:
       dp = (void *) (&data->tn3270sTimingMarkSupported);
       break;
#endif /* I_tn3270sTimingMarkSupported */

#ifdef I_tn3270sRunningTime
    case I_tn3270sRunningTime:
       dp = (void *) (&data->tn3270sRunningTime);
       break;
#endif /* I_tn3270sRunningTime */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the tn3270sStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
tn3270sStatsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    tn3270sStatsEntry_t *data;
    SR_UINT32   buffer[6];
    OID             inst;
    int             carry;
    SR_INT32        tn3270sIndex;
    SR_UINT32       tn3270sStatsServerAddr;
    SR_INT32        tn3270sStatsServerTcpPort;


    data = NULL;
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

    if ( (InstToInt(incoming, 5 + object->oid.length, &tn3270sStatsServerTcpPort, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        tn3270sStatsServerTcpPort = MAX(0, tn3270sStatsServerTcpPort);
    }

    if ( (InstToIP(incoming, 1 + object->oid.length, &tn3270sStatsServerAddr, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &tn3270sIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_tn3270sStatsEntry_get(serialNum, contextInfo, arg ,searchType, tn3270sIndex, tn3270sStatsServerAddr, tn3270sStatsServerTcpPort)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 6;
        inst.oid_ptr[0] = data->tn3270sIndex;
        inst.oid_ptr[1] = (data->tn3270sStatsServerAddr >> 24) & 0xff;
        inst.oid_ptr[2] = (data->tn3270sStatsServerAddr >> 16) & 0xff;
        inst.oid_ptr[3] = (data->tn3270sStatsServerAddr >> 8) & 0xff;
        inst.oid_ptr[4] = (data->tn3270sStatsServerAddr >> 0) & 0xff;
        inst.oid_ptr[5] = data->tn3270sStatsServerTcpPort;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_tn3270sStatsMaxSess
    case I_tn3270sStatsMaxSess:
       dp = (void *) (&data->tn3270sStatsMaxSess);
       break;
#endif /* I_tn3270sStatsMaxSess */

#ifdef I_tn3270sStatsSpareSess
    case I_tn3270sStatsSpareSess:
       dp = (void *) (&data->tn3270sStatsSpareSess);
       break;
#endif /* I_tn3270sStatsSpareSess */

#ifdef I_tn3270sStatsConnectsIn
    case I_tn3270sStatsConnectsIn:
       dp = (void *) (&data->tn3270sStatsConnectsIn);
       break;
#endif /* I_tn3270sStatsConnectsIn */

#ifdef I_tn3270sStatsDisconnects
    case I_tn3270sStatsDisconnects:
       dp = (void *) (&data->tn3270sStatsDisconnects);
       break;
#endif /* I_tn3270sStatsDisconnects */

#ifdef I_tn3270sStatsTN3270ConnectsFailed
    case I_tn3270sStatsTN3270ConnectsFailed:
       dp = (void *) (&data->tn3270sStatsTN3270ConnectsFailed);
       break;
#endif /* I_tn3270sStatsTN3270ConnectsFailed */

#ifdef I_tn3270sStatsInboundChains
    case I_tn3270sStatsInboundChains:
       dp = (void *) (&data->tn3270sStatsInboundChains);
       break;
#endif /* I_tn3270sStatsInboundChains */

#ifdef I_tn3270sStatsOutboundChains
    case I_tn3270sStatsOutboundChains:
       dp = (void *) (&data->tn3270sStatsOutboundChains);
       break;
#endif /* I_tn3270sStatsOutboundChains */

#ifdef I_tn3270sStatsSampledHostResponses
    case I_tn3270sStatsSampledHostResponses:
       dp = (void *) (&data->tn3270sStatsSampledHostResponses);
       break;
#endif /* I_tn3270sStatsSampledHostResponses */

#ifdef I_tn3270sStatsNetSampledHostResponseTime
    case I_tn3270sStatsNetSampledHostResponseTime:
       dp = (void *) (&data->tn3270sStatsNetSampledHostResponseTime);
       break;
#endif /* I_tn3270sStatsNetSampledHostResponseTime */

#ifdef I_tn3270sStatsSampledClientResponses
    case I_tn3270sStatsSampledClientResponses:
       dp = (void *) (&data->tn3270sStatsSampledClientResponses);
       break;
#endif /* I_tn3270sStatsSampledClientResponses */

#ifdef I_tn3270sStatsNetSampledClientResponseTime
    case I_tn3270sStatsNetSampledClientResponseTime:
       dp = (void *) (&data->tn3270sStatsNetSampledClientResponseTime);
       break;
#endif /* I_tn3270sStatsNetSampledClientResponseTime */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the tn3270sPuEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
tn3270sPuEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    tn3270sPuEntry_t *data;
    SR_UINT32   buffer[2];
    OID             inst;
    int             carry;
    SR_INT32        tn3270sIndex;
    SR_INT32        tn3270sPuIndex;

    data = NULL;
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

    if ( (InstToInt(incoming, 1 + object->oid.length, &tn3270sPuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &tn3270sIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_tn3270sPuEntry_get(serialNum, contextInfo, arg ,searchType, tn3270sIndex, tn3270sPuIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 2;
        inst.oid_ptr[0] = data->tn3270sIndex;
        inst.oid_ptr[1] = data->tn3270sPuIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_tn3270sPuIpAddr
    case I_tn3270sPuIpAddr:
       dp = (void *) (IPToOctetString(data->tn3270sPuIpAddr));
       break;
#endif /* I_tn3270sPuIpAddr */

#ifdef I_tn3270sPuTcpPort
    case I_tn3270sPuTcpPort:
       dp = (void *) (&data->tn3270sPuTcpPort);
       break;
#endif /* I_tn3270sPuTcpPort */

#ifdef I_tn3270sPuIdleTimeout
    case I_tn3270sPuIdleTimeout:
       dp = (void *) (&data->tn3270sPuIdleTimeout);
       break;
#endif /* I_tn3270sPuIdleTimeout */

#ifdef I_tn3270sPuKeepAlive
    case I_tn3270sPuKeepAlive:
       dp = (void *) (&data->tn3270sPuKeepAlive);
       break;
#endif /* I_tn3270sPuKeepAlive */

#ifdef I_tn3270sPuUnbindAction
    case I_tn3270sPuUnbindAction:
       dp = (void *) (&data->tn3270sPuUnbindAction);
       break;
#endif /* I_tn3270sPuUnbindAction */

#ifdef I_tn3270sPuGenericPool
    case I_tn3270sPuGenericPool:
       dp = (void *) (&data->tn3270sPuGenericPool);
       break;
#endif /* I_tn3270sPuGenericPool */

#ifdef I_tn3270sPuState
    case I_tn3270sPuState:
       dp = (void *) (&data->tn3270sPuState);
       break;
#endif /* I_tn3270sPuState */

#ifdef I_tn3270sPuType
    case I_tn3270sPuType:
       dp = (void *) (&data->tn3270sPuType);
       break;
#endif /* I_tn3270sPuType */

#ifdef I_tn3270sPuLuSeed
    case I_tn3270sPuLuSeed:
       dp = (void *) (CloneOctetString(data->tn3270sPuLuSeed));
       break;
#endif /* I_tn3270sPuLuSeed */

#ifdef I_tn3270sLocalSapAddress
    case I_tn3270sLocalSapAddress:
       dp = (void *) (&data->tn3270sLocalSapAddress);
       break;
#endif /* I_tn3270sLocalSapAddress */

#ifdef I_tn3270sRemoteSapAddress
    case I_tn3270sRemoteSapAddress:
       dp = (void *) (&data->tn3270sRemoteSapAddress);
       break;
#endif /* I_tn3270sRemoteSapAddress */

#ifdef I_tn3270sRemoteMacAddress
    case I_tn3270sRemoteMacAddress:
       dp = (void *) (CloneOctetString(data->tn3270sRemoteMacAddress));
       break;
#endif /* I_tn3270sRemoteMacAddress */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the tn3270sIpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
tn3270sIpEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    tn3270sIpEntry_t *data;
    SR_UINT32   buffer[6];
    OID             inst;
    int             carry;
    SR_INT32        tn3270sIndex;
    SR_UINT32       tn3270sIpClientAddr;
    SR_INT32        tn3270sIpClientTcpPort;


    data = NULL;
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

    if ( (InstToInt(incoming, 5 + object->oid.length, &tn3270sIpClientTcpPort, searchType, &carry)) < 0 ) {
       arg = -1;
    }
    if (searchType == NEXT) {
        tn3270sIpClientTcpPort = MAX(0, tn3270sIpClientTcpPort);
    }

    if ( (InstToIP(incoming, 1 + object->oid.length, &tn3270sIpClientAddr, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &tn3270sIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_tn3270sIpEntry_get(serialNum, contextInfo, arg ,searchType, tn3270sIndex, tn3270sIpClientAddr, tn3270sIpClientTcpPort)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 6;
        inst.oid_ptr[0] = data->tn3270sIndex;
        inst.oid_ptr[1] = (data->tn3270sIpClientAddr >> 24) & 0xff;
        inst.oid_ptr[2] = (data->tn3270sIpClientAddr >> 16) & 0xff;
        inst.oid_ptr[3] = (data->tn3270sIpClientAddr >> 8) & 0xff;
        inst.oid_ptr[4] = (data->tn3270sIpClientAddr >> 0) & 0xff;
        inst.oid_ptr[5] = data->tn3270sIpClientTcpPort;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_tn3270sIpPuIndex
    case I_tn3270sIpPuIndex:
       dp = (void *) (&data->tn3270sIpPuIndex);
       break;
#endif /* I_tn3270sIpPuIndex */

#ifdef I_tn3270sIpLuIndex
    case I_tn3270sIpLuIndex:
       dp = (void *) (&data->tn3270sIpLuIndex);
       break;
#endif /* I_tn3270sIpLuIndex */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the tn3270sLuEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
tn3270sLuEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    tn3270sLuEntry_t *data;
    SR_UINT32   buffer[3];
    OID             inst;
    int             carry;
    SR_INT32        tn3270sIndex;
    SR_INT32        tn3270sLuPuIndex;
    SR_INT32        tn3270sLuIndex;

    data = NULL;
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

    if ( (InstToInt(incoming, 2 + object->oid.length, &tn3270sLuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 1 + object->oid.length, &tn3270sLuPuIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if ( (InstToInt(incoming, 0 + object->oid.length, &tn3270sIndex, searchType, &carry)) < 0 ) {
       arg = -1;
    }

    if (carry) {
       arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_tn3270sLuEntry_get(serialNum, contextInfo, arg ,searchType, tn3270sIndex, tn3270sLuPuIndex, tn3270sLuIndex)) == NULL) {
        arg = -1;
     }

     else {
       /*
        * Build instance information
        */
        inst.oid_ptr = buffer;
        inst.length = 3;
        inst.oid_ptr[0] = data->tn3270sIndex;
        inst.oid_ptr[1] = data->tn3270sLuPuIndex;
        inst.oid_ptr[2] = data->tn3270sLuIndex;
     }


     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_tn3270sLuClientAddr
    case I_tn3270sLuClientAddr:
       dp = (void *) (IPToOctetString(data->tn3270sLuClientAddr));
       break;
#endif /* I_tn3270sLuClientAddr */

#ifdef I_tn3270sLuClientTcpPort
    case I_tn3270sLuClientTcpPort:
       dp = (void *) (&data->tn3270sLuClientTcpPort);
       break;
#endif /* I_tn3270sLuClientTcpPort */

#ifdef I_tn3270sLuTelnetType
    case I_tn3270sLuTelnetType:
       dp = (void *) (&data->tn3270sLuTelnetType);
       break;
#endif /* I_tn3270sLuTelnetType */

#ifdef I_tn3270sLuTermModel
    case I_tn3270sLuTermModel:
       dp = (void *) (CloneOctetString(data->tn3270sLuTermModel));
       break;
#endif /* I_tn3270sLuTermModel */

#ifdef I_tn3270sLuState
    case I_tn3270sLuState:
       dp = (void *) (&data->tn3270sLuState);
       break;
#endif /* I_tn3270sLuState */

#ifdef I_tn3270sLuCurInbPacing
    case I_tn3270sLuCurInbPacing:
       dp = (void *) (&data->tn3270sLuCurInbPacing);
       break;
#endif /* I_tn3270sLuCurInbPacing */

#ifdef I_tn3270sLuCurInbQsize
    case I_tn3270sLuCurInbQsize:
       dp = (void *) (&data->tn3270sLuCurInbQsize);
       break;
#endif /* I_tn3270sLuCurInbQsize */

#ifdef I_tn3270sLuCurOutQsize
    case I_tn3270sLuCurOutQsize:
       dp = (void *) (&data->tn3270sLuCurOutQsize);
       break;
#endif /* I_tn3270sLuCurOutQsize */

#ifdef I_tn3270sLuIdleTime
    case I_tn3270sLuIdleTime:
       dp = (void *) (&data->tn3270sLuIdleTime);
       break;
#endif /* I_tn3270sLuIdleTime */

#ifdef I_tn3270sLuType
    case I_tn3270sLuType:
       dp = (void *) (&data->tn3270sLuType);
       break;
#endif /* I_tn3270sLuType */

#ifdef I_tn3270sLuAppnLinkIndex
    case I_tn3270sLuAppnLinkIndex:
       dp = (void *) (CloneOctetString(data->tn3270sLuAppnLinkIndex));
       break;
#endif /* I_tn3270sLuAppnLinkIndex */

#ifdef I_tn3270sLuLfsid
    case I_tn3270sLuLfsid:
       dp = (void *) (&data->tn3270sLuLfsid);
       break;
#endif /* I_tn3270sLuLfsid */

#ifdef I_tn3270sLuLastEvent
    case I_tn3270sLuLastEvent:
       dp = (void *) (&data->tn3270sLuLastEvent);
       break;
#endif /* I_tn3270sLuLastEvent */

#ifdef I_tn3270sLuEvents
    case I_tn3270sLuEvents:
       dp = (void *) (CloneOctetString(data->tn3270sLuEvents));
       break;
#endif /* I_tn3270sLuEvents */

    default:
       return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}


/*
 * Start of system-dependent routines
 */


/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 */





tn3270sGlobalEntry_t *
k_tn3270sGlobalEntry_get(serialNum, contextInfo, nominator, searchType, tn3270sIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 tn3270sIndex;
{
  static tn3270sGlobalEntry_t tn3270sGlobalEntryData;
  static Tn3270sGblEntry_t    globalEntryData;
  static int                  lastSerialNum = MAXINT;
  static long                 lastIfIndex = MAXLONG;
  static OctetString          cpuName;

  hwidbtype* hwidb;
  long savedIndex = tn3270sIndex;
  
    /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
    if ((lastSerialNum == serialNum) && (lastIfIndex == tn3270sIndex))
        return &tn3270sGlobalEntryData;
  
    while ((hwidb = cip_get_virtual_hwidb(searchType, tn3270sIndex)) != NULL) {
        /* a cip hwidb is found */
        if (get_tn3270s(searchType == EXACT, hwidb, SYSMGT_TN3270S_GLOBAL, &globalEntryData))
            break;
        else if (searchType == EXACT)
            return NULL;
        else
            tn3270sIndex = hwidb->snmp_if_index + 1;       /* search next one */
    }
   
    if (hwidb != NULL) {
        /* Remember which info we've retrieved: */
        static char cpu[] = "CIP slot ?";
        lastIfIndex = savedIndex;
        lastSerialNum = serialNum;

        tn3270sGlobalEntryData.tn3270sIndex = hwidb->snmp_if_index;

        cpuName.octet_ptr = cpu;
        cpu[9] = hwidb->slot + '0';        /* convert ? to slot number */
        cpuName.length = 10;
        tn3270sGlobalEntryData.tn3270sCpuCard = &cpuName; /*???*/
        tn3270sGlobalEntryData.tn3270sMaxLus = globalEntryData.tn3270sMaxLus;
        tn3270sGlobalEntryData.tn3270sLusInUse = globalEntryData.tn3270sLusInUse;
        tn3270sGlobalEntryData.tn3270sStartupTime = GetTn3270sUpTime(hwidb);
        tn3270sGlobalEntryData.tn3270sGlobalTcpPort = globalEntryData.tn3270sGblTcpPort;
        tn3270sGlobalEntryData.tn3270sGlobalIdleTimeout = globalEntryData.tn3270sGblIdleTimeout;
        tn3270sGlobalEntryData.tn3270sGlobalKeepAlive = globalEntryData.tn3270sGblKeepAlive;
        tn3270sGlobalEntryData.tn3270sGlobalUnbindAction = globalEntryData.tn3270sGblUnbindAction;
        tn3270sGlobalEntryData.tn3270sGlobalGenericPool = globalEntryData.tn3270sGblGenericPool;
        tn3270sGlobalEntryData.tn3270sTimingMarkSupported = globalEntryData.tn3270sTimingMarkSupported;
        tn3270sGlobalEntryData.tn3270sRunningTime = globalEntryData.tn3270sRunningTime;
        return &tn3270sGlobalEntryData;
    }
    return NULL;
}

tn3270sStatsEntry_t *
k_tn3270sStatsEntry_get(serialNum, contextInfo, nominator, searchType, tn3270sIndex, tn3270sStatsServerAddr, tn3270sStatsServerTcpPort)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 tn3270sIndex;
   SR_UINT32 tn3270sStatsServerAddr;
   SR_INT32 tn3270sStatsServerTcpPort;
{
  static tn3270sStatsEntry_t tn3270sStatsEntryData;
  static Tn3270sStatsEntry_t statsEntryData;
  static int lastSerialNum = MAXINT;
  static SR_UINT32 lastServerAddr = MAXLONG;
  static SR_INT32 lastServerTcpPort = MAXLONG;
  static long lastTtn3270sIndex = MAXLONG;

  hwidbtype* hwidb;

    if ((lastSerialNum == serialNum) &&  (lastServerAddr == tn3270sStatsServerAddr) &&
         (lastServerTcpPort == tn3270sStatsServerTcpPort) && 
         (lastTtn3270sIndex == tn3270sIndex))
        return &tn3270sStatsEntryData;

    statsEntryData.tn3270sStatsServerAddr = tn3270sStatsServerAddr;
    statsEntryData.tn3270sStatsServerTcpPort = tn3270sStatsServerTcpPort;

    while ((hwidb = cip_get_virtual_hwidb(NEXT, tn3270sIndex)) != NULL) {
        /* find the first hwidb */
        if (get_tn3270s(searchType == EXACT, hwidb, SYSMGT_TN3270S_STATS, &statsEntryData)){
            break;
        }
        else  {
            tn3270sIndex = hwidb->snmp_if_index + 1;  /* search next one */
            statsEntryData.tn3270sStatsServerAddr = 0;
            statsEntryData.tn3270sStatsServerTcpPort = 0;
        } 
    }

    if (hwidb != NULL) {
        lastSerialNum = serialNum;
        lastServerAddr = tn3270sStatsServerAddr;
        lastServerTcpPort = tn3270sStatsServerTcpPort;
        lastTtn3270sIndex = hwidb->snmp_if_index;
        tn3270sStatsEntryData.tn3270sStatsServerAddr = statsEntryData.tn3270sStatsServerAddr;
        tn3270sStatsEntryData.tn3270sStatsServerTcpPort = statsEntryData.tn3270sStatsServerTcpPort;
        tn3270sStatsEntryData.tn3270sStatsMaxSess = statsEntryData.tn3270sStatsMaxSess;
        tn3270sStatsEntryData.tn3270sStatsSpareSess = statsEntryData.tn3270sStatsSpareSess;
        tn3270sStatsEntryData.tn3270sStatsConnectsIn = statsEntryData.tn3270sStatsConnectsIn;
        tn3270sStatsEntryData.tn3270sStatsDisconnects = statsEntryData.tn3270sStatsDisconnects;
        tn3270sStatsEntryData.tn3270sStatsTN3270ConnectsFailed
            = statsEntryData.tn3270sStatsTN3270ConnectsFailed;
        tn3270sStatsEntryData.tn3270sStatsInboundChains
            = statsEntryData.tn3270sStatsInboundTransactions;
        tn3270sStatsEntryData.tn3270sStatsOutboundChains
            = statsEntryData.tn3270sStatsOutboundTransactions;
        tn3270sStatsEntryData.tn3270sStatsSampledHostResponses
            = statsEntryData.tn3270sStatsSampledInboundTransactions;
        tn3270sStatsEntryData.tn3270sStatsNetSampledHostResponseTime
            = statsEntryData.tn3270sStatsNetSampledInboundTransactionRespTime;
        tn3270sStatsEntryData.tn3270sStatsSampledClientResponses
            = statsEntryData.tn3270sStatsSampledOutboundTransactions;
        tn3270sStatsEntryData.tn3270sStatsNetSampledClientResponseTime
            = statsEntryData.tn3270sStatsNetSampledOutboundTransactionRespTime;
        tn3270sStatsEntryData.tn3270sIndex = hwidb->snmp_if_index;
        return &tn3270sStatsEntryData;
    }
    return NULL;
}

tn3270sPuEntry_t *
k_tn3270sPuEntry_get(serialNum, contextInfo, nominator, searchType, tn3270sIndex, tn3270sPuIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 tn3270sIndex;
   SR_INT32 tn3270sPuIndex;
{
  static tn3270sPuEntry_t tn3270sPuEntryData;

  hwidbtype* idb;
  static Tn3270sPuEntry_t tn3270sPuEntry;
  static int lastSerialNum = MAXINT;
  static long lastPuIndex = MAXLONG;
  static SR_INT32 lastTn3270sIndex = MAXLONG;

    if (lastTn3270sIndex == tn3270sIndex && lastSerialNum == serialNum &&
        lastPuIndex == tn3270sPuIndex) {
          return &tn3270sPuEntryData;
    }

    idb = cip_get_virtual_hwidb(searchType, tn3270sIndex);
    while (idb) {
       tn3270sPuEntry.tn3270sPuIndex = tn3270sPuIndex;
       if (UpdatePuCardConfig(searchType, idb,  (LuControlCommon*)&tn3270sPuEntry)){
          static OctetString mac;
          static OctetString luSeed;

          lastSerialNum = serialNum;
          lastPuIndex = tn3270sPuIndex;
          lastTn3270sIndex = idb->snmp_if_index;
          mac.octet_ptr = tn3270sPuEntry.tn3270sRemoteMacAddress;
          mac.length = 6;
          luSeed.octet_ptr = tn3270sPuEntry.tn3270sPuLuSeed;
          luSeed.length = strlen(tn3270sPuEntry.tn3270sPuLuSeed);

          tn3270sPuEntryData.tn3270sIndex = idb->snmp_if_index;
          tn3270sPuEntryData.tn3270sPuIndex = tn3270sPuEntry.tn3270sPuIndex;
        
          tn3270sPuEntryData.tn3270sPuIpAddr = tn3270sPuEntry.tn3270sPuIpAddr;
          tn3270sPuEntryData.tn3270sPuTcpPort = tn3270sPuEntry.tn3270sPuTcpPort;
          tn3270sPuEntryData.tn3270sPuIdleTimeout = tn3270sPuEntry.tn3270sPuIdleTimeout;
          tn3270sPuEntryData.tn3270sPuKeepAlive = tn3270sPuEntry.tn3270sPuKeepAlive;
          tn3270sPuEntryData.tn3270sPuUnbindAction = tn3270sPuEntry.tn3270sPuUnbindAction;
          tn3270sPuEntryData.tn3270sPuGenericPool = tn3270sPuEntry.tn3270sPuGenericPool;
          tn3270sPuEntryData.tn3270sPuState = tn3270sPuEntry.tn3270sPuState;
          tn3270sPuEntryData.tn3270sPuType = tn3270sPuEntry.tn3270sPuType;
          tn3270sPuEntryData.tn3270sPuLuSeed = &luSeed;
          tn3270sPuEntryData.tn3270sLocalSapAddress = tn3270sPuEntry.tn3270sLocalSapAddress;
          tn3270sPuEntryData.tn3270sRemoteSapAddress = tn3270sPuEntry.tn3270sRemoteSapAddress;
          tn3270sPuEntryData.tn3270sRemoteMacAddress = &mac;
          return &tn3270sPuEntryData;
       }
       else {
          idb = cip_get_virtual_hwidb(searchType, idb->snmp_if_index + 1);
          /* search next idb */
       }
    }
    return NULL;
}

tn3270sIpEntry_t *
k_tn3270sIpEntry_get(serialNum, contextInfo, nominator, searchType, tn3270sIndex, tn3270sIpClientAddr, tn3270sIpClientTcpPort)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 tn3270sIndex;
   SR_UINT32 tn3270sIpClientAddr;
   SR_INT32 tn3270sIpClientTcpPort;
{
  static tn3270sIpEntry_t tn3270sIpEntryData;
  static Tn3270sIpEntry_t ipEntryData;

  static int lastSerialNum = MAXINT;
  static SR_UINT32  lastIpClientAddr = MAXLONG;
  static SR_INT32 lastIpClientTcpPort = MAXLONG;
   
    hwidbtype* hwidb;

   /*
    * put your code to retrieve the information here
    */

    /* 
    * Check for match of cached instance to avoid RPC request to CIP
    * If match, then set return pointer to cached instance.
    */
    if ((lastSerialNum == serialNum) &&  (lastIpClientAddr == tn3270sIpClientAddr) &&
          (lastIpClientTcpPort == tn3270sIpClientTcpPort))
        return &tn3270sIpEntryData;

    ipEntryData.tn3270sIpClientAddr = tn3270sIpClientAddr;        /* index to be searched */
    ipEntryData.tn3270sIpClientTcpPort = tn3270sIpClientTcpPort;  /* index to be searched */

    while ((hwidb = cip_get_virtual_hwidb(NEXT, tn3270sIndex)) != NULL) {
        /* Note that GET_NEXT is not supported for this MIB entity */
        if (get_tn3270s(searchType == EXACT, hwidb, SYSMGT_TN3270S_IP, &ipEntryData))
            break;
        else {
         /* Note -  we can do the following since ifIndex if not part of the MIB entity's index */
         tn3270sIndex = hwidb->snmp_if_index + 1;  /* search next one */
        } 
    }

    if (hwidb != NULL) {
        lastSerialNum = serialNum;
        lastIpClientAddr = tn3270sIpClientAddr;
        lastIpClientTcpPort = tn3270sIpClientTcpPort;

        tn3270sIpEntryData.tn3270sIpClientAddr = ipEntryData.tn3270sIpClientAddr;
        tn3270sIpEntryData.tn3270sIpClientTcpPort = ipEntryData.tn3270sIpClientTcpPort;
        tn3270sIpEntryData.tn3270sIpPuIndex = ipEntryData.tn3270sIpPuIndex;
        tn3270sIpEntryData.tn3270sIpLuIndex = ipEntryData.tn3270sIpLuIndex;
        tn3270sIpEntryData.tn3270sIndex = hwidb->snmp_if_index; 
        return &tn3270sIpEntryData;
    }
    return NULL;
}

tn3270sLuEntry_t *
k_tn3270sLuEntry_get(serialNum, contextInfo, nominator, searchType, tn3270sIndex, tn3270sLuPuIndex, tn3270sLuIndex)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
   int searchType;
   SR_INT32 tn3270sIndex;
   SR_INT32 tn3270sLuPuIndex;
   SR_INT32 tn3270sLuIndex;
{
  hwidbtype* hwidb;
  static tn3270sLuEntry_t tn3270sLuEntryData;
  static Tn3270sLuEntry_t luEntryData;
  static long lastSerialNum = MAXINT;
  static long lastLuPuIndex = MAXLONG;
  static long lastLuIndex = MAXLONG;

    if (lastSerialNum == serialNum && lastLuPuIndex == tn3270sLuPuIndex &&
          lastLuIndex ==  tn3270sLuIndex)
        return &tn3270sLuEntryData;

    luEntryData.tn3270sLuPuIndex = tn3270sLuPuIndex;
    luEntryData.tn3270sLuIndex = tn3270sLuIndex;

    if ((hwidb = GetLuControlBlock(searchType, SYSMGT_TN3270S_LU, 
                                   (LuControlCommon*)&luEntryData)) != NULL){
      static OctetString term;
      static OctetString event;
      static OctetString linkIndex;

        uint   len;

        lastSerialNum = serialNum;
        lastLuPuIndex = tn3270sLuPuIndex;
        lastLuIndex = tn3270sLuIndex;

        term.octet_ptr = luEntryData.tn3270sLuTermModel;
        term.length = strlen(luEntryData.tn3270sLuTermModel);

        event.octet_ptr = luEntryData.tn3270sLuEvents;
        event.length = sizeof(luEntryData.tn3270sLuEvents);


        tn3270sLuEntryData.tn3270sLuPuIndex = luEntryData.tn3270sLuPuIndex;
        tn3270sLuEntryData.tn3270sLuIndex = luEntryData.tn3270sLuIndex;
        tn3270sLuEntryData.tn3270sLuClientAddr = luEntryData.tn3270sLuClientAddr;
        tn3270sLuEntryData.tn3270sLuClientTcpPort = luEntryData.tn3270sLuClientTcpPort;
        tn3270sLuEntryData.tn3270sLuTelnetType = luEntryData.tn3270sLuTelnetType;
        tn3270sLuEntryData.tn3270sLuTermModel = &term;
        tn3270sLuEntryData.tn3270sLuState = luEntryData.tn3270sLuState;
        tn3270sLuEntryData.tn3270sLuCurInbPacing = luEntryData.tn3270sLuCurInbPacing;
        tn3270sLuEntryData.tn3270sLuCurInbQsize = luEntryData.tn3270sLuCurInbQsize;
        tn3270sLuEntryData.tn3270sLuCurOutQsize = luEntryData.tn3270sLuCurOutQsize;
        tn3270sLuEntryData.tn3270sLuIdleTime = luEntryData.tn3270sLuIdleTime;
        tn3270sLuEntryData.tn3270sLuType = luEntryData.tn3270sLuType;

        linkIndex.octet_ptr = luEntryData.tn3270sLuAppnLinkName;

        /* find maybe non-null terminated strlen(luEntryData.tn3270sLuAppnLinkName) */

        for (len = 0; len < sizeof(luEntryData.tn3270sLuAppnLinkName) &&
                      luEntryData.tn3270sLuAppnLinkName[len] != 0; len++){
           ;
        }
        linkIndex.length = len;
        tn3270sLuEntryData.tn3270sLuAppnLinkIndex = &linkIndex;
        tn3270sLuEntryData.tn3270sLuLfsid = luEntryData.tn3270sLuLfsid;
        tn3270sLuEntryData.tn3270sLuLastEvent = 0;
        tn3270sLuEntryData.tn3270sLuEvents =  &event;
        tn3270sLuEntryData.tn3270sIndex = hwidb->snmp_if_index;
        return &tn3270sLuEntryData;
    }
    return NULL;
}



static void
init_tn3270servermib (subsystype *subsys)
{
    load_mib(tn3270servermib_OidList, tn3270servermib_OidListNum);
    load_oid(tn3270servermib_oid_table);
    init_cipappnmib();     /* enable appn mib */
    init_cipdlurmib();     /* enable dlur mib */
}

/*
 * tn3270servermib subsystem header
 */

/* WARNING: update "req" with the subsystem being monitored */

#define MAJVERSION_tn3270servermib 1
#define MINVERSION_tn3270servermib 0
#define EDITVERSION_tn3270servermib 0

SUBSYS_HEADER(tn3270servermib,
              MAJVERSION_tn3270servermib,
              MINVERSION_tn3270servermib,
              EDITVERSION_tn3270servermib,
              init_tn3270servermib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: tn3270s cip_sysmgt_rpc",
              NULL);
/* WARNING: replace last two NULL parameters with "req:" and "seq:"
         directives if required */








      
