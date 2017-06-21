/* $Id: if_as5200_tdm.c,v 3.1.72.3 1996/08/28 12:57:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/if_as5200_tdm.c,v $
 *------------------------------------------------------------------
 * if_as5200_tdm.c -- Driver for Mitel TDM chip for AS5200 platform.
 *
 * October 1995, Joe Turner
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_as5200_tdm.c,v $
 * Revision 3.1.72.3  1996/08/28  12:57:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.72.2  1996/07/02  23:55:17  jturner
 * CSCdi61176:  post-brasil-commit code cleanup in tdm, asm, src and
 * makefile
 * Branch: California_branch
 *
 * Revision 3.1.72.1  1996/06/16  21:15:00  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.1  1995/12/03  21:26:48  jturner
 * Placeholder for Brasil.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Includes
 */
#include "master.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "logger.h"
#include "../les/msg_tdm.c"		/* Not a typo, see logger.h */
#include "if_les.h"
#include "if_as5200_tdm.h"

/*
 * tdm_verify_init(ch_info_t *dev_ch)
 *	- Verify that all channels of a given TDM are tristated.
 * 	This routine is not inherently necessary for switching and
 * 	should only be called as part of subsytem initialization.
 */
static boolean
tdm_verify_init (ch_info_t *dev_ch)
{
    int stream, channel;
    tdm_regs *tdm;
    boolean initpass;
 
    initpass = TRUE;
    /*
     * Set up pointer to tdm registers
     */
    tdm = tdm_getregs(dev_ch);

    /*
     * Verify all channels are tristated.
     * If not, errmsg and return FALSE.
     */
    for (stream = 0; stream < TDM_MAX_STREAMS; stream++) {
	tdm->control_reg = (TDM_CONNECTION_MEMORY_HIGH | stream);
	for (channel = 0; channel < TDM_MAX_CHANNELS; channel++) {
	    /* If enabled, the device failed to tristate the channel */
	    if ((tdm->channel[channel] & TDM_MASK) != TDM_CH_OUTPUT_DISABLE) {
		initpass = FALSE;
		if (dev_ch->slot == TDM_MOTHERBOARD_SLOT) {
        	    errmsg(&msgsym(TDM_MB_INITFAIL, TDM), stream, channel,
			tdm->channel[channel], tdm->control_reg);
		} else {
		    errmsg(&msgsym(TDM_INITFAIL, TDM), dev_ch->slot, stream,
			channel, tdm->channel[channel], tdm->control_reg);
		}
	    }
	}
    }
    return(initpass);
}

/*
 * tdm_getregs(ch_info_t *tdm)
 * Set up pointers to tdm registers
 */
tdm_regs *tdm_getregs (ch_info_t *tdm)
{
    tdm_regs *tdm_device;

    if (tdm->slot == TDM_MOTHERBOARD_SLOT) {
        tdm_device = (tdm_regs *)ADRSPC_BRASIL_MB_TDM_REG;
    } else {
        tdm_device = TDM_ADDRESS(tdm->slot);
    }
    return(tdm_device);
}

/*
 * tdm_open_connection(tdm_regs *tdm, int sti, int sto, int ch_in, int ch_out)
 *	- Program tdm device to open a connection.
 */
static void
tdm_open_connection (tdm_regs *tdm, int sti, int sto, int ch_in, int ch_out)
{
    if (tdm_debug) {
        buginf("\nTDM(reg: 0x%x): Connect STi%d, channel %d to "
	"STo%d, channel %d\n", tdm, sti, ch_in,  sto, ch_out);
    }

    tdm->control_reg = (TDM_CONNECTION_MEMORY_LOW | sto);
    tdm->channel[ch_out] = ((sti << TDM_CMEM_L_ST_SHIFT) | ch_in);
    tdm->control_reg = (TDM_CONNECTION_MEMORY_HIGH | sto);
    tdm->channel[ch_out] = TDM_CH_OUTPUT_ENABLE;
}

/*
 * tdm_close_connection(tdm_regs *tdm, int stream, int channel)
 *	- Tristate a timeslot.
 */
static void
tdm_close_connection (tdm_regs *tdm, int stream, int channel)
{
    if (tdm_debug) {
        buginf("\nTDM(reg: 0x%x): Close connection to STo%d, channel %d\n",
		tdm, stream, channel);
    }

    tdm->control_reg = (TDM_CONNECTION_MEMORY_HIGH | stream);
    tdm->channel[channel] = TDM_CH_OUTPUT_DISABLE;
}

