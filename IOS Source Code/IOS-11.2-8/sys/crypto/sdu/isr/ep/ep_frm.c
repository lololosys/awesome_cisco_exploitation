/* $Id: ep_frm.c,v 1.1.68.4 1996/07/01 12:27:11 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/isr/ep/ep_frm.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: ep_frm.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ep_frm.c,v $
 * Revision 1.1.68.4  1996/07/01  12:27:11  xliu
 * CSCdi61743:  decryption does not use cached pak pointer correctly.
 * Branch: California_branch
 *
 * Revision 1.1.68.3  1996/06/28  23:40:51  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 1.1.68.2  1996/06/07  00:27:18  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Cleanup to let compile with -Wall.
 *
 * Revision 1.1.68.1  1996/04/19  14:57:52  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.4  1996/01/18  23:23:32  bew
 * Branch: IosSec_branch
 * The crypto session manager no longer forks off connection messages but
 * handles them itself serially. This is due to SDU connection setup code
 * not handling the multitasking gracefully.
 *
 * Revision 1.1.2.3  1995/12/20  23:32:04  bew
 * Branch: IosSec_branch
 * Context is now freed after a sign operation. Before the callback is
 * called a pointer to the pak is taken so it can be used after the
 * callback returns.
 *
 * Revision 1.1.2.2  1995/12/12  23:12:21  bew
 * Branch: IosSec_branch
 * Better formatting of debug messages.
 *
 * Revision 1.1.2.1  1995/11/28  01:49:55  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:23  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/ISR/EP/EP_FRM.C 27    8/10/95 11:52a Peacock 
*
* DESCRIPTION:	Contains Entity Protection Init and Interrupt Functionality
*
* NOTES:	   	The EP Custodian is NOT defined in this file. It is defined in
*			   	the $/SecureLAN/SDU/Task/EPc project
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: EP_FRM.C 
* 
* *****************  Version 27  *****************
* User: Peacock      Date: 8/10/95    Time: 11:52a
* Updated in $/SecureLAN/SDU/ISR/EP
* Added Crypto Key index initialization during ep_init().
* 
* *****************  Version 26  *****************
* User: Tchi         Date: 6/01/95    Time: 6:11p
* Updated in $/SecureLAN/SDU/ISR/EP
* changed to use NO_PEDEF_NNC_TABLE
* 
* *****************  Version 24  *****************
* User: Sduhost      Date: 5/23/95    Time: 5:01p
* Updated in $/SecureLAN/SDU/ISR/EP
* Added epPassIntraNet variable to specify whether to pass or process
* Intra-Network traffic
* 
* *****************  Version 23  *****************
* User: Croyle       Date: 5/04/95    Time: 2:40p
* Updated in $/SecureLAN/SDU/ISR/EP
* Optimized just a tad where ep_frame() calls cryp_frame().
* 
* *****************  Version 19  *****************
* User: Sduhost      Date: 4/17/95    Time: 4:15p
* Updated in $/SecureLAN/SDU/ISR/EP
* Modified CIB initialization from (0) to (BAD_CONN) - used in counter
* updating.
* 
* *****************  Version 18  *****************
* User: Sduhost      Date: 4/14/95    Time: 2:01p
* Updated in $/SecureLAN/SDU/ISR/EP
* Added #include IOMGR.H.
* 
* *****************  Version 17  *****************
* User: Sduhost      Date: 4/13/95    Time: 7:05p
* Updated in $/SecureLAN/SDU/ISR/EP
* Implemented SDU States in ep_frm() logic. 
* 
* *****************  Version 16  *****************
* User: Tchi         Date: 4/07/95    Time: 7:38p
* Updated in $/SecureLAN/SDU/ISR/EP
* SNMP table changes. Define NO_PEDEF_NNC_TABLE if you do not want it to
* take effect.
* 
* *****************  Version 15  *****************
* User: Croyle       Date: 2/21/95    Time: 6:39p
* Updated in $/SecureLAN/SDU/ISR/EP
* Separated Startup Parameters used by EP Custodian to EPC specific
* variables:
* 
* ep_frm::epPEnodes->ep_cust::pet_nodes,  
* ep_frm::epCOTnodes->ep_cust::cot_nodes, 
* ep_frm::epCOThash->ep_cust::cothash_size
* 
* *****************  Version 14  *****************
* User: Sduhost      Date: 1/27/95    Time: 1:48p
* Updated in $/SecureLAN/SDU/ISR/EP
* Commented some printfs for Integration 2A 2 Port Demo.
* 
* *****************  Version 13  *****************
* User: Croyle       Date: 1/09/95    Time: 4:16p
* Updated in $/SecureLAN/SDU/ISR/EP
* Converted main function, ep_frame(), to call new crypto functions in
* new cy_frame().
* 
* *****************  Version 12  *****************
* User: Croyle       Date: 1/03/95    Time: 1:49p
* Updated in $/SecureLAN/SDU/ISR/EP
* Modified naming convention for all Croyle files.
* 
* *****************  Version 11  *****************
* User: Croyle       Date: 12/16/94   Time: 10:38a
* Updated in $/SecureLAN/SDU/ISR/EP
* Changed DEBUG() string symbols to constants embedded in DEBUG macro
* call.
* 
* *****************  Version 10  *****************
* User: Sduhost      Date: 12/05/94   Time: 11:15a
* Updated in $/SecureLAN/SDU/ISR/EP
* intgr1D
* 
* *****************  Version 9  *****************
* User: Sduhost      Date: 3/16/94    Time: 3:08p
* Updated in $/SecureLAN/SDU/ISR/EP
* Final Update for Integration 1C+.
* 
* *****************  Version 8  *****************
* User: Croyle       Date: 11/02/94   Time: 9:15a
* Updated in $/SecureLAN/SDU/ISR/EP
* Changed epPEdef from ptr allocated at runtime to array of size
* MAX_SDU_PE.
* 
* *****************  Version 7  *****************
* User: Croyle       Date: 11/01/94   Time: 6:46p
* Updated in $/SecureLAN/SDU/ISR/EP
* Modified DEBUG print statements to use the new DEBUG.C approach
* 
* *****************  Version 6  *****************
* User: Tchi         Date: 10/28/94   Time: 2:20p
* Updated in $/SecureLAN/SDU/ISR/EP
* BUG FIX:  chi didn't find this ... I did!  He chose weird addresses so
* I found an init bug in init_cot() (was init'ing hash table starting at
* index 1 instead of 0).
*
* *****************  Version 5  *****************
* User: Tchi         Date: 10/27/94   Time: 5:20p
* Updated in $/SecureLAN/SDU/ISR/EP
* Jim: fixed frameMode
* 
* *****************  Version 4  *****************
* User: Croyle       Date: 10/14/94   Time: 7:40p
* Updated in $/SecureLAN/SDU/ISR/EP
* Ooops...testing revealed a typo. No other changes.
* 
* *****************  Version 3  *****************
* User: Croyle       Date: 10/14/94   Time: 7:32p
* Updated in $/SecureLAN/SDU/ISR/EP
* Changed calling of cy_frame() when frameMode = frameProcess.
* ep_frame() now passes the protocol ID via the EntityID PID field
* (f->epEntry.ep.pid) for Cy's extraction of an IV generation value and
* index into frame for beginning cryptographic process.
* (also made observation and simplified action determination a little)
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 10/14/94   Time: 1:45p
* Updated in $/SecureLAN/SDU/ISR/EP
* Modified interface to Cy.  Now copy Crypto parameters into epEntry data
* structure (another union element added).  This is in place of passing
* the parameters via the stack (other files modified: ep.h, ep-debug.h).
* 
* *****************  Version 1  *****************
* User: Croyle       Date: 10/13/94   Time: 10:30a
* Created in $/SecureLAN/SDU/ISR/EP
* Entity Protection software.  The EP initialization routine (ep_init())
* allocates memory for the PET/COT database and inits the data
* structures.  The EP frame processing routine (ep_frame()) is called
* from the PA handler during a frame interrupt.  This software may be
* tested using the files in the project folder $SecureLAN/SDU/Test/EP/.
*******************************************************************************/

