/* $Id: if_c3000_pcbus.c,v 3.6.28.6 1996/08/28 12:57:32 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_c3000_pcbus.c,v $
 *------------------------------------------------------------------
 * if_c3000_pcbus.c - PC bus interface driver (for Fiji)
 * 
 * April 1994, Johnny Chan
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c3000_pcbus.c,v $
 * Revision 3.6.28.6  1996/08/28  12:57:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.28.5  1996/08/26  15:10:16  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.6.28.4  1996/08/13  02:19:51  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.6.28.3  1996/07/09  06:00:29  ppearce
 * CSCdi59527:  inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.6.28.2  1996/05/17  11:26:12  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.14.2  1996/04/26  15:22:04  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.6.14.1  1996/04/03  19:58:03  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.28.1  1996/03/18  20:41:18  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.3  1996/03/13  01:49:05  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.6.2.2  1996/03/09  05:07:32  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.6.2.1  1996/01/24  22:23:01  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.6  1996/01/22  06:37:32  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/07  22:30:20  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.4  1995/12/06  18:30:50  gstovall
 * CSCdi44769:  Interface idb queues are not getting populated correctly
 * Make sure the hwidbs are in the same order on the media hwidbQs as on
 * the hwidbQ.
 *
 * Revision 3.3  1995/11/17  17:38:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:45  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:31:10  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:14:14  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/21  03:01:01  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  06:47:04  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:33:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "logger.h"
#include "fastswitch_registry.h"
#include "../partner/partner_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "stacks.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../if/ether.h"
#include "../wan/frame_relay.h"
#include "../wan/smds.h"
#include "../wan/ppp.h"
#include "../os/pool.h"
#include "../os/buffers.h"

#include "if_les.h"
#include "if_c3000_pcbus.h"

#include "../if/network.h"
#include "../ip/ip.h"
#include "../wan/atm_dxi.h"
#include "../if/serial_inline.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */

#include "../srt/srb_core.h"
#include "../srt/srb_core_registry.h"
#include "../parser/parser_defs_parser.h"

/*
 * Validate Shared RAM control block pointers.
 */
static inline boolean validate_pcbus_inline (hwidbtype *idb,
					     pcbus_instance_t *ds,
					     boolean flag)
    /*
     * flag = TRUE indicates that RX buffer should be cleaned up
     *             and the write-read test done in the TX buffer.
     */
{
    volatile pc_if_ptrs_t *tx_ptr, *rx_ptr;

    /*
     * Validate the control block in Shared RAM.
     * If anything is screwed up, destroy the magic #. 
     */
    if (ds->ifaddr->magic != PCBUS_SHRAM_MAGIC)
        return (FALSE);

    bcopy((uchar *)ds->ifaddr, (uchar *)&(ds->c_blk), sizeof(shram_block_t));

    tx_ptr = ds->tx_ptr;
    rx_ptr = ds->rx_ptr;

    /*
     * Magic # OK. Check pointers.
     */
    if (tx_ptr->offset < rx_ptr->offset) { /* Tx buffer before Rx buffer */
        if ((tx_ptr->offset + tx_ptr->size) > rx_ptr->offset) {
            /* Tx buffer overflows into Rx buffer */
            ds->ifaddr->magic = ~PCBUS_SHRAM_MAGIC;
            return (FALSE);
        }
    }

    /*
     * Check for GET, PUT or WRAP pointer going beyond buffer.
     */
    if ((tx_ptr->get_ptr > tx_ptr->size) ||
        (tx_ptr->put_ptr > tx_ptr->size) ||
        (tx_ptr->wrap_ptr > tx_ptr->size)) {
        ds->ifaddr->magic = ~PCBUS_SHRAM_MAGIC;
        return (FALSE);
    }
    
    if (tx_ptr->put_ptr < tx_ptr->get_ptr) {  
        /* PUT pointer < GET pointer */
        if (tx_ptr->get_ptr > tx_ptr->wrap_ptr) {
            /* GET pointer beyond WRAP pointer */
            ds->ifaddr->magic = ~PCBUS_SHRAM_MAGIC;
            return (FALSE);
        }
    }

    /*
     * Now check for invalid PUT or GET pointer in the Rx buffer
     */
    if ((rx_ptr->put_ptr > rx_ptr->size) ||
        (rx_ptr->get_ptr > rx_ptr->size)) {
        ds->ifaddr->magic = ~PCBUS_SHRAM_MAGIC;
        return (FALSE);
    }

    if (flag) {
        volatile ulong *data_ptr;

        if (pcbus_debug)
            buginf("\n%%%s: R/w test in TX buffer.", idb->hw_namestring);

        /*
         * Do some read-writes in the Tx buffer area and check the
         * memory. Use the PUT pointer address. Do this only if the
         * MAGIC value is good.
         */
        /* 
         * Get Shared RAM address pointed to by the TX PUT pointer :
         * Shared RAM start addr + TX buffer offset + PUT ptr
         */
        data_ptr = (ulong *)((ulong)ds->ifaddr + tx_ptr->offset + 
                                    tx_ptr->put_ptr); 
        *data_ptr = 0x1DEAFCAB;
        if (*data_ptr != 0x1DEAFCAB) {
            ds->ifaddr->magic = ~PCBUS_SHRAM_MAGIC;
            return (FALSE);
        }
        /*
         * Now clean up RX buffer by setting the GET ptr = PUT ptr.
         * Check that GET ptr is indeed = PUT ptr. If not, drop the
         * guillotine.
         */
        rx_ptr = (pc_if_ptrs_t *)&ds->ifaddr->rx_buff;
        rx_ptr->get_ptr = rx_ptr->put_ptr;
        if (rx_ptr->get_ptr != rx_ptr->put_ptr)
            ds->ifaddr->magic = ~PCBUS_SHRAM_MAGIC;
    }  /* end if (flag) */

    return (ds->ifaddr->magic == PCBUS_SHRAM_MAGIC);
}


