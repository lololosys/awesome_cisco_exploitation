/* $Id: ep_cust.c,v 1.1.68.2 1996/06/07 00:27:56 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/sdu/task/epcustod/ep_cust.c,v $
 *------------------------------------------------------------------
 * Cylink Source code: ep_cust.c
 *
 * August 1995, Brian Weis
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ep_cust.c,v $
 * Revision 1.1.68.2  1996/06/07  00:27:56  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Cleanup to let compile with -Wall.
 *
 * Revision 1.1.68.1  1996/04/19  14:58:02  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.4  1996/04/16  21:54:26  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 1.1.2.3  1995/12/20  23:32:10  bew
 * Branch: IosSec_branch
 * Context is now freed after a sign operation. Before the callback is
 * called a pointer to the pak is taken so it can be used after the
 * callback returns.
 *
 * Revision 1.1.2.2  1995/12/12  23:12:34  bew
 * Branch: IosSec_branch
 * Better formatting of debug messages.
 *
 * Revision 1.1.2.1  1995/11/28  01:50:05  bew
 * Branch: IosSec_branch
 * Port of newest Cylink SDU code, plus CS cleanup. The SDU code
 * is moved to its original Cylink tree structure.
 *
 * Revision 1.1  1995/11/22  23:22:29  bew
 * Placeholders for new SDU file locations.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
* $Header: /SecureLAN/SDU/Task/epCustodian/EP_CUST.C 13    8/18/95 12:59p Peacock 
*
* DESCRIPTION:	Contains Entity Protection Custodian Functions and Task
*
* NOTES:	   	The EP frame processing routines, EP init function, and MIB
*				parameters are defined in the $/SecureLAN/SDU/ISR/EP project.
*
* Copyright(c)  Cylink Corporation 1994.  All rights reserved.
*******************************************************************************
* $History: EP_CUST.C 
* 
* *****************  Version 13  *****************
* User: Peacock      Date: 8/18/95    Time: 12:59p
* Updated in $/SecureLAN/SDU/Task/epCustodian
* 
* *****************  Version 12  *****************
* User: Peacock      Date: 8/10/95    Time: 11:46a
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Removed yields, now using global variable for time (updated in event
* manager).
* 
* *****************  Version 11  *****************
* User: Peacock      Date: 7/24/95    Time: 12:38p
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Fixed compiler warnings
* 
* *****************  Version 10  *****************
* User: Croyle       Date: 6/11/95    Time: 10:56a
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Added GetCCCstatus call in EP custodial loop.
* 
* *****************  Version 9  *****************
* User: Sduhost      Date: 5/02/95    Time: 11:44a
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Added assertions to custodian to guard against list corruption.
* 
* *****************  Version 8  *****************
* User: Sduhost      Date: 4/30/95    Time: 8:23a
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Fixed bug with initial creation of mulitple COT entries.
* 
* *****************  Version 7  *****************
* User: Sduhost      Date: 3/29/95    Time: 3:48p
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Changed from RTXC clock to Real Time Clock on Crypto Card.
* 
* *****************  Version 6  *****************
* User: Sduhost      Date: 3/28/95    Time: 3:34p
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Fixed Enable/Disable interrupt macros (had CLI and STI reversed)
* 
* *****************  Version 5  *****************
* User: Croyle       Date: 2/21/95    Time: 6:39p
* Updated in $/SecureLAN/SDU/Task/epCustodian
* Separated Startup Parameters used by EP Custodian to EPC specific
* variables:
* 
* ep_frm::epPEnodes->ep_cust::pet_nodes,  
* ep_frm::epCOTnodes->ep_cust::cot_nodes, 
* ep_frm::epCOThash->ep_cust::cothash_size
* 
* *****************  Version 4  *****************
* User: Croyle       Date: 1/03/95    Time: 1:49p
* Updated in $/SecureLAN/SDU/Task/EPc
* Modified naming convention for all Croyle files.
* 
* *****************  Version 3  *****************
* User: Croyle       Date: 11/03/94   Time: 2:28p
* Updated in $/SecureLAN/SDU/Task/EPc
* 
* *****************  Version 2  *****************
* User: Croyle       Date: 11/02/94   Time: 9:18a
* Updated in $/SecureLAN/SDU/Task/EPc
* Updated with new MIB variable epc_period
* 
* *****************  Version 1  *****************
* User: Croyle       Date: 10/13/94   Time: 10:26a
* Created in $/SecureLAN/SDU/Task/EPc
* Entity Protection Custodian.  Contains Task and Support Routines for
* performing PET node recycling, COT free list optimization, and Key
* aging functions at a specified interval (typically every second).
*******************************************************************************/

