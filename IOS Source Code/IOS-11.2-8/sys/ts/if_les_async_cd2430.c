/* $Id: if_les_async_cd2430.c,v 3.1.16.8 1996/09/10 03:08:38 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/if_les_async_cd2430.c,v $
 *------------------------------------------------------------------
 * if_les_async_cd2430.c - Terminal port driver for Cirrus
 *              CL-CD2430 quadart (common to all platform)
 *
 * May 1996, Samuel Li
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_async_cd2430.c,v $
 * Revision 3.1.16.8  1996/09/10  03:08:38  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.1.16.7  1996/08/28  13:18:03  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.16.6  1996/08/13  02:25:49  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.16.5  1996/08/10  01:08:42  billw
 * CSCdi54603:  make cirrus hardware gotchas more obvious in source code
 * Branch: California_branch
 *
 * Revision 3.1.16.4  1996/06/28  23:31:01  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.16.3  1996/06/18  07:27:42  irfan
 * CSCdi59224:  Split monolithic traffic[] array into per-protocol arrays
 * Branch: California_branch
 * Make the -c- igs and cobra images compile again.
 *
 * Revision 3.1.16.2  1996/06/18  01:49:42  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.1.16.1  1996/06/17  08:36:03  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:53:24  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define APPP_INTERNAL
#include "master.h"
#include "sys_registry.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "../if/network.h"
#include "../wan/serial.h"
#include "ttysrv.h"
#include "logger.h"
#include "../les/msg_cd2430.c"		/* Not a typo, see logger.h */
#include "stacks.h"
#include "subsys.h"
#include "../parser/parser_defs_parser.h"
#include "../os/async.h"
#include "../les/if_les.h"
#include "../les/if_les_common_cd2430.h"
#include "../ts/if_les_async_cd2430.h"
#include "../h/cs_registry.regh"
#include "../wan/ppp.h"
#include "if_async.h"
#include "async_ppp.h"
#include "fastswitch_registry.h"
#include "../os/pool.h"
#include "../os/buffers.h"

/*
 * Local storage
 */
vectortype cd2430_vector;
vectortype cd2430_dummy_vector;
boolean can_ppp;         /* Supports HW async HDLC */
static const char async_str[] =
        "Channel mode is asynchronous serial ";
cd2430_priv *cd2430info_base;   /* Pointer to allocated array of flags */

/*
 * "Basic" interrupt sources we always want to monitor.  This is receive
 * interrupts as well as modem line changes.
 */
#define IER_ASYNC_BASIC (IER_RET | IER_RXD | IER_MDM)

/*
 * cd2430_eoi()
 * Due to a bug in the Cirrus cd2430 chip, writing to an xEOIR register
 * does not always clear the current interrupt.  This routine will issue
 * eoi's until the interrupt has been cleared.
 */
static inline int
cd2430_eoi (cd2430_ro *stk, cd2430_wo *eoir, uchar val, uchar mask)
{
  int i;
  int count = 0;

  *eoir = val;
  for (i = 0; i < 100 && ((*stk & STK_CLVL_MASK) == mask); i++) {
    *eoir = val;
    count = 1;
  }
  return (count);
}

/*
 * cd2430_async_rxgood()
 *	Receive good data interrupt service
 *
 * Should never be entered; this is a DMA driver.  Packet completions and
 * buffer completions both arrive through the RX exception vector.  For
 * purposes of tracking stray interrupts, we tally them under "linenoise".
 */
void
cd2430_async_rxgoodint (void *ttyptr)
{
    cd2430_quadart *csr;
    tt_soc *tty = (tt_soc *)ttyptr;
    cd2430_priv *cd2430chan_info;
    int nested_rx_int = 0;

    csr = (cd2430_quadart *)tty->uartdata;
    cd2430chan_info = &cd2430info_base[CD2430_ASYNC_TTYNUM(tty)];
 
    /*
     * Light up the activity LED if this is a Cobra
     */
    CD2430_ASYNC_LED_FLASH(cd2430chan_info,tty);

    tty->linenoise += 1;
    /*
     * Issue receive eoi to controller.
     */
    nested_rx_int = cd2430_eoi(&csr->stk, &csr->reoir, REOIR_NOTRANSF,
                                STK_RX_CLVL);
    cd2430chan_info->nested_rx_interrupts += nested_rx_int;
}

/*
 * inpak_done()
 *	A packet is finished, free to appropriate pool
 */
inline static void
inpak_done (tt_soc *tty, paktype *pak)
{
    if (pak->pool == CD2430_POOL) {
	enqueue(&tty->rxpakq, pak);
    } else {

	int *buffs_allocated = 
	    &(cd2430info_base[CD2430_ASYNC_TTYNUM(tty)].rxbuffs_outstanding);

	datagram_done(pak);
	/*
	 * Do un-accounting of this buffer against this tty
	 *  We will decrement if this isn't HW PPP mode
	 */
	if ((!(tty->statbits & PPPMODE)) && (*buffs_allocated)) {
	    *buffs_allocated -= 1;
	}
    }
}

/*
 * tty_rxptrs()
 *	Fill in tty input pointers to use the given packet's data area
 *
 * This routine assumes that (1) we use the whole packet data area, and
 * (2) that we have placed the received size in datagramsize.
 */
inline static void
tty_rxptrs (tt_soc *tty, paktype *pak)
{
    tty->inheadpak = pak;
    tty->inhead = tty->intail = tty->instart = pak->datagramstart;
    tty->inend = tty->instart + pak->datagramsize;
    tty->incount = pak->datagramsize;
}

/*
 * flow_control()
 *	Apply flow control to named line
 *
 * HW flow control is done by the chip, so only SW is honored here
 *
 * Special note on Hardware flow control:
 *
 * The cd2430 is capable of supporting HW flow control without CPU 
 * intervention.  It will stop outputting when the CTS pin goes false,
 * and will de-assert DTR if internal buffers fill up for some reason.
 * Note that most external devices expect *RTS* to be deasserted for
 * flow control purposes.  For this reason, cisco HW uses the 2430 pin
 * labeled DTR as the RTS signal in external connectors, and moves the
 * external DTR signal is connected to the RTS chip lead.  Thus, within
 * the source code and for all practical purposes, 
 *     **** REFERENCES TO RTS AND DTR are SWAPPED ******
 */
static inline void
flow_control (tt_soc *tty, cd2430_quadart *csr)
{
    /*
     * Software flow control (special char 2 is XOFF)
     */
    if (tty->capabilities & SFLOWOUT) {
	if ((csr->csr & CSR_RXFLOFF) == 0) {
	    csr->stcr = STCR_SNDSPEC|STCR_SPC2;
	    tty->statbits |= SENTXOFF;
	}
    }
}

/*
 * tty_queue_receive_pak
 *      Queue receive pak either to the active input data (inhead/tail) 
 *      or, if that's still pointing to unconsumed data, to the rxdataq
 */
inline static void
tty_queue_receive_pak (tt_soc *tty, paktype *pak)
{
    if (tty->inheadpak == NULL) {
	tty_rxptrs(tty, pak);
    } else {
	enqueue(&tty->rxdataq, pak);
    }
}

/*
 * async_alloc_rx_buffers()
 *	Allocate A/B if idle & buffers are available
 *
 * This routine is complex as it has to handle the case of buffers not
 * being available, as well as the case of buffers having been unavailable
 * in the past.  For the latter, it may find a need to arm both A and B
 * before returning.
 */
void
async_alloc_rx_buffers (tt_soc *tty, cd2430_quadart *csr, int idx)
{
    paktype *newpak;
    pppContext *ctx = tty->appp_context;
    uint size;

    /*
     * Start with whichever buffer the 2400 wants next, and walk forward
     * (that is, to the other one) to fill both if needed.
     */
    for ( ; ; idx ^= 1) {
	/*
	 * Consider status of next buffer.  If it's still owned by the
	 * 2400, or has not been processed, return (we've filled as far
	 * as possible).
	 */
	if (tty->inpak[idx]) {
	    break;
	}

	/*
	 * Try to allocate a packet
	 */
	if (tty->statbits & PPPMODE) {
	    /*
	     * Ask async PPP to grab us our next receive packet.  Take
	     * it from PPP and consume it for DMA instead.
             * The size (to be passed on to firmware) is prepared:
             *  - to include the FCS size since firmware takes 'size'                       
             *    to also include the FCS size.
             *  - to include 2 bytes (even-aligned) to offset the
             *    off-by-one known bug in firmware. Obviously this will
             *    be taken off when the firmware is (ever) corrected.
             *  Note that the size of the pak itself, although it does
             *  not have these additional bytes tacked on, should not
             *  result in DMA access violation since the pak itself has
             *  'slush' always built-in to the size to be allocated
             *  (TRAILBYTES).
	     */
	    (void)appp_new_inpak(ctx);
	    newpak = ctx->inpak;
	    if (newpak) {
		ctx->inpak = NULL;
		newpak->length = ctx->imax;
                size = ctx->imax + PPP_FCS_SIZE + 2;
	     } else {
		 size = 0;
	     }
	} else {
	    /*
	     * Try our private pool, then the public one
	     */
	    newpak = dequeue(&tty->rxpakq);
	    if (NULL == newpak) {

		cd2430_priv *cd2430chan_info =
		    &(cd2430info_base[CD2430_ASYNC_TTYNUM(tty)]);

		if (cd2430chan_info->rxbuffs_outstanding <
		    CD2430_MAX_SYS_BUFFS) {
		    newpak = getbuffer(CD2430_PAKSIZE);
		    if (newpak) {
			newpak->length = CD2430_PAKSIZE;
			cd2430chan_info->rxbuffs_outstanding += 1;
		    }
		} else {
		    /* record buffer limitation */
		    cd2430chan_info->rxbuffs_disallowed++;
		}
	    }
	    size = CD2430_PAKSIZE;
	}

	/*
	 * Found one.  Set it up for DMA, and record its identity so we
	 * can get the pak pointer back later.
	 */
	if (newpak != NULL) {
	    tty->inpak[idx] = newpak;
	    arm_rx_dma(csr, idx, VTOP(newpak->datagramstart), size);
	} else {
	    /*
	     * Apply flow control when our ability to arm DMAs is
	     * hindered.
	     */
	    if (tty->rxpakq.qhead == NULL) {
		flow_control(tty, csr);
	    }

	    /*
	     * Log buffer shortage
	     */
	    tty->overflow += 1;

	    /*
	     * Log buffer miss if associated with IDB
	     */
	    if (ctx && ctx->idb) {
		ctx->idb->counters.output_nobuffers+= 1;
	    }

	    /*
	     * Get out of here
	     */
	    break;
	}
    }
}

