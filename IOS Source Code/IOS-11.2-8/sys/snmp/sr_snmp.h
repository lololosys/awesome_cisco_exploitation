/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_snmp.h,v 3.3.62.1 1996/03/18 21:49:02 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_snmp.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmp.h,v $
 * Revision 3.3.62.1  1996/03/18  21:49:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  12:34:06  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  17:15:07  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/11/17  19:01:11  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:27  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:18:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:39  hampton
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

/* snmp.h,v 1.22 1995/02/10 21:01:29 vaughn Exp */

/*
 * snmp.h,v
 * Revision 1.22  1995/02/10  21:01:29  vaughn
 * Changed the free(p) macro from
 * Pfree((p), (long)(CurPdu == NULL? 0: TID(CurPdu))) to
 * Pfree((char *)(p), (long)((void *)CurPdu == NULL? 0: TID(CurPdu))) to avoid
 * compile warnings
 *
 * Revision 1.21  1995/02/08  19:39:36  battle
 * added include of sr_type.h
 *
 * Revision 1.20  1994/10/28  17:13:50  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.19  1994/10/26  21:42:42  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.18  1994/10/26  19:33:08  reid
 * added prototype for CmpOctetStrings
 *
 * Revision 1.17  1994/10/11  21:08:56  reid
 * don't use FAR unless PKG is defined
 *
 * Revision 1.16  1994/10/06  13:46:40  pearson
 * Moved the copyright notice to a FAR data segment
 *
 * Revision 1.15  1994/09/19  13:25:52  levi
 * Moved prototype for CatOID from snmpuser.h
 *
 * Revision 1.14  1994/08/04  19:26:57  reid
 * #define exit for SR_WINDOWS so that I can print an error message box
 * before dying.
 *
 * Revision 1.13  1994/03/03  21:58:34  battle
 * Changed copyright and softversion strings.
 *
 * Revision 1.12  1994/02/14  20:41:45  levi
 * Added prototype for FreeObjectSyntax
 *
 * Revision 1.11  1994/02/14  16:40:34  pearson
 * Updated copyright notices for 1994.
 * AMP
 *
 * Revision 1.10  1994/02/11  22:24:29  levi
 * Added prototype for CloneObjectSyntax
 *
 * Revision 1.9  1994/02/08  18:42:45  reid
 * added support for WinSock
 *
 * Revision 1.8  1994/01/24  21:54:46  reid
 * chaged #include "proto.h" to #include "sr_proto.h"
 *
 * Revision 1.7  1994/01/12  22:33:06  key
 * Added prototype for NormalizeVarBindListLengths().
 *
 * Revision 1.6  1993/12/21  16:13:21  levi
 * Changed NVTString prototypes to DisplayString.
 *
 * Revision 1.5  1993/12/20  19:29:21  levi
 * Added prototypes for NVTString functions (nvt_chk.c) to snmp.h.
 *
 * Revision 1.4  1993/11/03  17:04:24  battle
 * Changed rcsid line to comment instead of static char array.
 *
 * Revision 1.3  1993/11/03  16:51:18  battle
 * Added 1 more " * " line in the copyright.
 *
 * Revision 1.2  1993/11/03  16:49:58  battle
 * Added prototype for MakeOIDFragFromDot().
 *
 *
 * Revision History:
 *
 *  2/6/89 JDC
 *
 *  8/3/89 JDC
 *  Corrected some embedded comments
 *
 *  11/8/89 JDC
 *  Have it bring in a string if not already defined
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  31-Dec-90 JDC
 *  Merged common version for 9.5.0.8
 *
 *  01-Jan-91 JDC
 *  Added NULLIT() and LIB_ERROR family of macros
 *
 *  18-Feb-91 JDC
 *  Ditched LIB_PERROR
 *
 *  25-Apr-92 MRD
 *  Added
 *	definitions for bcopy, bcmp, and bzero
 *	definitions for CatOID, Inst2IP, etc
 *	object info structure
 *
 *  27-Jun-92 JDC
 *  SMP modifications
 *
 *  05-Aug-92 JTT
 *  Replaced #if (defined... ) with #ifdef's to work with unifdef.
 *
 *  9-Sept-92 KWK
 *  Added declaration for unsigned long for inet_addr() for PKG agent.
 *
 *  5-Oct-92 DLB
 *
 *  Changed short in several prototypes to int.  See type promotion rules
 *  of ansi C to understand why.  Each occurance is marked with the comment:
 *  int in prototype matches short in old-style definition; type promotion
 *
 *  7-Dec-92 DLB
 *  Added headers for mymalloc, rudimentary instrumented malloc library.
 *
 *  8-Dec-1992 DLB
 *    Added prototypes for ParseImplicitSequence (prse_pkt.c).
 *
 *  10-Dec-1992 DLB
 *    Added prototypes for FreeBitString() and FreeUInt64().
 *
 *  10-Dec-1992 MGH
 *  Added #defines for the enumerated values for partyTime, MemoryType,
 *  partyLocal, and RowStatus.
 *
 *  6-Jan-1993 JDC
 *  Changed the value for MGMT_COM_TYPE to A2
 *
 *  6-Jan-1993 MGH
 *  Added the U_INTEGER_32_TYPE to the list of types.
 *
 *  8-Feb-1993 MGH
 *  Added #define's for ERROR codes used in auth_lib.c and the agent.
 *
 *  10-May-1993 MGH
 *  Changed all references to EXPORT to the new compliance levels in
 *  RFC 1447.
 *
 *  26-May-1993 DLB
 *  Changed MY_MALLOC headers to THREAD_MALLOC headers.
 *
 *  01-Jun-1993 DLB
 *  Changed #definition of bcopy and friends to always call memcpy, etc.
 *
 *  11-June-1993 DAR
 *  added function prototypes for inst_lib.c
 *
 *  14-Jun-1993 DLB
 *    Removed COMPRESS_OI structure.
 *    Changed oidtype, oidstatus, and nominator fields of ObjectInfo
 *    structure to unsigned char (from int).
 *
 *  16-Jun-1993 DLB
 *    Changed the default ObjectSyntax structure from the struct to the
 *    struct with embedded union.  The OS_UNION define has gone away and
 *    been replaced with OS_STRUCT.  If defined, you get the old struct.
 *
 *  19-July-93 MGH
 *  Added a #ifndef SR_MGR block around the ObjectInfo structure since
 *  it is only used by the agent. Added a similar block around the function
 *  prototype for MakeVarBind() since it is only used by the agent.  Added
 *  a #ifndef SR_AGENT block around the function prototypes for
 *  MakeVarBindWithNull() and MakeVarBindWithValue(),
 *  new routines used by the command-line utilities and managers.
 *
 *  28-July-93 DAR
 *  Added the include file oldnames.h (if SR_OLDNAMES defined).  This 
 *  backwards compatibility include file maps old library names to new
 *  library names. 
 *
 *  04-Oct-1993 DLB
 *    Removed orphaned #define OI_V2_FLAG 1.
 *
 *  07-Oct-1993 KWK
 *  inet_addr() prototype for PKG is now in pkg\include\ip.h
 */


