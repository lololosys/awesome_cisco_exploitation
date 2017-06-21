/* $Id: sr_ilmimib.c,v 3.9.8.10 1996/08/05 17:26:06 bbenson Exp $
 * $Source: /release/112/cvs/Xsys/atm/sr_ilmimib.c,v $
 *------------------------------------------------------------------
 * Placeholder for ILMI support
 *
 * March, 1995   Kartik Chandrasekhar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_ilmimib.c,v $
 * Revision 3.9.8.10  1996/08/05  17:26:06  bbenson
 * CSCdi59897:  Watchdog bites snmp due to lec interface lookup delay
 * Branch: California_branch
 * Typo in last commit.
 *
 * Revision 3.9.8.9  1996/08/05  17:17:27  bbenson
 * CSCdi59897:  Watchdog bites snmp due to lec interface lookup delay
 * Branch: California_branch
 * Remove for loop over all VCs using blocking call to getvc_stats.
 *
 * Revision 3.9.8.8  1996/07/23  23:01:39  kartik
 * CSCdi63824:  no ip-address does not reset ILMI MIB object
 * atmfMyIpNmAddress.
 * Branch: California_branch
 *
 * Revision 3.9.8.7  1996/07/01  18:44:03  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.9.8.6  1996/06/12  01:02:07  sdurham
 * CSCdi58631:  agent returned badValue/wrong error-index when setting
 * ifAdminStatus
 * Branch: California_branch - allow set back to original value for
 * aal5/atm layers
 *
 * Revision 3.9.8.5  1996/06/06  00:16:10  sdurham
 * CSCdi58097:  spurious accesses made at k_getIfEntry
 * Branch: California_branch.  Missing callback rountines are added
 *
 * Revision 3.9.8.4  1996/05/21  06:21:22  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.9.8.3  1996/05/09  14:11:38  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.9.8.2  1996/04/30  23:14:53  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.9.8.1.2.1  1996/04/27  06:34:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.9.8.1  1996/04/25  23:03:03  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.9.26.1  1996/04/10  23:35:57  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.9  1996/02/27  17:48:07  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.8  1996/02/01  05:59:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.7  1996/01/24  22:07:30  anke
 * CSCdi47169:  Remove snmp from mibs as a required subsystem
 *
 * Revision 3.6  1996/01/18  15:43:13  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.5  1995/12/14  08:22:42  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.4  1995/12/06  10:04:41  kartik
 * CSCdi44943:  ILMI low memory access error msg on 7k
 *
 * Revision 3.3  1995/11/23  19:56:51  bbenson
 * CSCdi42518:  vc creation fails because of wrong max_vpibits in ILMI
 * Fixes bits per VPI/VCI reported by the ILMI.
 *
 * Revision 3.2  1995/11/17  08:47:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/21  18:04:10  kartik
 * CSCdi37022:  ilmimib causes router crash on a mibwalk.
 *
 * Revision 2.3  1995/07/11  01:18:16  kartik
 * CSCdi36894:  ILMI should return the value zero for port instance in
 * response.
 *
 * Revision 2.2  1995/06/28  09:22:24  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:12:47  hampton
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
#include "../snmp/snmp_registry.h"
#include "sr_ilmimib.h"
#include "sr_ilmimib-mib.h"
#include "address.h"
#include "interface_private.h"
#include "../if/atm.h"
#include "parser_defs_atm.h"	/* bogus, but atm.h references it */
#include "atm_ilmi.h"
#include "ilmi_public.h"
#include "ilmi_private.h"
#include "ilmi_encode.h"
#include "../snmp/ifmib_registry.h"
#include "../if/network.h"
#include "packet.h"
#include "../wan/dialer_registry.h"

static long portSpecOID[] = {0,0};

static ifmib_err_t atm_subif_cntr32fn(
	  ifmib_cntr_t	ifCntr,
	  const	subiabtype *subiab,
          ulong		*retval,
	  int serialNum );

static ifmib_err_t aal5_subif_cntr32fn(
	  ifmib_cntr_t	ifCntr,
	  const	subiabtype *subiab,
          ulong		*retval,
	  int serialNum );


static boolean atm_admin_change(
	  enum SNMP_ADMIN_STATE	newStatus,
	  boolean		testPhase,
	  const	subiabtype      *subiab);

static void atm_add_subif(idbtype *idb);
static void atm_delete_subif(idbtype *idb);
static void atm_cleanup_subif(subiabtype *subiab);
static enum SNMP_ADMIN_STATE atm_get_adminstatus(const subiabtype *subiab);
static enum SNMP_ADMIN_STATE atm_get_operstatus(const subiabtype *subiab);


void
init_ilmimib (subsystype *subsys)
{
    load_mib(ilmimib_OidList, ilmimib_OidListNum);
    /*
     * register to process counters for atm subinterfaces for ifmib.
     */

    reg_add_ifmib_cntr32(D_ifType_atm,atm_subif_cntr32fn,"atm_subif_cntr32fn");
    reg_add_ifmib_cntr32(D_ifType_aal5,aal5_subif_cntr32fn,"aal5_subif_cntr32fn");
    reg_add_ifmib_admin_change(D_ifType_atm,atm_admin_change,"atm_admin_change");
    reg_add_ifmib_admin_change(D_ifType_aal5,atm_admin_change,"atm_admin_change");
    reg_add_ifmib_add_subif(ET_ATM,atm_add_subif,"atm_add_subif");
    reg_add_ifmib_delete_subif(ET_ATM,atm_delete_subif,"atm_delete_subif");
    reg_add_ifmib_get_operstatus(D_ifType_atm,atm_get_operstatus,"atm_get_operstatus");
    reg_add_ifmib_get_operstatus(D_ifType_aal5,atm_get_operstatus,"atm_get_operstatus");
    reg_add_ifmib_get_adminstatus(D_ifType_atm,atm_get_adminstatus,"atm_get_adminstatus");
    reg_add_ifmib_get_adminstatus(D_ifType_aal5,atm_get_adminstatus,"atm_get_adminstatus");
    load_oid(ilmimib_oid_table);
}