#define EP_FRM_C

/*****************************************************************************
								 INCLUDE FILES
*****************************************************************************/
#include "../crypto/sdu/include/cylink.h"
#ifndef CISCO
#include <alloc.h>							/* memory allocation prototypes */
#include <mem.h>							/* copy routine (memcpy() is inline) */
#endif

/* Common Include Files */

#include "../crypto/sdu/include/iqueue.h"
#include "../crypto/sdu/include/debug.h"

#ifndef CISCO
#include "phybufm.h"
#endif /* CISCO */
#include "../crypto/sdu/include/pa/pactl.h"
#include "../crypto/sdu/include/pa/paisr.h"
#include "../crypto/sdu/include/iomgr.h"
#include "../crypto/sdu/include/ep_frm.h"
#include "../crypto/sdu/include/cy_frm.h"
#include "../crypto/sdu/include/ep_cust.h"

/* RTXC related files */

#ifndef CISCO /* Missing! */
#include "rtxcapi.h"
#endif
#include "../crypto/sdu/include/rtxcinfo.h"

#ifdef CISCO
#include "../crypto/crypto_glue_sdu.h"
#endif
/******************************************************************************
									FILE MACROS
******************************************************************************/

/* minimum values for configuration parameters */

#define MIN_PNODES	2						/* min # PET nodes (one is NULL) */
#define MIN_PHASH	1						/* min # PET hash entries */
#define MIN_CNODES	3						/* min # COT nodes per page */
#define MIN_CHASH	1						/* min # COT hash entries */

/******************************************************************************
								PUBLIC GLOBALS
******************************************************************************/

/*=============================== MIB GLOBALS ================================= */

/*--------------------- Required Configuration Parameters --------------------- */

PEdefs  epPEdef;							/* PE Init array (Entity IDs) */

ushort	epPEnum		= 0;					/* # defs in pe[]. */
											/* TO BE DELEATED WHEN THINGS STABLE */
ushort	epPEnodes		= DEF_PNODES; 		/* # PET nodes to allocate */
ushort 	epPEThash		= DEF_PHASH;  		/* # entries per pet hash table */
ushort	epCOTnodes		= DEF_CNODES; 		/* # COT nodes per PE page */
ushort	epCOThash		= DEF_CHASH;  		/* # entries per COT hash table */
ushort	epLockPET		= DEF_LOCK;	  		/* Do NOT add new PEs to PET */
ushort	epOpen			= DEF_OPEN;	  		/* Pass non-PE traffic in clear */
ushort  epPassIntraNet	= DEF_INTRANET;		/* pass intra-network traffic in clear */
ushort	epMaxKeyAge		= DEF_MAXKEYAGE;	/* Max time to use keys (min) */
ushort	epSokDefs		= DEF_SOKDEFS;		/* number of socket definitions */

/*---------------------- EP Control and Status Info --------------------------- */

ushort	epOldKey	= 0;					/* Oldest key in use (min) */

/*=============================== SYSTEM GLOBALS ============================== */

/*------------------------ EP Control + Status Info --------------------------- */

bits16	ep_ctrl  = 0;						/* EP control bit flags */

/*---------------------------- Task Comm Queues ------------------------------- */

IQueue	iq_mg_req;							/* Manager Request Queue */
IQueue	iq_cs_frm;							/* EP->CS frame queue */

/*---------------------------- PET/COT Data Base ------------------------------ */

PETnode far	*pet;							/* PET hash node pool */
ushort  far	*pet_hash;						/* PET hash table (of indeces) */
ushort		 pet_free = 0;					/* index into PET free list */
ushort		 pet_cycle = 0;					/* index into PET recycle list */
COTpage far	*cot;							/* array of COT page definitions */
SokPtr		 sok_free = NULL;				/* ptr to linked list of socket defs */

#ifndef NO_PEDEF_NNC_TABLE
MgRequest    epMgReq[MAX_SDU_PE];			/* Add/Delete PET/COT request from Mg */
#endif

/******************************************************************************
								STATIC GLOBALS
******************************************************************************/

/* list processing variables */

static bits16	pet_mask = 0;				/* pet table hashing mask */
static bits16	cot_mask = 0;				/* COT hash table mask */
static ushort	cot_pg_i = 1;				/* page assignment index */

/* PID shifting value for protocol category, PIDs are IP, IDP, IPX, DDP */
/* (future PIDs are also included as DRP, VIP, NTBS) - index 0 is undefined */

#ifndef CISCO
static uchar pv_base[8] = { 0, 29, 22, 25, 19, 17, 14, 13 };
static uchar pv_ofs[16] = { 0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3 };
#endif /* CISCO */

/******************************************************************************
						PUBLIC DECLARATIONS (prototypes)
******************************************************************************/

int  ep_init  ( void );
void ep_frame ( void far *f );

/******************************************************************************
						STATIC DECLARATIONS (prototypes)
******************************************************************************/

static  int		near _fastcall init_pet ( void );
static  int		near _fastcall init_cot ( void );
static  ushort	near _fastcall add_pet (EntityID id);
#ifndef CISCO
static 	void	near _fastcall del_pet (EntityID id);
#endif
static  ushort	near _fastcall get_pet (Pa2EpFrameControl far *f);
static  ushort	near _fastcall get_cot (Pa2EpFrameControl far *f, ushort page);
#ifndef CISCO
static  void	near _fastcall post_cs (Pa2EpFrameControl far *f, CIBptr cib, ushort nnc);
static  CIBptr	near _fastcall req_cot (EntityID *pe, EntityID *upe, ushort req);
#endif

/******************************************************************************
						PUBLIC FUNCTION DEFINITIONS
******************************************************************************/

