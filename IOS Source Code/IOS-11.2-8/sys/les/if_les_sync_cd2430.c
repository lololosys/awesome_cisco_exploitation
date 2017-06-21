/* $Id: if_les_sync_cd2430.c,v 3.1.2.17 1996/09/15 18:19:35 srane Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_sync_cd2430.c,v $
 *------------------------------------------------------------------
 * if_les_sync_cd2430.c - Common code for synchronous serial
 *                        interface driver for Cirrus CL-CD2430
 *                        controller
 *
 * May 1996, Jose Hernandez
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_sync_cd2430.c,v $
 * Revision 3.1.2.17  1996/09/15  18:19:35  srane
 * CSCdi62516:  buffer changes lost after reload on a serial SMDS interface
 * Branch: California_branch
 *
 * Revision 3.1.2.16  1996/09/10  03:08:26  snyder
 * CSCdi68568:  more constant opportuniites
 *              160 out of data
 *               72 out of image
 * Branch: California_branch
 *
 * Revision 3.1.2.15  1996/09/09  01:34:30  xtang
 * CSCdi56632:  unknown clock rates on 4T PA, FSIP
 * Branch: California_branch
 *         add ALT_HELPS to use csb->which in different
 *         driver procedures, and print the
 *         driver specific info. In this case, the clockrate
 *         option table, and csb->which = CFG_INT_CLOCKRATE
 *
 * Revision 3.1.2.14  1996/09/05  17:59:30  etrehus
 * CSCdi68090:  bsc and sdlc can not coexist on the same controller
 * Branch: California_branch
 *
 * Revision 3.1.2.13  1996/08/29  23:20:21  srane
 * CSCdi54221:  Potential error when full duplex loopback by CD2430 sync.
 * driver
 * Branch: California_branch
 *
 * Revision 3.1.2.12  1996/08/28  12:58:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.2.11  1996/08/22  00:19:56  etrehus
 * CSCdi64284:  Cobra - frame error on async/sync ports with x25
 * Branch: California_branch
 * CSCdi65997:  DTR dropping on Cobra boxes using DDR on low speed
 * interfaces
 *
 * Revision 3.1.2.10  1996/08/13  02:20:35  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.1.2.9  1996/07/26  22:08:36  jbalestr
 * CSCdi64159:  Modular cobra commit missed some bug fixes for 11.2
 * Branch: California_branch
 *
 * Revision 3.1.2.8  1996/07/25  23:44:55  jhernand
 * CSCdi57040:  SDLC test fails intermittently on Async/Sync ports OK on
 *              Sync port
 * Add the TXCRXC control bit to Low Speed Serial Device Register Hi
 * Branch: California_branch
 *
 * Revision 3.1.2.7  1996/07/23  13:25:57  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.1.2.6  1996/07/15  23:56:50  etrehus
 * CSCdi57046:  Potential error when in hdx mode when schedule run
 * interval is used
 * Branch: California_branch
 *
 * Revision 3.1.2.5  1996/07/04  02:07:06  jhernand
 * CSCdi61640:  Cobra code does not conform to Cisco guidelines
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/06/28  23:23:48  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/27  19:51:58  jhernand
 * CSCdi61415:  Remove PAK_CD2430_FAST flag and its usage in Modular Cobra
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/20  01:44:06  jbalestr
 * CSCdi52599:  bsc does not update interface error counters
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/06/17  08:34:00  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.1  1996/05/13  18:57:10  samuell
 * Placeholder files for porting Modular 2500 Cobra platform from 11.1.
 * Modular 2500 Cobra files were derived from the original Cobra 11.1
 * Release by code separation into platform-independent code and 2500
 * platform-dependent code files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "interface_private.h"
#include "config.h"
#include "stacks.h"
#include "subsys.h"
#include "logger.h"
#include "../les/msg_cd2430.c"		/* Not a typo, see logger.h */
#include "../os/buffers.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_half_duplex.h"
#include "../if/network.h"
#include "../if/linkdown_event.h"
#include "../les/if_les.h"
#include "../les/if_les_common_cd2430.h"
#include "../les/if_les_sync_cd2430.h"
#include "../os/pool.h"
#include "../os/free.h"
#include "../ip/ip.h"
#include "../les/les_driver_inline.h"
#include "../tcp/tcpcompress.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../wan/serial_parser.h"
#include "../wan/atm_dxi.h"
#include "../wan/dialer_registry.h"
#include "../ibm/sdlc.h"
#include "if_les_serial.h"
#include "../if/serial_inline.h"
#include "../les/cd2430_sync_debug.h"
#include "../util/itemlist.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/traffic_shape.h"

/*
 * Basic interrupt sources for the Cirrus serial interface that we always
 * wish to monitor.
 */
#define IER_SYNC_BASIC (IER_MDM | IER_RXD | IER_TXD)

/*
 * Max loop counter for trying to clear the interrupt stack due to the bug
 * exists in the CD2430 chip.
 */
#define MAX_LOOP_CNT  100
#define SMALLEST_FRAME_SIZE 3           /* smallest valid frame */

int helper_pid = NO_PROCESS;            /* PID of FS helper */

static ulong global_teoir, global_reoir, global_meoir;

static void cd2430_sync_reset(hwidbtype *);

void cd2430_sync_mode_init(hwidbtype *);

static void cd2430_sync_show_errors(hwidbtype *idb);
static void cd2430_sync_show_hdx_log(hwidbtype *idb);
static void cd2430_clear_receiver(hwidbtype *idb, cd2430_context_t *ctx, 
                                  cd2430_quadart *reg_table);

static const char sync_str[] =
        "Channel mode is synchronous serial ";

/* 
 *        Pre-computed Clock Rate Table for the CD2430
 * 
 * To generate the transmit and receive clocks in the sync mode we use an
 * external crystal on the motherboard for reasons of accuracy. The
 * frequency of this crystal oscillator on the motherboard is 16.128 MHz.
 * We use the combination of a pre-divider on the motherboard and the
 * division offered by the Cirrus controller via the transmit and receive
 * bit period registers to obtain the required clock rate. The values for
 * the pre-divider and the bit-period register for the allowable clock
 * rates is pre-computed and stored in the table below.
 */
static const cd2430_clocktype_t cd2430_clock_table[] = {

	/* Rate              Pre-divider     Bit period register */ 
	{ 300,		   0x03, /* 256 */       209		},
	{ 1200,		   0x02, /* 64	*/       209		},
	{ 2400,		   0x01, /* 32	*/       209		},
	{ 4800,		   0x00, /* 14	*/       239		},
	{ 9600,		   0x00, /* 14  */       119		},
	{ 14400,	   0x00, /* 14  */       79		},
	{ 19200,	   0x00, /* 14  */       59		},
	{ 28800,	   0x00, /* 14  */       39		},
	{ 32000,	   0x00, /* 14  */       35		},
	{ 38400,	   0x00, /* 14  */       29		},
	{ 56000,	   0x01, /* 32	*/       8		},
	{ 57600,	   0x00, /* 14  */       19		},
	{ 64000,	   0x00, /* 14  */       17		},
	{ 72000,	   0x00, /* 14  */       15		},
	{ 115200,	   0x00, /* 14  */       9		},
	{ 0,		   0,		         0		}

};

hdx_log_element_t *hdx_log_base;



/*************************************************************************
 *         Inline Functions for the CD2430 Sync Driver
 *
 * The following functions have all been inlined because they are in
 * interrupt processing path for this driver where speed is of the
 * essence.
 *************************************************************************/

/*
 * This function will process high-priority packet receives in certain
 * situations. For a received packet, we attempt to replace the packet
 * buffer used for reception with another from the buffer pool of size
 * equal to the mtu on that interface. If we are unable to obtain a
 * buffer of that size AND the received packet is a high-priority
 * packet (like a keepalive etc), then this routine is invoked to
 * handle the situation.
 */
inline static void
cd2430_process_hi_priority_pak (hwidbtype *idb, paktype *pak)
{
    paktype *newpak;

    newpak = getbuffer(pak->datagramsize);
    if (newpak) { /* Got a smaller buffer */
        /*
         * Queue packet for process level:
         */
        newpak->enctype = pak->enctype;
        newpak->flags = pak->flags;
        newpak->datagramsize = pak->datagramsize;
        newpak->datagramstart = newpak->data_area +
                                (pak->datagramstart - pak->data_area);
        bcopy(pak->datagramstart, newpak->datagramstart,
                                                 pak->datagramsize);
        set_if_input(newpak, idb->firstsw);
        serial_queue_for_process(idb, newpak);
    }
    else {  /* Could not get a smaller buffer */
        idb->counters.input_drops++;
        MAYBE_DROPPED(idb);
    }
}

/*
 * This routine is used to extract packets for transmission from the
 * output queue for the specified interface. If there are no packets
 * for transmission in the output queue, this routine will return NULL.
 */
inline static paktype *
cd2430_extract_tx_pak (hwidbtype *idb)
{
    paktype *pak;

    do {
        pak = (*idb->oqueue_dequeue)(idb);

        if (pak == NULL)
            return(NULL);

        if (pak->datagramsize > idb->max_pak_size) {
            errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring,
                                          pak->datagramsize);
            datagram_done(pak);
            /* bisync wants completion notification */
            if (idb->poll_protocol_flags & PP_ACTIVE) {
                (*idb->tx_comp_notify)(idb);
            }
            pak = NULL;
            continue;
        }

        if ((cpu_type == CPU_CANCUN) || (cpu_type == CPU_SIERRA)) {
            paktype *new_pak;
            /*
             * Make sure that datagramstart will not be at an odd
             * address -  Cancun may get underruns with odd address
             * Note that multiple protocols may be referencing this
             * packet, thus even though there is room to just shift
             * it to an word aligned boundry, it is correct to
             * copy the packet to a new buffer and use that new packet
             */
            if ((uint)pak->datagramstart & 1) {
                new_pak = serial_fix_datagramstart(pak, idb);
                if (new_pak)
                    pak = new_pak;
                else {
                    datagram_done(pak);
                    pak = NULL;
                }
            }
        }
    } while (pak == NULL);

    return(pak);
}

/*
 * This routine takes care of necessary accounting for the specified
 * packet being transmitted on the specified interface.
 */
inline static void cd2430_tx_pak_accounting (hwidbtype *idb, paktype *pak)
{
    ulong acct_proto, bytes;

    acct_proto = (ulong)pak->acct_proto;
    bytes = pak->datagramsize;

    /*
     * Take care of accounting. Increment number of packets transmitted
     * and update total number of bytes transmitted.
     */
    idb->counters.outputs++;
    idb->counters.tx_cumbytes += bytes;

    if ((pak->enctype == ET_HDLC) &&
        (GETSHORT(pak->datagramstart) & HDLC_BCAST))
        idb->counters.output_broad++;
}

/*
 * This function does the necessary tasks prior to releasing a packet
 * buffer that has been transmitted, and then releases the packet. It
 * also ensures that the buffer in question is not owned by the Cirrus
 * before releasing the packet.
 */
inline static void
cd2430_release_tx_pak (hwidbtype *idb, cd2430_context_t *ctx, int index)
{
    cd2430_quadart *reg_table = ctx->reg_table;

    if (ctx->outpak[index] == NULL)
        return;

    /*
     * Make sure that the packet to be released is not currently owned by
     * the Cirrus controller.
     */
    if (index) {
        if (reg_table->btbsts & TBSTS_24OWN)
            return;
    }
    else {
        if (reg_table->atbsts & TBSTS_24OWN)
            return;
    }

    TIMER_STOP(idb->xmittimer);
    GET_TIMESTAMP(idb->lastoutput);
    datagram_done(ctx->outpak[index]);
    /*
     * If the transmit completion notify vector is initialized, call
     * notification function.
     */
    if (idb->poll_protocol_flags & PP_ACTIVE) {
        (*idb->tx_comp_notify)(idb);
    }

    ctx->outpak[index] = NULL;
}

/*
 * alloc_rx_buffers()
 *      Allocate A/B if idle & buffers are available
 *
 * This function must also take into account the possibility that buffers
 * could not be allocated due to unavailability in the past. In that case
 * both the A and B buffers will have to be replenished.
 */
inline static void
alloc_rx_buffers (hwidbtype *idb, int idx)
{
    paktype *newpak;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;

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
        if (ctx->inpak[idx]) {
            break;
        }

        /*
         * Ask protocol layer for new packet
         */
        newpak = les_driver_getbuf_inline(idb, TRUE);

        /*
         * Found one.  Set it up for DMA, and record its identity so we
         * can get the pak pointer back later.
         */
        if (newpak) {
            ctx->inpak[idx] = newpak;
            arm_rx_dma(reg_table, idx,
                VTOP(newpak->datagramstart), idb->max_buffer_size);
        }
        else
            break;
    }
}

/*
 * arm_tx_dma()
 *      Arm transmit DMA to CD2430
 *
 * Given the index of the buffer to be armed and the address and
 * size of the buffer to be programmed into the DMA transmit buffers
 * this function will do so. Also, the transmit buffer status register
 * will be set to indicate that
 *      o buffer is owned by CD2430
 *      o interrupt needed after buffer transmission
 *      o EOF bit since we don't chain buffers
 */
inline static void
arm_tx_dma (cd2430_quadart *reg_table, ulong addr,
            uint count, int buf_index)
{
    if (buf_index) {
        reg_table->btbadrl = CD2430LO(addr);
        reg_table->btbadru = CD2430HI(addr);
        reg_table->btbcnt = count;
        reg_table->btbsts = TBSTS_INTR | TBSTS_24OWN | TBSTS_EOF;
    }
    else {
        reg_table->atbadrl = CD2430LO(addr);
        reg_table->atbadru = CD2430HI(addr);
        reg_table->atbcnt = count;
        reg_table->atbsts = TBSTS_INTR | TBSTS_24OWN | TBSTS_EOF;
    }
}

/*
 * This inline function handles a packet that was just received.
 * If the received packet was received without errors, it consumes the
 * packet. Else, it returns the packet for it to be reused for packet
 * receives by the Cirrus controller.
 */
inline static paktype *
cd2430_receive_packet (hwidbtype *idb, cd2430_context_t *ctx,
                       cd2430_quadart *reg_table, paktype *pak, boolean giant)
{
    ushort count, status, curbuf;

    /*
     * If we have a scheduled run interval configured, check if we
     * need to drop packets to give processes a chance to run.
     */
    if (sched_run_interval && AWAKE(sched_run_time)) {
      goto drop_packet;
    }

    status = reg_table->risr;
    curbuf = (status & RISR_BUFB) ? 1 : 0;

    if (giant) {
        count = idb->max_buffer_size + 1;
    } else {
        count = (curbuf ? reg_table->brbcnt : reg_table->arbcnt);
    }

    if (count > 0) {
        /*
         * Try to get a buffer to replace this received packet buffer.
         * If we cannot get a replacement buffer, we will drop the
         * packet just received and re-arm the existing buffer for
         * receive DMA.
         */
         paktype *newpak = NULL;
         newpak = les_driver_getbuf_inline(idb, TRUE);
         if (newpak) { /* Replacement packet buffer was available */
            ctx->inpak[curbuf] = newpak; 
            if (giant) {
                cd2430_clear_receiver(idb, ctx, reg_table);
            } else {
                arm_rx_dma(reg_table, curbuf,
                           VTOP(newpak->datagramstart), idb->max_buffer_size);
            }

            pak->datagramsize = count;

            /*
             * If bisync, we need to do some processing to
             * determine the frame type.
             */
            if (idb->poll_protocol_flags & PP_ACTIVE)
                (*idb->rx_framer_int)(idb, &pak);

            if (pak) {
                /* Process the received packet */
                set_if_input(pak, idb->firstsw);
                GET_TIMESTAMP(idb->lastinput);
                serial_RX_interrupt_inline(idb, pak);
            } 
            return (NULL);
        } else {
drop_packet:
            if (giant) {
                cd2430_clear_receiver(idb, ctx, reg_table);
                pak = NULL;
            }
            idb->counters.input_drops++;
            MAYBE_DROPPED(idb);
        }
    }
    return (pak);
}

/*
 * This inline function extracts any packets that may be in the output
 * queue for this interface and transmits them.
 */
inline static void
cd2430_transmit_packet (hwidbtype *idb, cd2430_context_t *ctx,
                        cd2430_quadart *reg_table)
{
    paktype *pak;
    int i, buf_index;

    buf_index = (reg_table->dmabsts & DMABSTS_NTBUF) ? 1 : 0;
    for (i = 0; i < 2; i++, buf_index ^= 1) {
        if (buf_index) {
            if (reg_table->btbsts & TBSTS_24OWN)
                continue;
        }
        else {
            if (reg_table->atbsts & TBSTS_24OWN)
                continue;
        }
        /*
         * Free up any packet that may be associated with this
         * buffer owned by the cpu.
         */
        cd2430_release_tx_pak(idb, ctx, buf_index);

        pak = cd2430_extract_tx_pak(idb);
        if (pak == NULL)
            break;

        arm_tx_dma(reg_table, VTOP(pak->datagramstart), pak->datagramsize,
                                                     buf_index);
        /*
         * Cache this packet pointer in the corresponding element
         * of the outpak array and take care of accounting.
         */
        ctx->outpak[buf_index] = pak;
        TIMER_START(idb->xmittimer, CD2430_TX_TIMEOUT);
        cd2430_tx_pak_accounting(idb, pak);
    }
}


/*
 * cd2430_flush_restart_receive
 * 
 */
static void 
cd2430_flush_restart_receive (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;

    if (idb->poll_protocol_flags & PP_CHARINT) {
        ctx = INSTANCE;
        reg_table = ctx->reg_table;
        sendccr(reg_table, CCR_SET1 | CCR_CLRRCV);
        sendccr(reg_table, CCR_ENRX);
   }
}

/*
 * cd2430_disable_receiver
 *
 */
static void
cd2430_disable_receiver (hwidbtype *idb)
{    
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    if (idb->poll_protocol_flags & PP_CHARINT) {
        ctx = INSTANCE;
        reg_table = ctx->reg_table;
        sendccr(reg_table, CCR_DISRX);
    }
}    

/*
 * cd2430_get_sysbuffer
 *
 */
static paktype *
cd2430_get_sysbuffer (hwidbtype *idb)
{
    paktype *newpak = les_driver_getbuf_inline(idb, TRUE);
    newpak->datagramsize = 0;
    return(newpak);
}

/*
 * cd2430_syn_detected
 */
int cd2430_syn_detected (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    int syn_detected;
    leveltype level;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    level = raise_interrupt_level(NETS_DISABLE);
    setchan_sync(reg_table, idb);

    /* if flag set than a frame is being currently received */
    syn_detected = reg_table->csr & CSR_RXFLON;

    reset_interrupt_level(level);
    return(syn_detected);
}

/*
 * Enable modem signal change interrupts
 */
static inline void
enable_modem_interrupts (cd2430_quadart *reg_table)
{
    reg_table->ier |= IER_MDM;
}

/*
 * Disable modem signal change interrupts
 */
static inline void
disable_modem_interrupts (cd2430_quadart *reg_table)
{
    reg_table->ier &= ~IER_MDM;
}

/*
 * Check for possibly flapping interface
 */