/*
 * pcbus_getbuf
 * Get a packet buffer and initialize appropriate fields in the packet.
 * This routine is used by the receive interrupt routine and implements
 * input throttling:
 */
static paktype * pcbus_getbuf (hwidbtype *idb)
{
    uchar *start;
    paktype *pak;
    leveltype SR;
    idbtype *swidb = idb->firstsw;
    
    /*
     * Check for input congestion:
     */
    if (idb->input_qcount > idb->input_defqcount)
        return(NULL);

    /* First, disable higher level net interrupt */
    SR = raise_interrupt_level(NETS_DISABLE); 

    pak = NULL;
    /*
     * Get a cached buffer and charge the buffer to the input
     * interface, or note that we don't have one to give:
     */
    pak = pool_dequeue_cache(idb->pool);
    reset_interrupt_level(SR);

    if (!pak)
	pak = pool_getbuffer(idb->pool);
    if (pak) {
        if (pcbus_debug) {
	    if (!validmem(pak)) {
	        buginf("\n%%%s pcbus_getbuf: Invalid pak=%x",
		       idb->hw_namestring, pak);
	        return(NULL);
	    }
	    if (pak->next != NULL) {
	        buginf("\n%%%s pcbus_getbuf: buffer in list, pak=%x, next=%x",
                        idb->hw_namestring, pak, pak->next);
	        return(NULL);
	    }
	    if (pak->refcount != 1) {
	        buginf("\n%%%s pcbus_getbuf: pak->refcount not zero (%d),"
		       " pak=%x",
	    	       idb->hw_namestring, pak->refcount, pak);
	        return(NULL);
	    }
        }
	
	set_if_input(pak, swidb);

	/*
	 * Initialize the packet encapsulation type and datagramstart:
	 */
	start = (uchar *)(pak->data_area + idb->start_offset);
	pak->datagramstart = start;
	return(pak);
    } else
	return(NULL);
}


/*
 * Stop the pcbus.
 * Must be called with network interfaces disabled.
 */
static void pcbus_stop (hwidbtype *idb)
{
    register pcbus_instance_t *ds;
    leveltype SR;

    /*
     * Flag to the PC Host that interface is shut down
     * only if the Shared RAM control block is initialized.
     * Else, the PC Host will overwrite it anyway.
     */
    if (FIJI_SHRAM_INITIALIZED) {  /* Shared RAM-initialized Flag set */
        ds = (pcbus_instance_t *)INSTANCE;
        if (ds) {
            if (ds->ifaddr->magic  == PCBUS_SHRAM_MAGIC) {
                SR = raise_interrupt_level(LEVEL2_DISABLE);
                ds->ifaddr->state = PCBUS_SHUTDOWN;
                reset_interrupt_level(SR);
                if (pcbus_debug)
                    buginf("\n%%%s: Shared RAM state set to SHUTDOWN",
			   idb->hw_namestring);
            }
        }
        else {
#ifdef DEBUG
            buginf("\n%%%s: ### driver struct is NULL!", idb->hw_namestring);
#endif
            crashdump(0);
        }
    }
}


/*
 * PC bus throttle
 * Throttle the PC bus to give process level code a chance to run
 */
static inline void pcbus_throttle (hwidbtype *idb)
{
    idb_mark_throttled(idb);
}


/*
 * Handle a RX interrupt:
 */