/******************************************************************************
* NAME:			int ep_init ( void )
*
* DESCRIPTION:	allocates memory for and initializes PET and COT pages
*
* INPUTS:
*	PARAMETERS:	void
*	GLOBALS:	ushort	epLockPET			Do NOT add new PEs to PET
*				ushort	epOpen				Allow comm with non-PEs
* OUTPUT:
*	PARAMETERS:	void
*	GLOBALS:	bits16	pet_mask			pet table hashing mask
*				COTpage	(_far*) cot			array of COT page definitions
*				bits16	cot_mask			COT hash table mask
*				bits16	ep_ctrl				EP control flags
*	RETURN:		int							0 = success, else see EP.H
*
* NOTES:		most of the global data is intialized from the two static
*				functions init_pet() and init_cot().
*******************************************************************************/

int ep_init ( void )
{
	int retval;
#ifndef NO_PEDEF_NNC_TABLE
	int	i;
#endif

	/* init inter-module communication queues (for Mg and CS modules) */

	IQ_INIT(iq_mg_req);
	IQ_INIT(iq_cs_frm);

	/* init PET and COT structures */

	ep_ctrl = 0;								/* clear PET locking flag... */
	if ((retval = init_cot()) == EP_OKAY)		/* ...so nodes can be added */
		 retval = init_pet();

	/* set ep_ctrl flags now that PET is initialized */

	ep_ctrl = (epLockPET ? LOCK_PET : 0) | (epOpen ? OPEN_SDU : 0);

	/* set ep custodian variables */

	pet_nodes = epPEnodes;						/* # PET nodes to allocate */
	cot_nodes = epCOTnodes;						/* # COT nodes per PE page */
	cothash_size = epCOThash;					/* # entries per COT hash table */

	/* mark all entries in Mg Request List as free */
#ifndef NO_PEDEF_NNC_TABLE
	memset(epMgReq, 0, sizeof(epMgReq));
	for (i = 0; i < MAX_SDU_PE; i++) epMgReq[i].req = MG_REQ_SERVED;
#endif

	return (retval);
}

#ifdef CISCO
CIBptr ep_get_cotptr (Pa2EpFrameControl *fc)
{
	ushort pe_frame;
	short  cot_index;

	if ((pe_frame = get_pet(fc)) == 0) {
		DEBUG("ep_get_cotptr: No PET");
		return(NULL);
	} 
	if ((cot_index = get_cot(fc, pe_frame)) == 0) {
		DEBUG("ep_get_cotptr: No COT");
		return(NULL);
	}
	return((CIBptr) &cot[pe_frame].node[cot_index].cib);
}
#else /* !CISCO */
/*******************************************************************************
* NAME:			int ep_frame ( void far *f )
*
* DESCRIPTION:	This function is called by the Protocol Analysis module whenever
*				a new frame arrives on either LAN port.
* INPUTS:
*	PARAMETERS:	void far* f				PA ctrl block (Pa2EpFrameControl)
*	GLOBALS:	IQueue iq_mg_req		Manager request queue
*
* OUTPUT:
*	PARAMETERS:	void far* f				May modify f->frameMode field
*	GLOBALS:	IQueue iq_mg_req		( effect of IQ_GET() )
*	RETURN:		int						0=no-action, else signal CS
*
* NOTES:		If this routine returns a value greater than zero then value
*				is equivalent to the RTXC SEMAPHORE tag to be posted (could be
*				for Mg or CS modules).
*******************************************************************************/

/* MACRO for converting from void pointer to frame pointer */

#define FRMPTR ((Pa2EpFrameControl far *)f)

void ep_frame ( void far *f )
{
	ushort	p,c,i,j;					/* temp variables */
	ulong	pv;							/* temp var (protocol vectors) */
	bits16	act;						/* computed action */
	MgRequest *req;						/* Manager request pointer */
	CIBptr	cib;						/* CIB pointer */
	SokPtr	sok;						/* socket pointer */
	ushort	val;						/* temp var (socket/port value) */
	epFrameInfo *e;						/* temp Frame Info ptr */

	/* next, see if PA wants this one just passed onward */

	if (FRMPTR->frameMode==FrameBypass)	/* if PA wants to pass frame in clr */
		pass_frame(FRMPTR);

	/* first see if we're OFFLINE (or worse...) */

	else if (sduState < SDU_LOCKED)
		if (ep_ctrl & OPEN_SDU)
			pass_frame(FRMPTR);
		else
			toss_frame(FRMPTR);
	else
		/*------------------- DETERMINE ACTION FOR FRAME ----------------------- */

		/* search PET for PE; and then search COT for UPE */

		if (!(p=get_pet(f)))					/* if no PET node */
			switch (FRMPTR->frameMode)
			{
				case FrameBypass:	pass_frame(FRMPTR); break;
				case FrameReject:	toss_frame(FRMPTR); break;
				case NncFrame:		DEBUG("ep_frame: No PET, NNC frame ignored"); break;
				default:			DEBUG("ep_frame: No PET, bad frame mode"); break;
			}
		else if (!(c=get_cot(f,p)))				/* if no COT node avail */
		{
			if ( (FRMPTR->frameMode != NncFrame) &&
				 (FRMPTR->frameMode != ConnectInit) )
				toss_frame(FRMPTR);
		}
		else
		{	/* determine action for this frame */

			cib	= &cot[p].node[c].cib;
			e	= &FRMPTR->epEntry;
			pv	= cib->pvector & e->info.ep.pvector;
			if (!pv)
				if (!(cib->status & OPEN_NODE))
					toss_frame(FRMPTR);
				else
					pass_frame(FRMPTR);
			else
			{	/* find msbit of protocol vector by shifting into sign bit... */
				/* interesting twist: the nature of the protocol vector allows */
				/* for only one layer three and one layer two bit to be set at */
				/* one time.  Furthermore, only layer three bits may be set in */
				/* the socket mask (by definition). Thus, the protocol index can */
				/* be computed prior to applying the socket mask. */

				j = pv_base[e->pe.pid];
				i = j + pv_ofs[(ushort)(pv >> j)];
				if ( ((val = e->info.ep.socket)!=0) &&
					 ((sok = cib->socket)!=NULL) &&
					 ((pv & sok->mask)!=0) &&
					 ((val >= sok->range[i].low) && (val <= sok->range[i].high)) )
				{
					if (!i)	act = (bits16)((sok->act_msb<<1)|sok->act_lsb);
					else   	act = (bits16)((sok->act_msb>>(i-1))|(sok->act_lsb>>i));
				}
				else
				{
					if (!i)	act = (bits16)((cib->act_msb<<1)|cib->act_lsb);
					else   	act = (bits16)((cib->act_msb>>(i-1))|(cib->act_lsb>>i));
				}

				/* process computed action. NOTE: the actual L3 protocol index */
				/* is required in order for the Cy module to encrypt the approp. */
				/* part of the frame; EVEN IF the L2 protocol actions was used. */

				switch (act&3)
				{
					case 0:		pass_frame(FRMPTR);	break;
					case 1:		e->pe.pid = j+pv_ofs[(ushort)(e->info.ep.pvector >> j)];
								*((long*) &e->info)   = *((long*) &cib->crypto);
								*((short*)&e->info+2) = *((short*)&cib->crypto+2);
								cryp_frame(FRMPTR);	break;
					case 2:
					case 3:		toss_frame(FRMPTR);	break;
				}
			}
		}
	/*----------------------- SERVICE MANAGER REQUEST ------------------------- */

	IQ_GET(iq_mg_req,req);
	if (req)
	{
		switch (req->req)
		{
			case MG_ADD_PET:	DEBUG("ep_frame: MANAGER REQUEST: add pet entry");
								add_pet (req->pe);	break;
			case MG_DEL_PET:	DEBUG("ep_frame: MANAGER REQUEST: del pet entry");
								del_pet (req->pe);	break;
			case MG_ADD_COT:	DEBUG("ep_frame: MANAGER REQUEST: add cot entry");
								req->cib  = req_cot(&req->pe,&req->upe,MG_ADD_COT);
								break;
			case MG_DEL_COT:	DEBUG("ep_frame: MANAGER REQUEST: del cot entry");
								req_cot (&req->pe,&req->upe,MG_DEL_COT);  break;
			default:			DEBUG("ep_frame: MANAGER REQUEST: invalid request");
								break;
		}
		req->req |= 0x8000;			/* signal Mg that request has been serviced */
	}
}
#endif /* !CISCO */