/*
 * cd2430_async_rxint()
 *	Handle receive data exceptions
 *
 * This routine handles exceptional receive data conditions.  For this driver,
 * this is the main code path for DMA read completions.  Completions will
 * be caused by either the inter-character timer, or by an entire buffer
 * having filled.
 */
void
cd2430_async_rxint (void *ttyptr)
{
    cd2430_quadart *csr;
    tt_soc *tty = (tt_soc *)ttyptr;
    ushort curbuf, status, bufstatus;
    ushort count = 0;
    uchar reoir;
    ulong statbits;
    pppContext *ctx;
    boolean received_break = FALSE;
    cd2430_priv *cd2430chan_info;         /* Local status flags */
    int nested_rx_int = 0;

    csr = (cd2430_quadart *)tty->uartdata;
    ctx = tty->appp_context;
    cd2430chan_info = &cd2430info_base[CD2430_ASYNC_TTYNUM(tty)];

    /*
     * Light up the activity LED if this is a Cobra
     */
    CD2430_ASYNC_LED_FLASH(cd2430chan_info,tty);

    /*
     * Handle data first, then exceptions.  RISR tells us which buffer
     * is completing now.
     */
    statbits = tty->statbits;
    status = csr->risr;
    curbuf = (status & RISR_BUFB) ? 1 : 0;
    bufstatus = curbuf ? csr->brbsts : csr->arbsts;
    reoir = 0;

    /*
     * Restore real rtpr value if we overrode it to control DMA
     * latency
     */
    if (tty->short_rtpr) {
	csr->rtpr = tty->cache_rtpr;
	tty->short_rtpr = FALSE;
    }

    /*
     * check for TO first. All other status is invalid if TO set
     */

    /*
     * Force the buffer to be treated as a completion on receive timeout.
     * We only do this for timeouts where there is not already incoming
     * data in the input queue.  Or for rxservice--there is no queue for
     * us to examine.
     */


    if (status & RISR_TO) {
	if (cd2430chan_info->status & SEEINGBREAK) {
	    /*
	     * End-of-Break Condition is a TO right after ISR_BRK
	     * On a 2430 there is considerable delay b/w the
	     * Break-seen and End-of-break-seen. During this time,
	     * some CCR commands cause loss of data.
	     */
	    received_break = TRUE;
	    cd2430chan_info->status &= ~SEEINGBREAK;
	    /*
	     * Do not waste another buffer, if previous break
	     *  yet unnoticed. Also check to see if we do actually
	     *  have a buffer.
	     */
	    if ((tty->inpak[curbuf]) &&
		!(statbits & (LASTWASBREAK | ESCAPED))) {
		reoir = REOIR_TERM;
		bufstatus |= RBSTS_EOB;
	    }
	} else if (!(bufstatus & RBSTS_EOB)) {
	    if (tty->rxservice ||
		((tty->incount == 0) && (tty->rxdataq.qhead == NULL))) {
		reoir = REOIR_TERM;
		bufstatus |= RBSTS_EOB;
	    } else {
		/*
		 * Otherwise start the inter-character timer running for an
		 * interval twice the normal.  This more or less simulates
		 * retrying the inter-character idle timeout in the future.
		 */
		csr->gt1 = csr->rtpr * 2;
	    }
	}
    } else if (status & RISR_BRK) {
	cd2430chan_info->status |= SEEINGBREAK;
    }

    /*
     * If the buffer's done, feed in any bytes it has
     */
    if (bufstatus & RBSTS_EOB) {
	paktype *pak;

	/*
	 * Take completed packet off tt_soc.  Mark slot open.
	 */
	pak = tty->inpak[curbuf];
	tty->inpak[curbuf] = NULL;

	/*
	 * If bad packet, discard
	 */
	if ((statbits & PPPMODE) && (status & (RISR_CRC | RISR_RXABT))) {
	    /*
	     * Count bad frames, log as needed
	     */
	    if (async_framing_debug) {
		buginf("\nAPPP%t: CRC error", tty->ttynum);
	    }
	    if (ctx->idb) {
		ctx->idb->counters.input_crc++;
		slip_traffic.crcerr++;
	    }

	} else {
	    /*
	     * Figure out number of bytes in packet.  If the buffer's not
	     * done we have to use the DMA address, otherwise use the
	     * finishing count.
	     */
	    if (status & RISR_EOB) {
		count = (curbuf ? csr->brbcnt : csr->arbcnt);
		if (statbits & PPPMODE) {
		    /*
		     * If we got the whole buffer, but not end-of-frame,
		     * tally it as a giant.
		     */
		    if ((status & RISR_EOF) == 0) {
			if (!ctx->ingiant) {
			    if (async_framing_debug) {
				buginf("\nAPPP%t: Input packet too large",
				    tty->ttynum);
			    }
			    if (ctx->idb) {
				ctx->idb->counters.giants+= 1;
			    }
			    appp_rcv_error(tty);
			    ctx->ingiant = TRUE;
			}
			count = 0;

		    /*
		     * Otherwise, we're in the middle of a giant, and have
		     * now seen the end of the frame.  Discard the last
		     * part and prepare for the next frame.
		     */
		    } else if (ctx->ingiant) {
			count = 0;
			ctx->ingiant = FALSE;

		    /*
		     * Otherwise, it's good data; trim the CRC off the
		     * end and get ready to use it.
		     */
		    } else {
			count -= 2;
		    }
		}
	    } else {
		count = (pak) ? (((csr->rcbadru << 16) | csr->rcbadrl) -
				 VTOP(pak->datagramstart)) : 0;
	    }
	}

	/*
	 * Allocate new buffer(s) now that we've used the old
	 * [ab]rbcnt field.
	 */
	async_alloc_rx_buffers(tty, csr, curbuf);
 
	/*
	 * Tell the 2402 that it can go on.  From here, we don't talk to it,
	 * and in receive exceptions DMA can be on hold until we complete the
	 * interrupt.
	 */
	nested_rx_int = cd2430_eoi(&csr->stk, &csr->reoir,
                                  reoir | REOIR_NOTRANSF, STK_RX_CLVL);
        cd2430chan_info->nested_rx_interrupts += nested_rx_int;

	/*
	 * If there's data, consume it.
	 */
	if ((!pak) || (count > pak->length)) {
	    /* bogus information back from the chip.  Ignore this packet */
	    count = 0;
	} else if (count > 0) {
	    pak->datagramsize = count;
	    if (statbits & PPPMODE) {
		/*
		 * PPP mode.  Fiddle packet pointers to reflect
		 * received encapsulation size.  Pass up.
                 * Since Xremote does not use PPP encaps, don't
                 * parse packet.
		 */
                if (!tty->xrcb)
		   (void)ppp_parse_packet(tty->tty_idb, pak, NULL);
		(*(ctx->iqueue))(pak, tty);
		pak = NULL;

	    } else if (tty->rxservice) {
		(*(tty->rxservice))(tty, pak->datagramstart, count);
	    } else {

		/*
		 * Non-rxservice might need to process escapes, hold
		 * characters, and so forth out of the byte stream.  We
		 * do this, overwriting the buffer in place.
		 */
		if (statbits & PSIENA) {
		    uchar *src, *dest;

		    src = dest = pak->datagramstart;
		    while (count-- > 0) {
			int c;

			/*
			 * Filter out non-data characters by walking
			 * buffer and rewriting data sequentially.
			 */
			c = *src++;
			if (special_chars(tty, c) >= 0) {
			    *dest++ = c;
			}
		    }
		    count = pak->datagramsize =
			dest - (uchar *)pak->datagramstart;
		}

		/*
		 * If there's still data, queue it to the tty 
		 */
		if (count > 0){ 
                   tty_queue_receive_pak(tty, pak);
                   pak = NULL;
		}
	    }
	} else {
            /*
             * Buffer is done but Count = 0
             * If completed receiving a break, stick a break character
	     * to the input stream if it is non-rxservice and non-PPPMODE
             */
            if (received_break && (!tty->rxservice) &&
		!(statbits & PPPMODE)) {
		special_chars(tty, BREAK_CHAR);
                *pak->datagramstart = 0x0;
                pak->datagramsize = 1;
                tty_queue_receive_pak(tty, pak);
                pak = NULL;
	    }
        }

	/*
	 * Release the packet
	 */
	if (pak) {
	    inpak_done(tty, pak);
	}
    } else {
	/*
	 * Complete the interrupt; no data motion
	 */
        nested_rx_int = cd2430_eoi(&csr->stk, &csr->reoir,
                                  reoir | REOIR_NOTRANSF, STK_RX_CLVL);
        cd2430chan_info->nested_rx_interrupts += nested_rx_int;
    }

    /*
     * Tally overruns
     */
    if (status & RISR_OE) {
	tty->overrun += 1;
    }
    if (status & (RISR_FE | RISR_PE)) {
	tty->linenoise += 1;
    }
}

