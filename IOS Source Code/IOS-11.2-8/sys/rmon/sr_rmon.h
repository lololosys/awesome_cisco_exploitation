/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_rmon.h,v 3.3 1995/11/17 18:39:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rmon/sr_rmon.h,v $
 *------------------------------------------------------------------
 * Remote Monitoring MIB Support
 *
 * August 1995, SNMP Research
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sr_rmon.h,v $
 * Revision 3.3  1995/11/17  18:39:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:22:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:03:26  shaker
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


/* Id: rmon.h,v 1.7 1994/12/21 16:06:59 levi Exp  */

/*
 * Log: rmon.h,v 
 * Revision 1.7  1994/12/21  16:06:59  levi
 * Updates to make unix version build
 *
 * Revision 1.6  1994/11/17  21:49:52  pearson
 * Conditionally compiled a comment
 *
 * Revision 1.5  1994/10/28  17:30:52  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.4  1994/10/26  21:57:24  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.3  1994/09/22  14:13:41  battle
 * added #include of tmq.h
 *
 * Revision 1.2  1994/08/11  14:56:21  pearson
 * Cleaned up some debugging code, added the SR_SIF, SR_FTC, and SR_FTL
 * compilation switches.
 *
 * Revision 1.1  1994/08/08  19:31:43  pearson
 * First checkin of the RMON sources.
 *
 * Revision 1.2  1993/11/10  19:30:45  battle
 * Additions to emanate to add shared library support and cvs headers.
 *
 */

#ifndef _RMON_H_
#define _RMON_H_



#include "sr_lookup.h"

/* define some ifTypes */
#define	IFT_CSMACD		6	/* "DIX" ethernet */
#define	IFT_ISO8023		7	/* 802.3 ethernet */

/* the entrystatus textual convention */
#define	ES_UNINITIALIZED	0	/* implementation trick */
#define	ES_VALID		1
#define	ES_CREATEREQUEST	2
#define	ES_UNDERCREATION	3
#define	ES_INVALID		4

/* configuration macros */
#define MAXETHERS 10
#define MAXTOKENS 0
#define MAXFDDSI 0
#define MAX_ROW_CREATION_TIME ((long) 30000L)	/* 5 minutes in centiseconds */
#define	MINLOGENTRIES	10		/* required before lru deletion */
#define	MINHOSTENTRIES	20		/* required before lru deletion */

/* general macros */
#define	ETHERMACADDRLEN	6		/* ethernet address length */

#ifdef SR_FTL	/* Fast Table Lookup */
#define IEEE_MAC_ADDR	0x50		/* must not conflict w/ snmp.h!!! */
#else	/* SR_FTL */
#define IEEE_MAC_ADDR	OCTET_PRIM_TYPE
#endif	/* SR_FTL */

#ifdef	SR_SIF
/* always return true on data comparisons */
#define	CmpDataSource(s1, s2)	0
#endif	/* SR_SIF */

#include "../snmp/sr_tmq.h"


typedef struct _Ether {
    int             fd;
    long            ifIndex;
}               Ether_t;

/* this structure describes table entry properties */
typedef struct _snmpTypeS {
    int	type;		/* data type */
    int	readaccess;	/* read or read-write */
    int indextype;	/* index priority or -1 if not an index */
} SnmpType;

/* this structure describes a table */
typedef struct _rmonTableS {
    void **tp;		/* pointer to the table */
    Index_t *tip;	/* pointer(s) to table index structures */
    int nitems;		/* number of table entries */
    int nindices;	/* number of table indices */
    int rowsize;	/* size of a table element */
} RmonTable;

/* this structure defines state for received packets */
typedef struct _packetStateS {
    unsigned char *p;	/* pointer to the packet */
    int size;		/* byte count, including fcs */
    int error;		/* nonzero if FCS or alignment error */
    OID *source;	/* data source; that is, oid form of ifindex.XXX */
    int collisions;	/* collision count since the last packet */
    int drops;		/* drop count since the last packet */
    int orderapprox;	/* this packet's order in the sequence is uncertain */
    unsigned long id;	/* unique count for this interface */
    sys_timestamp timestamp; /* milliseconds since system boot */
} PacketState;

#define ROW_NONEXISTANT 0
#define ROW_EXISTS      1
#define ROW_VALID       2


#include "sr_rmonprot.h"

/* helper functions provided by the target system */
int	HeapLowMem SR_PROTOTYPE((void));

#endif				/* _RMON_H_ */