static void pcbus_RX_interrupt (hwidbtype *idb)
{
    pcbus_instance_t *ds;
    pcbus_frame_t *input_packet;
    pc_if_ptrs_t *rx_ptr;
    uchar *datagramstart;
    paktype *pak = NULL;
    boolean prio;
    ushort rx_count;
    leveltype SR;
    idbtype *swidb = idb->firstsw;

    SR = raise_interrupt_level(NETS_DISABLE);

    ds = (pcbus_instance_t *)INSTANCE;
    rx_ptr = ds->rx_ptr;
    /*
     * Set buff to point to physical start of RX buffer. This is = 
     * start of Shared RAM + offset of RX buffer within Shared RAM.
     */
    ds->buff = (uchar *)((ulong)ds->ifaddr + rx_ptr->offset);

    if (pcbus_debug)
        buginf("\n%%%s: RX_int buff at 0x%x, GET = 0x%x, PUT = 0x%x, WRAP = 0x%x",
               idb->hw_namestring, ds->buff, rx_ptr->get_ptr, 
               rx_ptr->put_ptr, rx_ptr->wrap_ptr);

    /*
     * Receive all input frames:
     */
    rx_count = 0;
    while (rx_count < MAX_FIJI_RX_PACKET) {

next_RX_packet:

        /*
         * If GET pointer exceeds PUT pointer, compare it with the
         * WRAP pointer. If it equals or exceeds the WRAP pointer
         * too, then wrap it to start of RX buffer.
         */
        if (rx_ptr->get_ptr > rx_ptr->put_ptr)
            if (rx_ptr->get_ptr >= rx_ptr->wrap_ptr)
                rx_ptr->get_ptr = 0;  /* wrap GET pointer */
        if (rx_ptr->get_ptr == rx_ptr->put_ptr) {
            /* RX buffer empty. Update GET ptr & return */
            ds->ifaddr->rx_buff.get_ptr = rx_ptr->put_ptr;
            reset_interrupt_level(SR);
            return;
        }
        /*
         * Get start of frame in Shared RAM. This will be equal to
         * start address of Shared RAM + start of RX buffer + GET pointer.
         */
        datagramstart = (uchar *)((ulong)ds->buff + 
                                  (ulong)rx_ptr->get_ptr);
        input_packet = (pcbus_frame_t *)datagramstart;
        /*
         * Increment GET pointer and align to long word boundary.
         */
        rx_ptr->get_ptr += ((input_packet->frm_len + PCBUS_FRAME_INC) &
			    0xFFFC);
        prio = serial_high_priority(input_packet->pcbus_hdlctype);

        /*
         * Check to make sure it has a non zero frame length
         * before we process it as a valid packet.
         */
        if (!input_packet->frm_len) {
            idb->counters.input_drops++;
            continue;  /* drop packet */
        }

	/*
	 * If we have a scheduled run interval configured, go see if we need
	 * to drop packets to give processes a chance to run:
	 */
        if (!prio) {  /* Not a high priority packet */
            if (!idb_is_throttled(idb))  /* input not throttled */
	        if (sched_run_interval && AWAKE(sched_run_time))
	            /*
	             * The scheduler has not run recently enough,
	             * so throttle the PC bus:
	             */
	            pcbus_throttle(idb);
            if (idb_is_throttled(idb)) {
	        idb->counters.input_drops++;  
                continue;  /* drop packet */
                rx_count++;
            }
        }
 
	/*
	 * Attempt to get a new packet buffer to copy in the 
	 * received packet.
	 */
	pak = pcbus_getbuf(idb);
    	if (!pak) { /* Couldn't get buffer. */
	    if (prio) { /* Try harder if high priority packet */
		pak = getbuffer(input_packet->frm_len);
                if (pak) {
                    pak->if_input = swidb;
                    pak->datagramstart = (uchar *)(pak->data_area + 
		                                   idb->start_offset);
                }
            }
        }

        /*
         * Increment processed packet count
         */
        rx_count++;

        if (pak) {
            /*
             * Copy packet from Shared RAM Rx buffer.
             */
            pak->datagramsize = input_packet->frm_len;  /* set packet size */
            bcopy((uchar *)&(input_packet->pcbus_station), pak->datagramstart,
		  input_packet->frm_len);

#ifdef PAKMON_ENABLED
	    if (idb->pakmon_enabled)
	        parse_serial(pak, idb->number, idb->enctype);
#endif

	    serial_RX_interrupt_inline(idb, pak);
	    NEXT_READ_PACKET;
	} else { 
	    /*
	     * We could not get a packet buffer, so drop the packet.
	     */
	    idb->counters.input_drops++;
	    pcbus_throttle(idb);
	}
    } /* end while loop */
    reset_interrupt_level(SR);
}


/*
 * pcbus_fastsend
 * Attempt to transmit this packet immediately.
 * Called from interrupt level.
 */
