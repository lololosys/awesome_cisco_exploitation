/* $Id: sar_if.h,v 1.1.4.3 1996/07/02 23:44:52 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/sar_if.h,v $
 *-----------------------------------------------------------------
 * sar_if.h -- sar driver interface file.  Common between SAR driver
 *             running on the ACP and the TX and RX SARs
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sar_if.h,v $
 * Revision 1.1.4.3  1996/07/02  23:44:52  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Incorporated CLI change requests for PVC config and throttle config.
 *
 * Revision 1.1.4.2  1996/06/18  16:44:43  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  14:54:37  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:02  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:47  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.3  1996/04/03  21:18:24  sakumar
 * LE-ARP indication from TSAR sends SRC address (as well as DEST address).
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.2  1996/03/30  02:17:27  sakumar
 * Changes to support sending of FLUSH message on Cat5K ATM LANE module.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:05  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:03  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:33  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 *
 *  FILE:  sar_if.h
 *
 *	CONTENTS:
 *
 *  This file describes the interface to the SARs.  It defines the ACP to SAR
 *	messaging queues, commands and responses/indications codes.
 *
 *	OWNER:
 *
 *		Ratinder P. S. Ahuja
 *
*/

#ifndef _SARIF_H_
#define _SARIF_H_

	/*
	**
	** INCLUDES
	**
	*/

#include "basic_types.h"
#include "queue_if.h"			/* describes the queue structure*/
#include "buffer_if.h"
#include "ivcimap.h"
#include "ovcimap.h"
#include "cam_if.h"


	/*
	**	
	**	C O M M A N D   R E Q U E S T  /  R E S P O N S E    M E S S A G E S
	**
	*/

#ifdef LANGUAGE_C

typedef struct sar_cmd {		/* request / response structure	*/

 	UINT8	cmd_length;		/* length of command + parmas	*/
 	UINT8	cmd_wakeup;		/* command reference number 	*/
 	UINT8	cmd_code;		/* request / response code		*/
 	UINT8	cmd_status;		/* completion status			*/
	union {
		UINT8 cmdval[56];	/* parameters start here		*/
		void    *pcmdval;	/* or pointer to parameter		*/
	} cmd_parm_u;
#define cmd_parmv  cmd_parm_u.cmdval
#define cmd_parmp  cmd_parm_u.pcmdval
} tSARCMD, *tpSARCMD;

#define mCMDOFFSET 4

#else	/* assembly */

        .data
        .struct 0
cmd_hdr_s:		.word 0
cmd_parm_s:		.word 0

        .text         			/* reestablish the default 		*/
#endif	/* language */

	/*
	**	Mask to apply to command header
	*/
#define CMDLEN_MASK_C	0xff000000
#define	CMDREF_MASK_C	0x00ff0000
#define	CMDCODE_MASK_C	0x0000ff00
#define	CMDSTAT_MASK_C	0x000000ff


	/*
	**
	**  Various commands, responses, indications and completion codes
	**	The commands for the SARs are:
	**   
	**	VPC map related:
	**   	set vci map	(tx & rx sar)
	**   	clear vci map	(tx & rx sar)
	**   	get_next vci map	(tx & rx sar)
	**
	**	CAM related:
	**		init cam
	**   	set cam entry			(tx sar)
	**   	clear cam entry			(tx sar)
	**		set pacing counter		(tx sar)
	**   	get cam entry			(tx sar)
	**
	**	Resets:
	**		warm start tx sar
	**		warm start rx sar
	**
	*/

/*
**  REQUESTS  / RESPONSE CODES as words
*/
#define TXSAR_SET_VPCMAP_C		0x00000100
#define TXSAR_GET_VPCMAP_C		0x00000200
#define TXSAR_CLEAR_VPCMAP_C		0x00000300
#define TXSAR_INIT_CAM_C		0x00000400
#define TXSAR_CREATE_CAM_C		0x00000500
#define TXSAR_SET_CAM_C			0x00000600
#define TXSAR_GET_CAM_C			0x00000700
#define TXSAR_CLEAR_CAM_C		0x00000800
#define TXSAR_SET_PACING_CTR_C		0x00000900
#define TXSAR_SEND_REQUEST_C		0x00000A00
#define TXSAR_RESTART_C			0x00000B00
#define	TXSAR_SET_VPI_VCI_BITS_C	0x00000C00
#define TXSAR_CHANGE_CAM_STATE_C        0x00000D00
#define	TXSAR_SET_THROTTLE_C		0x00001700
	/*
	** 	ASYNC events from txsar to ACP
	*/
