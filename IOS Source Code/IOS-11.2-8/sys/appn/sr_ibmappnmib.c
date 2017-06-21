/* $Id: sr_ibmappnmib.c,v 3.5.8.5 1996/08/05 07:25:11 bchan Exp $
 * $Source: /release/112/cvs/Xsys/appn/sr_ibmappnmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1995, Mark Regan (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_ibmappnmib.c,v $
 * Revision 3.5.8.5  1996/08/05  07:25:11  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.5.8.4  1996/07/17  06:52:31  bchan
 * CSCdi63198:  Fix appn/dlur mib for tn3270 server
 * Branch: California_branch
 *
 * Revision 3.5.8.3  1996/07/03  20:40:05  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5.8.2  1996/06/22  23:47:36  snyder
 * CSCdi60918:  empty seq: and req: fields should be NULL
 * Branch: California_branch
 *              and eliminate some unneccessary req: and seq:
 *              entries.  Hey 200 bytes is 200 bytes
 *
 * Revision 3.5.8.1  1996/05/17  10:39:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.8.2  1996/03/27  01:30:33  bchan
 * CSCdi50608:  makefile for mib incorrect
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.4.8.1  1996/03/17  17:34:24  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.5  1996/02/29  19:47:44  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/24  22:07:11  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.3  1995/12/14  08:22:03  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  08:37:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:52:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/12  17:19:38  mregan
 * CSCdi29195:  APPN MIB values incorrect
 * update APPN MIB with latest SNMP changes.
 *
 * Revision 2.1  1995/06/07  20:03:50  hampton
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
#include "sr_ibmappnmib.h"
#include "sr_ibmappnmib-mib.h"
#include "../entity/entity_api.h"











/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnGeneralInfoAndCaps family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
ibmappnGeneralInfoAndCaps_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    ibmappnGeneralInfoAndCaps_t *data = NULL;


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
     if ( (arg == -1) || (data = k_ibmappnGeneralInfoAndCaps_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeCpName
      case I_ibmappnNodeCpName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeCpName));
	break;
#endif				       /* I_ibmappnNodeCpName */

#ifdef I_ibmappnNodeNetid
      case I_ibmappnNodeNetid:
	dp = (void *) (CloneOctetString(data->ibmappnNodeNetid));
	break;
#endif				       /* I_ibmappnNodeNetid */

#ifdef I_ibmappnNodeBlockNum
      case I_ibmappnNodeBlockNum:
	dp = (void *) (CloneOctetString(data->ibmappnNodeBlockNum));
	break;
#endif				       /* I_ibmappnNodeBlockNum */

#ifdef I_ibmappnNodeIdNum
      case I_ibmappnNodeIdNum:
	dp = (void *) (CloneOctetString(data->ibmappnNodeIdNum));
	break;
#endif				       /* I_ibmappnNodeIdNum */

#ifdef I_ibmappnNodeType
      case I_ibmappnNodeType:
	dp = (void *) (&data->ibmappnNodeType);
	break;
#endif				       /* I_ibmappnNodeType */

#ifdef I_ibmappnNodeUpTime
      case I_ibmappnNodeUpTime:
	dp = (void *) (&data->ibmappnNodeUpTime);
	break;
#endif				       /* I_ibmappnNodeUpTime */

#ifdef I_ibmappnNodeNegotLs
      case I_ibmappnNodeNegotLs:
	dp = (void *) (&data->ibmappnNodeNegotLs);
	break;
#endif				       /* I_ibmappnNodeNegotLs */

#ifdef I_ibmappnNodeSegReasm
      case I_ibmappnNodeSegReasm:
	dp = (void *) (&data->ibmappnNodeSegReasm);
	break;
#endif				       /* I_ibmappnNodeSegReasm */

#ifdef I_ibmappnNodeBindReasm
      case I_ibmappnNodeBindReasm:
	dp = (void *) (&data->ibmappnNodeBindReasm);
	break;
#endif				       /* I_ibmappnNodeBindReasm */

#ifdef I_ibmappnNodeParallelTg
      case I_ibmappnNodeParallelTg:
	dp = (void *) (&data->ibmappnNodeParallelTg);
	break;
#endif				       /* I_ibmappnNodeParallelTg */

#ifdef I_ibmappnNodeService
      case I_ibmappnNodeService:
	dp = (void *) (&data->ibmappnNodeService);
	break;
#endif				       /* I_ibmappnNodeService */

#ifdef I_ibmappnNodeAdaptiveBindPacing
      case I_ibmappnNodeAdaptiveBindPacing:
	dp = (void *) (&data->ibmappnNodeAdaptiveBindPacing);
	break;
#endif				       /* I_ibmappnNodeAdaptiveBindPacing */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnUniqueInfoAndCaps family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnUniqueInfoAndCaps_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg;
    void            *dp;
    ibmappnNnUniqueInfoAndCaps_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnUniqueInfoAndCaps_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeNnRcvRegChar
      case I_ibmappnNodeNnRcvRegChar:
	dp = (void *) (&data->ibmappnNodeNnRcvRegChar);
	break;
#endif				       /* I_ibmappnNodeNnRcvRegChar */

#ifdef I_ibmappnNodeNnGateway
      case I_ibmappnNodeNnGateway:
	dp = (void *) (&data->ibmappnNodeNnGateway);
	break;
#endif				       /* I_ibmappnNodeNnGateway */

#ifdef I_ibmappnNodeNnCentralDirectory
      case I_ibmappnNodeNnCentralDirectory:
	dp = (void *) (&data->ibmappnNodeNnCentralDirectory);
	break;
#endif				       /* I_ibmappnNodeNnCentralDirectory */

#ifdef I_ibmappnNodeNnTreeCache
      case I_ibmappnNodeNnTreeCache:
	dp = (void *) (&data->ibmappnNodeNnTreeCache);
	break;
#endif				       /* I_ibmappnNodeNnTreeCache */

#ifdef I_ibmappnNodeNnTreeUpdate
      case I_ibmappnNodeNnTreeUpdate:
	dp = (void *) (&data->ibmappnNodeNnTreeUpdate);
	break;
#endif				       /* I_ibmappnNodeNnTreeUpdate */

#ifdef I_ibmappnNodeNnRouteAddResist
      case I_ibmappnNodeNnRouteAddResist:
	dp = (void *) (&data->ibmappnNodeNnRouteAddResist);
	break;
#endif				       /* I_ibmappnNodeNnRouteAddResist */

#ifdef I_ibmappnNodeNnIsr
      case I_ibmappnNodeNnIsr:
	dp = (void *) (&data->ibmappnNodeNnIsr);
	break;
#endif				       /* I_ibmappnNodeNnIsr */

#ifdef I_ibmappnNodeNnFrsn
      case I_ibmappnNodeNnFrsn:
	dp = (void *) (&data->ibmappnNodeNnFrsn);
	break;
#endif				       /* I_ibmappnNodeNnFrsn */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnEnUniqueCaps family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnEnUniqueCaps_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg;
    void            *dp;
    ibmappnEnUniqueCaps_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnEnUniqueCaps_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeEnSegGen
      case I_ibmappnNodeEnSegGen:
	dp = (void *) (&data->ibmappnNodeEnSegGen);
	break;
#endif				       /* I_ibmappnNodeEnSegGen */

#ifdef I_ibmappnNodeEnModeCosMap
      case I_ibmappnNodeEnModeCosMap:
	dp = (void *) (&data->ibmappnNodeEnModeCosMap);
	break;
#endif				       /* I_ibmappnNodeEnModeCosMap */

#ifdef I_ibmappnNodeEnLocateCdinit
      case I_ibmappnNodeEnLocateCdinit:
	dp = (void *) (&data->ibmappnNodeEnLocateCdinit);
	break;
#endif				       /* I_ibmappnNodeEnLocateCdinit */

#ifdef I_ibmappnNodeEnSendRegNames
      case I_ibmappnNodeEnSendRegNames:
	dp = (void *) (&data->ibmappnNodeEnSendRegNames);
	break;
#endif				       /* I_ibmappnNodeEnSendRegNames */

#ifdef I_ibmappnNodeEnSendRegChar
      case I_ibmappnNodeEnSendRegChar:
	dp = (void *) (&data->ibmappnNodeEnSendRegChar);
	break;
#endif				       /* I_ibmappnNodeEnSendRegChar */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodePortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodePortEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void            *dp;
    ibmappnNodePortEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodePortName = NULL;
    int             ibmappnNodePortName_offset;
    int             index;
    int             final_index;

    ibmappnNodePortName_offset = object->oid.length;
    final_index = ibmappnNodePortName_offset + GetVariableIndexLength(incoming, ibmappnNodePortName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodePortName_offset, &ibmappnNodePortName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodePortEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodePortName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodePortName->length;
	for(i = 0; i < data->ibmappnNodePortName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodePortName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodePortName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodePortName
      case I_ibmappnNodePortName:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortName));
	break;
#endif				       /* I_ibmappnNodePortName */

#ifdef I_ibmappnNodePortState
      case I_ibmappnNodePortState:
	dp = (void *) (&data->ibmappnNodePortState);
	break;
#endif				       /* I_ibmappnNodePortState */

#ifdef I_ibmappnNodePortDlcType
      case I_ibmappnNodePortDlcType:
	dp = (void *) (&data->ibmappnNodePortDlcType);
	break;
#endif				       /* I_ibmappnNodePortDlcType */

#ifdef I_ibmappnNodePortPortType
      case I_ibmappnNodePortPortType:
	dp = (void *) (&data->ibmappnNodePortPortType);
	break;
#endif				       /* I_ibmappnNodePortPortType */

#ifdef I_ibmappnNodePortSIMRIM
      case I_ibmappnNodePortSIMRIM:
	dp = (void *) (&data->ibmappnNodePortSIMRIM);
	break;
#endif				       /* I_ibmappnNodePortSIMRIM */

#ifdef I_ibmappnNodePortLsRole
      case I_ibmappnNodePortLsRole:
	dp = (void *) (&data->ibmappnNodePortLsRole);
	break;
#endif				       /* I_ibmappnNodePortLsRole */

#ifdef I_ibmappnNodePortMaxRcvBtuSize
      case I_ibmappnNodePortMaxRcvBtuSize:
	dp = (void *) (&data->ibmappnNodePortMaxRcvBtuSize);
	break;
#endif				       /* I_ibmappnNodePortMaxRcvBtuSize */

#ifdef I_ibmappnNodePortMaxIframeWindow
      case I_ibmappnNodePortMaxIframeWindow:
	dp = (void *) (&data->ibmappnNodePortMaxIframeWindow);
	break;
#endif				       /* I_ibmappnNodePortMaxIframeWindow */

#ifdef I_ibmappnNodePortDefLsGoodXids
      case I_ibmappnNodePortDefLsGoodXids:
	dp = (void *) (&data->ibmappnNodePortDefLsGoodXids);
	break;
#endif				       /* I_ibmappnNodePortDefLsGoodXids */

#ifdef I_ibmappnNodePortDefLsBadXids
      case I_ibmappnNodePortDefLsBadXids:
	dp = (void *) (&data->ibmappnNodePortDefLsBadXids);
	break;
#endif				       /* I_ibmappnNodePortDefLsBadXids */

#ifdef I_ibmappnNodePortDynLsGoodXids
      case I_ibmappnNodePortDynLsGoodXids:
	dp = (void *) (&data->ibmappnNodePortDynLsGoodXids);
	break;
#endif				       /* I_ibmappnNodePortDynLsGoodXids */

#ifdef I_ibmappnNodePortDynLsBadXids
      case I_ibmappnNodePortDynLsBadXids:
	dp = (void *) (&data->ibmappnNodePortDynLsBadXids);
	break;
#endif				       /* I_ibmappnNodePortDynLsBadXids */

#ifdef I_ibmappnNodePortSpecific
      case I_ibmappnNodePortSpecific:
	dp = (void *) (CloneOID(data->ibmappnNodePortSpecific));
	break;
#endif				       /* I_ibmappnNodePortSpecific */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodePortIpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodePortIpEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodePortIpEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodePortIpName = NULL;
    int             ibmappnNodePortIpName_offset;
    int             index;
    int             final_index;

    ibmappnNodePortIpName_offset = object->oid.length;
    final_index = ibmappnNodePortIpName_offset + GetVariableIndexLength(incoming, ibmappnNodePortIpName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodePortIpName_offset, &ibmappnNodePortIpName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodePortIpEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodePortIpName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodePortIpName->length;
	for(i = 0; i < data->ibmappnNodePortIpName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodePortIpName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodePortIpName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodePortIpName
      case I_ibmappnNodePortIpName:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortIpName));
	break;
#endif				       /* I_ibmappnNodePortIpName */

#ifdef I_ibmappnNodePortIpPortNum
      case I_ibmappnNodePortIpPortNum:
	dp = (void *) (&data->ibmappnNodePortIpPortNum);
	break;
#endif				       /* I_ibmappnNodePortIpPortNum */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodePortDlsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodePortDlsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodePortDlsEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodePortDlsName = NULL;
    int             ibmappnNodePortDlsName_offset;
    int             index;
    int             final_index;

    ibmappnNodePortDlsName_offset = object->oid.length;
    final_index = ibmappnNodePortDlsName_offset + GetVariableIndexLength(incoming, ibmappnNodePortDlsName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodePortDlsName_offset, &ibmappnNodePortDlsName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodePortDlsEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodePortDlsName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodePortDlsName->length;
	for(i = 0; i < data->ibmappnNodePortDlsName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodePortDlsName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodePortDlsName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodePortDlsName
      case I_ibmappnNodePortDlsName:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortDlsName));
	break;
#endif				       /* I_ibmappnNodePortDlsName */

#ifdef I_ibmappnNodePortDlsMac
      case I_ibmappnNodePortDlsMac:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortDlsMac));
	break;
#endif				       /* I_ibmappnNodePortDlsMac */

#ifdef I_ibmappnNodePortDlsSap
      case I_ibmappnNodePortDlsSap:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortDlsSap));
	break;
#endif				       /* I_ibmappnNodePortDlsSap */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodePortTrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodePortTrEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodePortTrEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodePortTrName = NULL;
    int             ibmappnNodePortTrName_offset;
    int             index;
    int             final_index;

    ibmappnNodePortTrName_offset = object->oid.length;
    final_index = ibmappnNodePortTrName_offset + GetVariableIndexLength(incoming, ibmappnNodePortTrName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodePortTrName_offset, &ibmappnNodePortTrName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodePortTrEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodePortTrName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodePortTrName->length;
	for(i = 0; i < data->ibmappnNodePortTrName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodePortTrName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodePortTrName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodePortTrName
      case I_ibmappnNodePortTrName:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortTrName));
	break;
#endif				       /* I_ibmappnNodePortTrName */

#ifdef I_ibmappnNodePortTrMac
      case I_ibmappnNodePortTrMac:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortTrMac));
	break;
#endif				       /* I_ibmappnNodePortTrMac */

#ifdef I_ibmappnNodePortTrSap
      case I_ibmappnNodePortTrSap:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortTrSap));
	break;
#endif				       /* I_ibmappnNodePortTrSap */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodePortDlcTraceEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodePortDlcTraceEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodePortDlcTraceEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodePortDlcTracPortName = NULL;
    int             ibmappnNodePortDlcTracPortName_offset;
    long            ibmappnNodePortDlcTracIndex;
    int             ibmappnNodePortDlcTracIndex_offset;
    int             index;
    int             final_index;

    ibmappnNodePortDlcTracPortName_offset = object->oid.length;
    ibmappnNodePortDlcTracIndex_offset = ibmappnNodePortDlcTracPortName_offset + GetVariableIndexLength(incoming, ibmappnNodePortDlcTracPortName_offset);
    final_index = ibmappnNodePortDlcTracIndex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToInt(incoming, ibmappnNodePortDlcTracIndex_offset, &ibmappnNodePortDlcTracIndex, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToVariableOctetString(incoming, ibmappnNodePortDlcTracPortName_offset, &ibmappnNodePortDlcTracPortName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodePortDlcTraceEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodePortDlcTracPortName, ibmappnNodePortDlcTracIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodePortDlcTracPortName->length;
	for(i = 0; i < data->ibmappnNodePortDlcTracPortName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodePortDlcTracPortName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodePortDlcTracIndex;
	inst.length = index;
    }

	FreeOctetString(ibmappnNodePortDlcTracPortName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodePortDlcTracPortName
      case I_ibmappnNodePortDlcTracPortName:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortDlcTracPortName));
	break;
#endif				       /* I_ibmappnNodePortDlcTracPortName */

#ifdef I_ibmappnNodePortDlcTracIndex
      case I_ibmappnNodePortDlcTracIndex:
	dp = (void *) (&data->ibmappnNodePortDlcTracIndex);
	break;
#endif				       /* I_ibmappnNodePortDlcTracIndex */

#ifdef I_ibmappnNodePortDlcTracDlcType
      case I_ibmappnNodePortDlcTracDlcType:
	dp = (void *) (&data->ibmappnNodePortDlcTracDlcType);
	break;
#endif				       /* I_ibmappnNodePortDlcTracDlcType */

#ifdef I_ibmappnNodePortDlcTracLocalAddr
      case I_ibmappnNodePortDlcTracLocalAddr:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortDlcTracLocalAddr));
	break;
