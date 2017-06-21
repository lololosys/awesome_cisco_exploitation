/* $Id: lec_ipc.h,v 1.1.4.1 1996/05/09 15:01:48 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/sysif/lec_ipc.h,v $
 *------------------------------------------------------------------
 * lecipc.h -- lec ipc interactions
 * 
 * This file defines the lec - earl and lec - brige task interactions
 *
 * (date), Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: 
 *------------------------------------------------------------------
 * $Log: lec_ipc.h,v $
 * Revision 1.1.4.1  1996/05/09  15:01:48  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:43  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.3  1996/04/26  01:25:25  sakumar
 * Code changes after review of VTP helper code.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.2  1996/04/11  05:31:29  rlowe
 * Add LCP driver and upcall support for VTP messages (over SCP).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:43:13  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/02  05:08:15  rlowe
 * Fix up include file paths.  Cleanup main ALC makefile regarding
 * sar images and tools.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:13:54  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:52  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:37  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:44:52  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.9  1995/11/03  03:10:41  rlowe
 * Allow LANE to see all flavours of spanning-tree port state changes.
 *
 * Revision 1.8  1995/10/16  22:49:28  sgarcia
 * Added or completed file header.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 
#ifndef _LECIPC_H_
#define _LECIPC_H_
 
 
    /*
    **
    ** INCLUDES
    **
    */
#define NO_VLAN

#include	"../alc_cominc/basic_types.h"

#include	"../sysif/utypes.h"
#include	"../sysif/ibframe.h"
#include	"../sysif/ibipc.h"
#include	"../sysif/ib_externs.h"
 
/*
**	LEC <--> NMP ipc messaging
*/


typedef struct  lecipc_msghdr {

  UINT32 leipc_msgtype;	            /* message op code	*/
  UINT32 leipc_reqid;               /* message transaction id	*/
  UINT32 leipc_msgparam[1];         /* parameters start here	*/

} tLEIPCHDR, *tpLEIPCHDR;


#define	mLEIPC_HSZ	( (UINT32)&((tpLEIPCHDR)0)->leipc_msgparam[0] - \
					  (UINT32)((tpLEIPCHDR)0) )

/*
** Messages:
*/
#define LEIPC_SET_MAC_REQ	1
#define LEIPC_SET_MAC_RESP	2
#define LEIPC_GET_MAC_REQ	3
#define LEIPC_GET_MAC_RESP	4
#define LEIPC_CLEAR_MAC_REQ	5
#define LEIPC_PORT_CREATE_REQ	6
#define LEIPC_PORT_DESTROY_REQ	7
#define LEIPC_PORT_BLOCK_IND	8 /* obsoleted by LEIPC_PORT_STATE_CHANGE_IND */
#define LEIPC_PORT_UNBLOCK_IND	9 /* obsoleted by LEIPC_PORT_STATE_CHANGE_IND */
#define LEIPC_TOPCHANGE_IND	10
#define LEIPC_PORT_CREATE_RESP	11
#define LEIPC_PORT_DESTROY_RESP	12
#define LEIPC_REMOVE_MACS_ON_VCD 13
#define	LEIPC_PORT_STATE_CHANGE_IND 14
#define	LEIPC_VTP_READY_IND 	15



	/*
	**	LEC - EARL Messaging
	**	The interactions needed are:
	**
	**		1.  set mac req 	(le -> earl)
	**		2.  get mac req 	(le -> earl)
	**		3.	get mac resp	(earl -> le)
	**		4.	clear mac req	(le -> earl)
	**
	**		All calls are synchrounous, i.e. blocking.  Upon returning
	**	from the call, the return code specifies success or failure of the
	**	ipc transport delivery.  It does not imply that the receiving 
	**	entity acted on the message.
	**
	*/

/*
**
**	Message parameters for earl messages
**
*/


typedef struct earlipc_param {

    UINT32	color;		/* 10 bit lan id as understood by earl	*/
    UINT32	vpci;		/* 10 bit vpi / vci derivative		*/
    UINT32	errcode;
    UINT8	macaddr[6];	/* mac address 				*/
	
} tEARL_PARAM, *tpEARL_PARAM;	/* params of the IPC mssg to earl	*/


#define EARLIPC_ERR     1
#define EARLIPC_OK      2


	/*
	**
	**	LEC - Spanning tree messaging
	**
	**		1.  create port req		(le -> spanning tree)
	**		2.  destroy port req	(le -> spanning tree)
	**		3.	port state ind		(spanning tree -> le)
	**		4.	topology change ind (spanning tree -> le)
	*/


/*
**	Parameters for spanning tree messages
*/


typedef struct sptipc_param {
	
	UINT32	port_color;
	UINT32	port_state;
    	UINT32	errcode;

} tSPT_PARAM, *tpSPT_PARAM;

typedef struct SptTopChange_param {
  UINT32  Vlan;
  UINT32  TopologyState;
  UINT32  ForwardDelay;
} tSPT_TOPCHANGE_PARAM, *tpSPT_TOPCHANGE_PARAM;


#define LEC_TOPOLOGY_NOT_CHANGING 0
#define LEC_TOPOLOGY_CHANGING     1


#define SPTIPC_ERR      1
#define SPTIPC_OK       2


#define LEC_PORTUNKNOWN  0
#define LEC_PORTBLOCK    1
#define LEC_PORTUNBLOCK  2
#define LEC_PORTDISABLD  3
#define LEC_PORTLISTEN   4
#define LEC_PORTLEARN    5

 

typedef struct lec_ipcmsg_t_ {
 
    UINT32      leipc_msgtype;          /* message op code */
    UINT32      leipc_reqid;            /* message transaction id */
    union {
        tEARL_PARAM earl_msg;
        tSPT_PARAM  spanning_msg;
	tSPT_TOPCHANGE_PARAM TopChange_msg;
    } param;
 
} tLEC_IPCMSG, *tpLEC_IPCMSG;
 

/*
**
**	PROTOTYPES
**
*/


extern UINT32  leipc_init(void);
extern void  leipc_term(void);
 
    /*
    **  Specific message handlers
    */
 
extern UINT32 leipc_send_earl_req(UINT32      reqid,
                             	  UINT32      req_type,
                            	  tpMACADDR   pMac,
                            	  tCOLOR      color,
	     			  tVPC		  vpci);
 
extern UINT32 leipc_send_spt_req(UINT32      reqid,
                            	 UINT32      req_type,
                            	 tCOLOR      lecid);
				/* color relates to lecid and vlanid	*/

extern UINT32 leipc_portstate_ind(UINT32      color,
		    		UINT32      portstate,
				UINT32      portnum);

extern UINT32 vtpipc_vlanparams_cmd(UINT8 * pVPC);
					 

#endif /* _LECIF_H_ */
















