/* $Id: ntc_dmabuf.c,v 1.1.4.1 1996/05/09 14:55:04 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/ntc_dmabuf.c,v $
 *------------------------------------------------------------------
 * ntc_dmabuf.c -- encapsulates the NTC DMA buffer.  Since the ACP
 *                 must access this buffer in non-cacheable space,
 *                 we define it here (in cacheable BSS section) and
 *                 return a pointer to it that is in non-cacheable
 *                 space.
 *
 * April 1995, sakumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ntc_dmabuf.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:04  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:19  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:31  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:00  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/10/04  01:30:54  kchristi
 * Fix CVS Endlog directives so that cvs-fix-merge script will work.
 *
 * Revision 1.1  1995/04/24  17:00:37  sakumar
 * New file. Inits Channel 0 DMA buffer.
 *
*
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *
 *  FILE: ntc_dmabuf.c
 *
 *  CONTENTS:
 *
 *  File encapsulates the NTC DMA buffer.  Since the ACP must access this buffer
 *	in non-cacheable space, we define it here (in cacheable BSS section) and return
 *	a pointer to it that is in non-cacheable space.
 *
 *  OWNERS:
 *      Ratinder P. S. Ahuja
 */
 

/*
**	INCLUDES
*/

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif

#include	"acp_basic_types.h"
#include	"acp_hw_defs.h"
#include 	"acp_c_defs.h"
#include	"ntc_if.h"

/*
**	GLOBALS
*/

/*
**	STATIC
*/

/* DMA buffers	*/
static tNTC_DMA_CH0_BLK ntc_dma_ch0_blk;  /* Area of DRAM into which NTC will DMA Channel 0 Parms */


/*
**	PROTOTYPES
*/


/* --------------------------------------------------------------------	*/
/*	    This routine initializes the NTC DMA area and returns a pointer	*/
/*		to it that is in non-cacheable ACP DRAM space.					*/
/* --------------------------------------------------------------------	*/
tpNTC_DMA_CH0_BLK
NTC_InitDmaBuf ()
{
	tpNTC_DMA_CH0_BLK	p ;


	p = ((tpNTC_DMA_CH0_BLK) (ACP_HW_NONCACHE_ADR ((& ntc_dma_ch0_blk)))) ;

	memset ((u_int8 *) p, 0, sizeof (*p)) ;

	return (p) ;

}	/* NTC_InitDmaBuf */

