/* $Id: sysdiag.c,v 1.1.4.1 1996/05/09 15:01:16 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/sysdiag.c,v $
 *------------------------------------------------------------------
 * sysdiag.c -- ATM system diagnostic test running on the ACP
 *
 * 1995, ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysdiag.c,v $
 * Revision 1.1.4.1  1996/05/09  15:01:16  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/03  05:20:41  rlowe
 * Fix startup diags so that local loop tests always use offline SAR
 * images, while syndiags loop test always uses run-time SAR images.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:17:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.2  1996/04/26  05:51:33  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:13:24  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:47  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:54:11  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:43  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.19.2.3.6.1  1996/03/05  02:32:15  rlowe
 * Fold in fixes from mfg_repair_branch.
 *
 * Revision 1.19.2.3  1995/12/08  00:00:56  rlowe
 * Only assert ACP_InDiagMode flag, not ACP_FLASH_CODE flag, in Module Rdy.
 *
 * Revision 1.19.2.2  1995/11/20  23:00:34  rbau
 * add function to send out Moudle_Ready message to NMP $ mark ALC in off-line
 *
 * Revision 1.19.2.1  1995/11/16  00:15:59  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.21  1995/11/16  00:12:38  rbau
 * took out Tx SAR cam registry for DVT loopback test
 *
 * Revision 1.20  1995/11/08  00:31:58  rbau
 * added support for DVT test.
 *
 * Revision 1.19  1995/10/13  18:13:20  rbau
 * added cvs keywords in the header. clean up some ifdefs. changed loop5 to some
 * sensible name
 *
 * Revision 1.17  1995/09/27  18:32:53  rbau
 * fixed the race condition between NMP and ACP when NMP starts loopback test
 * on ATM card. set status LED to yellow when system diag OK, otherwise, set it
 * to red.
 *
 * Revision 1.16  1995/09/14  02:09:18  rbau
 * moved rd_pmd_type function to pmd_eeprom.c in drivers/pmc/pmd_eeprom.c
 *
 * Revision 1.15  1995/09/12  19:03:40  rbau
 * moved reset BIGA after nmpdiag_looptst. put sars in reset before reset BIAG
 *
 * Revision 1.14  1995/09/09  02:10:07  rbau
 * corrected the checking for PMD_NTCa(was PMD_NTC)
 *
 * Revision 1.13  1995/09/09  00:24:18  rbau
 * put some debug messages in #ifdef condition. check pmd type before setting
 * PMD in loopback mode
 *
 * Revision 1.12  1995/09/08  18:40:21  rbau
 * added wrapper function to read PMD type
 *
 * Revision 1.11  1995/09/08  00:34:15  rlowe
 * More fixes relating to doing LCP driver init and system loopbacks.
 *
 * Revision 1.10  1995/09/07  12:09:02  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.9  1995/08/27  23:57:20  rbau
 * added code to support SYNDIAG on NMP. ifdef'd out debug messages
 *
 * Revision 1.8  1995/08/11  01:58:02  rbau
 * added PMD infos in the diagnostic results
 *
 * Revision 1.7  1995/08/08  02:48:56  rbau
 * reset BIGA after big loop test
 *
 * Revision 1.6  1995/08/04  00:21:01  rbau
 * ifdef'd out the PMD info 'till code merges with new scp_msgs.h
 *
 * Revision 1.5  1995/08/02  18:44:38  rbau
 * filled PMD serial eeprom info in the DIAG result.
 *
 * Revision 1.4  1995/07/20  23:47:21  rbau
 * put sage in default setup before each loopback test. more code clean-up
 *
 * Revision 1.3  1995/07/19  01:00:15  rbau
 * pick up loopback diag results in variable ALC_DiagStatus
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*---< Includes >---*/
#include  "acp_hw_defs.h"
#include  "acp_basic_types.h"
#include  "basic_types.h"
#include  "sar_if.h"
#include  "atmdrv_if.h"
#include  "../alc_drivers/ntc_if.h"
#include  "atmdiag.h"
#include  "acp_c_defs.h"
#include  "scp_msgs.h"
#include  "syn_ser_eeprom.h"
#include  "diag_results.h"
#include  "errcode.h"
#include  "lcp_api.h"
#include  "lcp_if.h"
#include  "bdg_if.h"


