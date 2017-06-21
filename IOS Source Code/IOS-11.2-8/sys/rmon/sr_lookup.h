/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_lookup.h,v 3.2 1995/11/17 18:39:39 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_lookup.h,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_lookup.h,v $
 * Revision 3.2  1995/11/17  18:39:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:24  shaker
 * Bump version numbers from 2.x to 3.x.
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

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#ifndef _lookup_h
#define	_lookup_h

#define T_uint 0
#define T_octet 1
#define T_oid 2
#ifdef SR_FTL
#define	T_ieeeMacAddr	3
#else	/* SR_FTL */
#define	T_ieeeMacAddr	T_octet
#endif	/* SR_FTL */

typedef union _anytype {
    unsigned long uint_val;
    OctetString *octet_val;
    OID         *oid_val;
} anytype_t;

typedef struct _Index {
    short offset;
    short type;
    anytype_t value;
} Index_t;

/* public functions */
int CmpOctetStrings SR_PROTOTYPE((struct _OctetString *o1,struct _OctetString *o2));
int TableInsert SR_PROTOTYPE((union _anytype ***pT  ,int *pTsize,struct _Index *I,int Isize,int RowSize));
int TableLookup SR_PROTOTYPE((union _anytype **T ,int Tsize,struct _Index *I,int Isize,int searchType));

#endif	/* _lookup_h */