#define	TXSAR_START_LEARP_IND_C		0x00000E00
#define	TXSAR_DATA_IND_C		0x00000F00
#define	TXSAR_CLEAR_MAC_IND_C		0x00001000

#define TXSAR_NUM_OF_COMMANDS           17


#define RXSAR_SET_VPCMAP_C		0x00001100
#define RXSAR_GET_VPCMAP_C		0x00001200
#define RXSAR_CLEAR_VPCMAP_C		0x00001300
#define RXSAR_SEND_TO_NMP_C		0x00001400
#define	RXSAR_SET_VPI_VCI_BITS_C	0x00001500
#define RXSAR_MODIFY_VPCMAP_C		0x00001600



#define RXSAR_RESTART_C			0x00001A00

#define	SAR_NOP_C			0x0000FF00	/* NO-OP for the sars */

/*
**	COMPLETION CODES
*/
#define	SAR_CMD_INVALID_C		0x00
#define	SAR_CMD_SUCCESS_C		0x01
#define	SAR_CMD_FAIL_C			0x02
#define	RXSAR_INVALID_VPC_C		0x03
#define	TXSAR_INVALID_VPC_C		0x05
#define	TXSAR_INVALID_CAM_C		0x06
#define	TXSAR_INVALID_LEN_C		0x07



	/*
	**
	** 	P A R M E T E R S 
	** 
	*/


/*
**	SET /CLEAR/ GET_NEXT VCI
*/

#ifdef LANGUAGE_C

typedef struct vpc_parm {
	
	tVPC			vpcp_vpc;	/* reduced VPC value		*/
	UINT16			vpcp_lecid;	/* LEC ID 			*/
	UINT32			vpcp_vcstate;	/* status bits			*/
	tQOS			vpcp_qos;	/* quality of service holder	*/
	tpIMAPENTRY		vpcp_pVcMapEntry;/* Where the map entry is	*/
	UINT16			vpcp_vlan_cl;	/* Color and cond Learn bit     */
	UINT16			vpcp_cid_vpc;	/* Card Instance and VPC        */
	tpRXSARVCSTAT		vpcp_pRVCStat;	/* Ptr to RX per-VC Stat buffer */
	tpTXSARVCSTAT		vpcp_pTVCStat;	/* Ptr to TX per-VC Stat buffer */
	
} tVPCPARM, *tpVPCPARM;

#else	/* assembly */

        .data
        .struct 0
vpcp_hdr_s:				.word 0
vpcp_vpc_16s:			.half 0
vpcp_lecid_16s:			.half 0
vpcp_vcstate_s:			.word 0
vpcp_qos_s:				.word 0
vpcp_pVcMapEntry_s:		.word 0
vpcp_vlan_cl_16s:		.half 0		/* Color and cond Learn bit     */
vpcp_cid_vpc_16s:		.half 0		/* Card Instance and VPC        */
vpcp_pRVCStat_s:		.word 0		/* Ptr to RX per-VC Stat buffer */
vpcp_pTVCStat_s:		.word 0		/* Ptr to TX per-VC Stat buffer */

        .text           			/* reestablish the default 	*/
#endif	/* language */

/*
**	vpcp_vcstate values
*/
#define VC_INVALID		0xffff
#define VPC_INACTIVE		0x0000
#define VPC_INACTIVE_C		VPC_INACTIVE
#define VPC_DEST_ACP		0x0001		/* inbound traffic goes to ACP	*/
#define VPC_DEST_ACP_TXSAR	0x0003		/* ACP and TX sar		*/
#define VPC_DEST_BIGA		0x0004		/* inbound traffic goes to BIGA	*/
#define VPC_DEST_ACP_BIGA	0x0005		/* BIGA and ACP			*/
#define	VPC_DEST_BIGA_FROM_BUS 0x0009	/* to BIGA from BUS		*/

#define VPC_RCV_FOR_BUS		0x000A		/* VC receive packets for BUS */
										/* i.e. send pkt out to TXSAR */
#define VPC_XMIT_BY_BUS		0x000B		/* VC transmits packets by BUS */

#define VPC_SNAP_PVC_TO_BIGA 0x000C     /* LLC SNAP encapsulation PVC */

#define VPC_CTRL_BIT		0x4000		/* Send packet to ACP, over-riding */
                                        /* VPC_DEST_BIGA (0xffff at start) */
#define VPC_ECHO_SUPP_BIT	0x2000		/* for Echo Suppression		*/
#define VPC_WT_TL_BIND_BIT	0x1000		/* Wait Till Bind Bit		*/