/*
 * arm_tx_dma()
 *	Arm transmit DMA to CD2430
 *
 * Reads the DMA status register to find out which buffer slot (A or B) is
 * to be used next, and then programs the buffer into that slot and enables
 * DMA (i.e., sets ownership to the 24XX) for it.
 */
inline static void
arm_tx_dma (cd2430_quadart *csr, ulong addr, uint count, uint options)
{
    if (csr->dmabsts & DMABSTS_NTBUF) {
	csr->btbadrl = CD2430LO(addr);
	csr->btbadru = CD2430HI(addr);
	csr->btbcnt = count;
	csr->btbsts = TBSTS_INTR | TBSTS_24OWN | TBSTS_EOF | options;
    } else {
	csr->atbadrl = CD2430LO(addr);
	csr->atbadru = CD2430HI(addr);
	csr->atbcnt = count;
	csr->atbsts = TBSTS_INTR | TBSTS_24OWN | TBSTS_EOF | options;
    }
}

/*
 * tty_txptrs()
 *	Fill in tty output pointers to use the given packet's data area
 *
 * Data is assumed to be placed starting at datagramstart, and pak->length
 * is assumed to hold the amount of space available in the buffer for
 * transmit data to be enqueued.
 */
inline static void
tty_txptrs (tt_soc *tty, paktype *pak)
{
    tty->outhead = tty->outtail = tty->outstart = pak->datagramstart-1;
    tty->outend = tty->outstart + pak->length;
    tty->outsize = pak->length;
    tty->outcount = 0;
}

/*
 * outpak_done()
 *	Output packet finished, free to right pool
 */
inline static void
outpak_done (tt_soc *tty, paktype *pak)
{
    if (pak->pool == CD2430_POOL) {
	enqueue(&tty->txpakq, pak);
    } else {
	datagram_done(pak);
	tty->outtail = tty->outstart + tty->outcount; /* reset outtail */
    }
}

/*
 * cd2430_async_txint()
 *	Handle a transmit completion interrupt
 *
 * TBD: what if it's not done, but it's something exceptional?  Looks like
 * it would have to be something pretty dire, like bus error.
 */
void
cd2430_async_txint (void *ttyptr)
{
    cd2430_quadart *csr;
    tt_soc *tty = (tt_soc *)ttyptr;
    cd2430_priv *cd2430chan_info;
    int count;
    paktype *pak = NULL, *donepak;
    uint opts = 0;
    int nested_tx_int = 0;

    /*
     * Get TTY base pointer
     */
    csr = (cd2430_quadart *)tty->uartdata;
    cd2430chan_info = &cd2430info_base[CD2430_ASYNC_TTYNUM(tty)];
 
    /*
     * Light up the activity LED if this is a Cobra
     */
    CD2430_ASYNC_LED_FLASH(cd2430chan_info,tty);

    /*
     * If buffer done, set up for next
     */
    if (csr->tisr & (TISR_EOF | TISR_EOB)) {
	/*
	 * Record which buffer completed, flag as not the output
	 * packet any more.
	 */
	donepak = tty->outpak;
	tty->outpak = NULL;
    } else {
	/*
	 * If the buffer's not done, and there IS a buffer being sent,
	 * bail out without trying to arm a new one.
	 */
	if (tty->outpak) {
	    csr->ier &= ~(IER_TXMPTY);
	    /*
             * Issue transmit eoi to controller.
             */
            nested_tx_int = cd2430_eoi(&csr->stk, &csr->teoir, TEOIR_NOTRANSF,
                                       STK_TX_CLVL);
            cd2430chan_info->nested_tx_interrupts += nested_tx_int;
	    
	    return;
	}
	donepak = NULL;
    }

    /*
     * Always clear TXMPTY; we are mostly driven off packet completions,
     * except for starting transmission from an idle state.
     */
    if (donepak == NULL) {
	csr->ier &= ~(IER_TXMPTY);
    }

    /*
     * If PPP mode, pull packets from PPP queue directly
     */
    if (tty->statbits & PPPMODE) {
	pppContext *ctx = tty->appp_context;

	/*
	 * Get next queued packet
	 */
	pak = appp_get_oqueue(ctx);

	if (pak) {
	    /*
	     * LCP packets always transmit with the all-1's, most conservative
	     * escape map.
	     */
	    if (pak->flags & PAK_CONTROL) {
		opts |= TBSTS_MAP32;
	    }

	    /*
	     * Hold state as APPP_FRAME_END to keep appp_startoutput from
	     * thinking we need to be restarted.
	     */
	    ctx->outstate = APPP_FRAME_END;

	} else {
	    /*
	     * Indicate that we would need to be restarted next time
	     */
	    ctx->outstate = APPP_IDLE;
	}

    /*
     * If the BREAK handling machinery is active, do this in preference
     * to other work.  Note that breaks can not happen during PPP mode.
     */
    } else if (tty->statbits & (SENDBREAK|SENDINGBREAK)) {
	/*
	 * Break generation works as a state machine.  The complexity is
	 * because the ETC mode and the processing of the embedded commands
	 * operate asynchronous of each other.  Bug in the chip, IMHO, but
	 * we have to live with it.
	 *
	 * States:
	 * SENDBREAK !SENDINGBREAK !break_pak:
	 *	Set ETC, queue the break packet
	 * SENDBREAK  SENDINGBREAK  break_pak:
	 *	Break packet has been sent, need to wait for transmit empty
	 * SENDBREAK  SENDINGBREAK !break_pak:
	 *	Break all sent, clear ETC, life continues
	 */
	if (!(tty->statbits & SENDINGBREAK)) {
	    /*
	     * If we are not yet sending break, put the line into embedded
	     * command mode, and arrange for the next packet to contain
	     * the embedded commands necessary to send a break.
	     */
	    csr->cor2 |= COR2_ETC;
	    tty->statbits |= SENDINGBREAK;
	    pak = break_pak;
	} else {
	    if (donepak == break_pak) {
		/*
		 * Packet data queued, now wait for FIFO to empty
		 * entirely.  We bail out at this point, as we don't
		 * want to let the driver try to queue new data.
		 */
		csr->ier |= IER_TXMPTY;
		/*
                 * Issue transmit eoi.
                 */
                nested_tx_int = cd2430_eoi(&csr->stk, &csr->teoir,
					   TEOIR_NOTRANSF, STK_TX_CLVL);
                cd2430chan_info->nested_tx_interrupts += nested_tx_int;

		return;
	    } else {
		/*
		 * Done sending the break, so leave embedded command
		 * mode.
		 */
		csr->cor2 &= ~COR2_ETC;
		tty->statbits &= ~(SENDBREAK|SENDINGBREAK);
	    }

	    /*
	     * Don't let the break packet be cleaned up; it's a static
	     * resource.
	     */
	    donepak = NULL;
	}
    }

    /*
     * If we have a break or PPP packets, they take priority.
     * Otherwise see about getting the next packet of data to send.
     */
    if (pak == NULL) {
	if (tty->txservice) {
	    /*
	     * Txservice is handled here by pulling bytes from it and
	     * queueing to the normal TTY output buffer.  With all the
	     * bytes lined up in that buffer, we can start a simple DMA.
	     *
	     * Note that outtail is advanced before storing a byte.
	     */
	    count = (*(tty->txservice))(tty, tty->outtail + 1,
		tty->outend - tty->outtail);
	    tty->outcount += count;
	    tty->outtail += count;
	}

	/*
	 * At this point, outhead+1..outtail describe a range of
	 * memory which should be transmitted.  This memory has
	 * either been filled in with putc()'s elsewhere, or filled
	 * from our txservice right above; we don't care, it's just
	 * data from here.
	 */
	if (tty->outcount > 0) {
	    pak = dequeue(&tty->txpakq);
	    pak->datagramsize = tty->outcount;
	    tty->outstart = NULL;	/* Flag that we've taken it */
	}
    }

    /*
     * If we have data to send, fire up DMA
     */
    if (pak) {
        if (tty->ispy) {
            (*tty->ispy)(tty, pak->datagramstart, pak->datagramsize);
          }
	arm_tx_dma(csr, VTOP(pak->datagramstart), pak->datagramsize, opts);
	tty->outpak = pak;
    }

    /*
     * Take care of previous packet.  If it's one of our "private reserve",
     * place it back on the queue, otherwise datagram_done() it.
     *
     * We waited until here to free the packet so that it may conveniently
     * become the target of outhead/tail/etc. in the following code.
     */
    if (donepak) {
	outpak_done(tty, donepak);
    }

    /*
     * If we removed the previous packet serving as outhead/tail/etc, set
     * up a new one.
     */
    if (tty->outstart == NULL) {
	pak = (paktype *)tty->txpakq.qhead;
	tty_txptrs(tty, pak);
    }
    /*
     * Issue eoi to controller.
     */
    nested_tx_int = cd2430_eoi(&csr->stk, &csr->teoir,
                                TEOIR_NOTRANSF, STK_TX_CLVL);
    cd2430chan_info->nested_tx_interrupts += nested_tx_int;
}

/*
 * cd2430_async_modint()
 *	Handle modem state change interrupt
 *
 * Clear the modem interrupt, but take no action.  We handle modem line
 * changes using busy-polling.
 */