/*---------------------------------------------------------------------
 * Retrieve data from the atmfPhysicalGroup family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
atmfPhysicalGroup_get(incoming, object, searchType, contextInfo, serialNum)
    OID             *incoming;
    ObjectInfo      *object;
    int             searchType;
    ContextInfo     *contextInfo;
    int             serialNum;
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    atmfPhysicalGroup_t *data;


	data = NULL;

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
	/*
	 * Get next on a port index is disallowed for ILMI.
	 */
	return (NULL);
        break;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
     if ( (arg == -1) || (data = k_atmfPhysicalGroup_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
     }

     /*
      * Build the variable binding for the variable that will be returned.
      */

     switch (arg) {

#ifdef I_atmfMyIpNmAddress
    case I_atmfMyIpNmAddress:
       dp = (void *) (IPToOctetString(data->atmfMyIpNmAddress));
       break;
#endif /* I_atmfMyIpNmAddress */

#ifdef I_atmfMyOsiNmNsapAddress
/* 
 *  Not Supported 
 */
    case I_atmfMyOsiNmNsapAddress:
       dp = (void *) (CloneOctetString(data->atmfMyOsiNmNsapAddress));
       break;
#endif /* I_atmfMyOsiNmNsapAddress */

    default:
       return ((VarBind *) NULL);

     }      /* switch */
     return (MakeVarBind(object, &ZeroOid, dp));
}




