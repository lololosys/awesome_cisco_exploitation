/* $Id: sysinit.c,v 1.1.4.1 1996/05/09 15:01:17 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/sysinit.c,v $
 *------------------------------------------------------------------
 * sysinit.c -- System Initialization for diagnostics
 *
 * December 1994, rbau
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysinit.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:17  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:17:16  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:25  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:12  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:44  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.2  1995/10/16  23:34:03  rbau
 * clean up some ifdefs. completed the file header with cvs keywords.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include	"types.h"
#include	"constant.h"
#include	"localdef.h"
#include	"nvram.h"
#include	"hw.h"
#include	"mfp.h"
#include	"clock.h"


#define error_hdlr	_error_hdlr
#define te_hdlr  		_te_hdlr
#define tbe_hdlr		_tbe_hdlr
#define re_hdlr		_re_hdlr
#define rbf_hdlr		_rbf_hdlr

extern	void	error_hdlr(), dtimer_A_hdlr();
#ifndef SCC_NEW
extern	void	te_hdlr(), tbe_hdlr();
extern	void	re_hdlr(), rbf_hdlr();
#endif /* SCC_NEW */

extern	uInt32	exception_pc;
extern	uInt16	exception_status, exception_vector;
extern	uInt16	exception_info[80];
extern	uInt16	exception_stack_info[82];
extern	char	cli_print_buf[];
extern	int	cntl_reg_1_shadow;

#define	LCPEXEC	0x01100000
#define	LCPDRAM	0x40000000

uInt32	vector_table_save[256];


/*--------------------------------------------------------------
 * init_vector_table()
 *--------------------------------------------------------------*/
void init_vector_table()
{
	int		i;
	VECTOR_TABLE	*vector;

}

#define TIMER_D_COUNT 2

/*--------------------------------------------------------------
 * mfp_dinit()
 *--------------------------------------------------------------*/
void mfp_dinit()
{
	uChar *mfp;

/*
* disable int sources while we play around
*/
	mfp = (uChar *) (MFP_BASE + MFP_IERA );
	*mfp = 0;
	mfp = (uChar *) (MFP_BASE + MFP_IERB );
	*mfp = 0;

	mfp = (uChar *) (MFP_BASE + MFP_AER);
	*mfp = 0;						/* falling edge interrupts */

	/*
		Clear gpdr.
	*/
	mfp = (uChar *) (MFP_BASE + MFP_GPDR);
	*mfp = 0;

	/*
		Tell chip we will do vectored interrupts.
	*/
	mfp = (uChar *)(MFP_BASE + MFP_VR);
	*mfp = MFP_BASE_VECTOR;

	/*
		Initialize Timer D as the baud rate generator.
	*/
	mfp = (uChar *) (MFP_BASE + MFP_IERB);
	*mfp &= ~EN_TIMER_D;			/* Disable timer D interrupts. 	*/
	mfp = (uChar *) (MFP_BASE + MFP_IMRB);
	*mfp &= ~EN_TIMER_D;			/* Mask interrupts on timer D	*/
	/*
		Set count and delay mode.
	*/
	mfp = (uChar *) (MFP_BASE + MFP_TDDR);
	*mfp = TIMER_D_COUNT;
	mfp = (uChar *) (MFP_BASE + MFP_TCDCR);
	*mfp = TIMER_DELAY_MODE_4;
/*
* clear all inservice and pending bits 
*/
	mfp = (uChar *) ( MFP_BASE + MFP_ISRA );
	*mfp = 0;
	mfp = (uChar *) ( MFP_BASE + MFP_ISRB );
	*mfp = 0;
	mfp = (uChar *) ( MFP_BASE + MFP_IPRA );
	*mfp = 0;
	mfp = (uChar *) ( MFP_BASE + MFP_IPRB );
	*mfp = 0;
/*
* clear all mask bits; turn them on as needed
*/
	mfp = (uChar *) ( MFP_BASE + MFP_IMRA );
	*mfp = 0;
	mfp = (uChar *) ( MFP_BASE + MFP_IMRB );
	*mfp = 0;
/*
* turn all enable bits on
*/

	mfp = (uChar *) ( MFP_BASE + MFP_IERA );
	*mfp = 0xff;
	mfp = (uChar *) ( MFP_BASE + MFP_IERB );
	*mfp = 0xff;
}

/*----------------------------------------------------------------
 * sys_init()
 *----------------------------------------------------------------*/
void sys_init()
{
 	int		i;

#if  0  /* the following for CALYPSO, rlb */
	init_vector_table();

	mfp_dinit();

#ifndef SCC_NEW
	scc_init(0);
#else /* SCC_NEW */
	new_scc_init(0);
#endif /* SCC_NEW */

#endif /* 0 CALYPSO */

        *(char *)CNTL_REG_0 = 1;     /* release 8031 reset */
        *(char *)CNTL_REG_3 = 1;     /* release biga reset */
        *(char *)CNTL_REG_5 = 1;     /* release NTC reset */


	for (i = 0; i < 1000; i++);

   /* init SAR comm. status here if any */
}


/*-----------------------------------------------------------
 * sys_term()
 *----------------------------------------------------------*/
void sys_term()
{
	int		i;
	VECTOR_TABLE	*vector;

}

void dbcopy(
	char	*from,
	char	*to,
	short	len)
{
	for(; len > 0; *to++ = *from++, len--);
}

#ifndef DIAGTEST
void c_test_PHY_intr()
{
}

void c_test_MAC_intr()
{
}
#endif /* DIAGTEST */

/*---------------------------------------------------------
 */
int reset_nmp()
{
  /* Get Stack & Reset Entry Point from Vector Table */
  asm(" movew  #0x2700,sr");
  while (1);
  /*asm(" movel #0x00000000,a0");
  asm(" movel a0@+,sp");
  asm(" movel a0@,a0");
  asm(" jmp   a0@");*/
}

/*-------------------------------------------------------------
 * reset_dmp()
 *------------------------------------------------------------*/
int reset_dmp()
{
   reset_txsar();
   reset_rxsar();
}

/*-------------------------------------------------------------
 * reset_txsar()
 *------------------------------------------------------------*/
int reset_txsar()
{
   *(char *)CNTL_REG_1 &= ~SAR_RESET_MASK;
   *(char *)CNTL_REG_1 |= SAR_RESET_MASK;
}

/*-------------------------------------------------------------
 * reset_rxsar()
 *------------------------------------------------------------*/
int reset_rxsar()
{
   *(char *)CNTL_REG_2 &= ~SAR_RESET_MASK;
   *(char *)CNTL_REG_2 |= SAR_RESET_MASK;
}


/*-----------------------------------------------------------*/