#endif				       /* I_ibmappnNodePortDlcTracLocalAddr */

#ifdef I_ibmappnNodePortDlcTracRemoteAddr
      case I_ibmappnNodePortDlcTracRemoteAddr:
	dp = (void *) (CloneOctetString(data->ibmappnNodePortDlcTracRemoteAddr));
	break;
#endif				       /* I_ibmappnNodePortDlcTracRemoteAddr */

#ifdef I_ibmappnNodePortDlcTracMsgType
      case I_ibmappnNodePortDlcTracMsgType:
	dp = (void *) (&data->ibmappnNodePortDlcTracMsgType);
	break;
#endif				       /* I_ibmappnNodePortDlcTracMsgType */

#ifdef I_ibmappnNodePortDlcTracCmdType
      case I_ibmappnNodePortDlcTracCmdType:
	dp = (void *) (&data->ibmappnNodePortDlcTracCmdType);
	break;
#endif				       /* I_ibmappnNodePortDlcTracCmdType */

#ifdef I_ibmappnNodePortDlcTracUseWan
      case I_ibmappnNodePortDlcTracUseWan:
	dp = (void *) (&data->ibmappnNodePortDlcTracUseWan);
	break;
#endif				       /* I_ibmappnNodePortDlcTracUseWan */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodeLsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodeLsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodeLsEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodeLsName = NULL;
    int             ibmappnNodeLsName_offset;
    int             index;
    int             final_index;

    ibmappnNodeLsName_offset = object->oid.length;
    final_index = ibmappnNodeLsName_offset + GetVariableIndexLength(incoming, ibmappnNodeLsName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodeLsName_offset, &ibmappnNodeLsName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodeLsEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodeLsName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodeLsName->length;
	for(i = 0; i < data->ibmappnNodeLsName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodeLsName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodeLsName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeLsName
      case I_ibmappnNodeLsName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsName));
	break;
#endif				       /* I_ibmappnNodeLsName */

#ifdef I_ibmappnNodeLsPortName
      case I_ibmappnNodeLsPortName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsPortName));
	break;
#endif				       /* I_ibmappnNodeLsPortName */

#ifdef I_ibmappnNodeLsDlcType
      case I_ibmappnNodeLsDlcType:
	dp = (void *) (&data->ibmappnNodeLsDlcType);
	break;
#endif				       /* I_ibmappnNodeLsDlcType */

#ifdef I_ibmappnNodeLsDynamic
      case I_ibmappnNodeLsDynamic:
	dp = (void *) (&data->ibmappnNodeLsDynamic);
	break;
#endif				       /* I_ibmappnNodeLsDynamic */

#ifdef I_ibmappnNodeLsState
      case I_ibmappnNodeLsState:
	dp = (void *) (&data->ibmappnNodeLsState);
	break;
#endif				       /* I_ibmappnNodeLsState */

#ifdef I_ibmappnNodeLsCpName
      case I_ibmappnNodeLsCpName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsCpName));
	break;
#endif				       /* I_ibmappnNodeLsCpName */

#ifdef I_ibmappnNodeLsTgNum
      case I_ibmappnNodeLsTgNum:
	dp = (void *) (&data->ibmappnNodeLsTgNum);
	break;
#endif				       /* I_ibmappnNodeLsTgNum */

#ifdef I_ibmappnNodeLsLimResource
      case I_ibmappnNodeLsLimResource:
	dp = (void *) (&data->ibmappnNodeLsLimResource);
	break;
#endif				       /* I_ibmappnNodeLsLimResource */

#ifdef I_ibmappnNodeLsMigration
      case I_ibmappnNodeLsMigration:
	dp = (void *) (&data->ibmappnNodeLsMigration);
	break;
#endif				       /* I_ibmappnNodeLsMigration */

#ifdef I_ibmappnNodeLsBlockNum
      case I_ibmappnNodeLsBlockNum:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsBlockNum));
	break;
#endif				       /* I_ibmappnNodeLsBlockNum */

#ifdef I_ibmappnNodeLsIdNum
      case I_ibmappnNodeLsIdNum:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsIdNum));
	break;
#endif				       /* I_ibmappnNodeLsIdNum */

#ifdef I_ibmappnNodeLsCpCpSession
      case I_ibmappnNodeLsCpCpSession:
	dp = (void *) (&data->ibmappnNodeLsCpCpSession);
	break;
#endif				       /* I_ibmappnNodeLsCpCpSession */

#ifdef I_ibmappnNodeLsTargetPacingCount
      case I_ibmappnNodeLsTargetPacingCount:
	dp = (void *) (&data->ibmappnNodeLsTargetPacingCount);
	break;
#endif				       /* I_ibmappnNodeLsTargetPacingCount */

#ifdef I_ibmappnNodeLsMaxSendBtuSize
      case I_ibmappnNodeLsMaxSendBtuSize:
	dp = (void *) (&data->ibmappnNodeLsMaxSendBtuSize);
	break;
#endif				       /* I_ibmappnNodeLsMaxSendBtuSize */

#ifdef I_ibmappnNodeLsEffCap
      case I_ibmappnNodeLsEffCap:
	dp = (void *) (&data->ibmappnNodeLsEffCap);
	break;
#endif				       /* I_ibmappnNodeLsEffCap */

#ifdef I_ibmappnNodeLsConnCost
      case I_ibmappnNodeLsConnCost:
	dp = (void *) (&data->ibmappnNodeLsConnCost);
	break;
#endif				       /* I_ibmappnNodeLsConnCost */

#ifdef I_ibmappnNodeLsByteCost
      case I_ibmappnNodeLsByteCost:
	dp = (void *) (&data->ibmappnNodeLsByteCost);
	break;
#endif				       /* I_ibmappnNodeLsByteCost */

#ifdef I_ibmappnNodeLsSecurity
      case I_ibmappnNodeLsSecurity:
	dp = (void *) (&data->ibmappnNodeLsSecurity);
	break;
#endif				       /* I_ibmappnNodeLsSecurity */

#ifdef I_ibmappnNodeLsDelay
      case I_ibmappnNodeLsDelay:
	dp = (void *) (&data->ibmappnNodeLsDelay);
	break;
#endif				       /* I_ibmappnNodeLsDelay */

#ifdef I_ibmappnNodeLsUsr1
      case I_ibmappnNodeLsUsr1:
	dp = (void *) (&data->ibmappnNodeLsUsr1);
	break;
#endif				       /* I_ibmappnNodeLsUsr1 */

#ifdef I_ibmappnNodeLsUsr2
      case I_ibmappnNodeLsUsr2:
	dp = (void *) (&data->ibmappnNodeLsUsr2);
	break;
#endif				       /* I_ibmappnNodeLsUsr2 */

#ifdef I_ibmappnNodeLsUsr3
      case I_ibmappnNodeLsUsr3:
	dp = (void *) (&data->ibmappnNodeLsUsr3);
	break;
#endif				       /* I_ibmappnNodeLsUsr3 */

#ifdef I_ibmappnNodeLsInXidBytes
      case I_ibmappnNodeLsInXidBytes:
	dp = (void *) (&data->ibmappnNodeLsInXidBytes);
	break;
#endif				       /* I_ibmappnNodeLsInXidBytes */

#ifdef I_ibmappnNodeLsInMsgBytes
      case I_ibmappnNodeLsInMsgBytes:
	dp = (void *) (&data->ibmappnNodeLsInMsgBytes);
	break;
#endif				       /* I_ibmappnNodeLsInMsgBytes */

#ifdef I_ibmappnNodeLsInXidFrames
      case I_ibmappnNodeLsInXidFrames:
	dp = (void *) (&data->ibmappnNodeLsInXidFrames);
	break;
#endif				       /* I_ibmappnNodeLsInXidFrames */

#ifdef I_ibmappnNodeLsInMsgFrames
      case I_ibmappnNodeLsInMsgFrames:
	dp = (void *) (&data->ibmappnNodeLsInMsgFrames);
	break;
#endif				       /* I_ibmappnNodeLsInMsgFrames */

#ifdef I_ibmappnNodeLsOutXidBytes
      case I_ibmappnNodeLsOutXidBytes:
	dp = (void *) (&data->ibmappnNodeLsOutXidBytes);
	break;
#endif				       /* I_ibmappnNodeLsOutXidBytes */

#ifdef I_ibmappnNodeLsOutMsgBytes
      case I_ibmappnNodeLsOutMsgBytes:
	dp = (void *) (&data->ibmappnNodeLsOutMsgBytes);
	break;
#endif				       /* I_ibmappnNodeLsOutMsgBytes */

#ifdef I_ibmappnNodeLsOutXidFrames
      case I_ibmappnNodeLsOutXidFrames:
	dp = (void *) (&data->ibmappnNodeLsOutXidFrames);
	break;
#endif				       /* I_ibmappnNodeLsOutXidFrames */

#ifdef I_ibmappnNodeLsOutMsgFrames
      case I_ibmappnNodeLsOutMsgFrames:
	dp = (void *) (&data->ibmappnNodeLsOutMsgFrames);
	break;
#endif				       /* I_ibmappnNodeLsOutMsgFrames */

#ifdef I_ibmappnNodeLsEchoRsps
      case I_ibmappnNodeLsEchoRsps:
	dp = (void *) (&data->ibmappnNodeLsEchoRsps);
	break;
#endif				       /* I_ibmappnNodeLsEchoRsps */

#ifdef I_ibmappnNodeLsCurrentDelay
      case I_ibmappnNodeLsCurrentDelay:
	dp = (void *) (&data->ibmappnNodeLsCurrentDelay);
	break;
#endif				       /* I_ibmappnNodeLsCurrentDelay */

#ifdef I_ibmappnNodeLsMaxDelay
      case I_ibmappnNodeLsMaxDelay:
	dp = (void *) (&data->ibmappnNodeLsMaxDelay);
	break;
#endif				       /* I_ibmappnNodeLsMaxDelay */

#ifdef I_ibmappnNodeLsMinDelay
      case I_ibmappnNodeLsMinDelay:
	dp = (void *) (&data->ibmappnNodeLsMinDelay);
	break;
#endif				       /* I_ibmappnNodeLsMinDelay */

#ifdef I_ibmappnNodeLsMaxDelayTime
      case I_ibmappnNodeLsMaxDelayTime:
	dp = (void *) (&data->ibmappnNodeLsMaxDelayTime);
	break;
#endif				       /* I_ibmappnNodeLsMaxDelayTime */

#ifdef I_ibmappnNodeLsGoodXids
      case I_ibmappnNodeLsGoodXids:
	dp = (void *) (&data->ibmappnNodeLsGoodXids);
	break;
#endif				       /* I_ibmappnNodeLsGoodXids */

#ifdef I_ibmappnNodeLsBadXids
      case I_ibmappnNodeLsBadXids:
	dp = (void *) (&data->ibmappnNodeLsBadXids);
	break;
#endif				       /* I_ibmappnNodeLsBadXids */

#ifdef I_ibmappnNodeLsSpecific
      case I_ibmappnNodeLsSpecific:
	dp = (void *) (CloneOID(data->ibmappnNodeLsSpecific));
	break;
#endif				       /* I_ibmappnNodeLsSpecific */

#ifdef I_ibmappnNodeLsSubState
      case I_ibmappnNodeLsSubState:
	dp = (void *) (&data->ibmappnNodeLsSubState);
	break;
#endif				       /* I_ibmappnNodeLsSubState */

#ifdef I_ibmappnNodeLsStartTime
      case I_ibmappnNodeLsStartTime:
	dp = (void *) (&data->ibmappnNodeLsStartTime);
	break;
#endif				       /* I_ibmappnNodeLsStartTime */

#ifdef I_ibmappnNodeLsActiveTime
      case I_ibmappnNodeLsActiveTime:
	dp = (void *) (&data->ibmappnNodeLsActiveTime);
	break;
#endif				       /* I_ibmappnNodeLsActiveTime */

#ifdef I_ibmappnNodeLsCurrentStateTime
      case I_ibmappnNodeLsCurrentStateTime:
	dp = (void *) (&data->ibmappnNodeLsCurrentStateTime);
	break;
#endif				       /* I_ibmappnNodeLsCurrentStateTime */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodeLsIpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodeLsIpEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodeLsIpEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodeLsIpName = NULL;
    int             ibmappnNodeLsIpName_offset;
    int             index;
    int             final_index;

    ibmappnNodeLsIpName_offset = object->oid.length;
    final_index = ibmappnNodeLsIpName_offset + GetVariableIndexLength(incoming, ibmappnNodeLsIpName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodeLsIpName_offset, &ibmappnNodeLsIpName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodeLsIpEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodeLsIpName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodeLsIpName->length;
	for(i = 0; i < data->ibmappnNodeLsIpName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodeLsIpName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodeLsIpName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeLsIpName
      case I_ibmappnNodeLsIpName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsIpName));
	break;
#endif				       /* I_ibmappnNodeLsIpName */

#ifdef I_ibmappnNodeLsIpState
      case I_ibmappnNodeLsIpState:
	dp = (void *) (&data->ibmappnNodeLsIpState);
	break;
#endif				       /* I_ibmappnNodeLsIpState */

#ifdef I_ibmappnNodeLsLocalIpAddr
      case I_ibmappnNodeLsLocalIpAddr:
	dp = (void *) (IPToOctetString(data->ibmappnNodeLsLocalIpAddr));
	break;
#endif				       /* I_ibmappnNodeLsLocalIpAddr */

#ifdef I_ibmappnNodeLsLocalIpPortNum
      case I_ibmappnNodeLsLocalIpPortNum:
	dp = (void *) (&data->ibmappnNodeLsLocalIpPortNum);
	break;
#endif				       /* I_ibmappnNodeLsLocalIpPortNum */

#ifdef I_ibmappnNodeLsRemoteIpAddr
      case I_ibmappnNodeLsRemoteIpAddr:
	dp = (void *) (IPToOctetString(data->ibmappnNodeLsRemoteIpAddr));
	break;
#endif				       /* I_ibmappnNodeLsRemoteIpAddr */

#ifdef I_ibmappnNodeLsRemoteIpPortNum
      case I_ibmappnNodeLsRemoteIpPortNum:
	dp = (void *) (&data->ibmappnNodeLsRemoteIpPortNum);
	break;
#endif				       /* I_ibmappnNodeLsRemoteIpPortNum */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodeLsDlsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodeLsDlsEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodeLsDlsEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodeLsDlsName = NULL;
    int             ibmappnNodeLsDlsName_offset;
    int             index;
    int             final_index;

    ibmappnNodeLsDlsName_offset = object->oid.length;
    final_index = ibmappnNodeLsDlsName_offset + GetVariableIndexLength(incoming, ibmappnNodeLsDlsName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodeLsDlsName_offset, &ibmappnNodeLsDlsName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodeLsDlsEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodeLsDlsName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodeLsDlsName->length;
	for(i = 0; i < data->ibmappnNodeLsDlsName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodeLsDlsName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodeLsDlsName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeLsDlsName
      case I_ibmappnNodeLsDlsName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsDlsName));
	break;
#endif				       /* I_ibmappnNodeLsDlsName */

#ifdef I_ibmappnNodeLsDlsState
      case I_ibmappnNodeLsDlsState:
	dp = (void *) (&data->ibmappnNodeLsDlsState);
	break;
#endif				       /* I_ibmappnNodeLsDlsState */

#ifdef I_ibmappnNodeLsLocalDlsMac
      case I_ibmappnNodeLsLocalDlsMac:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsLocalDlsMac));
	break;
#endif				       /* I_ibmappnNodeLsLocalDlsMac */

#ifdef I_ibmappnNodeLsLocalDlsSap
      case I_ibmappnNodeLsLocalDlsSap:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsLocalDlsSap));
	break;
#endif				       /* I_ibmappnNodeLsLocalDlsSap */

#ifdef I_ibmappnNodeLsRemoteDlsMac
      case I_ibmappnNodeLsRemoteDlsMac:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsRemoteDlsMac));
	break;
#endif				       /* I_ibmappnNodeLsRemoteDlsMac */

#ifdef I_ibmappnNodeLsRemoteDlsSap
      case I_ibmappnNodeLsRemoteDlsSap:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsRemoteDlsSap));
	break;
