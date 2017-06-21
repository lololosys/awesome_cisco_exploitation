/* $Id: init_c1005.h,v 3.2 1995/11/17 18:40:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-36-c1000/init_c1005.h,v $
 *------------------------------------------------------------------
 * init_c1005.h   Catwoman serial driver header
 *
 * May 1995, John Ng
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: init_c1005.h,v $
 * Revision 3.2  1995/11/17  18:40:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:19:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:26:42  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  22:52:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */



#ifndef CATWOMAN_SPECIFIC
#define CATWOMAN_SPECIFIC
/*
 *  These mask values is to determine from the PxDAT ports, in the case
 *  of Catwoman it is located on PCDAT.  Please refer to HW spec
 */
#define CTS_MASK		0x0400
#define DCD_MASK		0x0800
#define RTS_MASK		0x0008
#define DTR_MASK		0x0004
#define DSR_MASK		0x0002
#define CWHS_MASK               (CTS_MASK|DCD_MASK|RTS_MASK|DTR_MASK|DSR_MASK)
/*
 * These are the CIMR mask to turn on/off interrupts
 */
#define QUICC_CIMR_DCD_MASK	QUICC_CIMR_PC11
#define QUICC_CIMR_DSR_MASK	QUICC_CIMR_PC1


/* SCC4 will be use for serial channel */
#define SCC_SERIAL_INTR	QUICC_CIMR_SCC4

#define DCD_INTR_INDEX	0 /* DCD index into global_cpic_intr */
#define DSR_INTR_INDEX	1 /* DSR index into global_cpic_intr */

/*
 * These fields are actually XXX_INTR_INDEX * sizeof(quicc_cpic_interrupt_t)
 */
#define DCD_INTR_OFFSET	0
#define DSR_INTR_OFFSET	16


#ifndef ASMINCLUDE
typedef struct quicc_cpic_interrupt {
    hwidbtype *idb;			/* hwidb for the interface */
    void (*intr_handler)(hwidbtype *);	/* the specific function handler */
    ulong 	cimr_mask;		/* mask for this specific CPIC interrupt */
    volatile ulong *cisr;		/* the cisr register */

} quicc_cpic_interrupt_t;

/*
 * Defines determine which ports are used for CPAI Control/Status
 */



/*----------------------------------------------------------------------*/
/* Internal CPAI Interface Definitions                                  */
/*----------------------------------------------------------------------*/
/* Note: The TCTRL bits are assumed to be contiguous.                   */
/*       Two bits are clocked in by the same clock, that is to say that */
/*       you must write two bits even if you only wanted to change one. */
/*                                                                      */
/*  To write Invert or Loop do the following:                           */
/*       D1 = Invert value                                              */
/*       D0 = Loopback value                                            */
/*       D3 = High                                                      */
/*       Clock D2 from low to high.                                     */
/*                                                                      */
/*  To write Invert or Loop do the following:                           */
/*       D1 = DCD_INVERT value                                          */
/*       D0 = NO_TXCE  value                                            */
/*       D3 = High                                                      */
/*       Clock D4 from low to high.                                     */
/*                                                                      */
/*  To write Sync_off signal do the following:                          */
/*       D1 = low                                                       */
/*       D0 = Sync_off value                                            */
/*       D3 = Low                                                       */
/*       Clock D4 from low to high.                                     */
/*                                                                      */
/* CPAI_TCTRL_IDLE sets both Clks low, both data to 0, and TS3 = 1.     */
/* CPAI_TCTRL_SIDLE same as idle, but sets TS3 = 0, for SYNC_OFF        */
/* CPAI_TCTRL_CLK2  is the mask for CLK2 (LOOP and INVERT latch)        */
/* CPAI_TCTRL_CLK4  is the mask for CLK4 (TXCE, DCD_INV, and SYNC_OFF   */
/*----------------------------------------------------------------------*/

#define CPAI_TCTRL_MASK       0x001f  /* Bits in Port used by TCTRL     */
#define CPAI_TCTRL_SHIFT           0  /* Right shift to right justify   */


#define CPAI_TCTRL_D0         0x0001 << CPAI_TCTRL_SHIFT
#define CPAI_TCTRL_D1         0x0002 << CPAI_TCTRL_SHIFT
#define CPAI_TCTRL_IDLE       0x0008 << CPAI_TCTRL_SHIFT
#define CPAI_TCTRL_SIDLE      0x0000 << CPAI_TCTRL_SHIFT
#define CPAI_TCTRL_CLK2       0x0004 << CPAI_TCTRL_SHIFT
#define CPAI_TCTRL_CLK4       0x0010 << CPAI_TCTRL_SHIFT

#define CPAI_TCTRL_CID_MASK   (CPAI_TCTRL_CLK2 | CPAI_TCTRL_D1 | CPAI_TCTRL_D0)

#define CPAI_TSENSE_MASK      0x0010  /* Bits in Port used for TSENSE   */
#define CPAI_TSENSE_SHIFT          4  /* Right shift (TSENSE0 = D0)     */

#define DEVICE_SERIAL_RXC     0x1000
#define DEVICE_SERIAL_TXC     0x4000

#define CPAI_TCTRL_PORT  (((quicc_dpr_t *) getmbar())->regs.pip_pbdat)
#define CPAI_TSENSE_PORT (((quicc_dpr_t *) getmbar())->regs.pio_padat)
#define CPAI_HSHAKE_PORT (((quicc_dpr_t *) getmbar())->regs.pio_pcdat)
#define CPAI_CLOCK_PORT  (((quicc_dpr_t *) getmbar())->regs.pio_padat)


#define MAX_CPIC_INTR_INDEX	2
quicc_cpic_interrupt_t quicc_cpic_intr_array[MAX_CPIC_INTR_INDEX];

#endif

#endif CATWOMAN_SPECIFIC