static inline void
monitor_interface_flap (cd2430_context_t *ctx, cd2430_quadart *reg_table)
{
    if (++ctx->flap_count > ctx->flap_rate) {
        disable_modem_interrupts(reg_table);
        ctx->flap_enabled = TRUE;
    }

    if (!ctx->flap_enabled) {
        if (!TIMER_RUNNING_AND_SLEEPING(ctx->flap_time)) {
            TIMER_START(ctx->flap_time, ONESEC);
            ctx->flap_count = 0;
        }
    }
}



/*
 * Start general Timer 1 with specified value
 */
inline static void
start_timer1 (cd2430_quadart *reg_table, cd2430_timer_t *timer)
{
    sendccr(reg_table, (CCR_SET1 | CCR_CLRT1)); /* First clear the timer */
    reg_table->tpr = timer->tpr;
    reg_table->gt1 = timer->timer_val;
}

/*
 * Clear General Timer 1
 */
inline static void
clear_timer1 (cd2430_quadart *reg_table)
{
    sendccr(reg_table, (CCR_SET1 | CCR_CLRT1));
}

/*
 * Start general Timer 2 with specified value
 */
inline static void
start_timer2 (cd2430_quadart *reg_table, cd2430_timer_t *timer)
{
    sendccr(reg_table, (CCR_SET1 | CCR_CLRT2)); /* First clear the timer */
    reg_table->tpr = timer->tpr;
    reg_table->ttr = timer->timer_val;
}
 
/*
 * Clear General Timer 2
 */
inline static void
clear_timer2 (cd2430_quadart *reg_table)
{
    sendccr(reg_table, (CCR_SET1 | CCR_CLRT2));
}



/*
 * Report of the output queue has packets to transmit
 */
inline static boolean
txqueue_nonempty (hwidbtype *idb)
{
    paktype *pak;
    pak = (*idb->oqueue_dequeue)(idb);
    if (pak) {
        pak_requeue_head(idb, pak);
        return (TRUE);
    }
    else {
        return (FALSE);
    }
}

/*
 * Deassert all modem signals for this interface.
 */
inline static void
deassert_modem_signals (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    cd2430_quadart   *reg_table;
 
    ctx = INSTANCE;
    reg_table = ctx->reg_table;
 
    if (idb->dte_interface) {
        deassert_DTR(idb, reg_table);
        deassert_RTS(idb, ctx);
    }
    else {
        deassert_DSR(idb, reg_table);
        deassert_DCD(idb, ctx);
        deassert_CTS(idb, ctx);
    }
}

static void reset_interface(cd2430_context_t *ctx)
{
  if (serial_debug) {
    buginf("\nresetting interface request");
    bugtrace();
  }
  ctx->reset_interface = TRUE;
}

/*
 * cd2430_clear_nested_txint()
 *	This function will attempt to clear a potentially nested transmit 
 * interrupt. Writing to the TEOIR should result in the stack register
 * indicating that the current interrupt, if any, is something other
 * than a transmit interrupt. If we find that this is not the case, we
 * will write as many times as necessary to make that happen. This is
 * to guard against a known Cirrus bug where the chip indicates two
 * nested interrupts of the same kind - a state that is illegal for
 * the controller to be in.
 */
inline static void
cd2430_clear_nested_txint (cd2430_context_t *ctx, 
                           cd2430_quadart *reg_table, ulong val)
{
  int i;
 
  for (i = 0;
       i < MAX_LOOP_CNT && ((reg_table->stk & STK_CLVL_MASK) == STK_TX_CLVL);
       i++) {
    reg_table->teoir = val;
  }
  if (i > 0) {
    ctx->nested_tx_ints++;
  }
  /*
   * Signal the periodic process to reset this interface the
   * next time the process runs when we can't clear the interrupt
   * stack.
   */
  if (i == MAX_LOOP_CNT) {
    reset_interface(ctx);
  }
}

/*
 * cd2430_clear_nested_rxint()
 *      This function will attempt to clear a potentially nested receive
 * interrupt. Writing to the REOIR should result in the stack register
 * indicating that the current interrupt, if any, is something other 
 * than a receive interrupt. If we find that this is not the case, we 
 * will write as many times as necessary to make that happen. This is 
 * to guard against a known Cirrus bug where the chip indicates two
 * nested interrupts of the same kind - a state that is illegal for
 * the controller to be in.
 */
inline static void
cd2430_clear_nested_rxint (cd2430_context_t *ctx, 
                           cd2430_quadart *reg_table, ulong val)
{
  int i;
 
  for (i = 0;
       i < MAX_LOOP_CNT && ((reg_table->stk & STK_CLVL_MASK) == STK_RX_CLVL);
       i++) {
    reg_table->reoir = val;
  }
  if (i > 0) {
    ctx->nested_rx_ints++;
  }
  /*
   * Signal the periodic process to reset this interface the
   * next time the process runs when we can't clear the interrupt
   * stack.
   */
  if (i == MAX_LOOP_CNT) {
    reset_interface(ctx);
  }
}

/*
 * cd2430_clear_nested_modint()
 *      This function will attempt to clear a potentially nested modem
 * interrupt. Writing to the MEOIR should result in the stack register
 * indicating that the current interrupt, if any, is something other 
 * than a modem interrupt. If we find that this is not the case, we 
 * will write as many times as necessary to make that happen. This is 
 * to guard against a known Cirrus bug where the chip indicates two
 * nested interrupts of the same kind - a state that is illegal for
 * the controller to be in.
 */
inline static void
cd2430_clear_nested_modint (cd2430_context_t *ctx, 
                            cd2430_quadart *reg_table, ulong val)
{
  int i;
 
  for (i = 0;
       i < MAX_LOOP_CNT && ((reg_table->stk & STK_CLVL_MASK) == STK_MODEM_CLVL);
       i++) {
    reg_table->meoir = val;
  }
  if (i > 0) {
    ctx->nested_modem_ints++;
  }
  /*
   * Signal the periodic process to reset this interface the
   * next time the process runs when we can't clear the interrupt
   * stack.
   */
  if (i == MAX_LOOP_CNT) {
    reset_interface(ctx);
  }
}



/************************************************************************
 *              Half-duplex Logging Related Functions
 *
 * These functions are embedded in macros and are called to update the
 * half-duplex log as half-duplex events occur, states change or modem
 * signals are asserted or deasserted.
 ************************************************************************/
static void log_half_duplex_data (cd2430_context_t *ctx, char *fsm_string,
                                  char *state_string, char *event_string)
{
    hdx_log_base = ctx->hdx_log;
    GET_TIMESTAMP(hdx_log_base[ctx->hdx_log_index].timestamp);
    hdx_log_base[ctx->hdx_log_index].fsm   = fsm_string;
    hdx_log_base[ctx->hdx_log_index].state = state_string;
    hdx_log_base[ctx->hdx_log_index].event = event_string;
    hdx_log_base[ctx->hdx_log_index].figure = NULL;
    if ((++ctx->hdx_log_index) >= HDX_LOG_LENGTH)
        ctx->hdx_log_index = 0;
}

static void log_half_duplex_figure (cd2430_context_t *ctx, char *figure_string)
{
    hdx_log_base = ctx->hdx_log;
    GET_TIMESTAMP(hdx_log_base[ctx->hdx_log_index].timestamp);
    hdx_log_base[ctx->hdx_log_index].fsm   = NULL;
    hdx_log_base[ctx->hdx_log_index].state = NULL;
    hdx_log_base[ctx->hdx_log_index].event = NULL;
    hdx_log_base[ctx->hdx_log_index].figure = figure_string;
    if ((++ctx->hdx_log_index) >= HDX_LOG_LENGTH)
        ctx->hdx_log_index = 0;
}


/************************************************************************
 *     Half-Duplex Finite State Machines for the CD2430 Sync Driver
 *
 * The four state machines to handle half-duplex operation on the Cirrus
 * serial interfaces are
 *     o The DTE transmit finite state machine
 *     o The DTE receive finite state machine
 *     o The DCE transmit finite state machine
 *     o The DCE receive finite state machine
 *
 * These state machine implementations follow next. These state machines
 * are driven from the receive, transmit and modem interrupt service
 * routines. The line_state() routine which handles the line protocol
 * on the interface going up or down based on changes in modem signals
 * is also included in this section of the driver. 
 ************************************************************************/

/*
 * Check to see that the event-state combination for the DTE half-duplex
 * transmit state machine is valid.
 */
inline static boolean valid_dte_tx_fsm_state_event (enum HDX_TX_STATE state,
                                                    enum HDX_EVENT event)
{
    if ((state == HDX_DTE_TX_STATE_READY)    || /* First check the state ... */
        (state == HDX_DTE_TX_STATE_TRANSMIT_DELAY)     ||
        (state == HDX_DTE_TX_STATE_WAIT_CTS) ||
        (state == HDX_DTE_TX_STATE_TRANSMIT) ||
        (state == HDX_DTE_TX_STATE_WAIT_TXMPTY)        ||
        (state == HDX_DTE_TX_STATE_WAIT_RTSDROP_DELAY) ||
        (state == HDX_DTE_TX_STATE_WAIT_CTSDROP)) { /* ... then check event */
        if ((event == HDX_ENTRY_EVENT)      ||
            (event == HDX_TIMER1_EVENT)     ||
            (event == HDX_CTS_CHANGE_EVENT) ||
            (event == HDX_TX_EOF_EVENT)     ||
            (event == HDX_TX_EMPTY_EVENT)) {
            return (TRUE);
        }
    }
    return (FALSE);  /* Invalid state-event combination */
}

/*
 * The DTE Half-duplex Transmit Finite State Machine
 * General notes on this state machine:
 *  o When we are actively transmitting data in this state machine, we
 *    consider the de-assertion of CTS by the DCE to be a catastrophic
 *    event. The process level code is signalled via the reset_interface
 *    flag in the context structure that this interface is to be reset
 *    if such a catastrophic event occurs.
 */
static void cd2430_sync_hdx_dte_tx_fsm (hwidbtype *idb, 
                                        enum HDX_EVENT hdx_event)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    paktype *pak = NULL;
    boolean final_frame = FALSE;
    int buf_index;
    enum HDX_TX_STATE next_state;
    enum HDX_EVENT next_event;
    uchar bscConfigured;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    bscConfigured = (idb->poll_protocol_flags & PP_ACTIVE);
    next_state = ctx->hdx_tx_state;

    /*
     * Loop until all state changes necessary in this invocation of the
     * state machine are done.
     */
    while (next_state != HDX_TX_STATE_NULL) {
        
        /*
         * Log debug information
         */
        HDX_LOG(ctx, dte_tx_fsm, hdx_tx_state_list[ctx->hdx_tx_state-10], 
                            hdx_event_list[hdx_event]);

        /*
         * Check to ensure that this combination and the state and event
         * is a valid one for this state machine.
         */
        if (!valid_dte_tx_fsm_state_event(ctx->hdx_tx_state, hdx_event)) {
            errmsg(&msgsym(BADHDXFSM, CIRRUS), idb->unit, ctx->hdx_tx_state,
                                                                hdx_event);
            return;
        }

        next_state = HDX_TX_STATE_NULL; /* Initialize next state variable */
        next_event = HDX_ENTRY_EVENT;   /* Initialize next event variable */

        switch (ctx->hdx_tx_state) {
        /*
         * This is the quiescent state for this state machine. Transitions
         * out of this state happen when there is something to transmit
         * out of this interface. The state of the modem signals in this
         * state should be
         *     DTR  DSR  DCD  noRTS  noCTS
         * The normal exit path out of this state is via HDX_TX_EMPTY_EVENT.
         */
        case HDX_DTE_TX_STATE_READY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                clear_timer1(reg_table);
                deassert_RTS(idb, ctx);
                HDX_LOG_FIG(ctx, rts_deassert_fig);
                if (txqueue_nonempty(idb)) 
                    next_state = HDX_DTE_TX_STATE_TRANSMIT_DELAY;
                break; 
            case HDX_TX_EMPTY_EVENT:
                if (txqueue_nonempty(idb)) 
                    next_state = HDX_DTE_TX_STATE_TRANSMIT_DELAY;
                else 
                    ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_CTS_CHANGE_EVENT:
                if (DTE_CTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, cts_assert_fig);
                }
                else {
                    HDX_LOG_FIG(ctx, cts_deassert_fig);
                }
            case HDX_TIMER1_EVENT:
            case HDX_TX_EOF_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * This is an optional delay state for the DTE transmit state
         * machine. By default, no time is spent in this state by the
         * transmit state machine. However, the delay parameter can be
         * modified by a config command to allow delayed transmits which
         * may be required under some conditions. The state of the
         * modem signals in this state should be
         *	DTR  DSR  DCD  noRTS  noCTS
         * The normal exit path out of this state is via the HDX_TIMER1_EVENT.
         */
        case HDX_DTE_TX_STATE_TRANSMIT_DELAY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                if (ctx->transmit_delay->msecval != 0) {
                    start_timer1(reg_table, ctx->transmit_delay);
                }
                else {
                    next_state = HDX_DTE_TX_STATE_TRANSMIT_DELAY;
                    next_event = HDX_TIMER1_EVENT;
                }
                break;
            case HDX_TIMER1_EVENT:
                next_state = HDX_DTE_TX_STATE_WAIT_CTS;
                break;
            case HDX_CTS_CHANGE_EVENT:
                if (DTE_CTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, cts_assert_fig);
                }
                else {
                    HDX_LOG_FIG(ctx, cts_deassert_fig);
                }
            case HDX_TX_EMPTY_EVENT:
            case HDX_TX_EOF_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * In this state, the DTE interface asserts RTS and waits for CTS
         * to be asserted. The rts_timeout value is user configurable.
         * If CTS is already asserted, a transition is made to the transmit
         * state. The state of the modem signals in this state should be
         *     DTR  DSR  DCD  RTS  noCTS
         * The normal exit path out of this state is via HDX_CTS_CHANGE_EVENT. 
         */ 
        case HDX_DTE_TX_STATE_WAIT_CTS:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                assert_RTS(idb, ctx);
                HDX_LOG_FIG(ctx, rts_assert_fig);
                if (DTE_CTS_is_asserted(reg_table)) {
                    next_state = HDX_DTE_TX_STATE_TRANSMIT;
                }
                else {
                    if (ctx->rts_timeout->msecval != 0)
                        start_timer1(reg_table, ctx->rts_timeout);
                }
                break;
            case HDX_TIMER1_EVENT:
                idb->rts_timeout_count++;
                holdq_reset(idb);
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                    (*idb->tx_comp_notify)(idb);
                }
                next_state = HDX_DTE_TX_STATE_READY;
                break;
            case HDX_CTS_CHANGE_EVENT:
                if (DTE_CTS_is_asserted(reg_table)) {
                    next_state = HDX_DTE_TX_STATE_TRANSMIT;
                    HDX_LOG_FIG(ctx, cts_assert_fig);
                }
                else {
                    next_state = HDX_DTE_TX_STATE_READY;
                    HDX_LOG_FIG(ctx, cts_deassert_fig);
                }
                break;
            case HDX_TX_EOF_EVENT:
            case HDX_TX_EMPTY_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * This is the state from which the transmission of packets, as long
         * as there are some to transmit, takes place. When an interrupt
         * indicating end of frame transmission occurs, a check is made to
         * see if it was the last frame to be transmitted. The state of the
         * modem signals in this state should be
         *     DTR  DSR  DCD  RTS  CTS
         * The normal exit path out of this state is via HDX_TX_EOF_EVENT.
         */
        case HDX_DTE_TX_STATE_TRANSMIT:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                clear_timer1(reg_table);
                if (txqueue_nonempty(idb)) 
                    cd2430_transmit_packet(idb, ctx, reg_table);
                break;
            case HDX_TX_EOF_EVENT:
                /*
                 * Determine if packet just transmitted was the final packet
                 */
                buf_index = (reg_table->tisr & TISR_BUFB) ? 1 : 0;
                pak = ctx->outpak[buf_index];
                if (pak) {
                    if ((pak->datagramstart[1] & (uchar)SDLC_PF) || 
                                                                 bscConfigured)
                        final_frame = TRUE;
                    cd2430_release_tx_pak(idb, ctx, buf_index);
                }

                if (final_frame)  
                    next_state = HDX_DTE_TX_STATE_WAIT_TXMPTY;
                else 
                    next_state = HDX_DTE_TX_STATE_TRANSMIT;
                break;
            case HDX_TX_EMPTY_EVENT:
                if (txqueue_nonempty(idb))
                    cd2430_transmit_packet(idb, ctx, reg_table);
                else
                    ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_CTS_CHANGE_EVENT:
                if (DTE_CTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, cts_assert_fig);
                }
                else {
                    HDX_LOG_FIG(ctx, cts_deassert_fig);
                }
		reset_interface(ctx);
            case HDX_TIMER1_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * In this state the state machine is waiting for a TxEmpty interrupt
         * informing it that the final frame to be transmitted has made it
         * out to the wire completely. Once this happens, the state machine
         * can go ahead and drop RTS. The state of the modem signals in this
         * state is
         *     DTR  DSR  DCD  RTS  CTS
         * The normal exit path out of this state is via HDX_TX_EMPTY_EVENT.
         */
        case HDX_DTE_TX_STATE_WAIT_TXMPTY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                clear_timer1(reg_table);
                reg_table->ier |= IER_TXMPTY;
                break;
            case HDX_TX_EMPTY_EVENT:
                next_state = HDX_DTE_TX_STATE_WAIT_RTSDROP_DELAY;
                break;
            case HDX_CTS_CHANGE_EVENT:
                if (DTE_CTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, cts_assert_fig);
                }
                else {
                    HDX_LOG_FIG(ctx, cts_deassert_fig);
                }
		reset_interface(ctx);
                ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_TX_EOF_EVENT:
                buf_index = (reg_table->tisr & TISR_BUFB) ? 1 : 0;
                cd2430_release_tx_pak(idb, ctx, buf_index);
                reg_table->ier |= IER_TXMPTY; /* Enable this again */
		break;
            case HDX_TIMER1_EVENT:
                reg_table->ier |= IER_TXMPTY; /* Enable this again */
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * After a TxEmpty interrupt is taken following the transmission of
         * of the final frame, RTS is dropped after a user-configurable
         * delay of rts_drop_delay. The modem signals in this state are
         *     DTR  DSR  DCD  RTS  CTS
         * The normal exit path out of this state is via HDX_TIMER1_EVENT.
         */
        case HDX_DTE_TX_STATE_WAIT_RTSDROP_DELAY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                if (ctx->rts_drop_delay->msecval != 0) {
                    start_timer1(reg_table, ctx->rts_drop_delay);
                }
                else {
                    next_state = HDX_DTE_TX_STATE_WAIT_RTSDROP_DELAY;
                    next_event = HDX_TIMER1_EVENT;
                }
                break;
            case HDX_TIMER1_EVENT:
                next_state = HDX_DTE_TX_STATE_WAIT_CTSDROP;
                break;
            case HDX_CTS_CHANGE_EVENT:
                if (DTE_CTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, cts_assert_fig);
                }
                else {
                    HDX_LOG_FIG(ctx, cts_deassert_fig);
                }
		reset_interface(ctx);
            case HDX_TX_EOF_EVENT:
            case HDX_TX_EMPTY_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * After waiting for a time period equal to rts_drop_delay from the
         * time that the transmission of the final frame is complete, RTS
         * is dropped. Also, the state machine waits for a time period
         * equal to cts_drop_timeout for CTS to get deasserted. The state of
         * the modem signals in this state is
         *     DTR  DSR  DCD  noRTS  CTS
         * Thr normal exit path out of this state is via HDX_CTS_CHANGE_EVENT.
         */
        case HDX_DTE_TX_STATE_WAIT_CTSDROP:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                deassert_RTS(idb, ctx);
                HDX_LOG_FIG(ctx, rts_deassert_fig);
                if (ctx->cts_drop_timeout->msecval != 0)
                    start_timer1(reg_table, ctx->cts_drop_timeout);
                break;
            case HDX_TIMER1_EVENT:
                ctx->cts_deassertion_fail++;
                next_state = HDX_DTE_TX_STATE_READY;
                break;
            case HDX_CTS_CHANGE_EVENT:
                if (DTE_CTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, cts_assert_fig);
                }
                else {
                    HDX_LOG_FIG(ctx, cts_deassert_fig);
                }
                next_state = HDX_DTE_TX_STATE_READY;
                break;
            case HDX_TX_EOF_EVENT:
            case HDX_TX_EMPTY_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
 
        /*
         * If there is another state transition to be done, set the event to
         * indicate the HDX_ENTRY_EVENT into that state and update the
         * half-duplex state in the context structure to indicate the new
         * state.
         */
        if (next_state != HDX_TX_STATE_NULL) {
            ctx->hdx_tx_state = next_state;
            hdx_event = next_event;
        }
    }
}