static void pcbus_fastsend (hwidbtype *idb, paktype *pak)
{
    pcbus_instance_t *ds;
    pcbus_frame_t *outpak;
    pc_if_ptrs_t *tx_ptr;
    ushort bytes, free, ringbytes;
    idbtype *swidb = idb->firstsw;

    bytes = pak->datagramsize;

    ds = (pcbus_instance_t *)INSTANCE;
    tx_ptr = ds->tx_ptr;
    /*
     * Set buff to physical start of TX buffer. This would be =
     * start of Shared RAM + offset of TX buffer within Shared RAM.
     */
    ds->buff = (uchar *)((ulong)ds->ifaddr + tx_ptr->offset);

    if (pcbus_debug)
        buginf("\n%%%s: fast_send buff at 0x%x, GET = 0x%x, PUT = 0x%x, WRAP = 0x%x", 
               idb->hw_namestring, ds->buff, tx_ptr->get_ptr, 
               tx_ptr->put_ptr, tx_ptr->wrap_ptr);

    /*
     * If no packets are queued for output in the hold Q on this 
     * interface, immediately transmit the packet:
     */
    if (!idb->output_qcount) {
	
	/*
	 * Is there room for another packet in the Tx buffer ?
	 */
        if (tx_ptr->put_ptr >= tx_ptr->get_ptr)
            free = tx_ptr->size - tx_ptr->put_ptr;
        else 
            free = tx_ptr->get_ptr - tx_ptr->put_ptr;
          
	/*
	 * Calculate the amount of space required on the TX ring, including
	 * the header and all rounding bytes
	 */
	ringbytes = (bytes + PCBUS_FRAME_INC) & 0xFFFC;

        /*
         * See if a worst case descriptor fits within free space.
         */
        if (free <= ringbytes) {  /* Not enough space */
            /* See if we can wrap around */
            if ((tx_ptr->get_ptr > ringbytes) && 
                (tx_ptr->put_ptr >= tx_ptr->get_ptr)) { 
                /* free space available */
                tx_ptr->wrap_ptr = tx_ptr->put_ptr;
                tx_ptr->put_ptr = 0;
            } else {  /* Sorry, no space */
		pak->if_output = swidb;
		holdq_enqueue_fast(idb, pak);
                ds->ifaddr->tx_buff.oflow = ++ds->tx_ptr->oflow;
                return;
            }
        }

        outpak = (pcbus_frame_t *)((ulong)ds->buff + tx_ptr->put_ptr);
        bcopy(pak->datagramstart, (uchar *)&outpak->pcbus_station, bytes);
        outpak->frm_len = bytes;
        tx_ptr->put_ptr += ringbytes;

        /*
         * Now update wrap pointer followed by the PUT ptr.
         */
        ds->ifaddr->tx_buff.wrap_ptr = tx_ptr->wrap_ptr;
        ds->ifaddr->tx_buff.put_ptr = tx_ptr->put_ptr;

        /*
         * Now pulse the interrupt bit to kick the Host in the butt.
         */
        *(ds->regaddr) = FIJI_PC_INTERRUPT;
        *(ds->regaddr) = 0;
	
	/* Do packet accounting after the kick.
	 */
	ds->tx_count++;
	idb->counters.tx_cumbytes_fast[pak->acct_proto] += bytes;
	idb->counters.outputs_fast[pak->acct_proto]++;
	TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	if ((idb->enctype == ET_HDLC) &&
	    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
	    idb->counters.output_broad++;
	datagram_done(pak);
	return;

    } else {  /* Something in the hold Q */

	/*
	 * There are already packets on the output hold queue,
	 * so add this one to the end of the queue to minimize
	 * out of order situations:
	 */
	pak->if_output = swidb;
	holdq_enqueue_fast(idb, pak);
	return;
    }
}

/*
 * Start or restart output.
 */
static void pcbus_tx_start (hwidbtype *idb)
{
    pcbus_instance_t *ds = (pcbus_instance_t *)INSTANCE;
    pcbus_frame_t *outpak;
    pc_if_ptrs_t *tx_ptr;
    int bytes, ringbytes, free;
    paktype *pak;
    leveltype SR;
    volatile ushort txtype;
    
    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     * While there is another packet to output and room for it in the
     * PC bus TX buffer, put it in the buffer.
     */

    tx_ptr = ds->tx_ptr;
    ds->buff = (uchar *)((ulong)ds->ifaddr + tx_ptr->offset);

    if (pcbus_debug)
        buginf("\n%%%s: TX_start buff at 0x%x, GET = 0x%x, PUT = 0x%x, WRAP = 0x%x", 
               idb->hw_namestring, ds->buff, tx_ptr->get_ptr, 
               tx_ptr->put_ptr, tx_ptr->wrap_ptr);

    while (TRUE) { 
        /*
         * Compute free space in Tx buffer.
         */
        if (tx_ptr->put_ptr >= tx_ptr->get_ptr)
            free = tx_ptr->size - tx_ptr->put_ptr;
        else 
            free = tx_ptr->get_ptr - tx_ptr->put_ptr;
          
	pak = (*idb->oqueue_dequeue)(idb);
        if (pak) {

	    bytes = pak->datagramsize;

	    if (bytes > idb->max_pak_size) {
	        errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	        datagram_done(pak);
	        continue;
	    }

	    txtype = hdlc_decode_encapsulation_inline(pak,
						      (vhdlctype *)pak->datagramstart);
	    if (txtype == RXTYPE_DODIP)
		txtype = pak->datagramstart[13];

	    /*
	     * Calculate the amount of space required on the TX ring, including
	     * the header and all rounding bytes
	     */
	    ringbytes = (bytes + PCBUS_FRAME_INC) & 0xFFFC;

            if (free <= ringbytes) {  /* not enough space */
                /* See if we can wrap around */
                if ((tx_ptr->get_ptr > ringbytes) && 
                    (tx_ptr->put_ptr >= tx_ptr->get_ptr)) { 
                    /* free space available */
                    tx_ptr->wrap_ptr = tx_ptr->put_ptr;
                    tx_ptr->put_ptr = 0;
                } else {  /* Sorry, no space */
                    /*
                     * Should we pitch packet or put it back in
                     * the hold Q ?
                     */
                    ds->ifaddr->tx_buff.oflow = ++ds->tx_ptr->oflow;
	            datagram_done(pak);
                    break;  /* ???? */
                }
            }

            outpak = (pcbus_frame_t *)((ulong)ds->buff + tx_ptr->put_ptr);
            bcopy(pak->datagramstart, (uchar *)&outpak->pcbus_station, bytes);
            outpak->frm_len = bytes;
            tx_ptr->put_ptr += ringbytes;

	    idb->counters.tx_cumbytes += bytes;
	    TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	    idb->counters.outputs++;

	    if ((pak->enctype == ET_HDLC) &&
		(outpak->pcbus_station & HDLC_BCAST))
	        idb->counters.output_broad++;

	    datagram_done(pak);  /* Free the packet */
        }
        else
            break;
    }
    /*
     * Update pointers in control block.
     */
    ds->ifaddr->tx_buff.wrap_ptr = tx_ptr->wrap_ptr;
    ds->ifaddr->tx_buff.put_ptr = tx_ptr->put_ptr;

    /*
     * Now pulse the interrupt bit to give the Host a kick.
     */
    *(ds->regaddr) = FIJI_PC_INTERRUPT;
    *(ds->regaddr) = 0;

    reset_interrupt_level(SR);	/* Restore interrupt level */
}


/*
 * pcbus_enable
 * Enable a PC bus interface, most likely shut down for input congestion.
 * Please note that this function must be called with LEVEL2_DISABLED to 
 * work correctly.
 */
static void pcbus_enable (hwidbtype *idb)
{
    if (idb->type == IDBTYPE_PCBUS)
	idb_mark_unthrottled(idb);
}


/*
 * Show PC bus controller status:
 */
static boolean show_pcbus (hwidbtype *idb, parseinfo *csb)
{
    pcbus_instance_t *ds;
    pc_if_ptrs_t *buf;
    ushort sts;
    idbtype *swidb = idb->firstsw;
    
    ds = (pcbus_instance_t *)INSTANCE;
    if (!ds) {
#ifdef DEBUG
        buginf("\n%%%s: ### driver struct is NULL!", idb->hw_namestring);
#endif
        return(FALSE);
    }

    if (idb->type != IDBTYPE_PCBUS) {
	buginf("\n%%%s: Bad IDB type = 0x%x", idb->hw_namestring, idb->type);
	return(FALSE);
    }

    printf("PCbus unit %d,  Name = %s  ", idb->unit, idb->hw_namestring);

    printf("Hardware is ISA PCbus shared RAM\n");
    printf("IDB at %#x,  Interface driver data structure at %#x\n",
            idb, ds);
    printf("Control/status register at %#x,  Shared memory at %#x\n",
           ds->regaddr, ds->ifaddr);
    printf("Shared memory is %sinitialized\n", 
            (sts = FIJI_SHRAM_STATUS) ? "" : "NOT ");

    if (sts) {
    printf("\nShared memory interface control block :\n");
    printf("Magic no = 0x%x (%s)  Version = %d.%d\n",
           ds->ifaddr->magic, 
           (ds->ifaddr->magic == PCBUS_SHRAM_MAGIC ? "valid" : "INVALID"),
           ds->ifaddr->ver_major, ds->ifaddr->ver_minor);
    printf("Shared memory size = %dK bytes,  Interface is %s\n",
           ds->ifaddr->shram_size / 1024, 
           (ds->ifaddr->state == PCBUS_SHUTDOWN) ? "shutdown" : "NOT shutdown");
    printf("Interface state is %s, ", print_idbstate(swidb));
    printf("line protocol is %s\n", interface_up(swidb) ? "up" : "down");

    buf = (pc_if_ptrs_t *)&(ds->ifaddr->tx_buff);
    printf("\nTx buffer : (control block at 0x%x)\n", buf);
    printf("Start offset = 0x%x,  Size = 0x%x,  Overflows = %d\n", 
           buf->offset, buf->size, buf->oflow);
    printf("GET_ptr = 0x%x,  PUT_ptr = 0x%x,  WRAP_ptr = 0x%x\n",
            buf->get_ptr, buf->put_ptr, buf->wrap_ptr);

    buf = (pc_if_ptrs_t *)&(ds->ifaddr->rx_buff);
    printf("\nRx buffer : (control block at 0x%x)\n", buf);
    printf("Start offset = 0x%x,  Size 0x%x,  Overflows = %d\n", 
           buf->offset, buf->size, buf->oflow);
    printf("GET_ptr = 0x%x,  PUT_ptr = 0x%x,  WRAP_ptr = 0x%x\n",
            buf->get_ptr, buf->put_ptr, buf->wrap_ptr);
    }

    printf("\nInterrupts received = %d\n", ds->intr_cnt);

    return(FALSE);
}


/*
 * Service the FIJI SLARP special request
 */
static boolean pcbus_slarp_special_request (paktype *pak)
{
    struct slarp *ptr;
    slarp_sp_reply_t *sp_ptr;
    idbtype *idb;
    hwidbtype *hwidb, *tmpidb;
    srb_vector_table_t *srbV_p;

    ptr = (struct slarp *) (pak->data_area + ENCAPBYTES);
    idb = pak->if_input;
    hwidb = idb->hwptr;
    sp_ptr = (slarp_sp_reply_t *)ptr;
    
    FOR_ALL_HWIDBS(tmpidb) {
        if (tmpidb->status & IDB_ETHER) {
            sp_ptr->type = 2;
            sp_ptr->speed = 10;
            sp_ptr->LNM_support = 0;
            break;
        }
        else if (tmpidb->status & IDB_TR) {
            sp_ptr->type = 1;
            sp_ptr->speed = tmpidb->tr_ring_speed;
            if (pcbus_debug)
                buginf("(%s): TR ring speed = %d\n",
                       tmpidb->hw_namestring, sp_ptr->speed);
            /*
             * Set up the local lnm related fields if the feature
             * is available and enabled.
             */
            if (!reg_invoke_local_lnm_slarp((uchar *)sp_ptr, hwidb, tmpidb)) {
                sp_ptr->LNM_support = 0;
            }
            srbV_p = reg_invoke_srb_get_vector_table();
            if (srbV_p) {
                sp_ptr->tr_actualring = srbV_p->srb_sb_get_actualring(tmpidb->firstsw);
            } else {
                sp_ptr->tr_actualring = SRB_INVALID_RING_NUM;
            }
            break;
        }
    }
    
    if (!tmpidb)  /* Ethernet or TR idb not found */
	return(FALSE);
    
    sp_ptr->code = SLARP_SPECIAL_REPLY;
    
    /*
     * For Ethernet or TR interface
     */
    ieee_copy(tmpidb->hardware, sp_ptr->address);
    sp_ptr->status = ((tmpidb->state == IDBS_UP) ? 0 : 1);
    
    /*
     * for PC bus interface
     */
    sp_ptr->keep_period = (hwidb->nokeepalive ? 0 : hwidb->keep_period);
    sp_ptr->mtu = hwidb->maxmtu;
    
    /* PC bus IP address */
    sp_ptr->ip_address = idb->ip_address;
    if (idb->ip_address) {
	sp_ptr->ip_netmask = idb->ip_nets_mask;
    } else {
	sp_ptr->ip_netmask = 0;
    }
    
    pak->if_output = idb;
    pak->linktype = LINK_ADDRESS;
    
    if (pcbus_debug)
	buginf("\n%%%s: SPL_GET_STATUS request seen, sending status",
	       hwidb->hw_namestring);
    
    if (pak_encap(hwidb, pak, sizeof(slarp_sp_reply_t), 0)) {
	datagram_out(pak);
	return(TRUE);
    } 
    
    return(FALSE);
}


/*
 * Initialize interface.
 */
static void pcbus_init (hwidbtype *idb, boolean maxdgram_change)
{
    pcbus_instance_t *ds;
    leveltype SR;
    int rx_ring_size, tx_ring_size;
    pooltype *old_pool;

    idbtype *swidb = idb->firstsw;

    SR = raise_interrupt_level(LEVEL2_DISABLE);  /* disable interrupts */
    ds = (pcbus_instance_t *)INSTANCE;

    ds->status = FIJI_SHRAM_STATUS;  /* Shared RAM control block status */
 
    /*
     * Initialize the interrupt bit to 0.
     */
    *(ds->regaddr) = 0;

    if (idb->state != IDBS_ADMINDOWN) {
	/*
	 * If we are not shutdown, show that we are being reset:
	 */
	net_cstate(idb, IDBS_RESET);
    }

    /*
     * Flush the hold queue and any packet waiting for output.
     */
    holdq_reset(idb);

    /*
     * If interface is administratively down, and the PC Host has 
     * initialized the Shared RAM control block, let the PC Host know 
     * about the shutdown state by writing the SHUTDOWN-state magic 
     * value in the control block.
     */
    if (idb->state == IDBS_ADMINDOWN) {
        pcbus_stop(idb); /* write magic value into state field */
	reset_interrupt_level(SR);
	return;
    }

    /*
     * the call to select_ring_size is used only to determine the
     * appropriate size of the pool and cache.  There are no rings. 
     */

    select_ring_size(&rx_ring_size, &tx_ring_size, idb->max_buffer_size); 

    /*
     * If a private buffer pool for this interface exists AND there
     * has been an MTU change, discard the pool with the old-sized
     * buffers. 
     */
    
    if (idb->pool && maxdgram_change) {
      old_pool = idb->pool;
      pool_destroy(old_pool);
      idb->pool = NULL;
    }

    /* 
     * If the private buffer pool has not been allocated yet, or if
     * it was destroyed due to an MTU change above, allocate it now.
     * Populate the pool, initialize a private buffer cache,
     * and populate the buffer cache with buffers from the newly
     * created private pool.  Also set up the fallback public buffer
     * pool. 
     */
    
    if (!idb->pool) {
	if (idb->pool_group == POOL_GROUP_INVALID)
	    idb->pool_group = pool_create_group();

	if (!idb->hw_namestring)
	    idb_init_names(idb->firstsw, FALSE);

	idb->pool = pak_pool_create(idb->hw_namestring, idb->pool_group,
					idb->max_buffer_size,
					POOL_SANITY,
					NULL); 
	if (!idb->pool) {
	    crashdump(0);
	}
	idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
	pool_adjust(idb->pool, 0, PRIV_NUM_BUF(rx_ring_size),
		    PRIV_NUM_BUF(rx_ring_size), TRUE); 
	pak_pool_create_cache(idb->pool, PRIV_NUM_CACHE(rx_ring_size)); 
	pool_adjust_cache(idb->pool, PRIV_NUM_CACHE(rx_ring_size));
    }

    /*
     * Are we bridging?
     */
     if (bridging_on_this_interface(swidb)) {
        reg_invoke_media_interesting_addresses(swidb);
     }

    /*
     * If Shared RAM is not initialized, mark interface as down and return.
     */
    if (ds->status == 0) {  /* Shared RAM control block not yet initialized */
        idb->state = IDBS_DOWN;
        reg_invoke_if_pcbus_down(idb);
        reset_interrupt_level(SR);
        return;
    }

    /*
     * Interface is supposed to be UP. Validate the control block.
     * Ensure Shared RAM is accessible and in healthy state.
     */
    if (validate_pcbus_inline(idb, ds, TRUE)) {
        net_cstate(idb, IDBS_UP);
        ds->ifaddr->state = ~PCBUS_SHUTDOWN;
    }
    else {
        net_cstate(idb, IDBS_DOWN);
        reg_invoke_if_pcbus_down(idb);
    }
        
    reset_interrupt_level(SR);		/* Restore interrupt level */
}


/*
 * Set the maximum datagramsize for this interface and then
 * reset the interface to use it.  Don't bother using buffersize - it
 * is used for the mci/cbus routines.
 * Return TRUE if the datagram size was changed, else return FALSE.
 */
static boolean pcbus_set_maxdgram (hwidbtype *idb, int buffersize,
				   int maxdgram)
{
    if (!(idb->type == IDBTYPE_PCBUS)) {
	buginf(set_maxdgram_failed, maxdgram, idb->hw_namestring);
	return(FALSE);
    }

    if_maxdgram_adjust(idb, maxdgram);

    pcbus_init(idb, TRUE);

    return(TRUE);
}


/*
 * Wrapper routine for idb->reset vector:
 */
static void pcbus_restart (hwidbtype *idb)
{
    pcbus_init(idb, FALSE);
}


/*
 * pcbus_shutdown
 * Shut down a PC bus interface.
 * Set idb->state and call pcbus_init() to do the needfull.
 */
static void pcbus_shutdown (hwidbtype *idb, int newstate)
{
    net_cstate(idb, newstate);
    pcbus_init(idb, FALSE);
}


/*
 * Set up for bridging on this interface:
 */
static void pcbus_setup_bridging (hwidbtype *idb)
{
    pcbus_init(idb, FALSE);	/* Reset code will set it up */
    if (idb->counters.resets > 0)
        idb->counters.resets--;
}


static void pcbus_instance_init (hwidbtype *idb, pcbus_instance_t *ds)
{
    ds->status     = FIJI_SHRAM_STATUS; /* Shared RAM control block status */
    ds->tx_count   = 0;                 /* Count of packets in Tx buffer */
    idb->unknown_encaps = 0;
    ds->rx_ptr     = (pc_if_ptrs_t *)&(ds->c_blk.rx_buff);
    ds->tx_ptr     = (pc_if_ptrs_t *)&(ds->c_blk.tx_buff);
    ds->intr_cnt   = 0;

    /*
     * Setup start_offset
     */
    switch (idb->enctype) {
      case ET_HDLC:
	idb->start_offset = ENCAPBYTES - HDLC_ENCAPBYTES;
	break;
      default:
	errmsg(&msgsym(BOGUSENCAP, LINK), idb->hw_namestring, idb->enctype);
	idb->enctype = ET_HDLC;
	idb->start_offset = ENCAPBYTES - HDLC_ENCAPBYTES;
    }

    /*
     * Adjust the datagramstart address to a longword boundary
     * to speed up packet copies.
     */
    if (idb->start_offset & 3) {
	if (pcbus_debug)
	    buginf("\n%%%s: Changing start_offset from 0x%x to ", 
                    idb->hw_namestring, idb->start_offset);
	idb->start_offset = (idb->start_offset & 0xFFFFFFFC);
	if (pcbus_debug)
	    buginf("0x%x", idb->start_offset);
    }
}


/*
 * Common IDB initialization routine:
 */
static void pcbus_init_idb_common (hwidbtype *idb)
{
    pcbus_instance_t *ds;
    uchar *ds_base;
    idbtype *swidb = idb->firstsw;

    idb->status = IDB_SERIAL|IDB_PCBUS|IDB_BCAST|IDB_BRIDGE;
    idb->type = IDBTYPE_PCBUS;
    idb->typestring = "ISA PCbus shared RAM";
    idb->name = IFNAME_PCBUS;
    idb->hw_namestring = "PCbus0";
    idb->state = IDBS_INIT;
    GET_TIMESTAMP(idb->state_time);
    idb->error_threshold = 5000;
    idb->error_count = 0;
    idb->fast_switch_flag = 0;
    idb->input_qcount = 0;	/* make input_getbuffer succeed */
    idb->input_defqcount = INPUTQ_DEFAULT;
    
    /*
     * Set up function calls
     */
    serial_idb_init(idb, MAXSERIALSIZE, PAN_MAXDGRAM,
		    VISIBLE_LES_SERIAL_BANDWIDTH, LES_SERIAL_DELAY);

    if_maxdgram_adjust(idb, PAN_MAXDGRAM);

    idb->soutput = pcbus_tx_start;
    idb->reset = pcbus_restart;
    idb->shutdown = pcbus_shutdown;
    idb->enable = pcbus_enable;
    idb->setup_bridging = pcbus_setup_bridging;
    idb->device_periodic = NULL;
    swidb->span_disabled = FALSE;
    idb->fastsend = pcbus_fastsend;
    idb->show_controller = show_pcbus; 
    
    /*
     * Set up how arping is done.  This isn't used for
     * serial links but is here for consistency.
     */
    swidb->arp_arpa = swidb->arp_arpa_def = FALSE;
    swidb->arp_probe = swidb->arp_probe_def = FALSE;
    swidb->arp_snap = swidb->arp_snap_def = FALSE;

    /*
     * idb->set_maxdgram must be initialized after calling serial_idb_init()
     * so that it does not attempt to call pcbus_set_maxdgram()
     * while doing system init
     */
    idb->set_maxdgram = pcbus_set_maxdgram;
    
    idb->reliability = 255;
    idb->load = 1;
    idb->rxload = 1;
    idb->pool = NULL;
    idb->pool_group = POOL_GROUP_INVALID;

    /*
     * Local LNM variable initialization
     */
    idb->local_lnm_enabled = FALSE;
    idb->lrm_session_est = FALSE;

    serial_idb_enqueue(idb);
    idb_enqueue(idb);

    /*
     * Allocate this instance of the driver:
     */
    ds_base = malloc_named(PCBUS_DS_SIZE, "PCBus Instance");
    if (!ds_base) {
	crashdump(0);
    } else {
        ds = (pcbus_instance_t *)ds_base;
	INSTANCE = ds;

        /* 
         * Find the register address for the Shared RAM, and save the
         * IDB ptr for level 2 int wrapper. 
         */
        ds->regaddr = (ushort *)ADRSPC_FIJI_CONTROL;
        ds->ifaddr = (shram_block_t *)ADRSPC_FIJI_SHRAM;
        network_c_idb = idb;	

        if (pcbus_debug)
            buginf("\n%%%s: I/f addr = 0x%x, Reg addr = 0x%x, idb = 0x%x", 
                   idb->hw_namestring, ds->ifaddr, ds->regaddr, idb);
 
        pcbus_instance_init(idb, ds);
        if (pcbus_debug)
            buginf("\n%%%s: idb at 0x%x, ds = 0x%x", idb->hw_namestring, idb,
		   ds);
    }
}


/*
 * init_pcbus_idb -- set up an idb for a pcbus interface
 */
static void init_pcbus_idb (int which)	/* Unit number */
{
    hwidbtype *idb;

    /*
     * Set up storage, add IDB to interface queue:
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    idb->unit = which;

    /*
     * Initialize the IDB, then the interface.
     */
    pcbus_init_idb_common(idb);
    pcbus_init(idb, FALSE);
    
    idb->counters.resets = 0;		/* Reset count */
}


/*
 * local_lnm_if_pcbus_with_tr
 *
 * Check to see if it is a PCbus and Token Ring interface platform.
 */
static boolean local_lnm_if_pcbus_with_tr (void)
{
    hwidbtype *tmpidb;
    list_element *l_elt;

    FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_TOKEN, l_elt, tmpidb) {
	return(TRUE);
    }

    return (FALSE);
}