#ifndef _SNMP_H
#define _SNMP_H

#include "sr_type.h"
#include "sr_proto.h"
#include <string.h>

/* retrieve the local agent configuration options */
#include "sr_snmpcfg.h"

#ifndef lint


#endif				/* lint */

/*
 * guide to #defines
 * the user in the makefile will specify any or all (but at least one) of:
 * SR_SNMPv1    rfc1157
 * SR_SNMPv2	snmpv2
 *
 * the following will be generated automagically as a result, and the user
 * should NOT specify it
 * BOTH		SR_SNMPv1 or SR_SNMPv2
 *
 */

/* map old library names to new library names? */


#ifdef	BOTH

#ifdef	SR_SNMPv1
#undef	SR_SNMPv1
#endif				/* SR_SNMPv1 defined */

#define	SR_SNMPv1

#ifdef	SR_SNMPv2
#undef	SR_SNMPv2
#endif				/* SR_SNMPv2 defined */

#define	SR_SNMPv2
#endif				/* BOTH */


#ifdef	SR_SNMPv1
#define VERSION 0		/* SNMP Version 1 (0) */
#endif				/* SR_SNMPv1 */

/* choke on ANYTHING over this size (cause it will only get bigger later on). */
#define MAX_OCTET_STRING_SIZE 2048
#define MAX_OID_SIZE 128

