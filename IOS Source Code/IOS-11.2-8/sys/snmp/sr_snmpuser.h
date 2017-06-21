/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_snmpuser.h,v 3.4 1996/01/31 23:26:47 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/snmp/sr_snmpuser.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_snmpuser.h,v $
 * Revision 3.4  1996/01/31  23:26:47  jjohnson
 * CSCdi47892:  SNMP oid translation table needs to be modularized
 *
 * Revision 3.3  1995/11/17  19:01:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:18:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:54  hampton
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

/* Revision History:
 *
 *  8/3/89 JDC
 *  Redefined declaration of parse_sub_id_* routines to long from unsigned long
 *
 *  31-Dec-90 JDC
 *  Merged in hash table lookup code from Unix 9.5.0.7 sources
 *
 *  9-Dec-1992 DLB
 *    Changed return type of InitOIDTree and InitNameOIDHashArray to void
 *    and added them to the non-STDC section.
 *
 *  18-May-1993 MGH
 *    Changed the parse_sub_id* routines to return ints.  Added a parameter
 *    to the these routines that will hold the value that was previously
 *    returned.  Return -1 on error, 0 on success.  Removed prototype for
 *    parse_sub_id_alpha() because it doesn't exist.
 */

#ifndef _snmpuser_h
#define _snmpuser_h

#include "sr_proto.h"

/* oid_lib */

#ifdef NEW_OID_LIB
#define NAME_OID_HASH_SIZE 2000

typedef struct _OID_TREE_ELE {
    unsigned long   sid_value;
    char           *oid_name;
    char           *oid_number_str;
    struct _OID_TREE_ELE *next;
    struct _OID_TREE_ELE *first_descendent;
} OID_TREE_ELE;

#define HASHNEXT(x) x, NULL
#else 				/* NEW_OID_LIB */
#define HASHNEXT(x) x
#endif				/* NEW_OID_LIB */

struct MIB_OID {
    char           *name;
    char           *number;
#ifdef NEW_OID_LIB
    struct MIB_OID    *hash_next;
#endif /* NEW_OID_LIB */
};


/*
 * The following externs are needed to let users load/modify the mib
 */

#ifndef DO_NOT_DECLARE_MIB_OID_TABLE
extern struct MIB_OID *mib_oid_table;
extern char *unassigned_text;
extern int num_mot_entries;
#endif				/* DO_NOT_DECLARE_MIB_OID_TABLE */

/*
 * cisco additions to support dynamic loading of translation tables
 */
extern	void load_oid SR_PROTOTYPE((const struct MIB_OID *));
extern	void init_oid SR_PROTOTYPE((void));

/* oid_lib.c */
extern  short TypeStringToShort SR_PROTOTYPE((char *str));
extern  char *TypeShortToString SR_PROTOTYPE((int shrt));
extern  short AccessStringToShort SR_PROTOTYPE((char *str));
extern  char *AccessShortToString SR_PROTOTYPE((int shrt));
extern	struct _OID *MakeObjIdFromDot SR_PROTOTYPE((char *_text_str));
extern	int ParseSubIdHex SR_PROTOTYPE((unsigned char **temp_ptr, unsigned long *value));
extern	int ParseSubIdOctal SR_PROTOTYPE((unsigned char **temp_ptr, unsigned long *value));
extern	int ParseSubIdDecimal SR_PROTOTYPE((unsigned char **temp_ptr, unsigned long *value));
extern	short MakeDotFromOID SR_PROTOTYPE((struct _OID *oid_ptr, char *buffer));
extern	short GetStringFromSubIds SR_PROTOTYPE((char *string_buffer, unsigned long *sid, int sid_counter));
extern	short MatchGame SR_PROTOTYPE((char *ptr1, char *ptr2));
extern	int DecChar2L SR_PROTOTYPE((char *str, unsigned long *value));
extern	int HexChar2L SR_PROTOTYPE((char *str, unsigned long *value));
extern	int OctChar2L SR_PROTOTYPE((char *str, unsigned long *value));
extern	int Str2L SR_PROTOTYPE((char *str, int base, unsigned long *value));
extern	int CmpNOID SR_PROTOTYPE((struct _OID *oida, struct _OID *oidb, int length));
extern	int Inst2IP SR_PROTOTYPE((unsigned long *inst, int instLength, unsigned long *ipAddr, int searchType));
extern	int CmpOIDClass SR_PROTOTYPE((struct _OID *ptr1, struct _OID *ptr2));
extern	int CmpOID SR_PROTOTYPE((struct _OID *ptr1, struct _OID *ptr2));

/* oid_lib.c with NEW_OID_LIB defined */
extern	void InitNewOIDRoutines SR_PROTOTYPE((void));
extern	void MergeNewOIDRoutines SR_PROTOTYPE((int start));
extern	void InitOIDTree SR_PROTOTYPE((void));
extern	void MergeOIDTree SR_PROTOTYPE((int start));
extern	int AddEleToTree SR_PROTOTYPE((struct MIB_OID *mot));
extern	void InitNameOIDHashArray SR_PROTOTYPE((int start));
extern	struct _OID_TREE_ELE *OIDTreeDive SR_PROTOTYPE((struct _OID_TREE_ELE *oid_tree_ptr, unsigned long *sid, int sid_counter));
extern	int NewMIBFromFile SR_PROTOTYPE((char *filename));
extern	int ParseMIBLine SR_PROTOTYPE((char *in_buffer, char *name_buf, char *num_buf));
extern	int MergeMIBFromFile SR_PROTOTYPE((char *filename));
extern struct _OID *MakeOIDFromDot SR_PROTOTYPE((char *_text_str));