/*-------------------------------------------------------------------*
 * struct  SYSTEST_TOC{}
 *-------------------------------------------------------------------*/
typedef struct _SYSTEST_TOC
{
      char     *description;
      char     *name;
      int      (*procedure)();
      int      result; 
} SYSTEST_TOC, *pSYSTEST_TOC;


/*---< Globals >---*/
UINT32			testcase;
UINT32			testresult;
DiagResults_t	DiagResults;

dvt_param_t  dvt_test_param = {0,0,1,1};

int 	pmd_loopmode=1;     /* pmd loopback mode */

/*
 * dst. mac address for loopback test:00:02:03:04:05:06
 */
extern char diag_dmac[];

extern char *diag_error_info[];
extern uChar  nmptest_srcmac[];


/*---< Prototypes >---*/
int		loop1_test();
int		loop2_test();
int		loop3_test();
int		loop4_test();
int		nmpdiag_setup();
int		nmpdiag_looptest();
int		set_ver_in_diagresults(char *, int);
void	RunSysDiag(SYSTEST_TOC *);
int 	DVT_Test();
int 	LpbkSetup();
void	set_pmd_loopmode(int);
void	Notify_NMP_DiagMode(void);
extern int LCP_EchoTest(void);
extern int   loop3_core(void);
extern int   loop4_core(void);
extern int   nmploop_core(void);
extern char *strchr(char *, char);


/*--------------------------------------------------------------------*
 * LOOP1: ACP -> DP_RAM -> LCP -> DP_RAM ->ACP
 *--------------------------------------------------------------------*/
char	loop1_description[] = "ACP -> DP_RAM -> LCP -> DP_RAM ->ACP";

/*
 * loop1 results variables 
 */
char		loop1_time_of_run[18];
Int32		loop1_error_code;
uInt32	loop1_run_count;
uInt32	loop1_test_fail_count;
uInt32	loop1_error_count;


SYSTEST_TOC  loop1_toc =
{ loop1_description, "LOOP1", loop1_test};

/*--------------------------------------------------------------------*
 * LOOP2: ACP -> TxSAR -> PMD -> RxSAR ->ACP
 *-------------------------------------------------------------------*/
char		loop2_description[] =
			"ACP -> TxSAR -> PMD -> RxSAR -> ACP";

/*
 * loop2 results variables 
 */
char     loop2_time_of_run[18];
Int32    loop2_error_code;
uInt32   loop2_run_count;
uInt32   loop2_test_fail_count;
uInt32   loop2_error_count;

SYSTEST_TOC  loop2_toc =
	{ loop2_description, "LOOP2", loop2_test};

/*--------------------------------------------------------------------*
 * LOOP3: ACP -> RxSAR -> BIGA -> Pkt_Buffer ->LCP -> ACP
 *--------------------------------------------------------------------*/
char		loop3_description[] =
			"ACP ->RxSAR ->BIGA ->Pkt_Buffer ->LCP ->ACP";

/*
 * loop3 results variables 
 */
char     loop3_time_of_run[18];
Int32    loop3_error_code;
uInt32   loop3_run_count;
uInt32   loop3_test_fail_count;
uInt32   loop3_error_count;


SYSTEST_TOC  loop3_toc =
   { loop3_description, "LOOP3", loop3_test};

/*--------------------------------------------------------------------*
 * LOOP4: ACP -> DP_RAM -> LCP -> Pkt_Buffer -> BIGA ->
 *        -> TxSAR -> TxSAR CAM -> PMD -> RxSAR-> ACP
 *--------------------------------------------------------------------*/
char		loop4_description[] =
"ACP ->LCP ->Pkt_Buffer ->BIGA ->TxSAR ->CAM ->PMD ->RxSAR ->ACP";

/*
 * loop4 results variables 
 */
char     loop4_time_of_run[18];
Int32    loop4_error_code;
uInt32   loop4_run_count;
uInt32   loop4_test_fail_count;
uInt32   loop4_error_count;

SYSTEST_TOC  loop4_toc =
{ loop4_description, "LOOP4", loop4_test};


/*--------------------------------------------------------------------*
 * ALC_SysDiag() 
 *--------------------------------------------------------------------*/
