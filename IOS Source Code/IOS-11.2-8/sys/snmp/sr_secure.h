/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_secure.h,v 3.2.62.1 1996/03/18 21:49:00 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_secure.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_secure.h,v $
 * Revision 3.2.62.1  1996/03/18  21:49:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  12:34:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  17:15:00  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  19:00:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:27  hampton
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

/*
 *  Revision History:
 *
 * secure.h,v
 * Revision 1.19  1994/10/28  17:13:49  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.18  1994/10/26  21:42:41  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.17  1994/10/06  13:46:03  pearson
 * Added support for the IPX transport domain
 *
 * Revision 1.16  1994/09/28  22:06:04  reid
 * Changed the noPrivOID declaration to have an EXTDECL in front of it again.
 *
 * Revision 1.15  1994/06/24  15:44:51  hecht
 * Added the EXTDECL macro to the definitions for the global OIDs so they are
 * only globally declared once. All declarations from the noPrivOID to the
 * last_clock_write variable were changed.
 *
 * Revision 1.14  1994/05/25  19:20:27  levi
 * Added extra sets of pointer variables for switching between manager and
 *   agent 'context' in a dual role entity.  Also added prototypes for
 *   SetAgentContext and SetManagerContext.
 *
 * Revision 1.13  1994/05/13  16:37:43  hecht
 * Removed the clock_vals array and made it an element of the PARTY_TABLE
 * structure called clockVal. Eliminated the #define for MAX_CLOCK_VALS.
 *
 * Revision 1.12  1994/03/30  18:59:38  levi
 * Updates for WINDOWS port of reqlib and scriptlib, also added ./script/win,
 * a utility to run a script under WINDOWS
 *
 * Revision 1.11  1994/03/29  15:23:32  reid
 * added unsigned long cast around 0xffffffff
 *
 * Revision 1.10  1994/03/02  18:11:46  hecht
 * Added #ifndef SR_UNSECURABLE and #ifndef SR_NO_PRIVACY around des function
 * prototypes.
 *
 * Revision 1.9  1994/02/14  16:40:31  pearson
 * Updated copyright notices for 1994.
 * AMP
 *
 * Revision 1.8  1994/01/25  20:06:01  reid
 * changed prototype for GetViewIndex to return a long instead of an int
 *
 * Revision 1.7  1994/01/24  21:54:45  reid
 * chaged #include "proto.h" to #include "sr_proto.h"
 *
 * Revision 1.6  1994/01/07  15:48:51  reid
 * *** empty log message ***
 *
 * Revision 1.4  1994/01/05  16:01:19  reid
 * Added support for function prototypes if SR_WINDOWS is defined
 *
 * Revision 1.3  1993/12/15  18:41:36  battle
 * Fixed VXWORKS problems.
 *
 *
 *  16-Feb-93 KWK
 *     Added comment on TPort.
 *
 *  24-Mar-93 MGH
 *  Started converting internal representation of partyTDomain, partyCloneFrom
 *  and contextLocalTime to integers (see party and context table).
 *
 *  30-Mar-93 MGH
 *  Added global declarations for clock_vals and last_clock_write, #define
 *  for MAX_CLOCK_VALS.
 *
 *  10-May-93 MGH
 *  Changed all references to EXPORT to the new compliance levels in RFC 1447.
 *
 *  2-June-93 MGH
 *  Changed all ANSI prototypes for the SNMPv2 i/o routines to have no
 *  parameters.
 *
 *  14-June-93 MGH
 *  Changed the party, acl, view, and context structures.  Added pointer
 *  arrays for each table (PARTY_TABLE, V2_CONTEXT_TABLE, V2_ACL, and
 *  V2_VIEW).  Also added global head and tail pointers for each table
 *  and deleted the max_* variables.
 *
 *  17-June-93 MGH
 *  Added function prototypes for RecomputeViewVectors() and
 *  ComputeViewVectors().
 *
 *  25-June-93 MGH
 *  Deleted MAX_PARTIES, ... because they are not needed anymore.  Changed
 *  the name of the PARTIES structure to PARTY, the CONTEXTS structure to
 *  CONTEXT and the MIB_VIEWS structure to MIB_VIEW. Changed the name of the
 *  context table from mib_contexts to contexts. Changed the name of the
 *  num_acl variable to num_acls.
 *
 *  12-July-93 MGH & JDC
 *  Added new structures to support the new manager configuration file format
 *  (CLUSTER, TRIOS).  Changed some function prototypes and added some
 *  #defines for SR_*_OPERATION.
 *
 *  19-July-93 MGH
 *  Changed #ifndef SNMPD to #ifndef SR_AGENT
 *
 *  20-July-1992 DLB
 *  Changed MAX_CLOCK_VALUE to be expressed in hex for clarity and to shut
 *  up gcc (it warns when a decimal integer exceeds 2^31).
 */

