/* $Id: sr_old_ldecnetmib.c,v 3.5.12.3 1996/07/03 20:40:48 thille Exp $
 * $Source: /release/112/cvs/Xsys/decnet/sr_old_ldecnetmib.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * April 1994, Chifei W. Cheng (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_old_ldecnetmib.c,v $
 * Revision 3.5.12.3  1996/07/03  20:40:48  thille
 * CSCdi61860: Implement Jeffs glass of slim-fast for snmp
 * Branch: California_branch
 * Take hunk of common code from many mib files, make it a procedure in
 * snmp_util.c and call it from the mibs.  Save 1640 bytes.
 *
 * Revision 3.5.12.2  1996/05/21  09:45:00  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.12.1  1996/05/21  06:25:58  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.5  1996/02/15  19:50:10  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:44:31  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:22:57  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  08:59:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:17:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/17  07:36:20  bchan
 * CSCdi34760:  Ifindex usage incorrect
 *
 * Revision 2.3  1995/07/07  22:46:53  hampton
 * Convert DECNET to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi36830]
 *
 * Revision 2.2  1995/06/28 09:23:04  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  20:23:41  hampton
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
#include "subsys.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "decnet_externs.h"
#include "decnet.h"
#include "../snmp/snmp_api.h"
#include "sr_old_ldecnetmib.h"
#include "sr_old_ldecnetmib-mib.h"
#include "snmp_interface.h"
#include "../snmp/ifmibapi.h"


void
init_old_ldecnetmib (subsystype *subsys)
{
    load_mib(old_ldecnetmib_OidList, old_ldecnetmib_OidListNum);
    load_oid(old_ldecnetmib_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the tmpdecnet family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
tmpdecnet_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    
    void           *dp;
    tmpdecnet_t    *data = NULL;
    int             arg;


    arg = snmp_scalar_instance(incoming, object, searchType);

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_tmpdecnet_get(serialNum, contextInfo, arg)) == NULL) {
	arg = -1;
    }
    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dnForward
      case I_dnForward:
	dp = &data->dnForward;
	break;
#endif				       /* I_dnForward */

#ifdef I_dnReceived
      case I_dnReceived:
	dp = &data->dnReceived;
	break;
#endif				       /* I_dnReceived */

#ifdef I_dnFormaterr
      case I_dnFormaterr:
	dp = &data->dnFormaterr;
	break;
#endif				       /* I_dnFormaterr */

#ifdef I_dnNotgateway
      case I_dnNotgateway:
	dp = &data->dnNotgateway;
	break;
#endif				       /* I_dnNotgateway */

#ifdef I_dnNotimp
      case I_dnNotimp:
	dp = &data->dnNotimp;
	break;
#endif				       /* I_dnNotimp */

#ifdef I_dnHellos
      case I_dnHellos:
	dp = &data->dnHellos;
	break;
#endif				       /* I_dnHellos */

#ifdef I_dnBadhello
      case I_dnBadhello:
	dp = &data->dnBadhello;
	break;
#endif				       /* I_dnBadhello */

#ifdef I_dnNotlong
      case I_dnNotlong:
	dp = &data->dnNotlong;
	break;
#endif				       /* I_dnNotlong */

#ifdef I_dnDatas
      case I_dnDatas:
	dp = &data->dnDatas;
	break;
#endif				       /* I_dnDatas */

#ifdef I_dnBigaddr
      case I_dnBigaddr:
	dp = &data->dnBigaddr;
	break;
#endif				       /* I_dnBigaddr */

#ifdef I_dnNoroute
      case I_dnNoroute:
	dp = &data->dnNoroute;
	break;
#endif				       /* I_dnNoroute */

#ifdef I_dnNoencap
      case I_dnNoencap:
	dp = &data->dnNoencap;
	break;
#endif				       /* I_dnNoencap */

#ifdef I_dnLevel1s
      case I_dnLevel1s:
	dp = &data->dnLevel1s;
	break;
#endif				       /* I_dnLevel1s */

#ifdef I_dnBadlevel1
      case I_dnBadlevel1:
	dp = &data->dnBadlevel1;
	break;
#endif				       /* I_dnBadlevel1 */

#ifdef I_dnToomanyhops
      case I_dnToomanyhops:
	dp = &data->dnToomanyhops;
	break;
#endif				       /* I_dnToomanyhops */

#ifdef I_dnHellosent
      case I_dnHellosent:
	dp = &data->dnHellosent;
	break;
#endif				       /* I_dnHellosent */

#ifdef I_dnLevel1sent
      case I_dnLevel1sent:
	dp = &data->dnLevel1sent;
	break;
#endif				       /* I_dnLevel1sent */