#define EP_CUST_C

/*****************************************************************************
								 INCLUDE FILES
*****************************************************************************/

#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/cy_api.h"
#include "../crypto/sdu/include/csdata.h"
#include "../crypto/sdu/include/ep_frm.h"
#include "../crypto/sdu/include/ep_debug.h"
#include "../crypto/sdu/include/iomgr.h"
#ifdef CISCO
#include "../crypto/sdu/include/debug.h"
#endif

/* RTXC Include Files */

#ifndef CISCO
#ifndef  NO_RTXC
#include "../crypto/sdu/include/rtxcapi.h"
#include "../crypto/sdu/include/rtxcinfo.h"
#endif
#endif /* !CISCO */

/******************************************************************************
								FILE MACROS
******************************************************************************/
#ifdef CISCO
#define DEFN_INTERRUPT  leveltype level;
#define ENBL_INTERRUPT  reset_interrupt_level(level);	
#define DSBL_INTERRUPT  level = raise_interrupt_level(NETS_DISABLE);	
#else /* !CISCO */
#define ENBL_INTERRUPT		asm sti			/* enable  interrupt processing */
#define DSBL_INTERRUPT		asm cli			/* disable interrupt processing */
#endif

/******************************************************************************
								PUBLIC GLOBALS
******************************************************************************/

ushort	epcPeriod = 6000;					/* run every 1000ms (second) */
ushort	epcKeyCtr = 8000;					/* number of keys in crypto card */
ushort	epcKeyMin = 1000;					/* Minimum number of keys */

/* EP configuration values copied to these variables during INIT */

ushort	pet_nodes = 0;						/* # PET nodes allocated */
ushort	cot_nodes = 0;						/* # COT nodes allocated */
ushort	cothash_size = 0;					/* size of cot hash table */

/******************************************************************************
								STATIC GLOBALS
******************************************************************************/

ushort	epc_exec_ctr=0;						/* Debug counter (ep custod running) */

/******************************************************************************
						PUBLIC DECLARATIONS (prototypes)
******************************************************************************/

void epc_task ( void );						/* EPC task (periodically do EP hkp) */

/******************************************************************************
						STATIC DECLARATIONS (prototypes)
******************************************************************************/

#ifdef CISCO
/*
 * gcc doesn't like the order
 */
static void recycle_pet ( void );			/* recycle deleted PET nodes */
static void arrange_cot ( void );			/* re-order COT free list */
static void free_socket ( void );           /* free socket defs not in use */
static void chk_key_age ( void );			/* check age of crypto keys */
void crypto_glue_timeout_keys(CIBptr cib); /* should probably be defined in a .h file */
#else /* !CISCO */
static void free_socket ( void );			/* free socket defs not in use */
void static recycle_pet ( void );			/* recycle deleted PET nodes */
void static arrange_cot ( void );			/* re-order COT free list */
void static free_socket ( void );			/* free socket defs not in use */
void static chk_key_age ( void );			/* check age of crypto keys */
#endif /* !CISCO */

/******************************************************************************
						PUBLIC FUNCTION DEFINITIONS
******************************************************************************/

