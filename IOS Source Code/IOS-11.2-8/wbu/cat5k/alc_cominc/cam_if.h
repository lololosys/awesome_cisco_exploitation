/* $Id: cam_if.h,v 1.1.4.1 1996/05/09 14:54:31 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/cam_if.h,v $
 *-----------------------------------------------------------------
 * cam_if.h -- cam interface file.  Operated upn by the TX sar
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cam_if.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:31  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/03/30  02:17:22  sakumar
 * Changes to support sending of FLUSH message on Cat5K ATM LANE module.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:09:58  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:56  rlowe
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
 * cam_if.h -- cam interface file.  Operated upn by the TX sar
 *
 *------------------------------------------------------------------
 *
 *  FILE:  cmar_if.h
 *
 *	CONTENTS:
 *
 *  This file describes the interface to the CAM.  The CAM is addressed by the
 *	TX sar to check the state of a destination MAC address.
 *	The CAM is indexed using the 6 byte MAC address and 16 bit color 
 *	information from the synergy header.  It returns a 10 bit number if the
 *	cam entry valid.  This 10 bit number is used to index into a cam table
 *	maintained in the secondry RAM.
 *
 *	OWNER:
 *
 *		Ratinder P. S. Ahuja
 *
*/

#ifndef _CAMIF_H_
#define _CAMIF_H_


	/*
	**
	** INCLUDES
	**
	*/
#include	"param.h"
#include	"basic_types.h"
#include	"queue_if.h"



/*
**	CAM register base addresses in SAR secondary port address space
*/
#define	SAP_SP_ADDRESS		0xA0C00000	/* txsar secondary port base addr*/
#define SAR_WRITE_REG0_C	(0x280000+SAP_SP_ADDRESS)
#define SAR_WRITE_REG1_C	(0x280020+SAP_SP_ADDRESS)
#define SAR_READ_REG0_C		(0x280000+SAP_SP_ADDRESS)
#define	SAR_WRITE_REG2_BASE_C	(0x280040+SAP_SP_ADDRESS)

/* read/write mode when in		*/
/* CAM update, 0:write, 1:read	*/
#define	SAR_CAM_RWMODE_REG_C	(0x280054+SAP_SP_ADDRESS)	

/* data/command mode when in 	*/
/* CAM update, 0:data, 1:comand	*/
#define	SAR_CAM_DCMODE_REG_C	(0x280058+SAP_SP_ADDRESS)	

/* processor/lookup mode 		*/
/* 0:process update, 1:lookup	*/
#define	SAR_CAM_PLMODE_REG_C	(0x28005C+SAP_SP_ADDRESS)	


/*
**	READ_REG0 values
*/
#define	REG0_CAM_PAGE_MASK_C	0x78000000 /* bits 27-30 cam page #	*/
#define REG0_CAM_MATCH_ADDR_C	0x07fe0000 /* bits 17-26 cam match addr	*/
#define	REG0_CAM_MATCH_C	0x80000000 /* bit 31, 1: no match	*/
#define	REG0_CAM_DONE_C		0x00000004 /* bit 2, 1: not done	*/
#define	REG0_CAM_FULL_C		0x00000002 /* bit 1, 0: cam full	*/


/*
**	
**	CAM supports command reads, command writes, data reads and data
**	writes.
**
**	Command READ sequence:
**		- set bit 0 of SAR_CAM_PLMODE_REG to 0 (processor access mode)
**		- set bit 0 of SAR_CAM_DCMODE_REG to 1 (command mode)
**		- set bit 0 of SAR_CAM_RWMODE_REG to 1 ( read mode)
**		- do dumy write to SAR_WRITE_REG0 to start cam operation
**			200 nsec or poll SAR_READ_REG0 for !CAM_IN_OPERATION
**		- 16 bit command read is available at SAR_READ_REG0
**
**  CAM Data READ sequence:
**      - set bit 0 of SAR_CAM_PLMODE_REG to 0 (processor access mode)
**      - set bit 0 of SAR_CAM_DCMODE_REG to 0 (data mode)
**      - set bit 0 of SAR_CAM_RWMODE_REG to 1 (read mode)
**      - do dumy write to SAR_WRITE_REG0 to start cam operation
**          200 nsec or poll SAR_READ_REG0 for !CAM_IN_OPERATION
**      - 16 bit data read is availbale at SAR_READ_REG0
**
**
**  Command WRITE sequence:
**      - set bit 0 of SAR_CAM_PLMODE_REG to 0 (processor access mode)
**      - set bit 0 of SAR_CAM_DCMODE_REG to 1 (command mode)
**      - set bit 0 of SAR_CAM_RWMODE_REG to 0 ( write mode)
**      - write 16 bit command to SAR_WRITE_REG0 to start cam operation
**          200 nsec or poll SAR_READ_REG0 for !CAM_IN_OPERATION
**
**
**  CAM Data WRITE sequence:
**      - set bit 0 of SAR_CAM_PLMODE_REG to 0 (processor access mode)
**      - set bit 0 of SAR_CAM_DCMODE_REG to 0 (data mode)
**      - set bit 0 of SAR_CAM_RWMODE_REG to 0 ( write mode)
**      - write 16 bit command to SAR_WRITE_REG0 to start cam operation
**          200 nsec or poll SAR_READ_REG0 for !CAM_IN_OPERATION
**
*/