void
ALC_SysDiag(int run_looptest, p_void (*pBCS) (void))
{
   char		*preg;
	uInt32	rc;
   ALCSTATUS   *pALC_diagstatus = (ALCSTATUS *)&DiagResults.alc_diag_results;
   PMDSERPROM  *pPMD_eeprom = (PMDSERPROM *)&DiagResults.pmd_info;

   preg = (char *)ACP_HW_IOREG_BASE;

	/* 
	 * set card status LED to yellow for Off-line
	 */
	set_led(ACP_WRRG_PMD_STAT_LED, ACP_PMD_STAT_LED_YEL);

   /*
	* Diag Result AcpErrStat2
	*/
	rc = eeprom_checksum();
	if (rc)
		pALC_diagstatus->AcpErrStat2 |= ACP_PMD_EEPROM;   /* error */
	else
	{
		pALC_diagstatus->AcpErrStat2 &= ~ACP_PMD_EEPROM;  /* ok */

		/*
		 * read in PMD serial eeprom info
		 */
		rd_eeprom_c(0, ADDITIONAL_INFO_SIZE/2, (uInt16 *)pPMD_eeprom);
	}

  /*
	* Diag Result AcpErrStat3
	*/
	if (run_looptest)
	{
		RunSysDiag(&loop1_toc);
		pALC_diagstatus->AcpErrStat3 |= (loop1_toc.result ? ACP_SAR_LOOPBK1: 0);
		SageOff_synbus();

		/* 
		 * the following three tests need to have SAR run-time code
		 * and the atmdriver support
		 */
		RunSysDiag(&loop2_toc);
		pALC_diagstatus->AcpErrStat3 |= (loop2_toc.result ? ACP_SAR_LOOPBK2: 0);

		RunSysDiag(&loop3_toc);
		pALC_diagstatus->AcpErrStat3 |= (loop3_toc.result ? ACP_SAR_LOOPBK3: 0);
		SageOff_synbus();

		RunSysDiag(&loop4_toc);
		pALC_diagstatus->AcpErrStat3 |= (loop4_toc.result ? ACP_SAR_LOOPBK4: 0);
		SageOff_synbus();

#ifdef  DEBUG
		printf("\nALC_SysDiag: len = 0x%x\n", sizeof(DiagResults_t) );

		printf("\nALC_SysDiag: ALC_DiagStatus.AcpErrStat1 = 0x%x\n\n",
			pALC_diagstatus->AcpErrStat1);

		printf("\nALC_sysdiag: ALC_DiagStatus.AcpErrStat2 = 0x%x\n\n",
			pALC_diagstatus->AcpErrStat2);

		printf("\nALC_SysDiag: ALC_DiagStatus.AcpErrStat3 = 0x%x\n\n",
			pALC_diagstatus->AcpErrStat3);
#endif  /* DEBUG */

		/*
		 * need to turn off NTC loopback unless NTC
		 * is re-init'd by IOS module 
		 */ 

	}

	/* Setup now for nmp system loopback testing, to avoid race */
	/* conditions after sending Module Ready to nmp.            */

	/* Load the SARs with normal run-time code, and re-init ATM driver. */
	/* Do this before sending Module Ready since we don't know how long */
	/* this will take.  Note return code for success checks below.      */
	rc = (! BDG_SAR_InitRT (pBCS)) ;

	/* Send Module Ready to nmp. */
#ifdef  DEBUG
	printf ("%s: LCP_DriverInit_3 \n", __FILE__);
#endif  /* DEBUG */
	LCP_DriverInit_3 (FALSE, TRUE, TRUE, sizeof(DiagResults),
				(u_int8 *)&DiagResults);

	/* Assume we got LCP message to set NMP MAC.  If there is a failure */
	/* setting up for loopback, do NOT attempt it.  This will lead NMP  */
	/* to time out, and we will NOT go on-line in LCP_DriverInit_5.     */
	/* Set RED Status LED in any case other than success !!             */
	do {
		/* Loop test itself will check for system loopback */
		/* being disabled or being told to go on-line now. */

	    if (! rc)
		if (! nmpdiag_setup ())
			if (! nmpdiag_looptest ())
				if (! (pALC_diagstatus->AcpErrStat3))
					break ;

		/* We are in trouble ! */
		set_led(ACP_WRRG_PMD_STAT_LED, ACP_PMD_STAT_LED_RED);

	} while (FALSE) ;


   /* Ensure LCP driver gets to go on-line state. */
   /* We hang here (forever, as intended) if that does NOT happen. */
#ifdef  DEBUG 
   printf ("%s: LCP_DriverInit_5 \n", __FILE__);
#endif  /* DEBUG */
   LCP_DriverInit_5 (FALSE, TRUE) ;

	/* make sure  Tx/Rx SARs are in reset */
	BDG_ResetFrBufUsr (ACP_WRRG_TSAR_RESET_) ;
	BDG_ResetFrBufUsr (ACP_WRRG_RSAR_RESET_) ;

	/*
	 * reset BIGA
	 */
	BDG_ResetFrBufUsr (ACP_WRRG_BIGA_RESET_) ;
	BDG_ClearReset (ACP_WRRG_BIGA_RESET_) ;

} /* ALC_SysDiag() */ 