/*
 * Check to see that the event-state combination for the DTE half-duplex
 * receive state machine is valid.
 */
inline static boolean valid_dte_rx_fsm_state_event (enum HDX_RX_STATE state,
                                                    enum HDX_EVENT event)
{
    if ((state == HDX_DTE_RX_STATE_READY) || /* First check the state ... */
        (state == HDX_DTE_RX_STATE_INGIANT)) { /* ... then check event */
        if ((event == HDX_ENTRY_EVENT)        ||
            (event == HDX_RX_ABORT_EVENT)     ||
            (event == HDX_RX_CRC_ERROR_EVENT) ||
            (event == HDX_RX_OVERRUN_EVENT)   ||
            (event == HDX_RX_EOB_EVENT)       ||
            (event == HDX_RX_EOF_EVENT)) {
            return (TRUE);
        }
    }
    return (FALSE);  /* Invalid state-event combination */
}

/*
 * The DTE Half-duplex Receive Finite State Machine
 */
static void cd2430_sync_hdx_dte_rx_fsm (hwidbtype *idb, ushort curbuf, 
                                        enum HDX_EVENT hdx_event)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    paktype *pak;
    paktype *retpak;
    enum HDX_RX_STATE next_state;
    enum HDX_EVENT next_event;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    pak = retpak = ctx->inpak[curbuf];
    next_state = ctx->hdx_rx_state;

    /*
     * Loop until all state changes necessary in this invocation of the
     * state machine are done.
     */
    while (next_state != HDX_RX_STATE_NULL) {

        /*
         * Log debug information
         */
        HDX_LOG(ctx, dte_rx_fsm, hdx_rx_state_list[ctx->hdx_rx_state-50], 
                            hdx_event_list[hdx_event]);

        /*
         * Check to ensure that this combination and the state and event
         * is a valid one for this state machine.
         */
        if (!valid_dte_rx_fsm_state_event(ctx->hdx_rx_state, hdx_event)) {
            errmsg(&msgsym(BADHDXFSM, CIRRUS), idb->unit, ctx->hdx_tx_state,
                                                                   hdx_event);
            return;
        }

        next_state = HDX_RX_STATE_NULL; /* Initialize next state variable */
        next_event = HDX_ENTRY_EVENT;   /* Initialize next event variable */
    
        switch (ctx->hdx_rx_state) {
        /*
         * This is the quiescent state for this state machine. When the 
         * state machine is idle or when it is receiving frames, it is
         * in this state. The only way of exiting this path is if a
         * giant frame is received. The modem signals in this state are
         *     DTR  DSR  DCD  noRTS  noCTS
         */ 
        case HDX_DTE_RX_STATE_READY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                break;
            case HDX_RX_ABORT_EVENT:
            case HDX_RX_CRC_ERROR_EVENT:
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                   retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, FALSE);
                }
            case HDX_RX_OVERRUN_EVENT:
                break;
            case HDX_RX_EOB_EVENT:
                next_state = HDX_DTE_RX_STATE_INGIANT;
                break;
            case HDX_RX_EOF_EVENT:
                retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, FALSE);
                break;
            default:
                break;
            }
            break;
        /*
         * This state machine enters this state if a giant frame is 
         * received. An error event or the end of frame event will 
         * result in the state machine returning to the Ready state.
         * The state of the modem signals in this state is
         *     DTR  DSR  DCD  noRTS  noCTS
         */
        case HDX_DTE_RX_STATE_INGIANT:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                idb->counters.giants++;
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                    retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, TRUE);
                    next_state = HDX_DTE_RX_STATE_READY;
                }
                break;
            case HDX_RX_ABORT_EVENT:
            case HDX_RX_CRC_ERROR_EVENT:
            case HDX_RX_OVERRUN_EVENT:
            case HDX_RX_EOF_EVENT:
                next_state = HDX_DTE_RX_STATE_READY;
                break;
            case HDX_RX_EOB_EVENT:
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        /*
         * If the received packet was not passed to higher layers for 
         * processing, then there must have been an error condition of
         * some sort. Simply re-arm this packet for receive DMA.
         */
        if (retpak) {
            arm_rx_dma(reg_table, curbuf, VTOP(retpak->datagramstart),
		       idb->max_buffer_size);
            retpak = NULL;
        }

        /*
         * If there is another state transition to be done, set the event to
         * indicate the HDX_ENTRY_EVENT into that state and update the
         * half-duplex state in the context structure to indicate the new
         * state.
         */
        if (next_state != HDX_RX_STATE_NULL) {
            ctx->hdx_rx_state = next_state;
            hdx_event = next_event;
        }
    }
}

/*
 * Check to see that the event-state combination for the DTE half-duplex
 * receive state machine is valid.
 */
inline static boolean valid_dce_tx_fsm_state_event (enum HDX_TX_STATE state,
                                                    enum HDX_EVENT event)
{
    if ((state == HDX_DCE_TX_STATE_READY)    || /* First check the state ... */
        (state == HDX_DCE_TX_STATE_TRANSMIT_DELAY)    ||
        (state == HDX_DCE_TX_STATE_DCD_TXSTART_DELAY) ||
        (state == HDX_DCE_TX_STATE_TRANSMIT) ||
        (state == HDX_DCE_TX_STATE_DCD_DROP_DELAY) ||
        (state == HDX_DCE_TX_STATE_WAIT_TXMPTY)) { /* ... then check event */
        if ((event == HDX_ENTRY_EVENT)  ||
            (event == HDX_TIMER2_EVENT) ||
            (event == HDX_TX_EOF_EVENT) ||
            (event == HDX_TX_EMPTY_EVENT)) {
            return (TRUE);
        }
    }
    return (FALSE);  /* Invalid state-event combination */
}

/*
 * The DCE Half-duplex Transmit Finite State Machine
 */
static void cd2430_sync_hdx_dce_tx_fsm (hwidbtype *idb, 
                                        enum HDX_EVENT hdx_event)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    paktype *pak;
    boolean final_frame = FALSE;
    int buf_index;
    serialsb *ssb;
    enum HDX_TX_STATE next_state;
    enum HDX_EVENT next_event;
    uchar bscConfigured;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    bscConfigured = (idb->poll_protocol_flags & PP_ACTIVE);
    next_state = ctx->hdx_tx_state;

    /*
     * Loop until all state changes necessary in this invocation of the
     * state machine are done.
     */
    while (next_state != HDX_TX_STATE_NULL) {

        /*
         * Log debug information
         */
        HDX_LOG(ctx, dce_tx_fsm, hdx_tx_state_list[ctx->hdx_tx_state-10], 
                            hdx_event_list[hdx_event]);

        /*
         * Check to ensure that this combination of the state and event
         * is a valid one for this state machine.
         */
        if (!valid_dce_tx_fsm_state_event(ctx->hdx_tx_state, hdx_event)) {
            errmsg(&msgsym(BADHDXFSM, CIRRUS), idb->unit, ctx->hdx_tx_state,
                                                                   hdx_event);
            return;
        }

        next_state = HDX_TX_STATE_NULL; /* Initialize next state variable */
        next_event = HDX_ENTRY_EVENT;   /* Initialize next event variable */
    
        switch (ctx->hdx_tx_state) {
        /*
         * This is the quiescent state for this state machine. If there is
         * something to transmit in the output queue, the state machine
         * transitions to the transmit state. That there is something in
         * the output queue is usually signalled by the occurence of a 
         * TxEmpty interrupt. The state of the modem signals in this state
         * is
         *     DTR  DSR  DCD    noRTS  noCTS  -- Constant carrier mode
         *     DTR  DSR  noDCD  noRTS  noCTS  -- Controller carrier mode
         */
        case HDX_DCE_TX_STATE_READY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                if (ssb->controlled_carrier) {
                    deassert_DCD(idb, ctx);
                    HDX_LOG_FIG(ctx, dcd_deassert_fig);
                }
                if (txqueue_nonempty(idb)) {
                    next_state = HDX_DCE_TX_STATE_TRANSMIT_DELAY;
                }
                break;
            case HDX_TIMER2_EVENT:
            case HDX_TX_EOF_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_TX_EMPTY_EVENT:
                if (txqueue_nonempty(idb)) {
                    next_state = HDX_DCE_TX_STATE_TRANSMIT_DELAY;
                }
                else {
                    ctx->false_event_counters[hdx_event]++;
                }
                break;
            default:
                break;
            }
            break;
        /*
         * This is an optional delay state for the DCE transmit state
         * machine. By default, no time is spent in this state by the
         * transmit state machine. However, the delay parameter can be
         * modified by a config command to allow delayed transmits which
         * may be required under some conditions. The state of the
         * modem signals in this state should be
         *      DTR  DSR  DCD    noRTS  noCTS  -- Constant carrier mode
         *      DTR  DSR  noDCD  noRTS  noCTS  -- Controller carrier mode
         * The normal exit path out of this state is via the HDX_TIMER1_EVENT.
         */
        case HDX_DCE_TX_STATE_TRANSMIT_DELAY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                if (ctx->transmit_delay->msecval != 0) {
                    start_timer2(reg_table, ctx->transmit_delay);
                }
                else {
                    next_state = HDX_DCE_TX_STATE_TRANSMIT_DELAY;
                    next_event = HDX_TIMER2_EVENT;
                }
                break;
            case HDX_TIMER2_EVENT:
                if (ssb->controlled_carrier)
                    next_state = HDX_DCE_TX_STATE_DCD_TXSTART_DELAY;
                else
                    next_state = HDX_DCE_TX_STATE_TRANSMIT;
                break;
            case HDX_TX_EOF_EVENT:
            case HDX_TX_EMPTY_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * In controlled carrier mode of operation, the state machine waits
         * in this state after asserting DCD. The duration of this delay
         * is user adjustable. The state of the modem signals in this state
         * should be
         *	DTR  DSR  DCD  noRTS  noCTS
         * The normal exit path out of this state is via the
         * HDX_TIMER2_EVENT.
         */
        case HDX_DCE_TX_STATE_DCD_TXSTART_DELAY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                assert_DCD(idb, ctx);
                HDX_LOG_FIG(ctx, dcd_assert_fig);
                if (ctx->dcd_txstart_delay->msecval != 0) {
                    start_timer2(reg_table, ctx->dcd_txstart_delay);
                }
                else {
                    next_state = HDX_DCE_TX_STATE_DCD_TXSTART_DELAY;
                    next_event = HDX_TIMER2_EVENT;
                }
                break;
            case HDX_TIMER2_EVENT:
                next_state = HDX_DCE_TX_STATE_TRANSMIT;
                break;
            case HDX_TX_EOF_EVENT:
            case HDX_TX_EMPTY_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * This is the state from which the transmission of packets, as long
         * as there are some to transmit, takes place. When an interrupt
         * indicating end of frame transmission occurs, a check is made to
         * see if it was the last frame to be transmitted. The state of the
         * modem signals in this state should be
         *     DTR  DSR  DCD  noRTS  noCTS
         * The normal exit path out of this state is via HDX_TX_EOF_EVENT.
         */
        case HDX_DCE_TX_STATE_TRANSMIT:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                if (txqueue_nonempty(idb)) 
                    cd2430_transmit_packet(idb, ctx, reg_table);
                break;
            case HDX_TX_EOF_EVENT:
                /*
                 * Determine if packet just transmitted was the final packet
                 */
                buf_index = (reg_table->tisr & TISR_BUFB) ? 1 : 0;
                pak = ctx->outpak[buf_index];
                if (pak) {
                    if ((pak->datagramstart[1] & (uchar)SDLC_PF) || 
                                                                 bscConfigured)
                        final_frame = TRUE;
                    cd2430_release_tx_pak(idb, ctx, buf_index);
                }
                if (final_frame) 
                    next_state = HDX_DCE_TX_STATE_WAIT_TXMPTY;
                else 
                    next_state = HDX_DCE_TX_STATE_TRANSMIT;
                break;
            case HDX_TX_EMPTY_EVENT:
                if (txqueue_nonempty(idb))
                    cd2430_transmit_packet(idb, ctx, reg_table);
                else
                    ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_TIMER2_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * In this state the state machine is waiting for a TxEmpty interrupt
         * informing it that the final frame to be transmitted has made it
         * out to the wire completely. Once this happens, the state machine
         * returns to the Ready state. The state of the modem signals in this
         * state is
         *     DTR  DSR  DCD  noRTS  noCTS
         * The normal exit path out of this state is via HDX_TX_EMPTY_EVENT.
         */
        case HDX_DCE_TX_STATE_WAIT_TXMPTY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                reg_table->ier |= IER_TXMPTY;
                break;
            case HDX_TX_EOF_EVENT:
                buf_index = (reg_table->tisr & TISR_BUFB) ? 1 : 0;
                cd2430_release_tx_pak(idb, ctx, buf_index);
                reg_table->ier |= IER_TXMPTY; /* Enable this again */
                break;
            case HDX_TIMER2_EVENT:
                reg_table->ier |= IER_TXMPTY; /* Enable this again */
                ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_TX_EMPTY_EVENT:
                if (ssb->controlled_carrier)
                    next_state = HDX_DCE_TX_STATE_DCD_DROP_DELAY;
                else
                    next_state = HDX_DCE_TX_STATE_READY;
                break;
            default:
                break;
            }
            break;
        /*
         * This is the state of the state of the FSM where we wait for
         * a certain duration after the last frame has been transmitted
         * prior to deasserting DCD. This time parameter is user
         * adjustable. The state of the modem signals in this state is
         *	DTR  DSR  DCD  noRTS  noDTR
         * The normal exit path out of this state is via  the
         * HDX_TIMER2_EVENT.
         */
        case HDX_DCE_TX_STATE_DCD_DROP_DELAY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                if (ctx->dcd_drop_delay->msecval != 0) {
                    start_timer2(reg_table, ctx->dcd_drop_delay);
                }
                else {
                    next_state = HDX_DCE_TX_STATE_DCD_DROP_DELAY;
                    next_event = HDX_TIMER2_EVENT;
                }
                break;
            case HDX_TIMER2_EVENT:
                next_state = HDX_DCE_TX_STATE_READY;
                break;
            case HDX_TX_EOF_EVENT:
            case HDX_TX_EMPTY_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }

        /*
         * If there is another state transition to be done, set the event to
         * indicate the HDX_ENTRY_EVENT into that state and update the
         * half-duplex state in the context structure to indicate the new
         * state.
         */
        if (next_state != HDX_TX_STATE_NULL) {
            ctx->hdx_tx_state = next_state;
            hdx_event = next_event;
        }
    }
}

/*
 * Check to see that the event-state combination for the DTE half-duplex
 * receive state machine is valid.
 */
inline static boolean valid_dce_rx_fsm_state_event (enum HDX_RX_STATE state,
                                                    enum HDX_EVENT event)
{
    if ((state == HDX_DCE_RX_STATE_READY)     || /* First check the state ... */
        (state == HDX_DCE_RX_STATE_CTS_DELAY) ||
        (state == HDX_DCE_RX_STATE_RECEIVE)   ||
        (state == HDX_DCE_RX_STATE_INGIANT)) { /* ... then check event */
        if ((event == HDX_ENTRY_EVENT)        ||
            (event == HDX_TIMER1_EVENT)       ||
            (event == HDX_RTS_CHANGE_EVENT)   || 
            (event == HDX_RX_ABORT_EVENT)     ||
            (event == HDX_RX_CRC_ERROR_EVENT) ||
            (event == HDX_RX_OVERRUN_EVENT)   ||
            (event == HDX_RX_EOB_EVENT)       ||
            (event == HDX_RX_EOF_EVENT)) {
            return (TRUE);
        }
    }
    return (FALSE);  /* Invalid state-event combination */
}

/*
 * The DCE Half-duplex Recieve Finite State Machine
 */
