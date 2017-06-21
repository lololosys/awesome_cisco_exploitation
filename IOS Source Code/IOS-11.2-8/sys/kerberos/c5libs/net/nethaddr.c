/* $Id: nethaddr.c,v 3.2 1995/11/17 17:45:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/c5libs/net/nethaddr.c,v $
 *------------------------------------------------------------------
 * nethaddr.c - Functions to clear nethaddr structures
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nethaddr.c,v $
 * Revision 3.2  1995/11/17  17:45:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:19:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define csfSRCID "%W% %G%"
/*.original f_addr.c 3.6 6/3/94*/
/*.original f_addrs.c 3.4 6/3/94*/
/*.original copy_addrs.c 3.2 5/22/94*/
/*.original addr_comp.c 3.1 12/29/93*/
/*.original addr_srch.c 3.1 12/29/93*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990 by the Massachusetts Institute of Technology.
 * All rights reserved.
 */

#include <csfc5/header.h>


void csfC5clear_netHAddr (
	csfC5_netHAddr *val
){
	if (val) {
		csfCmemzero( val, sizeof(*val) );
	}
}

void csfC5free_netHAddr (
	csfC5_netHAddr *val
){
	if (val) {
		csfC5free_data( &val->haddr_data );
		csfC5clear_netHAddr( val );
	}
}


void csfC5freeC_netHAddr (
	csfC5_netHAddr **val
){
	if (val) {
		csfC5free_netHAddr( *val );
		csfCfree( *val );
		*val = NULL;
	}
}


void csfC5freeC_netHAddrS (
	csfC5_netHAddr ***val
){
	if (val && *val) {
		csfC5_netHAddr **temp;
		for (temp = *val; *temp; temp++) {
			csfC5freeC_netHAddr( temp );
		}
		csfCfree( *val );
		*val = NULL;
	}
}

csf_sts csfC5copyC_netHAddr (
	const csfC5_netHAddr *inad,
	csfC5_netHAddr **outad
){
	csfC5_netHAddr *tmpad;

	*outad = NULL;

	tmpad = (csfC5_netHAddr *) csfCmalloc( sizeof(*tmpad) );
	if (!tmpad) {
		return CSF_STS_ENOMEM;
	}

	*tmpad = *inad;
	tmpad->haddr_data.dp = csfCmalloc( inad->haddr_data.dl );
	if (!tmpad->haddr_data.dp) {
		csfCfree( tmpad );
		return CSF_STS_ENOMEM;
	}

	csfCmemcpy( tmpad->haddr_data.dp, inad->haddr_data.dp, inad->haddr_data.dl );
	*outad = tmpad;
	return CSF_STS_OK;
}


/*
 * Copy an haddr array, with fresh allocation.
 */

csf_sts csfC5copyC_netHAddrS (
	csfC5_netHAddr * const *inaddr,
	csfC5_netHAddr ***outaddr
){
	int nelems = 0;
	csf_sts rsts;
	csfC5_netHAddr **tempaddr;

	*outaddr = NULL;

	for (nelems = 0; inaddr[nelems]; ++nelems) {
		;	/* find last element */
	}

	/* one more for a null terminated list */
	tempaddr = (csfC5_netHAddr **) csfCcalloc( nelems+1, sizeof(*tempaddr) );
	if (!tempaddr) {
		return CSF_STS_ENOMEM;
	}

	for (nelems = 0; inaddr[nelems]; nelems++) {
		rsts = csfC5copyC_netHAddr( inaddr[nelems], &tempaddr[nelems] );
		if (rsts) {
			csfC5freeC_netHAddrS( &tempaddr );
			return rsts;
		}
	}

	*outaddr = tempaddr;
	return CSF_STS_OK;
}


/*
 * Append an haddr array to another haddr array, with fresh allocation.
 * Note that this function may change the value of *outaddr even if it
 * returns failure, but it will not change the contents of the list.
 */

csf_sts csfC5appendL_netHAddr (
	csfC5_netHAddr *const *inaddr,
	csfC5_netHAddr ***outaddr
){
	int nelems;
	int norigelems;
	csf_sts rsts;
	csfC5_netHAddr **tempaddr;
	csfC5_netHAddr **tempaddr2;

	if (!inaddr) {
		return CSF_STS_OK;
	}

	tempaddr2 = *outaddr;
	for (nelems = 0; inaddr[nelems]; nelems++) {
		;	/* count elements in array to be appended */
	}

	for (norigelems = 0; tempaddr2[norigelems]; norigelems++) {
		;	/* count elements in original array */
	}

	nelems += norigelems;
	++nelems;                           /* plus 1 for NULL */
	nelems *= sizeof(*tempaddr);
	tempaddr = (csfC5_netHAddr **) csfCrealloc( *outaddr, nelems );
	if (!tempaddr) {
		return CSF_STS_ENOMEM;
	}

	/* The old storage has been freed. */
	*outaddr = tempaddr;

	for (nelems = 0; inaddr[nelems]; nelems++) {
		rsts = csfC5copyC_netHAddr( inaddr[nelems], &tempaddr[norigelems + nelems] );
		if (rsts) {
			/* free the already allocated addreses */
			while (--nelems >= 0) {
				csfC5freeC_netHAddr( &tempaddr[norigelems + nelems] );
			}
			/* Try to allocate a smaller amount of memory for *outaddr.  */
			tempaddr = (csfC5_netHAddr **) csfCrealloc
				( (char *)tempaddr, (norigelems+1) * sizeof(*tempaddr) );
			if (tempaddr) {
				*outaddr = tempaddr;
			}
			return rsts;
		}
	}

	tempaddr[norigelems + nelems] = NULL;
	return CSF_STS_OK;
}


/*
 * If the two addresses are the same, return TRUE, else return FALSE
 */

csf_boolean csfC5isEq_netHAddr (
	const csfC5_netHAddr *addr1,
	const csfC5_netHAddr *addr2
){

	if (addr1->haddr_type != addr2->haddr_type) {
		return FALSE;
	}
	if( csfC5cmp_data( &addr1->haddr_data, &addr2->haddr_data ) != 0 ) {
		return FALSE;
	}

	return TRUE;

}


/*
 * if addr is listed in addrlist, or addrlist is null, return TRUE.
 * if not listed, return FALSE
 */

csf_boolean csfC5isIn_netHAddr (
	const csfC5_netHAddr *addr,
	csfC5_netHAddr * const *addrlist
){
	if (!addrlist) {
		return TRUE;
	}
	for (; *addrlist; addrlist++) {
		if (csfC5isEq_netHAddr( addr, *addrlist )) {
			return TRUE;
		}
	}
	return FALSE;
}
