/* $Id: ep_frm.h,v 1.1.68.3 1996/09/11 23:21:06 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/include/ep_frm.h,v $
 *------------------------------------------------------------------
 * Cylink Source code: ep_frm.h
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ep_frm.h,v $
 * Revision 1.1.68.3  1996/09/11  23:21:06  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 1.1.68.2  1996/06/05  23:12:02  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Fix crypto zeroize & key generation for 7500 with VIPs.
 * Fix crypto connection setup with traffic in both directions.
 * Fix encryption bugs.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:39  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.4  1996/04/16  21:53:56  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 1.1.2.3  1996/03/13  22:45:30  bew
 * Branch: IosSec_branch
 * The RSP/VIP environment (multiple crypto engines) are now supported
 * in crypto parser routines and crypto session mgmt code.
 *
 * Revision 1.1.2.2  1996/01/16  20:21:00  bew
 * Branch: IosSec_branch
 * Simplified the session mgmt debug msgs & changed the way that duplicate
 * connection messages are handled.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:38  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:12  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Include/EP_FRM.H 28    6/09/95 10:25a Tchi 
*
* DESCRIPTION:	Entity Protection Structure and Function Definitions
*
* NOTES:		This file contains only info related to EP ISR, see
*				EP_CUST.H for the EP Custodian information (though this
*				file is included in epc.h).
*
*				Structures defined in this file are carefully sized to conform
*				to longword (4byte) boundaries. This provides assurance that a
*				multi-byte memory operation will not result in more than the
*				required # of memory cycles.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: EP_FRM.H 
* 
* *****************  Version 28  *****************
* User: Tchi         Date: 6/09/95    Time: 10:25a
* Updated in $/SecureLAN/SDU/Include
* added epPEdefRec; add nnc to MgRequest
* 
* *****************  Version 27  *****************
* User: Tchi         Date: 6/01/95    Time: 6:11p
* Updated in $/SecureLAN/SDU/Include
* changed to use NO_PEDEF_NNC_TABLE and NO_SR_TABLE
* 
* *****************  Version 25  *****************
* User: Croyle       Date: 5/22/95    Time: 12:40p
* Updated in $/SecureLAN/SDU/Include
* Added epPassIntraNet flag for bridging case.
* 
* *****************  Version 24  *****************
* User: Sduhost      Date: 4/17/95    Time: 4:15p
* Updated in $/SecureLAN/SDU/Include
* Renamed NEW_CIB to BAD_CONN
* 
* *****************  Version 23  *****************
* User: Sduhost      Date: 4/17/95    Time: 2:08p
* Updated in $/SecureLAN/SDU/Include
* added NEW_CIB in cib status bits
* 
* *****************  Version 22  *****************
* User: Sduhost      Date: 4/14/95    Time: 4:03p
* Updated in $/SecureLAN/SDU/Include
* shrink down dynamically allocated memory
* 
* *****************  Version 21  *****************
* User: Tchi         Date: 4/07/95    Time: 7:37p
* Updated in $/SecureLAN/SDU/Include
* SNMP table changes. Please define NO_PEDEF_NNC_TABLE if you do not want
* it to take effect.
* 
* *****************  Version 20  *****************
* User: Bald_eagle   Date: 2/13/95    Time: 1:16p
* Updated in $/SecureLAN/SDU/Include
* Modified HMA index type from ushort to short
* 
* *****************  Version 19  *****************
* User: Croyle       Date: 1/03/95    Time: 1:48p
* Updated in $/SecureLAN/SDU/Include
* Modified naming convention for all Croyle files.
* 
* *****************  Version 18  *****************
* User: Croyle       Date: 12/16/94   Time: 10:37a
* Updated in $/SecureLAN/SDU/Include
* Modified Array Declarations to use TYPDEF'd array structure (allows
* sizeof() to be used for size determination).
* 
* *****************  Version 17  *****************
* User: Croyle       Date: 11/03/94   Time: 2:28p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 16  *****************
* User: Croyle       Date: 11/02/94   Time: 9:36a
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 14  *****************
* User: Croyle       Date: 10/19/94   Time: 4:38p
* Updated in $/SecureLAN/SDU/Include
* Final pre-integration check-in
* 
* *****************  Version 13  *****************
* User: Croyle       Date: 10/14/94   Time: 7:29p
* Updated in $/SecureLAN/SDU/Include
* Modified name of "ctrl" field in CyDef to "method" to reflect recent
* changes in the selection of cryptographic algorithms via NNC.
* 
* *****************  Version 12  *****************
* User: Croyle       Date: 10/14/94   Time: 1:54p
* Updated in $/SecureLAN/SDU/Include
* Changed structure definition name from CyInfo to CyDef for CIBdef.
* 
* *****************  Version 11  *****************
* User: Croyle       Date: 10/14/94   Time: 1:47p
* Updated in $/SecureLAN/SDU/Include
* Modified epFrameInfo data union field, info, to include a cy_info
* structure as well.  This structure is used for passing crypto
* parameters
* from ep to cy when appropriate.
* 
* *****************  Version 10  *****************
* User: Croyle       Date: 10/13/94   Time: 10:22a
* Updated in $/SecureLAN/SDU/Include
* First official release of EP.H.  
* 
* *****************  Version 9  *****************
* User: Croyle       Date: 10/10/94   Time: 5:44p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 8  *****************
* User: Croyle       Date: 10/10/94   Time: 1:37p
* Updated in $/SecureLAN/SDU/Include
* Modified epFrameInfo, added pre-integration #defines (NNC_AUTO and
* NNC_UPE), and modified epPEdef from EntityID array to PEinfo array
* (hash field replaced with NNC index).
* 
* *****************  Version 7  *****************
* User: Croyle       Date: 10/06/94   Time: 2:50p
* Updated in $/SecureLAN/SDU/Include
* Moved epFrameInfo from PACTL.H to EP.H and made other minor mod.s
* 
* *****************  Version 6  *****************
* User: Croyle       Date: 10/05/94   Time: 2:46p
* Updated in $/SecureLAN/SDU/Include
* 
* *****************  Version 5  *****************
* User: Croyle       Date: 10/04/94   Time: 10:41a
* Updated in $/SecureLAN/SDU/Include
* Moved Default Configuration Values/Macros from C file.
*
* *****************  Version 4  *****************
* User: Croyle       Date: 10/03/94   Time: 4:15p
* Updated in $/SecureLAN/SDU/Include
* Another minor update - a few changes so it would actually compile (what
* a keen idea!).
* 
* *****************  Version 3  *****************
* User: Croyle       Date: 10/03/94   Time: 9:50a
* Updated in $/SecureLAN/SDU/Include
* Modified various type declarations and typedef names (updating because
* I'm attempting to keep up-to-date copies available to others).
*
* *****************  Version 2  *****************
* User: Croyle       Date: 10/01/94   Time: 1:20p
* Updated in $/SecureLAN/SDU/Include
* Compiler idiosyncrocy discovered, (_far*) and (near*) caused problems -
* removed parenthesis.
*
* *****************  Version 1  *****************
* User: Croyle       Date: 9/30/94    Time: 9:28a
* Created in $/SecureLAN/SDU/Include
* First release of EP.H.  Things could still change a bit...at least
* until the first formal release of the EP ISR and Custodian software.
*******************************************************************************/