#define	CAM_COMMAND_REQ_C	0x0000001
#define	CAM_DATA_REQ_C		0x0000000
#define	CAM_READ_REQ_C		0x0000001
#define	CAM_WRITE_REQ_C		0x0000000

/*
** CAM control register bit assignments
** note that these are reversed in sense for use with CT_SET macro
*/


#define CT_SET(X)       (0x0000ffff ^ (X))
#define CT_RESET            0x8000
 
#define CT_MATCH_EN         0x6000
#define CT_MATCH_DIS        0x4000
 
#define CT_FULL_EN          0x1800
#define CT_FULL_DIS         0x1000
#define CT_TRANSLATE_DIS    0x0600
#define CT_TRANSLATE_EN     0x0400
 
#define CT_64BIT_CAM        0x01c0
#define CT_48BIT_CAM        0x0180
#define CT_32BIT_CAM        0x0140
#define CT_16BIT_CAM        0x0100
#define CT_ALL_RAM          0x00c0
 
#define CT_MASK_DIS         0x0030
#define CT_MASK1_EN         0x0020
#define CT_MASK2_EN         0x0010
 
#define CT_ADDRINC_EN       0x000c
#define CT_ADDRDEC_EN       0x0008
#define CT_AUTOADDR_DIS     0x0004

/*
** bit to modify address mode of commands
*/
 
#define F_ADDR_AR           0
#define F_ADDR_IMMED        1
 
/*
** the RRR values can be used in either SSS or DDD fields
*/
 
#define RRR_CR              0		/* Comparand register			*/
#define RRR_MR1             1
#define RRR_MR2             2
#define RRR_M_AR            4
#define RRR_M_HM            5
#define RRR_M_NF            6		/* Next free in memory array	*/
#define RRR_M_ALM           7
 
/*
** the valid bits
*/

#define VVV_NC              0
#define VVV_V               4			/* set valid	*/
#define VVV_E               5			/* set empty	*/
#define VVV_S               6			/* set skip		*/
#define VVV_R               7			/* set random	*/
 
/*
** which ( if any ) mask register to use
*/

#define MM_NONE             0
#define MM_MR1              1


/*
**
**	CAM compare command
**
*/

#define	CAM_CMP_V		0x0504		/* Compare valid location	*/
#define	CAM_CMP_E		0x0505		/* Compare empty location	*/
#define	CAM_CMP_S		0x0506		/* Compare skipped location	*/
#define	CAM_CMP_R		0x0507		/* Compare ram location		*/



/*
**
**	CAM segment counter register bits
**
*/

#define SET_SCR(X)  (0x8424 ^ (X))
#define SC_SET_DEST_CTR_LIMITS  0x8000
#define SC_SET_SRC_CTR_LIMITS   0x0400
#define SC_LOAD_DEST_CTR    0x0020
#define SC_LOAD_SRC_CTR     0x0004
 