void
cd2430_async_modint (void *ttyptr)
{
    cd2430_quadart *csr;
    tt_soc *tty = (tt_soc *)ttyptr;
    cd2430_priv *cd2430chan_info;
    uchar misr;
    int nested_mdm_int = 0;

    /*
     * Get right register set, record which lines changed
     */
    csr = (cd2430_quadart *)tty->uartdata;
    cd2430chan_info = &cd2430info_base[CD2430_ASYNC_TTYNUM(tty)];
 
    /*
     * Light up the activity LED if this is a Cobra
     */
    CD2430_ASYNC_LED_FLASH(cd2430chan_info,tty)

    misr = csr->misr;
    if (misr & MISR_DSR) {
	tty->modem_changes.ring = TRUE;
    }
    if (misr & MISR_CTS) {
	tty->modem_changes.cts = TRUE;
    }
    /*
     * All done, Issue eoi to controller.
     */
    nested_mdm_int = cd2430_eoi(&csr->stk, &csr->meoir,
                                0, STK_MODEM_CLVL);
    cd2430chan_info->nested_mdm_interrupts += nested_mdm_int;
}

/*
 *			Utility routines
 */

/*
 * cd2430_setcor()
 *	Set channel option registers
 *
 * This routine sets the channel option registers to match the values
 * in the tty structure. The parameters managed by this are:
 *
 *	Parity
 *	Databits
 *	Stopbits
 *	Hardware flow control
 *	Software flow control
 */
static void
cd2430_setcor (tt_soc *tty)
{
    leveltype level;

    uchar options1 = 0,
	options2 = 0,
	options3 = 0,
	options4 = 0,
	options5 = 0,
	options6 = 0,
	options7 = 0;
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;

    /*
     * COR1 options
     */
    switch (tty->tty_parity) {			/* Parity */
    case PARITY_NO:
	options1 |= COR1_PARITY_NONE;
	break;

    case PARITY_ODD:
	options1 |= COR1_PARITY_ODD|COR1_PARITY_NORM;
	break;

    case PARITY_EVEN:
	options1 |= COR1_PARITY_NORM;
	break;

    case PARITY_SPACE:
	options1 |= COR1_PARITY_FORCE;
	break;

    case PARITY_MARK:
	options1 |= COR1_PARITY_FORCE|COR1_PARITY_ODD;
	break;
    }
    options1 |= COR1_PARITY_IGN;

    switch (tty->tty_databits) {		/* Data bits */
    case 5:
	options1 |= COR1_5BIT;
	break;

    case 6:
	options1 |= COR1_6BIT;
	break;

    case 7:
	options1 |= COR1_7BIT;
	break;

    case 8:
	options1 |= COR1_8BIT;
	break;
    }

    /*
     * COR2 options (and COR3, where appropriate)
     */
    if (tty->capabilities & SFLOWIN) {		/* Software flow control */
	options2 |= COR2_TXINBAND;
	if (!(tty->statbits & PPPMODE)) {
	    options3 |= COR3_FCT | COR3_SCDE;
	}
    }
    if (tty->capabilities & HFLOWIN) {		/* Hardware flow control */
	options2 |= COR2_CTS;
    }

    /*
     * COR3 options
     */
    if (!(tty->statbits & PPPMODE)) {
	switch (tty->tty_stopbits) {		/* Stop bits */
	case STOPBITS1:
	    options3 |= COR3_1STOP;
	    break;

	case STOPBITS15:
	    options3 |= COR3_15STOP;
	    break;

	case STOPBITS2:
	    options3 |= COR3_2STOP;
	    break;
	}
    } else {
	options3 |= COR3_TXGEN | COR3_RXCK | COR3_CRCPASS;
    }

    /*
     * COR4 options
     */
    options4 = COR4_RXTHRESH(tty->tty_rxspeed); /* Set the input threshold */
    if (options4 < 2) {
	options4 = 2;
    }
    if (options4 > 12) {
	options4 = 12;
    }
    options4 |= COR4_DSRZD;
    if (!(tty->capabilities & HFLOWIN)) {
	options4 |= COR4_CTSZD;
    }

    /*
     * COR5 options
     */
    if (tty->capabilities & HFLOWOUT) {
	options5 |= (CD2430_BUFSIZE - 4);
    }
    options5 |= COR5_DSROD;
    if (!(tty->capabilities & HFLOWIN)) {
	options5 |= COR5_CTSOD;
    }

    level = raise_interrupt_level(TTY_DISABLE);
    /*
     * Select the channel
     * disable tx and rx for a while when changing parameters
     * note: don't reset/realloc buffers here
     */
    setchan_async(csr, tty);
    sendccr(csr, CCR_DISTX | CCR_DISRX);

    /*
     * Set the special characters
     */
    if (!(tty->statbits & PPPMODE)) {
	csr->schr1 = tty->tty_startchar;
	csr->schr2 = tty->tty_stopchar;
    }

    /*
     * Set the channel option registers
     */
    csr->cor1 = options1;	/* Set the option register 1 */
    csr->cor2 = options2;	/* Set the option register 2 */
    csr->cor3 = options3;	/* Set the option register 3 */
    csr->cor4 = options4;	/* Set the option register 4 */
    csr->cor5 = options5;	/* Set the option register 5 */
    csr->cor6 = options6;	/* Set the option register 6 */
    csr->cor7 = options7;	/* Set the option register 7 */

    /*
     * Set prescaler period
     */
    csr->tpr = CD2430_PRESCALE;

    /*
     * Tell 24XX to recalculate its internal tables and
     *      to re-enable the xmitter and receiver
     */
    sendccr(csr, CCR_INITCH | CCR_ENTX | CCR_ENRX);

    /*
     * Always start with "clear to send"
     */
    csr->msvr_dtr = MSVR_DTR;
    reset_interrupt_level(level);
}

/*
 * cd2430_putpkt()
 *	Output data stored in a packet buffer
 *
 * This routine is used to output data stored in a packet buffer.
 */
static int
cd2430_putpkt (tt_soc *tty, paktype *pak, uchar *datastart, int length)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    leveltype level;

    /*
     * If the channel is not in async mode, simply discard the packet.
     */
    if (!channel_mode_async(tty))
        outpak_done(tty, pak);
    
    /*
     * If padding needs to be added, do it as a byte stream, and stuff
     * into outhead/tail/etc.
     */
    if (tty->current.padding_ptr) {
	return(generic_putpkt(tty, pak, datastart, length));
    }

    /*
     * If a packet's going out, tell them to wait.  this is the "quick"
     * check; we'll check again with interrupts masked.
     */
    if (tty->outpak) {
	return(0);
    }

    /*
     * Access this TTY.  If we're not currently allowed to transmit,
     * tell them to wait.
     */
    if (tty->statbits & HOLDING) {
	return(0);			/* Don't output if holding */
    }

    /*
     * If we notice a byte stream of output is pending, wait until
     * it has cleared.  Note tty_dallytimer() will not wait forever.
     */
    if (tty->outcount != 0) {
	tty_dallytimer(tty);
	return(0);
    }

    /*
     * If we're wrapping up, just lie and say it's all taken care of.
     */
    if (tty->statbits & (CARDROP|IDLE)) {
	return(length);
    }

    /*
     * Place a lock on the packet, and mark it as our current output
     * packet.  Start DMA by handing the data to the 24XX.
     */
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    if (tty->outpak == NULL) {
	pak_lock(pak);
	tty->outpak = pak;
	tty->outtail = NULL;	/* Indicate a putpkt in progress */
        if (tty->ispy) {
            (*tty->ispy)(tty, datastart, length);
          }
	arm_tx_dma(csr, VTOP(datastart), length, 0);

	/*
	 * Enable transmit interrupts so we'll hear about the
	 * completion.  tty_dallytimer() will watch for hung transmits.
	 */
	csr->ier |= (IER_TXD);
    } else {
	/*
	 * Raced with a transmit interrupt, probably a TXMPTY on the
	 * FIFO emptying.
	 */
	length = 0;
    }
    reset_interrupt_level(level);

    /*
     * If we in fact started the transmit, start a timer.  Return
     * result.
     */
    if (length) {
	tty_dallytimer(tty);
    }
    return(length);
}

/*
 * cd2430_startinput()
 *	Resume pending input
 */
static void
cd2430_startinput (tt_soc *tty)
{
    cd2430_quadart *csr;
    leveltype level;

    csr = (cd2430_quadart *)tty->uartdata;
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);

    /*
     * If we've held off the other side, request him to start now
     * STCR_SC1 is the XON character
     */
    if (csr->csr & CSR_RXFLOFF) {
	csr->stcr = STCR_SNDSPEC|STCR_SPC1;
    }
    csr->ier |= IER_ASYNC_BASIC;	/* Enable basic interrupts */
    tty->statbits &= ~STOPPEDINPUT;	/* Input isn't stopped */
    reset_interrupt_level(level);
}

/*
 * cd2430_startoutput()
 *	Start up pending output
 *
 * This routine can be called either in interrupt state or process state.
 * If we are called from interrupt state, we must not touch the channel
 * access register. We can only be called in interrupt state for an
 * interrupting line.
 */