#ifndef	EP_FRM_H
#define	EP_FRM_H

/*****************************************************************************
							 INCLUDE FILES
*****************************************************************************/

#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/iqueue.h"
/*------------------------- EP Function Return Codes -------------------------- */

#define EP_OKAY			 0			/* function succeeded */
#define EP_ERR_MEM_PH	-1			/* not enuf memory for PET hash tbl */
#define EP_ERR_MEM_PN	-2			/* not enuf memory for PET node pool */
#define EP_ERR_MEM_CP	-3			/* not enuf memory for COT page table */
#define EP_ERR_MEM_CN	-4			/* not enuf memory for COT page nodes */
#define EP_ERR_SIZ_PH	-5			/* Invalid PET hash table size (>65k) */
#define EP_ERR_SIZ_PN	-6			/* Invalid PET node pool size (>65k) */
#define EP_ERR_SIZ_CP	-7			/* Too many COT pages */
#define EP_ERR_SIZ_CN	-8			/* COTnodes or HashTable too big per page */

/*------------------------- EP Control Flags (ep_ctrl) ------------------------ */

#define OPEN_SDU		0x8000		/* SDU open (pass non-PE frames) */
#define LOCK_PET		0x4000		/* PET locked (no auto-init) */
#define PASS_INTRANET	0x2000		/* Pass Intra-Net PE traffic */