static void cd2430_sync_hdx_dce_rx_fsm (hwidbtype *idb, ushort curbuf,
                                        enum HDX_EVENT hdx_event)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    paktype *pak;
    paktype *retpak;
    enum HDX_RX_STATE next_state;
    enum HDX_EVENT next_event;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    pak = ctx->inpak[curbuf];
    next_state = ctx->hdx_rx_state;
    retpak = NULL;

    /*
     * Loop until all state changes necessary in this invocation of the
     * state machine are done.
     */
    while (next_state != HDX_RX_STATE_NULL) {

        /*
         * Log debug information
         */
        HDX_LOG(ctx, dce_rx_fsm, hdx_rx_state_list[ctx->hdx_rx_state-50], 
                            hdx_event_list[hdx_event]);

        /*
         * Check to ensure that this combination and the state and event
         * is a valid one for this state machine.
         */
        if (!valid_dce_rx_fsm_state_event(ctx->hdx_rx_state, hdx_event)) {
            errmsg(&msgsym(BADHDXFSM, CIRRUS), idb->unit, ctx->hdx_rx_state,
                                                                   hdx_event);
            return;
        }

        next_state = HDX_RX_STATE_NULL; /* Initialize next state variable */
        next_event = HDX_ENTRY_EVENT;   /* Initialize next event variable */

        switch (ctx->hdx_rx_state) {
        /*
         * This is the quiescent state for this state machine. When the
         * DTE is ready to transmit it signals the DCE by asserting RTS.
         * Thus the normal exit path from this state is via the
         * HDX_RTS_CHANGE_EVENT. The state of the modem signals in this
         * state is
         *     DTR  DSR  DCD  noRTS  noCTS
         */
        case HDX_DCE_RX_STATE_READY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                clear_timer1(reg_table);
                deassert_CTS(idb, ctx);
                HDX_LOG_FIG(ctx, cts_deassert_fig);
                break;
            case HDX_RTS_CHANGE_EVENT:
                if (DCE_RTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, rts_assert_fig); 
                    next_state = HDX_DCE_RX_STATE_CTS_DELAY;
                }
                else {
                    HDX_LOG_FIG(ctx, rts_deassert_fig);
                }
                break;
            case HDX_RX_EOF_EVENT:
                retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, FALSE);
                break;
            case HDX_RX_ABORT_EVENT:
            case HDX_RX_CRC_ERROR_EVENT:
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                   retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, FALSE);
		   ctx->false_event_counters[hdx_event]++;
		   break;
                }
            case HDX_RX_OVERRUN_EVENT:
            case HDX_RX_EOB_EVENT:
	        retpak = pak;
            case HDX_TIMER1_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * In this state, the state machine is delaying for a time duration
         * equal to cts_delay prior to asserting CTS. This time duration is
         * user-configurable. CTS is asserted when the timer expires. Thus
         * the normal exit path out of this state is via the HDX_TIMER1_EVENT.
         * The state of the modem signals in this state is
         *     DTR  DSR  DCD  RTS  noCTS
         */ 
        case HDX_DCE_RX_STATE_CTS_DELAY:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                if (ctx->cts_delay->msecval != 0) {
                    start_timer1(reg_table, ctx->cts_delay);
                }
                else {
                    next_state = HDX_DCE_RX_STATE_CTS_DELAY;
                    next_event = HDX_TIMER1_EVENT;
                }
                break;
            case HDX_TIMER1_EVENT:
                next_state = HDX_DCE_RX_STATE_RECEIVE;
                break;
            case HDX_RTS_CHANGE_EVENT:
                if (!DCE_RTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, rts_deassert_fig); 
                    next_state = HDX_DCE_RX_STATE_READY;
                }
                else {
                    HDX_LOG_FIG(ctx, rts_assert_fig);
                }
                break;
            case HDX_RX_ABORT_EVENT:
            case HDX_RX_CRC_ERROR_EVENT:
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                   retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, FALSE);
		   ctx->false_event_counters[hdx_event]++;
		   break;
                }
            case HDX_RX_OVERRUN_EVENT:
            case HDX_RX_EOB_EVENT:
            case HDX_RX_EOF_EVENT:
	        retpak = pak;
                ctx->false_event_counters[hdx_event]++;
                break;
            default:
                break;
            }
            break;
        /*
         * This is state for this state machine where it actively receives
         * data from the DTE. If any giant packet is received it transitions
         * to the InGiant state until an end of frame event occurs. The
         * normal exit path out of this state is via the HDX_RTS_CHANGE_EVENT.
         * Once the DTE is done sending data, it deasserts RTS which causes
         * the exit event. The state of the meodem signals in this state
         * is
         *     DTR  DSR  DCD  RTS  CTS
         */
        case HDX_DCE_RX_STATE_RECEIVE:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                clear_timer1(reg_table);
                assert_CTS(idb, ctx);
                HDX_LOG_FIG(ctx, cts_assert_fig);
                break;
            case HDX_TIMER1_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_RTS_CHANGE_EVENT:
                if (!DCE_RTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, rts_deassert_fig);
                    next_state = HDX_DCE_RX_STATE_READY;
                }
                else {
                    HDX_LOG_FIG(ctx, rts_assert_fig);
                }
                break;
            case HDX_RX_ABORT_EVENT:
            case HDX_RX_CRC_ERROR_EVENT:
	        retpak = pak;
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                   retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, FALSE);
                }
                break;
            case HDX_RX_EOB_EVENT:
                next_state = HDX_DCE_RX_STATE_INGIANT;
            case HDX_RX_OVERRUN_EVENT:
	        retpak = pak;
                break;
            case HDX_RX_EOF_EVENT:
                retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, FALSE);
                break;
            default:
                break;
            }
            break;
        /*
         * This state machine enters this state if a giant frame is
         * received. An error event or the end of frame event will
         * result in the state machine returning to the Receive state.
         * The state of the modem signals in this state is
         *     DTR  DSR  DCD  RTS  CTS
         */
        case HDX_DCE_RX_STATE_INGIANT:
            switch (hdx_event) {
            case HDX_ENTRY_EVENT:
                clear_timer1(reg_table);
                idb->counters.giants++;
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                    retpak = cd2430_receive_packet(idb, ctx, reg_table, pak, TRUE);
                    next_state = HDX_DCE_RX_STATE_RECEIVE;
                }
                break;
            case HDX_TIMER1_EVENT:
                ctx->false_event_counters[hdx_event]++;
                break;
            case HDX_RTS_CHANGE_EVENT:
                if (!DCE_RTS_is_asserted(reg_table)) {
                    HDX_LOG_FIG(ctx, rts_deassert_fig);
                    next_state = HDX_DCE_RX_STATE_READY;
                }
                else {
                    HDX_LOG_FIG(ctx, rts_assert_fig);
                }
                break;
            case HDX_RX_ABORT_EVENT:
            case HDX_RX_CRC_ERROR_EVENT:
            case HDX_RX_OVERRUN_EVENT:
            case HDX_RX_EOF_EVENT:
                next_state = HDX_DCE_RX_STATE_RECEIVE;
            case HDX_RX_EOB_EVENT:
	        retpak = pak;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    
        /*
         * If the received packet was not passed to higher layers for
         * processing, then there must have been an error condition of
         * some sort. Simply re-arm this packet for receive DMA.
         */
        if (retpak) {
            arm_rx_dma(reg_table, curbuf, VTOP(retpak->datagramstart), 
		       idb->max_buffer_size);
            retpak  = NULL;
        }

        /*
         * If there is another state transition to be done, set the event to
         * indicate the HDX_ENTRY_EVENT into that state and update the
         * half-duplex state in the context structure to indicate the new
         * state.
         */
        if (next_state != HDX_RX_STATE_NULL) {
            ctx->hdx_rx_state = next_state;
            hdx_event = next_event;
        }
    }
}

/*
 * This routine processes changes in the state of the DTR, DSR and DCD
 * modem signals and indicates the state of the interface (up/down)
 * based on these changes.
 */
static void cd2430_sync_line_state (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    serialsb *ssb;
    int newstate;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    newstate = idb->state;

    if (cable_present(ctx)) {
        if (idb->state != IDBS_ADMINDOWN) {
            if (idb->dte_interface) {
                if (ssb->ignore_dcd) {  /* DSR is the line up signal */
                    if (DTE_DSR_is_asserted(reg_table)) {
                        newstate = IDBS_UP;
                    }
                    else {  /* DSR is not asserted */
                        newstate = IDBS_DOWN;
                        idb->failCause = Serial_SignalLost;
                        if (serial_debug)
                           buginf("\nCIRRUS(%d): %s is down", idb->unit, "DSR");
                    }
                }
                else {  /* DCD is the line up signal */
                    if (DTE_DCD_is_asserted(reg_table)) {
                        newstate = IDBS_UP;
                    }
                    else {  /* DCD is not asserted */
                        newstate = IDBS_DOWN;
                        idb->failCause = Serial_SignalLost;
                        if (serial_debug)
                           buginf("\nCIRRUS(%d): %s is down", idb->unit, "DCD");
                    }
                }
            }
            else {  /* This is a DCE interface */
                if (DCE_DTR_is_asserted(reg_table)) {
                    newstate = IDBS_UP;
                }
                else {  /* DTR is not asserted */
                    newstate = IDBS_DOWN;
                    idb->failCause = Serial_SignalLost;
                    if (serial_debug)
                       buginf("\nCIRRUS(%d): %s is down", idb->unit, "DTR");
                }
            }
        }
        else {  /* The interface is admin shutdown */
            disable_modem_interrupts(reg_table);
            deassert_modem_signals(idb);
            if (idb->dte_interface) {
                HDX_LOG_FIG(ctx, dtr_deassert_fig);
                HDX_LOG_FIG(ctx, rts_deassert_fig);
            }
            else {
                HDX_LOG_FIG(ctx, dcd_deassert_fig);
                HDX_LOG_FIG(ctx, dsr_deassert_fig);
                HDX_LOG_FIG(ctx, cts_deassert_fig);
            }
        }
    }
    else {  /* The cable is not present */
        newstate = IDBS_DOWN;
        idb->failCause = Serial_SignalLost;
        if (serial_debug)
            buginf("\nCIRRUS(%d): No cable present", idb->unit);
        disable_modem_interrupts(reg_table);
        deassert_modem_signals(idb);
        if (idb->dte_interface) {
            HDX_LOG_FIG(ctx, dtr_deassert_fig);
            HDX_LOG_FIG(ctx, rts_deassert_fig);
        }
        else {
            HDX_LOG_FIG(ctx, dcd_deassert_fig);
            HDX_LOG_FIG(ctx, dsr_deassert_fig);
            HDX_LOG_FIG(ctx, cts_deassert_fig);
        }
    }

    if (idb->state != IDBS_ADMINDOWN) {
        if (reg_invoke_dialer_spoofing_int(idb)) {
            if (newstate == IDBS_UP) {
                idb->counters.transitions++;
                net_cstate(idb, newstate);
            }
        }
        else {  /* Not spoofing */
            if (idb->state != newstate) {
                idb->counters.transitions++;
                net_cstate(idb, newstate);
            }
        }
    }
}


/***************************************************************************
 *     The Interrupt Service Routines for the CD2430 Sync Driver
 *
 * The interrupt service routines for the synchronous operation of the
 * Cirrus CL-CD2430 controller are included in this section of the
 * driver. The four kinds of interrupts generated by this controller are
 *     o Receive data interrupts
 *     o Transmit data interrupts
 *     o Modem signal change and timer interrupts
 *     o Receive good data interrupts
 *
 * The interrupt service routines for the full-duplex and half-duplex
 * cases are separate and are both listed below. Note that the Receive
 * Good Data interrupts must never occur since this is a DMA driver.
 * However, we have a bare-bones interrupt service routine to handle
 * potentially spurious interrupts of that kind. This service routine is
 * the same in full-duplex and half-duplex modes.
 **************************************************************************/

/*
 * This is the ISR to handle Receive Data Interrupts in the full-duplex
 * case. For this driver this is the main code path for DMA read 
 * completions.
 */
static void cd2430_sync_rxint (void *idbptr)
{
    hwidbtype *idb;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    paktype *pak = NULL;
    paktype *newpak = NULL;
    ushort curbuf, status, bufstatus, count;
    leveltype level;
    static boolean active_flag = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * First, check for re-entrancy. Since this service routine should never
     * be re-entered, treat re-entrancy as a catastrophic error.
     */
    if (active_flag)
        crashdump(0);
    else
        active_flag = TRUE;

    /*
     * Get the register table for this channel and the context structure
     * for this channel from the IDB pointer.
     */
    idb = (hwidbtype *)idbptr;
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    /*
     * Indicate activity on this interface by turning on the LED and
     * count the occurrence of this interrupt.
     */
    LSS_LED_ACTIVATE(ctx);
    ctx->rxint_cnt++;

    /*
     * Read the RISR to figure out which buffer just finished receiving
     * a packet. Also, get the buffer status of the concerned buffer.
     */
    status = reg_table->risr;
    curbuf = (status & RISR_BUFB) ? 1 : 0;
    bufstatus = curbuf ? reg_table->brbsts : reg_table->arbsts;
    count = 0;

    /* Get the packet whose reception interrupted us */
    pak = ctx->inpak[curbuf];

    if (bufstatus & RBSTS_EOB) { /* EOB bit set in DMA buffer status reg */
        /*
         * If we have a scheduled run interval configured, check if we
         * need to drop packets to give processes a chance to run.
         */
        if (sched_run_interval && AWAKE(sched_run_time)) {
            /*
             * The scheduler has not run recently; Drop packet
             */
            idb->counters.input_drops++;
            MAYBE_DROPPED(idb);
        }
        else if (status & RISR_ERROR) { /* Error indicated in status bits */
            if (status & RISR_CRC) {
                /* if BSC wish to receive the CRC errored frame */
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                   count = (curbuf ? reg_table->brbcnt : reg_table->arbcnt);
                }
                else {
                   idb->counters.input_crc++;
                }
            }
            else if (status & RISR_RXABT) {
	        /* The Cirrus chip treats garbage at the end of a frame
                 * as an aborted frame.  The Hitatchi chip doesn't tell
                 * us about short frames, and we don't want the Cirrus 
                 * interfaces to appear different.  So we count these to 
                 * be reported in show controller and decrement 
                 * input_error because its incremented later.
                 */

                /* if BSC wish to receive the abort frame */
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                    count = (curbuf ? reg_table->brbcnt : reg_table->arbcnt);
                    idb->counters.input_abort++;
		}
                else if ((curbuf ? reg_table->brbcnt : reg_table->arbcnt)
                            < SMALLEST_FRAME_SIZE) { 
                    ctx->aborted_short_cnt++;
                    idb->counters.input_error--;
                }
                else
                    idb->counters.input_abort++;
            }
            else if (status & RISR_OE) {
                idb->counters.input_overrun++;
            }
	    else if (status & RISR_RESIND) {
	        ctx->resind_cnt++;
	    }
            else {
                /*
                 * This is serious. Signal the periodic routine for this
                 * interface that this interface is to be reset the next 
                 * time that periodic routine executes.
                 */
	        if (status & RISR_BERR) {
	            ctx->buserr_cnt++;
		}
                idb->counters.input_frame++;
		reset_interface(ctx);
            }
            /* Note that an input error has occurred */
            idb->counters.input_error++;

            /* Clean up any giants that were in progress */
            ctx->ingiant = FALSE;
            MAYBE_DROPPED(idb);
        }
        else if (status & RISR_EOB) { /* No packet error indicated */
            count = (curbuf ? reg_table->brbcnt : reg_table->arbcnt);
            /*
             * Look at the EOF status bit, and use the knowledge of whether
             * or not we are in a giant, to figure out if we are maybe
             * beginning or finishing reception of a giant packet.
             */
            if (!(status & RISR_EOF)) { /* End of frame bit not set */
                if (!ctx->ingiant) { /* Giant just started coming in */
                    idb->counters.giants++;
                    cd2430_receive_packet(idb, ctx, reg_table, pak, TRUE);

                    reg_table->reoir = 0;
                    /*
                     * Clear potentially nested receive interrupt.
                     */
                    cd2430_clear_nested_rxint(ctx, reg_table, 0);

                    active_flag = FALSE;
                    reset_interrupt_level(level);
                    return;
                } else {
                    ctx->ingiant = TRUE;
                }
                count = 0;
            }
            else { /* End of frame bit is set in the status register */
                if (ctx->ingiant) { /* Currently in a giant */
                    count = 0;
                    ctx->ingiant = FALSE;
                }
            }
        }
        else {
            /*
             * There was no error in the received frame but EOB is not
             * set. This should never happen.
             */
            errmsg(&msgsym(BADRXISR, CIRRUS), idb->unit);
        }

        /*
         * If there is data to be processed, do so. Note that we can decide
         * whether or not there is data to be processed by looking at the
         * value of count. If there was an error, or if this packet was
         * either the start of a giant or part of a giant packet, then
         * count will be zero. It will be greater than zero only if there
         * was good data in the packet.
         */
        if (count > 0) {
            /*
             * Try to get a buffer to replace this received packet buffer.
             * If we cannot get a replacement buffer, we will drop the
             * packet just received and re-arm the existing buffer for
             * receive DMA.
             */
            newpak = les_driver_getbuf_inline(idb, TRUE);
            if (newpak) { /* Replacement packet buffer was available */
                /* Give the replacement buffer to the Cirrus */
                arm_rx_dma(reg_table, curbuf,
                           VTOP(newpak->datagramstart), idb->max_buffer_size);
                ctx->inpak[curbuf] = newpak;
                pak->datagramsize = count;

                /*
                 * Finish the interrupt now. Do not talk to the Cirrus after
                 * this.
                 */
                reg_table->reoir = 0;
                /*
                 * Clear potentially nested receive interrupt.
                 */
                cd2430_clear_nested_rxint(ctx, reg_table, 0);

                /* Process the received packet */
                set_if_input(pak, idb->firstsw);

                /*
                 * If bisync, we need to do some processing to
                 * determine the frame type.
                 */
                if (idb->poll_protocol_flags & PP_ACTIVE) {
                    (*idb->rx_framer_int)(idb, &pak);
                }

                if (pak) {
                    GET_TIMESTAMP(idb->lastinput);
                    /*
                     * If this is a DCE interface in loopback mode, just
                     * retransmit the packet out on the same interface.
                     * If it is a test packet, pass it to the test routine
                     * else pass it up for processing.
                     */
		    if (!idb->dte_interface && !ctx->ignore_local_loopback &&
 			DCE_loopback_is_asserted(ctx)) {
		        pak->if_output = idb->firstsw;
                        holdq_enqueue_fast(idb, pak);
                        (*idb->soutput)(idb);
                        idb->counters.rx_cumbytes += pak->datagramsize;
                        idb->counters.inputs++;
                    }
                    else
                        serial_RX_interrupt_inline(idb, pak);
                }
                active_flag = FALSE;
                reset_interrupt_level(level);
                return;
            }
            else if ((ctx->enctype == ET_HDLC) ||
                     (ctx->enctype == ET_FRAME_RELAY)) {
                /*
                 * We could not get a new buffer of size equal to the
                 * maximum transmission unit of this serial line.
                 * Are we an HDLC or Frame relay encapsulated serial line?
                 * If so, if this packet is a high priority packet (keepalive,
                 * etc.) try to get a smaller buffer big enough to copy the
                 * packet. Else, drop the packet.
                 */
                vhdlctype *hdlc;
                hdlc = (vhdlctype *)pak->datagramstart;
                if (serial_high_priority(hdlc->var_hdlctype)) {
                    cd2430_process_hi_priority_pak(idb, pak);
                }
                else {  /* This is not a high-priority packet */
                    idb->counters.input_drops++;
                    MAYBE_DROPPED(idb);
                }
            }
            else {
                /*
                 * Could not get a replacement buffer.  Also, this not a
                 * HDLC or frame-relay frame.  No recourse now other
                 * than to drop this packet.
                 */
                idb->counters.input_drops++;
                MAYBE_DROPPED(idb);
            }
        }
    }

    /*
     * If we decided to drop the received packet, or if the received packet
     * was in error, we did not pass it higher up for processing. Now, we
     * will simply re-arm this packet for receive DMA by transferring
     * ownership back to the Cirrus.
     */
    if ( pak )
       arm_rx_dma(reg_table, curbuf,
                  VTOP(pak->datagramstart), idb->max_buffer_size);

    reg_table->reoir = 0;
    /*
     * Clear potentially nested receive interrupt.
     */
    cd2430_clear_nested_rxint(ctx, reg_table, 0);

    active_flag = FALSE;
    reset_interrupt_level(level);
}

/*
 * cd2430_sync_hdx_rxint()
 * The half-duplex receive interrupt service routine for the CD2430
 */