/*
 * tdm_tristate_channel(ch_info_t *dev_ch)
 *	- Program tdm device to close a connection.
 */
static void
tdm_tristate_channel (ch_info_t *dev_ch)
{
    tdm_regs *tdm;
    /*
     * Set up pointer to tdm registers
     */
    tdm = tdm_getregs(dev_ch);

    tdm_close_connection(tdm, dev_ch->stream, dev_ch->channel);
}

/*
 * tdm_ode_enable(ch_info_t *dev_ch)
 *      - Enable the TDM by reseting the TDM ODE bit.
 */
static void
tdm_ode_enable (ch_info_t *dev_ch)
{
    tdm_regs *tdm;
 
    /*
     * Set up pointer to tdm registers
     */
    tdm = tdm_getregs(dev_ch);

    /*
     * Enable output
     */
    tdm->ode_reg = TDM_ODE_ENABLE;
}

/*
 * tdm_connect_dc(ch_info_t *modem_tx, ch_info_t *digit_collect)
 *      - TDM driver procedure specifically used to oneway connect
 *          a modem's tx line to the digit collector timeslot.
 */
void
tdm_connect_dc (ch_info_t *modem_tx, ch_info_t *digit_collect)
{
    tdm_regs *tdm_modem;

    if(tdm_debug)
	buginf("\ndigit collection connection requested.\n");

    /*
     * Validate stream types.
     */
    if (modem_tx->stream_type != TDM_MODEM_STREAM &&
	digit_collect->stream_type != TDM_DIGIT_COLLECT_STREAM)
	return;

    /*
     * Set up pointers to modem board tdm registers
     */
    tdm_modem = tdm_getregs(modem_tx);

    /*
     * Close the connection from the modem TX channel to the backplane
     *		then open the digit collector connection.
     * 		N.B. modem backplane stream determined by slot and
     *		digit collection is known to be on ST3.
     */
    tdm_close_connection(tdm_modem, modem_tx->slot, modem_tx->channel);

    /*
     * Connect up the digit collection channel.
     */
    if (digit_collect->unit == 0) {
        tdm_open_connection(tdm_modem, ST7, ST3,
	    modem_tx->channel, TDM_DC1_BP_CH);
    } else if (digit_collect->unit == 1) {
        tdm_open_connection(tdm_modem, ST7, ST3,
	    modem_tx->channel, TDM_DC2_BP_CH);
    }
}

/*
 * tdm_disconnect_dc(ch_info_t *modem_tx, ch_info_t *digit_collect)
 *      - TDM driver procedure specifically used to disconnect
 *          a modem's tx line to the digit collector timeslot.
 */
void
tdm_disconnect_dc (ch_info_t *modem_tx, ch_info_t *digit_collect)
{
    tdm_regs *tdm_modem;
 
    if(tdm_debug)
        buginf("\ndigit collection disconnect requested.\n");
 
    /*
     * Validate stream types.
     */
    if (modem_tx->stream_type != TDM_MODEM_STREAM &&
        digit_collect->stream_type != TDM_DIGIT_COLLECT_STREAM)
        return;

    /*
     * Set up pointer to modem tdm registers
     */
    tdm_modem = tdm_getregs(modem_tx);
 
    /*
     * Close connection to the digit collection channel.
     */
    if (digit_collect->unit == 0) {
	tdm_close_connection(tdm_modem, TDM_DC_STREAM, TDM_DC1_BP_CH);
    } else if (digit_collect->unit == 1) {
	tdm_close_connection(tdm_modem, TDM_DC_STREAM, TDM_DC2_BP_CH);
    } else {
        return;
    }

    /*
     * Make the default connection 
     */
    tdm_connect_special(DEFAULT_TX, modem_tx);
}

/*
 * tdm_connect_twoway(ch_info_t *tdm_in, ch_info_t *tdm_out)
 *	- IOS to TDM driver procedure for making a simple bi-
 *	diretional TDM connection.
 *
 * 	N.B. Brasil PRI-to-modem connections are "PRI board" centric.
 * 	Modem card connections are done at system initialization and
 * 	assumed for all connections.  Completing a connection
 * 	therefore defaults to PRI card tdm channel switching from
 * 	Framer/M32 to Framer/Backplane streams.  This is done in
 * 	order to reduce the number of channels searched for a
 *	given connection. The Mitel part is *not* a fast device.
 */