#endif				       /* I_ibmappnNodeLsRemoteDlsSap */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodeLsTrEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodeLsTrEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodeLsTrEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNodeLsTrName = NULL;
    int             ibmappnNodeLsTrName_offset;
    int             index;
    int             final_index;

    ibmappnNodeLsTrName_offset = object->oid.length;
    final_index = ibmappnNodeLsTrName_offset + GetVariableIndexLength(incoming, ibmappnNodeLsTrName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNodeLsTrName_offset, &ibmappnNodeLsTrName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodeLsTrEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodeLsTrName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNodeLsTrName->length;
	for(i = 0; i < data->ibmappnNodeLsTrName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNodeLsTrName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNodeLsTrName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeLsTrName
      case I_ibmappnNodeLsTrName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsTrName));
	break;
#endif				       /* I_ibmappnNodeLsTrName */

#ifdef I_ibmappnNodeLsTrState
      case I_ibmappnNodeLsTrState:
	dp = (void *) (&data->ibmappnNodeLsTrState);
	break;
#endif				       /* I_ibmappnNodeLsTrState */

#ifdef I_ibmappnNodeLsLocalTrMac
      case I_ibmappnNodeLsLocalTrMac:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsLocalTrMac));
	break;
#endif				       /* I_ibmappnNodeLsLocalTrMac */

#ifdef I_ibmappnNodeLsLocalTrSap
      case I_ibmappnNodeLsLocalTrSap:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsLocalTrSap));
	break;
#endif				       /* I_ibmappnNodeLsLocalTrSap */

#ifdef I_ibmappnNodeLsRemoteTrMac
      case I_ibmappnNodeLsRemoteTrMac:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsRemoteTrMac));
	break;
#endif				       /* I_ibmappnNodeLsRemoteTrMac */

#ifdef I_ibmappnNodeLsRemoteTrSap
      case I_ibmappnNodeLsRemoteTrSap:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsRemoteTrSap));
	break;
#endif				       /* I_ibmappnNodeLsRemoteTrSap */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNodeLsStatusEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNodeLsStatusEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    ibmappnNodeLsStatusEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            ibmappnNodeLsStatusIndex;


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

    if ( (InstToInt(incoming, 0 + object->oid.length, &ibmappnNodeLsStatusIndex, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNodeLsStatusEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNodeLsStatusIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ibmappnNodeLsStatusIndex;
    }

    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeLsStatusIndex
      case I_ibmappnNodeLsStatusIndex:
	dp = (void *) (&data->ibmappnNodeLsStatusIndex);
	break;
#endif				       /* I_ibmappnNodeLsStatusIndex */

#ifdef I_ibmappnNodeLsStatusTime
      case I_ibmappnNodeLsStatusTime:
	dp = (void *) (&data->ibmappnNodeLsStatusTime);
	break;
#endif				       /* I_ibmappnNodeLsStatusTime */

#ifdef I_ibmappnNodeLsStatusLsName
      case I_ibmappnNodeLsStatusLsName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusLsName));
	break;
#endif				       /* I_ibmappnNodeLsStatusLsName */

#ifdef I_ibmappnNodeLsStatusCpName
      case I_ibmappnNodeLsStatusCpName:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusCpName));
	break;
#endif				       /* I_ibmappnNodeLsStatusCpName */

#ifdef I_ibmappnNodeLsStatusNodeId
      case I_ibmappnNodeLsStatusNodeId:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusNodeId));
	break;
#endif				       /* I_ibmappnNodeLsStatusNodeId */

#ifdef I_ibmappnNodeLsStatusTgNum
      case I_ibmappnNodeLsStatusTgNum:
	dp = (void *) (&data->ibmappnNodeLsStatusTgNum);
	break;
#endif				       /* I_ibmappnNodeLsStatusTgNum */

#ifdef I_ibmappnNodeLsStatusGeneralSense
      case I_ibmappnNodeLsStatusGeneralSense:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusGeneralSense));
	break;
#endif				       /* I_ibmappnNodeLsStatusGeneralSense */

#ifdef I_ibmappnNodeLsStatusNofRetry
      case I_ibmappnNodeLsStatusNofRetry:
	dp = (void *) (&data->ibmappnNodeLsStatusNofRetry);
	break;
#endif				       /* I_ibmappnNodeLsStatusNofRetry */

#ifdef I_ibmappnNodeLsStatusEndSense
      case I_ibmappnNodeLsStatusEndSense:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusEndSense));
	break;
#endif				       /* I_ibmappnNodeLsStatusEndSense */

#ifdef I_ibmappnNodeLsStatusXidLocalSense
      case I_ibmappnNodeLsStatusXidLocalSense:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusXidLocalSense));
	break;
#endif				       /* I_ibmappnNodeLsStatusXidLocalSense */

#ifdef I_ibmappnNodeLsStatusXidRemoteSense
      case I_ibmappnNodeLsStatusXidRemoteSense:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusXidRemoteSense));
	break;
#endif				       /* I_ibmappnNodeLsStatusXidRemoteSense */

#ifdef I_ibmappnNodeLsStatusXidByteInError
      case I_ibmappnNodeLsStatusXidByteInError:
	dp = (void *) (&data->ibmappnNodeLsStatusXidByteInError);
	break;
#endif				       /* I_ibmappnNodeLsStatusXidByteInError */

#ifdef I_ibmappnNodeLsStatusXidBitInError
      case I_ibmappnNodeLsStatusXidBitInError:
	dp = (void *) (&data->ibmappnNodeLsStatusXidBitInError);
	break;
#endif				       /* I_ibmappnNodeLsStatusXidBitInError */

#ifdef I_ibmappnNodeLsStatusDlcType
      case I_ibmappnNodeLsStatusDlcType:
	dp = (void *) (&data->ibmappnNodeLsStatusDlcType);
	break;
#endif				       /* I_ibmappnNodeLsStatusDlcType */

#ifdef I_ibmappnNodeLsStatusLocalAddr
      case I_ibmappnNodeLsStatusLocalAddr:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusLocalAddr));
	break;
#endif				       /* I_ibmappnNodeLsStatusLocalAddr */

#ifdef I_ibmappnNodeLsStatusRemoteAddr
      case I_ibmappnNodeLsStatusRemoteAddr:
	dp = (void *) (CloneOctetString(data->ibmappnNodeLsStatusRemoteAddr));
	break;
#endif				       /* I_ibmappnNodeLsStatusRemoteAddr */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnSnmpInformation family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnSnmpInformation_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnSnmpInformation_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnSnmpInformation_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnSnmpInPkts
      case I_ibmappnSnmpInPkts:
	dp = (void *) (&data->ibmappnSnmpInPkts);
	break;
#endif				       /* I_ibmappnSnmpInPkts */

#ifdef I_ibmappnSnmpInGetRequests
      case I_ibmappnSnmpInGetRequests:
	dp = (void *) (&data->ibmappnSnmpInGetRequests);
	break;
#endif				       /* I_ibmappnSnmpInGetRequests */

#ifdef I_ibmappnSnmpInGetNexts
      case I_ibmappnSnmpInGetNexts:
	dp = (void *) (&data->ibmappnSnmpInGetNexts);
	break;
#endif				       /* I_ibmappnSnmpInGetNexts */

#ifdef I_ibmappnSnmpInSetRequests
      case I_ibmappnSnmpInSetRequests:
	dp = (void *) (&data->ibmappnSnmpInSetRequests);
	break;
#endif				       /* I_ibmappnSnmpInSetRequests */

#ifdef I_ibmappnSnmpInTotalVars
      case I_ibmappnSnmpInTotalVars:
	dp = (void *) (&data->ibmappnSnmpInTotalVars);
	break;
#endif				       /* I_ibmappnSnmpInTotalVars */

#ifdef I_ibmappnSnmpInGetVars
      case I_ibmappnSnmpInGetVars:
	dp = (void *) (&data->ibmappnSnmpInGetVars);
	break;
#endif				       /* I_ibmappnSnmpInGetVars */

#ifdef I_ibmappnSnmpInGetNextVars
      case I_ibmappnSnmpInGetNextVars:
	dp = (void *) (&data->ibmappnSnmpInGetNextVars);
	break;
#endif				       /* I_ibmappnSnmpInGetNextVars */

#ifdef I_ibmappnSnmpInSetVars
      case I_ibmappnSnmpInSetVars:
	dp = (void *) (&data->ibmappnSnmpInSetVars);
	break;
#endif				       /* I_ibmappnSnmpInSetVars */

#ifdef I_ibmappnSnmpOutNoSuchNames
      case I_ibmappnSnmpOutNoSuchNames:
	dp = (void *) (&data->ibmappnSnmpOutNoSuchNames);
	break;
#endif				       /* I_ibmappnSnmpOutNoSuchNames */

#ifdef I_ibmappnSnmpOutGenErrs
      case I_ibmappnSnmpOutGenErrs:
	dp = (void *) (&data->ibmappnSnmpOutGenErrs);
	break;
#endif				       /* I_ibmappnSnmpOutGenErrs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnMemoryUse family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnMemoryUse_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnMemoryUse_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnMemoryUse_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnMemorySize
      case I_ibmappnMemorySize:
	dp = (void *) (&data->ibmappnMemorySize);
	break;
#endif				       /* I_ibmappnMemorySize */

#ifdef I_ibmappnMemoryUsed
      case I_ibmappnMemoryUsed:
	dp = (void *) (&data->ibmappnMemoryUsed);
	break;
#endif				       /* I_ibmappnMemoryUsed */

#ifdef I_ibmappnMemoryWarnThresh
      case I_ibmappnMemoryWarnThresh:
	dp = (void *) (&data->ibmappnMemoryWarnThresh);
	break;
#endif				       /* I_ibmappnMemoryWarnThresh */

#ifdef I_ibmappnMemoryCritThresh
      case I_ibmappnMemoryCritThresh:
	dp = (void *) (&data->ibmappnMemoryCritThresh);
	break;
#endif				       /* I_ibmappnMemoryCritThresh */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnXidInformation family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnXidInformation_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnXidInformation_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnXidInformation_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNodeDefLsGoodXids
      case I_ibmappnNodeDefLsGoodXids:
	dp = (void *) (&data->ibmappnNodeDefLsGoodXids);
	break;
#endif				       /* I_ibmappnNodeDefLsGoodXids */

#ifdef I_ibmappnNodeDefLsBadXids
      case I_ibmappnNodeDefLsBadXids:
	dp = (void *) (&data->ibmappnNodeDefLsBadXids);
	break;
#endif				       /* I_ibmappnNodeDefLsBadXids */

#ifdef I_ibmappnNodeDynLsGoodXids
      case I_ibmappnNodeDynLsGoodXids:
	dp = (void *) (&data->ibmappnNodeDynLsGoodXids);
	break;
#endif				       /* I_ibmappnNodeDynLsGoodXids */

#ifdef I_ibmappnNodeDynLsBadXids
      case I_ibmappnNodeDynLsBadXids:
	dp = (void *) (&data->ibmappnNodeDynLsBadXids);
	break;
#endif				       /* I_ibmappnNodeDynLsBadXids */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnTopo family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnTopo_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnNnTopo_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnTopo_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNnTopoMaxNodes
      case I_ibmappnNnTopoMaxNodes:
	dp = (void *) (&data->ibmappnNnTopoMaxNodes);
	break;
#endif				       /* I_ibmappnNnTopoMaxNodes */

#ifdef I_ibmappnNnTopoCurNumNodes
      case I_ibmappnNnTopoCurNumNodes:
	dp = (void *) (&data->ibmappnNnTopoCurNumNodes);
	break;
#endif				       /* I_ibmappnNnTopoCurNumNodes */

#ifdef I_ibmappnNnTopoInTdus
      case I_ibmappnNnTopoInTdus:
	dp = (void *) (&data->ibmappnNnTopoInTdus);
	break;
#endif				       /* I_ibmappnNnTopoInTdus */

#ifdef I_ibmappnNnTopoOutTdus
      case I_ibmappnNnTopoOutTdus:
	dp = (void *) (&data->ibmappnNnTopoOutTdus);
	break;
#endif				       /* I_ibmappnNnTopoOutTdus */

#ifdef I_ibmappnNnTopoNodeLowRsns
      case I_ibmappnNnTopoNodeLowRsns:
	dp = (void *) (&data->ibmappnNnTopoNodeLowRsns);
	break;
#endif				       /* I_ibmappnNnTopoNodeLowRsns */

#ifdef I_ibmappnNnTopoNodeEqualRsns
      case I_ibmappnNnTopoNodeEqualRsns:
	dp = (void *) (&data->ibmappnNnTopoNodeEqualRsns);
	break;
#endif				       /* I_ibmappnNnTopoNodeEqualRsns */

#ifdef I_ibmappnNnTopoNodeGoodHighRsns
      case I_ibmappnNnTopoNodeGoodHighRsns:
	dp = (void *) (&data->ibmappnNnTopoNodeGoodHighRsns);
	break;
#endif				       /* I_ibmappnNnTopoNodeGoodHighRsns */

#ifdef I_ibmappnNnTopoNodeBadHighRsns
      case I_ibmappnNnTopoNodeBadHighRsns:
	dp = (void *) (&data->ibmappnNnTopoNodeBadHighRsns);
	break;
#endif				       /* I_ibmappnNnTopoNodeBadHighRsns */

#ifdef I_ibmappnNnTopoNodeStateUpdates
      case I_ibmappnNnTopoNodeStateUpdates:
	dp = (void *) (&data->ibmappnNnTopoNodeStateUpdates);
	break;
#endif				       /* I_ibmappnNnTopoNodeStateUpdates */

#ifdef I_ibmappnNnTopoNodeErrors
      case I_ibmappnNnTopoNodeErrors:
	dp = (void *) (&data->ibmappnNnTopoNodeErrors);
	break;
#endif				       /* I_ibmappnNnTopoNodeErrors */

#ifdef I_ibmappnNnTopoNodeTimerUpdates
      case I_ibmappnNnTopoNodeTimerUpdates:
	dp = (void *) (&data->ibmappnNnTopoNodeTimerUpdates);
	break;
#endif				       /* I_ibmappnNnTopoNodeTimerUpdates */

#ifdef I_ibmappnNnTopoNodePurges
      case I_ibmappnNnTopoNodePurges:
	dp = (void *) (&data->ibmappnNnTopoNodePurges);
	break;
#endif				       /* I_ibmappnNnTopoNodePurges */

#ifdef I_ibmappnNnTopoTgLowRsns
      case I_ibmappnNnTopoTgLowRsns:
	dp = (void *) (&data->ibmappnNnTopoTgLowRsns);
	break;
#endif				       /* I_ibmappnNnTopoTgLowRsns */

#ifdef I_ibmappnNnTopoTgEqualRsns
      case I_ibmappnNnTopoTgEqualRsns:
	dp = (void *) (&data->ibmappnNnTopoTgEqualRsns);
	break;
#endif				       /* I_ibmappnNnTopoTgEqualRsns */

#ifdef I_ibmappnNnTopoTgGoodHighRsns
      case I_ibmappnNnTopoTgGoodHighRsns:
	dp = (void *) (&data->ibmappnNnTopoTgGoodHighRsns);
	break;
#endif				       /* I_ibmappnNnTopoTgGoodHighRsns */

#ifdef I_ibmappnNnTopoTgBadHighRsns
      case I_ibmappnNnTopoTgBadHighRsns:
	dp = (void *) (&data->ibmappnNnTopoTgBadHighRsns);
	break;
#endif				       /* I_ibmappnNnTopoTgBadHighRsns */

#ifdef I_ibmappnNnTopoTgStateUpdates
      case I_ibmappnNnTopoTgStateUpdates:
	dp = (void *) (&data->ibmappnNnTopoTgStateUpdates);
	break;
#endif				       /* I_ibmappnNnTopoTgStateUpdates */

#ifdef I_ibmappnNnTopoTgErrors
      case I_ibmappnNnTopoTgErrors:
	dp = (void *) (&data->ibmappnNnTopoTgErrors);
	break;
#endif				       /* I_ibmappnNnTopoTgErrors */

#ifdef I_ibmappnNnTopoTgTimerUpdates
      case I_ibmappnNnTopoTgTimerUpdates:
	dp = (void *) (&data->ibmappnNnTopoTgTimerUpdates);
	break;
#endif				       /* I_ibmappnNnTopoTgTimerUpdates */

#ifdef I_ibmappnNnTopoTgPurges
      case I_ibmappnNnTopoTgPurges:
	dp = (void *) (&data->ibmappnNnTopoTgPurges);
	break;
#endif				       /* I_ibmappnNnTopoTgPurges */