static void cd2430_sync_hdx_rxint (void *idbptr)
{
    hwidbtype *idb;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    leveltype level;
    ushort curbuf, status;
    static boolean active_flag = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * First, check for re-entrancy. Since this service routine should never
     * be re-entered, treat re-entrancy as a catastrophic error.
     */
    if (active_flag)
        crashdump(0);
    else
        active_flag = TRUE;

    /*
     * Get the register table for this channel and the context structure
     * for this channel from the IDB pointer.
     */
    idb = (hwidbtype *)idbptr;
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    /*
     * Indicate activity on this interface by turning on the LED and
     * count the occurrence of this interrupt.
     */
    LSS_LED_ACTIVATE(ctx);
    ctx->rxint_cnt++;

    status = reg_table->risr;
    curbuf = (status & RISR_BUFB) ? 1 : 0;
    global_reoir = 0;

    if (status & RISR_ERROR) { /* Error indicated in status bits */
        if (status & RISR_CRC) {
            if (idb->poll_protocol_flags == PP_ROLE_UNDEFINED) {
               idb->counters.input_crc++;
            }
            (*ctx->hdx_rx_fsm)(idb, curbuf, HDX_RX_CRC_ERROR_EVENT);
        }
        else if (status & RISR_RXABT) {
	    /* The Cirrus chip treats garbage at the end of a frame
             * as an aborted frame.  The Hitatchi chip doesn't tell
             * us about short frames, and we don't want the Cirrus 
             * interfaces to appear different.  So we count these to 
             * be reported in show controller and decrement 
             * input_error because its incremented later.
             */
            /* if BSC wish to receive the abort frame */
            if (idb->poll_protocol_flags & PP_ACTIVE) {
                idb->counters.input_abort++;
	    }
            else if ((curbuf ? reg_table->brbcnt : reg_table->arbcnt)
                    < SMALLEST_FRAME_SIZE) { 
                ctx->aborted_short_cnt++;
                idb->counters.input_error--;
            }
            else {
                idb->counters.input_abort++;
	    }
            (*ctx->hdx_rx_fsm)(idb, curbuf, HDX_RX_ABORT_EVENT);
        }
        else if (status & RISR_OE) {
            idb->counters.input_overrun++;
            (*ctx->hdx_rx_fsm)(idb, curbuf, HDX_RX_OVERRUN_EVENT);
        }
	else if (status & RISR_RESIND) {
	    ctx->resind_cnt++;
	}
        else {  /* All other errors are treated as catastrophic and the
                   interface will be reset by the periodic routine */
	    if (status & RISR_BERR) {
	       ctx->buserr_cnt++;
	    }
            idb->counters.input_frame++;
	    reset_interface(ctx);
        }
        idb->counters.input_error++;
    }
    else if ((status & RISR_EOB) && (status & RISR_EOF)) {
        (*ctx->hdx_rx_fsm)(idb, curbuf, HDX_RX_EOF_EVENT);
    }
    else if (status & RISR_EOB) {
        (*ctx->hdx_rx_fsm)(idb, curbuf, HDX_RX_EOB_EVENT);
    }
    else {
        errmsg(&msgsym(BADRXISR, CIRRUS), idb->unit);
    }

    reg_table->reoir = global_reoir;
    /*
     * Clear potentially nested receive interrupt.
     */
    cd2430_clear_nested_rxint(ctx, reg_table, global_reoir);

    active_flag = FALSE;
    reset_interrupt_level(level);
}

/*
 * This is the Transmit Data ISR for this driver.
 * The transmit interrupt is usually entered to indicate that the
 * transmitter is empty or to indicate a completed transmission. The
 * former happens when the TxEmpty interrupt bit gets set to indicate
 * that there is some data to transmit.
 */
static void cd2430_sync_txint (void *idbptr)
{
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    hwidbtype *idb;
    int buf_index;
    leveltype level;
    char teoir_val = 0;
    static boolean active_flag = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * First, check for re-entrancy. Since this service routine should never
     * be re-entered, treat re-entrancy as a catastrophic error.
     */
    if (active_flag)
        crashdump(0);
    else
        active_flag = TRUE;

    /*
     * Get the register table for this channel and the context structure
     * for this channel from the IDB pointer.
     */
    idb = (hwidbtype *)idbptr;
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    /*
     * Always clear the TXMPTY interrupt source
     */
    reg_table->ier &= ~IER_TXMPTY;

    /*
     * Indicate activity on this interface by turning on the LED and 
     * count the occurence of this interrupt.
     */
    LSS_LED_ACTIVATE(ctx);
    ctx->txint_cnt++;

    /*
     * Take care of errors first.
     */
    if (reg_table->tisr & TISR_ERROR) {
        /*
         * If there was a bus error, indicate that the current buffer
         * be terminated by setting the terminate buffer bit while on
         * the way out of this interrupt.
         */
        if (reg_table->tisr & TISR_BERR) {
            teoir_val = TEOIR_TERM;
            /*
             * A bus error will be treated as a catastrophic problem.
             * Signal the periodic process to reset this interface the
             * next time it runs.
             */
	    reset_interface(ctx);
        }

        /*
         * Check for transmitter underruns.
         */
        if (reg_table->tisr & TISR_UE)
            idb->counters.output_underrun++;

        idb->counters.output_error++;
    }

    /*
     * Clean up the packet buffer whose transmission caused this interrupt.
     */
    buf_index = (reg_table->tisr & TISR_BUFB) ? 1 : 0;
    cd2430_release_tx_pak(idb, ctx, buf_index);

    /*
     * Transmit any packets that may be waiting in the output queue.
     */
    cd2430_transmit_packet(idb, ctx, reg_table);

    reg_table->teoir = teoir_val;
    /*
     * Clear potentially nested transmit interrupt.
     */
    cd2430_clear_nested_txint(ctx, reg_table, teoir_val);

    active_flag = FALSE;
    reset_interrupt_level(level);
}

/*
 * cd2430_sync_hdx_txint ()
 * The half-duplex transmit interrupt service routine for the CD2430
 */
static void cd2430_sync_hdx_txint (void *idbptr)
{
    hwidbtype *idb;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    leveltype level;
    ushort status;
    static boolean active_flag = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * First, check for re-entrancy. Since this service routine should never
     * be re-entered, treat re-entrancy as a catastrophic error.
     */
    if (active_flag)
        crashdump(0);
    else
        active_flag = TRUE;

    /*
     * Get the register table for this channel and the context structure
     * for this channel from the IDB pointer.
     */
    idb = (hwidbtype *)idbptr;
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    /*
     * Always clear the TXMPTY interrupt source
     */
    reg_table->ier &= ~IER_TXMPTY;

    /*
     * Indicate activity on this interface by turning on the LED and
     * count the occurrence of this interrupt.
     */
    LSS_LED_ACTIVATE(ctx);
    ctx->txint_cnt++;

    global_teoir = 0;
    status = reg_table->tisr;

    if (status & TISR_ERROR) {
        /*
         * If there was a bus error, indicate that the current buffer
         * be terminated by setting the terminate buffer bit while on
         * the way out of this interrupt.
         */
        if (status & TISR_BERR) {
            global_teoir = TEOIR_TERM;
            /*
             * A bus error will be treated as a catastrophic problem.
             * Signal the periodic process to reset this interface the
             * next time it runs.
             */
	    reset_interface(ctx);
        }

        /*
         * Check for transmitter underruns.
         */
        if (reg_table->tisr & TISR_UE)
            idb->counters.output_underrun++;

        idb->counters.output_error++;
    }
    else if (status & (TISR_EOF | TISR_EOB)) {
        (*ctx->hdx_tx_fsm)(idb, HDX_TX_EOF_EVENT);
    }
    else if (status & TISR_EOB) {
        idb->counters.output_error++;
    }
    else if (status & TISR_MPTY) {
        (*ctx->hdx_tx_fsm)(idb, HDX_TX_EMPTY_EVENT);
    }
    else {
        errmsg(&msgsym(BADTXISR, CIRRUS), idb->unit);
    }

    reg_table->teoir = global_teoir;
    /*
     * Clear potentially nested transmit interrupt.
     */
    cd2430_clear_nested_txint(ctx, reg_table, global_teoir);

    active_flag = FALSE;
    reset_interrupt_level(level);
}

/*
 * This is the ISR to handle changes in modem signals for a CD2430
 * port in the sync mode.
 */
static void cd2430_sync_modint (void *idbptr)
{
    hwidbtype *idb;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    ushort status;
    leveltype level;
    static boolean active_flag = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * First, check for re-entrancy. Since this service routine should never
     * be re-entered, treat re-entrancy as a catastrophic error.
     */
    if (active_flag)
        crashdump(0);
    else
        active_flag = TRUE;

    /*
     * Get the register table for this channel and the context structure
     * for this channel from the IDB pointer.
     */
    idb = (hwidbtype *)idbptr;
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    /*
     * Indicate activity on this interface by turning on the LED and
     * count the occurrence of this interrupt.
     */
    LSS_LED_ACTIVATE(ctx);
    ctx->modint_cnt++;

    status = reg_table->misr;

    /*
     * In the full duplex case, we do not react to changes in the state
     * CTS and RTS. However the signal used to indicate if the line
     * protocol on the serial interface is up or not is DTR in the case
     * of a DCE interface and either DCD or DSR in the case of a DTE
     * interface.
     */
    if (idb->dte_interface) {
        if DCD_DSR_CHANGE(status) /* Change in DCD or DSR */
            cd2430_sync_line_state(idb);
    }
    else {
        if DTR_CHANGE(status) /* Change in DTR */ 
            cd2430_sync_line_state(idb);
    } 

    monitor_interface_flap(ctx, reg_table);

    reg_table->meoir = 0;
    /*
     * Clear potentially nested modem interrupt.
     */
    cd2430_clear_nested_modint(ctx, reg_table, 0);

    active_flag = FALSE;
    reset_interrupt_level(level);
}

/*
 * cd2430_sync_hdx_modint()
 * This is the half-duplex modem signal change interrupt service routine.
 * Since timer events are also reported via the modem interrupts, this
 * ISR also handles timer events. Line state transitions are also
 * addressed in this routine.
 */
static void cd2430_sync_hdx_modint (void *idbptr)
{
    hwidbtype *idb;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    ushort status;
    leveltype level;
    static boolean active_flag = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * First, check for re-entrancy. Since this service routine should never
     * be re-entered, treat re-entrancy as a catastrophic error.
     */
    if (active_flag)
        crashdump(0);
    else
        active_flag = TRUE;

    /*
     * Get the register table for this channel and the context structure
     * for this channel from the IDB pointer.
     */
    idb = (hwidbtype *)idbptr;
    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    status = reg_table->misr;
    global_meoir = 0;

    /*
     * Indicate activity on this interface by turning on the LED and
     * count the occurrence of this interrupt.
     */
    LSS_LED_ACTIVATE(ctx);
    ctx->modint_cnt++;

    if (idb->dte_interface) {  /* DTE interface */
        /*
         * First take care of potential line state change events
         */
        if DCD_DSR_CHANGE(status) /* Change in DCD or DSR */
            cd2430_sync_line_state(idb);
        /*
         * Now take care of the half-duplex related events
         */
        if RTS_CHANGE(status) /* Change in RTS */
            (*ctx->hdx_tx_fsm)(idb, HDX_CTS_CHANGE_EVENT);
        if TIMER1_TIMEOUT(status) /* Timer 1 timeout */
            (*ctx->hdx_tx_fsm)(idb, HDX_TIMER1_EVENT);
        if TIMER2_TIMEOUT(status) /* Timer 2 timeout */
            ctx->false_event_counters[HDX_TIMER2_EVENT]++;
    }
    else { /* DCE interface */
        /*
         * First take care of potential line state change events
         */
        if DTR_CHANGE(status) /* Change in DTR */
            cd2430_sync_line_state(idb);
        /*
         * Now take care of the half-duplex related events
         */
        if RTS_CHANGE(status) /* Change in RTS */
            (*ctx->hdx_rx_fsm)(idb, 0, HDX_RTS_CHANGE_EVENT);
        if TIMER1_TIMEOUT(status) /* Timer 1 timeout */
            (*ctx->hdx_rx_fsm)(idb, 0, HDX_TIMER1_EVENT);
        if TIMER2_TIMEOUT(status) /* Timer 2 timeout */
            (*ctx->hdx_tx_fsm)(idb, HDX_TIMER2_EVENT);
    }

    monitor_interface_flap(ctx, reg_table);

    reg_table->meoir = global_meoir;
    /*
     * Clear potentially nested modem interrupt.
     */
    cd2430_clear_nested_modint(ctx, reg_table, global_meoir);

    active_flag = FALSE;
    reset_interrupt_level(level);
}

/*
 * This is the ISR to handle Receive Good Data interrupts. This should
 * never be entered since this is a DMA driver.
 */
static void cd2430_sync_rxgoodint (void *idbptr)
{
    hwidbtype *idb;
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    leveltype level;
    ushort status;
    uchar rx_char;
    static boolean active_flag = FALSE;

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * First, check for re-entrancy. Since this service routine should never
     * be re-entered, treat re-entrancy as a catastrophic error.
     */
    if (active_flag)
        crashdump(0);
    else
        active_flag = TRUE;

    /*
     * Get the register table for this channel and the context structure
     * for this channel from the IDB pointer.
     */
    idb = (hwidbtype *)idbptr;
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    /*
     * Indicate activity on this interface by turning on the LED and
     * count the occurrence of this interrupt.
     */
    LSS_LED_ACTIVATE(ctx);
    ctx->rxint_cnt++;

    if (idb->poll_protocol_flags & PP_CHARINT) {
       status = reg_table->risr;
       rx_char = reg_table->rdr;
       (*idb->rx_char_int)(idb, rx_char, status & RISR_OE);

       reg_table->reoir = 0;
       cd2430_clear_nested_rxint(ctx, reg_table, 0);
    }
    else {
       idb->counters.input_error++;

        /*
         * Since we didn't transfer data, we must specify this bit in the
         * interrupt completion.
         */
       reg_table->reoir = REOIR_NOTRANSF;
        /*
         * Clear potentially nested receive interrupt.
         */
       cd2430_clear_nested_rxint(ctx, reg_table, REOIR_NOTRANSF);
    }

    active_flag = FALSE;
    reset_interrupt_level(level);
}


/*************************************************************************
 *            IDB Vector Functions and Registry Functions for the 
 *                         CD2430 Sync Driver
 *
 * This section of the synchronous driver for the CD2430 controller
 * includes routines that are
 *     o Registered for callback from other parts of the system code, or
 *     o Pointed to by function vectors in the IDB, or
 *     o Callbacks for parser commands related to this driver
 **************************************************************************/

/*
 * Start Transmitter for a Sync port.
 * Note that we signal the appropriate channel of the port that there
 * is data to be sent by turning on the TxEmpty interrupt source if the
 * transmitter is currently idle. If the transmitter is currently busy
 * sending data, we do NOT want to generate a TxEmpty interrupt since
 * the interrupt generated at the end of the current transmission will
 * take care of the packet that just got enqueued. We can figure out if
 * the transmitter is currently busy by checking the outpak[] array
 * that hangs off of the context structure.
 *
 * If we are called from the context of an interrupt generated by
 * this controller, then we need to use the fastsend helper since we
 * are not permitted to change the channel access register.
 *
 */
void cd2430_sync_txstart (hwidbtype *idb)
{
    leveltype level;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    boolean hdx_mode;

    level = raise_interrupt_level(NETS_DISABLE);
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    if (!(ctx->outpak[0] || ctx->outpak[1])) {
        if (reg_table->stk) {
	    helper_mask |= ctx->helper_mask;
	    process_wakeup(helper_pid);
	} else {
            hdx_mode = idb->hdx_flags & HDX_FLAG_ON;
	    setchan_sync(reg_table, idb);
	    if (hdx_mode) {
	        reg_table->ier |= IER_TXMPTY;
	    } else {
	        /*
		 * Transmit any packets that may be waiting in the output
		 * queue.
		 */
	        cd2430_transmit_packet(idb, ctx, reg_table);
	    }
	}
    }
    reset_interrupt_level(level);
}


/*
 * cd2430_set_transmit_delay()
 * This is the registry function for the transmitter delay command.
 * This will program the minimum number of flags to be inserted between
 * consecutive transmissions. Some slow receivers will need that this be
 * appropriately tuned. The minimum number of flags for the CD2430 is
 * 1 with shared opening/closing flags permitted. The maximum number
 * of minimum flags (!) allowed is 15.
 */
void cd2430_set_transmit_delay (hwidbtype *idb, int value)
{
    if (value < 0)
        value = 0;
    if (value > COR1_MAX_DELAY_FLAGS)
        value = COR1_MAX_DELAY_FLAGS;

    idb->xmitdelay = value;

    cd2430_sync_reset(idb);
}

/*
 * get_clock()
 * This is the get_clock() routine for the actual Cobra. We use a
 * pre-computed table to calculate the clock parameters for the
 * requested baud-rate.
 */
static const cd2430_clocktype_t *get_clock (int baud)
{
    int index = 0;
 
    while (cd2430_clock_table[index].rate != 0) {
        if (baud == cd2430_clock_table[index].rate)
            return (&cd2430_clock_table[index]);
        index++;
    }
 
    return (NULL);
}

/*
 * The clockrate command for this controller.
 */
void cd2430_clockrate_command (parseinfo *csb)
{
    hwidbtype *idb = csb->interface->hwptr;
    serialsb *ssb;
    int index = 0;
    uint clock = 0;

    if (csb->sense) {
      clock = GETOBJ(int,1);
    }

    if ((csb->which == CFG_INT_CLOCKRATE) && (clock == 0)) {
        printf("\n  \t%s\n", "Speed (bits per second)");

	/* ignore "clock = 0" */

	while (cd2430_clock_table[index].rate != 0) {
	    printf("  %d\n", cd2430_clock_table[index].rate);
	    index++;
	}

	return;
    }
 
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Write it out if NV generation is being done.
     */
    if (csb->nvgen) {
        clock = ssb->serial_clockindex;
        if ((!idb->dte_interface) && clock)  /* DCE interface */
            nv_write(clock, "%s %d", csb->nv_command, clock);
        return;
    }

    if (idb->dte_interface) {
        printf("%%Error: This command applies only to DCE interfaces");
        return;
    }
    else {  
        if (csb->sense && (clock != 0)) {
            /*
             * Sanity check requested value
             */
            if (!get_clock(clock)) {
                printf("\n%%Error: Unsupported clock rate for this interface");
                return;
            }
        }
    }

    /*
     * Save the desired clock rate
     */
    ssb->serial_clockindex = clock;
    cd2430_sync_mode_init(idb);
}

/*
 * Set maximum datagram size as requested.
 */
static boolean cd2430_set_maxdgram (hwidbtype *idb, int buffersize, 
                                                                int maxdgram)
{
    if (idb->type != IDBTYPE_CD2430) {
        printf(set_maxdgram_failed, maxdgram, idb->hw_namestring);
        return (FALSE);
    }

    if_maxdgram_adjust(idb, maxdgram);
    cd2430_sync_mode_init(idb);
    return (TRUE);
}

/*
 * cd2430_set_rxoffset
 * Set receive offset for the CD2430
 */
void cd2430_set_rxoffset (hwidbtype *idb)
{
    cd2430_sync_mode_init(idb);
}

/*
 * The shutdown routine for a CD2430 sync port.
 */
static void cd2430_sync_shutdown (hwidbtype *idb, int newstate)
{
    net_cstate(idb, newstate);
    cd2430_sync_reset(idb);
}

/*
 * cd2430_periodic()
 * Monitor the Cirrus serial interface for possible interface flaps.
 * In the event of a flap, we disable modem interrupts. In this function
 * that gets called periodically, we check if the interface has been
 * throttled because it was flapping and turn it loose again after a
 * certain probation period.
 */