/*---------------------------------------------------------------------
 * Retrieve data from the atmfPortEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
atmfPortEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    atmfPortEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            atmfPortIndex;
	addrtype		*intf;


    data = NULL;		/* make compiler shut up */
    
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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	return (NULL);
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &atmfPortIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

	/*
	 * If the index value is zero then get the local interface number that
	 * is the one on which the SNMP pkt came in, and assign it to 
	 * atmfPortIndex.
	 */

	 if(!atmfPortIndex) {
     	if(!(intf = reg_invoke_snmp_packet_source()))
			return ((VarBind *) NULL);
     	atmfPortIndex = intf->addr.ilmi_address;
	 }

    /*
     * Retrieve the data from the kernel-specific routine.
     */

    if ((arg == -1) || (data = k_atmfPortEntry_get(serialNum, contextInfo, arg, searchType, atmfPortIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = 0;  /* ILMI interface instance is always 0 */
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_atmfPortIndex
      case I_atmfPortIndex:
	dp = &data->atmfPortIndex;
	break;
#endif				       /* I_atmfPortIndex */

#ifdef I_atmfPortAddress
      case I_atmfPortAddress:
	dp = MakeOctetString(data->atmfPortAddress->octet_ptr, data->atmfPortAddress->length);
	break;
#endif				       /* I_atmfPortAddress */

#ifdef I_atmfPortTransmissionType
      case I_atmfPortTransmissionType:
	dp = MakeOID(data->atmfPortTransmissionType->oid_ptr, data->atmfPortTransmissionType->length);
	break;
#endif				       /* I_atmfPortTransmissionType */

#ifdef I_atmfPortMediaType
      case I_atmfPortMediaType:
	dp = MakeOID(data->atmfPortMediaType->oid_ptr, data->atmfPortMediaType->length);
	break;
#endif				       /* I_atmfPortMediaType */

#ifdef I_atmfPortOperStatus
      case I_atmfPortOperStatus:
	dp = &data->atmfPortOperStatus;
	break;
#endif				       /* I_atmfPortOperStatus */

#ifdef I_atmfPortSpecific
      case I_atmfPortSpecific:
	dp = MakeOID(data->atmfPortSpecific->oid_ptr, data->atmfPortSpecific->length);
	break;
#endif				       /* I_atmfPortSpecific */

#ifdef I_atmfPortMyIfName
	  case I_atmfPortMyIfName:
   dp = (void *) (CloneOctetString(data->atmfPortMyIfName));
  break;
#endif /* I_atmfPortMyIfName */


      default:
	return ((VarBind *) NULL);

    }				       /* switch */
    return (MakeVarBind(object, &inst, dp));
}



/*---------------------------------------------------------------------
 * Retrieve data from the atmfAtmLayerEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
atmfAtmLayerEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    atmfAtmLayerEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            atmfAtmLayerIndex;
	addrtype		*intf;


    data = NULL;		/* make compiler shut up */

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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	return (NULL);
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &atmfAtmLayerIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

	/*
	 * If the index value is zero then get the local interface number that
	 * is the one on which the SNMP pkt came in, and assign it to 
	 * atmfPortIndex.
	 */

	 if(!atmfAtmLayerIndex) {
     	if(!(intf = reg_invoke_snmp_packet_source()))
			return ((VarBind *) NULL);
     	atmfAtmLayerIndex = intf->addr.ilmi_address;
	 }


    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_atmfAtmLayerEntry_get(serialNum, contextInfo, arg, searchType, atmfAtmLayerIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = 0;  /* ILMI interface instance is always 0 */
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_atmfAtmLayerIndex
      case I_atmfAtmLayerIndex:
	dp = &data->atmfAtmLayerIndex;
	break;
#endif				       /* I_atmfAtmLayerIndex */

#ifdef I_atmfAtmLayerMaxVPCs
      case I_atmfAtmLayerMaxVPCs:
	dp = &data->atmfAtmLayerMaxVPCs;
	break;
#endif				       /* I_atmfAtmLayerMaxVPCs */

#ifdef I_atmfAtmLayerMaxVCCs
      case I_atmfAtmLayerMaxVCCs:
	dp = &data->atmfAtmLayerMaxVCCs;
	break;
#endif				       /* I_atmfAtmLayerMaxVCCs */

#ifdef I_atmfAtmLayerConfiguredVPCs
      case I_atmfAtmLayerConfiguredVPCs:
	dp = &data->atmfAtmLayerConfiguredVPCs;
	break;
#endif				       /* I_atmfAtmLayerConfiguredVPCs */

#ifdef I_atmfAtmLayerConfiguredVCCs
      case I_atmfAtmLayerConfiguredVCCs:
	dp = &data->atmfAtmLayerConfiguredVCCs;
	break;
#endif				       /* I_atmfAtmLayerConfiguredVCCs */

#ifdef I_atmfAtmLayerMaxVpiBits
      case I_atmfAtmLayerMaxVpiBits:
	dp = &data->atmfAtmLayerMaxVpiBits;
	break;
#endif				       /* I_atmfAtmLayerMaxVpiBits */

#ifdef I_atmfAtmLayerMaxVciBits
      case I_atmfAtmLayerMaxVciBits:
	dp = &data->atmfAtmLayerMaxVciBits;
	break;
#endif				       /* I_atmfAtmLayerMaxVciBits */

#ifdef I_atmfAtmLayerUniType
      case I_atmfAtmLayerUniType:
	dp = &data->atmfAtmLayerUniType;
	break;
#endif				       /* I_atmfAtmLayerUniType */

#ifdef I_atmfAtmLayerUniVersion
    case I_atmfAtmLayerUniVersion:
       dp = (void *) (&data->atmfAtmLayerUniVersion);
       break;
#endif /* I_atmfAtmLayerUniVersion */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the atmfAtmStatsEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
atmfAtmStatsEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    atmfAtmStatsEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            atmfAtmStatsIndex;
	addrtype		*intf;


    data = NULL;		/* make compiler shut up */
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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	return (NULL);
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &atmfAtmStatsIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }

	/*
	 * If the index value is zero then get the local interface number that
	 * is the one on which the SNMP pkt came in, and assign it to 
	 * atmfPortIndex.
	 */

	 if(!atmfAtmStatsIndex) {
     	if(!(intf = reg_invoke_snmp_packet_source()))
			return ((VarBind *) NULL);
     	atmfAtmStatsIndex = intf->addr.ilmi_address;
	 }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_atmfAtmStatsEntry_get(serialNum, contextInfo, arg, searchType, atmfAtmStatsIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
  	inst.oid_ptr[0] = 0;  /* ILMI interface instance is always 0 */
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_atmfAtmStatsIndex
      case I_atmfAtmStatsIndex:
	dp = &data->atmfAtmStatsIndex;
	break;
#endif				       /* I_atmfAtmStatsIndex */

#ifdef I_atmfAtmStatsReceivedCells
      case I_atmfAtmStatsReceivedCells:
	dp = &data->atmfAtmStatsReceivedCells;
	break;
#endif				       /* I_atmfAtmStatsReceivedCells */

#ifdef I_atmfAtmStatsDroppedReceivedCells
      case I_atmfAtmStatsDroppedReceivedCells:
	dp = &data->atmfAtmStatsDroppedReceivedCells;
	break;
#endif				       /* I_atmfAtmStatsDroppedReceivedCells */

#ifdef I_atmfAtmStatsTransmittedCells
      case I_atmfAtmStatsTransmittedCells:
	dp = &data->atmfAtmStatsTransmittedCells;
	break;
#endif				       /* I_atmfAtmStatsTransmittedCells */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the atmfVpcEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
atmfVpcEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
	/*
	 * We do not support VPCs so we always return NULL.
	 */
	return ((VarBind *) NULL);

}

/*---------------------------------------------------------------------
 * Retrieve data from the atmfVccEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
atmfVccEntry_get(
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    atmfVccEntry_t *data;
    unsigned long   buffer[3];
    OID             inst;
    int             carry;
    long            atmfVccPortIndex;
    long            atmfVccVpi;
    long            atmfVccVci;
	addrtype		*intf;


    data = NULL;		/* make compiler shut up */

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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 2 + object->oid.length, &atmfVccVci, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 1 + object->oid.length, &atmfVccVpi, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &atmfVccPortIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
	

	/*
	 * If the index value is zero then get the local interface number that
	 * is the one on which the SNMP pkt came in, and assign it to 
	 * atmfPortIndex.
	 */
	 if(!atmfVccPortIndex) {
     	if(!(intf = reg_invoke_snmp_packet_source()))
			return ((VarBind *) NULL);
     	atmfVccPortIndex = intf->addr.ilmi_address;
	 }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_atmfVccEntry_get(serialNum, contextInfo, arg, searchType, atmfVccPortIndex, atmfVccVpi, atmfVccVci)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 3;
   	inst.oid_ptr[0] = 0;  /* ILMI interface instance is always 0 */
	inst.oid_ptr[1] = data->atmfVccVpi;
	inst.oid_ptr[2] = data->atmfVccVci;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_atmfVccPortIndex
      case I_atmfVccPortIndex:
	dp = &data->atmfVccPortIndex;
	break;
#endif				       /* I_atmfVccPortIndex */

#ifdef I_atmfVccVpi
      case I_atmfVccVpi:
	dp = &data->atmfVccVpi;
	break;
#endif				       /* I_atmfVccVpi */

#ifdef I_atmfVccVci
      case I_atmfVccVci:
	dp = &data->atmfVccVci;
	break;
#endif				       /* I_atmfVccVci */

#ifdef I_atmfVccOperStatus
      case I_atmfVccOperStatus:
	dp = &data->atmfVccOperStatus;
	break;
#endif				       /* I_atmfVccOperStatus */

#ifdef I_atmfVccTransmitTrafficDescriptorType
      case I_atmfVccTransmitTrafficDescriptorType:
	dp = MakeOID(data->atmfVccTransmitTrafficDescriptorType->oid_ptr, data->atmfVccTransmitTrafficDescriptorType->length);
	break;
#endif				       /* I_atmfVccTransmitTrafficDescriptorTy
				        * pe */

#ifdef I_atmfVccTransmitTrafficDescriptorParam1
      case I_atmfVccTransmitTrafficDescriptorParam1:
	dp = &data->atmfVccTransmitTrafficDescriptorParam1;
	break;
#endif				       /* I_atmfVccTransmitTrafficDescriptorPa
				        * ram1 */

#ifdef I_atmfVccTransmitTrafficDescriptorParam2
      case I_atmfVccTransmitTrafficDescriptorParam2:
	dp = &data->atmfVccTransmitTrafficDescriptorParam2;
	break;
#endif				       /* I_atmfVccTransmitTrafficDescriptorPa
				        * ram2 */

#ifdef I_atmfVccTransmitTrafficDescriptorParam3
      case I_atmfVccTransmitTrafficDescriptorParam3:
	dp = &data->atmfVccTransmitTrafficDescriptorParam3;
	break;
#endif				       /* I_atmfVccTransmitTrafficDescriptorPa
				        * ram3 */

#ifdef I_atmfVccTransmitTrafficDescriptorParam4
      case I_atmfVccTransmitTrafficDescriptorParam4:
	dp = &data->atmfVccTransmitTrafficDescriptorParam4;
	break;
#endif				       /* I_atmfVccTransmitTrafficDescriptorPa
				        * ram4 */

#ifdef I_atmfVccTransmitTrafficDescriptorParam5
      case I_atmfVccTransmitTrafficDescriptorParam5:
	dp = &data->atmfVccTransmitTrafficDescriptorParam5;
	break;
#endif				       /* I_atmfVccTransmitTrafficDescriptorPa
				        * ram5 */

#ifdef I_atmfVccReceiveTrafficDescriptorType
      case I_atmfVccReceiveTrafficDescriptorType:
	dp = MakeOID(data->atmfVccReceiveTrafficDescriptorType->oid_ptr, data->atmfVccReceiveTrafficDescriptorType->length);
	break;
#endif				       /* I_atmfVccReceiveTrafficDescriptorTyp
				        * e */

#ifdef I_atmfVccReceiveTrafficDescriptorParam1
      case I_atmfVccReceiveTrafficDescriptorParam1:
	dp = &data->atmfVccReceiveTrafficDescriptorParam1;
	break;
#endif				       /* I_atmfVccReceiveTrafficDescriptorPar
				        * am1 */

#ifdef I_atmfVccReceiveTrafficDescriptorParam2
      case I_atmfVccReceiveTrafficDescriptorParam2:
	dp = &data->atmfVccReceiveTrafficDescriptorParam2;
	break;
#endif				       /* I_atmfVccReceiveTrafficDescriptorPar
				        * am2 */

#ifdef I_atmfVccReceiveTrafficDescriptorParam3
      case I_atmfVccReceiveTrafficDescriptorParam3:
	dp = &data->atmfVccReceiveTrafficDescriptorParam3;
	break;
#endif				       /* I_atmfVccReceiveTrafficDescriptorPar
				        * am3 */

#ifdef I_atmfVccReceiveTrafficDescriptorParam4
      case I_atmfVccReceiveTrafficDescriptorParam4:
	dp = &data->atmfVccReceiveTrafficDescriptorParam4;
	break;
#endif				       /* I_atmfVccReceiveTrafficDescriptorPar
				        * am4 */

#ifdef I_atmfVccReceiveTrafficDescriptorParam5
      case I_atmfVccReceiveTrafficDescriptorParam5:
	dp = &data->atmfVccReceiveTrafficDescriptorParam5;
	break;
#endif				       /* I_atmfVccReceiveTrafficDescriptorPar
				        * am5 */

#ifdef I_atmfVccQoSCategory
      case I_atmfVccQoSCategory:
	dp = &data->atmfVccQoSCategory;
	break;
#endif				       /* I_atmfVccQoSCategory */

#ifdef I_atmfVccTransmitQoSClass
      case I_atmfVccTransmitQoSClass:
	dp = &data->atmfVccTransmitQoSClass;
	break;
#endif				       /* I_atmfVccTransmitQoSClass */

#ifdef I_atmfVccReceiveQoSClass
      case I_atmfVccReceiveQoSClass:
	dp = &data->atmfVccReceiveQoSClass;
	break;
#endif				       /* I_atmfVccReceiveQoSClass */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}





/*
 * snmp_get_atm
 * Return the atm structure for the passed index, per the searchtype
 */
atm_db_t *
snmp_get_atm (int searchtype, long *index)
{
    return(atm_find_SNMP(index, (searchtype == NEXT)));
}

/*
 * snmp_get_atm_vc
 * Return the VC structure for the passed port index, per the search type
 */
static vc_info_t *
snmp_get_atm_vc (int searchtype, int index, int vpi, int vci)
{
    return(atm_vc_find_SNMP(index, vpi, vci, (searchtype == NEXT)));
}


atmfPhysicalGroup_t *
k_atmfPhysicalGroup_get(serialNum, contextInfo, nominator)
   int serialNum;
   ContextInfo *contextInfo;
   int nominator;
{
   static atmfPhysicalGroup_t atmfPhysicalGroupData;
   addrtype		*intf;
   ilmi_intfblock *intfblk=NULL;
   idbtype *swidb;


   intf = reg_invoke_snmp_packet_source();
   if(intf) {
       intfblk = ilmi_getintfblk(intf->addr.ilmi_address);
       if (!intfblk)
	   return NULL;
       else {
	   atmfPhysicalGroupData.atmfMyIpNmAddress = 0;
	   FOR_ALL_SWIDBS_ON_HW(intfblk->idb,swidb) {
	       if(!swidb->ip_address)
		   continue;

	       atmfPhysicalGroupData.atmfMyIpNmAddress = swidb->ip_address;
	       break;

	   }
       }
   }
   
   return(&atmfPhysicalGroupData);
}


atmfPortEntry_t *
k_atmfPortEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            atmfPortIndex)
{
    static atmfPortEntry_t 	atmfPortEntryData;
    static OID 				TransmissionType;
    static OID 				MediaType;
    static OID 				PortSpecific;
	static OctetString      ifname;
    hwidbtype 				*idb;
    atm_db_t 				*atm = NULL;


    atm = snmp_get_atm(searchType, &atmfPortIndex);
    if (atm == NULL)
	return(NULL);
    idb = atm->hwidb;
    if (idb == NULL)
	return(NULL);		/* can never happen */

    atmfPortEntryData.atmfPortIndex = atmfPortIndex;

    atmfPortEntryData.atmfPortTransmissionType = &TransmissionType;
    switch (atm->plimtype) {
    default:
    case ATM_PLIM_INVALID:
		TransmissionType.oid_ptr = (unsigned long *)IDatmfUnknownType;
		TransmissionType.length = LNatmfUnknownType;
		break;
    case ATM_PLIM_4B5B:
		TransmissionType.oid_ptr = (unsigned long *)IDatmf4B5B;
		TransmissionType.length = LNatmf4B5B;
		break;
    case ATM_PLIM_SONET:
		TransmissionType.oid_ptr = (unsigned long *)IDatmfSonetSTS3c;
		TransmissionType.length = LNatmfSonetSTS3c;
		break;
    case ATM_PLIM_E3:
		TransmissionType.oid_ptr = (unsigned long *)IDatmfDs3;
		TransmissionType.length = LNatmfDs3;
		break;
    case ATM_PLIM_DS3:
		TransmissionType.oid_ptr = (unsigned long *)IDatmfDs3;
		TransmissionType.length = LNatmfDs3;
		break;
    case ATM_PLIM_8B10B:
		TransmissionType.oid_ptr = (unsigned long *)IDatmf8B10B;
		TransmissionType.length = LNatmf8B10B;
		break;
    }

    atmfPortEntryData.atmfPortMediaType = &MediaType;
    switch (atm->medium) {
    default:
    case ATM_MEDIA_UNKNOWN:
		MediaType.oid_ptr = (unsigned long *)IDatmfMediaUnknownType;
		MediaType.length = LNatmfMediaUnknownType;
		break;
    case ATM_MEDIA_COAX:
		MediaType.oid_ptr = (unsigned long *)IDatmfMediaCoaxCable;
		MediaType.length = LNatmfMediaCoaxCable;
		break;
    case ATM_MEDIA_SINGLE_FIBER:
		MediaType.oid_ptr = (unsigned long *)IDatmfMediaSingleMode;
		MediaType.length = LNatmfMediaSingleMode;
		break;
    case ATM_MEDIA_MULTI_FIBER:
		MediaType.oid_ptr = (unsigned long *)IDatmfMediaMultiMode;
		MediaType.length = LNatmfMediaMultiMode;
		break;
    case ATM_MEDIA_STP:
		MediaType.oid_ptr = (unsigned long *)IDatmfMediaStp;
		MediaType.length = LNatmfMediaStp;
		break;
    case ATM_MEDIA_UTP:
		MediaType.oid_ptr = (unsigned long *)IDatmfMediaUtp;
		MediaType.length = LNatmfMediaUtp;
		break;
    }

    if (idb->inloopback)
		atmfPortEntryData.atmfPortOperStatus = D_atmfPortOperStatus_loopBack;
    else {
		switch (idb->state) {
		default:
	    	atmfPortEntryData.atmfPortOperStatus = D_atmfPortOperStatus_other;
	    	break;
		case IDBS_UP:
	    	atmfPortEntryData.atmfPortOperStatus = D_atmfPortOperStatus_inService;
	    	break;
		case IDBS_DOWN:
	    	atmfPortEntryData.atmfPortOperStatus = D_atmfPortOperStatus_outOfService;
	    	break;
		}
    }

    PortSpecific.oid_ptr = portSpecOID;
    PortSpecific.length  = ILMI_PORTSPECIFIC_LEN;
    atmfPortEntryData.atmfPortSpecific = &PortSpecific;

	if (idb->hw_namestring) {
		ifname.octet_ptr = idb->hw_namestring;
		ifname.length    = strlen(idb->hw_namestring);
	} else {
		ifname.octet_ptr = "None";
		ifname.length    = 4;
	}
	atmfPortEntryData.atmfPortMyIfName = &ifname;
    return (&atmfPortEntryData);
}

