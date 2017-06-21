/* $Id: if_c3000_bri.c,v 3.9.12.14 1996/08/28 12:57:23 thille Exp $ 
 * $Source: /release/112/cvs/Xsys/les/if_c3000_bri.c,v $
 *------------------------------------------------------------------
 * if_c3000_bri.c  MC68302 and TP3420 drivers for ISDN Moosehead
 *
 * March 1994, Manoj Leelanivas.
 * Based on "if_les_bri.c", November 1991, Greg Stovall
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: if_c3000_bri.c,v $
 * Revision 3.9.12.14  1996/08/28  12:57:23  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.9.12.13  1996/08/13  02:19:47  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.9.12.12  1996/07/20  03:19:46  fred
 * CSCdi63607:  add access list to traffic shaping per customer request
 *              requires change of parameters on function called by
 *              fastsend routines in drivers
 * Branch: California_branch
 *
 * Revision 3.9.12.11  1996/07/17  18:30:37  etrehus
 * CSCdi62203:  D-channel collision race condition
 * Branch: California_branch
 *
 * Revision 3.9.12.10  1996/06/22  06:58:37  hrobison
 * CSCdi61029:  D channel collision count is not displayed
 * Branch: California_branch
 *
 * Revision 3.9.12.9  1996/06/19  23:06:08  etrehus
 * Handle D-Channel Collisions by retransmitting packet
 *
 * Revision 3.9.12.8  1996/06/17  17:33:11  wmay
 * CSCdi60601:  show controller bri crashes box - only allow for d-chans
 * Branch: California_branch
 *
 * Revision 3.9.12.7  1996/06/13  19:07:40  etrehus
 * Commit boa to California Branch
 *
 * Revision 3.9.12.6  1996/06/12  16:50:40  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.9.12.5  1996/05/10  01:34:11  wmay
 * CPP and ppp half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.9.12.4  1996/04/30  18:38:07  etrehus
 * CSCdi51546:  Need warning for non-isdn image on platforms with bri
 * Branch: California_branch Committed.
 *
 * Revision 3.9.12.3  1996/04/22  21:51:40  gtaylor
 * CSCdi54554:  Bridging over isdn leased-line broken for HDLC
 * encapsulation.
 * Branch: California_branch
 *
 * Revision 3.9.12.2  1996/04/17  13:43:32  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.9.12.1  1996/03/18  20:41:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.5  1996/03/16  07:11:49  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.5.2.4  1996/03/13  01:48:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.5.2.3  1996/03/09  05:07:19  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.5.2.2  1996/02/20  01:08:14  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/15  18:02:19  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.8  1996/02/13  22:10:48  fred
 * CSCdi47956:  WFQ severely degrades RSRB performance
 *              drivers: increase TQL in short queue mode for link
 *              stability
 *              fastsrb_les.c: make sure pointers are there for queuing
 *              routines
 *
 * Revision 3.7  1996/02/13  20:25:46  hampton
 * Move platform specific defines from common include files to platform
 * specific include files.  [CSCdi48844]
 *
 * Revision 3.6  1996/01/23  22:48:00  jhernand
 * CSCdi44262:  2500 BRI sends 0xFF as flags instead of 0x7e
 * Changed to send 0x7E for B Channels only. D Channel must send 0xFF.
 *
 * Revision 3.5.2.1  1996/01/24  22:22:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.5  1996/01/19  22:15:35  wmay
 * CSCdi45344:  ACL: input access lists are ignored on SMDS subinterfaces
 * Add extra include file so serial_inline is happy
 *
 * Revision 3.4  1996/01/05  01:02:02  sluong
 * CSCdi42391:  Configuring a protocol to be routed brings down B-channel
 *
 * Revision 3.3  1995/11/17  17:38:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:19  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  21:14:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/08/08  19:47:19  fred
 * CSCdi38414:  congestion of compressed interfaces reduces speed.
 * when compressing data AND using WFQ/PQ/CQ, use a ring size of two, not
 * one.
 *
 * Revision 2.8  1995/08/07  00:28:39  ssangiah
 * CSCdi38231:  Reserve cookie value for new development
 * Knowledge of this cookie value for the new platform has also been added
 * to the init code for the appropriate drivers so that the boot ROMs from
 * this release will initialize the corresponding interfaces on the
 * platform properly.
 *
 * Revision 2.7  1995/08/04  16:10:54  dclare
 * CSCdi38254:  ISDN BRI Layer 1 fails to come up after shut/no shut
 * sequence. Make sure it gets activated.
 *
 * Revision 2.6  1995/07/05  22:17:27  gtaylor
 * CSCdi34592:  PPP Reliable Link broken. Changed lapb's attachment to PPP
 * from taking over and saving the idb->iqueue to taking over the
 * parse_packet vector instead. This also fixes bridging problems with
 * reliable link.
 *
 * Revision 2.5  1995/06/23  18:48:15  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.4  1995/06/23  16:39:39  fred
 * CSCdi36295:  Commit Queue MIB to 11.0
 *
 * Revision 2.3  1995/06/21  03:00:56  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.2  1995/06/19  06:46:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  21:32:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *-----------------------------------------------------------------------
 * $Endlog$ 
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "interface_private.h"
#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../wan/serial.h"
#include "../wan/serial_private.h"
#include "../wan/ppp.h"
#include "../if/ether.h"
#include "../wan/smds.h"
#include "logger.h"
#include "../if/bri_debug.h"
#include "if_les.h"
#include "isdn_bri.h"
#include "if_les_bri.h"
#include "if_c3000_bri.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../os/pool.h"
#include "../os/buffers.h"
#include "../wan/atm_dxi.h"
#include "media_registry.h"
#include "../if/serial_inline.h"
#include "../wan/serial_parser.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../if/traffic_shape.h"

/*
 * External declarations:
 */
extern SWITCH_TYPE SwitchType;

enum activation activated = DEACTIVATED;
static int loopback = FALSE;

/*
 * static declarations
 */
static boolean isdn_process_up = FALSE;		/* flag indicating ISDN
						   process is up */
static void bri_system_configured(hwidbtype *); /* set ISDN cstate, etc. */


/*
 * write_sid()
 * Write to the TP3420 (SID)
 */
static uchar write_sid (mcc68302_system_t *system_ram, uint cont_func,
			uint last_sid)
{
   volatile internal_reg_t *mc68302_int_reg;        /* '302 Internal reg */
   volatile scc_internal_reg_t *mc68302_scc_int_reg;/* '302 SCC internal reg */
   volatile ushort *scp_bd;                         /* SCP buffer descriptor */
   uint count;                                      /* counter */


   /*
    * Set up the register pointers
    */
   mc68302_int_reg = &(system_ram->internal_reg);
   mc68302_scc_int_reg = &(system_ram->scc_internal_reg);
   scp_bd = (ushort *)MC68302_SCP_BD;

   /*
    * Flick the right pins to write the SID
    */
   *scp_bd = MC68302_SCP_BD_DONE | (uchar)cont_func;
   mc68302_int_reg->pio_padat = 0x0000; /* write PA12 pin */
   mc68302_scc_int_reg->spmode = MC68302_SPM_STR | MC68302_SPM_CI | 
     MC68302_SPM_EN;

   /*
    * Wait for the data transfer to complete
    */
   for (count = 200000; count > 0; count--){
      if (!(*scp_bd & MC68302_SCP_BD_DONE))
	 break;
      asm("nop");		/* Give the '302 a break */
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
   }

   if ((count == 0) && bri_debug)
      buginf("\nBRI: Write to SID timed out.\n");

   mc68302_int_reg->pio_padat = MC68302_PIO_PA12; /* write PA12 pin */

   if (bri_debug)
       buginf("\nBRI: write_sid: scp = %x, wrote = %x\n", *scp_bd, cont_func);

   if ((*scp_bd & 0xFF) == 0)
       return(last_sid & 0x0F);
   else
       return((uchar)*scp_bd & 0xFF);

}


/*
 * read_sid()
 * Read the status register from the SID
 */
static uchar read_sid (mcc68302_system_t *system_ram)
{
   volatile internal_reg_t *mc68302_int_reg;        /* '302 Internal reg */
   volatile scc_internal_reg_t *mc68302_scc_int_reg;/* '302 SCC internal reg */
   volatile ushort *scp_bd;                         /* SCP buffer descriptor */
   uint count;                                      /* counter */

   /*
    * Set up the register pointers
    */
   mc68302_int_reg = &(system_ram->internal_reg);
   mc68302_scc_int_reg = &(system_ram ->scc_internal_reg);
   scp_bd = (ushort *)MC68302_SCP_BD;

   /*
    * Flick the right pins to read the SID
    */
   *scp_bd = MC68302_SCP_BD_DONE | (uchar)TP3420_CRF_NOP;
   mc68302_int_reg->pio_padat = 0x0000; /* write PA12 pin */
   mc68302_scc_int_reg->spmode = MC68302_SPM_STR | MC68302_SPM_CI | 
     MC68302_SPM_EN;

   /*
    * Wait for the data transfer to complete
    */
   for (count = 200000; count > 0; count--){
      if (!(*scp_bd & MC68302_SCP_BD_DONE))
	 break;
      asm("nop");		/* Give the '302 a break */
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
      asm("nop");
   }

   if ((count == 0) && bri_debug)
      buginf("\nBRI: Read from SID timed out.\n");

   mc68302_int_reg->pio_padat = MC68302_PIO_PA12; /* write PA12 pin */

   return((uchar)*scp_bd & 0x0F);
}


/*
 * bri_reset()
 * Reset the BRI interface.
 */
static void bri_reset (hwidbtype *in_idb)
{
   bri_instance_t *ds;     /* Pointer to instance */
   hwidbtype *idb;         /* Pointer to idb */
   ushort control;         /* ADRSPC control bits */

   /* 
    * Set up idb and instance pointer 
    */
   idb = in_idb->isdn_info->d_chan_idb;
   ds = INSTANCE;

   /*
    * Gracefully take down the SID 
    */
   ds->last_sid = write_sid(ds->config_reg, TP3420_CRF_DR, ds->last_sid);
   ds->last_sid = write_sid(ds->config_reg, TP3420_CRF_PDN, ds->last_sid);

   /* 
    * Tickle the reset bit in the system control register. 
    */
   if (cpu_type == CPU_CRISTAL || cpu_type == CPU_CANCUN) {
       control = *(volatile u_short *)ADRSPC_RESET;
       ds->reset_mask = RESET_NETWORK_D;
       if (ds && ds->reset_mask) {
 	   *(volatile u_short *)ADRSPC_RESET = control | ds->reset_mask;
 	   asm("nop");
 	   asm("nop");
 	   *(volatile u_short *)ADRSPC_RESET = control & ~ds->reset_mask;
       }
   } else {
       control = *(volatile u_short *)ADRSPC_CONTROL;
       ds->reset_mask = CONTROL_RESET_NETWORK_B;
       if (ds && ds->reset_mask) {
	   *(volatile u_short *)ADRSPC_CONTROL = control | ds->reset_mask;
	   asm("nop");
	   asm("nop");
	   *(volatile u_short *)ADRSPC_CONTROL = control & ~ds->reset_mask;
       }
   }

   /* 
    * Write the 68302 BAR and SCR
    */
   *(ushort *)MC68302_BAR = MC68302_BAR_BASE;
   *(uint *)MC68302_SCR = MC68302_SCR_VGE | MC68302_SCR_WPVE | 
     MC68302_SCR_LPREC | MC68302_SCR_LPP16 | MC68302_SCR_LPEN | 
       MC68302_SCR_LPCD;
}


/*
 * bri_getbuf_inline()
 * Get a packet buffer and link it to the receive ring.
 * This routine is used by the receive interrupt routine and implements
 * input throttling:
 */