#define SC_DEST_0_0 (0 << 11)
#define SC_DEST_0_1 (1 << 11)
#define SC_DEST_0_2 (2 << 11)
#define SC_DEST_0_3 (3 << 11)
#define SC_DEST_1_1 (5 << 11)
#define SC_DEST_1_2 (6 << 11)
#define SC_DEST_1_3 (7 << 11)
#define SC_DEST_2_2 (0x0a << 11)
#define SC_DEST_2_3 (0x0b << 11)
#define SC_DEST_3_3 (0x0f << 11)
#define SC_SRC_0_0  (0 << 6)
#define SC_SRC_0_1  (1 << 6)
#define SC_SRC_0_2  (2 << 6)
#define SC_SRC_0_3  (3 << 6)
#define SC_SRC_1_1  (5 << 6)
#define SC_SRC_1_2  (6 << 6)
#define SC_SRC_1_3  (7 << 6)
#define SC_SRC_2_2  (0x0a << 6)
#define SC_SRC_2_3  (0x0b << 6)
#define SC_SRC_3_3  (0x0f << 6)
 
#define SC_LOAD_DCTR_0  (0 << 3)
#define SC_LOAD_DCTR_1  (1 << 3)
#define SC_LOAD_DCTR_2  (2 << 3)
#define SC_LOAD_DCTR_3  (3 << 3)
#define SC_LOAD_SCTR_0  0
#define SC_LOAD_SCTR_1  1
#define SC_LOAD_SCTR_2  2
#define SC_LOAD_SCTR_3  3

/*
** Commands...
**	SPS: set persistant source, when reading from cam
**	SPD: set persistant destination, when writing to cam
*/

#define SPS(F,RRR)          (0x0000|(F << 11)|RRR)
#define SPD(F,MM,RRR,VVV)   (0x0100|(F << 11)|(MM << 6)|(VVV)|(RRR << 3))

/*
**
**	CAM validity bit control
**
*/
#define	VBC(F, DDD, VVV)	(0x0100|(F << 11)|(DDD << 3)|(VVV))

/* Set empty at highest priority match */
#define	CAM_VBC_HM_E		VBC(F_ADDR_AR, RRR_M_HM, VVV_E)

/*
**
**	CAM INITIALIZATION COMMAND SEQUENCE
**	The following commands are to be written in to the CAM the order
**	is as specified in the document:  "CAM access operation" by Steve Ong.
**
**
*/


					/* TCO: temporary command override	*/
#define	CAM_TCO_ADREG_C		0x0220	/* select address register		*/
#define	CAM_TCO_DSREG_C		0x0228	/* select device select register	*/
#define	CAM_SELECT_ALLCAMS_C	0xFFFF	/* select all devices			*/
#define	CAM_TCO_CREG_C		0x0200	/* select control register		*/
#define	CAM_RESET_ALLCAMS_C	0x0000	/* reset all selected cams		*/

#define	CAM_TCO_PAREG_C		0x0208	/* select page address register		*/
#define	CAM_PAGE0_C		0x0000	/* set fist device page = 0		*/
#define	CAM_SET_FULLF_C		0x0700	/* set full flag			*/
#define	CAM_PAGE1_C		0x0001	/* set fist device page = 1		*/
#define	CAM_PAGE2_C		0x0002	/* set fist device page = 2		*/
#define	CAM_PAGE3_C		0x0003	/* set fist device page = 3		*/

#define	CAM_CONTROL_C		0x8000	/* No reset, enable match flag,		*/
					/* enable full flag, use all segme-	*/
					/* nts as CAM banks, disable mask 	*/
					/* register, enable address increment*/

#define	CAM_SPD_CR_C		0x0100	/* Select persistant destination,	*/
					/* use comparand register		*/
#define	CAM_TCO_SC_C		0x0210	/* Select segment control register	*/
#define	CAM_SC_VALUE_C		0x18C0	/* Segment control: set dest ctr	*/
					/* limit, start 00, end 11, set src	*/
					/* counter limit, start 00, end 11	*/
					/* load dest ctr with 0, load src	*/	
					/* ctr with 0				*/

#define	CAM_SC_VALUE2_C		SET_SCR(SC_SET_DEST_CTR_LIMITS | SC_DEST_0_3 |  SC_SET_SRC_CTR_LIMITS | SC_SRC_0_3 | SC_LOAD_DEST_CTR | SC_LOAD_DCTR_0 | SC_LOAD_SRC_CTR | SC_LOAD_SCTR_0)

/* Set persistant dest: next free n memory array */
#define	CAM_SPD_MNF_C		SPD(F_ADDR_AR, MM_NONE, RRR_M_NF, VVV_V)
/* Set persistant dest: comparand register */
#define	CAM_SPD_CMP_C		SPD(F_ADDR_AR, MM_NONE, RRR_CR, VVV_NC)