/*--------------------------------------------------------------------*
 * RunSysDiag() 
 *--------------------------------------------------------------------*/
void
RunSysDiag(SYSTEST_TOC *toc)
{
	printf("\n%s: %s \n", toc->name, toc->description);
	toc->result = toc->procedure(); 
}

/*--------------------------------------------------------------------*
 * SD_DumpSarBigaInfo()
 *    Dump counter, pointer output from SARs and BIGA.
 *--------------------------------------------------------------------*/
void
SD_DumpSarBigaInfo ()
{
        UINT32  R_head, R_tmphead, R_tail ;
        UINT32  T_head, T_tmphead, T_tail ;


        READ_BIGA32 (TTMPH, T_tmphead) ;
        READ_BIGA32 (THEAD, T_head) ;
        READ_BIGA32 (TTAIL, T_tail);

        READ_BIGA32 (RTMPH, R_tmphead) ;
        READ_BIGA32 (RHEAD, R_head) ;
        READ_BIGA32 (RTAIL, R_tail);


#ifdef        DEBUG
        printf ("\nBIGA  T-TmpHead $%08x,  T-Head $%08x,  T-Tail $%08x\n",
                T_tmphead, T_head, T_tail) ;

        printf ("BIGA  R-TmpHead $%08x,  R-Head $%08x,  R-Tail $%08x\n\n",
                R_tmphead, R_head, R_tail) ;

        show_stats () ;
        printf ("\n") ;
#endif

}     /* SD_DumpSarBigaInfo */

/*--------------------------------------------------------------------*
 * loop1_test() 
 *--------------------------------------------------------------------*/
int
loop1_test()
{
	int   rc;
	char  *pSAR_reset;

   rc = LCP_EchoTest();

#ifdef  DEBUG
   if (!rc)
      printf("Loop1 test passed !\n");
	else if (rc > 0)
      printf("Loop1: err_code = %d %s\n", rc, diag_error_info[rc-1]);

#endif  /* DEBUG */

	printf("\n");
	SD_DumpSarBigaInfo () ;

	return(rc);
}

/*--------------------------------------------------------------------*
 * loop2_test() 
 *--------------------------------------------------------------------*/
int
loop2_test()
{
	int   rc = 0;

	/* use atm_drv_init() provided by BDG_PUDInit() */

#ifdef  DEBUG
	printf("PMD Type = %d  \n", LCP_PmdType());
#endif  /* DEBUG */

	/* if pmd_loop_mode set, set pmd for internal loopback */
	set_pmd_loopmode(pmd_loopmode);

   rc = AtmSysDiagMain();

#ifdef  DEBUG
   if (!rc)
      printf("Loop2 test passed !\n");
	else if (rc > 0)
      printf("Loop2: err_code = %d %s\n", rc, diag_error_info[rc-1]);

#endif  /* DEBUG */

	printf("\n");
	SD_DumpSarBigaInfo () ;

	return(rc);
}

/*--------------------------------------------------------------------*
 * loop3_test() 
 *--------------------------------------------------------------------*/
int
loop3_test()
{
	int   rc = 0;

	SageOff_synbus();

   rc = loop3_core();

#ifdef  DEBUG
   if (!rc)
      printf("Loop3 test passed !\n");
	else if (rc > 0)
      printf("Loop3: err_code = %d %s\n", rc, diag_error_info[rc-1]);
#endif  /* DEBUG */

	printf("\n");
	SD_DumpSarBigaInfo () ;

	return(rc);
}