static inline paktype *bri_getbuf_inline (hwidbtype       *idb,
					  bri_buf_descr_t *bd,
					  boolean          cache_ok)
{
   uchar *start;         /* start of packet */
   paktype *pak = NULL;  /* packet */

   /*
    * Check for input congestion:
    */
   if (idb->input_qcount > idb->input_defqcount)
       return(NULL);

    /*
     * If cache_ok is set to be TRUE when the inline is used, this
     * code will be compiled in. *NO* conditional will be generated.
     */
    if (cache_ok) {
	/*
	 * Try to get a cached buffer. Else get a new buffer.
	 */
	pak = pool_dequeue_cache(idb->pool);
    }

   if (!pak)
       pak = pool_getbuffer(idb->pool);

   if (pak) {
       /*
	* Initialize the packet encapsulation type and datagramstart:
	*/
       start = pak->data_area + idb->start_offset;
       pak->datagramstart = start; 
       
       /*
	* Initialize the ring entry for this packet:
	*/
       bd->buffer = start;
       bd->data_len = 0;
       if (bd->stat_control & MC68302_RCS_W)
	   bd->stat_control = MC68302_RCS_E | MC68302_RCS_X | MC68302_RCS_W |
	       MC68302_RCS_I;
       else
	   bd->stat_control = MC68302_RCS_E | MC68302_RCS_X | MC68302_RCS_I;

       return(pak);
   } else {
       idb->counters.output_nobuffers++;
       return(NULL);
   }
}


/*
 * bri_getbuf
 *
 * Function call wrapper around inline function
 */

static paktype *bri_getbuf (hwidbtype *idb, bri_buf_descr_t *bd)
{
    return(bri_getbuf_inline(idb, bd, FALSE));
}


/*
 * bri_resetpak()
 * Set up the ring entry to reuse the packet buffer
 */
static void bri_resetpak (bri_buf_descr_t *bd)
{
   /*
    * Re-initialize fields in the buffer descriptor:
    */
   bd->data_len = 0;
   if (bd->stat_control & MC68302_RCS_W)
      bd->stat_control = MC68302_RCS_E | MC68302_RCS_X | MC68302_RCS_W |
	MC68302_RCS_I;
   else
      bd->stat_control = MC68302_RCS_E | MC68302_RCS_X | MC68302_RCS_I;
}


/*
 * bri_tx_start()
 * Start transmission.
 */