#ifdef I_ibmappnNnTopoTotalRouteCalcs
      case I_ibmappnNnTopoTotalRouteCalcs:
	dp = (void *) (&data->ibmappnNnTopoTotalRouteCalcs);
	break;
#endif				       /* I_ibmappnNnTopoTotalRouteCalcs */

#ifdef I_ibmappnNnTopoTotalRouteRejs
      case I_ibmappnNnTopoTotalRouteRejs:
	dp = (void *) (&data->ibmappnNnTopoTotalRouteRejs);
	break;
#endif				       /* I_ibmappnNnTopoTotalRouteRejs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnTopoRouteEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnTopoRouteEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNnTopoRouteEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNnTopoRouteCos = NULL;
    int             ibmappnNnTopoRouteCos_offset;
    int             index;
    int             final_index;

    ibmappnNnTopoRouteCos_offset = object->oid.length;
    final_index = ibmappnNnTopoRouteCos_offset + GetVariableIndexLength(incoming, ibmappnNnTopoRouteCos_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNnTopoRouteCos_offset, &ibmappnNnTopoRouteCos, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnTopoRouteEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNnTopoRouteCos)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNnTopoRouteCos->length;
	for(i = 0; i < data->ibmappnNnTopoRouteCos->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTopoRouteCos->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNnTopoRouteCos);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNnTopoRouteCos
      case I_ibmappnNnTopoRouteCos:
	dp = (void *) (CloneOctetString(data->ibmappnNnTopoRouteCos));
	break;
#endif				       /* I_ibmappnNnTopoRouteCos */

#ifdef I_ibmappnNnTopoRouteTrees
      case I_ibmappnNnTopoRouteTrees:
	dp = (void *) (&data->ibmappnNnTopoRouteTrees);
	break;
#endif				       /* I_ibmappnNnTopoRouteTrees */

#ifdef I_ibmappnNnTopoRouteCalcs
      case I_ibmappnNnTopoRouteCalcs:
	dp = (void *) (&data->ibmappnNnTopoRouteCalcs);
	break;
#endif				       /* I_ibmappnNnTopoRouteCalcs */

#ifdef I_ibmappnNnTopoRouteRejs
      case I_ibmappnNnTopoRouteRejs:
	dp = (void *) (&data->ibmappnNnTopoRouteRejs);
	break;