/*--------------------------------------------------------------------*
 * loop4_test() 
 *--------------------------------------------------------------------*/
int
loop4_test()
{
	int   rc = 0;

	tpCAM_ENTRY pCame = NULLP;              /* mac adddress for acp  */
    tCOLOR      acp_color = SYSTST_COLOR;   /* 0x0 */

	SageOff_synbus();
	
	/*
	 *  Add a CAM entry for the hosts mac address
	 *  The state is set to indicate that the destination is ACP 
	 */
    if ( (pCame = cam_create_entry((tMACADDR *)diag_dmac, acp_color,
                             (UINT32)CAME_LOOPTST_C)) == NULLP)
    {
#ifdef  DEBUG
        printf("\nALC_SYSDIAG CAM test cam_create_entry failed\n");
#endif  /* DEBUG */
		  return(-1);   /* failed to add entry, can't continue loopback test */
    }

	rc = loop4_core();

#ifdef  DEBUG 
   if (!rc)
      printf("Loop4 test passed !\n");
 else if (rc > 0)
      printf("Loop4: err_code = %d %s\n", rc, diag_error_info[rc-1]);

#endif  /* DEBUG */


	if(cam_clear_entry(NULLP, (tMACADDR *)diag_dmac, acp_color) != ACP_OK)
		printf("\n\r atmcam_clear: failed for big loop CAM entry\n");

	printf("\n");
	SD_DumpSarBigaInfo () ;

	return(rc);
}


/*--------------------------------------------------------------------*
 * set_ver_in_diagresults() 
 *    fill in software version infos in the diag results
 *--------------------------------------------------------------------*/
int
set_ver_in_diagresults(char *str, int index)
{
	char 			*cp;
	int 			hi_ver, low_ver;
	ALCSTATUS	*pALC_diagstatus = (ALCSTATUS *)&DiagResults.alc_diag_results;

	if (index > RXSAR_sw_ver)
		return(-1);				/* no such version defined yet */

#define  FREE_FMT
#ifdef  FREE_FMT
	low_ver = 0;
	hi_ver = 0;		

	if (strlen(str))
	{  /* check if in dotted format */

		sscanf(str, "%d", &hi_ver);

		cp = (char *)strchr(str, '.');
		if (cp)
		{
			sscanf(cp+1, "%d", &low_ver);
		}
	}
	else
		return(-1);

#else  /* !FREE_FMT */
	sscanf(str, "%d%*c%d", &hi_ver, &low_ver);
#endif  /* FREE_FMT */

	switch(index)
	{
		case  ACP_boot1_ver:
			pALC_diagstatus->ACPBootHiVersion = hi_ver;
			pALC_diagstatus->ACPBootLoVersion = low_ver;
			break;

		case	ACP_boot2_ver: 
			pALC_diagstatus->ACPBoot2HiVersion = hi_ver;
			pALC_diagstatus->ACPBoot2LoVersion = low_ver;
			break;

		case	ACP_IOS_boot_ver:
			pALC_diagstatus->ACPIosBootHiVersion = hi_ver;
			pALC_diagstatus->ACPIosBootLoVersion = low_ver;
			break;

		case	ACP_sw_ver:
			pALC_diagstatus->ACPSwHiVersion = hi_ver;
			pALC_diagstatus->ACPSwLoVersion = low_ver;
			break;

		case	TXSAR_sw_ver:
			pALC_diagstatus->TxSARSwHiVersion = hi_ver;
			pALC_diagstatus->TxSARSwLoVersion = low_ver;
			break;

		case	RXSAR_sw_ver:
			pALC_diagstatus->RxSARSwHiVersion = hi_ver;
			pALC_diagstatus->RxSARSwLoVersion = low_ver;
			break;

		default:
			break;
	}
	return (0);
}  /* set_ver_in_diagresults() */

/*--------------------------------------------------------------------*
 * nmpdiag_setup()
 *    set up connection to RxSAR for intercepting test packet from
 *    NMP Syndiag.
 *--------------------------------------------------------------------*/