static void bri_tx_start (hwidbtype *idb)
{
   bri_instance_t *ds = INSTANCE;  /* instance */
   int ring_index, bytes;          /* counters */
   bri_buf_descr_t *tmd;           /* TX buffer descriptor */
   ushort wrap_bit;                /* value of wrap bit */
   paktype *pak;                   /* packet */
   leveltype SR;                   /* Interrupt level */

   SR = raise_interrupt_level(NETS_DISABLE);

   /*
    * While there is another packet to output and room for it in the
    * 68302 TX queue, enqueue the packet:
    */
   while ((ds->tx_count < ds->tx_size) &&
	  (pak = (*idb->oqueue_dequeue)(idb))) {
      bytes = pak->datagramsize;
      if (bytes > idb->max_pak_size) {
	 errmsg(&msgsym(TOOBIG, LINK), idb->hw_namestring, bytes);
	 datagram_done(pak);
	 continue;
      }

      /*
       * Get pointers for the TX ring entry at tx_tail:
       */
      ring_index = ds->tx_tail++;
      if (ds->tx_tail >= ds->tx_size)
	 ds->tx_tail = 0;
      tmd = ds->tx_r + ring_index;

      ds->tx_p[ring_index] = pak;
      ds->tx_count++;

      idb->counters.tx_cumbytes += bytes;
      TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
      idb->counters.outputs++;

      /*
       * Count broadcasts
       */
      if ((pak->enctype == ET_HDLC) &&
	 ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
	 idb->counters.output_broad++;

      /*
       * Set up this TX ring entry for this packet
       */
      tmd->buffer = pak->datagramstart;
      tmd->data_len = bytes;
      if (ring_index == (ds->tx_size - 1))
	 wrap_bit = MC68302_TCS_W;
      else
	 wrap_bit = 0;
      tmd->stat_control = MC68302_TCS_R | MC68302_TCS_X | wrap_bit |
	MC68302_TCS_I | MC68302_TCS_L | MC68302_TCS_TC;

   }
   /*
    * If D chan, write SID to start output.
    */
   if ((idb->isdn_info->chan_num == BRI_D_CHAN) && (ds->tx_count > 0)) 
       ds->last_sid = write_sid(ds->config_reg, TP3420_CRF_DREQ1,
				ds->last_sid);
   reset_interrupt_level(SR);    /* Restore interrupt level */
}


/* 
 * bri_rx_int()
 * Handle a RX int.
 */
static void bri_rx_int (hwidbtype *idb)
{
   bri_buf_descr_t *rmd;           /* Pointer to RX buffer descr. */
   bri_instance_t *ds;             /* Pointer it Instance */
   idbtype *dchan_idb;             /* Pointer to D chan idb */
   uchar *datagramstart;           /* Non shared ram copy of datagramstart */
   ushort rxtype;                  /* Packet Type */
   paktype *pak;                   /* Pointer to pak */
   paktype *newpak;                /* Pointer to new packet */
   

   /*
    * Set up pointers
    */
   if(ISBRILEASED(idb))
       dchan_idb = idb->firstsw;
   else
       dchan_idb = idb->isdn_info->d_chan_idb->firstsw;
   /* N.B.:  Not subinterface-ready! */

   /*
    * Receive all input frames:
    */
  next_RX_packet:
   ds = INSTANCE;
   rmd = ds->rx_r + ds->rx_head;
   if ((rmd->stat_control & MC68302_RCS_E) == 0) {
      pak = ds->rx_p[(uint)ds->rx_head];
      pak->datagramsize = (ushort)(rmd->data_len) - 2; /* Remove CRC */

#if 0	/* +++ removed at France Telecom!!!  Wouldn't work w/ real switch */
      /* appears to be needed (sometimes we get AB instead of frame loss) */
      if (rmd->stat_control & MC68302_RCS_AB)
	    bri_phys_ind = PHYS_IND_EI1;
#endif

      /*
       * Check for RX errors
       */
      if ((rmd->stat_control & 0x3F) != 0) {
	 if ((rmd->stat_control & MC68302_RCS_LG) && bri_debug)
	    buginf("\nBRI(%d): RX Frame Length Violation. Length = %d\n",
		   idb->isdn_info->chan_num, rmd->data_len);
	 if ((rmd->stat_control & MC68302_RCS_NO) && bri_debug)
	    buginf("\nBRI(%d): RX Nonoctet Aligned Frame\n",
		   idb->isdn_info->chan_num);
	 if ((rmd->stat_control & MC68302_RCS_AB) && bri_debug)
	    buginf("\nBRI(%d): RX Abort Sequence\n",
		   idb->isdn_info->chan_num);
	 if ((rmd->stat_control & MC68302_RCS_CR) && bri_debug)
	    buginf("\nBRI(%d): RX CRC Error\n",
		   idb->isdn_info->chan_num);
	 if ((rmd->stat_control & MC68302_RCS_OV) && bri_debug)
	    buginf("\nBRI(%d): RX Overrun Error\n",
		   idb->isdn_info->chan_num);
	 if ((rmd->stat_control & MC68302_RCS_CD) && bri_debug)
	    buginf("\nBRI(%d): RX Carrier Detect Lost\n",
		   idb->isdn_info->chan_num);
	 bri_resetpak(rmd);
	 NEXT_READ_PACKET(ds);
      }

      /*
       * If we have a scheduled run interval configured, go see if we need
       * to drop packets to give processes a chance to run:
       */
      if (sched_run_interval)
	 goto bri_check_sched;

     return_bri_check_sched:
      /*
       * If packet from D-chan, just send it up and let the ISDN process
       * deal with it.
       */
      set_if_input(pak, idb->firstsw);
      newpak = bri_getbuf_inline(idb, rmd, TRUE);
      if (newpak) {
	  ds->rx_p[(uint)ds->rx_head] = newpak;
	  
          /*
           * If packet from D-chan, just send it up and let the ISDN process
           * deal with it.
           */
          if ((idb->isdn_info->chan_num == BRI_D_CHAN)&&
              (!ISBRILEASED(idb)))
              goto proc_level;

          /*
           * First, classify the packet:
           */
          datagramstart = pak->datagramstart;

          switch(idb->enctype) {
            case ET_HDLC:
              rxtype = hdlc_decode_encapsulation_inline(pak,
							(vhdlctype *)datagramstart);
	      if (reg_invoke_hdlc_fs(rxtype, pak)) {
		  NEXT_READ_PACKET(ds);
	      }
              break;

            case ET_PPP:
              rxtype = ppp_decode_encapsulation_inline(pak,
						       (ppp_hdr *)datagramstart);
	      if (reg_invoke_ppp_fs(rxtype, pak)) {
		  NEXT_READ_PACKET(ds);
	      }
	      break;

            default:
              rxtype = RXTYPE_UNKNOWN;
              break;
          }

          if (rxtype == RXTYPE_TRANSIT) {
              /*
               * Consider a frame for bridging
               */
              if (dchan_idb->tbridge_on_swidb) {
                  /*
                   * Mark as bridge encapsulation
                   */
                  pak->enctype = ET_BRIDGE;

                  if (!change_if_input(pak, dchan_idb)) {
                       bri_resetpak(rmd);
                       NEXT_READ_PACKET(ds);
                  }
                  /*
                   * If PPP is using reliable link then send to process level
                   * regardless
                   */
                  if(idb->ppp_lapb)
                      goto proc_level;

                  if (reg_invoke_tbridge_forward(pak)) {
                      NEXT_READ_PACKET(ds);
                  }
              }
          }

	  /*
	   * Queue packet for process level:
	   */
	proc_level:
	  serial_queue_for_process(idb, pak);
	  NEXT_READ_PACKET(ds);
      } else {
	 /*
	  * We could not get a new big buffer.
	  * Are we an HDLC or Frame relay encapsulated serial line?
	  */
	 if ((idb->enctype == ET_HDLC) ||
	    (idb->enctype == ET_FRAME_RELAY)) {
	     vhdlctype *hdlc = (vhdlctype *)pak->datagramstart;

	    /*
	     * If this packet is a high priority packet (keepalive, etc),
	     * try to get a smaller buffer big enough to copy the packet.
	     * Else, drop the packet:
	     */
	    if (serial_high_priority(hdlc->var_hdlctype)) {
		newpak = getbuffer(pak->datagramsize);
		if (newpak) {

		    /*
		     * Queue packet for process level:
		     */
                    clear_if_input(pak);
		    newpak->enctype = pak->enctype;
		    newpak->datagramsize = pak->datagramsize;
		    newpak->datagramstart = newpak->data_area +
			(pak->datagramstart - pak->data_area);
		    bcopy(pak->datagramstart, newpak->datagramstart,
			  pak->datagramsize);
                    set_if_input(newpak, idb->firstsw);
		    bri_resetpak(rmd); /* pak now owned by BRI */
		    serial_queue_for_process(idb, newpak);
		    NEXT_READ_PACKET(ds);
	       }
	    }
	 }

	 /*
	  * We could not get a new big buffer, so drop the packet:
	  */
	 bri_resetpak(rmd);
	 clear_if_input(pak);
	 idb->counters.input_drops++;
	 MAYBE_DROPPED(idb);
	 idb_mark_throttled(idb);
bri_next_packet:
	 NEXT_READ_PACKET(ds);
      }
   }

   return;

bri_check_sched:
   if (AWAKE(sched_run_time)) {

      /*
       * The scheduler has not run recently enough,
       * so, throttle the BRI:
       */
      bri_resetpak(rmd);
      idb->counters.input_drops++;
      clear_if_input(pak);
      MAYBE_DROPPED(idb);
      idb_mark_throttled(idb);
      goto bri_next_packet;
   } else
      goto return_bri_check_sched;
}


/*
 * bri_tx_int()
 * Handle a TX int.
 */
static void bri_tx_int (hwidbtype *idb)
{
   bri_buf_descr_t *tmd;   /* TX Buffer Descriptor */
   bri_instance_t *ds;     /* Pointer to Instance */
   int ring_index;         /* Ring Index counter */
   paktype *pak;           /* Pointer to a packet */


   /*
    * Process any transmitted packets:
    */
   ds = INSTANCE;
   ring_index = ds->tx_head;
   tmd = ds->tx_r + ds->tx_head;
   pak = ds->tx_p[ring_index];

   while (pak && ((tmd->stat_control & MC68302_TCS_R) == 0)) {

      /*
       * Update tx_head to point to next packet in queue:
       */
      tmd = ds->tx_r + ds->tx_head;
      tmd->stat_control &= ~(MC68302_TCS_X | MC68302_TCS_W | MC68302_TCS_I | 
			    MC68302_TCS_L | MC68302_TCS_TC);
      tmd->data_len = 0;
      tmd->buffer = NULL;
      ds->tx_head++;
      if (ds->tx_head >= ds->tx_size)
	 ds->tx_head = 0;

      /*
       * Do packet TX accounting and clean up:
       */
      datagram_done(pak); /* Release output packet*/
      GET_TIMESTAMP(idb->lastoutput);
      ds->tx_p[ring_index] = NULL;
      ds->tx_count--;

      /*
       * Next packet:
       */
      ring_index = ds->tx_head;
      tmd = ds->tx_r + ds->tx_head;
      pak = ds->tx_p[ring_index];
   }
   bri_tx_start(idb);	/* Possibly restart output */
}


/*
 * bri_misc_int()
 * Handle misc. BRI interrupts
 */
static void bri_misc_int (uint which, volatile uchar *scce, hwidbtype *idb)
{
   bri_instance_t *ds;            /* Pointer to instance */
   volatile uchar *com_reg;       /* Pointer to Command Register */
   int ring_index;                /* Ring index counter */
   bri_buf_descr_t *tmd;          /* Pointer to TX buffer descriptor */
   paktype *pak;                  /* Pointer to packet */

   /*
    * Set up pointers
    */
   ds = INSTANCE;

   /*
    * Check for Transmit Error Interrupt.
    */
   if (*scce & MC68302_SCCE_TXE) {
      *scce = MC68302_SCCE_TXE; /* clear the bit */
      idb->counters.output_error++;
      idb->counters.output_underrun++;
      MAYBE_DROPPED(idb);
   }

   /* 
    * Check for Busy Condition Interrupt
    */
   if (*scce & MC68302_SCCE_BSY) {
      *scce = MC68302_SCCE_BSY; /* clear the bit */ 
      idb->counters.input_error++;
      idb->counters.input_resource++;
      idb->counters.output_nobuffers++;
      MAYBE_DROPPED(idb);
   }
   /*
    * Give the buffer back to the '302
    */
   ring_index = ds->tx_head;
   pak = ds->tx_p[ring_index];

   if (pak != NULL) {
       tmd = ds->tx_r + ds->tx_head;
       tmd->stat_control &= (MC68302_TCS_X | MC68302_TCS_W | MC68302_TCS_I | 
			     MC68302_TCS_L | MC68302_TCS_TC);
       tmd->data_len = 0;
       tmd->buffer = NULL;
       ds->tx_head++;
       if (ds->tx_head >= ds->tx_size)
	   ds->tx_head = 0;
       datagram_done(pak); /* Release output packet*/
       GET_TIMESTAMP(idb->lastoutput);
       ds->tx_p[ring_index] = NULL;
       ds->tx_count--;
   }

   /*
    * restart Tx
    */
   com_reg = (uchar *)MC68302_CP_CR;
   *com_reg = MC68302_CR_RT | MC68392_CR_FLG | ((which & 0x03) << 1);
   bri_tx_start(idb);
}

static inline void wait_on_command(void)
{
  int i;
  volatile unsigned char *command = (uchar *)MC68302_CP_CR;

  for(i=0x4000;*command & MC68392_CR_FLG && i;i--)
    ;
}


/*
 * bri_interrupt()
 * BRI interrupt handler
 */
void bri_interrupt (hwidbtype *in_idb)
{
   hwidbtype *dchan_idb;               /* Pointer to D-chan idb */
   hwidbtype *idb;                     /* Pointer to idb */
   hwidbtype *int_idb;                 /* Pointer to interrupt idb */
   ushort ipr_reg;                   /* Contents of '302 IPR register */
   volatile ushort *ipr_reg_ptr;     /* Pointer to '302 IPR reg. */
   volatile ushort *isr_reg_ptr;     /* Pointer to '302 ISR reg. */
   uchar status_reg;                 /* Contents of SID status reg. */
   volatile uchar *scce;             /* Pointer to '302 SCCE reg. */
   ushort int_clear;                 /* Mask to clear the interrupt */
   uint which_scc;                   /* Which SCC the int. came from */
   bri_instance_t *ds;               /* Pointer to instance */
   volatile ushort berr_chan;        /* Chan. on which BERR occured */
   uchar *berr_addr;                 /* Addr. that caused the BERR */
   mcc68302_system_t *system_ram;    /* Pointer to base of '302 system ram */
   volatile scc_internal_reg_t *mc68302_scc_int_reg; /* SCC Internal Reg, */
   bri_buf_descr_t *tmd;             /* TX Buffer Descriptor */
   int ring_index;                   /* Ring Index counter */
   paktype *pak;                     /* Pointer to a packet */
   bri_buf_descr_t *txring;          /* TX ring pointer */
   int i;
   

   /*
    * Init. various pointers
    */
   idb = dchan_idb = in_idb->isdn_info->d_chan_idb;
   ds = INSTANCE;
   system_ram = (mcc68302_system_t *)MC68302_BASE;
   ipr_reg = system_ram->internal_reg.int_ipr;
   ipr_reg_ptr = &(system_ram->internal_reg.int_ipr);
   isr_reg_ptr = &(system_ram->internal_reg.int_isr);
   mc68302_scc_int_reg = &(system_ram->scc_internal_reg);

   /*
    * Check for SID interrupt
    */
   if (ipr_reg & MC68302_IPR_PB11) {
      /*
       * Clear the Interrupt Pins
       */
      *ipr_reg_ptr = MC68302_ISR_PB11;
      *isr_reg_ptr = MC68302_ISR_PB11;

      /*
       * Read the SID status register, then handle the interrupt
       */
      status_reg = read_sid(system_ram);

#if FALSE
      if (bri_debug)
	  buginf("SID interrupt. status reg = %x\n", status_reg);
#endif

      /*
       * Check for clobbered status register
       */
      if (status_reg == 0)
	  status_reg = ds->last_sid;

      ds->last_sid = 0;

      if (bri_debug)
	  buginf("SID interrupt. status reg = %x\n", status_reg);

      switch (status_reg) {
         case TP3420_SRF_LSD:   /* Line Signal Detect */

	    /* 
	     *Send PUP to start Activation procedure.
	     */
	    if (loopback) {
	        ds->last_sid = write_sid(system_ram, TP3420_CRF_LBL1,
					 ds->last_sid); /* loopback B1 */
	        ds->last_sid = write_sid(system_ram, TP3420_CRF_LBL2,
					 ds->last_sid); /* loopback B2 */
	    } else {
	        ds->last_sid = write_sid(system_ram, TP3420_CRF_CAL,
					 ds->last_sid); /* Clear Loopback */
	    }

	    ds->last_sid = write_sid(system_ram, TP3420_CRF_PUP, ds->last_sid);
	    break;

	 case TP3420_SRF_AP:   /* Activation Pending */
	    /*
	     * Turn off deactivation timer for Bridging Measure homologation
	     */
	    system_ram->internal_reg.timer_tmr1 = 0; /* Deactivation timer */ 
	    /*
	     * Send AR to continue Activation
	     */
	    ds->last_sid = write_sid(system_ram, TP3420_CRF_AR, ds->last_sid);
	    if(activated != ACTIVATED) {
	       system_ram->internal_reg.timer_trr2 = TIMER_8S; /* Set T3 */
	       system_ram->internal_reg.timer_tmr2 = 0xFF1D;   /* Start it */
	       activated = PENDING;
	    } else 
	       system_ram->internal_reg.timer_tmr2 = 0;/* Turn off timer T3 */
	    break;

	 case TP3420_SRF_CON:   /* D-chan collision */
	    /*
	     * According to TP3420 Data Book, it is necessary to resend
             * the packet when a collision occurs on the D-channel.  To
	     * make this the next packet out the D-Channel, we will have to
             * re-arm the 68302 to transmit the same packet by adjusting
             * its internal Transmit Buffer Descriptor Number (TBD#), see
	     * section 4.5.6.4 of the 68302 manual.  Before we can change
	     * that register we must first stop the transmitter.  See
	     * section 4.5.10 for the proper sequnce.  
	     */

	    (*(uchar *)MC68302_CP_CR) = MC68302_CR_ST | 
	                                MC68392_CR_FLG |
					MC68302_CR_SCC3;

	    mc68302_scc_int_reg->scm3 = MC68302_SCM_DIAG1 | 
              MC68302_SCM_DIAG0 | MC68302_SCM_ENR;

	    ds->last_sid = write_sid(ds->config_reg,
				     TP3420_CRF_DREQ1, ds->last_sid);

	    if (bri_debug)
	       buginf("\nBRI: D-chan Collision\n");

	    ring_index = ds->tx_head;
	    pak = ds->tx_p[ring_index];

	    /* Wait for stop command to complete */
	    wait_on_command();

	    if (pak != NULL) {
		tmd = ds->tx_r + ds->tx_head;
		tmd->stat_control &= (MC68302_TCS_X | MC68302_TCS_W |
				      MC68302_TCS_I | MC68302_TCS_L | 
				      MC68302_TCS_TC);
		tmd->stat_control |= MC68302_TCS_R;

		ds->config_reg->scc3_param.tx_int_buf_num =
		  MC68302_TXBD0_OFFSET + (ring_index << 3);
	    }

	    (*(uchar *)MC68302_CP_CR) = MC68302_CR_RT | 
	                                MC68392_CR_FLG |
					MC68302_CR_SCC3;

	    mc68302_scc_int_reg->scm3 = MC68302_SCM_DIAG1 | 
              MC68302_SCM_DIAG0 | MC68302_SCM_ENR | MC68302_SCM_ENT;
	    
	    TIMER_START(idb->xmittimer, TIMEOUT); /* Set timeout timer */
	    ds->d_chan_coll++;  /* Count d-chan collisions */

	    break;

	 case TP3420_SRF_AI:   /* Activation Indication */
	    /* 
	     * Send message to ISDN code and up the link.
	     * Turn on TX and RX for the SCC's and turn off timer T3 
	     */
	    system_ram->internal_reg.timer_tmr2 = 0; /* Turn off timer T3 */
	    system_ram->internal_reg.timer_tmr1 = 0; /* Deactivation timer */
	    
	    /* 
	     * only enable the D-channel scc, don't do the B-channels yet
	     */
	    mc68302_scc_int_reg->scm3 = MC68302_SCM_DIAG1 | 
              MC68302_SCM_DIAG0 | MC68302_SCM_ENR | MC68302_SCM_ENT;
	    if (bri_debug)
	       buginf("\nBRI: Received activation indication.\n");
	    /*
	     * Fixed homologation problem, when get a short 500ms break, do not
	     * set the bri_phys_ind flag, it will cause Layer 2 to remove the
	     * TEI value. Only for NET3 for now.
	     */
	    if((SwitchType != BRI_NET3_STYPE) ||
	      ((SwitchType == BRI_NET3_STYPE) && (activated != ACTIVATED))) {
	        bri_enq_prim(idb->unit, ISDN_PH_ACT_IND);
	        activated = ACTIVATED;
	        bri_tx_start(dchan_idb);
	    }
	    break;

	 case TP3420_SRF_EI:   /* Loss of Frame Alignment */
	    /*
	     * Print an error message.
	     */
	    if (bri_debug)
		buginf("\nReceived SID Loss of Frame Alignment int.\n");
	    if((SwitchType != BRI_1TR6_STYPE) &&
	       (SwitchType != BRI_NET3_STYPE))
		bri_enq_prim(idb->unit, ISDN_MPH_EI1_IND);
	    break;

	 case TP3420_SRF_DI:   /* Deactivation */
	    /*
	     * Set deactivation timer to make sure this isn't a line
	     * burp.  This is timer T4 handling.
	     *
	     * This should only be done in F7 and F8 for VN2,
	     * added NET3 for Fiji, I-CTR (Bridging Measure).
	     */
	    if (bri_debug)
	       	buginf("\nBRI: Received deactivation indication.\n");
	    if((activated == ACTIVATED) || ((SwitchType != VN2_STYPE) && 
	        (SwitchType != VN3_STYPE) && (SwitchType != BRI_NET3_STYPE))) {
	        if(bri_debug)
	           buginf("\nBRI: T4 timer started ");
	        system_ram->internal_reg.timer_trr1 = TIMER_600MS;/* Set T4 */
	        system_ram->internal_reg.timer_tmr1 = 0xFF1D;   /* Start it */
	    } else {
	        bri_enq_prim(idb->unit, ISDN_PH_DEACT_IND);
	        activated = DEACTIVATED;
	        ds->last_sid = write_sid(system_ram, TP3420_CRF_DR,
					 ds->last_sid);
	        ds->last_sid = write_sid(system_ram, TP3420_CRF_PDN,
					 ds->last_sid);
		
	        /*
	         * Turn off T3
	         */
	        system_ram->internal_reg.timer_tmr2 = 0; /* Turn off timer */

		/* 
		 * Clear int 
		 */
	        system_ram->internal_reg.timer_ter2 = MC68302_TER_REF;
	    }
	    break;
	    
	  default:
	    /*
	     * We shouldn't get anything else.  Print error message.
	     */
	    if (bri_debug)
	       buginf("\nUnexpected int. from SID.  status_reg = %x", 
		      status_reg);
	    break;
      }

      return;
   }

   /*
    * Check for SDMA Bus Error Interrupt
    */
   if (ipr_reg & MC68302_IPR_SDMA) {

      /* 
       *Read which channel caused the BERR
       */
      berr_chan = *(ushort *)MC68302_BERR_CHAN;

      /* 
       * Read the pointer that caused the BERR
       */
      switch (berr_chan) {
         case SCC1_TX_CHAN:
	    berr_addr = system_ram->scc1_param.tx_int_data_pt;
	    break;
         case SCC1_RX_CHAN:
	    berr_addr = system_ram->scc1_param.rx_int_data_pt;
	    break;
         case SCC2_TX_CHAN:
	    berr_addr = system_ram->scc2_param.tx_int_data_pt;
	    break;
         case SCC2_RX_CHAN:
	    berr_addr = system_ram->scc2_param.rx_int_data_pt;
	    break;
         case SCC3_TX_CHAN:
	    berr_addr = system_ram->scc3_param.tx_int_data_pt;
	    break;
         case SCC3_RX_CHAN:
	    berr_addr = system_ram->scc3_param.rx_int_data_pt;
	    break;
	 default:
	    berr_addr = 0;
      }	

      buginf("\nBRI: SDMA Bus Error on chan %d, with addr 0x%x\n", berr_chan,
	     berr_addr);

      /*
       * Re-init the SCC 
       */
      bri_init(idb, FALSE);
      return;
   }

   /*
    * Check for timer 1 interrupt. This timer insures we wait 2ms after
    * sending the PUP, to send the Activation request.
    */
   if (ipr_reg & MC68302_IPR_T1) {
      system_ram->internal_reg.timer_ter1 = MC68302_TER_REF; /* Clear int */

      if (system_ram->internal_reg.timer_trr1 == TIMER_2MS) {
	  /*
	   * Send a Activation Request
	   */
	  ds->last_sid = write_sid(system_ram, TP3420_CRF_AR, ds->last_sid);

	  system_ram->internal_reg.timer_trr2 = TIMER_8S;   /* Set T3 timer */
	  system_ram->internal_reg.timer_tmr2 = 0xFF1D;     /* Start timer */
      } else if (system_ram->internal_reg.timer_trr1 == TIMER_600MS) {
	  /*
	   * Must be the deactivate timer, check the FState of the 3420 to see 
	   * if must deactivate the line. Do not deactivate if in State F7. 
	   */
	  ds->last_sid = write_sid(system_ram, TP3420_CRF_ENST, ds->last_sid); 
	  ds->last_sid = write_sid(system_ram, TP3420_CRF_DISST,
				   ds->last_sid); 
	  if(ds->last_sid != TP3420_F7_STATE) { 
	    if(bri_debug) 
	       buginf("\nBRI: Deactivation timer expired. Activation halted ");
	    bri_enq_prim(idb->unit, ISDN_PH_DEACT_IND);
	    activated = DEACTIVATED;
	    ds->last_sid = write_sid(system_ram, TP3420_CRF_DR, ds->last_sid);
	    ds->last_sid = write_sid(system_ram, TP3420_CRF_PDN, ds->last_sid);
	  } else {
	    if(bri_debug)
	       buginf("\nBRI: Deactivation timer expired, staying in State F7 ");
	  }
	  
	  /* Turn off T3 */
	  system_ram->internal_reg.timer_tmr2 = 0;    /* Turn off timer */
	  system_ram->internal_reg.timer_ter2 = MC68302_TER_REF; /* Clear int*/
      }

      system_ram->internal_reg.timer_tmr1 = 0;    /* Turn off timer */
      *ipr_reg_ptr = MC68302_ISR_T1;
      *isr_reg_ptr = MC68302_ISR_T1;
      return;
   }

   /*
    * Check for timer 2 (ISDN T3).  If it triggers, deactivate.
    */
   if (ipr_reg & MC68302_IPR_T2) {
      system_ram->internal_reg.timer_tmr2 = 0;    /* Turn off timer */
      system_ram->internal_reg.timer_ter2 = MC68302_TER_REF; /* Clear int */
      *ipr_reg_ptr = MC68302_ISR_T2;
      *isr_reg_ptr = MC68302_ISR_T2;
      /*
       * Stop trying to activate and power down, but only if not in State F6.
       * Determine the state of Layer 1 with the ENST and DISST commands.
       */
      ds->last_sid = write_sid(system_ram, TP3420_CRF_ENST, ds->last_sid); 
      ds->last_sid = write_sid(system_ram, TP3420_CRF_DISST, ds->last_sid);
      if(((SwitchType == BRI_NET3_STYPE) || (SwitchType == BRI_TS013_STYPE)) &&
         (ds->last_sid == TP3420_F6_STATE)) {
        bri_enq_prim(idb->unit, ISDN_PH_DEACT_IND);
        if(bri_debug)
           buginf("\nBRI: Timer T3 expired in State F6, not deactivating ");
      } else {
        bri_enq_prim(idb->unit, ISDN_PH_DEACT_IND);
        activated = DEACTIVATED;
		ds->last_sid = write_sid((mcc68302_system_t *)system_ram,
					 TP3420_CRF_DR, ds->last_sid);
        ds->last_sid = write_sid((mcc68302_system_t *)system_ram,
				 TP3420_CRF_PDN, ds->last_sid);

        if (bri_debug) 
            buginf("\nBRI: Timer T3 expired. Activation halted ");
      }

      /* 
       * Blow out queues 
       */
      txring = ds->tx_r;
      if (ds->tx_size)
	 for (i = 0; i < ds->tx_size; i++) {
	    txring->stat_control = MC68302_TCS_X | MC68302_TCS_I | 
		MC68302_TCS_L | MC68302_TCS_TC;

	    /*
	     * Check for wrap bit
	     */
	    if (i == ds->tx_size - 1)
	       txring->stat_control |= MC68302_TCS_W;
	    txring->data_len = 0;
	    txring->buffer = NULL;
	    
	    if (ds->tx_p[i]) {
	       pak = ds->tx_p[i];
	       datagram_done(pak);
	       ds->tx_p[i] = NULL;
	    }
	    
	    txring++;
	 }

      /*
       * Make sure we reset tx_count after blowing away queues.
       * Fixed for Cabrillo homologation.
       */
      ds->tx_count = 0;
      holdq_reset(idb);

      return;
   }

   /*
    * Check for SCC interrupts.
    */
   if (ipr_reg & MC68302_IPR_SCC1) {
      which_scc = 0;
      int_idb = dchan_idb->isdn_info->b1_chan_idb;
      scce = &(system_ram->scc_internal_reg.scce1);
      int_clear = MC68302_ISR_SCC1;
   } else 
      if (ipr_reg & MC68302_IPR_SCC2) {
	 which_scc = 1;
	 int_idb = dchan_idb->isdn_info->b2_chan_idb;
	 scce = &(system_ram->scc_internal_reg.scce2);
	 int_clear = MC68302_ISR_SCC2;
      } else
	 if (ipr_reg & MC68302_IPR_SCC3) {
	    which_scc = 2;
	    int_idb = dchan_idb;
	    scce = &(system_ram->scc_internal_reg.scce3);
	    int_clear = MC68302_ISR_SCC3;
	 } else { /* Bad interrupt */
	    /* 
	     * Bad int. Print error message
	     */
	    if (bri_debug)
	       buginf("\n Unexpected IMP int: ipr = %x\n", ipr_reg);
	    *ipr_reg_ptr = ipr_reg;
            *isr_reg_ptr = ipr_reg;
	    return;
	 }

   /*
    * Found out who caused it, now find out why
    */
   if (*scce & MC68302_SCCE_RXF) {
      *scce = MC68302_SCCE_RXF;
      bri_rx_int(int_idb);
   } else
      if (*scce & MC68302_SCCE_TXB) {
	 *scce = MC68302_SCCE_TXB;
	 bri_tx_int(int_idb);
      } else
	 bri_misc_int(which_scc, scce, int_idb);

   /*
    * Clear the interrupt bits
    */
   *ipr_reg_ptr = int_clear;
   *isr_reg_ptr = int_clear;
}


/*
 * bri_stop()
 * Stop the BRI interface.
 */
static void bri_stop (mcc68302_system_t *ram_base)
{
   volatile scc_internal_reg_t *mc68302_scc_int_reg;

   /*
    * Stop the 68302 SCCs 
    */
   mc68302_scc_int_reg = &(ram_base->scc_internal_reg);
   mc68302_scc_int_reg->scm1 = 0;
   mc68302_scc_int_reg->scm2 = 0;
   mc68302_scc_int_reg->scm3 = 0;
}


/*
 * bri_init()
 * Initialize the BRI
 */
void bri_init (hwidbtype *in_idb, boolean maxdgram_change)
{
   SAVE_CALLER();
   bri_buf_descr_t *tmd;              /* TX buffer descr. */
   bri_buf_descr_t *rmd;              /* RX buffer descr. */
   bri_buf_descr_t *txring;           /* TX ring pointer */
   bri_buf_descr_t *rxring;           /* RX ring pointer */
   hwidbtype *dchan_idb;              /* D-Chan idb */
   hwidbtype *idb;                    /* idb */
   hwidbtype *bchan1_idb;             /* B- chan1 idb */
   hwidbtype *bchan2_idb;             /* B- chan2 idb */
   bri_instance_t *ds;                /* instance */
   paktype *pak;                      /* packet */
   leveltype SR;                      /* Interrupt level */
   int chan_cnt;                      /* Channel count */
   mcc68302_system_t *mcc68302_system_ram;      /* '302 system ram */
   volatile param_ram_scc_t *mc68302_param_ram; /* '302 param ram */
   volatile prot_specific_t *mc68302_prot_spec; /* '302 protocol spec. ram */
   volatile internal_reg_t *mc68302_int_reg;    /* '302 internal reg */
   volatile scc_internal_reg_t *mc68302_scc_int_reg; /* scc internal reg */
   pooltype *old_pool;
   isdn_bri_t *isdn_info;
   int i;


   /* Disable level 4 interrupt */ 
   SR = raise_interrupt_level(NETS_DISABLE);

   /*
    * Set up pointer to D-chan IDB 
    */
   dchan_idb = in_idb->isdn_info->d_chan_idb;
   idb = dchan_idb;
   isdn_info = idb->isdn_info;

   /*
    * Loop to init the 3 channel idbs 
    */
   for (chan_cnt=0; chan_cnt <BRI_MAX_CHAN; chan_cnt++) {

      switch (chan_cnt) {
	 case BRI_B1_CHAN:
	    idb = dchan_idb->isdn_info->b1_chan_idb;
	    break;
	 case BRI_B2_CHAN:
	    idb = dchan_idb->isdn_info->b2_chan_idb;
	    break;
      }

      MAYBE_DROPPED(idb);              /* May lose output from reset */
      
      /*
       * Have we ever been intialized before?
       */
      if (INSTANCE) {
	/* Yes, stop the BRI interface */
	ds = INSTANCE;
	bri_stop(ds->config_reg);
	idb->counters.resets++;
	
	if (bri_debug)
	  buginf("\nBRI(%d): Reset from 0x%x", idb->isdn_info->chan_num,
		 caller());
	
	/* 
	 * Take ownership of all TX ring entries and free any buffers:
	 */
	txring = ds->tx_r;
	if (ds->tx_size)
	   for (i = 0; i < ds->tx_size; i++) {
	      txring->stat_control = MC68302_TCS_X | MC68302_TCS_I | 
		MC68302_TCS_L | MC68302_TCS_TC;

	      /*
	       * Check for wrap bit
	       */
	      if (i == ds->tx_size - 1)
	         txring->stat_control |= MC68302_TCS_W;
	      txring->data_len = 0;
	      txring->buffer = NULL;
	    
	      if (ds->tx_p[i]) {
	         pak = ds->tx_p[i];
		 datagram_done(pak);
		 ds->tx_p[i] = NULL;
	      }
	    
	      txring++;
	   }
	
	/*
	 * Free all RX ring buffers
	 */
	rxring = ds->rx_r;
	if (ds->rx_size)
	   for (i = 0; i < ds->rx_size; i++) {
	      rxring->stat_control = MC68302_RCS_X | MC68302_RCS_I;
	      /*
	       * Check for wrap bit
	       */
	      if (i == ds->rx_size - 1)
	         rxring->stat_control |= MC68302_RCS_W;
	      rxring->data_len = 0;
	      rxring->buffer = NULL;
	      if (ds->rx_p[i]) {
	         if (maxdgram_change) {
		    discard_buffer(ds->rx_p[i]);
	         } else {
		    retbuffer(ds->rx_p[i]);
	         }
	         ds->rx_p[i] = NULL;
	      }
	      rxring++;
	   }
      }
      
      /*
       * Flush the hold queue
       */
      holdq_reset(idb);
   }

   /* 
    * Tell ISDN code about deactivation and take down the line.
    * Tell it only if we are changing state. Sending the primative
    * before the pdn because it is interferes with interrupts.
    */
   if (isdn_process_up) {
           if (activated != DEACTIVATED) {
   	       bri_enq_prim(dchan_idb->unit, ISDN_PH_DEACT_IND);
	       if (bri_debug)
	           buginf("\nBRI: Deactivating in bri_reset().\n");
	   }
   }

   activated = DEACTIVATED;

   /*
    * If interface is administratively down, don't re-enable it. Note that
    * we perform this check after shutting down the BRI and freeing
    * all buffers held by the RX and TX rings:
    */
   if ((dchan_idb->state == IDBS_ADMINDOWN) && !ISBRILEASED(dchan_idb)) {
       reset_interrupt_level(SR);
       return;
   }
   
   /*
    * Another loop to init the idb's
    */
   for (chan_cnt=0; chan_cnt <BRI_MAX_CHAN; chan_cnt++) {

      switch (chan_cnt) {
         case BRI_D_CHAN:
	    idb = dchan_idb;
	    break;
	 case BRI_B1_CHAN:
	    idb = dchan_idb->isdn_info->b1_chan_idb;
	    break;
	 case BRI_B2_CHAN:
	    idb = dchan_idb->isdn_info->b2_chan_idb;
	    break;
      } 

      /*
       * Was this interface ever initialized before?
       */
      if (INSTANCE)
	 /*
	  * Yes, get the pointer to this instance of the driver:
	  */
	 ds = INSTANCE;
      else {
	 uchar *ds_base;

	 /*
	  * No, allocate this instance of the driver:
	  */
	 ds_base = malloc(BRI_DS_SIZE);
	 if (ds_base) {
	    ds = (bri_instance_t *)(((ulong)ds_base + 7) & ~7L);
	    INSTANCE = ds;
	    ds->config_reg = (mcc68302_system_t *)MC68302_BASE;
	    ds->last_sid = 0;
	 } else {
	    idb->state = IDBS_DOWN;
	    GET_TIMESTAMP(idb->state_time);
	    reset_interrupt_level(SR);
	    return;
	 }
      }

      /*
       * Set up start_offset
       */
      switch (idb->enctype) {
         case ET_HDLC:
	 case ET_FRAME_RELAY:
	 case ET_PPP:
	 case ET_SMDS:
	 case ET_SDLCP:
         case ET_SDLCS:
         case ET_SDLC:
	 case ET_STUN:
         case ET_BSTUN:
	 case ET_LAPB:
	 case ET_X25:
         case ET_V120:
         case ET_CPP:
	    idb->start_offset = ENCAPBYTES - idb->encsize;
	    break;
	 default:
	    errmsg(&msgsym(BOGUSENCAP, LINK), idb->hw_namestring,
		   idb->enctype);
	    idb->enctype = ET_HDLC;
	    idb->start_offset = ENCAPBYTES - HDLC_ENCAPBYTES;
      }
   }

   /*
    * Find the register address for the BRI and stop it:
    */
   ds->config_reg = (mcc68302_system_t *)MC68302_BASE;
   if (cpu_type == CPU_CRISTAL || cpu_type == CPU_CANCUN)
       network_d_idb = dchan_idb;
   else
       network_b_idb = dchan_idb;
   ds->reset_mask = CONTROL_RESET_NETWORK_B;

   bri_stop(ds->config_reg);
   bri_reset(dchan_idb);

   /*
    * Yet another init loop
    */
   for (chan_cnt=0; chan_cnt <BRI_MAX_CHAN; chan_cnt++) {

      /* Select correct idb */
      switch (chan_cnt) {
         case BRI_D_CHAN:
	    idb = dchan_idb;
	    break;
	 case BRI_B1_CHAN:
	    idb = dchan_idb->isdn_info->b1_chan_idb;
	    break;
	 case BRI_B2_CHAN:
	    idb = dchan_idb->isdn_info->b2_chan_idb;
	    break;
      }

      ds = INSTANCE;

      /*
       * Set up buffer pointers
       */
      switch (chan_cnt) {
         case BRI_D_CHAN:
	    ds->rx_r = (bri_buf_descr_t *)MC68302_SCC3_RX_RING;
	    ds->tx_r = (bri_buf_descr_t *)MC68302_SCC3_TX_RING;
	    ds->rx_size = make_ring_size(D_CHAN_MAX_RX_SIZE);
	    if (hwidb_use_short_tx_q_inline(idb)) {
		/*
		 * shorten TQL to make reponsive to prioritization
		 * but not so short it leaves air gaps on the wire
		 * when compressing or line is high speed
		 */
		    ds->tx_size = make_ring_size(2);
	    } else {
		ds->tx_size = make_ring_size(D_CHAN_MAX_TX_SIZE);
	    }
	    idb->mci_txcount = ds->tx_size;
	    break;
	 case BRI_B1_CHAN:
	    ds->rx_r = (bri_buf_descr_t *)MC68302_SCC1_RX_RING;
	    ds->tx_r = (bri_buf_descr_t *)MC68302_SCC1_TX_RING;
	    ds->rx_size = make_ring_size(B_CHAN_MAX_RX_SIZE);
	    if (hwidb_use_short_tx_q_inline(idb)) {
		/*
		 * shorten TQL to make reponsive to prioritization
		 * but not so short it leaves air gaps on the wire
		 * when compressing or line is high speed
		 */
		    ds->tx_size = make_ring_size(2);
	    } else {
		ds->tx_size = make_ring_size(B_CHAN_MAX_TX_SIZE);
	    }
	    idb->mci_txcount = ds->tx_size;
	    break;
	 case BRI_B2_CHAN:
	    ds->rx_r = (bri_buf_descr_t *)MC68302_SCC2_RX_RING;
	    ds->tx_r = (bri_buf_descr_t *)MC68302_SCC2_TX_RING;
	    ds->rx_size = make_ring_size(B_CHAN_MAX_RX_SIZE);
	    if (hwidb_use_short_tx_q_inline(idb)) {
		/*
		 * shorten TQL to make reponsive to prioritization
		 * but not so short it leaves air gaps on the wire
		 * when compressing or line is high speed
		 */
		    ds->tx_size = make_ring_size(2);
	    } else {
		ds->tx_size = make_ring_size(B_CHAN_MAX_TX_SIZE);
	    }
	    idb->mci_txcount = ds->tx_size;
	    break;
      }
	    
      /*
       * Clear the variables used for RX and TX ring access
       */
      ds->rx_head = 0;
      ds->tx_count = 0;
      ds->tx_head = 0;
      ds->tx_tail = 0;

      /*
       * If a private buffer pool for this interface exists AND there
       * has been an MTU change, discard the pool with the old-sized
       * buffers. 
       */

      if (idb->pool && (idb->pool->size != idb->max_buffer_size)) {
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

	  idb->pool = pak_pool_create(idb->hw_namestring, idb->pool_group,
					  idb->max_buffer_size,
					  POOL_SANITY,
					  NULL); 
	  if (!idb->pool) {
	      crashdump(0);
	  }
	
	  idb->pool->fallback = pak_pool_find_by_size(idb->max_buffer_size);
	  pool_adjust(idb->pool, 0, PRIV_NUM_BUF(ds->rx_size),
		      PRIV_NUM_BUF(ds->rx_size), TRUE); 
	
	  pak_pool_create_cache(idb->pool, PRIV_NUM_CACHE(ds->rx_size));
	  pool_adjust_cache(idb->pool, PRIV_NUM_CACHE(ds->rx_size)); 
      }

      /*
       * Set up the RX ring in the 68302 and ensure that it is full
       * of buffers owned by the 68302:
       */
      rmd = (bri_buf_descr_t *)ds->rx_r;
      for (i = 0; i <ds->rx_size; i++) {
	 rmd->data_len = 0;

	 pak = bri_getbuf(idb, rmd);
	 if (pak) {
	    ds->rx_p[i] = pak;
	 } else {
	    /*
	     * If we could not get a packet, mark our interface as
	     * down so we try again later:
	     */
	    idb->state = IDBS_DOWN;
	    GET_TIMESTAMP(idb->state_time);
	    reset_interrupt_level(SR);
	    return;
	 }
	 rmd++;
      }

      /*
       * Set up the TX buffer ring and say that we own all of the
       * buffers so that the BRI does not try to transmit any of them:
       */
      tmd = (bri_buf_descr_t *)ds->tx_r;
      for (i = 0; i < ds->tx_size; i++)
	  tmd->data_len = 0;
  }


   /*
    * Select the proper assembly language level 4 interrupt wrapper:
    */
   if (cpu_type == CPU_CRISTAL || cpu_type == CPU_CANCUN) {
       pan_int_array[2].function = (ulong)level4_pan_b;
       pan_int_array[2].idb1 = dchan_idb;
   } else {
       pan_int_array[1].function = (ulong)level4_pan_b;
       pan_int_array[1].idb1 = dchan_idb;
   }

   idb = dchan_idb;
   ds = INSTANCE;

   /* 
    * Set up 68302 registers
    */
   mcc68302_system_ram = ds->config_reg;

   /* 
    * Internal registers
    */
   mc68302_int_reg = &(mcc68302_system_ram->internal_reg);
   mc68302_int_reg->int_gimr = MC68302_GIMR_IV7 | MC68302_GIMR_IV6 |
     MC68302_GIMR_IV1;
   
   mc68302_int_reg->pio_paddr = MC68302_PIO_PA12;
   mc68302_int_reg->pio_pacnt = MC68302_PIO_PA8 | MC68302_PIO_PA9 |
     MC68302_PIO_PA10 | MC68302_PIO_PA11;

   mc68302_int_reg->timer_wrr = 0;

   mc68302_int_reg->cs_br0 = 0;

   /*
    * Configure SCC's 
    */
   mc68302_scc_int_reg = &(mcc68302_system_ram->scc_internal_reg);
   mc68302_scc_int_reg->simode = 0x024b;

   /*
    * Init SCC1 Regs
    */
   mc68302_param_ram = &(mcc68302_system_ram->scc1_param);

   /*
    * For DMA accesses to and from the SCCs, set FC bits to
    * 001 (binary) for user data space. This will prevent
    * DMA accesses from ever matching with the internal
    * DPRAM accesses (since the 68302 uses only 24 address
    * bits and hence aliases every 16M).
    */
   mc68302_param_ram->rx_func_code = MC68302_SCC_RFCR;
   mc68302_param_ram->tx_func_code = MC68302_SCC_TFCR;
   bchan1_idb = dchan_idb->isdn_info->b1_chan_idb; 
   mc68302_param_ram->max_rx_buff_len = bchan1_idb->max_buffer_size;

   mc68302_prot_spec = &(mc68302_param_ram->prot_spec);
   mc68302_prot_spec->c_mask_low = 0xF0B8;
   mc68302_prot_spec->dis_frame_cnt = 0;
   mc68302_prot_spec->crc_err_cnt = 0;
   mc68302_prot_spec->abort_seq_cnt = 0;
   mc68302_prot_spec->non_mat_add_rx_cnt = 0;
   mc68302_prot_spec->frame_retrans_cnt = 0;
   mc68302_prot_spec->max_frame_len = 
     dchan_idb->isdn_info->b1_chan_idb->max_buffer_size;
   mc68302_prot_spec->add_mask = 0;

   mc68302_scc_int_reg->scon1 = 0x7FFE;
   mc68302_scc_int_reg->scce1 = 0xFF; /* Clear events */
   mc68302_scc_int_reg->sccm1 = MC68302_SCCE_TXE | MC68302_SCCE_RXF |
     MC68302_SCCE_BSY | MC68302_SCCE_TXB;

   /*
    * Init SCC2 Regs
    */
   mc68302_param_ram = &(mcc68302_system_ram->scc2_param);
   mc68302_param_ram->rx_func_code = MC68302_SCC_RFCR;
   mc68302_param_ram->tx_func_code = MC68302_SCC_TFCR;
   bchan2_idb = dchan_idb->isdn_info->b2_chan_idb;
   mc68302_param_ram->max_rx_buff_len = bchan2_idb->max_buffer_size; 

   mc68302_prot_spec = &(mc68302_param_ram->prot_spec);
   mc68302_prot_spec->c_mask_low = 0xF0B8;
   mc68302_prot_spec->dis_frame_cnt = 0;
   mc68302_prot_spec->crc_err_cnt = 0;
   mc68302_prot_spec->abort_seq_cnt = 0;
   mc68302_prot_spec->non_mat_add_rx_cnt = 0;
   mc68302_prot_spec->frame_retrans_cnt = 0;
   mc68302_prot_spec->max_frame_len = 
     dchan_idb->isdn_info->b2_chan_idb->max_buffer_size;
   mc68302_prot_spec->add_mask = 0;

   mc68302_scc_int_reg->scon2 = 0x7FFE;
   mc68302_scc_int_reg->scce2 = 0xFF; /* Clear events */
   mc68302_scc_int_reg->sccm2 = MC68302_SCCE_TXE | MC68302_SCCE_RXF | 
     MC68302_SCCE_BSY | MC68302_SCCE_TXB;

   /*
    * Init SCC3 Regs
    */
   mc68302_param_ram = &(mcc68302_system_ram->scc3_param);
   mc68302_param_ram->rx_func_code = MC68302_SCC_RFCR;
   mc68302_param_ram->tx_func_code = MC68302_SCC_TFCR;
   mc68302_param_ram->max_rx_buff_len = dchan_idb->max_buffer_size;

   mc68302_prot_spec = &(mc68302_param_ram->prot_spec);
   mc68302_prot_spec->c_mask_low = 0xF0B8;
   mc68302_prot_spec->dis_frame_cnt = 0;
   mc68302_prot_spec->crc_err_cnt = 0;
   mc68302_prot_spec->abort_seq_cnt = 0;
   mc68302_prot_spec->non_mat_add_rx_cnt = 0;
   mc68302_prot_spec->frame_retrans_cnt = 0;
   mc68302_prot_spec->max_frame_len = dchan_idb->max_buffer_size;
   mc68302_prot_spec->add_mask = 0;

   mc68302_scc_int_reg->scon3 = 0x7FFE;
   mc68302_scc_int_reg->scce3 = 0xFF; /* Clear events */
   mc68302_scc_int_reg->sccm3 = MC68302_SCCE_TXE | MC68302_SCCE_RXF | 
     MC68302_SCCE_BSY | MC68302_SCCE_TXB;

   /*
    * The following modification makes sure that all interrupts are
    * enabled only if the isdn process has been spawned.
    */
   if (isdn_process_up) {
       mc68302_int_reg->int_imr = MC68302_IPR_PB11 | MC68302_IPR_SCC1 |
	   MC68302_IPR_SDMA | MC68302_IPR_SCC2 | MC68302_IPR_T1 |
	       MC68302_IPR_T2 | MC68302_IPR_SCC3;
   }

   /*
    * Init the SID
    */

   ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_TEM,
			    ds->last_sid); /* TE Master mode */
   ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_EID,
			    ds->last_sid); /* EOM Interrupt Disabled */
   ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_DIF3,
			    ds->last_sid);/* DSI Format 3 (GCI) */
   ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_B1E, ds->last_sid);
   ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_B2E, ds->last_sid);

   /*
    * Check for Loopback Mode
    */
   if (dchan_idb->loopback) {
      loopback = TRUE;
      ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_LBL1,
			       ds->last_sid);
      ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_LBL2,
			       ds->last_sid);
      ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_PUP,
			       ds->last_sid);  /* Power up */
   } else {
      loopback = FALSE;
      ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_CAL,
			       ds->last_sid);  /* Clear Loopback */

      if ((dchan_idb->state != IDBS_ADMINDOWN) && (!ISBRILEASED(dchan_idb) ||
			  (is_layer1_current_state_activated(dchan_idb))))
		  net_cstate(dchan_idb, IDBS_UP);
   }

	/*
	 * If this interface has been configure for leased line ISDN then enable
	 * the interface and both B channels.
	 */
	if(ISBRILEASED(dchan_idb)){
		(*isdn_info->activate_line)(dchan_idb);
		(*isdn_info->enable_bri_channel)(dchan_idb->isdn_info->b1_chan_idb);
		(*isdn_info->enable_bri_channel)(dchan_idb->isdn_info->b2_chan_idb);
	}

   reset_interrupt_level(SR);                 /* Restore interrupt level */
   return;                         
}