#endif				       /* I_ibmappnNnTopoRouteRejs */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnAdjNodeEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnAdjNodeEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNnAdjNodeEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNnAdjNodeAdjName = NULL;
    int             ibmappnNnAdjNodeAdjName_offset;
    int             index;
    int             final_index;

    ibmappnNnAdjNodeAdjName_offset = object->oid.length;
    final_index = ibmappnNnAdjNodeAdjName_offset + GetVariableIndexLength(incoming, ibmappnNnAdjNodeAdjName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNnAdjNodeAdjName_offset, &ibmappnNnAdjNodeAdjName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnAdjNodeEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNnAdjNodeAdjName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNnAdjNodeAdjName->length;
	for(i = 0; i < data->ibmappnNnAdjNodeAdjName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnAdjNodeAdjName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNnAdjNodeAdjName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNnAdjNodeAdjName
      case I_ibmappnNnAdjNodeAdjName:
	dp = (void *) (CloneOctetString(data->ibmappnNnAdjNodeAdjName));
	break;
#endif				       /* I_ibmappnNnAdjNodeAdjName */

#ifdef I_ibmappnNnAdjNodeCpCpSessStatus
      case I_ibmappnNnAdjNodeCpCpSessStatus:
	dp = (void *) (&data->ibmappnNnAdjNodeCpCpSessStatus);
	break;
#endif				       /* I_ibmappnNnAdjNodeCpCpSessStatus */

#ifdef I_ibmappnNnAdjNodeOutOfSeqTdus
      case I_ibmappnNnAdjNodeOutOfSeqTdus:
	dp = (void *) (&data->ibmappnNnAdjNodeOutOfSeqTdus);
	break;
#endif				       /* I_ibmappnNnAdjNodeOutOfSeqTdus */

#ifdef I_ibmappnNnAdjNodeLastFrsnSent
      case I_ibmappnNnAdjNodeLastFrsnSent:
	dp = (void *) (&data->ibmappnNnAdjNodeLastFrsnSent);
	break;
#endif				       /* I_ibmappnNnAdjNodeLastFrsnSent */

#ifdef I_ibmappnNnAdjNodeLastFrsnRcvd
      case I_ibmappnNnAdjNodeLastFrsnRcvd:
	dp = (void *) (&data->ibmappnNnAdjNodeLastFrsnRcvd);
	break;
#endif				       /* I_ibmappnNnAdjNodeLastFrsnRcvd */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnTopologyEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnTopologyEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNnTopologyEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNnNodeName = NULL;
    int             ibmappnNnNodeName_offset;
    int             index;
    int             final_index;

    ibmappnNnNodeName_offset = object->oid.length;
    final_index = ibmappnNnNodeName_offset + GetVariableIndexLength(incoming, ibmappnNnNodeName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNnNodeName_offset, &ibmappnNnNodeName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnTopologyEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNnNodeName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNnNodeName->length;
	for(i = 0; i < data->ibmappnNnNodeName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnNodeName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNnNodeName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNnNodeName
      case I_ibmappnNnNodeName:
	dp = (void *) (CloneOctetString(data->ibmappnNnNodeName));
	break;
#endif				       /* I_ibmappnNnNodeName */

#ifdef I_ibmappnNnNodeFrsn
      case I_ibmappnNnNodeFrsn:
	dp = (void *) (&data->ibmappnNnNodeFrsn);
	break;
#endif				       /* I_ibmappnNnNodeFrsn */

#ifdef I_ibmappnNnNodeEntryTimeLeft
      case I_ibmappnNnNodeEntryTimeLeft:
	dp = (void *) (&data->ibmappnNnNodeEntryTimeLeft);
	break;
#endif				       /* I_ibmappnNnNodeEntryTimeLeft */

#ifdef I_ibmappnNnNodeType
      case I_ibmappnNnNodeType:
	dp = (void *) (&data->ibmappnNnNodeType);
	break;
#endif				       /* I_ibmappnNnNodeType */

#ifdef I_ibmappnNnNodeRsn
      case I_ibmappnNnNodeRsn:
	dp = (void *) (&data->ibmappnNnNodeRsn);
	break;
#endif				       /* I_ibmappnNnNodeRsn */

#ifdef I_ibmappnNnNodeRouteAddResist
      case I_ibmappnNnNodeRouteAddResist:
	dp = (void *) (&data->ibmappnNnNodeRouteAddResist);
	break;
#endif				       /* I_ibmappnNnNodeRouteAddResist */

#ifdef I_ibmappnNnNodeCongested
      case I_ibmappnNnNodeCongested:
	dp = (void *) (&data->ibmappnNnNodeCongested);
	break;
#endif				       /* I_ibmappnNnNodeCongested */

#ifdef I_ibmappnNnNodeIsrDepleted
      case I_ibmappnNnNodeIsrDepleted:
	dp = (void *) (&data->ibmappnNnNodeIsrDepleted);
	break;
#endif				       /* I_ibmappnNnNodeIsrDepleted */

#ifdef I_ibmappnNnNodeEndptDepleted
      case I_ibmappnNnNodeEndptDepleted:
	dp = (void *) (&data->ibmappnNnNodeEndptDepleted);
	break;
#endif				       /* I_ibmappnNnNodeEndptDepleted */

#ifdef I_ibmappnNnNodeQuiescing
      case I_ibmappnNnNodeQuiescing:
	dp = (void *) (&data->ibmappnNnNodeQuiescing);
	break;
#endif				       /* I_ibmappnNnNodeQuiescing */

#ifdef I_ibmappnNnNodeGateway
      case I_ibmappnNnNodeGateway:
	dp = (void *) (&data->ibmappnNnNodeGateway);
	break;
#endif				       /* I_ibmappnNnNodeGateway */

#ifdef I_ibmappnNnNodeCentralDirectory
      case I_ibmappnNnNodeCentralDirectory:
	dp = (void *) (&data->ibmappnNnNodeCentralDirectory);
	break;
#endif				       /* I_ibmappnNnNodeCentralDirectory */

#ifdef I_ibmappnNnNodeIsr
      case I_ibmappnNnNodeIsr:
	dp = (void *) (&data->ibmappnNnNodeIsr);
	break;
#endif				       /* I_ibmappnNnNodeIsr */

#ifdef I_ibmappnNnNodeChainSupport
      case I_ibmappnNnNodeChainSupport:
	dp = (void *) (&data->ibmappnNnNodeChainSupport);
	break;
#endif				       /* I_ibmappnNnNodeChainSupport */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnTgTopologyEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnTgTopologyEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNnTgTopologyEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnNnTgOwner = NULL;
    int             ibmappnNnTgOwner_offset;
    OctetString *   ibmappnNnTgDest = NULL;
    int             ibmappnNnTgDest_offset;
    long            ibmappnNnTgNum;
    int             ibmappnNnTgNum_offset;
    int             index;
    int             final_index;

    ibmappnNnTgOwner_offset = object->oid.length;
    ibmappnNnTgDest_offset = ibmappnNnTgOwner_offset + GetVariableIndexLength(incoming, ibmappnNnTgOwner_offset);
    ibmappnNnTgNum_offset = ibmappnNnTgDest_offset + GetVariableIndexLength(incoming, ibmappnNnTgDest_offset);
    final_index = ibmappnNnTgNum_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToInt(incoming, ibmappnNnTgNum_offset, &ibmappnNnTgNum, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (searchType == NEXT) {
        ibmappnNnTgNum = MAX(0, ibmappnNnTgNum);
    }
    if ( (InstToVariableOctetString(incoming, ibmappnNnTgDest_offset, &ibmappnNnTgDest, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToVariableOctetString(incoming, ibmappnNnTgOwner_offset, &ibmappnNnTgOwner, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnTgTopologyEntry_get(serialNum, contextInfo, arg, searchType, ibmappnNnTgOwner, ibmappnNnTgDest, ibmappnNnTgNum)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnNnTgOwner->length;
	for(i = 0; i < data->ibmappnNnTgOwner->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTgOwner->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

        inst.oid_ptr[index++] = data->ibmappnNnTgDest->length;
	for(i = 0; i < data->ibmappnNnTgDest->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTgDest->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTgNum;
	inst.length = index;
    }

	FreeOctetString(ibmappnNnTgOwner);
	FreeOctetString(ibmappnNnTgDest);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNnTgOwner
      case I_ibmappnNnTgOwner:
	dp = (void *) (CloneOctetString(data->ibmappnNnTgOwner));
	break;
#endif				       /* I_ibmappnNnTgOwner */

#ifdef I_ibmappnNnTgDest
      case I_ibmappnNnTgDest:
	dp = (void *) (CloneOctetString(data->ibmappnNnTgDest));
	break;
#endif				       /* I_ibmappnNnTgDest */

#ifdef I_ibmappnNnTgNum
      case I_ibmappnNnTgNum:
	dp = (void *) (&data->ibmappnNnTgNum);
	break;
#endif				       /* I_ibmappnNnTgNum */

#ifdef I_ibmappnNnTgFrsn
      case I_ibmappnNnTgFrsn:
	dp = (void *) (&data->ibmappnNnTgFrsn);
	break;
#endif				       /* I_ibmappnNnTgFrsn */

#ifdef I_ibmappnNnTgEntryTimeLeft
      case I_ibmappnNnTgEntryTimeLeft:
	dp = (void *) (&data->ibmappnNnTgEntryTimeLeft);
	break;
#endif				       /* I_ibmappnNnTgEntryTimeLeft */

#ifdef I_ibmappnNnTgDestVirtual
      case I_ibmappnNnTgDestVirtual:
	dp = (void *) (&data->ibmappnNnTgDestVirtual);
	break;
#endif				       /* I_ibmappnNnTgDestVirtual */

#ifdef I_ibmappnNnTgDlcData
      case I_ibmappnNnTgDlcData:
	dp = (void *) (CloneOctetString(data->ibmappnNnTgDlcData));
	break;
#endif				       /* I_ibmappnNnTgDlcData */

#ifdef I_ibmappnNnTgRsn
      case I_ibmappnNnTgRsn:
	dp = (void *) (&data->ibmappnNnTgRsn);
	break;
#endif				       /* I_ibmappnNnTgRsn */

#ifdef I_ibmappnNnTgOperational
      case I_ibmappnNnTgOperational:
	dp = (void *) (&data->ibmappnNnTgOperational);
	break;
#endif				       /* I_ibmappnNnTgOperational */

#ifdef I_ibmappnNnTgQuiescing
      case I_ibmappnNnTgQuiescing:
	dp = (void *) (&data->ibmappnNnTgQuiescing);
	break;
#endif				       /* I_ibmappnNnTgQuiescing */

#ifdef I_ibmappnNnTgCpCpSession
      case I_ibmappnNnTgCpCpSession:
	dp = (void *) (&data->ibmappnNnTgCpCpSession);
	break;
#endif				       /* I_ibmappnNnTgCpCpSession */

#ifdef I_ibmappnNnTgEffCap
      case I_ibmappnNnTgEffCap:
	dp = (void *) (&data->ibmappnNnTgEffCap);
	break;
#endif				       /* I_ibmappnNnTgEffCap */

#ifdef I_ibmappnNnTgConnCost
      case I_ibmappnNnTgConnCost:
	dp = (void *) (&data->ibmappnNnTgConnCost);
	break;
#endif				       /* I_ibmappnNnTgConnCost */

#ifdef I_ibmappnNnTgByteCost
      case I_ibmappnNnTgByteCost:
	dp = (void *) (&data->ibmappnNnTgByteCost);
	break;
#endif				       /* I_ibmappnNnTgByteCost */

#ifdef I_ibmappnNnTgSecurity
      case I_ibmappnNnTgSecurity:
	dp = (void *) (&data->ibmappnNnTgSecurity);
	break;
#endif				       /* I_ibmappnNnTgSecurity */

#ifdef I_ibmappnNnTgDelay
      case I_ibmappnNnTgDelay:
	dp = (void *) (&data->ibmappnNnTgDelay);
	break;
#endif				       /* I_ibmappnNnTgDelay */

#ifdef I_ibmappnNnTgModemClass
      case I_ibmappnNnTgModemClass:
	dp = (void *) (&data->ibmappnNnTgModemClass);
	break;
#endif				       /* I_ibmappnNnTgModemClass */

#ifdef I_ibmappnNnTgUsr1
      case I_ibmappnNnTgUsr1:
	dp = (void *) (&data->ibmappnNnTgUsr1);
	break;
#endif				       /* I_ibmappnNnTgUsr1 */

#ifdef I_ibmappnNnTgUsr2
      case I_ibmappnNnTgUsr2:
	dp = (void *) (&data->ibmappnNnTgUsr2);
	break;
#endif				       /* I_ibmappnNnTgUsr2 */

#ifdef I_ibmappnNnTgUsr3
      case I_ibmappnNnTgUsr3:
	dp = (void *) (&data->ibmappnNnTgUsr3);
	break;
#endif				       /* I_ibmappnNnTgUsr3 */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnTopologyFREntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnTopologyFREntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNnTopologyFREntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            ibmappnNnNodeFRFrsn;
    int             ibmappnNnNodeFRFrsn_offset;
    OctetString *   ibmappnNnNodeFRName = NULL;
    int             ibmappnNnNodeFRName_offset;
    int             index;
    int             final_index;

    ibmappnNnNodeFRFrsn_offset = object->oid.length;
    ibmappnNnNodeFRName_offset = ibmappnNnNodeFRFrsn_offset + 1;
    final_index = ibmappnNnNodeFRName_offset + GetVariableIndexLength(incoming, ibmappnNnNodeFRName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnNnNodeFRName_offset, &ibmappnNnNodeFRName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToInt(incoming, ibmappnNnNodeFRFrsn_offset, &ibmappnNnNodeFRFrsn, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (searchType == NEXT) {
        ibmappnNnNodeFRFrsn = MAX(0, ibmappnNnNodeFRFrsn);
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnTopologyFREntry_get(serialNum, contextInfo, arg, searchType, ibmappnNnNodeFRFrsn, ibmappnNnNodeFRName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
	inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnNodeFRFrsn;
        inst.oid_ptr[index++] = data->ibmappnNnNodeFRName->length;
	for(i = 0; i < data->ibmappnNnNodeFRName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnNodeFRName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnNnNodeFRName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNnNodeFRName
      case I_ibmappnNnNodeFRName:
	dp = (void *) (CloneOctetString(data->ibmappnNnNodeFRName));
	break;
#endif				       /* I_ibmappnNnNodeFRName */

#ifdef I_ibmappnNnNodeFRFrsn
      case I_ibmappnNnNodeFRFrsn:
	dp = (void *) (&data->ibmappnNnNodeFRFrsn);
	break;
#endif				       /* I_ibmappnNnNodeFRFrsn */

#ifdef I_ibmappnNnNodeFREntryTimeLeft
      case I_ibmappnNnNodeFREntryTimeLeft:
	dp = (void *) (&data->ibmappnNnNodeFREntryTimeLeft);
	break;
#endif				       /* I_ibmappnNnNodeFREntryTimeLeft */

#ifdef I_ibmappnNnNodeFRType
      case I_ibmappnNnNodeFRType:
	dp = (void *) (&data->ibmappnNnNodeFRType);
	break;
#endif				       /* I_ibmappnNnNodeFRType */

#ifdef I_ibmappnNnNodeFRRsn
      case I_ibmappnNnNodeFRRsn:
	dp = (void *) (&data->ibmappnNnNodeFRRsn);
	break;
#endif				       /* I_ibmappnNnNodeFRRsn */

#ifdef I_ibmappnNnNodeFRRouteAddResist
      case I_ibmappnNnNodeFRRouteAddResist:
	dp = (void *) (&data->ibmappnNnNodeFRRouteAddResist);
	break;
#endif				       /* I_ibmappnNnNodeFRRouteAddResist */

#ifdef I_ibmappnNnNodeFRCongested
      case I_ibmappnNnNodeFRCongested:
	dp = (void *) (&data->ibmappnNnNodeFRCongested);
	break;
#endif				       /* I_ibmappnNnNodeFRCongested */

#ifdef I_ibmappnNnNodeFRIsrDepleted
      case I_ibmappnNnNodeFRIsrDepleted:
	dp = (void *) (&data->ibmappnNnNodeFRIsrDepleted);
	break;
#endif				       /* I_ibmappnNnNodeFRIsrDepleted */

#ifdef I_ibmappnNnNodeFREndptDepleted
      case I_ibmappnNnNodeFREndptDepleted:
	dp = (void *) (&data->ibmappnNnNodeFREndptDepleted);
	break;
#endif				       /* I_ibmappnNnNodeFREndptDepleted */

#ifdef I_ibmappnNnNodeFRQuiescing
      case I_ibmappnNnNodeFRQuiescing:
	dp = (void *) (&data->ibmappnNnNodeFRQuiescing);
	break;
#endif				       /* I_ibmappnNnNodeFRQuiescing */

#ifdef I_ibmappnNnNodeFRGateway
      case I_ibmappnNnNodeFRGateway:
	dp = (void *) (&data->ibmappnNnNodeFRGateway);
	break;
#endif				       /* I_ibmappnNnNodeFRGateway */

#ifdef I_ibmappnNnNodeFRCentralDirectory
      case I_ibmappnNnNodeFRCentralDirectory:
	dp = (void *) (&data->ibmappnNnNodeFRCentralDirectory);
	break;
#endif				       /* I_ibmappnNnNodeFRCentralDirectory */

#ifdef I_ibmappnNnNodeFRIsr
      case I_ibmappnNnNodeFRIsr:
	dp = (void *) (&data->ibmappnNnNodeFRIsr);
	break;
#endif				       /* I_ibmappnNnNodeFRIsr */

#ifdef I_ibmappnNnNodeFRChainSupport
      case I_ibmappnNnNodeFRChainSupport:
	dp = (void *) (&data->ibmappnNnNodeFRChainSupport);
	break;
#endif				       /* I_ibmappnNnNodeFRChainSupport */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnNnTgTopologyFREntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnNnTgTopologyFREntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnNnTgTopologyFREntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    long            ibmappnNnTgFRFrsn;
    int             ibmappnNnTgFRFrsn_offset;
    OctetString *   ibmappnNnTgFROwner = NULL;
    int             ibmappnNnTgFROwner_offset;
    OctetString *   ibmappnNnTgFRDest = NULL;
    int             ibmappnNnTgFRDest_offset;
    long            ibmappnNnTgFRNum;
    int             ibmappnNnTgFRNum_offset;
    int             index;
    int             final_index;

    ibmappnNnTgFRFrsn_offset = object->oid.length;
    ibmappnNnTgFROwner_offset = ibmappnNnTgFRFrsn_offset + 1;
    ibmappnNnTgFRDest_offset = ibmappnNnTgFROwner_offset + GetVariableIndexLength(incoming, ibmappnNnTgFROwner_offset);
    ibmappnNnTgFRNum_offset = ibmappnNnTgFRDest_offset + GetVariableIndexLength(incoming, ibmappnNnTgFRDest_offset);
    final_index = ibmappnNnTgFRNum_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToInt(incoming, ibmappnNnTgFRNum_offset, &ibmappnNnTgFRNum, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (searchType == NEXT) {
        ibmappnNnTgFRNum = MAX(0, ibmappnNnTgFRNum);
    }
    if ( (InstToVariableOctetString(incoming, ibmappnNnTgFRDest_offset, &ibmappnNnTgFRDest, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToVariableOctetString(incoming, ibmappnNnTgFROwner_offset, &ibmappnNnTgFROwner, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToInt(incoming, ibmappnNnTgFRFrsn_offset, &ibmappnNnTgFRFrsn, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (searchType == NEXT) {
        ibmappnNnTgFRFrsn = MAX(0, ibmappnNnTgFRFrsn);
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnNnTgTopologyFREntry_get(serialNum, contextInfo, arg, searchType, ibmappnNnTgFRFrsn, ibmappnNnTgFROwner, ibmappnNnTgFRDest, ibmappnNnTgFRNum)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
	inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTgFRFrsn;
        inst.oid_ptr[index++] = data->ibmappnNnTgFROwner->length;
	for(i = 0; i < data->ibmappnNnTgFROwner->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTgFROwner->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

        inst.oid_ptr[index++] = data->ibmappnNnTgFRDest->length;
	for(i = 0; i < data->ibmappnNnTgFRDest->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTgFRDest->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.oid_ptr[index++] = (unsigned long) data->ibmappnNnTgFRNum;
	inst.length = index;
    }

	FreeOctetString(ibmappnNnTgFROwner);
	FreeOctetString(ibmappnNnTgFRDest);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnNnTgFROwner
      case I_ibmappnNnTgFROwner:
	dp = (void *) (CloneOctetString(data->ibmappnNnTgFROwner));
	break;
#endif				       /* I_ibmappnNnTgFROwner */

#ifdef I_ibmappnNnTgFRDest
      case I_ibmappnNnTgFRDest:
	dp = (void *) (CloneOctetString(data->ibmappnNnTgFRDest));
	break;
#endif				       /* I_ibmappnNnTgFRDest */

#ifdef I_ibmappnNnTgFRNum
      case I_ibmappnNnTgFRNum:
	dp = (void *) (&data->ibmappnNnTgFRNum);
	break;
#endif				       /* I_ibmappnNnTgFRNum */

#ifdef I_ibmappnNnTgFRFrsn
      case I_ibmappnNnTgFRFrsn:
	dp = (void *) (&data->ibmappnNnTgFRFrsn);
	break;
#endif				       /* I_ibmappnNnTgFRFrsn */

#ifdef I_ibmappnNnTgFREntryTimeLeft
      case I_ibmappnNnTgFREntryTimeLeft:
	dp = (void *) (&data->ibmappnNnTgFREntryTimeLeft);
	break;
#endif				       /* I_ibmappnNnTgFREntryTimeLeft */

#ifdef I_ibmappnNnTgFRDestVirtual
      case I_ibmappnNnTgFRDestVirtual:
	dp = (void *) (&data->ibmappnNnTgFRDestVirtual);
	break;
#endif				       /* I_ibmappnNnTgFRDestVirtual */

#ifdef I_ibmappnNnTgFRDlcData
      case I_ibmappnNnTgFRDlcData:
	dp = (void *) (CloneOctetString(data->ibmappnNnTgFRDlcData));
	break;
#endif				       /* I_ibmappnNnTgFRDlcData */

#ifdef I_ibmappnNnTgFRRsn
      case I_ibmappnNnTgFRRsn:
	dp = (void *) (&data->ibmappnNnTgFRRsn);
	break;
#endif				       /* I_ibmappnNnTgFRRsn */

#ifdef I_ibmappnNnTgFROperational
      case I_ibmappnNnTgFROperational:
	dp = (void *) (&data->ibmappnNnTgFROperational);
	break;
#endif				       /* I_ibmappnNnTgFROperational */

#ifdef I_ibmappnNnTgFRQuiescing
      case I_ibmappnNnTgFRQuiescing:
	dp = (void *) (&data->ibmappnNnTgFRQuiescing);
	break;
#endif				       /* I_ibmappnNnTgFRQuiescing */

#ifdef I_ibmappnNnTgFRCpCpSession
      case I_ibmappnNnTgFRCpCpSession:
	dp = (void *) (&data->ibmappnNnTgFRCpCpSession);
	break;
#endif				       /* I_ibmappnNnTgFRCpCpSession */

#ifdef I_ibmappnNnTgFREffCap
      case I_ibmappnNnTgFREffCap:
	dp = (void *) (&data->ibmappnNnTgFREffCap);
	break;
#endif				       /* I_ibmappnNnTgFREffCap */

#ifdef I_ibmappnNnTgFRConnCost
      case I_ibmappnNnTgFRConnCost:
	dp = (void *) (&data->ibmappnNnTgFRConnCost);
	break;
#endif				       /* I_ibmappnNnTgFRConnCost */

#ifdef I_ibmappnNnTgFRByteCost
      case I_ibmappnNnTgFRByteCost:
	dp = (void *) (&data->ibmappnNnTgFRByteCost);
	break;
#endif				       /* I_ibmappnNnTgFRByteCost */

#ifdef I_ibmappnNnTgFRSecurity
      case I_ibmappnNnTgFRSecurity:
	dp = (void *) (&data->ibmappnNnTgFRSecurity);
	break;
#endif				       /* I_ibmappnNnTgFRSecurity */

#ifdef I_ibmappnNnTgFRDelay
      case I_ibmappnNnTgFRDelay:
	dp = (void *) (&data->ibmappnNnTgFRDelay);
	break;
#endif				       /* I_ibmappnNnTgFRDelay */

#ifdef I_ibmappnNnTgFRModemClass
      case I_ibmappnNnTgFRModemClass:
	dp = (void *) (&data->ibmappnNnTgFRModemClass);
	break;
#endif				       /* I_ibmappnNnTgFRModemClass */

#ifdef I_ibmappnNnTgFRUsr1
      case I_ibmappnNnTgFRUsr1:
	dp = (void *) (&data->ibmappnNnTgFRUsr1);
	break;
#endif				       /* I_ibmappnNnTgFRUsr1 */

#ifdef I_ibmappnNnTgFRUsr2
      case I_ibmappnNnTgFRUsr2:
	dp = (void *) (&data->ibmappnNnTgFRUsr2);
	break;
#endif				       /* I_ibmappnNnTgFRUsr2 */

#ifdef I_ibmappnNnTgFRUsr3
      case I_ibmappnNnTgFRUsr3:
	dp = (void *) (&data->ibmappnNnTgFRUsr3);
	break;
#endif				       /* I_ibmappnNnTgFRUsr3 */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnLocalGeneral family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnLocalGeneral_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnLocalGeneral_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnLocalGeneral_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnLocalNodeName
      case I_ibmappnLocalNodeName:
	dp = (void *) (CloneOctetString(data->ibmappnLocalNodeName));
	break;
#endif				       /* I_ibmappnLocalNodeName */

#ifdef I_ibmappnLocalNodeType
      case I_ibmappnLocalNodeType:
	dp = (void *) (&data->ibmappnLocalNodeType);
	break;
#endif				       /* I_ibmappnLocalNodeType */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnLocalNnSpecific family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnLocalNnSpecific_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnLocalNnSpecific_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnLocalNnSpecific_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnLocalNnRsn
      case I_ibmappnLocalNnRsn:
	dp = (void *) (&data->ibmappnLocalNnRsn);
	break;
#endif				       /* I_ibmappnLocalNnRsn */

#ifdef I_ibmappnLocalNnRouteAddResist
      case I_ibmappnLocalNnRouteAddResist:
	dp = (void *) (&data->ibmappnLocalNnRouteAddResist);
	break;
#endif				       /* I_ibmappnLocalNnRouteAddResist */

#ifdef I_ibmappnLocalNnCongested
      case I_ibmappnLocalNnCongested:
	dp = (void *) (&data->ibmappnLocalNnCongested);
	break;
#endif				       /* I_ibmappnLocalNnCongested */

#ifdef I_ibmappnLocalNnIsrDepleted
      case I_ibmappnLocalNnIsrDepleted:
	dp = (void *) (&data->ibmappnLocalNnIsrDepleted);
	break;
#endif				       /* I_ibmappnLocalNnIsrDepleted */

#ifdef I_ibmappnLocalNnEndptDepleted
      case I_ibmappnLocalNnEndptDepleted:
	dp = (void *) (&data->ibmappnLocalNnEndptDepleted);
	break;
#endif				       /* I_ibmappnLocalNnEndptDepleted */

#ifdef I_ibmappnLocalNnQuiescing
      case I_ibmappnLocalNnQuiescing:
	dp = (void *) (&data->ibmappnLocalNnQuiescing);
	break;
#endif				       /* I_ibmappnLocalNnQuiescing */

#ifdef I_ibmappnLocalNnGateway
      case I_ibmappnLocalNnGateway:
	dp = (void *) (&data->ibmappnLocalNnGateway);
	break;
#endif				       /* I_ibmappnLocalNnGateway */

#ifdef I_ibmappnLocalNnCentralDirectory
      case I_ibmappnLocalNnCentralDirectory:
	dp = (void *) (&data->ibmappnLocalNnCentralDirectory);
	break;
#endif				       /* I_ibmappnLocalNnCentralDirectory */

#ifdef I_ibmappnLocalNnIsr
      case I_ibmappnLocalNnIsr:
	dp = (void *) (&data->ibmappnLocalNnIsr);
	break;
#endif				       /* I_ibmappnLocalNnIsr */

#ifdef I_ibmappnLocalNnChainSupport
      case I_ibmappnLocalNnChainSupport:
	dp = (void *) (&data->ibmappnLocalNnChainSupport);
	break;
#endif				       /* I_ibmappnLocalNnChainSupport */

#ifdef I_ibmappnLocalNnFrsn
      case I_ibmappnLocalNnFrsn:
	dp = (void *) (&data->ibmappnLocalNnFrsn);
	break;
#endif				       /* I_ibmappnLocalNnFrsn */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnLocalTgEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnLocalTgEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnLocalTgEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnLocalTgDest = NULL;
    int             ibmappnLocalTgDest_offset;
    long            ibmappnLocalTgNum;
    int             ibmappnLocalTgNum_offset;
    int             index;
    int             final_index;

    ibmappnLocalTgDest_offset = object->oid.length;
    ibmappnLocalTgNum_offset = ibmappnLocalTgDest_offset + GetVariableIndexLength(incoming, ibmappnLocalTgDest_offset);
    final_index = ibmappnLocalTgNum_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToInt(incoming, ibmappnLocalTgNum_offset, &ibmappnLocalTgNum, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToVariableOctetString(incoming, ibmappnLocalTgDest_offset, &ibmappnLocalTgDest, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnLocalTgEntry_get(serialNum, contextInfo, arg, searchType, ibmappnLocalTgDest, ibmappnLocalTgNum)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnLocalTgDest->length;
	for(i = 0; i < data->ibmappnLocalTgDest->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnLocalTgDest->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.oid_ptr[index++] = (unsigned long) data->ibmappnLocalTgNum;
	inst.length = index;
    }

	FreeOctetString(ibmappnLocalTgDest);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnLocalTgDest
      case I_ibmappnLocalTgDest:
	dp = (void *) (CloneOctetString(data->ibmappnLocalTgDest));
	break;
#endif				       /* I_ibmappnLocalTgDest */

#ifdef I_ibmappnLocalTgNum
      case I_ibmappnLocalTgNum:
	dp = (void *) (&data->ibmappnLocalTgNum);
	break;
#endif				       /* I_ibmappnLocalTgNum */

#ifdef I_ibmappnLocalTgDestVirtual
      case I_ibmappnLocalTgDestVirtual:
	dp = (void *) (&data->ibmappnLocalTgDestVirtual);
	break;
#endif				       /* I_ibmappnLocalTgDestVirtual */

#ifdef I_ibmappnLocalTgDlcData
      case I_ibmappnLocalTgDlcData:
	dp = (void *) (CloneOctetString(data->ibmappnLocalTgDlcData));
	break;
#endif				       /* I_ibmappnLocalTgDlcData */

#ifdef I_ibmappnLocalTgRsn
      case I_ibmappnLocalTgRsn:
	dp = (void *) (&data->ibmappnLocalTgRsn);
	break;
#endif				       /* I_ibmappnLocalTgRsn */

#ifdef I_ibmappnLocalTgQuiescing
      case I_ibmappnLocalTgQuiescing:
	dp = (void *) (&data->ibmappnLocalTgQuiescing);
	break;
#endif				       /* I_ibmappnLocalTgQuiescing */

#ifdef I_ibmappnLocalTgOperational
      case I_ibmappnLocalTgOperational:
	dp = (void *) (&data->ibmappnLocalTgOperational);
	break;
#endif				       /* I_ibmappnLocalTgOperational */

#ifdef I_ibmappnLocalTgCpCpSession
      case I_ibmappnLocalTgCpCpSession:
	dp = (void *) (&data->ibmappnLocalTgCpCpSession);
	break;
#endif				       /* I_ibmappnLocalTgCpCpSession */

#ifdef I_ibmappnLocalTgEffCap
      case I_ibmappnLocalTgEffCap:
	dp = (void *) (&data->ibmappnLocalTgEffCap);
	break;
#endif				       /* I_ibmappnLocalTgEffCap */

#ifdef I_ibmappnLocalTgConnCost
      case I_ibmappnLocalTgConnCost:
	dp = (void *) (&data->ibmappnLocalTgConnCost);
	break;
#endif				       /* I_ibmappnLocalTgConnCost */

#ifdef I_ibmappnLocalTgByteCost
      case I_ibmappnLocalTgByteCost:
	dp = (void *) (&data->ibmappnLocalTgByteCost);
	break;
#endif				       /* I_ibmappnLocalTgByteCost */

#ifdef I_ibmappnLocalTgSecurity
      case I_ibmappnLocalTgSecurity:
	dp = (void *) (&data->ibmappnLocalTgSecurity);
	break;
#endif				       /* I_ibmappnLocalTgSecurity */

#ifdef I_ibmappnLocalTgDelay
      case I_ibmappnLocalTgDelay:
	dp = (void *) (&data->ibmappnLocalTgDelay);
	break;
#endif				       /* I_ibmappnLocalTgDelay */

#ifdef I_ibmappnLocalTgModemClass
      case I_ibmappnLocalTgModemClass:
	dp = (void *) (&data->ibmappnLocalTgModemClass);
	break;
#endif				       /* I_ibmappnLocalTgModemClass */

#ifdef I_ibmappnLocalTgUsr1
      case I_ibmappnLocalTgUsr1:
	dp = (void *) (&data->ibmappnLocalTgUsr1);
	break;
#endif				       /* I_ibmappnLocalTgUsr1 */

#ifdef I_ibmappnLocalTgUsr2
      case I_ibmappnLocalTgUsr2:
	dp = (void *) (&data->ibmappnLocalTgUsr2);
	break;
#endif				       /* I_ibmappnLocalTgUsr2 */

#ifdef I_ibmappnLocalTgUsr3
      case I_ibmappnLocalTgUsr3:
	dp = (void *) (&data->ibmappnLocalTgUsr3);
	break;
#endif				       /* I_ibmappnLocalTgUsr3 */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnLocalEnEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnLocalEnEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnLocalEnEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnLocalEnName = NULL;
    int             ibmappnLocalEnName_offset;
    int             index;
    int             final_index;

    ibmappnLocalEnName_offset = object->oid.length;
    final_index = ibmappnLocalEnName_offset + GetVariableIndexLength(incoming, ibmappnLocalEnName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnLocalEnName_offset, &ibmappnLocalEnName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnLocalEnEntry_get(serialNum, contextInfo, arg, searchType, ibmappnLocalEnName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnLocalEnName->length;
	for(i = 0; i < data->ibmappnLocalEnName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnLocalEnName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnLocalEnName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnLocalEnName
      case I_ibmappnLocalEnName:
	dp = (void *) (CloneOctetString(data->ibmappnLocalEnName));
	break;
#endif				       /* I_ibmappnLocalEnName */

#ifdef I_ibmappnLocalEnEntryTimeLeft
      case I_ibmappnLocalEnEntryTimeLeft:
	dp = (void *) (&data->ibmappnLocalEnEntryTimeLeft);
	break;
#endif				       /* I_ibmappnLocalEnEntryTimeLeft */

#ifdef I_ibmappnLocalEnType
      case I_ibmappnLocalEnType:
	dp = (void *) (&data->ibmappnLocalEnType);
	break;
#endif				       /* I_ibmappnLocalEnType */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnLocalEnTgEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnLocalEnTgEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnLocalEnTgEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnLocalEnTgOrigin = NULL;
    int             ibmappnLocalEnTgOrigin_offset;
    OctetString *   ibmappnLocalEnTgDest = NULL;
    int             ibmappnLocalEnTgDest_offset;
    long            ibmappnLocalEnTgNum;
    int             ibmappnLocalEnTgNum_offset;
    int             index;
    int             final_index;

    ibmappnLocalEnTgOrigin_offset = object->oid.length;
    ibmappnLocalEnTgDest_offset = ibmappnLocalEnTgOrigin_offset + GetVariableIndexLength(incoming, ibmappnLocalEnTgOrigin_offset);
    ibmappnLocalEnTgNum_offset = ibmappnLocalEnTgDest_offset + GetVariableIndexLength(incoming, ibmappnLocalEnTgDest_offset);
    final_index = ibmappnLocalEnTgNum_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToInt(incoming, ibmappnLocalEnTgNum_offset, &ibmappnLocalEnTgNum, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToVariableOctetString(incoming, ibmappnLocalEnTgDest_offset, &ibmappnLocalEnTgDest, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if ( (InstToVariableOctetString(incoming, ibmappnLocalEnTgOrigin_offset, &ibmappnLocalEnTgOrigin, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnLocalEnTgEntry_get(serialNum, contextInfo, arg, searchType, ibmappnLocalEnTgOrigin, ibmappnLocalEnTgDest, ibmappnLocalEnTgNum)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnLocalEnTgOrigin->length;
	for(i = 0; i < data->ibmappnLocalEnTgOrigin->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnLocalEnTgOrigin->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

        inst.oid_ptr[index++] = data->ibmappnLocalEnTgDest->length;
	for(i = 0; i < data->ibmappnLocalEnTgDest->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnLocalEnTgDest->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.oid_ptr[index++] = (unsigned long) data->ibmappnLocalEnTgNum;
	inst.length = index;
    }

	FreeOctetString(ibmappnLocalEnTgOrigin);
	FreeOctetString(ibmappnLocalEnTgDest);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnLocalEnTgOrigin
      case I_ibmappnLocalEnTgOrigin:
	dp = (void *) (CloneOctetString(data->ibmappnLocalEnTgOrigin));
	break;
#endif				       /* I_ibmappnLocalEnTgOrigin */

#ifdef I_ibmappnLocalEnTgDest
      case I_ibmappnLocalEnTgDest:
	dp = (void *) (CloneOctetString(data->ibmappnLocalEnTgDest));
	break;
#endif				       /* I_ibmappnLocalEnTgDest */

#ifdef I_ibmappnLocalEnTgNum
      case I_ibmappnLocalEnTgNum:
	dp = (void *) (&data->ibmappnLocalEnTgNum);
	break;
#endif				       /* I_ibmappnLocalEnTgNum */

#ifdef I_ibmappnLocalEnTgEntryTimeLeft
      case I_ibmappnLocalEnTgEntryTimeLeft:
	dp = (void *) (&data->ibmappnLocalEnTgEntryTimeLeft);
	break;
#endif				       /* I_ibmappnLocalEnTgEntryTimeLeft */

#ifdef I_ibmappnLocalEnTgDestVirtual
      case I_ibmappnLocalEnTgDestVirtual:
	dp = (void *) (&data->ibmappnLocalEnTgDestVirtual);
	break;
#endif				       /* I_ibmappnLocalEnTgDestVirtual */

#ifdef I_ibmappnLocalEnTgDlcData
      case I_ibmappnLocalEnTgDlcData:
	dp = (void *) (CloneOctetString(data->ibmappnLocalEnTgDlcData));
	break;
#endif				       /* I_ibmappnLocalEnTgDlcData */

#ifdef I_ibmappnLocalEnTgOperational
      case I_ibmappnLocalEnTgOperational:
	dp = (void *) (&data->ibmappnLocalEnTgOperational);
	break;
#endif				       /* I_ibmappnLocalEnTgOperational */

#ifdef I_ibmappnLocalEnTgCpCpSession
      case I_ibmappnLocalEnTgCpCpSession:
	dp = (void *) (&data->ibmappnLocalEnTgCpCpSession);
	break;
#endif				       /* I_ibmappnLocalEnTgCpCpSession */

#ifdef I_ibmappnLocalEnTgEffCap
      case I_ibmappnLocalEnTgEffCap:
	dp = (void *) (&data->ibmappnLocalEnTgEffCap);
	break;
#endif				       /* I_ibmappnLocalEnTgEffCap */

#ifdef I_ibmappnLocalEnTgConnCost
      case I_ibmappnLocalEnTgConnCost:
	dp = (void *) (&data->ibmappnLocalEnTgConnCost);
	break;
#endif				       /* I_ibmappnLocalEnTgConnCost */

#ifdef I_ibmappnLocalEnTgByteCost
      case I_ibmappnLocalEnTgByteCost:
	dp = (void *) (&data->ibmappnLocalEnTgByteCost);
	break;
#endif				       /* I_ibmappnLocalEnTgByteCost */

#ifdef I_ibmappnLocalEnTgSecurity
      case I_ibmappnLocalEnTgSecurity:
	dp = (void *) (&data->ibmappnLocalEnTgSecurity);
	break;
#endif				       /* I_ibmappnLocalEnTgSecurity */

#ifdef I_ibmappnLocalEnTgDelay
      case I_ibmappnLocalEnTgDelay:
	dp = (void *) (&data->ibmappnLocalEnTgDelay);
	break;
#endif				       /* I_ibmappnLocalEnTgDelay */

#ifdef I_ibmappnLocalEnTgModemClass
      case I_ibmappnLocalEnTgModemClass:
	dp = (void *) (&data->ibmappnLocalEnTgModemClass);
	break;
#endif				       /* I_ibmappnLocalEnTgModemClass */

#ifdef I_ibmappnLocalEnTgUsr1
      case I_ibmappnLocalEnTgUsr1:
	dp = (void *) (&data->ibmappnLocalEnTgUsr1);
	break;
#endif				       /* I_ibmappnLocalEnTgUsr1 */

#ifdef I_ibmappnLocalEnTgUsr2
      case I_ibmappnLocalEnTgUsr2:
	dp = (void *) (&data->ibmappnLocalEnTgUsr2);
	break;
#endif				       /* I_ibmappnLocalEnTgUsr2 */

#ifdef I_ibmappnLocalEnTgUsr3
      case I_ibmappnLocalEnTgUsr3:
	dp = (void *) (&data->ibmappnLocalEnTgUsr3);
	break;
#endif				       /* I_ibmappnLocalEnTgUsr3 */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnDirPerf family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnDirPerf_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnDirPerf_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnDirPerf_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnDirMaxCaches
      case I_ibmappnDirMaxCaches:
	dp = (void *) (&data->ibmappnDirMaxCaches);
	break;
#endif				       /* I_ibmappnDirMaxCaches */

#ifdef I_ibmappnDirCurCaches
      case I_ibmappnDirCurCaches:
	dp = (void *) (&data->ibmappnDirCurCaches);
	break;
#endif				       /* I_ibmappnDirCurCaches */

#ifdef I_ibmappnDirCurHomeEntries
      case I_ibmappnDirCurHomeEntries:
	dp = (void *) (&data->ibmappnDirCurHomeEntries);
	break;
#endif				       /* I_ibmappnDirCurHomeEntries */

#ifdef I_ibmappnDirRegEntries
      case I_ibmappnDirRegEntries:
	dp = (void *) (&data->ibmappnDirRegEntries);
	break;
#endif				       /* I_ibmappnDirRegEntries */

#ifdef I_ibmappnDirInLocates
      case I_ibmappnDirInLocates:
	dp = (void *) (&data->ibmappnDirInLocates);
	break;
#endif				       /* I_ibmappnDirInLocates */

#ifdef I_ibmappnDirInBcastLocates
      case I_ibmappnDirInBcastLocates:
	dp = (void *) (&data->ibmappnDirInBcastLocates);
	break;
#endif				       /* I_ibmappnDirInBcastLocates */

#ifdef I_ibmappnDirOutLocates
      case I_ibmappnDirOutLocates:
	dp = (void *) (&data->ibmappnDirOutLocates);
	break;
#endif				       /* I_ibmappnDirOutLocates */

#ifdef I_ibmappnDirOutBcastLocates
      case I_ibmappnDirOutBcastLocates:
	dp = (void *) (&data->ibmappnDirOutBcastLocates);
	break;
#endif				       /* I_ibmappnDirOutBcastLocates */

#ifdef I_ibmappnDirNotFoundLocates
      case I_ibmappnDirNotFoundLocates:
	dp = (void *) (&data->ibmappnDirNotFoundLocates);
	break;
#endif				       /* I_ibmappnDirNotFoundLocates */

#ifdef I_ibmappnDirNotFoundBcastLocates
      case I_ibmappnDirNotFoundBcastLocates:
	dp = (void *) (&data->ibmappnDirNotFoundBcastLocates);
	break;
#endif				       /* I_ibmappnDirNotFoundBcastLocates */

#ifdef I_ibmappnDirLocateOutstands
      case I_ibmappnDirLocateOutstands:
	dp = (void *) (&data->ibmappnDirLocateOutstands);
	break;
#endif				       /* I_ibmappnDirLocateOutstands */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnDirEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnDirEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnDirEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnDirLuName = NULL;
    int             ibmappnDirLuName_offset;
    int             index;
    int             final_index;

    ibmappnDirLuName_offset = object->oid.length;
    final_index = ibmappnDirLuName_offset + GetVariableIndexLength(incoming, ibmappnDirLuName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnDirLuName_offset, &ibmappnDirLuName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnDirEntry_get(serialNum, contextInfo, arg, searchType, ibmappnDirLuName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnDirLuName->length;
	for(i = 0; i < data->ibmappnDirLuName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnDirLuName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnDirLuName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnDirLuName
      case I_ibmappnDirLuName:
	dp = (void *) (CloneOctetString(data->ibmappnDirLuName));
	break;
#endif				       /* I_ibmappnDirLuName */

#ifdef I_ibmappnDirServerName
      case I_ibmappnDirServerName:
	dp = (void *) (CloneOctetString(data->ibmappnDirServerName));
	break;
#endif				       /* I_ibmappnDirServerName */

#ifdef I_ibmappnDirLuOwnerName
      case I_ibmappnDirLuOwnerName:
	dp = (void *) (CloneOctetString(data->ibmappnDirLuOwnerName));
	break;
#endif				       /* I_ibmappnDirLuOwnerName */

#ifdef I_ibmappnDirLuLocation
      case I_ibmappnDirLuLocation:
	dp = (void *) (&data->ibmappnDirLuLocation);
	break;
#endif				       /* I_ibmappnDirLuLocation */

#ifdef I_ibmappnDirType
      case I_ibmappnDirType:
	dp = (void *) (&data->ibmappnDirType);
	break;
#endif				       /* I_ibmappnDirType */

#ifdef I_ibmappnDirWildCard
      case I_ibmappnDirWildCard:
	dp = (void *) (&data->ibmappnDirWildCard);
	break;
#endif				       /* I_ibmappnDirWildCard */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnCosModeEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnCosModeEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnCosModeEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnCosModeName = NULL;
    int             ibmappnCosModeName_offset;
    int             index;
    int             final_index;

    ibmappnCosModeName_offset = object->oid.length;
    final_index = ibmappnCosModeName_offset + GetVariableIndexLength(incoming, ibmappnCosModeName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnCosModeName_offset, &ibmappnCosModeName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnCosModeEntry_get(serialNum, contextInfo, arg, searchType, ibmappnCosModeName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnCosModeName->length;
	for(i = 0; i < data->ibmappnCosModeName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnCosModeName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnCosModeName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnCosModeName
      case I_ibmappnCosModeName:
	dp = (void *) (CloneOctetString(data->ibmappnCosModeName));
	break;
#endif				       /* I_ibmappnCosModeName */

#ifdef I_ibmappnCosModeCosName
      case I_ibmappnCosModeCosName:
	dp = (void *) (CloneOctetString(data->ibmappnCosModeCosName));
	break;
#endif				       /* I_ibmappnCosModeCosName */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnCosNameEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnCosNameEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnCosNameEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnCosName = NULL;
    int             ibmappnCosName_offset;
    int             index;
    int             final_index;

    ibmappnCosName_offset = object->oid.length;
    final_index = ibmappnCosName_offset + GetVariableIndexLength(incoming, ibmappnCosName_offset);

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToVariableOctetString(incoming, ibmappnCosName_offset, &ibmappnCosName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnCosNameEntry_get(serialNum, contextInfo, arg, searchType, ibmappnCosName)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnCosName->length;
	for(i = 0; i < data->ibmappnCosName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnCosName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnCosName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnCosName
      case I_ibmappnCosName:
	dp = (void *) (CloneOctetString(data->ibmappnCosName));
	break;
#endif				       /* I_ibmappnCosName */

#ifdef I_ibmappnCosTransPriority
      case I_ibmappnCosTransPriority:
	dp = (void *) (&data->ibmappnCosTransPriority);
	break;
#endif				       /* I_ibmappnCosTransPriority */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnCosNodeRowEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnCosNodeRowEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnCosNodeRowEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnCosNodeRowName = NULL;
    int             ibmappnCosNodeRowName_offset;
    long            ibmappnCosNodeRowIndex;
    int             ibmappnCosNodeRowIndex_offset;
    int             index;
    int             final_index;

    ibmappnCosNodeRowName_offset = object->oid.length;
    ibmappnCosNodeRowIndex_offset = ibmappnCosNodeRowName_offset + GetVariableIndexLength(incoming, ibmappnCosNodeRowName_offset);
    final_index = ibmappnCosNodeRowIndex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToInt(incoming, ibmappnCosNodeRowIndex_offset, &ibmappnCosNodeRowIndex, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (searchType == NEXT) {
        ibmappnCosNodeRowIndex = MAX(0, ibmappnCosNodeRowIndex);
    }
    if ( (InstToVariableOctetString(incoming, ibmappnCosNodeRowName_offset, &ibmappnCosNodeRowName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnCosNodeRowEntry_get(serialNum, contextInfo, arg, searchType, ibmappnCosNodeRowName, ibmappnCosNodeRowIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnCosNodeRowName->length;
	for(i = 0; i < data->ibmappnCosNodeRowName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnCosNodeRowName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.oid_ptr[index++] = (unsigned long) data->ibmappnCosNodeRowIndex;
	inst.length = index;
    }

	FreeOctetString(ibmappnCosNodeRowName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnCosNodeRowName
      case I_ibmappnCosNodeRowName:
	dp = (void *) (CloneOctetString(data->ibmappnCosNodeRowName));
	break;
#endif				       /* I_ibmappnCosNodeRowName */

#ifdef I_ibmappnCosNodeRowIndex
      case I_ibmappnCosNodeRowIndex:
	dp = (void *) (&data->ibmappnCosNodeRowIndex);
	break;
#endif				       /* I_ibmappnCosNodeRowIndex */

#ifdef I_ibmappnCosNodeRowWgt
      case I_ibmappnCosNodeRowWgt:
	dp = (void *) (CloneOctetString(data->ibmappnCosNodeRowWgt));
	break;
#endif				       /* I_ibmappnCosNodeRowWgt */

#ifdef I_ibmappnCosNodeRowResistMin
      case I_ibmappnCosNodeRowResistMin:
	dp = (void *) (&data->ibmappnCosNodeRowResistMin);
	break;
#endif				       /* I_ibmappnCosNodeRowResistMin */

#ifdef I_ibmappnCosNodeRowResistMax
      case I_ibmappnCosNodeRowResistMax:
	dp = (void *) (&data->ibmappnCosNodeRowResistMax);
	break;
#endif				       /* I_ibmappnCosNodeRowResistMax */

#ifdef I_ibmappnCosNodeRowMinCongestAllow
      case I_ibmappnCosNodeRowMinCongestAllow:
	dp = (void *) (&data->ibmappnCosNodeRowMinCongestAllow);
	break;
#endif				       /* I_ibmappnCosNodeRowMinCongestAllow */

#ifdef I_ibmappnCosNodeRowMaxCongestAllow
      case I_ibmappnCosNodeRowMaxCongestAllow:
	dp = (void *) (&data->ibmappnCosNodeRowMaxCongestAllow);
	break;
#endif				       /* I_ibmappnCosNodeRowMaxCongestAllow */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnCosTgRowEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnCosTgRowEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnCosTgRowEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnCosTgRowName = NULL;
    int             ibmappnCosTgRowName_offset;
    long            ibmappnCosTgRowIndex;
    int             ibmappnCosTgRowIndex_offset;
    int             index;
    int             final_index;

    ibmappnCosTgRowName_offset = object->oid.length;
    ibmappnCosTgRowIndex_offset = ibmappnCosTgRowName_offset + GetVariableIndexLength(incoming, ibmappnCosTgRowName_offset);
    final_index = ibmappnCosTgRowIndex_offset + 1;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToInt(incoming, ibmappnCosTgRowIndex_offset, &ibmappnCosTgRowIndex, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (searchType == NEXT) {
        ibmappnCosTgRowIndex = MAX(0, ibmappnCosTgRowIndex);
    }
    if ( (InstToVariableOctetString(incoming, ibmappnCosTgRowName_offset, &ibmappnCosTgRowName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnCosTgRowEntry_get(serialNum, contextInfo, arg, searchType, ibmappnCosTgRowName, ibmappnCosTgRowIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnCosTgRowName->length;
	for(i = 0; i < data->ibmappnCosTgRowName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnCosTgRowName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.oid_ptr[index++] = (unsigned long) data->ibmappnCosTgRowIndex;
	inst.length = index;
    }

	FreeOctetString(ibmappnCosTgRowName);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnCosTgRowName
      case I_ibmappnCosTgRowName:
	dp = (void *) (CloneOctetString(data->ibmappnCosTgRowName));
	break;
#endif				       /* I_ibmappnCosTgRowName */

#ifdef I_ibmappnCosTgRowIndex
      case I_ibmappnCosTgRowIndex:
	dp = (void *) (&data->ibmappnCosTgRowIndex);
	break;
#endif				       /* I_ibmappnCosTgRowIndex */

#ifdef I_ibmappnCosTgRowWgt
      case I_ibmappnCosTgRowWgt:
	dp = (void *) (CloneOctetString(data->ibmappnCosTgRowWgt));
	break;
#endif				       /* I_ibmappnCosTgRowWgt */

#ifdef I_ibmappnCosTgRowEffCapMin
      case I_ibmappnCosTgRowEffCapMin:
	dp = (void *) (&data->ibmappnCosTgRowEffCapMin);
	break;
#endif				       /* I_ibmappnCosTgRowEffCapMin */

#ifdef I_ibmappnCosTgRowEffCapMax
      case I_ibmappnCosTgRowEffCapMax:
	dp = (void *) (&data->ibmappnCosTgRowEffCapMax);
	break;
#endif				       /* I_ibmappnCosTgRowEffCapMax */

#ifdef I_ibmappnCosTgRowConnCostMin
      case I_ibmappnCosTgRowConnCostMin:
	dp = (void *) (&data->ibmappnCosTgRowConnCostMin);
	break;
#endif				       /* I_ibmappnCosTgRowConnCostMin */

#ifdef I_ibmappnCosTgRowConnCostMax
      case I_ibmappnCosTgRowConnCostMax:
	dp = (void *) (&data->ibmappnCosTgRowConnCostMax);
	break;
#endif				       /* I_ibmappnCosTgRowConnCostMax */

#ifdef I_ibmappnCosTgRowByteCostMin
      case I_ibmappnCosTgRowByteCostMin:
	dp = (void *) (&data->ibmappnCosTgRowByteCostMin);
	break;
#endif				       /* I_ibmappnCosTgRowByteCostMin */

#ifdef I_ibmappnCosTgRowByteCostMax
      case I_ibmappnCosTgRowByteCostMax:
	dp = (void *) (&data->ibmappnCosTgRowByteCostMax);
	break;
#endif				       /* I_ibmappnCosTgRowByteCostMax */

#ifdef I_ibmappnCosTgRowSecurityMin
      case I_ibmappnCosTgRowSecurityMin:
	dp = (void *) (&data->ibmappnCosTgRowSecurityMin);
	break;
#endif				       /* I_ibmappnCosTgRowSecurityMin */

#ifdef I_ibmappnCosTgRowSecurityMax
      case I_ibmappnCosTgRowSecurityMax:
	dp = (void *) (&data->ibmappnCosTgRowSecurityMax);
	break;
#endif				       /* I_ibmappnCosTgRowSecurityMax */

#ifdef I_ibmappnCosTgRowDelayMin
      case I_ibmappnCosTgRowDelayMin:
	dp = (void *) (&data->ibmappnCosTgRowDelayMin);
	break;
#endif				       /* I_ibmappnCosTgRowDelayMin */

#ifdef I_ibmappnCosTgRowDelayMax
      case I_ibmappnCosTgRowDelayMax:
	dp = (void *) (&data->ibmappnCosTgRowDelayMax);
	break;
#endif				       /* I_ibmappnCosTgRowDelayMax */

#ifdef I_ibmappnCosTgRowUsr1Min
      case I_ibmappnCosTgRowUsr1Min:
	dp = (void *) (&data->ibmappnCosTgRowUsr1Min);
	break;
#endif				       /* I_ibmappnCosTgRowUsr1Min */

#ifdef I_ibmappnCosTgRowUsr1Max
      case I_ibmappnCosTgRowUsr1Max:
	dp = (void *) (&data->ibmappnCosTgRowUsr1Max);
	break;
#endif				       /* I_ibmappnCosTgRowUsr1Max */

#ifdef I_ibmappnCosTgRowUsr2Min
      case I_ibmappnCosTgRowUsr2Min:
	dp = (void *) (&data->ibmappnCosTgRowUsr2Min);
	break;
#endif				       /* I_ibmappnCosTgRowUsr2Min */

#ifdef I_ibmappnCosTgRowUsr2Max
      case I_ibmappnCosTgRowUsr2Max:
	dp = (void *) (&data->ibmappnCosTgRowUsr2Max);
	break;
#endif				       /* I_ibmappnCosTgRowUsr2Max */

#ifdef I_ibmappnCosTgRowUsr3Min
      case I_ibmappnCosTgRowUsr3Min:
	dp = (void *) (&data->ibmappnCosTgRowUsr3Min);
	break;
#endif				       /* I_ibmappnCosTgRowUsr3Min */

#ifdef I_ibmappnCosTgRowUsr3Max
      case I_ibmappnCosTgRowUsr3Max:
	dp = (void *) (&data->ibmappnCosTgRowUsr3Max);
	break;
#endif				       /* I_ibmappnCosTgRowUsr3Max */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnIsInGlobal family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnIsInGlobal_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg;
    void           *dp;
    ibmappnIsInGlobal_t *data = NULL;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnIsInGlobal_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnIsInGlobeStatus
      case I_ibmappnIsInGlobeStatus:
	dp = (void *) (&data->ibmappnIsInGlobeStatus);
	break;
#endif				       /* I_ibmappnIsInGlobeStatus */

#ifdef I_ibmappnIsInGlobeRscv
      case I_ibmappnIsInGlobeRscv:
	dp = (void *) (&data->ibmappnIsInGlobeRscv);
	break;
#endif				       /* I_ibmappnIsInGlobeRscv */

#ifdef I_ibmappnIsInGlobeRscvTime
      case I_ibmappnIsInGlobeRscvTime:
	dp = (void *) (&data->ibmappnIsInGlobeRscvTime);
	break;
#endif				       /* I_ibmappnIsInGlobeRscvTime */

#ifdef I_ibmappnIsInGlobeCtrStatus
      case I_ibmappnIsInGlobeCtrStatus:
	dp = (void *) (&data->ibmappnIsInGlobeCtrStatus);
	break;
#endif				       /* I_ibmappnIsInGlobeCtrStatus */

#ifdef I_ibmappnIsInGlobeCtrStatusTime
      case I_ibmappnIsInGlobeCtrStatusTime:
	dp = (void *) (&data->ibmappnIsInGlobeCtrStatusTime);
	break;
#endif				       /* I_ibmappnIsInGlobeCtrStatusTime */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the ibmappnIsInEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
ibmappnIsInEntry_get(incoming, object, searchType, contextInfo, serialNum)
    OID            *incoming;
    ObjectInfo     *object;
    int             searchType;
    ContextInfo    *contextInfo;
    int             serialNum;
{
    int             arg = object->nominator;
    void           *dp;
    ibmappnIsInEntry_t *data = NULL;
    unsigned long   buffer[MAX_OID_SIZE];
    OID             inst;
    int             carry;
    int             i;
    OctetString *   ibmappnIsInFqLuName = NULL;
    int             ibmappnIsInFqLuName_offset;
    OctetString *   ibmappnIsInPcid = NULL;
    int             ibmappnIsInPcid_offset;
    int             index;
    int             final_index;

    ibmappnIsInFqLuName_offset = object->oid.length;
    ibmappnIsInPcid_offset = ibmappnIsInFqLuName_offset + GetVariableIndexLength(incoming, ibmappnIsInFqLuName_offset);
    final_index = ibmappnIsInPcid_offset + 8;

    if (searchType == EXACT) {
       if (final_index != incoming->length) {
	    return((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ( (InstToFixedOctetString(incoming, ibmappnIsInPcid_offset, &ibmappnIsInPcid, searchType, &carry, 8)) < 0 ) {
	arg = -1;
    }
    if ( (InstToVariableOctetString(incoming, ibmappnIsInFqLuName_offset, &ibmappnIsInFqLuName, searchType, &carry)) < 0 ) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_ibmappnIsInEntry_get(serialNum, contextInfo, arg, searchType, ibmappnIsInFqLuName, ibmappnIsInPcid)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	index = 0;
        inst.oid_ptr[index++] = data->ibmappnIsInFqLuName->length;
	for(i = 0; i < data->ibmappnIsInFqLuName->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnIsInFqLuName->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	for(i = 0; i < data->ibmappnIsInPcid->length; i++) {
	    inst.oid_ptr[index++] = (unsigned long) data->ibmappnIsInPcid->octet_ptr[i];
            if (index > MAX_OID_SIZE) {
                arg = -1;
                index--;
            }
	}

	inst.length = index;
    }

	FreeOctetString(ibmappnIsInFqLuName);
	FreeOctetString(ibmappnIsInPcid);
    /*
     * Build the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_ibmappnIsInFqLuName
      case I_ibmappnIsInFqLuName:
	dp = (void *) (CloneOctetString(data->ibmappnIsInFqLuName));
	break;
#endif				       /* I_ibmappnIsInFqLuName */

#ifdef I_ibmappnIsInPcid
      case I_ibmappnIsInPcid:
	dp = (void *) (CloneOctetString(data->ibmappnIsInPcid));
	break;
#endif				       /* I_ibmappnIsInPcid */

#ifdef I_ibmappnIsInPriLuName
      case I_ibmappnIsInPriLuName:
	dp = (void *) (CloneOctetString(data->ibmappnIsInPriLuName));
	break;
#endif				       /* I_ibmappnIsInPriLuName */

#ifdef I_ibmappnIsInSecLuName
      case I_ibmappnIsInSecLuName:
	dp = (void *) (CloneOctetString(data->ibmappnIsInSecLuName));
	break;
#endif				       /* I_ibmappnIsInSecLuName */

#ifdef I_ibmappnIsInModeName
      case I_ibmappnIsInModeName:
	dp = (void *) (CloneOctetString(data->ibmappnIsInModeName));
	break;
#endif				       /* I_ibmappnIsInModeName */

#ifdef I_ibmappnIsInCosName
      case I_ibmappnIsInCosName:
	dp = (void *) (CloneOctetString(data->ibmappnIsInCosName));
	break;
#endif				       /* I_ibmappnIsInCosName */

#ifdef I_ibmappnIsInTransPriority
      case I_ibmappnIsInTransPriority:
	dp = (void *) (&data->ibmappnIsInTransPriority);
	break;
#endif				       /* I_ibmappnIsInTransPriority */

#ifdef I_ibmappnIsInSessType
      case I_ibmappnIsInSessType:
	dp = (void *) (&data->ibmappnIsInSessType);
	break;
#endif				       /* I_ibmappnIsInSessType */

#ifdef I_ibmappnIsInSessState
      case I_ibmappnIsInSessState:
	dp = (void *) (&data->ibmappnIsInSessState);
	break;
#endif				       /* I_ibmappnIsInSessState */

#ifdef I_ibmappnIsInSessStartTime
      case I_ibmappnIsInSessStartTime:
	dp = (void *) (&data->ibmappnIsInSessStartTime);
	break;
#endif				       /* I_ibmappnIsInSessStartTime */

#ifdef I_ibmappnIsInSessUpTime
      case I_ibmappnIsInSessUpTime:
	dp = (void *) (&data->ibmappnIsInSessUpTime);
	break;
#endif				       /* I_ibmappnIsInSessUpTime */

#ifdef I_ibmappnIsInCtrUpTime
      case I_ibmappnIsInCtrUpTime:
	dp = (void *) (&data->ibmappnIsInCtrUpTime);
	break;
#endif				       /* I_ibmappnIsInCtrUpTime */

#ifdef I_ibmappnIsInP2SFmdPius
      case I_ibmappnIsInP2SFmdPius:
	dp = (void *) (&data->ibmappnIsInP2SFmdPius);
	break;
#endif				       /* I_ibmappnIsInP2SFmdPius */

#ifdef I_ibmappnIsInS2PFmdPius
      case I_ibmappnIsInS2PFmdPius:
	dp = (void *) (&data->ibmappnIsInS2PFmdPius);
	break;
#endif				       /* I_ibmappnIsInS2PFmdPius */

#ifdef I_ibmappnIsInP2SNonFmdPius
      case I_ibmappnIsInP2SNonFmdPius:
	dp = (void *) (&data->ibmappnIsInP2SNonFmdPius);
	break;
#endif				       /* I_ibmappnIsInP2SNonFmdPius */

#ifdef I_ibmappnIsInS2PNonFmdPius
      case I_ibmappnIsInS2PNonFmdPius:
	dp = (void *) (&data->ibmappnIsInS2PNonFmdPius);
	break;
#endif				       /* I_ibmappnIsInS2PNonFmdPius */

#ifdef I_ibmappnIsInP2SFmdBytes
      case I_ibmappnIsInP2SFmdBytes:
	dp = (void *) (&data->ibmappnIsInP2SFmdBytes);
	break;
#endif				       /* I_ibmappnIsInP2SFmdBytes */

#ifdef I_ibmappnIsInS2PFmdBytes
      case I_ibmappnIsInS2PFmdBytes:
	dp = (void *) (&data->ibmappnIsInS2PFmdBytes);
	break;
#endif				       /* I_ibmappnIsInS2PFmdBytes */

#ifdef I_ibmappnIsInP2SNonFmdBytes
      case I_ibmappnIsInP2SNonFmdBytes:
	dp = (void *) (&data->ibmappnIsInP2SNonFmdBytes);
	break;
#endif				       /* I_ibmappnIsInP2SNonFmdBytes */

#ifdef I_ibmappnIsInS2PNonFmdBytes
      case I_ibmappnIsInS2PNonFmdBytes:
	dp = (void *) (&data->ibmappnIsInS2PNonFmdBytes);
	break;
#endif				       /* I_ibmappnIsInS2PNonFmdBytes */

#ifdef I_ibmappnIsInPsAdjCpName
      case I_ibmappnIsInPsAdjCpName:
	dp = (void *) (CloneOctetString(data->ibmappnIsInPsAdjCpName));
	break;
#endif				       /* I_ibmappnIsInPsAdjCpName */

#ifdef I_ibmappnIsInPsAdjTgNum
      case I_ibmappnIsInPsAdjTgNum:
	dp = (void *) (&data->ibmappnIsInPsAdjTgNum);
	break;
#endif				       /* I_ibmappnIsInPsAdjTgNum */

#ifdef I_ibmappnIsInPsSendMaxBtuSize
      case I_ibmappnIsInPsSendMaxBtuSize:
	dp = (void *) (&data->ibmappnIsInPsSendMaxBtuSize);
	break;
#endif				       /* I_ibmappnIsInPsSendMaxBtuSize */

#ifdef I_ibmappnIsInPsSendPacingType
      case I_ibmappnIsInPsSendPacingType:
	dp = (void *) (&data->ibmappnIsInPsSendPacingType);
	break;
#endif				       /* I_ibmappnIsInPsSendPacingType */

#ifdef I_ibmappnIsInPsSendRpc
      case I_ibmappnIsInPsSendRpc:
	dp = (void *) (&data->ibmappnIsInPsSendRpc);
	break;
#endif				       /* I_ibmappnIsInPsSendRpc */

#ifdef I_ibmappnIsInPsSendNxWndwSize
      case I_ibmappnIsInPsSendNxWndwSize:
	dp = (void *) (&data->ibmappnIsInPsSendNxWndwSize);
	break;
#endif				       /* I_ibmappnIsInPsSendNxWndwSize */

#ifdef I_ibmappnIsInPsRecvPacingType
      case I_ibmappnIsInPsRecvPacingType:
	dp = (void *) (&data->ibmappnIsInPsRecvPacingType);
	break;
#endif				       /* I_ibmappnIsInPsRecvPacingType */

#ifdef I_ibmappnIsInPsRecvRpc
      case I_ibmappnIsInPsRecvRpc:
	dp = (void *) (&data->ibmappnIsInPsRecvRpc);
	break;
#endif				       /* I_ibmappnIsInPsRecvRpc */

#ifdef I_ibmappnIsInPsRecvNxWndwSize
      case I_ibmappnIsInPsRecvNxWndwSize:
	dp = (void *) (&data->ibmappnIsInPsRecvNxWndwSize);
	break;
#endif				       /* I_ibmappnIsInPsRecvNxWndwSize */

#ifdef I_ibmappnIsInSsAdjCpName
      case I_ibmappnIsInSsAdjCpName:
	dp = (void *) (CloneOctetString(data->ibmappnIsInSsAdjCpName));
	break;
#endif				       /* I_ibmappnIsInSsAdjCpName */

#ifdef I_ibmappnIsInSsAdjTgNum
      case I_ibmappnIsInSsAdjTgNum:
	dp = (void *) (&data->ibmappnIsInSsAdjTgNum);
	break;
#endif				       /* I_ibmappnIsInSsAdjTgNum */

#ifdef I_ibmappnIsInSsSendMaxBtuSize
      case I_ibmappnIsInSsSendMaxBtuSize:
	dp = (void *) (&data->ibmappnIsInSsSendMaxBtuSize);
	break;
#endif				       /* I_ibmappnIsInSsSendMaxBtuSize */

#ifdef I_ibmappnIsInSsSendPacingType
      case I_ibmappnIsInSsSendPacingType:
	dp = (void *) (&data->ibmappnIsInSsSendPacingType);
	break;
#endif				       /* I_ibmappnIsInSsSendPacingType */

#ifdef I_ibmappnIsInSsSendRpc
      case I_ibmappnIsInSsSendRpc:
	dp = (void *) (&data->ibmappnIsInSsSendRpc);
	break;
#endif				       /* I_ibmappnIsInSsSendRpc */

#ifdef I_ibmappnIsInSsSendNxWndwSize
      case I_ibmappnIsInSsSendNxWndwSize:
	dp = (void *) (&data->ibmappnIsInSsSendNxWndwSize);
	break;
#endif				       /* I_ibmappnIsInSsSendNxWndwSize */

#ifdef I_ibmappnIsInSsRecvPacingType
      case I_ibmappnIsInSsRecvPacingType:
	dp = (void *) (&data->ibmappnIsInSsRecvPacingType);
	break;
#endif				       /* I_ibmappnIsInSsRecvPacingType */

#ifdef I_ibmappnIsInSsRecvRpc
      case I_ibmappnIsInSsRecvRpc:
	dp = (void *) (&data->ibmappnIsInSsRecvRpc);
	break;
#endif				       /* I_ibmappnIsInSsRecvRpc */

#ifdef I_ibmappnIsInSsRecvNxWndwSize
      case I_ibmappnIsInSsRecvNxWndwSize:
	dp = (void *) (&data->ibmappnIsInSsRecvNxWndwSize);
	break;
#endif				       /* I_ibmappnIsInSsRecvNxWndwSize */

#ifdef I_ibmappnIsInRouteInfo
      case I_ibmappnIsInRouteInfo:
	dp = (void *) (CloneOctetString(data->ibmappnIsInRouteInfo));
	break;
#endif				       /* I_ibmappnIsInRouteInfo */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

AppnFunc* appnRP = NULL;
AppnFunc* appnCip = NULL;


ibmappnGeneralInfoAndCaps_t *
  k_ibmappnGeneralInfoAndCaps_get (serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return(func->t_ibmappnGeneralInfoAndCaps_get (serialNum, contextInfo, nominator));
}

ibmappnNnUniqueInfoAndCaps_t *
k_ibmappnNnUniqueInfoAndCaps_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return(func->t_ibmappnNnUniqueInfoAndCaps_get(serialNum, contextInfo, nominator));
}

ibmappnEnUniqueCaps_t *
k_ibmappnEnUniqueCaps_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return(func->t_ibmappnEnUniqueCaps_get(serialNum, contextInfo, nominator));
}

ibmappnNodePortEntry_t *
k_ibmappnNodePortEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodePortName;


{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodePortEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortName));
}

ibmappnNodePortIpEntry_t *
k_ibmappnNodePortIpEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortIpName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodePortIpName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodePortIpEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortIpName));
}

ibmappnNodePortDlsEntry_t *
k_ibmappnNodePortDlsEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortDlsName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodePortDlsName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodePortDlsEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortDlsName));
}

ibmappnNodePortTrEntry_t *
k_ibmappnNodePortTrEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortTrName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodePortTrName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodePortTrEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortTrName));
}

ibmappnNodePortDlcTraceEntry_t *
k_ibmappnNodePortDlcTraceEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortDlcTracPortName, ibmappnNodePortDlcTracIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodePortDlcTracPortName;
    long            ibmappnNodePortDlcTracIndex;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodePortDlcTraceEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodePortDlcTracPortName, ibmappnNodePortDlcTracIndex));
}

ibmappnNodeLsEntry_t *
k_ibmappnNodeLsEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodeLsName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodeLsEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsName));
}

ibmappnNodeLsIpEntry_t *
k_ibmappnNodeLsIpEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsIpName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodeLsIpName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodeLsIpEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsIpName));
}

ibmappnNodeLsDlsEntry_t *
k_ibmappnNodeLsDlsEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsDlsName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodeLsDlsName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodeLsDlsEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsDlsName));
}

ibmappnNodeLsTrEntry_t *
k_ibmappnNodeLsTrEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsTrName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNodeLsTrName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodeLsTrEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsTrName));
}

ibmappnNodeLsStatusEntry_t *
k_ibmappnNodeLsStatusEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsStatusIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            ibmappnNodeLsStatusIndex;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNodeLsStatusEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNodeLsStatusIndex));
}

ibmappnSnmpInformation_t *
k_ibmappnSnmpInformation_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnSnmpInformation_get(serialNum, contextInfo, nominator));
}

ibmappnMemoryUse_t *
k_ibmappnMemoryUse_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
   return (func->t_ibmappnMemoryUse_get(serialNum, contextInfo, nominator));
}


ibmappnXidInformation_t *
k_ibmappnXidInformation_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnXidInformation_get(serialNum, contextInfo, nominator));
}

ibmappnNnTopo_t *
k_ibmappnNnTopo_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;

{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNnTopo_get(serialNum, contextInfo, nominator));
}

ibmappnNnTopoRouteEntry_t *
k_ibmappnNnTopoRouteEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnTopoRouteCos)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNnTopoRouteCos;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNnTopoRouteEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnTopoRouteCos));
}

ibmappnNnAdjNodeEntry_t *
k_ibmappnNnAdjNodeEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnAdjNodeAdjName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNnAdjNodeAdjName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNnAdjNodeEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnAdjNodeAdjName));
}


ibmappnNnTopologyEntry_t *
k_ibmappnNnTopologyEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnNodeName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNnNodeName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNnTopologyEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnNodeName));
}

ibmappnNnTgTopologyEntry_t *
k_ibmappnNnTgTopologyEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnTgOwner, ibmappnNnTgDest, ibmappnNnTgNum)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnNnTgOwner;
    OctetString *   ibmappnNnTgDest;
    long            ibmappnNnTgNum;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNnTgTopologyEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnTgOwner, ibmappnNnTgDest, ibmappnNnTgNum));
}

ibmappnNnTopologyFREntry_t *
k_ibmappnNnTopologyFREntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnNodeFRFrsn, ibmappnNnNodeFRName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            ibmappnNnNodeFRFrsn;
    OctetString *   ibmappnNnNodeFRName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNnTopologyFREntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnNodeFRFrsn, ibmappnNnNodeFRName));
}

ibmappnNnTgTopologyFREntry_t *
k_ibmappnNnTgTopologyFREntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnTgFRFrsn, ibmappnNnTgFROwner, ibmappnNnTgFRDest, ibmappnNnTgFRNum)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    long            ibmappnNnTgFRFrsn;
    OctetString *   ibmappnNnTgFROwner;
    OctetString *   ibmappnNnTgFRDest;
    long            ibmappnNnTgFRNum;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnNnTgTopologyFREntry_get(serialNum, contextInfo, nominator, searchType, ibmappnNnTgFRFrsn, ibmappnNnTgFROwner, ibmappnNnTgFRDest, ibmappnNnTgFRNum));
}

ibmappnLocalGeneral_t *
k_ibmappnLocalGeneral_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnLocalGeneral_get(serialNum, contextInfo, nominator));
}