/*-------------------------- CIB Status Flags --------------------------------- */

#define PEND_CONN		0x8000		/* set when connection is pending */
#define OPEN_NODE		0x4000		/* set if node is open (0=closed) */
#define LOCK_NODE		0x2000		/* set if COT entry locked (cant reallocate) */
#define USED_NODE		0x1000		/* node has been used */
#define XCHG_KEYS		0x0800		/* set if new crypto keys needed */
#define TIME_KEYS		0x0400		/* check age of keys periodically */
#define KEY_ALLOC		0x0200		/* crypto keys have been allocated */
#define BAD_CONN		0x0001		/* set if a new connection is requested */

/*---------------------------- Manager Requests ------------------------------- */

#define	MG_REQ_SERVED	0x8000
#define MG_ADD_PET		0x0001		/* add a node to PET */
#define MG_DEL_PET		0x0002		/* delete a node from PET */
#define MG_ADD_COT		0x0010		/* add a node to PET */
#define MG_DEL_COT		0x0020		/* delete a node from PET */

/*------------------------ Default Configuration Values ----------------------- */
/* */
/* NOTE:	The amount of memory allocated at init time by the EP module is */
/*			computed as follows (the DEF_ prefix is omitted for clarity): */
/* */
/*			COT/PET data structures */
/*			----------------------- */
/*			PHASH * sizeof(ushort) + PNODES * ( sizeof(PETnode) + */
/*			sizeof(COTpage) + CNODES*sizeof(COTnode) + CHASH*sizeof(ushort) ) */
/* */
/*			Socket data structures */
/*			---------------------- */
/*			SOKDEFS*sizeof(SokDef). */
/* */
/* ASSUME:	sizeof(PETnode) = 16, sizeof(COTpage) = 16, sizeof(COTnode) = 44, */
/*			sizeof(ushort) = 2, sizeof(SokDef) = 142. */
/* */
/* THUS:	PET/COT Size =	PHASH*2+PNODES*(16+16+CNODES*44+CHASH*2) = 98,624 */
/*			Socket Size  =	SOKDEFS * 142 = 14,200 */
/*			Total Size   =	98,624 + 56,800 =  155424 bytes */

#define MAX_SDU_PE		100			/* maximum number of PEs defined */

#define DEF_PNODES		100			/* Default number of PET nodes */
#define DEF_PHASH	 	512			/* Default PET hash table size */
#define DEF_CNODES		10			/* Default # COT nodes per page */
#define DEF_CHASH	 	16			/* Default COT hash tbl size per page */
#define DEF_LOCK		0			/* Default PET locking status */
#define DEF_OPEN		0			/* Default open status */
#define DEF_INTRANET	0			/* Default action for intra entity traffic */
#define DEF_MAXKEYAGE	30			/* Default # mins before new keys needed */
#define DEF_SOKDEFS		20			/* Default # socket defintions */

/******************************************************************************
							STRUCTURE DEFINITIONS
******************************************************************************/

/*---------------------------- Socket Structures ------------------------------ */

/* Socket Range Definition */

typedef struct {
	ushort	low;							/* low socket number in range */
	ushort	high;							/* high socket number in range */
}	SokRng;									/* (size = 4 bytes) */

/* Socket Table */

typedef struct	{
	void far*	free;						/* far ptr links free list  */
	bits32		mask;						/* socket bit mask */
	bits32		act_msb;					/* action msbit (of 2 bit value) */
	bits32		act_lsb;					/* action lsbit (of 2 bit value) */
	SokRng		range[32];					/* socket definition table */
}	SokDef;									/* (size = 142 bytes) */

typedef SokDef far *SokPtr;					/* Socket Definition Pointer */

#ifdef CISCO
struct cisco_CyDef {
	int methods_allowed;
	char **peers_allowed;
	ushort slot;
	long connection_id;
	long new_connection_id;
	uint mode_flags;
};
#endif
/*-------------------- Connection Table (COT) Structures ---------------------- */