int
nmpdiag_setup()
{
   int   rc = 0;
   tVPC  vpc = SYSTST_VPC;                 /* 0x6 */

   tpCAM_ENTRY pCame = NULLP;              /* mac adddress for acp  */
   tCOLOR      acp_color = SYSTST_COLOR;   /* 0x0 */

   if (LCP_NMP_MacAddr((tLCP_MacAddrU *) &nmptest_mac) == (tLCP_MacAddrU *)0 )
	{
#ifdef  DEBUG
      printf("NMP MAC address is not set yet\n");
#endif  /* DEBUG */
		return(-1);
	}
   else
      printf("NMP MAC address %x:%x:%x:%x:%x:%x\n",
            nmptest_mac[0], nmptest_mac[1],nmptest_mac[2],
            nmptest_mac[3], nmptest_mac[4],nmptest_mac[5]);

   if (LCP_ReadMacAddr ((tLCP_MacAddrU *) &nmptest_srcmac) == (tLCP_MacAddrU *)0 )
	{
#ifdef  DEBUG
      printf("ATM MAC address is not set yet\n");
#endif  /* DEBUG */
		return(-1);
	}
   else
      printf("ATM MAC address %x:%x:%x:%x:%x:%x\n",
            nmptest_srcmac[0], nmptest_srcmac[1], nmptest_srcmac[2],
            nmptest_srcmac[3], nmptest_srcmac[4], nmptest_srcmac[5]);


   /*
    *  Add a CAM entry for the hosts mac address
    *  The state is set to indicate that the destination is ACP
    */
    if ( (pCame = cam_create_entry((tMACADDR *)nmptest_mac, acp_color,
                             (UINT32)CAME_LOOPTST_C)) == NULLP)
    {
#ifdef  DEBUG
        printf("ALC_SYSDIAG CAM test cam_create_entry failed\n");
#endif  /* DEBUG */
        return(-1);   /* failed to add entry, can't continue loopback test */
    }

   /* set up VC for RxSar to tune to */
   if( aalsap_open_vcc( 0, vpc, 0, vpc, 0,
      VPC_TX_ACTIVE, VPC_DEST_ACP, 0, 0,  nmploop_uep) == 0)
   {
#ifdef  DEBUG
      printf("nmpdiag_setup: aalsap_open_vcc failed \n");
#endif  /* DEBUG */
        return(LPBKDIAG_VCCOPEN_ERROR); /* failed to add CAM entry */
   }

		/* if pmd_loop_mode set, set pmd for internal loopback */
	set_pmd_loopmode(pmd_loopmode);

	SD_DumpSarBigaInfo () ;

   return(0);   
}  /* nmpdiag_setup() */


/*--------------------------------------------------------------------*
 * nmpdiag_looptest()
 *--------------------------------------------------------------------*/
int
nmpdiag_looptest()
{
   int   rc = 0;
   tVPC  vpc = SYSTST_VPC;

   tpCAM_ENTRY pCame = NULLP;              /* mac adddress for acp  */
   tCOLOR      acp_color = SYSTST_COLOR;   /* 0x0 */
   UINT32      CamRc = 0 ;

   rc = nmploop_core();

   /*
    * close the VC and clear CAM.
    */
   aalsap_close_vcc( vpc, 0);
   CamRc = cam_clear_entry (NULLP, (tMACADDR *)nmptest_mac, acp_color) ;

   SD_DumpSarBigaInfo () ;

#ifdef  DEBUG
   if (!rc)
      printf("NMP loopback test passed !\n");
   else if (rc > 0)
      printf("NMP loopback test: err_code = %d %s\n", rc, diag_error_info[rc-1]);
   else
      printf("NMP loopback test: test failed!\n");   /* non-defined error */
#endif  /* DEBUG */

   if(CamRc != ACP_OK)
   {
      printf("\n\r failed to clear CAM entry for NMP diag\n");
   }
   printf("\n");

   return(rc);
}  /* nmpdiag_looptest() */

/*--------------------------------------------------------------------*
 * DVT_Test()
 *--------------------------------------------------------------------*/
int
DVT_Test()
{
    int   rc = 0;
    char  tmpbuf[6];

    /* W&G DVT Test mac address */
    tMACADDR    tst_macaddr = {0x00,0x01,0x68,0x30,0x01,0x01};

    tpCAM_ENTRY pCame = NULLP;                  /* mac adddress for acp  */
    tCOLOR      acp_color = 0;

    acp_color = dvt_test_param.txpkt_vlan;

    rc = DVT_core();

#ifdef  DEBUG
   if (!rc)
      printf("DVT test passed !\n");
   else if (rc > 0)
      printf("DVT: err_code = %d %s\n", rc, diag_error_info[rc-1]);
   else
      printf("DVT: test failed!\n");   /* non-defined error */
#endif  /* DEBUG */


#ifdef  PEEKSAGE
    getSaintReg();
#endif  /* PEEKSAGE */

    return(rc);

}  /* DVT_Test() */