#ifndef SR_UNSECURABLE

#ifdef	SR_SNMPv2
#define MD5_DIGEST_SIZE 16
#endif				/* SR_SNMPv2 */

#endif				/* SR_UNSECURABLE */

#ifdef TRUE
#undef TRUE
#endif				/* TRUE */

#define TRUE 1

#ifdef FALSE
#undef FALSE
#endif				/* FALSE */

#define FALSE 0

/* Universals */
#define INTEGER_TYPE           0x02

#ifdef	SR_SNMPv2
#define BIT_STRING_TYPE        0x03
#endif				/* SR_SNMPv2 */

#define OCTET_PRIM_TYPE        0x04
#define NULL_TYPE              0x05
#define OBJECT_ID_TYPE         0x06
#define SEQUENCE_TYPE          0x30

#ifdef	SR_SNMPv2
#define INTEGER_32_TYPE        INTEGER_TYPE
#endif				/* SR_SNMPv2 */

/* Applications */
#define IP_ADDR_PRIM_TYPE      0x40
#define COUNTER_TYPE           0x41
#define GAUGE_TYPE             0x42
#define TIME_TICKS_TYPE        0x43
#define OPAQUE_PRIM_TYPE       0x44

#ifdef	SR_SNMPv2
#define COUNTER_32_TYPE        COUNTER_TYPE
#define	GAUGE_32_TYPE          GAUGE_TYPE
#define	NSAP_ADDR_TYPE         0x45
#define	COUNTER_64_TYPE        0x46
#define U_INTEGER_32_TYPE      0x47
#endif				/* SR_SNMPv2 */


/* Contexts */
#define GET_REQUEST_TYPE       0xA0
#define GET_NEXT_REQUEST_TYPE  0xA1
#define GET_RESPONSE_TYPE      0xA2
#define SET_REQUEST_TYPE       0xA3

#ifdef	SR_SNMPv1
#define TRAP_TYPE              0xA4
#endif				/* SR_SNMPv1 */

#ifdef	SR_SNMPv2
#define	RESPONSE_TYPE GET_RESPONSE_TYPE
#define GET_BULK_REQUEST_TYPE  0xA5
#define INFORM_REQUEST_TYPE    0xA6
#define SNMPv2_TRAP_TYPE       0xA7
#endif				/* SR_SNMPv2 */

#ifdef	SR_SNMPv2
/* Exceptions in SNMPv2 PDUs (also contexts) */
#define NO_SUCH_OBJECT_EXCEPTION     0x80
#define NO_SUCH_INSTANCE_EXCEPTION   0x81
#define END_OF_MIB_VIEW_EXCEPTION    0x82
#endif				/* SR_SNMPv2 */

#ifdef SR_SNMPv2
/* Contexts for security material */
#define PRIV_DATA_TYPE         0x81
#define MGMT_COM_TYPE          0xA2	/* Changed from A1->A2 for SNMPv2 */
#define AUTH_MSG_TYPE          0xA1	/* Used for SNMPv2 */
#define PRIV_MSG_TYPE          0xA1
#define NO_AUTHINFO_TYPE       0x04

#ifndef SR_UNSECURABLE

#ifdef	SR_SNMPv2
#define SNMPv2_MD5_AUTHINFO_TYPE  0xA2
#endif				/* SR_SNMPv2 */

#endif				/* SR_UNSECURABLE */

#endif				/* SR_SNMPv2 */

/* Error codes */
#define NO_ERROR            0
#define TOO_BIG_ERROR       1