static boolean
cd2430_startoutput (tt_soc *tty)
{
    cd2430_quadart *csr;
    leveltype level;

    /*
     * Return false if the channel is not in async mode.
     */
    if (!channel_mode_async(tty))
        return (FALSE);

    /*
     * Don't do anything if our user has stopped us from sending
     */
    if ((tty->statbits & HOLDING) == 0) {

	/*
	 * Point to our particular device's registers.  If in interrupt
	 * state our channel is selected for us, otherwise select it
	 * explicitly.
	 */
	csr = (cd2430_quadart *)tty->uartdata;
	if (!cd2430_istate) {
	    level = raise_interrupt_level(TTY_DISABLE);
	} else {
	    level = (leveltype)0;		/* keep compiler quiet */
	}
	setchan_async(csr, tty);

	/*
	 * Allow our channel to interrupt on data completions
	 */
	if (tty->outpak == NULL) {
	    csr->ier |= (IER_TXD | IER_TXMPTY);
	} else {
	    csr->ier |= IER_TXD;
	}

	/*
	 * Restore interrupts if not running in interrupt handler
	 */
	if (!cd2430_istate) {
	    reset_interrupt_level(level);
	}
    }
    return(TRUE);		/* Say it worked */
}

/*
 * cd2430_outputxoffed()
 *	Is CD2430 Output Xoffed?
 *
 * Checks for both HW assisted and regular SW flow control
 */
static boolean
cd2430_outputxoffed (tt_soc *tty)
{
    cd2430_quadart *csr;
    leveltype level;
    boolean result;

    csr = (cd2430_quadart *)(tty->uartdata);
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    result = ((tty->statbits & WANTXON) || (csr->csr & CSR_TXFLOFF));
    reset_interrupt_level(level);
    return(result);
}

/*
 * get_clock_async()
 *	Given baud rate, calculate divisor and period
 *
 * There can be multiple solutions; we return the closest.  Copan will be
 * showing up at various clock rates, thus we do the calculation instead
 * of using a table.
 *
 * Returns FALSE if no solution found, TRUE otherwise.
 */
static boolean
get_clock_async (uint baud, uchar *clkp, uchar *pdp)
{
    uint maxdx = 999999, clk;
    uint x, matches = 0;

    /*
     * For each possible divisor
     */
    for (x = 0, clk = cd2430clock/8; x < 5; ++x, clk >>= 2) {
	uint div, mod, tmpbaud, dx;

	/*
	 * Get nearest solution.  If the remainder is more than half way
	 * up the baud, we round up.  Floating point is for wimps.
	 */
	div = clk / baud;
	if ((div > 0xFF) || (div <= 0x01)) {	/* No solution in this range */
	    continue;
	}
	mod = clk % baud;
	if (mod >= (baud >> 1)) {
	    if (div < 0xFF) {
		div += 1;
	    } else {
		continue;
	    }
	}

	/*
	 * Figure out true baud rate, and record our parameters if this
	 * is the closest match yet.
	 */
	tmpbaud = clk / div;
	if (tmpbaud > baud) {
	    dx = tmpbaud - baud;
	} else {
	    dx = baud - tmpbaud;
	}
	if (dx < maxdx) {
	    matches += 1;
	    *clkp = x;
	    *pdp = div - 1;
	    maxdx = dx;
	}
    }
    return(matches != 0);
}

/*
 * calc_rtpr()
 *	Given baud rate, return right value for rtpr
 */
static uint
calc_rtpr (int baud)
{
    uint x;
    const uint target_char_delay = 6;

    x = (target_char_delay *
    	((cd2430clock/2048)/CD2430_PRESCALE)) / (baud/10);
    if (x > 254) {
	x = 254;
    } else if (x < 2) {
	x = 2;
    }
    return(x);
}

/*
 * reset_rx_buffers()
 *	Reset the receive side of TTY buffering
 */
static void
reset_rx_buffers (tt_soc *tty)
{
    /*
     * Free up any armed receive packets.  Note that both PPP and async
     * stream mode organize packets the same way, so this cleanup works
     * for transitions into and out of PPP mode.
     */
    if (tty->inpak[0]) {
	inpak_done(tty, tty->inpak[0]);
	tty->inpak[0] = NULL;
    }
    if (tty->inpak[1]) {
	inpak_done(tty, tty->inpak[1]);
	tty->inpak[1] = NULL;
    }
    if (tty->inheadpak) {
	inpak_done(tty, tty->inheadpak);
	tty->inheadpak = NULL;
    }
    tty->incount = 0;
    tty->isavedchar = -1;

    while (tty->rxdataq.qhead) {
	inpak_done(tty, dequeue(&tty->rxdataq));
    }
}

/*
 * reset_buffers()
 *	Move all TTY buffers to their free/inactive location
 */
static void
reset_buffers (tt_soc *tty)
{
    /*
     * Free up inbound packets
     */
    reset_rx_buffers(tty);

    /*
     * Free up outbound packet
     */
    if (tty->outpak) {
	outpak_done(tty, tty->outpak);
	tty->outpak = NULL;
    }
}

/*
 * cd2430_setspeed()
 *	Set a baud rate
 */
static boolean
cd2430_setspeed (tt_soc *tty, int txbaud, int rxbaud)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    uchar txpd, rxpd, txclk, rxclk;
    leveltype level;

    /*
     * Default baud rates if so desired
     */
    if (txbaud == -1) {
	txbaud = tty->tty_txspeed;
    }
    if (rxbaud == -1) {
	rxbaud = tty->tty_rxspeed;
    }
    if (txbaud > 115200 || rxbaud > 115200 || txbaud < 50  || rxbaud < 50) {
	return(FALSE);		/* Speed not supported */
    }

    /*
     * Calculate closest clock divisor and and period for each
     * baud rate.
     */
    if (!get_clock_async(txbaud, &txclk, &txpd) ||
	    !get_clock_async(rxbaud, &rxclk, &rxpd)) {
	return(FALSE);
    }

    /*
     * Disable transmit and receive while changing baud rate.  Clear
     * the channels and move all buffers back to their idle state.
     */
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    sendccr(csr, CCR_CLRCH | CCR_DISTX | CCR_DISRX);
    reset_buffers(tty);

    /*
     * Fix up transmit and receive speeds
     */
    csr->tcor = (txclk << 5);
    csr->tbpr = txpd;
    tty->tty_txspeed = txbaud;
    csr->rcor = rxclk;
    csr->rbpr = rxpd;
    tty->tty_rxspeed = rxbaud;
    tty->cache_rtpr = calc_rtpr(tty->tty_rxspeed);
    csr->rtpr = tty->cache_rtpr;

    /*
     * Replace the buffers under the receive slots.  Turn transmit and
     * receive back on at the new baud rate.  Kick of transmit again.
     */
    async_alloc_rx_buffers(tty, csr, 0);
    sendccr(csr, CCR_ENTX | CCR_ENRX);
    reset_interrupt_level(level);
    startoutput(tty);

    return(TRUE);
}

/*
 * cd2430_autosetspeed()
 *      Set speed for autobaud on given line
 */
static void
cd2430_autosetspeed (tt_soc *tty)
{
    (void)setspeed(tty, AUTOSPEEDHIGH, AUTOSPEEDHIGH);
}

/*
 * cd2430_autobaud
 * Automatically determine line speed from 300 to 19,200 baud.
 * Detect character is RETURN.
 */
static forktype
cd2430_autobaud (tt_soc *tty)
{
    static const uchar
	abmask[] =	{ 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F, 0x7F },
	abchar[] =	{ 0x7F, 0x7E, 0x7E, 0x0D, 0x3b, 0x66, 0x80, 0x0  };
    static const int
	abspeed[] =	{ 14400, 9600, 4800, 2400, 1800, 1200, 600,  300, -1 };
    int speed = -1;
    int ch, retries = autobaud_retry;
    uint i;

    MDMDBG("\nTTY%t: Autobaud started", tty->ttynum);

    /*
     * Try 19.2K first.
     */
    if (try_19200(tty)) {
        tty_startexec(tty);
        return;
    }
    /*
     * The wrong baud rate may well have left further cruft in
     * our buffer.  Clear it out so the regular autobaud code
     * can start cleanly.
     */
    tty_clearbuffers(tty);

    /*
     * Kick off regular autobaud
     */
    setspeed (tty, AUTOSPEED, AUTOSPEED);
    tty_portenable(tty);

    for (;;) {
	ch = getc(tty);			/* get character from the terminal */
	if (ch >= 0) {
	    MDMDBG("\n\nTTY%t: AUTOBAUD saw %#x at %u",tty->ttynum,
		    ch, AUTOSPEED);
	    for (i = 0; i < sizeof(abmask); i++)
		if ((ch & abmask[i]) == abchar[i])
		    break;
	    speed = abspeed[i];
	    if (speed != -1) {
try19:
		if (!setspeed (tty, speed, speed)) {
		    MDMDBG("\nTTY%t: autobaud failed setting speed %d",
			     tty->ttynum, speed);
		    setspeed(tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);
		    speed = -1;
		    continue;
		}
		MDMDBG("\nTTY%t: autobaud line speed is %d baud?", 
			tty->ttynum, speed);
		ch = auto_verify(tty);
		if (ch < 0) {
		    MDMDBG("\nTTY%t: Autobaud speed verify timeout",
			     tty->ttynum);
		    (void)setspeed(tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);
		    speed = -1;
		    continue;
		}

		/* Check for 4800 and 600 speeds */

                if ((speed == 300) && ((ch & 0x7f) == 0x72)) {
		    speed = 600;
		    goto try19;
		}

                if ((speed == 9600) && ((ch & 0x7f) == 0x66)) {
                    speed = 4800;
                    goto try19;
                }

		if ((ch == 015) || (ch == 0215)) {
		    tty_startexec(tty);
		} else {
		    MDMDBG("\nTTY%t: Autobaud verify failed (saw %#x), "
			"retrying", tty->ttynum, ch);
		    setspeed (tty, AUTOSPEED, AUTOSPEED);
		    tty_clearbuffers(tty);	
		    speed = -1;
		    continue;
		}
	    }
	}
	process_sleep_for (ONESEC >> 2);

	/*
	 * Either time out or give up if we were hung up (we are idle)
	 */
	if ((--retries <= 0) || (tty->statbits & CARDROP)) {
	    if (modem_debug) {
		if (retries <= 0)
		  ttybug(tty, "Autobaud timed out");
		if (tty->statbits & CARDROP)
		  ttybug(tty, "Autobaud line hung up");
	    }
	    death(tty);

/*
 * Should never get here.  For some reason the call to death() has returned.
 * Note it and kill it good.
 */

	    ttybug(tty, "Call to death() in autobaud() returned.");
	    process_kill(THIS_PROCESS);
	}
    }
}