/* Cryptographic Information */

typedef struct	{
	short	skey_i;							/* session key index */
	short	ikey_i;  	  					/* initialization vector key index */
	ushort	method;							/* crypto method (and algorithm) */
	ushort	time;							/* minute counter time stamp */
#ifdef CISCO
	/* 
	 * This cisco struct should really be in crypto_glue_strincts.h with
	 * a pointer her to a void *.
	 */
	struct cisco_CyDef cisco;
#endif

}	CyDef;                                  /* (size = 8 bytes) */

/* Connection Information Block */

typedef struct	{
	bits16	status;							/* status flags (see MACROs above) */
	ushort	pad;							/* pad struct to longword boundary */
	bits32	pvector;						/* protocol vector */
	bits32	act_msb;						/* protocol action (msbit) */
	bits32	act_lsb;						/* protocol action (lsbit) */
	SokPtr	socket;							/* socket (port) defintions */
	CyDef	crypto;							/* cryptographic information */
}	CIBdef;									/* (size = 28 bytes) */

typedef CIBdef far *CIBptr;

/*----------------- Generic (pet & cot) Table Structures ---------------------- */

/* Entity ID Information (adr+pid used as search key) */

typedef struct	{
	ulong	adr;							/* 4 bytes of "normalized" address */
	ushort	pid;							/* protocol ID */
	ushort	hash;							/* hashing value */
}	EntityID;                               /* (size = 8 bytes) */

/* Hash list indeces (links nodes with common hash value) */

typedef struct {
	ushort	next;							/* offset to next node in hash list */
	ushort	list;							/* start of list (hash table entry) */
}	HashList;                               /* (size = 4 bytes) */

/* Free List indeces (links list of all available nodes) */

typedef struct {
	ushort	next;							/* offset to next node in free list */
	ushort	prev;							/* offset to prev node in free list */
}	FreeList;								/* (size = 4 bytes) */

/*----------------------------- COT definitions ------------------------------ */

/* COT Node Definition */

typedef struct	{
	EntityID	id;							/* unprotected entity search key */
	HashList	hash;						/* next node in COT branch */
	FreeList	free;						/* free list indeces */
	CIBdef		cib;						/* connection information block */
}	COTnode;                                /* (size = 44 bytes) */

/* COT Page Definition */

typedef struct  {                   	
	COTnode far *node;						/* start of cot node pool */
	ushort	far *hash;						/* start of cot hash table */
	ushort		free;						/* start of free list */
	ushort		used;						/* end of free list (see custodian) */
	ushort		nnc;						/* NNC index for PE using this page */
	ushort		pad;						/* pad structure to long word boundary */
}	COTpage;                                /* (size = 16 bytes) */

/*------------------------ PET entry definition ------------------------------- */

/* PET entry structure */

typedef struct {
	EntityID	id;							/* protected entity search key */
	HashList 	hash;						/* next node in PET branch */
	short		free;						/* next node to (re)allocate */
	short		page;						/* Index of COT page this PE uses */
}	PETnode;                                /* (size = 16 bytes) */

/* PET entry initialization structure */

typedef struct {
	ulong		adr;						/* address of PE (same as EntityID) */
	ushort		pid;						/* protocol ID   (same as EntityID) */
	ushort		nnc;						/* index of NNC to use for PE */
}	PEdefRec;								/* (size = 8 bytes/entry) */

typedef PEdefRec PEdefs[MAX_SDU_PE];

/*------------------------ PA Interface definition ---------------------------- */

/* Info passed from EP to CY */

typedef struct {
	ushort		skey_i;						/* session key index */
	ushort		ikey_i;						/* initialization vector index */
	ushort		method;						/* control word */
}	cyInfo;

/* Info passed from PA to EP */

typedef struct {
	ulong		pvector;					/* protocol vector */
	ushort		socket;						/* socket/port id */
}	epInfo;

/* Info passed from EP to CS */

typedef struct {
	CIBptr		cib;						/* Pointer to CIB for CS module */
	ushort		nnc;						/* NNC index */
}	csInfo;

/* Frame information structure passed from PA and to CS */