/******************************************************************************
* NAME:			void epc_task ( void )
*
* DESCRIPTION:	This is the RXTC Entity Protection Custodian Task.  It performs
*				each of the custodial duties i.e., it recycles deleted PET
*				entries, re-orders the COT free list, checks for old cryto keys.
* INPUTS:
*	PARAMETERS:	none
*	GLOBALS:	none
*
* OUTPUT:
*	PARAMETERS:	none
*	GLOBALS:	none
*	RETURN:		none 			Never returns (loops continuously)
*
* NOTES:		No other task actually posts to EPC_SEMA, it it is used 
*				since KS_Delay seems to have a problem at high frame rates.
*******************************************************************************/
#ifdef CISCO
void epc_task ( void )
{
		/* perform EP Custodial duties */

		recycle_pet();							/* recycle deleted PET nodes */
		arrange_cot();							/* re-order COT free list */
        free_socket();                          /* free socket defs not used */
        chk_key_age();                          /* check age of crypto keys */

}
#else /* !CISCO */
void epc_task ( void )
{
	ulong time, status;

	for (;;)		
	{   
		/* perform a CY Custodial duty (as a favor...) */

		epc_exec_ctr++;
		KS_waitt(EPC_SEMA,100);				  	/* delay until next interval */
		ReadRTClock(&time);   				  	/* read current time */
		GetCCCstatus(&status);				  	/* read Crypto Card status */
		/* perform EP Custodial duties */

		recycle_pet();							/* recycle deleted PET nodes */
		arrange_cot();							/* re-order COT free list */
		free_socket();							/* free socket defs not used */
		chk_key_age();							/* check age of crypto keys */
	}
}
#endif /* !CISCO */

/*******************************************************************************
* STATIC FUNCTION DEFINITIONS
*******************************************************************************/

/*******************************************************************************
* NAME:			void recycle_pet ( void )
*
* DESCRIPTION:	This function is called by the ep_init() function to allocate
*				and initialize the Protected Entity Table (PET) data structures.
* INPUTS:
*	PARAMETERS:	none
*	GLOBALS:	COTnode	cot					Array of page definitions
*				ushort	cot_nodes			Number of COT nodes defined
*				ushort	cothash_size		Size of each COT hash table
*
* OUTPUT:
*	PARAMETERS:	none
*	GLOBALS:	COTnode	*cot				( only if XCHG_KEYS status bit set)
*	RETURN:		void
*
* NOTES:		void
*******************************************************************************/

static void recycle_pet ( void )
{
	ushort	i,m;							/* temporary variables */
	short 	*f;								/* temp free ptr for faster code */
	COTpage	*c;								/* temp page ptr for faster code */
    DEFN_INTERRUPT;                         /* define a var for int flipping */

	while (pet_cycle)
	{
		DSBL_INTERRUPT;						/* disallow interrupt preemption */
		m = pet_cycle;         				/* load pet cycle ptr */
		pet_cycle = pet[m].free;			/* move ptr to next in cycle list */
		ENBL_INTERRUPT;						/* interrupt preemption okay now */
		c = &cot[pet[m].page];				/* load page index used by PET node */
		for (i=0; i<cothash_size; i++)
			c->hash[i] = 0;
		for (i=1; i<cot_nodes; i++)
		{
			c->node[i].hash.list = 0;		/* not used in any hash list */
			c->node[i].free.prev = i-1;
			c->node[i].free.next = i+1;
			if (c->node[i].cib.status & BAD_CONN)
				sdu_conn_built--;
			c->node[i].cib.status = BAD_CONN;
		}
		c->node[i-1].free.next = 0;
		c->used = i-1;
		c->free = 1;
		c->nnc  = NNC_AUTO;					/* use generic "auto" NNC */
		f = &pet[m].free;
		DSBL_INTERRUPT;
		*f = pet_free;						/* add recycled node to PET free list */
		pet_free = m;
		ENBL_INTERRUPT;
	}
}