#ifdef I_dnNomemory
      case I_dnNomemory:
	dp = &data->dnNomemory;
	break;
#endif				       /* I_dnNomemory */

#ifdef I_dnOtherhello
      case I_dnOtherhello:
	dp = &data->dnOtherhello;
	break;
#endif				       /* I_dnOtherhello */

#ifdef I_dnOtherlevel1
      case I_dnOtherlevel1:
	dp = &data->dnOtherlevel1;
	break;
#endif				       /* I_dnOtherlevel1 */

#ifdef I_dnLevel2s
      case I_dnLevel2s:
	dp = &data->dnLevel2s;
	break;
#endif				       /* I_dnLevel2s */

#ifdef I_dnLevel2sent
      case I_dnLevel2sent:
	dp = &data->dnLevel2sent;
	break;
#endif				       /* I_dnLevel2sent */

#ifdef I_dnNovector
      case I_dnNovector:
	dp = &data->dnNovector;
	break;
#endif				       /* I_dnNovector */

#ifdef I_dnOtherlevel2
      case I_dnOtherlevel2:
	dp = &data->dnOtherlevel2;
	break;
#endif				       /* I_dnOtherlevel2 */

#ifdef I_dnNoaccess
      case I_dnNoaccess:
	dp = &data->dnNoaccess;
	break;
#endif				       /* I_dnNoaccess */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dnAreaTableEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dnAreaTableEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dnAreaTableEntry_t *data;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
    long            dnArea;


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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &dnArea, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dnAreaTableEntry_get(serialNum, contextInfo, arg, searchType, dnArea)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->dnArea;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dnArea
      case I_dnArea:
	dp = &data->dnArea;
	break;
#endif				       /* I_dnArea */

#ifdef I_dnACost
      case I_dnACost:
	dp = &data->dnACost;
	break;
#endif				       /* I_dnACost */

#ifdef I_dnAHop
      case I_dnAHop:
	dp = &data->dnAHop;
	break;
#endif				       /* I_dnAHop */

#ifdef I_dnAIfIndex
      case I_dnAIfIndex:
	dp = &data->dnAIfIndex;
	break;
#endif				       /* I_dnAIfIndex */

#ifdef I_dnANextHop
      case I_dnANextHop:
	dp = MakeOctetString(data->dnANextHop->octet_ptr, data->dnANextHop->length);
	break;
#endif				       /* I_dnANextHop */

#ifdef I_dnAAge
      case I_dnAAge:
	dp = &data->dnAAge;
	break;
#endif				       /* I_dnAAge */

#ifdef I_dnAPrio
      case I_dnAPrio:
	dp = &data->dnAPrio;
	break;