/* prnt_lib.c */
extern	void PrintPacket SR_PROTOTYPE((unsigned char *ptr, long len));
extern	void PrintOctetString SR_PROTOTYPE((struct _OctetString *oct_ptr, int wrap));
extern	void SPrintOctetString SR_PROTOTYPE((struct _OctetString *oct_ptr, int wrap, char *));
extern	int PrintVarBindList SR_PROTOTYPE((struct _VarBind *vb_ptr));
extern	int PrintVarBind SR_PROTOTYPE((struct _VarBind *vb_ptr));
extern	int SPrintVarBind SR_PROTOTYPE((struct _VarBind *vb_ptr, char *));
extern	int PrintVarBindValue SR_PROTOTYPE((struct _VarBind *vb_ptr, char *));
extern	int SPrintVarBindValue SR_PROTOTYPE((struct _VarBind *vb_ptr, char *, char *));
extern	int PrintAscii SR_PROTOTYPE((struct _OctetString *os_ptr));
extern	int SPrintAscii SR_PROTOTYPE((struct _OctetString *os_ptr, char *));
extern	void SetProgname SR_PROTOTYPE((char *s));
extern	void PrintErrorCode SR_PROTOTYPE((long error_index, long error_status));
extern  void PrintOID SR_PROTOTYPE((OID *oid));

/* mgrsupp.c */
extern	int SyncClocks SR_PROTOTYPE((int clusterIndex, int operationType));
extern	int SyncClocks_1 SR_PROTOTYPE((long req_id, struct _OID *clock_oid, int ci, int pty[4]));
extern	int SyncClocks_2 SR_PROTOTYPE((long req_id, struct _OID *clock_oid, int ci, int pty[4]));
extern	int SyncClocks_rekey SR_PROTOTYPE((long req_id, struct _OID *clock_oid, struct _OID *auth_secret_oid, struct _OID *priv_secret_oid, int ci, int pty[4]));
extern 	void flush_keys_to_file SR_PROTOTYPE((int ci, int pty[4]));
extern	unsigned char random_byte SR_PROTOTYPE((void));
extern	struct _AuthHeader *MakeV1Message SR_PROTOTYPE((char *cluster_name));
extern	struct _AuthHeader *MakeV2Message SR_PROTOTYPE((int srcIndex, int dstIndex, int contextIndex));
extern	struct _AuthHeader *MakeV2MessageWithClocks SR_PROTOTYPE((struct _OID *srcParty, struct _OID *dstParty, struct _OID *context, unsigned long dstClock, unsigned long srcClock));
extern	int CheckEndOfMIB SR_PROTOTYPE((struct _Pdu *pdu_ptr));
#define GetTimeNow snmp_sysUpTime
extern	int CheckOID SR_PROTOTYPE((struct _OID *oid1_ptr, struct _OID *oid2_ptr));
extern	void PrintMetInfo SR_PROTOTYPE((struct _VarBind *vb_list_ptr));
extern	void PrintRouteInfo SR_PROTOTYPE((struct _VarBind *vb_list_ptr));
extern	struct _VarBind *CreateVarBind SR_PROTOTYPE((char *name, char *type, char *value));
extern	struct _VarBind *BuildNextVarBind SR_PROTOTYPE((char **argv, int *in_index));
extern	void InitUtilFileNames SR_PROTOTYPE((void));
extern	void DoClusterInit SR_PROTOTYPE((char *arg, char **cluster_name, int *cluster_index));
#if (defined(SNMPINFO) && defined(NEW_OID_LIB))

#define BEHAVIOUR_REJECT_PARENT      0
#define BEHAVIOUR_ACCEPT_LEAF_PARENT 1
#define BEHAVIOUR_ACCEPT_ANY_PARENT  2

extern  OID_TREE_ELE *SR_GetMIBNodeFromOID SR_PROTOTYPE((OID *oid_ptr, int behaviour));
extern  OID_TREE_ELE *GetMIBNodeFromOID SR_PROTOTYPE((OID *oid_ptr));
extern  OID_TREE_ELE *GetMIBNodeFromDot SR_PROTOTYPE((char *name));
extern  struct MIB_OID *GetMOTEntryFromOID SR_PROTOTYPE((OID *oid_ptr));
extern  struct MIB_OID *GetMOTEntryFromDot SR_PROTOTYPE((char *name));
extern  OID_TREE_ELE *LP_GetMIBNodeFromOID SR_PROTOTYPE((OID *oid_ptr));
extern  OID_TREE_ELE *LP_GetMIBNodeFromDot SR_PROTOTYPE((char *name));
extern  struct MIB_OID *LP_GetMOTEntryFromOID SR_PROTOTYPE((OID *oid_ptr));
extern  struct MIB_OID *LP_GetMOTEntryFromDot SR_PROTOTYPE((char *name));
extern  OID_TREE_ELE *AP_GetMIBNodeFromOID SR_PROTOTYPE((OID *oid_ptr));
extern  OID_TREE_ELE *AP_GetMIBNodeFromDot SR_PROTOTYPE((char *name));
extern  struct MIB_OID *AP_GetMOTEntryFromOID SR_PROTOTYPE((OID *oid_ptr));
extern  struct MIB_OID *AP_GetMOTEntryFromDot SR_PROTOTYPE((char *name));
#endif	/* (defined(SNMPINFO) && defined(NEW_OID_LIB)) */

/* mgrio.c */
extern	void InitClusters SR_PROTOTYPE((void));

/* modv2.c */
extern int AddParty SR_PROTOTYPE((OID *partyIdentity, int discriminator, int cloneFrom));
extern int AddContext SR_PROTOTYPE((OID *contextIdentity, int view));

/* DO NOT PUT ANYTHING AFTER THIS ENDIF */
#endif				/* !defined _snmpuser_h */