/*******************************************************************************
* STATIC FUNCTION DEFINITIONS
*******************************************************************************/

/*******************************************************************************
* NAME:			int init_pet ( void )
*
* DESCRIPTION:	This function is called by the ep_init() function to allocate
*				and initialize the Protected Entity Table (PET) data structures.
* INPUTS:
*	PARAMETERS:	none
*	GLOBALS:	PEinfo	epPEdef[]			PE Init array {Addr,PID,NNC index)
*				ushort	epPEnum				# defs in pe[]
*				ushort	epPEnodes			# PET nodes to allocate
*				ushort 	epPEThash			# entries per pet hash table
*				COTpage	cot[]				COT Page definitions
* OUTPUT:
*	PARAMETERS:	none
*	GLOBALS:	PETnode	*pet				PET node pool
*				ushort	*pet_hash			PET hashing table
*				ushort 	pet_free			start index of PET entry free list
*				ushort 	pet_cycle			start index of PET recycling list
*				ushort	pet_mask			PET hashing mask
*				ushort 	epPEThash			( only if invalid config value )
*				ushort	epPETnodes			( only if invalid config value )
*	RETURN:		int							EP_OKAY=success, else see EP.H
*
* NOTES:		Note that in order to maintain a NULL entry the first PET
*				node (with index = 0) is never used.
*
*				If epPEThash is not an even power of two, this routine rounds
*				the number up to the next power of two.
*
* WARNING:		INIT_COT() MUST BE CALLED BEFORE THIS ROUTINE TO INIT PAGES.
*******************************************************************************/

static int near _fastcall init_pet ( void )
{
	EntityID	id;							/* temporary entity id for add_pet() */
	ushort		i,j;						/* 16-bit temporary variables */
	long		k;							/* 32-bit temporary variable */
	int 		retval=EP_OKAY;				/* return value */
#ifndef NO_PEDEF_NNC_TABLE                    /* 16-bit temporary variable */
	ushort		m = 0;
#endif

	/* validate configuration parameters */

	if (epPEThash < MIN_PHASH)  epPEThash = MIN_PHASH;

	for (i=j=epPEThash-1; j;) 				/* validate PET hash table size */
	{
		j >>= 1;  i |= j;					/* assures a mask of all 1s */
	}
	pet_mask = i;                  			/* init hashing mask */
	epPEThash = ++i;

	/* allocate and initialize data structures */

	k = (long)epPEThash * (long)sizeof(ushort);
	if ( k  > 65535L )						/* check size of hashing table */
	{
		retval = EP_ERR_SIZ_PH;
		DEBUG("init_pet: ERROR: bad hashing table size");
	}
	else
	{
		pet_hash = malloc((int)k);	/* allocate memory for hashing table */
		if (pet_hash == NULL)
		{
			retval = EP_ERR_MEM_PH;			/* not enuf memory for PET hash tbl */
			DEBUG("init_pet: ERROR: not enough memory for hashing table");
		}
		else
			if ( (k=(long)epPEnodes*(long)sizeof(PETnode)) > 65535L )
			{
				retval = EP_ERR_SIZ_PN;				/* check for valid PET size */
				DEBUG("init_pet: ERROR: bad pet size (too many nodes)");
			}
			else
			{
				pet = malloc((int)k);	/* allocate PET memory */
				if (pet == NULL)					/* if not enuf memory... */
				{
					retval = EP_ERR_MEM_PN;			/* ...for PET node pool */
					DEBUG("init_pet: ERROR: not enough memory for pet nodes");
				}
				else
				{	/* initialize pet hashing table */

					for (i=0; i<epPEThash; i++) pet_hash[i] = 0;

					/* initialize pet nodes and link free list */

					pet_free = 1;					/* 1st free node */
					pet_cycle = 0;					/* recycle list is empty */
					for (i=1; i<epPEnodes; i++)
					{
						pet[i].hash.list = 0;		/* not connected to hash list */
						pet[i].free = i+1;   		/* link node into free list */
						pet[i].page = 0;			/* set page index to NULL */
					}
					pet[i-1].free = 0;				/* reset last free node link */

					/* add initial pet nodes defined in epPEdef[] */

					if (epPEdef != NULL)
					{
/*						DEBUG("init_pet: Nodes Initialized, Constructing PET table..."); */

						#ifdef NO_PEDEF_NNC_TABLE
						for (i=0; i < epPEnum; i++)
						#else
						for (i=0; i < MAX_SDU_PE; i++)
						#endif

						{
						#ifndef NO_PEDEF_NNC_TABLE
							if (epPEdef[i].pid)		/*a non-zero entry */
							{
							m++;
						#endif
							id.adr = epPEdef[i].adr;
							id.pid = epPEdef[i].pid;
							id.hash = computeHashValue(id.adr,id.pid);
							if ((j=add_pet(id)) > 0)
								cot[(pet[j].page)].nnc = epPEdef[i].nnc;
						#ifndef NO_PEDEF_NNC_TABLE
							}
						#endif
						}

						#ifndef NO_PEDEF_NNC_TABLE
						i = m;
						#endif
						i++;						/* init remaining page nnc's */
					}
					else
						i = 1;						/* init all cot page nnc's */
					for (; i < epPEnodes; i++)
						cot[i].nnc = NNC_AUTO;		/* use automatic NNC (learning mode) */
				}
			}
	}
	return (retval);
}

