/* $Id: ivcimap.h,v 1.1.4.1 1996/05/09 14:54:32 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/ivcimap.h,v $
 *-----------------------------------------------------------------
 * ivcimap.h --  Incoming VCI maps.
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ivcimap.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:32  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:00  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  03:09:52  sakumar
 * Apply ATM line card 2.1 fixes to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:58  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:28  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * ivcimap.h --  Incoming VCI maps.
 *
 *------------------------------------------------------------------
 *
 *  FILE:  ivcimap.h
 *
 *	CONTENTS:
 *
 *  This file describes the structure of the incoming vci translation tables.
 *	The translation table are kept either in the RX SARs internal VC ram, or
 *	in the secondry ram if the VCR cannot hold more entries.  The location of
 *	a prticular map entry is indicated by the map entry location table.  
 *
 *	OWNER:
 *
 *		Ratinder P. S. Ahuja
 *
*/

#ifndef _IVCIMAP_H_
#define _IVCIMAP_H_

#include	"basic_types.h"
#include	"buffer_if.h"

	/*
	**
	**	S A R   S T A T I S T I C S
	**
	*/


#ifdef	LANGUAGE_C

typedef struct rxsar_glstat {

		/*
		**	NTC stats
		*/

	UINT32	rxgl_ntccell_drop;			/* total cells dropped by ntc	*/
	UINT32	rxgl_ntccell_recvd;			/* cells correctly recvd by ntc	*/
	UINT32	rxgl_ntchec_detect;			/* HEC error detected			*/
	UINT32	rxgl_ntchec_correct;		/* HEC error corrected			*/

		/*
		**	Frame stats
		*/
    UINT32	rxgl_packet_input;			/* number of frames received	  */
    UINT32	rxgl_packet_nmp;			/* frames sent to nmp by ipc	  */

		/*
		**	Frame Error counts
		*/
    UINT32	rxgl_cell_unknownvc_err;	/* cells dropped due to invalid vci	*/
    UINT32	rxgl_cell_no_buf_err;		/* cells dropped due to no buffers	*/
    UINT32	rxgl_packet_rto_err;		/* frames dropped due to timeout 	*/
    UINT32	rxgl_packet_crc_err;		/* frames dropped due to crc 		*/
    UINT32	rxgl_packet_bufovl_err;		/* frames dropped due buf overflow	*/
    UINT32	rxgl_packet_bigaovl_err;	/* frames dropped due biga overflow */
    UINT32  rxgl_packet_bad_len_err;    /* frames dropped bcos bad AAL5 len */

} tRXSARGLSTAT, *tpRXSARGLSTAT;

typedef struct rxsar_stat {				/* per vc stats					*/

	UINT32	rxvc_packet_input;			/* frames received 				*/
	UINT32	rxvc_ntccell_input;			/* cells recvd 					*/

} tRXSARVCSTAT, *tpRXSARVCSTAT;


#else

	/*
	**	GLOBAL RX SAR STATS
	*/

        .data
        .struct 0
        /*
        **  NTC stats
        */
 
rxgl_ntccell_drop_s:		.word 0          /* total cells dropped by ntc   */
rxgl_ntccell_recvd_s:		.word 0          /* cells correctly recvd by ntc */
rxgl_ntchec_detec_s:		.word 0          /* HEC error detected           */
rxgl_ntchec_correct_s:		.word 0          /* HEC error corrected          */
 
        /*
        **  Frame stats
        */
rxgl_packet_input_s:		.word 0           /* number of frames received	  */
rxgl_packet_nmp_s:			.word 0           /* frames sent to nmp by ipc    */
 
        /*
        **  Frame Error counts
        */