/*
 * PC bus interrupt
 */
static void pcbus_interrupt (void)
{
    pcbus_instance_t *ds;
    hwidbtype *idb;

    idb = network_c_idb;

    if (!(idb->type == IDBTYPE_PCBUS)) {
	buginf("\n%%%s: interrupt not for PC bus idb!",
		idb->hw_namestring);
	return;
    }

    ds = (pcbus_instance_t *)INSTANCE;
    if (!ds) {
        buginf("\n%%%s: ### driver struct is NULL!", idb->hw_namestring);
        return;
    }

    ds->intr_cnt++;  /* increment interrupt count */

    ds->status = FIJI_SHRAM_STATUS;
    if (!ds->status) {  /* The Shared RAM flag may have been reset */
        if (pcbus_debug)
            buginf("\n%%%s: interrupt - Shared RAM flag reset.",
		   idb->hw_namestring);
        if (idb->state != IDBS_ADMINDOWN) {
            idb->state = IDBS_DOWN;
            reg_invoke_if_pcbus_down(idb);
        }
        return;
    }

    /*
     * Shared RAM flag is set. Check Shared RAM block magic number and
     * validate Tx buffer pointers. If anything bad, destroy
     * the Shared RAM block magic number.
     */
    if (!validate_pcbus_inline(idb, ds, FALSE)) { 
        if (pcbus_debug)
            buginf("\n%%%s: interrupt - Shared RAM validation failed.", 
                    idb->hw_namestring);
        /* Shared RAM control block validation failed */
        if (idb->state != IDBS_ADMINDOWN) {
            idb->state = IDBS_DOWN;
            reg_invoke_if_pcbus_down(idb);
        }
        return;
    }

    /*
     * Shared RAM flag is set and Shared RAM block magic number is good.
     * Tx buffer pointers are also good.
     */
    if (idb->state != IDBS_ADMINDOWN) {
        idb->state = IDBS_UP;
        ds->ifaddr->state = ~PCBUS_SHUTDOWN;
    }
    else {
        ds->ifaddr->state = PCBUS_SHUTDOWN;
        return;   /* interface is shut down */
    }

    /*
     * Shared RAM flag set. Shared RAM block magic # good. Tx buffer OK.
     * Interface UP. So what's left ?
     * See if Rx buffer has any frame(s), or if we need to Tx any.
     */
    pcbus_RX_interrupt(idb);
    pcbus_tx_start(idb);
}