/*
**	
**	CAM Lookup:  WRITE_REG1:  Lower 32 bit mac address bits
**				 WRITE_REG0:  Upper 16 bit mac & 16 bit color 
**
**	CAM Update:  WRITE_REG1:  don't care
**				 WRITE_REG0:  LS 16 bits contain command or data for CAM 
**					updates.
**
*/

/*
**	Bits of 32 bit READ_REG0
*/

#define CAM_MISS_C		0x00000001
#define CAM_NOT_FULL_C		0x00000002
#define CAM_IN_OPERATION_C	0x00000004
#define CAM_ADDRESS_C		0x07FE0000	/* bits 17 to 26		*/
#define CAM_PAGE_C		0xF8000000	/* bits 27 to 31		*/


/*
**  A CAM read results in a 10 bit number that has a 3 bit cam state and 
**	a 7 bit pacing counter 
**
*/

/*
**	CAM states
**
** If you change these, make sure they agree with alc_lec.h.
*/

#define	CAME_FREE_C		0
#define LE_ARP_IN_PROGRESS_C	1
#define HOLDING_STATE_C		2
#define VC_ACTIVE_C		3
#define	CAME_DESTACP_C		4	/* This <macaddr, color> => ACP		*/
#define	CAME_LOOPTST_C		5	/* This <macaddr, color> => send	*/
					/* on vc 6 for a loop test		*/

/*
**	Pacing counter, default value 3
*/

#define	PACE_COUNT_C			3

#ifdef LANGUAGE_C

typedef struct cam_entry {

	UINT8	came_macstate;				/* state associated with dest mac	*/
	UINT8	came_pacing_counter;		/* pacing counter to bus vc 		*/
	UINT16	came_index;					/* index into the hardware cam		*/
	UINT16	came_vpc;					/* If active, then VPC is valid     */
	UINT16	came_pad;					/* To make CAM entry size power of 2*/

} tCAM_ENTRY, *tpCAM_ENTRY;

extern tpCAM_ENTRY  pLocalCamTable;		/*array of CAM_TABLE_SIZE in TX spram*/

#else 	/* assembly */

        .data
        .struct 0

came_macstate_8s:		.byte 0
came_pacing_counter_8s:		.byte 0
came_hwindex_16s:			.half 0		/* index into the hardware cam		*/
came_vpc_16s:				.half 0		/* If active, then VPC is valid     */
came_pad_16s:				.half 0		/* To make CAM entry size power of 2*/
came_size_s:

        .text                           /* reestablish the default      	*/

#endif  /* language */



/*
**
**	P R O T O T Y P E S
**
*/


#ifdef LANGUAGE_C
/*
**	cam_init:  Initialize cams
*/
extern UINT32  cam_init(void);


/*
**	cam_create_entry:  create cam entry in local cam.  This is a synchronous
**	(blocking call).  Returns a pointer into the CAM table entry.  The 
**	cam table is maintained in TX SPRAM
**	
*/
extern tpCAM_ENTRY  cam_create_entry(tpMACADDR  pDstMac, 
				     tCOLOR	color, 
				     UINT32	cam_state );

/*
**	cam_clear_entry:  clear cam entry in cam.  This is a synchronous
**	(blocking call)
*/
extern UINT32  cam_clear_entry (tpCAM_ENTRY 	pCamEntry,
				tpMACADDR	pDstMac,
				tCOLOR		color );

/*
**	cam_get_state: returns the cam state and pointer to entry.
**	(blocking call)
*/
extern tpCAM_ENTRY  cam_get_state(tpMACADDR	pDstMac, 
				  tCOLOR	color, 
				  UINT32	*pState );


/*
**	cam_set_state:  Set cam entry state. This is a synchronous.
**	(blocking call)
*/
extern UINT32  cam_set_state( tpMACADDR      pDstMac,
			     tCOLOR         color,
			     UINT32 cam_state,
			     UINT16 vcd);


/*
**	cam_set_counter:  Set the pacing counter to the specified value.
**	(blocking call)
*/
extern UINT32  cam_set_counter(void  *pCamEntry, UINT32 count);


/*
**	cam_getnext_entry:  Get next valid CAM entry. If pCamEntry is null, the
**	first CAM entry is returned.
*/
extern tpCAM_ENTRY  cam_getnext_entry(tpCAM_ENTRY	pCamEntry); 

#endif /* LANGUAGE_C */



#endif /* _CAMIF_H_ */