atmfAtmLayerEntry_t *
k_atmfAtmLayerEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            atmfAtmLayerIndex)
{
    static atmfAtmLayerEntry_t atmfAtmLayerEntryData;
    atm_db_t *atm;

    atm = snmp_get_atm(searchType, &atmfAtmLayerIndex);
    if (atm == NULL)
	return(NULL);

    atmfAtmLayerEntryData.atmfAtmLayerIndex = atmfAtmLayerIndex;
    atmfAtmLayerEntryData.atmfAtmLayerMaxVCCs = atm->config.max_intf_vcs;
    atmfAtmLayerEntryData.atmfAtmLayerConfiguredVCCs = atm->num_vc;
    atmfAtmLayerEntryData.atmfAtmLayerMaxVpiBits = atm->config.max_vpi_bits;
    atmfAtmLayerEntryData.atmfAtmLayerMaxVciBits = atm->config.max_vci_bits;
    atmfAtmLayerEntryData.atmfAtmLayerUniType = D_atmfAtmLayerUniType_private;
    if (atm->ignore_peer_uni)
      atmfAtmLayerEntryData.atmfAtmLayerUniVersion =  atm->uni_version;
    else
      atmfAtmLayerEntryData.atmfAtmLayerUniVersion =  ATMUNI_VERSION_31;

    return (&atmfAtmLayerEntryData);
}