boolean
tdm_connect_twoway (ch_info_t *tdm_in, ch_info_t *tdm_out)
{
    tdm_regs *tdmin, *tdmout;
 
#ifdef TDM_DEBUG
    SAVE_CALLER();
    buginf("\nTDM: two_connect_twoway called from 0x%x", caller());
#endif

    /*
     * Set up pointers to tdm registers
     */
    tdmin = tdm_getregs(tdm_in);
    tdmout = tdm_getregs(tdm_out);

    if(tdm_debug)
	buginf("\ntwo way connection requested.\n");

    /*
     * PRI/Modem switching
     */
    if (tdm_in->stream_type == TDM_PRI_STREAM &&
	tdm_out->stream_type == TDM_MODEM_STREAM) {
	/*
	 * If it's an E1 card modem connection, the
	 * backplane stream == ST3, else
	 * backplane stream == modem card slot
	 */
	if (BRASIL_BOARD_ID(tdm_in->slot) == BRASIL_E1_DPRI_PLUS_MODEMS) {
	    tdm_in->stream = ST3;
	} else {
	    tdm_in->stream = tdm_out->slot;
	}

	/*
	 * Close connections from PRI to M32
	 */
	tdm_close_connection(tdmin, (ST6 + tdm_in->unit), tdm_in->channel);
	tdm_close_connection(tdmin, (ST4 + tdm_in->unit), tdm_in->channel);

	/*
	 * Open connections between PRI card and backplane
	 */
        tdm_open_connection(tdmin, (ST6 + tdm_in->unit), tdm_in->stream,
			tdm_in->channel, tdm_out->channel);
        tdm_open_connection(tdmin, tdm_in->stream, (ST6 + tdm_in->unit),
			tdm_out->channel, tdm_in->channel);
	return(TRUE);
    } else if (tdm_in->stream_type == TDM_MODEM_STREAM &&
               tdm_out->stream_type == TDM_PRI_STREAM) {
	/*
	 * If it's an E1 card modem connection, the
	 * backplane stream == ST3, else
	 * backplane stream == modem card slot
	 */
	if (BRASIL_BOARD_ID(tdm_out->slot) == BRASIL_E1_DPRI_PLUS_MODEMS) {
	    tdm_out->stream = ST3;
	} else {
	    tdm_out->stream = tdm_in->slot;
	}

	/*
	 * Close connections from PRI to M32
	 */
	tdm_close_connection(tdmout, (ST6 + tdm_out->unit), tdm_out->channel);
	tdm_close_connection(tdmout, (ST4 + tdm_out->unit), tdm_out->channel);

	/*
	 * Open connections between PRI card and backplane
	 */
        tdm_open_connection(tdmout, (ST6 + tdm_out->unit), tdm_out->stream,
			tdm_out->channel, tdm_in->channel);
        tdm_open_connection(tdmout, tdm_out->stream, (ST6 + tdm_out->unit),
			tdm_in->channel, tdm_out->channel);
	return(TRUE);
    }

    /*
     * Modem-Modem testing
     */
    if (tdm_in->stream_type == TDM_MODEM_STREAM &&
	tdm_out->stream_type == TDM_MODEM_STREAM) {

	/*
	 * Back to back testing is only supported on a single card
	 */
	if (tdm_in->slot != tdm_out->slot)
	    return(FALSE);

	/*
	 * Close connections from modem stream to backplane stream.
	 * 	N.B. Backplane stream determined by slot number.
	 */
	tdm_close_connection(tdmin, tdm_in->slot, tdm_in->channel);
	tdm_close_connection(tdmin, ST7, tdm_in->channel);
	tdm_close_connection(tdmout, tdm_out->slot, tdm_out->channel);
	tdm_close_connection(tdmout, ST7, tdm_out->channel);

        tdm_open_connection(tdmin, ST7, ST7,
	    tdm_in->channel, tdm_out->channel);
        tdm_open_connection(tdmout, ST7, ST7,
	    tdm_out->channel, tdm_in->channel);
	return(TRUE);
    }

    /*
     * Call Switching: PRI/PRI switching
     * Not yet supported.
     */
    if (tdm_in->stream_type == TDM_PRI_STREAM &&
	tdm_out->stream_type == TDM_PRI_STREAM) {
	return(FALSE);
    }

    /*
     * Bad connection requested.
     */
    errmsg(&msgsym(TDM_BAD_CONNECT, TDM), tdm_in->slot, tdm_in->stream,
	tdm_in->channel, tdm_out->slot, tdm_out->stream, tdm_out->channel);
    return(FALSE);
}