#ifndef _SECURE_H_
#define _SECURE_H_

#include <ctype.h>

#include "sr_proto.h"

#ifndef _toupper
#define _toupper(c)      ((c)-'a'+'A')
#endif


#ifdef  MAIN
#define EXTDECL
#else
#define EXTDECL extern
#endif

#define SR_READ_OPERATION  1
#define SR_WRITE_OPERATION 2
#define SR_ADMIN_OPERATION 3

#define SR_MAX_INDEX    65535
#define SR_MAX_TIMEOUT  300
#define MAX_CLOCK_VALUE ((unsigned long) 0xffffffff)
#define TIME_WARP       86400

#define MANAGER         1	/* Used in InitParties */
#define AGENT           2	/* Used in InitParties */

#define TADDR_LEN       6
#define KEY_LENGTH      16
#define MASK_LENGTH     16	/* MGH 25-Jun-92 */

#define IS_VALID   1
#define IS_INVALID 2
#define INCLUDED   1
#define EXCLUDED   2
#define INVALID    3

/* read flags (access and variable) */
#define SR_DISCARD   -1
#define SR_NONE       0

#ifdef SR_READ_ONLY
#undef SR_READ_ONLY
#endif				/* SR_READ_ONLY */

#define SR_READ_ONLY  0x20

#ifdef SR_READ_WRITE
#undef SR_READ_WRITE
#endif				/* SR_READ_WRITE */

#define SR_READ_WRITE 0x21

#ifdef SR_WRITE_ONLY
#undef SR_WRITE_ONLY
#endif				/* SR_WRITE_ONLY */

#define SR_WRITE_ONLY SR_READ_WRITE

/* tdomain types */

/* Values for ACL privileges */
#define GET_MASK           1
#define GET_NEXT_MASK      2
#define GET_RESPONSE_MASK  4
#define SET_MASK           8
#define TRAP_MASK          16
#define BULK_MASK          32
#define INFORM_MASK        64
#define SNMPv2_TRAP_MASK   128
#define TOTAL_MASK         255	/* Upper bound for acl mask */

/* proxy types */
#define PROXY_FALSE 0
#define PROXY_TRUE  1

#define NOAUTH_LENGTH  10
#define NOPRIV_LENGTH  10

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
#define DESPRIV_LENGTH 10
#endif				/* SR_NO_PRIVACY */

#define MD5AUTH_LENGTH 10
#endif				/* SR_UNSECURABLE */

#define SNMPUDPDOMAIN_LENGTH     7
#define RFC1157_OID_LENGTH       8
#define RFC1157NOAUTH_OID_LENGTH 8

#ifdef	SR_SNMPv2
#define CURRENT_TIME_LENGTH 10
#define RESTART_TIME_LENGTH 10
#define CACHE_TIME_LENGTH   10
#endif				/* SR_SNMPv2 */

/* Internal representations for partyTDomain */
#define SNMP_UDP_DOMAIN 1
#define RFC_1157_DOMAIN 2

/* Internal representations for contextLocalTime */
#define C_CURRENT_TIME -1
#define C_RESTART_TIME -2

EXTDECL OID            *noPrivOID;

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
EXTDECL OID            *desPrivOID;
#endif				/* SR_NO_PRIVACY */
EXTDECL OID            *md5AuthOID;
#endif				/* SR_UNSECURABLE */
EXTDECL OID            *noAuthOID;
EXTDECL OID            *snmpUDPDomainOID;
EXTDECL OID            *rfc1157DomainOID;
EXTDECL OID            *rfc1157noAuthOID;

#ifdef	 SR_SNMPv2
EXTDECL OID            *currentTimeOID;
EXTDECL OID            *restartTimeOID;
EXTDECL OID            *cacheTimeOID;
#endif				/* SR_SNMPv2 */

