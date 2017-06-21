/* $Id: dlcsw_fsm.c,v 3.5.26.2 1996/07/19 22:45:19 snyder Exp $
 * $Source: /release/112/cvs/Xsys/cls/dlcsw_fsm.c,v $
 *-----------------------------------------------------------------
 * CLS Protocol Converter Finite State Machine
 *
 * September 1994, Meng Lyu
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dlcsw_fsm.c,v $
 * Revision 3.5.26.2  1996/07/19  22:45:19  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.5.26.1  1996/05/17  10:42:49  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.14.1  1996/04/26  07:33:13  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5  1996/01/18  02:42:41  utam
 * CSCdi46086:  fras does not handle non-activation xid
 * Change from no-action to normal-xid-processing for fsm state Contacted
 *
 * Revision 3.4  1995/12/08  17:22:20  sberl
 * CSCdi44998:  Removing FRAS/BAN from subinterface crashes router
 * 1) Added checking for fSapState before calling functions to activate
 * and deactivate the sap. Don't want to deactivate a sap that is not
 * active, nor activate one that is already active. Defined an enum to
 * represent the sap state instead of useing 0 and 1.
 *
 * 2) Fix loop in DLURemoveBanCep so that when list is modified from
 * within the loop, we start again back at the beginning of the list.
 *
 * 3) Validate cep in close_stn.cnf to handle case where DLU did not wait
 * for confirm before deleting the cep control block. If we get a confirm
 * for a cep that doesn't exist any more, just drop the confirm message.
 *
 * 4) Generic cleanup and commenting.
 *
 * Revision 3.3  1995/11/17  08:56:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:14:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:12:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:53:27  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/24  23:48:09  vrawat
 * CSCdi42311:  FRAS to SDLC does not recover properly
 *
 * Revision 2.4  1995/09/15  17:08:58  sberl
 * CSCdi40292:  FRAS backup needs to retry
 * Implement retry logic for fras backup command. We will not try to
 * activate the primary link several times before switching over to the
 * backup.
 *
 * Revision 2.3  1995/08/07  05:30:06  richl
 * CSCdi38315:  makefile rework
 *              add rif_util.c to libutil.a
 *              move common code to libibm_util.a
 *
 * Revision 2.2  1995/07/20  21:13:25  kchiu
 * CSCdi37258:  FRAS Dial-Backup for multiple stations failed
 *
 * Revision 2.1  1995/06/07  20:20:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#include "master.h"
#include "address.h"
#include "interface_generic.h"
#include "ieee.h"
#include "../if/rif.h"
#include "../util/fsm.h"
#include "../ibm/sna_util.h"
#include "clsi_msg.h"
#include "clsi_queue.h"
#include "dlu.h"
#include "dlcsw.h"
#include "../ibm/libibm_externs.h"

/*
 * A few definitions to cast the action routines appropriately so that
 * the table stays nice and pretty:
 */

#define PCLS_ACTION_a FSM_ACTION_ROUTINE(pcls_action_a)
#define PCLS_ACTION_b FSM_ACTION_ROUTINE(pcls_action_b)
#define PCLS_ACTION_c FSM_ACTION_ROUTINE(pcls_action_c)
#define PCLS_ACTION_d FSM_ACTION_ROUTINE(pcls_action_d)
#define PCLS_ACTION_e FSM_ACTION_ROUTINE(pcls_action_e)
#define PCLS_ACTION_f FSM_ACTION_ROUTINE(pcls_action_f)
#define PCLS_ACTION_g FSM_ACTION_ROUTINE(pcls_action_g)
#define PCLS_ACTION_h FSM_ACTION_ROUTINE(pcls_action_h)
#define PCLS_ACTION_i FSM_ACTION_ROUTINE(pcls_action_i)
#define PCLS_ACTION_j FSM_ACTION_ROUTINE(pcls_action_j)
#define PCLS_ACTION_k FSM_ACTION_ROUTINE(pcls_action_k)
#define PCLS_ACTION_l FSM_ACTION_ROUTINE(pcls_action_l)
#define PCLS_ACTION_m FSM_ACTION_ROUTINE(pcls_action_m)
#define PCLS_ACTION_n FSM_ACTION_ROUTINE(pcls_action_n)
#define PCLS_ACTION_o FSM_ACTION_ROUTINE(pcls_action_o)

