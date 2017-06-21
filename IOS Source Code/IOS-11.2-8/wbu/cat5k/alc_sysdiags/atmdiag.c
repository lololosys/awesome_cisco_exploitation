/* $Id: atmdiag.c,v 1.1.4.1 1996/05/09 15:00:45 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_sysdiags/atmdiag.c,v $
 *-----------------------------------------------------------------
 * atmdiag.c -- ATM driver test running on the ACP
 *
 * ?
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmdiag.c,v $
 * Revision 1.1.4.1  1996/05/09  15:00:45  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:16:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:42:03  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:12:54  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:44  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:53:37  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:43:14  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3  1995/10/13  18:16:40  rbau
 * added cvs keywords in the header
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*---< Includes >---*/

#include  "basic_types.h"
#include  "sar_if.h"
#include  "atmdrv_if.h"
#include  "../alc_drivers/ntc_if.h"
#include  "atmdiag.h"
#include  "acp_c_defs.h"

#include  "../alc_drivers/atmdvr_gdata.h"

/* to activate the SAR image download */
#define  SARDNLD
#define  FUNC_NULL      ((void (*)())0)

/*---< Globals >---*/
UINT32	testcase;
UINT32	testresult;
extern	int  MFG_JMPER;

/*---< Prototypes >---*/
void		sysdiag_dotst();
UINT32	sysdiag_result();
void  	sysdiag_next();
void		sysdiag_interrupt_txsar();
UINT8	  *sysdiag2str(UINT32);

extern UINT32 biga_init();
extern void sysdiag_1vpc(void);
extern void sysdiag_Nvpc(void);
extern void sysdiag_Ncell(void);
extern void atmtst_1cam(void);
extern void atmtst_Ncam(void);

char *testname[]=
{ 
	"SYSDIAG_INIT_TEST",
	"SYSDIAG_1ENQ_DEQ_TEST",
	"SYSDIAG_NENQ_DEQ_TEST",
	"SYSDIAG_1VPC_TEST",
	"SYSDIAG_NVPC_TEST",
	"SYSDIAG_1CAM_TEST",
	"SYSDIAG_NCAM_TEST",
	"SYSDIAG_LCP_PING",
	"SYSDIAG_NCELL_TEST",
	"SYSDIAG_TEST_OVER"
};


/*-------------------------------------------------------------------*/
typedef struct  test_table
{
   int   test_case;
   void  (*action)();
   int	next_case;
} TEST_TABLE_T, *PTEST_TABLE_T;


/*--------------------------------------------------------------------*
 *    next state decoder for this test state machine
 *--------------------------------------------------------------------*/
TEST_TABLE_T  atmdriver_tests[] =
{
	{SYSDIAG_INIT_TEST, FUNC_NULL, 0},
	{SYSDIAG_1VPC_TEST, sysdiag_1vpc, 0},
	{SYSDIAG_NVPC_TEST, sysdiag_Nvpc, 0},
	{SYSDIAG_NCELL_TEST, sysdiag_Ncell, 0},
	{SYSDIAG_1CAM_TEST, atmtst_1cam, 0},
	{SYSDIAG_NCAM_TEST, atmtst_Ncam, 0},
 	{SYSDIAG_TEST_OVER, FUNC_NULL, 0} 
}; 


/*--------------------------------------------------------------------*
 * loop2_core()
 *    return :   0  SUCCESS
 *               -1 FAILED
 *--------------------------------------------------------------------*/
int
loop2_core()
{
	int 	i;

   /*
    * Go through the tests
    */
   testcase = SYSDIAG_INIT_TEST;
   testresult = SYSDIAG_PASSED;

   /*
    * crank up state machine for this test
    */
   while(1)
   {
		switch(testresult)
		{
			case  SYSDIAG_RUNNING:
				break;      /* continue */

			case  SYSDIAG_PASSED:
				++testcase;
	         if( atmdriver_tests[testcase].test_case == SYSDIAG_TEST_OVER)
					testresult = SYSDIAG_EXIT;
				else	
				{
	         	if( atmdriver_tests[testcase].action == FUNC_NULL)
						continue;
					else
	         		atmdriver_tests[testcase].action(); 
				}
				break;

			case  SYSDIAG_FAILED:
#ifdef  DEBUG
				printf("\n ATM Driver Test Failed\n");
#endif  /* DEBUG */
				return(-1);
				break;

			case  SYSDIAG_TEST_OVER:
			case  SYSDIAG_EXIT:
#ifdef  DEBUG
         printf("\n ATM Driver Test Finished\n");
#endif  /* DEBUG */
				return(0);

			default:
				return(0);
      }  /* switch */
        
   } /*while*/

} /* loop2_core() */

/*--------------------------------------------------------------------*
 * AtmSysDiagMain() 
 *--------------------------------------------------------------------*/
int
AtmSysDiagMain()
{
	int 	rc;
	
	ncam = 5;
	rc =	loop2_core();

	return(rc);

}


/*--------------------------------------------------------------------*
 * sysdiag2str(UINT32 testcase)
 *    convert test state to state name
 *--------------------------------------------------------------------*/