EXTDECL unsigned long   last_clock_write;

typedef struct _PARTY {
    int             privs;
    int             domain;	/* snmp/udp/ip or snmpv2/udp/ip or etc */
    OID            *partyIdentity;
    int             partyTDomain;
    unsigned short  partyTPort;	/* In Network-Byte order */
    OctetString    *partyTAddress;
    long            partyMaxMessageSize;
    int             partyAuthProtocol;
    unsigned long   partyAuthClock;
    OctetString    *partyAuthPublic;
    OctetString    *partyAuthPrivate;
    long            partyAuthLifetime;
    int             partyPrivProtocol;
    OctetString    *partyPrivPublic;
    OctetString    *partyPrivPrivate;
    int             partyDiscriminator;

    long            partyLocal;
    long            partyStatus;
    unsigned long   snmpTrapNumbers;
    long            partyIndex;
    long            partyStorageType;
    int             partyCloneFrom;
    char	   *partyHandle;
    int		    access_list;
    struct _PARTY  *next_ptr;	/* MGH 10-June-93 */
}               PARTY;

typedef struct _PARTY_TABLE {
    PARTY          *party;
    unsigned long   last_active;

    unsigned long   clockVal;
}               PARTY_TABLE;

/*
 * Global pointers, these are what are used by the libraries.
 */
EXTDECL PARTY_TABLE *parties;
EXTDECL PARTY       *v2party_head_ptr;
EXTDECL PARTY       *v2party_tail_ptr;
EXTDECL int          num_parties;
EXTDECL int          max_party_index;


/* MGH 14-Dec-92 */
typedef struct _CONTEXT {
    OID             *contextIdentity;
    long             contextIndex;
    long             contextLocal;
    long             contextViewIndex;
    OctetString     *contextLocalEntity;
    int              contextLocalTime;
    OID             *contextProxyDstParty;
    OID             *contextProxySrcParty;
    OID             *contextProxyContext;
    long             contextStorageType;
    long             contextStatus;
    char            *contextHandle;
    struct _CONTEXT *next_ptr;
}               CONTEXT;

typedef struct _V2_CONTEXT_TABLE {
    CONTEXT        *context;
    unsigned long   last_active;
}               V2_CONTEXT_TABLE;

/* Global */
EXTDECL V2_CONTEXT_TABLE *contexts;
EXTDECL CONTEXT          *v2context_head_ptr;
EXTDECL CONTEXT          *v2context_tail_ptr;
EXTDECL int               num_contexts;
EXTDECL int               max_context_index;


/* JDC July 93 */
typedef struct _TRIOS {
    int             srcPartyIndex;
    int             dstPartyIndex;
    int             contextIndex;
}               TRIOS;

typedef struct _CLUSTER {
    OctetString    *cluster_name;
    int             readOperationAuthType;
    int             writeOperationAuthType;
    int             adminOperationAuthType;
    TRIOS           readOperation;
    TRIOS           writeOperation;
    TRIOS           noAuthClkSync;
    TRIOS           md5AuthClkSync;
    TRIOS           adminOperation;
    int             cs_party_index[10];
}               CLUSTER;


/* MGH 12-Jun-92 */
typedef struct _ACCESS {
    long            aclTarget;	        /* dstParty's partyIndex */
    long            aclSubject;	        /* srcParty's partyIndex */
    long            aclResources;	/* context's contextIndex */
    long            aclPrivileges;
    long            aclStorageType;	/* storage type */
    long            aclStatus;	        /* now of type RowStatus */
    struct _ACCESS *next_ptr;
}               ACCESS;

typedef struct _V2_ACL {
    ACCESS         *acl;
    unsigned long   last_active;
}               V2_ACL;

/* Global */
EXTDECL V2_ACL *access_control;
EXTDECL ACCESS *v2acl_head_ptr;
EXTDECL ACCESS *v2acl_tail_ptr;
EXTDECL int     num_acls;


/* MGH 9-Dec-92 */
typedef struct _MIB_VIEW {
    long              viewIndex;
    OID              *viewSubtree;
    OctetString      *viewMask;
    long              viewType;
    long              viewStorageType;
    long              viewStatus;
    char             *viewHandle;
    struct _MIB_VIEW *next_ptr;
}               MIB_VIEW;

