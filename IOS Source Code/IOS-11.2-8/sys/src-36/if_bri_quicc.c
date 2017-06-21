/* $Id: if_bri_quicc.c,v 3.3.60.1 1996/08/13 02:22:15 gstovall Exp $
 * $Source: /release/112/cvs/Xsys/src-36/if_bri_quicc.c,v $
 *------------------------------------------------------------------
 * if_bri_quicc.c - ISDN basic rate interface routines that use the 
 *                  QUICC chip.
 *
 * November 1994, Andrew G. Harvey
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_bri_quicc.c,v $
 * Revision 3.3.60.1  1996/08/13  02:22:15  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.3  1995/11/17  18:39:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:29:07  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:18:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:51:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "logger.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../les/if_les.h"
#include "../src-36/quicc_driver_lib.h"
#include "../src-36/if_bri_quicc.h"

/*
 * =================================================================
 * Name: quicc_access_sid
 *
 * Description:
 *	Send a command to the TP3420 via the SPI interface.  This
 *	routine will acquire the spi and also set the appropriate
 *	chip select.
 *
 * Parameters:
 *	quicc_dpr_t *dpr - pointer to dualport ram.
 *	uchar cmd - command to send.
 *
 * Returns:
 *	uchar - status from TP3420.
 * =================================================================
 */

uchar quicc_access_sid (quicc_dpr_t *dpr, uchar cmd)
{
    uchar command;
    uchar status;

    command = cmd;

    quicc_spi_acquire();
    TP3420_ASSERT(dpr);

    /*
     * If timedout what should we do?
     */
    quicc_spi_op(dpr, &command, &status, TP3420_CMD_LEN);
    TP3420_NEGATE(dpr);
    quicc_spi_release();

    return(status);
}


/*
 * =================================================================
 * Name: quicc_bri_set_bandwidth
 *
 * Description:
 *	This routine switches the SIRAM entries.  The primary
 *	entry is configured for 64Kb/s (real isdn) and the
 *	shadow RAM is configured for 56Kb/s.
 *
 * Parameters:
 *	quicc_dpr_t *dpr - pointer to dualport ram.
 *	ulong b1_bw - Bandwidth of B1 channel
 *	ulong b2_bw - Bandwidth of B2 channel
 *
 * Returns:
 *	void - none
 * =================================================================
 */

void quicc_bri_set_bandwidth (quicc_dpr_t *dpr, ulong b1_bw, ulong b2_bw)
{
    volatile ushort *siram;
    ushort roffset, toffset;
    ushort b1_bits, b2_bits;
    ushort pad1 = QUICC_SIRAM_PAD1;
    ushort pad2 = QUICC_SIRAM_PAD2;
    
    QASSERT((b1_bw == QUICC_BANDWIDTH_64) || (b1_bw == QUICC_BANDWIDTH_56));
    QASSERT((b2_bw == QUICC_BANDWIDTH_64) || (b2_bw == QUICC_BANDWIDTH_56));

    siram = dpr->regs.si_siram;

    /*
     * Calculate the number of bit that are significant.
     */
    if (b1_bw == QUICC_BANDWIDTH_64) {
	b1_bits = QUICC_SIRAM_BITS_64;
	if (b2_bw == QUICC_BANDWIDTH_64)
	    b2_bits = QUICC_SIRAM_BITS_64;
	else {
	    b2_bits = QUICC_SIRAM_BITS_56;
	    pad2++;
	}
    } else {
	b1_bits = QUICC_SIRAM_BITS_56;
	pad1++;
	if (b2_bw == QUICC_BANDWIDTH_64)
	    b2_bits = QUICC_SIRAM_BITS_64;
	else {
	    b2_bits = QUICC_SIRAM_BITS_56;
	    pad2++;
	}
    }

    /*
     * Find which map is running and switch the offsets.
     */
    if ((dpr->regs.si_sistr & 0xC0) == 0) {
	roffset = QUICC_SIRAM_MAP2_RX;
	toffset = QUICC_SIRAM_MAP2_TX;
    } else {
	roffset = QUICC_SIRAM_MAP1_RX;
	toffset = QUICC_SIRAM_MAP1_TX;
    }

    /*
     * Receive Routing Map.
     */
    siram[roffset + 0] = QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD);
    siram[roffset + 1] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BIT(b1_bits));
    siram[roffset + 2] = QUICC_SIRAM_CNT_BIT(pad1);
    siram[roffset + 3] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BIT(b2_bits));
    siram[roffset + 4] = (QUICC_SIRAM_CNT_BIT(pad2) | QUICC_SIRAM_LST);

    /*
     * Transmit Routing Map.
     */
    siram[toffset + 0] = QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD);
    siram[toffset + 1] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BIT(b1_bits));
    siram[toffset + 2] = QUICC_SIRAM_CNT_BIT(pad1);
    siram[toffset + 3] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BIT(b2_bits));
    siram[toffset + 4] = (QUICC_SIRAM_CNT_BIT(pad2) | QUICC_SIRAM_LST);

    dpr->regs.si_sicmr = 0xC0;
}