/*********************************************************************
*    analyze_pcbus()
*
*    This function reads the cookie to figure out if a PC ISA bus  
*    interface is present. It sets up the level 2 interrupt and
*    initializes the interface. It sets the number of interfaces
*    in npcbusnets.
*********************************************************************/
static void analyze_pcbus (void)
{
    /*
     *The PC card is a cancun derivative
     */
    if (cpu_type == CPU_CANCUN) {
        if ((cookie->interfaces == INTERFACE_1E1T1I) ||
            (cookie->interfaces == INTERFACE_1R1T1I)) {
                /*
                 * Initialize the level2 interrupts to point at the
		 * pcbus interrupt handler
                 */
                createlevel(PCBUS_INTLEVEL, pcbus_interrupt,
                            "PC bus interrupt");

                /*
                 * Attempt to start this interface
                 */
                init_pcbus_idb(npcbusnets++);

                return;
           }
    }
}


/*
 * PCBUS subsystem initialisation entry point
 */
static void pcbus_subsys_init (subsystype *subsys)
{
    /*
     * Discover all the PCBUS interfaces in system
     */
    analyze_pcbus();

    /*
     * Now register the SLARP extended type service
     */
    reg_add_slarp_extended_type(SLARP_SPECIAL_REQUEST,
				pcbus_slarp_special_request,
			  	"pcbus_slarp_special_request");

    /*
     * Now register the PCbus w/ TR check for local_lnm feature
     */
    reg_add_if_pcbus_with_tr(local_lnm_if_pcbus_with_tr,
                             "local_lnm_if_pcbus_with_tr");

    /*
     * Set up the debugging flags
     */
    pcbus_debug_init();
    
    /*
     * Set up the parser stuff 
     */
    pcbus_parser_init();

}

#define PCBUS_MAJVERSION   1
#define PCBUS_MINVERSION   0
#define PCBUS_EDITVERSION  1

SUBSYS_HEADER(pcbus,
              PCBUS_MAJVERSION, PCBUS_MINVERSION, PCBUS_EDITVERSION,
              pcbus_subsys_init, SUBSYS_CLASS_DRIVER,
              NULL,
              NULL);