/*
 * =====================================================================
 * bri_enable()
 *
 * Description:
 * Enable the BRI interface, most likely shut down for input congestion.
 * Please note that this function must be called with NETS_DISABLED to
 * work correctly.
 *
 * Parameters:
 * idb - the idb for the bri channel
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_enable (hwidbtype *idb)
{
   if (idb->type == IDBTYPE_BRI)
       idb_mark_unthrottled(idb);
}


/*
 * bri_shutdown()
 * Shutdown the BRI Interfaces.
 */
static void bri_shutdown(hwidbtype *in_idb, int newstate)
{
   hwidbtype *idb;
   bri_instance_t *ds;
   leveltype SR;

   idb = in_idb->isdn_info->d_chan_idb;
   ds = INSTANCE;

   idb->state = newstate;
   GET_TIMESTAMP(idb->state_time);

   SR = raise_interrupt_level(NETS_DISABLE);
   if (ds) {
      bri_stop(ds->config_reg);
      bri_reset(idb);
   }
   /* 
    * Tell ISDN code about deactivation and take down the line.
    * Tell it only if we are changing state. Sending the primative
    * before the pdn because it is interferes with interrupts.
    */
   if (isdn_process_up) {
           if (activated != DEACTIVATED) {
   	       bri_enq_prim(in_idb->unit, ISDN_PH_DEACT_IND);
	       if (bri_debug)
	           buginf("\nBRI: Deactivating in bri_reset().\n");
	   }
   }

   activated = DEACTIVATED;
   reset_interrupt_level(SR);

}


