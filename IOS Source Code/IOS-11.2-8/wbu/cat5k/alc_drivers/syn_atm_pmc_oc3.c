/* $Id: syn_atm_pmc_oc3.c,v 1.1.4.1 1996/05/09 14:55:08 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/syn_atm_pmc_oc3.c,v $
 *------------------------------------------------------------------
 * syn_atm_pmc_oc3.c -- Catalyst 5000 ATM Line Card SUNI-LITE
 *                      (STS-3c) driver.
 *
 * August, 1995  Sanjaya Kumar
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: syn_atm_pmc_oc3.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:08  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:43  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:36  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:04  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8.2.1.6.1  1996/02/22  23:23:23  sakumar
 * Added PMD statistics for "show controller".
 *
 * Revision 1.8.2.1.4.1  1996/02/22  23:19:14  sakumar
 * Added PMD statistics for "show controller".
 *
 * Revision 1.8.2.1  1995/11/15  23:25:52  rbau
 * merge mainline code changes in 1.4 branch
 *
 * Revision 1.9  1995/11/08  00:42:23  rbau
 * added show_pmc_stats function for off-line diag
 *
 * Revision 1.8  1995/10/18  23:36:36  sakumar
 * Due to the PMC erroneously counting idle cells as real cells, the TSAR
 * now keeps the output cell count. This count is then multiplied by
 * 48 to get the octet count.
 *
 * We still use the PMC's receive cell count.
 *
 * Revision 1.7  1995/10/17  18:03:53  rbau
 * corrected #ifdef 0 to #ifdef DEBUG
 *
 * Revision 1.6  1995/10/17  01:26:59  sakumar
 * Turn on LINK LED at bootup. Will fix later to sense the real link
 * state and set LED accordingly.
 *
 * Revision 1.5  1995/10/06  17:02:09  sakumar
 * Added RX and TX octet count (based on cell count*48). However, currently
 * disabling TX count since PMC counts idle cells sent by TXSAR.
 *
 * Revision 1.4  1995/09/15  23:04:46  rbau
 * alligned PMC registers on 32-bit boundary, use byte access.
 *
 * Revision 1.3  1995/09/07  17:37:23  sakumar
 * Fix for loopback.
 *
 * Revision 1.2  1995/09/07  16:24:24  sakumar
 * Added loopback function for diagnostics.
 *
 * Revision 1.1  1995/08/30  19:18:18  sakumar
 * First version of PMC Sierra SuniLite PMD driver.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*******************************************************************
			Include Files
 *******************************************************************/

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif

#include    "acp_basic_types.h"
#include    "lo_wdog.h"
#include    "acp_hw_defs.h"
#include    "acp_c_defs.h"
#include    "mfp_hw_defs.h"
#include    "mfp_acp_defs.h"
#include    "spram.h"
#include    "led_if.h"
#include    "ntc_client.h"
#include    "syn_atm_pmc_oc3.h"
#include    "basic_types.h"
#include    "param.h"
#include    "queue_if.h"
#include    "buffer_if.h"
#include    "ovcimap.h"
#include    "cam_if.h"
#include    "shram.h"
#include    "sar_if.h"
#include    "ivcimap.h"
#include    "atmdrv_if.h"
#include    "atmdrv_if.h"


/*******************************************************************
			Definitions
 *******************************************************************/
void PMC_IntrInit (void (** VecTbl)(), u_int8 fReset, u_int8 fMaskIt);
void show_pmc_stats (void); 



/*******************************************************************
			Variables
 *******************************************************************/


 
/*******************************************************************
			Globals
 *******************************************************************/

/* SUNI-LITE registers ptr */
suni_lite_t			*rp = (suni_lite_t *) ACP_HW_NTC_BASE;

UINT32 pmc_link_state = IN_FRAMING;
/* Function pointer to atmdriver upcall */
void (* pmc_line_state_change_upcall) (int) ;

extern tATMDRVGCB AtmdrvGcb;

static int init_poll_count = 3; /* counters will be zero for this many
                                 * initial poll intervals */

 
/*******************************************************************
			Functions
 *******************************************************************/



/*
 * s y n _ a t m _ p m c _ o c 3 _ i n i t ( )
 *
 * SUNI-LITE PLIM chip reset and initialization.
 */