/*******************************************************************************
* NAME:			int init_cot ( void )
*
* DESCRIPTION:	This function is called by the ep_init() function to allocate
*				memory for and initialize the COnnection Table (COT) data.
*
* INPUTS:
*	PARAMETERS:	void
*	GLOBALS:	ushort	epPEnodes			allocate a page per PE node
*				ushort	epCOTnodes			# COT nodes per page
*				ushort	epCOThash			# entries per COT hash table
*				ushort	epSokDefs			# Socket definitions
* OUTPUT:
*	PARAMETERS:	void
*	GLOBALS:	COTpage	*cot				array of COT page definitions
*				ushort	cot_mask			COT hashing mask
*				SokPtr	sok_free			Socket structure free list
*				ushort	epCOThash			( only if invalid config value )
*				ushort	epCOTnodes			( only if invalid config value )
*				ushort	epSokDefs			( only if run out of memory )
*	RETURN:		int							EP_OKAY=success, else see EP.H
*
* NOTES:		All of the hash tables and COT node pools are also allocated 
*				(and nodes init'd) for each page.
*
*				Note that COT node pools can use their first node (to maintain 
*				a NULL index) since the page begins with a hash table.  
*******************************************************************************/

static int near _fastcall init_cot ( void )
{
	ushort	i,j;							/* 16-bit temporary variables */
	long	h,n;							/* 32-bit temporary variable */
	int 	retval = EP_OKAY;				/* return value */
	SokPtr	ptr;							/* temp far ptr (used in socket ini) */
	
	/* validate configuration parameters  */

#ifdef CISCO
	epPEnodes = crypto_glue_get_sdu_entities();
	epCOTnodes = crypto_glue_get_sdu_connections();
#endif
	if (epPEnodes  < MIN_PNODES) epPEnodes  = DEF_PNODES;
	if (epCOTnodes < MIN_CNODES) epCOTnodes = DEF_CNODES;
	if (epCOThash  < MIN_CHASH)  epCOThash  = DEF_CHASH;

	for (i=j=epCOThash-1; j; ) 				/* validate COT hash size, set mask */
	{
		j >>= 1;  i |= j;					/* assures a mask of all 1's */
	}
	cot_mask = i;							/* setup hashing mask */
	epCOThash = ++i;

	/* allocate and initialize data structures */

	n = (long)epPEnodes*(long)sizeof(COTpage);	/* compute size of page table */
	if ( n > 65535L )
	{
		retval = EP_ERR_SIZ_CP;				/* ERROR: page table too big! */
		DEBUG("init_cot: ERROR: bad hashing table size");
	}
	else
	{
		cot = malloc((int)n);	/* allocate COT page table */
		if ( cot == NULL )
		{
			retval = EP_ERR_MEM_CP;			/* no memory for cot page table */
			DEBUG("init_cot: ERROR: not enough memory for hashing table");
		}
		else
		{	/* allocate COT Page Memory */

			h = (long)epCOThash*(long)sizeof(ushort);  	/* COT hash table size */
			n = (long)epCOTnodes*(long)sizeof(COTnode);	/* COT node pool size */
			if ( (n > 65535L) || (h > 65535L) )
			{
				retval = EP_ERR_SIZ_CN;
				DEBUG("init_cot: ERROR: bad cot size (too many nodes per page)");
			}
			else
			{
				for (i=1; i<epPEnodes; i++)				/* page alloc loop */
				{
					/* allocate memory for COT node pool and hashing table */

					cot[i].node = malloc((int)n);
					cot[i].hash = malloc((int)h);
					if ( !cot[i].node || !cot[i].hash )
					{
						retval = EP_ERR_MEM_CN;			/* no memory for COT nodes */
						DEBUG("init_cot: ERROR: out of memory allocating COT page");
						break;
					}
					else
					{   /* init this page's free list and COT nodes */

						cot[i].free = 1;
						cot[i].used = epCOTnodes-1;
						for (j=1; j<epCOTnodes; j++)
						{
							cot[i].node[j].hash.list = 0;
							cot[i].node[j].free.next =j+1;
							cot[i].node[j].free.prev =j-1;
							cot[i].node[j].cib.status = BAD_CONN;
							cot[i].node[j].cib.socket = NULL;
							cot[i].node[j].cib.crypto.skey_i = 0;
							cot[i].node[j].cib.crypto.ikey_i = 0;
						}
						cot[i].node[j-1].free.next = 0;

						/* initialize this page's COT hash table */

						for (j=0; j<epCOThash; j++) cot[i].hash[j] = 0;
					}
				}
				/* allocate and initialize Socket table free list */

				sok_free = NULL;
				for (i=epSokDefs,j=sizeof(SokDef); i; i--)
					if ( (ptr=(SokPtr)malloc(j)) != NULL )
					{
						ptr->free = sok_free;
						sok_free = ptr;
					}
					else
					{
						epSokDefs -= (i-1);			/* # actually allocated */
						i = 1;						/* terminate loop */
						DEBUG("init_cot: WARNING: Out of memory for socket tables");
					}
			}
		}
	}
	return (retval);
}

/*******************************************************************************
* NAME:			ushort add_pet (EntityID id)
*
* DESCRIPTION:	Add a node to PET table
*
* INPUTS:
*	PARAMETERS:	EntityID id				Definition of PE to add to PET
*	GLOBALS:	ushort 	cot_pg_i		COT page assignment variable
*
* OUTPUT:
*	PARAMETERS:	none
*	GLOBALS:	ushort	cot_pg_i		updated cot page index
*	RETURN:		ushort					Returns the new PET index
*
* NOTES:		This function has many similarities to find_pet() below, however
*				this routine is only used to add a node to the PET, and is thus
*				a bit simpler.
*******************************************************************************/