#define CONN_ACTION_a FSM_ACTION_ROUTINE(conn_action_a)
#define CONN_ACTION_b FSM_ACTION_ROUTINE(conn_action_b)
#define CONN_ACTION_c FSM_ACTION_ROUTINE(conn_action_c)
#define CONN_ACTION_d FSM_ACTION_ROUTINE(conn_action_d)
#define CONN_ACTION_e FSM_ACTION_ROUTINE(conn_action_e)
#define CONN_ACTION_f FSM_ACTION_ROUTINE(conn_action_f)
#define CONN_ACTION_g FSM_ACTION_ROUTINE(conn_action_g)

/*
**  PCLS_FSM State = LS_RESET
*/
const static struct fsm_state ls_reset[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {PCLS_ACTION_g,  FSM_NOARG,    LS_RQOPNSTNSENT},        /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {PCLS_ACTION_a,  FSM_NOARG,    LS_RQOPNSTNSENT},        /* 06	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 16	*/
  {PCLS_ACTION_o,  FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};


/*
**  PCLS_FSM State = LS_RQOPNSTNSENT
*/
const static struct fsm_state ls_RqOpnStnSent[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {PCLS_ACTION_b,  FSM_NOARG,    LS_EXCHGXID},            /* 01	  CONNECT_CONFIRM */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {PCLS_ACTION_m,  FSM_NOARG,    LS_RESET},               /* 06	*/
  {PCLS_ACTION_m,  FSM_NOARG,    LS_RESET},               /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {PCLS_ACTION_l,  FSM_NOARG,    LS_RESET},          	  /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {PCLS_ACTION_m,  FSM_NOARG,    LS_RESET},          	  /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = LS_EXCHGXID
*/
const static struct fsm_state ls_ExchgXid[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {PCLS_ACTION_n,  FSM_NOARG,    LS_CONNRSPWAIT},         /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {PCLS_ACTION_b,  FSM_NOARG,    FSM_NOCHANGE},           /* 06	ID_STN_INDICATION        */
  {PCLS_ACTION_b,  FSM_NOARG,    FSM_NOCHANGE},           /* 07	ID_CONFIRM, ID_INDICATION*/
  {PCLS_ACTION_d,  FSM_NOARG,    LS_CONNRQSENT},          /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {PCLS_ACTION_l,       FSM_NOARG,    LS_RESET},          /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {PCLS_ACTION_m,       FSM_NOARG,    LS_RESET},          /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = LS_CONNRQSENT
*/
const static struct fsm_state ls_ConnRqSent[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {PCLS_ACTION_c,    FSM_NOARG,    LS_SIGSTNWAIT},          /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 06	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {PCLS_ACTION_l,       FSM_NOARG,    LS_RESET},          /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {PCLS_ACTION_m,       FSM_NOARG,    LS_RESET},          /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = LS_SIGSTNWAIT
*/
const static struct fsm_state ls_SigStnWait[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 06	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {PCLS_ACTION_h,  FSM_NOARG,    LS_CONTACTED},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {PCLS_ACTION_l,       FSM_NOARG,    LS_RESET},          /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {PCLS_ACTION_m,       FSM_NOARG,    LS_RESET},          /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = LS_CONNRSPWAIT
*/
const static struct fsm_state ls_ConnRspWait[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 06	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {PCLS_ACTION_i,   FSM_NOARG,    LS_CONNRSPSENT},           /* 13	*/
  {PCLS_ACTION_l,       FSM_NOARG,    LS_RESET},          /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {PCLS_ACTION_m,       FSM_NOARG,    LS_RESET},          /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = LS_CONNRSPSENT
*/
const static struct fsm_state ls_ConnRspSent[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    LS_CONTACTED},           /* 05	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 06	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {PCLS_ACTION_l,       FSM_NOARG,    LS_RESET},          /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {PCLS_ACTION_m,       FSM_NOARG,    LS_RESET},          /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = LS_CONTACTED
*/
const static struct fsm_state ls_Contacted[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 06	*/
  {PCLS_ACTION_b,  FSM_NOARG,    FSM_NOCHANGE},           /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {PCLS_ACTION_e,   FSM_NOARG,    FSM_NOCHANGE},          /* 10	*/
  {PCLS_ACTION_f,   FSM_NOARG,    FSM_NOCHANGE},          /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {PCLS_ACTION_k,       FSM_NOARG, LS_DISCWAIT},          /* 14	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 15	*/
  {PCLS_ACTION_j,       FSM_NOARG,    LS_RESET},          /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = LS_DISCWAIT
*/
const static struct fsm_state ls_DiscWait[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 06	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 07	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 08	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 09	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 10	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 11	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 12	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 13	*/
  {PCLS_ACTION_l,       FSM_NOARG,    LS_RESET},          /* 14	*/
  {PCLS_ACTION_l,   FSM_NOARG,    LS_RESET},           /* 15	*/
  {PCLS_ACTION_m,       FSM_NOARG,    LS_RESET},          /* 16	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 17	*/
/* ------------------------------------------------------------------------------------- */
};


#define LS_FSM_DECODE FSM_DECODE_ROUTINE(ls_fsm_decode)
/*
**  Finite State Machine table for LS_FSM
*/

const static struct finite_state_machine pcls_fsm_table[] =
{
/*  State                       Decoder              State Value              */
 {   ls_reset,         		LS_FSM_DECODE}, 
 {   ls_RqOpnStnSent,         	LS_FSM_DECODE}, 
 {   ls_ExchgXid,         	LS_FSM_DECODE}, 
 {   ls_ConnRqSent,         	LS_FSM_DECODE}, 
 {   ls_SigStnWait,         	LS_FSM_DECODE}, 
 {   ls_ConnRspWait,         	LS_FSM_DECODE}, 
 {   ls_ConnRspSent,         	LS_FSM_DECODE}, 
 {   ls_Contacted,         	LS_FSM_DECODE}, 
 {   ls_DiscWait,         	LS_FSM_DECODE}, 
};

/******************************************************************************** 
			CONN_FSM
 ********************************************************************************/

/*
**  PCLS_FSM State = C_IDLE
*/
const static struct fsm_state c_idle[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {CONN_ACTION_a,  FSM_NOARG,    C_PRI_ACTIVE_PENDING},   /* 00	*/
  {CONN_ACTION_g,   FSM_NOARG,    C_SEC_ACTIVE_PENDING},   /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = C_EXCHG_XID
*/
const static struct fsm_state c_exchg_xid[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = C_PRI_ACTIVE_PENDING
*/
const static struct fsm_state c_pri_active_pending[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {CONN_ACTION_b,  FSM_NOARG,    C_CONN_ACTIVE},          /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = C_SEC_ACTIVE_PENDING
*/
const static struct fsm_state c_sec_active_pending[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {CONN_ACTION_b,  FSM_NOARG,    C_CONN_ACTIVE},          /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = C_CONN_ACTIVE
*/
const static struct fsm_state c_conn_active[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {CONN_ACTION_c,  FSM_NOARG,    C_PRI_BUSY},             /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {CONN_ACTION_d,  FSM_NOARG,    C_SEC_BUSY},             /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = C_PRI_BUSY
*/
const static struct fsm_state c_pri_busy[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {CONN_ACTION_e,  FSM_NOARG,    C_CONN_ACTIVE},          /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 05	*/
/* ------------------------------------------------------------------------------------- */
};

/*
**  PCLS_FSM State = C_SEC_BUSY
*/
const static struct fsm_state c_sec_busy[] =
{
/*  Action        Param Blk     Next State                 Index  Decoded Input          */
/* ------------------------------------------------------------------------------------- */
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 00	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 01	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 02	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 03	*/
  {FSM_NOACTION,   FSM_NOARG,    FSM_NOCHANGE},           /* 04	*/
  {CONN_ACTION_f,  FSM_NOARG,    C_CONN_ACTIVE},          /* 05	*/
/* ------------------------------------------------------------------------------------- */
};


#define CONN_FSM_DECODE FSM_DECODE_ROUTINE(conn_fsm_decode)

const static struct finite_state_machine conn_fsm_table[] =
{
/*  State                       Decoder              State Value              */
 {   c_idle,         		CONN_FSM_DECODE},
 {   c_exchg_xid,      		CONN_FSM_DECODE},
 {   c_pri_active_pending,      CONN_FSM_DECODE},
 {   c_sec_active_pending,      CONN_FSM_DECODE},
 {   c_conn_active,      	CONN_FSM_DECODE},
 {   c_pri_busy,      		CONN_FSM_DECODE},
 {   c_sec_busy,      		CONN_FSM_DECODE},
};

static char const *const BanBnnDebugTbl [] =
{
    "BNN FR ",
    "BNN LAN",
    "BAN FR ",
    "BAN LAN",
    "UNDEF  "
};


int
ls_fsm_decode ( TDLUCep *aCep, int major, int minor)
{
    int  	index = 0;

    switch (major)
    {
	case PCDLU_PRIMITIVE:
	    switch (minor)
	    {
		case CLS_REQ_OPNSTN | CLS_CONFIRM:
		    index = 1;		 
		    break;

		case CLS_CONNECT | CLS_CONFIRM:
		    index = 2;		 
		    break;

		case CLS_CONNECT | CLS_INDICATION:
		    index = 3;		 
		    break;

		case CLS_CONNECT_STN | CLS_INDICATION:
		    index = 4;		 
		    break;

		case CLS_CONNECTED | CLS_INDICATION:
		    index = 5;		 
		    break;

		case CLS_ID_STN | CLS_INDICATION:
		    index = 6;		 
		    break;

		case CLS_ID | CLS_CONFIRM:
		case CLS_ID | CLS_INDICATION:
		    index = 7;		 
		    break;

		case CLS_DATA | CLS_INDICATION:
		    index = 10;		 
		    break;

		case CLS_FLOW | CLS_INDICATION:
		    index = 11;		 
		    break;

		case CLS_DISCONNECT | CLS_CONFIRM:
		    index = 15;		 
		    break;

		case CLS_DISCONNECT | CLS_INDICATION:
		    index = 16;		 
		    break;

		case CLS_CLOSE_STN | CLS_CONFIRM:
		    index = 17;		 
		    break;

                default:
                    index = FSM_FAULT;
            }
            break;

        case PCDLU_RAW:
	    index = minor;
	    break;

	default:
	    index = FSM_FAULT;
	    break;
    }
    return (index);
}

int
conn_fsm_decode (TDLUCvt *aConn, int major, int minor)
{

    return (major);
}

ushort pcls_fsm(TDLUCep *theCep, TCLSIMsg* theMsg, int input)
{
    int fsm_rc;
    hword major, minor;
    int state = 0;
    int oldstate = 0;
    int dlci = 0;
    char frasStateView[MAX_FRAS_VIEWSTR];
    char frasAddrstr[MAX_FRAS_ADDRSTR];
    uchar localMacAddr[IEEEBYTES];

    if (theMsg) {
	major = PCDLU_PRIMITIVE;
	minor = CLSIMsgGetPrimitiveCode(theMsg);
     } else {
	major = PCDLU_RAW;
	minor = input;
    }
    theCep->fMsg = theMsg;

    if (dlcsw_state_debug) {
        oldstate = (((theCep->fCepState < 0) ||
                        (theCep->fCepState >= NUM_LS_STATES)) ?
                        0 : theCep->fCepState);
        sprintf(frasStateView, "%s %s",
                (theCep->fSapPtr->fPortId.fTypeInfo.swidb)->short_namestring,
                frasBanBnnDebug(theCep));
        if (theCep->fFrasType == FRAS_BNN) {
            /* BNN */
            switch (theCep->fSapPtr->fDLCType) {
            case CLS_SDLC:
                sprintf(frasAddrstr, "SDLC PollAddr=%x",
                    theCep->fDstAddr.addr[0]);
                break;
            case CLS_FRAME_RELAY:
                dlci = (ushort) ((theCep->fDstAddr.addr[0] << 8) | theCep->fDstAddr.addr[1]);
                sprintf(frasAddrstr, "DLCI=%d, RSap=%02x, LSap=%02x",
                        dlci, theCep->fRSap, theCep->fLSap);
                break;
            case CLS_LLC:
                ieee_copy(theCep->fSapPtr->fPortPtr->fPortId.fTypeInfo.swidb->hwptr->hardware,
                          localMacAddr);
                sprintf(frasAddrstr, "LLC DestAddr=%e, SrcAddr=%e, RSap=%02x, lSap=%02x",
                        theCep->fDstAddr.addr,
                        localMacAddr,
                        theCep->fRSap, theCep->fLSap);
                break;
            default:
                break;
            }
        }
        else if ((theCep->fFrasType == FRAS_BAN) || (theCep->fCepPtr->fFrasType == FRAS_BAN)) {
            /* BAN */
            switch (theCep->fSapPtr->fDLCType) {
            case CLS_SDLC:
                sprintf(frasAddrstr, "SDLC DestAddr=%e, SrcAddr=%e, RSap=%02x, LSap=%02x",
                        theCep->fDstAddr.addr,
                        theCep->fSrcAddr.addr,
                        theCep->fRSap, theCep->fLSap);
                break;
            case CLS_FRAME_RELAY:
                sprintf(frasAddrstr, "FR  DestAddr=%e, SrcAddr=%e, RSap=%02x, LSap=%02x",
                        theCep->fDstAddr.addr,
                        theCep->fSrcAddr.addr,
                        theCep->fRSap, theCep->fLSap);
                break;
            case CLS_RSRB:
                sprintf(frasAddrstr, "LLC DestAddr=%e, SrcAddr=%e, RSap=%02x, LSap=%02x",
                        theCep->fDstAddr.addr,
                        theCep->fSrcAddr.addr,
                        theCep->fRSap, theCep->fLSap);
                break;
            default:
                break;
            }
        }
    }

    fsm_rc = (int) fsm_execute("FRAS", pcls_fsm_table,
		       ((int *) &(theCep->fCepState)), NUM_LS_STATES,
		       theCep, major, minor, FALSE);

    if (dlcsw_state_debug) {
	if (minor == (CLS_CONFIRM | CLS_CLOSE_STN)) {
            state = 0;
        }
        else {
            state = (((theCep->fCepState < 0) ||
                    (theCep->fCepState >= NUM_LS_STATES)) ?
                    0 : theCep->fCepState);
        }
        buginf("\nFRAS: %s <%s>--<%s>, %s",
                frasStateView,
                ls2aTable[oldstate],
                (state) ? ls2aTable[state] : "closed",
                frasAddrstr);
    }
    
    return (fsm_rc);
}

ushort conn_fsm(TDLUCvt *theConn,  int input)
{
    int fsm_rc, oldstate;
    hword major, minor;

    major = input;
    minor = 0;
    oldstate = theConn->fCvtState;

    fsm_rc = (int) fsm_execute("IBM_CONVERTER", conn_fsm_table,
		       ((int *) &(theConn->fCvtState)), NUM_CONN_STATES,
			       theConn, major, minor, FALSE);
    return fsm_rc;
}

ushort pcls_action_a (TDLUCep *aCep)
{
    TCLSIMsg* theMsg = aCep->fMsg;
    IdStnInd_t* idStn;

    if (theMsg) {
      	idStn = (IdStnInd_t*) theMsg;
    /* oct 20,95 CSCdi42311, fPCepId has junk value here, don't use it, commented out */ 
    /*  aCep->fPCepId = GETLONG(&idStn->fCLSIHdr.fPCepId); */ 
        aCep->fConnectionInit = INBOUND_CONNECTION;
        aCep->fCepPtr->fConnectionInit = OUTBOUND_CONNECTION;
    }
    pcls_fsm(aCep->fCepPtr, NULL, START_LS);
    return 0;
}

ushort pcls_action_b (TDLUCep *aCep)
{
    TCLSIMsg* theMsg = aCep->fMsg;
    ReqOpnStnCnf_t* reqOpnCnf = NULL;
    word retCode;
    hword primitive;
    ushort aDlci=0;

    if (theMsg) {
    	primitive = CLSIMsgGetPrimitiveCode(theMsg);
        if (primitive == (CLS_REQ_OPNSTN | CLS_CONFIRM)) {

            retCode = CLSIMsgGetReturnCode(theMsg);
            if (retCode == CLS_OK) {
                dlcSwErrDebug("\n cls ok, for dlci %d,fras-type %d, dlctype %d",
                     aCep->fDlci, aCep->fFrasType,aCep->fSapPtr->fDLCType);
                if((aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) &&
                    isDlciDown(aCep->fCepPtr, aCep->fDlci) &&
                    (aCep->fFrasType == FRAS_BAN) &&
                    (aDlci = DLUGetBanDlci(aCep->fCepPtr)) ) {
                    dlcSwErrDebug("\ncls ok, but dlci %d down", aCep->fDlci);
                    aCep->fCepState = LS_RESET;
                    aCep->fCepPtr->fCepState = LS_RESET;
                    reqOpnCnf = (ReqOpnStnCnf_t*) theMsg;
                    aCep->fPCepId = reqOpnCnf->fCLSIHdr.fPCepId;
                    if (aCep->fPCepId) {
                        DLUSendCloseStnReq(aCep);
                    }
                    DLUBanCepEnable(aCep);  /* enable another cep pair */
                    aCep->fDlci = aDlci;
                    dlcSwErrDebug("pcls_action_b - updated dlci to %d",
                                   aCep->fDlci);
                    return 0;
                }
            }
            if (retCode != CLS_OK) {
                dlcSwErrDebug("\ncls not ok, for dlci %d ", aCep->fDlci);
		aCep->fCepState = LS_RESET;
		aCep->fCepPtr->fCepState = LS_RESET;
		if (aCep->fPCepId)
    	 	    DLUSendCloseStnReq(aCep);
                if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
		    if ((IsAnyBackup()) && (!aCep->fCepPtr->fCepOrigPtr)) {
                        if (aCep->fReqOpnStnMaxTry == GetFrasMaxRetry()) {
                            aCep->fReqOpnStnMaxTry = 0;
                            DLUBackupEnable(aCep);
                        } else {
                            aCep->fReqOpnStnMaxTry++;
                        }
                    }
                }
                return 0;
            } else {     /* if we get a REQ_OPNSTN.Cfm */
              reqOpnCnf = (ReqOpnStnCnf_t*) theMsg;
              aCep->fPCepId = reqOpnCnf->fCLSIHdr.fPCepId;
              if (aCep->fFrasType == FRAS_BAN) {
                if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
                    DLUUpdateDlciStatus(aCep->fCepPtr, DLU_DLCI_ACTIVE);
                } else
                  DLUBanCepEnable(aCep);  /* enable another cep pair */
              }
              /* open the station of another Cep */
              if (aCep->fConnectionInit == OUTBOUND_CONNECTION)
                DLUSendReqOpnStn(aCep->fCepPtr);
              if (aCep->fXidState == D_XID_SEND)
                DLUSendIDReq(aCep, NULL, 0);
              aCep->fXidState = D_XID_XCHG;
              return 0;
            }
        }
    }
    DLUProcessXid(aCep);
    return 0;
}

ushort pcls_action_c (TDLUCep *aCep)
{

    if (aCep->fRole != CONN_PRIMARY) {
	dlcSwErrDebug("\nFRAS: Link Role Negotiating Failed");
	return 0;
    }

    conn_fsm(aCep->fCvtPtr, PRI_CONTACTING);
    return 0;
}

ushort pcls_action_n (TDLUCep *aCep)
{

    aCep->fRole = CONN_SECONDARY;
    aCep->fCepPtr->fRole = CONN_PRIMARY;
    aCep->fCvtPtr->fPriCep = aCep->fCepPtr;
    aCep->fCvtPtr->fSecCep = aCep;

    conn_fsm(aCep->fCvtPtr, SEC_CONTACTING);
    return 0;
}

ushort pcls_action_d (TDLUCep *aCep)
{
    DLUSendConnectReq(aCep);
    return 0;
}

ushort pcls_action_f (TDLUCep *aCep)
{
#ifdef XXX
    int input;
    FlowInd_t* flowInd = (FlowInd_t*) aCep->fMsg;

    if (flowInd->fFlowAction == CLS_ACTION_STOP_FLOW) {
	if (aCep->fRole == CONN_PRIMARY)
	    input = PRI_FLOW_SET;
	else
	    input = SEC_FLOW_SET;
    } else {
        if (aCep->fRole == CONN_PRIMARY)
	    input = PRI_FLOW_UNSET;
	else
	    input = SEC_FLOW_UNSET;
    }
    conn_fsm(aCep->fCvtPtr, input);
#endif
    return 0;
}

/* DATA_IND, Pass on		*/
ushort pcls_action_e (TDLUCep *aCep)
{
    TCLSIMsg* theMsg = aCep->fMsg;

    CLSIMsgSetPrimitiveType(theMsg, CLS_REQUEST);
    theMsg->fIdentifierType = CLS_P_CEP_ID;
    theMsg->fIdentifier = aCep->fCepPtr->fPCepId;
    
    CLSIQueuePut(aCep->fDlu->fWriteQ, theMsg);
    return 0;
}

ushort pcls_action_g (TDLUCep *aCep)
{
    DLUSendReqOpnStn(aCep);
    aCep->fXidState = D_XID_SEND;
    return 0;
}

ushort pcls_action_h (TDLUCep *aCep)
{
    DLUSendSignalStn(aCep);
    return 0;
}

ushort pcls_action_i (TDLUCep *aCep)
{
    DLUSendConnectRsp(aCep);
    return 0;
}

ushort pcls_action_j (TDLUCep *aCep)
{
    DLUSendDiscRsp(aCep);
    pcls_fsm(aCep->fCepPtr, 0, SND_DISC);
    aCep->fCvtPtr->fCvtState = C_IDLE;
    DLUSendCloseStnReq(aCep);
    if (aCep->fCepOrigPtr)
        DLUReplaceBackupCep(aCep);
    return 0;
}

ushort pcls_action_k (TDLUCep *aCep)
{
    DLUSendDiscReq(aCep);
    return 0;
}

ushort pcls_action_l (TDLUCep *aCep)
{
    DLUSendCloseStnReq(aCep);
    if (aCep->fCepOrigPtr)
        DLUReplaceBackupCep(aCep);
    return 0;
}

ushort pcls_action_m (TDLUCep *aCep)
{
    DLUSendCloseStnReq(aCep);
    pcls_fsm(aCep->fCepPtr, 0, SND_DISC);
    aCep->fCvtPtr->fCvtState = C_IDLE;
    if (aCep->fCepOrigPtr)
        DLUReplaceBackupCep(aCep);
    return 0;
}

ushort pcls_action_o (TDLUCep *aCep)
{
    if ((aCep->fFrasType == FRAS_BAN) || 
               (aCep->fCepPtr->fFrasType == FRAS_BAN)) {
        DLUBanRouteDestroy(aCep, aCep->fCepPtr);
    } else {
        (void)DLURouteDestroy(aCep, aCep->fCepPtr);
    }
    return 0;
}


ushort conn_action_a (TDLUCvt *aConn)
{
    pcls_fsm(aConn->fSecCep, NULL, XID_COMP);
    return 0;
}

ushort conn_action_b (TDLUCvt *aConn)
{
    pcls_fsm(aConn->fPriCep, NULL, SIG_STN);
    pcls_fsm(aConn->fSecCep, NULL, SND_CONN_RSP);
    return 0;
}

ushort conn_action_c (TDLUCvt *aConn)
{
    DLUSendFlowRq(aConn->fSecCep, 1);
    return 0;
}

ushort conn_action_d (TDLUCvt *aConn)
{
    DLUSendFlowRq(aConn->fPriCep, 1);
    return 0;
}

ushort conn_action_e (TDLUCvt *aConn)
{
    DLUSendFlowRq(aConn->fSecCep, 0);
    return 0;
}

ushort conn_action_f (TDLUCvt *aConn)
{
    DLUSendFlowRq(aConn->fPriCep, 0);
    return 0;
}

ushort conn_action_g (TDLUCvt *aConn)
{
    pcls_fsm(aConn->fPriCep, NULL, XID_COMP);
    return 0;
}

static void DLUPXid3(TDLUCep *aCep)
{
    if (xid_get_format(aCep->fXidPtr) == XID_FMT_3) {
	aCep->fSendWindow = xid3_get_maxiframe(aCep->fXidPtr);
	if (aCep->fCepPtr->fSapPtr->fDLCType == CLS_SDLC)
	    xid3_set_abm(aCep->fXidPtr, FALSE);
        else 
	    xid3_set_abm(aCep->fXidPtr, TRUE);
    }
}

void DLUProcessXid(TDLUCep *aCep)
{
    word retCode;
    hword primitive;
    TCLSIMsg* theMsg = aCep->fMsg;
    IdCnf_t* IdCnf = (IdCnf_t *)theMsg;
    IdInd_t* IdInd = (IdInd_t *)theMsg;

    primitive = CLSIMsgGetPrimitiveCode(theMsg);
    if (aCep->fCepPtr->fXidState != D_XID_XCHG) 
	return;

    retCode = CLSIMsgGetReturnCode(theMsg);
    if (retCode != CLS_OK)
	return;

    if (primitive == (CLS_ID | CLS_CONFIRM)) {
        aCep->fXidPtr = IdCnf->fXID + IdCnf->fReserveLength;
        aCep->fXidLength = IdCnf->fCLSIHdr.fDataSize - 
		(offsetof(IdCnf_t, fXID)-CLS_ID_CNF_HDR_SZ) -
		    IdCnf->fReserveLength;
        DLUPXid3(aCep);
	DLUSendIDRsp(aCep->fCepPtr, aCep->fXidPtr, aCep->fXidLength);
    } else {
        aCep->fXidPtr = IdInd->fXID + IdInd->fReserveLength;
        aCep->fXidLength = IdInd->fCLSIHdr.fDataSize - 
	    (offsetof(IdInd_t, fXID)-CLS_ID_IND_HDR_SZ) -
		IdInd->fReserveLength;

	/* 
	   CSCdj04321:
	   Check the XID from the station and see if it's a
	   non-activation xid. If so, it's an error response.
	   Re-proxy the station with another NULL XID.
        */
	
	if ((aCep->fXidState == D_XID_XCHG) &&
	    (xid_is_nonactivation(aCep->fXidPtr))) {
	  buginf("\nFRAS: received non-activation XID");
	  DLUSendIDReq(aCep, NULL, 0);  /* resend NULL XID to PC */
	  return;
	}
        DLUPXid3(aCep);
	DLUSendIDReq(aCep->fCepPtr, aCep->fXidPtr, aCep->fXidLength);
    }
}

char const *const frasBanBnnDebug (TDLUCep *aCep)
{
    if ((aCep == NULL) ||
        (aCep->fCepPtr == NULL) ||
        (aCep->fSapPtr == NULL) ||
        (aCep->fCepState < 0) ||
        (aCep->fCepState >= NUM_LS_STATES)) {
            return(BanBnnDebugTbl[BANUN]);
    }

    if (aCep->fFrasType == FRAS_BNN) { /* BNN */
        if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
            /* this is a FRAME RELAY cep */
            return BanBnnDebugTbl[BNNFR];
        }
        else if (aCep->fSapPtr->fDLCType == CLS_RSRB) {
            /* this is a backup rsrb cep ( towards the host ) */
            return BanBnnDebugTbl[BNNFR];
        }
        else {
            /* it is a LAN cep */
            return BanBnnDebugTbl[BNNLAN];
        }
    }
    else { /* BAN */
        if (aCep->fSapPtr->fDLCType == CLS_SDLC) {
            /* this is LAN cep sdlc attached device. */
            return BanBnnDebugTbl[BANLAN];
        }
        else if (aCep->fSapPtr->fDLCType == CLS_FRAME_RELAY) {
            /* this is a frame-relay cep */
            return BanBnnDebugTbl[BANFR];
        }
        else if ((aCep->fSapPtr->fDLCType == CLS_RSRB) ||
                 (aCep->fSapPtr->fDLCType == CLS_VDLC)) {
            if (aCep->fCepPtr->fCepOrigPtr != NULL) {
               /* this is a backup rsrb cep ( towards the host ) */
               return BanBnnDebugTbl[BANFR];
            }
            else {
               /* this is from LAN side. */
               return BanBnnDebugTbl[BANLAN];
            }
        }
        else {
            return BanBnnDebugTbl[BANUN];
        }
    }
}