static void cd2430_periodic (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    paktype *pak;
    ulong duration;
    leveltype level;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    if (ctx) {
        /*
         * Check to see if this interface was flapping in the past
         * and has been shut off.
         */
        if (ctx->flap_enabled) {
            if (ctx->flap_backoff == 0)
                errmsg(&msgsym(LINEFLAP, CIRRUS), idb->unit);

            if (++ctx->flap_backoff > SERIAL_FLAP_BACKOFF) {
                /*
                 * We have now paid the price for flapping
                 */
                ctx->flap_count = 0;
                TIMER_STOP(ctx->flap_time);
                ctx->flap_backoff = 0;
                ctx->flap_enabled = FALSE;
                enable_modem_interrupts(reg_table);
            }
        }

        /*
         * Ensure that the Cirrus controller is running smoothly
         */
        if ((idb->state == IDBS_UP) && !reg_invoke_dialer_spoofing_int(idb)) {
            if (ctx->appl_periodic) {
                /*
                 * Call the applique specific periodic routine
                 */
                level = raise_interrupt_level(NETS_DISABLE);
                (*ctx->appl_periodic)(idb);
                reset_interrupt_level(level);
            }
        }
      
        /*
         * Check for hung outputs by looking at the transmit timer
         * hanging off of the IDB. If a hung transfer is found, reset
         * the interface.
         */
        if (ctx->outpak[0] || ctx->outpak[1]) {
            if (TIMER_RUNNING_AND_AWAKE(idb->xmittimer)) { 
                TIMER_STOP(idb->xmittimer);
		GET_TIMESTAMP(idb->lastreset);
		ctx->txhang_cnt++;
		if (serial_debug)
		    buginf("\nSerial%d, transmitter hang",idb->unit);
                (*(idb->reset))(idb);
            }
        }

        /*
         * Check for possible cable insertion/removal. If such a change is
         * detected, re-initialize this interface so that activities that
         * are necessary to handle a cable insertion or removal event
         * do happen.
         */
        if (cable_present(ctx) != ctx->cable_present)
            cd2430_sync_mode_init(idb);

        /*
         * If signalled from interrupt level that interface is to be reset,
         * reset it now.
         */
        if (ctx->reset_interface)
            (*(idb->reset))(idb);

        /*
         * Calculate the rate of incoming transmit, receive and modem
         * interrupts averaged over the time between two successive
         * invocations of this periodic routine.
         */
        duration = ELAPSED_TIME(ctx->duration);
        if (duration) {
            ctx->txint_rate = (ctx->txint_cnt * 1000)/((ulong)duration);
            ctx->rxint_rate = (ctx->rxint_cnt * 1000)/((ulong)duration);
            ctx->modint_rate = (ctx->modint_cnt * 1000)/((ulong)duration);
        }

        /*
         * Now, reset the timestamp and the counts.
         */
        GET_TIMESTAMP(ctx->duration);
        ctx->txint_cnt = 0;
        ctx->rxint_cnt = 0;
        ctx->modint_cnt = 0;

        /*
         * Check if a number of test packets need to be sent in half-duplex
         * mode. If so, send one now.
         */
        if (ctx->test_tx_pak_cnt) {
            pak = les_driver_getbuf_inline(idb, TRUE);
            sprintf(pak->datagramstart, 
                    "TTest Frame #%d: ABCDEFGHIJKLMNOPQRSTUVWXYZ",
                    ctx->test_tx_pak_cnt);
            pak->datagramsize = strlen(pak->datagramstart);
            level = raise_interrupt_level(NETS_DISABLE);
            holdq_enqueue_fast(idb, pak);
            (*idb->soutput)(idb);
            reset_interrupt_level(level);
            ctx->test_tx_pak_cnt--;
        }
    }
}

/*
 * cd2430_show_modem_signals()
 *      This function will be called to display the state of the modem signals
 * as part of the output from the "show interface" command.
 */
static void cd2430_show_serial_state (hwidbtype* idb, char* leader)
{
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    char dcd,dsr,dtr,rts,cts;
    leveltype level;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    level = raise_interrupt_level(NETS_DISABLE);

    setchan_sync(reg_table, idb);

    if (idb->dte_interface) {
        dcd = DTE_DCD_is_asserted(reg_table);
	dsr = DTE_DSR_is_asserted(reg_table);
	dtr = DTE_DTR_is_asserted(reg_table);
	rts = DTE_RTS_is_asserted(ctx);
	cts = DTE_CTS_is_asserted(reg_table);
    } else {
        dcd = DCE_DCD_is_asserted(ctx);
	dsr = DCE_DSR_is_asserted(reg_table);
	dtr = DCE_DTR_is_asserted(reg_table);
	rts = DCE_RTS_is_asserted(reg_table);
	cts = DCE_CTS_is_asserted(ctx);
    }
    reset_interrupt_level(level);

    printf("\n     DCD=%s  DSR=%s  DTR=%s  RTS=%s  CTS=%s",
	   (dcd ? "up" : "down"),
	   (dsr ? "up" : "down"),
	   (dtr ? "up" : "down"),
	   (rts ? "up" : "down"),
	   (cts ? "up" : "down"));
}



/*
 * cd2430_sync_show_controller()
 * The function to dump the CD2430 registers along with other useful
 * information for CD2430 serial interface is sync mode.
 */
boolean cd2430_sync_show_controller (hwidbtype *idb, parseinfo *csb)
{
    int chip, chan, mode;
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    per_channel_registers pcr;
    leveltype level;

    /*
     * Make sure it's us
     */

    if CD2430_VALID_PORT(idb) {
        return(FALSE);
    }

    ctx = INSTANCE;
    chip = ctx->ctrl;
    chan = ctx->chan;
 
    /*
     * Point to chip for this bank of channels. Also, set the channel access
     * register to point to the concerned channel.
     */

    reg_table = ctx->reg_table;
 
    level = raise_interrupt_level(NETS_DISABLE);
    setchan_sync(reg_table, idb);
    cd2430_copy_per_channel_registers(reg_table,&pcr);
    reset_interrupt_level(level);

    /*
     * Print out the preamble info for this channel
     */

    cd2430_show_channel_info(idb);

    mode = pcr.cmr & 0x7;
    switch (mode) {
    case CMR_MODE_HDLC:
        printf("\n%s", sync_str);
        break;
    case CMR_MODE_BISYNC:
        printf("\n%s(Bisync)", sync_str);
        break;
    default:
        printf("\nChannel mode UNKNOWN");
        break;
    }

    printf("\nidb 0x%x, buffer size %d,", idb, idb->max_buffer_size);

    cd2430_show_cable_state(idb);

    /* Display the global registers */
    cd2430_show_registers_global(reg_table);
 
    /* Display the per-channel regs */
    cd2430_show_registers_perchannel(&pcr);

    /* Display driver context information */
    cd2430_sync_show_driver_context(ctx);

    cd2430_sync_show_errors(idb);
    cd2430_sync_show_hdx_log(idb);
    printf("\n");
 
    return(FALSE);
}


/*************************************************************************
 *        General Utility Functions for the CD2430 Sync Driver
 *
 * General utility routines used in this driver are included in this
 * section of the driver. These routines are used in the initialization
 * path of the driver, handle transitions into the synchronous operation
 * mode etc.
 *************************************************************************/

/*
 * cd2430_set_channel_option_regs()
 * This function sets the channel option registers of the CD2430
 * controller depending on the mode that the controller must be in.
 */
static void cd2430_set_channel_option_regs (hwidbtype *idb,
                                            int mode)
{
    cd2430_quadart * reg_table;
    cd2430_context_t *ctx;
    serialsb *ssb;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * First disable the receiver and transmitter, and then clear the 
     * channel.
     */
    sendccr(reg_table, CCR_DISTX | CCR_DISRX);
    sendccr(reg_table, CCR_CLRCH);

    switch (mode) {
    case MODE_HDLC:
        reg_table->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_HDLC;
        reg_table->cor1 = idb->xmitdelay;
        reg_table->cor2 = 0;
        reg_table->cor3 = (ssb->nrzi_enable ? COR3_NRZI : COR3_NRZ) | COR3_2PAD;
        if (idb->serial_flags & SERIAL_FLAGS_DO_ALT_FLAG)
            reg_table->cor3 |= COR3_IDLE_MARK; 
        else
            reg_table->cor3 &= ~COR3_IDLE_MARK;

 	if (idb->dte_interface) {
 	    reg_table->cor4 = (COR4_DSRZD | COR4_CDZD | COR4_CTSZD) | 
 	                       COR4_RXTHRESH(9600);
 	    reg_table->cor5 = (COR5_DSROD | COR5_CDOD | COR5_CTSOD);
 	} else {
 	    reg_table->cor4 = (COR4_CDZD | COR4_CTSZD) | COR4_RXTHRESH(9600);
 	    reg_table->cor5 = (COR5_CDOD | COR5_CTSOD);
 	}

        reg_table->cor6 = 0;
        reg_table->cor7 = 0;
        reg_table->cpsr = CPSR_V41;
        break;
    case MODE_BISYNC:

        /*
         * bisync mode can be either done via frame interrupts
         * or character by character interrupts.
         * The default is frames.
         */
        if ( idb->poll_protocol_flags & PP_CHARINT ) {
            /*
             * the fifo threshhold has been deliberately set to zero.
             * higher values would lead to bad short frames
             */
            reg_table->cmr = CMR_TXDMA | CMR_MODE_AUTO;
            reg_table->cor4 = (COR4_DSRZD | COR4_CDZD | COR4_CTSZD);
        }
        else {
            reg_table->cmr = CMR_RXDMA | CMR_TXDMA | CMR_MODE_BISYNC;
            reg_table->cor4 = (COR4_DSRZD | COR4_CDZD | COR4_CTSZD) 
                                          | COR4_RXTHRESH(9600);
        }

        /*
         * Default is EBCDIC mode.
         * Put in ASCII only if explicitly told.
         */
        if ( idb->poll_protocol_flags & PP_ASCII ) {
            reg_table->cor1 = COR1_PARITY_ODD | COR1_PARITY_NORM | COR1_7BIT;
            reg_table->cor2 = COR2_LRC | COR2_APPEND_BCC;
        }
        else {
            reg_table->cor1 = COR1_PARITY_IGN | COR1_8BIT;
            reg_table->cor2 = COR2_APPEND_BCC | COR2_EBCDIC | COR2_CRC_NORMAL;
        }

        reg_table->cor2 |= COR2_NO_STRIP ;
        reg_table->cor3 = COR3_FCS_ZERO  | COR3_DISABLE_FCS
                        | COR3_IDLE_MARK | COR3_1PAD ;
        reg_table->cor4 = (COR4_DSRZD | COR4_CDZD | COR4_CTSZD) |
                                                           COR4_RXTHRESH(9600);
        reg_table->cor5 = (COR5_DSROD | COR5_CDOD | COR5_CTSOD);
        reg_table->cpsr = CPSR_CRC16;
        break;
    default:
        errmsg(&msgsym(BADMODE, CIRRUS), idb->unit);
        break;
    }

    /*
     * Set-up the per-channel interrupt vector register
     */

    CD2430_SET_LIVR_SYNC(idb);

    /*
     * Initialize CD2430 channel for configured option register values
     * to take effect. Cirrus informed us that issuing a initialize channel
     * command by itself can cause problems under some conditions. So,
     * issue the clear channel command along with the initialize channel
     * command.
     */
    sendccr(reg_table, CCR_CLRCH | CCR_INITCH);
    /*
     * The following write to the DMA buffer adjust register is needed to 
     * work around a Cirrus firmware bug. After the INITCH command, the 
     * firmware is supposed to zero out the DMA Buffer Adjust Register. 
     * However, the current version of the microcode (ver83) does not do 
     * this. We will do this zeroing of the internal Cirrus register in the 
     * driver until the firmware is fixed by Cirrus. Not zeroing the register 
     * causes the first frame out of the interface after a reset to be 
     * corrupted with some erroneous bytes prefixed to the frame header.
     */
    reg_table->dbadj = 0;
}

/*
 * cd2430_set_clock_option_regs()
 * This function will set the clock option registers for the CD2430
 * controller appropriately depending on whether the interface is in
 * DTE mode or DCE mode and the bitrate of the interface.
 */
void cd2430_set_clock_option_regs (hwidbtype *idb, int bitrate)
{
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    const cd2430_clocktype_t *clockinfo;
    serialsb *ssb;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Set the DTROpt bit in the MSVR-DTR register to indicate that the
     * transmit clock is to be output on the TxCout/DTR pin.
     */
    reg_table->msvr_dtr |= MSVR_DTROPT;
 
    /*
     * If this is a DTE interface, the DCE sources the clocks. We will program
     * the receive and transmit clock option registers to use a times-one
     * external clock.
     */
    if (idb->dte_interface) {
        reg_table->rcor = RCOR_EXT | (ssb->nrzi_enable ? RCOR_NRZI : RCOR_NRZ);
        reg_table->rbpr = 1;
        reg_table->tcor = (TCOR_EXT | TCOR_EXT1X);
        reg_table->tbpr = 1;
    }
    else {  /* DCE interface */
        if (bitrate) {
            clockinfo = get_clock(bitrate);
            if (!clockinfo)
                return;

            reg_table->rcor = RCOR_EXT | 
                                 (ssb->nrzi_enable ? RCOR_NRZI : RCOR_NRZ);
            reg_table->tcor = TCOR_EXT;
            /*
             * If DCE terminal timing is enabled, then we will use the clock
             * on RS-232 TxCE (pin 24) to receive data. Hence the receive clock
             * option register will be programmed for times-one external
             * clock. In normal DCE operation, the transmit and receive 
             * clocks input to the TxCin and RxCin input pins of the 
             * CD2430 come from the output of the external pre-divider.
             * The pre-divider and the bit period registers are programmed
             * appropriately to generate the desired bit rate.
             */
            if (ssb->dce_terminal_timing_enable) {
                reg_table->rbpr = 1;
            }
            else {
                /*
                 * Later versions of the Cobras can route TxCout back to
                 * RxCin.  This causes the receive clock to always be
                 * a 1X clock, which effectively disables the DPLL,
                 * which due to a hardware bug in the CD2402 can't be
                 * completely disabled.  Earlier versions (the else below)
                 * used the same input to RxCin as is input to TxCin.
                 * All of this when DCE terminal timing is NOT enabled.
                 * To distinguish old from new, try setting DEVICE_LSS_TXCRXC.
                 * If successful, it's a late model version.
                 */
                *ctx->serial_dev_reg_hi |= DEVICE_LSS_TXCRXC;
                if (*ctx->serial_dev_reg_hi & DEVICE_LSS_TXCRXC) {
                    reg_table->rbpr = 1;
                }
                else {
                    reg_table->rbpr = clockinfo->bpr;
                    /*
                     * Since we are getting the value for the bit period
                     * register from the clockinfo structure, the value
                     * programmed into it is certainly not 1. If the receive
                     * clock is not a times-one clock, then the DPLL must
                     * be turned on.  The reason it needs to be turned
                     * on is that the divisor counters for transmit and
                     * receive are NOT started at the same time and
                     * therefore the transmit and receive clocks are
                     * out of phase.  The DPLL brings them into phase
                     * after a few transitions on the RXD pin.
                     *
                     * The * problem with leaving it enabled is that some
                     * configurations (ie. DCE, NRZI, half-duplex) can
                     * have "valid" transitions on RXD that are out of
                     * sync with RXC.  Example, a modem sharing device
                     * (de)selecting a polled terminal in response to
                     * a transition on RTS.  In this case, the MSD will
                     * sometimes drive TD to/from a different state than
                     * the terminal left it at (typically driving it
                     * low, while the terminal being (de)selected is
                     * outputing a high, marking in NRZI).
                     */
                    reg_table->rcor |= RCOR_DPLL;
                }
	      }
              reg_table->tbpr = clockinfo->bpr;

              CD2430_SET_CLOCK_PRE_DIVIDER(ctx, clockinfo->pre_divider)
        }
        else {
            /*
             * No clock is to be sourced on TxC anc RxC. So, the DTROpt bit
             * will be set to output DTR. This is done just to prevent the 
             * output of clocks, not to control DTR from this pin. The bit
             * in the low-speed serial device register (low-byte) continues
             * to be used for that purpose.
             */
            reg_table->msvr_dtr &= ~MSVR_DTROPT;
        }
    }
}

/*
 * cd2430_set_timer_params()
 * This routine will compute the timer pre-scaler value and the timer
 * register value to be used for a particular (millisec) timeout.
 * The max_ticks parameter passed into this function will depend on
 * whether the timer being used is Timer1 or Timer2. The former is a
 * 16 bit timer and the latter is a 8 bit timer.
 */
static void cd2430_set_timer_params (ulong msecval, cd2430_timer_t *timer, 
                                     ushort max_ticks) 
{
    ushort pre_scaler;
    ushort ticks;
    ulong  temp;

    if (timer == NULL)
        return;

    timer->msecval = msecval;
    temp = ((cd2430clock)/(2048 * 1000)) * (msecval);

    for (pre_scaler = MIN_PRE_SCALER; pre_scaler <= MAX_PRE_SCALER; 
                                                              pre_scaler++) {
        ticks = (temp / pre_scaler);
        if (ticks <= max_ticks)
            break;
    }
    
    if (pre_scaler > MAX_PRE_SCALER) {
        printf("%%Error: Timer value of %d msec not possible", msecval);
    }
    else {
        timer->tpr = pre_scaler;
        timer->timer_val = ticks;
    }
}

/*
 * cd2430_buffer_pools_init()
 * This function will initialize the buffer pools corresponding to a
 * particular interface.
 */
static void cd2430_buffer_pools_init (hwidbtype *idb)
{
    boolean maxdgram_change = FALSE;

    /*
     * Has the MTU changed? If so, we need to get a new
     * ring of receive buffers.
     */
    if (idb->pool && (idb->max_buffer_size != idb->pool->size))
       maxdgram_change = TRUE;

    /*
     * If a private buffer pool for this interface exists AND there
     * has been an MTU change, discard the pool with the old-sized
     * buffers.
     */

    if (idb->pool && maxdgram_change) {
        idb->pool = pool_resize(idb->pool, idb->max_buffer_size, 0,
                                PRIV_NUM_BUF(4), PRIV_NUM_BUF(4));
        if (!idb->pool)
            crashdump(0);
    }

    /*
     * If the private buffer pool has not been allocated yet,allocate it now.
     */

    if (!idb->pool) {
        if (idb->pool_group == POOL_GROUP_INVALID)
            idb->pool_group = pool_create_group();

        if (!idb->hw_namestring)
            idb_init_names(idb->firstsw, FALSE);

        idb->pool = pak_pool_create(idb->hw_namestring, idb->pool_group,
                                    idb->max_buffer_size,
				    POOL_SANITY | POOL_CACHE,
                                    NULL);
        if (!idb->pool) {
            errmsg(&msgsym(NOMEMORY, CIRRUS), idb->unit, "private buffer pool");
            crashdump(0);
        }
        
        /*
         * Populate the pool with buffers
         */
        pool_adjust(idb->pool, 0, PRIV_NUM_BUF(4), PRIV_NUM_BUF(4), TRUE);
    }

    /*
     * If required, set the fallback pool
     */
    if (!idb->pool->fallback)
        idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
    
    /*
     * If required, create a buffer cache
     */
    if (!(idb->pool->flags & POOL_CACHE)) {
        pak_pool_create_cache(idb->pool, PRIV_NUM_CACHE(4));
        pool_adjust_cache(idb->pool, PRIV_NUM_CACHE(4));
    }
}

/*
 * cd2430_sync_setup_isr_vectors ()
 *      This function will set up the vectors in the instance structure
 * for this channel to the sync interrupt service routines.
 */