typedef struct V2_VIEW {
    MIB_VIEW       *mib_view;
    unsigned long   last_active;
}               V2_VIEW;

/* Global */
EXTDECL V2_VIEW  *party_views;
EXTDECL MIB_VIEW *v2view_head_ptr;
EXTDECL MIB_VIEW *v2view_tail_ptr;
EXTDECL int       num_views;


/* secure.c */
extern	void InitSecure SR_PROTOTYPE((void));
extern	int GetPartyEntry SR_PROTOTYPE((struct _OID *party));
extern	int GetPartyPos SR_PROTOTYPE((struct _OID *party));
extern	int GetPartyLocation SR_PROTOTYPE((long acl_index));
extern	int GetPartyIndex SR_PROTOTYPE((struct _OID *party, int location));
extern	int GetContextEntry SR_PROTOTYPE((long context));
extern	long GetViewIndex SR_PROTOTYPE((struct _OID *context));
extern	int GetContextIndex SR_PROTOTYPE((struct _OID *context));
extern	int GetContextLocation SR_PROTOTYPE((struct _OID *context, int location));
extern	int GetAclIndex SR_PROTOTYPE((long resource, long source, long dest));
extern	int GetAclEntry SR_PROTOTYPE((long source, long dest));
extern	int CheckMIBView SR_PROTOTYPE((struct _OID *object_instance, long view_index));
extern	int CheckClassMIBView SR_PROTOTYPE((struct _OID *object_class, long view_index));
extern	int GetViewEntry SR_PROTOTYPE((long view_index));
extern	void PartyInsertSort SR_PROTOTYPE((struct _PARTY *party_ptr));
extern	void FreePartyElement SR_PROTOTYPE((struct _PARTY *party_ptr));
extern	void ContextInsertSort SR_PROTOTYPE((struct _CONTEXT *context_ptr));
extern	void FreeContextElement SR_PROTOTYPE((struct _CONTEXT *context_ptr));
extern	int AclInsertSort SR_PROTOTYPE((struct _ACCESS *acl_ptr));
extern	void FreeAclElement SR_PROTOTYPE((struct _ACCESS *acl_ptr));
extern	int ViewInsertSort SR_PROTOTYPE((struct _MIB_VIEW *view_ptr));
extern	void FreeViewElement SR_PROTOTYPE((struct _MIB_VIEW *view_ptr));
extern	int RecomputeViewVectors SR_PROTOTYPE((long viewIndex));
extern	int ComputeViewVectors SR_PROTOTYPE((void));
extern	void UpperIt SR_PROTOTYPE((char *ptr));
extern	unsigned long BaseClock SR_PROTOTYPE((void));
extern	int DeleteAclElement SR_PROTOTYPE((struct _ACCESS *acl_ptr));
extern	void DeleteContextElement SR_PROTOTYPE((struct _CONTEXT *context_ptr));
extern	void DeletePartyElement SR_PROTOTYPE((struct _PARTY *party_ptr));
extern	int DeleteViewElement SR_PROTOTYPE((struct _MIB_VIEW *view_ptr));
extern	int CheckSecurityTables SR_PROTOTYPE((unsigned long now));

/* readv2.c */
extern	void InitParties SR_PROTOTYPE((void));
extern	void EchoCheck SR_PROTOTYPE((void));
extern	void InitAclEntries SR_PROTOTYPE((void));
extern	void InitViewEntries SR_PROTOTYPE((void));
extern	void InitContextEntries SR_PROTOTYPE((void));

/* writev2.c */
extern	int WriteParties SR_PROTOTYPE((void));
extern	int WriteAclFile SR_PROTOTYPE((void));
extern	int WriteViewFile SR_PROTOTYPE((void));
extern	int WriteContextFile SR_PROTOTYPE((void));

#ifndef SR_UNSECURABLE
#ifndef SR_NO_PRIVACY
/* des.c */
extern	int desinit SR_PROTOTYPE((int mode));
extern	int desdone SR_PROTOTYPE((void));
extern	int dessetkey SR_PROTOTYPE((char *key));
extern	int endes SR_PROTOTYPE((char *block));
extern	int dedes SR_PROTOTYPE((char *block));
#endif /* SR_UNSECURABLE */
#endif /* SR_NO_PRIVACY */

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* _SECURE_H_ */