/*
 * tdm_default_rx(ch_info_t *dev_ch, boolean tdm_init)
 *    Make a default channel RX connection based on stream type.
 *    If necessary, close old connections before restoring the default.
 */
static boolean
tdm_default_rx (ch_info_t *dev_ch, boolean tdm_init)
{
    tdm_regs *tdm_ptr;
    ushort cmem_low, old_stream, old_channel;

    if(tdm_debug)
	buginf("\ndefault RX connection requested.\n");

    /*
     * Setup tdm pointer.
     */
    tdm_ptr = tdm_getregs(dev_ch);

    /*
     * Make default connections based on stream_type.
     */
    switch (dev_ch->stream_type) {
	case TDM_TONE_STREAM:
	case TDM_MODEM_STREAM:
            /*
             * The RX initialization connection matrix for for the 24
             * modem card is as follows:
             *    STi[0:3] = Slot[0:3] -------> STo7: codec DR inputs
             */
            tdm_close_connection(tdm_ptr, ST7, dev_ch->channel);
	    tdm_open_connection(tdm_ptr, dev_ch->slot, ST7,
		dev_ch->channel, dev_ch->channel);
	    break;
	case TDM_PRI_STREAM:
            /*
             * The RX initialization connection matrix for for the DPRI
             * streams is as follows (STo [0:3] I/O to backplane):
             *       STi6: Framer1 -----> STo4: M32 #1
             *       STi7: Framer2 -----> STo5: M32 #2
             * Methodology: read connection memory low to determine old
             * connection; tristate old connection; make default connection.
             */
            if (tdm_init) {
	        /* Connect from digit collect STo3 to STi3 */
		tdm_open_connection(tdm_ptr, TDM_DC_STREAM, TDM_DC_STREAM,
		    TDM_DC1_BP_CH, TDM_DC1_CARD_CH);
		tdm_open_connection(tdm_ptr, TDM_DC_STREAM, TDM_DC_STREAM,
		    TDM_DC2_BP_CH, TDM_DC2_CARD_CH);
            } else {
                tdm_ptr->control_reg = (TDM_CONNECTION_MEMORY_LOW |
                                        (ST6 + dev_ch->unit));
                cmem_low = (tdm_ptr->channel[dev_ch->channel] & TDM_MASK);
                old_stream = ((cmem_low >> 5) & TDM_STREAM_ADDRESS_BITS);
                old_channel = (cmem_low & TDM_CHANNEL_BITS);
                tdm_close_connection(tdm_ptr, old_stream, old_channel);
	    }
            tdm_open_connection(tdm_ptr, (ST6 + dev_ch->unit),
                (ST4 + dev_ch->unit), dev_ch->channel, dev_ch->channel);
	    /* The E1 card modems need special care */
	    if (BRASIL_BOARD_ID(dev_ch->slot) == BRASIL_E1_DPRI_PLUS_MODEMS)
		tdm_close_connection(tdm_ptr, ST3, dev_ch->channel);
	    break;
	case TDM_DIGIT_COLLECT_STREAM:
	    /*
	     * Digit collection is a tx only type of connection
	     */
	    break;
	case TDM_ISDN_STREAM:
	default:
	    return(FALSE);
	    break;
    }
    return(TRUE);
}

/*
 * tdm_default_tx(ch_info_t *dev_ch, boolean tdm_init)
 *    Make a default channel TX connection based on stream type.
 *    If necessary, close old connections before restoring the default.
 */
