/* $Id: aurp_fsm.h,v 3.2 1995/11/17 08:44:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/aurp_fsm.h,v $
 *------------------------------------------------------------------
 * Dummy file for Aurp_branch
 *
 * February 1994, Steven Lin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: aurp_fsm.h,v $
 * Revision 3.2  1995/11/17  08:44:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:10:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* state transition events are numbered from 1..AURP_FSM_NEVENTS */
#define AURP_FSM_INITIATE	1
#define AURP_FSM_TIMEOUT2	2
#define AURP_FSM_OPEN_RSPBAD	3
#define AURP_FSM_OPEN_RSPGOOD	4
#define AURP_FSM_GET_RI	5
#define AURP_FSM_TIMEOUTLHFT	6
#define AURP_FSM_RD	7
#define AURP_FSM_RI_RSP	8
#define AURP_FSM_RI_REQ	9
#define AURP_FSM_TIMEOUTUPDATE	10
#define AURP_FSM_TICKLE_ACK	11
#define AURP_FSM_OPENREQBAD	12
#define AURP_FSM_SEND_RD	13
#define AURP_FSM_RI_ACK_DONE	14
#define AURP_FSM_DS_START	15
#define AURP_FSM_DR_START	16
#define AURP_FSM_FSM_HALT	17
#define AURP_FSM_DISCONNECT	18
#define AURP_FSM_RESET		19
#define	AURP_FSM_NEVENTS    19

extern char *aurp_eventStr(atfsmEvent ee);
extern char const *aurp_showstatusStr(FSMFUNC);
extern char *aurp_stateStr(FSMFUNC func);
extern void aurp_fsm_init(at_fsm *fsm, char *uarg);

/* functions supplied by programmer to implement aurp fsm */
extern void aurp_fsm_DR_Unconnected (at_fsm *fsm);
extern void aurp_fsm_DR_Open_Rsp_Wait (at_fsm *fsm);
extern void aurp_fsm_DR_Connected (at_fsm *fsm);
extern void aurp_fsm_DR_RI_Rsp_Wait (at_fsm *fsm);
extern void aurp_fsm_DR_Tickle_Ack_Wait (at_fsm *fsm);
extern void aurp_fsm_DS_Unconnected (at_fsm *fsm);
extern void aurp_fsm_DS_Connected (at_fsm *fsm);
extern void aurp_fsm_DS_RI_Ack_Wait1 (at_fsm *fsm);
extern void aurp_fsm_DS_RI_Ack_Wait2 (at_fsm *fsm);
extern void aurp_fsm_DS_Send_RD (at_fsm *fsm);
extern void aurp_fsm_DS_RI_Ack_Wait4 (at_fsm *fsm);
extern void aurp_fsm_DS_RI_Ack_Wait3 (at_fsm *fsm);
extern void aurp_fsm_initial (at_fsm *fsm);
extern void aurp_fsm_halt (at_fsm *fsm);