/*--------------------------------------------------------------------*
 * LpbkSetup()
 *    set  mode of loopback
 *    set  processor of loopback
 *    set  VLAN number for packets going back to synergy backplane
 *--------------------------------------------------------------------*/
int LpbkSetup()
{
	int   rc = 0;
    int   indx = 0;
    char  tmp_buf[5];
    int   tmp;
	int   err;


	/* 
     * check if Loopback is through ACP or RxSAR
     */
    printf("Set PMD to internal loopback mode ? (y/n) ");
    get_line(tmp_buf, 3);
    if (strchr(tmp_buf, 'y') != (char *)0 )
    {
        pmd_loopmode = 1;
        printf("PMD is set for internal loopback \n");
    }
    else
    {
        pmd_loopmode = 0;
        printf("To run loopback test, use external loopback plug.\n");
    }

	set_pmd_loopmode(pmd_loopmode);

    return(rc);
} /* LpbkSetup */


/*--------------------------------------------------------------------*
 * setup_dvt()
 *    set up for DVT test
 *    set  mode of loopback
 *    set  processor of loopback
 *    set  VLAN number for packets going back to synergy backplane
 *--------------------------------------------------------------------*/
int setup_dvt()
{
	int   rc = 0;
    int   indx = 0;
    char  tmp_buf[5];
    int   tmp;
	int   err;

	/* 
	 * setup local SAGE so that NMP can send SCP messages to
	 * configure the LTL & CBL on this card 
	 */ 
    EnDisSage(COMMAND_MODULE_ENABLE);
    EnDisSage(COMMAND_PORT_ENABLE);
/*  EnDisSage(COMMAND_MODULE_ON_LINE); */

    writeSaintReg(MySlotNum, 1, SAINT_SETREGS, 0x60, 0xA1);
    writeSaintReg(MySlotNum, 1, SAINT_SETREGS, 0x61, 0xE6);

	/* 
     * check if Loopback is through ACP or RxSAR
     */
#if 1
    printf("\nLoopback test through ACP ? (y/n) ");
    get_line(tmp_buf, 3);
	if (strchr(tmp_buf, 'y') != (char *)0 )
    {
		dvt_test_param.dvt_thru_acp = 1 ; 
        printf("Loopback test is through ACP\n");
    }
    else
    {
		dvt_test_param.dvt_thru_acp = 0 ;
        printf("Loopback test is through RxSAR\n");
    }
#else 
		dvt_test_param.dvt_thru_acp = 0 ;
        printf("Loopback test is through RxSAR\n");
#endif  /* ZERO */

	/* 
	 * check if DA/SA swapping is needed 
	 */
    printf("\nEnable DA/SA mac swapping ? (y/n) ");
    get_line(tmp_buf, 3);
	if (strchr(tmp_buf, 'y') != (char *)0 )
    {
        printf("DA/SA is swapped in loopback test\n");
		dvt_test_param.mac_swap = 1;
    }
    else
    {
		dvt_test_param.mac_swap = 0;
        printf("DA/SA is not swapped in loopback test\n");
    }

	/* 
     * set VLAN number for packet coming from synergy bus
     */
    do
    {
        /* default to VLAN 1 */
    	printf("\nEnter VLAN #(decimal) for packets coming from backplane: ");
		err = 0;
		get_line(tmp_buf,5);
		if (strlen(tmp_buf) != 0)
		{
			if (!sscanf_x(tmp_buf, "%d", &tmp))
            	err = 1;
			else
				dvt_test_param.txpkt_vlan = (uInt16)tmp;
		}
	} while (err);

	/* 
     * set VLAN number for packet going back to synergy bus
     */
    do
    {
        /* default to VLAN 2 */
    	printf("\nEnter VLAN #(decimal) for packets loopback to backplane: ");
		err = 0;
		get_line(tmp_buf,5);
		if (strlen(tmp_buf) != 0)
		{
			if (!sscanf_x(tmp_buf, "%d", &tmp))
            	err = 1;
			else
				dvt_test_param.rxpkt_vlan = (uInt16)tmp;
		}
	} while (err);

	/* 
     * make known of the dst mac address of the incoming packets
     */
	printf("DVT test packets COMING from VLAN %d\n",dvt_test_param.txpkt_vlan);
	printf("DVT test packets LOOPBACK on VLAN %d\n",dvt_test_param.rxpkt_vlan);

    /*
     * prompt user to config LTL & CBL on NMP console
     */
    printf(" *** Type in the following commands on NMP console ***\n");
    indx = 0x6000 + ((MySlotNum-1)<<6) & 0xffff;
    printf("set ltl %d %lX 1\n", MySlotNum, indx );
    indx = 0x4000 + dvt_test_param.txpkt_vlan ;
    printf("set ltl %d %lX 1\n", MySlotNum, indx );
#if  0
    indx = 0x4400 + dvt_test_param.txpkt_vlan ;
    printf("set ltl %d %lX 1\n", MySlotNum, indx );
#endif  /* ZERO */

    printf("set cbl %d %x 3\n", MySlotNum, dvt_test_param.txpkt_vlan);
    indx = 0x400 + dvt_test_param.txpkt_vlan ;
    printf("set cbl %d %x 3\n", MySlotNum, indx );

	if (dvt_test_param.txpkt_vlan != dvt_test_param.rxpkt_vlan)
	{
	    printf("set cbl %d %x 3\n", MySlotNum, dvt_test_param.rxpkt_vlan);
		indx = 0x400 + dvt_test_param.rxpkt_vlan ;
		printf("set cbl %d %x 3\n", MySlotNum, indx );
	}

    return(rc);

} /* setup_dvt */