atmfAtmStatsEntry_t *
k_atmfAtmStatsEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            atmfAtmStatsIndex)
{
    static atmfAtmStatsEntry_t atmfAtmStatsEntryData;
    atm_db_t *atm;

    atm = snmp_get_atm(searchType, &atmfAtmStatsIndex);
    if (atm == NULL)
	return(NULL);

    atmfAtmStatsEntryData.atmfAtmStatsIndex = atmfAtmStatsIndex;
    atmfAtmStatsEntryData.atmfAtmStatsReceivedCells = atm->cell.ifInOctets/53 ;
    atmfAtmStatsEntryData.atmfAtmStatsDroppedReceivedCells = atm->cell.ifInDiscards / 53 ;
    atmfAtmStatsEntryData.atmfAtmStatsTransmittedCells = atm->cell.ifOutOctets / 53 ;

    return (&atmfAtmStatsEntryData);
}

atmfVpcEntry_t *
k_atmfVpcEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            atmfVpcPortIndex,
    long            atmfVpcVpi)
{
	/* 
	 * VP service is not supported.
	 */

    return NULL;
}

atmfVccEntry_t *
k_atmfVccEntry_get(
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            atmfVccPortIndex,
    long            atmfVccVpi,
    long            atmfVccVci)
{
    static atmfVccEntry_t atmfVccEntryData;
    vc_info_t	*vc;
    static OID TxTrafficDescriptor, RxTrafficDescriptor;
    long peak;

    vc = snmp_get_atm_vc(searchType, atmfVccPortIndex, atmfVccVpi, atmfVccVci);
    if (vc == NULL)
	return(NULL);

    atmfVccEntryData.atmfVccPortIndex = vc->port_index;
    atmfVccEntryData.atmfVccVpi = vc->vpi;
    atmfVccEntryData.atmfVccVci = vc->vci;

    /* if we have a vc entry, we know we are up */
    atmfVccEntryData.atmfVccOperStatus = D_atmfVpcOperStatus_end2endUp;

    /*
     *	convert from peakrate in kb/s to peakrate in cells/s.  This is
     *	more complex than we would like as AAL3/4 allows 44 bytes of
     *	payload, AAL5 allows 48 bytes of payload.
     */
    peak = vc->peakrate * 1000;		/* to bits per second */
    peak /= 8;				/* to bytes per second */
    peak /= ATM_DATASIZE - (ISVCAAL34(vc) ? ATM_AAL34_OVERHEAD : 0);
					/* to cells per second */
    
    atmfVccEntryData.atmfVccTransmitTrafficDescriptorType=&TxTrafficDescriptor;
    TxTrafficDescriptor.oid_ptr = (unsigned long *)IDatmfNoClpNoScr;
    TxTrafficDescriptor.length = LNatmfNoClpNoScr;
    atmfVccEntryData.atmfVccTransmitTrafficDescriptorParam1 = peak;
    atmfVccEntryData.atmfVccTransmitTrafficDescriptorParam2 = 0; /* not used */
    atmfVccEntryData.atmfVccTransmitTrafficDescriptorParam3 = 0; /* not used */
    atmfVccEntryData.atmfVccTransmitTrafficDescriptorParam4 = 0; /* not used */
    atmfVccEntryData.atmfVccTransmitTrafficDescriptorParam5 = 0; /* not used */


    atmfVccEntryData.atmfVccReceiveTrafficDescriptorType= &RxTrafficDescriptor;
    RxTrafficDescriptor.oid_ptr = (unsigned long *)IDatmfNoClpNoScr;
    RxTrafficDescriptor.length = LNatmfNoClpNoScr;
    atmfVccEntryData.atmfVccReceiveTrafficDescriptorParam1 = peak;
    atmfVccEntryData.atmfVccReceiveTrafficDescriptorParam2 = 0; /* not used */
    atmfVccEntryData.atmfVccReceiveTrafficDescriptorParam3 = 0; /* not used */
    atmfVccEntryData.atmfVccReceiveTrafficDescriptorParam4 = 0; /* not used */
    atmfVccEntryData.atmfVccReceiveTrafficDescriptorParam5 = 0; /* not used */

    atmfVccEntryData.atmfVccTransmitQoSClass = 0;
    atmfVccEntryData.atmfVccReceiveQoSClass = 0;

    return (&atmfVccEntryData);
}