void
syn_atm_pmc_oc3_init (void)
{
 	u_int8	mctlr;
	volatile char *p_reg; 

    PMC_IntrInit (NULL, TRUE, FALSE) ;
 
    /* Init upcall function pointer */
    pmc_line_state_change_upcall = NULL;

    /*
     * Initialize local variables
     */
    mctlr = 0;

	/*
	 * Reset PLIM
	 */
	p_reg = (char *)&rp->mr;
   *p_reg = LITE_MR_RESET ;
#ifdef  DEBUG 
	printf("PMC reset reg %x = %x\n", p_reg, rp->mr);
#endif  /* DEBUG */
   *p_reg = 0;           /* clear SW reset */

    /*
     * Initialize chip registers
     */
	p_reg = (char *)&rp->mctlr;
   *p_reg = mctlr;

    /* Enable HCS coset polynomial */
	p_reg = (char *)&rp->racp_csr;
	*p_reg = LITE_RAOP_CS_HCSADD ;

	/* Enable line state change interrupts */
	p_reg = (char *)&rp->rsop_cier;
	*p_reg = LITE_RSOP_CIE_LOSE | LITE_RSOP_CIE_LOFE | LITE_RSOP_CIEOLOFE;


	/* Read RSOP Status/Interrupt Status to see current link state */
	p_reg = (char *)&rp->rsop_sisr;
	
	if (*p_reg & LITE_RSOP_SIS_LOSV) {
		/* Loss of Signal, so turn Link LED OFF */
		set_led(ACP_WRRG_PMD_BOT_LED, ACP_PMD_LEVL_LED_OFF);
	} else {
		set_led(ACP_WRRG_PMD_BOT_LED, ACP_PMD_LEVL_LED_GRN);
        init_poll_count = 3;
	}
}

/*
 * s y n _ a t m _ p m c _ o c 3 _ i n i t _ l o o p b a c k( )
 *
 * SUNI-LITE PLIM chip reset and initialization into loopback mode.
 */
int
syn_atm_pmc_oc3_init_loopback (void)
{
	u_int8      mctlr;
	volatile char  *p_reg;
	
    /*
     * Initialize local variables
     */
   mctlr = 0;

#ifdef DEBUG
	rp = (suni_lite_t *) ACP_HW_NTC_BASE;
	printf("PMC h/w base %x\n", rp);
#endif /* DEBUG */
 
	/*
	 * Reset PLIM
	 */
	p_reg = (char *)&rp->mr;
   *p_reg = LITE_MR_RESET ;

#ifdef DEBUG
	printf("PMC reset reg %x = %x\n", p_reg, rp->mr);
#endif /* DEBUG */
   *p_reg = 0 ;

#ifdef DEBUG
	printf("PMC reset reg %x = %x\n", p_reg, rp->mr);
#endif  /* DEBUG */

	/*
	 * Initialize chip registers
	 */
	p_reg = (char *)&rp->mctlr;
	mctlr |= LITE_MCTL_DLE;
   *p_reg = mctlr;

	if ((*p_reg & LITE_MCTL_LCDV) == LITE_MCTL_LCDV)
	{
#ifdef  DEBUG
		printf(" PMC mctrl = %x\n", rp->mctlr);
		printf(" PMC CSCS = %x\n", rp->cscsr);
		printf(" PMC CRCS = %x\n", rp->crcsr);
		printf(" PMC RACP = %x\n", rp->racp_csr);
#endif  /* DEBUG */
		printf("PMC not able to run in LOOPBACK mode\n");
		return(-1);        /* failed to set PMC loopback */
	}

	return(0); 
}



/*
 * s y n _ a t m _ p m c _ o c 3 _ i s r
 *
 * SUNI-LITE interrupt handler.
 */
void
syn_atm_pmc_oc3_isr (void)
{
    char *p_reg;
    UINT32 soft_rsop_sisr;
 
    p_reg = (char *)&rp->rsop_sisr;
    soft_rsop_sisr = *p_reg;
 
#ifdef DEBUG /* In debug we look for OOF and LOF events */
    if (soft_rsop_sisr & LITE_RSOP_SIS_OOFI) {
        if (soft_rsop_sisr & LITE_RSOP_SIS_OOFV) {
            AtmdrvGcb.OOF_count++;
        }
    }
 
    if (soft_rsop_sisr & LITE_RSOP_SIS_LOFI) {
        if (soft_rsop_sisr & LITE_RSOP_SIS_LOFV) {
            AtmdrvGcb.LOF_count++;
        }
    }
#endif /* DEBUG */
 
   
    if (soft_rsop_sisr & LITE_RSOP_SIS_LOSI) {
        if (soft_rsop_sisr & LITE_RSOP_SIS_LOSV) {
            pmc_link_state = LOSS_OF_FRAMING;
            set_led(ACP_WRRG_PMD_BOT_LED, ACP_PMD_LEVL_LED_OFF);
        } else {
            pmc_link_state = IN_FRAMING;
            set_led(ACP_WRRG_PMD_BOT_LED, ACP_PMD_LEVL_LED_GRN);
            init_poll_count = 3;
            
        }
        /* Inform ATM driver of line state change */
        pmc_line_state_change_upcall(pmc_link_state);
    }
}

