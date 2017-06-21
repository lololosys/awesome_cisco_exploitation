/* $Id: sr_udpmib2.c,v 3.5.10.2 1996/07/06 05:54:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/ip/sr_udpmib2.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, Jeffrey T. Johnson (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_udpmib2.c,v $
 * Revision 3.5.10.2  1996/07/06  05:54:04  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.5.10.1  1996/06/18  01:46:14  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.5  1996/02/23  15:20:26  bstewart
 * CSCdi48325:  SNMP MIBs need modularized translation tables
 *
 * Revision 3.4  1996/01/18  15:48:34  anke
 * CSCdi46969:  Change empty req and/or seq strings in SUBSYS_HEADERs to
 *              NULL
 *              Five bytes saved is five bytes earned
 *
 * Revision 3.3  1995/12/14  08:25:07  jjohnson
 * CSCdi44148:  rationalize snmp library services
 *
 * Revision 3.2  1995/11/17  09:36:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:58:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/28  09:26:59  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.1  1995/06/07  21:01:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1993 by SNMP Research, Incorporated.
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
#include "../snmp/snmp_api.h"
#include "sr_udpmib2.h"
#include "sr_udpmib2-mib.h"

#include "packet.h"
#include "ip.h"

void
init_udpmib2 (subsystype *subsys)
{
    load_mib(udpmib2_OidList, udpmib2_OidListNum);
    load_oid(udpmib2_oid_table);
}








/*---------------------------------------------------------------------
 * Retrieve data from the udp family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
udp_get(
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = -1;
    void            *dp;
    udp_t           *data = NULL;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 1 and the single
     * instance element be 0.
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
    if ( (arg == -1) || (data = k_udp_get(serialNum, contextInfo, arg)) == NULL) {
        arg = -1;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_udpInDatagrams
    case I_udpInDatagrams:
	dp = &data->udpInDatagrams;
	break;
#endif /* I_udpInDatagrams */

#ifdef I_udpNoPorts
    case I_udpNoPorts:
	dp = &data->udpNoPorts;
	break;
#endif /* I_udpNoPorts */

#ifdef I_udpInErrors
    case I_udpInErrors:
	dp = &data->udpInErrors;
	break;
#endif /* I_udpInErrors */

#ifdef I_udpOutDatagrams
    case I_udpOutDatagrams:
	dp = &data->udpOutDatagrams;
	break;
#endif /* I_udpOutDatagrams */

    default:
	return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &ZeroOid, dp));

}

/*---------------------------------------------------------------------
 * Retrieve data from the udpEntry family. This is
 * performed in 3 steps:
 *
 *   1) Test the validity of the object instance.
 *   2) Retrieve the data.
 *   3) Build the variable binding (VarBind) that will be returned.
 *---------------------------------------------------------------------*/
