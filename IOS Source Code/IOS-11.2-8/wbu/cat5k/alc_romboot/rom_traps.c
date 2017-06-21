/* $Id: rom_traps.c,v 1.1.4.1 1996/05/09 15:00:02 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_romboot/rom_traps.c,v $
 *-----------------------------------------------------------------
 * rom_traps.c --  C code exception handlers for ROM-based 68K-family Boots.
 *
 * November 30, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rom_traps.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:02  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:58  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:41:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:07  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:31  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:11  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* --------------------------------------------------------------------	*/
/*	C code exception handlers for ROM-based 68K-family Boots.	*/
/*									*/
/*	Modification History (Synergy):					*/
/*		based on Catalyst, "@(#)traps.c       1.1 6/24/93 "	*/
/*									*/
/*	11/30/94  RKL -	Grabbed relevant Catalyst code for Synergy	*/
/*			baseline.  Modified same for Synergy ACP Boot.	*/
/* --------------------------------------------------------------------	*/

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "lo_error.h"
#include "boot_defs.h"
#include "bt_nvram.h"
#include "bt_globvar.h"
#include "acp_c_defs.h"
#include "libc.h"


#if	0
void c_error_hdlr ()
{
	tAcpBtNvram *	pNvr = (tAcpBtNvram *) (BOOT_NVRAM_AREA) ;
	u_int32		base ;
	s_int8		c ;
	s_int16		h ;
	s_int16		i ;


	PUNCH_WD ;
	(void) A_BlockCopy (bt_exception_info, pNvr->NvExceptInfo,
			    BOOT_EXCP_DATA_SZ, AR_SvcWatchdog) ;

	++ (pNvr->NvExceptCount) ;


	PUNCH_WD ;
	sprintf (cli_print_buf,
"\nException occurred ... PC = %08X, Status = %04X, Vector = %04X",
		 bt_exception_pc, bt_exception_status, bt_exception_vector);
	printf (cli_print_buf);

	for (i = 0; i < 96; i += 4 )
	{
	    if (! (i % 32))
	    {
		sprintf (cli_print_buf, "\nsp+%02X: ", i);
		printf (cli_print_buf);
	    }

	    PUNCH_WD ;
	    sprintf (cli_print_buf, "%08X ", bt_exception_info [i/4]);
	    printf (cli_print_buf);
	}

	for (h = 0 ; h < 2 ; ++ h)
	{
	    if (h)
	    {
		c    = 'A' ;
		base = 128 / 4 ;
	    }
	    else
	    {
		c    = 'D' ;
		base = 96 / 4 ;
	    }

	    for (i = 0 ; i < 8 ; i += 4)
	    {
		PUNCH_WD ;
		sprintf (cli_print_buf,
			 "\n%c%1d: %08X, %c%1d: %08X, %c%1d: %08X, %c%1d: %08X",
			 c, i,   bt_exception_info [base+i],
			 c, i+1, bt_exception_info [base+i+1], 
			 c, i+2, bt_exception_info [base+1+2],
			 c, i+3, bt_exception_info [base+i+3]);

		printf (cli_print_buf);
	    }
	}

	PUNCH_WD ;
	sprintf (cli_print_buf,
		 "\nVBR: %08X  ISP: %08X  MSP: %08X  USP: %08X  CACR: %08X",
		 bt_exception_info [160/4], bt_exception_info [164/4],
		 bt_exception_info [168/4], bt_exception_info [172/4],
		 bt_exception_info [176/4]) ;

	printf (cli_print_buf);

	sprintf (cli_print_buf, "\n") ;
	printf (cli_print_buf);


		/* Flip LEDs until someone notices.  Do NOT return !! */
	PUNCH_WD ;
	while (1)
		A_FailLeds (LOER_STD_LED_FLASH, PUD_BT_EXCEPT,
			    0xffffffff, AR_SvcWatchdog) ;

}	/* c_error_hdlr */
#else
void c_error_hdlr ()
{
	tAcpBtNvram *	pNvr = (tAcpBtNvram *) (BOOT_NVRAM_AREA) ;
	u_int32		base ;
	s_int8		c ;
	s_int16		h ;
	s_int16		i ;


	PUNCH_WD ;
	(void) A_BlockCopy (bt_exception_info, pNvr->NvExceptInfo,
			    BOOT_EXCP_DATA_SZ, AR_SvcWatchdog) ;

	++ (pNvr->NvExceptCount) ;


	PUNCH_WD ;
	printf ( "\nException occurred ... PC = %08X, Status = %04X, Vector = %04X",
		 bt_exception_pc, bt_exception_status, bt_exception_vector);

	for (i = 0; i < 96; i += 4 )
	{
	    if (! (i % 32))
	    {
		printf ("\nsp+%02X: ", i);
	    }

	    PUNCH_WD ;
	    printf ("%08X ", bt_exception_info [i/4]);
	}

	for (h = 0 ; h < 2 ; ++ h)
	{
	    if (h)
	    {
		c    = 'A' ;
		base = 128 / 4 ;
	    }
	    else
	    {
		c    = 'D' ;
		base = 96 / 4 ;
	    }

	    for (i = 0 ; i < 8 ; i += 4)
	    {
		PUNCH_WD ;
		printf ( "\n%c%1d: %08X, %c%1d: %08X, %c%1d: %08X, %c%1d: %08X",
			 c, i,   bt_exception_info [base+i],
			 c, i+1, bt_exception_info [base+i+1], 
			 c, i+2, bt_exception_info [base+1+2],
			 c, i+3, bt_exception_info [base+i+3]);

	    }
	}

	PUNCH_WD ;
	printf ( "\nVBR: %08X  ISP: %08X  MSP: %08X  USP: %08X  CACR: %08X",
		 bt_exception_info [160/4], bt_exception_info [164/4],
		 bt_exception_info [168/4], bt_exception_info [172/4],
		 bt_exception_info [176/4]) ;

	printf ("\n") ;


		/* Flip LEDs until someone notices.  Do NOT return !! */
	PUNCH_WD ;
	while (1)
		A_FailLeds (LOER_STD_LED_FLASH, PUD_BT_EXCEPT,
			    0xffffffff, AR_SvcWatchdog) ;

}	/* c_error_hdlr */
#endif