#if ((defined(SR_SNMPv1) && ! ( defined(SR_SNMPv2) )) || defined(WANT_ALL_ERRORS))
#define NO_SUCH_NAME_ERROR  2
#define BAD_VALUE_ERROR     3
#define READ_ONLY_ERROR     4
#endif	/* ((defined(SR_SNMPv1) && ! ( defined(SR_SNMPv2) )) || defined(WANT_ALL_ERRORS)) */

#define GEN_ERROR           5

#ifndef	SR_SNMPv2
#define NO_ACCESS_ERROR             NO_SUCH_NAME_ERROR
#define WRONG_TYPE_ERROR            BAD_VALUE_ERROR
#define WRONG_LENGTH_ERROR          BAD_VALUE_ERROR
#define WRONG_ENCODING_ERROR        BAD_VALUE_ERROR
#define WRONG_VALUE_ERROR           BAD_VALUE_ERROR
#define NO_CREATION_ERROR           NO_SUCH_NAME_ERROR
#define INCONSISTENT_VALUE_ERROR    BAD_VALUE_ERROR
#define RESOURCE_UNAVAILABLE_ERROR  GEN_ERROR
#define COMMIT_FAILED_ERROR         GEN_ERROR
#define UNDO_FAILED_ERROR           GEN_ERROR
#define AUTHORIZATION_ERROR         NO_SUCH_NAME_ERROR
#define NOT_WRITABLE_ERROR          NO_SUCH_NAME_ERROR
#define INCONSISTENT_NAME_ERROR     BAD_VALUE_ERROR
#else				/* SR_SNMPv2 */
#define NO_ACCESS_ERROR              6
#define WRONG_TYPE_ERROR             7
#define WRONG_LENGTH_ERROR           8
#define WRONG_ENCODING_ERROR         9
#define WRONG_VALUE_ERROR           10
#define NO_CREATION_ERROR           11
#define INCONSISTENT_VALUE_ERROR    12
#define RESOURCE_UNAVAILABLE_ERROR  13
#define COMMIT_FAILED_ERROR         14
#define UNDO_FAILED_ERROR           15
#define AUTHORIZATION_ERROR         16
#define NOT_WRITABLE_ERROR          17
#define INCONSISTENT_NAME_ERROR     18
#endif				/* SR_SNMPv2 */

#ifdef SR_SNMPv2
#define THIRTY_SOMETHING_ERROR    1
#define ENCODING_ERROR            2
#define DST_PARTY_ERROR           3
#define DST_PARTY_MISMATCH_ERROR  4
#define SRC_PARTY_ERROR           5
#define BAD_AUTHS_ERROR           6
#define NOT_IN_LIFETIME_ERROR     7
#define WRONG_DIGEST_VALUES_ERROR 8
#define UNKNOWN_CONTEXT_ERROR     9
#endif				/* SR_SNMPv2 */

#ifdef SR_SNMPv1
#define SNMP_BAD_VERSION_ERROR   10
#endif				/* SR_SNMPv1 */

/* domains */
#define UNKNOWN_DOMAIN		      0
#define SNMPv1_DOMAIN                 1
#define SNMPv2_DOMAIN                 2

#ifdef SR_SNMPv2
/* authentication types */
#define UNKNOWN_AUTHENTICATION        0
#define TRIVIAL_AUTHENTICATION        1
#define NOAUTH_AUTHENTICATION         3
#define DIGEST_UNKNOWN_AUTHENTICATION 4

#ifndef SR_UNSECURABLE

#ifdef	SR_SNMPv2
#define MD5_SNMPv2_AUTHENTICATION     6
#endif				/* SR_SNMPv2 */

#endif				/* SR_UNSECURABLE */

/* privacy types */
#define ABSENT  0
#define NOPRIV  1

#ifndef SR_UNSECURABLE

#ifndef SR_NO_PRIVACY
#define DESPRIV 2
#endif				/* SR_NO_PRIVACY */

#endif				/* SR_UNSECURABLE */

/* StorageType values  */
#define OTHER 1
#define VOLATILE 2
#define NONVOLATILE 3
#define PERMANENT 4

