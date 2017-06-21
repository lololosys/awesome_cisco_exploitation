/* $Id: diagmain.c,v 1.1.4.1 1996/05/09 15:01:00 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/diagmain.c,v $
 *------------------------------------------------------------------
 * diagmain.c -- procedures related to "Main Routine" that are
 *               included as part of CALYPSO Diagnostics.
 *
 * December 1994, rlb (adapted to ATM line card)
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: diagmain.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:00  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:57  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:52  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:27  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.6.2.2.4.1  1996/01/18  05:02:19  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.6.2.2  1995/11/20  22:56:17  rbau
 * send Module_Ready message to NMP and mark ALC in off-line mode
 *
 * Revision 1.6.2.1  1995/11/15  23:07:27  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.8  1995/11/15  21:14:54  rbau
 * changed to download off-line diags sar code
 *
 * Revision 1.7  1995/11/08  00:35:15  rbau
 * get slot number from LCP driver
 *
 * Revision 1.6  1995/10/16  23:35:32  rbau
 * clean up ifdefs. completed file header with cvs keywords.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include	"version.h"
#include	"types.h"
#include	"constant.h"
#include	"struct.h"
#include	"clock.h"
#include	"globdef.h"
#include	"localdef.h"
#include	"diag.h"
#include	"eeprom.h"
#include	"nvram.h"

#include	"hw.h"
#include	"errcode.h"
#include	"acp_hw_defs.h"

#define   ATM   /* modified for ATM Line card */

/*#define	DIAG_VERSION	0 */