static ushort near _fastcall add_pet (EntityID id)
{
	ulong		val32 = id.adr;					/* 1st search value  */
	ushort		val16 = id.pid;					/* 2nd search value */
	ushort 		cur, prv=0;						/* Hash list traversing var.s */
	ushort		hsh = id.hash & pet_mask;		/* hashing index */

	/* Search for PET node */

	cur = pet_hash[hsh];						/* hash into PET list */
	while (((cur))&&((val32!=pet[cur].id.adr)||	/* search for same 32bit Address */
					 (val16!=pet[cur].id.pid))) /* and 16bit protocol ID */
	{
		prv = cur;								/* save previous node */
		cur = pet[cur].hash.next;				/* get next node in hash list */
	}

	/* if node found, move node to head of list and return COT page used */

	if ( cur )
	{
		if ( prv )								/* if not at head of hash list */
		{										/* re-link cur to head of list */
			pet[prv].hash.next = pet[cur].hash.next;
			pet[cur].hash.next = pet_hash[hsh];
			pet_hash[hsh] = cur;
			DEBUG("add_pet: ! Node already in PET (at head of list)");
		}
		else
			DEBUG("add_pet: ! Node already in PET");
	}
	else 	/* node NOT FOUND, see if a node can be allocated */

		if ( ((ep_ctrl&LOCK_PET)==0) && ((cur=pet_free)!=0) )
		{
			pet_free = pet[cur].free;			/* adjust free index */
			pet[cur].hash.next=pet_hash[hsh];	/* link to old list head */
			pet[cur].hash.list=hsh;				/* store hash list index */
			pet_hash[hsh] = cur;				/* re-link hash list */
			if (pet[cur].page == 0)				/* if not re-cycled, need a page */
				pet[cur].page = cot_pg_i++;		/* invalid page, use auto-page */
			pet[cur].id  = id;					/* copy entity to new PET entry */
/*			DEBUG("add_pet: PET Node Successfully added"); */
		}
#ifdef DEBUG_MSG
		else if (!(ep_ctrl&LOCK_PET))	{ DEBUG("add_pet: WARNING: Could not add Node, PET is LOCKED"); }
			 else 						{ DEBUG("add_pet: WARNING: Could not add Node, No Free Nodes"); }
#endif
	return (cur);
}

#ifndef CISCO
/*******************************************************************************
* NAME:			void del_pet (EntityID id)
*
* DESCRIPTION:	Add a node to PET table
*
* INPUTS:
*	PARAMETERS:	EntityID id				Definition of PE to add to PET
*	GLOBALS:	none
*
* OUTPUT:
*	PARAMETERS:	none
*	GLOBALS:	ushort pet_cycle		PET node recycling var (if PET deleted)
*	RETURN:		none
*
* NOTES:		This function is only called by the Manager Request logic at
*				the end of ep_frame().
*******************************************************************************/

static void near _fastcall del_pet (EntityID id)
{
	ulong		val32 = id.adr;					/* 1st search value  */
	ushort		val16 = id.pid;					/* 2nd search value */
	ushort 		cur, prv=0;						/* Hash list traversing var.s */
	ushort		hsh = id.hash & pet_mask;		/* hashing index */

	/* Search for PET node */

	cur = pet_hash[hsh];						/* hash into PET list */
	while (((cur))&&((val32!=pet[cur].id.adr)||	/* search for same 32bit Address */
					 (val16!=pet[cur].id.pid))) /* and 16bit protocol ID */
	{
		prv = cur;								/* save previous node */
		cur = pet[cur].hash.next;				/* get next node in hash list */
	}
	
	/* if node found, move node to head of list and return COT page used */
		
	if ( cur ) 
	{
		if ( prv )								/* if not at head of hash list */
			pet[prv].hash.next = pet[cur].hash.next;
		else 
			pet_hash[hsh] = pet[cur].hash.next;
		pet[cur].free = pet_cycle;				/* link to start of recylce list */
		pet_cycle = cur;
	}
}
#endif /* !CISCO */

/*******************************************************************************
* NAME:			ushort get_pet ( Pa2EpFrameControl *f )
*
* DESCRIPTION:	Find a node in the PET table
*
* INPUTS:
*	PARAMETERS:	Pa2EpFrameControl *f	pointer to PA's frame control block
*	GLOBALS:	PETnode	*pet			PET node memory pool
*				ushort	*pet_hash		PET hashing table
*				ushort	 pet_mask		PET hashing mask
*
* OUTPUT:
*	PARAMETERS:	Pa2EpFrameControl *f	may modify FrameMode, CIBptr fields
*	GLOBALS:	ushort	 pet_free		updated if a PET is allocated
*	RETURN:		short					The COT page to use for PE (-1=none)
*
* NOTES:		If a node is not found, the LOCK_PET flag is checked in the 
*				ep_ctrl variable.  If zero, and the pet_free list is not empty
*				a new PET node is added to the table.  The value currently in
*				cot_pg_i is used for the cot page and the variable is bumped.
*
*				If no PET is found or created then a COT page of 0 is returned.
*				In this case the calling function should not search the COT 
*				(i.e., ep_frame() should simply call cy_frame()).
*******************************************************************************/

static ushort near _fastcall get_pet (Pa2EpFrameControl far *f)
{
	ushort 		retval;							/* return cot page (0=none) */
	ulong		val32 = f->epEntry.pe.adr;		/* 1st search value  */
	ushort		val16 = f->epEntry.pe.pid;		/* 2nd search value */
	ushort 		hsh, hsh_i, cur, prv=0;			/* Hash list traversing var.s */
	PETnode		*n;								/* PET node pointer */

	/* Search for PET node */

	hsh = f->epEntry.pe.hash & pet_mask;		/* get index into hashing table */
	hsh_i = cur = pet_hash[hsh];				/* hash into PET list */
	while (((cur))&&((val32!=pet[cur].id.adr)||	/* search for same 32bit Address */
					 (val16!=pet[cur].id.pid))) /* and 16bit protocol ID */
	{
		prv = cur;								/* save previous node */
		cur = pet[cur].hash.next;				/* get next node in hash list */
	}

	/* if node found, move node to head of list and return COT page used */
		
	if ( cur )
	{		
		/*==================== PET NODE FOUND ======================== */

		retval = pet[cur].page;
		if ( prv )								/* if not at head of hash list */
		{										/* re-link cur to head of list */
			pet[prv].hash.next = pet[cur].hash.next;
			pet[cur].hash.next = hsh_i;
			pet_hash[hsh] = cur;
/*			DEBUG("get_pet: PET node found (moved to head of list)"); */
		}
/*		else */
/*			DEBUG("get_pet: PET node found (at head of list)"); */
	}
	else if ((ep_ctrl & LOCK_PET) || !(cur = pet_free) || (sduState == SDU_LOCKED))
	{
		/*================== CAN'T GET NEW PET NODE ======================= */

		if (f->frameMode == NncFrame)			/* don't throw away an NNC frame */
#ifdef CISCO
			;
#else
			post_cs(f,NULL,NNC_UPE);			/* post information to CS */
#endif
		else
		{
			f->frameMode = (ep_ctrl&OPEN_SDU ? FrameBypass:FrameReject);
			DEBUG("PET node not found, checking Open/Close flag");
		}
		retval = 0;
	}
	else  /*================= NEW PET NODE ALLOCATED ====================== */
	{
		n = &pet[cur];
		pet_free = n->free;						/* move free index */
		n->hash.next = hsh_i;					/* link to old list head */
		n->hash.list = hsh;						/* store hash list index */
		pet_hash[hsh] = cur;					/* re-link hash list */
		if (n->page == 0)						/* if not recycled... */
			n->page = cot_pg_i++;				/* ...get assigned page */
		n->id  = f->epEntry.pe;					/* copy entity id */
		retval = n->page;						/* return COT page */
		DEBUG("get_pet: PET node created");
	}
	return (retval);
}