static void cd2430_sync_setup_isr_vectors (hwidbtype *idb)
{
    cd2430_channel_inst_t **chan_inst;
    cd2430_context_t *ctx;
    boolean hdx_mode;

    ctx = INSTANCE;

    chan_inst = CD2430_GET_CHAN_INST(idb);

    /*
     * If this is the first time that we are setting up the channel instance
     * malloc the channel instance structure.
     */
    if (*chan_inst == NULL) {
        *chan_inst = malloc(sizeof(cd2430_channel_inst_t));
        if (*chan_inst == NULL) {
            errmsg(&msgsym(NOMEMORY, CIRRUS), idb->unit, "ISR vector set-up");
            crashdump(0);
        }
    }

    /*
     * Now set up the channel instance structure for synchronous operation.
     */
    (*chan_inst)->aid = idb;

    hdx_mode = idb->hdx_flags & HDX_FLAG_ON;

    /*
     * Now set up the ISR vectors depending on whether the interface is
     * in half-duplex mode or full-duplex mode. Note that for the rxgood
     * interrupt the service routine is the same in both full and
     * half-duplex modes. This is because this interrupt should not happen
     * in either case.
     */
    if (hdx_mode) {  /* Half-duplex case */
        (*chan_inst)->txint_ptr  = cd2430_sync_hdx_txint;
        (*chan_inst)->rxint_ptr  = cd2430_sync_hdx_rxint;
        (*chan_inst)->modint_ptr = cd2430_sync_hdx_modint;
        (*chan_inst)->rxgoodint_ptr = cd2430_sync_rxgoodint;
    }
    else {  /* Full duplex case */
        (*chan_inst)->rxint_ptr  = cd2430_sync_rxint;
        (*chan_inst)->txint_ptr  = cd2430_sync_txint;
        (*chan_inst)->modint_ptr = cd2430_sync_modint;
        (*chan_inst)->rxgoodint_ptr = cd2430_sync_rxgoodint;
    }
}

/*
 * This function will reset all buffers associated with a sync port.
 */
static void cd2430_reset_rx_buffers (hwidbtype *idb)
{
    cd2430_context_t *ctx;

    ctx = INSTANCE;

    /*
     * Free up any armed receive packets.
     */
    if (ctx->inpak[0]) {
        datagram_done(ctx->inpak[0]);
        ctx->inpak[0] = NULL;
    }
    if (ctx->inpak[1]) {
        datagram_done(ctx->inpak[1]);
        ctx->inpak[1] = NULL;
    }
}

static void cd2430_reset_tx_buffers (hwidbtype *idb)
{
    cd2430_context_t *ctx;

    ctx = INSTANCE;

    /*
     * Free up outbound packets
     */
    if (ctx->outpak[0]) {
        datagram_done(ctx->outpak[0]);
        ctx->outpak[0] = NULL;
    }
    if (ctx->outpak[1]) {
        datagram_done(ctx->outpak[1]);
        ctx->outpak[1] = NULL;
    }
}

/*
 * cd2430_show_hdx_log()
 *      This function displays the audit trail left by the half-duplex
 * code via the debug information logging macros.
 */
static void cd2430_sync_show_hdx_log (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    char *indent;
    hdx_log_element_t *hdx_log;
    int i, index;
 
    ctx = INSTANCE;

    if (!ctx->hdx_log) /* Logging is not turned on */
        return;

    /*
     * While the log is being printed, turn off logging. This is done by
     * simply setting the log pointer in the context structure to NULL.
     */
    hdx_log = ctx->hdx_log;
    ctx->hdx_log = NULL;

    indent = idb->dte_interface ? dte_indent : dce_indent;
    printf("\nTime\tState Machine\t\t\tDTE\t\t\t\tDCE\n");
    printf("---------------------------------------------------------------");
    printf("-------------\n");
    index = ctx->hdx_log_index;
 
    for (i = 0; i < HDX_LOG_LENGTH; i++) {
        if (hdx_log[index].fsm) {
            printf("%lTa ", hdx_log[index].timestamp);
            printf("%s\n", hdx_log[index].fsm);
        }
        if (hdx_log[index].state) {
            printf("%lTa ", hdx_log[index].timestamp);
            printf("%s%s\n", indent, hdx_log[index].state);
        }
        if (hdx_log[index].event) {
            printf("%lTa ", hdx_log[index].timestamp);
            printf("%s%s\n", indent, hdx_log[index].event);
        }
        if (hdx_log[index].figure) {
            printf("%lTa ", hdx_log[index].timestamp);
            printf("%s\n", hdx_log[index].figure);
        }
 
        index++;
        if (index >= HDX_LOG_LENGTH)
            index = 0;
    }
    /*
     * Now that we are done printing, turn the logging back on again.
     */
    ctx->hdx_log = hdx_log;
}

/*
 * cd2430_sync_show_errors()
 *      This function is called from within the show_controller() function
 * to display errors encountered and also some information that has been
 * gathered for analysis.
 */
static void cd2430_sync_show_errors (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    boolean hdx_mode;

    ctx = INSTANCE;

    printf("\nError counters");
    printf(
"\n  CTS deassertion failures\t%d", ctx->cts_deassertion_fail);
    printf(
"\n  Nested interrupt errors transmit %d, receive %d, modem %d",
    ctx->nested_tx_ints, ctx->nested_rx_ints, ctx->nested_modem_ints);

    /*
     * If in half-duplex mode, display the spurious event counters.
     */
    hdx_mode = idb->hdx_flags & HDX_FLAG_ON;

    if (hdx_mode) {
        printf("\nSpurious half-duplex event counters");
        printf(
"\n  Timer1 events\t\t\t%d", 
        ctx->false_event_counters[HDX_TIMER1_EVENT]);
        printf(
"\n  Timer2 events\t\t\t%d", 
        ctx->false_event_counters[HDX_TIMER2_EVENT]);
        printf(
"\n  RTS change events\t\t%d", 
        ctx->false_event_counters[HDX_RTS_CHANGE_EVENT]);
        printf(
"\n  CTS change events\t\t%d", 
        ctx->false_event_counters[HDX_CTS_CHANGE_EVENT]);
        printf(
"\n  Transmitter empty events\t%d",
        ctx->false_event_counters[HDX_TX_EMPTY_EVENT]);
        printf(
"\n  Transmitter EOB events\t%d",
        ctx->false_event_counters[HDX_TX_EOB_EVENT]);
        printf(
"\n  Transmitter EOF events\t%d",
        ctx->false_event_counters[HDX_TX_EOF_EVENT]);
        printf(
"\n  Receiver EOB events\t\t%d",
        ctx->false_event_counters[HDX_RX_EOB_EVENT]);
        printf(
"\n  Receiver EOF events\t\t%d",
        ctx->false_event_counters[HDX_RX_EOF_EVENT]);
    }

    printf("\n");
}


/*
 * cd2430_enter_transparent_mode()
 * Assert the modem signals and take care of associated logic.
 */
static void cd2430_enter_transparent_mode (hwidbtype *idb)
{
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    serialsb *ssb;
    boolean hdx_mode;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    hdx_mode = idb->hdx_flags & HDX_FLAG_ON;
    
    
    if (ctx->cable_present) {
        /*
         * Assert DSR or DTR as appropriate. This must be done before a check
         * for DCD is done if we are a DTE so that DTR->DCD loopbacks work.
         */
        if (idb->dte_interface) {
            assert_DTR(idb, reg_table);
            HDX_LOG_FIG(ctx, dtr_assert_fig);
        }
        else {
            assert_DSR(idb, reg_table);
            HDX_LOG_FIG(ctx, dsr_assert_fig);
        }

        /*
         * If a DTE in full-duplex mode assert RTS, else deassert RTS.
         * If DCE, assert DCD. Further if in half-duplex mode, deassert CTS
         * else, assert CTS.
         */
        if (idb->dte_interface) {  
            if (hdx_mode) {
                deassert_RTS(idb, ctx);
                HDX_LOG_FIG(ctx, rts_deassert_fig);
            }
            else {
                assert_RTS(idb, ctx);
                HDX_LOG_FIG(ctx, rts_assert_fig);
            }

            /*
             * Check if loopback bit is to be asserted by this DTE interface
             */
            if (idb->loopback)
                assert_loopback(idb, ctx);
            else
                deassert_loopback(idb, ctx);
        }
        else {  /* DCE interface */
            if (hdx_mode) {
                deassert_CTS(idb, ctx);
                HDX_LOG_FIG(ctx, cts_deassert_fig);
                if (ssb->controlled_carrier) {
                    deassert_DCD(idb, ctx);
                    HDX_LOG_FIG(ctx, dcd_deassert_fig);
                }
                else {
                    assert_DCD(idb, ctx);
                    HDX_LOG_FIG(ctx, dcd_assert_fig);
                }
            }
            else {
                assert_CTS(idb, ctx);
                assert_DCD(idb, ctx);
                HDX_LOG_FIG(ctx, cts_assert_fig);
                HDX_LOG_FIG(ctx, dcd_assert_fig);
            }
        }

        /*
         * Indicate that we are up or down based on DCD/DSR or DTR
         */
        if (idb->dte_interface) {
            if (ssb->ignore_dcd) {
                if (DTE_DSR_is_asserted(reg_table))
                    net_cstate(idb, IDBS_UP);
                else
                    net_cstate(idb, IDBS_DOWN);
            }
            else {
                if (DTE_DCD_is_asserted(reg_table))
                    net_cstate(idb, IDBS_UP);
                else
                    net_cstate(idb, IDBS_DOWN);
            }
        }
        else {  /* DCE interface */
            if (DCE_DTR_is_asserted(reg_table))
                net_cstate(idb, IDBS_UP);
            else
                net_cstate(idb, IDBS_DOWN);
        }
    }
    else {  /* No cable present */
        net_cstate(idb, IDBS_DOWN);
    }
      
    /*
     * Enable the receiver and transmitter and enable interrupts. Since the
     * timer interrupts are needed only in half-duplex mode, we enable them
     * only in half-duplex mode.
     */
    sendccr(reg_table, CCR_ENTX | CCR_ENRX);
    if (hdx_mode)
        reg_table->ier = (IER_SYNC_BASIC | IER_TIMER);
    else
        reg_table->ier = IER_SYNC_BASIC;
}

/*
 * cd2430_reset_channel()
 *      This function will reset the channel specified. All hardware related
 * reset pertaining to the Cirrus controller will be done in this routine.
 * The channel access register (CAR) must be set before a call to this
 * function is made.
 */
static void cd2430_reset_channel (hwidbtype *idb)
{
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;

    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    sendccr(reg_table, CCR_DISTX | CCR_DISRX);
    sendccr(reg_table, CCR_CLRCH);
    reg_table->ier = 0;

    deassert_modem_signals(idb);

    if (idb->dte_interface) {
        HDX_LOG_FIG(ctx, dtr_deassert_fig);
        HDX_LOG_FIG(ctx, rts_deassert_fig);
    }
    else {
        HDX_LOG_FIG(ctx, dcd_deassert_fig);
        HDX_LOG_FIG(ctx, dsr_deassert_fig);
        HDX_LOG_FIG(ctx, cts_deassert_fig);
    }

    MAYBE_DROPPED(idb);    /* May lose output from reset */
}

/*
 * cd2430_clear_receiver()
 *	This function will clear the receiver. This is used mainly in 
 * bisync mode to put the receiver back in syn-hunt mode. This is called
 * from interrupt context, and hence there is no need to set the CAR.
 */
static void
cd2430_clear_receiver (hwidbtype *idb,
                       cd2430_context_t *ctx, cd2430_quadart *reg_table)
{
    sendccr(reg_table, CCR_DISRX);
    sendccr(reg_table, CCR_SET1 | CCR_CLRRCV);
 
    cd2430_reset_rx_buffers(idb);
 
    /*
     * Allocate new receive buffers.
     */
    alloc_rx_buffers(idb, 0);

    /*
     * Now, enable the receiver
     */
    sendccr(reg_table, CCR_ENRX);
}

/*
 * Initialize the driver context structure. Note that we will not clear
 * any error counters in the context structure. Thus, these error 
 * counters are cumulative counters. 
 */
static void cd2430_driver_context_init (hwidbtype *idb, cd2430_context_t *ctx)
{
    boolean hdx_mode;

    /*
     * Compute and store the controller and channel number for this
     * interface.
     */

    cd2430_driver_context_init_chan(idb,ctx);

    ctx->reset_interface = FALSE;

    /*
     * Set context structure fields that depend on the encapsulation
     * on this interface.
     */
    switch (idb->enctype) {
    case ET_SMDS:
    case ET_HDLC:
    case ET_FRAME_RELAY:
    case ET_PPP:
    case ET_SDLCP:
    case ET_SDLCS:
    case ET_SDLC:
    case ET_STUN:
    case ET_LAPB:
    case ET_X25:
        ctx->enctype = idb->enctype;
        ctx->base_mode = MODE_HDLC;
        break;
    case ET_BSTUN:
        ctx->enctype = idb->enctype;
        ctx->base_mode = MODE_BISYNC;
        break;
    default:
        errmsg(&msgsym(BOGUSENCAP, LINK), idb->hw_namestring, idb->enctype);
        ctx->enctype = ET_HDLC;
        ctx->base_mode = MODE_HDLC;
        break;
    }

    if (ctx->base_mode != MODE_BISYNC) {
       idb->poll_protocol_flags = PP_ROLE_UNDEFINED;
    }

    /*
     * Set flap protection parameters in the context structure for
     * this interface
     */
    ctx->flap_rate = SERIAL_FLAP_RATE;
    ctx->flap_count = 0;
    TIMER_STOP(ctx->flap_time);
    ctx->flap_backoff = 0;
    ctx->flap_enabled = FALSE;

    /*
     * Determine if this interface is in half-duplex mode. That would be
     * the case if either SDLC half-duplex or bisync were enabled.
     */
    hdx_mode = idb->hdx_flags & HDX_FLAG_ON;

    /*
     * Initialize the pointers in the context structure to the appropriate 
     * state machines. Also initialize the starting state for the state 
     * machines. If we are turning off half-duplex operation, set the 
     * FSM pointers to NULL and make the state machine states invalid.
     */
    if (hdx_mode) {
        if (idb->dte_interface) {
            ctx->hdx_tx_fsm = cd2430_sync_hdx_dte_tx_fsm;
            ctx->hdx_tx_state = HDX_DTE_TX_STATE_READY;
            ctx->hdx_rx_fsm = cd2430_sync_hdx_dte_rx_fsm;
            ctx->hdx_rx_state = HDX_DTE_RX_STATE_READY;
        }
        else {
            ctx->hdx_tx_fsm = cd2430_sync_hdx_dce_tx_fsm;
            ctx->hdx_tx_state = HDX_DCE_TX_STATE_READY;
            ctx->hdx_rx_fsm = cd2430_sync_hdx_dce_rx_fsm;
            ctx->hdx_rx_state = HDX_DCE_RX_STATE_READY;
        }
    }
    else {  /* Interface is operating in full-duplex mode */
        /*
         * Initialize the state machines
         */
        ctx->hdx_tx_fsm = NULL;
        ctx->hdx_tx_state = HDX_TX_STATE_NULL;
        ctx->hdx_rx_fsm = NULL;
        ctx->hdx_rx_state = HDX_RX_STATE_NULL;
    }

    /*
     * Set the pointers to the registers to the high and low Cirrus serial
     * device registers.
     */

    cd2430_driver_context_init_var(idb,ctx);

    /*
     * Initialize the helper mask and the LED register mask for 
     * this interface.
     */


    /*
     * Initialize the half-duplex related timer structures if this is the
     * time that the structures are being created.
     */
    if (ctx->rts_timeout == NULL) {
        ctx->rts_timeout = malloc(sizeof(cd2430_timer_t));
        cd2430_set_timer_params(HDX_RTS_TIMEOUT_DEFAULT, ctx->rts_timeout,
                                MAX_TICKS_TIMER1); 
    }
    
    if (ctx->cts_delay == NULL) {
        ctx->cts_delay = malloc(sizeof(cd2430_timer_t));
        cd2430_set_timer_params(HDX_CTS_DELAY_DEFAULT, ctx->cts_delay,
                                MAX_TICKS_TIMER1); 
    }

    if (ctx->rts_drop_delay == NULL) {
        ctx->rts_drop_delay = malloc(sizeof(cd2430_timer_t));
        cd2430_set_timer_params(HDX_RTS_DROP_DELAY_DEFAULT, 
                                ctx->rts_drop_delay, MAX_TICKS_TIMER1); 
    }

    if (ctx->cts_drop_timeout == NULL) {
        ctx->cts_drop_timeout = malloc(sizeof(cd2430_timer_t));
        cd2430_set_timer_params(HDX_CTS_DROP_TIMEOUT_DEFAULT, 
                                ctx->cts_drop_timeout, MAX_TICKS_TIMER1);
    }

    if (ctx->dcd_txstart_delay == NULL) {
        ctx->dcd_txstart_delay = malloc(sizeof(cd2430_timer_t));
        cd2430_set_timer_params(HDX_DCD_TXSTART_DELAY_DEFAULT,
                                ctx->dcd_txstart_delay, 
                                MAX_TICKS_TIMER2);
    }

    if (ctx->dcd_drop_delay == NULL) {
        ctx->dcd_drop_delay = malloc(sizeof(cd2430_timer_t));
        cd2430_set_timer_params(HDX_DCD_DROP_DELAY_DEFAULT,
                                ctx->dcd_drop_delay, 
                                MAX_TICKS_TIMER2);
    }

    if (ctx->transmit_delay == NULL) {
        ctx->transmit_delay = malloc(sizeof(cd2430_timer_t));
        cd2430_set_timer_params(HDX_TRANSMIT_DELAY_DEFAULT,
                                ctx->transmit_delay,
                                MAX_TICKS_TIMER2);
    }

    /*
     * Note if the serial cable for this interface is inserted
     */
    ctx->cable_present = cable_present(ctx);
}

/*
 * Set the Channel Option Registers for the port corresponding to the
 * IDB passed in as a parameter to this function such that this port
 * now is in the sync mode.
 */