/*
 * s y n _ a t m _ p m c _ o c 3 _ g e t _ c e l l _ s t a t s
 *
 * Returns the number of cells received/transmitted since last call
 * to this function.
 */
void
syn_atm_pmc_oc3_get_cell_stats(UINT32 *rxcellcount, UINT32 *txcellcount)
{
	
	volatile UINT8 *p_reg, *p_reg_lo, *p_reg_mid, *p_reg_hi; 

	p_reg = (char *)&rp->mr;
	*p_reg = 0x30;
	MFP_Poll_uSec(2);

	p_reg_lo = (char *)&rp->racp_rcc0r;
	p_reg_mid = (char *)&rp->racp_rcc1r;
	p_reg_hi = (char *)&rp->racp_rcc2r;
	*rxcellcount = *p_reg_lo | (*p_reg_mid<<8) | ((*p_reg_hi&7)<<16);

	p_reg_lo = (char *)&rp->tacp_tcc0r;
	p_reg_mid = (char *)&rp->tacp_tcc1r;
	p_reg_hi = (char *)&rp->tacp_tcc2r;
	*txcellcount = *p_reg_lo | (*p_reg_mid<<8) | ((*p_reg_hi&7)<<16);

	if (init_poll_count <= 0) {
        /*
         * Start polling for these counters only after init_poll_count
         * number of polling intervals. This is because initially there
         * will always be non-zero counts that have no real significance 
         */
        p_reg_lo = (char *)&rp->rsop_bip80r;
        p_reg_mid = (char *)&rp->rsop_bip81r;
        AtmdrvGcb.B1_err += (*p_reg_lo | (*p_reg_mid<<8));
 
        p_reg_lo = (char *)&rp->rlop_bip8_240r;
        p_reg_mid = (char *)&rp->rlop_bip8_241r;
        p_reg_hi = (char *)&rp->rlop_bip8_242r;
        AtmdrvGcb.B2_err += (*p_reg_lo | (*p_reg_mid<<8) | ((*p_reg_hi&0xf)<<16));
 
        p_reg_lo = (char *)&rp->rlop_febe0r;
        p_reg_mid = (char *)&rp->rlop_febe1r;
        p_reg_hi = (char *)&rp->rlop_febe2r;
        AtmdrvGcb.Z2_err += (*p_reg_lo | (*p_reg_mid<<8) | ((*p_reg_hi&0xf)<<16));
 
        p_reg_lo = (char *)&rp->rpop_pbip80r;
        p_reg_mid = (char *)&rp->rpop_pbip81r;
        AtmdrvGcb.B3_err += (*p_reg_lo | (*p_reg_mid<<8));
 
        p_reg_lo = (char *)&rp->rpop_pfebe0r;
        p_reg_mid = (char *)&rp->rpop_pfebe1r;
        AtmdrvGcb.G1_err += (*p_reg_lo | (*p_reg_mid<<8));
 
        p_reg_lo = (char *)&rp->racp_checr;
        AtmdrvGcb.CHCS_err += (*p_reg_lo);
 
        p_reg_lo = (char *)&rp->racp_uhecr;
        AtmdrvGcb.UHCS_err += (*p_reg_lo);
    } else {
        init_poll_count--;
        /* Clear PMD counters */
        AtmdrvGcb.B1_err = 0;
        AtmdrvGcb.B2_err = 0;
        AtmdrvGcb.B3_err = 0;
        AtmdrvGcb.Z2_err = 0;
        AtmdrvGcb.G1_err = 0;
        AtmdrvGcb.CHCS_err = 0;
        AtmdrvGcb.UHCS_err = 0;
        AtmdrvGcb.OOF_count = 0;
        AtmdrvGcb.LOF_count = 0;
    }
}

