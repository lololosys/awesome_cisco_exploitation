/* $Id: lec_if.h,v 1.1.4.2 1996/06/18 16:44:32 sakumar Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_cominc/lec_if.h,v $
 *-----------------------------------------------------------------
 * lec_if.h -- lec interface file.  
 *
 * Ratinder P. S. Ahuja
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lec_if.h,v $
 * Revision 1.1.4.2  1996/06/18  16:44:32  sakumar
 * CSCdi60668:  PVC (RFC1483) support needs to be added to Cat5K ATM Module
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/09  14:54:33  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:13:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:42  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:01  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:40:59  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:29  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *------------------------------------------------------------------
 * lec_if.h -- lec interface file.  
 *
 *------------------------------------------------------------------
 *
 *  FILE:  lec_if.h
 *
 *  CONTENTS:
 *
 *  This file describes the data structures and interactions between the
 *	sars and the lec entity.  
 *
 *  OWNER:
 *
 *      Ratinder P. S. Ahuja
 *
*/
 
#ifndef _LECIF_H_
#define _LECIF_H_

#define ALC_PVC_ENCAP_MASK_C  0xf000	/* These bits are used for PVC encap
                                           mode in lece_busvpc field below */
#define ALC_PVC_LLC_ENCAP_C   0x1000	/* Use 1483 LLC SNAP bridge encoding */
#define ALC_PVC_VCMUX_ENCAP_C 0x2000    /* Use 1483 VC MUX encoding */
	
 
 
#ifdef LANGUAGE_C

    /*
    ** If the VLAN is supported by PVC(s) then this PVC info struct 
    ** will be used. If supported by LANE then not used.
    */
typedef struct pvc_info {
    struct pvc_info* pvc_next; /* next PVC for this VLAN (in TSAR SPRAM) */ 
    UINT16    pvc_vlan;		/* The VLAN number to which this PVC belongs */
    tVPC      pvc_vpc;      /* Active bits of VPI/VCI for this PVC       */
} tPVC_info, *tpPVC_info;

	/*
	**	The lec entry has the lecid and bus vpc for the lec.  A MAX_COLOR
	**	array of lec entries is kept in the txspram.  The array is indexed
	**	by the vlanid (color)
	*/
 
typedef struct lece {						/* lec entry				*/

	tLECID	   lece_lecid;					/* id for le client			*/
	tVPC	   lece_busvpc;					/* BUS vc for this lec		*/
    tpPVC_info lece_pvc;				    /* ptr to PVC struct        */

} tLEC_ENTRY, *tpLEC_ENTRY;


#else /* assembly */

        .data
        .struct 0

pvc_next_s:				.word 0             /* ptr to next PVC struct   */
pvc_vlan_16s:			.half 0				/* 2 byte vlan number id	*/
pvc_vpc_16s:		    .half 0				/* 2 byte PVC vpc			*/


        .data
        .struct 0

lece_lecid_16s:			.half 0				/* 2 byte lec id			*/
lece_busvpc_16s:		.half 0				/* 2 byte BUS vpc			*/
lece_pvc_s:				.word 0             /* ptr to PVC struct        */

        .text           					/* reestablish the default 	*/ 

#endif  /* language_c */



#endif /* _LECIF_H_ */