/*********************************************************************
    Global data.
*********************************************************************/
int	macCAMStatus[4];
char    *monthsInfo[] =
        { "", "Jan", "Feb", "Mar", "Apr", "May", "Jun",
              "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

extern char	cli_print_buf[];
char		*cli_display_buf_ptr[];
char		*diag_error_info[];

struct TEST_TOC		memdiag_toc[];
struct TEST_TOC		iodiag_toc[];

uInt16		lcpcntl1_shadow;
uInt16		lcpcntl2_shadow;
int		diag_test_mode;

uInt16 boot_sys_status = 0;
uInt16 sys_status = 0;
uInt16 power_status = 0;

uInt16 dmp_sys_status = 0;
uInt16 dmp_mac_status = 0;
uInt16 dmp_biga_status = 0;
uInt16 dmp_eti_status[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
uInt16 dmp_sw_ver_hi = 0;
uInt16 dmp_sw_ver_lo = 0;
uInt16 biga_status = 0;

	uChar		sys_mac_base[6];
#ifndef BOOT
	uChar		sys_source_fddi_mac[6];
	uChar		sys_dest_fddi_mac[6];
#endif /* BOOT */
	uChar		sys_source_ethernet_mac[9][6];
	uChar		sys_dest_ethernet_mac[9][6];

	uInt32		_randx;
	int		diagExecution;

	struct realtime	sys_clock;

	char	sys_date[20] = "YY/MM/DD";
	char	sys_time[20] = "HH:MM:SS";
	char	sys_title[] = "ATM Control Processor(ACP) Diagnostics";

	char	sys_cpr1[80] = "copyright (C) Cisco Systems Inc.";
	char	sys_cpr2[] = "1994-1996";

#ifndef BOOT
	char		sys_oem_string[OEM_STR_SIZE+1];
	char		sys_model[MODEL_STR_SIZE+1];
	uInt32		sys_serial_no;
	uInt16		sys_hw_version_high, sys_hw_version_low;
#endif /* BOOT */

	char		sys_specific_info[80];

	Int32		sys_error_code;
	uInt32	sys_fail_address;
	uInt32	sys_data_expected;
	uInt32	sys_data_read;
	int		sys_clock_display;
	int		sys_clock_update;
	int		sys_more_prompt;
	int		sys_cmd_no_flag;
	int		sys_term_vt100;
	int		sys_help_sort;
	int		ABCardPresent;
#ifndef BOOT
	int		sys_eeprom_valid;
#endif /* BOOT */

	int		cntl_reg_1_shadow;
	int		cntl_reg_2_shadow;

	char		test_in_progress_msg[] = 
	" Testing @%08lX ... {%s} -> %lu [%s]\n";

	char		NMPCompileDate[16];
	char		NMPCompileTime[16];
	char		DMPCompileDate[16];
	char		DMPCompileTime[16];

extern   uChar  MySlotNum ;    /* ATM card slot number */

/*---< Prototypes >---*/
void		display_nmp_status();
void		display_dmp_status();
void		init_test_params();

int diag_main(int, char **);
extern	 void	Notify_NMP_DiagMode(void);


/* --------------------------------------------------------------------	*/
/*	DM_SetupDevices							*/
/*		Initialize device drivers used by diags.		*/
/* --------------------------------------------------------------------	*/
void DM_SetupDevices (unsigned char fCmd)
{
	if (! Bt2_SetupOffLineSarDiags (fCmd))
	{
		printf
("\nSetup Offline Sar code failed! Will not be able to run loop tests\n");
	}

	if (! BDG_PUD_Init ())
	{
		printf
("\nPUD_Init failed! Will not be able to run loop tests\n");
	}

	/*
	 * send Module_Ready message to NMP, and mark ALC is in off-line mode
	 */

	Notify_NMP_DiagMode ();

}	/* DM_SetupDevices */

/*---------------------------------------------------------------*
 * diag_main()
 *---------------------------------------------------------------*/
int diag_main(
	int		argc,
	char		*argv[])
{
	int		rc, s, i, j, power_on, ex_flag;
	uInt32		hour, minute, second;
	uInt32		year, month, day, yr;
#ifndef BOOT
	uChar		c;
	uInt32		diag_verbosity_save;
extern	uInt32		diag_verbosity;
extern	struct eeprom_data eeprom_data_r;
extern	char		*monthsInfo[];
	struct logNVRAM	 *lognvram = (struct logNVRAM *) LOGNVRAM_START;
#endif /* BOOT */
	uChar		fFastBoot ;

   if (*(uChar *)MFP_BASE & MFG_JUMPER_MASK)
      return;      /* jumper not in, skip this diag */

	s = spl7();

	fFastBoot  = * (volatile uChar *) (ACP_HW_IOREG_BASE + ACP_RDRG_DEBUG) ;
	fFastBoot &= (1 << ACP_RRB_JMP_FASTBOOT_) ;
	fFastBoot  = (! fFastBoot) ;

	sys_init();
	get_clock(&sys_clock);

	if (fFastBoot)
	{
		printf ("\nOffline diags in FASTBOOT mode, NO driver inits.\n") ;
	}
	else	/* Not in FASTBOOT override mode, allow driver inits. */
	{
		DM_SetupDevices (FALSE) ;
	}

    MySlotNum = LCP_PhysSlotNbr();
	year = sys_clock.year;
	month = sys_clock.month;
	day = sys_clock.date;
	hour = sys_clock.hour;
	minute = sys_clock.minute;
	second = sys_clock.second;
	if (year < 91)
	   yr = 2000 + year;
	else
	   yr = 1900 + year;

	sprintf(sys_date, "%02u/%02u/%02u", year, month, day);
	sprintf(sys_time, "%02u:%02u:%02u", hour, minute, second);


	sys_term_vt100 = FALSE;
	sys_cmd_no_flag = TRUE;
	sys_more_prompt = FALSE;


#ifdef BOOT
	power_on = FALSE;
	while (1) {
	   rc = cli(power_on);
	}
#else /* !BOOT */

	power_on = FALSE;
	diagExecution = TRUE;


   printf("ACP CLI process\n");

	while (1) {
	   rc = cli(power_on);
	   if ((rc==CLI_EXIT) || power_on )
	      break;
	}

	power_on = TRUE; ex_flag = FALSE;
/*	rc = power_up_tests(power_on, ex_flag);  */

	spl7();

	dtimer_term();

	sys_term();

	/*  out put to front panel LED for ACP & SAR status */
#endif /* BOOT */
	
	diagExecution = FALSE;

	splx(s);


	return(rc);
}

/*--------------------------------------------------------------
 * Version()
 *--------------------------------------------------------------*/
int Version(
	int	argc,
	char	*argv[])
{
	register int	rc;

	if (argc > 1)
	   return(SYNTAX_UNEXPECTED_ARGUMENT_ERROR);

	sprintf(NMPCompileDate, "%s", __DATE__);
	sprintf(NMPCompileTime, "%s", __TIME__);

#ifdef BOOT
	sprintf(cli_print_buf, 
	        "\n ACP Boot Diagnostics Version %u.%u, Dated %s %s",
	        BOOTHIVERSION, BOOTLOVERSION, NMPCompileDate, NMPCompileTime);
	put_line(cli_print_buf);
#else /* !BOOT */
	sprintf(cli_print_buf, 
	        "\n ACP Diagnostics Version %u.%u, Dated %s %s",
	        ACPHIVERSION, ACPLOVERSION, NMPCompileDate, NMPCompileTime);
	put_line(cli_print_buf);
#endif /* BOOT */

#ifndef BOOT
	if (sys_eeprom_valid) {
	   sprintf(cli_print_buf, "\n Model %s, Board Serial# %lu, H/W Rev %u.%u",
	           sys_model, sys_serial_no, sys_hw_version_high, sys_hw_version_low);
	   put_line(cli_print_buf);
	}
#endif /* BOOT */

	sprintf(cli_print_buf, "\n %s  %s\n\n", sys_cpr1, sys_cpr2);
	put_line(cli_print_buf);

	return(rc = 0);
}

int Errorinfo(
	int		argc,
	char		*argv[])
{
	register int	rc, i;
	register char	*argp;
	uInt32		err;
	int		buf_index;

	for (buf_index = 0, i = 1; i < argc || argc == 1; i++) {
	   if (argc > 1) {
	      argp = argv[i];
	      if (*argp++ != '-')
	         return(SYNTAX_INVALID_ARGUMENT_DELIMITER_ERROR);
	      if (*argp++ != 'c')
	         return(SYNTAX_INVALID_ARGUMENT_OPTION_ERROR);
	      if (*argp == '\0') {
	         if (i == (argc-1))
	            return(SYNTAX_ERROR_CODE_NOT_SPECIFIED_ERROR);
	         argp = argv[++i];
	      }
	   }

	   if (argc == 1) {
	      err = i;
	      if (diag_error_info[err-1][0] == '\0')
	         break;
	   }
	   else {
	      if (!sscanf_x(argp, fmt_uInt32, &err))
	         return(SYNTAX_INVALID_ERROR_CODE_ERROR);
	      if (err < 1 || err > NUMBER_OF_ERROR_CODES)
	         return(SYNTAX_ERROR_CODE_OUT_OF_RANGE_ERROR);
	   }
	   sprintf(cli_display_buf_ptr[buf_index++], "%4u %s\n", err, 
	           diag_error_info[err-1]);
	   if (argc > 1)
	      strcpy(cli_display_buf_ptr[buf_index++], "-");
	}

	print_buf(cli_display_buf_ptr, buf_index);

	return(rc = 0);
}

#ifndef BOOT
int bringupDMP()
{
	int		i, s;

#ifndef  ATM
	dmp_sw_ver_hi = *((uChar *) COMM_DMP_VERSION);
	dmp_sw_ver_lo = *((uChar *) COMM_DMP_REVISION);

	*((volatile uChar *) COMM_UART_TXCHAR) = 0xed; /* should release DMP */
	for (i = 0; i < 100; i++);

	s = spl0();
	while (*((volatile uChar *) COMM_UART_RXCHAR) != 0xfe);
	splx(s);

	clearCounters();
#endif  /* ATM */
}

#define	DMP_RESPONSE_TIME	30000
/*---------------------------------------------------------
 * power_up_tests()
 *---------------------------------------------------------*/
int power_up_tests(
	int		power_on,
	int		ex_flag)
{
extern	uInt32		diag_verbosity, diag_current_runcount;
extern	uInt32		diag_maxerrorcount;
/*extern	char		diag_npbusarbitration[];*/
extern	char		diag_fbbusarbitration[];
extern	char		memdiag_databus_test[], memdiag_addrbus_test[];
extern	char		memdiag_cell_test[];
extern	int		diag_virtual_uart_mode;
extern	uInt32		diag_runcount, diag_total_errorcount;
extern	volatile uInt32	system_tick_time;

	register int	rc, i;
	uChar		c;
	uInt32		runcount;
	uInt32		start_time;
	uInt32		addr;
	struct logNVRAM	*lognvram = (struct logNVRAM *) LOGNVRAM_START;

	uInt32		diag_verbosity_save, diag_maxerrorcount_save;
	char		memdiag_cell_test_save[6];

	diag_verbosity_save = diag_verbosity;
	diag_verbosity = VERBOSITY_LEVEL0;
	diag_maxerrorcount_save = diag_maxerrorcount;
	diag_maxerrorcount = 99999999;
	diag_total_errorcount = 0;

#ifndef  ATM
	strcpy(memdiag_cell_test_save, memdiag_cell_test);
	strcpy(memdiag_cell_test, value_flag_false);

	diag_test_mode = TRUE;
	for (rc = 0, runcount = 1; runcount <= diag_runcount; runcount++) {

	   Version(0, 0);

	   diag_current_runcount = runcount;

	   sys_status = 0x0000;
	   power_status = 0x0000;
	   for (i = 0; i < 9; i++) {
	      eti_status[i] = 0x0000;
	   }

	   for (i = 0; i <= 7 && rc != BREAK_ALL_TESTS; i++) {
	      if (i == 1 || i == 3 || i == 4 || i == 6 || i == 7) {
	         init_test_params(&memdiag_toc[i], 0);
	         if ((rc = memdiag_toc[i].procedure(ex_flag))==BREAK_ALL_TESTS)
	            break; 
	         if (sys_error_code) {
	            switch (i) {
	               case 1: sys_status |= EEPROM_BAD; break;
	               case 3: sys_status |= DMP_FLASH_BAD; break;
	               case 4: 
	                  if (bootarea->clearConfig != MAGIC_PATTERN)
	                     sys_status |= NVRAM_BAD; 
	                  else
	                     bootarea->clearConfig = 0;
	                  break;
	               case 7: sys_status |= QUEUE_SRAM_BAD; break;
	               case 8: sys_status |= FRAME_BUF_BAD; break;
	            }
	            if (diag_verbosity >= VERBOSITY_LEVEL2)
	               display_test_failure(sys_error_code);
	         }
	      }
	   }

#if  0  /* RLB DBG */ 

	   for (i = 0; i <= 3 && rc != BREAK_ALL_TESTS; i++) {
	      if (i == 2 || i == 3) {
	         init_test_params(&iodiag_toc[i], 99999999);
	         if ((rc = iodiag_toc[i].procedure(ex_flag)) == BREAK_ALL_TESTS)
	            break; 
	         if (sys_error_code) {
	            if (i == 2)
	               sys_status |= TEMPFAN_BAD;
	            else
	               sys_status |= POWER_BAD;
	            if (diag_verbosity >= VERBOSITY_LEVEL2)
	               display_test_failure(sys_error_code);
	         }
	      }
	   }

	   for (i = 0; i <= 1 && rc != BREAK_ALL_TESTS; i++) {
	      init_test_params(&phydiag_toc[i], 99999999);
	      if ((rc = phydiag_toc[i].procedure(ex_flag)) == BREAK_ALL_TESTS)
	         break; 
	      if (sys_error_code) {
	         if (i == 0)
	            sys_status |= FDDI_PHY_BAD;
	         else
	            if (ABCardPresent)
	               sys_status |= FDDI_PHY_BAD;
	         if (diag_verbosity >= VERBOSITY_LEVEL2)
	            display_test_failure(sys_error_code);
	      }
	   }

	   for (i = 0; i <= 5 && rc != BREAK_ALL_TESTS; i++) {
	      init_test_params(&macdiag_toc[i], 0);
	      if ((rc = macdiag_toc[i].procedure(ex_flag)) == BREAK_ALL_TESTS)
	         break; 
	      if (sys_error_code) {
	         if (i <= 3)
	            sys_status |= FDDI_MAC_BAD;
	         else
	            if (ABCardPresent)
	               sys_status |= FDDI_MAC_BAD;
	         if (diag_verbosity >= VERBOSITY_LEVEL2)
	            display_test_failure(sys_error_code);
	      }
	   }

	   for (i = 0; i <= 2 && rc != BREAK_ALL_TESTS; i++) {
	      init_test_params(&etidiag_toc[i], 99999999);
	      if ((rc = etidiag_toc[i].procedure(ex_flag)) == BREAK_ALL_TESTS)
	         break; 
	      if (sys_error_code) {
	         sys_status |= ETHER_BAD;
	         if (diag_verbosity >= VERBOSITY_LEVEL2)
	            display_test_failure(sys_error_code);
	      }
	   }
#endif   /* RLB DBG */

	   if (rc == BREAK_ALL_TESTS) {
	      sys_status |= NMP_STATUS_UNKNOWN;
	      if (!power_on)
	         break;
	   }

	   if (diag_verbosity >= VERBOSITY_LEVEL2 || !power_on)
	      display_nmp_status();

	   if (power_on) {

	      dmp_sys_status = 0x0000;
	      dmp_mac_status = 0x0000;
	      dmp_biga_status = 0x0000;
	      for (i = 0; i < 8; i++) {
	         dmp_eti_status[i] = 0x0000;
	      }

	      /* Doing this Reset causes an exception */

              *((uInt32 *) COMM_STATUS) |= COMM_STATUS_UART_VIRTUAL;
              c = *((volatile uChar *) COMM_UART_RXCHAR) = 0x00;
              *((volatile uChar *) COMM_UART_TXCHAR) = 0x00;

              *(uChar *)CNTL_REG_1 = SAR_RESET_MASK;   /* reset TX SAR */
              *(uChar *)CNTL_REG_2 = SAR_RESET_MASK;   /* reset RX SAR */

	      if (rc == BREAK_ALL_TESTS) {
                 *((uInt32 *) COMM_STATUS) |= COMM_STATUS_BYPASS_TESTS;
	         sys_status |= DMP_STATUS_UNKNOWN;
	         break;
	      }

	      diag_test_mode = FALSE;
              diag_virtual_uart_mode = TRUE; start_time = system_tick_time;
              while (1) {

	         c = '0' + diag_verbosity;
	         *((volatile uChar *) COMM_UART_TXCHAR) = c;
	         *((volatile uInt32 *) COMM_STATUS) |= COMM_STATUS_UART_TXSTAT;

	         for (start_time = system_tick_time; 
	             (system_tick_time - start_time) < DMP_RESPONSE_TIME; ) {
                    if (*((volatile uInt32 *)COMM_STATUS) &
                                                      COMM_STATUS_UART_RXSTAT) {
                       c = *((volatile uChar *) COMM_UART_RXCHAR);
                       *((volatile uInt32 *) COMM_STATUS) &=
                                                       ~COMM_STATUS_UART_RXSTAT;
                       if (c == (uChar) 0xfe)
                          break;
                       putchar_p(c);
                    }
	            if (dgetch(&c)) {
	               if (!(*((volatile uInt32 *) COMM_STATUS) & 
	                                             COMM_STATUS_UART_TXSTAT)) {
	                  *((volatile uChar *) COMM_UART_TXCHAR) = c;
	                  *((volatile uInt32 *) COMM_STATUS) |= 
	                                                COMM_STATUS_UART_TXSTAT;
	               }
	            }
	            /*if (diag_npbusarbitration[0] == 'T') {
	               for (addr = QSRAM_START_ARBITRATE; 
	                    addr < QSRAM_END_ARBITRATE; addr += 4) {
	                  *((volatile uInt32 *) addr) = addr;
	                  if (*((volatile uInt32 *) addr) != addr)
	                     sys_status |= QUEUE_SRAM_BAD;
	               }
	            }*/
	            if (diag_fbbusarbitration[0] == 'T') {
	               for (addr = FRAMEBUF_START_ARBITRATE; 
	                    addr < FRAMEBUF_END_ARBITRATE; addr += 4) {
	                  *((volatile uInt32 *) addr) = addr;
	                  if (*((volatile uInt32 *) addr) != addr)
	                     sys_status |= FRAME_BUF_BAD;
	               }
	            }
                 }   

                 if (c == (uChar) 0xfe) {
	            strcpy(DMPCompileDate, (char *) COMM_DMP_COMPILEDATE);
	            strcpy(DMPCompileTime, (char *) COMM_DMP_COMPILETIME);
                    dmp_sys_status = *((uInt16 *) COMM_DMP_SYS_STATUS);
                    dmp_mac_status = *((uInt16 *) COMM_DMP_MAC_STATUS);
                    dmp_biga_status = *((uInt16 *) COMM_DMP_BIGA_STATUS);
                    dmp_eti_status[0] = *((uInt16 *) COMM_DMP_ETI_1_STATUS);
                    dmp_eti_status[1] = *((uInt16 *) COMM_DMP_ETI_2_STATUS);
                    dmp_eti_status[2] = *((uInt16 *) COMM_DMP_ETI_3_STATUS);
                    dmp_eti_status[3] = *((uInt16 *) COMM_DMP_ETI_4_STATUS);
                    dmp_eti_status[4] = *((uInt16 *) COMM_DMP_ETI_5_STATUS);
                    dmp_eti_status[5] = *((uInt16 *) COMM_DMP_ETI_6_STATUS);
                    dmp_eti_status[6] = *((uInt16 *) COMM_DMP_ETI_7_STATUS);
                    dmp_eti_status[7] = *((uInt16 *) COMM_DMP_ETI_8_STATUS);
	            sys_status |= dmp_sys_status;
                    break;
                 }  
                 else {
	            put_line("\nDMP Diagnostics TIMEOUT, Status Unknown ...\n");
                    sys_status |= DMP_STATUS_UNKNOWN;
	            if (*((uInt32 *) COMM_STATUS)&COMM_STATUS_DMP_FLASH_FAILURE)
	               sys_status |= DMP_FLASH_BAD;
	            if (*((uInt32 *)COMM_STATUS) & COMM_STATUS_DMP_SRAM_FAILURE)
	               sys_status |= DMP_SRAM_BAD;
	            if (*((uInt32 *)COMM_STATUS) & COMM_STATUS_DMP_DRAM_FAILURE)
	               sys_status |= DMP_DRAM_BAD;
	            if (*((uInt32 *)COMM_STATUS) & COMM_STATUS_DMP_COMM_FAILURE)
	               sys_status |= DMP_COMM_BAD;
                    break;
                 }
              }     
              diag_virtual_uart_mode = FALSE;
 
              if (diag_verbosity >= VERBOSITY_LEVEL2)
                 display_dmp_status();

	      *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_UART_VIRTUAL;

	      break;
	   }
	}
	diag_test_mode = FALSE;

	diag_verbosity = diag_verbosity_save;
	diag_maxerrorcount = diag_maxerrorcount_save;

	strcpy(memdiag_cell_test, memdiag_cell_test_save);

	if (!power_on)
	   *((uInt32 *) COMM_STATUS) &= ~COMM_STATUS_POWER_UP_MODE;
	else {
	   if (sys_status & DMP_FLASH_BAD)
	      lognvram->dmp_flash_cksum_failures++;
	   if (sys_status & DMP_SRAM_BAD)
	      lognvram->dmp_sram_failures++;
	   if (sys_status & DMP_DRAM_BAD)
	      lognvram->dmp_dram_failures++;
	   if (!ABCardPresent && (dmp_mac_status & MAC_EXTERNAL_FAIL) &&
	                         (dmp_biga_status & BIGA_EXTERNAL_FAIL) &&
	                         (dmp_sys_status & FDDI_MAC_BAD) &&
	                         (dmp_sys_status & FDDI_BIGA_BAD)) {
	      dmp_sys_status &= ~(FDDI_MAC_BAD | FDDI_BIGA_BAD);
	      sys_status &= ~(FDDI_MAC_BAD | FDDI_BIGA_BAD);
	   }
	}
#endif  /* ATM */

	return(rc);
}

int online_tests()
{
extern	uInt32		diag_verbosity;
extern	uInt32		diag_maxerrorcount;
extern	uInt32		diag_total_errorcount;

	register int	rc, i, ex_flag;
	uInt32		diag_verbosity_save, diag_maxerrorcount_save;

	ex_flag = FALSE;

	diag_verbosity_save = diag_verbosity;
	diag_verbosity = VERBOSITY_LEVEL0;
	diag_maxerrorcount_save = diag_maxerrorcount;
	diag_maxerrorcount = 99999999;
	diag_total_errorcount = 0;

	diag_test_mode = TRUE;

	cntl_reg_1_shadow = lcpcntl1_shadow;
	cntl_reg_2_shadow = lcpcntl2_shadow;
	
	Version(0, 0);

	for (i = 0; i <= 8; i++) {
	   if (i == 0 || i == 1 || i == 2 || i == 4) {
	      init_test_params(&memdiag_toc[i], 0);
	      if ((rc = memdiag_toc[i].procedure(ex_flag)) || 
	                sys_error_code) {
	         switch (i) {
	            case 0: boot_sys_status |= BOOTROM_BAD; break;
	            case 1: sys_status |= EEPROM_BAD; break;
	            case 2: boot_sys_status |= FLASH_BAD; break;
	            case 4: sys_status |= NVRAM_BAD; break;
	         }
	         if (diag_verbosity >= VERBOSITY_LEVEL2)
	            display_test_failure(sys_error_code);
	      }
	      else {
	         switch (i) {
	            case 0: boot_sys_status &= ~BOOTROM_BAD; break;
	            case 1: sys_status &= ~EEPROM_BAD; break;
	            case 2: boot_sys_status &= ~FLASH_BAD; break;
	            case 4: sys_status &= ~NVRAM_BAD; break;
	         }
	      }
	   }
	}

	for (i = 0; i <= 3; i++) {
	   if (i == 2 || i == 3) {
	      init_test_params(&iodiag_toc[i], 99999999);
	      if ((rc = iodiag_toc[i].procedure(ex_flag)) || 
	                sys_error_code) {
	         if (i == 2)
	            sys_status |= TEMPFAN_BAD;
	         else
	            sys_status |= POWER_BAD;
	         if (diag_verbosity >= VERBOSITY_LEVEL2)
	            display_test_failure(sys_error_code);
	      }
	   }
	   else {
	      if (i == 2)
	         sys_status &= ~TEMPFAN_BAD;
	      else
	         sys_status &= ~POWER_BAD;
	   }
	}
	diag_test_mode = FALSE;

	if (boot_sys_status || (sys_status & ~MINOR_ALARM))
	   put_line("*** On-line Diagnostics Failed ***\n");
	else
	   put_line("On-line Diagnostics Completed Successfully ...\n");

	diag_verbosity = diag_verbosity_save;
	diag_maxerrorcount = diag_maxerrorcount_save;

	lcpcntl1_shadow = cntl_reg_1_shadow;
	lcpcntl2_shadow = cntl_reg_2_shadow;
	
	return(rc);
}


/*--------------------------------------------------------------------
 * disp_nmp_status()
 *--------------------------------------------------------------------*/
void display_nmp_status()
{
#ifndef ATM 
	extern	int	macCAMStatus[4];
#endif  /* ATM */

	strcpy(cli_print_buf, "\nATM Control Processor (ACP) Status: (. = Pass, F = Fail, U = Unknown)\n");
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "  ROM:  %c   RAM:  .   DUART: .   Flash-EEPROM: %c   Ser-EEPROM: %c   NVRAM: %c\n",
	       (boot_sys_status & BOOTROM_BAD) ? 'F' : '.',
	       (boot_sys_status & FLASH_BAD) ? 'F' : '.',
	       (sys_status & NMP_STATUS_UNKNOWN) ? 'U' : (sys_status & EEPROM_BAD) ? 'F' : '.',
	       (sys_status & NMP_STATUS_UNKNOWN) ? 'U' : (sys_status & NVRAM_BAD) ? 'F' : '.');
	put_line(cli_print_buf);

	strcpy(cli_print_buf, "\n  Switch Memory Status:\n");
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "    RAM: %c    Cache-SRAM: %c\n",
	       (sys_status & NMP_STATUS_UNKNOWN) ? 'U' : (sys_status & FRAME_BUF_BAD) ? 'F' : '.',
	       (sys_status & NMP_STATUS_UNKNOWN) ? 'U' : (sys_status & QUEUE_SRAM_BAD) ? 'F' : '.');
	put_line(cli_print_buf);

	strcpy(cli_print_buf, "\n  FDDI Status:\n");
	put_line(cli_print_buf);

}