/*******************************************************************************
* NAME:			void arrange_cot ( void )
*
* DESCRIPTION:	This function re-arranges the free list of COT nodes for each
*				page currently in use by a PET node. This helps to maintain
*				a free list order based on usage.
* INPUTS:
*	PARAMETERS:	void
*	GLOBALS:	ushort	epPEnodes			Number of PET nodes defined
*				ushort	cot_nodes			Number of COT nodes defined
*				PETnode	pet[]				Array of PET node definitions
*				COTpage	cot[]				Array of page definitions
*
* OUTPUT:
*	PARAMETERS:	none
*	GLOBALS:	COTpage	cot[]				( see NOTES below )
*	RETURN:		void
*
* NOTES:		The COT page status, used_f, used and free list indeces may be
*				modified by this routine.
*******************************************************************************/

static void arrange_cot ( void )
{
	ushort	i,j;				/* pet and cot node indeces respectively */
	ushort	m,n;				/* temporary node index variables */
	short	c;					/* keep count of # COT entries traversed */
	ushort	p;					/* page index */
	ushort	u;					/* used and free list index (traverse backwards) */
	ushort	*s;					/* temporary status ptr for faster code */
    DEFN_INTERRUPT;                         /* define a var for int flipping */
 
	for (i=1; i<pet_nodes; i++)
		if ((p = pet[i].page) != 0)
		{
			j = u = cot[p].used;				/* get last node in free list */
			s = &cot[p].node[j].cib.status;
			c = cot_nodes;						/* load number of cot nodes */

			/* just clear USED BIT UNTIL first unused node */

			while ( (j) && (*s & USED_NODE) )
			{
				if (--c < 0) 					/* looped > max, list corrupted */
				{
#ifndef CISCO
					post2IOmanager(EP_MODULE,EPC_BADLIST_A,0);
					for (;;) KS_delay(SELFTASK,1000);
#else /* CISCO */
					DEBUG("Corrupted COT List.");
					return;
#endif /* CISCO */
				}
				if (!(*s & (LOCK_NODE|PEND_CONN)))
				{
					/* see if this can be an atomic instruction to clear single */
					/* bit --->>> then wouldn't have to disable interrupts. */

					DSBL_INTERRUPT;
					*s &= ~USED_NODE;
					ENBL_INTERRUPT;
				}
				j = cot[p].node[j].free.prev; 	/* get index of next node */
			}

			/* now relink used nodes to end of list (BUT be careful about ep_frame() */
			/* interrupting process - MUST disable interrupts when relinking */

			while (j)
			{
				if (--c < 0)	 				/* looped > max, list corrupted */
				{
#ifndef CISCO
					post2IOmanager(EP_MODULE,EPC_BADLIST_B,0);
					for (;;) KS_delay(SELFTASK,1000);
#else /* CISCO */
					DEBUG("Corrupted COT List.");
					return;
#endif /* CISCO */
				}
				m = cot[p].node[j].free.prev;	/* get previous node in free list */
				if ( *s & USED_NODE )
				{
					/* bypass node in list (node still linked to next) */

					n = cot[p].node[j].free.next;
					if (n)
						cot[p].node[n].free.prev = m;
					if (m)
						cot[p].node[m].free.next = n;

					/* link node to end of list */

					cot[p].node[j].free.prev = u;
					cot[p].node[j].free.next = 0;
					cot[p].node[u].free.next = j;
					u = j;

					/* clear "used" status bit flag */

					if ((*s & (LOCK_NODE|PEND_CONN))==0)
					{
						/* see if this can be an atomic instruction to clear single */
						/* bit --->>> then wouldn't have to disable interrupts. */

						DSBL_INTERRUPT;
						*s &= ~USED_NODE;
						ENBL_INTERRUPT;
					}
				}
				j = m;  		/* <<< BUG FIX: used to use cot[].node[].prev after mod */
			}
			/* reset free list pointer for this page to the beginning of the list */

			cot[p].used = u;
		}
}