/*******************************************************************************
* NAME:			short get_cot(Pa2EpFrameControl *f, ushort page)
*
* DESCRIPTION:	Find a COT node in specified COT page
*
* INPUTS:
*	PARAMETERS:	Pa2EpFrameControl *f	pointer to PA's frame control block
*				ushort	page			COT page index
*	GLOBALS:	COTpage	*cot			COT page table
*				ushort	 cot_mask		COT hashing mask
*				IQueue	*iq_cs_frm		Conn Setup Frame Queue
* OUTPUT:
*	PARAMETERS:	Pa2EpFrameControl *f	may modify FrameMode, CIBptr fields
*	GLOBALS:	ushort	 cot_free		updated if a COT is allocated
*	RETURN:		ushort					COT node index (0 = no node created).
*
* NOTES:		none
*******************************************************************************/

#ifdef CISCO
/*
 * Cisco uses this very differently than ep_frame(). We us get_cot() for the 
 * following reasons:
 *  1. To create a COT entry.
 * 	2. To find a CIB ptr for some use.
 * As best I can tell, ep_frame() uses it thusly:
 *  1. To create a COT entry.
 *  2. To enqueue an NNC for CS.
 *  3. To determine if a frame should be dropped (e.g., there exists a
 *     pending connection, or there are no COT entries available for this PET).
 */
#endif /* CISCO */

ushort near _fastcall get_cot (Pa2EpFrameControl far *f, ushort page)
{
	ulong 	val = f->epEntry.upe.adr;			/* pointer to search field */
	ushort 	hsh,cur,prv=0;						/* Hash list traversing var.s */
	COTnode far *nod = cot[page].node;			/* load COT node table */
	ushort	i;									/* temporary variables */
#ifdef CISCO
	short	retval = 0;							/* return value */
#else
	short	retval;								/* return value */
#endif
	CIBptr	cib;								/* CIB pointer */
	ushort	status;								/* temporary status variable */

	/* Search COT page for UPE node */

	hsh = f->epEntry.upe.hash & cot_mask;		/* compute hashing index */
	cur = cot[page].hash[hsh];					/* hash into COT list */
	while ( (cur) && (val!=nod[cur].id.adr) )	/* search hash list for node */
	{
		prv = cur;								/* save previous node */
		cur = nod[cur].hash.next;				/* get next node in hash list */
	}

	/* if node found, move node to head of list and return COT node index */
		
	if (cur)
	{
		cib = &nod[cur].cib;		
		status = cib->status | USED_NODE;
		cib->status = status;						/* set used flag */

		/* if this is an NNC frame pass it along to  */
		
		if ( f->frameMode == NncFrame ) 			/* if NNC frame arrived... */
		{                                           /* ...pass CIB pointer */
#ifdef CISCO
			/*
		 	 * Return the CIB to the caller, which is CS.
		 	 * NOTE: We can't update the status with set the pending bit here 
			 * because we don't always have a pending connection at this point 
			 * in the code, due to the number of additional reasons we call 
			 * this function.
		 	 */
			retval = cur;
#else
			if ( !(status & PEND_CONN) )			/* (if not already pending) */
				cib->status = status|PEND_CONN|USED_NODE;
			post_cs(f,cib,cot[page].nnc);	
			retval = 0;
/*			DEBUG("get_cot: Putting NNC frame into Queue and signalling CS"); */
#endif
		}
#ifdef CISCO
              /* 
               * We don't need this condition at all, because we only use this code
               * for connection setup, and it unduly complicates connection setup.
               */
#else

		/* if connection currently pending, reject frame */

		else if ( status & PEND_CONN )				/* if connection pending */
		{
			retval = 0;								/* do nothing */
			DEBUG("get_cot: Rejecting frame, Connection Pending");
		}
#endif
		/* if old crypto keys, re-connect */

		else if ( status & XCHG_KEYS )				/* check key exchange bit */
		{
#ifndef CISCO
			post_cs(f,cib,cot[page].nnc);			/*  post CS info to queue */
#endif
			if (cib->socket)
				cib->socket->mask = 0L;
			else if ((cib->socket = sok_free)!=NULL)
			{
				sok_free = sok_free->free;			/* move free socket ptr */
				cib->socket->mask = 0L;
			}
			/* set appropriate status bits and frame mode */

			cib->status = status|PEND_CONN|USED_NODE;
			if (f->frameMode != NncFrame)
				f->frameMode = ConnectInit;			/* wait for new connection */
			DEBUG("Key Timeout, Re-exchange Crypto Keys");
#ifdef CISCO
			/*
			 * We need to return the pointer still ... we manage the concept 
			 * of a 'bad' connection in the IOS code.
			 */
			retval = cur;
#else
			retval = 0;
#endif
		}
		else
		{
			retval = cur; 							/* return COT entry */
			if ( prv )								/* optimize list: normal frm */
			{										/* move node to head of list */
				nod[prv].hash.next = nod[cur].hash.next;
				nod[cur].hash.next = cot[page].hash[hsh];
				cot[page].hash[hsh] = cur;
			}
		}
#ifdef CISCO
		if ( f->frameMode == ConnectInit )	/* if a new connection ... */
		{
			if ( !(status & PEND_CONN) )	/* (if not already pending) */
			{
				cib->status = status|PEND_CONN|USED_NODE;
			}
		}
#endif
	}
	else if (sduState != SDU_LOCKED)
	{	
		/* COT node not found, allocate a new node from free list */

		cur = cot[page].free;
		for (i=3; ((cur))&&(nod[cur].cib.status&(LOCK_NODE|PEND_CONN))&&i; i--)
			cur = nod[cur].free.next;

		/* if all COT nodes are locked, REJECT the frame */

		if (!cur || !i)
		{
			/* POST ERROR!!! */
			retval = 0;
			DEBUG("ERROR: Cant alloc COT, nodes in free list are LOCKED");
		}
		else
		{	/* if node was linked to other hash list, 1st remove from list */

			cot[page].free = nod[cur].free.next;
			if ((i=nod[cur].hash.list)!=0)
			{
				if ((prv=cot[page].hash[i]) == cur)
					cot[page].hash[i] = nod[cur].hash.next;
				else
				{
					i = nod[prv].hash.next;
					while (i!=cur)
					{
						prv = i;
						i=nod[i].hash.next;
					}
					nod[prv].hash.next = nod[cur].hash.next;
				}
				DEBUG("New COT node RE-allocated, old node removed.");
			}

			/* initialize node in new hash list (attach socket structure) */

			nod = &nod[cur];
			cib = &nod->cib;
			nod->hash.next = cot[page].hash[hsh];
			nod->hash.list = hsh;
			cot[page].hash[hsh] = cur;
			if (cib->socket)
				cib->socket->mask = 0L;
			else if ((cib->socket = sok_free)!=NULL)
			{
				sok_free = sok_free->free;			/* move free socket ptr */
				cib->socket->mask = 0L;
			}
			nod->id = f->epEntry.upe;				/* copy UPE entity ID */

#ifdef CS_BYPASS
			cib->status = USED_NODE;
			cib->pvector = -1L;
			cib->act_msb =  0L;
			cib->act_lsb = -1L;
			cib->crypto.skey_i = 0x69;
			cib->crypto.ikey_i = 0x96;
			cib->crypto.method = 10;
			cib->crypto.time = 0;
			retval = cur;
#else
#ifndef CISCO
			post_cs(f,cib,cot[page].nnc);			/* post CS info to queue */
#endif
			cib->status |= (PEND_CONN|USED_NODE);
			if (f->frameMode != NncFrame)
				f->frameMode = ConnectInit;
#ifndef CISCO
			retval = 0;
#else
			retval = cur;
#endif
#endif
			DEBUG("get_cot: New COT node allocated");
		}
	}

	return (retval);
}