/* TruthValue types */
#define SR_LOCAL 1
#define SR_REMOTE 2

/* RowStatus types */
#define ACTIVE 1
#define NOT_IN_SERVICE 2
#define NOT_READY 3
#define CREATE_AND_GO 4
#define CREATE_AND_WAIT 5
#define DESTROY 6
#endif				/* SR_SNMPv2 */



#ifndef NONULL
#define NULLIT(pointer) pointer = NULL;
#else                           /* NONULL */
#define NULLIT(pointer)
#endif                          /* NONULL */

#ifdef MAX
#undef MAX
#endif

#ifdef MIN
#undef MIN
#endif

#define  MAX(x, y)       (((x) > (y)) ? (x) : (y))
#define  MIN(x, y)       (((x) < (y)) ? (x) : (y))

extern ulong snmp_sysUpTime(void);
#define SNMP_DELTA_TIME(x)     ((x/10) & MAXULONG)
#define SNMP_ABSOLUTE_TIME(x)  ((TS_VALUE(x)/10) & MAXULONG)
#define SNMP_ELAPSED_TIME(x)   ((ELAPSED_TIME64(x)/10) & MAXULONG)
#define SNMP_REMAINING_TIME(x) ((TIME_LEFT_SLEEPING64(x)/10) & MAXULONG)

/* structure references used throughout the code */
typedef struct _do_list doList_t;
typedef struct _ContextInfo ContextInfo;

typedef struct _OctetString {
    unsigned char  *octet_ptr;
    long            length;
} OctetString;

typedef struct _OID {
    int             length;
    unsigned long  *oid_ptr;
} OID;

#ifdef	SR_SNMPv2
typedef struct _UInt64 {
    unsigned long   big_end;
    unsigned long   little_end;
}               UInt64;

typedef struct _BitString {
    unsigned char  *octet_ptr;
    long            length;
    short           unused;
}               BitString;

#endif				/* SR_SNMPv2 */

#ifdef OS_STRUCT
typedef struct _ObjectSyntax {
    short           type;
    unsigned long   ul_value;	/* Counter, Gauge, time_ticks */
    long            sl_value;	/* simple num. */
    OctetString    *os_value;	/* for OS, IpAddress, Opaque */
    OID            *oid_value;	/* Object Identifier */

#ifdef	SR_SNMPv2
    UInt64         *uint64_value;	/* Big Unsigned Integer, e.g.,
					 * Counter64 */
    BitString      *bitstring_value;
#endif				/* SR_SNMPv2 */
} ObjectSyntax;

#else				/* OS_STRUCT */
typedef struct _ObjectSyntax {
    short           type;
    union {
	unsigned long   ulval;	/* Counter, Gauge, time_ticks */
#define ul_value    u.ulval
	long            slval;	/* simple num. */
#define sl_value    u.slval
	OctetString    *osval;	/* for OS, IpAddress, Opaque */
#define os_value    u.osval
	OID            *oidval;	/* Object Identifier */
#define oid_value   u.oidval

#ifdef	SR_SNMPv2
	UInt64         *uint64val;	/* Big Unsigned Integer, e.g.,
					 * Counter64 */
#define uint64_value   u.uint64val
	BitString      *bitstringval;
#define bitstring_value u.bitstringval
#endif				/* SR_SNMPv2 */
    }               u;
} ObjectSyntax;

#endif				/* OS_STRUCT */

typedef struct _VarBind {
    long            data_length;
    OID            *name;
    short           name_size;	/* number of bytes to encode the oid's tlv */
    ObjectSyntax    value;
    short           value_size;	/* number of bytes to encode the value's tlv */
    struct _VarBind *next_var;
} VarBind;

typedef struct _NormPdu {
    long   	    request_id;
    long            error_status;
    long            error_index;
} NormPdu;

#ifdef SR_SNMPv2
typedef struct _BulkPdu {
    long   	    request_id;
    long            non_repeaters;
    long            max_repetitions;
}               BulkPdu;

#endif				/* SR_SNMPv2 */