ibmappnLocalNnSpecific_t *
k_ibmappnLocalNnSpecific_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnLocalNnSpecific_get(serialNum, contextInfo, nominator));
}

ibmappnLocalTgEntry_t *
k_ibmappnLocalTgEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnLocalTgDest, ibmappnLocalTgNum)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnLocalTgDest;
    long            ibmappnLocalTgNum;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnLocalTgEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnLocalTgDest, ibmappnLocalTgNum));
}

ibmappnLocalEnEntry_t *
k_ibmappnLocalEnEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnLocalEnName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnLocalEnName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnLocalEnEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnLocalEnName));
}

ibmappnLocalEnTgEntry_t *
k_ibmappnLocalEnTgEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnLocalEnTgOrigin, ibmappnLocalEnTgDest, ibmappnLocalEnTgNum)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnLocalEnTgOrigin;
    OctetString *   ibmappnLocalEnTgDest;
    long            ibmappnLocalEnTgNum;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnLocalEnTgEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnLocalEnTgOrigin, ibmappnLocalEnTgDest, ibmappnLocalEnTgNum));
}

ibmappnDirPerf_t *
k_ibmappnDirPerf_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnDirPerf_get(serialNum, contextInfo, nominator));
}

ibmappnDirEntry_t *
k_ibmappnDirEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnDirLuName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnDirLuName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnDirEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnDirLuName));
}