/*
 * bri_setup_bridging()
 * Setup bridging on the interface.
 */
void bri_setup_bridging (hwidbtype *idb)
{
   bri_init(idb, FALSE);   /* Reset code will set it up */
   idb->counters.resets--;
}


/*
 * read_bri_errors()
 * Read the reported errors for the BRI hardware
 */
static void read_bri_errors (hwidbtype *idb)
{

   bri_instance_t *ds;
   leveltype SR;
   volatile param_ram_scc_t *param_ram;      /* '302 paramater ram */
   volatile mcc68302_system_t *system_ram;   /* '302 system ram */
   volatile prot_specific_t *prot_spec;      /* '302 protocol specefic area */

   /*
    * Set up pointers
    */
   ds = INSTANCE;
   system_ram = ds->config_reg;
   
   if (ds) {

      /*
       * get correct param RAM
       */
      switch (idb->isdn_info->chan_num) {
         case BRI_D_CHAN:
            param_ram = &(system_ram->scc3_param);
	    break;
	 case BRI_B1_CHAN:
	    param_ram = &(system_ram->scc1_param);
	    break;
         case BRI_B2_CHAN:
	    param_ram = &(system_ram->scc2_param);
	    break;
	 default:
	    return;
      }

      prot_spec = &(param_ram->prot_spec);

      /*
       * Poll the mc68302 param ram for error counts
       */
      SR = raise_interrupt_level(NETS_DISABLE);

      if (prot_spec->crc_err_cnt) {
	 idb->counters.input_error += prot_spec->crc_err_cnt;
	 idb->counters.input_crc += prot_spec->crc_err_cnt;
	 prot_spec->crc_err_cnt = 0;
	 MAYBE_DROPPED(idb);
      }
      if (prot_spec->abort_seq_cnt) {
	 idb->counters.input_error += prot_spec->abort_seq_cnt;
	 idb->counters.input_abort += prot_spec->abort_seq_cnt;
	 prot_spec->abort_seq_cnt = 0;
	 MAYBE_DROPPED(idb);
      }
      if (prot_spec->frame_retrans_cnt) {
	 idb->counters.output_error += prot_spec->frame_retrans_cnt;
	 idb->counters.output_collision += prot_spec->frame_retrans_cnt;
	 prot_spec->frame_retrans_cnt = 0;
	 MAYBE_DROPPED(idb);
      }
      reset_interrupt_level(SR);
   }
}