/*
 * cd2430_paritybits()
 *	Set the requested parity
 *
 * This routine is used to set the parity of the line. If this is a
 * change, we set the appropriate value in the tty block and call
 * cd2430_setcor() to update the option register.
 */
static void
cd2430_paritybits (tt_soc *tty, int type)
{
    if (type != tty->tty_parity) {
	tty->tty_parity = type;
	cd2430_setcor(tty);
    }
}

/*
 * cd2430_setflow()
 *	Set the requested flow control
 *
 * This routine is used to set the flow control of the line. If
 * this is a change, we will call cd2430_setcor()
 * Note: reconfiguring SFLOW on already SFLOW line running output
 *       has been noticed to cause data garbling.
 */
static void
cd2430_setflow (tt_soc *tty)
{

    ulong *status = &(cd2430info_base[CD2430_ASYNC_TTYNUM(tty)].status);
    ulong new_status = 0;


    if (tty->capabilities & SFLOWIN) {   /* what flow type is reqd. ? */
	new_status |= SFLOW_IN;
    }
    if (tty->capabilities & SFLOWOUT) {
	new_status |= SFLOW_OUT;
    }
    if (tty->capabilities & HFLOWIN) {
	new_status |= HFLOW_IN;
    }
    if (tty->capabilities & HFLOWOUT) {
	new_status |= HFLOW_OUT;
    }
    /*
     * are we already doing the required flow type ?
     */
    if (new_status != ((*status) & (SFLOW_IN | SFLOW_OUT | HFLOW_IN |
				    HFLOW_OUT))) {
	*status &= ~(SFLOW_IN | SFLOW_OUT | HFLOW_IN | HFLOW_OUT);
	*status |= new_status;    /* remember the new flow type */
        /*
         * should we wait until xmitter is empty ?
         * we know that it garbles data, otherwise
         */
        tty_wait_output(tty, MAX_TTY_OUTPUT_WAIT);

	cd2430_setcor(tty);       /* go configure the channel */
    }
}


/*
 * cd2430_databits()
 *	Set the requested databits
 *
 * This routine is used to set the requested databits of the line. If this
 * is a change, we set the appropriate value in the tty block and update
 * using cd2430_setcor().
 */
static void
cd2430_databits (tt_soc *tty, int bits)
{
    if (bits < 5 || bits > 8) {
	bits = 8;
    }

    if (bits != tty->tty_databits) {
	tty->tty_databits = bits;
	cd2430_setcor(tty);
    }
}

/*
 * cd2430_stopbits()
 *	Set the requested stopbits
 *
 * This routine is used to set the requested stopbits of the line. If this
 * is a change, we set the appropriate value in the tty block and update
 * using cd2430_setcor().
 */
static void
cd2430_stopbits (tt_soc *tty, int bits)
{
    if (bits != tty->tty_stopbits) {
	tty->tty_stopbits = bits;
	cd2430_setcor(tty);
    }
}

/*
 * cd2430_sendbreak()
 *	Send a break
 */
static void
cd2430_sendbreak (tt_soc *tty)
{
    /*
     * We don't send the break here.  Instead, we flag that we would
     * like a break to be sent, and let the TX interrupt handler see
     * the flag, set the special chip mode, and queue up a packet containing
     * the needed data.
     */
    if (!(tty->statbits & (SENDBREAK | SENDINGBREAK))) {
	tty->statbits |= SENDBREAK;
	cd2430_startoutput(tty);
    }
}

/*
 * cd2430_portenable()
 *	Enable the specified port for I/O
 */
static void
cd2430_portenable (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    leveltype level;

    /*
     * Wait for CCR to be free
     */
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    if ((csr->csr & (CSR_RXEN|CSR_TXEN)) != (CSR_RXEN|CSR_TXEN))
	sendccr(csr, CCR_ENTX | CCR_ENRX);
    reset_interrupt_level(level);
}

/*
 * cd2430_portdisable()
 *	Disable the specified port for I/O
 */
static void
cd2430_portdisable (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    leveltype level;
    /*
     * Wait for CCR to be free
     */
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    sendccr(csr, CCR_DISTX | CCR_DISRX);
    csr->msvr_dtr = MSVR_DTR;         /* But let modems yack */
    reset_interrupt_level(level);
}

/*
 *		Modem Control Routines
 */

/*
 * cd2430_dtrstate()
 *	Set the state of DTR
 */
static void
cd2430_dtrstate (tt_soc *tty, boolean sense)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    leveltype level;

    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    if (sense) {
	csr->msvr_rts |= MSVR_RTS;
    } else {
	csr->msvr_rts &= ~MSVR_RTS;
    }
    reset_interrupt_level(level);
}

/*
 * cd2430_modemsignal()
 *	Return state of DSR and CTS
 */
static modem_status
cd2430_modemsignal (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    uchar msvr;
    modem_status summary;
    leveltype level;

    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    msvr = csr->msvr_rts;		/* Read the modem status */
    summary.ring = ((msvr & MSVR_DSR) != 0);
    summary.cts = ((msvr & MSVR_CTS) != 0);
    summary.dtr = ((msvr & MSVR_RTS) != 0);
    summary.rts = ((msvr & MSVR_DTR) != 0);
    reset_interrupt_level(level);

    return(summary);
}

/*
 * cd2430_modemchange()
 *	Read and clear modem changed flags
 *
 * The 2400 hardware supports modem line changes, but only within a
 * modem change interrupt handler.  So the interrupt handler drops
 * them into tty->modem_changes, and we pull them out here.
 */
static modem_status
cd2430_modemchange (tt_soc *tty)
{
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    modem_status summary;
    leveltype level;

    /*
     * Select channel
     */
    setchan_async(csr, tty);

    /*
     * Note lines which have changed since last check, clear them out.
     * Protect against races.
     */
    level = raise_interrupt_level(ALL_DISABLE);
    summary = tty->modem_changes;
    tty->modem_changes = (modem_status){0};
    reset_interrupt_level(level);

    /*
     * Return results
     */
    return(summary);
}

/*
 * print_modemcontrol()
 *	Show the current state of modnem control
 */
void
print_modemcontrol (tt_soc *tty)
{
    cd2430_quadart *csr;
    uchar msvr;
    leveltype level;
 
    /*
     * This is a global registry, don't try to do things to non-24XX
     * chips.
     */
    if (!CD2430_ASYNC_IS_VALID_TTYNUM(tty))
	return;

    /*
     * Access chip, show state
     */
    csr = (cd2430_quadart *)tty->uartdata;
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);
    msvr = csr->msvr_rts;
    reset_interrupt_level(level);

    printf("\nModem hardware state: %sCTS%s%sDSR%s %sDTR %sRTS",
	   msvr & MSVR_CTS ? "":"no", tty->modem_changes.cts ? "* ":" ",
	   msvr & MSVR_DSR ? "":"no", tty->modem_changes.ring ? "* ":" ",
	   msvr & MSVR_RTS ? "":"no",
	   msvr & MSVR_DTR ? "":"no");
}

/*
 * tty_xon()
 *	Do out-of-band character for flow control
 */
boolean
tty_xon (tt_soc *tty)
{
    leveltype level;

    /*
     * This is a global registry, don't try to do things to non-24XX
     * chips.
     */
    if (!CD2430_ASYNC_IS_VALID_TTYNUM(tty))
	return(FALSE);

    /*
     * Send XON if currently in XOFF
     */
    if (tty->capabilities & SFLOWOUT) {
	cd2430_quadart *csr;

	csr = (cd2430_quadart *)(tty->uartdata);
	level = raise_interrupt_level(TTY_DISABLE);
	setchan_async(csr, tty);
	if (!csr->ccr) {
	    csr->stcr = STCR_SNDSPEC|STCR_SPC1;
	    reset_interrupt_level(level);
	    return(TRUE);
	}
	reset_interrupt_level(level);
    }
    return(FALSE);
}

/*
 * cd2430_show_buffer_info()
 *	Display buffer information for this CD2430 channel.
 */
void
cd2430_show_buffer_info (tt_soc *tty)
{
    cd2430_priv *cd2430info_chan;
    
    cd2430info_chan = &cd2430info_base[CD2430_ASYNC_TTYNUM(tty)];
    printf(
"\nBuffer information");

    printf(
"\n  Rx ttycnt %x, sysbuf %d %d",
    tty->incount,
    cd2430info_chan->rxbuffs_outstanding,
    cd2430info_chan->rxbuffs_disallowed);
    printf(
"\n  Tx ttycnt %x",
    tty->outcount);

    printf(
"\n  Rx Buffs: inpk %x/%x inheadpk %x dataq %x %x %d"
"\n            pakq %x %x %d",
            tty->inpak[0], tty->inpak[1], tty->inheadpak,
            tty->rxdataq.qhead, tty->rxdataq.qtail, tty->rxdataq.count,
            tty->rxpakq.qhead, tty->rxpakq.qtail, tty->rxpakq.count);
    printf(
"\n  Tx Buffs: outpk %x  txpkq %x %x %d",
            tty->outpak, tty->txpakq.qhead, tty->txpakq.qtail,
            tty->txpakq.count);
    printf(
"\n  Priv Flags: %8X", cd2430info_chan->status);
}