rxgl_cell_unknownvc_err_s:	.word 0      /* cells dropped due to invalid vci */
rxgl_cell_no_buf_err_s:		.word 0		 /* cells dropped due to no buffers	*/
rxgl_packet_rto_err_s:		.word 0      /* frames dropped due to timeout    */
rxgl_packet_crc_err_s:		.word 0      /* frames dropped due to crc        */
rxgl_packet_bufovl_err_s:	.word 0      /* frames dropped due buf overflow  */
rxgl_packet_bigaovl_err_s:	.word 0      /* frames dropped due biga overflow */
rxgl_packet_bad_len_err_s:  .word 0      /* frames dropped bcos bad AAL5 len */
rxgl_size_s:

        .text           				/* reestablish the default 		*/

	
	/*
	** PER VC RX STATS
	*/

        .data
        .struct 0
rx_packet_input_s:			.word 0			/* frames received 				*/
rx_ntccell_input_s:			.word 0			/* cells recvd 					*/

        .text           					/* reestablish the default 		*/
#endif	/* language */



/*
**
**	I V C I   M A P   E N T R Y
**
*/

#ifdef LANGUAGE_C

typedef struct ivci_map_entry {

	UINT32		imap_vpc_state;		/* state of entry as follows			*/
	UINT16		imap_vlan_cl;		/* Color and conditional Learn bit      */
	UINT16		imap_cid_vpc;		/* Card Instance and VPC				*/
	tpBUFFER	imap_pRbuf;			/* recv buffer associated with vpc		*/
	UINT8		*imap_pDma;			/* next dma address in buffer			*/
	UINT32		imap_crc32;			/* partial crc computed by rxsar		*/
	UINT16		imap_rlen;			/* Count of bytes received so far		*/
	UINT16		imap_lecid;			/* LECID for this VC (for Echo Supp)	*/
	struct ivci_map_entry	*imap_pRTO_next;/* Pointer to next VC in TimeOut List */ 
	struct ivci_map_entry	*imap_pRTO_prev;/* Pointer to prev VC in TimeOut List */ 
	UINT32		imap_RTO_time;		/* Time at which this Rcv will time out	*/
	tpRXSARVCSTAT	imap_pVCStat;	/* Pointer to per-VC Statistics area	*/
} tIMAPENTRY, *tpIMAPENTRY;


/*
**
**  I M A P   T A B L E.  array of pointers to map entires.  The map
**  entires may reside either in secondry ram, or in the internal vcr.
**
*/
 
extern tpIMAPENTRY  imap_table[];   /* array of map entry pointers          */
#define MAPTABLE_SIZE   MAX_VPC
 
/*
**
**  The VCR inside the RX SAR holds the first N map entries.  The vcr_map table
**  represents occupancy in the VCR.
**
*/
 
extern UINT32 max_vcr_entries;      /* maximum number of map entries in vcr */
extern UINT32 used_vcr_entries;     /* currently used vcr entries           */


#else	/* assembly */

        .data
        .struct 0
imap_vpc_state_s:	.word 0
imap_vlan_cl_16s:	.half 0		/* Color and conditional Learn bit      */
imap_cid_vpc_16s:	.half 0		/* Card Instance and VPC				*/
imap_pRbuf_s:		.word 0
imap_pDma_s:		.word 0
imap_crc32_s:		.word 0
imap_rlen_16s:		.half 0
imap_lecid_16s:		.half 0
imap_pRTO_next_s:	.word 0
imap_pRTO_prev_s:	.word 0
imap_RTO_time_s:	.word 0
imap_pVCStat_s:		.word 0		/* Pointer to per-VC Statistics area	*/

        .text           				/* reestablish the default 		*/
#endif /* language */



#ifdef LANGUAGE_C

typedef struct vcr_map_table {

	UINT32	status;					/* status of entry in vcr				*/
#define VCR_EFREE	0				/* entry free or used					*/
#define VCR_EUSED	1
	
} tVCRENTRY, *tpVCRENTRY;


extern tVCRENTRY vcr_table[];		/* max_vcr_entries table				*/
#define NUM_VCR_ENTRIES	128

extern tpVCRENTRY pNextFree;		/* */

#endif	/* language */

#endif	/* _IVCIMAP_H_ */
