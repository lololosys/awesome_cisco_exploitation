/* $Id: netinadr.c,v 3.2.60.1 1996/04/19 15:12:04 che Exp $
 * $Source: /release/112/cvs/Xsys/kerberos/c5libs/net/netinadr.c,v $
 *------------------------------------------------------------------
 * netinadr.c - Makes and gets INET FADDR
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netinadr.c,v $
 * Revision 3.2.60.1  1996/04/19  15:12:04  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.1.2.2  1996/04/02  08:13:37  carrel
 * cvs fixes
 *
 * Revision 3.1.2.1  1996/03/05  04:46:31  che
 * Branch: IosSec_branch
 * Kerberos Phase 2 commit.
 *
 * Revision 3.2  1995/11/17  17:45:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original full_ipadr.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#include <csfc5/header.h>


csf_sts csfC5netinetMakeFaddr (
	csfC5_netINetAddr adr,
	csfC5_netINetport port,
	csfC5_netHAddr *outaddr
){
	csf_ui32 smushaddr = (csf_ui32) adr; /* already in net order */
	csf_ui16 smushport = (csf_ui16) port; /* ditto */
	csf_o *marshal;
	csfC5_netHAddrType temptype;
	csf_ui32 templength;
	csf_size_t dl;

	csfC5clear_netHAddr( outaddr );

	dl = sizeof(smushaddr) + sizeof(smushport) +
		4*sizeof(temptype) + 2*sizeof(templength);
	outaddr->haddr_data.dp = csfCcmalloc( dl );
	if (!outaddr->haddr_data.dp) {
		return CSF_STS_ENOMEM;
	}
	outaddr->haddr_type = CSFC5cx_HADDR_T_PORT;
	outaddr->haddr_data.dl = dl;

	marshal = outaddr->haddr_data.dp;

	*marshal++ = 0x00;
	*marshal++ = 0x00;
	temptype = csfNEThton16(CSFC5c_HADDR_T_INET);
	(void) csfCmemcpy( marshal, &temptype, sizeof(temptype) );
	marshal += sizeof(temptype);

	templength = csfNEThton32(sizeof(smushaddr));
	(void) csfCmemcpy( marshal, &templength, sizeof(templength) );
	marshal += sizeof(templength);

	(void) csfCmemcpy( marshal, &smushaddr, sizeof(smushaddr) );
	marshal += sizeof(smushaddr);

	*marshal++ = 0x00;
	*marshal++ = 0x00;
	temptype = csfNEThton16(CSFC5cx_HADDR_T_IPPORT);
	(void) csfCmemcpy( marshal, &temptype, sizeof(temptype) );
	marshal += sizeof(temptype);

	templength = csfNEThton32(sizeof(smushport));
	(void) csfCmemcpy( marshal, &templength, sizeof(templength) );
	marshal += sizeof(templength);

	(void) csfCmemcpy( marshal, &smushport, sizeof(smushport) );
	marshal += sizeof(smushport);

	return CSF_STS_OK;
}


csf_sts csfC5netinetGetFaddr (
	const csfC5_netHAddr *inaddr,
	csfC5_netINetAddr *adr,
	csfC5_netINetport *port
){
	csf_ui32 smushaddr;
	csf_ui16 smushport;
	csf_o *marshal;
	csfC5_netHAddrType temptype;
	csf_ui32 templength;

	if (inaddr->haddr_type != CSFC5cx_HADDR_T_PORT) {
		return CSFC5_STS_PROG_ATYPE_NOSUPP;
	}

	{
		/* Yes, we could jam this into one test and not use "t"... */
		int t;
		t = sizeof(smushaddr) + sizeof(smushport) +
			4 * sizeof(temptype) + 2 * sizeof(templength);
		if (inaddr->haddr_data.dl != t) {
			return CSFC5_STS_PROG_ATYPE_NOSUPP;
		}
	}

	marshal = inaddr->haddr_data.dp;

	marshal+=2;
	(void) csfCmemcpy( &temptype, marshal, sizeof(temptype) );
	marshal += sizeof(temptype);
	if (temptype != csfNEThton16( CSFC5c_HADDR_T_INET )) {
		return CSFC5_STS_PROG_ATYPE_NOSUPP;
	}

	(void) csfCmemcpy( &templength, marshal, sizeof(templength) );
	marshal += sizeof(templength);
	if (templength != csfNEThton32( sizeof(smushaddr) )) {
		return CSFC5_STS_PROG_ATYPE_NOSUPP;
	}

	(void) csfCmemcpy( &smushaddr, marshal, sizeof(smushaddr) );
	/* leave in net order */
	marshal += sizeof(smushaddr);

	marshal+=2;
	(void) csfCmemcpy( &temptype, marshal, sizeof(temptype) );
	marshal += sizeof(temptype);
	if (temptype != csfNEThton16( CSFC5cx_HADDR_T_IPPORT )) {
		return CSFC5_STS_PROG_ATYPE_NOSUPP;
	}

	(void) csfCmemcpy( &templength, marshal, sizeof(templength) );
	marshal += sizeof(templength);
	if (templength != csfNEThton32( sizeof(smushport) )) {
		return CSFC5_STS_PROG_ATYPE_NOSUPP;
	}

	(void) csfCmemcpy( &smushport, marshal, sizeof(smushport) );
	/* leave in net order */

	*adr = (csfC5_netINetAddr) smushaddr;
	*port = (csfC5_netINetport) smushport;

	return CSF_STS_OK;
}


csf_sts csfC5netinetMakePortaddr (
	const csfC5_netHAddr *addr,
	void const *ptr,
	csfC5_netHAddr *outaddr
){
	csfC5_netINetAddr adr;
	csfC5_netINetport port;
	csf_sts rsts;

	if (addr->haddr_type != CSFC5c_HADDR_T_INET) {
		return CSFC5_STS_PROG_ATYPE_NOSUPP;
	}
	port = *(csfC5_netINetport *)ptr;
	csfCmemcpy( &adr, addr->haddr_data.dp, sizeof(adr) );
	rsts = csfC5netinetMakeFaddr( adr, port, outaddr );
	return rsts;
}