/*
 * cd2430_async_show_controller()
 * 	Show controllers for an async interface.
 * This function is used by async interfaces that are referred to as
 * "serial" interfaces. This displays the controller information for a
 * single interface. The wisdom of having two of these show_controller
 * functions is questionable, but this ensures that the user interface
 * for this function in sync and async modes looks similar.
 *
 * this function is for serial interface which supports both Sync and Async
 */
boolean cd2430_async_show_controller (hwidbtype *idb, parseinfo *csb)
{
    int mode;
    cd2430_quadart *reg_table;
    per_channel_registers pcr;
    leveltype level;

    reg_table = cd2430_async_show_controller_init (idb, csb);
    level = raise_interrupt_level(NETS_DISABLE);
    setchan_async(reg_table, idb->idb_tty);
    cd2430_copy_per_channel_registers(reg_table,&pcr);
    reset_interrupt_level(level);

    mode = pcr.cmr & 0x7;
    switch (mode) {
    case CMR_MODE_ASYNC:
        printf("\n%s", async_str);
        break;
    case CMR_MODE_PPP:
        printf("\n%s(PPP)", async_str);
        break;
    case CMR_MODE_SLIP:
        printf("\n%s(SLIP)", async_str);
        break;
    case CMR_MODE_MNP4:
        printf("\n%s(MNP4)", async_str);
        break;
    case CMR_MODE_AUTO:
        printf("\n%s(Autobaud)", async_str);
        break;
    default:
        printf("\nChannel mode UNKNOWN");
        break;
    }

    printf("\nidb 0x%x, buffer size %d,", idb, idb->max_buffer_size);

    /* Display the global registers */
    cd2430_show_registers_global(reg_table);
 
    /* Display the per-channel regs */
    cd2430_show_registers_perchannel(&pcr);

    cd2430_show_buffer_info(idb->idb_tty);

    printf("\n");
 
    return(FALSE);
}

/*
 * cd2430_show_errors()
 *      Display nested intrrupt errors for this CD2430 channel.
 */
void
cd2430_show_errors (tt_soc *tty)
{
    cd2430_priv *cd2430info_chan;
 
    cd2430info_chan = &cd2430info_base[tty->ttynum - cd2430Base];
    printf(
"\nError Counters");
 
    printf(
"\n  Nested interrupts: transmit %d, receive %d, modem %d",
            cd2430info_chan->nested_tx_interrupts,
            cd2430info_chan->nested_rx_interrupts,
            cd2430info_chan->nested_mdm_interrupts);
}

/*
 * show_contr()
 *      Show async controller info (for all platforms)
 */
void
show_contr (void)
{
    int chip;
    leveltype level;
    per_channel_registers pcr;
 
    automore_enable(NULL);
    for (chip = 0; chip < cd2430_controllers; ++chip) {
        cd2430_quadart *csr = NULL;
        int chan;
 
	CD2430_ASYNC_GET_CSR(chip,&csr);
	if (csr == NULL)
	    continue;
 
        /*
         * Print the preamble information for this controller.
         */
        printf("\n\nCirrus CD2430 Quadart %d, Chip Revision %02x, Microcode %x",
	       chip, initial_gfrcr[chip], csr->gfrcr);
        /*
         * Print the global registers for this controller.
         */
        cd2430_show_registers_global(csr);
        for (chan = 0; chan < CD2430_CHIPCHAN; chan++) {
            tt_soc *tty;
 
            tty = MODEMS[cd2430Base + (chip * CD2430_CHIPCHAN) + chan];
	    
	    level = raise_interrupt_level(NETS_DISABLE);
            setchan_async(csr, tty);
	    cd2430_copy_per_channel_registers(csr,&pcr);
	    reset_interrupt_level(level);
            /*
             * Print the per-channel register values for this channel
             * on this controller.
             */
            cd2430_show_registers_perchannel(&pcr);
            /*
             * Display buffer information for this channel.
             */
            cd2430_show_buffer_info(tty);
            cd2430_show_errors(tty);
        }
    }
    automore_disable();
}

/*
 * fillq()
 *	Initialize a 2430 packet queue
 *
 * These queues are just used to hold our local transmit or receive
 * resources.  We prime them with the indicated number of the indicated
 * size of packet.
 */
static void
fillq (queuetype *q, int max)
{
	int x;
	paktype *pak;

	queue_init(q, max);
	for (x = 0; x < max; ++x) {
	    pak = pool_getbuffer(cd2430_buffpool);
	    if (pak == NULL) {
		break;
	    }
	    pak->length = pak->pool->size;
	    enqueue(q, pak);
	}
}

/*
 * cd2430_inputpending()
 *	Tell if we have input available for our user
 */
static int
cd2430_inputpending (tt_soc *tty)
{
    uint count;

    /*
     * Do nothing if the channel is not in async mode.
     */
    if (!channel_mode_async(tty)) 
        return (0);

    /*
     * A pushed-back character counts as one more
     */
    if (tty->isavedchar != -1) {
	count = 1;
    } else {
	count = 0;
    }

    /*
     * Check for an empty input queue.  If it appears empty, interlock
     * interrupts, then free the old packet and set up a new one as
     * needed.
     */
    count += tty->incount;
    if (count == 0) {
	leveltype level;
	cd2430_quadart *csr;

	csr = (cd2430_quadart *)(tty->uartdata);
	level = raise_interrupt_level(TTY_DISABLE);
	setchan_async(csr, tty);
	if (tty->inheadpak) {
	    inpak_done(tty, tty->inheadpak);
	    tty->inheadpak = NULL;
	}
	if (tty->rxdataq.qhead) {
	    tty->inheadpak = dequeue(&tty->rxdataq);
	    tty_rxptrs(tty, tty->inheadpak);
	}

	/*
	 * If the freed data is needed to continue input, arm
	 * new receive DMA now.
	 */
	if ((tty->inpak[0] == NULL) || (tty->inpak[1] == NULL)) {
	    async_alloc_rx_buffers(tty, csr, (tty->inpak[0] == NULL) ? 0 : 1);
	}


	/*
	 * If we still have no data, request interrupt after next byte
	 */
	count = tty->incount;
	if ((count == 0) && !tty->short_rtpr) {
	    csr->rtpr = 1;
	    tty->short_rtpr = TRUE;
	}

	reset_interrupt_level(level);
    }

    /*
     * If input has been stopped due to SW flow control, start it again
     */
    if ((count == 0) && (tty->statbits & STOPPEDINPUT)) {
	startinput(tty);
    }

    /*
     * Return count
     */
    return(count);
}

/*
 * cd2430_pppmode()
 *	Switching to PPP mode, tell chip
 */
void
cd2430_pppmode (tt_soc *tty, int on)
{
    cd2430_quadart *csr;
    leveltype level;

    /*
     * If not changing, or can't change, no-op
     */
    if (!can_ppp ||
	    (on && (tty->statbits & PPPMODE)) ||
	    (!on && !(tty->statbits & PPPMODE))) {
	return;
    }

    /*
     * Access channel's registers
     */
    csr = (cd2430_quadart *)(tty->uartdata);
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);

    /*
     * Disable interrupts
     */
    csr->ier = 0;

    /*
     * Flush I/O
     */
    sendccr(csr, CCR_CLRCH | CCR_DISTX | CCR_DISRX);

    /*
     * Update 2430 mode
     */
    if (on) {
	csr->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_PPP;
	tty->statbits |= PPPMODE;
	tty->txservice = NULL;		/* Avoid this interface */
	tty->rxservice = NULL;
    } else {
	csr->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_ASYNC;
	tty->statbits &= ~PPPMODE;
    }

    /*
     * Clear all connected buffers
     */
    reset_buffers(tty);

    /*
     * Re-arm RX DMA under new mode.
     */
    async_alloc_rx_buffers(tty, csr, 0);

    /*
     * Set channel options. This also initializes the channel;
     * the chip will see its new mode.
     */
    cd2430_setcor(tty);

    /*
     * Restore rtpr to correct value
     */
    csr->rtpr = tty->cache_rtpr;

    /*
     * Re-enable transmit/receive
     */
    sendccr(csr, CCR_ENTX | CCR_ENRX);

    /*
     * Re-enable interrupts.  Transmit interrupts will come on when output
     * is started.
     */
    csr->ier = IER_ASYNC_BASIC;
    reset_interrupt_level(level);
}

/*
 * cd2430_pppmap()
 *	Update hardware knowledge of PPP control character map
 */
void
cd2430_pppmap (tt_soc *tty, ulong txmap, ulong rxmap)
{
    cd2430_quadart *csr;
    leveltype level;
    /*
     * Take no action if PPP is unavailable or off.  Otherwise access the
     * appropriate channel.
     */
    if (!can_ppp || !(tty->statbits & PPPMODE)) {
	return;
    }
    csr = (cd2430_quadart *)(tty->uartdata);
    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);

    /*
     * Fill in TXMAP and RXMAP
     */
    csr->rxmap1 = rxmap;		csr->txmap1 = txmap;
    csr->rxmap2 = (rxmap >> 8);		csr->txmap2 = (txmap >> 8);
    csr->rxmap3 = (rxmap >> 16);	csr->txmap3 = (txmap >> 16);
    csr->rxmap4 = (rxmap >> 24);	csr->txmap4 = (txmap >> 24);
    reset_interrupt_level(level);
}

/*
 * Determine if PPP mode is allowable with the firmware that is present
 * on the 24XX controllers.
 */
