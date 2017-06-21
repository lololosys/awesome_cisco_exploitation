/* $Id: sm_exec.c,v 3.2 1995/11/17 08:49:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/sm_exec.c,v $
 *------------------------------------------------------------------
 * General purpose Finite State Machine.
 *
 * February 1995, Andrew R. Rothwell.
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sm_exec.c,v $
 * Revision 3.2  1995/11/17  08:49:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "cape.h"


#define DEMAND(x, y) if (!(x)) return(y);


/*----------------------------------------------------------------------------*/
/* sm_exec: State machine execution unit. */
/*----------------------------------------------------------------------------*/

int
sm_exec (SM* psm,
         int* curstate,
         int event,
         void* pdata)

{
    int ActInd, RtnVal;

    /* get action index from state & event */
    DEMAND(*curstate < psm->smStates, smStateError);
    DEMAND(event < psm->smEvents, smEventError);

    ActInd = (*(*(psm->pStateEvent+*curstate)+event));

    do { /* loop until a new/no state termination */
	switch (psm->pActArray[ActInd].TType) {
	    case ActionOp:
	    case ActionParamOp:
		DEMAND(ActInd < psm->smAASize, smActIndError);

		/* execute action */
		RtnVal = (*psm->pActArray[ActInd].pTAct)(pdata);

		/* go to next tuple if new state not assigned */
		if (psm->pActArray[ActInd].TNsi == ContExec)
			ActInd++;
		else
			if (psm->pActArray[ActInd].TNsi == NoStateChg)
			return(smOK);
			else {
			*curstate = psm->pActArray[ActInd].TNsi;
			return(smOK);
		    }
		break;

	case SwitchOp:
	case SwitchParamOp:
		DEMAND(ActInd < psm->smAASize, smActIndError);
		RtnVal = (*psm->pActArray[ActInd].pTAct)(pdata);

	    /* check RtnVal for range correctness */
	    DEMAND((RtnVal >= 0 &&
		RtnVal <= psm->pSwTbl[psm->pActArray[ActInd].TNsi].SwMax-1),
		smSwRtnError);

	    ActInd = psm->pSwLblVect[psm->pSwTbl[
			 psm->pActArray[ActInd].TNsi].SwInd + RtnVal];
	    break;

	case JmpOp:
		ActInd = psm->pActArray[ActInd].TNsi;
	    if (ActInd == 0) return(smOK);
	    break;

	case NullActOp:
		if (psm->pActArray[ActInd].TNsi == NoStateChg)
		return(smOK);
	    else {
		*curstate = psm->pActArray[ActInd].TNsi;
		return(smOK);
	    }

	default:
		return(smATypeError);
	} /* switch */
    } while (1);
} /* sm_exec */