/*******************************************************************************
* NAME:			void free_socket ( void )
*
* DESCRIPTION:	This function scans the COT pages looking for COT nodes which
*				have a socket definition allocated, but are not using it. This
*				is defined as a node with a non-NULL socket pointer, a zero
*				socket mask and a zero PEND_CONN bit flag in the CIB status.
* INPUTS:
*	PARAMETERS:	void
*	GLOBALS:	ushort	pet_nodes			Number of PET nodes defined
*				ushort	cot_nodes			Number of COT nodes defined
*				PETnode	pet[]				Array of PET node definitions
*				COTpage	cot[]				Array of page definitions
*
* OUTPUT:
*	PARAMETERS:	none
*	GLOBALS:	COTpage	cot[]				( see NOTES below )
*	RETURN:		void
*
* NOTES:		The socket pointer for each COT node which is not using it's
*				socket definition will be set to NULL (and the def placed back
*				into the socket definition free list).
*******************************************************************************/

static void free_socket ( void )
{
	ushort	i,j;				/* pet and cot node indeces respectively */
	ushort	p;					/* page index */
	SokPtr	sok;				/* socket definition pointer */
    DEFN_INTERRUPT;             /* define a var for int flipping */
	
	for (i=1; i<pet_nodes; i++)
		if ((p = pet[i].page) != 0)
			for (j=1; j<cot_nodes; j++)
				if (!(cot[p].node[j].cib.status & PEND_CONN))
				{
					sok = cot[p].node[j].cib.socket;
					if (sok && !sok->mask)
					{
						cot[p].node[j].cib.socket = NULL;
						DSBL_INTERRUPT;					/* return socket def */
						sok->free = sok_free;			/* ...to free list */
						sok_free  = sok;
						ENBL_INTERRUPT;
					}
				}
}

/******************************************************************************
* NAME:			void chk_key_age ( void )
*
* DESCRIPTION:	This function checks the age of the cryptographic keys.  It
*				traverses all nodes in every COT page searching for old and
*				expired cryptographic keys.  If a COT node is found to have had
*				a set of crypto keys for too long, the XCHG_KEYS bit flag in
*				the CIB status word is set. Thus, when a frame is received
*				which indexes the expired COT node, the ep_frame() interrupt
*				function will request a new connection from CS.
* INPUTS:
*	PARAMETERS:	void
*	GLOBALS:	ushort	pet_nodes				Number of PE nodes
*				ushort	cot_nodes				Number of COT nodes
*				COTpage	cot[]					Use node CIB status, crypto info
*
* OUTPUT:
*	PARAMETERS:	void
*	GLOBALS:	COTpage	cot[]					( set CIB status bit XCHG_KEYS )
*	RETURN:		void
*
* NOTES:		none
*******************************************************************************/

static void chk_key_age ( void )
{
	ushort	i,j,p;						/* processing indeces */
	ushort	t,time;						/* time computation variables */
	ushort  *s;							/* temp status ptr for faster code */
#ifdef CISCO
	long ccc_time;
    DEFN_INTERRUPT;                         /* define a var for int flipping */

	(void) GetRTClock(&ccc_time);
	time = (ushort) (ccc_time / 60L); 	/* convert to mins % 2^16 */
#else
	time = (ushort) (ccc_time / 60L); 	/* convert to mins % 2^16 */
#endif
	for (i=pet_nodes-1; i; i--)
		if ((p = pet[i].page) != 0)
			for (j=cot_nodes-1; j; j--)
				if (cot[p].node[j].cib.status & TIME_KEYS)
				{
					t = cot[p].node[j].cib.crypto.time;
					if (t > time)						  /* get #minutes keys used */
						t = ((ushort)0xffff - t) + time;  /* time has wrapped, normalize */
					else
						t = time - t;
					if (t > epMaxKeyAge)				  /* see if time has expired */
					{
						s = &cot[p].node[j].cib.status;
						DSBL_INTERRUPT;
						*s |= XCHG_KEYS;
#ifdef CISCO
						DEBUG("Timed out key found");
						DEBUG_WITH_ARG("    CIB time: %d", cot[p].node[j].cib.crypto.time);
						DEBUG_WITH_ARG("    Current time: %d", time);
						DEBUG_WITH_ARG("    Elapsed time: %d", t);
						crypto_glue_timeout_keys(&cot[p].node[j].cib);
#endif
						ENBL_INTERRUPT;
					}
				}
}


/*------------------------- epc.c END OF FILE ---------------------------- */