int
pmc_get_framer_status()
{
    return (pmc_link_state);
}


void
pmc_Reg_line_state_change_upcall(void * pFunc)
{
    pmc_line_state_change_upcall = (tPmc_line_state_upcall *) pFunc;
}

/* -------------------------------------------------------------------- */
/*      This routine initializes the MFP for PMC interrupts.            */
/*      This interface is more general purpose than currently used,     */
/*      to allow for future growth/usage of the driver.                 */
/* -------------------------------------------------------------------- */
void
PMC_IntrInit (void (** VecTbl)(), u_int8 fReset, u_int8 fMaskIt)
{
    register volatile u_int8 *  pReg ;
    s_int16                     Count ;
 
 
    /* RKL:  Need to groom MFP here, both for interrupt lines and timer. */
 
    pReg = ((volatile u_int8 *) (C_ADDR_HW_REGS (0))) ;
 
 
        /* Whenever the PMC is reset, ensure that the DMA area is */
        /* put into a known (zero) state as well.                 */
    if (fReset)
    {
        pReg [ACP_WRRG_PMD_RESET_] = ON_ ;
        pReg [ACP_WRRG_NTC_RESET_] = ON_ ;
 
        for (Count = 0 ; Count < 10 ; ) /* Pause to take effect. */
            { ++ Count ; }
 
    }
 
        /* Setup MFP IO lines for PMC DMA. */
    MFP_BindEnb (VecTbl, NULL, MFP_BASE_VECTOR, MFP_IINDX_NTC) ;
 
        /* At this point PMC interrupts are enabled but masked off   */
        /* at the MFP.  We can unmask them elsewhere, eg. in IOS.    */
        /* PMC uses PMD, so drop PMD from reset (with PMC) too.      */
        /* Is ordering an issue here ?                               */
    if (! fMaskIt)
    {
        MFP_UnmaskIntrW (MFP_IMSK_NTC, FALSE, FALSE) ;
    }
    pReg [ACP_WRRG_PMD_RESET_ ] = OFF_ ;
    pReg [ACP_WRRG_NTC_RESET_ ] = OFF_ ;
 
}   /* PMC_IntrInit */

/*--------------------------------------------------------------------
 * reset_pmc()
 *--------------------------------------------------------------------
 */
void
reset_pmc()
{
	volatile char  *p_reg;

   /*
    * Reset PLIM
    */
   p_reg = (char *)&rp->mr;
   *p_reg = LITE_MR_RESET ;

#ifdef  DEBUG
   printf("PMC reset reg %x = %x\n", p_reg, rp->mr);
#endif  /* DEBUG */
   *p_reg = 0;           /* clear SW reset */
}


/*--------------------------------------------------------------------
 * show_pmc_stats() 
 *
 * show the cell counts of Tx/Rx, HEC error counts
 *-------------------------------------------------------------------*/
void
show_pmc_stats(void)
{
	int  cellcount;

	volatile UINT8 *p_reg, *p_reg_lo, *p_reg_mid, *p_reg_hi; 

	printf("\nAll counters show counts of events that occurred since last read\n");

	/* to reload the meters */
	p_reg = (char *)&rp->mr;
	*p_reg = 0x30;
	MFP_Poll_uSec(2);     /* at least 200ns wait before counters reloaded */

	p_reg_lo = (char *)&rp->racp_rcc0r;
	p_reg_mid = (char *)&rp->racp_rcc1r;
	p_reg_hi = (char *)&rp->racp_rcc2r;
	cellcount = *p_reg_lo | (*p_reg_mid<<8) | ((*p_reg_hi&7)<<16);
	printf("\n\tPMC Receive Cell Count: %lX\n", cellcount);

	p_reg_lo = (char *)&rp->tacp_tcc0r;
	p_reg_mid = (char *)&rp->tacp_tcc1r;
	p_reg_hi = (char *)&rp->tacp_tcc2r;
	cellcount = *p_reg_lo | (*p_reg_mid<<8) | ((*p_reg_hi&7)<<16);
	printf("\tPMC Transmit Cell Count: %lX\n", cellcount);

	p_reg = (char *)&rp->racp_checr;
	printf("\n\tPMC Correctable HCS Error Count: %x\n", *p_reg);
	p_reg = (char *)&rp->racp_uhecr;
	printf("\tPMC Uncorrectable HCS Error Count: %x\n", *p_reg);
	printf("\n");
}