static boolean
tdm_default_tx (ch_info_t *dev_ch, boolean tdm_init)
{
    tdm_regs *tdm_ptr;

    if(tdm_debug)
	buginf("\ndefault TX connection requested.\n");

    /*
     * Setup tdm pointer.
     */
    tdm_ptr = tdm_getregs(dev_ch);

    /*
     * Make default connections based on stream_type.
     */
    switch (dev_ch->stream_type) {
	case TDM_MODEM_STREAM:
            /*
             * The TX initialization connection matrix for for the 24
             * modem card is as follows:
             *   STi7: codec DX inputs -------> STo[0:3] = Slot[0:3]
             */
            tdm_close_connection(tdm_ptr, dev_ch->slot, dev_ch->channel);
	    tdm_open_connection(tdm_ptr, ST7, dev_ch->slot,
		dev_ch->channel, dev_ch->channel);
	    break;
	case TDM_PRI_STREAM:
            /*
             * The TX initialization connection matrix for DPRI
             * streams is as follows (ST0:3 I/O to backplane):
             *       STi4: M32 #1 -----> STo6: Framer1
             *       STi5: M32 #2 -----> STo7: Framer2
	     * We don't care about where the old connection came from,
	     * in this case just close it.
             */
            if (tdm_init) {
		/* Make tones connection */
		tdm_open_connection(tdm_ptr,
		    TDM_DIALTONE_STREAM, TDM_DIALTONE_STREAM,
		    TDM_DIALTONE_CARD_CH, TDM_DIALTONE_BP_CH);
		tdm_open_connection(tdm_ptr,
		    TDM_BUSYTONE_STREAM, TDM_BUSYTONE_STREAM,
		    TDM_BUSYTONE_CARD_CH, TDM_BUSYTONE_BP_CH);
	    } else {
		tdm_close_connection(tdm_ptr, (ST6 + dev_ch->unit), dev_ch->channel);
	    }
	    tdm_open_connection(tdm_ptr, (ST4 + dev_ch->unit),
		(ST6 + dev_ch->unit), dev_ch->channel, dev_ch->channel);
	    /* The E1 card modems need special care */
	    if (BRASIL_BOARD_ID(dev_ch->slot) == BRASIL_E1_DPRI_PLUS_MODEMS)
		tdm_close_connection(tdm_ptr, ST3, dev_ch->channel);
	    break;
	case TDM_DIGIT_COLLECT_STREAM:
	    /*
	     * Read out connections to digit collectors to determine
	     * which one we are connected to and close our connection.
	     */
	    tdm_ptr->control_reg = (TDM_CONNECTION_MEMORY_LOW | ST3);
	    if ((tdm_ptr->channel[TDM_DC1_BP_CH] & TDM_MASK) ==
		    ((ST7 << 5) | dev_ch->channel)) {
                tdm_close_connection(tdm_ptr, TDM_DC_STREAM, TDM_DC1_BP_CH);
	    }
	    if ((tdm_ptr->channel[TDM_DC2_BP_CH] & TDM_MASK) ==
		    ((ST7 << 5) | dev_ch->channel)) {
                tdm_close_connection(tdm_ptr, TDM_DC_STREAM, TDM_DC2_BP_CH);
	    }

	    /*
	     * Make the default connection
	     */
	    tdm_open_connection(tdm_ptr, ST7, dev_ch->slot,
		dev_ch->channel, dev_ch->channel);
	    break;
	case TDM_TONE_STREAM:
	    /*
	     * Tone stream is a rx only type of connection
	     */
	    break;
	case TDM_ISDN_STREAM:
	default:
	    return(FALSE);
	    break;
    }
    return(TRUE);
}

/*
 * tdm_default_rx_tx(ch_info_t *dev_ch, boolean tdm_init)
 *	Make default rx and tx connections.
 */
static boolean
tdm_default_rx_tx (ch_info_t *dev_ch, boolean tdm_init)
{
    return(tdm_default_rx(dev_ch, tdm_init) && tdm_default_tx(dev_ch, tdm_init));
}

/*
 * tdm_connect_busytone(ch_info_t *dev_ch)
 *	Connect given channel to busytone channel.
 */
static boolean
tdm_connect_busytone (ch_info_t *dev_ch)
{
    tdm_regs *tdm_ptr;

    if(tdm_debug)
	buginf("\nbusytone connection requested.\n");
    /*
     * Setup tdm pointer.
     */
    tdm_ptr = tdm_getregs(dev_ch);

    /*
     * Currently we only connect a channel from
     * a modem stream to the busytone.
     */
    switch (dev_ch->stream_type) {
	case TDM_TONE_STREAM:
	    if (BRASIL_BOARD_ID(dev_ch->slot) == BRASIL_E1_DPRI_PLUS_MODEMS) {
		tdm_open_connection(tdm_ptr, TDM_BUSYTONE_STREAM, ST3,
		    TDM_BUSYTONE_BP_CH, dev_ch->channel);
	    } else {
		tdm_close_connection(tdm_ptr, ST7, dev_ch->channel);
		tdm_open_connection(tdm_ptr, TDM_BUSYTONE_STREAM, ST7,
		    TDM_BUSYTONE_BP_CH, dev_ch->channel);
	    }
	    return(TRUE);
	    break;
	default:
	    return(FALSE);
	    break;
    }
}

