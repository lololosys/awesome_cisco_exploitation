/* $Id: cape.h,v 3.2.60.1 1996/07/19 22:45:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/bstun/cape.h,v $
 *------------------------------------------------------------------
 * Prototype definition and structures for CAPE FSM
 *
 * March 1995, Andrew R. Rothwell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cape.h,v $
 * Revision 3.2.60.1  1996/07/19  22:45:08  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:49:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CAPE_H
#define __CAPE_H

/*
		Bayfront Technologies
		Protocol/State Machine
			 Header File
			 Version 1.2
*/

/* -- sm_exec() return values -- */
#define smOK 0		/* return ok */
#define smActIndError 1	/* action table index error */
#define smSwRtnError 2	/* switch return value error */
#define smStateError 3	/* state out of bounds */
#define smEventError 4	/* event out of bounds */
#define smATypeError 5	/* unknown action type encountered */

/* ---- Interpreter constants ---- */

/* action types in the action tuple entry */
#define JmpOp 0
#define ActionOp 1
#define NullActOp 2
#define SwitchOp 3
#define ActionParamOp 5
#define SwitchParamOp 6

/* action array TNsi values (max # of states = 520) */
#define ContExec 521      /* continue exec of action array */
#define NoStateChg 520    /* no state change */

/* action array entry */
typedef struct AAEntry {
	int	(*pTAct)(void*);
	int	TType;
	int	TNsi;
} AAEntry;

/* switch statement table */
typedef struct SWTBL {
		int      SwInd; /* index into the SwLblVect structure */
		int      SwMax;	/* max return values */
} SWTBL;

/* data structures describing the state machine */
typedef struct SM {
	int          smStates;		/* max states in sm */
	int          smEvents;		/* max events in sm */
	int          smAASize;		/* action array size */
	const int    *pSwLblVect;	/* vectors into the action array */
	const struct SWTBL *pSwTbl;	/* contains info on switch statements */
	const int    **pStateEvent; /* state/event -> action array vector */
	const struct AAEntry *pActArray;  /* action array */
} SM;

/*---- Exported prototypes. */

int
sm_exec (SM* psm,
         int* curstate,
         int event,
         void* pdata);

#endif  /* __CAPE_H. */
/*---- End header file: cape.h. */