#define	VPC_TX_ACTIVE		0x0006		/* TX sar allowed to transmit	*/
#define	VPC_TX_ACTIVE_C		VPC_TX_ACTIVE
#define	VPC_TX_UCAST_C		0x0007		/* unicast vc			*/
#define	VPC_TX_BCAST_C		0x0008		/* broadcast vc			*/

#define VPC_HOLD_BIT		0x8000		/* if high bit set, hold traffic*/


/*
**	SET /CLEAR/ GET_NEXT CAM
*/

#ifdef LANGUAGE_C

typedef struct cam_parm {
	
	tMACADDR	camp_macaddr;		/* mac + vlanid form cam address*/
	tCOLOR		camp_vlanid;		/* color or vlan id		*/
	INT16		camp_index;		/* index into the cam table	*/
						/* computed by the tx sar, after*/
						/* doing an initial cam lookup	*/
	UINT16		camp_dd_vpc;		/* Data direct VPC              */
	UINT8		camp_state;		/* State of CamTableEntry       */
} tCAMPARM, *tpCAMPARM;

#else	/* assembly */

        .data
        .struct 0
camp_hdr_s:		.word 0
camp_macaddr_s:		.space 6
camp_vlanid_16s:	.half 0
camp_index_16s:		.half 0
camp_dd_vpc_16s:	.half 0
camp_state_8s:		.byte 0

        .text           			/* reestablish the default 	*/
#endif	/* language */



	/*
	**
	**	TX SAR Send request
	**
	*/

#ifdef LANGUAGE_C

typedef struct txsend_req {
	
	UINT32	txsend_len;			/* length of data buffer	*/
	UINT32	txsend_vpc;			/* which vpc to send it on	*/
	void	*txsend_pbuf;			/* pointer to data		*/

} tTXSEND_REQ, *tpTXSEND_REQ;

#else	/* assembly */

        .data
        .struct 0
txsend_hdr_s:		.word 0			/* generic command header	*/
txsend_len_s:		.word 0
txsend_vpc_s:		.word 0
txsend_pbuf_s:		.word 0

        .text           			/* reestablish the default 	*/
#endif	/* language */

	/*
	**
	** 	RX SAR Send-to-NMP request
	**
	*/

#ifdef LANGUAGE_C

typedef struct rx_nmp_req {
	void       *rx_nmp_pbuf;			/* pointer to data buffer    */
	UINT32	   rx_nmp_len;				/* length of data (in bytes) */
    UINT16     rx_nmp_vlan_cl;   		/* Color and cond Learn bit     */
    UINT16     rx_nmp_cid_vpc;   		/* Card Instance and VPC        */

} tRX_NMP_REQ, *tpRX_NMP_REQ;

#else /* assembly */

		.data
		.struct 0
rx_nmp_hdr_s:		.word 0				/* generic command header    */
rx_nmp_pbuf_s:		.word 0				/* pointer to data buffer    */
rx_nmp_len_s:		.word 0				/* length of data (in bytes) */
rx_nmp_vlan_cl_16s:	.half 0		   		/* Color and cond Learn bit     */
rx_nmp_cid_vpc_16s:	.half 0		   		/* Card Instance and VPC        */

		.text
#endif /* language */

    /*
    **
    **  RX SAR Set Active VPI/VCI Bits
    **
    */
 
#ifdef LANGUAGE_C
 
typedef struct rx_set_vpi_vci_bits_req {
	UINT32 rx_active_vpi_vci_mask; /* to mask the active bits in Rcv Cell */
	UINT32 rx_num_vpi_shift_bits;  /* number of bits to shift VPI right   */
} tRX_SET_VPI_VCI_BITS_REQ, *tpRX_SET_VPI_VCI_BITS_REQ;
 
#else /* assembly */
 
        .data
        .struct 0
rx_set_vpi_vci_bits_hdr_s:	.word 0	/* generic command header    */
rx_active_vpi_vci_mask_s:	.word 0	/* to mask the active bits in Rcv Cell */
rx_num_vpi_shift_bits_s:	.word 0	/* number of bits to shift VPI right   */
 
        .text
#endif /* language */



    /*
    **
    **  TX SAR Set Active VPI/VCI Bits
    **
    */
 
#ifdef LANGUAGE_C
 
typedef struct tx_set_vpi_vci_bits_req {
    UINT32 tx_active_vci_mask; 		/* to mask the VCI active bits in VPC */
    UINT32 tx_num_active_vci_bits;  /* number of bits to shift VPI right   */
} tTX_SET_VPI_VCI_BITS_REQ, *tpTX_SET_VPI_VCI_BITS_REQ;
 