/*
 * =================================================================
 * Name: quicc_siram_init
 *
 * Description:
 * 	Set SIRAM format to use a dynamic map.  One part is set for
 *	transmit and the other for receive.  The transmit and receive
 *	maps are symetric. The values below are derived using the
 *	following format.  These maps are for Sapphire your map may
 *	vary.  This corresponds to the TP3420A DIF4 Format.  Note
 *	the D Channel bits are not needed in the sapphire design.
 *
 * 	The following map is for both channels 64Kb/s:
 *
 *	SWTR	SSEL	CSEL	CNT	BYT	LST	Description
 *
 *	 0	0000	000	0000    0	0	1 bit N/A
 *	 0	0000	010	0111	0	0	B1 (8 bits)
 *	 0	0000	000	0001	0	0	2 Bits N/A
 *	 0	0000	100	0111	0	0	B2 (8 bits)
 *	 0	0000	000	0000    0	1	1 bit N/A
 *
 * 	The following map is for both channels 56Kb/s:
 *
 *	SWTR	SSEL	CSEL	CNT	BYT	LST	Description
 *
 *	 0	0000	000	0000	0	0	1 Bit N/A
 *	 0	0000	010	0110	0	0	B1 (7bits)
 *	 0	0000	000	0010	0	0	3 Bits N/A
 *	 0	0000	100	0110	0	0	B2 (7bits)
 *	 0	0000	000	0001    0	1	2 bit N/A
 *
 * 	The following map is for channels B1 64Kb/s B2 56Kb/s:
 *
 *	SWTR	SSEL	CSEL	CNT	BYT	LST	Description
 *
 *	 0	0000	000	0000    0	0	1 bit N/A
 *	 0	0000	010	0111	0	0	B1 (8 bits)
 *	 0	0000	000	0001	0	0	2 Bits N/A
 *	 0	0000	100	0110	0	0	B2 (7bits)
 *	 0	0000	000	0001    0	1	2 bit N/A
 *
 * 	The following map is for channels B1 56Kb/s B2 64Kb/s:
 *
 *	SWTR	SSEL	CSEL	CNT	BYT	LST	Description
 *
 *	 0	0000	000	0000	0	0	1 Bit N/A
 *	 0	0000	010	0110	0	0	B1 (7bits)
 *	 0	0000	000	0010	0	0	3 Bits N/A
 *	 0	0000	100	0111	0	0	B2 (8 bits)
 *	 0	0000	000	0000    0	1	1 bit N/A
 *
 * Parameters:
 *	quicc_dpr_t *dpr - pointer to dualport ram.
 *
 * Returns:
 *	void - none.
 * =================================================================
 */

static void quicc_siram_init (quicc_dpr_t *dpr)
{
    volatile ushort *siram;
    ushort offset;
    
    siram = dpr->regs.si_siram;

    /*
     * Receive Routing Map.
     */
    offset = QUICC_SIRAM_MAP1_RX;
    siram[offset + 0] = QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD);
    siram[offset + 1] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_BITS_64));
    siram[offset + 2] = QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD1);
    siram[offset + 3] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_BITS_64));
    siram[offset + 4] = (QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD2) |
			 QUICC_SIRAM_LST);

    /*
     * Transmit Routing Map.
     */
    offset = QUICC_SIRAM_MAP1_TX;
    siram[offset + 0] = QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD);
    siram[offset + 1] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC2) |
			 QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_BITS_64));
    siram[offset + 2] = QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD1);
    siram[offset + 3] = (QUICC_SIRAM_CSEL(QUICC_PORT_SCC4) |
			 QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_BITS_64));
    siram[offset + 4] = (QUICC_SIRAM_CNT_BIT(QUICC_SIRAM_PAD2) |
			 QUICC_SIRAM_LST);

    /*
     * Tell the system that we are going to have two maps
     * for a single TDM.
     */
    dpr->regs.si_sigmr = QUICC_SIGMR_ONE_DYNAMIC;
    
    /*
     * Initalize the mode register.  Most bits in this register
     * are zero.  The console clock source should be set in the
     * prom.  Thus, mask console (SMC1) and set all other bits
     * to zero.
     */
    dpr->regs.si_simode |= QUICC_SIMODE_CRTa;
    
    /*
     * Initalize the clock input for the D Channel.  The D Channel
     * is working in NMSI mode.  The receive clock pin is CLK6 and
     * the transmit clock pin is CLK 5 (both on port A).  Initalize
     * the B channels in SI mode.
     *
     * Initalize the clock routing.  Using the following bits:
     *
     *	SCC4:	1
     *	SCC3:	0  R3CS: 101b (CLK6) T3CS: 100b (CLK5)
     *  SCC2:	1
     */
    dpr->regs.si_sicr |= (QUICC_SICR_SCX(QUICC_PORT_SCC2) |
			  QUICC_SICR_SCX(QUICC_PORT_SCC4) |
			  QUICC_SICR_RXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK6) |
			  QUICC_SICR_TXCS(QUICC_PORT_SCC3, QUICC_SCC_TO_CLK5));

    quicc_bri_set_bandwidth(dpr, QUICC_BANDWIDTH_64, QUICC_BANDWIDTH_64);

    /*
     * Enable TDMa.
     */
    quicc_tdma_enable(dpr);
}
    