/*
 * tdm_connect_dialtone(ch_info_t *dev_ch)
 *	Connect given channel to dialtone channel.
 */
static boolean
tdm_connect_dialtone (ch_info_t *dev_ch)
{
    tdm_regs *tdm_ptr;

    if(tdm_debug)
	buginf("\ndialtone connection requested.\n");
    /*
     * Setup tdm pointer.
     */
    tdm_ptr = tdm_getregs(dev_ch);

    /*
     * Currently we only connect a channel from
     * a modem stream to the busytone.
     */
    switch (dev_ch->stream_type) {
	case TDM_TONE_STREAM:
	    if (BRASIL_BOARD_ID(dev_ch->slot) == BRASIL_E1_DPRI_PLUS_MODEMS) {
		tdm_open_connection(tdm_ptr, TDM_DIALTONE_STREAM, ST3,
		    TDM_DIALTONE_BP_CH, dev_ch->channel);
	    } else {
		tdm_close_connection(tdm_ptr, ST7, dev_ch->channel);
		tdm_open_connection(tdm_ptr, TDM_DIALTONE_STREAM, ST7,
		    TDM_DIALTONE_BP_CH, dev_ch->channel);
	    }
	    return(TRUE);
	    break;
	default:
	    return(FALSE);
	    break;
    }
}

/*
 * tdm_connect_special(special_chan_t ch_type, ch_info_t *dev_ch)
 *	- TDM driver procedure used to make special connections
 *	(ie. tones) and default connctions.
 */
boolean
tdm_connect_special (special_chan_t ch_type, ch_info_t *dev_ch)
{
    switch (ch_type) {
	case DEFAULT_RX_TX:
	    return(tdm_default_rx_tx(dev_ch, FALSE));
	    break;
	case DEFAULT_TX:
	    return(tdm_default_tx(dev_ch, FALSE));
	    break;
	case DEFAULT_RX:
	    return(tdm_default_rx(dev_ch, FALSE));
	    break;
	case BUSYTONE_RX:
	    return(tdm_connect_busytone(dev_ch));
	    break;
	case DIALTONE_RX:
	    return(tdm_connect_dialtone(dev_ch));
	    break;
	default:
	    return(FALSE);
	    break;
    }
}

/*
 * static void
 * tdm_init_feature_card(ch_info_t *dev_ch, int board_id)
 *      - When called the dev_ch->slot field of the ch_info_t
 *       structure is initialized.  This routine should fill
 *       up that structure and make default connections for
 *       each channel.  Call this at board init only.
 */
static void
tdm_init_feature_card (ch_info_t *dev_ch, int board_id)
{

    switch (board_id) {
	case BRASIL_1E2T_MOTHERBOARD: /* Shouldn't get this, but... */
	    /*
	     * N.B. Currently, only distributed switching is
	     * implemented so the motherboard TDM remains tristated.
	     */
	    break;
	case BRASIL_1R2T_MOTHERBOARD: /* Shouldn't get this, but... */
	    /*
	     * N.B. Currently, only distributed switching is
	     * implemented so the motherboard TDM remains tristated.
	     */
	    break;
	case BRASIL_T1_DPRI_PLUS_CSU:
	    dev_ch->stream_type = TDM_PRI_STREAM;
	    dev_ch->unit = 0; /* Init PRI interface 0 stream */
	    for (dev_ch->channel = 0; dev_ch->channel < TDM_T1_MAX_CHANNELS;
		 dev_ch->channel++)
		tdm_default_rx_tx(dev_ch, TRUE);

	    dev_ch->unit = 1; /* Init PRI interface 0 stream */
	    for (dev_ch->channel = 0; dev_ch->channel < TDM_T1_MAX_CHANNELS;
		 dev_ch->channel++)
		tdm_default_rx_tx(dev_ch, TRUE);
	    break;
	case BRASIL_E1_DPRI_PLUS_MODEMS:
            dev_ch->stream_type = TDM_PRI_STREAM;
            dev_ch->unit = 0; /* Init PRI interface 0 stream */
            for (dev_ch->channel = 0; dev_ch->channel < TDM_E1_MAX_CHANNELS;
                 dev_ch->channel++)
                tdm_default_rx_tx(dev_ch, TRUE);
 
            dev_ch->unit = 1; /* Init PRI interface 0 stream */
            for (dev_ch->channel = 0; dev_ch->channel < TDM_E1_MAX_CHANNELS;
                 dev_ch->channel++)
                tdm_default_rx_tx(dev_ch, TRUE);
	    break;
	case BRASIL_24_ASYNC_PLUS_MODEMS:
	    dev_ch->stream_type = TDM_MODEM_STREAM;
	    dev_ch->unit = 0; /* Default it. */
	    dev_ch->stream = ST7; /* Other stream is slot based */
	    for (dev_ch->channel = 0; dev_ch->channel < TDM_T1_MAX_CHANNELS;
		 dev_ch->channel++)
		tdm_default_rx_tx(dev_ch, TRUE);
	    break;
	default:
	    errmsg(&msgsym(TDM_BADUNIT, TDM), dev_ch->slot, board_id);
	    break;
    }
    return;
}