#ifdef	SR_SNMPv1
typedef struct _TrapPdu {
    OID            *enterprise;
    OctetString    *agent_addr;
    long            generic_trap;
    long            specific_trap;
    unsigned long   time_ticks;
} TrapPdu;

#endif				/* SR_SNMPv1 */

typedef struct _Pdu {
    OctetString    *packlet;
    short           type;
    long            num_varbinds;
    union {
	NormPdu         normpdu;

#ifdef	SR_SNMPv1
	TrapPdu         trappdu;
#endif				/* SR_SNMPv1 */

#ifdef	SR_SNMPv2
	BulkPdu         bulkpdu;
#endif				/* SR_SNMPv1 */
    }               u;
    VarBind        *var_bind_list;
    VarBind        *var_bind_end_ptr;
} Pdu;

typedef struct _AuthHeader {

#ifdef	SR_SNMPv1
    unsigned long   version;
#endif				/* SR_SNMPv1 */
    OctetString    *community;
    OctetString    *packlet;
    int             wrapper_size;

#ifdef SR_SNMPv2
    OID            *privDst;
    OID            *dstParty;
    OID            *srcParty;
    OID            *context;	/* MGH 6-Jan-93 */
    int             dstPartyIndex;
    int             srcPartyIndex;
    int             contextIndex;	/* MGH 6-Jan-93 */
    int             domain;
    int             authType;
    OctetString    *authPrivate;
    OctetString    *digest;
    int             offset_to_auth_message;
    int             offset_to_digest;
    int             bytecount;
    unsigned long   src_timestamp;
    unsigned long   dst_timestamp;
    int             privType;
    OctetString    *privPrivate;
    OctetString    *privData;
#endif				/* SR_SNMPv2 */
} AuthHeader;

#ifdef	SR_SNMPv2
typedef struct _SnmpParty {
    OID            *partyIdentity;
    OID            *partyTDomain;
    OctetString    *partyTAddress;
    unsigned long   partyMaxMessageSize;	/* MGH 27-Mar-92 */
    OID            *partyAuthProtocol;
    unsigned long   partyAuthClock;	/* MGH 1-Apr-92 */
    OctetString    *partyAuthPublic;
    OctetString    *partyAuthPrivate;	/* MGH 3-Apr-92 */
    unsigned long   partyAuthLifetime;	/* JDC 26-Sep-91 */
    OID            *partyPrivProtocol;
    OctetString    *partyPrivPublic;
    OctetString    *partyPrivPrivate;	/* MGH 3-Apr-92 */
}               SnmpParty;

#endif				/* SR_SNMPv2 */



#ifndef SR_MGR
typedef struct _ObjectInfo ObjectInfo;
struct _ObjectInfo {
    OID             oid;

#ifndef	LIGHT
    char           *oidname;
#endif				/* LIGHT */
    unsigned char   oidtype, oidstatus, nominator, priority;
    VarBind        *(*get) SR_PROTOTYPE((OID *incoming,
					 ObjectInfo *object,
					 int searchType,
					 ContextInfo *contextInfo,
					 int serialNum));
    int             (*test) SR_PROTOTYPE((OID *incoming,
					  ObjectInfo *object,
					  ObjectSyntax *value,
					  doList_t *doHead,
					  doList_t *doCur,
					  ContextInfo *contextInfo));
};

#endif				/* SR_MGR */

/* externs */

