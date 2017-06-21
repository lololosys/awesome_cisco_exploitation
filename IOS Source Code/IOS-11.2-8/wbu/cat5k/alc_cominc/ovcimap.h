/* $Id: ovcimap.h,v 1.1.4.1 1996/05/09 14:54:35 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/ovcimap.h,v $
 *-----------------------------------------------------------------
 * ivcimap.h --  Outgoing VCI maps.
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ovcimap.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:35  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:44  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:03  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:00  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:31  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * ivcimap.h --  Outgoing VCI maps.
 *
 *------------------------------------------------------------------
 *
 *  FILE:  ovcimap.h
 *
 *	CONTENTS:
 *
 *  This file describes the structure of the outging vci translation tables.
 *	The translation table are kept either in the TX SARs internal VC ram, or
 *	in the secondry ram if the VCR cannot hold more entries.  The location of
 *	a prticular map entry is indicated by the map entry location table.  
 *
 *	OWNER:
 *
 *		Ratinder P. S. Ahuja
 *
*/

#ifndef _OVCIMAP_H_
#define _OVCIMAP_H_

#include	"basic_types.h"


	/*
	**
	**	S A R   S T A T I S T I C S
	**
	*/


#ifdef	LANGUAGE_C

typedef struct txsar_glstat {			/* Global stats					*/

	/*
	**	Cell STATS (kept by the TXSAR)
	*/
	UINT32	txgl_cell_xmit;				/* total cells transmitted 		*/

	
	/*
	**	FRAME COUNTS
	*/
    UINT32	txgl_unknownflood_output;	/* dest vc unknown				*/
    UINT32	txgl_upacket_output;		/* unicast frames 				*/
    UINT32	txgl_bpacket_output;		/* bcast / mcast  frames		*/
    UINT32	txgl_packet_acp;			/* frames sent to nmp by ipc	*/
	UINT32	txgl_start_learp;			/* events sent to acp			*/


	/*
	**	ERROR COUNTS
	*/
	UINT32	txgl_unknownbus_err;		/* bus vc unknown				*/


} tTXSARGLSTAT, *tpTXSARGLSTAT;


	/*
	**	PER VC STATS
	*/

typedef struct txsar_vcstat {

	UINT32	txvc_packet_output;			/* frames transmitted 			*/
	UINT32	txvc_ntccell_output;		/* cells transmitted			*/ 

} tTXSARVCSTAT, *tpTXSARVCSTAT;

#else	/* assembly */

 
	/*
	**	GLOBAL STATS
	*/

        .data
        .struct 0
    /*
    **  NTC STATS
    */
txgl_cell_xmit_s:			.word 0          /* total cells transmitted      */
 
   
    /*
    **  FRAME COUNTS
    */
txgl_unknownflood_output_s:	.word 0   		/* dest vc unknown              */
txgl_upacket_output_s:		.word 0        	/* unicast frames               */
txgl_bpacket_output_s:		.word 0        	/* bcast / mcast  frames        */
txgl_packet_acp_s:			.word 0        	/* frames sent to nmp by ipc    */
txgl_start_learp_s:			.word 0        	/* events sent to acp           */
 
 
    /*
    **  ERROR COUNTS
    */
txgl_unknownbus_err_s:		.word 0        /* bus vc unknown               */
txgl_size_s:
 
        .text           				/* reestablish the default 		*/

	/*
	**	PER VC STATS
	*/

        .data
        .struct 0
txvc_packet_output_s:	.word 0			/* frames transmitted 			*/
txvc_ntccell_output_s:	.word 0			/* cells transmitted			*/ 
txvc_size_s:
        .text           				/* reestablish the default 		*/
#endif /* language */


/*
**
**	O V C I   M A P   E N T R Y
**
*/

#define	LOOPT_VPC_C		6			/* test vc for the loop test			*/

#ifdef LANGUAGE_C

extern  UINT16	max_color;
extern	tVPC	bus_vpc[];			/* this array holds the bus vpc for a	*/
extern	UINT8	ovc_state[];		/* state of vc. MAX_VPC entires kept in	*/
									/* vcr									*/
typedef struct ovci_map_entry {		/* created, in the vcr,  for current 	*/
									/* packet undergoing fragmentation		*/
	UINT32		omap_tlen;			/* total lenght of data					*/
	UINT32		omap_cellhdr;		/* gfc:4, vpi:8, vci:16, pt:3, clp:1	*/

} tOMAPENTRY, *tpOMAPENTRY;

#else /* assembly */

        .data
        .struct 0
omap_tlen_s:		.word 0   
omap_cellhdr_s:		.word 0     

        .text           				/* reestablish the default 		*/
#endif /* language */


#endif /* _OVCIMAP_H_ */