boolean cd2430_async_allow_pppmode (void)
{
    cd2430_quadart *csr;
 
    CD2430_ASYNC_GET_CSR(0,&csr);
 
    /*
     * Read the version ID, and allow PPP mode if it's 2430-vintage
     * firmware.  Note, this code assumes that all chips on a box
     * are the same.
     */
     /* return((csr->gfrcr >= 0x1f)); */
    /* Chip mode PPP currently disabled */
    return (FALSE);
}
 
/*
 * cd2430_clearinput()
 *	Remove all input
 */
static void
cd2430_clearinput (tt_soc *tty)
{
    leveltype level;
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    boolean rx_disabled = FALSE;

    /*
     * Hold off input interrupts
     */
    level = raise_interrupt_level(TTY_DISABLE);

    /*
     * Disable receiver (if enabled), clear RX DMA, free all queued data and 
     * arm fresh RX buffers
     */
    setchan_async(csr, tty);
    if (csr->csr & CSR_RXEN) {
	sendccr(csr, CCR_DISRX); 
	rx_disabled = TRUE;
    }
    sendccr(csr, CCR_SET1 | CCR_CLRRCV);
    reset_rx_buffers(tty);
    async_alloc_rx_buffers (tty, csr, 0);

    /*
     * Release flow control
     */
    if (tty->statbits & SENTXOFF) {

	csr->stcr = STCR_SNDSPEC|STCR_SPC1;
	tty->statbits &= ~(SENTXOFF);
    }

    /*
     * Reenable receiver if it was disabled 
     */
    if (rx_disabled) {
	sendccr(csr, CCR_ENRX);
    }

    /*
     * Allow interrupts
     */
    reset_interrupt_level(level);

    /*
     * Continue input if held
     */
    if (tty->statbits & STOPPEDINPUT) {
	startinput(tty);
    }
}

/*
 * cd2430_clearoutput()
 *	Remove output queued but not yet in DMA
 */
static void
cd2430_clearoutput (tt_soc *tty)
{
    leveltype level;
    cd2430_quadart *csr = (cd2430_quadart *)tty->uartdata;
    boolean tx_disabled = FALSE;

    level = raise_interrupt_level(TTY_DISABLE);
    setchan_async(csr, tty);

    /*
     * Disable transmitter (if enabled), clear current transmit buffer 
     * if necessary & possible
     */
    if (tty->outpak && !cd2430_istate) {

  	if (csr->csr & CSR_TXEN) {
  	    sendccr(csr, CCR_DISTX); 
  	    tx_disabled = TRUE;
  	}
	sendccr(csr, CCR_SET1 | CCR_CLRTX);
	outpak_done(tty, tty->outpak);
	tty->outpak = NULL;
    }

    /*
     * Dump any queued outbound data, resetting pointers and
     * ensure an output packet is set up.
     */
    tty_txptrs(tty, (paktype *)(tty->txpakq.qhead));

    /*
     * Reenable transmitter if it was disabled
     */
    if (tx_disabled) {
 	sendccr(csr, CCR_ENTX);
    }
    reset_interrupt_level(level);
}

/*
 * cd2430_registers_init()
 * Initialize all the cd2430 registers to put the controller in the
 * async mode. This will call cd2430_setcor() to set the channel option
 * registers.
 */
void cd2430_registers_init (tt_soc *tty, cd2430_quadart *csr)
{
    int ctrl;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    ctrl = (CD2430_ASYNC_TTYNUM(tty))/CD2430_CHIPCHAN;
    /*
     * Select proper line
     */
    setchan_async(csr, tty);
    sendccr(csr, CCR_CLRCH);

    /*
     * Initialize mode, set options and baud rate
     */
    csr->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_ASYNC;
    csr->cpsr = CPSR_V41;
    tty->statbits &= ~PPPMODE;
    sendccr(csr, CCR_INITCH);
    cd2430_setcor(tty);
    cd2430_setspeed(tty, ASYNC_DEFSPEED, ASYNC_DEFSPEED);
    cd2430_setflow(tty);

    csr->msvr_dtr = MSVR_DTR; /* Set DTR and RTS */
    csr->msvr_rts |= MSVR_RTS;
    /* One vector set per controller */
    CD2430_SET_LIVR_ASYNC(tty);

    /* csr->licr |= ((uchar)ctrl << 4); */
    CD2430LICR_WRITE(csr, ctrl);

    /*
     * Command channel to enable receive/transmit
     */
    sendccr(csr, CCR_ENTX | CCR_ENRX);
    csr->ier = IER_ASYNC_BASIC;       /* Enable only basic interrupts */
    reset_interrupt_level(level);
}

/*
 * cd2430_async_tty_init()
 * initialize async tty structure   
 */
tt_soc *cd2430_async_tty_init (int unit, cd2430_quadart *csr)
{
    tt_soc *tty;

    tty = (tt_soc *)tty_init(unit, READY, TTY_LINE, &cd2430_vector);
 
    tty->uartdata = csr;
    tty->tty_databits = 8;
    tty->tty_parity = PARITY_NO;
    tty->tty_stopbits = STOPBITS2;
    tty->tty_stopchar = CTRLS; /* XOFF */
    tty->tty_startchar = CTRLQ; /* XON */
    tty->modem_type = MODEMTYPE_LOCAL; /* Default to local line */
    tty->modem_changes = (modem_status){0};
 
    /*
     * Initialize the packet memory which is dedicated statically
     * as DMA data buffers.  Flag that the TTY, initially, has no
     * input data.  insize is not really used (we use the pak size
     * instead) but tell the world the size our pool uses.
     */
    fillq(&tty->rxpakq, CD2430_DEF_RXBUFF);
    tty->insize = CD2430_PAKSIZE;
    tty->incount = 0;
    fillq(&tty->txpakq, CD2430_DEF_TXBUFF);
    tty_txptrs(tty, (paktype *)tty->txpakq.qhead);
    /*
     * Initialize TTY queues
     */
    queue_init(&tty->rxdataq, 0);
    /*
     * Initialize the LED mask for platforms that use it to light up
     * the activity LEDs.
     */
    CD2430_ASYNC_LED_INIT();

    return tty;
}


/*
 * cd2430_async_vector_init()
 * initialize async vector routings 
 */
void cd2430_async_vector_init (void)
{
    /*
     * Wire ourselves in to our interrupt vectors
     */
    vector_init(&cd2430_vector);
 
    /*
     * Input processing
     */
    cd2430_vector.vgetc = serial_getc;
    cd2430_vector.vstartinput = cd2430_startinput;
    cd2430_vector.vclearinput = cd2430_clearinput;
    cd2430_vector.vinputpending = cd2430_inputpending;
 
    /*
     * Output processing
     */
    cd2430_vector.vputc = serial_putc;
    cd2430_vector.vstartoutput = cd2430_startoutput;
    cd2430_vector.vclearoutput = cd2430_clearoutput;
    cd2430_vector.voutputpending = serial_outputpending;
    cd2430_vector.voutputblock = serial_outputblock;
    cd2430_vector.voutputxoffed = cd2430_outputxoffed;
    cd2430_vector.vputpkt = cd2430_putpkt;
 
    /*
     * Special functions
     */
    cd2430_vector.vsetspeed = cd2430_setspeed;
    cd2430_vector.vautosetspeed = cd2430_autosetspeed;
    cd2430_vector.vauto_baud = cd2430_autobaud;
    cd2430_vector.vparitybits = cd2430_paritybits;
    cd2430_vector.vdatabits = cd2430_databits;
    cd2430_vector.vstopbits = cd2430_stopbits;
    cd2430_vector.vsendbreak = cd2430_sendbreak;
 
    cd2430_vector.vservicehook = generic_servicehook;
    cd2430_vector.vnoservicehook = generic_noservicehook;
 
    cd2430_vector.vdtrstate = cd2430_dtrstate;
    cd2430_vector.vportenable = cd2430_portenable;
    cd2430_vector.vportdisable = cd2430_portdisable;
    cd2430_vector.vmodemsignal = cd2430_modemsignal;
    cd2430_vector.vmodemchange = cd2430_modemchange;
    cd2430_vector.vsetflow = cd2430_setflow;
    cd2430_vector.vsetsflow = cd2430_setflow;
}

/*
 * cd2430_setup_break_packet()
 * create a BREAK packet.
 */
boolean cd2430_setup_break_packet(void)
{
  uchar *p;
 
    /*
     * Create a "generate BREAK" packet. Malloc from IO mem instead
     * of getting a buffer to leave initial system buffer levels at
     * desired/configured values.   For compatibility, we send break
     * for 1/2 sec.  The trailing escapes are to cover a race condition
     * where the data is still streaming out, but our interrupt handler
     * wants to turn off ETC mode.  The escapes will be harmless trailer
     * data with or without ETC mode.
     */
    break_pak = malloc_iomem(CD2430BREAKDATA + sizeof(paktype));
    if (!break_pak) {
        errmsg(&msgsym(NOBRKPAK, TTYDRIVER));
        crashdump(0);
        return FALSE;       /* Be paranoid */
    }
    break_pak->datagramstart = ((char *)break_pak) + sizeof(paktype);
 
    p = break_pak->datagramstart;
    *p++ = ETC_ESC; *p++ = ETC_BREAK;
    *p++ = ETC_ESC; *p++ = ETC_DELAY; *p++ =
        (cd2430clock / 2048 / CD2430_PRESCALE) / 2;
    *p++ = ETC_ESC; *p++ = ETC_ENDBREAK;
    break_pak->datagramsize = break_pak->length =
        (char *)p - (char *)break_pak->datagramstart;
    return TRUE;
}