/* make_lib.c */
extern	struct _OctetString *MakeOctetString SR_PROTOTYPE((unsigned char *string, long length));
extern	struct _ObjectSyntax *CloneObjectSyntax SR_PROTOTYPE((struct _ObjectSyntax *os));
extern	struct _OctetString *CloneOctetString SR_PROTOTYPE((struct _OctetString *os_ptr));
extern	struct _BitString *MakeBitString SR_PROTOTYPE((unsigned char *string, long length, int unused));
extern	struct _OID *MakeOID SR_PROTOTYPE((unsigned long *sid, int length));
#ifndef SR_MGR
extern	struct _VarBind *MakeVarBind SR_PROTOTYPE((struct _ObjectInfo *object, struct _OID *instance, void *value));
#endif /* SR_MGR */
extern	struct _VarBind *MakeVarBindWithValue SR_PROTOTYPE((struct _OID *object, struct _OID *instance, int type, void *value));
extern	struct _VarBind *MakeVarBindWithNull SR_PROTOTYPE((struct _OID *object, struct _OID *instance));
extern	struct _VarBind *make_varbind SR_PROTOTYPE((struct _OID *oid, int type, unsigned long _ul_value, long _sl_value, struct _OctetString *_os_value, struct _OID *_oid_value));
extern	struct _Pdu *MakePdu SR_PROTOTYPE((int type, long request_id, long error_status_or_non_repeaters, long error_index_or_max_repetitions, struct _OID *enterprise, struct _OctetString *agent_addr, long generic_trap, long specific_trap, unsigned long time_ticks));
extern	short LinkVarBind SR_PROTOTYPE((struct _Pdu *pdu_ptr, struct _VarBind *vb_ptr));
extern	struct _OctetString *MakeOctetStringFromText SR_PROTOTYPE((char *text_str));
extern	struct _OctetString *MakeOctetStringFromHex SR_PROTOTYPE((char *text_str));
extern	struct _OID *MakeOIDFromHex SR_PROTOTYPE((unsigned char *text_str));
extern	struct _OID *CloneOID SR_PROTOTYPE((struct _OID *oid_ptr));
extern	struct _OID *ClonePartialOID SR_PROTOTYPE((struct _OID *oid_ptr, int start, int len));
extern	struct _OID *MakeLenOID SR_PROTOTYPE((struct _OID *oida));
extern	struct _OID *MakeOIDSubLen SR_PROTOTYPE((struct _OID *oida));
extern  struct _OID *CatOID SR_PROTOTYPE((struct _OID *oida, struct _OID *oidb));
extern	struct _VarBind *CloneVarBindList SR_PROTOTYPE((struct _VarBind *in_vb));
extern	struct _VarBind *CloneVarBind SR_PROTOTYPE((struct _VarBind *in_vb));
extern	struct _UInt64 *CloneUInt64 SR_PROTOTYPE((struct _UInt64 *value));
extern	struct _BitString *CloneBitString SR_PROTOTYPE((struct _BitString *_bitstring_value));
extern struct _OID *MakeOIDFromDot SR_PROTOTYPE((char *_text_str));
extern struct _OID *MakeOIDFragFromDot SR_PROTOTYPE((char *_text_str));
extern int CmpOctetStrings SR_PROTOTYPE((OctetString *o1, OctetString *o2));


/* free_lib.c */
extern	void FreeOctetString SR_PROTOTYPE((struct _OctetString *os_ptr));
extern	void FreeObjectSyntax SR_PROTOTYPE((struct _ObjectSyntax *os));
extern	void FreeOID SR_PROTOTYPE((struct _OID *oid_ptr));
extern	void FreeUInt64 SR_PROTOTYPE((struct _UInt64 *uint64_ptr));
extern	void FreeBitString SR_PROTOTYPE((struct _BitString *bitstring_ptr));
extern	void FreeVarBind SR_PROTOTYPE((struct _VarBind *vb_ptr));
extern	void FreePdu SR_PROTOTYPE((struct _Pdu *pdu_ptr));
extern	void FreeVarBindList SR_PROTOTYPE((struct _VarBind *vb_ptr));