/*
 * bri_periodic()
 * Periodic poll.
 */
static void bri_periodic (hwidbtype *in_idb)
{
   hwidbtype *idb;
   bri_instance_t *ds;
   volatile mcc68302_system_t *system_ram;
   leveltype SR;
   ushort scm1, scm2, scm3;
   ushort scon1, scon2, scon3;
   uint scr;

   idb = in_idb->isdn_info->d_chan_idb;
   ds = INSTANCE;

   if (ds) {

      system_ram = ds->config_reg;

      /*
       * Read the error registers.
       */
      read_bri_errors(idb);
      read_bri_errors(idb->isdn_info->b1_chan_idb);
      read_bri_errors(idb->isdn_info->b2_chan_idb);

      if (idb->state == IDBS_UP) {
	 SR = raise_interrupt_level(NETS_DISABLE);
	 scm1 = system_ram->scc_internal_reg.scm1;
	 scm2 = system_ram->scc_internal_reg.scm2;
	 scm3 = system_ram->scc_internal_reg.scm3;

	 scon1 = system_ram->scc_internal_reg.scon1;
	 scon2 = system_ram->scc_internal_reg.scon2;
	 scon3 = system_ram->scc_internal_reg.scon3;

	 scr = *(uint *)MC68302_SCR;

	 reset_interrupt_level(SR);

	 /*
	  * Check the SCONs
	  */
	 if (((scon1 & (ushort)SCON_NORMAL) != (ushort)SCON_NORMAL)||
	     ((scon2 & (ushort)SCON_NORMAL) != (ushort)SCON_NORMAL)||
	     ((scon3 & (ushort)SCON_NORMAL) != (ushort)SCON_NORMAL)) {
	    if (bri_debug) {
	       buginf("\nBRI(%d): Bad state in SCON's", idb->unit);
	       buginf("\nscon1 = %x scon2 = %x scon3 = %x\n", scon1, scon2,
		      scon3);
	    }
	    bri_init(idb, FALSE);
	 }

	 /*
	  * Check the SCR
	  */
	 if ((scr & (uint)SCR_NORMAL) != (uint)SCR_NORMAL) {
	    if (bri_debug)
	       buginf("\nBRI(%d): Bad state in SCR. SCR = %x\n", idb->unit, 
		      scr);
	    bri_init(idb, FALSE);
	 }
      }
   }
}