#ifndef CISCO
/*******************************************************************************
* NAME:			void post_cs(Pa2EpFrameControl *f, CIBptr cib, ushort nnc)
*
* DESCRIPTION:	Find a COT node in specified COT page
*
* INPUTS:
*	PARAMETERS:	Pa2EpFrameControl *f	pointer to PA's frame control block
*				CIBptr cib				Pointer to CIB
*				ushort nnc				Index of NNC to use
*	GLOBALS:	none
*
* OUTPUT:
*	PARAMETERS:	Pa2EpFrameControl *f	modify FrameMode if queuing error
*	GLOBALS:	none
*	RETURN:		void
*
* NOTES:		none
*******************************************************************************/

static void near _fastcall post_cs (Pa2EpFrameControl far *f, CIBptr cib, ushort nnc)
{
	ushort status;
	
	IQ_PUT(iq_cs_frm,f,status);				/* pass frame to CS module */
	if (!status)							/* if error passing frame */
	{
		f->epEntry.info.cs.cib = cib;
		f->epEntry.info.cs.nnc = nnc;
		KS_ISRsignal(CS_FRM_SEMA);			/* signal CS module */
	}
	else
		f->frameMode = FrameReject;			/* reject frame */
}

/*******************************************************************************
* NAME:			CIBptr req_cot (EntityID *pe, EntityID *upe, ushort req)
*
* DESCRIPTION:	Find a COT node in PET node page and perform cmd'd request
*
* INPUTS:
*	PARAMETERS:	EntityID	*pe			pointer to PE entity ID
*				EntityID	*upe		pointer to UPE entity ID
*	GLOBALS:	COTpage		*cot		COT page table
*				ushort		cot_mask	COT hashing mask
*				IQueue		*iq_cs_frm	Conn Setup Frame Queue
* OUTPUT:
*	PARAMETERS:	void
*	GLOBALS:	ushort		cot_free	updated if a COT is allocated
*	RETURN:		CIBptr					ptr to COT node CIB (NULL=none)
*
* NOTES:		The return value is only valid if a COT node is added. If no
*				COT node could be added, otherwise NULL is returned (if all
*				nodes are pending connection or locked).
*
*				ep_ctrl bit flag for LOCKING PET is temporarily modified in
*				this routine to assure that either a PET is not added (DEL)
*				or IS added (ADD) despite the ep_ctrl flag settings.
*******************************************************************************/

static CIBptr near _fastcall req_cot (EntityID *pe, EntityID *upe, ushort req)
{
	ushort	p;									/* pet index */
	ushort  page;								/* page index */
	ushort 	hsh,cur,prv;						/* Hash list traversing var.s */
	COTnode *nod;								/* load COT node table */
	ushort	i,ctrl;								/* temporary variables */
	CIBptr	cib,retval=NULL;					/* return value */

	/* setup control flag bits for command processing (restore before return) */

	ctrl = ep_ctrl;								/* save control flags */
	if (req == MG_DEL_COT)
		ep_ctrl |=  LOCK_PET; 					/* don't allow new PET */
	else if (req == MG_ADD_COT)
		ep_ctrl &= ~LOCK_PET;					/* allow new PET allocation */

	/* search for PET */

	if ((p=add_pet(*pe))!=0)					/* if PET node available */
	{
		page = pet[p].page;						/* get page index from pet node */
		nod  = cot[page].node;

		/* Search COT page for UPE node */

		hsh = upe->hash & cot_mask;				/* compute hashing index */
		cur = cot[page].hash[hsh];				/* hash into COT list */
		prv = 0;
		while ((cur)&&(upe->adr!=nod[cur].id.adr))	/* search hash list for node */
		{
			prv = cur;							/* save previous node */
			cur = nod[cur].hash.next;			/* get next node in hash list */
		}

		/* if node found, move node to head of list and return COT node index */

		if (cur)
		{
			if (req == MG_ADD_COT)
			{
				nod[cur].cib.status = PEND_CONN|USED_NODE;	/* set pending connection */
				retval = &nod[cur].cib;						/* get CIB pointer */
			}
			else if ( req == MG_DEL_COT )
				if (prv)
					nod[prv].hash.next = nod[cur].hash.next;
				else
				{
					i = nod[cur].hash.list;
					cot[page].hash[hsh] = nod[cur].hash.next;
				}
		}
		else if ( req == MG_ADD_COT )
		{
			/* COT node not found, allocate a new node from free list */

			cur = cot[page].free;
			for (i=3; ((cur))&&(nod[cur].cib.status & PEND_CONN)&&i; i--)
				cur = nod[cur].free.next;

			/* if a COT node was found (else REJECT frame)... */

			if (cur && i)
			{
				cot[page].free = nod[cur].free.next;
				if ((i=nod[cur].hash.list)!=0)
				{
					if ((prv=cot[page].hash[i]) == cur)
						cot[page].hash[i] = nod[cur].hash.next;
					else
					{
						i = nod[prv].hash.next;
						while (i!=cur)
						{
							prv = i;
							i=nod[i].hash.next;
						}
						nod[prv].hash.next = nod[cur].hash.next;
					}
				}
				/* initialize node in new hash list (attach socket structure) */

				nod = &nod[cur];
				cib = &nod->cib;
				nod->hash.next = cot[page].hash[hsh];
				nod->hash.list = hsh;
				cot[page].hash[hsh] = cur;
				cib->status = PEND_CONN;
				if (cib->socket)
					cib->socket->mask = 0L;				/* clear socket mask */
				else if ((cib->socket=sok_free)!=NULL)
				{
					cib->socket->mask = 0L;				/* clear socket mask */
					sok_free = sok_free->free;			/* move free socket ptr */
				}
				nod->id = *upe;							/* copy UPE entity ID */
				retval = cib;
			}
		}
	}
	ep_ctrl = ctrl;										/* restore control flags */
	return (retval);
}
#endif /* !CISCO */

/*------------------------- ep_frm.c END OF FILE ---------------------------- */

