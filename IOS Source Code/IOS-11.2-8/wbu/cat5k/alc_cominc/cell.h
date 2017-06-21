/* $Id: cell.h,v 1.1.4.1 1996/05/09 14:54:32 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/cell.h,v $
 *-----------------------------------------------------------------
 * cell.h -- Describes an atm cell 
 *
 * October 1994, Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cell.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:32  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:57  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:00  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:57  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:27  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * cell.h -- Describes an atm cell 
 *
 *------------------------------------------------------------------
 *
 *  FILE:  cell.h
 *
 *
 *  CONTENTS:
 *
 *      Tab stop = 4
 *
 *      Assembler Coding Conventions:
 *
 *          <>_REG      for Processor Registers (s0-s7)
 *          <>_AREG     for ATMIZER Registers
 *          <>_s        for structure fields
 *          <>_C        for defined constants
 *          <>_L        for program labels
 *          Vcr_<>_s    for varibles in internal VCR, indexed off vcr base
 *
 *  This file describes the atm cell
 *
 *
 *  OWNER:
 *  Ratinder P. S. Ahuja
 *
 *  LOG:
 *  Created:  19 Oct 1994
 *
*/
 
#ifndef _CELL_H_
#define _CELL_H_

/*
**	INCLUDES
*/

#include  "basic_types.h"

/*
**  CONSTANTS
*/

#define AAL5_HDR_EOM_C 			(1<<1)  /* Bit 0 of PTI Field in ATM Header*/
#define	AAL5_HDRSZ_C			5
#define	AAL5_PAYLOADSZ_C		48
#define	AAL_TRAILERSZ_C			8		/* last 8 octets of last cell	*/
#define	AAL_SINGLECELLSZ_C		40		/* maximum size for a single cell*/
#define	AAL_MAXPADSZ_C			47

#define ABS_VPI_MASK	0x0ff00000
#define ABS_VCI_MASK	0x000ffff0

#define	ABS_VPI_MASK_C	ABS_VPI_MASK
#define	ABS_VCI_MASK_C	ABS_VCI_MASK


/*
**	D A T A   S T R U C T U R E S
*/

	/*
	**	ATM Cell header and payload
	*/

#ifdef LANGUAGE_C

typedef struct atmcell {				/* actual 53 byte atm cell 		*/

	union {								/* 5 byte header				*/
		struct {
			UINT32	gfc:	4;
			UINT32	vpi:	8;
			UINT32	vci:	16;
			UINT32	pt:		3;
			UINT32	clp:	1;
		} atmcell_hdr_bits;
	
		UINT32	hdr;

	} atmcell_hdr_u;					

	UINT8	atmcell_hdrhec;

	UINT8	atmcell_payload[48];		/* 48 byte cell payload			*/

} tATMCELL, *tpATMCELL;

#else /* assembly */

        .data
        .struct 0
atmcell_hdr_s:		.word	0
atmcell_hdrhec_8s:	.byte	0
atmcell_payload_s:	.space	48
atmcell_size_s:

	.text
#endif /* atm cell */

	
	/*
	**	ATM cell as kept in the VCR.  VCR cells always start at 
	**	64 byte boundaries.  The HEC is dealt with by the ACI and is
	**	not placed in the VCR.  Thus a 53 octet actm cell is represented
	**	by 52 octets, without the HEC
	*/

#ifdef LANGUAGE_C

typedef struct	vcrcell {

	UINT8	vcrcell_pad[12];
	UINT32	vcrcell_hdr;				/* 4 byte header				*/
	UINT8	vcrcell_payload[48];		/* 48 byte cell payload			*/

} tVCRCELL, *tpVCRCELL;

#else	/* assembly */

        .data
        .struct 0
vcrcell_pad_s:		.space 12
vcrcell_hdr_s:		.word  0      
vcrcell_payload_s:	.space 48  
vcrcell_size_s:

	.text
#endif /* language */

	
	/*
	**	AAL 5 last cell in VCR 
	*/

#ifdef LANGUAGE_C

typedef struct aal5tail {

	UINT8	aal5tail_pad[56];			/* last cell user data or pad	*/
	UINT8	aal5tail_uui;				/* user to user indication		*/
	UINT8	aal5tail_cpi;				/* common part indication		*/
	UINT16	aal5tail_len;				/* actual length of user cs pdu	*/
	UINT32	aal5tail_crc;				/* crc covers payload, pad, uui,*/
										/* cpi, and length				*/

} tAAL5TAIL,  *tpAAL5TAIL;

#else /* assembly */

        .data
        .struct 0
aal5tail_pad_s:		.space 56
aal5tail_uui_8s:	.byte	0
aal5tail_cpi_8s:	.byte	0
aal5tail_len_16s:	.half	0
aal5tail_crc_s:		.word	0

	.text
#endif /* language */



#endif	/* _CELL_H_ */