#else /* assembly */
 
        .data
        .struct 0
tx_set_vpi_vci_bits_hdr_s:  .word 0 /* generic command header    */
tx_active_vci_mask_s:   	.word 0 /* to mask the VCI active bits in VPC */
tx_num_active_vci_bits_s:   .word 0 /* number of bits to shift VPI right   */
 
        .text
#endif /* language */

    /*
    **
    **  TX SAR Set Throttle Parameter
    **
    */

#ifdef LANGUAGE_C

typedef struct tx_set_throttle_parm_req {
    UINT32 tx_throttle_parm;      /* Value to put in throttle timer */
} tTX_SET_THROTTLE_PARM_REQ, *tpTX_SET_THROTTLE_PARM_REQ;

#else /* assembly */

        .data
        .struct 0
tx_set_throttle_parm_hdr_s:  .word 0 /* generic command header    */
tx_throttle_parm_s:          .word 0 /* Value to put in throttle timer */

        .text
#endif /* language */

 


	/*
	**
	**	A S Y N C H    E V E N T S    F R O M   T X   S A R 
	**	EVENT record for TX SAR -> ACP events
	**	Used to indicate: 
	**		1. Start_Le_Arp event
	**		2. Inband data indication
	**	
	**	The event indication comprises of a command record followed by
	**	indication parameters.  For the Start_Le_Arp, the dest mac addr and
	**	color are inidcated.  For the data indication event, length and
	**	a pointer to the data buffer is contained parameter portion
	**
	*/

#ifdef LANGUAGE_C
 
typedef struct start_learp_param {	/* start learp parameters		*/
 
    UINT32	opcode;			/* message type				*/
    tMACADDR    learpp_dmacaddr;	/* dest mac address			*/
    tMACADDR    learpp_smacaddr;	/* src mac address			*/
    tCOLOR      learpp_vlanid;		/* color or vlan id     		*/
    UINT16      learpp_pad;
    void	*learp_came;		/* pointer to camentry 			*/
                                            
} tLEARP_PARAM, *tpLEARP_PARAM;
 
#else   /* assembly */
 
        .data
        .struct 0
learpp_hdr_s:			.word 0
learpp_opcode_s:		.word 0
learpp_dmacaddr_s:		.space 6
learpp_smacaddr_s:		.space 6
learpp_vlanid_16s:		.half 0
learpp_pad_16s:		    .half 0
learpp_came_s:			.word 0
 
        .text                           /* reestablish the default      */

#endif  /* language */



	/*
	**	ASYNC data indication from txsar
	*/

#ifdef LANGUAGE_C
 
typedef struct txsar_dind {			/* data indication from txsar	*/
 
    void	*txdind_pDesc;			/* pointer back to the biga desc*/
    UINT32  	txdind_len;                 	/* length of data buffer        */
    void    	*txdind_pbuf;                   /* pointer to data              */
 
} tTXSAR_DIND, *tpTXSAR_DIND;
 
#else   /* assembly */
 
        .data
        .struct 0
txdind_hdr_s:		.word 0         /* generic command header       */
txdind_pDesc_s:		.word 0		/* pointer back to the biga desc*/
txdind_len_s:       	.word 0		/* length of data in indication	*/
txdind_pbuf_s:      	.word 0

        .text                           /* reestablish the default      */
#endif  /* language */


	/*
	**		P R O T O T Y P E S 
	*/
#ifdef LANGUAGE_C

extern UINT32  txsar_cmd(UINT32 cmd, void * cmd_data);

extern tpSARCMD	txsar_post_nopcmd(void);

extern tpSARCMD txsar_post_vpccmd(UINT8		refnum,
				  UINT32	cmd,
				  tVPC		vpcval,
				  UINT16	vcstate);

extern UINT32 txsar_wait_vpcresp(UINT8	refnum,
				 UINT32	cmd,
				 tVPC	vpcval,
				 UINT16	*pState);

extern tpSARCMD txsar_post_sendcmd(UINT8	refnum,
				   tVPC		vpcval,
				   UINT32	len,
				   tpBUFFER	pBuf);

extern UINT32 txsar_wait_sendresp(UINT8  refnum, 
				  tVPC  vpc, 
				  UINT32 len); 

extern tpSARCMD txsar_post_caminit(void);

extern tpSARCMD txsar_post_camcmd(UINT32  cmd, tpCAMPARM   pCamReq);

extern UINT32 rxsar_cmd(UINT32 cmd, void * cmd_data);



#endif	/* LANGUAGE_C */

#endif /* _SARIF_H_ */