/*
 * =================================================================
 * Name: quicc_bri_once_only_init
 *
 * Description: bri initalization routines that should only be run
 *		once.  This will start the spi, initalize the port
 *		pins.  Install the interrupt wrappers.
 *
 * Parameters:
 *	quicc_dpr_t * - pointer to dual port ram.
 *	hwidbtype * - idb for this SCC.
 *
 * Returns:
 *	void - None
 * =================================================================
 */

static void quicc_bri_once_only_init (quicc_dpr_t *dpr)
{
    /*
     * Set up spi.  This may need to move if some other
     * device uses the spi.  Its here now for convince
     */
    quicc_spi_init(dpr);

    /*
     * Initalize the BRI pins.
     */
    quicc_set_bri_pins(dpr);

    /*
     * Install the 3420 interrupt wrapper.
     */

    quicc_install_cpm_exception(dpr, quicc_tp3420_interrupt, 
				QUICC_CPIC_INTERRUPT_PC1);

    /*
     * enable 3420 interrupt
     */

    quicc_set_cimr(QUICC_CIMR_PC1);

    /*
     * Connect B channels to TDM A.
     */
    quicc_siram_init(dpr);
}    


/*
 * =================================================================
 * Name: quicc_init_bri
 *
 * Description:
 *	Initalize the bri interface.  This included setting the TDM
 *	for 64Kb/s.  It will initalize all three channels.  The D
 *	channel will idle with ones.  The B channels will idle with
 *	flags.  This routine does NOT enable the channel this must
 *	be done by the user.
 *
 * Parameters:
 *	hwidbtype - idb D Channel
 *
 * Returns:
 *	void - none.
 * =================================================================
 */
void quicc_init_bri (hwidbtype *idb)
{
    static boolean first_time = TRUE;
    quicc_instance_t *ds;
    quicc_dpr_t *dpr;
    ulong mtu;

    ds = INSTANCE;
    dpr = ds->dprbase;
    mtu = idb->max_buffer_size;
    
    if (first_time == TRUE) {
	quicc_bri_once_only_init(dpr);
	first_time = FALSE;
    }
    
    /*
     * Initalize HDLC parameter ram on the D Channel.
     */
    quicc_hdlc_param_init(dpr, QUICC_PORT_SCC3, mtu);

    quicc_scc_reg_init(dpr, QUICC_PORT_SCC3);
    /*
     * The D Channel MUST NOT idle with flags.  Thus, set the RTSM
     * bit in the GSMR
     */
    quicc_scc_write_gsmr(dpr, QUICC_PORT_SCC3, 0, QUICC_HDLC_IDLE_ONES);

    /*
     * Initalize HDLC parameter ram on the B Channels.
     */
    quicc_hdlc_param_init(dpr, QUICC_PORT_SCC2, mtu);
    quicc_hdlc_param_init(dpr, QUICC_PORT_SCC4, mtu);

    quicc_scc_reg_init(dpr, QUICC_PORT_SCC2);
    quicc_scc_reg_init(dpr, QUICC_PORT_SCC4);

    /*
     * The B Channel should idle with flags.  This is not strictly
     * nessesary but some older devices will choke if we don't.
     */
    quicc_scc_write_gsmr(dpr, QUICC_PORT_SCC2, 0, QUICC_HDLC_IDLE_FLAGS);
    quicc_scc_write_gsmr(dpr, QUICC_PORT_SCC4, 0, QUICC_HDLC_IDLE_FLAGS);

    /*
     * Set bandwidth to 64Kb/s.
     */
    quicc_bri_set_bandwidth(dpr, QUICC_BANDWIDTH_64, QUICC_BANDWIDTH_64);
}