/*--------------------------------------------------------------------*
 * set_pmd_loopmode()
 *    set  mode of loopback
 *    set  processor of loopback
 *    set  VLAN number for packets going back to synergy backplane
 *--------------------------------------------------------------------*/
void 
set_pmd_loopmode(int loop_mode)
{
	int  type;

	type = LCP_PmdType();

	if (loop_mode)
	{
		/*
		 * if pmd_loop_mode set, set pmd for internal loopback
		 */
        switch(type)
        {
            case  PMD_NTCa:
                ntc_control(NTC_FRAMER_LOOPBACK_ON);
                break;
 
            case  PMD_PMC:
                syn_atm_pmc_oc3_init_loopback();
                break;
 
            default:
                syn_atm_pmc_oc3_init_loopback();
                break;
        }
	}
	else
	{   /* reset the pmd loopback mode */
		if (type == PMD_PMC) 
			syn_atm_pmc_oc3_init();
	}
}  /* set_pmd_loopmode() */


/*--------------------------------------------------------------------*
 * Notify_NMP_of_diagMode() 
 *--------------------------------------------------------------------*/
void
Notify_NMP_DiagMode(void)
{
	int rc = 0;
   char		*preg;
   ALCSTATUS   *pALC_diagstatus = (ALCSTATUS *)&DiagResults.alc_diag_results;
   PMDSERPROM  *pPMD_eeprom = (PMDSERPROM *)&DiagResults.pmd_info;

	/*
    * Diag Result AcpErrStat2
    */
    rc = eeprom_checksum();
    if (rc)
        pALC_diagstatus->AcpErrStat2 |= ACP_PMD_EEPROM;   /* error */
    else
    {
        pALC_diagstatus->AcpErrStat2 &= ~ACP_PMD_EEPROM;  /* ok */
 
        /*
         * read in PMD serial eeprom info
         */
        rd_eeprom_c(0, ADDITIONAL_INFO_SIZE/2, (uInt16 *)pPMD_eeprom);
    }

	/*
	* set ACP in Off-line Diag mode in Module Ready Message
	*/
	pALC_diagstatus->AcpErrStat2 |= (ACP_InDiagMode);   


	/* Send Module Ready to NMP */
	LCP_DriverInit_3 (FALSE, TRUE, TRUE, sizeof(DiagResults),
				(u_int8 *)&DiagResults);
	LCP_DriverInit_5 (FALSE, TRUE) ;


} /* Notify_NMP_DiagMode() */ 

/*---------------------------< END >---------------------------------*/