/* bld_pkt.c */
extern	short BuildPdu SR_PROTOTYPE((struct _Pdu *pdu_ptr));
extern	long FindLenVarBind SR_PROTOTYPE((struct _VarBind *vb_ptr));
extern	short FindLenBitString SR_PROTOTYPE((struct _BitString *bitstring_ptr));
extern	long FindLenOctetString SR_PROTOTYPE((struct _OctetString *os_ptr));
extern	short FindLenOID SR_PROTOTYPE((struct _OID *oid_ptr));
extern	short FindLenUInt SR_PROTOTYPE((unsigned long value));
extern	short FindLenInt SR_PROTOTYPE((long value));
extern	short FindLenUInt64 SR_PROTOTYPE((struct _UInt64 *value));
extern	short DoLenLen SR_PROTOTYPE((long len));
extern	void AddLen SR_PROTOTYPE((unsigned char **working_ptr, long lenlen, long data_len));
extern	short AddVarBind SR_PROTOTYPE((unsigned char **working_ptr, struct _VarBind *vb_ptr, struct _Pdu *pdu_ptr));
extern	short AddOctetString SR_PROTOTYPE((unsigned char **working_ptr, int type, struct _OctetString *os_ptr));
extern	short AddOID SR_PROTOTYPE((unsigned char **working_ptr, int type, struct _OID *oid_ptr));
extern	short AddUInt SR_PROTOTYPE((unsigned char **working_ptr, int type, unsigned long value));
extern	short AddInt SR_PROTOTYPE((unsigned char **working_ptr, int type, long value));
extern	short AddUInt64 SR_PROTOTYPE((unsigned char **working_ptr, int type, struct _UInt64 *value));
extern	short AddBitString SR_PROTOTYPE((unsigned char **working_ptr, int type, struct _BitString *bitstring_ptr));
extern	void AddNull SR_PROTOTYPE((unsigned char **working_ptr, int type));
extern  void NormalizeVarBindListLengths SR_PROTOTYPE((struct _VarBind *vb_ptr));
/* prse_pkt.c */
extern	struct _Pdu *ParsePdu SR_PROTOTYPE((struct _AuthHeader *auth_ptr));
extern	struct _VarBind *ParseVarBind SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr));
extern	struct _OctetString *ParseOctetString SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr, short *type));
extern	struct _OID *ParseOID SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr));
extern	struct _UInt64 *ParseUInt64 SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr));
extern	struct _BitString *ParseBitString SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr, short *type));
extern	unsigned long ParseUInt SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr, short *type));
extern	long ParseInt SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr, short *type));
extern	short ParseNull SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr, short *type));
extern	long ParseSequence SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr, short *type));
extern	short ParseType SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr));
extern	long ParseLength SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr));
extern	long ParseImplicitSequence SR_PROTOTYPE((unsigned char **working_ptr, unsigned char *end_ptr, short *type));

/* auth_lib.c */
#ifdef SR_SNMPv2
extern	struct _AuthHeader *MakeAuthentication SR_PROTOTYPE((struct _OctetString *community, struct _OID *privDst, struct _OID *dstParty, struct _OID *srcParty, struct _OID *context, int authType, struct _OctetString *authPrivate, unsigned long src_timestamp, unsigned long dst_timestamp, int privType, struct _OctetString *privPrivate));
#else /* SR_SNMPv2 */
extern	struct _AuthHeader *MakeAuthentication SR_PROTOTYPE((struct _OctetString *community));
#endif /* SR_SNMPv2 */
extern	short BuildAuthentication SR_PROTOTYPE((struct _AuthHeader *auth_ptr, struct _Pdu *pdu_ptr));
extern	void FreeAuthentication SR_PROTOTYPE((struct _AuthHeader *auth_ptr));
extern	struct _AuthHeader *ParseAuthentication SR_PROTOTYPE((unsigned char *packet_ptr, long length, int *error_code));
extern	int CheckDigest SR_PROTOTYPE((struct _AuthHeader *auth_ptr, int srcPartyIndex, int dstPartyIndex, int *error_code));
extern	int MakeDigest SR_PROTOTYPE((struct _AuthHeader *auth_ptr));
extern	unsigned long FindMaxClockOffset SR_PROTOTYPE((void));

/* nvt_chk.c */
extern int IsDisplayString SR_PROTOTYPE((struct _OctetString *os));
extern int ConvertToDisplayString SR_PROTOTYPE((struct _OctetString *os));
extern int ConvertFromDisplayString SR_PROTOTYPE((struct _OctetString *os));




/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* _SNMP_H */
