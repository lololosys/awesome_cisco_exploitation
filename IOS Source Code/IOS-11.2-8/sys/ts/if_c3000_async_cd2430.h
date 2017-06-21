/* $Id: if_c3000_async_cd2430.h,v 3.1.16.3 1996/08/07 22:24:57 swenson Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_c3000_async_cd2430.h,v $
 *------------------------------------------------------------------
 * if_c3000_async_cd2430.h - Definitions for the the terminal port driver for
 *                the Cirrus CL-CD2430 quadart (for c3000 platform only)
 *
 * May  1996, Samuel Li
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_c3000_async_cd2430.h,v $
 * Revision 3.1.16.3  1996/08/07  22:24:57  swenson
 * CSCdi65232:  crash in print_modemcontrol when show term entered on
 * console
 * Branch: California_branch
 * correct the check to see if a line uses the cd2430
 *
 * Revision 3.1.16.2  1996/07/04  02:13:05  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.16.1  1996/06/17  08:36:02  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:53:52  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __IF_C3000_ASYNC_CD2430_H__
#define __IF_C3000_ASYNC_CD2430_H__

/*
 * per channel private variables for
 * the 2430 driver
 */
typedef struct cd2430_priv_ {
    ulong status;               /* Various status flags */
    int rxbuffs_outstanding;    /* system buffers allocated */
    int rxbuffs_disallowed;     /* buffs disallowed for exceeding max */
    int nested_rx_interrupts;   /* nested receive interrupts */
    int nested_tx_interrupts;   /* nested transmit interrupts */
    int nested_mdm_interrupts;  /* nested modem interrupts */
    ushort led_mask;            /* Mask for the activity LEDs */
} cd2430_priv;

#define CD2430_ASYNC_IS_VALID_TTYNUM(tty) ((tty->ttynum >= cd2430Base) && \
                                    (tty->ttynum < (cd2430Base + cd2430ports)))

#define CD2430_ASYNC_LED_FLASH(cd2430chan_info,tty) \
	{ \
	    if (cd2430_platform_type == PLATFORM_COBRA) \
		/* pls see LSS_LED_ACTIVATE(ctx) */ \
                *(ushort *)(0x2134080) = cd2430chan_info->led_mask; \
	} 

#define CD2430_ASYNC_LED_INIT() \
	{ \
	    cd2430_priv *cd2430chan_info; \
            cd2430chan_info = &(cd2430info_base[unit - cd2430Base]); \
	    cd2430chan_info->led_mask = (1 << (unit - cd2430Base)); \
	}

#define CD2430_ASYNC_TTYNUM(tty) (tty->ttynum-cd2430Base)

#define CD2430_ASYNC_GET_CHANNEL() ((idb->unit - cd2430Base)%CD2430_CHIPCHAN)

#define CD2430_ASYNC_GET_CHIP() ((idb->unit - cd2430Base)/CD2430_CHIPCHAN)

#define CD2430_ASYNC_GET_CSR(chip,csr) \
	{ \
            /* \
             * Point to chip for this bank of channels \
             */ \
            switch (cd2430_platform_type) { \
            case PLATFORM_COPAN: \
                *csr = CD2430ADDRESS(chip); \
                break; \
            case PLATFORM_COBRA: \
                *csr = CD2430ADDRESS_COBRA(chip); \
                break; \
            default: \
                crashdump(0); \
            } \
	} 
	    

/* Prototypes */
extern  void setchan_async(cd2430_quadart *, tt_soc *);

/*
 * Inline function declarations
 *      These are inline functions that are used by async driver code.
 */

/*
 * channel_mode_async()
 *      Report if the channel is currently in async mode.
 * The parameter passed in to this function will be tty->ttynum. This can
 * be used directly to index into the MODEMS[] array, but we will have to
 * decrement it by cd2430Base to index into the channel_info[] array
 * because the information for the CD2430 channels is stored there
 * starting from 0.
 */
static inline boolean
channel_mode_async (tt_soc *tty)
{
    return ((cd2430_channel_info[tty->ttynum - cd2430Base])->aid == MODEMS[tty->ttynum]);
}

#endif /* __IF_C3000_ASYNC_CD2430_H__ */