/*
 * tdm_init_device(ch_info_t *dev_ch)
 *	- Init a tdm device.  In this case "init" equates
 *	to tristating all channels.
 */
static void
tdm_init_device (ch_info_t *dev_ch)
{
    int stream, channel;
 
    /*
     * Fill in ch_info_t and then tristate every channel
     */
    for (stream = 0; stream < TDM_MAX_STREAMS; stream++) {
	dev_ch->stream = stream;
	for (channel = 0; channel < TDM_MAX_CHANNELS; channel++) {
	    dev_ch->channel = channel;
	    dev_ch->unit = 0;
	    dev_ch->channel_group_used = FALSE;
	    tdm_tristate_channel(dev_ch);
	}
    }
    tdm_ode_enable(dev_ch);
}

/*
 * tdm_subsys_init(subsystype *subsys)
 *	- Discover and initialize all tdm devices.
 */
static void
tdm_subsys_init (subsystype *subsys)
{
    int slot;
    ch_info_t  *dev_ch;
    leveltype SR;
    ushort board_id;

#ifdef TDM_DEBUG
    SAVE_CALLER();
    buginf("\nInitialize all TDM devices; called from 0x%x", caller());
#endif

    /*
     * Initialize tdm portion of parser and debug facility
     */
    tdm_parser_init();
    tdm_debug_init();

    dev_ch = malloc(sizeof(ch_info_t));
    if (!dev_ch) {
	return;
    }

    SR = raise_interrupt_level(NETS_DISABLE);

    /*
     * Init the motherboard TDM device and verify
     */
    dev_ch->slot = TDM_MOTHERBOARD_SLOT;
    tdm_init_device(dev_ch);
    if (!tdm_verify_init(dev_ch)) {
	reset_interrupt_level(SR);
	free(dev_ch);
	return;
    }

    /*
     * Scan the box; if a board with a TDM is present,
     * init the TDM device.  Verify init.
     */
    for (slot = 0; slot < BRASIL_MAX_SLOTS; slot++) {
	if (!BRASIL_CARD_PRESENT(slot))
	    continue;
	dev_ch->slot = slot;
	tdm_init_device(dev_ch);
	if (!tdm_verify_init(dev_ch)) {
	    reset_interrupt_level(SR);
	    free(dev_ch);
	    return;
	}
    }

    /*
     * Make default TDM output connections.
     */
    for (slot = 0; slot < BRASIL_MAX_SLOTS; slot++) {
	if (!BRASIL_CARD_PRESENT(slot))
	    continue;

	dev_ch->slot = slot;
	board_id = BRASIL_BOARD_ID(slot);

	tdm_init_feature_card(dev_ch, board_id);
    }

    /*
     * Exit Stage Left
     */
    reset_interrupt_level(SR);
    free(dev_ch);
    return;
}

/*
 * TDM subsystem header
 */
#define TDM_MAJVERSION   1
#define TDM_MINVERSION   0
#define TDM_EDITVERSION  1
 
SUBSYS_HEADER(tdm, TDM_MAJVERSION, TDM_MINVERSION, TDM_EDITVERSION,
              tdm_subsys_init, SUBSYS_CLASS_DRIVER,
              "seq: mcn",
              NULL);