void cd2430_sync_mode_init (hwidbtype *idb)
{
    SAVE_CALLER();
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    serialsb *ssb;
    uint bitrate;
    leveltype level;

    if (serial_debug) {
        buginf("\nSerial%d, cd2430_sync_mode_init",idb->unit);
	bugtrace();
    }

    level = raise_interrupt_level(NETS_DISABLE);

    ctx = INSTANCE;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Initialize the driver context structure. Make sure that the register
     * table is extracted from the context structure only *after* the 
     * context structure has been initialized.
     */
    cd2430_driver_context_init(idb, ctx);
    reg_table = ctx->reg_table;

    /*
     * Reset the appropriate channel on the controller.
     */
    setchan_sync(reg_table, idb);
    cd2430_reset_channel(idb);

    /*
     * Change the state of the interface to reset.
     */
    if (idb->state != IDBS_ADMINDOWN)
        net_cstate(idb, IDBS_RESET);

    if (serial_debug)
        buginf("\nCIRRUS(%d): Reset from 0x%x", idb->unit, caller());

    /*
     * Flush the hold queue and reset the buffers.
     */
    holdq_reset(idb);
    cd2430_reset_rx_buffers(idb);
    cd2430_reset_tx_buffers(idb);

    les_serial_init_offset(idb);

    /*
     * Until clock rate is chosen, leave the interface inactive
     */
    bitrate = ssb->serial_clockindex;

    /*
     * Decide whether we are DCE or DTE depending on what cable is
     * plugged in.
     */
    idb->dte_interface = DTE_interface(ctx);

    /*
     * Pulse DTR if configured to do so.
     */
    if (!serial_pulse_DTR(idb) && idb->state != IDBS_ADMINDOWN) {
        reset_interrupt_level(level);
        return;
    }

    cd2430_set_channel_option_regs(idb, ctx->base_mode);

    cd2430_set_clock_option_regs(idb, bitrate);

    cd2430_buffer_pools_init(idb);

    cd2430_sync_setup_isr_vectors(idb);

    alloc_rx_buffers(idb, 0);

    /*
     * Invert transmit clock for DTE if requested
     */
    if (idb->dte_interface) {
        if (ssb->invert_txc) 
            CD2430_SET_MODEM_CTRL_TXC_INV;
        else
            CD2430_RESET_MODEM_CTRL_TXC_INV;
    }

    /*
     * Set DCE terminal timing if requested
     */
    if (!idb->dte_interface) {
        if (ssb->dce_terminal_timing_enable)
            CD2430_SET_MODEM_CTRL_DCE_TERM;
        else
            CD2430_RESET_MODEM_CTRL_DCE_TERM;
    }

    /* Tell the hardware we are in sync mode */
    CD2430_SET_PLATFORM_HW_SYNC;

    /*
     * Initialize the controller bits for the LICR register for this
     * channel.
     */
    CD2430LICR_WRITE(reg_table, ctx->ctrl);

    /*
     * If the line is not admin down, assert appropriate modem signals etc.
     */
    if (idb->state != IDBS_ADMINDOWN)
        cd2430_enter_transparent_mode(idb);

    reset_interrupt_level(level);
}

/*
 * Reset a sync port
 */
static void cd2430_sync_reset (hwidbtype *idb)
{
    /*  
     * Return if this interface is not in sync mode.
     */
    if (!channel_mode_sync(idb))
        return;

    /*
     * Re-initialize the channel to be in sync mode.
     */
    cd2430_sync_mode_init(idb);

    idb->counters.resets++;
}


/*************************************************************************
 *     Fast-switching Related Routines for the CD2430 Sync Driver
 *
 * These functions get invoked in connection with fast-switching packets
 * on the Cirrus serial interfaces. Note that fast-switching on the
 * CD2430 controller is a little different than usual because of the way
 * the controller behaves in interrupt context as opposed to the
 * background context. To take care of the possibility that the
 * controller corresponding to the channel out of we which we intend to
 * fast-switch a packet may be in the interrupt context, we use a helper
 * process which triggers on the value of the helper mask. It is expected
 * that the helper process will be used relatively infrequently to
 * 'fast-switch' packets even when a particular controller on the exit
 * path is in the interrupt context.
 *************************************************************************/



/*
 * cd2430_fastsend()
 *      Start or queue output from interrupt state
 */
static void cd2430_fastsend (hwidbtype *idb, paktype *pak)
{
    cd2430_quadart *reg_table;
    cd2430_context_t *ctx;
    paktype *qpak, *workpak;
    int i, buf_index;
    ulong acct_proto;
    leveltype level;
    traffic_shape_t *shape;

    if (idb->state != IDBS_UP) {
        idb->counters.output_total_drops++;
        datagram_done(pak);
        return;
    }

    level = raise_interrupt_level(NETS_DISABLE);

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
        traffic_shape_defer_sending(shape, pak);
	reset_interrupt_level(level);
        return;
    }

    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    /*
     * First try to do a true fast-switch. This requires that the CD2430
     * corresponding to this IDB not be in the interrupt context. We will
     * check for that by reading the stack register.
     */
    if (reg_table->stk == 0)   /* Controller is not in interrupt context */
    {
        /*
         * Set the CAR on the controller to refer to this channel.
         */
        setchan_sync(reg_table, idb);

        /*
         * Figure out which buffer the CD2430 is waiting to transmit next
         */
        buf_index = (reg_table->dmabsts & DMABSTS_NTBUF) ? 1 : 0;

        /*
         * Before attempting to set passed in packet for transmission
         * transmit any packets that may be in the output holdq. If either
         * buffer A or buffer B is free after doing that, then set the
         * passed in packet for transmission. If not, enqueue the packet
         * in the holdq and use the semi fast-switch mechanism via the
         * helper mask to send this packet.
         */
        for (i = 0; i < 2; i++, buf_index ^= 1) {
            if (buf_index) {
                if (reg_table->btbsts & TBSTS_24OWN)
                    continue;
            }
            else {
                if (reg_table->atbsts & TBSTS_24OWN)
                    continue;
            }
            /*
             * Free up any packet that may be associated with this
             * buffer owned by the cpu.
             */
            cd2430_release_tx_pak(idb, ctx, buf_index);

            qpak = cd2430_extract_tx_pak(idb);

            if (qpak == NULL) {
                workpak = pak;
                pak = NULL;
            }
            else
                workpak = qpak;

            acct_proto = (ulong)workpak->acct_proto;

            arm_tx_dma(reg_table, VTOP(workpak->datagramstart),
                                             workpak->datagramsize, buf_index);

            /*
             * Cache this packet pointer in the corresponding element of
             * the outpak array.
             */
            ctx->outpak[buf_index] = workpak;
            TIMER_START(idb->xmittimer, CD2430_TX_TIMEOUT);

            /*
             * Take care of accounting.
             */
            idb->counters.tx_cumbytes_fast[acct_proto] += workpak->datagramsize;
            idb->counters.outputs_fast[acct_proto] += 1;

            if ((workpak->enctype == ET_HDLC) &&
                (GETSHORT(workpak->datagramstart) & HDLC_BCAST))
                idb->counters.output_broad++;

            /*
             * If we ran out of packets in the holdq and got here we MUST
             * have transmitted the packet that was passed in to us.
             */
            if (qpak == NULL) {
                ctx->fs_count++;  /* Count it for analysis */
                break;
            }
        }
    }
    else {
        /*
         * This controller is in the interrupt context; Hence true
         * fast switching cannot be done. We will semi fast-switch the
         * packet via the mask and the helper process mechanism.
         */
        pak->if_output = idb->firstsw;
        holdq_enqueue_fast(idb, pak);
        ctx->sfs_count++;  /* Count it for analysis */
        pak = NULL;

        CD2430_SET_HELPER_MASK(idb->slot, ctx);

        process_wakeup(helper_pid);
    }

    /*
     * Now, check for the case where the packet that was given to this
     * routine for fast switching could not be transmitted because there
     * were packets ahead of it in the holdq. In that case, queue this
     * packet up in the hold queue. Note that we do not have set the
     * mask since this channel is already transmitting.
     */
    if (pak) {
        pak->if_output = idb->firstsw;
        holdq_enqueue_fast(idb, pak);
        ctx->sfs_count++;  /* Count it for analysis */
    }
    reset_interrupt_level(level);
}


/*************************************************************************
 *     Initialization Related Functions for the CD2430 Sync Driver
 * 
 * The routines used in the initialization for this driver. The
 * initialization consists of the subsystem initialization and the
 * initialization of an IDB when it is first created.
 *************************************************************************/
/*
 * cd2430_hw_set_config()
 *	This is the routine that will be used by the routine pointed
 * to by the state config vector in the IDB to set config parameters  
 * for this interface. 
 */
static boolean cd2430_hw_set_config (hwidbtype *idb, itemlist *ilist,
                                      serialsb *ssb)
{
    itemdesc *item;
    boolean any_unhandled = FALSE;   /* assume we've handled all items */
    tinybool this_item_unhandled;
    cd2430_context_t *ctx;
    cd2430_quadart *reg_table;
    leveltype level;
 
    ctx = INSTANCE;
    reg_table = ctx->reg_table;

    while ((item = itemlist_getnext(ilist))) {
        if (item->handled)
            continue;
        this_item_unhandled = FALSE;
 
        switch (item->item) {
 
          case SERIAL_HW_DCE_TERMINAL_TIMING_ENABLE:
            ssb->dce_terminal_timing_enable = item->u.l_value;
            cd2430_sync_mode_init(idb);
            break;
 
          case SERIAL_HW_INVERT_TXC:
            ssb->invert_txc = item->u.l_value;
            cd2430_sync_mode_init(idb);
            break;
 
          case SERIAL_HW_NRZI_ENABLE:
            ssb->nrzi_enable = item->u.l_value;
            cd2430_sync_mode_init(idb);
            break;
 
          case SERIAL_HW_IGNORE_DCD:
            ssb->ignore_dcd = item->u.l_value;
            cd2430_sync_mode_init(idb);
            break;
 
          case SERIAL_HW_IGNORE_LOCAL_LOOPBACK:
            ssb->ignore_local_loopback = item->u.l_value;
	    ctx->ignore_local_loopback = item->u.l_value;
            break;

          case SERIAL_HW_SET_CLOCKRATE:
            cd2430_clockrate_command(item->u.buffer);
            break;

          case SERIAL_HW_CONTROLLED_CARRIER:
            ssb->controlled_carrier = item->u.l_value;
            cd2430_sync_mode_init(idb);
            break;

          case SERIAL_HW_RTS_TIMEOUT:
            ssb->hdx_rts_timeout = item->u.l_value;
            cd2430_set_timer_params(item->u.l_value, 
                                    ctx->rts_timeout, MAX_TICKS_TIMER1);
            break;

          case SERIAL_HW_CTS_DELAY:
            ssb->hdx_cts_delay = item->u.l_value;
            cd2430_set_timer_params(item->u.l_value, 
                                    ctx->cts_delay, MAX_TICKS_TIMER1);
            break;

          case SERIAL_HW_RTS_DROP_DELAY:
            ssb->hdx_rts_drop_delay = item->u.l_value;
            cd2430_set_timer_params(item->u.l_value, 
                                    ctx->rts_drop_delay, MAX_TICKS_TIMER1);
            break;
 
          case SERIAL_HW_CTS_DROP_TIMEOUT:
            ssb->hdx_cts_drop_timeout = item->u.l_value;
            cd2430_set_timer_params(item->u.l_value,           
                                    ctx->cts_drop_timeout, MAX_TICKS_TIMER1);
            break;

          case SERIAL_HW_DCD_TXSTART_DELAY:
            ssb->hdx_dcd_txstart_delay = item->u.l_value;
            cd2430_set_timer_params(item->u.l_value,           
                                    ctx->dcd_txstart_delay, MAX_TICKS_TIMER2);
            break;

          case SERIAL_HW_DCD_DROP_DELAY:
            ssb->hdx_dcd_drop_delay = item->u.l_value;
            cd2430_set_timer_params(item->u.l_value,           
                                    ctx->dcd_drop_delay, MAX_TICKS_TIMER2);
            break;

          case SERIAL_HW_TRANSMIT_DELAY:
            ssb->hdx_transmit_delay = item->u.l_value;
            cd2430_set_timer_params(item->u.l_value,                       
                                    ctx->transmit_delay, MAX_TICKS_TIMER2);
            break;

          case SERIAL_HW_TEST_RTS:
            if (idb->dte_interface) {       /* If DTE (de)assert it */
                if (item->u.l_value)
                    assert_RTS(idb, ctx);
                else
                    deassert_RTS(idb, ctx);
            }
            else {                      /* If DCE report its state */
                printf("DCE: Signal RTS is %sasserted",
                       DCE_RTS_is_asserted(reg_table) ? "" : "not ");
            }
            break;

          case SERIAL_HW_TEST_CTS:
            if (!idb->dte_interface) {  /* If DCE (de)assert it */
                if (item->u.l_value)
                    assert_CTS(idb, ctx);
                else
                    deassert_CTS(idb, ctx);
            }
            else {                      /* If DTE report its state */
                printf("DTE: Signal CTS is %sasserted",
                       DTE_CTS_is_asserted(reg_table) ? "" : "not ");
            }
            break;

          case SERIAL_HW_TEST_DTR:
            if (idb->dte_interface) {  /* If DTE (de)assert it */
                if (item->u.l_value)
                    assert_DTR(idb, reg_table);
                else
                    deassert_DTR(idb, reg_table);
            }
            else {                     /* If DCE report its state */
                printf("DCE: Signal DTR is %sasserted",
                       DCE_DTR_is_asserted(reg_table) ? "" : "not ");
            }
            break;

          case SERIAL_HW_TEST_DSR:
            if (!idb->dte_interface) {  /* If DCE (de)assert it */
                if (item->u.l_value)
                    assert_DSR(idb, reg_table);
                else
                    deassert_DSR(idb, reg_table);
            }
            else {                      /* If DTE report its state */
                printf("DTE: Signal DSR is %sasserted",
                       DTE_DSR_is_asserted(reg_table) ? "" : "not ");
            }
            break;

          case SERIAL_HW_TEST_DCD:
            if (!idb->dte_interface) {  /* If DCE (de)assert it */
                if (item->u.l_value)
                    assert_DCD(idb, ctx);
                else
                    deassert_DCD(idb, ctx);
            }
            else {                      /* If DTE report its state */
                printf("DTE: Signal DCD is %sasserted",
                       DTE_DCD_is_asserted(reg_table) ? "" : "not ");
            }
            break;

          case SERIAL_HW_TEST_LOOPBACK:
            if (idb->dte_interface) {  /* If DTE (de)assert it */
                if (item->u.l_value)
                    assert_loopback(idb, ctx);
                else
                    deassert_loopback(idb, ctx);
            }                          /* If DCE report its state */
            else {
                printf("DCE: Loopback bit is %sasserted",
                       DCE_loopback_is_asserted(ctx) ? "" : "not ");
            }
            break;

          case SERIAL_HW_TEST_TRANSMIT:
            ctx->test_tx_pak_cnt = item->u.l_value;
            break;

          case SERIAL_HW_HDX_LOG_ENABLE:
            if (item->u.l_value) { /* Turning ON of logging is requested */
                ssb->hdx_log_enable = TRUE;
                if (ctx->hdx_log) /* Logging already enabled */
                    break;
                level = raise_interrupt_level(ALL_DISABLE);
                ctx->hdx_log_index = 0;
                ctx->hdx_log = 
                          malloc(sizeof(hdx_log_element_t) * HDX_LOG_LENGTH);
                reset_interrupt_level(level);
                if (!(ctx->hdx_log)) {
                    printf("%%Error: No memory to enable logging");
                    break;
                }
            }
            else { /* Turning OFF of logging is requested */
                ssb->hdx_log_enable = FALSE;
                level = raise_interrupt_level(ALL_DISABLE);
                if (ctx->hdx_log)
                    free(ctx->hdx_log);
                ctx->hdx_log_index = 0;
                ctx->hdx_log = NULL;
                reset_interrupt_level(level);
            }
            break;

          case SERIAL_HW_HDX_LOG_CLEAR:
            level = raise_interrupt_level(ALL_DISABLE);
            if (ctx->hdx_log)
                bzero(ctx->hdx_log, HDX_LOG_LENGTH*sizeof(hdx_log_element_t));
            reset_interrupt_level(level);
            break;

          default:
            this_item_unhandled = TRUE;
            any_unhandled = TRUE;
            break;
        }
 
        if (!this_item_unhandled)
            item->handled = TRUE;
    }
 
    return(!any_unhandled);
}

static boolean cd2430_hwidb_state_config (hwidbtype *hwidb, ulong opcode,
                                           itemlist *ilist)
{
    boolean ret;
    serialsb *ssb;
 
    if ((hwidb == NULL) && (ilist == NULL))
        return (FALSE);
 
    if (ilist->item_count == -1)
        ilist->item_count = itemlist_count(ilist);
 
    ssb = idb_use_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
    if (ssb == NULL)
        return (FALSE);
 
    switch (opcode) {
      case IDB_CONTROL_SET_CONFIG:
        ret = cd2430_hw_set_config(hwidb, ilist, ssb);
        break;

      default:
        ret = FALSE;
        break;
    }
    idb_release_hwsb_inline(hwidb, HWIDB_SB_SERIAL);
 
    if (!ret)
        ret = (*ssb->hw_super_state_config)(hwidb, opcode, ilist);
    return (ret);
}

/*
 * IDB Initialization
 */
void cd2430_init_sync_idb (hwidbtype *idb)
{
    idbtype *swidb;
    serialsb *ssb;
    
    swidb = idb->firstsw;
    idb->app_type = APPTYPE_NONE;
    idb->status = IDB_SERIAL|IDB_BCAST|IDB_HEARSELF|IDB_BRIDGE|IDB_SYNC;
    idb->state = IDBS_INIT;
    idb->type = IDBTYPE_CD2430;
    idb->typestring = "CD2430 in sync mode";
    serial_idb_init(idb, MAXSERIALSIZE, PAN_MAXDGRAM,
                    VISIBLE_LES_LSS_BANDWIDTH, LES_SERIAL_DELAY);

    idb->serial_flags |= CD2430_SERIAL_FLAG_DEFAULT;

    idb->devctl = malloc(sizeof(cd2430_context_t));

    idb->hdx_flags |= HDX_FLAG_CAPABLE; /* Init half-duplex enabling flag */

    /*
     * Processing function vectors for a CD2430 port in sync mode.
     */
    idb->soutput = cd2430_sync_txstart;
    idb->reset = cd2430_sync_reset;
    idb->shutdown = cd2430_sync_shutdown;
    idb->enable = (enable_t)return_nothing;
    idb->setup_bridging = les_serial_setup_bridging;
    idb->device_periodic = cd2430_periodic;
    idb->show_controller = cd2430_sync_show_controller;
    idb->show_serial_state = cd2430_show_serial_state;
    idb->fastsend = cd2430_fastsend;
    idb->set_maxdgram = cd2430_set_maxdgram;

    idb->restart_rx = cd2430_flush_restart_receive;
    idb->disable_rx = cd2430_disable_receiver;
    idb->getsysbuf  = cd2430_get_sysbuffer;
    idb->rx_in_progress = cd2430_syn_detected;

    GET_TIMESTAMP(idb->state_time);
    idb->error_threshold = 5000;
    idb->error_count = 0;
    idb->fast_switch_flag = 0;
    idb->reliability = 255;
    idb->load = 1;
    idb->rxload = 1;
    idb->xmitdelay_default = CD2430_DELAY_FLAGS_DEFAULT;
    idb->xmitdelay = CD2430_DELAY_FLAGS_DEFAULT;
    idb->counters.resets = 0;
    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;

    GET_TIMESTAMP(idb->state_time);

    /*
     * Init software IDB fields.
     */
    swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
    swidb->arp_probe = swidb->arp_probe_def = FALSE;
    swidb->arp_snap = swidb->arp_snap_def = FALSE;
    swidb->span_disabled = FALSE;

    serial_idb_enqueue(idb);
    idb_enqueue(idb);

    SET_CD2430_SYNC_IDBS(idb);

    reg_invoke_sw_idb_init(swidb, TRUE);
    ssb = idb_use_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->hw_super_state_config = idb->state_config;
    idb->state_config = cd2430_hwidb_state_config;
    idb_release_hwsb_inline(idb, HWIDB_SB_SERIAL);

    /*
     * Set the channel in the sync mode
     */
    cd2430_sync_mode_init(idb);

}