#endif				       /* I_dnAPrio */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dnHostTableEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dnHostTableEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dnHostTableEntry_t *data;
    unsigned long   buffer[2];
    OID             inst;
    int             carry;
    long            dnHIdx1;
    long            dnHIdx2;


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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 1 + object->oid.length, &dnHIdx2, searchType, &carry)) < 0) {
	arg = -1;
    }
    if ((InstToInt(incoming, 0 + object->oid.length, &dnHIdx1, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dnHostTableEntry_get(serialNum, contextInfo, arg, searchType, dnHIdx1, dnHIdx2)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 2;
	inst.oid_ptr[0] = data->dnHIdx1;
	inst.oid_ptr[1] = data->dnHIdx2;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dnHost
      case I_dnHost:
	dp = &data->dnHost;
	break;
#endif				       /* I_dnHost */

#ifdef I_dnHCost
      case I_dnHCost:
	dp = &data->dnHCost;
	break;
#endif				       /* I_dnHCost */

#ifdef I_dnHHop
      case I_dnHHop:
	dp = &data->dnHHop;
	break;
#endif				       /* I_dnHHop */

#ifdef I_dnHIfIndex
      case I_dnHIfIndex:
	dp = &data->dnHIfIndex;
	break;
#endif				       /* I_dnHIfIndex */

#ifdef I_dnHNextHop
      case I_dnHNextHop:
	dp = MakeOctetString(data->dnHNextHop->octet_ptr, data->dnHNextHop->length);
	break;
#endif				       /* I_dnHNextHop */

#ifdef I_dnHAge
      case I_dnHAge:
	dp = &data->dnHAge;
	break;
#endif				       /* I_dnHAge */

#ifdef I_dnHPrio
      case I_dnHPrio:
	dp = &data->dnHPrio;
	break;
#endif				       /* I_dnHPrio */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the dnIfTableEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind        *
dnIfTableEntry_get (
    OID            *incoming,
    ObjectInfo     *object,
    int             searchType,
    ContextInfo    *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void           *dp;
    dnIfTableEntry_t *data = NULL;
    unsigned long   buffer[1];
    OID             inst;
    int             carry;
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
	    return ((VarBind *) NULL);
	}
	carry = 0;
    } else {
	carry = 1;
    }

    if ((InstToInt(incoming, 0 + object->oid.length, &ifIndex, searchType, &carry)) < 0) {
	arg = -1;
    }
    if (carry) {
	arg = -1;
    }
    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ((arg == -1) || (data = k_dnIfTableEntry_get(serialNum, contextInfo, arg, searchType, ifIndex)) == NULL) {
	arg = -1;
    } else {
	/*
	 * Build instance information
	 */
	inst.oid_ptr = buffer;
	inst.length = 1;
	inst.oid_ptr[0] = data->ifIndex;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_dnIfCost
      case I_dnIfCost:
	dp = &data->dnIfCost;
	break;
#endif				       /* I_dnIfCost */

      default:
	return ((VarBind *) NULL);

    }				       /* switch */

    return (MakeVarBind(object, &inst, dp));

}





tmpdecnet_t    *
k_tmpdecnet_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator)
{
    static tmpdecnet_t tmpdecnetData;

    if (!dn_running)
	return(NULL);
    tmpdecnetData.dnForward = dn_stat[DNST_FORWARD];
    tmpdecnetData.dnReceived = dn_stat[DNST_RECEIVED];
    tmpdecnetData.dnFormaterr = dn_stat[DN_FORMATERR];
    tmpdecnetData.dnNotgateway = dn_stat[DN_NOTGATEWAY];
    tmpdecnetData.dnNotimp = dn_stat[DN_NOTIMP];
    tmpdecnetData.dnHellos = dn_stat[DNST_HELLOS];
    tmpdecnetData.dnBadhello = dn_stat[DN_BADHELLO];
    tmpdecnetData.dnNotlong = dn_stat[DN_NOTLONG];
    tmpdecnetData.dnDatas = dn_stat[DNST_DATAS];
    tmpdecnetData.dnBigaddr = dn_stat[DN_BIGADDR];
    tmpdecnetData.dnNoroute = dn_stat[DN_NOROUTE];
    tmpdecnetData.dnNoencap = dn_stat[DN_NOENCAP];
    tmpdecnetData.dnLevel1s = dn_stat[DNST_LVL1S];
    tmpdecnetData.dnBadlevel1 = dn_stat[DN_BADLVL1];
    tmpdecnetData.dnToomanyhops = dn_stat[DN_TOOMANYVISITS];
    tmpdecnetData.dnHellosent = dn_stat[DNST_HELLOSNT];
    tmpdecnetData.dnLevel1sent = dn_stat[DNST_LVL1SNT];
    tmpdecnetData.dnNomemory = dn_stat[DN_NOMEM];
    tmpdecnetData.dnOtherhello = dn_stat[DN_OTHER_AREA_HELLO];
    tmpdecnetData.dnOtherlevel1 = dn_stat[DN_OTHER_AREA_LVL1];
    tmpdecnetData.dnLevel2s = dn_stat[DNST_LVL2S];
    tmpdecnetData.dnLevel2sent = dn_stat[DNST_LVL2SNT];
    tmpdecnetData.dnNovector = dn_stat[DN_NOVEC];
    tmpdecnetData.dnOtherlevel2 = dn_stat[DN_OTHER_LVL2];
    tmpdecnetData.dnNoaccess = dn_stat[DN_NOACCESS];
    return (&tmpdecnetData);
}

dnAreaTableEntry_t *
k_dnAreaTableEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dnArea)
{
    static dnAreaTableEntry_t dnAreaTableEntryData;
    static OctetString aNextHop;
    dnroutetype **routeptr, *route;

    if (!dn_running || !dn_nets[0] || !dn_nets[0]->doareas || (u_long)dnArea >= DN_MAXAREAS)
	return(NULL);
    routeptr = dn_nets[0]->dnareas + dnArea;
    if (searchType == NEXT)
	for (; routeptr < dn_nets[0]->dnroutes && *routeptr == NULL; routeptr++);
    if (routeptr && (route = *routeptr) && routeptr < dn_nets[0]->dnroutes) {
	dnAreaTableEntryData.dnArea = routeptr - dn_nets[0]->dnareas;
	dnAreaTableEntryData.dnACost = route->cost;
	dnAreaTableEntryData.dnAHop = route->hops;
	dnAreaTableEntryData.dnAIfIndex = route->flags & DN_RT_ME ?
				0 : route->interface->hwptr->snmp_if_index;
	dnAreaTableEntryData.dnANextHop = &aNextHop;
	dnAreaTableEntryData.dnANextHop->octet_ptr = route->gateway;
	dnAreaTableEntryData.dnANextHop->length = sizeof(route->gateway);
	dnAreaTableEntryData.dnAAge = 0;
	for (; route != NULL; route = route->nextadj)
	    if (route->flags & DN_RT_HELLO) {
                dnAreaTableEntryData.dnAAge = TIME_LEFT_SLEEPING(route->routetimer)/ONESEC;
		break;
	    }
	dnAreaTableEntryData.dnAPrio = 0;
	for (route = *routeptr; route != NULL; route = route->nextadj)
	    if (route->flags & DN_RT_ROUTER) {
                dnAreaTableEntryData.dnAPrio = route->prio;
		break;
	    }
	return (&dnAreaTableEntryData);
    }
    else
	return (NULL);
}