static ifmib_err_t atm_subif_cntr32fn (
	  ifmib_cntr_t	ifCntr,
	  const	subiabtype *subiab,
          ulong		*retval,
	  int     serialNum)
/* 
 * get the specified ifEntry or ifXEntry counter 
 * INPUTS:
 *	ifCntr == enum of specific counter requested
 *	if_ptr == r/o pointer to IDB for this interface
 *      serialNum == passed in from getrequest for caching
 * OUTPUTS:
 *	*retval == the returned counter value
 * RETURNS:
 *	return status
 *
 *  this routine needed to support subinterfaces in ifmib for atm.
 */

{
    /* 
     * at this point, there are no cell counts for atm interfaces 
     * if/when implemented, they should be ifGeneralGroup, and 
     * ifFixedLengthGroup, as per rfc1695
     */
    return(IF_CNTR_NOT_AVAIL);

}

static ifmib_err_t aal5_subif_cntr32fn (
	  ifmib_cntr_t	ifCntr,
	  const	subiabtype *subiab,
          ulong		*retval,
	  int     serialNum)
/* 
 * get the specified ifEntry or ifXEntry counter 
 * INPUTS:
 *	ifCntr == enum of specific counter requested
 *	if_ptr == r/o pointer to IDB for this interface
 *      serialNum == passed in from getrequest for caching
 * OUTPUTS:
 *	*retval == the returned counter value
 * RETURNS:
 *	return status
 *
 *  this routine needed to support subinterfaces in ifmib for atm.
 * conformance info from rfc1695 for aal5 sublayer...ifGeneralGroup, 
 * ifPacketGroup, and ifHCPacketGroup
 */

{

    hwidbtype  *hwidb;
    atm_db_t *atm;

    hwidb = NULL;
    *retval = 0;

    if ((hwidb = subiab->master_ptr.ifptr.hwidb) == NULL) {
	return(IF_CNTR_NOT_AVAIL);
    }
    if (!is_atm(hwidb) || !IS_ATMIF_AIP(hwidb)) {
	/*
	 * if not atm, or not AIP type of atm interface, forget it.
	 * (as in can't tally ATMizer counts since it's in the
	 * fastpath, and don't want to report only active vc counts)
	 */
	return(IF_CNTR_NOT_AVAIL);
    }

    /*
     * Do a block update of all the stats so that they're accurate to
     * the last SNMP poll instead of the last 30 second periodic poll.
     */
    atm = hwidb->atm_db;
    atm->funcs.getvc_stats(hwidb, NULL);

    switch (ifCntr) {
    case IN_OCTETS_CNTR:
	*retval = atm->aal5.ifInOctets;
	break;
	
    case OUT_OCTETS_CNTR:
	*retval = atm->aal5.ifOutOctets;
	break;
	/*
	 * these are stated to be zero by rfc1695
	 */
    case IN_UNK_PROTOS_CNTR:
    case IN_BCAST_CNTR:			/* ifXTable */
    case IN_MCAST_CNTR:
    case OUT_BCAST_CNTR:
    case OUT_MCAST_CNTR:
	*retval = 0;
	break;
	/*
	 * these are separated because they should be supported for a
	 * conformant agent, but aren't available in the router yet
	 */
    case IN_ERRORS_CNTR:
    case OUT_ERRORS_CNTR:
    case IN_UCAST_CNTR:
    case IN_DISCARDS_CNTR:
    case OUT_UCAST_CNTR:
    case OUT_DISCARDS_CNTR:
    default:
	return(IF_CNTR_NOT_AVAIL);
	
    }
    return (IF_NO_ERR);
}