/*
 * bri_show_int_enhanced
 *
 * display any network terminations to the BRI interface on a 2524 or 2525 (Boa).
 */

void bri_nt1_show_int_enhanced (void)
{
    printf("Integrated NT1 for ISDN Basic Rate interface\n");
}

/*
 * bri_nt1_init_namestring
 * Initialize the NT1 type string for "show interface"
 * Assumes only 1 BRI interface on 2500s
 */
void bri_nt1_init_namestring (void)
{
    hwidbtype *idb;

    /*
     * Initialize the D-channel hwidbs typestring to indicate NT
     */
    FOR_ALL_HWIDBS(idb)
        if (idb->type == IDBTYPE_BRI)
            idb->typestring = "BRI with integrated NT1";
}
          
void bri_nt1_show_module_type (void)
{
    if ((cookie->interfaces == INTERFACE_1E1B2W) ||
        (cookie->interfaces == INTERFACE_1R1B2W)) {
        if (BOA_BRI_NT1_PRESENT) {
            printf(" with integrated NT1\n");
            return;
        }
    }
    printf("\n");
}


/*
 * init_bri_idb()
 * Initialize the idb structures. Specific to the low-end.
 */
static void init_bri_idb (void)
{
    hwidbtype *dchan_idb;
    
    /* 
     * Write the 68302 BAR and SCR
     */
    *(ushort *)MC68302_BAR = MC68302_BAR_BASE;
    *(uint *)MC68302_SCR = MC68302_SCR_VGE | MC68302_SCR_WPVE | 
	MC68302_SCR_LPREC | MC68302_SCR_LPP16 | MC68302_SCR_LPEN | 
	    MC68302_SCR_LPCD;
    
    dchan_idb = init_bri_idb_common(0);/* The unit # is 0 for low-end.*/
    if (dchan_idb == NULL)
	return;

    /*
     * Intialize the interface
     */
    bri_init(dchan_idb, FALSE);
    dchan_idb->counters.resets = 0;
    dchan_idb->isdn_info->b1_chan_idb->counters.resets = 0;
    dchan_idb->isdn_info->b2_chan_idb->counters.resets = 0;
}


/*
 * analyze_bri()
 * Discover the BRI chips
 */
static uint analyze_bri (void)
{
  int bri_count;

  bri_count = count_bri_hardware();
  if (bri_count == 0)
    return(0);

  bri_init_isdn_queue();      /* Enable layer 1 Q to isdn */
  init_bri_idb();
  bri_parser_init();

  /*
   * Platform specific BRI initialization
   */
  switch (cookie->interfaces) {
      case INTERFACE_1E1B2W:
      case INTERFACE_1R1B2W:
          /*
           * If the NT1 module is present, init some enhanced
           * show command registries
           */
           if (BOA_BRI_NT1_PRESENT) {
               bri_nt1_init_namestring();
               reg_add_show_int_enhanced(bri_nt1_show_int_enhanced,
                                         "bri_nt1_show_int_enhanced");
           }
	   break;
	default:
	   break;
  }
	
  return(bri_count);
}


/*
 * bri_show_bd()
 * Show a buffer descriptor entry.
 */
static boolean bri_show_bd (paktype *pak, bri_buf_descr_t *bd, int n)
{
   uchar *data_start;

   data_start = bd->buffer;
   printf("%02d pak=0x%06x ds=0x%06x status=%02x pak_size=%d\n",
	  n, pak, data_start, bd->stat_control, bd->data_len);

   if (pak && ((uint)data_start <= (uint)pak)) {
      printf("Error: datagramstart <= pak\n");
   }

   if ((uint)data_start & 1) {
      printf("Error: datagramstart is an odd address\n");
   }
   return(FALSE);
}


/*
 * bri_show_errors()
 * Show BRI errors.
 */
static void bri_show_errors (hwidbtype *idb)
{
   bri_instance_t *ds = INSTANCE;

   printf("%d missed datagrams, %d overruns, %d bad frame addresses\n",
	  idb->counters.input_resource, idb->counters.input_overrun,
	  ds->bad_frame_addr);
   printf("%d bad datagram encapsulations, %d memory errors\n",
	  idb->unknown_encaps, idb->dma_memory_error);
   printf("%d transmitter underruns\n",idb->counters.output_underrun);
   printf("%d d channel collisions\n", ds->d_chan_coll);
}


/*
 * bri_show_controller()
 * Show BRI status.
 */
static boolean bri_show_controller (hwidbtype *in_idb, parseinfo *csb)
{
   bri_instance_t *ds;
   int i, j;
   hwidbtype *idb;

   if ((in_idb->isdn_info == NULL) ||
       (in_idb->isdn_info->chantype != ISDN_Dchan))
       return(FALSE);

   idb = NULL;

   for (j = 0; j < BRI_MAX_CHAN; j++) {
      switch (j) {
         case BRI_D_CHAN:
	    idb = in_idb->isdn_info->d_chan_idb;
	    printf("BRI unit %d", idb->unit);
            /* If this is a c2524/2525 and a NT1 is present, say so */
            bri_nt1_show_module_type();
	    printf("D Chan Info:\n");
	    break;
	 case BRI_B1_CHAN:
	    idb = in_idb->isdn_info->b1_chan_idb; 
	    printf("B1 Chan Info:\n");
	    break;
	 case BRI_B2_CHAN:
	    idb = in_idb->isdn_info->b2_chan_idb; 
	    printf("B2 Chan Info:\n");
	    break;
	 default:
	    break;
      }

      ds = INSTANCE;

      if (ds) {
	 char rx_head, tx_count, tx_head, tx_tail;
	 bri_buf_descr_t *bri_rx_ring;
	 bri_buf_descr_t *bri_tx_ring;
	 bri_buf_descr_t *temp_ring;
	 paktype *rx_p[MAX_RING];
	 paktype *tx_p[MAX_RING];
	 leveltype SR;

	 /*
	  * Display the Layer 1 BRI status
	  */
	 if (activated == DEACTIVATED) 
	     printf("Layer 1 is DEACTIVATED\n");
	 else {
	     if (activated == ACTIVATED)
	         printf("Layer 1 is ACTIVATED\n");
	     else
	         printf("Layer 1 is PENDING ACTIVATION\n");
	 }

	 printf("idb %#x, ds %#x, reset_mask %#x\n",
		idb, ds, ds->reset_mask);

	 printf("buffer size %d\n", idb->max_buffer_size);

	 /*
	  * Allocate memory for the RX and TX rings:
	  */
	 bri_rx_ring = malloc(BRI_RING_SIZE);
	 if (!bri_rx_ring) {
	    return(TRUE);
	 }
	 bri_tx_ring = malloc(BRI_RING_SIZE);
	 if (!bri_tx_ring) { 
	    free(bri_rx_ring);
	    return(TRUE);
	 }
      
	 /*
	  * Disable network interrupts and take a snapshot of the ring state:
	  */
	 SR = raise_interrupt_level(NETS_DISABLE); 
	 rx_head = ds->rx_head;
	 bcopy(ds->rx_r, bri_rx_ring, (MAX_RING * sizeof(bri_buf_descr_t))); 
	 for (i = 0; i < ds->rx_size; i++)
	    rx_p[i] = ds->rx_p[i];
	 tx_count = ds->tx_count;
	 tx_head = ds->tx_head;
	 tx_tail = ds->tx_tail;
	 bcopy(ds->tx_r, bri_tx_ring, (MAX_RING * sizeof(bri_buf_descr_t)));
	 for (i = 0; i < ds->tx_size; i++)
	    tx_p[i] = ds->tx_p[i];
	 reset_interrupt_level(SR);

	 printf("RX ring with %d entries at 0x%06x : Rxhead %d\n",
		ds->rx_size, ds->rx_r, rx_head);
	 temp_ring = bri_rx_ring;
	 for (i = 0; i < ds->rx_size; i++) {
	    if (bri_show_bd(rx_p[i], (bri_buf_descr_t *)temp_ring, i)) {
	       free(bri_rx_ring);
	       free(bri_tx_ring);
	       return(TRUE);
	    }
	    temp_ring++;
	 }
	 free(bri_rx_ring);

	 printf("TX ring with %d entries at 0x%06x: ", ds->tx_size, ds->tx_r);

	 printf("tx_count = %d, tx_head = %d, tx_tail = %d\n",
		tx_count, tx_head, tx_tail);

	 temp_ring = bri_tx_ring;
	 for (i = 0; i < ds->tx_size; i++) {
	    if (bri_show_bd(tx_p[i], (bri_buf_descr_t *)temp_ring, i)) {
	       free(bri_tx_ring);
	       return(TRUE);
	    }
	    temp_ring++;
	 }
	 free(bri_tx_ring);
	 bri_show_errors(idb);

      } else
	 printf(not_initialized);
   }

   return(FALSE);
}


/*
 * set_bri_bandwidth()
 * Set the bandwidth on the B channel
 * parameters are B channel idb and 64kb or 56kb
 */
static void c3000_set_bri_bandwidth (hwidbtype *idb, uint bri_bandwidth)
{
   bri_instance_t *ds;
   mcc68302_system_t *mcc68302_reg;
   uint bri_bandwidth_mask = 0;
   ushort temp_mask;


   ds = INSTANCE;
   mcc68302_reg = (mcc68302_system_t *)(ds->config_reg);
   temp_mask = mcc68302_reg->scc_internal_reg.simask;

   if (ds) {
       if (bri_bandwidth == 64) {
           bri_bandwidth_mask = MC68302_64KB_MASK;
	   set_default_interface_bandwidth(idb->firstsw,
					   VISIBLE_BRI_BANDWIDTH_64);
       } else if (bri_bandwidth == 56) {
           bri_bandwidth_mask = MC68302_56KB_MASK;
	   set_default_interface_bandwidth(idb->firstsw,
					   VISIBLE_BRI_BANDWIDTH_56);
       } else {
           return;
       }

       if (idb->isdn_info->chan_num == BRI_B1_CHAN) {
	   temp_mask &= 0xFF00;
	   mcc68302_reg->scc_internal_reg.simask = 
	       (temp_mask | (uchar)bri_bandwidth_mask);
       } else if (idb->isdn_info->chan_num == BRI_B2_CHAN) {
	   temp_mask &= 0x00FF;
	   mcc68302_reg->scc_internal_reg.simask = 
	       (temp_mask | ((uchar)bri_bandwidth_mask << 8));
       }
   }
}


