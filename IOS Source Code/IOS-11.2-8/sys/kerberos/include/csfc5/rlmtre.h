/* $Id: rlmtre.h,v 3.2 1995/11/17 17:48:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/rlmtre.h,v $
 *------------------------------------------------------------------
 * rlmtre.h - Realm tree definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rlmtre.h,v $
 * Revision 3.2  1995/11/17  17:48:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_RLMTRE_H_
#define _CSFC5_RLMTRE_H_   "%W% %G%"
/*.original func-proto.h 3.8 5/22/94*/

/*
 * Copyright (C) 1993-1994 CyberSAFE Corporation.
 * Copyright 1990,1991 by the Massachusetts Institute of Technology.
 * All Rights Reserved.
 */

#ifndef CSFC5_REALM_BRANCH_CHAR
#define CSFC5_REALM_BRANCH_CHAR '.'
#endif


typedef struct _csfC5_realmTree {
	csf_size_t nrealm;
	csfC5_prnId *realms;
} csfC5_realmTree;

void csfC5free_realmTree( csfC5_realmTree * );


csf_sts csfC5get_realmTree (
	const csfC5_data *,
	const csfC5_data *,
	csfC5_realmTree *,
	char
);

#endif