ibmappnCosModeEntry_t *
k_ibmappnCosModeEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosModeName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnCosModeName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnCosModeEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosModeName));
}

ibmappnCosNameEntry_t *
k_ibmappnCosNameEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosName)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnCosName;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnCosNameEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosName));
}

ibmappnCosNodeRowEntry_t *
k_ibmappnCosNodeRowEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosNodeRowName, ibmappnCosNodeRowIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnCosNodeRowName;
    long            ibmappnCosNodeRowIndex;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnCosNodeRowEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosNodeRowName, ibmappnCosNodeRowIndex));
}

ibmappnCosTgRowEntry_t *
k_ibmappnCosTgRowEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosTgRowName, ibmappnCosTgRowIndex)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnCosTgRowName;
    long            ibmappnCosTgRowIndex;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnCosTgRowEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnCosTgRowName, ibmappnCosTgRowIndex));
}

ibmappnIsInGlobal_t *
k_ibmappnIsInGlobal_get(serialNum, contextInfo, nominator)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnIsInGlobal_get(serialNum, contextInfo, nominator));
}

ibmappnIsInEntry_t *
k_ibmappnIsInEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnIsInFqLuName, ibmappnIsInPcid)
    int             serialNum;
    ContextInfo    *contextInfo;
    int             nominator;
    int             searchType;
    OctetString *   ibmappnIsInFqLuName;
    OctetString *   ibmappnIsInPcid;
{
    AppnFunc* func;
    func = snmp_extract_logical_entity(contextInfo) ? appnCip : appnRP;
    if (func == NULL){
       return NULL;
    }
    return (func->t_ibmappnIsInEntry_get(serialNum, contextInfo, nominator, searchType, ibmappnIsInFqLuName, ibmappnIsInPcid));
}

void
init_ibmappnmib (subsystype *subsys)
{
    load_mib(ibmappnmib_OidList, ibmappnmib_OidListNum);
    load_oid(ibmappnmib_oid_table);
}


/*
 * ibmappnmib subsystem header
 */


#define MAJVERSION_ibmappnmib 1
#define MINVERSION_ibmappnmib 0
#define EDITVERSION_ibmappnmib 0

SUBSYS_HEADER(ibmappnmib,
              MAJVERSION_ibmappnmib,
              MINVERSION_ibmappnmib,
              EDITVERSION_ibmappnmib,
              init_ibmappnmib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