/*
* isdn_enable_bri_channel()
* Enable the TX and RX of a B channel
* to reduce the CRC and Abort errors before connection
*/
static void c3000_isdn_enable_bri_channel (hwidbtype *bchan_idb)
{
   mcc68302_system_t *system_ram;    /* Pointer to base of '302 system ram */
   volatile scc_internal_reg_t *mc68302_scc_int_reg; /* SCC Internal Reg, */
   leveltype SR;
   enum Chan bchan;

   bchan = bchan_idb->isdn_info->chantype;
 
   if (bri_debug)
       buginf("\nBRI: %s channel %s", 
               "enable",
	       (bchan == B1) ? "B1" : "B2");
   SR = raise_interrupt_level(NETS_DISABLE);
   system_ram = (mcc68302_system_t *)MC68302_BASE;
   mc68302_scc_int_reg = &(system_ram->scc_internal_reg);

   if (bchan == B1) 
       mc68302_scc_int_reg->scm1 = MC68302_SCM_ENR | MC68302_SCM_ENT |
         MC68302_SCM_FLG; /* Send Flags (0x7E) between frames */
   else if(bchan == B2)
       mc68302_scc_int_reg->scm2 = MC68302_SCM_ENR | MC68302_SCM_ENT |
         MC68302_SCM_FLG; /* Send Flags (0x7E) between frames */
   reset_interrupt_level(SR);
}


/*
* isdn_disable_bri_channel()
* Disable the TX and RX of a B channel
* to reduce the CRC and Abort errors before connection
*/
static void c3000_isdn_disable_bri_channel (hwidbtype *bchan_idb)
{
   mcc68302_system_t *system_ram;    /* Pointer to base of '302 system ram */
   volatile scc_internal_reg_t *mc68302_scc_int_reg; /* SCC Internal Reg, */
   enum Chan bchan;
   leveltype SR;
 
   bchan = bchan_idb->isdn_info->chantype;
   if (bri_debug)
       buginf("\nBRI: %s channel %s", 
               "disable",
	       (bchan == B1) ? "B1" : "B2");
   SR = raise_interrupt_level(NETS_DISABLE);
   system_ram = (mcc68302_system_t *)MC68302_BASE;
   mc68302_scc_int_reg = &(system_ram->scc_internal_reg);
 
   if (bchan == B1)
       mc68302_scc_int_reg->scm1 = 0;
   else if (bchan == B2)
            mc68302_scc_int_reg->scm2 = 0;
   reset_interrupt_level(SR);
}


/*
 * isdn_activate_line()
 * Activate the ISDN line
 */
static void c3000_isdn_activate_line (hwidbtype *idb)
{
   bri_instance_t *ds;                      /* Pointer to instance */
   mcc68302_system_t *mcc68302_system_ram;  /* '302 system ram */
   leveltype SR;                      /* Interrupt level */
   volatile internal_reg_t *mc68302_int_reg; /* '302 internal reg */

   if (activated == DEACTIVATED) {

       ds = INSTANCE;
       mcc68302_system_ram = ds->config_reg;
       mc68302_int_reg = &(mcc68302_system_ram->internal_reg);

       SR = raise_interrupt_level(NETS_DISABLE);
       if (!loopback) {
           if(!ISBRILEASED(idb))
               bri_init(idb, FALSE);

           ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_CAL,
				    ds->last_sid);  /* Clear Loopback */
       } else {
           ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_LBL1,
				    ds->last_sid);  /* set loopback B1 */
           ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_LBL2,
				    ds->last_sid);  /* set loopback B2 */
       } 
       ds->last_sid = write_sid(mcc68302_system_ram, TP3420_CRF_PUP,
				ds->last_sid);  /* Power up */
  
       /*
	* MUST wait 2 ms after PUP to send Activation Request.
	*/
       activated = PENDING;
       mc68302_int_reg->timer_trr1 = TIMER_2MS;         /* Set 2 ms timer */
       mc68302_int_reg->timer_tmr1 = 0x001B;            /* Start timer */
       reset_interrupt_level(SR);
   }
}


/*
 * bri_restart()
 * Restart the BRI interface.
 */
void bri_restart (hwidbtype *idb)
{
    if ((idb->type != IDBTYPE_BRI) ||
	(idb->isdn_info->chantype != ISDN_Dchan))
	return;
    
   bri_init(idb, FALSE);
   c3000_isdn_activate_line(idb);
}


/*
 * =====================================================================
 * isdn_bri_enable_layer1_Q - This function is called by isdn once a
 * switch type is configured. This will make sure that the layer1 will
 * start queueing primitives only after isdn process is spawned. The
 * interrupts are enabled here for the first time.
 *
 * Parameters: none.
 * Returns:    nothing.
 * Notes:      The interrupts for the MC68302 are enabled here for the
 *             first time. This function will be  called only one time.
 *             Do not make assumptions about shutdown/reset etc.
 * =====================================================================
 */
void isdn_bri_enable_layer1_Q (void)
{
   bri_instance_t *ds;                /* instance */
   hwidbtype *idb;		      /* idb ptr */
   mcc68302_system_t *mcc68302_system_ram;  /* '302 system ram */
   volatile internal_reg_t *mc68302_int_reg; /* '302 internal reg */


   FOR_ALL_HWIDBS(idb) {
       if ((idb->type == IDBTYPE_BRI) &&
	   (idb->isdn_info->chantype == ISDN_Dchan) &&
	   idb->state != IDBS_ADMINDOWN) {
	   ds = INSTANCE;

	   /* 
	    * Set up 68302 registers
	    */
	   mcc68302_system_ram = ds->config_reg;
	   
	   /* 
	    * Internal registers
	    */
	   mc68302_int_reg = &(mcc68302_system_ram->internal_reg);
	   /*
	    * For the first time, enable all interrupts.
	    */
	   mc68302_int_reg->int_imr = MC68302_IPR_PB11 | MC68302_IPR_SCC1 |
	       MC68302_IPR_SDMA | MC68302_IPR_SCC2 | MC68302_IPR_T1 |
		   MC68302_IPR_T2 | MC68302_IPR_SCC3;
       }
   }
	   
   /*
    * flag for future init routines needing to know about the
    * isdn process's existence.
    */
   isdn_process_up = TRUE;	
}


/*
 * =====================================================================
 * bri_txqlength()
 *
 * Description:
 * Return the number of frames on the transmit ring that are
 * waiting to be transmitted.
 *
 * Parameters:
 * idb
 *
 * Returns:
 * count of frames on the transmit ring
 * =====================================================================
 */
static int bri_txqlength (hwidbtype *idb)
{
    bri_instance_t *ds = INSTANCE;

    return(ds->tx_count);
}


/* bri_fastsend
 * Fastswitch a packet out a BRI channel.
 */
static void bri_fastsend (hwidbtype *idb, paktype *pak)
{
    bri_instance_t *ds;
    bri_buf_descr_t *tmd;
    traffic_shape_t *shape;
    ushort bytes, ring_index;
    ushort wrap_bit;

    if (idb->state != IDBS_UP) {
	idb->counters.output_total_drops++;
	datagram_done(pak);
	return;
    }

    ds = INSTANCE;

    /*
     * determine whether there is window available to send the message with
     * in the rate control bucket
     */
    shape = traffic_shape_required_inline(pak->if_output->traffic_shape, pak);
    if (shape) {
	traffic_shape_defer_sending(shape, pak);
	return;
    }

    /*
     * If no packets are queued for output on this interface, and there is
     * room for another packet in the output ring, immediately transmit the 
     * packet.
     */
    if (!idb->output_qcount && (ds->tx_count < ds->tx_size)) {
	bytes = pak->datagramsize;

	/*
	 * Get pointers for the TX ring entry at tx_tail.
	 */
	ring_index = ds->tx_tail++;
	if (ds->tx_tail >= ds->tx_size)
	    ds->tx_tail = 0;
	tmd = ds->tx_r + ring_index;

	ds->tx_p[ring_index] = pak;
	ds->tx_count++;

	/*
	 * Set up this TX ring entry for this packet.
	 */
	tmd->buffer = pak->datagramstart;
	tmd->data_len = bytes;
	if (ring_index == (ds->tx_size - 1))
	    wrap_bit = MC68302_TCS_W;
	else
	    wrap_bit = 0;
	tmd->stat_control = MC68302_TCS_R | MC68302_TCS_X | wrap_bit |
				MC68302_TCS_I | MC68302_TCS_L | MC68302_TCS_TC;

	/*
	 * Do packet accounting after starting the 68302.
	 */
	idb->counters.tx_cumbytes_fast[pak->acct_proto] += bytes;
	idb->counters.outputs_fast[pak->acct_proto]++;
	TIMER_START(idb->xmittimer, TIMEOUT);

	/*
	 * Count broadcasts.
	 */
	if ((pak->enctype == ET_HDLC) &&
	    ((*(ushort *)pak->datagramstart) & HDLC_BCAST))
	    idb->counters.output_broad++;
    } else {

	/*
	 * There are already packets on the output hold queue or the
	 * transmit ring is full, so add this one to the end of the 
	 * queue to minimize out of order situations.
	 */
	holdq_enqueue_fast(idb, pak);
	bri_tx_start(idb);
    }

}


/*
 * bri_init_idb_vectors()
 * Set the idb vectors.
 */
void bri_init_idb_vectors (hwidbtype *idb)
{
      /*
       * Set up function calls
       */
      idb->setencap = serial_encapsulation;
      idb->soutput = bri_tx_start;
      idb->reset = bri_restart;
      idb->shutdown = bri_shutdown;
      idb->enable = bri_enable;
      idb->setup_bridging = NULL;
      idb->periodic = NULL;
      if (idb->isdn_info->chan_num == BRI_D_CHAN) {
          idb->device_periodic = bri_periodic;
      }
      else {
          idb->device_periodic = NULL;
      }
      idb->clear_support = NULL;
      idb->show_controller = bri_show_controller;
      idb->txqlength = bri_txqlength;
      idb->system_configured = bri_system_configured;
      idb->fastsend = bri_fastsend;
      idb->isdn_info->activate_line = c3000_isdn_activate_line;
      idb->isdn_info->disable_bri_channel = c3000_isdn_disable_bri_channel;
      idb->isdn_info->enable_bri_channel = c3000_isdn_enable_bri_channel;
      idb->isdn_info->set_bri_bandwidth = c3000_set_bri_bandwidth;
}


/*
 * =====================================================================
 * bri_system_configured()
 *
 * Description:
 * Reinitialize the BRI interfaces:  Since the ISDN process is
 * started until after the driver process the isdn_cstate does not
 * get set since the cstate registry has not been added yet.
 *
 * Parameters:
 * hwidbtype *idb
 *
 * Returns:
 * nothing
 * =====================================================================
 */
static void bri_system_configured (hwidbtype *idb)
{
   if (idb->enctype == ET_HDLC)
      bri_init(idb, FALSE);
}
        

/*
 * =====================================================================
 * bri_subsys_init -
 *
 * Description: BRI subsystem initialisation entry point.
 *
 * Parameters:
 * subsys	: ptr to subsystem type.
 *
 * Returns:
 * nothing.
 * 
 * =====================================================================
 */
static void bri_subsys_init (subsystype *subsys)
{
    /*
     * Discover all the BRI serial interfaces in system
     */
    nbrinets += analyze_bri();

	/*
	 * Leased Line initialisation registry
	 */
	reg_add_isdn_leased_line(init_bri_leased_line, "init_bri_leased_line");
}

/*
 * BRI subsystem header
 */
#define BRI_MAJVERSION   1
#define BRI_MINVERSION   0
#define BRI_EDITVERSION  1

SUBSYS_HEADER(bri, BRI_MAJVERSION, BRI_MINVERSION, BRI_EDITVERSION,
	      bri_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

/*
 * Return TRUE is the BRI interface is in state ACTIVATED. FALSE is in any
 * other state. Added for Leased Line ISDN feature.
 */
boolean is_layer1_current_state_activated (hwidbtype *hwidb)
{
	return((activated == ACTIVATED) ? TRUE : FALSE);
}