/*
 * =====================================================================
 * quicc_t3_timer_clear -
 *
 * Description:
 * 	Clear the T3 timer interrupt.
 *
 * Paramters:
 * 	quicc_dpr_t * -	pointer to dual port ram.
 *
 * Returns:
 * 	void - None.
 * =====================================================================
 */
void quicc_t3_timer_clear (quicc_dpr_t *dpr)
{
    quicc_stop_timer(QUICC_TMR_3);
    dpr->regs.tmr_ter3 = QUICC_TER_REF_EVENT;
    dpr->regs.cpic_cisr = QUICC_CIMR_TIMER3;
}


/*
 * =====================================================================
 * quicc_t4_timer_clear -
 *
 * Description:
 * 	Clear the T4 timer interrupt.
 *
 * Paramters:
 * 	quicc_dpr_t * -	pointer to dual port ram.
 *
 * Returns:
 * 	void - None.
 * =====================================================================
 */
void quicc_t4_timer_clear (quicc_dpr_t *dpr)
{
    quicc_stop_timer(QUICC_TMR_4);
    dpr->regs.tmr_ter4 = QUICC_TER_REF_EVENT;
    dpr->regs.cpic_cisr = QUICC_CIMR_TIMER4;
}


/*
 * =====================================================================
 * quicc_2ms_timer_clear -
 *
 * Description:
 * 	Clear the 2ms timer interrupt.
 *
 * Paramters:
 * 	quicc_dpr_t * -	pointer to dual port ram.
 *
 * Returns:
 * 	void - None.
 * =====================================================================
 */
void quicc_2ms_timer_clear (quicc_dpr_t *dpr)
{
    quicc_stop_timer(QUICC_TMR_2);
    dpr->regs.tmr_ter2 = QUICC_TER_REF_EVENT;
    dpr->regs.cpic_cisr = QUICC_CIMR_TIMER2;
}


/*
 * =================================================================
 * Name: quicc_bri_show_controller
 *
 * Description:
 *	Show general controller parameters for BRI interface.
 *
 * Parameters:
 *	hwidbtype *idb - IDB pointer
 *      ulong bri_channel - D/B1/B2 channel indicator
 *
 * Returns:
 *	void - None.
 * =================================================================
 */
boolean quicc_bri_show_controller (hwidbtype *idb, ulong bri_channel)
{
    bri_instance_t *ds = (bri_instance_t *)INSTANCE;

    /*
     * Display scc general parms 
     */
    printf("Using SCC%d, Microcode ver %d\n", 
	   ds->port_no+1, ds->dprbase->misc.rev_num);

    quicc_scc_show_regs(idb, (bri_channel == 0) ? TRUE : FALSE);

    if (bri_channel == 0) { /* Display once only; BRI specific */
	printf("SPI Mode [SPMODE]=%#x, Events [SPIE]=%#x\n",
	       ds->regbase->spi_spmode, ds->regbase->spi_spie);
	printf("    Mask [SPIM]=%#x, Command [SPCOM]=%#x\n",
	       ds->regbase->spi_spim, ds->regbase->spi_spcom);
	printf("SI Mode [SIMODE]=%#x, Global [SIGMR]=%#x\n",
	       ds->regbase->si_simode, ds->regbase->si_sigmr);
        printf("   Cmnd [SICMR]=%#x, Stat [SISTR]=%#x\n",
	       ds->regbase->si_sicmr, ds->regbase->si_sistr);
	printf("SI Clock Route [SICR]=0x%08x\n",
	       ds->regbase->si_sicr);
	printf("RAM Ptrs - Tx [SIRPT]=%#x, Rx [SIRPR]=%#x\n",
	       ds->regbase->si_sirpt, ds->regbase->si_sirpr);
    }
	       
    /*
     * Display SCC generic stuff and
     * BRI SCC specific stuff.
     */
    quicc_scc_show_hdlc_param(ds->parmbase);

    /*
     * Allocate memory for the RX and TX rings.
     * Disable network interrupts and take a snapshot of the 
     * RX and TX ring states.
     * Enable network interrupts.
     * Display the Rx and Tx rings.
     */
    if (!show_bdrings(idb, sizeof(RXBD_T))) {
	return(TRUE);
    }
    
    return(FALSE);
}
/*
 * =====================================================================
 * quicc_3420_intr_clear
 *
 * Description:
 * 	Clear intr from the 3420
 *
 * Paramters:
 * 	quicc_dpr_t * -	pointer to dual port ram.
 *
 * Returns:
 * 	void - None.
 * =====================================================================
 */
void quicc_3420_intr_clear (quicc_dpr_t *dpr)
{
    dpr->regs.cpic_cisr = QUICC_CIMR_PC1;
}
