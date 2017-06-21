/* $Id: dlcsw.h,v 3.2 1995/11/17 08:55:57 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/cls/dlcsw.h,v $
 *------------------------------------------------------------------
 * DLU Protocol Converter Definitions
 * 
 * 01-September-1994  Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlcsw.h,v $
 * Revision 3.2  1995/11/17  08:55:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:12:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/15  17:08:56  sberl
 * CSCdi40292:  FRAS backup needs to retry
 * Implement retry logic for fras backup command. We will not try to
 * activate the primary link several times before switching over to the
 * backup.
 *
 * Revision 2.1  1995/06/07  20:20:35  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define SDLC_FR

#define PCDLU_PRIMITIVE   		1
#define PCDLU_RAW 			2

#define CONN_PRIMARY   			1
#define CONN_SECONDARY 			2


/* 	LS_FSM state						*/
#define LS_RESET			1
#define LS_RQOPNSTNSENT			2
#define LS_EXCHGXID			3
#define LS_CONNRQSENT			4
#define LS_SIGSTNWAIT			5
#define LS_CONNRSPWAIT			6
#define LS_CONNRSPSENT			7
#define LS_CONTACTED			8
#define LS_DISCWAIT			9
#define NUM_LS_STATES			10

/* 	LS_FSM input						*/
#define START_LS   			0
#define RQ_OPNSTN_CNF   		1
#define CONNECT_CNF   			2
#define CONNECT_IND   			3
#define CONNSTN_IND   			4
#define CONNED_IND   			5
#define ID_STN_IND   			6
#define ID_IND   			7
#define XID_COMP   			8
#define XID_FAILED			9
#define DATA_IND			10
#define FLOW_IND   			11
#define SIG_STN   			12
#define SND_CONN_RSP   			13
#define SND_DISC   			14
#define DISC_CNF   			15
#define DISC_IND   			16
#define CLOSE_STN_CNF  			17
#define NUM_LS_INPUTS  			18

/* 	CONNECTOR_FSM state					*/
#define C_IDLE   			1
#define C_EXCHG_XID   			2
#define C_PRI_ACTIVE_PENDING  		3
#define C_SEC_ACTIVE_PENDING  		4
#define C_CONN_ACTIVE   		5
#define C_PRI_BUSY   			6	
#define C_SEC_BUSY   			7
#define NUM_CONN_STATES   		8

/* 	CONNECTOR_FSM input					*/

#define PRI_CONTACTING   		0
#define SEC_CONTACTING   		1
#define PRI_FLOW_SET   			2
#define PRI_FLOW_UNSET   		3
#define SEC_FLOW_SET   			4
#define SEC_FLOW_UNSET   		5
#define NUM_CONN_INPUTS   		6

/* debug fras state defines */
#define MAX_FRAS_ADDRSTR               128
#define MAX_FRAS_VIEWSTR               32
#define BNNFR                           0
#define BNNLAN                          1
#define BANFR                           2
#define BANLAN                          3
#define BANUN                           4

#define dlcSwErrDebug       	if (dlcsw_e_debug) buginf
#define dlcSwStateDebug       	if (dlcsw_state_debug) buginf
#define dlcSwMsgDebug       	if (dlcsw_mess_debug) buginf

extern boolean dlcsw_e_debug; 
extern boolean dlcsw_state_debug; 
extern boolean dlcsw_mess_debug;

extern char *ls2aTable[];

extern void pcdlu_command (parseinfo *);
extern int pcDluReadPut(TCLSIQueue*, TCLSIMsg*);
extern void pcdlu_main(void);
extern TDLUCep* DLUGetCep(TCLSIMsg* );
extern void DLUFsData(TDLUCep*, TCLSIMsg*);
extern boolean pcdluBlock(void);
extern TDLUCep* DLUFindNewCep(TCLSIMsg*);
extern void DLUProcessXid(TDLUCep*);
extern ushort conn_fsm(TDLUCvt*,  int);
extern ushort pcls_fsm(TDLUCep*, TCLSIMsg*, int);
extern TDLUBackup* IsAnyBackup(void);
extern boolean DLUBackupEnable(TDLUCep *);
extern uchar GetFrasMaxRetry(void);

extern ushort pcls_action_a(TDLUCep *);
extern ushort pcls_action_b(TDLUCep *);
extern ushort pcls_action_c(TDLUCep *);
extern ushort pcls_action_d(TDLUCep *);
extern ushort pcls_action_e(TDLUCep *);
extern ushort pcls_action_f(TDLUCep *);
extern ushort pcls_action_g(TDLUCep *);
extern ushort pcls_action_h(TDLUCep *);
extern ushort pcls_action_i(TDLUCep *);
extern ushort pcls_action_j(TDLUCep *);
extern ushort pcls_action_k(TDLUCep *);
extern ushort pcls_action_l(TDLUCep *);
extern ushort pcls_action_m(TDLUCep *);
extern ushort pcls_action_n(TDLUCep *);
extern ushort pcls_action_o(TDLUCep *);

extern ushort conn_action_a(TDLUCvt *);
extern ushort conn_action_b(TDLUCvt *);
extern ushort conn_action_c(TDLUCvt *);
extern ushort conn_action_d(TDLUCvt *);
extern ushort conn_action_e(TDLUCvt *);
extern ushort conn_action_f(TDLUCvt *);
extern ushort conn_action_g(TDLUCvt *);

extern ls_fsm_decode ( TDLUCep*, int, int);
extern conn_fsm_decode (TDLUCvt*, int, int);
boolean isDlciDown(TDLUCep *aCep, ushort fDlci);

char const *const frasBanBnnDebug (TDLUCep *aCep);