typedef struct {
	EntityID	pe; 						/* Protected Entity ID */
	EntityID	upe;						/* UnProtected Entity ID */
	union  {
		cyInfo	cy;							/* key index and ctrl information */
		epInfo	ep;							/* Protocol oriented information */
		csInfo	cs;							/* connection oriented information */
	}			info;						/* module specific information */
#ifdef CISCO
	struct cisco_ {
	    ulong src;							/* Original src & dst IP addr */
	    ulong dst;
		void *ptr;							/* Pointer to return to glue func */
	} cisco;
#endif
}	epFrameInfo;

/*------------------------ Mg Interface definition ---------------------------- */

typedef struct
{
	EntityID	pe; 						/* Protected Entity ID */
	EntityID	upe;						/* UnProtected Entity ID */
	CIBptr		cib;						/* ptr to CIB (passed back) */
	ushort		req;						/* request (see macros above) */
	ushort		nnc;						/* index of NNC to use for PE */
}	MgRequest;

#ifndef CONNSTUP_H
#include "../crypto/sdu/include/connstup.h"
#endif

/*******************************************************************************
							GLOBAL MACROS
*******************************************************************************/

/* MACROS TO BE INCLUDED BY OTHER FILES DURING INTEGRATION */

#define CS_SEMAPHORE	0x6969
#define NNC_AUTO		0

#ifdef NO_PEDEF_NNC_TABLE
	#define NNC_UPE			140		/*...patch... */
#else
	#define NNC_UPE			MAX_NNC_SIZE
#endif

#ifndef EP_FRM_C
/*****************************************************************************
								 PUBLIC GLOBAL
*****************************************************************************/

/*=============================== MIB GLOBALS ================================= */

/*--------------------- Required Configuration Parameters --------------------- */

extern  PEdefs	epPEdef;					/* PE Init array {Addr, PID, NNC) */

extern  ushort	epPEnum;					/* # defs in pe[] */
											/* TO BE DELETED WHEN THINGS STABLE */

extern  ushort	epPEnodes;					/* # PET nodes to allocate */
extern  ushort 	epPEThash;					/* # entries per pet hash table */
extern  ushort	epCOTnodes;					/* # COT nodes per page */
extern  ushort	epCOThash;					/* # entries per COT hash table */
extern  ushort	epLockPET;					/* Do NOT add new PEs to PET */
extern  ushort	epOpen;						/* Pass non-PE traffic */
extern	ushort	epPassIntraNet;				/* Pass Intra Entity traffic */
extern  ushort	epMaxKeyAge;				/* Max time to use keys (min) */
extern	ushort	epSokDefs;					/* number of socket definitions */

/*---------------------- EP Control and Status Info --------------------------- */

extern  ushort	epOldKey;					/* Oldest key in use (min) */

/*=============================== SYSTEM GLOBALS ============================== */

/*------------------------ EP Control + Status Info --------------------------- */

extern  bits16	ep_ctrl;					/* EP control bit flags */
extern  ushort	CsRtxcSignal_g;				/* CS to be signalled on ISR exit */

/*---------------------------- Task Comm Queues ------------------------------- */

extern  IQueue	iq_mg_req;					/* Manager Request Queue */
extern  IQueue	iq_cs_frm;					/* EP->CS frame queue */

/*---------------------------- PET/COT Data Base ------------------------------ */

extern PETnode far	*pet;					/* PET hash node pool */
extern ushort  far	*pet_hash;				/* PET hash table (of indeces) */
extern ushort		 pet_free;				/* index into PET free list */
extern ushort		 pet_cycle;				/* index into PET recycle list */
extern COTpage far	*cot;					/* array of COT page definitions */
extern SokPtr		 sok_free;				/* ptr to linked list of socket defs */

#ifndef NO_PEDEF_NNC_TABLE
extern MgRequest     epMgReq[];				/* Add/Delete PET/COT request from Mg */
#endif

/*****************************************************************************
								 FUNCTION PROTOTYPES
*****************************************************************************/

extern int  ep_init  ( void );				/* EP init function */
extern void ep_frame ( void far *f );		/* determine action  */
#ifdef CISCO
extern CIBptr ep_get_cotptr (void *);
#endif

#endif
#endif
/*------------------------- ep_frm.h END OF FILE ------------------------------ */