VarBind *
udpEntry_get(
    OID             *incoming,
    ObjectInfo      *object,
    int             searchType,
    ContextInfo     *contextInfo,
    int             serialNum)
{
    int             instLength = incoming->length - object->oid.length;
    int             arg = object->nominator;
    void            *dp;
    udpEntry_t      *data = NULL;
    unsigned long   buffer[5];
    OID             inst;
    int             carry;
    unsigned long   udpLocalAddress;
    long            udpLocalPort;


    /*
     * Check the object instance.
     *
     * An EXACT search requires that the instance be of length 5 
     *
     * A NEXT search requires that the requested object does not
     * lexicographically precede the current object type.
     */

    if (searchType == EXACT) {
	if (instLength != 5) {
	    return((VarBind *) NULL);
        }
        carry = 0;
    } else {
        carry = 1;
    }

    if ( (InstToInt(incoming, 4 + object->oid.length, &udpLocalPort, searchType, &carry)) < 0 ) {
	arg = -1;
    }

    if ( (InstToIP(incoming, 0 + object->oid.length, &udpLocalAddress, searchType, &carry)) < 0 ) {
	arg = -1;
    }

    if (carry) {
	arg = -1;
    }

    /*
     * Retrieve the data from the kernel-specific routine.
     */
    if ( (arg == -1) || (data = k_udpEntry_get(serialNum, contextInfo, arg ,searchType, udpLocalAddress, udpLocalPort)) == NULL) {
        arg = -1;
    } else {
	/*
	 * Build instance information
	 */
        inst.oid_ptr = buffer;
        inst.length = 5;
        inst.oid_ptr[0] = (data->udpLocalAddress >> 24) & 0xff;
        inst.oid_ptr[1] = (data->udpLocalAddress >> 16) & 0xff;
        inst.oid_ptr[2] = (data->udpLocalAddress >> 8) & 0xff;
        inst.oid_ptr[3] = (data->udpLocalAddress >> 0) & 0xff;
        inst.oid_ptr[4] = data->udpLocalPort;
    }

    /*
     * Build the the variable binding for the variable that will be returned.
     */

    switch (arg) {

#ifdef I_udpLocalAddress
    case I_udpLocalAddress:
	dp = MakeOctetString((unsigned char *) &data->udpLocalAddress, 4L);
	break;
#endif /* I_udpLocalAddress */

#ifdef I_udpLocalPort
    case I_udpLocalPort:
	dp = &data->udpLocalPort;
	break;
#endif /* I_udpLocalPort */

    default:
	return ((VarBind *) NULL);

    }      /* switch */

    return (MakeVarBind(object, &inst, dp));

}




udp_t *
k_udp_get(
     int serialNum,
     ContextInfo *contextInfo,
     int nominator)
{
    static udp_t udpData;

    udpData.udpInDatagrams = udp_traffic.inputs;
    udpData.udpNoPorts = udp_traffic.noport;
    udpData.udpInErrors = udp_traffic.checksumerr;
    udpData.udpOutDatagrams = udp_traffic.outputs;

    return(&udpData);
}

udpEntry_t *
k_udpEntry_get(
   int serialNum,
   ContextInfo *contextInfo,
   int nominator,
   int searchType,
   unsigned long udpLocalAddress,
   long udpLocalPort)
{
    static udpEntry_t udpEntryData;

    ipsocktype *sock;
    ipsocktype *best;

    best = NULL;

    for (sock = (ipsocktype *)IPsocketQ.qhead; sock; sock = sock->next) {

	/* reject all non-udp sockets */
	if (sock->protocol != UDP_PROT)
	    continue;

	/* if we got what we want then terminate the loop */
	if ((sock->laddress.ip_addr == udpLocalAddress) &&
	    (sock->lport == udpLocalPort)) {
	    best = sock;
	    break;
	}

	/* if we're looking for an exact match, this record isn't it */
	if (searchType == EXACT)
	    continue;

	/* see if this record index is "big" enough */
	if ((sock->laddress.ip_addr < udpLocalAddress) ||
	    ((sock->laddress.ip_addr == udpLocalAddress) &&
	     (sock->lport < udpLocalPort)))
	    continue;

	/* see if this record index is "better" */
	if ((best) &&
	    ((sock->laddress.ip_addr > best->laddress.ip_addr) ||
	     ((sock->laddress.ip_addr == best->laddress.ip_addr) &&
	      (sock->lport > best->lport))))
	    continue;

	best = sock;
    }

    if (best == NULL)
	return (NULL);

    udpEntryData.udpLocalAddress = best->laddress.ip_addr;
    udpEntryData.udpLocalPort = best->lport;

    return(&udpEntryData);
}


/*
 * udpmib2 subsystem header
 */

#define MAJVERSION_udpmib2 1
#define MINVERSION_udpmib2 0
#define EDITVERSION_udpmib2 0

SUBSYS_HEADER(udpmib2,
              MAJVERSION_udpmib2,
              MINVERSION_udpmib2,
              EDITVERSION_udpmib2,
              init_udpmib2,
              SUBSYS_CLASS_MANAGEMENT,
              subsys_req_iphost,
              NULL);
      