/*---------------------------------------------------------------------
 * atm_add_subif
 *
 * register the specified ATM with the IF-MIB library
 * INPUTS:
 *	idb == ptr to idb to base IPATM, AAL5, ATM intf. to add to the IF-MIB
 *---------------------------------------------------------------------*/
static const char atm_layer_str[] = "-atm layer";
static const char aal5_layer_str[] = "-aal5 layer";
static void 
atm_add_subif (idbtype *idb)
{
    subiabtype    *subiab;
    snmpifptrtype if_ptr;
    char *buffer; 
    ulong state;
    long atm_if_index;
    snmpidbtype *snmpidb;
    boolean result;
    ifStackEntry_t * stack_result;
    hwidbtype *hwidb;

    snmpidb  = NULL;    

    if((idb == NULL) || (idb->subif_state & SUBIF_STATE_DELETED)) {
	return;
    }

    hwidb = idb->hwptr;

    /* if this is not the main swidb, we shouldn't do anything */
    if (idb != hwidb->firstsw) {
	return;
    }

    /*
     * if this swidb has already been registered with the ifmib then
     * we don't need to do anything.
     * note: must ensure that the swidb's ifindex != the hwidb's ifindex.
     * if they are equal, then this swidb has not been registered (only
     * the hwidb has been registered).
     */
    if ((idb->snmp_if_index) &&	
	(idb->snmp_if_index != hwidb->snmp_if_index)) {
	snmpidb = ifmib_snmpidb_get(NULL,0,EXACT,idb->snmp_if_index,NULL,NULL);
	if (snmpidb != NULL) {
	    return;
	}
    }

    /* need to register the aal5/atm layers.
     * atm sublayer has no associated structure.  so use subiabtype
     * NOTE - will have to free strings when subiabtype goes away!
     */
    subiab = malloc(sizeof(subiabtype));
    if (subiab == NULL) {
	return;
    }
    if_ptr.ifptr.subiab = subiab;

    /* now use master ptr to point back to the master controlling hwidb */
    subiab->master_ptr.ifptr.hwidb = hwidb;
    subiab->master_type = SNMP_IF_HWIDB;
    subiab->data_ptr = NULL;

    state = reg_invoke_ifmib_get_adminstatus(D_ifType_atm, subiab);

    /* allocate space for the concatinated name and the null terminator */
    buffer = malloc( strlen(idb->namestring) + strlen(atm_layer_str) + 1);

    if (buffer != NULL) {
	(void) sprintf(buffer, "%s%s", idb->namestring, atm_layer_str);
	setstring(&subiab->if_descrstring, buffer); 
	free(buffer);
    } else {
	subiab->if_descrstring = NULL;
    }
    /* aal5 is controlled by physical interface - no separate access!)*/
    subiab->if_name = idb->short_namestring;
    if (idb->nsap != NULL) {
	subiab->ifPhysAddr = idb->nsap->addr;
	subiab->ifPhysAddrLen = (uint *)&idb->nsap->length;
    }
    subiab->ifType = D_ifType_atm;
    subiab->maxmtu = NULL;           /* not applicable */
    result = reg_invoke_ifmib_register_subif(&if_ptr, SNMP_IF_SUBIAB, state);
    if (result == FALSE) {
	free(subiab);
	return;
    }
    /* 
     * now add the link to the ifmib stack table
     */
    stack_result =
	reg_invoke_ifmib_create_stacklink(subiab->snmp_if_index,
					  hwidb->snmp_if_index);
    if(stack_result == NULL) {
	return;
    }

    atm_if_index = subiab->snmp_if_index;

    /*
     * aal5 sublayer has no associated structure.  so use subiabtype
     * NOTE - will have to free strings when subiabtype goes away!
     */
    subiab = malloc(sizeof(subiabtype));
    if (subiab == NULL) {
	return;
    }
    if_ptr.ifptr.subiab = subiab;

    /* now use  master ptr to point back to the master controlling hwidb */
    subiab->master_ptr.ifptr.hwidb = hwidb;
    subiab->master_type = SNMP_IF_HWIDB;
    subiab->data_ptr = NULL;

    /* callback for cleaning up mem we alloc in the subiab */
    subiab->cleaner = atm_cleanup_subif;

    state = reg_invoke_ifmib_get_adminstatus(D_ifType_aal5, subiab);

    /* allocate space for the concatinated name and the null terminator */
    buffer = malloc(strlen(idb->namestring) + strlen(aal5_layer_str) + 1);
    if (buffer != NULL) {
	(void) sprintf(buffer, "%s%s", idb->namestring, aal5_layer_str);
	setstring(&subiab->if_descrstring, buffer); 
	free(buffer);
    } else {
	subiab->if_descrstring = NULL;
    }
    /* aal5 is controlled by physical interface - no separate access!)*/
    subiab->if_name = idb->short_namestring;
    subiab->ifPhysAddr = 0;
    subiab->ifPhysAddrLen = NULL;
    subiab->ifType = D_ifType_aal5;
    subiab->maxmtu = &hwidb->maxmtu;
    result = reg_invoke_ifmib_register_subif(&if_ptr, SNMP_IF_SUBIAB, state);
    if (result == FALSE) {
	free(subiab);
	return;
    }
    /* 
     * now add the link to the ifmib stack table
     */
    stack_result =
	reg_invoke_ifmib_create_stacklink(subiab->snmp_if_index, atm_if_index);
    if(stack_result == NULL) {
	return;
    }
}
			    