dnHostTableEntry_t *
k_dnHostTableEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            dnHIdx1,	/* area number */
    long            dnHIdx2)	/* node number */
{
    static dnHostTableEntry_t dnHostTableEntryData;
    static OctetString hNextHop;
    dnroutetype **routeptr, *route;

    if (!dn_running || !dn_nets[0])
	return (NULL);
    if (dnHIdx1 == dn_nets[0]->my_dn_area && (u_long)dnHIdx2 < DN_MAXNODES)
	routeptr = dn_nets[0]->dnroutes + dnHIdx2;
    else if (searchType == NEXT && dnHIdx1 < dn_nets[0]->my_dn_area)
	routeptr = dn_nets[0]->dnroutes;
    else
	return (NULL);
    if (searchType == NEXT)
	for (; routeptr < &dn_nets[0]->dnroutes[DN_MAXNODES] && *routeptr == NULL; routeptr++);
    if ((route = *routeptr) && routeptr < &dn_nets[0]->dnroutes[DN_MAXNODES]) {
	dnHostTableEntryData.dnHost = dnHostTableEntryData.dnHIdx2 =
					routeptr - dn_nets[0]->dnroutes;
	dnHostTableEntryData.dnHCost = route->cost;
	dnHostTableEntryData.dnHHop = route->hops;
	dnHostTableEntryData.dnHIfIndex = route->flags & DN_RT_ME ? 
					0 : route->interface->hwptr->snmp_if_index;
	dnHostTableEntryData.dnHNextHop = &hNextHop;
	dnHostTableEntryData.dnHNextHop->octet_ptr = route->gateway;
	dnHostTableEntryData.dnHNextHop->length = sizeof(route->gateway);
	dnHostTableEntryData.dnHAge = 0;
	for (route = *routeptr; route != NULL; route = route->nextadj)
	    if (route->flags & DN_RT_HELLO) {
                dnHostTableEntryData.dnHAge = TIME_LEFT_SLEEPING(route->routetimer)/ONESEC;
		break;
	    }
	dnHostTableEntryData.dnHPrio = 0;
	for (route = *routeptr; route != NULL; route = route->nextadj)
	    if (route->flags & DN_RT_ROUTER) {
                dnHostTableEntryData.dnHPrio = route->prio;
		break;
	    }
	dnHostTableEntryData.dnHIdx1 = dn_nets[0]->my_dn_area;
	return (&dnHostTableEntryData);
    } else
	return (NULL);
}

static boolean dnIfTester(hwidbtype* idb, long* dummy)
{
   return RUNNING_IF(LINK_DECNET, idb->firstsw);
}

dnIfTableEntry_t *
k_dnIfTableEntry_get (
    int             serialNum,
    ContextInfo    *contextInfo,
    int             nominator,
    int             searchType,
    long            ifIndex)
{
    static dnIfTableEntry_t dnIfTableEntryData;
    hwidbtype *idb;

    if (dn_running) {
	idb = ifmib_hwidb_get(serialNum, searchType, ifIndex, dnIfTester,NULL);
        if (idb != NULL) {
	    dnIfTableEntryData.dnIfCost = idb->firstsw->dn_cost;
	    dnIfTableEntryData.ifIndex = idb->snmp_if_index;
	    return (&dnIfTableEntryData);
	}
    }
    return (NULL);
}

/*
 * old_ldecnetmib subsystem header
 */

#define MAJVERSION_old_ldecnetmib 1
#define MINVERSION_old_ldecnetmib 0
#define EDITVERSION_old_ldecnetmib 0

SUBSYS_HEADER(decnetmib,
              MAJVERSION_old_ldecnetmib,
              MINVERSION_old_ldecnetmib,
              EDITVERSION_old_ldecnetmib,
              init_old_ldecnetmib,
              SUBSYS_CLASS_MANAGEMENT,
              "req: decnet",
              NULL);
      