UINT8	*
sysdiag2str(UINT32 testcase)
{
	if ( (!testcase) || (testcase > SYSDIAG_TEST_OVER) )  
		return "UNKNOWN";
   else
		return(testname[testcase-1]); 
}


#ifdef SYSDIAG_INIT 

/*--------------------------------------------------------------------*
 * sysdiag_init()
 *    initialize all buffers, queues for ATM linc card system diag
 *
 * return:  int
 *--------------------------------------------------------------------*/
int 
sysdiag_init(void)
{
	UINT32  *pLCP;			
	volatile UINT8	*pReg ;
	UINT32   i, j;

#ifdef  DEBUG
	printf("ALC system diagnostic init\n");
#endif  /* DEBUG */

	/* 
	 * initialize test result for test state machine 
	 */	
	testresult = SYSDIAG_PASSED;
 
   pReg  = (UINT8 *)(ACP_HW_IOREG_BASE) ;

/* #ifdef  SARDNLD */
	if (!MFG_JMPER)
	{
   /* Ensure TSAR/RSAR are in reset.   This could also be a good */
   /* place to download them ....                                */
#ifdef DEBUG
   printf("sysdiag_init: putting sar's into reset\n\r");
#endif
   	 pReg [ACP_WRRG_TSAR_RESET_] = ON_ ;               /* For TxSAR. */
	    pReg [ACP_WRRG_RSAR_RESET_] = ON_ ;               /* For RxSAR. */
	}

   /* Initialize driver registration data structures etc. */
   ADVR_Std_TSar_Upcall = NULL ;

   /* Initialize Structure that keeps track of which VC Maps are in VCR */
   VCR_ivci_map_info_init();

   /* Initialize number of active VPI and VCI bits */
   num_active_vpi_vci_bits = VPI_BITS_C + VCI_BITS_C;
   num_active_vpi_bits = VPI_BITS_C;
   num_active_vci_bits = VCI_BITS_C;

	/*
    * Initialize shared memory
    */
	if (shmem_init() != ACP_OK)
	{
		printf("%s:shared memory init failed\n",__FILE__);
		return SYSDIAG_ERR1;
	}

	/*
    * Initialize  TXSAR secondary port memory
    */
	if (tx_mem_init() != ACP_OK)
	{
		printf("%s:TX SAR SP memory init failed\n", __FILE__); 
		return SYSDIAG_ERR2;
	}

	/*
    * Initialize  RXSAR secondary port memory
    */
	if (rx_mem_init() != ACP_OK)
	{
		printf("%s:RX SAR SP memory init failed\n", __FILE__); 
		return SYSDIAG_ERR3;
	}

#ifdef DEBUG
   /* Print Memory Addresses */
   spram_log();
#endif /* DEBUG */

	/* reset BIGA
	 * temp hack to reset BIGA for BIGA may have already started
	 * by BIGA test
	 */
	*(char *)(0x38000003) = 0;		
	*(char *)(0x38000003) = 1; 

	/*
	 *	initialize BIGA
	 */
	(void)biga_init();
	
   /* wait for LCP_Ready message(Restart message 
    * moved to ALC_SysDiag()
    */
 
	/*
	 *	initialize NTC
	 */
	ntc_init();

#ifdef  SARDNLD 
	/*
	 * initialize SARS
	 */
#ifdef DEBUG
	printf("\n%s:sysdiag_init: dropping sar's out of reset\n",__FILE__);
#endif

    /* Drop TSAR/RSAR out of reset. */
    pReg [ACP_WRRG_TSAR_RESET_] = OFF_ ;			/* For TxSAR. */
    pReg [ACP_WRRG_RSAR_RESET_] = OFF_ ;			/* For RxSAR. */

	/* ------------------------------------------------------- */
	/* Assume that interrupts have been masked off at the MFP. */
	/* Temporarily sync to PMON-based serial-line downloads.   */
	/* A set of init-only interrupt ISRs have been bound.      */
	/* Allow interrupts NOW, then re-mask when init done.      */
	/* Caller will then bind real ISRs and un-mask interrupts. */
	/* ------------------------------------------------------- */
	/* Since this is system start-up code, assume that spin    */
	/* wait is okay.  We are NOT punching the watchdog here... */
	/* ------------------------------------------------------- */
#if	0
	while( sar_intr_status != (RXSAR_INTR | TXSAR_INTR) ) {
		wait_for_interrupt();
	}
#else
  	wait_for_interrupt (); 
#endif
	
	sar_intr_status = 0;
#endif  /* SARDNLD */

    /*
     *  Initialize CAM 
     */
#ifdef DEBUG
    printf("%s:sysdiag_init: Initializing CAMs\n",__FILE__);
#endif  /* DEBUG */

   if(cam_init() != ACP_OK)
   {
      printf("sysdiag_init: CAM init failed");
		while(1)
      {
			;							/* crash and burn */
		}
      return SYSDIAG_ERR7;
   }

	return SYSDIAG_OK;

} /* sysdiag_init() */
#endif  /* SYSDIAG_INIT */


/*---------------------------< END >---------------------------------*/