/*----------------------------------------------------------------------------*
 * disp_dmp_status()
 *----------------------------------------------------------------------------*/
void display_dmp_status()
{
	strcpy(cli_print_buf, "\nData Movement Processor (DMP) Status: (. = Pass, F = Fail, U = Unknown)\n");
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "  Flash-EEPROM: %c   RAM: %c   SRAM: %c   COMM: %c\n",
	       (sys_status & DMP_FLASH_BAD) ? 'F' : (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : '.',
	       (sys_status & DMP_DRAM_BAD) ? 'F' : (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : '.',
	       (sys_status & DMP_SRAM_BAD) ? 'F' : (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : '.',
	       (sys_status & DMP_COMM_BAD) ? 'F' : (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : '.');
	put_line(cli_print_buf);

	strcpy(cli_print_buf, "\n  Switch Memory Status:\n");
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "     RAM: %c    Cache-SRAM: %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_sys_status & FRAME_BUF_BAD) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_sys_status & QUEUE_SRAM_BAD) ? 'F' : '.');
	put_line(cli_print_buf);

	strcpy(cli_print_buf, "\n  FDDI Status:\n");
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "    MAC Access:             %c   MAC Buffer R/W:         %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_mac_status & MAC_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_mac_status & MAC_BUF_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "    MAC Internal LB:        %c   MAC External LB:        %c   LoadGen: %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_mac_status & MAC_INTLB_FAIL) ? 'F' : '.',
	       ((sys_status & DMP_STATUS_UNKNOWN) || !ABCardPresent) ? 'U' : 
	       (dmp_mac_status & MAC_EXTLB_FAIL) ? 'F' : '.',
	       ((sys_status & DMP_STATUS_UNKNOWN) || !ABCardPresent) ? 'U' : 
	       (dmp_mac_status & MAC_LOADGEN_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "    BIGA Access:            %c   BIGA->MAC Buffer R/W:   %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_biga_status & BIGA_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_biga_status & BIGA_MAC_BUF_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "    BIGA->MAC TxDMA:        %c   BIGA<-MAC RxDMA:        %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_biga_status & BIGA_TXDMA_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_biga_status & BIGA_RXDMA_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "    BIGA<->MAC Internal LB: %c   BIGA<->MAC External LB: %c   LoadGen: %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_biga_status & BIGA_INTLB_FAIL) ? 'F' : '.',
	       ((sys_status & DMP_STATUS_UNKNOWN) || !ABCardPresent) ? 'U' : 
	       (dmp_biga_status & BIGA_EXTLB_FAIL) ? 'F' : '.',
	       ((sys_status & DMP_STATUS_UNKNOWN) || !ABCardPresent) ? 'U' : 
	       (dmp_biga_status & BIGA_LOADGEN_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);

	strcpy(cli_print_buf, "\n  Ethernet Port Status:      3     4     5     6     7     8     9     10    \n");
	put_line(cli_print_buf);

	strcpy(cli_print_buf, "  --------------------------------------------------------------------------\n");
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "   Accessibility             %c     %c     %c     %c     %c     %c     %c     %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[0] & ETI_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[1] & ETI_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[2] & ETI_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[3] & ETI_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[4] & ETI_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[5] & ETI_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[6] & ETI_ACC_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[7] & ETI_ACC_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "   CAM Access                %c     %c     %c     %c     %c     %c     %c     %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[0] & ETI_CAM_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[1] & ETI_CAM_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[2] & ETI_CAM_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[3] & ETI_CAM_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[4] & ETI_CAM_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[5] & ETI_CAM_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[6] & ETI_CAM_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[7] & ETI_CAM_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);

	sprintf(cli_print_buf, "   Loopback                  %c     %c     %c     %c     %c     %c     %c     %c\n",
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[0] & ETI_LB_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[1] & ETI_LB_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[2] & ETI_LB_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[3] & ETI_LB_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[4] & ETI_LB_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[5] & ETI_LB_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[6] & ETI_LB_FAIL) ? 'F' : '.',
	       (sys_status & DMP_STATUS_UNKNOWN) ? 'U' : (dmp_eti_status[7] & ETI_LB_FAIL) ? 'F' : '.');
	put_line(cli_print_buf);
}

void init_test_params(
	struct TEST_TOC	*toc,
	uInt32		maxerrorcount)
{
extern	char		*diag_test_name, *diag_test_id;
extern	uInt32		diag_test_errorcount, diag_sect_maxerrorcount;
extern	uInt32		diag_verbosity;

	diag_test_name = toc->name;
	diag_test_id = toc->id;
	sys_error_code = 0;
	diag_test_errorcount = 0; diag_sect_maxerrorcount = maxerrorcount;
	if (diag_verbosity >= VERBOSITY_LEVEL2)
	   display_test_progress();
}
#endif /* BOOT */

int set_source_mac(
	uChar		base_mac[])
{
	int		i, j;

	for (i = 0; i < 6; i++) {
#ifndef BOOT
	   sys_source_fddi_mac[i] = base_mac[i];
#endif /* BOOT */
	   for (j = 0; j < 9; j++) {
	      sys_source_ethernet_mac[j][i] = base_mac[i];
	   }
	}
#ifndef BOOT
	bitswapcopy(sys_source_fddi_mac, sys_source_fddi_mac, 6);
#endif /* BOOT */
	for (j = 0; j < 9; j++) {
	   *((uInt32 *) &sys_source_ethernet_mac[j][2]) += (j+1);
	}

	return(0);
}

int set_dest_mac(
	uChar		base_mac[])
{
	int		i, j;

	for (i = 0; i < 6; i++) {
#ifndef BOOT
	   sys_dest_fddi_mac[i] = base_mac[i];
#endif /* BOOT */
	   for (j = 0; j < 9; j++) {
	      sys_dest_ethernet_mac[j][i] = base_mac[i];
	   }
	}
#ifndef BOOT
	bitswapcopy(sys_dest_fddi_mac, sys_dest_fddi_mac, 6);
#endif /* BOOT */
	for (j = 0; j < 9; j++) {
	   *((uInt32 *) &sys_dest_ethernet_mac[j][2]) += (j+1);
	}

	return(0);
}

#ifndef BOOT
int reset_source_mac()
{
	int		i, j;

	for (i = 0; i < 6; i++) {
	   sys_source_fddi_mac[i] = sys_mac_base[i];
	   for (j = 0; j < 9; j++) {
	      sys_source_ethernet_mac[j][i] = sys_mac_base[i];
	   }
	}
	bitswapcopy(sys_source_fddi_mac, sys_source_fddi_mac, 6);
	for (j = 0; j < 9; j++) {
	   *((uInt32 *) &sys_source_ethernet_mac[j][2]) += (j+1);
	}

	return(0);
}

int reset_dest_mac()
{
	int		i, j;

	for (i = 0; i < 6; i++) {
	   sys_dest_fddi_mac[i] = sys_mac_base[i];
	   for (j = 0; j < 9; j++) {
	      sys_dest_ethernet_mac[j][i] = sys_mac_base[i];
	   }
	}
	bitswapcopy(sys_dest_fddi_mac, sys_dest_fddi_mac, 6);
	for (j = 0; j < 9; j++) {
	   *((uInt32 *) &sys_dest_ethernet_mac[j][2]) += (j+1);
	}

	return(0);
}

int show_source_mac()
{
	int		i;
	uChar		sys_base_mac[6];

	for (i = 0; i < 6; i++)
	   sys_base_mac[i] = sys_source_fddi_mac[i];
	bitswapcopy(sys_base_mac, sys_base_mac, 6);

	sprintf(cli_print_buf, 
	        " Source MAC Address Base: %02X:%02X:%02X:%02X:%02X:%02X (%02X:%02X:%02X:%02X:%02X:%02X)\n",
	        sys_base_mac[0], sys_base_mac[1], sys_base_mac[2],
	        sys_base_mac[3], sys_base_mac[4], sys_base_mac[5],
	        sys_source_fddi_mac[0], sys_source_fddi_mac[1], 
	        sys_source_fddi_mac[2], sys_source_fddi_mac[3],
	        sys_source_fddi_mac[4], sys_source_fddi_mac[5]);
	put_line(cli_print_buf);

	return(0);
}

int show_dest_mac()
{
	int		i;
	uChar		sys_base_mac[6];

	for (i = 0; i < 6; i++)
	   sys_base_mac[i] = sys_dest_fddi_mac[i];
	bitswapcopy(sys_base_mac, sys_base_mac, 6);

	sprintf(cli_print_buf, 
	        " Destination MAC Address Base: %02X:%02X:%02X:%02X:%02X:%02X (%02X:%02X:%02X:%02X:%02X:%02X)\n",
	        sys_base_mac[0], sys_base_mac[1], sys_base_mac[2],
	        sys_base_mac[3], sys_base_mac[4], sys_base_mac[5],
	        sys_dest_fddi_mac[0], sys_dest_fddi_mac[1], 
	        sys_dest_fddi_mac[2], sys_dest_fddi_mac[3],
	        sys_dest_fddi_mac[4], sys_dest_fddi_mac[5]);
	put_line(cli_print_buf);

	return(0);
}
#endif /* BOOT */

/*-------------------------------------------------------------------*/