/*---------------------------------------------------------------------
 * atm_delete_subif
 *
 * de-register the specified atm and aal5 with the IF-MIB library
 * INPUTS:
 *	swidb
 *---------------------------------------------------------------------*/
static void 
atm_delete_subif (idbtype *idb)
{

    snmpidbtype *snmpidb;
    subiabtype *subiab;
    long ifIndex1, ifIndex2;

    ifIndex1 = 0;
    ifIndex2 = 0;
    subiab = NULL;
    
    if ((idb == NULL) || (idb != idb->hwptr->firstsw)) {
	return;
    }

    /* this is aal5/atm deregistration! */

    /* 
     * first - find the snmpidb associated with this ifIndex
     */
    FOR_ALL_SNMPIDBS(snmpidb) {
	if (snmpidb->snmp_if_struct != SNMP_IF_SUBIAB) {
	    continue;
	}
	subiab = snmpidb->snmp_if_ptr.ifptr.subiab;
	if ((subiab == NULL) || (subiab->master_type != SNMP_IF_HWIDB)) {
	    continue;
	}
	/*
	 * check if pointing to same hwidb, but not matching ifIndex
	 * (thus must be the atm or aal5 subif)
	 */
	if (idb->hwptr == subiab->master_ptr.ifptr.hwidb) {
	    if (ifIndex1 == 0) {
		ifIndex1 = snmpidb->snmp_if_index;
	    } else {
		ifIndex2 = snmpidb->snmp_if_index;
	    }
	    if (ifIndex2 != 0) {
		/* have both indices - break out and dereg */
		break;
	    }
	}
    }
    reg_invoke_ifmib_deregister_subif(ifIndex1);
    reg_invoke_ifmib_deregister_subif(ifIndex2);
}

/*---------------------------------------------------------------------
 * atm_cleanup_subif
 *
 * free up memory that we added to the atm and aal5 subinterfaces
 * INPUTS:
 *	subiab
 *---------------------------------------------------------------------*/
static void 
atm_cleanup_subif (subiabtype *subiab)
{
    if (subiab->if_descrstring) {
	free(subiab->if_descrstring);
    }
}

/*
 * AdminStatus for atm and aal5 layers.  matches the same for physical
 * interface
 */
static enum SNMP_ADMIN_STATE atm_get_adminstatus ( const subiabtype *subiab )
{
    hwidbtype  *hwidb;

    hwidb = subiab->master_ptr.ifptr.hwidb;
    if (hwidb == NULL) {
	return SNMP_ADMIN_ERROR;
    }
    if (hwidb->state == IDBS_ADMINDOWN) {
	return SNMP_ADMIN_DOWN;
    } else {
	return SNMP_ADMIN_UP;
    }
}

/*
 * operStatus for atm and aal5 layers.  matches the same for physical
 * interface
 */

static enum SNMP_ADMIN_STATE atm_get_operstatus (const subiabtype *subiab)
{

    hwidbtype  *hwidb;

    hwidb = subiab->master_ptr.ifptr.hwidb;
    if (hwidb == NULL) {
	return SNMP_ADMIN_UNKNOWN;
    }
    if (hw_interface_up(hwidb)) {
	if (hwidb->inloopback) {
	    return SNMP_ADMIN_TESTING;
	} else {
	    if (is_ddr(hwidb) && 
		(reg_invoke_dialer_spoofing_int(hwidb) ||
		 reg_invoke_dialer_spoofing_prot(hwidb))) {
		return SNMP_ADMIN_DORMANT;
	    } else {
		return SNMP_ADMIN_UP;
	    }
	}
    } else {
	return SNMP_ADMIN_DOWN;
    }
}

static boolean atm_admin_change (
	  enum SNMP_ADMIN_STATE	newStatus,
	  boolean		testPhase,
	  const	subiabtype      *subiab)

/*
 * support function for changing ifAdminStatus - aal5 and atm layers
 *
 *
 * LOG-IF-MIB ifAdminStatus change callback function.
 * INPUTS:
 *     newStatus == requested new value of ifAdminStatus
 *		     this must be different than the current value
 *     testPhase == TRUE if this the value is being checked
 *                   in the test phase of PDU processing
 *		     FALSE if the change should take effect now
 *     if_ptr == r/o pointer to IDB for this interface
 * RETURNS:
 *   TRUE if the change is allowed by LOG-IF-MIB
 *       if testPhase == TRUE:
 *          upon exit, ifAdminStatus has not changed.       
 *       if testPhase == FALSE:
 *	    upon exit, the new value of ifAdminStatus is in effect
 *  FALSE if LOG-IF-MIB is rejecting the requested change
 *       upon exit, ifAdminStatus has not changed.
 */ 

{
  
 /* note - do not support changing this admin status, as we have no
  * control over it in the cli.  But need a kludge patch to allow sets
  * back to the same value to pass snmp tcl tests. no need for
  * separate test phase, as not making any real changes...
  */
    
    
    hwidbtype  *hwidb;
    boolean result;

    result = FALSE;
    hwidb = subiab->master_ptr.ifptr.hwidb;
    if (hwidb == NULL) {
	return FALSE;
    }
    switch (hwidb->state) {
    case IDBS_ADMINDOWN :
	if (newStatus == SNMP_ADMIN_DOWN) {
	    result = TRUE; 
	}
	break;

    default:
	if (newStatus == SNMP_ADMIN_UP) {
	    result = TRUE; 
	}
	break;
    }
    return result;
}


/*
 * ilmimib subsystem header
 */


#define MAJVERSION_ilmimib 1
#define MINVERSION_ilmimib 0
#define EDITVERSION_ilmimib 0

SUBSYS_HEADER(ilmimib,
              MAJVERSION_ilmimib,
              MINVERSION_ilmimib,
              EDITVERSION_ilmimib,
              init_ilmimib,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
